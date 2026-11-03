/* jdarith.c (decompiled WWHD build)
 * Developed 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#include "libjpeg/jdarith.h"
static const s32 jpeg_aritab[114] = {
    0x5A1D0181, 0x2586020E, 0x11140310, 0x080B0412, 0x03D80514, 0x01DA0617,
    0x00E50719, 0x006F081C, 0x0036091E, 0x001A0A21, 0x000D0B23, 0x00060C09,
    0x00030D0A, 0x00010D0C, 0x5A7F0F8F, 0x3F251024, 0x2CF21126, 0x207C1227,
    0x17B91328, 0x1182142A, 0x0CEF152B, 0x09A1162D, 0x072F172E, 0x055C1830,
    0x04061931, 0x03031A33, 0x02401B34, 0x01B11C36, 0x01441D38, 0x00F51E39,
    0x00B71F3B, 0x008A203C, 0x0068213E, 0x004E223F, 0x003B2320, 0x002C0921,
    0x5AE125A5, 0x484C2640, 0x3A0D2741, 0x2EF12843, 0x261F2944, 0x1F332A45,
    0x19A82B46, 0x15182C48, 0x11772D49, 0x0E742E4A, 0x0BFB2F4B, 0x09F8304D,
    0x0861314E, 0x0706324F, 0x05CD3330, 0x04DE3432, 0x040F3532, 0x03633633,
    0x02D43734, 0x025C3835, 0x01F83936, 0x01A43A37, 0x01603B38, 0x01253C39,
    0x00F63D3A, 0x00CB3E3B, 0x00AB3F3D, 0x008F203D, 0x5B1241C1, 0x4D044250,
    0x412C4351, 0x37D84452, 0x2FE84553, 0x293C4654, 0x23794756, 0x1EDF4857,
    0x1AA94957, 0x174E4A48, 0x14244B48, 0x119C4C4A, 0x0F6B4D4A, 0x0D514E4B,
    0x0BB64F4D, 0x0A40304D, 0x583251D0, 0x4D1C5258, 0x438E5359, 0x3BDD545A,
    0x34EE555B, 0x2EAE565C, 0x299A575D, 0x25164756, 0x557059D8, 0x4CA95A5F,
    0x44D95B60, 0x3E225C61, 0x38245D63, 0x32B45E63, 0x2E17565D, 0x56A860DF,
    0x4F466165, 0x47E56266, 0x41CF6367, 0x3C3D6468, 0x375E5D63, 0x52316669,
    0x4C0F676A, 0x4639686B, 0x415E6367, 0x56276AE9, 0x50E76B6C, 0x4B85676D,
    0x55976D6E, 0x504F6B6F, 0x5A106FEE, 0x55226D70, 0x59EB6FF0, 0x5A1D7171
};
/*
 * jdarith.cpp (decompiled WWHD build)
 *
 * Developed 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 *
 * This file contains portable arithmetic entropy decoding routines for JPEG
 * (implementing the ISO/IEC IS 10918-1 and CCITT Recommendation ITU-T T.81).
 *
 * Both sequential and progressive modes are supported in this single module.
 *
 * Suspension is not currently supported in this module.
 */
/* Expanded entropy decoder object for arithmetic decoding. */
/* The following two definitions specify the allocation chunk size
 * for the statistics area.
 * According to sections F.1.4.4.1.3 and F.1.4.4.2, we need at least
 * 49 statistics bins for DC, and 245 statistics bins for AC coding.
 *
 * We use a compact representation with 1 byte per statistics bin,
 * thus the numbers directly represent byte sizes.
 * This 1 byte per statistics bin contains the meaning of the MPS
 * (more probable symbol) in the highest bit (mask 0x80), and the
 * index into the probability estimation state machine table
 * in the lower bits (mask 0x7F).
 */
