#ifndef QUICKSORTMP_DUALPIVOT_TASKS_H
#define QUICKSORTMP_DUALPIVOT_TASKS_H

#include <omp.h>
#include <stddef.h>
#include "partition.h"
#include "order.h"

void sort_parallel(
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

    #pragma omp task default(none) firstprivate(array, lower_index, left_pivot, size, compare)
    {
      sort_parallel(array, lower_index, left_pivot - 1, size, compare);
    }
    #pragma omp task default(none) firstprivate(array, left_pivot, right_pivot, size, compare)
    {
      sort_parallel(array, left_pivot + 1, right_pivot - 1, size, compare);
    }
    #pragma omp task default(none) firstprivate(array, right_pivot, higher_index, size, compare)
    {
      sort_parallel(array, right_pivot + 1, higher_index, size, compare);
    }
  }
}

void dual_pivot_quicksort_tasks(
    void* restrict array,
    size_t n_elements,
    size_t size,
    enum Order compare(const void*, const void*)
) {
  #pragma omp parallel default(none) shared(array, n_elements, size, compare)
  {
    #pragma omp single nowait
    {
      sort_parallel(array, 0, n_elements - 1, size, compare);
    }
  }
}

#endif //QUICKSORTMP_DUALPIVOT_TASKS_H