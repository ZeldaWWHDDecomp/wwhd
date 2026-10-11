// Big Octo eye. HD: actor base grows by 0x11C and animation controllers by 0x60.
// Legacy aliases keep the body actor source unchanged.
#pragma once
#include "bindings.h"
struct JntHit_c;
struct daDaiocta_Eye_c : fopAc_ac_c {
    enum Mode_e { MODE_WAIT, MODE_DAMAGE, MODE_DEATH };
    using ModeProcFunc = void (daDaiocta_Eye_c::*)();
    void _nodeControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    void _coHit(fopAc_ac_c*);
    void setMtx();
    void checkTgHit();
    void modeWaitInit();
    void modeWait();
    void modeDamageInit();
    void modeDamage();
    void modeDeathInit();
    void modeDeath();
    void modeProcCall();
    BOOL _execute(); // HD: preserve the full register return type.
    BOOL _draw();
    void createInit();
    s32 _create();
    BOOL _delete();
    /* 0x3AC */ union {
        be<s32> mEyeNo;
        be<s32> eyeNumber;
    };
    /* 0x3B0 */ union {
        be<u8> mbIsDead;
        be<u8> dead;
    };
    /* 0x3B1 */ be<u8> m295;
    /* 0x3B2 */ union {
        be<u8> mbIsDamaged;
        be<u8> damaged;
    };
    /* 0x3B3 */ union {
        be<u8> mbIsDamagedBomb;
        be<u8> damagedByBomb;
    };
    /* 0x3B4 */ union {
        be<u8> mbAppear;
        be<u8> appeared;
    };
    /* 0x3B5 */ u8 padding[3];
    /* 0x3B8 */ be<s32> mMode;
    /* 0x3BC */ request_of_phase_process_class mPhs;
    /* 0x3C4 */ gptr<J3DModel> mpModel;
    /* 0x3C8 */ gptr<J3DAnmTevRegKey> mpBrk;
    /* 0x3CC */ mDoExt_brkAnm mBrkAnm;
    /* 0x444 */ gptr<J3DAnmTextureSRTKey> mpBtk;
    /* 0x448 */ mDoExt_btkAnm mBtkAnm;
    /* 0x4BC */ be<s16> mEyeRotTargetX, mEyeRotTargetY, mEyeRotTargetZ;
    /* 0x4C2 */ csXyz mCurEyeRot;
    /* 0x4C8 */ be<s16> m2EC, m2EE, m2F0;
    /* 0x4CE */ be<s16> mEyeRndRotMaxX, mEyeRndRotMaxY, mEyeRndRotMaxZ;
    /* 0x4D4 */ dCcD_Sph mSph;
    /* 0x600 */ dCcD_Stts mStts;
    /* 0x63C */ cXyz mEyeScale;
    /* 0x648 */ be<s32> mScaleAnimeIdx;
    /* 0x64C */ gptr<JntHit_c> mpJntHit;
    /* 0x650 */ gptr<fopAc_ac_c> mpParentActor;
    /* 0x654 */ dPa_followEcallBack mParticleCallback;
    /* 0x668 */ cXyz mParticlePos;
    /* 0x674 */ csXyz m498;
    /* 0x67A */ u8 paddingTail[2];
};
WWHD_OFFSET(daDaiocta_Eye_c, mEyeNo, 0x3AC);
WWHD_OFFSET(daDaiocta_Eye_c, mMode, 0x3B8);
WWHD_OFFSET(daDaiocta_Eye_c, mpModel, 0x3C4);
WWHD_OFFSET(daDaiocta_Eye_c, mBrkAnm, 0x3CC);
WWHD_OFFSET(daDaiocta_Eye_c, mpBtk, 0x444);
WWHD_OFFSET(daDaiocta_Eye_c, mBtkAnm, 0x448);
WWHD_OFFSET(daDaiocta_Eye_c, mCurEyeRot, 0x4C2);
WWHD_OFFSET(daDaiocta_Eye_c, mSph, 0x4D4);
WWHD_OFFSET(daDaiocta_Eye_c, mStts, 0x600);
WWHD_OFFSET(daDaiocta_Eye_c, mEyeScale, 0x63C);
WWHD_OFFSET(daDaiocta_Eye_c, mpParentActor, 0x650);
WWHD_OFFSET(daDaiocta_Eye_c, mParticlePos, 0x668);
WWHD_SIZE(daDaiocta_Eye_c, 0x67C);

