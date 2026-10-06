// HD HUD picture objects; matcher names are tentative.
#include "gabi.h"
using namespace gabi;
static u32 U(u32 p,u32 n=0){return load<u32>(p+n);}
static f32 F(u32 p,u32 n=0){return load<f32>(p+n);}
static void S(u32 p,u32 n,f32 v){store<f32>(p+n,v);}
struct HudName16 { u8 bytes[16]; };
static void drawPicture(u32 p,f32 x,f32 y,f32 w,f32 h){call<void>(0x027EFA58,p,0u,0u,0u,x,y,w,h);}
static void setup2D(){u32 g=call<u32>(0x025200D4),p=U(g,0x5D10),v=U(p,0xB8);call_ptr<void>(U(v,0x24),p);}

u32 hudBatteryCtor(u32 p){
 WWHD_FUNC(0x0204698C,u32,p);
 if(!p){p=call<u32>(0x0273AD10,0x5Cu);if(!p)return 0;}
 call<void>(0x0252CCBC,p);
 store<u32>(p+4,0);store<u32>(p+12,0);store<u32>(p+8,0);store<u32>(p,0x10006C80u);
 for(u32 n=0x10;n<=0x50;n+=8)call<void>(0x028F521C,p+n,8u);
 S(p,0x58,F(0x10006BC0));return p;
}
VERIFY(0x0204698C,hudBatteryCtor);

u32 hudBatteryInit(u32 p,u32 a,u32 b,u32 c,u32 d){
 WWHD_FUNC(0x02046A68,u32,p,a,b,c,d);
 u32 q=call<u32>(0x027EF308,0u,a);store<u32>(p+4,q);if(!q)return 0;
 S(p,0x38,(f32)load<u16>(a+2));S(p,0x3C,(f32)load<u16>(a+4));
 q=call<u32>(0x027EF308,0u,b);store<u32>(p+8,q);if(!q)return 0;
 S(p,0x40,(f32)load<u16>(b+2));S(p,0x44,(f32)load<u16>(b+4));
 q=call<u32>(0x027EF308,0u,c);store<u32>(p+12,q);if(!q)return 0;
 S(p,0x48,(f32)load<u16>(c+2));S(p,0x4C,(f32)load<u16>(c+4));
 for(u32 n=0;n<8;n+=4){
  q=call<u32>(0x027EF308,0u,d);store<u32>(p+0x10+n,q);if(!q)return 0;
  q=call<u32>(0x027EF484,0u,0x10006BD4u);store<u32>(p+0x18+n,q);if(!q)return 0;
  q=call<u32>(0x027EF484,0u,0x10006BD4u);store<u32>(p+0x20+n,q);if(!q)return 0;
 }
 store<u8>(U(p,8)+0xA8,200);
 f32 w=(f32)load<u16>(d+2);u32 base=U(p,4);S(p,0x50,w);
 f32 h=(f32)load<u16>(d+4),ch=F(p,0x4C),zero=F(0x10006BC0),x=F(0x10006BC4),y=F(0x10006BCC),half=F(0x10006BC8);
 S(p,0x54,h);S(p,0x2C,zero);S(p,0x28,zero);S(p,0x30,x);S(p,0x34,fmadds(ch,half,y));
 store<u8>(base+0xA8,128);store<u32>(U(p,4)+0x104,0xFFFFFFFFu);store<u32>(U(p,4)+0x108,0xFFFFFF00u);
 store<u32>(U(p,0x10)+0x104,0xFFC800FFu);
 u32 back=U(p,0x14);for(u32 n=0x10C;n<=0x118;n+=4)store<u32>(back+n,0xFFu);
 store<u8>(U(p,0x14)+0xA8,80);
 store<u32>(U(p,0x18)+0x104,0xFFC800FFu);
 back=U(p,0x1C);for(u32 n=0x10C;n<=0x118;n+=4)store<u32>(back+n,0xFFu);
 store<u8>(U(p,0x1C)+0xA8,80);
 store<u32>(U(p,0x20)+0x104,0xFFC800FFu);
 back=U(p,0x24);for(u32 n=0x10C;n<=0x118;n+=4)store<u32>(back+n,0xFFu);
 store<u8>(U(p,0x24)+0xA8,80);S(p,0x58,F(0x10006BD0));return 1;
}
VERIFY(0x02046A68,hudBatteryInit);

