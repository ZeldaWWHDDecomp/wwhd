/* Big Octo eye: joint control, damage modes and hit particles.
 * GameCube names follow zeldaret/tww; verified against the private WWHD image.
 * HD: animation/resource ABI, callback cleanup and compiler-generated initialization differ.
 */
#include "d/actor/d_a_daiocta_eye.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
using namespace gabi;
using namespace octoEyeHD;

static Hio l_HIO;
static J3DModel* l_currentModel;
static Mtx34 l_currentMtx;
static dCcD_SrcSph l_sph_src;
static JointHitData search_data;
static f32 scale_table[] = {1.0f, 0.975f, 1.0f, 0.95f, 1.0f, 0.9f, 1.0f, 0.85f};
static HeaderStatics l_header;
static HeaderVector l_headerVector0;
static HeaderVector l_headerVector1;
static HeaderVector l_headerVector2;
static f32 l_negativePi;
static f32 l_positivePi;
static void* l_resourceControl;

extern void* eyeGetResource(void*, SafeString*, u32);
extern void eyeMonsterSound(u32, cXyz*, u32, u32, s8);
extern JntHit_c* eyeCreateJntHit(J3DModel*, const JointHitData*, s32);
extern BOOL eyeBombCheckState(fopAc_ac_c*, s32);
extern void eyeInitActor(fopAc_ac_c*);
extern void eyeConstructBrk(mDoExt_brkAnm*);
extern void eyeConstructBtk(mDoExt_btkAnm*);
extern void eyeConstructSph(dCcD_Sph*);
extern void eyeConstructStts(dCcD_Stts*);
extern void eyeConstructGlobalStts(void*);
extern void eyeConstructCallback(dPa_followEcallBack*, u32, u32);
extern void eyeEnemyDamage();
extern void eyeScaleAnime(be<f32>*, const be<f32>*, s32, be<s32>*, f32, f32, f32);
extern void eyeInitJntHio(void*);
extern void eyeRegisterGlobalObject(HeaderVector*);
extern void eyeConstructAngle(u8*);
extern void eyeConstructByte(u8*);
extern void eyeDestroyStts(dCcD_Stts*, s32);
extern void eyeDestroySph(dCcD_Sph*, s32);
extern void eyeDestroyActor(fopAc_ac_c*, s32);
extern void eyeInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s32, s32,
                       s32, s32);
extern void eyeInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, s32, s32, f32, s32, s32,
                       s32, s32);
extern JPABaseEmitter* eyeParticleSet(dPa_control_c*, u32, u32, const cXyz*, const csXyz*,
                                      const cXyz*, u32, dPa_followEcallBack*, s32, const void*,
                                      const void*, u32);
extern void eyeSetCollision(cCcS*, dCcD_Sph*);
extern f32 eyeGetWaterHeight(cXyz*);
extern void eyeSetLighting(void*, s32, cXyz*, dKy_tevstr_c*);
extern void eyeModelLighting(void*, J3DModel*, dKy_tevstr_c*);
extern void* eyeGetLighting();
extern void eyeModelUpdate(J3DModel*, s32);
extern void eyeAddAngle(be<s16>*, s16, s16, s16);
extern void eyeSetSphereRadius(cM3dGSph*, f32);
extern void eyeSetSphereCenter(cM3dGSph*, cXyz*);

// HD: resource name is capitalized; SafeString follows eight bytes of linkage scratch.
static void* getObjectResource(Local<ResourceLocal>& local, u32 id) {
    local->name.mStringTop = guest_addr("Daiocta");
    local->name.__vtbl = 0x1000D4B4;
    return eyeGetResource(l_resourceControl, &local->name, id);
}
static void parentSound(fopAc_ac_c* parent, u32 sound) {
    // HD: both null tests are retained, including guest address wrap at eyePos.
    if (!parent || !ea(&parent->eyePos))
        return;
    s8 room = parent->current.roomNo;
    u32 id = parent->mBsPcId;
    s8 reverb = dComIfGp_getReverb(room);
    eyeMonsterSound(sound, &parent->eyePos, id, 0, reverb);
}
static void actorSound(daDaiocta_Eye_c* actor, u32 sound, s32 extra) {
    s8 reverb = dComIfGp_getReverb(actor->current.roomNo);
    mDoAud_seStart(sound, &actor->eyePos, extra, reverb);
}
static void removeParticle(daDaiocta_Eye_c* actor) {
    auto* table = at<CallbackVtable>(actor->mParticleCallback.__vtbl);
    call_ptr<void>(table->mRemove, &actor->mParticleCallback);
}

