/* jcarith.c (decompiled WWHD build)
 * Developed 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#include "libjpeg/jcarith.h"
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
 * jcarith.cpp (decompiled WWHD build)
 *
 * Developed 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 *
 * This file contains portable arithmetic entropy encoding routines for JPEG
 * (implementing the ISO/IEC IS 10918-1 and CCITT Recommendation ITU-T T.81).
 *
 * Both sequential and progressive modes are supported in this single module.
 *
 * Suspension is not currently supported in this module.
 */
/* Expanded entropy encoder object for arithmetic encoding. */
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
/* NOTE: Uncomment the following #define if you want to use the
 * given formula for calculating the AC conditioning parameter Kx
 * for spectral selection progressive coding in section G.1.3.2
 * of the spec (Kx = Kmin + SRL (8 + Se - Kmin) 4).
 * Although the spec and P&M authors claim that this "has proven
 * to give good results for 8 bit precision samples", I'm not
 * convinced yet that this is really beneficial.
 * Early tests gave only very marginal compression enhancements
 * (a few - around 5 or so - bytes even for very large files),
 * which would turn out rather negative if we'd suppress the
 * DAC (Define Arithmetic Conditioning) marker segments for
 * the default parameters in the future.
 * Note that currently the marker writing module emits 12-byte
 * DAC segments for a full-component scan in a color image.
 * This is not worth worrying about IMHO. However, since the
 * spec defines the default values to be used if the tables
 * are omitted (unlike Huffman tables, which are required
 * anyway), one might optimize this behaviour in the future,
 * and then it would be disadvantageous to use custom tables if
 * they don't provide sufficient gain to exceed the DAC size.
 *
 * On the other hand, I'd consider it as a reasonable result
 * that the conditioning has no significant influence on the
 * compression performance. This means that the basic
 * statistical model is already rather stable.
 *
 * Thus, at the moment, we use the default conditioning values
 * anyway, and do not use the custom formula.
 *
#define CALCULATE_SPECTRAL_CONDITIONING
 */
/* IRIGHT_SHIFT is like RIGHT_SHIFT, but works on int rather than long.
 * We assume that int right shift is unsigned if long right shift is,
 * which should be safe.
 */
/* 0285B978 */
static void
emit_byte (int val, j_compress_ptr cinfo)
/* Write next output byte; we do not support suspension in this module. */
{
  struct jpeg_destination_mgr * dest = cinfo->dest;
  *dest->next_output_byte = (JOCTET) val;
  dest->next_output_byte = dest->next_output_byte.get() + 1;
  if (--dest->free_in_buffer == 0)
    if (! (*dest->empty_output_buffer) (cinfo))
      jpeg_error(cinfo, JERR_CANT_SUSPEND);
}
/*
 * Finish up at the end of an arithmetic-compressed scan.
 */
/* 0285B9F8 */
static void
finish_pass (j_compress_ptr cinfo)
{
  arith_entropy_encoder * e = (arith_entropy_encoder *) cinfo->entropy.get();
  s32 temp;
  /* Section D.1.8: Termination of encoding */
  /* Find the e->c in the coding interval with the largest
   * number of trailing zero bits */
  if ((temp = (e->a - 1 + e->c) & 0xFFFF0000L) < e->c)
    e->c = temp + 0x8000L;
  else
    e->c = temp;
  /* Send remaining bytes to output */
  e->c <<= e->ct;
  if (e->c & 0xF8000000L) {
    /* One final overflow has to be handled */
    if (e->buffer >= 0) {
      if (e->zc)
 do emit_byte(0x00, cinfo);
 while (--e->zc);
      emit_byte(e->buffer + 1, cinfo);
      if (e->buffer + 1 == 0xFF)
 emit_byte(0x00, cinfo);
    }
    e->zc += e->sc; /* carry-over converts stacked 0xFF bytes to 0x00 */
    e->sc = 0;
  } else {
    if (e->buffer == 0)
      ++e->zc;
    else if (e->buffer >= 0) {
      if (e->zc)
 do emit_byte(0x00, cinfo);
 while (--e->zc);
      emit_byte(e->buffer, cinfo);
    }
    if (e->sc) {
      if (e->zc)
 do emit_byte(0x00, cinfo);
 while (--e->zc);
      do {
 emit_byte(0xFF, cinfo);
 emit_byte(0x00, cinfo);
      } while (--e->sc);
    }
  }
  /* Output final bytes only if they are not 0x00 */
  if (e->c & 0x7FFF800L) {
    if (e->zc) /* output final pending zero bytes */
      do emit_byte(0x00, cinfo);
      while (--e->zc);
    emit_byte((e->c >> 19) & 0xFF, cinfo);
    if (((e->c >> 19) & 0xFF) == 0xFF)
      emit_byte(0x00, cinfo);
    if (e->c & 0x7F800L) {
      emit_byte((e->c >> 11) & 0xFF, cinfo);
      if (((e->c >> 11) & 0xFF) == 0xFF)
 emit_byte(0x00, cinfo);
    }
  }
}
/*
 * The core arithmetic encoding routine (common in JPEG and JBIG).
 * This needs to go as fast as possible.
 * Machine-dependent optimization facilities
 * are not utilized in this portable implementation.
 * However, this code should be fairly efficient and
 * may be a good base for further optimizations anyway.
 *
 * Parameter 'val' to be encoded may be 0 or 1 (binary decision).
 *
 * Note: I've added full "Pacman" termination support to the
 * byte output routines, which is equivalent to the optional
 * Discard_final_zeros procedure (Figure D.15) in the spec.
 * Thus, we always produce the shortest possible output
 * stream compliant to the spec (no trailing zero bytes,
 * except for FF stuffing).
 *
 * I've also introduced a new scheme for accessing
 * the probability estimation state machine table,
 * derived from Markus Kuhn's JBIG implementation.
 */
