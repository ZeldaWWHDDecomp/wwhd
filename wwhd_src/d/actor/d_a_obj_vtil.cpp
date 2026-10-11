/**
 * d_a_obj_vtil.cpp
 * Object - Tingle statues
 */
#include "d/actor/d_a_obj_vtil.h"
#include <cmath>

daObjVtil_c* daObjVtil_construct(daObjVtil_c*);

static u32 l_stageTbox[5] = {3, 4, 5, 7, 6};
static u32 l_modelRes[5] = {3, 4, 5, 7, 6};
static VtilCylSource l_co_cyl_data;
static VtilZeroWords l_zero;
static VtilMathStatics l_math;
static VtilGlobalDescriptors l_descriptors;
static VtilAction l_modeProc[4] = {{0,-1,0x023B6EE4},{0,-1,0x023B707C},{0,-1,0x023B71A0},{0,-1,0x023B7210}};
static void correctBackground(daObjVtil_c* statue) {
    statue->mAcch.CrrPos(dComIfG_Bgsp());
}
static void stopTracking(daObjVtil_c* statue) {
    VtilCamera* camera = reinterpret_cast<VtilCamera*>(dComIfGp_getCamera(0));
    u32 processID = statue->mBsPcId;
    vtilForceLockOff(&camera->mCamera, processID);
}

/* 023B6028 */
static BOOL solidHeapCB(fopAc_ac_c* actor) {
    return static_cast<daObjVtil_c*>(actor)->create_heap();
}

/* 023B5F70 */
BOOL daObjVtil_c::create_heap() {
    u32 type = mStatueType;
    // HD: this TU uses the legacy resource lookup and its SafeString vtable.
    gabi::Local<SafeString> archive;
    archive->__vtbl = VT_ARCHIVE;
    archive->mStringTop = gabi::guest_str("Vtil");
    u32 resource = u32(l_modelRes[type]);
    J3DModelData* modelData = vtilGetResource(dComIfG_resControl(), archive.get(), resource);
    if (!modelData) {
        JUT_ASSERT_fail("d_a_obj_vtil.cpp", 0x147, "mdl_data != 0");
        return 0;
    }
    mModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    return mModel.get() != nullptr;
}

/* 023B73D0 */
u32 daObjVtil_c::getPrm(u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 parameters = (shift & 32) ? 0 : (u32(mParameters) >> (shift & 31));
    return parameters & (mask - 1);
}

/* 023B6310 */
s32 daObjVtil_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            daObjVtil_construct(this);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    u32 type = getPrm(4, 0);
    if (type == 15 || type == 0xFFFFFFFF) type = 0;
    mStatueType = type;
    if (!vtilIsStageTbox(u32(l_stageTbox[type]), 15)) return 5;
    s32 phase = dComIfG_resLoad(&mPhase, "Vtil");
    if (phase != 4) return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0xCC0)) return 5;
    J3DModel* model = mModel;
    cullMtx = gabi::ea(model ? &model->mBaseTransformMtx : nullptr);
    init_mtx();
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 160.0f, 50.0f);
    init_co();
    init_bgc();
    gravity = -6.0f;
    renew_attention_pos();
    u32 attention = attention_info.flags;
    f32 y = current.pos.y, z = current.pos.z;
    attention_info.flags = attention | fopAc_Attn_ACTION_CARRY_e;
    attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = 0x17;
    eyePos.y = y + 110.0f;
    eyePos.z = z;
    mFirstGroundHit = 1;
    eyePos.x = current.pos.x;
    to_wait_mode();
    dKy_plight_set(&mLight);
    this->model = gabi::ea(mModel.get());
    return 4;
}

/* 023B64BC */
BOOL daObjVtil_c::_delete() {
    dKy_plight_cut(&mLight);
    model = 0;
    dComIfG_resDelete(&mPhase, "Vtil");
    return 1;
}

/* 023B602C */
void daObjVtil_c::init_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* model = mModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x; y = current.pos.y; z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), s16(shape_angle.x), s16(shape_angle.y), s16(shape_angle.z));
    // Snapshot the matrix before storing: model storage may overlap the stack matrix.
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = f32(mDoMtx_stack_c::get()->m[i / 4][i % 4]);
    model = mModel;
    for (u32 i = 0; i < 12; ++i) model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    mModel->calc();
}

