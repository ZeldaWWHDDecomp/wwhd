/**
 * d_a_npc_uk.cpp (WWHD)
 * NPC - Jin, Jan, & Jun-Roberto (the Killer Bees of Windfall)
 *
 * The GameCube decompilation of this TU
 * (zeldaret/tww src/d/actor/d_a_npc_uk.cpp) has only "Nonmatching" placeholders: every function
 * below is written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube
 * names and the TU's function list. Assert texts in the image give some original names
 * (m_maba_tex_pattern, table_bmt, head_bdl_table, headModelData, mk). "HD:" marks a known
 * difference from the GameCube build.
 */
#include "d/actor/d_a_npc_uk.h"

const gc_tu_data gc_tu = {
    .safeString=0x10022FF8,
    .objAcch={0x10023020,0x10023040,0x10023030},
    .objAcchDt={0x10023030,0x10023040},
};

#define UK_VTBL 0x10023050         /* daNpc_Uk_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_UK_e = 0x170 };
// HD: fixed direct targets stored in the eight-byte action descriptor.
enum : u32 { UK_WAIT_ACTION=0x022F5DB0, UK_HIND_ACTION=0x022F5E90, UK_VISIT_ACTION=0x022F6058, UK_SEEK_ACTION=0x022F6360 };

/* ---- file statics ---- */
static s32 l_bck_ix_tbl[11] = {16, 14, 15, 17, 13, 11, 8, 9, 7, 10, 12};
static s32 l_btp_ix_tbl[1] = {32};
static s32 head_bdl_table[3] = {20, 25, 21};
static u8 l_msg_anm_tbl[8] = {0, 1, 2, 3, 4, 6, 5, 8};
static const char* action_table[15] = {"WAIT", "TALK", "TALK2", "HOME", "RUN", "RUN3", "HIND", "DISP", "SPEAK", "LOOK_P", "WARNING", "JUMP", "JUMP2", "TURN", "WARP"};
static const char* staff_name_table[3] = {"UkB2", "UkC2", "UkD2"};
static u32 l_messageManager = 0;
static f32 l_clipperFar = 0.0f;
static u32 particleScaleInitialized = 0;
static cXyz particleScale = {0.0f, 0.0f, 0.0f};
/* l_msgId (HD: the message is reached through the message manager) */
static u32 l_msgId = 0;

// HD: exact-width compatibility adapters and message access.

static inline s32 uk_dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return dComIfGp_evmng_getMyActIdx(staffId, gabi::at<gptr<char>>(tbl), n, force, nameType);
}

/* the s16 event index is passed on as the callee returned it (GHS does not re-extend a s16 result) */
static inline u32 evmng_getEventIdx_raw(const char* name, u8 evNo) {
    return uk_getEventIdx(dComIfGp_getPEvtManager(), name, evNo);
}

// HD: message status belongs to the message manager.
static inline u32 msgMgr() { return l_messageManager; }

static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    return reinterpret_cast<UkTexPatternView*>(p)->getFrameMax();
}

static inline void dEvt_info_setEventName(fopAc_ac_c* a, const char* name) { uk_setEventName(&a->eventInfo, name); }
/* mDoLib_clipper::getFar(): the float at 0x1048D04C */
static inline f32 mDoLib_clipper_getFar() { return l_clipperFar; }

// HD: eight-byte GHS action descriptors, including virtual dispatch.
static inline BOOL pmf_call(void* self, UkActionFunc* pmf, void* arg) {
    return (static_cast<daNpc_Uk_c*>(self)->*(*pmf))(arg);
}

/* 12-word matrix copy: all loads, then all stores (lfs/stfs: an SNaN is quieted) */
static inline void mtx_wcopy(Mtx34* dst, const Mtx34* src) {
    u32 t[12];
    for (int i = 0; i < 12; i++) t[i] = reinterpret_cast<const be<u32>*>(src)[i];
    for (int i = 0; i < 12; i++) {
        u32 v = t[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u))
            v |= 0x00400000u;
        reinterpret_cast<be<u32>*>(dst)[i] = v;
    }
}

/* three bit-exact words (cXyz copied with lwz/stw) */
static inline void wcopy3(u32 dst, u32 src) {
    gabi::at<cXyz>(dst)->copy(*gabi::at<cXyz>(src));
}

/* setAction(target, NULL) (inline): end the current action, start the new one */
static inline void uk_setAction(daNpc_Uk_c* i_this, u32 target) {
    UkActionFunc* cur = &i_this->mCurrActionFunc;
    bool same = false;
    bool callOld = true;
    s16 i = cur->i;
    if (i == -1) {
        same = cur->d == 0 && cur->f == target;
    } else if (i == 0) {
        callOld = false;
    }
    if (!same) {
        if (callOld) {
            i_this->mActionStatus = daNpc_Uk_c::ACTION_ENDING;
            pmf_call(i_this, cur, nullptr);
        }
        i_this->mActionStatus = daNpc_Uk_c::ACTION_STARTING;
        cur->d = 0;
        cur->i = -1;
        cur->f = target;
        (i_this->*(*cur))(nullptr);
    }
}

/* square of the XZ length of {x, 0, z} (a stack vector passed to PSVECSquareMag) */
static f32 uk_sqMagXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    return PSVECSquareMag(v);
}
static inline u8 mk_mode(fopAc_ac_c* mk) { return reinterpret_cast<UkPartnerView*>(mk)->mVisitMode; } /* daNpc_Mk_c mode */

// HD: compiler-generated and template code of this translation unit.

/* 022F1060 */
u8 daNpc_Uk_c::getType() {
    return fopAcM_GetParam(this) & 0xFF;
}

/* 022F1144 */
u8 daNpc_Uk_c::getPath() {
    return (fopAcM_GetParam(this) >> 8) & 0xFF;
}

/* 022F0B68 */
u32 daNpc_Uk_c::getShapeType() {
    return (fopAcM_GetParam(this) >> 16) & 0xF;
}

/* 022F1068 */
u16 daNpc_Uk_c::getCaughtFlag() {
    switch (getShapeType()) {
    case 1:
        return 0x8;
    case 2:
        return 0x4;
    default:
        return 0x10;
    }
}

/* 022F26B8 */
u16 daNpc_Uk_c::getFoundFlag() {
    switch (getShapeType()) {
    case 1:
        return 0x180;
    case 2:
        return 0x140;
    default:
        return 0x1;
    }
}

/* 022F28F0 */
u16 daNpc_Uk_c::getFirstTalk() {
    switch (getShapeType()) {
    case 0:
        return 0x1202;
    case 1:
        return 0x1204;
    default:
        return 0x1201;
    }
}

/* 022F10B8 */
BOOL daNpc_Uk_c::chkGameStart() {
    if (dComIfGs_isTmpBit(0x40) && !dComIfGs_isTmpBit(getCaughtFlag())) {
        return TRUE;
    }
    return FALSE;
}

/* 022F53E0 */
BOOL daNpc_Uk_c::chkPositioning(f32 maxDist, f32 minY, f32 maxY, s16 angMin, s16 angMax) {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> v;  /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x14 */
    cXyz_mi(&player->current.pos, d, &current.pos);
    v->x = d->x;
    v->y = 0.0f;
    v->z = d->z;
    f32 dist = std_sqrtf(PSVECSquareMag(v));
    if (dist > maxDist) {
        return FALSE;
    }
    f32 dy = d->y;
    if (dy < minY || dy > maxY) {
        return FALSE;
    }
    s16 ang = (s16)(cLib_targetAngleY(&current.pos, &player->current.pos) - current.angle.y);
    if (angMin > angMax) {
        if (ang < angMin && ang > angMax) {
            return TRUE;
        }
        return FALSE;
    }
    if (ang < angMin || ang > angMax) {
        return TRUE;
    }
    return FALSE;
}

