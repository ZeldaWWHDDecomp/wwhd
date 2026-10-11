/* WWHD hammer switch; */
#include "d/actor/d_a_obj_swhammer.h"
using namespace daObjSwhammer;
enum SwhammerProfileHD { fpcNm_PLAYER_e = 168 };
enum SwhammerCutHD {
    CUT_TYPE_HAMMER_SIDESWING = 17,
    CUT_TYPE_HAMMER_FRONTSWING = 18,
    CUT_TYPE_JUMPCUT_HAMMER = 19
};
enum SwhammerParticleHD {
    ID_IT_SN_HAMSW_STAR00 = 0x81B7,
    ID_IT_SN_HAMSW_SYOUGEKI00 = 0x81B8,
    ID_AK_JT_ELEMENTSMOKE01 = 0x2027
};
enum SwhammerModeHD { Mode_Upper, Mode_UpperToLower, Mode_Lower, Mode_LowerToUpper };
enum SwhammerCrushHD { Crush_Idle, Crush_Start, Crush_Wait };
enum SwhammerDamageHD { Damage_None, Damage_Crush, Damage_Swing };
enum SwhammerSoundHD {
    JA_SE_OBJ_HAMMER_SW_ON = 0x69B2,
    JA_SE_OBJ_HAMMER_SW_OFF = 0x69B7,
    JA_SE_OBJ_HAMMER_SW_SWING = 0x69B8
};

const gc_tu_data gc_tu = {.safeString = 0x1002F9A4};
static const char *M_arcname = "MhmrSW";
using SwhammerS32HD = be<s32>;
using SwhammerS16HD = be<s16>;
using SwhammerU32HD = be<u32>;
static SwhammerS32HD M_damage;
static SwhammerS16HD M_damage_dir;
static Mtx34 M_tmp_mtx;
static SwhammerCylinderSourceHD M_cyl_src_co;
static SwhammerCylinderSourceHD M_cyl_src_tg;
static SwhammerHeaderHD l_header;
static cXyz particle_scale;
static cXyz vec;
static SwhammerU32HD l_scaleGuard;
static SwhammerU32HD l_translationGuard;
struct SwhammerModeProcHD {
    be<s16> mThisDelta;
    be<s16> mVirtualIndex;
    be<u32> mFunction;
};
static SwhammerModeProcHD mode_proc[] = {
    {0, -1, 0x023938B4}, {0, -1, 0x023939AC}, {0, -1, 0x02393A70}, {0, -1, 0x02393AF0}};
static BOOL jnodeCB(J3DNode *, s32);

// HD: the deletion wrapper forwards the entire native return register.
s32 Swhammer_deleteResult(Act_c *);
void Swhammer_setCullSphere(fopAc_ac_c *, f32, f32, f32, f32);
void Swhammer_initHeaderByte(be<u8> *);
void Swhammer_initHeaderFlag(be<u8> *);
void Swhammer_hitSound(cXyz *, s32, dCcD_Cyl *, s32);
void Swhammer_hitEffect(fopAc_ac_c *, dCcD_Cyl *);
s16 Swhammer_atan2s(f32, f32);
void Swhammer_transM(f32, f32, f32);
void Swhammer_quatM(Quaternion *);
void Swhammer_quatRotBaseY2(Quaternion *, cXyz *);
SwhammerEmitterHD *Swhammer_spawnParticle(dPa_control_c *, s32, s32, cXyz *, u32, u32, s32,
                                          dPa_smokeEcallBack *, s32, u32, u32, cXyz *);
void Swhammer_destroyStatus(dCcD_Stts *, s32);
void Swhammer_destroyCylinder(dCcD_Cyl *, s32);
void Swhammer_destroyActor(fopAc_ac_c *, s32);

