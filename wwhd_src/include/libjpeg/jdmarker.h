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
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
#define FAR
#define GETJOCTET(value) ((int)(value))
#define MAKESTMT(stuff) do { stuff } while(0)
#define D_ARITH_CODING_SUPPORTED
#define APP0_DATA_LEN 14
#define APP14_DATA_LEN 12
#define APPN_DATA_LEN 14
using UINT16=u16;
struct my_marker_reader {
 jpeg_marker_reader pub;
 jpeg_method<boolean(j_decompress_ptr)> process_COM,process_APPn[16];
 be<u32> length_limit_COM,length_limit_APPn[16];
 gptr<jpeg_marker_struct> cur_marker;
 be<u32> bytes_read;
};
using my_marker_ptr=my_marker_reader*;
WWHD_OFFSET(my_marker_reader,process_COM,0x1C);
WWHD_OFFSET(my_marker_reader,process_APPn,0x20);
WWHD_OFFSET(my_marker_reader,length_limit_COM,0x60);
WWHD_OFFSET(my_marker_reader,length_limit_APPn,0x64);
WWHD_OFFSET(my_marker_reader,cur_marker,0xA4);
WWHD_OFFSET(my_marker_reader,bytes_read,0xA8);
WWHD_SIZE(my_marker_reader,0xAC);
// HD stack: bits at SP+8 and huffval at SP+0x1C, both word-aligned.
struct marker_huffman_buffer { be<u8> bits[17]; u8 padding[3]; be<u8> huffval[256]; };
struct marker_app_bytes { be<JOCTET> bytes[14]; };
WWHD_SIZE(marker_huffman_buffer,276);
WWHD_OFFSET(marker_huffman_buffer,huffval,20);
WWHD_SIZE(marker_app_bytes,14);
static void* marker_copy(void* dest,const void* source,u32 size);
#define MEMCOPY(dest,src,size) marker_copy(dest,src,size)
typedef enum {			/* JPEG marker codes */
  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,
  
  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,
  
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  
  M_DHT   = 0xc4,
  
  M_DAC   = 0xcc,
  
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,
  
  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,
  
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  
  M_TEM   = 0x01,
  
  M_ERROR = 0x100
} JPEG_MARKER;
#define INPUT_VARS(cinfo)  \
	struct jpeg_source_mgr * datasrc = (cinfo)->src;  \
	const be<JOCTET>* next_input_byte = datasrc->next_input_byte;  \
	u32 bytes_in_buffer = datasrc->bytes_in_buffer

/* Unload the local copies --- do this only at a restart boundary */
#define INPUT_SYNC(cinfo)  \
	( datasrc->next_input_byte = next_input_byte,  \
	  datasrc->bytes_in_buffer = bytes_in_buffer )

/* Reload the local copies --- used only in MAKE_BYTE_AVAIL */
#define INPUT_RELOAD(cinfo)  \
	( next_input_byte = datasrc->next_input_byte,  \
	  bytes_in_buffer = datasrc->bytes_in_buffer )

/* Internal macro for INPUT_BYTE and INPUT_2BYTES: make a byte available.
 * Note we do *not* do INPUT_SYNC before calling fill_input_buffer,
 * but we must reload the local copies after a successful fill.
 */
#define MAKE_BYTE_AVAIL(cinfo,action)  \
	if (bytes_in_buffer == 0) {  \
	  if (! (*datasrc->fill_input_buffer) (cinfo))  \
	    { action; }  \
	  INPUT_RELOAD(cinfo);  \
	}

/* Read a byte into variable V.
 * If must suspend, take the specified action (typically "return FALSE").
 */
#define INPUT_BYTE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = GETJOCTET(*next_input_byte++); )

/* As above, but read two bytes interpreted as an unsigned 16-bit integer.
 * V should be declared unsigned int or perhaps INT32.
 */
#define INPUT_2BYTES(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = ((unsigned int) GETJOCTET(*next_input_byte++)) << 8; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += GETJOCTET(*next_input_byte++); )



static boolean get_soi (j_decompress_ptr cinfo);
static boolean get_sof (j_decompress_ptr cinfo, boolean is_baseline, boolean is_prog,
	 boolean is_arith);
static boolean get_sos (j_decompress_ptr cinfo);
static boolean get_dac (j_decompress_ptr cinfo);
static boolean get_dht (j_decompress_ptr cinfo);
static boolean get_dqt (j_decompress_ptr cinfo);
static boolean get_dri (j_decompress_ptr cinfo);
static void examine_app0 (j_decompress_ptr cinfo, be<JOCTET>* data,
	      unsigned int datalen, INT32 remaining);
static void examine_app14 (j_decompress_ptr cinfo, be<JOCTET>* data,
	       unsigned int datalen, INT32 remaining);
static boolean get_interesting_appn (j_decompress_ptr cinfo);
static boolean skip_variable (j_decompress_ptr cinfo);
static boolean next_marker (j_decompress_ptr cinfo);
static boolean first_marker (j_decompress_ptr cinfo);
static int read_markers (j_decompress_ptr cinfo);
static boolean read_restart_marker (j_decompress_ptr cinfo);
boolean jpeg_resync_to_restart (j_decompress_ptr cinfo, int desired);
static void reset_marker_reader (j_decompress_ptr cinfo);
void jinit_marker_reader (j_decompress_ptr cinfo);
JHUFF_TBL* jpeg_alloc_huff_table(j_common_ptr cinfo);
JQUANT_TBL* jpeg_alloc_quant_table(j_common_ptr cinfo);
