// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
	struct spinlock lock;
	int counter[(PHYSTOP-KERNBASE)/PGSIZE];
} refcnt;

void
kinit()
{
	initlock(&refcnt.lock, "refcnt");
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

inline void
acquire_refcnt()
{
	acquire(&refcnt.lock);
}

inline void 
release_refcnt()
{
	release(&refcnt.lock);
}

inline void add_ref(uint64 pa)
{
	refcnt.counter[(pa-KERNBASE)/PGSIZE] += 1;
}

inline void dec_ref(uint64 pa)
{
	int index = (pa-KERNBASE)/PGSIZE;
	if(refcnt.counter[index] <= 0)
		panic("free page\n");
	refcnt.counter[index] = refcnt.counter[index] - 1;
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
		int index = ((uint64)p - KERNBASE) / PGSIZE;
		refcnt.counter[index] = 0;
		kfree(p);
	}
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

	acquire_refcnt();
	if( refcnt.counter[((uint64)pa - KERNBASE)/PGSIZE] > 1 ){
		dec_ref((uint64)pa);
		release_refcnt();
		return ;
	} else 
		refcnt.counter[((uint64)pa - KERNBASE) / PGSIZE] = 0;
	release_refcnt();
	
	
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
		acquire_refcnt(&refcnt.lock);
		refcnt.counter[((uint64)r-KERNBASE) / PGSIZE] = 1;
		release_refcnt(&refcnt.lock);
	}
  return (void*)r;
}
