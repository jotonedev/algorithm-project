#ifndef TIM_SORT_H
#define TIM_SORT_H

// Define a threshold for switching to insertion sort.
// For very small subarrays, insertion sort is more efficient than merge sort.
#define THRESHOLD 64

// Define a structure to represent a run in the stack.
// A "run" is a sorted subsequence within the array.
typedef struct {
    int start;  // Start index of the run.
    int length; // Length of the run.
} Run;

void insertion_sort(int arr[], int left, int right);
void merge(int arr[], int left, int mid, int right, int* temp_arr);
int calculate_minrun(int n);
int find_run_length(int arr[], int start, int n);
void merge_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr);
void merge_force_collapse(int arr[], Run stack[], int* stack_size, int* temp_arr);
void tim_sort(int arr[], int n, int temp_arr[], Run stack[]);

#endif