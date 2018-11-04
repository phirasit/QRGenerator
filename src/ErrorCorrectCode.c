//
// Created by phirasit on 9/17/18.
//

#include "ErrorCorrect.h"
#include "QR.h"

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

static size_t ec_codeword_per_block[][4] = {
        {0, 0, 0, 0},
        // first row
        {7, 10, 13, 17, },
        {10, 16, 22, 28, },
        {15, 26, 18, 22, },
        {20, 18, 26, 16, },
        {26, 24, 18, 22, },
        {18, 16, 24, 28, },
        {20, 18, 18, 26, },
        {24, 22, 22, 26, },
        {30, 22, 20, 24, },
        {18, 26, 24, 28, },
        {20, 30, 28, 24, },
        {24, 22, 26, 28, },
        {26, 22, 24, 22, },
        {30, 24, 20, 24, },
        {22, 24, 30, 24, },
        {24, 28, 24, 30, },
        {28, 28, 28, 28, },
        {30, 26, 28, 28, },
        {28, 26, 26, 26, },
        {28, 26, 30, 28, },
        {28, 26, 28, 30, },
        {28, 28, 30, 24, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {26, 28, 30, 30, },
        {28, 28, 28, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
        {30, 28, 30, 30, },
};

static size_t first_group[][4][2] = {
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        // first row
        {{1, 19}, {1, 16}, {1, 13}, {1, 9}, },
        {{1, 34}, {1, 28}, {1, 22}, {1, 16}, },
        {{1, 55}, {1, 44}, {2, 17}, {2, 13}, },
        {{1, 80}, {2, 32}, {2, 24}, {4, 9}, },
        {{1, 108}, {2, 43}, {2, 15}, {2, 11}, },
        {{2, 68}, {4, 27}, {4, 19}, {4, 15}, },
        {{2, 78}, {4, 31}, {2, 14}, {4, 13}, },
        {{2, 97}, {2, 38}, {4, 18}, {4, 14}, },
        {{2, 116}, {3, 36}, {4, 16}, {4, 12}, },
        {{2, 68}, {4, 43}, {6, 19}, {6, 15}, },
        {{4, 81}, {1, 50}, {4, 22}, {3, 12}, },
        {{2, 92}, {6, 36}, {4, 20}, {7, 14}, },
        {{4, 107}, {8, 37}, {8, 20}, {12, 11}, },
        {{3, 115}, {4, 40}, {11, 16}, {11, 12}, },
        {{5, 87}, {5, 41}, {5, 24}, {11, 12}, },
        {{5, 98}, {7, 45}, {15, 19}, {3, 15}, },
        {{1, 107}, {10, 46}, {1, 22}, {2, 14}, },
        {{5, 120}, {9, 43}, {17, 22}, {2, 14}, },
        {{3, 113}, {3, 44}, {17, 21}, {9, 13}, },
        {{3, 107}, {3, 41}, {15, 24}, {15, 15}, },
        {{4, 116}, {17, 42}, {17, 22}, {19, 16}, },
        {{2, 111}, {17, 46}, {7, 24}, {34, 13}, },
        {{4, 121}, {4, 47}, {11, 24}, {16, 15}, },
        {{6, 117}, {6, 45}, {11, 24}, {30, 16}, },
        {{8, 106}, {8, 47}, {7, 24}, {22, 15}, },
        {{10, 114}, {19, 46}, {28, 22}, {33, 16}, },
        {{8, 122}, {22, 45}, {8, 23}, {12, 15}, },
        {{3, 117}, {3, 45}, {4, 24}, {11, 15}, },
        {{7, 116}, {21, 45}, {1, 23}, {19, 15}, },
        {{5, 115}, {19, 47}, {15, 24}, {23, 15}, },
        {{13, 115}, {2, 46}, {42, 24}, {23, 15}, },
        {{17, 115}, {10, 46}, {10, 24}, {19, 15}, },
        {{17, 115}, {14, 46}, {29, 24}, {11, 15}, },
        {{13, 115}, {14, 46}, {44, 24}, {59, 16}, },
        {{12, 121}, {12, 47}, {39, 24}, {22, 15}, },
        {{6, 121}, {6, 47}, {46, 24}, {2, 15}, },
        {{17, 122}, {29, 46}, {49, 24}, {24, 15}, },
        {{4, 122}, {13, 46}, {48, 24}, {42, 15}, },
        {{20, 117}, {40, 47}, {43, 24}, {10, 15}, },
        {{19, 118}, {18, 47}, {34, 24}, {20, 15}, },
};

static size_t second_group[][4][2] = {
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        // first
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, },
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, },
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, },
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, },
        {{0, 0}, {0, 0}, {2, 16}, {2, 12}, },
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, },
        {{0, 0}, {0, 0}, {4, 15}, {1, 14}, },
        {{0, 0}, {2, 39}, {2, 19}, {2, 15}, },
        {{0, 0}, {2, 37}, {4, 17}, {4, 13}, },
        {{2, 69}, {1, 44}, {2, 20}, {2, 16}, },
        {{0, 0}, {4, 51}, {4, 23}, {8, 13}, },
        {{2, 93}, {2, 37}, {6, 21}, {4, 15}, },
        {{0, 0}, {1, 38}, {4, 21}, {4, 12}, },
        {{1, 116}, {5, 41}, {5, 17}, {5, 13}, },
        {{1, 88}, {5, 42}, {7, 25}, {7, 13}, },
        {{1, 99}, {3, 46}, {2, 20}, {13, 16}, },
        {{5, 108}, {1, 47}, {15, 23}, {17, 15}, },
        {{1, 121}, {4, 44}, {1, 23}, {19, 15}, },
        {{4, 114}, {11, 45}, {4, 22}, {16, 14}, },
        {{5, 108}, {13, 42}, {5, 25}, {10, 16}, },
        {{4, 117}, {0, 0}, {6, 23}, {6, 17}, },
        {{7, 112}, {0, 0}, {16, 25}, {0, 0}, },
        {{5, 122}, {14, 48}, {14, 25}, {14, 16}, },
        {{4, 118}, {14, 46}, {16, 25}, {2, 17}, },
        {{4, 107}, {13, 48}, {22, 25}, {13, 16}, },
        {{2, 115}, {4, 47}, {6, 23}, {4, 17}, },
        {{4, 123}, {3, 46}, {26, 24}, {28, 16}, },
        {{10, 118}, {23, 46}, {31, 25}, {31, 16}, },
        {{7, 117}, {7, 46}, {37, 24}, {26, 16}, },
        {{10, 116}, {10, 48}, {25, 25}, {25, 16}, },
        {{3, 116}, {29, 47}, {1, 25}, {28, 16}, },
        {{0, 0}, {23, 47}, {35, 25}, {35, 16}, },
        {{1, 116}, {21, 47}, {19, 25}, {46, 16}, },
        {{6, 116}, {23, 47}, {7, 25}, {1, 17}, },
        {{7, 122}, {26, 48}, {14, 25}, {41, 16}, },
        {{14, 122}, {34, 48}, {10, 25}, {64, 16}, },
        {{4, 123}, {14, 47}, {10, 25}, {46, 16}, },
        {{18, 123}, {32, 47}, {14, 25}, {32, 16}, },
        {{4, 118}, {7, 48}, {22, 25}, {67, 16}, },
        {{6, 119}, {31, 48}, {34, 25}, {61, 16}, },
};

