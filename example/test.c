// a bit of js style :)

#include "../include/singlebench.h"
#include <unistd.h>

int main() {
    // test of empty cycle
    BENCH(1000, {
        // empty block
    });

    BENCH(10000000, {
        write(STDOUT_FILENO, NULL, 0);
    });

    return 0;
}
