// JPA1 draw construction and passes, starting from tww JPADraw.cpp.
#include "JSystem/JParticle/JPADraw.h"
using namespace JPADrawExtension;
using JPADrawVtableWord = be<u32>;
static JPADrawVtableWord sTextureNameVtable;
void* JPADrawNew(u32);
void JPADrawDelete(void*);
void JPADrawConstructArray(void*,u32,u32,u32);
/* 02830238 */
void JPADrawClipBoardDestroy(JPADrawClipBoard_l* self,s32 flags) {
    if (self && (flags & 1)) JPADrawDelete(self);
}
/* 0283024C */
JPADrawTexCoord_l* JPADrawTexCoordConstruct(JPADrawTexCoord_l* self) {
    if (!self) self = static_cast<JPADrawTexCoord_l*>(JPADrawNew(sizeof(JPADrawTexCoord_l)));
    return self;
}
/* 02830278 */
JPADrawClipBoard_l* JPADrawClipBoardConstruct(JPADrawClipBoard_l* self) {
    if (!self) {
        self = static_cast<JPADrawClipBoard_l*>(JPADrawNew(sizeof(JPADrawClipBoard_l)));
        if (!self) return self;
    }
    if (gabi::ea(&self->mGlobalScale) == 0) JPADrawNew(sizeof(JPADrawTexCoord_l));
    if (gabi::ea(&self->mPivot) == 0) JPADrawNew(sizeof(JPADrawTexCoord_l));
    JPADrawConstructArray(self->mTexCoordPt,4,sizeof(JPADrawTexCoord_l),
        gabi::guest_fn(reinterpret_cast<const void*>(JPADrawTexCoordConstruct)));
    if (gabi::ea(self->mProjectionMtx) == 0) JPADrawNew(sizeof(self->mProjectionMtx));
    return self;
}
void JPADrawDepthChild(JPADraw_l*);
void JPADrawDepthParticle(JPADraw_l*);
void JPADrawDepthAlternateChild(JPADraw_l*,u32);
void JPADrawDepthAlternateParticle(JPADraw_l*,u32);
void JPADrawAssert(const char*,s32,const char*);
f64 JPADrawSqrt(f64);
void JPADrawMatrixIdentity(Mtx34*);
/* 0282BA44 */
void JPADraw_l::zDraw() {
    mDrawFlags = u8(mDrawFlags) | 1;
    if (childOrder() && dc.pssp.get()) JPADrawDepthChild(this);
    JPADrawDepthParticle(this);
    if (!childOrder() && dc.pssp.get()) JPADrawDepthChild(this);
}
/* 0282C750 */
void JPADraw_l::zDrawAlternate(u32 parity) {
    mDrawFlags = u8(mDrawFlags) | 1;
    if (childOrder() && dc.pssp.get()) JPADrawDepthAlternateChild(this,parity);
    JPADrawDepthAlternateParticle(this,parity);
    if (!childOrder() && dc.pssp.get()) JPADrawDepthAlternateChild(this,parity);
}
/* 0282D1A8 */
void JPADrawCopyMatrix(Mtx34* dst,const Mtx34* src) {
    // HD: lfs/stfs copies preserve bits; all source words precede the destination stores.
    auto input = reinterpret_cast<const be<u32>*>(src);
    auto output = reinterpret_cast<be<u32>*>(dst);
    u32 words[12];
    for (u32 i = 0; i < 12; ++i) words[i] = input[i];
    for (u32 i = 0; i < 12; ++i) output[i] = words[i];
}
/* 0282AAF4 */
void JPADraw_l::loadYBBMtx(const Mtx34* mtx,JPADrawClipBoard_l* cb) {
    f32 z = mtx->m[2][1];
    f32 y = mtx->m[1][1];
    f32 square = gabi::fmadds(y,y,gabi::fmuls_ppc(z,z));
    constexpr f32 EPSILON = 0x1p-18f;
    if (!(square > EPSILON)) JPADrawAssert("JPADraw.cpp",0xA3B,"!v.isZero()");
    // HD: the normalization branch requires greater-than; unordered values skip it.
    if (square > EPSILON) {
        f32 length = f32(JPADrawSqrt(f64(square)));
        f32 inverse = 1.0f / length;
        z = gabi::fmuls_ppc(z,inverse);
        y = gabi::fmuls_ppc(y,inverse);
    }
    cb->mDrawYBBMtx.m[0][0] = 1.0f;
    cb->mDrawYBBMtx.m[0][2] = 0.0f;
    cb->mDrawYBBMtx.m[0][1] = 0.0f;
    u32 xTranslation = u32(*reinterpret_cast<const be<u32>*>(&mtx->m[0][3]));
    cb->mDrawYBBMtx.m[1][0] = 0.0f;
    *reinterpret_cast<be<u32>*>(&cb->mDrawYBBMtx.m[0][3]) = xTranslation;
    cb->mDrawYBBMtx.m[1][1] = y;
    cb->mDrawYBBMtx.m[1][2] = -z;
    u32 yTranslation = u32(*reinterpret_cast<const be<u32>*>(&mtx->m[1][3]));
    cb->mDrawYBBMtx.m[2][0] = 0.0f;
    *reinterpret_cast<be<u32>*>(&cb->mDrawYBBMtx.m[1][3]) = yTranslation;
    cb->mDrawYBBMtx.m[2][1] = z;
    cb->mDrawYBBMtx.m[2][2] = y;
    u32 zTranslation = u32(*reinterpret_cast<const be<u32>*>(&mtx->m[2][3]));
    *reinterpret_cast<be<u32>*>(&cb->mDrawYBBMtx.m[2][3]) = zTranslation;
    JPADrawMatrixIdentity(&cb->mDrawMtx);
}
/* 02829EAC */
void JPADraw_l::getTextureName(SafeString* name,s16 index) {
    auto binding = &dc.mpTextureResource->mTextureBindings.get()[index];
    if (!name) {
        name = static_cast<SafeString*>(JPADrawNew(sizeof(SafeString)));
        if (!name) return;
    }
    // HD: resource names are SafeStrings in the first eight bytes of each binding.
    name->__vtbl = gabi::ea(&sTextureNameVtable);
    name->mTextPtr = u32(*reinterpret_cast<be<u32>*>(binding));
}
/* WWHD glue
str 10172060
str 1017206C
data sTextureNameVtable 10171F28
ext 0282B0D4 void JPADrawDepthChild(JPADraw_l*)
ext 0282B898 void JPADrawDepthParticle(JPADraw_l*)
ext 0282C448 void JPADrawDepthAlternateChild(JPADraw_l*,u32)
ext 0282C5CC void JPADrawDepthAlternateParticle(JPADraw_l*,u32)
ext 0273AA24 void JPADrawAssert(const char*,s32,const char*)
ext 028F37F0 f64 JPADrawSqrt(f64)
ext 028E9098 void JPADrawMatrixIdentity(Mtx34*)
ext 0273AD10 void* JPADrawNew(u32)
ext 0273AF40 void JPADrawDelete(void*)
ext 028EFFD0 void JPADrawConstructArray(void*,u32,u32,u32)
*/
