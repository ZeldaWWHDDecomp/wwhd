/** HD Picto Box best-shot label and pictograph list parts. Binary-derived; private source. */
#pragma once
#include "d/hd_parts_in_out_loop.h"
#include "d/hd_parts_text_pane.h"

inline hdPartsPaneSRT_c* hdPictoRoot(hdParts_c* parts) { return (hdPartsPaneSRT_c*)parts->mpLayout->lyt()->mpRootPane.get(); }

struct hdBestShotMark_c : hdPartsInOutLoop_c {
    gptr<hdTextPane_c> mpBestShotText;
    be<u32> mLabelStorage;
    static hdBestShotMark_c* construct(hdBestShotMark_c*);
    static void destroy(hdBestShotMark_c*, s32);
    void inEnter(); void inExecute(); void loopEnter(); void loopExecute();
    void outEnter(); void outExecute(); void invisibleEnter(); void invisibleExecute();
    void initializePane();
    u32 initialize(u32, u32, hdPartsDesc_c*);
};
WWHD_OFFSET(hdBestShotMark_c, mpBestShotText, 0x5C);
WWHD_OFFSET(hdBestShotMark_c, mLabelStorage, 0x60);
WWHD_SIZE(hdBestShotMark_c, 0x64);

void hdPictoDestroyStrings(void*, u32, u32, u32, u32, u32);
void hdPictoRegisterGlobal(void*);
void hdPictoRepeatAnimation(hdAnimSet*, u32, u32);

void hdPictoInitializeLifetime(void*);
void hdPictoInitializeExitByte(void*);

/* guest 1048EEE8: the best-shot mark parts name "BestShotMark_00" (defined in hd_screen_best_shot_mark_init.cpp) */
extern hdUiSafeString& sBestShotPartNameBestShotInit;
