//#include <assert.h>

static inline void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void static partition_3way(int *a, int i, int j, int *k, int *l) {
    //assert(i < j);

    int pivot = a[j - 1]; 
    int p1 = i, p2 = i, p3 = i;

    while (p3 < j) {
        if (a[p3] < pivot) {
            swap(&a[p3], &a[p2]);
            swap(&a[p2], &a[p1]);
            p1++;
            p2++;
            p3++;
        } else if (a[p3] == pivot) {
            swap(&a[p3], &a[p2]);
            p2++;
            p3++;
        } else {
            p3++;
        }
    }

    *k = p1;
    *l = p2;
}

void quick_3way_sort(int *a, int i, int j) {
    if (j - i <= 1) {
        return;
    }

    int k, l; 
    partition_3way(a, i, j, &k, &l);

    quick_3way_sort(a, i, k);
    quick_3way_sort(a, l, j);
}