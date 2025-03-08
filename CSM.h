/**
 * @file CSM.h
 * @brief A STB style file for better memory manipulation in C99 but that
 * continue giving fine grained control over memory
 */
#ifndef CSM_GUARD
#define CSM_GUARD

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**\defgroup arena Arena Allocator struct and functions */
/**\defgroup ptr_stack Ptr_stack struct and functions*/
/**\defgroup dyn_ptr Dyn_ptr struct and functions */

#ifndef CSM_API
#define CSM_API static inline
#endif

/**
 * @ingroup arena
 * @struct Arena
 * @brief It's a struct that defines a Arena allocator
 * @param capacity the capacity of the arena
 * @param actual_size the size of the content allocated into arena
 * @param block the raw memory block of the arena
 */
typedef struct {
  size_t capacity; /**< is the maximum amount of "blocks" that Arena can hold*/
  size_t actual_size; /**< is the quantity of blocks into Arena in this moment */
  uint8_t *block; /**< is the raw memory block into Arena */
} Arena;

/**
 * @ingroup arena
 * @struct Arena_ptr
 * @brief Is a pointer into Arena allocator that includes it own size and a the block of memory
 * @param size the size of the ptr
 * @param block the raw memory block of the ptr
 */
typedef struct {
  size_t size; /**< is the length of the block */
  uint8_t *block; /**< is the memory itself */
} Arena_ptr;
/**
 * @ingroup arena
 * @fn Arena *create_arena(size_t capacity)
 * @brief It creates a arena allocator
 * @param capacity is a size_t that defines the initial capacity of the arena
 * @return a arena struct allocated in heap and already defined
 */
CSM_API Arena *create_arena(size_t capacity);

/**
 * @ingroup arena
 * @fn Arena *arena_alloc(Arena *arena, size_t size)
 * @brief It gets a block from arena allocator
 * @param size is a size_t that defines the size of the requested block
 * @param arena is the arena allocator
 */
CSM_API Arena_ptr arena_alloc(Arena *arena, size_t size);

/**
 * @ingroup arena
 * @fn Arena *arena_free(Arena *arena)
 * @brief It free all memory withing the arena
 * @param arena is the arena that it want to be free
 */
CSM_API void arena_free(Arena *arena);

/** 
 * @ingroup arena
 * @fn bool arena_realloc(Arena *arena, size_t extra_capacity)
 * @brief It increment the capacity of the Arena allocator
 * @param arena is the arena allocator
 * @param extra_capacity is the extra capacity you want to add to the arena
 * allocator
 */
CSM_API bool arena_realloc(Arena *arena, size_t extra_capacity);

/**
 * @ingroup dyn_ptr
 * @struct Dyn_ptr
 * @brief A Dynamic pointer, it free memory automatically
 * @param ptr is a void * pointer that contains the raw memory block from the arena allocator
 * @param size is the size of the memory into Dyn_ptr
 */

typedef struct Dyn_ptr {
  void *ptr; /**< is the raw memory that Dyn_ptr holds*/
  size_t size; /**< is the size of Dyn_ptr::ptr */
  void (*dealloc)(struct Dyn_ptr *); /**< is a function ptr that have the deallocator for the data, NOTE:this is just optional */
} Dyn_ptr;

/**
 * @ingroup ptr_stack
 * @brief it's a dynamic list that manage all Dyn_ptr's
 * @struct Ptr_stack
 * 
 * @param arena is the arena allocator that Ptr_stack uses
 * @param ptr_list is a dynamic array of Dyn_ptr's
 * @param length is the actual length of the Ptr_stack
 */
typedef struct {
  Arena *arena; /**< arena is the arena allocator used for Ptr_stack */
  Dyn_ptr *ptr_list; /**< ptr_list is the dynamic list that holds all Dyn_ptr's from Ptr_stack */
  size_t length; /**< is the number of Dyn_ptr's that is into Ptr_stack */
  size_t capacity; /**< is the quantity of how many Dyn_ptr's Ptr_stack can hold */
} Ptr_stack;

