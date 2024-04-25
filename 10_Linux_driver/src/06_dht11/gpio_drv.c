#include "asm-generic/errno-base.h"
#include "asm-generic/gpio.h"
#include "linux/jiffies.h"
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/delay.h>
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

static struct gpio_desc gpios[] = {
    {115, 0, "dht11", },
};

/* 主设备号                                                                 */
static int major = 0;
static struct class *gpio_class;

static u64 g_dht11_irq_time[84];
static int g_dht11_irq_cnt = 0;

/* 环形缓冲区 */
#define BUF_LEN 128
static char g_keys[BUF_LEN];
static int r, w;

struct fasync_struct *button_fasync;

static irqreturn_t dht11_isr(int irq, void *dev_id);
static void parse_dht11_datas(void);

#define NEXT_POS(x) ((x+1) % BUF_LEN)

static int is_key_buf_empty(void)
{
	return (r == w);
}

static int is_key_buf_full(void)
{
	return (r == NEXT_POS(w));
}

static void put_key(char key)
{
	if (!is_key_buf_full())
	{
		g_keys[w] = key;
		w = NEXT_POS(w);
	}
}

static char get_key(void)
{
	char key = 0;
	if (!is_key_buf_empty())
	{
		key = g_keys[r];
		r = NEXT_POS(r);
	}
	return key;
}


static DECLARE_WAIT_QUEUE_HEAD(gpio_wait);

// static void key_timer_expire(struct timer_list *t)
static void key_timer_expire(unsigned long data)
{
	// 解析数据, 放入环形buffer, 唤醒APP
	parse_dht11_datas();
}


/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t dht11_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	char kern_buf[2];

	if (size != 2)
		return -EINVAL;

	g_dht11_irq_cnt = 0;

	/* 1. 发送18ms的低脉冲 */
	err = gpio_request(gpios[0].gpio, gpios[0].name);
	gpio_direction_output(gpios[0].gpio, 0);
	gpio_free(gpios[0].gpio);

	mdelay(18);
	gpio_direction_input(gpios[0].gpio);  /* 引脚变为输入方向, 由上拉电阻拉为1 */

	/* 2. 注册中断 */
	err = request_irq(gpios[0].irq, dht11_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, gpios[0].name, &gpios[0]);
	mod_timer(&gpios[0].key_timer, jiffies + 10);	

	/* 3. 休眠等待数据 */
	wait_event_interruptible(gpio_wait, !is_key_buf_empty());

	free_irq(gpios[0].irq, &gpios[0]);

	//printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 设置DHT11 GPIO引脚的初始状态: output 1 */
	err = gpio_request(gpios[0].gpio, gpios[0].name);
	if (err)
	{
		printk("%s %s %d, gpio_request err\n", __FILE__, __FUNCTION__, __LINE__);
	}
	gpio_direction_output(gpios[0].gpio, 1);
	gpio_free(gpios[0].gpio);


	/* 4. copy_to_user */
	kern_buf[0] = get_key();
	kern_buf[1] = get_key();

	printk("get val : 0x%x, 0x%x\n", kern_buf[0], kern_buf[1]);
	if ((kern_buf[0] == (char)-1) && (kern_buf[1] == (char)-1))
	{
		printk("get err val\n");
		return -EIO;
	}

	err = copy_to_user(buf, kern_buf, 2);
	
	return 2;
}

static int dht11_release (struct inode *inode, struct file *filp)
{
	return 0;
}


/* 定义自己的file_operations结构体                                              */
static struct file_operations dht11_drv = {
	.owner	 = THIS_MODULE,
	.read    = dht11_read,
	.release = dht11_release,
};

static void parse_dht11_datas(void)
{
	int i;
	u64 high_time;
	unsigned char data = 0;
	int bits = 0;
	unsigned char datas[5];
	int byte = 0;
	unsigned char crc;

	/* 数据个数: 可能是81、82、83、84 */
	if (g_dht11_irq_cnt < 81)
	{
		/* 出错 */
		put_key(-1);
		put_key(-1);

		// 唤醒APP
		wake_up_interruptible(&gpio_wait);
		g_dht11_irq_cnt = 0;
		return;
	}

	// 解析数据
	for (i = g_dht11_irq_cnt - 80; i < g_dht11_irq_cnt; i+=2)
	{
		high_time = g_dht11_irq_time[i] - g_dht11_irq_time[i-1];

		data <<= 1;

		if (high_time > 50000) /* data 1 */
		{
			data |= 1;
		}

		bits++;

		if (bits == 8)
		{
			datas[byte] = data;
			data = 0;
			bits = 0;
			byte++;
		}
	}

	// 放入环形buffer
	crc = datas[0] + datas[1] + datas[2] + datas[3];
	if (crc == datas[4])
	{
		put_key(datas[0]);
		put_key(datas[2]);
	}
	else
	{
		put_key(-1);
		put_key(-1);
	}

	g_dht11_irq_cnt = 0;
	// 唤醒APP
	wake_up_interruptible(&gpio_wait);
}

static irqreturn_t dht11_isr(int irq, void *dev_id)
{
	struct gpio_desc *gpio_desc = dev_id;
	u64 time;
	
	/* 1. 记录中断发生的时间 */
	time = ktime_get_ns();
	g_dht11_irq_time[g_dht11_irq_cnt] = time;

	/* 2. 累计次数 */
	g_dht11_irq_cnt++;

	/* 3. 次数足够: 解析数据, 放入环形buffer, 唤醒APP */
	if (g_dht11_irq_cnt == 84)
	{
		del_timer(&gpio_desc->key_timer);
		parse_dht11_datas();
	}

	return IRQ_HANDLED;
}


/* 在入口函数 */
static int __init dht11_init(void)
{
    int err;
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	for (i = 0; i < count; i++)
	{		
		gpios[i].irq  = gpio_to_irq(gpios[i].gpio);

		/* 设置DHT11 GPIO引脚的初始状态: output 1 */
		err = gpio_request(gpios[i].gpio, gpios[i].name);
		gpio_direction_output(gpios[i].gpio, 1);
		gpio_free(gpios[i].gpio);

		setup_timer(&gpios[i].key_timer, key_timer_expire, (unsigned long)&gpios[i]);
	 	//timer_setup(&gpios[i].key_timer, key_timer_expire, 0);
		//gpios[i].key_timer.expires = ~0;
		//add_timer(&gpios[i].key_timer);
		//err = request_irq(gpios[i].irq, dht11_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "100ask_gpio_key", &gpios[i]);
	}

	/* 注册file_operations 	*/
	major = register_chrdev(0, "100ask_dht11", &dht11_drv);  /* /dev/gpio_desc */

	gpio_class = class_create(THIS_MODULE, "100ask_dht11_class");
	if (IS_ERR(gpio_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "100ask_dht11");
		return PTR_ERR(gpio_class);
	}

	device_create(gpio_class, NULL, MKDEV(major, 0), NULL, "mydht11"); /* /dev/mydht11 */
	
	return err;
}

/* 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 */
static void __exit dht11_exit(void)
{
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	device_destroy(gpio_class, MKDEV(major, 0));
	class_destroy(gpio_class);
	unregister_chrdev(major, "100ask_dht11");

	for (i = 0; i < count; i++)
	{
		//free_irq(gpios[i].irq, &gpios[i]);
		//del_timer(&gpios[i].key_timer);
	}
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(dht11_init);
module_exit(dht11_exit);

MODULE_LICENSE("GPL");


