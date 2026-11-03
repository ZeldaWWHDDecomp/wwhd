/* jdapimin.c (decompiled WWHD build)
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c decompression API to Cafe allocation and guest method tables.
 */
#include "libjpeg/jdapimin.h"
void jpeg_zero_words(void *, u32);
void jinit_memory_mgr(j_common_ptr);
void jinit_marker_reader(j_decompress_ptr);
void jinit_input_controller(j_decompress_ptr);
/* 028520E8 */
void jpeg_CreateDecompress(j_decompress_ptr cinfo, s32 version, u32 structsize) {
    cinfo->mem = nullptr;
    if (version != JPEG_LIB_VERSION)
        jpeg_error2(cinfo, JERR_BAD_LIB_VERSION, JPEG_LIB_VERSION, version);
    if (structsize != sizeof(jpeg_decompress_struct))
        jpeg_error2(cinfo, JERR_BAD_STRUCT_SIZE, sizeof(jpeg_decompress_struct), structsize);
    jpeg_error_mgr *err = cinfo->err;
    void *allocation_heap = cinfo->allocation_heap;
    void *client_data = cinfo->client_data;
    jpeg_zero_words(cinfo, sizeof(jpeg_decompress_struct) / 4);
    // HD: retain the Cafe allocator context across initialization.
    cinfo->allocation_heap = allocation_heap;
    cinfo->is_decompressor = 1;
    cinfo->client_data = client_data;
    cinfo->err = err;
    jinit_memory_mgr(cinfo);
    cinfo->src = nullptr;
    cinfo->progress = nullptr;
    for (s32 i = 0; i < NUM_QUANT_TBLS; ++i)
        cinfo->quant_tbl_ptrs[i] = nullptr;
    for (s32 i = 0; i < NUM_HUFF_TBLS; ++i) {
        cinfo->dc_huff_tbl_ptrs[i] = nullptr;
        cinfo->ac_huff_tbl_ptrs[i] = nullptr;
    }
    cinfo->marker_list = nullptr;
    jinit_marker_reader(cinfo);
    jinit_input_controller(cinfo);
    cinfo->global_state = DSTATE_START;
}
/* 02852210 */
void jpeg_destroy_decompress(j_decompress_ptr cinfo) { jpeg_destroy(cinfo); }
/* 02852214 */
static void default_decompress_parms(j_decompress_ptr cinfo) {
    switch ((s32)cinfo->num_components) {
    case 1:
        cinfo->jpeg_color_space = JCS_GRAYSCALE;
        cinfo->out_color_space = JCS_GRAYSCALE;
        break;
    case 3: {
        s32 color_space = JCS_YCbCr;
        if (!cinfo->saw_JFIF_marker) {
            if (cinfo->saw_Adobe_marker) {
                u32 transform = cinfo->Adobe_transform;
                if (transform == 0)
                    color_space = JCS_RGB;
                else if (transform != 1) {
                    jpeg_error_mgr *err = cinfo->err;
                    auto emit = err->emit_message.load();
                    err->msg_code = JWRN_ADOBE_XFORM;
                    err->msg_parm[0] = transform;
                    emit(cinfo, -1);
                }
            } else {
                jpeg_component_info *components = cinfo->comp_info;
                s32 cid0 = components[0].component_id;
                s32 cid1 = components[1].component_id;
                s32 cid2 = components[2].component_id;
                if (cid0 == 82 && cid1 == 71 && cid2 == 66)
                    color_space = JCS_RGB;
                else if (!(cid0 == 1 && cid1 == 2 && cid2 == 3)) {
                    jpeg_error_mgr *err = cinfo->err;
                    auto emit = err->emit_message.load();
                    err->msg_parm[2] = cid2;
                    err->msg_parm[1] = cid1;
                    err->msg_code = JTRC_UNKNOWN_IDS;
                    err->msg_parm[0] = cid0;
                    emit(cinfo, 1);
                }
            }
        }
        cinfo->jpeg_color_space = color_space;
        cinfo->out_color_space = JCS_RGB;
        break;
    }
    case 4: {
        s32 color_space = JCS_CMYK;
        if (cinfo->saw_Adobe_marker) {
            u32 transform = cinfo->Adobe_transform;
            if (transform != 0) {
                color_space = JCS_YCCK;
                if (transform != 2) {
                    jpeg_error_mgr *err = cinfo->err;
                    auto emit = err->emit_message.load();
                    err->msg_code = JWRN_ADOBE_XFORM;
                    err->msg_parm[0] = transform;
                    emit(cinfo, -1);
                }
            }
        }
        cinfo->jpeg_color_space = color_space;
        cinfo->out_color_space = JCS_CMYK;
        break;
    }
    default:
        cinfo->jpeg_color_space = JCS_UNKNOWN;
        cinfo->out_color_space = JCS_UNKNOWN;
    }
    s32 block_size = cinfo->block_size;
    cinfo->scale_num = block_size;
    cinfo->scale_denom = block_size;
    cinfo->output_gamma = 1.0;
    cinfo->buffered_image = 0;
    cinfo->raw_data_out = 0;
    cinfo->dct_method = JDCT_ISLOW;
    cinfo->do_fancy_upsampling = 1;
    cinfo->do_block_smoothing = 1;
    cinfo->quantize_colors = 0;
    cinfo->dither_mode = JDITHER_FS;
    cinfo->two_pass_quantize = 1;
    cinfo->desired_number_of_colors = 256;
    cinfo->colormap = nullptr;
    cinfo->enable_1pass_quant = 0;
    cinfo->enable_external_quant = 0;
    cinfo->enable_2pass_quant = 0;
}
/* 028525F0 */
s32 jpeg_consume_input(j_decompress_ptr cinfo) {
    switch ((s32)cinfo->global_state) {
    case DSTATE_START:
        cinfo->inputctl->reset_input_controller(cinfo);
        cinfo->src->init_source(cinfo);
        cinfo->global_state = DSTATE_INHEADER;
        [[fallthrough]];
    case DSTATE_INHEADER: {
        s32 retcode = cinfo->inputctl->consume_input(cinfo);
        if (retcode == JPEG_REACHED_SOS) {
            default_decompress_parms(cinfo);
            cinfo->global_state = DSTATE_READY;
        }
        return retcode;
    }
    case DSTATE_READY:
        return JPEG_REACHED_SOS;
    case DSTATE_PRELOAD:
    case DSTATE_PRESCAN:
    case DSTATE_SCANNING:
    case DSTATE_RAW_OK:
    case DSTATE_BUFIMAGE:
    case DSTATE_BUFPOST:
    case DSTATE_STOPPING:
        return cinfo->inputctl->consume_input(cinfo);
    default:
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
        return JPEG_SUSPENDED;
    }
}
/* 0285272C */
s32 jpeg_read_header(j_decompress_ptr cinfo, boolean require_image) {
    if (cinfo->global_state != DSTATE_START && cinfo->global_state != DSTATE_INHEADER)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    s32 retcode = jpeg_consume_input(cinfo);
    if (retcode == JPEG_REACHED_EOI) {
        if (require_image)
            jpeg_error(cinfo, JERR_NO_IMAGE);
        jpeg_abort(cinfo);
        retcode = JPEG_HEADER_TABLES_ONLY;
    }
    return retcode;
}
/* 028527E0 */
boolean jpeg_finish_decompress(j_decompress_ptr cinfo) {
    if ((cinfo->global_state == DSTATE_SCANNING || cinfo->global_state == DSTATE_RAW_OK) &&
        !cinfo->buffered_image) {
        if (cinfo->output_scanline < cinfo->output_height)
            jpeg_error(cinfo, JERR_TOO_LITTLE_DATA);
        cinfo->master->finish_output_pass(cinfo);
        cinfo->global_state = DSTATE_STOPPING;
    } else if (cinfo->global_state == DSTATE_BUFIMAGE)
        cinfo->global_state = DSTATE_STOPPING;
    else if (cinfo->global_state != DSTATE_STOPPING)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    while (!cinfo->inputctl->eoi_reached)
        if (cinfo->inputctl->consume_input(cinfo) == JPEG_SUSPENDED)
            return 0;
    cinfo->src->term_source(cinfo);
    jpeg_abort(cinfo);
    return 1;
}
/* WWHD glue
ext 02856308 void jinit_marker_reader(j_decompress_ptr cinfo)
*/
