/**
 * d_a_shutter2.cpp (WWHD)
 * Object - Earth Temple (R04) shutter door ("Htobi3").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shutter2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_shutter2.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1003CB3C,
    .headerStatics = {0x1046DE5C, 0x101D0668},
};
// HD: GHS folds every one-entry table access to entry zero, independent of mType.
// Translation-unit tables preserve the guest pointer loads and their image initializers.
static const char* m_arcname[1] = {"Htobi3"};
static const char* m_open_ev_name[1] = {"R04DOOROPEN"};
static const char* m_close_ev_name[1] = {"R04DOORCLOSE"};
static const char* m_staff_name[1] = {"Htobi3"};
static const s16 m_bdlidx[1] = {4};
static const s16 m_dzbidx[1] = {7};
static const s32 m_heapsize[1] = {0xC00};

/* 02485790 */
BOOL daShutter2_c::Delete() {
    dComIfG_resDeleteDemo(&mPhs, m_arcname[0]);
    return TRUE;
}

/* 02484F94 */
BOOL daShutter2_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname[0], m_bdlidx[0]);
    JUT_ASSERT(224, modelData != NULL);
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mpModel) {
        return FALSE;
    }
    return TRUE;
}

/* 02485118 */
BOOL daShutter2_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    /* m_cull_min / m_cull_max[0] */
    fopAcM_setCullSizeBox(this, -300.0f, -10.0f, -40.0f, 300.0f, 400.0f, 40.0f);
    mSwitchNo = daShutter2_prm::getSwitchNo(this);
    mbIsNearEnemy = fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this)) == nullptr;
    if (((mSwitchNo != 0xff) && (fopAcM_isSwitch(this, mSwitchNo))) ||
        ((mSwitchNo == 0xff) && (mbIsNearEnemy))) {
        m30C = 1;
        current.pos.y = home.pos.y + 350.0f;
    } else {
        m30C = 2;
    }
    mbIsSwitch = fopAcM_isSwitch(this, mSwitchNo);
    set_mtx();
    if (m_open_ev_name[0] != 0) {
        mOpenEventIdx = dComIfGp_evmng_getEventIdx(m_open_ev_name[0], 0xff);
    }
    if (m_close_ev_name[0] != 0) {
        mCloseEventIdx = dComIfGp_evmng_getEventIdx(m_close_ev_name[0], 0xff);
    }
    return TRUE;
}

/* 02484ED4 */
cPhs_State daShutter2_c::_create() {
    fopAcM_ct(this, daShutter2_c);
    mType = 0;
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, m_arcname[0]);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(m_arcname[0], m_dzbidx[0], 0, m_heapsize[0]);
        /* if (phase_state == cPhs_ERROR_e) phase_state = cPhs_ERROR_e; */
    }
    return phase_state;
}

/* 02485034 */
void daShutter2_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* 02485674 */
BOOL daShutter2_c::Execute(Mtx34** pMtx) {
    demo();
    set_mtx();
    mbIsSwitch = fopAcM_isSwitch(this, mSwitchNo);
    mbIsNearEnemy = fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this)) == nullptr;
    *reinterpret_cast<gptr<Mtx34>*>(pMtx) = &mMtx; // HD: guest pointer representation.
    return TRUE;
}

/* 024852CC */
void daShutter2_c::shutter_move() {
    static char* action_table[4] = {"WAIT", "OPEN", "CLOSE", "OPEN_INIT"};
    f32 fVar3;
    enum {
        ACT_WAIT,
        ACT_OPEN,
        ACT_CLOSE,
        ACT_OPEN_INIT,
    };
    s32 actionIndex = dComIfGp_evmng_getMyActIdx(mStaffId, action_table, ARRAY_SIZE(action_table), FALSE, 0);

    f32 maxVel = 3.0f; /* m_max_speed[mType] */
    f32 minVel = 1.0f; /* m_min_speed[mType] */

    switch (actionIndex) {
    case ACT_WAIT:
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    case ACT_OPEN_INIT:
        fopAcM_seStart(this, JA_SE_OBJ_WDUN_R04_STR_OP, 0);
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    case ACT_OPEN:
        fVar3 = cLib_addCalc(&current.pos.y, home.pos.y + 350.0f, 0.1f, maxVel, minVel);
        if (fVar3 == 0.0f) {
            dComIfGp_evmng_cutEnd(mStaffId);
        }
        break;
    case ACT_CLOSE:
        fVar3 = cLib_addCalc(&current.pos.y, home.pos.y, 0.1f, maxVel, minVel);
        if (fVar3 == 0.0f) {
            dComIfGp_evmng_cutEnd(mStaffId);
        }
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    }
}

