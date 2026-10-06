#include "gabi.h"
using namespace gabi;
// Retained metronome controllers, construction, pane setup and generated inline companions.
// Scope 0261D8D0..026200A8 exclusive: mapped melodyShow plus constructor/vtable
// 100E37B8 and state/name descriptors initialized by 0261F7AC. The preceding
// 0261D5BC initializer and following pane factories belong to neighboring units.
static u32 animation(u32 p) {return load<u32>(load<u32>(p+0x44)+0xD4);}
void metroCloseFlag(u32 p) {WWHD_FUNC(0x0261D93C,void,p);store<u8>(p+0x16B,0);}
VERIFY(0x0261D93C,metroCloseFlag);
void metroGlobalTransition(u32 p) {WWHD_FUNC(0x0261D948,void,p);if(load<u8>(0x1047B097))call<void>(0x020063C0,p+0x18,0x1048E464u);}
VERIFY(0x0261D948,metroGlobalTransition);
void metroPlay(u32 p,u32 i) {WWHD_FUNC(0x0261D968,void,p,i);const f32 f=load<f32>(0x100E3440);const u32 a=animation(p),v=load<u32>(0x100E338C+4*i);call<void>(0x020053E4,a,i,v,f);}
VERIFY(0x0261D968,metroPlay);
void metroFrame(u32 p,u32 i,u32 frame) {WWHD_FUNC(0x0261D98C,void,p,i,frame);const u32 a=animation(p),v=load<u32>(0x100E338C+4*i);call<void>(0x0200552C,a,i,v,f32(frame));}
VERIFY(0x0261D98C,metroFrame);
void metroBeatPlay(u32 p) {WWHD_FUNC(0x0261D9E4,void,p);u32 b=load<u8>(p+0x164);if(b==3||b==4||b==6){call<void>(0x0261D968,p,b==3?1u:b==4?2u:3u);b=load<u8>(p+0x164);}store<u8>(p+0x166,b);}
VERIFY(0x0261D9E4,metroBeatPlay);
void metroHide(u32 p) {WWHD_FUNC(0x0261DA70,void,p);const u32 b=load<u8>(p+0x164);const u32 off=b==3?0xA4:b==4?0xB0:0xC0;if(b!=3&&b!=4&&b!=6)return;for(u32 i=0;i<b;i++)store<u8>(load<u32>(p+off+4*i)+0x64,0);}
VERIFY(0x0261DA70,metroHide);
void metroOpenInit(u32 p) {WWHD_FUNC(0x0261DB68,void,p);u32 b=call<u32>(0x025E1EFC);store<u8>(p+0x164,b);store<u8>(p+0x165,b);store<u32>(p+0x154,0);store<u32>(p+0x124,0);store<u8>(p+0x16A,0);call<void>(0x0261D968,p,0u);for(u32 i=1;i<=3;i++)call<void>(0x0261D98C,p,i,0u);call<void>(0x0261D9E4,p);call<void>(0x0261DA70,p);for(u32 i=0;i<21;i++)call<void>(0x02702654,load<u32>(p+0x50+4*i));store<u8>(p+0x168,0);store<u8>(p+0x167,1);}
VERIFY(0x0261DB68,metroOpenInit);
void metroShrinkPlay(u32 p) {WWHD_FUNC(0x0261E5A8,void,p);u32 b=load<u8>(p+0x165);if(b==3||b==4||b==6){call<void>(0x0261D968,p,b==3?4u:b==4?5u:6u);b=load<u8>(p+0x165);}store<u8>(p+0x166,b);}
VERIFY(0x0261E5A8,metroShrinkPlay);
void metroShrinkWrapper(u32 p) {WWHD_FUNC(0x0261E634,void,p);call<void>(0x0261E5A8,p);}
VERIFY(0x0261E634,metroShrinkWrapper);
u32 metroShrinkDone(u32 p) {WWHD_FUNC(0x0261E638,u32,p);const u32 b=load<u8>(p+0x166);if(b!=3&&b!=4&&b!=6)return 1;return call<u32>(0x02005840,animation(p),b==3?4u:b==4?5u:6u);}
VERIFY(0x0261E638,metroShrinkDone);
void metroTimerInit(u32 p) {WWHD_FUNC(0x0261E760,void,p);store<u32>(p+0x158,10);}
VERIFY(0x0261E760,metroTimerInit);
void metroDemoInit(u32 p) {WWHD_FUNC(0x0261E808,void,p);const u32 q=call<u32>(0x0261DA70,p);store<u32>(q+0x15C,0);store<u32>(q+0x160,0);}
VERIFY(0x0261E808,metroDemoInit);
void metroEndPlay(u32 p) {WWHD_FUNC(0x0261EA3C,void,p);call<void>(0x0261D968,p,7u);}
VERIFY(0x0261EA3C,metroEndPlay);
void metroEndFlag(u32 p) {WWHD_FUNC(0x0261EABC,void,p);store<u8>(p+0x167,0);}
VERIFY(0x0261EABC,metroEndFlag);

