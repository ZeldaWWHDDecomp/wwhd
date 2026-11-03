// The projection polygon workspace that this unit constructs and fills (027D7BE4..027D7F44), on the canonical layout of
// agl::gx2util::GeometryWorkspace (the GX2 utility clipper cuts its polygons; the shadow projection fit reads them):
// three 12-byte groups of 16 point lists (group 0 = the clipper's cut lists at +0, groups 1 and 2 = mSets[0..1],
// alternating as the current polygon set), the active-set index, the eight box corners and two limits.
#pragma once
#include "wwhd.h"
#include "sead/HeapRegistry.h"
#include "agl/gx2/GeometryProjection.h"
namespace agl { namespace render {
using gx2util::GeometryGroupSet;
using gx2util::GeometryPointList;
using gx2util::GeometryWorkspace;
constexpr u32 kGeometryListsPerGroup = 16;   // point lists per group
constexpr u32 kGeometryPointsPerList = 16;   // points per list (0xC0 bytes, 32-aligned)
// Group 0 of the workspace (+0); the merged view keeps it as an opaque prefix.
inline GeometryGroupSet* geometryCutGroup(GeometryWorkspace* workspace) {
    return reinterpret_cast<GeometryGroupSet*>(workspace->mPrefix);
}
void allocateGeometryGroup(GeometryGroupSet*,sead::RegistryHeap*);
void allocateGeometryWorkspace(GeometryWorkspace*,sead::RegistryHeap*);
GeometryGroupSet* createGeometryGroup(GeometryGroupSet*);
GeometryWorkspace* createGeometryWorkspace(GeometryWorkspace*);
GeometryPointList* takeGeometrySlot(GeometryGroupSet*);
void resetGeometryWorkspace(GeometryWorkspace*);
} }
void* allocateGeometryObject(u32,sead::RegistryHeap*,s32);
void constructGeometryGroups(void*,u32,u32,u32);
void zeroGeometryPoints(void*,u32);
WWHD_SIZE(agl::gx2util::GeometryPointList,0xC);
WWHD_SIZE(agl::gx2util::GeometryGroupSet,0xC);
WWHD_SIZE(agl::gx2util::GeometryWorkspace,0x90);
WWHD_OFFSET(agl::gx2util::GeometryWorkspace,mSets,0xC);
WWHD_OFFSET(agl::gx2util::GeometryWorkspace,mActiveSet,0x24);
WWHD_OFFSET(agl::gx2util::GeometryWorkspace,mCorners,0x28);
WWHD_OFFSET(agl::gx2util::GeometryWorkspace,mDiameter,0x88);
WWHD_OFFSET(agl::gx2util::GeometryWorkspace,mTolerance,0x8C);
