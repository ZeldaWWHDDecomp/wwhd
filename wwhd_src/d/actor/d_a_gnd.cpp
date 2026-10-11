/**
 * d_a_gnd.cpp (WWHD)
 * Boss - Ganondorf
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs: every function here is written from the WWHD code (cking.rpx) and verified against it.
 * GameCube names are kept where the function map matched them.
 */
#include "d/actor/d_a_gnd_main.h"

static gndInitRecord l_tevInit;
static gndMainFlag l_hioInitialized;
static dCcD_SrcCyl cc_cyl_src;
static dCcD_SrcSph head_sph_src;
static dCcD_SrcSph chest_sph_src;
static dCcD_SrcSph wepon_sph_src;
static GXColor l_hairColor;
static f32 l_cameraCenterInit[15] = {-34.5f, 129.7f, -26.3f, -37.9f,  202.6f,
                                     -77.5f, -11.5f, 446.0f, -235.0f, 7.3f,
                                     293.0f, 55.9f,  -7.3f,  -51.6f,  116.5f};
static f32 l_cameraEyeInit[12] = {101.9f, 38.0f,  550.0f, -10.0f, 7.0f,   -644.0f,
                                  83.6f,  870.4f, -88.0f, 89.0f,  519.0f, -43.3f};

static u32 l_effectAnimations[18] = {36, 37, 38, 39, 40, 43, 41, 42, 72,
                                     71, 69, 70, 83, 84, 54, 55, 53, 78};
static u32 l_effectIDs[108] = {
    33670, 33671, 33672, 33673, 50013, 50014, 33674, 33675, 33676, 33677, 50015, 50016,
    33581, 33582, 33583, 33584, 49963, 49964, 33678, 33679, 33680, 33681, 50017, 50018,
    33682, 33683, 33684, 33685, 50019, 50020, 33686, 33687, 33688, 33689, 50021, 50022,
    33690, 33691, 33692, 33693, 50023, 50024, 33694, 33695, 33696, 33697, 50025, 50026,
    33698, 33699, 33700, 33701, 50027, 50028, 33702, 33703, 33704, 33705, 50029, 50030,
    33706, 33707, 33708, 33709, 50031, 50032, 33710, 33711, 33712, 33713, 50033, 50034,
    33714, 33715, 33716, 33717, 50035, 50036, 33718, 33719, 33720, 33721, 50037, 50038,
    33722, 33723, 33724, 33725, 50039, 50040, 33726, 33727, 33728, 33729, 50041, 50042,
    33726, 33727, 33728, 33729, 50041, 50042, 0,     0,     0,     33753, 0,     0};
static u32 l_flashFrames[15] = {10, 2, 2, 5, 5, 10, 15, 20, 13, 18, 10, 15, 20, 13, 18};
static s32 l_flashJoints[15] = {47, 60, 61, 62, 9, 11, 14, 17, 13, 16, 29, 32, 35, 31, 34};
static f32 l_flashScales[15] = {1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.8f,
                                0.8f, 0.8f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f};
static s32 l_hairJoints[4] = {27, 27, 45, 45};
static f32 l_hairOffsets[4] = {-4.0f, 4.0f, -4.0f, 4.0f};
static s32 l_defenceAnimations[6] = {46, 47, 49, 51, 52, 50};

static inline BOOL gnd_morfIsStop(mDoExt_McaMorf* morf) {
    return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f;
}

/* fopAcM_monsSeStart as inlined in damage_check: only the eyePos check */
static inline void gnd_monsSeStart_noActorCheck(gnd_class* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gndMain_monsSeStart(id, &a->eyePos, pid, 0, reverb);
    }
}

static inline void gnd_emitterColor(gnd_class* i_this, JPABaseEmitter* emitter) {
    u32 e = gabi::ea(emitter);
    u8 r = mainFields(i_this)->mParticleRed;
    u8 b = mainFields(i_this)->mParticleBlue;
    mainEmitter(e)->mGlobalColorPrm.r = r;
    u8 g = mainFields(i_this)->mParticleGreen;
    mainEmitter(e)->mGlobalColorPrm.b = b;
    mainEmitter(e)->mGlobalColorPrm.g = g;
}

// Debug text retains its original full-width format-string argument.
static inline void gnd_JUTReport(s32 x, s32 y, u32 fmt, s32 v) { gndMain_report(x, y, fmt, v); }

static inline int gnd_defenceAnm(s8 i) { return l_defenceAnimations[i]; }

/* 02157D48 */
void* daGnd_HIO_ct(void* hio) {
    if (hio == nullptr) {
        hio = operator_new(0x8C);
        if (hio == nullptr)
            return hio;
    }
    gndMainHIO* state = gabi::at<gndMainHIO>(gabi::ea(hio));
    state->mAttackDistance = 1200.0f;
    state->mWalkSpeed = 11.0f;
    state->mJumpSpeed = 25.0f;
    state->mFollowDistance = 629.0f;
    state->mFallSpeed = -30.0f;
    state->mHairLength = 45.0f;
    state->mPhaseLock = 0;
    state->mScale = 1.0f;
    state->mMoveRate = 5.0f;
    state->mTurnRate = 5.0f;
    state->mFreeze = 0;
    state->mRetreatSpeed = -0.5f;
    state->mAirGravity = -2.0f;
    state->mJumpHeight = 20.0f;
    state->mDrawHair = 1;
    state->mDodgeYaw = 0.0f;
    state->mGuardDistance = 100.0f;
    state->mCameraOffset = 0.0f;
    state->mDebug = 0;
    state->mJumpType = 1;
    state->mDodgeGravity = -2.0f;
    state->mDodgeJumpSpeed = 15.0f;
    state->__vtbl = 0x10010064; /* vtable */
    state->mJumpEnabled = 1;
    state->mDodgeHeight = 30.0f;
    state->mHairAngle = 35.0f;
    state->mParryWait = 10;
    state->mCloseAttackDistance = 630.0f;
    state->mNo = -1;
    state->mDodgeStep = 8.0f;
    state->mAttackWaitAlt = 30;
    state->mDodgeDistance = 400.0f;
    state->mCounterWait = 15;
    state->mRecoveryDistance = 600.0f;
    state->mStartZeldaDemo = 0;
    state->mLongIntro = 0;
    state->mPhaseOverride = 0;
    state->mSpinAttackType = 6;
    state->mSwordAttackType = 3;
    state->mSwordShield = 1;
    state->mAttackDelay = 0;
    state->mParryWaitAlt = 0;
    state->mAttackStart = 10;
    state->mGuardMode = 0;
    state->mFinishStart = 10;
    state->mAttackWait = 30;
    state->mDamageWait = 0x46;
    state->mFinishEnd = 30;
    state->mFinishActive = 1;
    state->mAttackEnd = 10;
    return hio;
}

/* 0215413C */
BOOL checkGround(gnd_class* i_this, f32 y) { return i_this->current.pos.y < y + 1.0f; }

/* 0215415C */
void splash_set(gnd_class* i_this) {
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x833C, &i_this->current.pos);
    if (emitter != nullptr) {
        u32 e = gabi::ea(emitter);
        u8 r = mainFields(i_this)->mParticleRed;
        u8 b = mainFields(i_this)->mParticleBlue;
        mainEmitter(e)->mGlobalColorPrm.r = r;
        u8 g = mainFields(i_this)->mParticleGreen;
        mainEmitter(e)->mGlobalColorPrm.b = b;
        mainEmitter(e)->mGlobalColorPrm.g = g;
    }
}

