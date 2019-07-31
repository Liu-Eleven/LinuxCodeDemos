
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ioport.h>
//#include <linux/init-input.h>
#include <linux/gpio.h>
#include <linux/input-polldev.h>
#include <linux/timer.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_PM)
#include <linux/pm.h>
#endif





#define MAG_MAX_NEG -500
#define MAG_MAX_POS 500

static struct input_dev *test_input_dev;

struct test_device{
//	struct input_dev *test_input_dev;
	struct timer_list s_timer; //定时器结构体
	unsigned int count;
};
struct test_device * test_dev;

//定时器处理函数
static void second_timer_handle(unsigned long arg)
{
	mod_timer(&test_dev->s_timer, jiffies + HZ);
	//提交input事件
	input_report_abs(test_input_dev, ABS_GAS, test_dev->count);
	input_sync(test_input_dev);
	test_dev->count++;
	if(test_dev->count == 100)
		test_dev->count =0;
//	printk( "current jiffies is %ld\n", jiffies);
//	printk( "count is %d\n", test_dev->count);
}
static int __init demo_init(void) //加载函数
{
	int ret =0;
	
	test_dev = kzalloc(sizeof(struct test_device),GFP_KERNEL);
	
	if(test_dev == NULL) {
		printk("Failed to allocate driver struct\n");
		ret = -ENOMEM;
		goto err_kzalloc_device;
	}
	
	test_input_dev = input_allocate_device();
	
	if(test_input_dev == NULL) {
		printk( "Failed to allocate input dev\n");
		ret = -ENOMEM;
		goto err_input_allocate_device;
	}
	
	test_input_dev->name = "test_input";
	test_input_dev->phys = "input/test";
	test_input_dev->id.bustype = BUS_HOST; // BUS_I2C;
	test_input_dev->id.vendor = 0x0001;
    test_input_dev->id.product = 0x0001;
    test_input_dev->id.version = 0x0100;
	
	//set_bit(EV_ABS, test_input_dev->evbit);
	//set_bit(EV_KEY, test_input_dev->evbit);
	
	test_input_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	test_input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	test_input_dev->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE);
	//input_set_abs_params(test_input_dev, ABS_X, 0, SCREEN_MAX_X, 0, 0);
	//input_set_abs_params(test_input_dev, ABS_Y, 0, SCREEN_MAX_Y, 0, 0);
	//input_set_abs_params(test_input_dev, ABS_PRESSURE, 0, 255, 0, 0);
	
	input_set_abs_params(test_input_dev, ABS_GAS, MAG_MAX_NEG, MAG_MAX_POS, 0, 0);
	
	ret = input_register_device(test_input_dev);
	
	if(ret) {
		printk( "Failed to input_unregister_device\n");
		goto err_input_unregister_device;
	}
	
	//初始化定时器，注册定时器
	test_dev->count = 0;
	init_timer(&test_dev->s_timer);
	test_dev->s_timer.function = second_timer_handle;
	test_dev->s_timer.expires = jiffies + HZ;
	add_timer(&test_dev->s_timer);
	printk("demo_init");
	
	return ret;
	
err_input_unregister_device:
	input_free_device(test_input_dev);
	err_input_allocate_device:
	kfree(test_dev);
	err_kzalloc_device:
	return ret;
}


static void __exit demo_exit(void) //卸载函数
{
	del_timer(&test_dev->s_timer);
	input_unregister_device(test_input_dev);
	input_free_device(test_input_dev);
	kfree(test_dev);
	printk("demo_exit");
}

module_init(demo_init);
module_exit(demo_exit);


MODULE_DESCRIPTION("input device driver");
MODULE_LICENSE("GPL v2");
