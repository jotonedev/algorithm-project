#ifndef COUNTING_SORT_H
#define COUNTING_SORT_H

#ifdef BENCHMARK_MODE
#include "./utils.h"
#endif

int find_max(int n, int data[]);
void counting_sort(int n, int data[], int out[], int count[], int k);
long long execute(int n, int data[]);

int main(int argc, char *argv[]);

#endif