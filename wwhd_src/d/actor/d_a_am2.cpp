#include "d/actor/d_a_am2.h"

// Armos behavior and rendering. HD embeds the action helpers in Execute.
enum Action {
    ACTION_DOUSA = 0,
    ACTION_MAHI = 1,
    ACTION_ITAI = 2,
    ACTION_HANDOU_MOVE = 3,
    ACTION_MODORU_MOVE = 4,
};

enum {
    dRes_INDEX_AM2_BCK_BURUBURU_e = 6,
    dRes_INDEX_AM2_BCK_DAMAGE_e = 7,
    dRes_INDEX_AM2_BCK_DEAD1_e = 8,
    dRes_INDEX_AM2_BCK_DEAD2_e = 9,
    dRes_INDEX_AM2_BCK_DEAD3_e = 10,
    dRes_INDEX_AM2_BCK_JUMP_e = 11,
    dRes_INDEX_AM2_BCK_MAHI_e = 12,
    dRes_INDEX_AM2_BCK_SLEEP_e = 13,
    dRes_INDEX_AM2_BCK_START_e = 14,
    dRes_INDEX_AM2_BCK_WAIT_e = 15,
    dRes_INDEX_AM2_BDL_AM2_e = 18,
    dRes_INDEX_AM2_BRK_AM2_e = 21,
    dRes_INDEX_AM2_BTK_AM2_e = 24,
};

static Am2SphereSource eye_co_sph_src = {0, 0x1000, 0, 0,    0,    0,    0,    0x81c4000, 3,
                                         0, 0,      0, 0,    0,    0,    0,    0,         0,
                                         0, 6,      0, 0.0f, 0.0f, 0.0f, 15.0f};
static Am2SphereSource week_co_sph_src = {0, 0x1000, 0, 0,    0,    0,    0,    0xff1dfeff, 3,
                                          0, 0,      0, 0,    0,    0,    0,    0,          0,
                                          0, 6,      0, 0.0f, 0.0f, 0.0f, 15.0f};
static Am2CylinderSource body_co_cyl_src = {0,   0x1000, 0, 0,    0,    0,    0,     0xff1dfeff, 3,
                                            117, 0,      0, 0,    0,    0,    0,     12,         0,
                                            0,   3,      0, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f};
static Am2CylinderSource sword_co_cyl_src = {0,  0x1000, 1, 0,    0,    0,    5,     0,   0,
                                             97, 0,      0, 6,    0,    0,    0,     12,  0,
                                             0,  3,      0, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f};
static Am2JointOffset cyl_offset_1[] = {{80.0f, -25.0f, 0.0f}, {80.0f, -30.0f, 0.0f}};
static Am2JointOffset sph_offset[] = {{50.0f, 30.0f, 0.0f}};
static Am2JointHitData search_data[] = {{2, 0, 20.0f, 0x1018FA6C}, {1, 0, 20.0f, 0x1018FA84}};

const gc_tu_data gc_tu = {.safeString = 0x10006F74};

// HD: the archive is named "Am2" rather than the GameCube "AM2".
static void* am2ObjectResource(s32 index, const char* archive = "Am2") {
    // Reserve the linkage prefix for actual nonleaf callees before the SafeString.
    gabi::Local<SafeString> name;
    gabi::Local<u64> linkage;
    name->mStringTop = gabi::guest_addr(archive);
    name->__vtbl = 0x10006F74;
    dRes_control_c* control = dComIfG_resControl();
    return am2ResourceGet(control, name.get(), index);
}

