from time import perf_counter_ns


cdef extern from "sort.h":
    void quick_sort(int *a, int i, int j)

cdef int verify(int *a, int n):
    cdef int i
    for i in range(1, n):
        if a[i] < a[i - 1]:
            return 0
    return 1

cdef unsigned long long quick_sort_benchmark(int* a, int n):
    # declare array b of the same size as a
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0

    start_ns = perf_counter_ns()
    quick_sort(a, 0, n)
    end_ns = perf_counter_ns()

    if verify(a, n) == 0:
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


def py_quick_sort(int[:] a, int n) -> tuple[int, int]:
    return quick_sort_benchmark(&a[0], n), get_resolution()
