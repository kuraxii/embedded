# 每cpu页帧缓存 (The Per-CPU Page Frame Cache)

内核经常请求和释放单个页框。在这样的场景下，页的分配效率比较低。为了提升系统性能，内存管理区引入了每cpu叶帧缓存（`The Per-CPU Page Frame Cache`）。每个 `cpu` 的高速缓存会预先缓存一些单个页框，用于该cpu申请单个页框。从而避免了频繁的访问全局页表

实际上，每个cpu对于每个内存管理区都有两个缓存，一个 `hot cache`，他的存储位置很有可能在 `cpu` 硬件高速缓存中 中， 一个 `cold cache`。
总的来说 `hot cache` 更有可以存放在更高速级别存储介质中。


## 数据结构

```c
struct zone{
    /* ...  */
    struct per_cpu_pageset	pageset[NR_CPUS];
    /* ...  */
};

struct per_cpu_pageset {
	struct per_cpu_pages pcp[2];	/* 0: hot.  1: cold */
} ____cacheline_aligned_in_smp;

struct per_cpu_pages {
	int count;		/* number of pages in the list */
	int low;		/* low watermark, refill needed */
	int high;		/* high watermark, emptying needed */
	int batch;		/* chunk size for buddy add/remove */
	struct list_head list;	/* the list of pages */
};
```
每 `cpu` 页帧缓存 的主要数据结构 是 `zone` 描述符中的`pageset` 字段结构体数组中的 `per_cpu_pages`数组。
`pageset` 数组的大小为 `NR_CPUS`，页管理区为每个 `cpu` 都分配了一个 `per_cpu_pages`。

`per_cpu_pages`数组大小为2。`0` 为 `hot pages`，`1` 为 `cold pages`

`per_cpu_pages`字段描述
|类型|名称|描述|
|----|---|----|
|int|count|该链表中物理页的个|
|int|low|当链表中的物理页个数低于该数值,会从zone buddy系统申请页框|
|int|high|当链表中的物理页个数超过该数值,会将部分页返还给zone buddy系统|
|int|batch|每次返还给buddy系统的物理页的个数|
|struct list_head|list|高速缓存中的页框描述符链表|

总的来说，通过 low high batch三个字段来控制缓存中的页框数目

## 通过每CPU页帧缓存分配页框

从每cpu页帧缓存申请页框，内核使用`buffered_rmqueue`函数，该函数有三个参数`zone`内存管理区，`order`请求的页块大小的对数，`gfp_flags` 从 `hot cache` 申请还是从`hot cache`申请
```c
static struct page *
buffered_rmqueue(struct zone *zone, int order, int gfp_flags)
{
	unsigned long flags;
	struct page *page = NULL;
	int cold = !!(gfp_flags & __GFP_COLD);

	if (order == 0) {
		struct per_cpu_pages *pcp;

		pcp = &zone->pageset[get_cpu()].pcp[cold];
		local_irq_save(flags);
		if (pcp->count <= pcp->low)
			pcp->count += rmqueue_bulk(zone, 0,
						pcp->batch, &pcp->list);
		if (pcp->count) {
			page = list_entry(pcp->list.next, struct page, lru);
			list_del(&page->lru);
			pcp->count--;
		}
		local_irq_restore(flags);
		put_cpu();
	}

	if (page == NULL) {
		spin_lock_irqsave(&zone->lock, flags);
		page = __rmqueue(zone, order);
		spin_unlock_irqrestore(&zone->lock, flags);
	}

	if (page != NULL) {
	
		mod_page_state_zone(zone, pgalloc, 1 << order);
		prep_new_page(page, order);

		if (gfp_flags & __GFP_ZERO)
			prep_zero_page(page, order, gfp_flags);

		if (order && (gfp_flags & __GFP_COMP))
			prep_compound_page(page, order);
	}
	return page;
}
```

buffered_rmqueue函数在指定的内存管理区分配页框。使用cpu高速缓存来分配单个页框（分配单个页框只对order=0有效）。

1. 计算申请的页框是冷缓存还是热缓存
    ```c
    int cold = !!(gfp_flags & __GFP_COLD);
    ```
2. 首先判断oeder是否为0， 为 0 就从cpu页帧缓存中分配，否则跳过
    ```c
    if (order == 0) {
        struct per_cpu_pages *pcp;
        pcp = &zone->pageset[get_cpu()].pcp[cold];
        // 如果count 小于 low，从伙伴系统申请页框
        if (pcp->count <= pcp->low)
            pcp->count += rmqueue_bulk(zone, 0,
                        pcp->batch, &pcp->list);
        // 缓存还有页框则从缓存分配                
        if (pcp->count) {
            // 分配页框链表中的第一个页框
            page = list_entry(pcp->list.next, struct page, lru);
            list_del(&page->lru);
            // 页帧缓存页框数减1
            pcp->count--;
        }
        put_cpu();
        // put_cpu get_cpu 函数对，禁止抢占并确保代码块在同一个CPU上执行，以保持处理的局部性和防止数据结构的竞争状态。
        // 因为 cpu页帧缓存是每个cpu针对页框的数据结构，操作过程中不能切换cpu
    }
    
    // rmqueue_bulk 循环分配大量大小为order的页框
    static int rmqueue_bulk(struct zone *zone, unsigned int order, 
			unsigned long count, struct list_head *list)
    {
        unsigned long flags;
        int i;
        int allocated = 0;
        struct page *page;
    
        for (i = 0; i < count; ++i) {
            page = __rmqueue(zone, order);
            if (page == NULL)
                break;
            allocated++;
            list_add_tail(&page->lru, list);
        }
    
        return allocated;
    }
    
    ```
