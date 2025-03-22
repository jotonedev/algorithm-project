#ifndef TIM_SORT_H
#define TIM_SORT_H


// Threshold for switching to insertion sort.
constexpr int THRESHOLD = 64;
// The minimum size to enable the tim sort algorithm.
constexpr int MIN_MERGE = 32;

// The maximum number of pending runs on the stack.
// This needs to be large enough to handle worst-case scenarios but small enough
// to fit in cache.
constexpr int MAX_PENDING_RUNS = 85;


/**
 * @brief Structure to store a run of elements in the array.
 */
struct Run_t {
    int start; // Start index of the run.
    int length; // Length of the run.
} typedef Run;

/**
 * @brief Structure to store the stack of runs.
 */
struct RunStack_t {
    Run stack[MAX_PENDING_RUNS];
    int num_runs; // Number of runs currently on the stack.
} typedef RunStack;


/**
 * @brief Sort the array using the insertion sort algorithm.
 * @param arr The array to sort
 * @param left The left index of the array
 * @param right The right index of the array
 */
void insertion_sort(int arr[], int left, int right);

/**
 * @brief Merge two sorted subarrays into a single sorted array.
 * @param arr The array to merge
 * @param left The left index of the first subarray
 * @param mid The right index of the first subarray and the left index of the second subarray
 * @param right The right index of the second subarray
 * @param temp_arr The temporary array to store the merged array
 */
void merge(int arr[], int left, int mid, int right, int *temp_arr);

/**
 * @brief Calculate the minimum run length for the array.
 * @param n The length of the array
 * @return The minimum run length
 */
int calculate_minrun(int n);

/**
 * @brief Count the length of a naturally occurring run in the array starting at 'start'.
 * A run is a sequence of elements that are either non-decreasing or strictly decreasing.
 * @param arr The array to count the run
 * @param start The start index of the run
 * @param n The length of the array
 * @return The length of the run
 */
int count_run(int arr[], int start, int n);

/**
 * @brief Extend the run to the maximum possible length and sort it using the insertion sort algorithm.
 * @param arr The array to sort
 * @param start The start index of the run
 * @param end The end index of the run
 * @param n The length of the array
 * @param min_run The minimum run length
 */
void extend_run_and_sort(int arr[], int start, int *end, int n, int min_run);

/**
 * @brief Push a run onto the stack of runs.
 * @param stack The stack of runs
 * @param start The start index of the run
 * @param length The length of the run
 */
void push_run(RunStack *stack, int start, int length);

/**
 * @brief Merge runs on the stack
 * @param arr The array to sort
 * @param stack The stack of runs
 * @param temp_arr The temporary array to store the merged array
 */
void merge_collapse(int arr[], RunStack *stack, int *temp_arr);

/**
 * @brief Merge two runs on the stack at indices a and b.
 * Used by the merge_collapse function. This should not be called directly or the stack invariants may be violated.
 * @param arr The array to sort
 * @param stack The stack of runs
 * @param left The index of the first run
 * @param right The index of the second run
 * @param temp_arr The temporary array to store the merged array
 */
void merge_runs(int arr[], RunStack *stack, int left, int right, int *temp_arr);

/**
 * @brief Sort the array using the TimSort algorithm.
 * @param arr The array to sort
 * @param n The length of the array
 * @param temp_arr The temporary array to store the merged array
 * @param run_stack The stack of runs
 */
void tim_sort(int arr[], int n, int *temp_arr, RunStack *run_stack);

/**
 * Execute the sort algorithm
 * @param n The length of the array
 * @param data The array of integers
 * @return The execution time in nanoseconds
 */
long long benchmark_algorithm(int n, int data[]);

/**
 * Main function to run the sort algorithm
 * @param argc The number of command-line arguments
 * @param argv The command-line arguments
 * @return The exit status
 */
int main(int argc, char *argv[]);

#endif
