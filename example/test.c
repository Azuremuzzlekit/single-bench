#include "bench.h"

void example() {
    volatile int x = 0;
    
    BENCH("Empty loop", {
        for(int i = 0; i < 1000; i++) {
            x = i;
        }
    }, 1000);

    BENCH_RDTSC("CPU Cycles test", {
        asm("nop");
    }, 100000);
}

int main() {
    example();
    return 0;
}
