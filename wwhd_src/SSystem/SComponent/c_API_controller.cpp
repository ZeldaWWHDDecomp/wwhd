#include "gabi.h"
using namespace gabi;
namespace c_API_controller {
u32 api_020075C0(s32 port) {
 WWHD_FUNC(0x020075C0,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return buttons;
}
VERIFY(0x020075C0,api_020075C0);
u32 api_020075E0(s32 port) {
 WWHD_FUNC(0x020075E0,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00010000)!=0;
}
VERIFY(0x020075E0,api_020075E0);
u32 api_0200760C(s32 port) {
 WWHD_FUNC(0x0200760C,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00020000)!=0;
}
VERIFY(0x0200760C,api_0200760C);
u32 api_02007638(s32 port) {
 WWHD_FUNC(0x02007638,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00004000)!=0;
}
VERIFY(0x02007638,api_02007638);
u32 api_02007664(s32 port) {
 WWHD_FUNC(0x02007664,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00000020)!=0;
}
VERIFY(0x02007664,api_02007664);
u32 api_02007690(s32 port) {
 WWHD_FUNC(0x02007690,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00000004)!=0;
}
VERIFY(0x02007690,api_02007690);
u32 api_020076BC(s32 port) {
 WWHD_FUNC(0x020076BC,u32,port);
 if(port)return 0;
 return load<u32>(load<u32>(0x101F5088)+0x124) & 1;
}
VERIFY(0x020076BC,api_020076BC);
u32 api_020076E0(s32 port) {
 WWHD_FUNC(0x020076E0,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00000002)!=0;
}
VERIFY(0x020076E0,api_020076E0);
u32 api_0200770C(s32 port) {
 WWHD_FUNC(0x0200770C,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00000008)!=0;
}
VERIFY(0x0200770C,api_0200770C);
u32 api_02007738(s32 port) {
 WWHD_FUNC(0x02007738,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x124);
 return (buttons & 0x00000010)!=0;
}
VERIFY(0x02007738,api_02007738);
u32 api_02007764(s32 port) {
 WWHD_FUNC(0x02007764,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00010000)!=0;
}
VERIFY(0x02007764,api_02007764);
u32 api_02007790(s32 port) {
 WWHD_FUNC(0x02007790,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00020000)!=0;
}
VERIFY(0x02007790,api_02007790);
u32 api_020077BC(s32 port) {
 WWHD_FUNC(0x020077BC,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00040000)!=0;
}
VERIFY(0x020077BC,api_020077BC);
u32 api_020077E8(s32 port) {
 WWHD_FUNC(0x020077E8,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00080000)!=0;
}
VERIFY(0x020077E8,api_020077E8);
u32 api_02007814(s32 port) {
 WWHD_FUNC(0x02007814,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00004000)!=0;
}
VERIFY(0x02007814,api_02007814);
u32 api_02007840(s32 port) {
 WWHD_FUNC(0x02007840,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000020)!=0;
}
VERIFY(0x02007840,api_02007840);
u32 api_0200786C(s32 port) {
 WWHD_FUNC(0x0200786C,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000004)!=0;
}
VERIFY(0x0200786C,api_0200786C);
u32 api_02007898(s32 port) {
 WWHD_FUNC(0x02007898,u32,port);
 if(port)return 0;
 return load<u32>(load<u32>(0x101F5088)+0x18) & 1;
}
VERIFY(0x02007898,api_02007898);
u32 api_020078BC(s32 port) {
 WWHD_FUNC(0x020078BC,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000002)!=0;
}
VERIFY(0x020078BC,api_020078BC);
u32 api_020078E8(s32 port) {
 WWHD_FUNC(0x020078E8,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000008)!=0;
}
VERIFY(0x020078E8,api_020078E8);
u32 api_02007914(s32 port) {
 WWHD_FUNC(0x02007914,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000010)!=0;
}
VERIFY(0x02007914,api_02007914);
u32 api_02007940(s32 port) {
 WWHD_FUNC(0x02007940,u32,port);
 if(port)return 0;
 u32 buttons=load<u32>(load<u32>(0x101F5088)+0x18);
 return (buttons & 0x00000800)!=0;
}
VERIFY(0x02007940,api_02007940);
f32 api_0200796C(s32 port) {
 WWHD_FUNC(0x0200796C,f32,port);
 if(port)return load<f32>(0x10000B24);
 return load<f32>(load<u32>(0x101F5088)+0x130);
}
VERIFY(0x0200796C,api_0200796C);
f32 api_02007990(s32 port) {
 WWHD_FUNC(0x02007990,f32,port);
 if(port)return load<f32>(0x10000B24);
 return load<f32>(load<u32>(0x101F5088)+0x134);
}
VERIFY(0x02007990,api_02007990);
f32 api_02007AFC(s32 port) {
 WWHD_FUNC(0x02007AFC,f32,port);
 if(port)return load<f32>(0x10000B24);
 return load<f32>(load<u32>(0x101F5088)+0x138);
}
VERIFY(0x02007AFC,api_02007AFC);
f32 api_02007B20(s32 port) {
 WWHD_FUNC(0x02007B20,f32,port);
 if(port)return load<f32>(0x10000B24);
 return load<f32>(load<u32>(0x101F5088)+0x13C);
}
VERIFY(0x02007B20,api_02007B20);
f32 api_020079B4(s32 port) {
 WWHD_FUNC(0x020079B4,f32,port);
 if(port)return load<f32>(0x10000B24);
 u32 pad=load<u32>(0x101F5088);f32 y=load<f32>(pad+0x134),x=load<f32>(pad+0x130);
 f32 squared=fmadds(x,x,fmuls_ppc(y,y));
 f32 magnitude=call<f32>(0x028F4384,squared);
 f32 bound=load<f32>(0x10000B28);
 return fsubs_ppc(bound,magnitude)>=0?magnitude:bound;
}
VERIFY(0x020079B4,api_020079B4);
s16 api_02007A1C(s32 port) {
 WWHD_FUNC(0x02007A1C,s16,port);
 if(port)return 0;
 u32 pad=load<u32>(0x101F5088);f32 zero=load<f32>(0x10000B24),y=load<f32>(pad+0x134),x=load<f32>(pad+0x130);
 f32 angle;
 if(y!=zero)angle=call<f32>(0x028F4D28,x,-y);
 else if(x>zero)angle=load<f32>(0x10000B2C);
 else {f32 again=load<f32>(pad+0x130);angle=again>=0?zero:load<f32>(0x10000B30);}
 return s16(ftoi(fmuls_ppc(angle,load<f32>(0x10000B34))));
}
VERIFY(0x02007A1C,api_02007A1C);
f32 api_02007B44(s32 port) {
 WWHD_FUNC(0x02007B44,f32,port);
 f32 zero=load<f32>(0x10000B24);if(port)return zero;
 u32 pad=load<u32>(0x101F5088);f32 y=load<f32>(pad+0x13C),x=load<f32>(pad+0x138);
 f32 squared=fmadds(x,x,fmuls_ppc(y,y));f64 estimate=zero;
 if(squared>zero){
  f32 three=load<f32>(0x10000B3C),half=load<f32>(0x10000B38);estimate=frsqrte(squared);
  f32 esq=f32(estimate*round25(estimate));f32 eh=f32(estimate*round25(half));
  f32 correction=fnmsubs(esq,squared,three);estimate=fmuls_ppc(correction,eh);
 }
 return f32(estimate*round25(squared));
}
VERIFY(0x02007B44,api_02007B44);
s16 api_02007BA0(s32 port) {
 WWHD_FUNC(0x02007BA0,s16,port);
 if(port)return 0;
 f32 y=call<f32>(0x02007B20,s32(0));f32 x=call<f32>(0x02007AFC,s32(0));
 f32 angle=call<f32>(0x028F4D28,x,-y);
 return s16(ftoi(fmuls_ppc(load<f32>(0x10000B34),angle)));
}
VERIFY(0x02007BA0,api_02007BA0);
f32 api_02007BFC(s32 port) {
 WWHD_FUNC(0x02007BFC,f32,port);
 if(!port && call<u32>(0x02007690,s32(0)))return load<f32>(0x10000B28);
 return load<f32>(0x10000B24);
}
VERIFY(0x02007BFC,api_02007BFC);
f32 api_02007C50(s32 port) {
 WWHD_FUNC(0x02007C50,f32,port);
 if(!port && call<u32>(0x02007664,s32(0)))return load<f32>(0x10000B28);
 return load<f32>(0x10000B24);
}
VERIFY(0x02007C50,api_02007C50);
u32 api_02007CA4(s32 port) {
 WWHD_FUNC(0x02007CA4,u32,port);
 if(port)return 0;
 return call<u32>(0x02007690,s32(0))!=0;
}
VERIFY(0x02007CA4,api_02007CA4);
u32 api_02007CDC(s32 port) {
 WWHD_FUNC(0x02007CDC,u32,port);
 if(port)return 0;
 return call<u32>(0x02007664,s32(0))!=0;
}
VERIFY(0x02007CDC,api_02007CDC);
u32 api_02007D14(s32 port) {
 WWHD_FUNC(0x02007D14,u32,port);
 if(port)return 0;
 return call<u32>(0x0200786C,s32(0))!=0;
}
VERIFY(0x02007D14,api_02007D14);
u32 api_02007D4C(s32 port) {
 WWHD_FUNC(0x02007D4C,u32,port);
 if(port)return 0;
 return call<u32>(0x02007840,s32(0))!=0;
}
VERIFY(0x02007D4C,api_02007D4C);
void static_init() {
  WWHD_FUNC(0x02007D84,void);
  store<u32>(0x101FF390,0);store<u32>(0x101FF388,0);store<u32>(0x101FF394,0);store<u32>(0x101FF38C,0);
  call<void>(0x028F026C,at<void>(0x1018C4B4));
  f32 first=load<f32>(0x10000B40),second=load<f32>(0x10000B44);
  store<f32>(0x101FF37C,first);store<f32>(0x101FF380,second);
  call<void>(0x028ED6F8,at<void>(0x101FF384));call<void>(0x028F026C,at<void>(0x1018C4C0));
  call<void>(0x028EAB2C,at<void>(0x101FF385));call<void>(0x028F026C,at<void>(0x1018C4CC));
}
VERIFY(0x02007D84,static_init);
}
