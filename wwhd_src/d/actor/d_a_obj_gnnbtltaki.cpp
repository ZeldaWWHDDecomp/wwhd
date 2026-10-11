/* Ganondorf central waterfall, private WWHD reconstruction. */
#include "d/actor/d_a_obj_gnnbtltaki.h"
const gc_tu_data gc_tu = { .safeString = 0x100296A4 };
const char daObjGnnbtaki_c::M_arcname[] = "Gnnbtltaki";
enum { TEV_TYPE_BG3 = 4 };
static u32 l_demoFrame;
enum { dRes_INDEX_GNNBTLTAKI_BDL_GNN_BTL_TAKI_e = 4,
       dRes_INDEX_GNNBTLTAKI_BTK_GNN_BTL_TAKI_e = 7 };
/* 0234AEBC */
BOOL daObjGnnbtaki_c::create_heap() {
    auto* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNBTLTAKI_BDL_GNN_BTL_TAKI_e);
    JUT_ASSERT(0x5A, mdl_data != NULL);
    if (mdl_data == NULL) return FALSE;
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    if (mpModel == NULL) return FALSE;
    auto* btk_data = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNBTLTAKI_BTK_GNN_BTL_TAKI_e);
    JUT_ASSERT(0x61, btk_data != NULL);
    if (btk_data == NULL) return FALSE;
    return mBtkAnm.init(mdl_data, btk_data, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) != 0;
}
/* 0234AFD4 */
// HD: free callback preserves the one-argument ABI in autoglue.
static BOOL solidHeapCB(daObjGnnbtaki_c* actor) { return actor->create_heap(); }
/* 0234AFD8 */
void daObjGnnbtaki_c::init_mtx() {
    // HD: preserve the component load/store order.
    f32 z = scale.z, x = scale.x, y = scale.y;
    mpModel->mBaseScale.x = x;
    mpModel->mBaseScale.z = z;
    mpModel->mBaseScale.y = y;
}
/* 0234AFF8 */
s32 daObjGnnbtaki_c::_create() {
    fopAcM_ct(this, daObjGnnbtaki_c);
    s32 phase = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase != cPhs_COMPLEATE_e) return phase;
    if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c*))solidHeapCB, 0)) return cPhs_ERROR_e;
    u8 type = fopAcM_GetParam(this) & 0xF;
    field_0x2B1 = type == 0xF ? 0 : type;
    cullMtx = mpModel == NULL ? 0 : gabi::ea(mpModel->getBaseTRMtx());
    init_mtx();
    mBtkAnm.mFrameCtrl.mFrame = (f32)(s16)mBtkAnm.mFrameCtrl.mStart;
    if (field_0x2B1 == 1) {
        field_0x2B0 = 0;
        mBtkAnm.mFrameCtrl.mRate = 0.0f;
    } else {
        field_0x2B0 = 1;
        mBtkAnm.mFrameCtrl.mRate = 1.0f;
    }
    return cPhs_COMPLEATE_e;
}
/* 0234B154 */
BOOL daObjGnnbtaki_c::_delete() {
    dComIfG_resDeleteDemo(&mPhase, M_arcname);
    return true;
}
/* 0234B184 */
BOOL daObjGnnbtaki_c::_execute() {
    // HD: SafeString comparison and the global demo-frame number replace GC helpers.
    if (field_0x2B1 == 1 && gnnbtaki_isTowerStage() && dComIfGp_event_runCheck()) {
        auto* play = (gnnbtaki_play_c*)dComIfGp_get();
        if (gnnbtaki_startCheck(&play->mEventManager, "g2before") &&
            !field_0x2B0 && l_demoFrame >= 0x126F) {
            field_0x2B0 = 1;
            mBtkAnm.mFrameCtrl.mFrame = (f32)(s16)mBtkAnm.mFrameCtrl.mStart;
            mBtkAnm.mFrameCtrl.mRate = 1.0f;
        }
    }
    mDoExt_baseAnm_play(&mBtkAnm);
    return true;
}
/* 0234B32C */
BOOL daObjGnnbtaki_c::_draw() {
    if (field_0x2B0) {
    g_env_light.settingTevStruct(TEV_TYPE_BG3, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    // HD: capture the frame before fetching model data.
    f32 frame = mBtkAnm.mFrameCtrl.mFrame;
    mBtkAnm.entry(mpModel->getModelData(), frame);
    mDoExt_modelUpdateDL(mpModel);
    }
    return true;
}
/* 0234B3A8 */
static s32 Mthd_Create(daObjGnnbtaki_c* actor) { return actor->_create(); }
/* 0234B3AC */
static BOOL Mthd_Delete(daObjGnnbtaki_c* actor) { return actor->_delete(); }
/* 0234B3B0 */
static BOOL Mthd_Execute(daObjGnnbtaki_c* actor) { return actor->_execute(); }
/* 0234B3B4 */
static BOOL Mthd_Draw(daObjGnnbtaki_c* actor) { return actor->_draw(); }
/* 0234B3B8 */
static void __sinit_d_a_obj_gnnbtltaki_cpp() {
    sinit_header_statics(0x10469C58, 0x101C9858);
}
/* 0234B44C */
// HD: compiler-generated deleting destructor for a trivial header static.
static void headerStatic_dt(void* object, s32 flags) {
    if (object && (flags & 1)) operator_delete(object);
}
/* 0234B460 */
// HD: deleting destructor; the animation member has a trivial destructor.
static void daObjGnnbtaki_dt(daObjGnnbtaki_c* actor, s32 flags) {
    if (actor) {
        gnnbtaki_actor_dt(actor, 0);
        if (flags & 1) operator_delete(actor);
    }
}
/* 0234B4B4 */
static void SafeString_assureTerminationImpl() {}
/* 0234B4B8 */
static BOOL Mthd_IsDelete(daObjGnnbtaki_c*) { return TRUE; }
daObjGnnbtaki_c::daObjGnnbtaki_c() {
    // HD: explicit base/animation construction and the TU virtual table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    mDoExt_btkAnm::ct(&mBtkAnm);
}
/* WWHD glue (tools/verify/autoglue.py): local HD data and binding
str 10029730 "Gnnbtltaki"
str 100296F0 "d_a_obj_gnnbtltaki.cpp"
str 100296D0 "mdl_data != NULL" renamed
str 100296E0 "btk_data != NULL" renamed
ext 025D50BC void gnnbtaki_actor_dt(void*, s32)
ext 025445B8 BOOL gnnbtaki_startCheck(void*, const char*)
data l_demoFrame 101D600C
str 10029714 "GTower"
str 1002971C "g2before"
*/