/* 0285BC54 */
static void
arith_encode (j_compress_ptr cinfo, be<u8>*st, int val)
{
  arith_entropy_encoder * e = (arith_entropy_encoder *) cinfo->entropy.get();
  unsigned char nl, nm;
  s32 qe, temp;
  int sv;
  /* Fetch values from our compact representation of Table D.2:
   * Qe values and probability estimation state machine
   */
  sv = *st;
  qe = jpeg_aritab[sv & 0x7F]; /* => Qe_Value */
  nl = qe & 0xFF; qe >>= 8; /* Next_Index_LPS + Switch_MPS */
  nm = qe & 0xFF; qe >>= 8; /* Next_Index_MPS */
  /* Encode & estimation procedures per sections D.1.4 & D.1.5 */
  e->a -= qe;
  if (val != (sv >> 7)) {
    /* Encode the less probable symbol */
    if (e->a >= qe) {
      /* If the interval size (qe) for the less probable symbol (LPS)
       * is larger than the interval size for the MPS, then exchange
       * the two symbols for coding efficiency, otherwise code the LPS
       * as usual: */
      e->c += e->a;
      e->a = qe;
    }
    *st = (sv & 0x80) ^ nl; /* Estimate_after_LPS */
  } else {
    /* Encode the more probable symbol */
    if (e->a >= 0x8000L)
      return; /* A >= 0x8000 -> ready, no renormalization required */
    if (e->a < qe) {
      /* If the interval size (qe) for the less probable symbol (LPS)
       * is larger than the interval size for the MPS, then exchange
       * the two symbols for coding efficiency: */
      e->c += e->a;
      e->a = qe;
    }
    *st = (sv & 0x80) ^ nm; /* Estimate_after_MPS */
  }
  /* Renormalization & data output per section D.1.6 */
  do {
    e->a <<= 1;
    e->c <<= 1;
    if (--e->ct == 0) {
      /* Another byte is ready for output */
      temp = e->c >> 19;
      if (temp > 0xFF) {
 /* Handle overflow over all stacked 0xFF bytes */
 if (e->buffer >= 0) {
   if (e->zc)
     do emit_byte(0x00, cinfo);
     while (--e->zc);
   emit_byte(e->buffer + 1, cinfo);
   if (e->buffer + 1 == 0xFF)
     emit_byte(0x00, cinfo);
 }
 e->zc += e->sc; /* carry-over converts stacked 0xFF bytes to 0x00 */
 e->sc = 0;
 /* Note: The 3 spacer bits in the C guarantee
	 * that the new buffer byte can't be 0xFF here
	 * (see page 160 in the P&M JPEG book). */
 e->buffer = temp & 0xFF; /* new output byte, might overflow later */
      } else if (temp == 0xFF) {
 ++e->sc; /* stack 0xFF byte (which might overflow later) */
      } else {
 /* Output all stacked 0xFF bytes, they will not overflow any more */
 if (e->buffer == 0)
   ++e->zc;
 else if (e->buffer >= 0) {
   if (e->zc)
     do emit_byte(0x00, cinfo);
     while (--e->zc);
   emit_byte(e->buffer, cinfo);
 }
 if (e->sc) {
   if (e->zc)
     do emit_byte(0x00, cinfo);
     while (--e->zc);
   do {
     emit_byte(0xFF, cinfo);
     emit_byte(0x00, cinfo);
   } while (--e->sc);
 }
 e->buffer = temp & 0xFF; /* new output byte (can still overflow) */
      }
      e->c &= 0x7FFFFL;
      e->ct += 8;
    }
  } while (e->a < 0x8000L);
}
/*
 * Emit a restart marker & resynchronize predictions.
 */
