/** Actor utility helpers, tww names and layouts. HD entry widths and call
 * ordering are retained. */
#include "d/d_lib.h"
// HD: entry points retain the verified word-address ABI and f64 floating arguments.
// HD: STControl puts its vtable last; field names follow tww, with HD offsets in the header.
// HD: secondary animation is the ninth integer argument, read from the guest stack.
using gabi::cpu;
using gabi::fadds_ppc;
using gabi::fmadds;
using gabi::fsubs_ppc;
using gabi::ftoi;

static f32 l_zero;
static f32 l_noGround;
static f32 l_waveSpacing;
static f32 l_waveAmplitude;
static f32 l_anmInitialFrame;
static f32 l_anmEndFrame;
static f32 l_groundRaise;
static f32 l_negativePi;
static f32 l_pi;
static f32 l_epsilon;
static f32 l_waveTuning;
static dLib_layout::dataAddress l_safeStringVtable;
static dLib_layout::dataAddress l_passBaseVtable;
static dLib_layout::dataAddress l_groundChkVtable;
static dLib_layout::dataAddress l_groundPolyVtable;
static dLib_layout::dataAddress l_groupPassVtable;
static dLib_layout::dataAddress l_polyPassVtable;
static dLib_layout::dataAddress l_stVtable;
static dLib_layout::dataAddress l_brkFile;
static dLib_layout::dataAddress l_brkCondition;
static dLib_layout::dataAddress l_btkFile;
static dLib_layout::dataAddress l_btkCondition;
static dLib_layout::dataAddress l_stageFile;
static dLib_layout::dataAddress l_roomCondition;
static dLib_layout::dataAddress l_stageEntriesCondition;
static dLib_layout::dataAddress l_stageIndexCondition;
static dLib_layout::dataAddress l_pathFile;
static dLib_layout::dataAddress l_pathCondition;
static dLib_layout::resourcePointer l_resources;
static dLib_layout::gamePointer l_gameInfo;
static Mtx34 l_now;
static dLib_layout::headerStatics l_headerStatics;
static dLib_layout::staticRecords l_records;
static dLib_layout::word l_axisXGuard;
static dLib_layout::word l_axisYGuard;
static dLib_layout::word l_axisZGuard;
static GXColor l_axisXColor;
static GXColor l_axisYColor;
static GXColor l_axisZColor;
static GXColor l_axisXSeed;
static GXColor l_axisYSeed;
static GXColor l_axisZSeed;

/* inlined into dLib_setAnm and dLib_bcks_setAnm */
static u32 dLib_resource(u32 arc, u32 index) {
  gabi::Local<dLib_layout::safeString> ref;
  ref->mData = arc;
  ref->mpVtable = gabi::ea(&l_safeStringVtable);
  return dLib_getResource(l_resources.mResource, gabi::ea(ref.get()), index);
}

/* inlined into dLib_setAnm and dLib_bcks_setAnm */
static void dLib_animations(u32 arc, u32 model, u32 animation, u32 parameter,
                            u32 previous, u32 indices, u32 parameters,
                            u32 force, u32 secondary, bool sounds) {
  s8 parm = *dLib_at<be<s8>>(parameter), old = *dLib_at<be<s8>>(previous);
  f32 zero = l_zero;
  s8 next = 0;
  bool change = false;
  if (parm != old) {
    next = dLib_at<dLib_anm_prm_c>(parameters + u32(s32(parm)) * 16)->mAnmIdx;
    change = next != -1;
  }
  if (!change && force) {
    next = dLib_at<dLib_anm_prm_c>(parameters + u32(s32(parm)) * 16)->mAnmIdx;
    change = true;
  }
  if (change) {
    u32 stride = sounds ? 8 : 4;
    *dLib_at<be<s8>>(animation) = next;
    u32 sound = 0, idx = indices + u32(s32(next)) * stride;
    if (sounds && dLib_at<dLib_anm_idx_c>(idx)->mSndIdx >= 0) {
      sound = dLib_resource(arc, dLib_at<dLib_anm_idx_c>(idx)->mSndIdx);
      idx = indices + u32(s32(*dLib_at<be<s8>>(animation))) * stride;
    }
    u32 data = dLib_resource(arc, dLib_at<dLib_anm_idx_c>(idx)->mBckIdx);
    u32 row = parameters + u32(s32(*dLib_at<be<s8>>(parameter))) * 16;
    f32 end = l_anmEndFrame;
    dLib_morfSetAnm(model, data, dLib_at<dLib_anm_prm_c>(row)->mLoopMode, sound,
                    dLib_at<dLib_anm_prm_c>(row)->mMorf,
                    dLib_at<dLib_anm_prm_c>(row)->mPlaySpeed, zero, end);
    if (secondary) {
      u32 extra = dLib_resource(
          arc, *dLib_at<be<u32>>(
                   indices + u32(s32(*dLib_at<be<s8>>(animation))) * stride));
      row = parameters + u32(s32(*dLib_at<be<s8>>(parameter))) * 16;
      dLib_morfSetAnm(secondary, extra, dLib_at<dLib_anm_prm_c>(row)->mLoopMode,
                      u32(0), dLib_at<dLib_anm_prm_c>(row)->mMorf,
                      dLib_at<dLib_anm_prm_c>(row)->mPlaySpeed, zero, end);
    }
    parm = *dLib_at<be<s8>>(parameter);
  }
  *dLib_at<be<s8>>(previous) = parm;
  if ((dLib_at<mDoExt_McaMorf>(model)->mFrameCtrl.mState & 1) ||
      dLib_at<mDoExt_McaMorf>(model)->mFrameCtrl.mRate == zero) {
    u32 row = parameters + u32(s32(*dLib_at<be<s8>>(parameter))) * 16;
    s8 nextparm = dLib_at<dLib_anm_prm_c>(row)->mNextPrmIdx;
    if (nextparm != -1 && dLib_at<dLib_anm_prm_c>(row)->mLoopMode == 0)
      *dLib_at<be<s8>>(parameter) = nextparm;
  }
}

