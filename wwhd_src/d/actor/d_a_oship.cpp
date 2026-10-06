/* Great Sea warship. Actual HD TU: 0x023C5054..0x023C84E3. */
#include "d/actor/d_a_oship.h"
namespace {
constexpr u32 OshipHIO=0x1046CB48;
void OshipGetArg(daOship_c* ship) {
 WWHD_FUNC(0x023C5890,void,ship);
 u32 params=gabi::load<u32>(gabi::ea(ship)+0xB0);
 s16 angle=gabi::load<s16>(gabi::ea(ship)+0x2F8);
 ship->subMode=params; ship->triforceMap=(params>>8)&15;
 ship->pathId=(params>>16)&255; ship->switchA=params>>24;
 ship->salvagePoint=(params>>12)&15; ship->switchB=angle;
 ship->modelType=(u16(angle)>>8)&255;
 gabi::store<s16>(gabi::ea(ship)+0x328,0);
 gabi::store<s16>(gabi::ea(ship)+0x2F8,0);
 gabi::store<s16>(gabi::ea(ship)+0x320,0);
}
VERIFY(0x023C5890,OshipGetArg);
void OshipSetAttention(daOship_c* ship) {
 WWHD_FUNC(0x023C641C,void,ship);
 u32 ea=gabi::ea(ship);
 u32 z=gabi::load<u32>(ea+0x31C),y=gabi::load<u32>(ea+0x318);
 gabi::store<u32>(ea+0x384,z);
 u32 x=gabi::load<u32>(ea+0x314);
 gabi::store<u32>(ea+0x380,y); gabi::store<u32>(ea+0x390,x);
 gabi::store<u32>(ea+0x398,z); gabi::store<u32>(ea+0x37C,x);
 gabi::store<u32>(ea+0x394,y);
 u32 play=gabi::call<u32>(0x025200D4);
 if(!gabi::load<u8>(play+0x5292)) {
  f32 attentionY=gabi::load<f32>(ea+0x394);
  attentionY+=gabi::load<f32>(OshipHIO+0xC);
  f32 eyeY=gabi::load<f32>(ea+0x380);
  gabi::store<f32>(ea+0x394,attentionY);
  eyeY+=gabi::load<f32>(OshipHIO+0x10);
  gabi::store<f32>(ea+0x380,eyeY);
 }
}
VERIFY(0x023C641C,OshipSetAttention);
void OshipDamageInit(daOship_c* ship) {
 WWHD_FUNC(0x023C6F94,void,ship);
 ship->attackTimer=60; ship->swayAmount=300;
}
VERIFY(0x023C6F94,OshipDamageInit);
void OshipRangeAInit(daOship_c* ship) {
 WWHD_FUNC(0x023C7034,void,ship);
 ship->attackTimer=gabi::load<s16>(OshipHIO+0x34);
}
VERIFY(0x023C7034,OshipRangeAInit);
void OshipRangeBInit(daOship_c* ship) {
 WWHD_FUNC(0x023C7044,void,ship);
 ship->attackTimer=gabi::load<s16>(OshipHIO+(ship->subMode==0?0x34:0x36));
}
VERIFY(0x023C7044,OshipRangeBInit);
void OshipRangeCInit(daOship_c* ship) {
 WWHD_FUNC(0x023C7070,void,ship);
 ship->attackTimer=gabi::load<s16>(OshipHIO+0x36);
}
VERIFY(0x023C7070,OshipRangeCInit);
void OshipSafeStringDelete(void* object,u32 flags) {
 WWHD_FUNC(0x023C82F4,void,object,flags);
 if(object && (flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x023C82F4,OshipSafeStringDelete);
void* OshipVectorCtor(void* object) {
 WWHD_FUNC(0x023C8308,void*,object);
 return object?object:gabi::call<void*>(0x0273AD10,12);
}
VERIFY(0x023C8308,OshipVectorCtor);
BOOL OshipIsDelete(daOship_c* ship) {
 WWHD_FUNC(0x023C83C0,BOOL,ship); return TRUE;
}
VERIFY(0x023C83C0,OshipIsDelete);
void OshipFollowDelete(void* object,u32 flags) {
 WWHD_FUNC(0x023C83C8,void,object,flags);
 if(object && (flags&1)) gabi::call(0x0273AF40,object);
}
VERIFY(0x023C83C8,OshipFollowDelete);
void OshipRangeDInit(daOship_c* ship) {
 WWHD_FUNC(0x023C83DC,void,ship);
}
VERIFY(0x023C83DC,OshipRangeDInit);
void OshipSafeStringPrepare(void* object) {
 WWHD_FUNC(0x023C84E0,void,object);
}
VERIFY(0x023C84E0,OshipSafeStringPrepare);

void* OshipFollowCtor(void* object) {
 WWHD_FUNC(0x023C5670,void*,object);
 return gabi::call<void*>(0x025A5894,object,0,0);
}
VERIFY(0x023C5670,OshipFollowCtor);
void* OshipCylinderCtor(void* object) {
 WWHD_FUNC(0x023C8334,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,0x130);
 if(!object) return nullptr;
 gabi::call(0x02515FB8,object);
 u32 ea=gabi::ea(object);
 gabi::store<u32>(ea+0x114,0x100015A8);
 gabi::store<u32>(ea+0x110,0x10033FA4);
 gabi::call(0x02018590,gabi::at<u8>(ea+0x118));
 gabi::store<u32>(ea+0x3C,0x1004B108);
 gabi::store<u32>(ea+0x12C,0x1004B150);
 gabi::store<u32>(ea+0x114,0x1004B160);
 return object;
}
VERIFY(0x023C8334,OshipCylinderCtor);
void OshipModeProc(daOship_c* ship,s32 kind,s32 mode) {
 WWHD_FUNC(0x023C58DC,void,ship,kind,mode);
 u32 descriptor;
 if(kind==0) {
  u32 status=gabi::load<u32>(gabi::ea(ship)+0x2E0);
  u32 attention=gabi::load<u32>(gabi::ea(ship)+0x39C);
  ship->currentMode=mode;
  gabi::store<u32>(gabi::ea(ship)+0x2E0,status|0x20);
  gabi::store<u32>(gabi::ea(ship)+0x39C,attention|4);
  descriptor=0x100340D0+u32(mode)*20;
 } else if(kind==1) {
  mode=ship->currentMode;
  descriptor=0x100340D8+u32(mode)*20;
 } else return;
 // The HD table uses compact CodeWarrior member-function descriptors.
 s16 adjustment=gabi::load<s16>(descriptor);
 s16 slot=gabi::load<s16>(descriptor+2);
 u32 receiver=gabi::ea(ship)+s32(adjustment);
 u32 target;
 if(slot<0) target=gabi::load<u32>(descriptor+4);
 else {
  s16 vtableOffset=gabi::load<s16>(descriptor+6);
  u32 table=gabi::load<u32>(receiver+s32(vtableOffset));
  target=gabi::load<u32>(table+u32(s32(slot))*8+4);
 }
 gabi::call(target,gabi::at<u8>(receiver),kind,mode);
}
VERIFY(0x023C58DC,OshipModeProc);
void OshipRangeCheck(daOship_c* ship) {
 WWHD_FUNC(0x023C59A4,void,ship);
 u32 play=gabi::call<u32>(0x025200D4);
 u32 player=gabi::load<u32>(play+0x5B2C);
 f32 distance=gabi::call<f32>(0x025D6958,ship,gabi::at<u8>(player));
 s32 mode;
 if(distance<gabi::load<f32>(OshipHIO+0x28)) mode=4;
 else if(distance<gabi::load<f32>(OshipHIO+0x2C)) mode=5;
 else mode=distance<gabi::load<f32>(OshipHIO+0x30)?6:7;
 if(u32(ship->currentMode)!=u32(mode)) OshipModeProc(ship,0,mode);
}
VERIFY(0x023C59A4,OshipRangeCheck);
void OshipWaitInit(daOship_c* ship) {
 WWHD_FUNC(0x023C6DB0,void,ship); OshipRangeCheck(ship);
}
VERIFY(0x023C6DB0,OshipWaitInit);
void OshipWait(daOship_c* ship) {
 WWHD_FUNC(0x023C7080,void,ship); OshipRangeCheck(ship);
}
VERIFY(0x023C7080,OshipWait);

daOship_c* OshipCtor(daOship_c* ship) {
 WWHD_FUNC(0x023C567C,daOship_c*,ship);
 if(!ship) ship=gabi::call<daOship_c*>(0x0273AD10,0xF1C);
 if(!ship) return nullptr;
 gabi::call(0x025D4ED0,ship);
 u32 ea=gabi::ea(ship);
 gabi::store<u32>(ea+0xB4,0x10034084);
 // Embedded vector constructors are otherwise empty. Preserve their HD
 // allocation branch when guest address addition wraps to zero.
 for(u32 callbackOffset : {0x414u,0x478u}) {
  gabi::store<u32>(ea+callbackOffset,0x100521A8);
  for(u32 vectorOffset : {0x3Cu,0x48u,0x54u})
   if(ea+callbackOffset+vectorOffset==0) gabi::call<void*>(0x0273AD10,12);
 }
 gabi::store<u32>(ea+0x4DC,0x100521E8);
 gabi::store<u32>(ea+0x4F8,0x10052268);
 gabi::call(0x028EFFD0,gabi::at<u8>(ea+0x508),3,12,0x023C8308);
 gabi::call(0x028EFFD0,&ship->smokeCallbacks,3,0x14,0x023C5670);
 gabi::call(0x0200BD2C,&ship->collisionStatus);
 gabi::call(0x02515DA0,gabi::at<u8>(ea+0x5EC));
 gabi::store<u32>(ea+0x5E8,0x1004AE88);
 gabi::store<u32>(ea+0x5EC,0x1004AEC0);
 gabi::call(0x028EFFD0,&ship->cylinders,5,0x130,0x023C8334);
 gabi::call(0x024F0474,&ship->groundCollision);
 gabi::store<u32>(ea+0xC10,0x10033FF4);
 gabi::store<u32>(ea+0xC0C,0x10033FD4);
 gabi::store<u8>(ea+0xC14,1);
 gabi::store<u32>(ea+0xC1C,0x10033FE4);
 gabi::call(0x024EFE94,&ship->wallCircle);
 gabi::call(0x02008FEC,&ship->lineCheck);
 gabi::store<u32>(ea+0xF18,1);
 gabi::store<u8>(ea+0xF12,0); gabi::store<u8>(ea+0xF10,0);
 gabi::store<u8>(ea+0xF0D,0);
 gabi::store<u32>(ea+0xEB4,ea+0xF14);
 gabi::store<u32>(ea+0xF08,0x10034074);
 gabi::store<u32>(ea+0xEB0,ea+0xF08);
 gabi::store<u32>(ea+0xED0,0x10034054);
 gabi::store<u32>(ea+0xF14,0x10034064);
 gabi::store<u32>(ea+0xEC0,0x10034044);
 gabi::store<u8>(ea+0xF0E,0); gabi::store<u8>(ea+0xF11,0);
 gabi::store<u8>(ea+0xF0F,0); gabi::store<u8>(ea+0xF0C,1);
 return ship;
}
VERIFY(0x023C567C,OshipCtor);
void OshipDtor(daOship_c* ship,u32 flags) {
 WWHD_FUNC(0x023C83E0,void,ship,flags);
 if(!ship) return;
 u32 ea=gabi::ea(ship);
 gabi::store<u32>(ea+0xF08,0x10034034);
 gabi::store<u32>(ea+0xF14,0x10033FC4);
 gabi::store<u32>(ea+0xED0,0x10033FB4);
 gabi::call(0x02008B4C,&ship->lineCheck,0);
 gabi::call(0x02018034,gabi::at<u8>(ea+0xDD4),2);
 gabi::store<u32>(ea+0xC1C,0x10033FE4);
 gabi::store<u32>(ea+0xC10,0x10033FF4);
 gabi::call(0x024EFD9C,&ship->groundCollision,0);
 gabi::call(0x028F0164,&ship->cylinders,5,0x130,0x02515A70,0,0);
 gabi::call(0x02515860,&ship->collisionStatus,2);
 gabi::call(0x028F0164,&ship->smokeCallbacks,3,0x14,0x023C83C8,0,0);
 gabi::call(0x025D50BC,ship,0);
 if(flags&1) gabi::call(0x0273AF40,ship);
}
VERIFY(0x023C83E0,OshipDtor);

BOOL OshipCreateHeap(daOship_c* ship) {
 WWHD_FUNC(0x023C5320,BOOL,ship);
 s32 resourceIndex=3;
 if(ship->modelType!=0xFF || gabi::load<s16>(0x1047BD48)!=0) resourceIndex=4;
 gabi::Local<SafeString> name;
 name->mStringTop=0x1003429C; name->__vtbl=0x10033F8C;
 void* data=gabi::call<void*>(0x026066C4,gabi::at<u8>(gabi::load<u32>(0x101F4F28)),name.get(),resourceIndex);
 if(!data) gabi::call(0x0273AA24,STR(0x100340A4),0x63F,STR(0x100340B4));
 void* model=gabi::call<void*>(0x025E38E0,data,0x80000,0x37441422);
 ship->model=gabi::ea(model);
 if(!model) return FALSE;
 gabi::store<u32>(gabi::ea(model)+0xB8,gabi::ea(ship));
 u16 index=0;
 u32 joints=gabi::call<u32>(0x027F3F94,data);
 while(index<gabi::load<u16>(joints+8)) {
  if(index>=1 && index<=2) {
   u32 count=gabi::load<u32>(gabi::ea(data)+4);
   u32 node=gabi::load<u32>(gabi::ea(data)+8);
   if(index<count) node+=u32(index)*28;
   gabi::store<u32>(node+8,0x023C52D8);
  }
  index=u16(index+1);
  joints=gabi::call<u32>(0x027F3F94,data);
 }
 return TRUE;
}
VERIFY(0x023C5320,OshipCreateHeap);
BOOL OshipHeapCallback(daOship_c* ship) {
 WWHD_FUNC(0x023C547C,BOOL,ship); return OshipCreateHeap(ship);
}
VERIFY(0x023C547C,OshipHeapCallback);
s32 OshipDraw(daOship_c* ship) {
 WWHD_FUNC(0x023C6D3C,s32,ship);
 u32 light=gabi::call<u32>(0x02555D0C);
 gabi::call(0x025626A4,gabi::at<u8>(light),0,&ship->current.pos,gabi::at<u8>(gabi::ea(ship)+0x110));
 light=gabi::call<u32>(0x02555D0C);
 gabi::call(0x02562F5C,gabi::at<u8>(light),gabi::at<u8>(ship->model),gabi::at<u8>(gabi::ea(ship)+0x110));
 u32 model=ship->model;
 f32 scale=gabi::load<f32>(OshipHIO+0x3C);
 gabi::call(0x0252CA4C,gabi::at<u8>(gabi::ea(ship)+0x37C),gabi::at<u8>(gabi::ea(ship)+0x110),gabi::at<u8>(model),scale);
 gabi::call(0x025E2E5C,gabi::at<u8>(ship->model));
 return 1;
}
VERIFY(0x023C6D3C,OshipDraw);
s32 OshipDrawCallback(daOship_c* ship) {
 WWHD_FUNC(0x023C6DAC,s32,ship); return OshipDraw(ship);
}
VERIFY(0x023C6DAC,OshipDrawCallback);
s32 OshipLineCheck(daOship_c* ship,cXyz* start,cXyz* end) {
 WWHD_FUNC(0x023C747C,s32,ship,start,end);
 gabi::call(0x024F1AFC,&ship->lineCheck,start,end,ship);
 u32 play=gabi::call<u32>(0x025200D4);
 return gabi::call<s32>(0x02008860,gabi::at<u8>(play+0x12A0),&ship->lineCheck);
}
VERIFY(0x023C747C,OshipLineCheck);

void OshipPathMove(daOship_c* ship) {
 WWHD_FUNC(0x023C7084,void,ship);
 f32 target=ship->targetSpeed;
 f32 rate=gabi::load<f32>(0x100341FC),step=gabi::load<f32>(0x100341F4);
 gabi::call(0x0200ED84,gabi::at<u8>(gabi::ea(ship)+0x370),target,rate,step);
 f32 speed=gabi::load<f32>(gabi::ea(ship)+0x370);
 u32 path=ship->path;
 gabi::call(0x02587D24,&ship->originalPosition,&ship->pathPoint,gabi::at<u8>(path),0x023C5658,ship,speed);
 f32 radius=gabi::load<f32>(0x1047BCD0);
 radius+=gabi::load<f32>(0x10034214);
 speed=gabi::load<f32>(gabi::ea(ship)+0x370);
 gabi::call(0x0200F268,&ship->current.pos,&ship->originalPosition,radius,speed);
 speed=gabi::load<f32>(gabi::ea(ship)+0x370);
 f32 zero=gabi::load<f32>(0x100340C8);
 if(speed!=zero && f32(ship->targetSpeed)!=zero) {
  s32 yaw=gabi::call<s32>(0x0200F93C,&ship->current.pos,&ship->originalPosition);
  gabi::call(0x0200F428,gabi::at<u8>(gabi::ea(ship)+0x32A),yaw,8,0x100);
 }
}
VERIFY(0x023C7084,OshipPathMove);
void OshipDeleteInit(daOship_c* ship) {
 WWHD_FUNC(0x023C6FA8,void,ship);
 s8 room=gabi::load<s8>(gabi::ea(ship)+0x326);
 s32 reverb=gabi::call<s32>(0x02520540,room);
 gabi::call(0x025E1A40,0x6A19,gabi::at<u8>(gabi::ea(ship)+0x37C),0,reverb);
 for(u32 i=0;i<3;++i) gabi::call(0x025A5AC8,&ship->smokeCallbacks[i]);
 gabi::call(0x025D99E8,ship,&ship->current.pos,5,0,0xFF);
}
VERIFY(0x023C6FA8,OshipDeleteInit);
BOOL OshipPlayerFireCheck(daOship_c* ship) {
 WWHD_FUNC(0x023C7CA8,BOOL,ship);
 u32 play=gabi::call<u32>(0x025200D4);
 u32 boat=gabi::load<u32>(play+0x5B3C);
 if(boat && (gabi::load<u32>(boat+0x644)&0x20000)) {
  f32 range=gabi::load<f32>(0x1003423C);
  f32 delay=gabi::call<f32>(0x020198D8,range);
  delay+=gabi::load<f32>(0x100341E4);
  s32 count=ship->aimCount;
  ship->playerFireTimer=gabi::ftoi(delay);
  ship->aimCount=u32(count)+1;
 }
 if(ship->playerFireTimer!=-1 && !gabi::call<s32>(0x0211D2F8,&ship->playerFireTimer)) {
  OshipModeProc(ship,0,1); ship->playerFireTimer=-1; return TRUE;
 }
 return FALSE;
}
VERIFY(0x023C7CA8,OshipPlayerFireCheck);

OshipHIOData* OshipHIOCtor(OshipHIOData* data) {
 WWHD_FUNC(0x023C8090,OshipHIOData*,data);
 if(!data) data=gabi::call<OshipHIOData*>(0x0273AD10,0x94);
 if(!data) return nullptr;
 // Named settings retain the actual HD load/store order.
 f32 setting0=gabi::load<f32>(0x10034248);
 data->debug04=0x0;
 f32 setting1=gabi::load<f32>(0x1003424C);
 data->debug07=0x0;
 data->waveOffsetZ=setting1;
 f32 setting2=gabi::load<f32>(0x1003425C);
 f32 setting3=gabi::load<f32>(0x10034254);
 data->splashTimerTarget=setting2;
 data->debug08=0x0;
 data->rangeB=setting3;
 f32 setting4=gabi::load<f32>(0x10034258);
 data->attentionOffsetY=setting0;
 data->rangeC=setting4;
 f32 setting5=gabi::load<f32>(0x100340C8);
 f32 setting6=gabi::load<f32>(0x10034240);
 data->attackDelayA=0x1E;
 data->debug05=0x0;
 data->trackOffsetY=setting6;
 f32 setting7=gabi::load<f32>(0x10034260);
 f32 setting8=gabi::load<f32>(0x100341DC);
 f32 setting9=gabi::load<f32>(0x100341F4);
 data->eyeOffsetY=setting8;
 data->trackVelocity=setting8;
 data->splashTimerMaximum=setting8;
 f32 setting10=gabi::load<f32>(0x10034278);
 data->debug06=0x0;
 data->waveFade1=setting9;
 data->trackOffsetZ=setting5;
 f32 setting11=gabi::load<f32>(0x10034250);
 data->bombOffset.x=setting10;
 f32 setting12=gabi::load<f32>(0x10034244);
 data->vtable=0x10034094;
 data->rangeA=setting11;
 data->waveSpeed=setting9;
 f32 setting13=gabi::load<f32>(0x1003427C);
 data->trackScaleY=setting12;
 data->bombSpeed=setting13;
 data->attackDelayB=0xC8;
 f32 setting14=gabi::load<f32>(0x10034280);
 data->bombOffset.z=setting5;
 data->waveMaximumVelocity=setting7;
 f32 setting15=gabi::load<f32>(0x1003426C);
 f32 setting16=gabi::load<f32>(0x10034270);
 data->collapsePosition0.z=setting15;
 f32 setting17=gabi::load<f32>(0x10034274);
 f32 setting18=gabi::load<f32>(0x10034284);
 f32 setting19=gabi::load<f32>(0x100341D4);
 data->badAimDistanceStart=setting18;
 data->collapsePosition1.y=setting19;
 f32 setting20=gabi::load<f32>(0x10034264);
 data->bombAcceleration=setting14;
 data->collapsePosition0.x=setting20;
 data->collapsePosition1.x=setting16;
 f32 setting21=gabi::load<f32>(0x10034268);
 data->angle8E=0x3000;
 data->collapsePosition0.y=setting21;
 f32 setting22=gabi::load<f32>(0x1003423C);
 data->collapsePosition1.z=setting17;
 data->pathSpeed=setting22;
 data->bombOffset.y=setting5;
 data->bombGravityDelay=0x23;
 f32 setting23=gabi::load<f32>(0x10034288);
 data->angle90=0x3500;
 data->specularScale=setting23;
 data->badAimMaximum=0x3000;
 data->node2RotationZ=0x2800;
 data->waveFadeOffset=setting5;
 return data;
}
VERIFY(0x023C8090,OshipHIOCtor);
void OshipStaticInitialize() {
 WWHD_FUNC(0x023C8254,void);
 gabi::store<u32>(0x1046CB34,0); gabi::store<u32>(0x1046CB2C,0);
 gabi::store<u32>(0x1046CB38,0); gabi::store<u32>(0x1046CB30,0);
 gabi::call(0x028F026C,gabi::at<u8>(0x101CE518));
 f32 zero=gabi::load<f32>(0x10034290),one=gabi::load<f32>(0x10034294);
 gabi::store<f32>(0x1046CB20,zero); gabi::store<f32>(0x1046CB24,one);
 gabi::call(0x028ED6F8,gabi::at<u8>(0x1046CB28));
 gabi::call(0x028F026C,gabi::at<u8>(0x101CE524));
 gabi::call(0x028EAB2C,gabi::at<u8>(0x1046CB29));
 gabi::call(0x028F026C,gabi::at<u8>(0x101CE530));
 OshipHIOCtor(gabi::at<OshipHIOData>(OshipHIO));
}
VERIFY(0x023C8254,OshipStaticInitialize);

void OshipRangeUpdate(daOship_c* ship,bool allowSubmode1) {
 u8 submode=ship->subMode;
 if((submode==2 || (allowSubmode1 && submode==1)) && ship->pathId!=0xFF) {
  ship->targetSpeed=gabi::load<f32>(OshipHIO+0x38);
  OshipPathMove(ship);
 }
 u32 play=gabi::call<u32>(0x025200D4);
 u32 player=gabi::load<u32>(play+0x5B2C);
 u32 ea=gabi::ea(ship);
 gabi::store<u32>(ea+0xE0C,gabi::load<u32>(player+0x314));
 gabi::store<u32>(ea+0xE10,gabi::load<u32>(player+0x318));
 gabi::store<u32>(ea+0xE14,gabi::load<u32>(player+0x31C));
 ship->current.pos.y=gabi::call<f32>(0x025871F8,&ship->current.pos,&ship->groundCollision);
 if(gabi::call<s32>(0x023C714C,ship)) return;
 if(OshipPlayerFireCheck(ship)) return;
 if(!gabi::call<s32>(0x0211D2F8,&ship->attackTimer)) OshipModeProc(ship,0,1);
 else OshipRangeCheck(ship);
}
void OshipRangeA(daOship_c* ship) {
 WWHD_FUNC(0x023C7D74,void,ship); OshipRangeUpdate(ship,true);
}
VERIFY(0x023C7D74,OshipRangeA);
void OshipRangeB(daOship_c* ship) {
 WWHD_FUNC(0x023C7E58,void,ship); OshipRangeUpdate(ship,true);
}
VERIFY(0x023C7E58,OshipRangeB);
void OshipRangeC(daOship_c* ship) {
 WWHD_FUNC(0x023C7F3C,void,ship); OshipRangeUpdate(ship,false);
}
VERIFY(0x023C7F3C,OshipRangeC);
void OshipRangeD(daOship_c* ship) {
 WWHD_FUNC(0x023C8018,void,ship);
 if(gabi::call<s32>(0x023C714C,ship)) return;
 if(ship->subMode==2 && ship->pathId!=0xFF) {
  ship->targetSpeed=gabi::load<f32>(OshipHIO+0x38); OshipPathMove(ship);
 }
 ship->current.pos.y=gabi::call<f32>(0x025871F8,&ship->current.pos,&ship->groundCollision);
 OshipRangeCheck(ship);
}
VERIFY(0x023C8018,OshipRangeD);
void OshipDamage(daOship_c* ship) {
 WWHD_FUNC(0x023C77C0,void,ship);
 s16 debug=gabi::load<s16>(0x1047BD4C);
 ship->swayTimer=u16(ship->swayTimer)+u32(s32(debug)+0x1830);
 gabi::call(0x0200F428,&ship->swayAmount,0,10,10);
 f32 amount=f32(s16(ship->swayAmount));
 u16 phase=ship->swayTimer;
 f32 sine=gabi::load<f32>(0x104A44F8+(u32(phase)>>3)*8);
 f32 sway=gabi::fmuls_ppc(amount,sine);
 f32 scale=gabi::load<f32>(0x10034218);
 f32 water=gabi::call<f32>(0x025871F8,&ship->current.pos,&ship->groundCollision);
 ship->current.pos.y=gabi::fmadds(sway,scale,water);
 if(!gabi::call<s32>(0x023C714C,ship)) {
  if(!gabi::call<s32>(0x0211D2F8,&ship->attackTimer) || ship->attackTimer<0) OshipRangeCheck(ship);
 }
}
VERIFY(0x023C77C0,OshipDamage);

BOOL OshipPathStep(daOship_c* ship,cXyz* position,cXyz* start,cXyz* end) {
 WWHD_FUNC(0x023C5480,BOOL,ship,position,start,end);
 u32 ea=gabi::ea(ship),a=gabi::ea(start),b=gabi::ea(end);
 gabi::store<u32>(ea+0x3E4,gabi::load<u32>(a));
 u32 startY=gabi::load<u32>(a+4);
 f32 height=ship->current.pos.y;
 gabi::store<u32>(ea+0x3E8,startY);
 u32 startZ=gabi::load<u32>(a+8);
 ship->pathStart.y=height;
 gabi::store<u32>(ea+0x3EC,startZ);
 gabi::store<u32>(ea+0x3FC,gabi::load<u32>(b));
 gabi::store<u32>(ea+0x400,gabi::load<u32>(b+4));
 u32 endZ=gabi::load<u32>(b+8);
 ship->pathEnd.y=height;
 gabi::store<u32>(ea+0x404,endZ);
 gabi::Local<cXyz> direction,delta,distanceVector,finalDelta,finalVector;
 gabi::call(0x0201ADE0,&ship->pathEnd,direction.get(),&ship->pathStart);
 if(!gabi::call<s32>(0x0201B47C,direction.get())) return TRUE;
 f32 z=direction->z,x=direction->x;
 s32 angle=gabi::call<s32>(0x020195B0,x,z);
 u16 difference=gabi::call<u32>(0x0200F378,gabi::at<u8>(ea+0x322),angle,8,0x200,8);
 f32 cosine=gabi::load<f32>(0x104A44FC+(u32(difference)>>3)*8);
 f32 speed=gabi::load<f32>(ea+0x370);
 f32 step=gabi::fmuls_ppc(speed,std::fabs(cosine));
 gabi::call(0x0200F764,position,&ship->pathEnd,step);
 gabi::call(0x0201ADE0,position,delta.get(),&ship->pathEnd);
 x=delta->x;
 f32 zero=gabi::load<f32>(0x100340C8);
 z=delta->z;
 distanceVector->y=zero;distanceVector->x=x;distanceVector->z=z;
 f32 square=gabi::call<f32>(0x028E8DD0,distanceVector.get());
 f64 distance=gabi::call<f64>(0x028F4384,f64(square));
 f32 factor=gabi::load<f32>(0x1047BCE4);
 factor+=gabi::load<f32>(0x100340CC);
 f32 threshold=gabi::fmuls_ppc(step,factor);
 if(distance<threshold) return TRUE;
 gabi::call(0x0201ADE0,position,finalDelta.get(),&ship->pathEnd);
 x=finalDelta->x;z=finalDelta->z;
 finalVector->x=x;finalVector->y=zero;finalVector->z=z;
 square=gabi::call<f32>(0x028E8DD0,finalVector.get());
 distance=gabi::call<f64>(0x028F4384,f64(square));
 return distance==zero;
}
VERIFY(0x023C5480,OshipPathStep);
BOOL OshipPathCallback(cXyz* position,cXyz* start,cXyz* end,daOship_c* ship) {
 WWHD_FUNC(0x023C5658,BOOL,position,start,end,ship);
 return OshipPathStep(ship,position,start,end);
}
VERIFY(0x023C5658,OshipPathCallback);

void OshipNodeControl(daOship_c* ship,void* node,void* model) {
 WWHD_FUNC(0x023C5054,void,ship,node,model);
 u32 joint=gabi::call<u32>(0x027F7878,node);
 u16 index=gabi::load<u16>(joint+4);
 u32 modelEA=gabi::ea(model);
 u32 matrices=gabi::load<u32>(modelEA+0x2C);
 u32 matrixData=gabi::load<u32>(matrices+0x10);
 u16 flags=gabi::load<u16>(matrices+4);
 gabi::store<u16>(matrices+4,flags|0x10);
 auto* matrix=gabi::at<u8>(0x1048D0CC);
 gabi::call(0x028E90D4,gabi::at<u8>(matrixData+u32(index)*48),matrix);
 gabi::Local<csXyz> rotation;
 gabi::call(0x0201A478,rotation.get(),0,0,0);
 if(index==1) rotation->x=ship->aimYaw;
 else if(index==2) {
  s16 nodeRotation=gabi::load<s16>(OshipHIO+0x92);
  gabi::call(0x025F1C5C,matrix,nodeRotation);
  rotation->z=s16(-s32(s16(ship->aimPitch)));
 }
 s16 y=rotation->y,x=rotation->x,z=rotation->z;
 gabi::call(0x025F1B48,matrix,x,y,z);
 gabi::call(0x028E90D4,matrix,gabi::at<u8>(0x104B4868));
 matrices=gabi::load<u32>(modelEA+0x2C);
 flags=gabi::load<u16>(matrices+4);
 gabi::store<u16>(matrices+4,flags|0x10);
 // HD loads all twelve floating values before writing the joint matrix.
 f32 values[12];
 for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(0x1048D0CC+4*i);
 matrixData=gabi::load<u32>(matrices+0x10)+u32(index)*48;
 for(u32 i=0;i<12;++i) gabi::store<f32>(matrixData+4*i,values[i]);
}
VERIFY(0x023C5054,OshipNodeControl);
s32 OshipNodeCallback(void* node,s32 timing) {
 WWHD_FUNC(0x023C52D8,s32,node,timing);
 if(timing==0) {
  u32 model=gabi::load<u32>(0x104B462C);
  auto* ship=gabi::at<daOship_c>(gabi::load<u32>(model+0xB8));
  if(ship) OshipNodeControl(ship,node,gabi::at<u8>(model));
 }
 return 1;
}
VERIFY(0x023C52D8,OshipNodeCallback);

void OshipSetCollision(daOship_c* ship) {
 WWHD_FUNC(0x023C64A0,void,ship);
 u16 angle=gabi::load<u16>(gabi::ea(ship)+0x322);
 f32 height=gabi::load<f32>(0x100341E8);
 u32 trig=0x104A44F8+(u32(angle)>>3)*8;
 f32 sine=gabi::load<f32>(trig),cosine=gabi::load<f32>(trig+4);
 gabi::Local<cXyz> center;
 for(u32 i=0;i<5;++i) {
  f32 offset=gabi::load<f32>(0x101CE4DC+4*i);
  f32 x=ship->current.pos.x,y=ship->current.pos.y;
  f32 centerX=gabi::fmadds(offset,sine,x);
  f32 z=ship->current.pos.z;
  f32 centerY=y-height,centerZ=gabi::fmadds(offset,cosine,z);
  center->x=centerX;center->y=centerY;center->z=centerZ;
  u32 cylinder=gabi::ea(ship)+0x60C+0x130*i;
  gabi::call(0x020182E0,gabi::at<u8>(cylinder+0x118),center.get());
  f32 radius=gabi::load<f32>(0x101CE4F0+4*i);
  gabi::call(0x020184DC,gabi::at<u8>(cylinder+0x118),radius);
  f32 cylinderHeight=gabi::load<f32>(0x101CE504+4*i);
  gabi::call(0x02018428,gabi::at<u8>(cylinder+0x118),cylinderHeight);
  u32 play=gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240,gabi::at<u8>(play+0x26A4),gabi::at<u8>(cylinder));
 }
}
VERIFY(0x023C64A0,OshipSetCollision);

void OshipSetWave(daOship_c* ship) {
 WWHD_FUNC(0x023C65C0,void,ship);
 u32 ea=gabi::ea(ship);
 f32 cutoff=gabi::load<f32>(0x100341F4);
 f32 speed=gabi::load<f32>(ea+0x370);
 f32 splashTarget=gabi::load<f32>(OshipHIO+0x48);
 f32 fade=gabi::load<f32>(OshipHIO+0x50);
 f32 maximum=gabi::load<f32>(OshipHIO+0x60);
 f32 trackVelocity=gabi::load<f32>(OshipHIO+0x54);
 f32 zero=gabi::load<f32>(0x100340C8);
 if(!(speed>cutoff) || ship->currentMode==3) {
  gabi::store<s16>(ea+0x4FC,1); fade=zero;splashTarget=zero;
 } else gabi::call(0x023C5D34,ship);
 f32 water=gabi::call<f32>(0x025871F8,&ship->wavePosition,&ship->groundCollision);
 u32 trackEmitter=gabi::load<u32>(ea+0x544);
 s16 yaw=gabi::load<s16>(ea+0x32A);
 ship->wavePosition.y=water;ship->waveRotation.y=yaw;
 if(trackEmitter) {
  f32 trackY=gabi::load<f32>(OshipHIO+0x40);
  u32 collisionFlags=gabi::load<u32>(ea+0xC24);
  f32 trackScale=gabi::load<f32>(OshipHIO+0x44);
  gabi::store<f32>(ea+0x53C,trackVelocity);
  water=ship->wavePosition.y;
  gabi::store<f32>(ea+0x534,trackY);
  gabi::store<f32>(ea+0x500,water);
  gabi::store<f32>(ea+0x538,trackScale);
  f32 level=(collisionFlags&0x800)?gabi::load<f32>(ea+0xDB8):gabi::load<f32>(0x100341F8);
  f32 alpha=gabi::load<f32>(0x100341C4);
  gabi::store<f32>(ea+0x504,level);gabi::store<f32>(ea+0x540,alpha);
 }
 // Particle visibility bits are updated independently for the two waves.
 u32 emitter=gabi::load<u32>(ea+0x474);
 if(emitter) {
  u32 flags=gabi::load<u32>(emitter+0x254);
  gabi::store<u32>(emitter+0x254,fade>zero?flags&~1u:flags|1);
 }
 emitter=gabi::load<u32>(ea+0x4D8);
 if(emitter) {
  u32 flags=gabi::load<u32>(emitter+0x254);
  gabi::store<u32>(emitter+0x254,fade>zero?flags&~1u:flags|1);
 }
 gabi::store<f32>(ea+0x480,fade);gabi::store<f32>(ea+0x41C,fade);
 gabi::store<f32>(ea+0x42C,maximum);gabi::store<f32>(ea+0x490,maximum);
 f32 offset=gabi::load<f32>(OshipHIO+0x5C);
 f32 one=gabi::load<f32>(0x100340CC);
 gabi::store<f32>(ea+0x48C,offset+one);
 offset=gabi::load<f32>(OshipHIO+0x5C);
 gabi::store<f32>(ea+0x428,one-offset);
 f32 endZ=gabi::load<f32>(OshipHIO+0x78);
 f32 startZ=gabi::load<f32>(OshipHIO+0x6C);
 f32 endY=gabi::load<f32>(OshipHIO+0x74);
 f32 startY=gabi::load<f32>(OshipHIO+0x68);
 f32 endX=gabi::load<f32>(OshipHIO+0x70);
 f32 startX=gabi::load<f32>(OshipHIO+0x64);
 gabi::store<f32>(ea+0x4A0,endX);
 gabi::store<f32>(ea+0x438,startZ);
 gabi::store<f32>(ea+0x43C,-endX);
 gabi::store<f32>(ea+0x430,-startX);
 gabi::store<f32>(ea+0x4A8,endZ);
 gabi::store<f32>(ea+0x49C,startZ);
 gabi::store<f32>(ea+0x434,startY);
 gabi::store<f32>(ea+0x494,startX);
 gabi::store<f32>(ea+0x440,endY);
 gabi::store<f32>(ea+0x4A4,endY);
 gabi::store<f32>(ea+0x444,endZ);
 gabi::store<f32>(ea+0x498,startY);
 f32 waveSpeed=gabi::load<f32>(OshipHIO+0x58);
 f32 rate=gabi::load<f32>(0x100341FC);
 gabi::store<f32>(ea+0x488,waveSpeed);
 waveSpeed=gabi::load<f32>(OshipHIO+0x58);
 f32 step=gabi::load<f32>(0x100341E4);
 gabi::store<f32>(ea+0x424,waveSpeed);
 gabi::call(0x0200ED84,&ship->splashScaleTimer,splashTarget,rate,step);
 emitter=gabi::load<u32>(ea+0x4F4);
 if(emitter) {
  f32 timer=ship->splashScaleTimer;
  u32 flags=gabi::load<u32>(emitter+0x254);
  gabi::store<u32>(emitter+0x254,timer>rate?flags&~1u:flags|1);
 }
 f32 timer=ship->splashScaleTimer;
 gabi::store<f32>(ea+0x4E4,timer);
 f32 splashMaximum=gabi::load<f32>(OshipHIO+0x4C);
 gabi::store<f32>(ea+0x4E8,splashMaximum);
}
VERIFY(0x023C65C0,OshipSetWave);

s32 OshipCreate(daOship_c* ship) {
 WWHD_FUNC(0x023C6220,s32,ship);
 u32 ea=gabi::ea(ship);
 u32 condition=gabi::load<u32>(ea+0x2E4);
 if(!(condition&8)) {
  if(ship) { OshipCtor(ship);condition=gabi::load<u32>(ea+0x2E4); }
  gabi::store<u32>(ea+0x2E4,condition|8);
 }
 s32 phase=gabi::call<s32>(0x02520460,&ship->phase,STR(0x1003429C));
 if(phase!=4) return phase;
 OshipGetArg(ship);
 u8 switchA=ship->switchA;
 if(switchA!=0xFF) {
  u32 save=gabi::load<u32>(0x101F84DC);
  s8 room=gabi::load<s8>(ea+0x326);
  if(gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save+0x20),switchA,room)) return 5;
 }
 u8 switchB=ship->switchB;
 if(switchB!=0xFF) {
  u32 save=gabi::load<u32>(0x101F84DC);
  s8 room=gabi::load<s8>(ea+0x326);
  if(!gabi::call<s32>(0x025BA0C0,gabi::at<u8>(save+0x20),switchB,room)) return 5;
 }
 u8 map=ship->triforceMap;
 if(map!=15) {
  u32 save=gabi::load<u32>(0x101F84DC);
  if(!gabi::call<s32>(0x025B8308,gabi::at<u8>(save+0xE4),map)) return 5;
  u8 point=ship->salvagePoint;
  if(point!=15) {
   save=gabi::load<u32>(0x101F84DC);
   if(point!=gabi::load<u8>(save+0x1C1)) return 5;
  }
 }
 if(!gabi::call<s32>(0x025D63E8,ship,0x023C547C,0x1280)) return 5;
 gabi::call(0x023C5F84,ship);
 return phase;
}
VERIFY(0x023C6220,OshipCreate);
s32 OshipCreateCallback(daOship_c* ship) {
 WWHD_FUNC(0x023C637C,s32,ship);return OshipCreate(ship);
}
VERIFY(0x023C637C,OshipCreateCallback);
s32 OshipDelete(daOship_c* ship) {
 WWHD_FUNC(0x023C6380,s32,ship);
 gabi::call(0x025204C8,&ship->phase,STR(0x1003429C));
 for(u32 i=0;i<3;++i) {
  u32 callback=gabi::ea(ship)+0x548+0x14*i;
  u32 vtable=gabi::load<u32>(callback);
  u32 target=gabi::load<u32>(vtable+0x44);
  gabi::call(target,gabi::at<u8>(callback));
 }
 gabi::call(0x025A92C0,&ship->waveCallbacks[1]);
 gabi::call(0x025A92C0,&ship->waveCallbacks[0]);
 gabi::call(0x025A99B8,&ship->splashCallback);
 gabi::call(0x025A9E38,&ship->trackCallback);
 gabi::call(0x025E1B34,&ship->bombPosition);
 return 1;
}
VERIFY(0x023C6380,OshipDelete);
s32 OshipDeleteCallback(daOship_c* ship) {
 WWHD_FUNC(0x023C6418,s32,ship);return OshipDelete(ship);
}
VERIFY(0x023C6418,OshipDeleteCallback);

