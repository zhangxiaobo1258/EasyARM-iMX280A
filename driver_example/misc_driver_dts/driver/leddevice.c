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


/* platform 设备结构体变量，需要注意的是name字段和platform驱动中的name字段一致，否则设备和驱动匹配失败 */
static struct platform_device leddevice = {
	.name 	= "mxs-leds",							
	.id 	= -1,
};

/* 设备模块加载函数，在此函数里面通过platform_device_register向kernel注册leddevice这个platform设备 */
static int __init led_init(void)  
{
	return platform_device_register(&leddevice);
}  

/* 设备模块卸载函数，在此函数里面通过platform_device_unregister从kernel删除掉leddevice这个platform设备 */
static void __exit led_exit(void)  
{  
	platform_device_unregister(&leddevice);
}  
  

module_init(led_init);  
module_exit(led_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_LICENSE("GPL v2"); 

