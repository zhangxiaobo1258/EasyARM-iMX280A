#!/bin/bash
#sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- mxs_defconfig -j4
#sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- menuconfig -j4
make imx28-evk.dtb -j4
make zImage -j4
cat arch/arm/boot/dts/imx28-evk.dtb >> arch/arm/boot/zImage 
sync
#make uImage LOADADDR=0x42000000 -j4

