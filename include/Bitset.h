//
// Created by phirasit on 9/17/18.
//

#ifndef QR_BITSET_H
#define QR_BITSET_H

#include <stdlib.h>

size_t bitset_get(unsigned char* ptr, size_t idx);
void bitset_set(unsigned char* ptr, size_t idx, size_t value);

size_t bitset_get_range(unsigned char* ptr, size_t a, size_t b);
size_t bitset_set_range(unsigned char* ptr, size_t idx, size_t len, size_t value);

size_t bitset_reverse(unsigned char* ptr, size_t idx, size_t len);

#endif //QR_BITSET_H
