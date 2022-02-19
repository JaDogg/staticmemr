//----------------------------------------------------------------
//
// Few features added by Bhathiya Perera
//
// Added stmmr_realloc, stmmr_calloc
// Should not crash if NULL is passed to stmmr_free
//----------------------------------------------------------------
// Statically-allocated memory manager
//
// by Eli Bendersky (eliben@gmail.com)
//
// This code is in the public domain.
//----------------------------------------------------------------
#ifndef STMMR_H
#define STMMR_H
#include <stdint.h>
//
// Memory manager: dynamically allocates memory from
// a fixed pool that is allocated statically at link-time.
//
// Usage: after calling stmmr_init() in your
// initialization routine, just use stmmr_alloc() instead
// of malloc() and stmmr_free() instead of free().
// Naturally, you can use the preprocessor to define
// malloc() and free() as aliases to stmmr_alloc() and
// stmmr_free(). This way the manager will be a drop-in
// replacement for the standard C library allocators, and can
// be useful for debugging memory allocation problems and
// leaks.
//
// Preprocessor flags you can define to customize the
// memory manager:
//
// STMMR_DEBUG_FATAL
//    Allow printing out a message when allocations fail
//
// STMMR_DEBUG_SUPPORT_STATS
//    Allow printing out of stats in function
//    stmmr_print_stats When this is disabled,
//    stmmr_print_stats does nothing.
//
// Note that in production code on an embedded system
// you'll probably want to keep those undefined, because
// they cause printf to be called.
//
// STMMR_POOL_SIZE
//    Size of the pool for new allocations. This is
//    effectively the heap size of the application, and can
//    be changed in accordance with the available memory
//    resources.
//
// STMMR_MIN_POOL_ALLOC_QUANTAS
//    Internally, the memory manager allocates memory in
//    quantas roughly the size of two ulong objects. To
//    minimize pool fragmentation in case of multiple allocations
//    and deallocations, it is advisable to not allocate
//    blocks that are too small.
//    This flag sets the minimal ammount of quantas for
//    an allocation. If the size of a ulong is 4 and you
//    set this flag to 16, the minimal size of an allocation
//    will be 4 * 2 * 16 = 128 bytes
//    If you have a lot of small allocations, keep this value
//    low to conserve memory. If you have mostly large
//    allocations, it is best to make it higher, to avoid
//    fragmentation.
//
// Notes:
// 1. This memory manager is *not thread safe*. Use it only
//    for single thread/task applications.
//
#define STMMR_DEBUG_SUPPORT_STATS 1
#ifndef STMMR_POOL_SIZE
#define STMMR_POOL_SIZE 8 * 1024
#endif
#ifndef STMMR_MIN_POOL_ALLOC_QUANTAS
#define STMMR_MIN_POOL_ALLOC_QUANTAS 16
#endif
typedef uint64_t stmmr_int_t;
// Initialize the memory manager. This function should be called
// only once in the beginning of the program.
//
void stmmr_init();
// 'malloc' clone
//
void *stmmr_alloc(stmmr_int_t nbytes);
// realloc clone
//
void *stmmr_realloc(void *p, stmmr_int_t nbytes);
// calloc clone
//
void *stmmr_calloc(stmmr_int_t num, stmmr_int_t nbytes);
// 'free' clone
//
void stmmr_free(void *ap);
// Prints statistics about the current state of the memory
// manager
//
void stmmr_print_stats();
#ifdef STMMR_IMPL
#include <memory.h>
#include <stdio.h>
union stmmr_mem_header_union {
  struct {
    // Pointer to the next block in the free list
    //
    union stmmr_mem_header_union *next;
    // Size of the block (in quantas of sizeof(stmmr_mem_header_t))
    //
    stmmr_int_t size;
  } s;
  // Used to align headers in memory to a boundary
  //
  stmmr_int_t align_dummy_;
};
typedef union stmmr_mem_header_union stmmr_mem_header_t;
// Initial empty list
//
static stmmr_mem_header_t base;
// Start of free list
//
static stmmr_mem_header_t *freep = 0;
// Static pool for new allocations
//
static uint8_t real_pool[STMMR_POOL_SIZE] = {0};
static uint8_t *pool = real_pool;
static stmmr_int_t pool_free_pos = 0;
void stmmr_init() {
  base.s.next = 0;
  base.s.size = 0;
  freep = 0;
  pool_free_pos = 0;
}
void stmmr_print_stats() {
#ifdef STMMR_DEBUG_SUPPORT_STATS
  stmmr_mem_header_t *p;
  printf("------ Memory manager stats ------\n\n");
  printf("Pool: free_pos = %llu (%llu bytes left)\n\n", pool_free_pos,
         STMMR_POOL_SIZE - pool_free_pos);
  p = (stmmr_mem_header_t *) pool;
  while (p < (stmmr_mem_header_t *) (pool + pool_free_pos)) {
    printf("  * Addr: %p; Size: %8llu\n", p, p->s.size);
    p += p->s.size;
  }
  printf("\nFree list:\n\n");
  if (freep) {
    p = freep;
    while (1) {
      printf("  * Addr: %p; Size: %8llu; Next: %p\n", p, p->s.size, p->s.next);
      p = p->s.next;
      if (p == freep) break;
    }
  } else {
    printf("Empty\n");
  }
  printf("\n");
#endif// STMMR_DEBUG_SUPPORT_STATS
}
static stmmr_mem_header_t *stmmr_get_mem_from_pool(stmmr_int_t nquantas) {
  stmmr_int_t total_req_size;
  if (nquantas < STMMR_MIN_POOL_ALLOC_QUANTAS)
    nquantas = STMMR_MIN_POOL_ALLOC_QUANTAS;
  total_req_size = nquantas * sizeof(stmmr_mem_header_t);
  if (pool_free_pos + total_req_size <= STMMR_POOL_SIZE) {
    stmmr_mem_header_t *h;
    h = (stmmr_mem_header_t *) (pool + pool_free_pos);
    h->s.size = nquantas;
    stmmr_free((void *) (h + 1));
    pool_free_pos += total_req_size;
  } else {
    return 0;
  }
  return freep;
}
// Allocations are done in 'quantas' of header size.
// The search for a free block of adequate size begins at the point 'freep'
// where the last block was found.
// If a too-big block is found, it is split and the tail is returned (this
// way the header of the original needs only to have its size adjusted).
// The pointer returned to the user points to the free space within the block,
// which begins one quanta after the header.
//
void *stmmr_alloc(stmmr_int_t nbytes) {
  stmmr_mem_header_t *p;
  stmmr_mem_header_t *prevp;
  // Calculate how many quantas are required: we need enough to house all
  // the requested bytes, plus the header. The -1 and +1 are there to make sure
  // that if nbytes is a multiple of nquantas, we don't allocate too much
  //
  stmmr_int_t nquantas =
      (nbytes + sizeof(stmmr_mem_header_t) - 1) / sizeof(stmmr_mem_header_t) +
      1;
  // First alloc call, and no free list yet ? Use 'base' for an initial
  // denegerate block of size 0, which points to itself
  //
  if ((prevp = freep) == 0) {
    base.s.next = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.next;; prevp = p, p = p->s.next) {
    // big enough ?
    if (p->s.size >= nquantas) {
      // exactly ?
      if (p->s.size == nquantas) {
        // just eliminate this block from the free list by pointing
        // its prev's next to its next
        //
        prevp->s.next = p->s.next;
      } else// too big
      {
        p->s.size -= nquantas;
        p += p->s.size;
        p->s.size = nquantas;
      }
      freep = prevp;
      return (void *) (p + 1);
    }
    // Reached end of free list ?
    // Try to allocate the block from the pool. If that succeeds,
    // stmmr_get_mem_from_pool adds the new block to the free list and
    // it will be found in the following iterations. If the call
    // to stmmr_get_mem_from_pool doesn't succeed, we've run out of
    // memory
    //
    else if (p == freep) {
      if ((p = stmmr_get_mem_from_pool(nquantas)) == 0) {
#ifdef STMMR_DEBUG_FATAL
        printf("!! Memory allocation failed !!\n");
#endif
        return 0;
      }
    }
  }
}
// Scans the free list, starting at freep, looking the the place to insert the
// free block. This is either between two existing blocks or at the end of the
// list. In any case, if the block being freed is adjacent to either neighbor,
// the adjacent blocks are combined.
//
void stmmr_free(void *ap) {
  if (NULL == ap) { return; }
  stmmr_mem_header_t *block;
  stmmr_mem_header_t *p;
  // acquire pointer to block header
  block = ((stmmr_mem_header_t *) ap) - 1;
  // Find the correct place to place the block in (the free list is sorted by
  // address, increasing order)
  //
  for (p = freep; !(block > p && block < p->s.next); p = p->s.next) {
    // Since the free list is circular, there is one link where a
    // higher-addressed block points to a lower-addressed block.
    // This condition checks if the block should be actually
    // inserted between them
    //
    if (p >= p->s.next && (block > p || block < p->s.next)) break;
  }
  // Try to combine with the higher neighbor
  //
  if (block + block->s.size == p->s.next) {
    block->s.size += p->s.next->s.size;
    block->s.next = p->s.next->s.next;
  } else {
    block->s.next = p->s.next;
  }
  // Try to combine with the lower neighbor
  //
  if (p + p->s.size == block) {
    p->s.size += block->s.size;
    p->s.next = block->s.next;
  } else {
    p->s.next = block;
  }
  freep = p;
}
void *stmmr_realloc(void *ap, stmmr_int_t nbytes) {
  if (NULL == ap) {
    return stmmr_alloc(nbytes);
  } else if (0 == nbytes) {
    stmmr_free(ap);
    return NULL;
  }
  stmmr_mem_header_t *block;
  block = ((stmmr_mem_header_t *) ap) - 1;
  stmmr_int_t expected =
      (nbytes + sizeof(stmmr_mem_header_t) - 1) / sizeof(stmmr_mem_header_t) +
      1;
  if (expected <= block->s.size) {
    return ap;
  } else {
    void *ptrNew = stmmr_alloc(nbytes);
    stmmr_int_t originalLength =
        (block->s.size - 1) * sizeof(stmmr_mem_header_t);
    if (ptrNew) {
      memcpy(ptrNew, ap, originalLength);
      stmmr_free(ap);
    }
    return ptrNew;
  }
}
void *stmmr_calloc(stmmr_int_t num, stmmr_int_t nbytes) {
  stmmr_int_t n = num * nbytes;
  void *p = stmmr_alloc(n);
  if (NULL != p) { memset(p, 0, n); }
  return p;
}
#endif
#endif// STMMR_H