/**
 * d_a_npc_p2_exec.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): createInit, setAnm, _execute, attention, lookBack.
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2_exec.h"

// HD: animation data and morph values are refreshed in place; the harness checks these initializers.
static s8 l_anm_tbl[3][24] = {
    {-1, 24, -1, -1, 1, 2, -1, -1, 3, 4, 4, 5, 6, 26, -1, -1, -1, -1, -1, -1, -1, -1, 25, 27},
    {-1, 7, 8, 9, 1, 2, 18, 19, 3, 4, 4, 5, 6, 12, 11, 10, 13, 14, 15, 16, 17, -1, -1, -1},
    {-1, 20, -1, -1, 1, 2, -1, -1, 3, 4, 4, 5, 6, 22, 23, -1, -1, -1, -1, -1, -1, 21, -1, -1}
};
static u32 l_play_mode[24] = {4294967295, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 2, 0, 0, 0, 2, 0};
static f32 l_morf[24] = {0.0f, 14.0f, 8.0f, 8.0f, 4.0f, 0.0f, 4.0f, 8.0f, 6.0f, 6.0f, 2.0f, 20.0f, 20.0f, 8.0f, 12.0f, 8.0f, 4.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 15.0f, 8.0f};
static f32 l_play_speed[24] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static s32 l_bck_idx[28] = {29, 30, 28, 7, 8, 26, 27, 15, 24, 25, 19, 20, 21, 22, 16, 23, 17, 14, 13, 18, 11, 12, 9, 10, 34, 32, 33, 31};
static const char* l_name_tbl[3] = {"P2a", "P2b", "P2c"};

/* HD: strcmp(dComIfGp_getStartStageName() (play + 0x5134), str) == 0 through two sead::SafeString
 * temporaries (the first one's assureTermination is called twice through the vtable) */
/* inlined into daNpc_P2_c::createInit */
static bool p2_isStartStage(const char* stage) {
    SafeString a;
    SafeString b;
    a.__vtbl = P2_SAFESTRING_VTBL;
    a.mStringTop = gabi::guest_addr(stage);
    P2StartStageView* play = reinterpret_cast<P2StartStageView*>(dComIfGp_get());
    b.mStringTop = gabi::ea(play->mStartStageName);
    b.__vtbl = P2_SAFESTRING_VTBL;
    gabi::call_ptr(P2_safeVtable(a.__vtbl)->assureTermination, &a);
    gabi::call_ptr(P2_safeVtable(a.__vtbl)->assureTermination, &a);
    u32 s1 = a.mStringTop;
    gabi::call_ptr(P2_safeVtable(b.__vtbl)->assureTermination, &b);
    u32 s2 = b.mStringTop;
    if (s1 == s2) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = P2_stringBytes(s1)[i];
        u8 c2 = P2_stringBytes(s2)[i];
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}

/* 022B6364 */
void daNpc_P2_c::setAnm() {
    /* l_morf (0x101C2D50, .data) is refreshed from the HIO on every call */
    for (int i = 0; i < 24; i++) {
        f32 v = p2_child(mType)->mMorf[i];
        l_morf[i] = v;
    }
    if ((u32)(s32)mCurAnm != (u32)(s32)mAnm) {
        /* l_anm_tbl (0x101C2CA8): per type 0x18 animation resources by animation */
        s8 res = l_anm_tbl[(u32)mType][(s32)mAnm];
        if (res != -1) {
            f32 evSpeed = mEventCut.mSpeed;
            s32 anm = mAnm;
            mAnmRes = res;
            mPrevFrame = 0.0f;
            f32 speed = l_play_speed[anm]; /* l_play_speed */
            if (evSpeed != 0.0f) {
                u32 actIdx = (u32)(s32)mEventCut.mCurActIdx;
                if (actIdx == 2 || actIdx == 4) { /* RUN_WAIT / JUMP_TO_LIFT */
                    speed = evSpeed * 0.25f;
                }
            }
            /* l_play_mode (0x101C2CF0), l_morf, l_bck_idx (0x1001FC6C) */
            P2_setNpcAnm(mpMorf, l_play_mode[anm], l_morf[anm], speed,
                           l_bck_idx[(s32)mAnmRes], -1, "P2");
        }
    }
    if (mpMorf->getFrame() == 1.0f && mAnmRes == 0x12) {
        p2_monsSeStart(this, 0x4897);
    }
    mCurAnm = mAnm;
    if (p2_isStop(mpMorf)) {
        if (mAnm == 0x13) {
            mAnm = 3;
        } else if (mAnm == 7) {
            mAnm = 1;
        }
    }
    if (mType == TYPE_P2A_e) {
        if (p2_isStop(mpMorf) && mAnm == 0x17) {
            mAnm = 0xD;
        }
    }
}

