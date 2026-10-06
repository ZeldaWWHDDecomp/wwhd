#include "gabi.h"
#include <cmath>
using namespace gabi;

s16 cM_rad2s_hd(f32 x) {
    WWHD_FUNC(0x02019510, s16, x);
    f32 y = call<f32>(0x028F4034, x, load<f32>(0x100035B8));
    s32 n = ftoi(y * load<f32>(0x100035C4));
    if (n < -32768) n = (s32)((u32)n + 65536);
    else if (n > 32767) n = (s32)((u32)n - 65536);
    return (s16)n;
}
VERIFY(0x02019510, cM_rad2s_hd);

u16 U_GetAtanTable_hd(f32 y, f32 x) {
    WWHD_FUNC(0x02019578, u16, y, x);
    s32 i = ftoi((y / x) * load<f32>(0x100035C8));
    return load<u16>(0x1018CA40u + (u32)i * 2u);
}
VERIFY(0x02019578, U_GetAtanTable_hd);

s16 cM_atan2s_hd(f32 y, f32 x) {
    WWHD_FUNC(0x020195B0, s16, y, x);
    f32 epsilon = load<f32>(0x100030B8);
    f32 zero = load<f32>(0x100035D4);
    if (std::fabs(y) < epsilon) return x < zero ? (s16)0x8000 : 0;
    if (std::fabs(x) < epsilon) return y < zero ? (s16)0xC000 : 0x4000;
    u32 a;
    if (!(y < zero)) {
        if (!(x < zero)) {
            if (!(x < y)) a = U_GetAtanTable_hd(y, x);
            else a = 0x4000u - U_GetAtanTable_hd(x, y);
        } else {
            if (-x < y) a = U_GetAtanTable_hd(-x, y) + 0x4000u;
            else a = 0x8000u - U_GetAtanTable_hd(y, -x);
        }
    } else if (x < zero) {
        if (!(-x < -y)) a = U_GetAtanTable_hd(-y, -x) + 0x8000u;
        else a = 0xC000u - U_GetAtanTable_hd(-x, -y);
    } else {
        if (x < -y) a = U_GetAtanTable_hd(x, -y) + 0xC000u;
        else a = 0u - U_GetAtanTable_hd(-y, x);
    }
    return (s16)a;
}
VERIFY(0x020195B0, cM_atan2s_hd);

f32 cM_atan2f_hd(f32 y, f32 x) {
    WWHD_FUNC(0x0201971C, f32, y, x);
    return (f32)cM_atan2s_hd(y, x) * load<f32>(0x100035E0);
}
VERIFY(0x0201971C, cM_atan2f_hd);

void cM_initRnd_hd(s32 a, s32 b, s32 c) {
    WWHD_FUNC(0x0201976C, void, a, b, c);
    store<s32>(0x101FF9DC, c);
    store<s32>(0x101FF9D4, a);
    store<s32>(0x101FF9D8, b);
}
VERIFY(0x0201976C, cM_initRnd_hd);

f32 cM_rnd_hd() {
    WWHD_FUNC(0x02019788, f32);
    s32 a = (s32)((u32)load<s32>(0x101FF9D4) * 171u) % 30269;
    s32 b = (s32)((u32)load<s32>(0x101FF9D8) * 172u) % 30307;
    s32 c = (s32)((u32)load<s32>(0x101FF9DC) * 170u) % 30323;
    f32 x = (f32)a / load<f32>(0x100035E4);
    f32 y = (f32)b / load<f32>(0x100035E8);
    f32 z = (f32)c / load<f32>(0x100035EC);
    store<s32>(0x101FF9D4, a);
    store<s32>(0x101FF9DC, c);
    store<s32>(0x101FF9D8, b);
    return std::fabs(call<f32>(0x028F4034, (x+y)+z, load<f32>(0x100035F0)));
}
VERIFY(0x02019788, cM_rnd_hd);

f32 cM_rndF_hd(f32 scale) {
    WWHD_FUNC(0x020198D8, f32, scale);
    return fmuls_ppc(cM_rnd_hd(), scale);
}
VERIFY(0x020198D8, cM_rndF_hd);

f32 cM_rndFX_hd(f32 scale) {
    WWHD_FUNC(0x02019918, f32, scale);
    f32 r = cM_rnd_hd();
    f32 v = fmuls_ppc(fsubs_ppc(r, load<f32>(0x100035F4)), scale);
    return fadds_ppc(v, v);
}
VERIFY(0x02019918, cM_rndFX_hd);

void c_math_sinit_hd() {
    WWHD_FUNC(0x02019968, void);
    store<u32>(0x101FF9F4, 0);
    store<u32>(0x101FF9EC, 0);
    store<u32>(0x101FF9F8, 0);
    store<u32>(0x101FF9F0, 0);
    call<void>(0x028F026C, at<void>(0x1018D244));
    f32 a = load<f32>(0x100035BC);
    f32 b = load<f32>(0x100035C0);
    store<f32>(0x101FF9E0, a);
    store<f32>(0x101FF9E4, b);
    call<void>(0x028ED6F8, at<void>(0x101FF9E8));
    call<void>(0x028F026C, at<void>(0x1018D250));
    call<void>(0x028EAB2C, at<void>(0x101FF9E9));
    call<void>(0x028F026C, at<void>(0x1018D25C));
}
VERIFY(0x02019968, c_math_sinit_hd);

/* ---- hosted here: the static initializer(s) of a separate header-static-only TU linked between c_math and c_node (name unknown).
 * Each only initializes the shared header statics (zeroed 16-byte object, -pi/pi pair, two
 * registered global objects); no code of their own. ---- */
void hd_static_init_020199FC() {
 WWHD_FUNC(0x020199FC,void);
 gabi::store<u32>(0x101FFA10,0);gabi::store<u32>(0x101FFA08,0);gabi::store<u32>(0x101FFA14,0);gabi::store<u32>(0x101FFA0C,0);
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D268));
 f32 negativePi=gabi::load<f32>(0x10003600),positivePi=gabi::load<f32>(0x10003604);
 gabi::store<f32>(0x101FF9FC,negativePi);gabi::store<f32>(0x101FFA00,positivePi);
 gabi::call<void>(0x028ED6F8,gabi::at<void>(0x101FFA04));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D274));
 gabi::call<void>(0x028EAB2C,gabi::at<void>(0x101FFA05));
 gabi::call<void>(0x028F026C,gabi::at<void>(0x1018D280));
}
VERIFY(0x020199FC,hd_static_init_020199FC);
