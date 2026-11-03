/* jdmainct.c (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeg_dct_types.h"
struct my_main_controller {
    jpeg_d_main_controller pub;
    gptr<gptr<be<JSAMPLE>>> buffer[10];
    be<boolean> buffer_full; be<JDIMENSION> rowgroup_ctr;
    gptr<gptr<gptr<be<JSAMPLE>>>> xbuffer[2];
    be<s32> whichptr,context_state;
    be<JDIMENSION> rowgroups_avail,iMCU_row_ctr;
};
using my_main_ptr=my_main_controller*;
WWHD_OFFSET(my_main_controller,buffer_full,0x30);
WWHD_OFFSET(my_main_controller,rowgroup_ctr,0x34);
WWHD_OFFSET(my_main_controller,xbuffer,0x38);
WWHD_SIZE(my_main_controller,0x50);
#define CTX_PREPARE_FOR_IMCU 0
#define CTX_PROCESS_IMCU 1
#define CTX_POSTPONED_ROW 2
static void alloc_funny_pointers (j_decompress_ptr cinfo)
/* Allocate space for the funny pointer lists.
 * This is done only once, not once per pass.
 */;
static void make_funny_pointers (j_decompress_ptr cinfo)
/* Create the funny pointer lists discussed in the comments above.
 * The actual workspace is already allocated (in main->buffer),
 * and the space for the pointer lists is allocated too.
 * This routine just fills in the curiously ordered lists.
 * This will be repeated at the beginning of each pass.
 */;
static void set_wraparound_pointers (j_decompress_ptr cinfo)
/* Set up the "wraparound" pointers at top and bottom of the pointer lists.
 * This changes the pointer list state from top-of-image to the normal state.
 */;
static void set_bottom_pointers (j_decompress_ptr cinfo)
/* Change the pointer lists to duplicate the last sample row at the bottom
 * of the image.  whichptr indicates which xbuffer holds the final iMCU row.
 * Also sets rowgroups_avail to indicate number of nondummy row groups in row.
 */;
static void process_data_context_main (j_decompress_ptr cinfo,
			   JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
			   JDIMENSION out_rows_avail);
static void process_data_simple_main (j_decompress_ptr cinfo,
			  JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
			  JDIMENSION out_rows_avail);
static void process_data_crank_post (j_decompress_ptr cinfo,
			 JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
			 JDIMENSION out_rows_avail);
static void start_pass_main (j_decompress_ptr cinfo, J_BUF_MODE pass_mode);
void jinit_d_main_controller (j_decompress_ptr cinfo, boolean need_full_buffer);
