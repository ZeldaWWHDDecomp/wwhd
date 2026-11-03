/* jmemsys.h (decompiled WWHD build)
 * Copyright (C) 1992-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted declarations use 32-bit guest allocation sizes.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct backing_store_struct;
using backing_store_ptr = backing_store_struct*;
void* jpeg_get_small(j_common_ptr cinfo, u32 sizeofobject);
void jpeg_free_small(j_common_ptr cinfo, void* object, u32 sizeofobject);
void* jpeg_get_large(j_common_ptr cinfo, u32 sizeofobject);
void jpeg_free_large(j_common_ptr cinfo, void* object, u32 sizeofobject);
s32 jpeg_mem_available(j_common_ptr cinfo, s32 min_bytes_needed, s32 max_bytes_needed, s32 already_allocated);
void jpeg_open_backing_store(j_common_ptr cinfo, backing_store_ptr info, s32 total_bytes_needed);
s32 jpeg_mem_init(j_common_ptr cinfo);
void jpeg_mem_term(j_common_ptr cinfo);
