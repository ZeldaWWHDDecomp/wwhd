/* Salvaged treasure box. GameCube members retain their order; HD has a 0x3AC actor base,
 * enlarged animation wrappers (bck 0x8C, btk 0x74, brk 0x78) and 0x24-byte lights.
 * Measured size 0x62C versus GameCube 0x350. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo.h"
struct SboxEmitter;
struct daSbox_c : fopAc_ac_c {
    static const char M_arcname[];
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel1;
    /* 0x3B8 */ mDoExt_bckAnm mBck1;
    /* 0x444 */ gptr<J3DModel> mpModel2;
    /* 0x448 */ mDoExt_bckAnm mBck2;
    /* 0x4D4 */ mDoExt_btkAnm mBtk;
    /* 0x548 */ mDoExt_brkAnm mBrk;
    /* 0x5C0 */ be<u8> mAction;
    /* 0x5C1 */ u8 _5c1[7];
    /* 0x5C8 */ be<s16> mTimer;
    /* 0x5CA */ u8 _5ca[2];
    /* 0x5CC */ be<s32> mStaff;
    /* 0x5D0 */ gptr<SboxEmitter> mEmitter;
    /* 0x5D4 */ be<s16> mVolumeTimer;
    /* 0x5D6 */ u8 _5d6[2];
    /* 0x5D8 */ be<f32> mDarkRatio;
    /* 0x5DC */ be<s16> mDarkTimer;
    /* 0x5DE */ u8 _5de[2];
    /* 0x5E0 */ LIGHT_INFLUENCE mLight;
    /* 0x604 */ LIGHT_INFLUENCE mEffectLight;
    /* 0x628 */ be<s16> mLightTimer;
    /* 0x62A */ be<u16> mFlags;
    bool chkFlag(u16 f) { return (mFlags & f) == f; }
    void setFlag(u16 f) { mFlags = mFlags | f; }
    void clrFlag(u16 f) { mFlags = mFlags & ~f; }
    BOOL CreateHeap();
    void calcMtx();
    void shipMtx();
    BOOL volmProc();
    BOOL darkProc();
    BOOL lightProc();
    void lightInit();
    BOOL CreateInit();
    cPhs_State create();
    void demoInitWait();
    BOOL demoProcWait();
    void demoInitOpen();
    void demoProcOpen();
    void demoInitDelete();
    void demoProcDelete();
    void demoInitCom();
    void demoProcCom();
    s32 getNowEventAction();
    BOOL demoProc();
    BOOL actionWait();
};
WWHD_OFFSET(daSbox_c, mBck1, 0x3B8);
WWHD_OFFSET(daSbox_c, mBck2, 0x448);
WWHD_OFFSET(daSbox_c, mBrk, 0x548);
WWHD_OFFSET(daSbox_c, mStaff, 0x5CC);
WWHD_OFFSET(daSbox_c, mFlags, 0x62A);
WWHD_SIZE(daSbox_c, 0x62C);

WWHD_OFFSET(daSbox_c, mPhase, 0x3AC);
WWHD_OFFSET(daSbox_c, mpModel1, 0x3B4);
WWHD_OFFSET(daSbox_c, mpModel2, 0x444);
WWHD_OFFSET(daSbox_c, mBtk, 0x4D4);
WWHD_OFFSET(daSbox_c, mAction, 0x5C0);
WWHD_OFFSET(daSbox_c, mTimer, 0x5C8);
WWHD_OFFSET(daSbox_c, mEmitter, 0x5D0);
WWHD_OFFSET(daSbox_c, mVolumeTimer, 0x5D4);
WWHD_OFFSET(daSbox_c, mDarkRatio, 0x5D8);
WWHD_OFFSET(daSbox_c, mDarkTimer, 0x5DC);
WWHD_OFFSET(daSbox_c, mLight, 0x5E0);
WWHD_OFFSET(daSbox_c, mEffectLight, 0x604);
WWHD_OFFSET(daSbox_c, mLightTimer, 0x628);

