/* d_bg_s (GameCube names): inline constructors/destructors of the collision helpers with this
 * TU's vtables (gc_tu). Layouts: d/d_bg_s.h. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"

/* dBgS_GndChk as a local (TailControl): inline constructor and destructor [v nz] */
inline void gabi_construct(dBgS_GndChk& c) { dBgS_GndChk_ct(&c, gc_tu.gndChk, false); }
inline void gabi_destroy(dBgS_GndChk& c) {
    u32 b = gabi::ea(&c);
    gabi::store<u32>(b + 0x20, gc_tu.gndChkDt[0]);
    gabi::store<u32>(b + 0x40, gc_tu.gndChkDt[1]);
    gabi::store<u32>(b + 0x4C, gc_tu.gndChkDt[2]);
    gabi::call(0x02008DAC, &c, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* members: compiler-generated construction/destruction (HD) [v nz] */
inline void __ct(dBgS_ObjAcch* a) { dBgS_ObjAcch_ct(a, gc_tu.objAcch); }
inline void __dt(dBgS_ObjAcch* a) {
    u32 b = gabi::ea(a);
    gabi::store<u32>(b + 0x20, gc_tu.objAcchDt[0]);
    gabi::store<u32>(b + 0x14, gc_tu.objAcchDt[1]);
    gabi::call(0x024EFD9C, a, 0); /* dBgS_Acch::~dBgS_Acch */
}
inline void __ct(dBgS_AcchCir* c) { gabi::call(0x024EFE94, c); }
inline void __dt(dBgS_AcchCir* c) { gabi::call(0x02018034, gabi::ea(c) + 0x14, 2); } /* its cM3dGCir */
inline void __dt(dCcD_Stts* s) { gabi::call(0x02515860, s, 2); }
inline void __dt(dCcD_Cyl* c) { gabi::call(0x02515A70, c, 2); } /* matcher: "dBgS_Acch::~dBgS_Acch" */

/* 024F2478 dBgW_NewSet(dzb, flags, mtx): new dBgW + Set; NULL on failure [v lpalm] */
inline dBgW* dBgW_NewSet(cBgD_t* dzb, u32 flags, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, dzb, flags, mtx); }
