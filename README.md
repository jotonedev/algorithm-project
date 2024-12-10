# Algorithms Project Report

Implementation, time complexity analysis and benchmarking of the following algorithms:

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

```bash
usage: Sorting Algorithms Benchmark [-h] -i INPUT [-o OUTPUT] [-s SAMPLES] [-r REPETITIONS] [--min MIN] [--max MAX] [-v] [--disable-check-correctness]
```

### Arguments

```bash
usage: Sorting Algorithms Benchmark [-h] -i INPUT [-o OUTPUT] [-s SAMPLES] [-r REPETITIONS] [--min MIN] [--max MAX] [-v]

Benchmarking tool for the project

options:
  -h, --help                                    Show this help message and exit
  -i INPUT, --input INPUT                       Input directory for algorithm inputs
  -o OUTPUT, --output OUTPUT                    Output directory for benchmark results
  -s SAMPLES, --samples SAMPLES                 Number of samples to get
  -r REPETITIONS, --repetitions REPETITIONS     Number of repetitions for each input length to run
  --min MIN                                     Minimum size of the input data. Default to 1
  --max MAX                                     Maximum size of the input data. Default to 10000
  -v, --verbose                                 Increase output verbosity
```
