#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Number of runs to average the execution time
constexpr int NUM_RUNS = 64;
constexpr int NUM_SAMPLES = 300;

// Standard types
typedef std::chrono::nanoseconds time_unit_t;
typedef std::chrono::duration<long long, time_unit_t> duration_t;
typedef std::chrono::time_point<std::chrono::steady_clock> time_point_t;

/**
 * @brief Structure to store benchmark results
 */
struct RunResult_t {
  int min;                  // Minimum value of the array
  int max;                  // Maximum value of the array
  int length;               // Length of the array
  long long time[NUM_RUNS]; // Execution time of the algorithm
} typedef RunResult;

/**
 * @brief Exchange elements at indices k and l
 * @param a the array of integers
 * @param k the index of the first element
 * @param l the index of the second element
 */
inline void swap(int *a, const int k, const int l) {
  const int temp = a[k];
  a[k] = a[l];
  a[l] = temp;
}

/**
 * @brief Generate an array of random integers
 * @param out the output array
 * @param n the size of the array
 * @param min the minimum value of the integers
 * @param max the maximum value of the integers
 * @return the array of random integers of size n
 */
void generate_input_data(int out[], int n, int min, int max);

/**
 * @brief Check if the array is sorted
 * @param n the size of the array
 * @param data the array to check
 * @throws std::runtime_error if the array is not sorted
 */
void check_result(int n, int data[]);

/**
 * @brief Get the resolution of the clock
 * @return the minimum time that can be measured by the clock in nanoseconds
 */
long long get_resolution();

/**
 * @brief Determine the minimum measurable time
 * @return the minimum measurable time in nanoseconds
 */
long long get_minimum_time();

/**
 * @brief Generate sample values either linearly or exponentially
 *
 * @param min_val the minimum value of the array
 * @param max_val the maximum value of the array
 * @param num_samples the number of samples to generate
 * @param linear true for linear scaling, false for exponential scaling
 * @return the vector of sample values
 */
std::vector<int> generate_sample_points(int min_val, int max_val,
                                        int num_samples, bool linear);

/**
 * @brief Generate a timestamped filename for benchmark results
 *
 * @param test_type the type of test ("length" or "max")
 * @param linear_scaling true for linear scaling, false for exponential scaling
 * @param sort_type the name of the sorting algorithm
 * @return the generated filename
 */
std::string generate_filename(const std::string &test_type, bool linear_scaling,
                              const std::string &sort_type);

/**
 * @brief Write benchmark results to a CSV file
 *
 * @param filename the name of the file to write to
 * @param runs the vector of benchmark runs
 */
void write_results_to_csv(const std::string &filename,
                          const std::vector<RunResult> &runs);

/**
 * @brief Set CPU affinity to the first core and increase the priority of the
 * process to the maximum
 */
void set_cpu_affinity();

/**
 * @brief Read array from stdin
 * @return the vector of integers read from stdin
 */
std::vector<int> read_input_data();

#endif
