/**
 * d_a_bmdfoot.cpp (WWHD)
 * Boss - Kalle Demos (floor tentacles) / 森ボス足 (Forest boss feet)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmdfoot.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source. HD drops the debug
 * register terms (REGn_F/REGn_S) of the GameCube source except REG14_F(4) in damage.
 */
#include "d/actor/d_a_bmdfoot.h"

enum {
    dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK1_e = 5,
    dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK2_e = 6,
    dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK_LOOP_e = 7,
    dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK1_e = 8,
    dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK2_e = 9,
    dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK3_e = 0xA,
    dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_e = 0xB,
    dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_LOOP_e = 0xC,
    dRes_INDEX_BMDFOOT_BCK_ASI_NOBIKIRU_e = 0xD,
    dRes_INDEX_BMDFOOT_BCK_ASI_NUKU_e = 0xE,
    dRes_INDEX_BMDFOOT_BCK_ASI_START1_e = 0xF,
    dRes_INDEX_BMDFOOT_BCK_ASI_START2_e = 0x10,
    dRes_INDEX_BMDFOOT_BCK_ASI_UMARU_e = 0x11,
    dRes_INDEX_BMDFOOT_BCK_ASI_UMARU_WAIT_e = 0x12,
    dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e = 0x13,
    dRes_INDEX_BMDFOOT_BMD_ASI_e = 0x16,
    dRes_INDEX_BMDFOOT_BTK_ASI_e = 0x19,
};

const gc_tu_data gc_tu = {
    .safeString = 0x10009D94,
    .headerStatics = {0x104624DC, 0x10191BBC, 0x104624E8},
};
static BmdfootBoss* boss;
static u8 hio_set;
static daBmdfoot_HIO_c l_HIO;
static BmdfootThreeJoints eff_id;
static BmdfootThreeJoints jno;
static BmdfootFiveJoints col_joint1;
static BmdfootFourJoints col_joint2;
static dCcD_SrcSph cc_sph_src;
static GXColor g_whiteColor;
static BmdfootLight l_defaultLight;
static BmdfootArchiveName attack_archive;

/* dComIfGp_particle_set with a group and a setup word (setToon: group 2, the room number) */
static inline JPABaseEmitter* particle_set_grp(u8 grp, u16 id, const cXyz* pos, const csXyz* angle, u8 alpha,
                                               void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, grp, id, pos, angle, nullptr, alpha, (dPa_levelEcallBack*)cb, setup, nullptr,
                           nullptr, nullptr);
}
static inline void se_start(fopAc_ac_c* a, u32 id, cXyz* pos) {
    mDoAud_seStart(id, pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline s32 morf_frame(mDoExt_McaMorf* m) { return gabi::ftoi(m->getFrame()); }

// HD: process, sound and particle IDs differ from GameCube.
enum {
    ID_AK_ST_BKMLATTACKSMOKE01 = 0xA0FC,
    ID_AK_SN_BKMASIWAIT00 = 0x80ED,
    JA_SE_CM_BKM_ATKVINE_PREP = 0x5851,
    JA_SE_CM_BKM_ATKVINE_ATTACK = 0x5852,
    ID_AK_SN_O_BKMSATTACKHOUSHI00 = 0x8065,
    ID_AK_SN_O_BKMSATTACKSOIL00 = 0x8066,
    ID_AK_ST_O_BKMSATTACKSMOKE00 = 0xA06A,
    JA_SE_CM_BKM_ATKVINE_IN_G = 0x7811,
    JA_SE_CM_BKM_ATKVINE_DIG = 0x5053,
    JA_SE_CM_BKM_ATKVINE_OUT_G = 0x7812,
    JA_SE_CM_BKM_ATKVINE_L_ATK = 0x5054,
    ID_AK_SN_BKMLATTACKHOUSHI00 = 0x80F9,
    ID_AK_SN_BKMLATTACKSOIL00 = 0x80FA,
    ID_AK_ST_BKMLATTACKSMOKE00 = 0xA0FB,
    JA_SE_CM_BKM_ATKVINE_IN_G2 = 0x7813,
    JA_SE_CM_BKM_ATKVINE_OUT_G2 = 0x7814,
    JA_SE_CM_BKM_ATKVINE_MOVE_1 = 0x584D,
    JA_SE_CM_BKM_ATKVINE_MOVE_2 = 0x5850,
    fpcNm_BMD_e = 0xEB,
};

enum BmdfootAction {
    Bmdfoot_Wait = 0,
    Bmdfoot_Attack1 = 1,
    Bmdfoot_Attack2 = 2,
    Bmdfoot_Damage = 3,
    Bmdfoot_Start = 10,
    Bmdfoot_End = 11,
};

// HD: the joint index is checked before accessing its model.
/* 020B5580 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = Bmdfoot_jointNumber(joint);
        if (jntNo >= 0x12) { /* JUT_ASSERT(0x120, jntNo < 18) */
            JUT_ASSERT_fail("d_a_bmdfoot.cpp", 0x120, "jntNo < 18");
            return TRUE;
        }
        J3DModel* model = j3dSys.getModel();                                       /* j3dSys.getModel() */
        bmdfoot_class* i_this = static_cast<bmdfoot_class*>(model->getUserArea()); /* getUserArea */
        if (i_this != nullptr) {
            PSMTXCopy(Bmdfoot_getAnmMtx(model, jntNo), calc_mtx());
            MtxRotX(i_this->m2CC[jntNo].x, 1);
            MtxRotZ(i_this->m2CC[jntNo].z, 1);
            mtx_copy(Bmdfoot_getAnmMtx(model, jntNo), calc_mtx()); /* setAnmMtx */
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx /* J3DSys::mCurrentMtx */);
        }
    }
    return TRUE;
}

