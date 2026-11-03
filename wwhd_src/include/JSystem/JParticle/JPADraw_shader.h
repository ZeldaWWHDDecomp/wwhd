// Cafe shader objects used by the JPA draw wrapper.
#pragma once
#include "JSystem/JParticle/JPADraw_passes.h"
#include "JSystem/JParticle/JPADraw_gpu_state.h"
namespace JPADrawExtension {
struct JPADrawShaderGroup_l { u8 mPrefix[0x28]; gptr<void> mState; };
struct JPADrawShader_l {
    be<u8> mFlags;
    u8 mBeforeDisplayList[3];
    gptr<void> mDisplayList;
    u8 mBeforeSize[4];
    be<u32> mDisplayListSize;
    u8 mBeforeGroup[0x6C];
    gptr<JPADrawShaderGroup_l> mGroup;
};
struct JPADrawShaderCache_l { gptr<void> mState; gptr<JPADrawShader_l> mShader; };
struct JPADrawGlobalContext_l {
    u8 mPrefix[0x18C];
    JPADrawRenderState_l mDefaultState;
};
struct JPADrawVertexVtable_l { u8 mPrefix[0x2C]; be<u32> mBind; };
WWHD_OFFSET(JPADrawShader_l,mDisplayListSize,0xC);
WWHD_OFFSET(JPADrawShader_l,mGroup,0x7C);
WWHD_OFFSET(JPADrawGlobalContext_l,mDefaultState,0x18C);
}
