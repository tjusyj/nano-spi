KERNELDIR := /home/tjusyj/nano/linux/

CURRENT_PATH := $(shell pwd)
 
obj-m := myled.o
 
build: kernel_modules
 
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules CROSS_COMPILE=/home/tjusyj/nano/buildroot-2017.08/output/host/bin/arm-none-linux-gnueabi- ARCH=arm 
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
