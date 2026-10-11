/* daRd_c (ReDead), WWHD layout. 
 *
 * GameCube -> WWHD (size 0xD44 -> 0xF1C, constructor 0245AA10):
 * - +0x11C up to mpMorf (fopEn_enemy_c);
 * - mDoExt_btkAnm (HD 0x74) and mDoExt_brkAnm (HD 0x78) grew by 0x60 each: +0x1DC from mSpawnPos;
 * - mShadowId and the padding before it (GameCube 0x68C..0x694) are gone (HD has no blob shadow):
 *   +0x1D4 from mInvisModel;
 * - enemyfire grew by 4 (LIGHT_INFLUENCE 0x24): +0x1D8 from mpJntHit.
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "d/d_lib.h"
#include "d/d_jnt_hit.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/tu.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, as in d_a_cc.h) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> m228;     /* HD: 1.0f in the constructor */
};
WWHD_SIZE(enemyfire_l, 0x22C);
#endif

/* mDoExt_brkAnm, HD 0x78: its J3DFrameCtrl first (frame +4) (SHARED-CANDIDATE, as d_a_npc_ji1.h) */
struct mDoExt_brkAnm_rd {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x78 - 0x10];
};
WWHD_SIZE(mDoExt_brkAnm_rd, 0x78);



struct daRd_c : fopEn_enemy_c {
    typedef void (daRd_c::*ModeFunc)();
    struct ModeEntry {
        gabi::ptmf<ModeFunc> init;
        gabi::ptmf<ModeFunc> run;
        gptr<const char> name;
    };
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode {
        MODE_WAIT = 0x0, MODE_DAMAGE = 0x1, MODE_PARALYSIS = 0x2, MODE_DEATH = 0x3, MODE_MOVE = 0x4,
        MODE_CRY = 0x5, MODE_CRY_WAIT = 0x6, MODE_ATTACK = 0x7, MODE_RETURN = 0x8, MODE_SILENT_PRAY = 0x9,
        MODE_SW_WAIT = 0xA, MODE_KANOKE = 0xB, MODE_NULL,
    };
    enum BckIdx {
        BckIdx_TACHIP     = 0x0,
        BckIdx_SUWARIP    = 0x1,
        BckIdx_WALK2ATACK = 0x2,
        BckIdx_ATACK      = 0x3,
        BckIdx_ATACK2WALK = 0x4,
        BckIdx_WALK       = 0x5,
        BckIdx_DAMAGE     = 0x6,
        BckIdx_DEAD       = 0x7,
        BckIdx_TATSU      = 0x8,
        BckIdx_SUWARU     = 0x9,
        BckIdx_KANOKEP    = 0xA,
        BckIdx_BEAM_HIT   = 0xB,
        BckIdx_BEAM       = 0xC,
        BckIdx_BEAM_END   = 0xD,
    };
    enum AnmPrm {
        AnmPrm_TACHIP0 = 0x0, AnmPrm_TACHIP1 = 0x1, AnmPrm_SUWARIP = 0x2, AnmPrm_WALK2ATACK = 0x3,
        AnmPrm_ATACK = 0x4, AnmPrm_ATACK2WALK = 0x5, AnmPrm_WALK = 0x6, AnmPrm_DAMAGE = 0x7,
        AnmPrm_DEAD = 0x8, AnmPrm_TATSU = 0x9, AnmPrm_SUWARU = 0xA, AnmPrm_KANOKEP = 0xB,
        AnmPrm_BEAM_HIT = 0xC, AnmPrm_BEAM = 0xD, AnmPrm_BEAM_END = 0xE, AnmPrm_NULL,
    };

