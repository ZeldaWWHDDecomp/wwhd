// Own parameter-writer prefixes derived from this unit's preparation caller.
#pragma once
#include "agl/DrawPreparation.h"
#include "agl/DrawResourceCleanup.h"
#include "agl/RenderSelection.h"
namespace agl {
struct DrawParameterRecord_l : DrawParameterState_l {
    be<u32> mValueCount;
    gptr<SelectionValue_l> mValues;
};
struct DrawParameterFrame_l {
    sead::RenderMatrix34 mWorldToView;
    u8 mPrefix[0xB8];
    be<f32> mVerticalMultiplier;
    u8 mBeforeProjection[0x3C];
    sead::RenderMatrix44 mViewProjection;
    u8 mPayload[0xCB0];
    sead::RenderMatrix34 mView;
    sead::RenderMatrix44 mProjection;
    be<f32> mOffsetX, mOffsetY;
    u8 mReserved[0xC];
    be<f32> mHorizontalMultiplier;
    be<f32> mProjectionHeight;
};
struct DrawPairParameterValue_l { be<u32> mFlags; UniformBuffer mBuffer; };
struct DrawPairParameterRecord_l {
    u8 mPrefix[8];
    gptr<DrawParameterState_l> mFirst, mSecond, mSelected;
    be<u32> mValueCount;
    gptr<DrawPairParameterValue_l> mValues;
};
struct DrawParameterPreparation_l {
    u8 mPrefix[4];
    gptr<DrawResourceConfiguration_l> mConfiguration;
    void preparePairParameters(DrawPairParameterRecord_l* record, u32 slot, const DrawParameterFrame_l* context);
    void prepareParameters(DrawParameterRecord_l* record, u32 slot, const DrawParameterFrame_l* context);
};
}
WWHD_SIZE(agl::DrawParameterRecord_l, 0x48);
WWHD_OFFSET(agl::DrawParameterRecord_l, mValueCount, 0x40);
WWHD_OFFSET(agl::DrawParameterRecord_l, mValues, 0x44);
WWHD_OFFSET(agl::DrawParameterFrame_l, mView, 0xE18);
WWHD_OFFSET(agl::DrawParameterFrame_l, mProjection, 0xE48);
WWHD_OFFSET(agl::DrawParameterFrame_l, mOffsetX, 0xE88);
WWHD_OFFSET(agl::DrawParameterFrame_l, mHorizontalMultiplier, 0xE9C);
WWHD_OFFSET(agl::DrawParameterFrame_l, mProjectionHeight, 0xEA0);

WWHD_SIZE(agl::DrawPairParameterValue_l, 0x20);
WWHD_OFFSET(agl::DrawPairParameterRecord_l, mSelected, 0x10);
WWHD_OFFSET(agl::DrawPairParameterRecord_l, mValueCount, 0x14);
WWHD_OFFSET(agl::DrawPairParameterRecord_l, mValues, 0x18);

WWHD_OFFSET(agl::DrawParameterFrame_l, mViewProjection, 0x128);