/* 020B56CC */
static BOOL daBmdfoot_Draw(bmdfoot_class* i_this) {

    fopAc_ac_c* actor = i_this;
    if (i_this->mBC8 != 0) {
        J3DModel* model = i_this->mpBodyVineMorf->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
        mDoExt_btkAnm* btk = i_this->btk;
        btk->entry(model->getModelData(), btk->getFrame());
        i_this->mpBodyVineMorf->entryDL();
        if (i_this->mBA8 >= 10) {
            model = i_this->mpFloorVineMorf->getModel();
            settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->mBAC, &i_this->mTevstr);
            setLightTevColorType(dKy_getEnvlight(), model, &i_this->mTevstr);
            i_this->mpFloorVineMorf->entryDL();
        }
    }
    return TRUE;
}

/* 020B5790 */
static void anm_init(bmdfoot_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
                     int soundFileIdx) {

    if (soundFileIdx >= 0) {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Bmdfoot", bckFileIdx);
        void* snd = dComIfG_getObjectRes("Bmdfoot", soundFileIdx);
        i_this->mpBodyVineMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("Bmdfoot", bckFileIdx);
        i_this->mpBodyVineMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 020B58B8 */
static void housi_off(bmdfoot_class* i_this) {

    for (int i = 0; i < 3; i++) {
        if (i_this->mAsiWaitFollowCB[i].getEmitter() != nullptr) {
            emitter_stopCreate(i_this->mAsiWaitFollowCB[i].getEmitter());
        }
    }
    if (i_this->mLAttackSmoke01CB.getEmitter() != nullptr) {
        Bmdfoot_smokeEnd(&i_this->mLAttackSmoke01CB); /* remove() */
    }
}

/* ---- move() and its states, inlined into daBmdfoot_Execute ---- */
static inline void wait(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    int frame = morf_frame(i_this->mpBodyVineMorf);
    gabi::Local<cXyz> local_98;
    local_98->set(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; i++) {
        PSMTXCopy(Bmdfoot_getAnmMtx(i_this->mpBodyVineMorf->getModel(), eff_id.values[i]), calc_mtx());
        MtxPosition(local_98, &i_this->m3F8[i]);
        if (i_this->m3F4 == 0) {
            particle_set_grp(0, ID_AK_SN_BKMASIWAIT00, &i_this->m3F8[i], nullptr, 0xFF,
                             &i_this->mAsiWaitFollowCB[i], -1);
        }
        if (i_this->mAsiWaitFollowCB[i].getEmitter() != nullptr) {
            if (frame == 0) {
                emitter_playCreate(i_this->mAsiWaitFollowCB[i].getEmitter());
            } else if (frame == 20) {
                emitter_stopCreate(i_this->mAsiWaitFollowCB[i].getEmitter());
            }
        }
    }
    i_this->m3F4 = 1;

    switch (i_this->m2BC) {
    case -1:
        for (int i = 2; i <= 16; i++) {
            cLib_addCalc2(&i_this->m2CC[i].x, (f32)(0x10 - i) * 0.003f, 0.1f, 0.008f);
            i_this->m3A4[i] = 0.0f;
        }
        if (i_this->m2C0[2] == 0) {
            housi_off(i_this);
            i_this->m2BA = Bmdfoot_Attack1;
            i_this->m2BC = 0;
        }
        break;
    case 0:
        i_this->m2BC = i_this->m2BC + 1;
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e, 50.0f, J3DFrameCtrl::EMode_LOOP,
                 cM_rndF(0.2f) + 0.9f, -1);
        i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(150.0f) + 100.0f);
        i_this->m3EC = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        for (int i = 2; i <= 16; i++) {
            i_this->m3A4[i] = cM_rndFX(0.1f) + 0.2f;
        }
        break;
    case 1:
        i_this->m3EC = (s16)(i_this->m3EC + 0x200);
        if (boss->m332 == 1) {
            s16 pa = fopAcM_searchPlayerAngleY(actor);
            s16 sVar2 = (s16)(actor->current.angle.y - pa);
            if (sVar2 < 0) {
                sVar2 = -sVar2;
            }
            if (sVar2 < l_HIO.m06) {
                housi_off(i_this);
                i_this->m2BA = Bmdfoot_Attack1;
                i_this->m2BC = 0;
            } else {
                i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            }
        } else if (boss->m332 == 2) {
            housi_off(i_this);
            i_this->m2BA = Bmdfoot_Attack2;
            i_this->m2BC = 0;
        }
        break;
    }
}