/* 02861084 */
static int
get_byte (j_decompress_ptr cinfo)
/* Read next input byte; we do not support suspension in this module. */
{
  struct jpeg_source_mgr * src = cinfo->src;
  if (src->bytes_in_buffer == 0)
    if (! (*src->fill_input_buffer) (cinfo))
      jpeg_error(cinfo, JERR_CANT_SUSPEND);
  src->bytes_in_buffer--;
  int result = int(*src->next_input_byte);
  src->next_input_byte = src->next_input_byte.get() + 1;
  return result;
}
/*
 * The core arithmetic decoding routine (common in JPEG and JBIG).
 * This needs to go as fast as possible.
 * Machine-dependent optimization facilities
 * are not utilized in this portable implementation.
 * However, this code should be fairly efficient and
 * may be a good base for further optimizations anyway.
 *
 * Return value is 0 or 1 (binary decision).
 *
 * Note: I've changed the handling of the code base & bit
 * buffer C compared to other implementations
 * based on the standards layout & procedures.
 * While it also contains both the actual base of the
 * coding interval (16 bits) and the next-bits buffer,
 * the cut-point between these two parts is floating
 * (instead of fixed) with the bit shift counter CT.
 * Thus, we also need only one (variable instead of
 * fixed size) shift for the LPS/MPS decision, and
 * we can get away with any renormalization update
 * of C (except for new data insertion, of course).
 *
 * I've also introduced a new scheme for accessing
 * the probability estimation state machine table,
 * derived from Markus Kuhn's JBIG implementation.
 */
/* 02861114 */
static int
arith_decode (j_decompress_ptr cinfo, be<u8>*st)
{
  arith_entropy_decoder * e = (arith_entropy_decoder *) cinfo->entropy.get();
  unsigned char nl, nm;
  s32 qe, temp;
  int sv, data;
  /* Renormalization & data input per section D.2.6 */
  while (e->a < 0x8000L) {
    if (--e->ct < 0) {
      /* Need to fetch next data byte */
      if (cinfo->unread_marker)
 data = 0; /* stuff zero data */
      else {
 data = get_byte(cinfo); /* read next input byte */
 if (data == 0xFF) { /* zero stuff or marker code */
   do data = get_byte(cinfo);
   while (data == 0xFF); /* swallow extra 0xFF bytes */
   if (data == 0)
     data = 0xFF; /* discard stuffed zero byte */
   else {
     /* Note: Different from the Huffman decoder, hitting
	     * a marker while processing the compressed data
	     * segment is legal in arithmetic coding.
	     * The convention is to supply zero data
	     * then until decoding is complete.
	     */
     cinfo->unread_marker = data;
     data = 0;
   }
 }
      }
      e->c = (e->c << 8) | data; /* insert data into C */
      if ((e->ct += 8) < 0) /* update bit shift counter */
 /* Need more initial bytes */
 if (++e->ct == 0)
   /* Got 2 initial bytes -> re-init A and exit loop */
   e->a = 0x8000L; /* => e->a = 0x10000L after loop exit */
    }
    e->a <<= 1;
  }
  /* Fetch values from our compact representation of Table D.2:
   * Qe values and probability estimation state machine
   */
  sv = *st;
  qe = jpeg_aritab[sv & 0x7F]; /* => Qe_Value */
  nl = qe & 0xFF; qe >>= 8; /* Next_Index_LPS + Switch_MPS */
  nm = qe & 0xFF; qe >>= 8; /* Next_Index_MPS */
  /* Decode & estimation procedures per sections D.2.4 & D.2.5 */
  temp = e->a - qe;
  e->a = temp;
  const u32 interval_shift = (u32)e->ct & 63;
  temp = interval_shift < 32 ? (s32)((u32)temp << interval_shift) : 0;
  if (e->c >= temp) {
    e->c -= temp;
    /* Conditional LPS (less probable symbol) exchange */
    if (e->a < qe) {
      e->a = qe;
      *st = (sv & 0x80) ^ nm; /* Estimate_after_MPS */
    } else {
      e->a = qe;
      *st = (sv & 0x80) ^ nl; /* Estimate_after_LPS */
      sv ^= 0x80; /* Exchange LPS/MPS */
    }
  } else if (e->a < 0x8000L) {
    /* Conditional MPS (more probable symbol) exchange */
    if (e->a < qe) {
      *st = (sv & 0x80) ^ nl; /* Estimate_after_LPS */
      sv ^= 0x80; /* Exchange LPS/MPS */
    } else {
      *st = (sv & 0x80) ^ nm; /* Estimate_after_MPS */
    }
  }
  return sv >> 7;
}
/*
 * Check for a restart marker & resynchronize decoder.
 */
