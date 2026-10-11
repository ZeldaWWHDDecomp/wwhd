/* nz_class (Rat), WWHD layout. 
 *
 * The GameCube header has no members (the actor is undecompiled there): the layout is from the WWHD
 * code. Derives from fopEn_enemy_c (+0x11C base). enemyice at 0xBE0, enemyfire (HD 0x22C) at 0xF98,
 * an 8-byte ice-material object at 0x11C4. Size 0x11CC (constructor 0230F190). */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "d/actor/d_a_pt.h" /* enemyice / enemyfire / smoke callback layouts */

struct NzParticleCallback { /* 0x00 */ be<u32> __vtbl; /* 0x04 */ gptr<JPABaseEmitter> mpEmitter; /* 0x08 */ u8 _08[0xC]; };
/* mDoExt_3DlineMat1_c (HD 0x188): vtable at +0x130, lines at +0x184 */
struct NzTailLine { gptr<cXyz> mpPositions; };
struct NzTailMaterial {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ gptr<NzTailLine> mpLines;   /* line array: +0 positions */
};
WWHD_SIZE(NzTailMaterial, 0x188);

struct nz_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> mArg0;
    /* 0x3D1 */ be<u8> mType;
    /* 0x3D2 */ be<u8> mbBombLit;
    /* 0x3D3 */ u8 _3D3[0x3D4 - 0x3D3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> mAction;
    /* 0x3D9 */ be<u8> mMode;
    /* 0x3DA */ be<u8> mStuckCounter;
    /* 0x3DB */ u8 _3DB[0x3DC - 0x3DB];
    /* 0x3DC */ be<u8> mSmokeTimer;
    /* 0x3DD */ be<u8> mbStole;
    /* 0x3DE */ be<u8> mbFrozen;
    /* 0x3DF */ be<u8> mbInWater;
    /* 0x3E0 */ be<u32> mItemId;
    /* 0x3E4 */ be<u32> mHoleId;
    /* 0x3E8 */ be<s16> mLookCount;
    /* 0x3EA */ be<s16> mSpinSpeed;
    /* 0x3EC */ be<s16> mSearchCounter;
    /* 0x3EE */ be<s16> mDeadCounter;
    /* 0x3F0 */ be<s16> mActionTimer;
    /* 0x3F2 */ be<s16> mTurnTimer;
    /* 0x3F4 */ be<s16> mLookTimer;
    /* 0x3F6 */ be<s16> mIdleTimer;
    /* 0x3F8 */ be<s16> mRippleTimer;
    /* 0x3FA */ u8 _3FA[0x3FE - 0x3FA];
    /* 0x3FE */ be<s16> mTailCounter;
    /* 0x400 */ be<u32> mBombRotation;
    /* 0x404 */ be<s32> mAnmIdx;
    /* 0x408 */ be<f32> mBodyOffset;
    /* 0x40C */ be<f32> mBodyOffsetTarget;
    /* 0x410 */ be<f32> mAttnOffsetY;
    /* 0x414 */ be<s16> mHeadRotX;
    /* 0x416 */ be<s16> mHeadRotY;
    /* 0x418 */ be<s16> mHeadRotZ;
    /* 0x41A */ u8 _41A[0x41C - 0x41A];
    /* 0x41C */ be<s16> mHeadRotYTarget;
    /* 0x41E */ u8 _41E[0x420 - 0x41E];
    /* 0x420 */ cXyz mRipplePos;
    /* 0x42C */ u8 _42C[0x438 - 0x42C];
    /* 0x438 */ cXyz mHitPos;
    /* 0x444 */ cXyz mHandPos0;
    /* 0x450 */ cXyz mHandPos1;
    /* 0x45C */ be<f32> mBgOffsetY;
    /* 0x460 */ u8 _460[0x478 - 0x460];
    /* 0x478 */ cXyz mProbeCenter;
    /* 0x484 */ cXyz mProbeEnd[8];      /* line-check ends (6 probes; [6], [7] debug copies) */
    /* 0x4E4 */ cXyz mProbeHit[8];      /* line-check hits */
    /* 0x544 */ be<s16> mWallAngle;
    /* 0x546 */ u8 _546;
    /* 0x547 */ be<u8> mbCarrying;
    /* 0x548 */ be<u8> mSearchResult;
    /* 0x549 */ u8 _549[0x54C - 0x549];
    /* 0x54C */ NzTailMaterial mLineMat;
    /* 0x6D4 */ cXyz mTailPos[10];
    /* 0x74C */ cXyz mTailVel[10];
    /* 0x7C4 */ cXyz mTailEnd[2];
    /* 0x7DC */ cXyz mTailStep;
    /* 0x7E8 */ be<s16> mMoveAngle;
    /* 0x7EA */ u8 _7EA[0x7EC - 0x7EA];
    /* 0x7EC */ cXyz mFollowPos;
    /* 0x7F8 */ csXyz mFollowAngle;
    /* 0x7FE */ u8 _7FE[0x800 - 0x7FE];
    /* 0x800 */ cXyz mSmokePos;
    /* 0x80C */ csXyz mSmokeAngle;
    /* 0x812 */ u8 _812[0x814 - 0x812];
    /* 0x814 */ NzParticleCallback mRippleCb;
    /* 0x828 */ dPa_smokeEcallBack_l mSmokeCb;
    /* 0x848 */ NzParticleCallback mFollowCb;
    /* 0x85C */ dBgS_AcchCir mAcchCir;
    /* 0x89C */ dBgS_ObjAcch mAcch;
    /* 0xA60 */ dCcD_Stts mStts;
    /* 0xA9C */ dCcD_Cyl mCyl;
    /* 0xBCC */ u8 _BCC[0x11];
    /* 0xBDD */ be<u8> mArg3;
    /* 0xBDE */ u8 _BDE[2];
    /* 0xBE0 */ enemyice mEnemyIce;
    /* 0xF98 */ enemyfire mEnemyFire;
    /* 0x11C4 */ u8 mIceMat[8];
};
WWHD_OFFSET(nz_class, mpMorf, 0x3D4);
WWHD_OFFSET(nz_class, mTailPos, 0x6D4);
WWHD_OFFSET(nz_class, mSmokeCb, 0x828);
WWHD_OFFSET(nz_class, mCyl, 0xA9C);
WWHD_OFFSET(nz_class, mEnemyIce, 0xBE0);
WWHD_OFFSET(nz_class, mEnemyFire, 0xF98);
WWHD_SIZE(nz_class, 0x11CC);

