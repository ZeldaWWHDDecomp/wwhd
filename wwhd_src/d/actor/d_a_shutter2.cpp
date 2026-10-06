/**
 * d_a_shutter2.cpp (WWHD)
 * Object - Earth Temple (R04) shutter door ("Htobi3").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shutter2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003CB3C
#define SHUTTER2_VTBL 0x1003CC0C /* HD: daShutter2_c vtable */

/* static const tables. The one-entry tables are indexed with mType, but GHS folds the index
 * (arrays of one element): every access reads entry 0. The pointer tables live in .data. */
#define m_arcname(t) STR(gabi::load<u32>(0x101D068C))       /* "Htobi3" */
#define m_open_ev_name(t) gabi::load<u32>(0x101D0690)       /* "R04DOOROPEN" */
#define m_close_ev_name(t) gabi::load<u32>(0x101D0694)      /* "R04DOORCLOSE" */
#define m_staff_name(t) STR(gabi::load<u32>(0x101D0698))    /* "Htobi3" */
#define m_bdlidx(t) gabi::load<s16>(0x1003CC00)
#define m_dzbidx(t) gabi::load<s16>(0x1003CC02)
#define m_heapsize(t) gabi::load<s32>(0x1003CBFC)
#define action_table 0x101D0658u /* static char* action_table[4] in shutter_move */

enum { JA_SE_OBJ_WDUN_R04_STR_OP = 0x69CA };

struct daShutter2_c : dBgS_MoveBgActor {
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    BOOL Execute(Mtx34** pMtx);
    void shutter_move();
    void demo();
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs; /* GameCube 0x2C8 (+0x118) */
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ Mtx34 mMtx;
    /* 0x41C */ be<s32> mDemoState;
    /* 0x420 */ be<s32> mSwitchNo;
    /* 0x424 */ be<s32> m30C;
    /* 0x428 */ be<s32> mStaffId;
    /* 0x42C */ be<s16> mOpenEventIdx;
    /* 0x42E */ be<s16> mCloseEventIdx;
    /* 0x430 */ be<u8> m318;
    /* 0x431 */ be<u8> mType;
    /* 0x432 */ be<u8> mbIsSwitch;
    /* 0x433 */ be<u8> mbIsNearEnemy;
};
WWHD_OFFSET(daShutter2_c, mpModel, 0x3E8);
WWHD_OFFSET(daShutter2_c, mMtx, 0x3EC);
WWHD_OFFSET(daShutter2_c, mDemoState, 0x41C);
WWHD_OFFSET(daShutter2_c, mbIsNearEnemy, 0x433);
WWHD_SIZE(daShutter2_c, 0x434);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D98E8 fopAcM_myRoomSearchEnemy(s8 roomNo) -> fopAc_ac_c* (also open-coded in door10/door12/tbox) */
static inline fopAc_ac_c* fopAcM_myRoomSearchEnemy(s8 roomNo) { return gabi::call<fopAc_ac_c*>(0x025D98E8, roomNo); }
/* 02542EDC dEvent_manager_c::getMyActIdx: the full r3 is compared (cmplwi), so s32 */
static inline s32 dComIfGp_evmng_getMyActIdx_s32(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 0200ECD4 cLib_addCalc(value, target, scale, maxStep, minStep) (local in zl1/ls1/ko1) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}

/* 02485790 */
BOOL daShutter2_c::Delete() {
    WWHD_FUNC(0x02485790, BOOL, this);
    dComIfG_resDelete(&mPhs, m_arcname(mType)); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x02485790, &daShutter2_c::Delete);

/* 02484F94 */
BOOL daShutter2_c::CreateHeap() {
    WWHD_FUNC(0x02484F94, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname(mType), m_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(224, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003CB70), 0xE0, STR(0x1003CB84));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mpModel) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02484F94, &daShutter2_c::CreateHeap);

/* 02485118 */
BOOL daShutter2_c::Create() {
    WWHD_FUNC(0x02485118, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    /* m_cull_min / m_cull_max[0] */
    fopAcM_setCullSizeBox(this, -300.0f, -10.0f, -40.0f, 300.0f, 400.0f, 40.0f);
    mSwitchNo = fopAcM_GetParam(this) & 0xFF; /* daShutter2_prm::getSwitchNo */
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
    if (m_open_ev_name(mType) != 0) {
        mOpenEventIdx = dComIfGp_evmng_getEventIdx(STR(m_open_ev_name(mType)), 0xff);
    }
    if (m_close_ev_name(mType) != 0) {
        mCloseEventIdx = dComIfGp_evmng_getEventIdx(STR(m_close_ev_name(mType)), 0xff);
    }
    return TRUE;
}
VERIFY(0x02485118, &daShutter2_c::Create);

/* 02484ED4 */
cPhs_State daShutter2_c::_create() {
    WWHD_FUNC(0x02484ED4, cPhs_State, this);
    /* fopAcM_ct(this, daShutter2_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = SHUTTER2_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mType = 0;
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, m_arcname(mType));
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(m_arcname(mType), m_dzbidx(mType), 0, m_heapsize(mType));
        /* if (phase_state == cPhs_ERROR_e) phase_state = cPhs_ERROR_e; */
    }
    return phase_state;
}
VERIFY(0x02484ED4, &daShutter2_c::_create);

