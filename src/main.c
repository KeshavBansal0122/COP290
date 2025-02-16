#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "backend.h"
#include "frontend.h"

int main(int argc, char **argv) {
    int rows = 100;
    int cols = 100;
    errno = 0;
    if(argc == 3) {
        rows = strtol(argv[1], NULL, 10);
        if (errno != 0) {
            printf("Invalid argument for rows");
            exit(-1);
        }
        cols = strtol(argv[2], NULL, 10);
        if (errno != 0) {
            printf("Invalid argument for columns");
            exit(-1);
        }
    }
    initFrontend(rows, cols);
    return 0;
}
