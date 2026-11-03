// HD file-adapter text methods; reconstructed from the game's instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRNativeText.h"

/* 0284B8A4 */
void NativeFileText::destroy(u32 flags) {
    if (this != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)deleteNativeText),this);
}

/* 0284B8B8 */
static u32 nativeFileReservedQuery() {
    return 0;
}

/* 0284B8C0 */
void NativePathText::terminateBuffer() {
    *gabi::at<be<u8>>(gabi::ea(mText.get()) + u32(mCapacity) - 1) = 0;
}

/* 0284B8D8 */
void NativePathText::destroy(u32 flags) {
    if (this != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)deleteNativeText),this);
}

/* 0284B8EC */
void NativeFileText::assureTermination() {
}

/* 0284B8F0 */
void NativePathTextBase::destroy(u32 flags) {
    if (this != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)deleteNativeText),this);
}

/* 0284B904 */
void NativeFileTextBase::destroy(u32 flags) {
    if (this != NULL && (flags & 1))
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)deleteNativeText),this);
}

/* WWHD glue
 ext 0273AF40 void deleteNativeText(void*) callback
*/
