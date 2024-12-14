import argparse
import gc
import importlib.util
import logging
import subprocess
import sys
from dataclasses import dataclass
from math import exp, log
from pathlib import Path
from random import randint
from statistics import median, mean, stdev
from typing import Callable, Iterator

import numpy as np
import pandas as pd
import psutil
import tqdm


@dataclass
class Algorithm:
    """Data class to store the algorithm name and the function to call"""
    name: str
    function: callable
    path: Path


def setup_logger(verbose: bool) -> None:
    """
    Set up the logging output format and level

    :param verbose: If True, set the log level to DEBUG, otherwise set it to INFO
    """
    log_level = logging.DEBUG if verbose else logging.INFO
    handler = logging.StreamHandler()
    handler.setStream(tqdm.tqdm)  # <-- important
    logging.basicConfig(
        level=log_level,
        format='[%(asctime)s] [%(levelname)s] %(message)s',  # change how logs are displayed
        datefmt='%Y-%m-%d %H:%M:%S',
        handlers=[handler]
    )


def generate_input_data(
        length: int,
        min_val: int,
        max_val: int,
        ensure_max_presence: bool = False,
) -> np.ndarray:
    """
    Generate a numpy array of random numbers

    :param length: The length of the array
    :param min_val: The minimum value
    :param max_val: The maximum value
    :param ensure_max_presence: If True, ensure that the maximum value is present in the array
    :return: A numpy array of random numbers
    """
    data = np.random.randint(min_val, max_val + 1, size=length, dtype=np.intc)
    if ensure_max_presence:
        data[randint(0, length - 1)] = max_val
    return data


# noinspection DuplicatedCode
def load_algorithms(input_dirs: list[Path]) -> list[Algorithm]:
    """
    Load the sorting algorithms from the input directories

    :param input_dirs: A list of input directories
    :return: A list of Algorithm data classes
    """
    algorithms: list[Algorithm] = []

    for dir_path in input_dirs:
        # make sure the path is absolute
        dir_path = dir_path.resolve()
        # check if the directory exists
        if not dir_path.exists() or not dir_path.is_dir():
            continue

        # check if the setup.py file exists (needed to compile the Cython extension)
        setup_py = dir_path / 'setup.py'
        if not setup_py.exists():
            continue

        # Build the Cython module
        build_cmd = [sys.executable, str(setup_py), 'build_ext', '--inplace']
        logging.debug(f"Compiling Cython extension in {dir_path}")
        try:
            subprocess.check_call(build_cmd, cwd=dir_path)
        except subprocess.CalledProcessError as e:
            logging.error(f"Failed to compile the Cython extension: {e}")
            exit(1)

        # Look for the compiled module to load into the process
        # .pyd for Windows, .so for Unix
        for ext in ['.pyd', '.so']:
            # find the module file
            module_path = None
            module_name = None
            for file in dir_path.glob(f"*{ext}"):
                module_path = file
                module_name = file.stem.split('.')[0]
                break

            # load the module
            spec = importlib.util.spec_from_file_location(module_name, module_path)
            if spec is None:
                continue
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)

            # Get all attributes from the module
            for attr in dir(module):
                # find the sorting function by checking the attribute name
                if attr.startswith('py_'):
                    algorithms.append(Algorithm(
                        name=attr.replace('py_', ''),
                        function=getattr(module, attr),
                        path=dir_path
                    ))
                    # break the loop to avoid adding the same algorithm multiple times
                    break

    return algorithms


def create_results_dataframe() -> pd.DataFrame:
    """Create a DataFrame to store the results of the benchmark"""
    return pd.DataFrame(
        {
            "size": pd.Series(dtype=int),
            "min_val": pd.Series(dtype=int),
            "max_val": pd.Series(dtype=int),
            "time": pd.Series(dtype=float),
            "resolution": pd.Series(dtype=int),
            "stdev": pd.Series(dtype=float)
        },
    )


def cpu_warmup(algorithm: Algorithm) -> None:
    """Warm up the CPU clock by running the algorithm with random input data"""
    for _ in range(5):
        algorithm.function(generate_input_data(100, 0, 100), 100)


