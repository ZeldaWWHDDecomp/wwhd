/* jcmarker.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2003-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c marker serialization to guest storage and callbacks.
 */
#pragma once
#include "libjpeg/jpeglib.h"
enum JPEG_MARKER {
    M_SOF0 = 0xC0,
    M_SOF1 = 0xC1,
    M_SOF2 = 0xC2,
    M_DHT = 0xC4,
    M_SOF9 = 0xC9,
    M_SOF10 = 0xCA,
    M_DAC = 0xCC,
    M_SOI = 0xD8,
    M_EOI = 0xD9,
    M_SOS = 0xDA,
    M_DQT = 0xDB,
    M_DRI = 0xDD,
    M_APP0 = 0xE0,
    M_APP14 = 0xEE
};
struct my_marker_writer : jpeg_marker_writer {
    be<u32> last_restart_interval;
};
WWHD_OFFSET(my_marker_writer, last_restart_interval, 0x1C);
WWHD_SIZE(my_marker_writer, 0x20);
static void emit_byte(j_compress_ptr, s32);
static void emit_marker(j_compress_ptr, s32);
static void emit_2bytes(j_compress_ptr, s32);
static s32 emit_dqt(j_compress_ptr, s32);
static void emit_dht(j_compress_ptr, s32, boolean);
static void emit_dac(j_compress_ptr);
static void emit_dri(j_compress_ptr);
static void emit_sof(j_compress_ptr, s32);
static void emit_sos(j_compress_ptr);
static void emit_pseudo_sos(j_compress_ptr);
static void emit_jfif_app0(j_compress_ptr);
static void emit_adobe_app14(j_compress_ptr);
static void write_marker_header(j_compress_ptr, s32, u32);
static void write_marker_byte(j_compress_ptr, s32);
static void write_file_header(j_compress_ptr);
static void write_frame_header(j_compress_ptr);
static void write_scan_header(j_compress_ptr);
static void write_file_trailer(j_compress_ptr);
static void write_tables_only(j_compress_ptr);
