import argparse

import pandas as pd
import seaborn as sns

from pathlib import Path


def load_data(filepath: Path) -> pd.DataFrame:
    """Load data from a CSV file into a pandas Dataframe"""
    df = pd.read_csv(
        filepath,
        dtype = {
            "size": int,
            "min_value": int,
            "max_value": int,
            "time": float,
            "resolution": float,
        }
    )
    return df

def find_filepaths(directory: Path) -> list[Path]:
    """Find all CSV files in a directory"""
    return list(directory.glob("*.csv"))

def plot_data(
        data: pd.DataFrame,
        out_file: Path,
        *,
        x_axis: str = "size",
        scale: str = "linear",
        name: str = "Placeholder",
) -> None:
    """Plot the data"""
    sns.set_theme(style="whitegrid")
    # Define the plot
    plot = sns.relplot(
        kind="line",
        x=x_axis,
        y="time",
        data=data,
        legend=False,
        label=name,
        markers=True,
    )
    # Set the legend outside the plot
    plot.ax.legend(
        loc='lower center',
        ncol=4,
        frameon=False,
        fancybox=True,
        bbox_to_anchor=(.5, 1),
        title=name,
    )
    # Change the axis scale
    plot.set(xscale=scale, yscale=scale, ylabel="Execution Time (ms)")
    # Change the axis labels
    if x_axis == "size":
        plot.set(xlabel="Length of Array")
    elif x_axis == "max_val":
        plot.set(xlabel="Maximum Value in Array")
    else:
        raise ValueError(f"Unknown x_axis: {x_axis}")

    # Save the plot
    plot.figure.savefig(out_file, format="svg", bbox_inches="tight")


def main(
        directory: Path,
        out_dir: Path,
) -> None:
    # Find all CSV files in the directory
    filepaths = find_filepaths(directory)
    # Load the data from each file
    for filepath in filepaths:
        if filepath.is_dir():
            continue

        # Extract the name and scale from the filename
        tags = filepath.stem.split("_")
        name: str = "_".join(tags[-len(tags):-5])
        scale: str = "log" if tags[-3] == "exponential" else "linear"
        x_axis: str = "max_val" if tags[-2] == "max" else "size"
        data = load_data(filepath)
        # Convert time column from nanoseconds to milliseconds
        data["time"] = data["time"] / 1_000_000
        # Plot the data
        out_file = out_dir / f"{filepath.stem}.svg"
        plot_data(
            data,
            out_file,
            x_axis=x_axis,
            scale=scale,
            name=name
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot benchmark data from CSV files")
    parser.add_argument("-i", "--input", type=Path, help="Directory containing CSV files", default=Path(".results/"))
    parser.add_argument("-o", "--output", type=Path, help="Output directory for plots", default=Path(".results/"))
    args = parser.parse_args()

    main(args.input, args.output)
