// Per-TU text adapters; canonical SafeText layouts are reused unchanged.
#pragma once
#include "sead/FileDevice.h"
struct NativeFileText : sead::SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct NativePathText : sead::SafeTextBuffer {
    void destroy(u32 flags);
    void terminateBuffer();
};
struct NativePathTextBase : sead::SafeTextBuffer { void destroy(u32 flags); };
struct NativeFileTextBase : sead::SafeText { void destroy(u32 flags); };
WWHD_SIZE(NativeFileText,8);
WWHD_SIZE(NativePathText,12);
WWHD_SIZE(NativePathTextBase,12);
WWHD_SIZE(NativeFileTextBase,8);
void deleteNativeText(void*);
