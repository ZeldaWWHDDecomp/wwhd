/* jutils.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c utility routines for explicit 32-bit Cafe arithmetic and guest memory.
 */
#pragma once
#include "libjpeg/jpeglib.h"
s32 jpeg_scaled_dct_size(s32, s32, s32);
s32 jpeg_scaled_dimension(s32, s32, s32);

extern const be<s32> (&jpeg_natural_order)[80];

void jpeg_copy_bytes(void *, const void *, u32);
extern const be<s32> (&jpeg_natural_order7)[65];
extern const be<s32> (&jpeg_natural_order6)[52];
extern const be<s32> (&jpeg_natural_order5)[41];
extern const be<s32> (&jpeg_natural_order4)[32];
extern const be<s32> (&jpeg_natural_order3)[25];
extern const be<s32> (&jpeg_natural_order2)[20];
