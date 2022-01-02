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
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/spi/spi.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define SPILED_CNT			1  			/* 设备数 */
#define SPILED_NAME			"spiled"	/* 设备名称 */



/* cat24c02设备结构体 */
struct cat24c_dev{
	dev_t devid;					/* 设备号 */
	struct cdev cdev;				/* cdev */
	struct class *class;			/* 类 */
	struct device *device;			/* 设备 */
	struct device_node *nd;			/* 设备节点 */
	int major;						/* 主设备号 */
	void *private_data;				/* 私有数据 */
	unsigned char read_data;		/* eeprom读数据缓冲区 */
	unsigned char write_data;		/* eeprom写数据缓冲区 */
};

struct cat24c_dev cat24c01 ;     	/* cat24c02设备 */


//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static int cat24c_open(struct inode *inode,struct file *filp )
{
	filp->private_data = &cat24c01;/*设置私有数据*/
	return 0;
}

static int cat24c_release(struct inode *inode,struct file *filp )
{
	return 0;
}

int cat24c_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	
	return 0;
}

static ssize_t cat24c_write(struct file *filp,const char *buf,size_t cnt,loff_t *offt)
{
	struct beep_dev *dev  = filp->private_data;
	return 0;
}


static const struct file_operations cat24c_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= cat24c_open,
	.read			= cat24c_read,						//read from device
	.write			= cat24c_write,					//send to device
	.release		= cat24c_release,
};
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init cat24c_init(void)  
{
	int retvalue = 0;
	u32 val = 0;
	const char *str;
	struct property *proper;


	/*获取设备树中的属性数据*/
	/*1.获取设备节点：beep*/
	cat24c01.nd = of_find_node_by_path("/i2c1");
	if(cat24c01.nd == NULL){
		printk("cat24c node can not found!\r\n");
		return -EINVAL;
	}else{
		printk("cat24c node has been found!\r\n");
	}
	
	/*2.注册字符设备驱动*/
	/*(1) 创建设备号*/
	if(cat24c01.major){
		cat24c01.devid = MKDEV(cat24c01.major,0);
		register_chrdev_region(cat24c01.devid,EEPROM_CNT,EEPROM_NAME);
	}else{
		alloc_chrdev_region(cat24c01.devid,0,EEPROM_CNT,EEPROM_NAME);
		cat24c01.major = MAJOR(cat24c01.devid);
	}
	
	printk("cat24c01 major=%d.\r\n",cat24c01.major);

	/*(2) 初始化cdev*/
	cat24c01.cdev.owner = THIS_MODULE;
	cdev_init(&cat24c01.cdev,&cat24c_fops);
	/*(3) 添加一个cdev*/
	cdev_add(&cat24c01.cdev,cat24c01.devid,EEPROM_CNT);

	/*(4) 创建类*/
	cat24c01.class = class_create(THIS_MODULE,EEPROM_NAME);
	if(IS_ERR(cat24c01.class)){
		return PTR_ERR(cat24c01.class);
	}

	/*(5) 创建设备*/
	cat24c01.device = device_create(cat24c01.class,NULL,cat24c01.devid,NULL,EEPROM_NAME);
	if(IS_ERR(cat24c01.device)){
		return PTR_ERR(cat24c01.device);
	}

    printk("cat24c_init (2020.10.23).\n");

	return 0;
}  


static void __exit cat24c_exit(void)  
{  
	int ret = 0;

	/* 注销字符设备 */
	cdev_del(&cat24c01.cdev);/*删除cdev*/
	unregister_chrdev_region(cat24c01.devid,EEPROM_CNT);
	/*device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt);*/
	device_destroy(cat24c01.class,cat24c01.devid);
	class_destroy(cat24c01.class);

    printk("cat24c_exit (2020.10.23).\n");  
}  
  

module_init(cat24c_init);  
module_exit(cat24c_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("eeprom");  
MODULE_LICENSE("GPL v2"); 

