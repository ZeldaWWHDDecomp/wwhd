/* bmd_class (Kalle Demos), WWHD layout. 
 *
 * HD size 0xD04 (GameCube 0xBE4; constructor 020B17EC). Members are GameCube +0x11C up to m2FA,
 * then HD adds an s32 (joint index of Makar's model used for the snap picture) at 0x41C, so
 * m2FE..m314 are +0x122; the padding before m318 absorbs 2 bytes, and the rest is +0x120. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_bg_s_movebg_actor.h"
#include "gc_api/d/d_cc_d.h"
#include "gc_api/d/d_bg_s.h"
#include "gc_api/d/d_particle.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
#include "gc_api/f_pc/f_pc_stdcreate_req.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_save.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/m_Do/m_Do_graphic.h"
#include "gc_api/d/d_camera.h"
#include "gc_api/d/d_snap.h"
#include "gc_api/d/d_cc_uty.h"
#include "gc_api/d/d_s_play.h"
#include "gc_api/JSystem/JUtility/JUTReport.h"

#include "gc_api/m_Do/m_Do_audio.h"
#include "gc_api/m_Do/m_Do_ext.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/d/d_lib.h"
#include "d/actor/d_a_player.h"
#include "gc_api/JSystem/JPAMath.h"
#include "d/d_kankyo.h"
#include "d/d_cc_uty.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"

struct bmd_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ gptr<mDoExt_baseAnm> mpBrkAnm;
    /* 0x3D8 */ gptr<mDoExt_baseAnm> mpBtkAnm;
    /* 0x3DC */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpHeadDeadMorf;
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMakarMorf;
    /* 0x3E8 */ gptr<J3DModel> mpMakarFaceModel;
    /* 0x3EC */ gptr<J3DModel> mpR00_EFModel;
    /* 0x3F0 */ gptr<mDoExt_baseAnm> mpR00_EFBrk;
    /* 0x3F4 */ be<f32> m2D8;
    /* 0x3F8 */ be<s8> m2DC;
    /* 0x3F9 */ u8 _3F9[3];
    /* 0x3FC */ cXyz m2E0;
    /* 0x408 */ u8 _408[0x416 - 0x408];
    /* 0x416 */ be<s16> m2FA;
    /* 0x418 */ u8 _418[4];
    /* 0x41C */ be<s32> mMakarSnapJnt; /* HD */
    /* 0x420 */ be<s16> m2FE;
    /* 0x422 */ be<s16> mMode;
    /* 0x424 */ be<s16> m302;
    /* 0x426 */ be<s16> m304;
    /* 0x428 */ be<s16> m306;
    /* 0x42A */ be<s16> m308[4];
    /* 0x432 */ be<s16> m310;
    /* 0x434 */ be<s16> m312;
    /* 0x436 */ be<s16> m314;
    /* 0x438 */ cXyz m318;
    /* 0x444 */ be<f32> m324;
    /* 0x448 */ be<f32> m328;
    /* 0x44C */ u8 _44C[4];
    /* 0x450 */ be<u8> m330;
    /* 0x451 */ be<s8> m331;
    /* 0x452 */ be<s8> m332;
    /* 0x453 */ u8 _453;
    /* 0x454 */ be<s16> m334;
    /* 0x456 */ be<s16> m336;
    /* 0x458 */ be<f32> m338;
    /* 0x45C */ dBgS_AcchCir mAcchCir;
    /* 0x49C */ dBgS_ObjAcch mAcch;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Sph mBodySph;
    /* 0x7C8 */ dCcD_Sph mCoreSph;
    /* 0x8F4 */ dCcD_Cyl mCoCyl;
    /* 0xA24 */ be<s16> m904;
    /* 0xA26 */ u8 _A26[2];
    /* 0xA28 */ be<f32> m908;
    /* 0xA2C */ csXyz m90C[2];
    /* 0xA38 */ be<s16> m918;
    /* 0xA3A */ u8 _A3A[2];
    /* 0xA3C */ be<f32> m91C;
    /* 0xA40 */ be<f32> m920;
    /* 0xA44 */ cXyz m924;
    /* 0xA50 */ cXyz m930;
    /* 0xA5C */ be<s16> m93C;
    /* 0xA5E */ be<s16> m93E;
    /* 0xA60 */ be<s16> m940;
    /* 0xA62 */ be<s8> m942;
    /* 0xA63 */ u8 _A63;
    /* 0xA64 */ Mtx34 m944[5];
    /* 0xB54 */ Mtx34 mA34;
    /* 0xB84 */ gptr<dBgW> pm_bgw[6];
    /* 0xB9C */ gptr<JPABaseEmitter> mA7C[3];
    /* 0xBA8 */ be<s16> mA88[2];
    /* 0xBAC */ be<s16> mA8C;
    /* 0xBAE */ u8 _BAE[2];
    /* 0xBB0 */ dPa_smokeEcallBack mSmokeCb[7];
    /* 0xC90 */ be<u8> mB70;
    /* 0xC91 */ be<s8> mB71;
    /* 0xC92 */ be<s16> mB72;
    /* 0xC94 */ be<s16> mB74;
    /* 0xC96 */ be<s16> mB76;
    /* 0xC98 */ be<s16> mB78;
    /* 0xC9A */ u8 _C9A[2];
    /* 0xC9C */ cXyz mB7C;
    /* 0xCA8 */ cXyz mB88;
    /* 0xCB4 */ be<s16> mB94;
    /* 0xCB6 */ be<s16> mB96;
    /* 0xCB8 */ u8 _CB8[4];
    /* 0xCBC */ be<f32> mB9C;
    /* 0xCC0 */ be<f32> mBA0;
    /* 0xCC4 */ be<f32> mBA4;
    /* 0xCC8 */ be<f32> mBA8;
    /* 0xCCC */ dKankyo::WIND_INFLUENCE mWindInfluence;
    /* 0xCF8 */ be<f32> mBD8;
    /* 0xCFC */ be<f32> mBDC;
    /* 0xD00 */ be<u8> mBE0;
    /* 0xD01 */ u8 _D01[3];
};
WWHD_OFFSET(bmd_class, mpBodyMorf, 0x3D0);
WWHD_OFFSET(bmd_class, m2E0, 0x3FC);
WWHD_OFFSET(bmd_class, m2FE, 0x420);
WWHD_OFFSET(bmd_class, m318, 0x438);
WWHD_OFFSET(bmd_class, mAcchCir, 0x45C);
WWHD_OFFSET(bmd_class, mStts, 0x660);
WWHD_OFFSET(bmd_class, mCoCyl, 0x8F4);
WWHD_OFFSET(bmd_class, m90C, 0xA2C);
WWHD_OFFSET(bmd_class, m944, 0xA64);
WWHD_OFFSET(bmd_class, pm_bgw, 0xB84);
WWHD_OFFSET(bmd_class, mSmokeCb, 0xBB0);
WWHD_OFFSET(bmd_class, mB7C, 0xC9C);
WWHD_OFFSET(bmd_class, mWindInfluence, 0xCCC);
WWHD_OFFSET(bmd_class, mBE0, 0xD00);
WWHD_SIZE(bmd_class, 0xD04);

