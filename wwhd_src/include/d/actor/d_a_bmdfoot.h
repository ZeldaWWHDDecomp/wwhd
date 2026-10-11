/* bmdfoot_class (Kalle Demos floor tentacle), WWHD layout. 
 *
 * GameCube members +0x11C up to mTevstr; dKy_tevstr_c is 0x1C8 in HD (GameCube 0xB0), so the
 * members after it move by a further +0x118 (size 0xE08, GameCube 0xBD4). */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

struct bmdfoot_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyVineMorf;
    /* 0x3D4 */ be<s16> m2B8;
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0[4];
    /* 0x3E4 */ be<s16> m2C8[2];
    /* 0x3E8 */ cXyz m2CC[18];
    /* 0x4C0 */ be<f32> m3A4[17]; /* GameCube m3A4[16]; the loops write [2..16] */
    /* 0x504 */ u8 _504[0x508 - 0x504];
    /* 0x508 */ be<s16> m3EC;
    /* 0x50A */ u8 _50A[0x510 - 0x50A];
    /* 0x510 */ be<u8> m3F4;
    /* 0x511 */ u8 _511[3];
    /* 0x514 */ cXyz m3F8[3];
    /* 0x538 */ dPa_followEcallBack mAsiWaitFollowCB[3];
    /* 0x574 */ dPa_smokeEcallBack mLAttackSmoke00CB[2];
    /* 0x5B4 */ cXyz m498[2];
    /* 0x5CC */ dPa_smokeEcallBack mLAttackSmoke01CB;
    /* 0x5EC */ dCcD_Stts mStts;
    /* 0x628 */ dCcD_Sph mSph[5];
    /* 0xC04 */ cXyz mAE8;
    /* 0xC10 */ gptr<mDoExt_McaMorf> mpFloorVineMorf;
    /* 0xC14 */ dKy_tevstr_c mTevstr;
    /* 0xDDC */ be<u8> mBA8;
    /* 0xDDD */ u8 _DDD[3];
    /* 0xDE0 */ cXyz mBAC;
    /* 0xDEC */ csXyz mBB8;
    /* 0xDF2 */ u8 _DF2[2];
    /* 0xDF4 */ be<f32> mBC0;
    /* 0xDF8 */ be<f32> mBC4;
    /* 0xDFC */ be<u8> mBC8;
    /* 0xDFD */ u8 _DFD[3];
    /* 0xE00 */ gptr<mDoExt_btkAnm> btk;
    /* 0xE04 */ be<s16> mBD0;
    /* 0xE06 */ be<u8> mBD2;
    /* 0xE07 */ u8 _E07;
};
WWHD_OFFSET(bmdfoot_class, m3F8, 0x514);
WWHD_OFFSET(bmdfoot_class, mStts, 0x5EC);
WWHD_OFFSET(bmdfoot_class, mSph, 0x628);
WWHD_OFFSET(bmdfoot_class, mAE8, 0xC04);
WWHD_OFFSET(bmdfoot_class, mTevstr, 0xC14);
WWHD_OFFSET(bmdfoot_class, mBA8, 0xDDC);
WWHD_OFFSET(bmdfoot_class, btk, 0xE00);
WWHD_SIZE(bmdfoot_class, 0xE08);

/* daBmdfoot_HIO_c (HD: vtable after the members, size 0xC) */
struct daBmdfoot_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<s8> m05;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<u32> __vtbl;
};
WWHD_SIZE(daBmdfoot_HIO_c, 0xC);

// HD: boss fields used by this unit. Unnamed members retain their tww names.
struct BmdfootBoss : fopAc_ac_c {
    u8 _3AC[0x448 - 0x3AC];
    be<f32> m328;
    u8 _44C[5];
    be<s8> m331;
    be<s8> m332;
    u8 _453;
    be<s16> m334;
    u8 _456[0xC96 - 0x456];
    be<s16> mB76;
};
WWHD_OFFSET(BmdfootBoss, m328, 0x448);
WWHD_OFFSET(BmdfootBoss, m334, 0x454);
WWHD_OFFSET(BmdfootBoss, mB76, 0xC96);
struct BmdfootThreeJoints {
    be<s32> values[3];
};
struct BmdfootFiveJoints {
    be<s32> values[5];
};
struct BmdfootFourJoints {
    be<s32> values[4];
};
struct BmdfootArchiveName {
    char name[8];
};

