/*
 * jfdctint.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modification developed 2003-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a slow-but-accurate integer implementation of the
 * forward DCT (Discrete Cosine Transform).
 *
 * A 2-D DCT can be done by 1-D DCT on each row followed by 1-D DCT
 * on each column.  Direct algorithms are also available, but they are
 * much more complex and seem not to be any faster when reduced to code.
 *
 * This implementation is based on an algorithm described in
 *   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
 *   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
 *   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
 * The primary algorithm described there uses 11 multiplies and 29 adds.
 * We use their alternate method with 12 multiplies and 32 adds.
 * The advantage of this method is that no data path contains more than one
 * multiplication; this allows a very simple and accurate implementation in
 * scaled fixed-point arithmetic, with a minimal number of shifts.
 *
 * We also provide FDCT routines with various input sample block sizes for
 * direct resolution reduction or enlargement and for direct resolving the
 * common 2x1 and 1x2 subsampling cases without additional resampling: NxN
 * (N=1...16), 2NxN, and Nx2N (N=1...8) pixels for one 8x8 output DCT block.
 *
 * For N<8 we fill the remaining block coefficients with zero.
 * For N>8 we apply a partial N-point FDCT on the input samples, computing
 * just the lower 8 frequency coefficients and discarding the rest.
 *
 * We must scale the output coefficients of the N-point FDCT appropriately
 * to the standard 8-point FDCT level by 8/N per 1-D pass.  This scaling
 * is folded into the constant multipliers (pass 2) and/or final/initial
 * shifting.
 *
 * CAUTION: We rely on the FIX() macro except for the N=1,2,4,8 cases
 * since there would be too many additional constants to pre-calculate.
 */

// Decompiled WWHD build; guest pointers and fixed-width integer arithmetic.
#include "libjpeg/jfdctint.h"
#include "sead/ElementAt.h"

#ifdef DCT_ISLOW_SUPPORTED


/*
 * This module is specialized to the case DCTSIZE = 8.
 */

#if DCTSIZE != 8
  Sorry, this code only copes with 8x8 DCT blocks. /* deliberate syntax err */
#endif


/*
 * The poop on this scaling stuff is as follows:
 *
 * Each 1-D DCT step produces outputs which are a factor of sqrt(N)
 * larger than the true DCT outputs.  The final outputs are therefore
 * a factor of N larger than desired; since N=8 this can be cured by
 * a simple right shift at the end of the algorithm.  The advantage of
 * this arrangement is that we save two multiplications per 1-D DCT,
 * because the y0 and y4 outputs need not be divided by sqrt(N).
 * In the IJG code, this factor of 8 is removed by the quantization step
 * (in jcdctmgr.c), NOT in this module.
 *
 * We have to do addition and subtraction of the integer inputs, which
 * is no problem, and multiplication by fractional constants, which is
 * a problem to do in integer arithmetic.  We multiply all the constants
 * by CONST_SCALE and convert them to integer constants (thus retaining
 * CONST_BITS bits of precision in the constants).  After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output.  This division can be done
 * cheaply as a right shift of CONST_BITS bits.  We postpone shifting
 * as long as possible so that partial sums can be added together with
 * full fractional precision.
 *
 * The outputs of the first pass are scaled up by PASS1_BITS bits so that
 * they are represented to better-than-integral precision.  These outputs
 * require BITS_IN_JSAMPLE + PASS1_BITS + 3 bits; this fits in a 16-bit word
 * with the recommended scaling.  (For 12-bit sample data, the intermediate
 * array is INT32 anyway.)
 *
 * To avoid overflow of the 32-bit intermediate results in pass 2, we must
 * have BITS_IN_JSAMPLE + CONST_BITS + PASS1_BITS <= 26.  Error analysis
 * shows that the values given below are the most effective.
 */

#if BITS_IN_JSAMPLE == 8
#define CONST_BITS  13
#define PASS1_BITS  2
#else
#define CONST_BITS  13
#define PASS1_BITS  1		/* lose a little precision to avoid overflow */
#endif

/* Some C compilers fail to reduce "FIX(constant)" at compile time, thus
 * causing a lot of useless floating-point operations at run time.
 * To get around this we use the following pre-calculated constants.
 * If you change CONST_BITS you may want to add appropriate values.
 * (With a reasonable C compiler, you can just rely on the FIX() macro...)
 */

