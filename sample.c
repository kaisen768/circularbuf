#include <stdio.h>
#include "circularbuf.h"

#define DATA_UNIT_SIZE  32

int main(int argc, const char *argv[])
{
    struct circularbuf_s cir;

    circularbuf_create(&cir, 8*DATA_UNIT_SIZE);

    for (int i = 0; i < 3; i++) {
        char in[DATA_UNIT_SIZE] = {0};
        snprintf(in, DATA_UNIT_SIZE, "number : %d", i);

        cir.put(&cir, in, DATA_UNIT_SIZE);
    }

    fprintf(stderr, "Circular Buffer Total Number : %d\n\n", cir.in/DATA_UNIT_SIZE);

    for (int i = 0; i < 3; i++) {
        char out[DATA_UNIT_SIZE] = {0};
        cir.get(&cir, out, DATA_UNIT_SIZE);

        // fprintf(stderr, "Circular Buffer in: %d, out: %d, number: %d\n", cir.in, cir.out, (cir.in - cir.out)/DATA_UNIT_SIZE);
        fprintf(stderr, "--- %s\n", out);
    }

    circularbuf_destroy(&cir);

    return 0;
}