/* inlined into STControl_checkLeftTrigger, STControl_checkRightTrigger,
 * STControl_checkUpTrigger and STControl_checkDownTrigger */
static s32 STControl_repeat(STControl *control, u8 bit, be<s16> *timer,
                            be<s16> *current, be<s16> *delay) {
  if (*timer != 0 || !(control->field_0x0c & bit))
    return 0;
  s16 remaining = *delay;
  *timer = *current;
  if (remaining == 0) {
    s16 next = s16(*current - control->field_0x16);
    s16 minimum = control->field_0x14;
    *current = next;
    if (next < minimum)
      *current = minimum;
  } else {
    *delay = remaining - 1;
  }
  return 1;
}

/* 02587684 */
void dLib_debugDrawFan() {}

/* 02587C88 */
void dLib_getPosFromMtx(u32 m, u32 out) {
  f32 y = dLib_at<Mtx34>(m)->m[1][3], x = dLib_at<Mtx34>(m)->m[0][3],
      z = dLib_at<Mtx34>(m)->m[2][3];
  dLib_at<cXyz>(out)->x = x;
  dLib_at<cXyz>(out)->y = y;
  dLib_at<cXyz>(out)->z = z;
}

/* 025885C4 */
void STControl_setWaitParm(u32 p, s32 first, s32 minimum, s32 step, s32 delay,
                           s32 margin, s32 sector, f64 high, f64 low) {
  dLib_at<STControl>(p)->field_0x12 = first;
  dLib_at<STControl>(p)->field_0x14 = minimum;
  dLib_at<STControl>(p)->field_0x16 = step;
  dLib_at<STControl>(p)->field_0x1c = delay;
  dLib_at<STControl>(p)->field_0x04 = high;
  dLib_at<STControl>(p)->field_0x08 = low;
  dLib_at<STControl>(p)->field_0x24 = margin;
  dLib_at<STControl>(p)->field_0x26 = sector;
}

/* 025885E8 */
void STControl_init(u32 p) {
  s16 first = dLib_at<STControl>(p)->field_0x12;
  dLib_at<STControl>(p)->field_0x10 = 0;
  s16 delay = dLib_at<STControl>(p)->field_0x1c;
  dLib_at<STControl>(p)->field_0x18 = first;
  dLib_at<STControl>(p)->field_0x1e = delay;
  dLib_at<STControl>(p)->field_0x22 = 0;
  dLib_at<STControl>(p)->field_0x20 = delay;
  dLib_at<STControl>(p)->field_0x0c = 0;
  dLib_at<STControl>(p)->field_0x0e = 0;
  dLib_at<STControl>(p)->field_0x0d = 0;
  dLib_at<STControl>(p)->field_0x1a = first;
}

/* 025886D8 */
void STControl_delete(u32 p, u32 flags) {
  if (p && (flags & 1))
    dLib_free(p, flags);
}

/* 025886EC */
void STControl_Xinit(u32 p) {
  s16 first = dLib_at<STControl>(p)->field_0x12;
  u8 flags = dLib_at<STControl>(p)->field_0x0c & 0xfc;
  s16 delay = dLib_at<STControl>(p)->field_0x1c;
  dLib_at<STControl>(p)->field_0x0e = 0;
  dLib_at<STControl>(p)->field_0x0c = flags;
  dLib_at<STControl>(p)->field_0x18 = first;
  dLib_at<STControl>(p)->field_0x1e = delay;
}

/* 02588714 */
void STControl_Yinit(u32 p) {
  s16 first = dLib_at<STControl>(p)->field_0x12;
  u8 flags = dLib_at<STControl>(p)->field_0x0c & 0xf3;
  s16 delay = dLib_at<STControl>(p)->field_0x1c;
  dLib_at<STControl>(p)->field_0x10 = 0;
  dLib_at<STControl>(p)->field_0x0c = flags;
  dLib_at<STControl>(p)->field_0x1a = first;
  dLib_at<STControl>(p)->field_0x20 = delay;
}

/* 02588A0C */
s32 STControl_checkLeftTrigger(u32 p) {
  return STControl_repeat(dLib_at<STControl>(p), STControl::LEFT,
                          &dLib_at<STControl>(p)->field_0x0e,
                          &dLib_at<STControl>(p)->field_0x18,
                          &dLib_at<STControl>(p)->field_0x1e);
}

/* 02588A7C */
s32 STControl_checkRightTrigger(u32 p) {
  return STControl_repeat(dLib_at<STControl>(p), STControl::RIGHT,
                          &dLib_at<STControl>(p)->field_0x0e,
                          &dLib_at<STControl>(p)->field_0x18,
                          &dLib_at<STControl>(p)->field_0x1e);
}

/* 02588AEC */
s32 STControl_checkUpTrigger(u32 p) {
  return STControl_repeat(
      dLib_at<STControl>(p), STControl::UP, &dLib_at<STControl>(p)->field_0x10,
      &dLib_at<STControl>(p)->field_0x1a, &dLib_at<STControl>(p)->field_0x20);
}

/* 02588B5C */
s32 STControl_checkDownTrigger(u32 p) {
  return STControl_repeat(dLib_at<STControl>(p), STControl::DOWN,
                          &dLib_at<STControl>(p)->field_0x10,
                          &dLib_at<STControl>(p)->field_0x1a,
                          &dLib_at<STControl>(p)->field_0x20);
}

/* 02588BCC */
f64 STControl_getValueStick() { return dLib_padStickValue(u32(0)); }

/* 02588BD4 */
s32 STControl_getAngleStick() { return dLib_padStickAngle(u32(0)); }

/* 02588DD4 */
void dLib_headerObjectDelete(u32 p, u32 flags) {
  if (p && (flags & 1))
    dLib_free(p, flags);
}

