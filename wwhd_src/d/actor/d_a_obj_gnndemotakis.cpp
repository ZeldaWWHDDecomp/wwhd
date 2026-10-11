/* Ganondorf central waterfall, private WWHD reconstruction. */
#include "d/actor/d_a_obj_gnndemotakis.h"
const gc_tu_data gc_tu = { .safeString = 0x100297D4 };
const char daObjGnntakis_c::M_arcname[] = "Gnndemotakis";
enum { TEV_TYPE_BG3 = 4 };
static u32 l_demoFrame;
enum { dRes_INDEX_GNNDEMOTAKIS_BDL_GNN_DEMO_TAKI_S_e = 4,
       dRes_INDEX_GNNDEMOTAKIS_BTK_GNN_DEMO_TAKI_S_e = 7 };
/* 0234B8E0 */
BOOL daObjGnntakis_c::create_heap() {
    auto* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNDEMOTAKIS_BDL_GNN_DEMO_TAKI_S_e);
    JUT_ASSERT(156, mdl_data != NULL);
    if (mdl_data == NULL) return FALSE;
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    if (mpModel == NULL) return FALSE;
    auto* btk_data = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_GNNDEMOTAKIS_BTK_GNN_DEMO_TAKI_S_e);
    JUT_ASSERT(163, btk_data != NULL);
    BOOL ret = FALSE;
    if (btk_data != NULL)
        ret = mpBtkAnm.init(mdl_data, btk_data, true, 0, 1.0f, 0, -1, false, 0) != 0;
    // HD: disable this model flag even when animation loading fails.
    auto* model = (gnntakis_model_c*)mpModel.get();
    u32 flags = (u32)model->mFlags & ~1u;
    model->mFlags = flags;
    gnntakis_applyModelFlags(mpModel, flags);
    return ret;
}
/* 0234BA18 */
// HD: free callback preserves the one-argument ABI in autoglue.
static BOOL solidHeapCB(daObjGnntakis_c* actor) { return actor->create_heap(); }
/* 0234BA1C */
void daObjGnntakis_c::init_mtx() {
    // HD: preserve the component load/store order.
    f32 z = scale.z, x = scale.x, y = scale.y;
    mpModel->mBaseScale.x = x;
    mpModel->mBaseScale.z = z;
    mpModel->mBaseScale.y = y;
}
/* 0234BA3C */
s32 daObjGnntakis_c::_create() {
    fopAcM_ct(this, daObjGnntakis_c);
    s32 phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e) return phase;
    if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c*))solidHeapCB, 0)) return cPhs_ERROR_e;
    cullMtx = mpModel == NULL ? 0 : gabi::ea(mpModel->getBaseTRMtx());
    init_mtx();
    s16 start = mpBtkAnm.mFrameCtrl.mStart;
    m2B0 = FALSE;
    mpBtkAnm.mFrameCtrl.mFrame = (f32)start;
    mpBtkAnm.mFrameCtrl.mRate = 0.0f;
    return cPhs_COMPLEATE_e;
}
/* 0234BB48 */
BOOL daObjGnntakis_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return true;
}
/* 0234BB78 */
BOOL daObjGnntakis_c::_execute() {
    // HD: stage equality uses two SafeStrings, including virtual termination calls.
    if (gnntakis_isTowerStage() && dComIfGp_event_runCheck()) {
        auto* play = (gnntakis_play_c*)dComIfGp_get();
        if (gnntakis_startCheck(&play->mEventManager, "g2before")) {
            s32 playing = m2B0;
            u32 demoFrame = l_demoFrame;
            if (!playing) {
                if (demoFrame >= 0x11D9) {
                    s16 start = mpBtkAnm.mFrameCtrl.mStart;
                    m2B0 = TRUE;
                    mpBtkAnm.mFrameCtrl.mFrame = (f32)start;
                    mpBtkAnm.mFrameCtrl.mRate = 1.0f;
                }
            } else if (demoFrame >= 0x126F) {
                fopAcM_delete(this);
            }
        }
    }
    mDoExt_baseAnm_play(&mpBtkAnm);
    return true;
}
/* 0234BD30 */
BOOL daObjGnntakis_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG3, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    // HD: capture the frame before fetching model data.
    f32 frame = mpBtkAnm.mFrameCtrl.mFrame;
    mpBtkAnm.entry(mpModel->getModelData(), frame);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
/* 0234BDA0 */
static s32 Mthd_Create(daObjGnntakis_c* actor) { return actor->_create(); }
/* 0234BDA4 */
static BOOL Mthd_Delete(daObjGnntakis_c* actor) { return actor->_delete(); }
/* 0234BDA8 */
static BOOL Mthd_Execute(daObjGnntakis_c* actor) { return actor->_execute(); }
/* 0234BDAC */
static BOOL Mthd_Draw(daObjGnntakis_c* actor) { return actor->_draw(); }
/* 0234BDB0 */
static void __sinit_d_a_obj_gnndemotakis_cpp() {
    sinit_header_statics(0x10469C90, 0x101C9940);
}
/* 0234BE44 */
// HD: compiler-generated deleting destructor for a trivial header static.
static void headerStatic_dt(void* object, s32 flags) {
    if (object && (flags & 1)) operator_delete(object);
}
/* 0234BE58 */
// HD: deleting destructor; the animation member has a trivial destructor.
static void daObjGnntakis_dt(daObjGnntakis_c* actor, s32 flags) {
    if (actor) {
        gnntakis_actor_dt(actor, 0);
        if (flags & 1) operator_delete(actor);
    }
}
/* 0234BEAC */
static void SafeString_assureTerminationImpl() {}
/* 0234BEB0 */
static BOOL Mthd_IsDelete(daObjGnntakis_c*) { return TRUE; }
daObjGnntakis_c::daObjGnntakis_c() {
    // HD: explicit base/animation construction and the TU virtual table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    mDoExt_btkAnm::ct(&mpBtkAnm);
}
/* WWHD glue (tools/verify/autoglue.py): local HD data and binding
str 10029868 "Gnndemotakis"
str 10029820 "d_a_obj_gnndemotakis.cpp"
str 10029800 "mdl_data != NULL" renamed
str 10029810 "btk_data != NULL" renamed
ext 025D50BC void gnntakis_actor_dt(void*, s32)
ext 027F596C void gnntakis_applyModelFlags(J3DModel*, u32)
ext 025445B8 BOOL gnntakis_startCheck(void*, const char*)
data l_demoFrame 101D600C
str 1002984C "GTower"
str 10029854 "g2before"
*/