/* 0204C2C4 */
static BOOL nodeCallBack(J3DNode* node, s32 timing) {
    if (timing == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<am2_class*>(model->getUserArea());
        u16 number = reinterpret_cast<Am2JointView*>(joint)->mJntNo;
        if (actor) {
            J3DMtxBuffer_hd* matrixBlock = model->mpMtxBuffer;
            u32 matrices = matrixBlock->mpAnmMtx;
            matrixBlock->mFlags = matrixBlock->mFlags | 0x10;
            u32 offset = number * 0x30;
            Mtx34* current = calc_mtx();
            MTXCopy(gabi::at<Mtx34>(matrices + offset), current);
            if (number >= 1 && number <= 3) {
                gabi::Local<cXyz> local;
                gabi::Local<u64> linkage;
                local->x = number == 1 ? 0.0f : number == 2 ? 30.0f : -10.0f;
                local->y = number == 1 ? 50.0f : 0.0f;
                local->z = number == 1 ? -40.0f : 0.0f;
                cXyz* destination = number == 1   ? &actor->mWeakPos
                                    : number == 2 ? &actor->mEyeballPos
                                                  : &actor->mNeedlePos;
                MtxPosition(local.get(), destination);
            }
            matrixBlock = model->mpMtxBuffer;
            current = calc_mtx();
            u16 flags = matrixBlock->mFlags;
            matrices = matrixBlock->mpAnmMtx;
            matrixBlock->mFlags = flags | 0x10;
            mtx_copy(gabi::at<Mtx34>(matrices + offset), current);
            current = calc_mtx();
            MTXCopy(current, J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 0204C4E4 */
static void draw_SUB(am2_class* actor) {
    J3DModel* model = actor->mpMorf->getModel();
    // HD: load all scale components, then store X, Z, Y.
    f32 y = actor->scale.y, x = actor->scale.x, z = actor->scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    Mtx34* matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_YrotM(matrix, actor->shape_angle.y);
    mDoMtx_XrotM(matrix, actor->shape_angle.x);
    mDoMtx_ZrotM(matrix, actor->shape_angle.z);
    model->setBaseTRMtx(matrix);
    actor->mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
}

/* 0204C5F8 */
static BOOL daAM2_Draw(am2_class* actor) {
    J3DModel* model = actor->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
    dSnap_RegistFig(0xB6, actor, 1.0f, 1.0f, 1.0f);
    mDoExt_brkAnm* brk = actor->mpBrkAnm;
    J3DModelData* data = model->getModelData();
    f32 frame = brk->getFrame();
    brk->entry(data, frame);
    mDoExt_btkAnm* btk = actor->mpBtkAnm;
    data = model->getModelData();
    frame = btk->getFrame();
    btk->entry(data, frame);
    actor->mpMorf->entryDL();
    mDoExt_brkAnm_remove(model->getModelData());
    mDoExt_btkAnm_remove(model->getModelData());
    // HD: there is no simple-shadow pass here.
    return TRUE;
}

/* 0204C69C */
static void anm_init(am2_class* actor, s32 bck, f32 morph, u8 loop, f32 speed, s32 sound) {
    actor->mCurrBckIdx = bck;
    auto* animation = static_cast<J3DAnmTransform*>(am2ObjectResource(bck));
    void* audio = sound >= 0 ? am2ObjectResource(sound) : nullptr;
    actor->mpMorf->setAnm(animation, loop, morph, speed, 0.0f, -1.0f, audio);
}

/* 0204CED4 */
static void BG_check(am2_class* actor) {
    f32 height = am2Debug().mAcchHeightAdjust + 40.0f;
    actor->mAcchCir.SetWall(height, actor->mAcchRadius);
    f32 correction = actor->mCorrectionOffsetY;
    actor->current.pos.y = (f32)actor->current.pos.y - correction;
    actor->old.pos.y = (f32)actor->old.pos.y - correction;
    dComIfG_inf_c* play = am2GameInfo();
    actor->mAcch.CrrPos(&play->play.mBgS);
    f32 y = actor->current.pos.y;
    correction = actor->mCorrectionOffsetY;
    f32 oldY = actor->old.pos.y;
    actor->current.pos.y = y + correction;
    actor->old.pos.y = oldY + correction;
}

/* 0204F9C8 */
static BOOL daAM2_IsDelete(am2_class* actor) { return TRUE; }

/* 0204F9D0 */
static BOOL daAM2_Delete(am2_class* actor) {
    dComIfG_resDelete(&actor->mPhase, "Am2");
    actor->mSmokeCb.remove();
    actor->mRippleCb.end();
    return TRUE;
}

static void am2Sound(am2_class* actor, u32 id) {
    if (actor && gabi::ea(&actor->eyePos)) {
        s32 reverb = dComIfGp_getReverb((s8)actor->current.roomNo);
        mDoAud_seStart(id, &actor->eyePos, 0x42, reverb);
    }
}
/* 0204CCCC */
static BOOL body_atari_check(am2_class* actor) {
    dComIfG_inf_c* play = am2GameInfo();
    auto* player = static_cast<Am2PlayerView*>(play->play.mPlayerInfo[0].mpPlayer.get());
    actor->mStts.Move();
    actor->mHitDirection = 0;
    if (!actor->mBodyCyl.ChkTgHit())
        return FALSE;
    auto* hit = static_cast<dCcD_GObjInf*>(actor->mBodyCyl.GetTgHitObj());
    if (!hit)
        return FALSE;
    u32 type = hit->mAtType;
    if (type == 2) {
        am2Sound(actor, 0x2803);
        return TRUE;
    }
    if (type == 0x40 || type == 0x80) {
        am2Sound(actor, 0x2833);
        return TRUE;
    }
    if (type == 0x10000) {
        am2Sound(actor, 0x2855);
        if (actor->mAction != ACTION_HANDOU_MOVE && actor->mAction != ACTION_ITAI) {
            actor->mAction = ACTION_HANDOU_MOVE;
            actor->mMode = 30;
            actor->mHitDirection = 7;
            if (player->mCutType == 0x11)
                actor->mHitDirection = 8;
        }
    } else
        am2Sound(actor, 0x2834);
    return TRUE;
}

/* 0204FCDC */
static void* am2_ctor(void* storage) {
    auto* actor = static_cast<am2_class*>(storage);
    if (!actor) {
        actor = static_cast<am2_class*>(operator_new(sizeof(am2_class)));
        if (!actor)
            return nullptr;
    }
    am2ActorConstruct(actor);
    actor->__vtbl = 0x1000702C;
    dBgS_AcchCir_ct(&actor->mAcchCir);
    am2AcchConstruct(&actor->mAcch);
    actor->mAcch.cBgS_Chk::__vtbl = 0x10006FBC;
    actor->mAcch.dBgS_GrpPassChk::__vtbl = 0x10006FCC;
    actor->mAcch.dBgS_PolyPassChk::__vtbl = 0x10006FDC;
    actor->mAcch.mbObjThrough = 1;
    am2StatusBaseConstruct(&actor->mStts);
    am2GameStatusConstruct(&actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = 0x1004AE88;
    actor->mStts.__vtbl_gstts = 0x1004AEC0;
    am2CollisionConstruct(&actor->mBodyCyl);
    am2Shape(actor->mBodyCyl).mShapeVtable = 0x100015A8;
    am2Shape(actor->mBodyCyl).mAabVtable = 0x10006F8C;
    am2CylinderConstruct(&actor->mBodyCyl.mCyl);
    actor->mBodyCyl.__vtbl_hitinf = 0x1004B108;
    actor->mBodyCyl.mCyl.__vtbl = 0x1004B150;
    am2Shape(actor->mBodyCyl).mShapeVtable = 0x1004B160;
    am2CollisionConstruct(&actor->mNeedleCyl);
    am2Shape(actor->mNeedleCyl).mShapeVtable = 0x100015A8;
    am2Shape(actor->mNeedleCyl).mAabVtable = 0x10006F8C;
    am2CylinderConstruct(&actor->mNeedleCyl.mCyl);
    actor->mNeedleCyl.__vtbl_hitinf = 0x1004B108;
    actor->mNeedleCyl.mCyl.__vtbl = 0x1004B150;
    am2Shape(actor->mNeedleCyl).mShapeVtable = 0x1004B160;
    am2SphereConstruct(&actor->mEyeSph);
    am2SphereConstruct(&actor->mWeakSph);
    am2SmokeConstruct(&actor->mSmokeCb);
    am2RippleConstruct(&actor->mRippleCb);
    am2StatusBaseConstruct(&actor->mEnemyIce.mStts);
    am2GameStatusConstruct(&actor->mEnemyIce.mStts.__vtbl_gstts);
    actor->mEnemyIce.mStts.__vtbl = 0x1004AE88;
    actor->mEnemyIce.mStts.__vtbl_gstts = 0x1004AEC0;
    am2CollisionConstruct(&actor->mEnemyIce.mCyl);
    am2Shape(actor->mEnemyIce.mCyl).mAabVtable = 0x10006F8C;
    am2Shape(actor->mEnemyIce.mCyl).mShapeVtable = 0x100015A8;
    am2CylinderConstruct(&actor->mEnemyIce.mCyl.mCyl);
    am2Shape(actor->mEnemyIce.mCyl).mShapeVtable = 0x1004B160;
    actor->mEnemyIce.mCyl.mCyl.__vtbl = 0x1004B150;
    actor->mEnemyIce.mCyl.__vtbl_hitinf = 0x1004B108;
    dBgS_AcchCir_ct(&actor->mEnemyIce.mBgAcchCir);
    am2AcchConstruct(&actor->mEnemyIce.mBgAcch);
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x10006FCC;
    actor->mEnemyIce.mBgAcch.cBgS_Chk::__vtbl = 0x10006FBC;
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x10006FDC;
    actor->mEnemyIce.mBgAcch.mbObjThrough = 1;
    return actor;
}

/* 0205067C */
static void am2_dtor(void* storage, s32 flags) {
    auto* actor = static_cast<am2_class*>(storage);
    if (!actor)
        return;
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x10006FCC;
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x10006FDC;
    am2AcchDestruct(&actor->mEnemyIce.mBgAcch);
    am2CircleDestruct(&actor->mEnemyIce.mBgAcchCir.m_cir);
    dCcD_Cyl_dt(&actor->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&actor->mEnemyIce.mStts, 2);
    am2SphereDestruct(&actor->mWeakSph);
    am2SphereDestruct(&actor->mEyeSph);
    dCcD_Cyl_dt(&actor->mNeedleCyl, 2);
    dCcD_Cyl_dt(&actor->mBodyCyl, 2);
    dCcD_Stts_dt(&actor->mStts, 2);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = 0x10006FCC;
    actor->mAcch.dBgS_PolyPassChk::__vtbl = 0x10006FDC;
    am2AcchDestruct(&actor->mAcch);
    am2CircleDestruct(&actor->mAcchCir.m_cir);
    __dt(actor);
    if (flags & 1)
        operator_delete(storage);
}

/* 02050784 */
static void am2_empty_virtual(void* storage) {}

/* 0205034C */
static void am2_safe_string_dtor(void* storage, s32 flags) {
    if (storage && (flags & 1))
        operator_delete(storage);
}

/* 020502B8 */
static void am2_static_init() {
    auto& state = am2HeaderStatics();
    // HD: GHS clears the pairs in this order before registering the header statics.
    state.mZeroState[2] = 0;
    state.mZeroState[0] = 0;
    state.mZeroState[3] = 0;
    state.mZeroState[1] = 0;
    __register_global_object(0x1018FB98);
    state.mNegativePi = am2AngleLimits().mNegativePi;
    state.mPi = am2AngleLimits().mPi;
    am2MathInit(&state.mMathInit);
    __register_global_object(0x1018FBA4);
    am2RandomInit(&state.mRandomInit);
    __register_global_object(0x1018FBB0);
}

/* 0204CF64 */
static BOOL Line_check(am2_class* actor, const cXyz* destination) {
    gabi::Local<dBgS_LinChk> check;
    gabi::Local<cXyz> center;
    gabi::Local<u64> linkage;
    dBgS_LinChk_ct(check.get(), {0x10006FEC, 0x10006FFC, 0x1000701C, 0x1000700C}, false);
    f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
    center->x = x;
    center->y = y;
    center->z = z;
    y += am2Debug().mLineHeightAdjust + 100.0f;
    actor->mLinChkCenter.x = x;
    actor->mLinChkCenter.z = z;
    actor->mLinChkCenter.y = y;
    actor->mLinChkDest.copy(*destination);
    center->y = y;
    am2LineSet(check.get(), center.get(), destination, actor);
    dComIfG_inf_c* play = am2GameInfo();
    BOOL crossed = play->play.mBgS.LineCross(check.get());
    check->dBgS_PolyPassChk::__vtbl = 0x1000701C;
    check->dBgS_GrpPassChk::__vtbl = 0x10006FAC;
    check->cBgS_PolyInfo::__vtbl = 0x10006F9C;
    am2LineDestruct(check.get());
    return crossed ^ 1;
}

/* 0204D0C4 */
static BOOL naraku_check(am2_class* actor) {
    auto* acch = &actor->mAcch;
    bool fell = false;
    if (acch->m_ground_h != -1000000000.0f) {
        dComIfG_inf_c* play = am2GameInfo();
        if (am2CheckPolySafe(&play->play.mBgS, &actor->mAcch.m_gnd)) {
            play = am2GameInfo();
            if (am2GroundCode(&play->play.mBgS, &actor->mAcch.m_gnd) == 4) {
                actor->mInAbyssTimer = (u8)(actor->mInAbyssTimer + 1);
                play = am2GameInfo();
                Am2CameraView* camera = am2Camera(play);
                s32 id = actor ? static_cast<s32>(actor->mBsPcId) : -1;
                camera->mCamera.ForceLockOff(id);
                fell = actor->current.pos.y < -500.0f || actor->mInAbyssTimer > 50;
            }
        }
    }
    if (!fell) {
        if (acch->m_flags & 0x1000) {
            gabi::Local<cXyz> position;
            gabi::Local<cXyz> scale;
            gabi::Local<u64> linkage;
            if (!actor->mbMadeWaterSplash) {
                position->x = actor->current.pos.x;
                position->y = actor->current.pos.y;
                position->z = actor->current.pos.z;
                if (actor->mCountDownTimers[4] == 0) {
                    f32 water = actor->mAcch.m_wtr.m_height;
                    position->y = water;
                    f32 y = actor->current.pos.y;
                    actor->mCountDownTimers[4] = 30;
                    f32 amplitude = am2Debug().mWaterPillarScaleAdjust + 0.1f;
                    f32 height = (water - (y + 80.0f)) * amplitude;
                    if (height < 0.0f)
                        height = 0.3f;
                    else if (height > 1.0f)
                        height = 1.0f;
                    fopKyM_createWpillar(position.get(), 1.0f, height, 0);
                    if (gabi::ea(&actor->eyePos)) {
                        s32 reverb = dComIfGp_getReverb((s8)actor->current.roomNo);
                        mDoAud_seStart(0x6919, &actor->eyePos, 0, reverb);
                    }
                    scale->x = 1.0f;
                    scale->y = 1.0f;
                    scale->z = 1.0f;
                    actor->mbMadeWaterSplash = 1;
                    actor->mRippleCb.end();
                    dComIfG_inf_c* play = am2GameInfo();
                    dPa_control_c* particles = play->play.mParticle;
                    particles->set(static_cast<u8>(5), 0x33, &actor->current.pos, nullptr,
                                   scale.get(), 0xFF,
                                   reinterpret_cast<dPa_levelEcallBack*>(&actor->mRippleCb),
                                   static_cast<s8>(-1), nullptr, nullptr, nullptr);
                    actor->mRippleCb.mRate = 0.0f;
                }
            }
            f32 threshold = actor->mAcch.m_wtr.m_height - (am2Debug().mWaterDepthAdjust + 80.0f);
            fell = actor->current.pos.y < threshold;
        } else if (actor->mbMadeWaterSplash) {
            actor->mbMadeWaterSplash = 0;
            actor->mRippleCb.end();
        }
    }
    if (fell) {
        actor->gravity = 0.0f;
        actor->speed.y = 0.0f;
        actor->speedF = 0.0f;
        actor->speed.x = 0.0f;
        actor->speed.z = 0.0f;
        return TRUE;
    }
    return FALSE;
}

/* 02050360 */
static void action_modoru_move(am2_class* actor) {
    dComIfGp_get();
    u8 mode = actor->mMode;
    if (mode < 40 || mode > 42)
        return;
    if (mode == 40) {
        s32 id = actor ? static_cast<s32>(actor->mBsPcId) : -1;
        Am2CameraBody* camera = am2CameraBody();
        camera->ForceLockOff(id);
        actor->mInAbyssTimer = 0;
        actor->mbMadeWaterSplash = 0;
        actor->mRippleCb.end();
        anm_init(actor, dRes_INDEX_AM2_BCK_WAIT_e, 10.0f, 0, 1.0f, -1);
        if (actor && gabi::ea(&actor->eyePos)) {
            s32 reverb = dComIfGp_getReverb((s8)actor->current.roomNo);
            mDoAud_seStart(0x5946, &actor->eyePos, 0, reverb);
        }
        actor->mMode = (u8)(actor->mMode + 1);
    }
    if (mode != 42) {
        cLib_addCalc0(&actor->scale.x, 1.0f, 0.1f);
        f32 scale = actor->scale.x;
        actor->scale.y = scale;
        actor->scale.z = scale;
        if (scale < 0.1f) {
            actor->speed.y = 0.0f;
            actor->speed.x = 0.0f;
            actor->speed.z = 0.0f;
            actor->speedF = 0.0f;
            actor->mbMadeWaterSplash = 0;
            actor->gravity = -3.0f;
            actor->mRippleCb.end();
            u32 y = am2Word(actor->mSpawnPos.y);
            u32 x = am2Word(actor->mSpawnPos.x);
            am2Word(actor->current.pos.y) = y;
            am2Word(actor->current.pos.x) = x;
            u8 mode = actor->mMode;
            u32 z = am2Word(actor->mSpawnPos.z);
            s16 angle = actor->mSpawnRotY;
            am2Word(actor->current.pos.z) = z;
            actor->current.angle.y = angle;
            actor->mMode = (u8)(mode + 1);
            actor->mTargetAngleY = angle;
            actor->shape_angle.y = angle;
            actor->scale.z = 0.0f;
            actor->scale.x = 0.0f;
            actor->scale.y = 0.0f;
        }
        return;
    }
    u32 y = am2Word(actor->mSpawnPos.y);
    u32 x = am2Word(actor->mSpawnPos.x);
    am2Word(actor->current.pos.y) = y;
    s16 angle = actor->mSpawnRotY;
    am2Word(actor->current.pos.x) = x;
    actor->mTargetAngleY = angle;
    u32 z = am2Word(actor->mSpawnPos.z);
    actor->current.angle.y = angle;
    actor->shape_angle.y = angle;
    am2Word(actor->current.pos.z) = z;
    cLib_addCalc2(&actor->scale.x, 1.0f, 1.0f, 0.1f);
    f32 scale = actor->scale.x;
    actor->scale.z = scale;
    actor->scale.y = scale;
    if (scale > 0.9f) {
        u32 weak = actor->mWeakSph.mObjTg.mSPrm & ~1u;
        u32 target = actor->mNeedleCyl.mObjTg.mSPrm & ~1u;
        actor->scale.y = 1.0f;
        u32 collision = actor->mNeedleCyl.mObjCo.mSPrm & ~1u;
        actor->mWeakSph.mObjTg.mSPrm = weak;
        actor->mNeedleCyl.mObjTg.mSPrm = target;
        u32 attack = actor->mNeedleCyl.mObjAt.mSPrm & ~1u;
        actor->mCountUpTimers[1] = 0;
        actor->scale.x = 1.0f;
        actor->mNeedleCyl.mObjAt.mSPrm = attack;
        actor->scale.z = 1.0f;
        actor->mNeedleCyl.mObjCo.mSPrm = collision;
        actor->mWeakSph.ClrTgHit();
        actor->mNeedleCyl.ClrTgHit();
        u8 inactive = actor->mStartsInactive;
        actor->mAction = ACTION_DOUSA;
        actor->mMode = 0;
        if (inactive == 1 && actor->mSwitch != 0xFF) {
            dSv_info_c* save = dComIfGs_info();
            s8 room = dComIfGp_roomControl_getStayNo();
            if (!save->isSwitch((u8)actor->mSwitch, room)) {
                actor->attention_info.flags = 0;
                actor->mCountDownTimers[2] = 600;
                actor->mAction = ACTION_MAHI;
                actor->mMode = 12;
            }
        }
    }
}

/* 0204FA28 */
static BOOL useHeapInit(fopAc_ac_c* baseActor) {
    auto* actor = static_cast<am2_class*>(baseActor);
    auto* modelData =
        static_cast<J3DModelData*>(am2ObjectResource(dRes_INDEX_AM2_BDL_AM2_e, "Am2"));
    auto* animation =
        static_cast<J3DAnmTransform*>(am2ObjectResource(dRes_INDEX_AM2_BCK_WAIT_e, "Am2"));
    actor->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, animation, 2, 1.0f,
                                           0, -1, 1, nullptr, 0, 0x11020203);
    mDoExt_McaMorf* morph = actor->mpMorf;
    if (!morph || !morph->getModel())
        return FALSE;
    J3DModel* model = morph->getModel();
    auto* btk = static_cast<mDoExt_btkAnm*>(operator_new(0x74));
    if (btk)
        btk = am2ConstructBtk(btk);
    actor->mpBtkAnm = btk;
    if (!btk)
        return FALSE;
    auto* btkData =
        static_cast<J3DAnmTextureSRTKey*>(am2ObjectResource(dRes_INDEX_AM2_BTK_AM2_e, "Am2"));
    btk = actor->mpBtkAnm;
    modelData = model->getModelData();
    if (!btk->init(modelData, btkData, true, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!actor->mpBtkAnm)
        return FALSE;
    auto* brk = static_cast<mDoExt_brkAnm*>(operator_new(0x78));
    if (brk)
        brk = am2ConstructBrk(brk);
    actor->mpBrkAnm = brk;
    if (!brk)
        return FALSE;
    auto* brkData =
        static_cast<J3DAnmTevRegKey*>(am2ObjectResource(dRes_INDEX_AM2_BRK_AM2_e, "Am2"));
    brk = actor->mpBrkAnm;
    modelData = model->getModelData();
    if (!brk->init(modelData, brkData, true, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!actor->mpBrkAnm)
        return FALSE;
    morph = actor->mpMorf;
    model = morph->getModel();
    model->setUserArea(reinterpret_cast<uintptr_t>(actor));
    morph = actor->mpMorf;
    model = morph->getModel();
    modelData = model->getModelData();
    u16 count = modelData->getJointNum();
    morph = actor->mpMorf;
    for (u16 number = 0; number < count; number = (u16)(number + 1)) {
        model = morph->getModel();
        model->getModelData()->getJointNodePointer(number)->setCallBack(nodeCallBack);
        morph = actor->mpMorf;
        model = morph->getModel();
        count = model->getModelData()->getJointNum();
        morph = actor->mpMorf;
    }
    u32 hit = am2CreateJntHit(morph->getModel(), search_data, 2);
    actor->mEyeJntHit = hit;
    if (!hit)
        return FALSE;
    actor->jntHit = hit;
    return TRUE;
}

/* 0204FE84 */
static s32 daAM2_Create(am2_class* actor) {
    if (!(actor->actor_condition & 8)) {
        if (actor)
            am2_ctor(actor);
        actor->actor_condition = actor->actor_condition | 8;
    }
    s32 phase = dComIfG_resLoad(&actor->mPhase, "Am2");
    if (phase != 4)
        return phase;
    if (!fopAcM_entrySolidHeap(actor, useHeapInit, 0x1AA0))
        return 5;
    actor->mSmokeCb.m11 = 0;
    u32 params = actor->mParameters;
    actor->mType = (u8)params;
    actor->mPrmAreaRadius = (u8)(actor->mParameters >> 8);
    actor->mStartsInactive = (u8)(actor->mParameters >> 16);
    actor->mSwitch = (u8)(actor->mParameters >> 24);
    if ((u8)params == 0xFF)
        actor->mType = 0;
    if (actor->mStartsInactive == 0xFF)
        actor->mStartsInactive = 0;
    if (am2Debug().mForceType != 0)
        actor->mType = 1;
    u8 radius = actor->mPrmAreaRadius;
    actor->mAreaRadius = (radius == 0xFF || radius == 0) ? 600.0f : (f32)radius * 100.0f;
    if (!actor->mStartsInactive && actor->mSwitch != 0xFF) {
        dSv_info_c* save = dComIfGs_info();
        s8 room = dComIfGp_roomControl_getStayNo();
        if (save->isSwitch((u8)actor->mSwitch, room))
            return 5;
    }
    dComIfG_inf_c* play = am2GameInfo();
    actor->itemTableIdx = am2CharTable(play)->GetNameIndex("amos2", 0);
    actor->max_health = 2;
    actor->health = 2;
    actor->stealItemLeft = 3;
    actor->model = gabi::ea(actor->mpMorf->getModel());
    dComIfGp_get();
    u32 model = gabi::ea(actor->mpMorf->getModel());
    actor->cullMtx = model ? model + 0xC8 : 0;
    fopAcM_setCullSizeBox(actor, -50.0f, 0.0f, -20.0f, 60.0f, 180.0f, 60.0f);
    actor->attention_info.flags = 0;
    actor->mAcch.Set(&actor->current.pos, &actor->old.pos, actor, 1, &actor->mAcchCir,
                     &actor->speed, nullptr, nullptr);
    dCcD_Stts* status = &actor->mStts;
    status->Init(0xFE, 1, actor);
    actor->gravity = -3.0f;
    actor->mEnemyIce.mWallRadius = 50.0f;
    actor->mEnemyIce.mCylHeight = 100.0f;
    actor->mEnemyIce.mpActor = actor;
    actor->attention_info.distances[4] = 9;
    actor->actor_status = actor->actor_status | 0x08000000;
    actor->mEyeSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&eye_co_sph_src));
    u32 flags = actor->mEyeSph.mObjTg.mSPrm;
    actor->mEyeSph.mStts = status;
    actor->mEyeSph.mObjTg.mSPrm = flags & ~1u;
    actor->mWeakSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&week_co_sph_src));
    flags = actor->mWeakSph.mObjTg.mSPrm;
    actor->mWeakSph.mStts = status;
    actor->mWeakSph.mObjTg.mSPrm = flags & ~1u;
    actor->mBodyCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&body_co_cyl_src));
    actor->mBodyCyl.mStts = status;
    actor->mNeedleCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&sword_co_cyl_src));
    u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm,
        collision = actor->mNeedleCyl.mObjCo.mSPrm;
    actor->mNeedleCyl.mObjAt.mSPrm = attack & ~1u;
    actor->mNeedleCyl.mStts = status;
    actor->mNeedleCyl.mObjCo.mSPrm = collision & ~1u;
    actor->mNeedleCyl.mObjTg.mSPrm = target & ~1u;
    actor->m304.copy(actor->current.pos);
    actor->mTargetAngleY = actor->current.angle.y;
    actor->mSpawnPos.copy(actor->current.pos);
    actor->mSpawnRotY = actor->shape_angle.y;
    BG_check(actor);
    draw_SUB(actor);
    if (actor->mStartsInactive == 1 && actor->mSwitch != 0xFF) {
        dSv_info_c* save = dComIfGs_info();
        s8 room = dComIfGp_roomControl_getStayNo();
        if (!save->isSwitch((u8)actor->mSwitch, room)) {
            actor->attention_info.flags = 0;
            actor->mCountDownTimers[2] = 600;
            actor->mAction = ACTION_MAHI;
            actor->mMode = 12;
        }
    }
    actor->mAcchRadius = am2Debug().mAcchCarryRadiusAdjust + 40.0f;
    return phase;
}

