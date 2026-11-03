/* agl::detail::PrivateResource: the graphics library's private singleton holding its work heap, the shader-compile
 * heap with the shader source text and work buffer, the archive device over the library's resource archive, and a
 * critical section. Layouts from this game's constructor (0278540C, sizeof 0x74 from createInstance) and the
 * instructions of its translation unit. */
#pragma once
#include "sead/FileDevice.h"
#include "sead/Thread.h"

namespace agl { namespace detail {

/* sead text instances of this translation unit (only their virtual bodies are out of line) */
struct ResourceText : sead::SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct ResourceTextBuffer : sead::SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};
/* text whose buffer was allocated from a heap (the shader source text) */
struct ResourceHeapText : sead::SafeTextBuffer {
    void destroy(u32 flags);
};
/* the archive device's drive name: the fixed-size base and the complete text */
struct ResourceDriveNameBase : sead::SafeTextBuffer {
    void destroy(u32 flags);
};
struct ResourceDriveName : sead::SafeTextBuffer {
    void destroy(u32 flags);
};

/* a byte buffer with its size */
struct ResourceWorkBuffer {
    be<s32> mSize;
    gptr<u8> mBuffer;
};

struct PrivateResource {
    sead::RegisteredObject mDisposer;
    gptr<sead::RegistryHeap> mWorkHeap;
    gptr<sead::RegistryHeap> mShaderBuildHeap;
    gptr<ResourceHeapText> mShaderText;
    ResourceWorkBuffer mShaderScratch;
    gptr<sead::ArchiveFileDevice> mArchiveDevice;
    gptr<sead::ArchiveRes> mArchive;
    gptr<sead::RegistryHeap> mDebugHeap;
    sead::CriticalSection mCriticalSection;
    gfn mHook;                       // an empty function by default
    gptr<sead::DisposerVtable> mVtable;

    static void createInstance(sead::RegistryHeap* heap);
    static PrivateResource* construct(PrivateResource* resource);
    void destroy(u32 flags);
    void initialize(sead::RegistryHeap* parent, sead::RegistryHeap* debugHeap, u32 workHeapSize);
    void setArchive(sead::ArchiveRes* archive);
};
/* the singleton's disposer, placed at the start of the instance */
struct PrivateResourceDisposer : sead::RegisteredObject {
    void destroy(u32 flags);
};

void defaultResourceHook();

} }

WWHD_SIZE(agl::detail::ResourceWorkBuffer, 8);
WWHD_OFFSET(agl::detail::PrivateResource, mWorkHeap, 0x10);
WWHD_OFFSET(agl::detail::PrivateResource, mShaderBuildHeap, 0x14);
WWHD_OFFSET(agl::detail::PrivateResource, mShaderText, 0x18);
WWHD_OFFSET(agl::detail::PrivateResource, mShaderScratch, 0x1C);
WWHD_OFFSET(agl::detail::PrivateResource, mArchiveDevice, 0x24);
WWHD_OFFSET(agl::detail::PrivateResource, mArchive, 0x28);
WWHD_OFFSET(agl::detail::PrivateResource, mDebugHeap, 0x2C);
WWHD_OFFSET(agl::detail::PrivateResource, mCriticalSection, 0x30);
WWHD_OFFSET(agl::detail::PrivateResource, mHook, 0x6C);
WWHD_OFFSET(agl::detail::PrivateResource, mVtable, 0x70);
WWHD_SIZE(agl::detail::PrivateResource, 0x74);