/* 021541E4 */
void attack_eff_remove(gnd_class* i_this) {
    for (int i = 0; i < 6; i++) {
        u32 e = mainFields(i_this)->mAttackEmitters[i];
        if (e != 0) {
            // Retire the emitter without deleting its particle system.
            u32 flags = mainEmitter(e)->mStatus;
            mainEmitter(e)->mMaxFrame = -1;
            mainEmitter(e)->mStatus = flags | 1;
            mainFields(i_this)->mAttackEmitters[i] = 0;
        }
    }
}

/* 02154228 */
void anm_init(gnd_class* i_this, int anm, f32 morf, u8 mode, f32 speed, int bas) {
    if (bas >= 0) {
        void* a = dComIfG_getObjectRes("Gnd", anm, GND_SAFESTRING_VTBL);
        void* b = dComIfG_getObjectRes("Gnd", bas, GND_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)a, mode, morf, speed, 0.0f, -1.0f, b);
    } else {
        void* a = dComIfG_getObjectRes("Gnd", anm, GND_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)a, mode, morf, speed, 0.0f, -1.0f, nullptr);
    }
    attack_eff_remove(i_this);

    for (int i = 0; i < 18; i++) {
        if ((u32)anm == l_effectAnimations[i]) {
            for (int j = 0; j < 6; j++) {
                u32 id = l_effectIDs[i * 6 + j];
                if (id != 0) {
                    JPABaseEmitter* emitter = dComIfGp_particle_set((u16)id, &i_this->current.pos);
                    mainFields(i_this)->mAttackEmitters[j] = gabi::ea(emitter);
                    if (j <= 3) {
                        u32 e = gabi::ea(emitter);
                        u8 r = mainFields(i_this)->mParticleRed;
                        u8 b = mainFields(i_this)->mParticleBlue;
                        mainEmitter(e)->mGlobalColorPrm.r = r;
                        u8 g = mainFields(i_this)->mParticleGreen;
                        mainEmitter(e)->mGlobalColorPrm.b = b;
                        mainEmitter(e)->mGlobalColorPrm.g = g;
                    }
                }
            }
            return;
        }
    }
}

/* 0215449C */
void* z_s_sub(void* a, void*) {
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == 0xD2) {
        return a;
    }
    return nullptr;
}

/* 021544EC */
BOOL daGnd_Draw(gnd_class* i_this) {
    s16 blur = mainFields(i_this)->mBlurRate;
    if (blur >= 1) {
        if (blur == 1) {
            mainFields(i_this)->mBlurRate = 0;
            mDoGph_offBlure();
        } else {
            mDoGph_setBlureRate((u8)blur);
            mDoGph_onBlure();
        }
    }
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    s16 flash = mainFields(i_this)->mEffectTimer;
    if (flash != 0) {
        // HD: body flash adjusts the fog colour and start depth.
        s16 add = mainFields(i_this)->mFlashColor;
        s16 r = (s16)(i_this->tevStr.mFogColor.r + add);
        if (r > 0xFF)
            r = 0xFF;
        s16 g = (s16)(i_this->tevStr.mFogColor.g + add);
        if (g > 0xFF)
            g = 0xFF;
        s16 b = (s16)(i_this->tevStr.mFogColor.g + add / 2);
        if (b > 0xFF)
            b = 0xFF;
        if (flash > (s16)(REG_S(8, 5) + 20)) {
            cLib_addCalcAngleS2(&mainFields(i_this)->mFlashColor, 0x118, 1, 0x1E);
            cLib_addCalc2(&mainFields(i_this)->mFlashFog, -50000.0f, 1.0f, 5000.0f);
        } else {
            cLib_addCalcAngleS2(&mainFields(i_this)->mFlashColor, 0, 1, 0xE);
            cLib_addCalc0(&mainFields(i_this)->mFlashFog, 1.0f, 2500.0f);
        }
        i_this->tevStr.mFogColor.r = (u8)r;
        i_this->tevStr.mFogColor.g = (u8)g;
        i_this->tevStr.mFogColor.b = (u8)b;
        i_this->tevStr.mFogStartZ =
            gabi::fadds_ppc(i_this->tevStr.mFogStartZ, mainFields(i_this)->mFlashFog);
    }
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    mDoExt_brkAnm* brk = i_this->mpBrk;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), brk->mFrameCtrl.mFrame);
    mDoExt_btkAnm* btk = i_this->mpBtk;
    mDoExt_btkAnm_entry(btk, J3DModel_getModelData(model), btk->mFrameCtrl.mFrame);
    mDoExt_btpAnm* btp = i_this->mpBtp;
    s16 frame = (s16)gabi::ftoi(btp->mFrameCtrl.mFrame);
    mDoExt_btpAnm_entry(btp, J3DModel_getModelData(model), frame);
    i_this->mpMorf->entryDL();
    dSnap_RegistFig(0xCE, i_this, 1.0f, 1.0f, 1.0f);
    if (mainHIO()->mDrawHair != 0) {
        mDoExt_3DlineMat0_update(i_this->mLineMat, 0x14, REG0_F(3) + 2.25f, &l_hairColor, 2,
                                 &i_this->tevStr);
        gnd_set3DlineMat(i_this->mLineMat);
    }
    return TRUE;
}

/* 021547B0 */
BOOL player_view_check(gnd_class* i_this, s16 angle) {
    dComIfGp_getPlayer(0); /* the player pointer is fetched but not used */
    s16 d = i_this->shape_angle.y - angle;
    if (d < 0)
        d = -d;
    return (u16)d < 0x4000;
}

/* 02154810 */
void ke_move(gnd_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    gabi::Local<cXyz> axis;
    gabi::Local<cXyz> base;
    gabi::Local<cXyz> step;
    gabi::Local<cXyz> out;
    for (int i = 0; i < 4; i++) {
        u32 ke = gabi::ea(&mainFields(i_this)->mHair[i]);
        cXyz* pos = mainFields(i_this)->mHair[i].mPos;
        cXyz* vel = mainFields(i_this)->mHair[i].mVelocity;
        // ke_pos_set: anchor the strand at its skeleton joint.
        s32 jnt = l_hairJoints[i];
        PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
        Mtx34* m = calc_mtx();
        f32 m00 = m->m[0][0];
        f32 m20 = m->m[2][0];
        f32 m10 = m->m[1][0];
        axis->z = m20;
        axis->x = m00;
        axis->y = m10;
        f32 scale = std_sqrtf(PSVECSquareMag(axis));
        BOOL ev = gnd_startCheckOld("endhr" /* "endhr" */);
        f32 reg4 = REG0_F(4);
        if (ev && mainDemoFrame() >= 300) {
            scale = 0.0f;
        }
        f32 off = l_hairOffsets[i];
        base->y = off;
        base->z = off;
        base->x = gabi::fadds_ppc(reg4, -20.0f);
        MtxPosition(base, &pos[0]);
        /* ke_control */
        f32 groundY = gabi::fadds_ppc(i_this->mAcch.m_ground_h, 3.0f);
        f32 len = gabi::fmuls_ppc(gabi::fmuls_ppc(mainHIO()->mHairLength, scale), 0.5f);
        step->x = 0.0f;
        step->y = 0.0f;
        f32 reg1 = REG0_F(1);
        if (scale > 0.05f) { /* ble: taken on NaN */
            step->z = len;
        } else {
            step->z = 0.0f;
        }
        f32 fall = gabi::fadds_ppc(reg1, -5.0f);
        f32 damp = gabi::fadds_ppc(REG0_F(2), 0.73f);
        for (int j = 1; j < 20; j++) {
            f32 y = gabi::fadds_ppc(gabi::fadds_ppc(pos[j].y, vel[j].y), fall);
            f32 dx = gabi::fadds_ppc(gabi::fsubs_ppc(pos[j].x, pos[j - 1].x), vel[j].x);
            if (y < groundY)
                y = groundY; /* bge: taken on NaN */
            f32 dy = gabi::fsubs_ppc(y, pos[j - 1].y);
            f32 dz = gabi::fadds_ppc(gabi::fsubs_ppc(pos[j].z, pos[j - 1].z), vel[j].z);
            s16 ax = -cM_atan2s(dy, dz);
            s16 ay = cM_atan2s(dx, std_sqrtf(gabi::fmadds(dy, dy, dz * dz)));
            gnd_XrotS(calc_mtx(), ax);
            mDoMtx_YrotM(calc_mtx(), ay);
            MtxPosition(step, out);
            vel[j].copy(pos[j]); /* the old position, integer copy */
            pos[j].x = gabi::fadds_ppc(pos[j - 1].x, out->x);
            pos[j].y = gabi::fadds_ppc(pos[j - 1].y, out->y);
            pos[j].z = gabi::fadds_ppc(pos[j - 1].z, out->z);
            vel[j].x = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].x, vel[j].x), damp);
            vel[j].y = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].y, vel[j].y), damp);
            vel[j].z = gabi::fmuls_ppc(gabi::fsubs_ppc(pos[j].z, vel[j].z), damp);
        }
        /* the line material's strand i: 16-byte entries at *(mLineMat + 0x144), points at +0 */
        u32 dst = gabi::at<gndMainLineMat>(gabi::ea(i_this->mLineMat))->mpLines[i].mpPos;
        for (int k = 0; k < 20; k++) {
            for (int w = 0; w < 3; w++)
                mainWordVectors(dst)[k].words[w] = mainWordVectors(ke)[k].words[w];
        }
    }
}

