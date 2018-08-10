cmake_minimum_required(VERSION 2.8)
project(QuickSortMP C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 11)

find_library(MEMKIND_LIB memkind /usr/lib64 DEFAULT_PATH)
if(MEMKIND_LIB)
else(MEMKIND_LIB)
    unset(MEMKIND_LIB_FOUND)
    message(FATAL_ERROR "Cannot find MEMKIND RUNTIME LIBRARY")
endif (MEMKIND_LIB)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -march=knl -O3 -mavx512f -mavx512pf -mavx512er -mavx512cd -mfma -malign-data=cacheline -finline-functions")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

    add_executable(QuickSortMP main.c partition.h dualpivot_tasks.h dualpivot_sequential.h dualpivot_sections.h)
    target_link_libraries(QuickSortMP ${MEMKIND_LIB})
else(OPENMP_FOUND)
    message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