/* 02588DE8 */
void dLib_headerObjectDestroy() {}

/* 02588034 */
u32 dLib_setFirstMsg(u32 bit, u32 first, u32 again) {
  if (dLib_isEventBit(gabi::ea(&dLib_at<dLib_layout::savedGame>(gabi::ea(l_gameInfo.mGame.get()))->mEvent), bit))
    return again;
  dLib_onEventBit(gabi::ea(&dLib_at<dLib_layout::savedGame>(gabi::ea(l_gameInfo.mGame.get()))->mEvent), bit);
  return first;
}

/* 02587688 */
s32 dLib_brkInit(u32 model, u32 animation, u32 arc, u32 number) {
  gabi::Local<dLib_layout::safeString> ref;
  ref->mData = arc;
  ref->mpVtable = gabi::ea(&l_safeStringVtable);
  u32 resource =
      dLib_getResource(l_resources.mResource, gabi::ea(ref.get()), number);
  if (!resource)
    dLib_assert(u32(gabi::ea(&l_brkFile)), u32(0xae),
                u32(gabi::ea(&l_brkCondition)));
  return dLib_brkInitialize(animation, model, resource, u32(1), u32(0), u32(0),
                            u32(-1), u32(0), u32(0), l_anmInitialFrame) != 0;
}

/* 02587740 */
s32 dLib_btkInit(u32 model, u32 animation, u32 arc, u32 number) {
  gabi::Local<dLib_layout::safeString> ref;
  ref->mData = arc;
  ref->mpVtable = gabi::ea(&l_safeStringVtable);
  u32 resource =
      dLib_getResource(l_resources.mResource, gabi::ea(ref.get()), number);
  if (!resource)
    dLib_assert(u32(gabi::ea(&l_btkFile)), u32(0xbb),
                u32(gabi::ea(&l_btkCondition)));
  return dLib_btkInitialize(animation, model, resource, u32(1), u32(0), u32(0),
                            u32(-1), u32(0), u32(0), l_anmInitialFrame) != 0;
}

/* 0258861C */
u32 STControl_construct(u32 p, s32 first, s32 minimum, s32 step, s32 delay,
                        s32 margin, s32 sector, f64 high, f64 low) {
  if (!p)
    p = dLib_alloc(u32(40));
  if (!p)
    return p;
  dLib_at<STControl>(p)->mpVtable.v = gabi::ea(&l_stVtable);
  p = STControl_setWaitParmResult(p, first, minimum, step, delay, margin,
                                  sector, high, low);
  return STControl_initResult(p);
}

/* 02588BDC */
s32 dLib_groundPosition(u32 pos) {
  gabi::Local<dBgS_GndChk> ground;
  u32 a = gabi::ea(ground.get());
  dLib_groundConstruct(gabi::ea(ground.get()));
  ground->dBgS_GrpPassChk::__vtbl = gabi::ea(&l_groupPassVtable);
  ground->mbObjThrough = 0;
  f32 x = dLib_at<cXyz>(pos)->x, y = dLib_at<cXyz>(pos)->y;
  ground->mbArrowThrough = 0;
  ground->cBgS_Chk::__vtbl = gabi::ea(&l_groundChkVtable);
  f32 up = l_groundRaise;
  ground->mGrp = 1;
  ground->mbBombThrough = 0;
  y = fadds_ppc(y, up);
  ground->dBgS_PolyPassChk::__vtbl = gabi::ea(&l_polyPassVtable);
  ground->cBgS_Chk::mGrpPassChk = a + 0x4c;
  ground->mbLinkThrough = 0;
  ground->mbCamThrough = 0;
  ground->cBgS_Chk::mPolyPassChk = a + 0x40;
  ground->cBgS_PolyInfo::__vtbl = gabi::ea(&l_groundPolyVtable);
  dLib_at<cXyz>(pos)->y = y;
  ground->mbBoomerangThrough = 0;
  ground->mbRopeThrough = 0;
  f32 z = dLib_at<cXyz>(pos)->z;
  ground->m_pos.x = x;
  ground->m_pos.y = y;
  ground->m_pos.z = z;
  u32 game = dLib_getGame();
  f64 height = dLib_groundCross(game + 0x12a0, gabi::ea(ground.get()));
  bool fail = height == l_noGround;
  if (fail) {
    f32 old = dLib_at<cXyz>(pos)->y;
    ground->cBgS_PolyInfo::__vtbl = gabi::ea(&l_groundPolyVtable);
    ground->dBgS_GrpPassChk::__vtbl = gabi::ea(&l_passBaseVtable);
    ground->dBgS_PolyPassChk::__vtbl = gabi::ea(&l_polyPassVtable);
    dLib_at<cXyz>(pos)->y = fsubs_ppc(old, up);
  } else {
    ground->cBgS_PolyInfo::__vtbl = gabi::ea(&l_groundPolyVtable);
    ground->dBgS_PolyPassChk::__vtbl = gabi::ea(&l_polyPassVtable);
    ground->dBgS_GrpPassChk::__vtbl = gabi::ea(&l_passBaseVtable);
    dLib_at<cXyz>(pos)->y = height;
  }
  dLib_groundDestroy(gabi::ea(ground.get()), u32(0));
  return !fail;
}