// HD: the emitter gains scale/color/status fields beyond the shared emitter prefix.
struct SboxEmitter {
    /* 0x000 */ u8 _000[8];
    /* 0x008 */ cXyz mEmitterScale;
    /* 0x014 */ u8 _014[0x48];
    /* 0x05C */ be<s32> mMaxFrame;
    /* 0x060 */ u8 _060[0x1C0];
    /* 0x220 */ cXyz mGlobalScale;
    /* 0x22C */ u8 _22C[12];
    /* 0x238 */ cXyz mGlobalParticleScale;
    /* 0x244 */ u8 _244[3];
    /* 0x247 */ be<u8> mGlobalAlpha;
    /* 0x248 */ u8 _248[12];
    /* 0x254 */ be<u32> mStatus;
};
WWHD_OFFSET(SboxEmitter, mEmitterScale, 8);
WWHD_OFFSET(SboxEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(SboxEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(SboxEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(SboxEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(SboxEmitter, mStatus, 0x254);
WWHD_SIZE(SboxEmitter, 0x258);
struct SboxShip : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[8];
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mMorf;
};
WWHD_OFFSET(SboxShip, mMorf, 0x3B4);
WWHD_SIZE(SboxShip, 0x3B8);
// HD: the bck cache is inline, with per-TU vtables and pointer fields.
struct SboxBckAnm {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> mVtable;
    /* 0x14 */ u8 mTransformCache[0x34];
    /* 0x48 */ be<u32> mCacheVtable;
    /* 0x4C */ u8 _4C[12];
    /* 0x58 */ be<u32> mCacheData;
    /* 0x5C */ u8 _5C[0x20];
    /* 0x7C */ be<u32> mCacheCount;
    /* 0x80 */ be<u32> mData;
    /* 0x84 */ be<u32> mResource;
    /* 0x62A */ /* 0x88 */ be<u32> mFlags;
};
WWHD_OFFSET(SboxBckAnm, mVtable, 0x10);
WWHD_OFFSET(SboxBckAnm, mTransformCache, 0x14);
WWHD_OFFSET(SboxBckAnm, mCacheVtable, 0x48);
WWHD_OFFSET(SboxBckAnm, mCacheData, 0x58);
WWHD_OFFSET(SboxBckAnm, mCacheCount, 0x7C);
WWHD_OFFSET(SboxBckAnm, mData, 0x80);
WWHD_OFFSET(SboxBckAnm, mResource, 0x84);
WWHD_OFFSET(SboxBckAnm, mFlags, 0x88);
WWHD_SIZE(SboxBckAnm, 0x8C);
enum : u32 {
    SBOX_VTABLE = 0x10039CF4,
    SBOX_ANM_BASE_VTABLE = 0x1016E54C,
    SBOX_CACHE_VTABLE = 0x1016D820,
    SBOX_BCK_VTABLE = 0x10039CCC,
};
enum {
    dRes_INDEX_DALWAYS_BDL_BOXSEA_e = 0x15,
    dRes_INDEX_DALWAYS_BCK_BOXOPENBOX_e = 8,
    dRes_INDEX_DALWAYS_BDL_IT_TAKARA_FLASH_e = 0x16,
    dRes_INDEX_DALWAYS_BCK_IT_TAKARA_FLASH2_e = 0xB,
    dRes_INDEX_DALWAYS_BTK_IT_TAKARA_FLASH_e = 0x25,
    dRes_INDEX_DALWAYS_BRK_IT_TAKARA_FLASH_e = 0x1E,
};
void sboxCutEnd(dEvent_manager_c *, s32);
void *sboxGetSubstance(dEvent_manager_c *, s32, const char *, s32);
s32 sboxGetAction(dEvent_manager_c *, s32, be<u32> *, s32, s32, s32);
s32 sboxGetStaff(dEvent_manager_c *, const char *, u32, s32);
BOOL sboxIsAdvance(dEvent_manager_c *, s32);
void sboxSetAllColorRatio(f32);
void sboxSetPriorityLight(LIGHT_INFLUENCE *);
s32 sboxGetReverb(s8);
void sboxStartSubBgm(u32);
daSbox_c *sboxAllocate(u32);
void sboxInitTransformCache(void *);
void sboxDestroyBckCache(void *, s32);
void sboxDestroyActor(fopAc_ac_c *, s32);

enum SboxFlag {
    OPEN_ANIMATION = 1,
    OPEN_EFFECT = 2,
    EFFECT_VISIBLE = 4,
    BOX_VISIBLE = 8,
    DARKEN_SCENE = 0x10,
    EFFECT_LIGHT = 0x20
};
enum SboxEventAction { ACT_WAIT, ACT_OPEN, ACT_DELETE };
enum : u32 {
    JA_SE_OBJ_TBOX_OPEN_S1 = 0x690C,
    JA_SE_OBJ_TBOX_UNLOCK = 0x690B,
    JA_SE_OBJ_TBOX_OPEN_S2 = 0x690D,
    JA_BGM_OPEN_BOX = 0x80000009
};
namespace dPa_name {
enum {
    ID_IT_JN_TAKARA_PAKAF = 0x1F1,
    ID_IT_JN_TAKARA_NAKAF = 0x1F2,
    ID_IT_JN_TAKARA_UEF = 0x1F3,
    ID_IT_JN_TAKARA_TSUBU = 0x1F4,
    ID_IT_JN_TAKARA_VOLM = 0x1F5,
    ID_IT_JN_TAKARA_PAKAF_SOTO = 0x1F6
};
}
