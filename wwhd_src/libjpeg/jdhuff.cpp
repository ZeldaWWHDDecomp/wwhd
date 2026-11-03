/*
 * jdhuff.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2006-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains Huffman entropy decoding routines.
 * Both sequential and progressive modes are supported in this single module.
 *
 * Much of the complexity here has to do with supporting input suspension.
 * If the data source module demands suspension, we want to be able to back
 * up to the start of the current MCU.  To do this, we copy state variables
 * into local working storage, and update them back to the permanent
 * storage only upon successful completion of an MCU.
 */
// Decompiled WWHD build: typed guest state and explicit suspend/rollback paths.
// IJG gotos retain shared slow decode, exhausted-input, and end-of-block exits;
// undoit rolls back partially refined coefficients after input suspension.
// The bad label shares progressive-parameter error reporting after validation.
#include "libjpeg/jdhuff.h"
static const s32 jpeg_zigzag_order[8][8] = {
  {  0,  1,  5,  6, 14, 15, 27, 28 },
  {  2,  4,  7, 13, 16, 26, 29, 42 },
  {  3,  8, 12, 17, 25, 30, 41, 43 },
  {  9, 11, 18, 24, 31, 40, 44, 53 },
  { 10, 19, 23, 32, 39, 45, 52, 54 },
  { 20, 22, 33, 38, 46, 51, 55, 60 },
  { 21, 34, 37, 47, 50, 56, 59, 61 },
  { 35, 36, 48, 49, 57, 58, 62, 63 }
};
static const s32 jpeg_zigzag_order7[7][7] = {
  {  0,  1,  5,  6, 14, 15, 27 },
  {  2,  4,  7, 13, 16, 26, 28 },
  {  3,  8, 12, 17, 25, 29, 38 },
  {  9, 11, 18, 24, 30, 37, 39 },
  { 10, 19, 23, 31, 36, 40, 45 },
  { 20, 22, 32, 35, 41, 44, 46 },
  { 21, 33, 34, 42, 43, 47, 48 }
};
static const s32 jpeg_zigzag_order6[6][6] = {
  {  0,  1,  5,  6, 14, 15 },
  {  2,  4,  7, 13, 16, 25 },
  {  3,  8, 12, 17, 24, 26 },
  {  9, 11, 18, 23, 27, 32 },
  { 10, 19, 22, 28, 31, 33 },
  { 20, 21, 29, 30, 34, 35 }
};
static const s32 jpeg_zigzag_order5[5][5] = {
  {  0,  1,  5,  6, 14 },
  {  2,  4,  7, 13, 15 },
  {  3,  8, 12, 16, 21 },
  {  9, 11, 17, 20, 22 },
  { 10, 18, 19, 23, 24 }
};
static const s32 jpeg_zigzag_order4[4][4] = {
  { 0,  1,  5,  6 },
  { 2,  4,  7, 12 },
  { 3,  8, 11, 13 },
  { 9, 10, 14, 15 }
};
static const s32 jpeg_zigzag_order3[3][3] = {
  { 0, 1, 5 },
  { 2, 4, 6 },
  { 3, 7, 8 }
};
static const s32 jpeg_zigzag_order2[2][2] = {
  { 0, 1 },
  { 2, 3 }
};
static const s32 bmask[16] =	/* bmask[n] is mask for n rightmost bits */
  { 0, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF,
    0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF };
