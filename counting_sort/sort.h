#ifndef COUNTING_SORT_H
#define COUNTING_SORT_H

/**
 * Find the maximum value in the array
 * @param n The length of the array
 * @param data The array of integers
 * @return The maximum value in the array
 */
int find_max(int n, int data[]);

/**
 * Find the minimum value in the array
 * @param n The length of the array
 * @param data The array of integers
 * @return The minimum value in the array
 */
int find_min(int n, int data[]);

/**
 * Perform counting sort on the input array
 * @param n The length of the array
 * @param data The array of integers
 * @return The execution time in nanoseconds
 */
void counting_sort(int n, int data[], int out[], int count[], int k);

/**
 * Execute the sort algorithm
 * @param n The length of the array
 * @param data The array of integers
 * @return The execution time in nanoseconds
 */
long long execute(int n, int data[]);

/**
 * Main function to run the sort algorithm
 * @param argc The number of command-line arguments
 * @param argv The command-line arguments
 * @return The exit status
 */
int main(int argc, char *argv[]);

#endif
