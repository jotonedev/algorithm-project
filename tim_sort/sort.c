#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // For memcpy

#include "sort.h"

// Function to perform insertion sort on a subarray.
// Insertion sort is efficient for small or nearly sorted arrays.
void insertion_sort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        // Shift elements greater than temp to the right.
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        // Insert temp in its correct position.
        arr[j + 1] = temp;
    }
}

// Function to merge two sorted subarrays.
// This is a standard merge operation used in merge sort.
void merge(int arr[], int left, int mid, int right, int* temp_arr) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    // Copy the left and right subarrays into temporary arrays using memcpy.
    memcpy(temp_arr, &arr[left], len1 * sizeof(int));
    memcpy(temp_arr + len1, &arr[mid + 1], len2 * sizeof(int));

    int i = 0;      // Index for the left subarray.
    int j = 0;      // Index for the right subarray.
    int k = left;   // Index for the merged array.

    // Merge the two subarrays back into the original array.
    while (i < len1 && j < len2) {
        if (temp_arr[i] <= temp_arr[len1 + j]) {
            arr[k] = temp_arr[i];
            i++;
        } else {
            arr[k] = temp_arr[len1 + j];
            j++;
        }
        k++;
    }

    // Copy any remaining elements from the left subarray.
    if (i < len1) {
        memcpy(&arr[k], &temp_arr[i], (len1 - i) * sizeof(int));
        k += (len1 - i);
    }

    // Copy any remaining elements from the right subarray.
    if (j < len2) {
        memcpy(&arr[k], &temp_arr[len1 + j], (len2 - j) * sizeof(int));
    }
}

// Function to calculate the minimum run length.
// We aim for runs of a size that is a power of 2 (or close to it)
// and also greater than or equal to the THRESHOLD.
int calculate_minrun(int n) {
    int r = 0;  // Becomes 1 if any 1 bits are shifted off.
    while (n >= THRESHOLD) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

// Function to perform TimSort.
// This function takes the array, its length, and a temporary array for merging.
void tim_sort(int arr[], int n, int* temp_arr) {
    // Calculate the minimum run length.
    int minrun = calculate_minrun(n);

    // Sort individual subarrays of size minrun using insertion sort.
    for (int start = 0; start < n; start += minrun) {
        int end = (start + minrun - 1 < n - 1) ? start + minrun - 1 : n - 1;
        insertion_sort(arr, start, end);
    }

    // Start merging from size minrun.
    // Merge subarrays to form size 2 * minrun, then 4 * minrun, and so on.
    for (int size = minrun; size < n; size = 2 * size) {
        // Pick starting point of the left subarray.
        for (int left = 0; left < n; left += 2 * size) {
            // Find the ending point of the left subarray.
            int mid = (left + size - 1 < n-1) ? left + size - 1 : n-1;
            int right = (left + 2 * size - 1 < n - 1) ? left + 2 * size - 1 : n - 1;

            // Merge the subarrays arr[left...mid] and arr[mid+1...right]
            // using the temporary array.
            if (mid < right) {
                merge(arr, left, mid, right, temp_arr);
            }
        }
    }
}
