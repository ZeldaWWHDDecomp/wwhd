/* WWHD Dragon Roost water platform. */
#include "d/actor/d_a_obj_gryw00.h"
const gc_tu_data gc_tu  =  {.safeString  =  0x10029940};
static const char* l_arcname  =  "Gryw00";
enum { dRes_INDEX_GRYW00_BDL_GRYW00_e=9, dRes_INDEX_GRYW00_BTK_GRYW00_e=12,
       dRes_INDEX_GRYW00_BCK_GRYW00_e = 6, dRes_INDEX_GRYW00_DZB_EWATER_e = 15 };
enum { JA_SE_OBJ_RDUN_MAE_GEYSER=0x704C, JA_SE_OBJ_RDUN_MAE_W_OUT=0x704B, JA_SE_OBJ_RDUN_MAE_W_UP=0x704D };
namespace dPa_name { enum { ID_AK_SN_RYUDANMAESPLASH00=0x8295, ID_AK_SN_RYUDANMAESTEAM00=0x8297,
    ID_AK_SN_RYUDANMAESPLASH01 = 0x8296, ID_AK_SN_RYUDANMAEHAMON00 = 0x8298 };
    }
// HD: member pointers contain these marked methods' guest addresses.
enum ActionTarget { SWITCH_WAIT=0x0234D024, SPREAD_WATER_FACE=0x0234D0F0,
    WATER_LEVEL_MOVE_WAIT = 0x0234D14C, ANIME_LOOP_START_WAIT = 0x0234D1EC, HIGH_WATER_LEVEL = 0x0234D260 };