void OshipCreateWave(daOship_c* ship) {
 WWHD_FUNC(0x023C5D34,void,ship);
 u32 initialized=gabi::load<u32>(0x1046CBDC);
 f32 unit=gabi::load<f32>(0x100340CC),scaleZ=gabi::load<f32>(0x100341B8);
 if(!initialized) {
  gabi::store<f32>(0x1046CBE8,unit);
  f32 scaleX=gabi::load<f32>(0x100341BC);
  gabi::store<u32>(0x1046CBDC,1);
  gabi::store<f32>(0x1046CBE4,scaleX);gabi::store<f32>(0x1046CBEC,scaleZ);
 }
 if(!gabi::load<u32>(0x1046CBE0)) {
  gabi::store<f32>(0x1046CBF4,unit);
  f32 scaleX=gabi::load<f32>(0x100341C0);
  gabi::store<f32>(0x1046CBF8,scaleZ);gabi::store<f32>(0x1046CBF0,scaleX);
  gabi::store<u32>(0x1046CBE0,1);
 }
 u32 ea=gabi::ea(ship);
 for(u32 i=0;i<2;++i) {
  u32 emitterField=ea+0x474+0x64*i;
  if(!gabi::load<u32>(emitterField)) {
   u32 play=gabi::call<u32>(0x025200D4);
   u32 manager=gabi::load<u32>(play+0x5AB0);
   gabi::call(0x025A847C,gabi::at<u8>(manager),0,0x37,&ship->wavePosition,&ship->waveRotation,0,255,&ship->waveCallbacks[i],-1,0,0,0);
   u32 emitter=gabi::load<u32>(emitterField);
   if(emitter) {
    u32 scale=0x1046CBE4+12*i;
    gabi::store<f32>(emitter+0x28,gabi::load<f32>(scale));
    gabi::store<f32>(emitter+0x2C,gabi::load<f32>(scale+4));
    gabi::store<f32>(emitter+0x30,gabi::load<f32>(scale+8));
   }
  }
 }
 if(!gabi::load<u32>(ea+0x4F4)) {
  u32 play=gabi::call<u32>(0x025200D4);
  u32 manager=gabi::load<u32>(play+0x5AB0);
  gabi::call(0x025A847C,gabi::at<u8>(manager),0,0x35,&ship->wavePosition,&ship->waveRotation,0,255,&ship->splashCallback,-1,0,0,0);
 }
 if(!gabi::load<u32>(ea+0x544)) {
  u32 play=gabi::call<u32>(0x025200D4);
  u32 manager=gabi::load<u32>(play+0x5AB0);
  gabi::call(0x025A847C,gabi::at<u8>(manager),5,0x36,&ship->trackPosition,gabi::at<u8>(ea+0x328),0,0,&ship->trackCallback,-1,0,0,0);
  u32 emitter=gabi::load<u32>(ea+0x544);
  if(emitter) {
   f32 scale=gabi::load<f32>(0x100341C4);
   for(u32 offset : {0x224u,0x240u,0x238u,0x23Cu,0x220u,0x228u}) gabi::store<f32>(emitter+offset,scale);
  }
 }
}
VERIFY(0x023C5D34,OshipCreateWave);