static const s32 jpeg_natural_order[80] = {
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
#define BITREAD_LOAD_STATE(cinfop,permstate)  \
	br_state.cinfo = cinfop; \
	br_state.next_input_byte = cinfop->src->next_input_byte; \
	br_state.bytes_in_buffer = cinfop->src->bytes_in_buffer; \
	get_buffer = permstate.get_buffer; \
	bits_left = permstate.bits_left;

#define BITREAD_SAVE_STATE(cinfop,permstate)  \
	cinfop->src->next_input_byte = br_state.next_input_byte; \
	cinfop->src->bytes_in_buffer = br_state.bytes_in_buffer; \
	permstate.get_buffer = get_buffer; \
	permstate.bits_left = bits_left

/*
 * These macros provide the in-line portion of bit fetching.
 * Use CHECK_BIT_BUFFER to ensure there are N bits in get_buffer
 * before using GET_BITS, PEEK_BITS, or DROP_BITS.
 * The variables get_buffer and bits_left are assumed to be locals,
 * but the state struct might not be (jpeg_huff_decode needs this).
 *	CHECK_BIT_BUFFER(state,n,action);
 *		Ensure there are N bits in get_buffer; if suspend, take action.
 *      val = GET_BITS(n);
 *		Fetch next N bits.
 *      val = PEEK_BITS(n);
 *		Fetch next N bits without removing them from the buffer.
 *	DROP_BITS(n);
 *		Discard next N bits.
 * The value N should be a simple variable, not an expression, because it
 * is evaluated multiple times.
 */

#define CHECK_BIT_BUFFER(state,nbits,action) \
	{ if (bits_left < (nbits)) {  \
	    if (! jpeg_fill_bit_buffer(&(state),get_buffer,bits_left,nbits))  \
	      { action; }  \
	    get_buffer = (state).get_buffer; bits_left = (state).bits_left; } }

#define GET_BITS(nbits) \
	(((int) (get_buffer >> (bits_left -= (nbits)))) & BIT_MASK(nbits))

#define PEEK_BITS(nbits) \
	(((int) (get_buffer >> (bits_left -  (nbits)))) & BIT_MASK(nbits))

#define DROP_BITS(nbits) \
	(bits_left -= (nbits))


/*
 * Code for extracting next Huffman-coded symbol from input bit stream.
 * Again, this is time-critical and we make the main paths be macros.
 *
 * We use a lookahead table to process codes of up to HUFF_LOOKAHEAD bits
 * without looping.  Usually, more than 95% of the Huffman codes will be 8
 * or fewer bits long.  The few overlength codes are handled with a loop,
 * which need not be inline code.
 *
 * Notes about the HUFF_DECODE macro:
 * 1. Near the end of the data segment, we may fail to get enough bits
 *    for a lookahead.  In that case, we do it the hard way.
 * 2. If the lookahead table contains no entry, the next code must be
 *    more than HUFF_LOOKAHEAD bits long.
 * 3. jpeg_huff_decode returns -1 if forced to suspend.
 */

#define HUFF_DECODE(result,state,htbl,failaction,slowlabel) \
{ int nb, look; \
  if (bits_left < HUFF_LOOKAHEAD) { \
    if (! jpeg_fill_bit_buffer(&state,get_buffer,bits_left, 0)) {failaction;} \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
    if (bits_left < HUFF_LOOKAHEAD) { \
      nb = 1; goto slowlabel; \
    } \
  } \
  look = PEEK_BITS(HUFF_LOOKAHEAD); \
  if ((nb = htbl->look_nbits[look]) != 0) { \
    DROP_BITS(nb); \
    result = htbl->look_sym[look]; \
  } else { \
    nb = HUFF_LOOKAHEAD+1; \
slowlabel: \
    if ((result=jpeg_huff_decode(&state,get_buffer,bits_left,htbl,nb)) < 0) \
	{ failaction; } \
    get_buffer = state.get_buffer; bits_left = state.bits_left; \
  } \
}



#define BIT_MASK(nbits) bmask[nbits]
#define HUFF_EXTEND(x,s) ((x) <= bmask[(s)-1] ? (x)-bmask[s] : (x))

/* 02863C20 */
static void jpeg_make_d_derived_tbl (j_decompress_ptr cinfo, boolean isDC, int tblno,
			 gptr<d_derived_tbl>* pdtbl)
{
  JHUFF_TBL *htbl;
  d_derived_tbl *dtbl;
  int p, i, l, si, numsymbols;
  int lookbits, ctr;
  char huffsize[257];
  unsigned int huffcode[257];
  unsigned int code;

  /* Note that huffsize[] and huffcode[] are filled in code-length order,
   * paralleling the order of the symbols themselves in htbl->huffval[].
   */

  /* Find the input Huffman table */
  if (tblno < 0 || tblno >= NUM_HUFF_TBLS)
    ERREXIT1(cinfo, JERR_NO_HUFF_TABLE, tblno);
  htbl =
    isDC ? cinfo->dc_huff_tbl_ptrs[tblno] : cinfo->ac_huff_tbl_ptrs[tblno];
  if (htbl == NULL)
    ERREXIT1(cinfo, JERR_NO_HUFF_TABLE, tblno);

  /* Allocate a workspace if we haven't already done so. */
  if (*pdtbl == NULL)
    *pdtbl = (d_derived_tbl *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(d_derived_tbl));
  dtbl = *pdtbl;
  dtbl->pub = htbl;		/* fill in back link */
  
  /* Figure C.1: make table of Huffman code length for each symbol */

  p = 0;
  for (l = 1; l <= 16; l++) {
    i = (int) htbl->bits[l];
    if (i < 0 || p + i > 256)	/* protect against table overrun */
      ERREXIT(cinfo, JERR_BAD_HUFF_TABLE);
    while (i--)
      huffsize[p++] = (char) l;
  }
  huffsize[p] = 0;
  numsymbols = p;
  
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
    if (((INT32) code) >= (((INT32) 1) << si))
      ERREXIT(cinfo, JERR_BAD_HUFF_TABLE);
    code <<= 1;
    si++;
  }

  /* Figure F.15: generate decoding tables for bit-sequential decoding */

  p = 0;
  for (l = 1; l <= 16; l++) {
    if (htbl->bits[l]) {
      /* valoffset[l] = huffval[] index of 1st symbol of code length l,
       * minus the minimum code of length l
       */
      dtbl->valoffset[l] = (INT32) p - (INT32) huffcode[p];
      p += htbl->bits[l];
      dtbl->maxcode[l] = huffcode[p-1]; /* maximum code of length l */
    } else {
      dtbl->maxcode[l] = -1;	/* -1 if no codes of this length */
    }
  }
  dtbl->maxcode[17] = 0xFFFFFL; /* ensures jpeg_huff_decode terminates */

  /* Compute lookahead tables to speed up decoding.
   * First we set all the table entries to 0, indicating "too long";
   * then we iterate through the Huffman codes that are short enough and
   * fill in all the entries that correspond to bit sequences starting
   * with that code.
   */

  huff_clear_words(dtbl->look_nbits,256);

  p = 0;
  for (l = 1; l <= HUFF_LOOKAHEAD; l++) {
    for (i = 1; i <= (int) htbl->bits[l]; i++, p++) {
      /* l = current code's length, p = its index in huffcode[] & huffval[]. */
      /* Generate left-justified code followed by all possible bit sequences */
      lookbits = huffcode[p] << (HUFF_LOOKAHEAD-l);
      for (ctr = 1 << (HUFF_LOOKAHEAD-l); ctr > 0; ctr--) {
	dtbl->look_nbits[lookbits] = l;
	dtbl->look_sym[lookbits] = htbl->huffval[p];
	lookbits++;
      }
    }
  }

  /* Validate symbols as being reasonable.
   * For AC tables, we make no check, but accept all byte values 0..255.
   * For DC tables, we require the symbols to be in range 0..15.
   * (Tighter bounds could be applied depending on the data depth and mode,
   * but this is sufficient to ensure safe decoding.)
   */
  if (isDC) {
    for (i = 0; i < numsymbols; i++) {
      int sym = htbl->huffval[i];
      if (sym < 0 || sym > 15)
	ERREXIT(cinfo, JERR_BAD_HUFF_TABLE);
    }
  }
}

