cmake_minimum_required(VERSION 3.26)
project(tim_sort)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

# Get the directory of the project
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# Add the executable
add_executable(tim_sort sort.cpp ../utils/utils.cpp)

# Add the include directories
target_include_directories(tim_sort PRIVATE ../utils)
target_include_directories(tim_sort PRIVATE ./)

# Enable benchmark mode compilation
add_definitions(-DBENCHMARK_MODE)

# Copy the artifacts to the bin directory
install(TARGETS tim_sort DESTINATION ${PROJECT_DIR}/bin)