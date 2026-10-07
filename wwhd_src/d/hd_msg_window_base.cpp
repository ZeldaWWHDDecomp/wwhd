/* HD message-window base: binary reconstruction of the scoped typing/wait states.
 * Original 30 Hz behaviour. */
#include "gabi.h"
using namespace gabi;
namespace hd_msg_window_base {
static inline u32 ld(u32 p) { return load<u32>(p); }
static inline void st(u32 p,u32 v) { store<u32>(p,v); }
static inline u32 vf(u32 s,u32 off) { return ld(ld(s+4)+off); }
static void enterOpen(u32 s) {
 WWHD_FUNC(0x026FCA80,void,s);
 st(s+0x94,ld(0x1010AC34));
 call_ptr<void>(vf(s,0x434),s);
 if(load<u8>(s+0x110) && !load<u8>(s+0x111)) call_ptr<void>(vf(s,0x42C),s);
 call<void>(0x025F74D0,ld(0x101F4B5C),6);
}
VERIFY(0x026FCA80,enterOpen);
static u32 advanceTyping(u32 s) {
 WWHD_FUNC(0x026FCB00,u32,s);
 u32 flags=ld(s+0x50), msg=ld(s+0x118), n=0;
 float acc=load<float>(0x1010AC34);
 if(flags&2) {
  u32 input=ld(msg+0x22C);
  if(input && !load<u8>(input+2)) {
   st(s+0x90,0x14F); st(ld(s+0x118)+0x6AC,0);
   store<float>(s+0x94,acc); return 1;
  }
 }
 s32 pause=load<s32>(msg+0x6AC);
 if(pause!=0) {
  st(msg+0x6AC,pause>0 ? u32(pause)-1:0);
  store<float>(s+0x94,acc); return 0;
 }
 acc=load<float>(s+0x94);
 float speed=load<float>(ld(0x101F4B5C)+0x980);
 if(flags&1) {
  u32 input=ld(msg+0x22C);
  if(input && !load<u8>(input+2)) speed=fmuls_ppc(speed,load<float>(0x1010AC38));
 }
 acc=fadds_ppc(acc,speed);
 float one=1.0f; // RPX read-only constant 1010AC28.
 if(acc>=one) do { acc=fsubs_ppc(acc,one); ++n; } while(acc>=one);
 store<float>(s+0x94,acc); return n;
}
VERIFY(0x026FCB00,advanceTyping);
static u32 hasInput(u32 s) {
 WWHD_FUNC(0x026FCEC8,u32,s);
 return ld(s+0x54)&1;
}
VERIFY(0x026FCEC8,hasInput);
static void updateOpen(u32 s) {
 WWHD_FUNC(0x026FCC10,void,s);
 u32 core=ld(0x101F4B5C), n=call<u32>(0x026FCB00,s);
 if(!n) return;
 if(load<s32>(s+0x90)<0x14F) {
  u32 count=ld(s+0x90)+n, index=ld(s+0xE8)+1;
  st(s+0x90,count);
  s32 stop=load<s32>(s+0xC0+(index<10 ? index*4:0));
  if(s32(count)>stop) st(s+0x90,u32(stop));
 }
 call_ptr<void>(vf(s,0x434),s);
 u32 msg=ld(s+0x118);
 if(load<u8>(msg+0x658)) return;
 u32 state=ld(msg), next=0;
 if(ld(msg+0x6B8)==1) {
  call<void>(0x025F74D0,core,ld(msg));
  next=ld(ld(s+0x118)+0x650) ? 0x1049E01C:0x1049E07C;
 } else if(state==7) {
  call<void>(0x025F74D0,core,ld(msg)); msg=ld(s+0x118);
  next=load<u8>(msg+0x697) ? 0x1049DF8C : (load<u8>(msg+0x698) ? 0x1049DFBC:0x1049DF2C);
 } else if(state==10) {
  call<void>(0x025F74D0,core,ld(msg)); msg=ld(s+0x118);
  if(load<u8>(msg+0x697)) next=0x1049E22C;
  else if(load<u8>(msg+0x698)) next=0x1049E25C;
 } else if(state==14 || state==21) {
  call<void>(0x025F74D0,core,ld(msg));
  next=state==14 ? 0x1049E2BC:0x1049E19C;
 }
 if(next) call<void>(0x020063C0,s+0x18,next);
}
VERIFY(0x026FCC10,updateOpen);
static void updateStop(u32 s) {
 WWHD_FUNC(0x026FCEE0,void,s);
 if(call<u32>(0x026FCEC8,s) || (ld(s+0x54)&2)) {
  u32 count=ld(s+0x84)+1; st(s+0x84,count);
  store<u8>(call<u32>(0x025200D4)+0x5BD2,count);
  store<u8>(0x1047B09E,2);
  u32 i=ld(s+0xE8)+1;
  if(load<s32>(s+0x98+(i<10?i*4:0))>=0) call<void>(0x020063C0,s+0x18,0x1049E3AC);
  else { call<void>(0x025F74D0,ld(0x101F4B5C),14); call<void>(0x020063C0,s+0x18,0x1049E2BC); }
 } else if(load<u8>(ld(s+0x118)+0x90E)) {
  call<void>(0x025F74D0,ld(0x101F4B5C),1); call<void>(0x020063C0,s+0x18,0x1049E40C);
 } else { call<void>(0x0270477C,ld(s+0x68),ld(s+0x118)); store<u8>(0x1047B09E,1); }
}
VERIFY(0x026FCEE0,updateStop);
static void updateWait(u32 s) {
 WWHD_FUNC(0x026FD054,void,s);
 u32 msg=ld(s+0x118);
 if(ld(msg+0x6B4)==0) {
  store<u8>(msg+0x697,0);
  u32 i=ld(s+0xE8)+1;
  if(load<s32>(s+0x98+(i<10?i*4:0))>=0) call<void>(0x020063C0,s+0x18,0x1049E3AC);
  else { call<void>(0x025F74D0,ld(0x101F4B5C),14); call<void>(0x020063C0,s+0x18,0x1049E2BC); }
 } else {
  s32 t=load<s32>(msg+0x6B4); st(msg+0x6B4,t>0?u32(t)-1:0);
  call<void>(0x0270477C,ld(s+0x68),ld(s+0x118));
 }
}
VERIFY(0x026FD054,updateWait);
static void updateWaitEnd(u32 s) {
 WWHD_FUNC(0x026FD14C,void,s);
 if(call_ptr<u32>(vf(s,0x4C4),s)) call<void>(0x020063C0,s+0x18,0x1049DFEC);
 else {
  u32 msg=ld(s+0x118); s32 t=load<s32>(msg+0x6B4);
  st(msg+0x6B4,t>0?u32(t)-1:0);
  call<void>(0x0270477C,ld(s+0x68),ld(s+0x118));
 }
}
VERIFY(0x026FD14C,updateWaitEnd);
static void updateWaitInput(u32 s) {
 WWHD_FUNC(0x026FD1DC,void,s);
 u32 msg=ld(s+0x118); bool accepted=ld(msg+0x6B4)==0; u8 mode=0;
 if(!accepted) {
  if(call<u32>(0x026FCEC8,s) || (ld(s+0x54)&2)) {
   u32 play=call<u32>(0x025200D4); msg=ld(s+0x118);
   if(!load<u8>(play+0x5C20)) { accepted=true; mode=2; }
  }
 }
 if(accepted) {
  store<u8>(msg+0x698,0);
  u32 count=ld(s+0x84)+1; st(s+0x84,count);
  store<u8>(call<u32>(0x025200D4)+0x5BD2,count); store<u8>(0x1047B09E,mode);
  u32 i=ld(s+0xE8)+1;
  if(load<s32>(s+0x98+(i<10?i*4:0))<0) {
   call<void>(0x025F74D0,ld(0x101F4B5C),14); call<void>(0x020063C0,s+0x18,0x1049E2BC);
  } else call<void>(0x020063C0,s+0x18,0x1049E3AC);
 } else {
  s32 t=load<s32>(msg+0x6B4); st(msg+0x6B4,t>0?u32(t)-1:0);
  call<void>(0x0270477C,ld(s+0x68),ld(s+0x118)); store<u8>(0x1047B09E,1);
 }
}
VERIFY(0x026FD1DC,updateWaitInput);
static void updateSelect2(u32 s) {
 WWHD_FUNC(0x026FD980,void,s);
 u32 core=ld(0x101F4B5C);
 bool input=call<u32>(0x026FCEC8,s)!=0;
 if(!input && (ld(s+0x54)&2)) input=load<u8>(call<u32>(0x025200D4)+0x5BB6)==39;
 if(!input) input=load<u8>(ld(s+0x118)+0x90D)!=0;
 if(input && call<u32>(0x0265233C,ld(core+0x964))) {
  if(ld(s+0x54)&2) store<u8>(call<u32>(0x025200D4)+0x5BD3,1);
  st(core+0x948,ld(s+0xF4)); call<void>(0x025F74D0,core,14);
  u32 count=ld(s+0x84)+1; st(s+0x84,count); store<u8>(call<u32>(0x025200D4)+0x5BD2,count);
  call<void>(0x025E1988,ld(s+0xF4)==0?0x80D:0x880);
  store<u8>(0x1047B09E,2); call<void>(0x020063C0,s+0x18,0x1049E10C); return;
 }
 u32 flags=ld(s+0x54),old=ld(s+0xF4),value=old;
 if(flags&0x100000) { value=0; st(s+0xF4,value); }
 else if(flags&0x200000) { value=1; st(s+0xF4,value); }
 else if(flags&0x10000) { value=0; st(s+0xF4,value); }
 else if(flags&0x20000) { value=1; st(s+0xF4,value); }
 if(value!=old) call<void>(0x025E1988,0x83D);
 store<u8>(0x1047B09E,1);
}
VERIFY(0x026FD980,updateSelect2);
static void updateSelect3(u32 s) {
 WWHD_FUNC(0x026FDB08,void,s);
 u32 core=ld(0x101F4B5C);
 bool input=call<u32>(0x026FCEC8,s)!=0;
 if(!input && (ld(s+0x54)&2)) input=load<u8>(call<u32>(0x025200D4)+0x5BB6)==39;
 if(!input) input=load<u8>(ld(s+0x118)+0x90D)!=0;
 if(input && call<u32>(0x0265233C,ld(core+0x964))) {
  if(ld(s+0x54)&2) store<u8>(call<u32>(0x025200D4)+0x5BD3,1);
  st(core+0x948,ld(s+0xF4)); call<void>(0x025F74D0,core,14);
  u32 count=ld(s+0x84)+1; st(s+0x84,count); store<u8>(call<u32>(0x025200D4)+0x5BD2,count);
  call<void>(0x025E1988,ld(s+0xF4)==2?0x880:0x80D);
  store<u8>(0x1047B09E,2); call<void>(0x020063C0,s+0x18,0x1049E10C); return;
 }
 u32 flags=ld(s+0x54),old=ld(s+0xF4),value=old;
 if(flags&0x100000) { if(s32(value)>0) { --value; st(s+0xF4,value); } }
 else if(flags&0x200000) { if(s32(value)<2) { ++value; st(s+0xF4,value); } }
 else if(flags&0x10000) { if(s32(value)>0) { --value; st(s+0xF4,value); } }
 else if(flags&0x20000) { if(s32(value)<2) { ++value; st(s+0xF4,value); } }
 if(value!=old) call<void>(0x025E1988,0x83D);
 store<u8>(0x1047B09E,1);
}
VERIFY(0x026FDB08,updateSelect3);

// Additional scoped selection and wait states (UI_msg_base_b).
u32 decreaseSelection(u32 s) {
 WWHD_FUNC(0x026FDEA0,u32,s);
 s32 value=load<s32>(s+0xF4);
 if(value<=0) return s;
 st(s+0xF4,u32(value)-1);
 return call<u32>(0x025E1988,0x8E1u);
}
VERIFY(0x026FDEA0,decreaseSelection);
u32 increaseSelection(u32 s,u32 limits) {
 WWHD_FUNC(0x026FDEC0,u32,s,limits);
 u32 limit=ld(limits)-1;
 s32 value=load<s32>(s+0xF4);
 if(value>=s32(limit)) return s;
 st(s+0xF4,u32(value)+1);
 return call<u32>(0x025E1988,0x8E1u);
}
VERIFY(0x026FDEC0,increaseSelection);
u32 approachNumber(u32 s,u32 number,u32 index) {
 WWHD_FUNC(0x026FDEE8,u32,s,number,index);
 u32 i=ld(index);
 if(i>=4) return 0;
 u32 table=0x1010AC54;
 u32 mode=ld(s+0xF4);
 s32 value=load<s16>(number), target=load<s16>(table+i*2);
 if(value>target) { store<u16>(number,u16(target)); return 0; }
 if(value==target || mode>2) return 0;
 s32 step=mode==0?1:(mode==1?10:100);
 value=s16(value+step);
 store<u16>(number,u16(value));
 i=ld(index);
 target=load<s16>(table+i*2);
 if(value>target) store<u16>(number,u16(target));
 return 1;
}
VERIFY(0x026FDEE8,approachNumber);
u32 decreaseNumber(u32 s,u32 number) {
 WWHD_FUNC(0x026FDFE0,u32,s,number);
 u32 mode=ld(s+0xF4);
 if(mode>2) return 0;
 s32 value=load<s16>(number);
 if(value<=0) return 0;
 s32 step=mode==0?1:(mode==1?10:100);
 value=s16(value-step);
 store<u16>(number,value<0?0:u16(value));
 return 1;
}
VERIFY(0x026FDFE0,decreaseNumber);
void updateNumberSelect(u32 s) {
 WWHD_FUNC(0x026FE414,void,s);
 if(call<u32>(0x026FCEC8,s)) {
  call<void>(0x025E1988,0x80Du);
  call<void>(0x025E1C64);
  u32 n=ld(s+0x84)+1; st(s+0x84,n);
  store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
  u32 pane=ld(s+0x7C); store<u8>(pane+0x44,load<u8>(pane+0x44)&0xFE);
  call<void>(0x020063C0,s+0x18,0x1049E49Cu);
  return;
 }
 bool three=ld(s+0x5C)==0x1CFA;
 FrameLocal<be<u32>> count(three?8:12);
 *count=three?3:2;
 call<void>(0x026FE070,s,count.get());
 u32 selection=ld(s+0xF4),core=ld(0x101F4B5C);
 st(core+0x948,selection);
 store<u8>(0x1047B09E,3);
 call_ptr<void>(vf(s,0x43C),s);
 u32 pane=ld(s+0x7C);
 store<u8>(pane+0x44,(load<u8>(pane+0x44)&0xFE)+1);
 call<void>(0x026FE228,s);
}
VERIFY(0x026FE414,updateNumberSelect);
void updateAutoWait(u32 s) {
 WWHD_FUNC(0x026FE698,void,s);
 u32 msg=ld(s+0x118);
 if(ld(msg+0x6B4)==0) {
  store<u8>(msg+0x697,0);call<void>(0x020063C0,s+0x18,0x1049E34Cu);
 } else {
  s32 timer=load<s32>(msg+0x6B4);st(msg+0x6B4,timer>0?ld(msg+0x6B4)-1:0);
  u32 pane=ld(s+0x68),message=ld(s+0x118);call<void>(0x0270477C,pane,message);
 }
}
VERIFY(0x026FE698,updateAutoWait);
void updateAutoWaitEnd(u32 s) {
 WWHD_FUNC(0x026FE6FC,void,s);
 if(call_ptr<u32>(vf(s,0x4C4),s)) call<void>(0x020063C0,s+0x18,0x1049E28Cu);
 else {
  u32 msg=ld(s+0x118);s32 t=load<s32>(msg+0x6B4);st(msg+0x6B4,t>0?u32(t)-1:0);
  u32 pane=ld(s+0x68),message=ld(s+0x118);call<void>(0x0270477C,pane,message);
 }
}
VERIFY(0x026FE6FC,updateAutoWaitEnd);
void updateAutoWaitInput(u32 s) {
 WWHD_FUNC(0x026FE78C,void,s);
 u32 msg=ld(s+0x118);
 if(ld(msg+0x6B4)==0) {
  store<u8>(msg+0x698,0);store<u8>(0x1047B09E,0);
  u32 n=ld(s+0x84)+1;st(s+0x84,n);store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
  call<void>(0x020063C0,s+0x18,0x1049E34Cu);return;
 }
 bool input=call<u32>(0x026FCEC8,s)!=0;
 if(!input) input=(ld(s+0x54)&2)!=0;
 if(!input) input=load<u8>(msg+0x90D)!=0;
 if(input) {
  u32 play=call<u32>(0x025200D4);u8 blocked=load<u8>(play+0x5C20);msg=ld(s+0x118);
  if(!blocked) {
   store<u8>(msg+0x698,0);store<u8>(0x1047B09E,0);
   call<void>(0x020063C0,s+0x18,0x1049E43Cu);return;
  }
 }
 s32 t=load<s32>(msg+0x6B4);st(msg+0x6B4,t>0?u32(t)-1:0);
 u32 pane=ld(s+0x68),message=ld(s+0x118);call<void>(0x0270477C,pane,message);
 store<u8>(0x1047B09E,4);
}
VERIFY(0x026FE78C,updateAutoWaitInput);
void updateHandStop(u32 s) {
 WWHD_FUNC(0x026FEAF4,void,s);
 bool input=call<u32>(0x026FCEC8,s)!=0;
 if(!input) input=(ld(s+0x54)&2)!=0;
 if(!input && ld(s+0x5C)==0x5AC) input=load<u8>(call<u32>(0x025200D4)+0x5BD3)!=0;
 if(input && !load<u8>(call<u32>(0x025200D4)+0x5C20)) {
  store<u8>(0x1047B09E,2);
  u32 play=call<u32>(0x025200D4);
  call<void>(0x025E1988,load<u8>(play+0x5BBA)==39?0x880u:0x80Bu);
  u32 n=ld(s+0x84)+1;st(s+0x84,n);
  store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
  store<u8>(call<u32>(0x025200D4)+0x5BD3,0);
  call<void>(0x020063C0,s+0x18,0x1049E3DCu);return;
 }
 u32 msg=ld(s+0x118);
 bool close=load<u8>(msg+0x90E)!=0;
 if(close) {
  u32 play=call<u32>(0x025200D4);u8 blocked=load<u8>(play+0x5C20);msg=ld(s+0x118);
  close=!blocked && !load<u8>(msg+0x90F) && ld(msg+0x910)!=ld(msg+0x11C);
 }
 if(close) {
  store<u8>(0x1047B09E,2);call<void>(0x025E1988,0x80Bu);
  call_ptr<void>(vf(s,0x4A4),s);call<void>(0x020063C0,s+0x18,0x1049DE9Cu);
 } else {
  call<void>(0x0270477C,ld(s+0x68),msg);store<u8>(0x1047B09E,1);
 }
}
VERIFY(0x026FEAF4,updateHandStop);
void updateNextStop(u32 s) {
 WWHD_FUNC(0x026FEDA8,void,s);
 if(!call_ptr<u32>(vf(s,0x4D4),s)) return;
 u32 next=ld(s+0xE8)+1,msg=ld(s+0x118);st(s+0xE8,next);st(s+0x90,0);
 u32 color=ld(msg+0x91D),pane=ld(s+0x68);
 st(pane+0xAC,color);st(pane+0xB0,color);store<u8>(s+0xED,0);
 call_ptr<void>(vf(s,0x434),s);call<void>(0x020063C0,s+0x18,0x1049DEFCu);
}
VERIFY(0x026FEDA8,updateNextStop);
void updateAdvanceA(u32 s) {
 WWHD_FUNC(0x026FEF28,void,s);
 call_ptr<void>(vf(s,0x4B4),s);
 u32 play=call<u32>(0x025200D4);
 call<void>(0x025E1988,load<u8>(play+0x5BBA)==39?0x880u:0x80Bu);
 u32 n=ld(s+0x84)+1;st(s+0x84,n);store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
}
VERIFY(0x026FEF28,updateAdvanceA);
void updateFinish(u32 s) {
 WWHD_FUNC(0x026FF028,void,s);
 if(!call_ptr<u32>(vf(s,0x4D4),s)) return;
 u32 n=ld(s+0x84)+1;store<u8>(s+0xED,0);st(s+0x84,n);
 store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
 u32 core=ld(0x101F4B5C);store<u8>(0x1047B09E,2);
 call<void>(0x025F74D0,core,14u);call<void>(0x020063C0,s+0x18,0x1049E2BCu);
}
VERIFY(0x026FF028,updateFinish);
void updateAdvanceB(u32 s) {
 WWHD_FUNC(0x026FF0BC,void,s);
 call_ptr<void>(vf(s,0x4B4),s);
 u32 play=call<u32>(0x025200D4);
 call<void>(0x025E1988,load<u8>(play+0x5BBA)==39?0x880u:0x80Bu);
 u32 n=ld(s+0x84)+1;st(s+0x84,n);store<u8>(call<u32>(0x025200D4)+0x5BD2,u8(n));
}
VERIFY(0x026FF0BC,updateAdvanceB);
}
