#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "dualpivot_sequential.h"
#include "dualpivot_tasks.h"
#include "dualpivot_sections.h"

#define N 700000000

int compare(const void* left, const void* right)
{
  size_t lhs = *((size_t*) left);
  size_t rhs = *((size_t*) right);
  if (lhs == rhs) return 0;
  else if (lhs > rhs) return 1;
  else return -1;
}

void shuffle(size_t* array, size_t n)
{
  srand48(time(NULL));
  
  if (n > 1) {
    for (size_t i = n - 1; i > 0; --i) {
      size_t j = (unsigned int) (drand48() * (i + 1));
      size_t t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}

int is_sorted(
    void* arr,
    size_t type_size,
    size_t n_elements,
    int (* compare)(const void*, const void*))
{
  char* array = (char*) arr;
  
  for (size_t i = 1; i < n_elements; ++i) {
    if (compare((array + type_size * (i - 1)), (array + type_size * i)) > 0) {
      return 0;
    }
  }
  return 1;
}

int main(int argc, char** argv)
{
  puts("Preparing array...");
  size_t* arr = (size_t*) malloc(N * sizeof(size_t));
  if (!arr) exit(666);
  for (size_t i = 0; i < N; ++i) {
    arr[i] = i;
  }

  shuffle(arr, N);
  size_t* arr2 = (size_t*) malloc(N * sizeof(size_t));
  if (!arr2) exit(666);
  for (size_t i = 0; i < N; ++i) {
    arr2[i] = arr[i];
  }

  printf("Sequential sorting...\n");
  double t_inicial = omp_get_wtime();
  dualpivot_quicksort_sequential(arr, 0, N - 1, sizeof(size_t), compare);
  double t_final = omp_get_wtime();
  printf("Elapsed time ordering %d elements sequentially: %lf\n", N, t_final - t_inicial);

  puts("Checking...");
  int check = is_sorted(arr, sizeof(size_t), N, compare);
  if (check) puts("Sequential sort is OK");
  else puts("Sequential sort is wrong");
  free(arr);

  size_t* arr3 = (size_t*) malloc(N * sizeof(size_t));
  if (!arr3) exit(666);
  for (size_t i = 0; i < N; ++i) {
    arr3[i] = arr2[i];
  }

  printf("Sorting with OpenMP Tasks...\n");
  t_inicial = omp_get_wtime();
  dualpivot_quicksort_tasks(arr2, N, sizeof(size_t), compare);
  t_final = omp_get_wtime();
  printf("Elapsed time ordering %d elements with OpenMP Tasks: %lf\n", N, t_final - t_inicial);

  puts("Checking...");
  check = is_sorted(arr2, sizeof(size_t), N, compare);
  if (check) puts("Sort with OpenMP Tasks is OK");
  else puts("Sort with OpenMP Tasks is wrong");
  free(arr2);

  printf("Sorting with OpenMP Sections...\n");
  t_inicial = omp_get_wtime();
  dualpivot_quicksort_sections(arr3, 0, N - 1, sizeof(size_t), compare);
  t_final = omp_get_wtime();
  printf("Elapsed time ordering %d elements with OpenMP Sections: %lf\n", N, t_final - t_inicial);

  puts("Checking...");
  check = is_sorted(arr3, sizeof(size_t), N, compare);
  if (check) puts("Sort with OpenMP Sections is OK");
  else puts("Sort with OpenMP Sections is wrong");
  free(arr3);
  
  return 0;
}