    /* 0x3C8 */ be<s32> mMode;
    /* 0x3CC */ u8 m2B0[4];
    /* 0x3D0 */ be<s32> mWhichIdleAnm;
    /* 0x3D4 */ be<s32> mAreaRadius;
    /* 0x3D8 */ be<s32> mChecksSwitch;
    /* 0x3DC */ be<s32> mSwNo;
    /* 0x3E0 */ be<u8> m2C4;
    /* 0x3E1 */ be<u8> mHitType;
    /* 0x3E2 */ u8 m2C6[2];
    /* 0x3E4 */ request_of_phase_process_class mPhs;
    /* 0x3EC */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3F0 */ mDoExt_btkAnm mBtkAnm;
    /* 0x464 */ mDoExt_brkAnm_rd mBrkAnm;
    /* 0x4DC */ cXyz mSpawnPos;
    /* 0x4E8 */ be<s16> mSpawnAngle;
    /* 0x4EA */ u8 m30E[2];
    /* 0x4EC */ be<s32> mTimer1;
    /* 0x4F0 */ be<s32> mTimer2;
    /* 0x4F4 */ be<s32> mBreakFreeCounter;
    /* 0x4F8 */ dBgS_ObjAcch mAcch;
    /* 0x6BC */ dBgS_AcchCir mAcchCir;
    /* 0x6FC */ dCcD_Stts mStts;
    /* 0x738 */ dCcD_Cyl mCyl;
    /* HD: no mShadowId */
    /* 0x868 */ u8 mInvisModel[8];        /* mDoExt_invisibleModel */
    /* 0x870 */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x8A4 */ be<u32> mCorpseID;
    /* 0x8A8 */ be<s32> m6D4;
    /* 0x8AC */ be<s8> mBckIdx;
    /* 0x8AD */ be<s8> mAnmPrmIdx;
    /* 0x8AE */ be<s8> mOldAnmPrmIdx;
    /* 0x8AF */ be<s8> m6DB;
    /* 0x8B0 */ be<s8> m6DC;
    /* 0x8B1 */ u8 m6DD[3];
    /* 0x8B4 */ enemyice_l mEnemyIce;
    /* 0xC6C */ enemyfire_l mEnemyFire;
    /* 0xE98 */ gptr<JntHit_c> mpJntHit;
    /* 0xE9C */ cXyz mTargetPos;
    /* 0xEA8 */ cXyz mRdEyePos;
    /* 0xEB4 */ be<s16> mMaxHeadTurnVel;
    /* 0xEB6 */ u8 mCDE[2];
    /* 0xEB8 */ be<s32> mCE0;
    /* 0xEBC */ be<s32> mCE4;
    /* 0xEC0 */ Mtx34 mCE8;
    /* 0xEF0 */ be<s16> mHeadAngle;
    /* 0xEF2 */ be<s16> mD1A;
    /* 0xEF4 */ be<s16> mD1C;
    /* 0xEF6 */ be<s16> mD1E;
    /* 0xEF8 */ u8 mD20[0xF06 - 0xEF8];
    /* 0xF06 */ be<s16> mD2E;
    /* 0xF08 */ u8 mD30[4];
    /* 0xF0C */ be<u8> mbIkari;
    /* 0xF0D */ u8 mD35[3];
    /* 0xF10 */ be<f32> mD38;
    /* 0xF14 */ be<s32> mD3C;
    /* 0xF18 */ be<s32> mD40;

    bool isAnm(s8 idx) { return mAnmPrmIdx == idx; }
    void onIkari() { mbIkari = 1; }
    void offIkari() { mbIkari = 0; }
    bool isIkari() { return mbIkari != 0; }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }

    fopAc_ac_c* _searchNearDeadRd(fopAc_ac_c*);
    void _nodeControl(J3DNode*, J3DModel*);
    void _nodeHeadControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    bool createArrowHeap();
    bool checkPlayerInAttack();
    bool checkPlayerInCry();
    void lookBack();
    bool checkTgHit();
    void setCollision();
    void setIceCollision();
    void setAttention();
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeDeathInit();
    void modeDeath();
    void modeDamageInit();
    void modeDamage();
    void modeParalysisInit();
    void modeParalysis();
    void modeMoveInit();
    void modeMove();
    void modeCryInit();
    void modeCry();
    void modeCryWaitInit();
    void modeCryWait();
    void modeAttackInit();
    void modeAttack();
    void modeReturnInit();
    void modeReturn();
    void modeSilentPrayInit();
    void modeSilentPray();
    void modeSwWaitInit();
    void modeSwWait();
    void modeKanokeInit();
    void modeKanoke();
    void modeProc(Proc_e, int);
    void setBrkAnm(s8);
    void setBtkAnm(s8);
    void setAnm(s8, bool);
    bool _execute();
    void debugDraw();
    bool _draw();
    bool isLinkControl();
    void createInit();
    void getArg();
    cPhs_State _create();
    bool _delete();
};
WWHD_OFFSET(daRd_c, mMode, 0x3C8);
WWHD_OFFSET(daRd_c, mPhs, 0x3E4);
WWHD_OFFSET(daRd_c, mBrkAnm, 0x464);
WWHD_OFFSET(daRd_c, mSpawnPos, 0x4DC);
WWHD_OFFSET(daRd_c, mAcch, 0x4F8);
WWHD_OFFSET(daRd_c, mCyl, 0x738);
WWHD_OFFSET(daRd_c, mJntCtrl, 0x870);
WWHD_OFFSET(daRd_c, mEnemyIce, 0x8B4);
WWHD_OFFSET(daRd_c, mEnemyFire, 0xC6C);
WWHD_OFFSET(daRd_c, mpJntHit, 0xE98);
WWHD_OFFSET(daRd_c, mCE8, 0xEC0);
WWHD_OFFSET(daRd_c, mD2E, 0xF06);
WWHD_OFFSET(daRd_c, mD38, 0xF10);
WWHD_SIZE(daRd_c, 0xF1C);