/* 02587128 */
void dLib_setCirclePath(u32 p) {
  s32 angle = dLib_at<dLib_circle_path_c>(p)->mAngle +
              dLib_at<dLib_circle_path_c>(p)->mAngleSpeed;
  f32 amplitude = dLib_at<dLib_circle_path_c>(p)->mWobbleAmplitude,
      x = dLib_at<dLib_circle_path_c>(p)->mTranslation.x,
      y = dLib_at<dLib_circle_path_c>(p)->mTranslation.y;
  dLib_at<dLib_circle_path_c>(p)->mAngle = angle;
  f32 sine = dLib_sine(angle);
  f32 radius = dLib_at<dLib_circle_path_c>(p)->mRadius,
      z = dLib_at<dLib_circle_path_c>(p)->mTranslation.z;
  f32 radial = fmadds(sine, amplitude, radius);
  dLib_matrixTrans(u32(gabi::ea(&l_now)), x, y, z);
  dLib_matrixYrotM(u32(gabi::ea(&l_now)),
                   s32(dLib_at<dLib_circle_path_c>(p)->mAngle));
  f32 zero = l_zero;
  dLib_matrixTransM(radial, zero, zero);
  dLib_at<dLib_circle_path_c>(p)->mPos.x = l_now.m[0][3];
  dLib_at<dLib_circle_path_c>(p)->mPos.y = l_now.m[1][3];
  dLib_at<dLib_circle_path_c>(p)->mPos.z = l_now.m[2][3];
}

/* 025871F8 */
f64 dLib_getWaterY(u32 pos, u32 acch) {
  bool hit = (dLib_at<dBgS_ObjAcch>(acch)->m_flags >> 11) & 1;
  f64 water = hit ? dLib_at<dBgS_ObjAcch>(acch)->m_wtr.m_height : l_noGround;
  u32 area = dLib_seaArea(dLib_at<cXyz>(pos)->x, dLib_at<cXyz>(pos)->z);
  if (area) {
    f64 sea = dLib_seaWave(dLib_at<cXyz>(pos)->x, dLib_at<cXyz>(pos)->z);
    if (!hit || sea > water)
      return sea;
  }
  return hit ? water : l_zero;
}

/* 02587CA4 */
s32 dLib_pathInfo(u32 out, s32 point) {
  u32 manager = gabi::ea(&dLib_at<dComIfG_inf_c>(dLib_getGame())->play.mStageData);
  u32 table = dLib_at<dLib_layout::stageData>(manager)->mpVtable.v;
  u32 path = gabi::call_ptr<u32>(
      dLib_at<dLib_layout::stageVtable>(table)->getPathInf, manager);
  if (!path)
    return 0;
  dLib_at<gptr<dLib_layout::dPath>>(out)->v =
      dLib_at<dLib_layout::dStage_dPath_c>(path)->m_path.v + u32(point) * 12;
  return 1;
}

/* 0258873C */
u32 STControl_checkTrigger(u32 p) {
  u8 old = dLib_at<STControl>(p)->field_0x0c;
  u32 table = dLib_at<STControl>(p)->mpVtable.v;
  dLib_at<STControl>(p)->field_0x0d = old;
  f64 value =
      gabi::call_ptr<f64>(dLib_at<STControlVtable>(table)->getValueStick, p);
  table = dLib_at<STControl>(p)->mpVtable.v;
  s32 angle =
      gabi::call_ptr<s32>(dLib_at<STControlVtable>(table)->getAngleStick, p);
  s16 sector = s16((0x2000 - dLib_at<STControl>(p)->field_0x26) >> 1);
  if (std::fabs(value) < l_epsilon) {
    dLib_at<STControl>(p)->field_0x0c = 0;
    STControl_Xinit(p);
    STControl_Yinit(p);
  } else {
    s32 shift = dLib_at<STControl>(p)->field_0x22;
    u8 bits;
    if (angle < shift + sector - 0x7000)
      bits = 4;
    else if (angle < shift - 0x5000 - sector)
      bits = 5;
    else if (angle < shift + sector - 0x3000)
      bits = 1;
    else if (angle < shift - 0x1000 - sector)
      bits = 9;
    else if (angle < shift + sector + 0x1000)
      bits = 8;
    else if (angle < shift + 0x3000 - sector)
      bits = 10;
    else if (angle < shift + sector + 0x5000)
      bits = 2;
    else if (angle < shift + 0x7000 - sector)
      bits = 6;
    else
      bits = 4;
    u8 flags;
    if (!(value < dLib_at<STControl>(p)->field_0x04))
      flags = bits;
    else if (value < dLib_at<STControl>(p)->field_0x08)
      flags = 0;
    else
      flags = dLib_at<STControl>(p)->field_0x0c & ~bits;
    u8 previous = dLib_at<STControl>(p)->field_0x0d;
    dLib_at<STControl>(p)->field_0x0c = flags;
    if (flags != previous) {
      if (!flags)
        dLib_at<STControl>(p)->field_0x22 = 0;
      else
        dLib_at<STControl>(p)->field_0x22 =
            (angle & 0x1fff) > 0x1000 ? dLib_at<STControl>(p)->field_0x24
                                      : -dLib_at<STControl>(p)->field_0x24;
    }
    if (!(dLib_at<STControl>(p)->field_0x0c & 3))
      STControl_Xinit(p);
    if (!(dLib_at<STControl>(p)->field_0x0c & 12))
      STControl_Yinit(p);
  }
  u8 flags = dLib_at<STControl>(p)->field_0x0c,
     overlap = dLib_at<STControl>(p)->field_0x0d & flags;
  if (overlap & 3) {
    s16 timer = dLib_at<STControl>(p)->field_0x0e;
    if (timer > 0) {
      overlap =
          dLib_at<STControl>(p)->field_0x0d & dLib_at<STControl>(p)->field_0x0c;
      dLib_at<STControl>(p)->field_0x0e = timer - 1;
    }
  }
  if (overlap & 12) {
    s16 timer = dLib_at<STControl>(p)->field_0x10;
    if (timer > 0) {
      flags = dLib_at<STControl>(p)->field_0x0c;
      dLib_at<STControl>(p)->field_0x10 = timer - 1;
    }
  }
  return flags;
}

