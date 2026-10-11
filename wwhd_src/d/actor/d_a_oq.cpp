/**
 * d_a_oq.cpp (WWHD)
 * Enemy - Octorok
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_oq.cpp) has only "Nonmatching" stubs for this actor (and no class layout):
 * the functions here are written from the WWHD code (cking.rpx), with the GameCube names and
 * signatures, and verified against it. daOQ_Execute (with action_dousa, action_kougeki,
 * action_tama_shoot, action_itai, moguru_check and Line_check inlined) is in d_a_oq_exec.cpp.
 */
#include "d/actor/d_a_oq.h"
#include "gc_api/d/actor/d_a_player.h"

static u32 oqRegistration[4] = {0, 0, 0, 0};
static f32 oqAngleMin;
static f32 oqAngleMax;
static u8 oqAngleStorage;
static u8 oqFlagStorage;
static u8 oqGlobalRecord1;
static u8 oqGlobalRecord2;
static u8 oqGlobalRecord3;
static dCcD_SrcSph tama_at_co_sph_src;
static dCcD_SrcSph tama_tg_co_sph_src;
static dCcD_SrcCyl body_co_cyl_src;
static dCcD_SrcCyl body_at_cyl_src;
static const s8 oqFlameJoints[10] = {3, 4, 5, 6, 7, 8, 10, 14, 15, 19};
static const f32 oqFlameScales[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static s16 oqSpawnDelay;
static csXyz oqSpins[4] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

/* 023C003C */
static BOOL nodeCallBack(J3DNode* i_node, int calcTiming) {

    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        J3DModel* model = j3dSys.getModel();
        oq_class* i_this = (oq_class*)model->getUserArea();
        s32 jntNo = ((OqJointView*)joint)->mJntNo;
        if (i_this != nullptr && (jntNo == 7 || jntNo == 8)) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == 8) {
                cXyz offset;
                offset.set(40.0f, 0.0f, 0.0f);
                MtxPosition(&offset, &i_this->mMouthEffectPos);
                offset.y = 0.0f;
                offset.z = 0.0f;
                offset.x = 20.0f;
                MtxPosition(&offset, &i_this->mMouthPos);
                mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            } else if (jntNo == 7) {
                mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
            }
        }
    }
    return TRUE;
}

static inline bool oq_is_octorok(u8 t) { return t <= 1 || (t >= 4 && t <= 5); }

/* 023C024C */
void draw_SUB(oq_class* i_this) {

    J3DModel* model = i_this->mpMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), i_this->current.pos.x, i_this->current.pos.y + i_this->mDrawOffsetY, i_this->current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    if (oq_is_octorok(i_this->mType)) {
        i_this->mpMorf->calc();
        enemy_fire(&i_this->mEnemyFire);
    }
    if (i_this->mType != OQ_SPAWNER && i_this->mType != OQ_PLACE) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    }
}

/* 023C039C */
static BOOL daOQ_Draw(oq_class* i_this) {

    if (i_this->mType == OQ_SPAWNER || i_this->mType == OQ_PLACE) {
        return TRUE;
    }
    J3DModel* model = i_this->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    if (i_this->mType == OQ_LARGE || i_this->mType == OQ_DELAYED || i_this->mType == OQ_CHILD) {
        dSnap_RegistFig(DSNAP_TYPE_OQ, i_this, 2.75f, 2.75f, 2.75f);
    }
    if (i_this->mType == OQ_SMALL) {
        dSnap_RegistFig(DSNAP_TYPE_OQ, i_this, 1.0f, 1.0f, 1.0f);
    }
    u8 t = i_this->mType;
    if ((t == OQ_SMALL || t == OQ_LARGE || t == OQ_DELAYED || t == OQ_CHILD) && i_this->mEnemyIce.mFreezeTimer > 0x14) {
        /* frozen: drawn as ice */
        mDoExt_invisibleModel_entryDL(i_this->mpMorf, -1, nullptr);
        return TRUE;
    }
    dComIfGp_get();
    t = i_this->mType;
    if (t >= 1 && (t == OQ_LARGE || (t >= 4 && t <= 5))) {
        mDoExt_baseAnm_oq* brk = i_this->mpBrk;
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)brk, J3DModel_getModelData(model), brk->mFrameCtrl.mFrame);
        i_this->mpBrk->mFrameCtrl.mFrame = (f32)i_this->mBrkFrame;
    } else if (t != OQ_SMALL) {
        if (t == OQ_ROCK) {
            i_this->mpMorf->updateDL();
        }
        return TRUE;
    }
    i_this->mpMorf->entryDL();
    if (i_this->mType != OQ_SMALL) {
        oqModelData(model->getModelData()).field_0x48 = 0;
    }
    return TRUE;
}

