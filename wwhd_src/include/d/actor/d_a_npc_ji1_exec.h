/* Orca execution: source-private HD views. */
#pragma once
#define dComIfGs_isEventBit ji1ExecLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1ExecLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1ExecLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1ExecLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
using Ji1ExecWordHD = be<u32>;
using Ji1ExecByteHD = be<u8>;
using Ji1ExecShortHD = be<u16>;
using Ji1ExecFloatHD = be<f32>;
struct Ji1ExecHeaderWordsHD {
  be<u32> mWords[4];
};
struct Ji1ExecGlobalRecordHD {
  u8 mRecord[12];
};
struct Ji1ExecCapsuleHD {
  cXyz mStart, mEnd;
  be<u32> __vtbl;
  be<f32> mRadius;
};
struct Ji1ExecCapsuleTailHD {
  u8 _0[0x18];
  be<u32> mShapeVtable;
  be<u32> mAabVtable;
  Ji1ExecCapsuleHD mCps;
};
WWHD_OFFSET(Ji1ExecCapsuleTailHD, mCps, 0x20);
WWHD_SIZE(Ji1ExecCapsuleTailHD, 0x40);
inline Ji1ExecCapsuleTailHD *ji1ExecCapsuleHD(dCcD_Cps *c) {
  return reinterpret_cast<Ji1ExecCapsuleTailHD *>(c->mShapeAttr);
}
struct Ji1ExecEmitterHD {
  u8 _0[0x5C];
  be<s32> mMaxFrame;
  u8 _60[0x254 - 0x60];
  be<u32> mStatus;
};
WWHD_OFFSET(Ji1ExecEmitterHD, mStatus, 0x254);
struct Ji1ExecRemoveTableHD {
  u8 _0[0x44];
  be<u32> mRemove;
};
struct Ji1ExecCallbackHD {
  gptr<Ji1ExecRemoveTableHD> mpVtable;
  void remove() { gabi::call_ptr(mpVtable->mRemove, this); }
};
struct Ji1ExecPatternTableHD {
  u8 _0[0x14];
  be<u32> mGetFrameMax;
};
struct Ji1ExecPatternHD {
  u8 _0[4];
  gptr<Ji1ExecPatternTableHD> mpVtable;
  s32 getFrameMax() {
    return gabi::call_ptr<s32>(mpVtable->mGetFrameMax, this);
  }
};
// HD: capture the matrix address before marking dirty, matching the original
// load order.
inline Mtx34 *ji1ExecGetAnmMtxHD(J3DModel *model, s32 joint) {
  J3DMtxBuffer_hd *block = model->mpMtxBuffer;
  u32 matrices = block->mpAnmMtx;
  block->mFlags |= 0x10;
  return gabi::at<Mtx34>(matrices + joint * 0x30);
}
constexpr u32 ji1ExecHeapCallbackHD = 0x0224A2B4;
constexpr u32 ji1ExecTgHitCallbackHD = 0x02248CF0;
constexpr u32 ji1ExecCoHitCallbackHD = 0x02248CA4;
constexpr u32 ji1ExecAtHitCallbackHD = 0x02248EA0;

BOOL ji1ExecMorfPlayHD(mDoExt_McaMorf *morph, cXyz *pos, u32 sound, s32 reverb);
void ji1ExecHeadMoveHD(void *head);
be<f32> *ji1ExecWindPowerHD();
cXyz *ji1ExecWindVectorHD();
f32 ji1ExecSquareDistanceHD(const cXyz *a, const cXyz *b);
s32 ji1ExecRoomIdHD(dBgS *bgs, void *polygon);
s32 ji1ExecPolyColorHD(dBgS *bgs, void *polygon);
u32 ji1ExecCreateChildHD(const char *name, u32 parent, u32 parameter, cXyz *pos,
                         s32 room, csXyz *angle, cXyz *scale, u32 callback);
void ji1ExecSetActorInfoHD(dNpc_EventCut_c *cut, const char *name,
                           fopAc_ac_c *actor);
void ji1ExecSetCapsuleHD(dCcD_Cps *capsule, void *source);
void ji1ExecSetStartEndHD(void *capsule, cXyz *start, cXyz *end);
void ji1ExecSmokeConstructHD(void *smoke, u32 mode);
void ji1ExecCutConstructHD(void *cut);
void ji1ExecJointConstructHD(void *joint);
void ji1ExecBtpConstructHD(void *anim);
void ji1ExecBrkConstructHD(void *anim);
void ji1ExecBtkConstructHD(void *anim);
void ji1ExecHitInfoConstructHD(void *info);
void ji1ExecLineConstructHD(void *line);
void ji1ExecHeaderByteConstructHD(void *byte);
void ji1ExecHeaderShortConstructHD(void *value);
void ji1ExecHitInfoDestructHD(void *info, s32 flags);
void ji1ExecCircleDestructHD(void *circle, s32 flags);
void ji1ExecAcchDestructHD(void *acch, s32 flags);
void ji1ExecActorDestructHD(void *actor, s32 flags);

inline void ji1ExecConstructAcchHD(dBgS_Acch *acch, u32 a, u32 b, u32 c) {
  const dBgS_ObjAcch_vt tables = {a, b, c};
  dBgS_ObjAcch_ct(acch, tables);
}

// HD: this extra byte throttles successive sword hits.
inline be<u8> &ji1ExecHitCooldownHD(daNpc_Ji1_c *actor) {
  return actor->mHD_E64;
}
enum Ji1ExecAnimation {
  Ji1Exec_BCK_TATEATTACK = 19,
  Ji1Exec_BCK_YOKOATTACK = 20,
  Ji1Exec_BCK_NAKU = 23,
  Ji1Exec_BCK_NUGUI = 24
};
enum Ji1ExecHarpoonState {
  Ji1Exec_HARPOON_HIDDEN,
  Ji1Exec_HARPOON_HELD,
  Ji1Exec_HARPOON_RELEASED
};
