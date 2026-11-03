/* jcmaster.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2003-2011 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c master control to the Cafe build without encoder scaling or scan scripts.
 */
#pragma once
#include "libjpeg/jpeglib.h"
enum c_pass_type { main_pass, huff_opt_pass, output_pass };
struct my_comp_master : jpeg_comp_master {
    be<s32> pass_type;
    be<s32> pass_number;
    be<s32> total_passes;
    be<s32> scan_number;
};
WWHD_OFFSET(my_comp_master, pass_type, 0x14);
WWHD_OFFSET(my_comp_master, pass_number, 0x18);
WWHD_OFFSET(my_comp_master, total_passes, 0x1C);
WWHD_OFFSET(my_comp_master, scan_number, 0x20);
WWHD_SIZE(my_comp_master, 0x24);
void jpeg_calc_jpeg_dimensions(j_compress_ptr);
static void jpeg_calc_trans_dimensions(j_compress_ptr);
static void initial_setup(j_compress_ptr, boolean);
static void select_scan_parameters(j_compress_ptr);
static void per_scan_setup(j_compress_ptr);
static void prepare_for_pass(j_compress_ptr);
static void pass_startup(j_compress_ptr);
static void finish_pass_master(j_compress_ptr);
