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

/* 023C003C */
static BOOL nodeCallBack(J3DNode* i_node, int calcTiming) {
    WWHD_FUNC(0x023C003C, BOOL, i_node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        J3DModel_oq* model = j3dSys_getModel();
        oq_class* i_this = gabi::at<oq_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr && (jntNo == 7 || jntNo == 8)) {
            PSMTXCopy(getAnmMtx((J3DModel*)model, jntNo), calc_mtx());
            if (jntNo == 8) {
                gabi::Local<cXyz> offset;
                offset->set(40.0f, 0.0f, 0.0f);
                MtxPosition(offset, &i_this->m450);
                offset->y = 0.0f;
                offset->z = 0.0f;
                offset->x = 20.0f;
                MtxPosition(offset, &i_this->m444);
                mtx_copy(getAnmMtx((J3DModel*)model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else if (jntNo == 7) {
                mtx_copy(getAnmMtx((J3DModel*)model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }
        }
    }
    return TRUE;
}
VERIFY(0x023C003C, nodeCallBack);

static inline bool oq_is_octorok(u8 t) { return t <= 1 || (t >= 4 && t <= 5); }

/* 023C024C */
void draw_SUB(oq_class* i_this) {
    WWHD_FUNC(0x023C024C, void, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), i_this->current.pos.x, i_this->current.pos.y + i_this->m43C, i_this->current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    if (oq_is_octorok(i_this->mType)) {
        i_this->mpMorf->calc();
        enemy_fire(&i_this->mEnemyFire);
    }
    if (i_this->mType != 3 && i_this->mType != 2) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    }
}
VERIFY(0x023C024C, draw_SUB);

/* 023C039C */
static BOOL daOQ_Draw(oq_class* i_this) {
    WWHD_FUNC(0x023C039C, BOOL, i_this);
    if (i_this->mType == 3 || i_this->mType == 2) {
        return TRUE;
    }
    J3DModel* model = i_this->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    if (i_this->mType == 1 || i_this->mType == 4 || i_this->mType == 5) {
        dSnap_RegistFig(DSNAP_TYPE_OQ, i_this, 2.75f, 2.75f, 2.75f);
    }
    if (i_this->mType == 0) {
        dSnap_RegistFig(DSNAP_TYPE_OQ, i_this, 1.0f, 1.0f, 1.0f);
    }
    u8 t = i_this->mType;
    if ((t == 0 || t == 1 || t == 4 || t == 5) && i_this->mEnemyIce.mFreezeTimer > 0x14) {
        /* frozen: drawn as ice */
        mDoExt_invisibleModel_entryDL(i_this->mpMorf, -1, nullptr);
        return TRUE;
    }
    dComIfGp_get();
    t = i_this->mType;
    if (t >= 1 && (t == 1 || (t >= 4 && t <= 5))) {
        mDoExt_baseAnm_oq* brk = i_this->mpBrk;
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)brk, J3DModel_getModelData(model), brk->mFrameCtrl.mFrame);
        i_this->mpBrk->mFrameCtrl.mFrame = (f32)i_this->m434;
    } else if (t != 0) {
        if (t == 6) {
            i_this->mpMorf->updateDL();
        }
        return TRUE;
    }
    i_this->mpMorf->entryDL();
    if (i_this->mType != 0) {
        gabi::store<u32>(gabi::ea(J3DModel_getModelData(model)) + 0x48, 0);
    }
    return TRUE;
}
VERIFY(0x023C039C, daOQ_Draw);

/* 023C0520 */
void anm_init(oq_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx) {
    WWHD_FUNC(0x023C0520, void, i_this, bckFileIdx, morf, loopMode, playSpeed, soundFileIdx);
    i_this->mAnmIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10033E6C) /* "Oq" */, bckFileIdx, SAFESTRING_VTBL_OQ);
        void* sound = dComIfG_getObjectRes(STR(0x10033E6C), soundFileIdx, SAFESTRING_VTBL_OQ);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10033E6C), bckFileIdx, SAFESTRING_VTBL_OQ);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x023C0520, anm_init);

