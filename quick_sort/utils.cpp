#include <random>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "utils.h"

// Function to generate an array of random integers
int* generate_input_data(int n, int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    int* data = new int[n];
    for (int i = 0; i < n; i++) {
        data[i] = dis(gen);
    }

    return data;
}


// Function to check if the array is sorted
void check_result(int n, int data[]) {
    for (int i = 0; i < n - 1; i++) {
        if (data[i] > data[i + 1]) {
            throw std::runtime_error("Counting sort failed");
        }
    }
}


// Function to get the resolution of the clock
long long get_resolution() {
    const auto start = std::chrono::steady_clock::now();
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return elapsed.count();
}


// Function to generate sample values either linearly or exponentially
std::vector<int> generate_sample_points(int min_val, int max_val, int num_samples, bool linear) {
    std::vector<int> samples;
    samples.reserve(num_samples);
    
    if (linear) {
        // Calculate linear step size
        double step = static_cast<double>(max_val - min_val) / (num_samples - 1);
        if (step <= 0) step = 1;
        
        // Generate linearly spaced values
        for (int i = 0; i < num_samples; i++) {
            int value = min_val + static_cast<int>(i * step);
            samples.push_back(value);
        }
    } else {
        // Calculate exponential scaling factor
        double factor = std::pow(static_cast<double>(max_val) / min_val, 1.0 / (num_samples - 1));
        
        // Generate exponentially spaced values
        for (int i = 0; i < num_samples; i++) {
            int value = static_cast<int>(min_val * std::pow(factor, i));
            samples.push_back(value);
        }
    }
    
    return samples;
}

// Function to generate a timestamped filename
std::string generate_filename(const std::string& test_type, bool linear_scaling) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "quick_sort_"
       << test_type << "_"
       << (linear_scaling ? "linear" : "exponential") << "_"
       << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S")
       << ".csv";
    
    return ss.str();
}

// Function to write benchmark results to a CSV file
void write_results_to_csv(const std::string& filename, const std::vector<Run>& runs) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    // Write header
    file << "size,min_val,max_val,resolution";
    for (int r = 0; r < NUM_RUNS; r++) {
        file << ",time_" << r;
    }
    file << std::endl;
    
    // Write data rows
    for (const auto& run : runs) {
        // Find the minimum time (best performance) from all runs
        
        file << run.length << ","
             << run.min << ","
             << run.max << ","
             << run.resolution;
        
        for (int r = 0; r < NUM_RUNS; r++) {
            file << "," << run.time[r];
        }

        file << std::endl;
    }
    
    file.close();
}
