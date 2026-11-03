/* jdinput.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c input control to guest storage and Cafe outlined scaling helpers.
 */
/* jdinput.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c input control to guest storage and Cafe outlined scaling helpers.
 */
#pragma once
#include "libjpeg/jpegint_decoder.h"
struct my_input_controller : jpeg_input_controller {
    be<s32> inheaders;
};
WWHD_OFFSET(my_input_controller, inheaders, 0x18);
WWHD_SIZE(my_input_controller, 0x1C);
static void initial_setup(j_decompress_ptr);
static void per_scan_setup(j_decompress_ptr);
static void latch_quant_tables(j_decompress_ptr);
static void start_input_pass(j_decompress_ptr);
static s32 consume_markers(j_decompress_ptr);
static void finish_input_pass(j_decompress_ptr);
static void reset_input_controller(j_decompress_ptr);
