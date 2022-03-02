#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define GPIO_BASE 0xfe200000 //rpi 4
//rpi b+ 이하 0x20200000
//rpi 2  이상 0x3f200000

#define GPFSEL1 0x4
#define GPSET0 0x1C
#define GPCLR0 0x28
#define GPLEV0 0x34

static void __iomem *gpio_base;
volatile unsigned *gpfsel1;
volatile unsigned *gpset0;
volatile unsigned *gpclr0;
volatile unsigned *gplev0;

int dev_open(struct inode *inode, struct file *filp){
	
	*gpset0 = 0b00000000000001000000000000000000;
	*gpclr0 = 0b00000000000000000000000000000000;

	printk(KERN_INFO "[gpio_dev]%s : LED_ON", __FUNCTION__);
	return 0;
}

int dev_close(struct inode *inode, struct file *filp){

	*gpset0 = 0b00000000000000000000000000000000;
	*gpclr0 = 0b00000000000001000000000000000000;

	printk(KERN_INFO "[gpio_dev]%s : LED_OFF", __FUNCTION__);

	return 0;
}

ssize_t dev_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	int result;
	static const int HIGH = 1;
	static const int LOW = 0;
	unsigned int input = *gplev0 & 0b00000000000010000000000000000000;

	if(input > 0){
		result = copy_to_user(buf, &HIGH, sizeof(HIGH));
	} else { 
		result = copy_to_user(buf, &LOW, sizeof(LOW));
	}

	if(result == 0){
		printk(KERN_INFO "[gpio_dev]%s : SWITCH_READ", __FUNCTION__);
		printk(KERN_INFO "[gpio_dev]input : %d", input);
	} else {
		printk(KERN_INFO "[gpio_dev]%s : SWITCH_READ Failed(%d) ", __FUNCTION__, result);
	}

	return count;
}

static struct file_operations dev_fops = {
	.read = dev_read,
	.open = dev_open,
	.release = dev_close
};

int __init dev_init(void){
	int result;

	printk("%s", __FUNCTION__);

	gpio_base = ioremap(GPIO_BASE, 0x60);

	gpfsel1 = (volatile unsigned *)(gpio_base + GPFSEL1);
	gpset0 = (volatile unsigned *)(gpio_base + GPSET0);
	gpclr0 = (volatile unsigned *)(gpio_base + GPCLR0);
	gplev0 = (volatile unsigned *)(gpio_base + GPLEV0);

	*gpfsel1 = 0b00000001000000000000000000000000;

	result = register_chrdev(300, "gpio_dev", &dev_fops);
	if(result < 0)
		printk("[gpio_dev]register_chrdev failed..");
	else
		printk("[gpio_dev]register_chrdev successful..");

	return 0;
}

void __exit dev_exit(void){
	printk("[gpio_dev]%s", __FUNCTION__);
	unregister_chrdev(300, "gpio_dev");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