s32 metroTimer(u32 p) {WWHD_FUNC(0x0261DCD0,s32,p);u32 env=call<u32>(0x025200D4);f32 rate=call<f32>(0x024430C0,load<u32>(env+0x5B34));s32 t=ftoi(fmuls_ppc(load<f32>(0x100E3450),rate));const u32 side=load<u8>(p+0x169);if(t>20)t=20;else if(t<0)t=0;if(side){if(load<f32>(p+0x148)>rate)store<u8>(p+0x169,0);return t<=10?10-t:t-10;}env=call<u32>(0x025200D4);rate=call<f32>(0x024430C0,load<u32>(env+0x5B34));if(load<f32>(p+0x148)>rate)store<u8>(p+0x169,1);return t<=10?t+10:30-t;}
VERIFY(0x0261DCD0,metroTimer);
void metroTimerReset(u32 p) {WWHD_FUNC(0x0261DDBC,void,p);const u32 env=call<u32>(0x025200D4);const f32 rate=call<f32>(0x024430C0,load<u32>(env+0x5B34));store<f32>(p+0x148,rate);store<u8>(p+0x169,0);const u32 t=call<u32>(0x0261DCD0,p);store<u32>(p+0x150,0);store<u8>(p+0x16B,1);store<u32>(p+0x14C,t);}
VERIFY(0x0261DDBC,metroTimerReset);
void metroTimerResetWrapper(u32 p) {WWHD_FUNC(0x0261DE1C,void,p);call<void>(0x0261DDBC,p);}
VERIFY(0x0261DE1C,metroTimerResetWrapper);
void metroTimerMove(u32 p) {WWHD_FUNC(0x0261DE20,void,p);const u32 t=call<u32>(0x0261DCD0,p);if(t!=load<u32>(p+0x14C)){u32 q=p+0x50;if(t<21)q+=4*t;store<u8>(load<u32>(q)+0x50,1);}const u32 env=call<u32>(0x025200D4);const f32 rate=call<f32>(0x024430C0,load<u32>(env+0x5B34));store<f32>(p+0x148,rate);store<u32>(p+0x14C,t);}
VERIFY(0x0261DE20,metroTimerMove);
static void metroVcall(u32 p,u32 offset) {call_ptr<void>(load<u32>(load<u32>(p+4)+offset),p);}
void metroEnterMove(u32 p) {WWHD_FUNC(0x0261DC58,void,p);if(call<u32>(0x02005840,animation(p),0u))call<void>(0x020063C0,p+0x18,0x1048E494u);metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261DC58,metroEnterMove);
void metroEndMove(u32 p) {WWHD_FUNC(0x0261EA44,void,p);if(call<u32>(0x02005840,animation(p),7u))call<void>(0x020063C0,p+0x18,0x1048E434u);metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261EA44,metroEndMove);
void metroDelete(u32 p) {WWHD_FUNC(0x0261F290,void,p);metroVcall(p,0xAC);metroVcall(p,0x84);call<void>(0x026F91F8,p);const u32 e=load<u32>(p+0x124);if(e){const u32 flags=load<u32>(e+0x254);store<u32>(e+0x5C,0xFFFFFFFF);store<u32>(e+0x254,flags|1);const u32 q=load<u32>(p+0x124);store<u32>(q+0x254,load<u32>(q+0x254)&~0x40u);}}
VERIFY(0x0261F290,metroDelete);
void metroMove(u32 p) {WWHD_FUNC(0x0261F314,void,p);if(!call<u32>(0x025AF2A4,p))call<void>(0x02006364,p+0x18);}
VERIFY(0x0261F314,metroMove);

void metroDraw(u32 p,u32 port) {WWHD_FUNC(0x0261F350,void,p,port);if(load<u8>(p+0x167)){call_ptr<void>(load<u32>(load<u32>(p+4)+0x9C),p,port);call_ptr<void>(load<u32>(load<u32>(p+4)+0xC4),p,port);}}
VERIFY(0x0261F350,metroDraw);
void metroIdleMove(u32 p) {WWHD_FUNC(0x0261E4A0,void,p);if(!load<u8>(0x1047B097))call<void>(0x020063C0,p+0x18,0x1048E554u);else{bool changed=false;if(!load<u8>(p+0x168)){const u32 current=call<u32>(0x025E1EFC);if(load<u8>(p+0x164)!=current){store<u8>(p+0x165,load<u8>(p+0x164));store<u8>(p+0x164,call<u32>(0x025E1EFC));store<u32>(p+0x154,0);call<void>(0x0261DA70,p);changed=true;}}call<void>(0x0261DE20,p);call<void>(0x0261E1A0,p);if(changed)call<void>(0x020063C0,p+0x18,0x1048E4C4u);else if(load<u8>(p+0x168))call<void>(0x020063C0,p+0x18,0x1048E4F4u);}metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261E4A0,metroIdleMove);
void metroShrinkMove(u32 p) {WWHD_FUNC(0x0261E68C,void,p);if(!load<u8>(0x1047B097))call<void>(0x020063C0,p+0x18,0x1048E554u);else{call<void>(0x0261DE20,p);call<void>(0x0261E1A0,p);if(load<u8>(p+0x168))call<void>(0x020063C0,p+0x18,0x1048E4F4u);else if(call<u32>(0x0261E638,p)){store<u8>(p+0x16A,0);call<void>(0x0261D9E4,p);call<void>(0x020063C0,p+0x18,0x1048E494u);}}metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261E68C,metroShrinkMove);
void metroTimerWait(u32 p) {WWHD_FUNC(0x0261E76C,void,p);if(load<u8>(0x1047B097)){call<void>(0x0261DE20,p);u32 t=load<u32>(p+0x158)-1;store<u32>(p+0x158,t);if(!t)call<void>(0x020063C0,p+0x18,0x1048E524u);}else call<void>(0x020063C0,p+0x18,0x1048E554u);metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261E76C,metroTimerWait);
void metroBeatEffects(u32 p) {WWHD_FUNC(0x0261E05C,void,p);const u32 b=load<u8>(p+0x164);if(b!=3&&b!=4&&b!=6)return;const u32 off=b==3?0xA4:b==4?0xB0:0xC0;for(u32 i=0;i<b;i++)call<void>(0x02620B68,load<u32>(p+off+4*i));}
VERIFY(0x0261E05C,metroBeatEffects);
struct MetroVec {f32 x,y,z;};
void metroGuide(u32 p,u32 note,u32 index) {WWHD_FUNC(0x0261DE98,void,p,note,index);const u32 b=load<u8>(p+0x164);if(b!=3&&b!=4&&b!=6)return;const u32 off=b==3?0xA4:b==4?0xB0:0xC0;const u32 pos=index<b?4*index:0;const u32 pane=load<u32>(p+off+pos);store<u32>(pane+0x5C,note);store<u8>(pane+0x64,1);const u32 target=load<u32>(p+(b==3?0xD8:b==4?0xE4:0xF4)+pos);if(!target)return;Local<MetroVec> coords,result;call<void>(0x02704D2C,coords.a,target);store<f32>(result.a,fadds_ppc(load<f32>(coords.a),load<f32>(p+0x128)));store<f32>(result.a+4,fadds_ppc(load<f32>(coords.a+4),load<f32>(p+0x12C)));store<f32>(result.a+8,load<f32>(coords.a+8));const u32 env=call<u32>(0x025200D4);const u32 e=call<u32>(0x025A847C,load<u32>(env+0x5AB0),7u,0x23Eu,result.a,0u,0u,255u,0u,0xFFFFFFFFu,0u,0u,0u);const f32 scale=load<f32>(p+0x138);store<f32>(e+0x238,scale);store<f32>(e+0x23C,scale);store<f32>(e+0x240,scale);const f32 wide=load<f32>(p+0x13C);store<f32>(e+0x220,wide);store<f32>(e+0x228,wide);store<f32>(e+0x224,wide);}
VERIFY(0x0261DE98,metroGuide);

