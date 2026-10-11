/** Earth Temple coffin, derived from HD disassembly; */
#include "d/actor/d_a_obj_kanoke.h"
#include "bindings.h"

static KanokeActions l_modeProc;
static KanokeMathStatics l_math;
static KanokeGlobalDescriptors l_descriptors;
static cXyz l_lidEffectOffset;
static KanokeLidPoints l_lidPoints;
static KanokeCapsuleSource l_bodyCpsSource;
static KanokeCapsuleSource l_lidCpsSource;
// HD: the tww unit leaves this coffin implementation nonmatching; fields retain verified HD behavior.

void* KanokeCapsule_construct(void* self);
daObjKanoke_c* daObjKanoke_construct(daObjKanoke_c* actor);
// HD: this legacy resource ABI uses the TU SafeString vtable and four caller-owned temporaries.
struct KanokeSafeString {
    be<u32> text, vtable;
};
void* kanokeResource(gabi::Local<KanokeSafeString>& name, u32 id) {
    name->text = gabi::guest_str("Mkanoke");
    name->vtable = VT_ARCHIVE;
    return kanokeGetResource(dComIfG_resControl(), name.get(), id);
}
/* 0236743C */
BOOL daObjKanoke_c::createHeap() {
    gabi::Local<KanokeSafeString> bodyName, bodyCollisionName, lidName, lidCollisionName;
    void* data = kanokeResource(bodyName, RES_BODY_MODEL);
    if (!data)
        return 0;
    J3DModel* model = mDoExt_J3DModel__create(static_cast<J3DModelData*>(data), 0, 0x11020203);
    mBodyModel = model;
    if (!model)
        return 0;
    dBgW* bg = new_dBgW();
    mpBodyBgW = bg;
    if (!bg)
        return 0;
    data = kanokeResource(bodyCollisionName, RES_BODY_COLLISION);
    if (mpBodyBgW->Set(static_cast<cBgD_t*>(data), 1, &mBodyMtx))
        return 0;
    data = kanokeResource(lidName, RES_LID_MODEL);
    if (!data)
        return 0;
    model = mDoExt_J3DModel__create(static_cast<J3DModelData*>(data), 0, 0x11020203);
    mLidModel = model;
    if (!model)
        return 0;
    bg = new_dBgW();
    mpLidBgW = bg;
    if (!bg)
        return 0;
    data = kanokeResource(lidCollisionName, RES_LID_COLLISION);
    return mpLidBgW->Set(static_cast<cBgD_t*>(data), 1, &mLidMtx) == 0;
}
/* 023675C0 */
BOOL CheckCreateHeap(daObjKanoke_c* a) {
    return a->createHeap();
}
/* 023675C4 */
u8 daObjKanoke_c::getPrmType() {
    return (u8)getPrm(1, 0);
}
/* 023675F0 */
u8 daObjKanoke_c::getPrmSearch() {
    return (u8)getPrm(5, 1);
}
/* 0236761C */
u8 daObjKanoke_c::getPrmSwNo() {
    return (u8)getPrm(8, 8);
}
/* 02367648 */
u8 daObjKanoke_c::getPrmSwNo2() {
    return (u8)getPrm(8, 16);
}
/* 02367674 */
void daObjKanoke_c::setMtxHontai() {
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x);
    mDoMtx_stack_c::transM((f32)mPivot.x, (f32)mPivot.y, (f32)mPivot.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.y);
    mDoMtx_stack_c::transM(-(f32)mPivot.x, -(f32)mPivot.y, -(f32)mPivot.z);
}
/* 0236770C */
void daObjKanoke_c::setMtxHuta(cXyz* position) {
    gabi::Local<cXyz> offset;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x);
    PSMTXMultVec(mDoMtx_stack_c::get(), &mLidOffset, offset.get());
    f32 x = (f32)position->x + (f32)offset->x, y = (f32)position->y + (f32)offset->y,
        z = (f32)position->z + (f32)offset->z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x);
    mDoMtx_stack_c::transM((f32)mPivot.x, (f32)mPivot.y, (f32)mPivot.z);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.x);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.y);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.z);
    mDoMtx_stack_c::transM(-(f32)mPivot.x, -(f32)mPivot.y, -(f32)mPivot.z);
}
void kanokeCopyModelMatrix(J3DModel* model) {
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 02367A38 */
void daObjKanoke_c::setMtx() {
    u8 flags = mHidden;
    if (!(flags & 1)) {
        setMtxHontai();
        kanokeCopyModelMatrix(mBodyModel);
        PSMTXCopy(mDoMtx_stack_c::get(), &mBodyMtx);
        flags = mHidden;
    }
    if (!(flags & 2)) {
        setMtxHuta(&current.pos);
        kanokeCopyModelMatrix(mLidModel);
        PSMTXCopy(mDoMtx_stack_c::get(), &mLidMtx);
    }
}
/* 02367B7C */
s32 daObjKanoke_c::createInit() {
    if (dComIfG_Bgsp()->Regist(mpBodyBgW, this))
        return cPhs_ERROR_e;
    u8 sw = mSwNo;
    bool opened = false;
    if (sw != 255) {
        opened = dComIfGs_isSwitch(sw, (s8)home.roomNo) != 0;
    }
    if (opened) {
        u8 type = mType;
        mState = STATE_WAIT;
        if (type == TYPE_YOKO) {
            if (dComIfG_Bgsp()->Regist(mpLidBgW, this))
                return cPhs_ERROR_e;
            mLidOffset.z = 0;
            mPivot.y = 0;
            mPivot.z = 0;
            mLidOffset.y = 75;
            mLidAngle.z = -5600;
            mLidOffset.x = 148;
            mPivot.x = -48;
        } else
            mHidden = (u8)mHidden | 2;
    } else {
        if (dComIfG_Bgsp()->Regist(mpLidBgW, this))
            return cPhs_ERROR_e;
    }
    mStts.Init(255, 255, this);
    kanokeSetCapsule(&mBodyCps, &l_bodyCpsSource);
    mBodyCps.SetStts(&mStts);
    kanokeSetCapsuleEnds(&mBodyCps.mCps, &current.pos, &current.pos);
    for (int i = 0; i < 3; i++) {
        KanokeCapsule* capsule = &mLidCps[i];
        kanokeSetCapsule(capsule, &l_lidCpsSource);
        capsule->SetStts(&mStts);
        kanokeSetCapsuleEnds(&capsule->mCps, &current.pos, &current.pos);
    }
    J3DModel* model = mBodyModel;
    cullMtx = gabi::ea(model ? &model->mBaseTransformMtx : nullptr);
    setMtx();
    if (mType == TYPE_YOKO)
        fopAcM_setCullSizeBox(this, -110.0f, 0.0f, -210.0f, 310.0f, 120.0f, 210.0f);
    else
        fopAcM_setCullSizeBox(this, -110.0f, 0.0f, -210.0f, 110.0f, 520.0f, 210.0f);
    return cPhs_COMPLEATE_e;
}
/* 02367E94 */
s32 daObjKanoke_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            daObjKanoke_construct(this);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhase, "Mkanoke");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(CheckCreateHeap)),
                              0x2400))
        return createInit();
    mpLidBgW = nullptr;
    mpBodyBgW = nullptr;
    return cPhs_ERROR_e;
}
/* 02367F48 */
s32 daObjKanokeCreate(daObjKanoke_c* a) {
    return a->_create();
}
/* 02367F4C */
BOOL daObjKanoke_c::_delete() {
    if (heap) {
        dBgW* bg = mpBodyBgW;
        if (bg && bg->m_id < 256) {
            dComIfG_Bgsp()->Release(mpBodyBgW);
        }
        bg = mpLidBgW;
        if (bg && bg->m_id < 256) {
            dComIfG_Bgsp()->Release(mpLidBgW);
        }
    }
    mSmoke.remove();
    dComIfG_resDelete(&mPhase, "Mkanoke");
    return 1;
}
/* 02367FF8 */
BOOL daObjKanokeDelete(daObjKanoke_c* a) {
    return a->_delete();
}
/* 02367FFC */
BOOL daObjKanoke_c::_execute() {
    gabi::Local<cXyz> start, end;
    start->x = 0;
    start->y = 0;
    start->z = -100;
    end->x = 0;
    end->y = 0;
    end->z = 100;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x);
    PSMTXMultVec(mDoMtx_stack_c::get(), start.get(), start.get());
    PSMTXMultVec(mDoMtx_stack_c::get(), end.get(), end.get());
    PSVECAdd(start.get(), &current.pos, start.get());
    PSVECAdd(end.get(), &current.pos, end.get());
    kanokeSetCapsuleEnds(&mBodyCps.mCps, start.get(), end.get());
    mBodyCps.mCps.mRadius = 140;
    cCcS_Set(dComIfG_Ccsp(), &mBodyCps);
    l_modeProc.entries[(u8)mState].invoke(this);
    setMtx();
    dBgW* bg = mpBodyBgW;
    if (bg->m_id < 256)
        bg->Move();
    bg = mpLidBgW;
    if (bg->m_id < 256)
        bg->Move();
    return 1;
}
/* 02368188 */
BOOL daObjKanokeExecute(daObjKanoke_c* a) {
    return a->_execute();
}
/* 0236818C */
BOOL daObjKanoke_c::_draw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mBodyModel, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mLidModel, &tevStr);
    dComIfGd_setListBG();
    u8 flags = mHidden;
    if (!(flags & 1)) {
        mDoExt_modelUpdateDL(mBodyModel, 0);
        flags = mHidden;
    }
    if (!(flags & 2))
        mDoExt_modelUpdateDL(mLidModel, 0);
    dComIfGd_setList();
    return 1;
}
/* 02368258 */
BOOL daObjKanokeDraw(daObjKanoke_c* a) {
    return a->_draw();
}
/* 0236825C */
u8 daObjKanoke_c::getPrmYure() {
    return (u8)getPrm(1, 6);
}
void kanokeSound(daObjKanoke_c* a, u32 id) {
    s32 reverb = dComIfGp_getReverb((s8)a->current.roomNo);
    mDoAud_seStart(id, &a->current.pos, 0, reverb);
}
void kanokeReleaseLid(daObjKanoke_c* a) {
    dBgW* bg = a->mpLidBgW;
    if (bg->m_id < 256) {
        dComIfG_Bgsp()->Release(a->mpLidBgW);
    }
}
/* 02368288 */
void daObjKanoke_c::executeNormal() {
    u8 sw = mSwNo;
    bool trigger = false;
    if (sw != 255) {
        trigger = dComIfGs_isSwitch(sw, (s8)home.roomNo) != 0;
    }
    if (trigger)
        mBodyCps.ClrTgHit();
    else {
        bool lit = kanokeCheckLight(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mDetect,
                                    &current.pos) != 0;
        if (!lit)
            lit = mBodyCps.ChkTgHit() != 0;
        if (lit) {
            s16 count = (s16)((s16)mLightTimer + 1);
            mLightTimer = count;
            if (count <= 20)
                return;
            mBodyCps.mTgType = 0;
            mBodyCps.ClrTgHit();
        } else {
            u8 search = mSearchRange;
            mLightTimer = 0;
            if (!search)
                return;
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            gabi::Local<cXyz> delta;
            cXyz_mi(&home.pos, delta.get(), &player->current.pos);
            // HD: this call chain compares the unrounded FPR result.
            f64 square = kanokeSquareMagnitudeFPR(delta.get());
            f64 distance = kanokeSqrtFPR(square);
            f32 limit = (f32)(u8)mSearchRange * 100.0f;
            if (!(distance < limit))
                return;
            mBodyCps.ClrTgHit();
        }
    }
    sw = mSwNo;
    if (sw != 255) {
        dComIfGs_onSwitch(sw, (s8)home.roomNo);
    }
    mAngularSpeed = 0;
    bool shake = getPrmYure() != 0;
    if (shake) {
        s8 room = current.roomNo;
        u8 type = mType;
        mTimer = 0;
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(type == TYPE_YOKO ? 0x695b : 0x6959, &current.pos, 0, reverb);
        if (type == TYPE_YOKO) {
            mLidAngle.z = 0;
            mShakeSpeed = 256;
            mState = STATE_YURE_YOKO;
        } else {
            mLidAngle.y = 0;
            mShakeSpeed = 1024;
            mState = STATE_YURE_TATE;
        }
    } else {
        u8 type = mType;
        kanokeSound(this, type == TYPE_YOKO ? 0x695c : 0x695a);
        if (type == TYPE_YOKO) {
            mLidAngle.z = 0;
            mState = STATE_OPEN_YOKO;
        } else {
            mState = STATE_OPEN_TATE;
            kanokeReleaseLid(this);
            mLidAngle.x = 0;
        }
    }
}
/* 02368534 */
void daObjKanoke_c::executeYureYoko() {
    s16 speed = (s16)((s16)mShakeSpeed - 8);
    u16 phase = (u16)((s16)mTimer + 0x2000);
    mTimer = (s16)phase;
    mShakeSpeed = speed;
    f32 sine = cM_ssin(phase);
    s16 angle = (s16)gabi::ftoi(sine * (f32)speed);
    mLidAngle.z = angle;
    s16 remaining = mShakeSpeed;
    mPivot.x = angle < 0 ? 100.0f : -100.0f;
    if (remaining <= 0) {
        kanokeSound(this, 0x695c);
        mLidAngle.z = 0;
        mPivot.x = 0;
        mState = STATE_OPEN_YOKO;
    }
}
void kanokeSmokeBegin(daObjKanoke_c* a, u32 id, u8 alpha) {
    dComIfGp_getParticle()->set(2, id, &a->mSmokePos, &a->mSmokeAngle, nullptr, alpha,
                                (dPa_levelEcallBack*)&a->mSmoke, -1, nullptr, nullptr, nullptr);
}
void kanokeFlagSmoke(KanokeEmitter* emitter) {
    if (emitter)
        emitter->mFlags = u32(emitter->mFlags) | 0x40;
}
/* 0236863C */
void daObjKanoke_c::executeOpenYoko() {
    f32 x = (f32)mLidOffset.x + 4.0f;
    mLidOffset.x = x;
    mPivot.x = 100.0f - x;
    if (!(x > 100.0f))
        return;
    s16 speed = mAngularSpeed, angle = (s16)((s16)mLidAngle.z + speed);
    mLidAngle.z = angle;
    mAngularSpeed = (s16)(speed - 100);
    if (angle > -5600)
        return;
    mState = STATE_EFFECT_YOKO;
    mLidAngle.z = -5600;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_stack_c::transM(100.0f, 75.0f, 0.0f);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.z);
    mDoMtx_stack_c::transM(-100.0f, -75.0f, 0.0f);
    gabi::Local<Mtx34> transform;
    PSMTXCopy(mDoMtx_stack_c::get(), transform.get());
    s16 yaw = shape_angle.y;
    mSmokeAngle.x = 0;
    mSmokeAngle.y = yaw;
    mSmokeAngle.z = 0;
    mSmokeAlpha = 180;
    gabi::Local<cXyz> sum, point;
    cXyz_pl(&mLidOffset, sum.get(), &l_lidEffectOffset);
    point->copy(*sum);
    PSMTXMultVec(transform.get(), point.get(), point.get());
    cXyz_pl(point.get(), sum.get(), &current.pos);
    mSmokePos.copy(*sum);
    KanokeEmitter* emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
    if (!emitter) {
        u8 alpha = (u8)gabi::ftoi((f32)mSmokeAlpha);
        kanokeSmokeBegin(this, 0xa181, alpha);
        emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
    }
    kanokeFlagSmoke(emitter);
    mTimer = 60;
}
/* 02368860 */
void daObjKanoke_c::executeEffectYoko() {
    s16 timer = (s16)((s16)mTimer - 1);
    mTimer = timer;
    if (!timer) {
        mSmoke.end();
        mState = STATE_WAIT;
        return;
    }
    if (timer > 50)
        return;
    f32 alpha = (f32)mSmokeAlpha - 3.6f;
    if (alpha < 0.0f)
        alpha = 0.0f;
    KanokeEmitter* emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
    mSmokeAlpha = alpha;
    if (emitter)
        emitter->mGlobalAlpha = (u8)gabi::ftoi(alpha);
}
/* 02368918 */
void daObjKanoke_c::executeYureTate() {
    u16 phase = (u16)((s16)mTimer + 0x2000);
    s16 speed = (s16)((s16)mShakeSpeed - 32);
    mTimer = (s16)phase;
    mShakeSpeed = speed;
    f32 sine = cM_ssin(phase);
    s16 angle = (s16)gabi::ftoi(sine * (f32)speed);
    mLidAngle.y = angle;
    s16 remaining = mShakeSpeed;
    mPivot.x = angle < 0 ? 100.0f : -100.0f;
    if (remaining <= 0) {
        kanokeSound(this, 0x695a);
        mState = STATE_OPEN_TATE;
        kanokeReleaseLid(this);
        mLidAngle.x = 0;
        mLidAngle.y = 0;
        mPivot.x = 0;
    }
}
/* 02368A44 */
void daObjKanoke_c::executeOpenTate() {
    s16 speed = mAngularSpeed, angle = (s16)((s16)mLidAngle.x + speed);
    mAngularSpeed = (s16)(speed + 100);
    mLidAngle.x = angle;
    if (angle >= 0x4000) {
        dVibration_c* vibration = dComIfGp_getVibration();
        gabi::Local<cXyz> up;
        up->z = 0;
        up->x = 0;
        up->y = 1;
        vibration->StartShock(4, -33, up.get());
        s16 yaw = shape_angle.y;
        f32 y = mLidMtx.m[1][3], x = mLidMtx.m[0][3];
        mSmokeAngle.y = yaw;
        u8 hidden = mHidden;
        mSmokeAngle.z = 0;
        mSmokePos.y = y;
        f32 z = mLidMtx.m[2][3];
        mSmokeAngle.x = 0;
        mSmokePos.z = z;
        mLidAngle.x = 0x4000;
        mSmokePos.x = x;
        mState = STATE_EFFECT_TATE;
        mHidden = hidden | 2;
        u32 effect = gabi::ea(
            dComIfGp_getParticle()->set(0, 0x817f, &mSmokePos, &mSmokeAngle, nullptr, 255, nullptr,
                                        -1, &tevStr.mColorK0, &tevStr.mColorK0, nullptr));
        KanokeEmitter* emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
        mEffectHandles[0] = effect;
        mSmokeAlpha = 180;
        if (!emitter) {
            kanokeSmokeBegin(this, 0xa180, 180);
            emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
        }
        kanokeFlagSmoke(emitter);
        mTimer = 60;
        return;
    }
    mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x);
    mDoMtx_stack_c::transM(0.0f, 110.0f, 200.0f);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)mLidAngle.x);
    mDoMtx_stack_c::transM(0.0f, -110.0f, -200.0f);
    gabi::Local<cXyz> sum, start, end;
    for (int i = 0; i < 3; i++) {
        KanokeCapsule* capsule = &mLidCps[i];
        cXyz_pl(&l_lidPoints.points[i * 2], sum.get(), &mLidOffset);
        start->copy(*sum);
        cXyz_pl(&l_lidPoints.points[i * 2 + 1], sum.get(), &mLidOffset);
        end->copy(*sum);
        PSMTXMultVec(mDoMtx_stack_c::get(), start.get(), start.get());
        PSMTXMultVec(mDoMtx_stack_c::get(), end.get(), end.get());
        PSVECAdd(start.get(), &current.pos, start.get());
        PSVECAdd(end.get(), &current.pos, end.get());
        kanokeSetCapsuleEnds(&capsule->mCps, start.get(), end.get());
        cCcS_Set(dComIfG_Ccsp(), capsule);
    }
}
/* 02368D34 */
void daObjKanoke_c::executeEffectTate() {
    s16 timer = (s16)((s16)mTimer - 1);
    mTimer = timer;
    if (!timer) {
        mSmoke.end();
        mState = STATE_WAIT;
        return;
    }
    if (!mSmoke.mpEmitter.get() || timer > 50)
        return;
    f32 alpha = (f32)mSmokeAlpha - 3.6f;
    KanokeEmitter* emitter = reinterpret_cast<KanokeEmitter*>(mSmoke.mpEmitter.get());
    if (alpha < 0.0f)
        alpha = 0.0f;
    mSmokeAlpha = alpha;
    emitter->mGlobalAlpha = (u8)gabi::ftoi(alpha);
}
/* 02369048 */
void daObjKanoke_c::executeWait() {}