def collect_results(
        generator: Callable[[], np.ndarray],
        algorithm: Algorithm,
        repetitions: int = 5,
) -> tuple[int, int, int]:
    """Collect the results from the generator and return the median"""
    results = []
    resolutions = []

    # collect old garbage before disabling the garbage collector
    gc.collect()
    gc.disable()
    for _ in range(repetitions):
        data = generator()
        exec_time, res = algorithm.function(data, len(data))
        results.append(exec_time)
        resolutions.append(res)
    # enable the garbage collector
    gc.enable()

    return median(results), round(mean(resolutions), 3), round(stdev(results), 3)


def sample_generator(
        samples: int,
        min_val: int,
        max_val: int,
        linear: bool = False,
) -> Iterator[tuple[int, int]]:
    """Generate samples for the benchmark"""
    if linear:
        scaling_factor = int((max_val - min_val) / samples)
        if scaling_factor <= 0:
            scaling_factor = 1
        # generate samples linearly
        for i in range(samples):
            yield i, min_val + (i * scaling_factor)
    else:
        scaling_factor = exp(log(max_val / min_val) / samples)
        # generate samples exponentially
        for i in range(samples):
            yield i, int(min_val * (scaling_factor ** i))


# noinspection DuplicatedCode
def benchmark_algorithm_by_length(
        algorithm: Algorithm,
        repetitions: int = 5,
        samples: int = 100,
        min_val: int = 1,
        max_val: int = 100_000,
        min_length: int = 100,
        max_length: int = 100_000,
        linear: bool = False,
) -> pd.DataFrame:
    """
    Benchmark the algorithm with random input data by varying the length of the input data

    :param algorithm: The algorithm data class
    :param repetitions: The number of repetitions to run for each sample
    :param samples: The number of samples to run for each input size
    :param min_val: The minimum value of the input data
    :param max_val: The maximum value of the input data (must be greater than min_val)
    :param min_length: The minimum length of the input data
    :param max_length: The maximum length of the input data
    :param linear: If True, use linear scaling, otherwise use exponential scaling
    :return: A DataFrame containing the results of the benchmark
    """
    logging.info(f"Benchmarking algorithm {algorithm.name} by length")

    # Create a DataFrame to store the results of execution times in nanoseconds
    results = create_results_dataframe()

    # Generate the samples
    generator = sample_generator(samples, min_length, max_length, linear)

    # warm up the cpu clock
    cpu_warmup(algorithm)

    try:
        for i, length in tqdm.tqdm(generator, desc=f"Benchmarking {algorithm.name}", dynamic_ncols=True, total=samples):
            exec_time, resolution, deviation = collect_results(
                generator=lambda: generate_input_data(length, min_val, max_val),
                algorithm=algorithm,
                repetitions=repetitions
            )
            # Append the result at the end of the DataFrame
            results.loc[i] = {
                "size": length,
                "min_val": min_val,
                "max_val": max_val,
                "time": exec_time,
                "resolution": resolution,
                "stdev": deviation
            }

    except KeyboardInterrupt:
        logging.error("Benchmarking was interrupted")
        return results

    return results


# noinspection DuplicatedCode
def benchmark_algorithm_by_max(
        algorithm: Algorithm,
        repetitions: int = 5,
        samples: int = 100,
        length: int = 10_000,
        min_val: int = 10,
        max_val: int = 1_000_000,
        linear: bool = False,
) -> pd.DataFrame:
    """
    Benchmark the algorithm with random input data by varying the maximum value of the input data

    :param algorithm: The Algorithm data class
    :param repetitions: The number of repetitions to run for each sample
    :param samples: The number of samples to run for each input size
    :param length: The length of the input data
    :param min_val: The minimum value of the input data
    :param max_val: The maximum value of the input data (must be greater than min_val)
    :param linear: If True, use linear scaling, otherwise use exponential scaling

    :return: A DataFrame containing the results of the benchmark
    """
    logging.info(f"Benchmarking algorithm {algorithm.name} by max value")

    # Create a DataFrame to store the results of execution times in nanoseconds
    results = create_results_dataframe()

    # Generate the samples
    generator = sample_generator(samples, min_val, max_val, linear)

    # warm up the cpu clock to avoid a cold start
    cpu_warmup(algorithm)

    try:
        for i, var_max in tqdm.tqdm(generator, desc=f"Benchmarking {algorithm.name}", dynamic_ncols=True, total=samples):
            exec_time, resolution, deviation = collect_results(
                generator=lambda: generate_input_data(length, min_val, var_max),
                algorithm=algorithm,
                repetitions=repetitions
            )

            results.loc[i] = {
                "size": length,
                "min_val": min_val,
                "max_val": var_max,
                "time": exec_time,
                "resolution": resolution,
                "stdev": deviation
            }
    except KeyboardInterrupt:
        # allows returning the results if the benchmarking was interrupted
        logging.warning("Benchmarking was interrupted")

    return results


