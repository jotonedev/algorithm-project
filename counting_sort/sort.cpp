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

int find_max(const int n, const int data[]) {
  int max = data[0];

  for (int i = 1; i < n; i++) {
    if (data[i] > max) {
      max = data[i];
    }
  }

  return max;
}

int find_min(const int n, const int data[]) {
  int min = data[0];

  for (int i = 1; i < n; i++) {
    if (data[i] < min) {
      min = data[i];
    }
  }

  return min;
}

void counting_sort(const int n, const int k, const int data[], int out[],
                   int count[]) {
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

// ------------------------ Benchmarking Code ------------------------

long long benchmark_algorithm(const int n, int data[]) {
  // Pre allocate the memory to avoid the overhead of malloc
  const int max = find_max(n, data);
  const int min = find_min(n, data);
  const int k = max - min + 1;

  auto *data_copy = new int[n];
  auto *out = new int[n];
  auto *count = new int[k];

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
    // Clear the arrays
    memset(out, 0, n * sizeof(int));
    memset(count, 0, k * sizeof(int));

    // Call the sort function
    counting_sort(n, k, data, out, count);
    // Measure the elapsed time
    time_point_t end = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<time_unit_t>(end - start).count();
  }

  // Free the allocated memory
  delete[] out;
  delete[] count;
  delete[] data_copy;

  // Compute the elapsed time in nanoseconds
  return elapsed / i;
}

// ------------------------ Main Function ------------------------

#ifndef BENCHMARK_MODE

int main(int argc, char *argv[]) {
  // Read array from stdin
  const std::vector<int> data = read_input_data();

  // Convert the vector to an array
  const int n = data.size();
  auto *arr = data.data();

  const int max = find_max(n, arr);
  const int min = find_min(n, arr);
  const int k = max - min + 1;

  // Pre allocate the memory
  auto *out = new int[n];
  auto *count = new int[k];
  // Initialize the arrays
  memset(out, 0, n * sizeof(int));
  memset(count, 0, k * sizeof(int));

  // Execute the sort algorithm
  counting_sort(n, k, arr, out, count);

  // Print the sorted array
  for (int i = 0; i < n; i++) {
    std::cout << out[i] << " ";
  }

  // Free the allocated memory
  delete[] out;
  delete[] count;

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
                                    linear_scaling, "counting_sort");
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
