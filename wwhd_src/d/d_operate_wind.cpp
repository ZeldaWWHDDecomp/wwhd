#include "wwhd.h"
#include "gabi.h"
// Qualified modern WindControl UI group;

void Wind_WeakDtor(u32 s,u32 flag) {

 WWHD_FUNC(0x0264D388,void,s,flag);

if(s&&(flag&1))gabi::call<void>(0x0273AF40,s);

}

VERIFY(0x0264D388,Wind_WeakDtor);


void Wind_Empty_0264D39C() {

 WWHD_FUNC(0x0264D39C,void);


}

VERIFY(0x0264D39C,Wind_Empty_0264D39C);


void Wind_Empty_0264D3A0() {

 WWHD_FUNC(0x0264D3A0,void);


}

VERIFY(0x0264D3A0,Wind_Empty_0264D3A0);


void Wind_Empty_0264D3A4() {

 WWHD_FUNC(0x0264D3A4,void);


}

VERIFY(0x0264D3A4,Wind_Empty_0264D3A4);


void Wind_Empty_0264D3A8() {

 WWHD_FUNC(0x0264D3A8,void);


}

VERIFY(0x0264D3A8,Wind_Empty_0264D3A8);


void Wind_Empty_0264D3AC() {

 WWHD_FUNC(0x0264D3AC,void);


}

VERIFY(0x0264D3AC,Wind_Empty_0264D3AC);


void Wind_Empty_0264D3B0() {

 WWHD_FUNC(0x0264D3B0,void);


}

VERIFY(0x0264D3B0,Wind_Empty_0264D3B0);


void Wind_Empty_0264D3B4() {

 WWHD_FUNC(0x0264D3B4,void);


}

VERIFY(0x0264D3B4,Wind_Empty_0264D3B4);


void Wind_Empty_0264D3B8() {

 WWHD_FUNC(0x0264D3B8,void);


}

VERIFY(0x0264D3B8,Wind_Empty_0264D3B8);


void Wind_Empty_0264D3C4() {

 WWHD_FUNC(0x0264D3C4,void);


}

VERIFY(0x0264D3C4,Wind_Empty_0264D3C4);


void Wind_Empty_0264D3C8() {

 WWHD_FUNC(0x0264D3C8,void);


}

VERIFY(0x0264D3C8,Wind_Empty_0264D3C8);


void Wind_Empty_0264D3CC() {

 WWHD_FUNC(0x0264D3CC,void);


}

VERIFY(0x0264D3CC,Wind_Empty_0264D3CC);


u32 Wind_StateId(u32 s) {

 WWHD_FUNC(0x0264D3BC,u32,s);

return gabi::load<u32>(s);

}

VERIFY(0x0264D3BC,Wind_StateId);


u32 Wind_Dispatch_12(u32 desc,u32 obj) {

 WWHD_FUNC(0x0264D3D0,u32,desc,obj);

s16 index=gabi::load<s16>(desc+14),adjust=gabi::load<s16>(desc+12);
u32 self=obj+s32(adjust),target;
if(index<0)target=gabi::load<u32>(desc+16);
else{
s16 vtOff=gabi::load<s16>(desc+18);
u32 vt=gabi::load<u32>(self+s32(vtOff));
target=gabi::load<u32>(vt+u32(s32(index))*8+4);
}
return gabi::call_ptr<u32>(target,self,obj);

}

VERIFY(0x0264D3D0,Wind_Dispatch_12);


u32 Wind_Dispatch_20(u32 desc,u32 obj) {

 WWHD_FUNC(0x0264D410,u32,desc,obj);

s16 index=gabi::load<s16>(desc+22),adjust=gabi::load<s16>(desc+20);
u32 self=obj+s32(adjust),target;
if(index<0)target=gabi::load<u32>(desc+24);
else{
s16 vtOff=gabi::load<s16>(desc+26);
u32 vt=gabi::load<u32>(self+s32(vtOff));
target=gabi::load<u32>(vt+u32(s32(index))*8+4);
}
return gabi::call_ptr<u32>(target,self,obj);

}

VERIFY(0x0264D410,Wind_Dispatch_20);


