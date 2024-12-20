from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "timSort",
    sources=["tim_sort.pyx", "sort.c"],
    include_dirs=["."]
)

setup(
    name="timSort",
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