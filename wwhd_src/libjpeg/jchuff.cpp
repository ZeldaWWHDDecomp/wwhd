#include "libjpeg/jchuff.h"
/*
 * jchuff.cpp (decompiled WWHD build)
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2006-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 *
 * This file contains Huffman entropy encoding routines.
 * Both sequential and progressive modes are supported in this single module.
 *
 * Much of the complexity here has to do with supporting output suspension.
 * If the data destination module demands suspension, we want to be able to
 * back up to the start of the current MCU.  To do this, we copy state
 * variables into local working storage, and update them back to the
 * permanent JPEG objects only upon successful completion of an MCU.
 *
 * We do not support output suspension for the progressive JPEG mode, since
 * the library currently does not allow multiple-scan files to be written
 * with output suspension.
 */
/* The legal range of a DCT coefficient is
 *  -1024 .. +1023  for 8-bit data;
 * -16384 .. +16383 for 12-bit data.
 * Hence the magnitude should always fit in 10 or 14 bits respectively.
 */
/* MAX_CORR_BITS is the number of bits the AC refinement correction-bit
 * buffer can hold.  Larger sizes may slightly improve compression, but
 * 1000 is already well into the realm of overkill.
 * The minimum safe size is 64 bits.
 */
/* IRIGHT_SHIFT is like RIGHT_SHIFT, but works on int rather than long.
 * We assume that int right shift is unsigned if long right shift is,
 * which should be safe.
 */
/*
 * Compute the derived values for a Huffman table.
 * This routine also performs some validation checks on the table.
 */
/* 0285E438 */
static void
jpeg_make_c_derived_tbl (j_compress_ptr cinfo, boolean isDC, int tblno,
    gptr<c_derived_tbl>* pdtbl)
{
  JHUFF_TBL *htbl;
  c_derived_tbl *dtbl;
  int p, i, l, lastp, si, maxsymbol;
  char huffsize[257];
  unsigned int huffcode[257];
  unsigned int code;
  /* Note that huffsize[] and huffcode[] are filled in code-length order,
   * paralleling the order of the symbols themselves in htbl->huffval[].
   */
  /* Find the input Huffman table */
  if (tblno < 0 || tblno >= NUM_HUFF_TBLS)
    jpeg_error1(cinfo, JERR_NO_HUFF_TABLE, tblno);
  htbl =
    isDC ? cinfo->dc_huff_tbl_ptrs[tblno] : cinfo->ac_huff_tbl_ptrs[tblno];
  if (htbl == nullptr)
    jpeg_error1(cinfo, JERR_NO_HUFF_TABLE, tblno);
  /* Allocate a workspace if we haven't already done so. */
  if (*pdtbl == nullptr)
    *pdtbl = (c_derived_tbl *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
      sizeof(c_derived_tbl));
  dtbl = *pdtbl;
  /* Figure C.1: make table of Huffman code length for each symbol */
  p = 0;
  for (l = 1; l <= 16; l++) {
    i = (int) htbl->bits[l];
    if (i < 0 || p + i > 256) /* protect against table overrun */
      jpeg_error(cinfo, JERR_BAD_HUFF_TABLE);
    while (i--)
      huffsize[p++] = (char) l;
  }
  huffsize[p] = 0;
  lastp = p;
  /* Figure C.2: generate the codes themselves */
  /* We also validate that the counts represent a legal Huffman code tree. */
  code = 0;
  si = huffsize[0];
  p = 0;
  while (huffsize[p]) {
    while (((int) huffsize[p]) == si) {
      huffcode[p++] = code;
      code++;
    }
    /* code is now 1 more than the last code used for codelength si; but
     * it must still fit in si bits, since no code is allowed to be all ones.
     */
    if (((s32) code) >= (((s32) 1) << si))
      jpeg_error(cinfo, JERR_BAD_HUFF_TABLE);
    code <<= 1;
    si++;
  }
  /* Figure C.3: generate encoding tables */
  /* These are code and size indexed by symbol value */
  /* Set all codeless symbols to have code length 0;
   * this lets us detect duplicate VAL entries here, and later
   * allows emit_bits to detect any attempt to emit such symbols.
   */
  huff_zero(dtbl->ehufsi, sizeof(dtbl->ehufsi));
  /* This is also a convenient place to check for out-of-range
   * and duplicated VAL entries.  We allow 0..255 for AC symbols
   * but only 0..15 for DC.  (We could constrain them further
   * based on data depth and mode, but this seems enough.)
   */
  maxsymbol = isDC ? 15 : 255;
  for (p = 0; p < lastp; p++) {
    i = htbl->huffval[p];
    if (i < 0 || i > maxsymbol || dtbl->ehufsi[i])
      jpeg_error(cinfo, JERR_BAD_HUFF_TABLE);
    dtbl->ehufco[i] = huffcode[p];
    dtbl->ehufsi[i] = huffsize[p];
  }
}
/* Outputting bytes to the file.
 * NB: these must be called only when actually outputting,
 * that is, entropy->gather_statistics == 0.
 */
/* Emit a byte, taking 'action' if must suspend. */
/* Emit a byte */
/* 0285E680 */
static boolean
dump_buffer_s (huff_encoder_working_state * state)
/* Empty the output buffer; return 1 if successful, 0 if must suspend */
{
  struct jpeg_destination_mgr * dest = state->cinfo->dest;
  if (! (*dest->empty_output_buffer) (state->cinfo))
    return 0;
  /* After a successful buffer dump, must reset buffer pointers */
  state->next_output_byte = dest->next_output_byte;
  state->free_in_buffer = dest->free_in_buffer;
  return 1;
}
/* 0285E6F8 */
static void
dump_buffer_e (huff_entropy_encoder * entropy)
/* Empty the output buffer; we do not support suspension in this case. */
{
  struct jpeg_destination_mgr * dest = entropy->cinfo->dest;
  if (! (*dest->empty_output_buffer) (entropy->cinfo))
    jpeg_error(entropy->cinfo, JERR_CANT_SUSPEND);
  /* After a successful buffer dump, must reset buffer pointers */
  entropy->next_output_byte = dest->next_output_byte;
  entropy->free_in_buffer = dest->free_in_buffer;
}
/* Outputting bits to the file */
/* Only the right 24 bits of put_buffer are used; the valid bits are
 * left-justified in this part.  At most 16 bits can be passed to emit_bits
 * in one call, and we never retain more than 7 bits in put_buffer
 * between calls, so 24 bits are sufficient.
 */