/* daBmd_HIO_c (HD 0x18: vtable at +0x14) */
struct daBmd_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<u32> __vtbl;
};
WWHD_SIZE(daBmd_HIO_c, 0x18);

/* ---- TU constants ---- */
#define SAFESTRING_VTBL 0x10009AA8
#define BMD_VTBL 0x10009B10
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10009AD0, 0x10009AF0, 0x10009AE0};
enum { TEV_TYPE_BG1 = 2 };
// Archive indices follow the GameCube Bmd resource header.
enum dRes_INDEX_BMD {
    /* BCK  */
    dRes_INDEX_BMD_BCK_CALL_01_e = 0x8,
    dRes_INDEX_BMD_BCK_CALL_02_e = 0x9,
    dRes_INDEX_BMD_BCK_CALL_03_e = 0xA,
    dRes_INDEX_BMD_BCK_COA_DAMAGE_e = 0xB,
    dRes_INDEX_BMD_BCK_COA_DEAD1_e = 0xC,
    dRes_INDEX_BMD_BCK_COA_DEAD2_e = 0xD,
    dRes_INDEX_BMD_BCK_COA_LDEAD_e = 0xE,
    dRes_INDEX_BMD_BCK_COA_NDEAD1_e = 0xF,
    dRes_INDEX_BMD_BCK_COA_NDEAD2_e = 0x10,
    dRes_INDEX_BMD_BCK_COA_START1_e = 0x11,
    dRes_INDEX_BMD_BCK_COA_START2_e = 0x12,
    dRes_INDEX_BMD_BCK_COA_START3_e = 0x13,
    dRes_INDEX_BMD_BCK_COA_WAIT_e = 0x14,
    dRes_INDEX_BMD_BCK_DROP_e = 0x15,
    dRes_INDEX_BMD_BCK_HANA_ATTACK_e = 0x16,
    dRes_INDEX_BMD_BCK_HANA_DEAD1_e = 0x17,
    dRes_INDEX_BMD_BCK_HANA_DEAD2_e = 0x18,
    dRes_INDEX_BMD_BCK_HANA_DEAD3_e = 0x19,
    dRes_INDEX_BMD_BCK_HANA_EAT_e = 0x1A,
    dRes_INDEX_BMD_BCK_HANA_HAKIDASU_e = 0x1B,
    dRes_INDEX_BMD_BCK_HANA_OTIRU_e = 0x1C,
    dRes_INDEX_BMD_BCK_HANA_START1_e = 0x1D,
    dRes_INDEX_BMD_BCK_HANA_START2_e = 0x1E,
    dRes_INDEX_BMD_BCK_HANA_TOJIRU_e = 0x1F,
    dRes_INDEX_BMD_BCK_HANA_WAIT_e = 0x20,
    dRes_INDEX_BMD_BCK_HIRAKU_WAIT_e = 0x21,
    dRes_INDEX_BMD_BCK_NEW_SDEMO1_e = 0x22,
    dRes_INDEX_BMD_BCK_NEW_SDEMO2_e = 0x23,
    dRes_INDEX_BMD_BCK_NEW_SDEMO3_e = 0x24,
    dRes_INDEX_BMD_BCK_NEW_SDEMO4_e = 0x25,
    /* BDLM */
    dRes_INDEX_BMD_BDL_R00_EF_e = 0x28,
    /* BMDM */
    dRes_INDEX_BMD_BMD_BKM_e = 0x2B,
    dRes_INDEX_BMD_BMD_BKM_COA_e = 0x2C,
    dRes_INDEX_BMD_BMD_BKM_COA_DEADMODEL_e = 0x2D,
    dRes_INDEX_BMD_BMD_CB_e = 0x2E,
    dRes_INDEX_BMD_BMD_CB_FACE_e = 0x2F,
    dRes_INDEX_BMD_BMD_CB_STICK_e = 0x30,
    /* BRK  */
    dRes_INDEX_BMD_BRK_BKM_e = 0x33,
    dRes_INDEX_BMD_BRK_R00_EF_e = 0x34,
    /* BTK  */
    dRes_INDEX_BMD_BTK_BKM_e = 0x37,
    /* DZB  */
    dRes_INDEX_BMD_DZB_COLL1_e = 0x3A,
    dRes_INDEX_BMD_DZB_COLL2_e = 0x3B,
};

