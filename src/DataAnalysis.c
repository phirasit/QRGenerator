#include "Bitset.h"
#include "QR.h"

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define MODE_LENGTH 4u

static size_t maximum_capacity[][4] = {
        {41, 25, 17, 10},
        {34, 20, 14, 8},
        {27, 16, 11, 7},
        {17, 10, 7, 4},
        {77, 47, 32, 20},
        {63, 38, 26, 16},
        {48, 29, 20, 12},
        {34, 20, 14, 8},
        {127, 77, 53, 32},
        {101, 61, 42, 26},
        {77, 47, 32, 20},
        {58, 35, 24, 15},
        {187, 114, 78, 48},
        {149, 90, 62, 38},
        {111, 67, 46, 28},
        {82, 50, 34, 21},
        {255, 154, 106, 65},
        {202, 122, 84, 52},
        {144, 87, 60, 37},
        {106, 64, 44, 27},
        {322, 195, 134, 82},
        {255, 154, 106, 65},
        {178, 108, 74, 45},
        {139, 84, 58, 36},
        {370, 224, 154, 95},
        {293, 178, 122, 75},
        {207, 125, 86, 53},
        {154, 93, 64, 39},
        {461, 279, 192, 118},
        {365, 221, 152, 93},
        {259, 157, 108, 66},
        {202, 122, 84, 52},
        {552, 335, 230, 141},
        {432, 262, 180, 111},
        {312, 189, 130, 80},
        {235, 143, 98, 60},
        {652, 395, 271, 167},
        {513, 311, 213, 131},
        {364, 221, 151, 93},
        {288, 174, 119, 74},
        {772, 468, 321, 198},
        {604, 366, 251, 155},
        {427, 259, 177, 109},
        {331, 200, 137, 85},
        {883, 535, 367, 226},
        {691, 419, 287, 177},
        {489, 296, 203, 125},
        {374, 227, 155, 96},
        {1022, 619, 425, 262},
        {796, 483, 331, 204},
        {580, 352, 241, 149},
        {427, 259, 177, 109},
        {1101, 667, 458, 282},
        {871, 528, 362, 223},
        {621, 376, 258, 159},
        {468, 283, 194, 120},
        {1250, 758, 520, 320},
        {991, 600, 412, 254},
        {703, 426, 292, 180},
        {530, 321, 220, 136},
        {1408, 854, 586, 361},
        {1082, 656, 450, 277},
        {775, 470, 322, 198},
        {602, 365, 250, 154},
        {1548, 938, 644, 397},
        {1212, 734, 504, 310},
        {876, 531, 364, 224},
        {674, 408, 280, 173},
        {1725, 1046, 718, 442},
        {1346, 816, 560, 345},
        {948, 574, 394, 243},
        {746, 452, 310, 191},
        {1903, 1153, 792, 488},
        {1500, 909, 624, 384},
        {1063, 644, 442, 272},
        {813, 493, 338, 208},
        {2061, 1249, 858, 528},
        {1600, 970, 666, 410},
        {1159, 702, 482, 297},
        {919, 557, 382, 235},
        {2232, 1352, 929, 572},
        {1708, 1035, 711, 438},
        {1224, 742, 509, 314},
        {969, 587, 403, 248},
        {2409, 1460, 1003, 618},
        {1872, 1134, 779, 480},
        {1358, 823, 565, 348},
        {1056, 640, 439, 270},
        {2620, 1588, 1091, 672},
        {2059, 1248, 857, 528},
        {1468, 890, 611, 376},
        {1108, 672, 461, 284},
        {2812, 1704, 1171, 721},
        {2188, 1326, 911, 561},
        {1588, 963, 661, 407},
        {1228, 744, 511, 315},
        {3057, 1853, 1273, 784},
        {2395, 1451, 997, 614},
        {1718, 1041, 715, 440},
        {1286, 779, 535, 330},
        {3283, 1990, 1367, 842},
        {2544, 1542, 1059, 652},
        {1804, 1094, 751, 462},
        {1425, 864, 593, 365},
        {3517, 2132, 1465, 902},
        {2701, 1637, 1125, 692},
        {1933, 1172, 805, 496},
        {1501, 910, 625, 385},
        {3669, 2223, 1528, 940},
        {2857, 1732, 1190, 732},
        {2085, 1263, 868, 534},
        {1581, 958, 658, 405},
        {3909, 2369, 1628, 1002},
        {3035, 1839, 1264, 778},
        {2181, 1322, 908, 559},
        {1677, 1016, 698, 430},
        {4158, 2520, 1732, 1066},
        {3289, 1994, 1370, 843},
        {2358, 1429, 982, 604},
        {1782, 1080, 742, 457},
        {4417, 2677, 1840, 1132},
        {3486, 2113, 1452, 894},
        {2473, 1499, 1030, 634},
        {1897, 1150, 790, 486},
        {4686, 2840, 1952, 1201},
        {3693, 2238, 1538, 947},
        {2670, 1618, 1112, 684},
        {2022, 1226, 842, 518},
        {4965, 3009, 2068, 1273},
        {3909, 2369, 1628, 1002},
        {2805, 1700, 1168, 719},
        {2157, 1307, 898, 553},
        {5253, 3183, 2188, 1347},
        {4134, 2506, 1722, 1060},
        {2949, 1787, 1228, 756},
        {2301, 1394, 958, 590},
        {5529, 3351, 2303, 1417},
        {4343, 2632, 1809, 1113},
        {3081, 1867, 1283, 790},
        {2361, 1431, 983, 605},
        {5836, 3537, 2431, 1496},
        {4588, 2780, 1911, 1176},
        {3244, 1966, 1351, 832},
        {2524, 1530, 1051, 647},
        {6153, 3729, 2563, 1577},
        {4775, 2894, 1989, 1224},
        {3417, 2071, 1423, 876},
        {2625, 1591, 1093, 673},
        {6479, 3927, 2699, 1661},
        {5039, 3054, 2099, 1292},
        {3599, 2181, 1499, 923},
        {2735, 1658, 1139, 701},
        {6743, 4087, 2809, 1729},
        {5313, 3220, 2213, 1362},
        {3791, 2298, 1579, 972},
        {2927, 1774, 1219, 750},
        {7089, 4296, 2953, 1817},
        {5596, 3391, 2331, 1435},
        {3993, 2420, 1663, 1024},
        {3057, 1852, 1273, 784},
};