/* 0285BF14 */
static void
emit_restart (j_compress_ptr cinfo, int restart_num)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  int ci;
  jpeg_component_info * compptr;
  finish_pass(cinfo);
  emit_byte(0xFF, cinfo);
  emit_byte(0xD0 + restart_num, cinfo);
  /* Re-initialize statistics areas */
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* DC needs no table for refinement scan */
    if (cinfo->Ss == 0 && cinfo->Ah == 0) {
      arith_zero(entropy->dc_stats[compptr->dc_tbl_no], 64);
      /* Reset DC predictions to 0 */
      entropy->last_dc_val[ci] = 0;
      entropy->dc_context[ci] = 0;
    }
    /* AC needs no table when not present */
    if (cinfo->Se) {
      arith_zero(entropy->ac_stats[compptr->ac_tbl_no], 256);
    }
  }
  /* Reset arithmetic encoding variables */
  entropy->c = 0;
  entropy->a = 0x10000L;
  entropy->sc = 0;
  entropy->zc = 0;
  entropy->ct = 11;
  entropy->buffer = -1; /* empty */
}
/*
 * MCU encoding for DC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 0285C01C */
static boolean
encode_mcu_DC_first (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<u8>*st;
  int blkn, ci, tbl;
  int v, v2, m;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      emit_restart(cinfo, entropy->next_restart_num);
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    tbl = cinfo->cur_comp_info[ci]->dc_tbl_no;
    /* Compute the DC value after the required point transform by Al.
     * This is simply an arithmetic right shift.
     */
    m = (((int) ((*block)[0])) >> (cinfo->Al));
    /* Sections F.1.4.1 & F.1.4.4.1: Encoding of DC coefficients */
    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];
    /* Figure F.4: Encode_DC_DIFF */
    if ((v = m - entropy->last_dc_val[ci]) == 0) {
      arith_encode(cinfo, st, 0);
      entropy->dc_context[ci] = 0; /* zero diff category */
    } else {
      entropy->last_dc_val[ci] = m;
      arith_encode(cinfo, st, 1);
      /* Figure F.6: Encoding nonzero value v */
      /* Figure F.7: Encoding the sign of v */
      if (v > 0) {
 arith_encode(cinfo, st + 1, 0); /* Table F.4: SS = S0 + 1 */
 st += 2; /* Table F.4: SP = S0 + 2 */
 entropy->dc_context[ci] = 4; /* small positive diff category */
      } else {
 v = -v;
 arith_encode(cinfo, st + 1, 1); /* Table F.4: SS = S0 + 1 */
 st += 3; /* Table F.4: SN = S0 + 3 */
 entropy->dc_context[ci] = 8; /* small negative diff category */
      }
      /* Figure F.8: Encoding the magnitude category of v */
      m = 0;
      if (v -= 1) {
 arith_encode(cinfo, st, 1);
 m = 1;
 v2 = v;
 st = entropy->dc_stats[tbl] + 20; /* Table F.4: X1 = 20 */
 while (v2 >>= 1) {
   arith_encode(cinfo, st, 1);
   m <<= 1;
   st += 1;
 }
      }
      arith_encode(cinfo, st, 0);
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) ((1L << cinfo->arith_dc_L[tbl]) >> 1))
 entropy->dc_context[ci] = 0; /* zero diff category */
      else if (m > (int) ((1L << cinfo->arith_dc_U[tbl]) >> 1))
 entropy->dc_context[ci] += 8; /* large diff category */
      /* Figure F.9: Encoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 arith_encode(cinfo, st, (m & v) ? 1 : 0);
    }
  }
  return 1;
}
/*
 * MCU encoding for AC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 0285C260 */