void OshipSetMatrix(daOship_c* ship) {
 WWHD_FUNC(0x023C5A54,void,ship);
 s16 sway=ship->swayAmount;
 s16 debug=gabi::load<s16>(0x1047BD4E);
 f32 amplitude=gabi::fmuls_ppc(f32(sway),f32(s32(debug)+1));
 gabi::call(0x025872F4,&ship->current.pos,&ship->wave,amplitude);
 debug=gabi::load<s16>(0x1047BD52);sway=ship->swayAmount;
 s32 strength=s32(sway)*(s32(debug)+10);
 u16 phase=ship->swayTimer;
 f32 sine=gabi::load<f32>(0x104A44F8+(u32(phase)>>3)*8);
 s16 oscillation=gabi::ftoi(gabi::fmuls_ppc(f32(strength),sine));
 u32 play=gabi::call<u32>(0x025200D4);
 u32 player=gabi::load<u32>(play+0x5B2C);
 s32 playerAngle=gabi::call<s32>(0x025D6894,ship,gabi::at<u8>(player));
 s16 shapeYaw=gabi::load<s16>(gabi::ea(ship)+0x32A);
 u16 angle=u32(playerAngle)+s32(shapeYaw);
 u32 trig=0x104A44F8+(u32(angle)>>3)*8;
 f32 cosine=gabi::load<f32>(trig+4);sine=gabi::load<f32>(trig);
 s16 pitch=gabi::ftoi(gabi::fmuls_ppc(cosine,f32(oscillation)));
 s16 roll=gabi::ftoi(gabi::fmuls_ppc(sine,f32(oscillation)));
 s32 mode=ship->currentMode;
 u32 ea=gabi::ea(ship);
 f32 scaleY=gabi::load<f32>(ea+0x334),scaleX=gabi::load<f32>(ea+0x330);
 if(mode!=3) {
  s16 waveX=ship->wave.rotationX,waveZ=ship->wave.rotationZ;
  gabi::store<s16>(ea+0x328,s32(waveX)+s32(pitch));
  gabi::store<s16>(ea+0x32C,s32(waveZ)+s32(roll));
 }
 u32 model=ship->model;
 f32 scaleZ=gabi::load<f32>(ea+0x338);
 gabi::store<f32>(model+0xBC,scaleX);gabi::store<f32>(model+0xC0,scaleY);gabi::store<f32>(model+0xC4,scaleZ);
 f32 x=ship->current.pos.x,z=ship->current.pos.z,y=ship->current.pos.y;
 auto* matrix=gabi::at<u8>(0x1048D0CC);
 gabi::call(0x028E93CC,matrix,x,y,z);
 pitch=gabi::load<s16>(ea+0x328);roll=gabi::load<s16>(ea+0x32C);
 gabi::call(0x025F19F8,matrix,pitch,0,roll);
 shapeYaw=gabi::load<s16>(ea+0x32A);
 gabi::call(0x025F1C28,matrix,shapeYaw);
 gabi::call(0x028E90D4,matrix,&ship->flagMatrix);
 f32 values[12];
 for(u32 i=0;i<12;++i) values[i]=gabi::load<f32>(0x1048D0CC+4*i);
 model=ship->model;
 for(u32 i=0;i<12;++i) gabi::store<f32>(model+0xC8+4*i,values[i]);
 f32 attentionHeight=gabi::load<f32>(OshipHIO+0xC);
 gabi::Local<cXyz> local;
 gabi::store<u32>(gabi::ea(local.get()),0);gabi::store<u32>(gabi::ea(local.get())+4,0);gabi::store<u32>(gabi::ea(local.get())+8,0);
 local->y=attentionHeight;
 gabi::call(0x028E8F64,matrix,local.get(),gabi::at<u8>(ea+0x390));
 local->y=gabi::load<f32>(OshipHIO+0x10);
 gabi::call(0x028E8F64,matrix,local.get(),gabi::at<u8>(ea+0x37C));
 f32 zero=gabi::load<f32>(0x100340C8),waveOffset=gabi::load<f32>(OshipHIO+0x14);
 local->y=zero;local->z=waveOffset;
 gabi::call(0x028E8F64,matrix,local.get(),&ship->wavePosition);
 local->z=gabi::load<f32>(OshipHIO+0x18);
 gabi::call(0x028E8F64,matrix,local.get(),&ship->trackPosition);
}
VERIFY(0x023C5A54,OshipSetMatrix);

