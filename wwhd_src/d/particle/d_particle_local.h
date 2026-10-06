#pragma once
#include "wwhd.h"
#include "gabi.h"
#include <cmath>
namespace pa {
inline void* p(u32 a){return gabi::at<void>(a);}
inline u32 w(u32 a){return gabi::load<u32>(a);}
inline u8 b(u32 a){return gabi::load<u8>(a);}
inline u16 h(u32 a){return gabi::load<u16>(a);}
inline s16 sh(u32 a){return gabi::load<s16>(a);}
inline f32 f(u32 a){return gabi::load<f32>(a);}
inline void W(u32 a,u32 v){gabi::store<u32>(a,v);}
inline void B(u32 a,u8 v){gabi::store<u8>(a,v);}
inline void H(u32 a,u16 v){gabi::store<u16>(a,v);}
inline void F(u32 a,f32 v){gabi::store<f32>(a,v);}
inline void copy(u32 s,u32 d,u32 n){u8 bytes[64];for(u32 i=0;i<n;i++)bytes[i]=b(s+i);for(u32 i=0;i<n;i++)B(d+i,bytes[i]);}
struct Vec {u8 bytes[12];};
struct Color {u8 bytes[4];};
struct Color16 {u8 bytes[8];};
struct ColorF {u8 bytes[16];};
inline void* allocate(void* obj,u32 size){return obj?obj:gabi::call<void*>(0x0273AD10,size);}
inline void destroy(void* obj,u32 flag){if(obj&&(flag&1))gabi::call<void>(0x0273AF40,obj);}
}
