/** Minigame countdown. HD uses frame counters and native HUD visibility rather than J2D animation.
 * All entry signatures and clobberable call order are preserved. Private game-derived source. */
#include "d/d_timer.h"

static dTimerShowFlags l_showFlags;
static dTimerIconType l_iconType;
static dTimerSeEntry ShipRaceSeTable[13] = {
    {30000, 0x845},
    {20000, 0x845},
    {10000, 0x846},
    {9000, 0x846},
    {8000, 0x846},
    {7000, 0x846},
    {6000, 0x846},
    {5000, 0x846},
    {4000, 0x846},
    {3000, 0x846},
    {2000, 0x846},
    {1000, 0x846},
    {4294967295, 0x0}
};
static dTimerSeEntry VolcanoSeTable[13] = {
    {60000, 0x8ad},
    {30000, 0x8ad},
    {10000, 0x8ae},
    {9000, 0x8ae},
    {8000, 0x8ae},
    {7000, 0x8ae},
    {6000, 0x8ae},
    {5000, 0x8ae},
    {4000, 0x8ae},
    {3000, 0x8ae},
    {2000, 0x8ae},
    {1000, 0x8ae},
    {4294967295, 0x0}
};
static f32 l_stockTimerX;
static f32 l_stockTimerY;
static f32 l_stockRupeeX;
static f32 l_stockRupeeY;
static f32 l_initialFirst;
static f32 l_initialSecond;
static f32 l_initialPairFirst;
static f32 l_initialPairSecond;
static dTimerHeaderStatics l_headerStatics;
static dTimerStaticRecords l_records;

/* 025C5770 */
void dTimer_drawNativeHud() {
    // HD: The native UI owns drawing.
}

/* 025C5774 */
s32 dTimer_Draw() { return TRUE; }

/* 025C577C */
void dTimer_timerHide() {
    l_showFlags.mRupee = false;
    l_showFlags.mTimer = false;
}

/* 025C5794 */
u32 dTimer_c::stop(u32 reason) {
    if (field_0x162 == 1 || field_0x163 != 0 || mStatus != RUNNING) return FALSE;
    field_0x163 = reason;
    field_0x162 = 1;
    return TRUE;
}

/* 025C57D4 */
u32 dTimer_c::restart(u32 reason) {
    if (field_0x162 != 1 || field_0x163 != reason || mStatus != RUNNING) return FALSE;
    field_0x162 = 0;
    field_0x163 = 0;
    return TRUE;
}

/* 025C5814 */
s32 dTimer_c::getRestTimeMs() {
    // HD: Multiply with u32 wrap before the signed divide, at the fixed 30 Hz rate.
    return s32(u32(mRestFrames) * 1000) / 30;
}

/* 025C583C */
u32 dTimer_c::stock_start() {
    if (mStatus != STOCK_DELAY) return FALSE;
    mStatus = RUNNING;
    return TRUE;
}

/* 025C5864 */
u32 dTimer_c::start() {
    u8 status = mStatus;
    if (status != WAITING && status != START_DELAY) return FALSE;
    mStatus = RUNNING;
    return TRUE;
}

/* 025C5890 */
u32 dTimer_c::getTimeMs() { return u32(mLimitFrames) - u32(mRestFrames); }

/* 025C58A0 */
u32 dTimer_c::getLimitTimeMs() { return mLimitFrames; }

/* 025C58A8 */
u32 dTimer_c::end(s32 delay) {
    if (mStatus != RUNNING) return FALSE;
    mStatus = ENDING;
    if (delay != -1) field_0x158 = delay;
    return TRUE;
}

/* 025C58D8 */
u32 dTimer_c::deleteRequest() {
    mStatus = DELETE_REQUEST;
    return TRUE;
}

/* 025C58E8 */
void dTimer_c::timerShow() {
    if (mShowType & SHOW_TIMER) l_showFlags.mTimer = true;
    if (mShowType & SHOW_RUPEE) l_showFlags.mRupee = true;
}

/* 025C591C */
u32 dTimer_c::RestTimeCheck(s32 threshold) {
    s32 now = getRestTimeMs();
    return now <= threshold && threshold > field_0x150;
}

/* 025C5960 */
void dTimer_c::SetSE() {
    if (field_0x162 == 0 && mTimerMode != SILENT) {
        s32 now = getRestTimeMs();
        if (now / 1000 < field_0x150 / 1000) dTimer_seStart(JA_SE_MINIGAME_TIMER_EACH);
    }
    if (!RestTimeCheck(mSeTimeLeft)) return;
    dTimerSeEntry* seTable;
    switch (u32(mTimerMode)) {
    case SHIP_RACE: seTable = ShipRaceSeTable; break;
    case VOLCANO: seTable = VolcanoSeTable; break;
    default: return;
    }
    u32 index = mSeTableIndex;
    u32 sound = seTable[index].mSound;
    if (sound == 0xFFFFFFFF) return;
    dTimer_seStart(sound);
    index = u32(mSeTableIndex) + 1;
    mSeTableIndex = index;
    mSeTimeLeft = seTable[index].mTime;
}

