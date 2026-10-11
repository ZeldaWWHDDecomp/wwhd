/* d_a_wz.h (WWHD): Wizzrobe. 
 * Layout from the WWHD code (GameCube fields +0x11C); names by WWHD offset where the GameCube
 * header has none. Size 0x10B0 (GameCube 0xF8C, the HD dCcD/J3D helpers add the rest). */
#pragma once
#include "bindings.h"

struct wz_class : fopEn_enemy_c {
    /* 0x3C8 */ u8 mPhase[0x8];                 /* request_of_phase_process_class */
    /* 0x3D0 */ gptr<mDoExt_McaMorf> m2B4;      // HD: GameCube m2B4, shifted by the enemy base.
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpRodMorf; /* the rod */
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf2;   /* mini-boss second body */
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x3E0 */ gptr<mDoExt_btkAnm> mpBtkAnm;
    /* 0x3E4 */ u8 _3E4[0x3E8 - 0x3E4];
    /* 0x3E8 */ Mtx34 mBallMtx; /* summon door / ball matrix */
    /* 0x418 */ cXyz mHandPos;  /* joint 0x11 */
    /* 0x424 */ be<f32> mRodScaleX;
    /* 0x428 */ be<f32> mRodScaleY;
    /* 0x42C */ be<f32> mRodScaleZ;
    /* 0x430 */ cXyz m314; // HD: GameCube target position m314.
    /* 0x43C */ cXyz mRodTipPos;
    /* 0x448 */ u8 _448[0x44C - 0x448];
    /* 0x44C */ be<f32> mBaseY;
    /* 0x450 */ u8 _450[0x454 - 0x450];
    /* 0x454 */ cXyz mSummonPos;
    /* 0x460 */ be<u32> mParentId;
    /* 0x464 */ be<u8> mBehaviorType; /* GameCube 0x348 */
    /* 0x465 */ be<u8> mEnemySummonTableIndex;
    /* 0x466 */ be<u8> mEnableSpawnSwitch;
    /* 0x467 */ be<u8> mDisableSpawnOnDeathSwitch;
    /* 0x468 */ be<u8> mAction;
    /* 0x469 */ be<u8> mMode;
    /* 0x46A */ be<u8> mHitKind;
    /* 0x46B */ be<u8> mHitLock;
    /* 0x46C */ be<u8> mIsMiniBoss; /* GameCube 0x350 */
    /* 0x46D */ be<u8> m351;
    /* 0x46E */ u8 _46E[0x46F - 0x46E];
    /* 0x46F */ be<u8> mStealCount;
    /* 0x470 */ be<u8> m470;
    /* 0x471 */ u8 _471[0x478 - 0x471];
    /* 0x478 */ be<u32> mChildId[20]; /* summoner: the summoned enemies */
    /* 0x4C8 */ be<u8> mChildFlag[20];
    /* 0x4DC */ u8 _4DC[0x4F0 - 0x4DC];
    /* 0x4F0 */ be<s16> m4F0;
    /* 0x4F2 */ u8 _4F2[0x4FA - 0x4F2];
    /* 0x4FA */ be<s16> m4FA[4];
    /* 0x502 */ u8 _502[0x504 - 0x502];
    /* 0x504 */ be<s16> mFuwaAngle;
    /* 0x506 */ be<s16> mAlpha;
    /* 0x508 */ be<s32> mSummonWave; /* mini-boss: the summon table row */
    /* 0x50C */ be<s32> mSummonSlot;
    /* 0x510 */ be<s16> mBallFlareTimer;
    /* 0x512 */ be<s16> mSummonCount;
    /* 0x514 */ u8 _514[0x518 - 0x514];
    /* 0x518 */ be<s32> mCurrBckIdx;
    /* 0x51C */ be<f32> mCorrectionOffsetY;
    /* 0x520 */ be<f32> mAcchWallH;
    /* 0x524 */ be<f32> mAcchWallR;
    /* 0x528 */ u8 _528[0x540 - 0x528];
    /* 0x540 */ be<u32> m540;
    /* 0x544 */ u8 _544[0x551 - 0x544];
    /* 0x551 */ be<u8> mSummonSw;
    /* 0x552 */ u8 _552[0x580 - 0x552];
    /* 0x580 */ dBgS_AcchCir mAcchCir;
    /* 0x5C0 */ dBgS_Acch mAcch;
    /* 0x784 */ dCcD_Stts mStts;
    /* 0x7C0 */ dCcD_Cyl mBodyCyl;
    /* 0x8F0 */ dCcD_Sph m7D8;          // HD: GameCube collision sphere m7D8.
    /* 0xA1C */ LIGHT_INFLUENCE mLight; /* LIGHT_INFLUENCE */
    /* 0xA40 */ u8 _A40[0xA44 - 0xA40];
    /* 0xA44 */ be<s16> mFreezeDuration;
    /* 0xA46 */ be<u8> mLightShrinkTimer;
    /* 0xA47 */ u8 _A47[0xA48 - 0xA47];
    /* 0xA48 */ be<f32> mFreezeYOffset;
    /* 0xA4C */ be<u8> mIceMode;
    /* 0xA4D */ u8 _A4D[0xA4E - 0xA4D];
    /* 0xA4E */ be<s16> mIceTimer;
    /* 0xA50 */ u8 _A50[0xA70 - 0xA50];
    /* 0xA70 */ u8 mStts2[0x3C];
    /* 0xAAC */ u8 mCyl2[0x130];
    /* 0xBDC */ u8 _BDC[0xBEC - 0xBDC];
    /* 0xBEC */ be<f32> mIceScaleXZ;
    /* 0xBF0 */ u8 _BF0[0xBF4 - 0xBF0];
    /* 0xBF4 */ u8 mAcchCir2[0x40];
    /* 0xC34 */ u8 mAcch2[0x1C4];
    /* 0xDF8 */ u8 mEnemyFire[0x22C];
    /* 0x1024 */ u8 _1024[0x1038 - 0x1024];
    /* 0x1038 */ dPa_followEcallBack mFollowCb[5];
    /* 0x109C */ u8 mInvisModel[0x8];
    /* 0x10A4 */ u8 mInvisModel2[0x8];
    /* 0x10AC */ u8 _10AC[0x10B0 - 0x10AC];
};
WWHD_SIZE(wz_class, 0x10B0);
WWHD_OFFSET(wz_class, mPhase, 0x3C8);
WWHD_OFFSET(wz_class, m2B4, 0x3D0);
WWHD_OFFSET(wz_class, mpRodMorf, 0x3D4);
WWHD_OFFSET(wz_class, mpMorf2, 0x3D8);
WWHD_OFFSET(wz_class, mpBrkAnm, 0x3DC);
WWHD_OFFSET(wz_class, mpBtkAnm, 0x3E0);
WWHD_OFFSET(wz_class, mBallMtx, 0x3E8);
WWHD_OFFSET(wz_class, mHandPos, 0x418);
WWHD_OFFSET(wz_class, mRodScaleX, 0x424);
WWHD_OFFSET(wz_class, mRodScaleY, 0x428);
WWHD_OFFSET(wz_class, mRodScaleZ, 0x42C);
WWHD_OFFSET(wz_class, m314, 0x430);
WWHD_OFFSET(wz_class, mRodTipPos, 0x43C);
WWHD_OFFSET(wz_class, mBaseY, 0x44C);
WWHD_OFFSET(wz_class, mSummonPos, 0x454);
WWHD_OFFSET(wz_class, mParentId, 0x460);
WWHD_OFFSET(wz_class, mBehaviorType, 0x464);
WWHD_OFFSET(wz_class, mEnemySummonTableIndex, 0x465);
WWHD_OFFSET(wz_class, mEnableSpawnSwitch, 0x466);
WWHD_OFFSET(wz_class, mDisableSpawnOnDeathSwitch, 0x467);
WWHD_OFFSET(wz_class, mAction, 0x468);
WWHD_OFFSET(wz_class, mMode, 0x469);
WWHD_OFFSET(wz_class, mHitKind, 0x46A);
WWHD_OFFSET(wz_class, mHitLock, 0x46B);
WWHD_OFFSET(wz_class, mIsMiniBoss, 0x46C);
WWHD_OFFSET(wz_class, m351, 0x46D);
WWHD_OFFSET(wz_class, mStealCount, 0x46F);
WWHD_OFFSET(wz_class, m470, 0x470);
WWHD_OFFSET(wz_class, mChildId, 0x478);
WWHD_OFFSET(wz_class, mChildFlag, 0x4C8);
WWHD_OFFSET(wz_class, m4F0, 0x4F0);
WWHD_OFFSET(wz_class, m4FA, 0x4FA);
WWHD_OFFSET(wz_class, mSummonWave, 0x508);
WWHD_OFFSET(wz_class, mSummonSlot, 0x50C);
WWHD_OFFSET(wz_class, mSummonCount, 0x512);
WWHD_OFFSET(wz_class, mFuwaAngle, 0x504);
WWHD_OFFSET(wz_class, mAlpha, 0x506);
WWHD_OFFSET(wz_class, mBallFlareTimer, 0x510);
WWHD_OFFSET(wz_class, mCurrBckIdx, 0x518);
WWHD_OFFSET(wz_class, mCorrectionOffsetY, 0x51C);
WWHD_OFFSET(wz_class, mAcchWallH, 0x520);
WWHD_OFFSET(wz_class, mAcchWallR, 0x524);
WWHD_OFFSET(wz_class, m540, 0x540);
WWHD_OFFSET(wz_class, mSummonSw, 0x551);
WWHD_OFFSET(wz_class, mAcchCir, 0x580);
WWHD_OFFSET(wz_class, mAcch, 0x5C0);
WWHD_OFFSET(wz_class, mStts, 0x784);
WWHD_OFFSET(wz_class, mBodyCyl, 0x7C0);
WWHD_OFFSET(wz_class, m7D8, 0x8F0);
WWHD_OFFSET(wz_class, mLight, 0xA1C);
WWHD_OFFSET(wz_class, mFreezeDuration, 0xA44);
WWHD_OFFSET(wz_class, mLightShrinkTimer, 0xA46);
WWHD_OFFSET(wz_class, mFreezeYOffset, 0xA48);
WWHD_OFFSET(wz_class, mIceMode, 0xA4C);
WWHD_OFFSET(wz_class, mIceTimer, 0xA4E);
WWHD_OFFSET(wz_class, mStts2, 0xA70);
WWHD_OFFSET(wz_class, mCyl2, 0xAAC);
WWHD_OFFSET(wz_class, mIceScaleXZ, 0xBEC);
WWHD_OFFSET(wz_class, mAcchCir2, 0xBF4);
WWHD_OFFSET(wz_class, mAcch2, 0xC34);
WWHD_OFFSET(wz_class, mEnemyFire, 0xDF8);
WWHD_OFFSET(wz_class, mFollowCb, 0x1038);
WWHD_OFFSET(wz_class, mInvisModel, 0x109C);
WWHD_OFFSET(wz_class, mInvisModel2, 0x10A4);

