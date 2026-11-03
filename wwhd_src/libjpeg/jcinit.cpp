/* jcinit.c (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c compressor module selection to guest method tables.
 */
#include "libjpeg/jpeglib.h"
void jinit_c_master_control(j_compress_ptr, boolean);
void jinit_color_converter(j_compress_ptr);
void jinit_downsampler(j_compress_ptr);
void jinit_c_prep_controller(j_compress_ptr, boolean);
void jinit_forward_dct(j_compress_ptr);
void jinit_arith_encoder(j_compress_ptr);
void jinit_huff_encoder(j_compress_ptr);
void jinit_c_coef_controller(j_compress_ptr, boolean);
void jinit_c_main_controller(j_compress_ptr, boolean);
void jinit_marker_writer(j_compress_ptr);
/* 0284E634 */
void jinit_compress_master(j_compress_ptr cinfo) {
    jinit_c_master_control(cinfo, 0);
    if (!cinfo->raw_data_in) {
        jinit_color_converter(cinfo);
        jinit_downsampler(cinfo);
        jinit_c_prep_controller(cinfo, 0);
    }
    jinit_forward_dct(cinfo);
    if (cinfo->arith_code)
        jinit_arith_encoder(cinfo);
    else
        jinit_huff_encoder(cinfo);
    jinit_c_coef_controller(cinfo, cinfo->num_scans > 1 || cinfo->optimize_coding);
    jinit_c_main_controller(cinfo, 0);
    jinit_marker_writer(cinfo);
    cinfo->mem->realize_virt_arrays(cinfo);
    cinfo->marker->write_file_header(cinfo);
}
/* WWHD glue
ext 0285DCA0 void jinit_color_converter(j_compress_ptr cinfo)
ext 0285E3C8 void jinit_forward_dct(j_compress_ptr cinfo)
ext 0285CEC0 void jinit_arith_encoder(j_compress_ptr cinfo)
ext 02860FF4 void jinit_huff_encoder(j_compress_ptr cinfo)
ext 0285D744 void jinit_c_coef_controller(j_compress_ptr cinfo,boolean need_full_buffer)
*/
