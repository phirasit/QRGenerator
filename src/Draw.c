//
// Created by phirasit on 9/28/18.
//

#include "QRImage.h"

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

void draw_bmp(FILE *fp, size_t px_size, QR qr);
void draw_jpg(FILE *fp, size_t px_size, QR qr);
void draw_png(FILE *fp, size_t px_size, QR qr);

void draw_qr_code(IMAGE_TYPE img_type, size_t px_size, char* const filename, const QR qr) {

    FILE *fp;

    if ((fp = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Cannot open file : %s\n", filename);
        return;
    }

    switch (img_type) {
        case BMP:
            draw_bmp(fp, px_size, qr);
            break;
        case JPG:
            break;
        case PNG:
            break;
        default:;
    }

    fclose(fp);

    fprintf(stderr, "create image : %s\n", filename);
}

void draw_bmp(FILE *fp, size_t px_size, const QR qr) {

    static unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    static unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    static unsigned char bmppad[3] = {0,0,0};

    static const int BORDER = 4;

    unsigned char* img;
    size_t img_size, file_size, offset;
    size_t i, j, r;

    img_size = px_size * (qr.size + 2 * BORDER);
    file_size = 54 + 3 * img_size * img_size;

    img = (unsigned char *)malloc(offset = 3 * img_size * img_size);
    memset(img, 255, offset);

    for (i = 0; i < qr.size; ++i) {
        for (j = 0; j < qr.size; ++j) {
            for (r = 0; r < px_size; ++r) {
                offset = img_size * (px_size * (i + BORDER) + r) + BORDER * px_size;
                memset(img + 3 * (offset + j * px_size),
                       qr.data[i][j] == get_black() ? 0 : 255,
                       3 * px_size);
            }
        }
    }

    bmpfileheader[ 2] = (unsigned char)(file_size      );
    bmpfileheader[ 3] = (unsigned char)(file_size >>  8);
    bmpfileheader[ 4] = (unsigned char)(file_size >> 16);
    bmpfileheader[ 5] = (unsigned char)(file_size >> 24);

    bmpinfoheader[ 4] = (unsigned char)(img_size      );
    bmpinfoheader[ 5] = (unsigned char)(img_size >>  8);
    bmpinfoheader[ 6] = (unsigned char)(img_size >> 16);
    bmpinfoheader[ 7] = (unsigned char)(img_size >> 24);
    bmpinfoheader[ 8] = (unsigned char)(img_size      );
    bmpinfoheader[ 9] = (unsigned char)(img_size >>  8);
    bmpinfoheader[10] = (unsigned char)(img_size >> 16);
    bmpinfoheader[11] = (unsigned char)(img_size >> 24);

    fwrite(bmpfileheader, 1, 14, fp);
    fwrite(bmpinfoheader, 1, 40, fp);
    for (i = 0; i < img_size; ++i) {
        fwrite(img + (img_size * (img_size-i-1) * 3), 3, img_size, fp);
        fwrite(bmppad, 1, (4 - (img_size*3)%4) % 4, fp);
    }

    free(img);
    fclose(fp);
}
