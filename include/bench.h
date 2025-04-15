// bench.h

/*
 * bench.h - High precision benchmarking library for C/C++
 * 
 * Provides macros for measuring code execution time:
 * - BENCH(): Measures time in nanoseconds using clock_gettime()
 * - BENCH_RDTSC(): Measures CPU cycles using RDTSCP instruction
 * 
 * Features:
 * - Memory barriers to prevent instruction reordering
 * - Calculates min/max/average execution times
 * - No output from measured code blocks
 * - Low overhead measurement
 * 
 * Author: Azuremuzzlekit (https://github.com/Azuremuzzlekit)
 * License: MIT
 */

#ifndef BENCH_H
#define BENCH_H

#include <time.h>
#include <stdint.h>
#include <stdio.h>

/*
* Macro for measuring execution time of a code block in nanoseconds.
* Uses CLOCK_MONOTONIC_RAW for maximum accuracy.
*
* Parameters:
* name - test name (for output)
* code - measured code block (in curly brackets)
* iterations - number of iterations for averaging
*
* Features:
* - Avoids instruction reordering via memory barrier
* - Calculates min/max/average time
* - Zero overhead outside the measured area
*/
#define BENCH(name, code, iterations) do { \
    struct timespec _bench_start, _bench_end; \
    uint64_t _bench_min = UINT64_MAX, _bench_max = 0, _bench_total = 0; \
    \
    /* Main measurement loop */ \
    for (int _bench_i = 0; _bench_i < iterations; _bench_i++) { \
        /* Memory barrier and getting time BEFORE code execution */ \
        asm volatile ("" ::: "memory"); \
        clock_gettime(CLOCK_MONOTONIC_RAW, &_bench_start); \
        \
        /* Code block to be measured */ \
        { code; } \
        \
        /* Memory barrier and getting time AFTER code execution */ \
        asm volatile ("" ::: "memory"); \
        clock_gettime(CLOCK_MONOTONIC_RAW, &_bench_end); \
        \
        /* Runtime calculations */ \
        uint64_t _bench_ns = ((_bench_end.tv_sec - _bench_start.tv_sec) * 1000000000ULL) \
                            + (_bench_end.tv_nsec - _bench_start.tv_nsec); \
        \
        /* Updating statistics */ \
        _bench_total += _bench_ns; \
        _bench_min = _bench_ns < _bench_min ? _bench_ns : _bench_min; \
        _bench_max = _bench_ns > _bench_max ? _bench_ns : _bench_max; \
    } \
    \
    /* Output results */ \
    printf("[%s]\nAvg     %7.2fns\nMin     %6luns\nMax     %6luns\nRuns     %d\n\n", \
           name, \
           (double)_bench_total / iterations, \
           _bench_min, \
           _bench_max, \
           iterations); \
} while(0)

/*
* BENCH_RDTSC - version using the CPU cycle counter (Time Stamp Counter).
* Gives the most accurate results in CPU cycles, but depends on the CPU frequency.
*
* Features:
* - Uses RDTSCP instead of RDTSC for pipeline serialization
* - Measures CPU cycles directly
* - Does not depend on the system clock
* - Requires calibration on systems with dynamic frequency
*/
#define BENCH_RDTSC(name, code, iterations) do { \
    uint64_t _bench_start, _bench_end; \
    uint64_t _bench_min = UINT64_MAX, _bench_max = 0, _bench_total = 0; \
    \
    for (int _bench_i = 0; _bench_i < iterations; _bench_i++) { \
        /* Read TSC with serialization (RDTSCP + registers) */ \
        asm volatile ( \
            "RDTSCP\n" /* Read the counter and processor ID */ \
            "shl $32, %%rdx\n" /* Shift the upper 32 bits */ \
            "or %%rdx, %%rax\n" /* Combine into 64-bit value */ \
            "mov %%rax, %0" \
            : "=m" (_bench_start) \
            : \
            : "rax", "rdx", "rcx" \
        ); \
        \
        /* Barrier for isolating the measured code */ \
        asm volatile ("" ::: "memory"); \
        { code; } \
        asm volatile ("" ::: "memory"); \
        \
        /* Re-read TSC */ \
        asm volatile ( \
            "RDTSCP\n" \
            "shl $32, %%rdx\n" \
            "or %%rdx, %%rax\n" \
            "mov %%rax, %0" \
            : "=m" (_bench_end) \
            : \
            : "rax", "rdx", "rcx" \
        ); \
        \
        /* Calculate cycles */ \
        uint64_t _bench_cycles = _bench_end - _bench_start; \
        _bench_total += _bench_cycles; \
        _bench_min = _bench_cycles < _bench_min ? _bench_cycles : _bench_min; \
        _bench_max = _bench_cycles > _bench_max ? _bench_cycles : _bench_max; \
    } \
    \
    printf("[%s]\nAvg     %7.2f cycles\nMin     %6lu\nMax     %6lu\nRuns     %d\n\n", \
           name, \
           (double)_bench_total / iterations, \
           _bench_min, \
           _bench_max, \
           iterations); \
} while(0)

#endif // BENCH_H
