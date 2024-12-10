import sys
import gc
import time
import tqdm
import argparse
import logging
import subprocess
import importlib.util

import polars as pl
import numpy as np

from math import exp, log
from dataclasses import dataclass
from pathlib import Path


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
    logging.basicConfig(
        level=log_level,
        format='[%(asctime)s] [%(levelname)s] %(message)s',  # change how logs are displayed
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def generate_input_data(length: int, min_val: int, max_val: int, *, use_float: bool = False) -> np.ndarray:
    """
    Generate a numpy array of random numbers
    
    :param length: The length of the array
    :param min_val: The minimum value
    :param max_val: The maximum value 
    :param use_float: If True, generate an array of floats, else integers
    :return: A numpy array of random numbers
    """
    if use_float:
        return np.random.uniform(min_val, max_val, size=length).astype(np.float64)
    return np.random.randint(min_val, max_val + 1, size=length, dtype=np.intc)


# noinspection DuplicatedCode
def load_algorithms(input_dirs: list[Path]) -> list[Algorithm]:
    """
    Load the sorting algorithms from the input directories

    :param input_dirs: A list of input directories
    :return: A list of Algorithm data classes
    """
    algorithms: list[Algorithm] = []

    for dir_path in input_dirs:
        # check if the directory exists
        dir_path = dir_path.resolve()
        if not dir_path.is_dir() or not dir_path.exists():
            continue
        
        # check if the setup.py file exists (needed to compile the Cython extension)
        setup_py = dir_path / 'setup.py'
        setup_py = setup_py.resolve()
        if not setup_py.exists():
            continue

        # Build the Cython extension
        build_cmd = [sys.executable, str(setup_py), 'build_ext', '--inplace']
        logging.debug(f"Compiling Cython extension in {dir_path}")
        try:
            # Run the build command ignoring the output
            subprocess.check_call(build_cmd, cwd=dir_path, stdout=subprocess.DEVNULL)
        except subprocess.CalledProcessError as e:
            logging.error(f"Failed to compile the Cython extension: {e}")
            exit(1)

        # Look for the compiled module with the correct name
        module_name = "sort"
        # .pyd for Windows, .so for Unix
        for ext in ['.pyd', '.so']:
            # find a module path
            module_path = None
            for file in dir_path.glob(f"{module_name}*{ext}"):
                module_path = file
                break
            
            # load the module
            spec = importlib.util.spec_from_file_location(module_name, module_path)
            if spec is None:
                continue
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)

            # Get the sorting function
            if hasattr(module, 'py_sort_test'):
                algorithms.append(Algorithm(
                    name="test",
                    function=module.py_sort_test,
                    path=dir_path
                ))
            break

    return algorithms


def check_sorting(data: np.ndarray, result: np.ndarray) -> bool:
    """
    Check the correctness of the sorting algorithm

    :param data: The original input data as NumPy array
    :param result: The sorted data as NumPy array
    :return: True if the data is sorted, False otherwise
    """
    # Use numpy sorting algorithm to check the correctness
    return np.array_equal(result, np.sort(data))


def create_empty_df(i: int) -> pl.DataFrame:
    """
    Create an empty DataFrame
    """
    columns = {f"rep_{n}": pl.Series(dtype=pl.Float64) for n in range(i)}
    return pl.DataFrame(columns)


def benchmark_algorithm(
        algorithm: Algorithm, 
        repetitions: int = 3,
        samples: int = 100,
        min_val: int = 100,
        max_val: int = 1000,
        check_correctness: bool = True,
    ) -> pl.DataFrame:
    """
    Benchmark the algorithm with random input data

    :param algorithm: The Algorithm data class
    :param repetitions: The number of repetitions to run
    :param samples: The number of samples to run
    :param min_val: The minimum value of the input data
    :param max_val: The maximum value of the input data. If None, set it to the length of the input data at each round
    :param check_correctness: If True, check the correctness of the algorithm
    :return: A list of execution times
    """
    logging.info(f"Benchmarking algorithm {algorithm.name}")

    # Create a DataFrame to store the results
    results = create_empty_df(repetitions)

    data_length = min_val
    scaling_factor = exp(log(max_val / min_val) / samples)
    for i in tqdm.tqdm(range(samples), desc=f"Benchmarking {algorithm.name}", dynamic_ncols=True):
        data_length = int(data_length * (scaling_factor ** i))

        # Generate random input data
        data = generate_input_data(data_length, min_val, max_val)

        # Run the algorithm for the given number of repetitions
        values = {}
        for i in range(repetitions):
            # Copy the input data to avoid modifying the original list
            input_data = data.copy()

            # Disable the garbage collector to avoid interference with the execution time
            gc.disable()
            # Measure the execution time (without sleep or other operations)
            start_time = time.process_time()
            result = algorithm.function(input_data, len(input_data))
            end_time = time.process_time()
            gc.enable()

            # Check the correctness of the algorithm
            if check_correctness:
                if not check_sorting(data, result):
                    logging.error(f"Algorithm {algorithm.name} failed the correctness check")
                    raise ValueError(f"Algorithm {algorithm.name} failed the correctness check")

            # Store the execution time
            values[f"rep_{i}"] = end_time - start_time

        # Add the execution times to the DataFrame
        results.vstack(
            pl.DataFrame(values),
            in_place=True
        )

    return results


# noinspection PyShadowingNames
def main(
    input_folders: list[Path],
    output: Path,
    repetitions: int,
    samples: int,
    min_val: int,
    max_val: int | None,
    verbose: bool,
    check_correctness: bool,
):
    """Run the benchmarking tool"""
    setup_logger(verbose)

    # Create an output directory if it does not exist
    output.mkdir(parents=True, exist_ok=True)

    logging.info("Starting the benchmarking tool")

    # Load the algorithms from the input directory
    algorithms: list[Algorithm] = load_algorithms(input_folders)

    # Run the benchmark for each algorithm
    for algorithm in tqdm.tqdm(algorithms, desc="Benchmarking Algorithms", dynamic_ncols=True):
        try:
            results = benchmark_algorithm(
                algorithm=algorithm,
                repetitions=repetitions,
                samples=samples,
                min_val=min_val,
                max_val=max_val,
                check_correctness=check_correctness,
            )
        except KeyboardInterrupt:
            logging.error("Benchmarking tool was interrupted")
            return

        # Save the results to a CSV file
        results.write_csv(output / f"{algorithm.name}.csv")

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
        default=Path('output/')
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
        default=3
    )
    parser.add_argument(
        "--min",
        help="Minimum size of the input data. Default to 1",
        type=int,
        default=100
    )
    parser.add_argument(
        "--max",
        help="Maximum size of the input data. Default to 10000",
        type=int,
        default=1_000
    )
    parser.add_argument(
        "-v", "--verbose",
        help="Increase output verbosity",
        action="store_true",
        default=False
    )
    parser.add_argument(
        "--disable-check-correctness",
        help="Check the correctness of the sorting algorithms",
        action="store_true",
        default=False,
    )

    args = parser.parse_args()

    main(
        input_folders=args.input,
        output=args.output,
        samples=args.samples,
        repetitions=args.repetitions,
        min_val=args.min,
        max_val=args.max,
        verbose=args.verbose,
        check_correctness=not args.disable_check_correctness,
    )
