#include "bindings.h"
#include "d/actor/d_a_obj_swheavy.h"
using daObjSwheavy::Act_c;
static u32 PrmAbstract(Act_c* a,s32 width,s32 shift) {
 WWHD_FUNC(0x023951C0,u32,a,width,shift);
 u32 value=a->mParameters,w=(u32)width&63,sh=(u32)shift&63;
 return (sh<32?value>>sh:0)&((w<32?1u<<w:0)-1);
}
VERIFY(0x023951C0,PrmAbstract);
static void JUT_assert(u32 file,u32 line,u32 message) { gabi::call(0x0273AA24,STR(file),line,STR(message)); }
static const char* arc() { return STR(0x1002FD88); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 attr(u32 type) { return 0x1002FD90+type*0x1C; }
static u32 checkedType(Act_c* a,u32 type,u32 file,u32 msg) {
 if(type>=4) { JUT_assert(file,0xFD,msg);type=a->mType; }
 return type;
}
static BOOL isSwitch(Act_c* a) {
 u32 sw=PrmAbstract(a,8,8),save=gabi::load<u32>(0x101F84DC);s8 room=a->home.roomNo;
 return gabi::call<BOOL>(0x025BA0C0,gabi::at<u8>(save+0x20),sw,room);
}
static u8 create_heap(Act_c* a) {
 WWHD_FUNC(0x02393CD0,u8,a);
 auto data=dComIfG_getObjectRes(arc(),4,0x1002FB34);
 if(!data) JUT_assert(0x1002FB9C,0x118,0x1002FB5C);
 a->mpModel1=mDoExt_J3DModel__create((J3DModelData*)data,0x80000,0x11000022);
 data=dComIfG_getObjectRes(arc(),5,0x1002FB34);
 if(!data) JUT_assert(0x1002FB9C,0x121,0x1002FB6C);
 a->mpModel2=mDoExt_J3DModel__create((J3DModelData*)data,0x80000,0x11000022);
 BOOL b1=FALSE,b2=FALSE;u32 bg=gabi::call<u32>(0x024F23F4,0);a->mpBgW1=bg;
 if(bg) {
 data=dComIfG_getObjectRes(arc(),8,0x1002FB34);
 if(!data) JUT_assert(0x1002FB9C,0x139,0x1002FB7C);
 if(!gabi::call<BOOL>(0x0200A030,gabi::at<u8>(a->mpBgW1),data,1,&a->mMtx1)) b1=TRUE;
 }
 bg=gabi::call<u32>(0x024F23F4,0);a->mpBgW2=bg;
 if(bg) {
 data=dComIfG_getObjectRes(arc(),9,0x1002FB34);
 if(!data) JUT_assert(0x1002FB9C,0x148,0x1002FB8C);
 if(!gabi::call<BOOL>(0x0200A030,gabi::at<u8>(a->mpBgW2),data,1,&a->mMtx2)) b2=TRUE;
 }
 u8 success=a->mpModel1!=nullptr&&a->mpModel2!=nullptr&&b1&&b2;
 if(!success) { a->mpBgW2=0;a->mpBgW1=0; }
 return success;
}
VERIFY(0x02393CD0,create_heap);
static u8 solidHeapCB(Act_c* a) { WWHD_FUNC(0x02393EC4,u8,a);return create_heap(a); }
VERIFY(0x02393EC4,solidHeapCB);
static void rideCB(void* bg,Act_c* a,fopAc_ac_c* rider) {
 WWHD_FUNC(0x02393EC8,void,bg,a,rider);
 if(rider->actor_status&0x400) {
 a->mRiding=1;
 if(rider&&gabi::load<s16>(gabi::ea(rider)+0xE)==0xA8&&(gabi::load<u32>(gabi::ea(rider)+0x3B8)&0x02000000)) a->mHeavyRiding=1;
 }
}
VERIFY(0x02393EC8,rideCB);
static void copyModelMtx(u32 model) {
 f32 v[12];for(u32 i=0;i<12;i++) v[i]=gabi::load<f32>(0x1048D0CC+4*i);
 for(u32 i=0;i<12;i++) gabi::store<f32>(model+0xC8+4*i,v[i]);
}
static void translateRotate(Act_c* a) {
 auto m=gabi::at<Mtx34>(0x1048D0CC);
 f32 x=a->current.pos.x,z=a->current.pos.z,y=a->current.pos.y;
 gabi::call(0x028E93CC,m,x,y,z);
 s16 xrot=a->shape_angle.x,yrot=a->shape_angle.y,zrot=a->shape_angle.z;
 gabi::call(0x025F1B48,m,xrot,yrot,zrot);
}
static void set_mtx(Act_c* a) {
 WWHD_FUNC(0x02393F08,void,a);
 translateRotate(a);copyModelMtx(gabi::ea((J3DModel*)a->mpModel1));
 f32 height=a->mModelHeight,old=gabi::load<f32>(0x1048D0E8);
 gabi::store<f32>(0x1048D0E8,old+height);
 copyModelMtx(gabi::ea((J3DModel*)a->mpModel2));
 translateRotate(a);
 f32 y=a->scale.y,x=a->scale.x,z=a->scale.z;
 gabi::call(0x025F2518,x,y,z);
 auto m=gabi::at<Mtx34>(0x1048D0CC);gabi::call(0x028E90D4,m,&a->mMtx1);
 old=gabi::load<f32>(0x1048D0E8);height=a->mTopPos;
 gabi::store<f32>(0x1048D0E8,old+height);gabi::call(0x028E90D4,m,&a->mMtx2);
}
VERIFY(0x02393F08,set_mtx);
static void init_mtx(Act_c* a) {
 WWHD_FUNC(0x0239409C,void,a); // Matcher incorrectly names this PrmAbstract.
 u32 model=gabi::ea((J3DModel*)a->mpModel1);f32 y=a->scale.y,x=a->scale.x,z=a->scale.z;
 gabi::store<f32>(model+0xBC,x);gabi::store<f32>(model+0xC0,y);gabi::store<f32>(model+0xC4,z);
 model=gabi::ea((J3DModel*)a->mpModel2);x=a->scale.x;z=a->scale.z;y=a->scale.y;
 gabi::store<f32>(model+0xC4,z);gabi::store<f32>(model+0xBC,x);gabi::store<f32>(model+0xC0,y);
 set_mtx(a);
}
VERIFY(0x0239409C,init_mtx);
static void mode_upper_init(Act_c* a) {
 WWHD_FUNC(0x023940D8,void,a);a->mMode=0;a->mTargetHFrac=1.0f;a->mChangingState=0;
}
VERIFY(0x023940D8,mode_upper_init);
static void mode_lower_init(Act_c* a) {
 WWHD_FUNC(0x023940F4,void,a);a->mChangingState=0;a->mMode=2;a->mTargetHFrac=0.0f;
}
VERIFY(0x023940F4,mode_lower_init);
static void top_bg_aim_req(Act_c* a,f32 target,s16 timer) {
 WWHD_FUNC(0x02394ADC,void,a,target,timer);a->mBgAimTarget=target;a->mBgAimTimer=timer;
}
VERIFY(0x02394ADC,top_bg_aim_req);
static void mode_l_u_init(Act_c* a) {
 WWHD_FUNC(0x02394E68,void,a);a->mMode=3;a->mTargetHFrac=1.0f;top_bg_aim_req(a,1.0f,1);
}
VERIFY(0x02394E68,mode_l_u_init);
static BOOL Delete(Act_c* a) {
 WWHD_FUNC(0x02394484,BOOL,a);
 u32 bg=a->mpBgW1;
 if(bg&&gabi::load<u32>(bg)<0x100) { u32 p=play();gabi::call(0x020087EC,gabi::at<u8>(p+0x12A0),gabi::at<u8>(a->mpBgW1)); }
 bg=a->mpBgW2;
 if(bg&&gabi::load<u32>(bg)<0x100) { u32 p=play();gabi::call(0x020087EC,gabi::at<u8>(p+0x12A0),gabi::at<u8>(a->mpBgW2)); }
 dComIfG_resDelete(&a->mPhase,arc());return TRUE;
}
VERIFY(0x02394484,Delete);
static BOOL Draw(Act_c* a) {
 WWHD_FUNC(0x02394A28,BOOL,a);
 auto env=dKy_getEnvlight();settingTevStruct(env,1,&a->current.pos,&a->tevStr);
 env=dKy_getEnvlight();auto model=(J3DModel*)a->mpModel1;setLightTevColorType(env,model,&a->tevStr);
 env=dKy_getEnvlight();model=(J3DModel*)a->mpModel2;setLightTevColorType(env,model,&a->tevStr);
 u32 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D70));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D74));
 gabi::call(0x025E2DE0,(J3DModel*)a->mpModel1,0);gabi::call(0x025E2DE0,(J3DModel*)a->mpModel2,0);
 p=play();gabi::store<u32>(0x104B4634,gabi::load<u32>(p+0x5D78));
 p=play();gabi::store<u32>(0x104B4638,gabi::load<u32>(p+0x5D7C));return TRUE;
}
VERIFY(0x02394A28,Draw);
static void sinit() { WWHD_FUNC(0x023950B8,void,(u32)0);sinit_header_statics(0x1046C008,0x101CCF28); }
VERIFY(0x023950B8,sinit);
static void trivialDestructor(void* a,s32 flags) { WWHD_FUNC(0x0239514C,void,a,flags);if(a&&(flags&1)) operator_delete(a); }
VERIFY(0x0239514C,trivialDestructor);
static void emptyVirtual(void* a) { WWHD_FUNC(0x02395160,void,a); }
VERIFY(0x02395160,emptyVirtual);
static void destructor(Act_c* a,s32 flags) {
 WWHD_FUNC(0x02395164,void,a,flags);if(a) { gabi::call(0x025D50BC,a,0);if(flags&1) operator_delete(a); }
}
VERIFY(0x02395164,destructor);
static BOOL IsDelete(void* a) { WWHD_FUNC(0x023951B8,BOOL,a);return TRUE; }
VERIFY(0x023951B8,IsDelete);
static void colorToFloat(void* dest,void* src) {
 WWHD_FUNC(0x023951DC,void,dest,src);
 u32 s=gabi::ea(src),d=gabi::ea(dest);f32 r=(f32)gabi::load<u8>(s)/255.0f,g=(f32)gabi::load<u8>(s+1)/255.0f;
 f32 b=(f32)gabi::load<u8>(s+2)/255.0f,alpha=(f32)gabi::load<u8>(s+3)/255.0f;
 gabi::store<f32>(d,r);gabi::store<f32>(d+4,g);gabi::store<f32>(d+8,b);gabi::store<f32>(d+12,alpha);
}
VERIFY(0x023951DC,colorToFloat);
static void set_push_flag(Act_c* a) {
 WWHD_FUNC(0x02394510,void,a);
 if(a->mMiniPushFlg) {
 if(a->mRiding) {
 u32 type=checkedType(a,a->mType,0x1002FC04,0x1002FC18);
 a->mMiniPushTimer=gabi::load<s16>(attr(type)+0x16);
 } else { s16 timer=(s16)((s16)a->mMiniPushTimer-1);a->mMiniPushTimer=timer;if(timer<=0) a->mMiniPushFlg=0; }
 } else if(a->mRiding) {
 u32 type=checkedType(a,a->mType,0x1002FC04,0x1002FC18);
 s16 timer=(s16)((s16)a->mMiniPushTimer+1);a->mMiniPushTimer=timer;
 if(timer>=gabi::load<s16>(attr(type)+0x16)) a->mMiniPushFlg=1;
 } else a->mMiniPushTimer=0;
 if(a->mPushFlg) {
 if(a->mHeavyRiding) {
 u32 type=checkedType(a,a->mType,0x1002FC04,0x1002FC18);
 a->mPushTimer=gabi::load<s16>(attr(type)+0x18);
 } else { s16 timer=(s16)((s16)a->mPushTimer-1);a->mPushTimer=timer;if(timer<=0) a->mPushFlg=0; }
 } else if(a->mHeavyRiding) {
 u32 type=checkedType(a,a->mType,0x1002FC04,0x1002FC18);
 s16 timer=(s16)((s16)a->mPushTimer+1);a->mPushTimer=timer;
 if(timer>=gabi::load<s16>(attr(type)+0x18)) a->mPushFlg=1;
 } else a->mPushTimer=0;
}
VERIFY(0x02394510,set_push_flag);
static void calc_top_pos(Act_c* a) {
 WWHD_FUNC(0x02394730,void,a);
 u32 type=a->mType;f32 target=a->mTargetHFrac,current=a->mCurHFrac,difference=current-target;
 type=checkedType(a,type,0x1002FC40,0x1002FC54);
 u32 params=attr(type);f32 decay=gabi::load<f32>(params+0xC);
 if(type>=4) { JUT_assert(0x1002FC40,0xFD,0x1002FC54);type=a->mType;params=attr(type); }
 f32 speed=a->mVSpeed,spring=gabi::load<f32>(params+8);
 speed=gabi::fnmsubs(difference,spring,speed);current=a->mCurHFrac;
 speed=gabi::fnmsubs(speed,decay,speed);current=current+speed;a->mCurHFrac=current;
 f32 height=(1.0f-current)*-35.5f;s16 timer=a->mBgAimTimer;
 a->mVSpeed=speed;
 if(height < -36.5f) height=-36.5f;
 else { f32 delta=height-1.0f;height=delta>=0.0f?1.0f:height; }
 a->mModelHeight=height;
 if(timer>0) { timer=(s16)(timer-1);a->mBgAimTimer=timer;if(timer==0) a->mBgAim=a->mBgAimTarget; }
 f32 fraction=a->mMode==0?(f32)a->mCurHFrac:(f32)a->mBgAim;
 f32 top=(1.0f-fraction)*-35.5f;f32 bottom=a->mModelHeight;
 a->mTopPos=top;a->mBgHFrac=fraction;if(top<bottom) a->mTopPos=bottom;
}
VERIFY(0x02394730,calc_top_pos);
static void mode_u_l_init(Act_c* a) {
 WWHD_FUNC(0x02394AE8,void,a);
 u32 type=a->mType;a->mMode=1;a->mTargetHFrac=0.0f;
 type=checkedType(a,type,0x1002FC98,0x1002FCAC);u32 params=attr(type);
 type=a->mType;a->mVSpeed=gabi::load<f32>(params+0x10);
 type=checkedType(a,type,0x1002FC98,0x1002FCAC);
 top_bg_aim_req(a,0.0f,gabi::load<s16>(attr(type)+0x14));
}
VERIFY(0x02394AE8,mode_u_l_init);
static void mode_upper(Act_c* a) {
 WWHD_FUNC(0x02394BCC,void,a);
 f32 height=1.0f;u8 mini=a->mMiniPushFlg;u32 type=a->mType;
 if(mini) {
 type=checkedType(a,type,0x1002FCD4,0x1002FCE8);
 if(!(gabi::load<u32>(attr(type)+4)&8)||a->mPushFlg) {
 a->mChangingState=1;mode_u_l_init(a);return;
 }
 height=0.9f;
 }
 type=checkedType(a,type,0x1002FCD4,0x1002FCE8);
 if((gabi::load<u32>(attr(type)+4)&1)&&isSwitch(a)) { mode_u_l_init(a);return; }
 a->mTargetHFrac=height;top_bg_aim_req(a,height,1);
}
VERIFY(0x02394BCC,mode_upper);
static void mode_u_l(Act_c* a) {
 WWHD_FUNC(0x02394D50,void,a);
 if(a->mCurHFrac>0.0f) return;
 if(a->mChangingState) {
 u32 type=checkedType(a,a->mType,0x1002FD0C,0x1002FD20);
 u32 toggle=gabi::load<u32>(attr(type)+4)&4,sw=PrmAbstract(a,8,8);
 s8 room=a->home.roomNo;u32 save=gabi::load<u32>(0x101F84DC);
 gabi::call(toggle?0x025BA20C:0x025B9E38,gabi::at<u8>(save+0x20),sw,room);
 }
 s8 room=a->current.roomNo;s32 reverb=gabi::call<s32>(0x02520540,room);
 gabi::call(0x025E1A40,0x6930,&a->eyePos,0,reverb);mode_lower_init(a);
}
VERIFY(0x02394D50,mode_u_l);
static void mode_lower(Act_c* a) {
 WWHD_FUNC(0x02394E84,void,a);
 BOOL pressing=FALSE;u8 mini=a->mMiniPushFlg;u32 type=a->mType;
 if(mini) {
 type=checkedType(a,type,0x1002FD44,0x1002FD58);
 if(!(gabi::load<u32>(attr(type)+4)&8)||a->mPushFlg) pressing=TRUE;
 }
 type=checkedType(a,type,0x1002FD44,0x1002FD58);
 BOOL pop=!(gabi::load<u32>(attr(type)+4)&2)&&!pressing;
 u32 params=attr(type);type=checkedType(a,type,0x1002FD44,0x1002FD58);params=attr(type);
 BOOL match=FALSE;
 if(gabi::load<u32>(params+4)&1) { BOOL active=isSwitch(a);if(!(active|pressing)) match=TRUE; }
 if(pop||match) {
 type=checkedType(a,a->mType,0x1002FD44,0x1002FD58);
 u32 toggle=(gabi::load<u32>(attr(type)+4)>>2)&1;
 if(pop&&!(toggle|pressing)) a->mChangingState=1;
 mode_l_u_init(a);
 }
}
VERIFY(0x02394E84,mode_lower);
static void mode_l_u(Act_c* a) {
 WWHD_FUNC(0x0239502C,void,a);
 if(a->mCurHFrac<1.0f) return;
 if(a->mChangingState) {
 u32 sw=PrmAbstract(a,8,8),save=gabi::load<u32>(0x101F84DC);s8 room=a->home.roomNo;
 gabi::call(0x025B9F7C,gabi::at<u8>(save+0x20),sw,room);
 }
 mode_upper_init(a);
}
VERIFY(0x0239502C,mode_l_u);
static s32 Create(Act_c* a) {
 WWHD_FUNC(0x02394114,s32,a);
 u32 condition=a->actor_condition;
 if(!(condition&8)) {
 if(a) { gabi::call(0x025D4ED0,a);a->__vtbl=0x1002FB4C;condition=a->actor_condition; }
 a->actor_condition=condition|8;
 }
 s32 phase=dComIfG_resLoad(&a->mPhase,arc());if(phase!=4) return phase;
 u32 type=PrmAbstract(a,3,24);a->mType=type;
 type=checkedType(a,type,0x1002FBCC,0x1002FBE0);
 BOOL up=TRUE;if(!(gabi::load<u32>(attr(type)+4)&4)&&isSwitch(a)) up=FALSE;
 f32 z=a->scale.z,x=a->scale.x;a->scale.z=z*1.5f;a->scale.x=x*1.5f;
 translateRotate(a);f32 sy=a->scale.y,sx=a->scale.x,sz=a->scale.z;gabi::call(0x025F2518,sx,sy,sz);
 auto m=gabi::at<Mtx34>(0x1048D0CC);gabi::call(0x028E90D4,m,&a->mMtx1);
 if(!up) { f32 y=gabi::load<f32>(0x1048D0E8);gabi::store<f32>(0x1048D0E8,y+-35.5f); }
 gabi::call(0x028E90D4,m,&a->mMtx2);
 if(!gabi::call<BOOL>(0x025D63E8,a,gabi::at<void>(0x02393EC4),0x2000)) return 5;
 u32 p=play();gabi::call(0x024EEA6C,gabi::at<u8>(p+0x12A0),gabi::at<u8>(a->mpBgW1),a);
 gabi::store<u32>((u32)a->mpBgW1+0xA8,0);
 p=play();gabi::call(0x024EEA6C,gabi::at<u8>(p+0x12A0),gabi::at<u8>(a->mpBgW2),a);
 gabi::store<u32>((u32)a->mpBgW2+0xA8,0);gabi::store<u32>((u32)a->mpBgW2+0xB0,0x02393EC8);
 auto model=(J3DModel*)a->mpModel1;a->cullMtx=model?gabi::ea(model)+0xC8:0;init_mtx(a);
 gabi::call(0x025D674C,a,-97.5f,-2.0f,-97.5f,97.5f,55.0f,97.5f);
 a->mPushFlg=0;type=a->mType;a->mPrevHeavyRiding=0;a->mPushTimer=0;a->mVSpeed=0.0f;
 a->mPrevRiding=0;a->mMiniPushTimer=0;a->mBgAimTimer=0;a->mHeavyRiding=0;a->mBgAimTarget=0.0f;
 a->mMiniPushFlg=0;a->mRiding=0;
 type=checkedType(a,type,0x1002FBCC,0x1002FBE0);
 if((gabi::load<u32>(attr(type)+4)&4)||!isSwitch(a)) {
 a->mTopPos=0.0f;a->mModelHeight=0.0f;a->mCurHFrac=1.0f;a->mBgAim=1.0f;a->mBgHFrac=1.0f;a->mTargetHFrac=1.0f;mode_upper_init(a);
 } else {
 a->mTopPos=-35.5f;a->mModelHeight=-35.5f;a->mBgAim=0.0f;a->mTargetHFrac=0.0f;a->mBgHFrac=0.0f;a->mCurHFrac=0.0f;mode_lower_init(a);
 }
 return phase;
}
VERIFY(0x02394114,Create);
static BOOL Execute(Act_c* a) {
 WWHD_FUNC(0x023948D4,BOOL,a);
 set_push_flag(a);ptmf_call(0x1002FC78+(u32)a->mMode*8,a);calc_top_pos(a);
 u8 riding=a->mRiding,heavy=a->mHeavyRiding;a->mPrevRiding=riding;a->mPrevHeavyRiding=heavy;
 a->mHeavyRiding=0;a->mRiding=0;set_mtx(a);
 gabi::call(0x024F43DC,gabi::at<u8>(a->mpBgW1));gabi::call(0x024F43DC,gabi::at<u8>(a->mpBgW2));
 f32 height=a->mModelHeight,y=a->current.pos.y;y=(y+height)- -35.5f;
 f32 x=a->current.pos.x,z=a->current.pos.z;a->eyePos.x=x;a->eyePos.z=z;a->eyePos.y=y;return TRUE;
}
VERIFY(0x023948D4,Execute);
static s32 CreateWrapper(Act_c* a) { WWHD_FUNC(0x023950A8,s32,a);return Create(a); }
VERIFY(0x023950A8,CreateWrapper);
static BOOL DeleteWrapper(Act_c* a) { WWHD_FUNC(0x023950AC,BOOL,a);return Delete(a); }
VERIFY(0x023950AC,DeleteWrapper);
static BOOL ExecuteWrapper(Act_c* a) { WWHD_FUNC(0x023950B0,BOOL,a);return Execute(a); }
VERIFY(0x023950B0,ExecuteWrapper);
static BOOL DrawWrapper(Act_c* a) { WWHD_FUNC(0x023950B4,BOOL,a);return Draw(a); }
VERIFY(0x023950B4,DrawWrapper);