/* 02154C34 */
void pos_move(gnd_class* i_this, s8 noTurn) {
    if (noTurn == 0) {
        gabi::Local<cXyz> d;
        cXyz_mi(&mainFields(i_this)->mTargetPos, d, &i_this->current.pos);
        f32 dx = d->x;
        f32 dz = d->z;
        s16 target = cM_atan2s(dx, dz);
        f32 step = (f32)mainFields(i_this)->mTurnStep * (f32)mainFields(i_this)->mTurnBlend;
        cLib_addCalcAngleS2(&i_this->current.angle.y, target, 5, (s16)gabi::ftoi(step));
        cLib_addCalc2(&mainFields(i_this)->mTurnBlend, 1.0f, 1.0f, 0.05f);
    }
    cLib_addCalc2(&i_this->speedF, mainFields(i_this)->mTargetSpeed, 1.0f, 6.0f);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    gabi::Local<cXyz> out;
    MtxPosition(v, out);
    i_this->speed.x = out->x;
    i_this->speed.z = out->z;
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    f32 sy = i_this->speed.y;
    f32 py = i_this->current.pos.y;
    f32 g = i_this->gravity;
    i_this->current.pos.y = gabi::fadds_ppc(py, sy);
    i_this->speed.y = gabi::fadds_ppc(sy, g);
    i_this->gravity = -5.0f;
}

/* 02154D88 */
void wait_set(gnd_class* i_this) {
    if ((s8)mainFields(i_this)->mBattlePhase.value <= 1) {
        anm_init(i_this, 0x57, 10.0f, 2, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4946, 0);
    } else {
        anm_init(i_this, 0x58, 10.0f, 2, 1.0f, -1);
    }
}

/* 02158134 */
void attack_last(gnd_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 mode = mainFields(i_this)->mMode;
    s32 frame = gabi::ftoi(i_this->mpMorf->mFrameCtrl.mFrame);
    mainFields(i_this)->mSwordRadiusRight = REG0_F(13) + 110.0f;
    mainFields(i_this)->mGuard.value = 0;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x52, 5.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x494D, 0);
        mainFields(i_this)->mMode = 1;
        break;
    case 1:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x51, 2.0f, 0, 1.0f, 0x1E);
            mainFields(i_this)->mMode = 2;
        }
        if (frame > mainHIO()->mFinishStart) {
            mainFields(i_this)->mGuardAttackType = 4;
        }
        break;
    case 2:
        if (frame >= mainHIO()->mFinishActive && frame <= mainHIO()->mFinishEnd) {
            mainFields(i_this)->mGuardAttackType = 4;
        }
        if ((u32)(frame - 6) < 4) {
            mainFields(i_this)->mSwordAttackRight = 1;
        }
        if (gnd_morfIsStop(i_this->mpMorf)) {
            s16 ang = i_this->shape_angle.y;
            mainFields(i_this)->mAction = 0;
            i_this->current.angle.y = ang;
            mainFields(i_this)->mMode = 0;
            mainFields(i_this)->mWaitTimer = 0;
        }
        break;
    }
    if (mainPlayer(player)->mNoResetFlg1 & 0x20000000) {
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mItemLifeCount = 0.0f;
        mainFields(i_this)->mGuard.value = 0;
        mainFields(i_this)->mAction = 0x1E;
        mainFields(i_this)->mMode = 0;
        mainFields(i_this)->mDamageTimer = 5;
    }
}

