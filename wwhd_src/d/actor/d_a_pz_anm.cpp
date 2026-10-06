/**
 * d_a_pz_anm.cpp (WWHD): d_a_pz animation, eye control and drawing. Written from the WWHD code (the GameCube decompilation has only stubs for
 * d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

/* 025E1AA4 mDoAud_monsSeStart (fopAcM_monsSeStart, HD inline; the process id is read after the reverb) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    gabi::call(0x025E1AA4, id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}
/* 025D5A20 fopAcM_createChild(name, parentId, param, pos, roomNo, angle, scale, subtype, createFunc) */
static inline fpc_ProcID fopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle,
                                            cXyz* scale, s8 subtype, u32 createFunc) {
    return gabi::call<fpc_ProcID>(0x025D5A20, name, parent, param, pos, roomNo, angle, scale, subtype, createFunc);
}
/* 025D54C4 fopAcM_SearchByID(id, &actor) */
static inline BOOL fopAcM_SearchByID(fpc_ProcID id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline f64 sqrtf_d(f32 x) { return gabi::call<f64>(0x028F4384, x); }
static inline s16 cLib_targetAngleX(const cXyz* a, const cXyz* b) { return gabi::call<s16>(0x0200F974, a, b); }
static inline void cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    gabi::call(0x0200ECD4, v, target, scale, maxStep, minStep);
}
static inline f32 REG12_F(int i) { return REG_F(12, i); }

/* 02453BA8 */
void daPz_c::ctrlEye() {
    WWHD_FUNC(0x02453BA8, void, this);
    m0AFC.copy(mLookPos);
    s16 angleX = cLib_targetAngleX(&mEyePos2, &m0AFC);
    s16 angleY = cLib_targetAngleY(&mEyePos2, &m0AFC);
    s16 dx = (s16)(angleX - (m_jnt.mAngles[0][0] + m_jnt.mAngles[1][0]));
    s16 dy = (s16)(angleY - (shape_angle.y + m_jnt.mAngles[0][1] + m_jnt.mAngles[1][1]));
    f32 offX = (f32)dx * (1.0f / 8192.0f);
    f32 offY = (f32)dy * (1.0f / 8192.0f);
    offY = offY * 0.1f;
    offX = offX * 0.1f;
    m07F4 = !(offY < -0.1f) && !(offY > 0.1f);
    if (offX < -0.1f)
        offX = -0.1f;
    else
        offX = (offX - 0.1f >= 0.0f) ? 0.1f : offX;
    if (offY < -0.1f)
        offY = -0.1f;
    else
        offY = (offY - 0.1f >= 0.0f) ? 0.1f : offY;
    if (mMode == 3)
        offY = 0.1f;
    if (mpMatAnm[0].get() != nullptr) {
        cLib_addCalc(&mpMatAnm[0]->mNowOffsetX, offY, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&mpMatAnm[0]->mNowOffsetY, offX, 0.5f, 0.1f, 0.03f);
    }
    offY = -offY;
    if (mpMatAnm[1].get() != nullptr) {
        cLib_addCalc(&mpMatAnm[1]->mNowOffsetX, offY, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&mpMatAnm[1]->mNowOffsetY, offX, 0.5f, 0.1f, 0.03f);
    }
}
VERIFY(0x02453BA8, &daPz_c::ctrlEye);

