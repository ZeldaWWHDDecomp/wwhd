/* 64-byte aligned sound storage, reconstructed from WWHD. */
#include "nw/snd/SoundMemoryArena.h"
using namespace nw::snd;
void* allocateSoundArena(u32 size);
/* 028AD8BC */
SoundMemoryArena* SoundMemoryArena::construct(SoundMemoryArena* arena) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (arena==nullptr) arena=static_cast<SoundMemoryArena*>(allocateSoundArena(sizeof(SoundMemoryArena)));
    if (arena==nullptr) return arena;
    arena->mEnd=nullptr;
    arena->mVtable=0x10187EE8u;
    arena->mState=0;
    arena->mLink.next=nullptr;
    arena->mLink.previous=nullptr;
    arena->mBegin=nullptr;
    arena->mPlayer=nullptr;
    arena->mCursor=nullptr;
    return arena;
}
/* 028AD91C */
void SoundMemoryArena::reset() { mCursor=mBegin; }
/* 028AD928 */
u32 SoundMemoryArena::initialize(void* storage,u32 size) {
    u32 begin=(gabi::ea(storage)+63)&~63u;
    u32 end=gabi::ea(storage)+size;
    if (begin>end) return 0;
    mBegin=gabi::at<be<u8>>(begin);
    mCursor=gabi::at<be<u8>>(begin);
    mEnd=gabi::at<be<u8>>(end);
    return 1;
}
/* 028AD958 */
void* SoundMemoryArena::allocate(u32 size) {
    be<u8>* cursor=mCursor;
    u32 next=gabi::ea(cursor)+size;
    be<u8>* end=mEnd;
    if (next>gabi::ea(end)) return nullptr;
    mCursor=gabi::at<be<u8>>((next+63)&~63u);
    return cursor;
}
/* WWHD glue
ext 0273AD10 void* allocateSoundArena(u32)
*/