static inline void sph_set(dCcD_Sph* sph, cXyz* c, f32 r) {
    sph->SetC(c);
    sph->SetR(r);
}

static inline void attack_1(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_5c;
    gabi::Local<cXyz> cStack_68;
    bool bVar2 = false;
    local_5c->set(0.0f, 0.0f, 0.0f);

    switch ((u16)i_this->m2BC) {
    case 0:
        i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(20.0f));
        i_this->m2BC = i_this->m2BC + 1;
        // fallthrough
    case 1:
        i_this->mBC4 = 0.0f;
        if (i_this->m2C0[0] == 0) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK1_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mBD0 = 0x1e;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 2:
        if (i_this->mBD0 == 0) {
            i_this->mBD0 = 0x1e;
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK_LOOP_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     -1);
            i_this->m2C0[0] = 0x3c;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 3:
        if (i_this->mBD0 == 0) {
            i_this->mBD0 = 0x1e;
        }
        for (int i = 2; i <= 16; i++) {
            i_this->m3A4[i] = 0.0f;
            cLib_addCalc0(&i_this->m2CC[i].z, 0.1f, 0.05f);
        }
        if (i_this->m2C0[0] == 0) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK2_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->m2BC = i_this->m2BC + 1;
            i_this->m2C0[0] = 0x1c;
            se_start(actor, JA_SE_CM_BKM_ATKVINE_PREP, &i_this->mAE8);
        }
        break;
    case 4:
        bVar2 = true;
        if (i_this->m2C0[0] == 1) {
            se_start(actor, JA_SE_CM_BKM_ATKVINE_ATTACK, &i_this->mAE8);
            for (int i = 0; i < 3; i++) {
                PSMTXCopy(Bmdfoot_getAnmMtx(i_this->mpBodyVineMorf->getModel(), jno.values[i]), calc_mtx());
                MtxPosition(local_5c, cStack_68);
                dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKHOUSHI00, cStack_68, 0xFF, &g_whiteColor,
                                            &g_whiteColor, 0);
            }
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKSOIL00, &i_this->mAE8, 0xFF, &g_whiteColor,
                                        &g_whiteColor, 0);
            dComIfGp_particle_setSimple(ID_AK_ST_O_BKMSATTACKSMOKE00, &i_this->mAE8, 0xB9, &g_whiteColor,
                                        &g_whiteColor, 0);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            i_this->m2BA = Bmdfoot_Wait;
            i_this->m2BC = 0;
        }
        break;
    }

    s16 pa = fopAcM_searchPlayerAngleY(actor);
    s16 sVar3 = (s16)(pa - actor->current.angle.y);
    s16 m06 = l_HIO.m06;
    s16 sVar5;
    if (sVar3 > m06) {
        sVar5 = m06;
    } else if (sVar3 < (s16)-m06) {
        sVar5 = (s16)-m06;
    } else {
        sVar5 = sVar3;
    }
    cLib_addCalcAngleS2(&i_this->m2BE, sVar5, 4, (s16)gabi::ftoi(i_this->mBC4 + 128.0f));
    cLib_addCalc2(&i_this->mBC4, 10000.0f, 1.0f, 20.0f);

    if (bVar2) {
        J3DModel* model = i_this->mpBodyVineMorf->getModel();
        for (int i = 0; i < 5; i++) {
            PSMTXCopy(Bmdfoot_getAnmMtx(model, (i_this->m2B8 & 7U) + col_joint1.values[i]), calc_mtx());
            MtxPosition(local_5c, cStack_68);
            sph_set(&i_this->mSph[i], cStack_68, 40.0f);
            i_this->mSph[i].OnAtSPrmBit(1);  /* OnAtSetBit */
            i_this->mSph[i].OffCoSPrmBit(1); /* OffCoSetBit */
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
    }
}

