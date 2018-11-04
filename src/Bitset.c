//
// Created by phirasit on 9/19/18.
//
#include "Bitset.h"

#include <stdlib.h>

size_t bitset_get(unsigned char * const ptr, size_t idx) {
    return (size_t) ((ptr[idx >> 3] >> (7 - (idx & 7u))) & 1);
}

void bitset_set(unsigned char * const ptr, size_t idx, size_t value) {
    ptr[idx >> 3] &= ~(1 << (7 - (idx & 7u)));
    ptr[idx >> 3] |= (value & 1) << (7 - (idx & 7u));
}

size_t bitset_get_range(unsigned char* ptr, size_t a, size_t len) {
    size_t ans = 0;

    while (len > 0) {
        ans = (ans << 1) | bitset_get(ptr, a++);
        --len;
    }
    return ans;
}

size_t bitset_set_range(unsigned char* const ptr, size_t idx, size_t len, size_t value)  {
    size_t i, j;

    for (i = len; i; --i, ++idx) {
        j = 7u - (idx & 7u);
        ptr[idx >> 3] &= ~(1 << j);
        ptr[idx >> 3] |= ((value >> (i-1)) & 1) << j;
    }
    return idx;
}

size_t bitset_reverse(unsigned char* const ptr, size_t idx, size_t len) {
    size_t a, b;

    while (len > 1) {
        a = bitset_get(ptr, idx);
        b = bitset_get(ptr, idx + len-1);
        bitset_set(ptr, idx, b);
        bitset_set(ptr, idx + len-1, a);
        ++idx;
        len -= 2;
    }
}