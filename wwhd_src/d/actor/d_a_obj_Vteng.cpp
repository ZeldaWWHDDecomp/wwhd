/**
 * d_a_obj_Vteng.cpp
 * Object - Puppet Ganon curtains (intro cutscene)
 *
 * WWHD, in GameCube style and verified against the recompiled original
 * (tools/verify, d_a_obj_Vteng).
 */
#include "d/actor/d_a_obj_Vteng.h"

// HD: this TU's inline SafeString vtable and common header statics.
const gc_tu_data gc_tu = {
    .safeString = 0x1003310C,
    .headerStatics = {0x1046C934, 0x101CDC7C, 0x1046C940},
};

static const char l_arcname[] = "Vteng";

/* inlined into _create */
daObjVteng_c::daObjVteng_c() {
    // HD: GHS constructs the actor base and stores this class's vtable.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 023B59D0 */
void daObjVteng_c::init_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    MTXCopy(mDoMtx_stack_c::get(), &mtx);
}

/* 023B59CC */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjVteng_c*)i_this)->create_heap();
}

/* 023B5898 */
bool daObjVteng_c::create_heap() {
    bool ret = true;

    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_BDL_VTENG_e);
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_BCK_VTENG_e);

    if (!pModelData || !pAnm) {
        JUT_ASSERT(0xB7, FALSE);
        ret = false;
    } else {
        mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, pAnm, J3DFrameCtrl::EMode_NONE, 1.0f, 0x3B, -1,
                                        0, nullptr, 0x00000000, 0x11020203);
        if (!mpMorf) {
            ret = false;
        } else {
            mpModel = mpMorf->getModel();
            mpBgW = dBgW_NewSet((cBgD_t*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_DZB_VTENG_e),
                                dBgW::MOVE_BG_e, &mtx);
            if (!mpBgW)
                ret = false;
        }
    }

    return ret;
}

/* 023B5C28 */
bool daObjVteng_c::jokai_demo() {
    bool ret = false;
    if (dDemo_setDemoData(this, DEMO_ENABLE_ANM_e | DEMO_ENABLE_ANM_FRAME_e, mpMorf, "Vteng", 0, nullptr, 0, 0) == TRUE)
        ret = true;
    return ret;
}

/* 023B5ACC */
cPhs_State daObjVteng_c::_create() {
    fopAcM_ct(this, daObjVteng_c);

    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);

    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0x72a0)) {
            fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
            init_mtx();
            if (dComIfG_Bgsp()->Regist(mpBgW, this)) {
                ret = cPhs_ERROR_e;
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }

    return ret;
}

/* 023B5BAC */
bool daObjVteng_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname);

    if (heap && mpBgW) {
        if (mpBgW->ChkUsed()) {
            dComIfG_Bgsp()->Release(mpBgW);
        }
        mpBgW = nullptr;
    }

    return true;
}

/* 023B5C84 */
bool daObjVteng_c::_execute() {
    if (mpBgW && mpBgW->ChkUsed())
        mpBgW->Move();
    if (!jokai_demo())
        mpMorf->play(nullptr, 0, 0);
    return true;
}

/* 023B5CF4 */
bool daObjVteng_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG3, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mpMorf->updateDL();
    model = gabi::ea(mpModel.get()); /* fopAcM_SetModel */
    return true;
}

/* 023B5BA8 */
static cPhs_State daObjVteng_Create(fopAc_ac_c* i_this) {
    return ((daObjVteng_c*)i_this)->_create();
}

/* 023B5C24 */
static BOOL daObjVteng_Delete(daObjVteng_c* i_this) {
    return i_this->_delete();
}

/* 023B5CF0 */
static BOOL daObjVteng_Execute(daObjVteng_c* i_this) {
    return i_this->_execute();
}

/* 023B5D54 */
static BOOL daObjVteng_Draw(daObjVteng_c* i_this) {
    return i_this->_draw();
}

/* 023B5D58 */
static BOOL daObjVteng_IsDelete(daObjVteng_c* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 023B5D60 */
static void __sinit_d_a_obj_Vteng_cpp() {
    __sinit_header_statics();
}

/* 023B5DF4 */
// HD: this TU's sead::SafeString deleting destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023B5E08 */
// HD: compiler-generated deleting destructor
static void daObjVteng_c_dt(daObjVteng_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 023B5E5C */
// HD: this TU's empty sead::SafeString virtual
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10033104
str 10033150 in 023B5C28
str 1003313C
str 10033138 "FALSE" renamed # HD: "0"
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8)
*/
