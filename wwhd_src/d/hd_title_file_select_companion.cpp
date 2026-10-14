/* HD save-data state companion; merged file-selection sources stay separate. */
#include "d/hd_title_parts.h"

static const u32 titleSaveDataStringDestructor = 0x026D3740;
extern void titleSaveDataDestroyStringArray(void*, u32, u32, u32, u32, u32);
struct SaveDataPaneNamesArray_l { hdUiSafeString mNames[6]; };
static SaveDataPaneNamesArray_l sSaveDataPaneNames;

/* 026CBDBC */
static u32 titleSaveDataStateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}


/* 026CBEBC */
static void titleSaveDataDestroySaveDataPaneNames() {
    titleSaveDataDestroyStringArray(sSaveDataPaneNames.mNames, 6, sizeof(hdUiSafeString), titleSaveDataStringDestructor, 0, 0);
}

/* WWHD glue (tools/verify/autoglue.py): save-data state key
 ext 028F0164 void titleSaveDataDestroyStringArray(void*,u32,u32,u32,u32,u32)
 data sSaveDataPaneNames 1049A3EC nocheck
*/
