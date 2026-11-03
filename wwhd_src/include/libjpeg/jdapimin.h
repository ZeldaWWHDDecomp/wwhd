/* jdapimin.h (decompiled WWHD declarations)
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c decompression API to Cafe allocation and guest method tables.
 */
#pragma once
#include "libjpeg/jpeglib.h"
static void default_decompress_parms(j_decompress_ptr);
s32 jpeg_consume_input(j_decompress_ptr);
