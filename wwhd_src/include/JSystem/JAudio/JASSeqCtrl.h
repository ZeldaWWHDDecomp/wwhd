/* JAudio sequence control; Cafe offsets match the GameCube layout. */
#pragma once
#include "bindings.h"
namespace JASystem {
class TSeqCtrl {
public:
    void init();
    void start(void* data, u32 offset);
    u32 callIntr(void* handler);
    u32 read16();
    u32 read24();
    u32 loopEnd();
    u32 retIntr();
    u32 waitCountDown();
    u32 readByte() {
        be<u8>* current = (be<u8>*)(void*)mCurrentFilePtr;
        mCurrentFilePtr = current + 1;
        return *current;
    }
    u32 get16(u32 offset) const;
    u32 get24(u32 offset) const;
    u32 get32(u32 offset) const;
    gptr<void> mRawFilePtr;
    gptr<void> mCurrentFilePtr;
    be<s32> mWaitTimer;
    be<u32> mLoopIndex;
    gptr<void> mLoopStartPositions[8];
    be<u16> mLoopTimers[8];
    be<s32> mSavedWaitTimer;
    gptr<void> mPreviousFilePtr;
};
WWHD_OFFSET(TSeqCtrl,mWaitTimer,8);
WWHD_OFFSET(TSeqCtrl,mLoopIndex,0xC);
WWHD_OFFSET(TSeqCtrl,mLoopStartPositions,0x10);
WWHD_OFFSET(TSeqCtrl,mLoopTimers,0x30);
WWHD_OFFSET(TSeqCtrl,mSavedWaitTimer,0x40);
WWHD_OFFSET(TSeqCtrl,mPreviousFilePtr,0x44);
WWHD_SIZE(TSeqCtrl,0x48);
}
