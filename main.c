#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <omp.h>

#include "dual_pivot_sequential.h"
#include "dual_pivot_sequential_adaptive.h"
#include "dualpivot_tasks_adaptive.h"
#include "order.h"
#include "shuffle.h"
#include "stats.h"
#include "dual_pivot_tasks.h"

const size_t ITERATIONS_COUNT = 10;
const size_t TEST_CASES[] = {
    100000,      //       100_000
    131072,     //         2 ^ 17
    262144,     //         2 ^ 18
    524288,     //         2 ^ 19
    1000000,     //     1_000_000
    1048576,     //        2 ^ 20
    2097152,     //        2 ^ 21
    4194304,     //        2 ^ 22
    8388608,     //        2 ^ 23
    10000000,    //    10_000_000
    16777216,    //        2 ^ 24
    33554432,    //        2 ^ 25
    67108864,    //        2 ^ 26
    100000000,   //   100_000_000
    134217728,   //        2 ^ 27
    268435456,   //        2 ^ 28
    536870912,   //        2 ^ 29
    1000000000,  // 1_000_000_000
    1073741824,  //        2 ^ 30
    2147483648,  //        2 ^ 31
    4294967296   //        2 ^ 32
};
const size_t NUM_TESTS = sizeof TEST_CASES / sizeof TEST_CASES[0];

enum Order int_compare(const void* left, const void* right) {
  int lhs = *((int*) left);
  int rhs = *((int*) right);
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
      fputs("Array is not properly ordered!!!\n", stderr);
      exit(999);
    }
  }
}

void benchmark(
    const void* restrict reference_array,
    void* restrict testing_array,
    size_t length,
    size_t size,
    enum Order compare(const void*, const void*),
    void sort_function(void* restrict, size_t, size_t, enum Order(const void*, const void*)),
    stats* statistics,
    size_t num_threads
) {
  // Preparing testing data (length * size) bytes of data
  memcpy(testing_array, reference_array, length * size);

  // Setting number of threads to OpenMP
  omp_set_num_threads(num_threads);

  // Benchmarking
  const double initial_time = omp_get_wtime();
  sort_function(testing_array, length, size, compare);
  const double final_time = omp_get_wtime();

  // Checking sort correctness
  check_order(testing_array, length, size, compare);

  // Collecting statistics
  stats_collect2(statistics, final_time - initial_time);
}

void stats_report(FILE* csv_file, stats* statistics, size_t elements_count, const char* name) {
  double mean = stats_mean(statistics);
  double std_deviation = 100.0 * stats_rsdev_unbiased(statistics);
  double avg_time_per_element = mean / elements_count;
  size_t num_threads = omp_get_max_threads();
  printf(
      "%s:\tElements: %ld\tE(T) (sec):\t%5.2e ± %4.02f%%,\tAverage time per element: %5.2e\t Number of threads: %ld\n",
      name,
      elements_count,
      mean,
      std_deviation,
      avg_time_per_element,
      num_threads
  );
  fprintf(
      csv_file,
      "%s,%ld,%5.2e,%4.02f,%5.2e,%ld\n",
      name,
      elements_count,
      mean,
      std_deviation,
      avg_time_per_element,
      num_threads
  );
}

FILE* prepare_csv_file(const char filename[]) {
  FILE* csv_file = fopen(filename, "w");
  if (!csv_file) {
    fprintf(stderr, "Could't create csv file");
    exit(666);
  }
  fputs(
      "Sort Name,Elements,E(T) (sec),± std deviation (%),Average time per element (sec),Number of threads\n",
      csv_file
  );

  return csv_file;
}

unsigned* prepare_reference_array(size_t current_test) {
  puts("Preparing reference array...");
  unsigned* reference_array = malloc(sizeof(unsigned[current_test]));
  if (!reference_array) exit(666);

  for (unsigned i = 0; i != current_test; ++i)
    reference_array[i] = i;

  puts("Shuffling reference array...");
  shuffle(reference_array, current_test, sizeof reference_array[0]);

  return reference_array;
}

unsigned* prepare_testing_array(size_t current_test) {
  unsigned* testing_array = malloc(sizeof(unsigned[current_test]));
  if (!testing_array) exit(666);

  return testing_array;
}

void benchmark_and_report(
    FILE* csv_file,
    const void* restrict reference_array,
    void* restrict testing_array,
    size_t size,
    size_t current_test,
    const char name[],
    size_t num_threads,
    enum Order compare(const void*, const void*),
    void sort_function(void* restrict, size_t, size_t, enum Order(const void*, const void*))
) {
  stats statistics = stats_new();
  printf("Sorting %lu elements with %s and %ld threads\n", current_test, name, num_threads);
  for (size_t test_number = 0; test_number < ITERATIONS_COUNT; ++test_number) {
    benchmark(
        reference_array,
        testing_array,
        current_test,
        size,
        compare,
        sort_function,
        &statistics,
        num_threads
    );
  }
  stats_report(csv_file, &statistics, current_test, name);
}

void run_benchmarks() {
  // Preparing csv file
  FILE* csv_file = prepare_csv_file("results.csv");
  for (size_t current_test_index = 0; current_test_index != NUM_TESTS; ++current_test_index) {
    const size_t current_test = TEST_CASES[current_test_index];

    puts("############################################################");
    printf("\t\t\t\tTest #%ld with %ld elements and %ld samples\n", current_test_index + 1, current_test,
           ITERATIONS_COUNT);
    puts("############################################################");

    unsigned* reference_array = prepare_reference_array(current_test);
    unsigned* testing_array = prepare_testing_array(current_test);

    /*******************************************************************************************************************
     *                                        SEQUENTIAL SORTING                                                       *
     ******************************************************************************************************************/
    benchmark_and_report(
        csv_file,
        reference_array,
        testing_array,
        sizeof testing_array[0],
        current_test,
        "Sequential C qsort",
        1,
        int_compare,
        qsort
    );

    benchmark_and_report(
        csv_file,
        reference_array,
        testing_array,
        sizeof testing_array[0],
        current_test,
        "Pure Sequential Dual Pivot",
        1,
        int_compare,
        dual_pivot_quicksort_sequential
    );

    benchmark_and_report(
        csv_file,
        reference_array,
        testing_array,
        sizeof testing_array[0],
        current_test,
        "Adaptive Sequential Dual Pivot",
        1,
        int_compare,
        dual_pivot_quicksort_sequential_adaptive
    );

    /*******************************************************************************************************************
     *                                        PARALLEL SORTING                                                         *
     ******************************************************************************************************************/
    for (size_t num_threads = 2; num_threads <= omp_get_num_procs(); num_threads <<= 1u) {
      benchmark_and_report(
          csv_file,
          reference_array,
          testing_array,
          sizeof testing_array[0],
          current_test,
          "Pure Parallel Dual Pivot",
          num_threads,
          int_compare,
          dual_pivot_quicksort_tasks
      );

      benchmark_and_report(
          csv_file,
          reference_array,
          testing_array,
          sizeof testing_array[0],
          current_test,
          "Adaptive Parallel Dual Pivot",
          num_threads,
          int_compare,
          dual_pivot_quicksort_tasks_adaptive
      );
    }

    free(reference_array);
    free(testing_array);

  }
  fclose(csv_file);
}

int main(int argc, char* argv[argc]) {

  run_benchmarks();

  return EXIT_SUCCESS;
}