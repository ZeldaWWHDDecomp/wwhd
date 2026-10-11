/* Helmaroc King owner helpers. HD layouts preserve the verified guest fields and store order. */
#pragma once
#include "d/actor/d_a_bdk.h"
#include "d/d_jnt_hit.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#undef l_HIO
#undef center_pos
#undef center_pos2
#undef wind_se_pos
// The support source owns this mapped object and gc_tu.
extern cXyz &wind_se_pos;

/* bdkobj_class::mEffs (HD bdo_eff_s, 0x1B8 bytes, from +0x3B8): the fields used here */
struct BdkBaseFragment {
    /* 0x000 */ be<s8> m000;
    /* 0x001 */ u8 _001[7];
    /* 0x008 */ cXyz m008;
    /* 0x014 */ u8 _014[0x10];
    /* 0x024 */ be<f32> m024x; /* HD: horizontal speed */
    /* 0x028 */ be<f32> m024z;
    /* 0x02C */ u8 _02C[0x12];
    /* 0x03E */ csXyz m036;
    /* 0x044 */ u8 _044[0x1B8 - 0x44];
};
WWHD_OFFSET(BdkBaseFragment, m008, 8);
WWHD_OFFSET(BdkBaseFragment, m024x, 0x24);
WWHD_OFFSET(BdkBaseFragment, m024z, 0x28);
WWHD_OFFSET(BdkBaseFragment, m036, 0x3E);
WWHD_SIZE(BdkBaseFragment, 0x1B8);