static boolean
encode_mcu_AC_first (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<u8>*st;
  int tbl, k, ke;
  int v, v2, m;
  const be<s32>* natural_order;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      emit_restart(cinfo, entropy->next_restart_num);
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  natural_order = cinfo->natural_order;
  /* Encode the MCU data block */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;
  /* Sections F.1.4.2 & F.1.4.4.2: Encoding of AC coefficients */
  /* Establish EOB (end-of-block) index */
  for (ke = cinfo->Se; ke > 0; ke--)
    /* We must apply the point transform by Al.  For AC coefficients this
     * is an integer division with rounding towards 0.  To do this portably
     * in C, we shift after obtaining the absolute value.
     */
    if ((v = (*block)[natural_order[ke]]) >= 0) {
      if (v >>= cinfo->Al) break;
    } else {
      v = -v;
      if (v >>= cinfo->Al) break;
    }
  /* Figure F.5: Encode_AC_Coefficients */
  for (k = cinfo->Ss; k <= ke; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    arith_encode(cinfo, st, 0); /* EOB decision */
    for (;;) {
      if ((v = (*block)[natural_order[k]]) >= 0) {
 if (v >>= cinfo->Al) {
   arith_encode(cinfo, st + 1, 1);
   arith_encode(cinfo, entropy->fixed_bin, 0);
   break;
 }
      } else {
 v = -v;
 if (v >>= cinfo->Al) {
   arith_encode(cinfo, st + 1, 1);
   arith_encode(cinfo, entropy->fixed_bin, 1);
   break;
 }
      }
      arith_encode(cinfo, st + 1, 0); st += 3; k++;
    }
    st += 2;
    /* Figure F.8: Encoding the magnitude category of v */
    m = 0;
    if (v -= 1) {
      arith_encode(cinfo, st, 1);
      m = 1;
      v2 = v;
      if (v2 >>= 1) {
 arith_encode(cinfo, st, 1);
 m <<= 1;
 st = entropy->ac_stats[tbl] +
      (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
 while (v2 >>= 1) {
   arith_encode(cinfo, st, 1);
   m <<= 1;
   st += 1;
 }
      }
    }
    arith_encode(cinfo, st, 0);
    /* Figure F.9: Encoding the magnitude bit pattern of v */
    st += 14;
    while (m >>= 1)
      arith_encode(cinfo, st, (m & v) ? 1 : 0);
  }
  /* Encode EOB decision only if k <= cinfo->Se */
  if (k <= cinfo->Se) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    arith_encode(cinfo, st, 1);
  }
  return 1;
}
/*
 * MCU encoding for DC successive approximation refinement scan.
 */
/* 0285C508 */
static boolean
encode_mcu_DC_refine (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  be<u8>*st;
  int Al, blkn;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      emit_restart(cinfo, entropy->next_restart_num);
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  st = entropy->fixed_bin; /* use fixed probability estimation */
  Al = cinfo->Al;
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    /* We simply emit the Al'th bit of the DC coefficient value. */
    arith_encode(cinfo, st, (MCU_data[blkn][0][0] >> Al) & 1);
  }
  return 1;
}
/*
 * MCU encoding for AC successive approximation refinement scan.
 */
/* 0285C5C4 */
static boolean
encode_mcu_AC_refine (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  JBLOCKROW block;
  be<u8>*st;
  int tbl, k, ke, kex;
  int v;
  const be<s32>* natural_order;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      emit_restart(cinfo, entropy->next_restart_num);
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  natural_order = cinfo->natural_order;
  /* Encode the MCU data block */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;
  /* Section G.1.3.3: Encoding of AC coefficients */
  /* Establish EOB (end-of-block) index */
  for (ke = cinfo->Se; ke > 0; ke--)
    /* We must apply the point transform by Al.  For AC coefficients this
     * is an integer division with rounding towards 0.  To do this portably
     * in C, we shift after obtaining the absolute value.
     */
    if ((v = (*block)[natural_order[ke]]) >= 0) {
      if (v >>= cinfo->Al) break;
    } else {
      v = -v;
      if (v >>= cinfo->Al) break;
    }
  /* Establish EOBx (previous stage end-of-block) index */
  for (kex = ke; kex > 0; kex--)
    if ((v = (*block)[natural_order[kex]]) >= 0) {
      if (v >>= cinfo->Ah) break;
    } else {
      v = -v;
      if (v >>= cinfo->Ah) break;
    }
  /* Figure G.10: Encode_AC_Coefficients_SA */
  for (k = cinfo->Ss; k <= ke; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    if (k > kex)
      arith_encode(cinfo, st, 0); /* EOB decision */
    for (;;) {
      if ((v = (*block)[natural_order[k]]) >= 0) {
 if (v >>= cinfo->Al) {
   if (v >> 1) /* previously nonzero coef */
     arith_encode(cinfo, st + 2, (v & 1));
   else { /* newly nonzero coef */
     arith_encode(cinfo, st + 1, 1);
     arith_encode(cinfo, entropy->fixed_bin, 0);
   }
   break;
 }
      } else {
 v = -v;
 if (v >>= cinfo->Al) {
   if (v >> 1) /* previously nonzero coef */
     arith_encode(cinfo, st + 2, (v & 1));
   else { /* newly nonzero coef */
     arith_encode(cinfo, st + 1, 1);
     arith_encode(cinfo, entropy->fixed_bin, 1);
   }
   break;
 }
      }
      arith_encode(cinfo, st + 1, 0); st += 3; k++;
    }
  }
  /* Encode EOB decision only if k <= cinfo->Se */
  if (k <= cinfo->Se) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    arith_encode(cinfo, st, 1);
  }
  return 1;
}
/*
 * Encode and output one MCU's worth of arithmetic-compressed coefficients.
 */