/* 02158360 */
void defence0(gnd_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    int end = 0;
    mainFields(i_this)->mInvulnerabilityTimer = 10;
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    f32 lim = (i_this->mAcch.m_ground_h + 200.0f) + REG0_F(11);
    s16 mode = mainFields(i_this)->mMode;
    if (i_this->current.pos.y > lim) {
        if (mode < 0xF) {
            mode = 0x14;
            mainFields(i_this)->mMode = mode;
        }
    }
    auto startDodge = [&]() {
        if (mainHIO()->mJumpEnabled != 0 && mainFields(i_this)->mDodge != 5 &&
            mainFields(i_this)->mGuardHits.signedValue >= 3 && cM_rndF(1.0f) < 0.2f) {
            bool j1;
            if (cM_rndF(1.0f) < 0.5f) {
                mainFields(i_this)->mModelYawOffset = 65536.0f;
            } else {
                mainFields(i_this)->mModelYawOffset = -65536.0f;
            }
            j1 = mainHIO()->mJumpType == 1;
            if (!j1) {
                i_this->speed.y = REG0_F(9) + 15.0f;
                mainFields(i_this)->mTargetSpeed = REG0_F(10) + -5.0f;
                mainFields(i_this)->mKnockbackSpeed = 0.0f;
                splash_set(i_this);
            } else {
                i_this->speed.y = REG0_F(7) + 20.0f;
                mainFields(i_this)->mTargetSpeed = REG0_F(8) + -20.0f;
                s16 reg = REG0_S(2);
                mainFields(i_this)->mKnockbackSpeed = 0.0f;
                mainFields(i_this)->mJumpTimer = reg + 10;
                splash_set(i_this);
            }
            gnd_monsSeStart_noActorCheck(i_this, 0x4956);
        } else {
            gnd_monsSeStart_noActorCheck(i_this, 0x494E);
        }
        mainFields(i_this)->mMode = 10;
    };
    switch ((u32)(s32)mode) {
    case 0: {
        f32 anmMorf;
        if (mainFields(i_this)->mBattlePhase.signedValue == 2 &&
            (f32)(player->current.pos.y - i_this->mAcch.m_ground_h) > REG0_F(19) + 300.0f) {
            mainFields(i_this)->mDodge = 0;
            mainFields(i_this)->mGuardHits.signedValue = 2;
            anmMorf = 4.0f;
        } else {
            bool set5 = false;
            if (mainPlayer(player)->mCutType == 0xF && cM_rndF(1.0f) < 0.8f) {
                set5 = true;
            } else if (mainFields(i_this)->mPreviousDodge == 1 && cM_rndF(1.0f) < 0.05f &&
                       mainFields(i_this)->mBattlePhase.signedValue == 2) {
                set5 = true;
            }
            if (set5) {
                mainFields(i_this)->mDodge = 5;
                s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                mainFields(i_this)->mDodgeAngle = ang + REG0_S(4) + 0x7800;
            }
            anmMorf = 4.0f;
        }
        anm_init(i_this, gnd_defenceAnm(mainFields(i_this)->mDodge), anmMorf, 0, 1.0f, -1);
        mainFields(i_this)->mJumpTimer = 0;
        startDodge();
        break;
    }
    case 0xA: {
        if (mainFields(i_this)->mBattlePhase.signedValue == 2 &&
            (f32)(player->current.pos.y - i_this->mAcch.m_ground_h) > REG0_F(19) + 300.0f &&
            mainFields(i_this)->mDodge != 0) {
            mainFields(i_this)->mDodge = 0;
            mainFields(i_this)->mGuardHits.signedValue = 2;
            anm_init(i_this, gnd_defenceAnm(0), 2.0f, 0, 1.0f, -1);
            mainFields(i_this)->mJumpTimer = 0;
            startDodge();
            break;
        }
        i_this->gravity = REG0_F(11) + -4.0f;
        if (checkGround(i_this, 0.0f)) {
            mainFields(i_this)->mTargetSpeed = 0.0f;
            bool jump = false;
            if (mainHIO()->mJumpType == 0) {
                if (dist < REG0_F(2) + 400.0f && mainFields(i_this)->mDodge != 5 &&
                    mainFields(i_this)->mWaitTimer == (s16)(REG0_S(5) + 1)) {
                    jump = true;
                }
            } else {
                s8 t = mainFields(i_this)->mJumpTimer;
                if (t != 0) {
                    t = t - 1;
                    mainFields(i_this)->mJumpTimer = t;
                    if (t == 0)
                        jump = true;
                }
            }
            if (jump) {
                mainFields(i_this)->mAction = 3;
                f32 r = cM_rndF(1.0f);
                mainFields(i_this)->mGuard.value = 0;
                mainFields(i_this)->mMode = (r < 0.5f) ? 0 : 1;
            }
        }
        gnd_JUTReport(0x1E, 0xC8, gabi::guest_addr("TIME 0 %d"), mainFields(i_this)->mActionTimer);
        gnd_JUTReport(0x1E, 0xDC, gabi::guest_addr("TIME 1 %d"), mainFields(i_this)->mWaitTimer);
        if (mainFields(i_this)->mActionTimer == 0 && mainFields(i_this)->mWaitTimer == 0 &&
            checkGround(i_this, 0.0f) && mainFields(i_this)->mJumpTimer == 0) {
            end = 1;
            break;
        }
        if (mainFields(i_this)->mGuardHit.signedValue == 0)
            break;
        mainFields(i_this)->mGuardHit.signedValue = 0;
        bool playReaction = false;
        if (checkGround(i_this, 0.0f)) {
            s8 n = mainFields(i_this)->mGuardHits.signedValue;
            bool tryY;
            if (n >= 8) {
                if (cM_rndF(1.0f) < 0.3f) {
                    mainFields(i_this)->mGuardHits.signedValue = 0;
                    mainFields(i_this)->mMode = 15;
                    mainFields(i_this)->mWaitTimer = REG_S(10, 4) + 10;
                    playReaction = true;
                }
                if (!playReaction)
                    tryY = mainFields(i_this)->mGuardHits.signedValue >= 4;
            } else {
                tryY = n >= 4;
            }
            if (!playReaction && tryY && cM_rndF(1.0f) < 0.5f) {
                mainFields(i_this)->mGuardTimer = REG0_S(6) + 0xD;
                mainFields(i_this)->mWaitTimer = REG_S(10, 4) + 10;
                playReaction = true;
            }
        }
        if (!playReaction) {
            s8 n = mainFields(i_this)->mGuardHits.signedValue;
            mainFields(i_this)->mMode = 0;
            if (n <= 1) {
                mainFields(i_this)->mDodge = 0;
            } else {
                for (int k = 20; k != 0; k--) {
                    s8 v = (s8)gabi::ftoi(cM_rndF(4.99f));
                    s8 cur = mainFields(i_this)->mDodge;
                    if ((u32)(s32)v != (u32)(s32)cur) {
                        mainFields(i_this)->mPreviousDodge = cur;
                        mainFields(i_this)->mDodge = v;
                        break;
                    }
                }
            }
            mainFields(i_this)->mWaitTimer = REG_S(10, 4) + 10;
        }
        if (gabi::ea(&i_this->eyePos) != 0) {
            gnd_monsSeStart_noActorCheck(i_this, mainFields(i_this)->mMode < 2 ? 0x488B : 0x488A);
        }
        break;
    }
    case 0xF: {
        anm_init(i_this, 0x30, 3.0f, 0, 1.0f, 0x12);
        mainFields(i_this)->mMode = mainFields(i_this)->mMode + 1;
        gnd_monsSeStart_noActorCheck(i_this, 0x494F);
        i_this->speed.y = REG0_F(5) + 33.0f;
        f32 x = i_this->current.pos.x;
        f32 sp = REG0_F(6) + 30.0f;
        f32 z = i_this->current.pos.z;
        i_this->speedF = sp;
        mainFields(i_this)->mTargetSpeed = sp;
        i_this->current.angle.y = cM_atan2s(-x, -z);
        s16 t = 0;
        if (cM_rndF(1.0f) < 0.5f)
            t = 100;
        mainFields(i_this)->mWaitTimer = t;
        mainFields(i_this)->mGuard.value = 0;
        splash_set(i_this);
        mainFields(i_this)->mLanded.value = 0;
        break;
    }
    case 0x10:
        i_this->gravity = REG0_F(7) + -2.0f;
        if (checkGround(i_this, 0.0f)) {
            if (mainFields(i_this)->mLanded.signedValue == 0) {
                splash_set(i_this);
                mainFields(i_this)->mLanded.value = 1;
            }
            mDoExt_McaMorf* morf = i_this->mpMorf;
            mainFields(i_this)->mTargetSpeed = 0.0f;
            if (gnd_morfIsStop(morf)) {
                end = (mainFields(i_this)->mWaitTimer != 0) ? 1 : 2;
            }
        } else {
            mainFields(i_this)->mGuard.value = 0;
        }
        break;
    case 0x14:
        anm_init(i_this, 0x56, 3.0f, 0, 1.0f, -1);
        mainFields(i_this)->mMode = mainFields(i_this)->mMode + 1;
        break;
    case 0x15:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            end = 1;
        } else if (checkGround(i_this, 0.0f)) {
            mainFields(i_this)->mTargetSpeed = 0.0f;
            anm_init(i_this, 0x31, 3.0f, 0, 1.0f, -1);
            mainFields(i_this)->mMode = mainFields(i_this)->mMode + 1;
        }
        break;
    case 0x16:
        if (gnd_morfIsStop(i_this->mpMorf))
            end = 1;
        break;
    }

    if (mainFields(i_this)->mDodge == 5) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, mainFields(i_this)->mDodgeAngle, 2,
                            (s16)(REG0_S(8) + 0x2000));
    } else {
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        cLib_addCalcAngleS2(&i_this->shape_angle.y, ang, 2, (s16)(REG0_S(8) + 0x2000));
    }
    mainFields(i_this)->mSwordAttackLeft = 10;
    mainFields(i_this)->mSwordAttackRight = 11;
    pos_move(i_this, 1);
    if (end != 0) {
        i_this->current.angle.y = i_this->shape_angle.y;
        if (end == 1) {
            mainFields(i_this)->mAction = 0;
            mainFields(i_this)->mMode = 0;
        } else {
            mainFields(i_this)->mAction = 1;
            anm_init(i_this, 0x2B, 5.0f, 0, 1.0f, 0xF);
            mainFields(i_this)->mMode = 8;
        }
    }
}