struct BdkBaseObject : fopAc_ac_c {
    u8 prefix[0x3B8 - sizeof(fopAc_ac_c)];
    BdkBaseFragment mEffs[3];
};
WWHD_OFFSET(BdkBaseObject, mEffs, 0x3B8);
WWHD_SIZE(BdkBaseObject, 0x8E0);
struct BdkBaseParameterBytes {
    be<u8> byte[4];
};
inline u8 BdkBase_parameterByte(fopAc_ac_c *actor) {
    // HD: test only the parameter word's least-significant byte.
    return reinterpret_cast<BdkBaseParameterBytes *>(&actor->mParameters)->byte[3];
}
inline void BdkBase_copyFloat(be<f32> &dst, const be<f32> &src) {
    reinterpret_cast<be<u32> &>(dst) = reinterpret_cast<const be<u32> &>(src);
}
struct BdkBaseHeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    u8 otherGuards[8];
    be<u8> mFirstObject, mSecondObject;
    u8 padding[0x2C - 0x12];
    be<u32> mZero[4];
};
WWHD_OFFSET(BdkBaseHeaderStatics, mFirstObject, 0x10);
WWHD_OFFSET(BdkBaseHeaderStatics, mZero, 0x2C);
WWHD_SIZE(BdkBaseHeaderStatics, 0x3C);
inline BdkBaseHeaderStatics &BdkBase_headerStatics() { return *gabi::at<BdkBaseHeaderStatics>(0x104618BC); }
struct BdkBaseDestructorRecord {
    be<u32> words[3];
};
WWHD_SIZE(BdkBaseDestructorRecord, 0xC);
inline BdkBaseDestructorRecord *BdkBase_descriptors() {
    return gabi::at<BdkBaseDestructorRecord>(0x10190A70);
}
struct BdkBaseEmitter {
    u8 prefix[0x254];
    be<u32> mFlags;
};
WWHD_OFFSET(BdkBaseEmitter, mFlags, 0x254);
inline be<u32> &BdkBase_emitterFlags(JPABaseEmitter *emitter) {
    // HD: add in guest address space, including a null guest emitter.
    return *gabi::at<be<u32>>(gabi::ea(emitter) + offsetof(BdkBaseEmitter, mFlags));
}
struct BdkBaseBackground {
    u8 prefix[0xA8];
    be<u32> mCrrFunc;
};
WWHD_OFFSET(BdkBaseBackground, mCrrFunc, 0xA8);
inline void BdkBase_setCrrFunc(dBgW *bg, u32 fn) { reinterpret_cast<BdkBaseBackground *>(bg)->mCrrFunc = fn; }
struct BdkBaseCallbackVtable {
    u8 prefix[0x44];
    be<u32> remove;
};
WWHD_OFFSET(BdkBaseCallbackVtable, remove, 0x44);
struct BdkBaseCallback {
    gptr<BdkBaseCallbackVtable> mpVtable;
    u8 prefix[0x12 - 4];
    be<u8> mFollowOff;
};
WWHD_OFFSET(BdkBaseCallback, mFollowOff, 0x12);
template <class CB> inline void BdkBase_remove(CB *callback) {
    BdkBaseCallback *cb = reinterpret_cast<BdkBaseCallback *>(callback);
    gabi::call_ptr(cb->mpVtable->remove, callback);
}
inline void BdkBase_setFollowOff(dPa_smokeEcallBack_bdk *callback) {
    reinterpret_cast<BdkBaseCallback *>(callback)->mFollowOff = 1;
}
// HD: only the first 0x44 bytes of each larger light object are copied by these inlines.
struct BdkBaseLight {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mShortParams[4];
    be<f32> mFloatParams[8];
};
WWHD_OFFSET(BdkBaseLight, mColor, 0x18);
WWHD_OFFSET(BdkBaseLight, mShortParams, 0x1C);
WWHD_OFFSET(BdkBaseLight, mFloatParams, 0x24);
WWHD_SIZE(BdkBaseLight, 0x44);
struct BdkBaseTev {
    BdkBaseLight mLight;
    u8 gap[0x84 - 0x44];
    cXyz mLightPosWorld;
    be<s16> mColorC0[4];
    be<u8> mColorK[8];
    be<s16> mFogColor[4];
    be<f32> mFogStartZ, mFogEndZ, mColpatBlend;
    be<u8> mFlags[9];
    u8 padding[3];
    BdkBaseLight mHdLight1;
    u8 hdGap[0x144 - 0x104];
    BdkBaseLight mHdLight2;
    u8 tail[0x1C8 - 0x188];
};
WWHD_OFFSET(BdkBaseTev, mLightPosWorld, 0x84);
WWHD_OFFSET(BdkBaseTev, mColorC0, 0x90);
WWHD_OFFSET(BdkBaseTev, mColorK, 0x98);
WWHD_OFFSET(BdkBaseTev, mFogColor, 0xA0);
WWHD_OFFSET(BdkBaseTev, mFogStartZ, 0xA8);
WWHD_OFFSET(BdkBaseTev, mFlags, 0xB4);
WWHD_OFFSET(BdkBaseTev, mHdLight1, 0xC0);
WWHD_OFFSET(BdkBaseTev, mHdLight2, 0x144);
WWHD_SIZE(BdkBaseTev, 0x1C8);
inline void BdkBase_initLight(BdkBaseLight &dst, const BdkBaseLight &src) {
    for (int i = 0; i < 6; ++i)
        dst.mPositionDirection[i] = src.mPositionDirection[i];
    for (int i = 0; i < 4; ++i)
        dst.mColor[i] = src.mColor[i];
    for (int i = 0; i < 4; ++i)
        dst.mShortParams[i] = src.mShortParams[i];
    for (int i = 0; i < 8; ++i)
        dst.mFloatParams[i] = src.mFloatParams[i];
}
inline void BdkBase_copyLight(BdkBaseLight &dst, const BdkBaseLight &src) {
    for (int i = 0; i < 6; ++i)
        BdkBase_copyFloat(dst.mPositionDirection[i], src.mPositionDirection[i]);
    for (int i = 0; i < 4; ++i)
        dst.mColor[i] = src.mColor[i];
    for (int i = 0; i < 4; ++i)
        dst.mShortParams[i] = src.mShortParams[i];
    for (int i = 0; i < 8; ++i)
        BdkBase_copyFloat(dst.mFloatParams[i], src.mFloatParams[i]);
}
inline void BdkBase_copyTev(dKy_tevstr_c *dst, const dKy_tevstr_c *src) {
    BdkBaseTev &d = *reinterpret_cast<BdkBaseTev *>(dst);
    const BdkBaseTev &s = *reinterpret_cast<const BdkBaseTev *>(src);
    BdkBase_copyLight(d.mLight, s.mLight);
    BdkBase_copyLight(d.mHdLight1, s.mHdLight1);
    BdkBase_copyLight(d.mHdLight2, s.mHdLight2);
    BdkBase_copyFloat(d.mLightPosWorld.x, s.mLightPosWorld.x);
    BdkBase_copyFloat(d.mLightPosWorld.y, s.mLightPosWorld.y);
    BdkBase_copyFloat(d.mLightPosWorld.z, s.mLightPosWorld.z);
    for (int i = 0; i < 4; ++i)
        d.mColorC0[i] = s.mColorC0[i];
    for (int i = 0; i < 8; ++i)
        d.mColorK[i] = s.mColorK[i];
    for (int i = 0; i < 4; ++i)
        d.mFogColor[i] = s.mFogColor[i];
    BdkBase_copyFloat(d.mFogStartZ, s.mFogStartZ);
    BdkBase_copyFloat(d.mFogEndZ, s.mFogEndZ);
    BdkBase_copyFloat(d.mColpatBlend, s.mColpatBlend);
    for (int i = 0; i < 9; ++i)
        d.mFlags[i] = s.mFlags[i];
}
namespace BdkBaseVtables {
enum : u32 { acchGroup = 0x10007EE8, acch = 0x10007EF8 };
}
namespace BdkBaseEntries {
// HD: the SDK array helpers and internal callback use guest entry addresses.
enum : u32 {
    sphereCtor = 0x025166F0,
    sphereDtor = 0x02515AE8,
    effectCtor = 0x0207260C,
    effectDtor = 0x0207267C,
    smokeCtorA = 0x02069948,
    smokeCtorB = 0x02069950,
    smokeDtor = 0x02072668,
    followCtor = 0x02069958,
    followDtor = 0x02072654,
    nodeCallBack = 0x02065794,
    moveBgTypical = 0x024EE658,
};
} // namespace BdkBaseEntries