// HD: preserve inline construction of the animation's SDK state.
daObjGryw00_c::daObjGryw00_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x10029A50;
    mDoExt_btkAnm::ct(&mBtk);
    Gryw_frameInit(&mBck.mFrameCtrl, 0);
    mBckState.mVtable = 0x1016E54C;
    Gryw_objectConstruct(mBckState.mObject);
    mBckState.field_0x88 = 0;
    mBckState.field_0x84 = 0;
    mBckState.mVtable = 0x10029958;
    mBckState.field_0x7C = 0;
    mBckState.mObjectVtable = 0x1016D820;
    mBckState.field_0x80 = 0;
    mBckState.field_0x58 = 0;
}
static void changeAction(daObjGryw00_c* p, u32 target) {
    p->modeFunc.d = 0;
    p->modeFunc.i = -1;
    p->modeFunc.f = target;
}
static void* objectResource(s32 index) {
    return dComIfG_getObjectRes(l_arcname, index);
}
static void soundCurrent(daObjGryw00_c* p, u32 sound) {
    s8 room = p->current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(sound, &p->current.pos, 0, reverb);
}
/* 0234D390 */
static u32 PrmAbstract(daObjGryw00_c* p, u32 width, u32 shift) {
    u32 parameters = p->mParameters;
    u32 mask = (width&32) ? 0 : (1U<<(width&31));
    u32 value = (shift&32) ? 0 : (parameters>>(shift&31));
    return value&(mask-1);
}
/* 0234C420 */
s32 daObjGryw00_c::Mthd_Create() {
    fopAcM_ct(this, daObjGryw00_c);
    s32 result = dComIfG_resLoad(&mPhs, l_arcname);
    if (result==4) {
        result = MoveBGCreate(l_arcname, dRes_INDEX_GRYW00_DZB_EWATER_e, dBgS_MoveBGProc_Trans, 0x9A0);
        if (result!=4 && result!=5)
            JUT_ASSERT_fail(__FILE_NAME__, 0x209, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return result;
}
/* 0234C550 */
s32 daObjGryw00_c::Mthd_Delete() {
    s32 result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, l_arcname);
    return result;
}
/* 0234C5C4 */
f32 daObjGryw00_c::get_draw_water_lv(daObjGryw00_c* p) {
    f32 value = p->home.pos.y+p->mWaterLv;
    return value+p->mWaveAmplitude;
}
/* 0234C5DC */
BOOL daObjGryw00_c::CreateHeap() {
    auto data = static_cast<J3DModelData*>(objectResource(dRes_INDEX_GRYW00_BDL_GRYW00_e));
    auto texture = static_cast<J3DAnmTextureSRTKey*>(objectResource(dRes_INDEX_GRYW00_BTK_GRYW00_e));
    auto transform = static_cast<J3DAnmTransform*>(objectResource(dRes_INDEX_GRYW00_BCK_GRYW00_e));
    if (!data || !texture || !transform) {
        JUT_ASSERT(0x199, FALSE);
        return FALSE;
    }
    J3DModel* created = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    mpModel = created;
    if (!created) return FALSE;
    J3DModelData* modelData = created->getModelData();
    BOOL textureReady = mBtk.init(modelData, texture, true, 0, 0.0f, 0, 400, false, 0);
    modelData = mpModel->getModelData();
    BOOL transformReady = mBck.init(modelData, transform, true, 0, 0.0f, 0, -1, false);
    if (!mpModel || !textureReady || !transformReady) return FALSE;
    return TRUE;
}
/* 0234C768 */
BOOL daObjGryw00_c::setup_high_water_level_btk_anm() {
    auto texture = static_cast<J3DAnmTextureSRTKey*>(objectResource(dRes_INDEX_GRYW00_BTK_GRYW00_e));
    if (!texture) JUT_ASSERT_fail(__FILE_NAME__, 0xDF, "btk_anm_p != NULL");
    J3DModelData* data = mpModel->getModelData();
    return mBtk.init(data, texture, true, 2, 1.0f, 401, -1, true, 0) ? TRUE : FALSE;
}
/* 0234C838 */
BOOL daObjGryw00_c::Create() {
    u32 sw = PrmAbstract(this, 8, 0);
    f32 finalY = home.pos.y+510.0f;
    mActivationSwitch = sw;
    mWaterLvFinalYPos = finalY;
    bool activated = false;
    if (sw!=255) activated = dComIfGs_isSwitch(sw, home.roomNo)==TRUE;
    if (activated) {
        setup_high_water_level_btk_anm();
        mIsHidden = 0;
        mWaterLv = 510.0f;
        mBck.mFrameCtrl.mFrame = 299.0f;
        mWaveMaxAmplitude = -10.0f;
        mWaterMaxLv = 510.0f;
        mWaveAmplitude = -10.0f;
        changeAction(this, HIGH_WATER_LEVEL);
    } else {
        mIsHidden = 1;
        mWaterMaxLv = -5.0f;
        mWaveAmplitude = 0.0f;
        mWaveMaxAmplitude = 0.0f;
        changeAction(this, SWITCH_WAIT);
        mWaterLv = -5.0f;
    }
    f32 x = scale.x, y = scale.y, z = scale.z;
    mpModel->mBaseScale.x = x;
    mpModel->mBaseScale.z = z;
    mpModel->mBaseScale.y = y;
    f32 level = home.pos.y+mWaterLv;
    f32 drawY = level+mWaveAmplitude;
    f32 homeZ = home.pos.z, homeX = home.pos.x;
    mDoMtx_stack_c::transS(homeX, drawY, homeZ);
    mtx_copy(&mpModel->mBaseTransformMtx, mDoMtx_stack_c::get());
    cullMtx = gabi::ea(mpModel->getBaseTRMtx());
    mParticleRetireRemaining = 0;
    return TRUE;
}
// HD: reflect the Y coordinate for emitter types >= 7.
/* 0234CAE8 */
void daObjGryw00_c::particle_move() {
    f32 y = home.pos.y+mWaterLv, z = home.pos.z, x = home.pos.x;
    for (u32 i = 0;i<2;++i) {
        GrywEmitter* emitter = mpEmitters[i];
        if (!emitter) continue;
        u8 type = emitter->mType;
        emitter->mGlobalTranslation.x = x;
        emitter->mGlobalTranslation.z = z;
        emitter->mGlobalTranslation.y = y;
        if (type>=7) emitter->mGlobalTranslation.y = -(f32)emitter->mGlobalTranslation.y;
    }
}
/* 0234CB60 */
void daObjGryw00_c::set_se() {
    if (mShouldPlaySe!=TRUE) return;
    f32 water = mWaterLv;
    s32 remaining = mGeyserSeRemaining;
    if (!(water<510.0f)) mShouldPlaySe = FALSE;
    if (remaining>0) { mGeyserSeRemaining = remaining-1;
    soundCurrent(this, JA_SE_OBJ_RDUN_MAE_GEYSER);
    }
    soundCurrent(this, JA_SE_OBJ_RDUN_MAE_W_OUT);
    soundCurrent(this, JA_SE_OBJ_RDUN_MAE_W_UP);
}
/* 0234CC24 */
BOOL daObjGryw00_c::Execute(gptr<Mtx34>* outMtx) {
    f32 wave = mWaveAmplitude+mWaveAmplitudeIncrement;
    f32 water = mWaterLv, limit = mWaveMaxAmplitude, step = mWaterLvIncrement;
    mWaveAmplitude = wave;
    f32 waterLimit = mWaterMaxLv;
    if (wave<limit) { mWaveAmplitudeIncrement = 0.0f;
    mWaveAmplitude = limit;
    }
    water = water+step;
    mWaterLv = water;
    if (water>waterLimit) { mWaterLvIncrement = 0.0f;
    mWaterLv = waterLimit;
    }
    f32 homeX = home.pos.x;
    f32 drawY = get_draw_water_lv(this);
    f32 homeZ = home.pos.z;
    mDoMtx_stack_c::transS(homeX, drawY, homeZ);
    mtx_copy(&mpModel->mBaseTransformMtx, mDoMtx_stack_c::get());
    f32 waterY = home.pos.y+mWaterLv;
    homeZ = home.pos.z;
    homeX = home.pos.x;
    mDoMtx_stack_c::transS(homeX, waterY, homeZ);
    f32 scaleZ = scale.z, scaleX = scale.x, scaleY = scale.y;
    Gryw_scaleM(scaleX, scaleY, scaleZ);
    Gryw_copyMatrix(mDoMtx_stack_c::get(), &mMtx);
    *outMtx=&mMtx;
    particle_move();
    set_se();
    mBtk.play();
    mBck.play();
    (this->*modeFunc)();
    return TRUE;
}
/* 0234CDE4 */
BOOL daObjGryw00_c::Draw() {
    if (!mIsHidden) {
        settingTevStruct(dKy_getEnvlight(), 2, &current.pos, &tevStr);
        auto light = dKy_getEnvlight();
        setLightTevColorType(light, mpModel, &tevStr);
        J3DModel* m = mpModel;
        f32 frame = mBtk.getFrame();
        mBtk.entry(m->getModelData(), frame);
        m = mpModel;
        frame = mBck.getFrame();
        mBck.entry(m->getModelData(), frame);
        mDoExt_modelUpdateDL(mpModel);
    }
    return TRUE;
}
/* 0234CE74 */
void daObjGryw00_c::particle_delete() {
    for (u32 i = 0;i<2;++i) {
        GrywEmitter* emitter = mpEmitters[i];
        if (!emitter) continue;
        u32 flags = emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags|1;
        mpEmitters[i] = nullptr;
    }
}
/* 0234CEC4 */
BOOL daObjGryw00_c::Delete() {
    particle_delete();
    return TRUE;
}
/* 0234CEE8 */
void daObjGryw00_c::particle_set() {
    gabi::Local<cXyz> homePosition;
    homePosition->x = home.pos.x;
    homePosition->y = home.pos.y;
    homePosition->z = home.pos.z;
    GXColor* tint = &tevStr.mColorK0;
    dComIfGp_particle_set(dPa_name::ID_AK_SN_RYUDANMAESPLASH00, homePosition, nullptr, nullptr, 255, nullptr, -1, nullptr, tint);
    dComIfGp_particle_set(dPa_name::ID_AK_SN_RYUDANMAESTEAM00, homePosition, nullptr, nullptr, 255, nullptr, -1, nullptr, tint);
    mpEmitters[0] = reinterpret_cast<GrywEmitter*>(dComIfGp_particle_set(dPa_name::ID_AK_SN_RYUDANMAESPLASH01, homePosition, nullptr, nullptr, 255, nullptr, -1, nullptr, tint));
    mpEmitters[1] = reinterpret_cast<GrywEmitter*>(dComIfGp_particle_set(dPa_name::ID_AK_SN_RYUDANMAEHAMON00, homePosition, nullptr, nullptr, 255, nullptr, -1, nullptr, tint));
}
/* 0234D024 */
void daObjGryw00_c::switch_wait_act_proc() {
    s32 sw = mActivationSwitch;
    if (sw==255 || dComIfGs_isSwitch(sw, home.roomNo)!=TRUE) return;
    mWaveMaxAmplitude = -10.0f;
    mWaveAmplitudeIncrement = -0.10526315867900848f;
    mBck.mFrameCtrl.mRate = 1.0f;
    mBtk.mFrameCtrl.mRate = 1.0f;
    mWaterMaxLv = 5.0f;
    mWaterLvIncrement = 0.10526315867900848f;
    particle_set();
    mIsHidden = 0;
    mShouldPlaySe = TRUE;
    mGeyserSeRemaining = 310;
    changeAction(this, SPREAD_WATER_FACE);
}
/* 0234D0F0 */
void daObjGryw00_c::spread_water_face_act_proc() {
    if (Gryw_checkFrame(&mBtk.mFrameCtrl, 95.0f)) changeAction(this, WATER_LEVEL_MOVE_WAIT);
}
/* 0234D14C */
void daObjGryw00_c::water_level_move_wait_act_proc() {
    if (Gryw_checkFrame(&mBtk.mFrameCtrl, 299.0f)) {
        mWaterMaxLv = 510.0f;
        mBck.mFrameCtrl.mRate = 0.0f;
        mWaterLvIncrement = 1.6777408123016357f;
        mBck.mFrameCtrl.mFrame = 299.0f;
        changeAction(this, ANIME_LOOP_START_WAIT);
    }
}
// HD: retire the emitters after a delay, rather than immediately.
/* 0234D1EC */
void daObjGryw00_c::anime_loop_start_wait_act_proc() {
    if (!(mBtk.mFrameCtrl.mState&1) && mBtk.mFrameCtrl.mRate!=0.0f) return;
    mParticleRetireRemaining = 100;
    setup_high_water_level_btk_anm();
    changeAction(this, HIGH_WATER_LEVEL);
}
/* 0234D260 */
void daObjGryw00_c::high_water_level_act_proc() {
    s32 remaining = mParticleRetireRemaining;
    if (!remaining) return;
    remaining = (s32)((u32)remaining-1);
    mParticleRetireRemaining = remaining;
    if (!remaining) particle_delete();
}
/* 0234D27C */
static void sinit() {
    sinit_header_statics(0x10469CC8, 0x101C9A48);
}
/* 0234D310 */
static void trivial_dt(void* p, s32 flags) {
    if (p && (flags&1)) operator_delete(p);
}
/* 0234D324 */
static BOOL isDelete(daObjGryw00_c* p) {
     return TRUE;
}
/* 0234D32C */
static void destroy(daObjGryw00_c* p, s32 flags) {
    if (p) {
        Gryw_objectDestruct(&p->mBckState.mVtable, 0);
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags&1) operator_delete(p);
    }
}
/* 0234C54C */
static s32 daObjGryw00_Create(daObjGryw00_c* p) {
     return p->Mthd_Create();
}
/* 0234C59C */
static s32 daObjGryw00_Delete(daObjGryw00_c* p) {
     return p->Mthd_Delete();
}
/* 0234C5A0 */
static BOOL daObjGryw00_Execute(daObjGryw00_c* p) {
     return p->MoveBGExecute();
}
/* 0234C5A4 */
static BOOL daObjGryw00_Draw(daObjGryw00_c* p) {
     return p->Draw_v();
}
/* 0234C5B4 */
static BOOL daObjGryw00_IsDelete(daObjGryw00_c* p) {
     return p->IsDelete_v();
}

/* ---- leftover functions of the translation unit ---- */

// HD: this TU has an empty SafeString termination hook.
/* 0234D38C */
static void gryw00_SafeString_assureTermination(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
ext 027F2BC0 void Gryw_frameInit(J3DFrameCtrl*, s32)
ext 027DA984 void Gryw_objectConstruct(void*)
ext 027F3628 void Gryw_objectDestruct(void*, s32)
ext 027F2BF8 BOOL Gryw_checkFrame(J3DFrameCtrl*, f32)
ext 025F2518 void Gryw_scaleM(f32, f32, f32)
ext 028E90D4 void Gryw_copyMatrix(const Mtx34*, Mtx34*)
str 10029938 "Gryw00"
str 10029980 "d_a_obj_gryw00.cpp" in 0234C420
str 10029994 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 100299E0 "d_a_obj_gryw00.cpp" in 0234C5DC
str 100299DC "FALSE" renamed # HD: assert spells zero.
str 100299F8 "d_a_obj_gryw00.cpp" in 0234C768
str 10029A0C "btk_anm_p != NULL"
*/
