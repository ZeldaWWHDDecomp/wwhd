/* jcsample.c (decompiled WWHD build)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c downsampling to guest storage and 32-bit Cafe arithmetic.
 */
#include "libjpeg/jcsample.h"
#include "libjpeg/jutils.h"
/* 02851680 */
static void start_pass_downsample(j_compress_ptr cinfo) {}
/* 02851684 */
static void expand_right_edge(JSAMPARRAY image_data, s32 num_rows, u32 input_cols,
                              u32 output_cols) {
    s32 numcols = output_cols - input_cols;
    if (numcols > 0)
        for (s32 row = 0; row < num_rows; ++row) {
            be<u8> *ptr = image_data[row].get() + input_cols;
            u32 pixval = ptr[-1];
            for (s32 count = numcols; count > 0; --count)
                *ptr++ = pixval;
        }
}
/* 028516C8 */
static void sep_downsample(j_compress_ptr cinfo, JSAMPIMAGE input_buf, u32 in_row_index,
                           JSAMPIMAGE output_buf, u32 out_row_group_index) {
    auto *downsample = static_cast<my_downsampler *>(cinfo->downsample.get());
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        auto *in_ptr = input_buf[ci].get() + in_row_index;
        auto *out_ptr =
            output_buf[ci].get() + out_row_group_index * (u32)downsample->rowgroup_height[ci];
        downsample->methods[ci](cinfo, comp, in_ptr, out_ptr);
    }
}
/* 02851768 */
static void int_downsample(j_compress_ptr cinfo, jpeg_component_info *comp, JSAMPARRAY input_data,
                           JSAMPARRAY output_data) {
    auto *downsample = static_cast<my_downsampler *>(cinfo->downsample.get());
    u32 output_cols = (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size;
    s32 h_expand = downsample->h_expand[comp->component_index];
    s32 v_expand = downsample->v_expand[comp->component_index];
    s32 numpix = h_expand * v_expand;
    s32 numpix2 = numpix / 2;
    expand_right_edge(input_data, cinfo->max_v_samp_factor, cinfo->image_width,
                      output_cols * (u32)h_expand);
    s32 outrow = 0;
    for (s32 inrow = 0; inrow < cinfo->max_v_samp_factor;
         inrow = (u32)inrow + (u32)v_expand, ++outrow) {
        be<u8> *outptr = output_data[outrow];
        u32 outcol_h = 0;
        for (u32 outcol = 0; outcol < output_cols; ++outcol, outcol_h += (u32)h_expand) {
            u32 outvalue = 0;
            for (s32 v = 0; v < v_expand; ++v) {
                be<u8> *inptr = input_data[(u32)inrow + (u32)v].get() + outcol_h;
                for (s32 h = 0; h < h_expand; ++h)
                    outvalue += (u32)*inptr++;
            }
            *outptr++ = ppc_divw(outvalue + (u32)numpix2, numpix);
        }
    }
}
/* 02851884 */
static void fullsize_downsample(j_compress_ptr cinfo, jpeg_component_info *comp,
                                JSAMPARRAY input_data, JSAMPARRAY output_data) {
    jcopy_sample_rows(input_data, 0, output_data, 0, cinfo->max_v_samp_factor, cinfo->image_width);
    expand_right_edge(output_data, cinfo->max_v_samp_factor, cinfo->image_width,
                      (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size);
}
/* 028518FC */
static void h2v1_downsample(j_compress_ptr cinfo, jpeg_component_info *comp, JSAMPARRAY input_data,
                            JSAMPARRAY output_data) {
    u32 output_cols = (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size;
    expand_right_edge(input_data, cinfo->max_v_samp_factor, cinfo->image_width, output_cols * 2);
    for (s32 inrow = 0; inrow < cinfo->max_v_samp_factor; ++inrow) {
        be<u8> *outptr = output_data[inrow];
        be<u8> *inptr = input_data[inrow];
        u32 bias = 0;
        for (u32 outcol = 0; outcol < output_cols; ++outcol) {
            *outptr++ = ((u32)inptr[0] + (u32)inptr[1] + bias) >> 1;
            bias ^= 1;
            inptr += 2;
        }
    }
}
/* 028519BC */
static void h2v2_downsample(j_compress_ptr cinfo, jpeg_component_info *comp, JSAMPARRAY input_data,
                            JSAMPARRAY output_data) {
    u32 output_cols = (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size;
    expand_right_edge(input_data, cinfo->max_v_samp_factor, cinfo->image_width, output_cols * 2);
    for (s32 inrow = 0, outrow = 0; inrow < cinfo->max_v_samp_factor; inrow += 2, ++outrow) {
        be<u8> *outptr = output_data[outrow];
        be<u8> *inptr0 = input_data[inrow];
        be<u8> *inptr1 = input_data[inrow + 1];
        u32 bias = 1;
        for (u32 outcol = 0; outcol < output_cols; ++outcol) {
            *outptr++ =
                ((u32)inptr0[0] + (u32)inptr0[1] + (u32)inptr1[0] + (u32)inptr1[1] + bias) >> 2;
            bias ^= 3;
            inptr0 += 2;
            inptr1 += 2;
        }
    }
}
/* 02851A9C */
static void h2v2_smooth_downsample(j_compress_ptr cinfo, jpeg_component_info *comp,
                                   JSAMPARRAY input_data, JSAMPARRAY output_data) {
    u32 output_cols = (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size;
    expand_right_edge(input_data - 1, (u32)cinfo->max_v_samp_factor + 2, cinfo->image_width,
                      output_cols * 2);
    u32 memberscale = 16384 - (u32)cinfo->smoothing_factor * 80;
    u32 neighscale = (u32)cinfo->smoothing_factor * 16;
    for (s32 inrow = 0, outrow = 0; inrow < cinfo->max_v_samp_factor; inrow += 2, ++outrow) {
        be<u8> *outptr = output_data[outrow];
        be<u8> *inptr0 = input_data[inrow];
        be<u8> *inptr1 = input_data[inrow + 1];
        be<u8> *above = input_data[inrow - 1];
        be<u8> *below = input_data[inrow + 2];
        for (u32 col = 0; col < output_cols; ++col) {
            s32 left = col == 0 ? 0 : -1;
            s32 right = col + 1 == output_cols ? 1 : 2;
            u32 membersum = (u32)inptr0[0] + (u32)inptr0[1] + (u32)inptr1[0] + (u32)inptr1[1];
            u32 neighsum = (u32)above[0] + (u32)above[1] + (u32)below[0] + (u32)below[1] +
                           (u32)inptr0[left] + (u32)inptr0[right] + (u32)inptr1[left] +
                           (u32)inptr1[right];
            neighsum = neighsum * 2 + (u32)above[left] + (u32)above[right] + (u32)below[left] +
                       (u32)below[right];
            *outptr++ = (membersum * memberscale + neighsum * neighscale + 32768) >> 16;
            inptr0 += 2;
            inptr1 += 2;
            above += 2;
            below += 2;
        }
    }
}
/* 02851CFC */
static void fullsize_smooth_downsample(j_compress_ptr cinfo, jpeg_component_info *comp,
                                       JSAMPARRAY input_data, JSAMPARRAY output_data) {
    u32 output_cols = (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size;
    expand_right_edge(input_data - 1, (u32)cinfo->max_v_samp_factor + 2, cinfo->image_width,
                      output_cols);
    u32 memberscale = 65536 - (u32)cinfo->smoothing_factor * 512;
    u32 neighscale = (u32)cinfo->smoothing_factor * 64;
    for (s32 inrow = 0; inrow < cinfo->max_v_samp_factor; ++inrow) {
        be<u8> *outptr = output_data[inrow];
        be<u8> *inptr = input_data[inrow];
        be<u8> *above = input_data[inrow - 1];
        be<u8> *below = input_data[inrow + 1];
        u32 colsum = (u32)*above++ + (u32)*below++ + (u32)*inptr;
        u32 membersum = *inptr++;
        u32 nextcolsum = (u32)*above + (u32)*below + (u32)*inptr;
        u32 neighsum = colsum + (colsum - membersum) + nextcolsum;
        *outptr++ = (membersum * memberscale + neighsum * neighscale + 32768) >> 16;
        u32 lastcolsum = colsum;
        colsum = nextcolsum;
        for (u32 col = output_cols - 2; col > 0; --col) {
            membersum = *inptr++;
            ++above;
            ++below;
            nextcolsum = (u32)*above + (u32)*below + (u32)*inptr;
            neighsum = lastcolsum + (colsum - membersum) + nextcolsum;
            *outptr++ = (membersum * memberscale + neighsum * neighscale + 32768) >> 16;
            lastcolsum = colsum;
            colsum = nextcolsum;
        }
        membersum = *inptr;
        neighsum = lastcolsum + (colsum - membersum) + colsum;
        *outptr = (membersum * memberscale + neighsum * neighscale + 32768) >> 16;
    }
}
/* 02851E6C */
void jinit_downsampler(j_compress_ptr cinfo) {
    auto *downsample = static_cast<my_downsampler *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(my_downsampler)));
    cinfo->downsample = downsample;
    downsample->need_context_rows = 0;
    boolean ccir_sampling = cinfo->CCIR601_sampling;
    downsample->downsample = sep_downsample;
    downsample->start_pass = start_pass_downsample;
    boolean smoothok = 1;
    if (ccir_sampling)
        jpeg_error(cinfo, JERR_CCIR601_NOTIMPL);
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        s32 h_out_group = ppc_divw((u32)comp->h_samp_factor * (u32)comp->DCT_h_scaled_size,
                                   cinfo->min_DCT_h_scaled_size);
        s32 v_out_group = ppc_divw((u32)comp->v_samp_factor * (u32)comp->DCT_v_scaled_size,
                                   cinfo->min_DCT_v_scaled_size);
        s32 h_in_group = cinfo->max_h_samp_factor, v_in_group = cinfo->max_v_samp_factor;
        downsample->rowgroup_height[ci] = v_out_group;
        if (h_in_group == h_out_group && v_in_group == v_out_group) {
            if (cinfo->smoothing_factor) {
                downsample->methods[ci] = fullsize_smooth_downsample;
                downsample->need_context_rows = 1;
            } else
                downsample->methods[ci] = fullsize_downsample;
        } else if (h_in_group == (s32)((u32)h_out_group * 2) && v_in_group == v_out_group) {
            smoothok = 0;
            downsample->methods[ci] = h2v1_downsample;
        } else if (h_in_group == (s32)((u32)h_out_group * 2) &&
                   v_in_group == (s32)((u32)v_out_group * 2)) {
            if (cinfo->smoothing_factor) {
                downsample->methods[ci] = h2v2_smooth_downsample;
                downsample->need_context_rows = 1;
            } else
                downsample->methods[ci] = h2v2_downsample;
        } else {
            s32 h_expand = ppc_divw(h_in_group, h_out_group),
                v_expand = ppc_divw(v_in_group, v_out_group);
            if ((u32)h_in_group - (u32)h_expand * (u32)h_out_group == 0 &&
                (u32)v_in_group - (u32)v_expand * (u32)v_out_group == 0) {
                smoothok = 0;
                downsample->methods[ci] = int_downsample;
                downsample->h_expand[ci] = h_expand;
                downsample->v_expand[ci] = v_expand;
            } else
                jpeg_error(cinfo, JERR_FRACT_SAMPLE_NOTIMPL);
        }
    }
    if (cinfo->smoothing_factor && !smoothok) {
        jpeg_error_mgr *err = cinfo->err;
        auto emit = err->emit_message.load();
        err->msg_code = JTRC_SMOOTH_NOTIMPL;
        emit(cinfo, 0);
    }
}
/* WWHD glue
 */
