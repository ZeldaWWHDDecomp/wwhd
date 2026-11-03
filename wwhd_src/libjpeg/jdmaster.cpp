/*
 * jdmaster.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains master control logic for the JPEG decompressor.
 * These routines are concerned with selecting the modules to be executed
 * and with determining the number of passes and the work to be done in each
 * pass.
 */
// Decompiled WWHD build: guest decompressor state and module dispatch.
#include "libjpeg/jdmaster.h"
/* 028563D0 */
static boolean use_merged_upsample (j_decompress_ptr cinfo)
{
#ifdef UPSAMPLE_MERGING_SUPPORTED
  /* Merging is the equivalent of plain box-filter upsampling */
  if (cinfo->do_fancy_upsampling || cinfo->CCIR601_sampling)
    return FALSE;
  /* jdmerge.c only supports YCC=>RGB color conversion */
  if (cinfo->jpeg_color_space != JCS_YCbCr || cinfo->num_components != 3 ||
      cinfo->out_color_space != JCS_RGB ||
      cinfo->out_color_components != RGB_PIXELSIZE)
    return FALSE;
  /* and it only handles 2h1v or 2h2v sampling ratios */
  if (cinfo->comp_info[0].h_samp_factor != 2 ||
      cinfo->comp_info[1].h_samp_factor != 1 ||
      cinfo->comp_info[2].h_samp_factor != 1 ||
      cinfo->comp_info[0].v_samp_factor >  2 ||
      cinfo->comp_info[1].v_samp_factor != 1 ||
      cinfo->comp_info[2].v_samp_factor != 1)
    return FALSE;
  /* furthermore, it doesn't work if we've scaled the IDCTs differently */
  if (cinfo->comp_info[0].DCT_h_scaled_size != cinfo->min_DCT_h_scaled_size ||
      cinfo->comp_info[1].DCT_h_scaled_size != cinfo->min_DCT_h_scaled_size ||
      cinfo->comp_info[2].DCT_h_scaled_size != cinfo->min_DCT_h_scaled_size ||
      cinfo->comp_info[0].DCT_v_scaled_size != cinfo->min_DCT_v_scaled_size ||
      cinfo->comp_info[1].DCT_v_scaled_size != cinfo->min_DCT_v_scaled_size ||
      cinfo->comp_info[2].DCT_v_scaled_size != cinfo->min_DCT_v_scaled_size)
    return FALSE;
  /* ??? also need to test for upsample-time rescaling, when & if supported */
  return TRUE;			/* by golly, it'll work... */
#else
  return FALSE;
#endif
}

/* 028564C4 */
void jpeg_calc_output_dimensions (j_decompress_ptr cinfo)
/* Do computations that are needed before master selection phase.
 * This function is used for full decompression.
 */
{
#ifdef IDCT_SCALING_SUPPORTED
  int ci;
  jpeg_component_info *compptr;
#endif

  /* Prevent application from calling me at wrong times */
  if (cinfo->global_state != DSTATE_READY)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

  /* Compute core output image dimensions and DCT scaling choices. */
  jpeg_core_output_dimensions(cinfo);

#ifdef IDCT_SCALING_SUPPORTED

  /* In selecting the actual DCT scaling for each component, we try to
   * scale up the chroma components via IDCT scaling rather than upsampling.
   * This saves time if the upsampler gets to use 1:1 scaling.
   * Note this code adapts subsampling ratios which are powers of 2.
   */
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    int ssize = 1;
    while (cinfo->min_DCT_h_scaled_size * ssize <=
	   (cinfo->do_fancy_upsampling ? DCTSIZE : DCTSIZE / 2) &&
	   (cinfo->max_h_samp_factor % (compptr->h_samp_factor * ssize * 2)) == 0) {
      ssize = ssize * 2;
    }
    compptr->DCT_h_scaled_size = cinfo->min_DCT_h_scaled_size * ssize;
    ssize = 1;
    while (cinfo->min_DCT_v_scaled_size * ssize <=
	   (cinfo->do_fancy_upsampling ? DCTSIZE : DCTSIZE / 2) &&
	   (cinfo->max_v_samp_factor % (compptr->v_samp_factor * ssize * 2)) == 0) {
      ssize = ssize * 2;
    }
    compptr->DCT_v_scaled_size = cinfo->min_DCT_v_scaled_size * ssize;

    /* We don't support IDCT ratios larger than 2. */
    if (compptr->DCT_h_scaled_size > compptr->DCT_v_scaled_size * 2)
	compptr->DCT_h_scaled_size = compptr->DCT_v_scaled_size * 2;
    else if (compptr->DCT_v_scaled_size > compptr->DCT_h_scaled_size * 2)
	compptr->DCT_v_scaled_size = compptr->DCT_h_scaled_size * 2;
  }

  /* Recompute downsampled dimensions of components;
   * application needs to know these if using raw downsampled data.
   */
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    /* Size in samples, after IDCT scaling */
    compptr->downsampled_width = (JDIMENSION)
      jdiv_round_up((s32) cinfo->image_width *
		    (s32) (compptr->h_samp_factor * compptr->DCT_h_scaled_size),
		    (s32) (cinfo->max_h_samp_factor * cinfo->block_size));
    compptr->downsampled_height = (JDIMENSION)
      jdiv_round_up((s32) cinfo->image_height *
		    (s32) (compptr->v_samp_factor * compptr->DCT_v_scaled_size),
		    (s32) (cinfo->max_v_samp_factor * cinfo->block_size));
  }

