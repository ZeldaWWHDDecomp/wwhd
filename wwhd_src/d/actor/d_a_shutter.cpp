/**
 * d_a_shutter.cpp (WWHD)
 * Object - Sliding double shutter (Tower of the Gods / Ice Ring Isle doors)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shutter.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_shutter.h"

const gc_tu_data gc_tu = {.safeString = 0x1003CA24};

// TU-local views of daShutter_c's GameCube static tables.
static const f32 m_max_speed[2] = {3.0f, 3.0f};
static const f32 m_min_speed[2] = {1.0f, 1.0f};
static const f32 m_move_len[2] = {200.0f, 84.0f};
static const f32 m_width[2] = {220.0f, 82.0f};
static const u32 m_heapsize[2] = {0x1140, 0x3000};
static const s16 m_bdlidx[2] = {4, 4};
static const s16 m_dzbidx[2] = {7, 7};
static const cXyz m_cull_min[2] = {{-500.0f, -100.0f, -50.0f}, {-150.0f, -100.0f, -50.0f}};
static const cXyz m_cull_max[2] = {{500.0f, 250.0f, 50.0f}, {150.0f, 250.0f, 50.0f}};
static const char* m_arcname[2] = {"Htobi1", "Htobi2"};
static const char* m_open_ev_name[2] = {"HYSDOOROPEN", "R03DOOROPEN"};
static const char* m_close_ev_name[2] = {0, "R03DOORCLOSE"};
static const char* m_staff_name[2] = {"Htobi1", "Htobi2"};

/* inlined into daShutter_c::_create */
daShutter_c::daShutter_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 02484634 */
bool daShutter_c::_delete() {
    dComIfG_resDelete(&mPhs, m_arcname[mType]); /* dComIfG_resDeleteDemo */
    if (heap != nullptr) {
        for (int i = 0; i < 2; i++) {
            dComIfG_Bgsp()->Release(mdBgW[i]);
        }
    }
    return TRUE;
}

/* 0248422C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daShutter_c*)i_this)->CreateHeap();
}

/* 024840DC */
BOOL daShutter_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname[mType], m_bdlidx[mType]);
    JUT_ASSERT(289, modelData != NULL);

    for (int i = 0; i < 2; i++) {
        mpModel[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (!mpModel[i]) {
            return FALSE;
        }
        mdBgW[i] = new_dBgW();
        if (mdBgW[i]) {
            cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(m_arcname[mType], m_dzbidx[mType]);
            if (mdBgW[i]->Set(dzb, dBgW::MOVE_BG_e, &mMtx[i]) == true) {
                return FALSE;
            }
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/* 02484368 */
BOOL daShutter_c::Create() {
    fopAcM_SetMtx(this, mpModel[0]->getBaseTRMtx());
    const cXyz* cullMin = &m_cull_min[mType];
    const cXyz* cullMax = &m_cull_max[mType];
    fopAcM_setCullSizeBox(this, cullMin->x, cullMin->y, cullMin->z, cullMax->x, cullMax->y, cullMax->z);
    mSwitchNo = daShutter_prm::getSwitchNo(this);
    // HD: the half-width multiplication is rounded before negation.
    mcXyz[0].x = -(m_width[mType] * 0.5f);
    mcXyz[1].x = m_width[mType] * 0.5f;
    if (!fopAcM_isSwitch(this, mSwitchNo)) {
        field_0x328 = 3;
    } else {
        field_0x328 = 2;
        mcXyz[0].x = mcXyz[0].x - m_move_len[mType];
        mcXyz[1].x = mcXyz[1].x + m_move_len[mType];
    }
    mFrameTimer = 30;
    set_mtx();
    for (int i = 0; i < 2; i++) {
        dComIfG_Bgsp()->Regist(mdBgW[i], this);
        mdBgW[i]->Move();
    }
    if (m_open_ev_name[mType] != nullptr) {
        mOpenEventIdx = dComIfGp_evmng_getEventIdx(m_open_ev_name[mType], 0xff);
    }
    if (m_close_ev_name[mType] != nullptr) {
        mCloseEventIdx = dComIfGp_evmng_getEventIdx(m_close_ev_name[mType], 0xff);
    }
    return TRUE;
}

/* 0248454C */
cPhs_State daShutter_c::_create() {
    fopAcM_ct(this, daShutter_c);
    mType = daShutter_prm::getType(this);
    cPhs_State result = dComIfG_resLoad(&mPhs, m_arcname[mType]);
    if (result == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, m_heapsize[mType])) {
            return cPhs_ERROR_e;
        } else {
            Create();
        }
    }
    return result;
}

/* 02484230 */
void daShutter_c::set_mtx() {
    gabi::Local<cXyz> local_48;
    for (int i = 0; i < 2; i++) {
        mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), &mcXyz[i], local_48);
        ((ShutterModel*)(J3DModel*)mpModel[i])->setBaseScale(scale);
        mDoMtx_stack_c::transS(current.pos.x + local_48->x, current.pos.y + local_48->y, current.pos.z + local_48->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
        ((ShutterModel*)(J3DModel*)mpModel[i])->setBaseTRMtx(mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx[i]);
    }
}

/* 02484D30 */
bool daShutter_c::_execute() {
    if (mFrameTimer >= 0) {
        mFrameTimer = (s8)(mFrameTimer - 1);
    }
    demo();
    set_mtx();
    for (int i = 0; i < 2; i++) {
        mdBgW[i]->Move();
    }
    mbIsSwitch = (u8)fopAcM_isSwitch(this, mSwitchNo);
    return TRUE;
}

