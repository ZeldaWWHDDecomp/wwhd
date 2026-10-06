/**
 * d_a_shutter.cpp (WWHD)
 * Object - Sliding double shutter (Tower of the Gods / Ice Ring Isle doors)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shutter.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003CA24 /* this TU's sead::SafeString vtable */
#define SHUTTER_VTBL 0x1003CA3C

/* static tables */
static f32 m_max_speed(s32 t) { return gabi::load<f32>(0x1003CAD0 + t * 4); }
static f32 m_min_speed(s32 t) { return gabi::load<f32>(0x1003CAD8 + t * 4); }
static f32 m_move_len(s32 t) { return gabi::load<f32>(0x1003CAE0 + t * 4); }
static f32 m_width(s32 t) { return gabi::load<f32>(0x1003CAE8 + t * 4); }
static u32 m_heapsize(s32 t) { return gabi::load<u32>(0x1003CAF0 + t * 4); }
static s16 m_bdlidx(s32 t) { return gabi::load<s16>(0x1003CAF8 + t * 2); }
static s16 m_dzbidx(s32 t) { return gabi::load<s16>(0x1003CAFC + t * 2); }
static cXyz* m_cull_min(s32 t) { return gabi::at<cXyz>(0x1003CB00 + t * 0xC); }
static cXyz* m_cull_max(s32 t) { return gabi::at<cXyz>(0x1003CB18 + t * 0xC); }
static const char* m_arcname(s32 t) { return gabi::at<const char>(gabi::load<u32>(0x101D05E8 + t * 4)); }
static const char* m_open_ev_name(s32 t) { return gabi::at<const char>(gabi::load<u32>(0x101D05F0 + t * 4)); }
static const char* m_close_ev_name(s32 t) { return gabi::at<const char>(gabi::load<u32>(0x101D05F8 + t * 4)); }
static const char* m_staff_name(s32 t) { return gabi::at<const char>(gabi::load<u32>(0x101D0600 + t * 4)); }

enum { JA_SE_OBJ_B_SHUTTER_OPEN = 0x69B4, JA_SE_OBJ_B_SHUTTER_STOP = 0x69B5 };
enum { STAGE_TOTG = 5 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, table, num, force, nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 table, s32 num, s32 force, s32 last) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, table, num, force, last);
}
/* 0200ECD4 cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 0207A9A0 cLib_calcTimer<u8> */
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* the stage info: play+0x5150 (dStage_stageDt_c), virtual getStagInfo at vtable +0x15C (as d_a_fallrock_tag) */
static inline u32 dComIfGp_getStageStagInfo() {
    u32 dt = dComIfGp_ea() + 0x5150;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
}
/* dStage_stagInfo_GetSaveTbl: bits 1..7 of the byte at +9 */
static inline s32 dStage_stagInfo_GetSaveTbl(u32 info) { return (gabi::load<u8>(info + 9) >> 1) & 0x7F; }

struct daShutter_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    void shutter_move();
    void demo();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel[2];
    /* 0x3BC */ gptr<dBgW> mdBgW[2];
    /* 0x3C4 */ Mtx34 mMtx[2];
    /* 0x424 */ cXyz mcXyz[2];
    /* 0x43C */ be<s32> field_0x320;
    /* 0x440 */ be<s32> mSwitchNo;
    /* 0x444 */ be<s32> field_0x328;
    /* 0x448 */ be<s32> mStaffId;
    /* 0x44C */ be<s16> mOpenEventIdx;
    /* 0x44E */ be<s16> mCloseEventIdx;
    /* 0x450 */ be<u8> field_0x334;
    /* 0x451 */ be<u8> mType;
    /* 0x452 */ be<u8> mbIsSwitch;
    /* 0x453 */ be<s8> mFrameTimer;
    /* 0x454 */ be<u8> mTimer;
    /* 0x455 */ be<u8> field_0x339;
    /* 0x456 */ be<u8> field_0x33A;
    /* 0x457 */ be<u8> field_0x33B;
};
WWHD_OFFSET(daShutter_c, mpModel, 0x3B4);
WWHD_OFFSET(daShutter_c, mMtx, 0x3C4);
WWHD_OFFSET(daShutter_c, mcXyz, 0x424);
WWHD_OFFSET(daShutter_c, field_0x320, 0x43C);
WWHD_OFFSET(daShutter_c, mType, 0x451);
WWHD_SIZE(daShutter_c, 0x458);

