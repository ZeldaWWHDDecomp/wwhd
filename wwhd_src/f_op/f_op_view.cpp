#include "gabi.h"
using gabi::load; using gabi::store;
s32 fopVw_Draw(void* view) {
 WWHD_FUNC(0x025DD050,s32,view);
 void* method=gabi::at<void>(load<u32>(gabi::ea(view)+0xC4));
 return gabi::call<s32>(0x025DF2C0,method,view);
}
VERIFY(0x025DD050,fopVw_Draw);
s32 fopVw_Execute(void* view) {
 WWHD_FUNC(0x025DD05C,s32,view);
 void* method=gabi::at<void>(load<u32>(gabi::ea(view)+0xC4));
 return gabi::call<s32>(0x025DFCC4,method,view);
}
VERIFY(0x025DD05C,fopVw_Execute);
s32 fopVw_IsDelete(void* view) {
 WWHD_FUNC(0x025DD068,s32,view);
 void* method=gabi::at<void>(load<u32>(gabi::ea(view)+0xC4));
 return gabi::call<s32>(0x025DFCCC,method,view);
}
VERIFY(0x025DD068,fopVw_IsDelete);
s32 fopVw_Delete(void* view) {
 WWHD_FUNC(0x025DD074,s32,view);
 void* method=gabi::at<void>(load<u32>(gabi::ea(view)+0xC4));
 return gabi::call<s32>(0x025DFCD4,method,view);
}
VERIFY(0x025DD074,fopVw_Delete);
s32 fopVw_Create(void* view) {
 WWHD_FUNC(0x025DD080,s32,view);
 u32 a=gabi::ea(view),profile=load<u32>(a+0x10);
 u32 method=load<u32>(profile+0x24);
 store<u32>(a+0xC4,method);
 u8 parameter=load<u8>(profile+0x28);
 store<u8>(a+0xC8,parameter);
 return gabi::call<s32>(0x025DFCDC,gabi::at<void>(method),view);
}
VERIFY(0x025DD080,fopVw_Create);
void fopVw_StaticInit() {
 WWHD_FUNC(0x025DD09C,void);
 store<u32>(0x1048A660,0);store<u32>(0x1048A658,0);
 store<u32>(0x1048A664,0);store<u32>(0x1048A65C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3888));
 f32 first=load<f32>(0x10057DC8),second=load<f32>(0x10057DCC);
 store<f32>(0x1048A64C,first);store<f32>(0x1048A650,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x1048A654));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F3894));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x1048A655));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x101F38A0));
}
VERIFY(0x025DD09C,fopVw_StaticInit);
