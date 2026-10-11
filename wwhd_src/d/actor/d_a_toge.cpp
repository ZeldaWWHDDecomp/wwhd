/**
 * d_a_toge.cpp (WWHD)
 * Object - Wind Temple - Floor spikes
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_toge.cpp) to the WWHD layout and verified against cking.rpx.
 * Layout: every GameCube member +0x11C (class size 0x5A4).
 */
#include "d/actor/d_a_toge.h"
const gc_tu_data gc_tu = {.safeString = 0x1004079C};
static TogeText m_arcname;
static TogeText sourceName;
static TogeText assertText;
static TogeText jointNameText;
static dCcD_SrcCyl l_cyl_src;
static u32 currentModel;
static Mtx34 currentMatrix;

/* 024C7338 */
BOOL daToge_c::_delete() {
    dComIfG_resDelete(&m_Phs, m_arcname.bytes);

    if (mpBgW1 != nullptr && dBgW_ChkUsed(mpBgW1)) {
        cBgS_Release(dComIfG_Bgsp(), mpBgW1);
    }

    if (mpBgW2 != nullptr && dBgW_ChkUsed(mpBgW2)) {
        cBgS_Release(dComIfG_Bgsp(), mpBgW2);
    }

    return TRUE;
}

/* 024C6E74 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return static_cast<daToge_c *>(i_this)->CreateHeap();
}

/* 024C6D5C */
BOOL daToge_c::CreateHeap() {
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
        m_arcname.bytes, dRes_INDEX_HTOGE1_BDL_HTOGE1_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0x11A, modelData != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x11A, assertText.bytes);

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000U, 0x11000002U);

    if (!mpModel) {
        return FALSE;
    }

    mpModel->mUserArea = gabi::ea(this); /* setUserArea */

    mpBgW1 = dBgW_NewSet((cBgD_t *)dComIfG_getObjectRes(
                             m_arcname.bytes, dRes_INDEX_HTOGE1_DZB_HTOGE1A_e, gc_tu.safeString),
                         toge_bg::MOVE_BG_e, &mtx1);
    mpBgW2 = dBgW_NewSet((cBgD_t *)dComIfG_getObjectRes(
                             m_arcname.bytes, dRes_INDEX_HTOGE1_DZB_HTOGE1B_e, gc_tu.safeString),
                         toge_bg::MOVE_BG_e, &mtx2);

    if (mpBgW1 == nullptr || mpBgW2 == nullptr) {
        return FALSE;
    }
    return TRUE;
}

/* 024C6E78 */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel *model = gabi::at<J3DModel>(currentModel);
        daToge_c *i_this = gabi::at<daToge_c>(model->mUserArea);
        u32 jntNo = (u16) reinterpret_cast<TogeJointData *>(joint)->mJointNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(0.0f, i_this->unk470, 0.0f);
            mtx_copy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get());

            PSMTXCopy(mDoMtx_stack_c::get(), &currentMatrix);
            PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mtx2);
        }
    }

    return TRUE;
}

/* 024C6FB0 */
BOOL daToge_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */

    fopAcM_setCullSizeBox(this, -80.0f, 0.0f, -80.0f, 80.0f, 120.0f, 80.0f);
    mStts.Init(0xFF, 0xFF, this);

    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);

    u8 sw = daToge_prm::getSwitchNo(this);
    mSwitchNo = sw;

    if (fopAcM_isSwitch(this, sw)) {
        unk470 = -150.0f; /* m_y_min */
        mEventState = RETRACT;
    }

    set_mtx();
    PSMTXCopy(&mtx1, &mtx2); /* cMtx_copy */

    JUTNameTab *jointName = mpModel->getModelData()->getJointName();
    /* HD: JUTNameTab::getIndex instead of the GameCube strcmp loop */
    s32 idx = jointName->getIndex(jointNameText.bytes);
    if (idx >= 0) {
        J3DJoint *jnt = mpModel->getModelData()->getJointNodePointer((u16)idx);
        jnt->setCallBack(nodeCallBack);
    }

    J3DModel_calc(mpModel);

    dBgS_Regist(dComIfG_Bgsp(), mpBgW1, this);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW2, this);

    dBgW_Move(mpBgW1);
    dBgW_Move(mpBgW2);

    return TRUE;
}

