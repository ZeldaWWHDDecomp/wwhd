/* jfdctfst.c (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2003-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeg_dct_types.h"
void jpeg_fdct_ifast (be<DCTELEM>* data, JSAMPARRAY sample_data, JDIMENSION start_col);
