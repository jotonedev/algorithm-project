from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "randomized_quick_sort",
    sources=["randomized_quick_sort.pyx", "sort.c"],
    include_dirs=["."]
)

setup(
    name="randomized_quick_sort",
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