/**
 * d_a_mtoge.cpp (WWHD)
 * Object - Forsaken Fortress - Spikes
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mtoge.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_mtoge.h"
#include "gc_api/f_op/f_op_actor_mng.h"

const gc_tu_data gc_tu = {.safeString = 0x1001584C};
static const char M_arcname[] = "Mtoge";
using ActionGuard = be<u32>;
static ActionGuard l_action_guard;
static MtogeActionTable l_action;
// HD: GHS copies this guest-code table into the guarded function-local table.
// Action order: Wait, Hind, Up, Arrival, Down.
static const MtogeActionTable l_action_source = {{
    0x021E010C, 0x021E0120, 0x021E01B8, 0x021E0230, 0x021E02C8
}};

/* 021E0114 */
u8 daMtoge_c::getSwbit() {
    return fopAcM_GetParam(this) & 0xFF;
}

/* 021E0108 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daMtoge_c*>(i_this)->CreateHeap();
}

/* 021DFFF4 */
BOOL daMtoge_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTOGE_BMD_S_MTOGE_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0x70, modelData != NULL) */
        JUT_ASSERT_fail("d_a_mtoge.cpp", 0x70, "modelData != NULL");

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!mpModel) {
        return FALSE;
    }

    mpBgW = new_dBgW();
    if (!mpBgW)
        return FALSE;

    cBgD_t* pData = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTOGE_DZB_S_MTOGE_e, gc_tu.safeString);
    if (!pData) {
        return FALSE;
    }

    calcMtx();

    if (cBgW_Set(mpBgW, pData, cBgW_MOVE_BG_e, J3DModel_getBaseTRMtx(mpModel)) == true) {
        return FALSE;
    }
    return TRUE;
}

/* 021DFF30 */
void daMtoge_c::calcMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y + mHeightOffset, current.pos.z);
    mDoMtx_stack_c::YrotM(home.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 021E047C */
BOOL daMtoge_c::CreateInit() {
    s32 sw = getSwbit();

    if (dBgS_Regist(dComIfG_Bgsp(), mpBgW, this)) /* JUT_ASSERT(0xA8, FALSE) */
        JUT_ASSERT_fail("d_a_mtoge.cpp", 0xA8, "0");

    tevStr.mRoomNo = fopAcM_GetRoomNo(this);

    if (sw == 0xFF) {
        mAction = ACT_WAIT;
    } else if (dComIfGs_isSwitch(sw + 1, fopAcM_GetRoomNo(this))) {
        mAction = ACT_WAIT;
        mHeightOffset = -300.0f;
    } else if (!dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(this))) {
        mAction = ACT_HIND;
        mHeightOffset = -300.0f;
    } else {
        mAction = ACT_ARRIVAL;
    }

    calcMtx();
    dBgW_Move(mpBgW);

    return TRUE;
}

/* 021E05EC */
cPhs_State daMtoge_c::create() {
    /* fopAcM_ct(this, daMtoge_c): placement new; HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x10015864;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhaseProcReq, M_arcname);
    if (phase_state != cPhs_COMPLEATE_e) {
        return phase_state;
    }

    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x15C0U)) {
        return cPhs_ERROR_e;
    }

    CreateInit();

    return cPhs_COMPLEATE_e;
}

/* 021E010C */
BOOL daMtoge_actionWait(daMtoge_c* i_this) {
    return TRUE;
}

/* 021E0120 */
BOOL daMtoge_actionHind(daMtoge_c* i_this) {
    s32 swbit = i_this->getSwbit();

    if (dComIfGs_isSwitch(swbit, fopAcM_GetRoomNo(i_this)) != 0) {
        i_this->setAction(ACT_UP);
        fopAcM_SetSpeedF(i_this, 0.0f);

        fopAcM_seStart(i_this, JA_SE_OBJ_TOGE_OUT, 0);
    }

    return TRUE;
}

/* 021E01B8 */
BOOL daMtoge_actionUp(daMtoge_c* i_this) {
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);

    if (cLib_chaseF(&i_this->mHeightOffset, 0.0f, i_this->speedF)) {
        i_this->setAction(ACT_ARRIVAL);
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}

/* 021E0230 */
BOOL daMtoge_actionArrival(daMtoge_c* i_this) {
    s32 swbit = i_this->getSwbit();

    if (dComIfGs_isSwitch(swbit, fopAcM_GetRoomNo(i_this)) == 0) {
        i_this->setAction(ACT_DOWN);
        fopAcM_SetSpeedF(i_this, 0.0f);

        fopAcM_seStart(i_this, JA_SE_OBJ_TOGE_IN, 0);
    }

    return TRUE;
}

/* 021E02C8 */
BOOL daMtoge_actionDown(daMtoge_c* i_this) {
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);

    if (cLib_chaseF(&i_this->mHeightOffset, -300.0f, i_this->speedF) != 0) {
        i_this->setAction(ACT_HIND);
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}

/* daMtoge_c::draw() inlined */
/* 021E0340 */
static BOOL daMtoge_Draw(daMtoge_c* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);

    mDoExt_modelUpdateDL(i_this->mpModel);

    return TRUE;
}

/* 021E039C */
static BOOL daMtoge_Execute(daMtoge_c* i_this) {
    if (l_action_guard == 0) {
        l_action_guard = 1;
        memcpy_g(&l_action, &l_action_source, sizeof(l_action));
    }
    gabi::call_ptr<BOOL>((u32)l_action.entries[i_this->mAction], i_this);
    return TRUE;
}

/* 021E041C */
static BOOL daMtoge_IsDelete(daMtoge_c*) {
    return TRUE;
}

/* 021E0424 */
static BOOL daMtoge_Delete(daMtoge_c* i_this) {
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhaseProcReq, M_arcname);

    /* i_this->~daMtoge_c(): nothing to do */
    return TRUE;
}

/* 021E06A0 */
static cPhs_State daMtoge_Create(fopAc_ac_c* i_this) {
    return static_cast<daMtoge_c*>(i_this)->create();
}

// HD: compiler-generated and template code of this translation unit.

/* 021E06A4 */
static void __sinit_d_a_mtoge_cpp() {
    sinit_header_statics(0x104658D0, 0x101BB034);
}

/* 021E0738 */
static void daMtoge_SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) operator_delete(p);
}

/* 021E074C */
static void daMtoge_c_dt(daMtoge_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1) operator_delete(p);
    }
}

/* 021E07A0 */
static void daMtoge_SafeString_assureTermination(void* p) {
}

/* WWHD glue
str 100158D0
str 10015874 in 021DFFF4
str 100158AC in 021E047C
str 10015884 "modelData != NULL"
str 100158A8 "0"
data l_action_guard 101FDB3C
data l_action 101FDB40
data l_action_source 101BB020
*/
