/** Framework - Scene Process Manager. WWHD. */
#include "f_op/f_op_scene_mng.h"

static u32 l_scnRqID = -1;
static SceneMngHeaderStatics l_headerStatics;
static const SceneMngPiPair l_piPair;

/* 025DC80C */
scene_class* fopScnM_SearchByID(u32 id) {
    SceneSearchKey key;
    key.id = id;
    return fopScnIt_Judge(fpcSch_JudgeByID, &key);
}

/* 025DC83C */
scene_class* fopScnM_SearchByName(u32 name) {
    SceneSearchKey key;
    // HD: the four-byte stack slot receives only the low halfword of the name.
    key.name = u16(name);
    return fopScnIt_Judge(fpcSch_JudgeForPName, &key);
}

/* 025DC86C */
BOOL fopScnM_ChangeReq(scene_class* i_scene, u32 procName, u32 fadeProcName, u32 fadePeekTime, u32 flags) {
    // HD: retain full register widths and the additional request flags argument.
    u32 sceneRequestID = fopScnRq_Request(SceneRequest_CHANGE, i_scene, procName, 0, fadeProcName, fadePeekTime, flags);
    if (sceneRequestID == -1) {
        return FALSE;
    }
    l_scnRqID = sceneRequestID;
    return TRUE;
}

/* 025DC8D0 */
BOOL fopScnM_DeleteReq(scene_class* i_scene) {
    u32 sceneRequestID = fopScnRq_Request(SceneRequest_DELETE, i_scene, fpcNm_INVALID_e, 0, fpcNm_INVALID_e, 0, 0);
    return sceneRequestID != -1;
}

/* 025DC91C */
BOOL fopScnM_CreateReq(u32 procName, u32 fadeProcName, u32 fadePeekTime, u32 user) {
    // HD: the verified entry point forwards all three process/fade values without narrowing.
    u32 sceneRequestID = fopScnRq_Request(SceneRequest_CREATE, nullptr, procName, user, fadeProcName, fadePeekTime, 0);
    return sceneRequestID != -1;
}

/* 025DC964 */
u32 fopScnM_ReRequest(u32 procName, u32 user) {
    if (l_scnRqID == -1) {
        return 0;
    }
    return fopScnRq_ReRequest(l_scnRqID, procName, user);
}

/* 025DC98C */
void fopScnM_Management() {
    if (!fopScnRq_Handler()) {
        JUT_ASSERT(328, FALSE);
    }
}

/* 025DC9CC */
static void __sinit_f_op_scene_mng_cpp() {
    // HD: preserve the common-header zero stores, pi reads, and constructor registrations.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3798);
    f32 lo = l_piPair.negativePi;
    f32 hi = l_piPair.positivePi;
    l_headerStatics.negativePi = lo;
    l_headerStatics.positivePi = hi;
    SceneMngFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F37A4);
    SceneMngSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F37B0);
}

/* 025DCA60 */
void fopScnM_Init() {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and other-unit calls
data l_scnRqID 101F3794
data l_headerStatics 1048A5DC
data l_piPair 10057D84
str 10057D68 "f_op_scene_mng.cpp"
str 10057D64 "FALSE" renamed
ext 025DC73C scene_class* fopScnIt_Judge(u32 callback, SceneSearchKey* key)
ext 025DCE04 u32 fopScnRq_Request(u32 kind, scene_class* scene, u32 procName, u32 user, u32 fadeProcName, u32 fadePeekTime, u32 flags)
ext 025DCF08 u32 fopScnRq_ReRequest(u32 id, u32 procName, u32 user)
ext 025DCF0C s32 fopScnRq_Handler()
ext 028ED6F8 void SceneMngFirstHeaderObject_ct(u8*)
ext 028EAB2C void SceneMngSecondHeaderObject_ct(u8*)
*/
