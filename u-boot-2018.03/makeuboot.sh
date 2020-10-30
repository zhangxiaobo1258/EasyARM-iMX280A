#!/bin/sh
#sudo make distclean
sudo make mx28evk_nand_defconfig
sudo make menuconfig
sudo make -j4
#elftosb -f  imx28 -c arch/arm/cpu/arm926ejs/mxs/u-boot-imx28.bd -o u-boot.sb
#cp u-boot.sb ~/imx28_ivt_uboot.sb && sync
#cp u-boot ../bootloader/imx-bootlets-src-10.12.01