/* 0285C830 */
static boolean
encode_mcu (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  jpeg_component_info * compptr;
  JBLOCKROW block;
  be<u8>*st;
  int blkn, ci, tbl, k, ke;
  int v, v2, m;
  const be<s32>* natural_order;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      emit_restart(cinfo, entropy->next_restart_num);
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  natural_order = cinfo->natural_order;
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];
    /* Sections F.1.4.1 & F.1.4.4.1: Encoding of DC coefficients */
    tbl = compptr->dc_tbl_no;
    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];
    /* Figure F.4: Encode_DC_DIFF */
    if ((v = (*block)[0] - entropy->last_dc_val[ci]) == 0) {
      arith_encode(cinfo, st, 0);
      entropy->dc_context[ci] = 0; /* zero diff category */
    } else {
      entropy->last_dc_val[ci] = (*block)[0];
      arith_encode(cinfo, st, 1);
      /* Figure F.6: Encoding nonzero value v */
      /* Figure F.7: Encoding the sign of v */
      if (v > 0) {
 arith_encode(cinfo, st + 1, 0); /* Table F.4: SS = S0 + 1 */
 st += 2; /* Table F.4: SP = S0 + 2 */
 entropy->dc_context[ci] = 4; /* small positive diff category */
      } else {
 v = -v;
 arith_encode(cinfo, st + 1, 1); /* Table F.4: SS = S0 + 1 */
 st += 3; /* Table F.4: SN = S0 + 3 */
 entropy->dc_context[ci] = 8; /* small negative diff category */
      }
      /* Figure F.8: Encoding the magnitude category of v */
      m = 0;
      if (v -= 1) {
 arith_encode(cinfo, st, 1);
 m = 1;
 v2 = v;
 st = entropy->dc_stats[tbl] + 20; /* Table F.4: X1 = 20 */
 while (v2 >>= 1) {
   arith_encode(cinfo, st, 1);
   m <<= 1;
   st += 1;
 }
      }
      arith_encode(cinfo, st, 0);
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) ((1L << cinfo->arith_dc_L[tbl]) >> 1))
 entropy->dc_context[ci] = 0; /* zero diff category */
      else if (m > (int) ((1L << cinfo->arith_dc_U[tbl]) >> 1))
 entropy->dc_context[ci] += 8; /* large diff category */
      /* Figure F.9: Encoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 arith_encode(cinfo, st, (m & v) ? 1 : 0);
    }
    /* Sections F.1.4.2 & F.1.4.4.2: Encoding of AC coefficients */
    tbl = compptr->ac_tbl_no;
    /* Establish EOB (end-of-block) index */
    for (ke = cinfo->lim_Se; ke > 0; ke--)
      if ((*block)[natural_order[ke]]) break;
    /* Figure F.5: Encode_AC_Coefficients */
    for (k = 1; k <= ke; k++) {
      st = entropy->ac_stats[tbl] + 3 * (k - 1);
      arith_encode(cinfo, st, 0); /* EOB decision */
      while ((v = (*block)[natural_order[k]]) == 0) {
 arith_encode(cinfo, st + 1, 0); st += 3; k++;
      }
      arith_encode(cinfo, st + 1, 1);
      /* Figure F.6: Encoding nonzero value v */
      /* Figure F.7: Encoding the sign of v */
      if (v > 0) {
 arith_encode(cinfo, entropy->fixed_bin, 0);
      } else {
 v = -v;
 arith_encode(cinfo, entropy->fixed_bin, 1);
      }
      st += 2;
      /* Figure F.8: Encoding the magnitude category of v */
      m = 0;
      if (v -= 1) {
 arith_encode(cinfo, st, 1);
 m = 1;
 v2 = v;
 if (v2 >>= 1) {
   arith_encode(cinfo, st, 1);
   m <<= 1;
   st = entropy->ac_stats[tbl] +
        (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
   while (v2 >>= 1) {
     arith_encode(cinfo, st, 1);
     m <<= 1;
     st += 1;
   }
 }
      }
      arith_encode(cinfo, st, 0);
      /* Figure F.9: Encoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
 arith_encode(cinfo, st, (m & v) ? 1 : 0);
    }
    /* Encode EOB decision only if k <= cinfo->lim_Se */
    if (k <= cinfo->lim_Se) {
      st = entropy->ac_stats[tbl] + 3 * (k - 1);
      arith_encode(cinfo, st, 1);
    }
  }
  return 1;
}
/*
 * Initialize for an arithmetic-compressed scan.
 */
