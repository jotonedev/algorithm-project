#ifndef QUICK3WAY_SORT_H
#define QUICK3WAY_SORT_H

/**
 * @brief Partitions the array into three parts for the 3-way quick sort.
 *
 * This function partitions the subarray a[i...j-1] into three parts:
 * - Elements less than the pivot.
 * - Elements equal to the pivot.
 * - Elements greater than the pivot.
 *
 * The pivot is chosen as the last element of the subarray (a[j-1]). It rearranges
 * the elements so that [i, k) contains elements less than the pivot, [k, l) contains
 * elements equal to the pivot, and [l, j) contains elements greater than the pivot.
 *
 * @param a Pointer to the array of integers.
 * @param i Starting index of the subarray.
 * @param j One past the last index of the subarray.
 * @param k Pointer to store the starting index of the equal-to-pivot region.
 * @param l Pointer to store the ending index (exclusive) of the equal-to-pivot region.
 */
void partition_3way(int *a, int i, int j, int *k, int *l);

/**
 * @brief Recursive 3-way QuickSort function.
 * @param a Pointer to the array of integers.
 * @param i The starting index of the subarray.
 * @param j One past the last index of the subarray.
 */
void quick_3way_sort(int *a, int i, int j);

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