/* 020B5904 */
static int ug_move(bmdfoot_class* i_this) {

    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 uVar4 = 0;
    switch (i_this->mBA8) {
    case 1: {
        i_this->mBAC.copy(actor->current.pos);
        i_this->mBAC.y = boss->m328;
        i_this->mBB8.y = actor->current.angle.y;
        i_this->mBC0 = 0.0f;
        i_this->mBA8 = 2;
        i_this->m2B8 = (s16)gabi::ftoi(cM_rndF(10000.0f));
        s8 room = actor->current.roomNo;
        particle_set_grp(2, ID_AK_ST_BKMLATTACKSMOKE01, &i_this->mBAC, &i_this->mBB8, 0xB9,
                         &i_this->mLAttackSmoke01CB, room); /* setToon */
        break;
    }
    case 2: {
        gabi::Local<cXyz> diff;
        cXyz_mi(&player->current.pos, diff, &i_this->mBAC);
        gabi::Local<cXyz> local_64;
        local_64->copy(*diff);
        f32 dVar5 = std_sqrtf(PSVECSquareMag(local_64)) * 10.0f;
        if (dVar5 > 10000.0f) {
            dVar5 = 10000.0f;
        }
        f32 dVar6 = cM_ssin(i_this->m2B8 * 2000);
        s16 r28 = (s16)gabi::ftoi(dVar6 * dVar5);
        s16 r4 = (s16)(r28 + cM_atan2s(local_64->x, local_64->z));
        cLib_addCalcAngleS2(&i_this->mBB8.y, r4, 0x10, 0x2000);
        gabi::Local<cXyz> local_4c;
        local_4c->y = 0.0f;
        local_4c->x = 0.0f;
        local_4c->z = 20.0f;
        cMtx_YrotS(calc_mtx(), i_this->mBB8.y);
        gabi::Local<cXyz> cStack_58;
        MtxPosition(local_4c, cStack_58);
        PSVECAdd(&i_this->mBAC, cStack_58, &i_this->mBAC);
        i_this->mBC0 = i_this->mBC0 + local_4c->z;
        if (std_sqrtf(PSVECSquareMag(local_64)) < 300.0f || i_this->mBC0 > 3000.0f) {
            i_this->mBA8 = 10;
            if (i_this->mLAttackSmoke01CB.getEmitter() != nullptr) {
                Bmdfoot_smokeEnd(&i_this->mLAttackSmoke01CB);
            }
            uVar4 = 1;
        }
        break;
    }
    default:
        break;
    }
    MtxTrans(i_this->mBAC.x, i_this->mBAC.y, i_this->mBAC.z, false);
    cMtx_YrotM(calc_mtx(), i_this->mBB8.y);
    J3DModel* model = i_this->mpFloorVineMorf->getModel();
    model->setBaseTRMtx(calc_mtx());
    i_this->mpFloorVineMorf->play(nullptr, 0, 0);
    i_this->mpFloorVineMorf->calc();
    return uVar4;
}

static inline void attack_2(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_3c;
    gabi::Local<cXyz> local_48;
    local_3c->x = local_3c->y = local_3c->z = 0.0f;
    boss->m334 = 10;
    switch ((u16)i_this->m2BC) {
    case 0:
        i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(20.0f));
        i_this->m2BC = i_this->m2BC + 1;
        // fallthrough
    case 1:
        i_this->mBC4 = 0.0f;
        if (i_this->m2C0[0] == 0) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_UMARU_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->m2BC = i_this->m2BC + 1;
            i_this->m2C0[0] = 5;
            i_this->mBD0 = 0x1e;
        }
        break;
    case 2:
        if (morf_frame(i_this->mpBodyVineMorf) == 49) {
            PSMTXCopy(Bmdfoot_getAnmMtx(i_this->mpBodyVineMorf->getModel(), 0x11 /* ASI18 */), calc_mtx());
            MtxPosition(local_3c, local_48);
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKHOUSHI00, local_48, 0xFF, &g_whiteColor,
                                        &g_whiteColor, 0);
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKSOIL00, local_48, 0xFF, &g_whiteColor,
                                        &g_whiteColor, 0);
            dComIfGp_particle_setSimple(ID_AK_ST_O_BKMSATTACKSMOKE00, local_48, 0xB9, &g_whiteColor,
                                        &g_whiteColor, 0);
            se_start(actor, JA_SE_CM_BKM_ATKVINE_IN_G, &i_this->mAE8);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_UMARU_WAIT_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     -1);
            i_this->mBA8 = 1;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 3:
        se_start(actor, JA_SE_CM_BKM_ATKVINE_DIG, &i_this->mBAC);
        if (ug_move(i_this)) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(
                attack_archive.name, dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK1_e);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_NONE, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 4:
        if (morf_frame(i_this->mpFloorVineMorf) == 2) {
            local_48->copy(i_this->mBAC);
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKHOUSHI00, local_48, 0xFF, &g_whiteColor,
                                        &g_whiteColor, 0);
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMSATTACKSOIL00, local_48, 0xFF, &g_whiteColor,
                                        &g_whiteColor, 0);
            dComIfGp_particle_setSimple(ID_AK_ST_O_BKMSATTACKSMOKE00, local_48, 0xB9, &g_whiteColor,
                                        &g_whiteColor, 0);
            se_start(actor, JA_SE_CM_BKM_ATKVINE_OUT_G, &i_this->mBAC);
        }
        ug_move(i_this);
        if (i_this->mpFloorVineMorf->isStop()) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(
                attack_archive.name, dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK2_e);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_LOOP, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2C0[0] = 0xb4;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 5:
        ug_move(i_this);
        se_start(actor, JA_SE_CM_BKM_ATKVINE_L_ATK, &i_this->mBAC);
        for (int i = 0; i < 4; i++) {
            PSMTXCopy(Bmdfoot_getAnmMtx(i_this->mpFloorVineMorf->getModel(), col_joint2.values[i]),
                      calc_mtx());
            MtxPosition(local_3c, local_48);
            sph_set(&i_this->mSph[i], local_48, 40.0f);
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
        if (i_this->m2C0[0] == 0) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(
                attack_archive.name, dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK3_e);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_NONE, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 6:
        ug_move(i_this);
        if (morf_frame(i_this->mpFloorVineMorf) == 12) {
            dComIfGp_particle_set(ID_AK_SN_BKMLATTACKHOUSHI00, &i_this->mBAC);
            dComIfGp_particle_set(ID_AK_SN_BKMLATTACKSOIL00, &i_this->mBAC);
            i_this->m498[0].copy(i_this->mBAC);
            s8 room = actor->current.roomNo;
            particle_set_grp(2, ID_AK_ST_BKMLATTACKSMOKE00, &i_this->m498[0], nullptr, 0xB9,
                             &i_this->mLAttackSmoke00CB[0], room);
            se_start(actor, JA_SE_CM_BKM_ATKVINE_IN_G2, &i_this->mBAC);
        }
        if (i_this->mpFloorVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_NUKU_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mBA8 = 0;
            i_this->m2BC = i_this->m2BC + 1;
            se_start(actor, JA_SE_CM_BKM_ATKVINE_OUT_G2, &i_this->mAE8);
        }
        break;
    case 7:
        if (morf_frame(i_this->mpBodyVineMorf) == 13) {
            local_48->copy(actor->current.pos);
            local_48->y = i_this->mBAC.y;
            dComIfGp_particle_set(ID_AK_SN_BKMLATTACKHOUSHI00, local_48);
            dComIfGp_particle_set(ID_AK_SN_BKMLATTACKSOIL00, local_48);
            s8 room = actor->current.roomNo;
            i_this->m498[1].copy(*local_48);
            particle_set_grp(2, ID_AK_ST_BKMLATTACKSMOKE00, &i_this->m498[1], nullptr, 0xB9,
                             &i_this->mLAttackSmoke00CB[1], room);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            i_this->m2BA = Bmdfoot_Wait;
            i_this->m2BC = 0;
        }
    }
    PSMTXCopy(Bmdfoot_getAnmMtx(i_this->mpBodyVineMorf->getModel(), 6 /* ASI5 */), calc_mtx());
    MtxPosition(local_3c, local_48);
    sph_set(&i_this->mSph[4], local_48, 50.0f);
    i_this->mSph[4].OffAtSPrmBit(1); /* OffAtSetBit */
    i_this->mSph[4].OnCoSPrmBit(1);  /* OnCoSetBit */
    dComIfG_Ccsp_Set(&i_this->mSph[4]);
}

