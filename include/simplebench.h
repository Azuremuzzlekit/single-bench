/**
 * @file single_bench.h
 * @brief Zero-overhead microbenchmarking library (C11/C17 compatible)
 * @author Azuremuzzlekit
 * @license MIT
 * @version 1.1
 *
 * @usage:
 * #include "single_bench.h"
 * BENCH(1000000, {
 *     // Code to measure
 *     volatile double x = 3.1415926;
 *     x = x * x;
 * });
 */

#pragma once

#include <stdio.h>    // For printf
#include <stdint.h>   // For uint64_t
#include <time.h>     // For clock_gettime

// =================================================================
// Platform-specific configuration
// =================================================================

#ifndef CLOCK_MONOTONIC
#error "CLOCK_MONOTONIC not supported on this platform"
#endif

// =================================================================
// Internal implementation details
// =================================================================

/**
 * @internal
 * @brief Get current timestamp in nanoseconds
 * @return Monotonic clock value in nanoseconds
 *
 * @note Uses CLOCK_MONOTONIC for measurement stability
 * @warning Not thread-safe (per-thread measurement recommended)
 */
static inline uint64_t __bench_get_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * UINT64_C(1000000000) + (uint64_t)ts.tv_nsec;
}

// =================================================================
// Public benchmarking API
// =================================================================

/**
 * @def BENCH(iterations, code_block)
 * @brief Benchmark execution time of a code block
 * @param iterations Number of times to execute the block (must be >0)
 * @param code_block Code to measure (in curly braces)
 *
 * @features:
 * - Zero function call overhead
 * - Compiler barrier for accurate measurement
 * - Automatic result formatting
 * - Type-safe calculations
 *
 * @example:
 * BENCH(1000000, {
 *     volatile int x = 42;
 *     x = (x << 3) | (x >> 5);
 * });
 */
#define BENCH(iterations, code_block) do { \
    /* Prevent compiler optimizations across measurements */ \
    volatile uint64_t __bench_start = __bench_get_ns(); \
    \
    /* Execute benchmark loop */ \
    for (uint32_t __bench_i = 0; __bench_i < (uint32_t)(iterations); ++__bench_i) { \
        code_block \
    } \
    \
    /* Calculate precise duration */ \
    const uint64_t __bench_end = __bench_get_ns(); \
    const uint64_t __bench_delta = __bench_end - __bench_start; \
    \
    /* Format and print results */ \
    printf("\n[BENCH] %s\n", #code_block); \
    printf("  Iterations:   %'u\n", (uint32_t)(iterations)); \
    printf("  Total time:   %.3f ms\n", __bench_delta / 1e6); \
    printf("  Average time: %.3f ns/op\n", (double)__bench_delta / (iterations)); \
    printf("  Throughput:   %.2f MOPS\n\n", (iterations) / (__bench_delta / 1e3)); \
} while(0)
