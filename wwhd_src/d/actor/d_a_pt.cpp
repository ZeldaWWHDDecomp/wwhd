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
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

#define SAFESTRING_VTBL 0x100380AC /* this TU's sead::SafeString vtable */
#define PT_VTBL 0x10038234         /* pt_class vtable (HD virtual destructor) */
#define HIO_VTBL 0x10038224        /* daPt_HIO_c vtable */
#define AAB_VTBL 0x100380C4        /* this TU's cM3dGAab vtable */

// HD: translation-unit vtables and common-header constructor storage.
const gc_tu_data gc_tu = {
    .safeString = SAFESTRING_VTBL,
    .objAcch = {0x100381B4, 0x100381D4, 0x100381C4},
    .objAcchDt = {0x100381C4, 0x100381D4},
    .gndChk = {0x10038174, 0x10038184, 0x100381A4, 0x10038194},
    .gndChkDt = {0x10038104, 0x10038124, 0x100380E4},
    .headerStatics = {0x1046D33C, 0x101CF050, 0},
};
static daPt_HIO_c l_HIO;
static u8 hio_set = 0;
static cXyz l_dir100[6] = {};
static cXyz l_dir3[6] = {};

static const PtSphereSource cc_sph_src = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF3DFEFF, 0x00000003, 0x00000075, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, {0.0f, 0.0f, 0.0f}, 40.0f};
static const PtSphereSource at_sph_src = {0x00000000, 0x00000800, 0x01000000, 0x00000005, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, {0.0f, 0.0f, 0.0f}, 25.0f};
static const s8 fire_jnt[10] = {8, -1, 7, -6, 11, -1, 13, -1, 0, 0};
static const f32 fire_scale[10] = {1.0f, 0.0f, 0.699999988f, 0.0f, 0.600000024f, 0.0f, 0.600000024f, 0.0f, 0.0f, 0.0f};

enum { fpcNm_PT_e = 0xF7 };
enum { DSNAP_TYPE_PT = 0xAB };

// HD: daPt_HIO_c::daPt_HIO_c
/* 02447138 */
static daPt_HIO_c* daPt_HIO_c_ct(daPt_HIO_c* self) {
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

/* 02445DBC */
void anm_init(pt_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx) {
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Pt", bckFileIdx, SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes("Pt", soundFileIdx, SAFESTRING_VTBL);
        i_this->m2C0->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Pt", bckFileIdx, SAFESTRING_VTBL);
        i_this->m2C0->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}

/* 02445EE4 */
static BOOL daPt_Draw(pt_class* i_this) {
    if (i_this->mInitialSpawnWaitsUntilOffScreen == 0) {
        J3DModel* model = i_this->m2C0->getModel();
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, model, &i_this->tevStr);
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)i_this->m2C4.get(), getModelData(model),
                            (s16)gabi::ftoi(i_this->m2C4->getFrame()));
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)i_this->m2C8.get(), getModelData(model), i_this->m2C8->getFrame());
        i_this->m2C0->entryDL();
        i_this->mbBgCheck = 0;
    }
    dSnap_RegistFig(DSNAP_TYPE_PT, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02445F98 */
void smoke_set(pt_class* i_this, signed char type) {
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
        PtEmitter* e = reinterpret_cast<PtEmitter*>(emitter);
        e->mSpread = 0.5f;  /* rate */
        e->mRate = 3.0f;
        e->mGlobalScale.y = 1.0f; /* global scale */
        e->mGlobalParticleScale.x = 1.5f; /* global particle scale */
        e->mGlobalScale.x = 1.0f;
        e->mGlobalParticleScale.y = 1.5f;
        e->mGlobalScale.z = 1.0f;
        e->mGlobalParticleScale.z = 1.5f;
        i_this->mSmokeType = type;
    }
}

// HD: the wall angle around the actor (two parallel lines hit a wall); 0xDCF when none
/* 02446074 */
s16 get_z_ang(pt_class* i_this) {
    gabi::Local<PtLineCheck> linChk;
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

/* 02446324 */
static void* esa_s_sub(void* ac1, void*) {
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_ESA_e) {
        return ac1;
    }
    return nullptr;
}

