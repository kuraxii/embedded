# 名词解释
虚拟页(VP, Virtual Page),虚拟空间中的页；
物理页(PP, Physical Page),物理内存中的页；
磁盘页(DP, Disk Page),磁盘中的页。

# linux中的分段



# linux中的分页 (linux 2.6.11)

页全局目录(PGD, Page Global Directory )

页上级目录(PUD, Page Upper Directory)

页中间目录(PMD, Page Middle Directory)

页表(Page Table)

**常规的32bit分页（常规使用4k分页，最开始使用三级分页，32位的线性地址被分为三个域）**

| Directory   | Table        | offset    |
| ----------- | ------------ | --------- |
| hight 10bit | middle 10bit | low 12bit |

线性地址的转换分为两步完成，每一步都基于一种转换表，第一种转换表称为页目录表(page directory)，第二种转换表称为页表(page table)。

为什么需要两级呢？

目的在于减少每个进程页表所需的 RAM 的数量。如果使用简单的一级页表，将需要高达2^20 个表项来表示每个进程的页表，即时一个进程并不使用所有的地址，二级模式通过置位进程实际使用的那些虚拟内存区请求页表来减少内存容量。

**每个活动的进程必须有一个页目录，但是却没有必要马上为所有进程的所有页表都分配 RAM，只有在实际需要一个页表时候才给该页表分配 RAM。**

**Directory字段和Table字段都是10位长，因此页目录和页表都可以多达1024项。那么一个页目录可以寻址到高达`1024*1024*4096=4294967296=4G`个存储单元，这和32位地址所期望的一样**

# 页框管理

linux页框采用4kb大小

## 页描述符

内核必须记录每个页框当前的状态，如内核必须能区分哪些页框包含的时属于进程的页，而哪些页框包含的是内核代码或内核数据

```c
struct page {
	page_flags_t flags;		/* Atomic flags, some possibly
					 * updated asynchronously */
	atomic_t _count;		/* Usage count, see below. */
	atomic_t _mapcount;		/* Count of ptes mapped in mms,
					 * to show when page is mapped
					 * & limit reverse map searches.
					 */
	unsigned long private;		/* Mapping-private opaque data:
					 * usually used for buffer_heads
					 * if PagePrivate set; used for
					 * swp_entry_t if PageSwapCache
					 * When page is free, this indicates
					 * order in the buddy system.
					 */
	struct address_space *mapping;	/* If low bit clear, points to
					 * inode address_space, or NULL.
					 * If page mapped as anonymous
					 * memory, low bit is set, and
					 * it points to anon_vma object:
					 * see PAGE_MAPPING_ANON below.
					 */
	pgoff_t index;			/* Our offset within mapping. */
	struct list_head lru;		/* Pageout list, eg. active_list
					 * protected by zone->lru_lock !
					 */
	/*
	 * On machines where all RAM is mapped into kernel address space,
	 * we can simply calculate the virtual address. On machines with
	 * highmem some memory is mapped into kernel virtual memory
	 * dynamically, so we need a place to store that address.
	 * Note that this field could be 16 bits on x86 ... ;)
	 *
	 * Architectures with slow multiplication can define
	 * WANT_PAGE_VIRTUAL in asm/page.h
	 */
#if defined(WANT_PAGE_VIRTUAL)
	void *virtual;			/* Kernel virtual address (NULL if
					   not kmapped, ie. highmem) */
#endif /* WANT_PAGE_VIRTUAL */
};
```

**页框的状态信息保存在一个类型为page的页描述符中。所有的页描述符存放在mem_map数组中。**

页描述符字段（仅了解关键字段）

>  **_count**:
>
> 页的引用计数器。
>
> 值为-1，则页空间
>
> 大于-1则说明页框被分配给了一个或多个进程，或用于存放内核数据结构
>
> page_count 函数返回_count + 1的值，为使用该页的使用者数量



## 内存管理区

linux 2.6.11 将内存划分为3个内存管理区域

ZONE_DMA

​	包含低于16MB的内存页框

ZONE_NORMAL

​	包含高于16MB且低于896MB的内存 (896 ~ 1024 这128MB内存一般用于内核映射高端内存)

ZOONE_HIGHMEM

​	包含从896MB开始高于896MB的内存页框


## neicun


## 高端内存页框的映射

内核线性地址空间的最后128mb专门用来映射高端内存页框

linux有三种不同的高端内存映射机制
- 永久内核映射
- 临时内核映射
- 非连续内存分配

### 永久内核映射

永久内核映射允许内核建立高端页框到内核地址空间的长期映射

使用永久内核映射的页表都存放在 `pkmap_page_table` 变量中， 页表的项数由 `LAST_PKMAP` 宏产生。 `LAST_PKMAP` 为页表总数，一般为 512 或 1024
数组 `pkmap_count` 与 `pkmap_page_table`共同作用，存储永久内核映射的信息

pkmap_count 为页的引用计数

> 页的引用计数器。
>
> 值为-1，则页空间
>
> 大于-1则说明页框被分配给了一个或多个进程，或用于存放内核数据结构
>
> page_count 函数返回pkmap_count + 1的值，为使用该页的使用者数量