static size_t remainder[] = {
        0,
        // first item
        0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
};

size_t get_remainder_bit(ERROR_CORRECT_LEVEL ec_level) {
    return remainder[ec_level];
}
size_t get_number_of_error_correct_per_block(size_t version, ERROR_CORRECT_LEVEL ec_level) {
    return ec_codeword_per_block[version][ec_level];
}

size_t get_number_of_blocks(size_t version, ERROR_CORRECT_LEVEL ec_level, size_t group) {
    switch (group) {
        case 1:
            return first_group[version][ec_level][0];
        case 2:
            return second_group[version][ec_level][0];
        default:
            return 0;
    }
}

size_t get_total_number_of_blocks(size_t version, ERROR_CORRECT_LEVEL ec_level) {
    return get_number_of_blocks(version, ec_level, 1) + get_number_of_blocks(version, ec_level, 2);
}

size_t get_number_of_codewords(size_t version, ERROR_CORRECT_LEVEL ec_level, size_t group) {
    switch (group) {
        case 1:
            return first_group[version][ec_level][1];
        case 2:
            return second_group[version][ec_level][1];
        default:
            return 0;
    }
}

size_t get_total_number_of_codewords(size_t version, ERROR_CORRECT_LEVEL ec_level) {
    size_t ans = 0, g;
    for (g = 1; g <= 2; ++g) {
        ans += get_number_of_blocks(version, ec_level, g) * get_number_of_codewords(version, ec_level, g);
    }
    return ans;
}