void action(pt_class* i_this); /* 02447300, d_a_pt_action.cpp */

/* 02446374 */
static BOOL daPt_Execute(pt_class* i_this) {
    gabi::Local<cXyz> zero;
    zero->x = 0.0f;
    zero->y = 0.0f;
    zero->z = 0.0f;

    if (pt_enemyIce(&i_this->mEnemyIce)) {
        i_this->m2C0->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->m2C0->setPlaySpeed(3.0f);
        i_this->m2C0->play(&i_this->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->m2C0->getModel(), mDoMtx_stack_c::get());
        i_this->m2C0->calc();
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);

    s16 counter = i_this->m2D0 + 1;
    i_this->m2D0 = counter;
    if ((counter & 0x1F) == 0 && i_this->current.pos.y - i_this->home.pos.y < -3000.0f) {
        /* fell out of the world */
        fopAcM_delete(i_this);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        if (i_this->mBehaviorType == 0) {
            i_this->m2BA = 1;
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
    if (i_this->m30E != 0) {
        i_this->m30E--;
    }

    if (l_HIO.mbStop != 0) {
        return TRUE;
    }
    pt_action(i_this);
    if (i_this->mInitialSpawnWaitsUntilOffScreen != 0) {
        return TRUE;
    }

    if (i_this->mbBgCheck != 0) {
        cXyz* ccMove = &i_this->mStts.m_cc_move; /* GetCCMoveP: null-preserving */
        if (gabi::ea(ccMove) != 0) {
            i_this->current.pos.x = i_this->current.pos.x + ccMove->x;
            i_this->current.pos.y = i_this->current.pos.y + ccMove->y;
            i_this->current.pos.z = i_this->current.pos.z + ccMove->z;
        }
        i_this->m314 = i_this->speed.y;
        i_this->m368.CrrPos(dComIfG_Bgsp());
    }

    mDoMtx_stack_transS(i_this->current.pos);
    if (i_this->scale.y < 0.9f) {
        mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.x);
    }
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_transM(0.0f, i_this->m318, 0.0f);
    J3DModel* model = i_this->m2C0->getModel();
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* blinking */
    if (i_this->m2D6 != 0) {
        s16 t = i_this->m2D6 - 1;
        i_this->m2D6 = t;
        if (t <= 7) {
            i_this->m2C4->setFrame((f32)t);
        }
    } else {
        i_this->m2D6 = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
    }
    i_this->m2C0->play(&i_this->eyePos, 0, 0);
    i_this->m2C8->setFrame((f32)i_this->m2CC);
    i_this->m2C0->calc();
    pt_enemyFire(&i_this->mEnemyFire);

    PSMTXCopy(getAnmMtx(model, 8), calc_mtx());
    MtxPosition(zero, &i_this->eyePos);
    cXyz* attnPos = &i_this->attention_info.position; /* attention_info.position */
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

/* 024468DC */
static BOOL daPt_IsDelete(pt_class*) {
    return TRUE;
}

/* 024468E4 */
static BOOL daPt_Delete(pt_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Pt");
    if (i_this->mDC4) {
        s8 no = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(no);
    }

    pt_removeSmoke(&i_this->mSmokeCb);
    pt_enemyFireRemove(&i_this->mEnemyFire);

    if (i_this->m2BA) {
        /* respawn: a new Miniblin at the home position */
        if (i_this->mEnableSpawnSwitch == 0xFF || dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, fopAcM_GetRoomNo(i_this))) {
            if (i_this->mDisableRespawnSwitch == 0xFF ||
                !dComIfGs_isSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo())) {
                PtCreateAppend* append = pt_createAppend();
                append->mPos.copy(i_this->home.pos);
                append->mAngle.x = i_this->home.angle.x;
                append->mAngle.y = i_this->home.angle.y;
                append->mAngle.z = i_this->home.angle.z;
                if (i_this->mRespawnDelay != 7) {
                    append->mAngle.x = i_this->mRespawnDelay * 20 + 20;
                }
                append->mParameter = fopAcM_GetParam(i_this) | 0x10;
                append->mRoomNo = fopAcM_GetRoomNo(i_this);
                pt_createRequest(pt_currentLayer(), fpcNm_PT_e, 0, nullptr, append);
            }
        }
    }
    return TRUE;
}