static void metroProject(u32 p,u32 pos,u32 out) {call<void>(0x025F0EA4,pos,out);store<f32>(out,fadds_ppc(load<f32>(out),load<f32>(p+0x130)));store<f32>(out+4,fadds_ppc(load<f32>(out+4),load<f32>(p+0x134)));}
static void metroParticleSize(u32 p,u32 e) {f32 x=load<f32>(p+0x140);store<f32>(e+0x238,x);store<f32>(e+0x23C,x);store<f32>(e+0x240,x);x=load<f32>(p+0x144);e=load<u32>(p+0x124);store<f32>(e+0x220,x);store<f32>(e+0x228,x);store<f32>(e+0x224,x);}
void metroMelodyShow(u32 p) {WWHD_FUNC(0x0261E1A0,void,p);Local<MetroVec> tact,projected;u32 env=call<u32>(0x025200D4);const bool input=load<u32>(load<u32>(env+0x5B2C)+0x3C0)&0x01000000;
 if(input){if(!load<u8>(p+0x16A)){u32 index=load<u32>(p+0x154);if(index>=load<u8>(p+0x164)){store<u32>(p+0x154,0);call<void>(0x0261DA70,p);index=load<u32>(p+0x154);}if(!index)store<u8>(load<u32>(p+0x78)+0x50,1);store<u8>(p+0x16A,1);env=call<u32>(0x025200D4);const u32 note=u32(s32(load<s16>(load<u32>(env+0x5B34)+0x691C)));index=load<u32>(p+0x154);store<u32>(p+0x10C+(index<6?4*index:0),note);call<void>(0x0261DE98,p,note,u32(load<u8>(p+0x157)));store<u32>(p+0x154,load<u32>(p+0x154)+1);
 env=call<u32>(0x025200D4);if(call<u32>(0x02443144,load<u32>(env+0x5B34),tact.a)){metroProject(p,tact.a,projected.a);u32 e=load<u32>(p+0x124);if(e){store<u32>(e+0x254,load<u32>(e+0x254)&~0x40u);store<u32>(p+0x124,0);}env=call<u32>(0x025200D4);e=call<u32>(0x025A847C,load<u32>(env+0x5AB0),7u,0x23Fu,projected.a,0u,0u,255u,0u,0xFFFFFFFFu,0u,0u,0u);store<u32>(p+0x124,e);if(e){store<u32>(e+0x254,load<u32>(e+0x254)|0x40u);metroParticleSize(p,load<u32>(p+0x124));}}
 env=call<u32>(0x025200D4);if(call<u32>(0x0244311C,load<u32>(env+0x5B34))){store<u8>(p+0x168,1);call<void>(0x0261E05C,p);call<void>(0x0262057C,load<u32>(p+0x78));}}
 store<u32>(p+0x150,0);
 }else{if(load<u32>(p+0x154)>=load<u8>(p+0x164)){u32 t=load<u32>(p+0x150)+1;store<u32>(p+0x150,t);if(t>30){store<u32>(p+0x154,0);call<void>(0x0261DA70,p);}}store<u8>(p+0x16A,0);}
 u32 e=load<u32>(p+0x124);if(!e)return;const u32 flags=load<u32>(e+0x254);if((flags&8)&&load<u32>(e+0x1B4)+load<u32>(e+0x1C0)==0){store<u32>(e+0x254,load<u32>(e+0x254)&~0x40u);store<u32>(p+0x124,0);return;}
 env=call<u32>(0x025200D4);if(call<u32>(0x02443144,load<u32>(env+0x5B34),tact.a)){metroProject(p,tact.a,projected.a);e=load<u32>(p+0x124);f32 y=load<f32>(projected.a+4);if(load<u8>(e+0x262)>=7)y=-y;store<f32>(e+0x230,y);store<f32>(e+0x22C,load<f32>(projected.a));store<f32>(e+0x234,load<f32>(projected.a+8));metroParticleSize(p,load<u32>(p+0x124));}
}
VERIFY(0x0261E1A0,metroMelodyShow);

void metroDemoMove(u32 p) {WWHD_FUNC(0x0261E834,void,p);const u32 ticks=load<u32>(p+0x15C);const u32 env=call<u32>(0x025200D4);u32 player=load<u32>(env+0x5B34);if(!ticks){if(call<u32>(0x024431E0,player))store<u32>(p+0x15C,1);return;}const u32 melody=call_ptr<u32>(load<u32>(load<u32>(player+0xB4)+0x2C),player);Local<s16[7]> frames;Local<s32[6]> notes;store<s16>(frames.a,1);u32 count=load<u8>(p+0x164);for(u32 i=0;i<count;i++){const f32 f=call<f32>(0x025E1F68,melody,i,notes.a+4*i);store<s16>(frames.a+2*(i+1),s16(ftoi(fadds_ppc(f,f32(load<s16>(frames.a+2*i))))));count=load<u8>(p+0x164);}const u32 index=load<u32>(p+0x160);if(count>index){u32 t=load<u32>(p+0x15C);if(u32(s32(load<s16>(frames.a+2*index)))==t){const u32 note=load<u32>(p+0x10C+(index<6?4*index:0));call<void>(0x0261DE98,p,note,index&255u);store<u32>(p+0x160,load<u32>(p+0x160)+1);t=load<u32>(p+0x15C);}store<u32>(p+0x15C,t+1);}}
VERIFY(0x0261E834,metroDemoMove);