enum BmdSoundId : u32 {
    JA_BGM_KINDAN_BOSS = 0x80000005,
    JA_BGM_PAST_BKM = 0x8000004A,
    JA_BGM_UNK_105 = 0x80000105,
    JA_BGM_UNK_151 = 0x80000151,
    JA_SE_CM_BKM_ATKVINE_ENTER = 0x584C,
    JA_SE_CM_BKM_BODY_CLOSE = 0x780F,
    JA_SE_CM_BKM_BODY_EATING = 0x7810,
    JA_SE_CM_BKM_BODY_FALL = 0x780E,
    JA_SE_CM_BKM_BODY_UP = 0x780D,
    JA_SE_CM_BKM_CORE_ENTER = 0x584A,
    JA_SE_CM_BKM_END_CORE_DROP = 0x5855,
    JA_SE_CM_BKM_END_CORE_LEAP = 0x5856,
    JA_SE_CM_BKM_END_FLW_DIE = 0x7817,
    JA_SE_CM_BKM_END_FLW_LIFTUP = 0x7816,
    JA_SE_CM_BKM_END_FLW_WAVING = 0x7815,
    JA_SE_CM_BKM_FLW_TO_BUD = 0x780B,
    JA_SE_CM_BKM_FLW_WAVING = 0x780A,
    JA_SE_CM_BKM_LINK_OUT = 0x584F,
    JA_SE_CM_BKM_VINE_OUT = 0x780C,
    JA_SE_CV_BKM_CUT_VINE_ALL = 0x4854,
    JA_SE_CV_BKM_DAMAGE = 0x4855,
    JA_SE_CV_BKM_DIE = 0x4859,
    JA_SE_CV_BKM_ENTER = 0x485A,
    JA_SE_CV_BKM_FALLDOWN = 0x4857,
    JA_SE_CV_BKM_JITABATA = 0x4858,
    JA_SE_CV_BKM_LAST_DAMAGE = 0x4856,
    JA_SE_CV_BKM_LAUGH = 0x485B,
    JA_SE_CV_CB_HELP = 0x48C5,
    JA_SE_CV_CB_SAVED = 0x48C7,
    JA_SE_LK_B_BOSS_WARP = 0x2888,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_STRM_BOSS_CLEAR = 0xC0000003,
};
namespace dPa_name {
enum BmdParticleId : u16 {
    ID_AK_SN_BKMCLOSEHOUSHI00 = 0x8100,
    ID_AK_SN_BKMCORESIGH00 = 0x80FE,
    ID_AK_SN_BKMDROPDOWNSOIL00 = 0x80F1,
    ID_AK_SN_BKMHAKIDASHIHOUSHI01 = 0x80F7,
    ID_AK_SN_BKMHAKIDASHISMOKE00 = 0x80F8,
    ID_AK_SN_BKMHANAKARERU00 = 0x80FF,
    ID_AK_SN_BKMOPENHOUSHI00 = 0x80F3,
    ID_AK_ST_BKMCOREDEADSMOKE00 = 0xA0FD,
    ID_AK_ST_BKMDROPDOWNSMOKE00 = 0xA0F2,
    ID_AK_ST_BKMOPENSMOKE00 = 0xA0F4,
};
}
enum { DSNAP_TYPE_BMD = 0xC8, BKM_COA_DEADMODEL_JNT_UWAAGO_e = 5 };

