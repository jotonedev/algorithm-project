#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.h"

// Function to generate an array of random integers
void generate_input_data(int out[], const int n, const int min, const int max) {
  // Initialize random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(min, max);

  // Create and fill an array with random integers
  for (int i = 0; i < n; i++) {
    out[i] = dis(gen);
  }

  // Put in a random position the maximum value
  const int max_pos = dis(gen) % n;
  out[max_pos] = max;
}

// Function to check if the array is sorted
void check_result(const int n, const int data[]) {
  for (int i = 0; i < n - 1; i++) {
    if (data[i] > data[i + 1]) {
      throw std::runtime_error("Counting sort failed");
    }
  }
}

// Function to get the resolution of the clock
long long get_resolution() {
  const time_point_t start = std::chrono::steady_clock::now();
  time_point_t end;
  do {
    end = std::chrono::steady_clock::now();
  } while (start >= end);

  const long long elapsed =
      std::chrono::duration_cast<time_unit_t>(end - start).count();
  return elapsed;
}

// Function to determine the minimum measurable time
long long get_minimum_time() {
  static constexpr float max_rel_error = 0.001;
  // Get the resolution of the clock
  const auto resolution = get_resolution();

  // Calculate the minimum measurable time
  const float min_time = resolution * (1 + 1 / max_rel_error);

  return min_time;
}

// Function to generate sample values either linearly or exponentially
std::vector<int> generate_sample_points(const int min_val, const int max_val,
                                        const int num_samples,
                                        const bool linear) {
  // Initialize vector to store sample values
  std::vector<int> samples;
  samples.reserve(num_samples);

  if (linear) { // Linear scaling
    // Calculate linear step size
    double step = static_cast<double>(max_val - min_val) / (num_samples - 1);
    if (step <= 0)
      step = 1;

    // Generate linearly spaced values
    for (int i = 0; i < num_samples; i++) {
      int value = min_val + static_cast<int>(i * step);
      if (value <= samples.back()) { // Avoid duplicates
        value = samples.back() + 1;
      }
      samples.push_back(value);
    }
  } else { // Exponential scaling
    // Calculate exponential scaling factor
    const double factor = std::pow(static_cast<double>(max_val) / min_val,
                                   1.0 / (num_samples - 1));

    // Generate exponentially spaced values
    for (int i = 0; i < num_samples; i++) {
      int value = static_cast<int>(min_val * std::pow(factor, i));
      if (value <=
          samples
              .back()) { // we want to increase the value not to have duplicates
        value = samples.back() + 1;
      }
      samples.push_back(value);
    }
  }

  return samples;
}

// Function to generate a timestamped filename
std::string generate_filename(const std::string &test_type,
                              const bool linear_scaling,
                              const std::string &sort_type) {
  std::stringstream ss;
  ss << sort_type << "_" << test_type << "_"
     << (linear_scaling ? "linear" : "exponential") << ".csv";

  return ss.str();
}

// Function to write benchmark results to a CSV file
void write_results_to_csv(const std::string &filename,
                          const std::vector<RunResult> &runs) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return;
  }

  // Write header
  file << "length,min_val,max_val";
  for (int r = 0; r < NUM_RUNS; r++) {
    file << ",time_" << r;
  }
  file << std::endl;

  // Write data rows
  for (const auto &[min, max, length, time] : runs) {
    file << length << "," << min << "," << max;

    for (int r = 0; r < NUM_RUNS; r++) {
      file << "," << time[r];
    }

    file << std::endl;
  }

  file.close();
}

// Function to set CPU affinity
#if defined(__linux__)
#include <sched.h> // sched_setaffinity+
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif
// Set CPU affinity to the first core
// and increase the priority of the process to the maximum
void set_cpu_affinity() {
#if defined(__linux__)
  // Set CPU affinity to the first core
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);
  if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
    std::cerr << "Failed to set CPU affinity" << std::endl;
  }

  // Set nice value to -20 to increase the priority of the process to the
  // maximum
  if (nice(-20) == -1) {
    std::cerr << "Failed to set nice value to -20" << std::endl;
    // Try to set the priority class to the maximum allower to a non-root user
    if (nice(1) == -1) {
      std::cerr << "Failed to set nice value to 1" << std::endl;
    }
  }

#elif defined(_WIN32)
  // Set CPU affinity to the first core
  DWORD_PTR mask = 1;
  if (!SetProcessAffinityMask(GetCurrentProcess(), mask)) {
    std::cerr << "Failed to set CPU affinity" << std::endl;
  }

  // Increase the priority of the process to the maximum
  if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
    std::cerr << "Failed to set priority class" << std::endl;
  }
#elif defined(__APPLE__)
  // Increase the priority of the process to the maximum
  if (setpriority(PRIO_PROCESS, 0, -20) == -1) {
    std::cerr << "Failed to set priority" << std::endl;
  }
#endif
}

// Function to read array from stdin
std::vector<int> read_input_data() {
  std::vector<int> data;
  std::string line;

  // Read the array from standard input
  std::getline(std::cin, line);
  std::istringstream iss(line);
  int num;
  while (iss >> num) {
    data.push_back(num);
  }

  return data;
}
