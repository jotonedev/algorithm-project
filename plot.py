import argparse

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

from pathlib import Path


def plot_data(
        data: pd.DataFrame,
        out_file: Path,
        *,
        x_axis: str = "length",
        scale: str = "linear",
        name: str = "Placeholder",
) -> None:
    """Plot the data"""
    # Set the style
    sns.set_theme(style="whitegrid", font_scale=1.9)
    fig, ax1 = plt.subplots(1, 1, figsize=(12, 10), gridspec_kw={'height_ratios': [1]})

    # Create the main plot
    ax1.plot(
        data[x_axis],
        data["time"],
        marker="o",
        linestyle="-",
        color="black",
        markersize=3,
    )

    # Set the scale
    if scale == "exp":
        ax1.set_xscale("log")
        ax1.set_yscale("log")

    # Set the labels
    ax1.set_title(name)
    if x_axis == "length":
        ax1.set_xlabel("Array Length")
    else:
        ax1.set_xlabel("Maximum Value")

    ax1.set_ylabel("Time (µs)")
    ax1.grid(True)

    # Increase the number of ticks
    if scale == "exp":
        ax1.get_xaxis().set_major_locator(plt.LogLocator(base=10, numticks=5))
        ax1.get_xaxis().set_minor_locator(plt.LogLocator(base=10, subs="auto", numticks=5))
        ax1.get_yaxis().set_major_locator(plt.LogLocator(base=10, numticks=5))
        ax1.get_yaxis().set_minor_locator(plt.NullLocator())
    else:
        ax1.get_xaxis().set_major_locator(plt.MaxNLocator(10))
        ax1.get_yaxis().set_major_locator(plt.MaxNLocator(10))
        # Remove the scientific notation on the x-axis
        ax1.ticklabel_format(axis="x", style="plain")

    # Save the plot
    plt.tight_layout()
    plt.savefig(out_file)
    plt.close()



def load_data(filepath: Path) -> pd.DataFrame:
    """Load data from a CSV file into a pandas Dataframe"""
    df = pd.read_csv(
        filepath,
        dtype={
            "length": int,
            "min_val": int,
            "max_val": int,
            "resolution": int,
        }
    )

    return df


def transform_data(df: pd.DataFrame) -> pd.DataFrame:
    """Clean and transform the data"""
    # Get the name of the columns that start with "time"
    time_columns = [col for col in df.columns if col.startswith("time_")]

    # Convert the time columns from nanoseconds to microseconds
    df[time_columns] = df[time_columns] / 1_000

    # Create a new column with the median time
    df["time"] = df[time_columns].median(axis=1)
    # Create a new column with the standard deviation of the time
    df["time_std"] = df[time_columns].std(axis=1)
    # Drop the original time columns
    df.drop(columns=time_columns, inplace=True)

    # Drop the resolution column
    df.drop(columns=["resolution"], inplace=True)

    return df


def list_csv_files(directory: Path) -> list[Path]:
    """List all CSV files in a directory"""
    return list(directory.glob("*.csv"))


def main(
        in_dir: Path,
        out_dir: Path,
) -> None:
    if not in_dir.exists():
        raise FileNotFoundError(f"Input directory not found: {in_dir}")
    if not out_dir.exists():
        out_dir.mkdir(parents=True, exist_ok=True)

    for file in list_csv_files(in_dir):
        data = load_data(file)
        data = transform_data(data)

        # Get the name of the file
        name = file.stem
        # Extract if the x-axis is the length or the maximum value
        x_axis = "length" if "length" in name else "max_val"
        # Extract the scale
        scale = "exp" if "exponential" in name else "linear"
        # Extract the name of the plot
        name = name.split("_sort_")[0].replace("_", " ") + " Sort"
        name = name.title()

        # Plot the data
        out_file = out_dir / f"{file.stem}.svg"
        plot_data(data, out_file, x_axis=x_axis, scale=scale, name=name)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot benchmark data from CSV files")
    parser.add_argument("-i", "--input", type=Path, help="Directory containing CSV files", default=Path("bin/"))
    parser.add_argument("-o", "--output", type=Path, help="Output directory for plots", default=Path(".results/"))
    args = parser.parse_args()

    main(args.input, args.output)
