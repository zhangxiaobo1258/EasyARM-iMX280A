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
#include <linux/input.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define KEY_CNT			1  							/* 设备数 */
#define KEYINPUT_NAME	"Keyinput"					/* 设备名称 */
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
struct keyinput_dev{
	dev_t devid;									/* 设备号 */
	struct cdev cdev;								/* cdev */
	struct class *class;							/* 类 */
	struct device *device;							/* 设备 */
	struct device_node *nd;							/* 设备节点 */
	struct timer_list timer;						/* 定义一个定时器 */
	struct irq_keydesc irqkeydesc[KEY_NUM];			/* 按键描述数组 */
	unsigned char curkeynum;						/* 当前的按键号 */
	struct input_dev *inputdev;						/* input结构体 */
};

struct keyinput_dev keyinputdev ;     				/* key input设备 */

/* 中断服务函数，开启定时器，延时10ms */
static irqreturn_t key_handler(int irq,void *dev_id)
{
	struct keyinput_dev *dev = (struct keyinput_dev *) dev_id;
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
	struct keyinput_dev *dev = from_timer(dev,timer,timer);

	num = dev->curkeynum;
	keydesc = &dev->irqkeydesc[num];
	value = gpio_get_value(keydesc->gpio);			/* 读取IO值 */
	printk("value is %d. \n\r",value);
	if(value == 0){									/* 按键按下 */
		/* 上报按键值 */
		input_report_key(dev->inputdev,keydesc->value,1);/* 1,按下 */
		printk("keydesc value is %d.\n\r",keydesc->value);
		input_sync(dev->inputdev);	
	}else{											/* 按键松开 */
		input_report_key(dev->inputdev,keydesc->value,0);
		input_sync(dev->inputdev);	
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
#if 0
static int key_open(struct inode *inode,struct file *filp )
{
	unsigned long flags;
	filp->private_data = &keyinputdev;							/*设置私有数据*/

	return 0;
}

static int key_release(struct inode *inode,struct file *filp )
{
	unsigned long flags;
	struct keyinput_dev *dev = filp->private_data;

	return 0;
}

int key_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	int ret;
	unsigned char keyvalue = 0;
	unsigned char releasekey = 0;
	struct keyinput_dev *dev =(struct keyinput_dev *) filp -> private_data;

	return 0;
}

static const struct file_operations key_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= key_open,
	.read			= key_read,						//read from device
};
#endif
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init keyinput_init(void)  
{
	int retvalue = 0;
	unsigned char  i;
	const char *str;
	struct property *proper;

	/* 获取设备树中的属性数据 */
	/* 1.获取设备节点：key */
	keyinputdev.nd = of_find_node_by_path("/keys");
	if(keyinputdev.nd == NULL){
		printk("key node can not found!\r\n");
		return -EINVAL;
	}else{
		printk("key node has been found!\r\n");
	}
	/* 2.获取设备树中gpio的属性 */
	for(i = 0;i < KEY_NUM;i++){
		keyinputdev.irqkeydesc[i].gpio = of_get_named_gpio(keyinputdev.nd,"keys-gpio",i);
		if(keyinputdev.irqkeydesc[i].gpio <0 ){
			printk("keys-gpio find failed !\r\n");
			return -EINVAL;
		}
		/* 3.初始化key所用的IO，并设置为中断模式 */
		memset(keyinputdev.irqkeydesc[i].name,0,sizeof(keyinputdev.irqkeydesc[i].name));
		sprintf(keyinputdev.irqkeydesc[i].name,"KEY%d",i+1);
		gpio_request(keyinputdev.irqkeydesc[i].gpio,keyinputdev.irqkeydesc[i].name);
		gpio_direction_input(keyinputdev.irqkeydesc[i].gpio);
		keyinputdev.irqkeydesc[i].irqnum = irq_of_parse_and_map(keyinputdev.nd,i);
#if 0
		keyinputdev.irqkeydesc[i].irqnum = gpio_to_irq(keyinputdev.irqkeydesc[i].gpio);
#endif
		printk("key%d:gpio = %d,irqnum = %d .\r\n",i,keyinputdev.irqkeydesc[i].gpio,keyinputdev.irqkeydesc[i].irqnum);

		/* 申请中断 */
		keyinputdev.irqkeydesc[i].handler = key_handler;
		keyinputdev.irqkeydesc[i].value  = i+2;					/* 使按键的值与include/uapi/linux/input-event-codes.h中的按键值对应，即对应.h文件中的KEY_1,KEY_2,KEY_3 */

		retvalue = request_irq(keyinputdev.irqkeydesc[i].irqnum,keyinputdev.irqkeydesc[i].handler,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,keyinputdev.irqkeydesc[i].name,&keyinputdev);
		if(retvalue < 0){
			printk("irq %d request failed!\r\n",keyinputdev.irqkeydesc[i].irqnum);
        	return -EFAULT;
		}
	}
	/* 创建定时器 */
	/*init_timers();
	key.timer.function = timer_function;*/
	timer_setup(&keyinputdev.timer,timer_function,0);

	/* 申请input_dev */
	keyinputdev.inputdev = input_allocate_device();
	keyinputdev.inputdev->name = KEYINPUT_NAME;
#if 0 
	/* 第一种设置事件和事件值的方法 */
	/* 初始化input_dev,设置产生哪些事件 */
	__set_bit(EV_KEY,keyinputdev.inputdev->evbit);/* 按键事件 */
	__set_bit(EV_REP,keyinputdev.inputdev->evbit);/* 重复事件 */

	/* 初始化input_dev,设置产生哪些按键 */
	__set_bit(KEY_0,keyinputdev.inputdev->keybit);

	/* 第二种设置事件和事件值的方法 */
	keyinputdev.inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	keyinputdev.inputdev->keybit[BIT_WORD(KEY_0)] |= BIT_MASK(KEY_0);

#endif
	/* 第三种设置事件和事件值的方法 */
	keyinputdev.inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	/*  input_set_capability函数一次只能设置一个具体事件，多个则需要多次设置 */
	input_set_capability(keyinputdev.inputdev,EV_KEY,KEY_1);	/* 设置产生那些按键值,需要与keyinputdev.irqkeydesc[i].value的值对应 */
	input_set_capability(keyinputdev.inputdev,EV_KEY,KEY_2);	/* 设置产生那些按键值 */
	input_set_capability(keyinputdev.inputdev,EV_KEY,KEY_3);	/* 设置产生那些按键值 */

	/*3.注册输入设备驱动*/
	retvalue = input_register_device(keyinputdev.inputdev);
	if( retvalue ){
		printk("register input device failed!\r\n");
		return retvalue;
	} 
    printk("key_init (2020.11.10).\n");

	return 0;
}  


static void __exit keyinput_exit(void)  
{  
	int ret = 0;
	unsigned char i;
	
	/* 删除定时器 */
	del_timer_sync(&keyinputdev.timer);
	/* 释放中断和对应的GPIO */
	for(i = 0;i < KEY_NUM;i++){
		free_irq(keyinputdev.irqkeydesc[i].irqnum,&keyinputdev);
		gpio_free(keyinputdev.irqkeydesc[i].gpio);
	}

	/* 注销输入设备 */
	input_unregister_device(keyinputdev.inputdev);
	input_free_device(keyinputdev.inputdev);

    printk("key_exit (2020.11.10).\n");  
}  
  

module_init(keyinput_init);  
module_exit(keyinput_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("key");  
MODULE_LICENSE("GPL v2"); 

