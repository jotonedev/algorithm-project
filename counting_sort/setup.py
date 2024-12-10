from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "countingsort",
    sources=["sort.pyx", "countingSort.c"],
    include_dirs=["."]
)

setup(
    name="countingSort",
    ext_modules=cythonize(ext, compiler_directives={
        'language_level': 3,
    })
)