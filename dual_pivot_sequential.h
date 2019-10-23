#ifndef QUICKSORTMP_DUAL_PIVOT_SEQUENTIAL_H
#define QUICKSORTMP_DUAL_PIVOT_SEQUENTIAL_H

#include <stddef.h>

#include "partition.h"
#include "order.h"

static
void sort(
    void* restrict array,
    ptrdiff_t lower_index,
    ptrdiff_t higher_index,
    size_t size,
    enum Order compare(const void*, const void*)
) {

  if (lower_index < higher_index) {
    ptrdiff_t left_pivot = 0;
    ptrdiff_t right_pivot = 0;

    partition(array, lower_index, higher_index, &left_pivot, &right_pivot, size, compare);

    sort(array, lower_index, left_pivot - 1, size, compare);
    sort(array, left_pivot + 1, right_pivot - 1, size, compare);
    sort(array, right_pivot + 1, higher_index, size, compare);
  }
}

void dual_pivot_quicksort_sequential(
    void* restrict array,
    size_t length,
    size_t size,
    enum Order compare(const void*, const void*)
) {
  sort(array, 0, length - 1, size, compare);
}

#endif //QUICKSORTMP_DUAL_PIVOT_SEQUENTIAL_H
