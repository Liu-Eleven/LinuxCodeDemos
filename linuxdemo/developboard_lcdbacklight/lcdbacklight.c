
#include <linux/init.h>                        
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/freezer.h>
#include <linux/spi/spi.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinconf-sunxi.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/list.h>
#include <mach/sys_config.h>
#include <mach/platform.h>
#include <mach/gpio.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>


#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif

#define DEVICE_NAME    "lcdbacklight"



#define lcdbacklight_ON    0x1
#define lcdbacklight_OFF    0x0

static int gpio_lcdbacklight;

static DEFINE_MUTEX(gpio_mutex);

/////////////////////////////////////////////////////////////

static void lcdbacklight_init(void);
static void lcdbacklight_on(void);
static void lcdbacklight_off(void);



/////////////////////////////////////////////////////////////

static void lcdbacklight_init(void)
{
	gpio_lcdbacklight = GPIOB(9);
	if(0 != gpio_request(gpio_lcdbacklight, "lcdbacklight_gpio")) 
	{		
		printk("ERROR: lcdbacklight_gpio gpio_request is failed.\n"); 			
		return -EBUSY;	
	}
	if (gpio_direction_output(gpio_lcdbacklight, 1))
		gpio_free(gpio_lcdbacklight);

	lcdbacklight_on();
	//lcdbacklight_off();
}

static void lcdbacklight_on(void)
{
	__gpio_set_value(gpio_lcdbacklight, 1);
}

static void lcdbacklight_off(void)
{
	__gpio_set_value(gpio_lcdbacklight, 0);
}

//////////////////////////////////////////////////////////////////


static int lcdbacklight_gpio_open(struct inode *inode, struct file *file)
{
	return 0;
}


static ssize_t lcdbacklight_gpio_read(struct file *file, char __user * buffer, size_t size, loff_t * pos)
{
	return size;
}


static ssize_t lcdbacklight_gpio_write(struct file *file, const char __user * buffer, size_t size, loff_t * pos)
{
	char data;

	if(size != 1)
	{
		printk(KERN_INFO "ERROR :: size != 1\r\n");
		return -EINVAL;
	}
	
	if(copy_from_user(&data, buffer, 1))
	{
		printk(KERN_INFO "ERROR :: copy_from_user fail\r\n");
		return -EINVAL;
	}

	mutex_lock(&gpio_mutex);
	
	switch(data)
	{
		case lcdbacklight_ON:
			lcdbacklight_on();
			break;
		case lcdbacklight_OFF:
			lcdbacklight_off();
			break;
		default:
			return -EINVAL;
	}
	
	mutex_unlock(&gpio_mutex);

	return size;
}


static int lcdbacklight_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) 
	{
		case lcdbacklight_ON:
			lcdbacklight_on();
			return 0;

		case lcdbacklight_OFF:
			lcdbacklight_off();
			return 0;
			
		default:
			return -ENOIOCTLCMD;
	}
}


static struct file_operations lcdbacklight_gpio_fops = {
	.owner		= THIS_MODULE,
	.read		= lcdbacklight_gpio_read,
	.write		= lcdbacklight_gpio_write,
	.open		= lcdbacklight_gpio_open,
	.unlocked_ioctl		= lcdbacklight_gpio_ioctl,
};


static struct miscdevice lcdbacklight_gpio_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "lcdbl",
	.fops		= &lcdbacklight_gpio_fops,
};


int __init lcdbacklight_gpio_init(void)
{
	int ret;

	lcdbacklight_init();

	ret = misc_register(&lcdbacklight_gpio_miscdev);
	if (ret) 
	{
		printk (KERN_ALERT "cannot register miscdev on minor(%d)\n", ret);
		return ret;
	}

	printk(KERN_ALERT "lcdbacklight driver successfully init !\n");

	return 0;
}



void __exit lcdbacklight_gpio_exit(void)
{
	misc_deregister(&lcdbacklight_gpio_miscdev); 	
	printk(KERN_ALERT"lcdbacklight driver exit.\n");
}


module_init(lcdbacklight_gpio_init);
module_exit(lcdbacklight_gpio_exit);


MODULE_AUTHOR("JasonWang");
MODULE_DESCRIPTION("lcdbacklight driver");
MODULE_LICENSE("GPL");






