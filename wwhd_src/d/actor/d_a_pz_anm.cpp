/**
 * d_a_pz_anm.cpp (WWHD): d_a_pz animation, eye control and drawing. Written from the WWHD code (the GameCube decompilation has only stubs for
 * d_a_pz), verified against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static s32 bcks[11] = {PZ_BCK_WAIT,     PZ_BCK_READY,     PZ_BCK_RUN,   PZ_BCK_DRAW_BOW,
                       PZ_BCK_HOLD_BOW, PZ_BCK_SHOOT_BOW, PZ_BCK_HIT,   PZ_BCK_DOWN,
                       PZ_BCK_WAKE,     PZ_BCK_DEFEND,    PZ_BCK_AFRAID};
static PzAnmParameterHD anm_prm[15] = {
    {0, -1, 0, 8.0f, 1.0f, 2}, {0, -1, 0, 8.0f, 1.0f, 2}, {1, -1, 0, 8.0f, 1.0f, 2},
    {2, -1, 0, 8.0f, 1.0f, 2}, {3, -1, 0, 2.0f, 1.0f, 0}, {4, -1, 0, 8.0f, 1.0f, 2},
    {5, -1, 0, 2.0f, 1.0f, 0}, {6, -1, 0, 4.0f, 1.0f, 0}, {7, -1, 0, 4.0f, 1.0f, 0},
    {8, -1, 0, 8.0f, 1.0f, 0}, {9, -1, 0, 8.0f, 1.0f, 0}, {10, -1, 0, 8.0f, 1.0f, 2},
    {1, -1, 0, 8.0f, 1.0f, 2}, {1, -1, 0, 8.0f, 1.0f, 2}, {4, -1, 0, 8.0f, 1.0f, 2}};

static s8 eye_tbl[15] = {0, 0, 7, 7, 1, 1, 1, 2, 3, 4, 5, 6, 8, 9, 8};

// HD: mDoAud_monsSeStart (fopAcM_monsSeStart, HD inline; the process id is read after the reverb)
// Native binding 025E1AA4
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    pzMonsSeStart(id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}
// HD: pzFopAcM_createChild(name, parentId, param, pos, roomNo, angle, scale, subtype, createFunc)
// Native binding 025D5A20

// HD: pzFopAcM_SearchByID(id, &actor)
// Native binding 025D54C4

static inline f32 REG12_F(int i) {
    return REG_F(12, i);
}

/* 02453BA8 */
void daPz_c::ctrlEye() {

    mEyeTargetPos.copy(m08C4);
    s16 angleX = pzCLib_targetAngleX(&mEyePos2, &mEyeTargetPos);
    s16 angleY = cLib_targetAngleY(&mEyePos2, &mEyeTargetPos);
    s16 dx = (s16)(angleX - (m_jnt.mAngles[0][0] + m_jnt.mAngles[1][0]));
    s16 dy = (s16)(angleY - (shape_angle.y + m_jnt.mAngles[0][1] + m_jnt.mAngles[1][1]));
    f32 offX = (f32)dx * (1.0f / 8192.0f);
    f32 offY = (f32)dy * (1.0f / 8192.0f);
    offY = offY * 0.1f;
    offX = offX * 0.1f;
    mbTargetInEyeRange = !(offY < -0.1f) && !(offY > 0.1f);
    if (offX < -0.1f)
        offX = -0.1f;
    else
        offX = (offX - 0.1f >= 0.0f) ? 0.1f : offX;
    if (offY < -0.1f)
        offY = -0.1f;
    else
        offY = (offY - 0.1f >= 0.0f) ? 0.1f : offY;
    if (mMode == MODE_ATTACK)
        offY = 0.1f;
    if (mpMatAnm[0].get() != nullptr) {
        pzCLib_addCalc(&mpMatAnm[0]->mNowOffsetX, offY, 0.5f, 0.1f, 0.03f);
        pzCLib_addCalc(&mpMatAnm[0]->mNowOffsetY, offX, 0.5f, 0.1f, 0.03f);
    }
    offY = -offY;
    if (mpMatAnm[1].get() != nullptr) {
        pzCLib_addCalc(&mpMatAnm[1]->mNowOffsetX, offY, 0.5f, 0.1f, 0.03f);
        pzCLib_addCalc(&mpMatAnm[1]->mNowOffsetY, offX, 0.5f, 0.1f, 0.03f);
    }
}

