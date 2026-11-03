/* Fading state machine, following tww JUTFader.
 * HD: control updates state without submitting a fullscreen GX rectangle. */
#include "JSystem/JUtility/JUTFader.h"
struct StaticState { be<u32> words[4]; };
static StaticState sStaticState;
static u8 sStaticRecord;
static JUTFaderVtable sFaderVtable;
/* 027ECD78 */
JUTFader* JUTFader__ct(JUTFader* fader,s32 x,s32 y,s32 width,s32 height,const J2DColor* color) {
    u32 rgba=color->rgba;
    if(!fader) fader=static_cast<JUTFader*>(faderAllocate(sizeof(JUTFader)));
    if(!fader) return nullptr;
    fader->mVtable=&sFaderVtable;
    f32 right=static_cast<f32>(static_cast<s32>(static_cast<u32>(x)+static_cast<u32>(width)));
    f32 bottom=static_cast<f32>(static_cast<s32>(static_cast<u32>(y)+static_cast<u32>(height)));
    fader->mColor.rgba=rgba;
    J2DBox* box=&fader->mBox;
    if(gabi::ea(box)==0) box=static_cast<J2DBox*>(faderAllocate(sizeof(J2DBox)));
    if(box) { box->i.x=static_cast<f32>(x); box->i.y=static_cast<f32>(y); box->f.x=right; box->f.y=bottom; }
    fader->mStatus=JUTFader::WaitOut; fader->mDelayStatus=JUTFader::WaitOut; fader->mDelayTimer=-1;
    fader->mFadeTime=0; fader->mTimer=0;
    return fader;
}
/* 027ECED4 */
void JUTFader::control() {
    s32 delay=mDelayTimer;
    if(delay>=0) { mDelayTimer=static_cast<s32>(static_cast<u32>(delay)-1); if(delay==0) mStatus=static_cast<u32>(mDelayStatus); }
    u32 status=mStatus;
    if(status==WaitIn) return;
    switch(status) {
    case WaitOut: mColor.rgba=(mColor.rgba & 0xFFFFFF00u)|255; break;
    case FadeIn: {
        u16 timer=static_cast<u16>(mTimer+1); u32 time=mFadeTime;
        s32 quotient=time?static_cast<s32>(timer*255u)/static_cast<s32>(time):0;
        mTimer=timer; mColor.rgba=(mColor.rgba & 0xFFFFFF00u)|static_cast<u8>(255-quotient);
        if(timer>=time) mStatus=WaitIn;
        break;
    }
    case FadeOut: {
        u16 timer=static_cast<u16>(mTimer+1); u32 time=mFadeTime;
        s32 quotient=time?static_cast<s32>(timer*255u)/static_cast<s32>(time):0;
        mTimer=timer; mColor.rgba=(mColor.rgba & 0xFFFFFF00u)|static_cast<u8>(quotient);
        if(timer>=time) mStatus=WaitOut;
        break;
    }
    }
}
/* 027ECFA8 */
bool JUTFader::startFadeIn(s32 time) {
    bool result=mStatus==WaitOut;
    if(result) { mFadeTime=time; mTimer=0; mStatus=FadeIn; }
    return result;
}
/* 027ECFD4 */
bool JUTFader::startFadeOut(s32 time) {
    bool result=mStatus==WaitIn;
    if(result) { mFadeTime=time; mTimer=0; mStatus=FadeOut; }
    return result;
}
/* 027ED004 */
void JUTFader::setStatus(u32 status,s32 delay) {
    if(status>1) return;
    mDelayStatus=status;
    if(delay==0) mStatus=status;
    mDelayTimer=delay;
}
/* 027ED064 */
static void __sinit_JUTFader_cpp() {
    sStaticState.words[2]=0; sStaticState.words[0]=0; sStaticState.words[3]=0; sStaticState.words[1]=0;
    faderRegisterStatic(&sStaticRecord);
}
/* WWHD glue
data sFaderVtable 1016DD64
data sStaticState 104A24A0
data sStaticRecord 101F97D8
ext 0273AD10 void* faderAllocate(u32)
ext 0273AF40 void faderDelete(void*)
ext 028F026C void faderRegisterStatic(void*)
*/