void OshipCreateInit(daOship_c* ship) {
 WWHD_FUNC(0x023C5F84,void,ship);
 u32 ea=gabi::ea(ship),play=gabi::call<u32>(0x025200D4);
 u32 distance=gabi::call<u32>(0x0200E814,gabi::at<u8>(play+0x50AC),STR(0x100341EC),0);
 gabi::store<u32>(ea+0x3A4,distance);gabi::store<u8>(ea+0x3A0,3);
 u32 x=gabi::load<u32>(ea+0x314),y=gabi::load<u32>(ea+0x318);
 gabi::store<u32>(ea+0x3D4,x);gabi::store<u8>(ea+0x3A1,3);
 u32 z=gabi::load<u32>(ea+0x31C);
 gabi::store<u32>(ea+0x3D8,y);ship->playerFireTimer=-1;
 gabi::store<u32>(ea+0x3DC,z);
 OshipRangeCheck(ship);
 gabi::store<u32>(ea+0x39C,4);gabi::store<u8>(ea+0x38A,0x22);
 gabi::call(0x02515F14,&ship->collisionStatus,255,0,ship);
 for(u32 i=0;i<5;++i) {
  u32 cylinder=ea+0x60C+0x130*i;
  gabi::call(0x02516518,gabi::at<u8>(cylinder),gabi::at<u8>(0x100342A4));
  gabi::store<u32>(cylinder+0x44,ea+0x5D0);
 }
 f32 range=gabi::load<f32>(0x100341C8);
 ship->wave.animationX=gabi::ftoi(gabi::call<f32>(0x020198D8,range));
 ship->wave.animationZ=gabi::ftoi(gabi::call<f32>(0x020198D8,range));
 OshipSetMatrix(ship);
 gabi::call(0x027F4D5C,gabi::at<u8>(ship->model));
 u32 model=ship->model;
 gabi::store<u32>(ea+0x348,model?model+0xC8:0);
 f32 maximumZ=gabi::load<f32>(0x100341CC);
 f32 maximumX=gabi::load<f32>(0x100341DC);
 f32 maximumY=gabi::load<f32>(0x100341E0);
 f32 minimumX=gabi::load<f32>(0x100341D0);
 f32 minimumY=gabi::load<f32>(0x100341D4);
 f32 minimumZ=gabi::load<f32>(0x100341D8);
 gabi::call(0x025D674C,ship,minimumX,minimumY,minimumZ,maximumX,maximumY,maximumZ);
 u8 type=ship->modelType;
 f32 farDistance=gabi::load<f32>(0x100341E4);
 gabi::store<f32>(ea+0x364,farDistance);
 if(type==0xFF && !gabi::load<s16>(0x1047BD48)) {
  s8 layer=gabi::load<s8>(ea+0x1C9);
  ship->flagId=gabi::call<u32>(0x025D5834,0xAE,4,&ship->current.pos,layer,gabi::at<u8>(ea+0x320),0,-1,0);
  if(gabi::load<u32>(0x1046CBFC)) {
   f32 zOffset=gabi::load<f32>(0x1046CB44),yOffset=gabi::load<f32>(0x1046CB40),xOffset=gabi::load<f32>(0x1046CB3C);
   ship->flagOffset.y=yOffset;ship->flagOffset.z=zOffset;ship->flagOffset.x=xOffset;
  } else {
   gabi::store<f32>(0x1046CB40,maximumZ);
   f32 zero=gabi::load<f32>(0x100340C8);
   gabi::store<u32>(0x1046CBFC,1);
   gabi::store<f32>(0x1046CB3C,zero);gabi::store<f32>(0x1046CB44,zero);
   ship->flagOffset.x=zero;ship->flagOffset.y=maximumZ;ship->flagOffset.z=zero;
  }
 }
 u8 path=ship->pathId;
 if(path!=0xFF) {
  s8 room=gabi::load<s8>(ea+0x326);
  ship->path=gabi::call<u32>(0x025AAF88,path,room);
 }
 OshipCreateWave(ship);
 f32 radius=gabi::load<f32>(0x100341E8);
 gabi::call(0x024EFF44,&ship->wallCircle,radius,radius);
 gabi::call(0x024F06B4,&ship->groundCollision,&ship->current.pos,gabi::at<u8>(ea+0x300),ship,1,&ship->wallCircle,gabi::at<u8>(ea+0x33C),0,0);
 u32 collisionFlags=gabi::load<u32>(ea+0xC24);
 gabi::store<u32>(ea+0xC24,collisionFlags|12);
}
VERIFY(0x023C5F84,OshipCreateInit);

