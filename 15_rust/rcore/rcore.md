# rcore
## 1.应用程序与基本执行环境



### 1.1 应用程序执行环境与平台支持

#### 1.1.1 理解应用程序执行环境
在现代通用操作系统（如 Linux）上运行应用程序，需要多层次的执行环境栈支持：
![alt text](rcore.assets/understand_application_execution_env.png)
应用程序执行环境栈：图中的白色块自上而下表示各级执行环境，黑色块则表示相邻两层执行环境之间的接口。 下层作为上层的执行环境，支持上层代码运行。

我们的应用程序通过调用标准库或第三方库提供的接口，仅需少量源代码就能完成复杂的功能； Hello, world! 程序调用的 println! 宏就是由 Rust 标准库 std 和 GNU Libc 等提供的。 这些库属于应用程序的**执行环境** (Execution Environment)，而它们的实现又依赖于操作系统提供的系统调用。

#### 1.1.2 平台与目标三元组

我们从三个重要的定义开始，具体如下：
```tex
Build表示编译交叉编译器的平台
Host表示交叉编译器将运行的平台
Target表示交叉编译器运行生成的可执行文件或者库所针对的平台
在标准的交叉编译器中，Build和Host平台是相同的。目标三元组定义了Build、Host、和Target平台。三元组用信息唯一地指定一个目标，此信息包括处理器架构、操作系统版本、C库类别、和目标文件类型。
```

Rust编译器通过 目标三元组 (Target Triplet) 来描述一个软件运行的目标平台。它一般包括 CPU、操作系统和运行时库等信息，从而控制Rust编译器可执行代码生成。比如，我们可以尝试看一下之前的 Hello, world! 的目标平台是什么。这可以通过打印编译器 rustc 的默认配置信息：Rust编译器通过 目标三元组 (Target Triplet) 来描述一个软件运行的目标平台。它一般包括 CPU、操作系统和运行时库等信息，从而控制Rust编译器可执行代码生成。比如，我们可以尝试看一下之前的 Hello, world! 的目标平台是什么。这可以通过打印编译器 rustc 的默认配置信息：
```shell
rustc --version --verbose
   rustc 1.57.0-nightly (e1e9319d9 2021-10-14)
   binary: rustc
   commit-hash: e1e9319d93aea755c444c8f8ff863b0936d7a4b6
   commit-date: 2021-10-14
   host: x86_64-unknown-linux-gnu
   release: 1.57.0-nightly
   LLVM version: 13.0.0
```
从其中的 host 一项可以看出默认的目标平台是 x86_64-unknown-linux-gnu，其中 CPU 架构是 x86_64，CPU 厂商是 unknown，操作系统是 linux，运行时库是 GNU libc（封装了 Linux 系统调用，并提供 POSIX 接口为主的函数库）。

#### 1.1.3 修改目标平台(交叉编译)
使用`--target` 加 目标平台三元组
```shell
cargo run --target riscv64gc-unknown-none-elf
```
或者使用cargo配置文件
```shell
# 在 project/.cargo/config 中
[build]
target = "riscv64gc-unknown-none-elf"
```

### 1.2 移除标准库依赖

#### 1.2.1 使用#![no_std]移除标准库

在 main.rs 的开头加上一行`#![no_std]`， 告诉 Rust 编译器不使用 Rust 标准库 std 转而使用核心库 core。
```rust
#![no_std]
fn main() {
}
```

#### 1.2.2 提供panic_handler功能应对致命错误

rust一种安全语言，所以在编译程序时，要求实现`panic!`，一般`panic!`被标准库包含，其大致功能是打印出错位置和原因并杀死当前应用。但是我们使用了`#![no_std]`移除标准库依赖，而更底层的核心库core中只有一个panic!宏的空壳。故我们还需要实现`panic!`

```tex
#[panic_handler]
#[panic_handler] 是一种编译指导属性，用于标记核心库core中的 panic! 宏要对接的函数（该函数实现对致命错误的具体处理）。该编译指导属性所标记的函数需要具有 fn(&PanicInfo) -> ! 函数签名，函数可通过 PanicInfo 数据结构获取致命错误的相关信息。这样Rust编译器就可以把核心库core中的 panic! 宏定义与 #[panic_handler] 指向的panic函数实现合并在一起，使得no_std程序具有类似std库的应对致命错误的功能。
```
```rust
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop{}
}
```