inline
static boolean
emit_bits_s (huff_encoder_working_state * state, unsigned int code, int size)
/* Emit some bits; return 1 if successful, 0 if must suspend */
{
  /* This routine is heavily used, so it's worth coding tightly. */
  s32 put_buffer = (s32) code;
  int put_bits = state->cur.put_bits;
  /* if size is 0, caller used an invalid Huffman table entry */
  if (size == 0)
    jpeg_error(state->cinfo, JERR_HUFF_MISSING_CODE);
  // HD: Cafe masks shift counts to six bits and yields zero when bit 5 is set.
  const u32 mask_shift = (u32)size & 63;
  const u32 code_mask = mask_shift < 32 ? (u32(1) << mask_shift) - 1 : ~u32(0);
  put_buffer &= code_mask; /* mask off any extra bits in code */
  put_bits += size; /* new number of bits in buffer */
  const u32 alignment_shift = (u32)(24 - put_bits) & 63;
  put_buffer = alignment_shift < 32 ? (s32)((u32)put_buffer << alignment_shift) : 0; /* align incoming bits */
  put_buffer |= state->cur.put_buffer; /* and merge with old buffer contents */
  while (put_bits >= 8) {
    int c = (int) ((put_buffer >> 16) & 0xFF);
    { *(state)->next_output_byte = (JOCTET) (c); (state)->next_output_byte = (state)->next_output_byte.get() + 1; if (--(state)->free_in_buffer == 0) if (! dump_buffer_s(state)) { return 0; } };
    if (c == 0xFF) { /* need to stuff a zero byte? */
      { *(state)->next_output_byte = (JOCTET) (0); (state)->next_output_byte = (state)->next_output_byte.get() + 1; if (--(state)->free_in_buffer == 0) if (! dump_buffer_s(state)) { return 0; } };
    }
    put_buffer <<= 8;
    put_bits -= 8;
  }
  state->cur.put_buffer = put_buffer; /* update state variables */
  state->cur.put_bits = put_bits;
  return 1;
}
inline
static void
emit_bits_e (huff_entropy_encoder * entropy, unsigned int code, int size)
/* Emit some bits, unless we are in gather mode */
{
  /* This routine is heavily used, so it's worth coding tightly. */
  s32 put_buffer = (s32) code;
  int put_bits = entropy->saved.put_bits;
  /* if size is 0, caller used an invalid Huffman table entry */
  if (size == 0)
    jpeg_error(entropy->cinfo, JERR_HUFF_MISSING_CODE);
  if (entropy->gather_statistics)
    return; /* do nothing if we're only getting stats */
  // HD: Cafe masks shift counts to six bits and yields zero when bit 5 is set.
  const u32 mask_shift = (u32)size & 63;
  const u32 code_mask = mask_shift < 32 ? (u32(1) << mask_shift) - 1 : ~u32(0);
  put_buffer &= code_mask; /* mask off any extra bits in code */
  put_bits += size; /* new number of bits in buffer */
  const u32 alignment_shift = (u32)(24 - put_bits) & 63;
  put_buffer = alignment_shift < 32 ? (s32)((u32)put_buffer << alignment_shift) : 0; /* align incoming bits */
  /* and merge with old buffer contents */
  put_buffer |= entropy->saved.put_buffer;
  while (put_bits >= 8) {
    int c = (int) ((put_buffer >> 16) & 0xFF);
    { *(entropy)->next_output_byte = (JOCTET) (c); (entropy)->next_output_byte = (entropy)->next_output_byte.get() + 1; if (--(entropy)->free_in_buffer == 0) dump_buffer_e(entropy); };
    if (c == 0xFF) { /* need to stuff a zero byte? */
      { *(entropy)->next_output_byte = (JOCTET) (0); (entropy)->next_output_byte = (entropy)->next_output_byte.get() + 1; if (--(entropy)->free_in_buffer == 0) dump_buffer_e(entropy); };
    }
    put_buffer <<= 8;
    put_bits -= 8;
  }
  entropy->saved.put_buffer = put_buffer; /* update variables */
  entropy->saved.put_bits = put_bits;
}
/* 0285E770 */
static boolean
flush_bits_s (huff_encoder_working_state * state)
{
  if (! emit_bits_s(state, 0x7F, 7)) /* fill any partial byte with ones */
    return 0;
  state->cur.put_buffer = 0; /* and reset bit-buffer to empty */
  state->cur.put_bits = 0;
  return 1;
}
/* 0285E860 */
static void
flush_bits_e (huff_entropy_encoder * entropy)
{
  emit_bits_e(entropy, 0x7F, 7); /* fill any partial byte with ones */
  entropy->saved.put_buffer = 0; /* and reset bit-buffer to empty */
  entropy->saved.put_bits = 0;
}
/*
 * Emit (or just count) a Huffman symbol.
 */
inline
static void
emit_dc_symbol (huff_entropy_encoder * entropy, int tbl_no, int symbol)
{
  if (entropy->gather_statistics)
    entropy->dc_count_ptrs[tbl_no][symbol]++;
  else {
    c_derived_tbl * tbl = entropy->dc_derived_tbls[tbl_no];
    emit_bits_e(entropy, tbl->ehufco[symbol], tbl->ehufsi[symbol]);
  }
}
inline
static void
emit_ac_symbol (huff_entropy_encoder * entropy, int tbl_no, int symbol)
{
  if (entropy->gather_statistics)
    entropy->ac_count_ptrs[tbl_no][symbol]++;
  else {
    c_derived_tbl * tbl = entropy->ac_derived_tbls[tbl_no];
    emit_bits_e(entropy, tbl->ehufco[symbol], tbl->ehufsi[symbol]);
  }
}
/*
 * Emit bits from a correction bit buffer.
 */