void OshipAttackInit(daOship_c* ship) {
 WWHD_FUNC(0x023C6DB4,void,ship);
 ship->attackTimer=-1;
 u32 play=gabi::call<u32>(0x025200D4);
 u32 player=gabi::load<u32>(play+0x5B2C),ea=gabi::ea(ship);
 gabi::store<u32>(ea+0xE0C,gabi::load<u32>(player+0x314));
 gabi::store<u32>(ea+0xE10,gabi::load<u32>(player+0x318));
 gabi::store<u32>(ea+0xE14,gabi::load<u32>(player+0x31C));
 gabi::Local<cXyz> delta,horizontal;
 gabi::call(0x0201ADE0,&ship->current.pos,delta.get(),&ship->targetPosition);
 f32 x=delta->x,zero=gabi::load<f32>(0x100340C8);
 horizontal->x=x;f32 z=delta->z;horizontal->y=zero;horizontal->z=z;
 f32 square=gabi::call<f32>(0x028E8DD0,horizontal.get());
 f64 distance=gabi::call<f64>(0x028F4384,f64(square));
 f32 start=gabi::load<f32>(OshipHIO+0x88),error=zero;
 if(distance>start) {
  f32 excess=f32(distance-f64(start));
  f32 scale=gabi::load<f32>(0x100341BC);
  error=gabi::fmuls_ppc(excess,scale);
 }
 f32 base=gabi::load<f32>(0x100341DC),chance=gabi::load<f32>(0x10034208);
 error+=base;
 f32 random=gabi::call<f32>(0x020198D8,chance);
 if(random<gabi::load<f32>(0x100341E4)) error=zero;
 play=gabi::call<u32>(0x025200D4);
 u32 playerStatus=gabi::load<u32>(play+0x5CD8);
 if(playerStatus&0x01100000) {
  ship->aimCount=0;
  u8 debug=gabi::load<u8>(OshipHIO+8);
  f32 extra=gabi::load<f32>(0x1003420C);
  error+=extra;
  if(debug) error=zero;
 } else {
  s32 count=ship->aimCount;
  if(count<6) {
   f32 scale=gabi::load<f32>(0x10034210);
   s32 remaining=s32(6u-u32(count));
   error=gabi::fmadds(f32(remaining),scale,error);
  }
  if(gabi::load<u8>(OshipHIO+8)) error=zero;
 }
 play=gabi::call<u32>(0x025200D4);
 player=gabi::load<u32>(play+0x5B2C);
 u16 angle=gabi::call<u32>(0x025D6894,ship,gabi::at<u8>(player));
 u32 trig=0x104A44F8+(u32(angle)>>3)*8;
 x=ship->targetPosition.x;
 f32 sine=gabi::load<f32>(trig);
 f32 targetX=gabi::fnmsubs(error,sine,x);
 z=ship->targetPosition.z;
 ship->targetPosition.x=targetX;
 f32 cosine=gabi::load<f32>(trig+4);
 ship->targetPosition.z=gabi::fnmsubs(error,cosine,z);
}
VERIFY(0x023C6DB4,OshipAttackInit);

