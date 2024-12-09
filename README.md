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
python benchmark.py -i INPUT [-o OUTPUT] [-r REPETITIONS] [-l LEN] [--min MIN] [--max MAX] [--step STEP] [-v] [--disable-check-correctness] [--linear] [--no-progress]
```

### Arguments

```bash
-h, --help                                  show this help message and exit
-i INPUT, --input INPUT                     Input directory for algorithm inputs
-o OUTPUT, --output OUTPUT                  Output directory for benchmark results
-r REPETITIONS, --repetitions REPETITIONS   Number of repetitions for each input length to run
-l LEN, --len LEN                           Maximum length of the input data
--min MIN                                   Minimum size of the input data. Default to 1
--max MAX                                   Maximum size of the input data. Default to the length of the input data
--step STEP                                 Step size for the input data
-v, --verbose                               Increase output verbosity
--disable-check-correctness                 Check the correctness of the sorting algorithms
--linear                                    Increase the input data length linearly. Defaults to exponential
--no-progress                               Show a progress bar
```