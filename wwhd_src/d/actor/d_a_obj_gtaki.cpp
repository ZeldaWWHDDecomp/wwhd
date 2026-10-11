// WWHD Ganon waterfall.
#include "d/actor/d_a_obj_gtaki.h"
using namespace gtaki_layout;
const gc_tu_data gc_tu = {.safeString=0x10029AAC};
static const char M_arcname[]="Gtaki";
static const char M_dummyName[]="B_dummy";
enum { GTAKI_VTBL=0x10029AD4, CYL_AAB_VTBL=0x10029AC4,
       dRes_INDEX_GTAKI_BDL_GTAKI_e=5, dRes_INDEX_GTAKI_BTK_GTAKI_e=8,
       dRes_INDEX_GTAKI_DZB_ITAKI_e=11, ID_AK_SN_GANONFALLSSPLASH00=0x835B };
static dCcD_SrcCyl l_cyl_src;
static LightPrefix lightTemplate;
static HeaderStatics headerStatics;
struct PiPair { be<f32> x,y; };
static PiPair piPair;
using GuestPointer=gptr<void>;
static GuestPointer renderManager;
using DrawBuffer=be<u32>;
static DrawBuffer currentOpa;
static DrawBuffer currentXlu;
const char* getTextureName(JUTNameTab*,u16);
void* getFrameBufferTexture(void*,u32);
CaptureBuffer* createCaptureBuffer(void*,void*);
void dKy_tevstr_init(dKy_tevstr_c*,s8,u8);
void initializeHeaderA(u8*);
void initializeHeaderB(u8*);
s32 CheckCreateHeap(fopAc_ac_c*);
daObjGtaki_c* constructGuest(daObjGtaki_c*);