// HD: dNpc_HIO_c and JntHit_HIO_c keep their vtables at the ends of the subobjects.
struct daRd_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<u8> m2C;
    /* 0x2D */ u8 padding_2D[3];
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<f32> m34;
    /* 0x38 */ be<f32> mCryRadius;
    /* 0x3C */ be<f32> mAttackRadius;
    /* 0x40 */ be<s16> m40;
    /* 0x42 */ be<s16> mCrySpreadAngle;
    /* 0x44 */ be<s16> mAttackSpreadAngle;
    /* 0x46 */ be<s16> m46;
    /* 0x48 */ be<s16> m48;
    /* 0x4A */ be<s16> m4A;
    /* 0x4C */ be<s16> m4C;
    /* 0x4E */ be<s16> m4E;
    /* 0x50 */ be<s16> m50;
    /* 0x52 */ be<s16> m52;
    /* 0x54 */ be<s16> m54;
    /* 0x56 */ u8 padding_56[2];
    /* 0x58 */ be<f32> m58;
    /* 0x5C */ be<f32> m5C;
    /* 0x60 */ be<f32> m60;
    /* 0x64 */ be<f32> m64;
    /* 0x68 */ be<f32> m68;
    /* 0x6C */ be<f32> mReturnWalkSpeed;
    /* 0x70 */ be<f32> m70;
    /* 0x74 */ be<f32> m74;
    /* 0x78 */ be<s16> m78;
    /* 0x7A */ be<s16> mParalysisDuration;
    /* 0x7C */ u8 m7C[0x2C]; // JntHit_HIO_c
};
WWHD_SIZE(daRd_HIO_c, 0xA8);
WWHD_OFFSET(daRd_HIO_c, mNpc, 4);
WWHD_OFFSET(daRd_HIO_c, mParalysisDuration, 0x7A);

enum dRes_INDEX_RD {
    /* BCKS */
    dRes_INDEX_RD_BCK_ATACK_e=0x6,
    dRes_INDEX_RD_BCK_ATACK2WALK_e=0x7,
    dRes_INDEX_RD_BCK_BEAM_e=0x8,
    dRes_INDEX_RD_BCK_BEAM_END_e=0x9,
    dRes_INDEX_RD_BCK_BEAM_HIT_e=0xA,
    dRes_INDEX_RD_BCK_DAMAGE_e=0xB,
    dRes_INDEX_RD_BCK_DEAD_e=0xC,
    dRes_INDEX_RD_BCK_IKARI_SAMPLE_e=0xD,
    dRes_INDEX_RD_BCK_KANOKEP_e=0xE,
    dRes_INDEX_RD_BCK_SUWARIP_e=0xF,
    dRes_INDEX_RD_BCK_SUWARU_e=0x10,
    dRes_INDEX_RD_BCK_TACHIP_e=0x11,
    dRes_INDEX_RD_BCK_TATSU_e=0x12,
    dRes_INDEX_RD_BCK_WALK_e=0x13,
    dRes_INDEX_RD_BCK_WALK2ATACK_e=0x14,
    /* BDLM */
    dRes_INDEX_RD_BDL_RD_e=0x17,
    /* BRK  */
    dRes_INDEX_RD_BRK_BEAM_e=0x1A,
    dRes_INDEX_RD_BRK_BEAM_END_e=0x1B,
    dRes_INDEX_RD_BRK_BEAM_HIT_e=0x1C,
    dRes_INDEX_RD_BRK_NML_e=0x1D,
    /* BTK  */
    dRes_INDEX_RD_BTK_RD_CLOSE_e=0x20,
    dRes_INDEX_RD_BTK_RD_IKARI_e=0x21,
    dRes_INDEX_RD_BTK_RD_NML_e=0x22,
    dRes_INDEX_RD_BTK_RD_OPEN_e=0x23,
};

