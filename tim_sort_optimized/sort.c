#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // For bool type
#include <string.h>  // For memcpy

#include "sort.h"

// Function to perform insertion sort on a subarray.
void insertion_sort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// Function to merge two sorted subarrays.
void merge(int arr[], int left, int mid, int right, int* temp_arr) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    memcpy(temp_arr, &arr[left], len1 * sizeof(int));
    memcpy(temp_arr + len1, &arr[mid + 1], len2 * sizeof(int));

    int i = 0;
    int j = 0;
    int k = left;

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

    if (i < len1) {
        memcpy(&arr[k], &temp_arr[i], (len1 - i) * sizeof(int));
    }

    if (j < len2) {
        memcpy(&arr[k], &temp_arr[len1 + j], (len2 - j) * sizeof(int));
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

// Counts the length of a naturally occurring run in the array starting at 'start'.
// A run is a sequence of elements that are either non-decreasing or strictly decreasing.
int count_run(int arr[], int start, int n) {
    if (start == n - 1) {
        return 1; // Single element is a run.
    }

    int curr = start;
    // Determine if the run is increasing or decreasing based on the first two elements.
    if (arr[curr] <= arr[curr + 1]) {
        // Increasing run.
        while (curr < n - 1 && arr[curr] <= arr[curr + 1]) {
            curr++;
        }
    } else {
        // Decreasing run.
        while (curr < n - 1 && arr[curr] > arr[curr + 1]) {
            curr++;
        }
        // Reverse the decreasing run to make it increasing.
        int left = start;
        int right = curr;
        while (left < right) {
            int temp = arr[left];
            arr[left] = arr[right];
            arr[right] = temp;
            left++;
            right--;
        }
    }
    // Return the length of the run.
    return curr - start + 1;
}

// Extends a short run to the minimum length (minrun) using insertion sort if necessary
// and sorts the extended run.
void extend_run_and_sort(int arr[], int start, int* end, int n, int minrun) {
    int run_length = count_run(arr, start, n);
    *end = start + run_length - 1;

    // If the run is shorter than minrun, extend it to minrun.
    if (run_length < minrun) {
        int new_end = (start + minrun - 1 < n - 1) ? start + minrun - 1 : n - 1;
        insertion_sort(arr, start, new_end);
        *end = new_end;
    }
}

// Pushes a run onto the stack.
void push_run(RunStack* stack, int start, int length) {
    stack->stack[stack->num_runs].start = start;
    stack->stack[stack->num_runs].length = length;
    stack->num_runs++;
}

// Merges runs on the stack to maintain the stacking invariants.
void merge_collapse(int arr[], RunStack* stack, int* temp_arr) {
    while (stack->num_runs > 1) {
        int n = stack->num_runs - 2;
        if (n > 0 && stack->stack[n - 1].length <= stack->stack[n].length + stack->stack[n + 1].length) {
            if (stack->stack[n - 1].length < stack->stack[n + 1].length) {
                n--;
            }
            merge_runs(arr, stack, n, n + 1, temp_arr);
        } else if (stack->stack[n].length <= stack->stack[n + 1].length) {
            merge_runs(arr, stack, n, n + 1, temp_arr);
        } else {
            break;
        }
    }
}

// Merges two runs on the stack at indices a and b.
void merge_runs(int arr[], RunStack* stack, int a, int b, int* temp_arr) {
    int start1 = stack->stack[a].start;
    int length1 = stack->stack[a].length;
    int start2 = stack->stack[b].start;
    int length2 = stack->stack[b].length;

    merge(arr, start1, start1 + length1 - 1, start2 + length2 - 1, temp_arr);

    stack->stack[a].length += length2;
    for (int i = b; i < stack->num_runs - 1; i++) {
        stack->stack[i] = stack->stack[i + 1];
    }
    stack->num_runs--;
}

// Internal TimSort function that sorts the array using the provided temporary array and run stack.
void tim_sort(int arr[], int n, int* temp_arr, RunStack* run_stack) {
    if (n < MIN_MERGE) {
        // For very small arrays, use insertion sort directly.
        insertion_sort(arr, 0, n - 1);
        return;
    }
    // Calculate the minimum run length.
    int minrun = calculate_minrun(n);

    run_stack->num_runs = 0;
    int start = 0;
    // Divide the array into runs and sort them.
    while (start < n) {
        int end;
        extend_run_and_sort(arr, start, &end, n, minrun);
        // Push the run onto the stack.
        push_run(run_stack, start, end - start + 1);
        // Merge runs to maintain the stacking invariants.
        merge_collapse(arr, run_stack, temp_arr);

        start = end + 1;
    }

    // Merge any remaining runs on the stack.
    while (run_stack->num_runs > 1) {
        int n = run_stack->num_runs - 2;
        if (n >= 0)
            merge_runs(arr, run_stack, n, n + 1, temp_arr);
    }
}
