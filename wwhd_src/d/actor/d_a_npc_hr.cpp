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

static s32 l_bck_ix_tbl[13] = {
    dRes_INDEX_HR_BCK_H_WAIT01_e, dRes_INDEX_HR_BCK_R_WAIT01_e, dRes_INDEX_HR_BCK_R_WAIT02_e,
    dRes_INDEX_HR_BCK_LOOK_e,     dRes_INDEX_HR_BCK_TALK01_e,   dRes_INDEX_HR_BCK_TALK01_e,
    dRes_INDEX_HR_BCK_TALK01_e,   dRes_INDEX_HR_BCK_TALK02_e,   dRes_INDEX_HR_BCK_TALK02_e,
    dRes_INDEX_HR_BCK_TALK03_e,   dRes_INDEX_HR_BCK_TALK03_e,   dRes_INDEX_HR_BCK_DAMAGE_e,
    dRes_INDEX_HR_BCK_TALK03_e};
static s32 l_btp_ix_tbl[3] = {dRes_INDEX_HR_BTP_MABA_H_e, dRes_INDEX_HR_BTP_MABA_R01_e,
                              dRes_INDEX_HR_BTP_MABA_R02_e};
static s32 brow_bdl_table[2] = {dRes_INDEX_HR_BDL_H_BROW_e, dRes_INDEX_HR_BDL_R_BROW_e};
static s32 ant_bdl_table[2] = {dRes_INDEX_HR_BDL_H_ANT_e, dRes_INDEX_HR_BDL_R_ANT_e};
static s32 ant_bck_table[2] = {dRes_INDEX_HR_BCK_H_ANT_e, dRes_INDEX_HR_BCK_R_ANT_e};
static s8 msg_anm_table[9] = {0, 1, 3, 4, 7, 5, 9, 8, 6};
static char* action_table[19] = {"WAIT",      "SPEAK",   "PATTEN", "TACT0",     "TACT1",
                                 "TACT2",     "TACT3",   "WIND0",  "WIND1",     "HIDE_LINK",
                                 "DISP_LINK", "DISP_MY", "MOVE",   "TURN_LINK", "LOOK",
                                 "SMALL",     "INTRO",   "CHANGE", "DEBUG"};

static HrJ3DSysHD l_hrJ3DSys;
static Mtx34 l_hrCurrentMtx;
static HrMessageStaticsHD l_hrMessage;
static HrMessageManagerPtrHD l_hrMessageManager;
static cSAngle l_hrRightAngle;

static dCcD_SrcCyl dNpc_hr_src;
static dCcD_SrcCyl dNpc_cyl_src;

enum { fpcNm_NPC_HR_e = 0x16D };

s16 daNpc_hr_XyCheckCB(void* i_this, int i_itemBtn);
s16 daNpc_hr_XyEventCB(void* i_this, int);
static BOOL nodeCallBack_Hr(J3DNode* node, int calcTiming);
static BOOL nodeCallBack_Ht_ant(J3DNode* node, int calcTiming);
static BOOL CheckCreateHeap(fopAc_ac_c* i_this);
f32 hr_colorGamma(u32 c);
static inline u8 hr_gammaByte(u8 c);
static cPhs_State daNpc_Hr_Create(fopAc_ac_c* i_this);
static BOOL daNpc_Hr_Delete(daNpc_Hr_c* i_this);
static BOOL daNpc_Hr_Execute(daNpc_Hr_c* i_this);
static BOOL daNpc_Hr_Draw(daNpc_Hr_c* i_this);
static BOOL daNpc_Hr_IsDelete(daNpc_Hr_c*);

/* 02241F20 */
void daNpc_Wind_Eff::init() {
    mWindEffState = WIND_EFF_INACTIVE;
    mAlphaFactor = 0.0f;
    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
    mpSquallPos = nullptr;
}

/* 022424C0 */
void daNpc_Wind_Eff::remove() {
    if (mWindEffState != WIND_EFF_INACTIVE) {
        mpFollowECallBack.remove(); /* mpFollowECallBack.end() */
        init();
    }
}

/* 022433BC */
BOOL daNpc_Wind_Eff::create(cXyz* squallPos) {
    if (mWindEffState == WIND_EFF_INACTIVE) {
        mPos.copy(*squallPos);
        mAlphaFactor = 1.0f;
        mpSquallPos = squallPos;
        JPABaseEmitter* pEmitter =
            dComIfGp_particle_set(dPa_name::ID_AK_JN_WINDLINE00, &mPos, nullptr, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)(void*)&mpFollowECallBack);
        if (pEmitter == nullptr) {
            return FALSE;
        }
        mWindEffState = WIND_EFF_FADE_IN;
        return TRUE;
    }
    return FALSE;
}

