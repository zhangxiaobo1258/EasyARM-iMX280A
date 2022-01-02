/* Linux 内核定时器实验 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <asm/io.h>

#define TIMER_CNT           1                   /* 设备号个数 */
#define TIMER_NAME          "timer"             /* 设备名称 */
#define CLOSE_CMD           1                   /* 关闭定时器 */
#define OPEN_CMD            2                   /* 打开定时器 */
#define SETPERIOD_CMD       3                   /* 设置定时器周期 */
#define LEDON               1                   /* 打开LED */
#define LEDOFF              0                   /* 关闭LED */

/* Timer设备结构体定义 */
struct timer_dev{
    dev_t devid;                                /* 设备号 */
    struct cdev cdev;                           /* cdev */
    struct class *class;                        /* 类 */
    struct device *device;                      /* 设备 */
    int major;                                  /* 主设备号 */
    int minor;                                  /* 次设备号 */
    struct device_node *nd;                     /* 设备节点 */
    int led_gpio;                               /* LED所使用的GPIO编号 */
    int timerperiod;                            /* 定时周期，单位为ms */
    struct timer_list timer;                    /* 定义一个定时器 */
    spinlock_t lock;                              /* 定义自旋锁 */
};

struct timer_dev timerdev;                      /* timer设备 */

/* 打开设备函数 */
static int timer_open(struct inode *inode,struct file *filp)
{
    filp->private_data = &timerdev;             /* 设置私有数据 */

    return 0;
}
/* 关闭设备函数 */
static int timer_release(struct inode *inode,struct file *filp )
{
	gpio_set_value(timerdev.led_gpio,1);                /* 卸载驱动的时候关闭LED */
    del_timer_sync(&timerdev.timer);                    /* 删除timer */
    return 0;
}
/* ioctl函数 */
static long timer_nolocked_ioctl(struct file *filp,unsigned int cmd , unsigned  long arg)
{
    struct timer_dev *dev = (struct  timer_dev *)filp->private_data;
    int timerperiod;
    unsigned long flags;

    switch (cmd)
    {
    case CLOSE_CMD :                                /* 关闭定时器 */
        del_timer_sync(&dev->timer);                /* 等待其他进程使用完定时器再删除 */
        break; 
    case OPEN_CMD :                                 /* 打开定时器 */
        spin_lock_irqsave(&dev->lock,flags);        /* 关闭中断 */
        timerperiod = dev->timerperiod;             /* 设置定时器周期为默认周期 */
        spin_unlock_irqrestore(&dev->lock,flags);   /* 恢复中断 */
        /* 打开定时器并修改定时器周期，msecs_to_jiffies()函数是将ms转换为处理器的节拍数 */
        mod_timer(&dev->timer,jiffies + msecs_to_jiffies(timerperiod));
        break;
    case SETPERIOD_CMD :                            /* 打开定时器 */
        spin_lock_irqsave(&dev->lock,flags);        /* 关闭中断 */
        timerdev.timerperiod = arg;               /* 设置定时器周期 */
        spin_unlock_irqrestore(&dev->lock,flags);   /* 恢复中断 */
        printk("Timer expires is %ld.\n\r",timerdev.timerperiod);
        /* 打开定时器并设置定时器周期，msecs_to_jiffies()函数是将ms转换为处理器的节拍数 */
        mod_timer(&dev->timer,jiffies + msecs_to_jiffies(timerdev.timerperiod));
        printk("Timer setted finished!\n\r");
        
        break;
    default:
        break;
    }
    return  0;
}
/* 设备操作函数 */
static struct file_operations timers_fops = {
    .owner          = THIS_MODULE,
    .open           = timer_open,
    .unlocked_ioctl = timer_nolocked_ioctl, 
    .release        = timer_release,

};