static void am2HitSound(am2_class* actor, u32 id) {
    if (gabi::ea(&actor->eyePos)) {
        s32 reverb = dComIfGp_getReverb((s8)actor->current.roomNo);
        mDoAud_seStart(id, &actor->eyePos, 0x35, reverb);
    }
}
/* 0204C7C8 */
static BOOL week_atari_check(am2_class* actor) {
    dComIfG_inf_c* play = am2GameInfo();
    auto* player = static_cast<Am2PlayerView*>(play->play.mPlayerInfo[0].mpPlayer.get());
    play = am2GameInfo();
    fopAc_ac_c* target = play->play.mPlayerInfo[0].mpPlayer;
    s16 angle = fopAcM_searchActorAngleY(actor, target);
    s16 distance = am2DistanceAngleS((s16)actor->shape_angle.y, angle);
    if (distance < 0x4000)
        return FALSE;
    actor->mStts.Move();
    actor->mHitDirection = 0;
    if (!actor->mWeakSph.ChkTgHit()) {
        actor->mbIsWeakBeingHit = 0;
        return FALSE;
    }
    if (actor->mbIsWeakBeingHit)
        return FALSE;
    auto* hit = static_cast<dCcD_GObjInf*>(actor->mWeakSph.GetTgHitObj());
    actor->mbIsWeakBeingHit = 1;
    if (!hit)
        return FALSE;
    u32 type = hit->mAtType;
    if (type & 0x100000) {
        actor->attention_info.flags = 0;
        actor->mEnemyIce.mYOffset = 80.0f;
        actor->mEnemyIce.mParticleScale = 1.0f;
        actor->mEnemyIce.mLightShrinkTimer = 1;
        return TRUE;
    }
    u8 hitType = 0;
    switch (type) {
    case 2:
    case 0x400:
    case 0x800:
    case 0x4000000:
    case 0x10000000: {
        am2HitSound(actor, 0x2803);
        u8 cut = player->mCutType;
        if ((cut >= 6 && cut <= 10) || cut == 12 || (cut >= 14 && cut <= 16) || cut == 21 ||
            (cut >= 25 && cut <= 27) || (cut >= 30 && cut <= 31)) {
            hitType = 1;
            actor->mHitDirection = 1;
        }
        break;
    }
    case 0x200000:
        actor->mHitDirection = 3;
        return TRUE;
    case 0x40:
        actor->mHitDirection = 4;
        return FALSE;
    case 0x80:
    case 0x2000:
    case 0x1000000:
        am2HitSound(actor, 0x2833);
        break;
    case 0x10000:
        am2HitSound(actor, 0x2855);
        actor->mHitDirection = 7;
        hitType = 1;
        if (player->mCutType == 0x11)
            actor->mHitDirection = 8;
        break;
    case 0x20:
        hitType = 1;
        actor->mHitDirection = 6;
        break;
    case 0x4000:
    case 0x40000:
    case 0x80000:
        hitType = 1;
        actor->mHitDirection = 5;
        am2HitSound(actor, 0x2834);
        break;
    default:
        am2HitSound(actor, 0x2834);
        break;
    }
    gabi::Local<cXyz> hitPosition;
    gabi::Local<cXyz> scale;
    gabi::Local<CcAtInfo> info;
    gabi::Local<u64> linkage;
    hitPosition->x = actor->mWeakSph.mGObjTg.mHitPos.x;
    hitPosition->y = actor->mWeakSph.mGObjTg.mHitPos.y;
    hitPosition->z = actor->mWeakSph.mGObjTg.mHitPos.z;
    hit = static_cast<dCcD_GObjInf*>(actor->mWeakSph.GetTgHitObj());
    info->mpObj = hit;
    info->pParticlePos = nullptr;
    am2AttackCheck(actor, info.get());
    play = am2GameInfo();
    dPa_control_c* particles = play->play.mParticle;
    auto angles = &player->shape_angle;
    if (hitType == 1) {
        particles->set(static_cast<u8>(0), 0x10, hitPosition.get(), nullptr, nullptr, 0xFF, nullptr,
                       static_cast<s8>(-1), nullptr, nullptr, nullptr);
        scale->x = 2.0f;
        scale->z = 2.0f;
        scale->y = 2.0f;
        play = am2GameInfo();
        particles = play->play.mParticle;
        particles->set(static_cast<u8>(0), 0xF, hitPosition.get(), angles, scale.get(), 0xFF,
                       nullptr, static_cast<s8>(-1), nullptr, nullptr, nullptr);
    } else {
        particles->set(static_cast<u8>(0), 0xD, hitPosition.get(), angles, nullptr, 0xFF, nullptr,
                       static_cast<s8>(-1), nullptr, nullptr, nullptr);
    }
    u8 direction = actor->mHitDirection;
    actor->mAction = ACTION_ITAI;
    actor->mMode = 20;
    if (direction == 7 || direction == 8)
        actor->health = 0;
    return TRUE;
}

