/**
 * d_a_obj_otble.cpp (WWHD) - generic brown wooden tables.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_otble.h"
using daObj_Otble::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x1002E32C,
    .objAcch = {0x1002E344, 0x1002E364, 0x1002E354},
    .objAcchDt = {0x1002E354, 0x1002E364},
    .headerStatics = {0x1046BB84, 0x101CC1D8, 0x1046BB90} };
static const u32 l_heapsize_init[2] = {0x1240, 0x1240};

/* 02380D28 */
BOOL Act_c::_createHeap() {
    // dRes_INDEX_OKMONO_BDL_OTABLE_e, dRes_INDEX_OKMONO_BDL_OTABLEL_e
    static const s32 bdl[2] = {5, 6};
    // dRes_INDEX_OKMONO_DZB_OTBLE_e, dRes_INDEX_OKMONO_DZB_OTBLE_L_e
    static const s32 dzb[2] = {9, 10};
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Okmono", bdl[m294]);
    JUT_ASSERT(0xC1, modelData != NULL); // HD: assertion line differs from GameCube.
    mModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mModel == NULL)
        return FALSE;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXCopy(mDoMtx_stack_c::get(), &m2A4);
    mBgW = new_dBgW();
    if (mBgW == NULL)
        return FALSE;
    cBgD_t* bgw_data = (cBgD_t*)dComIfG_getObjectRes("Okmono", dzb[m294]);
    if (mBgW->Set(bgw_data, dBgW::MOVE_BG_e, &m2A4))
        return FALSE;
    return TRUE;
}

/* 02380E70 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) { return ((Act_c*)i_this)->_createHeap(); }

/* 02380E74 */
void Act_c::set_mtx() {
    mObjAcch.CrrPos(*dComIfG_Bgsp());
    tevStr.mRoomNo = current.roomNo;
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &m2A4);
    mBgW->Move();
}

/* 02380F8C */
void Act_c::CreateInit() {
    dComIfG_Bgsp()->Regist(mBgW, this);
    mObjAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mObjAcchCir, &speed, nullptr, nullptr);
    // HD: the three disabled collision checks are folded into one flags update.
    mObjAcch.m_flags |= dBgS_Acch::WALL_NONE | dBgS_Acch::WATER_NONE | dBgS_Acch::ROOF_NONE;
    gravity = -6.5f;
    fopAcM_posMoveF(this, nullptr);
    set_mtx();
    fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mModel));
    if (m294 == 1)
        fopAcM_setCullSizeBox(this, -100.0f, -0.0f, -200.0f, 100.0f, 150.0f, 200.0f);
    else
        fopAcM_setCullSizeBox(this, -100.0f, -0.0f, -100.0f, 100.0f, 150.0f, 100.0f);
    fopAcM_setCullSizeFar(this, 10.0f);
}

/* 023810CC */
// HD: the M_attr execution check was removed.
BOOL Act_c::_execute() {
    fopAcM_posMoveF(this, nullptr);
    set_mtx();
    return TRUE;
}

/* 02381108 */
// HD: the M_attr draw check and simple shadow were removed.
BOOL Act_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return TRUE;
}

/* 023811A0 */
// HD: _create is emitted directly as the method-table entry.
cPhs_State Act_c::_create() {
    static u32 heapsize_initialized;
    static OtbleHeapSizes heapsize;
    // HD: set the guard before the memcpy call, whose side effects are observable.
    if (!heapsize_initialized) {
        heapsize_initialized = 1;
        memcpy_g(&heapsize, l_heapsize_init, sizeof(heapsize));
    }
    fopAcM_ct(this, Act_c);
    u8 variant = u32(mParameters) & 0xFF;
    if (variant > 1)
        variant = 1;
    m294 = variant;
    cPhs_State ret = dComIfG_resLoad(&mPhase, "Okmono");
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, createHeap_CB, heapsize.values[m294]))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return ret;
}

/* 023812FC */
// HD: _delete is emitted directly as the method-table entry.
bool Act_c::_delete() {
    dComIfG_resDelete(&mPhase, "Okmono");
    if (mBgW != NULL && mBgW->ChkUsed())
        dComIfG_Bgsp()->Release(mBgW);
    return true;
}

/* 02381360 */
static BOOL Mthd_Execute(void* i_this) { return ((Act_c*)i_this)->_execute(); }
/* 02381364 */
static BOOL Mthd_Draw(void* i_this) { return ((Act_c*)i_this)->_draw(); }
/* 02381368 */
static void __sinit_d_a_obj_otble_cpp() { __sinit_header_statics(); }
/* 023813FC */
// HD: compiler-emitted deleting destructor of a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 02381410 */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_(void*) {}
/* 02381414 */
// HD: destructor of the embedded collision helpers and actor base.
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mObjAcchCir);
        __dt(&i_this->mObjAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02381498 */
static BOOL Mthd_IsDelete(void*) { return TRUE; }

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 1002E394 in 02380D28 # "Okmono"
str 1002E324 in 023811A0 # "Okmono"
str 1002E3F0 in 023812FC # "Okmono"
str 1002E39C # "d_a_obj_otble.cpp"
str 1002E3B0 "modelData != NULL" renamed # HD: assertion prints null as 0
data bdl 1002E384
data dzb 1002E38C
data l_heapsize_init 1002E318
data heapsize 101FDC44
data heapsize_initialized 101FDC40
ext 024EEEB8 u32 dBgS_GetPolyColor(dBgS*, cBgS_PolyInfo*)
*/