/* daNZ_HIO_c (0x18, vtable first) */
struct daNZ_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> mJumpSpeed;
    /* 0x08 */ be<f32> mAttentionHeight;
    /* 0x0C */ be<f32> mRunSpeed;
    /* 0x10 */ be<f32> mThrowSpeedY;
    /* 0x14 */ be<f32> mGravity;
};
WWHD_SIZE(daNZ_HIO_c, 0x18);

namespace NzHD {
struct JointData { u8 mPad[4]; be<u16> mJointNo; };
WWHD_OFFSET(JointData, mJointNo, 4);
inline JointData* jointData(J3DJoint* joint) { return reinterpret_cast<JointData*>(joint); }
inline J3DModel* currentModel() { return *gabi::at<gptr<J3DModel>>(0x104B462C); }
inline Mtx34* const currentMatrix = gabi::at<Mtx34>(0x104B4868);
struct AttackInfo { u8 mPad[0x10]; be<u32> mAttackType; };
WWHD_OFFSET(AttackInfo, mAttackType, 0x10);
inline AttackInfo* attackInfo(void* object) { return static_cast<AttackInfo*>(object); }
struct PlayerInfo { u8 mPad[0x3C0]; be<u32> mStateFlags; u8 mPad3C4[0x20]; cXyz mAttackCenter; };
WWHD_OFFSET(PlayerInfo, mStateFlags, 0x3C0);
WWHD_OFFSET(PlayerInfo, mAttackCenter, 0x3E4);
inline PlayerInfo* playerInfo(fopAc_ac_c* player) { return reinterpret_cast<PlayerInfo*>(player); }
inline NzParticleCallback* particleCallback(u32 address) { return gabi::at<NzParticleCallback>(address); }
}