def main(
        input_folders: list[Path],
        output: Path,
        repetitions: int,
        samples: int,
        verbose: bool,
        linear: bool = False,
        by_max: bool = False,
):
    """Run the benchmarking tool"""
    setup_logger(verbose)

    # Increase process priority to avoid interruptions
    # check if it is windows
    process = psutil.Process()
    if sys.platform == 'win32':
        process.nice(psutil.HIGH_PRIORITY_CLASS)
    elif sys.platform == 'linux':
        # set the process to the highest priority if permission is granted
        try:
            process.nice(-20)
            logging.info("Set process nice -20")
        except psutil.AccessDenied:
            logging.warning("Could not set the process to the highest priority")
            process.nice(1)
            logging.info("Set process nice 1")
    # pin the process to the last core
    process.cpu_affinity([psutil.cpu_count(logical=False) - 1])

    # Create an output directory if it does not exist
    output.mkdir(parents=True, exist_ok=True)

    # Load the algorithms from the input directory
    algorithms: list[Algorithm] = load_algorithms(input_folders)

    # Run the benchmark for each algorithm
    for algorithm in tqdm.tqdm(algorithms, desc="Benchmarking Algorithms", dynamic_ncols=True, total=len(algorithms)):
        # Create the filename based on the parameters used
        os_type = "windows" if sys.platform == 'win32' else "linux"
        run_type = "linear" if linear else "exponential"
        bench_type = "max" if by_max else "length"
        filename = f"{algorithm.name}_{samples}_{repetitions}_{run_type}_{bench_type}_{os_type}.csv"

        if by_max:
            results = benchmark_algorithm_by_max(
                algorithm=algorithm,
                repetitions=repetitions,
                samples=samples,
                linear=linear
            )
        else:
            results = benchmark_algorithm_by_length(
                algorithm=algorithm,
                repetitions=repetitions,
                samples=samples,
                linear=linear
            )

        # Save the results to a CSV file
        results.to_csv(output / filename, index=False)

    logging.info("Benchmarking tool completed successfully")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog="Sorting Algorithms Benchmark",
        description='Benchmarking tool for the project',
    )
    parser.add_argument(
        "-i", "--input",
        help="Input directory for algorithm inputs",
        type=Path,
        required=True,
        action="append"
    )
    parser.add_argument(
        "-o", "--output",
        help="Output directory for benchmark results",
        type=Path,
        default=Path('.results/')
    )
    parser.add_argument(
        "-s", "--samples",
        help="Number of samples to get",
        type=int,
        default=100
    )
    parser.add_argument(
        "-r", "--repetitions",
        help="Number of repetitions for each input length to run",
        type=int,
        default=5
    )
    parser.add_argument(
        "-v", "--verbose",
        help="Increase output verbosity",
        action="store_true",
        default=False
    )
    parser.add_argument(
        "--linear",
        help="Use linear scaling instead of exponential",
        action="store_true",
        default=False
    )
    parser.add_argument(
        "--by-max",
        help="Benchmark by varying the maximum value of the input data",
        action="store_true",
        default=False
    )

    args = parser.parse_args()

    main(
        input_folders=args.input,
        output=args.output,
        samples=args.samples,
        repetitions=args.repetitions,
        verbose=args.verbose,
        linear=args.linear,
        by_max=args.by_max
    )
