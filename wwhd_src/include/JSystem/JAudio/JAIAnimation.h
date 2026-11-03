/* JAudio 1 animation sound state. Reuses the merged BAS entry layouts. */
#pragma once
#include "JAZelAudio/JAIZelAnime.h"
#include "JSystem/JAudio/JAIGlobalParameter.h"
struct JAIAnimeSoundSlot {
    be<u8> mbIsPlaying;
    u8 padding[3];
    gptr<JAISound> mpSound;
    gptr<JAIAnimeFrameSoundData> mpAfsData;
};
struct JAIAnimeSoundVtable {
    be<u32> reserved[9];
    be<u32> startAnimSound;
    be<u32> reservedSpeed;
    be<u32> setSpeedModifySound;
};
WWHD_OFFSET(JAIAnimeSoundVtable, startAnimSound, 0x24);
WWHD_OFFSET(JAIAnimeSoundVtable, setSpeedModifySound, 0x2C);
WWHD_SIZE(JAIAnimeSoundVtable, 0x30);
class JAIAnimeSound {
public:
    JAIAnimeSound();
    void initActorAnimSound(JAIAnimeSoundData*, u32 direction, f32 loopEnd);
    void setAnimSoundVec(void* basic, cXyz* position, f32 currentFrame, f32 speed, u32 flags, u32 owner);
    void setAnimSoundActor(void* basic, JAInter::Actor*, f32 currentFrame, f32 speed, u32 owner);
    void playActorAnimSound(void* basic, JAInter::Actor*, f32 speed, u32 owner);
    void startAnimSound(void* basic, u32 id, gptr<JAISound>*, JAInter::Actor*, u32 owner);
    void setSpeedModifySound(JAISound*, JAIAnimeFrameSoundData*, f32 speed);
    void stop();
    JAIAnimeSoundSlot mSlots[8];
    be<u32> mReservedWords[4];
    gptr<void> mReservedPointers[2];
    be<u32> mDataCounterInc;
    be<s32> mDataCounterLimit;
    be<u32> dataCounter;
    be<u32> mLoopCount;
    be<f32> mCurrentTime;
    u8 padding[4];
    gptr<void> mpAsData;
    gptr<JAIAnimeSoundVtable> mVtable;
};
WWHD_OFFSET(JAIAnimeSoundSlot, mpSound, 4);
WWHD_SIZE(JAIAnimeSoundSlot, 12);
WWHD_SIZE(JAInter::Actor, 16);
WWHD_OFFSET(JAIAnimeSound, mDataCounterInc, 0x78);
WWHD_OFFSET(JAIAnimeSound, dataCounter, 0x80);
WWHD_OFFSET(JAIAnimeSound, mpAsData, 0x90);
WWHD_OFFSET(JAIAnimeSound, mVtable, 0x94);
WWHD_SIZE(JAIAnimeSound, 0x98);
void animeStopSound(JAISound*, u32);
u32 animeGetSoundID(JAISound*);
void animeSetPitch(JAISound*, f32, u32, u32);
void animeSetVolume(JAISound*, f32, u32, u32);
void animeStartActor(void*, u32, gptr<JAISound>*, JAInter::Actor*, u32, u32);
void registerAnimeStatics(AudioDestructorRecord*);

void animeAssert(const char*, u32, const char*);
void animeSetPan(JAISound*, f32, u32, u32);

// HD: heap-name SafeString helper instance emitted at this TU boundary.
struct AnimationHeapName {
    gptr<const char> name;
    be<u32> vtable;
    void destroy(u32 flags);
    void assureTermination();
};
WWHD_SIZE(AnimationHeapName, 8);
void deleteAnimationHeapName(AnimationHeapName*);
