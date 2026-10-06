/**
 * d_a_mtoge.cpp (WWHD)
 * Object - Forsaken Fortress - Spikes
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mtoge.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "bindings.h"

/* guest string literals (.rodata) */
#define M_arcname gabi::at<const char>(0x100158D0) /* "Mtoge" */
#define FILE_NAME gabi::at<const char>(0x10015874) /* "d_a_mtoge.cpp" */
#define SAFESTRING_VTBL 0x1001584C /* this TU's copy of the sead::SafeString vtable */

enum {
    dRes_INDEX_MTOGE_BMD_S_MTOGE_e = 4,
    dRes_INDEX_MTOGE_DZB_S_MTOGE_e = 7,
};

enum Action {
    ACT_WAIT,
    ACT_HIND,
    ACT_UP,
    ACT_ARRIVAL,
    ACT_DOWN,
};

enum {
    JA_SE_OBJ_TOGE_OUT = 0x6976,
    JA_SE_OBJ_TOGE_IN = 0x6977,
};

struct daMtoge_c : fopAc_ac_c {
    void setAction(u8 action) { mAction = action; }

    u8 getSwbit();
    BOOL CreateHeap();
    void calcMtx();
    BOOL CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhaseProcReq; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ be<u8> mAction;
    /* 0x3BD */ be<s8> m2A1;
    /* 0x3BE */ be<s8> m2A2;
    /* 0x3BF */ be<s8> m2A3;
    /* 0x3C0 */ be<f32> mHeightOffset;
};
WWHD_OFFSET(daMtoge_c, mpModel, 0x3B4);
WWHD_OFFSET(daMtoge_c, mHeightOffset, 0x3C0);

/* 021E0114 */
u8 daMtoge_c::getSwbit() {
    WWHD_FUNC(0x021E0114, u8, this);
    return fopAcM_GetParam(this) & 0xFF;
}
VERIFY(0x021E0114, &daMtoge_c::getSwbit);

/* 021E0108 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E0108, BOOL, i_this);
    return static_cast<daMtoge_c*>(i_this)->CreateHeap();
}
VERIFY(0x021E0108, CheckCreateHeap);

/* 021DFFF4 */
BOOL daMtoge_c::CreateHeap() {
    WWHD_FUNC(0x021DFFF4, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTOGE_BMD_S_MTOGE_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x70, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x70, gabi::at<const char>(0x10015884));

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!mpModel) {
        return FALSE;
    }

    mpBgW = new_dBgW();
    if (!mpBgW)
        return FALSE;

    cBgD_t* pData = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTOGE_DZB_S_MTOGE_e, SAFESTRING_VTBL);
    if (!pData) {
        return FALSE;
    }

    calcMtx();

    if (cBgW_Set(mpBgW, pData, cBgW_MOVE_BG_e, J3DModel_getBaseTRMtx(mpModel)) == true) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x021DFFF4, &daMtoge_c::CreateHeap);

/* 021DFF30 */
void daMtoge_c::calcMtx() {
    WWHD_FUNC(0x021DFF30, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y + mHeightOffset, current.pos.z);
    mDoMtx_stack_c::YrotM(home.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x021DFF30, &daMtoge_c::calcMtx);

/* 021E047C */
BOOL daMtoge_c::CreateInit() {
    WWHD_FUNC(0x021E047C, BOOL, this);
    s32 sw = getSwbit();

    if (dBgS_Regist(dComIfG_Bgsp(), mpBgW, this)) /* JUT_ASSERT(0xA8, FALSE) */
        JUT_ASSERT_fail(gabi::at<const char>(0x100158AC), 0xA8, gabi::at<const char>(0x100158A8));

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
VERIFY(0x021E047C, &daMtoge_c::CreateInit);

/* 021E05EC */
cPhs_State daMtoge_c::create() {
    WWHD_FUNC(0x021E05EC, cPhs_State, this);
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

    if (!fopAcM_entrySolidHeap(this, 0x021E0108 /* CheckCreateHeap */, 0x15C0U)) {
        return cPhs_ERROR_e;
    }

    CreateInit();

    return cPhs_COMPLEATE_e;
}
VERIFY(0x021E05EC, &daMtoge_c::create);

/* 021E010C */
BOOL daMtoge_actionWait(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E010C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021E010C, daMtoge_actionWait);

/* 021E0120 */
BOOL daMtoge_actionHind(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E0120, BOOL, i_this);
    s32 swbit = i_this->getSwbit();

    if (dComIfGs_isSwitch(swbit, fopAcM_GetRoomNo(i_this)) != 0) {
        i_this->setAction(ACT_UP);
        fopAcM_SetSpeedF(i_this, 0.0f);

        fopAcM_seStart(i_this, JA_SE_OBJ_TOGE_OUT, 0);
    }

    return TRUE;
}
VERIFY(0x021E0120, daMtoge_actionHind);

/* 021E01B8 */
BOOL daMtoge_actionUp(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E01B8, BOOL, i_this);
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);

    if (cLib_chaseF(&i_this->mHeightOffset, 0.0f, i_this->speedF)) {
        i_this->setAction(ACT_ARRIVAL);
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}
VERIFY(0x021E01B8, daMtoge_actionUp);

