#include "acpi/acoutput.h"
#include "asm-generic/errno-base.h"
#include "asm-generic/gpio.h"
#include "asm/gpio.h"
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

static struct gpio_desc gpios[] = {
    {115, 0, "ds18b20", },
};

/* 主设备号                                                                 */
static int major = 0;
static struct class *gpio_class;

static spinlock_t ds18b20_spinlock;


static void ds18b20_udelay(int us)
{
	u64 time = ktime_get_ns();
	while (ktime_get_ns() - time < us*1000);
}

static int ds18b20_reset_and_wait_ack(void)
{
	int timeout = 100;

	gpio_set_value(gpios[0].gpio, 0);
	ds18b20_udelay(480);

	gpio_direction_input(gpios[0].gpio);

	/* 等待ACK */
	while (gpio_get_value(gpios[0].gpio) && timeout--)
	{
		ds18b20_udelay(1);
	}

	if (timeout == 0)
		return -EIO;

	/* 等待ACK结束 */
	timeout = 300;
	while (!gpio_get_value(gpios[0].gpio) && timeout--)
	{
		ds18b20_udelay(1);
	}
	if (timeout == 0)
		return -EIO;
	
	return 0;

}

static void ds18b20_send_cmd(unsigned char cmd)
{
	int i;
	
	gpio_direction_output(gpios[0].gpio, 1);

	for (i = 0; i < 8; i++)
	{
		if (cmd & (1<<i)) 
		{
			/* 发送1 */
			gpio_direction_output(gpios[0].gpio, 0);
			ds18b20_udelay(2);
			gpio_direction_output(gpios[0].gpio, 1);
			ds18b20_udelay(60);
		}
		else
		{
			/* 发送0 */
			gpio_direction_output(gpios[0].gpio, 0);
			ds18b20_udelay(60);		
			gpio_direction_output(gpios[0].gpio, 1);
		}
	}
}

static void ds18b20_read_data(unsigned char *buf)
{
	int i;
	unsigned char data = 0;

	gpio_direction_output(gpios[0].gpio, 1);
	for (i = 0; i < 8; i++)
	{
		gpio_direction_output(gpios[0].gpio, 0);
		ds18b20_udelay(2);
		gpio_direction_input(gpios[0].gpio);
		ds18b20_udelay(15);
		if (gpio_get_value(gpios[0].gpio))
		{
			data |= (1<<i);
		}
		ds18b20_udelay(50);
		gpio_direction_output(gpios[0].gpio, 1);
	}

	buf[0] = data;
}

/********************************************************/  
/*DS18B20的CRC8校验程序*/  
/********************************************************/   
/* 参考: https://www.cnblogs.com/yuanguanghui/p/12737740.html */   
static unsigned char calcrc_1byte(unsigned char abyte)   
{   
	unsigned char i,crc_1byte;     
	crc_1byte=0;                //设定crc_1byte初值为0  
	for(i = 0; i < 8; i++)   
	{   
		if(((crc_1byte^abyte)&0x01))   
		{   
			crc_1byte^=0x18;     
			crc_1byte>>=1;   
			crc_1byte|=0x80;   
		}         
		else     
			crc_1byte>>=1;   

		abyte>>=1;         
	}   
	return crc_1byte;   
}

/* 参考: https://www.cnblogs.com/yuanguanghui/p/12737740.html */   
static unsigned char calcrc_bytes(unsigned char *p,unsigned char len)  
{  
	unsigned char crc=0;  
	while(len--) //len为总共要校验的字节数  
	{  
		crc=calcrc_1byte(crc^*p++);  
	}  
	return crc;  //若最终返回的crc为0，则数据传输正确  
}  


static int ds18b20_verify_crc(unsigned char *buf)
{
    unsigned char crc;

	crc = calcrc_bytes(buf, 8);

    if (crc == buf[8])
		return 0;
	else
		return -1;
}

