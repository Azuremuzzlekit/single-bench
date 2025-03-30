/**
 * Minimalistic benchmarking library for C/C++
 * Creator: Azuremuzzlekit
 * License: MIT
 */

#pragma once

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct timespec __bench_start;

static inline void bench_start() {
    clock_gettime(CLOCK_MONOTONIC, &__bench_start);
}

static inline uint64_t bench_end() {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - __bench_start.tv_sec) * 1000000000ULL
           + (end.tv_nsec - __bench_start.tv_nsec);
}

static inline void bench_print(const char* name, uint64_t total_ns, int iterations) {
    printf("Benchmark '%s':\n", name);
    printf("  Iterations:\t%d\n", iterations);
    printf("  Total time:\t%.3f ms\n", total_ns / 1000000.0);
    printf("  Avg time:\t%.3f ns\n", (double)total_ns / iterations);
    printf("  Throughput:\t%.2f ops/ms\n\n", iterations * 1000.0 / (total_ns / 1000000.0));
}

// Макрос для удобного использования
#define BENCH(iterations, code_block) \
do { \
    bench_start(); \
    for (int _iter = 0; _iter < iterations; _iter++) { \
        code_block \
    } \
    uint64_t _time = bench_end(); \
    bench_print(#code_block, _time, iterations); \
} while(0)

#ifdef __cplusplus
}
#endif
