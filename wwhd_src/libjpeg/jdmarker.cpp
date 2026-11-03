/*
 * jdmarker.c
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains routines to decode JPEG datastream markers.
 * Most of the complexity arises from our desire to support input
 * suspension: if not all of the data for a marker is available,
 * we must exit back to the application.  On resumption, we reprocess
 * the marker.
 */
// Decompiled WWHD build: guest byte streams and suspension boundaries.
// The goto exits the component-ID search immediately after a matching component.
#include "libjpeg/jdmarker.h"
static const int jpeg_natural_order[80] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6,  7, 14, 21, 28,
 35, 42, 49, 56, 57, 50, 43, 36,
 29, 22, 15, 23, 30, 37, 44, 51,
 58, 59, 52, 45, 38, 31, 39, 46,
 53, 60, 61, 54, 47, 55, 62, 63,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order7[7*7+16] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6, 14, 21, 28, 35,
 42, 49, 50, 43, 36, 29, 22, 30,
 37, 44, 51, 52, 45, 38, 46, 53,
 54,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order6[6*6+16] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 41, 34, 27,
 20, 13, 21, 28, 35, 42, 43, 36,
 29, 37, 44, 45,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order5[5*5+16] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4, 12,
 19, 26, 33, 34, 27, 20, 28, 35,
 36,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order4[4*4+16] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 25, 18, 11, 19, 26, 27,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order3[3*3+16] = {
  0,  1,  8, 16,  9,  2, 10, 17,
 18,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};

static const int jpeg_natural_order2[2*2+16] = {
  0,  1,  8,  9,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};
/* 02853E78 */
static boolean get_soi (j_decompress_ptr cinfo)
/* Process an SOI marker */
{
  int i;
  
  TRACEMS(cinfo, 1, JTRC_SOI);

  if (cinfo->marker->saw_SOI)
    ERREXIT(cinfo, JERR_SOI_DUPLICATE);

  /* Reset all parameters that are defined to be reset by SOI */

  for (i = 0; i < NUM_ARITH_TBLS; i++) {
    cinfo->arith_dc_L[i] = 0;
    cinfo->arith_dc_U[i] = 1;
    cinfo->arith_ac_K[i] = 5;
  }
  cinfo->restart_interval = 0;

  /* Set initial assumptions for colorspace etc */

  cinfo->jpeg_color_space = JCS_UNKNOWN;
  cinfo->CCIR601_sampling = FALSE; /* Assume non-CCIR sampling??? */

  cinfo->saw_JFIF_marker = FALSE;
  cinfo->JFIF_major_version = 1; /* set default JFIF APP0 values */
  cinfo->JFIF_minor_version = 1;
  cinfo->density_unit = 0;
  cinfo->X_density = 1;
  cinfo->Y_density = 1;
  cinfo->saw_Adobe_marker = FALSE;
  cinfo->Adobe_transform = 0;

  cinfo->marker->saw_SOI = TRUE;

  return TRUE;
}

/* 02853F4C */
static boolean get_sof (j_decompress_ptr cinfo, boolean is_baseline, boolean is_prog,
	 boolean is_arith)
