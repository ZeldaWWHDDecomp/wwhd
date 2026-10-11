/* Glowing Tower of the Gods light bridge. HD enlarges actor and animation prefixes.
 * Offsets are HD; retail member names, including unk31C, are retained. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/JSystem/JPAEmitter.h"
struct LbridgeEmitter : JPABaseEmitter {
    u8 _70[0x247 - sizeof(JPABaseEmitter)];
    be<u8> mGlobalAlpha;
    u8 _248[12];
    be<u32> mStatus;
    enum { STOP_CREATE = 1, STOP_DRAW = 4 };
    void stopDrawParticle() { mStatus |= STOP_DRAW; }
    void playDrawParticle() { mStatus &= ~STOP_DRAW; }
    void playCreateParticle() { mStatus &= ~STOP_CREATE; }
    void becomeInvalidEmitter() {
        mMaxFrame = -1;
        mStatus |= STOP_CREATE;
    }
};
WWHD_OFFSET(LbridgeEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(LbridgeEmitter, mStatus, 0x254);
WWHD_SIZE(LbridgeEmitter, 0x258);
struct LbridgeText {
    char bytes[1];
};
WWHD_SIZE(LbridgeText, 1);
struct daLbridge_c : fopAc_ac_c {
    enum DemoState { NO_DEMO, APPEAR_DEMO, DISAPPEAR_DEMO };
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    void setMoveBGMtx();
    bool _execute();
    void sw_check();
    void demo();
    void appear_bridge();
    void disappear_bridge();
    void set_on_se();
    void set_off_se();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ Mtx34 mMtx;
    /* 0x3EC */ mDoExt_btkAnm mBtkAnm;
    /* 0x460 */ mDoExt_bpkAnm
        mBpkAnm; /* J3DFrameCtrl at +0: rate +0, frame +4, start +8, end +0xA */
    /* 0x4D4 */ mDoExt_brkAnm mBrkAnm;
    /* 0x54C */ gptr<LbridgeEmitter> mpEmitter;
    /* 0x550 */ be<s32> mSwitchNo;
    /* 0x554 */ be<s16> mAppearEventIdx;
    /* 0x556 */ be<s16> mDisappearEventIdx;
    /* 0x558 */ be<s16> unk31C;
    /* 0x55A */ be<u8> mTimer;
    /* 0x55B */ be<u8> mIsSw;
};
WWHD_OFFSET(daLbridge_c, mMtx, 0x3BC);
WWHD_OFFSET(daLbridge_c, mBtkAnm, 0x3EC);
WWHD_OFFSET(daLbridge_c, mBpkAnm, 0x460);
WWHD_OFFSET(daLbridge_c, mBrkAnm, 0x4D4);
WWHD_OFFSET(daLbridge_c, mpEmitter, 0x54C);
WWHD_OFFSET(daLbridge_c, mIsSw, 0x55B);
WWHD_SIZE(daLbridge_c, 0x55C);

