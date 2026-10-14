/** HD FaderMgr: render callbacks, ColorFader and WipeFader ownership. */
#pragma once
#include "hd/hd_color_fader.h"
#include "hd/hd_wipe_fader.h"
struct hdFadeTaskVtable_l {
    u8 mReserved[0x14];
    be<u32> mStart;
    be<u32> mStartAdjustment;
    be<u32> mIsDone;
};
WWHD_OFFSET(hdFadeTaskVtable_l, mStart, 0x14);
WWHD_OFFSET(hdFadeTaskVtable_l, mIsDone, 0x1C);
struct hdFadeTask_l {
    be<u32> mState;
    u8 mReserved[4];
    hdFadeColorBytes_l mColor;
    u8 mParameters[0x18];
    gptr<hdFadeTaskVtable_l> mpVtable;
};
WWHD_OFFSET(hdFadeTask_l, mColor, 8);
WWHD_OFFSET(hdFadeTask_l, mpVtable, 0x24);
struct hdFaderMgr_c {
    u8 mDisposer[12];
    be<u32> mVtable;
    hdFadeListener_l mSceneListener;
    hdFadeListener_l mForegroundListener;
    hdFadeListener_l mAuxiliaryListener;
    hdFadeListener_l mUnusedListener;
    hdColorFader_c mColorFader;
    hdWipeFader_c mWipeFader;
    gptr<hdFadeTask_l> mpTask;
    be<u8> mForegroundEnabled;
    u8 mPadding[3];
    be<u32> mDelay;
    static hdFaderMgr_c* create(void* heap);
    void destroy(u32 flags);
    void dispose(u32 flags);
    void bindLayers();
    void initialize();
    void calc();
    void draw(void* renderInfo);
    void drawForeground(void* renderInfo);
    void start(u32 direction, u32 delay);
    u32 isDone();
    u32 isFading();
    void reset();
    void bindParticleCallback(void* owner);
    void bindDrawCallback(void* owner);
    void bindShadowCallback(void* owner);
    void unbindAuxiliary();
};
WWHD_OFFSET(hdFaderMgr_c, mSceneListener, 0x10);
WWHD_OFFSET(hdFaderMgr_c, mForegroundListener, 0x50);
WWHD_OFFSET(hdFaderMgr_c, mAuxiliaryListener, 0x90);
WWHD_OFFSET(hdFaderMgr_c, mUnusedListener, 0xD0);
WWHD_OFFSET(hdFaderMgr_c, mColorFader, 0x110);
WWHD_OFFSET(hdFaderMgr_c, mWipeFader, 0x194);
WWHD_OFFSET(hdFaderMgr_c, mpTask, 0x440);
WWHD_OFFSET(hdFaderMgr_c, mForegroundEnabled, 0x444);
WWHD_OFFSET(hdFaderMgr_c, mDelay, 0x448);
WWHD_SIZE(hdFaderMgr_c, 0x44C);