void metroDemoState(u32 p) {WWHD_FUNC(0x0261E990,void,p);if(load<u8>(0x1047B097)){call<void>(0x0261DE20,p);call<void>(0x0261E834,p);}else call<void>(0x020063C0,p+0x18,0x1048E554u);metroVcall(p,0xB4);metroVcall(p,0x8C);}
VERIFY(0x0261E990,metroDemoState);
u32 metroCtor(u32 p) {WWHD_FUNC(0x0261EAC8,u32,p);if(!p)p=call<u32>(0x0273AD10,0x16Cu);if(!p)return 0;call<void>(0x026F89E8,p);store<u32>(p+4,0x100E37B8);store<u32>(p+0x30,0x100E3938);store<u32>(p+0x2C,0x100E3928);
 const u32 offsets[]={0x50,0xA4,0xB0,0xC0,0xD8,0xE4,0xF4,0x10C};const u32 sizes[]={84,12,16,24,12,16,24,24};for(u32 i=0;i<8;i++)if(!(p+offsets[i]))call<void>(0x0273AD10,sizes[i]);const u32 zero=load<u32>(0x100E3460);store<u32>(p+0x124,0);u32 q=p+0x128;if(!q)q=call<u32>(0x0273AD10,8u);if(q){store<u32>(q+4,zero);store<u32>(q,zero);}q=p+0x130;if(!q)q=call<u32>(0x0273AD10,8u);if(q){store<u32>(q+4,zero);store<u32>(q,zero);}
 store<u8>(p+0x164,0);store<u8>(p+0x166,0);store<u8>(p+0x169,0);const u32 one=load<u32>(0x100E3440);store<u32>(p+0x144,one);store<u32>(p+0x140,one);store<u32>(p+0x138,load<u32>(0x100E3464));store<u8>(p+0x168,0);store<u32>(p+0x154,0);store<u8>(p+0x16B,0);store<u32>(p+0x15C,0);store<u32>(p+0x13C,load<u32>(0x100E3468));store<u8>(p+0x167,0);store<u32>(p+0x150,0);store<u32>(p+0x158,0);store<u32>(p+0x14C,0);store<u32>(p+0x160,0);store<u8>(p+0x16A,0);store<u32>(p+0x148,zero);for(u32 i=0;i<21;i++)store<u32>(p+0x50+4*i,0);for(u32 i=0;i<3;i++){store<u32>(p+0xA4+4*i,0);store<u32>(p+0xD8+4*i,0);}for(u32 i=0;i<4;i++){store<u32>(p+0xB0+4*i,0);store<u32>(p+0xE4+4*i,0);}for(u32 i=0;i<6;i++){store<u32>(p+0xC0+4*i,0);store<u32>(p+0xF4+4*i,0);store<u32>(p+0x10C+4*i,0);}return p;}
VERIFY(0x0261EAC8,metroCtor);
void metroDtor(u32 p,u32 flags) {WWHD_FUNC(0x0261D8D0,void,p,flags);if(p){store<u32>(p+0x2C,0x100E3928);store<u32>(p+0x30,0x100E3938);call<void>(0x026F88F0,p,0u);if(flags&1)call<void>(0x0273AF40,p);}}
VERIFY(0x0261D8D0,metroDtor);

u32 metroLoadScreen(u32 p,u32 file,u32 archive,u32 arg6,u32 arg7,u32 arg8) {WWHD_FUNC(0x0261F680,u32,p,file,archive,arg6,arg7,arg8);u32 screen=call<u32>(0x0273B050,0x110u,load<u32>(load<u32>(0x1018C404)+0x10),4u);if(screen)screen=call<u32>(0x026FADB0,screen);store<u32>(p+0x44,screen);if(!screen)return 0;const u32 target=load<u32>(load<u32>(screen+0xE0)+0x14);if(!call_ptr<u32>(target,screen,file,archive,arg6,arg7,8u,4u,arg8))return 0;for(u32 i=0;i<8;i++){const u32 a=animation(p),index=load<u32>(0x100E338C+4*i);call<void>(0x02004E04,a,i,0x1048E1B4+8*i,0x1048E194+8*index);}metroVcall(p,0x8C);return 1;}
VERIFY(0x0261F680,metroLoadScreen);
static void metroStringTouch(u32 p) {call_ptr<void>(load<u32>(load<u32>(p+4)+0x14),p);}
static bool metroStringEqual(u32 a,u32 b) {if(a==b)return true;for(u32 i=0;i<0x40001;i++){const u8 x=load<u8>(a+i),y=load<u8>(b+i);if(x!=y)return false;if(!x)return true;}return false;}
u32 metroCreatePane(u32 p,u32 strings) {WWHD_FUNC(0x0261F3C0,u32,p,strings);Local<u32[2]> str;u32 creators[3]={0x026200A8,0x02620158,0x02620208};u32 names[3];const u32 holders[3]={0x1048E5B0,0x1048E5E4,0x1048E6F0};for(u32 i=0;i<3;i++){if(i==0)call_ptr<void>(load<u32>(load<u32>(holders[i]+4)+0x14),holders[i],strings,0x02620208u);else metroStringTouch(holders[i]);names[i]=load<u32>(holders[i]);}const u32 first=strings+0x14,second=strings+8;for(u32 i=0;i<34;i++){const u32 vt=load<u32>(first+4),fn=load<u32>(vt+0x14);call_ptr<void>(fn,first,vt,fn);metroStringTouch(first);const u32 table=0x1048E214+8*i;const u32 name=load<u32>(first);metroStringTouch(table);const u32 other=load<u32>(table);if(name!=other&&!metroStringEqual(load<u32>(first),load<u32>(table)))continue;for(u32 j=0;j<3;j++){store<u32>(str.a,names[j]);store<u32>(str.a+4,0x100E3374);metroStringTouch(second);metroStringTouch(second);const u32 n=load<u32>(second);metroStringTouch(str.a);const u32 other2=load<u32>(str.a);if(n!=other2&&!metroStringEqual(load<u32>(second),load<u32>(str.a)))continue;const u32 count=load<u32>(p+0x48);u32 slot=load<u32>(p+0x4C);if(i<count)slot+=4*i;const u32 result=call_ptr<u32>(creators[j],0u,0u,strings);store<u32>(slot,result);slot=load<u32>(p+0x4C);if(i<load<u32>(p+0x48))slot+=4*i;return load<u32>(slot)?1u:0u;}return call<u32>(0x026F92C0,p,i,strings);}return 1;}
VERIFY(0x0261F3C0,metroCreatePane);

