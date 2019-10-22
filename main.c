#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <omp.h>

#include "dual_pivot_sequential.h"
#include "dualpivot_tasks.h"
#include "order.h"
#include "shuffle.h"
#include "stats.h"

const size_t ITERATIONS_COUNT = 10;
const size_t TEST_CASES[] = {100000, 1000000}; //, 10000000, 100000000, 1_000_000_000};
const size_t NUM_TESTS = sizeof TEST_CASES / sizeof TEST_CASES[0];

enum Order int_compare(const void* left, const void* right) {
  int lhs = *((int*) left);
  int rhs = *((int*) right);
  if (lhs == rhs) return EQUAL;
  else if (lhs > rhs) return GREATER;
  else return SMALLER;
}

enum Order size_t_compare(const void* left, const void* right) {
  size_t lhs = *((size_t*) left);
  size_t rhs = *((size_t*) right);

  if (lhs == rhs) return EQUAL;
  else if (lhs > rhs) return GREATER;
  else return SMALLER;
}

void check_order(
    const void* restrict array,
    size_t length,
    size_t size,
    enum Order compare(const void*, const void*)
) {
  register const unsigned char* arr_ptr = array;

  for (size_t i = 1; i < length; ++i) {
    if (compare(arr_ptr + size * (i - 1), arr_ptr + size * i) == GREATER) {
      fputs("Array is not properly ordered!!!", stderr);
      exit(999);
    }
  }

}


int main(int argc, char* argv[argc]) {

  fprintf(stderr, "Using %d threads at most\n\n", omp_get_thread_limit());

  for (size_t current_test_index = 0; current_test_index != NUM_TESTS; ++current_test_index) {
    const size_t current_test = TEST_CASES[current_test_index];
    double initial_time = 0.0;
    double final_time = 0.0;
    stats sequential = stats_new();
    stats parallel = stats_new();

    fputs("############################################################", stderr);
    fprintf(stderr,"\t\t\t\tTesting #%ld with %ld elements\n", current_test_index + 1, current_test);
    fputs("############################################################\n", stderr);

    fputs("Preparing reference array...", stderr);
    unsigned int* reference_array = malloc(sizeof(unsigned int[current_test]));
    if (!reference_array) exit(666);
    for (unsigned int i = 0; i != current_test; ++i)
      reference_array[i] = i;

    fputs("Shuffling reference array", stderr);
    shuffle(reference_array, current_test, sizeof reference_array[0]);

    unsigned int* testing_array = malloc(sizeof(unsigned int[current_test]));
    if (!testing_array) exit(666);

    /*******************************************************************************************************************
     *                                        SEQUENTIAL SORTING                                                       *
     ******************************************************************************************************************/
    for (size_t test_number = 1; test_number != ITERATIONS_COUNT; ++test_number) {
      fprintf(stderr, "Copying reference array to sequential sorting #%ld\n", test_number);
      for (size_t i = 0; i != current_test; ++i)
        testing_array[i] = reference_array[i];

      fputs("Sequential sorting... ", stderr);
      initial_time = omp_get_wtime();
      dual_pivot_quicksort_sequential(
          testing_array,
          0,
          current_test - 1,
          sizeof testing_array[0],
          int_compare
      );
      final_time = omp_get_wtime();
      check_order(testing_array, current_test, sizeof testing_array[0], int_compare);
      stats_collect2(&sequential, final_time - initial_time);
      fprintf(stderr, "Elapsed time ordering %ld elements sequentially #%ld: %lf\n\n",
             current_test, test_number, final_time - initial_time);
    }

    /*******************************************************************************************************************
     *                                        PARALLEL SORTING                                                         *
     ******************************************************************************************************************/
    for (size_t test_number = 1; test_number != ITERATIONS_COUNT; ++test_number) {
      fprintf(stderr, "Copying reference array to OpenMP Tasks sorting #%ld\n", test_number);
      for (size_t i = 0; i != current_test; ++i)
        testing_array[i] = reference_array[i];

      fputs("Sorting with OpenMP Tasks...", stderr);
      initial_time = omp_get_wtime();
      dual_pivot_quicksort_tasks(testing_array, current_test, sizeof testing_array[0], int_compare);
      final_time = omp_get_wtime();
      check_order(testing_array, current_test, sizeof testing_array[0], int_compare);
      stats_collect2(&parallel, final_time - initial_time);
      fprintf(stderr, "Elapsed time ordering %ld elements with OpenMP Tasks #%ld: %lf\n\n",
             current_test, test_number, final_time - initial_time);
    }

    double mean = 0.0;
    mean = stats_mean(&sequential);
    printf("Sequential: E(t) (sec):\t%5.2e ± %4.02f%%,\taverage time per element %5.2e\n",
           mean, 100.0 * stats_rsdev_unbiased(&sequential), mean / current_test);

    mean = stats_mean(&parallel);
    printf("Parallel: E(t) (sec):\t%5.2e ± %4.02f%%,\taverage time per element %5.2e\n",
           mean, 100.0 * stats_rsdev_unbiased(&sequential), mean / current_test);


    free(reference_array);
    free(testing_array);
  }

  return EXIT_SUCCESS;
}