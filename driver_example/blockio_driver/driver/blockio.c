/*
GPIO Driver file,EasyARM_iMX280A board's keys interrupt 
*/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <linux/semaphore.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define KEY_CNT			1  							/* 设备数 */
#define KEY_NAME		"blockio"						/* 设备名称 */
#define KEY_NUM			3							/* 按键数量 */

/* 中断IO描述结构体 */
struct irq_keydesc
{
	int gpio;										/* GPIO */				
	int irqnum;										/* 中断号 */
	unsigned char value;							/* 按键对应的键值 */
	char name[10];									/* 名字 */
	irqreturn_t (*handler)(int , void *);			/* 中断服务函数 */
};


/* key设备结构体 */
struct key_dev{
	dev_t devid;									/* 设备号 */
	struct cdev cdev;								/* cdev */
	struct class *class;							/* 类 */
	struct device *device;							/* 设备 */
	int major;										/* 主设备号 */
	int minor;										/* 次设备号 */
	struct device_node *nd;							/* 设备节点 */
	atomic_t keyvalue;								/* 有效按键值 */
	atomic_t releasekey;							/* 标记是否完成一次完成的按键 */
	struct timer_list timer;						/* 定义一个定时器 */
	struct irq_keydesc irqkeydesc[KEY_NUM];			/* 按键描述数组 */
	unsigned char curkeynum;						/* 当前的按键号 */
	unsigned char flag;								/* 驱动卸载标志 */

	wait_queue_head_t r_wait;						/* 读等待队列头 */
};

struct key_dev key ;     							/* key设备 */

/* 中断服务函数，开启定时器，延时10ms */
static irqreturn_t key_handler(int irq,void *dev_id)
{
	struct key_dev *dev = (struct key_dev *) dev_id;
	switch(irq){
		case 70:			/* 根据中断号判断哪个按键 */
			dev->curkeynum = 0;
			break;
		case 87:
			dev->curkeynum = 1;
			break;
		case 88:
			dev->curkeynum = 2;
			break;
		default:
			break;
	}
	/*dev->timer.expires = (volatile long)dev_id;*/
	mod_timer(&dev->timer,jiffies+msecs_to_jiffies(10));
	return IRQ_RETVAL(IRQ_HANDLED);
}
/* 定时器中断服务函数 */
void timer_function(struct timer_list *timer)
{
	unsigned char value;
	unsigned char num;
	struct irq_keydesc *keydesc;
	struct key_dev *dev = from_timer(dev,timer,timer);

	num = dev->curkeynum;
	keydesc = &dev->irqkeydesc[num];
	value = gpio_get_value(keydesc->gpio);			/* 读取IO值,返回值为GPIO对应寄存器的值 */
	if(value == 0){									/* 按键按下 */
		atomic_set(&dev->keyvalue,keydesc->value);	
	}else{											/* 按键松开 */
		atomic_set(&dev->keyvalue,0x80 |keydesc->value);	
		atomic_set(&dev->releasekey,1);			/* 标记松开按键 */
	}

	/* 唤醒进程 */
	if(atomic_read(&dev->releasekey)){				/* 完成一次按键过程 */
		/* wake_up(&dev->r_wait); */
		wake_up_interruptible(&dev->r_wait);
	}
}
//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static int key_open(struct inode *inode,struct file *filp )
{
	unsigned long flags;
	filp->private_data = &key;							/*设置私有数据*/

	return 0;
}

static int key_release(struct inode *inode,struct file *filp )
{
	unsigned long flags;
	struct key_dev *dev = filp->private_data;
	unsigned char i;
	if(key.flag == 0){
		/* 删除定时器 */
		del_timer_sync(&key.timer);
		/* 释放中断和对应的GPIO */
		for(i = 0;i < KEY_NUM;i++){
			free_irq(key.irqkeydesc[i].irqnum,&key);
			gpio_free(key.irqkeydesc[i].gpio);
		}
		key.flag = 1;
	}

	return 0;
}

int key_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	int ret;
	unsigned char keyvalue = 0;
	unsigned char releasekey = 0;
	struct key_dev *dev =(struct key_dev *) filp -> private_data;

#if 0
	/* 加入等待队列，等待被唤醒，也就是有按键按下 */
	ret = wait_event_interruptible(dev->r_wait,atomic_read(&dev->releasekey));
	if(ret){
		set_current_state(TASK_RUNNING);				/* 设置任务为运行态 */
		remove_wait_queue(&dev->r_wait,&wait);			/* 将等待队列移除 */
		return ret;
	} 