// HD: private typed views for the draw, animation and damage functions. The state-machine
// fields above keep their verified storage, which the remaining functions use.
namespace wz {
enum Joint { JOINT_ROD_TIP = 2, JOINT_HAND = 17, JOINT_SECOND_BODY = 19 };
enum Behavior { FIRE_BALL = 10, SUMMON_BALL = 11, SUMMON_DOOR = 12, INTRO_DOOR = 13 };
enum AttackType : u32 {
    AT_TYPE_SWORD = 2,
    AT_TYPE_BOMB = 0x20,
    AT_TYPE_BOOMERANG = 0x40,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_FIRE = 0x200,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT = 0x100000,
    AT_TYPE_WIND = 0x200000,
    AT_TYPE_GRAPPLING_HOOK = 0x8000000
};
struct ColorF {
    be<f32> r, g, b, a;
};
WWHD_SIZE(ColorF, 0x10);
struct MaterialTable {
    u8 mUnaccessed[0x24];
    be<u16> mMaterialNum;
};
WWHD_OFFSET(MaterialTable, mMaterialNum, 0x24);
WWHD_SIZE(MaterialTable, 0x26);
struct TevVirtuals {
    u8 mUnaccessed[0x3C];
    be<u32> mSetTevKColor;
    u8 mOtherVirtuals[0x4C - 0x40];
    be<u32> mGetTevKColor;
};
WWHD_OFFSET(TevVirtuals, mGetTevKColor, 0x4C);
WWHD_SIZE(TevVirtuals, 0x50);
struct TevBlock {
    u8 mUnaccessed[4];
    gptr<TevVirtuals> __vtbl;
    GXColor* getTevKColor(u32 index) {
        return gabi::call_ptr<GXColor*>(__vtbl->mGetTevKColor, this, index);
    }
    void setTevKColor(u32 index, GXColor* color) {
        gabi::call_ptr(__vtbl->mSetTevKColor, this, index, color);
    }
};
WWHD_OFFSET(TevBlock, __vtbl, 4);
WWHD_SIZE(TevBlock, 8);
struct Material {
    u8 mUnaccessed[0x18];
    gptr<TevBlock> mTevBlock;
    u8 mOtherBlocks[0xA0 - 0x1C];
    be<u32> mFlags;
    u8 mRemaining[0x39C - 0xA4];
};
WWHD_OFFSET(Material, mTevBlock, 0x18);
WWHD_OFFSET(Material, mFlags, 0xA0);
WWHD_SIZE(Material, 0x39C);
struct InvisibleModel {
    u8 mStorage[8];
};
WWHD_SIZE(InvisibleModel, 8);
struct Player : fopAc_ac_c {
    be<u8> mCutType;
};
WWHD_OFFSET(Player, mCutType, 0x3AC);
WWHD_SIZE(Player, 0x3AD);
struct FirePrefix {
    gptr<fopAc_ac_c> mpActor;
    be<s16> mFireDuration;
};
WWHD_OFFSET(FirePrefix, mFireDuration, 4);
WWHD_SIZE(FirePrefix, 6);
struct CcAtInfo {
    gptr<dCcD_GObjInf> mpObj;
    u8 mUnaccessed[0x10];
    gptr<cXyz> pParticlePos;
    u8 mTail[4];
};
WWHD_OFFSET(CcAtInfo, pParticlePos, 0x14);
WWHD_SIZE(CcAtInfo, 0x1C);
template <class T> T* view(void* p) { return reinterpret_cast<T*>(p); }
template <class T> const T* view(const void* p) { return reinterpret_cast<const T*>(p); }
inline mDoExt_McaMorf* morf(u32 address) { return gabi::at<mDoExt_McaMorf>(address); }
} // namespace wz
wz::MaterialTable* wz_getMaterialTable(J3DModelData* data);
void wz_convertColor(wz::ColorF* out, wz::ColorF* in, f32 scale);
wz::ColorF* wz_getMaterialColor(be<u32>* flags, u32 index);
void wz_setTranslucent(J3DModel* model, u32 translucent);
void wz_invisibleEntry(wz::InvisibleModel* model);
void wz_invisibleEntryOpa(wz::InvisibleModel* model);
void wz_iceEntryDL(mDoExt_McaMorf* morf, s32 index, u32 param);
void wz_iceInvisibleDL(mDoExt_McaMorf* morf, s32 index, wz::InvisibleModel* model);
void wz_ccAtCheck(fopAc_ac_c* actor, wz::CcAtInfo* info);