/**
 * @ingroup dyn_ptr
 *
 * @fn void dyn_ptr_alloc(Ptr_stack *stack, Dyn_ptr *dyn_ptr, void *data, size_t size)
 * @brief It put memory into Dyn_ptr
 * @param dyn_ptr is the dyn_ptr where data is gonna be inserted
 * @param stack is the stack where the pointer is into
 * @param data is the data that is gonna be inserted
 * @param size is the length of the data that is gonna be inserted
 */

CSM_API void dyn_ptr_alloc(Ptr_stack *stack, Dyn_ptr *dyn_ptr, void *data, size_t size);

/**
 * @ingroup dyn_ptr
 * @brief It insert the new deallocator
 * @param dyn_ptr is the dyn_ptr where the new deallocator is gonna be inserted
 * @param dealloc is the deallocator that is gonna be inserted
 */

CSM_API void dyn_ptr_insert_deallocator(Dyn_ptr *dyn_ptr, void (*dealloc)(Dyn_ptr *));

/**
 * @ingroup dyn_ptr
 * @brief Is just the placeholder for the deallocator for dyn_ptr
 */
CSM_API void null_deallocator(Dyn_ptr *_);

/**
 * @ingroup dyn_ptr
 
 * @def get_dyn_ptr_data(T, dyn_ptr)
 * @brief it transform Dyn_ptr data to a specific type and it returns it
 * @param T is the type to what Dyn_ptr data is gonna transform
 * @param dyn_ptr is the Dyn_ptr where data is gonna be accessed
 */
#define get_dyn_ptr_data(T, dyn_ptr)

/**
 * @ingroup ptr_stack
 * @fn Ptr_stack *create_stack(size_t capacity)
 * @brief It creates a new Ptr_stack instance that is ready for use
 * @param capacity is the initial capacity of the stack
 * */
CSM_API Ptr_stack *create_stack(size_t capacity);

/**
 * @addtogroup ptr_stack
 * @addtogroup dyn_ptr
  
 * @fn Dyn_ptr *stack_new_ptr(Ptr_stack *stack, void *data, size_t dataSize)
 * @brief It creates a Dyn_ptr and it allocate it on Ptr_stack
 * @param stack the Ptr_stack
 * @param dataSize the size of the data that is gonna be inserted into the ptr
 * @param data is the data that is gonna be inserted into ptr
 */
CSM_API Dyn_ptr *stack_new_ptr(Ptr_stack *stack, void *data, size_t dataSize);

/**
 * @ingroup ptr_stack
  
 * @fn void stack_free(Ptr_stack *stack)
 * @brief It free the Ptr_stack
 * @param stack is the Ptr_stack that is gonna be freed
 */
CSM_API void stack_free(Ptr_stack *stack);

#ifdef CSM_IMPLEMENTATION
Arena *create_arena(size_t capacity) {
  Arena *arena = (Arena *)malloc(sizeof(Arena));

  if (arena == NULL) {
    return NULL;
  }

  arena->capacity = capacity;
  arena->actual_size = 0;
  arena->block = (uint8_t *)malloc(arena->capacity);

  if (arena->block == NULL) {
    free(arena);
    return NULL;
  }

  return arena;
}

Arena_ptr arena_alloc(Arena *arena, size_t size) {
  if (arena == NULL || size == 0)
    return (Arena_ptr){.size = 0, .block = NULL};

  if (arena->actual_size + size > arena->capacity)
    return (Arena_ptr){.size = 0, .block = NULL};

  uint8_t *block = &arena->block[arena->actual_size];
  arena->actual_size += size;

  return (Arena_ptr){.size = size, .block = block};
}