/* 0211D42C */
BOOL nodeControl_CB(J3DNode* i_nodeP, s32 i_calcTiming) {
    if (i_calcTiming == J3DNodeCBCalcTiming_In) {
        J3DModel* model_p = l_currentModel;
        auto* this_p = static_cast<daDaiocta_Eye_c*>(model_p->getUserArea());
        if (this_p)
            this_p->_nodeControl(i_nodeP, model_p);
    }
    return 1;
}
/* 0211D314 */
void daDaiocta_Eye_c::_nodeControl(J3DNode* i_nodeP, J3DModel* i_modelP) {
    auto* jnt_p = reinterpret_cast<J3DJoint*>(i_nodeP);
    u32 jnt_no = jnt_p->getJntNo();
    mDoMtx_stack_c::copy(i_modelP->getAnmMtx(jnt_no));
    if (jnt_no == DO_EYE1_JNT_J_DO_EYE2_e) {
        mDoMtx_stack_c::ZXYrotM(mCurEyeRot);
        mDoMtx_stack_c::scaleM(mEyeScale.x, mEyeScale.y, mEyeScale.z);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), &l_currentMtx);
    i_modelP->setAnmMtx(jnt_no, mDoMtx_stack_c::get());
}
/* 0211D658 */
BOOL createHeap_CB(daDaiocta_Eye_c* i_this) {
    return i_this->_createHeap();
}
/* 0211D474 */
BOOL daDaiocta_Eye_c::_createHeap() {
    Local<ResourceLocal> modelName, brkName, btkName;
    auto* modelData =
        static_cast<J3DModelData*>(getObjectResource(modelName, dRes_INDEX_DAIOCTA_BDL_DO_EYE1_e));
    if (!modelData)
        JUTAssertion::showAssert("d_a_daiocta_eye.cpp", 0xE9, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mpModel)
        return 0;
    mpModel->setUserArea(reinterpret_cast<uintptr_t>(this));
    mpBrk = static_cast<J3DAnmTevRegKey*>(
        getObjectResource(brkName, dRes_INDEX_DAIOCTA_BRK_DAMAGE_EYE_A1_e));
    if (!mpBrk)
        return 0;
    if (!mBrkAnm.init(modelData, mpBrk, true, 0, 1.0f, 0, -1, false, 0))
        return 0;
    mpBtk = static_cast<J3DAnmTextureSRTKey*>(
        getObjectResource(btkName, dRes_INDEX_DAIOCTA_BTK_DAMAGE_EYE_A1_e));
    if (!mpBtk)
        return 0;
    if (!mBtkAnm.init(modelData, mpBtk, true, 0, 1.0f, 0, -1, false, 0))
        return 0;
    modelData->getJointNodePointer(DO_EYE1_JNT_J_DO_EYE2_e)->setCallBack(nodeControl_CB);
    mpJntHit = eyeCreateJntHit(mpModel, &search_data, 1);
    if (!mpJntHit)
        return 0;
    jntHit = ea(mpJntHit.get());
    return 1;
}
/* 0211D8B8 */
void coHit_CB(daDaiocta_Eye_c* i_this, void* info, fopAc_ac_c* i_actor) {
    if (i_actor)
        i_this->_coHit(i_actor);
}
/* 0211D7CC */
void daDaiocta_Eye_c::_coHit(fopAc_ac_c* i_actor) {
    if (!i_actor || i_actor->mProcName != fpcNm_BOMB_e)
        return;
    if (!eyeBombCheckState(i_actor, 4))
        return;
    if (mbIsDead) {
        mbIsDamagedBomb = 1;
        return;
    }
    s8 room = current.roomNo;
    health = 0;
    mbIsDead = 1;
    m295 = 1;
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    eyeParticleSet(play->play.mParticle, 0, ID_IT_SN_DO_SMOKE00, &mParticlePos, &shape_angle,
                   nullptr, 0xFF, &mParticleCallback, room, nullptr, nullptr, 0);
    modeDeathInit();
}
/* 0211DB74 */
void daDaiocta_Eye_c::setMtx() {
    // HD: load all scale components before writing the model, as GHS does.
    f32 x = scale.x, y = scale.y, z = scale.z;
    mpModel->mBaseScale.x = x;
    mpModel->mBaseScale.y = y;
    mpModel->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mDoMtx_stack_c::transM(l_HIO.mParticleTranslationX, l_HIO.mParticleTranslationY,
                           l_HIO.mParticleTranslationZ);
    mParticlePos.x = mDoMtx_stack_c::get()->m[0][3];
    mParticlePos.y = mDoMtx_stack_c::get()->m[1][3];
    mParticlePos.z = mDoMtx_stack_c::get()->m[2][3];
}
/* 0211E114 */
void daDaiocta_Eye_c::checkTgHit() {
    mStts.Move();
    auto* tg_hit_obj_p = static_cast<dCcD_GObjInf*>(mSph.GetTgHitObj());
    if (!tg_hit_obj_p)
        return;
    eyeEnemyDamage();
    u32 type = tg_hit_obj_p->mAtType;
    s32 damage;
    u32 sound;
    if (type & AT_TYPE_ARROW_GROUP) {
        damage = 2;
        sound = JA_SE_LK_MS_WEP_HIT;
    } else if (type & AT_TYPE_LIGHT_ARROW) {
        damage = 4;
        sound = JA_SE_LK_MS_WEP_HIT;
    } else if (type & AT_TYPE_HOOKSHOT) {
        damage = 2;
        sound = JA_SE_LK_MS_WEP_HIT;
    } else if (type & AT_TYPE_BOOMERANG) {
        damage = 1;
        sound = JA_SE_LK_W_WEP_HIT;
    } else
        return;
    actorSound(this, sound, 32);
    health = static_cast<u8>(health - damage);
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player_p = play->play.mPlayerInfo[0].mpPlayer;
    Local<cXyz> tg_hit_pos, hit_particle_scale;
    tg_hit_pos->x = mSph.mGObjTg.mHitPos.x;
    tg_hit_pos->y = mSph.mGObjTg.mHitPos.y;
    tg_hit_pos->z = mSph.mGObjTg.mHitPos.z;
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    eyeParticleSet(play->play.mParticle, 0, ID_AK_JN_CRITICALHITFLASH, tg_hit_pos.get(), nullptr,
                   nullptr, 0xFF, nullptr, -1, nullptr, nullptr, 0);
    if (health > 0) {
        modeDamageInit();
        return;
    }
    hit_particle_scale->x = 2;
    hit_particle_scale->y = 2;
    hit_particle_scale->z = 2;
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    eyeParticleSet(play->play.mParticle, 0, ID_AK_JN_CRITICALHIT, tg_hit_pos.get(),
                   &player_p->shape_angle, hit_particle_scale.get(), 0xFF, nullptr, -1, nullptr,
                   nullptr, 0);
    actorSound(this, JA_SE_LK_LAST_HIT, 0);
    health = 0;
    modeDeathInit();
}
/* 0211DFB8 */
void daDaiocta_Eye_c::modeWaitInit() {
    mMode = MODE_WAIT;
}
/* 0211E40C */
void daDaiocta_Eye_c::modeWait() {
    checkTgHit();
}
/* 0211DFC4 */
void daDaiocta_Eye_c::modeDamageInit() {
    mMode = MODE_DAMAGE;
    mbIsDamaged = 1;
    J3DModel* model_p = mpModel;
    mScaleAnimeIdx = m_scale_damage_time;
    J3DModelData* model_data_p = model_p->getModelData();
    Local<ResourceLocal> brkName, btkName;
    mpBrk = static_cast<J3DAnmTevRegKey*>(
        getObjectResource(brkName, dRes_INDEX_DAIOCTA_BRK_DAMAGE_EYE_A1_e));
    eyeInitBrk(&mBrkAnm, model_data_p, mpBrk, 1, 0, 1.0f, 0, -1, 1, 0);
    mpBtk = static_cast<J3DAnmTextureSRTKey*>(
        getObjectResource(btkName, dRes_INDEX_DAIOCTA_BTK_DAMAGE_EYE_A1_e));
    eyeInitBtk(&mBtkAnm, model_data_p, mpBtk, 1, 0, 1.0f, 0, -1, 1, 0);
    parentSound(mpParentActor, JA_SE_CV_DO_DAMAGE);
}
/* 0211E410 */
void daDaiocta_Eye_c::modeDamage() {
    eyeScaleAnime(&mEyeScale.x, scale_table, m_scale_damage_time, &mScaleAnimeIdx, 0.2f, 10.0f,
                  0.025f);
    f32 x = mEyeScale.x;
    mEyeScale.z = x;
    mEyeScale.y = x;
    if (mBtkAnm.isStop() && mBrkAnm.isStop()) {
        if (mScaleAnimeIdx == 0)
            modeWaitInit();
    } else if (mScaleAnimeIdx == 0)
        mScaleAnimeIdx = m_scale_damage_time;
}
/* 0211D65C */
void daDaiocta_Eye_c::modeDeathInit() {
    mMode = MODE_DEATH;
    mEyeScale.y = 1;
    mbIsDamaged = 1;
    mEyeScale.z = 1;
    mEyeScale.x = 1;
    Local<ResourceLocal> brkName, btkName;
    mpBrk = static_cast<J3DAnmTevRegKey*>(
        getObjectResource(brkName, dRes_INDEX_DAIOCTA_BRK_DAMAGE_EYE_B1_e));
    J3DModelData* data = mpModel->getModelData();
    eyeInitBrk(&mBrkAnm, data, mpBrk, 1, 0, 1.0f, 0, -1, 1, 0);
    mpBtk = static_cast<J3DAnmTextureSRTKey*>(
        getObjectResource(btkName, dRes_INDEX_DAIOCTA_BTK_DAMAGE_EYE_B1_e));
    data = mpModel->getModelData();
    eyeInitBtk(&mBtkAnm, data, mpBtk, 1, 0, 1.0f, 0, -1, 1, 0);
    parentSound(mpParentActor, JA_SE_CV_DO_EYE_LOSE);
}
/* 0211E4E8 */
void daDaiocta_Eye_c::modeDeath() {
    mbIsDead = 1;
}
/* 0211DB28 */
void daDaiocta_Eye_c::modeProcCall() {
    static ModeProc mode_proc[] = {&daDaiocta_Eye_c::modeWait, &daDaiocta_Eye_c::modeDamage,
                                   &daDaiocta_Eye_c::modeDeath};
    (this->*mode_proc[static_cast<u32>(mMode)].fn)();
}
/* 0211DCAC */
BOOL daDaiocta_Eye_c::_execute() {
    if (!mbAppear)
        return 0;
    if (mbIsDead)
        mSph.mObjTg.mSPrm = mSph.mObjTg.mSPrm & ~1u;
    m2EC = l_HIO.m24;
    m2EE = l_HIO.m26;
    m2F0 = l_HIO.m28;
    mEyeRndRotMaxX = l_HIO.mEyeRndRotMaxX;
    mEyeRndRotMaxY = l_HIO.mEyeRndRotMaxY;
    mEyeRndRotMaxZ = l_HIO.mEyeRndRotMaxZ;
    mBrkAnm.play();
    mBtkAnm.play();
    mbIsDamaged = 0;
    mbIsDamagedBomb = 0;
    modeProcCall();
    setMtx();
    // HD: attention copies use floats; eyePos uses integer words. Keep their interleaving.
    f32 y = current.pos.y, z = current.pos.z;
    attention_info.position.z = z;
    f32 above = y + 100.0f;
    f32 x = current.pos.x;
    auto* source = reinterpret_cast<VectorWords*>(&current.pos);
    auto* eye = reinterpret_cast<VectorWords*>(&eyePos);
    u32 zbits = source->z, xbits = source->x;
    eye->z = zbits;
    eye->x = xbits;
    u32 ybits = source->y;
    attention_info.position.x = x;
    attention_info.position.y = above;
    eye->y = ybits;
    eyeAddAngle(&mCurEyeRot.x, mEyeRotTargetX, static_cast<s16>(l_HIO.mEyeRotScale + 1),
                l_HIO.mEyeRotStep);
    eyeAddAngle(&mCurEyeRot.y, mEyeRotTargetY, static_cast<s16>(l_HIO.mEyeRotScale + 1),
                l_HIO.mEyeRotStep);
    eyeAddAngle(&mCurEyeRot.z, mEyeRotTargetZ, static_cast<s16>(l_HIO.mEyeRotScale + 1),
                l_HIO.mEyeRotStep);
    eyeSetSphereRadius(&mSph.mSph, l_HIO.mSphRadius);
    eyeSetSphereCenter(&mSph.mSph, &current.pos);
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    eyeSetCollision(&play->play.mCcS, &mSph);
    Local<cXyz> current_pos;
    x = current.pos.x;
    y = current.pos.y;
    m498.z = 0;
    s16 rotation = shape_angle.y;
    current_pos->x = x;
    current_pos->y = y + 1000.0f;
    m498.y = rotation;
    z = current.pos.z;
    m498.x = 0;
    current_pos->z = z;
    f32 water_height = eyeGetWaterHeight(current_pos.get());
    y = current.pos.y;
    // HD: PPC bge takes unordered values; death may return before the event query.
    if (!(y < water_height)) {
        if (mMode == MODE_DEATH) {
            attention_info.flags = 0;
            return 0;
        }
        attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 34;
    } else if (mParticleCallback.mpEmitter) {
        removeParticle(this);
        if (mMode == MODE_DEATH) {
            attention_info.flags = 0;
            return 0;
        }
    }
    if (mMode == MODE_DEATH)
        attention_info.flags = 0;
    else if (dComIfGp_event_runCheck())
        attention_info.flags = 0;
    return 0;
}
/* 0211DEF0 */
BOOL daDaiocta_Eye_c::_draw() {
    if (!mbAppear)
        return 0;
    void* light = eyeGetLighting();
    eyeSetLighting(light, 0, &current.pos, &tevStr);
    light = eyeGetLighting();
    eyeModelLighting(light, mpModel, &tevStr);
    J3DModelData* data = mpModel->getModelData();
    f32 frame = mBtkAnm.getFrame();
    mBtkAnm.entry(data, frame);
    data = mpModel->getModelData();
    frame = mBrkAnm.getFrame();
    mBrkAnm.entry(data, frame);
    eyeModelUpdate(mpModel, 0);
    reinterpret_cast<ModelAnimationEntries*>(mpModel->getModelData())->mBtkEntry = 0;
    reinterpret_cast<ModelAnimationEntries*>(mpModel->getModelData())->mBrkEntry = 0;
    return 1;
}
/* 0211D8C8 */
void daDaiocta_Eye_c::createInit() {
    J3DModel* model_p = mpModel;
    mbAppear = 0;
    cullMtx = ea(model_p ? &model_p->mBaseTransformMtx : nullptr);
    mStts.Init(0, 0, this);
    mSph.Set(&l_sph_src);
    u32 parentId = parentActorID;
    health = 4;
    mEyeScale.y = 1;
    mEyeScale.x = 1;
    mSph.SetStts(&mStts);
    mSph.mGObjCo.mHitCallback = guest_fn(reinterpret_cast<void*>(coHit_CB));
    max_health = 4;
    mEyeScale.z = 1;
    if (parentId != 0xFFFFFFFF) {
        fopAc_ac_c* parent_p = fopAcM_SearchByID(parentId);
        if (parent_p && fopAc_IsActor(parent_p) && parent_p->mProcName == fpcNm_DAIOCTA_e)
            mpParentActor = parent_p;
    }
}
/* 0211D9BC */
s32 daDaiocta_Eye_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & 8)) {
        if (ea(this)) {
            eyeInitActor(this);
            __vtbl = 0x1000D4CC;
            eyeConstructBrk(&mBrkAnm);
            eyeConstructBtk(&mBtkAnm);
            eyeConstructSph(&mSph);
            eyeConstructStts(&mStts);
            eyeConstructGlobalStts(&mStts.__vtbl_gstts);
            mStts.__vtbl = 0x1004AE88;
            mStts.__vtbl_gstts = 0x1004AEC0;
            eyeConstructCallback(&mParticleCallback, 0, 0);
            flags = actor_condition;
        }
        actor_condition = flags | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Daiocta");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, guest_fn(reinterpret_cast<void*>(createHeap_CB)),
                                   m_heapsize))
            return cPhs_ERROR_e;
        createInit();
    }
    return phase;
}
/* 0211DAD4 */
BOOL daDaiocta_Eye_c::_delete() {
    dComIfG_resDelete(&mPhs, "Daiocta");
    removeParticle(this);
    return 1;
}
/* 0211DAD0 */
s32 daDaiocta_EyeCreate(daDaiocta_Eye_c* i_this) {
    return i_this->_create();
}
/* 0211DB24 */
BOOL daDaiocta_EyeDelete(daDaiocta_Eye_c* i_this) {
    return i_this->_delete();
}
/* 0211DEEC */
BOOL daDaiocta_EyeExecute(daDaiocta_Eye_c* i_this) {
    return i_this->_execute();
}
/* 0211DFB4 */
BOOL daDaiocta_EyeDraw(daDaiocta_Eye_c* i_this) {
    return i_this->_draw();
}
/* 0211E678 */
BOOL daDaiocta_EyeIsDelete(daDaiocta_Eye_c* i_this) {
    return 1;
}

