//#include <assert.h>

// Static inline function to swap two integers by pointer
static inline void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void static partition_3way(int *a, int i, int j, int *k, int *l) {
    // The function partitions the array into three parts:
    // - Elements less than the pivot
    // - Elements equal to the pivot
    // - Elements greater than the pivot
    
    //assert(i < j);

    int pivot = a[j - 1];       // Choose the last element as the pivot
    int p1 = i, p2 = i, p3 = i;

    while (p3 < j) {
         // Case 1: Current element is less than the pivot
        if (a[p3] < pivot) {
            swap(&a[p3], &a[p2]);// Move current element to the <= pivot region
            swap(&a[p2], &a[p1]);// Move boundary for < pivot
            p1++;
            p2++;
            p3++;

          // Case 2: Current element is equal to the pivot
        } else if (a[p3] == pivot) {
            swap(&a[p3], &a[p2]);// Move current element to the <= pivot region
            p2++;
            p3++;

          // Case 3: Current element is greater than the pivot
        } else {
            p3++;// Simply move to the next element
        }
    }

    // After the loop:
    // - Elements in range [i, p1) are < pivot
    // - Elements in range [p1, p2) are == pivot
    // - Elements in range [p2, j) are > pivot

    *k = p1;// Set `k` to the start of the == pivot region
    *l = p2;// Set `l` to the end of the == pivot region
}

// Recursive 3-way QuickSort function
void quick_3way_sort(int *a, int i, int j) {
    if (j - i <= 1) {
        // Base case: If the subarray size is 0 or 1, it is already sorted
        return;
    }

    int k, l;
     // Partition the array into three parts 
    partition_3way(a, i, j, &k, &l);

    quick_3way_sort(a, i, k);// Recursively sort the < pivot region
    quick_3way_sort(a, l, j);// Recursively sort the > pivot region
}