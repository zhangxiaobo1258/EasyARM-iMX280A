# EasyARM-iMX280A
致远电子低成本imx280A开发板，NXP官方网站提供u-boot版本：u-boot-2009.08，kernel版本：linux-2.6.35.3.

注意： 开发板网线直连电脑有时候ping不通，所以建议使用交换机或者路由器

首先，安装需要的安装包：

sudo apt install gcc-arm-linux-gnueabi make build-essential kernel-package libncurses-dev bison linux-headers-5.4.0-26-generic libssl-dev u-boot-tools mtd-utils

最后配置elftosb：

cd bootloader/elftosb

sudo cp elftosb_64bit /usr/bin/

cd /usr/bin/

sudo mv elftosb_64bit elftosb

sudo chmod 777 /usr/bin/elftosb

1.移植u-boot-2018.03

(1) 配置Makefile

sudo vim Makefile

CROSS_COMPILE ?=arm-linux-gnueabi-

(2) 开始编译

sudo make mx28evk_nand_defconfig

sudo make menuconfig

sudo make -j4

(3) 编译生成加密文件

将u-boot和u-boot.lds文件复制到bootloader/imx-bootlets-src-10.12.01/目录下，在bootloader/imx-bootlets-src-10.12.01/目录下build文件中

将

make CROSS_COMPILE=arm-fsl-linux-gnueabi- BOARD=iMX28_EVK

修改为：

make CROSS_COMPILE=arm-linux-gnueabi- BOARD=iMX28_EVK

首先执行:

make clean

清除以前编译的文件

执行

./build

生成imx28_ivt_uboot.sb加密文件

将imx28_ivt_uboot.sb复制到MfgTool 1.6.2.055\Profiles\MX28 Linux Update\OS Firmware\files下，使用MfgTool烧写.

注：在drivers/net/fec_mxs.c的1123行增加：

for(uint8_t i=0;i<20;i++){

printf("RMII Regsister [%d] Value is 0x%X.\n",i,fec_mdio_read(fec->eth,5,i));
}

会在uboot启动时打印RMII寄存器的信息，如果不需要可去掉。

2.移植kernel

在顶层Makefile中增加：

ARCH ?=arm

CROSS_COMPILE ?=arm-linux-gnueabi-

sudo make mxs_defconfig

sudo make menuconfig

sudo make imx28-evk.dtb -j4

sudo make modules -j4

sudo make zImage -j4

sudo chmod 777 arch/arm/boot/zImage

sudo cat arch/arm/boot/dts/imx28-evk.dtb >> arch/arm/boot/zImage

sudo sync

在PC端进入u-boot,执行：

tftp 0x42000000 zImage

nand erase.part kernel

nand write 0x42000000 kernel

3.使用buildroot制作文件系统：

make ARCH=arm freescale_imx28evk_defconfig

make menuconfig

make

烧写文件系统，以ubifs为例：

nand erase.part rootfs

ubi part rootfs 2048

ubi create rootfs

tftp 0x42000000 rootfs.ubifs

ubi write 0x42000000 rootfs $filesize