/* 023C0520 */
void anm_init(oq_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx) {

    i_this->mAnmIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Oq" /* "Oq" */, bckFileIdx, SAFESTRING_VTBL_OQ);
        void* sound = dComIfG_getObjectRes("Oq", soundFileIdx, SAFESTRING_VTBL_OQ);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Oq", bckFileIdx, SAFESTRING_VTBL_OQ);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}

/* the splash at pos (by value: the caller's copy is updated) */
/* 023C0FDC */
void shibuki_set(oq_class* i_this, cXyz* pos, float scale) {

    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        pos->y = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
    } else if (i_this->mAcch.ChkWaterIn()) {
        pos->y = oq_wtr_height(i_this);
    }
    fopKyM_createWpillar(pos, scale, 1.0f, 0);
}

/* on the sea or in water: bob on the surface, splash once */
/* 023C1080 */
BOOL sea_water_check(oq_class* i_this) {

    i_this->mWaterPos.y = (f32)i_this->mHomePos.y; /* mWaterPos.y = mHomePos.y */
    oqPositionWords(i_this->mWaterPos).z = (u32)oqPositionWords(i_this->current.pos).z;
    oqPositionWords(i_this->mWaterPos).x = (u32)oqPositionWords(i_this->current.pos).x;
    i_this->gravity = -3.0f;
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
        i_this->mWaterPos.y = h;
        if (!(i_this->current.pos.y < h + 40.0f)) {
            return FALSE;
        }
        i_this->mBobPhase += 0x800;
        i_this->gravity = 0.0f;
        cLib_addCalc2(&i_this->current.pos.y, h - gabi::fmadds(cM_ssin(i_this->mBobPhase), 15.0f, 45.0f), 1.0f, 30.0f);
    } else if (i_this->mAcch.ChkWaterIn()) {
        f32 h = oq_wtr_height(i_this);
        i_this->mWaterPos.y = h;
        i_this->gravity = -3.0f;
        if (!(i_this->current.pos.y < h + 20.0f)) {
            return TRUE;
        }
        i_this->mBobPhase += 0x800;
        i_this->gravity = 0.0f;
        cLib_addCalc2(&i_this->current.pos.y, (h + 20.0f) - gabi::fmadds(cM_ssin(i_this->mBobPhase), 15.0f, 45.0f), 1.0f, 30.0f);
    } else {
        if (i_this->mType == OQ_LARGE || i_this->mType == OQ_DELAYED || i_this->mType == OQ_CHILD) {
            i_this->gravity = 0.0f;
        }
        return FALSE;
    }
    if (i_this->mSplashDone == 0) {
        if (i_this->mType != OQ_CHILD) {
            cXyz scale;
            scale.set(1.0f, 1.0f, 1.0f);
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
            /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &mWaterPos, NULL, &scale, 0xFF, &mRipple) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->mWaterPos, nullptr, &scale, 0xFF,
                            (dPa_levelEcallBack*)&i_this->mRipple, -1, nullptr, nullptr, nullptr);
            i_this->mRipple.mRate = 0.0f;
        }
        cXyz pos;
        pos.x = (f32)i_this->current.pos.x;
        pos.y = (f32)i_this->current.pos.y;
        pos.z = (f32)i_this->current.pos.z;
        i_this->mSplashDone = 1;
        shibuki_set(i_this, &pos, i_this->mSplashScale);
    }
    return TRUE;
}

/* 023C1324 */
void search_y_check(oq_class* i_this, short step) {

    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mTargetAngleY, 1, step);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, step);
}

/* 023C38A4 */
static BOOL daOQ_IsDelete(oq_class*) {

    return TRUE;
}

/* 023C38AC */
static BOOL daOQ_Delete(oq_class* i_this) {

    dComIfG_resDelete(&i_this->mPhase, "Oq" /* "Oq" */);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
    i_this->mFollow.remove();
    if (oq_is_octorok(i_this->mType)) {
        enemy_fire_remove(&i_this->mEnemyFire);
    }
    return TRUE;
}

