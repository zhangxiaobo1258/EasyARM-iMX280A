# 1. EasyARM-iMX280A
致远电子低成本 `imx280A` 开发板，`NXP` 官方网站提供 `u-boot` 版本：`u-boot-2009.08`，`Kernel`版本：`linux-2ernel`版本：linux-2.6.35.3`.

注意： 开发板网线直连电脑有时候 `ping` 不通，所以建议使用交换机或者路由器

首先，`Ubuntu` 安装需要的安装包：

```bash
sudo apt install gcc-arm-linux-gnueabi make build-essential kernel-package libncurses-dev bison linux-headers-5.4.0-26-generic libssl-dev u-boot-tools mtd-utils
```

最后配置 `elftosb`：

```bash
cd bootloader/elftosb

sudo cp elftosb_64bit /usr/bin/

cd /usr/bin/

sudo mv elftosb_64bit elftosb

sudo chmod 777 /usr/bin/elftosb
```

1.移植 `u-boot-2018.03`

(1) 配置 `Makefile`

```bash
sudo vim Makefile

CROSS_COMPILE ?=arm-linux-gnueabi-
```

(2) 开始编译

```bash
sudo make mx28evk_nand_defconfig

sudo make menuconfig

sudo make -j4
```

(3) 编译生成加密文件

将 `u-boot` 和 `u-boot.lds` 文件复制到 `bootloader/imx-bootlets-src-10.12.01/` 目录下，在 `bootloader/imx-bootlets-src-10.12.01/` 目录下 `build` 文件中

将

```bash
make CROSS_COMPILE=arm-fsl-linux-gnueabi- BOARD=iMX28_EVK
```

修改为：

```bash
make CROSS_COMPILE=arm-linux-gnueabi- BOARD=iMX28_EVK
```

首先执行:

```bash
make clean
```

清除以前编译的文件

执行

```bash
./build

# 如果提示权限不足可以加 sudo 提权

sudo ./build

# 或使用 fake-root 提权

sudo apt install fake-root

fake-root ./build
```

生成 `imx28_ivt_uboot.sb` 加密文件

将 `imx28_ivt_uboot.sb` 复制到 `MfgTool 1.6.2.055\Profiles\MX28 Linux Update\OS Firmware\files` 下，使用 `MfgTool` 烧写.

`Ubuntu` 也可以使用 [mfgtools](https://github.com/NXPmicro/mfgtools) 工具烧录

```bash
sudo apt install mfgtools

uuu

uuu u-boot.imx            Download u-boot.imx via HID device

uuu list.uu               Run all the commands in list.uu

uuu -s                    Enter shell mode. Input command.

uuu -v u-boot.imx         verbose mode

uuu -d u-boot.imx         Once it detects the attachement of a known device, download boot.imx.

                            u-boot.imx can be replaced, new file will be download once board reset.

                            Do not unplug the SD card, write to the SD card, nor plug in a SD card when debugging uboot.

uuu -b emmc u-boot.imx    write u-boot.imx to emmc boot partition. u-boot.imx need enable fastboot

uuu -b emmc_all u-boot.imx sdcard.bz2\*
                            decompress sdcard.bz2 file and download the whole image into emmc

```   


注：在 `drivers/net/fec_mxs.c` 的 `1123` 行增加：

```bash
for(uint8_t i=0;i<20;i++){

printf("RMII Regsister [%d] Value is 0x%X.\n",i,fec_mdio_read(fec->eth,5,i));
}
```

会在 `uboot` 启动时打印 `RMII` 寄存器的信息，如果不需要可去掉。

2.移植 `kernel`

- 在顶层 `Makefile` 中增加：

```bash
ARCH ?=arm

CROSS_COMPILE ?=arm-linux-gnueabi-
```

- 编译 `zImage`

```bash
sudo make mxs_defconfig

sudo make menuconfig

sudo make imx28-evk.dtb -j4

sudo make modules -j4

sudo make zImage -j4

sudo chmod 777 arch/arm/boot/zImage

sudo cat arch/arm/boot/dts/imx28-evk.dtb >> arch/arm/boot/zImage

sudo sync
```

- 在 `PC` 端串口启动时按 <kbd>空格键</kbd> 进入 `u-boot`,执行：

- `u-boot` 设置本机 IP 地址和 TFTP 服务器 IP 地址

```bash
setenv ipaddr 10.10.10.55

setenv serverip 10.10.10.254

saveenv
```

- `u-boot` 通过 TFTP 刷 zImage

```bash
tftp 0x42000000 zImage

nand erase.part kernel

nand write 0x42000000 kernel
```

3.使用 `buildroot` 制作文件系统：

```bash
make ARCH=arm freescale_imx28evk_defconfig

make menuconfig

make
```

- 烧写文件系统，以 `ubifs` 为例：

```bash
nand erase.part rootfs

ubi part rootfs 2048

ubi create rootfs

tftp 0x42000000 rootfs.ubifs

ubi write 0x42000000 rootfs $filesize
```

- root 帐号和密码

```bash
User：root
Passwd：123456
```
