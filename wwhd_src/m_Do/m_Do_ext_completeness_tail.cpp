/* HD SDK compiler-generated tail; GameCube packet/line roles retained.
 * Preserve guest reloads, write order and PPC arithmetic. */
#include "m_Do/m_Do_ext_completeness_tail.h"
using namespace mDoExtTail;
using namespace gabi;

/* 025EE34C */
void mDoExt_HDLineManager_dt(void *self, u32 flags) {
  u32 s = ea(self);
  if (!s)
    return;
  u32 base = static_cast<u32>(view<LineManager>(s)->mpGroups);
  if (base) {
    for (s32 i = 0; i < static_cast<s32>(view<LineManager>(s)->mCount); i++) {
      u32 group = base + (u32)i * 20;
      if (group) {
        (view<LineGroup>(group)->mState = 0);
        for (u32 slot : {8u, 16u}) {
          u32 array = static_cast<u32>(
              view<LineGroup>(group)->mArrays[(slot - 8) / 8].mpEntries);
          if (array) {
            for (s32 j = 0;
                 j <
                 static_cast<s32>(
                     view<LineGroup>(group)->mArrays[(slot - 8) / 8].mCount);
                 j++) {
              u32 entry = array + (u32)j * 0xF4;
              call_ptr(
                  static_cast<u32>(
                      view<EntryVtable>(
                          static_cast<u32>(view<LineEntry>(entry)->mpVtable))
                          ->mDestroy),
                  view<void>(entry), 2);
              array = static_cast<u32>(
                  view<LineGroup>(group)->mArrays[(slot - 8) / 8].mpEntries);
            }
            u32 heap = findFromRoot(view<void>(rootHeap()), view<void>(array));
            call_ptr(static_cast<u32>(
                         view<HeapVtable>(
                             static_cast<u32>(view<Heap>(heap)->mpVtable))
                             ->mFree),
                     view<void>(heap),
                     view<void>(static_cast<u32>(view<LineGroup>(group)
                                                     ->mArrays[(slot - 8) / 8]
                                                     .mpEntries)));
            (view<LineGroup>(group)->mArrays[(slot - 8) / 8].mCount = 0);
            (view<LineGroup>(group)->mArrays[(slot - 8) / 8].mpEntries = 0);
          }
        }
        base = static_cast<u32>(view<LineManager>(s)->mpGroups);
      }
    }
    u32 heap = findFromRoot(view<void>(rootHeap()), view<void>(base));
    call_ptr(static_cast<u32>(
                 view<HeapVtable>(static_cast<u32>(view<Heap>(heap)->mpVtable))
                     ->mFree),
             view<void>(heap),
             view<void>(static_cast<u32>(view<LineManager>(s)->mpGroups)));
    (view<LineManager>(s)->mCount = 0);
    (view<LineManager>(s)->mpGroups = 0);
  }
  if (flags & DELETE_MEMORY)
    deleteObject(self);
}
/* 025EE22C */
void mDoExt_offCupOnAupPacket_drawContext(void *self, void *arguments) {
  call_ptr(
      static_cast<u32>(
          view<PacketVtable>(static_cast<u32>(view<Packet>(ea(self))->mpVtable))
              ->mDraw),
      self, arguments);
}
/* 025EE290 */
void mDoExt_onCupOffAupPacket_drawContext(void *self, void *arguments) {
  call_ptr(
      static_cast<u32>(
          view<PacketVtable>(static_cast<u32>(view<Packet>(ea(self))->mpVtable))
              ->mDraw),
      self, arguments);
}
/* 025EE2F4 */
void mDoExt_invJntPacket_noop() {}
/* 025EE584 */
s32 mDoExt_3DlineMat0_getMaterialID() { return 0; }
/* 025EE58C */
s32 mDoExt_3DlineMat1_getMaterialID() { return 1; }
/* 025EE594 */
void mDoExt_3DlineMatSortPacket_noop() {}
/* 025F03AC */
void mDoExt_3DlineMat_dt(void *self, u32 flags) {
  if (ea(self) && (flags & DELETE_MEMORY))
    deleteObject(self);
}
/* 025F03C0 */
void mDoExt_3DlineMat_noop() {}
/* HD: GHS line-element constructors and SDK quaternion helper. */

/* 025EE4E8: array-element constructor (12 bytes) used by the line-material init functions
 * (025E9B80, 025EBA58 via the array constructor 028EFFD0): zero word at +0, then the inline
 * constructor of the 8-byte member at +4 (GHS null check of the member address, allocation when
 * it is NULL) zeroes it. */
