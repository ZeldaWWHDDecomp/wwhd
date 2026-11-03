/* Based on CC0 tww JAIDummyObject.cpp; WWHD lists retain alias-sensitive stores. */
#include "JSystem/JAudio/JAIDummyObject.h"
namespace JAInter { namespace DummyObjectMgr {
static DummyVec* deadObjectFreePointer;
static DummyVec* deadObjectUsedPointer;
static DummyVec* deadObjectObject;
static JKRHeap* msCurrentHeap;
static JAIBasicView* msBasic;
}}
using namespace JAInter::DummyObjectMgr;
using JAInter::DummyVec;
static AudioHeaderStatics dummyHeaderStatics;
static AudioDestructorRecord dummyDestructorRecord;
namespace JAInter { namespace DummyObjectMgr {
/* 02803E78 */
void init() {
    JKRHeap* heap = msCurrentHeap;
    if (!heap) { dummyAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
    u32 count = getDummyObjectMax();
    DummyVec* objects = (DummyVec*)allocateDummyArray(count * 32, heap, 32);
    deadObjectObject = objects;
    if (!objects) {
        dummyAssert("JAIDummyObject.cpp", 37, "deadObjectObject");
        objects = deadObjectObject;
    }
    deadObjectFreePointer = objects;
    deadObjectUsedPointer = nullptr;
    objects[0].previous = nullptr;
    objects = deadObjectObject;
    objects[0].next = objects + 1;
    u32 index = 1;
    count = getDummyObjectMax();
    objects = deadObjectObject;
    objects[1].previous = objects;
    while (index < count - 1) {
        objects = deadObjectObject;
        objects[index].next = objects + index + 1;
        ++index;
        count = getDummyObjectMax();
        objects = deadObjectObject;
        objects[index].previous = objects + index - 1;
    }
    deadObjectObject[index].next = nullptr;
}
/* 02803FB0 */
DummyVec* getPointer(u32 lifetime, u32 actorSound) {
    DummyVec* node = deadObjectFreePointer;
    if (!node) return nullptr;
    DummyVec* used = deadObjectUsedPointer;
    deadObjectFreePointer = node->next;
    if (used) {
        node->next = used;
        deadObjectUsedPointer->previous = node;
        node->previous = nullptr;
    } else {
        node->previous = nullptr;
        node->next = nullptr;
    }
    deadObjectUsedPointer = node;
    node->lifetime = lifetime;
    node->actorSound = actorSound;
    node->sound = nullptr;
    return node;
}
/* 02804028 */
void releasePointer(DummyVec* node) {
    DummyVec* used = deadObjectUsedPointer;
    DummyVec* next = node->next;
    if (used != node) {
        node->previous->next = next;
        next = node->next;
        if (next) next->previous = node->previous;
    } else {
        deadObjectUsedPointer = next;
        next = node->next;
        if (next) next->previous = nullptr;
    }
    DummyVec* free = deadObjectFreePointer;
    node->next = free;
    if (free) free->previous = node;
    deadObjectFreePointer = node;
}
/* 02804094 */
void check() {
    DummyVec* node = deadObjectUsedPointer;
    while (node) {
        DummyVec* next = node->next;
        u32 lifetime = node->lifetime - 1;
        node->lifetime = lifetime;
        if (!lifetime) {
            if (node->actorSound) {
                stopDummyActorSounds(msBasic, node->position);
                releasePointer(node);
            } else {
                void* sound = node->sound;
                if (sound) stopDummySound(sound, 0);
                releasePointer(node);
            }
        } else if (!node->actorSound && !node->sound) {
            releasePointer(node);
        }
        node = next;
    }
}
}}
/* 02804154 */
void __sinit_JAIDummyObject_cpp() {
    dummyHeaderStatics.mWords[2] = 0;
    dummyHeaderStatics.mWords[0] = 0;
    dummyHeaderStatics.mWords[3] = 0;
    dummyHeaderStatics.mWords[1] = 0;
    registerDummyStatics(&dummyDestructorRecord);
}
/* WWHD glue
data deadObjectFreePointer 104B4FA0
data deadObjectUsedPointer 104B4FA4
data deadObjectObject 104B4FA8
data msCurrentHeap 101F9B54
data msBasic 104B4F14
data dummyHeaderStatics 104B4F90
data dummyDestructorRecord 101F9B90
str 1016F58C "JAIBasic.h"
str 1016F57C "msCurrentHeap"
str 1016F598 "JAIDummyObject.cpp"
str 1016F5AC "deadObjectObject"
ext 0273AA24 void dummyAssert(const char*, u32, const char*)
ext 0273B0D4 void* allocateDummyArray(u32, JKRHeap*, s32)
ext 028047CC u32 getDummyObjectMax()
ext 02802994 void stopDummyActorSounds(JAIBasicView*, be<f32>*)
ext 0280B1D8 void stopDummySound(void*, u32)
ext 028F026C void registerDummyStatics(AudioDestructorRecord*)
*/