/* 02446A4C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    pt_class* i_this = (pt_class*)a_this;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Pt", dRes_INDEX_PT_BDL_PT_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Pt", dRes_INDEX_PT_BCK_WAIT_e, SAFESTRING_VTBL);
    i_this->m2C0 = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1,
                                            1, nullptr, 0, 0x11020203);
    if (i_this->m2C0 == nullptr || i_this->m2C0->getModel() == nullptr) {
        return FALSE;
    }

    void* p = operator_new(0x74);
    if (p != nullptr) p = pt_btpCtor(p);
    i_this->m2C4 = (PtMaterialAnimation*)p;
    if (p == nullptr) {
        return FALSE;
    }
    J3DModel* model = i_this->m2C0->getModel();
    void* btp = dComIfG_getObjectRes("Pt", dRes_INDEX_PT_BTP_e, SAFESTRING_VTBL);
    if (!pt_btpInit(i_this->m2C4, getModelData(model), btp, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    p = operator_new(0x78);
    if (p != nullptr) p = mDoExt_brkAnm_ct(p);
    i_this->m2C8 = (PtMaterialAnimation*)p;
    if (p == nullptr) {
        return FALSE;
    }
    model = i_this->m2C0->getModel();
    void* brk = dComIfG_getObjectRes("Pt", dRes_INDEX_PT_BRK_e, SAFESTRING_VTBL);
    if (!pt_brkInit(i_this->m2C8, getModelData(model), brk, 1, 0, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    i_this->m2CC = gabi::ftoi(cM_rndF(3.999f));
    return TRUE;
}

// HD: enemyfire::enemyfire (inline constructor, this TU's copy)
/* 02446C68 */
static enemyfire* enemyfire_ct(enemyfire* self) {
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    /* JGeometry::TVec3 mDirection: GHS allocates when the member's address is NULL */
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    pt_sphereCtor(&self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->mParticleScaleMultiplier = 1.0f;
    return self;
}

// HD: pt_class::pt_class (member constructors; HD: allocates when this == NULL)
/* 02446CF4 */
static pt_class* pt_class_ct(pt_class* self) {
    if (self == nullptr) {
        self = (pt_class*)operator_new(sizeof(pt_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = PT_VTBL;
    pt_circleCtor(&self->m328); /* dBgS_AcchCir::dBgS_AcchCir */
    pt_acchCtor(&self->m368);    /* dBgS_Acch::dBgS_Acch */
    dBgS_ObjAcch* acch = &self->m368;       /* dBgS_ObjAcch */
    static_cast<cBgS_Chk*>(acch)->__vtbl = gc_tu.objAcch.v10;
    static_cast<dBgS_PolyPassChk*>(acch)->mbObjThrough = 1;
    static_cast<dBgS_GrpPassChk*>(acch)->__vtbl = gc_tu.objAcch.v20;
    static_cast<dBgS_PolyPassChk*>(acch)->__vtbl = gc_tu.objAcch.v14;
    dCcD_Stts_ct(&self->mStts);
    pt_sphereCtor(&self->mSph);     /* dCcD_Sph::dCcD_Sph */
    pt_sphereCtor(&self->mAtSph);
    pt_smokeCtor(&self->mSmokeCb, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, AAB_VTBL);
    pt_circleCtor(&self->mEnemyIce.mBgAcchCir);
    pt_acchCtor(&self->mEnemyIce.mBgAcch);
    acch = &self->mEnemyIce.mBgAcch;
    static_cast<dBgS_GrpPassChk*>(acch)->__vtbl = gc_tu.objAcch.v20;
    static_cast<cBgS_Chk*>(acch)->__vtbl = gc_tu.objAcch.v10;
    static_cast<dBgS_PolyPassChk*>(acch)->mbObjThrough = 1;
    static_cast<dBgS_PolyPassChk*>(acch)->__vtbl = gc_tu.objAcch.v14;
    enemyfire_ct(&self->mEnemyFire);
    return self;
}

/* 02446E54 */
static cPhs_State daPt_Create(fopAc_ac_c* a_this) {
    pt_class* i_this = (pt_class*)a_this;
    /* fopAcM_ct(a_this, pt_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) pt_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, "Pt");
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    u32 prm = fopAcM_GetParam(a_this);
    s16 spawnDelay = a_this->current.angle.x;
    i_this->mRespawnDelay = (prm >> 5) & 7;
    i_this->mBehaviorType = prm & 0xF;
    i_this->mNoticeRange = prm >> 8;
    if (prm & 0x10) {
        i_this->mInitialSpawnWaitsUntilOffScreen = 1;
    }
    prm = fopAcM_GetParam(a_this);
    i_this->mInitialSpawnDelay = spawnDelay;
    a_this->current.angle.x = 0;
    i_this->mDisableRespawnSwitch = prm >> 16;
    i_this->mEnableSpawnSwitch = prm >> 24;
    if (i_this->mEnableSpawnSwitch != 0xFF &&
        !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, dComIfGp_roomControl_getStayNo())) {
        i_this->mInitialSpawnWaitsUntilOffScreen = 1;
    }

    a_this->itemTableIdx = pt_characterIndex("Puti", 0);
    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x4B000)) {
        return cPhs_ERROR_e;
    }

    if (!hio_set) {
        i_this->mDC4 = 1;
        hio_set = 1;
        // HD: original HIO label is Shift-JIS 837683608375838A8393; glue retains those guest bytes.
        l_HIO.mNo = mDoHIO_createChild("Miniblin", &l_HIO);
    }

    i_this->m2D2 = PT_WAIT;
    a_this->actor_status |= fopAcStts_CULL_e;
    a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags */
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->m2C0->getModel()));
    i_this->m368.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->m328, &a_this->speed);
    i_this->m328.SetWall(50.0f, 50.0f);
    a_this->health = 2;
    a_this->max_health = 2;
    i_this->mStts.Init(100, 0, a_this);
    i_this->mSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&cc_sph_src));
    i_this->mSph.SetStts(&i_this->mStts);
    i_this->mAtSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&at_sph_src));
    i_this->mAtSph.SetStts(&i_this->mStts);
    a_this->stealItemLeft = 2;

    i_this->mEnemyIce.mpActor = a_this;
    i_this->mEnemyIce.mWallRadius = REG0_F(4) + 30.0f;
    i_this->mEnemyIce.mCylHeight = REG0_F(5) + 30.0f;
    i_this->mEnemyIce.mParticleScale = REG0_F(8) + 0.6f;
    i_this->mEnemyFire.mpMcaMorf = i_this->m2C0;
    i_this->mEnemyFire.mpActor = a_this;
    i_this->mEnemyIce.mYOffset = REG0_F(9) + 40.0f;
    for (int i = 0; i < 10; i++) {
        i_this->mEnemyFire.mFlameJntIdxs[i] = fire_jnt[i];
        i_this->mEnemyFire.mParticleScale[i] = fire_scale[i];
    }
    daPt_Execute(i_this);
    return ret;
}

