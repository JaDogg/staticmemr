# Static Memory Allocator

Based on public domain code by `Eli Bendersky (eliben at gmail.com)`.


### Tiny modifications added.

1. `memmgr_free` does not continue if pointer is `NULL`.
2. Uses `stdint.h` for unsigned data types.
3. Added `memmgr_realloc(p, size)`.
4. Added `memmgr_calloc(num, size)`.
5. Added data type `memmgr_int_t`."# staticmemr" 
