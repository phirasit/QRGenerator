//
// Created by phirasit on 9/19/18.
//

#include "Bitset.h"
#include "ErrorCorrect.h"
#include "QR.h"

#include <stdio.h>
#include <libnet.h>
#include <stdlib.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define BLACK '1'
#define WHITE '0'
#define EMPTY '-'
#define RESERVE '#'

#define MAX_ALIGNMENT 6
#define TIMING_PATTERN 6
#define ALIGNMENT_SIZE 5

static size_t alignment[][MAX_ALIGNMENT] = {
        {0, 0, 0, 0, 0, 0},
        // no alignment for first version
        {0, 0, 0, 0, 0, 0},
        // version 2 starts here
        {6, 18, 0, 0, 0, 0},
        {6, 22, 0, 0, 0, 0},
        {6, 26, 0, 0, 0, 0},
        {6, 30, 0, 0, 0, 0},
        {6, 34, 0, 0, 0, 0},
        {6, 22, 38, 0, 0, 0},
        {6, 24, 42, 0, 0, 0},
        {6, 26, 46, 0, 0, 0},
        {6, 28, 50, 0, 0, 0},
        {6, 30, 54, 0, 0, 0},
        {6, 32, 58, 0, 0, 0},
        {6, 34, 62, 0, 0, 0},
        {6, 26, 46, 66, 0, 0},
        {6, 26, 48, 70, 0, 0},
        {6, 26, 50, 74, 0, 0},
        {6, 30, 54, 78, 0, 0},
        {6, 30, 56, 82, 0, 0},
        {6, 30, 58, 86, 0, 0},
        {6, 34, 62, 90, 0, 0},
        {6, 28, 50, 72, 94, 0},
        {6, 26, 50, 74, 98, 0},
        {6, 30, 54, 78, 102, 0},
        {6, 28, 54, 80, 106, 0},
        {6, 32, 58, 84, 110, 0},
        {6, 30, 58, 86, 114, 0},
        {6, 34, 62, 90, 118, 0},
        {6, 26, 50, 74, 98, 122},
        {6, 30, 54, 78, 102, 126},
        {6, 26, 52, 78, 104, 130},
        {6, 30, 56, 82, 108, 134},
        {6, 34, 60, 86, 112, 138},
        {6, 30, 58, 86, 114, 142},
        {6, 34, 62, 90, 118, 146},
        {6, 30, 54, 78, 102, 126},
        {6, 24, 50, 76, 102, 128},
        {6, 28, 54, 80, 106, 132},
        {6, 32, 58, 84, 110, 136},
        {6, 26, 54, 82, 110, 138},
        {6, 30, 58, 86, 114, 142},
};

unsigned char get_black(void) { return BLACK; }
unsigned char get_white(void) { return WHITE; }

unsigned char get_color_from_bit(size_t b) {
    return (unsigned char) (b ? BLACK : WHITE);
}

unsigned char invert_color(unsigned char color) {
    switch (color) {
        case BLACK: return WHITE;
        case WHITE: return BLACK;
        default: return color;
    }
}

void add_qr_finder(unsigned char** buffer, size_t size);
void add_qr_alignments(unsigned char** buffer, size_t version);
void add_timing_pattern(unsigned char** buffer, size_t size);
void reserve_info_area(unsigned char** buffer, size_t size);
void add_version_info(unsigned char** buffer, size_t version, size_t size);
void add_data(unsigned char** buffer, size_t size, size_t length, unsigned char* code);

unsigned char** generate_qr_code_from_block(size_t* size, size_t version, size_t length, unsigned char* const code) {
    size_t i, j;
    unsigned char** buffer;

    // assign size
    *size = ((version-1) << 2) + 21;

    if ((buffer = (unsigned char**) malloc(*size * sizeof(unsigned char*))) == NULL) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", *size * sizeof(unsigned char*));
        exit(1);
    }

    for (i = 0; i < *size; ++i) {
        if ((buffer[i] = (unsigned char*) malloc((*size+1) * sizeof(unsigned char*))) == NULL) {
            fprintf(stderr, "Cannot allocate memory size = %zu\n", (*size+1) * sizeof(unsigned char*));
            exit(1);
        }
        buffer[i][*size+1] = 0;
        memset(buffer[i], EMPTY, *size);
    }

    add_qr_finder(buffer, *size);
    add_qr_alignments(buffer, version);
    add_timing_pattern(buffer, *size);
    add_version_info(buffer, version, *size);
    reserve_info_area(buffer, *size);

    add_data(buffer, *size, length, code);

    return buffer;
}

/*
 * Add qr finder at the corner of the matrix
 */
void add_qr_finder(unsigned char** buffer, size_t size) {
    size_t i, j;
    for (i = 0; i < 7; ++i) {
        for (j = 0; j < 7; ++j) {
            if (2 <= min(i, j) && max(i, j) <= 4) {
                // BLACK
                buffer[size-7+i][j] = buffer[i][size-7+j] = buffer[i][j] = BLACK;
            } else if (1 <= min(i, j) && max(i, j) <= 5) {
                // WHITE
                buffer[size-7+i][j] = buffer[i][size-7+j] = buffer[i][j] = WHITE;
            } else {
                // BLACK
                buffer[size-7+i][j] = buffer[i][size-7+j] = buffer[i][j] = BLACK;
            }
        }
    }

    // add separators
    for (i = 0; i < 8; ++i) {
        buffer[i][7] = buffer[7][i] = WHITE;
        buffer[i][size-1 - 7] = buffer[7][size-1 - i] = WHITE;
        buffer[size-1 - 7][i] = buffer[size-1 - i][7] = WHITE;
    }
}