enum RD_JNT {
    RD_JNT_REE_SKLROOT_e=0x0,
    RD_JNT_REE_KOSI_1_e=0x1,
    RD_JNT_REE_ASI_L1_e=0x2,
    RD_JNT_REE_ASI_L2_e=0x3,
    RD_JNT_REE_ASI_L3_e=0x4,
    RD_JNT_REE_ASI_R1_e=0x5,
    RD_JNT_REE_ASI_R2_e=0x6,
    RD_JNT_REE_ASI_R3_e=0x7,
    RD_JNT_REE_MUNE_1_e=0x8,
    RD_JNT_REE_MUNE_2_e=0x9,
    RD_JNT_REE_KUBI_1_e=0xA,
    RD_JNT_REE_KUBI_2_e=0xB,
    RD_JNT_REE_ATAMA_1_e=0xC,
    RD_JNT_REE_EARRING_L1_e=0xD,
    RD_JNT_REE_EARRING_R1_e=0xE,
    RD_JNT_REE_UDE_L1_e=0xF,
    RD_JNT_REE_UDE_L2_e=0x10,
    RD_JNT_REE_TEKUBI_L1_e=0x11,
    RD_JNT_REE_YUBI_L1_e=0x12,
    RD_JNT_REE_UDE_R1_e=0x13,
    RD_JNT_REE_UDE_R2_e=0x14,
    RD_JNT_REE_TEKUBI_R1_e=0x15,
    RD_JNT_REE_YUBI_R1_e=0x16,
};

WWHD_SIZE(daRd_c::ModeEntry, 0x14);
struct rd_anm_prm_struct { be<s8> m00, m01; u8 _padding[2]; be<s32> loopMode; };
WWHD_SIZE(rd_anm_prm_struct, 8);

// HD: guest pointers embedded in the original joint-hit table.
namespace RdJointOffsets {
enum : u32 {
    sebone_cyl_offset = 0x101CF528,
    muneA_cyl_offset = 0x101CF540,
    muneB1_cyl_offset = 0x101CF558,
    muneB2_cyl_offset = 0x101CF570,
    muneB3_cyl_offset = 0x101CF588,
    kosi1_cyl_offset = 0x101CF498,
    kosi2_cyl_offset = 0x101CF4B0,
    kosi3_cyl_offset = 0x101CF4C8,
    asi1_cyl_offset = 0x101CF4E0,
    asi2_cyl_offset = 0x101CF4F8,
    asi3_cyl_offset = 0x101CF510,
    ude1_cyl_offset = 0x101CF5A0,
    ude2_cyl_offset = 0x101CF5B8,
    te_cyl_offset = 0x101CF5D0,
    yubi_cyl_offset = 0x101CF5E8,
};
}

