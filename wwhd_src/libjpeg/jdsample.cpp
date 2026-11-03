/*
 * jdsample.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modified 2002-2008 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains upsampling routines.
 *
 * Upsampling input data is counted in "row groups".  A row group
 * is defined to be (v_samp_factor * DCT_v_scaled_size / min_DCT_v_scaled_size)
 * sample rows of each component.  Upsampling will normally produce
 * max_v_samp_factor pixel rows from each row group (but this could vary
 * if the upsampler is applying a scale factor of its own).
 *
 * An excellent reference for image resampling is
 *   Digital Image Warping, George Wolberg, 1990.
 *   Pub. by IEEE Computer Society Press, Los Alamitos, CA. ISBN 0-8186-8944-7.
 */
// Decompiled WWHD build: guest controller fields and method tables.
#include "libjpeg/jdsample.h"
/* 02857A9C */
static void start_pass_upsample (j_decompress_ptr cinfo)
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample.get();

  /* Mark the conversion buffer empty */
  upsample->next_row_out = cinfo->max_v_samp_factor;
  /* Initialize total-height counter for detecting bottom of image */
  upsample->rows_to_go = cinfo->output_height;
}

/* 02857AB4 */
static void sep_upsample (j_decompress_ptr cinfo,
	      JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
	      JDIMENSION in_row_groups_avail,
	      JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
	      JDIMENSION out_rows_avail)
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample.get();
  int ci;
  jpeg_component_info * compptr;
  JDIMENSION num_rows;

  /* Fill the conversion buffer, if it's empty */
  if (upsample->next_row_out >= cinfo->max_v_samp_factor) {
    for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	 ci++, compptr++) {
      /* Invoke per-component upsample method.  Notice we pass a POINTER
       * to color_buf[ci], so that fullsize_upsample can change it.
       */
      (*upsample->methods[ci]) (cinfo, compptr,
	input_buf[ci] + (*in_row_group_ctr * upsample->rowgroup_height[ci]),
	upsample->color_buf + ci);
    }
    upsample->next_row_out = 0;
  }

  /* Color-convert and emit rows */

  /* How many we have in the buffer: */
  num_rows = (JDIMENSION) (cinfo->max_v_samp_factor - upsample->next_row_out);
  /* Not more than the distance to the end of the image.  Need this test
   * in case the image height is not a multiple of max_v_samp_factor:
   */
  if (num_rows > upsample->rows_to_go) 
    num_rows = upsample->rows_to_go;
  /* And not more than what the client can accept: */
  out_rows_avail -= *out_row_ctr;
  if (num_rows > out_rows_avail)
    num_rows = out_rows_avail;

  (*cinfo->cconvert->color_convert) (cinfo, upsample->color_buf,
				     (JDIMENSION) upsample->next_row_out,
				     output_buf + *out_row_ctr,
				     (int) num_rows);

  /* Adjust counts */
  *out_row_ctr += num_rows;
  upsample->rows_to_go -= num_rows;
  upsample->next_row_out += num_rows;
  /* When the buffer is emptied, declare this input row group consumed */
  if (upsample->next_row_out >= cinfo->max_v_samp_factor)
    (*in_row_group_ctr)++;
}

/* 02857BFC */
static void fullsize_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		   JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr)
{
  *output_data_ptr = input_data;
}

/* 02857C04 */
static void noop_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr)
{
  *output_data_ptr = NULL;	/* safety check */
}

/* 02857C10 */
static void int_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	      JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr)
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample.get();
  JSAMPARRAY output_data = *output_data_ptr;
  JSAMPROW inptr, outptr;
  JSAMPLE invalue;
  int h;
  JSAMPROW outend;
  int h_expand, v_expand;
  int inrow, outrow;

  h_expand = upsample->h_expand[compptr->component_index];
  v_expand = upsample->v_expand[compptr->component_index];

  inrow = outrow = 0;
  while (outrow < cinfo->max_v_samp_factor) {
    /* Generate one output row with proper horizontal expansion */
    inptr = input_data[inrow];
    outptr = output_data[outrow];
    outend = outptr + cinfo->output_width;
    while (outptr < outend) {
      invalue = *inptr++;	/* don't need GETJSAMPLE() here */
      for (h = h_expand; h > 0; h--) {
	*outptr++ = invalue;
      }
    }
    /* Generate any additional output rows by duplicating the first one */
    if (v_expand > 1) {
      copy_upsample_rows(output_data, outrow, output_data, outrow+1,
			v_expand-1, cinfo->output_width);
    }
    inrow++;
    outrow += v_expand;
  }
}

