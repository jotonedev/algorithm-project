import argparse
import gc
import importlib.util
import logging
import subprocess
import sys
import timeit
from statistics import median

import psutil
import tqdm

from dataclasses import dataclass
from math import exp, log
from pathlib import Path

import numpy as np
import pandas as pd
import seaborn as sns
from numpy.ma.extras import average


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
    # disable seaborn logs
    logging.getLogger("matplotlib").setLevel(logging.ERROR)


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


def is_sorted(data: np.ndarray) -> bool:
    """
    Check the correctness of the sorting algorithm

    :param data: The original input data as NumPy array
    :return: True if the data is sorted, False otherwise
    """
    for elem in range(1, len(data)):
        if data[elem] < data[elem - 1]:
            return False


def benchmark_algorithm(
        algorithm: Algorithm, 
        repetitions: int = 5,
        samples: int = 100,
        min_val: int = 100,
        max_val: int | None = None,
        linear: bool = False,
) -> pd.DataFrame:
    """
    Benchmark the algorithm with random input data

    :param algorithm: The Algorithm data class
    :param repetitions: The number of repetitions to run
    :param samples: The number of samples to run
    :param min_val: The minimum value of the input data
    :param max_val: The maximum value of the input data.
    :param linear: If True, use linear scaling, otherwise use exponential scaling
    :return: A list of execution times
    """
    logging.info(f"Benchmarking algorithm {algorithm.name}")

    # set lower bound for max_val
    if max_val is None or max_val < samples:
        max_val = samples+1

    # Create a DataFrame to store the results of execution times in nanoseconds
    results = pd.DataFrame(
        {
            "size": pd.Series(dtype="int"),
            "time": pd.Series(dtype="float"),
        },
        index=range(samples),
    )

    if linear:
        data_length = 0
        scaling_factor = int((max_val - min_val) / samples)
    else:
        data_length = min_val
        scaling_factor = exp(log(max_val / min_val) / samples)

    for i in tqdm.tqdm(range(samples), desc=f"Benchmarking {algorithm.name}", dynamic_ncols=True):
        if linear:
            data_length += scaling_factor
        else:
            data_length = int(min_val * (scaling_factor ** i))

        gc.collect()
        gc.disable()
        execution_time = timeit.Timer(
            stmt="algorithm.function(data, data_length)",
            setup="data = generate_input_data(data_length, min_val, max_val)",
            globals=globals() | locals()
        ).repeat(repeat=repetitions, number=1)
        gc.enable()

        results.loc[i] = {
            "size": data_length,
            "time": median(execution_time)
        }

    return results


def plot_results(data: pd.DataFrame, output: Path, linear: bool = False) -> None:
    """
    Plot the benchmark results

    :param data: The benchmark results as a DataFrame
    :param output: The output file path
    :param linear: If True, use linear scaling, otherwise use exponential scaling for both axes
    :param name: The name of the output plot file
    """
    # cleanup the data
    data["time"] = data["time"] * 1e3

    # prepare the plot
    sns.set_theme(style="whitegrid")
    plot = sns.relplot(
        data=data,
        x="size",
        y="time",
        kind="line",
        size=5,
        aspect=1,
        legend=False
    )
    plot.set_xlabels("Input Size")
    plot.set_ylabels("Execution Time (ms)")

    # add red line for quadratic function
    plot.ax.plot(
        data["size"],
        (data["size"] ** 2) * 9e-7,
        color="red",
        linestyle="--",
        label="O(n^2)",
        alpha=0.8,
        linewidth=1.5
    )

    # add orange line for n*log(n) function
    y = np.multiply(data["size"], np.log10(data["size"])) * 25e-5
    plot.ax.plot(
        data["size"],
        y,
        color="orange",
        linestyle="--",
        label="O(n*log(n))",
        alpha=0.8,
        linewidth=1.5
    )

    # add green line for linear function
    plot.ax.plot(
        data["size"],
        data["size"] * 5e-4,
        color="green",
        linestyle="--",
        label="O(n)",
        alpha=0.8,
        linewidth=1.5
    )

    # set scale
    if linear:
        plot.set(xscale="linear", yscale="linear")
    else:
        plot.set(xscale="log", yscale="log")

    # add legend on the bottom left outside the plot
    plot.ax.legend(loc='lower left', ncol=4, frameon=False, bbox_to_anchor=(0, -0.25), fancybox=True)

    plot.savefig(output, format="svg", transparent=False)


def main(
    input_folders: list[Path],
    output: Path,
    repetitions: int,
    samples: int,
    min_val: int,
    max_val: int | None,
    verbose: bool,
    linear: bool = False,
):
    """Run the benchmarking tool"""
    setup_logger(verbose)

    # Increase process priority to avoid interruptions
    # check if is windows
    process = psutil.Process()
    if sys.platform == 'win32':
        process.nice(psutil.HIGH_PRIORITY_CLASS)
    elif sys.platform == 'linux':
        # set the process to the highest priority if permission is granted
        try:
            process.nice(-20)
        except psutil.AccessDenied:
            logging.warning("Could not set the process to the highest priority")
        finally:
            # set the process to the highest possible priority if permission is not granted
            process.nice(1)
    # pin process to the last core
    process.cpu_affinity([psutil.cpu_count(logical=False) - 1])

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
                linear=linear
            )
        except KeyboardInterrupt:
            logging.error("Benchmarking tool was interrupted")
            return

        # Save the results to a CSV file
        results.to_csv(output / f"{algorithm.name}.csv", index=False)
        # Plot the results
        plot_results(results, output / f"{algorithm.name}.svg", linear)

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
        default=5
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
        linear=args.linear
    )