/* 023C0FDC: the splash at pos (by value: the caller's copy is updated) */
void shibuki_set(oq_class* i_this, cXyz* pos, float scale) {
    WWHD_FUNC(0x023C0FDC, void, i_this, pos, scale);
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        pos->y = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
    } else if (i_this->mAcch.ChkWaterIn()) {
        pos->y = oq_wtr_height(i_this);
    }
    fopKyM_createWpillar(pos, scale, 1.0f, 0);
}
VERIFY(0x023C0FDC, shibuki_set);

/* 023C1080: on the sea or in water: bob on the surface, splash once */
BOOL sea_water_check(oq_class* i_this) {
    WWHD_FUNC(0x023C1080, BOOL, i_this);
    gabi::store<f32>(gabi::ea(&i_this->m498) + 4, gabi::load<f32>(gabi::ea(i_this) + 0x460)); /* m498.y = m45C.y */
    gabi::store<u32>(gabi::ea(&i_this->m498) + 8, gabi::load<u32>(gabi::ea(&i_this->current.pos) + 8));
    gabi::store<u32>(gabi::ea(&i_this->m498) + 0, gabi::load<u32>(gabi::ea(&i_this->current.pos) + 0));
    i_this->gravity = -3.0f;
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
        i_this->m498.y = h;
        if (!(i_this->current.pos.y < h + 40.0f)) {
            return FALSE;
        }
        i_this->m400 += 0x800;
        i_this->gravity = 0.0f;
        cLib_addCalc2(&i_this->current.pos.y, h - gabi::fmadds(cM_ssin(i_this->m400), 15.0f, 45.0f), 1.0f, 30.0f);
    } else if (i_this->mAcch.ChkWaterIn()) {
        f32 h = oq_wtr_height(i_this);
        i_this->m498.y = h;
        i_this->gravity = -3.0f;
        if (!(i_this->current.pos.y < h + 20.0f)) {
            return TRUE;
        }
        i_this->m400 += 0x800;
        i_this->gravity = 0.0f;
        cLib_addCalc2(&i_this->current.pos.y, (h + 20.0f) - gabi::fmadds(cM_ssin(i_this->m400), 15.0f, 45.0f), 1.0f, 30.0f);
    } else {
        if (i_this->mType == 1 || i_this->mType == 4 || i_this->mType == 5) {
            i_this->gravity = 0.0f;
        }
        return FALSE;
    }
    if (i_this->m3E3 == 0) {
        if (i_this->mType != 5) {
            gabi::Local<cXyz> scale;
            scale->set(1.0f, 1.0f, 1.0f);
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
            /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &m498, NULL, &scale, 0xFF, &mRipple) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->m498, nullptr, scale, 0xFF,
                            (dPa_levelEcallBack*)&i_this->mRipple, -1, nullptr, nullptr, nullptr);
            i_this->mRipple.mRate = 0.0f;
        }
        gabi::Local<cXyz> pos;
        pos->x = (f32)i_this->current.pos.x;
        pos->y = (f32)i_this->current.pos.y;
        pos->z = (f32)i_this->current.pos.z;
        i_this->m3E3 = 1;
        shibuki_set(i_this, pos, i_this->m438);
    }
    return TRUE;
}
VERIFY(0x023C1080, sea_water_check);

/* 023C1324 */
void search_y_check(oq_class* i_this, short step) {
    WWHD_FUNC(0x023C1324, void, i_this, step);
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m3FE, 1, step);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, step);
}
VERIFY(0x023C1324, search_y_check);

/* 023C38A4 */
static BOOL daOQ_IsDelete(oq_class*) {
    WWHD_FUNC(0x023C38A4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023C38A4, daOQ_IsDelete);

/* 023C38AC */
static BOOL daOQ_Delete(oq_class* i_this) {
    WWHD_FUNC(0x023C38AC, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10033F28) /* "Oq" */);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
    i_this->mFollow.remove();
    if (oq_is_octorok(i_this->mType)) {
        enemy_fire_remove(&i_this->mEnemyFire);
    }
    return TRUE;
}
VERIFY(0x023C38AC, daOQ_Delete);

