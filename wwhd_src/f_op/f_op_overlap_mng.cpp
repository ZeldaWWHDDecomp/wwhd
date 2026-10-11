/** Overlap Process Manager. WWHD. */
#include "f_op/f_op_overlap_mng.h"

static fopOvlpM_RequestPtr l_fopOvlpM_overlap;
static overlap_request_class l_fopOvlpM_Request;
static OverlapMngHeaderStatics l_headerStatics;
static const OverlapMngPiPair l_piPair;

/* 025DBD24 */
int fopOvlpM_SceneIsStop() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    if (!request) {
        return 0;
    }
    return fopScnPause_Enable(fpcEx_SearchByID(request->mpTask->mScenePId));
}

/* 025DBD74 */
int fopOvlpM_SceneIsStart() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    if (!request) {
        return 0;
    }
    return fopScnPause_Disable(fpcEx_SearchByID(request->mpTask->mScenePId));
}

/* 025DBDC4 */
BOOL fopOvlpM_IsOutReq(overlap_task_class* task) {
    return (task->mRq.mFlags & cReq_flag2Mask) == 2;
}

/* 025DBDDC */
void fopOvlpM_Done(overlap_task_class* task) {
    cReq_Done(&task->mRq);
}

/* 025DBDE4 */
void fopOvlpM_ToldAboutID(u32 id) {
    overlap_request_class* request = l_fopOvlpM_overlap;
    if (request) {
        request->mpTask->mScenePId = id;
    }
}

/* 025DBE00 */
BOOL fopOvlpM_IsPeek() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    return request ? request->mIsPeek : 0;
}

/* 025DBE1C */
BOOL fopOvlpM_IsDone() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    return request ? cReq_Is_Done(&request->base) : FALSE;
}

/* 025DBE38 */
BOOL fopOvlpM_IsDoingReq() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    return request && request->mRequestState == 1;
}

/* 025DBE64 */
BOOL fopOvlpM_ClearOfReq() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    return request ? fopOvlpReq_OverlapClr(request) : FALSE;
}

/* 025DBE80 */
overlap_request_class* fopOvlpM_Request(u32 procName, u32 peekTime, u32 flags) {
    // HD: full register widths and an extra request flags argument.
    if (l_fopOvlpM_overlap) {
        return nullptr;
    }
    overlap_request_class* request = fopOvlpReq_Request(&l_fopOvlpM_Request, procName, peekTime, flags);
    l_fopOvlpM_overlap = request;
    return request;
}

/* 025DBEEC */
void fopOvlpM_Management() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    if (request) {
        switch (fopOvlpReq_Handler(request)) {
        case cPhs_STOP_e:
        case cPhs_COMPLEATE_e:
        case cPhs_ERROR_e:
            l_fopOvlpM_overlap = nullptr;
            break;
        }
    }
}

/* 025DBF3C */
BOOL fopOvlpM_Cancel() {
    overlap_request_class* request = l_fopOvlpM_overlap;
    if (!request) {
        return TRUE;
    }
    // HD: success requires exactly TRUE; the GameCube warning is absent.
    if (fopOvlpReq_Cancel(request) != TRUE) {
        return FALSE;
    }
    l_fopOvlpM_overlap = nullptr;
    return TRUE;
}

/* 025DBFB8 */
static void __sinit_f_op_overlap_mng_cpp() {
    // HD: adjacent common-header initializer, preserving integer pi word copies.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F36D0);
    u32 lower = l_piPair.negativePiBits;
    u32 upper = l_piPair.positivePiBits;
    l_headerStatics.negativePiBits = lower;
    l_headerStatics.positivePiBits = upper;
    OverlapMngFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F36DC);
    OverlapMngSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F36E8);
}

/* 025DC04C */
void fopOvlpM_Init() {
}

/* WWHD glue: data and other-unit calls
 data l_fopOvlpM_overlap 101F36CC
 data l_fopOvlpM_Request 1048A55C
 data l_headerStatics 1048A540
 data l_piPair 10057D1C
 ext 025DE50C scene_class* fpcEx_SearchByID(u32)
 ext 025DCA64 int fopScnPause_Enable(scene_class*)
 ext 025DCAC4 int fopScnPause_Disable(scene_class*)
 ext 0201A330 void cReq_Done(request_base_class*)
 ext 0201A308 BOOL cReq_Is_Done(request_base_class*)
 ext 025DC4C4 BOOL fopOvlpReq_OverlapClr(overlap_request_class*)
 ext 025DC344 overlap_request_class* fopOvlpReq_Request(overlap_request_class*, u32 procName, u32 peekTime, u32 flags)
 ext 025DC418 u32 fopOvlpReq_Handler(overlap_request_class*)
 ext 025DC488 BOOL fopOvlpReq_Cancel(overlap_request_class*)
 ext 028ED6F8 void OverlapMngFirstHeaderObject_ct(u8*)
 ext 028EAB2C void OverlapMngSecondHeaderObject_ct(u8*)
*/