enum BdkBaseConstants {
    dRes_INDEX_BDK_BDL_DK_e = 0x4A,
    dRes_INDEX_BDK_BCK_FLY1_e = 0x2A,
    dRes_INDEX_BDK_BAS_FLY1_e = 0xB,
    dRes_INDEX_BDK_BDL_DK_KAMEN4_e = 0x4B,
    dRes_INDEX_BDK_BDL_DK_TAIL_e = 0x4C,
    dRes_INDEX_BDK_BDL_GROCK00_e = 0x4E,
    dRes_INDEX_BDK_BDL_GHANE00_e = 0x4D,
    dRes_INDEX_BDK_BDL_S_TSHUTTER_e = 0x51,
    dRes_INDEX_BDK_DZB_S_TSHUTTER_e = 0x58,
    dRes_INDEX_BDK_BDL_S_TTOGE_e = 0x52,
    dRes_INDEX_BDK_DZB_S_TTOGE_e = 0x59,
    JA_BGM_MJ_TOWER_BATTLE = 0x80000015,
    AT_TYPE_UNK400000 = 0x400000,
    ACTION_AFTER_FIGHT = 0x6E,
    ACTION_T_FLY = 0x64,
    fpcNm_KAMOME_e = 0xC2,   // HD: process enumeration differs from GameCube.
    fpcNm_KUI_e = 0xFA,      // HD: process enumeration differs from GameCube.
    fpcNm_EP_e = 0xB9,       // HD: process enumeration differs from GameCube.
    fpcNm_DK_e = 0xA7,       // HD: process enumeration differs from GameCube.
    fpcNm_BDKOBJ_e = 0xEF,   // HD: process enumeration differs from GameCube.
    fpcNm_Obj_Tide_e = 0x27, // HD: process enumeration differs from GameCube.
    fpcNm_BK_e = 0xBD,       // HD: process enumeration differs from GameCube.
    fpcNm_BOKO_e = 0x1CF,    // HD: process enumeration differs from GameCube.
    BdkBaseProcess_DemoActor = 0x1BE,
};
u32 BdkBase_smokeCtor(void *, u8);
u32 BdkBase_followCtor(void *, u8, u8);
void BdkBase_sphereCtor(dCcD_Sph *);
void BdkBase_sphereDtor(dCcD_Sph *, s32);
void BdkBase_statusDtor(dCcD_Stts *, s32);
void BdkBase_acchDtor(dBgS_Acch *, s32);
void BdkBase_circleDtor(cM3dGCir_l *, s32);
void BdkBase_actorDtor(fopAc_ac_c *, s32);
void BdkBase_constructArray(void *, u32, u32, u32);
void BdkBase_destroyArray(void *, u32, u32, u32, u32, u32);
void BdkBase_registerObject(BdkBaseDestructorRecord *);
void BdkBase_firstHeaderObject(be<u8> *);
void BdkBase_secondHeaderObject(be<u8> *);
u32 BdkBase_createJntHit(J3DModel *, __jnt_hit_data_c *, s32);
void BdkBase_onDungeonItem(dSv_memBit_c *, s32);

struct BdkBaseFootPositions {
    cXyz mPosition[2];
};
struct BdkBaseMaskModels {
    be<u16> mModels[4];
};
struct BdkBaseJointHits {
    __jnt_hit_data_c mEntries[12];
};
WWHD_SIZE(BdkBaseFootPositions, 0x18);
WWHD_SIZE(BdkBaseMaskModels, 8);
WWHD_SIZE(BdkBaseJointHits, 0x90);

inline BOOL fopAcM_IsActor(void *actor) { return fopAc_IsActor(actor); }

enum BdkBaseJoints {
    DK_JNT_J_DK_KUBI1_e = 0x17,
    DK_JNT_J_DK_TOSAKA_A1_e = 0x1F,
    DK_JNT_J_DK_O_LA2_e = 0x3A,
    DK_JNT_J_DK_O_LB2_e = 0x3C,
    DK_JNT_J_DK_O_RA2_e = 0x3E,
    DK_JNT_J_DK_O_RB2_e = 0x40,
};