/* 0285E930 */
static void
emit_buffered_bits (huff_entropy_encoder * entropy, be<u8>* bufstart,
      unsigned int nbits)
{
  if (entropy->gather_statistics)
    return; /* no real work */
  while (nbits > 0) {
    emit_bits_e(entropy, (unsigned int) (*bufstart), 1);
    bufstart++;
    nbits--;
  }
}
/*
 * Emit any pending EOBRUN symbol.
 */
/* 0285EA2C */
static void
emit_eobrun (huff_entropy_encoder * entropy)
{
  int temp, nbits;
  if (entropy->EOBRUN > 0) { /* if there is any pending EOBRUN */
    temp = entropy->EOBRUN;
    nbits = 0;
    while ((temp >>= 1))
      nbits++;
    /* safety check: shouldn't happen given limited correction-bit buffer */
    if (nbits > 14)
      jpeg_error(entropy->cinfo, JERR_HUFF_MISSING_CODE);
    emit_ac_symbol(entropy, entropy->ac_tbl_no, nbits << 4);
    if (nbits)
      emit_bits_e(entropy, entropy->EOBRUN, nbits);
    entropy->EOBRUN = 0;
    /* Emit any buffered correction bits */
    emit_buffered_bits(entropy, entropy->bit_buffer, entropy->BE);
    entropy->BE = 0;
  }
}
/*
 * Emit a restart marker & resynchronize predictions.
 */
/* 0285EC94 */
static boolean
emit_restart_s (huff_encoder_working_state * state, int restart_num)
{
  int ci;
  if (! flush_bits_s(state))
    return 0;
  { *(state)->next_output_byte = (JOCTET) (0xFF); (state)->next_output_byte = (state)->next_output_byte.get() + 1; if (--(state)->free_in_buffer == 0) if (! dump_buffer_s(state)) { return 0; } };
  { *(state)->next_output_byte = (JOCTET) (0xD0 + restart_num); (state)->next_output_byte = (state)->next_output_byte.get() + 1; if (--(state)->free_in_buffer == 0) if (! dump_buffer_s(state)) { return 0; } };
  /* Re-initialize DC predictions to 0 */
  for (ci = 0; ci < state->cinfo->comps_in_scan; ci++)
    state->cur.last_dc_val[ci] = 0;
  /* The restart counter is not updated until we successfully write the MCU. */
  return 1;
}
/* 0285ED94 */
static void
emit_restart_e (huff_entropy_encoder * entropy, int restart_num)
{
  int ci;
  emit_eobrun(entropy);
  if (! entropy->gather_statistics) {
    flush_bits_e(entropy);
    { *(entropy)->next_output_byte = (JOCTET) (0xFF); (entropy)->next_output_byte = (entropy)->next_output_byte.get() + 1; if (--(entropy)->free_in_buffer == 0) dump_buffer_e(entropy); };
    { *(entropy)->next_output_byte = (JOCTET) (0xD0 + restart_num); (entropy)->next_output_byte = (entropy)->next_output_byte.get() + 1; if (--(entropy)->free_in_buffer == 0) dump_buffer_e(entropy); };
  }
  if (entropy->cinfo->Ss == 0) {
    /* Re-initialize DC predictions to 0 */
    for (ci = 0; ci < entropy->cinfo->comps_in_scan; ci++)
      entropy->saved.last_dc_val[ci] = 0;
  } else {
    /* Re-initialize all AC-related fields to 0 */
    entropy->EOBRUN = 0;
    entropy->BE = 0;
  }
}
/*
 * MCU encoding for DC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 0285EE80 */
static boolean
encode_mcu_DC_first (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int temp, temp2;
  int nbits;
  int blkn, ci;
  int Al = cinfo->Al;
  JBLOCKROW block;
  jpeg_component_info * compptr;
  entropy->next_output_byte = cinfo->dest->next_output_byte;
  entropy->free_in_buffer = cinfo->dest->free_in_buffer;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval)
    if (entropy->restarts_to_go == 0)
      emit_restart_e(entropy, entropy->next_restart_num);
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];
    /* Compute the DC value after the required point transform by Al.
     * This is simply an arithmetic right shift.
     */
    temp2 = (((int) ((*block)[0])) >> (Al));
    /* DC differences are figured on the point-transformed values. */
    temp = temp2 - entropy->saved.last_dc_val[ci];
    entropy->saved.last_dc_val[ci] = temp2;
    /* Encode the DC coefficient difference per section G.1.2.1 */
    temp2 = temp;
    if (temp < 0) {
      temp = -temp; /* temp is abs value of input */
      /* For a negative input, want temp2 = bitwise complement of abs(input) */
      /* This code assumes we are on a two's complement machine */
      temp2--;
    }
    /* Find the number of bits needed for the magnitude of the coefficient */
    nbits = 0;
    while (temp) {
      nbits++;
      temp >>= 1;
    }
    /* Check for out-of-range coefficient values.
     * Since we're encoding a difference, the range limit is twice as much.
     */
    if (nbits > 10 +1)
      jpeg_error(cinfo, JERR_BAD_DCT_COEF);
    /* Count/emit the Huffman-coded symbol for the number of bits */
    emit_dc_symbol(entropy, compptr->dc_tbl_no, nbits);
    /* Emit that number of bits of the value, if positive, */
    /* or the complement of its magnitude, if negative. */
    if (nbits) /* emit_bits rejects calls with size 0 */
      emit_bits_e(entropy, (unsigned int) temp2, nbits);
  }
  cinfo->dest->next_output_byte = entropy->next_output_byte;
  cinfo->dest->free_in_buffer = entropy->free_in_buffer;
  /* Update restart-interval state too */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  return 1;
}
/*
 * MCU encoding for AC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */
