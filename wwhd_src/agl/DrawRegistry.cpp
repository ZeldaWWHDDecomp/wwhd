// Own registry lifetime and initializer; child constructor callbacks are not reimplemented here.
#include "agl/DrawRegistry.h"
void* allocateDrawRegistryScalar(u32 bytes);
void deleteDrawRegistryStorage(void* storage);
void releaseDrawRegistryArray(void* storage);
void* allocateDrawRegistry(u32 bytes, sead::AllocationHeap* heap, s32 alignment);
void* allocateDrawRegistryColor(u32 bytes, sead::AllocationHeap* heap, s32 alignment);
void registerDrawRegistry(sead::Disposer* object, sead::AllocationHeap* heap, u32 option);
void unregisterDrawRegistry(sead::Disposer* object, u32 flags);
agl::DrawRegistryRecord_l* constructDrawRegistryRecord(agl::DrawRegistryRecord_l* record);
void destroyDrawRegistryRecord(agl::DrawRegistryRecord_l* record, u32 flags);
void constructDrawRegistryArray(void* array, u32 count, u32 stride, u32 callback);
void destroyDrawRegistryArray(void* array, u32 count, u32 stride, u32 callback, u32 option, u32 extra);
void setDrawRegistryRecordData(agl::DrawRegistryRecord_l* record, void* data, u32 offset, u32 size);
void setDrawRegistryRecordFormat(agl::DrawRegistryRecord_l* record, u32 index, u32 format, u32 option, u32 extra);
namespace agl {
struct DrawRegistryStorage_l { gptr<DrawRegistry_l> mObject; };
struct DrawRegistryRegistrationStorage_l { gptr<DrawRegistryRegistration_l> mObject; };
struct DrawRegistryColor_l { be<f32> mValues[4]; };
static DrawRegistryStorage_l sDrawRegistry;
static DrawRegistryRegistrationStorage_l sDrawRegistryRegistration;
static sead::DisposerVtable sDrawRegistryTable;
static sead::DisposerVtable sDrawRegistryRegistrationTable;
/* 027CB85C */
agl::DrawRegistry_l* agl::DrawRegistry_l::construct(DrawRegistry_l* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(object) == 0) {
        object = static_cast<DrawRegistry_l*>(allocateDrawRegistryScalar(0x410));
        if (gabi::ea(object) == 0)
            return object;
    }
    object->mMethods = &sDrawRegistryTable;
    DrawRegistryRecord_l* records = object->mRecords;
    if (gabi::ea(records) == 0) {
        records = static_cast<DrawRegistryRecord_l*>(allocateDrawRegistryScalar(0x3FC));
        if (gabi::ea(records) == 0)
            return object;
    }
    constructDrawRegistryArray(records, 3, 0x154, gabi::guest_fn((const void*)constructDrawRegistryRecord));
    return object;
}
/* 027CB8D4 */
agl::DrawRegistry_l* agl::DrawRegistry_l::get(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DrawRegistry_l* object = sDrawRegistry.mObject;
    if (gabi::ea(object) == 0) {
        object = static_cast<DrawRegistry_l*>(allocateDrawRegistry(0x410, heap, 4));
        if (gabi::ea(object) != 0) {
            registerDrawRegistry(&object->mRegistration, heap, 3);
            object->mRegistration.mVtable = &sDrawRegistryRegistrationTable;
        }
        sDrawRegistryRegistration.mObject = gabi::at<DrawRegistryRegistration_l>(gabi::ea(object));
        if (gabi::ea(object) != 0)
            object = construct(object);
        sDrawRegistry.mObject = object;
    }
    return object;
}
/* 027CB968 */
void agl::DrawRegistry_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0) {
        mMethods = &sDrawRegistryTable;
        DrawRegistryRecord_l* begin = mRecords;
        DrawRegistryRecord_l* end = begin + 3;
        DrawRegistryRecord_l* record = begin;
        if (record != end) {
            do {
                void* data = record->mStorage;
                if (gabi::ea(data) != 0) {
                    data = record->mStorage;
                    releaseDrawRegistryArray(data);
                }
                ++record;
            } while (record != end);
        }
        destroyDrawRegistryArray(begin, 3, 0x154, gabi::guest_fn((const void*)destroyDrawRegistryRecord), 0, 0);
        if (flags & 1)
            deleteDrawRegistryStorage(this);
    }
}
/* 027CBA08 */
void agl::DrawRegistry_l::initialize(sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DrawRegistryRecord_l* begin = mRecords;
    DrawRegistryRecord_l* end = begin + 3;
    DrawRegistryRecord_l* record = begin;
    if (record != end) {
        do {
            u32 index = (gabi::ea(record) - gabi::ea(begin)) / 0x154;
            void* data = nullptr;
            if (index <= 2) {
                DrawRegistryColor_l* color = static_cast<DrawRegistryColor_l*>(allocateDrawRegistryColor(0x10, heap, 4));
                data = color;
                if (gabi::ea(color) != 0) {
                    f32 rgb = index == 0 ? 1.0f : 0.0f;
                    f32 alpha = index == 2 ? 0.0f : 1.0f;
                    color->mValues[1] = rgb;
                    color->mValues[2] = rgb;
                    color->mValues[3] = alpha;
                    color->mValues[0] = rgb;
                }
            }
            DrawRegistryRecord_l* target = begin + index;
            setDrawRegistryRecordData(target, data, 0, 0x10);
            setDrawRegistryRecordFormat(target, 0, 0x813, 0, 0);
            ++record;
        } while (record != end);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027CBC28 */
void agl::DrawRegistryRegistration_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0) {
        mVtable = &sDrawRegistryRegistrationTable;
        DrawRegistryRegistration_l* registered = sDrawRegistryRegistration.mObject;
        if (this == registered) {
            DrawRegistry_l* object = sDrawRegistry.mObject;
            sDrawRegistryRegistration.mObject = nullptr;
            gabi::call_ptr<void>(object->mMethods->mDestroy.mFunction, object, u32(2));
            sDrawRegistry.mObject = nullptr;
        }
        unregisterDrawRegistry(this, 0);
        if (flags & 1)
            deleteDrawRegistryStorage(this);
    }
}
}
/* WWHD glue
data sDrawRegistry 101F9604
data sDrawRegistryRegistration 101F9608
data sDrawRegistryTable 1016A7E8
data sDrawRegistryRegistrationTable 1016A7D8
ext 0273AD10 void* allocateDrawRegistryScalar(u32 bytes)
ext 0273AF40 void deleteDrawRegistryStorage(void* storage)
ext 0273AFC8 void releaseDrawRegistryArray(void* storage)
ext 0273B0D4 void* allocateDrawRegistry(u32 bytes, sead::AllocationHeap* heap, s32 alignment)
ext 0273B050 void* allocateDrawRegistryColor(u32 bytes, sead::AllocationHeap* heap, s32 alignment)
ext 02752B0C void registerDrawRegistry(sead::Disposer* object, sead::AllocationHeap* heap, u32 option)
ext 02752BEC void unregisterDrawRegistry(sead::Disposer* object, u32 flags)
ext 027B5BD8 agl::DrawRegistryRecord_l* constructDrawRegistryRecord(agl::DrawRegistryRecord_l* record) callback
ext 027B5CBC void destroyDrawRegistryRecord(agl::DrawRegistryRecord_l* record, u32 flags) callback
ext 028EFFD0 void constructDrawRegistryArray(void* array, u32 count, u32 stride, u32 callback)
ext 028F0164 void destroyDrawRegistryArray(void* array, u32 count, u32 stride, u32 callback, u32 option, u32 extra)
ext 027B5CFC void setDrawRegistryRecordData(agl::DrawRegistryRecord_l* record, void* data, u32 offset, u32 size)
ext 027B5D60 void setDrawRegistryRecordFormat(agl::DrawRegistryRecord_l* record, u32 index, u32 format, u32 option, u32 extra)
*/
