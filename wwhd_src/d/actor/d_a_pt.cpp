/**
 * d_a_pt.cpp (WWHD)
 * Enemy - Miniblin
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_pt.cpp) has only "Nonmatching" stubs for this actor: the functions here are
 * written from the WWHD code (cking.rpx), with the GameCube names and signatures, and verified
 * against it. action() (02447300, the AI with all its helpers inlined) is in d_a_pt_action.cpp.
 */
#include "d/actor/d_a_pt.h"

#define SAFESTRING_VTBL 0x100380AC /* this TU's sead::SafeString vtable */
#define PT_VTBL 0x10038234         /* pt_class vtable (HD virtual destructor) */
#define HIO_VTBL 0x10038224        /* daPt_HIO_c vtable */
#define AAB_VTBL 0x100380C4        /* this TU's cM3dGAab vtable */

#define cc_sph_src gabi::at<dCcD_SrcSph>(0x101CEF9C)
#define at_sph_src gabi::at<dCcD_SrcSph>(0x101CEFDC)

enum {
    dRes_INDEX_PT_BCK_WAIT_e = 0xC,   /* used by useHeapInit */
    dRes_INDEX_PT_BDL_PT_e = 0xF,
    dRes_INDEX_PT_BRK_e = 0x12,
    dRes_INDEX_PT_BTP_e = 0x15,
};
enum { PROC_PT = 0xF7, PROC_ESA = 0xDD };
enum { DSNAP_TYPE_PT = 0xAB };
enum { fopAc_Attn_LOCKON_BATTLE_e = 4 };

/* 02445DBC */
void anm_init(pt_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx) {
    WWHD_FUNC(0x02445DBC, void, i_this, bckFileIdx, morf, loopMode, playSpeed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1003824C) /* "Pt" */, bckFileIdx, SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes(STR(0x1003824C), soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1003824C), bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02445DBC, anm_init);

/* 02445EE4 */
static BOOL daPt_Draw(pt_class* i_this) {
    WWHD_FUNC(0x02445EE4, BOOL, i_this);
    if (i_this->mbHide == 0) {
        J3DModel* model = i_this->mpMorf->getModel();
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, model, &i_this->tevStr);
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)i_this->mpBtp.get(), getModelData(model),
                            (s16)gabi::ftoi(i_this->mpBtp->getFrame()));
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)i_this->mpBrk.get(), getModelData(model), i_this->mpBrk->getFrame());
        i_this->mpMorf->entryDL();
        i_this->mbBgCheck = 0;
    }
    dSnap_RegistFig(DSNAP_TYPE_PT, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02445EE4, daPt_Draw);

/* 02445F98 */
void smoke_set(pt_class* i_this, signed char type) {
    WWHD_FUNC(0x02445F98, void, i_this, type);
    if (i_this->mSmokeType != 0) {
        return;
    }
    dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    /* dComIfGp_particle_setToon(0x2022, &current.pos, &shape_angle, NULL, 0xB9, &mSmokeCb, roomNo) */
    JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &i_this->current.pos, &i_this->shape_angle,
                                              nullptr, 0xB9, (dPa_levelEcallBack*)&i_this->mSmokeCb, roomNo, nullptr,
                                              nullptr, nullptr);
    if (emitter != nullptr) {
        u32 e = gabi::ea(emitter);
        gabi::store<f32>(e + 0x58, 0.5f);  /* rate */
        gabi::store<f32>(e + 0x34, 3.0f);
        gabi::store<f32>(e + 0x224, 1.0f); /* global scale */
        gabi::store<f32>(e + 0x238, 1.5f); /* global particle scale */
        gabi::store<f32>(e + 0x220, 1.0f);
        gabi::store<f32>(e + 0x23C, 1.5f);
        gabi::store<f32>(e + 0x228, 1.0f);
        gabi::store<f32>(e + 0x240, 1.5f);
        i_this->mSmokeType = type;
    }
}
VERIFY(0x02445F98, smoke_set);

