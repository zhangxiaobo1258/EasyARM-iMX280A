#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/netdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define XXXX_MINOR  XXX //次设备号
#define MEM_CLEAR	0x1
#define XXXX_SIZE	XXXX

static int globalmem_major = XXXX_MINOR;
module_param(globalmem_major,int,S_IRUGO);
struct  globalmem_dev{
	struct cdev cdev;
	...........
};

struct  globalmem_dev *xxxx_devp;

//===================================================================
//file opt
//===================================================================
static int globalmem_open(struct net_device *dev)
{
	/*申请端口、IRQ等，类似于fops->open*/
	ret = request_irq(dev->irq, &globalmem_interrupt, 0, dev->name, dev);
	........
	netif_start_queue(dev);
	........
	return 0;
}

static int globalmem_release(struct net_device *dev)
{
	/*释放端口，IRQ等，类似于fops->close*/
	free_irq(dev->irq, dev);
	........
	netif_stop_queue(dev);
	........
	return 0;
}

int globalmem_tx(struct sk_buff *skb,struct net_device *dev)
{
	int len;
	char *data,shortpkt[ETH_ZLEN];
	/*发送队列未满，可以发送*/
	if(globalmem_send_available(...)){
		/*获得有效数据指针和长度*/
		data = skb->data;
		len  = skb->len;
		if(len < ETH_ZLEN){
			/*如果帧长小于以太网帧最小长度，补0*/
			memset(shortpkt,0,ETH_ZLEN);
			memcpy((shortpkt,skb->data,skb->len);
			len = ETH_ZLEN;
			data = shortpkt;
		}
		dev->trans_start = jiffies;/*记录发送时间戳*/
		if(avail){
			globalmem_hw_tx(data,len,dev);
		}else{
			netif_stop_queue(dev);
			.......
		}
	}

}
void globalmem_tx_timeout(struct net_device *dev)
{
	...........
	netif_wake_queue(dev);/*重启设备发送队列*/
} 

static void globalmem_rx(struct net_device *dev)
{
	.......
	length = get_rev_len(.....);
	/*分配新的套接字缓冲区*/
	skb = dev_alloc_skb(length+2);

	skb_reserve(skb,2); /*对齐*/
	skb->dev = dev;
	/*读取硬件上接收到的数据*/
	insw(ioaddr + RX_FRAME_PORT, skb_put(skb,length), length >> 1);
	if(length & l)
		skb->data[length - l] = inw(ioaddr +  RX_FRAME_PORT);
	/*获得上层协议类型*/
	skb->protocol = eth_type_trans(skb,dev);
	/*把数据包交给上层*/
	netif_rx(skb);
	/*记录时间戳*/
	dev->last_rx = jiffies;
	.......
}

static void globalmem_interrupt(int irq,void *dev_id)
{
	......
	switch(status &ISQ_ENEVT_MASK){
		case ISQ_RECEIVER_ENEVT:
			/*获取数据包*/
			globalmem_rx(dev);
			break;
			/*其他类型的中断*/
	}
}

static long globalmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	//int status;
	unsigned int data_in = (unsigned int)arg;
	
	//printk("################# globalmem_ioctl cmd = %x dat = %x\n", cmd, data_in);
 
	switch (cmd) 
	{	

		case CMD_CASE1:			

			return 0;

		default:
			return -ENOTTY;
	}
	
	return 0;
}


static const struct file_operations globalmem_fops = 
{
	.owner			= THIS_MODULE,					//
	.read			= globalmem_read,					//read from device
	.write			= globalmem_write,					//send to device
	.unlocked_ioctl = globalmem_ioctl,					//finish control command 
};


static int __init globalmem_init(struct net_device *dev)  
{  
	/*设备的私有信息结构体*/
	struct globalmem_priv *priv;
	/*检查设备是否存在和设备所使用的硬件资源*/
	globalmem_hw_init();
	/*初始化以太网设备的公用成员*/
	ether_setup(dev);
	/*设置设备的成员函数指针*/
	ndev->netdev_ops   		= &globalmem_netdev_ops;
	ndev->ethtool_ops  		= &globalmem_ethtool_ops;
	ndev->watchdog_timeo   	= timeout;
	/*取得私有信息，并初始化*/
	priv = netdev_priv(dev);
	........../*初始化设备私有数据区*/
    printk("globalmem init in (####.##.## V#.#).\n");   //date and version  
	return 0;
}  


static void __exit globalmem_exit(void)  
{  
	int ret = 0;
	cdev_del(&globalmem_dev.cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major,0),1);
    printk("globalmem_exit (####.##.##).\n");  
}  
  
static int globalmem_register(void)
{
	.....
	/*分配net_device结构体并对其成员赋值*/	
	globalmem_dev = alloc_netdev(sizeof(struct xxxx_priv),"sn%d",xxxx_init);
	if(globalmem_dev == NULL)
	..../*分配net_device失败*/
	
	/*注册net_device结构体*/
	if((result == register_netdev(globalmem_dev)))
	....
}

static int globalmem_unregister(void)
{
	.....
	/*注销net_device结构体*/	
	unregister_netdev(globalmem_dev);

	....
}

module_init(globalmem_init);  
module_exit(globalmem_exit);  
  
MODULE_AUTHOR("TOUCH Corporation.");
MODULE_DESCRIPTION("globalmem");  
MODULE_LICENSE("GPL v2"); 

