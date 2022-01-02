/*
cat24c01 Driver file,EasyARM_iMX280A board's cat24c01
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
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define I2CDEV_CNT 1		 /* 设备号长度 */
#define I2CDEV_NAME "dtsi2c" /* 设备名称 */

/* uda1380dev设备结构体 */
struct cat24c_dev
{
	dev_t devid;			  /* 设备号 */
	struct cdev cdev;		  /* cdev */
	struct class *class;	  /* 类 */
	struct device *device;	  /* 设备 */
	int major;				  /* 主设备号 */
	struct device_node *node; /* cat24c设备节点 */
	void *private_data;		  /* i2c中存储的数据 */
	char dat[DATANUM];		  /* cat24c02存储数据 */
};
struct cat24c_dev cat24cdev; /* cat24c设备 */

//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static int cat24c_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &cat24cdev; /* 设置私有数据 */
	return 0;
}

static int cat24c_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
* @description 	:读取I2C设备多个寄存器数据
* @param-dev	:I2C设备
* @para-reg		:要读取的寄存器首地址
* @para-val		:读取到的数据
* @para-len		:要读取的数据长度
* @return 		:操作结果
*/

static int cat24c_eeprom_read(struct cat24c_dev *dev, char *buf, void *val, size_t count)
{
	int ret;
	struct i2c_msg msg[2];
	struct i2c_client *client = (struct i2c_client *)dev->private_data;

	/* msg[0],发送要读取的首地址 */
	msg[0].addr = client->addr; /* I2C器件地址	*/
	msg[0].flags = 0;			/* 标记为发送数据 */
	msg[0].buf = &buf;			/* 读取的首地址 */
	msg[0].len = 1;				/* reg长度 */

	/* msg[1]，读取数据 */
	msg[1].addr = client->addr; /* I2C器件地址	*/
	msg[1].flags = I2C_M_RD;	/* 标记为读取数据 */
	msg[1].buf = val;			/* 读取数据缓冲区 */
	msg[1].len = count;			/* 读取的字节数 */

	ret = i2c_transfer(client->adapter, msg, 2); /* 向器件发送读取命令 */

	if (ret == 2){
		ret = 0;
	}else{
		ret = -EREMOTEIO;
	}

	return ret;
}

static ssize_t cat24c_read(struct file *filp,char __user *buf,size_t cnt,loff_t off)
{
	struct cat24c_dev *dev = (struct cat24c_dev *)filp->private_data;

	return cat24c_eeprom_read(dev,buf,off,cnt);
}

static ssize_t cat24c_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *off)
{
	return 0;
}

/*
* @description 	:向I2C设备多个寄存器写入数据
* @param-dev	:I2C设备
* @para-reg		:要写入的寄存器首地址
* @para-val		:要写入的数据缓冲区
* @para-len		:要写入的数据长度
* @return 		:操作结果
*/
static int cat24c_eeprom_write(struct cat24c_dev *dev, char reg, char *buf, char len)
{
	char b[256];
	struct i2c_msg msg;
	struct i2c_client *client = (struct i2c_client *)dev->private_data;

	b[0] = reg;				 /* 寄存器首地址 */
	memcpy(&b[1], buf, len); /* 将要发送的数据拷贝到数组b里面 */

	msg.addr = client->addr; /* I2C地址器件 */
	msg.flags = 0;			 /* 标记为写数据 */
	msg.buf = b;			 /* 要发送的缓冲区 */
	msg.len = len + 1;		 /* 要发送的数据长度 */

	return i2c_transfer(client->adapter, &msg, 1);
}

static long cat24c_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	unsigned int val;

	return 0;
}

static const struct file_operations cat24c_fops =
	{
		.owner = THIS_MODULE, //
		.open = cat24c_open,
		.read = cat24c_read,			//read from device
		.write = cat24c_write,			//send to device
		.unlocked_ioctl = cat24c_ioctl, //finish control command
		.release = cat24c_release,
};

/* i2c驱动的probe函数，当驱动与设备匹配以后，此函数就会执行*/
static int cat24c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	printk("cat24c driver and device has matched !\r\n");

	/* 1.注册字符设备驱动 */
	/* (1) 创建设备号 */
	if (cat24cdev.major)
	{
		cat24cdev.devid = MKDEV(cat24cdev.major, 0);
		register_chrdev_region(cat24cdev.devid, I2CDEV_CNT, I2CDEV_NAME);
	}
	else
	{
		alloc_chrdev_region(&cat24cdev.devid, 0, I2CDEV_CNT, I2CDEV_NAME);
		cat24cdev.major = MAJOR(cat24cdev.devid);
	}
	printk("cat24c02 major=%d.\r\n", cat24cdev.major);

	/*(2) 初始化cdev*/
	cat24cdev.cdev.owner = THIS_MODULE;
	cdev_init(&cat24cdev.cdev, &cat24c_fops);
	/*(3) 添加一个cdev*/
	cdev_add(&cat24cdev.cdev, cat24cdev.devid, I2CDEV_CNT);

	/*(4) 创建类*/
	cat24cdev.class = class_create(THIS_MODULE, I2CDEV_NAME);
	if (IS_ERR(cat24cdev.class))
	{
		return PTR_ERR(cat24cdev.class);
	}

	/*(5) 创建设备*/
	cat24cdev.device = device_create(cat24cdev.class, NULL, cat24cdev.devid, NULL, I2CDEV_NAME);
	if (IS_ERR(cat24cdev.device))
	{
		return PTR_ERR(cat24cdev.device);
	}

	cat24cdev.private_data = client;

	return 0;
}

static int cat24c_remove(struct i2c_client *client)
{
	/* 注销字符设备 */
	cdev_del(&cat24cdev.cdev); /* 删除cdev */
	unregister_chrdev_region(cat24cdev.devid, I2CDEV_CNT);
	/* device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt); */
	device_destroy(cat24cdev.class, cat24cdev.devid);
	class_destroy(cat24cdev.class);

	return 0;
}
static const struct i2c_device_id cat24c_ids[] = {
	{"cat24c02", 0},
	{} /* 必须留空 */
};
MODULE_DEVICE_TABLE(i2c, cat24c_ids);

/* 匹配列表 */
static const struct of_device_id cat24c_of_match[] = {
	{.compatible = "onsemi,cat24c02"}, /* 此处必须与设备树中的设备compatible一致 */
	{/* Sentinel */}};
/* 声明cat24c_of_match设备匹配表 */
MODULE_DEVICE_TABLE(of, cat24c_of_match);
/* i2c 驱动结构体 */
static struct i2c_driver cat24c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "cat24c02",				   /* 驱动名字，用于和设备匹配 */
		.of_match_table = cat24c_of_match, /* 设备树匹配表 */
	},
	.probe = cat24c_probe,
	.remove = cat24c_remove,
	.id_table = cat24c_ids, /* 这是一个包含所有支持名字的列表，只要注册的设备里有其中一个名字匹配就匹配成功 */
};

/* @description		:初始化CAT24C01
*  @param 			:无
*  @return 			:无
*/
#if 1
/* module_i2c_driver()函数可以用来代替cat24c_init()和cat24c_exit()以及module_init()和module_exit()以减少代码量 */
module_i2c_driver(cat24c_driver);
#else
static int __init cat24c_init(void)
{
	return i2c_add_driver(&cat24c_driver);
}

static void __exit cat24c_exit(void)
{
	i2c_del_driver(&cat24c_driver);
}

module_init(cat24c_init);
module_exit(cat24c_exit);
#endif

MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("cat24c");
MODULE_LICENSE("GPL v2");