/* 023C3928 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {

    oq_class* i_this = (oq_class*)a_this;
    if (i_this->mType == OQ_ROCK) {
        /* the rock */
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Oq" /* "Oq" */, dRes_INDEX_OQ_BDL_ISI_e, SAFESTRING_VTBL_OQ);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, 1, 0.0f, 0, -1, 1, nullptr, 0,
                                                0x11020203);
        if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
            return FALSE;
        }
    } else {
        J3DModelData* modelData;
        if (i_this->mType == OQ_SMALL) {
            modelData = (J3DModelData*)dComIfG_getObjectRes("Oq", dRes_INDEX_OQ_BMD_OQ_e, SAFESTRING_VTBL_OQ);
        } else {
            modelData = (J3DModelData*)dComIfG_getObjectRes("Oq", dRes_INDEX_OQ_BMD_RED_OQ_e, SAFESTRING_VTBL_OQ);
        }
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Oq", dRes_INDEX_OQ_BCK_START_e, SAFESTRING_VTBL_OQ);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 1, 1.0f, 0, -1, 1, nullptr, 0,
                                                0x11020203);
        if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
            return FALSE;
        }
        i_this->mpMorf->getModel()->setUserArea((uintptr_t)i_this);
        for (u16 i = 0; i < i_this->mpMorf->getModel()->getModelData()->getJointNum(); i++) {
            i_this->mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack((J3DJointCallBack)&nodeCallBack);
        }
        if (oq_is_octorok(i_this->mType)) {
            J3DModel* model = i_this->mpMorf->getModel();
            void* p = operator_new(0x78);
            if (p != nullptr) p = mDoExt_brkAnm_ct(p);
            i_this->mpBrk = (mDoExt_baseAnm_oq*)p;
            if (p == nullptr) {
                return FALSE;
            }
            void* brk = dComIfG_getObjectRes("Oq", dRes_INDEX_OQ_BRK_RED_OQ_e, SAFESTRING_VTBL_OQ);
            if (!mDoExt_brkAnm_init(i_this->mpBrk.get(), J3DModel_getModelData(model), brk, 1, 0, 1.0f, 0, -1, false, 0)) {
                return FALSE;
            }
        }
    }
    if (oq_is_octorok(i_this->mType)) {
        if (!mDoExt_invisibleModel_create(i_this->mInvisibleModel, i_this->mpMorf->getModel())) {
            return FALSE;
        }
    }
    if (oq_is_octorok(i_this->mType)) {
        u32 jntHit = JntHit_create(i_this->mpMorf->getModel(), OQ_JOINT_HIT_SOURCE, 3);
        i_this->mpJntHit = jntHit;
        if (jntHit == 0) {
            return FALSE;
        }
        i_this->jntHit = jntHit;
        return TRUE;
    }
    return TRUE;
}

/* enemyfire::enemyfire (inline constructor, this TU's copy) */
/* 023C3C80 */
static enemyfire* enemyfire_ct(enemyfire* self) {

    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    oqSphereCtor(&self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->m228 = 1.0f;
    return self;
}

/* oq_class::oq_class (HD: allocates when this == NULL) */
/* 023C3D0C */
static oq_class* oq_class_ct(oq_class* self) {

    if (self == nullptr) {
        self = (oq_class*)operator_new(sizeof(oq_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = OQ_VTBL;
    oqRippleCtor(&self->mRipple); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dPa_followEcallBack_ct(&self->mFollow, 0, 0);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, OQ_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mBodyCoCyl, OQ_AAB_VTBL);
    dCcD_Cyl_ct(&self->mBodyAtCyl, OQ_AAB_VTBL);
    oqSphereCtor(&self->mTamaAtSph); /* dCcD_Sph::dCcD_Sph */
    oqSphereCtor(&self->mTamaTgSph);
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, OQ_AAB_VTBL);
    dBgS_AcchCir_ct(&self->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&self->mEnemyIce.mBgAcch, OQ_OBJACCH_VT);
    enemyfire_ct(&self->mEnemyFire);
    oqInvisibleCtor(self->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    return self;
}

/* static initialisation of the translation unit */
/* 023C4720 */
static void __sinit_d_a_oq_cpp() {

    for (int i = 0; i < 4; i++) oqRegistration[i] = 0;
    __register_global_object(gabi::ea(&oqGlobalRecord1));
    oqAngleMin = -3.1415927f;
    oqAngleMax = 3.1415927f;
    oqAngleInit(gabi::ea(&oqAngleStorage));
    __register_global_object(gabi::ea(&oqGlobalRecord2));
    oqFlagInit(gabi::ea(&oqFlagStorage));
    __register_global_object(gabi::ea(&oqGlobalRecord3));
    /* four angles (0, +-0x1000, +-0x1000) */
    csXyz_ct(&oqSpins[0], 0, 0x1000, 0x1000);
    csXyz_ct(&oqSpins[1], 0, 0x1000, -0x1000);
    csXyz_ct(&oqSpins[2], 0, -0x1000, 0x1000);
    csXyz_ct(&oqSpins[3], 0, -0x1000, -0x1000);
}

/* sead::SafeString deleting destructor (this TU's copy) */
/* 023C481C */
static void SafeString_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1)) operator_delete(p);
}