/* 0285F1B0 */
static boolean
encode_mcu_AC_first (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int temp, temp2;
  int nbits;
  int r, k;
  int Se, Al;
  const be<s32>* natural_order;
  JBLOCKROW block;
  entropy->next_output_byte = cinfo->dest->next_output_byte;
  entropy->free_in_buffer = cinfo->dest->free_in_buffer;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval)
    if (entropy->restarts_to_go == 0)
      emit_restart_e(entropy, entropy->next_restart_num);
  Se = cinfo->Se;
  Al = cinfo->Al;
  natural_order = cinfo->natural_order;
  /* Encode the MCU data block */
  block = MCU_data[0];
  /* Encode the AC coefficients per section G.1.2.2, fig. G.3 */
  r = 0; /* r = run length of zeros */
  for (k = cinfo->Ss; k <= Se; k++) {
    if ((temp = (*block)[natural_order[k]]) == 0) {
      r++;
      continue;
    }
    /* We must apply the point transform by Al.  For AC coefficients this
     * is an integer division with rounding towards 0.  To do this portably
     * in C, we shift after obtaining the absolute value; so the code is
     * interwoven with finding the abs value (temp) and output bits (temp2).
     */
    if (temp < 0) {
      temp = -temp; /* temp is abs value of input */
      temp >>= Al; /* apply the point transform */
      /* For a negative coef, want temp2 = bitwise complement of abs(coef) */
      temp2 = ~temp;
    } else {
      temp >>= Al; /* apply the point transform */
      temp2 = temp;
    }
    /* Watch out for case that nonzero coef is zero after point transform */
    if (temp == 0) {
      r++;
      continue;
    }
    /* Emit any pending EOBRUN */
    if (entropy->EOBRUN > 0)
      emit_eobrun(entropy);
    /* if run length > 15, must emit special run-length-16 codes (0xF0) */
    while (r > 15) {
      emit_ac_symbol(entropy, entropy->ac_tbl_no, 0xF0);
      r -= 16;
    }
    /* Find the number of bits needed for the magnitude of the coefficient */
    nbits = 1; /* there must be at least one 1 bit */
    while ((temp >>= 1))
      nbits++;
    /* Check for out-of-range coefficient values */
    if (nbits > 10)
      jpeg_error(cinfo, JERR_BAD_DCT_COEF);
    /* Count/emit Huffman symbol for run length / number of bits */
    emit_ac_symbol(entropy, entropy->ac_tbl_no, (r << 4) + nbits);
    /* Emit that number of bits of the value, if positive, */
    /* or the complement of its magnitude, if negative. */
    emit_bits_e(entropy, (unsigned int) temp2, nbits);
    r = 0; /* reset zero run length */
  }
  if (r > 0) { /* If there are trailing zeroes, */
    entropy->EOBRUN++; /* count an EOB */
    if (entropy->EOBRUN == 0x7FFF)
      emit_eobrun(entropy); /* force it out to avoid overflow */
  }
  cinfo->dest->next_output_byte = entropy->next_output_byte;
  cinfo->dest->free_in_buffer = entropy->free_in_buffer;
  /* Update restart-interval state too */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  return 1;
}
/*
 * MCU encoding for DC successive approximation refinement scan.
 * Note: we assume such scans can be multi-component, although the spec
 * is not very clear on the point.
 */
/* 0285F678 */
static boolean
encode_mcu_DC_refine (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int temp;
  int blkn;
  int Al = cinfo->Al;
  JBLOCKROW block;
  entropy->next_output_byte = cinfo->dest->next_output_byte;
  entropy->free_in_buffer = cinfo->dest->free_in_buffer;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval)
    if (entropy->restarts_to_go == 0)
      emit_restart_e(entropy, entropy->next_restart_num);
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    /* We simply emit the Al'th bit of the DC coefficient value. */
    temp = (*block)[0];
    emit_bits_e(entropy, (unsigned int) (temp >> Al), 1);
  }
  cinfo->dest->next_output_byte = entropy->next_output_byte;
  cinfo->dest->free_in_buffer = entropy->free_in_buffer;
  /* Update restart-interval state too */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  return 1;
}
/*
 * MCU encoding for AC successive approximation refinement scan.
 */
