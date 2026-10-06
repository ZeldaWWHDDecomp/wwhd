/**
 * d_a_npc_hr.cpp (WWHD)
 * NPC - Zephos & Cyclos
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_hr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The event (demo*), tornado and state functions are in d_a_npc_hr_demo.cpp.
 */
#include "d/actor/d_a_npc_hr.h"

enum { fpcNm_NPC_HR_e = 0x16D };

bool daNpc_Hr_c::isMorf() { return gabi::load<f32>(gabi::ea(mpHrMorf.get()) + 0xB0) < 1.0f; }

/* 02240E2C */
s16 daNpc_Hr_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x02240E2C, s16, this, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */ ? TRUE : FALSE;
}
VERIFY(0x02240E2C, &daNpc_Hr_c::XyCheckCB);

/* 02240E6C (tail call) */
static s16 daNpc_hr_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x02240E6C, s16, i_this, i_itemBtn);
    return ((daNpc_Hr_c*)i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x02240E6C, daNpc_hr_XyCheckCB);

/* 02240E70 */
static s16 daNpc_hr_XyEventCB(void* i_this, int) {
    WWHD_FUNC(0x02240E70, s16, i_this, 0);
    daNpc_Hr_c* a_this = (daNpc_Hr_c*)i_this;
    s16 idx = dComIfGp_evmng_getEventIdx(STR(0x1001A760) /* "TACT_HT" */, 0xFF);
    a_this->mEventIdx = idx;
    return idx;
}
VERIFY(0x02240E70, daNpc_hr_XyEventCB);

/* 02240EB4 */
static BOOL nodeCallBack_Hr(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02240EB4, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Hr_c* i_this = gabi::at<daNpc_Hr_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                cMtx_YrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][1]);
                s16 angle;
                s8 anm = i_this->mAnmIdx;
                if (anm != 1 && anm != 9 && anm != 10) {
                    angle = i_this->m_jnt.mAngles[0][0]; /* getHead_x() */
                } else {
                    angle = 0;
                }
                cLib_addCalcAngleS(&i_this->mHeadRotation, angle, 8, 0x400, 0x100);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->mHeadRotation);
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                MtxPosition(temp, temp2);
                i_this->mAttnBasePos.x = temp2->x; /* setAttentionBasePos(temp2) */
                i_this->mAttnBasePos.y = temp2->y;
                i_this->mAttnBasePos.z = temp2->z;
                temp->x = 20.0f;
                temp->y = -20.0f;
                temp->z = 0.0f;
                MtxPosition(temp, temp2);
                i_this->mEyePos.x = temp2->x; /* setEyePos(temp2) */
                i_this->mEyePos.y = temp2->y;
                i_this->mEyePos.z = temp2->z;
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]); /* getBackbone_y() */
            } else if (jntNo == (u32)(s32)i_this->m_waist_jnt_num) {
                gabi::Local<cXyz> temp;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                MtxPosition(temp, &i_this->mCloudPos);
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}
VERIFY(0x02240EB4, nodeCallBack_Hr);

/* 022412D8 */
void daNpc_Hr_c::node_Ht_ant(int jntNo) {
    WWHD_FUNC(0x022412D8, void, this, jntNo);
    if (jntNo == 0) {
        gabi::Local<cXyz> temp2;
        gabi::Local<cXyz> temp;
        gabi::Local<cXyz> temp3;
        temp2->y = 0.0f;
        temp2->z = 0.0f;
        temp2->x = 0.0f;
        MtxPosition(temp2, temp);
        u16 fl = mFlags;
        if (!(fl & HR_FLAG_00000800)) {
            mAntennaVelocity.x = 0.0f;
            mAntennaVelocity.y = 0.0f;
            mAntennaVelocity.z = 0.0f;
            mAntennaOffset.x = 0.0f;
            mAntennaOffset.y = 0.0f;
            mAntennaOffset.z = 0.0f;
            mPrevAntennaPos.copy(*temp);
            mFlags = (u16)(fl | HR_FLAG_00000800);
        }
        cXyz_mi(&mPrevAntennaPos, temp3, temp);
        f32 x = temp3->x;
        f32 z = temp3->z;
        /* blt: a NaN scale does not skip the division */
        f32 sx = scale.x;
        if (!(sx < 1.0f)) {
            f32 sz = scale.z;
            if (!(sz < 1.0f)) {
                z = z / sz;
                x = x / sx;
            }
        }
        f32 ax = std::fabs(x);
        f32 az = std::fabs(z);
        f32 temp4 = (ax - az >= 0.0f) ? ax : az; /* fsel */
        if (temp4 > 10.0f) {
            f32 fac = 10.0f / temp4;
            z = z * fac;
            x = x * fac;
        }
        f32 vx = mAntennaVelocity.x * 0.8f;
        f32 vz = mAntennaVelocity.z * 0.8f;
        f32 ox = mAntennaOffset.x;
        f32 oz = mAntennaOffset.z;
        vx = gabi::fnmsubs(0.2f, ox, vx);
        vz = gabi::fnmsubs(0.2f, oz, vz);
        vx = vx + x;
        vz = vz + z;
        mAntennaVelocity.x = vx;
        ox = ox + vx;
        mAntennaVelocity.z = vz;
        oz = oz + vz;
        mAntennaOffset.x = ox;
        mAntennaOffset.z = oz;
        s16 ay = current.angle.y;
        f32 cs = cM_scos(ay);
        f32 sn = cM_ssin(ay);
        f32 rx = gabi::fmsubs(oz, sn, ox * cs);
        mAntennaRotation.x = rx;
        rx = rx * 0.05f;
        f32 rz = gabi::fmadds(oz, cs, ox * sn) * 0.05f;
        mAntennaRotation.x = rx;
        mAntennaRotation.z = rz;
        if (rx > 1.0f) {
            mAntennaRotation.x = 1.0f;
        }
        if (rz > 1.0f) {
            mAntennaRotation.z = 1.0f;
        }
        mPrevAntennaPos.copy(*temp);
    } else {
        cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi(3000.0f * mAntennaRotation.x));
        cMtx_ZrotM(calc_mtx(), (s16)gabi::ftoi(3000.0f * mAntennaRotation.z));
    }
}
VERIFY(0x022412D8, &daNpc_Hr_c::node_Ht_ant);

