/* Volcano boiling lava: private port of the actual HD translation unit. */
#include "d/actor/d_a_obj_Yboil.h"
using namespace yboil_layout;
const gc_tu_data gc_tu = {.safeString=0x10033924,.headerStatics={0x1046CA50,0x101CDF7C,0x1046CA5C}};
static const char M_arcname[]="Yboil";
enum { YBOIL_VTBL=0x10033964, BCK_BASE_VTBL=0x1016E54C, BCK_VTBL=0x1003393C,
       CALC_ANM_VTBL=0x1016D820, LavaDisabledEvent=0x1902,
       BtkConstructor=0x025E7C6C, BrkConstructor=0x025E80D0,
       dRes_INDEX_YBOIL_BDL_YBOIL00_e=6, dRes_INDEX_YBOIL_BCK_YBOIL00_e=9,
       dRes_INDEX_YBOIL_BTK_YBOIL00_e=15, dRes_INDEX_YBOIL_BRK_YBOIL00_e=12 };

static u8* bckConstructorGuest(u8*);
static void bckDestructorGuest(u8*,u32);
void constructArray(void*,u32,u32,u32);
void destroyArray(void*,u32,u32,u32,u32,u32);
void initializeFrameCtrl(J3DFrameCtrl*,s32);
void constructMatrixCalc(void*);
void destructMatrixCalc(void*,s32);
void dKy_get_seacolor(GXColor*,GXColor*);
void convertColor(FloatColor*,const FloatColor*,f32);
FloatColor* getShaderParameter(ShaderParams*,u32);
// HD: array construction receives guest entry addresses for the shared constructors.