/* 023C3928 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x023C3928, BOOL, a_this);
    oq_class* i_this = (oq_class*)a_this;
    if (i_this->mType == 6) {
        /* the rock */
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10033F2B) /* "Oq" */, 0x17, SAFESTRING_VTBL_OQ);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, 1, 0.0f, 0, -1, 1, nullptr, 0,
                                                0x11020203);
        if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
            return FALSE;
        }
    } else {
        J3DModelData* modelData;
        if (i_this->mType == 0) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10033F2B), 0x1A, SAFESTRING_VTBL_OQ);
        } else {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10033F2B), 0x1B, SAFESTRING_VTBL_OQ);
        }
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10033F2B), 0x13, SAFESTRING_VTBL_OQ);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 1, 1.0f, 0, -1, 1, nullptr, 0,
                                                0x11020203);
        if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
            return FALSE;
        }
        ((J3DModel_oq*)i_this->mpMorf->getModel())->mUserArea = gabi::ea(i_this);
        for (u16 i = 0; i < J3DModelData_getJointNum(((J3DModel_oq*)i_this->mpMorf->getModel())->mModelData); i++) {
            setJointCallBack(((J3DModel_oq*)i_this->mpMorf->getModel())->mModelData, i, 0x023C003C /* nodeCallBack */);
        }
        if (oq_is_octorok(i_this->mType)) {
            J3DModel* model = i_this->mpMorf->getModel();
            void* p = operator_new(0x78);
            if (p != nullptr) p = mDoExt_brkAnm_ct(p);
            i_this->mpBrk = (mDoExt_baseAnm_oq*)p;
            if (p == nullptr) {
                return FALSE;
            }
            void* brk = dComIfG_getObjectRes(STR(0x10033F2B), 0x1E, SAFESTRING_VTBL_OQ);
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
        u32 jntHit = JntHit_create(i_this->mpMorf->getModel(), 0x101CE2D8, 3);
        i_this->mpJntHit = jntHit;
        if (jntHit == 0) {
            return FALSE;
        }
        i_this->jntHit = jntHit;
        return TRUE;
    }
    return TRUE;
}
VERIFY(0x023C3928, useHeapInit);

/* 023C3C80: enemyfire::enemyfire (inline constructor, this TU's copy) */
static enemyfire* enemyfire_ct(enemyfire* self) {
    WWHD_FUNC(0x023C3C80, enemyfire*, self);
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->m228 = 1.0f;
    return self;
}
VERIFY(0x023C3C80, enemyfire_ct);

/* 023C3D0C: oq_class::oq_class (HD: allocates when this == NULL) */
static oq_class* oq_class_ct(oq_class* self) {
    WWHD_FUNC(0x023C3D0C, oq_class*, self);
    if (self == nullptr) {
        self = (oq_class*)operator_new(sizeof(oq_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = OQ_VTBL;
    gabi::call(0x025A9084, &self->mRipple); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dPa_followEcallBack_ct(&self->mFollow, 0, 0);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, OQ_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mBodyCoCyl, OQ_AAB_VTBL);
    dCcD_Cyl_ct(&self->mBodyAtCyl, OQ_AAB_VTBL);
    gabi::call(0x025166F0, &self->mTamaAtSph); /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &self->mTamaTgSph);
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, OQ_AAB_VTBL);
    dBgS_AcchCir_ct(&self->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&self->mEnemyIce.mBgAcch, OQ_OBJACCH_VT);
    enemyfire_ct(&self->mEnemyFire);
    gabi::call(0x025E895C, self->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    return self;
}
VERIFY(0x023C3D0C, oq_class_ct);

/* 023C4720: static initialisation of the translation unit */
static void __sinit_d_a_oq_cpp() {
    WWHD_FUNC(0x023C4720, void, (u32)0);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x1046CB10 + 4 * i, 0);
    __register_global_object(0x101CE468);
    gabi::store<f32>(0x1046CAEC, -3.1415927f);
    gabi::store<f32>(0x1046CAF0, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1046CAF4);
    __register_global_object(0x101CE474);
    gabi::call(0x028EAB2C, 0x1046CAF5);
    __register_global_object(0x101CE480);
    /* four angles (0, +-0x1000, +-0x1000) */
    csXyz_ct(gabi::at<csXyz>(0x1046CAF8), 0, 0x1000, 0x1000);
    csXyz_ct(gabi::at<csXyz>(0x1046CAFE), 0, 0x1000, -0x1000);
    csXyz_ct(gabi::at<csXyz>(0x1046CB04), 0, -0x1000, 0x1000);
    csXyz_ct(gabi::at<csXyz>(0x1046CB0A), 0, -0x1000, -0x1000);
}
VERIFY(0x023C4720, __sinit_d_a_oq_cpp);

