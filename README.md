# Static Memory Allocator

Based on public domain code by `Eli Bendersky (eliben at gmail.com)`.

### Why?
Pretend to use heap memory while using a stack allocated block of memory.


### Tiny modifications added.

1. `stmmr_free` does not continue if pointer is `NULL`.
2. Uses `stdint.h` for unsigned data types.
3. Added `stmmr_realloc(p, size)`.
4. Added `stmmr_calloc(num, size)`.
5. Added data type `stmmr_int_t`. 
6. Single header implementation. `#define STMMR_IMPL` in one `C` file before including `stmmr.h`.