/* joints (res/Object/Bmd.h) */
enum {
    BKM_COA_JNT_KUBI1_e = 0,
    BKM_COA_JNT_SITAAGO_e = 5,
    BKM_COA_JNT_TOSAKA1_e = 7,
    CB_JNT_BACKBONE_e = 1,
};

/* ---- statics ---- */
static inline daBmd_HIO_c& l_HIO() { return *gabi::at<daBmd_HIO_c>(0x104624A0); }

/* ---- HD inlined helpers ---- */
/* cXyz assignment through FPRs (lfs/stfs: a signalling NaN is quieted) */
static inline void cXyz_fcopy(cXyz* dst, const cXyz* src) {
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}
/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAudioApi::monsSeStart(id, pos, actorId, param, reverb) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        mDoAudioApi::monsSeStart(id, &a->eyePos, pid, param, reverb);
    }
}
// HD: background callbacks are stored in the extended world prefix.
struct BmdBackgroundView : dBgW {
    u8 mPrefix[0xA8 - sizeof(dBgW)];
    be<u32> mpCrrFunc;
    u8 mCallbackPadding[4];
    be<u32> mpRideCallback;
    void SetCrrFunc(decltype(&dBgS_MoveBGProc_Typical) callback) {
        mpCrrFunc = gabi::guest_fn((const void*)callback);
    }
    void SetRideCallback(void (*callback)(dBgW*, fopAc_ac_c*, fopAc_ac_c*)) {
        mpRideCallback = gabi::guest_fn((const void*)callback);
    }
};
WWHD_OFFSET(BmdBackgroundView, mpCrrFunc, 0xA8);
WWHD_OFFSET(BmdBackgroundView, mpRideCallback, 0xB0);
WWHD_SIZE(BmdBackgroundView, 0xB4);
struct BmdCreateAppend {
    be<u32> parameters;
};
WWHD_OFFSET(BmdCreateAppend, parameters, 0);
WWHD_SIZE(BmdCreateAppend, 4);
// HD: the shared d_com_inf_game accessors expose the boss-item getters; the setter reaches the same shared memory member.
static inline void bmd_onDungeonItem(s32 item) {
    dSv_memBit_onDungeonItem(&dComIfGs_info()->mMemory.mMembit, item);
}
enum { fpcNm_BMDHAND_e = 0xEC, fpcNm_BMDFOOT_e = 0xED };