/* 02241550 */
static BOOL nodeCallBack_Ht_ant(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02241550, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Hr_c* i_this = gabi::at<daNpc_Hr_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            i_this->node_Ht_ant(jntNo);
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), calc_mtx());
        }
    }
    return TRUE;
}
VERIFY(0x02241550, nodeCallBack_Ht_ant);

/* 0224165C */
int daNpc_Hr_c::getShapeType() {
    WWHD_FUNC(0x0224165C, int, this);
    return fopAcM_GetParam(this) & 0xFF;
}
VERIFY(0x0224165C, &daNpc_Hr_c::getShapeType);

/* 02243680 */
int daNpc_Hr_c::getSwbit() {
    WWHD_FUNC(0x02243680, int, this);
    return fopAcM_GetParam(this) >> 8 & 0xFF;
}
VERIFY(0x02243680, &daNpc_Hr_c::getSwbit);

/* 02241664 */
BOOL daNpc_Hr_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x02241664, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData(mpHrMorf->getModel());
    /* l_btp_ix_tbl (.rodata 0x1001A7D8) */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1001A808) /* "Hr" */,
                                                                 gabi::load<s32>(0x1001A7D8 + 4 * mTexPatternIdx), HR_SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(1808, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x1001A828), 0x710, STR(0x1001A80C));
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x02241664, &daNpc_Hr_c::initTexPatternAnm);