#### 1.2.3 移除main函数
标准的main函数是在有标准库依赖时特有的，在无标准库时，应该移除main函数
```rust
// os/src/main.rs
#![no_std]
#![no_main]

mod lang_items;

// os/src/lang_items.rs
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
```
这时就完成了一个完全移除了标准库依赖的rust程序，但是没有执行入口。
接下来将实现入口函数

### 1.3 构建用户态执行环境
#### 1.3.1 入口函数
操作系统的入口函数通常是_start()，在前面我们移除了标准库依赖，所以编译器不会为我们添加`_start()`，在编写用户态程序时，我们写出`_start()`作为我们的入口函数。
```rust
// os/src/main.rs
#[no_mangle]
extern "C" fn _start() {
    loop{};
}
```
我们重新编译代码
```shell
cargo build
#   Compiling os v0.1.0 (/home/shinbokuow/workspace/v3/rCore-Tutorial-v3/os)
#    Finished dev [unoptimized + debuginfo] target(s) in 0.06s
#
# [反汇编导出汇编程序]
rust-objdump -S target/riscv64gc-unknown-none-elf/debug/os
#   target/riscv64gc-unknown-none-elf/debug/os:       file format elf64-littleriscv
#
#   Disassembly of section .text:
#
#   0000000000011120 <_start>:
#   ;     loop {}
#     11120: 09 a0            j       2 <_start+0x2>
#     11122: 01 a0            j       0 <_start+0x2>
```

#### 1.3.2 程序正常退出
我们注释掉`_start()`函数中的loop，再次编译执行
```rust
// os/src/main.rs
#[no_mangle]
extern "C" fn _start() {
}
```
看起来程序是合法的，但是如果我们执行它，会引发问题
```shell
qemu-riscv64 target/riscv64gc-unknown-none-elf/debug/os
#  段错误 (核心已转储)
```
这个简单的程序导致 qemu-riscv64 崩溃了！为什么会这样？

因为在程序执行过程中，pc指针递增执行下一条指令，而_strat()之后的函数大概率是无效的指令，故会报段错误。我们需要调用exit系统调用来结束进程。
```rust
// os/src/main.rs

const SYSCALL_EXIT: usize = 93;

fn syscall(id: usize, args: [usize; 3]) -> isize {
    let mut ret;
    unsafe {
        core::arch::asm!(
            "ecall",
            inlateout("x10") args[0] => ret,
            in("x11") args[1],
            in("x12") args[2],
            in("x17") id,
        );
    }
    ret
}

pub fn sys_exit(xstate: i32) -> isize {
    syscall(SYSCALL_EXIT, [xstate as usize, 0, 0])
}

#[no_mangle]
extern "C" fn _start() {
    sys_exit(9);
}
```
我们编译执行以下修改后的程序：
```shell
cargo build --target riscv64gc-unknown-none-elf
  Compiling os v0.1.0 (/media/chyyuu/ca8c7ba6-51b7-41fc-8430-e29e31e5328f/thecode/rust/os_kernel_lab/os)
    Finished dev [unoptimized + debuginfo] target(s) in 0.26s

[打印程序的返回值]
qemu-riscv64 target/riscv64gc-unknown-none-elf/debug/os; echo $?
9
```
可以看到，返回的结果确实是 9 。这样，我们勉强完成了一个简陋的用户态最小化执行环境

#### 1.3.3 通过write系统调用实现输出字符功能

首先封装一下对 SYSCALL_WRITE 系统调用。
```rust
const SYSCALL_WRITE: usize = 64;

pub fn sys_write(fd: usize, buffer: &[u8]) -> isize {
  syscall(SYSCALL_WRITE, [fd, buffer.as_ptr() as usize, buffer.len()])
}
```

然后实现基于 Write Trait 的数据结构，并完成 Write Trait 所需要的 write_str 函数，并用 print 函数进行包装。
```rust
struct Stdout;

impl Write for Stdout {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        sys_write(1, s.as_bytes());
        Ok(())
    }
}

pub fn print(args: fmt::Arguments) {
    Stdout.write_fmt(args).unwrap();
}

```

