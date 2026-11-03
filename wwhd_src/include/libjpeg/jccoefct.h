/* jccoefct.cpp private layout (decompiled WWHD build)
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct c_coef_controller {
    jpeg_c_coef_controller pub;
    be<JDIMENSION> iMCU_row_num, mcu_ctr;
    be<s32> MCU_vert_offset, MCU_rows_per_iMCU_row;
    gptr<JBLOCK> MCU_buffer[10];
    gptr<jvirt_barray_control> whole_image[10];
};
WWHD_OFFSET(c_coef_controller, MCU_buffer, 0x18);
WWHD_OFFSET(c_coef_controller, whole_image, 0x40);
WWHD_SIZE(c_coef_controller, 0x68);
void jinit_c_coef_controller(j_compress_ptr cinfo, boolean need_full_buffer);