namespace daShutter_prm {
inline u8 getSwitchNo(daShutter_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u8 getType(daShutter_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0x0F; }
}  // namespace daShutter_prm

/* 02484634 */
bool daShutter_c::_delete() {
    WWHD_FUNC(0x02484634, bool, this);
    dComIfG_resDelete(&mPhs, m_arcname(mType)); /* dComIfG_resDeleteDemo */
    if (heap != nullptr) {
        for (int i = 0; i < 2; i++) {
            cBgS_Release(dComIfG_Bgsp(), mdBgW[i]);
        }
    }
    return TRUE;
}
VERIFY(0x02484634, &daShutter_c::_delete);

/* 0248422C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0248422C, BOOL, i_this);
    return ((daShutter_c*)i_this)->CreateHeap();
}
VERIFY(0x0248422C, CheckCreateHeap);

/* 024840DC */
BOOL daShutter_c::CreateHeap() {
    WWHD_FUNC(0x024840DC, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname(mType), m_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(289, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003CA74), 0x121, STR(0x1003CA84));

    for (int i = 0; i < 2; i++) {
        mpModel[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (!mpModel[i]) {
            return FALSE;
        }
        mdBgW[i] = new_dBgW();
        if (mdBgW[i]) {
            cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(m_arcname(mType), m_dzbidx(mType), SAFESTRING_VTBL);
            if (cBgW_Set(mdBgW[i], dzb, 1 /* cBgW::MOVE_BG_e */, &mMtx[i]) == true) {
                return FALSE;
            }
        } else {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x024840DC, &daShutter_c::CreateHeap);

/* 02484368 */
BOOL daShutter_c::Create() {
    WWHD_FUNC(0x02484368, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel[0])); /* fopAcM_SetMtx */
    cXyz* cullMin = m_cull_min(mType);
    cXyz* cullMax = m_cull_max(mType);
    fopAcM_setCullSizeBox(this, cullMin->x, cullMin->y, cullMin->z, cullMax->x, cullMax->y, cullMax->z);
    mSwitchNo = daShutter_prm::getSwitchNo(this);
    /* HD: -m_width / 2 is computed as -(m_width * 0.5) */
    mcXyz[0].x = -(m_width(mType) * 0.5f);
    mcXyz[1].x = m_width(mType) * 0.5f;
    if (!fopAcM_isSwitch(this, mSwitchNo)) {
        field_0x328 = 3;
    } else {
        field_0x328 = 2;
        mcXyz[0].x = mcXyz[0].x - m_move_len(mType);
        mcXyz[1].x = mcXyz[1].x + m_move_len(mType);
    }
    mFrameTimer = 30;
    set_mtx();
    for (int i = 0; i < 2; i++) {
        dBgS_Regist(dComIfG_Bgsp(), mdBgW[i], this);
        dBgW_Move(mdBgW[i]);
    }
    if (m_open_ev_name(mType) != nullptr) {
        mOpenEventIdx = dComIfGp_evmng_getEventIdx(m_open_ev_name(mType), 0xff);
    }
    if (m_close_ev_name(mType) != nullptr) {
        mCloseEventIdx = dComIfGp_evmng_getEventIdx(m_close_ev_name(mType), 0xff);
    }
    return TRUE;
}
VERIFY(0x02484368, &daShutter_c::Create);

/* 0248454C */
cPhs_State daShutter_c::_create() {
    WWHD_FUNC(0x0248454C, cPhs_State, this);
    /* fopAcM_ct(this, daShutter_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SHUTTER_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mType = daShutter_prm::getType(this);
    cPhs_State result = dComIfG_resLoad(&mPhs, m_arcname(mType));
    if (result == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0248422C /* CheckCreateHeap */, m_heapsize(mType))) {
            return cPhs_ERROR_e;
        } else {
            Create();
        }
    }
    return result;
}
VERIFY(0x0248454C, &daShutter_c::_create);