// HD: actor-local bindings; argument widths and call targets are verified.
// 025F18EC [v d_a_nz]
inline void mDoMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
// 0201A478 [v d_a_nz]
inline csXyz* csXyz_ct_r(csXyz* p, s16 x, s16 y, s16 z) { return gabi::call<csXyz*>(0x0201A478, p, x, y, z); }
// 020CB678 [v d_a_nz]
inline BOOL daBomb_getBombCheck_Flag(void* b) { return gabi::call<BOOL>(0x020CB678, b); }
// 020CB6A8 [v d_a_nz]
inline void daBomb_setBombCheck_Flag(void* b) { gabi::call(0x020CB6A8, b); }
// 020CB6DC [v d_a_nz]
inline void daBomb_setBombFire_ON(void* b) { gabi::call(0x020CB6DC, b); }
// 020CB768 [v d_a_nz]
inline void daBomb_setBombOffCoSet(void* b) { gabi::call(0x020CB768, b); }
// 020CB7A0 [v d_a_nz]
inline void daBomb_setBombOnCoSet(void* b) { gabi::call(0x020CB7A0, b); }
// 021831AC [v d_a_nz]
inline BOOL daItem_checkLock(void* i) { return gabi::call<BOOL>(0x021831AC, i); }
// 02183204 [v d_a_nz]
inline void daItem_setLock(void* i) { gabi::call(0x02183204, i); }
// 0218319C [v d_a_nz]
inline void daItem_endControl(void* i) { gabi::call(0x0218319C, i); }
// 02183250 [v d_a_nz]
inline void daItem_releaseLock(void* i) { gabi::call(0x02183250, i); }
// 025192A8 [v d_a_nz]
inline void cc_at_check_nz(fopAc_ac_c* a, void* info) { gabi::call(0x025192A8, a, info); }
// 025E1AA4 [v d_a_nz]
inline void mDoAud_monsSeStart_nz(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
// 027F3F94 [v d_a_nz]
inline u16 getJointNum(nz_class* i_this) {
    /* 027F3F94 */
    u32 tree = gabi::call<u32>(0x027F3F94, getModelData(i_this->mpMorf->getModel()));
    return gabi::load<u16>(tree + 8);
}
// 024F0474 [v d_a_nz]
inline void objAcch_ct(dBgS_ObjAcch* acch) {
    gabi::call(0x024F0474, acch); /* dBgS_Acch::dBgS_Acch */
    u32 a = gabi::ea(acch);
    gabi::store<u32>(a + 0x10, 0x1002436C);
    gabi::store<u32>(a + 0x20, 0x1002437C);
    gabi::store<u32>(a + 0x14, 0x1002438C);
    gabi::store<u8>(a + 0x18, 1);
}
// 025ABBBC [v d_a_nz]
inline s32 nz_characterNameIndex(const char* name, s32 n) {
    return gabi::call<s32>(0x025ABBBC, gabi::at<u8>(dComIfGp_ea() + 0x50A0), name, n);
}
// 0230A3C0 [v d_a_nz]
inline void callTailControl(nz_class* i_this) { gabi::call(0x0230A3C0, i_this); }
// 02008254 [v d_a_nz]
inline BOOL dBgS_ChkPolySafe(void* poly) { return gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), poly); }
// 024EF0BC [v d_a_nz]
inline s32 dBgS_GetGroundCode(void* poly) { return gabi::call<s32>(0x024EF0BC, dComIfG_Bgsp(), poly); }
// 024EEE30 [v d_a_nz]
inline BOOL dBgS_ChkGrpInf(void* poly, u32 grp) { return gabi::call<BOOL>(0x024EEE30, dComIfG_Bgsp(), poly, grp); }
// 025602F0 [v d_a_nz]
inline void dKy_get_seacolor(GXColor* amb, GXColor* dif) { gabi::call(0x025602F0, amb, dif); }
// 0230B108 [v d_a_nz]
inline void nz_item_poi(nz_class* i_this) { gabi::call(0x0230B108, i_this); }
// 0230AC30 [v d_a_nz]
inline BOOL callFallLineCheck(nz_class* i_this) { return gabi::call<BOOL>(0x0230AC30, i_this); }
// 0230AB4C [v d_a_nz]
inline void callSmokeSet(nz_class* i_this) { gabi::call(0x0230AB4C, i_this); }
// 0230B738 [v d_a_nz]
inline void nz_anm_init(nz_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x0230B738, i_this, bck, morf, loop, speed, bas);
}
// 0230BA9C [v d_a_nz]
inline int callBodyHitCheck(nz_class* i_this) { return gabi::call<int>(0x0230BA9C, i_this); }
// 020CB8D8 [v d_a_nz]
inline u32 daBomb_prm_make(s32 a, s32 b, s32 c) { return gabi::call<u32>(0x020CB8D8, a, b, c); }
// 020CB648 [v d_a_nz]
inline s32 daBomb_getBombRestTime(void* b) { return gabi::call<s32>(0x020CB648, b); }
// 020CB860 [v d_a_nz]
inline void daBomb_setBombRestTime(void* b, s16 t) { gabi::call(0x020CB860, b, t); }
// 0230BD24 [v d_a_nz]
inline int callRatMove(nz_class* i_this, s16 a) { return gabi::call<int>(0x0230BD24, i_this, a); }
// 0230B320 [v d_a_nz]
inline fopAc_ac_c* callSearchObject(nz_class* i_this) { return gabi::call<fopAc_ac_c*>(0x0230B320, i_this); }
// 0230B864 [v d_a_nz]
inline int callSearchCheck(nz_class* i_this) { return gabi::call<int>(0x0230B864, i_this); }
// 0230F8BC [v d_a_nz]
inline void callWaterCheck(nz_class* i_this) { gabi::call(0x0230F8BC, i_this); }
// 0230FD20 [v d_a_nz]
inline void callBackgroundCheck(nz_class* i_this) { gabi::call(0x0230FD20, i_this); }
// 0230FD90 [v d_a_nz]
inline void callDamageMove(nz_class* i_this) { gabi::call(0x0230FD90, i_this); }
// 02310544 [v d_a_nz]
inline void callBombMove(nz_class* i_this) { gabi::call(0x02310544, i_this); }
// 0230F86C [v d_a_nz]
inline fopAc_ac_c* callSearchByID(u32 id) { return gabi::call<fopAc_ac_c*>(0x0230F86C, id); }
// 025D8870 [v d_a_nz]
inline fpc_ProcID fopAcM_createItem(cXyz* pos, s32 itemNo, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action, cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D8870, pos, itemNo, bitNo, roomNo, type, angle, action, scale);
}
// 02183150 [v d_a_nz]
inline void daItem_startControl(void* i) { gabi::call(0x02183150, i); }
// 024EECAC [v d_a_nz]
inline u32 dBgS_GetMtrlSndId(void* poly) { return gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), poly); }
// 02515BBC [v d_a_nz]
inline fopAc_ac_c* dCcD_GetAc(void* obj) { return gabi::call<fopAc_ac_c*>(0x02515BBC, obj); }

