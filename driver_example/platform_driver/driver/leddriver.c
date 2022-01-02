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

#define LEDDEV_CNT		1  			/*设备号长度*/
#define LEDDEV_NAME		"platled"	/*设备名称*/
#define LEDOFF			0			/*设备名称*/
#define LEDON			1			/*设备名称*/

/*映射后的寄存器虚拟地址指针*/
static void __iomem *SW_PINCTRL_BASE;			
static void __iomem *SW_PINCTRL_MUXSEL4;
static void __iomem *SW_PINCTRL_DRIVE8;
static void __iomem *SW_PINCTRL_PULL2;	
static void __iomem *SW_PINCTRL_DOUT2;		
static void __iomem *SW_PINCTRL_DOE2;

struct leddev_dev{
	dev_t devid;					/* 设备号 */
	struct cdev cdev;				/* cdev */
	struct class *class;			/* 类 */
	struct device *device;			/* 设备 */
	int major;						/* 主设备号 */
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
	if(status < 0){
		printk("Kernel octl is failed!\r\n");
		return -EFAULT;
	}*/


	printk("################# led_ioctl cmd = 0x%x dat = 0x%x\n", cmd, arg);
	

	switch (cmd) 
	{	
		case 'l':
			if(arg == 0x30){
				val = readl(SW_PINCTRL_DOUT2);
				val &= ~0x80;			//向GPIO2_7写0
				writel(val,SW_PINCTRL_DOUT2);
				printk("LED is on，val is 0x%x\n",readl(SW_PINCTRL_DOUT2));
			}				
			else{
				val = readl(SW_PINCTRL_DOUT2);
				val |= 0x80;		//向GPIO2_7写1
				writel(val,SW_PINCTRL_DOUT2);	//将GPIO2_7输出低电平
				printk("LED is off，val is 0x%x\n",readl(SW_PINCTRL_DOUT2));
			}
			return 0;
		case 'b':			
			if(arg == 0x30){
				val = readl(SW_PINCTRL_DOUT2);			
				val &=  ~0x40;
				writel(val,SW_PINCTRL_DOUT2);	//将GPIO2_6输出低电平
				printk("Beep is on，val is 0x%x\n",readl(SW_PINCTRL_DOUT2));
			}
			else{
				val = readl(SW_PINCTRL_DOUT2);			
				val |=  0x40;
				writel(val,SW_PINCTRL_DOUT2);	//将GPIO2_6输出高电平
				printk("Beep is off，val is 0x%x\n",readl(SW_PINCTRL_DOUT2));
			}
			return 0;
		default:
			printk("LED error!\n");
			return -ENOTTY;
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
/* 在该函数里面初始化LED，注册字符设备驱动，也就是将原来的驱动加载函数的内容放到该函数里面完成 */
static int led_probe(struct platform_device *dev)
{
	int i =0;
	int resize[6];
	u32 val = 0;
	struct resource *ledsource[6];

	printk("led driver and device has matched !\r\n");
	/* 1.获取资源 */
	for(i = 0; i< 6;i++){
		ledsource[i] = platform_get_resource(dev,IORESOURCE_MEM, i);
		printk("%d.\n",i);
		if(!ledsource[i]){
			dev_err(&dev->dev,"No MEM resource for %d always on.\n",i);
			return -ENXIO;
		}
		resize[i] = resource_size(ledsource[i]);
	}
	/* 2.初始化LED */
	/* 寄存器地址映射 */
	SW_PINCTRL_BASE			= ioremap(ledsource[0]->start, resize[0]);
	SW_PINCTRL_MUXSEL4		= ioremap(ledsource[1]->start, resize[1]);
	SW_PINCTRL_DRIVE8		= ioremap(ledsource[2]->start, resize[2]);
	SW_PINCTRL_PULL2		= ioremap(ledsource[3]->start, resize[3]);
	SW_PINCTRL_DOUT2		= ioremap(ledsource[4]->start, resize[4]);
	SW_PINCTRL_DOE2			= ioremap(ledsource[5]->start, resize[5]);

	/* 3.设置GPIO的复用功能，驱动电流，上下拉，设置顺序参考《MCIMX28RM.pdf》Page 716 9.2.3.1 */
	
	val = 0x00;//Block Control Register设置为通用模式
	writel(val,SW_PINCTRL_BASE);
	val = readl(SW_PINCTRL_MUXSEL4) | ( 0xF<<12 )  ;
	writel(val,SW_PINCTRL_MUXSEL4);	//将GPIO2_6和GPIO_7设置为GPIO接口
	val = readl(SW_PINCTRL_DRIVE8) | 0x66000000;
	writel(val,SW_PINCTRL_DRIVE8);	//将GPIO2_6和GPIO_7设置为3.3V,20mA	
	val = readl(SW_PINCTRL_PULL2)  | 0x80;
	writel(val,SW_PINCTRL_PULL2);	//将GPIO2_6设置为无上拉，将GPIO_7设置为上拉
	val = readl(SW_PINCTRL_DOUT2)  | 0x80;
	writel(val,SW_PINCTRL_DOUT2);	//将GPIO2_6和GPIO_7输出低电平
	val = readl(SW_PINCTRL_DOE2)   | 0xC0;
	writel(val,SW_PINCTRL_DOE2);	//将GPIO2_6和GPIO_7使能输出

	/* 4.注册字符设备驱动 */
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
	return 0;
}
/* 当卸载platform驱动时该函数会运行，在此函数释放内存，注销字符设备 */
static int led_remove(struct platform_device *dev)
{
	iounmap(SW_PINCTRL_BASE );//取消映射硬件地址
	iounmap(SW_PINCTRL_MUXSEL4);
	iounmap(SW_PINCTRL_DRIVE8);
	iounmap(SW_PINCTRL_PULL2 );
	iounmap(SW_PINCTRL_DOUT2 );
	iounmap(SW_PINCTRL_DOE2 );

	/* 注销字符设备 */
	cdev_del(&leddev.cdev);/* 删除cdev */
	unregister_chrdev_region(leddev.devid,LEDDEV_CNT);
	/* device_destroy()在src/drivers/base/core.c中实现，函数原型为：
	void device_destroy(struct class *dev, dev_t devt); */
	device_destroy(leddev.class,leddev.devid);
	class_destroy(leddev.class);

	return 0;
}

/* platform 驱动结构体 */
static struct platform_driver led_driver = {
	.driver = {
		.name = "mxs-led",				/* 驱动名称，用于和设备匹配 */
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
	return platform_driver_register(&led_driver);
}  


static void __exit led_exit(void)  
{  
	platform_driver_unregister(&led_driver);
}  
  

module_init(led_init);  
module_exit(led_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("led");  
MODULE_LICENSE("GPL v2"); 