#endif /* IDCT_SCALING_SUPPORTED */

  /* Report number of components in selected colorspace. */
  /* Probably this should be in the color conversion module... */
  switch (cinfo->out_color_space) {
  case JCS_GRAYSCALE:
    cinfo->out_color_components = 1;
    break;
  case JCS_RGB:
#if RGB_PIXELSIZE != 3
    cinfo->out_color_components = RGB_PIXELSIZE;
    break;
#endif /* else share code with YCbCr */
  case JCS_YCbCr:
    cinfo->out_color_components = 3;
    break;
  case JCS_CMYK:
  case JCS_YCCK:
    cinfo->out_color_components = 4;
    break;
  default:			/* else must be same colorspace as in file */
    cinfo->out_color_components = cinfo->num_components;
    break;
  }
  cinfo->output_components = (cinfo->quantize_colors ? 1 :
			      cinfo->out_color_components);

  /* See if upsampler will want to emit more than one row at a time */
  if (use_merged_upsample(cinfo))
    cinfo->rec_outbuf_height = cinfo->max_v_samp_factor;
  else
    cinfo->rec_outbuf_height = 1;
}

/* 02856718 */
static void prepare_range_limit_table (j_decompress_ptr cinfo)
/* Allocate and fill in the sample_range_limit table */
{
  JSAMPROW table;
  int i;

  table = (JSAMPROW)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		(5 * (MAXJSAMPLE+1) + CENTERJSAMPLE) * SIZEOF(JSAMPLE));
  table += (MAXJSAMPLE+1);	/* allow negative subscripts of simple table */
  cinfo->sample_range_limit = table;
  /* First segment of "simple" table: limit[x] = 0 for x < 0 */
  MEMZERO(table - (MAXJSAMPLE+1), (MAXJSAMPLE+1) * SIZEOF(JSAMPLE));
  /* Main part of "simple" table: limit[x] = x */
  for (i = 0; i <= MAXJSAMPLE; i++)
    table[i] = (JSAMPLE) i;
  table += CENTERJSAMPLE;	/* Point to where post-IDCT table starts */
  /* End of simple table, rest of first half of post-IDCT table */
  for (i = CENTERJSAMPLE; i < 2*(MAXJSAMPLE+1); i++)
    table[i] = MAXJSAMPLE;
  /* Second half of post-IDCT table */
  MEMZERO(table + (2 * (MAXJSAMPLE+1)),
	  (2 * (MAXJSAMPLE+1) - CENTERJSAMPLE) * SIZEOF(JSAMPLE));
  MEMCOPY(table + (4 * (MAXJSAMPLE+1) - CENTERJSAMPLE),
	  cinfo->sample_range_limit, CENTERJSAMPLE * SIZEOF(JSAMPLE));
}