u32 Wind_Dispatch_28(u32 desc,u32 obj) {

 WWHD_FUNC(0x0264D450,u32,desc,obj);

s16 index=gabi::load<s16>(desc+30),adjust=gabi::load<s16>(desc+28);
u32 self=obj+s32(adjust),target;
if(index<0)target=gabi::load<u32>(desc+32);
else{
s16 vtOff=gabi::load<s16>(desc+34);
u32 vt=gabi::load<u32>(self+s32(vtOff));
target=gabi::load<u32>(vt+u32(s32(index))*8+4);
}
return gabi::call_ptr<u32>(target,self,obj);

}

VERIFY(0x0264D450,Wind_Dispatch_28);


u32 Wind_CurrentState(u32 s) {

 WWHD_FUNC(0x0264D490,u32,s);

u32 state=gabi::load<u32>(s+0x2C);
if(gabi::load<s32>(state)==-1)return gabi::load<u32>(s);
u32 vt=gabi::load<u32>(state+8);
return gabi::call_ptr<u32>(gabi::load<u32>(vt+0x14),state);

}

VERIFY(0x0264D490,Wind_CurrentState);


void Wind_Dtor(u32 s,u32 flag) {

 WWHD_FUNC(0x0264D4BC,void,s,flag);

if(!s)return;
u32 p=gabi::load<u32>(s+0x60);
gabi::store<u32>(s+4,0x100EED70);
gabi::store<u32>(s+0x2C,0x100EEEB0);
gabi::store<u32>(s+0x30,0x100EEEC0);
if(p){
u32 vt=gabi::load<u32>(p+0x24);
gabi::call_ptr<void>(gabi::load<u32>(vt+12),p,3);
}
gabi::call<void>(0x026F88F0,s,0);
if(flag&1)gabi::call<void>(0x0273AF40,s);

}

VERIFY(0x0264D4BC,Wind_Dtor);


void Wind_UpdateTimer(u32 s) {

 WWHD_FUNC(0x0264D554,void,s);

u16 timer=gabi::load<u16>(s+0x96);
if(timer){
timer=u16(timer-1);
gabi::store<u16>(s+0x96,timer);
if(!timer)gabi::store<u16>(s+0x90,u16(gabi::call<u32>(0x0257DACC)));
}
u32 play=gabi::call<u32>(0x025200D4);
if(gabi::load<u8>(play+0x5BD0)==2)gabi::call<void>(0x020063C0,s+0x18,0x10491A74);

}

VERIFY(0x0264D554,Wind_UpdateTimer);


void Wind_Rotate_84(u32 s,f32 angle) {

 WWHD_FUNC(0x0264D5C0,void,s,angle);

u32 pane=gabi::load<u32>(s+84);
u32 x=gabi::load<u32>(pane+0x28),y=gabi::load<u32>(pane+0x2C);
gabi::load<u32>(pane+0x30);
gabi::store<f32>(pane+0x28,gabi::f32_from_bits(x));
f32 yy=gabi::f32_from_bits(y);
u8 flags=gabi::load<u8>(pane+0x44);
gabi::store<f32>(pane+0x2C,yy);
gabi::store<f32>(pane+0x30,angle);
gabi::store<u8>(pane+0x44,flags|16);

}

VERIFY(0x0264D5C0,Wind_Rotate_84);


void Wind_Rotate_88(u32 s,f32 angle) {

 WWHD_FUNC(0x0264D608,void,s,angle);

u32 pane=gabi::load<u32>(s+88);
u32 x=gabi::load<u32>(pane+0x28),y=gabi::load<u32>(pane+0x2C);
gabi::load<u32>(pane+0x30);
gabi::store<f32>(pane+0x28,gabi::f32_from_bits(x));
f32 yy=gabi::f32_from_bits(y);
u8 flags=gabi::load<u8>(pane+0x44);
gabi::store<f32>(pane+0x2C,yy);
gabi::store<f32>(pane+0x30,angle);
gabi::store<u8>(pane+0x44,flags|16);

}

VERIFY(0x0264D608,Wind_Rotate_88);


