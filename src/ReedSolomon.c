//
// Created by phirasit on 9/18/18.
//

#include "Bitset.h"
#include "ErrorCorrect.h"

#include <stdlib.h>
#include <libnet.h>

#define N 256
#define GENERATOR 285

static unsigned char log[N];
static unsigned char anti_log[N];

static size_t init = 0;

void generate_log() {
    size_t i, val;

    log[0] = 1;
    anti_log[1] = 0;
    for (i = val = 1; i < N; ++i) {
        val = val << 1;
        if (val >> 8) val ^= GENERATOR;
        log[i] = (unsigned char) val;
        anti_log[log[i]] = (unsigned char) i;
    }

    init = 1;
}

unsigned char fix(size_t x) {
    return (unsigned char) ((x & 255) + (x >> 8));
}

unsigned char* generate_polynomial(size_t n) {

    if (!init) {
        generate_log();
    }

    unsigned char* buffer;
    size_t i, j;

    if ((buffer = (unsigned char*) malloc((n+1))) == NULL) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", (n+1));
        exit(1);
    }

    buffer[0] = 0;
    for (i = 1; i <= n; ++i) {
        buffer[i] = buffer[i-1];
        for (j = i-1; j; --j) {
            buffer[j] = anti_log[log[fix(buffer[j] + i-1)] ^ log[buffer[j-1]]];
        }
        buffer[0] = fix(buffer[0] + i-1);
    }

    return buffer;
}

unsigned char* divide_polynomial(size_t len_inp, unsigned char* inp, size_t len_divisor, unsigned char* divisor) {
    size_t i, j;
    unsigned char head;
    unsigned char* buffer;

    if (!init) {
        generate_log();
    }

    if ((buffer = (unsigned char*) malloc((len_divisor-1))) == NULL) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", (len_divisor-1));
        exit(1);
    }

    for (i = 1; i <= len_inp; ++i) {

        head = (i > 1 ? buffer[0] : inp[0]);
        memcpy(buffer, (i > 1 ? buffer : inp) + 1, len_divisor - 1);

        buffer[len_divisor-2] = (i + len_divisor-2 < len_inp ? inp[i+len_divisor-2] : (unsigned char) 0);

        for (j = 0; j < len_divisor-1; ++j) {
            buffer[j] ^= log[fix(divisor[len_divisor - j - 2] + anti_log[head])];
        }
    }

    return buffer;
}

unsigned char* bitset_divide_polynomial(size_t len_inp, size_t inp, size_t len_div, size_t div) {
    unsigned char *buffer;
    size_t i;

    if ((buffer = (unsigned char*) malloc(i = (len_inp + len_div - 2 + 7) >> 3)) == 0) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", i);
        exit(1);
    }

    bitset_set_range(buffer, 0, len_inp, inp);

    for (i = 0; i < len_inp; ++i) {
        if (bitset_get(buffer, i)) {
            bitset_set_range(buffer, i, len_div, bitset_get_range(buffer, i, len_div) ^ div);
        }
    }

    bitset_set_range(buffer, 0, len_inp, inp);

    return buffer;
}