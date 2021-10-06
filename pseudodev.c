#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#define N_DEVICES  1
#define BASE_MINOR 0
const char* DEV_NAME = "pseudodev";

dev_t my_pseudo_dev;

static int __init pseudodev_init(void){
	int ret;
	ret = alloc_chrdev_region(&my_pseudo_dev, BASE_MINOR, N_DEVICES, DEV_NAME);
	if(ret){
		printk("pseudodev: Failed to register.\n");
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

int pseudodev_open(struct inode* inode, struct file* file){
	printk("Pseudo Devide: open method called.\n");
	return 0;	
}

int pseudodev_close(struct inode* inode, struct file* file){
	printk("Pseudo Device: close method called.\n");
	return 0;
}

module_init(pseudodev_init);
module_exit(pseudodev_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bgv");
MODULE_DESCRIPTION("Pseudo char device driver");
