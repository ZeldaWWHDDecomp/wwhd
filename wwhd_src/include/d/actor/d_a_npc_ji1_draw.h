/* Orca drawing and harpoon: source-private HD views. */
#pragma once
// HD: retain unused legacy event inlines privately; use the shared game API.
#define dComIfGs_isEventBit ji1DrawLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1DrawLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1DrawLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1DrawLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: J3DNode_toJoint already fetched the native joint record.
struct Ji1DrawJointHD {
  u8 _0[4];
  be<u16> mJntNo;
};
WWHD_OFFSET(Ji1DrawJointHD, mJntNo, 4);
struct Ji1DrawPlayerHD : fopAc_ac_c {
  u8 _3AC[0x3F0 - 0x3AC];
  cXyz mLeftHandPos;
};
WWHD_OFFSET(Ji1DrawPlayerHD, mLeftHandPos, 0x3F0);
struct Ji1DrawEmitterHD {
  u8 _0[0x1F0];
  u8 mGlobalRotation[0x24];
  u8 _214[0x22C - 0x214];
  cXyz mGlobalTranslation;
};
WWHD_OFFSET(Ji1DrawEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Ji1DrawEmitterHD, mGlobalTranslation, 0x22C);
using Ji1DrawQuaternionHD = Quaternion_l;

// HD: release copies quaternion words in alternating load/store order.
struct Ji1DrawQuaternionWordsHD {
  be<u32> x, y, z, w;
};
inline void ji1DrawCopyQuaternionHD(Ji1DrawQuaternionHD *dst,
                                    Ji1DrawQuaternionHD *src) {
  auto *d = reinterpret_cast<Ji1DrawQuaternionWordsHD *>(dst);
  auto *s = reinterpret_cast<Ji1DrawQuaternionWordsHD *>(src);
  d->x = u32(s->x);
  d->y = u32(s->y);
  d->z = u32(s->z);
  d->w = u32(s->w);
}

void ji1DrawMultVecHD(Mtx34 *m, cXyz *a, cXyz *b);
void ji1DrawXYZrotMHD(Mtx34 *m, s16 x, s16 y, s16 z);
void ji1DrawSetEmitterMatrixHD(Mtx34 *m, void *rotation, cXyz *translation);
void ji1DrawRegistFigHD(s32 type, fopAc_ac_c *actor, cXyz *pos, s16 angle,
                        f32 x, f32 y, f32 z);
BOOL ji1DrawInitBtpHD(void *anim, J3DModelData *data, void *btp, s32 play,
                      s32 mode, f32 rate, s16 start, s16 end, bool modify,
                      s32 entry);
BOOL ji1DrawInitBrkHD(void *anim, J3DModelData *data, void *brk, s32 play,
                      s32 mode, f32 rate, s16 start, s16 end, bool modify,
                      s32 entry);
void ji1DrawPlayerEyePosHD(cXyz *out, f32 offset);
void ji1DrawLookAtTargetHD(dNpc_JntCtrl_c *joint, be<s16> *y, cXyz *target,
                           cXyz *eye, s16 a, s16 b, bool turn);
void ji1DrawQuatMultiplyHD(Ji1DrawQuaternionHD *a, Ji1DrawQuaternionHD *b,
                           Ji1DrawQuaternionHD *out);
void ji1DrawQuatNormalizeHD(Ji1DrawQuaternionHD *a, Ji1DrawQuaternionHD *out);
void ji1DrawQuatSlerpHD(Ji1DrawQuaternionHD *a, Ji1DrawQuaternionHD *b,
                        Ji1DrawQuaternionHD *out, f32 t);
void ji1DrawVectorSubtractHD(const cXyz *a, const cXyz *b, cXyz *out);
void ji1DrawMatrixConcatHD(Mtx34 *a, Mtx34 *b, Mtx34 *out);
void ji1DrawQuatMHD(Ji1DrawQuaternionHD *q);
void ji1DrawQuatRotVecHD(Ji1DrawQuaternionHD *q, cXyz *a, cXyz *b);
BOOL ji1DrawNormalizeHD(cXyz *v);
void ji1DrawSeStartHD(u32 id, cXyz *pos, u32 param);

enum Ji1DrawResource {
  dRes_INDEX_JI_BDL_JI_e = 0x42,
  dRes_INDEX_JI_BCK_WAIT01_e = 0x3F,
  dRes_INDEX_JI_BAS_WAIT01_e = 0x21,
  dRes_INDEX_JI_BDL_JI_YARI_e = 0x43,
  dRes_INDEX_JI_BCK_JIYARI_TATEATTACK_e = 0x3D,
  dRes_INDEX_JI_BDL_YJITR00_e = 0x44,
  dRes_INDEX_JI_BRK_YJITR00_e = 0x47,
  dRes_INDEX_JI_BTK_YJITR00_e = 0x4A,
  dRes_INDEX_JI_BTP_JI_e = 0x4D,
};
enum Ji1DrawAnimation {
  Ji1Draw_BCK_WARAI = 13,
  Ji1Draw_BCK_BTKAMASI = 16,
  Ji1Draw_BCK_BTKAMAE = 17,
  Ji1Draw_BCK_YOKOATTACK = 20,
};
enum Ji1DrawHarpoonState {
  Ji1Draw_HARPOON_HIDDEN,
  Ji1Draw_HARPOON_HELD,
  Ji1Draw_HARPOON_RELEASED
};
enum {
  DSNAP_TYPE_NPC_JI1 = 0x4D,
  JA_SE_CM_LANCE_BOUND = 0x5807,
  JI_YARI_JNT_JI_YARI_e = 1
};