/* 023C481C: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023C481C, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x023C481C, SafeString_dt);

static inline void oq_crr_pos(oq_class* i_this) {
    f32 o = i_this->m42C;
    i_this->current.pos.y = i_this->current.pos.y - o;
    i_this->old.pos.y = i_this->old.pos.y - o;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    o = i_this->m42C;
    i_this->current.pos.y = i_this->current.pos.y + o;
    i_this->old.pos.y = i_this->old.pos.y + o;
}

/* 023C4830 */
void BG_check(oq_class* i_this) {
    WWHD_FUNC(0x023C4830, void, i_this);
    bool crr = false;
    if (i_this->mType == 0) {
        i_this->mAcchCir.SetWall(REG_F(8, 10) + 80.0f, REG_F(8, 11) + 100.0f);
        crr = true;
    }
    if (i_this->mType == 1 || i_this->mType == 4 || i_this->mType == 5) {
        i_this->mAcchCir.SetWall(REG_F(8, 10) + 40.0f, REG_F(8, 11) + 90.0f);
        return;
    }
    if (i_this->mType == 6) {
        i_this->mAcchCir.SetWall(40.0f, 30.0f);
        oq_crr_pos(i_this);
        return;
    }
    if (crr) {
        oq_crr_pos(i_this);
    }
}
VERIFY(0x023C4830, BG_check);

/* 023C4F24: oq_class deleting destructor (HD virtual destructor) */
static void oq_class_dt(oq_class* self, s32 flags) {
    WWHD_FUNC(0x023C4F24, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x025E89F8, self->mInvisibleModel, 2);        /* ~mDoExt_invisibleModel */
    gabi::call(0x02515AE8, &self->mEnemyFire.mSph, 2);       /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    u32 acch = gabi::ea(&self->mEnemyIce.mBgAcch);            /* ~dBgS_ObjAcch (inline) */
    gabi::store<u32>(acch + 0x20, OQ_OBJACCH_VT.v20);
    gabi::store<u32>(acch + 0x14, OQ_OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, acch, 0);                          /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    gabi::call(0x02515AE8, &self->mTamaTgSph, 2);
    gabi::call(0x02515AE8, &self->mTamaAtSph, 2);
    dCcD_Cyl_dt(&self->mBodyAtCyl, 2);
    dCcD_Cyl_dt(&self->mBodyCoCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, OQ_OBJACCH_VT.v20);
    gabi::store<u32>(acch + 0x14, OQ_OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, acch, 0);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x023C4F24, oq_class_dt);