namespace wz {
struct JointData {
    be<u32> mHeader;
    be<u16> mJntNo;
};
WWHD_OFFSET(JointData, mJntNo, 4);
WWHD_SIZE(JointData, 6);
} // namespace wz

namespace wz {
struct ModelDataAnimations {
    u8 mUnaccessed[0x44];
    be<u32> mpTextureSRTAnm;
    be<u32> mpTevRegAnm;
};
WWHD_OFFSET(ModelDataAnimations, mpTextureSRTAnm, 0x44);
WWHD_OFFSET(ModelDataAnimations, mpTevRegAnm, 0x48);
WWHD_SIZE(ModelDataAnimations, 0x4C);
} // namespace wz

namespace wz {
enum {
    fpcNm_WZ_e = 0xD0,
    JA_SE_CV_WZ_ATTACK = 0x48DA,
    JA_SE_CM_WZ_ST_FIREBALL = 0x590A,
    JA_SE_CM_WZ_ST_SUMMONBALL = 0x590B,
    JA_SE_CV_WZ_NO_DAMAGE = 0x48D8,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    ID_AK_JN_OK = 0xD,
    ID_IT_JN_PIYOHIT00 = 0x27B
};
enum HitKind {
    HIT_NORMAL = 0,
    HIT_CRITICAL = 1,
    HIT_WIND = 3,
    HIT_BOOMERANG = 4,
    HIT_BOMB = 6,
    HIT_HAMMER = 7,
    HIT_HAMMER_CRITICAL = 8,
    HIT_GRAPPLE = 9
};
} // namespace wz

static BOOL nodeCallBack(J3DNode* node, s32 timing);
static BOOL rod_nodeCallBack(J3DNode* node, s32 timing);
void draw_SUB(wz_class* i_this);
static void wz_colorToF(wz::ColorF* out, const GXColor* in);
static BOOL daWZ_Draw(wz_class* i_this);
void anm_init(wz_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 playSpeed,
              int soundFileIdx);