namespace NzHD {
// 02008E0C [v d_a_nz]
inline void constructGroundCheck(dBgS_GndChk_l* check) { gabi::call(0x02008E0C, check); }
// 02008DAC [v d_a_nz]
inline void destroyGroundCheck(dBgS_GndChk_l* check, s32 flags) { gabi::call(0x02008DAC, check, flags); }
// 025166F0 [v d_a_nz]
inline void constructCollisionSphere(dCcD_Sph* sphere) { gabi::call(0x025166F0, sphere); }
// 025EB82C [v d_a_nz]
inline void constructTailMaterial(NzTailMaterial* material) { gabi::call(0x025EB82C, material); }
// 025A9084 [v d_a_nz]
inline void constructRippleCallback(NzParticleCallback* callback) { gabi::call(0x025A9084, callback); }
// 025A5B18 [v d_a_nz]
inline void constructSmokeCallback(dPa_smokeEcallBack_l* callback, s32 mode) { gabi::call(0x025A5B18, callback, mode); }
// 025A5894 [v d_a_nz]
inline void constructFollowCallback(NzParticleCallback* callback, s32 first, s32 second) { gabi::call(0x025A5894, callback, first, second); }
// 024EFE94 [v d_a_nz]
inline void constructAcchCircle(dBgS_AcchCir* circle) { gabi::call(0x024EFE94, circle); }
// 025E895C [v d_a_nz]
inline void constructIceMaterial(void* material) { gabi::call(0x025E895C, material); }
// 025E89F8 [v d_a_nz]
inline void destroyIceMaterial(void* material, s32 flags) { gabi::call(0x025E89F8, material, flags); }
// 02515AE8 [v d_a_nz]
inline void destroyCollisionSphere(dCcD_Sph* sphere, s32 flags) { gabi::call(0x02515AE8, sphere, flags); }
// 024EFD9C [v d_a_nz]
inline void destroyAcch(u32 acch, s32 flags) { gabi::call(0x024EFD9C, acch, flags); }
// 02018034 [v d_a_nz]
inline void destroyCircleGeometry(u32 circle, s32 flags) { gabi::call(0x02018034, circle, flags); }
// 025EB8B8 [v d_a_nz]
inline void destroyTailMaterial(NzTailMaterial* material, s32 flags) { gabi::call(0x025EB8B8, material, flags); }
// 025D50BC [v d_a_nz]
inline void destroyActor(nz_class* actor, s32 flags) { gabi::call(0x025D50BC, actor, flags); }
// 025EBA58 [v d_a_nz]
inline BOOL initializeTailMaterial(NzTailMaterial* material, s32 lines, s32 segments, void* texture, s32 mode) { return gabi::call<BOOL>(0x025EBA58, material, lines, segments, texture, mode); }
// 025E8A48 [v d_a_nz]
inline BOOL initializeIceMaterial(void* material, J3DModel* model) { return gabi::call<BOOL>(0x025E8A48, material, model); }
// 023111D4 [v d_a_nz]
inline void assureMaterialName(SafeString* name) { gabi::call(0x023111D4, name); }
// 027DF9B0 [v d_a_nz]
inline s32 lookupMaterialName(u32 table, u32 name) { return gabi::call<s32>(0x027DF9B0, table, name); }
// 0259138C [v d_a_nz]
inline void drawIceMaterial(mDoExt_McaMorf* morf, s32 index, void* material) { gabi::call(0x0259138C, morf, index, material); }
// 027F583C [v d_a_nz]
inline void drawJoint(J3DModel* model, u32 joint) { gabi::call(0x027F583C, model, joint); }
// 025EC62C [v d_a_nz]
inline void updateTailMaterial(NzTailMaterial* material, s32 segments, f32 width, u32 color, s32 type, dKy_tevstr_c* light) { gabi::call(0x025EC62C, material, segments, width, color, type, light); }
// 025EDD04 [v d_a_nz]
inline void sortTailMaterial(u32 packet, NzTailMaterial* material) { gabi::call(0x025EDD04, packet, material); }
// 028ED6F8 [v d_a_nz]
inline void initializeHeaderGuard(u32 guard) { gabi::call(0x028ED6F8, guard); }
// 028EAB2C [v d_a_nz]
inline void initializeHeaderFlag(u32 flag) { gabi::call(0x028EAB2C, flag); }
// 0230B320 [v d_a_nz]
inline fopAc_ac_c* searchObject(nz_class* actor) { return gabi::call<fopAc_ac_c*>(0x0230B320, actor); }
}

