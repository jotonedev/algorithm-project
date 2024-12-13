from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "quick3Way",
    sources=["quick_3_way_sort.pyx", "quick_3_way_sort.c"],
    include_dirs=["."]
)

setup(
    name="quick3Way",
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