/* 022F43DC */
u8 daNpc_Uk_c::nextVisitMode() {
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    u8 mkMode;
    bool search = false;
    if (mk == nullptr) {
        mVisitMode = 0;
        search = true;
    } else {
        u8 mode = mVisitMode;
        mkMode = mk_mode(mk);
        if (mode == 10) {
            if (mVisitTimer != 0) {
                mVisitTimer = mVisitTimer - 1;
                return 10;
            }
        } else if (mode == 0) {
            search = true;
        }
    }
    if (search) {
        mk = uk_fopAcM_searchFromName("Mk" , 0xFF, 1);
        if (mk == nullptr) {
            return 0;
        }
        mPartnerId = fopAcM_GetID(mk);
        mkMode = mk_mode(mk);
    }
    if (mkMode == 2 || mkMode == 9 || mkMode == 8) {
        gabi::Local<cXyz> d;
        cXyz_mi(&mk->current.pos, d, &current.pos);
        f32 dx = d->x;
        u8 m = mVisitMode;
        f32 dz = d->z;
        if (m == 3) {
            if (uk_sqMagXZ(dx, dz) > 40000.0f) {
                return 2;
            }
            if (!(uk_sqMagXZ(dx, dz) < 6400.0f)) { /* bge: taken on NaN */
                return 3;
            }
            return 4;
        }
        if (m == 4) {
            if (uk_sqMagXZ(dx, dz) > 10000.0f) {
                return 3;
            }
            return 4;
        }
        if (!(uk_sqMagXZ(dx, dz) < 22500.0f)) { /* bge: taken on NaN */
            return 2;
        }
        return 3;
    }
    u8 m = mVisitMode;
    if (m == 9) {
        if (mVisitTimer != 0) {
            mVisitTimer = mVisitTimer - 1;
            return 9;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(&player->current.pos, d, &current.pos);
        if (uk_sqMagXZ(d->x, d->z) < 22500.0f) {
            return 7;
        }
        if (mkMode == 4 || mkMode == 5) {
            return 5;
        }
        s16 diff = (s16)(fopAcM_searchActorAngleY(this, mk) - shape_angle.y);
        if (diff < 0) {
            diff = (s16)-diff;
        }
        if (diff < 0x3800) {
            return 2;
        }
        return 9;
    }
    if (mkMode == 6) {
        if (m == 8) {
            if (mVisitTimer != 0) {
                mVisitTimer = mVisitTimer - 1;
                return 8;
            }
            return 9;
        }
        return 8;
    }
    if (mkMode == 7) {
        return 10;
    }
    gabi::Local<cXyz> d;
    cXyz_mi(&player->current.pos, d, &current.pos);
    u8 m2 = mVisitMode;
    f32 dx = d->x;
    f32 dz = d->z;
    if (m2 < 5 || m2 > 7) {
        if (!(uk_sqMagXZ(dx, dz) < 22500.0f)) { /* bge: taken on NaN */
            return 2;
        }
        return 7;
    }
    if (m2 == 5) {
        if (uk_sqMagXZ(dx, dz) < 22500.0f) {
            return 7;
        }
        return 5;
    }
    if (uk_sqMagXZ(dx, dz) > 32400.0f) {
        return 5;
    }
    u8 m3 = mVisitMode;
    if (m3 == 7) {
        u8 t = mVisitTimer2;
        if (t == 0) {
            return 6;
        }
        m3 = mVisitMode;
        mVisitTimer2 = (u8)(t - 1);
    }
    return m3;
}

/* 022F4D0C */
void daNpc_Uk_c::approachRun(fopAc_ac_c* i_actor) {
    gabi::Local<be<s16>> angle; /* sp+0x08 */
    gabi::Local<cXyz> pos;      /* sp+0x0C */
    gabi::Local<cXyz> target;   /* sp+0x18 */
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    target->y = i_actor->current.pos.y;
    target->z = i_actor->current.pos.z;
    target->x = i_actor->current.pos.x;
    dNpc_calc_DisXZ_AngY(pos, target, nullptr, angle);
    cLib_addCalcAngleS2(&current.angle.y, *angle, 8, 0x800);
}

/* 022F4D8C */
void daNpc_Uk_c::aroundWalk(fopAc_ac_c* i_actor) {
    mMk.aroundWalk(this, i_actor, mVisitTimer2);
}

/* 022F4DA0 */
void daNpc_Uk_c::surrender() {
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk == nullptr || !(mk->speedF > 1.0f)) {
        return;
    }
    gabi::Local<cXyz> v;  /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x18 */
    cXyz_mi(&mk->current.pos, d, &current.pos);
    v->x = d->x;
    v->y = 0.0f;
    v->z = d->z;
    if (PSVECSquareMag(v) > 7225.0f) {
        return;
    }
    s16 diff = (s16)(fopAcM_searchActorAngleY(mk, this) - mk->shape_angle.y);
    s16 mkAngle = mk->shape_angle.y;
    if ((u32)((s32)diff + 0x2AAA) >= 0x5555u) {
        return;
    }
    s16 dir;
    if (diff >= 0) {
        dir = (s16)(mkAngle + 0x4000);
    } else {
        dir = (s16)(mkAngle - 0x4000);
    }
    speed.x = gabi::fmadds(cM_ssin(dir), 0.5f, speed.x);
    setFlag(FLAG_PUSHED);
    speed.z = gabi::fmadds(cM_scos(dir), 0.5f, speed.z);
}

/* 022F588C */
void daNpc_Uk_c::runawayInit() {
    switch (mMk.m0) {
    case 1:
    case 2:
    case 5:
        if (mCurrAnmIdx == 5) {
            u32 se;
            switch (getShapeType()) {
            case 1:
                se = 0x490D;
                break;
            case 2:
                se = 0x4911;
                break;
            default:
                se = 0x4909;
                break;
            }
            mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        }
        speedF = 0.0f;
        setAnm(4, 0);
        break;
    case 3:
        speedF = 0.0f;
        setAnm(5, 0);
        break;
    case 4: {
        speedF = 8.0f;
        setFlag(FLAG_AIRBORNE);
        speed.y = 25.0f;
        setAnm(8, 0);
        mState = STATE_JUMP;
        current.angle.y = mDemoTargetAngle;
        u32 se;
        switch (getShapeType()) {
        case 1:
            se = 0x490F;
            break;
        case 2:
            se = 0x4913;
            break;
        default:
            se = 0x490B;
            break;
        }
        mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    }
    }
}

