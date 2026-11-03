/* jutils.c (decompiled WWHD build)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c utility routines for explicit 32-bit Cafe arithmetic and guest memory.
 */
#include "libjpeg/jutils.h"
static const s32 jpeg_natural_order[80] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33,
    40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54,
    47, 55, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order7[65] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33, 40, 48,
    41, 34, 27, 20, 13, 6,  14, 21, 28, 35, 42, 49, 50, 43, 36, 29, 22, 30, 37, 44, 51, 52,
    45, 38, 46, 53, 54, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order6[52] = {0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18,
                                            11, 4,  5,  12, 19, 26, 33, 40, 41, 34, 27, 20, 13,
                                            21, 28, 35, 42, 43, 36, 29, 37, 44, 45, 63, 63, 63,
                                            63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order5[41] = {0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11,
                                            4,  12, 19, 26, 33, 34, 27, 20, 28, 35, 36, 63, 63, 63,
                                            63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order4[32] = {0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 25,
                                            18, 11, 19, 26, 27, 63, 63, 63, 63, 63, 63,
                                            63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order3[25] = {0,  1,  8,  16, 9,  2,  10, 17, 18, 63, 63, 63, 63,
                                            63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
static const s32 jpeg_natural_order2[20] = {0,  1,  8,  9,  63, 63, 63, 63, 63, 63,
                                            63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
/* 0284DFCC */
s32 jdiv_round_up(s32 a, s32 b) { return (s32)ppc_divw((u32)a + (u32)b - 1, (u32)b); }
/* 0284DFDC */
s32 jround_up(s32 a, s32 b) {
    s32 value = (s32)((u32)a + (u32)b - 1);
    s32 quotient = (s32)ppc_divw(value, b);
    return (s32)((u32)quotient * (u32)b);
}
/* 0284DFF8 */
s32 jpeg_scaled_dct_size(s32 block_size, s32 scale_num, s32 scale_denom) {
    // HD: the repeated scaled-DCT rounding expression is an out-of-line helper.
    s32 scaled = jdiv_round_up((s32)((u32)block_size * (u32)scale_num), scale_denom);
    if (scaled > 16)
        return 16;
    return scaled < 1 ? 1 : scaled;
}
/* 0284E040 */
s32 jpeg_scaled_dimension(s32 dimension, s32 denominator, s32 numerator) {
    // HD: scaled dimensions share this out-of-line multiply/round helper.
    return jdiv_round_up((s32)((u32)dimension * (u32)numerator), denominator);
}
void jpeg_copy_bytes(void *dst, const void *src, u32 size);
void jpeg_fill_bytes(void *dst, s32 byte, u32 size);
/* 0284E048 */
void jcopy_sample_rows(JSAMPARRAY input_array, s32 source_row, JSAMPARRAY output_array,
                       s32 dest_row, s32 num_rows, u32 num_cols) {
    input_array += source_row;
    output_array += dest_row;
    while (num_rows > 0) {
        JSAMPROW inptr = *input_array++;
        JSAMPROW outptr = *output_array++;
        jpeg_copy_bytes(outptr, inptr, num_cols);
        --num_rows;
    }
}
/* 0284E0C0 */
void jzero_far(void *target, u32 bytestozero) { jpeg_fill_bytes(target, 0, bytestozero); }
/* WWHD glue
data jpeg_natural_order 1017B7A8
data jpeg_natural_order7 1017B8E8
data jpeg_natural_order6 1017B9EC
data jpeg_natural_order5 1017BABC
data jpeg_natural_order4 1017BB60
data jpeg_natural_order3 1017BBE0
data jpeg_natural_order2 1017BC44
ext C000A848 void jpeg_copy_bytes(void* dst,const void* src,u32 size)
ext C000A858 void jpeg_fill_bytes(void* dst,s32 byte,u32 size)
*/