/* 028612D0 */
static void
process_restart (j_decompress_ptr cinfo)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  int ci;
  jpeg_component_info * compptr;
  /* Advance past the RSTn marker */
  if (! (*cinfo->marker->read_restart_marker) (cinfo))
    jpeg_error(cinfo, JERR_CANT_SUSPEND);
  /* Re-initialize statistics areas */
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    if (! cinfo->progressive_mode || (cinfo->Ss == 0 && cinfo->Ah == 0)) {
      arith_zero(entropy->dc_stats[compptr->dc_tbl_no], 64);
      /* Reset DC predictions to 0 */
      entropy->last_dc_val[ci] = 0;
      entropy->dc_context[ci] = 0;
    }
    if ((! cinfo->progressive_mode && cinfo->lim_Se) ||
 (cinfo->progressive_mode && cinfo->Ss)) {
      arith_zero(entropy->ac_stats[compptr->ac_tbl_no], 256);
    }
  }
  /* Reset arithmetic decoding variables */
  entropy->c = 0;
  entropy->a = 0;
  entropy->ct = -16; /* force reading 2 initial bytes to fill C */
  /* Reset restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;
}
/*
 * Arithmetic MCU decoding.
 * Each of these routines decodes and returns one MCU's worth of
 * arithmetic-compressed coefficients.
 * The coefficients are reordered from zigzag order into natural array order,
 * but are not dequantized.
 *
 * The i'th block of the MCU is stored into the block pointed to by
 * MCU_data[i].  WE ASSUME THIS AREA IS INITIALLY ZEROED BY THE CALLER.
 */
/*
 * MCU decoding for DC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 02861408 */
static boolean
decode_mcu_DC_first (j_decompress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<u8>*st;
  int blkn, ci, tbl, sign;
  int v, m;
  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }
  if (entropy->ct == -1) return 1; /* if error do nothing */
  /* Outer loop handles each block in the MCU */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    tbl = cinfo->cur_comp_info[ci]->dc_tbl_no;
    /* Sections F.2.4.1 & F.1.4.4.1: Decoding of DC coefficients */
    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];
    /* Figure F.19: Decode_DC_DIFF */
    if (arith_decode(cinfo, st) == 0)
      entropy->dc_context[ci] = 0;
    else {
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      sign = arith_decode(cinfo, st + 1);
      st += 2; st += sign;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
 st = entropy->dc_stats[tbl] + 20; /* Table F.4: X1 = 20 */
 while (arith_decode(cinfo, st)) {
   if ((m <<= 1) == 0x8000) {
     arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
     entropy->ct = -1; /* magnitude overflow */
     return 1;
   }
   st += 1;
 }
      }
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) ((1L << cinfo->arith_dc_L[tbl]) >> 1))
 entropy->dc_context[ci] = 0; /* zero diff category */
      else if (m > (int) ((1L << cinfo->arith_dc_U[tbl]) >> 1))
 entropy->dc_context[ci] = 12 + (sign * 4); /* large diff category */
      else
 entropy->dc_context[ci] = 4 + (sign * 4); /* small diff category */
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      entropy->last_dc_val[ci] += v;
    }
    /* Scale and output the DC coefficient (assumes jpeg_natural_order[0]=0) */
    (*block)[0] = (JCOEF) (entropy->last_dc_val[ci] << cinfo->Al);
  }
  return 1;
}
/*
 * MCU decoding for AC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 0286165C */