/* 022F08DC */
static BOOL nodeCallBack_Uk(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Uk_c* i_this = gabi::at<daNpc_Uk_c>(model->mUserArea);
        u32 jntNo = ((J3DJoint*)node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
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
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            mtx_wcopy(model->getAnmMtx(jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* 022F0B74 */
BOOL daNpc_Uk_c::initTexPatternAnm(u32 i_modify) {
    J3DModelData* modelData = mpHeadModel->getModelData();
    if (getShapeType() != 1) {
        return TRUE;
    }
    J3DAnmTexPattern* pat = (J3DAnmTexPattern*)dComIfG_getObjectRes("Uk" , l_btp_ix_tbl[0]);
    m_head_tex_pattern = pat;
    if (pat == nullptr) { /* JUT_ASSERT(0x2B0, m_maba_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x2B0, "m_maba_tex_pattern != (0)");
        pat = m_head_tex_pattern;
    }
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, pat, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022F1D14 */
void daNpc_Uk_c::playTexPatternAnm() {
    if (getShapeType() == 1 && cLib_calcTimer(&mBlinkTimer) == 0) {
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

/* 022F1DE4 */
void daNpc_Uk_c::setAnm(s8 anmIdx, u8 force) {
    f32 morf = 8.0f;
    if (anmIdx == 4 && mCurrAnmIdx == 10) {
        morf = 0.0f;
    }
    if (force & 1) {
        morf = 0.0f;
        mCurrAnmIdx = 11;
    }
    if (anmIdx != mCurrAnmIdx && anmIdx != -1) {
        mAnmTimer = 0.0f;
        mCurrAnmIdx = anmIdx;
        dNpc_setAnm(mpMorf, -1, morf, 1.0f, l_bck_ix_tbl[anmIdx] /* l_bck_ix_tbl */, -1, "Uk" );
    }
}

/* 022F2750 */
BOOL daNpc_Uk_c::chkAttentionLocal() {
    u32 attention = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    if (chkFlag(FLAG_TALKING)) {
        return TRUE;
    }
    if (mTalkWait != 0) {
        mTalkWait = mTalkWait - 1;
        return TRUE;
    }
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    }
    return this == dAttention_ActionTarget(attention, 0);
}

/* 022F2830 */
void daNpc_Uk_c::chkAttention() {
    u32 old = (mFlags >> 2) & 1;
    if (chkAttentionLocal()) {
        setFlag(FLAG_ATTENTION);
    } else {
        clrFlag(FLAG_ATTENTION);
    }
    if (old != ((mFlags >> 2) & 1u) && old != 0) {
        m_jnt.mbTrn = 1; /* setTrn() */
    }
}

/* 022F2708 */
void daNpc_Uk_c::setAnmStatus() {
    switch ((s32)mState) {
    case STATE_WARNING_B:
    case STATE_WARNING_C:
    case STATE_WARNING_D:
        setAnm(5, 0);
        break;
    case STATE_JITANDA01:
    case STATE_JITANDA02:
        break;
    default:
        setAnm(0, 0);
        break;
    }
}

/* 022F1EE8 */
void daNpc_Uk_c::eventOrder() {
    if (ChkOrder(3)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (ChkOrder(2)) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022F1E94 */
void daNpc_Uk_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd != 2 /* !checkCommandDemoAccrpt() */) {
        if (cmd == 1 /* checkCommandTalk() */ && ChkOrder(3)) {
            setFlag(FLAG_TALKING);
            talkInit();
        }
    }
    ClrOrder();
}

/* 022F28B4 */
u32 daNpc_Uk_c::next_msgStatus(be<u32>* pMsgNo) {
    switch ((u32)*pMsgNo) {
    case 0x26BF:
    case 0x26C1:
    case 0x26C3:
    case 0x26D8:
        *pMsgNo = *pMsgNo + 1;
        return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    }
    return 0x10; /* fopMsgStts_MSG_ENDS_e */
}

/* 022F293C */
u32 daNpc_Uk_c::getMsg() {
    u32 msgNo;
    switch ((s8)mType) {
    case 1:
        if (mState == STATE_DEMO || mState == STATE_EVENT) {
            return mMsgNo;
        }
        if (dComIfGs_isTmpBit(0x40)) {
            return getShapeType() + 0x26B6;
        }
        if (dComIfGs_isEventBit(0x1340)) {
            return getShapeType() + 0x26AE;
        }
        if (dComIfGs_isEventBit(getFirstTalk())) {
            return getShapeType() + 0x26BA;
        }
        msgNo = getShapeType() * 2 + 0x26BF;
        dComIfGs_onEventBit(getFirstTalk());
        return msgNo;
    case 2:
        if (dComIfGs_isTmpBit(getCaughtFlag())) {
            return getShapeType() + 0x26B6;
        }
        msgNo = getShapeType() + 0x26B2;
        dComIfGs_onTmpBit(getCaughtFlag());
        return msgNo;
    default:
        return 0;
    }
}

/* 022F1F10 */
void daNpc_Uk_c::setCollision() {
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    mCyl.SetC(centerPos);
    mCyl.SetR(30.0f);
    mCyl.SetH(80.0f);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 022F2AAC */
void daNpc_Uk_c::msgAnm(u32 attr) {
    /* u8 argument, compared unnormalised (callers pass a lbz value) */
    if ((u32)mMsgAnmIdx != attr) {
        mMsgAnmIdx = (u8)attr;
        if (attr <= 7) {
            setAnm((s8)l_msg_anm_tbl[attr] /* l_msg_anm_tbl */, 0);
        }
    }
}

/* 022F1E80 */
void daNpc_Uk_c::talkInit() {
    mTalkState = 0;
    mMsgAnmIdx = 0xFF;
}

/* 022F2ADC */
u16 daNpc_Uk_c::talk() {
    // HD: the message is the message manager (GameCube: l_msg found by l_msgId)
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    s8 talkState = mTalkState;
    if (talkState == 0) {
        l_msgId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = 1;
    } else if (talkState != -1) {
        if (l_msgId == 0xFFFFFFFF) {
            l_msgId = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            if (!chkFlag(FLAG_NO_MSG_ANIMATION)) {
                msgAnm(dComIfGp_getMesgAnimeAttrInfo());
                talkState = mTalkState;
            }
            switch ((s32)talkState) {
            case 1:
                mTalkState = 2; // HD: no fopMsgM_SearchByID
                if (chkFlag(FLAG_SET_TALK_PARTNER)) {
                    dComIfGp_event_setTalkPartner(this);
                    clrFlag(FLAG_SET_TALK_PARTNER);
                }
                break;
            case 2:
                msgStatus = msgMgr_getStatus(mgr);
                if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                    msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (msgMgr_getStatus(mgr) == 0xF) {
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

/* 022F1150 */
BOOL daNpc_Uk_c::init() {
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9; /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    gravity = -3.3f; // HD: this actor uses the reduced gravity scale.
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mtx_wcopy(&model->mBaseTransformMtx, mDoMtx_stack_c::get()); /* model->setBaseTRMtx */
    mpMorf->calc();
    mStts.Init(0x64, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    actor_status = (actor_status & ~0x3Fu) | 0x27; /* fopAcM_SetStatusMap(0x27) + OnStatus(SHOWMAP) */
    switch ((s8)mType) {
    case 0:
        uk_setAction(this, UK_WAIT_ACTION); /* wait_action */
        break;
    case 1:
        if (chkGameStart()) {
            uk_setAction(this, UK_HIND_ACTION); /* hind_action */
            actor_status &= ~0x20u;
            setFlag(FLAG_HIDDEN);
        } else {
            uk_setAction(this, UK_VISIT_ACTION); /* visit_action */
        }
        break;
    case 2:
        mStts.SetWeight(0xFE);
        dNpc_PathRun_setInf(&mPath, getPath(), current.roomNo, 1);
        if (mPath.mPath.get() == nullptr) {
            return FALSE;
        }
        if (chkGameStart()) {
            uk_setAction(this, UK_SEEK_ACTION); /* seek_action */
        } else {
            setFlag(FLAG_HIDDEN);
            actor_status &= ~0x20u;
            uk_setAction(this, UK_HIND_ACTION); /* hind_action */
        }
        actor_status |= 0x40u;
        break;
    }
    mEyePos.copy(current.pos);
    mAttnBasePos.copy(current.pos);
    f32 far = mDoLib_clipper_getFar();
    if (far > 1.0f) {
        cullSizeFar = 5000.0f / far; /* fopAcM_setCullSizeFar */
    }
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}

/* 022F2C80 */
void daNpc_Uk_c::setAttention(u32 param_1) {
    /* bool argument, tested unnormalised (callers pass a BOOL result) */
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    cXyz* att = &attention_info.position; /* attention_info.position */
    att->x = mAttnBasePos.x;
    att->y = gabi::fadds_ppc(mAttnBasePos.y, 45.0f);
    att->z = mAttnBasePos.z;
    eyePos.z = mEyePos.z; /* eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z) */
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
}

/* 022F2CD4 */
u32 daNpc_Uk_c::getLookBackMode() {
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk != nullptr && mState == STATE_VISIT) {
        u8 visitMode = mVisitMode;
        if (visitMode == 8 || visitMode == 4) {
            return 3;
        }
        u8 mkMode = reinterpret_cast<UkPartnerView*>(mk)->mVisitMode; /* daNpc_Mk_c mode */
        if (mkMode == 8 || mkMode == 9) {
            return 2;
        }
    }
    if (chkFlag(FLAG_LOOK_PARTNER)) {
        if (uk_fopAcM_getTalkEventPartner(dComIfGp_getLinkPlayer()) != this) {
            return 4;
        }
        return 2;
    }
    if (chkFlag(FLAG_NO_HEAD_TRACK)) {
        return 0;
    }
    if (chkFlag(FLAG_ATTENTION)) {
        return 1;
    }
    if (mState == STATE_VISIT && (mVisitMode == 6 || mVisitMode == 7)) {
        return 1;
    }
    return 2;
}

/* 022F2E18 */
void daNpc_Uk_c::lookBack() {
    gabi::Local<cXyz> target;  /* sp+0x08 */
    gabi::Local<cXyz> eye;     /* sp+0x14: passed by value (a copy) */
    gabi::Local<cXyz> eyeRes;  /* sp+0x24: dNpc_playerEyePos result */
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnly = 0;
    u32 mode = getLookBackMode();
    f32 eyeY = eyePos.y;
    f32 posZ = current.pos.z;
    f32 posX = current.pos.x;
    switch (mode) {
    case 0:
        m_jnt.mbTrn = 1; /* setTrn() */
        dNpc_playerEyePos(eyeRes, -20.0f);
        target->copy(*eyeRes);
        dstPos = target;
        break;
    case 1:
        headOnly = 1;
        dNpc_playerEyePos(eyeRes, -20.0f);
        target->copy(*eyeRes);
        dstPos = target;
        break;
    case 2:
        desiredYRot = current.angle.y;
        headOnly = 1;
        break;
    case 3:
        if (mk == nullptr) /* JUT_ASSERT(0x4EE, mk != NULL) */
            JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x4EE, "mk != (0)");
        m_jnt.mbTrn = 1;
        reinterpret_cast<be<u32>*>(target.get())[0] = reinterpret_cast<be<u32>*>(&mk->current.pos)[0];
        target->y = mk->eyePos.y;
        reinterpret_cast<be<u32>*>(target.get())[2] = reinterpret_cast<be<u32>*>(&mk->current.pos)[2];
        dstPos = target;
        break;
    case 4: {
        fopAc_ac_c* partner = uk_fopAcM_getTalkEventPartner(dComIfGp_getLinkPlayer());
        if (partner == nullptr) {
            partner = dComIfGp_getLinkPlayer();
        }
        m_jnt.mbTrn = 1;
        target->copy(partner->eyePos);
        posZ = current.pos.z;
        posX = current.pos.x;
        eyeY = eyePos.y;
        dstPos = target;
        break;
    }
    }
    s16 vel;
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x5DC, 4, 0x800);
        vel = mMaxHeadTurnVelocity;
    } else {
        mMaxHeadTurnVelocity = 0;
        vel = 0;
    }
    eye->x = posX;
    eye->y = eyeY;
    eye->z = posZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, vel, headOnly);
}

/* 022F335C */
u32 daNpc_Uk_c::getStaffName() {
    if (mType == 2) {
        u32 shape = getShapeType();
        if (shape <= 2) {
            return gabi::guest_addr(staff_name_table[shape]); /* "UkB2", "UkC2", "UkD2" */
        }
        return gabi::guest_addr("UkB");
    }
    switch (getShapeType()) {
    case 1:
        return gabi::guest_addr("UkC");
    case 2:
        return gabi::guest_addr("UkD");
    default:
        return gabi::guest_addr("UkB");
    }
}

/* 022F3458 */
BOOL daNpc_Uk_c::checkDemoStart() {
    u32 name = getStaffName();
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(name), nullptr, 0);
    mStaffIdx = staffIdx;
    if (staffIdx != -1) {
        mEventAction = getNowEventAction();
        return TRUE;
    }
    return FALSE;
}

/* 022F340C */
s32 daNpc_Uk_c::getNowEventAction() {

    return uk_dComIfGp_evmng_getMyActIdx(mStaffIdx, gabi::ea(action_table), 15, FALSE, 0);
}

/* 022F4B44 */
void daNpc_Uk_c::visitInit(u32 mode) {
    /* u8 argument, compared unnormalised */
    switch (mode) {
    case 2:
        setAnm(4, 0);
        mVisitMode = (u8)mode;
        break;
    case 3:
    case 6:
        setAnm(3, 0);
        mVisitMode = (u8)mode;
        break;
    case 4:
        setAnm(0, 0);
        speedF = 0.0f;
        mVisitMode = (u8)mode;
        break;
    case 5:
        setAnm(4, 0);
        if (mVisitMode == 9) {
            mApproachTimer = 0x2D;
        } else {
            mApproachTimer = 0;
        }
        mVisitMode = (u8)mode;
        break;
    case 7: {
        u8 m = mVisitMode;
        if (m == 9 || m == 8 || m == 2) {
            mVisitTimer2 = 0x2D;
        } else {
            mVisitTimer2 = mApproachTimer;
        }
        setAnm(4, 0);
        mVisitMode = (u8)mode;
        break;
    }
    case 8:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 5;
        break;
    case 9:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 15;
        break;
    case 10:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 30;
        break;
    default:
        speedF = 0.0f;
        mVisitMode = (u8)mode;
        break;
    }
}

/* 022F4F28 */
void daNpc_Uk_c::visitProc() {
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    switch (mVisitMode) {
    case 2:
        if (mk == nullptr) /* JUT_ASSERT(0x5A0, mk != NULL) */
            JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x5A0, "mk != (0)");
        else
            approachRun(mk);
        break;
    case 3:
        if (mk == nullptr) /* JUT_ASSERT(0x5AB, mk != NULL) */
            JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x5AB, "mk != (0)");
        else
            approachRun(mk);
        break;
    case 5:
        if (mApproachTimer < 0x2D) {
            mApproachTimer = mApproachTimer + 1;
        }
        approachRun(player);
        break;
    case 6:
    case 7:
        aroundWalk(player);
        break;
    case 4:
    case 8:
    case 9:
    case 10:
        surrender();
        break;
    }
    u8 m = mVisitMode;
    if (m >= 9 && m <= 10) {
        setFlag(FLAG_NO_HEAD_TRACK);
    }
}

/* 022F3138 */
BOOL daNpc_Uk_c::wait01() {
    if (chkFlag(FLAG_TALKING)) {
        s8 state = mState;
        mState = STATE_TALK;
        mReturnToState = state;
        setAnmStatus();
    } else {
        SetOrder(1);
    }
    return isMorf();
}

/* 022F31CC */
BOOL daNpc_Uk_c::talk01() {
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        mTalkWait = 5;
        clrFlag(FLAG_TALKING);
    }
    setFlag(FLAG_NO_HEAD_TRACK);
    return isMorf();
}

/* 022F325C */
BOOL daNpc_Uk_c::talk02() {
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (!mMk.chkGameSet()) {
            mState = mReturnToState;
            setAnmStatus();
            dComIfGp_event_reset();
            mTalkWait = 5;
            clrFlag(FLAG_TALKING);
        } else {
            u32 idx = evmng_getEventIdx_raw("MK_GAMESET" , 0xFF);
            mEventIdx = (s16)idx;
            fopAcM_orderChangeEventId_raw(this, idx, 0, 0xFFFF);
            mDoAud_seStart_1(0x8F1);
            mState = STATE_EVENT;
            mReturnToState = STATE_HIDDEN;
            mTalkWait = 5;
            clrFlag(FLAG_TALKING);
        }
    }
    setFlag(FLAG_NO_HEAD_TRACK);
    return isMorf();
}

/* 022F41B4 */
u32 daNpc_Uk_c::visitTalkInit() {
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        mEventIdx = eventInfo.mEventId; /* eventInfo.mEventId */
        dComIfGp_event_setTalkPartner(fopAcM_SearchByID(mPartnerId));
        mState = STATE_EVENT;
        demo02();
        dComIfGs_onEventBit(0x1E04);
        return 4;
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23) && !dComIfGs_checkGetItem(0x26)) {
        mEventIdx = eventInfo.mEventId;
        dComIfGp_event_setTalkPartner(fopAcM_SearchByID(mPartnerId));
        mState = STATE_EVENT;
        demo02();
        dComIfGs_onEventBit(0x1602);
        return 4;
    }
    return 1;
}

/* 022F5088 */
void daNpc_Uk_c::visitSetEvent() {
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        dEvt_info_setEventName(this, "MK_TALK3" );
    } else if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23) && !dComIfGs_checkGetItem(0x26)) {
        dEvt_info_setEventName(this, "MK_TALK2" );
    } else {
        dEvt_info_setEventName(this, "MK_TALK" );
    }
}

