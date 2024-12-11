from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "quickSort",
    sources=["qsort.pyx", "quickSort.c"],
    include_dirs=["."]
)

setup(
    name="quickSort",
    ext_modules=cythonize(ext, compiler_directives={
        'language_level': 3,
    })
)