void Wind_Reset(u32 s) {

 WWHD_FUNC(0x0264D650,void,s);

u32 angle=gabi::call<u32>(0x0257DACC);
u32 quantized=(angle+0x1000)&0xE000,whole=quantized<<16;
gabi::store<u16>(s+0x92,u16(quantized));
gabi::store<u32>(s+0x78,whole);
f32 degrees=(f32(s32(whole))*180.0f)/2147483648.0f;
gabi::call<void>(0x0264D5C0,s,degrees);
gabi::store<u16>(s+0x92,0);
gabi::store<u32>(s+0x78,0);
gabi::call<void>(0x0264D608,s,0.0f);
u32 obj=gabi::load<u32>(s+0x60);
gabi::store<u16>(s+0x94,gabi::load<u16>(s+0x92));
gabi::store<u32>(s+0x7C,gabi::load<u32>(s+0x78));
u32 vt=gabi::load<u32>(obj+0x24);
f32 result=gabi::call_ptr<f32>(gabi::load<u32>(vt+0x14),obj);
gabi::store<u8>(s+0x9B,0);
gabi::store<u8>(s+0x9A,0);
gabi::store<u8>(s+0x9C,0);
gabi::store<u8>(s+0x9D,0);
gabi::store<u8>(s+0x9E,0);
gabi::store<u32>(s+0x8C,0);
gabi::store<f32>(s+0x80,result);

}

VERIFY(0x0264D650,Wind_Reset);


void Wind_Enter(u32 s) {

 WWHD_FUNC(0x0264D73C,void,s);

u32 play=gabi::call<u32>(0x025200D4);
u8 b=gabi::load<u8>(play+0x5C29),a=gabi::load<u8>(play+0x5C28),c=gabi::load<u8>(play+0x5C2A);
gabi::store<u8>(play+0x5C2B,a);
gabi::store<u8>(play+0x5C2D,c);
gabi::store<u8>(play+0x5C2C,b);
play=gabi::call<u32>(0x025200D4);
gabi::store<u8>(play+0x5C28,23);
play=gabi::call<u32>(0x025200D4);
gabi::store<u8>(play+0x5C29,7);
gabi::store<u8>(0x1047B03C+0x5A,1);
gabi::store<u8>(0x1047B03C+0x40,3);
u32 layout=gabi::load<u32>(s+0x44);
gabi::call<void>(0x020053E4,gabi::load<u32>(layout+0xD4),0,0,1.0f);
gabi::call<void>(0x0264D650,s);
u32 global=gabi::load<u32>(0x101F8344),manager=gabi::load<u32>(global+0x1C8);
gabi::call<void>(0x020063C0,manager+0x18,0x10491B70);
gabi::store<u8>(s+0x98,1);
gabi::store<u8>(s+0xA0,1);

}

VERIFY(0x0264D73C,Wind_Enter);


void Wind_CheckEnter(u32 s) {

 WWHD_FUNC(0x0264D800,void,s);

u32 layout=gabi::load<u32>(s+0x44);
if(gabi::call<u32>(0x02005840,gabi::load<u32>(layout+0xD4),0))gabi::call<void>(0x020063C0,s+0x18,0x10491AA4);

}

VERIFY(0x0264D800,Wind_CheckEnter);


void Wind_Highlight(u32 s) {

 WWHD_FUNC(0x0264D850,void,s);

u32 layout=gabi::load<u32>(s+0x44);
gabi::call<void>(0x020053E4,gabi::load<u32>(layout+0xD4),1,1,1.0f);
layout=gabi::load<u32>(s+0x44);
gabi::call<void>(0x020053E4,gabi::load<u32>(layout+0xD4),2,2,1.0f);

}

VERIFY(0x0264D850,Wind_Highlight);


void Wind_ClearActive(u32 s) {

 WWHD_FUNC(0x0264DF80,void,s);

gabi::store<u8>(s+0xA0,0);

}

VERIFY(0x0264DF80,Wind_ClearActive);


void Wind_CommitWind(u32 s) {

 WWHD_FUNC(0x0264DF8C,void,s);

s32 angle=s32(gabi::load<u32>(s+0x78))/65536;
gabi::call<void>(0x0257E490,0,s16(-(angle+0x4000)));

}

VERIFY(0x0264DF8C,Wind_CommitWind);