namespace daLbridge_prm {
inline u8 getSwitchNo(daLbridge_c *ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
}; // namespace daLbridge_prm

WWHD_OFFSET(daLbridge_c, mPhs, 0x3AC);
WWHD_OFFSET(daLbridge_c, mpModel, 0x3B4);
WWHD_OFFSET(daLbridge_c, mpBgW, 0x3B8);
WWHD_OFFSET(daLbridge_c, mSwitchNo, 0x550);
WWHD_OFFSET(daLbridge_c, mAppearEventIdx, 0x554);
WWHD_OFFSET(daLbridge_c, mDisappearEventIdx, 0x556);
WWHD_OFFSET(daLbridge_c, unk31C, 0x558);
WWHD_OFFSET(daLbridge_c, mTimer, 0x55A);
enum {
    dRes_INDEX_GBRG00_BDL_GBRG00_e = 7,
    dRes_INDEX_GBRG00_BPK_GBRG00_e = 0xA,
    dRes_INDEX_GBRG00_BRK_GBRG00_e = 0xD,
    dRes_INDEX_GBRG00_BTK_GBRG00_e = 0x10,
    dRes_INDEX_GBRG00_DZB_HHASHI1_e = 0x13,
};
enum { JA_SE_OBJ_L_BRIDGE_ON = 0x6956, JA_SE_OBJ_L_BRIDGE_OFF = 0x6957 };
enum { ID_IT_SN_RBRIDGE_FLSH00 = 0x810F, ID_IT_SN_RBRIDGE_APP00 = 0x8119 };
enum { UNK_0E01 = 0xE01, UNK_0F40 = 0xF40 };

/* mDoExt_bpkAnm (HD 0x74): constructor 025E7480, init 025E74FC (entry on the stack), entry 025E779C
 */
static inline void lbridge_mDoExt_bpkAnm_ct(void *p) {
    constexpr u32 callTarget = 0x025E7480;
    gabi::call(callTarget, p);
}
static inline BOOL lbridge_mDoExt_bpkAnm_init(void *a, J3DModelData *d, void *bpk, s32 play,
                                              s32 mode, f32 rate, s16 start, s16 end, bool modify,
                                              s32 entry) {
    constexpr u32 callTarget = 0x025E74FC;
    return gabi::call<BOOL>(callTarget, a, d, bpk, play, mode, rate, start, end, modify, entry);
}
static inline void lbridge_mDoExt_bpkAnm_entryFrame(void *a, J3DModelData *d, f32 frame) {
    constexpr u32 callTarget = 0x025E779C;
    gabi::call(callTarget, a, d, frame);
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 (entry on the stack); entry 025E83FC
 * (shared) */
static inline void lbridge_mDoExt_brkAnm_construct(void *p) {
    constexpr u32 callTarget = 0x025E80D0;
    gabi::call(callTarget, p);
}
static inline BOOL lbridge_mDoExt_brkAnm_init(void *a, J3DModelData *d, void *brk, s32 play,
                                              s32 mode, f32 rate, s16 start, s16 end, bool modify,
                                              s32 entry) {
    constexpr u32 callTarget = 0x025E8154;
    return gabi::call<BOOL>(callTarget, a, d, brk, play, mode, rate, start, end, modify, entry);
}
/* 0200F4FC lbridge_cLib_chaseUC(u8* v, u8 target, u8 step); 0207A9A0 cLib_calcTimer<u8> */
static inline void lbridge_cLib_chaseUC(be<u8> *v, u8 target, u8 step) {
    constexpr u32 callTarget = 0x0200F4FC;
    gabi::call(callTarget, v, target, step);
}
static inline u8 lbridge_cLib_calcTimer_u8(be<u8> *t) {
    constexpr u32 callTarget = 0x0207A9A0;
    return gabi::call<u8>(callTarget, t);
}
/* lbridge_fopAcM_onDraw: lbridge_fopDwTg_ToDrawQ(&draw_tag (+0xDC),
 * lbridge_fpcLf_GetPriority(this)) (as in d_a_npc_md) */
static inline s16 lbridge_fpcLf_GetPriority(void *a) {
    constexpr u32 callTarget = 0x025DF2B8;
    return gabi::call<s16>(callTarget, a);
}
static inline void lbridge_fopDwTg_ToDrawQ(u32 tag, s16 prio) {
    constexpr u32 callTarget = 0x025DA874;
    gabi::call(callTarget, tag, prio);
}
static inline void lbridge_fopAcM_onDraw(fopAc_ac_c *a) {
    s16 prio = lbridge_fpcLf_GetPriority(a);
    lbridge_fopDwTg_ToDrawQ(gabi::ea(&a->draw_tag), prio);
}

inline void lbridge_destroyActor(fopAc_ac_c *p, s32 flags) {
    constexpr u32 target = 0x025D50BC;
    gabi::call(target, p, flags);
}
enum { LBRIDGE_VTBL = 0x10013DB4, LBRIDGE_MOVE_BG = 1 };
