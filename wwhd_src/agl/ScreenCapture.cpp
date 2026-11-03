// Screen capture request wrappers reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/ScreenCapture.h"
void lockScreenCapture(void*);
void unlockScreenCapture(void*);
void finalizeScreenCapture(sead::Disposer*, u32);
typedef gptr<void> ScreenCaptureLockPointer;
static ScreenCaptureLockPointer sBorrowedScreenCaptureLock;
static u32 sCaptureFileHandleTable;
/* 0279AF10 */
void agl::ScreenCapture::clearBusy() { mBusy = 0; }
/* 0279B4EC */
void agl::ScreenCapture::captureFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u8)mFirstPending == 0 || (u8)mBusy != 0) return;
    lockScreenCapture(sBorrowedScreenCaptureLock);
    render(mFirstSource, mFirstOptions, &mFirstPath, mFirstStorage);
    mFirstPending = 0;
    unlockScreenCapture(sBorrowedScreenCaptureLock);
}
/* 0279B568 */
void agl::ScreenCapture::captureSecond() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u8)mSecondPending == 0 || (u8)mBusy != 0) return;
    lockScreenCapture(sBorrowedScreenCaptureLock);
    render(mSecondSource, mSecondOptions, &mSecondPath, mSecondStorage);
    mSecondPending = 0;
    unlockScreenCapture(sBorrowedScreenCaptureLock);
}
/* 0279B6A0 */
void agl::screenshot_text::FileHandle::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    sead::FileDevice* sourceDevice = mSourceDevice;
    mDisposer.mVtable = gabi::at<sead::DisposerVtable>(gabi::ea(&sCaptureFileHandleTable));
    if (sourceDevice != NULL) sourceDevice->tryClose(this);
    finalizeScreenCapture(&mDisposer, 0);
    if ((flags & 1) != 0) operator_delete(this);
}
/* WWHD glue
 data sBorrowedScreenCaptureLock 101F8E04
 data sCaptureFileHandleTable 101675B8
 ext 027B72E8 void lockScreenCapture(void*)
 ext 027B7348 void unlockScreenCapture(void*)
 ext 02752BEC void finalizeScreenCapture(sead::Disposer*, u32)
*/
