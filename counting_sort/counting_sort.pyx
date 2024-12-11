cdef extern from "sort.h":
    void counting_sort(int *a, int n)

def py_counting_sort(int[:] a, int n):
    counting_sort(&a[0], n)

