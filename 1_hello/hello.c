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

#define MIN(a, b) (a < b ? a : b)

/* 1. 确定主设备号                                                                 */
static int major = 0;
static struct class *hello_class;

static char write_buf[100];
static char read_buf[100];
static char kernel_data[] = "This is kernel data!";

/* 3. 实现对应的open/read/write等函数，填入file_operations结构体                   */

/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int hello_open (struct inode *inode, struct file *filp)
{
	return 0;
}
/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int hello_close (struct inode *inode, struct file *filp)
{
	return 0;
}
/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t hello_read (struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int ret;

    memcpy(read_buf, kernel_data, sizeof(kernel_data));
    ret = copy_to_user(buf, read_buf, MIN(100, cnt));
    if(ret < 0){
        printk("kernel send data failed!\r\n");
        return -1;
    }else{
        printk("kernel send data ok!\r\n");
        return MIN(100, cnt);
    }

    return 0;    
}
/*
 * @description		: 向设备写数据 
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - buf 	: 要写给设备写入的数据
 * @param - cnt 	: 要写入的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static ssize_t hello_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    int ret;

    ret = copy_from_user(write_buf, buf, MIN(100, cnt));
    if(ret < 0){
        printk("kernel receive data failed!\r\n");
        return -1;
    }else{
        printk("kernel receive data: %s \r\n", write_buf);
        return MIN(100, cnt);
    }

    return 0;
}

/* 2. 定义自己的file_operations结构体                                              */

/*
 * 设备操作函数结构体 注意成员之间是逗号隔开的，结构最后还有个分号
 */
static struct file_operations hello = {
	.owner 		= THIS_MODULE,
	.open 		= hello_open,
	.read 		= hello_read,
	.write 		= hello_write,
	.release 	= hello_close,
};

/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数           */

/*
 * @description	: 驱动入口函数 
 * @param 		: 无
 * @return 		: 0 成功;其他 失败
 */
static int __init hello_init(void)
{   
    int err;

    printk("hello init\r\n");
    /* 4. 把file_operations结构体告诉内核：注册驱动程序                                 */
    major = register_chrdev(0, "hello", &hello);	/* /dev/hello */

    /* 7. 其他完善：提供设备信息，自动创建设备节点                                     */
    hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		unregister_chrdev(major, "hello");
		return -1;
	}
	device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello"); /* /dev/hello */

    return 0;
}

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */

/*
 * @description	: 驱动出口函数
 * @param 		: 无
 * @return 		: 无
 */
static void __exit hello_exit(void)
{
    printk("hello exit\r\n");

    device_destroy(hello_class, MKDEV(major, 0));
	class_destroy(hello_class);
	unregister_chrdev(major, "hello");
}

/* 
 * 指定为驱动入口和出口函数，以及LICENSE信息 
 */
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");