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

/**
 * @struct Arena
 * @brief Arena allocator struct
 * @param capacity the capacity of the arena
 * @param actual_size the size of the content allocated into arena
 * @param block the raw memory block of the arena
 */
typedef struct {
  size_t capacity;
  size_t actual_size;
  uint8_t *block;
} Arena;

/**
 * @struct Arena_ptr
 * @brief Arena ptr struct
 * @param size the size of the ptr
 * @param block the raw memory block of the ptr
 */
typedef struct {
  size_t size;
  uint8_t *block;
} Arena_ptr;
/**
 * @fn Arena *create_arena(size_t capacity)
 * @brief Arena constructor
 * @param capacity is a size_t that defines the initial capacity of the arena
 * @return a arena struct allocated in heap and already defined
 */
inline Arena *create_arena(size_t capacity);

/**
 * @fn Arena *arena_alloc(Arena *arena, size_t size)
 * @brief It gets a block from arena allocator
 * @param size is a size_t that defines the size of the requested block
 * @param arena is the arena allocator
 */
inline Arena_ptr arena_alloc(Arena *arena, size_t size);

/**
 * @fn Arena *arena_free(Arena *arena)
 * @brief Arena destructor
 * @param arena is the arena that it want to be free
 */
inline void arena_free(Arena *arena);

/**
 * @fn bool arena_realloc(Arena *arena, size_t extra_capacity)
 * @brief It aument the capacity of the Arena allocator
 * @param arena is the arena allocator
 * @param extra_capacity is the extra capacity you want to add to the arena
 * allocator
 */
inline bool arena_realloc(Arena *arena, size_t extra_capacity);

/**
 * @struct Dyn_ptr
 * @brief A Dynamic pointer, similar to unique_ptr
 * @param ptr is a void * pointer that contains the raw memory block from the
 * arena allocator
 *
 */
typedef struct {
  void *ptr;
} Dyn_ptr;

/**
 * @struct Ptr_struct
 * @param arena is the arena allocator that Ptr_stack uses
 * @param ptr_list is a dynamic array of Dyn_ptr's
 * @param length is the actual lenght of the Ptr_stack
 */
typedef struct {
  Arena *arena;
  Dyn_ptr *ptr_list;
  size_t length;
  size_t capacity;
} Ptr_stack;

/**
 * @fn Ptr_stack *create_stack(size_t capacity)
 * @brief The Ptr_stack constructor
 * @param capacity is the initial capacity of the stack
 * */
inline Ptr_stack *create_stack(size_t capacity);

/**
 * @fn Dyn_ptr *stack_new_ptr(Ptr_stack *stack)
 * @brief It creates a Dyn_ptr and it allocate it on Ptr_stack
 * @param stack the Ptr_stack
 */
inline Dyn_ptr *stack_new_ptr(Ptr_stack *stack);

/**
 * @fn void dyn_ptr_alloc(Dyn_ptr * dyn_ptr, void * data)
 * @brief It put memory into Dyn_ptr
 * @param dyn_ptr is the dyn_ptr where data is gonna be inserted
 */

inline void dyn_ptr_alloc(Ptr_stack *stack, Dyn_ptr *dyn_ptr, void *data,
                          size_t size);

/**
 * @def get_dyn_ptr_data(T, dyn_ptr)
 * @brief it transform Dyn_ptr data to a specific type and it returns it
 * @param T is the type to what Dyn_ptr data is gonna transform
 * @param dyn_ptr is the Dyn_ptr where data is gonna be accessed
 */
#define get_dyn_ptr_data(T, dyn_ptr)

/**
 * @fn void stack_free(Ptr_stack *stack)
 * @brief It free the Ptr_stack
 * @param stack is the Ptr_stack that is gonna be freed
 */
inline void stack_free(Ptr_stack *stack);

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

inline void arena_free(Arena *arena) {
  free(arena->block);
  free(arena);
}

bool arena_realloc(Arena *arena, size_t extra_capacity) {
  void *ptr = realloc(arena->block, arena->actual_size + extra_capacity);
  if (ptr == NULL)
    return false;
  arena->block = (uint8_t *)ptr;
  arena->capacity += extra_capacity;
  return true;
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

Dyn_ptr *stack_new_ptr(Ptr_stack *stack) {
  if (stack->length >= stack->capacity) {
    if (!arena_realloc(stack->arena, stack->arena->capacity * 2)) {
      return NULL;
    }
  }

  Arena_ptr arena_ptr = arena_alloc(stack->arena, sizeof(Dyn_ptr));
  if (arena_ptr.block == NULL) {
    return NULL;
  }

  Dyn_ptr *dyn_ptr = (Dyn_ptr *)arena_ptr.block;
  dyn_ptr->ptr = NULL;

  stack->ptr_list[stack->length] = *dyn_ptr;
  stack->length++;

  return &stack->ptr_list[stack->length - 1]; // Return the dyn_ptr
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
}

#undef get_dyn_ptr_data

/**
 * @def get_dyn_ptr_data(T, dyn_ptr)
 * @brief it transform Dyn_ptr data to a specific type and it returns it
 * @param T is the type to what Dyn_ptr data is gonna transform
 * @param dyn_ptr is the Dyn_ptr where data is gonna be accessed
 */
#define get_dyn_ptr_data(T, dyn_ptr) (T *)dyn_ptr->ptr

void stack_free(Ptr_stack *stack) {
  free(stack->ptr_list);
  arena_free(stack->arena);
  free(stack);
}
#endif
#endif