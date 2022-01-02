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

/*寄存器物理地址*/
#define HW_PINCTRL_BASE				(0x80018000)				
#define HW_PINCTRL_MUXSEL4			(HW_PINCTRL_BASE+0x140)
#define HW_PINCTRL_DRIVE8			(HW_PINCTRL_BASE+0x380)
#define HW_PINCTRL_PULL2			(HW_PINCTRL_BASE+0x620)	
#define HW_PINCTRL_DOUT2			(HW_PINCTRL_BASE+0x720)	
#define HW_PINCTRL_DOUT2_SET		(HW_PINCTRL_BASE+0x724)	
#define HW_PINCTRL_DOUT2_CLR		(HW_PINCTRL_BASE+0x728)	
#define HW_PINCTRL_DOUT2_TOG		(HW_PINCTRL_BASE+0x72C)	
#define HW_PINCTRL_DOE2				(HW_PINCTRL_BASE+0xB20)
#define HW_PINCTRL_DOE2_SET			(HW_PINCTRL_BASE+0xB24)
#define HW_PINCTRL_DOE2_CLR			(HW_PINCTRL_BASE+0xB28)
#define HW_PINCTRL_DOE2_TOG			(HW_PINCTRL_BASE+0xB2C)
#define REGISTER_LENGTH				4

//===================================================================
//file opt
//===================================================================
/* @description		:打开设备，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param - inode	:传递给驱动的inode
*  @param - filp	:设备文件，file结构体有个叫做private_data的成员变量，一般在open的时候将private_data指向设备结构体
*  @return 			:0 成功；其他 失败
*/
static void  led_release(struct device *dev )
{
	printk("led  device  released!\r\n");
}
/* 设备资源信息，也就是LED1 所使用的所有寄存器,也就是IORESOURCE_MEM资源 */
static struct resource led_resources[] = {
	[0] = {
		.start  = HW_PINCTRL_BASE,
		.end 	= (HW_PINCTRL_BASE + REGISTER_LENGTH - 1),
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = HW_PINCTRL_MUXSEL4,
		.end 	= (HW_PINCTRL_MUXSEL4 + REGISTER_LENGTH - 1),
		.flags  = IORESOURCE_MEM,
	},
	[2] = {
		.start  = HW_PINCTRL_DRIVE8,
		.end 	= (HW_PINCTRL_DRIVE8 + REGISTER_LENGTH - 1),
		.flags  = IORESOURCE_MEM,
	},
	[3] = {
		.start  = HW_PINCTRL_PULL2,
		.end 	= (HW_PINCTRL_PULL2 + REGISTER_LENGTH - 1),
		.flags  = IORESOURCE_MEM,
	},
	[4] = {
		.start  = HW_PINCTRL_DOUT2,
		.end 	= (HW_PINCTRL_DOUT2 + REGISTER_LENGTH - 1),
		.flags  = IORESOURCE_MEM,
	},
	[5] = {
		.start  = HW_PINCTRL_DOE2,
		.end 	= (HW_PINCTRL_DOE2 + REGISTER_LENGTH -1),
		.flags  = IORESOURCE_MEM,
	},
};

/* platform 设备结构体变量，需要注意的是name字段和platform驱动中的name字段一致，否则设备和驱动匹配失败 */
static struct platform_device leddevice = {
	.name 	= "mxs-led",							
	.id 	= -1,
	.dev 	= {
		.release = &led_release,
	},
	.num_resources 	= ARRAY_SIZE(led_resources),
	.resource		= led_resources,
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

