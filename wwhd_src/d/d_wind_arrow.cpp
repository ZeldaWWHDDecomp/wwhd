// HD wind arrow.
#include "gabi.h"
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 F(u32 p,u32 n=0){return load<f32>(p+n);}
static void S(u32 p,u32 n,f32 x){store<f32>(p+n,x);}
struct WindMtx { u8 bytes[48]; };
struct WindVec { u8 bytes[12]; };
struct WindResName { be<u32> a,b; };

u32 windDraw(u32 p){
 WWHD_FUNC(0x025CCAE8,u32,p);
 f32 zero=F(0x10056658);
 if(F(p,0x10C)==zero)return 1;
 u32 g=call<u32>(0x025200D4);if(!(U(g,0x5CD8)&0x10000u))return 1;
 g=call<u32>(0x025200D4);if(load<u8>(g+0x5292)!=0){g=call<u32>(0x025200D4);if(!(U(g,0x5CDC)&1))return 1;}
 u32 pos=0x101FDD1Cu,scale=0x101FDD2Cu;
 if(!U(0x101FDD18)){store<u32>(0x101FDD18u,1);f32 z=F(0x100566BC),y=F(0x100566B8);S(pos,8,z);S(pos,0,zero);S(pos,4,y);}
 if(!U(0x101FDD28)){f32 s=F(0x100566C0);store<u32>(0x101FDD28u,1);S(scale,0,s);S(scale,8,s);S(scale,4,s);}
 u32 actor=U(p,0xF8),mtx=0x1048D0CCu;Local<WindMtx> base,rot;Local<WindVec> point;
 call<void>(0x028E93CC,mtx,F(actor,0x314),F(actor,0x318),F(actor,0x31C));
 call<void>(0x025F1C28,mtx,load<s16>(actor+0x32A));call<void>(0x028E90D4,mtx,base.get());
 u32 wind=call<u32>(0x0257DAA8);s16 angle=call<s16>(0x020195B0,F(wind),F(wind,8));
 call<void>(0x025F1884,mtx,angle);call<void>(0x028E90D4,mtx,rot.get());
 u32 model=U(p,0x100);f32 sx=F(scale),sz=F(scale,8),sy=F(scale,4);S(model,0xC4,sz);S(model,0xBC,sx);S(model,0xC0,sy);
 call<void>(0x028E8F64,base.get(),pos,point.get());
 u32 r=ea(rot.get()),v=ea(point.get());f32 x=F(v),y=F(v,4),z=F(v,8);S(r,12,x);S(r,28,y);S(r,44,z);
 f32 a=F(r),b=F(r,4),c=F(r,8),d=F(r,16),e=F(r,20),f=F(r,24),h=F(r,32),i=F(r,36),j=F(r,40);
 model=U(p,0x100);S(model,0xEC,i);S(model,0xF0,j);S(model,0xD0,c);S(model,0xCC,b);S(model,0xE8,h);S(model,0xDC,e);S(model,0xD8,d);S(model,0xD4,x);S(model,0xE0,f);S(model,0xC8,a);S(model,0xE4,y);S(model,0xF4,z);
 model=U(p,0x100);call<void>(0x025E7FC4,p+0x10C,U(model,0xAC),F(p,0x110));
 u8 special=load<u8>(0x101F4829u);g=call<u32>(0x025200D4);u32 globals=0x104B4634u;
 store<u32>(globals,U(g,special?0x5D58:0x5D84));
 g=call<u32>(0x025200D4);store<u32>(globals+4,U(g,special?0x5D60:0x5D88));
 call<void>(0x025E2DE0,U(p,0x100),0u);
 g=call<u32>(0x025200D4);store<u32>(globals,U(g,0x5D78));g=call<u32>(0x025200D4);store<u32>(globals+4,U(g,0x5D7C));
 call<void>(0x025E8CD8,p+0x104);store<u32>(U(U(p,0x100),0xAC)+0x44,0);return 1;
}
VERIFY(0x025CCAE8,windDraw);
u32 windExecute(u32 p){WWHD_FUNC(0x025CCDA0,u32,p);u32 w=call<u32>(0x0257DB04);S(p,0x10C,F(w));call<void>(0x025E742C,p+0x10C);return 1;}
VERIFY(0x025CCDA0,windExecute);
void windPartsDtor(u32 p,u32 flags){WWHD_FUNC(0x025CCDE0,void,p,flags);if(p){call<void>(0x025E89F8,p+4,2u);if(flags&1)call<void>(0x0273AF40,p);}}
VERIFY(0x025CCDE0,windPartsDtor);
void windDtor(u32 p,u32 flags){WWHD_FUNC(0x025CCE34,void,p,flags);if(p){call<void>(0x025E899C,p+0x104);call<void>(0x025E3868,U(p,0xFC));call<void>(0x025CCDE0,p+0x100,2u);call<void>(0x025DD630,p,0u);if(flags&1)call<void>(0x0273AF40,p);}}
VERIFY(0x025CCE34,windDtor);
u32 windDelete(u32 p){WWHD_FUNC(0x025CCEB0,u32,p);call<void>(0x025CCE34,p,2u);return 1;}
VERIFY(0x025CCEB0,windDelete);
u32 windPartsCtor(u32 p){WWHD_FUNC(0x025CCED8,u32,p);if(!p){p=call<u32>(0x0273AD10,0x80u);if(!p)return 0;}store<u32>(p,0);call<void>(0x025E895C,p+4);call<void>(0x025E7C6C,p+12);return p;}
VERIFY(0x025CCED8,windPartsCtor);
u32 windCtor(u32 p){WWHD_FUNC(0x025CCF30,u32,p);if(!p){p=call<u32>(0x0273AD10,0x180u);if(!p)return 0;}call<void>(0x025DD5F0,p);store<u32>(p+0xB4,0x10056680u);call<void>(0x025CCED8,p+0x100);return p;}
VERIFY(0x025CCF30,windCtor);
u32 windCreateHeap(u32 p){WWHD_FUNC(0x025CCF8C,u32,p);if(!U(p,0xFC)){u32 heap=call<u32>(0x025E3630,0u,0u);store<u32>(p+0xFC,heap);if(!heap)return 0;store<u32>(heap+16,0x100566C4u);}return 1;}
VERIFY(0x025CCF8C,windCreateHeap);
void windAdjustHeap(u32 p){WWHD_FUNC(0x025CD000,void,p);call<void>(0x025E37D8);s32 size=call<s32>(0x025E3678,U(p,0xFC));if(size>=0){u32 heap=U(p,0xFC),vt=U(heap,12);u32 a=call_ptr<u32>(U(vt,0x6C),heap),b=call_ptr<u32>(U(vt,0x5C),heap);call<void>(0xC00088B8u,b,a);}}
VERIFY(0x025CD000,windAdjustHeap);
u32 windCreate(u32 p){
 WWHD_FUNC(0x025CD078,u32,p);if(!call<u32>(0x025CCF8C,p))return 5;if(p)call<void>(0x025CCF30,p);
 Local<WindResName> name;name->a=0x10056660u;name->b=0x10056668u;
 u32 data=call<u32>(0x026066C4,U(0x101F4F28u),name.get(),0x3Cu);
 if(!data)call<void>(0x0273AA24,0x10056690u,0x73u,0x100566A4u);
 u32 model=call<u32>(0x025E38E0,data,0x80000u,0x200u);store<u32>(p+0x100,model);
 if(!model || !call<u32>(0x025E8A48,p+0x104,model)){call<void>(0x025CD000,p);return 5;}
 Local<WindResName> animName;animName->b=0x10056668u;animName->a=0x10056660u;
 u32 anim=call<u32>(0x026066C4,U(0x101F4F28u),animName.get(),0x5Fu);
 u32 ok=call<u32>(0x025E7CE0,p+0x10C,data,anim,1u,2u,0u,0xFFFFFFFFu,0u,0u,F(0x100566D0));
 call<void>(0x025CD000,p);return ok?4:5;
}
VERIFY(0x025CD078,windCreate);
void windStaticInit(){WWHD_FUNC(0x025CD1B8,void);store<u32>(0x10487350u,0);store<u32>(0x10487348u,0);store<u32>(0x10487354u,0);store<u32>(0x1048734Cu,0);call<void>(0x028F026C,0x101F137Cu);f32 a=F(0x100566D8),b=F(0x100566DC);S(0x1048733C,0,a);S(0x10487340,0,b);call<void>(0x028ED6F8,0x10487344u);call<void>(0x028F026C,0x101F1388u);call<void>(0x028EAB2C,0x10487345u);call<void>(0x028F026C,0x101F1394u);}
VERIFY(0x025CD1B8,windStaticInit);
u32 windIsDelete(u32 p){WWHD_FUNC(0x025CD24C,u32,p);return 1;}
VERIFY(0x025CD24C,windIsDelete);
void windEmptyDtor(u32 p,u32 flags){WWHD_FUNC(0x025CD254,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025CD254,windEmptyDtor);
void windEmpty(u32 p){WWHD_FUNC(0x025CD268,void,p);}
VERIFY(0x025CD268,windEmpty);
