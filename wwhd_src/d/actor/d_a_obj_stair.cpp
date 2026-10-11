/** Crumbling stair. GameCube-style source verified against the HD original. */
#include "d/actor/d_a_obj_stair.h"
#include "gc_api/gc_api.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002F6BC,
    .headerStatics = {0x1046BEF8, 0x101CCD18, 0x1046BF04},
};
static const char *M_arcname = "Mkdan";
static daobj_stairHIO_c l_HIO;
static dCcD_SrcCps cps_src;
static StairQuaternionWordsHD l_identityQuaternion;
enum { JA_SE_OBJ_BREAK_STEPS = 0x692D };
enum { fpcNm_PLAYER_e = 0xA8 };

void mDoMtx_quatMultiply(const Quaternion *left, const Quaternion *right,
                         Quaternion *result);
void stairConstructGObjInf(StairCapsuleHD *capsule);
void stairConstructCapsule(StairCapsuleGeometryHD *capsule);
void stairSetCapsule(StairCapsuleHD *capsule, const dCcD_SrcCps *source);
void stairSetCapsuleStartEnd(StairCapsuleGeometryHD *capsule, const cXyz *start,
                             const cXyz *end);
void stairDeleteCapsule(StairCapsuleHD *capsule, s32 flags);
void stairDeleteActor(fopAc_ac_c *actor, s32 flags);

// HD: these callers retain the float-rounded sqrt result; cXyz::abs keeps a
// double.
static f32 stairAbs(const cXyz *vector) {
  return std_sqrtf(PSVECSquareMag(vector));
}
static void copyQuaternionWords(Quaternion *destination,
                                const Quaternion *source) {
  auto *src = reinterpret_cast<const StairQuaternionWordsHD *>(source);
  auto *dst = reinterpret_cast<StairQuaternionWordsHD *>(destination);
  // GHS copies each word before loading the next, preserving signalling NaNs.
  for (u32 i = 0; i < 4; ++i)
    dst->mWords[i] = src->mWords[i];
}
/* inlined into _create */
static void constructCapsule(StairCapsuleHD *capsule) {
  stairConstructGObjInf(capsule);
  capsule->mShapeAttr.mShapeVtable = 0x100015A8;
  capsule->mShapeAttr.mBoundsVtable = 0x1002F6D4;
  stairConstructCapsule(&capsule->mCapsule);
  capsule->mShapeAttr.mShapeVtable = 0x1004AF70;
  capsule->__vtbl_hitinf = 0x1004AF18;
  capsule->mCapsule.mVtable = 0x1004AF60;
}

/* 02391570 */
static daobj_stairHIO_c *daobj_stairHIO_c_ct(daobj_stairHIO_c *h) {
  if (h == nullptr) {
    h = (daobj_stairHIO_c *)operator_new(sizeof(daobj_stairHIO_c));
    if (h == nullptr)
      return h;
  }
  h->mNo = -1;
  h->m10 = 5.0f;
  h->__vtbl = 0x1002F6E4;
  h->m14 = 2.5f;
  h->m08 = 2.5f;
  // HD: the falling-speed limit is -175 instead of -70.
  h->m0C = -175.0f;
  h->m18 = 6;
  return h;
}

/* 023905F4 */
static void ride_call_back(dBgW *i_arg0, fopAc_ac_c *i_arg1,
                           fopAc_ac_c *i_arg2) {
  daObj_Stair_c *i_this = (daObj_Stair_c *)i_arg1;
  // HD: profile checks tolerate a null actor.
  if (i_arg2 != nullptr && (s16)i_arg2->mProfName == fpcNm_PLAYER_e &&
      i_this->field_0x2E0 < l_HIO.m18) {
    i_this->field_0x2E4 = 1;
    cXyz d;
    d = i_arg2->current.pos - i_arg1->current.pos;
    i_this->field_0x2C8.copy(d);
    i_this->field_0x2D4.x = 0.0f;
    i_this->field_0x2EC = 1.0f;
    i_this->field_0x2D4.y = -1.0f;
    i_this->field_0x2D4.z = 0.0f;
  }
}

/* 02391248 */
BOOL daObj_Stair_c::CreateHeap() {
  J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
      M_arcname, dRes_INDEX_MKDAN_BDL_MKDAN1_e);
  if (modelData == nullptr) /* JUT_ASSERT(0xD1, modelData != NULL) */
    JUTAssertion::showAssert("d_a_obj_stair.cpp", 0xD1, "modelData != (0)");
  mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
  if (l_HIO.mNo < 0) {
    l_HIO.mNo = mDoHIO_createChild("崩れる階段", &l_HIO);
  }
  return mpModel != nullptr;
}

