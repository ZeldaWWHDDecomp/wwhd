/* Miiverse chart selection and input controls. */
#include "d/hd_screen_menu_miiverse_texture.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_save_info_holder.h"
struct hdMiiverseEventMgrSlot_c { gptr<EventMgr> mpManager; };
WWHD_SIZE(hdMiiverseEventMgrSlot_c, 4);
static hdMiiverseEventMgrSlot_c sMiiverseEventMgr;
void miiverseSetInputMode(EventMgr*, u32);

/* 026A5A58 */
u32 hdMenuMiiverse_c::lastFilledChart() const {
    u32 last = 0;
    for (u32 index = 0; index < 10; ++index) {
        if (!mpChartButtons[index]->mFilled) break;
        last = index;
    }
    return last;
}

/* 026A6188 */
void hdMenuMiiverse_c::resetChartButtons() {
    for (u32 index = 0; index < 10; ++index) {
        miiverseResetChartButton(mpChartButtons[index]);
    }
    miiverseResetPreviousButton(mpPreviousButton);
    miiverseResetNextButton(mpNextButton);
}

/* 026A620C */
void hdMenuMiiverse_c::enableChartInput() {
    mInputEnabled = 1;
    for (u32 index = 0; index < 10; ++index) {
        mpChartButtons[index]->mInputEnabled = 1;
    }
}

/* 026A625C */
void hdMenuMiiverse_c::disableChartInput() {
    mInputEnabled = 0;
    for (u32 index = 0; index < 10; ++index) {
        mpChartButtons[index]->mInputEnabled = 0;
    }
}

/* 026A62A8 */
void hdMenuMiiverse_c::restoreInputMode() {
    miiverseSetInputMode(sMiiverseEventMgr.mpManager, mInputMode);
}

/* 026A62B8 */
u32 hdMenuMiiverse_c::acceptsInput() const {
    return 1;
}


struct hdMiiverseArchiveSlot_c { gptr<void> mpArchive; };
struct hdMiiverseSaveSlot_c { gptr<hdSaveInfoHolder_c> mpSaveInfo; };
static hdMiiverseArchiveSlot_c sMiiverseArchive;
static hdMiiverseSaveSlot_c sMiiverseSaveInfo;
u32 miiverseArchiveStatus(void*);
hdSavePair_c* miiverseScratchPair(hdSaveHolders_c*);

/* 026A45D0 */
static u32 miiverseGetArchiveStatus() {
    return miiverseArchiveStatus(sMiiverseArchive.mpArchive);
}

/* 026A45DC */
static u32 miiverseIsPrimarySaveMode() {
    hdSavePair_c* pair = miiverseScratchPair(&sMiiverseSaveInfo.mpSaveInfo->mHolders);
    return pair->mPrimary == 0;
}

/* 026A4614 */
static u32 miiverseIsSecondarySaveMode() {
    hdSavePair_c* pair = miiverseScratchPair(&sMiiverseSaveInfo.mpSaveInfo->mHolders);
    return pair->mPrimary == 1;
}

/* WWHD glue (tools/verify/autoglue.py): Miiverse chart controls
 data sMiiverseArchive 1018F4AC nocheck
 data sMiiverseSaveInfo 101F84DC nocheck
 ext 0203A3F0 u32 miiverseArchiveStatus(void*)
 ext 027200F4 hdSavePair_c* miiverseScratchPair(hdSaveHolders_c*)
 data sMiiverseEventMgr 101F4FF0 nocheck
 ext 02615154 void miiverseSetInputMode(EventMgr*, u32)
 ext 026A8B14 void miiverseResetChartButton(hdMiiverseChartButton_l*)
 ext 026AA748 void miiverseResetPreviousButton(hdParts_c*)
 ext 026ABAD4 void miiverseResetNextButton(hdParts_c*)
*/
