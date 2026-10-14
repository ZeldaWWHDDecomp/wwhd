/* HD E3 demo course-selection screen; private game-derived reconstruction. */
#pragma once
#include "d/hd_parts.h"
struct hdE3CourseRuntimeWords { be<u32> mWords[4]; };
struct hdE3CourseRegistration { be<u32> mWords[3]; };
struct hdE3CourseContext { be<u8> mByte; };
struct hdE3CourseStringPair { hdUiSafeString mNames[2]; };
struct hdE3CourseCallbackState_l {
    be<u32> mNumber;
    gptr<const char> mName;
    gptr<hdPartsStateIdVtbl_c> mVtable;
    hdPartsPtmf_c mInitialize, mExecute, mFinalize, mIsDone;
};
WWHD_SIZE(hdE3CourseRuntimeWords, 0x10);
WWHD_SIZE(hdE3CourseRegistration, 0xC);
WWHD_SIZE(hdE3CourseContext, 1);
WWHD_SIZE(hdE3CourseStringPair, 0x10);
WWHD_SIZE(hdE3CourseCallbackState_l, 0x2C);
WWHD_OFFSET(hdE3CourseCallbackState_l, mNumber, 0);
WWHD_OFFSET(hdE3CourseCallbackState_l, mName, 4);
WWHD_OFFSET(hdE3CourseCallbackState_l, mVtable, 8);
WWHD_OFFSET(hdE3CourseCallbackState_l, mInitialize, 0xC);
WWHD_OFFSET(hdE3CourseCallbackState_l, mExecute, 0x14);
WWHD_OFFSET(hdE3CourseCallbackState_l, mFinalize, 0x1C);
WWHD_OFFSET(hdE3CourseCallbackState_l, mIsDone, 0x24);
struct hdE3SelectCourse_c : hdParts_c {
    void initializeAnimation();
};
WWHD_SIZE(hdE3SelectCourse_c, 0x50);
WWHD_OFFSET(hdE3SelectCourse_c, mpLayout, 0x44);
