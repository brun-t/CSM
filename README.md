<h1 style="text-align:center;"> CSM. C99. Safe. Memory </h1>

CSM is a lib that make easier control memory in c but continue giving fine grained control.

CSM use STB style so you just copy the header "CSM.h" and put it and done.

## Features

- it comes with a arena allocator included for faster and robust memory
- it comes with automatic allocating and it just free all used memory after execution
- it allow user to add custom dealloctors(Think it like c++ destructors) in Dyn_ptrs

## In work features

- Countermeasures against Buffer Overflow(this problem is begin solved just now)
- Add a "Allocator" interface for let user make it's own allocator if needed

## A tiny example

```c
#define CSM_IMPLEMENTATION // define this just in ONE of your .c files
#include "CSM.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  Ptr_stack *st = create_stack(1024 * 1024); // create the stack where ptr are gonna be

  char *str = "Hello";
  Dyn_ptr *string_ptr = stack_new_ptr(st, str, strlen(str) + 1); // insert string into a ptr

  printf("pointer:%p, string:%s\n", string_ptr->ptr,
         get_dyn_ptr_data(char, string_ptr)); // show the raw pointer address and the data into it

  stack_free(st); // free the ptr stack

  return 0;
}
```
