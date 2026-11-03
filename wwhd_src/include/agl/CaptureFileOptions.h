/* Capture file options, from own constructor and assigned dialog/write callers. */
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct CaptureFileOptions {
    sead::SafeText mExtension;
    sead::SafeText mSuggestedName;
    sead::SafeText mDialogId;
    sead::SafeText mPath;
    gptr<sead::SafeTextBuffer> mOutputPath;
    be<u32> mTrailingValue; // initializedto32; no read in the assigned bodies
};
WWHD_OFFSET(CaptureFileOptions, mSuggestedName, 8);
WWHD_OFFSET(CaptureFileOptions, mDialogId, 0x10);
WWHD_OFFSET(CaptureFileOptions, mPath, 0x18);
WWHD_OFFSET(CaptureFileOptions, mOutputPath, 0x20);
WWHD_OFFSET(CaptureFileOptions, mTrailingValue, 0x24);
WWHD_SIZE(CaptureFileOptions, 0x28);
CaptureFileOptions* constructCaptureFileOptions(CaptureFileOptions* object);
}