/* 02243490 */
BOOL daNpc_Wind_Eff::end() {
    if (mWindEffState == WIND_EFF_FADE_IN || mWindEffState == WIND_EFF_ACTIVE) {
        mWindEffState = WIND_EFF_FADE_OUT;
        mpSquallPos = nullptr;
        return TRUE;
    }
    return FALSE;
}

/* 02242B8C */
void daNpc_Wind_Eff::proc() {
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
            ((HrEmitterHD*)pEmitter)->mGlobalAlpha =
                (u8)gabi::ftoi(alpha * 128.0f); /* setGlobalAlpha */
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
                ((HrEmitterHD*)pEmitter)->mGlobalAlpha = (u8)gabi::ftoi(mAlphaFactor * 128.0f);
            }
        } else {
            remove();
        }
        break;
    }
    }
}

/* 02242B2C */
void daNpc_Wind_Eff::setspd() {
    cXyz speed;
    cXyz_mi(mpSquallPos, &speed, &mPos);
    mSpeed.copy(speed);
}

/* 02242B7C */
void daNpc_Wind_Eff::move() {
    PSVECAdd(&mPos, &mSpeed, &mPos); /* mPos += mSpeed */
}

/* 022429E0 */
void daNpc_Wind_Clothes::setSquallPos(int param_1) {
    fopAc_ac_c* a_actor = fopAcM_SearchByID(mProcId);
    if (a_actor == nullptr) /* JUT_ASSERT(411, a_actor) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x19B, "a_actor");
    mSquallPos[param_1].copy(a_actor->current.pos);
    s16 cnt = mSquallCounter[param_1];
    f32 temp2 = gabi::fmadds(cM_ssin(cnt), 0.2f, 1.0f) * squallOrbitParam[2];
    cXyz temp;
    temp.x = gabi::fmadds(temp2, cM_scos(cnt), squallOrbitParam[0]);
    temp.y = gabi::fmadds(temp2, cM_ssin(cnt), squallOrbitParam[1]);
    temp.z = gabi::fmadds((f32)param_1, squallOrbitParam[3], squallOrbitParam[4]);
    fpoAcM_absolutePos(a_actor, &temp, &mSquallPos[param_1]);
}

/* 022434C4 */
BOOL daNpc_Wind_Clothes::create(fopAc_ac_c* i_this, u32 param_2, be<f32>* param_3, int param_4) {
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

/* 02243580 */
BOOL daNpc_Wind_Clothes::end() {
    mSquallState = 0;
    for (int i = 0; i < 4; i++) {
        mWindEff[i].end();
    }
    return TRUE;
}

/* 02242D24 */
void daNpc_Wind_Clothes::proc() {
    switch (mSquallState) {
    case 1: {
        fopAc_ac_c* a_actor = fopAcM_SearchByID(mProcId);
        if (a_actor == nullptr) /* JUT_ASSERT(478, a_actor) */
            JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x1DE, "a_actor");
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

/* 02241F48 */
void daNpc_Wind_Clothes::init() {
    mSquallState = 0;
    for (int i = 0; i < 4; i++) {
        mWindEff[i].init();
    }
}

/* 02242510 */
void daNpc_Wind_Clothes::remove() {
    for (int i = 0; i < 4; i++) {
        mWindEff[i].remove();
    }
}

/* 0224165C */
int daNpc_Hr_c::getShapeType() {
    return fopAcM_GetParam(this) & 0xFF;
}

/* 02243680 */
int daNpc_Hr_c::getSwbit() {
    return fopAcM_GetParam(this) >> 8 & 0xFF;
}

/* 02240E6C */
s16 daNpc_hr_XyCheckCB(void* i_this, int i_itemBtn) {
    return ((daNpc_Hr_c*)i_this)->XyCheckCB(i_itemBtn);
}

/* 02240E2C */
s16 daNpc_Hr_c::XyCheckCB(int i_itemBtn) {
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_WIND_WAKER_e ? TRUE : FALSE;
}

/* tail call */
/* 02240E70 */
s16 daNpc_hr_XyEventCB(void* i_this, int) {
    daNpc_Hr_c* a_this = (daNpc_Hr_c*)i_this;
    s16 idx = dComIfGp_evmng_getEventIdx("TACT_HT", 0xFF);
    a_this->mEventIdx = idx;
    return idx;
}

/* 02241C5C */
void daNpc_Hr_c::onHide(int param_1) {
    u16 fl = mFlags;
    if (!(fl & HR_FLAG_00000010) || param_1 != 0) {
        mFlags = (u16)(fl | HR_FLAG_00000010);
        mSmokeCallBack.remove(); /* mSmokeCallBack.end() */
    }
}

/* HD, new: a colour channel (0..255) to linear light, clamped to [0, 1]
 * (pow(c / 255, 2.2)); used where the HD emitters take their colours */
/* 02241C8C */
f32 hr_colorGamma(u32 c) {
    f32 in = (f32)c / 255.0f;
    f32 out = hrPowf(in, 2.2f);
    if (out < 0.0f) {
        return 0.0f;
    }
    if (out > 1.0f) {
        return 1.0f;
    }
    return out;
}

// HD: JPABaseEmitter::setGlobalPrmColor / setGlobalEnvColor store gamma-corrected colours
static inline u8 hr_gammaByte(u8 c) {
    return (u8)gabi::ftoi(hr_colorGamma(c) * 255.0f);
}

/* 02241D0C */
void daNpc_Hr_c::offHide(int param_1) {
    u16 fl = mFlags;
    if ((fl & HR_FLAG_00000010) || param_1 != 0) {
        s8 type = mType;
        mFlags = (u16)(fl & ~HR_FLAG_00000010);
        u16 particleID;
        switch (type) {
        case 1:
            particleID = dPa_name::ID_IT_SN_KINTOUN_RT00;
            break;
        default:
            particleID = dPa_name::ID_IT_SN_KINTOUN_FT00;
            break;
        }
        s8 roomNo = fopAcM_GetRoomNo(this);
        dPa_control_c* pa = dComIfGp_getParticle();
        JPABaseEmitter* pEmitter = dPa_control_set(
            pa, 0, particleID, &mCloudPos, &shape_angle, &scale, 0xFF,
            (dPa_levelEcallBack*)(void*)&mSmokeCallBack, roomNo, nullptr, nullptr, nullptr);
        if (pEmitter != nullptr && mType == 1) {
            HrEmitterHD* e = (HrEmitterHD*)pEmitter;
            u8 r = hr_gammaByte(0x97);
            u8 g = hr_gammaByte(0x76);
            u8 b = hr_gammaByte(0xA9);
            e->mPrmR = r; /* setGlobalPrmColor(0x97, 0x76, 0xA9) */
            e->mPrmG = g;
            e->mPrmB = b;
            r = hr_gammaByte(0x84);
            g = hr_gammaByte(0x70);
            b = hr_gammaByte(0x93);
            e->mEnvR = r; /* setGlobalEnvColor(0x84, 0x70, 0x93) */
            e->mEnvG = g;
            e->mEnvB = b;
        }
    }
}

/* 02241EBC */
void daNpc_Hr_c::defaultSetPos(cXyz* param_1) {
    f32 x = param_1->x;
    current.pos.x = x;
    f32 y = param_1->y;
    current.pos.y = y;
    f32 z = param_1->z;
    current.pos.z = z;
    cXyz* attPos = &attention_info.position; /* attention_info.position */
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

/* 0224401C */
s32 daNpc_Hr_c::getNowEventAction() {
    /* static char* action_table[19] (.data 0x101BE90C): WAIT, SPEAK, PATTEN, TACT0..3, WIND0,
     * WIND1, HIDE_LINK, DISP_LINK, DISP_MY, MOVE, TURN_LINK, LOOK, SMALL, INTRO, CHANGE, DEBUG */
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, action_table, 19, FALSE, 1);
}

/* 02240EB4 */
static BOOL nodeCallBack_Hr(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Hr_c* i_this = (daNpc_Hr_c*)model->getUserArea(); /* getUserArea() */
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
                cXyz temp;
                cXyz temp2;
                temp.x = 0.0f;
                temp.y = 0.0f;
                temp.z = 0.0f;
                MtxPosition(&temp, &temp2);
                i_this->mAttnBasePos.x = temp2.x; /* setAttentionBasePos(&temp2) */
                i_this->mAttnBasePos.y = temp2.y;
                i_this->mAttnBasePos.z = temp2.z;
                temp.x = 20.0f;
                temp.y = -20.0f;
                temp.z = 0.0f;
                MtxPosition(&temp, &temp2);
                i_this->mEyePos.x = temp2.x; /* setEyePos(&temp2) */
                i_this->mEyePos.y = temp2.y;
                i_this->mEyePos.z = temp2.z;
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]); /* getBackbone_y() */
            } else if (jntNo == (u32)(s32)i_this->m_waist_jnt_num) {
                cXyz temp;
                temp.x = 0.0f;
                temp.y = 0.0f;
                temp.z = 0.0f;
                MtxPosition(&temp, &i_this->mCloudPos);
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* 022412D8 */
void daNpc_Hr_c::node_Ht_ant(int jntNo) {
    if (jntNo == 0) {
        cXyz temp2;
        cXyz temp;
        cXyz temp3;
        temp2.y = 0.0f;
        temp2.z = 0.0f;
        temp2.x = 0.0f;
        MtxPosition(&temp2, &temp);
        u16 fl = mFlags;
        if (!(fl & HR_FLAG_00000800)) {
            mAntennaVelocity.x = 0.0f;
            mAntennaVelocity.y = 0.0f;
            mAntennaVelocity.z = 0.0f;
            mAntennaOffset.x = 0.0f;
            mAntennaOffset.y = 0.0f;
            mAntennaOffset.z = 0.0f;
            mPrevAntennaPos.copy(temp);
            mFlags = (u16)(fl | HR_FLAG_00000800);
        }
        cXyz_mi(&mPrevAntennaPos, &temp3, &temp);
        f32 x = temp3.x;
        f32 z = temp3.z;
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
        mPrevAntennaPos.copy(temp);
    } else {
        cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi(3000.0f * mAntennaRotation.x));
        cMtx_ZrotM(calc_mtx(), (s16)gabi::ftoi(3000.0f * mAntennaRotation.z));
    }
}

/* 02241550 */
static BOOL nodeCallBack_Ht_ant(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Hr_c* i_this = (daNpc_Hr_c*)model->getUserArea();
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

/* 02241664 */
BOOL daNpc_Hr_c::initTexPatternAnm(u32 i_modify) {
    J3DModelData* modelData = J3DModel_getModelData(mpHrMorf->getModel());
    /* l_btp_ix_tbl (.rodata 0x1001A7D8) */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes("Hr", l_btp_ix_tbl[mTexPatternIdx],
                                                                 HR_SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(1808, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0x710, "m_head_tex_pattern != (0)");
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f,
                            0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022425E8 */
void daNpc_Hr_c::playTexPatternAnm() {
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

/* 022435C4 */
u32 daNpc_Hr_c::setTexPtn(s32 param_1) {
    if ((u32)param_1 != (u32)(s32)mTexPatternIdx) {
        mTexPatternIdx = (s8)param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022426AC */
void daNpc_Hr_c::setAnm(s32 anmIdx) {
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
        dNpc_setAnm(mpHrMorf, -1 /* EMode_NULL */, morf, 1.0f, l_bck_ix_tbl[(s8)anmIdx], -1, "Hr");
        mAnmLoopCount = 0;
    }
}

/* 022435E4 */
void daNpc_Hr_c::setAnmStatus() {
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

/* 02242820 */
void daNpc_Hr_c::eventOrder() {
    if (ChkOrder(1)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
    }
    if (ChkOrder(2)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(4)) {
        eventInfo_onCondition(this, dEvtCnd_CANTALKITEM_e);
    }
}

/* 02242790 */
void daNpc_Hr_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
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

/* 02243644 */
u32 daNpc_Hr_c::next_msgStatus(be<u32>* pMsgNo) {
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

/* 0224368C */
u32 daNpc_Hr_c::getMsg() {
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

/* 02242898 */
void daNpc_Hr_c::setCollision() {
    cXyz temp;
    temp.copy(current.pos);
    f32 temp1 = 45.0f * scale.x;
    f32 temp2 = 100.0f * scale.y;
    mCyl.SetC(&temp);
    mCyl.SetR(temp1);
    mCyl.SetH(temp2);
    dComIfG_Ccsp_Set(&mCyl);
    if (mType == 1) {
        f32 sy = scale.y;
        temp2 = 35.0f * sy;
        temp.y = gabi::fmadds(-30.0f, sy, temp.y);
        temp1 = 80.0f * scale.x;
        mCyl2.SetC(&temp);
        mCyl2.SetR(temp1);
        mCyl2.SetH(temp2);
        dComIfG_Ccsp_Set(&mCyl2);
    }
}

/* 02242760 */
void daNpc_Hr_c::nextAnm(s32 i_nextAnm, int loopCount) {
    if ((s32)mAnmLoopCount < loopCount) {
        mAnmLoopCount = mAnmLoopCount + 1;
    } else {
        setAnm(i_nextAnm);
    }
}

/* 0224374C */
void daNpc_Hr_c::msgAnm(u32 param_1) {
    /* static s8 msg_anm_table[] = {0, 1, 3, 4, 7, 5, 9, 8, 6} (.data 0x101BE900) */
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = (u8)param_1;
        if (param_1 < 9) {
            setAnm(msg_anm_table[param_1]);
        }
    }
}

/* 0224277C */
void daNpc_Hr_c::talkInit() {
    mTalkState = TALK_INIT; /* TALK_INIT */
    mMsgAnmIdx = 0xFF;
}

/* 02243774 */
u16 daNpc_Hr_c::talk() {
    // HD: the message is the message manager (GameCube: l_msg found by l_msgId)
    u32 mgr = msgMgr();
    u16 msgStatus = HR_MSG_WAITING;
    if (mTalkState == TALK_INIT /* TALK_INIT */) {
        l_msgId() = fpcM_ERROR_PROCESS_ID_e;
        mCurrMsgNo = getMsg();
        mTalkState = TALK_MSG_CREATE; /* TALK_MSG_CREATE */
    } else if (mTalkState != TALK_FINISHED /* TALK_FINISHED */) {
        if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
            cXyz temp;
            temp.x = current.pos.x;
            temp.y = current.pos.y + 100.0f;
            temp.z = current.pos.z;
            l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, &temp);
        } else {
            msgAnm(dComIfGp_getMesgAnimeAttrInfo());
            switch ((u32)(s32)mTalkState) {
            case 1:
                mTalkState = TALK_ACTIVE; // HD: no fopMsgM_SearchByID
                break;
            case 2:
                msgStatus = (u16)msgMgr_getStatus(mgr);
                if (msgStatus == fopMsgStts_MSG_DISPLAYED_e) {
                    msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                        msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == fopMsgStts_BOX_CLOSED_e) {
                    msgMgr_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                    mTalkState = TALK_FINISHED;
                }
                break;
            }
        }
    }
    return msgStatus;
}

/* 02241F80 */
BOOL daNpc_Hr_c::init() {
    /* setAction(&daNpc_Hr_c::wait_action, NULL) (inline, always TRUE) */
    {
        ProcFunc_hr* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == gabi::guest_fn((const void*)hrWaitAction);
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = ACTION_ENDING; /* ACTION_ENDING */
                pmf_call_hr(this, cur, nullptr);
            }
            cur->f = gabi::guest_fn((const void*)hrWaitAction); /* wait_action */
            mActionStatus = ACTION_STARTING;                    /* ACTION_STARTING */
            cur->i = -1;
            cur->d = 0;
            wait_action(nullptr);
        }
    }
    mCloudPos.copy(current.pos);

    switch (mType) {
    case 1:
        /* HD (as GameCube retail) */
        if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_2710)) {
            onHide(1);
        } else {
            offHide(1);
        }
        dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0404);
        break;
    default:
        onHide(1);
        attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] =
            0x5B; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
        attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_CHECK_e;
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
        mCyl.Set(&dNpc_hr_src /* dNpc_hr_src */);
        mCyl2.Set(&dNpc_hr_src);
        break;
    default:
        mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
        eventInfo_setEventName(this, "HT_TALK");
        break;
    }
    mCyl.SetStts(&mStts);
    mCyl2.SetStts(&mStts2);
    defaultSetPos(&current.pos);
    mClothes.init();
    return TRUE;
}

/* 022438EC */
void daNpc_Hr_c::setAttention(int param_1) {
    if (chkFlag(HR_FLAG_00000010) || chkFlag(HR_FLAG_00000100)) {
        return;
    }
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 50.0f;
    attPos->z = mAttnBasePos.z;
    eyePos.z = mEyePos.z;
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
}

/* 02243954 */
u8 daNpc_Hr_c::getLookBackMode() {
    if (chkFlag(HR_FLAG_00000080)) {
        return 0;
    }
    if (chkFlag(HR_FLAG_00000040)) {
        return 1;
    }
    return 0xFF;
}

/* 0224397C */
void daNpc_Hr_c::lookBack() {
    cXyz eye; /* dNpc_playerEyePos result */
    cXyz temp2;
    cXyz temp; /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    // HD: current.angle.y is read once, before getLookBackMode() (GameCube re-reads it in the
    // default case)
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (getLookBackMode()) {
    case 0:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        dNpc_playerEyePos(&eye, -20.0f);
        temp2.copy(eye);
        dstPos = &temp2;
        tz = current.pos.z;
        tx = current.pos.x;
        ty = eyePos.y;
        break;
    case 1:
        dNpc_playerEyePos(&eye, -20.0f);
        temp2.copy(eye);
        dstPos = &temp2;
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
    temp.x = tx;
    temp.y = ty;
    temp.z = tz;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, &temp, desiredYRot, vel,
                              headOnlyFollow);
}

/* 02243B30 */
void daNpc_Hr_c::to_rt_hit() {
    mDoAud_seStart_noPos(JA_SE_CV_RC_DAMAGE);
    mDoAud_seStart_noPos(JA_SE_CV_RC_MODAE_1);
    setAnm(11);
    setTexPtn(1);
    setFlag(HR_FLAG_00000400);
    mHitDelayTimer = 0x52;
}

/* 02243B94 */
void daNpc_Hr_c::to_rt_tact() {
    mDoAud_seStart_noPos(JA_SE_CV_RC_DAMAGE);
    mDoAud_seStart_noPos(JA_SE_CV_RC_MODAE_2);
    mEventIdx = dComIfGp_evmng_getEventIdx("TACT0_RT", 0xFF);
    mReturnState = mState;
    mState = HR_STATE_RT_HIT_0;
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    mHitDelayTimer = 0x52;
    mDoAud_subBgmStop();
}

/* 02243E34 */
bool daNpc_Hr_c::wait01() {
    if (chkFlag(HR_FLAG_00000001)) {
        mReturnState = mState;
        mState = HR_STATE_TALK;
        setAnmStatus();
    } else {
        SetOrder(1);
    }
    return isMorf();
}

/* 02243EC8 */
void daNpc_Hr_c::endTalk() {
    mState = mReturnState;
    setAnmStatus();
    dComIfGp_event_reset();
    clrFlag(HR_FLAG_00000008 | HR_FLAG_00000001);
    field_0x60A = 5;
}

/* 02243F8C */
void daNpc_Hr_c::endTact() {
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

/* 02243C24 */
void daNpc_Hr_c::setEmitFlash(f32 param_1) {
    JPABaseEmitter* pEmitter = mSmokeCallBack.getEmitter();
    // HD: the lower clamp is an fsel (a NaN becomes 0)
    if (param_1 > 1.0f) {
        param_1 = 1.0f;
    } else {
        param_1 = (param_1 >= 0.0f) ? param_1 : 0.0f;
    }
    if (pEmitter != nullptr) {
        HrEmitterHD* e = (HrEmitterHD*)pEmitter;
        u8 r = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(104.0f, param_1, 151.0f)));
        u8 g = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(137.0f, param_1, 118.0f)));
        u8 b = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(86.0f, param_1, 169.0f)));
        e->mPrmR = r; /* setGlobalPrmColor */
        e->mPrmG = g;
        e->mPrmB = b;
        r = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(123.0f, param_1, 132.0f)));
        g = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(143.0f, param_1, 112.0f)));
        b = hr_gammaByte((u8)gabi::ftoi(gabi::fmadds(33.0f, param_1, 147.0f)));
        e->mEnvR = r; /* setGlobalEnvColor */
        e->mEnvG = g;
        e->mEnvB = b;
    }
}