/* 022BBD20 */
void daNpc_P2_c::setAttention() {
    f32 morf = mpMorf->mCurMorf;
    bool morfing = morf < 1.0f;
    if (mbNoAttention == 0 || morfing) {
        f32 ey = mEyePos.y;
        f32 hz = mHeadPos.z;
        eyePos.y = ey;
        f32 hy = mHeadPos.y;
        f32 ez = mEyePos.z;
        f32 ex = mEyePos.x;
        eyePos.x = ex;
        eyePos.z = ez;
        f32 offs = p2_child(mType)->mNpc.mAttnYOffset;
        f32 hx = mHeadPos.x;
        f32 y = hy + offs;
        attention_info.position.z = hz;
        attention_info.position.x = hx;
        attention_info.position.y = y;
    }
    mbNoAttention = 0;
}

/* 022BBBD0 */
BOOL daNpc_P2_c::chkAttention() {
    f32 dist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    u8 type = mType;
    s16 yaw = (s16)(current.angle.y + m_jnt.mAngles[0][1] + m_jnt.mAngles[1][1]);
    s16 maxAngle = p2_child(type)->mNpc.mMaxAttnAngleY;
    if (type == TYPE_P2B_e) {
        if (mbBigCyl != 0 || mMode == 0x15) {
            if (dist < p2_child(1)->mNpc.mMaxAttnDistXZ) return TRUE;
        }
    }
    s32 a = yaw < 0 ? -(s32)yaw : (s32)yaw;
    if (maxAngle > a) {
        if (dist < p2_child(type)->mNpc.mMaxAttnDistXZ) {
            if (p2_event_getMode() != 0) return TRUE;
        }
    }
    dAttention_c* att = dComIfGp_getAttention();
    if (P2_lockonTruth(att)) { /* LockonTruth */
        return P2_actionTarget(att, 0) == this; /* ActionTarget(0) */
    }
    return P2_lockonTarget(att, 0) == this; /* LockonTarget(0) */
}