size_t get_number_of_groups(size_t version, ERROR_CORRECT_LEVEL ec_level) {
    return get_number_of_blocks(version, ec_level, 2) ? 2 : 1;
}

unsigned char* interleave(size_t* length, size_t num_blocks,
                          unsigned char** const block,
                          size_t const * const len,
                          unsigned char** const ec,
                          size_t const * const ec_len,
                          ERROR_CORRECT_LEVEL ec_level) {

    size_t max1, max2, i, t, j, offset;
    unsigned char* buffer;

    *length = offset = (get_remainder_bit(ec_level) + 7) >> 3;
    for (i = max1 = max2 = 0; i < num_blocks; ++i) {
        *length += len[i] + ec_len[i];
        if (len[i] > max1) {
            max1 = len[i];
        }
        if (ec_len[i] > max2) {
            max2 = ec_len[i];
        }
    }

    if ((buffer = (unsigned char*) malloc(*length * sizeof(unsigned char))) == NULL) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", *length * sizeof(unsigned char));
        exit(1);
    }

    memset(buffer + *length - offset, 0, offset);

    for (i = t = 0; i < max1; ++i) {
        for (j = 0; j < num_blocks; ++j) {
            if (len == NULL || i < len[j]) {
                buffer[t++] = block[j][i];
            }
        }
    }

    for (i = 0; i < max2; ++i) {
        for (j = 0; j < num_blocks; ++j) {
            if (len == NULL || i < ec_len[j]) {
                buffer[t++] = ec[j][i];
            }
        }
    }

    *length = ((*length - offset) << 3) + get_remainder_bit(ec_level);

    return buffer;
}

unsigned char* generate_error_correct_code(size_t version, ERROR_CORRECT_LEVEL ec_level, unsigned char* block, size_t len) {
    size_t len_ec = get_number_of_error_correct_per_block(version, ec_level);
    return divide_polynomial(len, block, len_ec+1, generate_polynomial(len_ec));
}

unsigned char* create_interleaved_block(unsigned char* bin, size_t* length, size_t const version, ERROR_CORRECT_LEVEL const ec_level) {

    size_t i, j, g;
    size_t num_group;
    size_t ec_per_block_length;

    unsigned char** block;
    unsigned char** ec_block;
    size_t* block_length;
    size_t* ec_block_length;

    ec_per_block_length = get_number_of_error_correct_per_block(version, ec_level);

    num_group = get_number_of_groups(version, ec_level);
    block = (unsigned char **) malloc(get_total_number_of_blocks(version, ec_level) * sizeof(unsigned char *));
    ec_block = (unsigned char **) malloc(get_total_number_of_blocks(version, ec_level) * sizeof(unsigned char *));
    block_length = (size_t *) malloc(get_total_number_of_blocks(version, ec_level) * sizeof(size_t));
    ec_block_length = (size_t *) malloc(get_total_number_of_blocks(version, ec_level) * sizeof(size_t));

    j = 0;
    for (g = 1; g <= num_group; ++g) {
        for (i = 0; i < get_number_of_blocks(version, ec_level, g); ++i) {
            block[j] = (j ? block[j - 1] + block_length[j - 1] : bin);
            block_length[j] = get_number_of_codewords(version, ec_level, g);
            ec_block[j] = generate_error_correct_code(version, ec_level, block[j], block_length[j]);
            ec_block_length[j] = ec_per_block_length;

            fprintf(stderr, "Block #%zu.%zu (%zu): ", i+1, j+1, block_length[j]);
            for (int x = 0; x < block_length[j]; ++x) {
                fprintf(stderr, "%d ", block[j][x]);
            }
            fprintf(stderr, "\n");
            fprintf(stderr, "Block #%zu.%zu error correct code (%zu): ", i+1, j+1, ec_block_length[j]);
            for (int x = 0; x < ec_per_block_length; ++x) {
                fprintf(stderr, "%d ", ec_block[j][x]);
            }
            fprintf(stderr, "\n");

            ++j;
        }
    }

    return interleave(length, get_total_number_of_blocks(version, ec_level), block, block_length,
                      ec_block, ec_block_length, ec_level);
}