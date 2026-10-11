// Link's aura and water effects. HD animation channels and callback tails grow.
#pragma once
#include "d/actor/d_a_player_main_04_item.h"

struct LinkAuraEntry {
    gptr<J3DModel> mpModel;
    be<f32> mFrame;
};
WWHD_SIZE(LinkAuraEntry, 8);
WWHD_OFFSET(LinkAuraEntry, mFrame, 4);
struct LinkAuraRippleCallback {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
    u8 _08[8];
    be<f32> mRate;
};
WWHD_SIZE(LinkAuraRippleCallback, 0x14);
WWHD_OFFSET(LinkAuraRippleCallback, mpEmitter, 4);
WWHD_OFFSET(LinkAuraRippleCallback, mRate, 0x10);
struct LinkAuraSwimTailCallback {
    be<u32> __vtbl;
    be<u8> mEnd;
    u8 _05[3];
    be<f32> mSpeedRate, mWaterY, mWaterFlatY;
    cXyz mPos;
    u8 _20[8];
};
WWHD_SIZE(LinkAuraSwimTailCallback, 0x28);
WWHD_OFFSET(LinkAuraSwimTailCallback, mSpeedRate, 8);
WWHD_OFFSET(LinkAuraSwimTailCallback, mPos, 0x14);
struct LinkAuraLayout {
    u8 _0000[0x5378];
    gptr<J3DModel> mpSuimenMunyaModel;
    LinkItemBtk mpSuimenMunyaBtk;
    u8 _53F0[0x55E4 - 0x53F0];
    LinkAuraEntry mMagicArmorAuraEntries[6];
    LinkItemBrk mYaura00rBrk;
    LinkItemBtk mpYaura00Btk;
    gptr<J3DModel> mpYmgcs00Model;
    LinkItemBrk mYmgcs00Brk;
    LinkItemBtk mpYmgcs00Btk;
    u8 _57F0[0x6694 - 0x57F0];
    LinkAuraRippleCallback m3280;
    LinkAuraSwimTailCallback mSwimTailEcallBack[2];
    u8 _66F8[0x6814 - 0x66F8];
    LinkItemMtxCallback m3400;
};
WWHD_OFFSET(LinkAuraLayout, mpSuimenMunyaBtk, 0x537C);
WWHD_OFFSET(LinkAuraLayout, mMagicArmorAuraEntries, 0x55E4);
WWHD_OFFSET(LinkAuraLayout, mYaura00rBrk, 0x5614);
WWHD_OFFSET(LinkAuraLayout, mpYaura00Btk, 0x568C);
WWHD_OFFSET(LinkAuraLayout, mpYmgcs00Model, 0x5700);
WWHD_OFFSET(LinkAuraLayout, mYmgcs00Brk, 0x5704);
WWHD_OFFSET(LinkAuraLayout, mpYmgcs00Btk, 0x577C);
WWHD_OFFSET(LinkAuraLayout, m3280, 0x6694);
WWHD_OFFSET(LinkAuraLayout, mSwimTailEcallBack, 0x66A8);
WWHD_OFFSET(LinkAuraLayout, m3400, 0x6814);
WWHD_SIZE(LinkAuraLayout, 0x6820);
inline LinkAuraLayout& LinkAura(daPy_lk_c* player) { return *reinterpret_cast<LinkAuraLayout*>(player); }
struct LinkAuraEmitter {
    u8 _00[0x14];
    cXyz mEmitterTranslation;
    u8 _20[0x14];
    be<f32> mRate;
    u8 _38[0x20];
    be<f32> mSpread;
    u8 _5C[0x220 - 0x5C];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
    GXColor mGlobalPrmColor;
};
WWHD_OFFSET(LinkAuraEmitter, mEmitterTranslation, 0x14);
WWHD_OFFSET(LinkAuraEmitter, mRate, 0x34);
WWHD_OFFSET(LinkAuraEmitter, mSpread, 0x58);
WWHD_OFFSET(LinkAuraEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(LinkAuraEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(LinkAuraEmitter, mGlobalPrmColor, 0x244);
WWHD_SIZE(LinkAuraEmitter, 0x248);
inline LinkAuraEmitter* LinkAura_emitter(u32 emitter) { return gabi::at<LinkAuraEmitter>(emitter); }
// HD: this site tests the full halfword; the shared getMagic accessor reads one byte.
struct LinkAuraSave {
    u8 _00[0x24];
    be<u16> mMagic;
};
WWHD_OFFSET(LinkAuraSave, mMagic, 0x24);
WWHD_SIZE(LinkAuraSave, 0x26);
struct LinkAuraSavePointer {
    gptr<LinkAuraSave> mpSave;
};
WWHD_SIZE(LinkAuraSavePointer, 4);
// HD: this TU owns the single-ripple callback; this caller only passes its address.
struct LinkAuraSingleRippleCallback {
    u8 mStorage[0x1C];
};
WWHD_SIZE(LinkAuraSingleRippleCallback, 0x1C);
struct LinkAuraApi {
    static void initBrk(LinkItemBrk*, J3DModelData*, u32, s32, s32, f32, s32, s32, s32, s32);
    static void endRipple(LinkAuraRippleCallback*);
    static void subtractVector(const cXyz*, const cXyz*, cXyz*);
    static void StartQuake(dVibration_c*, s32, s32, cXyz*);
    static void StopQuake(dVibration_c*, s32);
};
namespace LinkAuraConstants {
enum : u32 {
    daPyProc_DEMO_DEAD_e = 0xB2,
    daPyStts0_SWIM_e = 0x100000,
    ModeFlg_IN_SHIP = 0x2000,
    ModeFlg_HANG = 0x20,
    dBgS_Attr_WATERFALL_e = 0x17,
    ID_AK_JN_HAMON00 = 0x33,
    ID_IT_JN_WP_HAMON03 = 0x3F,
    dRes_INDEX_LINK_BRK_YAURA00_R_e = 0x58,
    dRes_INDEX_LINK_BRK_YMGCS00_MS_e = 0x59,
    dRes_INDEX_LINK_BRK_YAURA00_G_e = 0x57,
    dRes_INDEX_LINK_BRK_YMGCS00_TS_e = 0x5A,
    JA_SE_LK_MG_SHELD_USING = 0x107E,
    dEvtFlag_TALK_e = 4,
    dPaStatus_WAVE_e = 1,
    J3DMtxBuffer_DIRTY = 0x10,
};
}

// HD: these are address-only views of d_demo's storage; d_demo retains ownership.
struct LinkAuraDemoObject;
struct LinkAuraDemoState {
    /* 0x00 */ u8 _00[0x18];
    /* 0x18 */ gptr<LinkAuraDemoObject> mDemoObj;
    /* 0x1C */ u8 _1C[0xC];
    /* 0x28 */ be<u32> mFrameNoMsg;
};
WWHD_OFFSET(LinkAuraDemoState, mDemoObj, 0x18);
WWHD_OFFSET(LinkAuraDemoState, mFrameNoMsg, 0x28);
WWHD_SIZE(LinkAuraDemoState, 0x2C);
struct LinkAuraDemoArchive {
    char mName[1];
};
WWHD_SIZE(LinkAuraDemoArchive, 1);
inline LinkAuraDemoState& LinkAura_demoState() {
    return gabi::object_at<LinkAuraDemoState>(0x101D5FE4);
}
inline LinkAuraDemoArchive& LinkAura_demoArchive() {
    return gabi::object_at<LinkAuraDemoArchive>(0x1047E6B8);
}
struct LinkAuraDemoApi {
    // HD: retain the complete actor result register and unsigned actor index.
    static u32 getActor(LinkAuraDemoObject*, u32);
    static void assureTermination(void*);
};