/* 02446074: the wall angle around the actor (two parallel lines hit a wall); 0xDCF when none */
s16 get_z_ang(pt_class* i_this) {
    WWHD_FUNC(0x02446074, s16, i_this);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);

    s16 angle = 0;
    for (int i = 16; i != 0; i--) {
        MtxPush();
        cMtx_ZrotM(calc_mtx(), angle);
        offset->x = 10.0f;
        offset->y = 2.0f;
        offset->z = 0.0f;
        MtxPosition(offset, start);
        offset->y = -5.0f;
        MtxPosition(offset, end);
        dBgS_LinChk_Set(linChk, start, end, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            offset->x = -10.0f;
            offset->y = 2.0f;
            MtxPosition(offset, start);
            offset->y = -5.0f;
            MtxPosition(offset, end);
            dBgS_LinChk_Set(linChk, start, end, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                MtxPull();
                dBgS_LinChk_dt(linChk);
                return angle;
            }
        }
        MtxPull();
        angle += 0x1000;
    }
    dBgS_LinChk_dt(linChk);
    return 0xDCF;
}
VERIFY(0x02446074, get_z_ang);

/* 02446324 */
static void* esa_s_sub(void* ac1, void*) {
    WWHD_FUNC(0x02446324, void*, ac1, (u32)0);
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == PROC_ESA) {
        return ac1;
    }
    return nullptr;
}
VERIFY(0x02446324, esa_s_sub);

void action(pt_class* i_this); /* 02447300, d_a_pt_action.cpp */
static inline void action_call(pt_class* i_this) { gabi::call(0x02447300, i_this); }

/* 02446374 */
static BOOL daPt_Execute(pt_class* i_this) {
    WWHD_FUNC(0x02446374, BOOL, i_this);
    gabi::Local<cXyz> zero;
    zero->x = 0.0f;
    zero->y = 0.0f;
    zero->z = 0.0f;

    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);

    s16 counter = i_this->mCounter + 1;
    i_this->mCounter = counter;
    if ((counter & 0x1F) == 0 && i_this->current.pos.y - i_this->home.pos.y < -3000.0f) {
        /* fell out of the world */
        fopAcM_delete(i_this);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        if (i_this->mBehaviorType == 0) {
            i_this->mbRespawn = 1;
            return TRUE;
        }
        if (i_this->mDisableRespawnSwitch != 0xFF) {
            dComIfGs_onSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo());
        }
        return TRUE;
    }

    for (int i = 0; i < 3; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i]--;
        }
    }
    if (i_this->mDamageTimer != 0) {
        i_this->mDamageTimer--;
    }

    if (l_HIO().mbStop != 0) {
        return TRUE;
    }
    action_call(i_this);
    if (i_this->mbHide != 0) {
        return TRUE;
    }

    if (i_this->mbBgCheck != 0) {
        cXyz* ccMove = &i_this->mStts.m_cc_move; /* GetCCMoveP: null-preserving */
        if (gabi::ea(ccMove) != 0) {
            i_this->current.pos.x = i_this->current.pos.x + ccMove->x;
            i_this->current.pos.y = i_this->current.pos.y + ccMove->y;
            i_this->current.pos.z = i_this->current.pos.z + ccMove->z;
        }
        i_this->mOldSpeedY = i_this->speed.y;
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
    }

    mDoMtx_stack_transS(i_this->current.pos);
    if (i_this->scale.y < 0.9f) {
        mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.x);
    }
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_transM(0.0f, i_this->mDrawYOffset, 0.0f);
    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* blinking */
    if (i_this->mBlinkTimer != 0) {
        s16 t = i_this->mBlinkTimer - 1;
        i_this->mBlinkTimer = t;
        if (t <= 7) {
            i_this->mpBtp->setFrame((f32)t);
        }
    } else {
        i_this->mBlinkTimer = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
    }
    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
    i_this->mpBrk->setFrame((f32)i_this->mBrkFrame);
    i_this->mpMorf->calc();
    enemy_fire(&i_this->mEnemyFire);

    PSMTXCopy(getAnmMtx(model, 8), calc_mtx());
    MtxPosition(zero, &i_this->eyePos);
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(i_this) + 0x390); /* attention_info.position */
    attnPos->z = (f32)i_this->eyePos.z;
    attnPos->x = (f32)i_this->eyePos.x;
    attnPos->y = i_this->eyePos.y + 30.0f;
    i_this->mSph.SetC(&i_this->eyePos);
    i_this->mSph.SetR(40.0f);
    dComIfG_Ccsp_Set(&i_this->mSph);

    gabi::Local<cXyz> atPos;
    if (i_this->mbAttack != 0) {
        PSMTXCopy(getAnmMtx(model, 15), calc_mtx());
        zero->y = 0.0f;
        zero->z = 0.0f;
        zero->x = 50.0f;
        MtxPosition(zero, atPos);
        i_this->mbAttack = 0;
        i_this->mAtSph.SetC(atPos);
        dComIfG_Ccsp_Set(&i_this->mAtSph);
    } else {
        atPos->x = 20000.0f;
        atPos->y = 50000.0f;
        atPos->z = 20000.0f;
        i_this->mAtSph.SetC(atPos);
        dComIfG_Ccsp_Set(&i_this->mAtSph);
    }
    return TRUE;
}
VERIFY(0x02446374, daPt_Execute);

