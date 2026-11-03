#include "libjpeg/jcdctmgr.h"
static inline s32 jpeg_fdct_multiply(s32 a, s32 b) { return a * b; }
static inline s32 jpeg_fdct_descale(s32 value, s32 bits) { return (value + (1 << (bits - 1))) >> bits; }
/*
 * jcdctmgr.cpp (decompiled WWHD build)
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 *
 * This file contains the forward-DCT management logic.
 * This code selects a particular DCT implementation to be used,
 * and it performs related housekeeping chores including coefficient
 * quantization.
 */
/* The current scaled-DCT routines require ISLOW-style divisor tables,
 * so be sure to compile that code if either ISLOW or SCALING is requested.
 */
/*
 * Perform forward DCT on one or more blocks of a component.
 *
 * The input samples are taken from the sample_data[] array starting at
 * position start_row/start_col, and moving to the right for any additional
 * blocks. The quantized coefficients are returned in coef_blocks[].
 */
/* 0285E024 */
static void
forward_DCT (j_compress_ptr cinfo, jpeg_component_info * compptr,
      JSAMPARRAY sample_data, JBLOCKROW coef_blocks,
      JDIMENSION start_row, JDIMENSION start_col,
      JDIMENSION num_blocks)
/* This version is used for integer DCT implementations. */
{
  /* This routine is heavily used, so it's worth coding it tightly. */
  fdct_controller * fdct = (fdct_controller *) cinfo->fdct.get();
  jpeg_callback<void(be<s32>*, JSAMPARRAY, u32)> do_dct = fdct->do_dct[compptr->component_index].load();
  be<s32>* divisors = fdct->divisors[compptr->quant_tbl_no];
  fdct_workspace workspace; /* work area for FDCT subroutine */
  JDIMENSION bi;
  sample_data += start_row; /* fold in the vertical offset once */
  for (bi = 0; bi < num_blocks; bi++, start_col += compptr->DCT_h_scaled_size) {
    /* Perform the DCT */
    do_dct (workspace.data, sample_data, start_col);
    /* Quantize/descale the coefficients, and store into coef_blocks[] */
    { s32 temp, qval;
      int i;
      be<JCOEF>* output_ptr = coef_blocks[bi];
      for (i = 0; i < DCTSIZE2; i++) {
 qval = divisors[i];
 temp = workspace.data[i];
 /* Divide the coefficient value by qval, ensuring proper rounding.
	 * Since C does not specify the direction of rounding for negative
	 * quotients, we have to force the dividend positive for portability.
	 *
	 * In most files, at least half of the output values will be zero
	 * (at default quantization settings, more like three-quarters...)
	 * so we should ensure that this case is fast.  On many machines,
	 * a comparison is enough cheaper than a divide to make a special test
	 * a win.  Since both inputs will be nonnegative, we need only test
	 * for a < b to discover whether a/b is 0.
	 * If your machine's division is fast enough, define FAST_DIVIDE.
	 */
 if (temp < 0) {
   temp = -temp;
   temp += qval>>1; /* for rounding */
   if (temp >= qval) temp /= qval; else temp = 0;
   temp = -temp;
 } else {
   temp += qval>>1; /* for rounding */
   if (temp >= qval) temp /= qval; else temp = 0;
 }
 output_ptr[i] = (JCOEF) temp;
      }
    }
  }
}
/*
 * Initialize for a processing pass.
 * Verify that all referenced Q-tables are present, and set up
 * the divisor table for each one.
 * In the current implementation, DCT of all components is done during
 * the first pass, even if only some components will be output in the
 * first scan.  Hence all components should be examined here.
 */
