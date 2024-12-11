cdef extern from "q.h":
    void quickSort(int *a, int i, int j)

def py_counting_sort(int[:] a, int i, int j):
    quickSort(&a[0], i, j) 

