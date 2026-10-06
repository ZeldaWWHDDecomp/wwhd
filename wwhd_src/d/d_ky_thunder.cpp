// HD thunder effect.
#include "gabi.h"
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 F(u32 p,u32 n=0){return load<f32>(p+n);}
static void S(u32 p,u32 n,f32 x){store<f32>(p+n,x);}
struct ThunderVec { u8 bytes[12]; };
struct ThunderMtx { u8 bytes[48]; };
struct ThunderRes { be<u32> a,b; };
u32 thunderDraw(u32 p){
 WWHD_FUNC(0x02585098,u32,p);
 if(!U(0x101FDD00)){f32 z=F(0x10050484),x=F(0x1005047C),y=F(0x10050480);store<u32>(0x101FDD00u,1);S(0x101FDD04,0,x);S(0x101FDD04,4,y);S(0x101FDD04,8,z);}
 u32 g=call<u32>(0x025200D4),camera=U(g,0x5AF8);Local<ThunderVec> v;call<void>(0x0201AD78,camera+0xDC,v.get(),p+0x210);
 u32 m=0x1048D0CCu,vec=ea(v.get());call<void>(0x028E93CC,m,F(vec),F(vec,4),F(vec,8));
 call<void>(0x025F1C5C,m,(s16)ftoi(F(p,0x228)));call<void>(0x025F1BF4,m,(s16)ftoi(F(p,0x228)));
 Local<ThunderMtx> copy;call<void>(0x028E90D4,m,copy.get());u32 model=U(p,0x100);f32 sz=F(p,0x200),sx=F(p,0x1F8),sy=F(p,0x1FC);S(model,0xBC,sx);S(model,0xC0,sy);S(model,0xC4,sz);
 f32 a[12];for(u32 n=0;n<12;++n)a[n]=F(ea(copy.get()),n*4);model=U(p,0x100);
 S(model,0xDC,a[5]);S(model,0xE0,a[6]);S(model,0xE4,a[7]);S(model,0xEC,a[9]);S(model,0xF0,a[10]);S(model,0xF4,a[11]);S(model,0xC8,a[0]);S(model,0xCC,a[1]);S(model,0xD0,a[2]);S(model,0xD4,a[3]);S(model,0xD8,a[4]);S(model,0xE8,a[8]);
 call<void>(0x025E7FC4,p+0x10C,U(U(p,0x100),0xAC),F(p,0x22C));call<void>(0x025E83FC,p+0x180,U(U(p,0x100),0xAC),F(p,0x184));
 g=call<u32>(0x025200D4);store<u32>(0x104B4634u,U(g,0x5D78));g=call<u32>(0x025200D4);store<u32>(0x104B4638u,U(g,0x5D7C));
 call<void>(0x025E2DE0,U(p,0x100),0u);call<void>(0x025E8CD8,p+0x104);store<u32>(U(U(p,0x100),0xAC)+0x44,0);store<u32>(U(U(p,0x100),0xAC)+0x48,0);return 1;
}
VERIFY(0x02585098,thunderDraw);
u32 thunderExecute(u32 p){WWHD_FUNC(0x02585278,u32,p);call<void>(0x025200D4);S(p,0x180,F(0x10050488));if(call<u32>(0x025E742C,p+0x180)){call<void>(0x025E19CC,0x69F6u,p+0x204);call<void>(0x025DAD48,p);}return 1;}
VERIFY(0x02585278,thunderExecute);
u32 thunderDelete(u32 p){WWHD_FUNC(0x025852D8,u32,p);call<void>(0x025E1B34,p+0x204);call<void>(0x025E1B34,p+0x21C);call<void>(0x025E3868,U(p,0xFC));call<void>(0x025E89F8,p+0x104,2u);call<void>(0x025DD630,p,0u);return 1;}
VERIFY(0x025852D8,thunderDelete);
u32 thunderCreateHeap(u32 p){
 WWHD_FUNC(0x02585334,u32,p);if(U(p,0xFC))return 1;u32 parent=U(0x1047C8B4u),heap=0;
 if(parent){heap=call<u32>(0x025E35BC,0x3804u,parent,0x20u);store<u32>(p+0xFC,heap);}
 if(!heap){parent=call<u32>(0x025E2F64,0u);heap=call<u32>(0x025E35BC,0x3804u,parent,0x20u);store<u32>(p+0xFC,heap);
  if(!heap){parent=call<u32>(0x025E2F64,1u);heap=call<u32>(0x025E35BC,0x3804u,parent,0x20u);store<u32>(p+0xFC,heap);if(!heap)return 0;}}
 store<u32>(heap+16,0x1005048Cu);return 1;
}
VERIFY(0x02585334,thunderCreateHeap);
void thunderAdjustHeap(u32 p){WWHD_FUNC(0x02585400,void,p);call<void>(0x025E37D8);s32 size=call<s32>(0x025E3678,U(p,0xFC));if(size>=0){u32 heap=U(p,0xFC),vt=U(heap,12);u32 a=call_ptr<u32>(U(vt,0x6C),heap),b=call_ptr<u32>(U(vt,0x5C),heap);call<void>(0xC00088B8u,b,a);}}
VERIFY(0x02585400,thunderAdjustHeap);
u32 thunderCreate(u32 p){
 WWHD_FUNC(0x02585478,u32,p);if(!call<u32>(0x02585334,p))return 5;
 u32 weather=call<u32>(0x02555D0C)+0xAB0,g=call<u32>(0x025200D4),camera=U(g,0x5AF8);
 if(p){call<void>(0x025DD5F0,p);store<u32>(p+0xB4,0x1005042Cu);call<void>(0x025E895C,p+0x104);call<void>(0x025E7C6C,p+0x10C);call<void>(0x025E80D0,p+0x180);}
 Local<ThunderRes> modelName;modelName->b=0x10050414u;modelName->a=0x1005040Cu;
 u32 data=call<u32>(0x026066C4,U(0x101F4F28u),modelName.get(),0x3Eu);if(!data)call<void>(0x0273AA24,0x10050448u,0x8Bu,0x1005045Cu);
 u32 model=call<u32>(0x025E38E0,data,0x80000u,0x01000200u);store<u32>(p+0x100,model);
 if(!model || !call<u32>(0x025E8A48,p+0x104,model)){call<void>(0x02585400,p);return 5;}
 Local<ThunderRes> btkName;btkName->b=0x10050414u;btkName->a=0x1005040Cu;
 u32 anim=call<u32>(0x026066C4,U(0x101F4F28u),btkName.get(),0x60u);if(!anim)call<void>(0x0273AA24,0x10050448u,0x9Au,0x10050470u);
 f32 one=F(0x10050488);if(!call<u32>(0x025E7CE0,p+0x10C,data,anim,0u,2u,0u,0xFFFFFFFFu,0u,0u,one)){call<void>(0x02585400,p);return 5;}
 Local<ThunderRes> brkName;brkName->b=0x10050414u;brkName->a=0x1005040Cu;
 anim=call<u32>(0x026066C4,U(0x101F4F28u),brkName.get(),0x52u);if(!anim)call<void>(0x0273AA24,0x10050448u,0xA9u,0x1005043Cu);
 if(!call<u32>(0x025E8154,p+0x180,data,anim,1u,0u,0u,0xFFFFFFFFu,0u,0u,one)){call<void>(0x02585400,p);return 5;}
 S(p,0x22C,call<f32>(0x020198D8,one));bool close=load<u8>(weather+1)<10;f32 zero=F(0x1005047C),factor=close?one:F(0x10050494);
 S(p,0x228,zero);f32 r=call<f32>(0x02019918,zero);S(p,0x228,fmuls_ppc(r,factor));
 r=call<f32>(0x020198D8,F(0x10050498));f32 size=fmuls_ppc(fadds_ppc(F(0x1005049C),r),factor);S(p,0x1F8,size);
 r=call<f32>(0x02019918,one);f64 boundary=load<f64>(0x100504A0u);if(close?(r>=boundary):!(r<boundary))S(p,0x1F8,-F(p,0x1F8));
 S(p,0x1FC,size);S(p,0x200,one);Local<ThunderVec> dir;call<void>(0x02563F64,camera+0xDC,camera+0xE8,dir.get());u32 v=ea(dir.get());
 f32 dx=F(v),dz=F(v,8);f32 horizontal=call<f32>(0x028F4384,fmadds(dx,dx,fmuls_ppc(dz,dz)));
 s16 yaw=call<s16>(0x020195B0,F(v),F(v,8)),pitch=call<s16>(0x020195B0,F(v,4),horizontal);
 bool right=!(call<f32>(0x02019918,one)<zero);u32 yawIndex=(u16)((s32)yaw+(right?0x4000:-0x4000)),pitchIndex=(u16)pitch;
 u32 tablePitch=0x104A44F8u+(pitchIndex>>3)*8,tableYaw=0x104A44F8u+(yawIndex>>3)*8;
 f32 cs=F(tablePitch,4),sn=F(tableYaw),cy=F(tableYaw,4),sideX=fmuls_ppc(cs,sn),sideZ=fmuls_ppc(cs,cy);
 f32 spread=call<f32>(0x020198D8,F(0x100504A8)),far=F(0x100504AC);
 f32 offX=fmadds(F(v),far,fmuls_ppc(sideX,spread));S(p,0x210,offX);
 f32 offY=call<f32>(0x02019918,F(0x100504B0));f32 offZ=fmadds(F(v,8),far,fmuls_ppc(sideZ,spread)),currentX=F(p,0x210);
 S(p,0x214,offY);S(p,0x218,offZ);S(p,0x204,fadds_ppc(F(camera,0xDC),currentX));S(p,0x208,fadds_ppc(F(camera,0xE0),offY));S(p,0x20C,fadds_ppc(F(camera,0xE4),offZ));
 r=call<f32>(0x020198D8,one);f32 chance=F(0x100504B4);if(right?(r<chance):!(r>=chance)){f32 x=F(p,0x204),y=F(p,0x208),z=F(p,0x20C);S(p,0x21C,-x);S(p,0x220,-y);S(p,0x224,-z);call<void>(0x025E19CC,0x69F6u,p+0x21C);}
 call<void>(0x02585400,p);return 4;
}
VERIFY(0x02585478,thunderCreate);
void thunderStaticInit(){WWHD_FUNC(0x02585A0C,void);store<u32>(0x104775D0u,0);store<u32>(0x104775C8u,0);store<u32>(0x104775D4u,0);store<u32>(0x104775CCu,0);call<void>(0x028F026C,0x101E9A84u);f32 a=F(0x100504B8),b=F(0x100504BC);S(0x104775BC,0,a);S(0x104775C0,0,b);call<void>(0x028ED6F8,0x104775C4u);call<void>(0x028F026C,0x101E9A90u);call<void>(0x028EAB2C,0x104775C5u);call<void>(0x028F026C,0x101E9A9Cu);}
VERIFY(0x02585A0C,thunderStaticInit);
u32 thunderIsDelete(u32 p){WWHD_FUNC(0x02585AA0,u32,p);return 1;}
VERIFY(0x02585AA0,thunderIsDelete);
void thunderEmptyDtor(u32 p,u32 flags){WWHD_FUNC(0x02585AA8,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x02585AA8,thunderEmptyDtor);
void thunderEmpty(u32 p){WWHD_FUNC(0x02585ABC,void,p);}
VERIFY(0x02585ABC,thunderEmpty);