/* 02863F2C */
static boolean jpeg_fill_bit_buffer (bitread_working_state * state,
		      bit_buf_type get_buffer, int bits_left,
		      int nbits)
/* Load up the bit buffer to a depth of at least nbits */
{
  /* Copy heavily used state fields into locals (hopefully registers) */
  const be<JOCTET>* next_input_byte = state->next_input_byte;
  u32 bytes_in_buffer = state->bytes_in_buffer;
  j_decompress_ptr cinfo = state->cinfo;

  /* Attempt to load at least MIN_GET_BITS bits into get_buffer. */
  /* (It is assumed that no request will be for more than that many bits.) */
  /* We fail to do so only if we hit a marker or are forced to suspend. */

  if (cinfo->unread_marker == 0) {	/* cannot advance past a marker */
    while (bits_left < MIN_GET_BITS) {
      int c;

      /* Attempt to read a byte */
      if (bytes_in_buffer == 0) {
	if (! (*cinfo->src->fill_input_buffer) (cinfo))
	  return FALSE;
	next_input_byte = cinfo->src->next_input_byte;
	bytes_in_buffer = cinfo->src->bytes_in_buffer;
      }
      bytes_in_buffer--;
      c = GETJOCTET(*next_input_byte++);

      /* If it's 0xFF, check and discard stuffed zero byte */
      if (c == 0xFF) {
	/* Loop here to discard any padding FF's on terminating marker,
	 * so that we can save a valid unread_marker value.  NOTE: we will
	 * accept multiple FF's followed by a 0 as meaning a single FF data
	 * byte.  This data pattern is not valid according to the standard.
	 */
	do {
	  if (bytes_in_buffer == 0) {
	    if (! (*cinfo->src->fill_input_buffer) (cinfo))
	      return FALSE;
	    next_input_byte = cinfo->src->next_input_byte;
	    bytes_in_buffer = cinfo->src->bytes_in_buffer;
	  }
	  bytes_in_buffer--;
	  c = GETJOCTET(*next_input_byte++);
	} while (c == 0xFF);

	if (c == 0) {
	  /* Found FF/00, which represents an FF data byte */
	  c = 0xFF;
	} else {
	  /* Oops, it's actually a marker indicating end of compressed data.
	   * Save the marker code for later use.
	   * Fine point: it might appear that we should save the marker into
	   * bitread working state, not straight into permanent state.  But
	   * once we have hit a marker, we cannot need to suspend within the
	   * current MCU, because we will read no more bytes from the data
	   * source.  So it is OK to update permanent state right away.
	   */
	  cinfo->unread_marker = c;
	  /* See if we need to insert some fake zero bits. */
	  goto no_more_bytes;
	}
      }

      /* OK, load c into get_buffer */
      get_buffer = (get_buffer << 8) | c;
      bits_left += 8;
    } /* end while */
  } else {
  no_more_bytes:
    /* We get here if we've read the marker that terminates the compressed
     * data segment.  There should be enough bits in the buffer register
     * to satisfy the request; if so, no problem.
     */
    if (nbits > bits_left) {
      /* Uh-oh.  Report corrupted data to user and stuff zeroes into
       * the data stream, so that we can produce some kind of image.
       * We use a nonvolatile flag to ensure that only one warning message
       * appears per data segment.
       */
      if (! ((huff_entropy_ptr) cinfo->entropy.get())->insufficient_data) {
	WARNMS(cinfo, JWRN_HIT_MARKER);
	((huff_entropy_ptr) cinfo->entropy.get())->insufficient_data = TRUE;
      }
      /* Fill the buffer with zero bits */
      get_buffer <<= MIN_GET_BITS - bits_left;
      bits_left = MIN_GET_BITS;
    }
  }

  /* Unload the local registers */
  state->next_input_byte = next_input_byte;
  state->bytes_in_buffer = bytes_in_buffer;
  state->get_buffer = get_buffer;
  state->bits_left = bits_left;

  return TRUE;
}

/* 028640C4 */
static int jpeg_huff_decode (bitread_working_state * state,
		  bit_buf_type get_buffer, int bits_left,
		  d_derived_tbl * htbl, int min_bits)
{
  int l = min_bits;
  INT32 code;

  /* HUFF_DECODE has determined that the code is at least min_bits */
  /* bits long, so fetch that many bits in one swoop. */

  CHECK_BIT_BUFFER(*state, l, return -1);
  code = GET_BITS(l);

  /* Collect the rest of the Huffman code one bit at a time. */
  /* This is per Figure F.16 in the JPEG spec. */

  while (code > htbl->maxcode[l]) {
    code <<= 1;
    CHECK_BIT_BUFFER(*state, 1, return -1);
    code |= GET_BITS(1);
    l++;
  }

  /* Unload the local registers */
  state->get_buffer = get_buffer;
  state->bits_left = bits_left;

  /* With garbage input we may reach the sentinel value l = 17. */

  if (l > 16) {
    WARNMS(state->cinfo, JWRN_HUFF_BAD_CODE);
    return 0;			/* fake a zero as the safest result */
  }

  return htbl->pub->huffval[ (int) (code + htbl->valoffset[l]) ];
}

