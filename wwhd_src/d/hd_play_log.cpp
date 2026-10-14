// HD play-log counters, accumulated statistics and current summary. Private source.
#include "d/hd_picture_statics.h"
#include "d/hd_play_log.h"
struct hdPlayLogQueries_c { gabi::ptmf<u32 (hdPlayLog_c::*)()> mMembers[21]; };
static hdPlayLogQueries_c sQueryMembers;
static hdPictureStaticState_c sPlayLogHeaderState;
static hdPictureStaticTokens_c sPlayLogHeaderTokens;
static const hdPictureAngles_c sPlayLogHeaderAngles;
/* 02726A34 */
static hdPlayLog_c* constructPlayLog(hdPlayLog_c* self) {
    if (!self) {
        self = static_cast<hdPlayLog_c*>(playLogAllocate(sizeof(hdPlayLog_c)));
        if (!self) return NULL;
    }
    self->mCounterA = 0;
    self->mCounterB = 0;
    self->mCounterC = 0;
    self->mNormalMode = 0;
    self->mCounterD = 0;
    self->mCounterE = 0;
    self->mCounterF = 0;
    self->mSummary = 0;
    self->mProgress = 0;
    self->mAlternateProgress = 0;
    self->mDeluxePictoBox = 0;
    self->mMenuState = 0;
    self->mPlayerSlot = 0;
    self->mPlayerMode = 0;
    self->mPlayerCompletion = 0;
    self->mPlayerStatus = 0;
    self->mCollectionState = 0;
    self->mCompletionState = 0;
    self->mCounterG = 0;
    self->mCounterH = 0;
    self->mTotalProgress = 0;
    return self;
}
/* 02726AD4 */
void hdPlayLog_c::reset() {
    mCounterA = 0;
    mCounterB = 0;
    mCounterC = 0;
    mNormalMode = 0;
    mCounterD = 0;
    mCounterE = 0;
    mCounterF = 0;
    mSummary = 0;
    mProgress = 0;
    mAlternateProgress = 0;
    mDeluxePictoBox = 0;
    mMenuState = 0;
    mPlayerSlot = 0;
    mPlayerMode = 0;
    mPlayerCompletion = 0;
    mPlayerStatus = 0;
    mCollectionState = 0;
    mCompletionState = 0;
    mCounterG = 0;
    mCounterH = 0;
    mTotalProgress = 0;
}
/* 02726B30 */
void hdPlayLog_c::clear() {
    mCounterA = 0;
    mCounterB = 0;
    mCounterC = 0;
    mNormalMode = 0;
    mCounterD = 0;
    mCounterE = 0;
    mCounterF = 0;
    mSummary = 0;
    mProgress = 0;
    mAlternateProgress = 0;
    mDeluxePictoBox = 0;
    mMenuState = 0;
    mPlayerSlot = 0;
    mPlayerMode = 0;
    mPlayerCompletion = 0;
    mPlayerStatus = 0;
    mCollectionState = 0;
    mCompletionState = 0;
    mCounterG = 0;
    mCounterH = 0;
    mTotalProgress = 0;
}
/* 02726B8C */
void hdPlayLog_c::clearSummary() {
    mNormalMode = 0;
    mAlternateProgress = 0;
    mCollectionState = 0;
    mMenuState = 0;
    mPlayerMode = 0;
    mProgress = 0;
    mPlayerCompletion = 0;
    mTotalProgress = 0;
    mCompletionState = 0;
    mPlayerStatus = 0;
    mSummary = 0;
    mPlayerSlot = 0;
    mDeluxePictoBox = 0;
}
/* 02726BC8 */
void hdPlayLog_c::copy(hdPlayLog_c* other) {
    mCounterA = other->mCounterA;
    mCounterB = other->mCounterB;
    mCounterC = other->mCounterC;
    mNormalMode = other->mNormalMode;
    mCounterD = other->mCounterD;
    mCounterE = other->mCounterE;
    mCounterF = other->mCounterF;
    mSummary = other->mSummary;
    mProgress = other->mProgress;
    mAlternateProgress = other->mAlternateProgress;
    mDeluxePictoBox = other->mDeluxePictoBox;
    mMenuState = other->mMenuState;
    mPlayerSlot = other->mPlayerSlot;
    mPlayerMode = other->mPlayerMode;
    mPlayerCompletion = other->mPlayerCompletion;
    mPlayerStatus = other->mPlayerStatus;
    mCollectionState = other->mCollectionState;
    mCompletionState = other->mCompletionState;
    mCounterG = other->mCounterG;
    mCounterH = other->mCounterH;
    mTotalProgress = other->mTotalProgress;
}
/* 02726C74 */
void hdPlayLog_c::copySummary(hdPlayLog_c* other) {
    mNormalMode = other->mNormalMode;
    mSummary = other->mSummary;
    mProgress = other->mProgress;
    mAlternateProgress = other->mAlternateProgress;
    mDeluxePictoBox = other->mDeluxePictoBox;
    mMenuState = other->mMenuState;
    mPlayerSlot = other->mPlayerSlot;
    mPlayerMode = other->mPlayerMode;
    mPlayerCompletion = other->mPlayerCompletion;
    mPlayerStatus = other->mPlayerStatus;
    mCollectionState = other->mCollectionState;
    mCompletionState = other->mCompletionState;
    mTotalProgress = other->mTotalProgress;
}
/* 02726CE0 */
u32 hdPlayLog_c::query(u32 index) {
    return (this->*sQueryMembers.mMembers[index])();
}
/* 02726D2C */
u32 hdPlayLog_c::getCounterA() { return mCounterA; }
/* 02726D34 */
void hdPlayLog_c::setCounterA(u32 value) { mCounterA = value; }
/* 02726D3C */
void hdPlayLog_c::addCounterA(u32 value) { mCounterA = u32(mCounterA) + value; }
/* 02726D4C */
u32 hdPlayLog_c::getCounterB() { return mCounterB; }
/* 02726D54 */
void hdPlayLog_c::setCounterB(u32 value) { mCounterB = value; }
/* 02726D5C */
void hdPlayLog_c::addCounterB(u32 value) { mCounterB = u32(mCounterB) + value; }
/* 02726D6C */
u32 hdPlayLog_c::getCounterC() { return mCounterC; }
/* 02726D74 */
void hdPlayLog_c::setCounterC(u32 value) { mCounterC = value; }
/* 02726D7C */
void hdPlayLog_c::addCounterC(u32 value) { mCounterC = u32(mCounterC) + value; }
/* 02726D8C */
u32 hdPlayLog_c::getNormalMode() { return mNormalMode; }
/* 02726D94 */
void hdPlayLog_c::setNormalMode(u32 value) { mNormalMode = value; }
/* 02726D9C */
u32 hdPlayLog_c::getCounterD() { return mCounterD; }
/* 02726DA4 */
void hdPlayLog_c::setCounterD(u32 value) { mCounterD = value; }
/* 02726DAC */
void hdPlayLog_c::addCounterD(u32 value) { mCounterD = u32(mCounterD) + value; }
/* 02726DBC */
u32 hdPlayLog_c::getCounterE() { return mCounterE; }
/* 02726DC4 */
void hdPlayLog_c::setCounterE(u32 value) { mCounterE = value; }
/* 02726DCC */
void hdPlayLog_c::addCounterE(u32 value) { mCounterE = u32(mCounterE) + value; }
/* 02726DDC */
u32 hdPlayLog_c::getCounterF() { return mCounterF; }
/* 02726DE4 */
void hdPlayLog_c::setCounterF(u32 value) { mCounterF = value; }
/* 02726DEC */
void hdPlayLog_c::addCounterF(u32 value) { mCounterF = u32(mCounterF) + value; }
/* 02726DFC */
u32 hdPlayLog_c::getSummary() { return mSummary; }
/* 02726E04 */
void hdPlayLog_c::setSummary(u32 value) { mSummary = value; }
/* 02726E0C */
void hdPlayLog_c::addSummary(u32 value) { mSummary = u32(mSummary) + value; }
/* 02726E1C */
u32 hdPlayLog_c::getProgress() { return mProgress; }
/* 02726E24 */
void hdPlayLog_c::setProgress(u32 value) { mProgress = value; }
/* 02726E2C */
u32 hdPlayLog_c::getAlternateProgress() { return mAlternateProgress; }
/* 02726E34 */
void hdPlayLog_c::setAlternateProgress(u32 value) { mAlternateProgress = value; }
/* 02726E3C */
u32 hdPlayLog_c::getDeluxePictoBox() { return mDeluxePictoBox; }
/* 02726E44 */
void hdPlayLog_c::setDeluxePictoBox(u32 value) { mDeluxePictoBox = value; }
/* 02726E4C */
u32 hdPlayLog_c::getMenuState() { return mMenuState; }
/* 02726E54 */
void hdPlayLog_c::setMenuState(u32 value) { mMenuState = value; }
/* 02726E5C */
u32 hdPlayLog_c::getPlayerSlot() { return mPlayerSlot; }
/* 02726E64 */
void hdPlayLog_c::setPlayerSlot(u32 value) { mPlayerSlot = value; }
/* 02726E6C */
u32 hdPlayLog_c::getPlayerMode() { return mPlayerMode; }
/* 02726E74 */
void hdPlayLog_c::setPlayerMode(u32 value) { mPlayerMode = value; }
/* 02726E7C */
u32 hdPlayLog_c::getPlayerCompletion() { return mPlayerCompletion; }
/* 02726E84 */
void hdPlayLog_c::setPlayerCompletion(u32 value) { mPlayerCompletion = value; }
/* 02726E8C */
u32 hdPlayLog_c::getPlayerStatus() { return mPlayerStatus; }
/* 02726E94 */
void hdPlayLog_c::setPlayerStatus(u32 value) { mPlayerStatus = value; }
/* 02726E9C */
u32 hdPlayLog_c::getCollectionState() { return mCollectionState; }
/* 02726EA4 */
void hdPlayLog_c::setCollectionState(u32 value) { mCollectionState = value; }
/* 02726EAC */
u32 hdPlayLog_c::getCompletionState() { return mCompletionState; }
/* 02726EB4 */
void hdPlayLog_c::setCompletionState(u32 value) { mCompletionState = value; }
/* 02726EBC */
u32 hdPlayLog_c::getCounterG() { return mCounterG; }
/* 02726EC4 */
void hdPlayLog_c::setCounterG(u32 value) { mCounterG = value; }
/* 02726ECC */
void hdPlayLog_c::addCounterG(u32 value) { mCounterG = u32(mCounterG) + value; }
/* 02726EDC */
u32 hdPlayLog_c::getCounterH() { return mCounterH; }
/* 02726EE4 */
void hdPlayLog_c::setCounterH(u32 value) { mCounterH = value; }
/* 02726EEC */
void hdPlayLog_c::addCounterH(u32 value) { mCounterH = u32(mCounterH) + value; }
/* 02726EFC */
u32 hdPlayLog_c::getTotalProgress() { return mTotalProgress; }
/* 02726F04 */
void hdPlayLog_c::setTotalProgress(u32 value) { mTotalProgress = value; }
/* 02726F0C */
static void initializePlayLogStatics() {
    initializePictureHeaderStatics(&sPlayLogHeaderState, &sPlayLogHeaderTokens, &sPlayLogHeaderAngles);
}
/* WWHD glue
 data sPlayLogHeaderState 1049F84C
 data sPlayLogHeaderTokens 101F85E0
 data sPlayLogHeaderAngles 10140D00
 data sQueryMembers 10140D08
 ext 0273AD10 void* playLogAllocate(u32)
*/
