#ifndef TIM_SORT_H
#define TIM_SORT_H

// Define a threshold for switching to insertion sort.
// For very small subarrays, insertion sort is more efficient than merge sort.
#define THRESHOLD 64

void insertion_sort(int arr[], int left, int right);
void merge(int arr[], int left, int mid, int right, int* temp_arr);
int calculate_minrun(int n);
void tim_sort(int arr[], int n, int temp_arr[]);

#endif