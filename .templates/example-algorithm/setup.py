from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "sort",
    sources=["sort.pyx", "sorting_algorithm.c"],
    include_dirs=["."]
)

setup(
    name="sorting_algorithm",
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