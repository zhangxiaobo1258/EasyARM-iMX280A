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
#include <linux/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR		200  	/*主设备号*/
#define LED_NAME		"led"	/*设备名称*/


/*
BEEP:GPIO2_6
RUN :GPIO2_7
ERR	:GPIO3_20
*/
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

/*映射后的寄存器虚拟地址指针*/
static void __iomem *SW_PINCTRL_BASE;			
static void __iomem *SW_PINCTRL_MUXSEL4;
static void __iomem *SW_PINCTRL_DRIVE8;
static void __iomem *SW_PINCTRL_PULL2;	
static void __iomem *SW_PINCTRL_DOUT2;	
static void __iomem *SW_PINCTRL_DOUT2_SET;	
static void __iomem *SW_PINCTRL_DOUT2_CLR;	
static void __iomem *SW_PINCTRL_DOUT2_TOG;	
static void __iomem *SW_PINCTRL_DOE2;
static void __iomem *SW_PINCTRL_DOE2_SET;
static void __iomem *SW_PINCTRL_DOE2_CLR;
static void __iomem *SW_PINCTRL_DOE2_TOG;

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
	int status;
	/*unsigned char data_in = (unsigned char)arg;
	unsigned char cmd_in  = (unsigned char)cmd;*/
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
/* @description		:初始化GPIO，将GPIO初始化为OUT模式，驱动能力8mA,NO pull
*  @param 			:无
*  @return 			:无
*/
static int __init led_init(void)  
{
	int retvalue = 0;
	u32 val = 0;

	/*初始化GPIO*/
	/*1.寄存器映射*/
	SW_PINCTRL_BASE			= ioremap(HW_PINCTRL_BASE , 4);//映射硬件地址和字节数
	SW_PINCTRL_MUXSEL4		= ioremap(HW_PINCTRL_MUXSEL4 , 4);
	SW_PINCTRL_DRIVE8		= ioremap(HW_PINCTRL_DRIVE8 , 4);
	SW_PINCTRL_PULL2		= ioremap(HW_PINCTRL_PULL2 , 4);
	SW_PINCTRL_DOUT2		= ioremap(HW_PINCTRL_DOUT2 , 4);
	SW_PINCTRL_DOUT2_SET	= ioremap(HW_PINCTRL_DOUT2_SET , 4);
	SW_PINCTRL_DOUT2_CLR	= ioremap(HW_PINCTRL_DOUT2_CLR , 4);
	SW_PINCTRL_DOUT2_TOG	= ioremap(HW_PINCTRL_DOUT2_TOG , 4);
	SW_PINCTRL_DOE2			= ioremap(HW_PINCTRL_DOE2 , 4);
	SW_PINCTRL_DOE2_SET		= ioremap(HW_PINCTRL_DOE2_SET , 4);
	SW_PINCTRL_DOE2_CLR		= ioremap(HW_PINCTRL_DOE2_CLR , 4);
	SW_PINCTRL_DOE2_TOG		= ioremap(HW_PINCTRL_DOE2_TOG , 4);

	/*2.设置GPIO的复用功能，驱动电流，上下拉，设置顺序参考《MCIMX28RM.pdf》Page 716 9.2.3.1*/
	
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

	/*3.注册字符设备驱动*/
	retvalue = register_chrdev(LED_MAJOR,LED_NAME,&led_fops);
    printk("led_init (2020.10.23).\n");  
	if(retvalue < 0){
		printk("Regsiter hrdev failed!\r\n");
		return -EIO;
	}

	return 0;
}  


static void __exit led_exit(void)  
{  
	int ret = 0;

	iounmap(SW_PINCTRL_BASE );//取消映射硬件地址
	iounmap(SW_PINCTRL_MUXSEL4);
	iounmap(SW_PINCTRL_DRIVE8);
	iounmap(SW_PINCTRL_PULL2 );
	iounmap(SW_PINCTRL_DOUT2 );
	iounmap(SW_PINCTRL_DOUT2_SET );
	iounmap(SW_PINCTRL_DOUT2_CLR );
	iounmap(SW_PINCTRL_DOUT2_TOG );
	iounmap(SW_PINCTRL_DOE2 );
	iounmap(SW_PINCTRL_DOE2_SET );
	iounmap(SW_PINCTRL_DOE2_CLR );
	iounmap(SW_PINCTRL_DOE2_TOG );
	
	unregister_chrdev(LED_MAJOR,LED_NAME);
    printk("led_exit (2020.10.23).\n");  
}  
  

module_init(led_init);  
module_exit(led_exit);  
  
MODULE_AUTHOR("zhangxiaobo");
MODULE_DESCRIPTION("led");  
MODULE_LICENSE("GPL v2"); 

