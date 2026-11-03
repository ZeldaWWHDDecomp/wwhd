/* Based on CC0 tww JAIAnimation.cpp; WWHD call order and floating-point operations. */
#include "JSystem/JAudio/JAIAnimation.h"
struct AnimeVtableRecord { be<u32> words[12]; };
static AnimeVtableRecord animeVtable;
static AudioHeaderStatics animeHeaderStatics;
static AudioDestructorRecord animeDestructorRecord;
/* 0280146C */
void AnimationHeapName::destroy(u32 flags) {
    if (gabi::ea(this) && (flags & 1)) deleteAnimationHeapName(this);
}
/* 02801480 */
void AnimationHeapName::assureTermination() {}
/* 02801484 */
void JAIAnimeSound::playActorAnimSound(void* basic, JAInter::Actor* actor, f32 speed, u32 owner) {
    JAIAnimeSoundData* data = (JAIAnimeSoundData*)(void*)mpAsData;
    u32 index = dataCounter;
    if (index >= data->datas) {
        animeAssert("JAIAnimation.cpp", 274, "(dataCounter>=0 && dataCounter<_asdata->datas)");
        index = dataCounter;
    }
    JAIAnimeFrameSoundData* frame = data->mAfsData + index;
    u8 slot = 0;
    for (; slot < 8; ++slot) {
        if (!mSlots[slot].mbIsPlaying) break;
        JAISound* sound = mSlots[slot].mpSound;
        u32 id;
        if (sound) {
            u32 oldID = animeGetSoundID(sound);
            id = frame->mSoundID;
            if (oldID != id) continue;
        } else id = frame->mSoundID;
        if (!(id & 0xC00)) { dataCounter = dataCounter + mDataCounterInc; return; }
        break;
    }
    if (slot != 8) {
        u32 flags = frame->mPlayFlags;
        if (!(flags & 8) || mLoopCount == frame->mLoopCount) {
            u32 direction = mDataCounterInc;
            if ((direction == 1 && !(flags & 2)) || (direction == 0xFFFFFFFFu && !(flags & 1))) {
                u32 target = mVtable->startAnimSound;
                u32 id = frame->mSoundID;
                gptr<JAISound>* sound = &mSlots[slot].mpSound;
                gabi::call_ptr<void>(target, this, basic, id, sound, actor, owner);
                if (*sound) {
                    mSlots[slot].mpAfsData = frame;
                    mSlots[slot].mbIsPlaying = 1;
                    f32 volume = (f32)(u8)frame->mVolume / 127.0f;
                    animeSetVolume(*sound, volume, 0, 5);
                    f32 factor = (f32)(s32)frame->mPitchFactor;
                    f32 offset = speed - 1.0f;
                    f32 pitch = frame->mPitch;
                    pitch = gabi::fmadds(factor * offset, 0.03125f, pitch);
                    animeSetPitch(*sound, pitch, 0, 5);
                    f32 pan = (f32)(u8)frame->mPanning / 127.0f;
                    animeSetPan(*sound, pan, 0, 5);
                }
            }
        }
    }
    dataCounter = dataCounter + mDataCounterInc;
}
/* 028016F8 */
void JAIAnimeSound::setAnimSoundActor(void* basic, JAInter::Actor* actor, f32 currentFrame, f32 speed, u32 owner) {
    JAIAnimeSoundData* data = (JAIAnimeSoundData*)(void*)mpAsData;
    if (!data) return;
    u32 count = data->datas;
    bool forward = mDataCounterInc == 1;
    f32 previous = mCurrentTime;
    if (forward) {
        if (previous > currentFrame) {
            while (dataCounter < count && !(data->mAfsData[dataCounter].mStartFrame > mCurrentTime + speed))
                playActorAnimSound(basic, actor, speed, owner);
            u32 limit = mDataCounterLimit;
            u32 loops = mLoopCount;
            dataCounter = limit;
            mCurrentTime = currentFrame;
            if (loops < 256) mLoopCount = loops + 1;
        }
    } else if (previous < currentFrame) {
        while (dataCounter < count && !(data->mAfsData[dataCounter].mStartFrame < mCurrentTime - speed))
            playActorAnimSound(basic, actor, speed, owner);
        dataCounter = count - 1;
        u32 loops = mLoopCount;
        mCurrentTime = currentFrame;
        if (loops == 0xFFFFFFFFu || loops < 256) mLoopCount = loops + 1;
    }
    bool stopped = speed == 0.0f;
    for (u32 slot = 0; slot < 8; ++slot) {
        gptr<JAISound>* sound = &mSlots[slot].mpSound;
        if (mSlots[slot].mbIsPlaying != 1) continue;
        JAIAnimeFrameSoundData* frame = mSlots[slot].mpAfsData;
        if (!frame) animeAssert("JAIAnimation.cpp", forward ? 157 : 224, "_afsdata");
        u32 id = frame->mSoundID;
        if (!(id & 0xC00) && (!stopped || !(frame->mPlayFlags & 0x20))) {
            f32 start = frame->mStartFrame;
            f32 end = frame->mEndFrame;
            bool play = start == end;
            if (forward) {
                if (!play && start < end && end > currentFrame && !(start > currentFrame)) play = true;
                if (!play && start > end && (end > currentFrame || start < currentFrame)) play = true;
            } else {
                if (!play && start > end && end < currentFrame && start > currentFrame) play = true;
                if (!play && start < end && (end < currentFrame || start > currentFrame)) play = true;
            }
            if (play) {
                u32 target = mVtable->startAnimSound;
                gabi::call_ptr<void>(target, this, basic, id, sound, actor, owner);
            } else {
                mSlots[slot].mbIsPlaying = 0;
                JAISound* playing = *sound;
                if (playing) animeStopSound(playing, 0);
            }
        }
        JAISound* playing = *sound;
        if (playing) {
            u32 target = mVtable->setSpeedModifySound;
            gabi::call_ptr<void>(target, this, playing, frame, speed);
            if (frame->mPlayFlags & 0x10) {
                f32 end = frame->mEndFrame;
                if (forward ? !(end > currentFrame) : !(end < currentFrame)) {
                    animeStopSound(*sound, 1);
                }
            }
        }
    }
    if (forward) {
        while (dataCounter < count && !(data->mAfsData[dataCounter].mStartFrame > currentFrame))
            playActorAnimSound(basic, actor, speed, owner);
    } else {
        while (dataCounter < count && !(data->mAfsData[dataCounter].mStartFrame < currentFrame))
            playActorAnimSound(basic, actor, speed, owner);
    }
    mCurrentTime = currentFrame;
}
/* 02801BA8 */
void JAIAnimeSound::initActorAnimSound(JAIAnimeSoundData* data, u32 direction, f32 loopEnd) {
    mpAsData = data;
    u32 index = 0;
    if (data) {
        data = (JAIAnimeSoundData*)(void*)mpAsData;
        mDataCounterInc = direction;
        dataCounter = 0;
        u32 count = data->datas;
        for (; index < count; ++index) {
            if (!(data->mAfsData[index].mStartFrame < loopEnd)) break;
        }
        mCurrentTime = 0.0f;
        if (direction == 1) {
            dataCounter = 0;
            mDataCounterLimit = index;
            mLoopCount = 0;
        } else {
            dataCounter = index;
            mDataCounterLimit = 0;
            mLoopCount = -1;
        }
    }
    for (u32 slot = 0; slot < 8; ++slot) {
        JAISound* sound = mSlots[slot].mpSound;
        JAIAnimeFrameSoundData* frame = mSlots[slot].mpAfsData;
        if (!sound) { mSlots[slot].mbIsPlaying = 0; continue; }
        if (frame->mPlayFlags & 4) {
            animeStopSound(sound, 1);
            mSlots[slot].mbIsPlaying = 0;
        }
        u32 id = animeGetSoundID(sound);
        if (!(id & 0xC00)) {
            animeStopSound(sound, 0);
            mSlots[slot].mbIsPlaying = 0;
        } else mSlots[slot].mbIsPlaying = 2;
    }
}
/* 02801D04 */
void JAIAnimeSound::setAnimSoundVec(void* basic, cXyz* position, f32 currentFrame, f32 speed, u32 flags, u32 owner) {
    JAInter::Actor actor;
    actor.field_0x0 = position;
    actor.field_0x8 = position;
    actor.field_0x4 = position;
    actor.field_0xc = flags;
    setAnimSoundActor(basic, &actor, currentFrame, speed, owner);
}
/* 02801D3C */
void JAIAnimeSound::stop() {
    for (u32 slot = 0; slot < 8; ++slot) {
        JAISound* sound = mSlots[slot].mpSound;
        if (sound) animeStopSound(sound, 0);
    }
}
/* 02801DA0 */
JAIAnimeSound::JAIAnimeSound() {
    mReservedWords[1] = 0;
    mReservedWords[3] = 0;
    mReservedWords[0] = 0;
    mVtable = (JAIAnimeSoundVtable*)&animeVtable;
    mReservedWords[2] = 0;
    for (u32 slot = 0; slot < 8; ++slot) {
        mSlots[slot].mpSound = nullptr;
        mSlots[slot].mbIsPlaying = 0;
    }
    for (u32 i = 0; i < 2; ++i) mReservedPointers[i] = nullptr;
    mLoopCount = 0;
    dataCounter = 0;
}
/* 02801E2C */
void JAIAnimeSound::startAnimSound(void* basic, u32 id, gptr<JAISound>* sound, JAInter::Actor* actor, u32 owner) {
    // HD: the base implementation passes zero as owner, retaining the actor token in r6.
    animeStartActor(basic, id, sound, actor, 0, owner);
}
/* 02801E44 */
void JAIAnimeSound::setSpeedModifySound(JAISound* sound, JAIAnimeFrameSoundData* frame, f32 speed) {
    s32 factor = frame->mPitchFactor;
    f32 pitch = frame->mPitch;
    if (factor) {
        f32 offset = speed - 1.0f;
        f32 product = (f32)factor * offset;
        pitch = gabi::fmadds(product, 0.03125f, pitch);
    }
    animeSetPitch(sound, pitch, 0, 5);
    factor = frame->mPitchFactor;
    s16 volume = frame->mVolume;
    if (factor) {
        f32 scale = (f32)(s32)frame->mVolumeFactor;
        f32 offset = speed - 1.0f;
        scale = scale + scale;
        s16 adjustment = (s16)gabi::ftoi(scale * offset);
        volume = (s16)(volume + adjustment);
        if (volume > 127) volume = 127;
        else if (volume < 0) volume = 0;
    }
    animeSetVolume(sound, (f32)(u8)volume / 127.0f, 0, 5);
}
/* 02802000 */
void __sinit_JAIAnimation_cpp() {
    animeHeaderStatics.mWords[2] = 0;
    animeHeaderStatics.mWords[0] = 0;
    animeHeaderStatics.mWords[3] = 0;
    animeHeaderStatics.mWords[1] = 0;
    registerAnimeStatics(&animeDestructorRecord);
}
/* WWHD glue
ext 0273AF40 void deleteAnimationHeapName(AnimationHeapName*)
data animeVtable 1016F354
data animeHeaderStatics 104B4EC4
data animeDestructorRecord 101F9B1C
ext 0280B1D8 void animeStopSound(JAISound*, u32)
ext 0280D2D8 u32 animeGetSoundID(JAISound*)
ext 0280BD74 void animeSetPitch(JAISound*, f32, u32, u32)
ext 0280B68C void animeSetVolume(JAISound*, f32, u32, u32)
ext 02802844 void animeStartActor(void*, u32, gptr<JAISound>*, JAInter::Actor*, u32, u32)
str 1016F330 "JAIAnimation.cpp" in 028016F8
str 1016F344 "_afsdata"
str 1016F2E8 "JAIAnimation.cpp" in 02801484
str 1016F2FC "(dataCounter>=0 && dataCounter<_asdata->datas)"
ext 0273AA24 void animeAssert(const char*, u32, const char*)
ext 0280BA28 void animeSetPan(JAISound*, f32, u32, u32)
ext 028F026C void registerAnimeStatics(AudioDestructorRecord*)
*/
