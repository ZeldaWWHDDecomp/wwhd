/**
 * d_a_ship_tail.cpp (WWHD)
 * King of Red Lions: static initialisation, the sight packet's draw, constructors and destructors
 * the compiler emits for the translation unit's members.
 *
 * Verified against cking.rpx.
 */
#include "d/actor/d_a_ship_tail.h"
using namespace ShipTail;

/* 0247D270: HD-only. The cannon-sight packet's draw (J3DPacket virtual): only in pass 2 of the
 * draw info; draws the model's current shape packet and the 25 sight segments with a fresh draw state. */
/* 0247D270 */
static void daShip_sightPacket_drawVirtual(SightPacket *pkt, DrawInfo *info) {
  if (info->pass != 2)
    return;
  gabi::Local<DrawState> state;
  sdk::constructDrawState(state.get());
  u32 flags = state->flags;
  state->state1 = 0;
  state->state0 = 0;
  state->flags = (((flags & ~0xfu) + 7) & ~0xf0u) + 0x10;
  sdk::applyDrawState(state.get());
  u32 model = pkt->model;
  if (model && pkt->modelPacket && pkt->segments) {
    auto *shape = view<SightModel>(model)->currentShape();
    sdk::drawShape(shape, info);
    for (int i = 0; i < 25; i++) {
      u32 segmentModel = pkt->model;
      u32 segment = pkt->segments + i * sizeof(Segment);
      auto *segmentShape = view<SightModel>(segmentModel)->currentShape();
      sdk::drawSightSegment(segmentShape, view<Segment>(segment));
    }
  }
  sdk::applyDrawState(defaultDrawState());
}

/* 0247BA34: createHeap callback (CheckCreateHeap) */
/* 0247BA34 */
static BOOL daShip_CheckCreateHeap(daShip_c *i_this) {
  return i_this->createHeap();
}

/* 0247D26C */
static s32 daShip_Create(daShip_c *i_this) { return i_this->create(); }

/* 02482C1C: __sinit: the header statics and l_rope_base_vec(0.0f, -10.0f, 0.0f) */
/* 02482C1C */
static void __sinit_d_a_ship_cpp() {
  initShipHeaderStatics();
  ropeBaseVec()->x = 0.0f;
  ropeBaseVec()->y = -10.0f;
  ropeBaseVec()->z = 0.0f;
}

/* deleting destructors of classes with nothing to destroy (sead::SafeString and friends) */
/* 02482CD0 */
static void daShip_emptyDt0(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}
/* 02482E1C */
static void daShip_emptyDt1(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}
/* 024832A4 */
static void daShip_emptyDt2(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}
/* 024832B8 */
static void daShip_emptyDt3(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}
/* 024832CC */
static void daShip_emptyDt4(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}

/* 02482CE4: sead::SafeString::assureTerminationImpl_ of a fixed buffer: buffer[size - 1] = 0 */
/* 02482CE4 */
static void daShip_SafeString_assureTermination(void *p) {
  auto *string = static_cast<FixedString *>(p);
  *view<be<u8>>(string->text + string->size - 1) = 0;
}

/* 02482CFC: sead::FormatFixedSafeString<32>(fmt, ...) (0x2C bytes): empty buffer, then format */
/* 02482CFC */
static void *daShip_FormatFixedSafeString_ct(void *p, u32 fmt, u32 a5, u32 a6,
                                             u32 a7, u32 a8, u32 a9, u32 a10) {
  if (!p) {
    p = operator_new(sizeof(FixedString));
    if (!p)
      return p;
  }
  auto *string = static_cast<FixedString *>(p);
  u32 address = gabi::ea(string);
  string->text = gabi::ea(string->buffer);
  string->vtable = FixedStringVtable;
  string->size = 32;
  string->buffer[31] = 0;
  string->vtable = BufferedStringVtable;
  *view<be<u8>>(string->text) = 0;
  string->vtable = FormatStringVtable;
  // HD: retain the original register-save payload and caller overflow-area address.
  gabi::Local<FormatFrame> frame;
  frame->registers[0] = address;
  frame->registers[1] = fmt;
  frame->registers[2] = a5;
  frame->registers[3] = a6;
  frame->registers[4] = a7;
  frame->registers[5] = a8;
  frame->registers[6] = a9;
  frame->registers[7] = a10;
  frame->gpr = 2;
  frame->fpr = 0;
  frame->overflow = gabi::ea(frame.get()) + 0x88;
  frame->registerSave = gabi::ea(frame->registers);
  sdk::formatV(string, fmt, frame.get());
  return p;
}

/* 02482E30: HD sight segment (0xA8 bytes): J3DMatPacket-like base, vtable, matrix at +0x74 */
/* 02482E30 */
static void *daShip_sightSeg_ct(void *p) {
  if (!p) {
    p = operator_new(sizeof(Segment));
    if (!p)
      return p;
  }
  auto *segment = static_cast<Segment *>(p);
  sdk::constructMatPacket(segment);
  segment->vtable = SegmentVtable;
  sdk::clearMemory(&segment->matrix, sizeof(Mtx34) + sizeof(u32));
  // HD: preserve the wrapped matrix-address allocation case.
  if (gabi::ea(&segment->matrix) == 0)
    operator_new(sizeof(Mtx34));
  return p;
}

/* 02482EA0: dCcD_Cyl constructor (this TU's cM3dGAab vtable) */
/* 02482EA0 */
static dCcD_Cyl *daShip_dCcD_Cyl_ct(dCcD_Cyl *p) {
  if (p == nullptr) {
    p = (dCcD_Cyl *)operator_new(0x130);
    if (p == nullptr) {
      return p;
    }
  }
  dCcD_Cyl_ct(p, CylinderBoxVtable);
  return p;
}

