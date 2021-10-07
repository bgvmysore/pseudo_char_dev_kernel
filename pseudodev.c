#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define N_DEVICES  1
#define BASE_MINOR 0

const char* DEV_NAME = "pseudodev";

dev_t my_pseudo_dev;

struct cdev cdev1;

int pseudodev_open(struct inode* inode, struct file* file){
	printk("Pseudo Device: open method called.\n");
	return 0;	
}

int pseudodev_close(struct inode* inode, struct file* file){
	printk("Pseudo Device: close method called.\n");
	return 0;
}

ssize_t pseudodev_read(struct file* file, char __user *buff, size_t size, loff_t* off){
	printk("Pseudo Device: read method called.\n");
	return 0;
}

ssize_t pseudodev_write(struct file* file, const char __user *buff, size_t size, loff_t* off){
	printk("Pseudo Device: write method called.\n");
	return -ENOSPC;
}

struct file_operations fops = {
	.open = pseudodev_open,
	.release = pseudodev_close,
	.write = pseudodev_write,
	.read = pseudodev_read
};


static int __init pseudodev_init(void){
	int ret;
	int idev = 0;

	ret = alloc_chrdev_region(&my_pseudo_dev, BASE_MINOR, N_DEVICES, DEV_NAME);
	if(ret){
		printk("pseudodev: Failed to register.\n");
		return -EINVAL;
	}
	
	cdev_init(&cdev1, &fops);
	kobject_set_name(&cdev1.kobj, "pseudodev%d", idev);
	ret = cdev_add(&cdev1, my_pseudo_dev, 1);

	if(ret){
		printk("pseudodev: Failed to add device.\n");
		return -EINVAL;
	}

	printk("pseudodev: Successfully registered!...MajorID:%d, MinorID:%d\n", MAJOR(my_pseudo_dev), MINOR(my_pseudo_dev));
	printk("Pseudo Device: Welcome!\n");
	return 0;
}

static void __exit pseudodev_cleanup(void){
	unregister_chrdev_region(my_pseudo_dev, N_DEVICES);
	printk("Pseudo Device: Bye!\n");
}

module_init(pseudodev_init);
module_exit(pseudodev_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bgv");
MODULE_DESCRIPTION("Pseudo char device driver");
