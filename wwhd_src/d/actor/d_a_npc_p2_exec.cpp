/**
 * d_a_npc_p2_exec.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): createInit, setAnm, _execute, attention, lookBack.
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arcName) */
static inline void p2_dNpc_setAnm(mDoExt_McaMorf* m, u32 loop, f32 morf, f32 speed, s32 idx, s32 snd, u32 arc) {
    gabi::call<BOOL>(0x0259D454, m, loop, morf, speed, idx, snd, arc);
}
/* 0259F7D4 dNpc_EventCut_c::setActorInfo(const char*, fopAc_ac_c*) */
static inline void p2_setActorInfo(dNpc_EventCut_c* c, u32 name, fopAc_ac_c* a) { gabi::call(0x0259F7D4, c, name, a); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, u8 headOnly) */
static inline void p2_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, void* target, cXyz* eye, s16 yrot, s16 vel, u8 head) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, head);
}

/* HD: strcmp(dComIfGp_getStartStageName() (play + 0x5134), str) == 0 through two sead::SafeString
 * temporaries (the first one's assureTermination is called twice through the vtable) */
static inline bool p2_isStartStage(u32 str) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = P2_SAFESTRING_VTBL;
    a->mStringTop = str;
    u32 play = dComIfGp_ea();
    b->mStringTop = play + 0x5134;
    b->__vtbl = P2_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}