/* 0285F808 */
static boolean
encode_mcu_AC_refine (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int temp;
  int r, k;
  int EOB;
  be<u8>* BR_buffer;
  unsigned int BR;
  int Se, Al;
  const be<s32>* natural_order;
  JBLOCKROW block;
  int absvalues[DCTSIZE2];
  entropy->next_output_byte = cinfo->dest->next_output_byte;
  entropy->free_in_buffer = cinfo->dest->free_in_buffer;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval)
    if (entropy->restarts_to_go == 0)
      emit_restart_e(entropy, entropy->next_restart_num);
  Se = cinfo->Se;
  Al = cinfo->Al;
  natural_order = cinfo->natural_order;
  /* Encode the MCU data block */
  block = MCU_data[0];
  /* It is convenient to make a pre-pass to determine the transformed
   * coefficients' absolute values and the EOB position.
   */
  EOB = 0;
  for (k = cinfo->Ss; k <= Se; k++) {
    temp = (*block)[natural_order[k]];
    /* We must apply the point transform by Al.  For AC coefficients this
     * is an integer division with rounding towards 0.  To do this portably
     * in C, we shift after obtaining the absolute value.
     */
    if (temp < 0)
      temp = -temp; /* temp is abs value of input */
    temp >>= Al; /* apply the point transform */
    absvalues[k] = temp; /* save abs value for main pass */
    if (temp == 1)
      EOB = k; /* EOB = index of last newly-nonzero coef */
  }
  /* Encode the AC coefficients per section G.1.2.3, fig. G.7 */
  r = 0; /* r = run length of zeros */
  BR = 0; /* BR = count of buffered bits added now */
  BR_buffer = entropy->bit_buffer + entropy->BE; /* Append bits to buffer */
  for (k = cinfo->Ss; k <= Se; k++) {
    if ((temp = absvalues[k]) == 0) {
      r++;
      continue;
    }
    /* Emit any required ZRLs, but not if they can be folded into EOB */
    while (r > 15 && k <= EOB) {
      /* emit any pending EOBRUN and the BE correction bits */
      emit_eobrun(entropy);
      /* Emit ZRL */
      emit_ac_symbol(entropy, entropy->ac_tbl_no, 0xF0);
      r -= 16;
      /* Emit buffered correction bits that must be associated with ZRL */
      emit_buffered_bits(entropy, BR_buffer, BR);
      BR_buffer = entropy->bit_buffer; /* BE bits are gone now */
      BR = 0;
    }
    /* If the coef was previously nonzero, it only needs a correction bit.
     * NOTE: a straight translation of the spec's figure G.7 would suggest
     * that we also need to test r > 15.  But if r > 15, we can only get here
     * if k > EOB, which implies that this coefficient is not 1.
     */
    if (temp > 1) {
      /* The correction bit is the next bit of the absolute value. */
      BR_buffer[BR++] = (char) (temp & 1);
      continue;
    }
    /* Emit any pending EOBRUN and the BE correction bits */
    emit_eobrun(entropy);
    /* Count/emit Huffman symbol for run length / number of bits */
    emit_ac_symbol(entropy, entropy->ac_tbl_no, (r << 4) + 1);
    /* Emit output bit for newly-nonzero coef */
    temp = ((*block)[natural_order[k]] < 0) ? 0 : 1;
    emit_bits_e(entropy, (unsigned int) temp, 1);
    /* Emit buffered correction bits that must be associated with this code */
    emit_buffered_bits(entropy, BR_buffer, BR);
    BR_buffer = entropy->bit_buffer; /* BE bits are gone now */
    BR = 0;
    r = 0; /* reset zero run length */
  }
  if (r > 0 || BR > 0) { /* If there are trailing zeroes, */
    entropy->EOBRUN++; /* count an EOB */
    entropy->BE += BR; /* concat my correction bits to older ones */
    /* We force out the EOB if we risk either:
     * 1. overflow of the EOB counter;
     * 2. overflow of the correction bit buffer during the next MCU.
     */
    if (entropy->EOBRUN == 0x7FFF || entropy->BE > (1000 -DCTSIZE2+1))
      emit_eobrun(entropy);
  }
  cinfo->dest->next_output_byte = entropy->next_output_byte;
  cinfo->dest->free_in_buffer = entropy->free_in_buffer;
  /* Update restart-interval state too */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  return 1;
}
/* Encode a single block's worth of coefficients */
/* 0285FD3C */
static boolean
encode_one_block (huff_encoder_working_state * state, be<JCOEF>* block, int last_dc_val,
    c_derived_tbl *dctbl, c_derived_tbl *actbl)
{
  int temp, temp2;
  int nbits;
  int k, r, i;
  int Se = state->cinfo->lim_Se;
  const be<s32>* natural_order = state->cinfo->natural_order;
  /* Encode the DC coefficient difference per section F.1.2.1 */
  temp = temp2 = block[0] - last_dc_val;
  if (temp < 0) {
    temp = -temp; /* temp is abs value of input */
    /* For a negative input, want temp2 = bitwise complement of abs(input) */
    /* This code assumes we are on a two's complement machine */
    temp2--;
  }
  /* Find the number of bits needed for the magnitude of the coefficient */
  nbits = 0;
  while (temp) {
    nbits++;
    temp >>= 1;
  }
  /* Check for out-of-range coefficient values.
   * Since we're encoding a difference, the range limit is twice as much.
   */
  if (nbits > 10 +1)
    jpeg_error(state->cinfo, JERR_BAD_DCT_COEF);
  /* Emit the Huffman-coded symbol for the number of bits */
  if (! emit_bits_s(state, dctbl->ehufco[nbits], dctbl->ehufsi[nbits]))
    return 0;
  /* Emit that number of bits of the value, if positive, */
  /* or the complement of its magnitude, if negative. */
  if (nbits) /* emit_bits rejects calls with size 0 */
    if (! emit_bits_s(state, (unsigned int) temp2, nbits))
      return 0;
  /* Encode the AC coefficients per section F.1.2.2 */
  r = 0; /* r = run length of zeros */
  for (k = 1; k <= Se; k++) {
    if ((temp = block[natural_order[k]]) == 0) {
      r++;
    } else {
      /* if run length > 15, must emit special run-length-16 codes (0xF0) */
      while (r > 15) {
 if (! emit_bits_s(state, actbl->ehufco[0xF0], actbl->ehufsi[0xF0]))
   return 0;
 r -= 16;
      }
      temp2 = temp;
      if (temp < 0) {
 temp = -temp; /* temp is abs value of input */
 /* This code assumes we are on a two's complement machine */
 temp2--;
      }
      /* Find the number of bits needed for the magnitude of the coefficient */
      nbits = 1; /* there must be at least one 1 bit */
      while ((temp >>= 1))
 nbits++;
      /* Check for out-of-range coefficient values */
      if (nbits > 10)
 jpeg_error(state->cinfo, JERR_BAD_DCT_COEF);
      /* Emit Huffman symbol for run length / number of bits */
      i = (r << 4) + nbits;
      if (! emit_bits_s(state, actbl->ehufco[i], actbl->ehufsi[i]))
 return 0;
      /* Emit that number of bits of the value, if positive, */
      /* or the complement of its magnitude, if negative. */
      if (! emit_bits_s(state, (unsigned int) temp2, nbits))
 return 0;
      r = 0;
    }
  }
  /* If the last coef(s) were zero, emit an end-of-block code */
  if (r > 0)
    if (! emit_bits_s(state, actbl->ehufco[0], actbl->ehufsi[0]))
      return 0;
  return 1;
}
/*
 * Encode and output one MCU's worth of Huffman-compressed coefficients.
 */
/* 028603B4 */
static boolean
encode_mcu_huff (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  huff_encoder_working_state state;
  int blkn, ci;
  jpeg_component_info * compptr;
  /* Load up working state */
  state.next_output_byte = cinfo->dest->next_output_byte;
  state.free_in_buffer = cinfo->dest->free_in_buffer;
  ((state.cur) = (entropy->saved));
  state.cinfo = cinfo;
  /* Emit restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! emit_restart_s(&state, entropy->next_restart_num))
 return 0;
  }
  /* Encode the MCU data blocks */
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];
    if (! encode_one_block(&state,
      MCU_data[blkn][0], state.cur.last_dc_val[ci],
      entropy->dc_derived_tbls[compptr->dc_tbl_no],
      entropy->ac_derived_tbls[compptr->ac_tbl_no]))
      return 0;
    /* Update last_dc_val */
    state.cur.last_dc_val[ci] = MCU_data[blkn][0][0];
  }
  /* Completed MCU, so update state */
  cinfo->dest->next_output_byte = state.next_output_byte;
  cinfo->dest->free_in_buffer = state.free_in_buffer;
  ((entropy->saved) = (state.cur));
  /* Update restart-interval state too */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      entropy->restarts_to_go = cinfo->restart_interval;
      entropy->next_restart_num++;
      entropy->next_restart_num &= 7;
    }
    entropy->restarts_to_go--;
  }
  return 1;
}
/*
 * Finish up at the end of a Huffman-compressed scan.
 */
