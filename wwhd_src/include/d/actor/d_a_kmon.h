#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
void Kmon_setEventName(dEvt_info_c *, const char *);
void KmonFrameCtrl_init(J3DFrameCtrl *, s16);
void KmonTransform_construct(void *);
void KmonMtxCalc_destroy(void *, s32);
// HD: embedded animation calculator layout and ordered inline constructor stores.
struct KmonBckLayoutHD {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mTransformMtx[0x34];
    be<u32> mMtxCalcVtable;
    u8 _4C[0x58 - 0x4C];
    be<u32> field_0x58;
    u8 _5C[0x7C - 0x5C];
    be<u32> field_0x7C, field_0x80, field_0x84, field_0x88;
};
WWHD_SIZE(KmonBckLayoutHD, 0x8C);
inline void KmonBck_construct(mDoExt_bckAnm *anm) {
    KmonBckLayoutHD *a = (KmonBckLayoutHD *)anm;
    KmonFrameCtrl_init(&a->mFrameCtrl, 0);
    a->mVtable = 0x1016E54C;
    KmonTransform_construct(a->mTransformMtx);
    a->field_0x7C = 0;
    a->mMtxCalcVtable = 0x1016D820;
    a->field_0x58 = 0;
    a->field_0x84 = 0;
    a->mVtable = 0x1001310C;
    a->field_0x80 = 0;
    a->field_0x88 = 0;
}
inline void KmonBck_destroy(mDoExt_bckAnm *anm) {
    KmonMtxCalc_destroy(&((KmonBckLayoutHD *)anm)->mVtable, 0);
}
/* daNpc_Ji1_c (HD): only the fields this unit touches (see d_a_npc_ji1.h) */
struct KmonActionHD {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
struct KmonOrcaHD : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3CC - 0x3AC];
    /* 0x3CC */ KmonActionHD mAction;
    /* 0x3D4 */ KmonActionHD field_0x2BC;
    /* 0x3DC */ KmonActionHD field_0x2C8;
    /* 0x3E4 */ u8 _3E4[0xEA0 - 0x3E4];
    /* 0xEA0 */ be<s8> field_0xC78;
    /* 0xEA1 */ u8 _EA1[0xEAC - 0xEA1];
    /* 0xEAC */ be<u32> field_0xC84;
};
WWHD_OFFSET(KmonOrcaHD, field_0xC84, 0xEAC);
#define ACT_eventAction 0x022596CC /* daNpc_Ji1_c::eventAction */

/* pointers to member functions (GHS, 8 bytes): as d_a_npc_ji1.h */
static inline bool ptmf_nonnull(KmonActionHD &p) {
    return p.i != 0;
}
static inline bool ptmf_eq(KmonActionHD &p, u32 fn) {
    return p.i == -1 && p.d == 0 && p.f == fn;
}
static inline void ptmf_set(KmonActionHD &p, u32 fn) {
    p.d = 0;
    p.i = -1;
    p.f = fn;
}
static inline void ptmf_copy(KmonActionHD &dst, KmonActionHD &src) {
    dst.d = (s16)src.d;
    dst.i = (s16)src.i;
    dst.f = (u32)src.f;
}
template <class... A> static inline BOOL ptmf_invoke(KmonActionHD &p, void *self, A... a) {
    s16 d = p.d;
    s16 i = p.i;
    void *obj = gabi::at<void>(gabi::ea(self) + d);
    if (i < 0)
        return gabi::call_ptr<BOOL>(p.f, obj, a...);
    u32 vt = gabi::load<u32>(gabi::ea(obj) + (s16)(u32)p.f);
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), obj, a...);
}
/* daNpc_Ji1_c::setAction (inline in the GameCube header) */
static inline void ji1_setAction(KmonOrcaHD *i_this, u32 action, void *arg) {
    if (!ptmf_eq(i_this->mAction, action)) {
        if (ptmf_nonnull(i_this->mAction)) {
            i_this->field_0xC78 = -1;
            ptmf_invoke(i_this->mAction, i_this, arg);
        }
        ptmf_copy(i_this->field_0x2BC, i_this->mAction);
        ptmf_set(i_this->mAction, action);
        i_this->field_0xC78 = 0;
        gabi::call_ptr<BOOL>(i_this->mAction.f, i_this, arg);
    }
}

struct daKmon_c : fopAc_ac_c {
    void set_mtx();
    BOOL CreateHeap();
    cPhs_State CreateInit();
    void checkTalk();

    cPhs_State _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ mDoExt_bckAnm mBckAnm;
    /* 0x4B8 */ dBgS_ObjAcch mAcch;
    /* 0x67C */ dBgS_AcchCir mAcchCir;
    /* 0x6BC */ gptr<KmonOrcaHD> mpJi1;
};
WWHD_OFFSET(daKmon_c, mBckAnm, 0x42C);
WWHD_OFFSET(daKmon_c, mAcch, 0x4B8);
WWHD_OFFSET(daKmon_c, mAcchCir, 0x67C);
WWHD_OFFSET(daKmon_c, mpJi1, 0x6BC);
WWHD_SIZE(daKmon_c, 0x6C0);