/* 025880B4 */
s32 dLib_checkActorInCircle(u32 center, u32 actor, f64 radius, f64 height) {
  gabi::Local<cXyz> diff, flat;
  dLib_vectorSubtract(center, gabi::ea(diff.get()), gabi::ea(&dLib_at<fopAc_ac_c>(actor)->current.pos));
  f32 x = dLib_at<cXyz>(gabi::ea(diff.get()))->x, zero = l_zero,
      z = dLib_at<cXyz>(gabi::ea(diff.get()))->z;
  dLib_at<cXyz>(gabi::ea(flat.get()))->y = zero;
  dLib_at<cXyz>(gabi::ea(flat.get()))->x = x;
  dLib_at<cXyz>(gabi::ea(flat.get()))->z = z;
  f64 squared = dLib_vectorSquared(gabi::ea(flat.get()));
  f64 distance = dLib_sqrt(squared);
  f32 ay = dLib_at<fopAc_ac_c>(actor)->current.pos.y,
      cy = dLib_at<cXyz>(center)->y;
  f32 delta = fsubs_ppc(cy, ay);
  return distance < radius && std::fabs(delta) < height;
}

/* 025881A4 */
s32 dLib_checkPlayerInCircle(u32 center, f64 radius, f64 height) {
  u32 game = dLib_getGame();
  f32 y = dLib_at<cXyz>(center)->y;
  u32 actor = gabi::ea(dLib_at<dComIfG_inf_c>(game)->play.mpPlayerPtr[0].get());
  f32 x = dLib_at<cXyz>(center)->x, z = dLib_at<cXyz>(center)->z;
  gabi::Local<cXyz> pos;
  dLib_at<cXyz>(gabi::ea(pos.get()))->y = y;
  dLib_at<cXyz>(gabi::ea(pos.get()))->x = x;
  dLib_at<cXyz>(gabi::ea(pos.get()))->z = z;
  return dLib_checkActorInCircle(gabi::ea(pos.get()), actor, radius, height);
}

/* 02588460 */
void dLib_get_QuatFromTriangle(u32 out, u32 a, u32 b, u32 c) {
  gabi::Local<cXyz> tmp, e1, e2, cross;
  gabi::Local<Quaternion> result;
  dLib_vectorSubtract(b, gabi::ea(tmp.get()), a);
  dLib_copyVector(gabi::ea(e1.get()), gabi::ea(tmp.get()));
  dLib_vectorSubtract(c, gabi::ea(tmp.get()), a);
  dLib_copyVector(gabi::ea(e2.get()), gabi::ea(tmp.get()));
  dLib_vectorCross(gabi::ea(e1.get()), gabi::ea(tmp.get()), gabi::ea(e2.get()));
  dLib_copyVector(gabi::ea(cross.get()), gabi::ea(tmp.get()));
  dLib_quatRotBaseY2(gabi::ea(result.get()), gabi::ea(cross.get()));
  u32 x = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->x,
      y = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->y;
  dLib_at<dLib_layout::quaternionWords>(out)->x = x;
  dLib_at<dLib_layout::quaternionWords>(out)->y = y;
  u32 w = dLib_at<dLib_layout::quaternionWords>(gabi::ea(result.get()))->w,
      z = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->z;
  dLib_at<dLib_layout::quaternionWords>(out)->w = w;
  dLib_at<dLib_layout::quaternionWords>(out)->z = z;
}

/* 02588544 */
void dLib_calc_QuatFromTriangle(u32 out, u32 a, u32 b, u32 c, f64 t) {
  gabi::Local<Quaternion> q, d;
  dLib_get_QuatFromTriangle(gabi::ea(q.get()), a, b, c);
  dLib_quatSlerp(out, gabi::ea(q.get()), gabi::ea(d.get()), t);
  u32 z = dLib_at<dLib_layout::quaternionWords>(gabi::ea(d.get()))->z,
      x = dLib_at<dLib_layout::quaternionWords>(gabi::ea(d.get()))->x,
      y = dLib_at<dLib_layout::quaternionWords>(gabi::ea(d.get()))->y;
  dLib_at<dLib_layout::quaternionWords>(out)->x = x;
  dLib_at<dLib_layout::quaternionWords>(out)->y = y;
  u32 w = dLib_at<dLib_layout::quaternionWords>(gabi::ea(d.get()))->w;
  dLib_at<dLib_layout::quaternionWords>(out)->z = z;
  dLib_at<dLib_layout::quaternionWords>(out)->w = w;
}

/* 02588230 */
s32 dLib_checkActorInFan(u32 center, u32 actor, s32 facing, s32 angle_limit,
                         f64 radius, f64 height) {
  gabi::Local<cXyz> diff, flat;
  dLib_vectorSubtract(center, gabi::ea(diff.get()), gabi::ea(&dLib_at<fopAc_ac_c>(actor)->current.pos));
  f32 x = dLib_at<cXyz>(gabi::ea(diff.get()))->x, zero = l_zero,
      z = dLib_at<cXyz>(gabi::ea(diff.get()))->z;
  dLib_at<cXyz>(gabi::ea(flat.get()))->y = zero;
  dLib_at<cXyz>(gabi::ea(flat.get()))->x = x;
  dLib_at<cXyz>(gabi::ea(flat.get()))->z = z;
  f64 squared = dLib_vectorSquared(gabi::ea(flat.get())),
      distance = dLib_sqrt(squared);
  f32 ay = dLib_at<fopAc_ac_c>(actor)->current.pos.y,
      cy = dLib_at<cXyz>(center)->y;
  f64 delta = std::fabs(fsubs_ppc(cy, ay));
  s32 direction = dLib_targetAngleY(center, gabi::ea(&dLib_at<fopAc_ac_c>(actor)->current.pos));
  s32 difference = dLib_distanceAngleS(facing, direction);
  return distance < radius && delta < height && difference < angle_limit;
}

