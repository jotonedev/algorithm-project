cdef extern from "sort.h":
    void quickSort3Way(int *a, int i, int j)

cdef int verify(int *a, int n):
    cdef int i
    for i in range(1, n):
        if a[i] < a[i - 1]:
            return 0
    return 1

cdef unsigned long long quick3Way_sort_benchmark(int* a, int i, int j):
    # declare array b of the same size as a
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0

    start_ns = perf_counter_ns()
    quickSort3Way(a, i, j)
    end_ns = perf_counter_ns()

    if verify(b, n) == 0:
        raise ValueError("Array is not sorted")

    return end_ns - start_ns

# leave this function as is
cdef unsigned long long get_resolution():
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0
    start_ns = perf_counter_ns()
    end_ns = perf_counter_ns()
    return end_ns - start_ns

def py_quick3Way_sort(int[:] a, int i, int j):
    quickSort3Way(&a[0], i, j)

