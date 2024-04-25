# linux_driver 理论篇

## 应用层和驱动的互交方式

### app和驱动
- copy_to_user()
- copy_from_user()

### 驱动和与硬件
- 各个子系统的函数
- 通过ioremap映射寄存器地址后，直接访问寄存器

### app使用驱动和的4中方式

驱动程序：提供能力，不提供策略
- 非阻塞(轮询)
- 阻塞(休眠-唤醒)
- poll(定时)
- 异步通知

## 字符设备驱动

### 简单的字符设备驱动框架

1. 确定主设备号，也可以让内核分配  int major;  major = 0 时自动分配
2. 定义自己的 file_operations 结构体
3. 实现对应的 drv_open/drv_read/drv_write 等函数，填入 file_operations 结构体
4. 把 file_operations 结构体告诉内核： register_chrdev
5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数
6. 有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用 unregister_chrdev
7. 其他完善：提供设备信息，自动创建设备节点： class_create, device_crea


register_chrdev方式，register_chrdev会使用整个主设备号,使用简单，但占用资源较大
```c
#include <asm/uaccess.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/kdev_t.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stddef.h>
#include <linux/usb/cdc.h>
#include <linux/module.h>
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

#define MIN(a, b) (a < b ? a : b)


// 1. 确定主设备号 0自动分配
static int major = 0;
static char kernel_buf[1024];
MODULE_LICENSE("GPL");

struct class* hello_class;


// 3. 填充file_operations
int hello_open (struct inode *node, struct file *file)
{
    printk("%s %s line %d", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}
ssize_t hello_read (struct file *file, char __user *buf, size_t size, loff_t *l)
{
    int ret;
    printk("%s %s line %d", __FILE__, __FUNCTION__, __LINE__);
    ret = copy_to_user(buf, kernel_buf, MIN(1024, size));
    return ret;
}
ssize_t hello_write (struct file *file, const char __user *buf, size_t size, loff_t *l)
{
    int ret;
    printk("%s %s line %d", __FILE__, __FUNCTION__, __LINE__);
    ret = copy_from_user(kernel_buf, buf, MIN(1024, size));
    return ret;
}
int hello_close (struct inode *node, struct file *file)
{
    printk("%s %s line %d", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

// 2. 定义自己的 file_operations 结构体

struct file_operations operations= {
    .read = hello_read,
    .open = hello_open,
    .write = hello_write,
    .release = hello_close,
};

static int __init hello_drv_init(void)
{
    int err;

    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

// 4. 申请设备号，并将模块添加到内核中
    major = register_chrdev(major, "hello", &operations);


    // 创建设备文件
    hello_class = class_create(THIS_MODULE, "hello_class");
    err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "hello");
		return -1;
	}

    device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello");

    return 0; 
}

// 6. 有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用 unregister_chrdev
static void __exit hello_drv_exit(void)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    device_destroy(hello_class, MKDEV(major, 0));
    class_destroy(hello_class);
    unregister_chrdev(0, "hello");
}
// 7. 其他完善：提供设备信息，自动创建设备节点：class_create,device_create

module_init(hello_drv_init);
module_exit(hello_drv_exit);
```

自动创建设备节点
```c
drv_init()
{
    class_create();
    device_create();
}

drv_exit()
{
    device_destory();
    class_destory();
}

```


```makefile
# 1. 使用不同的开发板内核时, 一定要修改KERN_DIR
# 2. KERN_DIR中的内核要事先配置、编译, 为了能编译内核, 要先设置下列环境变量:
# 2.1 ARCH,          比如: export ARCH=arm64
# 2.2 CROSS_COMPILE, 比如: export CROSS_COMPILE=aarch64-linux-gnu-
# 2.3 PATH,          比如: export PATH=$PATH:/home/book/100ask_roc-rk3399-pc/ToolChain-6.3.1/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin 
# 注意: 不同的开发板不同的编译器上述3个环境变量不一定相同,
#       请参考各开发板的高级用户使用手册

KERN_DIR = /home/book/100ask_imx6ull-sdk/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	$(CROSS_COMPILE)gcc -o hello_test hello.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f hello_drv

obj-m	+= hello_drv.o

```

