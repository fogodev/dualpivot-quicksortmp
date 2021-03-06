cmake_minimum_required(VERSION 3.10)
project(QuickSortMP C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3 -march=native -malign-data=cacheline -finline-functions")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

    add_executable(
            ${PROJECT_NAME}
            main.c
            swap.h
            order.h
            stats.h
            shuffle.h
            partition.h
            insertion_sort.h
            dual_pivot_tasks.h
            dual_pivot_sequential.h
            dualpivot_tasks_adaptive.h
            dual_pivot_sequential_adaptive.h
    )
    target_link_libraries(${PROJECT_NAME} m)
else (OPENMP_FOUND)
    message("ERROR: OpenMP could not be found.")
endif (OPENMP_FOUND)

