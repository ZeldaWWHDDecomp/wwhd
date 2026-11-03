/* IJG decoder/DCT declarations adapted for typed guest storage.
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
/* DCT declarations from jdct.h:
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpegint_decoder.h"
using INT32 = s32;
using INT16 = s16;
using UINT8 = u8;
using DCTELEM = s32;
using ISLOW_MULT_TYPE = s32;
using IFAST_MULT_TYPE = s32;
using JCOEFPTR = be<JCOEF>*;
using inverse_DCT_method_ptr=void(*)(j_decompress_ptr,jpeg_component_info*,JCOEFPTR,JSAMPARRAY,JDIMENSION);
#define DCTSIZE 8
#define DCTSIZE2 64
#define BITS_IN_JSAMPLE 8
#define CENTERJSAMPLE 128
#define MAXJSAMPLE 255
#define RANGE_MASK 1023
#define DCT_ISLOW_SUPPORTED
#define DCT_IFAST_SUPPORTED
#define IDCT_SCALING_SUPPORTED
#define GLOBAL(type) type
#define GETJSAMPLE(value) ((int)(value))
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,n) ((x) >> (n))
#define DESCALE(x,n) RIGHT_SHIFT((x) + (1 << ((n)-1)),n)
#define CONST_SCALE (1 << CONST_BITS)
#define FIX(x) ((INT32) ((x) * CONST_SCALE + 0.5))
#define MULTIPLY16C16(var,const) ((var)*(const))
#define MULTIPLY16V16(var1,var2) ((var1)*(var2))
#define IDCT_range_limit(cinfo) ((cinfo)->sample_range_limit.get()+CENTERJSAMPLE)
#define ONE 1
#define IFAST_SCALE_BITS 2

#define FALSE 0
#define TRUE 1
#define SIZEOF(object) sizeof(object)
#define QUANT_2PASS_SUPPORTED
#define UPSAMPLE_MERGING_SUPPORTED

using INT16 = s16;
#define METHODDEF(type) static type
#define LOCAL(type) static type
#define ERREXIT(cinfo,code) do { (cinfo)->err->msg_code=(code); (*(cinfo)->err->error_exit)((j_common_ptr)(cinfo)); } while(0)
#define ERREXIT2(cinfo,code,p1,p2) do { (cinfo)->err->msg_code=(code); (cinfo)->err->msg_parm[0]=(p1); (cinfo)->err->msg_parm[1]=(p2); (*(cinfo)->err->error_exit)((j_common_ptr)(cinfo)); } while(0)
#define GETJOCTET(value) ((int)(value))
#define ERREXIT1(cinfo,code,p1) do { (cinfo)->err->msg_code=(code); (cinfo)->err->msg_parm[0]=(p1); (*(cinfo)->err->error_exit)((j_common_ptr)(cinfo)); } while(0)
#define ERREXIT4(cinfo,code,p1,p2,p3,p4) do { (cinfo)->err->msg_code=(code); (cinfo)->err->msg_parm[0]=(p1); (cinfo)->err->msg_parm[1]=(p2); (cinfo)->err->msg_parm[2]=(p3); (cinfo)->err->msg_parm[3]=(p4); (*(cinfo)->err->error_exit)((j_common_ptr)(cinfo)); } while(0)
#define WARNMS(cinfo,code) do { (cinfo)->err->msg_code=(code); (*(cinfo)->err->emit_message)((j_common_ptr)(cinfo),-1); } while(0)
#define WARNMS2(cinfo,code,p1,p2) do { (cinfo)->err->msg_code=(code); (cinfo)->err->msg_parm[0]=(p1); (cinfo)->err->msg_parm[1]=(p2); (*(cinfo)->err->emit_message)((j_common_ptr)(cinfo),-1); } while(0)