static inline void oq_crr_pos(oq_class* i_this) {
    f32 o = i_this->mCollisionHeightOffset;
    i_this->current.pos.y = i_this->current.pos.y - o;
    i_this->old.pos.y = i_this->old.pos.y - o;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    o = i_this->mCollisionHeightOffset;
    i_this->current.pos.y = i_this->current.pos.y + o;
    i_this->old.pos.y = i_this->old.pos.y + o;
}

/* 023C4830 */
void BG_check(oq_class* i_this) {

    bool crr = false;
    if (i_this->mType == OQ_SMALL) {
        i_this->mAcchCir.SetWall(REG_F(8, 10) + 80.0f, REG_F(8, 11) + 100.0f);
        crr = true;
    }
    if (i_this->mType == OQ_LARGE || i_this->mType == OQ_DELAYED || i_this->mType == OQ_CHILD) {
        i_this->mAcchCir.SetWall(REG_F(8, 10) + 40.0f, REG_F(8, 11) + 90.0f);
        return;
    }
    if (i_this->mType == OQ_ROCK) {
        i_this->mAcchCir.SetWall(40.0f, 30.0f);
        oq_crr_pos(i_this);
        return;
    }
    if (crr) {
        oq_crr_pos(i_this);
    }
}

/* oq_class deleting destructor (HD virtual destructor) */
/* 023C4F24 */
static void oq_class_dt(oq_class* self, s32 flags) {

    if (self == nullptr) return;
    oqInvisibleDtor(self->mInvisibleModel, 2);        /* ~mDoExt_invisibleModel */
    oqSphereDtor(&self->mEnemyFire.mSph, 2);       /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    oqPrepareAcchDtor(self->mEnemyIce.mBgAcch);
    oqAcchDtor(gabi::ea(&self->mEnemyIce.mBgAcch), 0);                          /* dBgS_Acch::~dBgS_Acch */
    oqCircleDtor(gabi::ea(&self->mEnemyIce.mBgAcchCir.m_cir), 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    oqSphereDtor(&self->mTamaTgSph, 2);
    oqSphereDtor(&self->mTamaAtSph, 2);
    dCcD_Cyl_dt(&self->mBodyAtCyl, 2);
    dCcD_Cyl_dt(&self->mBodyCoCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    oqPrepareAcchDtor(self->mAcch);
    oqAcchDtor(gabi::ea(&self->mAcch), 0);
    oqCircleDtor(gabi::ea(&self->mAcchCir.m_cir), 2);
    oqActorDtor(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}

/* empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable 0x10033D58 +0x14) */
/* 023C5050 */
static void SafeString_assureTermination(void*) {

}

/* the hit sparks at the collider's hit position */
/* inlined into body_atari_check */
static inline void oq_hit_effect(oq_class* i_this, fopAc_ac_c* player) {
    cXyz pos;
    cXyz* hit = i_this->mBodyCoCyl.GetTgHitPosP();
    pos.x = (f32)hit->x;
    pos.y = (f32)hit->y;
    pos.z = (f32)hit->z;
    dPa_control_set(dComIfGp_getParticle(), 0, 0x10, &pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    cXyz scale;
    scale.set(2.0f, 2.0f, 2.0f);
    dPa_control_set(dComIfGp_getParticle(), 0, 0xF, &pos, &player->shape_angle, &scale, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 023C064C */
BOOL body_atari_check(oq_class* i_this) {

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mStts.Move();
    u8 t = i_this->mType;
    if (t == OQ_LARGE || t == OQ_DELAYED || t == OQ_CHILD) {
        if (i_this->mAnmIdx == dRes_INDEX_OQ_BCK_SHIP_HIT_e) {
            if (i_this->mpMorf->isStop() && i_this->mMode != 0xA && i_this->mMode != 0xB) {
                i_this->mAction = OQ_ACTION_DOUSA;
                i_this->mMode = 0xA;
            }
        } else if (i_this->mBodyAtCyl.ChkAtHit()) {
            /* the bite hit the player or the ship */
            fopAc_ac_c* hit = dCcD_GAtTgCoCommonBase_GetAc(&i_this->mBodyAtCyl.mGObjAt);
            if (hit != nullptr) {
                fopAc_ac_c* ship = dComIfGp_getPlayer(2); /* dComIfGp_getPlayer(2) */
                if ((hit == ship || hit == player) && i_this->mAnmIdx != dRes_INDEX_OQ_BCK_SHIP_HIT_e) {
                    i_this->mCollisionRadius = REG_F(8, 0x11) + 300.0f;
                    anm_init(i_this, dRes_INDEX_OQ_BCK_SHIP_HIT_e, 0.0f, 0, 1.0f, -1);
                }
            }
        }
        cLib_addCalc2(&i_this->mCollisionRadius, REG_F(8, 12) + 175.0f, 1.0f, 5.0f);
    }

    i_this->mDamageType = 0;
    if (!i_this->mBodyCoCyl.ChkTgHit()) {
        return FALSE;
    }
    void* obj = i_this->mBodyCoCyl.GetTgHitObj();
    if (obj == nullptr) {
        return FALSE;
    }
    CcAtInfo_oq atInfo;
    atInfo.pParticlePos = 0;
    switch (((OqAttackView*)obj)->mType /* At type */) {
    case 2: { /* sword */
        if (i_this != nullptr) oq_se(i_this, 0x2803, 0x20);
        i_this->mDamageType = 0;
        u8 cut = oqPlayerCutType(player); /* the player's cut type */
        if (cut >= 0x15) {
            if (cut == 0x15 || cut == 0x17 || (cut >= 0x19 && cut <= 0x1B) || (cut >= 0x1E && cut <= 0x1F)) {
                i_this->mDamageType = 1;
            }
        } else if (cut >= 5) {
            if (cut <= 0xA || cut == 0xC || (cut >= 0xE && cut <= 0x10)) {
                i_this->mDamageType = 1;
            }
        }
        oq_hit_effect(i_this, player);
        break;
    }
    case 0x20: /* bomb */
        i_this->mDamageType = 6;
        oq_hit_effect(i_this, player);
        break;
    case 0x40: /* boomerang */
        i_this->mDamageType = 4;
        /* fallthrough */
    case 0x80:
        if (i_this != nullptr) oq_se(i_this, 0x2833, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x10000: /* skull hammer */
        if (i_this != nullptr) oq_se(i_this, 0x2855, 0x20);
        i_this->mDamageType = 7;
        if (oqPlayerCutType(player) == 0x11) {
            i_this->mDamageType = 8;
        }
        oq_hit_effect(i_this, player);
        break;
    case 0x200:
    case 0x40000: /* fire */
        i_this->mEnemyFire.mFireDuration = 0x64;
        i_this->mDamageType = 5;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x80000: /* ice arrow */
        i_this->mEnemyIce.mFreezeDuration = 0xC8;
        i_this->mDamageType = 5;
        attn_flags_oq(i_this) = 0;
        oq_se(i_this, 0x2834, 0x20);
        return TRUE;
    case 0x100000: /* light arrow */
        i_this->mEnemyIce.mParticleScale = 1.0f;
        i_this->mEnemyIce.mLightShrinkTimer = 1;
        i_this->mEnemyIce.mYOffset = 80.0f;
        attn_flags_oq(i_this) = 0;
        /* fallthrough */
    case 0x4000:
        i_this->mDamageType = 5;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x200000: /* wind */
        i_this->mDamageType = 3;
        return TRUE;
    case 0x8000000: /* hookshot: steals an item */
        if (i_this->stealItemLeft > 0) {
            s8 health = i_this->health;
            i_this->health = 10;
            atInfo.mpObj = gabi::ea(i_this->mBodyCoCyl.GetTgHitObj());
            cc_at_check(i_this, &atInfo);
            i_this->health = health;
        }
        dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, &i_this->attention_info.position /* attention_info.position */,
                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        oq_se(i_this, 0x2834, 0x20);
        return TRUE;
    default:
        i_this->mDamageType = 0;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    }
    if (i_this != nullptr) oq_mons_se(i_this, 0x48CC);
    CcAtInfo_oq atInfo2;
    atInfo2.pParticlePos = 0;
    atInfo2.mpObj = gabi::ea(i_this->mBodyCoCyl.GetTgHitObj());
    cc_at_check(i_this, &atInfo2);
    i_this->mAction = OQ_ACTION_ITAI;
    i_this->mMode = 0x1E;
    return TRUE;
}

/* the spawner (types 2 and 3) */
/* 023C49B4 */
void action_wakidasi(oq_class* i_this) {

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((f32)i_this->mKillCount > REG_F(8, 0) + 16.0f) {
        fopAcM_delete(i_this);
        return;
    }
    switch (i_this->mMode) {
    case 0x32:
        for (int i = 0; i < 6; i++) {
            i_this->mActionFlags[i] = 0;
        }
        for (int i = 0; i < 6; i++) {
            i_this->mChildId[i] = fpcM_ERROR_PROCESS_ID_e;
        }
        i_this->actor_status &= ~0x100u;
        i_this->mMode += 1;
        /* fallthrough */
    case 0x33:
        if (fopAcM_searchPlayerDistance(i_this) < i_this->mRange) {
            i_this->mWaveCount = 0;
            i_this->mTimer[0] = 0;
            i_this->mMode += 1;
        }
        return;
    case 0x34: {
        if (i_this->mTimer[0] != 0 || i_this->mTimer[1] != 0) {
            return;
        }
        if (fopAcM_searchPlayerDistance(i_this) > i_this->mRange + 500.0f) {
            i_this->mMode = 0x33;
            return;
        }
        if (i_this->mWaveCount == 0) {
            s16 n = (s16)gabi::ftoi(REG_F(12, 4) + 2.0f);
            i_this->mWaveCount = n;
            i_this->mWaveCount = n + (s16)gabi::ftoi(cM_rndF(REG_F(12, 5) + 1.99f));
        }
        /* drop the children that are gone, keeping the order */
        gabi::Local<be<u32>[6]> ids;
        int n = 0;
        for (int i = 0; i < 6; i++) {
            (*ids)[i] = fpcM_ERROR_PROCESS_ID_e;
            u32 id = i_this->mChildId[i];
            if (id != fpcM_ERROR_PROCESS_ID_e) {
                if (fopAcM_SearchByID(id) == nullptr) {
                    i_this->mChildId[i] = fpcM_ERROR_PROCESS_ID_e;
                } else {
                    (*ids)[n++] = (u32)i_this->mChildId[i];
                }
            }
        }
        for (int i = 0; i < 6; i++) {
            i_this->mChildId[i] = (u32)(*ids)[i];
        }
        i_this->mMode += 1;
        return;
    }
    case 0x35: {
        s16 base = player->shape_angle.y;
        cMtx_YrotS(calc_mtx(), (s16)(base + (s16)gabi::ftoi(cM_rndFX(REG_F(12, 1) + 7000.0f))));
        cXyz offset;
        f32 r = REG_F(12, 2) + 3500.0f;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = r + cM_rndF(REG_F(12, 3) + 2500.0f);
        cXyz pos;
        MtxPosition(&offset, &pos);
        PSVECAdd(&pos, &player->current.pos, &pos);
        pos.y = pos.y - (REG_F(12, 9) + 100.0f);
        f32 dx = pos.x - i_this->current.pos.x;
        f32 dz = pos.z - i_this->current.pos.z;
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > i_this->mRange)) {
            int i;
            for (i = 0; i < 6; i++) {
                if (i_this->mChildId[i] == fpcM_ERROR_PROCESS_ID_e) {
                    u32 id = fopAcM_createChild(PROC_OQ, fopAcM_GetID(i_this), 0x105, &pos, fopAcM_GetRoomNo(i_this),
                                                &i_this->current.angle, nullptr, -1, 0);
                    if (id == fpcM_ERROR_PROCESS_ID_e) {
                        break;
                    }
                    i_this->mChildId[i] = id;
                    i_this->mWaveCount -= 1;
                    if (i_this->mWaveCount != 0) {
                        break;
                    }
                    s16 t = (s16)gabi::ftoi(REG_F(12, 7) + 90.0f);
                    i_this->mTimer[0] = t;
                    i_this->mTimer[0] = t + (s16)gabi::ftoi(cM_rndF(REG_F(12, 8) + 50.0f));
                    i_this->mMode = 0x34;
                    i_this->mTimer[1] = REG_S(8, 3) + 100;
                    return;
                }
            }
            if (i == 6) {
                /* all six out: the first one is told so */
                fopAc_ac_c* child = fopAcM_SearchByID(i_this->mChildId[0]);
                if (child != nullptr) {
                    ((oq_class*)child)->mWaveFull = 1;
                }
            }
        }
        i_this->mMode = 0x34;
        i_this->mTimer[1] = REG_S(8, 3) + 100;
        return;
    }
    }
}

/* 023C3EC8 */
static cPhs_State daOQ_Create(fopAc_ac_c* a_this) {

    oq_class* i_this = (oq_class*)a_this;
    i_this->mType = fopAcM_GetParam(a_this);
    i_this->mProjectileType = fopAcM_GetParam(a_this) >> 8;
    i_this->mRange = (f32)((fopAcM_GetParam(a_this) >> 16) & 0xFF);
    u32 heapSize = 0x3C80;
    /* fopAcM_ct(a_this, oq_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) oq_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    // HD: not in the boss room of this stage once the boss is beaten
    if (dComIfGs_isStageBossEnemy(4) && fopAcM_GetRoomNo(a_this) == 0x29 && oq_isStartStage(OQ_BOSS_STAGE_NAME)) {
        return cPhs_ERROR_e;
    }
    if (i_this->mType == 0xFF) {
        i_this->mType = 0;
    }
    f32 range = i_this->mRange;
    if (range == 255.0f || range == 0.0f) {
        i_this->mRange = 80000.0f;
    } else {
        i_this->mRange = range * 1000.0f;
    }
    if (i_this->mType == OQ_SPAWNER) {
        a_this->group = 0;
        i_this->mAction = OQ_ACTION_WAKIDASI;
        i_this->mMode = 0x32;
        return cPhs_COMPLEATE_e;
    }

    cPhs_State status = dComIfG_resLoad(&i_this->mPhase, "Oq" /* "Oq" */);
    if (status != cPhs_COMPLEATE_e) {
        return status;
    }
    if (i_this->mType == OQ_SMALL) {
        heapSize = 0x2E00;
    } else if (i_this->mType == OQ_ROCK) {
        heapSize = 0x9E0;
    }
    if (i_this->mProjectileType == 0xFF || i_this->mProjectileType > 1) {
        i_this->mProjectileType = 0;
    }
    if (i_this->mType != OQ_SPAWNER) {
        /* type 2 places an octorok (type 4) 1000 in front of the player and goes */
        auto spawn = [&]() -> cPhs_State {
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            oqSpawnDelay = 0x64;
            cMtx_YrotS(calc_mtx(), 0);
            cXyz offset;
            offset.set(0.0f, 0.0f, 1000.0f);
            cXyz pos;
            MtxPosition(&offset, &pos);
            PSVECAdd(&pos, &player->current.pos, &pos);
            pos.y = pos.y - 40.0f;
            pos.x = pos.x + cM_rndFX(200.0f);
            pos.z = pos.z + cM_rndFX(200.0f);
            csXyz angle;
            angle.x = (s16)a_this->current.angle.x;
            angle.y = (s16)a_this->current.angle.y;
            angle.z = (s16)a_this->current.angle.z;
            angle.y = cM_atan2s(pos.x - player->current.pos.x, pos.z - player->current.pos.z);
            fopAcM_create(PROC_OQ, 0x104, &pos, fopAcM_GetRoomNo(a_this), &angle, &a_this->scale, 0, 0);
            return cPhs_ERROR_e;
        };
        if (i_this->mType == OQ_PLACE) {
            return spawn();
        }
        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void*)&useHeapInit), heapSize)) {
            return cPhs_ERROR_e;
        }
        if (i_this->mType == OQ_PLACE) {
            return spawn();
        }
    }

    a_this->max_health = 1;
    a_this->health = 1;
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    attn_flags_oq(a_this) = 0;
    i_this->mHomePos.copy(a_this->current.pos);
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
    i_this->mStts.Init(0xC8, 1, a_this);
    i_this->mCollisionHeightOffset = 50.0f;
    if (i_this->mType == OQ_ROCK) {
        if (i_this->mProjectileType == 0) {
            i_this->mTamaAtSph.Set(&tama_at_co_sph_src /* tama_at_co_sph_src */);
            i_this->mTamaAtSph.SetStts(&i_this->mStts);
            i_this->mTamaTgSph.Set(&tama_tg_co_sph_src /* tama_tg_co_sph_src */);
            i_this->mTamaTgSph.SetStts(&i_this->mStts);
            fopAcM_setCullSizeBox(a_this, -50.0f, -50.0f, -50.0f, 50.0f, 50.0f, 50.0f);
            a_this->speedF = 40.0f;
            i_this->mAction = OQ_ACTION_TAMA_SHOOT;
            i_this->mMode = 0x28;
        }
    } else {
        if (oq_is_octorok(i_this->mType)) {
            a_this->gbaName = 0x1B;
            i_this->mEnemyIce.mpActor = a_this;
            i_this->mEnemyIce.mWallRadius = 170.0f;
            i_this->mEnemyIce.mCylHeight = 80.0f;
            i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
            i_this->mEnemyFire.mpActor = a_this;
            for (int i = 0; i < 10; i++) {
                i_this->mEnemyFire.mFlameJntIdxs[i] = oqFlameJoints[i];
                i_this->mEnemyFire.mParticleScale[i] = oqFlameScales[i];
            }
        }
        if (i_this->mType == OQ_DELAYED) {
            i_this->mStts.SetWeight(0xFE);
            s16 d = oqSpawnDelay;
            i_this->mTimer[0] = d;
            i_this->mTimer[0] = d + (s16)gabi::ftoi(cM_rndF(50.0f));
            oqSpawnDelay = oqSpawnDelay + 100;
        }
        if (i_this->mType == OQ_LARGE || i_this->mType == OQ_DELAYED || i_this->mType == OQ_CHILD) {
            i_this->mProjectileType = 1;
            i_this->mCollisionRadius = REG_F(8, 12) + 175.0f;
            if (i_this->mType == OQ_CHILD) {
                i_this->mParentId = a_this->parentActorID;
                if (i_this->mParentId == fpcM_ERROR_PROCESS_ID_e) {
                    return cPhs_ERROR_e;
                }
                a_this->attention_info.distances[2] = 4; /* attention_info.distances[2] */
            }
        }
        if (REG_S(8, 9) != 0) {
            i_this->mProjectileType = i_this->mProjectileType ^ 1;
        }
        a_this->gravity = -3.0f;
        fopAcM_setCullSizeBox(a_this, -250.0f, 0.0f, -80.0f, 250.0f, 200.0f, 80.0f);
        i_this->mBodyCoCyl.Set(&body_co_cyl_src /* body_co_cyl_src */);
        i_this->mBodyCoCyl.SetStts(&i_this->mStts);
        i_this->mBodyAtCyl.Set(&body_at_cyl_src /* body_at_cyl_src */);
        i_this->mBodyAtCyl.SetStts(&i_this->mStts);
        i_this->mBodyCoCyl.OffTgSPrmBit(1);
        i_this->mBodyCoCyl.OffCoSPrmBit(1);
        i_this->mBodyCoCyl.ClrTgHit();
        i_this->mBodyAtCyl.OffAtSPrmBit(1);
        if (i_this->mType == OQ_SMALL) {
            i_this->mBodyCoCyl.mObjTg.mSPrm = (i_this->mBodyCoCyl.mObjTg.mSPrm & ~0xEu) | 2;
            a_this->stealItemLeft = 1;
            i_this->mAcch.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE;
            a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex_oq("Oq" /* "Oq" */, 0);
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            a_this->attention_info.distances[2] = 3;
        } else {
            a_this->gbaName = 0x41;
            a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex_oq("Oqw" /* "Oqw" */, 0);
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            a_this->attention_info.distances[2] = 0x2A;
        }
        i_this->mAction = OQ_ACTION_DOUSA;
        i_this->mMode = 0;
    }
    i_this->mTargetAngleY = a_this->current.angle.y;
    if (i_this->mType != OQ_SPAWNER && i_this->mType != OQ_PLACE) {
        draw_SUB(i_this);
    }
    return status;
}