void rod_size_set(wz_class* i_this, u8 shrink);
BOOL body_atari_check(wz_class* i_this);
void BG_check(wz_class* i_this);
void weapon_shoot(wz_class* i_this, u8 kind);
static BOOL wz_jointCallBack(J3DNode* node, s32 timing, u16 jointNo, cXyz wz_class::* output);
static void wz_jointMtxCopy(gptr<mDoExt_McaMorf>& srcMorf, u32 jointNo,
                            gptr<mDoExt_McaMorf>& dstMorf);
static void wz_setMaterialAlpha(wz_class* i_this, J3DModelData* data);
void enemy_fire(void* fire);
void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb);
fpc_ProcID fopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo,
                              csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc);
static inline void wz_se(wz_class* actor, u32 id, u32 param, bool checkActor = true);
static inline void wz_stunSe(wz_class* actor, bool checkActor);
static inline void wz_followEnd(wz_class* actor, int index);

struct wzCcAtInfo {
    be<u32> mpObj;
    u8 _04[0x10];
    be<u32> pParticlePos;
    u8 _18[4];
};

struct wzCreateFrame {
    u8 _00[0xC];
    u8 timg[0x20];  /* 0x0C: ResTIMG (0x24 with the next word) */
    be<u32> texObj; /* 0x2C */
    u8 _30[0xB0 - 0x30];
};

WWHD_SIZE(wzCcAtInfo, 0x1C);
WWHD_OFFSET(wzCreateFrame, texObj, 0x2C);
WWHD_SIZE(wzCreateFrame, 0xB0);

// HD: state-machine fields in the padding of the owner layout.
namespace wz {
struct ActionFields {
  u8 _0[0x448 - 0x0];
  /* 0x448 */ be<f32> mHomeX;
  u8 _44C[0x450 - 0x44C];
  /* 0x450 */ be<f32> mHomeZ;
  u8 _454[0x46E - 0x454];
  /* 0x46E */ be<u8> mHasSummonedPartner;
  u8 _46F;
  be<u8> mHasActiveSummon;
  u8 _471[3];
  /* 0x474 */ be<u32> mPathPoint;
  u8 _478[0x4DC - 0x478];
  /* 0x4DC */ be<u8> mDemoChildFlag[20];
  /* 0x4F0 */ be<s16> mTimers[5];
  be<s16> mDemoSteps[4];
  /* 0x502 */ be<s16> mTargetAngle;
  u8 _504[0x514 - 0x504];
  /* 0x514 */ be<u8> mWeaponKind;
  u8 _515[0x528 - 0x515];
  /* 0x528 */ be<f32> mBallRadius;
  /* 0x52C */ be<u32> mpPath;
  u8 _530[0x53D - 0x530];
  /* 0x53D */ be<u8> mPathIndex;
  u8 _53E[2];
  be<u32> mpNextRoomPath;
  u8 _544[0x554 - 0x544];
  /* 0x554 */ be<f32> mDemoCenterX;
  /* 0x558 */ be<f32> mDemoCenterY;
  /* 0x55C */ be<f32> mDemoCenterZ;
  /* 0x560 */ be<f32> mDemoEyeX;
  /* 0x564 */ be<f32> mDemoEyeY;
  /* 0x568 */ be<f32> mDemoEyeZ;
  u8 _56C[0x57C - 0x56C];
  /* 0x57C */ be<f32> mDemoFovy;
  u8 _580[0xBDC - 0x580];
  /* 0xBDC */ be<f32> mIceWallHeight;
  /* 0xBE0 */ be<f32> mIceWallRadius;
  u8 _BE4[0x1024 - 0xBE4];
  /* 0x1024 */ cXyz mParticlePos;
  /* 0x1030 */ csXyz mParticleAngle;
  u8 _1036[0x10AC - 0x1036];
  /* 0x10AC */ be<u8> mSummonParentResolved;
  u8 _10AD[3];
};
WWHD_OFFSET(ActionFields, mHomeX, 0x448);
WWHD_OFFSET(ActionFields, mHomeZ, 0x450);
WWHD_OFFSET(ActionFields, mHasSummonedPartner, 0x46E);
WWHD_OFFSET(ActionFields, mPathPoint, 0x474);
WWHD_OFFSET(ActionFields, mDemoChildFlag, 0x4DC);
WWHD_OFFSET(ActionFields, mTimers, 0x4F0);
WWHD_OFFSET(ActionFields, mTargetAngle, 0x502);
WWHD_OFFSET(ActionFields, mWeaponKind, 0x514);
WWHD_OFFSET(ActionFields, mBallRadius, 0x528);
WWHD_OFFSET(ActionFields, mpPath, 0x52C);
WWHD_OFFSET(ActionFields, mPathIndex, 0x53D);
WWHD_OFFSET(ActionFields, mDemoCenterX, 0x554);
WWHD_OFFSET(ActionFields, mDemoCenterY, 0x558);
WWHD_OFFSET(ActionFields, mDemoCenterZ, 0x55C);
WWHD_OFFSET(ActionFields, mDemoEyeX, 0x560);
WWHD_OFFSET(ActionFields, mDemoEyeY, 0x564);
WWHD_OFFSET(ActionFields, mDemoEyeZ, 0x568);
WWHD_OFFSET(ActionFields, mDemoFovy, 0x57C);
WWHD_OFFSET(ActionFields, mIceWallHeight, 0xBDC);
WWHD_OFFSET(ActionFields, mIceWallRadius, 0xBE0);
WWHD_OFFSET(ActionFields, mParticlePos, 0x1024);
WWHD_OFFSET(ActionFields, mParticleAngle, 0x1030);
WWHD_OFFSET(ActionFields, mSummonParentResolved, 0x10AC);
WWHD_SIZE(ActionFields, 0x10B0);
inline ActionFields *state(wz_class *actor) {
  return view<ActionFields>(actor);
}
} // namespace wz