/* 02864234 */
static boolean process_restart (j_decompress_ptr cinfo)
{
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int ci;

  /* Throw away any unused bits remaining in bit buffer; */
  /* include any full bytes in next_marker's count of discarded bytes */
  cinfo->marker->discarded_bytes += entropy->bitstate.bits_left / 8;
  entropy->bitstate.bits_left = 0;

  /* Advance past the RSTn marker */
  if (! (*cinfo->marker->read_restart_marker) (cinfo))
    return FALSE;

  /* Re-initialize DC predictions to 0 */
  for (ci = 0; ci < cinfo->comps_in_scan; ci++)
    entropy->saved.last_dc_val[ci] = 0;
  /* Re-init EOB run count, too */
  entropy->saved.EOBRUN = 0;

  /* Reset restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;

  /* Reset out-of-data flag, unless read_restart_marker left us smack up
   * against a marker.  In that case we will end up treating the next data
   * segment as empty, and we can avoid producing bogus output pixels by
   * leaving the flag set.
   */
  if (cinfo->unread_marker == 0)
    entropy->insufficient_data = FALSE;

  return TRUE;
}

/* 02864310 */
static boolean decode_mcu_DC_first (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{   
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int Al = cinfo->Al;
  int s, r;
  int blkn, ci;
  JBLOCKROW block;
  BITREAD_STATE_VARS;
  gabi::Local<savable_state> state_storage; savable_state& state=*state_storage;
  d_derived_tbl * tbl;
  jpeg_component_info * compptr;

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* If we've run out of data, just leave the MCU set to zeroes.
   * This way, we return uniform gray for the remainder of the segment.
   */
  if (! entropy->insufficient_data) {

    /* Load up working state */
    BITREAD_LOAD_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(state, entropy->saved);

    /* Outer loop handles each block in the MCU */

    for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
      block = MCU_data[blkn];
      ci = cinfo->MCU_membership[blkn];
      compptr = cinfo->cur_comp_info[ci];
      tbl = entropy->derived_tbls[compptr->dc_tbl_no];

      /* Decode a single block's worth of coefficients */

      /* Section F.2.2.1: decode the DC coefficient difference */
      HUFF_DECODE(s, br_state, tbl, return FALSE, label1);
      if (s) {
	CHECK_BIT_BUFFER(br_state, s, return FALSE);
	r = GET_BITS(s);
	s = HUFF_EXTEND(r, s);
      }

      /* Convert DC difference to actual value, update last_dc_val */
      s += state.last_dc_val[ci];
      state.last_dc_val[ci] = s;
      /* Scale and output the coefficient (assumes jpeg_natural_order[0]=0) */
      (*block)[0] = (JCOEF) (s << Al);
    }

    /* Completed MCU, so update state */
    BITREAD_SAVE_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(entropy->saved, state);
  }

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;
}

/* 0286456C */
static boolean decode_mcu_AC_first (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{   
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int s, k, r;
  unsigned int EOBRUN;
  int Se, Al;
  const be<s32>* natural_order;
  JBLOCKROW block;
  BITREAD_STATE_VARS;
  d_derived_tbl * tbl;

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* If we've run out of data, just leave the MCU set to zeroes.
   * This way, we return uniform gray for the remainder of the segment.
   */
  if (! entropy->insufficient_data) {

    Se = cinfo->Se;
    Al = cinfo->Al;
    natural_order = cinfo->natural_order;

    /* Load up working state.
     * We can avoid loading/saving bitread state if in an EOB run.
     */
    EOBRUN = entropy->saved.EOBRUN;	/* only part of saved state we need */

    /* There is always only one block per MCU */

    if (EOBRUN > 0)		/* if it's a band of zeroes... */
      EOBRUN--;			/* ...process it now (we do nothing) */
    else {
      BITREAD_LOAD_STATE(cinfo,entropy->bitstate);
      block = MCU_data[0];
      tbl = entropy->ac_derived_tbl;

      for (k = cinfo->Ss; k <= Se; k++) {
	HUFF_DECODE(s, br_state, tbl, return FALSE, label2);
	r = s >> 4;
	s &= 15;
	if (s) {
	  k += r;
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  r = GET_BITS(s);
	  s = HUFF_EXTEND(r, s);
	  /* Scale and output coefficient in natural (dezigzagged) order */
	  (*block)[natural_order[k]] = (JCOEF) (s << Al);
	} else {
	  if (r == 15) {	/* ZRL */
	    k += 15;		/* skip 15 zeroes in band */
	  } else {		/* EOBr, run length is 2^r + appended bits */
	    EOBRUN = 1 << r;
	    if (r) {		/* EOBr, r > 0 */
	      CHECK_BIT_BUFFER(br_state, r, return FALSE);
	      r = GET_BITS(r);
	      EOBRUN += r;
	    }
	    EOBRUN--;		/* this band is processed at this moment */
	    break;		/* force end-of-band */
	  }
	}
      }

      BITREAD_SAVE_STATE(cinfo,entropy->bitstate);
    }

    /* Completed MCU, so update state */
    entropy->saved.EOBRUN = EOBRUN;	/* only part of saved state we need */
  }

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;
}

/* 0286483C */
static boolean decode_mcu_DC_refine (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{   
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int p1 = 1 << cinfo->Al;	/* 1 in the bit position being coded */
  int blkn;
  JBLOCKROW block;
  BITREAD_STATE_VARS;

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* Not worth the cycles to check insufficient_data here,
   * since we will not change the data anyway if we read zeroes.
   */

  /* Load up working state */
  BITREAD_LOAD_STATE(cinfo,entropy->bitstate);

  /* Outer loop handles each block in the MCU */

  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];

    /* Encoded data is simply the next bit of the two's-complement DC value */
    CHECK_BIT_BUFFER(br_state, 1, return FALSE);
    if (GET_BITS(1))
      (*block)[0] |= p1;
    /* Note: since we use |=, repeating the assignment later is safe */
  }

  /* Completed MCU, so update state */
  BITREAD_SAVE_STATE(cinfo,entropy->bitstate);

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;
}