/* 02482F2C: dCcD_Stts constructor */
/* 02482F2C */
static dCcD_Stts *daShip_dCcD_Stts_ct(dCcD_Stts *p) {
  if (p == nullptr) {
    p = (dCcD_Stts *)operator_new(0x3C);
    if (p == nullptr) {
      return p;
    }
  }
  dCcD_Stts_ct(p);
  return p;
}

/* 02482F94: constructor of an empty 0xC-byte object */
/* 02482F94 */
static void *daShip_empty_ct(void *p) {
  if (p == nullptr) {
    p = operator_new(0xC);
  }
  return p;
}

/* 02482FC0: dBgS_AcchCir deleting destructor (its cM3dGCir at +0x14) */
/* 02482FC0 */
static void daShip_AcchCir_dt(void *p, s32 flags) {
  if (p) {
    sdk::destroyCircle(&static_cast<dBgS_AcchCir *>(p)->m_cir, 2);
    if (flags & 1)
      operator_delete(p);
  }
}

/* 02483014: HD sight segment deleting destructor */
/* 02483014 */
static void daShip_sightSeg_dt(void *p, s32 flags) {
  if (p) {
    sdk::destroyMatPacket(p, 0);
    if (flags & 1)
      operator_delete(p);
  }
}

/* the sight packet's destructor body: the model (+0x9C) and the segment array (+0xA0) */
static inline void daShip_sightPacket_dtBody(SightPacket *packet) {
  packet->vtable = SightPacketVtable;
  if (packet->modelPacket) {
    sdk::destroyModelPacket((u32)packet->modelPacket, 3);
    packet->modelPacket = 0;
  }
  if (packet->segments) {
    sdk::destroyArray((u32)packet->segments, -1, sizeof(Segment),
                      SegmentDestructor, 1, 0);
    packet->segments = 0;
  }
  sdk::destroyPacket(packet, 0);
}

/* 02483068: HD sight packet deleting destructor */
/* 02483068 */
static void daShip_sightPacket_dt(void *p, s32 flags) {
  if (p != nullptr) {
    daShip_sightPacket_dtBody(static_cast<SightPacket *>(p));
    if (flags & 1)
      operator_delete(p);
  }
}

/* 02483118, 024832A0: empty functions */
/* 02483118 */
static void daShip_empty0() {}
/* 024832A0 */
static void daShip_empty1() {}

/* 0248311C: daShip_c deleting destructor */
/* 0248311C */
static void daShip_c_dt(daShip_c *p, s32 flags) {
  if (!p)
    return;
  sdk::destroyArray(p->mRopeSph, 128, sizeof(dCcD_Sph), SphereDestructor, 0, 0);
  sdk::destroyArray(p->mRopeStts, 128, sizeof(dCcD_Stts), StatusDestructor, 0,
                    0);
  daShip_sightPacket_dtBody(view<SightPacket>(gabi::ea(p->mHDPacket)));
  sdk::destroySphere(&p->mSph, 2);
  sdk::destroyArray(p->mCyl, 3, sizeof(dCcD_Cyl), CylinderDestructor, 0, 0);
  sdk::destroyStatus(&p->mStts, 2);
  p->mAcch.dBgS_GrpPassChk::__vtbl = AcchGroupVtable;
  p->mAcch.dBgS_PolyPassChk::__vtbl = AcchPolyVtable;
  sdk::destroyAcch(&p->mAcch, 0);
  sdk::destroyArray(p->mAcchCir, 4, sizeof(dBgS_AcchCir), AcchCirDestructor, 0,
                    0);
  sdk::destroyRopeLine(&p->mRopeLine, 2);
  sdk::destroyActor(p, 0);
  if (flags & 1)
    operator_delete(p);
}

/* ---- leftover functions of the translation unit ---- */

/* 024832E0 daShip_c::initStartPos (d_a_ship_static.cpp on GameCube; called from other actors' demos):
 * current/old position, both angles, clear the 0x10 state flag, gravity -2.5, stop the effect callbacks.
 * HD: the two follow callbacks (+0x148/+0x15C) are stopped through their vtable (slot +0x44). */
/* 024832E0 */
static void daShip_initStartPos(void *ship, void *pos, s16 rotY) {
  auto *actor = static_cast<daShip_c *>(ship);
  auto *input = static_cast<WordVector *>(pos);
  auto *current = view<WordVector>(gabi::ea(&actor->current.pos));
  auto *old = view<WordVector>(gabi::ea(&actor->old.pos));
  current->x = input->x;
  current->y = input->y;
  current->z = input->z;
  old->x = input->x;
  old->y = input->y;
  u32 z = input->z, flags = actor->mStateFlag;
  actor->gravity = -2.5f;
  old->z = z;
  actor->shape_angle.y = rotY;
  actor->current.angle.y = rotY;
  actor->mStateFlag = flags & ~daSFLG_UNK10_e;
  sdk::removeWave(actor->mWaveL);
  sdk::removeWave(actor->mWaveR);
  sdk::removeSplash(actor->mSplash);
  sdk::removeTrack(actor->mTrack);
  sdk::endRipple(actor->mRipple);
  // HD: these two follow callbacks dispatch through their virtual end slot.
  view<FollowCallback>(gabi::ea(&actor->m1984))->endVirtual();
  view<FollowCallback>(gabi::ea(&actor->m1998))->endVirtual();
  sdk::endFollow(&actor->m19AC);
  sdk::endRipple(actor->m19C0);
}

/* 024833BC __sinit_d_a_ship_static_cpp (ctor list 1018B480): the header statics only */
/* 024833BC */
static void __sinit_d_a_ship_static_cpp() { initStaticHeaderStatics(); }

/* WWHD glue (tools/verify/autoglue.py): private HD packet/frame views; no TU-static objects.
*/
