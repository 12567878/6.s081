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
volatile static int page_refer_num[(PHYSTOP-KERNBASE)/4096]={0};

struct spinlock refer_lock;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;


int
i_refer(uint64 pa){
    return (pa-KERNBASE)/4096;
}

int
refer_num(uint64 pa){
    return page_refer_num[i_refer(pa)];
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
    initlock(&refer_lock,"refer");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
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
    panic("kfree: invalid/unaligned pa");

  int i= i_refer((uint64)pa);
  if(page_refer_num[i]>1){
      page_refer_num[i]-=1;
      return;
  }


  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
  page_refer_num[i]=0;
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

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  if(r){
      int i= i_refer((uint64)r);
      if(page_refer_num[i]!=0){
          panic("kalloc: refer_num management fault");
      }
      page_refer_num[i]=1;
  }
  return (void*)r;
}



//plus大于零给pa引用加1，小于等于0则减一
void
add_refer_num(uint64 pa,int plus){
    acquire(&refer_lock);
    int i = i_refer(pa);
    if(plus>0) page_refer_num[i]+=1;
    else {
        panic("don't use");
//        page_refer_num[i] -= 1;
//        if(page_refer_num[i]<0) panic("kalloc.c/add_i: refer_num < 0\n");
    }
    release(&refer_lock);
}