#pragma once
#include "JSystem/J2DGraph/J2DGrafContext.h"
struct JUTFaderVtable { u8 methods[0x28]; };
WWHD_SIZE(JUTFaderVtable,0x28);
struct JUTFader {
    enum EStatus { WaitOut,WaitIn,FadeIn,FadeOut };
    be<u32> mStatus;
    be<u16> mFadeTime,mTimer;
    J2DColor mColor;
    J2DBox mBox;
    be<s32> mDelayTimer;
    be<u32> mDelayStatus;gptr<JUTFaderVtable> mVtable;
    void control();
    bool startFadeIn(s32);
    bool startFadeOut(s32);
    void setStatus(u32,s32);
};
WWHD_OFFSET(JUTFader,mColor,8); WWHD_OFFSET(JUTFader,mBox,0xC);
WWHD_OFFSET(JUTFader,mDelayTimer,0x1C); WWHD_OFFSET(JUTFader,mDelayStatus,0x20);
WWHD_OFFSET(JUTFader,mVtable,0x24); WWHD_SIZE(JUTFader,0x28);
void* faderAllocate(u32);
void faderDelete(void*);
void faderRegisterStatic(void*);
