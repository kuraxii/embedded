# linux_driver 理论篇

## 1. 应用层和驱动的互交方式

### 1.1 app和驱动
- copy_to_user()
- copy_from_user()

### 1.2 驱动和与硬件
- 各个子系统的函数
- 通过ioremap映射寄存器地址后，直接访问寄存器

### 1.3 app使用驱动和的4中方式

驱动程序：提供能力，不提供策略
- 非阻塞(轮询)
- 阻塞(休眠-唤醒)
- poll(定时)
- 异步通知

## 2. 字符设备驱动

### 2.1 简单的字符设备驱动框架

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

### 2.2 驱动设计的思想

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


## 3. 驱动进化之路

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


### 3.1 总线设备驱动模型

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

## 1. 常用函数集合

### 1.1 申请gpio 将gpio注册为中断


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
### 1.2 定时器相关

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

### 1.3 驱动注册与反注册框架 

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


## 2. platform 平台设备总线框架

![alt text](./Linux_driver.assets/image.png)
平台总线模型将原来驱动中的设备信息与驱动逻辑分离，使用两个源文件中进行构建，分别是xxx_device.c （设备硬件的描述）和 xxx_driver.c(驱动逻辑)
即使用：
xxx_device.c  描述设备硬件。比如：在这个文件中我们有描述GPIO相应外设接口的寄存器地址信息，中断号等。
xxx_driver.c  用来控制硬件。比如：在这个文件中我们书写如何操作GPIO，如何申请中断等逻辑。

### 2.1 platform_device 设备资源模块

#### 2.1.1相关结构体
```c
/* 设备相关结构体 */
struct platform_device {
	const char	*name;  // 设别名称
	int		id;  // 设备的id号，用于区分同意类型的多个设备
	bool		id_auto;  // 自动分配id号
	struct device	dev;  // 通用的设备结构体，
	u32		num_resources;  // 设别的资源个数  有几个资源就写几个
	struct resource	*resource; // 描述硬件的资源信息

	const struct platform_device_id	*id_entry;  // 与驱动匹配成功后，**被填充的匹配信息**
	char *driver_override; /* Driver name to force a match */
    // 强制绑定设备到指定的驱动

	/* MFD cell pointer */
	struct mfd_cell *mfd_cell;

	/* arch specific additions */
	struct pdev_archdata	archdata;
};

struct resource {
	resource_size_t start;  /* 硬件其实地址 */
	resource_size_t end;    /* 硬件终止地址 */
	const char *name;       /* 硬件资源名 */
	unsigned long flags;    /* 硬件资源类型 
    常用的有
        IORESOURCE_IO
        IORESOURCE_MEM
        IORESOURCE_IRQ
    */
	unsigned long desc;     /* 硬件描述 */
 	struct resource *parent, *sibling, *child; /* 节点描述相关 允许资源是树状的  比如：设备树*/
};
```

#### 2.1.2注册与注销设备到平台设备总线
```c
int platform_device_register(struct platform_device *p_dev)
/* 函数功能：加载platform_device对象到平台总线上 
返回0 代表成功
*/
void platform_device_unregister(struct platform_device *p_dev)
/* 函数功能：把platform_device从总线上卸载 */
```

#### 2.1.3示例代码
```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

// 定义一个平台设备

struct resource my_device_001_res[] ={
    [0] = {
        .start = 0x55,
        .end = 0x55 + 0x4,
        .name = "rcc-reg",
        .flags = IORESOURCE_MEM,
        .desc = "aaa",
    },
    [1] = {
        .start = 0x66,
        .end = 0x66 + 0x4,
        .name = "myemode-reg",
        .flags = IORESOURCE_MEM,
        .desc = "bbb",
    },
    [2] = {
        .start = 0x77,
        .end = 0x77 + 0x4,
        .name = "myORD-reg",
        .flags = IORESOURCE_MEM,
        .desc = "ccc",
    },
    [3] = {
        .start = 0x88,
        .end = 0x88 + 0x4,
        .name = "rcc-reg",
        .flags = IORESOURCE_IRQ,
        .desc = "ddd",
    },
 
};

void my_dev_release(struct device *dev)
{

}

struct platform_device my_platform_device = {
    .name = "zzj,my_device_001",
    .id = PLATFORM_DEVID_AUTO, // 自动分配或者给-1
    .resource = my_device_001_res,
    .num_resources = sizeof(my_device_001_res) / sizeof(struct resource),
    .dev = {
        .release = my_dev_release,
    }
};

static int __init my_dev_init(void)
{
    int ret = 0;
    ret = platform_device_register(&my_platform_device);
    if(ret)
    {
        return -1;
    }
    return 0;
}

static void __exit my_dev_exit(void)
{
    // 注销
    platform_device_unregister(&my_platform_device);
}

module_init(my_dev_init);
module_exit(my_dev_exit);
MODULE_LICENSE("GPL");

```