namespace wz {
struct GroundFields {
  u8 _0[0x5E8 - 0x0];
  be<u32> mAcchFlags;
  u8 _5EC[0x654 - 0x5EC];
  be<f32> mGroundHeight;
  u8 _658[0x77C - 0x658];
  be<f32> mWaterHeight;
};
WWHD_OFFSET(GroundFields, mAcchFlags, 0x5E8);
WWHD_OFFSET(GroundFields, mGroundHeight, 0x654);
WWHD_OFFSET(GroundFields, mWaterHeight, 0x77C);
WWHD_SIZE(GroundFields, 0x780);
} // namespace wz

// HD: SDK/GHS entry points used by the action/creation functions.
void wz_removeFire(void *fire);
void wz_stopSubBgm();
void wz_setAcchWall(void *circle, f32 height, f32 radius);
u32 wz_newMcaMorf(void *self, void *model, void *node, void *material,
                  void *animation, s32 loop, f32 speed, s32 start, s32 end,
                  s32 modify, void *sound, u32 flags, u32 displayFlags);
u32 wz_getJointTree(void *modelData);
s32 wz_getActorNameIndex(void *table, const char *name, s32 variant);
void wz_constructCcStatus(void *status);
void wz_constructGStatus(void *status);
void wz_constructSphere(void *sphere);
void wz_constructAcchCircle(void *circle);
void wz_constructAcch(void *acch);
void wz_constructGObjInf(void *object);
void wz_constructCylinderShape(void *cylinder);
void wz_constructInvisibleModel(void *model);
void wz_destroyInvisibleModel(void *model, s32 flags);
void wz_destroySphere(void *sphere, s32 flags);
void wz_destroyCcStatus(void *status, s32 flags);
void wz_destroyAcch(void *acch, s32 flags);
void wz_destroyAcchCircle(void *circle, s32 flags);
void wz_destroyCylinder(void *cylinder, s32 flags);
void wz_destroyActor(void *actor, s32 flags);
BOOL wz_createInvisibleModel(void *invisible, void *model);
void *wz_constructBrk(void *brk);
BOOL wz_initBrk(void *brk, void *modelData, void *animation, s32 modify,
                s32 loop, f32 speed, s32 start, s32 end, s32 color, s32 alpha);
void *wz_constructBtk(void *btk);
BOOL wz_initBtk(void *btk, void *modelData, void *animation, s32 modify,
                s32 loop, f32 speed, s32 start, s32 end, s32 texMtx, s32 flags);
BOOL wz_checkTargetHit(void *object);
u32 wz_getTargetHitObject(void *object);
void wz_clearTargetHit(void *object);
void wz_moveGStatus(void *status);
void wz_playMorf(void *morf, void *position, s32 room, s32 reverb);
u32 wz_getTextureName(void *names, u32 index);
u32 wz_getSystemTexture(void *textures, s32 index);
u32 wz_constructTexture(void *texture, void *image);
u32 wz_allocateTexture(u32 size);
u32 wz_getNextRoomPath(void *path, s32 room);
void wz_setCylinder(void *cylinder, void *source);
void wz_setSphere(void *sphere, void *source);
void wz_setAcch(void *acch, cXyz *position, cXyz *oldPosition,
                fopAc_ac_c *actor, s32 flags, void *circle, cXyz *speed,
                void *angle, void *shapeAngle);
void wz_initCcStatus(void *status, s32 weight, s32 flags, fopAc_ac_c *actor);
void wz_stopCamera(void *camera);
void wz_startCamera(void *camera);
void wz_setCameraTrim(void *camera, s32 trim);
void wz_resetCamera(void *camera, cXyz *center, cXyz *eye);
void wz_setCamera(void *camera, cXyz *center, cXyz *eye, f32 fovy, s16 bank);
void wz_stopQuake(void *vibration, s32 id);
void wz_muteBgm(s32 frames);
void wz_startSubBgm(u32 id);
BOOL wz_orderPotentialEvent(fopAc_ac_c *actor, u16 type, u16 flags, u16 param);
BOOL wz_updateIce(void *ice);
void wz_setEmitterMatrix(void *matrix, void *rotation, void *scale,
                         void *translation);
BOOL wz_checkMoveBg(void *background, void *polygon);
BOOL wz_checkAttackHit(void *object);
void wz_enableArrowColor(void *actor, s32 param);
void wz_setGbaName(fopAc_ac_c *actor, u8 item, u8 name0, u8 name1);
void wz_cancelCarry(fopAc_ac_c *actor);
void wz_setStun(fopAc_ac_c *actor);
void wz_registerCollision(void *collision, void *object);
void wz_registerMass(void *mass, void *object, u8 priority);
void wz_setCylinderCenter(void *cylinder, cXyz *center);
void wz_setCylinderHeight(void *cylinder, f32 height);
void wz_setCylinderRadius(void *cylinder, f32 radius);
void wz_setSphereCenter(void *sphere, cXyz *center);
void wz_setSphereRadius(void *sphere, f32 radius);

namespace wz {
// HD: enemyfire embeds collision status/sphere after its animation and effect
// state.
struct EnemyFire {
  gptr<fopAc_ac_c> mpActor;
  be<s16> mFireDuration;
  u8 _06[6];
  gptr<mDoExt_McaMorf> mpMorf;
  be<u8> mJointIndices[10];
  u8 _1A[2];
  be<f32> mJointScales[10];
  u8 _44[0x8C - 0x44];
  u8 mInlineObject[0xC];
  u8 _98[8];
  dCcD_Stts mStts;
  dCcD_Sph mSphere;
  u8 _208[0x228 - 0x208];
  be<f32> mHDScale;
};
WWHD_OFFSET(EnemyFire, mpMorf, 0xC);
WWHD_OFFSET(EnemyFire, mJointIndices, 0x10);
WWHD_OFFSET(EnemyFire, mJointScales, 0x1C);
WWHD_OFFSET(EnemyFire, mInlineObject, 0x8C);
WWHD_OFFSET(EnemyFire, mStts, 0xA0);
WWHD_OFFSET(EnemyFire, mSphere, 0xDC);
WWHD_OFFSET(EnemyFire, mHDScale, 0x228);
WWHD_SIZE(EnemyFire, 0x22C);
struct ShapeVtables {
  u8 _00[0x110];
  be<u32> mAabbVtable;
  be<u32> mShapeVtable;
};
WWHD_OFFSET(ShapeVtables, mAabbVtable, 0x110);
WWHD_OFFSET(ShapeVtables, mShapeVtable, 0x114);
WWHD_SIZE(ShapeVtables, 0x118);
enum Vtable : u32 {
  VT_ACTOR = 0x10042D90,
  VT_AABB = 0x10042D50,
  VT_ACCH_CHECK = 0x10042D60,
  VT_ACCH_POLY = 0x10042D80,
  VT_ACCH_GROUP = 0x10042D70,
  VT_SHAPE_BASE = 0x100015A8,
  VT_SHAPE_CYL = 0x1004B160,
  VT_CYL = 0x1004B108,
  VT_CYL_GEOMETRY = 0x1004B150
};
} // namespace wz

