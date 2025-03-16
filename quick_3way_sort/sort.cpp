#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "sort.h"
#include "utils.h"


// Partition function
void partition_3way(int *a, int i, int j, int *k, int *l) {
    // The function partitions the array into three parts:
    // - Elements less than the pivot
    // - Elements equal to the pivot
    // - Elements greater than the pivot

    int pivot = a[j - 1]; // Choose the last element as the pivot
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
void quick_3way_sort(int *a, int i, int j) {
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


long long execute(int n, int data[]) {
    // Initialize the clock to measure the execution time
    const auto start = std::chrono::steady_clock::now();
    // Call the counting sort function
    quick_3way_sort(data, 0, n);
    // Measure the elapsed time
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    // Verify the result
#ifndef BENCHMARK_MODE // Execute only if not in benchmark mode
    // Print the sorted array
    for (int i = 0; i < n; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
#else
    check_result(n, data);
#endif

    // Compute the elapsed time in nanoseconds
    return elapsed.count();
}


#ifndef BENCHMARK_MODE

int main(int argc, char *argv[]) {
    // Read array from stdin without length given separated by space
    std::vector<int> data;
    std::string line;

    // Read the array from standard input
    std::getline(std::cin, line);
    std::istringstream iss(line);
    int num;
    while (iss >> num) {
        data.push_back(num);
    }

    // Convert the vector to an array
    int n = data.size();
    int *arr = data.data();

    // Execute the counting sort algorithm
    try {
        execute(n, arr);
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#endif


#ifdef BENCHMARK_MODE

int main(int argc, char *argv[]) {
    set_cpu_affinity();

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [length|max] [linear|exponential] [output_file]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    bool test_length = (mode == "length");

    // Default to exponential scaling if not specified
    bool linear_scaling = false;
    if (argc >= 3) {
        std::string scaling_mode = argv[2];
        linear_scaling = (scaling_mode == "linear");
    }

    // Output filename
    std::string output_file;
    if (argc >= 4) {
        output_file = argv[3];
    } else {
        // Generate a filename based on test parameters and timestamp
        output_file = generate_filename(test_length ? "length" : "max", linear_scaling, "counting_sort");
    }

    int min_val = 1;
    int max_val = 1000000;
    int length = 100000;
    int num_samples = 300; // Can be adjusted

    std::vector<Run> runs;

    if (test_length) {
        // Test by varying length from 100 to 100,000
        int min_length = 100;
        int max_length = 100000;

        // Generate sample points based on scaling type
        std::vector<int> sample_lengths = generate_sample_points(min_length, max_length, num_samples, linear_scaling);

        for (const int curr_length: sample_lengths) {
            Run run;
            run.min = min_val;
            run.max = max_val;
            run.length = curr_length;
            run.resolution = get_resolution();

            // Run multiple times
            for (int r = 0; r < NUM_RUNS; r++) {
                // Generate input data
                int *data = generate_input_data(curr_length, min_val, max_val);
                // Execute and record time
                run.time[r] = execute(curr_length, data);
                // Free the allocated memory
                delete[] data;
            }

            // Print results for this run
            std::cout << "Length: " << curr_length << ", Min: " << run.min << ", Max: " << run.max << ", Resolution: " << run.resolution << std::endl;

            runs.push_back(run);
        }
    } else {
        // Test by varying max_val from 10 to 1,000,000
        int min_max = 10;
        int max_max = 1000000;

        // Generate sample points based on scaling type
        std::vector<int> sample_maxes = generate_sample_points(min_max, max_max, num_samples, linear_scaling);

        for (int curr_max: sample_maxes) {
            Run run;
            run.min = min_val;
            run.max = curr_max;
            run.length = length;
            run.resolution = get_resolution();

            // Run multiple times for each sample point
            for (int r = 0; r < NUM_RUNS; r++) {

                // Generate input data
                int *data = generate_input_data(length, min_val, curr_max);
                // Execute and record time
                run.time[r] = execute(length, data);
                // Free the allocated memory
                delete[] data;
            }

            // Print results for this run
            std::cout << "Length: " << run.length << ", Min: " << run.min << ", Max: " << run.max << ", Resolution: " << run.resolution << std::endl;

            runs.push_back(run);
        }
    }

    // Write results to CSV file
    write_results_to_csv(output_file, runs);

    return 0;
}

#endif
