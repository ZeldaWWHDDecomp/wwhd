#pragma once
#include "gc_api/gc_api.h"
struct daObjGnnbtaki_c : fopAc_ac_c {
    daObjGnnbtaki_c();
    enum : u32 { __vt = 0x100296BC };
    static const char M_arcname[];
    BOOL create_heap();
    void init_mtx();
    s32 _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    gptr<J3DModel> mpModel;
    request_of_phase_process_class mPhase;
    mDoExt_btkAnm mBtkAnm;
    be<u8> field_0x2B0, field_0x2B1;
    u8 padding[2];
};
WWHD_OFFSET(daObjGnnbtaki_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjGnnbtaki_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjGnnbtaki_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjGnnbtaki_c, field_0x2B0, 0x42C);
WWHD_OFFSET(daObjGnnbtaki_c, field_0x2B1, 0x42D);
WWHD_SIZE(daObjGnnbtaki_c, 0x430);

void gnnbtaki_actor_dt(void*, s32);

// HD: private play-field and SafeString views.
struct gnnbtaki_play_c {
    u8 pad[0x5134];
    char mStartStageName[8];
    u8 pad513C[0x52C4 - 0x513C];
    u8 mEventManager;
};
WWHD_OFFSET(gnnbtaki_play_c, mStartStageName, 0x5134);
WWHD_OFFSET(gnnbtaki_play_c, mEventManager, 0x52C4);
WWHD_SIZE(gnnbtaki_play_c, 0x52C5);
BOOL gnnbtaki_startCheck(void*, const char*);
// HD: this inline SafeString comparison is distinct from GameCube strcmp.
inline bool gnnbtaki_isTowerStage() {
    gabi::Local<SafeString> requested, stage;
    requested->mStringTop = gabi::guest_addr("GTower");
    requested->__vtbl = gc_tu.safeString;
    auto* play = (gnnbtaki_play_c*)dComIfGp_get();
    stage->mStringTop = gabi::ea(play->mStartStageName);
    stage->__vtbl = gc_tu.safeString;
    auto assure = [](SafeString* s) {
        gabi::call_ptr<void>(gabi::load<u32>((u32)s->__vtbl + 0x14), s);
    };
    assure(requested.get());
    assure(requested.get());
    u32 first = requested->mStringTop;
    assure(stage.get());
    u32 second = stage->mStringTop;
    if (first == second) return true;
    auto* left = gabi::at<be<u8>>(requested->mStringTop);
    auto* right = gabi::at<be<u8>>(stage->mStringTop);
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = left[i], b = right[i];
        if (a != b) return false;
        if (!a) return true;
    }
    return false;
}
