#include "gabi.h"
using namespace gabi;
namespace f_op_draw_iter {
void *get_tag() {
 WWHD_FUNC(0x025DA758,void *);
 u32 index=load<u32>(0x101F3390)+1;
 s32 count=load<s32>(0x101F33E0);
 if(s32(index)>=count)return nullptr;
 u32 lists=load<u32>(0x101F33DC);
 do {
  store<u32>(0x101F3390,index);
  u32 tag=load<u32>(lists+index*12);
  if(tag)return at<void>(tag);
  ++index;
 }while(s32(index)<count);
 return nullptr;
}
VERIFY(0x025DA758,get_tag);
void *begin() {
 WWHD_FUNC(0x025DA7AC,void *);
 u32 lists=load<u32>(0x101F33DC);
 u32 head=load<u32>(lists);
 store<u32>(0x101F3390,0);
 return head?at<void>(head):call<void *>(0x025DA758);
}
VERIFY(0x025DA7AC,begin);
void *next(void *tag) {
 WWHD_FUNC(0x025DA7D0,void *,tag);
 u32 next_tag=load<u32>(ea(tag)+8);
 return next_tag?at<void>(next_tag):call<void *>(0x025DA758);
}
VERIFY(0x025DA7D0,next);
void static_init() {
  WWHD_FUNC(0x025DA7E0,void);
  store<u32>(0x10487588,0);store<u32>(0x10487580,0);store<u32>(0x1048758C,0);store<u32>(0x10487584,0);
  call<void>(0x028F026C,at<void>(0x101F3394));
  f32 first=load<f32>(0x1005797C),second=load<f32>(0x10057980);
  store<f32>(0x10487574,first);store<f32>(0x10487578,second);
  call<void>(0x028ED6F8,at<void>(0x1048757C));call<void>(0x028F026C,at<void>(0x101F33A0));
  call<void>(0x028EAB2C,at<void>(0x1048757D));call<void>(0x028F026C,at<void>(0x101F33AC));
}
VERIFY(0x025DA7E0,static_init);
}