/* 02241778 */
BOOL daNpc_Hr_c::CreateHeap() {
    WWHD_FUNC(0x02241778, BOOL, this);
    /* HD: Zephos and Cyclos have their own body models (GameCube: one model, Cyclos' body
     * material table swapped in at draw time) */
    J3DModelData* modelData;
    if (getShapeType() == 1) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001A840) /* "Hr" */, 0x19, HR_SAFESTRING_VTBL);
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001A840), 0x14, HR_SAFESTRING_VTBL);
    }
    if (modelData == nullptr) /* JUT_ASSERT(3435, modelData) */
        JUT_ASSERT_fail(STR(0x1001A874), 0xD6B, STR(0x1001A84C));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001A840), 8 /* BCK_H_WAIT01 */, HR_SAFESTRING_VTBL);
    mpHrMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                      0x80000, 0x11020203);
    if (mpHrMorf.get() == nullptr || mpHrMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001A844) /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(3461, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x1001A874), 0xD85, STR(0x1001A884));
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001A858) /* "backbone1" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0)
        JUT_ASSERT_fail(STR(0x1001A874), 0xD8A, STR(0x1001A8A0));
    s8 waist = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001A838) /* "waist" */);
    m_waist_jnt_num = waist;
    if (waist < 0)
        JUT_ASSERT_fail(STR(0x1001A874), 0xD8E, STR(0x1001A8C0));

    /* brow_bdl_table (0x101BE8E8), ant_bdl_table (0x101BE8F0), ant_bck_table (0x101BE8F8) */
    J3DModelData* browModelData = (J3DModelData*)dComIfG_getObjectRes(
        STR(0x1001A840), gabi::load<s32>(0x101BE8E8 + (getShapeType() & 1) * 4), HR_SAFESTRING_VTBL);
    if (browModelData == nullptr)
        JUT_ASSERT_fail(STR(0x1001A874), 0xDAA, STR(0x1001A864));
    mpBrowModel = mDoExt_J3DModel__create(browModelData, 0x80000, 0x11000022);
    if (mpBrowModel.get() == nullptr) {
        return FALSE;
    }
    J3DModelData* antModelData = (J3DModelData*)dComIfG_getObjectRes(
        STR(0x1001A840), gabi::load<s32>(0x101BE8F0 + (getShapeType() & 1) * 4), HR_SAFESTRING_VTBL);
    if (antModelData == nullptr)
        JUT_ASSERT_fail(STR(0x1001A874), 0xDC3, STR(0x1001A8D8));
    J3DAnmTransform* antBck = (J3DAnmTransform*)dComIfG_getObjectRes(
        STR(0x1001A840), gabi::load<s32>(0x101BE8F8 + (getShapeType() & 1) * 4), HR_SAFESTRING_VTBL);
    mpAntennaMorf = mDoExt_McaMorf::create(nullptr, antModelData, nullptr, nullptr, antBck, -1 /* EMode_NULL */, 1.0f, 0, -1, 1,
                                           nullptr, 0x80000, 0x11000022);
    if (mpAntennaMorf.get() == nullptr || mpAntennaMorf->getModel() == nullptr) {
        return FALSE;
    }
    if (getShapeType() == 1) {
        mTexPatternIdx = 2;
    } else {
        mTexPatternIdx = 0;
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum || i == (u32)(s32)m_waist_jnt_num) {
            J3DModelData_setJointCallBack(J3DModel_getModelData(mpHrMorf->getModel()), i, 0x02240EB4 /* nodeCallBack_Hr */);
        }
    }
    gabi::store<u32>(gabi::ea(mpHrMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    for (u16 i = 0; i < J3DModelData_getJointNum(antModelData); i++) {
        J3DModelData_setJointCallBack(J3DModel_getModelData(mpAntennaMorf->getModel()), i, 0x02241550 /* nodeCallBack_Ht_ant */);
    }
    gabi::store<u32>(gabi::ea(mpAntennaMorf->getModel()) + 0xB8, gabi::ea(this));
    return TRUE;
}
VERIFY(0x02241778, &daNpc_Hr_c::CreateHeap);

/* 02241C58 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02241C58, BOOL, i_this);
    return ((daNpc_Hr_c*)i_this)->CreateHeap();
}
VERIFY(0x02241C58, CheckCreateHeap);

/* 02241C5C */
void daNpc_Hr_c::onHide(int param_1) {
    WWHD_FUNC(0x02241C5C, void, this, param_1);
    u16 fl = mFlags;
    if (!(fl & HR_FLAG_00000010) || param_1 != 0) {
        mFlags = (u16)(fl | HR_FLAG_00000010);
        mSmokeCallBack.remove(); /* mSmokeCallBack.end() */
    }
}
VERIFY(0x02241C5C, &daNpc_Hr_c::onHide);

/* 02241C8C: HD, new: a colour channel (0..255) to linear light, clamped to [0, 1]
 * (pow(c / 255, 2.2)); used where the HD emitters take their colours */
f32 hr_colorGamma(u32 c) {
    WWHD_FUNC(0x02241C8C, f32, c);
    f32 in = (f32)c / 255.0f;
    f32 out = gabi::call<f32>(0x028F4560 /* powf */, in, 2.2f);
    if (out < 0.0f) {
        return 0.0f;
    }
    if (out > 1.0f) {
        return 1.0f;
    }
    return out;
}
VERIFY(0x02241C8C, hr_colorGamma);

/* HD: JPABaseEmitter::setGlobalPrmColor / setGlobalEnvColor store gamma-corrected colours */
static inline u8 hr_gammaByte(u8 c) { return (u8)gabi::ftoi(hr_colorGamma(c) * 255.0f); }

/* 02241D0C */
void daNpc_Hr_c::offHide(int param_1) {
    WWHD_FUNC(0x02241D0C, void, this, param_1);
    u16 fl = mFlags;
    if ((fl & HR_FLAG_00000010) || param_1 != 0) {
        s8 type = mType;
        mFlags = (u16)(fl & ~HR_FLAG_00000010);
        u16 particleID;
        switch (type) {
        case 1:
            particleID = 0x8220; /* dPa_name::ID_IT_SN_KINTOUN_RT00 */
            break;
        default:
            particleID = 0x821F; /* dPa_name::ID_IT_SN_KINTOUN_FT00 */
            break;
        }
        s8 roomNo = fopAcM_GetRoomNo(this);
        dPa_control_c* pa = dComIfGp_getParticle();
        JPABaseEmitter* pEmitter = dPa_control_set(pa, 0, particleID, &mCloudPos, &shape_angle, &scale, 0xFF,
                                                   (dPa_levelEcallBack*)(void*)&mSmokeCallBack, roomNo, nullptr, nullptr, nullptr);
        if (pEmitter != nullptr && mType == 1) {
            u32 e = gabi::ea(pEmitter);
            u8 r = hr_gammaByte(0x97);
            u8 g = hr_gammaByte(0x76);
            u8 b = hr_gammaByte(0xA9);
            gabi::store<u8>(e + 0x244, r); /* setGlobalPrmColor(0x97, 0x76, 0xA9) */
            gabi::store<u8>(e + 0x245, g);
            gabi::store<u8>(e + 0x246, b);
            r = hr_gammaByte(0x84);
            g = hr_gammaByte(0x70);
            b = hr_gammaByte(0x93);
            gabi::store<u8>(e + 0x248, r); /* setGlobalEnvColor(0x84, 0x70, 0x93) */
            gabi::store<u8>(e + 0x249, g);
            gabi::store<u8>(e + 0x24A, b);
        }
    }
}
VERIFY(0x02241D0C, &daNpc_Hr_c::offHide);

/* 02241EBC */
void daNpc_Hr_c::defaultSetPos(cXyz* param_1) {
    WWHD_FUNC(0x02241EBC, void, this, param_1);
    f32 x = param_1->x;
    current.pos.x = x;
    f32 y = param_1->y;
    current.pos.y = y;
    f32 z = param_1->z;
    current.pos.z = z;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 ay = y + 150.0f;
    attPos->x = x;
    attPos->y = ay;
    attPos->z = z;
    mAttnBasePos.x = x;
    mAttnBasePos.y = ay;
    mAttnBasePos.z = z;
    f32 ey = y + 100.0f;
    eyePos.x = x;
    eyePos.y = ey;
    eyePos.z = z;
    mEyePos.x = x;
    mEyePos.y = ey;
    mEyePos.z = z;
}
VERIFY(0x02241EBC, &daNpc_Hr_c::defaultSetPos);

/* 02241F20 */
void daNpc_Wind_Eff::init() {
    WWHD_FUNC(0x02241F20, void, this);
    mWindEffState = WIND_EFF_INACTIVE;
    mAlphaFactor = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
    mpSquallPos = nullptr;
}
VERIFY(0x02241F20, &daNpc_Wind_Eff::init);

/* 02241F48 */
void daNpc_Wind_Clothes::init() {
    WWHD_FUNC(0x02241F48, void, this);
    mSquallState = 0;
    for (int i = 0; i < 4; i++) {
        mWindEff[i].init();
    }
}
VERIFY(0x02241F48, &daNpc_Wind_Clothes::init);

/* 02241F80 */
BOOL daNpc_Hr_c::init() {
    WWHD_FUNC(0x02241F80, BOOL, this);
    /* setAction(&daNpc_Hr_c::wait_action, NULL) (inline, always TRUE) */
    {
        ProcFunc_hr* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == 0x02246D58;
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = -1; /* ACTION_ENDING */
                pmf_call_hr(this, cur, nullptr);
            }
            cur->f = 0x02246D58; /* wait_action */
            mActionStatus = 0;   /* ACTION_STARTING */
            cur->i = -1;
            cur->d = 0;
            wait_action(nullptr);
        }
    }
    mCloudPos.copy(current.pos);

    switch (mType) {
    case 1:
        /* HD (as GameCube retail) */
        if (dComIfGs_isEventBit(0x2710 /* UNK_2710 */)) {
            onHide(1);
        } else {
            offHide(1);
        }
        dComIfGs_offTmpBit(0x0404 /* UNK_0404 */);
        break;
    default:
        onHide(1);
        gabi::store<u8>(gabi::ea(this) + 0x38B, 0x5B);        /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
        gabi::store<u32>(gabi::ea(this) + 0x39C, 0x20000008); /* attention_info.flags = ACTION_SPEAK | TALKFLAG_CHECK */
        break;
    }

    J3DModel* model = mpHrMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    mpHrMorf->calc();

    mStts.Init(0xFE, 0xFF, this);
    mStts2.Init(0xFE, 0xFF, this);

    switch (mType) {
    case 1:
        mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BE8A4) /* dNpc_hr_src */);
        mCyl2.Set(gabi::at<dCcD_SrcCyl>(0x101BE8A4));
        break;
    default:
        mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
        eventInfo_setEventName(this, STR(0x1001A900) /* "HT_TALK" */);
        break;
    }
    mCyl.SetStts(&mStts);
    mCyl2.SetStts(&mStts2);
    defaultSetPos(&current.pos);
    mClothes.init();
    return TRUE;
}
VERIFY(0x02241F80, &daNpc_Hr_c::init);