// HD: light blocks include four short factors and eight float parameters.
struct BmdfootLight {
    cXyz position;
    cXyz direction;
    be<u8> color[4];
    be<u16> factors[4];
    be<f32> parameters[8];
    u8 _44[0x84 - 0x44];
};
WWHD_SIZE(BmdfootLight, 0x84);
struct BmdfootTev {
    BmdfootLight light;
    cXyz mLightPosWorld;
    GXColorS10 mColorC0;
    GXColor mColorK0;
    GXColor mColorK1;
    GXColorS10 mFogColor;
    be<f32> mFogStartZ, mFogEndZ, mColpatBlend;
    be<u8> mInitTimer, mEnvrIdxCurr, mEnvrIdxPrev, mColpatCurr, mColpatPrev;
    be<s8> mRoomNo;
    be<u8> mEnvrIdxOverride, mLightMode, mInitType;
    u8 _BD[3];
    BmdfootLight lightCopy1;
    BmdfootLight lightCopy2;
};
WWHD_OFFSET(BmdfootTev, mLightPosWorld, 0x84);
WWHD_OFFSET(BmdfootTev, lightCopy1, 0xC0);
WWHD_OFFSET(BmdfootTev, lightCopy2, 0x144);
WWHD_SIZE(BmdfootTev, 0x1C8);
inline BmdfootTev& Bmdfoot_tev(dKy_tevstr_c& tev) { return reinterpret_cast<BmdfootTev&>(tev); }
template <class T> inline void Bmdfoot_copyWord(T& dst, const T& src) {
    static_assert(sizeof(T) == 4);
    reinterpret_cast<be<u32>&>(dst) = reinterpret_cast<const be<u32>&>(src);
}

// HD: preserve the read of the matrix pointer before updating the dirty flag.
inline Mtx34* Bmdfoot_getAnmMtx(J3DModel* model, s32 joint) {
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    u32 matrices = block->mpAnmMtx;
    block->mFlags |= 0x10;
    return gabi::at<Mtx34>(matrices + (u32)joint * sizeof(Mtx34));
}
struct BmdfootJointNumber {
    u8 _00[4];
    be<u16> number;
};
inline u16 Bmdfoot_jointNumber(J3DJoint* joint) {
    return reinterpret_cast<BmdfootJointNumber*>(joint)->number;
}
struct BmdfootCallbackVtable {
    u8 _00[0x44];
    be<u32> remove;
};
template <class T> inline void Bmdfoot_remove(T* callback) {
    auto* vtable = gabi::at<BmdfootCallbackVtable>(callback->__vtbl);
    gabi::call_ptr(vtable->remove, callback);
}
// HD: add the status offset in guest space to retain null-plus-offset and wrapping.
inline be<u32>& Bmdfoot_emitterFlags(JPABaseEmitter* emitter) {
    return *gabi::at<be<u32>>(gabi::ea(emitter) + 0x254);
}
inline void emitter_playCreate(JPABaseEmitter* emitter) { Bmdfoot_emitterFlags(emitter) &= ~1u; }
inline void emitter_stopCreate(JPABaseEmitter* emitter) { Bmdfoot_emitterFlags(emitter) |= 1u; }
inline BOOL fopAcM_IsActor(void* actor) { return fopAc_IsActor(actor); }
void Bmdfoot_constructArray(void*, u32, u32, u32);
void Bmdfoot_destroyArray(void*, u32, u32, u32, u32, u32);
u32 Bmdfoot_smokeCtor(dPa_smokeEcallBack*, u8);
void Bmdfoot_statusCtor(dCcD_Stts*);
void Bmdfoot_globalStatusCtor(void*);
void Bmdfoot_statusDtor(dCcD_Stts*, s32);
void Bmdfoot_actorDtor(fopAc_ac_c*, s32);
mDoExt_btkAnm* Bmdfoot_btkCtor(mDoExt_btkAnm*);

