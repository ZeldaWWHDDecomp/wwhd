/* jpegint.h decoder support (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted public method interfaces and support macros for big-endian guest storage.
 */
#pragma once
#include "libjpeg/jpegint_decoder.h"
#include "libjpeg/jpegint_color.h"
using UINT8=u8;
using INT32=s32;
#define TRUE 1
#define FALSE 0
#define GLOBAL(type) type
#define METHODDEF(type) static type
#define LOCAL(type) static type
#define SIZEOF(object) sizeof(object)
#define GETJSAMPLE(value) ((int)(value))
#define BITS_IN_JSAMPLE 8
#define MAXJSAMPLE 255
#define CENTERJSAMPLE 128
#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
#define RGB_PIXELSIZE 3
#define QUANT_2PASS_SUPPORTED
#define UPSAMPLE_MERGING_SUPPORTED
#define ERREXIT(cinfo,code) do { auto e=(cinfo)->err.get(); auto f=e->error_exit.load(); e->msg_code=(code); f((j_common_ptr)(cinfo)); } while(0)
#define ERREXIT1(cinfo,code,p1) do { auto e=(cinfo)->err.get(); auto f=e->error_exit.load(); e->msg_code=(code);e->msg_parm[0]=(p1);f((j_common_ptr)(cinfo)); } while(0)
#define ERREXIT2(cinfo,code,p1,p2) do { auto e=(cinfo)->err.get(); auto f=e->error_exit.load(); e->msg_code=(code);e->msg_parm[0]=(p1);e->msg_parm[1]=(p2);f((j_common_ptr)(cinfo)); } while(0)

template<class... P> inline void decoder_emit(j_common_ptr cinfo,s32 level,s32 code,const P&... parameters) {
 auto err=cinfo->err.get();auto emit=err->emit_message.load();
 s32 values[]={0,static_cast<s32>(parameters)...};
 for (u32 i=1;i<sizeof...(P)+1;i++) err->msg_parm[i-1]=values[i];
 err->msg_code=code;emit(cinfo,level);
}
#define TRACEMS(cinfo,level,code) decoder_emit((j_common_ptr)(cinfo),(level),(code))
#define TRACEMS1(cinfo,level,code,p0) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0)
#define TRACEMS2(cinfo,level,code,p0,p1) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0,p1)
#define TRACEMS3(cinfo,level,code,p0,p1,p2) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0,p1,p2)
#define TRACEMS4(cinfo,level,code,p0,p1,p2,p3) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0,p1,p2,p3)
#define TRACEMS5(cinfo,level,code,p0,p1,p2,p3,p4) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0,p1,p2,p3,p4)
#define TRACEMS8(cinfo,level,code,p0,p1,p2,p3,p4,p5,p6,p7) decoder_emit((j_common_ptr)(cinfo),(level),(code),p0,p1,p2,p3,p4,p5,p6,p7)
#define WARNMS(cinfo,code) decoder_emit((j_common_ptr)(cinfo),-1,(code))
#define WARNMS2(cinfo,code,p0,p1) decoder_emit((j_common_ptr)(cinfo),-1,(code),(p0),(p1))