/* 02587BE0 */
void dLib_scaleAnime(u32 value, u32 table, s32 count, u32 index, f64 speed,
                     f64 maximum, f64 minimum) {
  s32 current = *dLib_at<be<s32>>(index);
  if (current < count) {
    f64 delta = dLib_addCalc(value, dLib_at<be<f32>>(table)[u32(current)],
                             speed, maximum, minimum);
    if (delta == l_zero) {
      s32 next = s32(*dLib_at<be<u32>>(index) - 1);
      *dLib_at<be<s32>>(index) = next < 0 ? 0 : next;
    }
  } else
    *dLib_at<be<u32>>(index) = u32(current) - 1;
}

/* 02587514 */
void dLib_debugDrawAxis(u32 matrix, f64 length) {
  gabi::Local<dLib_layout::sixVectors> inputs, outputs;
  u32 a = gabi::ea(inputs.get()), b = gabi::ea(outputs.get());
  f32 zero = l_zero;
  for (u32 i = 0; i < 18; i++)
    dLib_at<be<f32>>(a)[i] = (i == 0 || i == 4 || i == 8) ? f32(length) : zero;
  dLib_matrixCopy(matrix, u32(gabi::ea(&l_now)));
  for (u32 i = 0; i < 6; i++)
    dLib_matrixMultVec(u32(gabi::ea(&l_now)), a + i * 12, b + i * 12);
  if (!l_axisXGuard.mValue) {
    l_axisXGuard.mValue = 1;
    dLib_memcpy(u32(gabi::ea(&l_axisXColor)), u32(gabi::ea(&l_axisXSeed)),
                u32(4));
  }
  if (!l_axisYGuard.mValue) {
    l_axisYGuard.mValue = 1;
    dLib_memcpy(u32(gabi::ea(&l_axisYColor)), u32(gabi::ea(&l_axisYSeed)),
                u32(4));
  }
  if (!l_axisZGuard.mValue) {
    l_axisZGuard.mValue = 1;
    dLib_memcpy(u32(gabi::ea(&l_axisZColor)), u32(gabi::ea(&l_axisZSeed)),
                u32(4));
  }
}

/* 025872F4 */
void dLib_waveRot(u32 pos, u32 wave, f64 sway) {
  f32 spacing = l_waveSpacing, near = fsubs_ppc(dLib_at<cXyz>(pos)->z, spacing);
  f64 first = dLib_seaWave(dLib_at<cXyz>(pos)->x, near);
  f32 far = fadds_ppc(dLib_at<cXyz>(pos)->z, spacing);
  f64 second = dLib_seaWave(dLib_at<cXyz>(pos)->x, far);
  s16 ax = s16(-dLib_atan2s(f32(second - first), fsubs_ppc(far, near)));
  near = fsubs_ppc(dLib_at<cXyz>(pos)->x, spacing);
  first = dLib_seaWave(near, dLib_at<cXyz>(pos)->z);
  far = fadds_ppc(dLib_at<cXyz>(pos)->x, spacing);
  second = dLib_seaWave(far, dLib_at<cXyz>(pos)->z);
  s32 az = dLib_atan2s(f32(second - first), fsubs_ppc(far, near));
  dLib_addCalcAngleS2(wave, s32(ax), s32(10), s32(0x200));
  dLib_addCalcAngleS2(wave + 2, az, s32(10), s32(0x200));
  s16 angle_x = dLib_at<dLib_wave_c>(wave)->mAngX;
  f32 amplitude = l_waveAmplitude;
  s32 phase_x = dLib_at<dLib_wave_c>(wave)->mAnimX + 400;
  dLib_at<dLib_wave_c>(wave)->mAnimX = phase_x;
  s32 phase_z = dLib_at<dLib_wave_c>(wave)->mAnimZ + 430;
  dLib_at<dLib_wave_c>(wave)->mAnimZ = phase_z;
  f32 sum = f32(sway + amplitude);
  sum = fadds_ppc(sum, l_waveTuning);
  f32 sin = dLib_sine(phase_x);
  dLib_at<dLib_wave_c>(wave)->mRotX = s16(ftoi(fmadds(sin, sum, f32(angle_x))));
  s16 angle_z = dLib_at<dLib_wave_c>(wave)->mAngZ;
  f32 cos = dLib_cosine(phase_z);
  dLib_at<dLib_wave_c>(wave)->mRotZ = s16(ftoi(fmadds(cos, sum, f32(angle_z))));
}

/* 025877F8 */
void dLib_setAnm(u32 arc, u32 model, u32 animation, u32 parameter, u32 previous,
                 u32 indices, u32 parameters, u32 force) {
  u32 secondary = dLib_stackSecondary();
  dLib_animations(arc, model, animation, parameter, previous, indices,
                  parameters, force, secondary, true);
}

/* 02587A0C */
void dLib_bcks_setAnm(u32 arc, u32 model, u32 animation, u32 parameter,
                      u32 previous, u32 indices, u32 parameters, u32 force) {
  u32 secondary = dLib_stackSecondary();
  dLib_animations(arc, model, animation, parameter, previous, indices,
                  parameters, force, secondary, false);
}