/* 02243F20 */
bool daNpc_Hr_c::talk01() {
    if (talk() == fopMsgStts_BOX_CLOSED_e) {
        endTalk();
        daPy_offPlayerNoDraw(dComIfGp_getLinkPlayer());
    }
    return isMorf();
}

/* 02243168 */
BOOL daNpc_Hr_c::_draw() {
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
    mDoExt_btpAnm_entry(&mBtpAnm, modelData, mBlinkFrame);
    // HD: no Cyclos material table (the HD models are separate), mpHrMorf->entryDL() for both
    mpHrMorf->entryDL();
    J3DModel_setBaseTRMtx(mpBrowModel, getAnmMtx(hrModel, m_jnt.mHeadJntNum));
    mDoExt_modelUpdateDL(mpBrowModel);
    J3DModel_setBaseTRMtx(antennaModel, getAnmMtx(hrModel, m_jnt.mHeadJntNum));
    mpAntennaMorf->updateDL();
    ((HrModelDataHD*)modelData)->mpTexPattern = nullptr; /* mBtpAnm.remove(modelData) */
    if (getShapeType() == 1 && mState == HR_STATE_RT_ANGRY) {
        dSnap_RegistFig(DSNAP_TYPE_NPC_HR, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}

/* tail call */
/* 02242DF8 */
BOOL daNpc_Hr_c::_execute() {
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

/* tail call */
/* 02242558 */
BOOL daNpc_Hr_c::_delete() {
    dComIfG_resDelete(&mPhs, "Hr");
    if (mpHrMorf.get() != nullptr) {
        mpHrMorf->stopZelAnime();
    }
    mSmokeCallBack.remove(); /* mSmokeCallBack.end() */
    if (mType == 1) {
        dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0404);
    }
    mClothes.remove();
    return TRUE;
}

/* tail call */
/* 02241C58 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Hr_c*)i_this)->CreateHeap();
}

/* 022422B0 */
cPhs_State daNpc_Hr_c::_create() {
    /* fopAcM_ct(this, daNpc_Hr_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            hrActorConstruct(this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = HR_VTBL;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, HR_AAB_VTBL);
            dCcD_Stts_ct(&mStts2);
            dCcD_Cyl_ct(&mCyl2, HR_AAB_VTBL);
            hrJointControllerConstruct(&m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            dPa_followEcallBack_ct(&mSmokeCallBack, 0, 0);
            hrConstructArray(
                &mClothes, 4, sizeof(daNpc_Wind_Eff),
                gabi::guest_fn(
                    (const void*)
                        daNpc_Wind_Eff_ct)); /* __construct_array(mWindEff, daNpc_Wind_Eff ctor) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Hr");
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_HR_e:
            mType = getShapeType() == 1 ? 1 : 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0xB7B0)) {
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

/* tail call */
/* 02241778 */
BOOL daNpc_Hr_c::CreateHeap() {
    /* HD: Zephos and Cyclos have their own body models (GameCube: one model, Cyclos' body
     * material table swapped in at draw time) */
    J3DModelData* modelData;
    if (getShapeType() == 1) {
        modelData = (J3DModelData*)dComIfG_getObjectRes("Hr", dRes_INDEX_HR_BMT_R_BODY_e,
                                                        HR_SAFESTRING_VTBL);
    } else {
        modelData =
            (J3DModelData*)dComIfG_getObjectRes("Hr", dRes_INDEX_HR_BDL_HR_e, HR_SAFESTRING_VTBL);
    }
    if (modelData == nullptr) /* JUT_ASSERT(3435, modelData) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xD6B, "modelData");
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(
        "Hr", dRes_INDEX_HR_BCK_H_WAIT01_e, HR_SAFESTRING_VTBL);
    mpHrMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                      1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020203);
    if (mpHrMorf.get() == nullptr || mpHrMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "head");
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(3461, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xD85, "m_jnt.getHeadJntNum() >= 0");
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "backbone1");
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0)
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xD8A, "m_jnt.getBackboneJntNum() >= 0");
    s8 waist = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "waist");
    m_waist_jnt_num = waist;
    if (waist < 0)
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xD8E, "m_waist_jnt_num >= 0");

    /* brow_bdl_table (0x101BE8E8), ant_bdl_table (0x101BE8F0), ant_bck_table (0x101BE8F8) */
    J3DModelData* browModelData = (J3DModelData*)dComIfG_getObjectRes(
        "Hr", brow_bdl_table[getShapeType() & 1], HR_SAFESTRING_VTBL);
    if (browModelData == nullptr)
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xDAA, "browModelData");
    mpBrowModel = mDoExt_J3DModel__create(browModelData, 0x80000, 0x11000022);
    if (mpBrowModel.get() == nullptr) {
        return FALSE;
    }
    J3DModelData* antModelData = (J3DModelData*)dComIfG_getObjectRes(
        "Hr", ant_bdl_table[getShapeType() & 1], HR_SAFESTRING_VTBL);
    if (antModelData == nullptr)
        JUT_ASSERT_fail("d_a_npc_hr.cpp", 0xDC3, "antModelData");
    J3DAnmTransform* antBck = (J3DAnmTransform*)dComIfG_getObjectRes(
        "Hr", ant_bck_table[getShapeType() & 1], HR_SAFESTRING_VTBL);
    mpAntennaMorf =
        mDoExt_McaMorf::create(nullptr, antModelData, nullptr, nullptr, antBck, -1 /* EMode_NULL */,
                               1.0f, 0, -1, 1, nullptr, 0x80000, 0x11000022);
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
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum ||
            i == (u32)(s32)m_waist_jnt_num) {
            J3DModelData_setJointCallBack(J3DModel_getModelData(mpHrMorf->getModel()), i,
                                          gabi::guest_fn((const void*)nodeCallBack_Hr));
        }
    }
    mpHrMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    for (u16 i = 0; i < J3DModelData_getJointNum(antModelData); i++) {
        J3DModelData_setJointCallBack(J3DModel_getModelData(mpAntennaMorf->getModel()), i,
                                      gabi::guest_fn((const void*)nodeCallBack_Ht_ant));
    }
    mpAntennaMorf->getModel()->mUserArea = gabi::ea(this);
    return TRUE;
}