/* 022422B0 */
cPhs_State daNpc_Hr_c::_create() {
    WWHD_FUNC(0x022422B0, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Hr_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025D4ED0, this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = HR_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, HR_AAB_VTBL);
            dCcD_Stts_ct(&mStts2);
            dCcD_Cyl_ct(&mCyl2, HR_AAB_VTBL);
            gabi::call(0x0259DAA0, &m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            dPa_followEcallBack_ct(&mSmokeCallBack, 0, 0);
            gabi::call(0x028EFFD0, &mClothes, 4, 0x38, 0x02247154); /* __construct_array(mWindEff, daNpc_Wind_Eff ctor) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001A908) /* "Hr" */);
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_HR_e:
            mType = getShapeType() == 1 ? 1 : 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, 0x02241C58 /* CheckCreateHeap */, 0xB7B0)) {
            mpHrMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpHrMorf->getModel())); /* fopAcM_SetMtx */
        if (!init()) {
            mpHrMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022422B0, &daNpc_Hr_c::_create);

/* 022424BC (tail call) */
static cPhs_State daNpc_Hr_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022424BC, cPhs_State, i_this);
    return ((daNpc_Hr_c*)i_this)->_create();
}
VERIFY(0x022424BC, daNpc_Hr_Create);

/* 022424C0 */
void daNpc_Wind_Eff::remove() {
    WWHD_FUNC(0x022424C0, void, this);
    if (mWindEffState != WIND_EFF_INACTIVE) {
        mpFollowECallBack.remove(); /* mpFollowECallBack.end() */
        init();
    }
}
VERIFY(0x022424C0, &daNpc_Wind_Eff::remove);

/* 02242510 */
void daNpc_Wind_Clothes::remove() {
    WWHD_FUNC(0x02242510, void, this);
    for (int i = 0; i < 4; i++) {
        mWindEff[i].remove();
    }
}
VERIFY(0x02242510, &daNpc_Wind_Clothes::remove);

/* 02242558 */
BOOL daNpc_Hr_c::_delete() {
    WWHD_FUNC(0x02242558, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001A90B) /* "Hr" */);
    if (mpHrMorf.get() != nullptr) {
        mpHrMorf->stopZelAnime();
    }
    mSmokeCallBack.remove(); /* mSmokeCallBack.end() */
    if (mType == 1) {
        dComIfGs_offTmpBit(0x0404 /* UNK_0404 */);
    }
    mClothes.remove();
    return TRUE;
}
VERIFY(0x02242558, &daNpc_Hr_c::_delete);

/* 022425E4 (tail call) */
static BOOL daNpc_Hr_Delete(daNpc_Hr_c* i_this) {
    WWHD_FUNC(0x022425E4, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022425E4, daNpc_Hr_Delete);

/* 022425E8 */
void daNpc_Hr_c::playTexPatternAnm() {
    WWHD_FUNC(0x022425E8, void, this);
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= frameMax0) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = mBlinkFrame + 1;
        }
    }
}
VERIFY(0x022425E8, &daNpc_Hr_c::playTexPatternAnm);

/* 022426AC */
void daNpc_Hr_c::setAnm(s32 anmIdx) {
    WWHD_FUNC(0x022426AC, void, this, anmIdx);
    s8 cur = mAnmIdx;
    f32 morf = 8.0f;
    if (cur == 3 || anmIdx == 3) {
        morf = 18.0f;
    }
    if (cur == -1) {
        morf = 0.0f;
    }
    if ((u32)anmIdx != (u32)(s32)cur && anmIdx != -1) {
        mAnmTimer = 0.0f;
        mAnmIdx = (s8)anmIdx;
        /* l_bck_ix_tbl (.rodata 0x1001A7A4) */
        dNpc_setAnm(mpHrMorf, -1 /* EMode_NULL */, morf, 1.0f, gabi::load<s32>(0x1001A7A4 + 4 * (s8)anmIdx), -1,
                    STR(0x1001A91C) /* "Hr" */);
        mAnmLoopCount = 0;
    }
}
VERIFY(0x022426AC, &daNpc_Hr_c::setAnm);

/* 02242760 */
void daNpc_Hr_c::nextAnm(s32 i_nextAnm, int loopCount) {
    WWHD_FUNC(0x02242760, void, this, i_nextAnm, loopCount);
    if ((s32)mAnmLoopCount < loopCount) {
        mAnmLoopCount = mAnmLoopCount + 1;
    } else {
        setAnm(i_nextAnm);
    }
}
VERIFY(0x02242760, &daNpc_Hr_c::nextAnm);

