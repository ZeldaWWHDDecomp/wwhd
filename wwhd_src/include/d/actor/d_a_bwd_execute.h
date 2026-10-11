// Molgera execute split: private ABI declarations.
#pragma once
#include "d/actor/d_a_bwd.h"

BOOL bwdExecHookshotReturn(u32);
void bwdExecBgmStart(u32);
void bwdExecMonsterSound(u32, cXyz*, u32, u32, s32);
u32 bwdExecCameraBody();
void bwdExecCameraType(u32, const char*, void*);
void bwdExecColorSet(u8, u8, f32);
void bwdExecAddCalcPos(cXyz*, cXyz*, f32, f32);
void bwdExecMtxPush();
void bwdExecMtxPull();
void bwdExecVectorAdd(cXyz*, cXyz*, cXyz*);
void bwdExecVectorSubtract(cXyz*, cXyz*, cXyz*);
void bwdExecRegisterCollider(cCcS*, void*);
void bwdExecSearch(u32, bwd_class*);
void bwdExecMove(bwd_class*);
void bwdExecDemoCamera(bwd_class*);
void bwdExecRemoveSmoke(void*);
void bwdExecRemoveFollow(void*);

namespace bwd_execute_detail {
enum BWD_JNT { BWD_JNT_SITA_e = 0x1D };
enum BWD_BTK {
    dRes_INDEX_BWD_BTK_TAKI_END_e = 0x33,
    dRes_INDEX_BWD_BTK_TAKI_WAIT_e = 0x35,
};

// HD: generated history indices retain 32-bit guest address wrapping.
inline csXyz* historyAngles(bwd_class* actor, u32 index) {
    return gabi::at<csXyz>(gabi::ea(&actor->m1108[0]) + sizeof(csXyz) * index);
}
inline cXyz* historyPosition(bwd_class* actor, u32 index) {
    return gabi::at<cXyz>(gabi::ea(&actor->m0508[0]) + sizeof(cXyz) * index);
}

struct CollisionOffsets {
    cXyz mPosition[9];
};
WWHD_SIZE(CollisionOffsets, 0x6C);
struct PlayerVtable {
    u8 mUnused[0xEC];
    gabi::gfn mSetOutPower;
};
WWHD_OFFSET(PlayerVtable, mSetOutPower, 0xEC);
struct Player {
    u8 mUnused[0xB4];
    gptr<PlayerVtable> mVtable;
};
WWHD_OFFSET(Player, mVtable, 0xB4);
// HD: dispatch the existing player virtual slot without changing its argument ABI.
inline void setOutPower(PlayerVtable* table, fopAc_ac_c* player, f32 power, s16 angle) {
    gabi::call(table->mSetOutPower.get(), player, power, angle, 0);
}
// HD: the timer loops span named fields and padding in the original actor layout.
struct Timers {
    u8 mUnused[0x1B50];
    be<s16> mAction[5];
};
WWHD_OFFSET(Timers, mAction, 0x1B50);
WWHD_SIZE(Timers, 0x1B5A);
struct EffectTimers {
    u8 mUnused[0x3D58];
    be<s16> mEffect[10];
};
WWHD_OFFSET(EffectTimers, mEffect, 0x3D58);
WWHD_SIZE(EffectTimers, 0x3D6C);
} // namespace bwd_execute_detail
