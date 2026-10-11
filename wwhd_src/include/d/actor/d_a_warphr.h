/**
 * d_a_warphr.h (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "m_Do/m_Do_graphic.h"
struct daWarphr_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel1;
    gptr<mDoExt_btkAnm> mpBtkAnm1;
    gptr<J3DModel> mpModel2;
    gptr<mDoExt_btkAnm> mpBtkAnm2;
    gptr<mDoExt_brkAnm> mpBrkAnm;
    gptr<JPABaseEmitter> m2AC, m2B0;
    Mtx34 m2B4;
    be<u8> m2E4;
    u8 pad401[11];
    be<s32> m2F0;
    u8 pad410[4];
    be<s16> m2F8;
    be<u8> m2FA;
    u8 pad417[5];
    be<u32> m300;
    be<s32> m304;
    s32 CreateHeap();
    void CreateInit();
    s32 _create();
    bool _delete();
    bool _draw();
    s32 get_return_count();
    s32 check_warp();
    void anim_play(s32);
    bool normal_execute();
    void checkOrder();
    void demo_proc();
    void eventOrder();
    bool demo_execute();
    bool _execute();
    void set_end_anim();
    void initWait(s32);
    bool actWarpArrive(s32);
    bool actWait(s32);
    void initWarp(s32);
    bool actWarp(s32);
    void initWarpArrive(s32);
    void initWarpArriveEnd(s32);
    bool actWarpArriveEnd(s32);
    void initStartWarp(s32);
    bool actStartWarp(s32);
};
WWHD_OFFSET(daWarphr_c, mPhase, 0x3AC);
WWHD_OFFSET(daWarphr_c, mpModel1, 0x3B4);
WWHD_OFFSET(daWarphr_c, m2AC, 0x3C8);
WWHD_OFFSET(daWarphr_c, m2B4, 0x3D0);
WWHD_OFFSET(daWarphr_c, m2E4, 0x400);
WWHD_OFFSET(daWarphr_c, m2F0, 0x40C);
WWHD_OFFSET(daWarphr_c, m2F8, 0x414);
WWHD_OFFSET(daWarphr_c, m300, 0x41C);
WWHD_OFFSET(daWarphr_c, m304, 0x420);
WWHD_SIZE(daWarphr_c, 0x424);

struct SafeName {
    be<u32> text, vt;
};
namespace warphr {
// HD: the emitter's global rotation matrix, translation and flags are beyond its shared partial view.
struct Emitter {
    u8 pad00[0x5C];
    be<s32> mMaxFrame;
    u8 pad60[0x190];
    u8 mGlobalRotation[0x30];
    u8 pad220[12];
    cXyz mGlobalTranslation;
    u8 pad238[0x1C];
    be<u32> mFlags;
};
WWHD_OFFSET(Emitter, mFlags, 0x254);
inline Emitter* emitter(u32 address) {
    return gabi::at<Emitter>(address);
}
inline mDoExt_baseAnm* animation(u32 address) {
    return gabi::at<mDoExt_baseAnm>(address);
}
inline cXyz* actorPosition(u32 address) {
    return gabi::at<cXyz>(address + offsetof(fopAc_ac_c, current) + offsetof(actor_place, pos));
}
inline cXyz* modelScale(u32 address) {
    return gabi::at<cXyz>(address + offsetof(J3DModel, mBaseScale));
}
inline u32 modelData(u32 address) {
    return gabi::load<u32>(address + offsetof(J3DModel, mModelData));
}
inline dComIfG_inf_c* game(u32 address) {
    return gabi::at<dComIfG_inf_c>(address);
}
inline u32 player(u32 address) {
    return gabi::ea(game(address)->play.mPlayerInfo[0].mpPlayer.get());
}
inline u32 ship(u32 address) {
    return gabi::ea(game(address)->play.mpPlayerPtr[2].get());
}
inline u32 playerStatus(u32 address) {
    return game(address)->play.mPlayerStatus[0][0];
}
inline u8 eventRunning(u32 address) {
    return game(address)->play.mEvtCtrl.mMode;
}
inline void cameraPosition(u32 camera, cXyz* out) {
    u32 vt = gabi::load<u32>(camera);
    gabi::call_ptr<void>(gabi::load<u32>(vt + 0xDC), gabi::at<void>(camera), out);
}
inline void cameraTarget(u32 camera, cXyz* out) {
    u32 vt = gabi::load<u32>(camera);
    gabi::call_ptr<void>(gabi::load<u32>(vt + 0xFC), gabi::at<void>(camera), out);
}
inline void assure(SafeName* s) {
    u32 vt = s->vt;
    gabi::call_ptr<void>(gabi::load<u32>(vt + 0x14), s);
}
inline bool equalStrings(u32 left, u32 right) {
    bool equal = left == right;
    if (!equal) {
        equal = true;
        for (u32 n = 0; n < 0x40001; n++) {
            u8 a = gabi::load<u8>(left + n), b = gabi::load<u8>(right + n);
            if (a != b) {
                equal = false;
                break;
            }
            if (a == 0)
                break;
            if (n == 0x40000)
                equal = false;
        }
    }
    return equal;
}
inline s32 invokeAction(daWarphr_c* actor, const void* descriptor) {
    u32 address = gabi::ea(descriptor);
    s16 slot = gabi::load<s16>(address + 2), delta = gabi::load<s16>(address);
    s32 staff = actor->m304;
    u32 object = gabi::ea(actor) + s32(delta), target;
    if (slot < 0)
        target = gabi::load<u32>(address + 4);
    else {
        s16 offset = gabi::load<s16>(address + 6);
        u32 vt = gabi::load<u32>(object + s32(offset));
        target = gabi::load<u32>(vt + u32(s32(slot)) * 8 + 4);
    }
    return gabi::call_ptr<s32>(target, gabi::at<void>(object), staff);
}
template <class T> inline T* eventAt(T* table, u32 index) {
    return gabi::at<T>(gabi::ea(table) + index * sizeof(T));
}
struct DrawLists {
    u8 pad00[0x5D58];
    be<u32> projectionOpaque;
    u8 pad5D5C[4];
    be<u32> projectionTranslucent;
    u8 pad5D64[0x14];
    be<u32> normalOpaque, normalTranslucent;
};
struct CurrentDrawLists {
    be<u32> opaque, translucent;
};
inline DrawLists* drawLists(u32 address) {
    return gabi::at<DrawLists>(address);
}
inline u32 demoShape(u32 address) {
    return gabi::load<u32>(address + 0x28);
}
} // namespace warphr

enum GhrwpResource : s32 {
    dRes_INDEX_GHRWP_BDL_GHRWPA00_e = 5,
    dRes_INDEX_GHRWP_BDL_GHRWPB00_e = 6,
    dRes_INDEX_GHRWP_BTK_GHRWPA00_e = 12,
    dRes_INDEX_GHRWP_BTK_GHRWPB00_e = 13,
    dRes_INDEX_GHRWP_BRK_GHRWPB00_e = 9
};

namespace warphr {
struct InitTable {
    gabi::ptmf<void (daWarphr_c::*)(s32)> entries[5];
};
struct ActionTable {
    gabi::ptmf<bool (daWarphr_c::*)(s32)> entries[5];
};
} // namespace warphr

namespace warphr {
inline u8* ptr(u32 address) {
    return gabi::at<u8>(address);
}
inline mDoExt_btkAnm* btkAt(u32 address) {
    return gabi::at<mDoExt_btkAnm>(address);
}
inline mDoExt_brkAnm* brkAt(u32 address) {
    return gabi::at<mDoExt_brkAnm>(address);
}
inline JPABaseEmitter* emitterAt(u32 address) {
    return gabi::at<JPABaseEmitter>(address);
}
} // namespace warphr

WWHD_OFFSET(daWarphr_c, mpBtkAnm1, 0x3B8);
WWHD_OFFSET(daWarphr_c, mpModel2, 0x3BC);
WWHD_OFFSET(daWarphr_c, mpBrkAnm, 0x3C4);
WWHD_OFFSET(daWarphr_c, m2B0, 0x3CC);
WWHD_OFFSET(warphr::Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(warphr::Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(warphr::Emitter, mGlobalTranslation, 0x22C);
WWHD_SIZE(SafeName, 8);
WWHD_SIZE(warphr::InitTable, 40);
WWHD_SIZE(warphr::ActionTable, 40);

namespace warphr {
enum : u32 {
    ID_IT_SN_HRWARPA00 = 0x8291,
    ID_IT_SN_HRWARPB00 = 0x8292,
    ID_IT_SP_DEMO20_INDWP00 = 0xC2B9,
    JA_SE_LK_HL_WAPR_U_IN = 0x288F,
    JA_SE_LK_HL_WAPR_D_OUT = 0x288E,
    JA_SE_OBJ_HL_WAPR_EFF = 0x1088,
    ActorVtable = 0x10042660
};
inline u32 cullMatrixAddress(J3DModel* model) {
    return model ? gabi::ea(model) + offsetof(J3DModel, mBaseTransformMtx) : 0;
}
} // namespace warphr
