from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "quick_sort_random_pivot",
    sources=["randomized_quick_sort.pyx", "sort.c"],
    include_dirs=["."]
)

setup(
    name="quick_sort_random_pivot",
    ext_modules=cythonize(ext, compiler_directives={
        'language_level': 3,
        'boundscheck': False,
        'wraparound': False,
        'initializedcheck': False,
        'overflowcheck.fold': False,
        'overflowcheck': False,
        'cdivision': True,
    })
)