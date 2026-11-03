/* Sequence track pool adapters, reconstructed from WWHD. */
#include "nw/snd/SequenceTrackPool.h"
using namespace nw::snd;
void* allocateSequenceTrack(be<u32>* pool);
SequenceTrack_l* constructSequenceTrack(void* storage);
void releaseSequenceTrack(be<u32>* pool,SequenceTrack_l* track);
void initializeSequenceTrackPool(be<u32>* pool,void* storage,u32 size,u32 stride);
void resetSequenceTrackPool(be<u32>* pool);
u32 sequenceTrackPoolAvailable(const be<u32>* pool);
/* 028AD7D0 */
SequenceTrack_l* SequenceTrackPool::allocate(void* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* storage=allocateSequenceTrack(mStorage);
    if (storage==nullptr) return nullptr;
    SequenceTrack_l* track=constructSequenceTrack(storage);
    if (track!=nullptr) {
        track->mBase.player=static_cast<SequencePlayer_l*>(owner);
        track->mPoolArgument=mArgument;
    }
    return track;
}
/* 028AD83C */
void SequenceTrackPool::release(SequenceTrack_l* track) {
    gabi::AutoFrame frame{gabi::current_function()};
    track->mBase.player=nullptr;
    if (track==nullptr) return;
    gabi::call_ptr<void>(track->mBase.vtable->destroy.function,track,2u);
    releaseSequenceTrack(mStorage,track);
}
/* 028AD8A0 */
void SequenceTrackPool::initialize(void* storage,u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeSequenceTrackPool(mStorage,storage,size,0x224);
}
/* 028AD8AC */
void SequenceTrackPool::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    resetSequenceTrackPool(mStorage);
}
/* 028AD8B4 */
u32 SequenceTrackPool::available() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return sequenceTrackPoolAvailable(mStorage);
}
/* WWHD glue
ext 0288A248 void* allocateSequenceTrack(be<u32>*)
ext 028B1E58 nw::snd::SequenceTrack_l* constructSequenceTrack(void*)
ext 0288A26C void releaseSequenceTrack(be<u32>*,nw::snd::SequenceTrack_l*)
ext 0288A184 void initializeSequenceTrackPool(be<u32>*,void*,u32,u32)
ext 0288A1D0 void resetSequenceTrackPool(be<u32>*)
ext 0288A224 u32 sequenceTrackPoolAvailable(const be<u32>*)
*/
