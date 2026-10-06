#pragma once
#include "f_op/f_op_actor.h"
#include "d/d_cc_d.h"

struct J3DModel;

// The HD compiler represents these action member pointers in eight bytes.
struct NhAction {
    be<s16> thisAdjustment;
    be<s16> virtualIndex;
    be<u32> target;
};
WWHD_SIZE(NhAction, 8);

struct daNh_c : fopAc_ac_c {
    enum { TYPE_BOTTLE = 1 };
    u8 _3AC[8];
    gptr<J3DModel> mpModel;                // 3B4
    u8 mAcch[0x1C4];                      // 3B8
    u8 mAcchCir[0x40];                    // 57C
    dCcD_Stts mStts;                      // 5BC
    dCcD_Cyl mCyl;                        // 5F8
    u8 mBrkAnm[0x84];                     // 728
    u8 mPolyInfo[0x10];                   // 7AC
    NhAction mCurrAction;                 // 7BC
    be<f32> mGlowMtx[3][4];                         // 7C4
    be<f32> mPlayerDist;                  // 7F4
    be<f32> mGroundY;                     // 7F8
    be<s32> mBottleTimer;                 // 7FC
    be<s32> mAlpha;                       // 800
    be<u8> unk804;
    be<s8> mActionStatus;
    be<u8> mGlowAlpha;
    be<u8> mWobbleDir;
    be<u8> mWobbleTimer;
    be<u8> mType;
    u8 _80A[2];
    be<s16> mEscapeTimer;
    be<s16> unk80E;
    be<s16> unk810;
    be<s16> unk812;
    u8 _814[4];
    be<f32> unk818;
    u8 _81C[4];
    cXyz mLightPos;                       // 820
    be<u16> mLightRed;
    be<u16> mLightGreen;
    be<u16> mLightBlue;
    u8 _832[2];
    be<f32> mLightBlend;                  // 834
    be<f32> mLightParameter;                 // 838
    be<s32> mLightId;                     // 83C native registered-light slot ID
    be<f32> mLightIntensity;              // 840

    BOOL init();
    BOOL execute();
    void BGCheck();
    void setBaseMtx();
    BOOL searchPlayer();
    BOOL checkBinCatch();
    BOOL waitAction(void*);
    BOOL checkEscapeEnd();
    BOOL escapeAction(void*);
    BOOL returnAction(void*);
    BOOL setAction(const NhAction*, void*);
    void action(void*);
    BOOL initBrkAnm(u32);
    BOOL createHeap();
    BOOL draw();
    BOOL checkTimer();
    void airMove();
    BOOL moveProc(f32, f32, u32);
    f32 getHomeDistance();
};
WWHD_SIZE(daNh_c, 0x844);
WWHD_OFFSET(daNh_c, mpModel, 0x3B4);
WWHD_OFFSET(daNh_c, mAcch, 0x3B8);
WWHD_OFFSET(daNh_c, mStts, 0x5BC);
WWHD_OFFSET(daNh_c, mCyl, 0x5F8);
WWHD_OFFSET(daNh_c, mBrkAnm, 0x728);
WWHD_OFFSET(daNh_c, mCurrAction, 0x7BC);
WWHD_OFFSET(daNh_c, mLightPos, 0x820);

// Parameters precede the HD Disposer subobject rather than a JOR vtable.
struct daNh_HIO_c {
    be<s8> mNo;
    u8 _01[3];
    be<f32> catchDistance, catchUpper, catchLower;
    be<f32> glowOffsetY, glowScale, minFrightenSpeed;
    be<f32> heightAboveGround, ascentSpeed, descentSpeed, gravity;
    be<f32> maxHomeDist, searchDistance, modelScale;
    be<s16> catchAngle, defaultGlowAlpha, bottleLifetime;
    u8 _3E[2];
    gptr<daNh_c> mpActor;
    be<u32> disposerVtable;
};
WWHD_SIZE(daNh_HIO_c, 0x48);

WWHD_OFFSET(daNh_c, mLightParameter, 0x838);
WWHD_OFFSET(daNh_c, mLightId, 0x83C);
