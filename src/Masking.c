//
// Created by phirasit on 9/20/18.
//

#include "Bitset.h"
#include "ErrorCorrect.h"
#include "QR.h"

#include <stdio.h>
#include <stdlib.h>

#define abs(x) ((x) > 0 ? (x) : -(x))

size_t get_int_from_error_correct_level(ERROR_CORRECT_LEVEL ec_level) {
    return ec_level ^ 1;
}

int is_mask_cell(size_t mask, size_t i, size_t j);
size_t evaluate_condition1(unsigned char** buffer, size_t size);
size_t evaluate_condition2(unsigned char** buffer, size_t size);
size_t evaluate_condition3(unsigned char** buffer, size_t size);
size_t evaluate_condition4(unsigned char** buffer, size_t size);

void apply_mask_info(unsigned char** buffer, size_t size, ERROR_CORRECT_LEVEL ec_level, size_t mask) {

    static const int generator = 0x537;
    static const int mask_string = 0x5412;

    unsigned char *poly, ch;
    size_t i, j;

    if ((mask & 7) != mask) {
        fprintf(stderr, "Invalid Mask(%zu)\n", mask);
        exit(1);
    }

    // create a hash polynomial
    poly = bitset_divide_polynomial(5, (get_int_from_error_correct_level(ec_level) << 3) | mask, 11, generator);
    bitset_set_range(poly, 0, 15, bitset_get_range(poly, 0, 15) ^ mask_string);

    // write info into the buffer
    for (i = 0; i < 15; ++i) {
        ch = get_color_from_bit(bitset_get(poly, i));
        if (i <= 6) {
            buffer[8][i + (i == 6)] = ch;
            buffer[size-1-i][8] = ch;
        } else {
            buffer[15-i - (i > 8)][8] = ch;
            buffer[8][size-15+i] = ch;
        }
    }
}

void apply_mask(unsigned char** buffer, size_t size, size_t version, size_t mask) {
    size_t i, j;
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            if (is_mask_cell(mask, i, j) && is_data_cell(version, size, i, j)) {
                buffer[i][j] = invert_color(buffer[i][j]);
            }
        }
    }
}

size_t evaluate_mask(unsigned char** buffer, size_t size, size_t version, ERROR_CORRECT_LEVEL ec_level, size_t mask) {
    size_t ans;

    // mask the data
    apply_mask_info(buffer, size, ec_level, mask);
    apply_mask(buffer, size, version, mask);

    // calculate answer
    ans = evaluate_condition1(buffer, size) +
          evaluate_condition2(buffer, size) +
          evaluate_condition3(buffer, size) +
          evaluate_condition4(buffer, size);

    apply_mask(buffer, size, version, mask);

    return ans;
}

void add_mask_info(unsigned char** buffer, size_t size, size_t version, ERROR_CORRECT_LEVEL ec_level) {

    size_t best_mask, best_value, value;
    size_t m;

    best_mask = best_value = 0;
    for (m = 0; m < 8; ++m) {
        value = evaluate_mask(buffer, size, version, ec_level, m);
        if (m == 0 || value < best_value) {
            best_mask = m;
            best_value = value;
        }

    }

    apply_mask_info(buffer, size, ec_level, best_mask);
    apply_mask(buffer, size, version, best_mask);

    fprintf(stderr, "The best mask is (%zu) with score = %zu\n", best_mask, best_value);
}

int is_mask_cell(size_t mask, size_t i, size_t j) {
    switch (mask) {
        case 0: return (i & 1) == (j & 1);
        case 1: return (i & 1) == 0;
        case 2: return j % 3 == 0;
        case 3: return (i + j) % 3 == 0;
        case 4: return (((i >> 1) + j / 3) & 1) == 0;
        case 5: return (!(i & 1) || !(j & 1)) && (i * j % 3 == 0);
        case 6: return (!(i & 1) || !(j & 1)) == (i * j % 3 != 1);
        case 7: return ((i ^ j) & 1) == ((i * j % 3) & 1);
        default: return 0;
    }
}

size_t evaluate_condition1(unsigned char** buffer, size_t size) {
    size_t count, consecutive, i, j;

    for (count = i = 0; i < size; ++i) {
        for (consecutive = j = 1; j < size; ++j) {
            if (buffer[i][j] != buffer[i][j-1]) {
                consecutive = 1;
            } else {
                ++consecutive;
            }
            if (consecutive == 5) {
                count += 3;
            } else if (consecutive > 5) {
                ++count;
            }
        }
    }

    for (j = 0; j < size; ++j) {
        for (consecutive = i = 1; i < size; ++i) {
            if (buffer[i][j] != buffer[i-1][j]) {
                consecutive = 1;
            } else {
                ++consecutive;
            }
            if (consecutive == 5) {
                count += 3;
            } else if (consecutive > 5) {
                ++count;
            }
        }
    }

    return count;
}

size_t evaluate_condition2(unsigned char** buffer, size_t size) {
    size_t count, i, j;

    for (count = 0, i = 1; i < size; ++i) {
        for (j = 1; j < size; ++j) {
            if (buffer[i][j] == buffer[i-1][j]
                && buffer[i][j] == buffer[i][j-1]
                && buffer[i][j] == buffer[i-1][j-1]) {

                ++count;
            }
        }
    }

    return count + (count << 1);
}

size_t evaluate_condition3(unsigned char** buffer, size_t size) {
    size_t count, i, j, k, val;

    for (count = i = 0; i < size; ++i) {
        for (j = 0; j < size - 10; ++j) {
            val = 0;
            for (k = 0; k < 11; ++k) {
                val = 2 * val + (buffer[i][j+k] == get_black());
            }
            if (val == 93 || val == 1488) {
                ++count;
            }
        }
    }

    for (j = 0; j < size; ++j) {
        for (i = 0; i < size-10; ++i) {
            val = 0;
            for (k = 0; k < 11; ++k) {
                val = 2 * val + (buffer[i+k][j] == get_black());
            }
            if (val == 93 || val == 1488) {
                ++count;
            }
        }
    }

    return 40 * count;
}

size_t evaluate_condition4(unsigned char** buffer, size_t size) {

    size_t count, i, j;

    for (count = i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            if (buffer[i][j] == get_black()) {
                ++count;
            }
        }
    }

    return 10 * abs(5 - 10 * count / (size * size));

}