namespace wz {
struct JointSlot {
  u8 _00[8];
  be<u32> mCallback;
  u8 _0C[0x1C - 0xC];
};
struct JointArray {
  u8 _00[4];
  be<u32> mCount;
  gptr<JointSlot> mpEntries;
};
struct JointTree {
  u8 _00[8];
  be<u16> mJointNum;
};
WWHD_SIZE(JointSlot, 0x1C);
WWHD_OFFSET(JointSlot, mCallback, 8);
WWHD_SIZE(JointArray, 0xC);
WWHD_OFFSET(JointArray, mCount, 4);
WWHD_OFFSET(JointArray, mpEntries, 8);
WWHD_SIZE(JointTree, 0xA);
WWHD_OFFSET(JointTree, mJointNum, 8);
struct PathPoint {
  u8 mArgs[4];
  cXyz m_position;
};
struct Path {
  be<u16> m_num;
  u8 _02[6];
  gptr<PathPoint> m_points;
};
WWHD_SIZE(PathPoint, 0x10);
WWHD_OFFSET(PathPoint, m_position, 4);
WWHD_SIZE(Path, 0xC);
WWHD_OFFSET(Path, m_points, 8);
struct Emitter {
  u8 _00[0x1F0];
  Mtx34 mGlobalRotation;
  cXyz mGlobalDynamicsScale;
  cXyz mGlobalTranslation;
  cXyz mGlobalParticleScale;
  GXColor mGlobalPrmColor;
  GXColor mGlobalEnvColor;
};
WWHD_SIZE(Emitter, 0x24C);
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Emitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(Emitter, mGlobalEnvColor, 0x248);
struct ModelTextureData {
  u8 _00[0x30];
  be<u32> mpTextures;
  be<u32> mpTextureNames;
};
WWHD_OFFSET(ModelTextureData, mpTextures, 0x30);
WWHD_OFFSET(ModelTextureData, mpTextureNames, 0x34);
WWHD_SIZE(ModelTextureData, 0x38);
struct PlayerDemo : fopAc_ac_c {
  u8 _3AC[0x420 - 0x3AC];
  be<s16> mDemoMode;
  u8 _422[6];
  be<u32> mDemoData;
  u8 _42C[4];
  be<u32> mDemoParam;
};
WWHD_OFFSET(PlayerDemo, mDemoMode, 0x420);
WWHD_OFFSET(PlayerDemo, mDemoData, 0x428);
WWHD_OFFSET(PlayerDemo, mDemoParam, 0x430);
WWHD_SIZE(PlayerDemo, 0x434);
struct PlayerVirtuals {
  u8 _00[0x114];
  be<u32> mSetPlayerPosAndAngle;
};
WWHD_SIZE(PlayerVirtuals, 0x118);
WWHD_OFFSET(PlayerVirtuals, mSetPlayerPosAndAngle, 0x114);
struct CameraEntry {
  be<u32> mpCamera;
  u8 _04[0x34 - 4];
};
WWHD_SIZE(CameraEntry, 0x34);
struct PlayCameraFields {
  u8 _00[0x52B8];
  be<u16> mEventFlags;
  u8 _52BA[0x5AF8 - 0x52BA];
  CameraEntry mCamera[1];
  u8 _5B2C[4];
  be<s8> mPlayerCameraId;
};
WWHD_OFFSET(PlayCameraFields, mEventFlags, 0x52B8);
WWHD_OFFSET(PlayCameraFields, mCamera, 0x5AF8);
WWHD_OFFSET(PlayCameraFields, mPlayerCameraId, 0x5B30);
WWHD_SIZE(PlayCameraFields, 0x5B31);
inline PlayCameraFields *playCamera() {
  return gabi::at<PlayCameraFields>(dComIfGp_ea());
}
struct IcePrefix {
  gptr<wz_class> mpActor;
};
WWHD_SIZE(IcePrefix, 4);
} // namespace wz