// HD: the player fields reached by this actor beyond the shared player base.
struct RdLinkState : daPy_py_c {
    /* 0x043C */ u8 _043C[0x699E - 0x43C];
    /* 0x699E */ be<s16> mNoDamageTimer;
    /* 0x69A0 */ u8 _69A0[0x69E8 - 0x69A0];
    /* 0x69E8 */ be<u8> mStrongHitTimerActive;
};
WWHD_OFFSET(RdLinkState, mNoDamageTimer, 0x699E);
WWHD_OFFSET(RdLinkState, mStrongHitTimerActive, 0x69E8);
WWHD_SIZE(RdLinkState, 0x69E9);
// HD: animation pointers cleared after drawing, following the common model-data prefix.
struct RdModelAnimations {
    /* 0x00 */ u8 _00[0x44];
    /* 0x44 */ gptr<J3DAnmTextureSRTKey> mpAnmTexMtx;
    /* 0x48 */ gptr<J3DAnmTevRegKey> mpAnmTevReg;
};
WWHD_OFFSET(RdModelAnimations, mpAnmTexMtx, 0x44);
WWHD_OFFSET(RdModelAnimations, mpAnmTevReg, 0x48);
WWHD_SIZE(RdModelAnimations, 0x4C);
struct RdJointData {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_OFFSET(RdJointData, mJntNo, 4);
WWHD_SIZE(RdJointData, 6);

// HD: named ABI bindings used by the compiler-generated functions in this unit.
namespace RdHD {
inline void constructSphere(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
inline void constructBrk(mDoExt_brkAnm_rd* brk) { gabi::call(0x025E80D0, brk); }
inline void constructInvisibleModel(void* model) { gabi::call(0x025E895C, model); }
inline void constructJointControl(dNpc_JntCtrl_c* ctrl) { gabi::call(0x0259DAA0, ctrl); }
inline void constructNpcHIO(dNpc_HIO_c* hio) { gabi::call(0x0259DA18, hio); }
inline void constructJntHitHIO(void* hio) { gabi::call(0x02552BE8, hio); }
inline void destroyAcch(dBgS_ObjAcch* acch) { gabi::call(0x024EFD9C, acch, 0); }
inline void destroySphere(dCcD_Sph* sphere) { gabi::call(0x02515AE8, sphere, 2); }
inline void destroyCircle(cM3dGCir_l* circle) { gabi::call(0x02018034, circle, 2); }
inline void destroyInvisibleModel(void* model) { gabi::call(0x025E89F8, model, 2); }
inline void destroyActor(daRd_c* actor) { gabi::call(0x025D50BC, actor, 0); }
}

// Unit-local bindings retain the verified HD calling conventions and explicit guest copies.
static inline daRd_HIO_c& l_HIO() { return *gabi::at<daRd_HIO_c>(0x1046D6A0); }
/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, mDoMtx_stack_c::now 0x1048D0CC;
 * joint matrices in the model's block at +0x2C (+0x4 flags, +0x10 matrices), user area +0xB8 */
static inline J3DModel* j3dSys_model() { return *gabi::at<gptr<J3DModel>>(0x104B462C); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* mtx_now() { return gabi::at<Mtx34>(0x1048D0CC); }
static inline Mtx34* rd_getAnmMtx(J3DModel* model, u32 jntNo) {
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    u32 matrices = buffer->mpAnmMtx;
    buffer->mFlags |= 0x10;
    return gabi::at<Mtx34>(matrices + jntNo * sizeof(Mtx34));
}
static inline J3DModelData* rd_modelData(J3DModel* model) { return model->getModelData(); }
static inline J3DJoint* rd_jointNode(J3DModelData* modelData, u32 joint) {
    return modelData->getJointNodePointer(joint);
}
static inline void mDoMtx_inverseTranspose(Mtx34* src, Mtx34* dst) { gabi::call(0x025F20B0, src, dst); }
static inline void mDoMtx_MtxToRot(Mtx34* m, csXyz* out) { gabi::call(0x025F232C, m, out); }
// 02552B60 JntHit_create(J3DModel*, __jnt_hit_data_c*, s16)
static inline JntHit_c* JntHit_create(J3DModel* m, u32 data, s16 num) { return gabi::call<JntHit_c*>(0x02552B60, m, data, num); }
// 025E8A48 mDoExt_invisibleModel::create(J3DModel*)
static inline bool invisibleModel_create(void* inv, J3DModel* m) { return gabi::call<bool>(0x025E8A48, inv, m); }
// 0259138C dMat_ice_c::entryDL (matcher; mDoExt_invisibleModel::entryDL per phpt) (morf, s32, invisibleModel*)
static inline void dMat_control_iceEntryDL(mDoExt_McaMorf* m, s32 p, void* inv) { gabi::call(0x0259138C, m, p, inv); }
// 025E8154 mDoExt_brkAnm::init(data, key, anmPlay, attr, rate, start, end, modify, entry(stack))
static inline BOOL brkAnm_init(mDoExt_brkAnm_rd* a, J3DModelData* d, void* key, bool play, s32 mode, f32 rate, s16 start, s16 end,
                               bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, mode, rate, start, end, modify, entry);
}
// 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32 frame)
static inline void brkAnm_entry(mDoExt_brkAnm_rd* a, J3DModelData* d, f32 f) { gabi::call(0x025E83FC, a, d, f); }
// 02587A0C dLib_bcks_setAnm(arc, morf, s8* bck, s8* prm, s8* old, const int* bcks, const dLib_anm_prm_c*, bool, HD int)
static inline void dLib_bcks_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks,
                                    u32 prms, bool force) {
    gabi::call(0x02587A0C, arc, morf, bck, prm, old, bcks, prms, force, 0);
}
// 02588230 dLib_checkActorInFan(cXyz center (copy), fopAc_ac_c*, s16 angle, s16 spread, f32 radius, f32 height)
static inline bool dLib_checkActorInFan(cXyz* c, fopAc_ac_c* a, s16 ang, s16 spread, f32 r, f32 h) {
    return gabi::call<bool>(0x02588230, c, a, ang, spread, r, h);
}
// 025880B4 dLib_checkActorInCircle(cXyz center (copy), fopAc_ac_c*, f32 radius, f32 height)
static inline bool dLib_checkActorInCircle(cXyz* c, fopAc_ac_c* a, f32 r, f32 h) { return gabi::call<bool>(0x025880B4, c, a, r, h); }
static inline bool checkInFan(const cXyz& center, fopAc_ac_c* a, s16 ang, s16 spread, f32 r, f32 h) {
    gabi::Local<cXyz> c;
    c->copy(center);
    return dLib_checkActorInFan(c, a, ang, spread, r, h);
}
static inline bool checkInCircle(const cXyz& center, fopAc_ac_c* a, f32 r, f32 h) {
    gabi::Local<cXyz> c;
    c->copy(center);
    return dLib_checkActorInCircle(c, a, r, h);
}
// 02587684 dLib_debugDrawFan(cXyz& pos, s16, s16, f32, const GXColor&), 02587514 dLib_debugDrawAxis(Mtx, f32)
static inline void dLib_debugDrawFan(cXyz* p, s16 a, s16 s, f32 r, u32 color) { gabi::call(0x02587684, p, a, s, r, color); }
static inline void dLib_debugDrawAxis(Mtx34* m, f32 s) { gabi::call(0x02587514, m, s); }
// 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly)
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, const cXyz& eye, s16 yrot, s16 vel, bool headOnly) {
    gabi::Local<cXyz> e;
    e->copy(eye);
    gabi::call(0x0259DED0, j, outY, target, e.get(), yrot, vel, headOnly);
}
/* cLib_calcTimer<int> 0211D2F8, cLib_calcTimer<u8> 0207A9A0 (out-of-line copies) */
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline u8 cLib_calcTimer(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
// 0200F268 cLib_addCalcPosXZ2(cXyz*, const cXyz&, f32 scale, f32 maxStep)
static inline void cLib_addCalcPosXZ2(cXyz* p, const cXyz* t, f32 s, f32 m) { gabi::call(0x0200F268, p, t, s, m); }
// 024EFF3C dBgS_AcchCir::SetWallR
static inline void AcchCir_SetWallR(dBgS_AcchCir* c, f32 r) { gabi::call(0x024EFF3C, c, r); }
// 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out)
static inline bool fopAcM_SearchByID_out(u32 id, gptr<fopAc_ac_c>* out) { return gabi::call<bool>(0x025D54C4, id, out); }
// 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind)
static inline void fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) { gabi::call(0x025D77DC, a, name, flag, hind); }
// 025DA088 fopAcM_setGbaName(actor, item, gbaName0, gbaName1)
static inline void fopAcM_setGbaName(fopAc_ac_c* a, u8 item, u8 n0, u8 n1) { gabi::call(0x025DA088, a, item, n0, n1); }
// 025445B8 dEvent_manager_c::startCheckOld(name) (play+0x52C4)
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* dComIfGp_event_runCheck: the event control's run flag (play+0x5292) */
/* event reset through an already fetched play pointer (HD hoists dComIfGp_get) */
static inline void event_reset_at(u32 play) { gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag |= 8; }
static inline fopAc_ac_c* player0_at(u32 play) { return gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer; }
// 0252A038 dDetect_c::chk_light(cXyz*) (play+0x5A20)
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, dComIfGp_ea() + PLAY_DETECT, pos); }
// 0200E814 cDT_NamePTbl::GetIndex (play+0x50AC)
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, name, p);
}
/* CPad (port 0): main stick X/Y (f32), A/B/X/Y trigger (bool) */
static inline f32 CPad_GET_STICK_POS_X(u32 port) { return gabi::call<f32>(0x0200796C, port); }
static inline f32 CPad_GET_STICK_POS_Y(u32 port) { return gabi::call<f32>(0x02007990, port); }
static inline BOOL CPad_CHECK_TRIG_A(u32 port) { return gabi::call<BOOL>(0x02007898, port); }
static inline BOOL CPad_CHECK_TRIG_B(u32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline BOOL CPad_CHECK_TRIG_X(u32 port) { return gabi::call<BOOL>(0x020078E8, port); }
static inline BOOL CPad_CHECK_TRIG_Y(u32 port) { return gabi::call<BOOL>(0x02007914, port); }
/* c_damagereaction */
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* f) { gabi::call(0x02041570, f); }
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* d_cc_uty: CcAtInfo (0x1C), cc_at_check */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
/* audio (HD inlines with `this` known non-null: no checks) */
static inline void rd_seStart(daRd_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}
// 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb)
static inline void rd_monsSeStart(daRd_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1AA4, id, &a->eyePos, (u32)a->mBsPcId, param, reverb);
}
/* daPy_py_c (HD layout): head top position +0x3D8, cut type +0x3AC; virtuals at the HD vtable (+0xB4) */
static inline cXyz* daPy_getHeadTopPos(fopAc_ac_c* p) { return &static_cast<daPy_py_c*>(p)->mHeadTopPos; }
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return *reinterpret_cast<be<u8>*>(&static_cast<daPy_py_c*>(p)->mCutType); }
static inline f32 daPy_getBaseAnimeFrame(fopAc_ac_c* p) { return gabi::call_ptr<f32>(gabi::load<u32>(p->__vtbl + 0xA4), p); }
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 a) { gabi::call_ptr(gabi::load<u32>(p->__vtbl + 0x114), p, pos, a); }
/* daPy_lk_c::checkNoDamageMode (inline): +0x3BC bit 0, or the HD s16 at +0x699E */
static inline bool daPy_checkNoDamageMode(fopAc_ac_c* p) {
    return ((u32)static_cast<daPy_py_c*>(p)->mNoResetFlg1 & 1) != 0 || (s16)static_cast<RdLinkState*>(p)->mNoDamageTimer != 0;
}
static inline void daPy_lk_setDamagePoint(fopAc_ac_c* p, f32 d) { gabi::call(0x023F51D0, p, d); }
// 023F4CB0 (unnamed, HD): the no-damage-mode counterpart of setDamagePoint (plays SE 0x2886)
static inline void daPy_lk_noDamagePoint(fopAc_ac_c* p, f32 d) { gabi::call(0x023F4CB0, p, d); }
/* daPy_py_c::changeOriginalDemo (inline): demo parameter 0 +0x428 = 0, demo type +0x420 = 3 */
static inline void daPy_changeOriginalDemo(fopAc_ac_c* p) {
    static_cast<daPy_py_c*>(p)->mDemo.mParam0 = 0;
    static_cast<daPy_py_c*>(p)->mDemo.mDemoType = 3;
}
/* save: dComIfGs_getLife (u16 at info+2) */

