#include "wwhd.h"
#include "gabi.h"
struct LinVector { be<f32> x,y,z; };
WWHD_SIZE(LinVector,12);
void cBgS_LinChk_ct(void* object) {
 WWHD_FUNC(0x02008F64,void,object);
 gabi::Local<LinVector> zero;
 f32 x=gabi::load<f32>(0x101FFBA8),y=gabi::load<f32>(0x101FFBAC);
 zero->x=x;
 f32 z=gabi::load<f32>(0x101FFBB0);
 zero->y=y;zero->z=z;
 u32 a=gabi::ea(object);
 gabi::call<void>(0x02018808,gabi::at<void>(a+0x24),zero.get(),zero.get());
 u32 endZ=gabi::load<u32>(gabi::ea(zero.get())+8),endX=gabi::load<u32>(gabi::ea(zero.get()));
 gabi::store<u32>(a+0x4C,0);gabi::store<u8>(a+0x54,0);gabi::store<u32>(a+8,0xFFFFFFFF);
 gabi::store<u32>(a+0x40,endX);gabi::store<u8>(a+0x53,1);
 u32 endY=gabi::load<u32>(gabi::ea(zero.get())+4);
 gabi::store<u32>(a+0x48,endZ);gabi::store<u32>(a+0x44,endY);
}
VERIFY(0x02008F64,cBgS_LinChk_ct);
void* cBgS_LinChk_ctor(void* object) {
 WWHD_FUNC(0x02008FEC,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,0x58);
 if(object) {
  u32 a=gabi::ea(object);
  gabi::call<void*>(0x02008B60,object);
  gabi::store<u32>(a+0x18,0);gabi::store<u8>(a+0x54,0);gabi::store<u32>(a+0x4C,0);
  gabi::store<u32>(a+0x10,0x10000F64);gabi::store<u16>(a+0x14,0xFFFF);gabi::store<u16>(a+0x16,0x100);
  gabi::store<u32>(a+0x1C,0xFFFFFFFF);gabi::store<u8>(a+0x52,0);
  gabi::store<u32>(a+0x3C,0x10000F44);gabi::store<u32>(a+0x20,0x10000F74);
  gabi::store<u8>(a+0x53,0);gabi::store<u8>(a+0x51,0);gabi::store<u8>(a+0x50,0);
  cBgS_LinChk_ct(object);
 }
 return object;
}
VERIFY(0x02008FEC,cBgS_LinChk_ctor);
void cBgS_LinChk_Set2(void* object,u32 start,u32 end,u32 actor) {
 WWHD_FUNC(0x0200909C,void,object,start,end,actor);
 u32 a=gabi::ea(object);
 gabi::call<void>(0x02018808,gabi::at<void>(a+0x24),gabi::at<void>(start),gabi::at<void>(end));
 u32 x=gabi::load<u32>(end);gabi::store<u32>(a+0x40,x);
 u32 y=gabi::load<u32>(end+4),flags=gabi::load<u32>(a+0x4C);
 gabi::store<u32>(a+0x44,y);
 u32 z=gabi::load<u32>(end+8);
 gabi::store<u32>(a+8,actor);gabi::store<u32>(a+0x18,0);gabi::store<u32>(a+0x1C,0xFFFFFFFF);
 gabi::store<u32>(a+0x4C,flags&~0x10u);gabi::store<u32>(a+0x48,z);
 gabi::store<u16>(a+0x14,0xFFFF);gabi::store<u16>(a+0x16,0x100);
}
VERIFY(0x0200909C,cBgS_LinChk_Set2);
void c_bg_s_lin_chk_static_init() {
 WWHD_FUNC(0x02009130,void);
 gabi::store<u32>(0x101FF470,0);gabi::store<u32>(0x101FF468,0);gabi::store<u32>(0x101FF474,0);gabi::store<u32>(0x101FF46C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C5DC));
 f32 negativePi=gabi::load<f32>(0x10000F8C),positivePi=gabi::load<f32>(0x10000F90);
 gabi::store<f32>(0x101FF45C,negativePi);gabi::store<f32>(0x101FF460,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF464));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C5E8));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF465));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C5F4));
}
VERIFY(0x02009130,c_bg_s_lin_chk_static_init);

/* ---- hosted here: the static initializer(s) of three separate header-static-only TUs linked between c_bg_s_lin_chk and c_bg_w (probably c_bg_s_poly_info, c_bg_s_shdw_draw and one more, by link order).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_020091C4() {
 WWHD_FUNC(0x020091C4,void);
 gabi::store<u32>(0x101FF48C,0);gabi::store<u32>(0x101FF484,0);gabi::store<u32>(0x101FF490,0);gabi::store<u32>(0x101FF488,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C600));
 f32 negativePi=gabi::load<f32>(0x10000FC4),positivePi=gabi::load<f32>(0x10000FC8);
 gabi::store<f32>(0x101FF478,negativePi);gabi::store<f32>(0x101FF47C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF480));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C60C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF481));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C618));
}
VERIFY(0x020091C4,hd_static_init_020091C4);
void hd_static_init_02009258() {
 WWHD_FUNC(0x02009258,void);
 gabi::store<u32>(0x101FF4A8,0);gabi::store<u32>(0x101FF4A0,0);gabi::store<u32>(0x101FF4AC,0);gabi::store<u32>(0x101FF4A4,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C624));
 f32 negativePi=gabi::load<f32>(0x10000FEC),positivePi=gabi::load<f32>(0x10000FF0);
 gabi::store<f32>(0x101FF494,negativePi);gabi::store<f32>(0x101FF498,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF49C));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C630));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF49D));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C63C));
}
VERIFY(0x02009258,hd_static_init_02009258);
void hd_static_init_020092EC() {
 WWHD_FUNC(0x020092EC,void);
 gabi::store<u32>(0x101FF4C4,0);gabi::store<u32>(0x101FF4BC,0);gabi::store<u32>(0x101FF4C8,0);gabi::store<u32>(0x101FF4C0,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C648));
 f32 negativePi=gabi::load<f32>(0x10000FF8),positivePi=gabi::load<f32>(0x10000FFC);
 gabi::store<f32>(0x101FF4B0,negativePi);gabi::store<f32>(0x101FF4B4,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF4B8));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C654));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF4B9));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C660));
}
VERIFY(0x020092EC,hd_static_init_020092EC);
