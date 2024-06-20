# 中断及中断子系统

## 1. 什么是中断

### 1.1 为什么要有中断

处理器速度很快，外围设备速度往往不在一个数量级上，如果处理器给硬件发出一个请求，再专门等回应，显然是浪费了cpu的处理其它事务的能力，所以采取当外围硬件处理好自己的事件，再来发请求的时候，处理器再回头处理它。

现在操作系统关于中断已经广义的说法了，对于系统而言，为了对进程的调度及状态切换，在系统软件层也实现了不依赖于中断控制器的软件中断，通常是程序通过特定指令（如x86架中的INT汇编指令，ARM架构中SWI汇编指令）主动触发的中断。比如允许用户程序请求操作系统提供服务的系统调用就是软件中断。

所以说中断是系统实现实时性的核心。

### 1.2 中断的分类

#### 1.2.1 硬件中断（Hardware Interrupt）

**定义：**硬件中断是由外部硬件设备（如键盘、鼠标、硬盘、网络接口等）产生的信号，通知CPU需要处理某个事件。硬件中断是异步的，因为它们可以在任何时候发生，不依赖于当前正在执行的指令序列。

**特点：**

1. 异步：不依赖于CPU当前执行的指令，可以在任何时候发生
2. 高优先级：通常比普通的程序执行优先级高，能够打断当前执行的指令序列
3. 响应速度快：硬件中断的设计目的是为了快速响应外部设备的请求

**应用场景：**

- 当键盘被按下时，生成键盘中断。
- 当网络接口接收到数据包时，生成网络中断。
- 硬盘读取或写入完成时，生成硬盘中断。

**处理过程**：（分配中断 保存现场,关中断  处理   回复现场）

1. 硬件设备发送中断信号给中断控制器，中断控制器分配CPU处理相应的中断任务。
2. CPU暂时停止当前的指令执行，保存当前状态。
3. CPU跳转到对应的中断处理程序（Interrupt Service Routine，ISR）来处理中断。
4. 处理完成后，恢复之前的状态并继续执行被打断的程序。

#### 1.2.2 软件中断（Software Interrupt）

**定义**：软件中断是由程序通过特定指令（如x86架构中的INT指令，ARM架构为SWI指令）主动触发的中断。软件中断通常用于操作系统提供的系统调用，允许用户程序请求操作系统提供的服务。

**特点**：

1. **同步：**由程序执行特定指令触发，发生在程序控制之内。
2. **可控性：**程序员可以决定何时触发软件中断。
3. **与硬件无关：**不依赖于外部硬件设备，完全由软件逻辑控制。

**应用场景**：

- 用户程序需要操作系统的服务（如文件读写、内存分配等）时，触发软件中断进行系统调用。
- 程序异常处理（如除零错误、非法访问等），通过软件中断处理异常情况。

**处理过程**：

1. 程序执行触发中断的指令。
2. CPU响应中断，保存当前状态。
3. CPU跳转到对应的中断处理程序（通常是操作系统提供的系统调用处理程序）。
4. 处理完成后，恢复之前的状态并继续执行程序。

#### 1.2.3对比总结

- **触发方式：**
  - 硬件中断：由外部硬件设备触发。
  - 软件中断：由程序指令触发（intel: int    arm: swi）。
- **时机：**
  - 硬件中断：异步，可以在任何时候发生。
  - 软件中断：同步，在程序执行中主动触发。
- **应用：**
  - 硬件中断：响应外部设备的请求。
  - 软件中断：实现系统调用和异常处理。

**总结**：**通过硬件中断和软件中断，计算机系统可以有效地处理各种事件，提供高效的资源管理和服务响应能力。**

**不过软件中断都已经由系统构建好，我们这里主要学习的是硬件中断。**

#### 1.2.4 根据中断号分类

![alt text](./Linux_driver.assets/interrupt01.png)

## 2. 中断的实现原理

### 2.1 硬件中断的实现结构框图

![alt text](./Linux_driver.assets/interrupt03.png)

### 2.2 软件中断的实现结构框图

![alt text](./Linux_driver.assets/interrupt02.png)

### 2.3 设备树节点的描述

```c
mykey{
    compatible = "WX,my_device_key";
    dev_type = "KEY";
    interrupt-parent = <&gpiof>;
    //key1 = PF9,   key2 = PF7, Key3 = PF8
    interrupts = <9 0>,<7 0>,<8 0>;
    status = "okay";
};


// 也可以使用gpio端口申请irq号
myled{
    model = "this is a led desc";
    compatible = "zzj,my_device_001";
    dev_type = "LED";
    led-gpio = <&gpio5 3 GPIO_ACTIVE_LOW>;
    // 描述按键中断 引脚
    key-gpios = <&gpio5 1 GPIO_ACTIVE_LOW>,
                <&gpio4 14 GPIO_ACTIVE_LOW>;
    status = "okay";
	};
```