static boolean
decode_mcu_AC_first (j_decompress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<u8>*st;
  int tbl, sign, k;
  int v, m;
  const be<s32>* natural_order;
  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }
  if (entropy->ct == -1) return 1; /* if error do nothing */
  natural_order = cinfo->natural_order;
  /* There is always only one block per MCU */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;
  /* Sections F.2.4.2 & F.1.4.4.2: Decoding of AC coefficients */
  /* Figure F.20: Decode_AC_coefficients */
  for (k = cinfo->Ss; k <= cinfo->Se; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    if (arith_decode(cinfo, st)) break; /* EOB flag */
    while (arith_decode(cinfo, st + 1) == 0) {
      st += 3; k++;
      if (k > cinfo->Se) {
 arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
 entropy->ct = -1; /* spectral overflow */
 return 1;
      }
    }
    /* Figure F.21: Decoding nonzero value v */
    /* Figure F.22: Decoding the sign of v */
    sign = arith_decode(cinfo, entropy->fixed_bin);
    st += 2;
    /* Figure F.23: Decoding the magnitude category of v */
    if ((m = arith_decode(cinfo, st)) != 0) {
      if (arith_decode(cinfo, st)) {
 m <<= 1;
 st = entropy->ac_stats[tbl] +
      (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
 while (arith_decode(cinfo, st)) {
   if ((m <<= 1) == 0x8000) {
     arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
     entropy->ct = -1; /* magnitude overflow */
     return 1;
   }
   st += 1;
 }
      }
    }
    v = m;
    /* Figure F.24: Decoding the magnitude bit pattern of v */
    st += 14;
    while (m >>= 1)
      if (arith_decode(cinfo, st)) v |= m;
    v += 1; if (sign) v = -v;
    /* Scale and output coefficient in natural (dezigzagged) order */
    (*block)[natural_order[k]] = (JCOEF) (v << cinfo->Al);
  }
  return 1;
}
/*
 * MCU decoding for DC successive approximation refinement scan.
 */
/* 02861870 */
static boolean
decode_mcu_DC_refine (j_decompress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  be<u8>*st;
  int p1, blkn;
  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }
  st = entropy->fixed_bin; /* use fixed probability estimation */
  p1 = 1 << cinfo->Al; /* 1 in the bit position being coded */
  /* Outer loop handles each block in the MCU */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    /* Encoded data is simply the next bit of the two's-complement DC value */
    if (arith_decode(cinfo, st))
      MCU_data[blkn][0][0] |= p1;
  }
  return 1;
}
/*
 * MCU decoding for AC successive approximation refinement scan.
 */
/* 02861928 */
static boolean
decode_mcu_AC_refine (j_decompress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<JCOEF>* thiscoef;
  be<u8>*st;
  int tbl, k, kex;
  int p1, m1;
  const be<s32>* natural_order;
  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }
  if (entropy->ct == -1) return 1; /* if error do nothing */
  natural_order = cinfo->natural_order;
  /* There is always only one block per MCU */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;
  p1 = 1 << cinfo->Al; /* 1 in the bit position being coded */
  m1 = (-1) << cinfo->Al; /* -1 in the bit position being coded */
  /* Establish EOBx (previous stage end-of-block) index */
  for (kex = cinfo->Se; kex > 0; kex--)
    if ((*block)[natural_order[kex]]) break;
  for (k = cinfo->Ss; k <= cinfo->Se; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    if (k > kex)
      if (arith_decode(cinfo, st)) break; /* EOB flag */
    for (;;) {
      thiscoef = *block + natural_order[k];
      if (*thiscoef) { /* previously nonzero coef */
 if (arith_decode(cinfo, st + 2)) {
   if (*thiscoef < 0)
     *thiscoef += m1;
   else
     *thiscoef += p1;
 }
 break;
      }
      if (arith_decode(cinfo, st + 1)) { /* newly nonzero coef */
 if (arith_decode(cinfo, entropy->fixed_bin))
   *thiscoef = m1;
 else
   *thiscoef = p1;
 break;
      }
      st += 3; k++;
      if (k > cinfo->Se) {
 arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
 entropy->ct = -1; /* spectral overflow */
 return 1;
      }
    }
  }
  return 1;
}
/*
 * Decode one MCU's worth of arithmetic-compressed coefficients.
 */