/* 02587EFC */
void dLib_setNextStageBySclsNum(u32 index, s32 room) {
  u32 manager;
  if (room == -1)
    manager = gabi::ea(&dLib_at<dComIfG_inf_c>(dLib_getGame())->play.mStageData);
  else {
    if (u32(room) >= 64)
      dLib_assert(u32(gabi::ea(&l_stageFile)), u32(0x1e3),
                  u32(gabi::ea(&l_roomCondition)));
    u32 game = dLib_getGame();
    manager = dLib_roomStatus(gabi::ea(&dLib_at<dComIfG_inf_c>(game)->play.mRoomCtrl), room);
  }
  u32 table = dLib_at<dLib_layout::stageData>(manager)->mpVtable.v,
      info = gabi::call_ptr<u32>(
          dLib_at<dLib_layout::stageVtable>(table)->getSclsInfo, manager);
  if (!info)
    return;
  u32 entries =
      dLib_at<dLib_layout::stage_scls_info_dummy_class>(info)->m_entries.v;
  if (!entries)
    dLib_assert(u32(gabi::ea(&l_stageFile)), u32(0x1ed),
                u32(gabi::ea(&l_stageEntriesCondition)));
  if (s32(index) >=
      dLib_at<dLib_layout::stage_scls_info_dummy_class>(info)->num)
    dLib_assert(u32(gabi::ea(&l_stageFile)), u32(0x1ee),
                u32(gabi::ea(&l_stageIndexCondition)));
  u32 entry = entries + index * 12,
      wipe = dLib_at<dLib_layout::stage_scls_info_class>(entry)->mWipe & 15;
  s32 targetroom = dLib_at<dLib_layout::stage_scls_info_class>(entry)->mRoom;
  u32 start = dLib_at<dLib_layout::stage_scls_info_class>(entry)->mStart;
  dLib_setNextStage(entry, start, targetroom, s32(-1), u32(0), u32(1), wipe,
                    l_zero);
}

/* 02587D24 */
void dLib_pathMove(u32 pos, u32 point, u32 path, u32 callback, u32 data,
                   f64 speed) {
  if (!path)
    dLib_assert(u32(gabi::ea(&l_pathFile)), u32(0x1b7),
                u32(gabi::ea(&l_pathCondition)));
  u16 count = dLib_at<dLib_layout::dPath>(path)->m_num;
  s8 current = *dLib_at<be<s8>>(point),
     next = current < s32(count) - 1 ? s8(current + 1) : 0;
  u32 points = dLib_at<dLib_layout::dPath>(path)->m_points.v;
  gabi::Local<cXyz> a, b, diff, normal, move, scaled, result;
  u32 begin = points + u32(s32(current)) * 16 + 4,
      end = points + u32(s32(next)) * 16 + 4;
  for (u32 i = 0; i < 3; i++)
    dLib_at<be<f32>>(gabi::ea(a.get()))[i] = dLib_at<be<f32>>(begin)[i];
  for (u32 i = 0; i < 3; i++)
    dLib_at<be<f32>>(gabi::ea(b.get()))[i] = dLib_at<be<f32>>(end)[i];
  dLib_vectorSubtract(gabi::ea(b.get()), gabi::ea(diff.get()),
                      gabi::ea(a.get()));
  dLib_vectorNormZP(gabi::ea(diff.get()), gabi::ea(normal.get()));
  dLib_copyVector(gabi::ea(move.get()), gabi::ea(normal.get()));
  if (callback) {
    if (gabi::call_ptr<u32>(callback, pos, gabi::ea(a.get()), gabi::ea(b.get()),
                            data))
      *dLib_at<be<s8>>(point) = next;
    return;
  }
  dLib_vectorScale(gabi::ea(move.get()), gabi::ea(scaled.get()), speed);
  dLib_vectorAdd(pos, gabi::ea(result.get()), gabi::ea(scaled.get()));
  u32 x = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->x,
      z = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->z;
  dLib_at<dLib_layout::vectorWords>(pos)->x = x;
  u32 y = dLib_at<dLib_layout::vectorWords>(gabi::ea(result.get()))->y;
  dLib_at<dLib_layout::vectorWords>(pos)->z = z;
  dLib_at<dLib_layout::vectorWords>(pos)->y = y;
  f64 sq = dLib_vectorSquaredDistance(gabi::ea(a.get()), pos),
      distance = dLib_sqrt(sq);
  sq = dLib_vectorSquaredDistance(gabi::ea(a.get()), gabi::ea(b.get()));
  f64 total = dLib_sqrt(sq);
  if (distance > total) {
    u32 y = dLib_at<dLib_layout::vectorWords>(gabi::ea(b.get()))->y;
    *dLib_at<be<s8>>(point) = next;
    x = dLib_at<dLib_layout::vectorWords>(gabi::ea(b.get()))->x;
    z = dLib_at<dLib_layout::vectorWords>(gabi::ea(b.get()))->z;
    dLib_at<dLib_layout::vectorWords>(pos)->x = x;
    dLib_at<dLib_layout::vectorWords>(pos)->z = z;
    dLib_at<dLib_layout::vectorWords>(pos)->y = y;
  }
}

/* 02588DEC */
f64 dLib_lerp(f64 a, f64 b, f64 t) {
  f32 delta = f32(b - a);
  u64 bits;
  std::memcpy(&bits, &t, 8);
  bits = (bits & 0xfffffffff8000000ull) + (bits & 0x8000000ull);
  std::memcpy(&t, &bits, 8);
  return f32(f64(delta) * t + a);
}

/* 0258839C */
u32 dLib_getIplDaysFromSaveTime() {
  u32 game = gabi::ea(l_gameInfo.mGame.get()),
      hi = dLib_at<dLib_layout::savedGame>(game)->mDateIPL[0],
      lo = dLib_at<dLib_layout::savedGame>(game)->mDateIPL[1];
  if (!(hi | lo))
    return 0;
  u32 nowhi = dLib_getTimeHigh(), nowlo = cpu->r[4];
  game = gabi::ea(l_gameInfo.mGame.get());
  u64 saved = (u64(dLib_at<dLib_layout::savedGame>(game)->mDateIPL[0])
               << 32) |
              dLib_at<dLib_layout::savedGame>(game)->mDateIPL[1];
  u64 elapsed = ((u64(nowhi) << 32) | nowlo) - saved;
  u32 info = dLib_systemInfo();
  u64 ticks = u64(dLib_at<dLib_layout::systemInfo>(info)->mBusClock >> 2) * 60 *
              60 * 24;
  dLib_divideTicks(u32(elapsed >> 32), u32(elapsed), u32(ticks >> 32),
                   u32(ticks));
  return cpu->r[4];
}