void hudBatteryDraw(u32 p){
 WWHD_FUNC(0x02046E70,void,p);setup2D();
 u32 q=U(p,8),vt=U(q,0xC8);f32 h=F(p,0x44),half=F(0x10006BC8),w=F(p,0x40),angle=F(p,0x58);
 S(q,0x9C,fmuls_ppc(h,half));S(q,0x98,w);S(q,0xA0,angle);store<u8>(q+0xA4,0x7A);call_ptr<void>(U(vt,0x3C),q);
 drawPicture(U(p,4),fadds_ppc(F(p,0x28),F(0x10006BE4)),fadds_ppc(F(p,0x2C),F(0x10006BE8)),F(p,0x38),F(p,0x3C));
 drawPicture(U(p,8),fadds_ppc(F(p,0x28),F(0x10006BEC)),fadds_ppc(F(p,0x2C),F(0x10006BF0)),F(p,0x40),F(p,0x44));
 drawPicture(U(p,12),fadds_ppc(F(p,0x28),F(0x10006BC4)),fadds_ppc(F(p,0x2C),F(0x10006BCC)),F(p,0x48),F(p,0x4C));
 f32 four=F(0x10006BBC);
 drawPicture(U(p,0x24),fadds_ppc(F(p,0x30),F(0x10006BF4)),fadds_ppc(F(p,0x34),four),F(p,0x50),F(p,0x54));
 w=F(p,0x50);drawPicture(U(p,0x1C),fadds_ppc(fadds_ppc(F(p,0x30),w),F(0x10006BF8)),fadds_ppc(F(p,0x34),four),w,F(p,0x54));
 w=F(p,0x50);drawPicture(U(p,0x14),fadds_ppc(fadds_ppc(F(p,0x30),fadds_ppc(w,w)),four),fadds_ppc(F(p,0x34),four),w,F(p,0x54));
 drawPicture(U(p,0x20),fadds_ppc(F(p,0x30),F(0x10006C00)),F(p,0x34),F(p,0x50),F(p,0x54));
 w=F(p,0x50);drawPicture(U(p,0x18),fadds_ppc(fadds_ppc(F(p,0x30),w),F(0x10006C04)),F(p,0x34),w,F(p,0x54));
 w=F(p,0x50);drawPicture(U(p,0x10),fadds_ppc(F(p,0x30),fadds_ppc(w,w)),F(p,0x34),w,F(p,0x54));
}
VERIFY(0x02046E70,hudBatteryDraw);

void hudBatteryRotate(u32 p,f32 angle){
 WWHD_FUNC(0x020470F4,void,p,angle);
 f32 low=F(0x10006C08),high=F(0x10006C0C);
 if(low>angle || !(angle<=high))call<void>(0x0273AA24,0x10006C3Cu,0x123u,0x10006C4Cu);
 f32 fifteen=F(0x10006C10),ratio=fsubs_ppc(angle,low)/low;
 s32 value=ftoi(fadds_ppc(fmadds(F(0x10006C14),ratio,fifteen),F(0x10006BC8)));
 s32 tens=value/10;Local<HudName16> name;u32 buf=ea(name.get());
 call<void>(0x028F17A8,buf,16u,0x10006C28u,tens);
 call<void>(0x027EFB68,U(p,0x20),buf,0u);call<void>(0x027EFB68,U(p,0x24),buf,0u);
 call<void>(0x028F17A8,buf,16u,0x10006C28u,(u32)value-(u32)tens*10u);
 call<void>(0x027EFB68,U(p,0x18),buf,0u);call<void>(0x027EFB68,U(p,0x1C),buf,0u);
 f32 twenty=F(0x10006BD0),result;
 if(!(angle>=twenty))result=F(0x10006C18);
 else if(!(angle<=high))result=F(0x10006C1C);
 else result=fsubs_ppc(F(0x10006C20),fmadds(F(0x10006C24),fsubs_ppc(angle,twenty),fifteen));
 S(p,0x58,result);
}
VERIFY(0x020470F4,hudBatteryRotate);

u32 hudObjectCtor(u32 p){
 WWHD_FUNC(0x020472CC,u32,p);
 if(!p){p=call<u32>(0x0273AD10,0x2Cu);if(!p)return 0;}
 call<void>(0x0252CCBC,p);store<u32>(p,0x10006C98u);
 call<void>(0x028F521C,p+4,8u);call<void>(0x028F521C,p+12,8u);call<void>(0x028F521C,p+20,16u);
 store<u8>(p+0x28,0);S(p,0x24,F(0x10006BC0));return p;
}
VERIFY(0x020472CC,hudObjectCtor);

u32 hudObjectInit(u32 p,u32 a,u32 b){
 WWHD_FUNC(0x02047358,u32,p,a,b);
 u32 q=call<u32>(0x027EF308,0u,a);store<u32>(p+4,q);if(!q)return 0;
 S(p,0x14,(f32)load<u16>(a+2));S(p,0x18,(f32)load<u16>(a+4));
 q=call<u32>(0x027EF308,0u,b);store<u32>(p+8,q);if(!q)return 0;
 S(p,0x1C,(f32)load<u16>(b+2));f32 h=(f32)load<u16>(b+4);
 store<u8>(p+0x28,0);S(p,0x20,h);S(p,0x24,F(0x10006C74));return 1;
}
VERIFY(0x02047358,hudObjectInit);

void hudObjectDraw(u32 p){
 WWHD_FUNC(0x0204747C,void,p);setup2D();
 u8 flag=load<u8>(p+0x28);f32 scale=F(p,0x24),y=F(p,0x10),x=F(p,12);
 u32 n=flag?0x1Cu:0x14u;f32 w=F(p,n),h=F(p,n+4);
 drawPicture(U(p,flag?8:4),x,y,fmuls_ppc(w,scale),fmuls_ppc(h,scale));
}
VERIFY(0x0204747C,hudObjectDraw);

void hudStaticInit(){
 WWHD_FUNC(0x02047520,void);
 store<u32>(0x10461358u,0);store<u32>(0x10461350u,0);store<u32>(0x1046135Cu,0);store<u32>(0x10461354u,0);
 call<void>(0x028F026C,0x1018F68Cu);f32 a=F(0x10006C78),b=F(0x10006C7C);S(0x10461344,0,a);S(0x10461348,0,b);
 call<void>(0x028ED6F8,0x1046134Cu);call<void>(0x028F026C,0x1018F698u);call<void>(0x028EAB2C,0x1046134Du);call<void>(0x028F026C,0x1018F6A4u);
}
VERIFY(0x02047520,hudStaticInit);