/* 02453E00 */
void daPz_c::playEyeAnm() {

    s8 eye = mCurEye;
    be<s16>& endFrame = mBtpAnm.mFrameCtrl.mEnd; /* &mBtpAnm frame control: end frame */
    if (eye == 0 || eye == 7) {
        if (pzCLib_calcTimer(&mBlinkTimer) == 0) {
            u8 frame = (u8)(mEyeFrame + 1);
            f32 f = (f32)frame;
            f32 end = (f32)endFrame.get();
            mEyeFrame = frame;
            if (f > end) {
                f32 r = cM_rndF(100.0f);
                mBlinkTimer = (s16)gabi::ftoi(r + 100.0f);
                mEyeFrame = 0;
            }
        }
    } else if (((u32)eye >= 1 && (u32)eye <= 2) || ((u32)eye >= 5 && (u32)eye <= 6)) {
        mEyeFrame = 1;
    } else {
        u8 frame = (u8)(mEyeFrame + 1);
        f32 f = (f32)frame;
        f32 end = (f32)endFrame.get();
        mEyeFrame = frame;
        if (f > end) {
            f32 r = cM_rndF(100.0f);
            f32 t = r + 100.0f;
            s16 e = endFrame.get();
            mBlinkTimer = (s16)gabi::ftoi(t);
            mEyeFrame = (u8)gabi::ftoi((f32)e);
        }
    }
    if (mCurEye == 4) {
        for (int i = 0; i < 2; i++)
            if (mpMatAnm[i].get() != nullptr)
                mpMatAnm[i]->mMoveFlag = 0;
        mDoExt_baseAnm_play(&mBtkAnm);
    } else {
        for (int i = 0; i < 2; i++)
            if (mpMatAnm[i].get() != nullptr)
                mpMatAnm[i]->mMoveFlag = 1;
        ctrlEye();
    }
}

/* 02454654 */
void daPz_c::setAnm(s8 i_anm, s32 i_morf, s32 i_eye) {

    s32 anm;
    if (i_anm == 15) {
        anm = m06D3;
    } else {
        m06D3 = i_anm;
        anm = i_anm;
    }

    PzAnmParametersHD prm;
    be<u32>* destination = reinterpret_cast<be<u32>*>(&prm);
    be<u32>* source = reinterpret_cast<be<u32>*>(anm_prm);
    for (int i = 0; i < 60; i++)
        destination[i] = source[i];
    prm.mEntries[7].mMorf = l_HIO.mDownMorfTime;
    prm.mEntries[8].mMorf = l_HIO.mDownWaitMorfTime;
    if (mOldAnm != anm) {
        if (anm == ANM_BOW_READY) {
            fopAcM_monsSeStart(this, JA_SE_CV_ZL_DRAW_BOW, 0);
            setBowAnm(2, 0);
            mLightArrowID = pzFopAcM_createChild(fpcNm_ARROW_e, fopAcM_GetID(this), 0, &current.pos,
                                                 current.roomNo, nullptr, nullptr, -1, 0);
            anm = m06D3;
        }
        if (anm == ANM_BOW_HOLD) {
            setBowAnm(3, 1);
            anm = m06D3;
        }
        if (anm == ANM_BOW_SHOOT) {
            fopAcM_monsSeStart(this, JA_SE_CV_ZL_SHOOT_BOW, 0);
            setBowAnm(4, 1);
            gabi::Local<be<u32>> arrow;
            if (pzFopAcM_SearchByID(mLightArrowID, arrow)) {
                if (*arrow == 0) /* JUT_ASSERT(0x694, arrow_a != NULL) */
                    JUT_ASSERT_fail("d_a_pz.cpp", 0x694, "arrow_a != (0)");
                if (*arrow != 0)
                    pzGuest<fopAc_ac_c>(*arrow)->mParameters =
                        1; /* the arrow's parameters: shoot */
            }
            anm = m06D3;
        }
        bool bow;
        if (anm == ANM_BOW_READY || anm == ANM_BOW_HOLD) {
            dPa_followEcallBack_end(&mFollowCB1);
            bow = (u32)(m06D3 - 4) <= 2;
        } else {
            gabi::Local<be<u32>> arrow2;
            if (pzFopAcM_SearchByID(mLightArrowID, arrow2) && *arrow2 != 0)
                pzGuest<fopAc_ac_c>(*arrow2)->mParameters = 1;
            if (m06D3 == ANM_WAKE) {
                setHeadSplash();
                bow = (u32)(m06D3 - 4) <= 2;
            } else {
                dPa_followEcallBack_end(&mFollowCB1);
                bow = (u32)(m06D3 - 4) <= 2;
            }
        }
        if (bow) {
            setBowString(1);
        } else {
            setBowAnm(1, 1);
            setBowString(0);
        }
        if (i_eye != 15)
            setEyeAnm(eye_tbl[i_eye]);
        else
            setEyeAnm(eye_tbl[m06D3]);
    }
    pzDLib_bcks_setAnm(l_arcName, mpMorf, &mBckIdx, &m06D3, &mOldAnm, gabi::ea(bcks),
                       gabi::ea(&prm), i_morf, 0);
}