/* 02453E00 */
void daPz_c::playEyeAnm() {
    WWHD_FUNC(0x02453E00, void, this);
    s8 eye = mCurEye;
    u32 endAddr = gabi::ea(mBtpAnm) + 0xA; /* mBtpAnm frame control: end frame */
    if (eye == 0 || eye == 7) {
        if (cLib_calcTimer(&m07F0) == 0) {
            u8 frame = (u8)(m1197 + 1);
            f32 f = (f32)frame;
            f32 end = (f32)gabi::load<s16>(endAddr);
            m1197 = frame;
            if (f > end) {
                f32 r = cM_rndF(100.0f);
                m07F0 = (s16)gabi::ftoi(r + 100.0f);
                m1197 = 0;
            }
        }
    } else if (((u32)eye >= 1 && (u32)eye <= 2) || ((u32)eye >= 5 && (u32)eye <= 6)) {
        m1197 = 1;
    } else {
        u8 frame = (u8)(m1197 + 1);
        f32 f = (f32)frame;
        f32 end = (f32)gabi::load<s16>(endAddr);
        m1197 = frame;
        if (f > end) {
            f32 r = cM_rndF(100.0f);
            f32 t = r + 100.0f;
            s16 e = gabi::load<s16>(endAddr);
            m07F0 = (s16)gabi::ftoi(t);
            m1197 = (u8)gabi::ftoi((f32)e);
        }
    }
    if (mCurEye == 4) {
        for (int i = 0; i < 2; i++)
            if (mpMatAnm[i].get() != nullptr)
                mpMatAnm[i]->mMoveFlag = 0;
        mDoExt_baseAnm_play(mBtkAnm);
    } else {
        for (int i = 0; i < 2; i++)
            if (mpMatAnm[i].get() != nullptr)
                mpMatAnm[i]->mMoveFlag = 1;
        ctrlEye();
    }
}
VERIFY(0x02453E00, &daPz_c::playEyeAnm);

/* 02454654 */
void daPz_c::setAnm(s8 i_anm, s32 i_morf, s32 i_eye) {
    WWHD_FUNC(0x02454654, void, this, i_anm, i_morf, i_eye);
    s32 anm;
    if (i_anm == 15) {
        anm = mAnm;
    } else {
        mAnm = i_anm;
        anm = i_anm;
    }
    /* dLib_anm_prm_c anm_prm[15] (copy of 0x10038B18; two morf times from the HIO) */
    gabi::Local<u8[0xF0]> prm;
    u32 p = gabi::ea(prm.get());
    for (u32 o = 0; o < 0xF0; o += 4) gabi::store<u32>(p + o, gabi::load<u32>(0x10038B18 + o));
    gabi::store<f32>(p + 7 * 0x10 + 4, hio_f32(0xF0));
    gabi::store<f32>(p + 8 * 0x10 + 4, hio_f32(0xF4));
    if (mOldAnm != anm) {
        if (anm == 4) {
            fopAcM_monsSeStart(this, 0x495E, 0);
            setBowAnm(2, 0);
            m0AE0 = fopAcM_createChild(0x1D8 /* PROC_ARROW? (Zelda's light arrow) */, fopAcM_GetID(this), 0, &current.pos,
                                       current.roomNo, nullptr, nullptr, -1, 0);
            anm = mAnm;
        }
        if (anm == 5) {
            setBowAnm(3, 1);
            anm = mAnm;
        }
        if (anm == 6) {
            fopAcM_monsSeStart(this, 0x495F, 0);
            setBowAnm(4, 1);
            gabi::Local<be<u32>> arrow;
            if (fopAcM_SearchByID(m0AE0, arrow)) {
                if (*arrow == 0) /* JUT_ASSERT(0x694, arrow_a != NULL) */
                    JUT_ASSERT_fail(STR(0x10038AFC), 0x694, STR(0x10038B08));
                if (*arrow != 0)
                    gabi::store<u32>(*arrow + 0xB0, 1); /* the arrow's parameters: shoot */
            }
            anm = mAnm;
        }
        bool bow;
        if (anm == 4 || anm == 5) {
            dPa_followEcallBack_end(&mFollowCB1);
            bow = (u32)(mAnm - 4) <= 2;
        } else {
            gabi::Local<be<u32>> arrow2;
            if (fopAcM_SearchByID(m0AE0, arrow2) && *arrow2 != 0)
                gabi::store<u32>(*arrow2 + 0xB0, 1);
            if (mAnm == 9) {
                setHeadSplash();
                bow = (u32)(mAnm - 4) <= 2;
            } else {
                dPa_followEcallBack_end(&mFollowCB1);
                bow = (u32)(mAnm - 4) <= 2;
            }
        }
        u32 eyeTbl = 0x10038AEC; /* eye state per animation */
        if (bow) {
            setBowString(1);
        } else {
            setBowAnm(1, 1);
            setBowString(0);
        }
        if (i_eye != 15)
            setEyeAnm(gabi::load<s8>(eyeTbl + i_eye));
        else
            setEyeAnm(gabi::load<s8>(eyeTbl + mAnm));
    }
    dLib_bcks_setAnm(l_arcName, mpMorf, &mBckIdx, &mAnm, &mOldAnm, 0x10038AC0 /* bcks */, p, i_morf, 0);
}
VERIFY(0x02454654, &daPz_c::setAnm);

