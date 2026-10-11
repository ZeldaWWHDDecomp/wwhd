/** Puppet Ganon intro bed (WWHD). Written in the GameCube decompilation's style and
 * verified against cking.rpx. */
#include "d/actor/d_a_obj_ganonbed.h"

const gc_tu_data gc_tu = {.safeString = 0x10029564};
static const char* l_arcname = "Gbed";
enum {
    dRes_INDEX_GBED_BDL_K_GBED_e = 4,
    dRes_INDEX_GBED_DZB_K_GBED_e = 7,
};

daObjGbed_c::daObjGbed_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1002957C; // HD: GHS actor vtable.
}

/* 02349E18 */
BOOL daObjGbed_c::create_heap() {
    J3DModelData* pModelData = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(l_arcname, dRes_INDEX_GBED_BDL_K_GBED_e));
    if (pModelData == NULL) {
        JUT_ASSERT(177, FALSE);
        return FALSE;
    }
    mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000022);
    // HD: allocation failure returns before requesting collision data.
    if (mpModel == NULL) {
        return FALSE;
    }
    cBgD_t* dzb = static_cast<cBgD_t*>(
        dComIfG_getObjectRes(l_arcname, dRes_INDEX_GBED_DZB_K_GBED_e));
    J3DModel* model = mpModel;
    mpBgW = dBgW_NewSet(dzb, dBgW::MOVE_BG_e, model->getBaseTRMtx());
    return mpModel != NULL && mpBgW != NULL;
}

/* 02349F04 */
// HD: heap callback emitted as a free function; tww calls this solidHeapCB.
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return static_cast<daObjGbed_c*>(i_this)->create_heap();
}

/* 02349F08 */
void daObjGbed_c::init_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02349FE8 */
cPhs_State daObjGbed_c::_create() {
    fopAcM_ct(this, daObjGbed_c);
    cPhs_State phase = dComIfG_resLoad(&mPhs, l_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x13E0)) {
            return cPhs_ERROR_e;
        }
        if (dComIfG_Bgsp()->Regist(mpBgW, this)) {
            return cPhs_ERROR_e;
        }
        fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
        init_mtx();
    }
    return phase;
}

/* 0234A0C8 */
static cPhs_State daObjGbed_Create(daObjGbed_c* i_this) {
    return i_this->_create();
}

/* 0234A0CC */
BOOL daObjGbed_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname);
    if (heap != NULL && mpBgW != NULL) {
        if (mpBgW->ChkUsed()) {
            dComIfG_Bgsp()->Release(mpBgW);
        }
        mpBgW = NULL;
    }
    return TRUE;
}

/* 0234A144 */
static BOOL daObjGbed_Delete(daObjGbed_c* i_this) {
    return i_this->_delete();
}

/* 0234A148 */
BOOL daObjGbed_c::_execute() {
    dBgW* background = mpBgW;
    if (background != NULL && background->ChkUsed()) {
        background->Move();
    }
    return TRUE;
}

/* 0234A184 */
static BOOL daObjGbed_Execute(daObjGbed_c* i_this) {
    return i_this->_execute();
}

/* 0234A188 */
BOOL daObjGbed_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 0234A1E4 */
static BOOL daObjGbed_Draw(daObjGbed_c* i_this) {
    return i_this->_draw();
}

/* 0234A1E8 */
static BOOL daObjGbed_IsDelete() {
    return TRUE;
}

/* 0234A1F0 */
static void __sinit_d_a_obj_ganonbed_cpp() {
    sinit_header_statics(0x10469BD8, 0x101C971C);
}

/* 0234A284 */
static void trivialDestructor(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) {
        operator_delete(object);
    }
}

/* 0234A298 */
static void daObjGbed_c_dt(daObjGbed_c* object, u32 flags) {
    if (object != NULL) {
        __dt(static_cast<fopAc_ac_c*>(object));
        if (flags & 1) {
            operator_delete(object);
        }
    }
}

/* 0234A2EC */
static void SafeString_assureTerminationImpl(void* object) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002955C
str 10029590
str 1002958C "FALSE" renamed # HD: original uses "0".
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
*/
