/**
 * d_a_obj_bscurtain.cpp (WWHD) - curtains in Beedle's Shop.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_bscurtain.h"
const gc_tu_data gc_tu = { .safeString = 0x10026484,
    .headerStatics = {0x10469310, 0x101C8160, 0x1046932C} };
const char daObj_Bscurtain_c::M_arcname[4] = "Ptc";
static daObj_Bscurtain_HIO_c l_HIO;

/* 0232897C */
BOOL daObj_Bscurtain_c::CreateHeap() {
    s32 index = dRes_INDEX_PTC_BDL_PTCO_e;
    if ((u32(mParameters) & 0xFF) == 1 && !dComIfGs_isEventBit(EVENT_UNK_1F08))
        index = dRes_INDEX_PTC_BDL_PTCU_e;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, index);
    JUT_ASSERT(0xA9, modelData != NULL);
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    return mpModel != NULL;
}

/* 02328A74 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObj_Bscurtain_c*)i_this)->CreateHeap();
}

/* 02328A78 */
void daObj_Bscurtain_c::set_mtx() {
    static cXyz trans(cXyz::Zero);
    mDoMtx_stack_c::transS(trans.x, trans.y, trans.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 02328BE0 */
cPhs_State daObj_Bscurtain_c::CreateInit() {
    set_mtx();
    fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mpModel));
    return cPhs_COMPLEATE_e;
}

/* 02328C24 */
// HD: _create is emitted directly as the method-table entry.
cPhs_State daObj_Bscurtain_c::_create() {
    fopAcM_ct(this, daObj_Bscurtain_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x10000))
            return cPhs_ERROR_e;
        return CreateInit();
    }
    return ret;
}

/* 02328CC4 */
// HD: _delete is emitted directly as the method-table entry.
bool daObj_Bscurtain_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 02328D18 */
// HD: _draw is emitted directly as the method-table entry.
bool daObj_Bscurtain_c::_draw() {
    dComIfGd_setListBG();
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}

/* 02328CF4 */
// HD: _execute is emitted directly as the method-table entry.
bool daObj_Bscurtain_c::_execute() {
    set_mtx();
    return false;
}

/* 02328DB0 */
// HD: allocation and vtable store around the GameCube HIO constructor.
static daObj_Bscurtain_HIO_c* daObj_Bscurtain_HIO_c_ct(daObj_Bscurtain_HIO_c* i_this) {
    if (i_this == nullptr)
        i_this = (daObj_Bscurtain_HIO_c*)operator_new(sizeof(daObj_Bscurtain_HIO_c));
    if (i_this != nullptr) {
        i_this->mNo = -1;
        i_this->field_0x08 = 0.0f;
        i_this->field_0x0c = 0;
        i_this->__vtbl = 0x1002649C;
    }
    return i_this;
}

/* 02328E0C */
static void __sinit_d_a_obj_bscurtain_cpp() {
    __sinit_header_statics();
    daObj_Bscurtain_HIO_c_ct(&l_HIO);
}
/* 02328EAC */
// HD: HIO deleting destructor.
static void daObj_Bscurtain_HIO_c_dt(daObj_Bscurtain_HIO_c* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 02328EC8 */
// HD: actor deleting destructor.
static void daObj_Bscurtain_c_dt(daObj_Bscurtain_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02328F1C */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 02328EC0 */
static BOOL daObj_BscurtainIsDelete(void*) { return TRUE; }

/* 02328F20 */
// HD: color conversion companion, absent from the GameCube actor TU.
static void colorToFloat(BscurtainFloatColor* out, GXColor* color) {
    f32 r = f32(u8(color->r)) / 255.0f;
    f32 g = f32(u8(color->g)) / 255.0f;
    f32 b = f32(u8(color->b)) / 255.0f;
    f32 a = f32(u8(color->a)) / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10026500 # "Ptc"
str 100264BC # "d_a_obj_bscurtain.cpp"
str 100264D4 "modelData != NULL" renamed # HD: assertion prints null as 0
data l_HIO 1046931C
data trans 1046933C guard 10469348
*/
