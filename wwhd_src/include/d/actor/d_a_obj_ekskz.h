#pragma once
#include "gc_api/gc_api.h"
template <u32 Address> inline constexpr u32 ekskzCall = Address;
struct EkskzEmitter : JPABaseEmitter {
    u8 _70[0x244 - 0x70];
    GXColor mPrmColor;
    GXColor mEnvColor;
};
WWHD_OFFSET(EkskzEmitter, mPrmColor, 0x244);
WWHD_OFFSET(EkskzEmitter, mEnvColor, 0x248);
WWHD_SIZE(EkskzEmitter, 0x24C);
struct EkskzLight {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mColorSigned[4];
    be<f32> mFactors[8];
};
WWHD_SIZE(EkskzLight, 0x44);
struct EkskzTevView {
    EkskzLight mLight;
    u8 _44[0xC0 - 0x44];
    EkskzLight mLightCopy0;
    u8 _104[0x40];
    EkskzLight mLightCopy1;
};
WWHD_OFFSET(EkskzTevView, mLightCopy0, 0xC0);
WWHD_OFFSET(EkskzTevView, mLightCopy1, 0x144);
struct EkskzBckView : mDoExt_baseAnm {
    be<u32> mMtxCalcVtable;
    u8 mMtxCalcBase[0x34];
    be<u32> mBlendVtable;
    u8 mBlendState[0xC];
    be<u32> mBlendFlags;
    u8 mTransformState[0x20];
    be<u32> mAnmFlags;
    be<u32> mAnmIndex;
    be<u32> mAnmData;
    be<u32> mAnmTarget;
};
WWHD_OFFSET(EkskzBckView, mMtxCalcVtable, 0x10);
WWHD_OFFSET(EkskzBckView, mBlendVtable, 0x48);
WWHD_OFFSET(EkskzBckView, mBlendFlags, 0x58);
WWHD_OFFSET(EkskzBckView, mAnmFlags, 0x7C);
WWHD_SIZE(EkskzBckView, 0x8C);
struct EkskzText {
    char bytes[1];
};

#define SAFESTRING_VTBL 0x10027D68

#define ACT_VTBL 0x10027E98     /* daObjEkskz::Act_c vtable (HD virtual destructor) */
#define CYL_AAB_VTBL 0x10027D80 /* this TU's cM3dGAab vtable */
#define BCK_VTBL 0x10027D90     /* this TU's mDoExt_bckAnm vtable */