/* 02857CE8 */
static void h2v1_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr)
{
  JSAMPARRAY output_data = *output_data_ptr;
  JSAMPROW inptr, outptr;
  JSAMPLE invalue;
  JSAMPROW outend;
  int outrow;

  for (outrow = 0; outrow < cinfo->max_v_samp_factor; outrow++) {
    inptr = input_data[outrow];
    outptr = output_data[outrow];
    outend = outptr + cinfo->output_width;
    while (outptr < outend) {
      invalue = *inptr++;	/* don't need GETJSAMPLE() here */
      *outptr++ = invalue;
      *outptr++ = invalue;
    }
  }
}

/* 02857D48 */
static void h2v2_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr)
{
  JSAMPARRAY output_data = *output_data_ptr;
  JSAMPROW inptr, outptr;
  JSAMPLE invalue;
  JSAMPROW outend;
  int inrow, outrow;

  inrow = outrow = 0;
  while (outrow < cinfo->max_v_samp_factor) {
    inptr = input_data[inrow];
    outptr = output_data[outrow];
    outend = outptr + cinfo->output_width;
    while (outptr < outend) {
      invalue = *inptr++;	/* don't need GETJSAMPLE() here */
      *outptr++ = invalue;
      *outptr++ = invalue;
    }
    copy_upsample_rows(output_data, outrow, output_data, outrow+1,
		      1, cinfo->output_width);
    inrow++;
    outrow += 2;
  }
}

/* 02857DF8 */
void jinit_upsampler (j_decompress_ptr cinfo)
{
  my_upsample_ptr upsample;
  int ci;
  jpeg_component_info * compptr;
  boolean need_buffer;
  int h_in_group, v_in_group, h_out_group, v_out_group;

  upsample = (my_upsample_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(my_upsampler));
  cinfo->upsample = (struct jpeg_upsampler *) upsample;
  upsample->pub.start_pass = start_pass_upsample;
  upsample->pub.upsample = sep_upsample;
  upsample->pub.need_context_rows = FALSE; /* until we find out differently */

  if (cinfo->CCIR601_sampling)	/* this isn't supported */
    ERREXIT(cinfo, JERR_CCIR601_NOTIMPL);

  /* Verify we can handle the sampling factors, select per-component methods,
   * and create storage as needed.
   */
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    /* Compute size of an "input group" after IDCT scaling.  This many samples
     * are to be converted to max_h_samp_factor * max_v_samp_factor pixels.
     */
    h_in_group = (compptr->h_samp_factor * compptr->DCT_h_scaled_size) /
		 cinfo->min_DCT_h_scaled_size;
    v_in_group = (compptr->v_samp_factor * compptr->DCT_v_scaled_size) /
		 cinfo->min_DCT_v_scaled_size;
    h_out_group = cinfo->max_h_samp_factor;
    v_out_group = cinfo->max_v_samp_factor;
    upsample->rowgroup_height[ci] = v_in_group; /* save for use later */
    need_buffer = TRUE;
    if (! compptr->component_needed) {
      /* Don't bother to upsample an uninteresting component. */
      upsample->methods[ci] = noop_upsample;
      need_buffer = FALSE;
    } else if (h_in_group == h_out_group && v_in_group == v_out_group) {
      /* Fullsize components can be processed without any work. */
      upsample->methods[ci] = fullsize_upsample;
      need_buffer = FALSE;
    } else if (h_in_group * 2 == h_out_group &&
	       v_in_group == v_out_group) {
      /* Special case for 2h1v upsampling */
      upsample->methods[ci] = h2v1_upsample;
    } else if (h_in_group * 2 == h_out_group &&
	       v_in_group * 2 == v_out_group) {
      /* Special case for 2h2v upsampling */
      upsample->methods[ci] = h2v2_upsample;
    } else if ((h_out_group % h_in_group) == 0 &&
	       (v_out_group % v_in_group) == 0) {
      /* Generic integral-factors upsampling method */
      upsample->methods[ci] = int_upsample;
      upsample->h_expand[ci] = (UINT8) (h_out_group / h_in_group);
      upsample->v_expand[ci] = (UINT8) (v_out_group / v_in_group);
    } else
      ERREXIT(cinfo, JERR_FRACT_SAMPLE_NOTIMPL);
    if (need_buffer) {
      upsample->color_buf[ci] = (*cinfo->mem->alloc_sarray)
	((j_common_ptr) cinfo, JPOOL_IMAGE,
	 (JDIMENSION) round_upsample_width((s32) cinfo->output_width,
				(s32) cinfo->max_h_samp_factor),
	 (JDIMENSION) cinfo->max_v_samp_factor);
    }
  }
}
/* WWHD glue
ext 0284DFDC s32 round_upsample_width(s32 a,s32 b)
ext 0284E048 void copy_upsample_rows(JSAMPARRAY input_array,s32 source_row,JSAMPARRAY output_array,s32 dest_row,s32 num_rows,JDIMENSION num_cols)
*/