/* 023B6114 */
void daObjVtil_c::init_co() {
    mCollisionStatus.Init(200, 255, this);
    mCylinder.Set(reinterpret_cast<dCcD_SrcCyl*>(&l_co_cyl_data));
    mCylinder.SetStts(&mCollisionStatus);
    mCylinder.mCyl.SetC(&current.pos);
    vtilWord(mCylinder.mGObjAt.mVec.x) = u32(l_zero.x);
    vtilWord(mCylinder.mGObjAt.mVec.y) = u32(l_zero.y);
    vtilWord(mCylinder.mGObjAt.mVec.z) = u32(l_zero.z);
    u32 x = u32(l_zero.x);
    u32 flags = mCylinder.mGObjTg.mSPrm;
    vtilWord(mCylinder.mGObjTg.mVec.x) = x;
    vtilWord(mCylinder.mGObjTg.mVec.y) = u32(l_zero.y);
    u32 z = u32(l_zero.z);
    mCylinder.mGObjTg.mSPrm = flags | 1;
    vtilWord(mCylinder.mGObjTg.mVec.z) = z;
}

/* 023B61B4 */
void daObjVtil_c::init_bgc() {
    mAcchCircle.SetWall(30.0f, 56.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCircle, &speed, &current.angle, &shape_angle);
    u32 flags = mAcch.m_flags;
    mAcch.m_roof_crr_height = 160.0f;
    mAcch.m_flags = (flags & ~u32(dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE)) | dBgS_Acch::SEA_CHECK | dBgS_Acch::SEA_WATER_HEIGHT;
    fopAcM_posMoveF(this, 0);
    correctBackground(this);
    mAcch.m_flags = u32(mAcch.m_flags) & ~u32(dBgS_Acch::GROUND_LANDING);
}

/* 023B6268 */
void daObjVtil_c::renew_attention_pos() {
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    attention_info.position.z = z;
    attention_info.position.x = x;
    attention_info.position.y = y + 160.0f;
}

/* 023B6868 */
void daObjVtil_c::make_smoke() {
    vtilMakeLandEffect(this, &mAcch.m_gnd, 1.0f);
}

/* 023B6878 */
void daObjVtil_c::se_smoke() {
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    s32 material = play->play.mBgS.GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    s32 reverb = dComIfGp_getReverb(s32(s8(current.roomNo)));
    mDoAud_seStart(0x6929, &eyePos, material, reverb);
}

/* 023B663C */
void daObjVtil_c::make_splash() {
    gabi::Local<cXyz> position;
    position->x = f32(current.pos.x);
    position->y = f32(mAcch.m_sea_height);
    position->z = f32(current.pos.z);
    fopKyM_createWpillar(position.get(), 1.0f, 0.75f, 0);
}

/* 023B668C */
void daObjVtil_c::se_splash() {
    s32 material = 19;
    cBgS_PolyInfo* polygons[] = {
        reinterpret_cast<cBgS_PolyInfo*>(&mAcch.m_wtr),
        static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd),
    };
    for (cBgS_PolyInfo* polygon : polygons) {
        if (polygon->mBgIndex < 256) {
            dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            material = play->play.mBgS.GetMtrlSndId(polygon);
            break;
        }
    }
    s32 reverb = dComIfGp_getReverb(s32(s8(current.roomNo)));
    mDoAud_seStart(0x6918, &eyePos, material, reverb);
    set_sound(125, 5);
}

/* 023B6508 */
void daObjVtil_c::set_sound(s32 sound, s32 mode) {
    gabi::Local<cXyz> position;
    position->x = f32(current.pos.x);
    position->z = f32(current.pos.z);
    u32 processID = mBsPcId;
    position->y = f32(current.pos.y);
    dKy_Sound_set(position.get(), sound, processID, mode);
}

/* 023B6290 */
void daObjVtil_c::to_wait_mode() {
    u32 attack = mCylinder.mObjAt.mSPrm;
    u32 collision = mCylinder.mObjCo.mSPrm;
    u32 target = mCylinder.mObjTg.mSPrm;
    gravity = -6.0f;
    mCylinder.mObjAt.mSPrm = attack & ~1u;
    speedF = 0.0f;
    mCylinder.mObjTg.mSPrm = target | 1;
    mCylinder.mObjCo.mSPrm = collision | 1;
    mCollisionStatus.Init(200, 255, this);
    mMode = MODE_WAIT;
}

