/*
GPIO Driver file,EasyARM_iMX280A board's leds and beep
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
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define LEDDEV_CNT		1  				/*设备号长度*/
#define LEDDEV_NAME		"dtsplatled"	/*设备名称*/
#define LEDOFF			0				/*设备名称*/
#define LEDON			1				/*设备名称*/

/* leddev设备结构体 */
struct leddev_dev{
	dev_t devid;					/* 设备号 */
	struct cdev cdev;				/* cdev */
	struct class *class;			/* 类 */
	struct device *device;			/* 设备 */
	int major;						/* 主设备号 */
	struct device_node *node;		/* LED设备节点 */
	int led0;						/* LED的GPIO标号 */
};
struct leddev_dev leddev;			/* led设备 */

//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static int led_open(struct inode *inode,struct file *filp )
{
	filp->private_data = &leddev;					/* 设置私有数据 */	
	return 0;
}

static int led_release(struct inode *inode,struct file *filp )
{
	return 0;
}

int led_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
	return 0;
}

static ssize_t led_write(struct file *filp,const char *buf,size_t cnt,loff_t *offt)
{
	return 0;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	unsigned int val;

	/*status = copy_from_user(cmd_in,&cmd,1);
	if(status < 0){
		printk("Kernel octl is failed!\r\n");
		return -EFAULT;
	}

	status = copy_from_user(data_in,&arg,1);
	if(status <
		printk("Kernel octl is failed!\r\n");
		return -EFAULT;
	}*/


	printk("################# led_ioctl cmd = 0x%x dat = 0x%x\n", cmd, arg);
	if(cmd == 'l'){
		if (arg == '0'){
			gpio_set_value(leddev.led0, 0);
		}else if (arg == '1'){
			gpio_set_value(leddev.led0, 1);
		}
	}
	
	return 0;
	
}


static const struct file_operations led_fops = 
{
	.owner			= THIS_MODULE,					//
	.open			= led_open,
	.read			= led_read,						//read from device
	.write			= led_write,					//send to device
	.unlocked_ioctl = led_ioctl,					//finish control command 
	.release		= led_release,
};

/* platform 驱动的probe函数，当驱动与设备匹配以后，此函数就会执行*/
static int led_probe(struct platform_device *dev)
{

	printk("led driver and device has matched !\r\n");
	
	/* 1.注册字符设备驱动 */
	/* (1) 创建设备号 */
	if(leddev.major){
		leddev.devid = MKDEV(leddev.major,0);
		register_chrdev_region(leddev.devid,LEDDEV_CNT,LEDDEV_NAME);
	}else{
		alloc_chrdev_region(&leddev.devid,0,LEDDEV_CNT,LEDDEV_NAME);
		leddev.major = MAJOR(leddev.devid);
	}
	printk("LED major=%d.\r\n",leddev.major);

	/*(2) 初始化cdev*/
	leddev.cdev.owner = THIS_MODULE;
	cdev_init(&leddev.cdev,&led_fops);
	/*(3) 添加一个cdev*/
	cdev_add(&leddev.cdev,leddev.devid,LEDDEV_CNT);

	/*(4) 创建类*/
	leddev.class = class_create(THIS_MODULE,LEDDEV_NAME);
	if(IS_ERR(leddev.class)){
		return PTR_ERR(leddev.class);
	}

	/*(5) 创建设备*/
	leddev.device = device_create(leddev.class,NULL,leddev.devid,NULL,LEDDEV_NAME);
	if(IS_ERR(leddev.device)){
		return PTR_ERR(leddev.device);
	}

	/* 2.初始化LED */
	leddev.node = of_find_node_by_path("/leds");
	if(leddev.node == NULL){
		printk("gpioled node not find!\r\n" );
		return -EINVAL;
	}
	printk("gpioled node has found!\r\n" );
	leddev.led0 = of_get_named_gpio(leddev.node,"leds-gpio",0);
	if(leddev.led0 < 0){
		printk("Can't get led1-gpio \r\n");
		return -EINVAL;
	}
	printk("led-gpio num = %d\r\n",leddev.led0);

	gpio_request(leddev.led0,"led0");
	gpio_direction_output(leddev.led0,1);	/* 设置为输出，默认高电平 */

	return 0;
}

static int led_remove(struct platform_device *dev)
{
	/* 卸载驱动的时候关闭LED */
	gpio_set_value(leddev.led0,1);
	/* 注销字符设备 */
	cdev_del(&leddev.cdev);/* 删除cdev */
	unregister_chrdev_region(leddev.devid,LEDDEV_CNT);
	/* device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt); */
	device_destroy(leddev.class,leddev.devid);
	class_destroy(leddev.class);

	return 0;
}
/* 匹配列表 */
static const struct of_device_id led_of_match[] = {
	{ .compatible	= "gpio-leds"	},				/* 此处必须与设备树中的设备compatible一致 */
	{ /* Sentinel */}
};
/* 声明led_of_match设备匹配表 */
MODULE_DEVICE_TABLE(of,led_of_match);
/* platform 驱动结构体 */
static struct platform_driver led_platform_driver = {
	.driver = {
		.name 			= "mxs-leds",				/* 驱动名字，用于和设备匹配 */
		.of_match_table = led_of_match,				/* 设备树匹配表 */ 
	},
	.probe  = led_probe,
	.remove = led_remove,
};

/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init led_init(void)  
{
	return platform_driver_register(&led_platform_driver);
}  


static void __exit led_exit(void)  
{  
	platform_driver_unregister(&led_platform_driver);
}  
  

module_init(led_init);  
module_exit(led_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("led");  
MODULE_LICENSE("GPL v2"); 