namespace NzHD {
// HD: particle members used by the smoke and water surface effects.
struct Emitter {
    u8 mPad[0x34]; be<f32> mRate; u8 mPad38[0x20];
    be<f32> mSpread; be<u32> mMaxFrame; u8 mPad60[0x10];
    be<f32> mInitialVelOmni; u8 mPad74[0x220-0x74];
    cXyz mGlobalScale; u8 mPad22C[0xC]; cXyz mGlobalParticleScale; GXColor mGlobalPrmColor;
};
WWHD_OFFSET(Emitter, mRate, 0x34);
WWHD_OFFSET(Emitter, mSpread, 0x58);
WWHD_OFFSET(Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Emitter, mInitialVelOmni, 0x70);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Emitter, mGlobalPrmColor, 0x244);
inline Emitter* emitter(u32 address) { return gabi::at<Emitter>(address); }
struct RemoveVtable { u8 mPad[0x44]; be<u32> mRemove; };
WWHD_OFFSET(RemoveVtable, mRemove, 0x44);
inline void removeParticle(void* callback, u32 vtable) {
    gabi::call_ptr(gabi::at<RemoveVtable>(vtable)->mRemove, callback);
}
struct TailVtable { u8 mPad[0x14]; be<u32> mMaterialId; };
WWHD_OFFSET(TailVtable, mMaterialId, 0x14);
inline s32 tailMaterialId(NzTailMaterial* material) {
    return gabi::call_ptr<s32>(gabi::at<TailVtable>(material->__vtbl)->mMaterialId, material);
}
struct ResourceHeader { u8 mPad[0x18]; be<u32> mMaterialNameOffset; };
WWHD_OFFSET(ResourceHeader, mMaterialNameOffset, 0x18);
struct DrawBuffers { gptr<J3DDrawBuffer> mpOpaque; gptr<J3DDrawBuffer> mpTranslucent; };
inline DrawBuffers& drawBuffers() { return *gabi::at<DrawBuffers>(0x104B4634); }
struct PlayDrawLists { u8 mPad[0x5D78]; DrawBuffers mNormal; u8 mPad5D80[4]; DrawBuffers mEyes; };
WWHD_OFFSET(PlayDrawLists, mNormal, 0x5D78);
WWHD_OFFSET(PlayDrawLists, mEyes, 0x5D84);
inline PlayDrawLists* playDrawLists() { return gabi::at<PlayDrawLists>(dComIfGp_ea()); }
struct HoleInfo { u8 mPad[0x3DA]; be<s16> mSpawnCount; be<s16> mReturnedCount; be<s16> mReturnTimer; };
WWHD_OFFSET(HoleInfo, mSpawnCount, 0x3DA);
WWHD_OFFSET(HoleInfo, mReturnedCount, 0x3DC);
WWHD_OFFSET(HoleInfo, mReturnTimer, 0x3DE);
inline HoleInfo* holeInfo(fopAc_ac_c* actor) { return reinterpret_cast<HoleInfo*>(actor); }
struct AcchVtables { u8 mPad[0x14]; be<u32> mObjectVtable; u8 mPad18[8]; be<u32> mCheckVtable; };
WWHD_OFFSET(AcchVtables, mObjectVtable, 0x14);
WWHD_OFFSET(AcchVtables, mCheckVtable, 0x20);
inline AcchVtables* acchVtables(u32 address) { return gabi::at<AcchVtables>(address); }
struct VectorWords { be<u32> x, y, z; };
inline VectorWords* words(cXyz* vector) { return reinterpret_cast<VectorWords*>(vector); }
struct HeaderState { be<f32> mPiMin; be<f32> mPiMax; u8 mGuard; u8 mFlag; u8 mPad[0x1A]; be<u32> mZero[4]; };
WWHD_OFFSET(HeaderState, mZero, 0x24);
inline HeaderState& headerState() { return *gabi::at<HeaderState>(0x10468CE8); }
}

