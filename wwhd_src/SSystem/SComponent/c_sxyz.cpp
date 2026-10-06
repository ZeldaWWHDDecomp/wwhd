#include "SSystem/SComponent/c_xyz.h"
using gabi::load; using gabi::store;
// Explicit six-byte aggregate return ABI: r3 contains x/y, r4 high half z.
struct SxyzResult { u32 xy; u16 z; };
namespace gabi {
template<> constexpr RetKind ret_kind<SxyzResult>() { return RET_AGG6; }
template<> inline void store_ret<SxyzResult>(Cpu* c,SxyzResult value) { c->r[3]=value.xy; c->r[4]=u32(value.z)<<16; }
template<> inline SxyzResult result<SxyzResult>(Cpu* c) { return {c->r[3],u16(c->r[4]>>16)}; }
}
void* sxyz_construct(void* object,s16 x,s16 y,s16 z) {
 WWHD_FUNC(0x0201A478,void*,object,x,y,z);
 if(!gabi::ea(object)) object=gabi::call<void*>(0x0273AD10,6);
 if(gabi::ea(object)) {
  store<s16>(gabi::ea(object)+2,y);
  store<s16>(gabi::ea(object),x);
  store<s16>(gabi::ea(object)+4,z);
 }
 return object;
}
VERIFY(0x0201A478,sxyz_construct);
SxyzResult sxyz_add(void* left,void* right) {
 WWHD_FUNC(0x0201A4DC,SxyzResult,left,right);
 s16 x=s16(s32(load<s16>(gabi::ea(left)))+load<s16>(gabi::ea(right)));
 s16 z=s16(s32(load<s16>(gabi::ea(left)+4))+load<s16>(gabi::ea(right)+4));
 s16 y=s16(s32(load<s16>(gabi::ea(left)+2))+load<s16>(gabi::ea(right)+2));
 gabi::Local<csXyz> temporary;
 void* result=sxyz_construct(temporary.get(),x,y,z);
 u32 a=gabi::ea(result);
 return {u32(load<u16>(a))<<16 | load<u16>(a+2),load<u16>(a+4)};
}
VERIFY(0x0201A4DC,sxyz_add);
void sxyz_add_assign(void* left,void* right) {
 WWHD_FUNC(0x0201A554,void,left,right);
 u32 a=gabi::ea(left),b=gabi::ea(right);
 s32 x=load<s16>(a)+load<s16>(b);
 s32 y=load<s16>(a+2);
 store<s16>(a,s16(x));
 y+=load<s16>(b+2);
 s32 z=load<s16>(a+4);
 store<s16>(a+2,s16(y));
 z+=load<s16>(b+4);
 store<s16>(a+4,s16(z));
}
VERIFY(0x0201A554,sxyz_add_assign);
SxyzResult sxyz_scale(void* object,f32 scale) {
 WWHD_FUNC(0x0201A588,SxyzResult,object,scale);
 s16 x=s16(gabi::ftoi(f32(load<s16>(gabi::ea(object)))*scale));
 s16 y=s16(gabi::ftoi(f32(load<s16>(gabi::ea(object)+2))*scale));
 s16 z=s16(gabi::ftoi(f32(load<s16>(gabi::ea(object)+4))*scale));
 gabi::Local<csXyz> temporary;
 void* result=sxyz_construct(temporary.get(),x,y,z);
 u32 a=gabi::ea(result);
 return {u32(load<u16>(a))<<16 | load<u16>(a+2),load<u16>(a+4)};
}
VERIFY(0x0201A588,sxyz_scale);
// HD math/header globals and their registration records.
void sxyz_static_init() {
 WWHD_FUNC(0x0201A668,void);
 store<u32>(0x101FFB0C,0);
 store<u32>(0x101FFB04,0);
 store<u32>(0x101FFB10,0);
 store<u32>(0x101FFB08,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3AC));
 f32 first=load<f32>(0x100036BC);
 f32 second=load<f32>(0x100036C0);
 store<f32>(0x101FFAF8,first);
 store<f32>(0x101FFAFC,second);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFB00));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3B8));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFB01));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D3C4));
 sxyz_construct(gabi::at<void>(0x101FFB14),0,0,0);
}
VERIFY(0x0201A668,sxyz_static_init);