/* 0224277C */
void daNpc_Hr_c::talkInit() {
    WWHD_FUNC(0x0224277C, void, this);
    mTalkState = 0; /* TALK_INIT */
    mMsgAnmIdx = 0xFF;
}
VERIFY(0x0224277C, &daNpc_Hr_c::talkInit);

/* 02242790 */
void daNpc_Hr_c::checkOrder() {
    WWHD_FUNC(0x02242790, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd != 2 /* !checkCommandDemoAccrpt() */) {
        if (cmd == 1 /* checkCommandTalk() */ && ChkOrder(7)) {
            if (dComIfGp_event_chkTalkXY()) {
                setFlag(HR_FLAG_00000008);
            } else {
                setFlag(HR_FLAG_00000001);
            }
            talkInit();
        }
    }
    ClrOrder();
}
VERIFY(0x02242790, &daNpc_Hr_c::checkOrder);

/* 02242820 */
void daNpc_Hr_c::eventOrder() {
    WWHD_FUNC(0x02242820, void, this);
    if (ChkOrder(1)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
    }
    if (ChkOrder(2)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(4)) {
        eventInfo_onCondition(this, 0x20 /* dEvtCnd_CANTALKITEM_e */);
    }
}
VERIFY(0x02242820, &daNpc_Hr_c::eventOrder);

/* 02242898 */
void daNpc_Hr_c::setCollision() {
    WWHD_FUNC(0x02242898, void, this);
    gabi::Local<cXyz> temp;
    temp->copy(current.pos);
    f32 temp1 = 45.0f * scale.x;
    f32 temp2 = 100.0f * scale.y;
    mCyl.SetC(temp);
    mCyl.SetR(temp1);
    mCyl.SetH(temp2);
    dComIfG_Ccsp_Set(&mCyl);
    if (mType == 1) {
        f32 sy = scale.y;
        temp2 = 35.0f * sy;
        temp->y = gabi::fmadds(-30.0f, sy, temp->y);
        temp1 = 80.0f * scale.x;
        mCyl2.SetC(temp);
        mCyl2.SetR(temp1);
        mCyl2.SetH(temp2);
        dComIfG_Ccsp_Set(&mCyl2);
    }
}
VERIFY(0x02242898, &daNpc_Hr_c::setCollision);

/* 022429E0 */
void daNpc_Wind_Clothes::setSquallPos(int param_1) {
    WWHD_FUNC(0x022429E0, void, this, param_1);
    fopAc_ac_c* a_actor = fopAcM_SearchByID(mProcId);
    if (a_actor == nullptr) /* JUT_ASSERT(411, a_actor) */
        JUT_ASSERT_fail(STR(0x1001A940), 0x19B, STR(0x1001A938));
    mSquallPos[param_1].copy(a_actor->current.pos);
    s16 cnt = mSquallCounter[param_1];
    f32 temp2 = gabi::fmadds(cM_ssin(cnt), 0.2f, 1.0f) * squallOrbitParam[2];
    gabi::Local<cXyz> temp;
    temp->x = gabi::fmadds(temp2, cM_scos(cnt), squallOrbitParam[0]);
    temp->y = gabi::fmadds(temp2, cM_ssin(cnt), squallOrbitParam[1]);
    temp->z = gabi::fmadds((f32)param_1, squallOrbitParam[3], squallOrbitParam[4]);
    fpoAcM_absolutePos(a_actor, temp, &mSquallPos[param_1]);
}
VERIFY(0x022429E0, &daNpc_Wind_Clothes::setSquallPos);

/* 02242B2C */
void daNpc_Wind_Eff::setspd() {
    WWHD_FUNC(0x02242B2C, void, this);
    gabi::Local<cXyz> speed;
    cXyz_mi(mpSquallPos, speed, &mPos);
    mSpeed.copy(*speed);
}
VERIFY(0x02242B2C, &daNpc_Wind_Eff::setspd);

/* 02242B7C */
void daNpc_Wind_Eff::move() {
    WWHD_FUNC(0x02242B7C, void, this);
    PSVECAdd(&mPos, &mSpeed, &mPos); /* mPos += mSpeed */
}
VERIFY(0x02242B7C, &daNpc_Wind_Eff::move);

/* 02242B8C */
void daNpc_Wind_Eff::proc() {
    WWHD_FUNC(0x02242B8C, void, this);
    switch ((u8)mWindEffState) {
    case WIND_EFF_FADE_IN: {
        f32 alpha = mAlphaFactor;
        JPABaseEmitter* pEmitter = mpFollowECallBack.getEmitter();
        if (alpha < 0.9f) {
            alpha = alpha + 0.1f;
            mAlphaFactor = alpha;
        } else {
            mWindEffState = WIND_EFF_ACTIVE;
            alpha = 1.0f;
            mAlphaFactor = alpha;
        }
        if (pEmitter != nullptr) {
            gabi::store<u8>(gabi::ea(pEmitter) + 0x247, (u8)gabi::ftoi(alpha * 128.0f)); /* setGlobalAlpha */
        }
        setspd();
        move();
        break;
    }
    case WIND_EFF_ACTIVE:
        setspd();
        move();
        break;
    case WIND_EFF_FADE_OUT: {
        f32 alpha = mAlphaFactor;
        JPABaseEmitter* pEmitter = mpFollowECallBack.getEmitter();
        if (alpha > 0.1f) {
            mAlphaFactor = alpha - 0.1f;
            move();
            if (pEmitter != nullptr) {
                gabi::store<u8>(gabi::ea(pEmitter) + 0x247, (u8)gabi::ftoi(mAlphaFactor * 128.0f));
            }
        } else {
            remove();
        }
        break;
    }
    }
}
VERIFY(0x02242B8C, &daNpc_Wind_Eff::proc);