// HD: the harness exposes the original non-const member entry; PPC shift counts wrap to six bits.
/* 0236904C */
u32 daObjKanoke_c::getPrm(u32 width, u32 shift) {
    u32 value = mParameters, n = width & 63, s = shift & 63;
    u32 mask = (n < 32 ? (1u << n) : 0u) - 1u, shifted = s < 32 ? value >> s : 0u;
    return shifted & mask;
}

// HD: compiler-generated and template code of this translation unit.

// HD: checked GHS constructors can allocate on a null this argument and preserve vtable store order.
/* 02367808 */
daObjKanoke_c* daObjKanoke_construct(daObjKanoke_c* a) {
    if (!a)
        a = static_cast<daObjKanoke_c*>(operator_new(sizeof(daObjKanoke_c)));
    if (!a)
        return a;
    kanokeActorConstruct(a);
    a->__vtbl = VT_ACTOR;
    kanokeStatusConstruct(&a->mStts);
    kanokeGlobalStatusConstruct(&a->mStts.__vtbl_gstts);
    a->mStts.__vtbl = VT_STATUS;
    a->mStts.__vtbl_gstts = VT_GLOBAL_STATUS;
    kanokeHitInfoConstruct(&a->mBodyCps);
    a->mBodyCps.mShape.__vtbl = VT_SHAPE;
    a->mBodyCps.mShape.boundsVtable = VT_BOUNDS;
    kanokeCapsuleGeometryConstruct(&a->mBodyCps.mCps);
    a->mBodyCps.__vtbl_hitinf = VT_CAPSULE_HIT;
    a->mBodyCps.mShape.__vtbl = VT_CAPSULE_SHAPE;
    a->mBodyCps.mCps.__vtbl = VT_CAPSULE;
    kanokeConstructArray(&a->mLidCps, 3, 0x138,
                         gabi::guest_fn(reinterpret_cast<const void*>(KanokeCapsule_construct)));
    kanokeSmokeConstruct(&a->mSmoke, 1);
    a->mType = a->getPrmType();
    a->mSearchRange = a->getPrmSearch();
    a->mSwNo = a->getPrmSwNo();
    u8 second = a->getPrmSwNo2(), type = a->mType;
    a->mSwNo2 = second;
    if (type == TYPE_YOKO) {
        a->mLidOffset.z = 0;
        a->mPivot.z = 0;
        a->mLidAngle.z = 0;
        a->mPivot.x = 0;
        a->mAngularSpeed = 0;
        a->mPivot.y = 0;
        a->mLidAngle.y = 0;
        a->mLidOffset.x = 0;
        a->mLidAngle.x = 0;
        a->mLidOffset.y = 75;
    } else {
        a->mPivot.x = 0;
        a->mLidOffset.z = 0;
        f32 y = (f32)a->home.pos.y + 200.0f;
        a->shape_angle.x = 0x4000;
        a->mPivot.y = 35;
        a->mPivot.z = 200;
        a->current.pos.y = y;
        a->mLidAngle.x = 0;
        a->mLidOffset.y = 75;
        a->mLidAngle.y = 0;
        a->mLidAngle.z = 0;
        a->mLidOffset.x = 0;
        a->mAngularSpeed = 0;
    }
    a->mEffectHandles[0] = 0;
    a->mEffectHandles[1] = 0;
    a->mState = STATE_NORMAL;
    a->mSmoke.m11 = 0;
    a->mLightTimer = 0;
    a->mHidden = 0;
    a->setMtxHontai();
    PSMTXCopy(mDoMtx_stack_c::get(), &a->mBodyMtx);
    a->setMtxHuta(&a->current.pos);
    PSMTXCopy(mDoMtx_stack_c::get(), &a->mLidMtx);
    return a;
}

