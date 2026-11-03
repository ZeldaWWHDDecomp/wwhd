/* jddctmgr.c (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2002-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeg_dct_types.h"
#include "libjpeg/jidctfst.h"
#include "libjpeg/jidctint.h"
#define PROVIDE_ISLOW_TABLES
struct my_idct_controller { jpeg_inverse_dct pub; be<s32> cur_method[10]; };
using my_idct_ptr=my_idct_controller*;
union multiplier_table { be<ISLOW_MULT_TYPE> islow_array[64]; be<IFAST_MULT_TYPE> ifast_array[64]; };
WWHD_OFFSET(my_idct_controller,cur_method,0x2C);
WWHD_SIZE(my_idct_controller,0x54);
WWHD_SIZE(multiplier_table,0x100);
static void* jpeg_clear_memory(void* dest,s32 value,u32 size);
static void start_pass (j_decompress_ptr cinfo);
void jinit_inverse_dct (j_decompress_ptr cinfo);
