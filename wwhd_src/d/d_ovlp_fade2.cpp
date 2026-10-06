#include "wwhd.h"
#include "gabi.h"
static void phase(u32 s,u32 target) {
  gabi::store<s16>(s+0xD2,-1);
  gabi::store<s16>(s+0xD0,0);
  gabi::store<u32>(s+0xD4,target);
}
void Fd2_Draw(u32 s) {
  WWHD_FUNC(0x025A1BD0,void,s);
  if(gabi::load<u8>(s+0x11C)) {
    u32 play=gabi::call<u32>(0x025200D4);
    if(!gabi::load<u8>(play+0x5AC9)) {
      f32 z=gabi::load<f32>(0x1047B610)+(-420.0f);
      gabi::call<void>(0x028E93CC,0x1048D0CC,0.0f,0.0f,z);
      gabi::call<void>(0x025F24E0,0.5f,0.5f,0.0f);
      gabi::call<void>(0x025F1C5C,0x1048D0CC,gabi::load<s16>(s+0x114));
      f32 y=-gabi::fmsubs(gabi::load<f32>(s+0x118),0.5f,1.0f);
      gabi::call<void>(0x025F2518,1.0f,y,1.0f);
      gabi::call<void>(0x025F1C5C,0x1048D0CC,s16(-s32(gabi::load<s16>(s+0x114))));
      gabi::call<void>(0x025F24E0,-0.5f,-0.5f,0.0f);
      gabi::call<void>(0x028E90D4,0x1048D0CC,s+0xE0);
      play=gabi::call<u32>(0x025200D4)+0x5D30;
      gabi::call<void>(0x0252CDC0,play,play+0x264,play+0x268,s+0xDC);
    }
  }
  gabi::store<u8>(0x101F4825,0);
}
VERIFY(0x025A1BD0,Fd2_Draw);
u32 Fd2_DrawWrapper(u32 s) {
  WWHD_FUNC(0x025A1D10,u32,s);
  Fd2_Draw(s);
  return 1;
}
VERIFY(0x025A1D10,Fd2_DrawWrapper);
u32 Fd2_Execute(u32 s) {
  WWHD_FUNC(0x025A1D34,u32,s);
  s16 k=gabi::load<s16>(s+0xD2);
  u32 self=s+s32(gabi::load<s16>(s+0xD0));
  u32 target;
  if(k<0)target=gabi::load<u32>(s+0xD4);
  else {
    u32 vt=gabi::load<u32>(self+s32(gabi::load<s16>(s+0xD6)));
    target=gabi::load<u32>(vt+u32(s32(k))*8+4);
  }
  gabi::call_ptr<void>(target,self);
  return 1;
}
VERIFY(0x025A1D34,Fd2_Execute);
u32 Fd2_Delete(u32 s) {
  WWHD_FUNC(0x025A1DA0,u32,s);
  if(s) {
    gabi::call<void>(0x027B9694,s+0x120,2);
    gabi::call<void>(0x0252CCFC,s+0xDC,0);
    gabi::call<void>(0x0252CCFC,s+0xD8,0);
    gabi::call<void>(0x025DD630,s,0);
  }
  gabi::call<void>(0x027F88C8,gabi::load<u32>(0x101F9968),4);
  gabi::call<void>(0x02728EE0,gabi::load<u32>(0x101F86B4));
  return 1;
}
VERIFY(0x025A1DA0,Fd2_Delete);
u32 Fd2_ctor(u32 s) {
  WWHD_FUNC(0x025A1E1C,u32,s);
  if(!s)s=gabi::call<u32>(0x0273AD10,0x2A8);
  if(s) {
    gabi::call<void>(0x025DD5F0,s);
    gabi::store<u32>(s+0xB4,0x100518C4);
    gabi::call<void>(0x0252CCBC,s+0xD8);
    gabi::store<u32>(s+0xD8,0x10051940);
    gabi::call<void>(0x0252CCBC,s+0xDC);
    gabi::store<u32>(s+0xDC,0x10051900);
    gabi::call<void>(0x027B95B4,s+0x120);
    gabi::call<void>(0x027B6CB8,s+0x160);
    phase(s,0x025A1F20);
    u32 p=gabi::call<u32>(0x025200D4);
    gabi::store<u8>(p+0x5C22,0);
    gabi::store<u8>(s+0x11D,2);
    gabi::call<void>(0x02728E64,gabi::load<u32>(0x101F86B4),s);
    gabi::call<void>(0x028E9098,s+0xE0);
  }
  return s;
}
VERIFY(0x025A1E1C,Fd2_ctor);
u32 Fd2_Create(u32 s) {
  WWHD_FUNC(0x025A1EF4,u32,s);
  if(s)Fd2_ctor(s);
  return 4;
}
VERIFY(0x025A1EF4,Fd2_Create);
void Fd2_First(u32 s) {
  WWHD_FUNC(0x025A1F20,void,s);
  if(gabi::load<u8>(s+0x11C)) {
    if(!gabi::call<u32>(0x0220CCB4,s+0x11D)) {
      phase(s,0x025A1FB4);
      gabi::call<void>(0x025DBDDC,s);
      gabi::store<s8>(s+0x11D,-12);
    }
    u32 p=gabi::call<u32>(0x025200D4);
    gabi::store<u8>(p+0x5AC9,0);
    if(gabi::load<u8>(0x101F4829))gabi::call<void>(0x025F0830);
  }
}
VERIFY(0x025A1F20,Fd2_First);
void Fd2_Out(u32 s) {
  WWHD_FUNC(0x025A1FB4,void,s);
  u32 p=gabi::call<u32>(0x025200D4);
  gabi::store<u8>(p+0x5AC9,0);
  gabi::call<void>(0x0200F8D0,s+0x112,2000,100);
  s32 a=gabi::load<s16>(s+0x110),b=gabi::load<s16>(s+0x112);
  u32 cross=(u32(a+0x4000)&0x8000)|0x4000;
  s32 sum=a+b;
  s32 diff=s16(s32(cross)-b-sum);
  gabi::store<s16>(s+0x110,s16(sum));
  s32 timer=gabi::load<s8>(s+0x11D);
  bool transitioned=false;
  if(diff*b<0&&timer==0) {
    if(gabi::call<u32>(0x025DBDC4,s)) {
      gabi::call<void>(0x025DBD74);
      phase(s,0x025A2150);
      gabi::store<u8>(s+0x11D,15);
      gabi::store<s16>(s+0x110,-0x4000);
      gabi::call<void>(0x0220CCB4,s+0x11D);
      transitioned=true;
    }
    else timer=gabi::load<s8>(s+0x11D);
  }
  if(!transitioned) {
    if(timer<0) {
      s8 next=s8(timer+1);
      gabi::store<s8>(s+0x11D,next);
      if(!next) {
        gabi::call<void>(0x025F05E8,16);
        gabi::store<u8>(s+0x11D,u8(s32(gabi::load<s16>(0x1047B68A))+20));
      }
    }
    else gabi::call<void>(0x0220CCB4,s+0x11D);
  }
  s32 step=s32(gabi::load<s16>(0x1047B688))+0x800;
  gabi::store<s16>(s+0x114,s16(s32(gabi::load<s16>(s+0x114))+step));
  f32 f1=gabi::load<f32>(0x1047B614)+1.0f,f3=gabi::load<f32>(0x1047B618)+0.05f;
  gabi::call<void>(0x0200ED84,s+0x118,f1,1.0f,f3);
}
VERIFY(0x025A1FB4,Fd2_Out);
void Fd2_Next(u32 s) {
  WWHD_FUNC(0x025A2150,void,s);
  if(!gabi::call<u32>(0x0220CCB4,s+0x11D)&&!gabi::call<u32>(0x02728CB8,gabi::load<u32>(0x101F86B4),16,1)) {
    gabi::store<u8>(s+0x11C,0);
    gabi::store<s16>(s+0x110,s16(s32(gabi::load<s16>(s+0x110))+s32(gabi::load<s16>(s+0x112))));
    u32 p=gabi::call<u32>(0x025200D4);
    gabi::store<u8>(p+0x5AC9,1);
    p=gabi::call<u32>(0x025200D4);
    gabi::store<u8>(p+0x5C22,0);
    phase(s,0x025A21F0);
  }
}
VERIFY(0x025A2150,Fd2_Next);
void Fd2_In(u32 s) {
  WWHD_FUNC(0x025A21F0,void,s);
  s32 step=s32(gabi::load<s16>(0x1047B688))+0x800;
  gabi::store<s16>(s+0x114,s16(s32(gabi::load<s16>(s+0x114))-step));
  f32 f2=gabi::load<f32>(0x1047B61C)+0.03f;
  gabi::call<void>(0x0200EDC8,s+0x118,1.0f,f2);
  if(gabi::load<f32>(s+0x118)<0.001f) {
    if(!gabi::load<u8>(s+0x11E)) {
      gabi::call<void>(0x025DBD74);
      gabi::store<u8>(s+0x11E,1);
    }
    else {
      gabi::call<void>(0x025DBDDC,s);
      u32 p=gabi::call<u32>(0x025200D4);
      gabi::store<u8>(p+0x5AC9,1);
      p=gabi::call<u32>(0x025200D4);
      gabi::store<u8>(p+0x5C22,1);
    }
  }
  else {
    u32 p=gabi::call<u32>(0x025200D4);
    gabi::store<u8>(p+0x5AC9,0);
    gabi::call<void>(0x025DBD24);
  }
}
VERIFY(0x025A21F0,Fd2_In);
void Fd2_MtxCopy(u32 dst,u32 src) {
  WWHD_FUNC(0x025A22D4,void,dst,src);
  f32 a[12];
  for(u32 i=0;i<12;++i)a[i]=gabi::load<f32>(src+4*i);
  for(u32 i=0;i<12;++i)gabi::store<f32>(dst+4*i,a[i]);
}
VERIFY(0x025A22D4,Fd2_MtxCopy);
void Fd2_Snapshot(u32 s,u32 snap) {
  WWHD_FUNC(0x025A2374,void,s,snap);
  u32 manager=gabi::load<u32>(0x101F9968);
  if(!gabi::load<u8>(s+0x11C)) {
    gabi::call<void>(0x027F88C8,manager,4);
    gabi::call<void>(0x027F84A4,manager,4,snap);
    gabi::store<u8>(s+0x11C,1);
  }
  u32 renderer=gabi::load<u32>(0x101F8710);
  gabi::call<void>(0x0274D690,gabi::load<u32>(snap+8));
  u32 r3=gabi::load<u32>(snap+0x1C),r4=gabi::load<u32>(snap+8);
  gabi::call<void>(0x0274F964,r3,r4);
  u32 texture=gabi::call<u32>(0x027F81A4,manager,4);
  gabi::Local<u8[0x198]> view;
  gabi::call<void>(0x027BE114,view.a,texture);
  gabi::Local<u8[48]> mtx;
  Fd2_MtxCopy(mtx.a,s+0xE0);
  u32 ref=gabi::call<u32>(0x027F29D4,0x104B45C0);
  u32 old=gabi::load<u32>(ref);
  u32 handle=gabi::call<u32>(0x0272CAE0,renderer,view.a,mtx.a,old);
  ref=gabi::call<u32>(0x027F29D4,0x104B45C0);
  gabi::store<u32>(ref,handle);
  gabi::store<u32>(ref+4,0);
  gabi::call<void>(0x027BE2B0,view.a,2);
}
VERIFY(0x025A2374,Fd2_Snapshot);
void Fd2_Init() {
  WWHD_FUNC(0x025A2474,void);
  gabi::store<u32>(0x1047B114,0);
  gabi::store<u32>(0x1047B10C,0);
  gabi::store<u32>(0x1047B118,0);
  gabi::store<u32>(0x1047B110,0);
  gabi::call<void>(0x028F026C,0x101EA2EC);
  gabi::store<f32>(0x1047B100,-3.1415927410125732f);
  gabi::store<f32>(0x1047B104,3.1415927410125732f);
  gabi::call<void>(0x028ED6F8,0x1047B108);
  gabi::call<void>(0x028F026C,0x101EA2F8);
  gabi::call<void>(0x028EAB2C,0x1047B109);
  gabi::call<void>(0x028F026C,0x101EA304);
}
VERIFY(0x025A2474,Fd2_Init);
u32 Fd2_IsDelete(u32 s) {
  WWHD_FUNC(0x025A2508,u32,s);
  return 1;
}
VERIFY(0x025A2508,Fd2_IsDelete);
void Fd2_Empty(u32 s) {
  WWHD_FUNC(0x025A2510,void,s);
}
VERIFY(0x025A2510,Fd2_Empty);