#endif

	DECLARE_WAITQUEUE(wait,current);					/* 定义一个等待队列 */
	if(atomic_read(&dev->releasekey) == 0 ){			/* 没有按键按下 */
		add_wait_queue(&dev->r_wait,&wait);				/* 添加到等待队列头 */
		__set_current_state(TASK_INTERRUPTIBLE);		/* 设置任务状态 */
		schedule();										/* 进行一次任务切换 */
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			set_current_state(TASK_RUNNING);			/* 设置任务为运行态 */
			remove_wait_queue(&dev->r_wait,&wait);		/* 将等待队列移除 */
			return ret;
		}
		__set_current_state(TASK_RUNNING);				/* 设置为运行状态 */
		remove_wait_queue(&dev->r_wait,&wait);			/* 将等待队列移除 */
	}

	keyvalue = atomic_read(&dev->keyvalue);				/* 保存按键值 */
	releasekey = atomic_read(&dev->releasekey);			/* 保存按键值 */
	
	if(releasekey){										/* 有按键按下 */							
		if(keyvalue & 0x80){
			ret = copy_to_user(buf,&keyvalue,sizeof(keyvalue)); 
		}else{
			return -EINVAL;
		}
		atomic_set(&dev->releasekey,0);					/* 按键标志清零 */
	}else{
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations key_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= key_open,
	.read			= key_read,						//read from device
	.release		= key_release,	
};
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init keyirq_init(void)  
{
	int retvalue = 0;
	unsigned char  i;
	const char *str;
	struct property *proper;

	/* 初始化原子变量 */
	atomic_set(&key.keyvalue,0xFF);	/* 初始化原子变量 */
	atomic_set(&key.releasekey,0);
	/* 获取设备树中的属性数据 */
	/* 1.获取设备节点：key */
	key.nd = of_find_node_by_path("/keys");
	if(key.nd == NULL){
		printk("key node can not found!\r\n");
		return -EINVAL;
	}else{
		printk("key node has been found!\r\n");
	}
	/* 2.获取设备树中gpio的属性 */
	for(i = 0;i < KEY_NUM;i++){
		key.irqkeydesc[i].gpio = of_get_named_gpio(key.nd,"keys-gpio",i);
		if(key.irqkeydesc[i].gpio <0 ){
			printk("keys-gpio find failed !\r\n");
			return -EINVAL;
		}
		printk("key%d-gpio num = %d\r\n",i,key.irqkeydesc[i].gpio);
	}

	/* 3.初始化key所用的IO，并设置为中断模式 */
	for(i = 0;i < KEY_NUM;i++){
		memset(key.irqkeydesc[i].name,0,sizeof(key.irqkeydesc[i].name));
		sprintf(key.irqkeydesc[i].name,"KEY%d",i+1);
		gpio_request(key.irqkeydesc[i].gpio,key.irqkeydesc[i].name);
		gpio_direction_input(key.irqkeydesc[i].gpio);
		key.irqkeydesc[i].irqnum = irq_of_parse_and_map(key.nd,i);
#if 0
		key.irqkeydesc[i].irqnum = gpio_to_irq(key.irqkeydesc[i].gpio);
#endif
		printk("key%d:gpio = %d,irqnum = %d .\r\n",i,key.irqkeydesc[i].gpio,key.irqkeydesc[i].irqnum);
	}

	/* 申请中断 */
	for(i = 0;i < KEY_NUM; i++ ){
		key.irqkeydesc[i].handler = key_handler;
		key.irqkeydesc[i].value  = i;
		retvalue = request_irq(key.irqkeydesc[i].irqnum,key.irqkeydesc[i].handler,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,key.irqkeydesc[i].name,&key);
		if(retvalue < 0){
			printk("irq %d request failed!\r\n",key.irqkeydesc[i].irqnum);
			return -EFAULT;
		}
	}
	/* 创建定时器 */
	/*init_timers();
	key.timer.function = timer_function;*/
	timer_setup(&key.timer,timer_function,0);

	/*3.注册字符设备驱动*/
	/*(1) 创建设备号*/
	if(key.major){
		key.devid = MKDEV(key.major,0);
		register_chrdev_region(key.devid,KEY_CNT,KEY_NAME);
	}else{
		alloc_chrdev_region(&key.devid,0,KEY_CNT,KEY_NAME);
		key.major = MAJOR(key.devid);
		key.minor = MINOR(key.devid);
	}
	printk("Key major=%d,minor=%d\r\n",key.major,key.minor);

	/*(2) 初始化cdev*/
	key.cdev.owner = THIS_MODULE;
	cdev_init(&key.cdev,&key_fops);
	/*(3) 添加一个cdev*/
	cdev_add(&key.cdev,key.devid,KEY_CNT);

	/*(4) 创建类*/
	key.class = class_create(THIS_MODULE,KEY_NAME);
	if(IS_ERR(key.class)){
		return PTR_ERR(key.class);
	}

	/*(5) 创建设备*/
	key.device = device_create(key.class,NULL,key.devid,NULL,KEY_NAME);
	if(IS_ERR(key.device)){
		return PTR_ERR(key.device);
	}

	key.flag = 0;
	/* 初始化等待队列头 */
	init_waitqueue_head(&key.r_wait);

    printk("key_init (2020.11.10).\n");

	return 0;
}  


static void __exit keyirq_exit(void)  
{  
	int ret = 0;
	unsigned char i;
	
	if(key.flag == 0){
		/* 删除定时器 */
		del_timer_sync(&key.timer);
		/* 释放中断和对应的GPIO */
		for(i = 0;i < KEY_NUM;i++){
			free_irq(key.irqkeydesc[i].irqnum,&key);
			gpio_free(key.irqkeydesc[i].gpio);
		}
		key.flag = 1;
	}

	/* 注销字符设备 */
	cdev_del(&key.cdev);/* 删除cdev */
	unregister_chrdev_region(key.devid,KEY_CNT);
	/* device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt); */
	device_destroy(key.class,key.devid);
	class_destroy(key.class);

    printk("key_exit (2020.11.10).\n");  
}  
  

module_init(keyirq_init);  
module_exit(keyirq_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("key");  
MODULE_LICENSE("GPL v2"); 