u32 Wind_HitBlocked(u32 s) {

 WWHD_FUNC(0x0264D8C8,u32,s);

if(gabi::call<u32>(0x02617AE4,gabi::load<u32>(0x101F5088)))return 1;
u32 pad=gabi::load<u32>(0x101F5088);
u32 point=(gabi::load<u32>(pad+0x24)&1)?pad+0x1B4:0x1049FFA0;
f32 x=gabi::load<f32>(point),y=gabi::load<f32>(point+4);
gabi::store<f32>(s+0x70,x);
gabi::Local<u8[8]> xy;
gabi::store<f32>(xy.a,x);
gabi::store<f32>(xy.a+4,y);
u32 pane=gabi::load<u32>(s+0x50);
gabi::store<f32>(s+0x74,y);
if(!gabi::call<u32>(0x0287F05C,pane,xy.a))return 1;
if(!gabi::load<u8>(s+0x9B)){
f32 dx=gabi::load<f32>(s+0x70)-gabi::load<f32>(s+0x64),dy=gabi::load<f32>(s+0x74)-gabi::load<f32>(s+0x68);
f32 d=gabi::fmadds(dx,dx,dy*dy);
if(d<gabi::load<f32>(s+0x88))return 1;
}
return 0;

}

VERIFY(0x0264D8C8,Wind_HitBlocked);


u32 Wind_PadHeld() {

 WWHD_FUNC(0x0264D9B0,u32);

if(gabi::call<u32>(0x02617AE4,gabi::load<u32>(0x101F5088)))return 0;
u32 pad=gabi::load<u32>(0x101F5088);
return (gabi::load<u32>(pad+0x124)&0x8000)!=0;

}

VERIFY(0x0264D9B0,Wind_PadHeld);


void Wind_TouchAngle(u32 s) {

 WWHD_FUNC(0x0264DA14,void,s);

f32 dx=gabi::load<f32>(s+0x70)-gabi::load<f32>(s+0x64),dy=gabi::load<f32>(s+0x74)-gabi::load<f32>(s+0x68);
gabi::store<u8>(s+0x99,1);
u32 angle=gabi::call<u32>(0x020195B0,-dx,dy);
gabi::store<u16>(s+0x92,u16((angle+0x1000)&0xE000));

}

VERIFY(0x0264DA14,Wind_TouchAngle);


u32 Wind_TouchNear(u32 s) {

 WWHD_FUNC(0x0264DA70,u32,s);

gabi::Local<u8[12]> point; /* game-test Local sweep 2026-10-05: 02704D2C writes a 12-byte vector */
gabi::call<void>(0x02704D2C,point.a,gabi::load<u32>(s+0x5C));
f32 dy=gabi::load<f32>(s+0x74)-gabi::load<f32>(point.a+4),dx=gabi::load<f32>(s+0x70)-gabi::load<f32>(point.a);
return gabi::fmadds(dx,dx,dy*dy)<gabi::load<f32>(s+0x84);

}

VERIFY(0x0264DA70,Wind_TouchNear);


void Wind_RequestChange(u32 s) {

 WWHD_FUNC(0x0264DAD4,void,s);

if(!gabi::load<u8>(s+0x9D)||gabi::load<u8>(s+0x9C)||gabi::load<u8>(s+0x9F))return;
gabi::store<u8>(s+0x9C,1);
u32 global=gabi::load<u32>(0x101F8344);
gabi::call<void>(0x0264F3F4,gabi::load<u32>(global+0x1C8));

}

VERIFY(0x0264DAD4,Wind_RequestChange);


void Wind_TouchControl(u32 s) {

 WWHD_FUNC(0x0264DB10,void,s);

s32 blocked=gabi::call<s32>(0x0264D8C8,s);
u32 mode=gabi::load<u32>(s+0x8C);
if(mode==0){
if(gabi::call<u32>(0x0264D9B0,s)){
if(blocked>0){
gabi::store<u8>(s+0x9A,1);
return;
}
u8 was=gabi::load<u8>(s+0x9A);
gabi::store<u8>(s+0x9B,0);
gabi::store<u32>(s+0x8C,1);
if(was)return;
gabi::call<void>(0x02030B38,0x100EEBF4);
if(blocked==0){
gabi::call<void>(0x0264DA14,s);
if(gabi::call<u32>(0x0264DA70,s))gabi::store<u8>(s+0x9B,1);
}
}
else{
u16 angle=gabi::load<u16>(s+0x92);
gabi::store<u8>(s+0x9A,0);
gabi::store<u8>(s+0x9E,1);
gabi::store<u16>(s+0x94,angle);
gabi::call<void>(0x0264DAD4,s);
}
}
else if(mode==1){
u32 held=gabi::call<u32>(0x0264D9B0,s);
u8 near=gabi::load<u8>(s+0x9B);
if(held){
if(near){
gabi::store<u8>(s+0x9E,1);
gabi::store<u16>(s+0x94,gabi::load<u16>(s+0x92));
gabi::call<void>(0x0264DA14,s);
}
else if(gabi::call<u32>(0x0264DA70,s)){
gabi::store<u8>(s+0x9B,1);
gabi::call<void>(0x0264DA14,s);
}
}
else{
if(near)gabi::store<u8>(s+0x9B,0);
gabi::store<u32>(s+0x8C,0);
}
}

}