/* 0239130C */
BOOL daObj_Stair_c::Create() {
  fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
  return TRUE;
}

/* 02391328 */
BOOL daObj_Stair_c::Execute(Mtx34 **mtx) {
  Quaternion quat;
  mDoMtx_quatMultiply(&field_0x4A0, &field_0x4B0, &quat);
  copyQuaternionWords(&field_0x4A0, &quat);

  mpModel->setBaseScale(scale);
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y + field_0x2E8,
                         current.pos.z);
  mDoMtx_stack_c::quatM(&field_0x4A0);
  if (field_0x2E4 == 1) {
    cXyz temp;
    temp.set(0.0f, 0.0f, 0.0f);
    f32 r = cM_rndFX(175.0f);
    temp.x = 0.0f + r;
    f32 r2 = cM_rndFX(40.0f);
    temp.z = temp.z + r2;
    PSMTXMultVec(mDoMtx_stack_c::get(), &temp, &field_0x4C0);
  }
  mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
  PSMTXCopy(mDoMtx_stack_c::get(), &field_0x2FC);
  *reinterpret_cast<gptr<Mtx34> *>(mtx) = &field_0x2FC;
  return TRUE;
}

/* 023914D8 */
BOOL daObj_Stair_c::Draw() {
  settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
  dScnKy_env_light_c *env = dKy_getEnvlight();
  setLightTevColorType(env, mpModel, &tevStr);
  dComIfGd_setListBG();
  mDoExt_modelUpdateDL(mpModel);
  dComIfGd_setList();
  return TRUE;
}

/* 023916AC */
BOOL daObj_Stair_c::Delete() { return TRUE; }

