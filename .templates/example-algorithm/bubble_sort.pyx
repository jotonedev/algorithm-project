# distutils: language=c
# cython: language_level=3

cdef extern from "sort.h":
    void bubble_sort(int *arr, int n)

def py_sort_bubble_sort(int[:] arr, int n):
    bubble_sort(&arr[0], n)