/* 02454978 */
void daPz_c::setAnmRunSpeed() {

    if (m06D3 != ANM_RUN)
        return;
    cXyz d;
    cXyz_mi(&current.pos, &d, &old.pos);
    f64 dist = pzSqrtf_d(PSVECSquareMag(&d));
    f32 rate = (f32)(dist / (f64)(REG12_F(6) + 10.0f));
    if (rate > 0.0f) {
        if (!(rate < 1.0f))
            rate = 1.0f;
    } else {
        rate = 0.0f;
    }
    f32 spd = gabi::fmuls_ppc(rate, l_HIO.mRunRate);
    f32 lo = l_HIO.mRunMinRate;
    spd = (gabi::fsubs_ppc(spd, lo) >= 0.0f) ? spd : lo;
    f32 hi = l_HIO.mRunMaxRate;
    spd = (gabi::fsubs_ppc(spd, hi) >= 0.0f) ? hi : spd;
    mpMorf->setPlaySpeed(spd);
    s32 frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
    if (frame != 7 && frame != 14)
        return;
    // HD: Preserve the compiler's component and guard write order.

    if (l_landScaleInitialized == 0) {
        l_landScaleInitialized = 1;
        l_landScale.y = 0.6f;
        l_landScale.z = 0.6f;
        l_landScale.x = 0.6f;
    }
    gabi::Local<be<s32>> landId;
    u32 e = pzSetSimpleLand(dComIfGp_getParticle(),
                            gabi::ea(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)), &current.pos,
                            &shape_angle, &tevStr, 1.25f, 1.5f, 1.0f, landId.get(), 7);
    if (e != 0) {
        pzGuest<PzEmitterHD>(e)->mRate = 18.0f;
        pzGuest<PzEmitterHD>(e)->mSpread = 1.0f;
        f32 sx = l_landScale.x;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.x = sx;
        f32 sy = l_landScale.y;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.y = sy;
        f32 sz = l_landScale.z;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.z = sz;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.x = sx;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.y = sy;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.z = sz;
    }
}

/* 02455540 */
void daPz_c::bowDraw() {

    mDoExt_McaMorf* bow = mpBowMcaMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, bow->getModel(), &tevStr);
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), PZ_JNT_HAND_R1_e), mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpBowMcaMorf->getModel(), mDoMtx_stack_c::get());
    mpBowMcaMorf->updateDL();
}

/* 0245562C */
BOOL daPz_c::_draw() {

    u32 play = dComIfGp_ea();
    // HD: word copies preserve bits and interleave each load with its store.
    PzPlayDrawHD* draw = pzGuest<PzPlayDrawHD>(play);
    be<u32>* dst = reinterpret_cast<be<u32>*>(&draw->mPosition);
    be<u32>* src = reinterpret_cast<be<u32>*>(&current.pos);
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    pzSetDrawPosition(gabi::ea(draw->mDrawList));
    if (mEventIce.mFreezeTimer > 20)
        pzIceEntryDL(mpMorf.get(), -1, mInvisibleModel); /* dMat_control_c::iceEntryDL */
    else
        bodyDraw();
    if (m0F88)
        bowDraw();
    dSnap_RegistFig(0x87, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02455070 */
static BOOL daPzExecute(void* i_this) {

    return ((daPz_c*)i_this)->_execute();
}

/* 024556E4 */
static BOOL daPzDraw(void* i_this) {

    return ((daPz_c*)i_this)->_draw();
}

// HD: daPz_matAnm_c::calc(J3DMaterial*) const (HD: marks the texture matrices it changes)
/* 024556E8 */
static void daPz_matAnm_c_calc(daPz_matAnm_c* i_this, void* i_mat) {
    PzMaterialHD* material = reinterpret_cast<PzMaterialHD*>(i_mat);
    PzTextureBlockHD* block = material->mpTextureBlock;
    s32 index = pzFindNameIndex(gabi::ea(block->mNames.get()), "texmtx1");
    PzTextureBlockHD* block2 = material->mpTextureBlock;
    PzTextureEntryHD* entries = block2->mEntries.get();
    PzTextureEntryHD* entry = entries + index;
    if (entry->mAnimationIndex >= 0) {
        material->mDirtyFlags |= 4;
        be<u32>* dirty = material->mpDirtyMatrices.get() + (index >> 5);
        *dirty |= 1u << (index & 31);
        block2 = material->mpTextureBlock;
    }
    PzTextureEntryHD* entries2 = block2->mEntries.get();
    u16 linkedIndex = entry->mLinkedIndex;
    PzTextureEntryHD* linkedEntry = entries2 + linkedIndex;
    if (linkedEntry->mAnimationIndex >= 0) {
        material->mDirtyFlags |= 4;
        be<u32>* dirty = material->mpDirtyMatrices.get() + (linkedIndex >> 5);
        *dirty |= 1u << (linkedIndex & 31);
    }
    if (i_this->mMoveFlag != 0) {
        PzTextureMatrixHD* matrix = reinterpret_cast<PzTextureMatrixHD*>(
            material->mpTextureMatrices.get() + entry->mMatrixOffset);
        matrix->mOffsetX = i_this->mNowOffsetX;
        matrix->mOffsetY = i_this->mNowOffsetY;
    }
}

// clang-format off
/* WWHD glue
data bcks 10038AC0
data anm_prm 10038B18
data eye_tbl 10038AEC
str 10038AFC in 02454654
str 10038B08 in 02454654
str 10038C24 in 024556E8
str 10038D00
*/
// clang-format on
