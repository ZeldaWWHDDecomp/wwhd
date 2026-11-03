/* Projective geometry views inferred from this unit's instruction offsets. */
#pragma once
#include "agl/gx2/MatrixUtility.h"
#include "agl/gx2/GeometryStorage.h"
namespace agl { namespace gx2util {
struct ProjectionMatrix { MatrixRow mRows[4]; };
struct GeometryBounds { Vector3 mMinimum; Vector3 mMaximum; };
struct GeometryCorners { Vector3 mPoints[8]; };
struct GeometryByte { be<u8> mValue; };
struct GeometryPlane { Vector3 mNormal; be<f32> mOffset; };
struct GeometryGroupSet { be<u32> mCapacity; gptr<gptr<GeometryPointList>> mEntries; be<s32> mCount; };
struct GeometryWorkspace {
    u8 mPrefix[0xC]; GeometryGroupSet mSets[2]; be<u32> mActiveSet;
    GeometryCorners mCorners; be<f32> mDiameter; be<f32> mTolerance;
};
WWHD_SIZE(ProjectionMatrix, 0x40);
WWHD_SIZE(GeometryBounds, 0x18);
WWHD_SIZE(GeometryCorners, 0x60);
} }
