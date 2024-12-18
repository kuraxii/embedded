# 管理区分配器（The Zone Allocator）

相关函数：
- alloc_pages(gfp_mask, order)
- struct page * fastcall__alloc_pages(unsigned int gfp_mask, unsigned int order, struct zonelist *zonelist)

管理区分配器(alloc_pages)是内核分配器的前端（前面讲到的伙伴系统，每cpu页帧缓存等等不能直接调用，通过管理区分配器来调用）。该部件必须分配一个足够大的空闲页块以满足内存请求。这个任务并不是看起来都的那么简单，因为内存管理区分配器必须满足几个目标：
- 它应该保护保留的页
- 当内存不足且允许阻塞当前进程时，它应当触发页框回收算法；一旦某些页框被释放，管理区分配器将再次尝试分配。
- 如果可能，他应该保存小而珍贵的ZOME_DMA内存管理区

从前面的章节（分区页框分配器 The Zoned Page Frame Allocator）我们知道，对一组连续页帧的请求实际上是通过`alloc_pages`宏来处理的。紧接着，这个宏又调用`__alloc_pages`函数，这是管理区分配的核心。在linux源码的注释里， `This is the 'heart' of the zoned buddy allocator 这是分区伙伴分配器的“核心”`。它接收三个参数：
|  类型|名称|描述|
|--------|-------|--------|
|unsigned int|gpf_mask|指定了内存请求的标志|
|unsigned int|order|要分配的连续页框的大小的对数|
|struct zonelist*|zonelist|指向zonelist结构体的指针，结构体存放struct zone指针数组，按优先顺序描述了适于分配的内存管理区|
```c
struct zonelist {
	struct zone *zones[MAX_NUMNODES * MAX_NR_ZONES + 1]; // NULL delimited
};

#define alloc_pages(gfp_mask, order) \
		alloc_pages_node(numa_node_id(), gfp_mask, order)
//  返回当前节点的编号        
#define numa_node_id()		(cpu_to_node(_smp_processor_id()))
```

__alloc_pages 函数扫描zone_list中的所有内存管理区，其代码如下：
```c
    for(i = 0; (z = zone_list[i]) != NULL; i++){
        if(zone_watermark_ok(z, order, ...)){
            page = buffered_rmqueue(z, order, gpf_mask);
            if(page)
                return page;
        }
    }
```
对于每个内存管理区，该函数将空闲页框数与一个阈值比较，这个阈值取决于内存申请标志，当前进程类型以及管理区被函数被检查的次数。实际上，如果空闲内存不足，那么每个内存管理区都会被检查几次，每次都会在请求最小空闲页框的基础上比上次更小
因此，前面的一段代码在`__alloc_pages`出现了几次，但是变化都很小

`buffered_rmqueue` 函数在前面的已经详细解析过了，它返回页块的首个页框的页描述符，如果分配失败，内存管理区没有足够大小的空闲页框块，就返回`NULL`

`zone_watermark_ok`辅助函数接收几个参数，它们决定内存管理区的阈值`min`
特别的，如果满足下列条件则返回1
1. 除了被分配的页框，在管理区中至少还有min个空闲页框，
2. 除了被分配的页框外，这里在`order`至少为k的块中起码还有`min/(2^k)`个空闲框，其中，对于每个k，取值在1和分配的`order`之间。因此对于`order > 0`，那么在大小至少为2的块中的块起码还有min/2个空闲页框;如果`order > 1`，那么在大小至少为4的块中起码还有min/4个空闲页框。以此类推

