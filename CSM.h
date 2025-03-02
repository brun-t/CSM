#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef CSM_IMPLEMENTATION
typedef struct arena {
  size_t capacity;
  size_t actual_size;
  uint8_t *block;
} arena;

arena *create_arena(size_t capacity);
/**
 * * Warning: dyn_ptr needs arena allocator when arena allocator done then do
 * dyn_ptr
 */
typedef struct dyn_ptr {
} dyn_ptr;
typedef struct ptr_stack {
} ptr_stack;
#else
#endif