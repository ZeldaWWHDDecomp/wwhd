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

#define SAFESTRING_VTBL 0x10009D94
#define BMDFOOT_ARC STR(0x10009E08) /* "Bmdfoot" */
#define BMDFOOT_VTBL 0x10009DBC
#define WHITE gabi::at<GXColor>(0x101D5E98) /* g_whiteColor */

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

/* ---- statics ---- */
static inline u32 boss_ea() { return gabi::load<u32>(0x104624D8); }
static inline void set_boss(u32 p) { gabi::store<u32>(0x104624D8, p); }
static inline be<u8>& hio_set() { return *gabi::at<be<u8>>(0x10191B1C); }
static inline daBmdfoot_HIO_c& l_HIO() { return *gabi::at<daBmdfoot_HIO_c>(0x104624F8); }
static inline s32 tbl(u32 a, s32 i) { return gabi::load<s32>(a + 4 * i); }
#define eff_id 0x10191B50          /* wait: ASI11, ASI13, ASI18 */
#define jno 0x10191B5C             /* attack_1 */
#define col_joint1 0x10191B68      /* attack_1: ASI2..ASI10 */
#define col_joint2 0x10191B40      /* attack_2: ASI18, ASI16, ASI13, ASI10 */
#define cc_sph_src gabi::at<dCcD_SrcSph>(0x10191B7C)

/* bmd_class (Kalle Demos): the fields the feet use (HD offsets) */
static inline s16 boss_shape_angle(u32 b, int i) { return gabi::load<s16>(b + 0x328 + 2 * i); }
static inline cXyz* boss_pos(u32 b) { return gabi::at<cXyz>(b + 0x314); }
static inline f32 boss_m328(u32 b) { return gabi::load<f32>(b + 0x448); }
static inline s8 boss_m331(u32 b) { return gabi::load<s8>(b + 0x451); }
static inline s8 boss_m332(u32 b) { return gabi::load<s8>(b + 0x452); }
static inline void boss_set_m334(u32 b, s16 v) { gabi::store<s16>(b + 0x454, v); }
static inline s16 boss_mB76(u32 b) { return gabi::load<s16>(b + 0xC96); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025166F0 dCcD_Sph::dCcD_Sph (as in d_a_bmdhand) */
static inline void dCcD_Sph_ct(void* p) { gabi::call(0x025166F0, p); }
/* dComIfGs_isStageBossDemo: dSv_memBit_c::isDungeonItem(save + 0x798, 5) (as in d_a_bmdhand) */
static inline BOOL dComIfGs_isStageBossDemo() { return gabi::call<BOOL>(0x025B9100, gabi::load<u32>(0x101F84DC) + 0x798, 5); }
static inline u8 dComIfGp_getStartStageName0() { return gabi::load<u8>(dComIfGp_ea() + 0x5134); }
/* __construct_array / __destroy_arr (as in d_a_kb.h) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 a, u32 b) { gabi::call(0x028F0164, p, n, size, dtor, a, b); }
/* 025A5F88 dPa_smokeEcallBack::end; 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void smoke_end(dPa_smokeEcallBack_l* cb) { gabi::call(0x025A5F88, cb); }
static inline void smoke_ct(dPa_smokeEcallBack_l* cb, u8 a) { gabi::call(0x025A5B18, cb, a); }
/* the virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void vremove(CB* cb) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb);
}
/* JPABaseEmitter::playCreateParticle / stopCreateParticle: flag bit 0 of +0x254 (HD) */
static inline void emitter_playCreate(JPABaseEmitter* e) { u32 a = gabi::ea(e) + 0x254; gabi::store<u32>(a, gabi::load<u32>(a) & ~1u); }
static inline void emitter_stopCreate(JPABaseEmitter* e) { u32 a = gabi::ea(e) + 0x254; gabi::store<u32>(a, gabi::load<u32>(a) | 1u); }
/* J3D (HD): the joint matrix block at model+0x2C {+4 flags (0x10: dirty), +0x10 matrices};
 * getAnmMtx marks the matrices dirty (as in d_a_bmdhand) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* model, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
/* dComIfGp_particle_set with a group and a setup word (setToon: group 2, the room number) */
static inline JPABaseEmitter* particle_set_grp(u8 grp, u16 id, const cXyz* pos, const csXyz* angle, u8 alpha, void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, grp, id, pos, angle, nullptr, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}
static inline void se_start(fopAc_ac_c* a, u32 id, cXyz* pos) {
    mDoAud_seStart(id, pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline s32 morf_frame(mDoExt_McaMorf* m) { return gabi::ftoi(m->getFrame()); }

/* 020B5580 nodeCallBack (matcher: "himo2_draw"). HD: asserts the joint number */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x020B5580, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (jntNo >= 0x12) { /* JUT_ASSERT(0x120, jntNo < 18) */
            JUT_ASSERT_fail(STR(0x10009DF0), 0x120, STR(0x10009DE8));
            return TRUE;
        }
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        bmdfoot_class* i_this = gabi::at<bmdfoot_class>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea */
        if (i_this != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), calc_mtx());
            MtxRotX(i_this->m2CC[jntNo].x, 1);
            MtxRotZ(i_this->m2CC[jntNo].z, 1);
            mtx_copy(J3DModel_getAnmMtx(model, jntNo), calc_mtx()); /* setAnmMtx */
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868) /* J3DSys::mCurrentMtx */);
        }
    }
    return TRUE;
}
VERIFY(0x020B5580, nodeCallBack);