/* 02485034 */
void daShutter2_c::set_mtx() {
    WWHD_FUNC(0x02485034, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
VERIFY(0x02485034, &daShutter2_c::set_mtx);

/* 02485674 */
BOOL daShutter2_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x02485674, BOOL, this, pMtx);
    demo();
    set_mtx();
    mbIsSwitch = fopAcM_isSwitch(this, mSwitchNo);
    mbIsNearEnemy = fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this)) == nullptr;
    gabi::store<u32>(gabi::ea(pMtx), gabi::ea(&mMtx));
    return TRUE;
}
VERIFY(0x02485674, &daShutter2_c::Execute);

/* 024852CC */
void daShutter2_c::shutter_move() {
    WWHD_FUNC(0x024852CC, void, this);
    f32 fVar3;
    enum {
        ACT_WAIT,
        ACT_OPEN,
        ACT_CLOSE,
        ACT_OPEN_INIT,
    };
    s32 actionIndex = dComIfGp_evmng_getMyActIdx_s32(mStaffId, action_table, 4, FALSE, 0);

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
VERIFY(0x024852CC, &daShutter2_c::shutter_move);

/* 02485430 */
void daShutter2_c::demo() {
    WWHD_FUNC(0x02485430, void, this);
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
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        if (dComIfGp_evmng_startCheck(mOpenEventIdx) && (mDemoState == 1)) {
            mDemoState = 0;
        }
        if (dComIfGp_evmng_startCheck(mCloseEventIdx) && (mDemoState == 2)) {
            mDemoState = 0;
        }
        if (dComIfGp_evmng_endCheck(mOpenEventIdx) || dComIfGp_evmng_endCheck(mCloseEventIdx)) {
            dComIfGp_event_reset();
        }
        mStaffId = dComIfGp_evmng_getMyStaffId(m_staff_name(mType), nullptr, 0);
        shutter_move();
    } else if ((mDemoState == 1) && (mOpenEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mOpenEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo_onCondition(this, 2); /* dEvtCnd_UNK2_e */
    } else if ((mDemoState == 2) && (mCloseEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mCloseEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo_onCondition(this, 2);
    }
}
VERIFY(0x02485430, &daShutter2_c::demo);

/* 024856F8 */
BOOL daShutter2_c::Draw() {
    WWHD_FUNC(0x024856F8, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x024856F8, &daShutter2_c::Draw);

/* method table entries (HD: tail branches) */
/* 02484F78 */
static cPhs_State daShutter2_Create(void* i_this) {
    WWHD_FUNC(0x02484F78, cPhs_State, i_this);
    return ((daShutter2_c*)i_this)->_create();
}
VERIFY(0x02484F78, daShutter2_Create);

/* 02484F7C */
static BOOL daShutter2_Delete(void* i_this) {
    WWHD_FUNC(0x02484F7C, BOOL, i_this);
    return ((daShutter2_c*)i_this)->MoveBGDelete();
}
VERIFY(0x02484F7C, daShutter2_Delete);

/* 02484F80: MoveBGDraw is the virtual Draw */
static BOOL daShutter2_Draw(void* i_this) {
    WWHD_FUNC(0x02484F80, BOOL, i_this);
    return ((daShutter2_c*)i_this)->Draw_v();
}
VERIFY(0x02484F80, daShutter2_Draw);

/* 02484F90 */
static BOOL daShutter2_Execute(void* i_this) {
    WWHD_FUNC(0x02484F90, BOOL, i_this);
    return ((daShutter2_c*)i_this)->MoveBGExecute();
}
VERIFY(0x02484F90, daShutter2_Execute);

/* 02485854 */
static BOOL daShutter2_IsDelete(void*) {
    WWHD_FUNC(0x02485854, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02485854, daShutter2_IsDelete);

/* ---- compiler-generated (no GameCube source) ---- */
/* 02485878: daShutter2_c deleting destructor (vtable +0xC) */
static void daShutter2_c_dt(daShutter2_c* i_this, s32 flags) {
    WWHD_FUNC(0x02485878, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02485878, daShutter2_c_dt);

/* 024858CC: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty; SafeString vtable +0x14) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024858CC, void, (u32)0);
}
VERIFY(0x024858CC, SafeString_assureTerminationImpl);

/* 02485870: dBgS_MoveBgActor::IsDelete (this TU's copy, vtable +0x3C) */
static BOOL MoveBgActor_IsDelete(daShutter2_c*) {
    WWHD_FUNC(0x02485870, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02485870, MoveBgActor_IsDelete);

/* 024857C0 */
static void __sinit_d_a_shutter2_cpp() {
    WWHD_FUNC(0x024857C0, void, (u32)0);
    sinit_header_statics(0x1046DE5C, 0x101D0668);
}
VERIFY(0x024857C0, __sinit_d_a_shutter2_cpp);

/* 0248585C: sead::SafeString deleting destructor (SafeString vtable +0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0248585C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0248585C, SafeString_dt);
