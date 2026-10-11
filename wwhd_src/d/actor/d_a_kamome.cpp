/**
 * d_a_kamome.cpp (WWHD)
 * NPC - Seagull
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kamome.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kamome.h"

static kamomeHIO_c l_kamomeHIO;
static u8 hio_set;
static s32 esa_check_count;
static s32 ko_count;
static fopAc_ac_c* esa_info[100] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static dCcD_SrcSph co_sph_src;
static u32 kamomeRegistration[4] = {0,0,0,0};
static f32 kamomeAngleMin;
static f32 kamomeAngleMax;
static u8 kamomeAngleStorage;
static u8 kamomeFlagStorage;
static u8 kamomeGlobalRecord1;
static u8 kamomeGlobalRecord2;
static u8 kamomeGlobalRecord3;
const gc_tu_data gc_tu = {.safeString = 0x10012304};

/* guest string literals: GHS keeps one copy per use */
#define SAFESTRING_VTBL 0x10012304 /* this TU's sead::SafeString vtable */


/* ---- file statics (.bss) ---- */


/* esa_class (bait): the fields read here */


/* 02185558 */
static void anm_init(kamome_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float speed, int soundAnmResIdx) {

    if (REG0_S(3) == 0x23) {
        anmResIdx = dRes_INDEX_KAMOME_BCK_KA_LAND1_e;
    } else if (REG0_S(3) == 0x24) {
        anmResIdx = dRes_INDEX_KAMOME_BCK_KA_FLY1_e;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Kamome", anmResIdx, SAFESTRING_VTBL);
    void* sound = dComIfG_getObjectRes("Kamome", soundAnmResIdx, SAFESTRING_VTBL);
    i_this->mpMorf->setAnm( anm, loopMode, morf, speed, 0.0f, -1.0f, sound);
}

/* dBgS_LinChk (stack object): HD layout, sub-object vtables after the members */





/* 0218566C */
static void* s_a_d_sub(void* ac1, void* ac2) {

    dBgS_LinChk_l linChk;
    dBgS_LinChk_ct(&linChk);
    fopAc_ac_c* actor2 = (fopAc_ac_c*)ac2;

    // HD: also requires the bait's mState == 1
    if (esa_check_count < 100 && fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_ESA_e) {
        esa_class* esa = (esa_class*)ac1;
        if (esa->field_0x298 == 0 && esa->mState == 1) {
            cXyz sp14;
            cXyz sp08;

            auto p = esa->actor.current.pos.get();
            p.y += 10.0f;
            sp08 = p;
            sp14.copy(actor2->current.pos);

            dBgS_LinChk_Set(&linChk, &sp14, &sp08, actor2);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                s32 n = esa_check_count;
                esa_check_count = n + 1;
                esa_info[n] = &esa->actor;
            }
        }
    }
    dBgS_LinChk_dt(&linChk);
    return nullptr;
}

