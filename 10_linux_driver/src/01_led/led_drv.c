#include "asm-generic/errno-base.h"
#include "asm-generic/gpio.h"
#include "asm/uaccess.h"
#include <linux/module.h>
#include <linux/poll.h>

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
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/timer.h>

struct gpio_desc{
	int gpio;
	int irq;
    char *name;
    int key;
	struct timer_list key_timer;
} ;

static struct gpio_desc gpios[2] = {
    {131, 0, "led0", },
    //{132, 0, "led1", },
};

/* 主设备号                                                                 */
static int major = 0;
static struct class *gpio_class;


/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t gpio_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	char tmp_buf[2];
	int err;
    int count = sizeof(gpios)/sizeof(gpios[0]);

	if (size != 2)
		return -EINVAL;

	err = copy_from_user(tmp_buf, buf, 1);

	if (tmp_buf[0] >= count)
		return -EINVAL;

	tmp_buf[1] = gpio_get_value(gpios[tmp_buf[0]].gpio);

	err = copy_to_user(buf, tmp_buf, 2);
	
	return 2;
}

static ssize_t gpio_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    unsigned char ker_buf[2];
    int err;

    if (size != 2)
        return -EINVAL;

    err = copy_from_user(ker_buf, buf, size);
    
    if (ker_buf[0] >= sizeof(gpios)/sizeof(gpios[0]))
        return -EINVAL;

    gpio_set_value(gpios[ker_buf[0]].gpio, ker_buf[1]);
    return 2;    
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations gpio_key_drv = {
	.owner	 = THIS_MODULE,
	.read    = gpio_drv_read,
	.write   = gpio_drv_write,
};


/* 在入口函数 */
static int __init gpio_drv_init(void)
{
    int err;
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	for (i = 0; i < count; i++)
	{		
		/* set pin as output */
		err = gpio_request(gpios[i].gpio, gpios[i].name);
		if (err < 0) {
			printk("can not request gpio %s %d\n", gpios[i].name, gpios[i].gpio);
			return -ENODEV;
		}
		
		gpio_direction_output(gpios[i].gpio, 1);
	}

	/* 注册file_operations 	*/
	major = register_chrdev(0, "100ask_led", &gpio_key_drv);  /* /dev/gpio_desc */

	gpio_class = class_create(THIS_MODULE, "100ask_led_class");
	if (IS_ERR(gpio_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "100ask_led_class");
		return PTR_ERR(gpio_class);
	}

	device_create(gpio_class, NULL, MKDEV(major, 0), NULL, "100ask_led"); /* /dev/100ask_gpio */
	
	return err;
}

/* 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 */
static void __exit gpio_drv_exit(void)
{
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	device_destroy(gpio_class, MKDEV(major, 0));
	class_destroy(gpio_class);
	unregister_chrdev(major, "100ask_led");

	for (i = 0; i < count; i++)
	{
		gpio_free(gpios[i].gpio);		
	}
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);

MODULE_LICENSE("GPL");


