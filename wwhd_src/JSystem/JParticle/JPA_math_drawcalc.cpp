/** Particle rotation/key math and draw initialization.
 * HD texture descriptors and particle caches retain their native load/store order. */
#include "JSystem/JParticle/JPA_math_drawcalc.h"
#include <cstring>
using namespace JPAMathDraw;
static JPAMathSinCosTable l_sincosTable;
using JPAMathFloat = be<f32>;
static JPAMathFloat l_axisX;
static JPAMathFloat l_axisY;
static JPAMathFloat l_axisZ;
void JPAMath_bindTexture(void *, u32, JPATextureDescriptor *, void *);
JPATextureState *JPAMath_getTextureState(void *, u32);
void JPAMath_resetTextureState(JPADraw *, s16, JPATextureState *);
void JPAMath_copyTextureState(JPATextureState *, JPATextureDescriptor *);
f64 JPAMath_sqrt(f64);
namespace {
f32 sinAngle(u16 a) { return l_sincosTable.entries[a >> 3].sin; }
f32 cosAngle(u16 a) { return l_sincosTable.entries[a >> 3].cos; }
} // namespace
/* 02824520 */
void JPAGetYZRotateMtx(s16 y, s16 z, JPAMtxP dst) {
    f32 sy = sinAngle(y), sz = sinAngle(z), cy = cosAngle(y), cz = cosAngle(z);
    dst[0][0] = gabi::fmuls_ppc(cy, cz);
    dst[0][1] = -sz;
    dst[0][2] = gabi::fmuls_ppc(sy, cz);
    dst[0][3] = 0.0f;
    dst[1][0] = gabi::fmuls_ppc(cy, sz);
    dst[1][1] = cz;
    dst[1][2] = gabi::fmuls_ppc(sy, sz);
    dst[1][3] = 0.0f;
    dst[2][0] = -sy;
    dst[2][1] = 0.0f;
    dst[2][2] = cy;
    dst[2][3] = 0.0f;
}
/* 028245AC */
void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, JPAMtxP dst) {
    f32 sx = sinAngle(x), sy = sinAngle(y), sz = sinAngle(z), cx = cosAngle(x), cy = cosAngle(y),
        cz = cosAngle(z);
    f32 sxcz = gabi::fmuls_ppc(sx, cz), cxsz = gabi::fmuls_ppc(cx, sz);
    f32 sxsz = gabi::fmuls_ppc(sx, sz), cxcz = gabi::fmuls_ppc(cx, cz);
    dst[0][0] = gabi::fmuls_ppc(cy, cz);
    dst[1][0] = gabi::fmuls_ppc(cy, sz);
    dst[2][0] = -sy;
    dst[2][1] = gabi::fmuls_ppc(sx, cy);
    dst[2][2] = gabi::fmuls_ppc(cx, cy);
    dst[0][1] = gabi::fmsubs(sxcz, sy, cxsz);
    dst[1][2] = gabi::fmsubs(cxsz, sy, sxcz);
    dst[0][2] = gabi::fmadds(cxcz, sy, sxsz);
    dst[1][1] = gabi::fmadds(sxsz, sy, cxcz);
    dst[0][3] = 0.0f;
    dst[1][3] = 0.0f;
    dst[2][3] = 0.0f;
}
/* 02824AA0 */
void JPAGetUnitVec(s16 x, s16 y, cXyz *dst) {
    f32 sx = sinAngle(x);
    dst->x = gabi::fmuls_ppc(cosAngle(y), sx);
    dst->y = gabi::fmuls_ppc(sinAngle(y), sx);
    dst->z = cosAngle(x);
}
/* 0282466C */
void JPAGetDirMtx(const cXyz *dir, JPAMtxP dst) {
    f32 y = -(f32)dir->x, x = dir->y, z = dir->z;
    f32 squared = gabi::fmadds(x, x, gabi::fmuls_ppc(y, y));
    f32 length = (f32)JPAMath_sqrt((f64)squared);
    if (length > 0x1p-18f) {
        f32 inv = 1.0f / length;
        x = gabi::fmuls_ppc(x, inv);
        y = gabi::fmuls_ppc(y, inv);
    } else {
        x = 0.0f;
        y = 0.0f;
    }
    f32 xx = gabi::fmuls_ppc(x, x), yy = gabi::fmuls_ppc(y, y), xy = gabi::fmuls_ppc(x, y);
    f32 ym = gabi::fmuls_ppc(y, length), xm = gabi::fmuls_ppc(x, length);
    f32 off = gabi::fmuls_ppc(xy, gabi::fsubs_ppc(1.0f, z));
    dst[0][0] = gabi::fmadds(z, gabi::fsubs_ppc(1.0f, xx), xx);
    dst[0][1] = off;
    dst[0][2] = -ym;
    dst[0][3] = 0.0f;
    dst[1][0] = off;
    dst[1][1] = gabi::fmadds(z, gabi::fsubs_ppc(1.0f, yy), yy);
    dst[1][2] = xm;
    dst[1][3] = 0.0f;
    dst[2][0] = ym;
    dst[2][1] = -xm;
    dst[2][2] = z;
    dst[2][3] = 0.0f;
}
/* 028249D8 */
f32 JPAGetKeyFrameValue(f32 time, s32 frameNum, const be<f32> *pFrames) {
    // HD: the verified frame count is signed32; retain GHS Hermite fusion and comparisons.
    f32 first = pFrames[0];
    if (time < first)
        return pFrames[1];
    if (!((f32)pFrames[(u32)frameNum * 4 - 4] > time))
        return pFrames[(u32)frameNum * 4 - 3];
    while (frameNum > 1) {
        s32 half = frameNum / 2;
        if (time < (f32)pFrames[half * 4])
            frameNum = half;
        else {
            frameNum -= half;
            pFrames += half * 4;
        }
    }
    f32 t0 = pFrames[0], t1 = pFrames[4];
    f32 delta = gabi::fsubs_ppc(time, t0), span = gabi::fsubs_ppc(t1, t0), u = delta / span;
    f32 square = gabi::fmuls_ppc(u, u), difference = gabi::fsubs_ppc(square, u);
    f32 value = pFrames[1], next = pFrames[5], out = pFrames[3], in = pFrames[6];
    f32 a = gabi::fmadds(out, difference, out);
    f32 b = gabi::fmsubs(gabi::fadds_ppc(u, u), difference, square);
    f32 c = gabi::fmadds(in, difference, a);
    f32 d = gabi::fmadds(b, gabi::fsubs_ppc(value, next), value);
    f32 e = gabi::fmsubs(u, out, c);
    return -gabi::fmsubs(delta, e, d);
}
// HD: texture binding state has no GameCube counterpart.
/* 0282DDE4 */
void JPA_drawBindTextureState(JPADraw *draw, s16 index, JPATextureState *state) {
    if (!state)
        return;
    JPATextureResource *resource = draw->dc.mpTextureResource;
    // HD: signed texture index uses native 32-bit wrapping address arithmetic.
    JPATextureBinding *entry =
        gabi::at<JPATextureBinding>(gabi::ea(resource->mTextureBindings.get()) + s32(index) * 16);
    u32 oldA = state->mBinding[0];
    u32 a = entry->mBinding[0];
    u32 oldB = state->mBinding[1];
    u32 b = entry->mBinding[1];
    if (oldA != a) {
        state->mBinding[0] = a;
        state->mDirtyFlags = u8(state->mDirtyFlags) | TEXTURE_BINDING_DIRTY;
    }
    if (oldB != b) {
        state->mBinding[1] = b;
        state->mDirtyFlags = u8(state->mDirtyFlags) | TEXTURE_BINDING_DIRTY;
    }
    for (u32 i = 0; i < 2; ++i) {
        if (u32(state->mMode[i]) != 1) {
            state->mMode[i] = 1;
            state->mDirtyFlags = u8(state->mDirtyFlags) | TEXTURE_MODE_DIRTY;
        }
    }
    for (u32 i = 0; i < 4; ++i)
        state->mExtraState[i] = 0;
    state->mDirtyFlags = u8(state->mDirtyFlags) | TEXTURE_EXTRA_DIRTY;
}
/* 0282DC48 */
void JPADraw::calc() {
    for (s32 i = 0; i < u8(calcEmtrVisNum); ++i) {
        JPACalcVisitor *visitor = mpCalcEmtrVis[i];
        gabi::call_ptr<void>(visitor->__vtbl->calc, visitor, &dc);
    }
}
/* 0282DCBC */
void JPADraw::calcParticle(JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    params->mRotateAngle = u16(params->mRotateAngle) + s16(params->mRotateSpeed);
    for (s32 i = 0; i < u8(calcPtclVisNum); ++i) {
        JPACalcVisitor *visitor = mpCalcPtclVis[i];
        gabi::call_ptr<void>(visitor->__vtbl->calc, visitor, &dc, ptcl);
    }
}
/* 0282DD50 */
void JPADraw::calcChild(JPABaseParticle *ptcl) {
    JPADrawParams *params = reinterpret_cast<JPADrawParams *>(ptcl->mDrawParams);
    params->mRotateAngle = u16(params->mRotateAngle) + s16(params->mRotateSpeed);
    for (s32 i = 0; i < u8(calcChldVisNum); ++i) {
        JPACalcVisitor *visitor = mpCalcChldVis[i];
        gabi::call_ptr<void>(visitor->__vtbl->calc, visitor, &dc, ptcl);
    }
}
namespace {
f32 randomF(JPADraw *draw) {
    JPABaseEmitterHD *emitter = draw->dc.pbe;
    u32 seed = u32(emitter->mRandomSeed) * 0x19660du + 0x3c6ef35fu;
    emitter->mRandomSeed = seed;
    u32 bits = (seed >> 9) | 0x3f800000;
    f32 value;
    std::memcpy(&value, &bits, 4);
    return gabi::fsubs_ppc(value, 1.0f);
}
f32 randomRF(JPADraw *draw) {
    f32 value = randomF(draw);
    return gabi::fsubs_ppc(gabi::fadds_ppc(value, value), 1.0f);
}
// HD: texture indices keep native 32-bit wrapping table arithmetic.
u32 textureIndex(JPADraw *draw, u32 index) {
    return *gabi::at<be<u16>>(gabi::ea(draw->dc.pTexIdx.get()) + index * 2);
}
JPATextureDescriptor *texture(JPADraw *draw, u32 index) {
    return gabi::at<gptr<JPATextureDescriptor>>(gabi::ea(draw->dc.mpTextureResource->pTexResArray.get()) +
                                                index * 4)
        ->get();
}
void bindTexture(JPADraw *draw, JPAParticleHD *particle, u32 slot, u32 index) {
    JPATextureDescriptor *object = texture(draw, index);
    void *context = draw->dc.pbe->mpTextureContext;
    JPAMath_bindTexture(particle->mpTextureBindings.get(), slot, object, context);
}
JPATextureState *textureState(JPAParticleHD *particle, u32 slot) {
    return JPAMath_getTextureState(particle->mpTextureBindings.get(), slot);
}
void resetTextureState(JPADraw *draw, JPAParticleHD *particle, u32 slot, u32 index) {
    JPATextureState *state = textureState(particle, slot);
    // HD: retain the separately verified native call, rather than inlining its stores.
    JPAMath_resetTextureState(draw, s16(index), state);
}
// HD: particles cache a descriptor and the emitter's texture-generation counter.
void cacheTexture(JPADraw *draw, JPAParticleHD *particle, u32 index, JPATextureDescriptor *object) {
    if (u32(particle->mTextureIndex) == index &&
        u32(particle->mTextureGeneration) == u16(draw->dc.pbe->mTextureGeneration))
        return;
    JPATextureState *state = textureState(particle, 0);
    bool equal = true;
    for (u32 i = 0; i < 6; ++i) {
        if (u32(state->mDescriptor.mLayout[i]) != u32(object->mLayout[i])) {
            equal = false;
            break;
        }
    }
    if (equal && u32(state->mDescriptor.mLayoutTail[1]) != u32(object->mLayoutTail[1]))
        equal = false;
    if (equal && u32(state->mDescriptor.mLayoutTail[0]) != u32(object->mLayoutTail[0]))
        equal = false;
    if (equal && u32(state->mDescriptor.mSwizzle) != u32(object->mSwizzle))
        equal = false;
    if (!equal)
        JPAMath_copyTextureState(state, object);
    else {
        u32 address = object->mAddress;
        u32 mipAddress = object->mMipAddress;
        state->mCachedAddress = address;
        state->mCachedMipAddress = mipAddress;
        state->mDescriptor.mAddress = address;
        state->mDescriptor.mMipAddress = mipAddress;
    }
    particle->mTextureIndex = index;
    particle->mTextureGeneration = u16(draw->dc.pbe->mTextureGeneration);
    if (!draw->dc.pbsp->textureIsEmpty())
        resetTextureState(draw, particle, 0, index);
}
} // namespace
/* 0282DED0 */
void JPADraw::initParticle(JPAParticleHD *ptcl) {
    f32 ax = f32(l_axisX), ay = f32(l_axisY), az = f32(l_axisZ);
    ptcl->mDrawParams.mAxis.y = ay;
    ptcl->mDrawParams.mAxis.x = ax;
    ptcl->mDrawParams.mAxis.z = az;
    ptcl->mDrawParams.mPrmColor = u32(this->mPrmColor);
    ptcl->mDrawParams.mEnvColor = u32(this->mEnvColor);
    ptcl->mDrawParams.mAlphaOut = 1.0f;
    f32 random = randomF(this);
    s32 offset = this->dc.pbsp->getLoopOffset();
    ptcl->mDrawParams.mLoopOffset = gabi::ftoi(gabi::fmuls_ppc(random, (f32)offset));
    if (this->dc.pesp.get()) {
        if (this->dc.pesp->isEnableRotate()) {
            f32 r = gabi::fsubs_ppc(randomF(this), 0.5f);
            JPAExtraShape *extra = this->dc.pesp;
            JPAExtraShapeVtable *table = extra->__vtbl;
            f32 a = gabi::call_ptr<f32>(u32(table->getRotateRandomAngle), extra);
            f32 angleRandom = gabi::fmuls_ppc(r, a);
            f32 angle = gabi::call_ptr<f32>(u32(table->getRotateAngle), this->dc.pesp.get());
            ptcl->mDrawParams.mRotateAngle =
                gabi::ftoi(gabi::fmadds(angleRandom, 65536.0f, gabi::fmuls_ppc(angle, 32768.0f)));
            r = randomRF(this);
            f32 direction = this->dc.pesp->getRotateDirection();
            extra = this->dc.pesp.get();
            table = extra->__vtbl.get();
            f32 speedRandom = randomRF(this);
            f32 speed = gabi::call_ptr<f32>(u32(table->getRotateSpeed), this->dc.pesp.get());
            if (!(r < direction))
                speed = -speed;
            f32 variation = gabi::call_ptr<f32>(u32(table->getRotateRandomSpeed), this->dc.pesp.get());
            ptcl->mDrawParams.mRotateSpeed = gabi::ftoi(gabi::fmuls_ppc(
                gabi::fmuls_ppc(speed, gabi::fmadds(variation, speedRandom, 1.0f)), 32768.0f));
        } else {
            ptcl->mDrawParams.mRotateAngle = 0;
            ptcl->mDrawParams.mRotateSpeed = 0;
        }
        f32 scale;
        if (this->dc.pesp->isEnableScale()) {
            f32 r = randomRF(this), variation = this->dc.pesp->getRandomScale();
            scale = gabi::fmuls_ppc(gabi::fmadds(r, variation, 1.0f), f32(this->mScaleOut));
        } else
            scale = f32(this->mScaleOut);
        ptcl->mDrawParams.mScaleOut = scale;
        ptcl->mDrawParams.mScaleY = scale;
        ptcl->mDrawParams.mScaleX = scale;
        f32 alpha = 1.0f;
        if (this->dc.pesp->isEnableAlpha()) {
            f32 r = randomRF(this), variation = this->dc.pesp->getAlphaWaveRandom();
            alpha = gabi::fmadds(r, variation, 1.0f);
        }
        ptcl->mDrawParams.mAlphaWaveRandom = alpha;
    } else {
        ptcl->mDrawParams.mRotateAngle = 0;
        ptcl->mDrawParams.mRotateSpeed = 0;
        f32 scale = f32(this->mScaleOut);
        ptcl->mDrawParams.mScaleY = scale;
        ptcl->mDrawParams.mScaleX = scale;
        ptcl->mDrawParams.mScaleOut = scale;
        ptcl->mDrawParams.mAlphaWaveRandom = 1.0f;
    }
    u32 index = textureIndex(this, 0);
    JPATextureDescriptor *object = texture(this, index);
    if (this->dc.pbsp->textureIsEmpty()) {
        JPATextureResource *resource = this->dc.mpTextureResource;
        object = texture(this, u32(resource->mDefaultTextureIndex));
        index = DEFAULT_TEXTURE_CACHE_INDEX;
    }
    cacheTexture(this, ptcl, index, object);
    if (!this->dc.petx.get())
        return;
    u32 type = this->dc.petx->getIndTexMode();
    if (type == IND_TEXTURE_SINGLE || type == IND_TEXTURE_SUB) {
        index = textureIndex(this, this->dc.petx->getIndTextureID());
        bindTexture(this, ptcl, IND_TEXTURE_SLOT, index);
        if (type == IND_TEXTURE_SINGLE)
            resetTextureState(this, ptcl, IND_TEXTURE_SLOT, index);
        else {
            index = textureIndex(this, this->dc.petx->getSubTextureID());
            bindTexture(this, ptcl, SUB_TEXTURE_SLOT, index);
        }
    }
    if (this->dc.petx->isEnableSecondTex()) {
        index = textureIndex(this, this->dc.petx->getSecondTexIndex());
        bindTexture(this, ptcl, SECOND_TEXTURE_SLOT, index);
        resetTextureState(this, ptcl, SECOND_TEXTURE_SLOT, index);
    }
}
/* 0282E770 */
void JPADraw::initChild(JPAParticleHD *ptcl, JPAParticleHD *chld) {
    *reinterpret_cast<be<u32> *>(&chld->mDrawParams.mAxis.x) =
        u32(*reinterpret_cast<be<u32> *>(&ptcl->mDrawParams.mAxis.x));
    *reinterpret_cast<be<u32> *>(&chld->mDrawParams.mAxis.y) =
        u32(*reinterpret_cast<be<u32> *>(&ptcl->mDrawParams.mAxis.y));
    chld->mDrawParams.mAlphaOut = 1.0f;
    *reinterpret_cast<be<u32> *>(&chld->mDrawParams.mAxis.z) =
        u32(*reinterpret_cast<be<u32> *>(&ptcl->mDrawParams.mAxis.z));
    if (this->dc.pssp->isInheritedRGB()) {
        f32 ratio = this->dc.pssp->getInheritRGB();
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mPrmColor)[0] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mPrmColor)[0]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mPrmColor)[1] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mPrmColor)[1]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mPrmColor)[2] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mPrmColor)[2]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mEnvColor)[0] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mEnvColor)[0]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mEnvColor)[1] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mEnvColor)[1]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mEnvColor)[2] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mEnvColor)[2]), ratio));
    } else {
        chld->mDrawParams.mPrmColor = this->dc.pssp->getPrm();
        chld->mDrawParams.mEnvColor = this->dc.pssp->getEnv();
    }
    if (this->dc.pssp->isInheritedAlpha()) {
        f32 ratio = this->dc.pssp->getInheritAlpha();
        ratio = gabi::fmuls_ppc(ratio, f32(ptcl->mDrawParams.mAlphaOut));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mPrmColor)[3] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mPrmColor)[3]), ratio));
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mEnvColor)[3] = gabi::ftoi(
            gabi::fmuls_ppc((f32)u8(reinterpret_cast<be<u8> *>(&ptcl->mDrawParams.mEnvColor)[3]), ratio));
    } else {
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mPrmColor)[3] = this->dc.pssp->getPrmAlpha();
        reinterpret_cast<be<u8> *>(&chld->mDrawParams.mEnvColor)[3] = this->dc.pssp->getEnvAlpha();
    }
    if (this->dc.pssp->isInheritedScale()) {
        f32 ratio = this->dc.pssp->getInheritScale(),
            x = gabi::fmuls_ppc(ratio, f32(ptcl->mDrawParams.mScaleX));
        chld->mDrawParams.mScaleOut = x;
        chld->mDrawParams.mScaleX = x;
        f32 y = gabi::fmuls_ppc(ratio, f32(ptcl->mDrawParams.mScaleY));
        chld->mDrawParams.mAlphaWaveRandom = y;
        chld->mDrawParams.mScaleY = y;
    } else {
        chld->mDrawParams.mAlphaWaveRandom = 1.0f;
        chld->mDrawParams.mScaleOut = 1.0f;
        chld->mDrawParams.mScaleX = 1.0f;
        chld->mDrawParams.mScaleY = 1.0f;
    }
    chld->mDrawParams.mRotateAngle = u16(ptcl->mDrawParams.mRotateAngle);
    if (this->dc.pssp->isEnableRotate()) {
        f32 speed = this->dc.pssp->getRotateSpeed();
        chld->mDrawParams.mRotateSpeed = gabi::ftoi(gabi::fmuls_ppc(speed, 32768.0f));
    } else
        chld->mDrawParams.mRotateSpeed = 0;
    u32 index = textureIndex(this, 0);
    if (!this->dc.pbsp->textureIsEmpty())
        index = textureIndex(this, this->dc.pssp->getTextureIndex());
    JPATextureDescriptor *object = texture(this, index);
    if (this->dc.pbsp->textureIsEmpty()) {
        object = texture(this, u32(this->dc.mpTextureResource->mDefaultTextureIndex));
        index = DEFAULT_TEXTURE_CACHE_INDEX;
    }
    cacheTexture(this, chld, index, object);
}

/* WWHD glue (tools/verify/autoglue.py): translation unit data
data l_sincosTable 104A44F8
data l_axisX 104B576C
data l_axisY 104B577C
data l_axisZ 104B578C
ext 027FE500 void JPAMath_bindTexture(void*,u32,JPATextureDescriptor*,void*)
ext 027FE640 JPATextureState* JPAMath_getTextureState(void*,u32)
ext 0282DDE4 void JPAMath_resetTextureState(JPADraw*,s16,JPATextureState*)
ext 027BDEB4 void JPAMath_copyTextureState(JPATextureState*,JPATextureDescriptor*)
ext 028F37F0 f64 JPAMath_sqrt(f64)
*/
