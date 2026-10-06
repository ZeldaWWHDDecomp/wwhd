#include "gabi.h"
using namespace gabi;
namespace d_level_se {
s32 execute(void *self) {
 WWHD_FUNC(0x02586F8C,s32,self);
 u32 p=ea(self);u8 flag=load<u8>(p+0x101);u32 sound=load<u32>(p+0xF8);
 if(!(flag&8)){
  void *position=at<void>(p+0xE0);
  if(flag&1)call<void>(0x025E1A04,sound,position,load<u32>(p+0xFC));
  else if(flag&4){s32 reverb=load<s8>(p+0x100);call<void>(0x025E1A40,sound,position,load<u32>(p+0xFC),reverb);}
  else call<void>(0x025E19CC,sound,position);
 }
 return 1;
}
VERIFY(0x02586F8C,execute);
s32 destroy(void *self) {
 WWHD_FUNC(0x02587000,s32,self);
 call<void>(0x025E1B34,at<void>(ea(self)+0xE0));
 if(self)call<void>(0x025DD630,self,u32(0));
 return 1;
}
VERIFY(0x02587000,destroy);
s32 create(void *self) {
 WWHD_FUNC(0x02587048,s32,self);
 if(self){call<void>(0x025DD5F0,self);store<u32>(ea(self)+0xB4,0x100505F4);}
 return 4;
}
VERIFY(0x02587048,create);
void static_init() {
 WWHD_FUNC(0x0258708C,void);
 store<u32>(0x10477640,0);store<u32>(0x10477638,0);store<u32>(0x10477644,0);store<u32>(0x1047763C,0);
 call<void>(0x028F026C,at<void>(0x101E9BC8));
 f32 first=load<f32>(0x1005060C),second=load<f32>(0x10050610);
 store<f32>(0x1047762C,first);store<f32>(0x10477630,second);
 call<void>(0x028ED6F8,at<void>(0x10477634));call<void>(0x028F026C,at<void>(0x101E9BD4));
 call<void>(0x028EAB2C,at<void>(0x10477635));call<void>(0x028F026C,at<void>(0x101E9BE0));
}
VERIFY(0x0258708C,static_init);
s32 is_delete(){WWHD_FUNC(0x02587120,s32);return 1;}
VERIFY(0x02587120,is_delete);
}
