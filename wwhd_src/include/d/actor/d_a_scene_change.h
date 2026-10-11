/* Scene change marker, WWHD layout: actor member +0x11C from GameCube.
 * HIO members move -4, with the HD vtable following them. Names follow tww. */
#pragma once
#include "gc_api/gc_api.h"

class d_a_scene_change_c : public fopAc_ac_c {
public:
    d_a_scene_change_c();
    enum : u32 { __vt = 0x10039DFC }; // HD: virtual destructor
    /* 0x3AC */ Mtx34 mTransformMtx; /* GameCube 0x290 */
};
WWHD_OFFSET(d_a_scene_change_c, mTransformMtx, 0x3AC);

/* HD: GHS places the vtable pointer after the members */
class daSceneChgHIO_c {
public:
    enum : u32 { __vt = 0x10039DEC };
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ be<s8> m0005;
    /* 0x2 */ u8 _2[2];
    /* 0x4 */ be<f32> m0008;
    /* 0x8 */ be<u32> __vtbl;
};
WWHD_SIZE(daSceneChgHIO_c, 0xC);

WWHD_SIZE(d_a_scene_change_c, 0x3DC);
WWHD_OFFSET(daSceneChgHIO_c, __vtbl, 8);
