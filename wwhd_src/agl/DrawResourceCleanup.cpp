// Release optional value resources while preserving the original array snapshot.
#include "agl/DrawResourceCleanup.h"

namespace agl {
/* 027CCE48 */
void agl::DrawResourceCleanup_l::releaseRecordResources(DrawResourceRecord_l* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = record->mValueCount;
    DrawResourceValue_l* value = record->mValues;
    DrawResourceValue_l* end = value + count;
    while (value != end) {
        DrawReleaseResource_l* resource = value->mResource;
        if (gabi::ea(resource) != 0)
            gabi::call_ptr<void>(resource->mMethods->mDestroy.mFunction, resource, 3);
        ++value;
    }
}

/* 027CE0C0 */
void agl::DrawResourceCleanup_l::releaseLargeRecordResources(LargeDrawResourceRecord_l* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = record->mValueCount;
    DrawResourceValue_l* value = record->mValues;
    DrawResourceValue_l* end = value + count;
    while (value != end) {
        DrawReleaseResource_l* resource = value->mResource;
        if (gabi::ea(resource) != 0)
            gabi::call_ptr<void>(resource->mMethods->mDestroy.mFunction, resource, 3);
        ++value;
    }
}
}

/* WWHD glue
*/
