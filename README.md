# Precision Benchmark Library

Ultra-low overhead benchmarking tools for C/C++ developers.

## Features

- Two measurement modes:
  - `BENCH()`: Nanosecond precision (using `clock_gettime()`)
  - `BENCH_RDTSC()`: CPU cycle counts (using `RDTSCP`)
- Calculates statistics: min/max/average times
- Memory barriers prevent instruction reordering
- No output pollution from measured code blocks

## Usage

```c
#include "bench.h"

int main() {
    // Measure nanoseconds
    BENCH("Memory write", {
        volatile int x = 0;
        for(int i=0; i<1000; i++) x = i;
    }, 1000);
    
    // Measure CPU cycles
    BENCH_RDTSC("NOP instruction", {
        asm("nop");
    }, 100000);
    
    return 0;
}