VERIFY(0x0264DB10,Wind_TouchControl);


void Wind_ButtonStep(u32 s) {

 WWHD_FUNC(0x0264DCF4,void,s);

if(gabi::load<u8>(s+0x99))return;
u32 pad=gabi::load<u32>(0x101F5088),mode=gabi::load<u32>(pad+0x124),a=gabi::load<u32>(pad+0x20),b=gabi::load<u32>(pad+0x18),buttons=a|b;
if(mode&0x00F00000)buttons&=0x00F00000;
if(buttons&0x00440000)gabi::store<u16>(s+0x92,u16((gabi::load<u16>(s+0x92)+0x2000)&0xE000));
else if(buttons&0x00880000)gabi::store<u16>(s+0x92,u16((gabi::load<u16>(s+0x92)-0x2000)&0xE000));

}

VERIFY(0x0264DCF4,Wind_ButtonStep);

void Wind_Move(u32 s) {

 WWHD_FUNC(0x0264DD5C,void,s);

gabi::store<u8>(s+0x99,0);
gabi::call<void>(0x0264DB10,s);
u8 near=gabi::load<u8>(s+0x9B);
gabi::store<u8>(s+0x9F,0);
if(near){
u32 whole=u32(gabi::load<u16>(s+0x92))<<16;
gabi::store<u32>(s+0x78,whole);
gabi::store<u32>(s+0x7C,whole);
gabi::call<void>(0x0264D608,s,(f32(s32(whole))*180.0f)/2147483648.0f);
}
else{
gabi::call<void>(0x0264DCF4,s);
u32 target=u32(gabi::load<u16>(s+0x92))<<16;
u32 previous=gabi::load<u32>(s+0x7C);
gabi::Local<u32> to,step;
gabi::store<u32>(to.a,target);
gabi::store<u32>(step.a,0x20000000);
if(previous!=target){
gabi::call<void>(0x02030B38,0x100EEC04);
gabi::store<u32>(s+0x7C,gabi::load<u32>(to.a));
}
gabi::call<void>(0x0271F738,s+0x78,to.a,step.a);
gabi::call<void>(0x0264D608,s,(f32(gabi::load<s32>(s+0x78))*180.0f)/2147483648.0f);
}
if(gabi::load<u8>(s+0x9E)&&gabi::load<u16>(s+0x94)!=gabi::load<u16>(s+0x92)){
gabi::store<u8>(s+0x9E,0);
gabi::store<u8>(s+0x9D,1);
gabi::call<void>(0x02030B38,0x100EEC04);
}

}

VERIFY(0x0264DD5C,Wind_Move);


void Wind_StateControl(u32 s) {

 WWHD_FUNC(0x0264DED8,void,s);

u32 root=gabi::load<u32>(0x101F8344),manager=gabi::load<u32>(root+0x1C8);
if(gabi::load<u8>(manager+0x52)){
gabi::call<void>(0x020063C0,s+0x18,0x10491AD4);
return;
}
if(gabi::load<u8>(manager+0x53)){
u32 play=gabi::call<u32>(0x025200D4);
gabi::store<u8>(play+0x5BD0,0);
gabi::call<void>(0x020063C0,s+0x18,0x10491B04);
return;
}
gabi::call<void>(0x0264DD5C,s);

}

VERIFY(0x0264DED8,Wind_StateControl);


void Wind_Confirm(u32 s) {

 WWHD_FUNC(0x0264DFAC,void,s);

u32 root=gabi::load<u32>(0x101F8344),manager=gabi::load<u32>(root+0x1C8);
if(gabi::load<u8>(manager+0x54)){
gabi::store<u16>(s+0x90,gabi::load<u16>(s+0x92));
gabi::call<void>(0x0264DF8C,s);
u32 play=gabi::call<u32>(0x025200D4);
gabi::store<u8>(play+0x5BD0,1);
gabi::call<void>(0x020063C0,s+0x18,0x10491B04);
}

}