/* 0286496C */
static boolean decode_mcu_AC_refine (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{   
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int s, k, r;
  unsigned int EOBRUN;
  int Se, p1, m1;
  const be<s32>* natural_order;
  JBLOCKROW block;
  JCOEFPTR thiscoef;
  BITREAD_STATE_VARS;
  d_derived_tbl * tbl;
  int num_newnz;
  int newnz_pos[DCTSIZE2];

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* If we've run out of data, don't modify the MCU.
   */
  if (! entropy->insufficient_data) {

    Se = cinfo->Se;
    p1 = 1 << cinfo->Al;	/* 1 in the bit position being coded */
    m1 = (-1) << cinfo->Al;	/* -1 in the bit position being coded */
    natural_order = cinfo->natural_order;

    /* Load up working state */
    BITREAD_LOAD_STATE(cinfo,entropy->bitstate);
    EOBRUN = entropy->saved.EOBRUN; /* only part of saved state we need */

    /* There is always only one block per MCU */
    block = MCU_data[0];
    tbl = entropy->ac_derived_tbl;

    /* If we are forced to suspend, we must undo the assignments to any newly
     * nonzero coefficients in the block, because otherwise we'd get confused
     * next time about which coefficients were already nonzero.
     * But we need not undo addition of bits to already-nonzero coefficients;
     * instead, we can test the current bit to see if we already did it.
     */
    num_newnz = 0;

    /* initialize coefficient loop counter to start of band */
    k = cinfo->Ss;

    if (EOBRUN == 0) {
      for (; k <= Se; k++) {
	HUFF_DECODE(s, br_state, tbl, goto undoit, label3);
	r = s >> 4;
	s &= 15;
	if (s) {
	  if (s != 1)		/* size of new coef should always be 1 */
	    WARNMS(cinfo, JWRN_HUFF_BAD_CODE);
	  CHECK_BIT_BUFFER(br_state, 1, goto undoit);
	  if (GET_BITS(1))
	    s = p1;		/* newly nonzero coef is positive */
	  else
	    s = m1;		/* newly nonzero coef is negative */
	} else {
	  if (r != 15) {
	    EOBRUN = 1 << r;	/* EOBr, run length is 2^r + appended bits */
	    if (r) {
	      CHECK_BIT_BUFFER(br_state, r, goto undoit);
	      r = GET_BITS(r);
	      EOBRUN += r;
	    }
	    break;		/* rest of block is handled by EOB logic */
	  }
	  /* note s = 0 for processing ZRL */
	}
	/* Advance over already-nonzero coefs and r still-zero coefs,
	 * appending correction bits to the nonzeroes.  A correction bit is 1
	 * if the absolute value of the coefficient must be increased.
	 */
	do {
	  thiscoef = *block + natural_order[k];
	  if (*thiscoef != 0) {
	    CHECK_BIT_BUFFER(br_state, 1, goto undoit);
	    if (GET_BITS(1)) {
	      if ((*thiscoef & p1) == 0) { /* do nothing if already set it */
		if (*thiscoef >= 0)
		  *thiscoef += p1;
		else
		  *thiscoef += m1;
	      }
	    }
	  } else {
	    if (--r < 0)
	      break;		/* reached target zero coefficient */
	  }
	  k++;
	} while (k <= Se);
	if (s) {
	  int pos = natural_order[k];
	  /* Output newly nonzero coefficient */
	  (*block)[pos] = (JCOEF) s;
	  /* Remember its position in case we have to suspend */
	  newnz_pos[num_newnz++] = pos;
	}
      }
    }

    if (EOBRUN > 0) {
      /* Scan any remaining coefficient positions after the end-of-band
       * (the last newly nonzero coefficient, if any).  Append a correction
       * bit to each already-nonzero coefficient.  A correction bit is 1
       * if the absolute value of the coefficient must be increased.
       */
      for (; k <= Se; k++) {
	thiscoef = *block + natural_order[k];
	if (*thiscoef != 0) {
	  CHECK_BIT_BUFFER(br_state, 1, goto undoit);
	  if (GET_BITS(1)) {
	    if ((*thiscoef & p1) == 0) { /* do nothing if already changed it */
	      if (*thiscoef >= 0)
		*thiscoef += p1;
	      else
		*thiscoef += m1;
	    }
	  }
	}
      }
      /* Count one block completed in EOB run */
      EOBRUN--;
    }

    /* Completed MCU, so update state */
    BITREAD_SAVE_STATE(cinfo,entropy->bitstate);
    entropy->saved.EOBRUN = EOBRUN; /* only part of saved state we need */
  }

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;

undoit:
  /* Re-zero any output coefficients that we made newly nonzero */
  while (num_newnz > 0)
    (*block)[newnz_pos[--num_newnz]] = 0;

  return FALSE;
}

/* 02864DE4 */
static boolean decode_mcu_sub (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  const be<s32>* natural_order;
  int Se, blkn;
  BITREAD_STATE_VARS;
  gabi::Local<savable_state> state_storage; savable_state& state=*state_storage;

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* If we've run out of data, just leave the MCU set to zeroes.
   * This way, we return uniform gray for the remainder of the segment.
   */
  if (! entropy->insufficient_data) {

    natural_order = cinfo->natural_order;
    Se = cinfo->lim_Se;

    /* Load up working state */
    BITREAD_LOAD_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(state, entropy->saved);

    /* Outer loop handles each block in the MCU */

    for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
      JBLOCKROW block = MCU_data[blkn];
      d_derived_tbl * htbl;
      int s, k, r;
      int coef_limit, ci;

      /* Decode a single block's worth of coefficients */

      /* Section F.2.2.1: decode the DC coefficient difference */
      htbl = entropy->dc_cur_tbls[blkn];
      HUFF_DECODE(s, br_state, htbl, return FALSE, label1);

      htbl = entropy->ac_cur_tbls[blkn];
      k = 1;
      coef_limit = entropy->coef_limit[blkn];
      if (coef_limit) {
	/* Convert DC difference to actual value, update last_dc_val */
	if (s) {
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  r = GET_BITS(s);
	  s = HUFF_EXTEND(r, s);
	}
	ci = cinfo->MCU_membership[blkn];
	s += state.last_dc_val[ci];
	state.last_dc_val[ci] = s;
	/* Output the DC coefficient */
	(*block)[0] = (JCOEF) s;

	/* Section F.2.2.2: decode the AC coefficients */
	/* Since zeroes are skipped, output area must be cleared beforehand */
	for (; k < coef_limit; k++) {
	  HUFF_DECODE(s, br_state, htbl, return FALSE, label2);

	  r = s >> 4;
	  s &= 15;

	  if (s) {
	    k += r;
	    CHECK_BIT_BUFFER(br_state, s, return FALSE);
	    r = GET_BITS(s);
	    s = HUFF_EXTEND(r, s);
	    /* Output coefficient in natural (dezigzagged) order.
	     * Note: the extra entries in natural_order[] will save us
	     * if k > Se, which could happen if the data is corrupted.
	     */
	    (*block)[natural_order[k]] = (JCOEF) s;
	  } else {
	    if (r != 15)
	      goto EndOfBlock;
	    k += 15;
	  }
	}
      } else {
	if (s) {
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  DROP_BITS(s);
	}
      }

      /* Section F.2.2.2: decode the AC coefficients */
      /* In this path we just discard the values */
      for (; k <= Se; k++) {
	HUFF_DECODE(s, br_state, htbl, return FALSE, label3);

	r = s >> 4;
	s &= 15;

	if (s) {
	  k += r;
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  DROP_BITS(s);
	} else {
	  if (r != 15)
	    break;
	  k += 15;
	}
      }

      EndOfBlock: ;
    }

    /* Completed MCU, so update state */
    BITREAD_SAVE_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(entropy->saved, state);
  }

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;
}