/* 022F515C */
BOOL daNpc_Uk_c::visit01() {
    if (chkFlag(FLAG_TALKING)) {
        mReturnToState = mState;
        mState = (s8)visitTalkInit();
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = 1;
        return TRUE;
    }
    if (checkDemoStart()) {
        s8 state = mState;
        mState = STATE_DEMO;
        mReturnToState = state;
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = 1;
        demo01();
        return TRUE;
    }
    u32 next = nextVisitMode();
    if (next != mVisitMode) {
        visitInit(next);
    }
    visitProc();
    f32 target = 0.0f; // HD: retain the verified initialized speed target.
    if (mCurrAnmIdx == 3) {
        if (mVisitMode == 6) {
            target = 2.5f;
        } else {
            target = mWalkSpeed;
        }
    } else if (mCurrAnmIdx == 4) {
        target = mRunSpeed;
        if (mVisitMode == 7) {
            target = gabi::fsubs_ppc(target, 2.5f);
        }
    }
    f32 step = (gabi::fsubs_ppc(target, speedF) >= 0.0f) ? 1.1f : 2.8f;
    cLib_chaseF(&speedF, target, step);
    SetOrder(1);
    u8 m = mVisitMode;
    if (m == 6 || m == 7) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        s16 diff = (s16)(fopAcM_searchActorAngleY(player, this) - player->shape_angle.y);
        if (diff < 0) {
            diff = (s16)-diff;
        }
        if (diff > 0x1800) {
            mOrderFlags = (u8)(mOrderFlags & ~1);
        }
    }
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk != nullptr && (mk_mode(mk) == 8 || mk_mode(mk) == 9)) {
        mOrderFlags = (u8)(mOrderFlags & ~1);
    }
    if (ChkOrder(1)) {
        visitSetEvent();
    }
    return TRUE;
}