static char * const digit = "0123456789";
static char * const alpha = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

static size_t const numeric_offset[] = {0u, 4, 7};
static size_t const alpha_offset[] = {0u, 6};

size_t get_alphanumeric_from_char(char ch) {
    if (isdigit(ch)) {
        return (size_t) (ch - '0');
    } else if (isupper(ch)) {
        return (size_t) (10 + ch - 'A');
    } else {
        char* pos = strchr(alpha, ch);
        return pos ? pos - alpha : -1;
    }
}

size_t get_int_from_string(char const * begin, char const * const end) {
    size_t ans = 0;
    while (begin != end) {
        ans = 10u * ans + *(begin++) - '0';
    }
    return ans;
}

TEXT_TYPE get_text_type(char* text) {
    size_t len;
    len = strlen(text);
    if (len == strspn(text, digit)) {
        return NUMERIC;
    }
    if (len == strspn(text, alpha)) {
        return ALPHANUMERIC;
    }
    return BYTE;
}

size_t get_text_type_length(TEXT_TYPE type, size_t version) {
    if (1u <= version && version <= 9u) {
        switch (type) {
            case NUMERIC: return 10u;
            case ALPHANUMERIC: return 9u;
            case BYTE: return 8u;
            case ECI: return 8u;
            default:;
        }
    } else if (10u <= version && version <= 26u) {
        switch (type) {
            case NUMERIC: return 12u;
            case ALPHANUMERIC: return 11u;
            case BYTE: return 16u;
            case ECI: return 10u;
            default:;
        }
    } else if (27u <= version && version <= 40u) {
        switch (type) {
            case NUMERIC: return 14u;
            case ALPHANUMERIC: return 13u;
            case BYTE: return 16u;
            case ECI: return 12u;
            default:;
        }
    }

    fprintf(stderr, "Invalid version [should be between 1-40]\n");
    exit(1);
}