**linux 源码 highmem.h解释了高端内存映射的顺序** 

```c
/*
 * Ordering is:
 *
 * FIXADDR_TOP  1024MB
 * 			fixed_addresses
 * FIXADDR_START
 * 			temp fixed addresses
 * FIXADDR_BOOT_START
 * 			Persistent kmap area  // 永久内核映射区域
 * PKMAP_BASE  // 永久内核映射的起始
 *   8kb
 * VMALLOC_END
 * 			Vmalloc area
 * VMALLOC_START
 *	 8mb
 * high_memory  896MB
 */
```

为了记录高端内存页框与永久内核映射包含的线性地址之间的联系。内核使用了 `page_address_htable` 散列表。
表中包含一个 `page_address_map` 的结构，记录了高端页框与内核映射的联系。(该数据结构包含一个指向页描述符的指针和分配给该页框的线性地址)

### 永久内核映射的过程

前端函数 kamp
```c
void *kmap(struct page *page)
{
	might_sleep();
	if (!PageHighMem(page))  
		return page_address(page);  
	return kmap_high(page);  // 页框是高端内存，则进入 kmap_high 进行分配
}
```
分配前端函数 kmap_high
```c
void fastcall *kmap_high(struct page *page)
{
	unsigned long vaddr;
	/*
	 * For highmem pages, we can't trust "virtual" until
	 * after we have the lock.
	 *
	 * We cannot call this from interrupts, as it may block
	 */
	spin_lock(&kmap_lock);  // 使用一个自旋锁来确保页表只被 一个线程 访问，保护页表的同步性
	vaddr = (unsigned long)page_address(page); // 用来测试是否是高端内存，或者是否已经被映射
	if (!vaddr)
		vaddr = map_new_virtual(page);  // 创建一个地址映射，并返回线性地址 (将页框填入pkmap_page_table中，并返回线性地址)
	pkmap_count[PKMAP_NR(vaddr)]++;  // 计数加一，将永久内核映射页表计数 +1
	if (pkmap_count[PKMAP_NR(vaddr)] < 2)
		BUG();
	spin_unlock(&kmap_lock);
	return (void*) vaddr;
}
```

建立地址映射的函数 map_new_virtual
```c
// 本质上是个双层 for 循环
static inline unsigned long map_new_virtual(struct page *page)
{
	unsigned long vaddr;
	int count;

start:
	count = LAST_PKMAP;  // 一轮至少循环 LAST_PKMAP 次
	/* Find an empty entry */
	for (;;) {
        // 从上次查询的地方开始查询  last_pkmap_nr = (last_pkmap_nr + 1) & LAST_PKMAP_MASK;
		last_pkmap_nr = (last_pkmap_nr + 1) & LAST_PKMAP_MASK;   // 0 - 511 循环
		if (!last_pkmap_nr) {   // last_pkmap_nr == 0 从头开始检查时，刷新一次pkmap_count
			flush_all_zero_pkmaps();
			count = LAST_PKMAP;
		}
		if (!pkmap_count[last_pkmap_nr])  // 找到pkmap_count == 0 的表，表示可以进行映射
			break;	/* Found a usable entry */
		if (--count)
			continue;

		/*
		 * Sleep for somebody else to unmap their entries
		 */

         /* 如果没有查询到，等待直到其他进程释放了页框 */
		{
			DECLARE_WAITQUEUE(wait, current);

			__set_current_state(TASK_UNINTERRUPTIBLE);
			add_wait_queue(&pkmap_map_wait, &wait);
			spin_unlock(&kmap_lock);
			schedule();
			remove_wait_queue(&pkmap_map_wait, &wait);
			spin_lock(&kmap_lock);

			/* Somebody else might have mapped it while we slept */
            // 在等待的过程中，可能会有其他的线程把页框映射到了内核，故检查一下
			if (page_address(page))
				return (unsigned long)page_address(page);

			/* Re-start */
			goto start;
		}
	}
	vaddr = PKMAP_ADDR(last_pkmap_nr); // 获取 last_pkmap_nr 个页表处，的线性地址
	set_pte(&(pkmap_page_table[last_pkmap_nr]), mk_pte(page, kmap_prot));

	pkmap_count[last_pkmap_nr] = 1;  // 映射成功设置为 1，出函数后会再次 +1 。
	set_page_address(page, (void *)vaddr);  // 将线性地址，与 page 联系。 设置 page_address_map

	return vaddr;
}
```