/* 022F554C */
BOOL daNpc_Uk_c::warningB() {
    if (chkPositioning(300.0f, -10.0f, 80.0f, 0x3A98, -0x3A98)) {
        mState = STATE_FOUND;
        u32 idx = evmng_getEventIdx_raw("UkB_FOUND" , 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}

/* 022F55E8 */
BOOL daNpc_Uk_c::warningC() {
    if (chkPositioning(180.0f, -10.0f, 80.0f, 0x3A98, -0x1388)) {
        mState = STATE_FOUND;
        u32 idx = evmng_getEventIdx_raw("UkC_FOUND" , 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}

/* 022F5684 */
BOOL daNpc_Uk_c::warningD() {
    if (chkPositioning(150.0f, -10.0f, 80.0f, 0x4650, 0)) {
        mState = STATE_FOUND;
        u32 idx = evmng_getEventIdx_raw("UkD_FOUND" , 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    if (chkPositioning(150.0f, -10.0f, 80.0f, 0, -0x4650)) {
        mState = STATE_FOUND;
        u32 idx = evmng_getEventIdx_raw("UkD_FOUND2" , 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}

/* 022F57DC */
BOOL daNpc_Uk_c::found() {
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mReturnToState = STATE_RUNAWAY;
        mState = STATE_EVENT;
        mMk.init(0x50, 0x12C);
        dComIfGs_onTmpBit(getFoundFlag());
        return demo02();
    }
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    return FALSE;
}

/* 022F5AE8 */
BOOL daNpc_Uk_c::runaway() {
    u32 st = Mk_runAwayProc(&mMk, this, &mPath, &mCyl, &mDemoTargetAngle);
    u32 old = mMk.m0;
    if (st != old) {
        if ((st == 1 && old == 2) || (st == 2 && old == 1)) {
            if (cM_rndF(1.0f) < 0.5f) {
                u32 se;
                switch (getShapeType()) {
                case 1:
                    se = 0x490E;
                    break;
                case 2:
                    se = 0x4912;
                    break;
                default:
                    se = 0x490A;
                    break;
                }
                mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            }
        }
        mMk.m0 = (u8)st;
        runawayInit();
    }
    s8 anm = mCurrAnmIdx;
    if (st == 3) {
        setFlag(FLAG_NO_HEAD_TRACK);
    }
    if (anm == 4) {
        cLib_chaseF(&speedF, mMk.getSpeedF(15.0f, 18.0f), 2.8f);
    }
    return FALSE;
}

/* 022F5C5C */
BOOL daNpc_Uk_c::jump() {
    if (!chkFlag(FLAG_AIRBORNE)) {
        setAnm(0, 0);
        speedF = 0.0f;
        if (isMorf()) {
            mState = STATE_JITANDA01;
            setAnm(6, 0);
            SetOrder(2);
        }
    }
    return TRUE;
}

/* 022F5CEC */
BOOL daNpc_Uk_c::jitanda01() {
    if (chkFlag(FLAG_TALKING)) {
        setFlag(FLAG_NO_MSG_ANIMATION);
        mReturnToState = STATE_JITANDA02;
        mState = STATE_TALK;
        return TRUE;
    }
    SetOrder(2);
    return TRUE;
}

/* 022F5D2C */
BOOL daNpc_Uk_c::jitanda02() {
    if (chkFlag(FLAG_TALKING)) {
        s8 state = mState;
        mState = STATE_TALK;
        mReturnToState = state;
    }
    if (checkDemoStart()) {
        s8 state = mState;
        mState = STATE_DEMO;
        mReturnToState = state;
        demo01();
        return TRUE;
    }
    SetOrder(1);
    return TRUE;
}

/* 022F437C */
BOOL daNpc_Uk_c::demo01() {
    if (!checkDemoStart()) {
        mState = mReturnToState;
        setAnmStatus();
        return TRUE;
    }
    return demoProc();
}

/* 022F40EC */
BOOL daNpc_Uk_c::demo02() {
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        mTalkWait = 5;
        clrFlag(FLAG_TALKING);
        return TRUE;
    }
    if (!checkDemoStart()) /* JUT_ASSERT(0x796, 0) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x796, "0");
    return demoProc();
}

/* 022F34E0 */
void daNpc_Uk_c::demoInitCom() {
    be<s32>* sound = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, "SOUND" );
    if (sound == nullptr) {
        return;
    }
    u32 se;
    switch ((u32)(s32)*sound) {
    case 1:
        switch (getShapeType()) {
        case 1:
            se = 0x490C;
            break;
        case 2:
            se = 0x4910;
            break;
        default:
            se = 0x4908;
            break;
        }
        break;
    case 2:
        switch (getShapeType()) {
        case 1:
            se = 0x490D;
            break;
        case 2:
            se = 0x4911;
            break;
        default:
            se = 0x4909;
            break;
        }
        break;
    default:
        return;
    }
    mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}

/* 022F3664 */
BOOL daNpc_Uk_c::demoProc() {
    BOOL ret = FALSE;
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        switch ((u32)mEventAction) {
        case 0: /* WAIT */
        case 1: /* TALK */
        case 9: /* LOOK_P */
            speedF = 0.0f;
            setAnm(0, 0);
            break;
        case 2: /* TALK2 */
            speedF = 0.0f;
            setAnm(2, 0);
            break;
        case 3: /* HOME */
            current.angle.y = home.angle.y;
            old.pos.copy(home.pos);
            current.pos.copy(home.pos);
            speedF = 0.0f;
            break;
        case 4: /* RUN */
        case 5: { /* RUN3 */
            if (mCurrAnmIdx == 4) {
                speedF = mRunSpeed;
            } else {
                setAnm(4, 0);
            }
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "Pos" , 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x7F4, a_xyz) */
                JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x7F4, "a_xyz");
            mDemoStartPos.copy(current.pos);
            wcopy3(gabi::ea(&mDemoTargetPos), a_xyz);
            u32 a_timer = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "Timer" , 3));
            if (a_timer != 0) {
                s16 timer = gabi::at<UkEventInteger>(a_timer)->mLowHalf; /* (s16)*a_timer */
                mDemoTimer = timer;
                if (getShapeType() == 1 && timer == 0x69) {
                    mDemoTimer = 0x9B;
                }
            } else {
                mDemoTimer = -1;
            }
            break;
        }
        case 13: { /* TURN */
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "Pos" , 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x808, a_xyz) */
                JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x808, "a_xyz");
            mDemoStartPos.copy(current.pos);
            wcopy3(gabi::ea(&mDemoTargetPos), a_xyz);
            mDemoTargetAngle = cLib_targetAngleY(&mDemoStartPos, &mDemoTargetPos);
            setAnm(0, 0);
            break;
        }
        case 6: /* HIND */
            setFlag(FLAG_HIDDEN);
            actor_status &= ~0x20u;
            speedF = 0.0f;
            mReturnToState = STATE_HIDDEN;
            break;
        case 7: /* DISP: mCurrActionFunc = &visit_action, started without ending the current action */
            mCurrActionFunc.d = 0;
            mCurrActionFunc.i = -1;
            mCurrActionFunc.f = UK_VISIT_ACTION;
            mActionStatus = 0;
            (this->*mCurrActionFunc)(nullptr);
            actor_status |= 0x20u;
            mReturnToState = mState;
            mState = STATE_DEMO;
            clrFlag(FLAG_HIDDEN);
            break;
        case 8: { /* SPEAK */
            speedF = 0.0f;
            talkInit();
            setFlag(FLAG_SET_TALK_PARTNER);
            u32 a_intP = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "MsgNo" , 3));
            if (a_intP == 0) /* JUT_ASSERT(0x823, a_intP) */
                JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x823, "a_intP");
            mMsgNo = *gabi::at<be<u32>>(a_intP);
            switch (getShapeType()) {
            case 0:
                setAnm(0, 0);
                break;
            case 1:
                setAnm(1, 0);
                break;
            default:
                setAnm(2, 0);
                break;
            }
            ret = TRUE;
            break;
        }
        case 10: /* WARNING */
            speedF = 0.0f;
            setFlag(FLAG_WARNING);
            setAnm(5, 1);
            break;
        case 11: /* JUMP */
        case 12: { /* JUMP2 */
            speed.y = 28.0f;
            setFlag(0x202);
            speedF = 12.0f;
            setAnm(9, 0);
            u32 se;
            switch (getShapeType()) {
            case 1:
                se = 0x490C;
                break;
            case 2:
                se = 0x4910;
                break;
            default:
                se = 0x4908;
                break;
            }
            mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            break;
        }
        case 14: { /* WARP */
            speedF = 0.0f;
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "Pos" , 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x84E, a_xyz) */
                JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x84E, "a_xyz");
            wcopy3(gabi::ea(&current.pos), a_xyz);
            wcopy3(gabi::ea(&old.pos), a_xyz);
            u32 a_intP = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, "Angle" , 3));
            if (a_intP == 0) /* JUT_ASSERT(0x854, a_intP) */
                JUT_ASSERT_fail("d_a_npc_uk.cpp", 0x854, "a_intP");
            current.angle.y = (s16)*gabi::at<be<u32>>(a_intP);
            shape_angle.y = (s16)*gabi::at<be<u32>>(a_intP);
            ret = TRUE;
            break;
        }
        }
    }
    switch ((u32)mEventAction) {
    case 1:
    case 2:
        setFlag(FLAG_NO_HEAD_TRACK);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 4:
    case 5: { /* RUN, RUN3 */
        gabi::Local<be<s16>> angle;  /* sp+0x18 */
        gabi::Local<cXyz> pos;       /* sp+0x1C */
        gabi::Local<cXyz> target;    /* sp+0x28 */
        gabi::Local<be<f32>> dist;   /* sp+0x40 */
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        target->x = mDemoTargetPos.x;
        target->y = mDemoTargetPos.y;
        target->z = mDemoTargetPos.z;
        dNpc_calc_DisXZ_AngY(pos, target, dist, angle);
        if (*dist < mRunSpeed || mMk.chkPointPass(&mDemoStartPos, &mDemoTargetPos, &current.pos)) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            if (mEventAction == 5) {
                current.pos.x = mDemoTargetPos.x;
                speedF = 0.0f;
                current.pos.z = mDemoTargetPos.z;
            }
            return TRUE;
        }
        cLib_addCalcAngleS(&current.angle.y, *angle, 4, 0x2000, 0x400);
        cLib_chaseF(&speedF, mRunSpeed, 1.1f);
        s16 timer = mDemoTimer;
        if (timer < 0) {
            return TRUE;
        }
        if (timer > 0) {
            timer = (s16)(timer - 1);
            mDemoTimer = timer;
            if (timer != 0) {
                return TRUE;
            }
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
    case 8: { /* SPEAK */
        u32 st = talk();
        if (st == 0x12 || st == 0xFE) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        setFlag(FLAG_NO_HEAD_TRACK);
        if (ret) {
            return TRUE;
        }
        return isMorf();
    }
    case 9: /* LOOK_P */
        setFlag(FLAG_LOOK_PARTNER);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 10: /* WARNING */
        if (chkFlag(FLAG_WARNING)) {
            if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
                clrFlag(FLAG_WARNING);
            }
            return TRUE;
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 11: /* JUMP */
    case 12: /* JUMP2 */
        if (chkFlag(FLAG_AIRBORNE)) {
            return TRUE;
        }
        setAnm(10, 0);
        speedF = 0.0f;
        if (chkFlag(FLAG_WARNING)) {
            clrFlag(FLAG_WARNING);
            gabi::Local<cXyz> shockPos;
            shockPos->x = 0.0f;
            shockPos->y = 1.0f;
            shockPos->z = 0.0f;
            dComIfGp_getVibration_StartShock(4, -0x21, shockPos);
            if (mEventAction == 12) {
                /* static cXyz scale(0.6f, 0.6f, 0.6f) (function-local, initialised on first use) */
                if (particleScaleInitialized == 0) {
                    particleScaleInitialized = 1;
                    particleScale.x = 0.6f;
                    particleScale.z = 0.6f;
                    particleScale.y = 0.6f;
                }
                JPABaseEmitter* emitter = dComIfGp_particle_set(0x23, &current.pos, &current.angle, &particleScale);
                if (emitter != nullptr) {
                    emitter->setSpread(1.0f);
                    emitter->setMaxFrame(1);
                    emitter->setRate(18.0f);
                    return TRUE;
                }
            }
            return TRUE;
        }
        if (mAnmEnded != 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return TRUE;
    case 13: /* TURN */
        cLib_addCalcAngleS(&current.angle.y, mDemoTargetAngle, 1, 0x800, 0x800);
        if (current.angle.y == mDemoTargetAngle) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return TRUE;
    default: /* WAIT, HOME, HIND, DISP, WARP */
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
}

/* 022F5DB0 */
BOOL daNpc_Uk_c::wait_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        mState = STATE_WAIT;
        setAnmStatus();
        mActionStatus = mActionStatus + 1; /* ACTION_ONGOING */
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case STATE_WAIT:
            temp = wait01();
            break;
        case STATE_TALK:
            temp = talk01();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}

/* 022F6360 */
BOOL daNpc_Uk_c::seek_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(getFoundFlag())) {
            mState = STATE_RUNAWAY;
            mMk.init(0x50, 0x12C);
            mMk.setRndPathPos(this, &mPath);
            setAnmStatus();
        } else {
            mState = (s8)(getShapeType() + 6);
            setAnmStatus();
        }
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case STATE_TALK:
            temp = talk02();
            break;
        case STATE_DEMO:
            temp = demo01();
            break;
        case STATE_EVENT:
            temp = demo02();
            break;
        case STATE_HIDDEN:
            temp = 0;
            uk_setAction(this, UK_HIND_ACTION); /* hind_action */
            break;
        case STATE_WARNING_B:
            temp = warningB();
            break;
        case STATE_WARNING_C:
            temp = warningC();
            break;
        case STATE_WARNING_D:
            temp = warningD();
            break;
        case STATE_FOUND:
            temp = found();
            break;
        case STATE_RUNAWAY:
            temp = runaway();
            break;
        case STATE_JUMP:
            temp = jump();
            break;
        case STATE_JITANDA01:
            temp = jitanda01();
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}

