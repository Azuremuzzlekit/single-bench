// example of using the bench.h library

#include "bench.h"

void example() {
    volatile int x = 0; // volatile prevents optimization
    
    BENCH("Empty loop", {
        for(int i = 0; i < 1000; i++) {
            x = i; // pseudo-load
        }
    }, 1000);

    BENCH_RDTSC("CPU Cycles test", {
        asm("nop"); // precise measurement of one instruction
    }, 100000);
}

int main() {
    example();
    return 0;
}
