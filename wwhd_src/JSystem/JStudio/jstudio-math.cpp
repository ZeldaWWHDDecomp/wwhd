// JStudio degree-based matrix helpers, adapted from the tww JStudio reference.
#include "JSystem/JStudio/jstudio-math.h"

namespace {
constexpr f32 DEG_TO_RAD = 0.01745329238474369f;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
f64 JStudio_cos(f64);
f64 JStudio_sin(f64);
void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
void PSMTXRotRad(Mtx34*, u32, f32);
void PSMTXScale(Mtx34*, f32, f32, f32);
void PSMTXTransApply(const Mtx34*, Mtx34*, f32, f32, f32);
void JStudio_mathRegisterGlobalObject(void*);

namespace JStudio { namespace math {

/* 028399FC */
void getRotation_xyz(Mtx34* out, f32 x, f32 y, f32 z) {
    f32 radx = gabi::fmuls_ppc(x, DEG_TO_RAD);
    // HD: the trigonometric callees return double, retained in the saved FPRs.
    f64 cosx = JStudio_cos(radx);
    f64 sinx = JStudio_sin(radx);
    f32 rady = gabi::fmuls_ppc(y, DEG_TO_RAD);
    f64 cosy = JStudio_cos(rady);
    f64 siny = JStudio_sin(rady);
    f32 radz = gabi::fmuls_ppc(z, DEG_TO_RAD);
    f64 cosz = JStudio_cos(radz);
    f64 sinz = JStudio_sin(radz);
    // HD: preserve GHS single-operation rounding and fused product ordering.
    f32 cosycosz = f32(cosy * cosz);
    f32 sinxcosy = f32(sinx * cosy);
    out->m[0][0] = cosycosz;
    f32 cosxcosy = f32(cosx * cosy);
    out->m[2][1] = sinxcosy;
    f32 sinxcosz = f32(sinx * cosz);
    out->m[2][2] = cosxcosy;
    f32 sinxsinz = f32(sinx * sinz);
    f32 cosysinz = f32(cosy * sinz);
    out->m[2][0] = f32(-siny);
    f32 cosxsinz = f32(cosx * sinz);
    out->m[1][3] = 0.0f;
    f32 cosxcosz = f32(cosx * cosz);
    out->m[0][3] = 0.0f;
    f32 row01 = f32(f64(sinxcosz) * siny - f64(cosxsinz));
    out->m[1][0] = cosysinz;
    f32 row02 = f32(f64(cosxcosz) * siny + f64(sinxsinz));
    out->m[2][3] = 0.0f;
    f32 row11 = f32(f64(sinxsinz) * siny + f64(cosxcosz));
    out->m[0][1] = row01;
    f32 row12 = f32(f64(cosxsinz) * siny - f64(sinxcosz));
    out->m[0][2] = row02;
    out->m[1][1] = row11;
    out->m[1][2] = row12;
}

/* 02839B80 */
void rotate_xyz(Mtx34* out, const Mtx34* src, f32 x, f32 y, f32 z) {
    Mtx34 rotation;
    getRotation_xyz(&rotation, x, y, z);
    PSMTXConcat(&rotation, src, out);
}

/* 02839BCC */
void rotate_y(Mtx34* out, const Mtx34* src, f32 y) {
    Mtx34 rotation;
    PSMTXRotRad(&rotation, 'y', gabi::fmuls_ppc(y, DEG_TO_RAD));
    PSMTXConcat(&rotation, src, out);
}

/* 02839C28 */
void getTransformation_SRxyzT(Mtx34* out, const Vec& scale,
                                           const Vec& rotation, const Vec& translation) {
    Mtx34 scaling;
    PSMTXScale(&scaling, scale.x, scale.y, scale.z);
    Mtx34 rotated;
    rotate_xyz(&rotated, &scaling, rotation.x, rotation.y, rotation.z);
    PSMTXTransApply(&rotated, out, translation.x, translation.y, translation.z);
}

/* 02839CAC */
void getTransformation_RyT(Mtx34* out, f32 y, const Vec& translation) {
    Mtx34 rotation;
    PSMTXRotRad(&rotation, 'y', gabi::fmuls_ppc(y, DEG_TO_RAD));
    PSMTXTransApply(&rotation, out, translation.x, translation.y, translation.z);
}

}} // namespace JStudio::math
using namespace JStudio::math;

// HD: compiler-generated initialization of this translation unit.
/* 02839D10 */
static void __sinit_jstudio_math_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JStudio_mathRegisterGlobalObject(&sStaticRegister);
}

/* WWHD glue
data sStaticBlock 104B603C
data sStaticRegister 101FAD38
ext 028F4BE0 f64 JStudio_cos(f64)
ext 028F43F8 f64 JStudio_sin(f64)
ext 028E9108 void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*)
ext 028E98C0 void PSMTXRotRad(Mtx34*, u32, f32)
ext 028E945C void PSMTXScale(Mtx34*, f32, f32, f32)
ext 028E9410 void PSMTXTransApply(const Mtx34*, Mtx34*, f32, f32, f32)
ext 028F026C void JStudio_mathRegisterGlobalObject(void*)
*/
