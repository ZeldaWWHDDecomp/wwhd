// Valoo, ceiling rock and lava-pit layouts.
// HD: actor fields retain the GameCube offsets +0x11C; explicit vtables retain measured placement.
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
enum {
    dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e = 9,
    dRes_INDEX_DR2_BDL_IWA00_e = 0xE,
    dRes_INDEX_DR2_BMD_DR_e = 0x17,
    dRes_INDEX_DR2_BMD_DR_SIPPO_e = 0x18,
    dRes_INDEX_DR2_BMD_GAN_MAGMA_e = 0x1B,
    dRes_INDEX_DR2_BMD_MBYO1_e = 0x1C,
    dRes_INDEX_DR2_BMD_MBYO2_e = 0x1D,
    dRes_INDEX_DR2_BRK_MBYO2_e = 0x20,
    dRes_INDEX_DR2_BTK_GAN_MAGMA_e = 0x23,
    dRes_INDEX_DR2_BTK_MBYO1_e = 0x24,
    dRes_INDEX_DR2_BTK_MBYO2_e = 0x25,
    dRes_INDEX_DR2_DZB_MBYO1_e = 0x28,
    dRes_INDEX_DR2_DZB_MBYO2_e = 0x29,
};
enum { DR_JNT_J_DR_HANE_L2_e = 11 };
enum {
    JA_SE_CM_BTD_STN_FALL = 0x5828,
    JA_SE_CM_BTD_ROCK_FALL = 0x582A,
    JA_SE_CM_BTD_ROCK_HIT = 0x582B,
    JA_SE_CM_BTD_ROCK_ATTACH = 0x582F,
};
enum {
    dPa_name_ID_AK_SN_BTDDRIPMAGMA00 = 0x80B7,
    dPa_name_ID_AK_SN_BTDBROKENROCK00 = 0x80B9,
    dPa_name_ID_AK_SN_O_BTDBROKENROCKTAIL00 = 0x8062,
    dPa_name_ID_AK_ST_BTDSMOKE02 = 0xA0B8,
};
enum { fpcNm_BTD_e = 0xEA, fpcNm_KUI_e = 0xFA };


// HD: private measured views for material projection, animation functors and particles.
struct Dr2FrameFunctor {
    be<f32> mResult, mA, mB; u8 mPadding[4]; be<u32> mFunction, mArgument;
};
struct Dr2BtkAnimation {
    J3DFrameCtrl mFrameCtrl; gptr<Dr2FrameFunctor> mpFunctor;
    u8 mPadding[0x68-0x14]; gptr<be<f32>> mpAnimationFrame;
};
struct Dr2BrkAnimation {
    J3DFrameCtrl mFrameCtrl; gptr<be<f32>> mpAnimationFrame;
    u8 mPadding[0x20-0x14]; gptr<Dr2FrameFunctor> mpFunctor;
};
struct Dr2MaterialNameBlock {
    u8 mPadding[0x18]; be<s32> mNameOffset;
    JUTNameTab* getNames() {
        s32 offset = mNameOffset;
        return gabi::at<JUTNameTab>(offset != 0 ? gabi::ea(&mNameOffset) + offset : 0);
    }
};
struct Dr2ShapePacket { u8 mPadding[0x3C]; };
struct Dr2Model {
    u8 mPadding[0x14]; gptr<Dr2MaterialNameBlock> mpMaterialNames;
    u8 mPad18[0x34-0x18]; gptr<Dr2ShapePacket> mpShapePackets;
};
struct Dr2TextureMatrix {
    u8 mPadding[0x24]; be<u32> mProjection[11];
    be<f32> mTranslation, mRow0, mRow1, mRow3, mRow2;
    Mtx34 mEffectMatrix;
};
struct Dr2TexGenVtable { u8 mPadding[0x14]; be<u32> mGetTextureMatrix; };
struct Dr2TexGenBlock {
    gptr<Dr2TexGenVtable> mpVtable;
    Dr2TextureMatrix* getTextureMatrix(s32 index) {
        return gabi::call_ptr<Dr2TextureMatrix*>(mpVtable->mGetTextureMatrix, this, index);
    }
};
struct Dr2Material {
    u8 mPadding[0x14]; gptr<Dr2TexGenBlock> mpTexGen;
    u8 mRest[0x39C-0x18];
};
struct Dr2JointData { u8 mPadding[4]; be<u16> mJointNo; };
struct Dr2Particle { u8 mPadding[0x28]; cXyz mGlobalPosition; };
struct Dr2ParticleLink {
    gptr<Dr2Particle> mpObject; u8 mPadding[8]; gptr<Dr2ParticleLink> mpNext;
};
struct Dr2Emitter {
    u8 mPadding[0x34]; be<f32> mRate; u8 mPad38[0x24]; be<s32> mMaxFrame;
    u8 mPad60[0x1AC-0x60]; gptr<Dr2ParticleLink> mpFirst;
    u8 mPad1B0[4]; be<u32> mParticleCount; u8 mPad1B8[8]; be<u32> mChildCount;
    u8 mPad1C4[0x254-0x1C4]; be<u32> mFlags;
};
struct Dr2Btd {
    u8 mPadding[0x41E]; be<u8> m02F6; u8 mPad41F[5]; cXyz m02FC[9];
};
struct Dr2Player { u8 mPadding[0x3F0]; cXyz mLeftHandPosition; };
struct Dr2SmokeCallback {
    be<u32> mVtable; u8 mPadding[0xE]; be<u8> mFollowOff; u8 mRest[0xD];
};
struct Dr2SmokeVtable { u8 mPadding[0x44]; be<u32> mRemove; };
struct Dr2CreateAppend { be<u32> mParameter; u8 mPadding[0x1D]; be<s8> mRoomNo; };
WWHD_SIZE(Dr2FrameFunctor, 0x18);
WWHD_SIZE(Dr2BtkAnimation, 0x6C);
WWHD_SIZE(Dr2BrkAnimation, 0x24);
WWHD_SIZE(Dr2MaterialNameBlock, 0x1C);
WWHD_SIZE(Dr2ShapePacket, 0x3C);
WWHD_SIZE(Dr2Model, 0x38);
WWHD_SIZE(Dr2TextureMatrix, 0x94);
WWHD_SIZE(Dr2TexGenVtable, 0x18);
WWHD_SIZE(Dr2TexGenBlock, 4);
WWHD_SIZE(Dr2Material, 0x39C);
WWHD_SIZE(Dr2JointData, 6);
WWHD_SIZE(Dr2Particle, 0x34);
WWHD_SIZE(Dr2ParticleLink, 0x10);
WWHD_SIZE(Dr2Emitter, 0x258);
WWHD_SIZE(Dr2Btd, 0x490);
WWHD_SIZE(Dr2Player, 0x3FC);
WWHD_SIZE(Dr2SmokeCallback, 0x20);
WWHD_SIZE(Dr2SmokeVtable, 0x48);
WWHD_SIZE(Dr2CreateAppend, 0x22);
WWHD_OFFSET(Dr2TextureMatrix, mTranslation, 0x50);
WWHD_OFFSET(Dr2TextureMatrix, mEffectMatrix, 0x64);
WWHD_OFFSET(Dr2Emitter, mpFirst, 0x1AC);
WWHD_OFFSET(Dr2Emitter, mParticleCount, 0x1B4);
WWHD_OFFSET(Dr2Emitter, mChildCount, 0x1C0);
WWHD_OFFSET(Dr2Emitter, mFlags, 0x254);
WWHD_OFFSET(Dr2SmokeCallback, mFollowOff, 0x12);
WWHD_OFFSET(Dr2Btd, m02FC, 0x424);
/* daDr2_HIO_c (HD: vtable after the members) */
struct daDr2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<u32> __vtbl;
};
WWHD_SIZE(daDr2_HIO_c, 0xC);


