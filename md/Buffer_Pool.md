# Buffer Pool Manager 

数据库系统的一个主要目标是减少磁盘和内存之间块的传输，其中一种办法就是在内存中保留尽量多的块，但是，不可能在内存中保留所有的块。

Buffer是 内存中存储磁盘块的备份的地方，Buffer Manager是负责缓冲区分配的子系统。

这个Lab就是要实现一个 Buffer Pool Manager

## Extendible Hashing Table 

实现一个动态哈希表。

注意 global depth增加 的条件 和 桶分裂的条件。

桶分裂的条件: 只要插入新元素之后，桶中元素的数目溢出了，桶就一定要分裂。 （但是不能无限分裂下去）

global depth增加的条件 : 插入新元素之后，桶中元素溢出了，且该桶的local depth等于global depth（或者说只有一个指针指向这个桶，就不得不增加 global depth）

还有注意的地方是，一次插入，可能会引起引起桶的多次分裂，因为桶重新分配之后，所有的元素还有可能在桶一个桶里面。

还有一种情况是，一个桶中的所有的元素的哈希值是相等的，如果这个桶溢出了，无论怎么重新分配（分裂桶），所有的元素还是会在同一个桶里面，所以要限制桶分裂的次数，超过一定次数，就把溢出的元素放在一个溢出桶里面，不再分裂桶。 

## LRU REPLACER 

当缓冲区中没有块时，需要从缓存区中移除一个旧的块，为新的块留位置。 （这就替换）

大部分的数据库采用的替换机制是 LRU(Least Recently Used) 最近最少使用。把最近最少使用的块移除。

算法根据数据的历史访问记录来进行淘汰数据，其核心思想是“如果数据最近被访问过，那么将来被访问的几率也更高”。 

新数据插入到链表头部。 

某块缓存数据被访问，则将这块数据移到链表头部。 

当链表满的时候，将链表尾部的数据丢弃。

## Buffer Pool Manager 

利用了上面实现的Extendible Hashing 和 LRU

数据结构如下:

```c
  size_t pool_size_; // number of pages in buffer pool
  Page *pages_;      // array of pages
  DiskManager *disk_manager_;
  HashTable<page_id_t, Page *> *page_table_; // 利用page_id 查找每一个page 
  Replacer<Page *> *replacer_;   // to find an unpinned page for replacement
  std::list<Page *> *free_list_; // to find a free page for replacement
  std::mutex latch_;             // to protect shared data structure
```

实现的机制：

根据page_id 查找需要的块，Buffer Manager要做的就是，现在 Buffer中查找 有没有 id 为 page_id 的块，如果有，就直接返回。

如果没有，就要从磁盘中加载id为page_id的块到内存中，加载到何处呢？

首先，先如果free_table_里面还有空闲的块，就使用空闲的块。

如果，free_table_中没有空闲的块，Buffer Manager就需要启用替换机制（LRU），把缓冲区中，最近最少使用的块移除，然后，把新的块，加载到这个块中。 需要注意的是，移除之前，如果这个块是dirty的，需要先写回到到磁盘，再把新的块加载到当前块中。 

写回磁盘需要注意的问题：

被钉住的块(pinned block)为了数据库系统能从系统奔溃中恢复，需要限制一个块写回磁盘的时间。当一个缓冲区中的块上的更新操作正在进行时，不允许写回磁盘，因为这个块被钉住了。 




