/** Regular knob doors, WWHD. Derived game code: */
#include "bindings.h"
#include "d/actor/d_a_knob00.h"

static u32 play() { return gabi::call<u32>(0x025200D4); }
static void resetEvent() { u32 p=play(); gabi::store<u16>(p+0x52B8,gabi::load<u16>(p+0x52B8)|8); }
static BOOL demoProc(daKnob00_c* a) { return gabi::call<BOOL>(0x021A3FB8,a); } // Matcher incorrectly labels this chkPassward.
static void setEventPrm(daKnob00_c* a) {
 WWHD_FUNC(0x021A4318,void,a);
 u8 action=a->mAction;
 a->mEventNo=a->mFrontCheck==0?7:8;
 if(action==4||action==5||action==8||action==10) a->mEventNo=9;
 if(!gabi::call<BOOL>(0x0252AE04,a,6400.0f,12100.0f,62500.0f)) {
  a->mFlags=(u8)a->mFlags&0xFB;
 } else {
  u8 event=a->mEventNo;
  u16 flags=gabi::load<u16>(gabi::ea(a)+0xFA);
  gabi::store<s16>(gabi::ea(a)+0xFC,gabi::load<s16>(gabi::ea(a)+0x3BE + event*2));
  u8 tool=gabi::load<u8>(gabi::ea(a)+0x3D6+event);
  gabi::store<u16>(gabi::ea(a)+0xFA,flags|4);
  gabi::store<u8>(gabi::ea(a)+0xFE,tool);
 }
}
VERIFY(0x021A4318,setEventPrm);
static u8 getShapeType(daKnob00_c* a) { WWHD_FUNC(0x021A3430,u8,a); return gabi::call<u8>(0x0252AA20,a); }
VERIFY(0x021A3430,getShapeType);
static u8 getType2(daKnob00_c* a) { WWHD_FUNC(0x021A4FFC,u8,a); return (u32)a->mParameters>>28; }
VERIFY(0x021A4FFC,getType2);
static BOOL IsDelete(daKnob00_c* a) { WWHD_FUNC(0x021A52D0,BOOL,a); return TRUE; }
VERIFY(0x021A52D0,IsDelete);
static BOOL msgDoor(daKnob00_c* a) {
 WWHD_FUNC(0x021A4A94,BOOL,a);
 if(a->mDoorType==1) return TRUE;
 s32 night=gabi::call<s32>(0x02556D14);
 return a->mDoorType==(night==1?2:3);
}
VERIFY(0x021A4A94,msgDoor);
static void setAngle(daKnob00_c* a) {
 WWHD_FUNC(0x021A3BBC,void,a);
 u32 p=play(); s16 angle=a->shape_angle.y;
 u32 player=gabi::load<u32>(p+0x5B2C);
 gabi::store<s16>(player+0x422,(s16)(angle+0x7FFF));
}
VERIFY(0x021A3BBC,setAngle);
static BOOL demoProc2(daKnob00_c* a) {
 WWHD_FUNC(0x021A4568,BOOL,a);
 if(gabi::call<s32>(0x0252A684,a)==16) return TRUE;
 s32 staff=a->mStaffId; u32 p=play();
 gabi::call(0x02543280,gabi::at<u8>(p+0x52C4),staff);
 return FALSE;
}
VERIFY(0x021A4568,demoProc2);
static BOOL actionWait(daKnob00_c* a) {
 WWHD_FUNC(0x021A4CB4,BOOL,a);
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) {
  gabi::call(0x0252A2DC,a,0); a->mAction=2; demoProc(a);
 } else setEventPrm(a);
 return TRUE;
}
VERIFY(0x021A4CB4,actionWait);
static BOOL actionTalkWait(daKnob00_c* a) {
 WWHD_FUNC(0x021A4C58,BOOL,a);
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) {
  gabi::call(0x0252A2DC,a,0); a->mAction=3; demoProc(a);
 } else setEventPrm(a);
 return TRUE;
}
VERIFY(0x021A4C58,actionTalkWait);
static BOOL actionPassward(daKnob00_c* a) {
 WWHD_FUNC(0x021A4A30,BOOL,a);
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) {
  gabi::call(0x0252A2DC,a,0); a->mAction=6; gabi::call<BOOL>(0x021A4808,a); a->mPasswordState=0;
 } else setEventPrm(a);
 return TRUE;
}
VERIFY(0x021A4A30,actionPassward);
static BOOL actionDemo(daKnob00_c* a) {
 WWHD_FUNC(0x021A4ED0,BOOL,a);
 s16 event=gabi::load<s16>(gabi::ea(a)+0x3BE + (u8)a->mEventNo*2);
 u32 p=play();
 if(gabi::call<BOOL>(0x025440C8,gabi::at<u8>(p+0x52C4),event)) {
  u8 no=a->mEventNo;
  if(no!=7&&no!=8) { a->mAction=1; resetEvent(); a->shape_angle.y=a->current.angle.y; }
 } else demoProc(a);
 return TRUE;
}
VERIFY(0x021A4ED0,actionDemo);
static BOOL actionTalk(daKnob00_c* a) {
 WWHD_FUNC(0x021A4F70,BOOL,a);
 if(demoProc(a)) {
  u8 action=a->mAction;
  if(action==9) {
   u32 mt=gabi::call<u32>(0x025D9F38,STR(0x1001348C),0,0);
   if(mt) gabi::store<u8>(mt+0x8C7,0);
   action=a->mAction;
  }
  a->mAction=action+1; resetEvent(); a->shape_angle.y=a->current.angle.y;
 }
 return TRUE;
}
VERIFY(0x021A4F70,actionTalk);
static void calcMtx(daKnob00_c* a) {
 WWHD_FUNC(0x021A3434,void,a);
 f32 x=a->current.pos.x,z=a->current.pos.z,y=a->current.pos.y;
 gabi::call(0x028E93CC,gabi::at<u8>(0x1048D0CC),x,y,z);
 gabi::call(0x025F1C28,gabi::at<u8>(0x1048D0CC),(s16)a->current.angle.y);
 f32 m[12]; for(int i=0;i<12;i++) m[i]=gabi::load<f32>(0x1048D0CC+4*i);
 u32 model=gabi::ea((J3DModel*)a->mpModel);
 for(int i=0;i<12;i++) gabi::store<f32>(model+0xC8+4*i,m[i]);
}
VERIFY(0x021A3434,calcMtx);
static BOOL eventBit(u16 bit) { u32 save=gabi::load<u32>(0x101F84DC); return gabi::call<BOOL>(0x025B8B94,gabi::at<u8>(save+0x644),bit); }
static BOOL symbol(s32 bit) { u32 save=gabi::load<u32>(0x101F84DC); return gabi::call<BOOL>(0x025B7D90,gabi::at<u8>(save+0xD4),bit); }
static BOOL chkException(daKnob00_c* a) {
 WWHD_FUNC(0x021A4B04,BOOL,a);
 switch((s16)a->home.angle.z) {
 case 0x6A6: if(symbol(0)) return TRUE; return !eventBit(0x2110);
 case 0x6A7: return !eventBit(0x0520);
 case 0x6A8: if(!eventBit(0x0A02)) return TRUE; return eventBit(0x2110)!=0;
 case 0x6A9: return eventBit(0x1701)!=0;
 case 0x6AA: if(!symbol(1)) return TRUE; return eventBit(0x1A80)!=0;
 default: return FALSE;
 }
}
VERIFY(0x021A4B04,chkException);
static BOOL actionVilla(daKnob00_c* a) {
 WWHD_FUNC(0x021A44DC,BOOL,a);
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) { gabi::call(0x0252A2DC,a,0); a->mAction=7; demoProc(a); }
 else if(eventBit(0x2D80)) a->mAction=1;
 else setEventPrm(a);
 return TRUE;
}
VERIFY(0x021A44DC,actionVilla);
static BOOL actionFigure(daKnob00_c* a) {
 WWHD_FUNC(0x021A43F8,BOOL,a);
 if(gabi::load<u16>(gabi::ea(a)+0xF8)==3) {
  u32 mt=gabi::call<u32>(0x025D9F38,STR(0x10013484),0,0);
  if(mt) {
   u32 control=play()+0x51D0;
   u32 id=gabi::call<u32>(0x0253F124,gabi::at<u8>(control),gabi::at<u8>(mt));
   gabi::store<u32>(control+0xCC,id);
   u32 status=gabi::load<u32>(mt+0x2E0);
   gabi::store<u8>(mt+0x8C7,1); gabi::store<u32>(mt+0x2E0,status|0x800);
  }
  gabi::call(0x0252A2DC,a,0); a->mAction=9; demoProc(a);
 } else if(eventBit(0x3401)) a->mAction=1;
 else setEventPrm(a);
 return TRUE;
}
VERIFY(0x021A43F8,actionFigure);
static void openEnd(daKnob00_c* a) {
 WWHD_FUNC(0x021A3E14,void,a);
 a->mFlags=(u8)a->mFlags&0xFE;
 u32 p=play(); u32 bg=a->mpBgW;
 if(gabi::call<BOOL>(0x024EEA6C,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg),a))
  gabi::call(0x0273AA24,STR(0x10013448),0x21B,STR(0x10013444));
 gabi::store<u8>(0x1047A964,0); gabi::call(0x0252A550,a);
}
VERIFY(0x021A3E14,openEnd);
static BOOL drawCheckShip(daKnob00_c* a) {
 WWHD_FUNC(0x021A3940,BOOL,a);
 u32 id=a->parentActorID;
 if(id!=0xFFFFFFFF) {
  gabi::Local<be<u32>> key; *key=id;
  u32 ship=gabi::call<u32>(0x025D5218,0x025E1234,(be<u32>*)key);
  BOOL actor=gabi::call<BOOL>(0x025D4604,gabi::at<u8>(ship));
  if(actor&&ship&&gabi::load<s16>(ship+8)==0x39) return gabi::load<u8>(ship+0x3E4)!=0;
 }
 return TRUE;
}
VERIFY(0x021A3940,drawCheckShip);
static void emptyVirtual(void* a) { WWHD_FUNC(0x021A342C,void,a); }
VERIFY(0x021A342C,emptyVirtual);
static void releaseBg(daKnob00_c* a) { auto p=play(); auto bg=(u32)a->mpBgW; gabi::call(0x020087EC,gabi::at<void>(p+0x12A0),gabi::at<void>(bg)); }
static void openInit(daKnob00_c* a,s32 index) {
 WWHD_FUNC(0x021A3AA0,void,a,index);
 auto model=(J3DModel*)a->mpModel; auto data=gabi::load<u32>(gabi::ea(model)+0xAC);
 auto res=dComIfG_getObjectRes(STR(0x100134A0),gabi::load<s32>(0x101B8628+4*(u32)index),0x100132F8);
 if(!gabi::call<BOOL>(0x025E8508,gabi::at<void>(gabi::ea(a)+0x504),gabi::at<void>(data),res,1,0,1.0f,0,-1,1)) JUT_ASSERT_fail(STR(0x10013410),0x1C0,STR(0x1001340C));
 gabi::call(0x0252A3A0,a,0); a->mFlags=(u8)a->mFlags|1;
 if(index>=2) gabi::store<f32>(gabi::ea(a)+0x508,34.0f); else releaseBg(a);
}
VERIFY(0x021A3AA0,openInit);
static BOOL openProc(daKnob00_c* a,s32 action) {
 WWHD_FUNC(0x021A3BF8,BOOL,a,action);
 auto anm=gabi::at<void>(gabi::ea(a)+0x504);
 if(gabi::call<BOOL>(0x025E742C,anm)) return TRUE;
 if(action==12||action==13) {
 auto night=gabi::call<s32>(0x02556D14); bool white=(night==1)==(action==13);
 if(gabi::call<BOOL>(0x027F2BF8,anm,15.0f)) { gabi::store<f32>(0x101F4810,0.0f); gabi::call(0x025F0658,gabi::at<void>(white?0x101D5E9C:0x101D5E94),0.05f); }
 }
 u32 sound=0;
 if(action==12||action==14) {
 if(gabi::call<BOOL>(0x027F2BF8,anm,28.0f)) sound=0x696E;
 else if(gabi::call<BOOL>(0x027F2BF8,anm,60.0f)) sound=0x696F;
 else if(gabi::call<BOOL>(0x027F2BF8,anm,67.0f)&&action==14) sound=0x6970;
 } else {
 if(gabi::call<BOOL>(0x027F2BF8,anm,25.0f)) sound=0x696E;
 else if(gabi::call<BOOL>(0x027F2BF8,anm,49.0f)) sound=0x696F;
 else if(gabi::call<BOOL>(0x027F2BF8,anm,64.0f)&&action==15) sound=0x6970;
 }
 if(sound) { auto room=(s8)a->current.roomNo; auto reverb=gabi::call<s32>(0x02520540,room); gabi::call(0x025E1A40,sound,&a->eyePos,0,reverb); }
 return FALSE;
}
VERIFY(0x021A3BF8,openProc);
static BOOL draw(daKnob00_c* a) {
 WWHD_FUNC(0x021A37A8,BOOL,a);
 if(!gabi::call<BOOL>(0x0252B0FC,a,0)) { if(getShapeType(a)==1) a->actor_status=(u32)a->actor_status&~0x20u; return TRUE; }
 auto shape=getShapeType(a); auto hidden=gabi::load<u8>(gabi::ea(a)+0x5A7);
 if(shape==1) a->actor_status=(u32)a->actor_status&~0x20u;
 if(hidden) return TRUE;
 auto type=getShapeType(a)==5 ? 0:1;
 settingTevStruct(dKy_getEnvlight(),type,&a->current.pos,&a->tevStr);
 auto env=dKy_getEnvlight(); auto model2=gabi::load<u32>(gabi::ea(a)+0x590);
 setLightTevColorType(env,gabi::at<J3DModel>(model2),&a->tevStr);
 auto model=(J3DModel*)a->mpModel; f32 frame=gabi::load<f32>(gabi::ea(a)+0x508); auto data=gabi::load<u32>(gabi::ea(model)+0xAC);
 gabi::call(0x025E86B8,gabi::at<void>(gabi::ea(a)+0x504),gabi::at<void>(data),frame);
 model=(J3DModel*)a->mpModel; gabi::call(0x027F4D5C,model);
 model=(J3DModel*)a->mpModel; auto joint=gabi::load<s8>(gabi::ea(a)+0x5A5); auto block=gabi::load<u32>(gabi::ea(model)+0x2C);
 auto flags=gabi::load<u16>(block+4); auto matrices=gabi::load<u32>(block+0x10); model2=gabi::load<u32>(gabi::ea(a)+0x590);
 gabi::store<u16>(block+4,flags|0x10);
 f32 values[12]; for(u32 i=0;i<12;i++) values[i]=gabi::load<f32>(matrices+0x30*(s32)joint+4*i);
 for(u32 i=0;i<12;i++) gabi::store<f32>(model2+0xC8+4*i,values[i]);
 model2=gabi::load<u32>(gabi::ea(a)+0x590); gabi::call(0x025E2DE0,gabi::at<J3DModel>(model2),0); return TRUE;
}
VERIFY(0x021A37A8,draw);
static BOOL Draw(daKnob00_c* a) { WWHD_FUNC(0x021A393C,BOOL,a); return draw(a); }
VERIFY(0x021A393C,Draw);
static BOOL CheckCreateHeap(daKnob00_c* a) { WWHD_FUNC(0x021A37A4,BOOL,a); return gabi::call<BOOL>(0x021A34F0,a); }
VERIFY(0x021A37A4,CheckCreateHeap);
static BOOL actionInit(daKnob00_c* a) {
 WWHD_FUNC(0x021A4D10,BOOL,a);
 if(gabi::load<u32>((u32)a->mpBgW)>=0x100) {
  if(drawCheckShip(a)) { u32 p=play(); u32 bg=a->mpBgW; gabi::call(0x024EEA6C,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg),a); }
  else gabi::store<u8>(gabi::ea(a)+0x5A7,1);
 }
 u8 type=a->mDoorType;
 if(type==6) {
  if(!eventBit(0x3401)) { actionFigure(a); a->mAction=10; return TRUE; }
  type=a->mDoorType;
 }
 if(type==5) {
  if(!eventBit(0x2D80)) { actionVilla(a); a->mAction=8; return TRUE; }
  type=a->mDoorType;
 }
 if(type==4) {
  if(!eventBit(0x1910)) { a->mAction=5; actionPassward(a); return TRUE; }
  if(gabi::call<BOOL>(0x02520C0C,0x31)) { a->mAction=11; return TRUE; }
 }
 if(msgDoor(a)&&!chkException(a)) { actionTalkWait(a); a->mAction=4; }
 else { actionWait(a); a->mAction=1; }
 return TRUE;
}
VERIFY(0x021A4D10,actionInit);
static BOOL Delete(daKnob00_c* a) {
 WWHD_FUNC(0x021A52D8,BOOL,a);
 if(a->heap!=nullptr) { u32 bg=a->mpBgW;
  if(bg&&gabi::load<u32>(bg)<0x100) { u32 p=play(); bg=a->mpBgW; gabi::call(0x020087EC,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg)); }
 }
 gabi::call(0x025204C8,gabi::at<u8>(gabi::ea(a)+0x4F8),STR(0x100134A0)); return TRUE;
}
VERIFY(0x021A52D8,Delete);
static BOOL CreateInit(daKnob00_c* a) {
 WWHD_FUNC(0x021A5348,BOOL,a);
 s8 room=a->current.roomNo;
 f32 att=gabi::load<f32>(gabi::ea(a)+0x394)+150.0f;
 f32 eye=a->eyePos.y+150.0f;
 a->tevStr.mRoomNo=room; gabi::store<f32>(gabi::ea(a)+0x394,att); a->eyePos.y=eye;
 a->mAction=0; gabi::store<u32>(gabi::ea(a)+0x39C,0x20);
 calcMtx(a); gabi::call(0x024F43DC,gabi::at<u8>((u32)a->mpBgW));
 gabi::call(0x0252A9E0,a,0); a->mDoorType=gabi::call<u8>(0x0252AA2C,a);
 if(getShapeType(a)==4&&a->mDoorType==1) {
  s8 rn=a->current.roomNo; a->mDoorType=5;
  gabi::call(0x025D5834,0x1A2,0xFFFFFF03,&a->current.pos,rn,&a->current.angle,(void*)nullptr,0,(void*)nullptr);
  att=gabi::load<f32>(gabi::ea(a)+0x394)+60.0f; eye=a->eyePos.y+60.0f;
  gabi::store<f32>(gabi::ea(a)+0x394,att); a->eyePos.y=eye;
 }
 if(getShapeType(a)==6) { a->mDoorType=6; a->home.angle.z=0x366A; }
 return TRUE;
}
VERIFY(0x021A5348,CreateInit);
static s32 create(daKnob00_c* a) {
 WWHD_FUNC(0x021A5464,s32,a);
 s32 phase=gabi::call<s32>(0x02520460,gabi::at<u8>(gabi::ea(a)+0x4F8),STR(0x100134A0));
 u32 condition=a->actor_condition;
 if(!(condition&8)) {
 if(a) {
 gabi::call(0x0252A244,a); a->__vtbl=0x100133A0;
 gabi::call(0x025DD5F0,gabi::at<u8>(gabi::ea(a)+0x3F4));
 gabi::store<u32>(gabi::ea(a)+0x4A8,0x10013360);
 gabi::call(0x027F2BC0,gabi::at<u8>(gabi::ea(a)+0x504),0);
 gabi::store<u32>(gabi::ea(a)+0x514,0x1016E54C);
 gabi::call(0x027DA984,gabi::at<u8>(gabi::ea(a)+0x518));
 gabi::store<u32>(gabi::ea(a)+0x55C,0); gabi::store<u32>(gabi::ea(a)+0x584,0);
 gabi::store<u32>(gabi::ea(a)+0x514,0x10013338); gabi::store<u32>(gabi::ea(a)+0x588,0);
 gabi::store<u32>(gabi::ea(a)+0x54C,0x1016D820); gabi::store<u32>(gabi::ea(a)+0x58C,0); gabi::store<u32>(gabi::ea(a)+0x580,0);
 gabi::call(0x0252BA44,gabi::at<u8>(gabi::ea(a)+0x598)); condition=a->actor_condition;
 }
 a->actor_condition=condition|8;
 }
 if(phase!=4) return phase;
 if(a->current.roomNo==-1) a->current.roomNo=gabi::call<s8>(0x0252A37C,a);
 if(!gabi::call<BOOL>(0x025D63E8,a,0x021A37A4,0x2700)) return 5;
 CreateInit(a); return 4;
}
VERIFY(0x021A5464,create);
static s32 Create(daKnob00_c* a) { WWHD_FUNC(0x021A55CC,s32,a); return create(a); }
VERIFY(0x021A55CC,Create);
static BOOL Execute(daKnob00_c* a) {
 WWHD_FUNC(0x021A5008,BOOL,a);
 if(gabi::load<u8>(gabi::ea(a)+0x5A7)&&drawCheckShip(a)) {
 gabi::store<u8>(gabi::ea(a)+0x5A7,0); u32 p=play(); u32 bg=a->mpBgW; gabi::call(0x024EEA6C,gabi::at<u8>(p+0x12A0),gabi::at<u8>(bg),a);
 }
 switch(gabi::call<s32>(0x0252AC98,a)) {
 case 0: a->mAction=0; break;
 case 1: gabi::call(0x0252AD50,a); demoProc(a); break;
 case 2:
 switch((u8)a->mAction) {
 case 0: actionInit(a); break;
 case 1: actionWait(a); break;
 case 2: actionDemo(a); break;
 case 3: case 7: case 9: actionTalk(a); break;
 case 4: actionTalkWait(a); break;
 case 5: actionPassward(a); break;
 case 6: gabi::call<BOOL>(0x021A4808,a); break;
 case 8: actionVilla(a); break;
 case 10: actionFigure(a); break;
 } break;
 default: gabi::call(0x0273AA24,STR(0x100133B0),0x4E7,STR(0x100132F4));
 }
 gabi::store<u8>(gabi::ea(a)+0x3AC,gabi::load<u8>(0x1047E6C8));
 if(getType2(a)==1) { calcMtx(a); u32 bg=a->mpBgW; if(bg&&gabi::load<u32>(bg)<0x100) gabi::call(0x024F43DC,gabi::at<u8>(bg)); }
 return TRUE;
}
VERIFY(0x021A5008,Execute);
static void setStart(daKnob00_c* a,f32 lateral,f32 forward) {
 WWHD_FUNC(0x021A39CC,void,a,lateral,forward);
 u32 p=play(); u16 angle=(s16)a->shape_angle.y+0x7FFF;
 u32 trig=0x104A44F8+8*(angle>>3); f32 cosine=gabi::load<f32>(trig+4),sine=gabi::load<f32>(trig);
 f32 dx=gabi::fmsubs(sine,forward,cosine*lateral),dz=gabi::fmadds(cosine,forward,sine*lateral);
 f32 x=a->current.pos.x,z=a->current.pos.z,y=a->current.pos.y;
 u32 player=gabi::load<u32>(p+0x5B2C); gabi::Local<cXyz> pos; pos->set(x+dx,y,z+dz);
 u32 vt=gabi::load<u32>(player+0xB4),target=gabi::load<u32>(vt+0x114); s16 rot=gabi::load<s16>(player+0x32A);
 gabi::call(target,gabi::at<u8>(player),(cXyz*)pos,rot);
}
VERIFY(0x021A39CC,setStart);
static BOOL adjustmentProc(daKnob00_c* a) {
 WWHD_FUNC(0x021A3E90,BOOL,a);
 u32 p=play(),player=gabi::load<u32>(p+0x5B2C);
 u16 angle=(s16)a->shape_angle.y+0x7FFF; u32 trig=0x104A44F8+8*(angle>>3);
 f32 px=gabi::load<f32>(player+0x314),py=gabi::load<f32>(player+0x318),pz=gabi::load<f32>(player+0x31C);
 f32 targetZ=gabi::fmadds(gabi::load<f32>(trig+4),-70.0f,(f32)a->current.pos.z);
 f32 targetX=gabi::fmadds(gabi::load<f32>(trig),-70.0f,(f32)a->current.pos.x);
 s16 timer=a->mAdjustTimer; f32 targetY=a->current.pos.y;
 gabi::Local<cXyz> pos;
 if(timer>0) pos->set(gabi::fmadds(px,0.8f,targetX*0.2f),py,gabi::fmadds(pz,0.8f,targetZ*0.2f));
 else pos->set(targetX,targetY,targetZ);
 u32 vt=gabi::load<u32>(player+0xB4),target=gabi::load<u32>(vt+0x114); s16 rot=gabi::load<s16>(player+0x322);
 gabi::call(target,gabi::at<u8>(player),(cXyz*)pos,rot);
 if(timer>0) { a->mAdjustTimer=(s16)a->mAdjustTimer-1; return FALSE; } return TRUE;
}
VERIFY(0x021A3E90,adjustmentProc);
static void cutEnd(daKnob00_c* a) { s32 staff=a->mStaffId; u32 p=play(); gabi::call(0x02543280,gabi::at<u8>(p+0x52C4),staff); }
static BOOL demoProcImpl(daKnob00_c* a) {
 WWHD_FUNC(0x021A3FB8,BOOL,a);
 s32 action=gabi::call<s32>(0x0252A684,a); s32 staff=a->mStaffId; u32 p=play();
 if(gabi::call<BOOL>(0x025447C8,gabi::at<u8>(p+0x52C4),staff)) {
 switch(action) {
 case 9: setStart(a,0,-70); break;
 case 17: case 18:
 setStart(a,action==17?-43.0f:46.0f,action==17?-14.8f:-86.0f);
 if(gabi::load<u32>((u32)a->mpBgW)<0x100) releaseBg(a); break;
 case 12: case 13: case 14: case 15: openInit(a,action-12); break;
 case 10: setAngle(a); break;
 case 11: a->mAdjustTimer=10; break;
 case 16: gabi::call(0x0252BEC4,gabi::at<u8>(gabi::ea(a)+0x3EC),(s16)a->home.angle.z); break;
 }
 }
 switch(action) {
 case 12: case 13: case 14: case 15:
 if((u8)a->mFlags&1) { if(openProc(a,action)) { openEnd(a); cutEnd(a); } } else cutEnd(a); break;
 case 11: if(adjustmentProc(a)) cutEnd(a); break;
 case 16: {
 u32 pos=gabi::ea(a)+0x390;
 if(a->mAction==9) { u32 mt=gabi::call<u32>(0x025D9F38,STR(0x10013474),0,0); if(mt) pos=mt+0x390; }
 return gabi::call<BOOL>(0x0252BEDC,gabi::at<u8>(gabi::ea(a)+0x3EC),gabi::at<cXyz>(pos))!=0;
 }
 default: cutEnd(a);
 }
 return FALSE;
}
VERIFY(0x021A3FB8,demoProcImpl);
static void onEventBit(u16 bit) { u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025B8B68,gabi::at<u8>(save+0x644),bit); }
static BOOL actionPassward2(daKnob00_c* a) {
 WWHD_FUNC(0x021A4808,BOOL,a);
 auto msg=gabi::at<u8>(gabi::ea(a)+0x3EC);
 switch((u8)a->mPasswordState) {
 case 0:
 if(demoProc2(a)) {
 a->mPasswordState=(u8)a->mPasswordState+1;
 if(!eventBit(0x3B20)) { onEventBit(0x3B20); f32 random=gabi::call<f32>(0x020198D8,6.0f); u32 save=gabi::load<u32>(0x101F84DC); gabi::call(0x025B8AF4,gabi::at<u8>(save+0x644),0xBA0F,(u8)gabi::ftoi(random)); }
 u32 save=gabi::load<u32>(0x101F84DC); s32 reg=gabi::call<u8>(0x025B8BB0,gabi::at<u8>(save+0x644),0xBA0F);
 gabi::call(0x0252BEC4,msg,(s16)(reg+0x1B1A));
 } break;
 case 1:
 if(gabi::call<BOOL>(0x0252BEDC,msg,gabi::at<cXyz>(gabi::ea(a)+0x390))) { a->mPasswordState=(u8)a->mPasswordState+1; u32 p=play(); gabi::store<u8>(p+0x5BDC,2); } break;
 case 2: {
 u32 p=play(); u8 state=gabi::load<u8>(p+0x5BDC);
 if(state>1) break;
 if(state==1&&gabi::call<s32>(0x021A45C8,a)!=-1) { onEventBit(0x1910); a->mPasswordState=(u8)a->mPasswordState+1; gabi::call(0x0252BEC4,msg,0x1B18); }
 else { a->mPasswordState=10; gabi::call(0x0252BEC4,msg,0x1B19); } break;
 }
 case 3: case 10: {
 u8 state=a->mPasswordState;
 if(gabi::call<BOOL>(0x0252BEDC,msg,gabi::at<cXyz>(gabi::ea(a)+0x390))) { a->mAction=state==3?1:5; resetEvent(); a->shape_angle.y=a->current.angle.y; } break;
 }
 }
 return TRUE;
}
VERIFY(0x021A4808,actionPassward2);
static void copyMatrix(u32 src,u32 dst) { f32 m[12]; for(u32 i=0;i<12;i++) m[i]=gabi::load<f32>(src+4*i); for(u32 i=0;i<12;i++) gabi::store<f32>(dst+4*i,m[i]); }
static BOOL CreateHeap(daKnob00_c* a) {
 WWHD_FUNC(0x021A34F0,BOOL,a);
 auto data=dComIfG_getObjectRes(STR(0x100134A0),9,0x100132F8);
 if(!data) gabi::call(0x0273AA24,STR(0x100133D8),0xA6,STR(0x100133E8));
 auto model=mDoExt_J3DModel__create((J3DModelData*)data,0,0x11020203); a->mpModel=model;
 if(!model) return FALSE;
 f32 x=a->scale.x,z=a->scale.z,y=a->scale.y;
 gabi::store<f32>(gabi::ea(model)+0xBC,x); gabi::store<f32>(gabi::ea(model)+0xC4,z); gabi::store<f32>(gabi::ea(model)+0xC0,y);
 x=a->current.pos.x; y=a->current.pos.y; z=a->current.pos.z;
 gabi::call(0x028E93CC,gabi::at<u8>(0x1048D0CC),x,y,z);
 gabi::call(0x025F1C28,gabi::at<u8>(0x1048D0CC),(s16)a->current.angle.y);
 model=a->mpModel; copyMatrix(0x1048D0CC,gabi::ea(model)+0xC8);
 auto anm=dComIfG_getObjectRes(STR(0x100134A0),5,0x100132F8);
 if(!gabi::call<BOOL>(0x025E8508,gabi::at<u8>(gabi::ea(a)+0x504),data,anm,1,0,1.0f,0,-1,0)) return FALSE;
 u32 names=gabi::call<u32>(0x027F68FC,data); u32 offset=gabi::load<u32>(names+0x10);
 s32 joint=gabi::call<s32>(0x027DF9B0,gabi::at<u8>(offset?names+0x10+offset:0),STR(0x100133CC));
 gabi::store<s8>(gabi::ea(a)+0x5A5,joint);
 if((s8)joint<0) gabi::call(0x0273AA24,STR(0x100133D8),0xC2,STR(0x100133FC));
 u8 shape=getShapeType(a); s32 file=(shape>=1&&shape<=7)?gabi::load<u8>(0x100133C3+shape):10;
 data=dComIfG_getObjectRes(STR(0x100134A0),file,0x100132F8);
 model=mDoExt_J3DModel__create((J3DModelData*)data,0x80000,0x11020002);
 gabi::store<u32>(gabi::ea(a)+0x590,gabi::ea(model)); if(!model) return FALSE;
 u32 bg=gabi::call<u32>(0x024F23F4,0); a->mpBgW=bg; if(!bg) return FALSE;
 auto bgd=dComIfG_getObjectRes(STR(0x100134A0),20,0x100132F8); if(!bgd) return FALSE;
 calcMtx(a); model=a->mpModel; bg=a->mpBgW;
 return gabi::call<BOOL>(0x0200A030,gabi::at<u8>(bg),bgd,1,gabi::at<u8>(model?gabi::ea(model)+0xC8:0))==0;
}
VERIFY(0x021A34F0,CreateHeap);
struct WideString_l { be<u32> data; be<u32> vtable; };
struct WideBuffer_l { be<u32> data; be<u32> vtable; be<s32> capacity; be<u16> chars[18]; };
static void assureString(WideString_l* s) { u32 vt=s->vtable; u32 target=gabi::load<u32>(vt+0x14); gabi::call(target,s); }
static s32 chkPassward(daKnob00_c* a) {
 WWHD_FUNC(0x021A45C8,s32,a);
 u32 p=play(); u32 vt=gabi::load<u32>(p+0x5BF4),target=gabi::load<u32>(vt+0x14);
 gabi::call(target,gabi::at<u8>(p+0x5BF0));
 u32 input=gabi::load<u32>(p+0x5BF0);
 gabi::Local<WideString_l> source; source->data=input; source->vtable=0x10013310;
 gabi::Local<WideBuffer_l> entered; entered->data=gabi::ea(&entered->chars[0]); entered->vtable=0x10013370; entered->capacity=17; entered->chars[16]=0;
 gabi::call(0x021A56E8,(WideString_l*)source);
 u32 text=source->data; s32 length=0;
 if(gabi::load<u16>(text)) {
 do { ++length; text+=2; if(length>0x40000) { length=0; break; } } while(gabi::load<u16>(text));
 }
 s32 capacity=entered->capacity;
 if(length>=capacity) length=capacity-1;
 assureString(source);
 text=source->data;
 gabi::call(0xC0009988,gabi::at<u8>(gabi::ea(&entered->chars[0])),gabi::at<u8>(text),length*2,0);
 gabi::Local<WideBuffer_l> expected; expected->chars[16]=0; expected->chars[0]=0; expected->capacity=17;
 entered->chars[length]=0; entered->vtable=0x10013388;
 expected->data=gabi::ea(&expected->chars[0]); expected->vtable=0x10013388;
 if(!eventBit(0x2110)) return -1;
 u32 save=gabi::load<u32>(0x101F84DC); s32 reg=gabi::call<u8>(0x025B8BB0,gabi::at<u8>(save+0x644),0xBA0F);
 assureString((WideString_l*)(WideBuffer_l*)expected);
 u32 out=expected->data;
 gabi::call(0x025F8A08,gabi::at<u8>(gabi::load<u32>(0x101F4B5C)),gabi::at<u8>(out),reg+0x1B37);
 assureString((WideString_l*)(WideBuffer_l*)expected);
 assureString((WideString_l*)(WideBuffer_l*)expected);
 u32 lhs=expected->data;
 assureString((WideString_l*)(WideBuffer_l*)entered);
 u32 rhs=entered->data;
 if(lhs==rhs) return reg;
 lhs=expected->data;
 for(u32 i=0;i<0x40001;i++) { u16 x=gabi::load<u16>(lhs),y=gabi::load<u16>(rhs); if(x!=y) return -1; if(!x) return reg; lhs+=2; rhs+=2; }
 return -1;
}
VERIFY(0x021A45C8,chkPassward);
static void sinit_knob00() { WWHD_FUNC(0x021A55D0,void,(u32)0); sinit_header_statics(0x10464C20,0x101B8638); }
VERIFY(0x021A55D0,sinit_knob00);
static void trivialDestructor(void* a,s32 flags) { WWHD_FUNC(0x021A5664,void,a,flags); if(a&&(flags&1)) operator_delete(a); }
VERIFY(0x021A5664,trivialDestructor);
static void knobDestructor(daKnob00_c* a,s32 flags) {
 WWHD_FUNC(0x021A5678,void,a,flags);
 if(a) { gabi::call(0x027F3628,gabi::at<u8>(gabi::ea(a)+0x514),0); gabi::call(0x025DD630,gabi::at<u8>(gabi::ea(a)+0x3F4),0); gabi::call(0x025D50BC,a,0); if(flags&1) operator_delete(a); }
}
VERIFY(0x021A5678,knobDestructor);
static void emptyVirtual2(void* a) { WWHD_FUNC(0x021A56E4,void,a); }
VERIFY(0x021A56E4,emptyVirtual2);
static void emptyString(void* a) { WWHD_FUNC(0x021A56E8,void,a); }
VERIFY(0x021A56E8,emptyString);
static void terminateWideString(WideBuffer_l* a) { WWHD_FUNC(0x021A56EC,void,a); s32 capacity=a->capacity; u32 data=a->data; gabi::store<u16>(data+2*(u32)capacity-2,0); }
VERIFY(0x021A56EC,terminateWideString);