enum {
    dRes_INDEX_EKSKZ_BCK_e = 0x08,
    dRes_INDEX_EKSKZ_BDL_e = 0x0B,
    dRes_INDEX_EKSKZ_BDL_EFF_e = 0x0E,
    dRes_INDEX_EKSKZ_BRK_e = 0x11,
    dRes_INDEX_EKSKZ_BTK_e = 0x14,
    dRes_INDEX_EKSKZ_DZB_e = 0x17,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1988 HD: mDoAud_seStart(id) (one argument; also local in d_a_mo2.h) */
static inline void ekskz_mDoAud_seStart_1(u32 id) { gabi::call(ekskzCall<0x025E1988>, id); }
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (the matcher calls it init), 025E8154 init [as d_a_oq.h] */
static inline void ekskz_mDoExt_brkAnm_ct(void *p) { gabi::call(ekskzCall<0x025E80D0>, p); }
static inline BOOL ekskz_mDoExt_brkAnm_init(void *a, J3DModelData *d, void *brk, s32 anmPlay,
                                            s32 mode, f32 rate, s16 start, s16 end, bool modify,
                                            s32 entry) {
    return gabi::call<BOOL>(ekskzCall<0x025E8154>, a, d, brk, anmPlay, mode, rate, start, end,
                            modify, entry);
}
/* 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32) */
static inline void ekskz_mDoExt_brkAnm_entry(void *a, J3DModelData *d, f32 frame) {
    gabi::call(ekskzCall<0x025E83FC>, a, d, frame);
}
/* 0255FFF4 dKy_tevstr_init(tevstr, roomNo, u8) [as d_a_kb.h] */
static inline void ekskz_dKy_tevstr_init(dKy_tevstr_c *t, s8 roomNo, u8 p) {
    gabi::call(ekskzCall<0x0255FFF4>, t, roomNo, p);
}
/* GHS array helpers */
static inline void ekskz___construct_array(void *p, u32 n, u32 size, u32 ctor) {
    gabi::call(ekskzCall<0x028EFFD0>, p, n, size, ctor);
}
static inline void ekskz___destroy_arr(void *p, u32 n, u32 size, u32 dtor) {
    gabi::call(ekskzCall<0x028F0164>, p, n, size, dtor, 0, 0);
}

/* dKy_tevstr_c::dKy_tevstr_c (HD, inline): three 0x44-byte light blocks at +0, +0xC0, +0x144
 * from the default at 0x1016E414 (as in d_a_kb / d_a_npc_bmsw) */
static inline void tevstr_ct(dKy_tevstr_c *t) {
    auto *destination = reinterpret_cast<EkskzTevView *>(t);
    const EkskzLight &source = gabi::object_at<EkskzLight>(0x1016E414);
    EkskzLight *blocks[3] = {&destination->mLight, &destination->mLightCopy0,
                             &destination->mLightCopy1};
    for (EkskzLight *block : blocks) {
        for (int i = 0; i < 6; i++)
            block->mPositionDirection[i] = (f32)source.mPositionDirection[i];
        for (int i = 0; i < 4; i++)
            block->mColor[i] = (u8)source.mColor[i];
        for (int i = 0; i < 4; i++)
            block->mColorSigned[i] = (s16)source.mColorSigned[i];
        for (int i = 0; i < 8; i++)
            block->mFactors[i] = (f32)source.mFactors[i];
    }
}
/* inline mDoExt_bckAnm::mDoExt_bckAnm() (HD, 0x8C bytes; as d_a_itembase) */
static inline void bckAnm_ct(u32 address) {
    auto *b = gabi::at<EkskzBckView>(address);
    gabi::call(ekskzCall<0x027F2BC0>, b, 0);
    b->mMtxCalcVtable = 0x1016E54C;
    gabi::call(ekskzCall<0x027DA984>, &b->mMtxCalcBase);
    b->mAnmIndex = 0;
    b->mAnmFlags = 0;
    b->mAnmTarget = 0;
    b->mBlendFlags = 0;
    b->mMtxCalcVtable = BCK_VTBL;
    b->mAnmData = 0;
    b->mBlendVtable = 0x1016D820;
}
/* JPABaseEmitter global colours (HD: prm rgb at +0x244, env rgb at +0x248) */
static inline void emitter_setColors(dPa_smokeEcallBack &callback, const GXColor &color) {
    auto *emitter = reinterpret_cast<EkskzEmitter *>(callback.mpEmitter.get());
    u8 r = color.r, g = color.g, b = color.b;
    emitter->mPrmColor.r = r;
    emitter->mPrmColor.g = g;
    emitter->mPrmColor.b = b;
    g = color.g;
    r = color.r;
    emitter = reinterpret_cast<EkskzEmitter *>(callback.mpEmitter.get());
    b = color.b;
    emitter->mEnvColor.g = g;
    emitter->mEnvColor.r = r;
    emitter->mEnvColor.b = b;
}

namespace daObjEkskz {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34 **mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel; /* GameCube 0x2D0 */
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Cyl mCyl;
    /* 0x558 */ gptr<J3DModel> mpModelEff;
    /* 0x55C */ mDoExt_bckAnm mBckAnm;  /* HD 0x8C */
    /* 0x5E8 */ mDoExt_baseAnm mBrkAnm; /* mDoExt_brkAnm, HD 0x78 */
    /* 0x5F8 */ u8 _5F8[0x660 - 0x5F8];
    /* 0x660 */ mDoExt_btkAnm mBtkAnm; /* HD 0x74 */
    /* 0x6D4 */ be<u8> m480;           /* broken */
    /* 0x6D5 */ u8 _6D5[3];
    /* 0x6D8 */ dPa_smokeEcallBack
        mSmokeCallback[4]; /* dPa_smokeEcallBack[4]: vtable +0, emitter +4 */
    /* 0x758 */ be<u32> m504[2];
    /* 0x760 */ gptr<JPABaseEmitter> mpSmoke0;
    /* 0x764 */ be<u32> m510;
    /* 0x768 */ dKy_tevstr_c mTevStr;
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mBckAnm, 0x55C);
WWHD_OFFSET(Act_c, mBtkAnm, 0x660);
WWHD_OFFSET(Act_c, mSmokeCallback, 0x6D8);
WWHD_OFFSET(Act_c, mTevStr, 0x768);
WWHD_SIZE(Act_c, 0x930);
} // namespace daObjEkskz
using daObjEkskz::Act_c;

inline void ekskz_destroyMtxCalc(void *calc) { gabi::call(ekskzCall<0x027F3628>, calc, 0); }

namespace ekskzAbi {
enum : u32 { HeaderStatics = 0x104697A4, HeaderStaticsData = 0x101C9030 };
}
inline u32 ekskz_createSmokeCallback(void *callback) {
    return gabi::call<u32>(ekskzCall<0x025A5B18>, callback, (u8)1);
}
inline void ekskz_destroyActor(void *actor) { gabi::call(ekskzCall<0x025D50BC>, actor, 0); }