/* 02393CB4 */
static u32 PrmAbstract(fopAc_ac_c *a, s32 width, s32 shift) {
    u32 w = (u32)width & 63, s = (u32)shift & 63;
    return (s < 32 ? (u32)a->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
static bool is_switch(Act_c *a) {
    s32 sw = PrmAbstract(a, 8, 8);
    return fopAcM_isSwitch(a, sw) != 0;
}
static void on_switch(Act_c *actor) {
    s32 no = PrmAbstract(actor, 8, 8);
    dSv_info_c *save = dComIfGs_info();
    s32 room = actor->home.roomNo;
    save->onSwitch(no, room);
}
static void off_switch2(Act_c *actor) {
    s32 no = PrmAbstract(actor, 8, 0);
    dSv_info_c *save = dComIfGs_info();
    s32 room = actor->home.roomNo;
    save->offSwitch(no, room);
}
/* 02392F08 */
void Act_c::mode_upper_init() {
    mCylTg.SetH(112.5f);
    mCylCo.SetH(112.5f);
    u32 flags = (u32)attention_info.flags;
    mMode = Mode_Upper;
    attention_info.flags = flags | fopAc_Attn_LOCKON_MISC_e;
    mTargetHFrac = 1;
    m5FC = 0;
}
/* 02392E94 */
void Act_c::mode_lower_init() {
    mCylTg.SetH(l_header.mLowerHeight);
    mCylCo.SetH(l_header.mLowerHeight);
    u32 flags = (u32)attention_info.flags;
    mMode = Mode_Lower;
    mTargetHFrac = 0;
    attention_info.flags = flags & ~u32(fopAc_Attn_LOCKON_MISC_e);
    m5FC = 0;
}
/* 02393858 */
void Act_c::mode_u_l_init() {
    fopAcM_seStart(this, JA_SE_OBJ_HAMMER_SW_ON, 0);
    mMode = Mode_UpperToLower;
    mTargetHFrac = 0;
}
/* 02393A58 */
void Act_c::mode_l_u_init() {
    mMode = Mode_LowerToUpper;
    mTargetHFrac = 1;
}
/* 023938B4 */
void Act_c::mode_upper() {
    if ((s32)M_damage == Damage_Crush) {
        m5FC = 1;
        mode_u_l_init();
    } else if (is_switch(this))
        mode_u_l_init();
    else if ((s32)M_damage == Damage_Swing) {
        vib_start((s16)M_damage_dir, 1);
        fopAcM_seStart(this, JA_SE_OBJ_HAMMER_SW_SWING, 0);
    }
}
/* 02393A70 */
void Act_c::mode_lower() {
    if (!is_switch(this)) {
        fopAcM_seStart(this, JA_SE_OBJ_HAMMER_SW_OFF, 0);
        mode_l_u_init();
    }
}
/* 023939AC */
void Act_c::mode_u_l() {
    if (!(mCurHFrac > 0)) {
        on_switch(this);
        if (m5FC)
            off_switch2(this);
        mode_lower_init();
    }
}
/* 02393AF0 */
void Act_c::mode_l_u() {
    if (!(mCurHFrac < 1))
        mode_upper_init();
}
/* 02393824 */
void Act_c::vib_start(s16 dir, f32 mag) {
    mAngleSpeedZ = cM_scos(dir) * mag;
    mAngleSpeedX = cM_ssin(dir) * mag;
}
/* 023934E4 */
void Act_c::vib_proc() {
    f32 z = mAngleZ, x = mAngleX, vz = mAngleSpeedZ, vx = mAngleSpeedX;
    // HD: retain native fused arithmetic and evaluation order.
    f32 az = gabi::fnmsubs(vz, 0.18f, -(z * 0.99f)), ax = gabi::fnmsubs(vx, 0.18f, -(x * 0.99f));
    vz += az;
    vx += ax;
    mAngleSpeedZ = vz;
    mAngleSpeedX = vx;
    mAngleZ = z + vz;
    mAngleX = x + vx;
}
/* 023932A0 */
void Act_c::crush_start() {
    mCrushState = Crush_Start;
}
/* 02393540 */
void Act_c::crush_proc() {
    if (mCrushState == Crush_Start) {
        mCrushState = Crush_Wait;
        mScaleY = 0.2f;
        mCrushTimer = 18;
        return;
    }
    if (mCrushState == Crush_Wait) {
        s16 timer = (s16)((s16)mCrushTimer - 1);
        mCrushTimer = timer;
        if (timer <= 0) {
            mCrushState = Crush_Idle;
            mScaleYSpeed = 0;
        }
        return;
    }
    f32 scale = mScaleY, velocity = mScaleYSpeed;
    f32 accel = gabi::fnmsubs(velocity, 0.45f, -((scale - 1.0f) * 0.8f));
    velocity += accel;
    mScaleYSpeed = velocity;
    mScaleY = scale + velocity;
}
/* 02393488 */
void Act_c::calc_top_pos() {
    f32 frac = mCurHFrac, diff = frac - (f32)mTargetHFrac;
    f32 velocity = gabi::fnmsubs(diff, 0.8f, mVSpeed);
    velocity = gabi::fnmsubs(velocity, 0.55f, velocity);
    frac += velocity;
    frac = (frac - 1.0f) >= 0 ? 1.0f : frac;
    mVSpeed = velocity;
    mCurHFrac = frac;
    mTopPos = (1 - frac) * -60.0f;
}
/* 02393798 */
s32 Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}
/* 023937F4 */
s32 Act_c::Delete() {
    mSmokeCb.remove();
    return TRUE;
}
/* 02393B08 */
static s32 Mthd_Create(Act_c *a) {
    return a->_create();
}
/* 02393B0C */
static s32 Mthd_Delete(Act_c *a) {
    return Swhammer_deleteResult(a);
}
/* 02393B10 */
static s32 Mthd_Execute(Act_c *a) {
    return a->MoveBGExecute();
}
/* 02393B14 */
static s32 Mthd_Draw(Act_c *a) {
    return a->Draw_v();
}
/* 02393B24 */
static s32 Mthd_IsDelete(Act_c *a) {
    return a->IsDelete_v();
}
/* 02392880 */
static Act_c *Swhammer_construct(Act_c *a) {
    if (!a) {
        a = (Act_c *)operator_new(0x718);
        if (!a)
            return a;
    }
    dBgS_MoveBgActor::ct(a);
    a->__vtbl = 0x1002FAD0;
    dCcD_Cyl_ct(&a->mCylCo, 0x1002F9BC);
    dCcD_Stts_ct(&a->mSttsCo);
    dCcD_Cyl_ct(&a->mCylTg, 0x1002F9BC);
    dCcD_Stts_ct(&a->mSttsTg);
    __ct(&a->mSmokeCb, 1);
    a->mSmokeCb.mColor.g = 160;
    a->mSmokeCb.mColor.r = 160;
    a->mSmokeCb.mColor.a = 255;
    a->mSmokeCb.mColor.b = 128;
    return a;
}
/* 023929AC */
s32 Act_c::_create() {
    if (!((u32)actor_condition & 8)) {
        if (this != nullptr)
            Swhammer_construct(this);
        actor_condition = (u32)actor_condition | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase == 4) {
        phase = MoveBGCreate(M_arcname, 7, 0, 0xA80);
        if (phase != 4 && phase != 5)
            JUT_ASSERT_fail("d_a_obj_swhammer.cpp", 0x1E8,
                            "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}
/* 02392A70 */
bool Act_c::_delete() {
    s32 result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname);
    return result != 0;
}
/* 02392CB0 */
s32 Act_c::CreateHeap() {
    auto *data = (J3DModelData *)dComIfG_getObjectRes(M_arcname, 4);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_swhammer.cpp", 0x18B, "modelData != 0");
    J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    if (model) {
        data->getJointNodePointer(1)->setCallBack(jnodeCB);
        mpModel->setUserArea((uintptr_t)this);
        model = mpModel;
    }
    return model != nullptr;
}
/* 02392D80 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 02392E44 */
void Act_c::init_mtx() {
    f32 x = scale.x, y = scale.y;
    J3DModel *model = mpModel;
    f32 z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
    PSMTXCopy(mDoMtx_stack_c::get(), (&M_tmp_mtx));
}
/* 02392F8C */
s32 Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    init_mtx();
    Swhammer_setCullSphere(this, 0.0f, 30.0f, 0.0f, 120.0f);
    mSttsCo.Init(255, 255, this);
    mCylCo.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_co));
    mCylCo.SetStts(&mSttsCo);
    mSttsTg.Init(255, 255, this);
    mCylTg.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_tg));
    mScaleYSpeed = 0;
    mVSpeed = 0;
    mCrushTimer = 0;
    u32 flags = (u32)mCylTg.mGObjTg.mSPrm;
    mAngleSpeedZ = 0;
    mCylTg.SetStts(&mSttsTg);
    mTargetHFrac = 0;
    mScaleY = 1;
    mAngleX = 0;
    mAngleZ = 0;
    mCylTg.mGObjTg.mSPrm = flags | 4;
    mAngleSpeedX = 0;
    mCrushState = Crush_Idle;
    if (is_switch(this)) {
        mCurHFrac = 0;
        mTopPos = -60;
        mode_lower_init();
    } else {
        mCurHFrac = 1;
        mTopPos = 0;
        mode_upper_init();
    }
    f32 y = (f32)current.pos.y + (f32)mTopPos + 120;
    attention_info.distances[fopAc_Attn_TYPE_MISC_e] = 45;
    attention_info.position.y = y;
    eyePos.y = y;
    return TRUE;
}
// HD: native initialization mutates the cylinder radius/height fields.
/* 02393B34 */
static void __sinit_d_a_obj_swhammer_cpp() {
    for (u32 i = 0; i < 4; i++)
        l_header.mZero[i] = 0;
    __register_global_object(0x101CCE24);
    l_header.mPiMin = -3.1415927f;
    l_header.mPiMax = 3.1415927f;
    Swhammer_initHeaderByte(&l_header.mAngleByte);
    __register_global_object(0x101CCE30);
    Swhammer_initHeaderFlag(&l_header.mAngleFlag);
    __register_global_object(0x101CCE3C);
    M_cyl_src_co.mRadius = 100;
    l_header.mLowerHeight = 52.5f;
    M_cyl_src_tg.mRadius = 50;
    M_cyl_src_tg.mHeight = 112.5f;
    M_cyl_src_co.mHeight = 112.5f;
}
/* 02393C10 */
static void Swhammer_trivialDestructor(void *a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}
/* 02393178 */
void Act_c::set_damage() {
    // HD: the cut type comes from player slot zero, independent of the Link pointer.
    SwhammerPlayerHD *player = reinterpret_cast<SwhammerPlayerHD *>(dComIfGp_getPlayer(0));
    u8 cut = player->mCutType;
    M_damage = Damage_None;
    M_damage_dir = 0;
    if (mCylTg.ChkTgHit()) {
        void *hit = mCylTg.GetTgHitObj();
        fopAc_ac_c *actor = mCylTg.GetTgHitAc();
        if ((static_cast<dCcD_GObjInf *>(hit)->mAtType & 0x10000) && actor &&
            actor->mProfName == fpcNm_PLAYER_e) {
            if (cut == CUT_TYPE_HAMMER_FRONTSWING || cut == CUT_TYPE_JUMPCUT_HAMMER)
                M_damage = Damage_Crush;
            else if (cut == CUT_TYPE_HAMMER_SIDESWING) {
                f32 x = (f32)mCylTg.mGObjTg.mRVec.x, z = (f32)mCylTg.mGObjTg.mRVec.z;
                s16 dir = Swhammer_atan2s(x, z);
                M_damage_dir = dir;
                M_damage = Damage_Swing;
            }
        }
        Swhammer_hitSound(&eyePos, (s32)current.roomNo, &mCylTg, 17);
        Swhammer_hitEffect(this, &mCylTg);
        mCylTg.ClrTgHit();
    }
    mSttsTg.Move();
}
/* 023935E4 */
s32 Act_c::Execute(Mtx34 **matrix) {
    set_damage();
    if ((s32)M_damage == Damage_Crush) {
        crush_start();
        eff_crush();
    }
    (this->*reinterpret_cast<const gabi::ptmf<void (Act_c::*)()> &>(mode_proc[(u32)mMode]))();
    calc_top_pos();
    vib_proc();
    crush_proc();
    set_mtx();
    *reinterpret_cast<gptr<Mtx34> *>(matrix) = (&M_tmp_mtx);
    mCylTg.SetC(&current.pos);
    mCylCo.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCylTg);
    dComIfG_Ccsp_Set(&mCylCo);
    f32 y = (f32)current.pos.y + (f32)mTopPos + 120;
    eyePos.y = y;
    attention_info.position.y = y;
    return TRUE;
}
/* 02392AC0 */
static BOOL jnodeCB(J3DNode *node, s32 timing) {
    if (timing == 0) {
        J3DModel *model = j3dSys.getModel();
        Act_c *actor = static_cast<Act_c *>(model->getUserArea());
        u16 index = reinterpret_cast<J3DJoint *>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(index), mDoMtx_stack_c::get());
        f32 scale = (f32)actor->mScaleY;
        Swhammer_transM(0.0f, 20.0f * (1.0f - scale), 0.0f);
        scale = (f32)actor->mScaleY;
        mDoMtx_stack_c::scaleM(1, scale, 1);
        Swhammer_transM(0.0f, (f32)actor->mTopPos, 0.0f);
        gabi::Local<cXyz> axis;
        f32 x = (f32)actor->mAngleX, z = (f32)actor->mAngleZ;
        axis->y = 1;
        axis->x = x;
        axis->z = z;
        gabi::Local<Quaternion> quat;
        Swhammer_quatRotBaseY2((Quaternion *)quat, (cXyz *)axis);
        Swhammer_transM(0.0f, -20.0f, 0.0f);
        Swhammer_quatM((Quaternion *)quat);
        Swhammer_transM(0.0f, 20.0f, 0.0f);
        mtx_copy(model->getAnmMtx(index), mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}
/* 023932AC */
void Act_c::eff_crush() {
    if (mMode == Mode_Upper) {
        dComIfGp_particle_set(ID_IT_SN_HAMSW_STAR00, &current.pos);
        dComIfGp_particle_set(ID_IT_SN_HAMSW_SYOUGEKI00, &current.pos);
    }
    // HD: retain guard placement and component-store order.
    if (!(u32)l_scaleGuard) {
        particle_scale.x = 1.5f;
        particle_scale.z = 1;
        particle_scale.y = 1.5f;
        l_scaleGuard = 1;
    }
    dPa_control_c *particles = dComIfGp_getParticle();
    SwhammerEmitterHD *emitter =
        Swhammer_spawnParticle(particles, 2, ID_AK_JT_ELEMENTSMOKE01, &current.pos, (u32)0, (u32)0, 200,
                               &mSmokeCb, -1, (u32)0, (u32)0, &particle_scale);
    if (emitter) {
        emitter->mMaxFrame = 1;
        emitter->mLifeTime = 45;
        emitter->mRate = 30;
        emitter->mInitialVelOmni = 0;
        emitter->mInitialVelAxis = 38;
        if (!(u32)l_translationGuard) {
            l_translationGuard = 1;
            vec.z = 0;
            vec.y = 15;
            vec.x = 0;
        }
        emitter->mEmitterTranslation.x = vec.x;
        emitter->mEmitterTranslation.y = vec.y;
        emitter->mEmitterTranslation.z = vec.z;
    }
}
/* 02393C30 */
static void Swhammer_actorDestructor(Act_c *a, s32 flags) {
    if (a) {
        Swhammer_destroyStatus(&a->mSttsTg, 2);
        Swhammer_destroyCylinder(&a->mCylTg, 2);
        Swhammer_destroyStatus(&a->mSttsCo, 2);
        Swhammer_destroyCylinder(&a->mCylCo, 2);
        Swhammer_destroyActor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}
/* 02393C24 */
static BOOL Swhammer_baseIsDelete(void *a) {
    return TRUE;
}
/* 02393C2C */
static void Swhammer_baseDestructor(void *a) {}

/* WWHD glue
ext 02392A70 s32 Swhammer_deleteResult(Act_c*)
str 1002FAC8 "MhmrSW"
data M_damage 101CCE48
data M_damage_dir 101CCE4C
data M_tmp_mtx 1046BFD8
data M_cyl_src_co 101CCEA0
data M_cyl_src_tg 101CCEE4
data l_header 1046BF98
data particle_scale 1046BFB8
data vec 1046BFC4
data l_scaleGuard 1046BFD0
data l_translationGuard 1046BFD4
data mode_proc 1002FA90
str 1002F9CC in 023929AC # "d_a_obj_swhammer.cpp"
str 1002F9E4 # "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1002FA34 in 02392CB0 # "d_a_obj_swhammer.cpp"
str 1002FA4C # "modelData != 0"
ext 025D6768 void Swhammer_setCullSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 028ED6F8 void Swhammer_initHeaderByte(be<u8>*)
ext 028EAB2C void Swhammer_initHeaderFlag(be<u8>*)
ext 023129C4 void Swhammer_hitSound(cXyz*, s32, dCcD_Cyl*, s32)
ext 02312E54 void Swhammer_hitEffect(fopAc_ac_c*, dCcD_Cyl*)
ext 020195B0 s16 Swhammer_atan2s(f32, f32)
ext 025F24E0 void Swhammer_transM(f32, f32, f32)
ext 025F25CC void Swhammer_quatM(Quaternion*)
ext 02312548 void Swhammer_quatRotBaseY2(Quaternion*, cXyz*)
ext 025A847C SwhammerEmitterHD* Swhammer_spawnParticle(dPa_control_c*, s32, s32, cXyz*, u32, u32, s32, dPa_smokeEcallBack*, s32, u32, u32, cXyz*)
ext 02515860 void Swhammer_destroyStatus(dCcD_Stts*, s32)
ext 02515A70 void Swhammer_destroyCylinder(dCcD_Cyl*, s32)
ext 025D50BC void Swhammer_destroyActor(fopAc_ac_c*, s32)
*/
