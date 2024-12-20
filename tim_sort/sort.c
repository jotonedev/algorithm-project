#include <stdio.h>
#include <stdlib.h>

#define RUN 32

// This function sorts array from left index to right index which is of size atmost RUN
void insertionSort(int *a, int left, int right){
    int i, j, temp;
    for(i = left + 1; i <= right; i++){
        temp = a[i];
        j = i - 1;
        while(j >= left && a[j] > temp){
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = temp;
    }
}

// Merge function merges the sorted runs
void merge(int *a, int l, int m, int r) {
    int len1 = m - l + 1;
    int len2 = r - m;
    int *left = (int*)malloc(len1 * sizeof(int));
    int *right = (int*)malloc(len2 * sizeof(int));
    
    if (!left || !right) {
        printf("Errore: allocazione memoria fallita\n");
        exit(EXIT_FAILURE);
    }

    int i=0, j=0, k=l;
    for (i = 0; i < len1; i++){    
        left[i] = a[l + i];
    }
    for (i = 0; i < len2; i++){
        right[i] = a[m + 1 + i];
    }

    // After comparing, we merge those two arrays into the larger sub-array
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            a[k] = left[i];
            i++;
        } else {
            a[k] = right[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements of left, if any
    while (i < len1) {
        a[k] = left[i];
        k++;
        i++;
    }
    
    // Copy remaining elements of right, if any
    while (j < len2) {
        a[k] = right[j];
        k++;
        j++;
    }

    // Free dynamically allocated memory
    free(left);
    free(right);
}

// Iterative Timsort function to sort the array[0...n-1] (similar to merge sort)
void timSort(int *a, int n) {

    // Sort individual subarrays of size RUN
    int i;
    for (i = 0; i < n; i += RUN){
        insertionSort(a, i, (i + RUN - 1 < n - 1) ? (i + RUN - 1) : (n - 1));
    }
    // Start merging from size RUN (or 32)
    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = (left + 2 * size - 1 < n - 1) ? (left + 2 * size - 1) : (n - 1);

            if (mid < right){
                merge(a, left, mid, right);
            }
        }
    }
}