/* ---- functions of this unit called from other files ---- */
void anm_init(bmd_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void move1(bmd_class* i_this);
void mk_voice_set(bmd_class* i_this, u32 param_2);
void move(bmd_class* i_this);
void demo_camera(bmd_class* i_this);

// HD: guest stage storage may lie outside the directly mapped guest window.
static inline u8 bmd_startStageName0() { return *gabi::at<be<u8>>(gabi::ea(dComIfGp_getStartStageName())); }

// HD: the node conversion precedes the model lookup; joint data is read afterward.
struct BmdJointData {
    u8 mHeader[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(BmdJointData, mJntNo, 4);

// HD: the camera view prefix used by the boss cutscene.
struct BmdCameraLookAtView {
    u8 mPrefix[0xDC];
    cXyz mEye;
    cXyz mCenter;
};
WWHD_OFFSET(BmdCameraLookAtView, mEye, 0xDC);
WWHD_OFFSET(BmdCameraLookAtView, mCenter, 0xE8);
WWHD_SIZE(BmdCameraLookAtView, 0xF4);

// HD: the emitter tail used by the boss is absent from the common prefix view.
struct BmdEmitterView {
    u8 mPrefix[0x5C];
    be<s32> mMaxFrame;
    u8 mDynamics[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    u8 mRenderState[0x254 - 0x244];
    be<u32> mFlags;
    void setGlobalRTMatrix(Mtx34* mtx) { JPASetRMtxTVecfromMtx(mtx, &mGlobalRotation, &mGlobalTranslation); }
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
    void setGlobalScale(f32 scale) {
        mGlobalDynamicsScale.x = scale;
        mGlobalDynamicsScale.y = scale;
        mGlobalDynamicsScale.z = scale;
        mGlobalParticleScale.x = scale;
        mGlobalParticleScale.y = scale;
        mGlobalParticleScale.z = scale;
    }
};
WWHD_OFFSET(BmdEmitterView, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BmdEmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BmdEmitterView, mGlobalParticleScale, 0x238);
WWHD_OFFSET(BmdEmitterView, mFlags, 0x254);
WWHD_SIZE(BmdEmitterView, 0x258);

// HD: the follow camera is embedded after the process/view prefix.
struct BmdCameraProcessView {
    u8 mPrefix[0x248];
    dCamera_c mCamera;
};
WWHD_OFFSET(BmdCameraProcessView, mCamera, 0x248);
WWHD_SIZE(BmdCameraProcessView, 0x248 + sizeof(dCamera_c));

// HD: integer copies preserve signaling NaNs in the non-float parts of tevStr.
static inline void bmd_copyWords(void* dst, const void* src, u32 count) {
    for (u32 i = 0; i < count; ++i)
        gabi::store<u32>(gabi::ea(dst) + i * 4, gabi::load<u32>(gabi::ea(src) + i * 4));
}
struct BmdVectorWords {
    be<u32> x, y, z;
};
WWHD_OFFSET(BmdVectorWords, z, 8);
WWHD_SIZE(BmdVectorWords, 12);

// HD: the create function calls these entries through the guest ABI.
void bmd_constructEntry(bmd_class* actor);
void bmd_executeEntry(bmd_class* actor);