bool arena_realloc(Arena *arena, size_t extra_capacity) {
  void *ptr = realloc(arena->block, arena->actual_size + extra_capacity);
  if (ptr == NULL)
    return false;
  arena->block = (uint8_t *)ptr;
  arena->capacity += extra_capacity;
  return true;
}

void arena_free(Arena *arena) {
  free(arena->block);
  free(arena);
}

Ptr_stack *create_stack(size_t capacity) {
  Ptr_stack *ptr_stack = (Ptr_stack *)malloc(sizeof(Ptr_stack));
  if (ptr_stack == NULL)
    return NULL;

  Dyn_ptr *dyn_ptrs = (Dyn_ptr *)malloc(capacity * sizeof(Dyn_ptr));
  if (dyn_ptrs == NULL) {
    free(ptr_stack);
    return NULL;
  }

  ptr_stack->capacity = capacity; // Initialize capacity
  Arena *arena = create_arena(ptr_stack->capacity);
  if (arena == NULL) {
    free(dyn_ptrs);
    free(ptr_stack);
    return NULL;
  }

  ptr_stack->length = 0;
  ptr_stack->ptr_list = dyn_ptrs;
  ptr_stack->arena = arena;

  return ptr_stack;
}

Dyn_ptr *stack_new_ptr(Ptr_stack *stack, void *data, size_t dataSize) {
  size_t growth = dataSize * (size_t)2;
  if (growth < 1024) { // minimum of 1KB growth.
    growth = 1024;
  }
  if (stack->length >= stack->capacity && !arena_realloc(stack->arena, growth)) {
    return NULL;
  }

  Arena_ptr arena_ptr = arena_alloc(stack->arena, sizeof(Dyn_ptr));
  if (arena_ptr.block == NULL) {
    return NULL;
  }

  Dyn_ptr *dyn_ptr = (Dyn_ptr *)arena_ptr.block;
  dyn_ptr->ptr = NULL;

  stack->ptr_list[stack->length] = *dyn_ptr;
  stack->length++;

  dyn_ptr = &stack->ptr_list[stack->length - 1];
  dyn_ptr->size = dataSize;
  dyn_ptr->dealloc = null_deallocator;
  dyn_ptr_alloc(stack, dyn_ptr, data, dataSize);
  if (dyn_ptr->ptr == NULL)
    return NULL;
  return dyn_ptr;
}

void dyn_ptr_alloc(Ptr_stack *stack, Dyn_ptr *dyn_ptr, void *data,
                   size_t size) {
  if (stack == NULL || dyn_ptr == NULL || data == NULL || size == 0) {
    return;
  }

  Arena_ptr data_arena_ptr = arena_alloc(stack->arena, size);

  if (data_arena_ptr.block == NULL) {
    return;
  }

  memcpy(data_arena_ptr.block, data, size);

  dyn_ptr->ptr = data_arena_ptr.block;
  dyn_ptr->size = size; /// ? It should be size or data_arena_ptr.size
}

void dyn_ptr_insert_deallocator(Dyn_ptr *dyn_ptr, void (*dealloc)(Dyn_ptr *)) {
  dyn_ptr->dealloc = dealloc;
}

void null_deallocator(Dyn_ptr *_){};

#undef get_dyn_ptr_data

/**
 * @def get_dyn_ptr_data(T, dyn_ptr)
 * @brief it transform Dyn_ptr data to a specific type and it returns it
 * @param T is the type to what Dyn_ptr data is gonna transform
 * @param dyn_ptr is the Dyn_ptr where data is gonna be accessed
 */
#define get_dyn_ptr_data(T, dyn_ptr) (T *)(dyn_ptr->ptr)

void stack_free(Ptr_stack *stack) {
  for (size_t i = 0; i < stack->length; i++) {
    stack->ptr_list[i].dealloc(&stack->ptr_list[i]);
  }

  free(stack->ptr_list);
  arena_free(stack->arena);
  free(stack);
}
#endif
#endif