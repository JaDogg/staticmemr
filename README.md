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


# License
> This is free and unencumbered software released into the public domain.
> 
> Anyone is free to copy, modify, publish, use, compile, sell, or
> distribute this software, either in source code form or as a compiled
> binary, for any purpose, commercial or non-commercial, and by any
> means.
> 
> In jurisdictions that recognize copyright laws, the author or authors
> of this software dedicate any and all copyright interest in the
> software to the public domain. We make this dedication for the benefit
> of the public at large and to the detriment of our heirs and
> successors. We intend this dedication to be an overt act of
> relinquishment in perpetuity of all present and future rights to this
> software under copyright law.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
> EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
> MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
> IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
> OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
> ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
> OTHER DEALINGS IN THE SOFTWARE.
> 
> For more information, please refer to <https://unlicense.org>