/* 024468DC */
static BOOL daPt_IsDelete(pt_class*) {
    WWHD_FUNC(0x024468DC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024468DC, daPt_IsDelete);

/* 024468E4 */
static BOOL daPt_Delete(pt_class* i_this) {
    WWHD_FUNC(0x024468E4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1003829C) /* "Pt" */);
    if (i_this->mbHioSet) {
        s8 no = l_HIO().mNo;
        hio_set() = 0;
        mDoHIO_deleteChild(no);
    }
    /* mSmokeCb.remove() (virtual) */
    gabi::call_ptr(gabi::load<u32>(i_this->mSmokeCb.__vtbl + 0x44), &i_this->mSmokeCb);
    enemy_fire_remove(&i_this->mEnemyFire);

    if (i_this->mbRespawn) {
        /* respawn: a new Miniblin at the home position */
        if (i_this->mEnableSpawnSwitch == 0xFF || dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, fopAcM_GetRoomNo(i_this))) {
            if (i_this->mDisableRespawnSwitch == 0xFF ||
                !dComIfGs_isSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo())) {
                fopAcM_prm_class_l* append = fopAcM_CreateAppend();
                append->mPos.copy(i_this->home.pos);
                append->mAngle.x = i_this->home.angle.x;
                append->mAngle.y = i_this->home.angle.y;
                append->mAngle.z = i_this->home.angle.z;
                if (i_this->mRespawnDelay != 7) {
                    append->mAngle.x = i_this->mRespawnDelay * 20 + 20;
                }
                append->mParameter = fopAcM_GetParam(i_this) | 0x10;
                append->mRoomNo = fopAcM_GetRoomNo(i_this);
                fpcSCtRq_Request(fpcLy_CurrentLayer(), PROC_PT, 0, nullptr, append);
            }
        }
    }
    return TRUE;
}
VERIFY(0x024468E4, daPt_Delete);