struct iwa_hahen_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ be<u8> unk_04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ cXyz unk_08;
    /* 0x14 */ cXyz unk_14;
    /* 0x20 */ csXyz unk_20;
    /* 0x26 */ u8 _26[2];
};
WWHD_SIZE(iwa_hahen_s, 0x28);

/* GameCube +0x11C throughout */
struct dr2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf1;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0x3D8 */ be<s16> unk_2BC;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ iwa_hahen_s mRockFragments[6];
    /* 0x4CC */ cXyz unk_3B0;
    /* 0x4D8 */ be<s16> unk_3BC[12];
    /* 0x4F0 */ be<s16> unk_3D4[12];
    /* 0x508 */ be<f32> unk_3EC;
    /* 0x50C */ be<f32> unk_3F0;
    /* 0x510 */ be<f32> unk_3F4;
    /* 0x514 */ be<f32> unk_3F8;
    /* 0x518 */ be<u32> unk_3FC;
    /* 0x51C */ be<s16> unk_400[5];
    /* 0x526 */ be<u8> unk_40A;
    /* 0x527 */ u8 _527;
    /* 0x528 */ be<f32> unk_40C;
    /* 0x52C */ be<f32> unk_410;
    /* 0x530 */ be<f32> unk_414;
    /* 0x534 */ gptr<J3DModel> unk_418;
    /* 0x538 */ gptr<J3DModel> unk_41C;
    /* 0x53C */ gptr<void> unk_420; /* mDoExt_btkAnm* */
    /* 0x540 */ be<s16> unk_424;
    /* 0x542 */ u8 _542[2];
    /* 0x544 */ gptr<J3DModel> unk_428;
    /* 0x548 */ gptr<J3DModel> unk_42C;
    /* 0x54C */ gptr<void> unk_430; /* mDoExt_btkAnm* */
    /* 0x550 */ gptr<void> unk_434; /* mDoExt_btkAnm* */
    /* 0x554 */ gptr<void> unk_438; /* mDoExt_brkAnm* */
    /* 0x558 */ be<u8> unk_43C;
    /* 0x559 */ u8 _559[3];
    /* 0x55C */ Mtx34 unk_440;
    /* 0x58C */ Mtx34 unk_470;
    /* 0x5BC */ gptr<dBgW> mpBgW1;
    /* 0x5C0 */ gptr<dBgW> mpBgW2;
    /* 0x5C4 */ cXyz unk_4A8;
    /* 0x5D0 */ csXyz unk_4B4;
    /* 0x5D6 */ be<s16> unk_4BA;
    /* 0x5D8 */ u8 _5D8[4];
    /* 0x5DC */ be<f32> unk_4C0;
    /* 0x5E0 */ be<u8> unk_4C4;
    /* 0x5E1 */ u8 _5E1;
    /* 0x5E2 */ be<s16> unk_4C6;
    /* 0x5E4 */ be<s16> unk_4C8;
    /* 0x5E6 */ be<s16> unk_4CA;
    /* 0x5E8 */ be<s16> unk_4CC;
    /* 0x5EA */ be<s16> unk_4CE;
    /* 0x5EC */ be<f32> unk_4D0;
    /* 0x5F0 */ gptr<JPABaseEmitter> unk_4D4;
    /* 0x5F4 */ dPa_followEcallBack unk_4D8;
    /* 0x608 */ Dr2SmokeCallback unk_4EC; /* dPa_smokeEcallBack (HD 0x20, vtable at +0) */
    /* 0x628 */ be<u8> unk_50C;
    /* 0x629 */ be<u8> unk_50D;
    /* 0x62A */ be<u8> unk_50E;
    /* 0x62B */ u8 _62B;
    /* 0x62C */ be<s16> unk_510;
    /* 0x62E */ u8 _62E[2];
    /* 0x630 */ gptr<fopAc_ac_c> unk_514; /* btd_class* */
};
WWHD_OFFSET(dr2_class, mRockFragments, 0x3DC);
WWHD_OFFSET(dr2_class, unk_40A, 0x526);
WWHD_OFFSET(dr2_class, mpBgW1, 0x5BC);
WWHD_OFFSET(dr2_class, unk_4D8, 0x5F4);
WWHD_OFFSET(dr2_class, unk_50C, 0x628);
WWHD_SIZE(dr2_class, 0x634);


