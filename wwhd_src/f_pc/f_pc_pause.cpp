/* Process pause flags and propagation, GameCube-style source verified against WWHD
 * (tools/verify, f_pc_pause). */
#include "f_pc/f_pc_pause.h"

static u32 g_fpcNd_type = 0;

/* 025E0B20 */
s32 fpcPause_IsEnable(void* i_proc, u32 i_flag) {
    // HD: retain the full mask register, including its upper bits.
    base_process_class* pProc = (base_process_class*)i_proc;
    return ((u32)(u8)pProc->mPauseFlag & i_flag) == i_flag;
}

/* 025E0B38 */
s32 fpcPause_Enable(void* i_proc, u32 i_flag) {
    pause_process_node* pNode = (pause_process_node*)i_proc;
    u32 bits = (u8)pNode->mPauseFlag;
    u32 type = pNode->mSubType;
    pNode->mPauseFlag = bits | i_flag;
    if (fpcBs_Is_JustOfType(g_fpcNd_type, type)) {
        fpcLyIt_OnlyHere(pNode->mLayer, fpcPause_Enable, i_flag);
    }
    return 1;
}

/* 025E0BA8 */
s32 fpcPause_Disable(void* i_proc, u32 i_flag) {
    pause_process_node* pNode = (pause_process_node*)i_proc;
    u32 bits = (u8)pNode->mPauseFlag;
    u32 type = pNode->mSubType;
    // HD: subtract the full register before narrowing the stored pause byte.
    pNode->mPauseFlag = bits & (255u - i_flag);
    if (fpcBs_Is_JustOfType(g_fpcNd_type, type)) {
        fpcLyIt_OnlyHere(pNode->mLayer, fpcPause_Disable, i_flag);
    }
    return 1;
}

/* 025E0C1C */
void fpcPause_Init(void* i_proc) {
    ((base_process_class*)i_proc)->mPauseFlag = 0;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025E0C28 */
static void __sinit_f_pc_pause_cpp() {
    sinit_header_statics_rodata(0x1048AB08, 0x101F3E48, 0x1048AB14, 0x1005843C);
}

/* WWHD glue
data g_fpcNd_type 101F3D60
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DED70 void fpcLyIt_OnlyHere(void* layer, pause_callback callback, u32 mask)
*/