内核页表刷新函数 将所有 count == 1 的页表置 0
```c
static void flush_all_zero_pkmaps(void)
{
	int i;

	flush_cache_kmaps();

	for (i = 0; i < LAST_PKMAP; i++) {
		struct page *page;

		/*
		 * zero means we don't have anything to do,
		 * >1 means that it is still in use. Only
		 * a count of 1 means that it is free but
		 * needs to be unmapped
		 */
		if (pkmap_count[i] != 1)  
			continue;
		pkmap_count[i] = 0;

		/* sanity check */
		if (pte_none(pkmap_page_table[i]))
			BUG();

		/*
		 * Don't need an atomic fetch-and-clear op here;
		 * no-one has the page mapped, and cannot get at
		 * its virtual address (and hence PTE) without first
		 * getting the kmap_lock (which is held here).
		 * So no dangers, even with speculative execution.
		 */
		page = pte_page(pkmap_page_table[i]);
		pte_clear(&pkmap_page_table[i]);

		set_page_address(page, NULL);
	}
	flush_tlb_kernel_range(PKMAP_ADDR(0), PKMAP_ADDR(LAST_PKMAP));
}
```

## 临时内核映射

临时内核映射比永久内核映射简单; 此外，它可以用在中断处理程序和可延迟函数的内部，因为他们从不阻塞当前线程

在高端内存的任何一页可以通过 `km_type` 数据结构表示。该数据结构实现如下。
```cpp
enum km_type {
	KM_BOUNCE_READ,
	KM_SKB_SUNRPC_DATA,
	KM_SKB_DATA_SOFTIRQ,
	KM_USER0,
	KM_USER1,
	KM_BIO_SRC_IRQ,
	KM_BIO_DST_IRQ,
	KM_PTE0,
	KM_PTE1,
	KM_IRQ0,
	KM_IRQ1,
	KM_SOFTIRQ0,
	KM_SOFTIRQ1,
	KM_TYPE_NR
};
```
内核必须保证每个符号都只能由一个内核成分使用，并以该成份命名

在 `km_type` 中每个符号(除了最后一个)都是固定映射的线性地址的下标。

`fixed_addresses` 数据结构 有两个字段，`FIX_KMAP_BEGIN` `FIX_KMAP_END`, `FIX_KMAP_END` 被赋值为 `FIX_KMAP_BEGIN+(KM_TYPE_NR*NR_CPUS)-1`
如此，每个cpu就都有 `KM_TYPE_NR - 1` 个固定的映射线性地址。
```cpp
enum fixed_addresses {
#ifdef CONFIG_HIGHMEM
	FIX_KMAP_BEGIN,	/* reserved pte's for temporary kernel mappings */
	FIX_KMAP_END = FIX_KMAP_BEGIN+(KM_TYPE_NR*NR_CPUS)-1,
#endif
	__end_of_fixed_addresses
};
```


为了建立临时内核映射, 内核调用 `kmap_atomic` 来实现
使用type 和 cpu 标识符(smp_processor_id()) 指定了使用哪个固定的映射线性地址
```c
/*
 * kmap_atomic/kunmap_atomic is significantly faster than kmap/kunmap because
 * no global lock is needed and because the kmap code must perform a global TLB
 * invalidation when the kmap pool wraps.
 * 
 * However when holding an atomic kmap is is not legal to sleep, so atomic
 * kmaps are appropriate for short, tight code paths only.
 * 然而，在[线程]持有kmap时睡眠不是合法的，所以kmaps只适用于紧凑而短的代码
 */
void *kmap_atomic(struct page *page, enum km_type type)
{
	enum fixed_addresses idx;
	unsigned long vaddr;

	/* even !CONFIG_PREEMPT needs this, for in_atomic in do_page_fault */
	inc_preempt_count();

    // 如果页框不属于高端内存，直接返回线性地址
	if (!PageHighMem(page))  
		return page_address(page);

    // 通过 type 与 cpu 标识符 找到临时内存映射的偏移量
	idx = type + KM_TYPE_NR*smp_processor_id();  /
	
    // 通过偏移量 取得 映射窗口(也是页框)的线性地址
    // #define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
    vaddr = __fix_to_virt(FIX_KMAP_BEGIN + idx);  
#ifdef CONFIG_DEBUG_HIGHMEM
	if (!pte_none(*(kmap_pte-idx)))
		BUG();
#endif
    // 将pteval写入pteptr所指的页表项中:
    // #define set_pte(pteptr, pteval) (*(pteptr) = pteval)
	set_pte(kmap_pte-idx, mk_pte(page, kmap_prot));
	__flush_tlb_one(vaddr);

	return (void*) vaddr;
}
```
翻译和设置页表条目

下面这组宏用来地址映射（地址及page到PTE）以及单个的页表条目的设置,ptep_get_and_clear用来保护和修改页表条目或物理页

| --------名称------- | --------------------功能------------------------------ |
| ------------------- | ------------------------------------------------------ |
| mk_pte()            | 通过 struct page 和 保护位，联合生成一个pte_t 的页表项 |
| mk_pte_phys()       | 类似mk_pte，不同的是使用的页面物理地址作为输入参数     |
| pte_page            | 通过pte_t页表项获得memmap中对应的struct page 地址      |
| pmd_page            | 通过pmd_t页表项获得memmap中pte所在的struct page 地址   |
| set_pte             | 将 mk_pte返回的pte_t 表项内容设置到进程的pte页表项中   |
| pte_clear           | 清除pte页表中一个页表项；set_pte的逆操作               |
| ptep_get_and_clear  | 返回并清除页表中的一个条目pte_t                        |