/* Process a SOFn marker */
{
  INT32 length;
  int c, ci;
  jpeg_component_info * compptr;
  INPUT_VARS(cinfo);

  cinfo->is_baseline = is_baseline;
  cinfo->progressive_mode = is_prog;
  cinfo->arith_code = is_arith;

  INPUT_2BYTES(cinfo, length, return FALSE);

  INPUT_BYTE(cinfo, cinfo->data_precision, return FALSE);
  INPUT_2BYTES(cinfo, cinfo->image_height, return FALSE);
  INPUT_2BYTES(cinfo, cinfo->image_width, return FALSE);
  INPUT_BYTE(cinfo, cinfo->num_components, return FALSE);

  length -= 8;

  TRACEMS4(cinfo, 1, JTRC_SOF, cinfo->unread_marker,
	   (int) cinfo->image_width, (int) cinfo->image_height,
	   cinfo->num_components);

  if (cinfo->marker->saw_SOF)
    ERREXIT(cinfo, JERR_SOF_DUPLICATE);

  /* We don't support files in which the image height is initially specified */
  /* as 0 and is later redefined by DNL.  As long as we have to check that,  */
  /* might as well have a general sanity check. */
  if (cinfo->image_height <= 0 || cinfo->image_width <= 0
      || cinfo->num_components <= 0)
    ERREXIT(cinfo, JERR_EMPTY_IMAGE);

  if (length != (cinfo->num_components * 3))
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  if (cinfo->comp_info == NULL)	/* do only once, even if suspend */
    cinfo->comp_info = (jpeg_component_info *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_IMAGE,
			 cinfo->num_components * SIZEOF(jpeg_component_info));
  
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    compptr->component_index = ci;
    INPUT_BYTE(cinfo, compptr->component_id, return FALSE);
    INPUT_BYTE(cinfo, c, return FALSE);
    compptr->h_samp_factor = (c >> 4) & 15;
    compptr->v_samp_factor = (c     ) & 15;
    INPUT_BYTE(cinfo, compptr->quant_tbl_no, return FALSE);

    TRACEMS4(cinfo, 1, JTRC_SOF_COMPONENT,
	     compptr->component_id, compptr->h_samp_factor,
	     compptr->v_samp_factor, compptr->quant_tbl_no);
  }

  cinfo->marker->saw_SOF = TRUE;

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02854398 */
static boolean get_sos (j_decompress_ptr cinfo)
/* Process a SOS marker */
{
  INT32 length;
  int i, ci, n, c, cc;
  jpeg_component_info * compptr;
  INPUT_VARS(cinfo);

  if (! cinfo->marker->saw_SOF)
    ERREXIT(cinfo, JERR_SOS_NO_SOF);

  INPUT_2BYTES(cinfo, length, return FALSE);

  INPUT_BYTE(cinfo, n, return FALSE); /* Number of components */

  TRACEMS1(cinfo, 1, JTRC_SOS, n);

  if (length != (n * 2 + 6) || n > MAX_COMPS_IN_SCAN ||
      (n == 0 && !cinfo->progressive_mode))
      /* pseudo SOS marker only allowed in progressive mode */
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  cinfo->comps_in_scan = n;

  /* Collect the component-spec parameters */

  for (i = 0; i < n; i++) {
    INPUT_BYTE(cinfo, cc, return FALSE);
    INPUT_BYTE(cinfo, c, return FALSE);
    
    for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	 ci++, compptr++) {
      if (cc == compptr->component_id)
	goto id_found;
    }

    ERREXIT1(cinfo, JERR_BAD_COMPONENT_ID, cc);

  id_found:

    cinfo->cur_comp_info[i] = compptr;
    compptr->dc_tbl_no = (c >> 4) & 15;
    compptr->ac_tbl_no = (c     ) & 15;
    
    TRACEMS3(cinfo, 1, JTRC_SOS_COMPONENT, cc,
	     compptr->dc_tbl_no, compptr->ac_tbl_no);
  }

  /* Collect the additional scan parameters Ss, Se, Ah/Al. */
  INPUT_BYTE(cinfo, c, return FALSE);
  cinfo->Ss = c;
  INPUT_BYTE(cinfo, c, return FALSE);
  cinfo->Se = c;
  INPUT_BYTE(cinfo, c, return FALSE);
  cinfo->Ah = (c >> 4) & 15;
  cinfo->Al = (c     ) & 15;

  TRACEMS4(cinfo, 1, JTRC_SOS_PARAMS, cinfo->Ss, cinfo->Se,
	   cinfo->Ah, cinfo->Al);

  /* Prepare to scan data & restart markers */
  cinfo->marker->next_restart_num = 0;

  /* Count another (non-pseudo) SOS marker */
  if (n) cinfo->input_scan_number++;

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02854754 */
static boolean get_dac (j_decompress_ptr cinfo)
/* Process a DAC marker */
{
  INT32 length;
  int index, val;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  length -= 2;
  
  while (length > 0) {
    INPUT_BYTE(cinfo, index, return FALSE);
    INPUT_BYTE(cinfo, val, return FALSE);

    length -= 2;

    TRACEMS2(cinfo, 1, JTRC_DAC, index, val);

    if (index < 0 || index >= (2*NUM_ARITH_TBLS))
      ERREXIT1(cinfo, JERR_DAC_INDEX, index);

    if (index >= NUM_ARITH_TBLS) { /* define AC table */
      cinfo->arith_ac_K[index-NUM_ARITH_TBLS] = (UINT8) val;
    } else {			/* define DC table */
      cinfo->arith_dc_L[index] = (UINT8) (val & 0x0F);
      cinfo->arith_dc_U[index] = (UINT8) (val >> 4);
      if (cinfo->arith_dc_L[index] > cinfo->arith_dc_U[index])
	ERREXIT1(cinfo, JERR_DAC_VALUE, val);
    }
  }

  if (length != 0)
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 0285494C */
static boolean get_dht (j_decompress_ptr cinfo)
/* Process a DHT marker */
{
  INT32 length;
  marker_huffman_buffer scratch;
  
  int i, index, count;
  gptr<JHUFF_TBL>* htblptr;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  length -= 2;
  
  while (length > 16) {
    INPUT_BYTE(cinfo, index, return FALSE);

    TRACEMS1(cinfo, 1, JTRC_DHT, index);
      
    scratch.bits[0] = 0;
    count = 0;
    for (i = 1; i <= 16; i++) {
      INPUT_BYTE(cinfo, scratch.bits[i], return FALSE);
      count += scratch.bits[i];
    }

    length -= 1 + 16;

    TRACEMS8(cinfo, 2, JTRC_HUFFBITS,
	     scratch.bits[1], scratch.bits[2], scratch.bits[3], scratch.bits[4],
	     scratch.bits[5], scratch.bits[6], scratch.bits[7], scratch.bits[8]);
    TRACEMS8(cinfo, 2, JTRC_HUFFBITS,
	     scratch.bits[9], scratch.bits[10], scratch.bits[11], scratch.bits[12],
	     scratch.bits[13], scratch.bits[14], scratch.bits[15], scratch.bits[16]);

    /* Here we just do minimal validation of the counts to avoid walking
     * off the end of our table space.  jdhuff.c will check more carefully.
     */
    if (count > 256 || ((INT32) count) > length)
      ERREXIT(cinfo, JERR_BAD_HUFF_TABLE);

    for (i = 0; i < count; i++)
      INPUT_BYTE(cinfo, scratch.huffval[i], return FALSE);

    length -= count;

    if (index & 0x10) {		/* AC table definition */
      index -= 0x10;
      htblptr = &cinfo->ac_huff_tbl_ptrs[index];
    } else {			/* DC table definition */
      htblptr = &cinfo->dc_huff_tbl_ptrs[index];
    }

    if (index < 0 || index >= NUM_HUFF_TBLS)
      ERREXIT1(cinfo, JERR_DHT_INDEX, index);

    if (*htblptr == NULL)
      *htblptr = jpeg_alloc_huff_table((j_common_ptr) cinfo);
  
    MEMCOPY((*htblptr)->bits, scratch.bits, SIZEOF((*htblptr)->bits));
    MEMCOPY((*htblptr)->huffval, scratch.huffval, SIZEOF((*htblptr)->huffval));
  }

  if (length != 0)
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02854CD0 */
static boolean get_dqt (j_decompress_ptr cinfo)
/* Process a DQT marker */
{
  INT32 length, count, i;
  int n, prec;
  unsigned int tmp;
  JQUANT_TBL *quant_ptr;
  const be<s32>* natural_order;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  length -= 2;

  while (length > 0) {
    length--;
    INPUT_BYTE(cinfo, n, return FALSE);
    prec = n >> 4;
    n &= 0x0F;

    TRACEMS2(cinfo, 1, JTRC_DQT, n, prec);

    if (n >= NUM_QUANT_TBLS)
      ERREXIT1(cinfo, JERR_DQT_INDEX, n);
      
    if (cinfo->quant_tbl_ptrs[n] == NULL)
      cinfo->quant_tbl_ptrs[n] = jpeg_alloc_quant_table((j_common_ptr) cinfo);
    quant_ptr = cinfo->quant_tbl_ptrs[n];

    if (prec) {
      if (length < DCTSIZE2 * 2) {
	/* Initialize full table for safety. */
	for (i = 0; i < DCTSIZE2; i++) {
	  quant_ptr->quantval[i] = 1;
	}
	count = length >> 1;
      } else
	count = DCTSIZE2;
    } else {
      if (length < DCTSIZE2) {
	/* Initialize full table for safety. */
	for (i = 0; i < DCTSIZE2; i++) {
	  quant_ptr->quantval[i] = 1;
	}
	count = length;
      } else
	count = DCTSIZE2;
    }

    switch (count) {
    case (2*2): natural_order = jpeg_natural_order2; break;
    case (3*3): natural_order = jpeg_natural_order3; break;
    case (4*4): natural_order = jpeg_natural_order4; break;
    case (5*5): natural_order = jpeg_natural_order5; break;
    case (6*6): natural_order = jpeg_natural_order6; break;
    case (7*7): natural_order = jpeg_natural_order7; break;
    default:    natural_order = jpeg_natural_order;  break;
    }

    for (i = 0; i < count; i++) {
      if (prec)
	INPUT_2BYTES(cinfo, tmp, return FALSE);
      else
	INPUT_BYTE(cinfo, tmp, return FALSE);
      /* We convert the zigzag-order table to natural array order. */
      quant_ptr->quantval[natural_order[i]] = (UINT16) tmp;
    }

    if (cinfo->err->trace_level >= 2) {
      for (i = 0; i < DCTSIZE2; i += 8) {
	TRACEMS8(cinfo, 2, JTRC_QUANTVALS,
		 quant_ptr->quantval[i],   quant_ptr->quantval[i+1],
		 quant_ptr->quantval[i+2], quant_ptr->quantval[i+3],
		 quant_ptr->quantval[i+4], quant_ptr->quantval[i+5],
		 quant_ptr->quantval[i+6], quant_ptr->quantval[i+7]);
      }
    }

    length -= count;
    if (prec) length -= count;
  }

  if (length != 0)
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02855168 */
static boolean get_dri (j_decompress_ptr cinfo)
/* Process a DRI marker */
{
  INT32 length;
  unsigned int tmp;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  
  if (length != 4)
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  INPUT_2BYTES(cinfo, tmp, return FALSE);

  TRACEMS1(cinfo, 1, JTRC_DRI, tmp);

  cinfo->restart_interval = tmp;

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 028552E4 */
static void examine_app0 (j_decompress_ptr cinfo, be<JOCTET>* data,
	      unsigned int datalen, INT32 remaining)
/* Examine first few bytes from an APP0.
 * Take appropriate action if it is a JFIF marker.
 * datalen is # of bytes at data[], remaining is length of rest of marker data.
 */
{
  INT32 totallen = (INT32) datalen + remaining;

  if (datalen >= APP0_DATA_LEN &&
      GETJOCTET(data[0]) == 0x4A &&
      GETJOCTET(data[1]) == 0x46 &&
      GETJOCTET(data[2]) == 0x49 &&
      GETJOCTET(data[3]) == 0x46 &&
      GETJOCTET(data[4]) == 0) {
    /* Found JFIF APP0 marker: save info */
    cinfo->saw_JFIF_marker = TRUE;
    cinfo->JFIF_major_version = GETJOCTET(data[5]);
    cinfo->JFIF_minor_version = GETJOCTET(data[6]);
    cinfo->density_unit = GETJOCTET(data[7]);
    cinfo->X_density = (GETJOCTET(data[8]) << 8) + GETJOCTET(data[9]);
    cinfo->Y_density = (GETJOCTET(data[10]) << 8) + GETJOCTET(data[11]);
    /* Check version.
     * Major version must be 1, anything else signals an incompatible change.
     * (We used to treat this as an error, but now it's a nonfatal warning,
     * because some bozo at Hijaak couldn't read the spec.)
     * Minor version should be 0..2, but process anyway if newer.
     */
    if (cinfo->JFIF_major_version != 1)
      WARNMS2(cinfo, JWRN_JFIF_MAJOR,
	      cinfo->JFIF_major_version, cinfo->JFIF_minor_version);
    /* Generate trace messages */
    TRACEMS5(cinfo, 1, JTRC_JFIF,
	     cinfo->JFIF_major_version, cinfo->JFIF_minor_version,
	     cinfo->X_density, cinfo->Y_density, cinfo->density_unit);
    /* Validate thumbnail dimensions and issue appropriate messages */
    if (GETJOCTET(data[12]) | GETJOCTET(data[13]))
      TRACEMS2(cinfo, 1, JTRC_JFIF_THUMBNAIL,
	       GETJOCTET(data[12]), GETJOCTET(data[13]));
    totallen -= APP0_DATA_LEN;
    if (totallen !=
	((INT32)GETJOCTET(data[12]) * (INT32)GETJOCTET(data[13]) * (INT32) 3))
      TRACEMS1(cinfo, 1, JTRC_JFIF_BADTHUMBNAILSIZE, (int) totallen);
  } else if (datalen >= 6 &&
      GETJOCTET(data[0]) == 0x4A &&
      GETJOCTET(data[1]) == 0x46 &&
      GETJOCTET(data[2]) == 0x58 &&
      GETJOCTET(data[3]) == 0x58 &&
      GETJOCTET(data[4]) == 0) {
    /* Found JFIF "JFXX" extension APP0 marker */
    /* The library doesn't actually do anything with these,
     * but we try to produce a helpful trace message.
     */
    switch (GETJOCTET(data[5])) {
    case 0x10:
      TRACEMS1(cinfo, 1, JTRC_THUMB_JPEG, (int) totallen);
      break;
    case 0x11:
      TRACEMS1(cinfo, 1, JTRC_THUMB_PALETTE, (int) totallen);
      break;
    case 0x13:
      TRACEMS1(cinfo, 1, JTRC_THUMB_RGB, (int) totallen);
      break;
    default:
      TRACEMS2(cinfo, 1, JTRC_JFIF_EXTENSION,
	       GETJOCTET(data[5]), (int) totallen);
      break;
    }
  } else {
    /* Start of APP0 does not match "JFIF" or "JFXX", or too short */
    TRACEMS1(cinfo, 1, JTRC_APP0, (int) totallen);
  }
}

/* 028555C8 */
static void examine_app14 (j_decompress_ptr cinfo, be<JOCTET>* data,
	       unsigned int datalen, INT32 remaining)
/* Examine first few bytes from an APP14.
 * Take appropriate action if it is an Adobe marker.
 * datalen is # of bytes at data[], remaining is length of rest of marker data.
 */
{
  unsigned int version, flags0, flags1, transform;

  if (datalen >= APP14_DATA_LEN &&
      GETJOCTET(data[0]) == 0x41 &&
      GETJOCTET(data[1]) == 0x64 &&
      GETJOCTET(data[2]) == 0x6F &&
      GETJOCTET(data[3]) == 0x62 &&
      GETJOCTET(data[4]) == 0x65) {
    /* Found Adobe APP14 marker */
    version = (GETJOCTET(data[5]) << 8) + GETJOCTET(data[6]);
    flags0 = (GETJOCTET(data[7]) << 8) + GETJOCTET(data[8]);
    flags1 = (GETJOCTET(data[9]) << 8) + GETJOCTET(data[10]);
    transform = GETJOCTET(data[11]);
    TRACEMS4(cinfo, 1, JTRC_ADOBE, version, flags0, flags1, transform);
    cinfo->saw_Adobe_marker = TRUE;
    cinfo->Adobe_transform = (UINT8) transform;
  } else {
    /* Start of APP14 does not match "Adobe", or too short */
    TRACEMS1(cinfo, 1, JTRC_APP14, (int) (datalen + remaining));
  }
}

/* 028556C8 */
static boolean get_interesting_appn (j_decompress_ptr cinfo)
/* Process an APP0 or APP14 marker without saving it */
{
  INT32 length;
  marker_app_bytes b;
  unsigned int i, numtoread;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  length -= 2;

  /* get the interesting part of the marker data */
  if (length >= APPN_DATA_LEN)
    numtoread = APPN_DATA_LEN;
  else if (length > 0)
    numtoread = (unsigned int) length;
  else
    numtoread = 0;
  for (i = 0; i < numtoread; i++)
    INPUT_BYTE(cinfo, b.bytes[i], return FALSE);
  length -= numtoread;

  /* process it */
  switch (cinfo->unread_marker) {
  case M_APP0:
    examine_app0(cinfo, b.bytes, numtoread, length);
    break;
  case M_APP14:
    examine_app14(cinfo, b.bytes, numtoread, length);
    break;
  default:
    /* can't get here unless jpeg_save_markers chooses wrong processor */
    ERREXIT1(cinfo, JERR_UNKNOWN_MARKER, cinfo->unread_marker);
    break;
  }

  /* skip any remaining data -- could be lots */
  INPUT_SYNC(cinfo);
  if (length > 0)
    (*cinfo->src->skip_input_data) (cinfo, (s32) length);

  return TRUE;
}

/* 02855898 */
static boolean skip_variable (j_decompress_ptr cinfo)
/* Skip over an unknown or uninteresting variable-length marker */
{
  INT32 length;
  INPUT_VARS(cinfo);

  INPUT_2BYTES(cinfo, length, return FALSE);
  length -= 2;
  
  TRACEMS2(cinfo, 1, JTRC_MISC_MARKER, cinfo->unread_marker, (int) length);

  INPUT_SYNC(cinfo);		/* do before skip_input_data */
  if (length > 0)
    (*cinfo->src->skip_input_data) (cinfo, (s32) length);

  return TRUE;
}

/* 028559AC */
static boolean next_marker (j_decompress_ptr cinfo)
{
  int c;
  INPUT_VARS(cinfo);

  for (;;) {
    INPUT_BYTE(cinfo, c, return FALSE);
    /* Skip any non-FF bytes.
     * This may look a bit inefficient, but it will not occur in a valid file.
     * We sync after each discarded byte so that a suspending data source
     * can discard the byte from its buffer.
     */
    while (c != 0xFF) {
      cinfo->marker->discarded_bytes++;
      INPUT_SYNC(cinfo);
      INPUT_BYTE(cinfo, c, return FALSE);
    }
    /* This loop swallows any duplicate FF bytes.  Extra FFs are legal as
     * pad bytes, so don't count them in discarded_bytes.  We assume there
     * will not be so many consecutive FF bytes as to overflow a suspending
     * data source's input buffer.
     */
    do {
      INPUT_BYTE(cinfo, c, return FALSE);
    } while (c == 0xFF);
    if (c != 0)
      break;			/* found a valid marker, exit loop */
    /* Reach here if we found a stuffed-zero data sequence (FF/00).
     * Discard it and loop back to try again.
     */
    cinfo->marker->discarded_bytes += 2;
    INPUT_SYNC(cinfo);
  }

  if (cinfo->marker->discarded_bytes != 0) {
    WARNMS2(cinfo, JWRN_EXTRANEOUS_DATA, cinfo->marker->discarded_bytes, c);
    cinfo->marker->discarded_bytes = 0;
  }

  cinfo->unread_marker = c;

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02855B24 */
static boolean first_marker (j_decompress_ptr cinfo)
/* Like next_marker, but used to obtain the initial SOI marker. */
/* For this marker, we do not allow preceding garbage or fill; otherwise,
 * we might well scan an entire input file before realizing it ain't JPEG.
 * If an application wants to process non-JFIF files, it must seek to the
 * SOI before calling the JPEG library.
 */
{
  int c, c2;
  INPUT_VARS(cinfo);

  INPUT_BYTE(cinfo, c, return FALSE);
  INPUT_BYTE(cinfo, c2, return FALSE);
  if (c != 0xFF || c2 != (int) M_SOI)
    ERREXIT2(cinfo, JERR_NO_SOI, c, c2);

  cinfo->unread_marker = c2;

  INPUT_SYNC(cinfo);
  return TRUE;
}

/* 02855C18 */
static int read_markers (j_decompress_ptr cinfo)
{
  /* Outer loop repeats once for each marker. */
  for (;;) {
    /* Collect the marker proper, unless we already did. */
    /* NB: first_marker() enforces the requirement that SOI appear first. */
    if (cinfo->unread_marker == 0) {
      if (! cinfo->marker->saw_SOI) {
	if (! first_marker(cinfo))
	  return JPEG_SUSPENDED;
      } else {
	if (! next_marker(cinfo))
	  return JPEG_SUSPENDED;
      }
    }
    /* At this point cinfo->unread_marker contains the marker code and the
     * input point is just past the marker proper, but before any parameters.
     * A suspension will cause us to return with this state still true.
     */
    switch (cinfo->unread_marker) {
    case M_SOI:
      if (! get_soi(cinfo))
	return JPEG_SUSPENDED;
      break;

    case M_SOF0:		/* Baseline */
      if (! get_sof(cinfo, TRUE, FALSE, FALSE))
	return JPEG_SUSPENDED;
      break;

    case M_SOF1:		/* Extended sequential, Huffman */
      if (! get_sof(cinfo, FALSE, FALSE, FALSE))
	return JPEG_SUSPENDED;
      break;

    case M_SOF2:		/* Progressive, Huffman */
      if (! get_sof(cinfo, FALSE, TRUE, FALSE))
	return JPEG_SUSPENDED;
      break;

    case M_SOF9:		/* Extended sequential, arithmetic */
      if (! get_sof(cinfo, FALSE, FALSE, TRUE))
	return JPEG_SUSPENDED;
      break;

    case M_SOF10:		/* Progressive, arithmetic */
      if (! get_sof(cinfo, FALSE, TRUE, TRUE))
	return JPEG_SUSPENDED;
      break;

    /* Currently unsupported SOFn types */
    case M_SOF3:		/* Lossless, Huffman */
    case M_SOF5:		/* Differential sequential, Huffman */
    case M_SOF6:		/* Differential progressive, Huffman */
    case M_SOF7:		/* Differential lossless, Huffman */
    case M_JPG:			/* Reserved for JPEG extensions */
    case M_SOF11:		/* Lossless, arithmetic */
    case M_SOF13:		/* Differential sequential, arithmetic */
    case M_SOF14:		/* Differential progressive, arithmetic */
    case M_SOF15:		/* Differential lossless, arithmetic */
      ERREXIT1(cinfo, JERR_SOF_UNSUPPORTED, cinfo->unread_marker);
      break;

    case M_SOS:
      if (! get_sos(cinfo))
	return JPEG_SUSPENDED;
      cinfo->unread_marker = 0;	/* processed the marker */
      return JPEG_REACHED_SOS;
    
    case M_EOI:
      TRACEMS(cinfo, 1, JTRC_EOI);
      cinfo->unread_marker = 0;	/* processed the marker */
      return JPEG_REACHED_EOI;
      
    case M_DAC:
      if (! get_dac(cinfo))
	return JPEG_SUSPENDED;
      break;
      
    case M_DHT:
      if (! get_dht(cinfo))
	return JPEG_SUSPENDED;
      break;
      
    case M_DQT:
      if (! get_dqt(cinfo))
	return JPEG_SUSPENDED;
      break;
      
    case M_DRI:
      if (! get_dri(cinfo))
	return JPEG_SUSPENDED;
      break;
      
    case M_APP0:
    case M_APP1:
    case M_APP2:
    case M_APP3:
    case M_APP4:
    case M_APP5:
    case M_APP6:
    case M_APP7:
    case M_APP8:
    case M_APP9:
    case M_APP10:
    case M_APP11:
    case M_APP12:
    case M_APP13:
    case M_APP14:
    case M_APP15:
      if (! (*((my_marker_ptr) cinfo->marker.get())->process_APPn[
		cinfo->unread_marker - (int) M_APP0]) (cinfo))
	return JPEG_SUSPENDED;
      break;
      
    case M_COM:
      if (! (*((my_marker_ptr) cinfo->marker.get())->process_COM) (cinfo))
	return JPEG_SUSPENDED;
      break;

    case M_RST0:		/* these are all parameterless */
    case M_RST1:
    case M_RST2:
    case M_RST3:
    case M_RST4:
    case M_RST5:
    case M_RST6:
    case M_RST7:
    case M_TEM:
      TRACEMS1(cinfo, 1, JTRC_PARMLESS_MARKER, cinfo->unread_marker);
      break;

    case M_DNL:			/* Ignore DNL ... perhaps the wrong thing */
      if (! skip_variable(cinfo))
	return JPEG_SUSPENDED;
      break;

    default:			/* must be DHP, EXP, JPGn, or RESn */
      /* For now, we treat the reserved markers as fatal errors since they are
       * likely to be used to signal incompatible JPEG Part 3 extensions.
       * Once the JPEG 3 version-number marker is well defined, this code
       * ought to change!
       */
      ERREXIT1(cinfo, JERR_UNKNOWN_MARKER, cinfo->unread_marker);
      break;
    }
    /* Successfully processed marker, so reset state variable */
    cinfo->unread_marker = 0;
  } /* end loop */
}

/* 0285602C */
static boolean read_restart_marker (j_decompress_ptr cinfo)
{
  /* Obtain a marker unless we already did. */
  /* Note that next_marker will complain if it skips any data. */
  if (cinfo->unread_marker == 0) {
    if (! next_marker(cinfo))
      return FALSE;
  }

  if (cinfo->unread_marker ==
      ((int) M_RST0 + cinfo->marker->next_restart_num)) {
    /* Normal case --- swallow the marker and let entropy decoder continue */
    TRACEMS1(cinfo, 3, JTRC_RST, cinfo->marker->next_restart_num);
    cinfo->unread_marker = 0;
  } else {
    /* Uh-oh, the restart markers have been messed up. */
    /* Let the data source manager determine how to resync. */
    if (! (*cinfo->src->resync_to_restart) (cinfo,
					    cinfo->marker->next_restart_num))
      return FALSE;
  }

  /* Update next-restart state */
  cinfo->marker->next_restart_num = (cinfo->marker->next_restart_num + 1) & 7;

  return TRUE;
}

/* 0285611C */
boolean jpeg_resync_to_restart (j_decompress_ptr cinfo, int desired)
{
  int marker = cinfo->unread_marker;
  int action = 1;
  
  /* Always put up a warning. */
  WARNMS2(cinfo, JWRN_MUST_RESYNC, marker, desired);
  
  /* Outer loop handles repeated decision after scanning forward. */
  for (;;) {
    if (marker < (int) M_SOF0)
      action = 2;		/* invalid marker */
    else if (marker < (int) M_RST0 || marker > (int) M_RST7)
      action = 3;		/* valid non-restart marker */
    else {
      if (marker == ((int) M_RST0 + ((desired+1) & 7)) ||
	  marker == ((int) M_RST0 + ((desired+2) & 7)))
	action = 3;		/* one of the next two expected restarts */
      else if (marker == ((int) M_RST0 + ((desired-1) & 7)) ||
	       marker == ((int) M_RST0 + ((desired-2) & 7)))
	action = 2;		/* a prior restart, so advance */
      else
	action = 1;		/* desired restart or too far away */
    }
    TRACEMS2(cinfo, 4, JTRC_RECOVERY_ACTION, marker, action);
    switch (action) {
    case 1:
      /* Discard marker and let entropy decoder resume processing. */
      cinfo->unread_marker = 0;
      return TRUE;
    case 2:
      /* Scan to the next marker, and repeat the decision loop. */
      if (! next_marker(cinfo))
	return FALSE;
      marker = cinfo->unread_marker;
      break;
    case 3:
      /* Return without advancing past this marker. */
      /* Entropy decoder will be forced to process an empty segment. */
      return TRUE;
    }
  } /* end loop */
}

/* 028562E0 */
static void reset_marker_reader (j_decompress_ptr cinfo)
{
  my_marker_ptr marker = (my_marker_ptr) cinfo->marker.get();

  cinfo->comp_info = NULL;		/* until allocated by get_sof */
  cinfo->input_scan_number = 0;		/* no SOS seen yet */
  cinfo->unread_marker = 0;		/* no pending marker */
  marker->pub.saw_SOI = FALSE;		/* set internal state too */
  marker->pub.saw_SOF = FALSE;
  marker->pub.discarded_bytes = 0;
  marker->cur_marker = NULL;
}

/* 02856308 */
void jinit_marker_reader (j_decompress_ptr cinfo)
{
  my_marker_ptr marker;
  int i;

  /* Create subobject in permanent pool */
  marker = (my_marker_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				SIZEOF(my_marker_reader));
  cinfo->marker = (struct jpeg_marker_reader *) marker;
  /* Initialize public method pointers */
  marker->pub.reset_marker_reader = reset_marker_reader;
  marker->pub.read_markers = read_markers;
  marker->pub.read_restart_marker = read_restart_marker;
  /* Initialize COM/APPn processing.
   * By default, we examine and then discard APP0 and APP14,
   * but simply discard COM and all other APPn.
   */
  marker->process_COM = skip_variable;
  marker->length_limit_COM = 0;
  for (i = 0; i < 16; i++) {
    marker->process_APPn[i] = skip_variable;
    marker->length_limit_APPn[i] = 0;
  }
  marker->process_APPn[0] = get_interesting_appn;
  marker->process_APPn[14] = get_interesting_appn;
  /* Reset marker processing state */
  reset_marker_reader(cinfo);
}
/* WWHD glue
ext C000A848 void* marker_copy(void* dest,const void* source,u32 size)
ext 02850620 JHUFF_TBL* jpeg_alloc_huff_table(j_common_ptr cinfo)
ext 028505E4 JQUANT_TBL* jpeg_alloc_quant_table(j_common_ptr cinfo)
data jpeg_natural_order 1017B7A8
data jpeg_natural_order7 1017B8E8
data jpeg_natural_order6 1017B9EC
data jpeg_natural_order5 1017BABC
data jpeg_natural_order4 1017BB60
data jpeg_natural_order3 1017BBE0
data jpeg_natural_order2 1017BC44
*/
