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


int find_max(int n, int data[]) {
    int max = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] > max) {
            max = data[i];
        }
    }

    return max;
}


int find_min(int n, int data[]) {
    int min = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] < min) {
            min = data[i];
        }
    }

    return min;
}


void counting_sort(int n, int k, int data[], int out[], int count[]) {
    // Find the minimum and maximum elements of the array
    int min_val = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] < min_val) {
            min_val = data[i];
        }
    }

    // Count occurrences of each element
    for (int i = 0; i < n; i++) {
        count[data[i] - min_val]++;
    }

    // Calculate cumulative count
    for (int i = 1; i < k; i++) {
        count[i] += count[i - 1];
    }

    // Build the output array
    for (int i = n - 1; i >= 0; i--) {
        out[count[data[i] - min_val] - 1] = data[i];
        count[data[i] - min_val]--;
    }
}


long long execute(int n, int data[]) {
    // Pre allocate the memory to avoid the overhead of malloc
    int max = find_max(n, data);
    int min = find_min(n, data);
    int k = max - min + 1;

    int *out = new int[n];
    int *count = new int[k];

    // Initialize the count array
    for (int i = 0; i < k; i++) {
        count[i] = 0;
    }
    // Initialize the output array
    for (int i = 0; i < n; i++) {
        out[i] = 0;
    }

    // Initialize the clock to measure the execution time
    const auto start = std::chrono::steady_clock::now();
    // Call the counting sort function
    counting_sort(n, k, data, out, count);
    // Measure the elapsed time
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

// Verify the result
#ifndef BENCHMARK_MODE // Execute only if not in benchmark mode
    // Print the sorted array
    for (int i = 0; i < n; i++) {
        std::cout << out[i] << " ";
    }
    std::cout << std::endl;
#else
    check_result(n, out);
#endif

    // Free the allocated memory
    delete[] out;
    delete[] count;

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

    int min_val = 100;
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

            // Generate input data
            int *data = generate_input_data(curr_length, min_val, max_val);

            // Run multiple times
            for (int r = 0; r < NUM_RUNS; r++) {
                // Make a copy of the data for each run
                int *data_copy = new int[curr_length];
                std::copy(data, data + curr_length, data_copy);

                // Execute and record time
                run.time[r] = execute(curr_length, data_copy);

                delete[] data_copy;
            }

            runs.push_back(run);
            delete[] data;

            // Print results for this run
            std::cout << "Length: " << curr_length << ", Min: " << run.min << ", Max: " << run.max
                      << ", Time (median): " << *std::min_element(run.time, run.time + NUM_RUNS)
                      << ", Resolution: " << run.resolution << std::endl;
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

            // Generate input data
            int *data = generate_input_data(length, min_val, curr_max);

            // Run multiple times
            for (int r = 0; r < NUM_RUNS; r++) {
                // Make a copy of the data for each run
                int *data_copy = new int[length];
                std::copy(data, data + length, data_copy);

                // Execute and record time
                run.time[r] = execute(length, data_copy);

                delete[] data_copy;
            }

            runs.push_back(run);
            delete[] data;
        }
    }

    // Write results to CSV file
    write_results_to_csv(output_file, runs);

    return 0;
}

#endif