### 2.2 platform_driver 驱动模块

#### 2.2.1 相关结构体
```c
struct platform_driver {
	int (*probe)(struct platform_device *);  // 设备与驱动匹配匹配成功时执行的回调函数
	int (*remove)(struct platform_device *); //  设备与驱动匹配分离时执行的回调函数
	void (*shutdown)(struct platform_device *); // 设备关闭与重启时的回调
	int (*suspend)(struct platform_device *, pm_message_t state);  // 设备挂起时的回调函数
	int (*resume)(struct platform_device *);  // 设备从低功耗状态唤醒时的回调函数
	struct device_driver driver;  // 通用的设备驱动结构体
	const struct platform_device_id *id_table; // id_table 匹配表
	bool prevent_deferred_probe;  // 是否演示探针标志
};

struct device_driver {
	const char		*name;  /* 驱动名称 */
	struct bus_type		*bus;  

	const struct of_device_id	*of_match_table; /* 设备树of样式匹配表 与设备节点 .compatible 属性匹配*/
	const struct acpi_device_id	*acpi_match_table; /* acpi电源管理接口匹配 */

	struct driver_private *p;  /* 私有数据，主要用于传参 */
};
```

#### 2.2.2 注册与注销设备驱动到平台驱动总线
```c
int platform_driver_register(struct platform_driver *) /* 注册驱动到platform总线上，并自动匹配platfoem_device或设备树 */
int platform_driver_unregister(struct platform_driver *)
```

#### 2.2.3 代码示例
```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

int my_platform_probe(struct platform_device *device)
{
    printk("my_platform_probe函数执行了\n");
    // 1. 直接使用struct platform_device获取资源
    printk("rcc-reg: %#llx\n", device->resource[0].start);
    // 2. 使用platform_get_resource函数获取资源
    struct resource *res =  platform_get_resource(device, IORESOURCE_MEM, 1);
    printk("gpioemode-reg: %#llx\n", res->start);
    return 0;
}

int my_platform_remove(struct platform_device *device)
{
    printk("my_platform_remove函数执行了\n");

    return 0;
}

/* 
    struct platform_device_id {
        char name[PLATFORM_NAME_SIZE];
        kernel_ulong_t driver_data;
    };
 */
static const struct platform_device_id id_table[] = {
    [0] = {"zzj,my_device_001",1},
    [1] = {"zzj,my_device_002",2},
    [2] = { }, /* 最后一个一定要给定一个空元素，代表结束 */
};

static struct platform_driver my_platform_driver = {
    .probe = my_platform_probe,
    .remove = my_platform_remove,
    .driver = {
        .name = "zzj,my_device_001",
    },
    .id_table = id_table,
};

static int __init my_drv_init(void)
{
    int ret;
    ret = platform_driver_register(&my_platform_driver);
    if(ret < 0)
    {
        return -1;
    }
    return 0;
}

static void __exit my_drv_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
}
module_init(my_drv_init);
module_exit(my_drv_exit);

MODULE_LICENSE("GPL");
```


### 2.3 设备与驱动的匹配方式

#### 2.3.1 设备与驱动有3中匹配方式
- name属性匹配
- 设备名称与驱动的id_table匹配实现多对一的匹配
- 设备树节点中的.compatible属性与of_match_table匹配实现多对一的匹配。