/* attention_info (fopAc_ac_c +0x388): position +0x08, flags +0x14 */
static inline cXyz* attention_pos(fopAc_ac_c* a) { return &a->attention_info.position; }
static inline be<u32>* attention_flags(fopAc_ac_c* a) { return &a->attention_info.flags; }


static inline void rd_setAnmMtx(J3DModel* model, u32 jntNo) {
    mtx_copy(rd_getAnmMtx(model, jntNo), mtx_now());
}
static inline void createInit_tail(daRd_c* i_this) {
    i_this->setBrkAnm(0);
    i_this->setMtx();
    i_this->mBtkAnm.play();
    mDoExt_baseAnm_play(&i_this->mBrkAnm);
    i_this->mpMorf->play(&i_this->current.pos, 0, 0);
    i_this->mBrkAnm.mFrameCtrl.setFrame(0.0f);
    i_this->mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
}
static inline u32 rd_static_color(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::call(0xC000A848, dst, src, 4); /* memcpy (import, through the thunk 028FEAC0) */
    }
    return dst;
}
static inline bool rd_noticePlayer(daRd_c* i_this, fopAc_ac_c* player, BOOL isOto) {
    if (checkInCircle(i_this->mSpawnPos, i_this, (f32)i_this->mAreaRadius, 1000.0f)) {
        /* Bug (GameCube too): the 200-unit-tall cylinder is around the spawn point */
        if (checkInCircle(i_this->mSpawnPos, player, (f32)i_this->mAreaRadius, 100.0f)) {
            if ((checkInCircle(i_this->mSpawnPos, player, (f32)i_this->mAreaRadius, 100.0f) &&
                 player->speedF > REG_F(12, 4) + 10.0f) ||
                isOto ||
                checkInFan(i_this->current.pos, player, i_this->shape_angle.y, l_HIO().m40, l_HIO().m34, 100.0f)) {
                return true;
            }
        }
    }
    return false;
}
static inline bool morf_isStop(mDoExt_McaMorf* m) { return m->isStop(); }
static inline void rd_walkToPlayer(daRd_c* i_this, f32 dist) {
    f32 temp = (dist / 100.0f) + REG_F(12, 3);
    if (temp > 1.0f) {
        temp = 1.0f;
    }
    if (temp < 0.1f) {
        temp = 0.1f;
    }
    f32 temp2 = l_HIO().m68;
    if (i_this->mEnemyFire.mMode != 0) {
        temp2 = temp2 + temp2;
    }
    cLib_addCalc2(&i_this->speedF, temp2 * temp, 0.1f, REG_F(12, 0) + 0.1f);
}
static inline void rd_breakFree(daRd_c* i_this, f32 stickPosX, f32 stickPosY) {
    if (i_this->mD3C > 0 && stickPosX < 0.0f) {
        i_this->mD3C = -1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    } else if (i_this->mD3C < 0 && stickPosX > 0.0f) {
        i_this->mD3C = 1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }

    if (i_this->mD40 > 0 && stickPosY < 0.0f) {
        i_this->mD40 = -1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    } else if (i_this->mD40 < 0 && stickPosY > 0.0f) {
        i_this->mD40 = 1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }

    if (CPad_CHECK_TRIG_A(0) || CPad_CHECK_TRIG_B(0) || CPad_CHECK_TRIG_X(0) || CPad_CHECK_TRIG_Y(0)) {
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }
}
static inline void rd_objacch_dt(dBgS_ObjAcch* a) {
    a->dBgS_GrpPassChk::__vtbl = 0x10038EC4;
    a->dBgS_PolyPassChk::__vtbl = 0x10038ED4;
    RdHD::destroyAcch(a);
}