u32 metroScreenSet(u32 p,u32 archive,u32 a,u32 b) {WWHD_FUNC(0x0261EDFC,u32,p,archive,a,b);call<void>(0x026F90D8,p,0x1048E1F4u,4u);u32 target=load<u32>(load<u32>(p+4)+0x7C);if(!call_ptr<u32>(target,p,0x1048E584u,load<u32>(p+0x34),archive,a,b))return 0;if(!call_ptr<u32>(load<u32>(load<u32>(p+4)+0xA4),p))return 0;call<void>(0x0200552C,animation(p),0u,0u,load<f32>(0x100E3460));const u32 screen=load<u32>(load<u32>(load<u32>(p+0x44)+4)+0xC);
 for(u32 i=0;i<21;i++){const u32 count=load<u32>(p+0x48),array=load<u32>(p+0x4C);store<u32>(p+0x50+4*i,load<u32>(array+(i<count?4*i:0)));}
 const u32 off[3]={0xA4,0xB0,0xC0},out[3]={0xD8,0xE4,0xF4},n[3]={3,4,6},base[3]={21,24,28};for(u32 j=0;j<3;j++)for(u32 i=0;i<n[j];i++){const u32 index=base[j]+i,count=load<u32>(p+0x48),array=load<u32>(p+0x4C);store<u32>(p+off[j]+4*i,load<u32>(array+(index<count?4*index:0)));const u32 str=0x1048E214+8*index;const u32 vtable=load<u32>(screen+8);metroStringTouch(str);const u32 pane=call_ptr<u32>(load<u32>(vtable+0x5C),screen,load<u32>(str),1u);store<u32>(p+out[j]+4*i,pane);}
 const u32 state=load<u32>(p+0x18);const u32 action=call_ptr<u32>(load<u32>(load<u32>(state)+0x14),state,0x1048E434u);store<u32>(p+0x20,action);call_ptr<void>(load<u32>(load<u32>(action)+0x1C),action);return 1;}
VERIFY(0x0261EDFC,metroScreenSet);