```c
struct bus_type platform_bus_type = {
	.name		= "platform",
	.dev_groups	= platform_dev_groups,
	.match		= platform_match,
	.uevent		= platform_uevent,
	.pm		= &platform_dev_pm_ops,
};
static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct platform_driver *pdrv = to_platform_driver(drv);

	/* 先匹配强制匹配 */
	if (pdev->driver_override)
		return !strcmp(pdev->driver_override, drv->name);

	/* 然后是设备树节点匹配 */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* 电源管理驱动匹配 */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	/* 然后使用platform_device  id_table匹配 */
	if (pdrv->id_table)
		return platform_match_id(pdrv->id_table, pdev) != NULL;

	/* 最后使用platform_device  name匹配 */
	return (strcmp(pdev->name, drv->name) == 0);
}
```

#### 2.3.2 加载驱动时如何进行匹配的(为什么与加载顺序无关)

platform_device_register和platform_driver_register无论调用哪个，都会去调用匹配函数
```c
// 调用流程
platform_device_register
platform_device_add  --> pdev->dev.bus = &platform_bus_type;
    device_add
        bus_add_device // 放入链表
        bus_probe_device // probe 枚举设备，即找到匹配的(dev, drv)
            device_initial_probe
            __device_attach
                bus_for_each_drv(...,__device_attach_driver,...) // 遍历driver进行匹配
                __device_attach_driver
                    driver_match_device(drv, dev) // drv->bus->match(dev, drv) 是否匹配
                    driver_probe_device // 调用 drv 的 probe

platform_driver_register
__platform_driver_register --> drv->driver.bus = &platform_bus_type;  bus中包含了总线匹配等关键操作函数
    driver_register
        bus_add_driver // 放入链表
            driver_attach(drv)
                bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);  // 遍历device进行匹配
                    __driver_attach
                        driver_match_device(drv, dev) // drv->bus->match(dev, drv) 是否匹配 
                        driver_probe_device // 调用 drv 的 probe
```


### 2.4 驱动获取设备的方式
#### 2.4.1 通过probe函数形参方式直接获取
```c
int my_platform_probe(struct platform_device *device)
{
    printk("my_platform_probe函数执行了\n");
    // 直接使用struct platform_device获取资源
    printk("rcc-reg: %#llx\n", device->resource[0].start);
    return 0;
}
```

#### 2.4.2 通过内核中的api platform_get_resource获取
```c
struct resource* platform_get_resource(struct platform_device* pdev，unsigned int type，unsigned int index);
//参数1：即设备对象指针
//参数2：即设备资源的类型：即那个flags标记类型是内存资源，还是中断资源，还是什么别的资源等。
//参数3：即同类资源所处位置的索引值（注意是同类资源的索引）
//功能：返回获取到的资源，失败返回NULL

int my_platform_probe(struct platform_device *device)
{
    printk("my_platform_probe函数执行了\n");
    // 使用platform_get_resource函数获取资源
    struct resource *res =  platform_get_resource(device, IORESOURCE_MEM, 1);
    printk("gpioemode-reg: %#llx\n", res->start);
    return 0;
}
```

## 3. 设备树

### 3.1 设备树简述

#### 3.1.1什么是设备树

设备树是一种数据结构，用于描述硬件设备及其配置。它采用类似与josn或xml的树状结构，通过节点 **节点属性**的方式来描述系统中的个中硬件资源及其链接关系。设备树的文件通常以.dts(设备树源文件)和.dtsi(设备树包含文件)为后缀的文本文件。

##### 设备树的结构

设备树的基本结构包括根节点，子节点和属性。每个节点都可以包含多个子节点和属性，属性是以键值对的形式存在的。
示例

```c
/dts-v1/;
/ {
    model = "Example Board";
    compatible = "example,board";
    cpus {
        #address-cells = <1>;
        #size-cells = <0>;
        cpu@0 {
            device_type = "cpu";
            compatible = "arm,cortex-a9";
            reg = <0>;
        };
        cpu@1 {
            device_type = "cpu";
            compatible = "arm,cortex-a9";
            reg = <1>;
        };
    };
    memory {
        device_type = "memory";
        reg = <0x80000000 0x40000000>;
    };
};
```

##### 设备树文件：

1. 什么是dts文件，什么是dtc,什么是dtb文件？

他们之间的关系
![](Linux_driver.assets/devicetree.png)