/* 02158E80 */
void finish(gnd_class* i_this) {
    dComIfGp_getPlayer(0); /* fetched, not used */
    s16 mode = mainFields(i_this)->mMode;
    mainFields(i_this)->mDamageTimer = 5;
    mainFields(i_this)->mGuard.value = 0;
    switch ((u32)(s32)mode) {
    case 0:
        mainFields(i_this)->mCameraCut = 100;
        mainFields(i_this)->mMode = 1;
        break;
    case 1:
        mainFields(i_this)->mMode = 2;
        break;
    case 2:
        mainFields(i_this)->mMode = 3;
        break;
    }
}

/* 02158F10 */
void damage(gnd_class* i_this) {
    dComIfGp_getPlayer(0); /* fetched, not used */
    s8 req = mainFields(i_this)->mDamageRequest;
    mainFields(i_this)->mGuard.value = 0;
    if (req != 0) {
        if (req == 1) {
            mainFields(i_this)->mGuard.value = 0;
            mainFields(i_this)->mMode = 0;
            mainFields(i_this)->mAction = 4;
            mainFields(i_this)->mDamageRequest = 0;
            return;
        }
        mainFields(i_this)->mDamageRequest = 0;
        mainFields(i_this)->mCameraCut = 10;
        return;
    }
    s16 mode = mainFields(i_this)->mMode;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x38, 2.0f, 0, 1.0f, -1);
        mainFields(i_this)->mMode = 1;
        mainFields(i_this)->mActionTimer = mainHIO()->mDamageWait;
        gnd_monsSeStart(i_this, 0x4952, 0);
        /* fall through */
    case 1:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x39, 2.0f, 2, 1.0f, -1);
            mainFields(i_this)->mMode = 9;
            mainFields(i_this)->mWaitTimer = 10;
        }
        break;
    case 5:
        anm_init(i_this, 0x3A, 2.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4951, 0);
        mainFields(i_this)->mMode = 6;
        mainFields(i_this)->mActionTimer = mainHIO()->mDamageWait;
        /* fall through */
    case 6:
        if (gnd_morfIsStop(i_this->mpMorf)) {
            anm_init(i_this, 0x3B, 2.0f, 2, 1.0f, -1);
            mainFields(i_this)->mMode = 10;
            mainFields(i_this)->mWaitTimer = 15;
        }
        break;
    case 9:
    case 10: {
        if (mainFields(i_this)->mWaitTimer == 0) {
            gnd_monsSeStart(i_this, 0x4953, 0);
            mainFields(i_this)->mWaitTimer = (mode == 9) ? 0x14 : 0x1E;
        }
        bool recover;
        if (mainFields(i_this)->mCameraCut == 0) {
            if (mainFields(i_this)->mActionTimer >= 0x1E)
                break;
            if ((s8)i_this->health < 0x19 && mainFields(i_this)->mBattlePhase.signedValue == 2) {
                s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                if (player_view_check(i_this, ang) &&
                    fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0)) <
                        mainHIO()->mRecoveryDistance) {
                    mainFields(i_this)->mMode = 0;
                    mainFields(i_this)->mGuard.value = 1;
                    mainFields(i_this)->mAction = 5;
                    mainFields(i_this)->mHitTimer = 0x1E;
                    return;
                }
            }
        }
        recover = mainFields(i_this)->mActionTimer == 0;
        if (recover) {
            mainFields(i_this)->mAction = 0;
            mainFields(i_this)->mMode = 0;
            mainFields(i_this)->mHitTimer = 0x1E;
            mainFields(i_this)->mGuard.value = 1;
        }
        break;
    }
    case 0x14:
        anm_init(i_this, 0x3C, 3.0f, 0, 1.0f, -1);
        gnd_monsSeStart(i_this, 0x4950, 0);
        mainFields(i_this)->mMode = 10;
        break;
    }
    pos_move(i_this, 1);
    mainFields(i_this)->mTargetSpeed = 0.0f;
}

