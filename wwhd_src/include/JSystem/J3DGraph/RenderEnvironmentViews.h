// J3D HD renderer: views of the environment objects the frame update reads (lights, colour, fog).
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderFrameArray.h"
namespace j3d {
struct RenderEnvironmentMethods_l {
    u8 mEarlierMethods[0x44];
    be<u32> mMatchesType;
};
struct RenderEnvironmentObject_l {
    u8 mParameterState[0x58];
    gptr<RenderEnvironmentMethods_l> mMethods;
    u8 mObjectState[0x14];
    be<u8> mEnabled;
};
struct RenderPositionEnvironment_l : RenderEnvironmentObject_l {
    u8 mColorState[0x83];
    RenderWideWords mColor;
    u8 mPositionState[0x28];
    RenderFloat3 mPosition;
};
struct RenderColorEnvironment_l : RenderEnvironmentObject_l {
    u8 mColorState[0x83];
    RenderWideWords mColor;
};
struct RenderFogEnvironment_l : RenderEnvironmentObject_l {
    u8 mRangeState[0x83];
    be<f32> mNear;
    u8 mNearParameterState[12];
    be<f32> mFar;
    u8 mFarParameterState[12];
    RenderWideWords mColor;
};
struct RenderEnvironmentRange_l {
    be<u16> mFirst;
    be<u16> mCount;
};
struct RenderEnvironmentCollection_l {
    u8 mCollectionState[8];
    be<u32> mRangeCount;
    gptr<RenderEnvironmentRange_l> mRanges;
    be<u32> mObjectCount;
    gptr<gptr<RenderEnvironmentObject_l>> mObjects;
};
struct RenderEnvironmentTypeIndex_l { be<s16> mIndex; };
struct RenderFrameEnvironmentEntries {
    static void update(RenderFrameArray* frames, u32 index, const Mtx34* model,
                       const Mtx44* projection, const RenderEnvironmentCollection_l* environment);
};
}
WWHD_OFFSET(j3d::RenderEnvironmentMethods_l, mMatchesType, 0x44);
WWHD_OFFSET(j3d::RenderEnvironmentObject_l, mMethods, 0x58);
WWHD_OFFSET(j3d::RenderEnvironmentObject_l, mEnabled, 0x70);
WWHD_OFFSET(j3d::RenderPositionEnvironment_l, mColor, 0xF4);
WWHD_OFFSET(j3d::RenderPositionEnvironment_l, mPosition, 0x12C);
WWHD_OFFSET(j3d::RenderColorEnvironment_l, mColor, 0xF4);
WWHD_OFFSET(j3d::RenderFogEnvironment_l, mNear, 0xF4);
WWHD_OFFSET(j3d::RenderFogEnvironment_l, mFar, 0x104);
WWHD_OFFSET(j3d::RenderFogEnvironment_l, mColor, 0x114);
WWHD_SIZE(j3d::RenderEnvironmentRange_l, 4);
WWHD_OFFSET(j3d::RenderEnvironmentCollection_l, mRangeCount, 8);
WWHD_OFFSET(j3d::RenderEnvironmentCollection_l, mRanges, 12);
WWHD_OFFSET(j3d::RenderEnvironmentCollection_l, mObjectCount, 16);
WWHD_OFFSET(j3d::RenderEnvironmentCollection_l, mObjects, 20);
WWHD_SIZE(j3d::RenderEnvironmentCollection_l, 24);
void composeRenderFrameProjection(Mtx44* output, const Mtx44* projection, const Mtx34* model);
void writeRenderFrameProjection(const Mtx44* projection, Mtx44* output);
