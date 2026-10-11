/* Ganondorf central waterfall, private WWHD reconstruction. */
#include "d/actor/d_a_obj_gnndemotakie.h"
const gc_tu_data gc_tu = { .safeString = 0x10029744 };
const char daObjGnntakie_c::M_arcname[] = "Gnndemotakie";
enum { TEV_TYPE_BG3 = 4 };
enum { dRes_INDEX_GNNDEMOTAKIE_BDL_GNN_DEMO_TAKI_E_e = 4,
       dRes_INDEX_GNNDEMOTAKIE_BTK_GNN_DEMO_TAKI_E_e = 7 };
/* 0234B4C0 */
BOOL daObjGnntakie_c::create_heap() {
    auto* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNDEMOTAKIE_BDL_GNN_DEMO_TAKI_E_e);
    JUT_ASSERT(0x5A, mdl_data != NULL);
    if (mdl_data == NULL) return FALSE;
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    if (mpModel == NULL) return FALSE;
    auto* btk_data = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNDEMOTAKIE_BTK_GNN_DEMO_TAKI_E_e);
    JUT_ASSERT(0x61, btk_data != NULL);
    if (btk_data == NULL) return FALSE;
    return mpBtkAnm.init(mdl_data, btk_data, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) != 0;
}
/* 0234B5D8 */
// HD: free callback preserves the one-argument ABI in autoglue.
static BOOL solidHeapCB(daObjGnntakie_c* actor) { return actor->create_heap(); }
/* 0234B5DC */
void daObjGnntakie_c::init_mtx() {
    // HD: preserve the component load/store order.
    f32 z = scale.z, x = scale.x, y = scale.y;
    mpModel->mBaseScale.x = x;
    mpModel->mBaseScale.z = z;
    mpModel->mBaseScale.y = y;
}
/* 0234B5FC */
s32 daObjGnntakie_c::_create() {
    fopAcM_ct(this, daObjGnntakie_c);
    s32 phase = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase != cPhs_COMPLEATE_e) return phase;
    if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c*))solidHeapCB, 0)) return cPhs_ERROR_e;
    cullMtx = mpModel == NULL ? 0 : gabi::ea(mpModel->getBaseTRMtx());
    init_mtx();
    mpBtkAnm.mFrameCtrl.mFrame = (f32)(s16)mpBtkAnm.mFrameCtrl.mStart;
    mpBtkAnm.mFrameCtrl.mRate = 1.0f;
    return cPhs_COMPLEATE_e;
}
/* 0234B700 */
bool daObjGnntakie_c::_delete() {
    dComIfG_resDeleteDemo(&mPhase, M_arcname);
    return true;
}
/* 0234B730 */
bool daObjGnntakie_c::_execute() {
    mDoExt_baseAnm_play(&mpBtkAnm);
    return true;
}
/* 0234B758 */
bool daObjGnntakie_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG3, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    // HD: capture the frame before fetching model data.
    f32 frame = mpBtkAnm.mFrameCtrl.mFrame;
    mpBtkAnm.entry(mpModel->getModelData(), frame);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
/* 0234B7C8 */
static s32 Mthd_Create(daObjGnntakie_c* actor) { return actor->_create(); }
/* 0234B7CC */
static bool Mthd_Delete(daObjGnntakie_c* actor) { return actor->_delete(); }
/* 0234B7D0 */
static bool Mthd_Execute(daObjGnntakie_c* actor) { return actor->_execute(); }
/* 0234B7D4 */
static bool Mthd_Draw(daObjGnntakie_c* actor) { return actor->_draw(); }
/* 0234B7D8 */
static void __sinit_d_a_obj_gnndemotakie_cpp() {
    sinit_header_statics(0x10469C74, 0x101C98CC);
}
/* 0234B86C */
// HD: compiler-generated deleting destructor for a trivial header static.
static void headerStatic_dt(void* object, u32 flags) {
    if (object && (flags & 1)) operator_delete(object);
}
/* 0234B880 */
// HD: deleting destructor; the animation member has a trivial destructor.
static void daObjGnntakie_dt(daObjGnntakie_c* actor, u32 flags) {
    if (actor) {
        gnntakie_actor_dt(actor, 0);
        if (flags & 1) operator_delete(actor);
    }
}
/* 0234B8D4 */
static void SafeString_assureTerminationImpl(void*) {}
/* 0234B8D8 */
static BOOL Mthd_IsDelete(daObjGnntakie_c*) { return TRUE; }
daObjGnntakie_c::daObjGnntakie_c() {
    // HD: explicit base/animation construction and the TU virtual table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    mDoExt_btkAnm::ct(&mpBtkAnm);
}
/* WWHD glue (tools/verify/autoglue.py): local HD data and binding
str 100297C0 "Gnndemotakie"
str 10029790 "d_a_obj_gnndemotakie.cpp"
str 10029770 "mdl_data != NULL" renamed
str 10029780 "btk_data != NULL" renamed
ext 025D50BC void gnntakie_actor_dt(void*, s32)
*/