最后，实现基于 print 函数，实现Rust语言 格式化宏 ( formatting macros )。
```rust
#[macro_export]
macro_rules! print {
    ($fmt: literal $(, $($arg: tt)+)?) => {
        $crate::console::print(format_args!($fmt $(, $($arg)+)?));
    }
}

#[macro_export]
macro_rules! println {
    ($fmt: literal $(, $($arg: tt)+)?) => {
        print(format_args!(concat!($fmt, "\n") $(, $($arg)+)?));
    }
}
```

接下来，我们调整一下应用程序，让它发出显示字符串和退出的请求：
```rust
#[no_mangle]
extern "C" fn _start() {
    println!("Hello, world!");
    sys_exit(9);
}
```
### 1.4 构建裸机执行环境

有了上一节实现的用户态的最小执行环境，稍加改造，就可以完成裸机上的最小执行环境了。 本节中，我们将把 Hello world! 应用程序从用户态搬到内核态。

#### 1.4.1 裸机启动过程
用 QEMU 软件 qemu-system-riscv64 来模拟 RISC-V 64 计算机。加载内核程序的命令如下：
```shell
qemu-system-riscv64 \
            -machine virt \
            -nographic \
            -bios $(BOOTLOADER) \
            -device loader,file=$(KERNEL_BIN),addr=$(KERNEL_ENTRY_PA)
```
- -bios $(BOOTLOADER) 意味着硬件加载了一个 BootLoader 程序，即 RustSBI
- -device loader,file=$(KERNEL_BIN),addr=$(KERNEL_ENTRY_PA) 表示硬件内存中的特定位置 $(KERNEL_ENTRY_PA) 放置了操作系统的二进制代码 $(KERNEL_BIN) 。 $(KERNEL_ENTRY_PA) 的值是 0x80200000 。

当我们执行包含上述启动参数的 qemu-system-riscv64 软件，就意味给这台虚拟的 RISC-V64 计算机加电了。 此时，CPU 的其它通用寄存器清零，而 PC 会指向 0x1000 的位置，这里有固化在硬件中的一小段引导代码， 它会很快跳转到 0x80000000 的 RustSBI 处。 RustSBI完成硬件初始化后，会跳转到 $(KERNEL_BIN) 所在内存位置 0x80200000 处， 执行操作系统的第一条指令。

#### 1.4.2 实现关机功能
```rust
// os/src/sbi.rs
const SBI_SHUTDOWN: usize = 8;
pub fn shutdown() -> !{
    sbi_call(SBI_SHUTDOWN, 0, 0, 0);
    panic!("It should shutdown!");
}

// os/src/main.rs
#[no_mangle]
extern "C" fn _start() {
    shutdown();
}
```
应用程序访问操作系统提供的系统调用的指令是 ecall ，操作系统访问 RustSBI提供的SBI调用的指令也是 ecall ， 虽然指令一样，但它们所在的特权级是不一样的。 简单地说，应用程序位于最弱的用户特权级（User Mode）， 操作系统位于内核特权级（Supervisor Mode）， RustSBI位于机器特权级（Machine Mode）。 下一章会进一步阐释具体细节。
编译执行，结果如下：
```shell
# 编译生成ELF格式的执行文件
$ cargo build --release
 Compiling os v0.1.0 (/media/chyyuu/ca8c7ba6-51b7-41fc-8430-e29e31e5328f/thecode/rust/os_kernel_lab/os)
  Finished release [optimized] target(s) in 0.15s
# 把ELF执行文件转成bianary文件
$ rust-objcopy --binary-architecture=riscv64 target/riscv64gc-unknown-none-elf/release/os --strip-all -O binary target/riscv64gc-unknown-none-elf/release/os.bin

# 加载运行
$ qemu-system-riscv64 -machine virt -nographic -bios ../bootloader/rustsbi-qemu.bin -device loader,file=target/riscv64gc-unknown-none-elf/release/os.bin,addr=0x80200000
# 无法退出，风扇狂转，感觉碰到死循环
```
问题在哪？通过 rust-readobj 分析 os 可执行程序，发现其入口地址不是 RustSBI 约定的 0x80200000 。我们需要修改程序的内存布局并设置好栈空间。

