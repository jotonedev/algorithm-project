import argparse
import gc
import importlib.util
import logging
import subprocess
import sys
from dataclasses import dataclass
from math import exp, log
from pathlib import Path
from statistics import median, mean

import numpy as np
import pandas as pd
import psutil
import seaborn as sns
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
    # disable seaborn/matplotlib logs
    logging.getLogger("matplotlib").setLevel(logging.ERROR)


def generate_input_data(length: int, min_val: int, max_val: int) -> np.ndarray:
    """
    Generate a numpy array of random numbers
    
    :param length: The length of the array
    :param min_val: The minimum value
    :param max_val: The maximum value
    :return: A numpy array of random numbers
    """
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
        # make sure the path is absolute
        dir_path = dir_path.resolve()
        # check if the directory exists
        if not dir_path.exists() or not dir_path.is_dir() :
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


def benchmark_algorithm(
        algorithm: Algorithm,
        repetitions: int = 5,
        samples: int = 100,
        min_val: int = 1,
        max_val: int = 1000,
        linear: bool = False,
) -> pd.DataFrame:
    """
    Benchmark the algorithm with random input data

    :param algorithm: The Algorithm data class
    :param repetitions: The number of repetitions to run for each sample
    :param samples: The number of samples to run for each input size
    :param min_val: The minimum value of the input data
    :param max_val: The maximum value of the input data (must be greater than min_val)
    :param linear: If True, use linear scaling, otherwise use exponential scaling

    :return: A DataFrame containing the results of the benchmark
             (size, time, average resolution between repetitions)
    """
    logging.info(f"Benchmarking algorithm {algorithm.name}")

    # Create a DataFrame to store the results of execution times in nanoseconds
    results = pd.DataFrame(
        {
            "size": pd.Series(dtype=int),
            "time": pd.Series(dtype=float),
            "resolution": pd.Series(dtype=int),
        },
    )

    if linear:
        data_length = 0
        scaling_factor = int((max_val - min_val) / samples)
        if scaling_factor <= 0:
            scaling_factor = 1
    else:
        data_length = min_val
        scaling_factor = exp(log(max_val / min_val) / samples)

    # warm up the cpu clock
    for _ in range(5):
        algorithm.function(generate_input_data(100, 0, 100), 100)

    try:
        for i in tqdm.tqdm(range(samples), desc=f"Benchmarking {algorithm.name}", dynamic_ncols=True):
            if linear:
                data_length += scaling_factor
            else:
                data_length = int(min_val * (scaling_factor ** i))

            gc.collect()
            gc.disable()
            execution_times: list[int] = []
            resolutions: list[int] = []
            for _ in range(repetitions):
                data = generate_input_data(data_length, min_val, max_val)
                exec_time, res = algorithm.function(data, data_length)
                execution_times.append(exec_time)
                resolutions.append(res)
            gc.enable()

            results.loc[i] = {
                "size": data_length,
                "time": median(execution_times),
                "resolution": mean(resolutions)
            }
    except KeyboardInterrupt:
        logging.error("Benchmarking was interrupted")
        return results

    return results


def main(
        input_folders: list[Path],
        output: Path,
        repetitions: int,
        samples: int,
        min_val: int,
        max_val: int,
        verbose: bool,
        linear: bool = False,
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
            logging.info("Set process to -20 nice")
        except psutil.AccessDenied:
            logging.warning("Could not set the process to the highest priority")
            process.nice(1)
            logging.info("Set process to 1 nice")
    # pin the process to the last core
    process.cpu_affinity([psutil.cpu_count(logical=False) - 1])

    # Create an output directory if it does not exist
    output.mkdir(parents=True, exist_ok=True)

    logging.info("Starting the benchmarking tool")

    # Load the algorithms from the input directory
    algorithms: list[Algorithm] = load_algorithms(input_folders)

    # Run the benchmark for each algorithm
    for algorithm in tqdm.tqdm(algorithms, desc="Benchmarking Algorithms", dynamic_ncols=True):
        # Determine file name
        smp = str(samples).replace("000", "k")
        run_type = "linear" if linear else "log"
        os_type = "win" if sys.platform == 'win32' else "lnx"
        filename = f"{algorithm.name.replace(' ', '_')}_{smp}_{run_type}_{repetitions}rep_{os_type}.csv"

        try:
            results = benchmark_algorithm(
                algorithm=algorithm,
                repetitions=repetitions,
                samples=samples,
                min_val=min_val,
                max_val=max_val,
                linear=linear
            )
        except KeyboardInterrupt:
            logging.error("Benchmarking tool was interrupted")
            return

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
        "--min",
        help="Minimum size of the input data. Default to 1",
        type=int,
        default=1
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
        "--linear",
        help="Use linear scaling instead of exponential",
        action="store_true",
        default=False
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
        linear=args.linear,
    )