namespace NzHD {
struct LinePolygonView { u8 mPad[0x14]; cBgS_PolyInfo mPolygon; };
WWHD_OFFSET(LinePolygonView, mPolygon, 0x14);
inline cBgS_PolyInfo* linePolygon(dBgS_LinChk_l* line) { return &reinterpret_cast<LinePolygonView*>(line)->mPolygon; }
}

enum NzAction {
    NZ_ACTION_CRAWL = 0,
    NZ_ACTION_JUMP = 1,
    NZ_ACTION_DROP = 2,
    NZ_ACTION_CARRY = 3,
    NZ_ACTION_DAMAGE = 4,
    NZ_ACTION_BOMB = 5,
};
enum NzMode {
    NZ_MODE_WAIT_INIT = 0,
    NZ_MODE_WAIT = 1,
    NZ_MODE_RUN_INIT = 2,
    NZ_MODE_RUN = 3,
    NZ_MODE_WATCH_INIT = 4,
    NZ_MODE_WATCH = 5,
    NZ_MODE_FLEE_INIT = 6,
    NZ_MODE_FLEE = 7,
    NZ_MODE_DROP_INIT = 10,
    NZ_MODE_DROP = 11,
    NZ_MODE_LAND = 12,
    NZ_MODE_JUMP_INIT = 20,
    NZ_MODE_JUMP = 21,
    NZ_MODE_BOUNCE_INIT = 22,
    NZ_MODE_BOUNCE = 23,
    NZ_MODE_CARRY_INIT = 30,
    NZ_MODE_PICKUP_INIT = 31,
    NZ_MODE_PICKUP = 32,
    NZ_MODE_CARRY_START = 33,
    NZ_MODE_CARRY_RUN = 34,
    NZ_MODE_HOLE_ENTER = 35,
    NZ_MODE_THROW_INIT = 36,
    NZ_MODE_THROW = 37,
    NZ_MODE_THROW_WAIT = 38,
    NZ_MODE_RECOVER_INIT = 39,
    NZ_MODE_RECOVER = 40,
    NZ_MODE_RETREAT = 41,
    NZ_MODE_DAMAGE_INIT = 50,
    NZ_MODE_DAMAGE_FALL = 51,
    NZ_MODE_DAMAGE_LAND_INIT = 52,
    NZ_MODE_DAMAGE_LAND = 53,
    NZ_MODE_DAMAGE_BOUNCE = 54,
    NZ_MODE_DAMAGE_BOUNCE_INIT = 55,
    NZ_MODE_DAMAGE_FINAL_BOUNCE = 56,
    NZ_MODE_DEAD = 57,
    NZ_MODE_SPIN_DAMAGE_INIT = 60,
    NZ_MODE_SPIN_DAMAGE_FALL = 61,
    NZ_MODE_SPIN_DAMAGE_LAND = 62,
    NZ_MODE_BOMB_INIT = 70,
    NZ_MODE_BOMB_SPAWN = 71,
    NZ_MODE_BOMB_WANDER = 72,
    NZ_MODE_BOMB_LIGHT_INIT = 73,
    NZ_MODE_BOMB_LIGHT = 74,
    NZ_MODE_BOMB_CHASE = 75,
    NZ_MODE_BOMB_FACE_PLAYER = 76,
    NZ_MODE_BOMB_RETURN_INIT = 77,
    NZ_MODE_BOMB_RETURN = 78,
};

