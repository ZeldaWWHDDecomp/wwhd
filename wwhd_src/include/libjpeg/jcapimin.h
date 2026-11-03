/* jcapimin.h (decompiled WWHD declarations)
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * Modified 2003-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c compression API to Cafe allocation and guest method tables.
 */
#pragma once
#include "libjpeg/jpeglib.h"
void jpeg_CreateCompress(j_compress_ptr, s32, u32);
void jpeg_suppress_tables(j_compress_ptr, s32);