namespace wz {
// HD: zero-initialized debug/HIO adjustment block.
struct DebugFields {
  u8 _0[0x14 - 0x0];
  be<f32> mFireRed;
  be<f32> mFireGreen;
  be<f32> mFireBlue;
  be<f32> mSummonRed;
  be<f32> mSummonGreen;
  be<f32> mSummonBlue;
  be<f32> mLightPower;
  be<f32> mLightFluctuation;
  u8 _34[0x80 - 0x34];
  be<s16> mBallWait;
  u8 _82[0x90 - 0x82];
  be<s16> mHoldLookCamera;
  be<s16> mHoldIntro;
  u8 _94[0x48C - 0x94];
  be<f32> mEntranceFovy;
  be<f32> mEntranceCenterX;
  be<f32> mEntranceCenterY;
  be<f32> mEntranceCenterZ;
  be<f32> mEntranceEyeX;
  be<f32> mEntranceEyeY;
  be<f32> mEntranceEyeZ;
  be<f32> mEntranceAlphaStep;
  u8 _4AC[0x4B0 - 0x4AC];
  be<f32> mDoorsFovy;
  be<f32> mDoorsCenterX;
  be<f32> mDoorsCenterY;
  be<f32> mDoorsCenterZ;
  be<f32> mDoorsEyeX;
  be<f32> mDoorsEyeY;
  be<f32> mDoorsEyeZ;
  be<f32> mDoorsCameraRate;
  u8 _4D0[0x502 - 0x4D0];
  be<s16> mInvincible;
  u8 _504[0x506 - 0x504];
  be<s16> mVanishAngleStep;
  be<s16> mOpenDoors;
  u8 _50A[0x63C - 0x50A];
  be<f32> mTriggerDistance;
  be<f32> mIntroWait;
  be<f32> mIntroFovy;
  be<f32> mIntroCenterX;
  be<f32> mIntroCenterY;
  be<f32> mIntroCenterZ;
  be<f32> mIntroEyeX;
  be<f32> mIntroEyeY;
  be<f32> mIntroEyeZ;
  be<f32> mIntroLookWait;
  be<f32> mFirstLookWait;
  be<f32> mFirstLookDistance;
  be<f32> mSecondLookWait;
  be<f32> mSecondLookDistance;
  be<f32> mLookFovy;
  be<f32> mLookCenterX;
  be<f32> mLookCenterY;
  be<f32> mLookCenterZ;
  be<f32> mLookCameraRate;
  u8 _688[0x6B0 - 0x688];
  be<s16> mLookEyeX;
  be<s16> mLookEyeY;
  be<s16> mLookEyeZ;
  be<s16> mAppearWait;
  be<s16> mAppearAlphaStep;
  be<s16> mLookEndWait;
  be<s16> mAppearEndAlphaStep;
  u8 _6BE[0x6C8 - 0x6BE];
  be<f32> mFightFovy;
  be<f32> mFightCenterX;
  be<f32> mFightCenterY;
  be<f32> mFightCenterZ;
  be<f32> mFightEyeX;
  be<f32> mFightEyeY;
  be<f32> mFightEyeZ;
  be<f32> mFightCameraRate;
  be<f32> mFightStartFrame;
  be<f32> mBgmWait;
  be<f32> mIntroBackwardSpeed;
  be<f32> mIntroDeceleration;
  be<f32> mEntranceWait;
  u8 _6FC[0x700 - 0x6FC];
  be<f32> mIntroFovyStep;
  be<f32> mDoorsFovyStep;
  be<f32> mFightFovyStep;
  be<f32> mDefeatEffectHeight;
  be<f32> mEyeHeight;
  u8 _714[0x746 - 0x714];
  be<s16> mDefeatWait;
  be<s16> mHoldDefeat;
  u8 _74A[0x752 - 0x74A];
  be<s16> mHoldPartner;
};
WWHD_OFFSET(DebugFields, mFireRed, 0x14);
WWHD_OFFSET(DebugFields, mFireGreen, 0x18);
WWHD_OFFSET(DebugFields, mFireBlue, 0x1C);
WWHD_OFFSET(DebugFields, mSummonRed, 0x20);
WWHD_OFFSET(DebugFields, mSummonGreen, 0x24);
WWHD_OFFSET(DebugFields, mSummonBlue, 0x28);
WWHD_OFFSET(DebugFields, mLightPower, 0x2C);
WWHD_OFFSET(DebugFields, mLightFluctuation, 0x30);
WWHD_OFFSET(DebugFields, mBallWait, 0x80);
WWHD_OFFSET(DebugFields, mHoldLookCamera, 0x90);
WWHD_OFFSET(DebugFields, mHoldIntro, 0x92);
WWHD_OFFSET(DebugFields, mEntranceFovy, 0x48C);
WWHD_OFFSET(DebugFields, mEntranceCenterX, 0x490);
WWHD_OFFSET(DebugFields, mEntranceCenterY, 0x494);
WWHD_OFFSET(DebugFields, mEntranceCenterZ, 0x498);
WWHD_OFFSET(DebugFields, mEntranceEyeX, 0x49C);
WWHD_OFFSET(DebugFields, mEntranceEyeY, 0x4A0);
WWHD_OFFSET(DebugFields, mEntranceEyeZ, 0x4A4);
WWHD_OFFSET(DebugFields, mEntranceAlphaStep, 0x4A8);
WWHD_OFFSET(DebugFields, mDoorsFovy, 0x4B0);
WWHD_OFFSET(DebugFields, mDoorsCenterX, 0x4B4);
WWHD_OFFSET(DebugFields, mDoorsCenterY, 0x4B8);
WWHD_OFFSET(DebugFields, mDoorsCenterZ, 0x4BC);
WWHD_OFFSET(DebugFields, mDoorsEyeX, 0x4C0);
WWHD_OFFSET(DebugFields, mDoorsEyeY, 0x4C4);
WWHD_OFFSET(DebugFields, mDoorsEyeZ, 0x4C8);
WWHD_OFFSET(DebugFields, mDoorsCameraRate, 0x4CC);
WWHD_OFFSET(DebugFields, mInvincible, 0x502);
WWHD_OFFSET(DebugFields, mVanishAngleStep, 0x506);
WWHD_OFFSET(DebugFields, mOpenDoors, 0x508);
WWHD_OFFSET(DebugFields, mTriggerDistance, 0x63C);
WWHD_OFFSET(DebugFields, mIntroWait, 0x640);
WWHD_OFFSET(DebugFields, mIntroFovy, 0x644);
WWHD_OFFSET(DebugFields, mIntroCenterX, 0x648);
WWHD_OFFSET(DebugFields, mIntroCenterY, 0x64C);
WWHD_OFFSET(DebugFields, mIntroCenterZ, 0x650);
WWHD_OFFSET(DebugFields, mIntroEyeX, 0x654);
WWHD_OFFSET(DebugFields, mIntroEyeY, 0x658);
WWHD_OFFSET(DebugFields, mIntroEyeZ, 0x65C);
WWHD_OFFSET(DebugFields, mIntroLookWait, 0x660);
WWHD_OFFSET(DebugFields, mFirstLookWait, 0x664);
WWHD_OFFSET(DebugFields, mFirstLookDistance, 0x668);
WWHD_OFFSET(DebugFields, mSecondLookWait, 0x66C);
WWHD_OFFSET(DebugFields, mSecondLookDistance, 0x670);
WWHD_OFFSET(DebugFields, mLookFovy, 0x674);
WWHD_OFFSET(DebugFields, mLookCenterX, 0x678);
WWHD_OFFSET(DebugFields, mLookCenterY, 0x67C);
WWHD_OFFSET(DebugFields, mLookCenterZ, 0x680);
WWHD_OFFSET(DebugFields, mLookCameraRate, 0x684);
WWHD_OFFSET(DebugFields, mLookEyeX, 0x6B0);
WWHD_OFFSET(DebugFields, mLookEyeY, 0x6B2);
WWHD_OFFSET(DebugFields, mLookEyeZ, 0x6B4);
WWHD_OFFSET(DebugFields, mAppearWait, 0x6B6);
WWHD_OFFSET(DebugFields, mAppearAlphaStep, 0x6B8);
WWHD_OFFSET(DebugFields, mLookEndWait, 0x6BA);
WWHD_OFFSET(DebugFields, mAppearEndAlphaStep, 0x6BC);
WWHD_OFFSET(DebugFields, mFightFovy, 0x6C8);
WWHD_OFFSET(DebugFields, mFightCenterX, 0x6CC);
WWHD_OFFSET(DebugFields, mFightCenterY, 0x6D0);
WWHD_OFFSET(DebugFields, mFightCenterZ, 0x6D4);
WWHD_OFFSET(DebugFields, mFightEyeX, 0x6D8);
WWHD_OFFSET(DebugFields, mFightEyeY, 0x6DC);
WWHD_OFFSET(DebugFields, mFightEyeZ, 0x6E0);
WWHD_OFFSET(DebugFields, mFightCameraRate, 0x6E4);
WWHD_OFFSET(DebugFields, mFightStartFrame, 0x6E8);
WWHD_OFFSET(DebugFields, mBgmWait, 0x6EC);
WWHD_OFFSET(DebugFields, mIntroBackwardSpeed, 0x6F0);
WWHD_OFFSET(DebugFields, mIntroDeceleration, 0x6F4);
WWHD_OFFSET(DebugFields, mEntranceWait, 0x6F8);
WWHD_OFFSET(DebugFields, mIntroFovyStep, 0x700);
WWHD_OFFSET(DebugFields, mDoorsFovyStep, 0x704);
WWHD_OFFSET(DebugFields, mFightFovyStep, 0x708);
WWHD_OFFSET(DebugFields, mDefeatEffectHeight, 0x70C);
WWHD_OFFSET(DebugFields, mEyeHeight, 0x710);
WWHD_OFFSET(DebugFields, mDefeatWait, 0x746);
WWHD_OFFSET(DebugFields, mHoldDefeat, 0x748);
WWHD_OFFSET(DebugFields, mHoldPartner, 0x752);
WWHD_SIZE(DebugFields, 0x754);
} // namespace wz