/* 020B56CC */
static BOOL daBmdfoot_Draw(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B56CC, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    if (i_this->mBC8 != 0) {
        J3DModel* model = i_this->mpBodyVineMorf->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
        mDoExt_btkAnm* btk = i_this->btk;
        btk->entry(J3DModel_getModelData(model), btk->getFrame());
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
VERIFY(0x020B56CC, daBmdfoot_Draw);

/* 020B5790 */
static void anm_init(bmdfoot_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x020B5790, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(BMDFOOT_ARC, bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(BMDFOOT_ARC, soundFileIdx, SAFESTRING_VTBL);
        i_this->mpBodyVineMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(BMDFOOT_ARC, bckFileIdx, SAFESTRING_VTBL);
        i_this->mpBodyVineMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x020B5790, anm_init);

/* 020B58B8 */
static void housi_off(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B58B8, void, i_this);
    for (int i = 0; i < 3; i++) {
        if (i_this->mAsiWaitFollowCB[i].getEmitter() != nullptr) {
            emitter_stopCreate(i_this->mAsiWaitFollowCB[i].getEmitter());
        }
    }
    if (i_this->mLAttackSmoke01CB.getEmitter() != nullptr) {
        smoke_end(&i_this->mLAttackSmoke01CB); /* remove() */
    }
}
VERIFY(0x020B58B8, housi_off);

/* 020B5904 */
static int ug_move(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B5904, int, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 uVar4 = 0;
    switch (i_this->mBA8) {
    case 1: {
        i_this->mBAC.copy(actor->current.pos);
        i_this->mBAC.y = boss_m328(boss_ea());
        i_this->mBB8.y = actor->current.angle.y;
        i_this->mBC0 = 0.0f;
        i_this->mBA8 = 2;
        i_this->m2B8 = (s16)gabi::ftoi(cM_rndF(10000.0f));
        s8 room = actor->current.roomNo;
        particle_set_grp(2, 0xA0FC /* ID_AK_ST_BKMLATTACKSMOKE01 */, &i_this->mBAC, &i_this->mBB8, 0xB9,
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
                smoke_end(&i_this->mLAttackSmoke01CB);
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
    J3DModel_setBaseTRMtx(model, calc_mtx());
    i_this->mpFloorVineMorf->play(nullptr, 0, 0);
    i_this->mpFloorVineMorf->calc();
    return uVar4;
}
VERIFY(0x020B5904, ug_move);

/* 020B5D74. HD: the search result is checked for NULL */
static void* s_a_d_sub(void* search, void* param_2) {
    WWHD_FUNC(0x020B5D74, void*, search, param_2);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == 0xEB /* fpcNm_BMD_e */) {
        return search;
    }
    return nullptr;
}
VERIFY(0x020B5D74, s_a_d_sub);

/* ---- move() and its states, inlined into daBmdfoot_Execute ---- */
static inline void wait(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    int frame = morf_frame(i_this->mpBodyVineMorf);
    gabi::Local<cXyz> local_98;
    local_98->set(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; i++) {
        PSMTXCopy(J3DModel_getAnmMtx(i_this->mpBodyVineMorf->getModel(), tbl(eff_id, i)), calc_mtx());
        MtxPosition(local_98, &i_this->m3F8[i]);
        if (i_this->m3F4 == 0) {
            particle_set_grp(0, 0x80ED /* ID_AK_SN_BKMASIWAIT00 */, &i_this->m3F8[i], nullptr, 0xFF, &i_this->mAsiWaitFollowCB[i], -1);
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
            i_this->m2BA = 1;
            i_this->m2BC = 0;
        }
        break;
    case 0:
        i_this->m2BC = i_this->m2BC + 1;
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e, 50.0f, J3DFrameCtrl::EMode_LOOP, cM_rndF(0.2f) + 0.9f, -1);
        i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(150.0f) + 100.0f);
        i_this->m3EC = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        for (int i = 2; i <= 16; i++) {
            i_this->m3A4[i] = cM_rndFX(0.1f) + 0.2f;
        }
        break;
    case 1:
        i_this->m3EC = (s16)(i_this->m3EC + 0x200);
        if (boss_m332(boss_ea()) == 1) {
            s16 pa = fopAcM_searchPlayerAngleY(actor);
            s16 sVar2 = (s16)(actor->current.angle.y - pa);
            if (sVar2 < 0) {
                sVar2 = -sVar2;
            }
            if (sVar2 < l_HIO().m06) {
                housi_off(i_this);
                i_this->m2BA = 1;
                i_this->m2BC = 0;
            } else {
                i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            }
        } else if (boss_m332(boss_ea()) == 2) {
            housi_off(i_this);
            i_this->m2BA = 2;
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
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_ATTACK_LOOP_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
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
            se_start(actor, 0x5851 /* JA_SE_CM_BKM_ATKVINE_PREP */, &i_this->mAE8);
        }
        break;
    case 4:
        bVar2 = true;
        if (i_this->m2C0[0] == 1) {
            se_start(actor, 0x5852 /* JA_SE_CM_BKM_ATKVINE_ATTACK */, &i_this->mAE8);
            for (int i = 0; i < 3; i++) {
                PSMTXCopy(J3DModel_getAnmMtx(i_this->mpBodyVineMorf->getModel(), tbl(jno, i)), calc_mtx());
                MtxPosition(local_5c, cStack_68);
                dComIfGp_particle_setSimple(0x8065 /* ID_AK_SN_O_BKMSATTACKHOUSHI00 */, cStack_68, 0xFF, WHITE, WHITE, 0);
            }
            dComIfGp_particle_setSimple(0x8066 /* ID_AK_SN_O_BKMSATTACKSOIL00 */, &i_this->mAE8, 0xFF, WHITE, WHITE, 0);
            dComIfGp_particle_setSimple(0xA06A /* ID_AK_ST_O_BKMSATTACKSMOKE00 */, &i_this->mAE8, 0xB9, WHITE, WHITE, 0);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            i_this->m2BA = 0;
            i_this->m2BC = 0;
        }
        break;
    }

    s16 pa = fopAcM_searchPlayerAngleY(actor);
    s16 sVar3 = (s16)(pa - actor->current.angle.y);
    s16 m06 = l_HIO().m06;
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
            PSMTXCopy(J3DModel_getAnmMtx(model, (i_this->m2B8 & 7U) + tbl(col_joint1, i)), calc_mtx());
            MtxPosition(local_5c, cStack_68);
            sph_set(&i_this->mSph[i], cStack_68, 40.0f);
            i_this->mSph[i].OnAtSPrmBit(1);  /* OnAtSetBit */
            i_this->mSph[i].OffCoSPrmBit(1); /* OffCoSetBit */
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
    }
}

static inline void attack_2(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_3c;
    gabi::Local<cXyz> local_48;
    local_3c->x = local_3c->y = local_3c->z = 0.0f;
    boss_set_m334(boss_ea(), 10);
    switch ((u16)i_this->m2BC) {
    case 0:
        i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(20.0f));
        i_this->m2BC = i_this->m2BC + 1;
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
            PSMTXCopy(J3DModel_getAnmMtx(i_this->mpBodyVineMorf->getModel(), 0x11 /* ASI18 */), calc_mtx());
            MtxPosition(local_3c, local_48);
            dComIfGp_particle_setSimple(0x8065, local_48, 0xFF, WHITE, WHITE, 0);
            dComIfGp_particle_setSimple(0x8066, local_48, 0xFF, WHITE, WHITE, 0);
            dComIfGp_particle_setSimple(0xA06A, local_48, 0xB9, WHITE, WHITE, 0);
            se_start(actor, 0x7811 /* JA_SE_CM_BKM_ATKVINE_IN_G */, &i_this->mAE8);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_UMARU_WAIT_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->mBA8 = 1;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 3:
        se_start(actor, 0x5053 /* JA_SE_CM_BKM_ATKVINE_DIG */, &i_this->mBAC);
        if (ug_move(i_this)) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009D80), dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK1_e, SAFESTRING_VTBL);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_NONE, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 4:
        if (morf_frame(i_this->mpFloorVineMorf) == 2) {
            local_48->copy(i_this->mBAC);
            dComIfGp_particle_setSimple(0x8065, local_48, 0xFF, WHITE, WHITE, 0);
            dComIfGp_particle_setSimple(0x8066, local_48, 0xFF, WHITE, WHITE, 0);
            dComIfGp_particle_setSimple(0xA06A, local_48, 0xB9, WHITE, WHITE, 0);
            se_start(actor, 0x7812 /* JA_SE_CM_BKM_ATKVINE_OUT_G */, &i_this->mBAC);
        }
        ug_move(i_this);
        if (i_this->mpFloorVineMorf->isStop()) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009D80), dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK2_e, SAFESTRING_VTBL);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_LOOP, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2C0[0] = 0xb4;
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 5:
        ug_move(i_this);
        se_start(actor, 0x5054 /* JA_SE_CM_BKM_ATKVINE_L_ATK */, &i_this->mBAC);
        for (int i = 0; i < 4; i++) {
            PSMTXCopy(J3DModel_getAnmMtx(i_this->mpFloorVineMorf->getModel(), tbl(col_joint2, i)), calc_mtx());
            MtxPosition(local_3c, local_48);
            sph_set(&i_this->mSph[i], local_48, 40.0f);
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
        if (i_this->m2C0[0] == 0) {
            J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009D80), dRes_INDEX_BMDFOOT_BCK_ASI_DATTACK3_e, SAFESTRING_VTBL);
            i_this->mpFloorVineMorf->setAnm(pBck, J3DFrameCtrl::EMode_NONE, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            i_this->m2BC = i_this->m2BC + 1;
        }
        break;
    case 6:
        ug_move(i_this);
        if (morf_frame(i_this->mpFloorVineMorf) == 12) {
            dComIfGp_particle_set(0x80F9 /* ID_AK_SN_BKMLATTACKHOUSHI00 */, &i_this->mBAC);
            dComIfGp_particle_set(0x80FA /* ID_AK_SN_BKMLATTACKSOIL00 */, &i_this->mBAC);
            i_this->m498[0].copy(i_this->mBAC);
            s8 room = actor->current.roomNo;
            particle_set_grp(2, 0xA0FB /* ID_AK_ST_BKMLATTACKSMOKE00 */, &i_this->m498[0], nullptr, 0xB9, &i_this->mLAttackSmoke00CB[0], room);
            se_start(actor, 0x7813 /* JA_SE_CM_BKM_ATKVINE_IN_G2 */, &i_this->mBAC);
        }
        if (i_this->mpFloorVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_NUKU_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mBA8 = 0;
            i_this->m2BC = i_this->m2BC + 1;
            se_start(actor, 0x7814 /* JA_SE_CM_BKM_ATKVINE_OUT_G2 */, &i_this->mAE8);
        }
        break;
    case 7:
        if (morf_frame(i_this->mpBodyVineMorf) == 13) {
            local_48->copy(actor->current.pos);
            local_48->y = i_this->mBAC.y;
            dComIfGp_particle_set(0x80F9, local_48);
            dComIfGp_particle_set(0x80FA, local_48);
            s8 room = actor->current.roomNo;
            i_this->m498[1].copy(*local_48);
            particle_set_grp(2, 0xA0FB, &i_this->m498[1], nullptr, 0xB9, &i_this->mLAttackSmoke00CB[1], room);
        }
        if (i_this->mpBodyVineMorf->isStop()) {
            i_this->m2BA = 0;
            i_this->m2BC = 0;
        }
    }
    PSMTXCopy(J3DModel_getAnmMtx(i_this->mpBodyVineMorf->getModel(), 6 /* ASI5 */), calc_mtx());
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
                cLib_addCalc2(&i_this->m2CC[i].x, -(gabi::fmadds((f32)i, 0.03f, 0.05f) + dVar5), 0.1f, reg + 0.1f);
            }
        }
        if (boss_m331(boss_ea()) > 0) {
            i_this->m2BA = 0;
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
        if (boss_m332(boss_ea()) == 6) {
            i_this->m2BC = 1;
            i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(10.0f));
        }
        break;
    case 1:
        if (i_this->m2C0[0] == 0) {
            i_this->mBC8 = 1;
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_START1_e, 1.0f, J3DFrameCtrl::EMode_NONE, cM_rndF(0.2f) + 0.9f, -1);
            i_this->m2BC = 2;
        }
        break;
    case 2:
        if (i_this->mpBodyVineMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_START2_e, 30.0f, J3DFrameCtrl::EMode_LOOP, cM_rndF(0.2f) + 0.9f, -1);
            i_this->m2BC = 3;
        }
        break;
    case 3:
        if (morf_frame(i_this->mpBodyVineMorf) == 2) {
            se_start(actor, 0x584D /* JA_SE_CM_BKM_ATKVINE_MOVE_1 */, &i_this->mAE8);
        }
        if (boss_m332(boss_ea()) == 7) {
            i_this->m2BA = 0;
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
    if (boss_mB76(boss_ea()) == 2) {
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_LOOP_e, 30.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
    }
    if (boss_mB76(boss_ea()) == 0x17c) {
        anm_init(i_this, dRes_INDEX_BMDFOOT_BCK_ASI_DEAD_e, 30.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
    }
}

static inline void move(bmdfoot_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (boss_ea() != 0) {
        s16 sVar1 = (s16)((fopAcM_GetParam(actor) & 0xF) << 13);
        cMtx_YrotS(calc_mtx(), boss_shape_angle(boss_ea(), 1));
        cMtx_XrotM(calc_mtx(), boss_shape_angle(boss_ea(), 0));
        cMtx_ZrotM(calc_mtx(), boss_shape_angle(boss_ea(), 2));
        cMtx_YrotM(calc_mtx(), sVar1);
        actor->current.angle.y = (s16)(sVar1 + boss_shape_angle(boss_ea(), 1));
        gabi::Local<cXyz> local_20;
        local_20->x = 0.0f;
        local_20->y = 0.0f;
        local_20->z = 180.0f;
        gabi::Local<cXyz> cStack_2c;
        MtxPosition(local_20, cStack_2c);
        gabi::Local<cXyz> sum;
        cXyz_pl(boss_pos(boss_ea()), sum, cStack_2c);
        actor->current.pos.copy(*sum);
        switch ((u16)i_this->m2BA) {
        case 0:
            wait(i_this);
            break;
        case 1:
            attack_1(i_this);
            break;
        case 2:
            attack_2(i_this);
            break;
        case 3:
            damage(i_this);
            break;
        case 10:
            start(i_this);
            break;
        case 11:
            end(i_this);
            break;
        }
        if (boss_m332(boss_ea()) == 3) {
            housi_off(i_this);
            i_this->mBA8 = 0;
            i_this->m2BA = 3;
            i_this->m2BC = 0;
        }
        /* HD: for every state (GameCube: in the damage state only), and only once */
        if (boss_m332(boss_ea()) == 8 && i_this->m2BA != 0xb) {
            i_this->m2BA = 0xb;
            i_this->m2BC = 0;
        }
    }
}

/* 020B5DC4 */
static BOOL daBmdfoot_Execute(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B5DC4, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused accessor call */
    if (boss_ea() == 0) {
        set_boss(gabi::ea(fpcM_Search(0x020B5D74 /* s_a_d_sub */, i_this)));
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
    J3DModel_setBaseTRMtx(model, calc_mtx());
    i_this->mpBodyVineMorf->play(nullptr, 0, 0);
    i_this->mpBodyVineMorf->calc();
    if (i_this->mBD0 != 0) {
        if (i_this->mBD0 == 0x1e) {
            se_start(actor, 0x5850 /* JA_SE_CM_BKM_ATKVINE_MOVE_2 */, &i_this->mAE8);
        }
        i_this->mBD0 = i_this->mBD0 + -1;
    }
    s16 sVar1 = (s16)(29 - i_this->mBD0);
    if (sVar1 >= 30) {
        sVar1 = (s16)(sVar1 % 30);
    }
    i_this->btk->mFrameCtrl.setFrame((f32)sVar1);
    PSMTXCopy(J3DModel_getAnmMtx(model, 0x10 /* ASI17 */), calc_mtx());
    gabi::Local<cXyz> local_68;
    local_68->x = local_68->y = local_68->z = 0.0f;
    MtxPosition(local_68, &i_this->mAE8);
    if (boss_ea() != 0 && i_this->mAE8.y < boss_m328(boss_ea())) {
        i_this->mAE8.y = boss_m328(boss_ea());
    }
    cLib_addCalcAngleS2(&i_this->m2BE, 0, 0x10, 0x80);
    s16 sVar1_2;
    if (i_this->m2BA < 3) {
        sVar1_2 = l_HIO().m08;
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
VERIFY(0x020B5DC4, daBmdfoot_Execute);

/* 020B79BC */
static BOOL daBmdfoot_IsDelete(bmdfoot_class*) {
    WWHD_FUNC(0x020B79BC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020B79BC, daBmdfoot_IsDelete);

/* 020B79C4 */
static BOOL daBmdfoot_Delete(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B79C4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10009E88) /* "Bmdfoot" */); /* dComIfG_resDeleteDemo */
    if (i_this->mBD2 != 0) {
        s8 no = l_HIO().mNo;
        hio_set() = 0;
        mDoHIO_deleteChild(no);
    }
    mDoAud_seDeleteObject(&i_this->mAE8);
    mDoAud_seDeleteObject(&i_this->mBAC);
    for (int i = 0; i < 3; i++) {
        vremove(&i_this->mAsiWaitFollowCB[i]);
    }
    vremove(&i_this->mLAttackSmoke00CB[0]);
    vremove(&i_this->mLAttackSmoke00CB[1]);
    vremove(&i_this->mLAttackSmoke01CB);
    set_boss(0);
    return TRUE;
}
VERIFY(0x020B79C4, daBmdfoot_Delete);

/* 020B7AB8. HD: also the solid heap callback (solidHeapCB folded into it) */
static BOOL useHeapInit(bmdfoot_class* i_this) {
    WWHD_FUNC(0x020B7AB8, BOOL, i_this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10009D88), dRes_INDEX_BMDFOOT_BMD_ASI_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009D88), dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e, SAFESTRING_VTBL);
    i_this->mpBodyVineMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                                                    cM_rndF(0.2f) + 0.9f, 0, -1, 1, nullptr, 0, 0x11020203);
    /* HD: the morf is checked before its model */
    if (i_this->mpBodyVineMorf == nullptr) {
        return FALSE;
    }
    J3DModel* model = i_this->mpBodyVineMorf->getModel();
    if (model == nullptr) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(i_this->mpBodyVineMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */
    /* the joint count comes from 027F3F94 (the model data's joint tree), re-read every iteration */
    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F94, J3DModel_getModelData(model)) + 8); i++) {
        u32 md = gabi::ea(J3DModel_getModelData(model));
        u32 node = gabi::load<u32>(md + 8);
        if (i < gabi::load<u32>(md + 4)) {
            node += i * 0x1C;
        }
        gabi::store<u32>(node + 8, 0x020B5580); /* getJointNodePointer(i)->setCallBack(nodeCallBack) */
    }

    mDoExt_btkAnm* btk = (mDoExt_btkAnm*)operator_new(0x74);
    if (btk != nullptr) {
        btk = gabi::call<mDoExt_btkAnm*>(0x025E7C6C, btk); /* mDoExt_btkAnm::mDoExt_btkAnm */
    }
    i_this->btk = btk;
    if (btk == nullptr) /* JUT_ASSERT(1450, i_this->btk) */
        JUT_ASSERT_fail(STR(0x10009DCC), 0x5AA, STR(0x10009DDC));
    J3DAnmTextureSRTKey* pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR(0x10009D88), dRes_INDEX_BMDFOOT_BTK_ASI_e, SAFESTRING_VTBL);
    J3DModelData* md = J3DModel_getModelData(model);
    if (!i_this->btk->init(md, pBtk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10009D88), dRes_INDEX_BMDFOOT_BMD_ASI_e, SAFESTRING_VTBL);
    anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009D88), dRes_INDEX_BMDFOOT_BCK_ASI_WAIT_e, SAFESTRING_VTBL);
    i_this->mpFloorVineMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                                                     cM_rndF(0.2f) + 0.9f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (i_this->mpFloorVineMorf->getModel() == nullptr) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x020B7AB8, useHeapInit);