namespace octoEyeHD {
struct JointHitData {
    be<s16> mShapeType, mJointIndex;
    be<f32> mRadius;
    gptr<cXyz> mpOffsets;
};
WWHD_OFFSET(JointHitData, mpOffsets, 8);
WWHD_SIZE(JointHitData, 12);
struct ModeProc {
    gabi::ptmf<daDaiocta_Eye_c::ModeProcFunc> fn;
};
WWHD_SIZE(ModeProc, 8);
enum Resource {
    dRes_INDEX_DAIOCTA_BDL_DO_EYE1_e = 0x10,
    dRes_INDEX_DAIOCTA_BRK_DAMAGE_EYE_A1_e = 0x18,
    dRes_INDEX_DAIOCTA_BRK_DAMAGE_EYE_B1_e = 0x19,
    dRes_INDEX_DAIOCTA_BTK_DAMAGE_EYE_A1_e = 0x23,
    dRes_INDEX_DAIOCTA_BTK_DAMAGE_EYE_B1_e = 0x24,
    DO_EYE1_JNT_J_DO_EYE2_e = 2,
    fpcNm_BOMB_e = 0x126,
    fpcNm_DAIOCTA_e = 0xE1,
    JA_SE_CV_DO_DAMAGE = 0x48C8,
    JA_SE_CV_DO_EYE_LOSE = 0x48CB,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_LAST_HIT = 0x2828,
    ID_IT_SN_DO_SMOKE00 = 0x8206,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    AT_TYPE_ARROW_GROUP = 0xC4000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_BOOMERANG = 0x40,
    m_heapsize = 0xB20,
    m_scale_damage_time = 8,
};
struct ResourceLocal {
    be<u32> linkage[2];
    SafeString name;
};
struct ModelAnimationEntries {
    u8 padding[0x44];
    be<u32> mBtkEntry, mBrkEntry;
};
struct VectorWords {
    be<u32> x, y, z;
};
struct HeaderVector {
    be<u32> words[3];
};
struct HeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    u8 mAngleTag, mByteTag, padding[2];
    be<u32> mZero[4];
};
struct CallbackVtable {
    u8 padding[0x44];
    be<u32> mRemove;
};
struct CollisionVtables {
    be<u32> mStatus, mGlobalStatus;
};
struct Hio {
    be<u32> __vtbl;
    be<s16> m04;
    u8 padding[2];
    be<f32> m08, m0C;
    be<f32> mParticleTranslationX, mParticleTranslationY, mParticleTranslationZ;
    be<f32> mSphRadius;
    be<s16> mEyeRotStep, mEyeRotScale;
    be<s16> m24, m26, m28;
    be<s16> mEyeRndRotMaxX, mEyeRndRotMaxY, mEyeRndRotMaxZ;
    u8 m30[0x2C];
};
WWHD_SIZE(ResourceLocal, 0x10);
WWHD_OFFSET(ResourceLocal, name, 8);
WWHD_OFFSET(ModelAnimationEntries, mBtkEntry, 0x44);
WWHD_SIZE(ModelAnimationEntries, 0x4C);
WWHD_SIZE(VectorWords, 12);
WWHD_SIZE(HeaderVector, 12);
WWHD_SIZE(HeaderStatics, 0x1C);
WWHD_OFFSET(CallbackVtable, mRemove, 0x44);
WWHD_SIZE(CallbackVtable, 0x48);
WWHD_SIZE(CollisionVtables, 8);
WWHD_OFFSET(Hio, mSphRadius, 0x1C);
WWHD_OFFSET(Hio, m24, 0x24);
WWHD_OFFSET(Hio, m30, 0x30);
WWHD_SIZE(Hio, 0x5C);
} // namespace octoEyeHD
