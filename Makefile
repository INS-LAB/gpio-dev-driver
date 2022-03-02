obj-m := gpio_dev.o

KDIR := # Please, insert the Rpi kernel directory path

CC := arm-linux-gnueabihf-gcc
OBJECT := gpio_app

default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C$(KDIR) M=$(shell pwd) modules
	$(CC) -o $(OBJECT) $(OBJECT).c

clean:
	make -C$(KDIR) M=$(shell pwd) clean
	rm $(OBJECT)