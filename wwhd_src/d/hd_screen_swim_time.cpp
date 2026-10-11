/* HD swim timer blink and flash animation. Private reconstruction. */
#include "d/hd_hud_b.h"
using namespace gabi;
extern void hdHudAnimationStart(void*,u32,u32,f32);
extern void hdHudAnimationRate(void*,u32,u32,f32);
extern void hdHudAnimationRatio(void*,u32,u32,f32);
extern u32 hdHudAnimationEnded(void*,u32);
extern f32 hdHudSwimRatio(s32,s32,u8);
extern void hdHudPlaySound(u32);
extern f32 hdHudRandom(f32);
extern u32 hdHudSwimAvailable(HdHudSwim*);
static HdHudModes l_animationModes;
static HdHudState l_swimFinishedState;
static HdHudState l_swimWaitState;
static void* animation(HdHudSwim* self) {
    return self->mAnimation->mPlayer;
}

/* 026E6774 */
void hdSwimBlink(HdHudSwim* self) {
    s32 current = hdHudGetPlay()->mSwimTime;
    s32 maximum = hdHudGetPlay()->mSwimTimeMax;
    if (!maximum) maximum = current;
    s32 half = maximum / 2;
    if (current < half) {
        if (!self->mBlinking) {
            hdHudAnimationStart(animation(self),3,2,1.0f);
            self->mBlinking = 1;
        }
        f32 ratio = (f32)(s32)((u32)half-(u32)current)/(f32)half;
        f32 range = fsubs_ppc(self->mBlinkRange,1.0f);
        hdHudAnimationRate(animation(self),3,2,fmadds(ratio,range,1.0f));
    }
    u32 frame = self->mBlinkFrame+1;
    u32 period = self->mBlinkPeriod;
    self->mBlinkFrame = frame;
    // HD: both signed guards are retained, including wrapped frame/period values.
    if ((s32)frame < (s32)period || (s32)frame < (s32)(period<<1)) return;
    self->mBlinkFrame = 0;
    if (current <= 0) {
        self->mBlinkPeriod = 0;
        return;
    }
    if (current >= half) return;
    f32 ratio = hdHudSwimRatio(half,(s32)((u32)half-(u32)current),1);
    u32 cooldown = self->mSoundCooldown;
    self->mBlinkPeriod = ftoi(fmadds(-17.0f,ratio,20.0f));
    if ((s32)cooldown <= 0) {
        self->mSoundCooldown = 2;
        hdHudPlaySound(0x840);
    } else {
        self->mSoundCooldown = cooldown-1;
    }
}

/* 026E6970 */
void hdSwimFlash(HdHudSwim* self) {
    u32 delay = self->mFlashDelay;
    if (delay) {
        self->mFlashDelay = delay-1;
    } else {
        u32 available[6];
        u32 count = 0;
        for (u32 i=0;i<6;++i) {
            if (!self->mFlashing[i]) available[count++] = i;
        }
        if (count) {
            f32 choice = hdHudRandom((f32)count);
            u32 selected = choice < 2147483648.0f ? (u32)ftoi(choice) :
                (u32)ftoi(fsubs_ppc(choice,2147483648.0f))+0x80000000u;
            u32 index = available[selected];
            self->mFlashing[index] = 1;
            // HD: the pane/Y table falls back to zero for an out-of-range slot.
            gptr<HdHudPane>* slot = &self->mPanes[index<6?index:0];
            if (*slot) {
                hdHudGetPlay();
                hdHudGetPlay();
                HdHudPane* pane = *slot;
                u32 x = pane->mXBits;
                u32 z = pane->mZBits;
                f32 distance = self->mDistance;
                f32 ratio = self->mRatio;
                f32 y = self->mPaneY[index<6?index:0];
                f32 position = fnmsubs(distance,ratio,y);
                pane = *slot;
                u8 flags = pane->mFlags;
                pane->mXBits = x;
                pane->mY = position;
                pane->mFlags = flags|HD_HUD_PANE_TRANSFORM_DIRTY;
                pane->mZBits = z;
            }
            u32 animationId = index+4;
            hdHudAnimationStart(animation(self),animationId,l_animationModes.mModes[animationId],1.0f);
            self->mFlashDelay = ftoi(hdHudRandom(10.0f));
        }
    }
    for (u32 i=0;i<6;++i) {
        if (self->mFlashing[i] && hdHudAnimationEnded(animation(self),i+4)) {
            self->mFlashing[i] = 0;
        }
    }
}

/* 026E6BF8 */
void hdSwimUpdate(HdHudSwim* self) {
    s32 current = hdHudGetPlay()->mSwimTime;
    s32 maximum = hdHudGetPlay()->mSwimTimeMax;
    self->mRatio = (f32)(s32)((u32)maximum-(u32)current)/(f32)maximum;
    if (current) {
        hdSwimBlink(self);
        hdSwimFlash(self);
        HdHudPlay* play = hdHudGetPlay();
        if (play->mSwimTimerRunning) play->mSwimTime = (u32)play->mSwimTime-1;
        hdHudAnimationRatio(animation(self),10,1,self->mRatio);
        if (!hdHudGetPlay()->mSwimTime) {
            play = hdHudGetPlay();
            play->mSwimTimerRunning = 0;
            hdHudChangeState(&self->mState,&l_swimFinishedState);
        }
        if (!hdHudSwimAvailable(self)) hdHudChangeState(&self->mState,&l_swimWaitState);
    } else {
        hdHudChangeState(&self->mState,&l_swimWaitState);
    }
    gabi::call_ptr<void>(self->mVTable[35].get(),self);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_animationModes 10107044
data l_swimFinishedState 1049C874
data l_swimWaitState 1049C844
ext 025200D4 HdHudPlay* hdHudGetPlay()
ext 020053E4 void hdHudAnimationStart(void*,u32,u32,f32)
ext 02005488 void hdHudAnimationRate(void*,u32,u32,f32)
ext 0200552C void hdHudAnimationRatio(void*,u32,u32,f32)
ext 02005840 u32 hdHudAnimationEnded(void*,u32)
ext 025DB6F4 f32 hdHudSwimRatio(s32,s32,u8)
ext 025E1988 void hdHudPlaySound(u32)
ext 020198D8 f32 hdHudRandom(f32)
ext 026E6428 u32 hdHudSwimAvailable(HdHudSwim*)
*/