/* 02861B14 */
static boolean
decode_mcu (j_decompress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  jpeg_component_info * compptr;
  JBLOCKROW block;
  be<u8>*st;
  int blkn, ci, tbl, sign, k;
  int v, m;
  const be<s32>* natural_order;
  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }
  if (entropy->ct == -1) return 1; /* if error do nothing */
  natural_order = cinfo->natural_order;
  /* Outer loop handles each block in the MCU */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];
    /* Sections F.2.4.1 & F.1.4.4.1: Decoding of DC coefficients */
    tbl = compptr->dc_tbl_no;
    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];
    /* Figure F.19: Decode_DC_DIFF */
    if (arith_decode(cinfo, st) == 0)
      entropy->dc_context[ci] = 0;
    else {
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      sign = arith_decode(cinfo, st + 1);
      st += 2; st += sign;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
 st = entropy->dc_stats[tbl] + 20; /* Table F.4: X1 = 20 */
 while (arith_decode(cinfo, st)) {
   if ((m <<= 1) == 0x8000) {
     arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
     entropy->ct = -1; /* magnitude overflow */
     return 1;
   }
   st += 1;
 }
      }
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) ((1L << cinfo->arith_dc_L[tbl]) >> 1))
 entropy->dc_context[ci] = 0; /* zero diff category */
      else if (m > (int) ((1L << cinfo->arith_dc_U[tbl]) >> 1))
 entropy->dc_context[ci] = 12 + (sign * 4); /* large diff category */
      else
 entropy->dc_context[ci] = 4 + (sign * 4); /* small diff category */
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      entropy->last_dc_val[ci] += v;
    }
    (*block)[0] = (JCOEF) entropy->last_dc_val[ci];
    /* Sections F.2.4.2 & F.1.4.4.2: Decoding of AC coefficients */
    tbl = compptr->ac_tbl_no;
    /* Figure F.20: Decode_AC_coefficients */
    for (k = 1; k <= cinfo->lim_Se; k++) {
      st = entropy->ac_stats[tbl] + 3 * (k - 1);
      if (arith_decode(cinfo, st)) break; /* EOB flag */
      while (arith_decode(cinfo, st + 1) == 0) {
 st += 3; k++;
 if (k > cinfo->lim_Se) {
   arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
   entropy->ct = -1; /* spectral overflow */
   return 1;
 }
      }
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      sign = arith_decode(cinfo, entropy->fixed_bin);
      st += 2;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
 if (arith_decode(cinfo, st)) {
   m <<= 1;
   st = entropy->ac_stats[tbl] +
        (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
   while (arith_decode(cinfo, st)) {
     if ((m <<= 1) == 0x8000) {
       arith_warn(cinfo, JWRN_ARITH_BAD_CODE);
       entropy->ct = -1; /* magnitude overflow */
       return 1;
     }
     st += 1;
   }
 }
      }
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      (*block)[natural_order[k]] = (JCOEF) v;
    }
  }
  return 1;
}
/*
 * Initialize for an arithmetic-compressed scan.
 */
