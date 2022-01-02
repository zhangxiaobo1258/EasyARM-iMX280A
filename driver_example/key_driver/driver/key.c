/*
GPIO Driver file,EasyARM_iMX280A board's keys and key
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
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define KEY_CNT			1  		/* 设备数 */
#define KEY_NAME		"Key"	/* 设备名称 */



/* key设备结构体 */
struct key_dev{
	dev_t devid;					/* 设备号 */
	struct cdev cdev;				/* cdev */
	struct class *class;			/* 类 */
	struct device *device;			/* 设备 */
	int major;						/* 主设备号 */
	int minor;						/* 次设备号 */
	struct device_node *nd;			/* 设备节点 */
	int key_gpio;					/* 按键所使用gpio管脚 */
	atomic_t keyvalue;				/* 原子操作 */
};

struct key_dev key ;     			/* key设备 */


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

	return 0;
}

int key_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	int ret = 0;
	unsigned char value;
	struct key_dev *dev = filp -> private_data;

	if(gpio_get_value(dev->key_gpio) == 0){			/* key1按下 */
		while(!gpio_get_value(dev->key_gpio));			/* 等待按键释放 */
		atomic_set(&dev->keyvalue,0xFF);
	}else{												/* 无效按键值 */
		atomic_set(&dev->keyvalue,0x00);
	}

	value = atomic_read(&dev->keyvalue);				/* 保存按键值 */
	ret = copy_to_user(buf,&value,sizeof(value)); 
	return ret;
}

static ssize_t key_write(struct file *filp,const char *buf,size_t cnt,loff_t *offt)
{
	struct key_dev *dev  = filp->private_data;

	return 0;
}

static long key_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int status;
	unsigned int val;
	struct key_dev *dev = file->private_data;

	return 0;
}


static const struct file_operations key_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= key_open,
	.read			= key_read,						//read from device
	.write			= key_write,					//send to device
	.unlocked_ioctl = key_ioctl,					//finish control command 
	.release		= key_release,
};
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init mykey_init(void)  
{
	int retvalue = 0;
	u32 val = 0;
	const char *str;
	struct property *proper;

	/* 初始化原子变量 */
	atomic_set(&key.keyvalue,0x00);	/* 初始化原子变量 */
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
	key.key_gpio = of_get_named_gpio(key.nd,"keys-gpio",0);
	if(key.key_gpio <0 ){
		printk("key1-gpio find failed !\r\n");
		return -EINVAL;
	}
	printk("key1-gpio num = %d\r\n",key.key_gpio);
	/* 3.请求IO */
	gpio_request(key.key_gpio,"key1");/* 请求IO */
	/* 4.设置key对应的gpio为输入 */
	retvalue = gpio_direction_input(key.key_gpio);
	if(retvalue < 0){
		printk("Set key failed!\r\n");
	}
	
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

    printk("key_init (2020.11.10).\n");

	return 0;
}  


static void __exit mykey_exit(void)  
{  
	int ret = 0;

	gpio_free(key.key_gpio);/* 释放申请的GPIO */
	/* 注销字符设备 */
	cdev_del(&key.cdev);/* 删除cdev */
	unregister_chrdev_region(key.devid,KEY_CNT);
	/* device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt); */
	device_destroy(key.class,key.devid);
	class_destroy(key.class);

    printk("key_exit (2020.11.10).\n");  
}  
  

module_init(mykey_init);  
module_exit(mykey_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("key");  
MODULE_LICENSE("GPL v2"); 

