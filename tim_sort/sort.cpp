#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include "sort.h"
#include "utils.h"


// Function to perform insertion sort on a subarray.
void insertion_sort(int arr[], const int left, const int right) {
    for (int i = left + 1; i <= right; i++) {
        const int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// Function to merge two sorted sub arrays.
// It uses the merge operation from the merge sort algorithm.
void merge(int arr[], const int left, const int mid, const int right, int *temp_arr) {
    const int left_len = mid - left + 1;
    const int right_len = right - mid;

    memcpy(temp_arr, &arr[left], left_len * sizeof(int));
    memcpy(temp_arr + left_len, &arr[mid + 1], right_len * sizeof(int));

    int i = 0;
    int j = 0;
    int k = left;

    while (i < left_len && j < right_len) {
        if (temp_arr[i] <= temp_arr[left_len + j]) {
            arr[k] = temp_arr[i];
            i++;
        } else {
            arr[k] = temp_arr[left_len + j];
            j++;
        }
        k++;
    }

    if (i < left_len) {
        memcpy(&arr[k], &temp_arr[i], (left_len - i) * sizeof(int));
    }

    if (j < right_len) {
        memcpy(&arr[k], &temp_arr[left_len + j], (right_len - j) * sizeof(int));
    }
}

// Function to calculate the minimum run length.
int calculate_minrun(int n) {
    int r = 0;
    // Compute the minimum run length
    while (n >= THRESHOLD) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

// Counts the length of a naturally occurring run in the array starting at 'start'.
// A run is a sequence of elements that are either non-decreasing or strictly decreasing.
int count_run(int arr[], const int start, const int n) {
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
            const int temp = arr[left];
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
void extend_run_and_sort(int arr[], const int start, int *end, const int n,
                         const int min_run) {
    const int run_length = count_run(arr, start, n);
    *end = start + run_length - 1;

    // If the run is shorter than minrun, extend it to minrun.
    if (run_length < min_run) {
        const int new_end = (start + min_run - 1 < n - 1) ? start + min_run - 1 : n - 1;
        insertion_sort(arr, start, new_end);
        *end = new_end;
    }
}

// Pushes a run onto the stack.
void push_run(RunStack *stack, const int start, const int length) {
  	// Add the run to the array while the invariants are maintained.
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
void merge_runs(int arr[], RunStack *stack, const int left, const int right, int *temp_arr) {
    const int start_left = stack->stack[left].start;
    const int length_left = stack->stack[left].length;
    const int start_right = stack->stack[right].start;
    const int length_right = stack->stack[right].length;

    merge(arr, start_left, start_left + length_left - 1, start_right + length_right - 1, temp_arr);

    stack->stack[left].length += length_right;
    for (int i = right; i < stack->num_runs - 1; i++) {
        stack->stack[i] = stack->stack[i + 1];
    }
    stack->num_runs--;
}

// TimSort function that sorts the array using the provided temporary array and run stack.
void tim_sort(int arr[], const int n, int *temp_arr, RunStack *run_stack) {
    if (n < MIN_MERGE) {
        // For very small arrays, use insertion sort directly.
        insertion_sort(arr, 0, n - 1);
        return;
    }
    // Calculate the minimum run length.
    const int minrun = calculate_minrun(n);

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

// ------------------------ Benchmarking Code ------------------------

long long benchmark_algorithm(const int n, int data[]) {
    // Pre allocate the memory to avoid the overhead of malloc
    auto *data_copy = new int[n];
    auto *temp_arr = new int[n];
    auto *run_stack = new RunStack;

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
        // Clear the run stack
        memset(run_stack, 0, sizeof(RunStack));
        // Clear the temporary array
        memset(temp_arr, 0, n * sizeof(int));

        // Call the sort function
        tim_sort(data_copy, n, temp_arr, run_stack);
        // Measure the elapsed time
        time_point_t end = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<time_unit_t>(end - start).count();
    }

    // Free the allocated memory
    delete[] data_copy;
    delete[] temp_arr;
    delete run_stack;

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

    // Pre allocate the memory
    auto *temp_arr = new int[n];
    auto *run_stack = new RunStack;
    // Initialize the arrays
    memset(temp_arr, 0, n * sizeof(int));
    memset(run_stack, 0, sizeof(RunStack));

    // Execute the sort algorithm
    tim_sort(arr, n, temp_arr, run_stack);

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
        output_file = generate_filename(test_length ? "length" : "max", "tim_sort");
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
