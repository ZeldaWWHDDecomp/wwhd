/* jerror.c declarations (decompiled WWHD build)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
static void error_exit(j_common_ptr cinfo);
static void output_message(j_common_ptr cinfo);
static void emit_message(j_common_ptr cinfo,s32 msg_level);
static void format_message(j_common_ptr cinfo,char* buffer);
static void reset_error_mgr(j_common_ptr cinfo);
jpeg_error_mgr* jpeg_std_error(jpeg_error_mgr* err);
static void jpeg_console_message(const char* format,const char* buffer);
static s32 format_string_message(char* buffer,const char* format,const char* text);
static s32 format_integer_message(char* buffer,const char* format,s32 p0,s32 p1,s32 p2,s32 p3,s32 p4,s32 p5,s32 p6,s32 p7);
struct jpeg_message_buffer { be<char> characters[200]; };
WWHD_SIZE(jpeg_message_buffer,200);
using jpeg_message_ptr=gptr<char>;