VERIFY(0x0264DFAC,Wind_Confirm);


void Wind_ExitAnim(u32 s) {

 WWHD_FUNC(0x0264E018,void,s);

u32 layout=gabi::load<u32>(s+0x44);
gabi::call<void>(0x020053E4,gabi::load<u32>(layout+0xD4),3,0,1.0f);

}

VERIFY(0x0264E018,Wind_ExitAnim);


void Wind_CheckExit(u32 s) {

 WWHD_FUNC(0x0264E034,void,s);

u32 layout=gabi::load<u32>(s+0x44);
if(gabi::call<u32>(0x02005840,gabi::load<u32>(layout+0xD4),3))gabi::call<void>(0x020063C0,s+0x18,0x10491A44);

}

VERIFY(0x0264E034,Wind_CheckExit);


void Wind_RestoreGame(u32 s) {

 WWHD_FUNC(0x0264E084,void,s);

u32 play=gabi::call<u32>(0x025200D4);
gabi::store<u8>(play+0x5BD0,0);
gabi::store<u8>(s+0x98,0);
play=gabi::call<u32>(0x025200D4);
u8 c=gabi::load<u8>(play+0x5C2D),b=gabi::load<u8>(play+0x5C2C);
gabi::store<u8>(play+0x5C2A,c);
u8 a=gabi::load<u8>(play+0x5C2B);
gabi::store<u8>(play+0x5C29,b);
gabi::store<u8>(play+0x5C28,a);
gabi::store<u8>(0x1047B096,0);

}

VERIFY(0x0264E084,Wind_RestoreGame);


u32 Wind_Ctor(u32 s) {

 WWHD_FUNC(0x0264E0E8,u32,s);

if(!s)s=gabi::call<u32>(0x0273AD10,0xA4);
if(s){
gabi::call<void>(0x026F89E8,s);
gabi::store<u32>(s+0x30,0x100EEEC0);
gabi::store<u32>(s+4,0x100EED70);
gabi::store<u32>(s+0x2C,0x100EEEB0);
if(s+0x50==0)gabi::call<u32>(0x0273AD10,4);
gabi::store<u32>(s+0x58,0);
gabi::store<f32>(s+0x6C,0);
gabi::store<u32>(s+0x54,0);
gabi::store<f32>(s+0x68,0);
gabi::store<u32>(s+0x60,0);
u32 vec=s+0x70;
gabi::store<f32>(s+0x64,0);
if(!vec)vec=gabi::call<u32>(0x0273AD10,8);
if(vec){
gabi::store<f32>(vec+4,0);
gabi::store<f32>(vec,0);
}
gabi::store<u16>(s+0x90,0);
gabi::store<u8>(s+0xA0,0);
gabi::store<u8>(s+0x9F,0);
gabi::store<u16>(s+0x96,2);
gabi::store<u8>(s+0x9C,0);
gabi::store<u32>(s+0x7C,0);
gabi::store<u8>(s+0x9A,0);
gabi::store<u8>(s+0x9E,0);
gabi::store<f32>(s+0x84,0);
gabi::store<u32>(s+0x78,0);
gabi::store<u16>(s+0x92,0);
gabi::store<u8>(s+0x98,0);
gabi::store<f32>(s+0x80,0);
gabi::store<u8>(s+0x9B,0);
gabi::store<u8>(s+0x9D,0);
gabi::store<u8>(s+0x99,0);
gabi::store<f32>(s+0x88,0);
gabi::store<u32>(s+0x8C,0);
u32 manager=gabi::call<u32>(0x0258861C,0,5,2,3,2,0x1000,0x2000,1.0f,1.0f);
gabi::store<u32>(s+0x60,manager);
}
return s;

}

VERIFY(0x0264E0E8,Wind_Ctor);