/* 02454978 */
void daPz_c::setAnmRunSpeed() {
    WWHD_FUNC(0x02454978, void, this);
    if (mAnm != 3)
        return;
    gabi::Local<cXyz> d;
    cXyz_mi(&current.pos, d, &old.pos);
    f64 dist = sqrtf_d(PSVECSquareMag(d));
    f32 rate = (f32)(dist / (f64)(REG12_F(6) + 10.0f));
    if (rate > 0.0f) {
        if (!(rate < 1.0f))
            rate = 1.0f;
    } else {
        rate = 0.0f;
    }
    f32 spd = gabi::fmuls_ppc(rate, hio_f32(0x44));
    f32 lo = hio_f32(0x4C);
    spd = (gabi::fsubs_ppc(spd, lo) >= 0.0f) ? spd : lo;
    f32 hi = hio_f32(0x48);
    spd = (gabi::fsubs_ppc(spd, hi) >= 0.0f) ? hi : spd;
    mpMorf->setPlaySpeed(spd);
    s32 frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
    if (frame != 7 && frame != 14)
        return;
    /* static cXyz land_scale(0.6, 0.6, 0.6) (guard 0x1046D5F4) */
    if (gabi::load<u32>(0x1046D5F4) == 0) {
        gabi::store<u32>(0x1046D5F4, 1);
        gabi::store<f32>(0x1046D5FC, 0.6f);
        gabi::store<f32>(0x1046D600, 0.6f);
        gabi::store<f32>(0x1046D5F8, 0.6f);
    }
    gabi::Local<be<s32>> landId;
    u32 e = gabi::call<u32>(0x025A8BFC /* dPa_control_c::setSimpleLand */, dComIfGp_getParticle(),
                            gabi::ea(&mObjAcch) + 0xD4 + 0x14, &current.pos, &shape_angle, &tevStr, 1.25f, 1.5f, 1.0f,
                            landId.get(), 7);
    if (e != 0) {
        gabi::store<f32>(e + 0x34, 18.0f);
        gabi::store<f32>(e + 0x58, 1.0f);
        f32 sx = gabi::load<f32>(0x1046D5F8);
        gabi::store<f32>(e + 0x220, sx);
        f32 sy = gabi::load<f32>(0x1046D5FC);
        gabi::store<f32>(e + 0x224, sy);
        f32 sz = gabi::load<f32>(0x1046D600);
        gabi::store<f32>(e + 0x228, sz);
        gabi::store<f32>(e + 0x238, sx);
        gabi::store<f32>(e + 0x23C, sy);
        gabi::store<f32>(e + 0x240, sz);
    }
}
VERIFY(0x02454978, &daPz_c::setAnmRunSpeed);

/* 02455540 */
void daPz_c::bowDraw() {
    WWHD_FUNC(0x02455540, void, this);
    mDoExt_McaMorf* bow = mpBowMcaMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, bow->getModel(), &tevStr);
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), 14 /* right hand */), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpBowMcaMorf->getModel(), mDoMtx_stack_c::get());
    mpBowMcaMorf->updateDL();
}
VERIFY(0x02455540, &daPz_c::bowDraw);