/* 定时器回调函数,即定时器中断实现 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
void timer_function(unsigned long arg)
{
    struct timer_dev *dev = (struct timer_dev *)arg;
#else
void timer_function(struct timer_list *timer)
{
    struct timer_dev *dev = from_timer(dev,timer,timer); 
#endif
    static int sta = 1;
    int timerperiod;
    unsigned long flags;

    sta = !sta;                                         /* 每次都取反，实现LED反转 */
    gpio_set_value(dev->led_gpio,sta);
    printk("System is interrupted!\r\n");

    /* 由于内核定时器不是循环定时器，执行一次后需要重启定时器，如果只需要使用一次定时器，屏蔽下面的代码即可 */
    spin_lock_irqsave(&dev->lock,flags);        /* 关闭中断 */
    printk("Close interrupt!\r\n");
    timerperiod = dev->timerperiod;           /* 设置定时器周期为默认周期 */
    printk("Timer period is %ld !\r\n",timerperiod);
    spin_unlock_irqrestore(&dev->lock,flags);   /* 恢复中断 */
    printk("Start interrupt !\r\n");
    /* 打开定时器并修改定时器周期，msecs_to_jiffies()函数是将ms转换为处理器的节拍数 */
    mod_timer(&dev->timer,jiffies + msecs_to_jiffies(timerperiod));
    printk("Timer restart finished !\r\n");
}

/* 驱动入口函数 */
static int __init timer_init(void)
{
    int ret = 0;
    /* 初始化自旋锁 */
    spin_lock_init(&timerdev.lock);

    /* 1.初始化timer,设置定时器处理函数，因为未设置定时器周期，故不会激活定时器 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    init_timers(); 
    timerdev.timer.function = timer_function;          /* 需要定时处理的函数指针 */
    timerdev.timer.data = (unsigned long) &timerdev;
#else
    timerdev.timerperiod = 1000;                            /* 默认周期为1s */
    timer_setup(&timerdev.timer,timer_function,0);          /* 初始化定时器 */
    
    //add_timer(&timerdev.timer);                           /* 启动定时器 */
    printk("timer init is  finished!.\n");
#endif
    /* 2.获取设备树中led的属性数据 */
    timerdev.nd = of_find_node_by_path("/leds");
    if(timerdev.nd == NULL){
        printk("leds node find failed!\n\r");
        return -EINVAL;
    }
    timerdev.led_gpio = of_get_named_gpio(timerdev.nd,"leds-gpio",0);
    if(timerdev.led_gpio<0){
        printk("led-gpio find failed!\r\n");
        return -EINVAL;
    }
    gpio_request(timerdev.led_gpio,"led");
    ret = gpio_direction_output(timerdev.led_gpio,1);
    if(ret < 0){
        printk("Set led-gpio failed!\r\n");
    }

    printk("gpio init is  finished!\n");

    /* 注册字符设备驱动 */
    /* 3.创建设备号 */
    if(timerdev.major){                         /* 定义了设备号 */
        timerdev.devid = MKDEV(timerdev.major,0);
        register_chrdev_region(timerdev.devid,TIMER_CNT,TIMER_NAME);
    }else{
        alloc_chrdev_region(&timerdev.devid,0,TIMER_CNT,TIMER_NAME);
        timerdev.major = MAJOR(timerdev.devid); /* 获取主设备号 */
        timerdev.minor = MINOR(timerdev.devid); /* 获取次设备号 */
        printk("Set major!\r\n");
    }
    printk("Timer major = %d,minor= %d\r\n",timerdev.major,timerdev.minor);

    /* 4.初始化cdev */
    timerdev.cdev.owner = THIS_MODULE;
    cdev_init(&timerdev.cdev,&timers_fops);    
    printk("Cdev init is  finished!.\n");
    
    /* 5.添加一个cdev */
    cdev_add(&timerdev.cdev,timerdev.devid,TIMER_CNT);
    printk("Cdev add is  finished!.\n");

    /* 6.创建类 */
    timerdev.class = class_create(THIS_MODULE,TIMER_NAME);
    printk("Class is  finished!.\n");
    if(IS_ERR(timerdev.class)){
        return PTR_ERR(timerdev.class);
    }

    /* 7.创建设备 */
    timerdev.device = device_create(timerdev.class,NULL,timerdev.devid,NULL,TIMER_NAME);
    printk("device is  finished!.\n");
    if(IS_ERR(timerdev.device)){
        return PTR_ERR(timerdev.device);
    }

    printk("Timer_init (2020.11.22).\n");

    return 0;

}

static void __exit timer_exit(void)
{
    gpio_set_value(timerdev.led_gpio,1);                /* 卸载驱动的时候关闭LED */
    del_timer_sync(&timerdev.timer);                    /* 删除timer */

    /*注销字符设备驱动*/
    cdev_del(&timerdev.cdev);
    unregister_chrdev_region(timerdev.devid,TIMER_CNT);

    device_destroy(timerdev.class,timerdev.devid);
    class_destroy(timerdev.class);
    printk("Timer_exit (2020.11.22).\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("timer");  
MODULE_LICENSE("GPL");