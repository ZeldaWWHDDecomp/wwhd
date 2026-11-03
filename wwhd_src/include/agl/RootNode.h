/* agl::detail::RootNode: the graphics library's root debug node singleton (its name text and a critical section)
 * and the node-meta helper that formats a node's meta text while a debug heap exists. Layouts from this game's
 * constructor (02785A64, sizeof 0x15C from createInstance) and the instructions of its translation unit. */
#pragma once
#include "sead/FileDevice.h"
#include "sead/Thread.h"
#include "sead/RuntimeFormatSupport.h"
#include "agl/CascadeFormattedLabel.h"
#include "agl/PrivateResource.h"

namespace agl { namespace detail {

/* sead text instances of this translation unit (only their virtual bodies are out of line) */
struct RootText : sead::SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct RootTextBuffer : sead::SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};
struct RootNameBase : sead::SafeTextBuffer { void destroy(u32 flags); };
struct RootName : sead::SafeTextBuffer { void destroy(u32 flags); };
struct RootFormatTextBase : sead::SafeTextBuffer { void destroy(u32 flags); };
struct RootSpareText : sead::SafeTextBuffer { void destroy(u32 flags); };

/* a 1024-character text built from a format and its arguments */
struct RootFormatText : sead::format::ByteTextBuffer {
    be<u8> mStorage[0x400];
    static RootFormatText* createFormatted(RootFormatText* text, const char* format, u32 firstArgument,
        u32 secondArgument, u32 thirdArgument, u32 fourthArgument, u32 fifthArgument, u32 sixthArgument);
    void destroy(u32 flags);
};

struct RootNodeVtable {
    sead::VtableSlot mHeader;
    sead::VtableSlot mReserved;
    sead::VtableSlot mDestroy;
};

struct RootNode {
    gptr<RootNodeVtable> mVtable;
    sead::RegisteredObject mDisposer;
    sead::FixedSafeText<256> mName;
    sead::CriticalSection mCriticalSection;

    static void createInstance(sead::RegistryHeap* heap);
    static RootNode* construct(RootNode* node);
    void destroy(u32 flags);
    void initialize(sead::RegistryHeap* heap, const sead::SafeText* name);
    static void setNodeMeta(void* node, const sead::SafeText* meta);
    static void setNoteMeta(u32 unusedFirst, u32 unusedSecond, void* node);
};
/* the singleton's disposer, placed after the vtable word */
struct RootNodeDisposer : sead::RegisteredObject {
    void destroy(u32 flags);
};

} }

WWHD_SIZE(agl::detail::RootFormatText, 0x40C);
WWHD_OFFSET(agl::detail::RootNodeVtable, mDestroy, 0x10);
WWHD_OFFSET(agl::detail::RootNode, mDisposer, 4);
WWHD_OFFSET(agl::detail::RootNode, mName, 0x14);
WWHD_OFFSET(agl::detail::RootNode, mCriticalSection, 0x120);
WWHD_SIZE(agl::detail::RootNode, 0x15C);
