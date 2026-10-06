#include "wwhd.h"
#include "gabi.h"
// HD line_tag:20-byte create_tag base and signed list ID at+14; size18.
void fpcLnTg_QueueTo(void* tag) {
 WWHD_FUNC(0x025DF694,void,tag);
 gabi::call<s32>(0x0201A724,tag);
 gabi::store<u32>(gabi::ea(tag)+0x14,0xFFFFFFFF);
}
VERIFY(0x025DF694,fpcLnTg_QueueTo);
s32 fpcLnTg_ToQueue(void* tag,s32 listID) {
 WWHD_FUNC(0x025DF6C8,s32,tag,listID);
 s32 result=gabi::call<s32>(0x0201A770,gabi::at<void>(0x101F3C14),listID,tag);
 if(result) {gabi::store<s32>(gabi::ea(tag)+0x14,listID);return 1;}
 return 0;
}
VERIFY(0x025DF6C8,fpcLnTg_ToQueue);
s32 fpcLnTg_Move(void* tag,s32 listID) {
 WWHD_FUNC(0x025DF738,s32,tag,listID);
 if(gabi::load<u32>(gabi::ea(tag)+0x14)!=u32(listID)) {
  fpcLnTg_QueueTo(tag);
  return fpcLnTg_ToQueue(tag,listID);
 }
 return 1;
}
VERIFY(0x025DF738,fpcLnTg_Move);
void fpcLnTg_Init(void* tag,u32 data) {
 WWHD_FUNC(0x025DF7A8,void,tag,data);
 gabi::call<void>(0x0201A918,tag,gabi::at<void>(data));
 gabi::store<u32>(gabi::ea(tag)+0x14,0xFFFFFFFF);
}
VERIFY(0x025DF7A8,fpcLnTg_Init);
void f_pc_line_tag_static_init() {
 WWHD_FUNC(0x025DF7DC,void);
 gabi::store<u32>(0x1048A96C,0);gabi::store<u32>(0x1048A964,0);gabi::store<u32>(0x1048A970,0);gabi::store<u32>(0x1048A968,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C40));
 f32 negativePi=gabi::load<f32>(0x1005836C),positivePi=gabi::load<f32>(0x10058370);
 gabi::store<f32>(0x1048A958,negativePi);gabi::store<f32>(0x1048A95C,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A960));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C4C));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A961));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3C58));
}
VERIFY(0x025DF7DC,f_pc_line_tag_static_init);
