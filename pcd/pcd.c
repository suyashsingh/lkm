/*
 * Pseudo Character Driver.
 * Works on memory area.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h> /* MAJOR(), MINOR() macros*/
#include <linux/cdev.h> /* struct cdev */
#include <linux/export.h> /* THIS_MODULE */
#include <linux/device.h> /* class_create(), device_create() */
#include <linux/uaccess.h> /* copy_from_user() */

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

#define NUM_DEVICES 1
#define BUFF_SIZE   500
/* Driver Buffer */
char drv_buff[BUFF_SIZE];

/* device functions */
loff_t pcd_llseek (struct file *filp, loff_t offset, int whence) {
	pr_info("llseek method called\n");
	return 0;
}

ssize_t pcd_read (struct file * filp, char __user * buff, size_t count, loff_t *offset) {
	pr_info("read method called, size = %zd, offset = %lld\n", count, *offset);
    
    if(count + *offset > BUFF_SIZE) {
        count = BUFF_SIZE - *offset;
    }

    if(copy_to_user(buff, &drv_buff[*offset], count)) {
        pr_err("error in copy to user\n");
        return -EFAULT;
    }

    /* Update the offset */
    *offset += count;
    pr_info("read method copied = %zd bytes, new offset = %lld\n", count, *offset);
	return count;
}

ssize_t pcd_write (struct file *filp, const char __user * buff, size_t count, loff_t *offset){
    pr_info("write method called, size = %zd, offset = %lld\n", count, *offset);

    if(count + *offset > BUFF_SIZE) {
        count = BUFF_SIZE - *offset;
    }

    if(!count) {
        pr_err("No space left on device!\n");
        return -ENOMEM;
    }

    if(copy_from_user(&drv_buff[*offset], buff, count)) {
        return -EFAULT;
    }    
    
    /* update the offset */
    *offset += count;

    pr_info("successfully written %zd bytes, new offset = %lld\n", count, *offset);
	return count;

	/*
	 *return 0;
	 * BAD IDEA! userspace progams(echo) would keep retrying 
	*/
}

int pcd_open (struct inode *inode, struct file *filp) {
	pr_info("open method called\n");
	return 0;
}

int pcd_release (struct inode *inode, struct file *filp) {
	pr_info("release method called\n");
	return 0;
}

dev_t device_number;
struct cdev pcd_cdev;
struct class *pcd_class;
struct device *pcd_device;

struct file_operations pcd_fops = {
	.owner	=	THIS_MODULE,
	.open	=	pcd_open,
	.read	=	pcd_read,
	.write	=	pcd_write,
	.llseek	=	pcd_llseek,
	.release =	pcd_release
};

static int __init pcd_init(void) {
    int ret;
    /* 1. allocatate device number */
    ret = alloc_chrdev_region(&device_number, 0, NUM_DEVICES, "pcd");
    if(ret < 0) {
    	pr_err("Error allocating device number\n");
    	goto out;
    }

    /* initialize and add cdev structure */
    cdev_init(&pcd_cdev, &pcd_fops);

    /* add char device to the system */
    ret = cdev_add(&pcd_cdev, device_number, NUM_DEVICES);
    if(ret < 0) {
    	pr_err("Error adding char device to system\n");
    	goto undo_device_num_reg;
    }

    /* create device class */
    pcd_class = class_create(THIS_MODULE, "pcd");
    if(IS_ERR(pcd_class)) {
    	pr_err("class_create() failed\n");
    	goto undo_cdev_add;
    }
    /* create a device and register it with sysfs */
    pcd_device = device_create(pcd_class, NULL, device_number, NULL, "pcd_%d", 0);
    if(IS_ERR(pcd_device)) {
    	pr_err("device_create() failed\n");
    	goto undo_class_create;
    }

    pr_info("Hello char module loaded successfully!\n");
    pr_info("allocated device number <major>: <minor> = %u: %u\n", MAJOR(device_number), MINOR(device_number));
    return 0;

undo_class_create:
	device_destroy(pcd_class, device_number);
undo_cdev_add:
	cdev_del(&pcd_cdev);
undo_device_num_reg:
	unregister_chrdev_region(device_number, NUM_DEVICES);
out:
	return ret;
}

static void __exit pcd_exit(void) {    
    device_destroy(pcd_class, device_number);
    class_destroy(pcd_class);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, NUM_DEVICES);
    pr_info("Hello char module unloaded. Bye!\n");
}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_AUTHOR("Suyash");
MODULE_DESCRIPTION("Simple character driver with dummy functions");
MODULE_LICENSE("GPL");