- dtsi: 即卖你说共享设备信息的部分
- dts: 即描述独有设备信息的部分
- dtc: 设备树编译器
- dtb: 设备树的二进制文件

2. Linux中的设备树所在的路径：
    arch/arm/boot/dts

##### 如何编译设备树
1. 在linux根目录下使用 make dtbs会编译dts目录下所有的设备树文件，也可以使用dtc命令单独编译
2. 编译设备树命令
   ```shell
        dtc -I dts -O dtb -o mytest.dtb test_device_tree.dts
        
        # 反编译设备树
        dtc -I dtb -O dts -o hello.dts mytest.dtb
   ```

#### 3.1.2 设备树是用来做什么的

现代驱动模型: 设备树 + 总线 + 驱动，设备树是用来**替换**总线中的**设备模块**的

为什么要用设备树替换原来总线中的设备模块呢？原因：资源信息与内核分离
平台总线把设备资源信息与驱动分离，对于内核来讲，设备资源信息也要与内核分离，通过这种耦合，实现内核通用性。

![alt text](Linux_driver.assets/devicetree2.png)

这样当设备资源发生变动时，内核无需进行编译匹配，而是兼容。可以更好的对内核进行升级，语序编译设备


#### 3.1.3 分离后内核驱动要使用设备资源信息怎么办？

uboot在启动内核前会将内核与设备树文件加载到不同的内存地址处。
内核会到设备树所在地址自动解析设备树文件，解析为 struct of_node结构体，最终解析为struct platform_device与驱动进匹配

![alt text](Linux_driver.assets/devicetree3.png)