/* 025EE4E8 */
void *mDoExt_HDLineElement_ct(void *self) {

  u32 o = ea(self);
  if (!o) {
    o = allocate(0xC);
    if (!o)
      return nullptr;
  }
  u32 m = o + 4;
  (view<LineElement>(o)->mState = 0);
  if (!m) {
    m = allocate(8);
    if (!m)
      return view<void>(o);
  }
  (view<LineMember>(m)->mNext = 0);
  (view<LineMember>(m)->mValue = 0);
  return view<void>(o);
}
/* 025EE558: array-element constructor of an empty 16-byte class (line-material init, 025E9B80):
 * only allocates when this == NULL */
/* 025EE558 */
void *mDoExt_HDLineEmptyElement_ct(void *self) {

  u32 o = ea(self);
  if (!o)
    o = allocate(0x10);
  return view<void>(o);
}
/* 025EE5EC: this TU's sead::SafeString assureTerminationImpl_ (vtable 10058704; also called
 * directly by mDoExt_McaMorf::setAnm / mDoExt_McaMorf2::setAnm): empty */
/* 025EE5EC */
void mDoExt_safeString_assureTerminationImpl(void *self) {}
/* 025EE5F0: rotation matrix (3x4, r4) -> quaternion {x,y,z,w} (r3), this TU's out-of-line copy of
 * the sead quaternion-from-matrix helper (called by the old-frame and morf2 calc functions).
 * Picks the largest of w^2 = (trace+1)/4 and x^2, y^2, z^2 (= w^2 - (other two diagonals)/2),
 * takes its square root with the frsqrte + one Newton step idiom, divides 1/4 by it and fills
 * the other three components from matrix sums/differences. */
static inline f32 quat_sqrt(f32 v, f32 zero, f32 half, f32 three) {
  /* the estimate stays a double; fmuls rounds its second operand to 25 bits (Espresso) */
  f32 r = zero;
  if (v > zero) {
    f64 e = frsqrte(v);
    f32 sq = (f32)(e * round25(e)), eh = (f32)(e * round25(half));
    sq = fnmsubs(sq, v, three);
    r = fmuls_ppc(sq, eh);
  }
  return fmuls_ppc(r, v);
}
/* 025EE5F0 */
void mDoExt_matrixToQuat(void *outp, void *mtxp) {

  u32 q = ea(outp), m = ea(mtxp);
  f32 m00 = static_cast<f32>(view<Matrix34>(m)->value[0][0]),
      m11 = static_cast<f32>(view<Matrix34>(m)->value[1][1]),
      m22 = static_cast<f32>(view<Matrix34>(m)->value[2][2]);
  f32 s01 = fadds_ppc(m00, m11);
  f32 tr = fadds_ppc(s01, m22);
  f32 one = static_cast<f32>(constants()->mOne);
  f32 s20 = fadds_ppc(m22, m00);
  tr = fadds_ppc(tr, one);
  f32 quarter = static_cast<f32>(constants()->mQuarter);
  f32 s12 = fadds_ppc(m11, m22);
  f32 w2 = fmuls_ppc(quarter, tr);
  f32 half = static_cast<f32>(constants()->mHalf);
  f32 x2 = fnmsubs(half, s12, w2), y2 = fnmsubs(half, s20, w2),
      z2 = fnmsubs(half, s01, w2);
  int c;
  if (w2 > x2) {
    if (w2 > y2) {
      c = (w2 > z2) ? 0 : 3;
    } else
      c = (y2 > z2) ? 2 : 3;
  } else if (x2 > y2) {
    c = (x2 > z2) ? 1 : 3;
  } else
    c = (y2 > z2) ? 2 : 3;
  f32 zero = static_cast<f32>(constants()->mZero),
      three = static_cast<f32>(constants()->mThree);
  if (c == 0) {
    f32 r = quat_sqrt(w2, zero, half, three);
    f32 k = quarter / r;
    (view<QuatView>(q)->value[3] = r);
    (view<QuatView>(q)->value[0] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[2][1]),
                             static_cast<f32>(view<Matrix34>(m)->value[1][2])),
                   k));
    (view<QuatView>(q)->value[1] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[0][2]),
                             static_cast<f32>(view<Matrix34>(m)->value[2][0])),
                   k));
    (view<QuatView>(q)->value[2] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[1][0]),
                             static_cast<f32>(view<Matrix34>(m)->value[0][1])),
                   k));
  } else if (c == 1) {
    f32 r = quat_sqrt(x2, zero, half, three);
    f32 k = quarter / r;
    (view<QuatView>(q)->value[0] = r);
    (view<QuatView>(q)->value[3] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[2][1]),
                             static_cast<f32>(view<Matrix34>(m)->value[1][2])),
                   k));
    (view<QuatView>(q)->value[1] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[0][1]),
                             static_cast<f32>(view<Matrix34>(m)->value[1][0])),
                   k));
    (view<QuatView>(q)->value[2] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[0][2]),
                             static_cast<f32>(view<Matrix34>(m)->value[2][0])),
                   k));
  } else if (c == 2) {
    f32 r = quat_sqrt(y2, zero, half, three);
    f32 k = quarter / r;
    (view<QuatView>(q)->value[1] = r);
    (view<QuatView>(q)->value[3] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[0][2]),
                             static_cast<f32>(view<Matrix34>(m)->value[2][0])),
                   k));
    (view<QuatView>(q)->value[2] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[1][2]),
                             static_cast<f32>(view<Matrix34>(m)->value[2][1])),
                   k));
    (view<QuatView>(q)->value[0] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[1][0]),
                             static_cast<f32>(view<Matrix34>(m)->value[0][1])),
                   k));
  } else {
    f32 r = quat_sqrt(z2, zero, half, three);
    f32 k = quarter / r;
    (view<QuatView>(q)->value[2] = r);
    (view<QuatView>(q)->value[3] =
         fmuls_ppc(fsubs_ppc(static_cast<f32>(view<Matrix34>(m)->value[1][0]),
                             static_cast<f32>(view<Matrix34>(m)->value[0][1])),
                   k));
    (view<QuatView>(q)->value[0] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[2][0]),
                             static_cast<f32>(view<Matrix34>(m)->value[0][2])),
                   k));
    (view<QuatView>(q)->value[1] =
         fmuls_ppc(fadds_ppc(static_cast<f32>(view<Matrix34>(m)->value[2][1]),
                             static_cast<f32>(view<Matrix34>(m)->value[1][2])),
                   k));
  }
}
/* HD: compiler-generated deleting destructors.
 * GHS deleting destructors in this TU's vtables (destructor slot +0xC): run the base destructor with
 * flags 0, then free the object when bit 0 of the flags is set; nothing when this is NULL.
 * The matcher's name J3DFrameCtrl::~J3DFrameCtrl is wrong for all five. */
