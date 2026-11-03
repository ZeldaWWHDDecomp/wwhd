/* jcmainct.h (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c strip-buffer controller to guest storage and callbacks.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct my_main_controller : jpeg_c_main_controller {
    be<u32> cur_iMCU_row;
    be<u32> rowgroup_ctr;
    be<s32> suspended;
    be<s32> pass_mode;
    gptr<gptr<be<u8>>> buffer[MAX_COMPONENTS];
};
WWHD_OFFSET(my_main_controller, cur_iMCU_row, 0x8);
WWHD_OFFSET(my_main_controller, rowgroup_ctr, 0xC);
WWHD_OFFSET(my_main_controller, suspended, 0x10);
WWHD_OFFSET(my_main_controller, pass_mode, 0x14);
WWHD_OFFSET(my_main_controller, buffer, 0x18);
WWHD_SIZE(my_main_controller, 0x40);
