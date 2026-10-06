/**
 * d_a_atdoor.cpp (WWHD)
 * Object - Tower of the Gods jail door (Atdoor)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_atdoor.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "bindings.h"

/* guest string literals (.rodata) */
#define M_arcname gabi::at<const char>(0x100076E8) /* "Atdoor" */
#define SAFESTRING_VTBL 0x10007674 /* this TU's copy of the sead::SafeString vtable */

enum {
    dRes_INDEX_ATDOOR_BDL_SDOOR01_e = 4,
    dRes_INDEX_ATDOOR_DZB_SDOOR01_e = 7,
};

enum Action_e {
    ACT_WAIT_e = 0,
    ACT_CLOSE_WAIT_e = 1,
    ACT_CLOSE_e = 2,
    ACT_OPEN_WAIT_e = 3,
    ACT_OPEN_e = 4,
};

enum { JA_SE_OBJ_TC_JAIL_DOOR_OP = 0x69A4 };

struct daAtdoor_c : fopAc_ac_c {
    void setAction(u8 action) { mAction = action; }

    u8 getSwbit();
    BOOL CreateHeap();
    void calcMtx();
    bool CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ be<u8> mAction;
    /* 0x3BD */ u8 _3BD;
    /* 0x3BE */ be<s16> unk_2A2;
};
WWHD_OFFSET(daAtdoor_c, mpModel, 0x3B4);
WWHD_OFFSET(daAtdoor_c, unk_2A2, 0x3BE);
WWHD_SIZE(daAtdoor_c, 0x3C0);

/* attention_info.position.y (attention_info at 0x388, position at +0x8) */
static be<f32>& attention_pos_y(fopAc_ac_c* a) { return *gabi::at<be<f32>>(gabi::ea(a) + 0x394); }

/* 02058138 */
u8 daAtdoor_c::getSwbit() {
    WWHD_FUNC(0x02058138, u8, this);
    return (fopAcM_GetParam(this) >> 0) & 0xFF;
}
VERIFY(0x02058138, &daAtdoor_c::getSwbit);

/* 0205812C (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0205812C, BOOL, i_this);
    return ((daAtdoor_c*)i_this)->CreateHeap();
}
VERIFY(0x0205812C, CheckCreateHeap);

/* 02058018 */
BOOL daAtdoor_c::CreateHeap() {
    WWHD_FUNC(0x02058018, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ATDOOR_BDL_SDOOR01_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(112, modelData != NULL) */
        JUT_ASSERT_fail(gabi::at<const char>(0x1000769C), 0x70, gabi::at<const char>(0x100076AC));

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mpModel) {
        return FALSE;
    }

    mpBgW = new_dBgW();
    if (!mpBgW) {
        return FALSE;
    }

    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ATDOOR_DZB_SDOOR01_e, SAFESTRING_VTBL);
    if (dzb == nullptr) {
        return FALSE;
    }

    calcMtx();

    J3DModel* model = mpModel;
    dBgW* bgw = mpBgW;
    if (cBgW_Set(bgw, dzb, cBgW_MOVE_BG_e, J3DModel_getBaseTRMtx(model)) == true) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02058018, &daAtdoor_c::CreateHeap);

/* 02057F50 */
void daAtdoor_c::calcMtx() {
    WWHD_FUNC(0x02057F50, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(home.angle.y - unk_2A2);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02057F50, &daAtdoor_c::calcMtx);

/* 0205842C */
bool daAtdoor_c::CreateInit() {
    WWHD_FUNC(0x0205842C, bool, this);
    s32 swBit = getSwbit();
    if (dBgS_Regist(dComIfG_Bgsp(), mpBgW, this)) /* JUT_ASSERT(170, FALSE) */
        JUT_ASSERT_fail(gabi::at<const char>(0x100076C8), 0xAA, gabi::at<const char>(0x100076C4));

    tevStr.mRoomNo = current.roomNo;

    if (swBit == 0xFF) {
        mAction = ACT_WAIT_e;
    } else if (dComIfGs_isSwitch(swBit, fopAcM_GetRoomNo(this))) {
        mAction = ACT_OPEN_WAIT_e;
        unk_2A2 = 0x4000;
    } else {
        mAction = ACT_CLOSE_WAIT_e;
    }

    attention_pos_y(this) = attention_pos_y(this) + 150.0f;
    eyePos.y = eyePos.y + 150.0f;
    calcMtx();
    dBgW_Move(mpBgW);
    return true;
}
VERIFY(0x0205842C, &daAtdoor_c::CreateInit);

/* 02058590 */
cPhs_State daAtdoor_c::create() {
    WWHD_FUNC(0x02058590, cPhs_State, this);
    cPhs_State ret = dComIfG_resLoad(&mPhase, M_arcname);
    /* fopAcM_ct(this, daAtdoor_c): placement new; HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1000768C;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    if (!fopAcM_entrySolidHeap(this, 0x0205812C /* CheckCreateHeap */, 0x1580)) {
        return cPhs_ERROR_e;
    }

    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02058590, &daAtdoor_c::create);

