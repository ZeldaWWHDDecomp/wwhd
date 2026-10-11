/* WWHD Dragonhead. Ported from zeldaret/tww with audited HD behavior.
 */
#include "d/actor/d_a_obj_dragonhead.h"
using namespace daObjDragonhead;
using daObjDragonhead::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x10027684};
static const char M_arcname[] = "Qdghd";
enum { DRAGONHEAD_VTBL=0x1002769C, dRes_INDEX_QDGHD_BDL_QDGHD_e=4,
       dRes_INDEX_QDGHD_DZB_QDGHD_e=7, JA_SE_OBJ_ICEBERG_BREATH=0x61CC,
       JA_SE_OBJ_ICEVERG_MELT=0x69D0, AT_TYPE_FIRE_ARROW=0x40000 };
static dCcD_SrcSph sph_check_src;
static DragonHeaderStatics headerStatics;
void constructSphere(dCcD_Sph*);
void destructSphere(dCcD_Sph*,s32);
DragonModelResource* getModelResource(J3DModelData*);
void convertColor(FloatColor*,const FloatColor*,f32);
FloatColor* getShaderParameter(DragonShaderParams*,u32);
void initializeHeaderA(u8*);
void initializeHeaderB(u8*);

/* 0233780C */
BOOL Act_c::CreateHeap() {
  auto* data = (J3DModelData*) dComIfG_getObjectRes(M_arcname, dRes_INDEX_QDGHD_BDL_QDGHD_e);
  if (!data)
    JUT_ASSERT_fail("d_a_obj_dragonhead.cpp", 0xA0, "model_data != 0");
  mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
  if (!mpModel)
    return FALSE;
  auto* matrix = mDoMtx_stack_c::get();
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  mDoMtx_stack_c::transS(x, y, z);
  mDoMtx_YrotM(matrix, shape_angle.y);
  x = scale.x;
  y = scale.y;
  z = scale.z;
  mDoMtx_stack_c::scaleM(x, y, z);
  PSMTXCopy(matrix, &mtx);
  mpBgW = new_dBgW();
  if (!mpBgW)
    return FALSE;
  auto* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_QDGHD_DZB_QDGHD_e);
  if (mpBgW->Set(dzb, 1, &mtx))
    return FALSE;
  return mpModel != nullptr;
}
/* 0233794C */
BOOL CheckCreateHeap(Act_c *actor) {
  return actor->CreateHeap();
}
/* 02337950 */
void Act_c::set_mtx() {
  // HD: scale is read completely before the first model write.
  f32 x=scale.x, y=scale.y, z=scale.z;
  J3DModel* model=mpModel;
  model->mBaseScale.x=x;
  model->mBaseScale.y=y;
  model->mBaseScale.z=z;
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
  model=mpModel;
  model->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 02337A28 */
void Act_c::CreateInit() {
  fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
  fopAcM_setCullSizeFar(this, 1.0f);
  set_mtx();
  field_0x40c=0;
  s8 room=home.roomNo;
  u32 switchNo=(mParameters >> 8) & 255;
  if (dComIfGs_isSwitch(switchNo, room)) {
    mAlpha=0;
    mSwitchOn=1;
  } else {
    u8 registered=field_0x40c;
    mSwitchOn=0;
    mAlpha=255;
    if (!registered && !dComIfG_Bgsp()->Regist(mpBgW,this))
      field_0x40c=1;
  }
  mStts.Init(255,255,this);
  mSph.Set(&sph_check_src);
  mSphCenter.y=950.0f;
  mSphCenter.z=220000.0f;
  mSphCenter.x=79338.0f;
  mSph.SetC(&mSphCenter);
  mSph.SetStts(&mStts);
}
/* 02337BB4 */
s32 Create(Act_c* actor) {
  u32 flags=actor->actor_condition;
  if (!(flags & fopAcCnd_INIT_e)) {
    if (actor) {
      fopAc_ac_c_ct(actor);
      actor->__vtbl=DRAGONHEAD_VTBL;
      dCcD_Stts_ct(&actor->mStts);
      constructSphere(&actor->mSph);
      flags=actor->actor_condition;
    }
    actor->actor_condition=flags | fopAcCnd_INIT_e;
  }
  cPhs_State phase=dComIfG_resLoad(&actor->mPhs,M_arcname);
  if (phase==cPhs_COMPLEATE_e) {
    if (!fopAcM_entrySolidHeap(actor,(BOOL(*)(fopAc_ac_c*))CheckCreateHeap,0x10500))
      return cPhs_ERROR_e;
    actor->CreateInit();
  }
  return phase;
}
/* 02337CAC */
BOOL Delete(Act_c* actor) {
  if (actor->heap && actor->field_0x40c)
    dComIfG_Bgsp()->Release(actor->mpBgW);
  mDoAud_seDeleteObject(&actor->mSphCenter);
  dComIfG_resDelete(&actor->mPhs,M_arcname);
  return TRUE;
}
/* 02337D18 */
void colorToFloat(FloatColor *out, GXColor *in) {
  f32 r=in->r, g=in->g, b=in->b, a=in->a;
  out->r = r / 255.0f;
  out->g = g / 255.0f;
  out->b = b / 255.0f;
  out->a = a / 255.0f;
}
/* 02337DCC */
BOOL Draw(Act_c* actor) {
  g_env_light.settingTevStruct(1,&actor->current.pos,&actor->tevStr);
  g_env_light.setLightTevColorType(actor->mpModel,&actor->tevStr);
  dComIfGd_setListBG();
  J3DModelData* data=actor->mpModel->getModelData();
  u32 count=getModelResource(data)->mMaterialNum;
  u16 index=0;
  while (count) {
    auto* material=reinterpret_cast<DragonMaterialView*>(data->getMaterialNodePointer(index));
    GXColor* color=material->mTevBlock->getTevKColor(3);
    color->a=actor->mAlpha;
    // HD: preserve the repeated virtual lookups and write back the color.
    color=material->mTevBlock->getTevKColor(3);
    material->mTevBlock->setTevKColor(3,color);
    // HD: convert the byte color and upload the linear shader parameter.
    FloatColor input;
    FloatColor output;
    colorToFloat(&input,color);
    convertColor(&output,&input,1.0f);
    material->mShaderParams.flags |= 0x400;
    FloatColor* dest=getShaderParameter(&material->mShaderParams,10);
    f32 alpha=color->a, r=output.r, g=output.g, b=output.b;
    dest->g=g;
    dest->b=b;
    dest->r=r;
    dest->a=alpha / 255.0f;
    ++index;
    --count;
  }
  mDoExt_modelUpdateDL(actor->mpModel);
  dComIfGd_setList();
  return TRUE;
}
/* 02337FA0 */
BOOL Execute(Act_c* actor) {
  dComIfG_Ccsp_Set(&actor->mSph);
  if (!actor->mSwitchOn) {
    s32 reverb=dComIfGp_getReverb(actor->current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_ICEBERG_BREATH,&actor->mSphCenter,0,reverb);
    if (actor->mSph.ChkTgHit()) {
      auto* hit=static_cast<dCcD_GObjInf*>(actor->mSph.GetTgHitObj());
      if (hit && (hit->mAtType & AT_TYPE_FIRE_ARROW)) {
        u32 parameters=actor->mParameters;
        s8 room=actor->home.roomNo;
        dComIfGs_onSwitch((parameters >> 8) & 255,room);
        actor->mSwitchOn=1;
      }
    }
  }
  u32 parameters=actor->mParameters;
  s8 room=actor->home.roomNo;
  if (dComIfGs_isSwitch((parameters >> 8) & 255,room)) {
    if (actor->field_0x40c) {
      s32 ret=dComIfG_Bgsp()->Release(actor->mpBgW);
      s8 soundRoom=actor->current.roomNo;
      if (!ret) actor->field_0x40c=0;
      s32 reverb=dComIfGp_getReverb(soundRoom);
      mDoAud_seStart(JA_SE_OBJ_ICEVERG_MELT,&actor->eyePos,0,reverb);
    }
    u8 alpha=actor->mAlpha;
    if (alpha) actor->mAlpha=alpha >= 2 ? alpha-2 : 0;
  } else {
    u8 registered=actor->field_0x40c;
    actor->mSwitchOn=0;
    if (!registered && !dComIfG_Bgsp()->Regist(actor->mpBgW,actor))
      actor->field_0x40c=1;
    u8 alpha=actor->mAlpha;
    if (alpha < 255) actor->mAlpha=alpha <= 253 ? alpha+2 : 255;
  }
  actor->set_mtx();
  return TRUE;
}
/* 0233818C */
void staticInitialize() {
  headerStatics.zero[2]=0;
  headerStatics.zero[0]=0;
  headerStatics.zero[3]=0;
  headerStatics.zero[1]=0;
  __register_global_object(0x101C8D88);
  headerStatics.piMin=-3.1415927410125732f;
  headerStatics.piMax=3.1415927410125732f;
  initializeHeaderA(&headerStatics.initializeA);
  __register_global_object(0x101C8D94);
  initializeHeaderB(&headerStatics.initializeB);
  __register_global_object(0x101C8DA0);
}
/* 02338220 */
void deleteStatic(void *p, s32 flags) {
  if (p && (flags & 1))
    operator_delete(p);
}
/* 02338234 */
BOOL IsDelete(Act_c *actor) {
  return TRUE;
}
/* 0233823C */
void destruct(Act_c *actor, s32 flags) {
  if (actor) {
    destructSphere(&actor->mSph,2);
    __dt(&actor->mStts);
    __dt(static_cast<fopAc_ac_c*>(actor));
    if (flags & 1)
      operator_delete(actor);
  }
}
/* 023382A8 */
void emptyVirtual() {}

/* WWHD glue
str 1002767C
str 100276AC in 0233780C
str 100276EC in 02337CAC
str 100276C4
str 100276B4
ext 025166F0 void constructSphere(dCcD_Sph*)
ext 02515AE8 void destructSphere(dCcD_Sph*,s32)
ext 027F3F8C DragonModelResource* getModelResource(J3DModelData*)
ext 0274D458 void convertColor(FloatColor*,const FloatColor*,f32)
ext 027F9F0C FloatColor* getShaderParameter(DragonShaderParams*,u32)
ext 028ED6F8 void initializeHeaderA(u8*)
ext 028EAB2C void initializeHeaderB(u8*)
data sph_check_src 101C8D28
data headerStatics 10469678
*/
