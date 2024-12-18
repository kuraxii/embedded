
```c
asmlinkage __visible __init __no_sanitize_address __noreturn __no_stack_protector
void start_kernel(void)
{
	char *command_line;
	char *after_dashes;

	// 设置任务栈结束标记
    set_task_stack_end_magic(&init_task);
    // 设置处理器ID
    smp_setup_processor_id();
    // 早期调试对象初始化
    debug_objects_early_init();
    // 初始化vmlinux构建ID
    init_vmlinux_build_id();

    // 早期cgroup初始化
	cgroup_init_early();

    // 禁用中断
	local_irq_disable();
	early_boot_irqs_disabled = true;

    // 初始化cpu
	boot_cpu_init();
    // 初始化页面地址映射
	page_address_init();
    // 打印内核启动的信息
	pr_notice("%s", linux_banner);
    // 早期的安全初始化
	early_security_init();
	// 架构相关的初始化， 包含解析参数 内存的映射等
    setup_arch(&command_line);
	setup_boot_config();
	setup_command_line(command_line);
    // 设置cpu的数量
	setup_nr_cpu_ids();
    // 设置cpu的内存区域
	setup_per_cpu_areas();
    // 准备引导CPU的SMP（对称多处理）相关设置。通常包括特定架构的引导CPU钩子。
	smp_prepare_boot_cpu();	
    // 初始化引导CPU的热插拔支持。设置CPU热插拔相关的数据结构和配置。
	boot_cpu_hotplug_init();

	pr_notice("Kernel command line: %s\n", saved_command_line);
	

	// 初始化静态键
    jump_label_init();

    // 解析早期参数
    parse_early_param();

    // 解析命令行参数
    after_dashes = parse_args("Booting kernel",
                static_command_line, __start___param,
                __stop___param - __start___param,
                -1, -1, NULL, &unknown_bootoption);

    // 打印未知的启动选项
    print_unknown_bootoptions();

    // 如果有额外的参数，继续解析
    if (!IS_ERR_OR_NULL(after_dashes))
        parse_args("Setting init args", after_dashes, NULL, 0, -1, -1,
            NULL, set_init_arg);
    if (extra_init_args)
        parse_args("Setting extra init args", extra_init_args,
            NULL, 0, -1, -1, NULL, set_init_arg);

    // 架构和非时间保持随机数生成器初始化，在分配器初始化之前
    random_init_early(command_line);

    // 这些使用大块的bootmem分配，必须在页面分配器初始化之前进行
	setup_log_buf(0);
	vfs_caches_init_early();
	sort_main_extable();
	trap_init();
	mm_core_init();
	poking_init();
	ftrace_init();

	// trace_printk可以在这里启用
	early_trace_init();

	// 在启动任何中断（如定时器中断）之前设置调度器。
    // 完整的拓扑设置在smp_init()时进行 - 但同时我们仍然有一个功能正常的调度器。
	sched_init();

	if (WARN(!irqs_disabled(),
		 "Interrupts were enabled *very* early, fixing it\n"))
		local_irq_disable();

    // 初始化基数树，用于高效的键值存储和查找。
	radix_tree_init();

    // 初始化枫树，用于高效的键值存储和查找。
    maple_tree_init();

	/*
	 * Set up housekeeping before setting up workqueues to allow the unbound
	 * workqueue to take non-housekeeping into account.
	 */
	housekeeping_init();

	/*
	 * Allow workqueue creation and work item queueing/cancelling
	 * early.  Work item execution depends on kthreads and starts after
	 * workqueue_init().
	 */
	workqueue_init_early();

    // 初始化RCU（Read-Copy Update），用于高效的读写同步机制。
	rcu_init();

	// 初始化跟踪事件，允许在此之后进行事件跟踪。
	trace_init();

    // 如果启用了initcall调试，则启用initcall调试功能。
	if (initcall_debug)
		initcall_debug_enable();

    // 初始化上下文跟踪，用于跟踪上下文切换和中断处理。
	context_tracking_init();

    /* init some links before init_ISA_irqs() */
    // 初始化早期中断处理。
    early_irq_init();
    
    // 初始化中断请求（IRQ）处理。
    init_IRQ();

    // 初始化系统时钟滴答。
    tick_init();

    // 初始化无滴答RCU（Read-Copy Update）。
    rcu_init_nohz();

    // 初始化定时器子系统。
    init_timers();

    // 初始化SRCU（Sleepable Read-Copy Update）。
    srcu_init();

    // 初始化高分辨率定时器。
    hrtimers_init();

    // 初始化软中断处理。
    softirq_init();

    // 初始化时间保持机制。
    timekeeping_init();

    // 初始化系统时间。
    time_init();

	// 这必须在时间保持初始化之后
    random_init();

    // 这些使用完全初始化的随机数生成器
    kfence_init();
    boot_init_stack_canary();

    // 初始化性能事件
    perf_event_init();

    // 初始化性能分析
    profile_init();

    // 初始化调用函数
    call_function_init();

    WARN(!irqs_disabled(), "Interrupts were enabled early\n");

    early_boot_irqs_disabled = false;
    // 开启中断
    local_irq_enable();

    // 延迟初始化内存缓存
    kmem_cache_init_late();

	// 警告！这是早期的。我们在进行PCI设置等之前启用了控制台，console_init()必须意识到这一点。
    // 但我们确实希望尽早输出，以防出现问题。
	console_init();
	if (panic_later)
		panic("Too many boot %s vars at `%s'", panic_later,
		      panic_param);

    // 初始化锁依赖性
	lockdep_init();

	/*
	 * Need to run this when irqs are enabled, because it wants
	 * to self-test [hard/soft]-irqs on/off lock inversion bugs
	 * too:
	 */
	locking_selftest();


    // 设置每个CPU的页面集
    setup_per_cpu_pageset();

    // 初始化NUMA（非统一内存访问）策略
    numa_policy_init();

    // 早期初始化ACPI（高级配置和电源接口）
    acpi_early_init();

    if (late_time_init)
        late_time_init();

    // 初始化调度时钟
    sched_clock_init();

    // 校准延迟
    calibrate_delay();

    // 完成架构CPU初始化
    arch_cpu_finalize_init();

    // 初始化PID IDR（ID分配器）
    pid_idr_init();

    // 初始化匿名VMA（虚拟内存区域）
    anon_vma_init();

    #ifdef CONFIG_X86
    if (efi_enabled(EFI_RUNTIME_SERVICES))
        efi_enter_virtual_mode();
    #endif

    // 初始化线程栈缓存
    thread_stack_cache_init();

    // 初始化凭证
    cred_init();

    // 初始化进程创建
    fork_init();

    // 初始化进程缓存
    proc_caches_init();

    // 初始化UTS（UNIX时间共享）命名空间
    uts_ns_init();

    // 初始化密钥管理
    key_init();

    // 初始化安全模块
    security_init();

    // 延迟初始化调试
    dbg_late_init();

    // 初始化网络命名空间
    net_ns_init();

    // 初始化VFS（虚拟文件系统）缓存
    vfs_caches_init();

    // 初始化页面缓存
    pagecache_init();

    // 初始化信号处理
    signals_init();

    // 初始化序列文件
    seq_file_init();

    // 初始化proc文件系统根目录
    proc_root_init();

    // 初始化命名空间文件系统
    nsfs_init();

    // 初始化CPU集
    cpuset_init();

    // 初始化cgroup（控制组）
    cgroup_init();

    // 早期初始化任务统计
    taskstats_init_early();

    // 初始化延迟账户
    delayacct_init();

    // 初始化ACPI子系统
    acpi_subsystem_init();

    // 在ACPI子系统初始化后进行架构特定的初始化
    arch_post_acpi_subsys_init();

    // 初始化KCSAN（Kernel Concurrency Sanitizer）
    kcsan_init();

    /* 执行其余的非__init代码，我们现在活着了 */
    arch_call_rest_init();

    /*
    * 避免gcc-10及更早版本中调用boot_init_stack_canary的调用者中的栈金丝雀。
    */
    #if !__has_attribute(__no_stack_protector__)
    prevent_tail_call_optimization();
    #endif
}
```