### 驱动设计的思想

- 面向对象

针对led驱动，字符设备驱动抽象出file_operations结构体，我们写的程序也可以抽象出led_operations结构体
```c
struct led_operations{
    int (*init) (int which);  // 根据次设备号初始化led
    int (*ctl) (int which, char status);
    int (*read) (int which, char* status);
    int (*release) (int which);
};
// 提供了一些方法，提供给上层使用。类似于java中类的方法
struct led_operations* get_board_led_opr(void);
int get_board_led_num(void);

```

- 分层

    将与硬件相关与硬件无关的代码分离
    ![led的分层](Linux_driver.assets/1698063451897.png)

- 分离

    将不同的引脚操作进行分离，比如GPIO1与GPIO2的硬件设置不同就导致了相关的代码不同
    使用文件进行分离
    ![驱动的分离](Linux_driver.assets/1698064550218.png)


## 驱动进化之路

驱动编写的三种写法
1. 传统写法

    ![传统写法](./Linux_driver.assets/1698588931036.png)
- 使用哪个引脚，怎么操作引脚，都写死在代码里
- 最简单，不考虑扩展性，可以快速实现功能
- 修改代码时，余姚重新编译

2. 总线设备驱动模型

    ![总线设备驱动模型](./Linux_driver.assets/1698589166846.png)
- 引入platform_device/platform_driver,将‘资源’和‘驱动’分离开
- 代码稍微复杂，但是易于扩展
- 更换引脚时，图9.3中的led_drv.c基本不用改，但是需要修改led_dev.c

3. 设备树

    ![设备树](./Linux_driver.assets/1698589377314.png)
- 通过配置文件──设备树来定义“资源”。
- 代码稍微复杂，但是易于扩展。
- 无冗余代码，修改引脚时只需要修改 dts 文件并编译得到 dtb 文件，把它传给内核。
- 无需重新编译内核/驱动。


### 总线设备驱动模型

#### deice与driver的匹配规则
- 最先比较
    - platform_device.driver_override 和 platform_driver.driver.name
    - 可以设置 platform_device 的 driver_override，强制选择某个 platform_driver。

- 然后比较
    - platform_device. name 和 platform_driver.id_table[i].name
    - Platform_driver.id_table 是“platform_device_id”指针，表示该 drv 支持若干个 device，它里面列出了各个 device 的{.name, .driver_data}，其中的“ name”表示该
drv 支持的设备的名字， driver_data 是些提供给该 device 的私有数据。

- 最后比较
    - platform_device.name 和 platform_driver.driver.name
    - platform_driver.id_table 可能为空，
这时可以根据 platform_driver.driver.name 来寻找同名的 platform_device。

#### 比较过城中的函数调用
```c
platform_device_register
platform_device_add()
    device_add()
        bus_add_device() // 放入链表
                bus_probe_device() // probe 枚举设备，即找到匹配的(dev, drv)
                    device_initial_probe()
                            __device_attach()
                                    bus_for_each_drv(...,__device_attach_driver,...)
                                        __device_attach_driver()
                                            driver_match_device(drv, dev) // 是否匹配
                                                driver_probe_device() // 调用 drv 的 probe
platform_driver_register()
__platform_driver_register()
    driver_register()
        bus_add_driver() // 放入链表
            driver_attach(drv)
                bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);
                    __driver_attach()
                        driver_match_device(drv, dev) // 是否匹配
                            driver_probe_device() // 调用 drv 的 probe
```


#### 常用函数