size_t get_mode_indicator(TEXT_TYPE type) {
    switch (type) {
        case NUMERIC: return 1;
        case ALPHANUMERIC: return 2;
        case BYTE: return 4;
        case KANJI: return 8;
        case ECI: return 7;
    }
    fprintf(stderr, "Not a valid mode indicator\n");
    exit(1);
}

size_t get_maximum_capacity(TEXT_TYPE type, size_t version, ERROR_CORRECT_LEVEL ev_level) {
    return maximum_capacity[((version-1) << 2) | ev_level][type];
}

size_t allocate_bin_with_header(unsigned char** bin, size_t* length, size_t bin_len,
                                TEXT_TYPE type, size_t version, ERROR_CORRECT_LEVEL ec_level) {
    size_t offset, total;

    offset = MODE_LENGTH + get_text_type_length(type, version);
    total = get_total_number_of_codewords(version, ec_level);
    fprintf(stderr, "Total number of codewords = %zu (offset %zu)\n", total, offset);

    if ((*bin = (unsigned char *) malloc(total)) == NULL) {
        fprintf(stderr, "Cannot allocate memory size = %zu\n", total);
        exit(1);
    }

    *length = total << 3;
    return offset;
}

unsigned char* to_binary(size_t* total_length, char* text, TEXT_TYPE type, size_t version, ERROR_CORRECT_LEVEL ec_level) {

    unsigned char *bin = NULL;
    size_t text_len, offset;
    size_t i, idx = 0;

    text_len = strlen(text);

    switch (type) {
        case NUMERIC:
            offset = numeric_offset[text_len % 3u];
            idx = allocate_bin_with_header(&bin, total_length, 10u * (text_len / 3u) + offset, NUMERIC, version, ec_level);
            for (i = 0; i < text_len; i += 3u) {
                if (i + 2u < text_len) {
                    idx = bitset_set_range(bin, idx, 10u, get_int_from_string(text + i, text + i + 3u));
                } else {
                    idx = bitset_set_range(bin, idx, offset, get_int_from_string(text + i, text + text_len));
                }
            }
            break;
        case ALPHANUMERIC:
            offset = alpha_offset[text_len & 1u];
            idx = allocate_bin_with_header(&bin, total_length, 11u * (text_len >> 1) + offset, ALPHANUMERIC, version, ec_level);
            for (i = 0; i < text_len; i += 2u) {
                if (i + 1u < text_len) {
                    idx = bitset_set_range(bin, idx, 11u,
                            get_alphanumeric_from_char(text[i]) * 45u + get_alphanumeric_from_char(text[i+1]));
                } else {
                    idx = bitset_set_range(bin, idx, offset, get_alphanumeric_from_char(text[i]));
                }
            }
            break;
        case BYTE:
            idx = allocate_bin_with_header(&bin, total_length, text_len << 3, BYTE, version, ec_level);
            for (i = 0; i < text_len; ++i) {
                idx = bitset_set_range(bin, idx, 8u, (size_t) text[i]);
            }
            break;
        case KANJI:
            break;
        default:
            fprintf(stderr, "No valid converter\n");
            return NULL;
    }

    // add header
    bitset_set_range(bin, 0, MODE_LENGTH, get_mode_indicator(type));
    bitset_set_range(bin, MODE_LENGTH, get_text_type_length(type, version), text_len);

    // padding the remaining block
    bin[idx >> 3] &= ~((1 << (idx & 7)) - 1);
    idx = (idx + 7) >> 3 << 3;
    for (i = 0; idx < *total_length; i = i^1, idx += 8u) {
        bin[idx >> 3] = (unsigned char) (i ? 0x11 : 0xec);
    }

    return bin;
}