static inline void damage(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch ((u16)i_this->m2BC) {
    case 0:
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_NOBIKIRU_e, 40.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m2BC = i_this->m2BC + 1;
        i_this->m2C0[0] = 0x1e;
        i_this->m3EC = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        for (int i = 2; i <= 16; i++) {
            i_this->m3A4[i] = cM_rndFX(0.2f) + 0.3f;
        }
    case 1:
        if (i_this->m2C0[0] != 0) {
            f32 dVar5 = (f32)(fopAcM_GetParam(actor) & 0xf) * 0.01f;
            for (int i = 2; i <= 16; i++) {
                f32 reg = REG_F(14, 4);
                cLib_addCalc2(&i_this->m2CC[i].x, -(gabi::fmadds((f32)i, 0.03f, 0.05f) + dVar5), 0.1f,
                              reg + 0.1f);
            }
        }
        if (boss->m331 > 0) {
            i_this->m2BA = Bmdfoot_Wait;
            i_this->m2BC = -1;
            i_this->m2C0[2] = 0x14;
        }
        break;
    }
}

static inline void start(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    for (int i = 2; i <= 16; i++) {
        i_this->m2CC[i].z = 0.0f;
        i_this->m3A4[i] = 0.0f;
    }
    switch ((u16)i_this->m2BC) {
    case 0:
        if (boss->m332 == 6) {
            i_this->m2BC = 1;
            i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(10.0f));
        }
        break;
    case 1:
        if (i_this->m2C0[0] == 0) {
            i_this->mBC8 = 1;
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_START1_e, 1.0f, J3DFrameCtrl::EMode_NONE,
                     cM_rndF(0.2f) + 0.9f, -1);
            i_this->m2BC = 2;
        }
        break;
    case 2:
        if (i_this->mpBodyVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_START2_e, 30.0f, J3DFrameCtrl::EMode_LOOP,
                     cM_rndF(0.2f) + 0.9f, -1);
            i_this->m2BC = 3;
        }
        break;
    case 3:
        if (morf_frame(i_this->mpBodyVineMorf) == 2) {
            se_start(actor, JA_SE_CM_BKM_ATKVINE_MOVE_1, &i_this->mAE8);
        }
        if (boss->m332 == 7) {
            i_this->m2BA = Bmdfoot_Wait;
            i_this->m2BC = 0;
        }
        break;
    }
}

static inline void end(bmdfoot_class* i_this) {
    for (int i = 2; i <= 16; i++) {
        i_this->m2CC[i].z = 0.0f;
        i_this->m3A4[i] = 0.0f;
    }
    if (boss->mB76 == 2) {
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_LOOP_e, 30.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
    }
    if (boss->mB76 == 0x17c) {
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_e, 30.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
    }
}

