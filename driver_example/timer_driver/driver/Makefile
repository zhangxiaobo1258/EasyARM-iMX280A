obj-m     := timer.o

KDIR      := /home/davinci/Projects/linux-4.15.12/
PWD       := $(shell pwd)
COMPILE	  := /home/davinci/Projects/buildroot-2020.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabi-gcc
LD_PATH	  := /home/davinci/Projects/buildroot-2020.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabi-ld
AR_PATH	  := /home/davinci/Projects/buildroot-2020.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabi-ar
ARCH_TYPE := arm

all:
	make CC=$(COMPILE) LD=$(LD_PATH) AR=$(AR_PATH) ARCH=$(ARCH_TYPE) -C $(KDIR) M=$(PWD)  modules
clean:
	make CC=$(COMPILE) LD=$(LD_PATH) AR=$(AR_PATH) ARCH=$(ARCH_TYPE) -C $(KDIR) M=$(PWD)  clean