static void am2SoundAtEye(am2_class* actor, u32 id, u32 parameter = 0) {
    if (actor && gabi::ea(&actor->eyePos)) {
        s32 room = (s8)actor->current.roomNo;
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(id, &actor->eyePos, parameter, reverb);
    }
}
static void am2Voice(am2_class* actor, u32 id) {
    if (gabi::ea(&actor->eyePos)) {
        s8 room = actor->current.roomNo;
        s32 process = actor ? static_cast<s32>(actor->mBsPcId) : -1;
        s32 reverb = dComIfGp_getReverb(room);
        am2MonsterSound(id, &actor->eyePos, process, 0, reverb);
    }
}
static Am2PlayerView* am2Player() {
    dComIfG_inf_c* play = am2GameInfo();
    return static_cast<Am2PlayerView*>(play->play.mPlayerInfo[0].mpPlayer.get());
}
static f32 am2PlayerDistance(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    return fopAcM_searchActorDistance(actor, player);
}
static s16 am2PlayerAngle(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    return fopAcM_searchActorAngleY(actor, player);
}
static void am2Flag(be<u32>& parameter, bool enable) {
    u32 flags = parameter;
    parameter = enable ? flags | 1u : flags & ~1u;
}
static bool am2Stopped(am2_class* actor) {
    mDoExt_McaMorf* morph = actor->mpMorf;
    return (morph->mFrameCtrl.mState & 1) || morph->mFrameCtrl.mRate == 0.0f;
}
static void am2ResetUpTimers(am2_class* actor) {
    for (auto& timer : actor->mCountUpTimers)
        timer = 0;
}
static void am2Fell(am2_class* actor) {
    if (actor->mbNotInHomeRoom)
        fopAcM_delete(actor);
    else {
        actor->mMode = 40;
        actor->mAction = ACTION_MODORU_MOVE;
    }
}
static void am2Particle(am2_class* actor, s32 group, u16 id, const cXyz* position,
                        const csXyz* angle = nullptr, const cXyz* scale = nullptr, u8 alpha = 0xFF,
                        void* callback = nullptr, s32 room = -1) {
    dComIfG_inf_c* play = am2GameInfo();
    dPa_control_c* particles = play->play.mParticle;
    particles->set(static_cast<u8>(group), id, position, angle, scale, alpha,
                   reinterpret_cast<dPa_levelEcallBack*>(callback), static_cast<s8>(room), nullptr,
                   nullptr, nullptr);
}

