#include <assert.h>

// Partition function to rearrange the elements around the pivot
int partition(int *a, int i, int j) {
    assert(i < j);
    int k = i;
    int pivot = a[j - 1]; // Choose the last element as the pivot

    for (int l = i; l < j; l++) {
        if (a[l] <= pivot) {
            // Swap a[k] and a[l]
            int temp = a[k];
            a[k] = a[l];
            a[l] = temp;
            k++;
        }
    }
    return k - 1; // Return the pivot's final position
}

// QuickSort function to sort the subarray a[i:j] (j exclusive)
void quickSort(int *a, int i, int j) {
    // Base case: array of size 0 or 1
    if (j - i <= 1) {
        return;
    }

    // Inductive case: partition the array and sort the partitions
    int k = partition(a, i, j);
    quickSort(a, i, k);   // Sort elements less than or equal to the pivot
    quickSort(a, k + 1, j); // Sort elements greater than the pivot
}