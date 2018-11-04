//
// Created by phirasit on 9/28/18.
//
#include "Bitset.h"
#include "QR.h"

#include <stdio.h>

QR generate_qr_code(char* const inp, size_t length, size_t min_version, ERROR_CORRECT_LEVEL const ec_level) {

    TEXT_TYPE type;

    unsigned char* bin;
    unsigned char* interleaved_block;
    unsigned char** qr_code;
    size_t qr_size;

    size_t i;

// Data Analysis

    // get type
    type = get_text_type(inp);

    // get version
    while (min_version <= 40 && get_maximum_capacity(type, min_version, ec_level) < length) {
        ++min_version;
    }

    if (min_version > 40) {
        fprintf(stderr, "The data is too large (maxmimum = %zu)\n", get_maximum_capacity(type, 40, ec_level));
        exit(1);
    }

    fprintf(stderr, "Minimum version required is @%zu with maximum text size %zu\n", min_version,
            get_maximum_capacity(type, min_version, ec_level));

    // convert to binary
    bin = to_binary(&length, inp, type, min_version, ec_level);

    fprintf(stderr, "Encoded Binary (%zu)\n", length);
    for (i = 0; i < length; ++i) {
        if ((i & 7) == 0) fprintf(stderr, " ");
        fprintf(stderr, "%d", (bin[i >> 3] >> (7u - (i & 7u))) & 1);
    }
    fprintf(stderr, "\n");

// Error Correct Code
// Structure Final Message

    interleaved_block = create_interleaved_block(bin, &length, min_version, ec_level);

    fprintf(stderr, "Encoded Error-Corrected Binary (%zu)\n", length);
    for (i = 0; i < length; ++i) {
        if ((i & 7) == 0) fprintf(stderr, " ");
        fprintf(stderr, "%zu", bitset_get(interleaved_block, i));
    }
    fprintf(stderr, "\n");
    for (i = 0; i < (length >> 3); ++i) {
        fprintf(stderr, "%d ", interleaved_block[i]);
    }
    fprintf(stderr, "\n");

// Module Placement in Matrix

    qr_code = generate_qr_code_from_block(&qr_size, min_version, length, interleaved_block);
    free(interleaved_block);

// Data Masking
// Format and Version Information

    add_mask_info(qr_code, qr_size, min_version, ec_level);

    return (QR) {min_version, qr_size, qr_code};
}
