# 伙伴系统算法

## 为什么使用伙伴系统算法？

在系统的使用过程中，频繁的请求和释放页框，势必会导致已分配的页框之间分散存在了许多小块的空闲页框。由次带来的问题是，即使有足够的空叶匡可以满足请求，但要分配一个大块的连续页框就可能无法满足。

本质上说，避免外碎片由两种方法
- 利用分页单元把一组非连续空闲页框映射到连续的线性地址上
- 开发一种适当的技术来记录现存的空闲页框的分配情况，以尽量避免为满足对小块的请求二分割大的空闲块。

基于以下原因，linux内核首选第二种情况：
- 在某些情况下，如DMA。仅仅是连续页框并不满足请求。MDA在一次单独的IO操作传送磁盘数据时，DMA忽略分页单元而直接访问地址总线。因此，所请求的缓冲区必须是一段连续的页框
- 连续页框分配在保持内核页表不变方面所起到的作用是不容小觑的。频繁的访问页表势必导致平均访问内存的次数增加，这会使CPU频繁的刷新TBL的内容
- 内核通过4Mb的页可以访问到大块的连续物理内存。这样就减小了TBL的失效率，因此提高了访问内存的平均速度

Linux采用著名的伙伴系统(buddy ststem)算法来解决外碎片问题。把所有的空闲页框分组为11和链表块，每个链表块分别包含`1，2，4，5，16，32，62，128，256，512，和1024`和连续的页框。对1024个页框的最大请求对应着4MB大小的来连续RAM块。每个块的第一个物理地址是这个块大小的整数倍。例如，大小为16个页框的块，其起始地址是 `16*12^2` 的倍数。

linux将 大小为b的一对空间合并为一个大小为 2b的单独块。满足以下条件的称为 伙伴
- 两个块具有相同的大小， 记作b
- 他们的物理地址是连续的
- 第一块的第一个页框的物理地址是 2 * b * 12^12的倍数

## 数据结构

linux 2.6为每个内存管理区都使用了不同的伙伴系统。 ZONE_DMA ZONE_NORMAL ZONE_HIGH

伙伴系统使用的主要数据结构如下：
- linux将物理内存使用 `mem_map` 来管理。实际上 每个管理区都对应着 `mem_map` 的子集。子集中的第一个元素和元素大小由管理区数据结构的 `zone_mem_map` 和 `size` 字段指定
- 包含11个元素 (struct free_are; free_area[MAX_ORDER]; MAX_ORDER == 11), 每个元素代表一种块的大小。该数组存放在管理区描述符的free_area字段中

```c
struct zone { // 内存管理区数据结构 省略了一部分字段
	unsigned long		free_pages;
	unsigned long		pages_min, pages_low, pages_high;
	spinlock_t		lock;
	struct free_area	free_area[MAX_ORDER];
	struct page		*zone_mem_map;
}
struct free_area {
	struct list_head	free_list;  // 空闲块链表
	unsigned long		nr_free;    // 空闲块数目
};

struct page {
	page_flags_t flags;			
	atomic_t _count;		
	atomic_t _mapcount;	
	unsigned long private;		
	struct address_space *mapping;	
	pgoff_t index;		
	struct list_head lru; // page 的 lru 成员	链表本体	
};
```
free_area 中的 struct list_head 存储了第一个空闲块首个page的 双向链表字段(lru)。这些 order相同的空闲块使用 lru 串联起来。
nr_free 指定了大小为 order 的空闲块的个数。如果没有空闲块， nr_free == 0， 且 free_list 的前后指针指向自己
另外，一个 2^k 的空闲块的第一个 `page` 的描述符的 `private` 字段  (也就是`free_list 链表中的每个 page 所代表 的 page 的private字段`)存储了 `order` 的值，通过这个值，内核可以确定这个空闲块的伙伴是否也空闲

## 分配块

内核使用 `__rmqueue` 函数来在管理区中找到一个空闲块。该函数需要两个参数。`zone` 内存管理区描述符, `order` 空闲区的大小

