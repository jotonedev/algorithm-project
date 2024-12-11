cdef extern from "quick3Way.h":
    void quickSort3Way(int *a, int i, int j)

def py_quick3Way_sort(int[:] a, int i, int j):
    quickSort3Way(&a[0], i, j)