void OshipAttackCannon(daOship_c* ship,s32 slot) {
 WWHD_FUNC(0x023C74C0,void,ship,slot);
 u32 ea=gabi::ea(ship);
 s16 shapeX=gabi::load<s16>(ea+0x328),aimPitch=ship->aimPitch,aimYaw=ship->aimYaw;
 s16 shapeY=gabi::load<s16>(ea+0x32A),shapeZ=gabi::load<s16>(ea+0x32C);
 gabi::Local<csXyz> angle;
 angle->x=s32(shapeX)-s32(aimPitch);angle->z=shapeZ;angle->y=s32(shapeY)+s32(aimYaw);
 u32 params=gabi::call<u32>(0x020CB8D8,4,1,1);
 s8 layer=gabi::load<s8>(ea+0x1C9);
 u32 bomb=gabi::call<u32>(0x025D5928,0x126,params,&ship->bombPosition,layer,angle.get(),0,-1,0,0);
 u32 id=bomb?gabi::load<u32>(bomb+4):0xFFFFFFFF;
 gabi::store<u32>(ea+0xE70+u32(slot)*4,id);
 gabi::store<u8>(ea+0xEA8+u32(slot),1);
 s16 gravityDelay=gabi::load<s16>(OshipHIO+0x84);
 gabi::call(0x020CB89C,gabi::at<u8>(bomb),gravityDelay);
 u16 pitch=angle->x;
 u32 trig=0x104A44F8+(u32(pitch)>>3)*8;
 f32 speed=gabi::load<f32>(OshipHIO+0x7C),cosine=gabi::load<f32>(trig+4);
 gabi::store<f32>(bomb+0x370,gabi::fmuls_ppc(cosine,speed));
 pitch=angle->x;trig=0x104A44F8+(u32(pitch)>>3)*8;
 f32 sine=gabi::load<f32>(trig);
 speed=gabi::load<f32>(OshipHIO+0x7C);
 f32 vertical=-gabi::fmuls_ppc(sine,speed);
 f32 scale=gabi::load<f32>(0x100341F4);
 gabi::store<f32>(bomb+0x340,vertical);
 f32 acceleration=gabi::load<f32>(OshipHIO+0x80);
 gabi::store<f32>(bomb+0x330,scale);gabi::store<f32>(bomb+0x374,acceleration);
 gabi::store<f32>(bomb+0x334,scale);gabi::store<f32>(bomb+0x338,scale);
 gabi::call(0x025E19CC,0x2852,&ship->bombPosition);
}
VERIFY(0x023C74C0,OshipAttackCannon);