/* 02446A4C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02446A4C, BOOL, a_this);
    pt_class* i_this = (pt_class*)a_this;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100382A4) /* "Pt" */, dRes_INDEX_PT_BDL_PT_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100382A4), dRes_INDEX_PT_BCK_WAIT_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1,
                                            1, nullptr, 0, 0x11020203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    void* p = operator_new(0x74);
    if (p != nullptr) p = mDoExt_btpAnm_ct(p);
    i_this->mpBtp = (mDoExt_baseAnm_l*)p;
    if (p == nullptr) {
        return FALSE;
    }
    J3DModel* model = i_this->mpMorf->getModel();
    void* btp = dComIfG_getObjectRes(STR(0x100382A4), dRes_INDEX_PT_BTP_e, SAFESTRING_VTBL);
    if (!mDoExt_btpAnm_init(i_this->mpBtp, getModelData(model), btp, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    p = operator_new(0x78);
    if (p != nullptr) p = mDoExt_brkAnm_ct(p);
    i_this->mpBrk = (mDoExt_baseAnm_l*)p;
    if (p == nullptr) {
        return FALSE;
    }
    model = i_this->mpMorf->getModel();
    void* brk = dComIfG_getObjectRes(STR(0x100382A4), dRes_INDEX_PT_BRK_e, SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mpBrk, getModelData(model), brk, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    i_this->mBrkFrame = gabi::ftoi(cM_rndF(3.999f));
    return TRUE;
}
VERIFY(0x02446A4C, useHeapInit);

/* 02446C68: enemyfire::enemyfire (inline constructor, this TU's copy) */
static enemyfire* enemyfire_ct(enemyfire* self) {
    WWHD_FUNC(0x02446C68, enemyfire*, self);
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    /* JGeometry::TVec3 mDirection: GHS allocates when the member's address is NULL */
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->m228 = 1.0f;
    return self;
}
VERIFY(0x02446C68, enemyfire_ct);

/* 02446CF4: pt_class::pt_class (member constructors; HD: allocates when this == NULL) */
static pt_class* pt_class_ct(pt_class* self) {
    WWHD_FUNC(0x02446CF4, pt_class*, self);
    if (self == nullptr) {
        self = (pt_class*)operator_new(sizeof(pt_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = PT_VTBL;
    gabi::call(0x024EFE94, &self->mAcchCir); /* dBgS_AcchCir::dBgS_AcchCir */
    gabi::call(0x024F0474, &self->mAcch);    /* dBgS_Acch::dBgS_Acch */
    u32 acch = gabi::ea(&self->mAcch);       /* dBgS_ObjAcch */
    gabi::store<u32>(acch + 0x10, 0x100381B4);
    gabi::store<u8>(acch + 0x18, 1);
    gabi::store<u32>(acch + 0x20, 0x100381C4);
    gabi::store<u32>(acch + 0x14, 0x100381D4);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph);     /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &self->mAtSph);
    gabi::call(0x025A5B18, &self->mSmokeCb, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, AAB_VTBL);
    gabi::call(0x024EFE94, &self->mEnemyIce.mBgAcchCir);
    gabi::call(0x024F0474, &self->mEnemyIce.mBgAcch);
    acch = gabi::ea(&self->mEnemyIce.mBgAcch);
    gabi::store<u32>(acch + 0x20, 0x100381C4);
    gabi::store<u32>(acch + 0x10, 0x100381B4);
    gabi::store<u8>(acch + 0x18, 1);
    gabi::store<u32>(acch + 0x14, 0x100381D4);
    enemyfire_ct(&self->mEnemyFire);
    return self;
}
VERIFY(0x02446CF4, pt_class_ct);

/* 02446E54 */
static cPhs_State daPt_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02446E54, cPhs_State, a_this);
    pt_class* i_this = (pt_class*)a_this;
    /* fopAcM_ct(a_this, pt_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) pt_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x100382AC) /* "Pt" */);
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    u32 prm = fopAcM_GetParam(a_this);
    s16 spawnDelay = a_this->current.angle.x;
    i_this->mRespawnDelay = (prm >> 5) & 7;
    i_this->mBehaviorType = prm & 0xF;
    i_this->mNoticeRange = prm >> 8;
    if (prm & 0x10) {
        i_this->mbHide = 1;
    }
    prm = fopAcM_GetParam(a_this);
    i_this->mInitialSpawnDelay = spawnDelay;
    a_this->current.angle.x = 0;
    i_this->mDisableRespawnSwitch = prm >> 16;
    i_this->mEnableSpawnSwitch = prm >> 24;
    if (i_this->mEnableSpawnSwitch != 0xFF &&
        !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, dComIfGp_roomControl_getStayNo())) {
        i_this->mbHide = 1;
    }

    a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex(STR(0x100382B0) /* "Puti" */, 0);
    if (!fopAcM_entrySolidHeap(a_this, 0x02446A4C /* useHeapInit */, 0x4B000)) {
        return cPhs_ERROR_e;
    }

    if (!hio_set()) {
        i_this->mbHioSet = 1;
        hio_set() = 1;
        l_HIO().mNo = mDoHIO_createChild(STR(0x100382B8) /* "プチブリン" */, &l_HIO());
    }

    i_this->mAction = 0;
    a_this->actor_status |= 0x100;
    gabi::store<u32>(gabi::ea(a_this) + 0x39C, fopAc_Attn_LOCKON_BATTLE_e); /* attention_info.flags */
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
    i_this->mAcchCir.SetWall(50.0f, 50.0f);
    a_this->health = 2;
    a_this->max_health = 2;
    i_this->mStts.Init(100, 0, a_this);
    i_this->mSph.Set(cc_sph_src);
    i_this->mSph.SetStts(&i_this->mStts);
    i_this->mAtSph.Set(at_sph_src);
    i_this->mAtSph.SetStts(&i_this->mStts);
    a_this->stealItemLeft = 2;

    i_this->mEnemyIce.mpActor = a_this;
    i_this->mEnemyIce.mWallRadius = REG0_F(4) + 30.0f;
    i_this->mEnemyIce.mCylHeight = REG0_F(5) + 30.0f;
    i_this->mEnemyIce.mParticleScale = REG0_F(8) + 0.6f;
    i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
    i_this->mEnemyFire.mpActor = a_this;
    i_this->mEnemyIce.mYOffset = REG0_F(9) + 40.0f;
    for (int i = 0; i < 10; i++) {
        i_this->mEnemyFire.mFlameJntIdxs[i] = fire_jnt(i);
        i_this->mEnemyFire.mParticleScale[i] = fire_scale(i);
    }
    daPt_Execute(i_this);
    return ret;
}
VERIFY(0x02446E54, daPt_Create);