/* 021E0230 */
BOOL daMtoge_actionArrival(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E0230, BOOL, i_this);
    s32 swbit = i_this->getSwbit();

    if (dComIfGs_isSwitch(swbit, fopAcM_GetRoomNo(i_this)) == 0) {
        i_this->setAction(ACT_DOWN);
        fopAcM_SetSpeedF(i_this, 0.0f);

        fopAcM_seStart(i_this, JA_SE_OBJ_TOGE_IN, 0);
    }

    return TRUE;
}
VERIFY(0x021E0230, daMtoge_actionArrival);

/* 021E02C8 */
BOOL daMtoge_actionDown(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E02C8, BOOL, i_this);
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);

    if (cLib_chaseF(&i_this->mHeightOffset, -300.0f, i_this->speedF) != 0) {
        i_this->setAction(ACT_HIND);
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}
VERIFY(0x021E02C8, daMtoge_actionDown);

/* daMtoge_c::draw() inlined */
/* 021E0340 */
static BOOL daMtoge_Draw(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E0340, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);

    mDoExt_modelUpdateDL(i_this->mpModel);

    return TRUE;
}
VERIFY(0x021E0340, daMtoge_Draw);

/* daMtoge_c::execute() inlined.
 * static BOOL (*l_action[])(daMtoge_c*) = { Wait, Hind, Up, Arrival, Down };
 * HD: GHS initialises the function-local table on first use (guard 0x101FDB3C, table
 * 0x101FDB40, copied from .data 0x101BB020). */
static be<u32>& l_action_guard() { return *gabi::at<be<u32>>(0x101FDB3C); }
static gfn* l_action() { return gabi::at<gfn>(0x101FDB40); }

/* 021E039C */
static BOOL daMtoge_Execute(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E039C, BOOL, i_this);
    if (l_action_guard() == 0) {
        l_action_guard() = 1;
        memcpy_g(l_action(), gabi::at<void>(0x101BB020), 0x14);
    }
    gabi::call_ptr<BOOL>(l_action()[i_this->mAction].get(), i_this);
    return TRUE;
}
VERIFY(0x021E039C, daMtoge_Execute);

/* 021E041C */
static BOOL daMtoge_IsDelete(daMtoge_c*) {
    WWHD_FUNC(0x021E041C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021E041C, daMtoge_IsDelete);

/* 021E0424 */
static BOOL daMtoge_Delete(daMtoge_c* i_this) {
    WWHD_FUNC(0x021E0424, BOOL, i_this);
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhaseProcReq, M_arcname);

    /* i_this->~daMtoge_c(): nothing to do */
    return TRUE;
}
VERIFY(0x021E0424, daMtoge_Delete);

/* 021E06A0 */
static cPhs_State daMtoge_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021E06A0, cPhs_State, i_this);
    return static_cast<daMtoge_c*>(i_this)->create();
}
VERIFY(0x021E06A0, daMtoge_Create);

/* ---- leftover functions of the translation unit ---- */

/* 021E06A4 __sinit_d_a_mtoge_cpp: the header statics only */
static void __sinit_d_a_mtoge_cpp() {
    WWHD_FUNC(0x021E06A4, void);
    sinit_header_statics(0x104658D0, 0x101BB034);
}
VERIFY(0x021E06A4, __sinit_d_a_mtoge_cpp);

/* 021E0738 sead::SafeStringBase<char>::~SafeStringBase (deleting; vtable slot 10015858) */
static void daMtoge_SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021E0738, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x021E0738, daMtoge_SafeString_dt);

/* 021E074C daMtoge_c::~daMtoge_c (deleting; vtable slot 10015870): only the fopAc_ac_c base */
static void daMtoge_c_dt(daMtoge_c* p, s32 flags) {
    WWHD_FUNC(0x021E074C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x021E074C, daMtoge_c_dt);

/* 021E07A0 sead::SafeStringBase<char>::assureTerminationImpl_ (empty; vtable slot 10015860, after the destructor) */
static void daMtoge_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x021E07A0, void, p);
}
VERIFY(0x021E07A0, daMtoge_SafeString_assureTermination);