// HD embeds the eye-hit state machine in Execute.
static bool medama_atari_check(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    if (actor->mStartsInactive == 1 && actor->mSwitch != 0xFF) {
        dSv_info_c* save = dComIfGs_info();
        s8 room = dComIfGp_roomControl_getStayNo();
        if (!save->isSwitch((u8)actor->mSwitch, room))
            return false;
    }
    actor->mStts.Move();
    if (!actor->mEyeSph.ChkTgHit())
        return false;
    auto* hit = static_cast<dCcD_GObjInf*>(actor->mEyeSph.GetTgHitObj());
    if (!hit)
        return false;
    gabi::Local<cXyz> position;
    gabi::Local<CcAtInfo> info;
    gabi::Local<u64> linkage;
    position->x = actor->mEyeSph.mGObjTg.mHitPos.x;
    position->y = actor->mEyeSph.mGObjTg.mHitPos.y;
    position->z = actor->mEyeSph.mGObjTg.mHitPos.z;
    u32 type = hit->mAtType;
    if (type & 0x08000000) {
        if (actor->mCurrBckIdx != dRes_INDEX_AM2_BCK_SLEEP_e) {
            if (actor->stealItemLeft > 0) {
                u8 health = actor->health;
                actor->health = 10;
                hit = static_cast<dCcD_GObjInf*>(actor->mEyeSph.GetTgHitObj());
                info->mpObj = hit;
                info->pParticlePos = nullptr;
                am2AttackCheck(actor, info.get());
                actor->health = health;
                am2Particle(actor, 0, 0x27B, &actor->attention_info.position);
            } else
                am2Particle(actor, 0, 0xC, position.get());
            am2SoundAtEye(actor, 0x2834, 0x42);
        }
        return true;
    }
    if (type & 0x100000) {
        actor->mEnemyIce.mParticleScale = 1.0f;
        actor->attention_info.flags = 0;
        actor->mEnemyIce.mLightShrinkTimer = 1;
        actor->mEnemyIce.mYOffset = 80.0f;
        return true;
    }
    if (!(type & 0xC4000))
        return false;
    if (actor->mCurrBckIdx == dRes_INDEX_AM2_BCK_SLEEP_e) {
        anm_init(actor, dRes_INDEX_AM2_BCK_WAIT_e, 1.0f, 0, 1.0f, -1);
        u32 status = actor->actor_status;
        actor->mAction = ACTION_DOUSA;
        am2Flag(actor->mNeedleCyl.mObjTg.mSPrm, true);
        am2Flag(actor->mNeedleCyl.mObjAt.mSPrm, true);
        actor->mNeedleCyl.mObjAt.mRPrm = 1;
        actor->actor_status = status | 0x20;
        actor->mMode = 2;
        actor->attention_info.flags = 4;
    } else {
        am2Particle(actor, 0, 0x10, position.get(), &player->shape_angle);
        am2SoundAtEye(actor, 0x58B4);
        am2Voice(actor, 0x48B5);
        actor->mAction = ACTION_MAHI;
        actor->mMode = 10;
    }
    return true;
}