static inline void deletingDtor(void *self, u32 flags, u32 base) {
  if (!ea(self))
    return;
  call_ptr(base, self, 0);
  if (flags & DELETE_MEMORY)
    deleteObject(self);
}
/* 025EE1D8: mDoExt_McaMorf's calculation-interface vtable 10058CC0 (set by the constructor 025E4F64);
 * the base destructor 027F3628 is the empty J3DMtxCalc-family destructor */
/* 025EE1D8 */
void mDoExt_McaMorf_mtxCalc_dt(void *self, u32 flags) {
  deletingDtor(self, flags, MTX_CALC_BASE);
}
/* 025EE23C: mDoExt_offCupOnAupPacket (vtable 10058D10, draw 025E8728); base J3DPacket destructor 027F13DC */
/* 025EE23C */
void mDoExt_offCupOnAupPacket_dt(void *self, u32 flags) {
  deletingDtor(self, flags, PACKET_BASE);
}
/* 025EE2A0: mDoExt_onCupOffAupPacket (vtable 10058D40, draw 025E8774) */
/* 025EE2A0 */
void mDoExt_onCupOffAupPacket_dt(void *self, u32 flags) {
  deletingDtor(self, flags, PACKET_BASE);
}
/* 025EE2F8: mDoExt_invJntPacket (vtable 10058D70, draw 025E87C0) */
/* 025EE2F8 */
void mDoExt_invJntPacket_dt(void *self, u32 flags) {
  deletingDtor(self, flags, PACKET_BASE);
}
/* 025EE598: mDoExt_3DlineMatSortPacket (vtable 10058E20, set by the constructor 025EDC2C; draw 025EDC88) */
/* 025EE598 */
void mDoExt_3DlineMatSortPacket_dt(void *self, u32 flags) {
  deletingDtor(self, flags, PACKET_BASE);
}

/* WWHD glue (tools/verify/autoglue.py): no TU-wide static objects. */