/* 021594B0 */
void damage_check(gnd_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mStts.Move();
    /* the sword: parried (Tg) or hitting (At) */
    for (int i = 0; i < 2; i++) {
        dCcD_Sph* sph = &i_this->mWeponSph[i];
        if (sph->ChkTgHit()) {
            gnd_def_se_set(i_this, sph->GetTgHitObj(), 0x40);
            mainFields(i_this)->mKnockbackSpeed = REG0_F(19) + 30.0f;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            u8 n = mainFields(i_this)->mGuardHits.value;
            mainFields(i_this)->mGuardHit.value = 1;
            mainFields(i_this)->mKnockbackAngle = ang + 0x8000;
            mainFields(i_this)->mGuardHits.value = n + 1;
            gabi::Local<cXyz> hit;
            cXyz* hitPos = sph->GetTgHitPosP();
            hit->x = hitPos->x;
            hit->y = hitPos->y;
            hit->z = hitPos->z;
            gnd_SordFlush_set(hit, 0);
            gabi::Local<cXyz> scale;
            scale->z = 2.0f;
            scale->x = 2.0f;
            scale->y = 2.0f;
            dComIfGp_particle_set(0xC, hitPos, &i_this->shape_angle, scale);
            dComIfGp_particle_set(0x8380, hitPos, &i_this->shape_angle);
        }
        if (sph->ChkAtHit()) {
            void* obj = gnd_GetAtHitObj(sph);
            if (obj != nullptr) {
                u32 stts = gabi::at<dCcD_GObjInf>(gabi::ea(obj))->mStts.v;
                if (stts == 0)
                    continue;
                fopAc_ac_c* ac = gabi::at<dCcD_Stts>(stts)->mp_actor.get();
                if (ac != nullptr && fpcM_GetName(ac) == 0xA8) {
                    if (mainFields(i_this)->mMode == 0x16) {
                        mainFields(i_this)->mSwordBlocked = 1;
                        gabi::Local<cXyz> vpos;
                        u32 vib = gabi::ea(dComIfGp_getVibration());
                        s16 reg = REG0_S(2);
                        vpos->x = 0.0f;
                        vpos->y = 1.0f;
                        vpos->z = 0.0f;
                        gndMain_startShock(vib, reg + 6, -0x21, vpos.get());
                        continue;
                    }
                    if (daPy_vfunc3C(player)) {
                        gabi::Local<cXyz> vpos;
                        u32 vib = gabi::ea(dComIfGp_getVibration());
                        vpos->x = 0.0f;
                        s16 reg = REG0_S(2);
                        vpos->y = 1.0f;
                        vpos->z = 0.0f;
                        gndMain_startShock(vib, reg + 3, -0x21, vpos.get());
                        s16 mode = mainFields(i_this)->mMode;
                        if (mode == 2 || mode == 7) {
                            mainFields(i_this)->mSwordBlocked = 1;
                        }
                    }
                }
            }
        }
    }
    if (mainFields(i_this)->mDamageTimer != 0)
        return;

    /* the chest */
    if (i_this->mChestSph.ChkTgHit()) {
        void* obj = i_this->mChestSph.GetTgHitObj();
        if (obj == nullptr)
            return;
        u32 stts = gabi::at<dCcD_GObjInf>(gabi::ea(obj))->mStts.v;
        bool toBody = false;
        if (stts == 0) {
            toBody = mainFields(i_this)->mGuard.signedValue == 0;
        } else {
            fopAc_ac_c* ac = gabi::at<dCcD_Stts>(stts)->mp_actor.get();
            if (ac != nullptr && fpcM_GetName(ac) == 0x1D8 && mainPlayer(ac)->mCutType != 0) {
                s16 d = i_this->shape_angle.y - ac->current.angle.y;
                if (d < 0)
                    d = -d;
                if (((u16)d < 0x4000 && mainFields(i_this)->mBattlePhase.signedValue < 2) ||
                    gabi::at<gndMainArrow>(gabi::ea(ac))->mReflected != 0) {
                    toBody = true;
                } else {
                    mainFields(i_this)->mGuard.value = 2;
                    mainFields(i_this)->mAction = 10;
                    mainFields(i_this)->mMode = 15;
                    mainFields(i_this)->mGuardHits.value = 0;
                    return;
                }
            } else {
                toBody = mainFields(i_this)->mGuard.signedValue == 0;
            }
        }
        if (!toBody) {
            s8 guard = mainFields(i_this)->mGuard.signedValue;
            mainFields(i_this)->mAction = 10;
            if (guard != 1) {
                mainFields(i_this)->mGuardHits.value = 0;
                mainFields(i_this)->mMode = 15;
                return;
            }
            gnd_def_se_set(i_this, i_this->mChestSph.GetTgHitObj(), 0x40);
            mainFields(i_this)->mKnockbackSpeed = REG0_F(19) + 30.0f;
            s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
            gabi::Local<cXyz> hit;
            cXyz* hitPos = i_this->mChestSph.GetTgHitPosP();
            hit->y = hitPos->y;
            hit->z = hitPos->z;
            mainFields(i_this)->mKnockbackAngle = ang + 0x8000;
            hit->x = hitPos->x;
            gnd_SordFlush_set(hit, 0);
            gabi::Local<cXyz> scale;
            scale->x = 2.0f;
            scale->y = 2.0f;
            scale->z = 2.0f;
            dComIfGp_particle_set(0xC, hitPos, &i_this->shape_angle, scale);
            dComIfGp_particle_set(0x8380, hitPos, &i_this->shape_angle);
            gnd_monsSeStart(i_this, 0x494E, 0);
            int anm = 0x2F;
            if (cM_rndF(1.0f) < 0.5f)
                anm = 0x2E;
            anm_init(i_this, anm, 2.0f, 0, 1.0f, -1);
            mainFields(i_this)->mActionTimer = 0x14;
            mainFields(i_this)->mMode = 10;
            return;
        }
    }

    /* the body */
    if (mainFields(i_this)->mHitTimer != 0 && !i_this->mChestSph.ChkTgHit())
        return;
    bool head = false;
    if (!i_this->mCyl.ChkTgHit() && !i_this->mHeadSph.ChkTgHit() && !i_this->mChestSph.ChkTgHit())
        return;
    mainFields(i_this)->mDamageTimer = 7;
    bool chest = false;
    gabi::Local<CcAtInfo_gnd> info;
    fopAc_ac_c* hitAc;
    if (i_this->mHeadSph.ChkTgHit()) {
        info->mpObj = gabi::ea(i_this->mHeadSph.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mHeadSph.GetTgHitPosP());
        head = true;
    } else if (i_this->mChestSph.ChkTgHit()) {
        info->mpObj = gabi::ea(i_this->mChestSph.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mChestSph.GetTgHitPosP());
        chest = true;
    } else {
        info->mpObj = gabi::ea(i_this->mCyl.GetTgHitObj());
        info->pParticlePos = gabi::ea(i_this->mCyl.GetTgHitPosP());
    }
    hitAc = gnd_cc_at_check(i_this, info);
    info->mpActor = gabi::ea(hitAc);
    if (info->mResultingAttackType == 1 &&
        gabi::at<gndMainPlayerState>(gabi::ea(player))->mSwordAttack != 0) {
        mainFields(i_this)->mDamageTimer = 2;
    }
    if (chest && hitAc != nullptr) {
        /* light arrow in the chest: flash */
        dComIfGp_particle_set(0x8382, gabi::at<cXyz>(info->pParticlePos), &hitAc->current.angle);
        fopAc_ac_c* ac2 = gabi::at<fopAc_ac_c>(info->mpActor);
        dComIfGp_particle_set(0x8383, gabi::at<cXyz>(info->pParticlePos), &ac2->current.angle);
        mainFields(i_this)->mFlashFrame = 1;
        mainFields(i_this)->mEffectTimer = REG_S(8, 4) + 0x1E;
        for (int k = 0; k < 15; k++) {
            cXyz* p = &mainFields(i_this)->mFlashOffsets[k];
            p->x = cM_rndFX(30.0f);
            p->y = cM_rndFX(30.0f);
            p->z = cM_rndFX(30.0f);
        }
        mainFields(i_this)->mCameraShake = REG0_F(19) + 20.0f;
    } else {
        u8 t = mainPlayer(player)->mCutType;
        if (t == 5 || t == 0xF) {
            dComIfGp_particle_set(0x8381, gabi::at<cXyz>(info->pParticlePos), &i_this->shape_angle);
        } else {
            JPABaseEmitter* emitter = dComIfGp_particle_set(
                0x837F, gabi::at<cXyz>(info->pParticlePos), &i_this->shape_angle);
            if (emitter != nullptr)
                gnd_emitterColor(i_this, emitter);
        }
    }
    /* health */
    s8 h = i_this->health;
    bool toEnd = false; /* E/F selection below */
    if (h > 0x19) {
        if (h <= 0x32 && mainFields(i_this)->mBattlePhase.signedValue == 0) {
            fopAc_ac_c* act = gabi::at<fopAc_ac_c>(info->mpActor);
            if (act != nullptr && fpcM_GetName(act) == 0x1D8) {
                if (mainPlayer(act)->mCutType != 0) {
                    mainFields(i_this)->mDamageRequest = 1;
                    if ((s8)i_this->health < 0)
                        i_this->health = 0;
                }
            } else {
                i_this->health = 0x32;
            }
        }
    } else {
        if (mainFields(i_this)->mBattlePhase.signedValue == 1) {
            i_this->health = 0x19;
            mainFields(i_this)->mBattlePhase.signedValue = 2;
            mainFields(i_this)->mDamageRequest = 2;
        } else if (h < 0) {
            i_this->health = 0;
        }
    }
    (void)toEnd;
    if (mainFields(i_this)->mAction == 0xB) {
        mainFields(i_this)->mMode = 0x14;
    } else {
        mainFields(i_this)->mAction = 0xB;
        mainFields(i_this)->mMode = head ? 5 : 0;
        mainFields(i_this)->mGuard.value = 0;
    }
    gnd_monsSeStart_noActorCheck(i_this, 0x4809);
}

/* 02154E4C */
void* shot_s_sub(void* a, void*) {
    BOOL actor = fopAc_IsActor(a);
    if (a != nullptr) {
        s16 name = fpcM_GetName(a);
        if ((actor && name == 0x1BE) || name == 0x1B0 || name == 0xA9 || name == 0x1D8) {
            if (!(((fopAc_ac_c*)a)->speedF < 10.0f)) {
                return a;
            }
        }
    }
    return nullptr;
}

