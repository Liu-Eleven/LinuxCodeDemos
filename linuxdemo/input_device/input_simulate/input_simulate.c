

#include <linux/module.h>  
#include <linux/platform_device.h>  
#include <linux/slab.h>  
#include <linux/input.h>  
#include <linux/kthread.h>  
#include <linux/semaphore.h>  
  
//定义虚拟touch设备结构体

  
struct touch_dev *vtouch_dev = NULL;  
static int get_random_number(struct touch_dev * vtouch_dev);
void get_random_bytes(void *buf, int nbytes);

extern void msleep(unsigned int msecs);
  
//获取随机数函数
static int get_random_number(struct touch_dev * vtouch_dev)
{
	unsigned long randNum;

	get_random_bytes(&randNum, sizeof(unsigned long));

	vtouch_dev->x = (int)randNum  % 1000;
	vtouch_dev->y = (int)(randNum / 1000) % 1000;

	printk(KERN_ALERT "the touch point coordinate:  %d  %d\n", vtouch_dev->x,vtouch_dev->y);

	return 0;
}

//开启内核线程一直上报坐标
static int vtouch_thread(void *data)  
{  
    int x,y;  
    struct touch_dev *vtouch_dev = (struct touch_dev*)data;  
      
    printk(KERN_INFO "vtouch thread running\n");  
      
    do{  
		
		//使用随机数计算坐标
		get_random_number(vtouch_dev);

        x = vtouch_dev->x;  
        y = vtouch_dev->y;  
		
		//上报绝对坐标
        input_report_abs(vtouch_dev->input,ABS_X,x);  
        input_report_abs(vtouch_dev->input,ABS_Y,y);  
		//上报同步通知
        input_sync(vtouch_dev->input);  
  
        printk("vtouch thread report\n");
		msleep(2000);
    }  while(!kthread_should_stop());//线程退出条件
  
    return 0;  
}     

//vtouch设备驱动匹配后进入probe  
static int vtouch_probe(struct platform_device *pdev)  
{  
    int ret = -1;  
    printk("%s debug \n",__func__);  
      
    if(vtouch_dev->p_dev == pdev){  
        printk("platform device is same\n");  
    }  
    
	//为input device申请内存
    vtouch_dev->input = input_allocate_device();  
    if(!(vtouch_dev->input)){  
        printk("%s request input deivce error\n",__func__);  
        goto alloc_input;  
    }  
     
	//设置 vtouch 设备名称
    vtouch_dev->input->name = "vtouch";  
	//设置设备支持坐标事件，包括X坐标，Y坐标事件，Z坐标事件。
    set_bit(EV_ABS,vtouch_dev->input->evbit);  
	//对于X轴范围是-1024到+1024，数据误差是-2到+2，中心平滑位置是0 
    input_set_abs_params(vtouch_dev->input, ABS_X, -1024, 1024, 2, 0);
	//同上
    input_set_abs_params(vtouch_dev->input, ABS_Y, -1024, 1024, 2, 0);  
  
	//注册输入设备
    ret = input_register_device(vtouch_dev->input);  
    if(ret < 0){  
        printk("%s register input device error\n",__func__);  
        goto input_register;  
    }  
    //开启线程利用内核随机数不断上报绝对坐标 
    vtouch_dev->run_thread = kthread_run(vtouch_thread,vtouch_dev,"vtouch_thread");  
      
    return 0; 
  
input_register:  
    input_free_device(vtouch_dev->input);  
alloc_input:  
    kfree(vtouch_dev);  
    return ret;  
}  
  
static struct platform_driver vtouch_driver = {  
    .probe = vtouch_probe,  
    .driver = {  
        .owner = THIS_MODULE,  
        .name = "v_touch",  
    },  
};  
  
static int __init vtouch_init(void)  
{  
    int ret =-1;  
      
    printk("%s\n", __func__);   

    vtouch_dev = kzalloc(sizeof(struct touch_dev),GFP_KERNEL);  
    if(vtouch_dev == NULL){  
        printk("%s alloc memory  error\n",__func__);  
        return -ENOMEM;  
    }  
  
	//注册平台设备
    vtouch_dev->p_dev= platform_device_register_simple("v_touch",-1,NULL,0);   
    if(!(vtouch_dev->p_dev)){  
        printk("%s register platform device error\n",__func__);  
        return ret;  
    }  
     
	//注册平台驱动
    ret = platform_driver_register(&vtouch_driver);  
    if(ret < 0){  
        printk("%s register driver error\n",__func__);  
        return ret;  
    }  
  
    return 0;  
}  
  
static void __exit vtouch_exit(void)  
{  
    printk("%s\n", __func__);  
	if(IS_ERR(vtouch_dev->run_thread) ){
		int ret ;
		ret = kthread_stop(vtouch_dev->run_thread);
    	printk("kthread_stop after ... %s ret = %d\n", __func__,ret);  
	}
    printk("kthread_stop after ... %s\n", __func__);  

    if(vtouch_dev->input != NULL){  
        input_unregister_device(vtouch_dev->input);  
    }  
      
    if(vtouch_dev != NULL){  
    	platform_device_unregister(vtouch_dev->p_dev);  
    }  
      
    platform_driver_unregister(&vtouch_driver);  
      

    kfree(vtouch_dev);  
	
}  
  
module_init(vtouch_init);  
module_exit(vtouch_exit);  
MODULE_LICENSE("GPL");  
MODULE_AUTHOR("<xf.dong@chinapalms.com>");  