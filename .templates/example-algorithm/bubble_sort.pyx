from time import perf_counter_ns


cdef extern from "sort.h":
    void bubble_sort(int *arr, int n)

cdef int verify(int *a, int n):
    cdef int i
    for i in range(1, n):
        if a[i] < a[i - 1]:
            return 0
    return 1

cdef unsigned long long bubble_sort_benchmark(int* a, int n):
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0

    start_ns = perf_counter_ns()
    bubble_sort(a, n)
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

# the function must have the specified signature
def py_bubble_sort(int[:] arr, int n) -> tuple[int, int]:
    return bubble_sort_benchmark(&arr[0], n), get_resolution()