void Wind_CenterPane(u32 s) {

 WWHD_FUNC(0x0264E240,void,s);

u32 layout=gabi::load<u32>(s+0x44),nameVt=gabi::load<u32>(0x104919AC),a=gabi::load<u32>(layout+4),owner=gabi::load<u32>(a+12),table=gabi::load<u32>(owner+8);
gabi::call_ptr<void>(gabi::load<u32>(nameVt+0x14),0x104919A8);
u32 pane=gabi::call_ptr<u32>(gabi::load<u32>(table+0x5C),owner,gabi::load<u32>(0x104919A8),1);
gabi::store<u32>(s+0x50,pane);
gabi::store<u32>(s+0x64,gabi::load<u32>(pane+0x1C));
gabi::store<u32>(s+0x68,gabi::load<u32>(pane+0x20));
gabi::store<u32>(s+0x6C,gabi::load<u32>(pane+0x24));

}

VERIFY(0x0264E240,Wind_CenterPane);


void Wind_Execute(u32 s) {

 WWHD_FUNC(0x0264E560,void,s);

u32 vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0xAC),s);
vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0x84),s);
gabi::call<void>(0x026F91F8,s);

}

VERIFY(0x0264E560,Wind_Execute);


void Wind_Animate(u32 s) {

 WWHD_FUNC(0x0264E5B4,void,s);

gabi::call<void>(0x02006364,s+0x18);
u32 vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0xB4),s);
vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0x8C),s);

}

VERIFY(0x0264E5B4,Wind_Animate);


void Wind_Draw(u32 s) {

 WWHD_FUNC(0x0264E60C,void,s);

if(gabi::load<u8>(s+0x98)){
u32 vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0x9C),s);
}

}

VERIFY(0x0264E60C,Wind_Draw);


u32 Wind_LoadLayout(u32 s,u32 archive,u32 a,u32 b,u32 c,u32 d) {

 WWHD_FUNC(0x0264E628,u32,s,archive,a,b,c,d);

u32 global=gabi::load<u32>(0x1018C404);
u32 layout=gabi::call<u32>(0x0273B050,0x110,gabi::load<u32>(global+0x10),4);
if(layout)layout=gabi::call<u32>(0x026FADB0,layout);
gabi::store<u32>(s+0x44,layout);
if(!layout)return 0;
u32 vt=gabi::load<u32>(layout+0xE0);
if(!gabi::call_ptr<u32>(gabi::load<u32>(vt+0x14),layout,archive,a,b,c,4,3,d))return 0;
for(u32 i=0;i<4;++i){
u32 target=gabi::load<u32>(s+0x44),index=gabi::load<u32>(0x100EEB3C+4*i);
gabi::call<void>(0x02004E04,gabi::load<u32>(target+0xD4),i,0x10491A14+8*i,0x104919FC+8*index);
}
vt=gabi::load<u32>(s+4);
gabi::call_ptr<void>(gabi::load<u32>(vt+0x8C),s);
return 1;

}

VERIFY(0x0264E628,Wind_LoadLayout);


static u32 wind_pane(u32 owner,u32 name) {

 u32 vt=gabi::load<u32>(name+4),target=gabi::load<u32>(vt+0x14),table=gabi::load<u32>(owner+8);

 gabi::call_ptr<void>(target,name);

 return gabi::call_ptr<u32>(gabi::load<u32>(table+0x5C),owner,gabi::load<u32>(name),1);

}

u32 Wind_Setup(u32 s,u32 archive,u32 a,u32 b) {

 WWHD_FUNC(0x0264E2C8,u32,s,archive,a,b);

 gabi::call<void>(0x026F90D8,s,0x10491A34,2);

 u32 vt=gabi::load<u32>(s+4),target=gabi::load<u32>(vt+0x7C);

 if(!gabi::call_ptr<u32>(target,s,0x10491B34,gabi::load<u32>(s+0x34),archive,a,b))return 0;

 gabi::call<void>(0x0264E240,s);

 u32 layout=gabi::load<u32>(s+0x44),namevt=gabi::load<u32>(0x104919B4),base=gabi::load<u32>(layout+4),owner=gabi::load<u32>(base+12),table=gabi::load<u32>(owner+8),nameTarget=gabi::load<u32>(namevt+0x14);

 gabi::call_ptr<void>(nameTarget,0x104919B0);

 u32 pane=gabi::call_ptr<u32>(gabi::load<u32>(table+0x5C),owner,gabi::load<u32>(0x104919B0),1);

 gabi::store<u32>(s+0x54,pane);

 pane=wind_pane(owner,0x104919B8);
gabi::store<u32>(s+0x58,pane);

 pane=wind_pane(owner,0x104919C0);
gabi::store<u32>(s+0x5C,pane);

 f32 x=gabi::load<f32>(pane+0x3C)*0.5f,y=gabi::load<f32>(pane+0x40)*0.5f;

 gabi::store<f32>(s+0x84,gabi::fmadds(x,x,y*y));

 pane=wind_pane(owner,0x104919C8);
x=gabi::load<f32>(pane+0x3C)*0.5f;
y=gabi::load<f32>(pane+0x40)*0.5f;

 gabi::store<f32>(s+0x88,gabi::fmadds(x,x,y*y)*0.5f);

 pane=wind_pane(owner,0x104919A0);
gabi::store<u8>(pane+0x44,gabi::load<u8>(pane+0x44)&0xFE);

 u32 state=gabi::load<u32>(s+0x18);
vt=gabi::load<u32>(state);

 state=gabi::call_ptr<u32>(gabi::load<u32>(vt+0x14),state,0x10491A44);
gabi::store<u32>(s+0x20,state);

 vt=gabi::load<u32>(state);
gabi::call_ptr<void>(gabi::load<u32>(vt+0x1C),state);
return 1;

}

