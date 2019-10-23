#ifndef QUICKSORTMP_DUALPIVOT_TASKS_ADAPTIVE_H
#define QUICKSORTMP_DUALPIVOT_TASKS_ADAPTIVE_H

#include <omp.h>
#include <stddef.h>
#include "partition.h"
#include "order.h"
#include "insertion_sort.h"


static
void sort_parallel_adaptive(
    void* restrict array,
    ptrdiff_t lower_index,
    ptrdiff_t higher_index,
    size_t size,
    enum Order compare(const void*, const void*)
) {

  if (lower_index < higher_index) {
    size_t insertion_length = 0;
    if ((insertion_length = higher_index - lower_index) < INSERTION_SORT_THRESHOLD) {
      register unsigned char* arr_ptr = array;
      insertion_sort(arr_ptr + size * lower_index, insertion_length + 1, size, compare);
    } else {
      ptrdiff_t left_pivot = 0;
      ptrdiff_t right_pivot = 0;

      partition(array, lower_index, higher_index, &left_pivot, &right_pivot, size, compare);

      if ((insertion_length = left_pivot - lower_index) > INSERTION_SORT_THRESHOLD) {
        #pragma omp task default(none) firstprivate(array, lower_index, left_pivot, size, compare)
        {
          sort_parallel_adaptive(array, lower_index, left_pivot - 1, size, compare);
        }
      } else {
        register unsigned char* arr_ptr = array;
        insertion_sort(arr_ptr + size * lower_index, insertion_length + 1, size, compare);
      }

      if ((insertion_length = right_pivot - left_pivot) > INSERTION_SORT_THRESHOLD) {
        #pragma omp task default(none) firstprivate(array, left_pivot, right_pivot, size, compare)
        {
          sort_parallel_adaptive(array, left_pivot + 1, right_pivot - 1, size, compare);
        }
      } else {
        register unsigned char* arr_ptr = array;
        insertion_sort(arr_ptr + size * left_pivot, insertion_length + 1, size, compare);
      }

      if ((insertion_length = higher_index - right_pivot) > INSERTION_SORT_THRESHOLD) {
        #pragma omp task default(none) firstprivate(array, right_pivot, higher_index, size, compare)
        {
          sort_parallel_adaptive(array, right_pivot + 1, higher_index, size, compare);
        }
      } else {
        register unsigned char* arr_ptr = array;
        insertion_sort(arr_ptr + size * right_pivot, insertion_length + 1, size, compare);
      }
    }
  }
}

void dual_pivot_quicksort_tasks_adaptive(
    void* restrict array,
    size_t length,
    size_t size,
    enum Order compare(const void*, const void*)
) {
  if (length > 10000){
    #pragma omp parallel default(none) shared(array, length, size, compare)
    {
      #pragma omp single nowait
      {
        sort_parallel_adaptive(array, 0, length - 1, size, compare);
      }
    }
  } else {
    insertion_sort(array, length, size, compare);
  }

}

#endif //QUICKSORTMP_DUALPIVOT_TASKS_ADAPTIVE_H