/* 025C5A8C */
s32 dTimer_c::_execute() {
    auto* play = dTimer_getPlay();
    // HD: Special game state freezes the frame timer and hides both HUD groups.
    if (play->mGameStatus & dTimerPlayView::TIMER_FREEZE) {
        field_0x162 = 1;
        dTimer_hideObject(this);
        return TRUE;
    }
    u8 status = mStatus;
    if ((status == WAITING || status == RUNNING) && (mTimerMode == VOLCANO || mTimerMode == SHIP_RACE)) {
        bool shouldStop = dTimer_menuFlag() != 0;
        if (!shouldStop) shouldStop = dTimer_eventPause() != 0;
        if (!shouldStop) shouldStop = dTimer_getPlay()->mPauseState == 1;
        if (shouldStop) stop(1);
        else restart(1);
    }
    if (field_0x162 != 1) {
        field_0x150 = getRestTimeMs();
        status = mStatus;
        if (status == START_DELAY || status == STOCK_DELAY) {
            s16 delay = s16(u16(u16(field_0x160) - 1));
            field_0x160 = delay;
            if (delay <= 0) {
                if (mStatus == STOCK_DELAY) stock_start();
                else start();
            }
        } else if (status == RUNNING) {
            mRestFrames = u32(mRestFrames) - 1;
            u32 value = getTimeMs();
            dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = value;
            value = getLimitTimeMs();
            dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs = value;
            if (getRestTimeMs() <= 0) {
                value = getTimeMs();
                dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = value;
                if (mTimerMode == VOLCANO) end(30);
            }
        }
    }
    status = mStatus;
    if (status == DELETE_REQUEST) dTimer_deleteMessage(this);
    else if (status == ENDING) {
        s32 delay = field_0x158;
        if (delay > 0) field_0x158 = u32(delay) - 1;
        else if (mTimerMode == VOLCANO) deleteRequest();
        else mStatus = FINISHED;
    }
    // HD: Visibility is delayed by a decrementing counter; the GameCube animates the draw object.
    u32 showDelay = field_0x15c;
    if (showDelay) {
        --showDelay;
        field_0x15c = showDelay;
        if (showDelay == 0) timerShow();
    }
    SetSE();
    return TRUE;
}

/* 025C5D18 */
s32 dTimer_Execute(dTimer_c* i_this) { return i_this->_execute(); }

/* 025C5D1C */
s32 dTimer_IsDelete() { return TRUE; }

/* 025C5D24 */
s32 dTimer_c::_delete() {
    if (mTimerMode == VOLCANO) {
        u8 status = mStatus;
        dTimer_getPlay()->mTimerInfo.mTimerMode =
            (status == DELETE_REQUEST || status == FINISHED || status == ENDING) ? -1 : VOLCANO;
        u32 value = getTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = value;
        value = getLimitTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs = value;
    } else {
        dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = 0;
        dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs = 0;
        dTimer_getPlay()->mTimerInfo.mTimerMode = -1;
    }
    dTimer_getPlay()->mTimerInfo.mTimerPtr = nullptr;
    dTimer_hideObject(this);
    return TRUE;
}

/* 025C5E50 */
s32 dTimer_Delete(dTimer_c* i_this) { return i_this->_delete(); }

/* 025C5E54 */
void dTimer_c::setShowType(u32 bits) {
    u8 initialized = mIsInitialized;
    mShowType = bits;
    if (initialized) {
        l_showFlags.mTimer = bits & SHOW_TIMER;
        l_showFlags.mRupee = (mShowType >> 1) & 1;
    }
}

/* 025C5E84 */
void dTimer_setIconType(u32 ignored, u32 type) { l_iconType.mType = type; }

/* 025C5E90 */
u32 dTimer_c::stock_startDelay(s16 delay) {
    if (mStatus != WAITING) return FALSE;
    field_0x160 = delay;
    mStatus = STOCK_DELAY;
    return TRUE;
}

