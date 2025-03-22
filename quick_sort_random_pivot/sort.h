#ifndef RANDOMIZED_QUICKSORT_SORT_H
#define RANDOMIZED_QUICKSORT_SORT_H


/**
 * @brief Partitions the array
 *
 * This function rearranges elements in the subarray such that all elements less than
 * or equal to the pivot are placed to its left and all greater elements to its right.
 * The pivot is chosen as the last element of the subarray.
 *
 * @param a Pointer to the array of integers.
 * @param i Starting index of the subarray.
 * @param j Ending index of the subarray (exclusive).
 * @return int The final index position of the pivot after partitioning.
 */
int partition(int *a, int i, int j);

/**
 * @brief Generates a random number between i and j
 *
 * @param i The lower bound of the random number.
 * @param j The upper bound of the random number.
 * @return int The random number generated.
 */
int random_between(int i, int j);

/**
 * @brief Randomized partition function
 *
 * This function selects a random index between i and j-1, swaps it with the last element,
 * and then calls the partition function to rearrange the elements around the pivot.
 *
 * @param a Pointer to the array of integers.
 * @param i Starting index of the subarray.
 * @param j Ending index of the subarray (exclusive).
 * @return int The final index position of the pivot after partitioning.
 */
int randomized_partition(int *a, int i, int j);

/**
 * @brief Randomized quick sort algorithm
 * This function sorts the array of integers using the randomized quick sort algorithm.
 * @param a Pointer to the array of integers.
 * @param i Starting index of the subarray.
 * @param j Ending index of the subarray (exclusive).
 */
void randomized_quick_sort(int *a, int i, int j);

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
