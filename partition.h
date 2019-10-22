#ifndef QUICKSORTMP_PARTITION_H
#define QUICKSORTMP_PARTITION_H

#include <stdlib.h>
#include <stddef.h>

#include "order.h"
#include "swap.h"

inline
void partition(
    void* restrict array,
    ptrdiff_t left_pivot_index,
    ptrdiff_t right_pivot_index,
    ptrdiff_t* restrict returned_left_pivot,
    ptrdiff_t* restrict returned_right_pivot,
    size_t size,
    enum Order compare(const void*, const void*)
) {

  register unsigned char* arr_ptr = array;

  // Swapping pivots if left one is bigger than right one
  if (compare(arr_ptr + size * left_pivot_index, arr_ptr + size * right_pivot_index) == GREATER) {
    SWAP(arr_ptr + size * left_pivot_index, arr_ptr + size * right_pivot_index, size);
  }

  register ptrdiff_t left_index = left_pivot_index + 1;
  register ptrdiff_t right_index = right_pivot_index - 1;
  register ptrdiff_t current_index = left_index;
  register unsigned char* left_pivot = arr_ptr + size * left_pivot_index;
  register unsigned char* right_pivot = arr_ptr + size * right_pivot_index;

  while (current_index <= right_index) {
    if (compare(arr_ptr + size * current_index, left_pivot) == SMALLER) {
      SWAP(arr_ptr + size * current_index, arr_ptr + size * left_index++, size);
    } else {
      register enum Order compared = compare(arr_ptr + size * current_index, right_pivot);
      if (compared == EQUAL || compared == GREATER) {
        while (
            compare(arr_ptr + size * right_index, right_pivot) == GREATER &&
            current_index < right_index
            ) {
          --right_index;
        }

        SWAP(arr_ptr + size * current_index, arr_ptr + size * right_index, size);

        --right_index;

        if (compare(arr_ptr + size * current_index, left_pivot) == SMALLER) {
          SWAP((arr_ptr + size * current_index), (arr_ptr + size * left_index), size);
          ++left_index;
        }
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