/* 02185808 */
static fopAc_ac_c* search_esa(kamome_class* i_this) {

    esa_check_count = 0;
    fpcM_Search(gabi::guest_fn((const void*)&s_a_d_sub), &i_this->actor);

    if (esa_check_count != 0) {
        f32 fVar3 = 50.0f;
        s32 i = 0;
        while (i < esa_check_count) {
            esa_class* esa = (esa_class*)(fopAc_ac_c*)esa_info[i];
            f32 x = esa->actor.current.pos.x - i_this->actor.current.pos.x;
            f32 z = esa->actor.current.pos.z - i_this->actor.current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < fVar3) {
                esa->field_0x298 = 1;
                return &esa->actor;
            }

            i++;
            if (i == esa_check_count) {
                i = 0;
                fVar3 += 50.0f;
                if (fVar3 > 10000.0f) {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}

/* 02185960 */
static void* s_a_i_sub(void* ac1, void* ac2) {

    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_NPC_LS1_e) {
        return ac1;
    }
    return nullptr;
}

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */




/* 021859B0 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        kamome_class* pKamome = static_cast<kamome_class*>(model->getUserArea());
        s32 jntNo = kamomeJoint(joint)->mJntNo;
        if (pKamome != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), pKamome->mJointRotY);
            cMtx_ZrotM(calc_mtx(), pKamome->mJointRotZ);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* dst = model->getAnmMtx(jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 02185AE0 */
static BOOL daKamome_Draw(kamome_class* i_this) {

    // HD: based on the USA version (mbNoDraw is reset in Execute)
    if ((i_this->mSwitchNo != 0) || (i_this->mbNoDraw != 0)) {
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
    mDoExt_McaMorf* morf = i_this->mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &i_this->actor.tevStr);
    i_this->mpMorf->entryDL();

    // HD: no blob shadow (dComIfGd_setShadow)

    dSnap_RegistFig(DSNAP_TYPE_KAMOME, &i_this->actor, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02185B78 */
static void kamome_pos_move(kamome_class* i_this) {

    f32 fVar4 = i_this->mTargetPos.x - i_this->actor.current.pos.x;
    f32 fVar5 = i_this->mTargetPos.z - i_this->actor.current.pos.z;
    f32 fVar1 = i_this->mTargetPos.y - i_this->actor.current.pos.y;
    s16 rotTargetY = cM_atan2s(fVar4, fVar5);
    s16 rotTargetX = -cM_atan2s(fVar1, std_sqrtf(gabi::fmadds(fVar4, fVar4, fVar5 * fVar5)));
    s16 sVar3 = i_this->actor.current.angle.y;
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y, rotTargetY, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    sVar3 = (sVar3 - i_this->actor.current.angle.y) * 0x20;

    s16 rotTargetZ = REG0_S(1) + 5500;
    if (sVar3 > rotTargetZ) {
        sVar3 = rotTargetZ;
    } else if (sVar3 < -rotTargetZ) {
        sVar3 = -rotTargetZ;
    }

    cLib_addCalcAngleS2(&i_this->actor.current.angle.z, sVar3, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade * 0.5f));
    cLib_addCalcAngleS2(&i_this->actor.current.angle.x, rotTargetX, REG0_S(3) + 10,
                        gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    // HD: the target is REG18_F(0) + 1.0f (GameCube: 1.0f)
    cLib_addCalc2(&i_this->mRotVelFade, REG_F(18, 0) + 1.0f, 1.0f, 0.04f);
    cLib_addCalc2(&i_this->actor.speedF, i_this->mVelocityFwdTarget, 1.0f, i_this->mVelocityFwdTargetMaxVel);
    cXyz fwd;
    fwd.x = 0.0f;
    fwd.y = 0.0f;
    fwd.z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    MtxPosition(&fwd, &i_this->actor.speed);
    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;

    if (i_this->mRiseTimer != 0) {
        i_this->mRiseTimer--;
        i_this->actor.current.pos.y += 5.0f;
    }
}

/* 02185DF0 */
static void kamome_bgcheck(kamome_class* i_this) {

    f32 fVar1 = i_this->mScale * 40.0f;
    i_this->actor.current.pos.y = i_this->actor.current.pos.y - fVar1;
    i_this->actor.old.pos.y = i_this->actor.old.pos.y - fVar1;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    i_this->actor.current.pos.y = i_this->actor.current.pos.y + fVar1;
    i_this->actor.old.pos.y = i_this->actor.old.pos.y + fVar1;
}

/* 02185E80 */
static void kamome_ground_pos_move(kamome_class* i_this) {

    s16 iVar2 = cM_atan2s(i_this->mTargetPos.x - i_this->actor.current.pos.x, i_this->mTargetPos.z - i_this->actor.current.pos.z);

    cLib_addCalcAngleS2(&i_this->actor.current.angle.y, iVar2, REG0_S(3) + 2, gabi::ftoi(i_this->mRotVel * i_this->mRotVelFade));
    cLib_addCalc2(&i_this->mRotVelFade, 1.0f, 1.0f, 0.1f);
    cLib_addCalc2(&i_this->actor.speedF, i_this->mVelocityFwdTarget, 1.0f, i_this->mVelocityFwdTargetMaxVel);

    cXyz sp14;
    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);

    cXyz sp08;
    MtxPosition(&sp14, &sp08);
    i_this->actor.speed.x = sp08.x;
    i_this->actor.speed.z = sp08.z;

    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;

    i_this->actor.speed.y -= 3.0f;
    kamome_bgcheck(i_this);

    if (i_this->mAcch.ChkGroundHit()) {
        i_this->actor.speed.y = -0.5f;
    }
}

/* 02185FE4 */
static void* ko_s_sub(void* ac1, void* ac2) {

    kamome_class* i_this = (kamome_class*)ac1;
    if (fopAc_IsActor(ac1) && i_this != nullptr && fpcM_GetName(ac1) == fpcNm_KAMOME_e &&
        (fopAcM_GetParam(&i_this->actor) & 0xf) == 7) {
        ko_count++;
    }
    return nullptr;
}

/* 02186050 */
static void* h_s_sub(void* ac1, void* ac2) {

    kamome_class* i_this = (kamome_class*)ac1;
    if (fopAc_IsActor(ac1) && i_this != nullptr && fpcM_GetName(ac1) == fpcNm_KAMOME_e &&
        (fopAcM_GetParam(&i_this->actor) & 0xf) == 6) {
        return &i_this->actor;
    }
    return nullptr;
}

// HD: the head callback uses joint 8.


/* 021860B0 */
static void daKamome_setMtx(kamome_class* i_this) {

    MtxTrans(i_this->actor.current.pos.x, i_this->actor.current.pos.y, i_this->actor.current.pos.z, 0);

    // HD: no mRot offsets (GameCube: angle.y + mRot.y, angle.x + mRot.x)
    cMtx_YrotM(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->actor.current.angle.z);

    J3DModel* pJVar1 = i_this->mpMorf->getModel();
    J3DModel_setBaseTRMtx(pJVar1, calc_mtx());
    pJVar1->getModelData()->getJointNodePointer(8)->setCallBack((J3DJointCallBack)&nodeCallBack);
    i_this->mpMorf->calc();
    pJVar1->getModelData()->getJointNodePointer(8)->mCallBack = 0;
}

/* 02188C04 */
static BOOL daKamome_IsDelete(kamome_class*) {

    return TRUE;
}

/* 02188C0C */
static BOOL daKamome_Delete(kamome_class* i_this) {

    dComIfG_resDelete(&i_this->mPhase, "Kamome");
    if (i_this->m688 != 0) {
        s8 no = l_kamomeHIO.mNo;
        hio_set = false;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}

/* GHS: `new mDoExt_McaMorf(...)` calls the constructor with this == NULL */
/* 02188C70 */
static BOOL createHeap(fopAc_ac_c* a_this) {

    kamome_class* i_this = (kamome_class*)a_this;

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Kamome", dRes_INDEX_KAMOME_BDL_KA_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Kamome", dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, SAFESTRING_VTBL);
    void* sound = dComIfG_getObjectRes("Kamome", dRes_INDEX_KAMOME_BAS_KA_WAIT1_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, sound,
                                                 0x80000, 0x11000022);
    i_this->mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }

    morf->getModel()->setUserArea((uintptr_t)&i_this->actor); /* setUserArea */
    return TRUE;
}

/* member constructors run by fopAcM_ct (HD: inline parts write the sub-object vtables) */
/* inlined into daKamome_Create */
static void kamome_class_ct(kamome_class* i_this) {
    fopAc_ac_c_ct(&i_this->actor);
    i_this->actor.__vtbl = KAMOME_ACTOR_VTABLE;
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    kamomeAcchCtor(&i_this->mAcch);
    dCcD_Stts_ct(&i_this->mStts);
    kamomeMain_sphereCtor(&i_this->mSph);
}

/* 02188D84 */
static cPhs_State daKamome_Create(fopAc_ac_c* a_this) {

    kamome_class* i_this = (kamome_class*)a_this;
    /* fopAcM_ct(a_this, kamome_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) kamome_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State PVar1 = dComIfG_resLoad(&i_this->mPhase, "Kamome");
    if (PVar1 == cPhs_COMPLEATE_e) {
        i_this->mType = fopAcM_GetParam(a_this);
        if (i_this->mType == 0xff) {
            i_this->mType = 0;
        }

        i_this->mKoMaxCount = fopAcM_GetParam(a_this) >> 8;
        i_this->mPathIdx = fopAcM_GetParam(a_this) >> 0x10;
        i_this->mSwitchNoPrm = fopAcM_GetParam(a_this) >> 0x18;

        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void*)&createHeap), 0x1360)) {
            return cPhs_ERROR_e;
        }

        if (i_this->mPathIdx != 0xff) {
            i_this->mpPath = dPath_GetRoomPath(i_this->mPathIdx, fopAcM_GetRoomNo(a_this));
            if (i_this->mpPath == nullptr) {
                return cPhs_ERROR_e;
            }
            i_this->mbUsePathMovement = i_this->mPathIdx + 1;
            i_this->mPathIdxIncr = 1;
        }

        if (i_this->mSwitchNoPrm != 0xff) {
            i_this->mSwitchNo = i_this->mSwitchNoPrm + 1;
        }

        /* fopAcM_SetMtx(a_this, model->getBaseTRMtx()) */
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        f32 tmp = REG0_F(2) + 1.0f;
        f32 fVar6 = REG0_F(3) + 1.0f;
        i_this->mScale = fVar6 + cM_rndF(tmp - fVar6);
        if ((i_this->mType == 4) || (i_this->mType == 5)) {
            i_this->mScale = i_this->mScale * (REG0_F(9) + 0.75f);
        }

        /* model->setBaseScale(mScale, mScale, mScale) */
        J3DModel* model = i_this->mpMorf->getModel();
        f32 s = i_this->mScale;
        model->mBaseScale.x = s;
        model->mBaseScale.y = s;
        model->mBaseScale.z = s;
        daKamome_setMtx(i_this);
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr); /* dBgS_Acch::Set */

        if ((i_this->mType == 4) || (i_this->mType == 5)) {
            i_this->mAcchCir.SetWall( 50.0f, 80.0f); /* dBgS_AcchCir::SetWall */
        } else {
            i_this->mAcchCir.SetWall( 30.0f, 20.0f);
        }

        if (i_this->mType == 4) {
            i_this->mStts.Init( 0x32, 0xff, a_this); /* dCcD_Stts::Init */
            i_this->mSph.Set(&co_sph_src);       /* dCcD_Sph::Set(co_sph_src) */
            i_this->mSph.SetStts(&i_this->mStts);
        }

        i_this->mGlobalTimer = (s16)gabi::ftoi(cM_rndF(10000.0f));

        if (!hio_set) {
            l_kamomeHIO.mNo = mDoHIO_createChild("J", &l_kamomeHIO);
            i_this->m688 = 1;
            hio_set = true;
        }

        if ((i_this->mType != 6) && (i_this->mType == 7)) {
            i_this->m2B4 = (s16)gabi::ftoi(cM_rndF(10000.0f));
            fopAcM_setStageLayer(a_this);
            a_this->current.roomNo = (s8)0xff; /* fopAcM_SetRoomNo */
        }
    }
    return PVar1;
}

