#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sort.h"


// Function to perform insertion sort on a subarray.
// Insertion sort is efficient for small or nearly sorted arrays.
void insertion_sort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        // Shift elements greater than temp to the right to make space for inserting 'temp'.
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        // Insert 'temp' in its correct sorted position.
        arr[j + 1] = temp;
    }
}

// Function to merge two sorted subarrays.
// This is a standard merge operation used in merge sort.
// It takes two sorted subarrays (arr[left...mid] and arr[mid+1...right])
// and merges them into a single sorted subarray in 'arr'.
// 'temp_arr' is used as temporary storage during the merge.
void merge(int arr[], int left, int mid, int right, int* temp_arr) {
    int len1 = mid - left + 1; // Length of the left subarray.
    int len2 = right - mid;    // Length of the right subarray.

    // Copy the left and right subarrays into temporary arrays.
    for (int i = 0; i < len1; i++) {
        temp_arr[i] = arr[left + i];
    }
    for (int i = 0; i < len2; i++) {
        temp_arr[len1 + i] = arr[mid + 1 + i];
    }

    int i = 0;      // Index for the left subarray.
    int j = 0;      // Index for the right subarray.
    int k = left;   // Index for the merged subarray in 'arr'.

    // Merge the two subarrays back into the original array 'arr'.
    while (i < len1 && j < len2) {
        // Compare elements from the left and right subarrays.
        // Place the smaller element into the merged array.
        if (temp_arr[i] <= temp_arr[len1 + j]) {
            arr[k] = temp_arr[i];
            i++;
        } else {
            arr[k] = temp_arr[len1 + j];
            j++;
        }
        k++;
    }

    // Copy any remaining elements from the left subarray (if any).
    while (i < len1) {
        arr[k] = temp_arr[i];
        k++;
        i++;
    }

    // Copy any remaining elements from the right subarray (if any).
    while (j < len2) {
        arr[k] = temp_arr[len1 + j];
        k++;
        j++;
    }
}

// Function to calculate the minimum run length.
// TimSort works best when the array is divided into runs (sorted subarrays)
// of a certain minimum size. This function calculates that minimum size.
// We aim for runs of a size that is a power of 2 (or close to it)
// and also greater than or equal to the THRESHOLD.
int calculate_minrun(int n) {
    int r = 0;  // Becomes 1 if any 1 bits are shifted off during the while loop
    while (n >= THRESHOLD) {
        r |= (n & 1); // Set r to 1 if the least significant bit of n is 1.
        n >>= 1;      // Right-shift n by 1 bit (equivalent to dividing by 2).
    }
    return n + r;
}

// Function to find the length of a natural run starting at 'start'.
// A natural run is a sequence that is already sorted (either ascending or descending).
// TimSort takes advantage of these naturally occurring runs.
int find_run_length(int arr[], int start, int n) {
    // If we're at the last element, the run length is 1.
    if (start == n - 1) {
        return 1;
    }

    int end = start + 1;

    // Determine if the run is ascending or descending.
    if (arr[end] >= arr[start]) {
        // Ascending run: keep going as long as the elements are in increasing order.
        while (end < n && arr[end] >= arr[end - 1]) {
            end++;
        }
    } else {
        // Descending run: keep going as long as the elements are in decreasing order.
        while (end < n && arr[end] < arr[end - 1]) {
            end++;
        }
        // Reverse the descending run in-place to make it ascending.
        for (int i = start, j = end - 1; i < j; i++, j--) {
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    return end - start; // Return the length of the run.
}

// Function to merge runs on the stack to maintain invariants.
// TimSort uses a stack to keep track of the runs that have been created.
// This function ensures that the runs on the stack are merged in a way that
// maintains certain invariants. These invariants are important for the
// efficiency of the algorithm.
// The invariants, in simplified terms, are:
// If we have runs A, B, and C on the stack (C being the most recently added), then:
// 1. A.length > B.length + C.length
// 2. B.length > C.length
void merge_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr) {
    // Keep merging until the invariants are satisfied.
    while (*stack_size > 1) {
        int n = *stack_size - 2; // Index of the second-to-last run on the stack

        // Check if we need to merge to maintain invariants.
        if ((n > 0 && stack[n - 1].length <= stack[n].length + stack[n + 1].length) ||
            (n > 1 && stack[n - 2].length <= stack[n - 1].length + stack[n].length))
        {
            // If the second-to-last run is shorter than the last run, merge them.
            if (stack[n - 1].length < stack[n + 1].length) {
                n--;
            }
            // Merge runs at indices n and n+1.
            merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
                  stack[n + 1].start + stack[n + 1].length - 1, temp_arr);
            // Update the length of the merged run.
            stack[n].length += stack[n + 1].length;
            // Remove the merged run from the stack.
            stack[n + 1] = stack[n + 2];
            (*stack_size)--;
        } else {
            // Invariants are satisfied, so we can stop merging for now.
            break;
        }
    }
}

// Function to merge all remaining runs on the stack.
// This function is called at the end of TimSort to ensure that only one
// sorted run (the entire sorted array) remains.
void merge_force_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr) {
    // Keep merging until only one run remains on the stack.
    while (*stack_size > 1) {
        int n = *stack_size - 2; // Index of the second-to-last run
        // If there are at least two runs and the second-to-last run is shorter than the last,
        // merge the second-to-last run with the run before it.
        if (n > 0 && stack[n - 1].length < stack[n + 1].length) {
            n--;
        }
        // Merge runs at indices n and n+1.
        merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
              stack[n + 1].start + stack[n + 1].length - 1, temp_arr);
        // Update the length of the merged run.
        stack[n].length += stack[n + 1].length;
        // Reduce the stack size.
        (*stack_size)--;
    }
}

// Function to perform TimSort.
// This is the main function for the TimSort algorithm.
void tim_sort(int arr[], int n, int temp_arr[], Run stack[]) {
    // Calculate the minimum run length.
    int minrun = calculate_minrun(n);

    int stack_size = 0; // Initialize the stack size to 0.

    // Iterate through the array, finding and extending runs.
    int start = 0;
    while (start < n) {
        // Find the length of the natural run starting at 'start'.
        int run_length = find_run_length(arr, start, n);

        // If the natural run is shorter than 'minrun', extend it to 'minrun'.
        int end = (start + run_length - 1 < n - 1) ? start + run_length - 1 : n - 1;
        if (run_length < minrun) {
            end = (start + minrun - 1 < n - 1) ? start + minrun - 1 : n - 1;
            run_length = end - start + 1;
        }

        // Sort the run using insertion sort.
        insertion_sort(arr, start, end);

        // Push the run onto the stack.
        stack[stack_size].start = start;
        stack[stack_size].length = run_length;
        stack_size++;

        // Merge runs on the stack to maintain invariants.
        merge_collapse(arr, stack, &stack_size, temp_arr);

        // Move to the next run.
        start = end + 1;
    }

    // Merge any remaining runs on the stack.
    merge_force_collapse(arr, stack, &stack_size, temp_arr);
}