/* 02865310 */
static boolean decode_mcu (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data)
{
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int blkn;
  BITREAD_STATE_VARS;
  gabi::Local<savable_state> state_storage; savable_state& state=*state_storage;

  /* Process restart marker if needed; may have to suspend */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      if (! process_restart(cinfo))
	return FALSE;
  }

  /* If we've run out of data, just leave the MCU set to zeroes.
   * This way, we return uniform gray for the remainder of the segment.
   */
  if (! entropy->insufficient_data) {

    /* Load up working state */
    BITREAD_LOAD_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(state, entropy->saved);

    /* Outer loop handles each block in the MCU */

    for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
      JBLOCKROW block = MCU_data[blkn];
      d_derived_tbl * htbl;
      int s, k, r;
      int coef_limit, ci;

      /* Decode a single block's worth of coefficients */

      /* Section F.2.2.1: decode the DC coefficient difference */
      htbl = entropy->dc_cur_tbls[blkn];
      HUFF_DECODE(s, br_state, htbl, return FALSE, label1);

      htbl = entropy->ac_cur_tbls[blkn];
      k = 1;
      coef_limit = entropy->coef_limit[blkn];
      if (coef_limit) {
	/* Convert DC difference to actual value, update last_dc_val */
	if (s) {
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  r = GET_BITS(s);
	  s = HUFF_EXTEND(r, s);
	}
	ci = cinfo->MCU_membership[blkn];
	s += state.last_dc_val[ci];
	state.last_dc_val[ci] = s;
	/* Output the DC coefficient */
	(*block)[0] = (JCOEF) s;

	/* Section F.2.2.2: decode the AC coefficients */
	/* Since zeroes are skipped, output area must be cleared beforehand */
	for (; k < coef_limit; k++) {
	  HUFF_DECODE(s, br_state, htbl, return FALSE, label2);

	  r = s >> 4;
	  s &= 15;

	  if (s) {
	    k += r;
	    CHECK_BIT_BUFFER(br_state, s, return FALSE);
	    r = GET_BITS(s);
	    s = HUFF_EXTEND(r, s);
	    /* Output coefficient in natural (dezigzagged) order.
	     * Note: the extra entries in jpeg_natural_order[] will save us
	     * if k >= DCTSIZE2, which could happen if the data is corrupted.
	     */
	    (*block)[jpeg_natural_order[k]] = (JCOEF) s;
	  } else {
	    if (r != 15)
	      goto EndOfBlock;
	    k += 15;
	  }
	}
      } else {
	if (s) {
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  DROP_BITS(s);
	}
      }

      /* Section F.2.2.2: decode the AC coefficients */
      /* In this path we just discard the values */
      for (; k < DCTSIZE2; k++) {
	HUFF_DECODE(s, br_state, htbl, return FALSE, label3);

	r = s >> 4;
	s &= 15;

	if (s) {
	  k += r;
	  CHECK_BIT_BUFFER(br_state, s, return FALSE);
	  DROP_BITS(s);
	} else {
	  if (r != 15)
	    break;
	  k += 15;
	}
      }

      EndOfBlock: ;
    }

    /* Completed MCU, so update state */
    BITREAD_SAVE_STATE(cinfo,entropy->bitstate);
    ASSIGN_STATE(entropy->saved, state);
  }

  /* Account for restart interval (no-op if not using restarts) */
  entropy->restarts_to_go--;

  return TRUE;
}

