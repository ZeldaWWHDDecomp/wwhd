/* daPz_c (Princess Zelda, Ganon's Tower), WWHD layout. 
 *
 * GameCube -> WWHD (size 0x1049 -> 0x16B8, constructor 0245226C):
 * - fopNpc_npc_c is 0x7DC (d/d_npc.h): members +0x118 up to mPhs (0x884);
 * - mDoExt_invisibleModel (0x88C) and the private dKy_tevstr_c (0x894, HD 0x1C8): +0x114 / +0x22C;
 * - enemyice (0xB54) and enemyfire (0xF0C, HD 0x22C): +0x22C; from 0x1138 on +0x230;
 * - mDoExt_brkAnm/btkAnm/btpAnm grew (HD 0x78/0x74/0x74) and HD adds a second btp (0x1320);
 * - HD-only tail: material tables (0x1394..0x1418), four 0x98-byte packets (0x1418..0x1678) and
 *   a 16-entry material index table (0x1678).
 * Field names follow GameCube where known; private HD views describe the verified extra data. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

#include "c/c_damagereaction.h"

/* daPz_matAnm_c : J3DMaterialAnm (HD 0x80, constructor 024514A4: vtable at +0x68) */
struct daPz_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ be<f32> mNowOffsetX;
    /* 0x70 */ be<f32> mNowOffsetY;
    /* 0x74 */ u8 _74[8];
    /* 0x7C */ be<u8> mMoveFlag;
    /* 0x7D */ u8 _7D[3];
};
WWHD_SIZE(daPz_matAnm_c, 0x80);

/* the 0x98-byte HD packets at 0x1418.. (constructor 027F1278, vtable at +0xC) */
struct daPz_packet_c {
    /* 0x00 */ u8 _00[0x0C];
    /* 0x0C */ be<u32> __vtbl;
    /* 0x10 */ u8 _10[0x98 - 0x10];
};
WWHD_SIZE(daPz_packet_c, 0x98);

