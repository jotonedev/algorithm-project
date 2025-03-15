cmake_minimum_required(VERSION 3.26)
project(SuperBuild)

include(ExternalProject)

# Create library specific path variables
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

ExternalProject_Add(counting_sort
        BUILD_ALWAYS 1
        SOURCE_DIR ${counting_sort_SOURCE_DIR}
        BINARY_DIR ${counting_sort_SOURCE_DIR}/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND cmake --build . --config Release
        INSTALL_COMMAND
)


ExternalProject_Add(quick_sort
        BUILD_ALWAYS 1
        SOURCE_DIR ${quick_sort_SOURCE_DIR}
        BINARY_DIR ${quick_sort_SOURCE_DIR}/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND cmake --build . --config Release
        INSTALL_COMMAND ""
)


ExternalProject_Add(quick_3way_sort
        BUILD_ALWAYS 1
        SOURCE_DIR ${quick_3way_sort_SOURCE_DIR}
        BINARY_DIR ${quick_3way_sort_SOURCE_DIR}/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND cmake --build . --config Release
        INSTALL_COMMAND ""
)


ExternalProject_Add(quick_sort_random_pivot
        BUILD_ALWAYS 1
        SOURCE_DIR ${quick_sort_random_pivot_SOURCE_DIR}
        BINARY_DIR ${quick_sort_random_pivot_SOURCE_DIR}/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND cmake --build . --config Release --target install
        INSTALL_COMMAND ""
)


ExternalProject_Add(tim_sort
        BUILD_ALWAYS 1
        SOURCE_DIR ${tim_sort_SOURCE_DIR}
        BINARY_DIR ${tim_sort_SOURCE_DIR}/build
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND cmake --build . --config Release
        INSTALL_COMMAND ""
)