/* 025C5EB8 */
s32 dTimer_c::_create() {
    auto* prm = mpAppend.get();
    mIsInitialized = false;
    if (!prm) return cPhs_ERROR_e; // HD: append-only creation, without the Timer archive/J2D heap.
    setShowType(u32(prm->mShowType));
    dTimer_setIconType(gabi::ea(this), u32(prm->mIconType));
    u32 mode = prm->mTimerMode;
    mTimerMode = mode;
    if (mode == STOCK) {
        mLimitFrames = dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs;
        auto* play = dTimer_getPlay();
        mRestFrames = u32(mLimitFrames) - u32(play->mTimerInfo.mTimerNowTimeMs);
        u32 value = getTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = value;
        value = getLimitTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs = value;
        mode = dTimer_getPlay()->mTimerInfo.mTimerMode;
        mStatus = WAITING;
        mTimerMode = mode;
        dTimer_getPlay()->mTimerInfo.mTimerMode = mode;
        dTimer_getPlay()->mTimerInfo.mTimerPtr = this;
        if (mTimerMode == VOLCANO) setShowType(1);
        stock_startDelay(s16(10));
    } else {
        u32 frames = u32(prm->mLimitTimeMs) * 30;
        mLimitFrames = frames;
        mRestFrames = frames;
        u32 value = getTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerNowTimeMs = value;
        value = getLimitTimeMs();
        dTimer_getPlay()->mTimerInfo.mTimerLimitTimeMs = value;
        mode = mTimerMode;
        mStatus = WAITING;
        dTimer_getPlay()->mTimerInfo.mTimerMode = mode;
        dTimer_getPlay()->mTimerInfo.mTimerPtr = this;
    }
    field_0x163 = 0;
    field_0x162 = 0;
    field_0x158 = 120;
    mSeTimeLeft = 0;
    field_0x15c = 6;
    mSeTableIndex = 0;
    mode = mTimerMode;
    if (mode == SHIP_RACE || mode == VOLCANO) {
        dTimerSeEntry* seTable = mode == SHIP_RACE ? ShipRaceSeTable : VolcanoSeTable;
        u32 index = mSeTableIndex;
        for (;;) {
            u32 threshold = seTable[index].mTime;
            mSeTimeLeft = threshold;
            s32 now = getRestTimeMs();
            if (now > s32(threshold)) break;
            index = u32(mSeTableIndex) + 1;
            u32 old = mSeTimeLeft;
            mSeTableIndex = index;
            if (s32(old) < 0) break;
        }
    }
    mIsInitialized = true;
    return cPhs_COMPLEATE_e;
}

/* 025C60F0 */
s32 dTimer_Create(dTimer_c* i_this) { return i_this->_create(); }

/* 025C60F4 */
u32 dTimer_createTimer(u32 mode, u32 seconds, u32 bits, u32 icon, f32 x, f32 y, f32 rx, f32 ry) {
    if (dTimer_getPlay()->mTimerInfo.mTimerMode != -1) return u32(-1);
    return dTimer_createAppend(fpcNm_TIMER_e, mode & 0xFF, seconds, bits, icon, 0, x, y, rx, ry);
}

/* 025C61E4 */
u32 dTimer_createStockTimer() {
    if (dTimer_getPlay()->mTimerInfo.mTimerMode == -1) return u32(-1);
    f32 x = l_stockTimerX, rx = l_stockRupeeX, ry = l_stockRupeeY, y = l_stockTimerY;
    return dTimer_createAppend(fpcNm_TIMER_e, dTimer_c::STOCK, 0, 3, 0, 0, x, y, rx, ry);
}

/* 025C6250 */
u32 dTimer_c::startDelay(s16 delay) {
    if (mStatus != WAITING) return FALSE;
    field_0x160 = delay;
    mStatus = START_DELAY;
    return TRUE;
}

/* 025C6278 */
u32 dTimer_c::deleteCheck() { return mStatus == FINISHED; }

// HD: Compiler-generated statics of this translation unit.
/* 025C628C */
void dTimer_staticInit() {
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    dTimer_registerStatic(&l_records.mFirst);
    f32 first = l_initialFirst, second = l_initialSecond;
    l_headerStatics.mFirst = first;
    l_headerStatics.mSecond = second;
    dTimer_initFlag(&l_headerStatics.mFlag);
    dTimer_registerStatic(&l_records.mSecond);
    dTimer_initByte(&l_headerStatics.mByte);
    dTimer_registerStatic(&l_records.mThird);
    first = l_initialPairFirst;
    second = l_initialPairSecond;
    l_headerStatics.mFirstPair[0] = first;
    l_headerStatics.mSecondPair[0] = second;
    l_headerStatics.mFirstPair[1] = first;
    l_headerStatics.mSecondPair[1] = second;
}

/* WWHD glue
ext 025C577C void dTimer_hideObject(void*)
ext 025200D4 dTimerPlayView* dTimer_getPlay()
ext 025986BC u32 dTimer_menuFlag()
ext 025AF2A4 u32 dTimer_eventPause()
ext 025E1988 void dTimer_seStart(u32)
ext 025DF944 void dTimer_deleteMessage(void*)
ext 025DB468 u32 dTimer_createAppend(u32, u32, u32, u32, u32, u32, f32, f32, f32, f32)
ext 028F026C void dTimer_registerStatic(void*)
ext 028ED6F8 void dTimer_initFlag(void*)
ext 028EAB2C void dTimer_initByte(void*)
data l_showFlags 1047B099
data l_iconType 1047B07E
data ShipRaceSeTable 101EE750
data VolcanoSeTable 101EE7B8
data l_stockTimerX 1005608C
data l_stockTimerY 10056090
data l_stockRupeeX 10056094
data l_stockRupeeY 10056098
data l_initialFirst 100560A0
data l_initialSecond 100560A4
data l_initialPairFirst 100560A8
data l_initialPairSecond 100560AC
data l_headerStatics 104871E8
data l_records 101EE820
*/
