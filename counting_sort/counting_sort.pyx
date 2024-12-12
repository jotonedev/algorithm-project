from time import perf_counter_ns
from libc.stdlib cimport malloc, calloc, free


cdef extern from "sort.h":
    void counting_sort(int n, int *a, int *b, int *c, int max_val)

cdef int find_max(int *a, int n):
    cdef int i
    cdef int max_val = a[0]
    for i in range(1, n):
        if a[i] > max_val:
            max_val = a[i]
    return max_val

cdef int verify(int *a, int n):
    cdef int i
    for i in range(1, n):
        if a[i] < a[i - 1]:
            return 0
    return 1

cdef unsigned long long counting_sort_benchmark(int* a, int n):
    # declare array b of the same size as a
    cdef:
        int *b = <int *> malloc(n * sizeof(int))
        int max_val = find_max(a, n)
        int *c = <int *> calloc(max_val + 1, sizeof(int))

        unsigned long long start_ns = 0
        unsigned long long end_ns = 0

    start_ns = perf_counter_ns()
    counting_sort(n, a, b, c, max_val)
    end_ns = perf_counter_ns()

    if verify(b, n) == 0:
        raise ValueError("Array is not sorted")

    free(<void*>b)
    free(<void*>c)

    return end_ns - start_ns

# leave this function as is
cdef unsigned long long get_resolution():
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0
    start_ns = perf_counter_ns()
    end_ns = perf_counter_ns()
    return end_ns - start_ns

# the function must have the specified signature
def py_counting_sort(int[:] a, int n) -> tuple[int, int]:
    return counting_sort_benchmark(&a[0], n), get_resolution()
