# Algorithms Project Report

Implementation, time complexity analysis, and benchmarking of the following algorithms:

- [ ] Quick Sort
- [ ] Quick 3 Way Sort
- [ ] Counting Sort
- [ ] Tim Sort

## Project Structure

- [Quick Sort](/quick_sort)
- [Quick 3 Way Sort](/quick_3_way_sort)
- [Counting Sort](/counting_sort)
- [Tim Sort](/tim_sort)
- [Project report](/docs)

## How to run

First of all, you need to install the dependencies:

```bash
pip install -r requirements.txt
```

Then, you can run the benchmarking tool:
```bash
python benchmark.py -i <input>
```

### Arguments

```bash
usage: Sorting Algorithms Benchmark [-h] -i INPUT [-o OUTPUT] [-s SAMPLES] [-r REPETITIONS] [--min MIN] [--max MAX] [-v] [--linear] [--show-linear] [--show-quadratic] [--show-nlogn]

Benchmarking tool for the project

options:
  -h, --help            show this help message and exit
  -i INPUT, --input INPUT                     Input directory for algorithm inputs
  -o OUTPUT, --output OUTPUT                  Output directory for benchmark results
  -s SAMPLES, --samples SAMPLES               Number of samples to get
  -r REPETITIONS, --repetitions REPETITIONS   Number of repetitions for each input length to run
  --min MIN             Minimum size of the input data. Default to 1
  --max MAX             Maximum size of the input data. Default to 10000
  -v, --verbose         Increase output verbosity
  --linear              Use linear scaling instead of exponential
  --show-linear         Show the linear function in the plot
  --show-quadratic      Show the quadratic function in the plot
  --show-nlogn          Show the n*log(n) function in the plot
```

### How benchmark.py works

1. Set process priority to the highest available to avoid interrupts
2. Pin the process to one CPU core to avoid context switching
3. Load the algorithm to benchmark and compile it using Cython
4. Start the benchmark part, runs the algorithm with different input sizes, repeating it multiple times
5. Save the results to a CSV file
6. Generate a plot for the results

### What is Cython

Cython is a static compiler for both the Python programming languages.
It allows to write C extensions for Python, which can be used to optimize the code.

By using Cython, we can write the algorithm in C and compile it to a Python module, 
which can be imported and used in the benchmarking tool.

### Why using Cython

Because it allows using the flexibility of python to write the benchmarking tool 
and use the speed and predictability of C to run the algorithms.
