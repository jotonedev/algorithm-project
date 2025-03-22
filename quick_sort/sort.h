#ifndef QUICKSORT_SORT_H
#define QUICKSORT_SORT_H


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
 * QuickSort function to sort the subarray a[i:j] (j exclusive)
 * @param a The array of integers
 * @param i The index of the first element
 * @param j The index of the last element
 */
void quick_sort(int *a, int i, int j);

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