// HD: the original opaque collision initializer remains shared guest data.
static inline dCcD_SrcCyl* rd_cylinderSource() { return gabi::at<dCcD_SrcCyl>(0x10039380); }

namespace RdParticle { enum { ID_IT_JN_PIYOHIT00 = 0x27B, ID_AK_JN_CRITICALHITFLASH = 0x10, ID_AK_JN_CRITICALHIT = 0xF, ID_AK_JN_OK = 0xD }; }

namespace RdCutType { enum {
    CUT_TYPE_BT_JUMPCUT = 0x05,
    CUT_TYPE_CUT_EA = 0x06,
    CUT_TYPE_CUT_EB = 0x07,
    CUT_TYPE_CUT_TURN = 0x08,
    CUT_TYPE_CUT_ROLL = 0x09,
    CUT_TYPE_JUMPCUT_SWORD = 0x0A,
    CUT_TYPE_JUMPCUT_STICK = 0x0C,
    CUT_TYPE_JUMPCUT_MACHETE = 0x0E,
    CUT_TYPE_BT_ROLLCUT = 0x0F,
    CUT_TYPE_BT_VERTICALJUMPCUT = 0x10,
    CUT_TYPE_HAMMER_SIDESWING = 0x11,
    CUT_TYPE_JUMPCUT_CLUB = 0x15,
    CUT_TYPE_JUMPCUT_DN_SWORD = 0x17,
    CUT_TYPE_JUMPCUT_SPEAR = 0x19,
    CUT_TYPE_CUT_EXA = 0x1A,
    CUT_TYPE_CUT_EXB = 0x1B,
}; }

