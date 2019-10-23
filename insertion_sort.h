#ifndef QUICKSORTMP_INSERTION_SORT_H
#define QUICKSORTMP_INSERTION_SORT_H

#include <stddef.h>
#include <string.h>

#include "order.h"
#include "swap.h"

#define INSERTION_SORT_THRESHOLD 100

void insertion_sort(
    void* restrict array,
    size_t length,
    size_t size,
    enum Order compare(const void*, const void*)
) {
  register unsigned char* arr_ptr = array;
  unsigned char temp[size];

  for (register size_t index = 1; index < length; ++index) {
    memcpy(temp, arr_ptr + size * index, size);
    register ptrdiff_t correct_index = index - 1;

    while (correct_index >= 0 && compare(arr_ptr + size * correct_index, temp) == GREATER) {
      memcpy(arr_ptr + size * (correct_index + 1), arr_ptr + size * correct_index, size);
      --correct_index;
    }
    memcpy(arr_ptr + size * (correct_index + 1), temp, size);
  }
}

#endif //QUICKSORTMP_INSERTION_SORT_H
