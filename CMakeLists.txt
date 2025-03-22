cmake_minimum_required(VERSION 3.26)
project(SuperBuild)

include(ExternalProject)

# Create library specific path variables
set(BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(tim_sort_SOURCE_DIR ${PROJECT_SOURCE_DIR}/tim_sort)
set(quick_sort_SOURCE_DIR ${PROJECT_SOURCE_DIR}/quick_sort)
set(quick_3way_sort_SOURCE_DIR ${PROJECT_SOURCE_DIR}/quick_3way_sort)
set(quick_sort_random_pivot_SOURCE_DIR ${PROJECT_SOURCE_DIR}/quick_sort_random_pivot)
set(counting_sort_SOURCE_DIR ${PROJECT_SOURCE_DIR}/counting_sort)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

# Include the utils
include_directories(${PROJECT_SOURCE_DIR}/utils)

add_subdirectory(${counting_sort_SOURCE_DIR})

add_subdirectory(${quick_sort_SOURCE_DIR})

add_subdirectory(${quick_3way_sort_SOURCE_DIR})

add_subdirectory(${quick_sort_random_pivot_SOURCE_DIR})

add_subdirectory(${tim_sort_SOURCE_DIR})