static inline void move(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (boss != nullptr) {
        s16 sVar1 = (s16)((fopAcM_GetParam(actor) & 0xF) << 13);
        cMtx_YrotS(calc_mtx(), boss->shape_angle.y);
        cMtx_XrotM(calc_mtx(), boss->shape_angle.x);
        cMtx_ZrotM(calc_mtx(), boss->shape_angle.z);
        cMtx_YrotM(calc_mtx(), sVar1);
        actor->current.angle.y = (s16)(sVar1 + boss->shape_angle.y);
        gabi::Local<cXyz> local_20;
        local_20->x = 0.0f;
        local_20->y = 0.0f;
        local_20->z = 180.0f;
        gabi::Local<cXyz> cStack_2c;
        MtxPosition(local_20, cStack_2c);
        gabi::Local<cXyz> sum;
        cXyz_pl(&boss->current.pos, sum, cStack_2c);
        actor->current.pos.copy(*sum);
        switch ((u16)i_this->m2BA) {
        case Bmdfoot_Wait:
            wait(i_this);
            break;
        case Bmdfoot_Attack1:
            attack_1(i_this);
            break;
        case Bmdfoot_Attack2:
            attack_2(i_this);
            break;
        case Bmdfoot_Damage:
            damage(i_this);
            break;
        case Bmdfoot_Start:
            start(i_this);
            break;
        case Bmdfoot_End:
            end(i_this);
            break;
        }
        if (boss->m332 == 3) {
            housi_off(i_this);
            i_this->mBA8 = 0;
            i_this->m2BA = Bmdfoot_Damage;
            i_this->m2BC = 0;
        }
        /* HD: for every state (GameCube: in the damage state only), and only once */
        if (boss->m332 == 8 && i_this->m2BA != Bmdfoot_End) {
            i_this->m2BA = Bmdfoot_End;
            i_this->m2BC = 0;
        }
    }
}

/* 020B5D74 */
static void* s_a_d_sub(void* search, void* param_2) {

    if (fopAcM_IsActor(search) && search != nullptr &&
        static_cast<fopAc_ac_c*>(search)->mProcName == fpcNm_BMD_e) {
        return search;
    }
    return nullptr;
}

/* 020B5DC4 */
static BOOL daBmdfoot_Execute(bmdfoot_class* i_this) {

    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused accessor call */
    if (boss == nullptr) {
        boss = static_cast<BmdfootBoss*>(
            fpcM_Search(gabi::guest_fn(reinterpret_cast<const void*>(s_a_d_sub)), i_this));
    }
    i_this->m2B8 = i_this->m2B8 + 1;
    for (int i = 0; i < 4; i++) {
        if (i_this->m2C0[i] != 0) {
            i_this->m2C0[i] = i_this->m2C0[i] - 1;
        }
    }
    if (i_this->m2C8[0] != 0) {
        i_this->m2C8[0] = i_this->m2C8[0] - 1;
    }
    move(i_this);
    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), (s16)(actor->current.angle.y + i_this->m2BE));
    cMtx_XrotM(calc_mtx(), actor->current.angle.x);
    cMtx_ZrotM(calc_mtx(), (s16)(actor->current.angle.z + i_this->m2BE * 2));
    J3DModel* model = i_this->mpBodyVineMorf->getModel();
    model->setBaseTRMtx(calc_mtx());
    i_this->mpBodyVineMorf->play(nullptr, 0, 0);
    i_this->mpBodyVineMorf->calc();
    if (i_this->mBD0 != 0) {
        if (i_this->mBD0 == 0x1e) {
            se_start(actor, JA_SE_CM_BKM_ATKVINE_MOVE_2, &i_this->mAE8);
        }
        i_this->mBD0 = i_this->mBD0 + -1;
    }
    s16 sVar1 = (s16)(29 - i_this->mBD0);
    if (sVar1 >= 30) {
        sVar1 = (s16)(sVar1 % 30);
    }
    i_this->btk->mFrameCtrl.setFrame((f32)sVar1);
    PSMTXCopy(Bmdfoot_getAnmMtx(model, 0x10 /* ASI17 */), calc_mtx());
    gabi::Local<cXyz> local_68;
    local_68->x = local_68->y = local_68->z = 0.0f;
    MtxPosition(local_68, &i_this->mAE8);
    if (boss != nullptr && i_this->mAE8.y < boss->m328) {
        i_this->mAE8.y = boss->m328;
    }
    cLib_addCalcAngleS2(&i_this->m2BE, 0, 0x10, 0x80);
    s16 sVar1_2;
    if (i_this->m2BA < 3) {
        sVar1_2 = l_HIO.m08;
    } else {
        sVar1_2 = 0;
    }
    cLib_addCalcAngleS2(&actor->current.angle.x, sVar1_2, 0x10, 0x80);
    for (int i = 2; i <= 16; i++) {
        f32 f1 = cM_ssin(i_this->m3EC + i * 9000) * i_this->m3A4[i];
        cLib_addCalc2(&i_this->m2CC[i].z, f1, 0.05f, 0.01f);
        cLib_addCalc0(&i_this->m2CC[i].x, 0.05f, 0.005f);
    }
    return TRUE;
}

/* 020B79BC */
static BOOL daBmdfoot_IsDelete(bmdfoot_class*) { return TRUE; }

