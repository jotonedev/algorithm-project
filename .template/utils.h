#ifndef UTILS_H
#define UTILS_H


#include <cmath>
#include <string>
#include <vector>

#define NUM_RUNS 7


/**
 * @brief Structure to store benchmark results
 */
struct RunResult {
    int min; // Minimum value of the array
    int max; // Maximum value of the array
    int length; // Length of the array
    long long time[NUM_RUNS]; // Execution time of the algorithm
    long long resolution; // Resolution of the clock
} typedef Run;


/**
 * @brief Exchange elements at indices k and l
 * @param a the array of integers
 * @param k the index of the first element
 * @param l the index of the second element
 */
void swap(int *a, int k, int l);


/**
 * @brief Generate an array of random integers
 * @param n the size of the array
 * @param min the minimum value of the integers
 * @param max the maximum value of the integers
 * @return the array of random integers of size n
 */
int *generate_input_data(int n, int min, int max);

/**
 * @brief Check if the array is sorted
 * @param n the size of the array
 * @param data the array to check
 * @throws std::runtime_error if the array is not sorted
 */
void check_result(int n, const int data[]);

/**
 * @brief Get the resolution of the clock
 * @return the minimum time that can be measured by the clock
 */
long long get_resolution();

/**
 * @brief Generate sample values either linearly or exponentially
 *
 * @param min_val the minimum value of the array
 * @param max_val the maximum value of the array
 * @param num_samples the number of samples to generate
 * @param linear true for linear scaling, false for exponential scaling
 * @return the vector of sample values
 */
std::vector<int> generate_sample_points(int min_val, int max_val, int num_samples, bool linear);

/**
 * @brief Generate a timestamped filename for benchmark results
 *
 * @param test_type the type of test ("length" or "max")
 * @param linear_scaling true for linear scaling, false for exponential scaling
 * @return the generated filename
 */
std::string generate_filename(const std::string &test_type, const bool linear_scaling, const std::string &sort_type);

/**
 * @brief Write benchmark results to a CSV file
 *
 * @param filename the name of the file to write to
 * @param runs the vector of benchmark runs
 */
void write_results_to_csv(const std::string &filename, const std::vector<Run> &runs);


/**
 * @brief Set CPU affinity to the first core and increase the priority of the process to the maximum
 */
void set_cpu_affinity();

#endif
