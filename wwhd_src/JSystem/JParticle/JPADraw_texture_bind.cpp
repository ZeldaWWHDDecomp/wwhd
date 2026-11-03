// Cafe texture binding and per-particle descriptor caches extend tww JPADraw texture selection.
#include "JSystem/JParticle/JPADraw_passes.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
using namespace JPADrawExtension;
using JPADrawTextureDescriptor = JPAMathDraw::JPATextureDescriptor;
using JPADrawTextureState = JPAMathDraw::JPATextureState;
void JPADrawTextureAssert(const char*,s32,const char*);
JPADrawTextureState* JPADrawTextureGetState(JPAParticleRenderer*,u32);
void JPADrawTextureCopyState(JPADrawTextureState*,JPADrawTextureDescriptor*);
void JPADrawTextureBind(JPAParticleRenderer*,u32,JPADrawTextureDescriptor*,u32);
/* 0282ED90 */
bool JPADraw_l::loadTexture(u32 texNo,u8 texMap,JPAParticleRenderer* renderer) {
    auto emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    if (u8(emitter->mpDataLinkInfo->textureNum) <= texNo)
        JPADrawTextureAssert("JPADraw.cpp",0x429,"dc.pbe->getEmitterDataBlockInfoPtr()->getTextureNum() > tex_no");
    if (renderer) {
        u16 textureIndex = dc.pTexIdx.get()[texNo];
        auto source = dc.mpTextureResource->pTexResArray.get()[textureIndex].get();
        if (texMap) {
            JPADrawTextureBind(renderer,texMap,source,0);
        } else {
            auto state = JPADrawTextureGetState(renderer,0);
            bool same = true;
            for (u32 i = 0; i < 6; ++i) {
                if (u32(state->mDescriptor.mLayout[i]) != u32(source->mLayout[i])) { same = false; break; }
            }
            if (same) same = u32(state->mDescriptor.mSwizzle) == u32(source->mSwizzle);
            if (same) same = u32(state->mDescriptor.mTileMode) == u32(source->mTileMode);
            if (same) same = u32(state->mDescriptor.mAA) == u32(source->mAA);
            if (!same) JPADrawTextureCopyState(state,source);
            else {
                u32 address = source->mImageAddress;
                u32 mipAddress = source->mMipAddress;
                state->mDescriptor.mImageAddress = address;
                state->mDescriptor.mMipAddress = mipAddress;
                state->mCachedImageAddress = address;
                state->mCachedMipAddress = mipAddress;
            }
        }
        return true;
    }
    auto list = gabi::at<JPAParticleList>(u32(dc.mpActiveParticles));
    auto node = list->mpHead.get();
    while (node) {
        u16 textureIndex = dc.pTexIdx.get()[texNo];
        auto source = dc.mpTextureResource->pTexResArray.get()[textureIndex].get();
        auto particle = node->mpObject.get();
        if (source) {
            emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
            u16 generation = emitter->mGpuState.mResourceIndex;
            if (u32(particle->mCachedTextureIndex) != textureIndex || u32(particle->mCachedEmitterIndex) != generation) {
                auto state = JPADrawTextureGetState(particle->mpRenderer.get(),texMap);
                bool same = true;
                for (u32 i = 0; i < 6; ++i) {
                    if (u32(state->mDescriptor.mLayout[i]) != u32(source->mLayout[i])) { same = false; break; }
                }
                if (same) same = u32(state->mDescriptor.mSwizzle) == u32(source->mSwizzle);
                if (same) same = u32(state->mDescriptor.mTileMode) == u32(source->mTileMode);
                if (same) same = u32(state->mDescriptor.mAA) == u32(source->mAA);
                if (!same) JPADrawTextureCopyState(state,source);
                else {
                    u32 address = source->mImageAddress;
                    u32 mipAddress = source->mMipAddress;
                    state->mDescriptor.mImageAddress = address;
                    state->mCachedMipAddress = mipAddress;
                    state->mCachedImageAddress = address;
                    state->mDescriptor.mMipAddress = mipAddress;
                }
                emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
                generation = emitter->mGpuState.mResourceIndex;
                particle->mCachedTextureIndex = textureIndex;
                particle->mCachedEmitterIndex = generation;
            }
        }
        node = node->mpNext.get();
    }
    return true;
}
/* WWHD glue
str 10172190
str 1017219C
ext 0273AA24 void JPADrawTextureAssert(const char*,s32,const char*)
ext 027FE640 JPADrawTextureState* JPADrawTextureGetState(JPAParticleRenderer*,u32)
ext 027BDEB4 void JPADrawTextureCopyState(JPADrawTextureState*,JPADrawTextureDescriptor*)
ext 027FE500 void JPADrawTextureBind(JPAParticleRenderer*,u32,JPADrawTextureDescriptor*,u32)
*/
