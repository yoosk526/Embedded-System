#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("bootV5");
MODULE_DESCRIPTION("A simple gpio driver for segments");

/* Variables for device and device class */
// MAJOR(my_device_nr) : 주번호를 가져옴, MINOR(my_device_nr) : 부번호를 가져옴
static dev_t my_device_nr;			// 디바이스를 구분하기 위한 번호. Major : 상위 12 bit, Minor : 하위 20 bit
static struct class* my_class;		// 간단히 디바이스의 그룹이라고 할 수 있다.
static struct cdev my_device;		// 문자 디바이스 드라이버는 커널 내부에서 cdev 구조체로 관리된다.

#define DRIVER_NAME "my_buzzer"
#define DRIVER_CLASS "MyModuleClass_buz"


static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
    unsigned short value = 0;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	// user_buffer의 사용자 공간의 메모리에서 value에 읽어옴
	not_copied = copy_from_user(&value, user_buffer, to_copy);

    switch (value) {
    case '0':
        gpio_set_value(16, 0);
        break;
    case'1':
        gpio_set_value(16, 1);
        break;
    default:
        printk("Invalid Input!\n");
        break;
    }

    /* Calculate data */
    delta = to_copy - not_copied;
    return delta;
}

// 디바이스 파일이 열리면 실행되는 함수
static int driver_open(struct inode* device_file, struct file* instance) {
	printk("Buzzer - open was called!\n");
	return 0;
}

// 디바이스 파일이 닫히면 실행되는 함수
static int driver_close(struct inode* device_file, struct file* instance) {
	printk("Buzzer - close was called!\n");
	return 0;
}

// 디바이스 드라이버에서 지정된 함수를 디바이스 드라이버의 
// open/release, read/write 등의 용도에 대응할 수 있도록 지정
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	// .read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	// &my_device_nr에 주번호를 할당 받음
	// 0 : 부번호의 시작번호, 1 : 할당 받을 부번호의 개수, DRIVER_NAME : 디바이스 드라이버 이름
	// 할당 받는 것을 성공할 경우 0을 리턴
	if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}

	// cdev 구조체를 초기화하고, 커널에 등록
	cdev_init(&my_device, &fops);

	// 초기화한 cdev 구조체(my_device)를 커널에 등록
	if (cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}
	
	// sysfs에 우리가 만든 class가 등록됨
	if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

	// 위에서 cdev_add를 해주었지만, 아직 /dev 디렉토리에 디바이스 파일은 생성되지 않음
	// device_create() : 앞에서 등록한 문자 디바이스와 연결된 디바이스 파일을 만들어줌
	if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	printk("Successfully created Buzzer device : Nr. Major = %d, Minor = %d\n", MAJOR(my_device_nr), MINOR(my_device_nr));

	/* GPIO 16 init */
	if (gpio_request(16, "rpi-gpio-16")) {
		printk("Can not allocate GPIO 16\n");
		goto AddError;
	}

	/* Set GPIO 16 direction */
	if (gpio_direction_output(16, 0)) {
		printk("Can not set GPIO 16 to output!\n");
		goto Gpio16Error;
	}
    
	return 0;

Gpio16Error:
	gpio_free(16);
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
	gpio_free(16);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);