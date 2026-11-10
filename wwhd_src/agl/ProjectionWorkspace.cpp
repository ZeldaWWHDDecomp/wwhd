// Projection polygon workspace (agl::gx2util::GeometryWorkspace): construction, point-list allocation, slot acquire and
// the reset that writes the box's six faces from its eight corners into the next polygon set.
#include "agl/ProjectionPolygonWorkspace.h"
#include "sead/ElementAt.h"
using namespace agl::render;
void* allocateGeometryStorage(u32);
// Constructor defaults: diameter 0.0, clip tolerance 0.05.
typedef be<f32> GeometryScalar;
static GeometryScalar sGeometryInitialDiameter_l;
static GeometryScalar sGeometryInitialTolerance_l;
static void* allocateFromHeap(sead::RegistryHeap* heap,u32 size,s32 alignment) {
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,size,alignment);
}
/* 027D7BE4 */
void agl::render::allocateGeometryGroup(GeometryGroupSet* group,sead::RegistryHeap* requestedHeap) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::RegistryHeap* heap = requestedHeap;
    if (!heap) heap = registryCurrentHeap(sRegistryManager);
    gptr<GeometryPointList>* lists = static_cast<gptr<GeometryPointList>*>(
        allocateFromHeap(heap,kGeometryListsPerGroup * sizeof(gptr<GeometryPointList>),4));
    if (lists) {
        group->mEntries = lists;
        group->mCapacity = kGeometryListsPerGroup;
    } else {
        lists = group->mEntries;
    }
    for (u32 index = 0; index < kGeometryListsPerGroup; ++index) {
        gptr<GeometryPointList>* slot = index < (u32)group->mCapacity ? lists + index : lists;
        GeometryPointList* list = static_cast<GeometryPointList*>(allocateGeometryObject(sizeof(GeometryPointList),requestedHeap,4));
        if (list) {
            list->mData = NULL;
            list->mCapacity = 0;
            list->mCount = 0;
        }
        *slot = list;
        gptr<GeometryPointList>* entries = group->mEntries;
        list = index < (u32)group->mCapacity ? entries[index] : entries[0];
        heap = requestedHeap;
        if (!heap) heap = registryCurrentHeap(sRegistryManager);
        void* points = allocateFromHeap(heap,kGeometryPointsPerList * sizeof(agl::gx2util::GeometryTriple),0x20);
        if (points) {
            list->mData = points;
            list->mCapacity = kGeometryPointsPerList;
        }
        lists = group->mEntries;
    }
}
/* 027D7D40 */
void agl::render::allocateGeometryWorkspace(GeometryWorkspace* workspace,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    allocateGeometryGroup(&workspace->mSets[0],heap);
    allocateGeometryGroup(&workspace->mSets[1],heap);
    allocateGeometryGroup(geometryCutGroup(workspace),heap);
}
/* 027D7D94 */
agl::render::GeometryGroupSet* agl::render::createGeometryGroup(GeometryGroupSet* group) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!group) group = static_cast<GeometryGroupSet*>(allocateGeometryStorage(sizeof(GeometryGroupSet)));
    if (group) {
        group->mEntries = NULL;
        group->mCapacity = 0;
        group->mCount = 0;
    }
    return group;
}
/* 027D7E04 */
agl::render::GeometryWorkspace* agl::render::createGeometryWorkspace(GeometryWorkspace* workspace) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!workspace) workspace = static_cast<GeometryWorkspace*>(allocateGeometryStorage(sizeof(GeometryWorkspace)));
    if (!workspace) return NULL;
    createGeometryGroup(geometryCutGroup(workspace));
    constructGeometryGroups(workspace->mSets,2,sizeof(GeometryGroupSet),gabi::guest_fn((const void*)createGeometryGroup));
    workspace->mActiveSet = 0;
    f32 diameter = sGeometryInitialDiameter_l;
    f32 tolerance = sGeometryInitialTolerance_l;
    workspace->mDiameter = diameter;
    workspace->mTolerance = tolerance;
    return workspace;
}
/* 027D7EA8 */
agl::render::GeometryPointList* agl::render::takeGeometrySlot(GeometryGroupSet* group) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 used = group->mCount;
    if ((s32)used >= (s32)kGeometryListsPerGroup) return NULL;
    gptr<GeometryPointList>* entries = group->mEntries;
    GeometryPointList* list = used < (u32)group->mCapacity ? entries[used] : entries[0];
    zeroGeometryPoints(list->mData,(u32)list->mCapacity * sizeof(agl::gx2util::GeometryTriple));
    list->mCount = 0;
    group->mCount = (u32)group->mCount + 1u;
    return list;
}
static void appendGeometryCorner(GeometryPointList* list,const agl::gx2util::Vector3* corner) {
    u32 used = list->mCount;
    u32 capacity = list->mCapacity;
    agl::gx2util::GeometryTriple* points = static_cast<agl::gx2util::GeometryTriple*>(list->mData.get());
    list->mCount = used + 1u;
    f32 x = corner->x.mValue;
    agl::gx2util::GeometryTriple* point = &sead::elementAt<agl::gx2util::GeometryTriple>(gabi::ea(points),used < capacity ? used : 0u); /* guest EA arithmetic: wraps at 32 bits as on the console */
    point->x = x;
    point->y = (f32)corner->y.mValue;
    point->z = (f32)corner->z.mValue;
}
// Corner indices of the box faces, in the winding the clipper expects.
static const u8 kBoxFaces[6][4] = {{0,1,2,3},{7,6,5,4},{0,3,7,4},{5,6,2,1},{6,7,3,2},{1,0,4,5}};
/* 027D7F44 */
void agl::render::resetGeometryWorkspace(GeometryWorkspace* workspace) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 face = 0; face < 6; ++face) {
        u32 next = ((u32)workspace->mActiveSet + 1u) & 1u;
        GeometryPointList* list = takeGeometrySlot(&workspace->mSets[next]);
        for (u32 corner = 0; corner < 4; ++corner) appendGeometryCorner(list,&workspace->mCorners.mPoints[kBoxFaces[face][corner]]);
    }
    u32 next = ((u32)workspace->mActiveSet + 1u) & 1u;
    workspace->mActiveSet = next;
    workspace->mSets[(next + 1u) & 1u].mCount = 0;
}
/* WWHD glue
data sGeometryInitialDiameter_l 1016C378
data sGeometryInitialTolerance_l 1016C380
ext 0273B050 void* allocateGeometryObject(u32,sead::RegistryHeap*,s32)
ext 028EFFD0 void constructGeometryGroups(void*,u32,u32,u32)
ext C00088C8 void zeroGeometryPoints(void*,u32)
ext 0273AD10 void* allocateGeometryStorage(u32)
*/