/* 02368DF0 */
void __sinit_d_a_obj_kanoke_cpp() {
    l_math.mZero[1] = 0;
    l_math.mZero[3] = 0;
    l_math.mZero[0] = 0;
    l_math.mZero[2] = 0;
    kanokeRegisterGlobal(&l_descriptors.entries[0]);
    l_math.mMinAngle = -3.1415927410125732f;
    l_math.mMaxAngle = 3.1415927410125732f;
    kanokeConstructSinTable(&l_math.mSinTable);
    kanokeRegisterGlobal(&l_descriptors.entries[1]);
    kanokeConstructAtanTable(&l_math.mAtanTable);
    kanokeRegisterGlobal(&l_descriptors.entries[2]);
    l_lidPoints.points[0].x = 50;
    l_lidPoints.points[0].y = 0;
    l_lidPoints.points[0].z = -175;
    l_lidPoints.points[1].x = 50;
    l_lidEffectOffset.x = 100;
    l_lidEffectOffset.y = 0;
    l_lidPoints.points[1].y = 0;
    l_lidPoints.points[1].z = 175;
    l_lidPoints.points[2].x = 0;
    l_lidPoints.points[2].y = 0;
    l_lidPoints.points[2].z = -175;
    l_lidPoints.points[3].x = 0;
    l_lidPoints.points[3].y = 0;
    l_lidPoints.points[3].z = 175;
    l_lidPoints.points[4].x = -50;
    l_lidPoints.points[4].y = 0;
    l_lidPoints.points[4].z = -175;
    l_lidPoints.points[5].x = -50;
    l_lidPoints.points[5].y = 0;
    l_lidPoints.points[5].z = 175;
    l_lidEffectOffset.z = 0;
}
/* 02368F10 */
void KanokeDescriptor_destruct(void* self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 02368F24 */
void* KanokeCapsule_construct(void* self) {
    if (!self)
        self = operator_new(sizeof(KanokeCapsule));
    if (!self)
        return self;
    KanokeCapsule* capsule = static_cast<KanokeCapsule*>(self);
    kanokeHitInfoConstruct(self);
    capsule->mShape.__vtbl = VT_SHAPE;
    capsule->mShape.boundsVtable = VT_BOUNDS;
    kanokeCapsuleGeometryConstruct(&capsule->mCps);
    capsule->__vtbl_hitinf = VT_CAPSULE_HIT;
    capsule->mCps.__vtbl = VT_CAPSULE;
    capsule->mShape.__vtbl = VT_CAPSULE_SHAPE;
    return self;
}
/* 02368FB0 */
BOOL daObjKanokeIsDelete(daObjKanoke_c* a) {
    return 1;
}
/* 02368FB8 */
void KanokeDescriptor_dummy(void* self) {}
/* 02368FBC */
void daObjKanoke_destruct(daObjKanoke_c* a, s32 flags) {
    if (!a)
        return;
    kanokeDestructArray(&a->mLidCps, 3, 0x138, KANOKE_CAPSULE_DESTRUCTOR, 0, 0);
    kanokeCapsuleDestruct(&a->mBodyCps, 2);
    kanokeStatusDestruct(&a->mStts, 2);
    kanokeActorDestruct(a, 0);
    if (flags & 1)
        operator_delete(a);
}
/* WWHD glue
str 1002BC7C "Mkanoke" in 0236743C
data l_modeProc 101CA68C
data l_math 1046A3B4
data l_descriptors 101CA784
data l_lidEffectOffset 1046A3D0
data l_lidPoints 1046A3DC
data l_bodyCpsSource 101CA6EC
data l_lidCpsSource 101CA738
ext 025D4ED0 void kanokeActorConstruct(fopAc_ac_c*)
ext 0200BD2C void kanokeStatusConstruct(dCcD_Stts*)
ext 02515DA0 void kanokeGlobalStatusConstruct(void*)
ext 02515FB8 void kanokeHitInfoConstruct(void*)
ext 02018150 void kanokeCapsuleGeometryConstruct(void*)
ext 028EFFD0 void kanokeConstructArray(void*, s32, s32, u32)
ext 025A5B18 void kanokeSmokeConstruct(dPa_smokeEcallBack*, u8)
ext 025164C0 void kanokeSetCapsule(void*, const void*)
ext 02018808 void kanokeSetCapsuleEnds(void*, const cXyz*, const cXyz*)
ext 0252A038 s32 kanokeCheckLight(void*, const cXyz*)
ext 028E8DD0 f64 kanokeSquareMagnitudeFPR(const cXyz*)
ext 028F4384 f64 kanokeSqrtFPR(f64)
ext 028F026C void kanokeRegisterGlobal(void*)
ext 028ED6F8 void kanokeConstructSinTable(void*)
ext 028EAB2C void kanokeConstructAtanTable(void*)
ext 028F0164 void kanokeDestructArray(void*, s32, s32, u32, s32, s32)
ext 02515980 void kanokeCapsuleDestruct(void*, s32)
ext 02515860 void kanokeStatusDestruct(dCcD_Stts*, s32)
ext 025D50BC void kanokeActorDestruct(fopAc_ac_c*, s32)
ext 026066C4 void* kanokeGetResource(void*, const void*, s32)
str 1002BCB8 "Mkanoke" in 02367E94
str 1002BCC0 "Mkanoke" in 02367F4C
*/
