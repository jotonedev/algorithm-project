#ifndef TIM_SORT_H
#define TIM_SORT_H

// Threshold for switching to insertion sort (tuned value).
#define THRESHOLD 64

// Initial minimum run length (tuned value).
#define MIN_MERGE 32

// Structure to represent a run in the stack.
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