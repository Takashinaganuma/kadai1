#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <asm/delay.h>

MODULE_AUTHOR("Takashinaganuma");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;

static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	char c;
	int i;
	int j;
	if(copy_from_user(&c,buf,sizeof(char)))
		return -EFAULT;

	if(c == '0'){
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 24;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 22;
	}
	else if(c == 'a'){
		for(i=0; i< 10; i++){
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 24;
		gpio_base[7] = 1 << 23;
		gpio_base[7] = 1 << 22;
		for(j=0; j < 1000; j++){
		udelay(500);}
		gpio_base[10] = 1 << 25;
		gpio_base[10] = 1 << 24;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 22;
		for(j=0; j < 1000; j++){
		udelay(500);}
		}
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 24;
		gpio_base[7] = 1 << 23;
		gpio_base[7] = 1 << 22;
	}
	else if(c == 'b'){
		for(i=0; i< 10; i++){
		gpio_base[7] = 1 << 25;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 25;
		gpio_base[7] = 1 << 24;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 24;
		gpio_base[7] = 1 << 23;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 22;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 22;
		}
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 24;
		gpio_base[7] = 1 << 23;
		gpio_base[7] = 1 << 22;
	}
	else if(c == 'c'){
		for(i=0; i< 10; i++){
		gpio_base[7] = 1 << 22;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 22;
		gpio_base[7] = 1 << 23;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 24;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 24;
		gpio_base[7] = 1 << 25;
		for(j=0; j < 1000; j++){
		udelay(300);}
		gpio_base[10] = 1 << 25;
		}
		gpio_base[7] = 1 << 25;
		gpio_base[7] = 1 << 24;
		gpio_base[7] = 1 << 23;
		gpio_base[7] = 1 << 22;
	}
        return 1;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write
};

static int __init init_mod(void)
{
	int retval;

	gpio_base = ioremap_nocache(0x3f200000, 0xA0); //0x3f..:base address, 0xA0: region to map

	const u32 led = 25;
	const u32 index = led/10;//GPFSEL2
	const u32 shift = (led%10)*3;//15bit
	const u32 mask = ~(0x7 << shift);//11111111111111000111111111111111
	gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);//001: output flag
	//11111111111111001111111111111111
	
	//22
	const u32 led2 = 22;
	const u32 index2 = led2/10;//GPFSEL2
	const u32 shift2 = (led2%10)*3;//15bit
	const u32 mask2 = ~(0x7 << shift2);//11111111111111000111111111111111
	gpio_base[index] = (gpio_base[index] & mask2) | (0x1 << shift2);//001: output flag

	//23
	const u32 led3 = 23;
	const u32 index3 = led3/10;//GPFSEL2
	const u32 shift3 = (led3%10)*3;//15bit
	const u32 mask3 = ~(0x7 << shift3);//11111111111111000111111111111111
	gpio_base[index] = (gpio_base[index] & mask3) | (0x1 << shift3);//001: output flag

	//24
	const u32 led4 = 24;
	const u32 index4 = led4/10;//GPFSEL2
	const u32 shift4 = (led4%10)*3;//15bit
	const u32 mask4 = ~(0x7 << shift4);//11111111111111000111111111111111
	gpio_base[index] = (gpio_base[index] & mask4) | (0x1 << shift4);//001: output flag
	
	retval =  alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	cdv.owner = THIS_MODULE;
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE,"myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));

	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
	iounmap(gpio_base);
}

module_init(init_mod);
module_exit(cleanup_mod);