#if CONST_BITS == 13
#define FIX_0_298631336  ((INT32)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((INT32)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((INT32)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((INT32)  6270)	/* FIX(0.765366865) */
#define FIX_0_899976223  ((INT32)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((INT32)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((INT32)  12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((INT32)  15137)	/* FIX(1.847759065) */
#define FIX_1_961570560  ((INT32)  16069)	/* FIX(1.961570560) */
#define FIX_2_053119869  ((INT32)  16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((INT32)  20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((INT32)  25172)	/* FIX(3.072711026) */
#else
#define FIX_0_298631336  FIX(0.298631336)
#define FIX_0_390180644  FIX(0.390180644)
#define FIX_0_541196100  FIX(0.541196100)
#define FIX_0_765366865  FIX(0.765366865)
#define FIX_0_899976223  FIX(0.899976223)
#define FIX_1_175875602  FIX(1.175875602)
#define FIX_1_501321110  FIX(1.501321110)
#define FIX_1_847759065  FIX(1.847759065)
#define FIX_1_961570560  FIX(1.961570560)
#define FIX_2_053119869  FIX(2.053119869)
#define FIX_2_562915447  FIX(2.562915447)
#define FIX_3_072711026  FIX(3.072711026)
#endif


/* Multiply an INT32 variable by an INT32 constant to yield an INT32 result.
 * For 8-bit samples with the recommended scaling, all the variable
 * and constant values involved are no more than 16 bits wide, so a
 * 16x16->32 bit multiply can be used instead of a full 32x32 multiply.
 * For 12-bit samples, a full 32-bit multiplication will be needed.
 */

#if BITS_IN_JSAMPLE == 8
#define MULTIPLY(var,const)  MULTIPLY16C16(var,const)
#else
#define MULTIPLY(var,const)  ((var) * (const))
#endif


/*
 * Perform the forward DCT on one block of samples.
 */

