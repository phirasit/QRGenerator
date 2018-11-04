//
// Created by phirasit on 9/17/18.
//

#ifndef QR_QR_H
#define QR_QR_H

#include <stdlib.h>

typedef struct {
    size_t version;
    size_t size;
    unsigned char** data;
} QR;

typedef enum {
    NUMERIC,
    ALPHANUMERIC,
    BYTE,
    KANJI,
    ECI,
    // STRUCTURED_APPEND,
    // FNC1,
} TEXT_TYPE;

typedef enum {
    L, // 7% of data
    M, // 15% of data
    Q, // 25% of data
    H, // 30% of data
} ERROR_CORRECT_LEVEL;

// QR.c
QR generate_qr_code(char* inp, size_t length, size_t min_version, ERROR_CORRECT_LEVEL ec_level);

// DataAnalysis.c
TEXT_TYPE get_text_type(char* text);
size_t get_maximum_capacity(TEXT_TYPE type, size_t version, ERROR_CORRECT_LEVEL ev_level);
unsigned char* to_binary(size_t* total_length, char* text,
        TEXT_TYPE type, size_t version, ERROR_CORRECT_LEVEL ec_level);

// ErrorCorrectCode.c
size_t get_number_of_error_correct_per_block(size_t version, ERROR_CORRECT_LEVEL ec_level);
size_t get_number_of_groups(size_t version, ERROR_CORRECT_LEVEL ec_level);
size_t get_number_of_blocks(size_t version, ERROR_CORRECT_LEVEL ec_level, size_t group);
size_t get_total_number_of_blocks(size_t version, ERROR_CORRECT_LEVEL ec_level);
size_t get_number_of_codewords(size_t version, ERROR_CORRECT_LEVEL ec_level, size_t group);
size_t get_total_number_of_codewords(size_t version, ERROR_CORRECT_LEVEL ec_level);

unsigned char* create_interleaved_block(unsigned char* bin, size_t* length, size_t version, ERROR_CORRECT_LEVEL ec_level);

// ModulePlacement.c
unsigned char get_black(void);
unsigned char get_white(void);
unsigned char get_color_from_bit(size_t b);
unsigned char invert_color(unsigned char color);

int is_finder_cell(size_t size, size_t r, size_t c);
int is_timing_cell(size_t r, size_t c);
int is_data_cell(size_t version, size_t size, size_t r, size_t c);

unsigned char** generate_qr_code_from_block(size_t* size, size_t version, size_t length, unsigned char* code);

// Masking.c
void add_mask_info(unsigned char** buffer, size_t size, size_t version, ERROR_CORRECT_LEVEL ec_level);

#endif //QR_QR_H
