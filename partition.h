#ifndef QUICKSORTMP_PARTITION_H
#define QUICKSORTMP_PARTITION_H

#include <stdlib.h>

#define SWAP(lhs, rhs, size)                                                         \
    do                                                                               \
      {                                                                              \
        size_t __size = (size);                                                      \
        char *__lhs = (lhs);                                                         \
        char *__rhs = (rhs);                                                         \
        do                                                                           \
          {                                                                          \
            char __tmp = *__lhs;                                                     \
            *__lhs++ = *__rhs;                                                       \
            *__rhs++ = __tmp;                                                        \
          } while (--__size > 0);                                                    \
      } while (0)

void partition(
    const void* array,
    long long left_pivot_index,
    long long right_pivot_index,
    long long* returned_left_pivot,
    long long* returned_right_pivot,
    size_t size,
    int (*compare)(const void*, const void*))
{
  char* arr_ptr = (char*) array;
  
  // Swapping pivots if left one is bigger than right one
  if (compare((void*)(arr_ptr + size * left_pivot_index), (void*)(arr_ptr + size * right_pivot_index)) > 0)
    SWAP((arr_ptr + size * left_pivot_index), (arr_ptr + size * right_pivot_index), size);
  
  long long left_index = left_pivot_index + 1;
  long long right_index = right_pivot_index - 1;
  long long current_index = left_index;
  char* left_pivot = (arr_ptr + size * left_pivot_index);
  char* right_pivot = (arr_ptr + size * right_pivot_index);
  
  while (current_index <= right_index) {
    
    if (compare((void*) (arr_ptr + size * current_index), (void*) left_pivot) < 0) {
      SWAP((arr_ptr + size * current_index), (arr_ptr + size * left_index++), size);
    }
    else if (compare((void*) (arr_ptr + size * current_index), (void*) right_pivot) >= 0) {
      while (compare((void*) (arr_ptr + size * right_index), (void*) right_pivot) > 0
             && current_index < right_index) {
        --right_index;
      }
      SWAP((arr_ptr + size * current_index), (arr_ptr + size * right_index), size);
      --right_index;
      if (compare((void*) (arr_ptr + size * current_index), (void*) left_pivot) < 0) {
        SWAP((arr_ptr + size * current_index), (arr_ptr + size * left_index), size);
        ++left_index;
      }
    }
    ++current_index;
  }
  --left_index;
  ++right_index;
  
  SWAP(left_pivot, (arr_ptr + size * left_index), size);
  SWAP(right_pivot, (arr_ptr + size * right_index), size);
  
  *returned_left_pivot = left_index;
  *returned_right_pivot = right_index;
}

#endif //QUICKSORTMP_PARTITION_H