/* 02866A9C */
void jpeg_fdct_islow (be<DCTELEM>* data, JSAMPARRAY sample_data, JDIMENSION start_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3;
  INT32 tmp10, tmp11, tmp12, tmp13;
  INT32 z1;
  be<DCTELEM>* dataptr;
  JSAMPROW elemptr;
  int ctr;
  SHIFT_TEMPS

  /* Pass 1: process rows. */
  /* Note results are scaled up by sqrt(8) compared to a true DCT; */
  /* furthermore, we scale the results by 2**PASS1_BITS. */

  dataptr = data;
  for (ctr = 0; ctr < DCTSIZE; ctr++) {
    elemptr = &sead::elementAt(sample_data[ctr], start_col);

    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */

    tmp0 = GETJSAMPLE(elemptr[0]) + GETJSAMPLE(elemptr[7]);
    tmp1 = GETJSAMPLE(elemptr[1]) + GETJSAMPLE(elemptr[6]);
    tmp2 = GETJSAMPLE(elemptr[2]) + GETJSAMPLE(elemptr[5]);
    tmp3 = GETJSAMPLE(elemptr[3]) + GETJSAMPLE(elemptr[4]);

    tmp10 = tmp0 + tmp3;
    tmp12 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp13 = tmp1 - tmp2;

    tmp0 = GETJSAMPLE(elemptr[0]) - GETJSAMPLE(elemptr[7]);
    tmp1 = GETJSAMPLE(elemptr[1]) - GETJSAMPLE(elemptr[6]);
    tmp2 = GETJSAMPLE(elemptr[2]) - GETJSAMPLE(elemptr[5]);
    tmp3 = GETJSAMPLE(elemptr[3]) - GETJSAMPLE(elemptr[4]);

    /* Apply unsigned->signed conversion */
    dataptr[0] = (DCTELEM) ((tmp10 + tmp11 - 8 * CENTERJSAMPLE) << PASS1_BITS);
    dataptr[4] = (DCTELEM) ((tmp10 - tmp11) << PASS1_BITS);

    z1 = MULTIPLY(tmp12 + tmp13, FIX_0_541196100);
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);
    dataptr[2] = (DCTELEM) RIGHT_SHIFT(z1 + MULTIPLY(tmp12, FIX_0_765366865),
				       CONST_BITS-PASS1_BITS);
    dataptr[6] = (DCTELEM) RIGHT_SHIFT(z1 - MULTIPLY(tmp13, FIX_1_847759065),
				       CONST_BITS-PASS1_BITS);

    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents sqrt(2) * cos(K*pi/16).
     * i0..i3 in the paper are tmp0..tmp3 here.
     */

    tmp10 = tmp0 + tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp1 + tmp3;
    z1 = MULTIPLY(tmp12 + tmp13, FIX_1_175875602); /*  c3 */
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);

    tmp0  = MULTIPLY(tmp0,    FIX_1_501321110);    /*  c1+c3-c5-c7 */
    tmp1  = MULTIPLY(tmp1,    FIX_3_072711026);    /*  c1+c3+c5-c7 */
    tmp2  = MULTIPLY(tmp2,    FIX_2_053119869);    /*  c1+c3-c5+c7 */
    tmp3  = MULTIPLY(tmp3,    FIX_0_298631336);    /* -c1+c3+c5-c7 */
    tmp10 = MULTIPLY(tmp10, - FIX_0_899976223);    /*  c7-c3 */
    tmp11 = MULTIPLY(tmp11, - FIX_2_562915447);    /* -c1-c3 */
    tmp12 = MULTIPLY(tmp12, - FIX_0_390180644);    /*  c5-c3 */
    tmp13 = MULTIPLY(tmp13, - FIX_1_961570560);    /* -c3-c5 */

    tmp12 += z1;
    tmp13 += z1;

    dataptr[1] = (DCTELEM)
      RIGHT_SHIFT(tmp0 + tmp10 + tmp12, CONST_BITS-PASS1_BITS);
    dataptr[3] = (DCTELEM)
      RIGHT_SHIFT(tmp1 + tmp11 + tmp13, CONST_BITS-PASS1_BITS);
    dataptr[5] = (DCTELEM)
      RIGHT_SHIFT(tmp2 + tmp11 + tmp12, CONST_BITS-PASS1_BITS);
    dataptr[7] = (DCTELEM)
      RIGHT_SHIFT(tmp3 + tmp10 + tmp13, CONST_BITS-PASS1_BITS);

    dataptr += DCTSIZE;		/* advance pointer to next row */
  }

  /* Pass 2: process columns.
   * We remove the PASS1_BITS scaling, but leave the results scaled up
   * by an overall factor of 8.
   */

  dataptr = data;
  for (ctr = DCTSIZE-1; ctr >= 0; ctr--) {
    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */

    tmp0 = dataptr[DCTSIZE*0] + dataptr[DCTSIZE*7];
    tmp1 = dataptr[DCTSIZE*1] + dataptr[DCTSIZE*6];
    tmp2 = dataptr[DCTSIZE*2] + dataptr[DCTSIZE*5];
    tmp3 = dataptr[DCTSIZE*3] + dataptr[DCTSIZE*4];

    /* Add fudge factor here for final descale. */
    tmp10 = tmp0 + tmp3 + (ONE << (PASS1_BITS-1));
    tmp12 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp13 = tmp1 - tmp2;

    tmp0 = dataptr[DCTSIZE*0] - dataptr[DCTSIZE*7];
    tmp1 = dataptr[DCTSIZE*1] - dataptr[DCTSIZE*6];
    tmp2 = dataptr[DCTSIZE*2] - dataptr[DCTSIZE*5];
    tmp3 = dataptr[DCTSIZE*3] - dataptr[DCTSIZE*4];

    dataptr[DCTSIZE*0] = (DCTELEM) RIGHT_SHIFT(tmp10 + tmp11, PASS1_BITS);
    dataptr[DCTSIZE*4] = (DCTELEM) RIGHT_SHIFT(tmp10 - tmp11, PASS1_BITS);

    z1 = MULTIPLY(tmp12 + tmp13, FIX_0_541196100);
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS+PASS1_BITS-1);
    dataptr[DCTSIZE*2] = (DCTELEM)
      RIGHT_SHIFT(z1 + MULTIPLY(tmp12, FIX_0_765366865), CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*6] = (DCTELEM)
      RIGHT_SHIFT(z1 - MULTIPLY(tmp13, FIX_1_847759065), CONST_BITS+PASS1_BITS);

    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents sqrt(2) * cos(K*pi/16).
     * i0..i3 in the paper are tmp0..tmp3 here.
     */

    tmp10 = tmp0 + tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp1 + tmp3;
    z1 = MULTIPLY(tmp12 + tmp13, FIX_1_175875602); /*  c3 */
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS+PASS1_BITS-1);

    tmp0  = MULTIPLY(tmp0,    FIX_1_501321110);    /*  c1+c3-c5-c7 */
    tmp1  = MULTIPLY(tmp1,    FIX_3_072711026);    /*  c1+c3+c5-c7 */
    tmp2  = MULTIPLY(tmp2,    FIX_2_053119869);    /*  c1+c3-c5+c7 */
    tmp3  = MULTIPLY(tmp3,    FIX_0_298631336);    /* -c1+c3+c5-c7 */
    tmp10 = MULTIPLY(tmp10, - FIX_0_899976223);    /*  c7-c3 */
    tmp11 = MULTIPLY(tmp11, - FIX_2_562915447);    /* -c1-c3 */
    tmp12 = MULTIPLY(tmp12, - FIX_0_390180644);    /*  c5-c3 */
    tmp13 = MULTIPLY(tmp13, - FIX_1_961570560);    /* -c3-c5 */

    tmp12 += z1;
    tmp13 += z1;

    dataptr[DCTSIZE*1] = (DCTELEM)
      RIGHT_SHIFT(tmp0 + tmp10 + tmp12, CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*3] = (DCTELEM)
      RIGHT_SHIFT(tmp1 + tmp11 + tmp13, CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*5] = (DCTELEM)
      RIGHT_SHIFT(tmp2 + tmp11 + tmp12, CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*7] = (DCTELEM)
      RIGHT_SHIFT(tmp3 + tmp10 + tmp13, CONST_BITS+PASS1_BITS);

    dataptr++;			/* advance pointer to next column */
  }
}
#endif
/* WWHD glue
*/