void OshipAttack(daOship_c* ship) {
 WWHD_FUNC(0x023C760C,void,ship);
 if(ship->pathId!=0xFF) {
  ship->targetSpeed=gabi::load<f32>(0x100340C8);OshipPathMove(ship);
 }
 if(gabi::call<s32>(0x023C714C,ship)) return;
 f32 water=gabi::call<f32>(0x025871F8,&ship->current.pos,&ship->groundCollision);
 s32 timer=ship->attackTimer;
 ship->current.pos.y=water;
 if(timer<0) {
  s16 angle=ship->aimYaw,target=ship->targetAimYaw;
  gabi::call<s32>(0x0200FAAC,angle,target);
  angle=ship->aimPitch;target=ship->targetAimPitch;
  gabi::call<s32>(0x0200FAAC,angle,target);
  ship->attackTimer=-1;
  if(gabi::load<u8>(OshipHIO+6) || OshipLineCheck(ship,&ship->smokePosition,&ship->targetPosition)) {
   OshipRangeCheck(ship);return;
  }
  f32 cutoff=gabi::load<f32>(0x100341F4);
  for(u32 i=0;i<5;++i) {
   if(!ship->bombAllocated[i]) {
    f32 speed=gabi::load<f32>(gabi::ea(ship)+0x370);
    if(!(speed>cutoff)) {
     OshipAttackCannon(ship,i);ship->swayAmount=100;ship->attackTimer=15;return;
    }
   }
  }
  return;
 }
 if(!gabi::call<s32>(0x0211D2F8,&ship->attackTimer)) {
  OshipRangeCheck(ship);return;
 }
 s16 debug=gabi::load<s16>(0x1047BD4C);
 s16 sway=ship->swayTimer;
 ship->swayTimer=s32(sway)+s32(debug)+0x1830;
 gabi::call(0x0200F428,&ship->swayAmount,0,10,10);
}
VERIFY(0x023C760C,OshipAttack);