/* 02588D40 */
void dLib_staticInit() {
  l_headerStatics.mZeroWords[2] = 0;
  l_headerStatics.mZeroWords[0] = 0;
  l_headerStatics.mZeroWords[3] = 0;
  l_headerStatics.mZeroWords[1] = 0;
  dLib_registerStatic(gabi::ea(&l_records.mFirst));
  f32 negativePi = l_negativePi, pi = l_pi;
  l_headerStatics.mNegativePi = negativePi;
  l_headerStatics.mPi = pi;
  dLib_initFlag(gabi::ea(&l_headerStatics.mFlag));
  dLib_registerStatic(gabi::ea(&l_records.mSecond));
  dLib_initByte(gabi::ea(&l_headerStatics.mByte));
  dLib_registerStatic(gabi::ea(&l_records.mThird));
}

/* WWHD glue (tools/verify/autoglue.py)
ext 020079B4 f64 dLib_padStickValue(u32)
ext 02007A1C s32 dLib_padStickAngle(u32)
ext 02008974 f64 dLib_groundCross(u32, u32)
ext 0200ECD4 f64 dLib_addCalc(u32, f32, f64, f64, f64)
ext 0246BA0C f64 dLib_seaWave(f32, f32)
ext 028E8DD0 f64 dLib_vectorSquared(u32)
ext 028E8DE8 f64 dLib_vectorSquaredDistance(u32, u32)
ext 028F4384 f64 dLib_sqrt(f64)
ext 0200F93C s32 dLib_targetAngleY(u32, u32)
ext 0200FAAC s32 dLib_distanceAngleS(s32, s32)
ext 020195B0 s32 dLib_atan2s(f32, f32)
ext 0246B6A4 u32 dLib_seaArea(f32, f32)
ext 025200D4 u32 dLib_getGame()
ext 025885C4 u32 STControl_setWaitParmResult(u32, s32, s32, s32, s32, s32, s32, f64, f64)
ext 025885E8 u32 STControl_initResult(u32)
ext 025B8B94 u32 dLib_isEventBit(u32, u32)
ext 025C11DC u32 dLib_roomStatus(u32, s32)
ext 025E7CE0 u32 dLib_btkInitialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32)
ext 025E8154 u32 dLib_brkInitialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32)
ext 026066C4 u32 dLib_getResource(u32, u32, u32)
ext 0273AD10 u32 dLib_alloc(u32)
ext C0009C80 u32 dLib_systemInfo()
ext C0009CD8 u32 dLib_getTimeHigh()
ext 02008DAC void dLib_groundDestroy(u32, u32)
ext 02008E0C void dLib_groundConstruct(u32)
ext 0200F428 void dLib_addCalcAngleS2(u32, s32, s32, s32)
ext 0201AD78 void dLib_vectorAdd(u32, u32, u32)
ext 0201ADE0 void dLib_vectorSubtract(u32, u32, u32)
ext 0201AE48 void dLib_vectorScale(u32, u32, f64)
ext 0201B080 void dLib_vectorCross(u32, u32, u32)
ext 0201B12C void dLib_vectorNormZP(u32, u32)
ext 02312548 void dLib_quatRotBaseY2(u32, u32)
ext 0252012C void dLib_setNextStage(u32, u32, s32, s32, u32, u32, u32, f32)
ext 025B8B68 void dLib_onEventBit(u32, u32)
ext 025E4A98 void dLib_morfSetAnm(u32, u32, u32, u32, f32, f32, f32, f32)
ext 025F1C28 void dLib_matrixYrotM(u32, s32)
ext 025F24E0 void dLib_matrixTransM(f32, f32, f32)
ext 0273AA24 void dLib_assert(u32, u32, u32)
ext 0273AF40 void dLib_free(u32, u32)
ext 028E8F64 void dLib_matrixMultVec(u32, u32, u32)
ext 028E90D4 void dLib_matrixCopy(u32, u32)
ext 028E93CC void dLib_matrixTrans(u32, f32, f32, f32)
ext 028E9BC0 void dLib_quatSlerp(u32, u32, u32, f64)
ext 028EAB2C void dLib_initByte(u32)
ext 028ED6F8 void dLib_initFlag(u32)
ext 028F026C void dLib_registerStatic(u32)
ext 028F5B8C void dLib_divideTicks(u32, u32, u32, u32)
ext C000A848 void dLib_memcpy(u32, u32, u32)
data l_zero 10050690
data l_noGround 10050694
data l_waveSpacing 10050698
data l_waveAmplitude 1005069C
data l_anmInitialFrame 100506A8
data l_anmEndFrame 100506E4
data l_groundRaise 10050778
data l_negativePi 1005077C
data l_pi 10050780
data l_epsilon 100030B8
data l_waveTuning 1047BCD0
data l_safeStringVtable 10050628
data l_passBaseVtable 10050640
data l_groundChkVtable 10050650
data l_groundPolyVtable 10050660
data l_groupPassVtable 10050670
data l_polyPassVtable 10050680
data l_stVtable 10050788
data l_brkFile 100506AC
data l_brkCondition 100506B8
data l_btkFile 100506C4
data l_btkCondition 100506D0
data l_stageFile 10050734
data l_roomCondition 10050740
data l_stageEntriesCondition 10050760
data l_stageIndexCondition 10050708
data l_pathFile 100506E8
data l_pathCondition 100506F4
data l_resources 101F4F28
data l_gameInfo 101F84DC
data l_now 1048D0CC
data l_headerStatics 10477648
data l_records 101E9C14
data l_axisXGuard 101FDAC0
data l_axisYGuard 101FDA48
data l_axisZGuard 101FDA50
data l_axisXColor 101FEBF8
data l_axisYColor 101FEBEC
data l_axisZColor 101FEBF4
data l_axisXSeed 10050618
data l_axisYSeed 1005061C
data l_axisZSeed 10050620
*/
