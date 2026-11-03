#ifndef WWHD_JASHEAPCTRL_H
#define WWHD_JASHEAPCTRL_H
#include "JSystem/JSupport/JSUList.h"
namespace JASystem { namespace Kernel {
struct DisposerVTable { u8 prefix[0xC]; gfn onDispose; };
struct TDisposer { gptr<DisposerVTable> mVtable; };
struct HeapMutex { u8 storage[0x2C]; };
// HD: mutex storage expanded, alignment is 64 bytes, and the heap type is absent.
class THeap {
public:
    JSUPtrList mChildren;
    JSUPtrLink mLink;
    HeapMutex mMutex;
    gptr<TDisposer> mDisposer;
    be<u32> mBase;
    be<u32> mSize;
    gptr<THeap> mTail;
    void initRootHeap(u32 base, u32 size);
    u32 getCurOffset();
    THeap* getTailHeap();
    u32 getTailOffset();
    void insertChild(THeap* heap, THeap* next, u32 base, u32 size, u32 fromTail);
    u32 alloc(THeap* mother, u32 size);
    u32 free();
};
THeap* constructAudioHeap(THeap*, TDisposer*);
}}
WWHD_OFFSET(JASystem::Kernel::THeap, mLink, 0xC);
WWHD_OFFSET(JASystem::Kernel::THeap, mMutex, 0x1C);
WWHD_OFFSET(JASystem::Kernel::THeap, mDisposer, 0x48);
WWHD_OFFSET(JASystem::Kernel::THeap, mBase, 0x4C);
WWHD_OFFSET(JASystem::Kernel::THeap, mSize, 0x50);
WWHD_OFFSET(JASystem::Kernel::THeap, mTail, 0x54);
WWHD_SIZE(JASystem::Kernel::THeap, 0x58);
WWHD_SIZE(JASystem::Kernel::HeapMutex, 0x2C);
WWHD_SIZE(JASystem::Kernel::TDisposer, 4);
#endif
