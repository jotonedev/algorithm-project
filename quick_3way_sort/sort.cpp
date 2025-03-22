#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "sort.h"
#include "utils.h"


// Partition function
void partition_3way(int *a, const int i, const int j, int *k, int *l) {
    // The function partitions the array into three parts:
    // - Elements less than the pivot
    // - Elements equal to the pivot
    // - Elements greater than the pivot

    const int pivot = a[j - 1]; // Choose the last element as the pivot
    int p1 = i, p2 = i, p3 = i;

    while (p3 < j) {
        if (a[p3] < pivot) { // Case 1: Current element is less than the pivot
            swap(a, p3, p2); // Move current element to the <= pivot region
            swap(a, p2, p1); // Move boundary for < pivot
            p1++;
            p2++;
            p3++;
        } else if (a[p3] == pivot) { // Case 2: Current element is equal to the pivot
            swap(a, p3, p2); // Move current element to the <= pivot region
            p2++;
            p3++;
        } else { // Case 3: Current element is greater than the pivot
            p3++; // Simply move to the next element
        }
    }

    // After the loop:
    // - Elements in range [i, p1) are < pivot
    // - Elements in range [p1, p2) are == pivot
    // - Elements in range [p2, j) are > pivot

    *k = p1; // Set `k` to the start of the == pivot region
    *l = p2; // Set `l` to the end of the == pivot region
}

// Recursive 3-way QuickSort function
void quick_3way_sort(int *a, const int i, const int j) {
    if (j - i <= 1) {
        // Base case: If the subarray size is 0 or 1, it is already sorted
        return;
    }

    int k, l;
    // Partition the array into three parts
    partition_3way(a, i, j, &k, &l);

    quick_3way_sort(a, i, k); // Recursively sort the < pivot region
    quick_3way_sort(a, l, j); // Recursively sort the > pivot region
}

// ------------------------ Benchmarking Code ------------------------

long long benchmark_algorithm(const int n, int data[]) {
    // Pre allocate the memory to avoid the overhead of malloc
    auto *data_copy = new int[n];

    // Get the resolution of the clock
    const auto min_time = get_minimum_time();

    // Initialize the clock to measure the execution time
    long long elapsed = 0;
    // Measure the elapsed time
    int i = 0;  // Counter to measure the number of iterations
    const time_point_t start = std::chrono::steady_clock::now();  // Start the clock
    while (elapsed < min_time) {  // Continue until the minimum time is reached
        // Increment the counter
        i++;

        // Duplicate the input data
        memcpy(data_copy, data, n * sizeof(int));

        // Call the sort function
        quick_3way_sort(data, 0, n);
        // Measure the elapsed time
        time_point_t end = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<time_unit_t>(end - start).count();
    }

    // Free the allocated memory
    delete[] data_copy;

    // Compute the elapsed time in nanoseconds
    return elapsed / i;
}

// ------------------------ Main Function ------------------------

#ifndef BENCHMARK_MODE

int main(int argc, char *argv[]) {
    // Read array from stdin
    std::vector<int> data = read_input_data();

    // Convert the vector to an array
    const int n = data.size();
    int *arr = data.data();

    // Execute the sort algorithm
    quick_3way_sort(arr, 0, n);

    // Print the sorted array
    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }

    return 0;
}

#else

int main(const int argc, char *argv[]) {
    set_cpu_affinity();

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                << " [length|max] [output_file]"
                << std::endl;
        return 1;
    }

    const std::string mode = argv[1];
    const bool test_length = (mode == "length");

    // Output filename
    std::string output_file;
    if (argc >= 4) {
        output_file = argv[3];
    } else {
        // Generate a filename based on test parameters and timestamp
        output_file = generate_filename(test_length ? "length" : "max", "quick_3way_sort");
    }

    std::vector<RunResult> runs;

    // Determine which parameter to vary and its range
    int min_param, max_param;
    bool vary_length;
    if (test_length) {
        min_param = 100;        // min_length
        max_param = 100000;     // max_length
        vary_length = true;
    } else {
        min_param = 10;         // min_max
        max_param = 1000000;    // max_max
        vary_length = false;
    }

    // Generate sample points based on scaling type
    std::vector<int> sample_points = generate_sample_points(min_param, max_param, NUM_SAMPLES);

    // Run tests for each sample point
    for (const int param_value : sample_points) {
        RunResult run = {};

        if (vary_length) {
            run.max = 1000000;
            run.length = param_value;
        } else {
            run.max = param_value;
            run.length = 100000;
        }

        // Run multiple times for each sample point
        for (int r = 0; r < NUM_RUNS; r++) {
            // Generate input data with appropriate parameters
            const auto data = new int[run.length];
            generate_input_data(data, run.length, run.min, run.max);
            // Execute and record time
            run.time[r] = benchmark_algorithm(run.length, data);
            // Free the allocated memory
            delete[] data;
        }

        // Print results for this run
        std::cout << "Length: " << run.length
                  << ", Min: " << run.min
                  << ", Max: " << run.max << std::endl;

        runs.push_back(run);
    }

    // Write results to CSV file
    write_results_to_csv(output_file, runs);

    return 0;
}

#endif
