//
// Created by phirasit on 9/28/18.
//

#ifndef QR_QRIMAGE_H
#define QR_QRIMAGE_H

#include "QR.h"

#include <stdlib.h>

typedef enum {
    BMP,
    JPG,
    PNG,
} IMAGE_TYPE;

// Draw.c
void draw_qr_code(IMAGE_TYPE img_type, size_t px_size, char* filename, QR qr);

#endif //QR_QRIMAGE_H