/* 020B79C4 */
static BOOL daBmdfoot_Delete(bmdfoot_class* i_this) {

    dComIfG_resDelete(&i_this->mPhase, "Bmdfoot"); /* dComIfG_resDeleteDemo */
    if (i_this->mBD2 != 0) {
        s8 no = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(no);
    }
    mDoAud_seDeleteObject(&i_this->mAE8);
    mDoAud_seDeleteObject(&i_this->mBAC);
    for (int i = 0; i < 3; i++) {
        Bmdfoot_remove(&i_this->mAsiWaitFollowCB[i]);
    }
    Bmdfoot_remove(&i_this->mLAttackSmoke00CB[0]);
    Bmdfoot_remove(&i_this->mLAttackSmoke00CB[1]);
    Bmdfoot_remove(&i_this->mLAttackSmoke01CB);
    boss = nullptr;
    return TRUE;
}

// HD: useHeapInit also serves as the solid-heap callback.
/* 020B7AB8 */
static BOOL useHeapInit(bmdfoot_class* i_this) {

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Bmdfoot", dRes_INDEX_BMDFOOT_BMD_ASI_e);
    J3DAnmTransform* anm =
        (J3DAnmTransform*)dComIfG_getObjectRes("Bmdfoot", dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e);
    i_this->mpBodyVineMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               cM_rndF(0.2f) + 0.9f, 0, -1, 1, nullptr, 0, 0x11020203);
    /* HD: the morf is checked before its model */
    if (i_this->mpBodyVineMorf == nullptr) {
        return FALSE;
    }
    J3DModel* model = i_this->mpBodyVineMorf->getModel();
    if (model == nullptr) {
        return FALSE;
    }

    i_this->mpBodyVineMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(i_this)); /* setUserArea */
    /* the joint count comes from 027F3F94 (the model data's joint tree), re-read every iteration */
    for (u16 i = 0; i < model->getModelData()->getJointNum(); i++) {
        // HD: load the joint pointer before the bounds-check count.
        J3DJoint* joint = Bmdfoot_jointNode(model->getModelData(), i);
        joint->setCallBack(nodeCallBack);
    }

    mDoExt_btkAnm* btk = (mDoExt_btkAnm*)operator_new(0x74);
    if (btk != nullptr) {
        btk = Bmdfoot_btkCtor(btk); /* mDoExt_btkAnm::mDoExt_btkAnm */
    }
    i_this->btk = btk;
    if (btk == nullptr) /* JUT_ASSERT(1450, i_this->btk) */
        JUT_ASSERT_fail("d_a_bmdfoot.cpp", 0x5AA, "i_this->btk");
    J3DAnmTextureSRTKey* pBtk =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes("Bmdfoot", dRes_INDEX_BMDFOOT_BTK_ASI_e);
    J3DModelData* md = model->getModelData();
    if (!i_this->btk->init(md, pBtk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes("Bmdfoot", dRes_INDEX_BMDFOOT_BMD_ASI_e);
    anm = (J3DAnmTransform*)dComIfG_getObjectRes("Bmdfoot", dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e);
    i_this->mpFloorVineMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               cM_rndF(0.2f) + 0.9f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (i_this->mpFloorVineMorf->getModel() == nullptr) {
        return FALSE;
    }
    return TRUE;
}

static bmdfoot_class* bmdfoot_class_ct(bmdfoot_class* self);

/* 020B7FC8 */
static cPhs_State daBmdfoot_Create(fopAc_ac_c* a_this) {

    /* fopAcM_ct(a_this, bmdfoot_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            bmdfoot_class_ct((bmdfoot_class*)a_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    bmdfoot_class* i_this = (bmdfoot_class*)a_this;
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, "Bmdfoot");
    if (res == cPhs_ERROR_e) {
        return cPhs_ERROR_e;
    } else if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    i_this->mLAttackSmoke00CB[0].m12 = 1; /* setFollowOff */
    i_this->mLAttackSmoke00CB[1].m12 = 1;
    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x5040)) {
        return cPhs_ERROR_e;
    }
    if (hio_set == 0) {
        hio_set = 1;
        i_this->mBD2 = 1;
        l_HIO.mNo = mDoHIO_createChild("森ボス足", &l_HIO);
    }
    a_this->health = 2;
    i_this->m2B8 = (s16)gabi::ftoi(cM_rndF(10000.0f));
    boss = nullptr;
    i_this->mStts.Init(0xFF, 0, a_this);
    for (int i = 0; i < 5; i++) {
        i_this->mSph[i].SetStts(&i_this->mStts);
        i_this->mSph[i].Set(&cc_sph_src);
    }
    a_this->attention_info.position.y = -20000.0f; /* attention_info.position.y */
    a_this->eyePos.y = -20000.0f;
    if (!dComIfGs_isStageBossDemo() &&
        *reinterpret_cast<const be<u8>*>(dComIfGp_getStartStageName()) != 'X') {
        i_this->m2BA = Bmdfoot_Start;
    } else {
        i_this->mBC8 = 1;
    }
    Bmdfoot_copyTev(Bmdfoot_tev(i_this->mTevstr), Bmdfoot_tev(a_this->tevStr));
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated constructor/destructor functions of this translation unit.
/* 020B7D80 */
static void follow_ct(dPa_followEcallBack* p) { dPa_followEcallBack_ct(p, 0, 0); }

