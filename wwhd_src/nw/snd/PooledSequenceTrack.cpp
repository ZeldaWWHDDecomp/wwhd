/* Allocation, pool dispatch and destruction recovered from assigned instructions. */
#include "nw/snd/PooledSequenceTrack.h"
using namespace nw::snd;
static SequenceTrackVtable sPooledSequenceTrackVtable;
static void* allocatePooledTrack(u32 size);
static SequenceTrack* initializeSequenceTrackBase(SequenceTrack*);
static void releasePooledTrackFromManager(void* manager,SequenceTrack_l* track,u32 argument);
static void deletePooledTrack(void*);
/* 028B1E58 */
SequenceTrack_l* constructSequenceTrack(void* memory) {
    gabi::AutoFrame frame{gabi::current_function()};
    SequenceTrack_l* track=static_cast<SequenceTrack_l*>(memory);
    if (!track) track=static_cast<SequenceTrack_l*>(allocatePooledTrack(sizeof(SequenceTrack_l)));
    if (track) {
        initializeSequenceTrackBase(&track->mBase);
        track->mBase.vtable=&sPooledSequenceTrackVtable;
    }
    return track;
}
/* 028B1EAC */
void releasePooledSequenceTrack(SequenceTrack_l* track,u32 argument) {
    gabi::AutoFrame frame{gabi::current_function()};
    releasePooledTrackFromManager(gabi::at<void>(track->mPoolArgument),track,argument);
}
/* 028B1EC0 */
void destroyPooledSequenceTrack(SequenceTrack_l* track,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (track) {
        SequenceTrack::destroy(&track->mBase,0);
        if (flags&1) deletePooledTrack(track);
    }
}
/* WWHD glue
data sPooledSequenceTrackVtable 10188094
ext 0273AD10 void* allocatePooledTrack(u32)
ext 0288D158 nw::snd::SequenceTrack* initializeSequenceTrackBase(nw::snd::SequenceTrack*)
ext 028AAB94 void releasePooledTrackFromManager(void*,nw::snd::SequenceTrack_l*,u32)
ext 0288DCF8 void nw::snd::SequenceTrack::destroy(nw::snd::SequenceTrack*,u32)
ext 0273AF40 void deletePooledTrack(void*)
*/