/* 02447138: daPt_HIO_c::daPt_HIO_c */
static daPt_HIO_c* daPt_HIO_c_ct(daPt_HIO_c* self) {
    WWHD_FUNC(0x02447138, daPt_HIO_c*, self);
    if (self == nullptr) {
        self = (daPt_HIO_c*)operator_new(sizeof(daPt_HIO_c));
        if (self == nullptr) return self;
    }
    self->mNo = -1;
    self->__vtbl = HIO_VTBL;
    self->m02 = 0;
    self->mbStop = 0;
    return self;
}
VERIFY(0x02447138, daPt_HIO_c_ct);

/* 0244718C: static initialisation of the translation unit */
static void __sinit_d_a_pt_cpp() {
    WWHD_FUNC(0x0244718C, void, (u32)0);
    sinit_header_statics(0x1046D33C, 0x101CF050);
    daPt_HIO_c_ct(&l_HIO());
    for (int i = 0; i < 6; i++) {
        f32 v = (i & 1) ? -100.0f : 100.0f;
        f32 w = (i & 1) ? 3.0f : -3.0f;
        cXyz& a = l_dir100()[i];
        cXyz& b = l_dir3()[i];
        a.x = (i >> 1) == 0 ? v : 0.0f;
        a.y = (i >> 1) == 1 ? v : 0.0f;
        a.z = (i >> 1) == 2 ? v : 0.0f;
        b.x = (i >> 1) == 0 ? w : 0.0f;
        b.y = (i >> 1) == 1 ? w : 0.0f;
        b.z = (i >> 1) == 2 ? w : 0.0f;
    }
}
VERIFY(0x0244718C, __sinit_d_a_pt_cpp);

/* 024472EC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024472EC, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x024472EC, SafeString_dt);

/* 02449B54: pt_class deleting destructor (HD virtual destructor) */
static void pt_class_dt(pt_class* self, s32 flags) {
    WWHD_FUNC(0x02449B54, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x02515AE8, &self->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    /* enemyice: ~dBgS_ObjAcch (inline) -> ~dBgS_Acch, ~dBgS_AcchCir */
    u32 acch = gabi::ea(&self->mEnemyIce.mBgAcch);
    gabi::store<u32>(acch + 0x20, 0x100381C4);
    gabi::store<u32>(acch + 0x14, 0x100381D4);
    gabi::call(0x024EFD9C, acch, 0);                                   /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    gabi::call(0x02515AE8, &self->mAtSph, 2);
    gabi::call(0x02515AE8, &self->mSph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, 0x100381C4);
    gabi::store<u32>(acch + 0x14, 0x100381D4);
    gabi::call(0x024EFD9C, acch, 0);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x02449B54, pt_class_dt);

/* 02445DB8, 02449C5C: empty virtual functions in this TU's vtables (0x10038098, 0x100380B4) */
static void empty_virtual_02445DB8(void*) {
    WWHD_FUNC(0x02445DB8, void, (u32)0);
}
VERIFY(0x02445DB8, empty_virtual_02445DB8);
static void empty_virtual_02449C5C(void*) {
    WWHD_FUNC(0x02449C5C, void, (u32)0);
}
VERIFY(0x02449C5C, empty_virtual_02449C5C);
