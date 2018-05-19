#ifndef QUICKSORTMP_DUALPIVOT_SECTIONS_H
#define QUICKSORTMP_DUALPIVOT_SECTIONS_H

#include <omp.h>
#include "partition.h"

void dualpivot_quicksort_sections(
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
    
    omp_set_max_active_levels(8);
    omp_set_nested(1);
    #pragma omp parallel sections
    {
      #pragma omp section
      dualpivot_quicksort_sections(array, lower_index, left_pivot - 1, size, compare);
  
      #pragma omp section
      dualpivot_quicksort_sections(array, left_pivot + 1, right_pivot - 1, size, compare);
  
      #pragma omp section
      dualpivot_quicksort_sections(array, right_pivot + 1, higher_index, size, compare);
    }
    
  }
}

#endif //QUICKSORTMP_DUALPIVOT_SECTIONS_H
