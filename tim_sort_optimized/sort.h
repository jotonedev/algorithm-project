#ifndef TIM_SORT_H
#define TIM_SORT_H

#include <stdbool.h>

// Define a threshold for switching to insertion sort.
#define THRESHOLD 64

// Define a minimum size for merging.
#define MIN_MERGE 32

// Define the maximum number of pending runs on the stack.
// This needs to be large enough to handle worst-case scenarios but small enough to fit in cache.
#define MAX_PENDING_RUNS 85

// Structure to represent a run in the array.
typedef struct {
    int start;  // Start index of the run.
    int length; // Length of the run.
} Run;

// Structure to manage the stack of pending runs.
typedef struct {
    Run stack[MAX_PENDING_RUNS];
    int num_runs;  // Number of runs currently on the stack.
} RunStack;

void insertion_sort(int arr[], int left, int right);
void merge(int arr[], int left, int mid, int right, int* temp_arr);
int calculate_minrun(int n);
void tim_sort(int arr[], int n, int* temp_arr, RunStack* run_stack);
int count_run(int arr[], int start, int n);
void extend_run_and_sort(int arr[], int start, int* end, int n, int minrun);
void push_run(RunStack* stack, int start, int length);
void merge_collapse(int arr[], RunStack* stack, int* temp_arr);
void merge_runs(int arr[], RunStack* stack, int a, int b, int* temp_arr);

#endif