/** Minigame timer. HD removes the J2D draw object and counts 30 Hz frames rather than OS ticks.
 * The numbered fields retain the GameCube names. Private game-derived source. */
#pragma once
#include "bindings.h"

struct fopMsg_timerAppend {
    u8 padding0[0x1C];
    /* 0x1C */ be<u32> mTimerMode;
    /* 0x20 */ be<u16> mLimitTimeMs; // HD: seconds, converted to frames.
    /* 0x22 */ be<u8> mShowType;
    /* 0x23 */ be<u8> mIconType;
};
struct dTimer_c {
    enum Status { WAITING, START_DELAY, RUNNING, STOCK_DELAY, ENDING, FINISHED, DELETE_REQUEST };
    enum Mode { SHIP_RACE = 2, VOLCANO = 3, SILENT = 4, STOCK = 7 };
    enum ShowType { SHOW_TIMER = 1, SHOW_RUPEE = 2 };
    u8 padding0[0xAC];
    /* 0x0AC */ gptr<fopMsg_timerAppend> mpAppend;
    u8 paddingB0[0x108 - 0xB0];
    /* 0x108 */ be<u32> mLimitFrames; // HD: replaces mLimitTime's OS ticks.
    /* 0x10C */ be<u32> mRestFrames; // HD: counts down once per execution.
    /* 0x110 */ be<s32> field_0x150;
    /* 0x114 */ be<u32> mTimerMode;
    /* 0x118 */ be<s32> field_0x158;
    /* 0x11C */ be<u32> field_0x15c;
    /* 0x120 */ be<s16> field_0x160;
    /* 0x122 */ be<u8> field_0x162;
    /* 0x123 */ be<u8> field_0x163;
    /* 0x124 */ be<u8> mStatus;
    u8 padding125[3];
    /* 0x128 */ be<u32> mSeTimeLeft;
    /* 0x12C */ be<u32> mSeTableIndex;
    u8 padding130[4];
    /* 0x134 */ be<u8> mShowType;
    /* 0x135 */ be<u8> mIsInitialized;
    u8 padding136[2];
    u32 stop(u32 reason);
    u32 restart(u32 reason);
    s32 getRestTimeMs();
    u32 stock_start();
    u32 stock_startDelay(s16 delay);
    u32 start();
    u32 startDelay(s16 delay);
    u32 getTimeMs(); // HD: frame count; shared timer-info stores it without conversion.
    u32 getLimitTimeMs(); // HD: frame count.
    u32 end(s32 delay);
    u32 deleteRequest();
    void timerShow();
    u32 RestTimeCheck(s32 threshold);
    void SetSE();
    s32 _execute();
    s32 _delete();
    void setShowType(u32 bits);
    s32 _create();
    u32 deleteCheck();
};
// Unit-local field view: the shared play header does not expose timer-info yet.
struct dTimerInfoView {
    gptr<dTimer_c> mTimerPtr;
    be<u32> mTimerNowTimeMs, mTimerLimitTimeMs;
    be<s32> mTimerMode;
};
struct dTimerPlayView {
    enum { TIMER_FREEZE = 0x20000000 };
    u8 padding0[0x5292];
    /* 0x5292 */ be<u8> mPauseState;
    u8 padding5293[0x5CD8 - 0x5293];
    /* 0x5CD8 */ be<u32> mGameStatus;
    u8 padding5CDC[0x5CF0 - 0x5CDC];
    /* 0x5CF0 */ dTimerInfoView mTimerInfo;
};
struct dTimerSeEntry { be<u32> mTime, mSound; };
struct dTimerIconType { be<u8> mType; };
struct dTimerShowFlags { be<u8> mTimer, mRupee; };
struct dTimerHeaderStatics {
    be<f32> mFirst, mSecond;
    be<f32> mFirstPair[2], mSecondPair[2];
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
struct dTimerStaticRecord { u8 opaque[12]; };
struct dTimerStaticRecords { dTimerStaticRecord mFirst, mSecond, mThird; };
WWHD_OFFSET(dTimer_c, mpAppend, 0xAC);
WWHD_OFFSET(dTimer_c, mLimitFrames, 0x108);
WWHD_OFFSET(dTimer_c, mStatus, 0x124);
WWHD_OFFSET(dTimer_c, mShowType, 0x134);
WWHD_SIZE(dTimer_c, 0x138);
WWHD_SIZE(fopMsg_timerAppend, 0x24);
WWHD_OFFSET(dTimerPlayView, mTimerInfo, 0x5CF0);
WWHD_SIZE(dTimerPlayView, 0x5D00);
WWHD_SIZE(dTimerInfoView, 16);
WWHD_SIZE(dTimerSeEntry, 8);
WWHD_SIZE(dTimerShowFlags, 2);
WWHD_SIZE(dTimerIconType, 1);
WWHD_SIZE(dTimerStaticRecord, 12);
WWHD_SIZE(dTimerHeaderStatics, 0x2C);
WWHD_SIZE(dTimerStaticRecords, 36);
dTimerPlayView* dTimer_getPlay();
u32 dTimer_menuFlag();
u32 dTimer_eventPause();
void dTimer_seStart(u32 sound);
void dTimer_deleteMessage(void*);
u32 dTimer_createAppend(u32 process, u32 mode, u32 seconds, u32 show, u32 icon, u32 user,
    f32 timerX, f32 timerY, f32 rupeeX, f32 rupeeY);
void dTimer_registerStatic(void*);
void dTimer_initFlag(void*);
void dTimer_initByte(void*);
void dTimer_timerHide();
void dTimer_setIconType(u32 ignored, u32 type);

enum { fpcNm_TIMER_e = 0x1E0, JA_SE_MINIGAME_TIMER_EACH = 0x8EA };

// HD: Calls to the parameterless hide entry still forward this in r3.
void dTimer_hideObject(void*);
