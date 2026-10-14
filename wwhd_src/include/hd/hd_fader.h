/** HD Fader render layer; byte-exact local views of the Wii U layer/event objects. */
#pragma once
#include "bindings.h"
enum hdFaderRenderMode_e { HD_FADER_OUTPUT_DRC_e = 2, HD_FADER_DRAW_MODE_e = 3, HD_FADER_LAYER_e = 6, HD_FADER_DRAW_PASS_e = 14 };
enum hdFaderEventType_e { HD_FADER_EVENT_DRAW_e = 2 };
struct hdFaderEvent_c { be<u32> mType; u8 mPayload[8]; be<u32> mLayer; };
struct hdFaderRenderer_l { u8 mStorage[0x1684]; be<u32> mOutput; };
struct hdFaderLayer_c {
    u8 mDisposer[0xC];
    gptr<void> mpLayerVtable, mpRenderVtable;
    u8 mStorage[0x610 - 0x14];
    u32 initialize();
    u32 execute();
    void destroy(u32);
    void attachEvent();
    void drawEvent(hdFaderEvent_c*);
};
struct hdFaderName_l { be<u32> mText; gptr<void> mpVtable; };
struct hdFaderDelegate_l {
    gptr<void> mpVtable;
    gptr<hdFaderLayer_c> mpObject;
    be<s16> mAdjustment, mVirtualIndex;
    gabi::gfn mFunction;
};
struct hdFaderSubscription_l {
    u8 mDisposer[0xC];
    gptr<void> mpVtable;
    hdFaderName_l mName;
    be<u32> mEnabled, mReserved;
    hdFaderDelegate_l mDelegate;
    u8 mTail[0x10];
};
struct hdFaderDrawMode_l { be<u32> mMode, mFlags; };
WWHD_SIZE(hdFaderLayer_c, 0x610);
WWHD_OFFSET(hdFaderLayer_c, mpRenderVtable, 0x10);
WWHD_OFFSET(hdFaderEvent_c, mLayer, 0xC);
WWHD_OFFSET(hdFaderRenderer_l, mOutput, 0x1684);
WWHD_SIZE(hdFaderSubscription_l, 0x40);
WWHD_OFFSET(hdFaderSubscription_l, mDelegate, 0x20);

struct hdFaderInitLists_l { be<u32> mFirst, mSecond, mThird, mFourth; };
struct hdFaderAngles_l { be<f32> mMinimum, mMaximum; be<u8> mFirstObject, mSecondObject; };
struct hdFaderInitRecord_l { u8 mStorage[12]; };
struct hdFaderAngleLimits_l { be<f32> mMinimum, mMaximum; };
