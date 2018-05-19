#ifndef QUICKSORTMP_DUALPIVOT_TASKS_H
#define QUICKSORTMP_DUALPIVOT_TASKS_H

#include <omp.h>
#include "partition.h"

void sort(
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
    
    #pragma omp task default(none) firstprivate(array, lower_index, left_pivot, size, compare)
    {
      sort(array, lower_index, left_pivot - 1, size, compare);
    }
    #pragma omp task default(none) firstprivate(array, left_pivot, right_pivot, size, compare)
    {
      sort(array, left_pivot + 1, right_pivot - 1, size, compare);
    }
    #pragma omp task default(none) firstprivate(array, right_pivot, higher_index, size, compare)
    {
      sort(array, right_pivot + 1, higher_index, size, compare);
    }
  }
}

void dualpivot_quicksort_tasks(
    const void* array,
    size_t n_elements,
    size_t size,
    int (* compare)(const void*, const void*))
{
  #pragma omp parallel default(none) shared(array, n_elements, size, compare)
  {
    #pragma omp single nowait
    {
      sort(array, 0, n_elements - 1, size, compare);
    }
  }
}

#endif //QUICKSORTMP_DUALPIVOT_TASKS_H