`zone_watermark_ok`的函数实现在[mm/page_alloc.c line664](https://elixir.bootlin.com/linux/v2.6.11/source/mm/page_alloc.c#L664)中

函数声明
```c
int zone_watermark_ok(struct zone *z, int order, unsigned long mark,
		      int classzone_idx, int can_try_harder, int gfp_high)
```
|  类型|名称|描述|
|--------|-------|--------|
|struct zone*|z|内存管理区数据结构数组|
|unsigned int|order|要分配的连续页框的大小的对数|
|unsigned long|mark|当前阈值，见下面第2点|
|int|classzone_idx|当前的内存管理区下标|
|int|can_try_harder|标志位 见下面第3点|
|int|gfp_high|标志位，是否位高端地址|

min的值在zone_watermark_ok函数中确定：具体如下
1. 作为min的基本值，在函数传递可以是 `page_high`，`page_low`，`page_min`中的任意一个。
    ```c
        long min = mark, free_pages = z->free_pages - (1 << order) + 1;
    ```
2. 如果作为参数传递的`gfp_high`被置位，那么`min`的值`/2`。通常如果`gfp_mask`中的`__GFP_WAIT`标志被置位，则这个标志就被置`1`，**`gfp_high`代表更加激进的分配方式，不用考虑页框的剩余量**
   ```c
    if (gfp_high)
		min -= min / 2;
   ```
3. 如果作为参数传递的`can_try_harder`被置位，那么`min`的值将再减少`1/4`。如果`gfp_mask`中的`__GFP_WAIT`标志被置位，或者当前进程是个实时进程并且内存分配是在进程在上下文中（中断程序和可延时函数除外）完成的，则`can_try_harder`被置1
   ```c
    if (can_try_harder)
		min -= min / 4;
   ```
函数的判定策略  暂时留个坑
总之 **这个函数主要是用来检查在特定的内存区中是否有足够的空闲内存来保证系统的健康运行和避免内存过度分配的风险**
```c
    if (free_pages <= min + z->lowmem_reserve[classzone_idx])
		return 0;
	for (o = 0; o < order; o++) {
		/* At the next order, this order's pages become unavailable */
		free_pages -= z->free_area[o].nr_free << o;

		/* Require fewer higher order pages to be free */
		min >>= 1;

		if (free_pages <= min)
			return 0;
	}
	return 1;
```


`__alloc_pages`的函数实现在[mm/page_alloc.c line964](https://elixir.bootlin.com/linux/v2.6.11/source/mm/page_alloc.c#L694)中
执行步骤如下：
1. 执行对内存管理区的第一次扫描，在第一次扫描中，阈值被设置为`pages_low`，如果分配到了满足条件的pages，就跳转到got_pg 函数返回
    ```c
        /* Go through the zonelist once, looking for a zone with enough free */
        for (i = 0; (z = zones[i]) != NULL; i++) {

            if (!zone_watermark_ok(z, order, z->pages_low,
                        classzone_idx, 0, 0))
                continue;

            page = buffered_rmqueue(z, order, gfp_mask);
            if (page)
                goto got_pg;
        }
    /* ... */    
    got_pg:
        // 一个统计分析函数，用于内存管理的优化，不再深究 
        zone_statistics(zonelist, z);
        return pages;
    ```
2. 如果在第一次没有成功的分配到页框，则唤醒`kswapd`内核线程，来异步的回收页框
    ```c
    for (i = 0; (z = zones[i]) != NULL; i++)
        wakeup_kswapd(z, order);
    ```
3. 执行对管理区的第二次扫描， 这次将`can_try_harder`置`1`，和引入`__GFP_HIGH`以更加激进的分配标志。降低了基础阈值。
    ```c
    for (i = 0; (z = zones[i]) != NULL; i++) {
		if (!zone_watermark_ok(z, order, z->pages_min,
				       classzone_idx, can_try_harder,
				       gfp_mask & __GFP_HIGH))
			continue;
		page = buffered_rmqueue(z, order, gfp_mask);
		if (page)
			goto got_pg;
	}
    /* ... */    
    got_pg:
        // 一个统计分析函数，用于内存管理的优化，不再深究 
        zone_statistics(zonelist, z);
        return pages;
    ```
4. 如果函数在上一步还是没有终止，那么表明确实是系统内存不足。如果发出内存请求的内核控制路径不在中断上下文并且正在进行内存页框回收，就进行第三次扫描
   这次扫描忽略了阈值`min`。**唯有这种情况下才允许内核控制路径耗尽为页框不足预留的页**（由管理区的lowmem_reserve字段指定）。如果还是没有分配到页框，则返回NULL提示调用者发生了错误，没有分配到页框。实际上，这种情况的内存申请目的不是为了获得更多内存来使用，而是为了通过某种方式（可能是内存压缩、回收不再使用的页面等）来释放占用的内存页框架。
    ```c
    /* This allocation should allow future memory freeing. */
    if (((p->flags & PF_MEMALLOC) || unlikely(test_thread_flag(TIF_MEMDIE))) && !in_interrupt()) {
        /* go through the zonelist yet again, ignoring mins */
        for (i = 0; (z = zones[i]) != NULL; i++) {
            page = buffered_rmqueue(z, order, gfp_mask);
            if (page)
                goto got_pg;
        }
        goto nopage;
    }
    /* ... */    
    nopage:
	if (!(gfp_mask & __GFP_NOWARN) && printk_ratelimit()) {
		printk(KERN_WARNING "%s: page allocation failure."
			" order:%d, mode:0x%x\n",
			p->comm, order, gfp_mask);
		dump_stack();
	}
    ```
5. 在这里，正在调用的内核控制路径并不是为了回收内存，如果__GFP_WAIT标志位没有被置位，就返回NULL提示该内核控制路径内存分配失败。这种情况下，如果不阻塞就无法满足分配需求
    ```c
    /* Atomic allocations - we can't balance anything */
	if (!wait)
		goto nopage;
    /* ... */    
    nopage:
	if (!(gfp_mask & __GFP_NOWARN) && printk_ratelimit()) {
		printk(KERN_WARNING "%s: page allocation failure."
			" order:%d, mode:0x%x\n",
			p->comm, order, gfp_mask);
		dump_stack();
	}
	return NULL;
    ```
6. 在这里，当前进程可以被阻塞。调用`cond_resched`检查其他进程是否需要cpu
    ```c
        cond_resched();
    ```
7. 设置PF_MEMALLOC标志表示进程已经准备好做内存回收
    ```c
    /* We now go into synchronous reclaim */
    p->flags |= PF_MEMALLOC;
    ```
8. 将`reclaim_state`中的`reclaimed_slab`标志设置为0，这个结构体只有这一个字段。（在后面`slab分配器章节将会看到如何使用它`）。之后函数可能可能会阻塞这个进程
    ```c
    reclaim_state.reclaimed_slab = 0;
	p->reclaim_state = &reclaim_state;
    ```
9. 调用`try_to_free_pages`尝试去寻找一些页来回收，（在17章，内存紧缺回收在分析）。一旦函数返回，就清空之前设置的标志位，并再次调用`cond_resched`
    ```c
    did_some_progress = try_to_free_pages(zones, gfp_mask, order);
    p->reclaim_state = NULL;
	p->flags &= ~PF_MEMALLOC;
    ```
10. 如果上一步已经回收了一些页框，那么接下来还会执行第三步相同的内存管理区扫描
    ```c
	if (likely(did_some_progress)) {
		/*
		 * Go through the zonelist yet one more time, keep
		 * very high watermark here, this is only to catch
		 * a parallel oom killing, we must fail if we're still
		 * under heavy pressure.
		 */
		for (i = 0; (z = zones[i]) != NULL; i++) {
			if (!zone_watermark_ok(z, order, z->pages_min,
					       classzone_idx, can_try_harder,
					       gfp_mask & __GFP_HIGH))
				continue;

			page = buffered_rmqueue(z, order, gfp_mask);
			if (page)
				goto got_pg;
		}
    }
    ```
11. 如果在第9步没有释放任何页框，说明内核遇到了很大的麻烦，应为页框已经少到了危险的底部，并且无法在回收到任何页框。也许到了做出重大决策的时候了。如果内核控制路径允许依赖文件系统的操作来杀死一个进程，并且`__GFP_NORETRY`标志被置0，那么执行以下步骤
    1. 使用`z->pages_high`的阈值再次扫描内存管理区，尝试最后的分配，如果还是不行就进行下一步
    2. 调用`out_of_memory`杀死一个进程来一些释放页框。
    3. 跳回到第一步，再次进行页框分配操作
    因为第1步的阈值要远高于之前设置的阈值，所以这个步骤很容易失败。实际上，只有当另一个内核控制路径杀死一个进程来回收了它的页框后，第1步才有可能执行成功。
    因此第1步避免了两个进程被无辜的杀死。
    ```c
    else if ((gfp_mask & __GFP_FS) && !(gfp_mask & __GFP_NORETRY)) {
		/*
		 * Go through the zonelist yet one more time, keep
		 * very high watermark here, this is only to catch
		 * a parallel oom killing, we must fail if we're still
		 * under heavy pressure.
		 */
		for (i = 0; (z = zones[i]) != NULL; i++) {
			if (!zone_watermark_ok(z, order, z->pages_high,
					       classzone_idx, 0, 0))
				continue;

			page = buffered_rmqueue(z, order, gfp_mask);
			if (page)
				goto got_pg;
		}
		out_of_memory(gfp_mask);
		goto restart;
	}
    ```
12. 如果在`10`，`11` 步，都没有满足的条件，并且`__GFP_NORETRY`被置0，并且`__GFP_NOFAIL`或`__GFP_REPEAT`被置位）就调用`blk_congestion_wait`休眠一会，再返回`第6步`重新来，否则返回NULL，通知调用者内存分配失败
    ```c
    do_retry = 0;
    if (!(gfp_mask & __GFP_NORETRY)) {
        if ((order <= 3) || (gfp_mask & __GFP_REPEAT))
            do_retry = 1;
        if (gfp_mask & __GFP_NOFAIL)
            do_retry = 1;
    }
    if (do_retry) {
        blk_congestion_wait(WRITE, HZ/50);
        goto rebalance;
    }
    ```


	

