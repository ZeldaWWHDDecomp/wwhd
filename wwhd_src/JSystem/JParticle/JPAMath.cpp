// JPA matrix decomposition, adapted from tww JPAMath.cpp for Cafe arithmetic.
#include "JSystem/JParticle/JPAMath.h"

f64 JPAMath_sqrt(f64);
void JPAMathIdentity(Mtx34*);
void JPAMathRegister(void*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegistration;
}

/* 028247E4 */
void JPASetSVecfromMtx(const Mtx34* mtx, cXyz& scale) {
    // HD: each column uses GHS's fused operation order before the sqrt call.
    f32 m10 = mtx->m[1][0];
    f32 m00 = mtx->m[0][0];
    f32 square = gabi::fmadds(m00, m00, gabi::fmuls_ppc(m10, m10));
    f32 m20 = mtx->m[2][0];
    scale.x = f32(JPAMath_sqrt(gabi::fmadds(m20, m20, square)));
    f32 m11 = mtx->m[1][1];
    f32 m01 = mtx->m[0][1];
    square = gabi::fmadds(m01, m01, gabi::fmuls_ppc(m11, m11));
    f32 m21 = mtx->m[2][1];
    scale.y = f32(JPAMath_sqrt(gabi::fmadds(m21, m21, square)));
    f32 m12 = mtx->m[1][2];
    f32 m02 = mtx->m[0][2];
    square = gabi::fmadds(m02, m02, gabi::fmuls_ppc(m12, m12));
    f32 m22 = mtx->m[2][2];
    scale.z = f32(JPAMath_sqrt(gabi::fmadds(m22, m22, square)));
}

/* 02824890 */
void JPASetRMtxSTVecfromMtx(const Mtx34* src, Mtx34* dst, cXyz& scale, cXyz& translate) {
    JPASetSVecfromMtx(src, scale);
    JPAMathIdentity(dst);
    f32 sx = scale.x;
    if (sx != 0.0f) {
        f32 inv = 1.0f / sx;
        dst->m[0][0] = gabi::fmuls_ppc(src->m[0][0], inv);
        dst->m[1][0] = gabi::fmuls_ppc(src->m[1][0], inv);
        dst->m[2][0] = gabi::fmuls_ppc(src->m[2][0], inv);
    }
    f32 sy = scale.y;
    if (sy != 0.0f) {
        f32 inv = 1.0f / sy;
        dst->m[0][1] = gabi::fmuls_ppc(src->m[0][1], inv);
        dst->m[1][1] = gabi::fmuls_ppc(src->m[1][1], inv);
        dst->m[2][1] = gabi::fmuls_ppc(src->m[2][1], inv);
    }
    f32 sz = scale.z;
    if (sz != 0.0f) {
        f32 inv = 1.0f / sz;
        dst->m[0][2] = gabi::fmuls_ppc(src->m[0][2], inv);
        dst->m[1][2] = gabi::fmuls_ppc(src->m[1][2], inv);
        dst->m[2][2] = gabi::fmuls_ppc(src->m[2][2], inv);
    }
    // HD: cache all three source words before the potentially overlapping stores.
    auto words = reinterpret_cast<const JPAMathMatrixWords*>(src);
    u32 ty = words->m[1][3], tx = words->m[0][3], tz = words->m[2][3];
    auto out = reinterpret_cast<JPAMathVectorWords*>(&translate);
    out->x = tx; out->y = ty; out->z = tz;
}

/* 028249B0 */
void JPASetRMtxTVecfromMtx(const Mtx34* src, Mtx34* dst, cXyz& translate) {
    cXyz scale;
    JPASetRMtxSTVecfromMtx(src, dst, scale, translate);
}

// HD: compiler-generated initialization of this translation unit.
/* 02824AF0 */
void __sinit_JPAMath_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JPAMathRegister(&sStaticRegistration);
}

/* WWHD glue
ext 028F37F0 f64 JPAMath_sqrt(f64)
ext 028E9098 void JPAMathIdentity(Mtx34*)
ext 028F026C void JPAMathRegister(void*)
data sStaticBlock 104B5970
data sStaticRegistration 101FA07C
*/
