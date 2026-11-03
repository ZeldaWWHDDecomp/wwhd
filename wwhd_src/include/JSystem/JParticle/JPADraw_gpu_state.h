// Cafe draw-state block used for the additional particle passes.
#pragma once
#include "wwhd.h"
namespace JPADrawExtension {
struct JPADrawRenderState_l {
    be<u8> mDepthEnabled,mDepthWriteEnabled;
    u8 mDepthPadding[2];
    be<u32> mDepthFunction,mCullMode,mBlendMask;
    be<u32> mColorSource[2],mColorDestination[2],mColorOperation;
    u8 mBeforeAlphaBlend[4];
    be<u32> mAlphaSource[2],mAlphaDestination[2];
    u8 mBeforeAlpha[0xA8];
    be<u8> mAlphaEnabled;
    u8 mAlphaPadding[3];
    be<u32> mAlphaFunction;
    be<f32> mAlphaReference;
    be<u32> mLogicMask;
    u8 mBeforeChannels[0x24];
    be<u8> mChannelEnabled[4];
    u8 mTail[4];
};
WWHD_OFFSET(JPADrawRenderState_l,mAlphaEnabled,0xE0);
WWHD_OFFSET(JPADrawRenderState_l,mAlphaReference,0xE8);
WWHD_OFFSET(JPADrawRenderState_l,mChannelEnabled,0x114);
WWHD_SIZE(JPADrawRenderState_l,0x11C);
}