/* 0285CC78 */
static void
start_pass (j_compress_ptr cinfo, boolean gather_statistics)
{
  arith_entropy_encoder * entropy = (arith_entropy_encoder *) cinfo->entropy.get();
  int ci, tbl;
  jpeg_component_info * compptr;
  if (gather_statistics)
    /* Make sure to avoid that in the master control logic!
     * We are fully adaptive here and need no extra
     * statistics gathering pass!
     */
    jpeg_error(cinfo, JERR_NOT_COMPILED);
  /* We assume jcmaster.c already validated the progressive scan parameters. */
  /* Select execution routines */
  if (cinfo->progressive_mode) {
    if (cinfo->Ah == 0) {
      if (cinfo->Ss == 0)
 entropy->pub.encode_mcu = encode_mcu_DC_first;
      else
 entropy->pub.encode_mcu = encode_mcu_AC_first;
    } else {
      if (cinfo->Ss == 0)
 entropy->pub.encode_mcu = encode_mcu_DC_refine;
      else
 entropy->pub.encode_mcu = encode_mcu_AC_refine;
    }
  } else
    entropy->pub.encode_mcu = encode_mcu;
  /* Allocate & initialize requested statistics areas */
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* DC needs no table for refinement scan */
    if (cinfo->Ss == 0 && cinfo->Ah == 0) {
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
    /* AC needs no table when not present */
    if (cinfo->Se) {
      tbl = compptr->ac_tbl_no;
      if (tbl < 0 || tbl >= NUM_ARITH_TBLS)
 jpeg_error1(cinfo, JERR_NO_ARITH_TABLE, tbl);
      if (entropy->ac_stats[tbl] == nullptr)
 entropy->ac_stats[tbl] = (be<u8>*) (*cinfo->mem->alloc_small)
   ((j_common_ptr) cinfo, JPOOL_IMAGE, 256);
      arith_zero(entropy->ac_stats[tbl], 256);
    }
  }
  /* Initialize arithmetic encoding variables */
  entropy->c = 0;
  entropy->a = 0x10000L;
  entropy->sc = 0;
  entropy->zc = 0;
  entropy->ct = 11;
  entropy->buffer = -1; /* empty */
  /* Initialize restart stuff */
  entropy->restarts_to_go = cinfo->restart_interval;
  entropy->next_restart_num = 0;
}
/*
 * Module initialization routine for arithmetic entropy encoding.
 */
/* 0285CEC0 */
void
jinit_arith_encoder (j_compress_ptr cinfo)
{
  arith_entropy_encoder * entropy;
  int i;
  entropy = (arith_entropy_encoder *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
    sizeof(arith_entropy_encoder));
  cinfo->entropy = (struct jpeg_entropy_encoder *) entropy;
  entropy->pub.start_pass = start_pass;
  entropy->pub.finish_pass = finish_pass;
  /* Mark tables unallocated */
  for (i = 0; i < NUM_ARITH_TBLS; i++) {
    entropy->dc_stats[i] = nullptr;
    entropy->ac_stats[i] = nullptr;
  }
  /* Initialize index for fixed probability estimation */
  entropy->fixed_bin[0] = 113;
}

/* WWHD glue
data jpeg_aritab 1017DB14
*/