/* 02159F9C */
void demowait(gnd_class* i_this) {
    mainFields(i_this)->mDamageTimer = 5;
    mainFields(i_this)->mGuard.value = 0;
}

/* 02159FB0 */
void yawait(gnd_class* i_this) {
    mainFields(i_this)->mGuard.value = 0;
    i_this->current.pos.y = 0.0f;
}

/* 0215CE60 */
void body_flash(gnd_class* i_this) {
    s16 t = mainFields(i_this)->mFlashFrame;
    J3DModel* model = i_this->mpMorf->getModel();
    if (t != 0) {
        t = t + 1;
        if (t > 100) {
            t = 0;
        }
        mainFields(i_this)->mFlashFrame = t;
    }

    for (int i = 0; i < 15; i++) {
        if ((u32)(s32)t == l_flashFrames[i]) {
            mainFields(i_this)->mFlashTimers[i] = 0x63;
        } else {
            s16 timer = mainFields(i_this)->mFlashTimers[i];
            if (timer == 0) {
                u32 e = mainFields(i_this)->mFlashEmitters[i];
                if (e != 0) {
                    gnd_becomeInvalidEmitter(e);
                    mainFields(i_this)->mFlashEmitters[i] = 0;
                }
                t = mainFields(i_this)->mFlashFrame;
                continue;
            }
            mainFields(i_this)->mFlashTimers[i] = timer - 1;
        }
        s32 jnt = l_flashJoints[i];
        PSMTXCopy(gnd_getAnmMtx(model, jnt), calc_mtx());
        gabi::Local<cXyz> pos;
        MtxPosition(&mainFields(i_this)->mFlashOffsets[i], pos);
        f32 timer = (f32)(s16)mainFields(i_this)->mFlashTimers[i];
        f32 k = REG_F(8, 0) + 0.01f;
        f32 scale = l_flashScales[i] * timer * k;
        u32 e = mainFields(i_this)->mFlashEmitters[i];
        if (e == 0) {
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x3ED, pos);
            mainFields(i_this)->mFlashEmitters[i] = gabi::ea(emitter);
        } else {
            u8 v = mainEmitter(e)->mDrawType;
            f32 y = pos->y;
            f32 x = pos->x;
            f32 z = pos->z;
            if (v >= 7)
                y = -y;
            mainEmitter(e)->mGlobalTranslation.x = x;
            mainEmitter(e)->mGlobalTranslation.y = y;
            mainEmitter(e)->mGlobalTranslation.z = z;
            e = mainFields(i_this)->mFlashEmitters[i];
            mainEmitter(e)->mGlobalScale.x = scale;
            mainEmitter(e)->mGlobalScale.y = scale;
            mainEmitter(e)->mGlobalScale.z = scale;
            mainEmitter(e)->mParticleScale.x = scale;
            mainEmitter(e)->mParticleScale.y = scale;
            mainEmitter(e)->mParticleScale.z = scale;
        }
        t = mainFields(i_this)->mFlashFrame;
    }
}

/* 0215758C */
BOOL daGnd_IsDelete(gnd_class*) { return TRUE; }

/* 02157594 */
BOOL daGnd_Delete(gnd_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Gnd" /* "Gnd" */);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mRipple);
    if (i_this->mHIOInit) {
        s8 no = mainHIO()->mNo;
        l_hioInitialized = 0;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}

/* 02157600 */
BOOL useHeapInit(fopAc_ac_c* a_this) {
    gnd_class* i_this = (gnd_class*)a_this;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes("Gnd", GND_MODEL, GND_SAFESTRING_VTBL);
    void* anm = dComIfG_getObjectRes("Gnd", GND_INITIAL_ANIMATION, GND_SAFESTRING_VTBL);
    mDoExt_McaMorf* morf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, (J3DAnmTransform*)anm, 2, 1.0f,
                               0, -1, 1, nullptr, 0, 0x11020203);
    i_this->mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr)
        return FALSE;

    void* brk = operator_new(0x78);
    if (brk != nullptr)
        brk = gnd_brkAnm_ct(brk);
    i_this->mpBrk = (mDoExt_brkAnm*)brk;
    if (brk == nullptr)
        return FALSE;
    J3DModel* model = i_this->mpMorf->getModel();
    void* res = dComIfG_getObjectRes("Gnd", GND_COLOR_ANIMATION, GND_SAFESTRING_VTBL);
    if (!gnd_brkAnm_init(i_this->mpBrk, J3DModel_getModelData(model), res, 1, 2, 1.0f, 0, -1, false,
                         0))
        return FALSE;

    void* btk = operator_new(0x74);
    if (btk != nullptr)
        btk = gnd_btkAnm_ct(btk);
    i_this->mpBtk = (mDoExt_btkAnm*)btk;
    if (btk == nullptr)
        return FALSE;
    model = i_this->mpMorf->getModel();
    res = dComIfG_getObjectRes("Gnd", GND_TEXTURE_ANIMATION, GND_SAFESTRING_VTBL);
    if (!gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1, false,
                         0))
        return FALSE;

    void* btp = operator_new(0x74);
    if (btp != nullptr)
        btp = gnd_btpAnm_ct(btp);
    i_this->mpBtp = (mDoExt_btpAnm*)btp;
    if (btp == nullptr)
        return FALSE;
    model = i_this->mpMorf->getModel();
    res = dComIfG_getObjectRes("Gnd", GND_PATTERN_ANIMATION, GND_SAFESTRING_VTBL);
    if (!gnd_btpAnm_init(i_this->mpBtp, J3DModel_getModelData(model), res, 1, 2, 1.0f, 0, -1, false,
                         0))
        return FALSE;

    if (!mDoExt_3DlineMat0_init(i_this->mLineMat, 4, 0x14, FALSE))
        return FALSE;
    return TRUE;
}

/* 02157B30 */
cPhs_State daGnd_Create(fopAc_ac_c* a_this) {
    gnd_class* i_this = (gnd_class*)a_this;
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            gnd_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, "Gnd" /* "Gnd" */);
    if (ret == cPhs_COMPLEATE_e) {
        mainStatics()->mpZelda = 0;
        execFields(i_this)->mBgmParam = fopAcM_GetParam(a_this) & 0xF;
        if (!fopAcM_entrySolidHeap(a_this, 0x02157600 /* useHeapInit */, 0x96000)) {
            return cPhs_ERROR_e;
        }
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags */
        a_this->attention_info.distances[2] = 4;                   /* attention_info.distances[2] */
        if (l_hioInitialized == 0) {
            i_this->mHIOInit = 1;
            l_hioInitialized = 1;
            mainHIO()->mNo = mDoHIO_createChild("Kmc" /* "ガノン" */, mainHIO());
        }
        i_this->mBtHeight = REG0_F(6) + 400.0f;
        i_this->mBtBodyR = REG0_F(5) + 150.0f;
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                          &a_this->speed, nullptr, nullptr);
        mainFields(i_this)->mAcchFlags = 0;
        i_this->mAcchCir.SetWall(200.0f, 200.0f);
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        i_this->mCyl.Set(&cc_cyl_src /* cc_cyl_src */);
        i_this->mCyl.SetStts(&i_this->mStts);
        i_this->mHeadSph.Set(&head_sph_src /* head_sph_src */);
        i_this->mHeadSph.SetStts(&i_this->mStts);
        i_this->mChestSph.Set(&chest_sph_src /* chest_sph_src */);
        i_this->mChestSph.SetStts(&i_this->mStts);
        for (int i = 0; i < 2; i++) {
            i_this->mWeponSph[i].Set(&wepon_sph_src /* wepon_sph_src */);
            i_this->mWeponSph[i].SetStts(&i_this->mStts);
        }
        gndMain_execute(i_this); /* daGnd_Execute */
        i_this->max_health = 0x4B;
        i_this->health = 0x4B;
        if (execFields(i_this)->mBgmParam != 0) {
            mDoAud_bgmStart(0x8000004D);
        }
    }
    return ret;
}