// GameCube names for this actor's sound, process, item and attack constants.
enum {
    JA_SE_LK_SW_HIT_S = 0x2803, JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_HS_SPIKE = 0x286F, JA_SE_LK_ARROW_HIT = 0x2879, JA_SE_CV_RD_SCREAM = 0x48D3,
    JA_SE_CV_RD_DAMAGE = 0x48D4, JA_SE_CV_RD_DIE = 0x48D5, JA_SE_CM_PW_BECOME_SOLID = 0x5135,
};
enum { fpcNm_RD_e = 0xE0, DSNAP_TYPE_RD = 0xB9, dItemNo_MIRROR_SHIELD_e = 0x3C };
enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_FIRE = 0x200, AT_TYPE_MACHETE = 0x400, AT_TYPE_UNK800 = 0x800, AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_HOOKSHOT = 0x8000, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_UNK20000 = 0x20000,
    AT_TYPE_FIRE_ARROW = 0x40000, AT_TYPE_ICE_ARROW = 0x80000, AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_WIND = 0x200000, AT_TYPE_LIGHT = 0x800000, AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_DARKNUT_SWORD = 0x4000000, AT_TYPE_GRAPPLING_HOOK = 0x8000000, AT_TYPE_MOBLIN_SPEAR = 0x10000000,
};
