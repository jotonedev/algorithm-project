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

// Function to perform a galloping search to the left.
// This function helps to find the position where an element from the right
// subarray should be inserted into the left subarray during merging,
// when galloping mode is active.
// Returns the index in the left subarray.
int gallop_left(int key, int arr[], int start, int length, int hint) {
    int ofs = 1;
    int last_ofs = 0;

    if (key > arr[start + hint]) { // key could be after hint
        int max_ofs = length - hint;
        while (ofs < max_ofs && key > arr[start + hint + ofs]) {
            last_ofs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) {
                ofs = max_ofs; // Overflow
            }
        }
        if (ofs > max_ofs) {
            ofs = max_ofs;
        }
        // Now we know that key belongs somewhere between start + hint + last_ofs and start + hint + ofs
        last_ofs += hint;
        ofs += hint;
    } else { // key must be before hint
        int max_ofs = hint + 1;
        while (ofs < max_ofs && key <= arr[start + hint - ofs]) {
            last_ofs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) {
                ofs = max_ofs; // Overflow
            }
        }
        if (ofs > max_ofs) {
            ofs = max_ofs;
        }
        // Now we know that key belongs somewhere between start + hint - ofs and start + hint - last_ofs
        int tmp = last_ofs;
        last_ofs = hint - ofs;
        ofs = hint - tmp;
    }

    // Now do a binary search to find the exact location
    last_ofs++;
    while (last_ofs < ofs) {
        int m = last_ofs + ((ofs - last_ofs) >> 1);
        if (key > arr[start + m]) {
            last_ofs = m + 1;
        } else {
            ofs = m;
        }
    }
    return ofs; // Index in arr
}

// Function to perform a galloping search to the right.
// Similar to gallop_left, but finds the position where an element from the
// left subarray should be inserted into the right subarray.
// Returns the index in the right subarray.
int gallop_right(int key, int arr[], int start, int length, int hint) {
    int ofs = 1;
    int last_ofs = 0;

    if (key < arr[start + hint]) { // key could be before hint
        int max_ofs = hint + 1;
        while (ofs < max_ofs && key < arr[start + hint - ofs]) {
            last_ofs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) {
                ofs = max_ofs;
            }
        }
        if (ofs > max_ofs) {
            ofs = max_ofs;
        }
        int tmp = last_ofs;
        last_ofs = hint - ofs;
        ofs = hint - tmp;
    } else { // key must be after hint
        int max_ofs = length - hint;
        while (ofs < max_ofs && key >= arr[start + hint + ofs]) {
            last_ofs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0) {
                ofs = max_ofs;
            }
        }
        if (ofs > max_ofs) {
            ofs = max_ofs;
        }
        last_ofs += hint;
        ofs += hint;
    }

    last_ofs++;
    while (last_ofs < ofs) {
        int m = last_ofs + ((ofs - last_ofs) >> 1);
        if (key < arr[start + m]) {
            ofs = m;
        } else {
            last_ofs = m + 1;
        }
    }
    return ofs;
}

// Function to merge two sorted subarrays using galloping mode when appropriate.
void merge(int arr[], int left, int mid, int right, int* temp_arr, int* min_gallop) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    // Copy the left and right subarrays into temporary arrays.
    memcpy(temp_arr, arr + left, len1 * sizeof(int));

    int* left_arr = temp_arr;
    int* right_arr = arr + mid + 1;

    int i = 0;      // Index for the left subarray.
    int j = 0;      // Index for the right subarray.
    int k = left;   // Index for the merged subarray in 'arr'.

    int gallop = *min_gallop;

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
void merge_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr, int* min_gallop) {
    while (*stack_size > 1) {
        int n = *stack_size - 2;
        if ((n > 0 && stack[n - 1].length <= stack[n].length + stack[n + 1].length) ||
            (n > 1 && stack[n - 2].length <= stack[n - 1].length + stack[n].length)) {
            if (stack[n - 1].length < stack[n + 1].length) {
                n--;
            }
            merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
                  stack[n + 1].start + stack[n + 1].length - 1, temp_arr, min_gallop);
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
    int min_gallop = MIN_GALLOP;
    while (*stack_size > 1) {
        int n = *stack_size - 2;
        if (n > 0 && stack[n - 1].length < stack[n + 1].length) {
            n--;
        }
        merge(arr, stack[n].start, stack[n].start + stack[n].length - 1,
              stack[n + 1].start + stack[n + 1].length - 1, temp_arr, &min_gallop);
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
    int min_gallop = MIN_GALLOP;

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

        merge_collapse(arr, stack, &stack_size, temp_arr, &min_gallop);

        start = end + 1;
    }

    merge_force_collapse(arr, stack, &stack_size, temp_arr);
}