/* 02242D24 */
void daNpc_Wind_Clothes::proc() {
    WWHD_FUNC(0x02242D24, void, this);
    switch (mSquallState) {
    case 1: {
        fopAc_ac_c* a_actor = fopAcM_SearchByID(mProcId);
        if (a_actor == nullptr) /* JUT_ASSERT(478, a_actor) */
            JUT_ASSERT_fail(STR(0x1001A964), 0x1DE, STR(0x1001A95C));
        for (int i = 0; i < 4; i++) {
            mSquallCounter[i] = (s16)(mSquallCounter[i] + 0x100);
            setSquallPos(i);
        }
        break;
    }
    }
    for (int i = 0; i < 4; i++) {
        mWindEff[i].proc();
    }
}
VERIFY(0x02242D24, &daNpc_Wind_Clothes::proc);

/* 02242DF8 */
BOOL daNpc_Hr_c::_execute() {
    WWHD_FUNC(0x02242DF8, BOOL, this);
    m_jnt.setParam(0, 0x1C70, 0, -0x1C70, 0x1FFE, 9000, -0x1FFE, -9000, 0x1000);
    playTexPatternAnm();
    mAnmEnded = (s8)mpHrMorf->play(&eyePos, 0, 0);
    if (mpHrMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpHrMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((u32)(s32)mAnmIdx) {
        case 3:
            nextAnm(0, 2);
            break;
        case 4:
            nextAnm(0, 3);
            break;
        case 5:
            nextAnm(1, 3);
            break;
        case 8:
            nextAnm(1, 3);
            break;
        case 9:
            nextAnm(1, 3);
            break;
        case 10:
            nextAnm(2, 2);
            break;
        case 2:
            nextAnm(10, gabi::ftoi(cM_rndF(4.0f) + 4.0f));
            break;
        }
    }
    checkOrder();
    pmf_call_hr(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    shape_angle.y = current.angle.y;
    if (!chkFlag(HR_FLAG_00000020)) {
        fopAcM_posMove(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
    }
    clrFlag(HR_FLAG_00000020);
    tevStr.mRoomNo = current.roomNo;
    J3DModel* hrModel = mpHrMorf->getModel();
    J3DModel_setBaseScale(hrModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(hrModel, mDoMtx_stack_c::get());
    mpHrMorf->calc();
    if (chkFlag(HR_FLAG_00000010)) {
        mCyl.OffCoSPrmBit(1); /* OffCoSetBit() */
    } else {
        if (mType != 1) {
            mCyl.OnCoSPrmBit(1); /* OnCoSetBit() */
        }
        setCollision();
    }
    mClothes.proc();
    return TRUE;
}
VERIFY(0x02242DF8, &daNpc_Hr_c::_execute);

/* 02243164 (tail call) */
static BOOL daNpc_Hr_Execute(daNpc_Hr_c* i_this) {
    WWHD_FUNC(0x02243164, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02243164, daNpc_Hr_Execute);

/* 02243168 */
BOOL daNpc_Hr_c::_draw() {
    WWHD_FUNC(0x02243168, BOOL, this);
    J3DModel* hrModel = mpHrMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData(hrModel);
    J3DModel* antennaModel = mpAntennaMorf->getModel();
    if (chkFlag(HR_FLAG_00000010)) {
        return FALSE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), hrModel, &tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpBrowModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), antennaModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, modelData, mBlinkFrame);
    /* HD: no Cyclos material table (the HD models are separate), mpHrMorf->entryDL() for both */
    mpHrMorf->entryDL();
    J3DModel_setBaseTRMtx(mpBrowModel, getAnmMtx(hrModel, m_jnt.mHeadJntNum));
    mDoExt_modelUpdateDL(mpBrowModel);
    J3DModel_setBaseTRMtx(antennaModel, getAnmMtx(hrModel, m_jnt.mHeadJntNum));
    mpAntennaMorf->updateDL();
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* mBtpAnm.remove(modelData) */
    if (getShapeType() == 1 && mState == HR_STATE_RT_ANGRY) {
        dSnap_RegistFig(0x9A /* DSNAP_TYPE_NPC_HR */, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}
VERIFY(0x02243168, &daNpc_Hr_c::_draw);

/* 022433B0 (tail call) */
static BOOL daNpc_Hr_Draw(daNpc_Hr_c* i_this) {
    WWHD_FUNC(0x022433B0, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022433B0, daNpc_Hr_Draw);

/* 022433B4 */
static BOOL daNpc_Hr_IsDelete(daNpc_Hr_c*) {
    WWHD_FUNC(0x022433B4, BOOL, (daNpc_Hr_c*)nullptr);
    return TRUE;
}
VERIFY(0x022433B4, daNpc_Hr_IsDelete);

/* 022433BC */
BOOL daNpc_Wind_Eff::create(cXyz* squallPos) {
    WWHD_FUNC(0x022433BC, BOOL, this, squallPos);
    if (mWindEffState == WIND_EFF_INACTIVE) {
        mPos.copy(*squallPos);
        mAlphaFactor = 1.0f;
        mpSquallPos = squallPos;
        JPABaseEmitter* pEmitter = dComIfGp_particle_set(0x31 /* dPa_name::ID_AK_JN_WINDLINE00 */, &mPos, nullptr, nullptr, 0xFF,
                                                         (dPa_levelEcallBack*)(void*)&mpFollowECallBack);
        if (pEmitter == nullptr) {
            return FALSE;
        }
        mWindEffState = WIND_EFF_FADE_IN;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022433BC, &daNpc_Wind_Eff::create);

/* 02243490 */
BOOL daNpc_Wind_Eff::end() {
    WWHD_FUNC(0x02243490, BOOL, this);
    if (mWindEffState == WIND_EFF_FADE_IN || mWindEffState == WIND_EFF_ACTIVE) {
        mWindEffState = WIND_EFF_FADE_OUT;
        mpSquallPos = nullptr;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02243490, &daNpc_Wind_Eff::end);

/* 022434C4 */
BOOL daNpc_Wind_Clothes::create(fopAc_ac_c* i_this, u32 param_2, be<f32>* param_3, int param_4) {
    WWHD_FUNC(0x022434C4, BOOL, this, i_this, param_2, param_3, param_4);
    mSquallState = (u8)param_2;
    mProcId = fopAcM_GetID(i_this);
    if (param_3 != nullptr) {
        for (int i = 0; i < param_4; i++) {
            squallOrbitParam[i] = (f32)param_3[i];
        }
    }
    for (int i = 0; i < 4; i++) {
        mSquallCounter[i] = (s16)(i * 0x4000);
        setSquallPos(i);
        mWindEff[i].create(&mSquallPos[i]);
    }
    return TRUE;
}
VERIFY(0x022434C4, &daNpc_Wind_Clothes::create);

/* 02243580 */
BOOL daNpc_Wind_Clothes::end() {
    WWHD_FUNC(0x02243580, BOOL, this);
    mSquallState = 0;
    for (int i = 0; i < 4; i++) {
        mWindEff[i].end();
    }
    return TRUE;
}
VERIFY(0x02243580, &daNpc_Wind_Clothes::end);

/* 022435C4 */
u32 daNpc_Hr_c::setTexPtn(s32 param_1) {
    WWHD_FUNC(0x022435C4, u32, this, param_1);
    if ((u32)param_1 != (u32)(s32)mTexPatternIdx) {
        mTexPatternIdx = (s8)param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022435C4, &daNpc_Hr_c::setTexPtn);

/* 022435E4 */
void daNpc_Hr_c::setAnmStatus() {
    WWHD_FUNC(0x022435E4, void, this);
    switch (mState) {
    case HR_STATE_RT_ANGRY:
        setAnm(2);
        break;
    default:
        if (getShapeType() == 1) {
            setAnm(1);
        } else {
            setAnm(0);
        }
        break;
    }
}
VERIFY(0x022435E4, &daNpc_Hr_c::setAnmStatus);

/* 02243644 */
u32 daNpc_Hr_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02243644, u32, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x1906:
    case 0x1933:
    case 0x1934:
    case 0x1938:
        *pMsgNo = *pMsgNo + 1;
        break;
    default:
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}
VERIFY(0x02243644, &daNpc_Hr_c::next_msgStatus);

/* 0224368C */
u32 daNpc_Hr_c::getMsg() {
    WWHD_FUNC(0x0224368C, u32, this);
    u32 msgNo = 0;
    switch ((u32)(s32)mState) {
    case 7:
    case 10:
    case 3:
        msgNo = mMsgNo;
        switch (msgNo) {
        case 0x5B3:
            if (mType == 0) {
                s32 sw = getSwbit();
                dComIfGs_onSwitch(sw, fopAcM_GetRoomNo(this));
                dComIfGp_setMelodyNum(0);
            } else {
                dComIfGp_setMelodyNum(1);
            }
            break;
        }
        break;
    case 2:
        msgNo = 0x1901;
        break;
    }
    return msgNo;
}
VERIFY(0x0224368C, &daNpc_Hr_c::getMsg);

/* 0224374C */
void daNpc_Hr_c::msgAnm(u32 param_1) {
    WWHD_FUNC(0x0224374C, void, this, param_1);
    /* static s8 msg_anm_table[] = {0, 1, 3, 4, 7, 5, 9, 8, 6} (.data 0x101BE900) */
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = (u8)param_1;
        if (param_1 < 9) {
            setAnm(gabi::load<s8>(0x101BE900 + param_1));
        }
    }
}
VERIFY(0x0224374C, &daNpc_Hr_c::msgAnm);

/* 02243774 */
u16 daNpc_Hr_c::talk() {
    WWHD_FUNC(0x02243774, u16, this);
    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    if (mTalkState == 0 /* TALK_INIT */) {
        l_msgId() = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = 1; /* TALK_MSG_CREATE */
    } else if (mTalkState != -1 /* TALK_FINISHED */) {
        if (l_msgId() == 0xFFFFFFFF) {
            gabi::Local<cXyz> temp;
            temp->x = current.pos.x;
            temp->y = current.pos.y + 100.0f;
            temp->z = current.pos.z;
            l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, temp);
        } else {
            msgAnm(dComIfGp_getMesgAnimeAttrInfo());
            switch ((u32)(s32)mTalkState) {
            case 1:
                mTalkState = 2; /* HD: no fopMsgM_SearchByID */
                break;
            case 2:
                msgStatus = (u16)msgMgr_getStatus(mgr);
                if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                    msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (msgMgr_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                        msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                    msgMgr_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                    mTalkState = -1;
                }
                break;
            }
        }
    }
    return msgStatus;
}
VERIFY(0x02243774, &daNpc_Hr_c::talk);

/* 022438EC */
void daNpc_Hr_c::setAttention(int param_1) {
    WWHD_FUNC(0x022438EC, void, this, param_1);
    if (chkFlag(HR_FLAG_00000010) || chkFlag(HR_FLAG_00000100)) {
        return;
    }
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 50.0f;
    attPos->z = mAttnBasePos.z;
    eyePos.z = mEyePos.z;
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
}
VERIFY(0x022438EC, &daNpc_Hr_c::setAttention);

/* 02243954 */
u8 daNpc_Hr_c::getLookBackMode() {
    WWHD_FUNC(0x02243954, u8, this);
    if (chkFlag(HR_FLAG_00000080)) {
        return 0;
    }
    if (chkFlag(HR_FLAG_00000040)) {
        return 1;
    }
    return 0xFF;
}
VERIFY(0x02243954, &daNpc_Hr_c::getLookBackMode);

/* 0224397C */
void daNpc_Hr_c::lookBack() {
    WWHD_FUNC(0x0224397C, void, this);
    gabi::Local<cXyz> eye;   /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;  /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    /* HD: current.angle.y is read once, before getLookBackMode() (GameCube re-reads it in the default case) */
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (getLookBackMode()) {
    case 0:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tz = current.pos.z;
        tx = current.pos.x;
        ty = eyePos.y;
        break;
    case 1:
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        ty = eyePos.y;
        tx = current.pos.x;
        tz = current.pos.z;
        headOnlyFollow = true;
        break;
    default:
        headOnlyFollow = true;
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x800, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    s16 vel = mMaxHeadTurnVelocity;
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, vel, headOnlyFollow);
}
VERIFY(0x0224397C, &daNpc_Hr_c::lookBack);

/* 02243B30 */
void daNpc_Hr_c::to_rt_hit() {
    WWHD_FUNC(0x02243B30, void, this);
    mDoAud_seStart_noPos(0x48EA /* JA_SE_CV_RC_DAMAGE */);
    mDoAud_seStart_noPos(0x48EB /* JA_SE_CV_RC_MODAE_1 */);
    setAnm(11);
    setTexPtn(1);
    setFlag(HR_FLAG_00000400);
    mHitDelayTimer = 0x52;
}
VERIFY(0x02243B30, &daNpc_Hr_c::to_rt_hit);

/* 02243B94 */
void daNpc_Hr_c::to_rt_tact() {
    WWHD_FUNC(0x02243B94, void, this);
    mDoAud_seStart_noPos(0x48EA /* JA_SE_CV_RC_DAMAGE */);
    mDoAud_seStart_noPos(0x48EC /* JA_SE_CV_RC_MODAE_2 */);
    mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001A97C) /* "TACT0_RT" */, 0xFF);
    mReturnState = mState;
    mState = HR_STATE_RT_HIT_0;
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    mHitDelayTimer = 0x52;
    mDoAud_subBgmStop();
}
VERIFY(0x02243B94, &daNpc_Hr_c::to_rt_tact);

/* 02243C24 */
void daNpc_Hr_c::setEmitFlash(f32 param_1) {
    WWHD_FUNC(0x02243C24, void, this, param_1);
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    /* HD: the lower clamp is an fsel (a NaN becomes 0) */
    if (param_1 > 1.0f) {
        param_1 = 1.0f;
    } else {
        param_1 = (param_1 >= 0.0f) ? param_1 : 0.0f;
    }
    if (pEmitter != nullptr) {
        u32 e = gabi::ea(pEmitter);
        u8 r = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(104.0f, param_1, 151.0f)));
        u8 g = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(137.0f, param_1, 118.0f)));
        u8 b = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(86.0f, param_1, 169.0f)));
        gabi::store<u8>(e + 0x244, r); /* setGlobalPrmColor */
        gabi::store<u8>(e + 0x245, g);
        gabi::store<u8>(e + 0x246, b);
        r = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(123.0f, param_1, 132.0f)));
        g = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(143.0f, param_1, 112.0f)));
        b = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(33.0f, param_1, 147.0f)));
        gabi::store<u8>(e + 0x248, r); /* setGlobalEnvColor */
        gabi::store<u8>(e + 0x249, g);
        gabi::store<u8>(e + 0x24A, b);
    }
}
VERIFY(0x02243C24, &daNpc_Hr_c::setEmitFlash);