/* 02485430 */
void daShutter2_c::demo() {
    u8 isSwitch = fopAcM_isSwitch(this, mSwitchNo);
    u8 isNearEnemy = fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this)) == nullptr;
    if (mDemoState == 0) {
        if (mSwitchNo != 0xFF) {
            if (isSwitch != mbIsSwitch) {
                if (!isSwitch) {
                    mDemoState = 2;
                } else {
                    mDemoState = 1;
                }
            }
        } else if (isNearEnemy != mbIsNearEnemy) {
            if (!isNearEnemy) {
                mDemoState = 2;
            } else {
                mDemoState = 1;
            }
        }
    }
    if (eventInfo.checkCommandDemoAccrpt()) {
        if (dComIfGp_evmng_startCheck(mOpenEventIdx) && (mDemoState == 1)) {
            mDemoState = 0;
        }
        if (dComIfGp_evmng_startCheck(mCloseEventIdx) && (mDemoState == 2)) {
            mDemoState = 0;
        }
        if (dComIfGp_evmng_endCheck(mOpenEventIdx) || dComIfGp_evmng_endCheck(mCloseEventIdx)) {
            dComIfGp_event_reset();
        }
        mStaffId = dComIfGp_evmng_getMyStaffId(m_staff_name[0], nullptr, 0);
        shutter_move();
    } else if ((mDemoState == 1) && (mOpenEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mOpenEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    } else if ((mDemoState == 2) && (mCloseEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mCloseEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
}

/* 024856F8 */
BOOL daShutter2_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 02484F78 */
static cPhs_State daShutter2_Create(void* i_this) {
    return ((daShutter2_c*)i_this)->_create();
}

/* 02484F7C */
static BOOL daShutter2_Delete(void* i_this) {
    return ((daShutter2_c*)i_this)->MoveBGDelete();
}

/* 02484F80 */
// HD: MoveBGDraw is the virtual Draw
static BOOL daShutter2_Draw(void* i_this) {
    return ((daShutter2_c*)i_this)->Draw_v();
}

/* 02484F90 */
static BOOL daShutter2_Execute(void* i_this) {
    return ((daShutter2_c*)i_this)->MoveBGExecute();
}

/* 02485854 */
static BOOL daShutter2_IsDelete(void*) {
    return TRUE;
}

/* ---- compiler-generated (no GameCube source) ---- */
/* 02485878 */
// HD: daShutter2_c deleting destructor (vtable +0xC)
static void daShutter2_c_dt(daShutter2_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024858CC */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty; SafeString vtable +0x14)
static void SafeString_assureTerminationImpl(void*) {
}

/* 02485870 */
// HD: dBgS_MoveBgActor::IsDelete (this TU's copy, vtable +0x3C)
static BOOL MoveBgActor_IsDelete(daShutter2_c*) {
    return TRUE;
}

/* 024857C0 */
static void __sinit_d_a_shutter2_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 0248585C */
// HD: sead::SafeString deleting destructor (SafeString vtable +0xC)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* WWHD glue (tools/verify/autoglue.py)
str 1003CB70
str 1003CB84 "modelData != NULL" renamed
data m_arcname 101D068C
data m_open_ev_name 101D0690
data m_close_ev_name 101D0694
data m_staff_name 101D0698
data m_bdlidx 1003CC00
data m_dzbidx 1003CC02
data m_heapsize 1003CBFC
data action_table 101D0658
ext 025D98E8 fopAc_ac_c* fopAcM_myRoomSearchEnemy(s8)
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
*/
