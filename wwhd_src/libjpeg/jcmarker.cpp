/* jcmarker.c (decompiled WWHD build)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2003-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c marker serialization to guest storage and callbacks.
 */
#include "libjpeg/jcmarker.h"
/* 0284E990 */
static void emit_byte(j_compress_ptr cinfo, s32 val) {
    jpeg_destination_mgr *dest = cinfo->dest;
    be<u8> *next = dest->next_output_byte;
    dest->next_output_byte = next + 1;
    *next = val;
    u32 free_in_buffer = (u32)dest->free_in_buffer - 1;
    dest->free_in_buffer = free_in_buffer;
    if (free_in_buffer == 0 && !dest->empty_output_buffer(cinfo))
        jpeg_error(cinfo, JERR_CANT_SUSPEND);
}
/* 0284EA10 */
static void emit_marker(j_compress_ptr cinfo, s32 mark) {
    emit_byte(cinfo, 0xFF);
    emit_byte(cinfo, mark);
}
/* 0284EA58 */
static void emit_2bytes(j_compress_ptr cinfo, s32 value) {
    emit_byte(cinfo, ((u32)value >> 8) & 0xFF);
    emit_byte(cinfo, value & 0xFF);
}
/* 0284EAA0 */
static s32 emit_dqt(j_compress_ptr cinfo, s32 index) {
    JQUANT_TBL *qtbl = cinfo->quant_tbl_ptrs[index];
    if (qtbl == nullptr)
        jpeg_error1(cinfo, JERR_NO_QUANT_TABLE, index);
    s32 prec = 0;
    for (s32 i = 0; i <= cinfo->lim_Se; ++i)
        if (qtbl->quantval[cinfo->natural_order[i]] > 255)
            prec = 1;
    if (!qtbl->sent_table) {
        emit_marker(cinfo, M_DQT);
        emit_2bytes(cinfo, prec ? (u32)cinfo->lim_Se * 2 + 5 : (u32)cinfo->lim_Se + 4);
        emit_byte(cinfo, (u32)index + ((u32)prec << 4));
        for (s32 i = 0; i <= cinfo->lim_Se; ++i) {
            u32 qval = qtbl->quantval[cinfo->natural_order[i]];
            if (prec)
                emit_byte(cinfo, qval >> 8);
            emit_byte(cinfo, qval & 0xFF);
        }
        qtbl->sent_table = 1;
    }
    return prec;
}
/* 0284EC18 */
static void emit_dht(j_compress_ptr cinfo, s32 index, boolean is_ac) {
    JHUFF_TBL *htbl;
    if (is_ac) {
        htbl = cinfo->ac_huff_tbl_ptrs[index];
        index = (u32)index + 0x10;
    } else
        htbl = cinfo->dc_huff_tbl_ptrs[index];
    if (htbl == nullptr)
        jpeg_error1(cinfo, JERR_NO_HUFF_TABLE, index);
    if (!htbl->sent_table) {
        emit_marker(cinfo, M_DHT);
        s32 length = 0;
        for (s32 i = 1; i <= 16; ++i)
            length += (u32)htbl->bits[i];
        emit_2bytes(cinfo, length + 19);
        emit_byte(cinfo, index);
        for (s32 i = 1; i <= 16; ++i)
            emit_byte(cinfo, htbl->bits[i]);
        for (s32 i = 0; i < length; ++i)
            emit_byte(cinfo, htbl->huffval[i]);
        htbl->sent_table = 1;
    }
}
/* 0284ED2C */
static void emit_dac(j_compress_ptr cinfo) {
    gabi::Local<be<u8>[NUM_ARITH_TBLS]> dc_buffer;
    gabi::Local<be<u8>[NUM_ARITH_TBLS]> ac_buffer;
    be<u8> *dc_in_use = *dc_buffer.get();
    be<u8> *ac_in_use = *ac_buffer.get();
    for (s32 i = 0; i < NUM_ARITH_TBLS; ++i)
        dc_in_use[i] = ac_in_use[i] = 0;
    for (s32 i = 0; i < cinfo->comps_in_scan; ++i) {
        jpeg_component_info *comp = cinfo->cur_comp_info[i];
        if (cinfo->Ss == 0 && cinfo->Ah == 0)
            dc_in_use[comp->dc_tbl_no] = 1;
        if (cinfo->Se)
            ac_in_use[comp->ac_tbl_no] = 1;
    }
    s32 length = 0;
    for (s32 i = 0; i < NUM_ARITH_TBLS; ++i)
        length += dc_in_use[i] + ac_in_use[i];
    if (length) {
        emit_marker(cinfo, M_DAC);
        emit_2bytes(cinfo, length * 2 + 2);
        for (s32 i = 0; i < NUM_ARITH_TBLS; ++i) {
            if (dc_in_use[i]) {
                emit_byte(cinfo, i);
                emit_byte(cinfo, (u32)cinfo->arith_dc_L[i] + ((u32)cinfo->arith_dc_U[i] << 4));
            }
            if (ac_in_use[i]) {
                emit_byte(cinfo, i + 0x10);
                emit_byte(cinfo, cinfo->arith_ac_K[i]);
            }
        }
    }
}
/* 0284EE94 */
static void emit_dri(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_DRI);
    emit_2bytes(cinfo, 4);
    emit_2bytes(cinfo, cinfo->restart_interval);
}
/* 0284EEDC */
static void emit_sof(j_compress_ptr cinfo, s32 code) {
    emit_marker(cinfo, code);
    emit_2bytes(cinfo, (u32)cinfo->num_components * 3 + 8);
    if ((s32)cinfo->jpeg_height > 65535 || (s32)cinfo->jpeg_width > 65535)
        jpeg_error1(cinfo, JERR_IMAGE_TOO_BIG, 65535);
    emit_byte(cinfo, cinfo->data_precision);
    emit_2bytes(cinfo, cinfo->jpeg_height);
    emit_2bytes(cinfo, cinfo->jpeg_width);
    emit_byte(cinfo, cinfo->num_components);
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        emit_byte(cinfo, comp->component_id);
        emit_byte(cinfo, ((u32)comp->h_samp_factor << 4) + (u32)comp->v_samp_factor);
        emit_byte(cinfo, comp->quant_tbl_no);
    }
}
/* 0284EFF4 */
static void emit_sos(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_SOS);
    emit_2bytes(cinfo, (u32)cinfo->comps_in_scan * 2 + 6);
    emit_byte(cinfo, cinfo->comps_in_scan);
    for (s32 i = 0; i < cinfo->comps_in_scan; ++i) {
        jpeg_component_info *comp = cinfo->cur_comp_info[i];
        emit_byte(cinfo, comp->component_id);
        s32 td = cinfo->Ss == 0 && cinfo->Ah == 0 ? (s32)comp->dc_tbl_no : 0;
        s32 ta = cinfo->Se ? (s32)comp->ac_tbl_no : 0;
        emit_byte(cinfo, ((u32)td << 4) + (u32)ta);
    }
    emit_byte(cinfo, cinfo->Ss);
    emit_byte(cinfo, cinfo->Se);
    emit_byte(cinfo, ((u32)cinfo->Ah << 4) + (u32)cinfo->Al);
}
/* 0284F10C */
static void emit_pseudo_sos(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_SOS);
    emit_2bytes(cinfo, 6);
    emit_byte(cinfo, 0);
    emit_byte(cinfo, 0);
    emit_byte(cinfo, (u32)cinfo->block_size * (u32)cinfo->block_size - 1);
    emit_byte(cinfo, 0);
}
/* 0284F180 */
static void emit_jfif_app0(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_APP0);
    emit_2bytes(cinfo, 16);
    emit_byte(cinfo, 0x4A);
    emit_byte(cinfo, 0x46);
    emit_byte(cinfo, 0x49);
    emit_byte(cinfo, 0x46);
    emit_byte(cinfo, 0);
    emit_byte(cinfo, cinfo->JFIF_major_version);
    emit_byte(cinfo, cinfo->JFIF_minor_version);
    emit_byte(cinfo, cinfo->density_unit);
    emit_2bytes(cinfo, cinfo->X_density);
    emit_2bytes(cinfo, cinfo->Y_density);
    emit_byte(cinfo, 0);
    emit_byte(cinfo, 0);
}
/* 0284F24C */
static void emit_adobe_app14(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_APP14);
    emit_2bytes(cinfo, 14);
    emit_byte(cinfo, 0x41);
    emit_byte(cinfo, 0x64);
    emit_byte(cinfo, 0x6F);
    emit_byte(cinfo, 0x62);
    emit_byte(cinfo, 0x65);
    emit_2bytes(cinfo, 100);
    emit_2bytes(cinfo, 0);
    emit_2bytes(cinfo, 0);
    s32 colorspace = cinfo->jpeg_color_space;
    emit_byte(cinfo, colorspace == JCS_YCbCr ? 1 : colorspace == JCS_YCCK ? 2 : 0);
}
/* 0284F320 */
static void write_marker_header(j_compress_ptr cinfo, s32 marker, u32 datalen) {
    if (datalen > 65533)
        jpeg_error(cinfo, JERR_BAD_LENGTH);
    emit_marker(cinfo, marker);
    emit_2bytes(cinfo, datalen + 2);
}
/* 0284F39C */
static void write_marker_byte(j_compress_ptr cinfo, s32 val) { emit_byte(cinfo, val); }
/* 0284F3A0 */
static void write_file_header(j_compress_ptr cinfo) {
    auto *marker = static_cast<my_marker_writer *>(cinfo->marker.get());
    emit_marker(cinfo, M_SOI);
    marker->last_restart_interval = 0;
    if (cinfo->write_JFIF_header)
        emit_jfif_app0(cinfo);
    if (cinfo->write_Adobe_marker)
        emit_adobe_app14(cinfo);
}
/* 0284F40C */
static void write_frame_header(j_compress_ptr cinfo) {
    s32 prec = 0;
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp)
        prec = (u32)prec + (u32)emit_dqt(cinfo, comp->quant_tbl_no);
    boolean is_baseline = 0;
    if (!cinfo->arith_code && !cinfo->progressive_mode && cinfo->data_precision == 8 &&
        cinfo->block_size == DCTSIZE) {
        is_baseline = 1;
        comp = cinfo->comp_info;
        for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp)
            if (comp->dc_tbl_no > 1 || comp->ac_tbl_no > 1)
                is_baseline = 0;
        if (prec && is_baseline) {
            is_baseline = 0;
            jpeg_error_mgr *err = cinfo->err;
            auto emit = err->emit_message.load();
            err->msg_code = JTRC_16BIT_TABLES;
            emit(cinfo, 0);
        }
    }
    if (cinfo->arith_code)
        emit_sof(cinfo, cinfo->progressive_mode ? M_SOF10 : M_SOF9);
    else
        emit_sof(cinfo, cinfo->progressive_mode ? M_SOF2 : is_baseline ? M_SOF0 : M_SOF1);
    if (cinfo->progressive_mode && cinfo->block_size != DCTSIZE)
        emit_pseudo_sos(cinfo);
}
/* 0284F5C0 */
static void write_scan_header(j_compress_ptr cinfo) {
    auto *marker = static_cast<my_marker_writer *>(cinfo->marker.get());
    if (cinfo->arith_code)
        emit_dac(cinfo);
    else
        for (s32 i = 0; i < cinfo->comps_in_scan; ++i) {
            jpeg_component_info *comp = cinfo->cur_comp_info[i];
            if (cinfo->Ss == 0 && cinfo->Ah == 0)
                emit_dht(cinfo, comp->dc_tbl_no, 0);
            if (cinfo->Se)
                emit_dht(cinfo, comp->ac_tbl_no, 1);
        }
    if (cinfo->restart_interval != marker->last_restart_interval) {
        emit_dri(cinfo);
        marker->last_restart_interval = cinfo->restart_interval;
    }
    emit_sos(cinfo);
}
/* 0284F6C0 */
static void write_file_trailer(j_compress_ptr cinfo) { emit_marker(cinfo, M_EOI); }
/* 0284F6C8 */
static void write_tables_only(j_compress_ptr cinfo) {
    emit_marker(cinfo, M_SOI);
    for (s32 i = 0; i < NUM_QUANT_TBLS; ++i)
        if (cinfo->quant_tbl_ptrs[i] != nullptr)
            emit_dqt(cinfo, i);
    if (!cinfo->arith_code)
        for (s32 i = 0; i < NUM_HUFF_TBLS; ++i) {
            if (cinfo->dc_huff_tbl_ptrs[i] != nullptr)
                emit_dht(cinfo, i, 0);
            if (cinfo->ac_huff_tbl_ptrs[i] != nullptr)
                emit_dht(cinfo, i, 1);
        }
    emit_marker(cinfo, M_EOI);
}
/* 0284F7A0 */
void jinit_marker_writer(j_compress_ptr cinfo) {
    auto *marker = static_cast<my_marker_writer *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(my_marker_writer)));
    cinfo->marker = marker;
    marker->write_file_header = write_file_header;
    marker->write_frame_header = write_frame_header;
    marker->write_scan_header = write_scan_header;
    marker->write_file_trailer = write_file_trailer;
    marker->write_tables_only = write_tables_only;
    marker->write_marker_header = write_marker_header;
    marker->write_marker_byte = write_marker_byte;
    marker->last_restart_interval = 0;
}
/* WWHD glue
 */
