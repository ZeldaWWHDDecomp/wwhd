/* Interpolation helpers, following CC0 tww JAISound.cpp and WWHD instruction order. */
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAISoundParams_helpers.h"
using namespace JAInter;
static JAISound* linkBuffer(LinkSound* link) { return static_cast<JAISound*>(static_cast<void*>(link->Buffer)); }
static void allocateLinkPosition(LinkSound* link,u32 index,u32 line,const char* condition) {
    JKRHeap* heap=audioCurrentHeap;
    if(!heap) { soundLinkAssert("JAIBasic.h",169,"msCurrentHeap");heap=audioCurrentHeap; }
    JAISound* sound=linkBuffer(link)+index;
    u32 cameras=soundLinkCameraMax();
    sound->positionInfo=static_cast<SePositionInfo*>(soundLinkAllocate(cameras*sizeof(SePositionInfo),heap,32));
    if(!linkBuffer(link)[index].positionInfo) soundLinkAssert("JAISound.cpp",line,condition);
}
namespace JAInter {
/* 0280D90C */
void LinkSound::init(JAISound* objects,u32 count) {
    soundLinkLock(criticalSection);
    soundLinkCameraMax();
    Buffer=objects;usedEnd=nullptr;freeStart=objects;
    objects->previous=nullptr;
    linkBuffer(this)->next=linkBuffer(this)+1;
    linkBuffer(this)->state=0;
    allocateLinkPosition(this,0,2614,"Buffer[0x00].mPositionInfo");
    linkBuffer(this)[1].previous=linkBuffer(this);
    u32 index=1;
    for(;index<count-1;++index) {
        linkBuffer(this)[index].next=linkBuffer(this)+index+1;
        linkBuffer(this)[index].state=0;
        allocateLinkPosition(this,index,2622,"Buffer[i].mPositionInfo");
        linkBuffer(this)[index+1].previous=linkBuffer(this)+index;
    }
    linkBuffer(this)[index].next=nullptr;
    linkBuffer(this)[index].state=0;
    allocateLinkPosition(this,index,2629,"Buffer[i].mPositionInfo");
    soundLinkUnlock(criticalSection);
}
/* 02808100 */
void MoveParaSet::init() { mMoveCounter=0;mCurrentValue=1.0f;mTargetValue=1.0f; }
/* 0280B20C */
s32 MoveParaSet::set(f32 value,u32 frames) {
    if(mMoveCounter==0) {
        if(static_cast<f32>(mCurrentValue)==value) return 2;
    } else if(static_cast<f32>(mTargetValue)==value) return 2;
    mTargetValue=value;
    if(frames==0) { mCurrentValue=value;return 0; }
    f32 amount=static_cast<f32>(mCurrentValue)-value;
    if(frames!=1) amount=amount/static_cast<f32>(frames);
    mMoveAmount=amount;mMoveCounter=frames;
    return 1;
}
/* 0280D8C0 */
s32 MoveParaSet::move() {
    u32 count=mMoveCounter;
    if(count==0) return 0;
    mMoveCounter=--count;
    if(count) { mCurrentValue=static_cast<f32>(mCurrentValue)-static_cast<f32>(mMoveAmount);return 1; }
    mCurrentValue=mTargetValue;
    return 0;
}
/* 0280DB3C */
JAISound* LinkSound::getSound() {
    soundLinkLock(criticalSection);
    JAISound* sound=static_cast<JAISound*>(static_cast<void*>(freeStart));
    if(sound) {
        freeStart=sound->next;
        JAISound* used=static_cast<JAISound*>(static_cast<void*>(usedEnd));
        if(used) {
            sound->next=used;
            used=static_cast<JAISound*>(static_cast<void*>(usedEnd));
            used->previous=sound;
            sound->previous=nullptr;
            usedEnd=sound;
        } else {
            sound->previous=nullptr;sound->next=nullptr;usedEnd=sound;
        }
    }
    soundLinkUnlock(criticalSection);
    return sound;
}
/* 0280DBE0 */
void LinkSound::releaseSound(JAISound* sound) {
    soundLinkLock(criticalSection);
    sound->owner=nullptr;
    if(static_cast<void*>(usedEnd)!=sound) {
        JAISound* previous=sound->previous;
        if(previous) previous->next=sound->next;
        JAISound* next=sound->next;
        if(next) next->previous=sound->previous;
    } else {
        usedEnd=sound->next;
        JAISound* next=sound->next;
        if(next) next->previous=nullptr;
    }
    JAISound* free=static_cast<JAISound*>(static_cast<void*>(freeStart));
    sound->next=free;
    if(free) free->previous=sound;
    freeStart=sound;
    soundLinkUnlock(criticalSection);
}
}
/* HD: compiler-generated deleting wrapper for the critical-section-bearing sound list. */
/* 0280AE74 */
static void destroySoundLink(JAInter::LinkSound* link,s32 flags) {
    if(link) {
        soundLinkDestroyCritical(link->criticalSection,2);
        if(flags&1) soundLinkDelete(link);
    }
}
/* WWHD glue
ext 027601BC void soundLinkLock(void*)
ext 028047F0 u32 soundLinkCameraMax()
ext 0273B0D4 void* soundLinkAllocate(u32,JKRHeap*,s32)
ext 0273AA24 void soundLinkAssert(const char*,u32,const char*)
str 1016FBE0 "msCurrentHeap"
str 1016FBF0 "JAIBasic.h"
str 1016FC0C "Buffer[0x00].mPositionInfo"
str 1016FBFC "JAISound.cpp"
str 1016FBC8 "Buffer[i].mPositionInfo"
ext 027601F0 void soundLinkUnlock(void*)
ext 02760168 void soundLinkDestroyCritical(void*,s32)
ext 0273AF40 void soundLinkDelete(void*)
*/