static void action_dousa(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    gabi::Local<cXyz> offset, rotated, destination;
    gabi::Local<u64> linkage;
    u8 mode = actor->mMode;
    if (mode == 4 || mode == 5) {
        Mtx34* matrix = calc_mtx();
        mDoMtx_YrotS(matrix, (s16)actor->current.angle.y);
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = 200.0f;
        MtxPosition(offset.get(), rotated.get());
        PSVECAdd(rotated.get(), &actor->current.pos, rotated.get());
        f32 extra = am2Debug().mLineHeightAdjust + 100.0f;
        rotated->y = (f32)rotated->y + extra;
        mode = actor->mMode;
    }
    switch (mode) {
    case 0:
        am2ResetUpTimers(actor);
        actor->mAcchRadius = am2Debug().mAcchActiveRadiusAdjust + 80.0f;
        if (actor->mCurrBckIdx != dRes_INDEX_AM2_BCK_WAIT_e)
            anm_init(actor, dRes_INDEX_AM2_BCK_WAIT_e, 10.0f, 0, 1.0f, -1);
        actor->mMode = (u8)(actor->mMode + 1);
        [[fallthrough]];
    case 1: {
        f32 distance = am2PlayerDistance(actor);
        if (distance < (f32)actor->mAreaRadius) {
            f32 extra = am2Debug().mLineHeightAdjust + 100.0f;
            f32 y = player->current.pos.y;
            destination->x = player->current.pos.x;
            destination->z = player->current.pos.z;
            destination->y = y + extra;
            if (Line_check(actor, destination.get())) {
                u32 status = actor->actor_status;
                actor->attention_info.flags = 4;
                actor->actor_status = status | 0x20;
                anm_init(actor, dRes_INDEX_AM2_BCK_START_e, 1.0f, 0, 1.0f, -1);
                am2Voice(actor, 0x48B3);
                u8 mode = actor->mMode;
                u32 eye = actor->mEyeSph.mObjTg.mSPrm, weak = actor->mWeakSph.mObjTg.mSPrm;
                actor->mMode = (u8)(mode + 1);
                actor->mWeakSph.mObjTg.mSPrm = weak | 1;
                actor->mEyeSph.mObjTg.mSPrm = eye | 1;
            }
        }
        break;
    }
    case 2: {
        mDoExt_McaMorf* morph = actor->mpMorf;
        if (morph->checkFrame(24.0f))
            am2SoundAtEye(actor, 0x58B9);
        if (!am2Stopped(actor))
            break;
        actor->mMode = (u8)(actor->mMode + 1);
        [[fallthrough]];
    }
    case 3: {
        am2ResetUpTimers(actor);
        actor->speedF = 0.0f;
        f32 distance = am2PlayerDistance(actor);
        f32 radius = (f32)actor->mAreaRadius + 200.0f;
        if (distance > radius)
            actor->mMode = 6;
        else {
            u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm;
            actor->mNeedleCyl.mObjAt.mRPrm = 1;
            actor->mNeedleCyl.mObjAt.mSPrm = attack | 1;
            actor->mNeedleCyl.mObjTg.mSPrm = target | 1;
            s16 angle = am2PlayerAngle(actor);
            s32 animation = actor->mCurrBckIdx;
            actor->mTargetAngleY = angle;
            actor->gravity = -3.0f;
            actor->speed.y = 12.0f;
            if (animation != dRes_INDEX_AM2_BCK_JUMP_e)
                anm_init(actor, dRes_INDEX_AM2_BCK_JUMP_e, 2.0f, 2, 1.0f, -1);
            actor->mMode = (u8)(actor->mMode + 1);
        }
        break;
    }
    case 4:
        if (actor->mAcch.m_flags & 0x20) {
            actor->gravity = -3.0f;
            actor->speed.y = 12.0f;
            am2SoundAtEye(actor, 0x58B3);
            actor->mMode = 3;
            s16 distance =
                am2DistanceAngleS((s16)actor->shape_angle.y, (s16)actor->current.angle.y);
            if (distance < 0x1000) {
                destination->z = rotated->z;
                destination->y = rotated->y;
                destination->x = rotated->x;
                if (Line_check(actor, destination.get()) || player->mDamageWaitTimer == 0)
                    actor->mMode = 5;
            }
        }
        break;
    case 5: {
        f32 speed = actor->speedF;
        u32 ground = actor->mAcch.m_flags;
        if (speed > 0.0f && (actor->mNeedleCyl.mGObjAt.mRPrm & 1)) {
            actor->speed.y = 0.0f;
            actor->speedF = -9.0f;
        }
        if (ground & 0x20) {
            am2SoundAtEye(actor, 0x58B3);
            actor->mSmokeCb.end();
            am2SoundAtEye(actor, 0x5882);
            s8 room = actor->current.roomNo;
            am2Particle(actor, 2, 0xA125, &actor->current.pos, &actor->shape_angle, nullptr, 0xB9,
                        &actor->mSmokeCb, room);
            Am2EmitterView* emitter = am2SmokeEmitter(actor);
            if (emitter) {
                emitter->mRate = 12.0f;
                emitter = am2SmokeEmitter(actor);
                am2SmokeScale(emitter, 0.45f);
            }
            if (actor->mCountUpTimers[0] > 8)
                actor->mMode = 3;
            else {
                actor->speedF = 9.0f;
                actor->speed.y = 20.0f;
                actor->gravity = -8.0f;
                destination->z = rotated->z;
                destination->x = rotated->x;
                destination->y = rotated->y;
                actor->mCountUpTimers[1] = 0;
                BOOL clear = Line_check(actor, destination.get());
                s16 count = actor->mCountUpTimers[0];
                if (!clear || player->mDamageWaitTimer != 0)
                    actor->speedF = 0.0f;
                actor->mCountUpTimers[0] = (s16)(count + 1);
            }
        }
        break;
    }
    case 6:
        anm_init(actor, dRes_INDEX_AM2_BCK_SLEEP_e, 1.0f, 0, 1.0f, -1);
        am2SoundAtEye(actor, 0x58BA);
        am2Flag(actor->mWeakSph.mObjTg.mSPrm, false);
        actor->mWeakSph.ClrTgHit();
        actor->mMode = (u8)(actor->mMode + 1);
        [[fallthrough]];
    case 7:
        if (am2Stopped(actor)) {
            am2ResetUpTimers(actor);
            u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm;
            actor->mNeedleCyl.mObjAt.mSPrm = attack & ~1u;
            actor->mNeedleCyl.mObjTg.mSPrm = target & ~1u;
            actor->mNeedleCyl.ClrTgHit();
            actor->mMode = 1;
            u32 z = am2Word(actor->current.pos.z);
            am2Word(actor->m304.z) = z;
            u32 x = am2Word(actor->current.pos.x), y = am2Word(actor->current.pos.y);
            am2Word(actor->m304.x) = x;
            am2Word(actor->m304.y) = y;
            actor->mAction = ACTION_DOUSA;
        }
        break;
    }
    cLib_addCalcAngleS2(&actor->current.angle.y, (s16)actor->mTargetAngleY, 1, 0x500);
    cLib_addCalcAngleS2(&actor->shape_angle.y, (s16)actor->current.angle.y, 1, 0x500);
    if (naraku_check(actor))
        am2Fell(actor);
    else if (!medama_atari_check(actor)) {
        if (actor->mMode < 3 || !week_atari_check(actor))
            body_atari_check(actor);
    }
}

