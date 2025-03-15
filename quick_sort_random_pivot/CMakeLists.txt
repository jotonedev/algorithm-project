cmake_minimum_required(VERSION 3.26)
project(quick_sort_random_pivot)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

# Get the directory of the project
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# Add the executable
add_executable(quick_sort_random_pivot sort.cpp ../utils/utils.cpp)

# Add the include directories
target_include_directories(quick_sort_random_pivot PRIVATE ../utils)
target_include_directories(quick_sort_random_pivot PRIVATE ./)

# Enable benchmark mode compilation
add_definitions(-DBENCHMARK_MODE)

# Copy the artifacts to the bin directory
install(TARGETS quick_sort_random_pivot DESTINATION ${PROJECT_DIR}/bin)