/* 023B6EE4 */
void daObjVtil_c::mode_wait() {
    if (u32(actor_status) & fopAcStts_CARRY_e) {
        to_carry_mode();
        mPreviousVerticalSpeed = f32(speed.y);
        correctBackground(this);
        return;
    }
    u32 flags = mAcch.m_flags;
    if (flags & dBgS_Acch::GROUND_LANDING) {
        if (!u8(mFirstGroundHit)) {
            make_smoke();
            se_smoke();
            make_vib();
        } else mFirstGroundHit = 0;
        flags = mAcch.m_flags;
        mAcch.m_flags = flags & ~u32(dBgS_Acch::GROUND_LANDING);
    }
    u32 attention = attention_info.flags;
    attention_info.flags = (flags & dBgS_Acch::GROUND_HIT) ? attention | fopAc_Attn_ACTION_CARRY_e : attention & ~u32(fopAc_Attn_ACTION_CARRY_e);
    fopAcM_posMoveF(this, &mCollisionStatus.m_cc_move);
    mPreviousVerticalSpeed = f32(speed.y);
    correctBackground(this);
}

/* 023B6EA8 */
void daObjVtil_c::to_carry_mode() {
    u32 collision = mCylinder.mObjCo.mSPrm;
    u32 attack = mCylinder.mObjAt.mSPrm;
    u32 target = mCylinder.mObjTg.mSPrm;
    u32 attention = attention_info.flags;
    mCylinder.mObjCo.mSPrm = collision & ~1u;
    mCylinder.mObjTg.mSPrm = target | 1;
    attention_info.flags = attention & ~u32(fopAc_Attn_ACTION_CARRY_e);
    mMode = MODE_CARRY;
    mCylinder.mObjAt.mSPrm = attack & ~1u;
}

/* 023B707C */
void daObjVtil_c::mode_carry() {
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    f32 speedX = speed.x, speedY = speed.y, speedZ = speed.z;
    f32 vertical = speedY;
    if (!(u32(actor_status) & fopAcStts_CARRY_e)) {
        if (f32(speedF) > 0.0f) to_throw_mode();
        else to_wait_mode();
        vertical = speed.y;
    }
    mPreviousVerticalSpeed = vertical;
    correctBackground(this);
    speed.y = speedY;
    current.pos.x = x;
    speed.x = speedX;
    speed.z = speedZ;
    current.pos.y = y;
    current.pos.z = z;
}

/* 023B6FE0 */
void daObjVtil_c::to_throw_mode() {
    dComIfGp_get();
    u32 collision = mCylinder.mObjCo.mSPrm;
    u32 flags = mAcch.m_flags;
    u32 attack = mCylinder.mObjAt.mSPrm;
    u32 target = mCylinder.mObjTg.mSPrm;
    speed.y = 27.0f;
    gravity = -6.0f;
    mCylinder.mObjAt.mSPrm = attack | 1;
    mCylinder.mObjCo.mSPrm = collision | 1;
    attention_info.flags = attention_info.flags & ~0x10u;
    speedF = 36.0f;
    mCylinder.mObjTg.mSPrm = target | 1;
    mAcch.m_flags = (flags & ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE | dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE)) | dBgS_Acch::LINE_CHECK;
    mMode = MODE_THROW;
}

/* 023B6DA0 */
void daObjVtil_c::calc_throw(be<f32>* gravityOut, be<f32>* linearOut, be<f32>* angularOut) {
    f32 linear = 0.002f, angular = 0.0002f, gravity = -6.0f;
    if (u32(mAcch.m_flags) & dBgS_Acch::SEA_IN) {
        f32 depth = f32(current.pos.y) - f32(mAcch.m_sea_height);
        f32 blend = 0.0f;
        if (depth < 0.0f) {
            if (!(depth > -160.0f)) blend = 0.5f;
            else blend = -(depth * 0.003125f);
        }
        f32 rest = 1.0f - blend;
        linear = std::fma(blend, 0.2f, rest * 0.002f);
        angular = std::fma(blend, 0.02f, rest * 0.0002f);
        gravity = std::fma(blend, 4.0f, -6.0f);
    }
    // HD: output pointers can overlap; retain the original store order.
    *linearOut = linear;
    *angularOut = angular;
    *gravityOut = gravity;
}