// HD: compiler-generated and template code of this translation unit.

/* 0215788C */
gnd_class* gnd_class_ct(gnd_class* i_this) {
    if (i_this == nullptr) {
        i_this = (gnd_class*)operator_new(sizeof(gnd_class));
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    mainFields(i_this)->mpSelf = gabi::ea(i_this);
    i_this->__vtbl = GND_VTBL;
    // HD: the constructor reads the complete initialization record before storing its three copies.
    f32 f0[6];
    u8 b[4];
    s16 h[4];
    f32 f1[8];
    for (int k = 0; k < 6; k++)
        f0[k] = l_tevInit.mPositionValues[k];
    for (int k = 0; k < 4; k++)
        b[k] = l_tevInit.mColorValues[k];
    for (int k = 0; k < 4; k++)
        h[k] = l_tevInit.mAngleValues[k];
    for (int k = 0; k < 8; k++)
        f1[k] = l_tevInit.mLightValues[k];
    gndInitFields* init = gndInitialFields(i_this);
    gndInitRecord* dst[3] = {&init->mBattle, &init->mSword, &init->mBody};
    for (int n = 0; n < 3; n++) {
        for (int k = 0; k < 6; k++)
            dst[n]->mPositionValues[k] = f0[k];
        for (int k = 0; k < 4; k++)
            dst[n]->mColorValues[k] = b[k];
        for (int k = 0; k < 4; k++)
            dst[n]->mAngleValues[k] = h[k];
        for (int k = 0; k < 8; k++)
            dst[n]->mLightValues[k] = f1[k];
    }
    mDoExt_3DlineMat0_ct(i_this->mLineMat);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    /* dBgS_ObjAcch (inline): this TU's vtables */
    gndMain_acchConstruct(&i_this->mAcch);
    mainFields(i_this)->mAcchVtable = 0x10010024;
    mainFields(i_this)->mGroupPassVtable = 0x10010034;
    mainFields(i_this)->mPolyPassVtable = 0x10010044;
    mainFields(i_this)->mPolyPass = 1;
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, 0x10010014);
    gnd_Sph_ct(&i_this->mHeadSph);
    gnd_Sph_ct(&i_this->mChestSph);
    gnd_construct_array(i_this->mWeponSph, 2, sizeof(dCcD_Sph), 0x025166F0);
    gnd_rippleEcallBack_ct(i_this->mRipple);
    return i_this;
}

/* 02157F10 */
void __sinit_d_a_gnd_cpp() {
    // HD: GHS initializes the translation unit globals in this order.
    for (int i = 3; i >= 0; i--)
        mainStatics()->mZeroWords[i] = 0;
    __register_global_object(0x101B58F8);
    mainStatics()->mAngleMin = -3.1415927f;
    mainStatics()->mAngleMax = 3.1415927f;
    gndMain_globalByteConstruct(gabi::ea(&mainStatics()->mConstructedByte));
    __register_global_object(0x101B5904);
    gndMain_globalByteDestruct(gabi::ea(&mainStatics()->mDestructedByte));
    __register_global_object(0x101B5910);
    /* a cXyz (-20000, -20000, -20000) */
    mainStatics()->mInactiveSwordPos.x = -20000.0f;
    mainStatics()->mInactiveSwordPos.y = -20000.0f;
    mainStatics()->mInactiveSwordPos.z = -20000.0f;
    daGnd_HIO_ct(mainHIO());

    for (int k = 0; k < 15; k++)
        mainStatics()->mCameraCenters[k] = l_cameraCenterInit[k];
    const u8 b_src[15] = {0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    for (int k = 0; k < 15; k++)
        mainStatics()->mCameraEyes[k] = l_cameraEyeInit[b_src[k]];
}

/* 02158120 */
void gnd_SafeString_dt(void* str, s32 flags) {
    if (str != nullptr && (flags & 1))
        operator_delete(str);
}

/* 0215D188 */
void gnd_class_dt(gnd_class* i_this, s32 flags) {
    if (i_this == nullptr)
        return;
    gnd_destroy_arr(i_this->mWeponSph, 2, 0x12C, 0x02515AE8, 0, 0);
    gnd_Sph_dt(&i_this->mChestSph, 2);
    gnd_Sph_dt(&i_this->mHeadSph, 2);
    dCcD_Cyl_dt(&i_this->mCyl, 2);
    dCcD_Stts_dt(&i_this->mStts, 2);
    /* dBgS_Acch::~dBgS_Acch (inline): this TU's vtables, then the base destructor */
    mainFields(i_this)->mGroupPassVtable = 0x10010034;
    mainFields(i_this)->mPolyPassVtable = 0x10010044;
    gndMain_acchDestruct(&i_this->mAcch, 0);
    /* dBgS_AcchCir::~dBgS_AcchCir (inline): its cM3dGCir member */
    gndMain_circleDestruct(&i_this->mAcchCir.m_cir, 2);
    mDoExt_3DlineMat0_dt(i_this->mLineMat, 2);
    gndMain_actorDestruct(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(i_this);
}

/* 0215D268 */
void gnd_SafeString_assureTermination(void*) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): Ganondorf main fragment
str 10010090 in 02158360
str 10010084 in 02158360
data l_hioInitialized 101B54D8
data cc_cyl_src 101B58B4
data head_sph_src 101B57F4
data chest_sph_src 101B5834
data wepon_sph_src 101B5874
data l_hairColor 101B57C4
data l_cameraCenterInit 1001022C
data l_cameraEyeInit 10010268
data l_effectAnimations 100100C8
data l_effectIDs 101B5614
data l_flashFrames 101B559C
data l_flashJoints 101B5560
data l_flashScales 101B55D8
data l_hairJoints 101B54F4
data l_hairOffsets 101B5504
data l_defenceAnimations 101B5514
str 100100C4 in 02154228
str 100101D8 in 02157594
str 100101DC in 02157600
str 100101E0 in 02157B30
str 100101E4 in 02157B30
str 10010138 in 02154810
data l_tevInit 1016E414
ext 024EFD9C void gndMain_acchDestruct(dBgS_Acch* acch, s32 flags)
ext 02018034 void gndMain_circleDestruct(void* circle, s32 flags)
ext 025D50BC void gndMain_actorDestruct(gnd_class* actor, s32 flags)
ext 024F0474 void gndMain_acchConstruct(dBgS_Acch* acch)
ext 028ED6F8 void gndMain_globalByteConstruct(s32 address)
ext 028EAB2C void gndMain_globalByteDestruct(s32 address)
ext 02154EE0 void gndMain_execute(gnd_class* actor)
ext 025E1AA4 void gndMain_monsSeStart(u32 id, cXyz* pos, u32 actorID, s32 param, s32 reverb)
ext 025CB374 BOOL gndMain_startShock(u32 vibration, s32 strength, s32 length, cXyz* pos)
ext 027EC9E8 void gndMain_report(s32 x, s32 y, u32 fmt, s32 value)
*/
// clang-format on
