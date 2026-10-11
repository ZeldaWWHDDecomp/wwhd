/* Tower of the Gods exterior; actual HD TU023A6490..023A6AF7.
 * Includes trailing destructor and SafeString virtual noop. */
#include "d/actor/d_a_obj_tower.h"
enum { dRes_INDEX_X_TOWER_BDL_X_TOWER_e = 4, dRes_INDEX_X_TOWER_DZB_X_TOWER_e = 7 };
const gc_tu_data gc_tu = {.safeString = 0x10031644};
static u32 l_demoManager;
/* 023A6490 */
BOOL daObjTower_c::CreateHeap() {
    auto *modelData =
        (J3DModelData *)dComIfG_getObjectRes(STR(0x10031688), dRes_INDEX_X_TOWER_BDL_X_TOWER_e);
    if (!modelData)
        JUT_ASSERT_fail(STR(0x10031690), 86, STR(0x100316A4));
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (!mpModel)
        return 0;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
    mpBgW = new_dBgW();
    if (!mpBgW)
        return 0;
    auto *collision =
        (cBgD_t *)dComIfG_getObjectRes(STR(0x10031688), dRes_INDEX_X_TOWER_DZB_X_TOWER_e);
    return mpBgW->Set(collision, dBgW::MOVE_BG_e, &mMtx) ? 0 : 1;
}
/* 023A65C8 */
BOOL CheckCreateHeap(fopAc_ac_c *actor) {
    return ((daObjTower_c *)actor)->CreateHeap();
}
/* 023A65CC */
void daObjTower_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 023A66A4 */
void daObjTower_c::CreateInit() {
    J3DModel *model = mpModel;
    cullMtx = model ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0;
    fopAcM_setCullSizeBox(this, -30000.f, -5000.f, -30000.f, 30000.f, 40000.f, 30000.f);
    dComIfG_Bgsp()->Regist(mpBgW, this);
    mBackgroundRegistered = 1;
    set_mtx();
}
/* 023A6734 */
s32 daObjTower_c::_create() {
    fopAcM_ct(this, daObjTower_c);
    mBackgroundRegistered = 0;
    if (!dComIfGs_isEventBit(0x1E40))
        return cPhs_STOP_e;
    s32 phase = dComIfG_resLoad(&mPhs, STR(0x10031638));
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x1C6C0))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 023A683C */
BOOL daObjTower_c::_delete() {
    // HD: any nonzero registration flag releases the background.
    if (mBackgroundRegistered) {
        dComIfG_Bgsp()->Release(mpBgW);
    }
    if (mCreateResult != cPhs_STOP_e)
        dComIfG_resDelete(&mPhs, STR(0x100316C8));
    return 1;
}
/* 023A68A0 */
BOOL daObjTower_c::_draw() {
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    return 1;
}
/* 023A6938 */
BOOL daObjTower_c::_execute() {
    u8 id = demoActorID;
    // HD: the inline demo lookup rejects IDs above 0x20 and asserts on a missing manager.
    if (id && id <= 0x20) {
        u32 manager = l_demoManager;
        if (!manager) {
            JUT_ASSERT_fail(STR(0x1003167C), 570, STR(0x1003166C));
            manager = l_demoManager;
        }
        tower_demo_actor_c *actor = tower_getDemoActor(tower_demoManager(manager), id);
        if (actor && (actor->mEnable & tower_demo_actor_c::ENABLE_TRANS_e)) {
            current.pos.x = actor->mTrans.x;
            current.pos.y = actor->mTrans.y;
            current.pos.z = actor->mTrans.z;
        }
    }
    set_mtx();
    return 1;
}
/* 023A69F0 */
void tower_staticInit() {
    sinit_header_statics(0x1046C578, 0x101CD550);
}
/* 023A6A84 */
void tower_staticDtor(void *object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 023A6A98 */
BOOL tower_IsDelete(daObjTower_c *tower) {
    return 1;
}
/* 023A6AA0 */
void tower_destructor(daObjTower_c *tower, u32 flags) {
    if (tower) {
        tower_actor_dt(tower, 0);
        if (flags & 1)
            operator_delete(tower);
    }
}
/* 023A6AF4 */
void tower_stringNoop(void *object) {
}

daObjTower_c::daObjTower_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
data l_demoManager 101D5FFC
ext 02526E70 tower_demo_actor_c* tower_getDemoActor(void*, u8)
ext 025D50BC void tower_actor_dt(void*, s32)
*/
