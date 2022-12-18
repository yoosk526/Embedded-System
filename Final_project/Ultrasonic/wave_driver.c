#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("BootV5");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED and reading a button");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_wave"
#define DRIVER_CLASS "MyModuleClass_wave"

static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
    unsigned short value = 0;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	// user_buffer의 사용자 공간의 메모리에서 value에 읽어옴
	not_copied = copy_from_user(&value, user_buffer, to_copy);

    switch (value) {
    case '0':
        gpio_set_value(19, 0);
        break;
    case'1':
        gpio_set_value(19, 1);
        break;
    default:
        printk("Invalid Input!\n");
        break;
    }

    /* Calculate data */
    delta = to_copy - not_copied;
    return delta;
}

static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs) {
 	int to_copy, not_copied, delta;
	char tmp;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	tmp = gpio_get_value(13) + '0';

	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

static int driver_open(struct inode* device_file, struct file* instance) {
	printk("wave - open was called!\n");
	return 0;
}

static int driver_close(struct inode* device_file, struct file* instance) {
	printk("wave - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}

	cdev_init(&my_device, &fops);

	if (cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}
	
	if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

	if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	printk("Successfully created Ultrasonic device : Nr. Major = %d, Minor = %d\n", MAJOR(my_device_nr), MINOR(my_device_nr));

	if (gpio_request(19, "rpi-gpio-19")) {
		printk("Can not allocate GPIO 19\n");
		goto AddError;
	}

	if (gpio_direction_input(19)) {
		printk("Can not set GPIO 19 to input!\n");
		goto Gpio19Error;
	}

	if (gpio_request(13, "rpi-gpio-13")) {
		printk("Can not allocate GPIO 13\n");
		goto AddError;
	}

	if (gpio_direction_output(13, 0)) {
		printk("Can not set GPIO 13 to output!\n");
		goto Gpio13Error;
	}

	return 0;

Gpio19Error:
	gpio_free(19);
Gpio13Error:
	gpio_free(13);
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	gpio_free(19);
	gpio_free(13);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);