/* 020B7D80: dPa_followEcallBack array element constructor */
static void follow_ct(dPa_followEcallBack* p) {
    WWHD_FUNC(0x020B7D80, void, p);
    dPa_followEcallBack_ct(p, 0, 0);
}
VERIFY(0x020B7D80, follow_ct);

/* 020B7D8C: dPa_smokeEcallBack array element constructor */
static u32 smoke_elem_ct(dPa_smokeEcallBack_l* p) {
    WWHD_FUNC(0x020B7D8C, u32, p);
    return gabi::call<u32>(0x025A5B18, p, (u8)1); /* smoke_ct: dPa_smokeEcallBack ctor, returns this */
}
VERIFY(0x020B7D8C, smoke_elem_ct);

/* dKy_tevstr_c member copy / default construction (HD: member-wise, floats through FPRs) */
enum { F, B, H, W };
struct copy_e { u16 dst, src; u8 t; };
static inline void copy_member(u32 dst, u32 src, u8 t) {
    switch (t) {
    case F: gabi::store<f32>(dst, gabi::load<f32>(src)); break;
    case B: gabi::store<u8>(dst, gabi::load<u8>(src)); break;
    case H: gabi::store<u16>(dst, gabi::load<u16>(src)); break;
    default: gabi::store<u32>(dst, gabi::load<u32>(src)); break;
    }
}
/* the inline dKy_tevstr_c constructor: three light blocks from the defaults at 0x1016E414 */
static const copy_e tevstr_ct_tbl[] = {
    {0xC14, 0x0, F}, {0xC18, 0x4, F}, {0xC1C, 0x8, F}, {0xC20, 0xC, F}, {0xC24, 0x10, F}, {0xC28, 0x14, F}, {0xC2C, 0x18, B}, {0xC2D, 0x19, B}, {0xC2E, 0x1A, B}, {0xC2F, 0x1B, B}, {0xC30, 0x1C, H}, {0xC32, 0x1E, H}, {0xC34, 0x20, H}, {0xC36, 0x22, H}, {0xC38, 0x24, F}, {0xC3C, 0x28, F}, {0xC40, 0x2C, F}, {0xC44, 0x30, F}, {0xC48, 0x34, F}, {0xC4C, 0x38, F}, {0xC50, 0x3C, F}, {0xD68, 0x10, F}, {0xCF8, 0x24, F}, {0xD5C, 0x4, F}, {0xD00, 0x2C, F}, {0xCE8, 0x14, F}, {0xCEF, 0x1B, B}, {0xD74, 0x1C, H}, {0xCE4, 0x10, F}, {0xD71, 0x19, B}, {0xD76, 0x1E, H}, {0xCED, 0x19, B}, {0xCD4, 0x0, F}, {0xD70, 0x18, B}, {0xD58, 0x0, F}, {0xCE0, 0xC, F}, {0xCEC, 0x18, B}, {0xCF2, 0x1E, H}, {0xCDC, 0x8, F}, {0xD60, 0x8, F}, {0xD04, 0x30, F}, {0xD6C, 0x14, F}, {0xCEE, 0x1A, B}, {0xD73, 0x1B, B}, {0xD72, 0x1A, B}, {0xCF4, 0x20, H}, {0xD78, 0x20, H}, {0xCF0, 0x1C, H}, {0xC54, 0x40, F}, {0xD64, 0xC, F}, {0xD14, 0x40, F}, {0xCF6, 0x22, H}, {0xD08, 0x34, F}, {0xCFC, 0x28, F}, {0xD0C, 0x38, F}, {0xD7A, 0x22, H}, {0xD10, 0x3C, F}, {0xCD8, 0x4, F}, {0xD7C, 0x24, F}, {0xD80, 0x28, F}, {0xD84, 0x2C, F}, {0xD88, 0x30, F}, {0xD8C, 0x34, F}, {0xD90, 0x38, F}, {0xD94, 0x3C, F}, {0xD98, 0x40, F},
};
/* mTevstr = tevStr (member-wise; dst - src = 0xB04) */
static const copy_e tevstr_copy_tbl[] = {
    {0xC14, 0x110, F}, {0xC18, 0x114, F}, {0xC1C, 0x118, F}, {0xC2C, 0x128, B}, {0xC2D, 0x129, B}, {0xC2E, 0x12A, B}, {0xC2F, 0x12B, B}, {0xC20, 0x11C, F}, {0xC24, 0x120, F}, {0xC28, 0x124, F}, {0xC38, 0x134, F}, {0xC3C, 0x138, F}, {0xC30, 0x12C, H}, {0xC40, 0x13C, F}, {0xC44, 0x140, F}, {0xC48, 0x144, F}, {0xC32, 0x12E, H}, {0xC34, 0x130, H}, {0xC4C, 0x148, F}, {0xC36, 0x132, H}, {0xC50, 0x14C, F}, {0xC54, 0x150, F}, {0xCA4, 0x1A0, H}, {0xC98, 0x194, W}, {0xC9C, 0x198, W}, {0xCA6, 0x1A2, H}, {0xCA0, 0x19C, W}, {0xCA8, 0x1A4, H}, {0xCAA, 0x1A6, H}, {0xCAC, 0x1A8, W}, {0xCB0, 0x1AC, W}, {0xCC8, 0x1C4, B}, {0xCB4, 0x1B0, H}, {0xCB6, 0x1B2, H}, {0xCB8, 0x1B4, H}, {0xCC9, 0x1C5, B}, {0xCCA, 0x1C6, B}, {0xCCB, 0x1C7, B}, {0xCBC, 0x1B8, F}, {0xCC0, 0x1BC, F}, {0xCBA, 0x1B6, H}, {0xCC4, 0x1C0, F}, {0xCD4, 0x1D0, F}, {0xCD8, 0x1D4, F}, {0xCDC, 0x1D8, F}, {0xCCC, 0x1C8, B}, {0xCCD, 0x1C9, B}, {0xCCE, 0x1CA, B}, {0xCCF, 0x1CB, B}, {0xCD0, 0x1CC, B}, {0xCEC, 0x1E8, B}, {0xCED, 0x1E9, B}, {0xCEE, 0x1EA, B}, {0xCEF, 0x1EB, B}, {0xCE0, 0x1DC, F}, {0xCE4, 0x1E0, F}, {0xCE8, 0x1E4, F}, {0xCF8, 0x1F4, F}, {0xCFC, 0x1F8, F}, {0xD00, 0x1FC, F}, {0xD04, 0x200, F}, {0xD08, 0x204, F}, {0xD0C, 0x208, F}, {0xD10, 0x20C, F}, {0xCF0, 0x1EC, H}, {0xD14, 0x210, F}, {0xCF2, 0x1EE, H}, {0xD58, 0x254, F}, {0xCF4, 0x1F0, H}, {0xD5C, 0x258, F}, {0xCF6, 0x1F2, H}, {0xD60, 0x25C, F}, {0xD74, 0x270, H}, {0xD64, 0x260, F}, {0xD68, 0x264, F}, {0xD70, 0x26C, B}, {0xD6C, 0x268, F}, {0xD71, 0x26D, B}, {0xD72, 0x26E, B}, {0xD7C, 0x278, F}, {0xD80, 0x27C, F}, {0xD73, 0x26F, B}, {0xD84, 0x280, F}, {0xD88, 0x284, F}, {0xD76, 0x272, H}, {0xD8C, 0x288, F}, {0xD90, 0x28C, F}, {0xD78, 0x274, H}, {0xD94, 0x290, F}, {0xD7A, 0x276, H}, {0xD98, 0x294, F},
};