```c
/* 
 * Do the hard work of removing an element from the buddy allocator.
 执行从伙伴系统移除元素是一个艰巨的任务
 * Call me with the zone->lock already held.
    在 以获取 zone->lock 锁后调用我
 */
static struct page *__rmqueue(struct zone *zone, unsigned int order)
{
	struct free_area * area;
	unsigned int current_order;
	struct page *page;

	for (current_order = order; current_order < MAX_ORDER; ++current_order) {
        // 找到从管理区的空闲区域 使用 zone 和 order定位要申请的区域和块的大小
		area = zone->free_area + current_order;
        // 查询有无空闲块
		if (list_empty(&area->free_list))
            // 如果没有则 向 更大的 order + 1 继续查询
			continue;

        // 通过结构体中的成员获取结构体指针 内核经典操作
        // 这里是使用 struct page 的成员的指针 area->free_list.next 
        // free_list.next 是 内存管理区 zone的空闲区域的第一个 page 链表
        // 和 lru 这个成员的名字来获取这个结构体
		page = list_entry(area->free_list.next, struct page, lru);
        // 从 链表中删除这个页
		list_del(&page->lru);
        // 移除页的 private 的 order 标志
		rmv_page_order(page);
        // 值为 order 的空闲区块的值 减 1
		area->nr_free--;
        // 内存管理区的空闲 页表 - 2^order 个
		zone->free_pages -= 1UL << order;

        // 分配
		return expand(zone, page, order, current_order, area);
	}

	return NULL;
}

static inline struct page *
expand(struct zone *zone, struct page *page,
 	int low, int high, struct free_area *area)
{
	unsigned long size = 1 << high;
    // 如果找到的 order 比 提交的申请的 order大，则需要将伙伴拆分 
    // 把后后面 2^h - 2^l 个页框循环分配给free_area链表中下标在 h 到 k之间的元素
	// (2^h - 2^l) - 2^(h*-1) 个人总结公式  h* 为初始值为 h 的变量
	while (high > low) {
		// 将指针向前移动  struct free_are free_area[MAX_ORDER]; 
		// 第一次的area指向的order = high 的free_area 
		// 相当于area指向了 order - 1 的free_area
		area--;  
		high--;
		size >>= 1;  // 剩余的大小
		// BUG_ON(bad_range(zone, &page[size]));
		list_add(&page[size].lru, &area->free_list);
		area->nr_free++;
		set_page_order(&page[size], high);
	}
	return page;
}
```


list_entry函数展开
```cpp
/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})  
```

list_del 展开
```c
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
    // 将被解开的节点的前后指针指向内核中的"空",未被引用的标记
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    // 将当前节点的前后节点互相指引，这样就是删除了当前节点
	next->prev = prev;
	prev->next = next;
}
```


## 释放块

__free_pages_bulk函数按照伙伴系统的策略释放页框。它使用3个基本输入参数。
page
	被释放的块中第一个页面描述符的地址
zone
	管理区描述符
order 
	被释放的块的大小

```c
static inline void __free_pages_bulk (struct page *page, struct page *base,
		struct zone *zone, unsigned int order)
{
	unsigned long page_idx;
	struct page *coalesced;
	int order_size = 1 << order; 

	if (unlikely(order))
		destroy_compound_page(page, order);
	page_idx = page - base;
	
	BUG_ON(page_idx & (order_size - 1));
	BUG_ON(bad_range(zone, page));

	zone->free_pages += order_size;
	while (order < MAX_ORDER-1) {
		struct free_area *area;
		struct page *buddy;
		int buddy_idx;

		buddy_idx = (page_idx ^ (1 << order));
		buddy = base + buddy_idx;
		if (bad_range(zone, buddy))
			break;
		if (!page_is_buddy(buddy, order))
			break;
		/* Move the buddy up one level. */
		list_del(&buddy->lru);
		area = zone->free_area + order;
		area->nr_free--;
		rmv_page_order(buddy);
		page_idx &= buddy_idx;
		order++;
	}
	coalesced = base + page_idx;
	set_page_order(coalesced, order);
	list_add(&coalesced->lru, &zone->free_area[order].free_list);
	zone->free_area[order].nr_free++;
}
``

```c
	int order_size = 1 << order; 
	zone->free_pages += order_size;
```
order_size为计算出的当前页块的数目
然后加入 zone->free_pages 页数目计数

然后是while循环，循环最多进行 MAX_ORDER-1 - order 次
为什么是 MAX_ORDER-1 呢，因为 order最大下标为 MAX_order - 1

```c
	struct page *buddy;
	int buddy_idx;
	buddy_idx = (page_idx ^ (1 << order));
	buddy = base + buddy_idx;
```
在while循环中寻找当前页框块的伙伴页框块 `buddy_idx` ， 伙伴块的下标可以使用位运算简单运算
(page_idx ^ (1 << order)) 将页框块的下标的第 order 位取反，相当于page_index + (1 << order) 或者 page_index - (1 << order)

一旦知道了伙伴块的下标，就可以根据基址找到伙伴块的下标

```c
	if (bad_range(zone, buddy))
		break;
	if (!page_is_buddy(buddy, order))
		break;
	/* Move the buddy up one level. */
	list_del(&buddy->lru);
	area = zone->free_area + order;
	area->nr_free--;
	rmv_page_order(buddy);
	page_idx &= buddy_idx;
	order++;
```
接下来是检查该伙伴块是否合法，如果该伙伴块合法，则将这个伙伴块从当前free_area中删除，相当于将当前块和伙伴块合并
然后想上一级order + 1 free_area继续寻找

```c
static inline int page_is_buddy(struct page *page, int order)
{
       if (PagePrivate(page)           &&
           (page_order(page) == order) &&
           !PageReserved(page)         &&
            page_count(page) == 0)
               return 1;
       return 0;
}
```
检查一个伙伴块是否合法，首先检查page的Private字段是否合法，然后检查Private字段的值是否为oroder再然后检查页框的属性是否为可移动内存，最后他的 page的_count字段必须为-1 page_count在页空闲时返回 0。
以上条件均满足后，就可以释放伙伴块，继续向上尝试合并更大的块

```c
	coalesced = base + page_idx;
	set_page_order(coalesced, order);
	list_add(&coalesced->lru, &zone->free_area[order].free_list);
	zone->free_area[order].nr_free++;
```
最终跳出while循环就开始将块合并进入对应order的free_area中