/* 02243E34 */
bool daNpc_Hr_c::wait01() {
    WWHD_FUNC(0x02243E34, bool, this);
    if (chkFlag(HR_FLAG_00000001)) {
        mReturnState = mState;
        mState = HR_STATE_TALK;
        setAnmStatus();
    } else {
        SetOrder(1);
    }
    return isMorf();
}
VERIFY(0x02243E34, &daNpc_Hr_c::wait01);

/* 02243EC8 */
void daNpc_Hr_c::endTalk() {
    WWHD_FUNC(0x02243EC8, void, this);
    mState = mReturnState;
    setAnmStatus();
    dComIfGp_event_reset();
    clrFlag(HR_FLAG_00000008 | HR_FLAG_00000001);
    field_0x60A = 5;
}
VERIFY(0x02243EC8, &daNpc_Hr_c::endTalk);

/* 02243F20 */
bool daNpc_Hr_c::talk01() {
    WWHD_FUNC(0x02243F20, bool, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        endTalk();
        daPy_offPlayerNoDraw(dComIfGp_getLinkPlayer());
    }
    return isMorf();
}
VERIFY(0x02243F20, &daNpc_Hr_c::talk01);

/* 02243F8C */
void daNpc_Hr_c::endTact() {
    WWHD_FUNC(0x02243F8C, void, this);
    onHide(0);
    defaultSetPos(&home.pos);
    s16 angle = home.angle.y;
    shape_angle.y = angle;
    speed.x = 0.0f;
    speed.y = 0.0f;
    speed.z = 0.0f;
    current.angle.y = angle;
    speedF = 0.0f;
    if (mType == 0) {
        s32 sw = getSwbit();
        dComIfGs_offSwitch(sw, fopAcM_GetRoomNo(this));
    }
}
VERIFY(0x02243F8C, &daNpc_Hr_c::endTact);

/* 0224401C */
s32 daNpc_Hr_c::getNowEventAction() {
    WWHD_FUNC(0x0224401C, s32, this);
    /* static char* action_table[19] (.data 0x101BE90C): WAIT, SPEAK, PATTEN, TACT0..3, WIND0, WIND1,
     * HIDE_LINK, DISP_LINK, DISP_MY, MOVE, TURN_LINK, LOOK, SMALL, INTRO, CHANGE, DEBUG */
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, 0x101BE90C, 19, FALSE, 1);
}
VERIFY(0x0224401C, &daNpc_Hr_c::getNowEventAction);