#### 1.4.3 设置正确的程序内存布局
可以通过 **链接脚本** (Linker Script) 调整链接器的行为，使得最终生成的可执行文件的内存布局符合我们的预期。

修改 Cargo 的配置文件来使用我们自己的链接脚本 `os/src/linker.ld`：
```rust
// os/.cargo/config
[build]
target = "riscv64gc-unknown-none-elf"

[target.riscv64gc-unknown-none-elf]
rustflags = [
    "-Clink-arg=-Tsrc/linker.ld", "-Cforce-frame-pointers=yes"
]
```

具体的链接脚本 os/src/linker.ld 如下：
```tex
OUTPUT_ARCH(riscv)
ENTRY(_start)
BASE_ADDRESS = 0x80200000;

SECTIONS
{
    . = BASE_ADDRESS;
    skernel = .;

    stext = .;
    .text : {
        *(.text.entry)
        *(.text .text.*)
    }

    . = ALIGN(4K);
    etext = .;
    srodata = .;
    .rodata : {
        *(.rodata .rodata.*)
    }

    . = ALIGN(4K);
    erodata = .;
    sdata = .;
    .data : {
        *(.data .data.*)
    }

    . = ALIGN(4K);
    edata = .;
    .bss : {
        *(.bss.stack)
        sbss = .;
        *(.bss .bss.*)
    }

    . = ALIGN(4K);
    ebss = .;
    ekernel = .;

    /DISCARD/ : {
        *(.eh_frame)
    }
}
```

第1行我们设置了目标平台为`riscv`；第2行我们设置了整个程序的入口点为之前定义的全局符号`_start`； 第3行定义了一个常量`BASE_ADDRESS`为`0x80200000`，`RustSBI`期望的`OS`起始地址；

从`BASE_ADDRESS`开始，代码段`.text`, 只读数据段`.rodata`，数据段`.data`, `bss`段`.bss`由低到高依次放置，且每个段都有两个全局变量给出其起始和结束地址（比如 .text段的开始和结束地址分别是`stext`和`etext`）。

#### 1.4.4 正确配置栈空间布局
用另一段汇编代码初始化栈空间：
```asm
# os/src/entry.asm
    .section .text.entry
    .globl _start
_start:
    la sp, boot_stack_top
    call rust_main

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 * 16
    .globl boot_stack_top
boot_stack_top:
```
在第8行，我们预留了一块大小为`4096 * 16`字节，也就是`64kib`的空间， 用作操作系统的栈空间。栈顶地址被全局符号`boot_stack_top`标识，栈底则被全局符号`boot_stack`标识。 同时，这块栈空间被命名为`.bss.stack`，链接脚本里有它的位置。

`_start`作为操作系统的入口地址，将依据链接脚本被放在`BASE_ADDRESS`处。`la sp, boot_stack_top`作为`OS`的第一条指令，将`sp`设置为栈空间的栈顶。简单起见，我们目前不考虑`sp`越过栈底`boot_stack`，也就是栈溢出的情形。 第二条指令则是函数调用`rust_main`，这里的`rust_main`是我们稍后自己编写的应用入口。

接着，我们在 main.rs 中嵌入这些汇编代码并声明应用入口 rust_main ：
```rust
// os/src/main.rs
#![no_std]
#![no_main]

mod lang_items;
core::arch::global_asm!(include_str!("entry.asm"));
#[no_mangle]
pub fn rust_main() -> ! {
    shutdown();
}
```

#### 1.4.5 清空bss段
等一等，与内存相关的部分太容易出错了， 清零 .bss 段 的工作我们还没有完成。
```rust
// os/src/main.rs
fn clear_bss() {
    extern "C" {
        fn sbss();
        fn ebss();
    }
    
    (sbss as usize..ebss as usize).for_each(|a| {
        unsafe { (a as *mut u8).write_volatile(0) }
    });
}

pub fn rust_main() -> ! {
    clear_bss();
    shutdown();
}
```
通过全局全局符号中的`sbss`，`ebss`得到bss段的位置。


## 2.批处理系统



## 3.多道程序与分时任务



## 4.地址空间


## 5.进程及进程管理

## 6.文件系统与io重定向

## 7.进程间通信

## 8.并发