// HD: GHS callback and vtable copies belonging to this unit.
enum BmdfootEntry : u32 {
    BmdfootEntry_FollowCtor = 0x020B7D80,
    BmdfootEntry_SmokeCtor = 0x020B7D8C,
    BmdfootEntry_FollowDtor = 0x020B8564,
    BmdfootEntry_SmokeDtor = 0x020B8578,
    BmdfootEntry_SphereCtor = 0x025166F0,
    BmdfootEntry_SphereDtor = 0x02515AE8,
    BmdfootVtable_Actor = 0x10009DBC,
    BmdfootVtable_Hio = 0x10009DAC,
    BmdfootVtable_Status = 0x1004AE88,
    BmdfootVtable_GlobalStatus = 0x1004AEC0,
};

// HD: exact member-copy scheduling of the generated constructor.
inline void Bmdfoot_constructTev(BmdfootTev& dst, const BmdfootLight& src) {
    dst.light.position.x = src.position.x;
    dst.light.position.y = src.position.y;
    dst.light.position.z = src.position.z;
    dst.light.direction.x = src.direction.x;
    dst.light.direction.y = src.direction.y;
    dst.light.direction.z = src.direction.z;
    dst.light.color[0] = src.color[0];
    dst.light.color[1] = src.color[1];
    dst.light.color[2] = src.color[2];
    dst.light.color[3] = src.color[3];
    dst.light.factors[0] = src.factors[0];
    dst.light.factors[1] = src.factors[1];
    dst.light.factors[2] = src.factors[2];
    dst.light.factors[3] = src.factors[3];
    dst.light.parameters[0] = src.parameters[0];
    dst.light.parameters[1] = src.parameters[1];
    dst.light.parameters[2] = src.parameters[2];
    dst.light.parameters[3] = src.parameters[3];
    dst.light.parameters[4] = src.parameters[4];
    dst.light.parameters[5] = src.parameters[5];
    dst.light.parameters[6] = src.parameters[6];
    dst.lightCopy2.direction.y = src.direction.y;
    dst.lightCopy1.parameters[0] = src.parameters[0];
    dst.lightCopy2.position.y = src.position.y;
    dst.lightCopy1.parameters[2] = src.parameters[2];
    dst.lightCopy1.direction.z = src.direction.z;
    dst.lightCopy1.color[3] = src.color[3];
    dst.lightCopy2.factors[0] = src.factors[0];
    dst.lightCopy1.direction.y = src.direction.y;
    dst.lightCopy2.color[1] = src.color[1];
    dst.lightCopy2.factors[1] = src.factors[1];
    dst.lightCopy1.color[1] = src.color[1];
    dst.lightCopy1.position.x = src.position.x;
    dst.lightCopy2.color[0] = src.color[0];
    dst.lightCopy2.position.x = src.position.x;
    dst.lightCopy1.direction.x = src.direction.x;
    dst.lightCopy1.color[0] = src.color[0];
    dst.lightCopy1.factors[1] = src.factors[1];
    dst.lightCopy1.position.z = src.position.z;
    dst.lightCopy2.position.z = src.position.z;
    dst.lightCopy1.parameters[3] = src.parameters[3];
    dst.lightCopy2.direction.z = src.direction.z;
    dst.lightCopy1.color[2] = src.color[2];
    dst.lightCopy2.color[3] = src.color[3];
    dst.lightCopy2.color[2] = src.color[2];
    dst.lightCopy1.factors[2] = src.factors[2];
    dst.lightCopy2.factors[2] = src.factors[2];
    dst.lightCopy1.factors[0] = src.factors[0];
    dst.light.parameters[7] = src.parameters[7];
    dst.lightCopy2.direction.x = src.direction.x;
    dst.lightCopy1.parameters[7] = src.parameters[7];
    dst.lightCopy1.factors[3] = src.factors[3];
    dst.lightCopy1.parameters[4] = src.parameters[4];
    dst.lightCopy1.parameters[1] = src.parameters[1];
    dst.lightCopy1.parameters[5] = src.parameters[5];
    dst.lightCopy2.factors[3] = src.factors[3];
    dst.lightCopy1.parameters[6] = src.parameters[6];
    dst.lightCopy1.position.y = src.position.y;
    dst.lightCopy2.parameters[0] = src.parameters[0];
    dst.lightCopy2.parameters[1] = src.parameters[1];
    dst.lightCopy2.parameters[2] = src.parameters[2];
    dst.lightCopy2.parameters[3] = src.parameters[3];
    dst.lightCopy2.parameters[4] = src.parameters[4];
    dst.lightCopy2.parameters[5] = src.parameters[5];
    dst.lightCopy2.parameters[6] = src.parameters[6];
    dst.lightCopy2.parameters[7] = src.parameters[7];
}

