#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
void merge(int arr[], int left, int mid, int right, int* temp_arr) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    // Copy the left and right subarrays into temporary arrays.
    memcpy(temp_arr, arr + left, len1 * sizeof(int));

    int* left_arr = temp_arr;
    int* right_arr = arr + mid + 1;

    int i = 0;      // Index for the left subarray.
    int j = 0;      // Index for the right subarray.
    int k = left;   // Index for the merged subarray in 'arr'.

    // Merge the two subarrays back into the original array 'arr'.
    while (i < len1 && j < len2) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k++] = left_arr[i++];
        } else {
            arr[k++] = right_arr[j++];
        }
    }

    // Copy any remaining elements from the left subarray.
    if (i < len1) {
        memcpy(arr + k, left_arr + i, (len1 - i) * sizeof(int));
    }

    // Copy any remaining elements from the right subarray.
    if (j < len2) {
        memcpy(arr + k, right_arr + j, (len2 - j) * sizeof(int));
    }
}

// Function to calculate the minimum run length.
int calculate_minrun(int n) {
    int r = 0;
    while (n >= THRESHOLD) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

// Function to find the length of a natural run.
int find_run_length(int arr[], int start, int n) {
    if (start == n - 1) {
        return 1;
    }

    int end = start + 1;
    if (arr[end] >= arr[start]) {
        while (end < n && arr[end] >= arr[end - 1]) {
            end++;
        }
    } else {
        while (end < n && arr[end] < arr[end - 1]) {
            end++;
        }
        for (int i = start, j = end - 1; i < j; i++, j--) {
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    return end - start;
}

// Function to merge runs on the stack to maintain invariants.
void merge_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr) {
    while (*stack_size > 1) {
        int n = *stack_size - 2;
        if ((n > 0 && stack[n - 1].length <= stack[n].length + stack[n + 1].length) ||
            (n > 1 && stack[n - 2].length <= stack[n - 1].length + stack[n].length)) {
            if (stack[n - 1].length < stack[n + 1].length) {
                n--;
            }
            merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
                  stack[n + 1].start + stack[n + 1].length - 1, temp_arr);
            stack[n].length += stack[n + 1].length;
            stack[n + 1] = stack[n + 2];
            (*stack_size)--;
        } else {
            break; // Invariants are satisfied
        }
    }
}

// Function to merge all remaining runs on the stack.
void merge_force_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr) {
    while (*stack_size > 1) {
        int n = *stack_size - 2;
        if (n > 0 && stack[n - 1].length < stack[n + 1].length) {
            n--;
        }
        merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
              stack[n + 1].start + stack[n + 1].length - 1, temp_arr);
        stack[n].length += stack[n + 1].length;
        (*stack_size)--;
    }
}

// Function to perform TimSort.
void tim_sort(int arr[], int n, int* temp_arr, Run* stack) {
    if (n < 2) {
        return; // Nothing to sort
    }

    int minrun = calculate_minrun(n);
    int stack_size = 0;

    int start = 0;
    while (start < n) {
        int run_length = find_run_length(arr, start, n);
        int end = (start + run_length - 1 < n - 1) ? start + run_length - 1 : n - 1;

        if (run_length < minrun) {
            end = (start + minrun - 1 < n - 1) ? start + minrun - 1 : n - 1;
            run_length = end - start + 1;
        }

        insertion_sort(arr, start, end);

        stack[stack_size].start = start;
        stack[stack_size].length = run_length;
        stack_size++;

        merge_collapse(arr, stack, &stack_size, temp_arr);

        start = end + 1;
    }

    merge_force_collapse(arr, stack, &stack_size, temp_arr);
}