/* 0245562C */
BOOL daPz_c::_draw() {
    WWHD_FUNC(0x0245562C, BOOL, this);
    u32 play = dComIfGp_ea();
    gabi::store<u32>(play + 0x5D3C, gabi::load<u32>(gabi::ea(&current.pos) + 0));
    gabi::store<u32>(play + 0x5D40, gabi::load<u32>(gabi::ea(&current.pos) + 4));
    gabi::store<u32>(play + 0x5D44, gabi::load<u32>(gabi::ea(&current.pos) + 8));
    gabi::call(0x0252F4E0, play + 0x5D30); /* HD: draw list (play+0x5D30) call with the position */
    if (mEnemyIce.mFreezeTimer > 20)
        gabi::call(0x0259138C, mpMorf.get(), -1, mInvisibleModel); /* dMat_control_c::iceEntryDL */
    else
        bodyDraw();
    if (m11B8)
        bowDraw();
    dSnap_RegistFig(0x87, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0245562C, &daPz_c::_draw);

/* 02455070 */
static BOOL daPzExecute(void* i_this) {
    WWHD_FUNC(0x02455070, BOOL, i_this);
    return ((daPz_c*)i_this)->_execute();
}
VERIFY(0x02455070, daPzExecute);

/* 024556E4 */
static BOOL daPzDraw(void* i_this) {
    WWHD_FUNC(0x024556E4, BOOL, i_this);
    return ((daPz_c*)i_this)->_draw();
}
VERIFY(0x024556E4, daPzDraw);

/* 024556E8 daPz_matAnm_c::calc(J3DMaterial*) const (HD: marks the texture matrices it changes) */
static void daPz_matAnm_c_calc(daPz_matAnm_c* i_this, void* i_mat) {
    WWHD_FUNC(0x024556E8, void, i_this, i_mat);
    u32 mat = gabi::ea(i_mat);
    /* the material's texture matrix name table (offset-based pointer at +0x38 of the texture block) */
    u32 blk = gabi::load<u32>(mat + 0);
    s32 off = gabi::load<s32>(blk + 0x38);
    u32 nameTab = off != 0 ? blk + 0x38 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, nameTab, STR(0x10038C24) /* "texmtx1" */); /* JUTNameTab::getIndex */
    u32 blk2 = gabi::load<u32>(mat + 0);
    s32 off2 = gabi::load<s32>(blk2 + 0x34);
    u32 tbl = off2 != 0 ? blk2 + 0x34 + off2 : 0;
    u32 entry = tbl + idx * 0x14;
    if (gabi::load<s32>(entry + 4) >= 0) {
        gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
        u32 w = gabi::load<u32>(mat + 0xC) + (u32)((idx >> 5) * 4);
        gabi::store<u32>(w, gabi::load<u32>(w) | (1u << (idx & 31)));
        blk2 = gabi::load<u32>(mat + 0);
    }
    s32 off3 = gabi::load<s32>(blk2 + 0x34);
    u32 tbl3 = off3 != 0 ? blk2 + 0x34 + off3 : 0;
    u16 idx2 = gabi::load<u16>(entry + 0xC);
    u32 entry2 = tbl3 + idx2 * 0x14;
    if (gabi::load<s32>(entry2 + 4) >= 0) {
        gabi::store<u16>(mat + 4, (u16)(gabi::load<u16>(mat + 4) | 4));
        u32 w = gabi::load<u32>(mat + 0xC) + (u32)((idx2 >> 5) * 4);
        gabi::store<u32>(w, gabi::load<u32>(w) | (1u << (idx2 & 31)));
    }
    if (i_this->mMoveFlag != 0) {
        u32 texMtx = gabi::load<u32>(mat + 0x28) + gabi::load<u16>(entry + 2);
        gabi::store<f32>(texMtx + 0x10, i_this->mNowOffsetX);
        gabi::store<f32>(texMtx + 0x14, i_this->mNowOffsetY);
    }
}
VERIFY(0x024556E8, daPz_matAnm_c_calc);