void add_qr_alignments(unsigned char** buffer, size_t version) {
    size_t i, j, r, c, ii, jj;
    unsigned char *ch;

    for (i = 0; i < MAX_ALIGNMENT; ++i) {
        if (!alignment[version][i]) break;
        r = alignment[version][i] - 2;
        for (j = 0; j < MAX_ALIGNMENT; ++j) {
            if (!alignment[version][j]) break;
            c = alignment[version][j] - 2;

            for (ii = 0; ii < ALIGNMENT_SIZE; ++ii) {
                for (jj = 0; jj < ALIGNMENT_SIZE; ++jj) {
                    if (buffer[r+ii][c+jj] != EMPTY) {
                        goto skip;
                    }
                }
            }

            // apply alignment
            for (ii = 0; ii < ALIGNMENT_SIZE; ++ii) {
                for (jj = 0; jj < ALIGNMENT_SIZE; ++jj) {
                    ch = &buffer[r+ii][c+jj];
                    if (ii == 2 && jj == 2) {
                        *ch = BLACK;
                    } else if (1 <= min(ii, jj) && max(ii, jj) <= 3) {
                        *ch = WHITE;
                    } else {
                        *ch = BLACK;
                    }
                }
            }

            skip:;
        }
    }
}

void add_timing_pattern(unsigned char** buffer, size_t size) {
    size_t i;
    for (i = 1; i < size; ++i) {
        if (buffer[TIMING_PATTERN][i] == EMPTY) {
            buffer[TIMING_PATTERN][i] = (unsigned char) (buffer[TIMING_PATTERN][i - 1] == BLACK ? WHITE : BLACK);
        }
        if (buffer[i][TIMING_PATTERN] == EMPTY) {
            buffer[i][TIMING_PATTERN] = (unsigned char) (buffer[i-1][TIMING_PATTERN] == BLACK ? WHITE : BLACK);
        }
    }
}

void add_version_info(unsigned char** buffer, size_t version, size_t size) {

    static const int generator = 0x1F25;

    size_t i, j;
    unsigned char* poly;

    if (version < 7) return;

    poly = bitset_divide_polynomial(6, version, 13, generator);
    bitset_reverse(poly, 0, 18);

    for (i = 0; i < 6; ++i) {
        for (j = 0; j < 3; ++j) {
            buffer[size-11+j][i] = buffer[i][size-11+j] = get_color_from_bit(bitset_get(poly, 3*i + j));
        }
    }
}

void reserve_info_area(unsigned char** buffer, size_t size) {
    size_t i;
    for (i = 0; i < 9; ++i) {
        if (i < 7) {
            buffer[size-i-1][8] = RESERVE;
        }
        if (i < 8) {
           buffer[8][size-i-1] = RESERVE;
        }
        if (buffer[8][i] == EMPTY) buffer[8][i] = RESERVE;
        if (buffer[i][8] == EMPTY) buffer[i][8] = RESERVE;
    }
    buffer[size-8][8] = BLACK;
}

int is_timing_cell(size_t r, size_t c) {
    return r == TIMING_PATTERN || c == TIMING_PATTERN;
}

int is_finder_cell(size_t size, size_t r, size_t c) {
    return (r <= 8 && c <= 8)
           || (r <= 8 && c >= size-8)
           || (r >= size-8 && c <= 8);
}

int is_version_cell(size_t version, size_t size, size_t r, size_t c) {
    if (version < 7) return 0;
    return (size-11 <= r && r < size-8 && 0 <= c && c < 6)
           || (0 <= r && r < 6 && size-11 <= c && c < size-8);
}

int is_data_cell(size_t version, size_t size, size_t r, size_t c) {
    size_t i, j, ii, jj, cr, cc;

    if (is_finder_cell(size, r, c)) return 0;
    if (is_timing_cell(r, c)) return 0;
    if (is_version_cell(version, size, r, c)) return 0;

    for (i = 0; i < MAX_ALIGNMENT; ++i) {
        if (!alignment[version][i]) break;
        cr = alignment[version][i] - 2;
        if (!(cr <= r && r < cr + ALIGNMENT_SIZE)) continue;

        for (j = 0; j < MAX_ALIGNMENT; ++j) {
            if (!alignment[version][j]) break;
            cc = alignment[version][j] - 2;
            if (!(cc <= c && c < cc + ALIGNMENT_SIZE)) continue;

            for (ii = 0; ii < ALIGNMENT_SIZE; ++ii) {
                for (jj = 0; jj < ALIGNMENT_SIZE; ++jj) {
                    if (is_finder_cell(size, cr + ii, cc + jj)) {
                        goto skip;
                    }
                }
            }

            return 0;

            skip:;
        }
    }

    return 1;
}

int is_empty(unsigned char ch) {
    return ch == EMPTY;
}

void add_data(unsigned char** const buffer, size_t size, size_t length, unsigned char* const code) {
    size_t i, j, offset, t;

    offset = 0;

    for (i = j = size-1, t = 0; i < size-1 || j;) {
        if (is_empty(buffer[i][j])) {
            buffer[i][j] = get_color_from_bit(bitset_get(code, t++));
        }

        switch ((size-j-1 - offset) & 3) {
            case 0:
            case 2: --j; break;
            case 1:
                if (i > 0) {
                    --i;
                    ++j;
                } else {
                    --j;
                }
                break;
            case 3:
                if (i < size-1) {
                    ++i;
                    ++j;
                } else {
                    --j;
                }
                break;
            default:;
        }

        if (j == TIMING_PATTERN) {
            --j;
            ++offset;
        }
    }
}