/* 023C5050: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable 0x10033D58 +0x14) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x023C5050, void, (u32)0);
}
VERIFY(0x023C5050, SafeString_assureTermination);

/* the hit sparks at the collider's hit position */
static inline void oq_hit_effect(oq_class* i_this, fopAc_ac_c* player) {
    gabi::Local<cXyz> pos;
    cXyz* hit = i_this->mBodyCoCyl.GetTgHitPosP();
    pos->x = (f32)hit->x;
    pos->y = (f32)hit->y;
    pos->z = (f32)hit->z;
    dPa_control_set(dComIfGp_getParticle(), 0, 0x10, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    gabi::Local<cXyz> scale;
    scale->set(2.0f, 2.0f, 2.0f);
    dPa_control_set(dComIfGp_getParticle(), 0, 0xF, pos, &player->shape_angle, scale, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 023C064C */
BOOL body_atari_check(oq_class* i_this) {
    WWHD_FUNC(0x023C064C, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mStts.Move();
    u8 t = i_this->mType;
    if (t == 1 || t == 4 || t == 5) {
        if (i_this->mAnmIdx == 0x12) {
            if (i_this->mpMorf->isStop() && i_this->m3E0 != 0xA && i_this->m3E0 != 0xB) {
                i_this->m3DF = 0xA;
                i_this->m3E0 = 0xA;
            }
        } else if (i_this->mBodyAtCyl.ChkAtHit()) {
            /* the bite hit the player or the ship */
            fopAc_ac_c* hit = dCcD_GAtTgCoCommonBase_GetAc(&i_this->mBodyAtCyl.mGObjAt);
            if (hit != nullptr) {
                u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getPlayer(2) */
                if ((gabi::ea(hit) == ship || hit == player) && i_this->mAnmIdx != 0x12) {
                    i_this->m428 = REG_F(8, 0x11) + 300.0f;
                    anm_init(i_this, 0x12, 0.0f, 0, 1.0f, -1);
                }
            }
        }
        cLib_addCalc2(&i_this->m428, REG_F(8, 12) + 175.0f, 1.0f, 5.0f);
    }

    i_this->m3E1 = 0;
    if (!i_this->mBodyCoCyl.ChkTgHit()) {
        return FALSE;
    }
    void* obj = i_this->mBodyCoCyl.GetTgHitObj();
    if (obj == nullptr) {
        return FALSE;
    }
    gabi::Local<CcAtInfo_oq> atInfo;
    atInfo->pParticlePos = 0;
    bool done = false;
    switch (gabi::load<u32>(gabi::ea(obj) + 0x10) /* At type */) {
    case 2: { /* sword */
        if (i_this != nullptr) oq_se(i_this, 0x2803, 0x20);
        i_this->m3E1 = 0;
        u8 cut = gabi::load<u8>(gabi::ea(player) + 0x3AC); /* the player's cut type */
        if (cut >= 0x15) {
            if (cut == 0x15 || cut == 0x17 || (cut >= 0x19 && cut <= 0x1B) || (cut >= 0x1E && cut <= 0x1F)) {
                i_this->m3E1 = 1;
            }
        } else if (cut >= 5) {
            if (cut <= 0xA || cut == 0xC || (cut >= 0xE && cut <= 0x10)) {
                i_this->m3E1 = 1;
            }
        }
        oq_hit_effect(i_this, player);
        break;
    }
    case 0x20: /* bomb */
        i_this->m3E1 = 6;
        oq_hit_effect(i_this, player);
        break;
    case 0x40: /* boomerang */
        i_this->m3E1 = 4;
        /* fallthrough */
    case 0x80:
        if (i_this != nullptr) oq_se(i_this, 0x2833, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x10000: /* skull hammer */
        if (i_this != nullptr) oq_se(i_this, 0x2855, 0x20);
        i_this->m3E1 = 7;
        if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0x11) {
            i_this->m3E1 = 8;
        }
        oq_hit_effect(i_this, player);
        break;
    case 0x200:
    case 0x40000: /* fire */
        i_this->mEnemyFire.mFireDuration = 0x64;
        i_this->m3E1 = 5;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x80000: /* ice arrow */
        i_this->mEnemyIce.mFreezeDuration = 0xC8;
        i_this->m3E1 = 5;
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
        i_this->m3E1 = 5;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    case 0x200000: /* wind */
        i_this->m3E1 = 3;
        return TRUE;
    case 0x8000000: /* hookshot: steals an item */
        if (i_this->stealItemLeft > 0) {
            s8 health = i_this->health;
            i_this->health = 10;
            atInfo->mpObj = gabi::ea(i_this->mBodyCoCyl.GetTgHitObj());
            cc_at_check(i_this, atInfo);
            i_this->health = health;
        }
        dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, gabi::at<cXyz>(gabi::ea(i_this) + 0x390) /* attention_info.position */,
                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        oq_se(i_this, 0x2834, 0x20);
        return TRUE;
    default:
        i_this->m3E1 = 0;
        if (i_this != nullptr) oq_se(i_this, 0x2834, 0x20);
        oq_hit_effect(i_this, player);
        break;
    }
    (void)done;
    if (i_this != nullptr) oq_mons_se(i_this, 0x48CC);
    gabi::Local<CcAtInfo_oq> atInfo2;
    atInfo2->pParticlePos = 0;
    atInfo2->mpObj = gabi::ea(i_this->mBodyCoCyl.GetTgHitObj());
    cc_at_check(i_this, atInfo2);
    i_this->m3DF = 0x14;
    i_this->m3E0 = 0x1E;
    return TRUE;
}
VERIFY(0x023C064C, body_atari_check);

/* 023C49B4: the spawner (types 2 and 3) */
void action_wakidasi(oq_class* i_this) {
    WWHD_FUNC(0x023C49B4, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if ((f32)i_this->m404 > REG_F(8, 0) + 16.0f) {
        fopAcM_delete(i_this);
        return;
    }
    switch (i_this->m3E0) {
    case 0x32:
        for (int i = 0; i < 6; i++) {
            i_this->m3F2[i] = 0;
        }
        for (int i = 0; i < 6; i++) {
            i_this->mChildId[i] = fpcM_ERROR_PROCESS_ID_e;
        }
        i_this->actor_status &= ~0x100u;
        i_this->m3E0 += 1;
        /* fallthrough */
    case 0x33:
        if (fopAcM_searchPlayerDistance(i_this) < i_this->m430) {
            i_this->m402 = 0;
            i_this->mTimer[0] = 0;
            i_this->m3E0 += 1;
        }
        return;
    case 0x34: {
        if (i_this->mTimer[0] != 0 || i_this->mTimer[1] != 0) {
            return;
        }
        if (fopAcM_searchPlayerDistance(i_this) > i_this->m430 + 500.0f) {
            i_this->m3E0 = 0x33;
            return;
        }
        if (i_this->m402 == 0) {
            s16 n = (s16)gabi::ftoi(REG_F(12, 4) + 2.0f);
            i_this->m402 = n;
            i_this->m402 = n + (s16)gabi::ftoi(cM_rndF(REG_F(12, 5) + 1.99f));
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
        i_this->m3E0 += 1;
        return;
    }
    case 0x35: {
        s16 base = player->shape_angle.y;
        cMtx_YrotS(calc_mtx(), (s16)(base + (s16)gabi::ftoi(cM_rndFX(REG_F(12, 1) + 7000.0f))));
        gabi::Local<cXyz> offset;
        f32 r = REG_F(12, 2) + 3500.0f;
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = r + cM_rndF(REG_F(12, 3) + 2500.0f);
        gabi::Local<cXyz> pos;
        MtxPosition(offset, pos);
        PSVECAdd(pos, &player->current.pos, pos);
        pos->y = pos->y - (REG_F(12, 9) + 100.0f);
        f32 dx = pos->x - i_this->current.pos.x;
        f32 dz = pos->z - i_this->current.pos.z;
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > i_this->m430)) {
            int i;
            for (i = 0; i < 6; i++) {
                if (i_this->mChildId[i] == fpcM_ERROR_PROCESS_ID_e) {
                    u32 id = fopAcM_createChild(PROC_OQ, fopAcM_GetID(i_this), 0x105, pos, fopAcM_GetRoomNo(i_this),
                                                &i_this->current.angle, nullptr, -1, 0);
                    if (id == fpcM_ERROR_PROCESS_ID_e) {
                        break;
                    }
                    i_this->mChildId[i] = id;
                    i_this->m402 -= 1;
                    if (i_this->m402 != 0) {
                        break;
                    }
                    s16 t = (s16)gabi::ftoi(REG_F(12, 7) + 90.0f);
                    i_this->mTimer[0] = t;
                    i_this->mTimer[0] = t + (s16)gabi::ftoi(cM_rndF(REG_F(12, 8) + 50.0f));
                    i_this->m3E0 = 0x34;
                    i_this->mTimer[1] = REG_S(8, 3) + 100;
                    return;
                }
            }
            if (i == 6) {
                /* all six out: the first one is told so */
                fopAc_ac_c* child = fopAcM_SearchByID(i_this->mChildId[0]);
                if (child != nullptr) {
                    ((oq_class*)child)->m3E4 = 1;
                }
            }
        }
        i_this->m3E0 = 0x34;
        i_this->mTimer[1] = REG_S(8, 3) + 100;
        return;
    }
    }
}
VERIFY(0x023C49B4, action_wakidasi);

/* 023C3EC8 */
static cPhs_State daOQ_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x023C3EC8, cPhs_State, a_this);
    oq_class* i_this = (oq_class*)a_this;
    i_this->mType = fopAcM_GetParam(a_this);
    i_this->m3DE = fopAcM_GetParam(a_this) >> 8;
    i_this->m430 = (f32)((fopAcM_GetParam(a_this) >> 16) & 0xFF);
    u32 heapSize = 0x3C80;
    /* fopAcM_ct(a_this, oq_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) oq_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    /* HD: not in the boss room of this stage once the boss is beaten */
    if (dComIfGs_isStageBossEnemy(4) && fopAcM_GetRoomNo(a_this) == 0x29 && oq_isStartStage(0x10033F50)) {
        return cPhs_ERROR_e;
    }
    if (i_this->mType == 0xFF) {
        i_this->mType = 0;
    }
    f32 range = i_this->m430;
    if (range == 255.0f || range == 0.0f) {
        i_this->m430 = 80000.0f;
    } else {
        i_this->m430 = range * 1000.0f;
    }
    if (i_this->mType == 3) {
        a_this->group = 0;
        i_this->m3DF = 0x64;
        i_this->m3E0 = 0x32;
        return cPhs_COMPLEATE_e;
    }

    cPhs_State status = dComIfG_resLoad(&i_this->mPhase, STR(0x10033F58) /* "Oq" */);
    if (status != cPhs_COMPLEATE_e) {
        return status;
    }
    if (i_this->mType == 0) {
        heapSize = 0x2E00;
    } else if (i_this->mType == 6) {
        heapSize = 0x9E0;
    }
    if (i_this->m3DE == 0xFF || i_this->m3DE > 1) {
        i_this->m3DE = 0;
    }
    if (i_this->mType != 3) {
        /* type 2 places an octorok (type 4) 1000 in front of the player and goes */
        auto spawn = [&]() -> cPhs_State {
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            oq_spawn_delay() = 0x64;
            cMtx_YrotS(calc_mtx(), 0);
            gabi::Local<cXyz> offset;
            offset->set(0.0f, 0.0f, 1000.0f);
            gabi::Local<cXyz> pos;
            MtxPosition(offset, pos);
            PSVECAdd(pos, &player->current.pos, pos);
            pos->y = pos->y - 40.0f;
            pos->x = pos->x + cM_rndFX(200.0f);
            pos->z = pos->z + cM_rndFX(200.0f);
            gabi::Local<csXyz> angle;
            angle->x = (s16)a_this->current.angle.x;
            angle->y = (s16)a_this->current.angle.y;
            angle->z = (s16)a_this->current.angle.z;
            angle->y = cM_atan2s(pos->x - player->current.pos.x, pos->z - player->current.pos.z);
            fopAcM_create(PROC_OQ, 0x104, pos, fopAcM_GetRoomNo(a_this), angle, &a_this->scale, 0, 0);
            return cPhs_ERROR_e;
        };
        if (i_this->mType == 2) {
            return spawn();
        }
        if (!fopAcM_entrySolidHeap(a_this, 0x023C3928 /* useHeapInit */, heapSize)) {
            return cPhs_ERROR_e;
        }
        if (i_this->mType == 2) {
            return spawn();
        }
    }

    a_this->max_health = 1;
    a_this->health = 1;
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    attn_flags_oq(a_this) = 0;
    i_this->m45C.copy(a_this->current.pos);
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
    i_this->mStts.Init(0xC8, 1, a_this);
    i_this->m42C = 50.0f;
    if (i_this->mType == 6) {
        if (i_this->m3DE == 0) {
            i_this->mTamaAtSph.Set(gabi::at<dCcD_SrcSph>(0x101CE32C) /* tama_at_co_sph_src */);
            i_this->mTamaAtSph.SetStts(&i_this->mStts);
            i_this->mTamaTgSph.Set(gabi::at<dCcD_SrcSph>(0x101CE36C) /* tama_tg_co_sph_src */);
            i_this->mTamaTgSph.SetStts(&i_this->mStts);
            fopAcM_setCullSizeBox(a_this, -50.0f, -50.0f, -50.0f, 50.0f, 50.0f, 50.0f);
            a_this->speedF = 40.0f;
            i_this->m3DF = 0x1E;
            i_this->m3E0 = 0x28;
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
                i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101CE3D4 + i);
                i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101CE3AC + 4 * i);
            }
        }
        if (i_this->mType == 4) {
            i_this->mStts.SetWeight(0xFE);
            s16 d = oq_spawn_delay();
            i_this->mTimer[0] = d;
            i_this->mTimer[0] = d + (s16)gabi::ftoi(cM_rndF(50.0f));
            oq_spawn_delay() = oq_spawn_delay() + 100;
        }
        if (i_this->mType == 1 || i_this->mType == 4 || i_this->mType == 5) {
            i_this->m3DE = 1;
            i_this->m428 = REG_F(8, 12) + 175.0f;
            if (i_this->mType == 5) {
                i_this->m440 = a_this->parentActorID;
                if (i_this->m440 == fpcM_ERROR_PROCESS_ID_e) {
                    return cPhs_ERROR_e;
                }
                gabi::store<u8>(gabi::ea(a_this) + 0x38A, 4); /* attention_info.distances[1] */
            }
        }
        if (REG_S(8, 9) != 0) {
            i_this->m3DE = i_this->m3DE ^ 1;
        }
        a_this->gravity = -3.0f;
        fopAcM_setCullSizeBox(a_this, -250.0f, 0.0f, -80.0f, 250.0f, 200.0f, 80.0f);
        i_this->mBodyCoCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CE3E0) /* body_co_cyl_src */);
        i_this->mBodyCoCyl.SetStts(&i_this->mStts);
        i_this->mBodyAtCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CE424) /* body_at_cyl_src */);
        i_this->mBodyAtCyl.SetStts(&i_this->mStts);
        i_this->mBodyCoCyl.OffTgSPrmBit(1);
        i_this->mBodyCoCyl.OffCoSPrmBit(1);
        i_this->mBodyCoCyl.ClrTgHit();
        i_this->mBodyAtCyl.OffAtSPrmBit(1);
        if (i_this->mType == 0) {
            i_this->mBodyCoCyl.mObjTg.mSPrm = (i_this->mBodyCoCyl.mObjTg.mSPrm & ~0xEu) | 2;
            a_this->stealItemLeft = 1;
            i_this->mAcch.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE;
            a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex_oq(STR(0x10033F58) /* "Oq" */, 0);
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            gabi::store<u8>(gabi::ea(a_this) + 0x38A, 3);
        } else {
            a_this->gbaName = 0x41;
            a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex_oq(STR(0x10033F54) /* "Oqw" */, 0);
            a_this->scale.set(0.0f, 0.0f, 0.0f);
            gabi::store<u8>(gabi::ea(a_this) + 0x38A, 0x2A);
        }
        i_this->m3DF = 0xA;
        i_this->m3E0 = 0;
    }
    i_this->m3FE = a_this->current.angle.y;
    if (i_this->mType != 3 && i_this->mType != 2) {
        draw_SUB(i_this);
    }
    return status;
}
VERIFY(0x023C3EC8, daOQ_Create);
