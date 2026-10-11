/** Windfall bomb-shop counter flap (WWHD), written in the GameCube decompilation's style. */
#include "d/actor/d_a_obj_pbco.h"

const gc_tu_data gc_tu = {.safeString = 0x1002E554};
const char* daObj_Pbco_c::M_arcname = "Pbco";
static daObj_Pbco_HIO_c l_HIO;

enum {
    dRes_INDEX_PBCO_BDL_PBCO_e = 4,
    dRes_INDEX_PBCO_BDL_PBC2_e = 5,
    dRes_INDEX_PBCO_DZB_PBCO_e = 8,
    dItemNo_PEARL_NAYRU_e = 0x69,
};
namespace dSv_event_flag_c {
enum { ENDLESS_NIGHT = 0xA02 };
}

static bool counterClosed() {
    return dComIfGs_isEventBit(dSv_event_flag_c::ENDLESS_NIGHT) &&
           !checkItemGet(dItemNo_PEARL_NAYRU_e, TRUE);
}

daObj_Pbco_c::daObj_Pbco_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1002E57C; // HD: this translation unit's GHS actor vtable.
}

/* 02381EF4 */
BOOL daObj_Pbco_c::CreateHeap() {
    J3DModelData* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes(
        M_arcname, counterClosed() ? dRes_INDEX_PBCO_BDL_PBCO_e : dRes_INDEX_PBCO_BDL_PBC2_e));
    JUT_ASSERT(0xA9, modelData != NULL);
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    // HD: the event and item checks are repeated after model creation.
    if (counterClosed()) {
        mpBgW = NULL;
    } else {
        cBgD_t* dzb = static_cast<cBgD_t*>(
            dComIfG_getObjectRes(M_arcname, dRes_INDEX_PBCO_DZB_PBCO_e));
        J3DModel* model = mpModel;
        mpBgW = dBgW_NewSet(dzb, dBgW::MOVE_BG_e, model->getBaseTRMtx());
        if (mpBgW == NULL) {
            return FALSE;
        }
    }
    return mpModel != NULL;
}

/* 02382078 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daObj_Pbco_c*>(i_this)->CreateHeap();
}

/* 0238207C */
void daObj_Pbco_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0238215C */
cPhs_State daObj_Pbco_c::CreateInit() {
    set_mtx();
    J3DModel* model = mpModel;
    dBgW* bg = mpBgW;
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    if (bg != NULL) {
        // HD: reload mpBgW after obtaining the play object, as GHS emitted it.
        dBgS* bgs = dComIfG_Bgsp();
        bgs->Regist(mpBgW, this);
    }
    return cPhs_COMPLEATE_e;
}

/* 023821C0 */
static cPhs_State daObj_PbcoCreate(daObj_Pbco_c* i_this) {
    fopAcM_ct(i_this, daObj_Pbco_c);
    cPhs_State phase = dComIfG_resLoad(&i_this->mPhs, daObj_Pbco_c::M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0x10000)) {
            return i_this->CreateInit();
        }
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 02382260 */
static BOOL daObj_PbcoDelete(daObj_Pbco_c* i_this) {
    if (i_this->mpBgW != NULL) {
        dComIfG_Bgsp()->Release(i_this->mpBgW);
    }
    dComIfG_resDelete(&i_this->mPhs, daObj_Pbco_c::M_arcname);
    return TRUE;
}

/* 023822B8 */
static BOOL daObj_PbcoExecute(daObj_Pbco_c* i_this) {
    dBgW* bg = i_this->mpBgW;
    if (bg != NULL) {
        bg->Move();
    }
    i_this->set_mtx();
    return FALSE;
}

/* 023822FC */
static BOOL daObj_PbcoDraw(daObj_Pbco_c* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}

/* 02382358 */
// HD: GHS's constructor entry also supports allocating the eight-byte HIO object.
static daObj_Pbco_HIO_c* daObj_Pbco_HIO_c_ct(daObj_Pbco_HIO_c* i_this) {
    if (i_this == NULL) {
        i_this = static_cast<daObj_Pbco_HIO_c*>(operator_new(sizeof(daObj_Pbco_HIO_c)));
    }
    if (i_this != NULL) {
        i_this->mNo = -1;
        i_this->mVtable = 0x1002E56C;
    }
    return i_this;
}

/* 023823A0 */
static void __sinit_d_a_obj_pbco_cpp() {
    sinit_header_statics(0x1046BBC4, 0x101CC2E0);
    daObj_Pbco_HIO_c_ct(&l_HIO);
}

/* 02382440 */
static void daObj_Pbco_HIO_c_dt(void* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02382454 */
static BOOL daObj_PbcoIsDelete(daObj_Pbco_c* i_this) {
    return TRUE;
}

/* 0238245C */
static void daObj_Pbco_c_dt(daObj_Pbco_c* i_this, s32 flags) {
    if (i_this != NULL) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 023824B0 */
static void SafeString_assureTerminationImpl() {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002E5C8
str 1002E58C
str 1002E5A0 "modelData != NULL" renamed # HD: original uses 0.
data l_HIO 1046BBBC
*/
