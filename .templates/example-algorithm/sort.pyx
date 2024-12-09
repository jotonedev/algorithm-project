# distutils: language=c
# cython: language_level=3

cdef extern from "sorting_algorithm.h":
    void sort_test(int *arr, int n)

def py_sort_test(int[:] arr, int n):
    sort_test(&arr[0], n)
