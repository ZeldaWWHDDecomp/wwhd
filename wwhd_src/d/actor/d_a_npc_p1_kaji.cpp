/**
 * d_a_npc_p1_kaji.cpp (WWHD): daNpc_P1_c::kaji_anm, daNpc_P1_c::lookBack
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

/* daKaji_c (d_a_kaji.h): mpMorf at +0x3B8 (HD) */
static inline mDoExt_McaMorf* p1_kajiMorf(fopAc_ac_c* kaji) {
    return gabi::at<mDoExt_McaMorf>(gabi::load<u32>(gabi::ea(kaji) + 0x3B8));
}

/* daKaji_c::setAnm(int, f32) (inline). "Kaji" at 0x1001F874. The `frame >= 0.0f` test is
 * compiled as bge / !blt (branch if not less), so a NaN frame is stored too. */
static inline void p1_kajiSetAnm(fopAc_ac_c* kaji, s32 i_anm, f32 frame) {
    s32 bckIdx, basIdx;
    switch ((u32)i_anm) {
    case 0: bckIdx = 0xE; basIdx = 8; break; /* KJ_WAIT */
    case 1: bckIdx = 0xC; basIdx = 6; break; /* KJ_OMO */
    case 2: bckIdx = 0xD; basIdx = 7; break; /* KJ_TORI */
    case 3: bckIdx = 0xB; basIdx = 5; break; /* KJ_ANG */
    default: return;
    }
    J3DAnmTransform* bck = (J3DAnmTransform*)p1_getRes(0x1001F874, bckIdx);
    void* bas = p1_getRes(0x1001F874, basIdx);
    p1_kajiMorf(kaji)->setAnm(bck, 2 /* EMode_LOOP */, 0.0f, 1.0f, 0.0f, -1.0f, bas);
    if (!(frame < 0.0f)) {
        p1_kajiMorf(kaji)->setFrame(frame);
    }
}

/* 022B2830 */
BOOL daNpc_P1_c::kaji_anm() {
    WWHD_FUNC(0x022B2830, BOOL, this);
    if (m671 != 0) {
        if (mAnmNum == 0xA || mAnmNum == 0xB) {
            if (p1_checkEnd(mpMorf, 1.0f)) {
                setAnm(9, -1.0f);
                mKajiTimer = 300;
            } else if (mpMorf->checkFrame(30.0f)) {
                /* fopAcM_seStart(this, JA_SE_OBJ_PIRATE_WHEEL, 0); HD: no null checks on this */
                s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
                mDoAud_seStart(0x697E, &eyePos, 0, reverb);
            }
        } else {
            s16 t = mKajiTimer;
            mKajiTimer = (s16)(t - 1);
            if (t < 0) {
                if (p1_checkEnd(mpMorf, 1.0f)) {
                    if (!p1_checkAction(this, P1_talkAction)) {
                        if (cM_rndF(1.0f) > 0.5f) {
                            setAnm(0xA, -1.0f);
                        } else {
                            setAnm(0xB, -1.0f);
                        }
                    }
                }
            }
        }
        u32 kajiId = mKajiId;
        if (kajiId != 0xFFFFFFFF) {
            /* fopAcM_SearchByID(mKajiId) */
            gabi::Local<be<u32>> key;
            *key = kajiId;
            fopAc_ac_c* kaji = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
            if (fopAc_IsActor(kaji) && kaji != nullptr && fpcM_GetName(kaji) == fpcNm_Kaji_e) {
                s32 anm = mAnmNum;
                f32 frame = mpMorf->getFrame();
                p1_kajiSetAnm(kaji, anm - 9, frame);
            }
        }
        return TRUE;
    }
    if (mType == TYPE_P1B_e && mParam == 2) {
        if (!p1_checkAction(this, P1_talkAction)) {
            if (mAnmNum == 0xE && p1_checkEnd(mpMorf, 1.0f) && (m66C = m66C - 1) <= 0) {
                setAnm(0xF, -1.0f);
                if (cM_rndF(1.0f) > 0.5f) {
                    m66C = 1;
                } else {
                    m66C = 2;
                }
            } else if (mAnmNum == 0xF) {
                if (p1_checkEnd(mpMorf, 1.0f)) {
                    if ((m66C = m66C - 1) <= 0) {
                        setAnm(0xE, -1.0f);
                        m66C = 4;
                    }
                }
            }
        }
    }
    return FALSE;
}
VERIFY(0x022B2830, &daNpc_P1_c::kaji_anm);

/* 022B2E70 */
BOOL daNpc_P1_c::lookBack() {
    WWHD_FUNC(0x022B2E70, BOOL, this);
    BOOL o_retval = FALSE;
    /* (player->current.pos - current.pos).absXZ(): the x/z of the difference go through FPRs */
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B2C)); /* dComIfGp_getPlayer(0) */
    gabi::Local<cXyz> posdiff;
    cXyz_mi(&player->current.pos, posdiff.get(), &current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = posdiff->x;
    f32 dz = posdiff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz.get()));

    gabi::Local<cXyz> dstPos;
    gabi::Local<cXyz> eye;
    cXyz* dstPos_p;
    u8 look_at_target = 1;
    if (m671 == 1) {
        if (mAnmNum == 9 && p1_checkAction(this, P1_talkAction)) {
            dNpc_playerEyePos(eye.get(), 0.0f);
            p1_copy12(gabi::ea(dstPos.get()), gabi::ea(eye.get()));
            dstPos_p = dstPos.get();
        } else {
            dstPos_p = nullptr;
        }
    } else if (mbAttentionFlag != 0) {
        /* mEventCut6B0.getAttnPos() (+0x54), copied through FPRs */
        cXyz* attn = gabi::at<cXyz>(gabi::ea(&mEventCut6B0) + 0x54);
        dstPos->x = attn->x;
        dstPos->y = attn->y;
        dstPos->z = attn->z;
        dstPos_p = dstPos.get();
    } else if (dist < p1_child(mType)->mMaxTalkDist || p1_checkAction(this, P1_talkAction) ||
               p1_checkAction(this, P1_explainAction) || p1_checkAction(this, P1_speakAction) ||
               p1_checkAction(this, P1_p1c_speakAction)) {
        gabi::Local<cXyz> eye2;
        dNpc_playerEyePos(eye2.get(), 0.0f);
        p1_copy12(gabi::ea(dstPos.get()), gabi::ea(eye2.get()));
        dstPos_p = dstPos.get();
    } else {
        dstPos_p = nullptr;
    }
    daNpc_P1_childHIO_c* c = p1_child(mType);
    m_jnt.setParam(c->mMaxBackboneX, c->mMaxBackboneY, c->mMinBackboneX, c->mMinBackboneY, c->mMaxHeadX,
                   c->mMaxHeadY, c->mMinHeadX, c->mMinHeadY, c->mMaxTurnStep);
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        cLib_addCalcAngleS2(&mMaxLookVel, p1_child(mType)->mLookBackTargetY, 4, 0x800);
        look_at_target = 0;
        o_retval = TRUE;
    } else {
        mMaxLookVel = 0;
    }
    gabi::Local<cXyz> srcpos;
    srcpos->x = current.pos.x;
    srcpos->y = current.pos.y + 190.0f;
    srcpos->z = current.pos.z;
    gabi::call(0x0259DED0, &m_jnt, &current.angle.y, dstPos_p, srcpos.get(), (s32)current.angle.y,
               (s32)(s16)mMaxLookVel, (u32)look_at_target); /* dNpc_JntCtrl_c::lookAtTarget */
    return o_retval;
}
VERIFY(0x022B2E70, &daNpc_P1_c::lookBack);
