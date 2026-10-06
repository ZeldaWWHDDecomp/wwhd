/* Local WWHD process pause reconstruction. */
#include "wwhd.h"
s32 pause_isSet(void* process,u32 mask) {
 WWHD_FUNC(0x025E0B20,s32,process,mask);
 u32 bits=gabi::load<u8>(gabi::ea(process)+0xB);
 return (bits&mask)==mask;
}
VERIFY(0x025E0B20,pause_isSet);
s32 pause_enable(void* process,u32 mask) {
 WWHD_FUNC(0x025E0B38,s32,process,mask);
 u32 addr=gabi::ea(process);
 u32 bits=gabi::load<u8>(addr+0xB);
 u32 type=gabi::load<u32>(addr+0xB8);
 gabi::store<u8>(addr+0xB,bits|mask);
 u32 baseType=gabi::load<u32>(0x101F3D60);
 if(gabi::call<s32>(0x025DD258,baseType,type))
  gabi::call(0x025DED70,gabi::at<void>(addr+0xC0),gabi::at<void>(0x025E0B38),mask);
 return 1;
}
VERIFY(0x025E0B38,pause_enable);
s32 pause_disable(void* process,u32 mask) {
 WWHD_FUNC(0x025E0BA8,s32,process,mask);
 u32 addr=gabi::ea(process);
 u32 bits=gabi::load<u8>(addr+0xB);
 u32 type=gabi::load<u32>(addr+0xB8);
 gabi::store<u8>(addr+0xB,bits&(255u-mask));
 u32 baseType=gabi::load<u32>(0x101F3D60);
 if(gabi::call<s32>(0x025DD258,baseType,type))
  gabi::call(0x025DED70,gabi::at<void>(addr+0xC0),gabi::at<void>(0x025E0BA8),mask);
 return 1;
}
VERIFY(0x025E0BA8,pause_disable);
void pause_clear(void* process) {
 WWHD_FUNC(0x025E0C1C,void,process);
 gabi::store<u8>(gabi::ea(process)+0xB,0);
}
VERIFY(0x025E0C1C,pause_clear);
void pause_initializer() {
 WWHD_FUNC(0x025E0C28,void);
 for(u32 off:{8u,0u,12u,4u}) gabi::store<u32>(0x1048AB14+off,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101F3E48));
 f32 a=gabi::load<f32>(0x1005843C),b=gabi::load<f32>(0x10058440);
 gabi::store<f32>(0x1048AB08,a); gabi::store<f32>(0x1048AB0C,b);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1048AB10));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3E54));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1048AB11));
 gabi::call(0x028F026C,gabi::at<void>(0x101F3E60));
}
VERIFY(0x025E0C28,pause_initializer);
