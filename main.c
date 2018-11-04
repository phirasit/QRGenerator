#include "include/QR.h"
#include "include/QRImage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 2000

char* output_filename = "output.bmp";

char* inp;
size_t length;

ERROR_CORRECT_LEVEL ec_level;
size_t version = 1;

int is_set_input, is_set_ec_level, is_set_version;

void parse_arguments(int argc, char *argv[]);
void header(void);
void display_help(int argc, char *argv[]);

int set_input_filename(char* buffer);
int set_input(char* buffer);
int set_error_correct(char* buffer);
int set_output_filename(char* buffer);
int set_version(char* buffer);

int main(int argc, char *argv[]) {
    char buffer[MAX_LENGTH];
    QR qr_code;
    size_t i;

    parse_arguments(argc, argv);
    header();

// read input

    // read error correction level
    while (!is_set_ec_level) {
        is_set_ec_level = 1;

        fprintf(stderr, "Input Error Correction Level [LMQH]: ");
        fflush(stderr);

        if (fgets(buffer, MAX_LENGTH, stdin) == 0) {
            fprintf(stderr, "Cannot read input from stdin\n");
            exit(1);
        }

        buffer[strcspn(buffer, "\r\n")] = 0;
        if (!set_error_correct(buffer)) {
            fprintf(stderr, "Invalid Error Correct type\n");
        }
    }

    // read input
    while (!is_set_input) {
        fprintf(stderr, "Input Text: ");
        fflush(stderr);

        if (fgets(buffer, MAX_LENGTH, stdin) == 0) {
            fprintf(stderr, "Cannot read input from stdin\n");
            exit(1);
        }

        buffer[strcspn(buffer, "\r\n")] = 0;

        if (!set_input(buffer)) {
            fprintf(stderr, "Invalid input\n");
        }
    }

    fprintf(stderr, "Input text : \"%s\"\n", inp);
    fprintf(stderr, "Text length : %zu\n", length);

    // set version
    if (!is_set_version) {
        set_version("1");
    }

// generate QR code

    qr_code = generate_qr_code(inp, length, version, ec_level);

    fprintf(stderr, "QR CODE\n");
    for (i = 0; i < qr_code.size; ++i) {
        printf("%s\n", qr_code.data[i]);
    }
    fflush(stdout);

// draw image

    draw_qr_code(BMP, 10, output_filename, qr_code);

    return 0;
}

void parse_arguments(int argc, char *argv[]) {
    size_t i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // get help page
            display_help(argc, argv);
            exit(0);
        } else if (strcmp(argv[i], "-out") == 0 || strcmp(argv[i], "--output") == 0) {
            // set output filename
            if (i+1 < argc && set_output_filename(argv[i+1])) {
                i += 2;
            } else {
                fprintf(stderr, "invalid output filename\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-in") == 0 || strcmp(argv[i], "--input") == 0) {
            // set input filename
            if (i + 1 < argc && set_input_filename(argv[i+1])) {
                i += 2;
            } else {
                fprintf(stderr, "invalid input\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--text") == 0) {
            // set input text
            if (i + 1 < argc && set_input(argv[i+1])) {
                i += 2;
            } else {
                fprintf(stderr, "invalid input\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-ec") == 0 || strcmp(argv[i], "--ec") == 0) {
            // set error correct level
            if (i + 1 < argc && set_error_correct(argv[i+1])) {
                i += 2;
            } else {
                fprintf(stderr, "invalid error correct code\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            // set error correct level
            if (i + 1 < argc && set_version(argv[i+1])) {
                i += 2;
            } else {
                fprintf(stderr, "invalid version\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "undefined argument : %s\n", argv[i]);
            display_help(argc, argv);
            exit(1);
        }
    }
}

void header(void) {
    fprintf(stderr, "QR CODE generator\n");
    fprintf(stderr, "Implemented by Phirasit Charoenchitseriwong\n");
}

void display_help(int argc, char *argv[]) {

    fprintf(stderr, "%s\n", argv[0]);
    fprintf(stderr, "usage %s : [ -h | -in [file] | -out [file] | -t [input] | -ec [LHMQ]\n", argv[0]);
    fprintf(stderr, " -h, --help : help page\n");
    fprintf(stderr, " -in, --input [filename] : set input filename\n");
    fprintf(stderr, " -out, --output [filename] : set output filename\n");
    fprintf(stderr, " -t, --text [input] : set input text\n");
    fprintf(stderr, " -v, --version [version] : set default version\n");

    fprintf(stderr, " -ec, --ec [LHMQ] : set qr error correct level\n");
    fprintf(stderr, "   ERROR CORRECT LEVEL\n");
    fprintf(stderr, "   L : low (7%%)\n");
    fprintf(stderr, "   M : medium (15%%)\n");
    fprintf(stderr, "   Q : quartile (25%%)\n");
    fprintf(stderr, "   H : high (30%%)\n");

    fflush(stderr);
}

int set_input_filename(char* buffer) {
    FILE *fp;
    size_t size;

    if ((fp = fopen(buffer, "r")) == 0) {
        fprintf(stderr, "Cannot open: %s\n", buffer);
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = (size_t) ftell(fp);
    rewind(fp);

    if ((buffer = (char*) malloc(size)) == 0) {
        fprintf(stderr, "Cannot allocate memory size: %zu\n", size);
        return 0;
    }

    if (fread(buffer, 1, size, fp) != size) {
        fprintf(stderr, "Cannot read file size %zu\n", size);
        return 0;
    }

    fclose(fp);

    return set_input(buffer);
}
int set_input(char* buffer) {
    inp = buffer;
    length = strlen(buffer);
    return is_set_input = 1;
}

int set_error_correct(char* buffer) {

    if (strlen(buffer) != 1) {
        return 0;
    }

    switch (buffer[0]) {
        case 'L': ec_level = L; break;
        case 'M': ec_level = M; break;
        case 'Q': ec_level = Q; break;
        case 'H': ec_level = H; break;
        default: return 0;
    }

    return is_set_ec_level = 1;
}

int set_output_filename(char* buffer) {
    output_filename = buffer;
    return 1;
}

int set_version(char* buffer) {
    char * end;

    version = (size_t) strtol(buffer, &end, 10);
    if (end != buffer + strlen(buffer)) {
        return 0;
    } else {
        return is_set_version = 1;
    }
}
