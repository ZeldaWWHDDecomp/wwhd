/*
 * jmemmgr.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the JPEG system-independent memory management
 * routines.  This code is usable across a wide variety of machines; most
 * of the system dependencies have been isolated in a separate file.
 * The major functions provided here are:
 *   * pool-based allocation and freeing of memory;
 *   * policy decisions about how to divide available memory among the
 *     virtual arrays;
 *   * control logic for swapping virtual arrays between main memory and
 *     backing storage.
 * The separate system-dependent file provides the actual backing-storage
 * access code, and it contains the policy decision about how much total
 * main memory to use.
 * This file is system-dependent in the sense that some of its functions
 * are unnecessary in some systems.  For example, if there is enough virtual
 * memory so that backing storage will never be used, much of the virtual
 * array control logic could be removed.  (Of course, if you have that much
 * memory then you shouldn't care about a little bit of unused code...)
 */
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
#include "libjpeg/jmemnobs.h"
#define FAR
#define NO_GETENV
#define ALIGN_TYPE f64
#define MAX_ALLOC_CHUNK 1000000000
#define MIN_SLOP 50
#define MIN(a,b) ((a)<(b)?(a):(b))
struct alignas(8) small_pool_struct {
 struct { gptr<small_pool_struct> next; be<u32> bytes_used,bytes_left; } hdr;
};
struct alignas(8) large_pool_struct {
 struct { gptr<large_pool_struct> next; be<u32> bytes_used,bytes_left; } hdr;
};
using small_pool_ptr=small_pool_struct*;
using large_pool_ptr=large_pool_struct*;
using small_pool_hdr=small_pool_struct;
using large_pool_hdr=large_pool_struct;
WWHD_SIZE(small_pool_hdr,0x10);
WWHD_SIZE(large_pool_hdr,0x10);
WWHD_OFFSET(small_pool_hdr,hdr.bytes_used,4);
WWHD_OFFSET(small_pool_hdr,hdr.bytes_left,8);
struct backing_store_struct {
 jpeg_method<void(j_common_ptr,backing_store_ptr,void*,s32,s32)> read_backing_store,write_backing_store;
 jpeg_method<void(j_common_ptr,backing_store_ptr)> close_backing_store;
};
using backing_store_info=backing_store_struct;
WWHD_SIZE(backing_store_info,0xC);
// HD: the backing-store descriptor contains methods only; no stdio file/name fields.
struct jvirt_sarray_control {
 gptr<gptr<be<JSAMPLE>>> mem_buffer;
 be<JDIMENSION> rows_in_array,samplesperrow,maxaccess,rows_in_mem,rowsperchunk,cur_start_row,first_undef_row;
 be<boolean> pre_zero,dirty,b_s_open;
 gptr<jvirt_sarray_control> next;
 backing_store_info b_s_info;
};
struct jvirt_barray_control {
 gptr<gptr<JBLOCK>> mem_buffer;
 be<JDIMENSION> rows_in_array,blocksperrow,maxaccess,rows_in_mem,rowsperchunk,cur_start_row,first_undef_row;
 be<boolean> pre_zero,dirty,b_s_open;
 gptr<jvirt_barray_control> next;
 backing_store_info b_s_info;
};
using jvirt_sarray_ptr=jvirt_sarray_control*;
using jvirt_barray_ptr=jvirt_barray_control*;
WWHD_OFFSET(jvirt_sarray_control,next,0x2C);
WWHD_OFFSET(jvirt_sarray_control,b_s_info,0x30);
WWHD_SIZE(jvirt_sarray_control,0x3C);
WWHD_OFFSET(jvirt_barray_control,next,0x2C);
WWHD_OFFSET(jvirt_barray_control,b_s_info,0x30);
WWHD_SIZE(jvirt_barray_control,0x3C);
struct my_memory_mgr {
 jpeg_memory_mgr pub;
 gptr<small_pool_struct> small_list[2];
 gptr<large_pool_struct> large_list[2];
 gptr<jvirt_sarray_control> virt_sarray_list;
 gptr<jvirt_barray_control> virt_barray_list;
 be<s32> total_space_allocated;
 be<JDIMENSION> last_rowsperchunk;
};
using my_mem_ptr=my_memory_mgr*;
WWHD_OFFSET(my_memory_mgr,small_list,0x34);
WWHD_OFFSET(my_memory_mgr,large_list,0x3C);
WWHD_OFFSET(my_memory_mgr,virt_sarray_list,0x44);
WWHD_OFFSET(my_memory_mgr,virt_barray_list,0x48);
WWHD_OFFSET(my_memory_mgr,total_space_allocated,0x4C);
WWHD_OFFSET(my_memory_mgr,last_rowsperchunk,0x50);
WWHD_SIZE(my_memory_mgr,0x54);
static void out_of_memory (j_common_ptr cinfo, int which);
static void * alloc_small (j_common_ptr cinfo, int pool_id, u32 sizeofobject);
static void* alloc_large (j_common_ptr cinfo, int pool_id, u32 sizeofobject);
static JSAMPARRAY alloc_sarray (j_common_ptr cinfo, int pool_id,
	      JDIMENSION samplesperrow, JDIMENSION numrows);
static JBLOCKARRAY alloc_barray (j_common_ptr cinfo, int pool_id,
	      JDIMENSION blocksperrow, JDIMENSION numrows);
static jvirt_sarray_ptr request_virt_sarray (j_common_ptr cinfo, int pool_id, boolean pre_zero,
		     JDIMENSION samplesperrow, JDIMENSION numrows,
		     JDIMENSION maxaccess);
static jvirt_barray_ptr request_virt_barray (j_common_ptr cinfo, int pool_id, boolean pre_zero,
		     JDIMENSION blocksperrow, JDIMENSION numrows,
		     JDIMENSION maxaccess);
static void realize_virt_arrays (j_common_ptr cinfo);
static void do_sarray_io (j_common_ptr cinfo, jvirt_sarray_ptr ptr, boolean writing);
static void do_barray_io (j_common_ptr cinfo, jvirt_barray_ptr ptr, boolean writing);
static JSAMPARRAY access_virt_sarray (j_common_ptr cinfo, jvirt_sarray_ptr ptr,
		    JDIMENSION start_row, JDIMENSION num_rows,
		    boolean writable);
static JBLOCKARRAY access_virt_barray (j_common_ptr cinfo, jvirt_barray_ptr ptr,
		    JDIMENSION start_row, JDIMENSION num_rows,
		    boolean writable);
static void free_pool (j_common_ptr cinfo, int pool_id);
static void self_destruct (j_common_ptr cinfo);
void jinit_memory_mgr (j_common_ptr cinfo);
