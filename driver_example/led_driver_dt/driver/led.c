/*
GPIO Driver file,EasyARM_iMX280A board's beeps and beep
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
#include <linux/uaccess.h>
#include <asm/io.h>

#define BEEP_CNT			1  	/*设备数*/
#define BEEP_NAME		"beep"	/*设备名称*/


/*
BEEP:GPIO2_6
RUN :GPIO2_7
ERR	:GPIO3_20
*/

/*beep设备结构体*/
struct beep_dev{
	dev_t devid;					/* 设备号 */
	struct cdev cdev;				/* cdev */
	struct class *class;			/* 类 */
	struct device *device;			/* 设备 */
	int major;						/* 主设备号 */
	int minor;						/* 次设备号 */
	struct device_node *nd;			/* 设备节点 */
	int beep_gpio;					/* 蜂鸣其所使用gpio管脚 */
};

struct beep_dev beep ;     			/* beep设备 */


//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static int beep_open(struct inode *inode,struct file *filp )
{
	filp->private_data = &beep;/*设置私有数据*/
	return 0;
}

static int beep_release(struct inode *inode,struct file *filp )
{
	return 0;
}

int beep_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	return 0;
}

static ssize_t beep_write(struct file *filp,const char *buf,size_t cnt,loff_t *offt)
{
	struct beep_dev *dev  = filp->private_data;
	return 0;
}

static long beep_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int status;
	unsigned int val;
	struct beep_dev *dev = file->private_data;

	printk("################# beep_ioctl cmd = 0x%x dat = 0x%x\n", cmd, arg);
	

	switch (cmd) 
	{	
		case 'b':			
			if(arg == 0x30){
				gpio_set_value(dev->beep_gpio,1);//打开蜂鸣器
				printk("Beep is on.\r\n");
			}
			else{
				gpio_set_value(dev->beep_gpio,0);//将GPIO2_6输出高电平
				printk("Beep is off.\n");
			}
			return 0;
		default:
			printk("BEEP error!\n");
			return -ENOTTY;
	}
	
	return 0;
}


static const struct file_operations beep_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= beep_open,
	.read			= beep_read,						//read from device
	.write			= beep_write,					//send to device
	.unlocked_ioctl = beep_ioctl,					//finish control command 
	.release		= beep_release,
};
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init beep_init(void)  
{
	int retvalue = 0;
	u32 val = 0;
	const char *str;
	struct property *proper;


	/*获取设备树中的属性数据*/
	/*1.获取设备节点：beep*/
	beep.nd = of_find_node_by_path("/beep");
	if(beep.nd == NULL){
		printk("beep node can not found!\r\n");
		return -EINVAL;
	}else{
		printk("beep node has been found!\r\n");
	}
	/*2.获取设备树中gpio的属性*/
	beep.beep_gpio = of_get_named_gpio(beep.nd,"beep-gpio",0);
	if(beep.beep_gpio <0 ){
		printk("beep-gpio find failedbeep!\r\n");
		return -EINVAL;
	}
	printk("beep-gpio num = %d\r\n",beep.beep_gpio);
	/*3.设置beep对应的gpio为输出*/
	retvalue = gpio_direction_output(beep.beep_gpio,1);
	if(retvalue < 0){
		printk("Set Beep failedbeep!\r\n");
	}
	
	/*3.注册字符设备驱动*/
	/*(1) 创建设备号*/
	if(beep.major){
		beep.devid = MKDEV(beep.major,0);
		register_chrdev_region(beep.devid,BEEP_CNT,BEEP_NAME);
	}else{
		alloc_chrdev_region(&beep.devid,0,BEEP_CNT,BEEP_NAME);
		beep.major = MAJOR(beep.devid);
		beep.minor = MINOR(beep.devid);
	}
	printk("Beep major=%d,minor=%d\r\n",beep.major,beep.minor);

	/*(2) 初始化cdev*/
	beep.cdev.owner = THIS_MODULE;
	cdev_init(&beep.cdev,&beep_fops);
	/*(3) 添加一个cdev*/
	cdev_add(&beep.cdev,beep.devid,BEEP_CNT);

	/*(4) 创建类*/
	beep.class = class_create(THIS_MODULE,BEEP_NAME);
	if(IS_ERR(beep.class)){
		return PTR_ERR(beep.class);
	}

	/*(5) 创建设备*/
	beep.device = device_create(beep.class,NULL,beep.devid,NULL,BEEP_NAME);
	if(IS_ERR(beep.device)){
		return PTR_ERR(beep.device);
	}

    printk("beep_init (2020.10.23).\n");

	return 0;
}  


static void __exit beep_exit(void)  
{  
	int ret = 0;

	/* 注销字符设备 */
	cdev_del(&beep.cdev);/*删除cdev*/
	unregister_chrdev_region(beep.devid,BEEP_CNT);
	/*device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt);*/
	device_destroy(beep.class,beep.devid);
	class_destroy(beep.class);

    printk("beep_exit (2020.10.23).\n");  
}  
  

module_init(beep_init);  
module_exit(beep_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("beep");  
MODULE_LICENSE("GPL v2"); 

