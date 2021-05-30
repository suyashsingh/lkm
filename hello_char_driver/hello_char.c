/*
 * Hello char Kernel Module.
 * lkm with dummy read, write, llseek & release functions
 * 
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h> /* MAJOR(), MINOR() macros*/
#include <linux/cdev.h> /* struct cdev */
#include <linux/export.h> /* THIS_MODULE */
#include <linux/device.h> /* class_create(), device_create() */

#define NUM_DEVICES 1

/* device functions */
loff_t hello_char_llseek (struct file *filp, loff_t offset, int whence) {
	pr_info("llseek method called\n");
	return 0;
}

ssize_t hello_char_read (struct file * filp, char __user * buff, size_t count, loff_t *offset) {
	pr_info("read method called\n");
	return 0;
}

ssize_t hello_char_write (struct file *filp, const char __user * buff, size_t count, loff_t *offset){
	pr_info("write method called\n");
	return count;
	/*
	 *return 0;
	 * BAD IDEA! userspace progams(echo) would keep retrying 
	*/
}

int hello_char_open (struct inode *inode, struct file *filp) {
	pr_info("open method called\n");
	return 0;
}

int hello_char_release (struct inode *inode, struct file *filp) {
	pr_info("release method called\n");
	return 0;
}

dev_t device_number;
struct cdev hello_char_cdev;
struct class *hello_char_class;
struct device *hello_char_device;

struct file_operations hello_char_fops = {
	.owner	=	THIS_MODULE,
	.open	=	hello_char_open,
	.read	=	hello_char_read,
	.write	=	hello_char_write,
	.llseek	=	hello_char_llseek,
	.release =	hello_char_release
};

static int __init hello_char_init(void) {
    int ret;
    /* 1. allocatate device number */
    ret = alloc_chrdev_region(&device_number, 0, NUM_DEVICES, "hello_char");
    if(ret < 0) {
    	pr_err("Error allocating device number\n");
    	goto out;
    }

    /* initialize and add cdev structure */
    cdev_init(&hello_char_cdev, &hello_char_fops);

    /* add char device to the system */
    ret = cdev_add(&hello_char_cdev, device_number, NUM_DEVICES);
    if(ret < 0) {
    	pr_err("Error adding char device to system\n");
    	goto undo_device_num_reg;
    }

    /* create device class */
    hello_char_class = class_create(THIS_MODULE, "hello_char");
    if(IS_ERR(hello_char_class)) {
    	pr_err("class_create() failed\n");
    	goto undo_cdev_add;
    }
    /* create a device and register it with sysfs */
    hello_char_device = device_create(hello_char_class, NULL, device_number, NULL, "hello_char_%d", 0);
    if(IS_ERR(hello_char_device)) {
    	pr_err("device_create() failed\n");
    	goto undo_class_create;
    }

    pr_info("Hello char module loaded successfully!\n");
    pr_info("allocated device number <major>: <minor> = %u: %u\n", MAJOR(device_number), MINOR(device_number));
    return 0;

undo_class_create:
	device_destroy(hello_char_class, device_number);
undo_cdev_add:
	cdev_del(&hello_char_cdev);
undo_device_num_reg:
	unregister_chrdev_region(device_number, NUM_DEVICES);
out:
	return ret;
}

static void __exit hello_char_exit(void) {    
    device_destroy(hello_char_class, device_number);
    class_destroy(hello_char_class);
    cdev_del(&hello_char_cdev);
    unregister_chrdev_region(device_number, NUM_DEVICES);
    pr_info("Hello char module unloaded. Bye!\n");
}

module_init(hello_char_init);
module_exit(hello_char_exit);

MODULE_AUTHOR("Suyash");
MODULE_DESCRIPTION("Simple character driver with dummy functions");
MODULE_LICENSE("GPL");