/* 020B7D8C */
static u32 smoke_elem_ct(dPa_smokeEcallBack* p) {

    return Bmdfoot_smokeCtor(p, (u8)1); /* smoke_ct: dPa_smokeEcallBack ctor, returns this */
}

/* 020B7D94 */
static bmdfoot_class* bmdfoot_class_ct(bmdfoot_class* self) {

    if (self == nullptr) {
        self = (bmdfoot_class*)operator_new(0xE08);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = BmdfootVtable_Actor;
    Bmdfoot_constructArray(self->mAsiWaitFollowCB, 3, 0x14, BmdfootEntry_FollowCtor);
    Bmdfoot_constructArray(self->mLAttackSmoke00CB, 2, 0x20, BmdfootEntry_SmokeCtor);
    Bmdfoot_smokeCtor(&self->mLAttackSmoke01CB, 1);
    Bmdfoot_statusCtor(&self->mStts);                    /* cCcD_Stts */
    Bmdfoot_globalStatusCtor(&self->mStts.__vtbl_gstts); /* dCcD_GStts */
    self->mStts.__vtbl = BmdfootVtable_Status;
    self->mStts.__vtbl_gstts = BmdfootVtable_GlobalStatus;
    Bmdfoot_constructArray(self->mSph, 5, 0x12C, BmdfootEntry_SphereCtor /* dCcD_Sph::dCcD_Sph */);
    Bmdfoot_constructTev(Bmdfoot_tev(self->mTevstr), l_defaultLight);
    return self;
}

/* 020B8458 */
static daBmdfoot_HIO_c* daBmdfoot_HIO_c_ct(daBmdfoot_HIO_c* h) {

    if (h == nullptr) {
        h = (daBmdfoot_HIO_c*)operator_new(0xC);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->__vtbl = BmdfootVtable_Hio;
    h->m06 = 3500;
    h->m08 = 1000;
    return h;
}

/* 020B84B0 */
static void __sinit_d_a_bmdfoot_cpp() {

    __sinit_header_statics();
    daBmdfoot_HIO_c_ct(&l_HIO);
}

/* 020B8550 */
static void trivial_dt1(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 020B8564 */
static void trivial_dt2(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 020B8578 */
static void trivial_dt3(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 020B858C */
static void bmdfoot_class_dt(bmdfoot_class* i_this, s32 flags) {

    if (i_this != nullptr) {
        Bmdfoot_destroyArray(i_this->mSph, 5, 0x12C, BmdfootEntry_SphereDtor /* dCcD_Sph::~dCcD_Sph */, 0, 0);
        Bmdfoot_statusDtor(&i_this->mStts, 2); /* dCcD_Stts::~dCcD_Stts */
        Bmdfoot_destroyArray(i_this->mLAttackSmoke00CB, 2, 0x20, BmdfootEntry_SmokeDtor, 0, 0);
        Bmdfoot_destroyArray(i_this->mAsiWaitFollowCB, 3, 0x14, BmdfootEntry_FollowDtor, 0, 0);
        Bmdfoot_actorDtor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 020B864C */
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue
data boss 104624D8
data hio_set 10191B1C
data l_HIO 104624F8
data eff_id 10191B50
data jno 10191B5C
data col_joint1 10191B68
data col_joint2 10191B40
data cc_sph_src 10191B7C
data g_whiteColor 101D5E98
data l_defaultLight 1016E414
data attack_archive 10009D80
str 10009E08 "Bmdfoot" in 020B5790
str 10009D88 "Bmdfoot" in 020B7AB8
str 10009E88 "Bmdfoot" in 020B79C4
str 10009E94 "Bmdfoot" in 020B7FC8
str 10009E9C "\u68ee\u30dc\u30b9\u8db3" renamed # HD: original HIO label is Shift-JIS.
str 10009DF0 "d_a_bmdfoot.cpp" renamed in 020B5580
str 10009DE8 "jntNo < 18" renamed
str 10009DCC "d_a_bmdfoot.cpp" renamed in 020B7AB8
str 10009DDC "i_this->btk" renamed
ext 028EFFD0 void Bmdfoot_constructArray(void*, u32, u32, u32)
ext 028F0164 void Bmdfoot_destroyArray(void*, u32, u32, u32, u32, u32)
ext 025A5B18 u32 Bmdfoot_smokeCtor(dPa_smokeEcallBack*, u8)
ext 0200BD2C void Bmdfoot_statusCtor(dCcD_Stts*)
ext 02515DA0 void Bmdfoot_globalStatusCtor(void*)
ext 02515860 void Bmdfoot_statusDtor(dCcD_Stts*, s32)
ext 025D50BC void Bmdfoot_actorDtor(fopAc_ac_c*, s32)
ext 025E7C6C mDoExt_btkAnm* Bmdfoot_btkCtor(mDoExt_btkAnm*)
*/
// clang-format on