/* 024C71FC */
cPhs_State daToge_c::_create() {
    /* fopAcM_ct(this, daToge_c); HD: fopAc_ac_c has a vtable, the members' inline constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TOGE_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, TOGE_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&m_Phs, m_arcname.bytes);

    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap), 0x1400)) {
            return cPhs_ERROR_e;
        }

        Create();
    }

    return phase_state;
}

/* 024C77F0 */
BOOL daToge_c::_execute() {
    if (mSwitchNo != 0xFF) {
        if (fopAcM_isSwitch(this, mSwitchNo) != 0) {
            if (mEventState != RETRACT_WAIT) {
                unk485 = 1;
            }
        } else if (!fopAcM_isSwitch(this, mSwitchNo) && mEventState != EXTEND) {
            mEventState = EXTEND_START;
        }
    } else {
        search_wind();

        fopAc_ac_c *pActor = fopAcM_SearchByID(mWindTagId);

        if (pActor != nullptr) {
            if (reinterpret_cast<TogeWindTag *>(pActor)->mOffsY > 0.0f) {
                if (mEventState != EXTEND) {
                    mEventState = EXTEND_START;
                }
            } else if (mEventState != RETRACT_WAIT) {
                unk485 = 1;
            }
        }
    }

    toge_move();

    set_mtx();

    dBgW_Move(mpBgW2);

    set_collision();

    return TRUE;
}

/* 024C7774 */
void daToge_c::set_collision() {
    if (mEventState != RETRACT) {
        // HD: component float stores retain the original load/store order.
        gabi::Local<cXyz> center;
        f32 x = current.pos.x;
        f32 d = unk470 - 10.0f;
        center->x = x;
        center->y = current.pos.y + d;
        center->z = current.pos.z;

        mCyl.SetC(center.get());
        dComIfG_Ccsp_Set(&mCyl);
    }
}

/* 024C7464 */
void daToge_c::search_wind() {
    fopAc_ac_c *pActor = toge_fopAcM_SearchByName(fpcNm_WindTag_e);

    if (pActor != nullptr) {
        mWindTagId = (u32)pActor->mBsPcId;
    } else {
        mWindTagId = fpcM_ERROR_PROCESS_ID_e;
    }
}

/* 024C7520 */
void daToge_c::toge_move() {
    f32 f31 = 30.0f;
    f32 f30 = 15.0f;
    int timer = 0xA;
    bool r30 = true;
    switch (mEventState) {
    case WAIT:
        break;
    case RETRACT_WAIT:
        if (toge_cLib_calcTimer_u8(&unk486) != 0)
            break;
        toge_seStart(JA_SE_OBJ_TOGETOGE_IN);
        mEventState = RETRACT;
        [[fallthrough]];
    case RETRACT:
        toge_cLib_addCalc(&unk470, -150.0f, 0.1f, f31, f30);
        break;
    case EXTEND_START:
        toge_seStart(JA_SE_OBJ_TOGETOGE_OUT);
        mEventState = EXTEND;
        r30 = false;
        [[fallthrough]];
    case EXTEND:
        if (toge_cLib_addCalc(&unk470, unk474, 0.1f, f31, f30) == 0.0f) {
            if (unk470 < 0.0f) {
                unk474 = 0.0f;
            } else if (unk485 != 0) {
                unk486 = timer;
                mEventState = RETRACT_WAIT;
                unk485 = 0;
            } else {
                if (r30) {
                    toge_seStart(JA_SE_OBJ_TOGETOGE_MOVE);
                }
                unk474 = -60.0f;
            }
        }
        break;
    }
}

/* 024C74C0 */
void daToge_c::toge_seStart(u32 i_seNum) {
    if (daToge_prm::getSeEnabled(this)) {
        mDoAud_seStart(i_seNum, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}

/* 024C73C8 */
BOOL daToge_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();

    return TRUE;
}

/* 024C7334 */
static cPhs_State daToge_Create(void *i_this) { return static_cast<daToge_c *>(i_this)->_create(); }

/* 024C73C4 */
static BOOL daToge_Delete(void *i_this) { return static_cast<daToge_c *>(i_this)->_delete(); }

/* 024C7460 */
static BOOL daToge_Draw(void *i_this) { return static_cast<daToge_c *>(i_this)->_draw(); }

/* 024C79B4 */
static BOOL daToge_Execute(void *i_this) { return static_cast<daToge_c *>(i_this)->_execute(); }

/* 024C7A60 */
static BOOL daToge_IsDelete(void *) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 024C79B8 */
static void __sinit_d_a_toge_cpp() { sinit_header_statics(0x1046E97C, 0x101D28FC); }

/* 024C7A4C */
static void SafeString_dt(SafeString *s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 024C7A68 */
static void daToge_c_dt(daToge_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        toge_fopAc_ac_c_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024C7AD4 */
static void SafeString_assureTermination(SafeString *) {}

/* WWHD glue (tools/verify/autoglue.py)
data m_arcname 1004083C nocheck
data sourceName 100407D4 nocheck
data assertText 100407E4 nocheck
data jointNameText 1004080C nocheck
data l_cyl_src 101D28B8 nocheck
data currentModel 104B462C
data currentMatrix 104B4868
*/
