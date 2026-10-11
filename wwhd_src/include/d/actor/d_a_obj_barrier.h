#pragma once
#include "bindings.h"
#include "d/d_event.h"
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
// HD: Animation containers and effect textures use expanded layouts.

enum BarrierResource_e {
    dRes_INDEX_YCAGE_BDL_YCAGE00_e = 10,
    dRes_INDEX_YCAGE_BTK_YCAGE00_e = 18,
    dRes_INDEX_YCAGE_BRK_YCAGE00_e = 14,
    dRes_INDEX_YCAGE_DZB_KEKKAI_e = 22,
    dRes_INDEX_YCAGE_BDL_YHRBR00_e = 11,
    dRes_INDEX_YCAGE_BTK_YHRBR00_e = 19,
    dRes_INDEX_YCAGE_BCK_YHRBR00_e = 7,
    dRes_INDEX_YCAGE_BRK_YHRBR00_e = 15
};

struct BarrierCylinder : dCcD_GObjInf {
    u8 padF8[0x18];
    be<u32> mAttrVtable, mShapeVtable;
    cM3dGCyl mCyl;
};

struct BarrierBrkAnm : mDoExt_baseAnm {
    u8 state[0x78 - 0x10];
};
struct daObjBarrier_anm_c {
    BOOL init();
    gptr<J3DModel> mpModel;
    mDoExt_btkAnm mBtk;
    BarrierBrkAnm mBrk;
    be<f32> mBrkFrame;
};
struct daObjBarrier_ef_c {
    void setDummyTexture(u32 index);
    BOOL checkHitActor(void* actor);
    void birth(void* actor, cXyz* center, cXyz* hit, s32 check, f32 radius);
    BOOL init();
    void create();
    void execute();
    void draw();
    be<u32> mActiveEffFlags;
    gptr<J3DModel> mpModel[4];
    mDoExt_btkAnm mBtk[4];
    mDoExt_bckAnm mBck[4];
    BarrierBrkAnm mBrk[4];
    cXyz mPos[4];
    be<s16> mAngle[4];
    gptr<void> mHitActor[4];
};
struct daObjBarrier_c : fopAc_ac_c {
    enum Proc_e { PROC_BREAK_START_WAIT_e, PROC_BREAK_ORDER_e, PROC_BREAK_END_WAIT_e };
    enum Param_e { PRM_DAMAGE_W = 1, PRM_DAMAGE_S = 16, PRM_MOYA_W = 1, PRM_MOYA_S = 8 };
    BOOL create_heap();
    void init_mtx();
    BOOL _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    void brkAnmPlay();
    void break_start_wait_proc();
    void break_order_proc();
    void break_end_wait_proc();
    BOOL break_check();
    void checkCollision_At();
    BOOL checkCollision_Tg();
    void registCollisionTable();

    daObjBarrier_anm_c mAnm;               // 3AC
    request_of_phase_process_class mPhase; // 4A0
    gptr<dBgW> mpBgW;                      // 4A8
    Mtx34 mBgMtx;                          // 4AC
    dCcD_Stts mAtStts, mTgStts;            // 4DC,518
    BarrierCylinder mAtCyl, mTgCyl;        // 554,684
    daObjBarrier_ef_c mEffect;             // 7B4
    be<u8> mBarrierActive;
    u8 reserved[3]; // DF0
    be<s32> mMoya;  // DF4
    be<s16> mEventID;
    u8 reservedEvent[2];  // DF8
    be<s32> mBarrierProc; // DFC
};
WWHD_SIZE(BarrierBrkAnm, 0x78);
WWHD_SIZE(daObjBarrier_anm_c, 0xF4);
WWHD_SIZE(daObjBarrier_ef_c, 0x63C);
WWHD_SIZE(daObjBarrier_c, 0xE00);
WWHD_OFFSET(daObjBarrier_c, mEffect, 0x7B4);
WWHD_OFFSET(daObjBarrier_ef_c, mBck, 0x1E4);
WWHD_OFFSET(daObjBarrier_ef_c, mHitActor, 0x62C);

WWHD_OFFSET(daObjBarrier_c, mAnm, 0x3AC);
WWHD_OFFSET(daObjBarrier_c, mPhase, 0x4A0);
WWHD_OFFSET(daObjBarrier_c, mBgMtx, 0x4AC);
WWHD_OFFSET(daObjBarrier_c, mAtCyl, 0x554);
WWHD_OFFSET(daObjBarrier_c, mTgCyl, 0x684);
WWHD_OFFSET(daObjBarrier_c, mBarrierActive, 0xDF0);
WWHD_OFFSET(daObjBarrier_c, mBarrierProc, 0xDFC);
WWHD_OFFSET(daObjBarrier_ef_c, mBrk, 0x414);
WWHD_OFFSET(daObjBarrier_ef_c, mPos, 0x5F4);
WWHD_OFFSET(daObjBarrier_ef_c, mAngle, 0x624);