void dr2_rotateY(f32 radians, u8 concatenate);
void dr2_updateModel(J3DModel* model);
s16 dr2_radiansToAngle(f32 radians);
void dr2_inverse(const Mtx34* source, Mtx34* result);
void dr2_scale(Mtx34* matrix, f32 x, f32 y, f32 z);
void dr2_concat(const Mtx34* left, const Mtx34* right, Mtx34* result);
void dr2_brkEntry(void* animation, J3DModelData* data, f32 frame);
void dr2_btkEntry(void* animation, J3DModelData* data, f32 frame);
void dr2_shock(dVibration_c* vibration, s32 strength, s32 flags, cXyz* position);
void dr2_playAnimation(void* animation);
void dr2_finishFrame(gptr<Dr2FrameFunctor>* functor);
gptr<Mtx34>* dr2_shapeMatrixSlot(Dr2ShapePacket* packet, s32 index);
void* dr2_btkCtor(void* animation);
BOOL dr2_btkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry);
void* dr2_brkCtor(void* animation);
BOOL dr2_brkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry);
void dr2_smokeCtor(Dr2SmokeCallback* callback, u8 mode);
Dr2CreateAppend* dr2_createAppend();
u32 dr2_currentLayer();
u32 dr2_createRequest(u32 layer, s16 process, u32 callback, u32 data, Dr2CreateAppend* append);
void dr2_actorDtor(fopAc_ac_c* actor, s32 flags);

inline Dr2Material* dr2_materialBase(J3DModelData* data) {
    return gabi::at<Dr2Material>(data->mMaterials);
}
inline Mtx34* dr2_viewMatrix() { return gabi::at<Mtx34>(0x104B45F8); }

struct Dr2MoveBg { u8 mPadding[0xA8]; be<u32> mCorrectionFunction; };
WWHD_SIZE(Dr2MoveBg, 0xAC);
constexpr u32 DR2_MOVE_BG_TYPICAL = 0x024EE658;
inline void dr2_setMoveBgCorrection(dBgW* background) {
    reinterpret_cast<Dr2MoveBg*>(background)->mCorrectionFunction = DR2_MOVE_BG_TYPICAL;
}
inline void dr2_removeSmoke(Dr2SmokeCallback* callback) {
    gabi::call_ptr(gabi::at<Dr2SmokeVtable>(callback->mVtable)->mRemove, callback);
}

enum Dr2TailState { DR2_TAIL_IDLE, DR2_TAIL_PULLED, DR2_TAIL_GRABBED, DR2_TAIL_ROCK_SHAKE };
enum Dr2RockState {
    DR2_ROCK_RETURN = -1, DR2_ROCK_ATTACHED, DR2_ROCK_FALL, DR2_ROCK_RIDE_GOHMA,
    DR2_ROCK_RISE, DR2_ROCK_BREAK = 10, DR2_ROCK_GONE
};