VERIFY(0x0264E2C8,Wind_Setup);

void Wind_StaticInit() {

 WWHD_FUNC(0x0264E754,void);

 for(u32 off: {
12u,8u,4u,0u}
)gabi::store<u32>(0x104919EC+off,0);

 gabi::call<void>(0x028F026C,0x101F58F4);

 gabi::store<f32>(0x104919D0,-3.1415927410125732f);
gabi::store<f32>(0x104919D4,3.1415927410125732f);

 gabi::call<void>(0x028ED6F8,0x104919E8);
gabi::call<void>(0x028F026C,0x101F5900);
gabi::call<void>(0x028EAB2C,0x104919E9);
gabi::call<void>(0x028F026C,0x101F590C);

 gabi::store<f32>(0x104919D8,50000.0f);
gabi::store<f32>(0x104919E0,10000.0f);
gabi::store<f32>(0x104919DC,50000.0f);
gabi::store<f32>(0x104919E4,10000.0f);

 const u32 names[]={
0x104919FC,0x10491A14,0x10491A1C,0x10491A24,0x10491A2C,0x104919A0,0x10491A34,0x10491A3C,0x104919B8,0x104919B0,0x104919A8,0x10491B34,0x104919C0,0x104919C8,0x10491A04,0x10491A0C}
;

 const u32 strings[]={
0x100EECB0,0x100EEC2C,0x100EEC70,0x100EECD0,0x100EEC34,0x100EEC7C,0x100EECE0,0x100EEC88,0x100EED14,0x100EED04,0x100EECF0,0x100EECE0,0x100EEC3C,0x100EED28,0x100EECBC,0x100EEC5C}
;

 for(u32 i=0;i<16;++i){
gabi::store<u32>(names[i],strings[i]);
gabi::store<u32>(names[i]+4,0x100EEB24);
}

 bool initialized=gabi::load<u32>(0x101FD8E4)!=0;
u32 id;

 if(initialized)id=gabi::load<u32>(0x101FDD50);
else{
gabi::store<u32>(0x101FD8E4,1);
id=0;
}

 const u32 objects[]={
0x10491A44,0x10491A74,0x10491AA4,0x10491AD4,0x10491B04}
;

 const u32 stateNames[]={
0x100EED3C,0x100EEC4C,0x100EED4C,0x100EEC9C,0x100EED5C}
;

 for(u32 i=0;i<5;++i){
u32 at=objects[i];
gabi::store<u32>(at,++id);
gabi::store<u32>(at+4,stateNames[i]);
gabi::store<u32>(at+8,0x100EEB94);
for(u32 k=0;k<3;++k){
gabi::store<u16>(at+12+8*k,0);
gabi::store<u16>(at+14+8*k,u16(25+i*3+k));
gabi::store<u32>(at+16+8*k,4);
}
gabi::store<u16>(at+36,0);
gabi::store<u16>(at+38,0);
gabi::store<u32>(at+40,0);
gabi::store<u32>(at+44,0x101FF32C);
}

 gabi::store<u32>(0x101FDD50,id);

}

VERIFY(0x0264E754,Wind_StaticInit);

