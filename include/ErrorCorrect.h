//
// Created by phirasit on 9/20/18.
//

#ifndef QR_ERRORCORRECT_H
#define QR_ERRORCORRECT_H

#include "Bitset.h"
#include "QR.h"

#include <stdlib.h>

unsigned char* generate_polynomial(size_t n);
unsigned char* divide_polynomial(size_t len_inp, unsigned char* inp, size_t len_divisor, unsigned char* divisor);
unsigned char* bitset_divide_polynomial(size_t len_inp, size_t inp, size_t len_div, size_t div);

#endif //QR_ERRORCORRECT_H