/* 023B71A0 */
void daObjVtil_c::mode_throw() {
    gabi::Local<be<f32>> gravity, linear, angular;
    calc_throw(gravity.get(), linear.get(), angular.get());
    f32 gravityValue = *gravity, linearValue = *linear, angularValue = *angular;
    this->gravity = gravityValue;
    daObj_posMoveF_stream(this, &mCollisionStatus.m_cc_move, reinterpret_cast<cXyz*>(&l_zero), linearValue, angularValue);
    mPreviousVerticalSpeed = f32(speed.y);
    correctBackground(this);
}

/* 023B6770 */
void daObjVtil_c::to_sink_mode() {
    f32 forwardSquared = f32(speedF) * f32(speedF);
    u32 flags = mAcch.m_flags;
    u32 collision = mCylinder.mObjCo.mSPrm;
    u32 attack = mCylinder.mObjAt.mSPrm;
    u32 target = mCylinder.mObjTg.mSPrm;
    mCylinder.mObjCo.mSPrm = collision | 1;
    f32 vertical = speed.y;
    mCylinder.mObjAt.mSPrm = attack & ~1u;
    f32 squared = std::fma(vertical, vertical, forwardSquared);
    mCylinder.mObjTg.mSPrm = target | 1;
    gravity = -2.0f;
    mAcch.m_flags = ((flags | dBgS_Acch::ROOF_NONE) & ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE | dBgS_Acch::WATER_NONE)) | dBgS_Acch::LINE_CHECK;
    // HD: keep the full floating-point return register through the square-root call.
    f64 magnitude = vtilSqrtRegister(squared);
    if (magnitude > 30.0) {
        f32 factor = f32(30.0 / magnitude);
        PSVECScale(&speed, &speed, factor);
        speedF = f32(speedF) * factor;
    }
    make_splash();
    se_splash();
    u32 attention = attention_info.flags;
    mMode = MODE_SINK;
    attention_info.flags = attention & ~u32(fopAc_Attn_ACTION_CARRY_e);
}

/* 023B7210 */
void daObjVtil_c::mode_sink() {
    daObj_posMoveF_stream(this, &mCollisionStatus.m_cc_move, reinterpret_cast<cXyz*>(&l_zero), 0.2f, 0.02f);
    mPreviousVerticalSpeed = f32(speed.y);
    correctBackground(this);
}

/* 023B654C */
void daObjVtil_c::hit_co() {
    mCollisionStatus.Move();
    if (mCylinder.ChkAtHit()) {
        mCylinder.ClrAtHit();
        speedF = f32(speedF) * 0.8f;
        return;
    }
    if (mCylinder.ChkTgHit()) {
        vtilHitSeStart(&eyePos, s32(s8(current.roomNo)), &mCylinder, 13);
        set_sound(150, 5);
        vtilHitEffHibana(this, &mCylinder);
        mCylinder.ClrTgHit();
    }
}

/* 023B6604 */
BOOL daObjVtil_c::check_sink() {
    if (!(u32(mAcch.m_flags) & dBgS_Acch::SEA_IN)) return 0;
    return f32(mAcch.m_sea_height) > f32(current.pos.y) + 70.0f;
}

/* 023B69BC */
BOOL daObjVtil_c::check_sink_end() {
    if (!(u32(mAcch.m_flags) & dBgS_Acch::SEA_IN)) return 0;
    return f32(mAcch.m_sea_height) > f32(current.pos.y) + 210.0f;
}

/* 023B69F4 */
void daObjVtil_c::hit_bg() {
    u32 flags = mAcch.m_flags;
    bool groundHit = (flags & dBgS_Acch::GROUND_HIT) != 0;
    BOOL sinking = check_sink();
    u32 mode = mMode;
    if (mode == MODE_WAIT || (mode == MODE_THROW && !(groundHit || (flags & (dBgS_Acch::WALL_HIT | dBgS_Acch::ROOF_HIT))))) {
        if (sinking) to_sink_mode();
    } else if (mode == MODE_THROW) {
        if (f32(mPreviousVerticalSpeed) < f32(gravity) - 1.0f) {
            speedF = f32(speedF) * 0.6f;
            return;
        }
        if (groundHit) {
            make_smoke();
            se_smoke();
            make_vib();
        }
        speedF = 0.0f;
        to_wait_mode();
        stopTracking(this);
    } else if (mode == MODE_SINK) {
        if (check_sink_end()) {
            mDeleteState = 1;
            return;
        }
        if (!sinking) {
            to_wait_mode();
            stopTracking(this);
        }
    }
}