/* 02860544 */
static void
finish_pass_huff (j_compress_ptr cinfo)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  huff_encoder_working_state state;
  if (cinfo->progressive_mode) {
    entropy->next_output_byte = cinfo->dest->next_output_byte;
    entropy->free_in_buffer = cinfo->dest->free_in_buffer;
    /* Flush out any buffered data */
    emit_eobrun(entropy);
    flush_bits_e(entropy);
    cinfo->dest->next_output_byte = entropy->next_output_byte;
    cinfo->dest->free_in_buffer = entropy->free_in_buffer;
  } else {
    /* Load up working state ... flush_bits needs it */
    state.next_output_byte = cinfo->dest->next_output_byte;
    state.free_in_buffer = cinfo->dest->free_in_buffer;
    ((state.cur) = (entropy->saved));
    state.cinfo = cinfo;
    /* Flush out the last data */
    if (! flush_bits_s(&state))
      jpeg_error(cinfo, JERR_CANT_SUSPEND);
    /* Update state */
    cinfo->dest->next_output_byte = state.next_output_byte;
    cinfo->dest->free_in_buffer = state.free_in_buffer;
    ((entropy->saved) = (state.cur));
  }
}
/*
 * Huffman coding optimization.
 *
 * We first scan the supplied data and count the number of uses of each symbol
 * that is to be Huffman-coded. (This process MUST agree with the code above.)
 * Then we build a Huffman coding tree for the observed counts.
 * Symbols which are not needed at all for the particular image are not
 * assigned any code, which saves space in the DHT marker as well as in
 * the compressed data.
 */
/* Process a single block's worth of coefficients */
/* 0286065C */
static void
htest_one_block (j_compress_ptr cinfo, be<JCOEF>* block, int last_dc_val,
   be<s32>* dc_counts, be<s32>* ac_counts)
{
  int temp;
  int nbits;
  int k, r;
  int Se = cinfo->lim_Se;
  const be<s32>* natural_order = cinfo->natural_order;
  /* Encode the DC coefficient difference per section F.1.2.1 */
  temp = block[0] - last_dc_val;
  if (temp < 0)
    temp = -temp;
  /* Find the number of bits needed for the magnitude of the coefficient */
  nbits = 0;
  while (temp) {
    nbits++;
    temp >>= 1;
  }
  /* Check for out-of-range coefficient values.
   * Since we're encoding a difference, the range limit is twice as much.
   */
  if (nbits > 10 +1)
    jpeg_error(cinfo, JERR_BAD_DCT_COEF);
  /* Count the Huffman symbol for the number of bits */
  dc_counts[nbits]++;
  /* Encode the AC coefficients per section F.1.2.2 */
  r = 0; /* r = run length of zeros */
  for (k = 1; k <= Se; k++) {
    if ((temp = block[natural_order[k]]) == 0) {
      r++;
    } else {
      /* if run length > 15, must emit special run-length-16 codes (0xF0) */
      while (r > 15) {
 ac_counts[0xF0]++;
 r -= 16;
      }
      /* Find the number of bits needed for the magnitude of the coefficient */
      if (temp < 0)
 temp = -temp;
      /* Find the number of bits needed for the magnitude of the coefficient */
      nbits = 1; /* there must be at least one 1 bit */
      while ((temp >>= 1))
 nbits++;
      /* Check for out-of-range coefficient values */
      if (nbits > 10)
 jpeg_error(cinfo, JERR_BAD_DCT_COEF);
      /* Count Huffman symbol for run length / number of bits */
      ac_counts[(r << 4) + nbits]++;
      r = 0;
    }
  }
  /* If the last coef(s) were zero, emit an end-of-block code */
  if (r > 0)
    ac_counts[0]++;
}
/*
 * Trial-encode one MCU's worth of Huffman-compressed coefficients.
 * No data is actually output, so no suspension return is possible.
 */
/* 028607C4 */
static boolean
encode_mcu_gather (j_compress_ptr cinfo, JBLOCKARRAY MCU_data)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int blkn, ci;
  jpeg_component_info * compptr;
  /* Take care of restart intervals if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0) {
      /* Re-initialize DC predictions to 0 */
      for (ci = 0; ci < cinfo->comps_in_scan; ci++)
 entropy->saved.last_dc_val[ci] = 0;
      /* Update restart state */
      entropy->restarts_to_go = cinfo->restart_interval;
    }
    entropy->restarts_to_go--;
  }
  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];
    htest_one_block(cinfo, MCU_data[blkn][0], entropy->saved.last_dc_val[ci],
      entropy->dc_count_ptrs[compptr->dc_tbl_no],
      entropy->ac_count_ptrs[compptr->ac_tbl_no]);
    entropy->saved.last_dc_val[ci] = MCU_data[blkn][0][0];
  }
  return 1;
}
/*
 * Generate the best Huffman code table for the given counts, fill htbl.
 *
 * The JPEG standard requires that no symbol be assigned a codeword of all
 * one bits (so that padding bits added at the end of a compressed segment
 * can't look like a valid code).  Because of the canonical ordering of
 * codewords, this just means that there must be an unused slot in the
 * longest codeword length category.  Section K.2 of the JPEG spec suggests
 * reserving such a slot by pretending that symbol 256 is a valid symbol
 * with count 1.  In theory that's not optimal; giving it count zero but
 * including it in the symbol set anyway should give a better Huffman code.
 * But the theoretically better code actually seems to come out worse in
 * practice, because it produces more all-ones bytes (which incur stuffed
 * zero bytes in the final file).  In any case the difference is tiny.
 *
 * The JPEG standard requires Huffman codes to be no more than 16 bits long.
 * If some symbols have a very small but nonzero probability, the Huffman tree
 * must be adjusted to meet the code length restriction.  We currently use
 * the adjustment method suggested in JPEG section K.2.  This method is *not*
 * optimal; it may not choose the best possible limited-length code.  But
 * typically only very-low-frequency symbols will be given less-than-optimal
 * lengths, so the code is almost optimal.  Experimental comparisons against
 * an optimal limited-length-code algorithm indicate that the difference is
 * microscopic --- usually less than a hundredth of a percent of total size.
 * So the extra complexity of an optimal algorithm doesn't seem worthwhile.
 */
