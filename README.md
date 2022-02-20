# Stack Memory Allocator
Based on public domain code by `Eli Bendersky (eliben at gmail.com)`.

Changes added By - Bhathiya Perera AKA JaDogg. 🌞

### Why?
Pretend to use heap memory while using a stack allocated fixed size (during compile time) block of memory.

### Usage
1. Rename `malloc` etc to functions provided here using something like `#define malloc stmmr_malloc`.
2. For libraries you use override appropriate `#define`s and point them to `stmmr_xxx` functions. 
3. Do not mix with any other allocator's allocated memory.
4. Available methods. `stmmr_malloc`, `stmmr_free`, `stmmr_realloc`, `stmmr_calloc`.


### Tiny modifications added.
1. `stmmr_free` does not continue if pointer is `NULL`.
2. Uses `stdint.h` for unsigned data types. This is now therefore need `C99`.
3. Added `stmmr_realloc(p, size)`.
  * This uses the hidden header allocated behind the pointer to get the size of current memory block. 
5. Added `stmmr_calloc(num, size)`.
6. Added data type `stmmr_int_t`. 
7. Single header implementation. `#define STMMR_IMPL` in one `C` file before including `stmmr.h`. For other files just include `stmmr.h`


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