// Static animation names, pane names and state registration descriptors, in native write order.
void metroStaticInit() {WWHD_FUNC(0x0261F7AC,void);
store<u32>(0x1048E190u,0x00000000u);
store<u32>(0x1048E18Cu,0x00000000u);
store<u32>(0x1048E188u,0x00000000u);
store<u32>(0x1048E184u,0x00000000u);
call<void>(0x028F026C,0x101F5194u);
store<u32>(0x1048E178u,load<u32>(0x100E346Cu));
store<u32>(0x1048E17Cu,load<u32>(0x100E3470u));
call<void>(0x028ED6F8,0x1048E180u);
call<void>(0x028F026C,0x101F51A0u);
call<void>(0x028EAB2C,0x1048E181u);
call<void>(0x028F026C,0x101F51ACu);
store<u32>(0x1048E198u,0x100E3374u);
store<u32>(0x1048E194u,0x100E370Cu);
store<u32>(0x1048E1A0u,0x100E3374u);
store<u32>(0x1048E19Cu,0x100E348Cu);
store<u32>(0x1048E1A8u,0x100E3374u);
store<u32>(0x1048E1A4u,0x100E349Cu);
store<u32>(0x1048E1B0u,0x100E3374u);
store<u32>(0x1048E1ACu,0x100E34ACu);
store<u32>(0x1048E1B8u,0x100E3374u);
store<u32>(0x1048E1B4u,0x100E347Cu);
store<u32>(0x1048E1C0u,0x100E3374u);
store<u32>(0x1048E1BCu,0x100E3474u);
store<u32>(0x1048E1C8u,0x100E3374u);
store<u32>(0x1048E1C4u,0x100E3474u);
store<u32>(0x1048E1D0u,0x100E3374u);
store<u32>(0x1048E1CCu,0x100E3474u);
store<u32>(0x1048E1D8u,0x100E3374u);
store<u32>(0x1048E1D4u,0x100E3718u);
store<u32>(0x1048E1E0u,0x100E3374u);
store<u32>(0x1048E1DCu,0x100E3718u);
store<u32>(0x1048E1E8u,0x100E3374u);
store<u32>(0x1048E218u,0x100E3374u);
store<u32>(0x1048E214u,0x100E34BCu);
store<u32>(0x1048E1E4u,0x100E3718u);
store<u32>(0x1048E220u,0x100E3374u);
store<u32>(0x1048E21Cu,0x100E34CCu);
store<u32>(0x1048E228u,0x100E3374u);
store<u32>(0x1048E1F0u,0x100E3374u);
store<u32>(0x1048E224u,0x100E34DCu);
store<u32>(0x1048E230u,0x100E3374u);
store<u32>(0x1048E22Cu,0x100E34ECu);
store<u32>(0x1048E238u,0x100E3374u);
store<u32>(0x1048E1ECu,0x100E3484u);
store<u32>(0x1048E234u,0x100E34FCu);
store<u32>(0x1048E240u,0x100E3374u);
store<u32>(0x1048E23Cu,0x100E350Cu);
store<u32>(0x1048E248u,0x100E3374u);
store<u32>(0x1048E244u,0x100E351Cu);
store<u32>(0x1048E250u,0x100E3374u);
store<u32>(0x1048E24Cu,0x100E352Cu);
store<u32>(0x1048E258u,0x100E3374u);
store<u32>(0x1048E254u,0x100E353Cu);
store<u32>(0x1048E260u,0x100E3374u);
store<u32>(0x1048E25Cu,0x100E354Cu);
store<u32>(0x1048E268u,0x100E3374u);
store<u32>(0x1048E264u,0x100E355Cu);
store<u32>(0x1048E270u,0x100E3374u);
store<u32>(0x1048E26Cu,0x100E356Cu);
store<u32>(0x1048E278u,0x100E3374u);
store<u32>(0x1048E274u,0x100E357Cu);
store<u32>(0x1048E280u,0x100E3374u);
store<u32>(0x1048E27Cu,0x100E358Cu);
store<u32>(0x1048E288u,0x100E3374u);
store<u32>(0x1048E284u,0x100E359Cu);
store<u32>(0x1048E290u,0x100E3374u);
store<u32>(0x1048E28Cu,0x100E35ACu);
store<u32>(0x1048E298u,0x100E3374u);
store<u32>(0x1048E294u,0x100E35BCu);
store<u32>(0x1048E2A0u,0x100E3374u);
store<u32>(0x1048E29Cu,0x100E35CCu);
store<u32>(0x1048E2A8u,0x100E3374u);
store<u32>(0x1048E2A4u,0x100E35DCu);
store<u32>(0x1048E2B0u,0x100E3374u);
store<u32>(0x1048E2ACu,0x100E35ECu);
store<u32>(0x1048E2B8u,0x100E3374u);
store<u32>(0x1048E2B4u,0x100E35FCu);
store<u32>(0x1048E2C0u,0x100E3374u);
store<u32>(0x1048E2BCu,0x100E360Cu);
store<u32>(0x1048E2C8u,0x100E3374u);
store<u32>(0x1048E2C4u,0x100E361Cu);
store<u32>(0x1048E2D0u,0x100E3374u);
store<u32>(0x1048E2CCu,0x100E362Cu);
store<u32>(0x1048E2D8u,0x100E3374u);
store<u32>(0x1048E2D4u,0x100E363Cu);
store<u32>(0x1048E2E0u,0x100E3374u);
store<u32>(0x1048E2DCu,0x100E364Cu);
store<u32>(0x1048E2E8u,0x100E3374u);
store<u32>(0x1048E2E4u,0x100E365Cu);
store<u32>(0x1048E2F0u,0x100E3374u);
store<u32>(0x1048E2ECu,0x100E366Cu);
store<u32>(0x1048E2F8u,0x100E3374u);
store<u32>(0x1048E2F4u,0x100E367Cu);
store<u32>(0x1048E32Cu,0x100E3724u);
store<u32>(0x1048E348u,0x100E3374u);
store<u32>(0x1048E350u,0x100E3374u);
store<u32>(0x1048E330u,0x100E3374u);
store<u32>(0x1048E34Cu,0x100E3724u);
store<u32>(0x1048E340u,0x100E3374u);
store<u32>(0x1048E334u,0x100E3724u);
store<u32>(0x1048E324u,0x100E3724u);
store<u32>(0x1048E328u,0x100E3374u);
store<u32>(0x1048E380u,0x100E3374u);
store<u32>(0x1048E36Cu,0x100E3724u);
store<u32>(0x1048E370u,0x100E3374u);
store<u32>(0x1048E344u,0x100E3724u);
store<u32>(0x1048E364u,0x100E3724u);
store<u32>(0x1048E33Cu,0x100E3724u);
store<u32>(0x1048E368u,0x100E3374u);
store<u32>(0x1048E390u,0x100E3374u);
store<u32>(0x1048E388u,0x100E3374u);
store<u32>(0x1048E358u,0x100E3374u);
store<u32>(0x1048E354u,0x100E3724u);
store<u32>(0x1048E3B4u,0x100E3724u);
store<u32>(0x1048E3B8u,0x100E3374u);
store<u32>(0x1048E3C0u,0x100E3374u);
store<u32>(0x1048E3ACu,0x100E3724u);
store<u32>(0x1048E37Cu,0x100E3724u);
store<u32>(0x1048E378u,0x100E3374u);
store<u32>(0x1048E3BCu,0x100E3724u);
store<u32>(0x1048E3B0u,0x100E3374u);
store<u32>(0x1048E300u,0x100E3374u);
store<u32>(0x1048E2FCu,0x100E368Cu);
store<u32>(0x1048E35Cu,0x100E3724u);
store<u32>(0x1048E38Cu,0x100E3724u);
store<u32>(0x1048E308u,0x100E3374u);
store<u32>(0x1048E304u,0x100E369Cu);
store<u32>(0x1048E3C8u,0x100E3374u);
store<u32>(0x1048E310u,0x100E3374u);
store<u32>(0x1048E30Cu,0x100E36ACu);
store<u32>(0x1048E318u,0x100E3374u);
store<u32>(0x1048E314u,0x100E36BCu);
store<u32>(0x1048E420u,0x100E3374u);
store<u32>(0x1048E320u,0x100E3374u);
store<u32>(0x1048E31Cu,0x100E36CCu);
store<u32>(0x1048E3A4u,0x100E3724u);
store<u32>(0x1048E384u,0x100E3724u);
store<u32>(0x1048E398u,0x100E3374u);
store<u32>(0x1048E3A8u,0x100E3374u);
store<u32>(0x1048E3C4u,0x100E3724u);
store<u32>(0x1048E394u,0x100E3724u);
store<u32>(0x1048E3D0u,0x100E3374u);
store<u32>(0x1048E39Cu,0x100E3724u);
store<u32>(0x1048E338u,0x100E3374u);
store<u32>(0x1048E374u,0x100E3738u);
store<u32>(0x1048E3A0u,0x100E3374u);
store<u32>(0x1048E1F8u,0x100E3374u);
store<u32>(0x1048E1F4u,0x100E36FCu);
store<u32>(0x1048E200u,0x100E3374u);
store<u32>(0x1048E3ECu,0x100E374Cu);
store<u32>(0x1048E410u,0x100E3374u);
store<u32>(0x1048E1FCu,0x100E3724u);
store<u32>(0x1048E40Cu,0x100E374Cu);
const u32 registration=load<u32>(0x101FD8E4u);
store<u32>(0x1048E3DCu,0x100E374Cu);
store<u32>(0x1048E3F0u,0x100E3374u);
store<u32>(0x1048E41Cu,0x100E374Cu);
store<u32>(0x1048E3E8u,0x100E3374u);
store<u32>(0x1048E3E0u,0x100E3374u);
store<u32>(0x1048E414u,0x100E374Cu);
store<u32>(0x1048E3F8u,0x100E3374u);
store<u32>(0x1048E428u,0x100E3374u);
store<u32>(0x1048E404u,0x100E374Cu);
store<u32>(0x1048E408u,0x100E3374u);
store<u32>(0x1048E3F4u,0x100E374Cu);
store<u32>(0x1048E3D8u,0x100E3374u);
store<u32>(0x1048E3CCu,0x100E374Cu);
store<u32>(0x1048E400u,0x100E3374u);
store<u32>(0x1048E3E4u,0x100E374Cu);
store<u32>(0x1048E424u,0x100E374Cu);
store<u32>(0x1048E3FCu,0x100E374Cu);
store<u32>(0x1048E3D4u,0x100E374Cu);
store<u32>(0x1048E208u,0x100E3374u);
store<u32>(0x1048E204u,0x100E374Cu);
store<u32>(0x1048E210u,0x100E3374u);
store<u32>(0x1048E20Cu,0x100E3738u);
store<u32>(0x1048E418u,0x100E3374u);
store<u32>(0x1048E42Cu,0x100E374Cu);
store<u32>(0x1048E360u,0x100E3374u);
store<u32>(0x1048E588u,0x100E3374u);
store<u32>(0x1048E430u,0x100E3374u);
store<u32>(0x1048E584u,0x100E36FCu);
u32 nextId;if(registration){nextId=load<u32>(0x101FDD50);}else{nextId=0;store<u32>(0x101FD8E4,1);}
store<u16>(0x1048E440u,0x00000000u);
store<u16>(0x1048E442u,0x00000019u);
store<u16>(0x1048E448u,0x00000000u);
store<u16>(0x1048E44Au,0x0000001Au);
++nextId;
store<u16>(0x1048E470u,0x00000000u);
store<u32>(0x1048E434u,nextId);
store<u32>(0x1048E438u,0x100E375Cu);
store<u16>(0x1048E450u,0x00000000u);
store<u16>(0x1048E452u,0x0000001Bu);
store<u16>(0x1048E472u,0x0000001Cu);
store<u16>(0x1048E478u,0x00000000u);
++nextId;
store<u32>(0x1048E444u,0x00000004u);
store<u32>(0x1048E464u,nextId);
store<u32>(0x1048E468u,0x100E36DCu);
store<u32>(0x1048E474u,0x00000004u);
store<u32>(0x1048E47Cu,0x00000004u);
store<u32>(0x1048E484u,0x00000004u);
store<u16>(0x1048E47Au,0x0000001Du);
store<u32>(0x1048E44Cu,0x00000004u);
store<u16>(0x1048E480u,0x00000000u);
store<u16>(0x1048E482u,0x0000001Eu);
store<u16>(0x1048E488u,0x00000000u);
store<u32>(0x1048E454u,0x00000004u);
store<u32>(0x1048E45Cu,0x00000000u);
store<u16>(0x1048E48Au,0x00000000u);
store<u16>(0x1048E458u,0x00000000u);
store<u32>(0x1048E43Cu,0x100E33F4u);
++nextId;
store<u32>(0x1048E48Cu,0x00000000u);
store<u32>(0x1048E46Cu,0x100E33F4u);
store<u32>(0x1048E494u,nextId);
store<u32>(0x1048E460u,0x101FF32Cu);
store<u32>(0x1048E490u,0x101FF32Cu);
store<u16>(0x1048E4A0u,0x00000000u);
store<u16>(0x1048E4A2u,0x0000001Fu);
store<u32>(0x1048E498u,0x100E376Cu);
store<u32>(0x1048E4A4u,0x00000004u);
store<u32>(0x1048E4ACu,0x00000004u);
store<u16>(0x1048E4D0u,0x00000000u);
store<u16>(0x1048E4D2u,0x00000022u);
store<u16>(0x1048E4D8u,0x00000000u);
store<u16>(0x1048E45Au,0x00000000u);
store<u32>(0x1048E4B4u,0x00000004u);
store<u16>(0x1048E4A8u,0x00000000u);
store<u16>(0x1048E4DAu,0x00000023u);
++nextId;
store<u16>(0x1048E4E0u,0x00000000u);
store<u32>(0x1048E4C4u,nextId);
store<u32>(0x1048E4BCu,0x00000000u);
++nextId;
store<u16>(0x1048E4E2u,0x00000024u);
store<u16>(0x1048E4AAu,0x00000020u);
store<u16>(0x1048E4E8u,0x00000000u);
store<u32>(0x1048E524u,nextId);
store<u16>(0x1048E4B0u,0x00000000u);
store<u16>(0x1048E4B2u,0x00000021u);
store<u16>(0x1048E4B8u,0x00000000u);
store<u32>(0x1048E4C8u,0x100E377Cu);
store<u16>(0x1048E4EAu,0x00000000u);
store<u16>(0x1048E530u,0x00000000u);
store<u16>(0x1048E532u,0x00000028u);
store<u16>(0x1048E538u,0x00000000u);
store<u16>(0x1048E53Au,0x00000029u);
store<u16>(0x1048E4BAu,0x00000000u);
++nextId;
store<u32>(0x1048E528u,0x100E3790u);
store<u32>(0x1048E4F4u,nextId);
store<u32>(0x1048E4F8u,0x100E36ECu);
store<u32>(0x1048E49Cu,0x100E33F4u);
store<u32>(0x1048E4C0u,0x101FF32Cu);
store<u32>(0x1048E4D4u,0x00000004u);
store<u16>(0x1048E540u,0x00000000u);
store<u16>(0x1048E542u,0x0000002Au);
store<u16>(0x1048E548u,0x00000000u);
store<u16>(0x1048E54Au,0x00000000u);
store<u32>(0x1048E4DCu,0x00000004u);
store<u32>(0x1048E4E4u,0x00000004u);
store<u32>(0x1048E4ECu,0x00000000u);
store<u32>(0x1048E4CCu,0x100E33F4u);
store<u32>(0x1048E534u,0x00000004u);
store<u32>(0x1048E53Cu,0x00000004u);
store<u32>(0x1048E544u,0x00000004u);
store<u32>(0x1048E54Cu,0x00000000u);
store<u32>(0x1048E52Cu,0x100E33F4u);
store<u32>(0x1048E550u,0x101FF32Cu);
store<u32>(0x1048E4F0u,0x101FF32Cu);
store<u16>(0x1048E500u,0x00000000u);
store<u16>(0x1048E502u,0x00000025u);
store<u16>(0x1048E508u,0x00000000u);
store<u16>(0x1048E50Au,0x00000026u);
store<u16>(0x1048E510u,0x00000000u);
store<u32>(0x1048E504u,0x00000004u);
++nextId;
store<u16>(0x1048E560u,0x00000000u);
store<u32>(0x1048E554u,nextId);
store<u32>(0x1048E558u,0x100E37A0u);
store<u32>(0x1048E564u,0x00000004u);
store<u32>(0x1048E56Cu,0x00000004u);
store<u32>(0x1048E574u,0x00000004u);
store<u32>(0x1048E57Cu,0x00000000u);
store<u32>(0x1048E55Cu,0x100E33F4u);
store<u32>(0x1048E50Cu,0x00000004u);
store<u32>(0x1048E514u,0x00000004u);
store<u16>(0x1048E562u,0x0000002Bu);
store<u16>(0x1048E568u,0x00000000u);
store<u32>(0x1048E51Cu,0x00000000u);
store<u16>(0x1048E56Au,0x0000002Cu);
store<u16>(0x1048E570u,0x00000000u);
store<u16>(0x1048E572u,0x0000002Du);
store<u16>(0x1048E578u,0x00000000u);
store<u32>(0x1048E4FCu,0x100E33F4u);
store<u16>(0x1048E57Au,0x00000000u);
store<u16>(0x1048E512u,0x00000027u);
store<u32>(0x101FDD50u,nextId);
store<u32>(0x1048E520u,0x101FF32Cu);
store<u16>(0x1048E518u,0x00000000u);
store<u16>(0x1048E51Au,0x00000000u);
store<u32>(0x1048E580u,0x101FF32Cu);
}
VERIFY(0x0261F7AC,metroStaticInit);