/* 022F5E90 */
BOOL daNpc_Uk_c::hind_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        clrFlag(0x60);
        s8 type = mType;
        if (type == 2) {
            if (chkGameStart()) {
                uk_setAction(this, UK_SEEK_ACTION); /* seek_action */
                clrFlag(FLAG_HIDDEN);
                actor_status |= 0x20u;
            }
            type = mType;
        }
        if ((type == 1 || type == 2) && checkDemoStart()) {
            u32 temp = demoProc();
            lookBack();
            setAttention(temp);
        }
    }
    return TRUE;
}

/* 022F6058 */
BOOL daNpc_Uk_c::visit_action(void*) {
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x40)) {
            mState = STATE_JITANDA02;
            setAnm(6, 0);
            mVisitMode = 0;
            mStts.SetWeight(0xFE);
        } else {
            mState = STATE_VISIT;
            setAnmStatus();
            mVisitMode = 0;
            mStts.SetWeight(0x64);
        }
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case STATE_TALK:
            temp = talk01();
            break;
        case STATE_VISIT:
            temp = visit01();
            break;
        case STATE_DEMO:
            temp = demo01();
            break;
        case STATE_EVENT:
            temp = demo02();
            break;
        case STATE_HIDDEN:
            temp = 0;
            uk_setAction(this, UK_HIND_ACTION); /* hind_action */
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}

