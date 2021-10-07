#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define N_DEVICES  1
#define BASE_MINOR 0
#define MAX_SIZE   1024

const char* DEV_NAME = "pseudodev";

dev_t my_pseudo_dev;

struct class *pseudocls;
struct device *pseudodevice;

struct cdev cdev1;

unsigned char *pseudo_buff;
int read_offset  = 0;
int write_offset = 0;
int buff_len     = 0;

int pseudodev_open(struct inode* inode, struct file* file){
	printk("Pseudo Device: open method called.\n");
	return 0;	
}

int pseudodev_close(struct inode* inode, struct file* file){
	printk("Pseudo Device: close method called.\n");
	return 0;
}

ssize_t pseudodev_read(struct file* file, char __user *buff, size_t size, loff_t* off){
	
	int ret = 0;
	int rcnt = 0;	
	
	printk("Pseudo Device: read method called.\n");

	if(buff_len == 0){
		printk("Pseudo Device: Empty.\n");
		return 0;
	}
	
	rcnt = size;
	if(rcnt > buff_len){
		rcnt = buff_len;
	}

	ret = copy_to_user(buff, pseudo_buff + read_offset, rcnt);
	if(ret){
		printk("Pseudo Device: Copy to user space failed.\n");
		return -EFAULT;
	}
	
	read_offset += rcnt;
	buff_len -= rcnt;	

	return rcnt;
}

ssize_t pseudodev_write(struct file* file, const char __user *buff, size_t size, loff_t* off){
	
	int ret = 0;
	int wcnt = 0;	

	printk("Pseudo Device: write method called.\n");

	if(write_offset >= MAX_SIZE){
		printk("Pseudo Device: buffer full.\n");
		return -ENOSPC;
	}
	
	wcnt = size;
	if(wcnt > MAX_SIZE - write_offset){
		wcnt = (MAX_SIZE - write_offset);
	}
	
	ret = copy_from_user(pseudo_buff + write_offset, buff, wcnt);
	if(ret){
		printk("Pseudo Device: Copy from user space failed.\n");
		return -EFAULT;
	}
	
	write_offset += wcnt;
	buff_len += wcnt;
	return wcnt;
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
	
	pseudocls = class_create(THIS_MODULE, "pseudo_class");
	if(pseudocls == NULL){
		printk("pseudodev: Failed to create pseudo class.\n");
		return -EINVAL;
	}

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

	pseudodevice = device_create(pseudocls, NULL, my_pseudo_dev, NULL, "psample%d", idev);
	if(pseudodevice == NULL){
		printk("pseudodev: Failed to create psedudo device.\n");
		return -EINVAL;
	}
	
	pseudo_buff = kmalloc(MAX_SIZE, GFP_KERNEL);
	if(pseudo_buff == NULL){
		printk("pseudodev: kmalloc failed.\n");
		return -ENOMEM;
	}
	
	printk("pseudodev: Successfully registered!...MajorID:%d, MinorID:%d\n", MAJOR(my_pseudo_dev), MINOR(my_pseudo_dev));
	printk("Pseudo Device: Welcome!\n");
	return 0;
}

static void __exit pseudodev_cleanup(void){
	kfree(pseudo_buff);	

	device_destroy(pseudocls, my_pseudo_dev);
	unregister_chrdev_region(my_pseudo_dev, N_DEVICES);
	class_destroy(pseudocls);
	printk("Pseudo Device: Bye!\n");

}

module_init(pseudodev_init);
module_exit(pseudodev_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bgv");
MODULE_DESCRIPTION("Pseudo char device driver");
