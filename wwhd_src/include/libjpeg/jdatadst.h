/* jdatadst.h (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c memory destination to Cafe caller-owned buffers.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct my_mem_destination_mgr : jpeg_destination_mgr {
    gptr<gptr<be<u8>>> outbuffer;
    gptr<be<u32>> outsize;
    gptr<be<u8>> newbuffer;
    gptr<be<u8>> buffer;
    be<u32> bufsize;
};
WWHD_OFFSET(my_mem_destination_mgr, outbuffer, 0x14);
WWHD_OFFSET(my_mem_destination_mgr, outsize, 0x18);
WWHD_OFFSET(my_mem_destination_mgr, newbuffer, 0x1C);
WWHD_OFFSET(my_mem_destination_mgr, buffer, 0x20);
WWHD_OFFSET(my_mem_destination_mgr, bufsize, 0x24);
WWHD_SIZE(my_mem_destination_mgr, 0x28);
