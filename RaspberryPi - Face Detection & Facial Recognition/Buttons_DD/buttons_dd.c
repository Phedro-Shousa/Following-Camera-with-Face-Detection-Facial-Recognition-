#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>

#include <linux/io.h>
//#include <mach/platform.h>

#include "utils.h"



#define DEVICE_NAME "buttons"
#define CLASS_NAME "buttonsClass"




MODULE_LICENSE("GPL");

/* Device variables */
static struct class* buttonsDevice_class = NULL;
static struct device* buttonsDevice_device = NULL;
static dev_t buttonsDevice_majorminor;
static struct cdev c_dev;  // Character device structure


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;
struct GpioRegisters *s_pGpioRegisters;

static const int InputButtonGpioPin = 20;
static const int OutputButtonGpioPin = 21;

ssize_t buttons_device_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) { 

	struct GpioRegisters *pdev; 
	
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GpioRegisters *)pfile->private_data;
	if (pbuff[0]=='0')
		SetGPIOOutputValue(pdev, OutputButtonGpioPin, 0);
	else
		SetGPIOOutputValue(pdev, OutputButtonGpioPin, 1);
	return len;
}

ssize_t buttons_device_read(struct file *pfile, char __user *p_buff,size_t len, loff_t *poffset){
	struct GpioRegisters *pdev;	
	
	//pr_alert("%s: called (%u)\n",__FUNCTION__,len);

	
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GpioRegisters *)pfile->private_data;
	p_buff[0]=GetGPIOInputValue(pdev, InputButtonGpioPin)+48;
	
	//pr_alert("%s: register value is %s\n",__FUNCTION__,p_buff);
	
	return 0;
}

int buttons_device_close(struct inode *p_inode, struct file * pfile){
	
	//pr_alert("%s: called\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}


int buttons_device_open(struct inode* p_indode, struct file *p_file){

	//pr_alert("%s: called\n",__FUNCTION__);
	p_file->private_data = (struct GpioRegisters *) s_pGpioRegisters;
	return 0;
	
}


static struct file_operations buttonsDevice_fops = {
	.owner = THIS_MODULE,
	.write = buttons_device_write,
	.read = buttons_device_read,
	.release = buttons_device_close,
	.open = buttons_device_open,
};

static int __init buttonsModule_init(void) {
	int ret;
	struct device *dev_ret;

	//pr_alert("%s: called\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&buttonsDevice_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(buttonsDevice_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(buttonsDevice_majorminor, 1);
		return PTR_ERR(buttonsDevice_class);
	}
	if (IS_ERR(dev_ret = device_create(buttonsDevice_class, NULL, buttonsDevice_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(buttonsDevice_class);
		unregister_chrdev_region(buttonsDevice_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &buttonsDevice_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, buttonsDevice_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(buttonsDevice_class, buttonsDevice_majorminor);
		class_destroy(buttonsDevice_class);
		unregister_chrdev_region(buttonsDevice_majorminor, 1);
		return ret;
	}


	//s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));
	s_pGpioRegisters = (struct GpioRegisters *)ioremap_nocache(GPIO_BASE, sizeof(struct GpioRegisters));
	
	//pr_alert("map to virtual adresse: 0x%x\n", (unsigned)s_pGpioRegisters);
	
	//make sure they are configured as input
	SetGPIOFunction(s_pGpioRegisters, InputButtonGpioPin, 0); //Input
	SetGPIOFunction(s_pGpioRegisters, OutputButtonGpioPin, 0b001); //Output

	return 0;
}

static void __exit buttonsModule_exit(void) {
	
	//pr_alert("%s: called\n",__FUNCTION__);
	
	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(buttonsDevice_class, buttonsDevice_majorminor);
	class_destroy(buttonsDevice_class);
	unregister_chrdev_region(buttonsDevice_majorminor, 1);
}

module_init(buttonsModule_init); //sets the specified functions as init and exit of the module on the eyes of the kernel
module_exit(buttonsModule_exit);
