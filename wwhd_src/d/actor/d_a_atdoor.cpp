/**
 * d_a_atdoor.cpp (WWHD)
 * Object - Tower of the Gods jail door (Atdoor)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_atdoor.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_atdoor.h"

const gc_tu_data gc_tu = {.safeString = 0x10007674};
static const char* M_arcname = "Atdoor";

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

// HD: placement construction stores the actor vtable.
daAtdoor_c::daAtdoor_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1000768C;
}

/* 02058138 */
u8 daAtdoor_c::getSwbit() {
    return (fopAcM_GetParam(this) >> 0) & 0xFF;
}

/* 0205812C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daAtdoor_c*)i_this)->CreateHeap();
}

/* 02058018 */
BOOL daAtdoor_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ATDOOR_BDL_SDOOR01_e);
    JUT_ASSERT(112, modelData != NULL);

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mpModel) {
        return FALSE;
    }

    mpBgW = new_dBgW();
    if (!mpBgW) {
        return FALSE;
    }

    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ATDOOR_DZB_SDOOR01_e);
    if (dzb == nullptr) {
        return FALSE;
    }

    calcMtx();

    J3DModel* model = mpModel;
    dBgW* bgw = mpBgW;
    if (bgw->Set(dzb, dBgW::MOVE_BG_e, model->getBaseTRMtx()) == true) {
        return FALSE;
    }
    return TRUE;
}

/* 02057F50 */
void daAtdoor_c::calcMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(home.angle.y - unk_2A2);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0205842C */
bool daAtdoor_c::CreateInit() {
    s32 swBit = getSwbit();
    if (dComIfG_Bgsp()->Regist(mpBgW, this)) {
        JUT_ASSERT(170, FALSE);
    }

    tevStr.mRoomNo = current.roomNo;

    if (swBit == 0xFF) {
        mAction = ACT_WAIT_e;
    } else if (dComIfGs_isSwitch(swBit, fopAcM_GetRoomNo(this))) {
        mAction = ACT_OPEN_WAIT_e;
        unk_2A2 = 0x4000;
    } else {
        mAction = ACT_CLOSE_WAIT_e;
    }

    attention_info.position.y = attention_info.position.y + 150.0f;
    eyePos.y = eyePos.y + 150.0f;
    calcMtx();
    mpBgW->Move();
    return true;
}

/* 02058590 */
cPhs_State daAtdoor_c::create() {
    cPhs_State ret = dComIfG_resLoad(&mPhase, M_arcname);
    fopAcM_ct(this, daAtdoor_c);
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x1580)) {
        return cPhs_ERROR_e;
    }

    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 02058130 */
bool daAtdoor_actionWait(daAtdoor_c* i_this) {
    return true;
}

/* 02058144 */
bool daAtdoor_actionCloseWait(daAtdoor_c* i_this) {
    s32 sw = i_this->getSwbit();
    if (dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(i_this))) {
        fopAcM_seStart(i_this, JA_SE_OBJ_TC_JAIL_DOOR_OP, 0);
        i_this->setAction(ACT_OPEN_e);
    }
    return true;
}

/* 020581D0 */
bool daAtdoor_actionClose(daAtdoor_c* i_this) {
    i_this->unk_2A2 = (s16)(i_this->unk_2A2 - 0x400);
    if (i_this->unk_2A2 <= 0) {
        i_this->setAction(ACT_CLOSE_WAIT_e);
        i_this->unk_2A2 = 0;
    }

    i_this->calcMtx();
    i_this->mpBgW->Move();
    return true;
}

/* 02058230 */
bool daAtdoor_actionOpenWait(daAtdoor_c* i_this) {
    s32 sw = i_this->getSwbit();
    if (!dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(i_this))) {
        i_this->setAction(ACT_CLOSE_e);
    }
    return true;
}

/* 0205828C */
bool daAtdoor_actionOpen(daAtdoor_c* i_this) {
    i_this->unk_2A2 = (s16)(i_this->unk_2A2 + 0x400);
    if (i_this->unk_2A2 >= 0x4000) {
        i_this->setAction(ACT_OPEN_WAIT_e);
        i_this->unk_2A2 = 0x4000;
    }

    i_this->calcMtx();
    i_this->mpBgW->Move();
    return true;
}

/* daAtdoor_c::draw() inlined */
/* 020582F0 */
static BOOL daAtdoor_Draw(daAtdoor_c* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(i_this->mpModel);
    return TRUE;
}

// HD: GHS initializes the function-local action table with a guard and memcpy.
// The source table is checked against the HD image; dispatch reads the copied guest entries.
static u32 l_action_guard;
static u32 l_action[5] = {0, 0, 0, 0, 0};
// HD: entries are actionWait, actionCloseWait, actionClose, actionOpenWait, actionOpen.
static const u32 l_action_init[] = {
    0x02058130,
    0x02058144,
    0x020581D0,
    0x02058230,
    0x0205828C,
};

/* 0205834C */
static BOOL daAtdoor_Execute(daAtdoor_c* i_this) {
    if (l_action_guard == 0) {
        l_action_guard = 1;
        memcpy_g(l_action, l_action_init, sizeof(l_action_init));
    }
    gabi::call_ptr<bool>(l_action[i_this->mAction], i_this);
    return TRUE;
}

/* 020583CC */
static BOOL daAtdoor_IsDelete(daAtdoor_c*) {
    return TRUE;
}

/* 020583D4 */
static BOOL daAtdoor_Delete(daAtdoor_c* i_this) {
    if (i_this->heap != nullptr) {
        dComIfG_Bgsp()->Release(i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhase, M_arcname);
    /* i_this->~daAtdoor_c(): nothing to do */
    return TRUE;
}

/* 02058658 */
static cPhs_State daAtdoor_Create(fopAc_ac_c* i_this) {
    return ((daAtdoor_c*)i_this)->create();
}

/* ---- compiler-generated (HD) ---- */

/* 0205865C */
static void __sinit_d_a_atdoor_cpp() {
    sinit_header_statics(0x104614D0, 0x1018FFC4);
}

// HD: deleting destructor of a class with a trivial destructor
/* 020586F0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: daAtdoor_c deleting destructor (HD virtual destructor)
/* 02058704 */
static void daAtdoor_c_dt(daAtdoor_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: empty virtual
/* 02058758 */
static void emptyVirtual() {}

/* WWHD glue (tools/verify/autoglue.py)
str 100076E8
str 1000769C
str 100076C8 in 0205842C
str 100076AC "modelData != NULL" renamed # HD: assertion spells zero instead of NULL.
str 100076C4 "FALSE" renamed # HD: assertion text spells zero.
data l_action_guard 101FDA20
data l_action 101FDA24 nocheck # HD: BSS table is populated by the checked source copy on first use.
data l_action_init 1018FFB0
*/