// HD: This build retains only the integer 8-by-8 forward DCT methods.
/* 0285E16C */
static void
start_pass_fdctmgr (j_compress_ptr cinfo)
{
  fdct_controller * fdct = (fdct_controller *) cinfo->fdct.get();
  int ci, qtblno, i;
  jpeg_component_info *compptr;
  int method = 0;
  JQUANT_TBL * qtbl;
  be<s32>* dtbl;
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    /* Select the proper DCT routine for this component's scaling */
    switch ((compptr->DCT_h_scaled_size << 8) + compptr->DCT_v_scaled_size) {
    case ((DCTSIZE << 8) + DCTSIZE):
      switch (cinfo->dct_method) {
      case JDCT_ISLOW:
 fdct->do_dct[ci] = jpeg_fdct_islow;
 method = JDCT_ISLOW;
 break;
      case JDCT_IFAST:
 fdct->do_dct[ci] = jpeg_fdct_ifast;
 method = JDCT_IFAST;
 break;
      default:
 jpeg_error(cinfo, JERR_NOT_COMPILED);
 break;
      }
      break;
    default:
      jpeg_error2(cinfo, JERR_BAD_DCTSIZE,
        compptr->DCT_h_scaled_size, compptr->DCT_v_scaled_size);
      break;
    }
    qtblno = compptr->quant_tbl_no;
    /* Make sure specified quantization table is present */
    if (qtblno < 0 || qtblno >= NUM_QUANT_TBLS ||
 cinfo->quant_tbl_ptrs[qtblno] == nullptr)
      jpeg_error1(cinfo, JERR_NO_QUANT_TABLE, qtblno);
    qtbl = cinfo->quant_tbl_ptrs[qtblno];
    /* Compute divisors for this quant table */
    /* We may do this more than once for same table, but it's not a big deal */
    switch (method) {
    case JDCT_ISLOW:
      /* For LL&M IDCT method, divisors are equal to raw quantization
       * coefficients multiplied by 8 (to counteract scaling).
       */
      if (fdct->divisors[qtblno] == nullptr) {
 fdct->divisors[qtblno] = (be<s32>*)
   (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
          DCTSIZE2 * sizeof(s32));
      }
      dtbl = fdct->divisors[qtblno];
      for (i = 0; i < DCTSIZE2; i++) {
 dtbl[i] = ((s32) qtbl->quantval[i]) << 3;
      }
      fdct->pub.forward_DCT[ci] = forward_DCT;
      break;
    case JDCT_IFAST:
      {
 /* For AA&N IDCT method, divisors are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 * We apply a further scale factor of 8.
	 */
 static const s16 aanscales[64] = {
   /* precomputed values scaled up by 14 bits */
   16384, 22725, 21407, 19266, 16384, 12873, 8867, 4520,
   22725, 31521, 29692, 26722, 22725, 17855, 12299, 6270,
   21407, 29692, 27969, 25172, 21407, 16819, 11585, 5906,
   19266, 26722, 25172, 22654, 19266, 15137, 10426, 5315,
   16384, 22725, 21407, 19266, 16384, 12873, 8867, 4520,
   12873, 17855, 16819, 15137, 12873, 10114, 6967, 3552,
    8867, 12299, 11585, 10426, 8867, 6967, 4799, 2446,
    4520, 6270, 5906, 5315, 4520, 3552, 2446, 1247
 };
 
 if (fdct->divisors[qtblno] == nullptr) {
   fdct->divisors[qtblno] = (be<s32>*)
     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
     DCTSIZE2 * sizeof(s32));
 }
 dtbl = fdct->divisors[qtblno];
 for (i = 0; i < DCTSIZE2; i++) {
   dtbl[i] = (s32)
     jpeg_fdct_descale(jpeg_fdct_multiply((s32) qtbl->quantval[i],
      (s32) aanscales[i]),
      14 -3);
 }
      }
      fdct->pub.forward_DCT[ci] = forward_DCT;
      break;
    default:
      jpeg_error(cinfo, JERR_NOT_COMPILED);
      break;
    }
  }
}
/*
 * Initialize FDCT manager.
 */
/* 0285E3C8 */
void
jinit_forward_dct (j_compress_ptr cinfo)
{
  fdct_controller * fdct;
  int i;
  fdct = (fdct_controller *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
    sizeof(fdct_controller));
  cinfo->fdct = (struct jpeg_forward_dct *) fdct;
  fdct->pub.start_pass = start_pass_fdctmgr;
  /* Mark divisor tables unallocated */
  for (i = 0; i < NUM_QUANT_TBLS; i++) {
    fdct->divisors[i] = nullptr;
  }
}

/* WWHD glue
data aanscales 1017D6A8
*/