static BOOL am2Switch(am2_class* actor) {
    dSv_info_c* save = dComIfGs_info();
    s8 room = dComIfGp_roomControl_getStayNo();
    return save->isSwitch((u8)actor->mSwitch, room);
}
static void am2Attention(am2_class* actor, u32 bits, bool enable) {
    u32 flags = actor->attention_info.flags;
    actor->attention_info.flags = enable ? flags | bits : flags & ~bits;
}
static void am2Smoke(am2_class* actor, f32 scale) {
    s8 room = actor->current.roomNo;
    am2Particle(actor, 2, 0xA125, &actor->current.pos, &actor->shape_angle, nullptr, 0xB9,
                &actor->mSmokeCb, room);
    Am2EmitterView* emitter = am2SmokeEmitter(actor);
    if (emitter) {
        emitter->mRate = 12.0f;
        emitter = am2SmokeEmitter(actor);
        am2SmokeScale(emitter, scale);
    }
}
static void action_mahi(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    switch ((u8)actor->mMode) {
    case 10: {
        am2ResetUpTimers(actor);
        u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm;
        actor->mNeedleCyl.mObjAt.mSPrm = attack & ~1u;
        actor->mNeedleCyl.mObjTg.mSPrm = target & ~1u;
        actor->mNeedleCyl.ClrTgHit();
        actor->mTargetAngleY = (s16)(am2PlayerAngle(actor) + 0x8000);
        actor->current.angle.y = (s16)(am2PlayerAngle(actor) + 0x8000);
        actor->mCountDownTimers[2] = 0;
        actor->mCountDownTimers[3] = 0;
        actor->speedF = 20.0f;
        am2SoundAtEye(actor, 0x58BA);
        anm_init(actor, dRes_INDEX_AM2_BCK_DAMAGE_e, 1.0f, 0, 1.0f, -1);
        actor->mMode = (u8)(actor->mMode + 1);
        break;
    }
    case 11:
        cLib_addCalc0(&actor->speedF, 0.5f, 1.0f);
        if (actor->speedF < 0.2f && am2Stopped(actor)) {
            actor->speedF = 0.0f;
            actor->mCountDownTimers[2] = 600;
            anm_init(actor, dRes_INDEX_AM2_BCK_MAHI_e, 1.0f, 2, 1.0f, -1);
            actor->mMode = (u8)(actor->mMode + 1);
        }
        break;
    case 12:
        if (actor->mCountDownTimers[3] == 0 || actor->mCountDownTimers[3] > 3)
            am2Attention(actor, 0x10, true);
        if (naraku_check(actor))
            am2Fell(actor);
        else {
            if (actor->mCountUpTimers[1] != 0 && (actor->mAcch.m_flags & 0x20)) {
                am2SoundAtEye(actor, 0x58B2);
                actor->mCountUpTimers[1] = 0;
            }
            if (actor->actor_status & 0x2000) {
                actor->mbMadeWaterSplash = 0;
                actor->mAcchRadius = am2Debug().mAcchCarryRadiusAdjust + 40.0f;
                actor->mRippleCb.end();
                u32 status = actor->actor_status;
                u8 mode = actor->mMode;
                u32 collision = actor->mBodyCyl.mObjCo.mSPrm;
                f32 y = actor->current.pos.y;
                actor->actor_status = status & ~0x20u;
                actor->mPickedUpYPos = y;
                s16 angle = player->shape_angle.y;
                actor->speedF = 0.0f;
                actor->mbNotInHomeRoom = 0;
                actor->speed.y = 0.0f;
                actor->mMode = (u8)(mode + 1);
                actor->speed.x = 0.0f;
                actor->mTargetAngleY = angle;
                actor->speed.z = 0.0f;
                actor->current.angle.y = angle;
                actor->gravity = 0.0f;
                actor->mBodyCyl.mObjCo.mSPrm = collision & ~1u;
            }
        }
        break;
    case 13: {
        s8 room = actor->current.roomNo, home = actor->home.roomNo;
        s16 angle = player->shape_angle.y;
        actor->current.angle.y = angle;
        if (home != room)
            actor->mbNotInHomeRoom = 1;
        f32 picked = (f32)actor->mPickedUpYPos + 10.0f;
        if (!(picked > (f32)actor->current.pos.y))
            cLib_addCalcAngleS2(&actor->shape_angle.y, (s16)actor->current.angle.y, 1, 0x1000);
        if (!(actor->actor_status & 0x2000)) {
            u32 collision = actor->mBodyCyl.mObjCo.mSPrm;
            f32 radius = am2Debug().mAcchCarryRadiusAdjust + 40.0f;
            f32 speed = actor->speedF;
            actor->mBodyCyl.mObjCo.mSPrm = collision | 1;
            actor->mAcchRadius = radius;
            if (speed > 0.0f) {
                u32 flags = actor->mAcch.m_flags;
                actor->gravity = -5.0f;
                actor->mMode = 14;
                actor->mAcch.m_flags = flags | 0x2000;
                actor->speedF = 35.0f;
                actor->speed.y = 25.0f;
            } else {
                actor->gravity = -3.0f;
                actor->mCountUpTimers[1] = 1;
                actor->mMode = 12;
            }
        }
        break;
    }
    case 14:
        if (actor->mAcch.m_flags & 0x10)
            actor->speedF = 0.0f;
        if (naraku_check(actor)) {
            if (actor->mbNotInHomeRoom)
                fopAcM_delete(actor);
            else {
                u32 flags = actor->mAcch.m_flags;
                actor->mAction = ACTION_MODORU_MOVE;
                actor->mAcch.m_flags = flags & ~0x2000u;
                actor->mMode = 40;
            }
        } else {
            if (actor->mAcch.m_flags & 0x20) {
                gabi::Local<cXyz> direction;
                gabi::Local<u64> linkage;
                dComIfG_inf_c* play = am2GameInfo();
                direction->x = 0.0f;
                direction->y = 1.0f;
                direction->z = 0.0f;
                play->play.mVibration.StartShock(1, -0x21, direction.get());
                play = am2GameInfo();
                Am2CameraView* camera = am2Camera(play);
                s32 id = actor ? static_cast<s32>(actor->mBsPcId) : -1;
                camera->mCamera.ForceLockOff(id);
                u32 flags = actor->mAcch.m_flags;
                actor->mbMadeWaterSplash = 0;
                actor->mAcch.m_flags = flags & ~0x2000u;
                actor->mRippleCb.end();
                if (!actor->mCountUpTimers[0]) {
                    actor->mSmokeCb.end();
                    am2SoundAtEye(actor, 0x58B3);
                    am2Smoke(actor, 0.8f);
                    s16 count = actor->mCountUpTimers[0];
                    actor->speed.y = 25.0f;
                    actor->mCountUpTimers[0] = (s16)(count + 1);
                    actor->speedF = 7.0f;
                    actor->gravity = -14.0f;
                } else {
                    actor->speedF = 0.0f;
                    actor->mCountUpTimers[0] = 0;
                    actor->mMode = 12;
                }
            }
            cLib_addCalc0(&actor->speedF, 0.5f, 1.0f);
        }
        break;
    case 15:
        if (naraku_check(actor)) {
            if (actor->mbNotInHomeRoom)
                fopAcM_delete(actor);
            else {
                actor->mMode = 40;
                actor->mAction = ACTION_MODORU_MOVE;
                return;
            }
        }
        actor->shape_angle.y = (s16)(actor->shape_angle.y + 0x1000);
        if (actor->mAcch.m_flags & 0x20) {
            actor->gravity = -3.0f;
            actor->actor_status = actor->actor_status | 0x20;
            actor->mMode = 3;
            actor->attention_info.flags = 4;
            actor->mAction = ACTION_DOUSA;
        }
        break;
    }
    actor->mTargetAngleY = actor->current.angle.y;
    u8 mode = actor->mMode;
    if (mode >= 12 && mode != 15) {
        if (actor->mStartsInactive == 1 && actor->mSwitch != 0xFF) {
            if (!am2Switch(actor)) {
                actor->mCountDownTimers[2] = 600;
                body_atari_check(actor);
                return;
            }
            u32 status = actor->actor_status;
            actor->mMode = 0;
            actor->mSwitch = 0xFF;
            actor->mAction = ACTION_DOUSA;
            if (status & 0x2000) {
                fopAcM_cancelCarryNow(actor);
                am2Attention(actor, 0x10, false);
                actor->speed.y = 20.0f;
                actor->gravity = -4.0f;
            }
            return;
        }
        if (!actor->mCountDownTimers[2]) {
            if (actor->mCurrBckIdx != dRes_INDEX_AM2_BCK_BURUBURU_e) {
                anm_init(actor, dRes_INDEX_AM2_BCK_BURUBURU_e, 1.0f, 2, 1.0f, -1);
                actor->mCountDownTimers[3] = 600;
                am2Attention(actor, 0x10, false);
                am2SoundAtEye(actor, 0x58B5);
                am2Voice(actor, 0x48B3);
            }
            if (actor->mCountDownTimers[3] == 1) {
                u32 status = actor->actor_status;
                if (status & 0x2000) {
                    fopAcM_cancelCarryNow(actor);
                    am2Attention(actor, 0x10, false);
                    status = actor->actor_status;
                }
                u32 collision = actor->mBodyCyl.mObjCo.mSPrm;
                actor->speed.y = 20.0f;
                actor->gravity = -4.0f;
                actor->mBodyCyl.mObjCo.mSPrm = collision | 1;
                actor->mMode = 15;
                actor->actor_status = status | 0x20;
            }
        }
    }
    if ((actor->actor_status & 0x2000) || actor->mMode == 15 || !week_atari_check(actor))
        body_atari_check(actor);
}

static void action_itai(am2_class* actor) {
    dComIfGp_get();
    switch ((u8)actor->mMode) {
    case 20: {
        am2ResetUpTimers(actor);
        u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm;
        actor->mNeedleCyl.mObjAt.mSPrm = attack & ~1u;
        actor->mNeedleCyl.mObjTg.mSPrm = target & ~1u;
        actor->mNeedleCyl.ClrTgHit();
        actor->mTargetAngleY = (s16)(am2PlayerAngle(actor) + 0x8000);
        actor->current.angle.y = (s16)(am2PlayerAngle(actor) + 0x8000);
        actor->speedF = 20.0f;
        am2Voice(actor, 0x48B6);
        am2Particle(actor, 0, 0x81AE, &actor->mWeakPos, &actor->shape_angle);
        if (actor->health > 0) {
            anm_init(actor, dRes_INDEX_AM2_BCK_DAMAGE_e, 1.0f, 0, 1.0f, -1);
            if (actor->mCountDownTimers[2] > 5)
                actor->speedF = 5.0f;
            actor->mMode = (u8)(actor->mMode + 1);
        } else
            actor->mMode = 22;
        break;
    }
    case 21:
        cLib_addCalc0(&actor->speedF, 0.5f, 1.0f);
        if (actor->speedF < 0.2f) {
            actor->speedF = 0.0f;
            actor->gravity = -3.0f;
            if (actor->mCountDownTimers[2] < 5) {
                actor->mAction = ACTION_DOUSA;
                actor->mMode = 3;
            } else {
                anm_init(actor, dRes_INDEX_AM2_BCK_MAHI_e, 1.0f, 2, 1.0f, -1);
                actor->mAction = ACTION_MAHI;
                actor->mMode = 12;
            }
        }
        break;
    case 22: {
        u32 eye = actor->mEyeSph.mObjTg.mSPrm, weak = actor->mWeakSph.mObjTg.mSPrm;
        actor->mEyeSph.mObjTg.mSPrm = eye & ~1u;
        actor->mWeakSph.mObjTg.mSPrm = weak & ~1u;
        actor->mEyeSph.ClrTgHit();
        actor->mWeakSph.ClrTgHit();
        anm_init(actor, dRes_INDEX_AM2_BCK_DEAD1_e, 1.0f, 0, 1.0f, -1);
        actor->mMode = (u8)(actor->mMode + 1);
        break;
    }
    case 23:
        if (!am2Stopped(actor))
            break;
        {
            u32 attack = actor->mNeedleCyl.mObjAt.mSPrm;
            actor->mNeedleCyl.mObjAt.mRPrm = 1;
            actor->mNeedleCyl.mObjAt.mSPrm = attack | 1;
            actor->gravity = -10.0f;
            actor->speed.y = 25.0f;
            actor->speedF = 10.0f;
            actor->mCountDownTimers[0] = 100;
            actor->current.angle.y = am2PlayerAngle(actor);
            anm_init(actor, dRes_INDEX_AM2_BCK_DEAD2_e, 1.0f, 0, 1.0f, -1);
            actor->mMode = (u8)(actor->mMode + 1);
        }
        [[fallthrough]];
    case 24:
        if (actor->speed.y > 0.0f && !actor->mCountUpTimers[1]) {
            am2Voice(actor, 0x48B4);
            actor->mCountUpTimers[1] = 1;
        }
        actor->shape_angle.y = (s16)(actor->shape_angle.y + 0x1000);
        if (actor->mAcch.m_flags & 0x20) {
            actor->mSmokeCb.end();
            am2Smoke(actor, 0.45f);
            am2SoundAtEye(actor, 0x58B6);
            am2Voice(actor, 0x4879);
            actor->speedF = 10.0f;
            actor->speed.y = 25.0f;
            actor->gravity = -10.0f;
        }
        if (!actor->mCountDownTimers[0]) {
            anm_init(actor, dRes_INDEX_AM2_BCK_DEAD3_e, 1.0f, 0, 1.0f, -1);
            am2SoundAtEye(actor, 0x58B7);
            actor->speedF = 0.0f;
            actor->mMode = (u8)(actor->mMode + 1);
        }
        break;
    case 25:
        if (am2Stopped(actor)) {
            gabi::Local<cXyz> center;
            gabi::Local<u64> linkage;
            center->x = actor->current.pos.x;
            center->y = actor->current.pos.y;
            center->z = actor->current.pos.z;
            center->y = (f32)center->y + 50.0f;
            am2Particle(actor, 0, 0x81AF, &actor->current.pos, &actor->shape_angle);
            am2Particle(actor, 0, 0x81B0, &actor->current.pos, &actor->shape_angle);
            am2SoundAtEye(actor, 0x58B8);
            fopAcM_createDisappear(actor, center.get(), 5, 0, 0xFF);
            s8 room = actor->home.roomNo;
            dSv_info_c* save = dComIfGs_info();
            u16 id = actor->setID;
            save->onActor(id, room);
            fopAcM_delete(actor);
        }
        break;
    }
    if (naraku_check(actor)) {
        if (actor->mbNotInHomeRoom || actor->health <= 0) {
            if (actor->mMode != 25) {
                anm_init(actor, dRes_INDEX_AM2_BCK_DEAD3_e, 1.0f, 0, 1.0f, -1);
                am2SoundAtEye(actor, 0x58B7);
                actor->speedF = 0.0f;
                actor->mMode = 25;
            }
        } else {
            actor->mMode = 40;
            actor->mAction = ACTION_MODORU_MOVE;
        }
    }
}