/* 022BBDA4 */
void daNpc_P2_c::lookBack() {
    f32 px = 0.0f, ey = 0.0f, pz = 0.0f;
    cXyz* target = nullptr;
    s16 yaw = current.angle.y;
    u8 headOnly = mEventCut.mbNoTurn;
    cXyz tgt;
    if (mbEvtAttention != 0 || mEventCut.mbAttention != 0) {
        cXyz base;
        cXyz res;
        base.x = (f32)mEventCut.mPos.x;
        base.y = (f32)mEventCut.mPos.y;
        base.z = (f32)mEventCut.mPos.z;
        m_jnt.mbTrn = 1;
        res = base + mLookOffs;
        P2_copyPos(&tgt, &res);
        ey = eyePos.y;
        pz = current.pos.z;
        target = &tgt;
        px = current.pos.x;
    } else {
        s8 mode = mMode;
        if (mode == 0xE) {
            m_jnt.mbTrn = 1;
        }
        if ((u32)(s32)mode == 2) {
            m_jnt.mbTrn = 1;
            cXyz res;
            dNpc_playerEyePos(&res, p2_child(mType)->mNpc.m04);
            px = current.pos.x;
            pz = current.pos.z;
            P2_copyPos(&tgt, &res);
            ey = eyePos.y;
            target = &tgt;
        } else if ((u32)(s32)mode >= 9 && (u32)(s32)mode <= 10) {
            P2_copyPos(&tgt, &mLookPos);
            px = current.pos.x;
            m_jnt.mbTrn = 1;
            ey = eyePos.y;
            pz = current.pos.z;
            target = &tgt;
        } else if (mbAttention != 0) {
            cXyz res;
            dNpc_playerEyePos(&res, p2_child(mType)->mNpc.m04);
            ey = eyePos.y;
            P2_copyPos(&tgt, &res);
            pz = current.pos.z;
            target = &tgt;
            px = current.pos.x;
        }
    }
    if (mType == TYPE_P2A_e) {
        s8 anm = mAnm;
        if (anm == 0x17) {
            /* Zuko looks through the telescope at the ship (fopAcM_SearchByName(0x1AA)) */
            gabi::Local<be<s16>> name;
            *name = 0x1AA;
            fopAc_ac_c* actor = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
            if (actor != nullptr) {
                s16 ang = fopAcM_searchActorAngleY(this, actor);
                cLib_addCalcAngleS2(&current.angle.y, ang, 4, 0x800);
                s16 vel = mLookBackY;
                m_jnt.mbHeadLock = 1;
                cXyz eye;
                eye.x = (f32)current.pos.x;
                eye.z = (f32)current.pos.z;
                eye.y = (f32)eyePos.y;
                m_jnt.mbBackBoneLock = 1;
                P2_lookAtTarget(&m_jnt, &current.angle.y, &actor->current.pos, &eye, yaw, vel, 0);
                return;
            }
        } else if (anm == 0x16) {
            m_jnt.mbTrn = 0;
            headOnly = 0;
            m_jnt.mbHeadLock = 1;
            m_jnt.mbBackBoneLock = 1;
        } else {
            m_jnt.mbHeadLock = 0;
            m_jnt.mbBackBoneLock = 0;
        }
    }
    if (mMode == 0x10) {
        headOnly = 0;
        m_jnt.mbTrn = 0;
    }
    if (mType == TYPE_P2B_e && mMode == 0xD && mSubType == 1) {
        auto& dest = *reinterpret_cast<P2PositionWords*>(&tgt);
        auto& source = *reinterpret_cast<P2PositionWords*>(&mGoalLookPos);
        u32 x = source.x, z = source.z;
        dest.x = x;
        dest.z = z;
        px = current.pos.x;
        u32 y = source.y;
        pz = current.pos.z;
        target = &tgt;
        dest.y = y;
        ey = eyePos.y;
        m_jnt.mbTrn = 1;
    } else if (m_jnt.mbTrn == 0) {
        mLookBackY = 0;
        cXyz eye;
        eye.x = px;
        eye.y = ey;
        eye.z = pz;
        P2_lookAtTarget(&m_jnt, &current.angle.y, target, &eye, yaw, 0, headOnly);
        return;
    }
    cLib_addCalcAngleS2(&mLookBackY, p2_child(mType)->mNpc.mMaxHeadTurnVel, 4, 0x800);
    s16 vel = mLookBackY;
    cXyz eye;
    eye.y = ey;
    eye.x = px;
    eye.z = pz;
    P2_lookAtTarget(&m_jnt, &current.angle.y, target, &eye, yaw, vel, headOnly);
}