namespace wz {
struct TextureImage {
  u8 _00[2];
  be<u16> mWidth, mHeight;
  u8 _06[2];
  be<u8> mPaletteFormat;
  u8 _09[3];
  be<u32> mPaletteOffset;
  u8 _10[0xC];
  be<u32> mImageOffset;
  be<u32> mpTexture;
};
WWHD_SIZE(TextureImage, 0x24);
WWHD_OFFSET(TextureImage, mWidth, 2);
WWHD_OFFSET(TextureImage, mHeight, 4);
WWHD_OFFSET(TextureImage, mPaletteFormat, 8);
WWHD_OFFSET(TextureImage, mPaletteOffset, 0xC);
WWHD_OFFSET(TextureImage, mImageOffset, 0x1C);
WWHD_OFFSET(TextureImage, mpTexture, 0x20);
struct TextureTable {
  be<u16> mCount;
  u8 _02[2];
  gptr<TextureImage> mpImages;
  be<u32> mValidHi, mValidLo, mOverflowHi, mOverflowLo, mShiftHi, mShiftLo;
};
WWHD_SIZE(TextureTable, 0x20);
WWHD_OFFSET(TextureTable, mpImages, 4);
WWHD_OFFSET(TextureTable, mValidHi, 8);
WWHD_OFFSET(TextureTable, mOverflowHi, 0x10);
WWHD_OFFSET(TextureTable, mShiftHi, 0x18);
struct NativeTexture {
  u8 _00[8];
  be<u32> mWidth, mHeight;
};
WWHD_SIZE(NativeTexture, 0x10);
WWHD_OFFSET(NativeTexture, mWidth, 8);
WWHD_OFFSET(NativeTexture, mHeight, 0xC);
struct SafeStringVirtuals {
  u8 _00[0x14];
  be<u32> mAssureTermination;
};
WWHD_SIZE(SafeStringVirtuals, 0x18);
WWHD_OFFSET(SafeStringVirtuals, mAssureTermination, 0x14);
} // namespace wz

namespace wz {
WWHD_OFFSET(ActionFields, mHasActiveSummon, 0x470);
WWHD_OFFSET(ActionFields, mDemoSteps, 0x4FA);
WWHD_OFFSET(ActionFields, mpNextRoomPath, 0x540);
enum Action {
  ACTION_NORMAL = 0,
  ACTION_DAMAGE = 1,
  ACTION_DEMO = 3,
  ACTION_BALL = 100,
  ACTION_SUMMON = 200
};
} // namespace wz

static void *wz_class_ct(void *actor);