/* 022B6364 */
void daNpc_P2_c::setAnm() {
    WWHD_FUNC(0x022B6364, void, this);
    /* l_morf (0x101C2D50, .data) is refreshed from the HIO on every call */
    for (int i = 0; i < 24; i++) {
        f32 v = p2_child(mType)->mMorf[i];
        gabi::store<f32>(0x101C2D50 + 4 * i, v);
    }
    if ((u32)(s32)mCurAnm != (u32)(s32)mAnm) {
        /* l_anm_tbl (0x101C2CA8): per type 0x18 animation resources by animation */
        s8 res = (s8)gabi::load<u8>(0x101C2CA8 + (u32)mType * 0x18 + (s32)mAnm);
        if (res != -1) {
            f32 evSpeed = mEventCut.mSpeed;
            s32 anm = mAnm;
            mAnmRes = res;
            mPrevFrame = 0.0f;
            f32 speed = gabi::load<f32>(0x101C2DB0 + 4 * anm); /* l_play_speed */
            if (evSpeed != 0.0f) {
                u32 actIdx = (u32)(s32)mEventCut.mCurActIdx;
                if (actIdx == 2 || actIdx == 4) { /* RUN_WAIT / JUMP_TO_LIFT */
                    speed = evSpeed * 0.25f;
                }
            }
            /* l_play_mode (0x101C2CF0), l_morf, l_bck_idx (0x1001FC6C) */
            p2_dNpc_setAnm(mpMorf, gabi::load<u32>(0x101C2CF0 + 4 * anm), gabi::load<f32>(0x101C2D50 + 4 * anm), speed,
                           gabi::load<s32>(0x1001FC6C + 4 * (s32)mAnmRes), -1, P2_ARC);
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
VERIFY(0x022B6364, &daNpc_P2_c::setAnm);

/* 022B65B0 */
void daNpc_P2_c::createInit() {
    WWHD_FUNC(0x022B65B0, void, this);
    if (mType == TYPE_P2B_e) {
        if (p2_isStartStage(0x1001FCEC /* "Asoko" */)) {
            actor_status |= 0x4000;
        }
    }
    mpMorf->calc();
    u32 model = gabi::ea(mpMorf->getModel());
    cullMtx = model != 0 ? model + 0xC8 : 0; /* getBaseTRMtx */
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u8 type = mType;
    u32 attn = gabi::ea(this) + 0x388; /* attention_info.distances[] */
    gabi::store<u8>(attn + 1, 0xA9);
    gabi::store<u8>(attn + 3, 0xA9);
    if (type == TYPE_P2B_e) {
        u8 sub = mSubType;
        if (sub == 0) {
            if (!p2_isEventBit(0x808) && p2_isEventBit(0x720)) {
                p2_copy12(gabi::ea(&current.pos), gabi::ea(&p2_child(mType)->mGoalTalkPos));
            }
        } else if (sub == 1) {
            if (!p2_isEventBit(0xF02) && p2_isEventBit(0x1A04)) {
                p2_copy12(gabi::ea(&current.pos), gabi::ea(&p2_child(mType)->mGoalPos2));
            }
        }
    }
    type = mType;
    gabi::store<u16>(gabi::ea(&mHomeAngle) + 0, gabi::load<u16>(gabi::ea(&current.angle) + 0));
    gabi::store<u16>(gabi::ea(&mHomeAngle) + 2, gabi::load<u16>(gabi::ea(&current.angle) + 2));
    gabi::store<u16>(gabi::ea(&mHomeAngle) + 4, gabi::load<u16>(gabi::ea(&current.angle) + 4));
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gravity = -9.0f;
    /* l_name_tbl (0x101C2E10): "P2a", "P2b", "P2c" */
    p2_setActorInfo(&mEventCut, gabi::load<u32>(0x101C2E10 + 4 * (u32)type), this);
    if (mType == TYPE_P2B_e) {
        if (p2_isStartStage(0x1001FCEC /* "Asoko" */)) {
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
    p2_copy12(gabi::ea(&mHeadPos), gabi::ea(&current.pos));
    p2_copy12(gabi::ea(&mEyePos), gabi::ea(&current.pos));
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190)); /* l_cyl_src (shared NPC cylinder) */
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
        if (ship != nullptr && fopAc_IsActor(ship) && ship != nullptr && fpcM_GetName(ship) == 0x39 /* Obj_Pirateship */) {
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
VERIFY(0x022B65B0, &daNpc_P2_c::createInit);

/* 022BB228 */
BOOL daNpc_P2_c::_execute() {
    WWHD_FUNC(0x022BB228, BOOL, this);
    dNpc_HIO_p2* n = &p2_child(mType)->mNpc;
    m_jnt.setParam(n->mMaxBackboneX, n->mMaxBackboneY, n->mMinBackboneX, n->mMinBackboneY, n->mMaxHeadX, n->mMaxHeadY,
                   n->mMinHeadX, n->mMinHeadY, n->mMaxTurnStep);
    if (mType != TYPE_P2C_e) {
        playTexPatternAnm();
    }
    BOOL end;
    if (mObjAcch.ChkGroundHit()) {
        u32 snd = gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), gabi::ea(this) + 0x5DC /* mObjAcch.m_gnd */);
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
    if (p2_event_getMode() != 0 && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !checkCommandTalk */) {
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
    fopAcM_posMoveF(this, (cXyz*)&mStts); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::store<s8>(gabi::ea(this) + 0x1C9, gabi::call<s8>(0x024EF130, dComIfG_Bgsp(), gabi::ea(this) + 0x5DC)); /* tevStr.mRoomNo = GetRoomId */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, gabi::call<u8>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(this) + 0x5DC)); /* GetPolyColor */
    setMtx();
    mpMorf->calc();
    if (mbRopeHang) {
        u32 blk = gabi::load<u32>(gabi::ea(mpMorf->getModel()) + 0x2C);
        fopAc_ac_c* rope = mpRope;
        u16 fl = gabi::load<u16>(blk + 4);
        u32 mtx = gabi::load<u32>(blk + 0x10) + 0xC * 0x30; /* getAnmMtx(12): the hand */
        gabi::store<u16>(blk + 4, (u16)(fl | 0x10));
        gabi::call(0x02587C88, mtx, &mHandPos); /* dLib_getPosFromMtx */
        gabi::Local<cXyz> pos;
        pos->x = (f32)mHandPos.x;
        pos->y = (f32)mHandPos.y;
        pos->z = (f32)mHandPos.z;
        gabi::call(0x02174F44, rope, pos.get(), (s32)shape_angle.y); /* the rope's setActorHang */
    }
    setCollision();
    if (mAnm == 4) {
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff.get(), &old.pos);
        f32 d = std_sqrtf(PSVECSquareMag(diff.get()));
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
VERIFY(0x022BB228, &daNpc_P2_c::_execute);

/* 022BBBD0 */
BOOL daNpc_P2_c::chkAttention() {
    WWHD_FUNC(0x022BBBD0, BOOL, this);
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
    u32 att = dComIfGp_ea() + PLAY_ATTENTION;
    if (gabi::call<u32>(0x024EDFCC, att)) { /* LockonTruth */
        return gabi::call<u32>(0x024EC8D0, att, 0) == gabi::ea(this); /* ActionTarget(0) */
    }
    return gabi::call<u32>(0x024EE464, att, 0) == gabi::ea(this); /* LockonTarget(0) */
}
VERIFY(0x022BBBD0, &daNpc_P2_c::chkAttention);

/* 022BBD20 */
void daNpc_P2_c::setAttention() {
    WWHD_FUNC(0x022BBD20, void, this);
    f32 morf = gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0);
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
        gabi::store<f32>(gabi::ea(this) + 0x398, hz); /* attention_info.position */
        gabi::store<f32>(gabi::ea(this) + 0x390, hx);
        gabi::store<f32>(gabi::ea(this) + 0x394, y);
    }
    mbNoAttention = 0;
}
VERIFY(0x022BBD20, &daNpc_P2_c::setAttention);

/* 022BBDA4 */
void daNpc_P2_c::lookBack() {
    WWHD_FUNC(0x022BBDA4, void, this);
    f32 px = 0.0f, ey = 0.0f, pz = 0.0f;
    cXyz* target = nullptr;
    s16 yaw = current.angle.y;
    u8 headOnly = mEventCut.mbNoTurn;
    gabi::Local<cXyz> tgt;
    if (mbEvtAttention != 0 || mEventCut.mbAttention != 0) {
        gabi::Local<cXyz> base;
        gabi::Local<cXyz> res;
        base->x = (f32)mEventCut.mPos.x;
        base->y = (f32)mEventCut.mPos.y;
        base->z = (f32)mEventCut.mPos.z;
        m_jnt.mbTrn = 1;
        cXyz_pl(base.get(), res.get(), &mLookOffs);
        p2_copy12(gabi::ea(tgt.get()), gabi::ea(res.get()));
        ey = eyePos.y;
        pz = current.pos.z;
        target = tgt.get();
        px = current.pos.x;
    } else {
        s8 mode = mMode;
        if (mode == 0xE) {
            m_jnt.mbTrn = 1;
        }
        if ((u32)(s32)mode == 2) {
            m_jnt.mbTrn = 1;
            gabi::Local<cXyz> res;
            dNpc_playerEyePos(res.get(), p2_child(mType)->mNpc.m04);
            px = current.pos.x;
            pz = current.pos.z;
            p2_copy12(gabi::ea(tgt.get()), gabi::ea(res.get()));
            ey = eyePos.y;
            target = tgt.get();
        } else if ((u32)(s32)mode >= 9 && (u32)(s32)mode <= 10) {
            p2_copy12(gabi::ea(tgt.get()), gabi::ea(&mLookPos));
            px = current.pos.x;
            m_jnt.mbTrn = 1;
            ey = eyePos.y;
            pz = current.pos.z;
            target = tgt.get();
        } else if (mbAttention != 0) {
            gabi::Local<cXyz> res;
            dNpc_playerEyePos(res.get(), p2_child(mType)->mNpc.m04);
            ey = eyePos.y;
            p2_copy12(gabi::ea(tgt.get()), gabi::ea(res.get()));
            pz = current.pos.z;
            target = tgt.get();
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
                gabi::Local<cXyz> eye;
                eye->x = (f32)current.pos.x;
                eye->z = (f32)current.pos.z;
                eye->y = (f32)eyePos.y;
                m_jnt.mbBackBoneLock = 1;
                p2_lookAtTarget(&m_jnt, &current.angle.y, &actor->current.pos, eye.get(), yaw, vel, 0);
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
        u32 t = gabi::ea(tgt.get());
        u32 s = gabi::ea(&mGoalLookPos);
        u32 a = gabi::load<u32>(s), c = gabi::load<u32>(s + 8);
        gabi::store<u32>(t, a);
        gabi::store<u32>(t + 8, c);
        px = current.pos.x;
        u32 b = gabi::load<u32>(s + 4);
        pz = current.pos.z;
        target = tgt.get();
        gabi::store<u32>(t + 4, b);
        ey = eyePos.y;
        m_jnt.mbTrn = 1;
    } else if (m_jnt.mbTrn == 0) {
        mLookBackY = 0;
        gabi::Local<cXyz> eye;
        eye->x = px;
        eye->y = ey;
        eye->z = pz;
        p2_lookAtTarget(&m_jnt, &current.angle.y, target, eye.get(), yaw, 0, headOnly);
        return;
    }
    cLib_addCalcAngleS2(&mLookBackY, p2_child(mType)->mNpc.mMaxHeadTurnVel, 4, 0x800);
    s16 vel = mLookBackY;
    gabi::Local<cXyz> eye;
    eye->y = ey;
    eye->x = px;
    eye->z = pz;
    p2_lookAtTarget(&m_jnt, &current.angle.y, target, eye.get(), yaw, vel, headOnly);
}
VERIFY(0x022BBDA4, &daNpc_P2_c::lookBack);
