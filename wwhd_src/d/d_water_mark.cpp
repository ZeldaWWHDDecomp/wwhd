// HD water-mark effect.
#include "gabi.h"
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 F(u32 p,u32 n=0){return load<f32>(p+n);}
static void S(u32 p,u32 n,f32 x){store<f32>(p+n,x);}
struct WaterVec { be<u32> xyz[3]; };
struct WaterRes { be<u32> a,b; };
u32 waterDraw(u32 p){
 WWHD_FUNC(0x025CC200,u32,p);Local<WaterVec> v;
 u32 x=U(p,0xE0),y=U(p,0xE4),z=U(p,0xE8);v->xyz[0]=x;v->xyz[1]=y;v->xyz[2]=z;
 u32 clipped=call<u32>(0x02838148,0x1048CFF0u,0x104B45F8u,v.get(),fmuls_ppc(F(0x10056540),F(p,0xEC)));
 if(!clipped){call<void>(0x025E83FC,p+0x104,U(U(p,0x100),0xAC),F(p,0x108));call<void>(0x025E7B3C,p+0x17C,U(U(p,0x100),0xAC),load<s16>(p+0xFA));call<void>(0x025E2DE0,U(p,0x100),0u);}return 1;
}
VERIFY(0x025CC200,waterDraw);
u32 waterSetMatrix(u32 p){
 WWHD_FUNC(0x025CC2B4,u32,p);u32 chk=0x104872E8u;
 f32 y=fadds_ppc(F(p,0xE4),F(0x10056638)),x=F(p,0xE0),z=F(p,0xE8);S(chk,0x24,x);S(chk,0x2C,z);S(chk,0x28,y);
 u32 g=call<u32>(0x025200D4);f32 ground=call<f32>(0x02008974,g+0x12A0,chk);f32 bad=F(0x1005663C);S(p,0xE4,ground);if(ground==bad)return 0;
 g=call<u32>(0x025200D4);u32 plane=call<u32>(0x020084C8,g+0x12A0,load<u16>(chk+0x16),load<u16>(chk+0x14));if(!plane)return 0;
 s16 direction=call<s16>(0x020195B0,F(plane),F(plane,8));u32 relative=(u16)((s32)direction-(s32)load<s16>(p+0x1F2));
 Local<WaterVec> v;v->xyz[0]=U(plane);v->xyz[2]=U(plane,8);store<f32>(ea(v.get())+4,F(0x10056640));
 f32 mag=call<f32>(0x028E8DD0,v.get());f32 len=call<f32>(0x028F4384,mag);
 u32 mtx=0x1048D0CCu;call<void>(0x028E93CC,mtx,F(p,0xE0),fadds_ppc(F(p,0xE4),F(0x10056644)),F(p,0xE8));
 u32 table=0x104A44F8u+(relative>>3)*8;
 s16 rotX=call<s16>(0x020195B0,fmuls_ppc(len,F(table,4)),F(plane,4));
 s16 heading=load<s16>(p+0x1F2);s16 rotZ=call<s16>(0x020195B0,-fmuls_ppc(len,F(table)),F(plane,4));
 call<void>(0x025F1B48,mtx,rotX,heading,rotZ);
 f32 a[12];for(u32 n=0;n<12;++n)a[n]=F(mtx,n*4);u32 model=U(p,0x100);
 S(model,0xF4,a[11]);S(model,0xE4,a[7]);S(model,0xD0,a[2]);S(model,0xE8,a[8]);S(model,0xDC,a[5]);S(model,0xEC,a[9]);S(model,0xD8,a[4]);S(model,0xF0,a[10]);S(model,0xE0,a[6]);S(model,0xC8,a[0]);S(model,0xD4,a[3]);S(model,0xCC,a[1]);
 g=call<u32>(0x025200D4);s16 moving=call<s16>(0x024EEABC,g+0x12A0,chk+0x14);store<s16>(p+0x1F0,moving);return 1;
}
VERIFY(0x025CC2B4,waterSetMatrix);
u32 waterExecute(u32 p){
 WWHD_FUNC(0x025CC4A4,u32,p);bool play=load<s16>(p+0x1F8)==-1;
 if(!play){s16 start=load<s16>(p+0x1F4),end=load<s16>(p+0x1F6),now=load<s16>(0x101F133Eu);
  if((start<end && !(start>now) && !(end<=now)) || (start>=end && ((start<=now) || !(end<=now)))){store<s16>(p+0x1F8,-1);play=true;}}
 if(play)call<void>(0x025E742C,p+0x104);
 bool remove=(load<u8>(p+0x113)&1)!=0;
 if(!remove){if(F(p,0x104)==F(0x10056640))remove=true;else if(load<s16>(p+0x1F0)==1)remove=call<u32>(0x025CC2B4,p)==0;}
 if(remove)call<void>(0x025DAD48,p);return 1;
}
VERIFY(0x025CC4A4,waterExecute);
u32 waterIsDelete(u32 p){WWHD_FUNC(0x025CC564,u32,p);return 1;}
VERIFY(0x025CC564,waterIsDelete);
u32 waterDelete(u32 p){WWHD_FUNC(0x025CC56C,u32,p);u32 heap=U(p,0xFC);if(heap)call<void>(0x025E3868,heap);if(U(p,0xF8)==1)store<s16>(0x101F133Cu,(s16)((u16)load<s16>(0x101F133Cu)-1));call<void>(0x025DD630,p,0u);return 1;}
VERIFY(0x025CC56C,waterDelete);
u32 waterCreate(u32 p){
 WWHD_FUNC(0x025CC5D0,u32,p);if(p){call<void>(0x025DD5F0,p);store<u32>(p+0xB4,0x10056570u);call<void>(0x025E80D0,p+0x104);call<void>(0x025E7820,p+0x17C);}
 u32 params=U(p,0xF8),kind=params&0xFFFFu;store<u16>(p+0x1F2,(u16)(params>>16));store<u32>(p+0xF8,kind);
 if(kind>2)return 5;if(kind==1){s16 count=(s16)((u16)load<s16>(0x101F133Cu)+1);store<s16>(0x101F133Cu,count);if(count>10)return 5;}
 u32 parent=U(0x1047C8B4u),heap;
 if(parent){heap=call<u32>(0x025E35BC,0x3534u,parent,0x100u);store<u32>(p+0xFC,heap);}else heap=U(p,0xFC);
 if(!heap){parent=call<u32>(0x025E2F64,0u);heap=call<u32>(0x025E35BC,0x3534u,parent,0x100u);store<u32>(p+0xFC,heap);
  if(!heap){parent=call<u32>(0x025E2F64,1u);heap=call<u32>(0x025E35BC,0x3534u,parent,0x100u);store<u32>(p+0xFC,heap);if(!heap)return 5;}}
 store<u32>(heap+16,0x10056548u);Local<WaterRes> modelName;modelName->a=0x10056550u;modelName->b=0x10056558u;
 u32 data=call<u32>(0x026066C4,U(0x101F4F28u),modelName.get(),0x2Fu);if(!data)call<void>(0x0273AA24,0x10056610u,0x14Eu,0x10056624u);
 u32 model=call<u32>(0x025E38E0,data,0x80000u,0x11020022u);store<u32>(p+0x100,model);
 Local<WaterRes> brkName;brkName->b=0x10056558u;brkName->a=0x10056550u;
 u32 resource=call<u32>(0x026066C4,U(0x101F4F28u),brkName.get(),0x4Du);f32 rate=F(0x10056648);
 u32 brk=call<u32>(0x025E8154,p+0x104,data,resource,1u,0u,0u,0xFFFFFFFFu,0u,0u,rate);
 Local<WaterRes> btpName;btpName->b=0x10056558u;btpName->a=0x10056550u;
 resource=call<u32>(0x026066C4,U(0x101F4F28u),btpName.get(),0x63u);
 u32 ok=brk&call<u32>(0x025E789C,p+0x17C,data,resource,0u,0u,0u,0xFFFFFFFFu,0u,0u,rate);
 call<void>(0x025E37D8);call<void>(0x025E3678,U(p,0xFC));model=U(p,0x100);if(!model||!ok)return 5;
 f32 sx=F(p,0xEC),sz=F(p,0xF4),sy=F(p,0xF0);S(model,0xC4,sz);S(model,0xBC,sx);S(model,0xC0,sy);
 if(!call<u32>(0x025CC2B4,p))return 5;
 if(U(p,0xF8)==2){s16 old=load<s16>(0x101F133Eu);store<s16>(p+0x1F8,old);s16 next=(s16)((u16)load<s16>(0x101F133Eu)+1);store<s16>(0x101F133Eu,next==40?0:next);
  s16 start=(s16)((u16)load<s16>(p+0x1F8)+20);if(start>=40)start=(s16)((u16)start-40);s16 end=(s16)((u16)start+20);store<s16>(p+0x1F4,start);
  if(end<40){store<s16>(p+0x1F6,end);store<u32>(p+0xF8,0);}else{store<u32>(p+0xF8,0);store<s16>(p+0x1F6,(s16)((u16)end-40));}
 }else store<s16>(p+0x1F8,-1);return 4;
}
VERIFY(0x025CC5D0,waterCreate);
void waterStaticInit(){
 WWHD_FUNC(0x025CC934,void);store<u32>(0x104872E0u,0);store<u32>(0x104872D8u,0);store<u32>(0x104872E4u,0);store<u32>(0x104872DCu,0);call<void>(0x028F026C,0x101F130Cu);
 f32 a=F(0x10056650),b=F(0x10056654);S(0x104872CC,0,a);S(0x104872D0,0,b);call<void>(0x028ED6F8,0x104872D4u);call<void>(0x028F026C,0x101F1318u);call<void>(0x028EAB2C,0x104872D5u);call<void>(0x028F026C,0x101F1324u);
 u32 p=0x104872E8u;call<void>(0x02008E0C,p);store<u8>(p+0x45,0);store<u8>(p+0x46,0);store<u32>(p,p+0x40);store<u8>(p+0x47,0);store<u8>(p+0x48,0);store<u32>(p+4,p+0x4C);store<u32>(p+0x50,1);store<u8>(p+0x49,0);store<u32>(p+0x10,0x100565D0u);store<u32>(p+0x20,0x100565E0u);store<u32>(p+0x4C,0x100565F0u);store<u8>(p+0x4A,0);store<u32>(p+0x40,0x10056600u);store<u8>(p+0x44,1);call<void>(0x028F026C,0x101F1330u);
}
VERIFY(0x025CC934,waterStaticInit);
void waterEmptyDtor(u32 p,u32 flags){WWHD_FUNC(0x025CCA58,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025CCA58,waterEmptyDtor);
void waterGroundDtor(u32 p,u32 flags){WWHD_FUNC(0x025CCA6C,void,p,flags);if(p){store<u32>(p+0x20,0x100565A0u);store<u32>(p+0x40,0x100565C0u);store<u32>(p+0x4C,0x10056580u);call<void>(0x02008DAC,p,0u);if(flags&1)call<void>(0x0273AF40,p);}}
VERIFY(0x025CCA6C,waterGroundDtor);
void waterEmpty(u32 p){WWHD_FUNC(0x025CCAE4,void,p);}
VERIFY(0x025CCAE4,waterEmpty);