// HD: compiler-generated constructors, destructors and static initialization.
/* 0211E4F4 */
void* daDaiocta_Eye_HIO_construct(void* object) {
    auto* hio = static_cast<Hio*>(object);
    if (!hio) {
        hio = static_cast<Hio*>(operator_new(sizeof(Hio)));
        if (!hio)
            return nullptr;
    }
    hio->__vtbl = 0x1000D4DC;
    eyeInitJntHio(hio->m30);
    hio->mEyeRotStep = 0x1000;
    hio->m26 = 0x1000;
    hio->m04 = 0xF;
    hio->mSphRadius = 195.0f;
    hio->m28 = 0x1000;
    hio->mEyeRndRotMaxY = 0x2000;
    hio->mEyeRndRotMaxZ = 0x2000;
    hio->mParticleTranslationZ = 30;
    hio->mParticleTranslationY = 0;
    hio->m08 = 5;
    hio->mEyeRndRotMaxX = 0x7FFF;
    hio->mEyeRotScale = 4;
    hio->m0C = 8;
    hio->mParticleTranslationX = 0;
    hio->m24 = 0x4000;
    return hio;
}
/* 0211E5C4 */
void daDaiocta_Eye_staticInitialize() {
    l_header.mZero[2] = 0;
    l_header.mZero[0] = 0;
    l_header.mZero[3] = 0;
    l_header.mZero[1] = 0;
    eyeRegisterGlobalObject(&l_headerVector0);
    f32 minimum = l_negativePi, maximum = l_positivePi;
    l_header.mNegativePi = minimum;
    l_header.mPositivePi = maximum;
    eyeConstructAngle(&l_header.mAngleTag);
    eyeRegisterGlobalObject(&l_headerVector1);
    eyeConstructByte(&l_header.mByteTag);
    eyeRegisterGlobalObject(&l_headerVector2);
    daDaiocta_Eye_HIO_construct(&l_HIO);
}
/* 0211E664 */
void daDaiocta_Eye_HIO_destroy(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0211E680 */
void daDaiocta_Eye_destroy(daDaiocta_Eye_c* i_this, u32 flags) {
    if (!i_this)
        return;
    eyeDestroyStts(&i_this->mStts, 2);
    eyeDestroySph(&i_this->mSph, 2);
    eyeDestroyActor(i_this, 0);
    if (flags & 1)
        operator_delete(i_this);
}
/* 0211E6EC */
void daDaiocta_Eye_HIO_genMessage(void* object) {}
/* WWHD glue
str 1000D568
str 1000D4F0
str 1000D504
data l_HIO 10463A68
data l_currentModel 104B462C
data l_currentMtx 104B4868
data l_sph_src 101B411C
data search_data 101B417C
data scale_table 101B4194
data l_header 10463A4C
data l_headerVector0 101B41B4
data l_headerVector1 101B41C0
data l_headerVector2 101B41CC
data l_negativePi 1000D560
data l_positivePi 1000D564
data l_resourceControl 101F4F28
data mode_proc 1000D518
ext 026066C4 void* eyeGetResource(void*, SafeString*, u32)
ext 025E1AA4 void eyeMonsterSound(u32, cXyz*, u32, u32, s8)
ext 02552B60 JntHit_c* eyeCreateJntHit(J3DModel*, const JointHitData*, s32)
ext 020CB92C BOOL eyeBombCheckState(fopAc_ac_c*, s32)
ext 025D4ED0 void eyeInitActor(fopAc_ac_c*)
ext 025E80D0 void eyeConstructBrk(mDoExt_brkAnm*)
ext 025E7C6C void eyeConstructBtk(mDoExt_btkAnm*)
ext 025166F0 void eyeConstructSph(dCcD_Sph*)
ext 0200BD2C void eyeConstructStts(dCcD_Stts*)
ext 02515DA0 void eyeConstructGlobalStts(void*)
ext 025A5894 void eyeConstructCallback(dPa_followEcallBack*, u32, u32)
ext 025E1FD8 void eyeEnemyDamage()
ext 02587BE0 void eyeScaleAnime(be<f32>*, const be<f32>*, s32, be<s32>*, f32, f32, f32)
ext 02552BE8 void eyeInitJntHio(void*)
ext 028F026C void eyeRegisterGlobalObject(HeaderVector*)
ext 028ED6F8 void eyeConstructAngle(u8*)
ext 028EAB2C void eyeConstructByte(u8*)
ext 02515860 void eyeDestroyStts(dCcD_Stts*, s32)
ext 02515AE8 void eyeDestroySph(dCcD_Sph*, s32)
ext 025D50BC void eyeDestroyActor(fopAc_ac_c*, s32)
ext 025E8154 void eyeInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s32, s32, s32, s32)
ext 025E7CE0 void eyeInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, s32, s32, f32, s32, s32, s32, s32)
ext 025A847C JPABaseEmitter* eyeParticleSet(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32, dPa_followEcallBack*, s32, const void*, const void*, u32)
ext 0200E240 void eyeSetCollision(cCcS*, dCcD_Sph*)
ext 024F17D4 f32 eyeGetWaterHeight(cXyz*)
ext 025626A4 void eyeSetLighting(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void eyeModelLighting(void*, J3DModel*, dKy_tevstr_c*)
ext 02555D0C void* eyeGetLighting()
ext 025E2DE0 void eyeModelUpdate(J3DModel*, s32)
ext 0200F428 void eyeAddAngle(be<s16>*, s16, s16, s16)
ext 02018C8C void eyeSetSphereRadius(cM3dGSph*, f32)
ext 02018D40 void eyeSetSphereCenter(cM3dGSph*, cXyz*)
*/
