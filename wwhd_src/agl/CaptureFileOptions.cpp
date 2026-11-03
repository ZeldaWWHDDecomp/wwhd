#include "agl/CaptureFileOptions.h"
#include "agl/CaptureProvider.h"
namespace agl {
static sead::SafeTextVtable sCaptureOptionsTextTable;
/* 027C57E8 */
CaptureFileOptions* constructCaptureFileOptions(CaptureFileOptions* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) {
        object = static_cast<CaptureFileOptions*>(allocateCaptureStorage(sizeof(CaptureFileOptions)));
        if (!object) return nullptr;
    }
    object->mExtension.mVtable = &sCaptureOptionsTextTable;
    object->mSuggestedName.mVtable = &sCaptureOptionsTextTable;
    be<u8>* empty = sFileDeviceEmptyText.mText;
    object->mSuggestedName.mText = empty;
    object->mExtension.mText = empty;
    sead::SafeText* id = &object->mDialogId;
    if (gabi::ea(id) == 0)
        id = static_cast<sead::SafeText*>(allocateCaptureStorage(sizeof(sead::SafeText)));
    if (id) {
        id->mVtable = &sCaptureOptionsTextTable;
        id->mText.v = gabi::guest_str("agl_default");
    }
    object->mPath.mVtable = &sCaptureOptionsTextTable;
    object->mPath.mText = empty;
    object->mOutputPath = nullptr;
    object->mTrailingValue = 32;
    return object;
}
}
/* WWHD glue
data sCaptureOptionsTextTable 1016A368
str 1016A464 "agl_default"
*/