/* 0239069C */
cPhs_State daObj_Stair_c::_create() {
  cPhs_State phase_state;
  if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
    if (gabi::ea(this) != 0) {
      dBgS_MoveBgActor::ct(this);
      __vtbl = 0x1002F7F8;
      dCcD_Stts_ct(&mStts);
      constructCapsule(&mCps);
    }
    fopAcM_OnCondition(this, fopAcCnd_INIT_e);
  }
  u32 switchIdx = fopAcM_GetParam(this) & 0xFF;

  if (switchIdx != 0xFF && fopAcM_isSwitch(this, switchIdx)) {
    return cPhs_STOP_e;
  }
  phase_state = dComIfG_resLoad(&mPhs, M_arcname);
  if (phase_state == cPhs_COMPLEATE_e) {
    // GHS loads the four identity-quaternion words before the native calls.
    u32 q0 = l_identityQuaternion.mWords[0],
        q1 = l_identityQuaternion.mWords[1];
    u32 q2 = l_identityQuaternion.mWords[2],
        q3 = l_identityQuaternion.mWords[3];
    f32 f29 = 0.0f;
    f32 sn = cM_ssin(current.angle.y >> 1);
    f32 cs = cM_scos(current.angle.y >> 1);
    phase_state =
        MoveBGCreate(M_arcname, dRes_INDEX_MKDAN_DZB_MKDAN1_e, 0, 0x8A0);
    if (!((phase_state == cPhs_COMPLEATE_e) ||
          (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x113, ...) */
      JUTAssertion::showAssert(
          "d_a_obj_stair.cpp", 0x113,
          "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");

    field_0x4A0.x = f29;
    field_0x4A0.y = sn;
    field_0x4A0.z = f29;
    field_0x4A0.w = cs;
    auto *words = reinterpret_cast<StairQuaternionWordsHD *>(&field_0x4B0);
    words->mWords[0] = q0;
    words->mWords[1] = q1;
    words->mWords[2] = q2;
    words->mWords[3] = q3;
    field_0x2E0 = 0;

    reinterpret_cast<StairBackgroundHD *>(mpBgW.get())
        ->SetRideCallback(ride_call_back);
    mStts.Init(0xFF, 0xFF, this);
    stairSetCapsule(&mCps, &cps_src);
    mCps.SetStts(&mStts);

    cXyz start;
    cXyz end;
    end.copy(current.pos);
    start.copy(end);
    s16 ay = current.angle.y;
    f32 c = cM_scos(ay), s = cM_ssin(ay);
    f32 ex = gabi::fmadds(-125.0f, c, current.pos.x);
    f32 ez = gabi::fmadds(125.0f, s, current.pos.z);
    start.x = gabi::fmadds(125.0f, c, start.x);
    start.z = gabi::fmadds(-125.0f, s, start.z);
    end.x = ex;
    end.z = ez;
    stairSetCapsuleStartEnd(&mCps.mCapsule, &start, &end);
    field_0x2E8 = 0.0f;
    field_0x2EC = 0.0f;
    mCps.mCapsule.mRadius = 50.0f;
  }
  return phase_state;
}

/* 02390A58 */
bool daObj_Stair_c::_delete() {
  if (l_HIO.mNo >= 0) {
    mDoHIO_deleteChild(l_HIO.mNo);
    l_HIO.mNo = -1;
  }
  dComIfG_resDelete(&mPhs, M_arcname);
  return MoveBGDelete() != 0;
}

/* 02390AC4 */
bool daObj_Stair_c::_execute() {
  if (field_0x2E4 == 1) {
    if (field_0x2E0 == l_HIO.m18) {
      cXyz temp;
      temp = field_0x2C8.normZP();
      cXyz op;
      op = temp.outprod(field_0x2D4);
      cXyz temp2;
      temp2.copy(op);

      if (stairAbs(&temp2) > 8e-11f) {
        cXyz temp3;
        temp3 = temp2.normZP();
        f32 len = stairAbs(&field_0x2C8);
        f32 f2EC = field_0x2EC;
        f32 temp4 = l_HIO.m10 * 0.5f * len * 0.01f * f2EC;
        s16 ang = (s16)gabi::ftoi(temp4 * 182.04444885253906f); /* DEG2S */
        f32 sn = cM_ssin(ang);
        f32 cs = cM_scos(ang);

        field_0x4B0.w = cs;
        field_0x4B0.y = sn * temp3.y;
        field_0x4B0.x = sn * temp3.x;
        field_0x4B0.z = sn * temp3.z;

        speed.x = l_HIO.m14 * cs * f2EC * field_0x2D4.x;
        speed.y = l_HIO.m14 * cs * f2EC * field_0x2D4.y;
        speed.z = l_HIO.m14 * cs * field_0x2EC * field_0x2D4.z;
      } else {
        field_0x4B0.y = 0.0f;
        field_0x4B0.z = 0.0f;
        field_0x4B0.x = 0.0f;
        field_0x4B0.w = 1.0f;

        f32 f2EC = field_0x2EC;
        speed.x = f2EC * l_HIO.m14 * field_0x2D4.x;
        speed.y = f2EC * l_HIO.m14 * field_0x2D4.y;
        speed.z = f2EC * l_HIO.m14 * field_0x2D4.z;
      }
      /* fopAcM_seStart(this, JA_SE_OBJ_BREAK_STEPS, 0) */
      s32 reverb = dComIfGp_getReverb(current.roomNo);
      mDoAud_seStart(JA_SE_OBJ_BREAK_STEPS, &eyePos, 0, reverb);
    }

    field_0x2E0 = field_0x2E0 + 1;

    if (field_0x2E0 > l_HIO.m18) {
      cLib_addCalc2(&speed.y, l_HIO.m0C, 0.5f, l_HIO.m08);
      current.pos.y = current.pos.y + speed.y;
    }
    if (field_0x2E0 > l_HIO.m18 + 0xF0) {
      fopAcM_delete(this);
    }
  } else {
    dComIfG_Ccsp_Set(&mCps);

    if (mCps.ChkTgHit()) {
      fopAc_ac_c *ac = mCps.GetTgHitAc();
      if (ac) {
        // HD: profile checks tolerate a null actor.
        if (fopAc_IsActor(ac) && ac != nullptr &&
            (s16)ac->mProfName == fpcNm_BOMB_e) {
          f32 sn = cM_ssin(current.angle.y);
          f32 cs = cM_scos(current.angle.y);
          field_0x2E4 = 1;
          cXyz d;
          d = ac->current.pos - current.pos;
          field_0x2C8.copy(d);
          d = current.pos - ac->current.pos;
          cXyz temp4;
          cXyz temp5;
          temp4.set(cs, 0.0f, -sn);
          temp5.set(sn, 0.0f, cs);
          field_0x2D4.copy(d);
          f32 dot1 = PSVECDotProduct(&temp4, &field_0x2D4);
          f32 dot2 = PSVECDotProduct(&temp5, &field_0x2D4);
          if (dot1 > 175.0f) {
            field_0x2D4.x = gabi::fnmsubs(175.0f, cs, field_0x2D4.x);
            field_0x2D4.z = gabi::fnmsubs(-175.0f, sn, field_0x2D4.z);
          } else if (dot1 < -175.0f) {
            field_0x2D4.z = gabi::fmadds(-175.0f, sn, field_0x2D4.z);
            field_0x2D4.x = gabi::fmadds(175.0f, cs, field_0x2D4.x);
          } else {
            field_0x2D4.x = gabi::fnmsubs(dot1, cs, field_0x2D4.x);
            field_0x2D4.z = gabi::fmadds(dot1, sn, field_0x2D4.z);
          }

          if (dot2 > 40.0f) {
            field_0x2D4.x = gabi::fnmsubs(40.0f, sn, field_0x2D4.x);
            field_0x2D4.z = gabi::fnmsubs(40.0f, cs, field_0x2D4.z);
          } else if (dot2 < -40.0f) {
            field_0x2D4.x = gabi::fmadds(40.0f, sn, field_0x2D4.x);
            field_0x2D4.z = gabi::fmadds(40.0f, cs, field_0x2D4.z);
          } else {
            field_0x2D4.x = gabi::fnmsubs(dot2, sn, field_0x2D4.x);
            field_0x2D4.z = gabi::fnmsubs(dot2, cs, field_0x2D4.z);
          }
          d = field_0x2D4.normZP();
          field_0x2D4.copy(d);
          f32 a2 = PSVECSquareMag(&field_0x2C8);
          s16 sub = (s16)gabi::ftoi(0.0002f * a2);
          field_0x2E0 = l_HIO.m18 - sub;
          if (stairAbs(&field_0x2C8) < 80.0f) {
            field_0x2EC = 0.7f;
          } else {
            field_0x2EC = 56.0f / stairAbs(&field_0x2C8);
          }
        } else {
          field_0x2E4 = 1;
          cXyz d;
          d = *mCps.GetTgHitPosP() - current.pos;
          field_0x2C8.copy(d);
          field_0x2D4.copy(mCps.mGObjTg.mRVec); /* *mCps.GetTgRVecP() */
          d = field_0x2D4.normZP();
          field_0x2D4.copy(d);

          field_0x2E0 = l_HIO.m18 - 2;
          field_0x2EC = 1.0f;
        }
      }
    }
  }
  s32 temp5 = field_0x2E0;
  s32 m18 = l_HIO.m18;
  if (temp5 > m18 - 10 && temp5 < m18) {
    f32 sinResult = cM_ssin((((temp5 - m18) + 10) & 0x3) << 14);
    field_0x2E8 = 1.5f * sinResult;
  }
  return MoveBGExecute() != 0;
}

/* 02391214 */
static BOOL daObj_StairDraw(void *i_this) {
  return ((daObj_Stair_c *)i_this)->Draw_v() != FALSE;
}

/* 02391688 */
static BOOL daObj_StairIsDelete(void *i_this) { return TRUE; }

/* ---------------- HD: compiler-generated code of this translation unit
 * ---------------- */

/* 023915E8 */
static void __sinit_d_a_obj_stair_cpp() {
  __sinit_header_statics();
  daobj_stairHIO_c_ct(&l_HIO);
}

/* 02391690 */
static void trivial_dt(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}

/* 023916A4 */
static BOOL MoveBgActor_IsDelete(void *i_this) { return TRUE; }

/* 023916B4 */
static void daObj_Stair_c_dt(daObj_Stair_c *i_this, s32 flags) {
  if (i_this != nullptr) {
    stairDeleteCapsule(&i_this->mCps, 2);
    dCcD_Stts_dt(&i_this->mStts, 2);
    stairDeleteActor(i_this, 0);
    if (flags & 1)
      operator_delete(i_this);
  }
}

/* 02391720 */
static void empty_virtual(void *p) {}
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and private adapters
data l_HIO 1046BF14
data cps_src 101CCCCC
data l_identityQuaternion 1002F6F4
str 1002F7F0
str 1002F7A8 in 02391248
str 1002F704 in 0239069C
str 1002F7BC
str 1002F718
str 1002F7D0 "\u5d29\u308c\u308b\u968e\u6bb5" renamed # HD label is Shift-JIS; source uses readable UTF-8.
ext 028E8B48 void mDoMtx_quatMultiply(const Quaternion* left,const Quaternion* right,Quaternion* result)
ext 02515FB8 void stairConstructGObjInf(StairCapsuleHD* capsule)
ext 02018150 void stairConstructCapsule(StairCapsuleGeometryHD* capsule)
ext 025164C0 void stairSetCapsule(StairCapsuleHD* capsule,const dCcD_SrcCps* source)
ext 02018808 void stairSetCapsuleStartEnd(StairCapsuleGeometryHD* capsule,const cXyz* start,const cXyz* end)
ext 02515980 void stairDeleteCapsule(StairCapsuleHD* capsule,s32 flags)
ext 025D50BC void stairDeleteActor(fopAc_ac_c* actor,s32 flags)
*/
