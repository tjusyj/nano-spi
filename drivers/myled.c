#include <linux/init.h> 
#include <linux/module.h>
#include <linux/device.h> 
#include <linux/kernel.h> 
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/string.h>
 
/* 主设备号 */
#define LED_MAJOR 200
/* 设备名称 */
#define LED_NAME  "LED"
/* 操作的引脚 */
#define LED_PIN 1
/* 内核态用户态交互缓冲 */
static char recv_msg[20];
 
/* 自动创建设备树：类 */
static struct class *led_control_class = NULL;
/* 自动创建设备树：设备 */
static struct device *led_control_device = NULL;
 
/* led初始化 */
static int led_open(struct inode *inode, struct file *filep){
    printk("GPIO init \n");
    /* 校验 */
    if(!gpio_is_valid(LED_PIN)){
        printk("Error wrong gpio number !!\n");
        return;
    }
    gpio_request(LED_PIN, "led_ctr");
    /* 设置direction为输出 */
    gpio_direction_output(LED_PIN,1);
    /* 初始置为高电平 */
    gpio_set_value(LED_PIN,1);
    return 0;
}
 
/* 引脚电平写入 */
static int led_write(struct file *filep, const char __user *buf, size_t count, loff_t *ppos){
    int cnt = _copy_from_user(recv_msg, buf, count);
    if(0 == cnt){
        /* 如果用户输入了on标记，代表打开 */   
        if(0 == memcmp(recv_msg, "on", 2)){
            printk("LED on! \n");
            gpio_set_value(LED_PIN, 1);
        }
        /* 如果用户输入了其他标记，代表关闭 */
        else{
            printk("LED off! \n");
            gpio_set_value(LED_PIN, 0);
        }
    }else{
        printk("ERROR occur when writing!!\n");
        return -EIO;
    }
    return count;
}
 
/* led注销 */
static int led_release(struct inode *inode, struct file *filep){
    printk("Release !! \n");
    gpio_free(LED_PIN);
    return 0;
}
 
/* 设备文件操作对象 */
static const struct file_operations led_fops = {
    .owner   = THIS_MODULE,
    .open    = led_open,
    .write   = led_write,
    .release = led_release,
};
 
/* 设备操作的初始化  */
static int __init led_init(void){
 
    int ret = 0;
 
    //1. 注册字符设备
    ret = register_chrdev(LED_MAJOR, LED_NAME,&led_fops);
    if(ret <0){
        printk("register fail!\r\n");
        return -EIO;
    }
    printk("register success, major number is %d \r\n",ret);
    
    /* 2. 自动注册设备：首先创建类 */
    led_control_class = class_create(THIS_MODULE, LED_NAME);
    if(IS_ERR(led_control_class)){
        unregister_chrdev(LED_MAJOR, LED_NAME);
        return -EIO;
    }
 
    /* 3. 自动注册设备：基于类创建设备 */
    led_control_device = device_create(led_control_class, NULL, MKDEV(LED_MAJOR,0), NULL,LED_NAME);
    if(IS_ERR(led_control_device)){
        class_destroy(led_control_class);
        unregister_chrdev(LED_MAJOR, LED_NAME);
        return -EIO;
    }
 
    printk("led control device init success! \r\n");
 
    return 0;
}
 
/* 设备注销 */
static void __exit led_exit(void){
    printk(" led_exit \r\n");
    device_destroy(led_control_class, MKDEV(LED_MAJOR,0));
    class_unregister(led_control_class);
    class_destroy(led_control_class);
    unregister_chrdev(LED_MAJOR,LED_NAME);
}
 
module_init(led_init);
module_exit(led_exit);
 
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("CXSR");
