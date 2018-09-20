#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
//#include <hbwmalloc.h>
#include "dualpivot_sequential.h"
#include "dualpivot_tasks.h"
#include "dualpivot_sections.h"

const int NUM_TESTS  = 5;
const int TEST_CASES[] = {100000, 1000000, 10000000, 100000000, 1000000000};

int compare(const void* left, const void* right) {
  int lhs = *((int*) left);
  int rhs = *((int*) right);
  if (lhs == rhs) return 0;
  else if (lhs > rhs) return 1;
  else return -1;
}

void shuffle(int* array, size_t n) {
  srand48(time(NULL));

  if (n > 1) {
    for (size_t i = n - 1; i > 0; --i) {
      size_t j = (unsigned int) (drand48() * (i + 1));
      int temp = array[j];
      array[j] = array[i];
      array[i] = temp;
    }
  }
}

int main(int argc, char** argv) {

  printf("Using %d threads at most\n\n", omp_get_thread_limit());

  for (size_t current_test_index = 0; current_test_index != NUM_TESTS; ++current_test_index) {
    int current_test = TEST_CASES[current_test_index];
    double initial_time, final_time;
      puts("############################################################");
    printf("              Testing #%ld with %d elements\n", current_test_index + 1, current_test);
      puts("############################################################\n");

    puts("Preparing reference array...");
    int* reference_array = (int*) malloc(current_test * sizeof(int));
    if(!reference_array) exit(666);
    for (volatile int i = 0; i != current_test; ++i)
      reference_array[i] = i;

    puts("Shuffling reference array");
    shuffle(reference_array, current_test);

    int* testing_array = (int*) malloc(current_test * sizeof(int));
    //    int* sequential_array = (int*) hbw_malloc(current_test * sizeof(int));
    if (!testing_array) exit(666);

    for (int test_number = 1; test_number != 4; ++test_number) {
      printf("Copying reference array to sequential sorting #%d\n", test_number);
      for (volatile size_t i = 0; i != current_test; ++i)
        testing_array[i] = reference_array[i];

      puts("Sequential sorting... ");
      initial_time = omp_get_wtime();
      dualpivot_quicksort_sequential(testing_array, 0, current_test - 1, sizeof(int), compare);
      final_time = omp_get_wtime();
      printf("Elapsed time ordering %d elements sequentially #%d: %lf\n\n",
          current_test, test_number, final_time - initial_time);
    }

    for (int test_number = 1; test_number != 4; ++test_number) {
      printf("Copying reference array to OpenMP Tasks sorting #%d\n", test_number);
      for (volatile size_t i = 0; i != current_test; ++i)
        testing_array[i] = reference_array[i];

      puts("Sorting with OpenMP Tasks...");
      initial_time = omp_get_wtime();
      dualpivot_quicksort_tasks(testing_array, current_test, sizeof(int), compare);
      final_time = omp_get_wtime();
      printf("Elapsed time ordering %d elements with OpenMP Tasks #%d: %lf\n\n",
          current_test, test_number, final_time - initial_time);
    }

//    for (int test_number = 1; test_number != 4; ++test_number) {
//      printf("Copying reference array to OpenMP Sections sorting #%d\n", test_number);
//      for (volatile size_t i = 0; i != current_test; ++i)
//        testing_array[i] = reference_array[i];
//
//      puts("Sorting with OpenMP Sections...");
//      initial_time = omp_get_wtime();
//      dualpivot_quicksort_sections(testing_array, 0, current_test - 1, sizeof(int), compare);
//      final_time = omp_get_wtime();
//      printf("Elapsed time ordering %d elements with OpenMP Sections #%d: %lf\n\n",
//          current_test, test_number, final_time - initial_time);
//    }

    free(reference_array);
    free(testing_array);
//    hbw_free(testing_array);
  }

  return 0;
}