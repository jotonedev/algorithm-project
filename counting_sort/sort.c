#include <stdlib.h>

int find_max(int *a, int n) {
    int key = a[0];
    for (int i = 1; i < n; i++) {
        if (key < a[i]) {
            key = a[i];
        }
    }
    return key; 
}

void counting_sort(int *a, int n) {
    int k = find_max(a, n);

    int* B = malloc(n * sizeof(int));
    int* C = calloc(k + 1, sizeof(int));

    for (int j = 0; j < n; j++) {
        C[a[j]]++;
    }

    for (int i = 1; i <= k; i++) {
        C[i] += C[i - 1];
    }

    for (int j = n - 1; j >= 0; j--) { 
        B[C[a[j]] - 1] = a[j];
        C[a[j]]--;
    }

    for (int i = 0; i < n; i++) {
        a[i] = B[i];
    }

    free(B);
    free(C);
}