/* 02484230 */
void daShutter_c::set_mtx() {
    WWHD_FUNC(0x02484230, void, this);
    gabi::Local<cXyz> local_48;
    for (int i = 0; i < 2; i++) {
        mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), &mcXyz[i], local_48);
        J3DModel_setBaseScale(mpModel[i], &scale);
        mDoMtx_stack_c::transS(current.pos.x + local_48->x, current.pos.y + local_48->y, current.pos.z + local_48->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
        J3DModel_setBaseTRMtx(mpModel[i], mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx[i]);
    }
}
VERIFY(0x02484230, &daShutter_c::set_mtx);

/* 02484D30 */
bool daShutter_c::_execute() {
    WWHD_FUNC(0x02484D30, bool, this);
    if (mFrameTimer >= 0) {
        mFrameTimer = (s8)(mFrameTimer - 1);
    }
    demo();
    set_mtx();
    for (int i = 0; i < 2; i++) {
        dBgW_Move(mdBgW[i]);
    }
    mbIsSwitch = (u8)fopAcM_isSwitch(this, mSwitchNo);
    return TRUE;
}
VERIFY(0x02484D30, &daShutter_c::_execute);

/* 0248478C */
void daShutter_c::shutter_move() {
    WWHD_FUNC(0x0248478C, void, this);
    /* static char* action_table[] = {"WAIT", "WAIT02", "OPEN", "CLOSE"} (0x101D05B4) */
    enum { ACT_WAIT, ACT_WAIT02, ACT_OPEN, ACT_CLOSE };
    s32 actionIndex = dComIfGp_evmng_getMyActIdx(mStaffId, 0x101D05B4, 4, FALSE, 0);

    f32 maxVel = m_max_speed(mType);
    f32 minVel = m_min_speed(mType);
    f32 fVar4;
    f32 fVar5;

    switch ((u32)actionIndex) {
    case ACT_WAIT:
        mTimer = 0xf;
        field_0x33A = 0;
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    case ACT_WAIT02:
        if (cLib_calcTimer_u8(&mTimer) == 0) {
            field_0x339 = 0;
            dComIfGp_evmng_cutEnd(mStaffId);
        }
        break;
    case ACT_OPEN:
        field_0x33A = (u8)(field_0x33A + 1);
        if (field_0x339 == 0) {
            field_0x339 = 1;
            if (dStage_stagInfo_GetSaveTbl(dComIfGp_getStageStagInfo()) == STAGE_TOTG) {
                fopAcM_seStart(this, JA_SE_OBJ_B_SHUTTER_OPEN, 0);
            }
        }
        /* -m_move_len - m_width / 2 (fused) */
        fVar4 = cLib_addCalc(&mcXyz[0].x, gabi::fnmsubs(m_width(mType), 0.5f, -m_move_len(mType)), 0.1f, maxVel, minVel);
        fVar5 = cLib_addCalc(&mcXyz[1].x, gabi::fmadds(m_width(mType), 0.5f, m_move_len(mType)), 0.1f, maxVel, minVel);
        if (dStage_stagInfo_GetSaveTbl(dComIfGp_getStageStagInfo()) == STAGE_TOTG && field_0x33A == 75) {
            fopAcM_seStart(this, JA_SE_OBJ_B_SHUTTER_STOP, 0);
            gabi::Local<cXyz> shock;
            shock->x = 0.0f;
            shock->y = 1.0f;
            shock->z = 0.0f;
            dComIfGp_getVibration_StartShock(4, -0x21, shock);
        }
        if (fVar4 != 0.0f) {
            break;
        }
        if (fVar5 != 0.0f) {
            break;
        }
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    case ACT_CLOSE:
        if (field_0x339 == 0) {
            field_0x339 = 1;
        }
        fVar4 = cLib_addCalc(&mcXyz[0].x, -(m_width(mType) * 0.5f), 0.1f, maxVel, minVel);
        fVar5 = cLib_addCalc(&mcXyz[1].x, m_width(mType) * 0.5f, 0.1f, maxVel, minVel);
        if (fVar4 != 0.0f) {
            break;
        }
        if (fVar5 != 0.0f) {
            break;
        }
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    }
}
VERIFY(0x0248478C, &daShutter_c::shutter_move);

