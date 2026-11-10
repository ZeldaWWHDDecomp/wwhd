/* Geometry workspace ownership and compiler-generated storage hooks. */
#include "agl/gx2/GeometryStorage.h"
#include "agl/gx2/GeometryProjection.h"
#include "sead/ElementAt.h"
using namespace agl::gx2util;
extern void* graphicsAllocate(u32 size);
extern void graphicsDelete(void* object);
extern GeometryHeap* findGeometryHeap(void* manager, void* address);

/* 027DA4C8 */
static void destroyGeometryOwner(GeometryOwner* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) return;
    s32 capacity = object->mCapacity;
    gptr<GeometryAllocation>* entries = object->mEntries;
    for (s32 index = 0; index < capacity; ++index) {
        GeometryAllocation* allocation = entries[(u32)index < (u32)capacity ? index : 0];
        if (allocation != NULL) {
            void* data = allocation->mData;
            if (data != NULL) {
                GeometryHeap* heap = findGeometryHeap(sRegistryManager, data);
                u32 release = heap->dispatch()->mRelease.mFunction;
                gabi::call_ptr<void>(release, heap, allocation->mData.get());
                allocation->mCapacity = 0;
                allocation->mData = NULL;
            }
            graphicsDelete(allocation);
            capacity = object->mCapacity;
            entries = object->mEntries;
        }
    }
    if (entries != NULL) {
        GeometryHeap* heap = findGeometryHeap(sRegistryManager, entries);
        u32 release = heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(release, heap, object->mEntries.get());
        object->mCapacity = 0;
        object->mEntries = NULL;
    }
    if (flags & 1) graphicsDelete(object);
}

/* 027DA5E4 */
static GeometryTriple* constructGeometryTriple(GeometryTriple* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<GeometryTriple*>(graphicsAllocate(sizeof(GeometryTriple)));
    return object;
}

/* These inline text interfaces are tied to deleting-destructor and termination
 * table slots. Their concrete source owners are unproven; names describe roles. */
/* 027DA610 */
static void destroyGeometryTextView(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) graphicsDelete(object);
}

/* 027DA624 */
static void finishDetachedGeometryText(void* object) {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 027DA628 */
void agl::gx2util::BufferedGeometryText::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
    be<char>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 027DA640 */
static void destroyBufferedGeometryText(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) graphicsDelete(object);
}

/* 027DA654 */
static void destroyGeometryTextBuffer(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) graphicsDelete(object);
}

/* 027DA668 */
static void destroyGeometryByteBuffer(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) graphicsDelete(object);
}

/* 027DA67C */
static void destroyUnboundGeometryText(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) graphicsDelete(object);
}

/* 027DA690 */
static void finishUnboundGeometryText(void* object) {
    gabi::AutoFrame frame{gabi::current_function()};
}



/* 027D9214 */
void agl::gx2util::GeometryPointGroups::swap(u32 first, u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 capacity = mCapacity;
    auto entries = mEntries.get();
    GeometryPointList* saved = sead::elementAt(entries, first < capacity ? first : 0); /* guest EA arithmetic: wraps at 32 bits as on the console */
    auto target = sead::elementPtr(entries, first < capacity ? first : 0);
    auto source = sead::elementPtr(entries, second < capacity ? second : 0);
    *target = source->get();
    capacity = mCapacity;
    entries = mEntries;
    sead::elementAt(entries, second < capacity ? second : 0) = saved;
}

/* 027D9294 */
s32 agl::gx2util::GeometryPointGroups::moveMatchingPair(const void* query) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = mCount;
    if (count <= 0) return -1;
    s32 index = count - 1;
    auto entries = mEntries.get();
    u32 capacity = mCapacity;
    for (s32 remaining = count; remaining != 0; --remaining, --index) {
        GeometryPointList* entry = entries[(u32)index < capacity ? index : 0];
        if ((s32)entry->mCount == 2) {
            s32 result = findPoint(entry, static_cast<const GeometryTriple*>(query));
            if (result >= 0) {
                swap(index, count - 1);
                return result;
            }
        }
    }
    return -1;
}

/* 027D9150 */
s32 agl::gx2util::GeometryPointGroups::findPoint(GeometryPointList* points, const GeometryTriple* query) const {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = points->mCount;
    if (count <= 0) return -1;
    u32 capacity = points->mCapacity;
    f32 tolerance = mTolerance;
    f32 queryX = query->x;
    auto data = static_cast<GeometryTriple*>(points->mData.get());
    for (s32 index = 0; index < count; ++index) {
        GeometryTriple* point = data + ((u32)index < capacity ? index : 0);
        f32 dx = gabi::fsubs_ppc(point->x, queryX);
        f32 lower = -tolerance;
        if (lower > dx || dx > tolerance) continue;
        f32 dy = gabi::fsubs_ppc(point->y, query->y);
        if (lower > dy || dy > tolerance) continue;
        f32 dz = gabi::fsubs_ppc(point->z, query->z);
        if (lower > dz) continue;
        if (!(dz > tolerance)) return index;
    }
    return -1;
}

extern GeometryPointList* takeGeometrySlot(void*);
static GeometryTriple* geometryListPoint(GeometryPointList* list, u32 index) {
    auto data = static_cast<GeometryTriple*>(list->mData.get());
    return data + (index < (u32)list->mCapacity ? index : 0);
}
static void appendGeometryPoint(GeometryPointList* list, const GeometryTriple* source) {
    u32 count = list->mCount;
    u32 capacity = list->mCapacity;
    auto data = static_cast<GeometryTriple*>(list->mData.get());
    list->mCount = count + 1;
    f32 x = source->x;
    GeometryTriple* target = data + (count < capacity ? count : 0);
    target->x = x;
    target->y = source->y;
    target->z = source->z;
}

/* 027D935C */
u32 agl::gx2util::GeometryPointGroups::joinPairs(GeometryGroupSet* destination) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = mCount;
    if (count < 3) return 0;
    s32 index = count - 1;
    auto entries = mEntries.get();
    u32 capacity = mCapacity;
    for (; index > 0; --index) {
        GeometryPointList* point = entries[(u32)index < capacity ? index : 0];
        if ((s32)point->mCount == 2) break;
    }
    count = index + 1;
    mCount = count;
    if (count < 3) return 0;
    GeometryPointList* output = takeGeometrySlot(destination);
    if (output == NULL) return 0;
    u32 last = (u32)mCount - 1;
    GeometryPointList* pair = mEntries[last < (u32)mCapacity ? last : 0];
    auto first = static_cast<GeometryTriple*>(pair->mData.get());
    appendGeometryPoint(output, first);
    appendGeometryPoint(output, geometryListPoint(pair, 1));
    s32 remaining = (s32)mCount - 1;
    mCount = remaining;
    u32 outputLast = (u32)output->mCount - 1;
    while (remaining > 0) {
        s32 matched = moveMatchingPair(geometryListPoint(output, outputLast));
        if (matched >= 0) {
            last = (u32)mCount - 1;
            pair = mEntries[last < (u32)mCapacity ? last : 0];
            s32 other = (s32)((u32)matched + 1) % 2;
            appendGeometryPoint(output, geometryListPoint(pair, other));
        }
        remaining = (s32)mCount - 1;
        mCount = remaining;
        outputLast = (u32)output->mCount - 1;
    }
    output->mCount = outputLast;
    return 1;
}

/* WWHD glue
 ext 02755FEC agl::gx2util::GeometryHeap* findGeometryHeap(void*, void*)
*/
