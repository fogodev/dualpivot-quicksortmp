#ifndef QUICKSORTMP_DUALPIVOT_SEQUENTIAL_H
#define QUICKSORTMP_DUALPIVOT_SEQUENTIAL_H

#include "partition.h"

void dualpivot_quicksort_sequential(
    const void* array,
    long long lower_index,
    long long higher_index,
    size_t size,
    int (* compare)(const void*, const void*))
{
  if (lower_index < higher_index) {
    long long left_pivot = 0;
    long long right_pivot = 0;
    
    partition(array, lower_index, higher_index, &left_pivot, &right_pivot, size, compare);
    dualpivot_quicksort_sequential(array, lower_index, left_pivot - 1, size, compare);
    dualpivot_quicksort_sequential(array, left_pivot + 1, right_pivot - 1, size, compare);
    dualpivot_quicksort_sequential(array, right_pivot + 1, higher_index, size, compare);
  }
}

#endif //QUICKSORTMP_DUALPIVOT_SEQUENTIAL_H
