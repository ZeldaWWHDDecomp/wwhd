/* HD E3 demo thank-you state callbacks; private game-derived reconstruction. */
#pragma once
#include "d/hd_parts.h"
struct hdE3ThankYouRuntimeWords { be<u32> mWords[4]; };
struct hdE3ThankYouRegistration { be<u32> mWords[3]; };
struct hdE3ThankYouContext { be<u8> mByte; };
struct hdE3ThankYouAnimationNames { hdUiSafeString mNames[2]; };
WWHD_SIZE(hdE3ThankYouRuntimeWords, 0x10);
WWHD_SIZE(hdE3ThankYouRegistration, 0xC);
WWHD_SIZE(hdE3ThankYouContext, 1);
WWHD_SIZE(hdE3ThankYouAnimationNames, 0x10);
struct hdE3ThankYou_c : hdParts_c {
    u32 acceptsInput();
    u32 isComplete();
};
WWHD_SIZE(hdE3ThankYou_c, 0x50);
u32 hdE3ThankYouStateKey(hdPartsStateId_c* state);
