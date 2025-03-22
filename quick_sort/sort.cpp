#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "sort.h"
#include "utils.h"

// ------------------------ Algorithm Code ------------------------

// Partition function to rearrange the elements around the pivot
int partition(int *a, const int i, const int j) {
  int k = i; // index of the pivot's final position after the partition function
  const int pivot = a[j - 1]; // Choose the last element as the pivot

  for (int l = i; l < j; l++) { // Exclude pivot from comparisons
    if (a[l] <= pivot) {
      swap(a, k, l); // Place smaller elements on the left
      k++;
    }
  }
  return k - 1; // Return the pivot's final position
}

// QuickSort function to sort the subarray a[i:j] (j exclusive)
void quick_sort(int *a, const int i, const int j) {
  // Base case: array of size 0 or 1
  if (j - i <= 1) {
    return;
  }

  // Inductive case: partition the array and sort the partitions
  const int k = partition(a, i, j);
  quick_sort(a, i, k);     // Sort elements less than or equal to the pivot
  quick_sort(a, k + 1, j); // Sort elements greater than the pivot
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
  int i = 0; // Counter to measure the number of iterations
  const time_point_t start =
      std::chrono::steady_clock::now(); // Start the clock
  while (elapsed < min_time) { // Continue until the minimum time is reached
    // Increment the counter
    i++;

    // Duplicate the input data
    memcpy(data_copy, data, n * sizeof(int));

    // Call the sort function
    quick_sort(data_copy, 0, n);
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
  quick_sort(arr, 0, n);

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
              << " [length|max] [linear|exponential] [output_file]"
              << std::endl;
    return 1;
  }

  const std::string mode = argv[1];
  const bool test_length = (mode == "length");

  // Default to exponential scaling if not specified
  bool linear_scaling = false;
  if (argc >= 3) {
    const std::string scaling_mode = argv[2];
    linear_scaling = (scaling_mode == "linear");
  }

  // Output filename
  std::string output_file;
  if (argc >= 4) {
    output_file = argv[3];
  } else {
    // Generate a filename based on test parameters and timestamp
    output_file = generate_filename(test_length ? "length" : "max",
                                    linear_scaling, "quick_sort");
  }

  std::vector<RunResult> runs;

  // Determine which parameter to vary and its range
  int min_param, max_param;
  bool vary_length;
  if (test_length) {
    min_param = 100;    // min_length
    max_param = 100000; // max_length
    vary_length = true;
  } else {
    min_param = 10;      // min_max
    max_param = 1000000; // max_max
    vary_length = false;
  }

  // Generate sample points based on scaling type
  std::vector<int> sample_points =
      generate_sample_points(min_param, max_param, NUM_SAMPLES, linear_scaling);

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
    std::cout << "Length: " << run.length << ", Min: " << run.min
              << ", Max: " << run.max << std::endl;

    runs.push_back(run);
  }

  // Write results to CSV file
  write_results_to_csv(output_file, runs);

  return 0;
}

#endif
