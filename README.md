# single-bench

`single-bench` is a minimalist benchmarking library for C/C++. It allows you to measure the execution time of a block of code over a specified number of iterations and outputs performance-related information.

## Description

The library provides the `BENCH` macro, which measures the execution time of a code block. At the end, it prints information about the execution time, average time per operation, and throughput.

**Example usage:**

```c
#include "singlebench.h"

int main() {
    int sum = 0;
    BENCH(1000, {
        // Code to benchmark
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
    });
    return 0;
}