/* 022BB228 */
BOOL daNpc_P2_c::_execute() {
    dNpc_HIO_p2* n = &p2_child(mType)->mNpc;
    m_jnt.setParam(n->mMaxBackboneX, n->mMaxBackboneY, n->mMinBackboneX, n->mMinBackboneY, n->mMaxHeadX, n->mMaxHeadY,
                   n->mMinHeadX, n->mMinHeadY, n->mMaxTurnStep);
    if (mType != TYPE_P2C_e) {
        playTexPatternAnm();
    }
    BOOL end;
    if (mObjAcch.ChkGroundHit()) {
        u32 snd = dComIfG_Bgsp()->GetMtrlSndId(&mObjAcch.m_gnd);
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        end = mpMorf->play(&eyePos, snd, (s8)reverb);
    } else {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        end = mpMorf->play(&eyePos, 0, (s8)reverb);
    }
    mMorfPlayEnd = (u8)end;
    if (mpMorf->getFrame() < mPrevFrame) {
        mMorfPlayEnd = 1; /* the animation looped */
    }
    mPrevFrame = mpMorf->getFrame();
    checkOrder();
    p2_pmf_call(this, &mAction, nullptr);
    if (p2_event_getMode() != 0 && !eventInfo.checkCommandTalk()) {
        if (!mEventCut.cutProc()) {
            mEventCut.mbAttention = 0;
            cutProc();
        } else {
            u32 actIdx = (u32)(s32)mEventCut.mCurActIdx;
            mbEvtAttention = 0;
            if (actIdx != 0xFFFFFFFF && (actIdx == 2 || actIdx == 4) && mEventCut.mSpeed != 0.0f) {
                if (!(mEventCut.mSpeed > 10.0f)) {
                    mAnm = 4;
                } else {
                    mAnm = 5;
                }
            } else {
                mAnm = 1;
            }
        }
    }
    if (p2_event_getMode() == 0) {
        mEventCut.mbAttention = 0;
        mEventCut.mbNoTurn = 0;
        mbEvtAttention = 0;
    }
    eventOrder();
    setAnm();
    setTexAnm();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = P2_getRoomId(dComIfG_Bgsp(), &mObjAcch.m_gnd);
    tevStr.mEnvrIdxOverride = P2_getPolyColor(dComIfG_Bgsp(), &mObjAcch.m_gnd);
    setMtx();
    mpMorf->calc();
    if (mbRopeHang) {
        J3DMtxBuffer_hd* matrices = mpMorf->getModel()->mpMtxBuffer;
        fopAc_ac_c* rope = mpRope;
        u16 flags = matrices->mFlags;
        // HD: the hand is joint 12; the matrix block is marked dirty after fetching it.
        Mtx34* handMtx = P2_jointMatrix(matrices, 12);
        matrices->mFlags = (u16)(flags | 0x10);
        P2_getPosFromMtx(handMtx, &mHandPos);
        cXyz pos;
        pos.x = (f32)mHandPos.x;
        pos.y = (f32)mHandPos.y;
        pos.z = (f32)mHandPos.z;
        P2_setActorHang(rope, &pos, (s32)shape_angle.y);
    }
    setCollision();
    if (mAnm == 4) {
        cXyz diff;
        diff = current.pos - old.pos;
        f32 d = std_sqrtf(PSVECSquareMag(&diff));
        f32 ratio = d / (REG_F(12, 6) + 10.0f);
        f32 scale = p2_HIO()->mRunSpeedScale;
        f32 v;
        if (ratio > 0.0f) {
            if (!(ratio < 1.0f)) ratio = 1.0f;
            v = ratio * scale;
        } else {
            v = 0.0f * scale;
        }
        f32 min = p2_HIO()->mMinRunSpeed;
        mpMorf->setPlaySpeed((v - min >= 0.0f) ? v : min);
    }
    return TRUE;
}

