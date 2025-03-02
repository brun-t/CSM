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
Arena *create_arena(size_t capacity);

/**
 * @fn Arena *arena_alloc(Arena *arena, size_t size)
 * @brief It gets a block from arena allocator
 * @param size is a size_t that defines the size of the requested block
 * @param arena is the arena allocator
 * @return a memory block from arena allocator
 */
Arena_ptr arena_alloc(Arena *arena, size_t size);

/**
 * @fn Arena *arena_free(Arena *arena)
 * @brief Arena destructor
 * @param Arena is the arena that it want to be free
 */
void arena_free(Arena *arena);

bool arena_realloc(Arena *arena, size_t extra_capacity);

/**
 * @brief A Dynamic pointer, similar to unique_ptr

typedef struct Dyn_ptr {
} Dyn_ptr;
typedef struct Ptr_stack {
} Ptr_stack;
*/

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

void arena_free(Arena *arena) {
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

#endif
#endif