/* 0286583C */
static void start_pass_huff_decoder (j_decompress_ptr cinfo)
{
  huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy.get();
  int ci, blkn, tbl, i;
  jpeg_component_info * compptr;

  if (cinfo->progressive_mode) {
    /* Validate progressive scan parameters */
    if (cinfo->Ss == 0) {
      if (cinfo->Se != 0)
	goto bad;
    } else {
      /* need not check Ss/Se < 0 since they came from unsigned bytes */
      if (cinfo->Se < cinfo->Ss || cinfo->Se > cinfo->lim_Se)
	goto bad;
      /* AC scans may have only one component */
      if (cinfo->comps_in_scan != 1)
	goto bad;
    }
    if (cinfo->Ah != 0) {
      /* Successive approximation refinement scan: must have Al = Ah-1. */
      if (cinfo->Ah-1 != cinfo->Al)
	goto bad;
    }
    if (cinfo->Al > 13) {	/* need not check for < 0 */
      /* Arguably the maximum Al value should be less than 13 for 8-bit precision,
       * but the spec doesn't say so, and we try to be liberal about what we
       * accept.  Note: large Al values could result in out-of-range DC
       * coefficients during early scans, leading to bizarre displays due to
       * overflows in the IDCT math.  But we won't crash.
       */
      bad:
      ERREXIT4(cinfo, JERR_BAD_PROGRESSION,
	       cinfo->Ss, cinfo->Se, cinfo->Ah, cinfo->Al);
    }
    /* Update progression status, and verify that scan order is legal.
     * Note that inter-scan inconsistencies are treated as warnings
     * not fatal errors ... not clear if this is right way to behave.
     */
    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      int coefi, cindex = cinfo->cur_comp_info[ci]->component_index;
      be<s32>* coef_bit_ptr = & cinfo->coef_bits[cindex * DCTSIZE2];
      if (cinfo->Ss && coef_bit_ptr[0] < 0) /* AC without prior DC scan */
	WARNMS2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, 0);
      for (coefi = cinfo->Ss; coefi <= cinfo->Se; coefi++) {
	int expected = (coef_bit_ptr[coefi] < 0) ? 0 : coef_bit_ptr[coefi];
	if (cinfo->Ah != expected)
	  WARNMS2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, coefi);
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

    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      compptr = cinfo->cur_comp_info[ci];
      /* Make sure requested tables are present, and compute derived tables.
       * We may build same derived table more than once, but it's not expensive.
       */
      if (cinfo->Ss == 0) {
	if (cinfo->Ah == 0) {	/* DC refinement needs no table */
	  tbl = compptr->dc_tbl_no;
	  jpeg_make_d_derived_tbl(cinfo, TRUE, tbl,
				  & entropy->derived_tbls[tbl]);
	}
      } else {
	tbl = compptr->ac_tbl_no;
	jpeg_make_d_derived_tbl(cinfo, FALSE, tbl,
				& entropy->derived_tbls[tbl]);
	/* remember the single active table */
	entropy->ac_derived_tbl = entropy->derived_tbls[tbl];
      }
      /* Initialize DC predictions to 0 */
      entropy->saved.last_dc_val[ci] = 0;
    }

    /* Initialize private state variables */
    entropy->saved.EOBRUN = 0;
  } else {
    /* Check that the scan parameters Ss, Se, Ah/Al are OK for sequential JPEG.
     * This ought to be an error condition, but we make it a warning because
     * there are some baseline files out there with all zeroes in these bytes.
     */
    if (cinfo->Ss != 0 || cinfo->Ah != 0 || cinfo->Al != 0 ||
	((cinfo->is_baseline || cinfo->Se < DCTSIZE2) &&
	cinfo->Se != cinfo->lim_Se))
      WARNMS(cinfo, JWRN_NOT_SEQUENTIAL);

    /* Select MCU decoding routine */
    /* We retain the hard-coded case for full-size blocks.
     * This is not necessary, but it appears that this version is slightly
     * more performant in the given implementation.
     * With an improved implementation we would prefer a single optimized
     * function.
     */
    if (cinfo->lim_Se != DCTSIZE2-1)
      entropy->pub.decode_mcu = decode_mcu_sub;
    else
      entropy->pub.decode_mcu = decode_mcu;

    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      compptr = cinfo->cur_comp_info[ci];
      /* Compute derived values for Huffman tables */
      /* We may do this more than once for a table, but it's not expensive */
      tbl = compptr->dc_tbl_no;
      jpeg_make_d_derived_tbl(cinfo, TRUE, tbl,
			      & entropy->dc_derived_tbls[tbl]);
      if (cinfo->lim_Se) {	/* AC needs no table when not present */
	tbl = compptr->ac_tbl_no;
	jpeg_make_d_derived_tbl(cinfo, FALSE, tbl,
				& entropy->ac_derived_tbls[tbl]);
      }
      /* Initialize DC predictions to 0 */
      entropy->saved.last_dc_val[ci] = 0;
    }

    /* Precalculate decoding info for each block in an MCU of this scan */
    for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
      ci = cinfo->MCU_membership[blkn];
      compptr = cinfo->cur_comp_info[ci];
      /* Precalculate which table to use for each block */
      entropy->dc_cur_tbls[blkn] = entropy->dc_derived_tbls[compptr->dc_tbl_no];
      entropy->ac_cur_tbls[blkn] = entropy->ac_derived_tbls[compptr->ac_tbl_no];
      /* Decide whether we really care about the coefficient values */
      if (compptr->component_needed) {
	ci = compptr->DCT_v_scaled_size;
	i = compptr->DCT_h_scaled_size;
	switch (cinfo->lim_Se) {
	case (1*1-1):
	  entropy->coef_limit[blkn] = 1;
	  break;
	case (2*2-1):
	  if (ci <= 0 || ci > 2) ci = 2;
	  if (i <= 0 || i > 2) i = 2;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order2[ci - 1][i - 1];
	  break;
	case (3*3-1):
	  if (ci <= 0 || ci > 3) ci = 3;
	  if (i <= 0 || i > 3) i = 3;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order3[ci - 1][i - 1];
	  break;
	case (4*4-1):
	  if (ci <= 0 || ci > 4) ci = 4;
	  if (i <= 0 || i > 4) i = 4;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order4[ci - 1][i - 1];
	  break;
	case (5*5-1):
	  if (ci <= 0 || ci > 5) ci = 5;
	  if (i <= 0 || i > 5) i = 5;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order5[ci - 1][i - 1];
	  break;
	case (6*6-1):
	  if (ci <= 0 || ci > 6) ci = 6;
	  if (i <= 0 || i > 6) i = 6;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order6[ci - 1][i - 1];
	  break;
	case (7*7-1):
	  if (ci <= 0 || ci > 7) ci = 7;
	  if (i <= 0 || i > 7) i = 7;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order7[ci - 1][i - 1];
	  break;
	default:
	  if (ci <= 0 || ci > 8) ci = 8;
	  if (i <= 0 || i > 8) i = 8;
	  entropy->coef_limit[blkn] = 1 + jpeg_zigzag_order[ci - 1][i - 1];
	  break;
	}
      } else {
	entropy->coef_limit[blkn] = 0;
      }
    }
  }

  /* Initialize bitread state variables */
  entropy->bitstate.bits_left = 0;
  entropy->bitstate.get_buffer = 0; /* unnecessary, but keeps Purify quiet */
  entropy->insufficient_data = FALSE;

  /* Initialize restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;
}

/* 02865F18 */
void jinit_huff_decoder (j_decompress_ptr cinfo)
{
  huff_entropy_ptr entropy;
  int i;

  entropy = (huff_entropy_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(huff_entropy_decoder));
  cinfo->entropy = (jpeg_entropy_decoder*) entropy;
  entropy->pub.start_pass = start_pass_huff_decoder;

  if (cinfo->progressive_mode) {
    /* Create progression status table */
    be<s32>* coef_bit_ptr; int ci;
    cinfo->coef_bits = (be<s32>*)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  cinfo->num_components*DCTSIZE2*SIZEOF(int));
    coef_bit_ptr = cinfo->coef_bits.get();
    for (ci = 0; ci < cinfo->num_components; ci++)
      for (i = 0; i < DCTSIZE2; i++)
	*coef_bit_ptr++ = -1;

    /* Mark derived tables unallocated */
    for (i = 0; i < NUM_HUFF_TBLS; i++) {
      entropy->derived_tbls[i] = NULL;
    }
  } else {
    /* Mark tables unallocated */
    for (i = 0; i < NUM_HUFF_TBLS; i++) {
      entropy->dc_derived_tbls[i] = entropy->ac_derived_tbls[i] = NULL;
    }
  }
}
/* WWHD glue
ext 028F5914 void huff_clear_words(be<s32>* dest,u32 count)
data jpeg_zigzag_order 1017D85C
data jpeg_zigzag_order7 1017D95C
data jpeg_zigzag_order6 1017DA20
data jpeg_zigzag_order5 1017DAB0
data jpeg_zigzag_order4 1017D7A8
data jpeg_zigzag_order3 1017D838
data jpeg_zigzag_order2 1017D7E8
data bmask 1017D7F8
data jpeg_natural_order 1017B7A8
*/
