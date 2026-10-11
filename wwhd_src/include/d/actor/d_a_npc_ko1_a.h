/** Part A private views and TU statics. Shared legacy actor header remains unchanged. */
#pragma once
#define SAFESTRING_VTBL 0x1001C87C
#include "d/actor/d_a_npc_ko1.h"
#include "gc_api/f_op/f_op_actor.h"

struct Ko1ActorList { gptr<fopAc_ac_c> mActors[20]; };
WWHD_OFFSET(Ko1ActorList, mActors, 0);
WWHD_SIZE(Ko1ActorList, 80);
struct daNpc_Ko1_childHIO_c {
    u8 mUnused[0x18];
    be<f32> mAttentionOffsetY;
    u8 mUnused1C[0x60-0x1C];
};
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mAttentionOffsetY, 0x18);
WWHD_SIZE(daNpc_Ko1_childHIO_c, 0x60);
struct daNpc_Ko1_HIO_c { daNpc_Ko1_childHIO_c mChild[2]; };
WWHD_SIZE(daNpc_Ko1_HIO_c, 0xC0);
// Part A owns the definitions. Later part files must reference these objects, not define copies.
extern be<s32>& l_check_wrk;
extern Ko1ActorList& l_check_inf;
extern daNpc_Ko1_HIO_c& l_HIO;

struct Ko1MorfVtable { u8 mUnused[0xC]; gfn destroy; };
WWHD_OFFSET(Ko1MorfVtable, destroy, 0xC);
WWHD_SIZE(Ko1MorfVtable, 0x10);
struct Ko1MorfDeleteView {
    gptr<Ko1MorfVtable> mVtable;
    void destroy(s32 flags) { gabi::call_ptr(mVtable->destroy.get(), this, flags); }
};
WWHD_SIZE(Ko1MorfDeleteView, 4);
// HD: preserve the full return register in GHS's tail-call adapters.
u32 setBtpRegister(daNpc_Ko1_c*, u32, s32);
u32 createHeapRegister(daNpc_Ko1_c*);
// HD: the original helper does not call SafeString::assureTermination.
inline void* ko1ObjectRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