/* 0248478C */
void daShutter_c::shutter_move() {
    static char* action_table[] = {"WAIT", "WAIT02", "OPEN", "CLOSE"};
    enum { ACT_WAIT, ACT_WAIT02, ACT_OPEN, ACT_CLOSE };
    s32 actionIndex = dComIfGp_evmng_getMyActIdx(mStaffId, action_table, 4, FALSE, 0);

    f32 maxVel = m_max_speed[mType];
    f32 minVel = m_min_speed[mType];
    f32 fVar4;
    f32 fVar5;

    switch ((u32)actionIndex) {
    case ACT_WAIT:
        mTimer = 0xf;
        field_0x33A = 0;
        dComIfGp_evmng_cutEnd(mStaffId);
        break;
    case ACT_WAIT02:
        if (shutter_calcTimer(&mTimer) == 0) {
            field_0x339 = 0;
            dComIfGp_evmng_cutEnd(mStaffId);
        }
        break;
    case ACT_OPEN:
        field_0x33A = (u8)(field_0x33A + 1);
        if (field_0x339 == 0) {
            field_0x339 = 1;
            if (shutter_getSaveTable(dComIfGp_getStageStagInfo()) == STAGE_TOTG) {
                fopAcM_seStart(this, JA_SE_OBJ_B_SHUTTER_OPEN, 0);
            }
        }
        // HD: GHS fuses the half-width and travel-distance arithmetic.
        fVar4 = cLib_addCalc(&mcXyz[0].x, gabi::fnmsubs(m_width[mType], 0.5f, -m_move_len[mType]), 0.1f, maxVel, minVel);
        fVar5 = cLib_addCalc(&mcXyz[1].x, gabi::fmadds(m_width[mType], 0.5f, m_move_len[mType]), 0.1f, maxVel, minVel);
        if (shutter_getSaveTable(dComIfGp_getStageStagInfo()) == STAGE_TOTG && field_0x33A == 75) {
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
        fVar4 = cLib_addCalc(&mcXyz[0].x, -(m_width[mType] * 0.5f), 0.1f, maxVel, minVel);
        fVar5 = cLib_addCalc(&mcXyz[1].x, m_width[mType] * 0.5f, 0.1f, maxVel, minVel);
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

/* 02484B10 */
void daShutter_c::demo() {
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
    if (eventInfo.checkCommandDemoAccrpt()) {
        if (dComIfGp_evmng_startCheck(mOpenEventIdx) && (field_0x320 == 1)) {
            field_0x320 = 0;
        }
        if (dComIfGp_evmng_startCheck(mCloseEventIdx) && (field_0x320 == 2)) {
            field_0x320 = 0;
        }
        if (dComIfGp_evmng_endCheck(mOpenEventIdx) || dComIfGp_evmng_endCheck(mCloseEventIdx)) {
            dComIfGp_event_reset();
        }
        mStaffId = dComIfGp_evmng_getMyStaffId(m_staff_name[mType], nullptr, 0);
        shutter_move();
    } else if ((field_0x320 == 1) && (mOpenEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mOpenEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    } else if ((field_0x320 == 2) && (mCloseEventIdx != 0)) {
        fopAcM_orderOtherEventId(this, mCloseEventIdx, 0xFF, 0xFFFF, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
}

/* 024846B4 */
bool daShutter_c::_draw() {
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

/* 02484630 */
static cPhs_State daShutter_Create(void* i_this) {
    return ((daShutter_c*)i_this)->_create();
}

/* 024846B0 */
static bool daShutter_Delete(void* i_this) {
    return ((daShutter_c*)i_this)->_delete();
}

/* 02484788 */
static bool daShutter_Draw(void* i_this) {
    return ((daShutter_c*)i_this)->_draw();
}

/* 02484DC8 */
static bool daShutter_Execute(void* i_this) {
    return ((daShutter_c*)i_this)->_execute();
}

/* 02484E74 */
static BOOL daShutter_IsDelete(void*) {
    return TRUE;
}

/* 02484DCC */
// HD: __sinit_d_a_shutter_cpp (HD header statics only)
static void __sinit_d_a_shutter_cpp() {
    sinit_header_statics(0x1046DE40, 0x101D05C4);
}

/* 02484E60 */
// HD: deleting destructor of an empty class (this TU's copy)
static void deleting_dtor_empty(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02484E7C */
// HD: daShutter_c::~daShutter_c (deleting destructor)
static void daShutter_dtor(daShutter_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02484ED0 */
// HD: empty virtual (next to 02484E60 in the same per-TU vtable)
static void shutter_empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1003CA14
str 1003CA1C
str 1003CA4C
str 1003CA58
str 1003CA64
str 1003CA74
str 1003CA84 "modelData != NULL"
str 1003CAA8
str 1003CAB0
str 1003CAB8
str 1003CAC0
data m_max_speed 1003CAD0
data m_min_speed 1003CAD8
data m_move_len 1003CAE0
data m_width 1003CAE8
data m_heapsize 1003CAF0
data m_bdlidx 1003CAF8
data m_dzbidx 1003CAFC
data m_cull_min 1003CB00
data m_cull_max 1003CB18
data m_arcname 101D05E8
data m_open_ev_name 101D05F0
data m_close_ev_name 101D05F8
data m_staff_name 101D0600
data action_table 101D05B4
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 0207A9A0 u8 shutter_calcTimer(be<u8>*)
*/