struct BarrierPlayer : fopAc_ac_c {
    be<u8> mCutType;
};
struct BarrierGameInfo {
    u8 pad0[0x12A0];
    dBgS mBgS;
    u8 pad1[0x26A4 - 0x12A0 - sizeof(dBgS)];
    u8 mCollision[1];
    u8 pad2[0x51D0 - 0x26A5];
    dEvt_control_c mEvent;
    u8 mEventManager[1];
    u8 pad3[0x5AB0 - 0x52C5];
    gptr<void> mpParticle;
    u8 pad4[0x5B2C - 0x5AB4];
    gptr<BarrierPlayer> mpPlayer;
    u8 pad5[0x5BAC - 0x5B30];
    be<u16> mSwordActive;
    u8 pad6[0x5D70 - 0x5BAE];
    be<u32> mBgDrawList, mBgXluDrawList, mDrawList, mXluDrawList;
    u8 pad7[0x5D8C - 0x5D80];
    be<u32> mInvisibleDrawList, mInvisibleXluDrawList;
};
struct BarrierSave {
    u8 pad0[0x2E];
    be<u8> mEquippedSword;
    u8 pad1[0x644 - 0x2F];
    u8 mEvent[1];
};
struct BarrierBackground {
    u8 pad[0xA8];
    be<u32> mCorrectionCallback;
};
struct BarrierTextureObject {
    u8 pad[8];
    be<u32> mWidth, mHeight;
};
struct BarrierResTIMG {
    be<u8> mFormat, mAlphaEnabled;
    be<u16> mWidth, mHeight;
    be<u8> mWrapS, mWrapT, mIndexTexture, mColorFormat;
    be<u16> mNumColors;
    be<u32> mPaletteOffset;
    u8 pad10[0xC];
    be<u32> mImageOffset;
    gptr<BarrierTextureObject> mpTexture;
};
union BarrierImageWords {
    BarrierResTIMG mImage;
    be<u32> mWords[9];
};
struct BarrierTexture {
    be<u16> mNum;
    u8 pad2[2];
    gptr<BarrierImageWords> mpImages;
    be<u32> mUsedHi, mUsedLo, mUsedHiExtra, mUsedLoExtra, mMaskHi, mMaskLo;
};
struct BarrierModelData {
    u8 pad[0x30];
    gptr<BarrierTexture> mpTexture;
    gptr<JUTNameTab> mpTextureNames;
};
struct BarrierTextureScratch {
    u8 pad[8];
    BarrierImageWords mImage;
    u8 tail[36];
};
struct BarrierDummyName {
    be<u8> mBytes[8];
};
struct BarrierArchive {
    gptr<char> mpName;
    be<u32> mVtable;
};
struct BarrierTrigEntry {
    be<f32> mSin, mCos;
};
struct BarrierTrigTable {
    BarrierTrigEntry mEntries[8192];
};
struct BarrierHeaderStatics {
    be<f32> mMin, mMax;
    union {
        be<u32> mRandom;
        be<u8> mRandomBytes[4];
    };
    be<u32> mSeeds[4];
    cXyz mEffectScale;
};
struct BarrierPiLimits {
    be<f32> mMin, mMax;
};
struct BarrierBckView {
    u8 pad0[0x10];
    be<u32> mMatrixCalcVtable;
    u8 mBinding[0x34];
    be<u32> mBindingVtable;
    u8 pad4C[0xC];
    be<u32> mAnimation;
    u8 pad5C[0x20];
    be<u32> mState, mFlags, mReserved, mLast;
};
WWHD_SIZE(BarrierCylinder, 0x130);
WWHD_SIZE(BarrierResTIMG, 0x24);
WWHD_SIZE(BarrierTextureScratch, 80);
WWHD_SIZE(BarrierBckView, 0x8C);
WWHD_OFFSET(BarrierCylinder, mCyl, 0x118);
WWHD_OFFSET(BarrierGameInfo, mEvent, 0x51D0);
WWHD_OFFSET(BarrierGameInfo, mEventManager, 0x52C4);
WWHD_OFFSET(BarrierGameInfo, mpParticle, 0x5AB0);
WWHD_OFFSET(BarrierGameInfo, mpPlayer, 0x5B2C);
WWHD_OFFSET(BarrierGameInfo, mSwordActive, 0x5BAC);
WWHD_OFFSET(BarrierGameInfo, mDrawList, 0x5D78);
WWHD_OFFSET(BarrierGameInfo, mInvisibleDrawList, 0x5D8C);
WWHD_OFFSET(BarrierSave, mEvent, 0x644);
WWHD_OFFSET(BarrierBckView, mBinding, 0x14);
WWHD_OFFSET(BarrierBckView, mBindingVtable, 0x48);
WWHD_OFFSET(BarrierBckView, mLast, 0x88);