/* WWHD glue
data oqRegistration 1046CB10 nocheck # BSS registration array
data oqAngleMin 1046CAEC
data oqAngleMax 1046CAF0
data oqAngleStorage 1046CAF4
data oqFlagStorage 1046CAF5
data oqGlobalRecord1 101CE468
data oqGlobalRecord2 101CE474
data oqGlobalRecord3 101CE480
data tama_at_co_sph_src 101CE32C
data tama_tg_co_sph_src 101CE36C
data body_co_cyl_src 101CE3E0
data body_at_cyl_src 101CE424
data oqFlameJoints 101CE3D4
data oqFlameScales 101CE3AC
data oqSpawnDelay 101CE2B4
data oqSpins 1046CAF8 nocheck # BSS angle array initialized by sinit
ext 025166F0 void oqSphereCtor(dCcD_Sph* sphere)
ext 025A9084 void oqRippleCtor(dPa_rippleEcallBack_oq* ripple)
ext 025E895C void oqInvisibleCtor(void* model)
ext 028ED6F8 void oqAngleInit(u32 address)
ext 028EAB2C void oqFlagInit(u32 address)
ext 025E89F8 void oqInvisibleDtor(void* model, s32 flags)
ext 02515AE8 void oqSphereDtor(dCcD_Sph* sphere, s32 flags)
ext 024EFD9C void oqAcchDtor(u32 acch, s32 flags)
ext 02018034 void oqCircleDtor(u32 circle, s32 flags)
ext 025D50BC void oqActorDtor(oq_class* actor, s32 flags)
str 10033E6C in 023C0520
str 10033F28 in 023C38AC
str 10033F2B in 023C3928
str 10033F54 in 023C3EC8
str 10033F58 in 023C3EC8
*/
