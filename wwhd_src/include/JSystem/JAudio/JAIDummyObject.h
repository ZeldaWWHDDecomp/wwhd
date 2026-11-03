/* JAudio 1 recycled sound positions. */
#pragma once
#include "JSystem/JAudio/JAIGlobalParameter.h"
namespace JAInter {
struct DummyVec {
    gptr<DummyVec> previous;
    gptr<DummyVec> next;
    gptr<void> sound;
    be<f32> position[3];
    be<u32> lifetime;
    be<u8> actorSound;
    u8 padding[3];
};
WWHD_OFFSET(DummyVec, previous, 0);
WWHD_OFFSET(DummyVec, next, 4);
WWHD_OFFSET(DummyVec, sound, 8);
WWHD_OFFSET(DummyVec, position, 12);
WWHD_OFFSET(DummyVec, lifetime, 24);
WWHD_OFFSET(DummyVec, actorSound, 28);
WWHD_SIZE(DummyVec, 32);
namespace DummyObjectMgr {
void init();
DummyVec* getPointer(u32 lifetime, u32 actorSound);
void releasePointer(DummyVec* node);
void check();
}}
void dummyAssert(const char*, u32, const char*);
void* allocateDummyArray(u32, JKRHeap*, s32);
u32 getDummyObjectMax();
void stopDummyActorSounds(JAIBasicView*, be<f32>*);
void stopDummySound(void*, u32);
void registerDummyStatics(AudioDestructorRecord*);