/* 023B695C */
void daObjVtil_c::make_vib() {
    u32 near = u32(check_circle()) & 1;
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::Local<cXyz> direction;
    direction->x = 0.0f; direction->y = 1.0f; direction->z = 0.0f;
    play->play.mVibration.StartShock( near + 1, 1, direction.get());
}

/* 023B68DC */
BOOL daObjVtil_c::check_circle() {
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    gabi::Local<cXyz> difference;
    cXyz_mi(&player->current.pos, difference.get(), &current.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = f32(difference->x);
    horizontal->y = 0.0f;
    horizontal->z = f32(difference->z);
    // HD: this call chain consumes the full FPR result, rather than a narrowed float.
    f64 squared = vtilSquareMagRegister(horizontal.get());
    // HD: keep the full floating-point return register through the square-root call.
    f64 magnitude = vtilSqrtRegister(squared);
    return !(magnitude > 190.0);
}

/* 023B6B70 */
BOOL daObjVtil_c::_execute() {
    hit_co();
    hit_bg();
    if (!u8(mDeleteState)) {
        u32 mode = mMode;
        if (mode >= MODE_MAX) {
            JUT_ASSERT_fail("d_a_obj_vtil.cpp", 0x3EA, "M_mode >= 0 && M_mode < MODE_MAX");
            mode = mMode;
        }
        l_modeProc[mode].invoke(this);
        renew_attention_pos();
        init_mtx();
        vtilMoveCAtTg(&mCylinder, &current.pos);
        dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        cCcS_Set(&play->play.mCcS, &mCylinder);
        f32 y = current.pos.y, z = current.pos.z;
        mLight.mColorB = 0x34;
        mLight.mPos.z = z;
        mLight.mPower = 230.0f;
        mLight.mFluctuation = 250.0f;
        mLight.mColorR = 0x94;
        mLight.mColorG = 0x7F;
        mLight.mPos.y = y + 70.0f;
        mLight.mPos.x = f32(current.pos.x);
    }
    if (u8(mDeleteState) == 1) {
        fopAcM_delete(this);
        mDeleteState = 2;
    }
    return 1;
}

/* 023B6D44 */
BOOL daObjVtil_c::_draw() {
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    settingTevStruct(environment, 0, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, mModel.get(), &tevStr);
    mDoExt_modelUpdateDL(mModel.get(), 0);
    return 1;
}

/* 023B7270 */
static s32 Mthd_Create(daObjVtil_c* statue) {
    return statue->_create();
}

/* 023B7274 */
static BOOL Mthd_Delete(daObjVtil_c* statue) {
    return statue->_delete();
}

/* 023B7278 */
static BOOL Mthd_Execute(daObjVtil_c* statue) {
    return statue->_execute();
}

/* 023B727C */
static BOOL Mthd_Draw(daObjVtil_c* statue) {
    return statue->_draw();
}

/* 023B73C8 */
static BOOL Mthd_IsDelete(daObjVtil_c* statue) {
    return 1;
}

/* 023B5E60 */
daObjVtil_c* daObjVtil_construct(daObjVtil_c* statue) {
    if (!statue) statue = static_cast<daObjVtil_c*>(operator_new(sizeof(daObjVtil_c)));
    if (!statue) return nullptr;
    fopAc_ac_c_ct(statue);
    statue->__vtbl = VT_ACTOR;
    vtilAcchConstruct(&statue->mAcch);
    statue->mAcch.cBgS_Chk::__vtbl = VT_ACCH_CHECK;
    statue->mAcch.dBgS_GrpPassChk::__vtbl = VT_ACCH_GROUP;
    statue->mAcch.dBgS_PolyPassChk::__vtbl = VT_ACCH_POLYGON;
    statue->mAcch.mbObjThrough = 1;
    dBgS_AcchCir_ct(&statue->mAcchCircle);
    vtilStatusConstruct(&statue->mCollisionStatus);
    vtilGlobalStatusConstruct(&statue->mCollisionStatus.__vtbl_gstts);
    statue->mCollisionStatus.__vtbl = VT_STATUS;
    statue->mCollisionStatus.__vtbl_gstts = VT_GLOBAL_STATUS;
    vtilColliderConstruct(&statue->mCylinder);
    vtilShape(&statue->mCylinder)->mShapeVtable = VT_SHAPE;
    vtilShape(&statue->mCylinder)->mBoundsVtable = VT_BOUNDS;
    vtilCylinderConstruct(&statue->mCylinder.mCyl);
    statue->mLight.mHD20 = 1.0f;
    statue->mCylinder.mCyl.__vtbl = VT_CYLINDER;
    statue->mCylinder.__vtbl_hitinf = VT_CYL_HIT;
    vtilShape(&statue->mCylinder)->mShapeVtable = VT_CYL_SHAPE;
    return statue;
}

/* 023B7280 */
void __sinit_d_a_obj_vtil_cpp() {
    l_math.mZero[0] = 0;
    l_math.mZero[1] = 0;
    l_math.mZero[2] = 0;
    l_math.mZero[3] = 0;
    __register_global_object(gabi::ea(&l_descriptors.mEntries[0]));
    l_math.mMinAngle = -3.1415927410125732f;
    l_math.mMaxAngle = 3.1415927410125732f;
    vtilInitSinTable(&l_math.mSinTable);
    __register_global_object(gabi::ea(&l_descriptors.mEntries[1]));
    vtilInitAtanTable(&l_math.mAtanTable);
    __register_global_object(gabi::ea(&l_descriptors.mEntries[2]));
}

/* 023B7314 */
void __dt_VtilHeaderStatics(void* object, u32 flags) {
    if (object && (flags & 1)) operator_delete(object);
}

/* 023B7328 */
void daObjVtil_destruct(daObjVtil_c* statue, u32 flags) {
    if (!statue) return;
    dCcD_Cyl_dt(&statue->mCylinder, 2);
    dCcD_Stts_dt(&statue->mCollisionStatus, 2);
    vtilCircleDestruct(&statue->mAcchCircle.m_cir, 2);
    statue->mAcch.dBgS_GrpPassChk::__vtbl = VT_ACCH_GROUP;
    statue->mAcch.dBgS_PolyPassChk::__vtbl = VT_ACCH_POLYGON;
    vtilAcchDestruct(&statue->mAcch, 0);
    vtilActorDestruct(statue, 0);
    if (flags & 1) operator_delete(statue);
}

/* 023B73C4 */
void daObjVtil_emptyVirtual(void* object) {
}

/* WWHD glue
data l_stageTbox 100332D8
data l_modelRes 100332EC
data l_co_cyl_data 10033300
data l_zero 101FFBA8
data l_math 1046C950
data l_descriptors 101CDCD0
data l_modeProc 10033244
ext 025165A4 void vtilMoveCAtTg(dCcD_Cyl*, cXyz*)
ext 025052BC void vtilForceLockOff(VtilCameraController*, u32)
ext 026066C4 J3DModelData* vtilGetResource(void*, SafeString*, u32)
ext 02520864 BOOL vtilIsStageTbox(u32, s32)
ext 0255F458 void dKy_Sound_set(cXyz*, s32, u32, s32)
ext 023129C4 void vtilHitSeStart(cXyz*, s32, dCcD_GObjInf*, u32)
ext 02312E54 void vtilHitEffHibana(fopAc_ac_c*, dCcD_Cyl*)
ext 02311CD8 void vtilMakeLandEffect(fopAc_ac_c*, dBgS_GndChk*, f32)
ext 028F4384 f64 vtilSqrtRegister(f32)
ext 028E8DD0 f64 vtilSquareMagRegister(const cXyz*)
ext 028ED6F8 void vtilInitSinTable(u8*)
ext 028EAB2C void vtilInitAtanTable(u8*)
ext 024F0474 void vtilAcchConstruct(dBgS_Acch*)
ext 0200BD2C void vtilStatusConstruct(dCcD_Stts*)
ext 02515DA0 void vtilGlobalStatusConstruct(be<u32>*)
ext 02515FB8 void vtilColliderConstruct(dCcD_GObjInf*)
ext 02018590 void vtilCylinderConstruct(cM3dGCyl*)
ext 02018034 void vtilCircleDestruct(cM3dGCir_l*, s32)
ext 024EFD9C void vtilAcchDestruct(dBgS_Acch*, s32)
ext 025D50BC void vtilActorDestruct(fopAc_ac_c*, s32)
str 100332D0 "Vtil"
str 100331E0 "d_a_obj_vtil.cpp" in 023B5F70
str 100331D0 "mdl_data != 0"
str 10033264 "d_a_obj_vtil.cpp" in 023B6B70
str 10033278 "M_mode >= 0 && M_mode < MODE_MAX"
*/
