# i2c

## 1. i2c总线通信协议

### 1.1 起始信号与停止信号

起始信号
- scl时钟线为高电平
- sda数据线为下降沿

停止信号
- scl时钟线为高电平
- sda数据线为上升沿

### 1.2 写数据帧

1. 发送起始位
2. 发送设备地址（7位）
    1. 首先发送7位设备地址
    2. 然后是写数据的标志（值为0）
    3. 从机发送应答信号 0
3. 发送寄存器地址（8位）
    1. 发送8位的寄存器地址
    2. 从机返回应答信号
4. 写入数据
    1. 发送8位数据
    2. 从机再次进行应答
5. 发送停止位

### 1.3 读数据帧

1. 发送起始位
2. 发送设备地址（7位）
    1. 首先发送7位设备地址
    2. 然后发送写数据的标志
    3. 从机发送低电平的应答信号
3. 发送要写的寄存器地址
    1. 发送8位的寄存器地址
    2. 从机返回应答信号
4. 主机发送起始位
5. 发送设备地址（7位）
    1. 首先发送7位设备地址
    2. 设备地址后面是读数据标志
6. 接收数据
    1. 接受8位的数据
    2. 主机发送应答信号 1
注: 在主句接收完数据后，可以选择直接发送停止位而不先发送应答信号



## i2c子系统api


### 数据结构



### 添加i2c驱动








## 示例

设备树
```tex
&i2c1 {
	clock-frequency = <100000>;
	pinctrl-names = "default";
	pinctrl-0 = <&pinctrl_i2c1>;
	status = "okay";
	at24c02 {
		compatible = "zzj,i2cdev";
		reg = <0x50>;

	};

};
```




```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/of_gpio.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
// #define MYGPIO

static struct eeprom {
    struct class *class;
    struct device *dev;
    struct cdev *cdev;
    struct i2c_client *client;
} EEPROM;

int led_open(struct inode *i, struct file *f)
{
    return 0;
}

int led_close(struct inode *i, struct file *f)
{
    return 0;
}

ssize_t led_read(struct file *f, char __user *buf, size_t size, loff_t *offset)
{
    int ret;
    unsigned char *k_buf;
    struct i2c_msg msgs[2];

    /* 从 0地址开始 读取 size 字节 */
    k_buf = kzalloc(size, GFP_KERNEL);
    /* 初始化i2c_msg
    1. 发起一次写操作：把0发给AT24c02，表示要从0地址读取数据
    2. 发起一次写操作：得到数据
    */

    msgs[0].addr = EEPROM.client->addr;
    msgs[0].flags = 0; /* 0 写操作 */
    msgs[0].buf = k_buf;
    k_buf[0] = 0; /* 把数据0 发给设备 */
    msgs[0].len = 1;

    msgs[1].addr = EEPROM.client->addr;
    msgs[1].flags = I2C_M_RD; /* 1 读操作 */
    msgs[1].buf = k_buf;
    msgs[1].len = size;

    ret = i2c_transfer(EEPROM.client->adapter, msgs, 2);

    printk("read buf: %s\n", k_buf);
    ret = copy_to_user(buf, k_buf, size);
    kfree(k_buf);

    return size;
}

ssize_t led_write(struct file *f, const char __user *buf, size_t size, loff_t *offset)
{

    int ret;
    unsigned char *k_buf;
    struct i2c_msg msgs[1];

    /* 从 0 读取 size 字节 */
    k_buf = kzalloc(size + 2, GFP_KERNEL);
    if (!k_buf) {
        printk(KERN_ERR "Failed to allocate memory\n");
        return -ENOMEM;
    }

    ret = copy_from_user(k_buf + 1, buf, size);
    if (ret) {
        printk(KERN_ERR "Failed to copy data from user, ret = %d\n", ret);
        kfree(k_buf);
        return -EFAULT;
    }

    printk(KERN_INFO "write buf: %s\n", k_buf + 1);

    /* 初始化i2c_msg
    1. 发起一次写操作：把0发给AT24c02，表示要从0地址读取数据
    2. 发起一次写操作：得到数据
    */

    msgs[0].addr = EEPROM.client->addr;
    msgs[0].flags = 0;   /* 0 写操作 */
    msgs[0].buf = k_buf; /* 写设备 发给设备 */
    msgs[0].len = size + 1;
    k_buf[0] = 0; // 寄存器地址
    ret = i2c_transfer(EEPROM.client->adapter, msgs, 1);

    kfree(k_buf);

    return size;
}

static struct file_operations led_operations = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_close,
};

int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

    int ret;

    EEPROM.cdev = cdev_alloc();
    if (!EEPROM.cdev) {
        ret = -ENOMEM;
        goto return_;
    }

    cdev_init(EEPROM.cdev, &led_operations);

    ret = alloc_chrdev_region(&EEPROM.cdev->dev, 0, 1, "myi2c");
    if (ret < 0) {
        goto err_free_cdev;
    }

    ret = cdev_add(EEPROM.cdev, EEPROM.cdev->dev, 1);
    if (ret < 0) {
        goto err_unregister;
    }

    EEPROM.class = class_create(THIS_MODULE, "i2c_class");
    if (IS_ERR(EEPROM.class)) {
        ret = PTR_ERR(EEPROM.class);
        goto err_del_cdev;
    }

    EEPROM.dev = device_create(EEPROM.class, NULL, EEPROM.cdev->dev, NULL, client->name);
    if (IS_ERR(EEPROM.dev)) {
        ret = PTR_ERR(EEPROM.dev);
        goto err_destroy_class;
    }

    EEPROM.client = client;

    printk(KERN_INFO "Probed I2C device: %s successfully\n", client->name);

err_destroy_class:
    class_destroy(EEPROM.class);
err_del_cdev:
    cdev_del(EEPROM.cdev);
err_unregister:
    unregister_chrdev_region(EEPROM.cdev->dev, 1);
err_free_cdev:
    kfree(EEPROM.cdev);
return_:
    return 0;
}

int i2c_remove(struct i2c_client *client)
{
    class_destroy(EEPROM.class);
    cdev_del(EEPROM.cdev);
    unregister_chrdev_region(EEPROM.cdev->dev, 1);
    kfree(EEPROM.cdev);
    return 0;
}

static const struct of_device_id of_i2c_match_table[] = {[0] = {.compatible = "zzj,i2cdev", .data = 0}, [2] = {}};

static const struct i2c_device_id i2c_ids[] = {[0] = {"xxxxyyy", (kernel_ulong_t)NULL}, [1] = {/* END OF LIST */}};

static struct i2c_driver my_i2c_driver = {
    .probe = i2c_probe,
    .remove = i2c_remove,
    .id_table = i2c_ids,  // 在linux 4.9.88 不管有没有用都要添加上
    .driver =
        {
            .name = "at24c02",
            .of_match_table = of_i2c_match_table,
        },
};

static int __init eeprom_drv_init(void)
{
    return i2c_add_driver(&my_i2c_driver);
}

static void __exit eeprom_drv_exit(void)
{
    i2c_del_driver(&my_i2c_driver);
}

module_init(eeprom_drv_init);
module_exit(eeprom_drv_exit);

MODULE_LICENSE("GPL");
```