/* 0234D9C8 */
void daObjGtaki_c::set_mtx() {
  f32 x=scale.x,y=scale.y,z=scale.z;
  J3DModel* model=mpModel;
  model->mBaseScale.x=x;
  model->mBaseScale.y=y;
  model->mBaseScale.z=z;
  mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
  mDoMtx_YrotM(mDoMtx_stack_c::get(),current.angle.y);
  model=mpModel;
  model->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 0234DC28 */
s32 daObjGtaki_c::_create() {
  u32 flags=actor_condition;
  if (!(flags & fopAcCnd_INIT_e)) {
    if (gabi::ea(this)) {
      constructGuest(this);
      flags=actor_condition;
    }
    actor_condition=flags | fopAcCnd_INIT_e;
  }
  cPhs_State phase=dComIfG_resLoad(&mPhase,M_arcname);
  if (phase==cPhs_COMPLEATE_e) {
    if (!fopAcM_entrySolidHeap(this,CheckCreateHeap,0x3450)) return cPhs_ERROR_e;
    CreateInit();
  }
  return phase;
}
/* 0234DCE0 */
s32 daObjGtaki_c::_delete() {
  if (heap) dComIfG_Bgsp()->Release(mpBgW);
  dComIfG_resDelete(&mPhase,M_arcname);
  return TRUE;
}
static DrawListView* drawLists() {
  return reinterpret_cast<DrawListView*>(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->drawlist);
}
/* 0234DD38 */
s32 daObjGtaki_c::_draw() {
  g_env_light.settingTevStruct(4,&current.pos,&tevStr);
  g_env_light.setLightTevColorType(mpModel,&tevStr);
  currentOpa=drawLists()->invisibleOpa;
  currentXlu=drawLists()->invisibleXlu;
  J3DModelData* data=mpModel->getModelData();
  f32 frame=mBtkAnm.getFrame();
  mBtkAnm.entry(data,frame);
  mDoExt_modelUpdateDL(mpModel);
  // HD: remove the texture animation through model data, after reloading the model.
  reinterpret_cast<ModelDataView*>(mpModel->getModelData())->mTextureSRTAnm=0;
  dComIfGd_setList();
  return TRUE;
}
/* 0234DDF4 */
s32 daObjGtaki_c::_execute() {
  mCyl.SetC(&current.pos);
  dComIfG_Ccsp_Set(&mCyl);
  mBtkAnm.play();
  set_mtx();
  return TRUE;
}
/* 0234DF00 */
s32 IsDelete(daObjGtaki_c*) { return TRUE; }
/* 0234DF74 */
void emptyVirtual(daObjGtaki_c*) {}
/* 0234DEEC */
void deleteStatic(void* obj,u32 flags) {
  if (obj && (flags & 1)) operator_delete(obj);
}
/* 0234DF08 */
void destruct(daObjGtaki_c* obj,u32 flags) {
  if (obj) {
    __dt(&obj->mCyl);
    __dt(&obj->mStts);
    __dt(static_cast<fopAc_ac_c*>(obj));
    if (flags & 1) operator_delete(obj);
  }
}
/* 0234DE58 */
void staticInitialize() {
  headerStatics.zero[2]=0;
  headerStatics.zero[0]=0;
  headerStatics.zero[3]=0;
  headerStatics.zero[1]=0;
  __register_global_object(0x101C9B00);
  headerStatics.piMin=piPair.x;
  headerStatics.piMax=piPair.y;
  initializeHeaderA(&headerStatics.initializeA);
  __register_global_object(0x101C9B0C);
  initializeHeaderB(&headerStatics.initializeB);
  __register_global_object(0x101C9B18);
}
/* 0234D794 */
daObjGtaki_c* constructGuest(daObjGtaki_c* obj) {
  // HD: GHS constructor entry allocates the actor when passed a null this pointer.
  if (!obj) {
    obj=static_cast<daObjGtaki_c*>(operator_new(sizeof(daObjGtaki_c)));
    if (!obj) return nullptr;
  }
  fopAc_ac_c_ct(obj);
  obj->__vtbl=GTAKI_VTBL;
  mDoExt_btkAnm::ct(&obj->mBtkAnm);
  dCcD_Stts_ct(&obj->mStts);
  dCcD_Cyl_ct(&obj->mCyl,CYL_AAB_VTBL);
  // HD: all three embedded light prefixes use the same template and load order.
  f32 vectors[6];
  for (int i=0;i<6;++i) vectors[i]=lightTemplate.vectors[i];
  u8 color[4];
  for (int i=0;i<4;++i) color[i]=lightTemplate.color[i];
  s16 color16[4];
  for (int i=0;i<4;++i) color16[i]=lightTemplate.color16[i];
  f32 factors[8];
  for (int i=0;i<8;++i) factors[i]=lightTemplate.factors[i];
  auto* view=reinterpret_cast<TevConstructorView*>(&obj->mTevStr);
  for (LightPrefix* light : {&view->light,&view->hdLightA,&view->hdLightB}) {
    for (int i=0;i<6;++i) light->vectors[i]=vectors[i];
    for (int i=0;i<4;++i) light->color[i]=color[i];
    for (int i=0;i<4;++i) light->color16[i]=color16[i];
    for (int i=0;i<8;++i) light->factors[i]=factors[i];
  }
  return obj;
}
/* 0234DAA0 */
s32 daObjGtaki_c::CreateInit() {
  fopAcM_SetMtx(this,mpModel->getBaseTRMtx());
  fopAcM_setCullSizeBox(this,-600.f,-0.f,-600.f,600.f,10000.f,600.f);
  fopAcM_setCullSizeFar(this,1.f);
  mStts.Init(255,255,this);
  mCyl.Set(&l_cyl_src);
  mCyl.SetR(70.f * scale.x);
  s8 room=home.roomNo;
  mCyl.SetStts(&mStts);
  dKy_tevstr_init(&mTevStr,room,255);
  g_env_light.settingTevStruct(2,&current.pos,&mTevStr);
  auto* emitter=reinterpret_cast<EmitterView*>(dComIfGp_getParticle()->set(1,
      ID_AK_SN_GANONFALLSSPLASH00,&current.pos,nullptr,nullptr,255,nullptr,-1,nullptr,nullptr,nullptr));
  if (emitter) {
    f32 z=scale.z,x=scale.x,y=scale.y;
    emitter->mGlobalDynamicsScale.x=x;
    emitter->mGlobalDynamicsScale.z=z;
    emitter->mGlobalDynamicsScale.y=y;
    x=scale.x;
    emitter->mGlobalParticleScale.x=x;
    emitter->mGlobalParticleScale.y=x;
    emitter->mGlobalParticleScale.z=x;
    u8 r=mTevStr.mColorC0.r,b=mTevStr.mColorC0.b;
    emitter->mGlobalPrmColor.r=r;
    u8 g=mTevStr.mColorC0.g;
    emitter->mGlobalPrmColor.b=b;
    emitter->mGlobalPrmColor.g=g;
  }
  set_mtx();
  return dComIfG_Bgsp()->Regist(mpBgW,this);
}
/* 0234D5EC */
s32 daObjGtaki_c::CreateHeap() {
  auto* data=static_cast<J3DModelData*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_GTAKI_BDL_GTAKI_e));
  if (!data) JUT_ASSERT_fail("d_a_obj_gtaki.cpp",0x123,"modelData != (0)");
  mpModel=mDoExt_J3DModel__create(data,0,0x11020203);
  if (!mpModel) return FALSE;
  auto* anim=static_cast<J3DAnmTextureSRTKey*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_GTAKI_BTK_GTAKI_e));
  if (!anim) JUT_ASSERT_fail("d_a_obj_gtaki.cpp",0x12D,"btk != 0");
  mBtkAnm.init(data,anim,true,J3DFrameCtrl::EMode_LOOP,1.f,0,-1,false,0);
  setDummyTexture();
  mDoMtx_stack_c::transS(current.pos.x,current.pos.y,current.pos.z);
  mDoMtx_YrotM(mDoMtx_stack_c::get(),shape_angle.y);
  f32 y=scale.y,x=scale.x,z=scale.z;
  mDoMtx_stack_c::scaleM(x,y,z);
  PSMTXCopy(mDoMtx_stack_c::get(),&mMtx);
  mpBgW=new_dBgW();
  if (!mpBgW) return FALSE;
  auto* mesh=static_cast<cBgD_t*>(dComIfG_getObjectRes(M_arcname,dRes_INDEX_GTAKI_DZB_ITAKI_e));
  return mpBgW->Set(mesh,1,&mMtx)==0;
}
/* 0234D790 */
s32 CheckCreateHeap(fopAc_ac_c* actor) {
  return static_cast<daObjGtaki_c*>(actor)->CreateHeap();
}
static u32 shiftLeftWord(u32 value,u32 shift) {
  shift &= 63;
  return shift < 32 ? value << shift : 0;
}
static u32 shiftRightWord(u32 value,u32 shift) {
  shift &= 63;
  return shift < 32 ? value >> shift : 0;
}
/* 0234D3AC */
void daObjGtaki_c::setDummyTexture() {
  auto* data=reinterpret_cast<ModelDataView*>(mpModel->getModelData());
  TextureTable* texture=data->mTexture;
  JUTNameTab* names=data->mTextureName;
  if (!texture) JUT_ASSERT_fail("d_a_obj_gtaki.cpp",0xB8,"texture != (0)");
  if (!names) JUT_ASSERT_fail("d_a_obj_gtaki.cpp",0xB9,"textureName != (0)");
  // HD: the relocated offsets escape the stack, so retain GHS's 80-byte frame
  // and header position at entrySP-72, including its untouched padding bytes.
  struct ImageFrame { u8 bytes[80]; };
  gabi::Local<ImageFrame> frameStorage;
  auto* image=reinterpret_cast<TextureImage*>(frameStorage->bytes + 8);
  for (u16 i=0;i<texture->getNum();++i) {
    const char* name=getTextureName(names,i);
    if (!name) {
      JUT_ASSERT_fail("d_a_obj_gtaki.cpp",0xC0,"name != (0)");
      continue;
    }
    const be<u8>* s=reinterpret_cast<const be<u8>*>(name);
    const char* t=M_dummyName;
    u8 c,d;
    do { c=*s++; d=*t++; } while (c==d && c);
    if (c!=d) continue;
    void* frame=getFrameBufferTexture(renderManager,6);
    void* allocation=operator_new(0xC0);
    CaptureBuffer* buffer=createCaptureBuffer(allocation,frame);
    image->buffer=gabi::ea(buffer);
    image->width=(u16)buffer->width;
    image->indexTexture=0;
    image->height=(u16)buffer->height;
    auto* dest=reinterpret_cast<TextureWords*>(&texture->mpRes[i]);
    auto* words=reinterpret_cast<TextureWords*>(image);
    for (int j=0;j<9;++j) dest->word[j]=words->word[j];
    // HD: reload mpRes after each write; partially overlapping tables are verified.
    TextureImage* relocated=&texture->mpRes[i];
    relocated->imageOffset=relocated->imageOffset + gabi::ea(image) - gabi::ea(relocated);
    relocated=&texture->mpRes[i];
    relocated->paletteOffset=relocated->paletteOffset + gabi::ea(image) - gabi::ea(relocated);
    relocated=&texture->mpRes[i];
    relocated->buffer=image->buffer;
    u32 hi=texture->maskHigh,lo=texture->maskLow;
    if (i<64) {
      u32 oldlo=texture->modifiedLow,oldhi=texture->modifiedHigh;
      u32 h=shiftLeftWord(hi,i) | shiftRightWord(lo,32-i) | shiftLeftWord(lo,i+32);
      texture->modifiedHigh=oldhi | h;
      texture->modifiedLow=oldlo | shiftLeftWord(lo,i);
    } else {
      u32 k=i-64,oldlo=texture->modifiedSecondLow,oldhi=texture->modifiedSecondHigh;
      u32 h=shiftLeftWord(hi,k) | shiftRightWord(lo,32-k) | shiftLeftWord(lo,k+32);
      texture->modifiedSecondLow=oldlo | shiftLeftWord(lo,k);
      texture->modifiedSecondHigh=oldhi | h;
    }
  }
}

/* WWHD glue
str 10029AA4
str 10029B84 in 0234DCE0
str 10029B34 in 0234D5EC
str 10029AE4
str 10029B3C in 0234D5EC
str 10029B50
str 10029B64
str 10029AF8 in 0234D3AC
str 10029B0C
str 10029B1C
str 10029AEC
ext 027ED1F0 const char* getTextureName(JUTNameTab*,u16)
ext 027F81A4 void* getFrameBufferTexture(void*,u32)
ext 02773680 CaptureBuffer* createCaptureBuffer(void*,void*)
ext 0255FFF4 void dKy_tevstr_init(dKy_tevstr_c*,s8,u8)
ext 028ED6F8 void initializeHeaderA(u8*)
ext 028EAB2C void initializeHeaderB(u8*)
data l_cyl_src 101C9ABC
data lightTemplate 1016E414
data headerStatics 10469CE4
data piPair 10029B90
data renderManager 101F9968
data currentOpa 104B4634
data currentXlu 104B4638
*/
