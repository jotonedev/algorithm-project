
#include <stdio.h>
#include <stdlib.h>

int findK(int *a, int n) {
    int key = a[0];
    for (int i = 1; i < n; i++) {
        if (key < a[i]) {
            key = a[i];
        }
    }
    return key; // Missing return statement fixed
}

int* countingSort(int *a, int n, int k) {
    int* B = (int*)malloc(n * sizeof(int)); // Output array
    int* C = (int*)calloc(k + 1, sizeof(int)); // Count array (initialize to 0)

    // Step 1: Count occurrences of each element
    for (int j = 0; j < n; j++) {
        C[a[j]]++;
    }

    // Step 2: Cumulative sum of counts
    for (int i = 1; i <= k; i++) {
        C[i] += C[i - 1];
    }

    // Step 3: Build the sorted output array
    for (int j = n - 1; j >= 0; j--) { // Start from the last element
        B[C[a[j]] - 1] = a[j];
        C[a[j]]--;
    }

    // Free the count array
    free(C);

    // Return the sorted array
    return B;
}