/* 022B65B0 */
void daNpc_P2_c::createInit() {
    if (mType == TYPE_P2B_e) {
        if (p2_isStartStage("Asoko")) {
            actor_status |= fopAcStts_UNK4000_e;
        }
    }
    mpMorf->calc();
    J3DModel* model = mpMorf->getModel();
    cullMtx = gabi::ea(model->getBaseTRMtx());
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u8 type = mType;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9;
    if (type == TYPE_P2B_e) {
        u8 sub = mSubType;
        if (sub == 0) {
            if (!p2_isEventBit(0x808) && p2_isEventBit(0x720)) {
                P2_copyPos(&current.pos, &p2_child(mType)->mGoalTalkPos);
            }
        } else if (sub == 1) {
            if (!p2_isEventBit(0xF02) && p2_isEventBit(0x1A04)) {
                P2_copyPos(&current.pos, &p2_child(mType)->mGoalPos2);
            }
        }
    }
    type = mType;
    mHomeAngle.x = (s16)current.angle.x;
    mHomeAngle.y = (s16)current.angle.y;
    mHomeAngle.z = (s16)current.angle.z;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    gravity = -9.0f;
    /* l_name_tbl (0x101C2E10): "P2a", "P2b", "P2c" */
    P2_setActorInfo(&mEventCut, l_name_tbl[(u32)type], this);
    if (mType == TYPE_P2B_e) {
        if (p2_isStartStage("Asoko")) {
            u8 sub = mSubType;
            if (sub == 0) {
                if (!p2_isEventBit(0x808)) {
                    p2_setAction(this, P2_intro_action);
                } else {
                    p2_setAction(this, P2_wait_action);
                }
            } else if (sub == 1) {
                if (!p2_isEventBit(0xF02)) {
                    p2_setAction(this, P2_intro_action);
                } else {
                    p2_setAction(this, P2_wait_action);
                }
            }
        } else {
            p2_setAction(this, P2_wait_action);
        }
    } else {
        p2_setAction(this, P2_wait_action);
    }
    P2_copyPos(&mHeadPos, &current.pos);
    P2_copyPos(&mEyePos, &current.pos);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(P2_sharedCyl()); /* l_cyl_src (shared NPC cylinder) */
    mCyl.SetStts(&mStts);
    mTexAnm = 1;
    mAnm = 1;
    mCurAnm = 0;
    setTexAnm();
    setAnm();
    u32 pid = parentActorID;
    if (pid != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = pid;
        fopAc_ac_c* ship = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
        if (ship != nullptr && fopAc_IsActor(ship) && ship != nullptr && fpcM_GetName(ship) == fpcNm_Obj_Pirateship_e) {
            if (mType == TYPE_P2A_e) {
                if (!p2_isEventBit(0x808)) {
                    m_jnt.mbHeadLock = 1;
                    m_jnt.mbBackBoneLock = 1;
                    mShipAngleOffs = (s16)(home.angle.y - ship->home.angle.y);
                } else if (mType == TYPE_P2C_e) {
                    mShipAngleOffs = (s16)(home.angle.y - ship->home.angle.y);
                }
            } else if (mType == TYPE_P2C_e) {
                mShipAngleOffs = (s16)(home.angle.y - ship->home.angle.y);
            }
        }
    }
    f32 r = cM_rndF(100.0f);
    mLookOffs.x = 0.0f;
    mLookOffs.y = 0.0f;
    mLookOffs.z = 0.0f;
    mMoccoTimer = (s32)(s16)gabi::ftoi(r + 200.0f);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_anm_tbl 101C2CA8
data l_play_mode 101C2CF0
data l_morf 101C2D50
data l_play_speed 101C2DB0
data l_bck_idx 1001FC6C
data l_name_tbl 101C2E10
str 1001FCF4 "P2a"
str 1001FCF8 "P2b"
str 1001FCFC "P2c"
str 1001FCEC "Asoko"
str 100200A0 "P2"
ext 0259D454 void P2_setNpcAnm(mDoExt_McaMorf*, u32, f32, f32, s32, s32, const char*)
ext 0259F7D4 void P2_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*)
ext 0259DED0 void P2_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8)
ext 024EF130 s8 P2_getRoomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 P2_getPolyColor(dBgS*, cBgS_PolyInfo*)
ext 02587C88 void P2_getPosFromMtx(Mtx34*, cXyz*)
ext 02174F44 void P2_setActorHang(fopAc_ac_c*, cXyz*, s32)
ext 024EDFCC u32 P2_lockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* P2_actionTarget(dAttention_c*, s32)
ext 024EE464 fopAc_ac_c* P2_lockonTarget(dAttention_c*, s32)
*/
