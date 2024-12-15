
static inline void swap(int *a, int k, int l){
    int temp = a[k];
    a[k] = a[l];
    a[l] = temp;
}

// Partition function to rearrange the elements around the pivot
int partition(int *a, int i, int j) {
    //assert(i < j);
    int k = i;            // index of the pivot's final position after the partition function completes         
    int pivot = a[j - 1]; // Choose the last element as the pivot

    for (int l = i; l < j; l++) {// Exclude pivot from comparisons
        if (a[l] <= pivot) {
            swap(a, k, l);// Place smaller elements on the left
            k++;
        }
    }
    return k - 1; // Return the pivot's final position
}

int randomQ(int i, int j) {
    return i + rand() % (j - i + 1); // Generate a random number between i and j
}

int randomized_partition(int *a, int i, int j) {
    int x = randomQ(i, j - 1); // Select a random index between i and j-1
    swap(a, j - 1, x);         // Swap it with the last element
    return partition(a, i, j); // Perform partitioning
}

void randomized_quick_sort(int *a, int i, int j) {
    if (i < j) {
        int q = randomized_partition(a, i, j);
        randomized_quick_sort(a, i, q - 1);
        randomized_quick_sort(a, q + 1, j);
    }
}