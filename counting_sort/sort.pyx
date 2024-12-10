cdef extern from "countingsort.h":
    void countingSort(int *a, int n)

def py_counting_sort(int[:] a, int n):
    countingSort(&a[0], n)