void metroSmallDtor(u32 p,u32 flag) {WWHD_FUNC(0x0261FF78,void,p,flag);if(p&&(flag&1))call<void>(0x0273AF40,p);}
VERIFY(0x0261FF78,metroSmallDtor);
u32 metroStringValue(u32 p) {WWHD_FUNC(0x0261FFA8,u32,p);return load<u32>(p);}
VERIFY(0x0261FFA8,metroStringValue);
void metroNoop0261FF8C() {WWHD_FUNC(0x0261FF8C,void);}
VERIFY(0x0261FF8C,metroNoop0261FF8C);
void metroNoop0261FF90() {WWHD_FUNC(0x0261FF90,void);}
VERIFY(0x0261FF90,metroNoop0261FF90);
void metroNoop0261FF94() {WWHD_FUNC(0x0261FF94,void);}
VERIFY(0x0261FF94,metroNoop0261FF94);
void metroNoop0261FF98() {WWHD_FUNC(0x0261FF98,void);}
VERIFY(0x0261FF98,metroNoop0261FF98);
void metroNoop0261FF9C() {WWHD_FUNC(0x0261FF9C,void);}
VERIFY(0x0261FF9C,metroNoop0261FF9C);
void metroNoop0261FFA0() {WWHD_FUNC(0x0261FFA0,void);}
VERIFY(0x0261FFA0,metroNoop0261FFA0);
void metroNoop0261FFA4() {WWHD_FUNC(0x0261FFA4,void);}
VERIFY(0x0261FFA4,metroNoop0261FFA4);
void metroNoop0261FFB0() {WWHD_FUNC(0x0261FFB0,void);}
VERIFY(0x0261FFB0,metroNoop0261FFB0);
void metroNoop0261FFB4() {WWHD_FUNC(0x0261FFB4,void);}
VERIFY(0x0261FFB4,metroNoop0261FFB4);
void metroNoop0261FFB8() {WWHD_FUNC(0x0261FFB8,void);}
VERIFY(0x0261FFB8,metroNoop0261FFB8);

