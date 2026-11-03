/*
 * jddctmgr.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2002-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the inverse-DCT management logic.
 * This code selects a particular IDCT implementation to be used,
 * and it performs related housekeeping chores.  No code in this file
 * is executed per IDCT step, only during output pass setup.
 *
 * Note that the IDCT routines are responsible for performing coefficient
 * dequantization as well as the IDCT proper.  This module sets up the
 * dequantization multiplier table needed by the IDCT routine.
 */
// Decompiled WWHD build: guest layout and pointer adaptation.
#include "libjpeg/jddctmgr.h"
/* 02863288 */
static void start_pass (j_decompress_ptr cinfo)
{
  my_idct_ptr idct = (my_idct_ptr) cinfo->idct.get();
  int ci, i;
  jpeg_component_info *compptr;
  int method = 0;
  auto method_ptr = static_cast<inverse_DCT_method_ptr>(nullptr);
  JQUANT_TBL * qtbl;

  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    /* Select the proper IDCT routine for this component's scaling */
    switch ((compptr->DCT_h_scaled_size << 8) + compptr->DCT_v_scaled_size) {
#ifdef IDCT_SCALING_SUPPORTED
    case ((1 << 8) + 1):
      method_ptr = jpeg_idct_1x1;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((2 << 8) + 2):
      method_ptr = jpeg_idct_2x2;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((3 << 8) + 3):
      method_ptr = jpeg_idct_3x3;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((4 << 8) + 4):
      method_ptr = jpeg_idct_4x4;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((5 << 8) + 5):
      method_ptr = jpeg_idct_5x5;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((6 << 8) + 6):
      method_ptr = jpeg_idct_6x6;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((7 << 8) + 7):
      method_ptr = jpeg_idct_7x7;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((9 << 8) + 9):
      method_ptr = jpeg_idct_9x9;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((10 << 8) + 10):
      method_ptr = jpeg_idct_10x10;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((11 << 8) + 11):
      method_ptr = jpeg_idct_11x11;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((12 << 8) + 12):
      method_ptr = jpeg_idct_12x12;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((13 << 8) + 13):
      method_ptr = jpeg_idct_13x13;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((14 << 8) + 14):
      method_ptr = jpeg_idct_14x14;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((15 << 8) + 15):
      method_ptr = jpeg_idct_15x15;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((16 << 8) + 16):
      method_ptr = jpeg_idct_16x16;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((16 << 8) + 8):
      method_ptr = jpeg_idct_16x8;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((14 << 8) + 7):
      method_ptr = jpeg_idct_14x7;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((12 << 8) + 6):
      method_ptr = jpeg_idct_12x6;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((10 << 8) + 5):
      method_ptr = jpeg_idct_10x5;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((8 << 8) + 4):
      method_ptr = jpeg_idct_8x4;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((6 << 8) + 3):
      method_ptr = jpeg_idct_6x3;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((4 << 8) + 2):
      method_ptr = jpeg_idct_4x2;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((2 << 8) + 1):
      method_ptr = jpeg_idct_2x1;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((8 << 8) + 16):
      method_ptr = jpeg_idct_8x16;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((7 << 8) + 14):
      method_ptr = jpeg_idct_7x14;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((6 << 8) + 12):
      method_ptr = jpeg_idct_6x12;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((5 << 8) + 10):
      method_ptr = jpeg_idct_5x10;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((4 << 8) + 8):
      method_ptr = jpeg_idct_4x8;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((3 << 8) + 6):
      method_ptr = jpeg_idct_3x6;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((2 << 8) + 4):
      method_ptr = jpeg_idct_2x4;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
    case ((1 << 8) + 2):
      method_ptr = jpeg_idct_1x2;
      method = JDCT_ISLOW;	/* jidctint uses islow-style table */
      break;
#endif
    case ((DCTSIZE << 8) + DCTSIZE):
      switch (cinfo->dct_method) {
#ifdef DCT_ISLOW_SUPPORTED
      case JDCT_ISLOW:
	method_ptr = jpeg_idct_islow;
	method = JDCT_ISLOW;
	break;
#endif
#ifdef DCT_IFAST_SUPPORTED
      case JDCT_IFAST:
	method_ptr = jpeg_idct_ifast;
	method = JDCT_IFAST;
	break;
#endif
      default:
	ERREXIT(cinfo, JERR_NOT_COMPILED);
	break;
      }
      break;
    default:
      ERREXIT2(cinfo, JERR_BAD_DCTSIZE,
	       compptr->DCT_h_scaled_size, compptr->DCT_v_scaled_size);
      break;
    }
    idct->pub.inverse_DCT[ci] = method_ptr;
    /* Create multiplier table from quant table.
     * However, we can skip this if the component is uninteresting
     * or if we already built the table.  Also, if no quant table
     * has yet been saved for the component, we leave the
     * multiplier table all-zero; we'll be reading zeroes from the
     * coefficient controller's buffer anyway.
     */
    if (! compptr->component_needed || idct->cur_method[ci] == method)
      continue;
    qtbl = compptr->quant_table;
    if (qtbl == NULL)		/* happens if no data yet for component */
      continue;
    idct->cur_method[ci] = method;
    switch (method) {
#ifdef PROVIDE_ISLOW_TABLES
    case JDCT_ISLOW:
      {
	/* For LL&M IDCT method, multipliers are equal to raw quantization
	 * coefficients, but are stored as ints to ensure access efficiency.
	 */
	be<ISLOW_MULT_TYPE>* ismtbl = (be<ISLOW_MULT_TYPE>*) compptr->dct_table.get();
	for (i = 0; i < DCTSIZE2; i++) {
	  ismtbl[i] = (ISLOW_MULT_TYPE) qtbl->quantval[i];
	}
      }
      break;
#endif
#ifdef DCT_IFAST_SUPPORTED
    case JDCT_IFAST:
      {
	/* For AA&N IDCT method, multipliers are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 * For integer operation, the multiplier table is to be scaled by
	 * IFAST_SCALE_BITS.
	 */
	be<IFAST_MULT_TYPE>* ifmtbl = (be<IFAST_MULT_TYPE>*) compptr->dct_table.get();
#define CONST_BITS 14
	static const s16 aanscales[64] = {
	  /* precomputed values scaled up by 14 bits */
	  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
	  22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
	  21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
	  19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
	  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
	  12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
	   8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
	   4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
	};
	SHIFT_TEMPS

	for (i = 0; i < DCTSIZE2; i++) {
	  ifmtbl[i] = (IFAST_MULT_TYPE)
	    DESCALE(MULTIPLY16V16((INT32) qtbl->quantval[i],
				  (INT32) aanscales[i]),
		    CONST_BITS-IFAST_SCALE_BITS);
	}
      }
      break;
#endif
    default:
      ERREXIT(cinfo, JERR_NOT_COMPILED);
      break;
    }
  }
}

/* 02863B50 */
void jinit_inverse_dct (j_decompress_ptr cinfo)
{
  my_idct_ptr idct;
  int ci;
  jpeg_component_info *compptr;

  idct = (my_idct_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(my_idct_controller));
  cinfo->idct = (jpeg_inverse_dct*) idct;
  idct->pub.start_pass = start_pass;

  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    /* Allocate and pre-zero a multiplier table for each component */
    compptr->dct_table =
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(multiplier_table));
    jpeg_clear_memory(compptr->dct_table.get(), 0, SIZEOF(multiplier_table));
    /* Mark multiplier table not yet set up for any method */
    idct->cur_method[ci] = -1;
  }
}
/* WWHD glue
ext C000A858 void* jpeg_clear_memory(void* dest,s32 value,u32 size)
data aanscales 1017D728
*/