/* 02484B10 */
void daShutter_c::demo() {
    WWHD_FUNC(0x02484B10, void, this);
    u8 isSwitch = (u8)fopAcM_isSwitch(this, mSwitchNo);
    if (field_0x320 == 0) {
        if ((isSwitch != mbIsSwitch) && (mFrameTimer < 0)) {
            if (!isSwitch) {
                field_0x320 = 2;
            } else {
                field_0x320 = 1;
            }
        }
    }
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mOpenEventIdx) && (field_0x320 == 1)) {
            field_0x320 = 0;
        }
        if (dComIfGp_evmng_startCheck(mCloseEventIdx) && (field_0x320 == 2)) {
            field_0x320 = 0;
        }
        if (dComIfGp_evmng_endCheck(mOpenEventIdx) || dComIfGp_evmng_endCheck(mCloseEventIdx)) {
            dComIfGp_event_reset();
        }
        mStaffId = dComIfGp_evmng_getMyStaffId(m_staff_name(mType), nullptr, 0);
        shutter_move();
    } else if ((field_0x320 == 1) && (mOpenEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mOpenEventIdx, 0xFF, 0xFFFF, 0, 1);
        gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 2); /* onCondition(dEvtCnd_UNK2_e) */
    } else if ((field_0x320 == 2) && (mCloseEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mCloseEventIdx, 0xFF, 0xFFFF, 0, 1);
        gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 2);
    }
}
VERIFY(0x02484B10, &daShutter_c::demo);

/* 024846B4 */
bool daShutter_c::_draw() {
    WWHD_FUNC(0x024846B4, bool, this);
    for (int i = 0; i < 2; i++) {
        gabi::Local<cXyz> actorPos;
        actorPos->x = current.pos.x;
        actorPos->y = current.pos.y;
        actorPos->z = current.pos.z;
        PSVECAdd(actorPos, &mcXyz[i], actorPos); /* actorPos += mcXyz[i] */
        settingTevStruct(dKy_getEnvlight(), 1 /* TEV_TYPE_BG0 */, actorPos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpModel[i], &tevStr);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(mpModel[i]);
        dComIfGd_setList();
    }
    return TRUE;
}
VERIFY(0x024846B4, &daShutter_c::_draw);

/* 02484630 */
static cPhs_State daShutter_Create(void* i_this) {
    WWHD_FUNC(0x02484630, cPhs_State, i_this);
    return ((daShutter_c*)i_this)->_create();
}
VERIFY(0x02484630, daShutter_Create);

/* 024846B0 */
static bool daShutter_Delete(void* i_this) {
    WWHD_FUNC(0x024846B0, bool, i_this);
    return ((daShutter_c*)i_this)->_delete();
}
VERIFY(0x024846B0, daShutter_Delete);

/* 02484788 */
static bool daShutter_Draw(void* i_this) {
    WWHD_FUNC(0x02484788, bool, i_this);
    return ((daShutter_c*)i_this)->_draw();
}
VERIFY(0x02484788, daShutter_Draw);

/* 02484DC8 */
static bool daShutter_Execute(void* i_this) {
    WWHD_FUNC(0x02484DC8, bool, i_this);
    return ((daShutter_c*)i_this)->_execute();
}
VERIFY(0x02484DC8, daShutter_Execute);

/* 02484E74 */
static BOOL daShutter_IsDelete(void*) {
    WWHD_FUNC(0x02484E74, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02484E74, daShutter_IsDelete);

/* 02484DCC: __sinit_d_a_shutter_cpp (HD header statics only) */
static void __sinit_d_a_shutter_cpp() {
    WWHD_FUNC(0x02484DCC, void, (u32)0);
    sinit_header_statics(0x1046DE40, 0x101D05C4);
}
VERIFY(0x02484DCC, __sinit_d_a_shutter_cpp);

/* 02484E60: deleting destructor of an empty class (this TU's copy) */
static void deleting_dtor_empty(void* p, s32 flags) {
    WWHD_FUNC(0x02484E60, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02484E60, deleting_dtor_empty);

/* 02484E7C: daShutter_c::~daShutter_c (deleting destructor) */
static void daShutter_dtor(daShutter_c* p, s32 flags) {
    WWHD_FUNC(0x02484E7C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02484E7C, daShutter_dtor);

/* 02484ED0: empty virtual (next to 02484E60 in the same per-TU vtable) */
static void shutter_empty_virtual(void* p) {
    WWHD_FUNC(0x02484ED0, void, p);
}
VERIFY(0x02484ED0, shutter_empty_virtual);
