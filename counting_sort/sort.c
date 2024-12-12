void counting_sort(int n, int data[], int out[], int count[], int k) {
    for (int j = 0; j < n; j++) {
        count[data[j]]++;
    }

    for (int i = 1; i <= k; i++) {
        count[i] += count[i - 1];
    }

    for (int j = n - 1; j >= 0; j--) { 
        out[count[data[j]] - 1] = data[j];
        count[data[j]]--;
    }
}
