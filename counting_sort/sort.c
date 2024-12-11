
int findK(int *a, int n) {
    int key = a[0];
    for (int i = 1; i < n; i++) {
        if (key < a[i]) {
            key = a[i];
        }
    }
    return key; 
}

void countingSort(int *a, int n, int k) {
    int* B = (int*)malloc(n * sizeof(int));
    int* C = (int*)calloc(k + 1, sizeof(int));


    for (int i = 0; i <= k; i++) {
        C[i] = 0;
    }
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
