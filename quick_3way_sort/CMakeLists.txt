cmake_minimum_required(VERSION 3.26)
project(quick_3way_sort)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

# Get the directory of the project
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# Add the executable
add_executable(quick_3way_sort sort.cpp ../utils/utils.cpp)

# Add the include directories
target_include_directories(quick_3way_sort PRIVATE ../utils)
target_include_directories(quick_3way_sort PRIVATE ./)

# Copy the artifacts to the bin directory
install(TARGETS quick_3way_sort DESTINATION ${PROJECT_DIR}/bin)

