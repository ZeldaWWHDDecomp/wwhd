#include "wwhd.h"
#include "gabi.h"

// HD base layout: pass-check pointers+0/+4, actor ID+8, flag+C, vtable+10; size14.
void cBgS_Chk_delete(void* object,s32 flags) {
 WWHD_FUNC(0x02008B4C,void,object,flags);
 if(object && (u32(flags)&1)) gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02008B4C,cBgS_Chk_delete);
void* cBgS_Chk_ctor(void* object) {
 WWHD_FUNC(0x02008B60,void*,object);
 if(!object) object=gabi::call<void*>(0x0273AD10,0x14);
 if(object) {
  u32 a=gabi::ea(object);
  gabi::store<u32>(a+8,0);gabi::store<u32>(a,0);gabi::store<u32>(a+4,0);
  gabi::store<u32>(a+0x10,0x10000E80);gabi::store<u8>(a+0xC,1);
 }
 return object;
}
VERIFY(0x02008B60,cBgS_Chk_ctor);
u8 cBgS_Chk_ChkSameActorPid(void* object,u32 pid) {
 WWHD_FUNC(0x02008BB8,u8,object,pid);
 u32 actor=gabi::load<u32>(gabi::ea(object)+8);
 if(actor==0xFFFFFFFF || pid==0xFFFFFFFF || !gabi::load<u8>(gabi::ea(object)+0xC)) return 0;
 return actor==pid;
}
VERIFY(0x02008BB8,cBgS_Chk_ChkSameActorPid);
void c_bg_s_chk_static_init() {
 WWHD_FUNC(0x02008BF0,void);
 gabi::store<u32>(0x101FF400,0);gabi::store<u32>(0x101FF3F8,0);gabi::store<u32>(0x101FF404,0);gabi::store<u32>(0x101FF3FC,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C54C));
 f32 negativePi=gabi::load<f32>(0x10000E74),positivePi=gabi::load<f32>(0x10000E78);
 gabi::store<f32>(0x101FF3EC,negativePi);gabi::store<f32>(0x101FF3F0,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF3F4));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C558));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF3F5));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C564));
}
VERIFY(0x02008BF0,c_bg_s_chk_static_init);

/* ---- hosted here: the static initializer(s) of two separate header-static-only TUs linked between c_bg_s_chk and c_bg_s_gnd_chk (names unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_02008C84() {
 WWHD_FUNC(0x02008C84,void);
 gabi::store<u32>(0x101FF41C,0);gabi::store<u32>(0x101FF414,0);gabi::store<u32>(0x101FF420,0);gabi::store<u32>(0x101FF418,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C570));
 f32 negativePi=gabi::load<f32>(0x10000EA0),positivePi=gabi::load<f32>(0x10000EA4);
 gabi::store<f32>(0x101FF408,negativePi);gabi::store<f32>(0x101FF40C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF410));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C57C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF411));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C588));
}
VERIFY(0x02008C84,hd_static_init_02008C84);
void hd_static_init_02008D18() {
 WWHD_FUNC(0x02008D18,void);
 gabi::store<u32>(0x101FF438,0);gabi::store<u32>(0x101FF430,0);gabi::store<u32>(0x101FF43C,0);gabi::store<u32>(0x101FF434,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C594));
 f32 negativePi=gabi::load<f32>(0x10000EEC),positivePi=gabi::load<f32>(0x10000EF0);
 gabi::store<f32>(0x101FF424,negativePi);gabi::store<f32>(0x101FF428,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FF42C));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C5A0));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FF42D));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018C5AC));
}
VERIFY(0x02008D18,hd_static_init_02008D18);
