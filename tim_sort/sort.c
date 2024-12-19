#include <stdio.h>
#include <stdlib.h>

#define RUN 32

// This function sorts array from left index to right index which is of size atmost RUN
void insertionSort(int arr[], int left, int right){
    int i, j, temp;
    for(i = left + 1; i <= right; i++){
        temp = arr[i];
        j = i - 1;
        while(j >= left && arr[j] > temp){
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// Merge function merges the sorted runs
void merge(int arr[], int l, int m, int r) {
    int len1 = m - l + 1;
    int len2 = r - m;
    int *left = (int*)malloc(len1 * sizeof(int));
    int *right = (int*)malloc(len2 * sizeof(int));
    
    int i, j, k;
    for (i = 0; i < len1; i++){    
        left[i] = arr[l + i];
    }
    for (i = 0; i < len2; i++){
        right[i] = arr[m + 1 + i];
    }

    i = 0;
    j = 0;
    k = l;
    
    // After comparing, we merge those two arrays into the larger sub-array
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements of left, if any
    while (i < len1) {
        arr[k] = left[i];
        k++;
        i++;
    }
    
    // Copy remaining elements of right, if any
    while (j < len2) {
        arr[k] = right[j];
        k++;
        j++;
    }

    // Free dynamically allocated memory
    free(left);
    free(right);
}

// Iterative Timsort function to sort the array[0...n-1] (similar to merge sort)
void timSort(int arr[], int n) {

    // Sort individual subarrays of size RUN
    int i;
    for (i = 0; i < n; i += RUN){
        insertionSort(arr, i, (i + RUN - 1 < n - 1) ? (i + RUN - 1) : (n - 1));
    }
    // Start merging from size RUN (or 32)
    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = (left + 2 * size - 1 < n - 1) ? (left + 2 * size - 1) : (n - 1);

            if (mid < right){
                merge(arr, left, mid, right);
            }
        }
    }
}

// Utility function to print the Array
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++){
        printf("%d  ", arr[i]);
    }
    printf("\n");
}

// Driver program to test the above function
int main() {
    int arr[] = { -2, 7, 15, -14, 0, 15, 0, 7, -7, -4, -13, 5, 8, -14, 12 };
    int n = sizeof(arr) / sizeof(arr[0]);
    printf("Given Array is\n");
    printArray(arr, n);

    // Function Call
    timSort(arr, n);

    printf("After Sorting Array is\n");
    printArray(arr, n);
    return 0;
}