##### 注册/反注册
```c
platform_device_register()/ platform_device_unregister()
platform_driver_register()/ platform_driver_unregister()
platform_add_devices() // 注册多个 device
```
##### 获得资源
返回该 dev 中某类型(type)资源中的第几个(num)：
```c
struct resource *platform_get_resource(struct platform_device *dev, unsigned int type, unsigned int num)
```
返回该 dev 所用的第几个(num)中断：
```c
int platform_get_irq(struct platform_device *dev, unsigned int num)
```
通过名字(name)返回该 dev 的某类型(type)资源：
```c
struct resource *platform_get_resource_byname(struct platform_device *dev,
unsigned int type,
const char *name)
```
通过名字(name)返回该 dev 的中断号：
```c
int platform_get_irq_byname(struct platform_device *dev， const char *name)
```

#### 怎么写程序
- 分配/设置/注册 platform_device 结构体
    - 在里面定义所用资源，指定设备名字。
- 分配/设置/注册 platform_driver 结构体
    - 在其中的 probe 函数里，分配/设置/注册 file_operations 结构体，
    - 并从 platform_device 中确实所用硬件资源。
    - 指定 platform_driver 的名字。



# linux_driver 实战篇

## 常用函数集合

### 申请gpio 将gpio注册为中断


[查看实战示例](src/04_sr04_improve/gpio_drv.c#L221)

```c
    // 申请gpio 并设置gpio功能
    err = gpio_request(gpios[0].gpio, gpios[0].name);
    gpio_direction_output(gpios[0].gpio, 0);  // 输出模式
    gpio_direction_input(gpios[0].gpio);      // 输入模式
    // 释放gpio
    gpio_free(gpios[0].gpio);
```
[查看实战示例](src/02_sr501/gpio_drv.c#L159)
```c
    // 将gpio注册为中断
    gpios[1].irq  = gpio_to_irq(gpios[1].gpio);
	err = request_irq(gpios[1].irq, sr04_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, gpios[1].name, &gpios[1]);
    // 释放irq
    free_irq(gpios[1].irq, &gpios[1]);
```
### 定时器相关

定时器，定时结束执行函数
[查看实战示例](src/04_sr04_improve/gpio_drv.c#L230)
```c
    // 注册定时器
    setup_timer(&gpios[1].key_timer, sr04_timer_func, (unsigned long)&gpios[1]);
    // 开始计时
    mod_timer(&gpios[1].key_timer, jiffies + msecs_to_jiffies(50));  
    // 停止计时 
    del_timer(&gpios[1].key_timer);

    // 销毁timer
    del_timer(&gpios[1].key_timer);

```

### 驱动注册与反注册框架 

```c
// 注册
static int __init gpio_drv_init(void)
{
    int err;
    int i;

    /* 获取gpio资源，使用平台总线后使用probe函数获取 */
    int count = sizeof(gpios) / sizeof(gpios[0]);
    for (i = 0; i < count; i++)
    {
        err = gpio_request(gpios[i].gpio, gpios[i].name);
        if (err < 0)
        {
            printk("can not request gpio %d %s\n", gpios[i].gpio, gpios[i].name);
            return -ENODEV;
        }
        gpio_direction_output(gpios[i].gpio, 1);
    }

    /* 注册file_operations 	0 表示自动分配主设备号 */  
    /* 生成 /proc/devices/100ask_gpio_led */
    major = register_chrdev(0, "100ask_gpio_led", &gpio_led_drv);

    /* 创建一个设备类，可以用来帮助自动创建设备节点。 */  
    /* 对应目录 /proc/devices/100ask_gpio_led */
    gpio_class = class_create(THIS_MODULE, "100ask_gpio_led_class");
    if (IS_ERR(gpio_class))
    {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major, "100ask_gpio_led");
        return PTR_ERR(gpio_class);
    }

    /* 通过上面的设备类自动创建设备节点 */  
    /* 对应文件 /dev/100ask_gpio */
    device_create(gpio_class, NULL, MKDEV(major, 0), NULL, "100ask_led"); /* /dev/100ask_gpio */

    return err;
}

// 反注册

/* 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 */
static void __exit gpio_drv_exit(void)
{
    int i;
    int count = sizeof(gpios) / sizeof(gpios[0]);

    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    
    /* 卸载设备节点文件 */
    device_destroy(gpio_class, MKDEV(major, 0));
    /* 删除设备类 */
    class_destroy(gpio_class);
    /* 删除设备在系统中的注册 */
    unregister_chrdev(major, "100ask_gpio_led");


    /* 释放之前申请的设备资源 */
    for (i = 0; i < count; i++)
    {
        gpio_free(gpios[i].gpio);
    }
}
```

中断 poll 异步信号以及详解,来自驱动实验 sr501模块
```c
// 定义的gpio描述结构体 包含了gpio号 irq中断 名字 以及 超时(在后续会引用解释)
struct gpio_desc{
	int gpio;
	int irq;
    char *name;
    int key;
	struct timer_list key_timer;
} ;

// 通过linux shell获取的gpio号，直接写死，这也是在没有platform框架下唯一的获取硬件资源的方式
static struct gpio_desc gpios[1] = {
    {115, 0, "sr501", },
};

/* 主设备号                                                                 */
static int major = 0;
static struct class *gpio_class;

/* 环形缓冲区 */
#define BUF_LEN 128
static int g_keys[BUF_LEN];
static int r, w;

struct fasync_struct *button_fasync;

#define NEXT_POS(x) ((x+1) % BUF_LEN)

static int is_key_buf_empty(void)
{
	return (r == w);
}

static int is_key_buf_full(void)
{
	return (r == NEXT_POS(w));
}

static void put_key(int key)
{
	if (!is_key_buf_full())
	{
		g_keys[w] = key;
		w = NEXT_POS(w);
	}
}
static int get_key(void)
{
	int key = 0;
	if (!is_key_buf_empty())
	{
		key = g_keys[r];
		r = NEXT_POS(r);
	}
	return key;
}


// 等待队列声明，将应用层的进程添加到等待队列，等待条件触发。 如 read函数的阻塞
// 在进程休眠时，进程时无法知道外部发生了生么的，只能通过wake_up_interruptible唤醒，让调度器
static DECLARE_WAIT_QUEUE_HEAD(gpio_wait);


// 中断服务程序 是这个驱动事件触发的核心，中断的触发事件已经在open函数中设置了
static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
	int val;
	int key;
	struct gpio_desc *gpio_desc = dev_id;
    // 触发了中断，获取gpio当前的值，然后将值存入环形缓冲区
	val = gpio_get_value(gpio_desc->gpio);
	key = (gpio_desc->key) | (val<<8);
	put_key(key);
     
    // 唤醒进程，
	wake_up_interruptible(&gpio_wait);

    // 发送 向 button_fasync 中所有的进程发送信号， 并且可以通过select poll等方式获取 POLL_IN标识，示例如下
	kill_fasync(&button_fasync, SIGIO, POLL_IN);
    /* 
    在应用层可以这样写
    void sigio_handler(int signo) {
    struct pollfd fds[1];
    fds[0].fd = sockfd;  // sockfd是监听套接字或已连接套接字
    fds[0].events = POLLIN | POLLOUT;

    if (poll(fds, 1, 0) > 0) {
        if (fds[0].revents & POLLIN) {
            // 处理可读事件
        }
        if (fds[0].revents & POLLOUT) {
            // 处理可写事件
        }
    }
}

    */
	return IRQ_HANDLED;
}



/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t gpio_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	//printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	int err;
	int key;

	if (is_key_buf_empty() && (file->f_flags & O_NONBLOCK))
		return -EAGAIN;
	
    /* 
    wait_event_interruptible 宏会检查给定的条件（第二个参数），如果条件为真（非零），宏会立即返回并继续执行后续代码。如果条件为假（零），进程会进入 TASK_INTERRUPTIBLE 状态，即可被信号打断的休眠状态，直到某个其他地方的代码显式地唤醒它（例如通过 wake_up_interruptible）或者进程接收到一个信号。
     */
	wait_event_interruptible(gpio_wait, !is_key_buf_empty());
	key = get_key();
	err = copy_to_user(buf, &key, 4);
	
	return 4;
}

static unsigned int gpio_drv_poll(struct file *fp, poll_table * wait)
{
	//printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 为poll添加了一种机制，使得  poll机制可以通过 gpio_wait等待队列的唤醒而唤醒*/
	poll_wait(fp, &gpio_wait, wait);
	return is_key_buf_empty() ? 0 : POLLIN | POLLRDNORM;
}

static int gpio_drv_fasync(int fd, struct file *file, int on)
{
    // 用于将当前进程注册到 struct fasync_struct 中。
    // 在应用层 使用 fcntl 实现注册信号
    /*
        应用层开启异步io的示例如下 
        int flags = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, flags | O_ASYNC); 
    */
	if (fasync_helper(fd, file, on, &button_fasync) >= 0)
		return 0;
	else
		return -EIO;
}


/* 定义自己的file_operations结构体                                              */
static struct file_operations gpio_key_drv = {
	.owner	 = THIS_MODULE,
	.read    = gpio_drv_read,
	.poll    = gpio_drv_poll,
	.fasync  = gpio_drv_fasync,
};



/* 在入口函数 */
static int __init gpio_drv_init(void)
{
    int err;
    int i;
    int count = sizeof(gpios)/sizeof(gpios[0]);
	for (i = 0; i < count; i++)
	{		
		gpios[i].irq  = gpio_to_irq(gpios[i].gpio);

		/* 注册中断 
        参数
            中断号是gpios[i].irq 
            中断处理函数 gpio_key_isr
            中断属性以及触发方式 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING
            中断name 一般用于debug
            中断独立参数 一般用于共享中断时的中断区别参数  在中断处理函数 void *dev_id可以得到它
            */
		err = request_irq(gpios[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, gpios[i].name, &gpios[i]);
        /* 
        下面解释一下dev_id参数为什么必须的，而且是必须唯一的。
        当调用free_irq注销中断处理函数时（通常卸载驱动时其中断处理函数也会被注销掉），因为dev_id是唯一的，所以可以通过它来判断从共享中断线上的多个中断处理程序中删除指定的一个。如果没有这个参数，那么kernel不可能知道给定的中断线上到底要删除哪一个处理程序。 
        */
    }
	return err;
}

/* 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 */
static void __exit gpio_drv_exit(void)
{
    int count = sizeof(gpios)/sizeof(gpios[0]);
	for (i = 0; i < count; i++)
	{
		free_irq(gpios[i].irq, &gpios[i]);	
	}
}
module_init(gpio_drv_init);
module_exit(gpio_drv_exit);
MODULE_LICENSE("GPL");
```




外设时序 读取逻辑解析  来自驱动实现 sr04
```c
#define CMD_TRIG 100

struct gpio_desc
{
    int gpio;
    int irq;
    char *name;
    int key;
    struct timer_list key_timer;
};

static struct gpio_desc gpios[2] = {
    {
        115,
        0,
        "trig",
    },
    {
        116,
        0,
        "echo",
    },

};

/* 主设备号      */
static int major = 0;
static struct class *gpio_class;

/* 环形缓冲区 */
#define BUF_LEN 128
static int g_vals[BUF_LEN];
static int r, w;

struct fasync_struct *button_fasync;

#define NEXT_POS(x) ((x + 1) % BUF_LEN)

static int is_val_buf_empty(void)
{
    return (r == w);
}

static int is_cal_buf_full(void)
{
    return (r == NEXT_POS(w));
}

static void put_val(int key)
{
    if (!is_cal_buf_full())
    {
        g_vals[w] = key;
        w = NEXT_POS(w);
    }
}

static int get_val(void)
{
    int val = 0;
    if (!is_val_buf_empty())
    {
        val = g_vals[r];
        r = NEXT_POS(r);
    }
    return val;
}

static DECLARE_WAIT_QUEUE_HEAD(gpio_wait);

static ssize_t sr04_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    int err;
    int val;

    if (is_val_buf_empty() && (file->f_flags & O_NONBLOCK))
        return -EAGAIN;

    wait_event_interruptible(gpio_wait, !is_val_buf_empty());
    val = get_val();
    err = copy_to_user(buf, &val, 4);

    return 4;
}

static unsigned int sr04_drv_poll(struct file *fp, poll_table *wait)
{
    poll_wait(fp, &gpio_wait, wait);
    return is_val_buf_empty() ? 0 : POLLIN | POLLRDNORM;
}

static int sr04_drv_fasync(int fd, struct file *file, int on)
{
    if (fasync_helper(fd, file, on, &button_fasync) >= 0)
        return 0;
    else
        return -EIO;
}

// ioctl (fd, cmd, arg)
long sr04_drv_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case CMD_TRIG: {
        gpio_set_value(gpios[0].gpio, 1);
        udelay(20);
        gpio_set_value(gpios[0].gpio, 0);
    }
    }
    return 0;
}

                                     */
static struct file_operations gpio_sr04_drv = {
    .owner = THIS_MODULE,
    .read = sr04_drv_read,
    .poll = sr04_drv_poll,
    .fasync = sr04_drv_fasync,
    .unlocked_ioctl = sr04_drv_ioctl,
};

static irqreturn_t gpio_sr04_isr(int irq, void *dev_id)
{
    struct gpio_desc *gpio_desc = dev_id;
    /* data ==> gpio */
    int val;

    static u64 rising_time = 0;
    u64 time;

    val = gpio_get_value(gpio_desc->gpio);
    if (val)
    {
        rising_time = ktime_get_ns();
    }
    else
    {
        if (rising_time == 0)
        {
            printk("missing raising interrupt");
            return IRQ_HANDLED;
        }
        // 下降沿记录结束时间，并计算时间差 计算距离
        time = ktime_get_ns() - rising_time;
        rising_time = 0;
        put_val(time);
        wake_up_interruptible(&gpio_wait);           // 中断
        kill_fasync(&button_fasync, SIGIO, POLL_IN); //  异步
    }
    return IRQ_HANDLED;
}


static int __init gpio_drv_init(void)
{
    int err;

    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    // trig pin
    err = gpio_request(gpios[0].gpio, gpios[0].name);
    if (err)
    {
        printk("%s: can not open GPIO %d\n", __func__, gpios[0].gpio);
        return 0;
    }
    err = gpio_direction_output(gpios[0].gpio, 0);
    if (err)
    {
        pr_err("Failed to direction input the GPIO %d\n", gpios[1].gpio);
        return 0;
    }

    // echo pin
    err = gpio_request(gpios[1].gpio, gpios[1].name);
    if (err)
    {
        printk("%s: can not open GPIO %d\n", __func__, gpios[0].gpio);
        return 0;
    }
    // 将echo引脚注册到中断号
    gpios[1].irq = gpio_to_irq(gpios[1].gpio);
    err =
        request_irq(gpios[1].irq, gpio_sr04_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, gpios[1].name, &gpios[1]);

    /* 注册file_operations 	*/
    major = register_chrdev(0, "100ask_gpio_sr04", &gpio_sr04_drv); /* /dev/gpio_desc */

    gpio_class = class_create(THIS_MODULE, "100ask_gpio_key_class");
    if (IS_ERR(gpio_class))
    {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major, "100ask_gpio_sr04");
        return PTR_ERR(gpio_class);
    }

    device_create(gpio_class, NULL, MKDEV(major, 0), NULL, "sr04"); /* /dev/100ask_gpio */

    return err;
}

static void __exit gpio_drv_exit(void)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    device_destroy(gpio_class, MKDEV(major, 0));
    class_destroy(gpio_class);
    unregister_chrdev(major, "100ask_gpio_sr04");

    // 释放gpio口与与其绑定的中断
    gpio_free(gpios[0].gpio);
    gpio_free(gpios[1].gpio);
    free_irq(gpios[1].irq, &gpios[1]);
}

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);
```
