3. 验证没有分配成功 或者 申请的页框数 order > 0,则从伙伴系统中区分配页框块
    ```c
    if (page == NULL) {
		spin_lock_irqsave(&zone->lock, flags);
		page = __rmqueue(zone, order);
		spin_unlock_irqrestore(&zone->lock, flags);
	}
    ```
4. 如果在上一步还是分配失败了，就返回NULL，否则 初始化第一个页框的页描述符
   清除一些标志，将private字段置0，并将引用计数置1

   ```c
    if (page != NULL) {
        // 这是内核的一个统计函数
		mod_page_state_zone(zone, pgalloc, 1 << order);
		prep_new_page(page, order);
		if (gfp_flags & __GFP_ZERO)
			prep_zero_page(page, order, gfp_flags);
        // 分配的是一个页块， 初始化页块
		if (order && (gfp_flags & __GFP_COMP))
			prep_compound_page(page, order);
	}
   ```
## 释放页框到每cpu页帧缓存

释放页框到每cpu页帧缓存，内核使用`free_hot_page` 和`free_cold_page`函数，这两个函数是`free_hot_cold_page`的前端函数

```c
void fastcall free_hot_page(struct page *page)
{
	free_hot_cold_page(page, 0);
}
	
void fastcall free_cold_page(struct page *page)
{
	free_hot_cold_page(page, 1);
}
static void fastcall free_hot_cold_page(struct page *page, int cold)
{
	struct zone *zone = page_zone(page);
	struct per_cpu_pages *pcp;
	unsigned long flags;

	arch_free_page(page, 0);
	kernel_map_pages(page, 1, 0);
	//inc_page_state(pgfree);
	if (PageAnon(page))
		page->mapping = NULL;
	free_pages_check(__FUNCTION__, page);
	pcp = &zone->pageset[get_cpu()].pcp[cold];
	//local_irq_save(flags);
	if (pcp->count >= pcp->high)
		pcp->count -= free_pages_bulk(zone, pcp->batch, &pcp->list, 0);
	list_add(&page->lru, &pcp->list);
	pcp->count++;
	//local_irq_restore(flags);
	put_cpu();
}
```

`free_hot_cold_page` 的执行流程如下
1. 从page的flag字段获取当前页框的管理区
    ```c
    struct zone *zone = page_zone(page);
    ```
2. 获取由cold标志所选择的 per_cpu_pages 的地址
    ```c
    pcp = &zone->pageset[get_cpu()].pcp[cold];
    ```
3. 判断 count 与 high的值，如果count >= high，就释放掉batch个页框，返回给伙伴系统
    ```c
    if (pcp->count >= pcp->high)
		pcp->count -= free_pages_bulk(zone, pcp->batch, &pcp->list, 0);
    ```
4. 将当前页框添加到`per_cpu_pages`，并增加`count`字段
   ```c
        list_add(&page->lru, &pcp->list);
        pcp->count++;
   ```
   值得注意的是，在当前linux 2.6内核，没有页框会被释放到冷缓存中。对于硬件缓存，内核总是假设被释放的页框放入`hot cache`中。当然，着并不意味着`cold cache`是空的，当到达`low`下界时，会使用`buffered_rmqueue`申请页框。


## 一些疑问

为什么在申请的时候没有调用arch_free_page(page, 0); kernel_map_pages(page, 1, 0);对应的相关的函数，却在释放的时候区调用了arch_free_page(page, 0); kernel_map_pages(page, 1, 0);

1. 架构相关的释放准备 `arch_free_page`
   可能涉及到特定架构需要在物理内存页面被释放回内存池之前做的准备工作，例如清理或重置与该页面相关的硬件特定数据（如TLB条目或其他缓存机制）。这是一个预防性的步骤，确保页面在重新分配前不保留旧数据的痕迹或配置。
2. 更新内核页表 `kernel_map_pages`
   在这里是用来在内核的地址空间中取消映射该页面。这样做的目的是防止释放后的页面被意外访问，从而可能导致安全问题或数据错误。在页面分配时，页面会自动映射到需要的地址空间中，所以在分配时不需要显式调用取消映射。
3. 释放时的安全检查 `free_pages_check`
   这一调用是用来在释放页面之前进行一系列的完整性和一致性检查，这是为了确保释放的页面不会导致未定义的行为或内核崩溃。
4. 性能优化
   在**页面分配**时，通常关注的是如何快速有效地找到一个足够的页面来满足请求。这个过程中，**内核会尽量减少对页面的操作以提高效率**。相反，在页面释放时，进行更多的清理和安全检查是有益的，因为这可以为未来的分配提供一个更稳定和可靠的环境。
5. 分配与释放的不对称
   内存分配与释放在操作系统中往往是不对称的。分配时，系统的目标是尽快满足请求，而释放时则更注重彻底清理和正确归还资源。这就解释了为什么在释放过程中会有额外的步骤，而在分配时则可能没有相应的对称操作。