static u32 metroMemberTarget(u32 desc,u32 base,u32 off,u32& self,u32& r9,u32& r10) {const s32 slot=load<s16>(desc+off+2);r9=u32(s32(load<s16>(desc+off)));self=base+r9;if(slot<0)return load<u32>(desc+off+4);const s32 vt=load<s16>(desc+off+6);r9=u32(slot)*8;r10=load<u32>(self+u32(vt))+r9;return load<u32>(r10+4);}
u32 metroDispatch0261FFBC(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x0261FFBC,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=metroMemberTarget(desc,base,12,self,r9,r10);return call_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x0261FFBC,metroDispatch0261FFBC);
u32 metroDispatch0261FFFC(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x0261FFFC,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=metroMemberTarget(desc,base,20,self,r9,r10);return call_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x0261FFFC,metroDispatch0261FFFC);
u32 metroDispatch0262003C(u32 desc,u32 base,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x0262003C,u32,desc,base,r5,r6,r7,r8,r9,r10);u32 self;const u32 target=metroMemberTarget(desc,base,28,self,r9,r10);return call_ptr<u32>(target,self,base,r5,r6,r7,r8,r9,r10);}
VERIFY(0x0262003C,metroDispatch0262003C);
u32 metroDescriptorValue(u32 p,u32 r4,u32 r5,u32 r6,u32 r7,u32 r8,u32 r9,u32 r10) {WWHD_FUNC(0x0262007C,u32,p,r4,r5,r6,r7,r8,r9,r10);const u32 desc=load<u32>(p+0x2C);if(load<s32>(desc)==-1)return load<u32>(p);const u32 target=load<u32>(load<u32>(desc+8)+0x14);return call_ptr<u32>(target,desc,r4,r5,r6,r7,r8,r9,r10);}
VERIFY(0x0262007C,metroDescriptorValue);