/* 02861EC8 */
static void
start_pass (j_decompress_ptr cinfo)
{
  arith_entropy_decoder * entropy = (arith_entropy_decoder *) cinfo->entropy.get();
  int ci, tbl;
  jpeg_component_info * compptr;
  if (cinfo->progressive_mode) {
    /* Validate progressive scan parameters before installing a decoding method. */
    bool invalid_scan;
    if (cinfo->Ss == 0) {
      invalid_scan = cinfo->Se != 0;
    } else {
      invalid_scan = cinfo->Se < cinfo->Ss || cinfo->Se > cinfo->lim_Se ||
                     cinfo->comps_in_scan != 1;
    }
    if (invalid_scan ||
        (cinfo->Ah != 0 && cinfo->Ah - 1 != cinfo->Al) || cinfo->Al > 13) {
      arith_error4(cinfo, JERR_BAD_PROGRESSION,
                   cinfo->Ss, cinfo->Se, cinfo->Ah, cinfo->Al);
    }
    /* Update progression status, and verify that scan order is legal.
     * Note that inter-scan inconsistencies are treated as warnings
     * not fatal errors ... not clear if this is right way to behave.
     */
    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      int coefi, cindex = cinfo->cur_comp_info[ci]->component_index;
      be<s32>* coef_bit_ptr = cinfo->coef_bits.get() + cindex * DCTSIZE2;
      if (cinfo->Ss && coef_bit_ptr[0] < 0) /* AC without prior DC scan */
 arith_warn2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, 0);
      for (coefi = cinfo->Ss; coefi <= cinfo->Se; coefi++) {
 int expected = (coef_bit_ptr[coefi] < 0) ? 0 : coef_bit_ptr[coefi];
 if (cinfo->Ah != expected)
   arith_warn2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, coefi);
 coef_bit_ptr[coefi] = cinfo->Al;
      }
    }
    /* Select MCU decoding routine */
    if (cinfo->Ah == 0) {
      if (cinfo->Ss == 0)
 entropy->pub.decode_mcu = decode_mcu_DC_first;
      else
 entropy->pub.decode_mcu = decode_mcu_AC_first;
    } else {
      if (cinfo->Ss == 0)
 entropy->pub.decode_mcu = decode_mcu_DC_refine;
      else
 entropy->pub.decode_mcu = decode_mcu_AC_refine;
    }
  } else {
    /* Check that the scan parameters Ss, Se, Ah/Al are OK for sequential JPEG.
     * This ought to be an error condition, but we make it a warning.
     */
    if (cinfo->Ss != 0 || cinfo->Ah != 0 || cinfo->Al != 0 ||
 (cinfo->Se < DCTSIZE2 && cinfo->Se != cinfo->lim_Se))
      arith_warn(cinfo, JWRN_NOT_SEQUENTIAL);
    /* Select MCU decoding routine */
    entropy->pub.decode_mcu = decode_mcu;
  }
  /* Allocate & initialize requested statistics areas */
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    if (! cinfo->progressive_mode || (cinfo->Ss == 0 && cinfo->Ah == 0)) {
      tbl = compptr->dc_tbl_no;
      if (tbl < 0 || tbl >= NUM_ARITH_TBLS)
 jpeg_error1(cinfo, JERR_NO_ARITH_TABLE, tbl);
      if (entropy->dc_stats[tbl] == nullptr)
 entropy->dc_stats[tbl] = (be<u8>*) (*cinfo->mem->alloc_small)
   ((j_common_ptr) cinfo, JPOOL_IMAGE, 64);
      arith_zero(entropy->dc_stats[tbl], 64);
      /* Initialize DC predictions to 0 */
      entropy->last_dc_val[ci] = 0;
      entropy->dc_context[ci] = 0;
    }
    if ((! cinfo->progressive_mode && cinfo->lim_Se) ||
 (cinfo->progressive_mode && cinfo->Ss)) {
      tbl = compptr->ac_tbl_no;
      if (tbl < 0 || tbl >= NUM_ARITH_TBLS)
 jpeg_error1(cinfo, JERR_NO_ARITH_TABLE, tbl);
      if (entropy->ac_stats[tbl] == nullptr)
 entropy->ac_stats[tbl] = (be<u8>*) (*cinfo->mem->alloc_small)
   ((j_common_ptr) cinfo, JPOOL_IMAGE, 256);
      arith_zero(entropy->ac_stats[tbl], 256);
    }
  }
  /* Initialize arithmetic decoding variables */
  entropy->c = 0;
  entropy->a = 0;
  entropy->ct = -16; /* force reading 2 initial bytes to fill C */
  /* Initialize restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;
}
/*
 * Module initialization routine for arithmetic entropy decoding.
 */
/* 028622D0 */
void
jinit_arith_decoder (j_decompress_ptr cinfo)
{
  arith_entropy_decoder * entropy;
  int i;
  entropy = (arith_entropy_decoder *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
    sizeof(arith_entropy_decoder));
  cinfo->entropy = (struct jpeg_entropy_decoder *) entropy;
  entropy->pub.start_pass = start_pass;
  /* Mark tables unallocated */
  for (i = 0; i < NUM_ARITH_TBLS; i++) {
    entropy->dc_stats[i] = nullptr;
    entropy->ac_stats[i] = nullptr;
  }
  /* Initialize index for fixed probability estimation */
  entropy->fixed_bin[0] = 113;
  if (cinfo->progressive_mode) {
    /* Create progression status table */
    be<s32>* coef_bit_ptr;
    int ci;
    cinfo->coef_bits = (be<s32>*)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
      cinfo->num_components*DCTSIZE2*sizeof(int));
    coef_bit_ptr = cinfo->coef_bits.get();
    for (ci = 0; ci < cinfo->num_components; ci++)
      for (i = 0; i < DCTSIZE2; i++)
 *coef_bit_ptr++ = -1;
  }
}

/* WWHD glue
data jpeg_aritab 1017DB14
*/