/* 028608C0 */
static void
jpeg_gen_optimal_table (j_compress_ptr cinfo, JHUFF_TBL * htbl, be<s32>* freq)
{
  optimal_huffman_workspace workspace;
  be<u8>* bits = workspace.bits;
  be<s32>* codesize = workspace.codesize;
  be<s32>* others = workspace.others;
  int c1, c2;
  int p, i, j;
  s32 v;
  /* This algorithm is explained in section K.2 of the JPEG standard */
  huff_zero(bits, 33);
  huff_clear_words(codesize, 257);
  for (i = 0; i < 257; i++)
    others[i] = -1; /* init links to empty */
  freq[256] = 1; /* make sure 256 has a nonzero count */
  /* Including the pseudo-symbol 256 in the Huffman procedure guarantees
   * that no real symbol is given code-value of all ones, because 256
   * will be placed last in the largest codeword category.
   */
  /* Huffman's basic algorithm to assign optimal code lengths to symbols */
  for (;;) {
    /* Find the smallest nonzero frequency, set c1 = its symbol */
    /* In case of ties, take the larger symbol number */
    c1 = -1;
    v = 1000000000L;
    for (i = 0; i <= 256; i++) {
      if (freq[i] && freq[i] <= v) {
 v = freq[i];
 c1 = i;
      }
    }
    /* Find the next smallest nonzero frequency, set c2 = its symbol */
    /* In case of ties, take the larger symbol number */
    c2 = -1;
    v = 1000000000L;
    for (i = 0; i <= 256; i++) {
      if (freq[i] && freq[i] <= v && i != c1) {
 v = freq[i];
 c2 = i;
      }
    }
    /* Done if we've merged everything into one frequency */
    if (c2 < 0)
      break;
    /* Else merge the two counts/trees */
    freq[c1] += freq[c2];
    freq[c2] = 0;
    /* Increment the codesize of everything in c1's tree branch */
    codesize[c1]++;
    while (others[c1] >= 0) {
      c1 = others[c1];
      codesize[c1]++;
    }
    others[c1] = c2; /* chain c2 onto c1's tree branch */
    /* Increment the codesize of everything in c2's tree branch */
    codesize[c2]++;
    while (others[c2] >= 0) {
      c2 = others[c2];
      codesize[c2]++;
    }
  }
  /* Now count the number of symbols of each code length */
  for (i = 0; i <= 256; i++) {
    if (codesize[i]) {
      /* The JPEG standard seems to think that this can't happen, */
      /* but I'm paranoid... */
      if (codesize[i] > 32)
 jpeg_error(cinfo, JERR_HUFF_CLEN_OVERFLOW);
      bits[codesize[i]]++;
    }
  }
  /* JPEG doesn't allow symbols with code lengths over 16 bits, so if the pure
   * Huffman procedure assigned any such lengths, we must adjust the coding.
   * Here is what the JPEG spec says about how this next bit works:
   * Since symbols are paired for the longest Huffman code, the symbols are
   * removed from this length category two at a time.  The prefix for the pair
   * (which is one bit shorter) is allocated to one of the pair; then,
   * skipping the BITS entry for that prefix length, a code word from the next
   * shortest nonzero BITS entry is converted into a prefix for two code words
   * one bit longer.
   */
  for (i = 32; i > 16; i--) {
    while (bits[i] > 0) {
      j = i - 2; /* find length of new prefix to be used */
      while (bits[j] == 0)
 j--;
      bits[i] -= 2; /* remove two symbols */
      bits[i-1]++; /* one goes in this length */
      bits[j+1] += 2; /* two new symbols in this length */
      bits[j]--; /* symbol of this length is now a prefix */
    }
  }
  /* Remove the count for the pseudo-symbol 256 from the largest codelength */
  while (bits[i] == 0) /* find largest codelength still in use */
    i--;
  bits[i]--;
  /* Return final symbol counts (only for lengths 0..16) */
  huff_copy(htbl->bits, bits, sizeof(htbl->bits));
  /* Return a list of the symbols sorted by code length */
  /* It's not real clear to me why we don't need to consider the codelength
   * changes made above, but the JPEG spec seems to think this works.
   */
  p = 0;
  for (i = 1; i <= 32; i++) {
    for (j = 0; j <= 255; j++) {
      if (codesize[j] == i) {
 htbl->huffval[p] = (u8) j;
 p++;
      }
    }
  }
  /* Set sent_table 0 so updated table will be written to JPEG file. */
  htbl->sent_table = 0;
}
/*
 * Finish up a statistics-gathering pass and create the new Huffman tables.
 */
