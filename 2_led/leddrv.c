#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include "led_opr.h"


/* 1. 确定主设备号                                                                 */
static int major = 0;
static struct class *led_class;
struct led_operations *p_led_opr;


/* 3. 实现对应的open/read/write等函数，填入file_operations结构体                   */

static int led_drv_open (struct inode *node, struct file *file)
{
	int minor = iminor(node);
	
	/* 根据次设备号初始化LED */
	p_led_opr->init(minor);
	
	return 0;
}

static int led_drv_close (struct inode *node, struct file *file)
{
	return 0;
}

static ssize_t led_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	return 0;
}

/* write(fd, &val, 1); */
static ssize_t led_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	char status;
	struct inode *node = file_inode(file);
	int minor = iminor(node);

	err = copy_from_user(&status, buf, 1);

	/* 根据次设备号和status控制LED */
	p_led_opr->ctl(minor, status);
	
	return 1;
}


/* 2. 定义自己的file_operations结构体                                              */
static struct file_operations led_drv = {
	.owner	 = THIS_MODULE,
	.open    = led_drv_open,
	.read    = led_drv_read,
	.write   = led_drv_write,
	.release = led_drv_close,
};

/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数 */
static int __init led_init(void)
{
	int err;
	int i;
	
	printk("LED init \r\n");

	/* 4. 把file_operations结构体告诉内核：注册驱动程序                                */
	major = register_chrdev(0, "led", &led_drv);  /* /dev/led */

	/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */
	led_class = class_create(THIS_MODULE, "led_class");
	err = PTR_ERR(led_class);
	if (IS_ERR(led_class)) {
		unregister_chrdev(major, "led");
		return -1;
	}

	/* 注意要在创建设备之前获得led_operaions结构体(需要用到其中的num) */
	p_led_opr = get_board_led_opr();
	for (i = 0; i < p_led_opr->num; i++)
		device_create(led_class, NULL, MKDEV(major, i), NULL, "led%d", i); /* /dev/led0,1,... */

	
	return 0;
}

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */
static void __exit led_exit(void)
{
	int i;

	printk("LED exit \r\n");

	for (i = 0; i < p_led_opr->num; i++)
		device_destroy(led_class, MKDEV(major, i)); /* /dev/led0,1,... */
    
	class_destroy(led_class);
	unregister_chrdev(major, "led");
}


module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