/* 028567C4 */
static void master_selection (j_decompress_ptr cinfo)
{
  my_master_ptr master = (my_master_ptr) cinfo->master.get();
  boolean use_c_buffer;
  s32 samplesperrow;
  JDIMENSION jd_samplesperrow;

  /* Initialize dimensions and other stuff */
  jpeg_calc_output_dimensions(cinfo);
  prepare_range_limit_table(cinfo);

  /* Width of an output scanline must be representable as JDIMENSION. */
  samplesperrow = (s32) cinfo->output_width * (s32) cinfo->out_color_components;
  jd_samplesperrow = (JDIMENSION) samplesperrow;
  if ((s32) jd_samplesperrow != samplesperrow)
    ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);

  /* Initialize my private state */
  master->pass_number = 0;
  master->using_merged_upsample = use_merged_upsample(cinfo);

  /* Color quantizer selection */
  master->quantizer_1pass = NULL;
  master->quantizer_2pass = NULL;
  /* No mode changes if not using buffered-image mode. */
  if (! cinfo->quantize_colors || ! cinfo->buffered_image) {
    cinfo->enable_1pass_quant = FALSE;
    cinfo->enable_external_quant = FALSE;
    cinfo->enable_2pass_quant = FALSE;
  }
  if (cinfo->quantize_colors) {
    if (cinfo->raw_data_out)
      ERREXIT(cinfo, JERR_NOTIMPL);
    /* 2-pass quantizer only works in 3-component color space. */
    if (cinfo->out_color_components != 3) {
      cinfo->enable_1pass_quant = TRUE;
      cinfo->enable_external_quant = FALSE;
      cinfo->enable_2pass_quant = FALSE;
      cinfo->colormap = NULL;
    } else if (cinfo->colormap != NULL) {
      cinfo->enable_external_quant = TRUE;
    } else if (cinfo->two_pass_quantize) {
      cinfo->enable_2pass_quant = TRUE;
    } else {
      cinfo->enable_1pass_quant = TRUE;
    }

    if (cinfo->enable_1pass_quant) {
#ifdef QUANT_1PASS_SUPPORTED
      jinit_1pass_quantizer(cinfo);
      master->quantizer_1pass = cinfo->cquantize;
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    }

    /* We use the 2-pass code to map to external colormaps. */
    if (cinfo->enable_2pass_quant || cinfo->enable_external_quant) {
#ifdef QUANT_2PASS_SUPPORTED
      jinit_2pass_quantizer(cinfo);
      master->quantizer_2pass = cinfo->cquantize;
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    }
    /* If both quantizers are initialized, the 2-pass one is left active;
     * this is necessary for starting with quantization to an external map.
     */
  }

  /* Post-processing: in particular, color conversion first */
  if (! cinfo->raw_data_out) {
    if (master->using_merged_upsample) {
#ifdef UPSAMPLE_MERGING_SUPPORTED
      jinit_merged_upsampler(cinfo); /* does color conversion too */
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    } else {
      jinit_color_deconverter(cinfo);
      jinit_upsampler(cinfo);
    }
    jinit_d_post_controller(cinfo, cinfo->enable_2pass_quant);
  }
  /* Inverse DCT */
  jinit_inverse_dct(cinfo);
  /* Entropy decoding: either Huffman or arithmetic coding. */
  if (cinfo->arith_code)
    jinit_arith_decoder(cinfo);
  else {
    jinit_huff_decoder(cinfo);
  }

  /* Initialize principal buffer controllers. */
  use_c_buffer = cinfo->inputctl->has_multiple_scans || cinfo->buffered_image;
  jinit_d_coef_controller(cinfo, use_c_buffer);

  if (! cinfo->raw_data_out)
    jinit_d_main_controller(cinfo, FALSE /* never need full buffer here */);

  /* We can now tell the memory manager to allocate virtual arrays. */
  (*cinfo->mem->realize_virt_arrays) ((j_common_ptr) cinfo);

  /* Initialize input side of decompressor to consume first scan. */
  (*cinfo->inputctl->start_input_pass) (cinfo);

#ifdef D_MULTISCAN_FILES_SUPPORTED
  /* If jpeg_start_decompress will read the whole file, initialize
   * progress monitoring appropriately.  The input step is counted
   * as one pass.
   */
  if (cinfo->progress != NULL && ! cinfo->buffered_image &&
      cinfo->inputctl->has_multiple_scans) {
    int nscans;
    /* Estimate number of scans to set pass_limit. */
    if (cinfo->progressive_mode) {
      /* Arbitrarily estimate 2 interleaved DC scans + 3 AC scans/component. */
      nscans = 2 + 3 * cinfo->num_components;
    } else {
      /* For a nonprogressive multiscan file, estimate 1 scan per component. */
      nscans = cinfo->num_components;
    }
    cinfo->progress->pass_counter = 0L;
    cinfo->progress->pass_limit = (s32) cinfo->total_iMCU_rows * nscans;
    cinfo->progress->completed_passes = 0;
    cinfo->progress->total_passes = (cinfo->enable_2pass_quant ? 3 : 2);
    /* Count the input pass as done */
    master->pass_number++;
  }
