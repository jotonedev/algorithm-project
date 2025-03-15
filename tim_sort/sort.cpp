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
#include <cstring>

#include "sort.h"
#include "utils.h"


// Function to perform insertion sort on a subarray.
void insertion_sort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// Function to merge two sorted subarrays.
void merge(int arr[], int left, int mid, int right, int *temp_arr) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    memcpy(temp_arr, &arr[left], len1 * sizeof(int));
    memcpy(temp_arr + len1, &arr[mid + 1], len2 * sizeof(int));

    int i = 0;
    int j = 0;
    int k = left;

    while (i < len1 && j < len2) {
        if (temp_arr[i] <= temp_arr[len1 + j]) {
            arr[k] = temp_arr[i];
            i++;
        } else {
            arr[k] = temp_arr[len1 + j];
            j++;
        }
        k++;
    }

    if (i < len1) {
        memcpy(&arr[k], &temp_arr[i], (len1 - i) * sizeof(int));
    }

    if (j < len2) {
        memcpy(&arr[k], &temp_arr[len1 + j], (len2 - j) * sizeof(int));
    }
}

// Function to calculate the minimum run length.
int calculate_minrun(int n) {
    int r = 0;
    while (n >= THRESHOLD) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

// Counts the length of a naturally occurring run in the array starting at 'start'.
// A run is a sequence of elements that are either non-decreasing or strictly decreasing.
int count_run(int arr[], int start, int n) {
    if (start == n - 1) {
        return 1; // Single element is a run.
    }

    int curr = start;
    // Determine if the run is increasing or decreasing based on the first two elements.
    if (arr[curr] <= arr[curr + 1]) {
        // Increasing run.
        while (curr < n - 1 && arr[curr] <= arr[curr + 1]) {
            curr++;
        }
    } else {
        // Decreasing run.
        while (curr < n - 1 && arr[curr] > arr[curr + 1]) {
            curr++;
        }
        // Reverse the decreasing run to make it increasing.
        int left = start;
        int right = curr;
        while (left < right) {
            int temp = arr[left];
            arr[left] = arr[right];
            arr[right] = temp;
            left++;
            right--;
        }
    }
    // Return the length of the run.
    return curr - start + 1;
}

// Extends a short run to the minimum length (minrun) using insertion sort if necessary
// and sorts the extended run.
void extend_run_and_sort(int arr[], int start, int *end, int n, int min_run) {
    int run_length = count_run(arr, start, n);
    *end = start + run_length - 1;

    // If the run is shorter than minrun, extend it to minrun.
    if (run_length < min_run) {
        int new_end = (start + min_run - 1 < n - 1) ? start + min_run - 1 : n - 1;
        insertion_sort(arr, start, new_end);
        *end = new_end;
    }
}

// Pushes a run onto the stack.
void push_run(RunStack *stack, int start, int length) {
    stack->stack[stack->num_runs].start = start;
    stack->stack[stack->num_runs].length = length;
    stack->num_runs++;
}

// Merges runs on the stack to maintain the stacking invariants.
void merge_collapse(int arr[], RunStack *stack, int *temp_arr) {
    while (stack->num_runs > 1) {
        int n = stack->num_runs - 2;
        if (n > 0 && stack->stack[n - 1].length <= stack->stack[n].length + stack->stack[n + 1].length) {
            if (stack->stack[n - 1].length < stack->stack[n + 1].length) {
                n--;
            }
            merge_runs(arr, stack, n, n + 1, temp_arr);
        } else if (stack->stack[n].length <= stack->stack[n + 1].length) {
            merge_runs(arr, stack, n, n + 1, temp_arr);
        } else {
            break;
        }
    }
}

// Merges two runs on the stack at indices a and b.
void merge_runs(int arr[], RunStack *stack, int a, int b, int *temp_arr) {
    int start1 = stack->stack[a].start;
    int length1 = stack->stack[a].length;
    int start2 = stack->stack[b].start;
    int length2 = stack->stack[b].length;

    merge(arr, start1, start1 + length1 - 1, start2 + length2 - 1, temp_arr);

    stack->stack[a].length += length2;
    for (int i = b; i < stack->num_runs - 1; i++) {
        stack->stack[i] = stack->stack[i + 1];
    }
    stack->num_runs--;
}

// TimSort function that sorts the array using the provided temporary array and run stack.
void tim_sort(int arr[], int n, int *temp_arr, RunStack *run_stack) {
    if (n < MIN_MERGE) {
        // For very small arrays, use insertion sort directly.
        insertion_sort(arr, 0, n - 1);
        return;
    }
    // Calculate the minimum run length.
    int minrun = calculate_minrun(n);

    run_stack->num_runs = 0;
    int start = 0;
    // Divide the array into runs and sort them.
    while (start < n) {
        int end;
        extend_run_and_sort(arr, start, &end, n, minrun);
        // Push the run onto the stack.
        push_run(run_stack, start, end - start + 1);
        // Merge runs to maintain the stacking invariants.
        merge_collapse(arr, run_stack, temp_arr);

        start = end + 1;
    }

    // Merge any remaining runs on the stack.
    while (run_stack->num_runs > 1) {
        int m = run_stack->num_runs - 2;
        if (m >= 0) {
            merge_runs(arr, run_stack, m, m + 1, temp_arr);
        }
    }
}


long long execute(int n, int data[]) {
    // Pre allocate the memory to avoid the overhead of malloc
    int *temp_arr = new int[n];
    RunStack *run_stack = new RunStack;

    for (int i = 0; i < n; i++) {
        temp_arr[i] = 0;
    }
    memset(run_stack, 0, sizeof(RunStack));

    // Initialize the clock to measure the execution time
    const auto start = std::chrono::steady_clock::now();
    // Call the counting sort function
    tim_sort(data, n, temp_arr, run_stack);
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

    // Free the allocated memory
    delete[] temp_arr;
    delete run_stack;

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
        output_file = generate_filename(test_length ? "length" : "max", linear_scaling, "tim_sort");
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

        for (int curr_length: sample_lengths) {
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
