//#include <assert.h>

void static partition_3way(int *a, int i, int j, int *k, int *l) {
    //assert(i < j);

    int pivot = a[j - 1]; 
    int p1 = i, p2 = i, p3 = i;

    while (p3 < j) {
        if (a[p3] < pivot) {
            int temp = a[p3];
            a[p3] = a[p2];
            a[p2] = a[p1];
            a[p1] = temp;
            p1++;
            p2++;
            p3++;
        } else if (a[p3] == pivot) {
            int temp = a[p3];
            a[p3] = a[p2];
            a[p2] = temp;
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