详细追踪内核启动函数start_kernel的分析：
- [linux设备驱动程序-设备树(1)-dtb转换成device_node - 牧野星辰 - 博客园 (cnblogs.com)](https://www.cnblogs.com/downey-blog/p/10485596.html)
- [linux设备驱动程序-设备树(2)-device_node转换成platform_device - 牧野星辰 - 博客园 (cnblogs.com)](https://www.cnblogs.com/downey-blog/p/10486568.html)

### 3.2 设备树语法

设备树语法及如何使用：设备树参考 - [eLinux.org](https://elinux.org/Device_Tree_Reference#What_Is_Device_Tree)

#### 3.2.1 基本数据格式

设备树是一个简单的右节点和属性组成的树状结构。属性是键值对，而节点可以包含属性和子节点


> 使用 "/{}；;"的方式待变系统总线节点，在总线上也就是大括号以再去描述相应的设备，以节点及子节点的方法来边坡是连接关系

示例
```c
/dts-v1/;
/{//表示系统总线
    node1{//子节点1
         //   
    };
    node2{//子节点2
            
    };
};
同胞节点不可以重名，通过子节点的方式表示连接关系。
```



> 节点有一对属性组成，使用简单的键值对的方法表示，表示方法如下：

```c
string-property = "字符串";//值为单字符串。
stringlist-property = "字符串1"， “字符串2”，..., "字符串n";//用逗号隔开的字符串列表。
int32-property = <0x12 0x34 0x56> ;//用<>表示uint32的整数。
binarydata-property = [12 34 56];//使用[]时使用16进制数，可以不带0x前缀;
misc-list-property = "字符串"，<0x12 0x34 0x56>,[0x12 0x56 0x78];//也可以使用逗号的方式把不同的数据连接在一起。
```
代码示例
```c
//设备树根节点：可以理解为系统总线：
/{
    myled1{
        model = "this is a led1 desc";
        //设备节点中信息的描述，使用键值对的方式：
        dev_type = "LED";
        rcc_e = <0x50000a28 0x4>;
        gpioe_modr = <0x50006000 0x4>;
        gpioe_odr = <0x50006014 0x4>;
        binary-data = [12 34 56];
    };

    myled2{
        ...
    };
};
```



> 总结 

设备树原则上可以更加自由的通过键值对的方式来描述设备信息，也更加灵活。只要附合以上的基础结构就不会有错，但是要更标准的话，还要学习接下来的规则与基本写法。
Linux官方设备树手册的示例模型机的链接关系图
![alt text](./Linux_driver.assets/devicetree4.png)

#### 3.2.2 基本规则写法（标准写法）

##### 设备节点中compatible属性与status属性的写法与重要性

- compatible属性是一个字符串，它指定了确切的设备型号，格式为<制造商>,<型号>，主要用来与驱动匹配。compatible如果有多个字符串列，后续的字符串表示该设备与哪些其他设备驱动兼容。
- status属性有两种："okay"启用设备，"disable"禁用设备，是一种状态的描述。status属性若在节点没有存在默认为okay状态。

当一个节点中有compatible属性及status = "okay"时的设备树节点，后由内核启动阶段转换为struct device_node后，最终转换为platform_device设备，与驱动进行匹配。
```c
//版本号：
/dts-v1/;
//设备树根节点：可以理解为系统总线：
/{
    myled1{
        model = "this is a led1 desc";
        //用来与驱动进行匹配的
        compatible = "WX,my_device_01";
        //设备节点中信息的描述，使用键值对的方式：
        dev_type = "LED";
        rcc_e = <0x50000a28 0x4>;
        gpioe_modr = <0x50006000 0x4>;
        gpioe_odr = <0x50006014 0x4>;
        binary-data = [12 34 56];
        //status设置okay，表示启用此结点。
        status = "okay";
    };
    myled2{

    };
};
```

##### 节点命名方式: 节点名 <name>, 节点全名 (节点名@地址) 

1. 在设备树中，每个节点都必须有一个名字， 形式为<name>[@<unit-address>]
    <name> 是一个简单的 ASCII 字符串，长度最多可达 31 个字符。通常，节点的命名是根据它所代表的设备类型来确定的。例如，一个 3com 以太网适配器的节点应该使用 ethernet 这个名字，而不是 3com509。比如：
    
    ```c
    //版本号：
    /dts-v1/;
    //设备树根节点：可以理解为系统总线：
    /{
        myled@50006000{
            model = "this is a led1 desc";
            //用来与驱动进行匹配的
            compatible = "WX,my_device_01";
            //设备节点中信息的描述，使用键值对的方式：
            dev_type = "LED";
            reg = <0x50006000 0x4 0x50006014 0x4 0x50000a28 0x4>;
            binary-data = [12 34 56];
            //status设置okay，表示启用此结点。
            status = "okay";
        };
    
        myled{
            ...
        };
    };
    ```
    如果节点描述的是一个具有地址的设备，则包含 <unit-address>。通常，单元地址是用于访问设备的主要地址，并会在节点的 reg 属性中列出。我们将在本文档的后面部分介绍 reg 属性。
    
    同级节点（sibling nodes）必须具有唯一的名称，但只要地址不同，多个节点使用相同的通用名称是正常的（例如，serial@101f1000 和 serial@101f2000）。
    
2. 节点的引用标签lable:
    ```c
    //版本号：
    /dts-v1/;
    //设备树根节点：可以理解为系统总线：
    /{
        //led1是myled的别名标签(引用节点)
        led1:myled@50006000{
            model = "this is a led1 desc";
            //用来与驱动进行匹配的
            compatible = "WX,my_device_01";
            //设备节点中信息的描述，使用键值对的方式：
            dev_type = "LED";
            reg = <0x50006000 0x4 0x50006014 0x4 0x50000a28 0x4>;
            binary-data = [12 34 56];
            //status设置okay，表示启用此结点。
            status = "okay";
        };
    
        myled{
            ...
        };
    };
    ```
    



##### 地址资源属性的描述方式：reg 属性

内存映射设备使用以下属性将地址信息编码到设备树中：
- reg
- #address-cells
- #size-cells

每个内存映射设备都有一个 reg 属性，它是一个元组列表，形式为 reg = <address1 length1 [address2 length2] [address3 length3] ... >。每个元组表示设备使用的地址范围。每个地址值都是一个或多个 32 位整数的列表，称为单元（cells）。类似地，长度值也可以是一个单元列表，或者为空。

由于地址和长度字段的大小都是可变的，父节点中的 #address-cells 和 #size-cells 属性用于声明每个字段中有多少个单元。换句话说，正确解释 reg 属性需要参考父节点的 #address-cells 和 #size-cells 值。



> 1. 最简单的cpu寻址设备

```c
cpus {
    #address-cells = <1>;
    #size-cells = <0>;
    cpu@0 {
        compatible = "arm,cortex-a9";
        reg = <0>;
    };
    cpu@1 {
        compatible = "arm,cortex-a9";
        reg = <1>;
    };
};
```
在cpus节点中，#address-cells被设置为1，而#size-cells被设置为0。这意味着子节点的reg值是一个单独的uint32，它表示地址，没有大小字段。在这种情况下，两个CPU被分配了地址0和1。对于CPU节点，#size-cells为0，因为每个CPU只被分配了一个地址。

您还会注意到reg值匹配节点名称中的值。按照惯例，如果节点有一个reg属性，那么节点名称必须包含单元地址（unit-address），即reg属性中的第一个地址值。



> 2. 内存映射设备

内存映射设备与CPU节点不同，它们被分配了一个地址范围，设备将响应这个范围内的地址。在子节点的reg元组中，#size-cells用于声明每个长度字段的大小。

以下是一个示例，其中每个地址值和每个长度值都是一个单元（32位），这在32位系统中很常见。

64位机器可能会将#address-cells和#size-cells的值设置为2，以便在设备树中获得64位寻址以高32位与低32位表示。

```c
/dts-v1/;  
  
/ {  
    #address-cells = <1>;  
    #size-cells = <1>;  
  
    ...  
  
    /* 串口设备，分配了一个地址范围 */  
    serial@101f0000 {  
        compatible = "arm,pl011";  
        reg = <0x101f0000 0x1000 >; /* 起始地址0x101f0000，长度0x1000 */  
    };  
  
    serial@101f2000 {  
        compatible = "arm,pl011";  
        reg = <0x101f2000 0x1000 >; /* 起始地址0x101f2000，长度0x1000 */  
    };  
  
    /* GPIO设备，分配了两个地址范围 */  
    gpio@101f3000 {  
        compatible = "arm,pl061";  
        reg = <0x101f3000 0x1000  /* 起始地址0x101f3000，长度0x1000 */  
               0x101f4000 0x0010>; /* 起始地址0x101f4000，长度0x0010 */  
    };  
  
    /* 中断控制器 */  
    interrupt-controller@10140000 {  
        compatible = "arm,pl190";  
        reg = <0x10140000 0x1000 >; /* 起始地址0x10140000，长度0x1000 */  
    };  
  
    /* SPI设备 */  
    spi@10115000 {  
        compatible = "arm,pl022";  
        reg = <0x10115000 0x1000 >; /* 起始地址0x10115000，长度0x1000 */  
    };  
    ...  
};
```



> 3. 无内存映射设备

不如iic设备

在处理器总线上，有些设备并不是内存映射的。它们可能有地址范围，但这些地址范围不是直接由CPU访问的。相反，这些设备的访问是由其父设备的驱动程序代表CPU间接进行的。

以IIC（Inter-Integrated Circuit）设备为例，每个IIC设备都被分配了一个地址，但这个地址并没有与长度或范围相关联。这与CPU地址分配看起来非常相似，但实际上是有区别的。

在提供的示例中，I²C总线被分配了一个地址，但没有指定长度，因为IIC设备是通过IIC总线的通信协议来访问的，而不是直接通过内存映射。

```c
i2c@1,0 {  
    compatible = "acme,a1234-i2c-bus";  
    #address-cells = <1>; /* 用于描述子设备地址的单元数，这里是1 */  
    #size-cells = <0>;    /* 用于描述子设备大小或长度的单元数，这里是0，因为没有大小或长度 */  
    reg = <1 0 0x1000>;   //此为I2c1-0的地址映射空间  与rang属性相关
  
    rtc@58 {  
        compatible = "maxim,ds1338";  
        reg = <0x58>;        /* RTC设备的I²C地址是58 */  
    };  
};
```



### 3.3 gpio子系统

gpio子系统操作api从`gpio_*`发展到`gpiod_*`，到现在为止，`gpio_*`样式的api已经被弃用

相关介绍文章：

- [stack overflow](https://stackoverflow.com/questions/39103185/gpiod-vs-gpio-methods-in-the-linux-kernel)

- [kernel doc](https://www.kernel.org/doc/html/v5.2/driver-api/gpio/index.html)

#### 3.3.1 gpiod_* api 

> 相关数据结构

```c
// include/linux/gpio/consumer.h
struct gpio_desc; // 无法看到具体实现，只能使用指针进行传参

// 定义gpio描述数组结构，用于批量处理gpio
struct gpio_descs {
        struct gpio_array *info;
        unsigned int ndescs;
        struct gpio_desc *desc[];
}

/* gpio 属性标志*/

/* 设备树配置属性 */
#define GPIO_ACTIVE_HIGH 0  高电平活动(亮)
#define GPIO_ACTIVE_LOW 1   地点平活动(灭)

/* 使用 gpiod_get 时，gpio初始化配置*/
enum gpiod_flags {
	GPIOD_ASIS	= 0,
	GPIOD_IN	= GPIOD_FLAGS_BIT_DIR_SET,   // 输入模式
	GPIOD_OUT_LOW	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT,   //输出模式 高电平
	GPIOD_OUT_HIGH	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT |  //输出模式 低电平
			  GPIOD_FLAGS_BIT_DIR_VAL,
};
```

> gpio资源的申请与释放

```c
// 申请
struct gpio_desc *gpiod_get(struct device *dev, const char *con_id,
                            enum gpiod_flags flags)
    
struct gpio_desc *gpiod_get_index(struct device *dev,
                          const char *con_id, unsigned int idx,
                          enum gpiod_flags flags)   
struct gpio_desc *gpiod_get_optional(struct device *dev,
                             const char *con_id,
                             enum gpiod_flags flags)

struct gpio_desc *gpiod_get_index_optional(struct device *dev,
                                   const char *con_id,
                                   unsigned int index,
                                   enum gpiod_flags flags)   
struct gpio_descs *gpiod_get_array(struct device *dev,
                       const char *con_id,
                       enum gpiod_flags flags)

struct gpio_descs *gpiod_get_array_optional(struct device *dev,
                                const char *con_id,
                                enum gpiod_flags flags)
// 释放
void gpiod_put(struct gpio_desc *desc)
 void gpiod_put_array(struct gpio_descs *descs)
```

> 设置 gpio

```c
int gpiod_direction_input(struct gpio_desc *desc)
int gpiod_direction_output(struct gpio_desc *desc, int value)

int gpiod_get_direction(const struct gpio_desc *desc)
```

> 自旋锁安全 访问 gpio(可以在中断函数中使用)

```c
int gpiod_get_value(const struct gpio_desc *desc);
void gpiod_set_value(struct gpio_desc *desc, int value);
```

> 允许sleep的 访问gpio

```c
int gpiod_cansleep(const struct gpio_desc *desc)
    
int gpiod_get_value_cansleep(const struct gpio_desc *desc)
void gpiod_set_value_cansleep(struct gpio_desc *desc, int value)
```

#### 3.3.2 低电平有效和开漏语义

| 函数（示例）                  | 线路属性              | 物理线路       |
| ----------------------------- | --------------------- | -------------- |
| gpiod_set_raw_value(desc, 0); | 不关心                | low            |
| gpiod_set_raw_value(desc, 1); | 不关心                | high           |
| gpiod_set_value(desc, 0);     | 默认（高电平有效）    | low            |
| gpiod_set_value(desc, 1);     | 默认（高电平有效）    | high           |
| gpiod_set_value(desc, 0);     | 低电平有效            | high           |
| gpiod_set_value(desc, 1);     | 低电平有效            | low            |
| gpiod_set_value(desc, 0);     | default (active high) | low            |
| gpiod_set_value(desc, 1);     | default (active high) | high           |
| gpiod_set_value(desc, 0);     | 漏极开路              | low            |
| gpiod_set_value(desc, 1);     | 漏极开路              | high impedance |
| gpiod_set_value(desc, 0);     | 源极开路              | high impedance |
| gpiod_set_value(desc, 1);     | 源极开路              | high           |

可以使用 set_raw/get_raw 函数覆盖这些语义，但应尽可能避免，特别是对于系统无关的驱动程序，它们不需要关心实际的物理线路级别，而是担心逻辑值。