// HD: compiler-generated and template code of this translation unit.
// Static initialisation precedes the mutable direction tables' use.
/* 0244718C */
static void __sinit_d_a_pt_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daPt_HIO_c_ct(&l_HIO);
    for (int i = 0; i < 6; i++) {
        f32 v = (i & 1) ? -100.0f : 100.0f;
        f32 w = (i & 1) ? 3.0f : -3.0f;
        cXyz& a = l_dir100[i];
        cXyz& b = l_dir3[i];
        a.x = (i >> 1) == 0 ? v : 0.0f;
        a.y = (i >> 1) == 1 ? v : 0.0f;
        a.z = (i >> 1) == 2 ? v : 0.0f;
        b.x = (i >> 1) == 0 ? w : 0.0f;
        b.y = (i >> 1) == 1 ? w : 0.0f;
        b.z = (i >> 1) == 2 ? w : 0.0f;
    }
}

// HD: sead::SafeString deleting destructor (this TU's copy)
/* 024472EC */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) operator_delete(p);
}

// HD: pt_class deleting destructor (HD virtual destructor)
/* 02449B54 */
static void pt_class_dt(pt_class* self, s32 flags) {
    if (self == nullptr) return;
    pt_sphereDtor(&self->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    /* enemyice: ~dBgS_ObjAcch (inline) -> ~dBgS_Acch, ~dBgS_AcchCir */
    dBgS_ObjAcch* acch = &self->mEnemyIce.mBgAcch;
    static_cast<dBgS_GrpPassChk*>(acch)->__vtbl = gc_tu.objAcch.v20;
    static_cast<dBgS_PolyPassChk*>(acch)->__vtbl = gc_tu.objAcch.v14;
    pt_acchDtor(acch, 0);                                   /* dBgS_Acch::~dBgS_Acch */
    pt_circleDtor(&self->mEnemyIce.mBgAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    pt_sphereDtor(&self->mAtSph, 2);
    pt_sphereDtor(&self->mSph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = &self->m368;
    static_cast<dBgS_GrpPassChk*>(acch)->__vtbl = gc_tu.objAcch.v20;
    static_cast<dBgS_PolyPassChk*>(acch)->__vtbl = gc_tu.objAcch.v14;
    pt_acchDtor(acch, 0);
    pt_circleDtor(&self->m328.m_cir, 2);
    pt_actorDtor(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}

// HD: empty virtual function.
/* 02445DB8 */
static void empty_base_virtual(void*) {
}

/* 02449C5C */
static void empty_actor_virtual(void*) {
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0200E814 s32 pt_nameIndex(void* table, const char* name, s32 parameter)
ext 020402C8 BOOL pt_enemyIce(enemyice* ice)
ext 02041570 void pt_enemyFire(enemyfire* fire)
ext 02041C30 void pt_enemyFireRemove(enemyfire* fire)
ext 025D5600 PtCreateAppend* pt_createAppend()
ext 025DED64 void* pt_currentLayer()
ext 025E14A8 u32 pt_createRequest(void* layer, s16 name, u32 callback, void* data, void* append)
ext 025E7820 PtMaterialAnimation* pt_btpCtor(void* animation)
ext 025E789C BOOL pt_btpInit(PtMaterialAnimation* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry)
ext 025E80D0 PtMaterialAnimation* pt_brkCtor(void* animation)
ext 025E8154 BOOL pt_brkInit(PtMaterialAnimation* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry)
ext 025166F0 void pt_sphereCtor(dCcD_Sph* sphere)
ext 024EFE94 void pt_circleCtor(dBgS_AcchCir* circle)
ext 024F0474 void pt_acchCtor(dBgS_ObjAcch* collision)
ext 025A5B18 void pt_smokeCtor(PtSmokeCallback* callback, s32 mode)
ext 02515AE8 void pt_sphereDtor(dCcD_Sph* sphere, s32 flags)
ext 024EFD9C void pt_acchDtor(dBgS_ObjAcch* collision, s32 flags)
ext 02018034 void pt_circleDtor(cM3dGCir_l* circle, s32 flags)
ext 025D50BC void pt_actorDtor(fopAc_ac_c* actor, s32 flags)
ext 02447300 void pt_action(pt_class* actor)
data l_HIO 1046D334 nocheck
data hio_set 101CEF78
data l_dir100 1046D358 nocheck
data l_dir3 1046D3A0 nocheck
data cc_sph_src 101CEF9C
data at_sph_src 101CEFDC
data fire_jnt 101CF044
data fire_scale 101CF01C
str 1003824C "Pt" in 02445DBC
str 1003829C "Pt" in 024468E4
str 100382A4 "Pt" in 02446A4C
str 100382AC "Pt" in 02446E54
str 100382B0 "Puti" in 02446E54
str 100382B8 "Miniblin" renamed in 02446E54
*/