/* 020B7D94: bmdfoot_class::bmdfoot_class (allocates when this == NULL) */
static bmdfoot_class* bmdfoot_class_ct(bmdfoot_class* self) {
    WWHD_FUNC(0x020B7D94, bmdfoot_class*, self);
    if (self == nullptr) {
        self = (bmdfoot_class*)operator_new(0xE08);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = BMDFOOT_VTBL;
    __construct_array(self->mAsiWaitFollowCB, 3, 0x14, 0x020B7D80);
    __construct_array(self->mLAttackSmoke00CB, 2, 0x20, 0x020B7D8C);
    smoke_ct(&self->mLAttackSmoke01CB, 1);
    gabi::call(0x0200BD2C, &self->mStts);                                /* cCcD_Stts */
    gabi::call(0x02515DA0, gabi::at<u8>(gabi::ea(&self->mStts) + 0x1C));  /* dCcD_GStts */
    self->mStts.__vtbl = 0x1004AE88;
    self->mStts.__vtbl_gstts = 0x1004AEC0;
    __construct_array(self->mSph, 5, 0x12C, 0x025166F0 /* dCcD_Sph::dCcD_Sph */);
    u32 b = gabi::ea(self);
    for (const copy_e& e : tevstr_ct_tbl)
        copy_member(b + e.dst, 0x1016E414 + e.src, e.t);
    return self;
}
VERIFY(0x020B7D94, bmdfoot_class_ct);

/* 020B7FC8 */
static cPhs_State daBmdfoot_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020B7FC8, cPhs_State, a_this);
    /* fopAcM_ct(a_this, bmdfoot_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            bmdfoot_class_ct((bmdfoot_class*)a_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    bmdfoot_class* i_this = (bmdfoot_class*)a_this;
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, STR(0x10009E94) /* "Bmdfoot" */);
    if (res == cPhs_ERROR_e) {
        return cPhs_ERROR_e;
    } else if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    i_this->mLAttackSmoke00CB[0].mFollowOff = 1; /* setFollowOff */
    i_this->mLAttackSmoke00CB[1].mFollowOff = 1;
    if (!fopAcM_entrySolidHeap(a_this, 0x020B7AB8 /* useHeapInit */, 0x5040)) {
        return cPhs_ERROR_e;
    }
    if (hio_set() == 0) {
        hio_set() = 1;
        i_this->mBD2 = 1;
        l_HIO().mNo = mDoHIO_createChild(STR(0x10009E9C) /* "森ボス足" */, &l_HIO());
    }
    a_this->health = 2;
    i_this->m2B8 = (s16)gabi::ftoi(cM_rndF(10000.0f));
    set_boss(0);
    i_this->mStts.Init(0xFF, 0, a_this);
    for (int i = 0; i < 5; i++) {
        i_this->mSph[i].SetStts(&i_this->mStts);
        i_this->mSph[i].Set(cc_sph_src);
    }
    gabi::store<f32>(gabi::ea(a_this) + 0x394, -20000.0f); /* attention_info.position.y */
    a_this->eyePos.y = -20000.0f;
    if (!dComIfGs_isStageBossDemo() && dComIfGp_getStartStageName0() != 'X') {
        i_this->m2BA = 10;
    } else {
        i_this->mBC8 = 1;
    }
    u32 b = gabi::ea(i_this);
    for (const copy_e& e : tevstr_copy_tbl) /* i_this->mTevstr = a_this->tevStr */
        copy_member(b + e.dst, b + e.src, e.t);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x020B7FC8, daBmdfoot_Create);