BOOL OshipCheckTargetHit(daOship_c* ship) {
 WWHD_FUNC(0x023C714C,BOOL,ship);
 u32 ea=gabi::ea(ship);
 gabi::call(0x02515E50,gabi::at<u8>(ea+0x5EC));
 if(gabi::load<u8>(OshipHIO+5)) {
  gabi::store<u8>(ea+0x3A1,0);OshipModeProc(ship,0,3);return TRUE;
 }
 if(gabi::call<s32>(0x0211D2F8,&ship->hitTimer)) return FALSE;
 gabi::Local<cXyz> hitPosition,scale;
 u32 hit=0;
 for(u32 i=0;i<5;++i) {
  u32 cylinder=ea+0x60C+0x130*i;
  hit=gabi::call<u32>(0x02516300,gabi::at<u8>(cylinder));
  u32 target=gabi::ea(hitPosition.get());
  gabi::store<u32>(target,gabi::load<u32>(cylinder+0xCC));
  gabi::store<u32>(target+4,gabi::load<u32>(cylinder+0xD0));
  gabi::store<u32>(target+8,gabi::load<u32>(cylinder+0xD4));
  gabi::call<u32>(0x02516300,gabi::at<u8>(cylinder));
  if(hit) break;
 }
 if(!hit || gabi::load<u32>(hit+0x10)!=32) return FALSE;
 u8 health=gabi::load<u8>(ea+0x3A1);
 s32 count=ship->smokeCount;
 gabi::store<u8>(ea+0x3A1,u32(health)-1);
 if(count<3 && !gabi::load<u32>(ea+0x54C+u32(count)*20)) {
  s32 yaw=gabi::call<s32>(0x0200F93C,&ship->current.pos,hitPosition.get());
  count=ship->smokeCount;
  s16 shapeYaw=gabi::load<s16>(ea+0x32A);
  gabi::store<s16>(ea+0x5BA+u32(count)*2,u32(yaw)-s32(shapeYaw));
  count=ship->smokeCount;
  u32 rotation=ea+0x5A8+u32(count)*6;
  u16 value=gabi::load<u16>(ea+0x328);gabi::store<u16>(rotation,value);
  value=gabi::load<u16>(ea+0x32A);gabi::store<u16>(rotation+2,value);
  value=gabi::load<u16>(ea+0x32C);gabi::store<u16>(rotation+4,value);
  count=ship->smokeCount;
  rotation=ea+0x5A8+u32(count)*6;
  s16 relativeYaw=gabi::load<s16>(ea+0x5BA+u32(count)*2);
  s16 rotationY=gabi::load<s16>(rotation+2);
  gabi::store<s16>(rotation+2,s32(rotationY)+s32(relativeYaw));
  count=ship->smokeCount;
  rotation=ea+0x5A8+u32(count)*6;
  s8 room=gabi::load<s8>(ea+0x326);
  u32 callback=ea+0x548+u32(count)*20;
  u32 play=gabi::call<u32>(0x025200D4);
  u32 manager=gabi::load<u32>(play+0x5AB0);
  gabi::call(0x025A847C,gabi::at<u8>(manager),0,0x3E1,&ship->smokePosition,gabi::at<u8>(rotation),gabi::at<u8>(ea+0x330),255,gabi::at<u8>(callback),room,0,0,0);
  ship->smokeCount=u32(s32(ship->smokeCount))+1;
 }
 u32 play=gabi::call<u32>(0x025200D4);
 u32 player=gabi::load<u32>(play+0x5B2C);
 play=gabi::call<u32>(0x025200D4);
 u32 manager=gabi::load<u32>(play+0x5AB0);
 gabi::call(0x025A847C,gabi::at<u8>(manager),0,0x10,hitPosition.get(),0,0,255,0,-1,0,0,0);
 f32 size=gabi::load<f32>(0x100341F4);
 scale->x=size;scale->y=size;scale->z=size;
 play=gabi::call<u32>(0x025200D4);
 manager=gabi::load<u32>(play+0x5AB0);
 gabi::call(0x025A847C,gabi::at<u8>(manager),0,0xF,hitPosition.get(),gabi::at<u8>(player+0x328),scale.get(),255,0,-1,0,0,0);
 s8 remaining=gabi::load<s8>(ea+0x3A1);
 ship->hitTimer=5;
 if(remaining<=0) {
  s8 room=gabi::load<s8>(ea+0x326);
  s32 reverb=gabi::call<s32>(0x02520540,room);
  gabi::call(0x025E1A40,0x2828,gabi::at<u8>(ea+0x37C),0,reverb);
  OshipModeProc(ship,0,3);
 } else {
  ship->aimCount=6;OshipModeProc(ship,0,2);
 }
 return TRUE;
}
VERIFY(0x023C714C,OshipCheckTargetHit);
void OshipModeDelete(daOship_c* ship) {
 WWHD_FUNC(0x023C78D0,void,ship);
 u32 ea=gabi::ea(ship);
 u8 modelType=ship->modelType;
 f32 sinkOffset=gabi::load<f32>(0x1003421C);
 f32 maxStep=gabi::load<f32>(0x10034224);
 f32 nearWater=gabi::load<f32>(0x10034210);
 f32 smoothing=gabi::load<f32>(0x10034220);
 bool special=modelType!=255 || gabi::load<s16>(0x1047BD48)!=0;
 if(special && gabi::load<u16>(ea+0xF8)!=2) {
  gabi::call(0x025D77DC,ship,gabi::at<u8>(0x10034228),1,0xFFFF);return;
 }
 s32 angleResult=gabi::call<s32>(0x0200F378,gabi::at<u8>(ea+0x328),-15000,20,0x1000,0x100);
 f32 water=gabi::call<f32>(0x025871F8,gabi::at<u8>(ea+0x314),&ship->groundCollision);
 f32 epsilon=gabi::load<f32>(0x100341E4);
 f32 result=gabi::call<f32>(0x0200ECD4,gabi::at<u8>(ea+0x318),f32(sinkOffset+water),smoothing,maxStep,epsilon);
 s16 angleTest=s16(gabi::ftoi(std::fabs(f32(angleResult))));
 bool settled=angleTest<=256 && std::fabs(f32(result-water))<nearWater;
 if(settled && ship->switchA!=255) {
  u32 save=gabi::load<u32>(0x101F84DC);
  s8 room=gabi::load<s8>(ea+0x326);
  gabi::call(0x025B9E38,gabi::at<u8>(save+0x20),u32(ship->switchA),room);
  gabi::Local<be<u16>> actorType;*actorType=0x191;
  u32 target=gabi::call<u32>(0x025D5218,0x025E121C,actorType.get());
  gabi::call(0x024679F4,gabi::at<u8>(target),ship);
  if(special) gabi::call(0x025E1988,0x806);
 }
 if(special) {
  u32 play=gabi::call<u32>(0x025200D4);
  s32 end=gabi::call<s32>(0x0254457C,gabi::at<u8>(play+0x52C4),gabi::at<u8>(0x10034228));
  if(end) {
   u32 save=gabi::load<u32>(0x101F84DC);
   gabi::call(0x025B8B68,gabi::at<u8>(save+0x644),0x3E80);
   play=gabi::call<u32>(0x025200D4);
   u16 flags=gabi::load<u16>(play+0x52B8);
   gabi::store<u16>(play+0x52B8,flags|8);
   gabi::call(0x025D57E0,ship);
  }
  return;
 }
 if(!settled) return;
 u32 flag=ship->flagId;
 gabi::Local<be<u32>> id;*id=flag;
 u32 actor=0;
 if(flag!=0xFFFFFFFF) actor=gabi::call<u32>(0x025D5218,0x025E1234,id.get());
 if(actor) gabi::call(0x025D57E0,gabi::at<u8>(actor));
 gabi::call(0x025D57E0,ship);
}
VERIFY(0x023C78D0,OshipModeDelete);
s32 OshipExecute(daOship_c* ship) {
 WWHD_FUNC(0x023C68B8,s32,ship);
 u32 ea=gabi::ea(ship);
 gabi::store<u32>(ea+0x39C,4);
 f32 particleScale=gabi::load<f32>(0x100341C4);
 gabi::store<u8>(ea+0x38A,0x22);
 for(u32 i=0;i<3;++i) {
  u32 callback=ea+0x548+i*20;
  if(gabi::load<u32>(callback+4)) {
   u32 rotation=ea+0x5A8+i*6;
   s16 x=gabi::load<s16>(ea+0x328);gabi::store<s16>(rotation,x);
   s16 y=gabi::load<s16>(ea+0x32A);gabi::store<s16>(rotation+2,y);
   s16 z=gabi::load<s16>(ea+0x32C);gabi::store<s16>(rotation+4,z);
   s16 yaw=gabi::load<s16>(ea+0x5BA+i*2);gabi::store<s16>(rotation+2,s32(y)+s32(yaw));
   u32 particle=gabi::load<u32>(callback+4);
   for(u32 off: {0x220u,0x224u,0x228u,0x238u,0x23Cu,0x240u}) gabi::store<f32>(particle+off,particleScale);
  }
 }
 f32 zeroBias=gabi::load<f32>(0x100341E4);
 f32 heightBias=gabi::load<f32>(0x10034200);
 f32 acceleration=gabi::load<f32>(0x100341FC);
 constexpr u32 settings=0x1047B608;
 gabi::Local<be<u32>> bombId;
 for(u32 i=0;i<5;++i) {
  if(!ship->bombAllocated[i]) continue;
  u32 id=ship->bombIds[i];*bombId=id;
  u32 bomb=0;
  if(id!=0xFFFFFFFF) bomb=gabi::call<u32>(0x025D5218,0x025E1234,bombId.get());
  if(!bomb) {ship->bombAllocated[i]=0;continue;}
  if(gabi::load<u8>(OshipHIO+7)==0) continue;
  f32 limit=f32(gabi::load<f32>(settings+0x6CC)+heightBias);
  f32 height=gabi::load<f32>(bomb+0x340);
  if(height>limit) continue;
  f32 target=f32(gabi::load<f32>(settings+0x6D4)+zeroBias);
  f32 max=f32(gabi::load<f32>(settings+0x6D8)+zeroBias);
  gabi::call(0x0200ED84,gabi::at<u8>(bomb+0x370),target,acceleration,max);
  f32 newHeight=f32(gabi::load<f32>(settings+0x6D0)+heightBias);
  gabi::store<f32>(bomb+0x340,newHeight);
 }
 s32 yaw=gabi::call<s32>(0x0200F93C,gabi::at<u8>(ea+0x314),&ship->targetPosition);
 ship->targetAimYaw=u32(yaw)-u32(s32(gabi::load<s16>(ea+0x32A)));
 gabi::Local<cXyz> difference,flat;
 gabi::call(0x0201ADE0,gabi::at<u8>(ea+0x314),difference.get(),&ship->targetPosition);
 f32 x=difference->x;
 f32 flatY=gabi::load<f32>(0x100340C8);
 f32 z=difference->z;
 flat->x=x;flat->z=z;flat->y=flatY;
 gabi::call(0x028E8DD0,flat.get());
 f32 distance=gabi::call<f32>(0x028F4384);
 f32 threshold=gabi::load<f32>(OshipHIO+0x88);
 s32 correction=0;
 if(distance>threshold) {
  correction=gabi::call<s32>(0x0200FAAC,s32(s16(gabi::ftoi(distance))),s32(s16(gabi::ftoi(threshold))));
  s16 maximum=gabi::load<s16>(OshipHIO+0x8C);
  if(correction>maximum) correction=maximum;
 }
 s32 pitch=gabi::call<s32>(0x0200F974,gabi::at<u8>(ea+0x314),&ship->targetPosition);
 s16 targetPitch=s16(u32(pitch)+u32(correction));
 ship->targetAimPitch=targetPitch;
 gabi::call(0x0200F428,&ship->aimPitch,s32(targetPitch),6,0x300);
 gabi::call(0x0200F428,&ship->aimYaw,s32(s16(ship->targetAimYaw)),6,0x300);
 gabi::call(0x023C58DC,ship,1,8);
 gabi::call(0x027F4D5C,gabi::at<u8>(u32(ship->model)));
 gabi::Local<cXyz> offset;
 offset->x=gabi::load<f32>(OshipHIO+0x1C);
 u32 model=ship->model;
 offset->y=gabi::load<f32>(OshipHIO+0x20);
 offset->z=gabi::load<f32>(OshipHIO+0x24);
 u32 modelData=gabi::load<u32>(model+0x2C);
 u16 flags=gabi::load<u16>(modelData+4);
 u32 matrices=gabi::load<u32>(modelData+0x10);
 gabi::store<u16>(modelData+4,flags|0x10);
 gabi::call(0x028E8F64,gabi::at<u8>(matrices+0x60),offset.get(),&ship->bombPosition);
 model=ship->model;
 gabi::Local<cXyz> origin;origin->x=0;origin->y=0;origin->z=0;
 modelData=gabi::load<u32>(model+0x2C);
 flags=gabi::load<u16>(modelData+4);
 matrices=gabi::load<u32>(modelData+0x10);
 gabi::store<u16>(modelData+4,flags|0x10);
 gabi::call(0x028E8F64,gabi::at<u8>(matrices+0x30),origin.get(),&ship->smokePosition);
 gabi::call(0x023C641C,ship);gabi::call(0x023C64A0,ship);gabi::call(0x023C5A54,ship);
 model=ship->model;
 u32 matrix=model?model+0xC8:0;
 f32 a=gabi::load<f32>(0x100341D0);
 f32 d=gabi::load<f32>(0x100341DC);
 f32 c=gabi::load<f32>(0x100341D8);
 f32 e=gabi::load<f32>(0x100341E0);
 f32 f=gabi::load<f32>(0x100341CC);
 f32 b=gabi::load<f32>(0x100341D4);
 s32 culled=gabi::call<s32>(0x025D6B70,gabi::at<u8>(matrix),a,b,c,d,e,f);
 f32 speed=gabi::load<f32>(ea+0x370);
 f32 minimum=gabi::load<f32>(0x100341F4);
 bool waves=false;
 if(speed>minimum && !culled) {
  u32 play=gabi::call<u32>(0x025200D4);
  u32 player=gabi::load<u32>(play+0x5B2C);
  f32 playerDistance=gabi::call<f32>(0x025D6958,ship,gabi::at<u8>(player));
  f32 maximum=gabi::load<f32>(0x10034204);
  waves=!(playerDistance>maximum);
 }
 if(waves) gabi::call(0x023C65C0,ship);
 else {
  gabi::call(0x025A92C0,gabi::at<u8>(ea+0x478));
  gabi::call(0x025A92C0,gabi::at<u8>(ea+0x414));
  gabi::call(0x025A99B8,gabi::at<u8>(ea+0x4DC));
  gabi::store<u16>(ea+0x4FC,1);
 }
 if(ship->modelType==255 && gabi::load<s16>(settings+0x740)==0) {
  u32 id=ship->flagId;gabi::Local<be<u32>> flagId;*flagId=id;
  u32 flag=0;
  if(id!=0xFFFFFFFF) flag=gabi::call<u32>(0x025D5218,0x025E1234,flagId.get());
  if(flag) {
   gabi::store<u32>(flag+0x1E34,ea+0xE3C);
   gabi::store<u32>(flag+0x1E38,ea+0xE30);
  }
 }
 return 0;
}
VERIFY(0x023C68B8,OshipExecute);
s32 OshipExecuteCallback(daOship_c* ship) {WWHD_FUNC(0x023C6D38,s32,ship);return OshipExecute(ship);}
VERIFY(0x023C6D38,OshipExecuteCallback);
}