static void ds18b20_calc_val(unsigned char ds18b20_buf[], int result[])
{
	unsigned char tempL=0,tempH=0;
	unsigned int integer;
	unsigned char decimal1,decimal2,decimal;

	tempL = ds18b20_buf[0]; //读温度低8位
	tempH = ds18b20_buf[1]; //读温度高8位

	if (tempH > 0x7f)      							//最高位为1时温度是负
	{
		tempL    = ~tempL;         				    //补码转换，取反加一
		tempH    = ~tempH+1;      
		integer  = tempL/16+tempH*16;      			//整数部分
		decimal1 = (tempL&0x0f)*10/16; 			//小数第一位
		decimal2 = (tempL&0x0f)*100/16%10;			//小数第二位
		decimal  = decimal1*10+decimal2; 			//小数两位
	}
	else
	{
		integer  = tempL/16+tempH*16;      				//整数部分
		decimal1 = (tempL&0x0f)*10/16; 					//小数第一位
		decimal2 = (tempL&0x0f)*100/16%10;				//小数第二位
		decimal  = decimal1*10+decimal2; 				//小数两位
	}
	result[0] = integer;
	result[1] = decimal;
}

/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t ds18b20_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned long flags;
	int err;
	unsigned char kern_buf[9];
	int i;
	int result_buf[2];

	if (size != 8)
		return -EINVAL;

	/* 1. 启动温度转换 */
	/* 1.1 关中断 */
	spin_lock_irqsave(&ds18b20_spinlock, flags);

	/* 1.2 发出reset信号并等待回应 */
	err = ds18b20_reset_and_wait_ack();
	if (err)
	{
		spin_unlock_irqrestore(&ds18b20_spinlock, flags);
		printk("ds18b20_reset_and_wait_ack err\n");
		return err;
	}

	/* 1.3 发出命令: skip rom, 0xcc */
	ds18b20_send_cmd(0xcc);

	/* 1.4 发出命令: 启动温度转换, 0x44 */
	ds18b20_send_cmd(0x44);

	/* 1.5 恢复中断 */
	spin_unlock_irqrestore(&ds18b20_spinlock, flags);

	/* 2. 等待温度转换成功 : 可能长达1s */
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(msecs_to_jiffies(1000));

	/* 3. 读取温度 */
	/* 3.1 关中断 */
	spin_lock_irqsave(&ds18b20_spinlock, flags);

	/* 3.2 发出reset信号并等待回应 */
	err = ds18b20_reset_and_wait_ack();
	if (err)
	{
		spin_unlock_irqrestore(&ds18b20_spinlock, flags);
		printk("ds18b20_reset_and_wait_ack second err\n");
		return err;
	}
	/* 3.3 发出命令: skip rom, 0xcc */
	ds18b20_send_cmd(0xcc);

	/* 3.4 发出命令: read scratchpad, 0xbe */
	ds18b20_send_cmd(0xbe);

	/* 3.5 读9字节数据 */
	for (i = 0; i < 9; i++)
	{
		ds18b20_read_data(&kern_buf[i]);
	}

	/* 3.6 恢复中断 */
	spin_unlock_irqrestore(&ds18b20_spinlock, flags);

	/* 3.7 计算CRC验证数据 */
	err = ds18b20_verify_crc(kern_buf);
	if (err)
	{
		printk("ds18b20_verify_crc err\n");
		return err;
	}

	/* 4. copy_to_user */
	ds18b20_calc_val(kern_buf, result_buf);
	
	err = copy_to_user(buf, result_buf, 8);
	return 8;
}


/* 定义自己的file_operations结构体                                              */
static struct file_operations gpio_key_drv = {
	.owner	 = THIS_MODULE,
	.read    = ds18b20_read,
};


/* 在入口函数 */
static int __init ds18b20_init(void)
{
    int err;
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	spin_lock_init(&ds18b20_spinlock);
	
	for (i = 0; i < count; i++)
	{		
		err = gpio_request(gpios[i].gpio, gpios[i].name);
		gpio_direction_output(gpios[i].gpio, 1);
	}

	/* 注册file_operations 	*/
	major = register_chrdev(0, "100ask_ds18b20", &gpio_key_drv);  /* /dev/gpio_desc */

	gpio_class = class_create(THIS_MODULE, "100ask_ds18b20_class");
	if (IS_ERR(gpio_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "100ask_ds18b20");
		return PTR_ERR(gpio_class);
	}

	device_create(gpio_class, NULL, MKDEV(major, 0), NULL, "myds18b20"); /* /dev/myds18b20 */
	
	return err;
}

/* 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 */
static void __exit ds18b20_exit(void)
{
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	device_destroy(gpio_class, MKDEV(major, 0));
	class_destroy(gpio_class);
	unregister_chrdev(major, "100ask_ds18b20");

	for (i = 0; i < count; i++)
	{
		gpio_free(gpios[i].gpio);
	}
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(ds18b20_init);
module_exit(ds18b20_exit);

MODULE_LICENSE("GPL");