/* 020B8458: daBmdfoot_HIO_c::daBmdfoot_HIO_c (allocates when this == NULL) */
static daBmdfoot_HIO_c* daBmdfoot_HIO_c_ct(daBmdfoot_HIO_c* h) {
    WWHD_FUNC(0x020B8458, daBmdfoot_HIO_c*, h);
    if (h == nullptr) {
        h = (daBmdfoot_HIO_c*)operator_new(0xC);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->__vtbl = 0x10009DAC;
    h->m06 = 3500;
    h->m08 = 1000;
    return h;
}
VERIFY(0x020B8458, daBmdfoot_HIO_c_ct);

/* 020B84B0 */
static void __sinit_d_a_bmdfoot_cpp() {
    WWHD_FUNC(0x020B84B0, void, (u32)0);
    sinit_header_statics(0x104624DC, 0x10191BBC);
    daBmdfoot_HIO_c_ct(&l_HIO());
}
VERIFY(0x020B84B0, __sinit_d_a_bmdfoot_cpp);

/* 020B8550, 020B8564, 020B8578: deleting destructors of classes with trivial destructors */
static void trivial_dt1(void* p, s32 flags) {
    WWHD_FUNC(0x020B8550, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020B8550, trivial_dt1);
static void trivial_dt2(void* p, s32 flags) {
    WWHD_FUNC(0x020B8564, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020B8564, trivial_dt2);
static void trivial_dt3(void* p, s32 flags) {
    WWHD_FUNC(0x020B8578, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020B8578, trivial_dt3);

/* 020B858C: bmdfoot_class deleting destructor (inline member destructors) */
static void bmdfoot_class_dt(bmdfoot_class* i_this, s32 flags) {
    WWHD_FUNC(0x020B858C, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->mSph, 5, 0x12C, 0x02515AE8 /* dCcD_Sph::~dCcD_Sph */, 0, 0);
        gabi::call(0x02515860, &i_this->mStts, 2); /* dCcD_Stts::~dCcD_Stts */
        __destroy_arr(i_this->mLAttackSmoke00CB, 2, 0x20, 0x020B8578, 0, 0);
        __destroy_arr(i_this->mAsiWaitFollowCB, 3, 0x14, 0x020B8564, 0, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020B858C, bmdfoot_class_dt);

/* 020B864C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x020B864C, void, p);
}
VERIFY(0x020B864C, empty_virtual);