/*  static initialisers */
/* 02189180 */
static void __sinit_d_a_kamome_cpp() {

    /* l_kamomeHIO's base JORReflexible / mDoHIO_entry registration objects (HD) */
    for (int i = 3; i >= 0; i--) kamomeRegistration[i] = 0;
    kamomeMain_registerGlobal(gabi::ea(&kamomeGlobalRecord1)); /* __register_global_object */
    kamomeAngleMin = -3.1415927f;
    kamomeAngleMax = 3.1415927f;
    kamomeMain_initAngle(gabi::ea(&kamomeAngleStorage));
    kamomeMain_registerGlobal(gabi::ea(&kamomeGlobalRecord2));
    kamomeMain_initFlag(gabi::ea(&kamomeFlagStorage));
    kamomeMain_registerGlobal(gabi::ea(&kamomeGlobalRecord3));
    /* kamomeHIO_c::kamomeHIO_c() */
    kamomeHIO_c& h = l_kamomeHIO;
    h.m06 = 1;
    h.m05 = 0;
    h.m0C = 400.0f;
    h.__vtbl = KAMOME_HIO_VTABLE;
    h.m10 = 300.0f;
    h.m08 = 8;
}

/*  kamomeHIO_c deleting destructor (HD: classes with a virtual destructor) */
/* 0218925C */
static void kamomeHIO_c_dt(kamomeHIO_c* p, s32 flags) {

    if (p != nullptr && (flags & 1)) operator_delete(p); /* operator delete */
}

/* WWHD glue
data l_kamomeHIO 104648C8
data hio_set 101B7C00
data esa_check_count 104648A4
data ko_count 104648A8
data esa_info 104648DC nocheck # BSS array; no image bytes
data co_sph_src 101B7C24
data kamomeRegistration 104648B8 nocheck # BSS array; initialised explicitly by sinit
data kamomeAngleMin 104648AC
data kamomeAngleMax 104648B0
data kamomeAngleStorage 104648B4
data kamomeFlagStorage 104648B5
data kamomeGlobalRecord1 101B7C64
data kamomeGlobalRecord2 101B7C70
data kamomeGlobalRecord3 101B7C7C
str 10012414 in 02185558
str 100124C4 in 02188C0C
str 100124CC in 02188C70
str 100124D8 in 02188D84
str 100124E0 in 02188D84
ext 025166F0 void kamomeMain_sphereCtor(dCcD_Sph* sphere)
ext 028F026C void kamomeMain_registerGlobal(u32 record)
ext 028ED6F8 void kamomeMain_initAngle(u32 storage)
ext 028EAB2C void kamomeMain_initFlag(u32 storage)
*/