/* 023BC3D8 */
BOOL daObjYboil_c::CreateHeap() {
 auto* modelData=static_cast<J3DModelData*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_YBOIL_BDL_YBOIL00_e));
 if (!modelData) JUT_ASSERT_fail("d_a_obj_Yboil.cpp",0x5C,"modelData != (0)");
 auto* bck=static_cast<J3DAnmTransform*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_YBOIL_BCK_YBOIL00_e));
 if (!bck) JUT_ASSERT_fail("d_a_obj_Yboil.cpp",0x60,"bck != 0");
 auto* btk=static_cast<J3DAnmTextureSRTKey*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_YBOIL_BTK_YBOIL00_e));
 if (!btk) JUT_ASSERT_fail("d_a_obj_Yboil.cpp",0x64,"btk != 0");
 auto* brk=static_cast<J3DAnmTevRegKey*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_YBOIL_BRK_YBOIL00_e));
 if (!brk) JUT_ASSERT_fail("d_a_obj_Yboil.cpp",0x68,"brk != 0");
 for (u32 i=0;i<50;++i) {
  mModel[i]=mDoExt_J3DModel__create(modelData,0x80000,0x11000222);
  if (!mModel[i]) return FALSE;
  if (!mBckAnm[i].init(modelData,bck,true,J3DFrameCtrl::EMode_NONE,1.f,0,-1,false)) return FALSE;
  if (!mBtkAnm[i].init(modelData,btk,true,J3DFrameCtrl::EMode_NONE,1.f,0,-1,false,0)) return FALSE;
  if (!mBrkAnm[i].init(modelData,brk,true,J3DFrameCtrl::EMode_NONE,1.f,0,-1,false,0)) return FALSE;
 }
 return TRUE;
}
/* 023BC624 */
static BOOL CheckCreateHeap(daObjYboil_c* self) {
  return self->CreateHeap();
}
/* 023BC628 */
void daObjYboil_c::pos_reset(s32 i) {

 f32 spread=(f32)mScaleMax-(f32)mScaleMin;
 f32 size=cM_rndF(spread)+(f32)mScaleMin;
 mMdlScale[i].x=size; mMdlScale[i].y=size; mMdlScale[i].z=size;
 f32 distance=cM_rndF(1400.0f)+1300.0f;
 u16 angle=(u16)gabi::ftoi(cM_rndF(65536.0f));
 mMdlPos[i].x=gabi::fmadds(distance,cM_ssin(angle),current.pos.x);
 mMdlPos[i].y=current.pos.y;
 mMdlPos[i].z=gabi::fmadds(distance,cM_scos(angle),current.pos.z);
}
/* 023BC718 */
void daObjYboil_c::set_mtx() {

 for(u32 i=0;i<50;i++) {
  J3DModel_setBaseScale(mModel[i],&mMdlScale[i]);
  mDoMtx_stack_c::transS(mMdlPos[i].x,mMdlPos[i].y,mMdlPos[i].z);
  J3DModel_setBaseTRMtx(mModel[i],mDoMtx_stack_c::get());
 }
}
/* 023BC800 */
void daObjYboil_c::CreateInit() {

 mScaleMin=0.5f; mScaleMax=1.0f;
 for(u32 i=0;i<50;i++) {
  pos_reset(i);
  cullMtx=gabi::ea(J3DModel_getBaseTRMtx(mModel[i]));
  fopAcM_setCullSizeBox(this,-3000.0f,-0.0f,-3000.0f,3000.0f,200.0f,3000.0f);
  cullSizeFar=1.0f;
  f32 frame=(f32)gabi::ftoi(cM_rndF(29.999900817871094f));
  mBckAnm[i].mFrameCtrl.mFrame=frame; mBtkAnm[i].mFrameCtrl.mFrame=frame; mBrkAnm[i].mFrameCtrl.mFrame=frame;
  mMdlTimer[i]=0;
 }
 set_mtx();
}
/* 023BC9BC */
s32 daObjYboil_c::_create() {

 if(!(this->actor_condition & fopAcCnd_INIT_e)) {
  if(this) {
   fopAc_ac_c_ct(this); this->__vtbl=YBOIL_VTBL;
   constructArray(&this->mBckAnm[0],50,sizeof(mDoExt_bckAnm),gabi::guest_fn((const void*)bckConstructorGuest));
   constructArray(&this->mBtkAnm[0],50,sizeof(mDoExt_btkAnm),BtkConstructor);
   constructArray(&this->mBrkAnm[0],50,sizeof(YboilBrk),BrkConstructor);
  }
  this->actor_condition=(u32)this->actor_condition | fopAcCnd_INIT_e;
 }
 if(dComIfGs_isEventBit(LavaDisabledEvent)) return 3;
 s32 result=dComIfG_resLoad(&this->mPhs,M_arcname);
 if(result==4) {
  if(!fopAcM_entrySolidHeap(this,(BOOL(*)(fopAc_ac_c*))CheckCreateHeap,0x14500)) return 5;
  this->CreateInit();
 }
 return result;
}
/* 023BCB04 */
BOOL daObjYboil_c::_delete() {

 if (this->mCreateResult != cPhs_STOP_e) dComIfG_resDelete(&this->mPhs,M_arcname);
 return 1;
}
/* 023BCB40 */
static void normalizeColor(FloatColor* out,GXColor* color) {
 f32 r=color->r,g=color->g,b=color->b,a=color->a;
 out->r=r/255.f;
 out->g=g/255.f;
 out->b=b/255.f;
 out->a=a/255.f;
}
/* 023BCBF4 */
BOOL daObjYboil_c::_draw() {
 dComIfGd_setListBG();
 g_env_light.settingTevStruct(1,&this->current.pos,&this->tevStr);
 for (u32 i=0;i<50;++i) {
  g_env_light.setLightTevColorType(this->mModel[i],&this->tevStr);
  auto* data=reinterpret_cast<ModelData*>(this->mModel[i]->getModelData());
  Material* material=data->mJointNodes->mMesh;
  while (material) {
   // HD: retain the four color temporaries in their verified stack order.
   gabi::Local<GXColor> ambient,diffuse;
   gabi::Local<FloatColor> input,converted;
   dKy_get_seacolor(ambient.get(),diffuse.get());
   for (u32 component=0;component<3;++component) {
    auto* dest=reinterpret_cast<be<s16>*>(material->mTevBlock->getTevColor());
    auto* src=reinterpret_cast<be<u8>*>(ambient.get());
    u16 value=src[component];
    dest[component]=value;
   }
   GXColorS10* color=material->mTevBlock->getTevColor();
   material->mTevBlock->setTevColor(color);
   input->r=(f32)color->r/255.f;
   input->g=(f32)color->g/255.f;
   input->b=(f32)color->b/255.f;
   input->a=(f32)color->a/255.f;
   convertColor(converted.get(),input.get(),1.f);
   material->mShaderParams.flags |= 0x10;
   FloatColor* output=getShaderParameter(&material->mShaderParams,4);
   f32 alpha=(f32)color->a/255.f;
   // HD: green is copied as bits to preserve signaling NaNs; red/blue round via FPRs.
   f32 red=converted->r,blue=converted->b;
   u32 green=reinterpret_cast<FloatColorBits*>(converted.get())->g;
   reinterpret_cast<FloatColorBits*>(output)->g=green;
   output->b=blue;
   output->r=red;
   output->a=alpha;
   for (u32 component=0;component<3;++component) {
    auto* dest=reinterpret_cast<be<u8>*>(material->mTevBlock->getTevKColor());
    auto* src=reinterpret_cast<be<u8>*>(diffuse.get());
    u8 value=src[component];
    dest[component]=value;
   }
   GXColor* kColor=material->mTevBlock->getTevKColor();
   material->mTevBlock->setTevKColor(kColor);
   normalizeColor(input.get(),kColor);
   convertColor(converted.get(),input.get(),1.f);
   material->mShaderParams.flags |= 0x80;
   output=getShaderParameter(&material->mShaderParams,7);
   alpha=(f32)kColor->a/255.f;
   // HD: the second conversion rounds all RGB channels through FPRs.
   f32 redFloat=converted->r,greenFloat=converted->g,blueFloat=converted->b;
   output->g=greenFloat;
   output->b=blueFloat;
   output->r=redFloat;
   output->a=alpha;
   material=material->mNext;
  }
  J3DModelData* modelData=this->mModel[i]->getModelData();
  this->mBckAnm[i].entry(modelData,this->mBckAnm[i].getFrame());
  modelData=this->mModel[i]->getModelData();
  this->mBtkAnm[i].entry(modelData,this->mBtkAnm[i].getFrame());
  modelData=this->mModel[i]->getModelData();
  this->mBrkAnm[i].entry(modelData,this->mBrkAnm[i].getFrame());
  mDoExt_modelUpdateDL(this->mModel[i]);
 }
 dComIfGd_setList();
 return TRUE;
}
/* 023BD3F8 */
static u32 PrmAbstract(daObjYboil_c* self,u32 width,u32 shift) {

 u32 mask=width&32?0:1u<<(width&31);
 u32 value=shift&32?0:(u32)self->mParameters>>(shift&31);
 return value&(mask-1);
}
/* 023BD02C */
BOOL daObjYboil_c::_execute() {

 for(u32 i=0;i<50;i++) {
  if(this->mMdlTimer[i]==0) {
   this->mBckAnm[i].play();this->mBtkAnm[i].play();this->mBrkAnm[i].play();
   if(this->mBckAnm[i].mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || this->mBckAnm[i].mFrameCtrl.mRate==0.0f) {
    this->mBckAnm[i].mFrameCtrl.mFrame=0;this->mBtkAnm[i].mFrameCtrl.mFrame=0;this->mBrkAnm[i].mFrameCtrl.mFrame=0;
    this->mBckAnm[i].mFrameCtrl.mRate=1;this->mBtkAnm[i].mFrameCtrl.mRate=1;this->mBrkAnm[i].mFrameCtrl.mRate=1;
    this->mMdlTimer[i]=(u32)gabi::ftoi(cM_rndF(3.9999001026153564f))+1;
   }
  } else {
   u32 sw=PrmAbstract(this,PRM_SWSAVE_W,PRM_SWSAVE_S);
   if (!dComIfGs_isSwitch(sw,this->home.roomNo)) {
    u32 timer=(u32)this->mMdlTimer[i]-1;
    this->mMdlTimer[i]=timer;
    if(timer==0) this->pos_reset(i);
   }
  }
 }
 this->set_mtx();return 1;
}
/* 023BD1EC */
static void staticInitialize() { __sinit_header_statics(); }
/* 023BD280 */
static void deleteStatic(u8* self,u32 flags) {
  if(self && (flags&1)) operator_delete(self);
}
/* 023BD294 */
static u8* bckConstructorGuest(u8* self) {
 // HD: this GHS constructor allocates on a null pointer.
 if (!self) { self=static_cast<u8*>(operator_new(0x8C)); if (!self) return nullptr; }
 auto* view=reinterpret_cast<BckConstructorView*>(self);
 initializeFrameCtrl(&view->mFrameCtrl,0);
 view->__vtbl=BCK_BASE_VTBL;
 constructMatrixCalc(view->mCalc);
 view->mpAnm=0;
 view->__vtblAnm=CALC_ANM_VTBL;
 view->mTransform=0;
 view->mCalcState[2]=0;
 view->__vtbl=BCK_VTBL;
 view->mCalcState[1]=0;
 view->mCalcState[0]=0;
 return self;
}
/* 023BD324 */
static BOOL IsDelete(daObjYboil_c* self) {
 return 1;
}
/* 023BD32C */
static void bckDestructorGuest(u8* self,u32 flags) {

 if(self) { destructMatrixCalc(&reinterpret_cast<BckConstructorView*>(self)->__vtbl,0);if(flags&1) operator_delete(self); }
}
/* 023BD380 */
static void actorDestructor(daObjYboil_c* self,u32 flags) {

 if(self) {
  destroyArray(&self->mBckAnm[0],50,sizeof(mDoExt_bckAnm),gabi::guest_fn((const void*)bckDestructorGuest),0,0);
  __dt(static_cast<fopAc_ac_c*>(self));if(flags&1) operator_delete(self);
 }
}
/* 023BD3F4 */
static void emptyVirtual(u8* self) {  }

/* WWHD glue
str 1003391C
str 10033978 in 023BC3D8
str 100339F8 in 023BCB04
str 10033980
str 10033994
str 100339A8
str 100339B4
str 100339C0
ext 025E8154 BOOL YboilBrk::init(J3DModelData*,J3DAnmTevRegKey*,bool,s32,f32,s16,s16,bool,s32)
ext 025E83FC void YboilBrk::entry(J3DModelData*,f32)
ext 028EFFD0 void constructArray(void*,u32,u32,u32)
ext 028F0164 void destroyArray(void*,u32,u32,u32,u32,u32)
ext 027F2BC0 void initializeFrameCtrl(J3DFrameCtrl*,s32)
ext 027DA984 void constructMatrixCalc(void*)
ext 027F3628 void destructMatrixCalc(void*,s32)
ext 025602F0 void dKy_get_seacolor(GXColor*,GXColor*)
ext 0274D458 void convertColor(FloatColor*,const FloatColor*,f32)
ext 027F9F0C FloatColor* getShaderParameter(ShaderParams*,u32)
*/