/* tail call */
/* 022424BC */
static cPhs_State daNpc_Hr_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Hr_c*)i_this)->_create();
}

/* 022425E4 */
static BOOL daNpc_Hr_Delete(daNpc_Hr_c* i_this) {
    return i_this->_delete();
}

/* 02243164 */
static BOOL daNpc_Hr_Execute(daNpc_Hr_c* i_this) {
    return i_this->_execute();
}

/* 022433B0 */
static BOOL daNpc_Hr_Draw(daNpc_Hr_c* i_this) {
    return i_this->_draw();
}

/* 022433B4 */
static BOOL daNpc_Hr_IsDelete(daNpc_Hr_c*) {
    return TRUE;
}

// clang-format off
/* WWHD glue
ext 0253E9B0 void hrEventInfoSetEventName(dEvt_info_c* info, const char* name)
ext 02246D58 BOOL hrWaitAction(daNpc_Hr_c* actor, void* arg) callback
ext 028F4560 f32 hrPowf(f32 value, f32 exponent)
ext 025D4ED0 void hrActorConstruct(fopAc_ac_c* actor)
ext 0259DAA0 void hrJointControllerConstruct(dNpc_JntCtrl_c* controller)
ext 028EFFD0 void hrConstructArray(void* array, u32 count, u32 size, u32 callback)
data dNpc_hr_src 101BE8A4
data dNpc_cyl_src 101EA190
data l_hrJ3DSys 104B462C
data l_hrCurrentMtx 104B4868
data l_hrMessage 104671CC
data l_hrMessageManager 101F4B5C
data l_hrRightAngle 101FF358
data l_bck_ix_tbl 1001A7A4
data l_btp_ix_tbl 1001A7D8
data brow_bdl_table 101BE8E8
data ant_bdl_table 101BE8F0
data ant_bck_table 101BE8F8
data msg_anm_table 101BE900
data action_table 101BE90C
ext 025D7970 BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind)
ext 025E1988 void mDoAud_seStart_noPos(u32 id)
ext 025E1928 void mDoAud_subBgmStop()
ext 025E1918 void mDoAud_subBgmStart(u32 id)
ext 025E1960 void mDoAud_bgmAllMute(s32 t)
ext 025E1E94 void mDoAud_tact_reset()
ext 025E1F58 void mDoAud_tact_melodyPlay(s32 i)
ext 025E1F14 void mDoAud_tact_ambientPlay()
ext 0257E490 void dKyw_tact_wind_set(s16 a, s16 b)
ext 0257E52C void dKyw_tact_wind_set_go()
ext 0257DAA8 cXyz* dKyw_get_wind_vec()
ext 0257E594 void dKyw_custom_windpower(f32 p)
ext 025553B8 void dKy_actor_addcol_set(s16 r, s16 g, s16 b, f32 ratio)
ext 02556B38 void dKy_vrbox_addcol_set(s16 r, s16 g, s16 b, f32 ratio)
ext 02007324 void cSGlobe_ct(void* g, const cXyz* v)
ext 02006644 void cSAngle_ct_copy(be<s16>* a, const be<s16>* b)
ext 020068CC void cSAngle_addeq(be<s16>* a, const be<s16>* b)
ext 02006838 f32 cSAngle_Cos(const be<s16>* a)
ext 02006814 f32 cSAngle_Sin(const be<s16>* a)
ext 025DA124 void fpoAcM_absolutePos(fopAc_ac_c* a, cXyz* rel, cXyz* out)
ext 025D9F38 fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm)
ext 025D68A0 s32 fopAcM_seenActorAngleY(fopAc_ac_c* a, fopAc_ac_c* b)
ext 0254DA38 void execItemGet(u8 item)
ext 0201B4E0 bool hrVectorIsZero(cXyz* v)
ext 0201B31C void hrVectorNormalize(cXyz* v, cXyz* res)
ext 02017300 s16 cM3d_CalcVecZAngle(const cXyz* v, csXyz* out)
ext 02055B64 s16 cLib_calcTimer(be<s16>* t)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 0259D454 BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 025F795C u32 msgMgr_getStatus(u32 m)
ext 025F74D0 void msgMgr_setStatus(u32 m, u32 st)
ext 025F7DB0 u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos)
str 1001A760 "TACT_HT" in 02240E70
str 1001A808 "Hr" in 02241664
str 1001A828 "d_a_npc_hr.cpp" in 02241664
str 1001A80C "m_head_tex_pattern != (0)" in 02241664
str 1001A840 "Hr" in 02241778
str 1001A874 "d_a_npc_hr.cpp" in 02241778
str 1001A84C "modelData" in 02241778
str 1001A844 "head" in 02241778
str 1001A884 "m_jnt.getHeadJntNum() >= 0" in 02241778
str 1001A858 "backbone1" in 02241778
str 1001A8A0 "m_jnt.getBackboneJntNum() >= 0" in 02241778
str 1001A838 "waist" in 02241778
str 1001A8C0 "m_waist_jnt_num >= 0" in 02241778
str 1001A864 "browModelData" in 02241778
str 1001A8D8 "antModelData" in 02241778
str 1001A900 "HT_TALK" in 02241F80
str 1001A908 "Hr" in 022422B0
str 1001A90B "Hr" in 02242558
str 1001A91C "Hr" in 022426AC
str 1001A940 "d_a_npc_hr.cpp" in 022429E0
str 1001A938 "a_actor" in 022429E0
str 1001A964 "d_a_npc_hr.cpp" in 02242D24
str 1001A95C "a_actor" in 02242D24
str 1001A97C "TACT0_RT" in 02243B94
*/
// clang-format on