static void action_handou_move(am2_class* actor) {
    Am2PlayerView* player = am2Player();
    switch ((u8)actor->mMode) {
    case 30: {
        actor->speedF = 40.0f;
        s16 angle = am2PlayerAngle(actor);
        u8 direction = actor->mHitDirection;
        u32 attack = actor->mNeedleCyl.mObjAt.mSPrm, target = actor->mNeedleCyl.mObjTg.mSPrm;
        actor->current.angle.y = (s16)(angle + 0x8000);
        if (direction == 8) {
            actor->current.angle.y = (s16)(player->shape_angle.y - 0x4000);
            actor->speedF = 40.0f;
        }
        actor->mNeedleCyl.mObjAt.mSPrm = attack & ~1u;
        actor->mNeedleCyl.mObjTg.mSPrm = target & ~1u;
        actor->mNeedleCyl.ClrTgHit();
        s16 current = actor->current.angle.y;
        u8 mode = actor->mMode;
        actor->mTargetAngleY = current;
        actor->mMode = (u8)(mode + 1);
        [[fallthrough]];
    }
    case 31:
        am2SoundAtEye(actor, 0x50BB);
        cLib_addCalc0(&actor->speedF, 0.8f, 2.0f);
        if (actor->speedF < 0.1f) {
            u8 inactive = actor->mStartsInactive;
            s16 angle = actor->shape_angle.y;
            actor->mAction = ACTION_DOUSA;
            actor->current.angle.y = angle;
            actor->speedF = 0.0f;
            actor->mMode = 3;
            if (inactive == 1 && actor->mSwitch != 0xFF && !am2Switch(actor)) {
                actor->attention_info.flags = 0;
                actor->mCountDownTimers[2] = 600;
                actor->mAction = ACTION_MAHI;
                actor->mMode = 12;
            }
        }
        break;
    }
    if (naraku_check(actor))
        am2Fell(actor);
}

/* 0204D3A4 */
static BOOL daAM2_Execute(am2_class* actor) {
    for (auto& timer : actor->mCountDownTimers) {
        s16 value = timer;
        if (value)
            timer = (s16)(value - 1);
    }
    am2SetGbaName(actor, 0x27, 0xB, 0x29);
    if (am2EnemyIceProcess(&actor->mEnemyIce)) {
        mDoExt_McaMorf* morph = actor->mpMorf;
        Mtx34* matrix = mDoMtx_stack_c::get();
        J3DModel* model = morph->getModel();
        mtx_copy(&model->mBaseTransformMtx, matrix);
        actor->mpMorf->calc();
        return TRUE;
    }
    actor->actor_status = actor->actor_status | 0x400;
    switch ((u8)actor->mAction) {
    case ACTION_DOUSA:
        action_dousa(actor);
        break;
    case ACTION_MAHI:
        action_mahi(actor);
        break;
    case ACTION_ITAI:
        action_itai(actor);
        break;
    case ACTION_HANDOU_MOVE:
        action_handou_move(actor);
        break;
    case ACTION_MODORU_MOVE:
        action_modoru_move(actor);
        break;
    }
    actor->mpMorf->play(nullptr, 0, 0);
    actor->mpBtkAnm->play();
    actor->mpBrkAnm->play();
    Mtx34* matrix = calc_mtx();
    mDoMtx_YrotS(matrix, (s16)actor->current.angle.y);
    gabi::Local<cXyz> offset, rotated;
    gabi::Local<u64> linkage;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = actor->speedF;
    MtxPosition(offset.get(), rotated.get());
    f32 y = actor->speed.y, x = rotated->x, gravity = actor->gravity;
    actor->speed.x = x;
    y += gravity;
    actor->speed.z = rotated->z;
    if (y < -50.0f)
        y = -50.0f;
    f32 posX = actor->current.pos.x, posY = actor->current.pos.y;
    actor->speed.y = y;
    f32 posZ = actor->current.pos.z;
    actor->attention_info.position.x = posX;
    actor->attention_info.position.z = posZ;
    actor->attention_info.position.y = posY + 120.0f;
    actor->eyePos.x = actor->mEyeballPos.x;
    f32 eyeY = actor->mEyeballPos.y;
    actor->eyePos.y = eyeY;
    actor->eyePos.z = actor->mEyeballPos.z;
    actor->eyePos.y = eyeY - (am2Debug().mEyeOffsetAdjust + 15.0f);
    actor->mBodyCyl.SetC(&actor->current.pos);
    actor->mBodyCyl.SetH(150.0f);
    actor->mBodyCyl.SetR(35.0f);
    dComIfG_inf_c* play = am2GameInfo();
    cCcS_Set(&play->play.mCcS, &actor->mBodyCyl);
    actor->mNeedleCyl.SetC(&actor->mNeedlePos);
    actor->mNeedleCyl.SetH(20.0f);
    actor->mNeedleCyl.SetR(am2Debug().mNeedleRadiusAdjust + 55.0f);
    play = am2GameInfo();
    cCcS_Set(&play->play.mCcS, &actor->mNeedleCyl);
    actor->mEyeSph.SetC(&actor->mEyeballPos);
    actor->mEyeSph.SetR(30.0f);
    play = am2GameInfo();
    cCcS_Set(&play->play.mCcS, &actor->mEyeSph);
    actor->mWeakSph.SetC(&actor->mWeakPos);
    actor->mWeakSph.SetR(30.0f);
    play = am2GameInfo();
    cCcS_Set(&play->play.mCcS, &actor->mWeakSph);
    void* collision = (actor->mBodyCyl.mObjCo.mSPrm & 1) ? &actor->mStts : nullptr;
    am2PositionMove(actor, collision);
    if (actor->mAction != ACTION_MODORU_MOVE)
        BG_check(actor);
    draw_SUB(actor);
    return TRUE;
}

// clang-format off
/* WWHD glue
str 10007058 "Am2" in 0204C69C
str 100070E4 "Am2" in 0204FA28
str 100070E0 "Am2" in 0204F9D0
str 10007104 "Am2" in 0204FE84
str 10007108 "amos2" in 0204FE84
data eye_co_sph_src 1018FA90
data week_co_sph_src 1018FAD0
data body_co_cyl_src 1018FB10
data sword_co_cyl_src 1018FB54
data cyl_offset_1 1018FA6C
data sph_offset 1018FA84
data search_data 1018FA54
*/
// clang-format on
