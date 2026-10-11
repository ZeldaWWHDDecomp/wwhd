/* Request flags. */
#pragma once
#include "wwhd.h" /* light: f_op/f_op_overlap.h includes it */

// The GameCube flag0/flag1/flag2 bitfields occupy one big-endian byte.
enum cReq_Flag : u8 {
    cReq_flag0 = 0x80,
    cReq_flag1 = 0x40,
    cReq_flag2Mask = 0x3F,
};
struct request_base_class {
    be<u8> mFlags;
};
WWHD_OFFSET(request_base_class, mFlags, 0);
WWHD_SIZE(request_base_class, 1);
BOOL cReq_Is_Done(request_base_class*);
void cReq_Done(request_base_class*);
void cReq_Create(request_base_class*, u32);
void cReq_Command(request_base_class*, u32);