// HD: exact member-copy scheduling of the generated assignment.
inline void Bmdfoot_copyTev(BmdfootTev& dst, const BmdfootTev& src) {
    dst.light.position.x = src.light.position.x;
    dst.light.position.y = src.light.position.y;
    dst.light.position.z = src.light.position.z;
    dst.light.color[0] = src.light.color[0];
    dst.light.color[1] = src.light.color[1];
    dst.light.color[2] = src.light.color[2];
    dst.light.color[3] = src.light.color[3];
    dst.light.direction.x = src.light.direction.x;
    dst.light.direction.y = src.light.direction.y;
    dst.light.direction.z = src.light.direction.z;
    dst.light.parameters[0] = src.light.parameters[0];
    dst.light.parameters[1] = src.light.parameters[1];
    dst.light.factors[0] = src.light.factors[0];
    dst.light.parameters[2] = src.light.parameters[2];
    dst.light.parameters[3] = src.light.parameters[3];
    dst.light.parameters[4] = src.light.parameters[4];
    dst.light.factors[1] = src.light.factors[1];
    dst.light.factors[2] = src.light.factors[2];
    dst.light.parameters[5] = src.light.parameters[5];
    dst.light.factors[3] = src.light.factors[3];
    dst.light.parameters[6] = src.light.parameters[6];
    dst.light.parameters[7] = src.light.parameters[7];
    dst.mColorC0.r = src.mColorC0.r;
    Bmdfoot_copyWord(dst.mLightPosWorld.x, src.mLightPosWorld.x);
    Bmdfoot_copyWord(dst.mLightPosWorld.y, src.mLightPosWorld.y);
    dst.mColorC0.g = src.mColorC0.g;
    Bmdfoot_copyWord(dst.mLightPosWorld.z, src.mLightPosWorld.z);
    dst.mColorC0.b = src.mColorC0.b;
    dst.mColorC0.a = src.mColorC0.a;
    Bmdfoot_copyWord(dst.mColorK0, src.mColorK0);
    Bmdfoot_copyWord(dst.mColorK1, src.mColorK1);
    dst.mInitTimer = src.mInitTimer;
    dst.mFogColor.r = src.mFogColor.r;
    dst.mFogColor.g = src.mFogColor.g;
    dst.mFogColor.b = src.mFogColor.b;
    dst.mEnvrIdxCurr = src.mEnvrIdxCurr;
    dst.mEnvrIdxPrev = src.mEnvrIdxPrev;
    dst.mColpatCurr = src.mColpatCurr;
    dst.mFogStartZ = src.mFogStartZ;
    dst.mFogEndZ = src.mFogEndZ;
    dst.mFogColor.a = src.mFogColor.a;
    dst.mColpatBlend = src.mColpatBlend;
    dst.lightCopy1.position.x = src.lightCopy1.position.x;
    dst.lightCopy1.position.y = src.lightCopy1.position.y;
    dst.lightCopy1.position.z = src.lightCopy1.position.z;
    dst.mColpatPrev = src.mColpatPrev;
    dst.mRoomNo = src.mRoomNo;
    dst.mEnvrIdxOverride = src.mEnvrIdxOverride;
    dst.mLightMode = src.mLightMode;
    dst.mInitType = src.mInitType;
    dst.lightCopy1.color[0] = src.lightCopy1.color[0];
    dst.lightCopy1.color[1] = src.lightCopy1.color[1];
    dst.lightCopy1.color[2] = src.lightCopy1.color[2];
    dst.lightCopy1.color[3] = src.lightCopy1.color[3];
    dst.lightCopy1.direction.x = src.lightCopy1.direction.x;
    dst.lightCopy1.direction.y = src.lightCopy1.direction.y;
    dst.lightCopy1.direction.z = src.lightCopy1.direction.z;
    dst.lightCopy1.parameters[0] = src.lightCopy1.parameters[0];
    dst.lightCopy1.parameters[1] = src.lightCopy1.parameters[1];
    dst.lightCopy1.parameters[2] = src.lightCopy1.parameters[2];
    dst.lightCopy1.parameters[3] = src.lightCopy1.parameters[3];
    dst.lightCopy1.parameters[4] = src.lightCopy1.parameters[4];
    dst.lightCopy1.parameters[5] = src.lightCopy1.parameters[5];
    dst.lightCopy1.parameters[6] = src.lightCopy1.parameters[6];
    dst.lightCopy1.factors[0] = src.lightCopy1.factors[0];
    dst.lightCopy1.parameters[7] = src.lightCopy1.parameters[7];
    dst.lightCopy1.factors[1] = src.lightCopy1.factors[1];
    dst.lightCopy2.position.x = src.lightCopy2.position.x;
    dst.lightCopy1.factors[2] = src.lightCopy1.factors[2];
    dst.lightCopy2.position.y = src.lightCopy2.position.y;
    dst.lightCopy1.factors[3] = src.lightCopy1.factors[3];
    dst.lightCopy2.position.z = src.lightCopy2.position.z;
    dst.lightCopy2.factors[0] = src.lightCopy2.factors[0];
    dst.lightCopy2.direction.x = src.lightCopy2.direction.x;
    dst.lightCopy2.direction.y = src.lightCopy2.direction.y;
    dst.lightCopy2.color[0] = src.lightCopy2.color[0];
    dst.lightCopy2.direction.z = src.lightCopy2.direction.z;
    dst.lightCopy2.color[1] = src.lightCopy2.color[1];
    dst.lightCopy2.color[2] = src.lightCopy2.color[2];
    dst.lightCopy2.parameters[0] = src.lightCopy2.parameters[0];
    dst.lightCopy2.parameters[1] = src.lightCopy2.parameters[1];
    dst.lightCopy2.color[3] = src.lightCopy2.color[3];
    dst.lightCopy2.parameters[2] = src.lightCopy2.parameters[2];
    dst.lightCopy2.parameters[3] = src.lightCopy2.parameters[3];
    dst.lightCopy2.factors[1] = src.lightCopy2.factors[1];
    dst.lightCopy2.parameters[4] = src.lightCopy2.parameters[4];
    dst.lightCopy2.parameters[5] = src.lightCopy2.parameters[5];
    dst.lightCopy2.factors[2] = src.lightCopy2.factors[2];
    dst.lightCopy2.parameters[6] = src.lightCopy2.parameters[6];
    dst.lightCopy2.factors[3] = src.lightCopy2.factors[3];
    dst.lightCopy2.parameters[7] = src.lightCopy2.parameters[7];
}

inline void Bmdfoot_smokeEnd(dPa_smokeEcallBack* smoke) { smoke->end(); }
inline J3DJoint* Bmdfoot_jointNode(J3DModelData* data, u16 index) {
    u32 node = data->mJointNodes;
    if (index < data->mJointNum)
        node += (u32)index * 0x1C;
    return gabi::at<J3DJoint>(node);
}