/* 022F2520 */
BOOL daNpc_Uk_c::_draw() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pHeadData = mpHeadModel->getModelData();
    if (chkFlag(FLAG_HIDDEN)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    if (getShapeType() == 1) {
        mDoExt_btpAnm_entry(&mBtpAnm, pHeadData, mBlinkFrame);
    }
    mpMorf->entryDL();
    Mtx34* headMtx = pModel->getAnmMtx(m_jnt.mHeadJntNum);
    mtx_wcopy(&mpHeadModel->mBaseTransformMtx, headMtx); /* mpHeadModel->setBaseTRMtx */
    mDoExt_modelUpdateDL(mpHeadModel, 0);
    u32 shape = getShapeType();
    if (shape == 1) {
        reinterpret_cast<UkModelDataView*>(pHeadData)->mpTexNoAnimator = 0; /* mBtpAnm.remove(pHeadData) */
        shape = getShapeType();
    }
    // HD: no dComIfGd_setShadow
    if (shape <= 2) {
        dSnap_RegistFig(0x61 /* DSNAP_TYPE_NPC_UK */, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}

/* 022F1F98 */
BOOL daNpc_Uk_c::_execute() {
    m_jnt.setParam(0, 0x1C84, 0, -0x1C84, 0x1F40, 0x2328, -0x7D0, -0x2328, 0x3E8);
    playTexPatternAnm();
    switch ((s32)mCurrAnmIdx) {
    case 3: {
        f32 rate = speedF * 0.5f;
        if (rate < 1.0f) {
            rate = 1.0f;
        } else if (rate - 1.5f >= 0.0f) {
            rate = 1.5f;
        }
        mpMorf->setPlaySpeed(rate);
        break;
    }
    case 4: {
        f32 rate = speedF * 0.2f;
        if (rate < 1.0f) {
            rate = 1.0f;
        } else if (rate - 1.5f >= 0.0f) {
            rate = 1.5f;
        }
        mpMorf->setPlaySpeed(rate);
        break;
    }
    }
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((s32)mCurrAnmIdx) {
        case 6:
            if (cM_rnd() < 0.4f) {
                setAnm(7, 0);
            }
            break;
        case 7:
            setAnm(6, 0);
            break;
        }
    }
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    if (!chkFlag(FLAG_KEEP_SHAPE_ANGLE)) {
        shape_angle.y = current.angle.y;
    }
    clrFlag(FLAG_KEEP_SHAPE_ANGLE);
    mMtrlSndId = 0;
    if (!chkFlag(FLAG_HIDDEN)) {
        if (chkFlag(FLAG_PUSHED)) {
            f32 y = gabi::fadds_ppc(speed.y, gravity);
            speed.y = y;
            if (y < maxFallSpeed) {
                speed.y = maxFallSpeed;
            }
            fopAcM_posMove(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        } else {
            fopAcM_posMoveF(this, &mStts.m_cc_move);
        }
        mObjAcch.CrrPos(dComIfG_Bgsp());
        void* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* mObjAcch.m_gnd */
        if (!mObjAcch.ChkGroundHit()) {
            setFlag(FLAG_AIRBORNE);
        } else {
            clrFlag(FLAG_AIRBORNE);
            mMtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
        }
        tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
        tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), gnd); /* tevStr.mEnvrIdxOverride */
    } else {
        setFlag(FLAG_AIRBORNE);
    }
    clrFlag(FLAG_PUSHED);
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mtx_wcopy(&pModel->mBaseTransformMtx, mDoMtx_stack_c::get()); /* pModel->setBaseTRMtx */
    mpMorf->calc();
    if (chkFlag(FLAG_HIDDEN)) {
        mCyl.OffCoSPrmBit(1);
    } else {
        mCyl.OnCoSPrmBit(1);
        setCollision();
    }
    return TRUE;
}

/* 022F1CC4 */
BOOL daNpc_Uk_c::_delete() {
    dComIfG_resDelete(&mPhs, "Uk" );
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022F105C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Uk_c*)i_this)->CreateHeap();
}

