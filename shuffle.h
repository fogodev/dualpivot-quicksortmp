#ifndef QUICKSORTMP_SHUFFLE_H
#define QUICKSORTMP_SHUFFLE_H

#include <stdlib.h>
#include <time.h>

#include "swap.h"


void shuffle(void* restrict array, size_t length, size_t size) {
  if (length > 1) {
    srand48(time(NULL));
    register unsigned char* arr_ptr = array;

    for (size_t i = length - 1; i > 0; --i) {
      SWAP(arr_ptr + size * i, arr_ptr + size * (size_t) (drand48() * (double) (i + 1)), size);
    }
  }
}


#endif //QUICKSORTMP_SHUFFLE_H