/* 02860BA0 */
static void
finish_pass_gather (j_compress_ptr cinfo)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int ci, tbl;
  jpeg_component_info * compptr;
  gptr<JHUFF_TBL>* htblptr;
  huffman_finished_tables finished;
  be<boolean>* did_dc = finished.did_dc;
  be<boolean>* did_ac = finished.did_ac;
  /* It's important not to apply jpeg_gen_optimal_table more than once
   * per table, because it clobbers the input frequency counts!
   */
  if (cinfo->progressive_mode)
    /* Flush out buffered data (all we care about is counting the EOB symbol) */
    emit_eobrun(entropy);
  huff_clear_words(did_dc, 4);
  huff_clear_words(did_ac, 4);
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* DC needs no table for refinement scan */
    if (cinfo->Ss == 0 && cinfo->Ah == 0) {
      tbl = compptr->dc_tbl_no;
      if (! did_dc[tbl]) {
 htblptr = & cinfo->dc_huff_tbl_ptrs[tbl];
 if (*htblptr == nullptr)
   *htblptr = jpeg_alloc_huff_table((j_common_ptr) cinfo);
 jpeg_gen_optimal_table(cinfo, *htblptr, entropy->dc_count_ptrs[tbl]);
 did_dc[tbl] = 1;
      }
    }
    /* AC needs no table when not present */
    if (cinfo->Se) {
      tbl = compptr->ac_tbl_no;
      if (! did_ac[tbl]) {
 htblptr = & cinfo->ac_huff_tbl_ptrs[tbl];
 if (*htblptr == nullptr)
   *htblptr = jpeg_alloc_huff_table((j_common_ptr) cinfo);
 jpeg_gen_optimal_table(cinfo, *htblptr, entropy->ac_count_ptrs[tbl]);
 did_ac[tbl] = 1;
      }
    }
  }
}
/*
 * Initialize for a Huffman-compressed scan.
 * If gather_statistics is 1, we do not output anything during the scan,
 * just count the Huffman symbols used and generate Huffman code tables.
 */
/* 02860CE8 */
static void
start_pass_huff (j_compress_ptr cinfo, boolean gather_statistics)
{
  huff_entropy_encoder * entropy = (huff_entropy_encoder *) cinfo->entropy.get();
  int ci, tbl;
  jpeg_component_info * compptr;
  if (gather_statistics)
    entropy->pub.finish_pass = finish_pass_gather;
  else
    entropy->pub.finish_pass = finish_pass_huff;
  if (cinfo->progressive_mode) {
    entropy->cinfo = cinfo;
    entropy->gather_statistics = gather_statistics;
    /* We assume jcmaster.c already validated the scan parameters. */
    /* Select execution routine */
    if (cinfo->Ah == 0) {
      if (cinfo->Ss == 0)
 entropy->pub.encode_mcu = encode_mcu_DC_first;
      else
 entropy->pub.encode_mcu = encode_mcu_AC_first;
    } else {
      if (cinfo->Ss == 0)
 entropy->pub.encode_mcu = encode_mcu_DC_refine;
      else {
 entropy->pub.encode_mcu = encode_mcu_AC_refine;
 /* AC refinement needs a correction bit buffer */
 if (entropy->bit_buffer == nullptr)
   entropy->bit_buffer = (be<u8>*)
     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
     1000 * sizeof(char));
      }
    }
    /* Initialize AC stuff */
    entropy->ac_tbl_no = cinfo->cur_comp_info[0]->ac_tbl_no;
    entropy->EOBRUN = 0;
    entropy->BE = 0;
  } else {
    if (gather_statistics)
      entropy->pub.encode_mcu = encode_mcu_gather;
    else
      entropy->pub.encode_mcu = encode_mcu_huff;
  }
  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* DC needs no table for refinement scan */
    if (cinfo->Ss == 0 && cinfo->Ah == 0) {
      tbl = compptr->dc_tbl_no;
      if (gather_statistics) {
 /* Check for invalid table index */
 /* (make_c_derived_tbl does this in the other path) */
 if (tbl < 0 || tbl >= NUM_HUFF_TBLS)
   jpeg_error1(cinfo, JERR_NO_HUFF_TABLE, tbl);
 /* Allocate and zero the statistics tables */
 /* Note that jpeg_gen_optimal_table expects 257 entries in each table! */
 if (entropy->dc_count_ptrs[tbl] == nullptr)
   entropy->dc_count_ptrs[tbl] = (be<s32>*)
     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
     257 * sizeof(s32));
 huff_clear_words(entropy->dc_count_ptrs[tbl], 257);
      } else {
 /* Compute derived values for Huffman tables */
 /* We may do this more than once for a table, but it's not expensive */
 jpeg_make_c_derived_tbl(cinfo, 1, tbl,
    & entropy->dc_derived_tbls[tbl]);
      }
      /* Initialize DC predictions to 0 */
      entropy->saved.last_dc_val[ci] = 0;
    }
    /* AC needs no table when not present */
    if (cinfo->Se) {
      tbl = compptr->ac_tbl_no;
      if (gather_statistics) {
 if (tbl < 0 || tbl >= NUM_HUFF_TBLS)
   jpeg_error1(cinfo, JERR_NO_HUFF_TABLE, tbl);
 if (entropy->ac_count_ptrs[tbl] == nullptr)
   entropy->ac_count_ptrs[tbl] = (be<s32>*)
     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
     257 * sizeof(s32));
 huff_clear_words(entropy->ac_count_ptrs[tbl], 257);
      } else {
 jpeg_make_c_derived_tbl(cinfo, 0, tbl,
    & entropy->ac_derived_tbls[tbl]);
      }
    }
  }
  /* Initialize bit buffer to empty */
  entropy->saved.put_buffer = 0;
  entropy->saved.put_bits = 0;
  /* Initialize restart stuff */
  entropy->restarts_to_go = cinfo->restart_interval;
  entropy->next_restart_num = 0;
}
/*
 * Module initialization routine for Huffman entropy encoding.
 */
/* 02860FF4 */
void
jinit_huff_encoder (j_compress_ptr cinfo)
{
  huff_entropy_encoder * entropy;
  int i;
  entropy = (huff_entropy_encoder *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
    sizeof(huff_entropy_encoder));
  cinfo->entropy = (struct jpeg_entropy_encoder *) entropy;
  entropy->pub.start_pass = start_pass_huff;
  /* Mark tables unallocated */
  for (i = 0; i < NUM_HUFF_TBLS; i++) {
    entropy->dc_derived_tbls[i] = entropy->ac_derived_tbls[i] = nullptr;
    entropy->dc_count_ptrs[i] = entropy->ac_count_ptrs[i] = nullptr;
  }
  if (cinfo->progressive_mode)
    entropy->bit_buffer = nullptr; /* needed only in AC refinement scan */
}

/* WWHD glue
*/
