from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    "quick3Way",
    sources=["q3sort.pyx", "quick3Way.c"],
    include_dirs=["."]
)

setup(
    name="quick3Way",
    ext_modules=cythonize(ext, compiler_directives={
        'language_level': 3,
    })
)