WWHD_OFFSET(nz_class, mAction, 0x3D8);
WWHD_OFFSET(nz_class, mMode, 0x3D9);
WWHD_OFFSET(nz_class, mActionTimer, 0x3F0);
WWHD_OFFSET(nz_class, mTurnTimer, 0x3F2);
WWHD_OFFSET(nz_class, mLookTimer, 0x3F4);
WWHD_OFFSET(nz_class, mIdleTimer, 0x3F6);
WWHD_OFFSET(nz_class, mBombRotation, 0x400);
WWHD_OFFSET(nz_class, mbCarrying, 0x547);
WWHD_OFFSET(nz_class, mSearchResult, 0x548);
WWHD_OFFSET(nz_class, mProbeCenter, 0x478);
WWHD_SIZE(NzParticleCallback, 0x14);
WWHD_SIZE(NzTailLine, 4);

// HD: resource indices retained from the verified Nz archive.
enum NzAnimation {
    NZ_BCK_JUMP = 0x15,
    NZ_BCK_LAND = 0x16,
    NZ_BCK_RETREAT = 0x17,
    NZ_BCK_DAMAGE = 0x18,
    NZ_BCK_DAMAGE_BOUNCE = 0x19,
    NZ_BCK_THROW = 0x1C,
    NZ_BCK_RECOVER = 0x1D,
    NZ_BCK_PICKUP = 0x1E,
    NZ_BCK_RUN = 0x1F,
    NZ_BCK_CARRY_RUN = 0x20,
    NZ_BCK_BOMB_LIGHT_START = 0x21,
    NZ_BCK_BOMB_LIGHT = 0x22,
    NZ_BCK_WAIT = 0x23,
};
