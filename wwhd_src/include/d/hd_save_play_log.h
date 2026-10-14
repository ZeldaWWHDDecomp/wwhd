/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "bindings.h"

struct hdSavePoint_c { be<u32> mXBits; be<u32> mYBits; };
WWHD_SIZE(hdSavePoint_c,8);
struct hdSavePlayLog_c {
    /* 0x00 */ hdSavePoint_c mStart;
    /* 0x08 */ be<f32> mStartTime;
    /* 0x0C */ hdSavePoint_c mEnd;
    /* 0x14 */ be<f32> mEndTime;
    /* 0x18 */ be<u32> mFlags[49];
    void init(); void clear(); void setStart(hdSavePoint_c*); void setStartTime(f32); f32 getStartTime();
    void setEnd(hdSavePoint_c*); hdSavePoint_c* getEnd(); void setEndTime(f32); f32 getEndTime();
    void setFlag(u32,u32); void setVisited(u32); BOOL hasFlag(u32,u32); BOOL isVisited(u32); void copyFrom(hdSavePlayLog_c*);
};
WWHD_OFFSET(hdSavePlayLog_c,mStartTime,8);
WWHD_OFFSET(hdSavePlayLog_c,mEnd,0xC);
WWHD_OFFSET(hdSavePlayLog_c,mEndTime,0x14);
WWHD_OFFSET(hdSavePlayLog_c,mFlags,0x18);
WWHD_SIZE(hdSavePlayLog_c,0xDC);


hdSavePlayLog_c* createSavePlayLog(hdSavePlayLog_c*);