// HD: emitter globals follow the expanded particle lists.
struct PzEmitterHD : JPABaseEmitter {
    u8 _070[0x1F0 - 0x70];
    be<f32> mGlobalRotation[9];
    u8 _214[0x220 - 0x214];
    cXyz mGlobalDynamicsScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    be<u8> mGlobalColor[4];
};
WWHD_OFFSET(PzEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(PzEmitterHD, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(PzEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(PzEmitterHD, mGlobalParticleScale, 0x238);
WWHD_OFFSET(PzEmitterHD, mGlobalColor, 0x244);
WWHD_SIZE(PzEmitterHD, 0x248);

struct PzPlayerHD : fopAc_ac_c {
    be<u8> mAction;
};
struct PzArrowHD : fopAc_ac_c {
    be<u8> mbInactive;
    u8 _3AD[0x780 - 0x3AD];
    be<u8> mbReflected;
};
struct PzGanondorfHD : fopAc_ac_c {
    u8 _3AC[0x3EA - 0x3AC];
    be<s16> mDemoState;
};
struct PzItemHD : fopAc_ac_c {
    u8 _3AC[0x783 - 0x3AC];
    be<u8> mColliderFlags;
};
WWHD_OFFSET(PzPlayerHD, mAction, 0x3AC);
WWHD_OFFSET(PzArrowHD, mbReflected, 0x780);
WWHD_OFFSET(PzGanondorfHD, mDemoState, 0x3EA);
WWHD_OFFSET(PzItemHD, mColliderFlags, 0x783);
WWHD_SIZE(PzPlayerHD, 0x3AD);
WWHD_SIZE(PzArrowHD, 0x781);
WWHD_SIZE(PzGanondorfHD, 0x3EC);
WWHD_SIZE(PzItemHD, 0x784);

// HD: the verifier supplies guest addresses for opaque native objects.
template <class T> inline T* pzGuest(u32 address) {
    return gabi::at<T>(address);
}

template <class T> struct PzRelativePtrHD {
    be<s32> mOffset;
    T* get() const {
        s32 offset = mOffset;
        return pzGuest<T>(offset != 0 ? gabi::ea(this) + offset : 0);
    }
};
struct PzTextureEntryHD {
    be<u16> mIndex, mMatrixOffset;
    be<s32> mAnimationIndex;
    u8 _008[4];
    be<u16> mLinkedIndex;
    u8 _00E[6];
};
struct PzTextureBlockHD {
    u8 _000[0x34];
    PzRelativePtrHD<PzTextureEntryHD> mEntries;
    PzRelativePtrHD<JUTNameTab> mNames;
};
struct PzTextureMatrixHD {
    u8 _000[0x10];
    be<f32> mOffsetX, mOffsetY;
};
struct PzMaterialHD {
    gptr<PzTextureBlockHD> mpTextureBlock;
    be<u16> mDirtyFlags;
    u8 _006[2];
    gptr<J3DShape> mpShape;
    gptr<be<u32>> mpDirtyMatrices;
    u8 _010[0x24 - 0x10];
    gptr<daPz_matAnm_c> mpMaterialAnm;
    gptr<u8> mpTextureMatrices;
};
WWHD_SIZE(PzRelativePtrHD<u8>, 4);
WWHD_OFFSET(PzTextureEntryHD, mLinkedIndex, 0xC);
WWHD_SIZE(PzTextureEntryHD, 0x14);
WWHD_OFFSET(PzTextureBlockHD, mNames, 0x38);
WWHD_SIZE(PzTextureBlockHD, 0x3C);
WWHD_OFFSET(PzTextureMatrixHD, mOffsetX, 0x10);
WWHD_SIZE(PzTextureMatrixHD, 0x18);
WWHD_OFFSET(PzMaterialHD, mpMaterialAnm, 0x24);
WWHD_OFFSET(PzMaterialHD, mpTextureMatrices, 0x28);
WWHD_SIZE(PzMaterialHD, 0x2C);

struct PzShapeHD : J3DShape {
    u8 _005[3];
    gptr<J3DShape> mpLinkedShape;
};
struct PzShapeHolderHD {
    u8 _000[4];
    gptr<gptr<PzShapeHD>> mpShapes;
};
struct PzJointTreeHD {
    u8 _000[0x10];
    gptr<PzShapeHolderHD> mpShapeHolder;
};
WWHD_OFFSET(PzShapeHD, mpLinkedShape, 8);
WWHD_SIZE(PzShapeHD, 0xC);
WWHD_OFFSET(PzShapeHolderHD, mpShapes, 4);
WWHD_SIZE(PzShapeHolderHD, 8);
WWHD_OFFSET(PzJointTreeHD, mpShapeHolder, 0x10);
WWHD_SIZE(PzJointTreeHD, 0x14);

struct PzLightParametersHD {
    be<f32> mVectors[6];
    be<u8> mColor[4];
    be<s16> mAttenuation[4];
    be<f32> mParameters[8];
};
struct PzTevParametersHD {
    PzLightParametersHD mLight;
    u8 _044[0xC0 - 0x44];
    PzLightParametersHD mCopy0;
    u8 _104[0x144 - 0x104];
    PzLightParametersHD mCopy1;
};
struct PzRippleHD {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
    u8 _008[8];
    be<f32> mRate;
};
WWHD_SIZE(PzLightParametersHD, 0x44);
WWHD_OFFSET(PzTevParametersHD, mCopy0, 0xC0);
WWHD_OFFSET(PzTevParametersHD, mCopy1, 0x144);
WWHD_SIZE(PzTevParametersHD, 0x188);
WWHD_OFFSET(PzRippleHD, mRate, 0x10);
WWHD_SIZE(PzRippleHD, 0x14);

struct PzBtpNameEntryHD {
    u8 _000[0xC];
    PzRelativePtrHD<char> mName;
    u8 _010[0xC];
};
struct PzBtpResourceHD {
    u8 _000[0x16];
    be<u16> mMaterialCount;
    u8 _018[0x2C - 0x18];
    PzRelativePtrHD<PzBtpNameEntryHD> mNames;
};
struct PzBtpAnmHD {
    J3DFrameCtrl mFrameCtrl;
    gptr<J3DAnmTexPattern> mpAnm;
    u8 _014[0x54 - 0x14];
    gptr<be<u32>> mpMaterialLinks;
    u8 _058[8];
    gptr<PzBtpResourceHD> mpMaterialResource;
    u8 _064[0x10];
};
struct PzBtkResourceHD {
    u8 _000[0x14];
    be<u16> mMaterialCount;
};
struct PzBtkStateHD {
    u8 _000[0xC];
    gptr<PzBtkResourceHD> mpResource;
};
struct PzBtkAnmHD {
    u8 _000[0x68];
    gptr<PzBtkStateHD> mpState;
};
struct PzMaterialNamesHD {
    u8 _000[0x18];
    PzRelativePtrHD<JUTNameTab> mNames;
    u8 _01C[8];
    be<u16> mMaterialCount;
};
WWHD_SIZE(PzBtpNameEntryHD, 0x1C);
WWHD_OFFSET(PzBtpResourceHD, mNames, 0x2C);
WWHD_SIZE(PzBtpResourceHD, 0x30);
WWHD_OFFSET(PzBtpAnmHD, mpMaterialLinks, 0x54);
WWHD_OFFSET(PzBtpAnmHD, mpMaterialResource, 0x60);
WWHD_SIZE(PzBtpAnmHD, 0x74);
WWHD_SIZE(PzBtkResourceHD, 0x16);
WWHD_SIZE(PzBtkStateHD, 0x10);
WWHD_SIZE(PzBtkAnmHD, 0x6C);
WWHD_OFFSET(PzMaterialNamesHD, mMaterialCount, 0x24);
WWHD_SIZE(PzMaterialNamesHD, 0x26);

struct PzAnimationVTableHD {
    u8 _000[0x14];
    be<u32> mGetFrameMax;
};
struct PzAnimationResourceHD {
    u8 _000[4];
    gptr<PzAnimationVTableHD> mpVTable;
};
struct PzPlayDrawHD {
    u8 _000[0x5D30];
    u8 mDrawList[0xC];
    cXyz mPosition;
    u8 _5D48[0x5D60 - 0x5D48];
    gptr<J3DDrawBuffer> mBuffers[4];
};
struct PzRendererHD {
    gptr<J3DDrawBuffer> mpOpa, mpXlu;
};
struct PzModelDataTailHD : J3DModelData {
    u8 _014[0x38 - 0x14];
    be<u32> mBtkAnm;
    u8 _03C[8];
    be<u32> mBtpAnm, mBrkAnm;
};
struct PzDemoSingletonHD {
    be<u32> mObject;
};
struct PzMessageAttributeHD {
    u8 _000[0x5BC5];
    be<u8> mAttribute;
};
enum PzDrawBuffer_e { PZ_DRAW_XLU, PZ_DRAW_FACE, PZ_DRAW_OPA, PZ_DRAW_SPECIAL };
WWHD_SIZE(PzAnimationVTableHD, 0x18);
WWHD_SIZE(PzAnimationResourceHD, 8);
WWHD_OFFSET(PzPlayDrawHD, mPosition, 0x5D3C);
WWHD_OFFSET(PzPlayDrawHD, mBuffers, 0x5D60);
WWHD_SIZE(PzPlayDrawHD, 0x5D70);
WWHD_SIZE(PzRendererHD, 8);
WWHD_OFFSET(PzModelDataTailHD, mBtkAnm, 0x38);
WWHD_OFFSET(PzModelDataTailHD, mBtpAnm, 0x44);
WWHD_SIZE(PzModelDataTailHD, 0x4C);
WWHD_SIZE(PzDemoSingletonHD, 4);
WWHD_SIZE(PzMessageAttributeHD, 0x5BC6);
extern PzRendererHD& l_pzRenderer;

struct PzSafeStringVTableHD {
    u8 _000[0x14];
    be<u32> mAssureTermination;
};
struct PzStartStageHD {
    u8 _000[0x5134];
    char mName[8];
};
WWHD_SIZE(PzSafeStringVTableHD, 0x18);
WWHD_OFFSET(PzStartStageHD, mName, 0x5134);
WWHD_SIZE(PzStartStageHD, 0x513C);
// HD: native byte walks read guest address zero too; a host null pointer cannot model that.
inline u8 pzStringByte(u32 address) {
    return gabi::load<u8>(address);
}
inline void pzAssureTermination(SafeString* string) {
    gabi::call_ptr(pzGuest<PzSafeStringVTableHD>(string->__vtbl)->mAssureTermination, string);
}

enum PzProcess_e { fpcNm_GND_e = 0xF6, fpcNm_ARROW_e = 0x1D8 };
enum PzSound_e {
    JA_SE_CV_ZL_DRAW_BOW = 0x495E,
    JA_SE_CV_ZL_SHOOT_BOW = 0x495F,
    JA_SE_CV_ZL_GN_DAMAGE = 0x4960
};
enum PzMessage_e {
    PZ_MSG_FIRST = 0x3562,
    PZ_MSG_ADVICE = 0x3563,
    PZ_MSG_ADVICE_CONTINUE = 0x3564,
    PZ_MSG_REPEAT = 0x3565
};
enum PzResource_e {
    PZ_BCK_BOW_READY = 7,
    PZ_BCK_BOW_SHOOT = 8,
    PZ_BCK_BOW_HOLD = 9,
    PZ_BCK_HIT = 10,
    PZ_BCK_DOWN = 11,
    PZ_BCK_DEFEND = 12,
    PZ_BCK_DRAW_BOW = 13,
    PZ_BCK_RUN = 14,
    PZ_BCK_SHOOT_BOW = 15,
    PZ_BCK_WAKE = 16,
    PZ_BCK_WAIT = 17,
    PZ_BCK_READY = 18,
    PZ_BCK_HOLD_BOW = 19,
    PZ_BCK_AFRAID = 20,
    PZ_BDL_BOW = 23,
    PZ_BDL_BODY = 24,
    PZ_BRK_BODY = 27,
    PZ_BTK_EYE_NORMAL = 31,
    PZ_BTP_EYE_NORMAL = 39
};
enum PzJoint_e {
    PZ_JNT_ROOT_e = 0,
    PZ_JNT_WAIST_e = 1,
    PZ_JNT_HEAD_e = 4,
    PZ_JNT_HAND_R1_e = 14,
    PZ_JNT_WAIST2_e = 21,
    PZ_JNT_SKIRT_e = 25
};
enum PzAttackType_e : u32 {
    AT_TYPE_SWORD = 0x2,
    AT_TYPE_MACHETE = 0x400,
    AT_TYPE_UNK800 = 0x800,
    AT_TYPE_DARKNUT_SWORD = 0x4000000,
    AT_TYPE_MOBLIN_SPEAR = 0x10000000,
    AT_TYPE_BOMB = 0x20,
    AT_TYPE_BOOMERANG = 0x40,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_WIND = 0x200000,
    AT_TYPE_GRAPPLING_HOOK = 0x8000000,
};

enum PzParticle_e {
    PZ_PARTICLE_HIT = 0xC,
    PZ_PARTICLE_SPLASH = 0x23,
    PZ_PARTICLE_RIPPLE = 0x33,
    PZ_PARTICLE_FALL_SPLASH = 0x3F,
    PZ_PARTICLE_HEAD_SPLASH = 0x837D
};
struct PzHeaderStaticsHD {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mMathInitialized, mRandomInitialized;
    u8 _00A[2];
    be<u32> mZero[4];
};
struct PzRegistrationHD {
    be<u32> mWords[3];
};
WWHD_SIZE(PzHeaderStaticsHD, 0x1C);
WWHD_SIZE(PzRegistrationHD, 0xC);

struct daPz_c : fopNpc_npc_c {
    enum Mode_e {
        MODE_WAIT,
        MODE_MOVE,
        MODE_ATTACK_WAIT,
        MODE_ATTACK,
        MODE_DEFEND,
        MODE_DOWN,
        MODE_AFRAID,
        MODE_SIDE_STEP,
        MODE_BACK_STEP,
        MODE_TALK,
        MODE_FOLLOW,
        MODE_KEEP
    };
    enum Anm_e {
        ANM_NONE,
        ANM_WAIT,
        ANM_READY,
        ANM_RUN,
        ANM_BOW_READY,
        ANM_BOW_HOLD,
        ANM_BOW_SHOOT,
        ANM_HIT,
        ANM_DOWN,
        ANM_WAKE,
        ANM_DEFEND,
        ANM_AFRAID,
        ANM_TALK_A,
        ANM_TALK_B,
        ANM_TALK_C,
        ANM_KEEP
    };
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };

    void _nodeHeadControl(J3DNode*, J3DModel*);
    void _nodeWaistControl(J3DNode*, J3DModel*);
    void _nodeWaist2Control(J3DNode*, J3DModel*);
    void _nodeSkirtControl(J3DNode*, J3DModel*);
    BOOL bodyCreateHeap();
    BOOL bowCreateHeap();
    BOOL _createHeap();
    void getGndPos();
    BOOL checkEyeArea(cXyz*);
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    void anmAtr(u32); /* u16, compared as a full register */
    void eventOrder();
    void checkOrder();
    void setFallSplash();
    void setHeadSplash();
    void setRipple();
    void setJntStatus();
    BOOL demo();
    BOOL checkTgHit();
    void getArg();
    void setAttention();
    void setBowAnm(s8, s32);   /* HD: the bool argument is passed on as a full register */
    void setBowString(s32);    /* bool, tested as a full register */
    void setAnm(s8, s32, s32); /* (s8 anm, bool morf, int) */
    void setAnmRunSpeed();
    void setEyeBtp(int);
    void setEyeBtk(int);
    void setEyeAnm(s8);
    void ctrlEye();
    void playEyeAnm();
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeMoveInit();
    void modeMove();
    void modeAttackWaitInit();
    void modeAttackWait();
    void modeAttackInit();
    void modeAttack();
    void modeDefendInit();
    void modeDefend();
    void modeDownInit();
    void modeDown();
    void modeAfraidInit();
    void modeAfraid();
    void modeSideStepInit();
    void modeSideStep();
    void modeBackStepInit();
    void modeBackStep();
    void modeTalkInit();
    void modeTalk();
    void modeFollowInit();
    void modeFollow();
    void modeProc(s32, s32);
    BOOL _execute();
    void bowDraw();
    void bodyDraw();
    BOOL _draw();
    void bodyCreateInit();
    void createInit();
    cPhs_State _create();
    BOOL _delete();

    /* 0x7DC */ be<s32> mMode; /* GameCube 0x6C4 */
    /* 0x7E0 */ be<s32> mPreviousMode;
    /* 0x7E4 */ be<s32> mMessageAnmStarted; /* anmAtr: message animation started */
    /* 0x7E8 */ be<u8> mTgHitTimer;
    /* 0x7E9 */ be<u8> mHitType;
    /* 0x7EA */ be<s8> mBckIdx; /* pzDLib_bcks_setAnm: bck */
    /* 0x7EB */ be<s8> m06D3;   /* GameCube m06D3: anm prm number */
    /* 0x7EC */ be<s8> mOldAnm;
    /* 0x7ED */ be<s8> mCurEye; /* GameCube 0x6D5 */
    /* 0x7EE */ u8 _7EE[2];
    /* 0x7F0 */ be<s32> mBlinkTimer;
    /* 0x7F4 */ be<u8> mbTargetInEyeRange;
    /* 0x7F5 */ be<s8> mBowBckIdx;
    /* 0x7F6 */ be<s8> mBowAnm;
    /* 0x7F7 */ be<s8> mBowOldAnm;
    /* 0x7F8 */ PzRippleHD mRipple; /* dPa_rippleEcallBack */
    /* 0x80C */ dPa_followEcallBack mFollowCB0;
    /* 0x820 */ dPa_followEcallBack mFollowCB1;
    /* 0x834 */ cXyz mFallSplashPos;
    /* 0x840 */ be<s32> mFallSplashTimer; /* fall splash timer */
    /* 0x844 */ cXyz mHeadPos;            /* head joint translation */
    /* 0x850 */ u8 _850[0x856 - 0x850];
    /* 0x856 */ be<u8> mbEyesFollowGanondorf;
    /* 0x857 */ be<u8> m073F;
    /* 0x858 */ be<u8> mbWakeUp;
    /* 0x859 */ u8 _859[3];
    /* 0x85C */ be<s32> mbLastHitFromPlayer; /* last hit was not Ganondorf */
    /* 0x860 */ cXyz mHitPos;
    /* 0x86C */ be<s32> mPlayerHitCount; /* hits by others in a row */
    /* 0x870 */ be<s32> mPlayerTargetAttackCount;
    /* 0x874 */ be<u8> mbTargetPlayer;
    /* 0x875 */ u8 _875[3];
    /* 0x878 */ be<s32> mGanondorfHitCount; /* hits by Ganondorf in a row */
    /* 0x87C */ be<s32> mGanondorfTargetAttackCount;
    /* 0x880 */ be<u8> mbTargetGanondorf;
    /* 0x881 */ u8 _881[3];
    /* 0x884 */ request_of_phase_process_class mPhs; /* GameCube 0x76C */
    /* 0x88C */ u8 mInvisibleModel[8];               /* mDoExt_invisibleModel, GameCube 0x778 */
    /* 0x894 */ dKy_tevstr_c mTevstr;                /* GameCube 0x780 */
    /* 0xA5C */ cXyz mFollowPos;
    /* 0xA68 */ dBgS_LinChk mLinChk;
    /* 0xAD4 */ gptr<daPz_matAnm_c> mpMatAnm[2]; /* eyes L/R, GameCube m08A8 */
    /* 0xADC */ be<s32> m08B0;
    /* 0xAE0 */ be<s32> mLightArrowID;
    /* 0xAE4 */ cXyz mEyePos; /* head joint * (0,0,0) */
    /* 0xAF0 */ cXyz m08C4;
    /* 0xAFC */ cXyz mEyeTargetPos;
    /* 0xB08 */ cXyz mEyePos2; /* head joint * (24,-16,0) */
    /* 0xB14 */ be<s16> mLookTurnSpeed;
    /* 0xB16 */ be<u8> mbHeadOnly;
    /* 0xB17 */ u8 _B17;
    /* 0xB18 */ be<s32> mModeTimer;
    /* 0xB1C */ be<s32> mAttackWaitTimer;
    /* 0xB20 */ be<s32> mDodgeTimer;
    /* 0xB24 */ be<s32> mWallTurnTimer;
    /* 0xB28 */ cXyz mCircleCenter;        /* dLib_circle_path_c: centre */
    /* 0xB34 */ cXyz mCirclePos;           /*   position on the circle */
    /* 0xB40 */ be<f32> mCircleRadius;     /*   radius */
    /* 0xB44 */ be<f32> mCircleSpeed;      /*   walk speed */
    /* 0xB48 */ be<s16> mCircleAngle;      /*   angle */
    /* 0xB4A */ be<s16> mCircleAngleSpeed; /*   angle speed */
    /* 0xB4C */ be<s32> m0920;
    /* 0xB50 */ be<f32> mTargetSpeed;
    /* 0xB54 */ enemyice mEventIce;   /* GameCube 0x928 */
    /* 0xF0C */ enemyfire mEnemyFire; /* GameCube 0xCE0 */
    /* 0x1138 */ Mtx34 mWaistMtx;
    /* 0x1168 */ cXyz mWaistPos;
    /* 0x1174 */ be<f32> mAttackAimSpread;
    /* 0x1178 */ be<u8> m0F48;
    /* 0x1179 */ u8 _1179[3];
    /* 0x117C */ cXyz mGanondorfPosCurrent; /* GameCube 0xF4C */
    /* 0x1188 */ cXyz mGanondorfPos4;       /* Ganondorf eyePos, GameCube 0xF58 */
    /* 0x1194 */ be<u8> mbHasGanondorf;     /* GameCube 0xF64 */
    /* 0x1195 */ be<u8> m0F65;
    /* 0x1196 */ be<u8> mbDemoActive;
    /* 0x1197 */ be<u8> mEyeFrame;
    /* 0x1198 */ be<f32> mHitSlideSpeed;
    /* 0x119C */ be<s16> mHitAngle;
    /* 0x119E */ u8 _119E[2];
    /* 0x11A0 */ be<f32> mStepSpeed;
    /* 0x11A4 */ u8 _11A4[4];
    /* 0x11A8 */ be<s16> mWaistAngleZ;
    /* 0x11AA */ be<s16> mWaistAngleY;
    /* 0x11AC */ be<s32> m0F7C;
    /* 0x11B0 */ be<u8> m0F80;
    /* 0x11B1 */ be<u8> mbInPlayerFan;
    /* 0x11B2 */ be<u8> m0F82; /* 1: speak event, 2: other event */
    /* 0x11B3 */ u8 _11B3;
    /* 0x11B4 */ be<s32> mArg; /* GameCube 0xF84 */
    /* 0x11B8 */ be<u8> m0F88;
    /* 0x11B9 */ u8 _11B9[3];
    /* 0x11BC */ gptr<mDoExt_McaMorf> mpBowMcaMorf; /* GameCube 0xF8C */
    /* 0x11C0 */ mDoExt_brkAnm mBrkAnm;             /* mDoExt_brkAnm (HD 0x78), GameCube 0xF90 */
    /* 0x1238 */ mDoExt_btkAnm mBtkAnm;             /* mDoExt_btkAnm (HD 0x74), GameCube 0xFA8 */
    /* 0x12AC */ mDoExt_btpAnm mBtpAnm;             /* mDoExt_btpAnm (HD 0x74), GameCube 0xFBC */
    /* 0x1320 */ mDoExt_btpAnm mBtpAnm2;            /* HD: a second mDoExt_btpAnm */
    /* 0x1394 */ be<u32> mpJointNodes;              /* model data +8 */
    /* 0x1398 */ be<u32> mMat0[6];
    /* 0x13B0 */ be<u32> mMat1[6];
    /* 0x13C8 */ be<u32> mMat2[4];
    /* 0x13D8 */ be<u32> mMatSh0[6];
    /* 0x13F0 */ be<u32> mMatSh1[6];
    /* 0x1408 */ be<u32> mMatSh2[4];
    /* 0x1418 */ daPz_packet_c mPacket[4];
    /* 0x1678 */ be<u32> mMatIdx[16]; /* sead::SafeArray: low halfword = material index */
};
WWHD_OFFSET(daPz_c, mPhs, 0x884);
WWHD_OFFSET(daPz_c, mTevstr, 0x894);
WWHD_OFFSET(daPz_c, mLinChk, 0xA68);
WWHD_OFFSET(daPz_c, mEventIce, 0xB54);
WWHD_OFFSET(daPz_c, mEnemyFire, 0xF0C);
WWHD_OFFSET(daPz_c, mArg, 0x11B4);
WWHD_OFFSET(daPz_c, mBtpAnm2, 0x1320);
WWHD_OFFSET(daPz_c, mPacket, 0x1418);
WWHD_SIZE(daPz_c, 0x16B8);

// HD: per-translation-unit virtual table identities used during construction and destruction.
enum PzVTable_e {
    PZ_VT_ACTOR = 0x10038D60,
    PZ_VT_SAFE_STRING = 0x10038558,
    PZ_VT_MATERIAL_ANM = 0x10038D48,
    PZ_VT_HIO = 0x10038650,
    PZ_VT_ICE_CYLINDER = 0x10038570,
    PZ_VT_OBJ_ACCH = 0x100385A0,
    PZ_VT_ACCH_GROUP = 0x100385B0,
    PZ_VT_ACCH_POLYGON = 0x100385C0,
    PZ_VT_OBJ_LINE = 0x10038610,
    PZ_VT_OBJ_LINE_INFO = 0x10038620,
    PZ_VT_OBJ_LINE_POLYGON = 0x10038640,
    PZ_VT_OBJ_LINE_GROUP = 0x10038630,
    PZ_VT_LINE_POLYGON = 0x10038600,
    PZ_VT_LINE_GROUP = 0x10038590,
    PZ_VT_LINE_INFO = 0x10038580,
    PZ_VT_FACE_PACKET = 0x10058D10,
    PZ_VT_SPECIAL_PACKET = 0x10058D40
};

/* ======== constants and local bindings used by the d_a_pz*.cpp part files ======== */
#define SAFESTRING_VTBL PZ_VT_SAFE_STRING /* this TU's sead::SafeString vtable */
#define PZ_VTBL PZ_VT_ACTOR               /* daPz_c vtable (constructor 0245226C) */
#define l_arcName "Pz"                    /* "Pz" */
extern dCcD_SrcCyl& m_cyl_src;

// HD: tunable Zelda parameters; constructor writes retain their original order.
struct daPz_HIO_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<f32> mPlayerEyeYOffset;
    /* 0x008 */ be<s16> mHeadMaxX;
    /* 0x00A */ be<s16> mHeadMinX;
    /* 0x00C */ be<s16> mBackboneMaxX;
    /* 0x00E */ be<s16> mBackboneMinX;
    /* 0x010 */ be<s16> mHeadMaxY;
    /* 0x012 */ be<s16> mHeadMinY;
    /* 0x014 */ be<s16> mBackboneMaxY;
    /* 0x016 */ be<s16> mBackboneMinY;
    /* 0x018 */ be<s16> mMaxJointStep;
    /* 0x01A */ be<s16> mTurnSpeed;
    /* 0x01C */ be<f32> mJointRadius;
    /* 0x020 */ be<s16> mJointAngle;
    /* 0x022 */ be<u8> mJointDebugFlag;
    u8 _pad023[1];
    /* 0x024 */ be<f32> mJointDistance;
    u8 _pad028[4];
    /* 0x02C */ be<u8> mDebugFlag;
    /* 0x02D */ be<u8> mbEnableAttack;
    u8 _pad02E[1];
    /* 0x02F */ be<u8> mDebugAnm;
    /* 0x030 */ be<u8> mbLookAtPlayer;
    /* 0x031 */ be<u8> mDebugFlag2;
    /* 0x032 */ be<u8> mbDebugSideStep;
    /* 0x033 */ be<u8> mbDebugWarpToPlayer;
    /* 0x034 */ be<u8> mDebugButtons[10];
    u8 _pad03E[2];
    /* 0x040 */ be<f32> mAttentionYOffset;
    /* 0x044 */ be<f32> mRunRate;
    /* 0x048 */ be<f32> mRunMaxRate;
    /* 0x04C */ be<f32> mRunMinRate;
    /* 0x050 */ be<f32> mMoveSpeed;
    /* 0x054 */ be<s16> mEyeAreaAngle;
    u8 _pad056[2];
    /* 0x058 */ be<f32> mEyeAreaRadius;
    /* 0x05C */ be<f32> mAimSpread[3];
    /* 0x068 */ be<f32> mPlayerAimChance[3];
    /* 0x074 */ be<f32> mStraightAimChance[3];
    /* 0x080 */ be<s16> mAttackWaitTime[3];
    /* 0x086 */ be<s16> mMoveWaitTime[3];
    /* 0x08C */ be<s16> mMoveWaitRandom[3];
    /* 0x092 */ be<s16> mAttackChargeTime[3];
    /* 0x098 */ be<s16> mAttackChargeRandom[3];
    u8 _pad09E[2];
    /* 0x0A0 */ be<f32> mCircleRadius[3];
    /* 0x0AC */ be<f32> mCircleSpeed;
    /* 0x0B0 */ be<f32> mGravity;
    /* 0x0B4 */ be<f32> mBackStepJumpSpeed;
    /* 0x0B8 */ be<f32> mBackStepSpeed;
    /* 0x0BC */ be<f32> mSideStepJumpSpeed;
    /* 0x0C0 */ be<f32> mSideStepSpeed;
    /* 0x0C4 */ be<f32> mDownJumpSpeed;
    /* 0x0C8 */ be<f32> mDownSlideSpeed;
    /* 0x0CC */ be<s16> mDownSlideTime;
    u8 _pad0CE[2];
    /* 0x0D0 */ be<f32> mDodgeDistance;
    /* 0x0D4 */ be<f32> mSplashYOffset;
    /* 0x0D8 */ be<f32> mReverseDirectionChance;
    /* 0x0DC */ be<s16> mDebugTimer;
    /* 0x0DE */ be<s16> mDebugTimer2;
    /* 0x0E0 */ be<s16> mHitCooldown;
    /* 0x0E2 */ be<s16> mGanondorfHitLimit;
    /* 0x0E4 */ be<s16> mGanondorfTargetAttacks;
    /* 0x0E6 */ be<s16> mPlayerHitLimit;
    /* 0x0E8 */ be<s16> mPlayerTargetAttacks;
    u8 _pad0EA[2];
    /* 0x0EC */ be<f32> mFollowDistance;
    /* 0x0F0 */ be<f32> mDownMorfTime;
    /* 0x0F4 */ be<f32> mDownWaitMorfTime;
    /* 0x0F8 */ be<s16> mTalkWaitTime[3];
    u8 _pad0FE[2];
    /* 0x100 */ be<f32> mTalkDistance[3];
};
WWHD_SIZE(daPz_HIO_c, 0x10C);
extern daPz_HIO_c& l_HIO;

/* debug registers g_regHIO child 12 (REG12_S(i): 0x1047B688 + 12*0x90 + 2i) */
static inline s16 REG12_S(int i) {
    return REG_S(12, i);
}

/* Private HD adapters used by this actor. */
// HD: native joint conversion has already run before this direct joint-number read.
struct PzJointDataHD {
    be<u32> mType;
    be<u16> mJntNo;
};
WWHD_OFFSET(PzJointDataHD, mJntNo, 4);
WWHD_SIZE(PzJointDataHD, 6);
using PzModelPointerHD = gptr<J3DModel>;
extern PzModelPointerHD& l_pzCurrentModel;
extern Mtx34& l_pzCurrentMtx;
static inline Mtx34* getAnmMtx(J3DModel* model, s32 joint) {
    return model->getAnmMtx(joint);
}
static inline J3DModel* j3dSys_getModel() {
    return l_pzCurrentModel;
}
static inline Mtx34* J3DSys_mCurrentMtx() {
    return &l_pzCurrentMtx;
}
static inline u16 J3DJoint_getJntNo(J3DJoint* joint) {
    return reinterpret_cast<PzJointDataHD*>(joint)->mJntNo;
}

/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    pzAssureTermination(a);
    pzAssureTermination(a);
    u32 s1 = a->mStringTop;
    pzAssureTermination(b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = pzStringByte(p + i);
        if (c != pzStringByte(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name
 * at play+0x5134) */
static inline bool pzIsStartStage(const char* lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_str(lit);
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = gabi::ea(pzGuest<PzStartStageHD>(dComIfGp_ea())->mName);
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}

/* 02587A0C pzDLib_bcks_setAnm(arc, morf, s8* bck, s8* prm, s8* old, const int* bcks, const
 * dLib_anm_prm_c*, bool, HD int) */

/* eventInfo (dEvt_info_c at actor+0xF8): mCommand +0xF8, mCondition +0xFA, mEventId +0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) {
    return a->eventInfo.mCommand;
}
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) {
    a->eventInfo.mEventId = id;
}

// HD: native entry points preserve each verified register signature.
BOOL pzFopAcM_SearchByName(s16 name, be<u32>* out);
s32 pzDBgS_GetAttributeCode(dBgS* bgs, void* poly);
s32 pzCLib_calcTimer(be<s32>* t);
f64 pzSqrtf_d(f32 x);
void pzLookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel,
                    u8 headOnly);
void pzJPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t);
void pzDLib_setCirclePath(void* path);
s32 pzCLib_calcTimer_u8(be<u8>* t);
void pzDef_se_set(fopAc_ac_c* a, void* obj, u32 mtrl);
void pzDKy_SordFlush_set(cXyz* pos, s32 p);
fopAc_ac_c* pzFopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* rot, cXyz* scale,
                                    f32 speedF, f32 speedY, f32 gravity, s32 bitNo, u32 createFunc);
void pzLineSet(dBgS_LinChk* chk, cXyz* start, cXyz* end, fopAc_ac_c* actor);
fpc_ProcID pzFopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo,
                                csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc);
BOOL pzFopAcM_SearchByID(fpc_ProcID id, be<u32>* out);
s16 pzCLib_targetAngleX(const cXyz* a, const cXyz* b);
void pzCLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep);
f64 pzSearchActorDistanceXZ_d(fopAc_ac_c* a, fopAc_ac_c* b);
s32 pzJUTNameTab_getIndex(u32 tab, u32 name);
void pzDLib_bcks_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm,
                        be<s8>* old, u32 bcks, u32 prms, s32 force, s32 hd);
void pzDKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p);

extern cXyz& l_eyeOffset;
extern cXyz& l_eyeOffset2;
extern cXyz& l_rippleScale;
extern cXyz& l_landScale;
extern cXyz& l_downFootScale;
extern cXyz& l_downWaistScale;
extern cXyz& l_downHeadScale;
extern be<u32>& l_eyeOffsetInitialized;
extern be<u32>& l_eyeOffset2Initialized;
extern be<s16>& l_waistDebugAngle;
extern be<u32>& l_materialNamesInitialized;
extern be<u32>& l_rippleScaleInitialized;
extern be<u32>& l_landScaleInitialized;
extern be<u32>& l_downFootScaleInitialized;
extern be<u32>& l_downWaistScaleInitialized;
extern be<u32>& l_downHeadScaleInitialized;

enum { dEvtCnd_CANTALK_e = 1 };
void pzMonsSeStart(u32 id, cXyz* pos, fpc_ProcID actorID, u32 param, s32 reverb);
u32 pzSetSimpleLand(dPa_control_c* particle, u32 poly, cXyz* pos, csXyz* angle, dKy_tevstr_c* tev,
                    f32 scaleX, f32 scaleY, f32 scaleZ, be<s32>* id, s32 type);
const char* pzGetNowCutName(dEvent_manager_c* manager, s32 staffID);
u32 pzGetEventIndex(dEvent_manager_c* manager, const char* name, s32 archive);
u32 pzGetEventData(u32 manager, u32 index);
u8 pzCheckActorInFan(cXyz* pos, fopAc_ac_c* actor, s16 angle, s32 fanAngle, f32 radius, f32 height);
void pzDestroyArray(u32 array, u32 count, u32 size, u32 destructor, u32 flags, u32 unused);
void pzSphereConstruct(dCcD_Sph* sphere);
void pzPacketDestruct(daPz_packet_c* packet, s32 flags);
void pzSphereDestruct(dCcD_Sph* sphere, s32 flags);
void pzAcchDestruct(u32 acch, s32 flags);
void pzCircleDestruct(u32 circle, s32 flags);
void pzLineDestruct(u32 line, s32 flags);
void pzInvisibleDestruct(void* model, s32 flags);
void pzActorDestruct(fopAc_ac_c* actor, s32 flags);
void pzSetDrawPosition(u32 drawList);
void pzIceEntryDL(mDoExt_McaMorf* morf, s32 material, void* invisibleModel);
s32 pzFindNameIndex(u32 table, const char* name);
BOOL pzBtpInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate,
               s32 start, s32 end, s32 modify, s32 entry);
BOOL pzBtkInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate,
               s32 start, s32 end, s32 modify, s32 entry);
BOOL pzBrkInit(void* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate,
               s32 start, s32 end, s32 modify, s32 entry);
u32 pzGetMaterialTable(J3DModelData* model);
BOOL pzInvisibleCreate(void* invisibleModel, J3DModel* model);
u32 pzGetMaterialNameTable(J3DModelData* model);
u32 pzDemoGetActor(u32 object, u8 id);
void pzSetDemoData(fopAc_ac_c* actor, s32 flags, mDoExt_McaMorf* morf, const char* archive, s32 a,
                   s32 b, s32 c, s32 d);
void* pzDemoGetBtp(u32 actor, const char* archive);
void* pzDemoGetBtk(u32 actor, const char* archive);
void* pzDemoGetBrk(u32 actor, const char* archive);
void pzEnemyFire(enemyfire* fire);
BOOL pzEnemyIce(enemyice* ice);
void pzMorfPlay(mDoExt_McaMorf* morf, cXyz* pos, u32 sound, s32 reverb);
void pzEntryImmediate(u32 drawBuffer, u32 packet, s32 index);
void pzUpdateMaterialDisplayList(J3DModel* model, u32 joints);
void pzBtpEntry(void* animation, J3DModel* model, f32 frame);
void pzNpcConstruct(fopNpc_npc_c* actor);
void pzRippleConstruct(void* callback);
void pzInvisibleConstruct(void* model);
void pzBrkConstruct(void* animation);
void pzBtpConstruct(void* animation);
void pzPacketConstruct(daPz_packet_c* packet);
void pzJointHIOConstruct(void* parameters);

daPz_c* daPz_c_ct(daPz_c* actor);
enemyfire* enemyfire_ct(enemyfire* fire);
daPz_matAnm_c* daPz_matAnm_c_ct(daPz_matAnm_c* animation);
u8* daPz_HIO_c_ct(u8* hio);

using PzModeFunc = void (daPz_c::*)();
struct PzModeEntryHD {
    gabi::ptmf<PzModeFunc> mInit;
    gabi::ptmf<PzModeFunc> mExecute;
    gptr<char> mName;
};
struct PzEyeAnmEntryHD {
    be<s32> mBtp, mBtk;
};
WWHD_SIZE(PzModeEntryHD, 0x14);
WWHD_OFFSET(PzModeEntryHD, mExecute, 8);
WWHD_OFFSET(PzModeEntryHD, mName, 0x10);
WWHD_SIZE(PzEyeAnmEntryHD, 8);

struct PzEyeMaterialNamesHD {
    SafeString mNames[2];
};
struct PzMaterialNamesStaticHD {
    SafeString mNames[16];
};
WWHD_SIZE(PzEyeMaterialNamesHD, 16);
WWHD_SIZE(PzMaterialNamesStaticHD, 128);
extern PzEyeMaterialNamesHD& l_eyeMaterialNames;

struct PzAnmParameterHD {
    be<s8> mAnmIndex, mNextIndex;
    be<s16> mFlags;
    be<f32> mMorf, mPlaySpeed;
    be<s32> mLoopMode;
};
struct PzAnmParametersHD {
    PzAnmParameterHD mEntries[15];
};
WWHD_SIZE(PzAnmParameterHD, 0x10);
WWHD_OFFSET(PzAnmParameterHD, mMorf, 4);
WWHD_SIZE(PzAnmParametersHD, 0xF0);

extern PzMaterialNamesStaticHD& material_names;
BOOL stealItem_CB(void* actor);
BOOL createHeap_CB(fopAc_ac_c* actor);
void SafeString_dt(SafeString* string, s32 flags);
BOOL nodeHeadControl_CB(J3DNode* node, int timing);
BOOL nodeWaistControl_CB(J3DNode* node, int timing);
BOOL nodeWaist2Control_CB(J3DNode* node, int timing);
BOOL nodeSkirtControl_CB(J3DNode* node, int timing);
