#pragma once
/* d_kankyo_rain, WWHD: weather-vector and camera helpers.
 * Full TU inventory and remaining rendering/weather bodies are tracked separately.
 */
#include "bindings.h"
#include <bit>
namespace d_kankyo_rain_cpp {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline f32 lf(u32 a) { return gabi::load<f32>(a); }
static inline f64 ldbl(u32 a) { return gabi::load<f64>(a); }
static inline void sf(u32 a, f32 v) { gabi::store<f32>(a,v); }
struct Vec_l { be<f32> x,y,z; };
struct DoublePos_l { be<f64> x,y,z; };
WWHD_SIZE(Vec_l,12);
WWHD_SIZE(DoublePos_l,24);


static f32 subtract_fpr(f32 a,f32 b) {
    volatile f64 left=(f64)a,right=(f64)b;
    return (f32)(left-right);
}




static inline u32 environment() { return gabi::call<u32>(0x02555D0C); }
static inline u32 gameInfo() { return gabi::call<u32>(0x025200D4); }
static inline u32 windPacket() { return ld(environment()+0xAAC); }

struct ProjectParams_l { u8 bytes[40]; };
WWHD_SIZE(ProjectParams_l,40);

struct SafeString_l { be<u32> text,vtable; };
WWHD_SIZE(SafeString_l,8);
static inline u32 rainPacket() { return ld(environment()+0xA44); }

struct MassCollisionResult_l { u8 bytes[20]; };
WWHD_SIZE(MassCollisionResult_l,20);


static inline u32 poisonPacket() { return ld(environment()+0xA6C); }

struct SnapEntry_l { u8 bytes[52]; };
WWHD_SIZE(SnapEntry_l,52);


struct GroundCheck_l { u8 bytes[84]; };
struct RoofCheck_l { u8 bytes[80]; };
WWHD_SIZE(GroundCheck_l,84);
WWHD_SIZE(RoofCheck_l,80);
static f32 forward_fmadds(f32 a,f32 c,f32 b) {
    // Native FMA adds the camera operand after the product; retain its NaN payload.
    if(b!=b) return gabi::ppc_qnan(b);
    if(a!=a) return gabi::ppc_qnan(a);
    if(c!=c) return gabi::ppc_qnan(c);
    return gabi::fmadds(a,c,b);
}

static bool equal_terminated_strings(u32 left,u32 right) {
    if(left==right) return true;
    for(u32 i=0;i<0x40001;i++) {
        u8 a=gabi::load<u8>(left+i),b=gabi::load<u8>(right+i);
        if(a!=b) return false;
        if(!a) return true;
    }
    return false;
}
static void safe_string_callback(SafeString_l* string) {
    gabi::call_ptr(ld((u32)string->vtable+0x14),string);
}
static inline u32 snowPacket() { return ld(environment()+0xA50); }

static void thunder_color(u32 target,u32 thunder,u32 scale,u32 red=90,u32 green=160,u32 blue=245) {
    gabi::call(target,red,green,blue,gabi::fmuls_ppc(lf(thunder+8),lf(scale)));
}

struct Matrix_l { be<f32> cells[12]; };
struct ColorF_l { be<f32> channels[4]; };
struct Quad_l { Vec_l vertices[4]; };
WWHD_SIZE(Matrix_l,48);
WWHD_SIZE(ColorF_l,16);
WWHD_SIZE(Quad_l,48);




static f32 wave_multiply_double(f32 a,f64 c) {
    u64 bits=std::bit_cast<u64>(c);
    bits=(bits&0xFFFFFFFFF8000000ull)+(bits&0x08000000ull);
    return (f32)((f64)a*std::bit_cast<f64>(bits));
}





}