/* 02058130 */
bool daAtdoor_actionWait(daAtdoor_c* i_this) {
    WWHD_FUNC(0x02058130, bool, i_this);
    return true;
}
VERIFY(0x02058130, daAtdoor_actionWait);

/* 02058144 */
bool daAtdoor_actionCloseWait(daAtdoor_c* i_this) {
    WWHD_FUNC(0x02058144, bool, i_this);
    s32 sw = i_this->getSwbit();
    if (dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(i_this))) {
        fopAcM_seStart(i_this, JA_SE_OBJ_TC_JAIL_DOOR_OP, 0);
        i_this->setAction(ACT_OPEN_e);
    }
    return true;
}
VERIFY(0x02058144, daAtdoor_actionCloseWait);

/* 020581D0 */
bool daAtdoor_actionClose(daAtdoor_c* i_this) {
    WWHD_FUNC(0x020581D0, bool, i_this);
    i_this->unk_2A2 = (s16)(i_this->unk_2A2 - 0x400);
    if (i_this->unk_2A2 <= 0) {
        i_this->setAction(ACT_CLOSE_WAIT_e);
        i_this->unk_2A2 = 0;
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);
    return true;
}
VERIFY(0x020581D0, daAtdoor_actionClose);

/* 02058230 */
bool daAtdoor_actionOpenWait(daAtdoor_c* i_this) {
    WWHD_FUNC(0x02058230, bool, i_this);
    s32 sw = i_this->getSwbit();
    if (!dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(i_this))) {
        i_this->setAction(ACT_CLOSE_e);
    }
    return true;
}
VERIFY(0x02058230, daAtdoor_actionOpenWait);

/* 0205828C */
bool daAtdoor_actionOpen(daAtdoor_c* i_this) {
    WWHD_FUNC(0x0205828C, bool, i_this);
    i_this->unk_2A2 = (s16)(i_this->unk_2A2 + 0x400);
    if (i_this->unk_2A2 >= 0x4000) {
        i_this->setAction(ACT_OPEN_WAIT_e);
        i_this->unk_2A2 = 0x4000;
    }

    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);
    return true;
}
VERIFY(0x0205828C, daAtdoor_actionOpen);

/* daAtdoor_c::draw() inlined */
/* 020582F0 */
static BOOL daAtdoor_Draw(daAtdoor_c* i_this) {
    WWHD_FUNC(0x020582F0, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}
VERIFY(0x020582F0, daAtdoor_Draw);

/* daAtdoor_c::execute() inlined.
 * HD: the function-local table l_action is initialised on first use (guard 0x101FDA20, table
 * 0x101FDA24, copied from .data 0x1018FFB0). */
static be<u32>& l_action_guard() { return *gabi::at<be<u32>>(0x101FDA20); }
static gfn* l_action() { return gabi::at<gfn>(0x101FDA24); }

/* 0205834C */
static BOOL daAtdoor_Execute(daAtdoor_c* i_this) {
    WWHD_FUNC(0x0205834C, BOOL, i_this);
    if (l_action_guard() == 0) {
        l_action_guard() = 1;
        memcpy_g(l_action(), gabi::at<void>(0x1018FFB0), 0x14); /* 028FEAC0 memcpy */
    }
    gabi::call_ptr<bool>(l_action()[i_this->mAction].get(), i_this);
    return TRUE;
}
VERIFY(0x0205834C, daAtdoor_Execute);

/* 020583CC */
static BOOL daAtdoor_IsDelete(daAtdoor_c*) {
    WWHD_FUNC(0x020583CC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020583CC, daAtdoor_IsDelete);

/* 020583D4 */
static BOOL daAtdoor_Delete(daAtdoor_c* i_this) {
    WWHD_FUNC(0x020583D4, BOOL, i_this);
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhase, M_arcname);
    /* i_this->~daAtdoor_c(): nothing to do */
    return TRUE;
}
VERIFY(0x020583D4, daAtdoor_Delete);

/* 02058658 (tail call) */
static cPhs_State daAtdoor_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02058658, cPhs_State, i_this);
    return ((daAtdoor_c*)i_this)->create();
}
VERIFY(0x02058658, daAtdoor_Create);

/* ---- compiler-generated (HD) ---- */

/* 0205865C */
static void __sinit_d_a_atdoor_cpp() {
    WWHD_FUNC(0x0205865C, void, (u32)0);
    sinit_header_statics(0x104614D0, 0x1018FFC4);
}
VERIFY(0x0205865C, __sinit_d_a_atdoor_cpp);

/* 020586F0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020586F0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020586F0, trivial_dt);

/* 02058704: daAtdoor_c deleting destructor (HD virtual destructor) */
static void daAtdoor_c_dt(daAtdoor_c* i_this, s32 flags) {
    WWHD_FUNC(0x02058704, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02058704, daAtdoor_c_dt);

/* 02058758: empty virtual */
static void emptyVirtual() { WWHD_FUNC(0x02058758, void); }
VERIFY(0x02058758, emptyVirtual);