#endif /* D_MULTISCAN_FILES_SUPPORTED */
}

/* 02856AC4 */
static void prepare_for_output_pass (j_decompress_ptr cinfo)
{
  my_master_ptr master = (my_master_ptr) cinfo->master.get();

  if (master->pub.is_dummy_pass) {
#ifdef QUANT_2PASS_SUPPORTED
    /* Final pass of 2-pass quantization */
    master->pub.is_dummy_pass = FALSE;
    (*cinfo->cquantize->start_pass) (cinfo, FALSE);
    (*cinfo->post->start_pass) (cinfo, JBUF_CRANK_DEST);
    (*cinfo->main->start_pass) (cinfo, JBUF_CRANK_DEST);
#else
    ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif /* QUANT_2PASS_SUPPORTED */
  } else {
    if (cinfo->quantize_colors && cinfo->colormap == NULL) {
      /* Select new quantization method */
      if (cinfo->two_pass_quantize && cinfo->enable_2pass_quant) {
	cinfo->cquantize = master->quantizer_2pass;
	master->pub.is_dummy_pass = TRUE;
      } else if (cinfo->enable_1pass_quant) {
	cinfo->cquantize = master->quantizer_1pass;
      } else {
	ERREXIT(cinfo, JERR_MODE_CHANGE);
      }
    }
    (*cinfo->idct->start_pass) (cinfo);
    (*cinfo->coef->start_output_pass) (cinfo);
    if (! cinfo->raw_data_out) {
      if (! master->using_merged_upsample)
	(*cinfo->cconvert->start_pass) (cinfo);
      (*cinfo->upsample->start_pass) (cinfo);
      if (cinfo->quantize_colors)
	(*cinfo->cquantize->start_pass) (cinfo, master->pub.is_dummy_pass);
      (*cinfo->post->start_pass) (cinfo,
	    (master->pub.is_dummy_pass ? JBUF_SAVE_AND_PASS : JBUF_PASS_THRU));
      (*cinfo->main->start_pass) (cinfo, JBUF_PASS_THRU);
    }
  }

  /* Set up progress monitor's pass info if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->completed_passes = master->pass_number;
    cinfo->progress->total_passes = master->pass_number +
				    (master->pub.is_dummy_pass ? 2 : 1);
    /* In buffered-image mode, we assume one more output pass if EOI not
     * yet reached, but no more passes if EOI has been reached.
     */
    if (cinfo->buffered_image && ! cinfo->inputctl->eoi_reached) {
      cinfo->progress->total_passes += (cinfo->enable_2pass_quant ? 2 : 1);
    }
  }
}

/* 02856D70 */
static void finish_output_pass (j_decompress_ptr cinfo)
{
  my_master_ptr master = (my_master_ptr) cinfo->master.get();

  if (cinfo->quantize_colors)
    (*cinfo->cquantize->finish_pass) (cinfo);
  master->pass_number++;
}

/* 02856DC0 */
void jinit_master_decompress (j_decompress_ptr cinfo)
{
  my_master_ptr master;

  master = (my_master_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(my_decomp_master));
  cinfo->master = (struct jpeg_decomp_master *) master;
  master->pub.prepare_for_output_pass = prepare_for_output_pass;
  master->pub.finish_output_pass = finish_output_pass;

  master->pub.is_dummy_pass = FALSE;

  master_selection(cinfo);
}
/* WWHD glue
ext C000A858 void* master_clear(void* dest,s32 value,u32 size)
ext C000A848 void* master_copy(void* dest,const void* source,u32 size)
ext 02853078 void jpeg_core_output_dimensions(j_decompress_ptr cinfo)
ext 0285A1E0 void jinit_1pass_quantizer(j_decompress_ptr cinfo)
ext 0285B75C void jinit_2pass_quantizer(j_decompress_ptr cinfo)
ext 02862FEC void jinit_color_deconverter(j_decompress_ptr cinfo)
ext 02863B50 void jinit_inverse_dct(j_decompress_ptr cinfo)
ext 028622D0 void jinit_arith_decoder(j_decompress_ptr cinfo)
ext 02865F18 void jinit_huff_decoder(j_decompress_ptr cinfo)
ext 02862A88 void jinit_d_coef_controller(j_decompress_ptr cinfo,boolean need_full_buffer)
ext 02866750 void jinit_d_main_controller(j_decompress_ptr cinfo,boolean need_full_buffer)
ext 0284DFCC s32 jdiv_round_up(s32 a,s32 b)
*/
