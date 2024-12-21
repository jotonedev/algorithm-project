from time import perf_counter_ns
from libc.stdlib cimport malloc, free


cdef extern from "sort.h":
    struct Run:
        int start
        int length

    struct RunStack:
        Run* stack
        int nums_runs

    void tim_sort(int arr[], int n, int* temp_arr, RunStack* run_stack)

cdef int verify(int *a, int n):
    cdef int i
    for i in range(1, n):
        if a[i] < a[i - 1]:
            return 0
    return 1

# noinspection DuplicatedCode
cdef unsigned long long tim_sort_benchmark(int* a, int n):
    # declare array b of the same size as a
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0

        int *temp_arr = <int *>malloc(n * sizeof(int))
        RunStack *runs = <RunStack *>malloc(sizeof(RunStack))

    # run the benchmark
    start_ns = perf_counter_ns()
    tim_sort(a, n, temp_arr, runs)
    end_ns = perf_counter_ns()

    if verify(a, n) == 0:
        raise ValueError("Array is not sorted")

    # free the memory allocated for the merge state
    free(<void *>temp_arr)
    free(<void *>runs)

    return end_ns - start_ns

# leave this function as is
cdef unsigned long long get_resolution():
    cdef:
        unsigned long long start_ns = 0
        unsigned long long end_ns = 0
    start_ns = perf_counter_ns()
    end_ns = perf_counter_ns()
    return end_ns - start_ns

def py_tim_sort_optimized(int[:] a, int n) -> tuple[int, int]:
    return tim_sort_benchmark(&a[0], n), get_resolution()