/* 022F193C */
cPhs_State daNpc_Uk_c::_create() {
    /* fopAcM_ct(this, daNpc_Uk_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = UK_VTBL;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            __ct(&mObjAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10023010);
            uk_constructJntCtrl(&m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "Uk" );
    if (state == cPhs_COMPLEATE_e) {
        if (fpcM_GetName(this) != fpcNm_NPC_UK_e) {
            return cPhs_ERROR_e;
        }
        switch (getType()) {
        case 1:
            mType = 1;
            switch (getShapeType()) {
            case 0:
                mWalkSpeed = 2.5f;
                mRunSpeed = 10.0f;
                break;
            case 1:
                mWalkSpeed = 2.3f;
                mRunSpeed = 8.5f;
                break;
            case 2:
                mWalkSpeed = 2.0f;
                mRunSpeed = 7.0f;
                break;
            }
            break;
        case 2:
            mType = 2;
            argument = (s8)(getShapeType() + 5);
            mRunSpeed = 14.0f;
            break;
        default:
            mType = 0;
            break;
        }
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xB7B0)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        tevStr.mRoomNo = current.roomNo;
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(this, -35.0f, -10.0f, -35.0f, 35.0f, 100.0f, 35.0f);
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}

/* 022F0CA0 */
BOOL daNpc_Uk_c::CreateHeap() {
    u32 shape = getShapeType();
    J3DModelData* modelData;
    if (shape == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes("Uk", dRes_INDEX_UK_BDL_UK_e);
    } else {
        /* static const int table_bmt[] = {-1, 0x1C, 0x1D} (built on the stack) */
        const s32 table_bmt[3] = {-1, dRes_INDEX_UK_BMT_UK_C_e, dRes_INDEX_UK_BMT_UK_D_e};
        if (shape >= 3) {
            JUT_ASSERT_fail("d_a_npc_uk.cpp", 0xAF7, "shape_type < (sizeof(table_bmt) / sizeof(table_bmt[0]))");
            if (shape >= 3) return FALSE;
        }
        modelData = (J3DModelData*)dComIfG_getObjectRes("Uk", table_bmt[shape]);
    }
    if (modelData == nullptr) {
        return FALSE;
    }
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Uk", dRes_INDEX_UK_BCK_UK_WAIT_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x15021222);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "head" );
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(0xB19, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0xB19, "m_jnt.getHeadJntNum() >= 0");
    s8 backbone = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "backbone" );
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(0xB1E, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0xB1E, "m_jnt.getBackboneJntNum() >= 0");
    u32 shape2 = getShapeType();
    if (shape2 >= 3) { /* JUT_ASSERT(0xB34, shape_type < ARRAY_SIZE(head_bdl_table)) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0xB34, "shape_type < (sizeof(head_bdl_table) / sizeof(head_bdl_table[0]))");
        if (shape2 >= 3) return FALSE;
    }
    J3DModelData* headModelData = (J3DModelData*)dComIfG_getObjectRes("Uk", head_bdl_table[shape2] /* head_bdl_table */);
    if (headModelData == nullptr) { /* JUT_ASSERT(0xB38, headModelData) */
        JUT_ASSERT_fail("d_a_npc_uk.cpp", 0xB38, "headModelData");
        return FALSE;
    }
    mpHeadModel = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
    if (mpHeadModel.get() == nullptr) {
        return FALSE;
    }
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Uk) */
            J3DModelData* md = mpMorf->getModel()->getModelData();
            md->getJointNodePointer(i)->setCallBack(nodeCallBack_Uk);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(60.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mPartnerId = 0xFFFFFFFF;
    maxFallSpeed = -90.0f;
    mObjAcch.SetGroundCheckOffset(100.0f);
    return TRUE;
}

/* 022F1CC0 */
static cPhs_State daNpc_Uk_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Uk_c*)i_this)->_create();
}

/* 022F1D10 */
static BOOL daNpc_Uk_Delete(daNpc_Uk_c* i_this) {
    return i_this->_delete();
}

/* 022F251C */
static BOOL daNpc_Uk_Execute(daNpc_Uk_c* i_this) {
    return i_this->_execute();
}

/* 022F26AC */
static BOOL daNpc_Uk_Draw(daNpc_Uk_c* i_this) {
    return i_this->_draw();
}

/* 022F26B0 */
static BOOL daNpc_Uk_IsDelete(daNpc_Uk_c*) {
    return TRUE;
}

// HD: additional initialization and virtual destructor entrypoints.

/* 022F6764 */
static void __sinit_d_a_npc_uk_cpp() {
    sinit_header_statics(0x10468924, 0x101C69A0);
}

/* 022F67F8 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022F680C */
static void daNpc_Uk_c_dt(daNpc_Uk_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mObjAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022F68A8 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue (tools/verify/autoglue.py): unit bindings and data
ext 02543F10 u32 uk_getEventIdx(dEvent_manager_c* manager, const char* name, u8 evNo)
ext 0253E9B0 void uk_setEventName(dEvt_info_c* info, const char* name)
str 1002309C in 022F0B74
str 100230BC in 022F0B74
str 100230A0 in 022F0B74
str 100230DC in 022F0CA0
str 10023130 in 022F0CA0
str 100230E8 in 022F0CA0
str 100230E0 in 022F0CA0
str 10023140 in 022F0CA0
str 1002315C in 022F0CA0
str 10023168 in 022F0CA0
str 10023188 in 022F0CA0
str 10023120 in 022F0CA0
str 100231FC in 022F193C
str 100231FF in 022F1CC4
str 10023208 in 022F1DE4
str 10023238 in 022F2E18
str 1002322C in 022F2E18
str 10023248 in 022F325C
str 100232F8 in 022F34E0
str 10023354 in 022F40EC
str 10023350 in 022F40EC
str 10023314 in 022F3664
str 10023340 in 022F3664
str 10023318 in 022F3664
str 10023320 in 022F3664
str 10023328 in 022F3664
str 10023338 in 022F3664
str 10023330 in 022F3664
str 10023378 in 022F43DC
str 1002338C in 022F4F28
str 10023380 in 022F4F28
str 100233A4 in 022F5088
str 100233B0 in 022F5088
str 1002339C in 022F5088
str 100233C4 in 022F554C
str 100233D4 in 022F55E8
str 100233E4 in 022F5684
str 100233F0 in 022F5684
data l_bck_ix_tbl 10023060
data l_btp_ix_tbl 10022FF4
data head_bdl_table 101C6954
data l_msg_anm_tbl 10023220
data action_table 101C6960
str 10023284
str 1002328C
str 10023294
str 1002329C
str 100232A4
str 100232A8
str 100232B0
str 100232B8
str 100232C0
str 100232C8
str 100232D0
str 100232D8
str 100232E0
str 100232E8
str 100232F0
data staff_name_table 10023260
str 1002326C
str 10023274
str 1002327C
str 10023254 in 022F335C
str 10023258 in 022F335C
str 1002325C in 022F335C
data l_msgId 10468920 nocheck
data l_messageManager 101F4B5C nocheck
data l_clipperFar 1048D04C nocheck
data particleScaleInitialized 1046894C nocheck
data particleScale 10468940 nocheck
ext 0259DAA0 void uk_constructJntCtrl(dNpc_JntCtrl_c* jnt)
ext 025D7874 BOOL fopAcM_orderChangeEventId_raw(fopAc_ac_c* a, u32 ev, u16 flag, u16 hind)
ext 025D7A58 BOOL fopAcM_orderOtherEventId_raw(fopAc_ac_c* a, u32 ev, u8 mapToolId, u16 p3, u16 prio, u16 flag)
ext 025D7C6C fopAc_ac_c* uk_fopAcM_getTalkEventPartner(fopAc_ac_c* a)
ext 025D9F38 fopAc_ac_c* uk_fopAcM_searchFromName(const char* name, u32 mask, u32 prm)
ext 025E1988 void mDoAud_seStart_1(u32 id)
ext 025F795C u16 msgMgr_getStatus(u32 m)
ext 025F74D0 void msgMgr_setStatus(u32 m, u32 st)
ext 025F7DB0 u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos)
ext 0259E6D0 BOOL dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 024EF130 s8 dBgS_GetRoomId(dBgS* bgs, void* poly)
ext 024EEEB8 u8 dBgS_GetPolyColor(dBgS* bgs, void* poly)
ext 024EECAC u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly)
ext 024EDFCC bool dAttention_LockonTruth(u32 a)
ext 024EC8D0 fopAc_ac_c* dAttention_LockonTarget(u32 a, s32 i)
ext 024EE464 fopAc_ac_c* dAttention_ActionTarget(u32 a, s32 i)
ext 0229B54C BOOL daNpc_Mk_Static_c::chkGameSet()
ext 0229AD60 void daNpc_Mk_Static_c::init(u8 a, u16 b)
ext 0229AB78 void daNpc_Mk_Static_c::aroundWalk(fopAc_ac_c* a, fopAc_ac_c* b, u8 c)
ext 0229ACC0 f32 daNpc_Mk_Static_c::getSpeedF(f32 a, f32 b)
ext 0229B164 u32 Mk_runAwayProc(daNpc_Mk_Static_c* m, fopAc_ac_c* a, MkPath_l* p, void* cyl, be<s16>* ang)
ext 0229B5F0 void daNpc_Mk_Static_c::setRndPathPos(fopAc_ac_c* a, MkPath_l* p)
ext 0229B768 BOOL daNpc_Mk_Static_c::chkPointPass(cXyz* a, cXyz* b, cXyz* c)
*/