## 3. 中断子系统

### 3.1 中断子系统框架

![alt text](./Linux_driver.assets/interrupt04.png)

### 3.2 获取中断号

####  3.2.1 直接获取

```c
int of_irq_get(struct device_node *dev, int index);
// 解析一个节点的IRQ并将其作为Linux IRQ编号返回 
// @dev: 指向设备树节点的指针 
// @index: IRQ的从零开始的索引 
// 返回值: * 成功时返回Linux IRQ编号； 
// 如果IRQ映射失败，则返回0；
// 如果IRQ域尚未创建，则返回-EPROBE_DEFER；
// 其他任何失败情况下返回错误代码。 
```

> 示例

```c
// 设备树
mykey{
    compatible = "WX,my_device_key";
    dev_type = "KEY";
    interrupt-parent = <&gpiof>;
    //key1 = PF9,   key2 = PF7, Key3 = PF8
    interrupts = <9 0>,<7 0>,<8 0>;
    status = "okay";
};

mykey.key1_interrupts = of_irq_get(key_node,0);
request_irq(mykey.key1_interrupts,key_ISR,IRQF_TRIGGER_RISING | IRQF_TRIGGER_RISING,"key-interrupts",NULL);
```


#### 3.2.2 通过gpio获取
```c
// 设备树
myled{
    model = "this is a led desc";
    compatible = "zzj,my_device_001";
    // 描述按键中断 引脚
    key-gpios = <&gpio5 1 GPIO_ACTIVE_LOW>,
                <&gpio4 14 GPIO_ACTIVE_LOW>;
    status = "okay";
	};


descs = gpiod_get_array(&p_dev->dev, "key", GPIOD_IN);
if (IS_ERR(descs)) {
    ret = PTR_ERR(descs);
    goto err_put_desc;
}

for (i = 0; i < descs->ndescs; i++) {
    mykey[i].irq = gpiod_to_irq(descs->desc[i]);
    if (mykey[i].irq < 0) {
        ret = -1;
        goto err_free_irqs;
    }

    ret = request_irq(mykey[i].irq, irq_isr, IRQF_TRIGGER_RISING, "key", NULL);
    if (ret) {
        goto err_free_irqs;
    }
}
```

#### 3.2.3 注册中断处理程序 xx_isr(中断上文)

```c
int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
const char *name, void *dev);
// 函数作用：请求中断并注册中断处理程序的函数，给调用BSP原厂中断控制器的驱动进行相关寄存器进行设置。
// 函数参数：
// 1.irq:中断号，是指要请求的Linux中断号。
// 2.handle:中断处理函数，类型：typedef irqreturn_t (*irq_handler_t)(int irq, void* dev),就是一个函数指针.
// 回调函数的中的参数irq 即为中断号，参数dev即传递过来的dev最后一个参数。
// 回调函数返回值是irqreturn_t 是一个枚举类型有三个值：
enum irqreturn{
    IRQ_NONE = 0(0 << 0),
    IRQ_HANDLE = (1 << 0),
    IRQ_WAKE_THREAD = (1 << 1)
};
// 同文件中还有个宏定义：
#define IRQ_RETVAL(x) ((x) ? IRQ_HANDLED : IRQ_NONE)
// 所以也可以使用此宏做为返回值。

// 3.flags:中断标志，就是标记什么情况下发生的中断，Linux中提供了相应的标记:/include/linux/interrupt.h下定义
#define IRQF_TRIGGER_NONE   0x00000000
#define IRQF_TRIGGER_RISING 0x00000001  上升沿
#define IRQF_TRIGGER_FALLING    0x00000002 下降沿
#define IRQF_TRIGGER_HIGH   0x00000004 高电平
#define IRQF_TRIGGER_LOW    0x00000008 低电平
#define IRQF_TRIGGER_MASK   (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
                 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_TRIGGER_PROBE  0x00000010

// 4.name:中断的名字，在/proc/irq下可以看到，由程序员定义
// 5.dev:中断发生后调用中断处理函数传递给中断处理函数的参数。如果中断设置标志为共享（IRQF_SHARED）的话，此参数用来区分具体的中断
// 共享中断只有在释放最后中断处理函数的时候才会被禁止掉，即传递具体那个设备的地址，说明是那个设备发出的中断。
// 返回值：成功返回0，失败返回非零错误码。
```
因为ISR中断处理例程在执行前系统为关闭当前cpu的中断，所以在ISR执行时，不可以有阻塞的操作。执行实时性比较高的任务，如果任务需要延迟处理，则应该放在中断的下文tasklet或workqueue中执行。

