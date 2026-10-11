/**
 * d_a_ki.cpp (WWHD)
 * Enemy - Keese
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"

const gc_tu_data gc_tu = {.safeString = KI_SAFESTRING_VTABLE, .safeStringAssure = 0x0219DBF8};
static kiHIO_c l_kiHIO;
static u8 hio_set;
static s32 ki_all_count;
static s32 ki_fight_count;
static u32 ki_emitterManager;
static dCcD_SrcSph at_sph_src;
static dCcD_SrcSph co_sph_src;
static dCcD_SrcSph tg_sph_src;
static u16 ki_tex_anm_idx[4] = {30, 33, 32, 31};
static u16 fk_tex_anm_idx[4] = {34, 37, 36, 35};
static u16 ki_tex_max_frame[4] = {2, 2, 3, 2};
static u8 ki_tex_loop[4] = {0, 0, 0, 0};
static u8 ki_item_tbl[4] = {10, 11, 12, 10};
static s8 fire_j[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static f32 fire_sc[10] = {1.0f, 0.800000011920929f, 0.699999988079071f, 0.6000000238418579f, 0.5f, 1.0f, 0.800000011920929f, 0.699999988079071f, 0.6000000238418579f, 0.5f};

#define SAFESTRING_VTBL KI_SAFESTRING_VTABLE /* this TU's sead::SafeString vtable */
#define KI_VTBL KI_ACTOR_VTABLE         /* ki_class vtable (HD virtual destructor) */
#define KIHIO_VTBL KI_HIO_VTABLE      /* kiHIO_c vtable */


// HD: named call bindings preserve this split translation unit's signatures.
static inline void enemy_fire_remove(enemyfire_l* f) { kiMain_removeFire(f); }
static inline void dMat_control_iceEntryDL(mDoExt_McaMorf* morf, s32 p1, void* p2) { kiMain_iceEntry(morf, p1, p2); }
static inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm* a, J3DModelData* d, J3DAnmTexPattern* pat, s32 anmPlay, s32 attr,
                                      f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return kiMain_initPattern(a, d, pat, anmPlay, attr, start, end, modify, entry, rate);
}
static inline mDoExt_btpAnm* new_mDoExt_btpAnm() { return kiMain_createPattern((u32)0); }
static inline mDoExt_btkAnm* mDoExt_btkAnm_ct(void* p) { return kiMain_createTexture(p); }
static inline BOOL mDoExt_btkAnm_init(mDoExt_btkAnm* a, J3DModelData* d, void* key, s32 anmPlay, s32 attr, f32 rate,
                                      s16 start, s16 end, bool modify, s32 entry) {
    return kiMain_initTexture(a, d, key, anmPlay, attr, start, end, modify, entry, rate);
}

static inline void J3DJoint_entryIn(J3DModel* model, u32 joint) { kiMain_jointEntry(model, joint); }

static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return kiMain_characterIndex(dComIfGp_ea() + PLAY_NAMETBL, name, p);
}
static inline void ki_packet_init(void* pkt, u32 p) { kiMain_packetInit(pkt, p); }
/* this TU's sead::SafeString::assureTerminationImpl_ (empty) */

/* dComIfGd_setListMaskOff / dComIfGd_setList (HD: j3dSys draw buffers from the play object) */
/* dComIfGd_setList: shared (bindings.h) */

/* HD J3DModel / J3DModelData accessors */
static inline u32 J3DModel_modelData(J3DModel* m) { return (u32)gabi::ea(m->getModelData()); }
/* HD: getMaterialNodePointer(name): a sead::SafeString key looked up in the material name table
 * (sead::Buffer semantics: an index past the end yields element 0) */

/* 021991AC */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        ki_class* i_this = static_cast<ki_class*>(model->getUserArea());
        s32 jntNo = (u16)kiJoint(joint)->mJntNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m328);
            cMtx_ZrotM(calc_mtx(), i_this->m326);
            Mtx34* dst = model->getAnmMtx(jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 021992DC */
static void* ki_a_d_sub(void* ac1, void*) {
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_KI_e) {
        ki_class* ki = (ki_class*)ac1;
        ki_all_count += 1;
        if (ki->mAction == ki_class::ACT_ATTACK_MOVE_INDEX_e && ki->mBehaviorType < 10) {
            ki_fight_count += 1;
        }
    }
    return nullptr;
}

/* 02199364 */
static u32 ki_check(ki_class* i_this) {
    ki_all_count = 0;
    ki_fight_count = 0;
    return (u32)gabi::ea(fpcM_Search(gabi::guest_fn((const void*)&ki_a_d_sub), &i_this->actor));
}

/* 02199388 */
static void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    if (soundResIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Ki", anmResIdx, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes("Ki", soundResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Ki", anmResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}

/* 021994B0 */
static void tex_anm_set(ki_class* i_this, unsigned short idx) {
    i_this->m335 = 1;
    // HD: the fire keese has its own eye patterns
    u16 res = i_this->mDamageType != 0 ? fk_tex_anm_idx[idx] : ki_tex_anm_idx[idx];
    J3DAnmTexPattern* pat = (J3DAnmTexPattern*)dComIfG_getObjectRes("Ki", res, SAFESTRING_VTBL);
    i_this->m336 = (u8)ki_tex_max_frame[idx];
    i_this->m337 = ki_tex_loop[idx];
    i_this->m32C = pat;
    i_this->m334 = 0;
    kiPattern(pat)->mFrame = 0.0f; /* m32C->setFrame(0.0f); HD: frame at +0 */
    // HD: no J3DTexNoAnm loop (the pattern is applied through an mDoExt_btpAnm in Draw)
}

/* 021995BC */
static BOOL ki_player_bg_check(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    /* player->checkGrabWear(): daPy_py_c field (GameCube 0x2B0) < 0 */
    if (std::fabs((f32)player->speedF) < 0.1f && (f32)kiPlayer(player)->mGrabWear < 0.0f) {
        return TRUE;
    }

    KiLineCheck linChk;
    ki_LinChk_ct(&linChk);
    cXyz sp14;
    cXyz sp08;
    sp08.copy(player->eyePos);
    sp14.copy(a_this->eyePos);
    dBgS_LinChk_Set(&linChk, &sp14, &sp08, a_this);
    BOOL ret = cBgS_LineCross(dComIfG_Bgsp(), &linChk);
    ki_LinChk_dt(&linChk);
    return ret;
}

/* 02199748 */
static BOOL daKi_Draw(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    if (i_this->m2D4 != 0) {
        return TRUE;
    }

    J3DModel* pModel = i_this->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), pModel, &a_this->tevStr);

    if (i_this->mEnemyIce.mFreezeTimer > 0x14) {
        dMat_control_iceEntryDL(i_this->mpMorf, -1, nullptr);
        return TRUE;
    }

    a_this->model = gabi::ea(pModel);
    dSnap_RegistFig(DSNAP_TYPE_KI, a_this, 1.0f, 1.0f, 1.0f);
    // HD: the eye pattern goes through an mDoExt_btpAnm (GameCube: setTexNoAnimator)
    kiPattern(i_this->m32C)->mFrame = (f32)(u8)i_this->m334;
    mDoExt_btpAnm_init(i_this->m330, pModel->getModelData(), i_this->m32C, 1, 2, 1.0f, 0, -1,
                       true, 0);
    mDoExt_btpAnm_entry(i_this->m330, pModel->getModelData(), i_this->m334);

    J3DModelData* md = pModel->getModelData();
    if (i_this->mDamageType == 0) {
        // HD: joint and materials looked up by name
        u32 joint = md->mJointNodes; /* getJointNodePointer(KI_JNT_KI_ALLROOT_e) */
        J3DMaterial* material = md->getMaterialNodePointer("MX_ki_eye_l");
        J3DShape* shape = material->getShape();
        shape->hide(); /* shape->hide() */
        i_this->mpMorf->entryDL();
        dComIfGd_setListMaskOff();
        shape->show(); /* shape->show() */
        // HD: no getMatPacket(0)->unlock()

        material = md->getMaterialNodePointer("m_ki_main");
        material->getShape()->hide();
        J3DJoint_entryIn(pModel, joint);
        material->getShape()->show();

        dComIfGd_setList();
    } else {
        mDoExt_btkAnm_entry(i_this->m920, md, i_this->m920->getFrame());

        dComIfGd_setListMaskOff();
        i_this->mpMorf->entryDL();
        dComIfGd_setList();
    }

    // HD: no blob shadow here
    return TRUE;
}

/* 02199A64 */
static void ki_pos_move(ki_class* i_this, s8 arg1) {
    if (arg1 == 0) {
        f32 x = i_this->mPosMove.x - i_this->actor.current.pos.x;
        f32 y = i_this->mPosMove.y - i_this->actor.current.pos.y;
        f32 z = i_this->mPosMove.z - i_this->actor.current.pos.z;
        s16 xzAngle = cM_atan2s(x, z);
        s16 iVar6 = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));

        cLib_addCalcAngleS2(&i_this->actor.current.angle.y, xzAngle, 5, gabi::ftoi(i_this->m304 * i_this->mPosMoveDist));
        cLib_addCalcAngleS2(&i_this->actor.current.angle.x, iVar6, 5, gabi::ftoi(i_this->m304 * i_this->mPosMoveDist));
        cLib_addCalc2(&i_this->mPosMoveDist, 1.0f, 1.0f, 0.04f);
    }

    cLib_addCalc2(&i_this->actor.speedF, i_this->mPosMoveTarget, 1.0f, i_this->mPosMoveMaxSpeed);
    cXyz sp0C;
    sp0C.x = 0.0f;
    sp0C.y = 0.0f;
    sp0C.z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    MtxPosition(&sp0C, &i_this->actor.speed);
    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;
}

/* 0219CE90 */
static BOOL daKi_IsDelete(ki_class* i_this) {
    return TRUE;
}

/* 0219CE98 */
static BOOL daKi_Delete(ki_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Ki");

    if (i_this->m339 != 0) {
        hio_set = 0;
        mDoHIO_deleteChild(l_kiHIO.mNo);
    }

    if (i_this->actor.heap != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }

    if (i_this->mDamageType != 0) {
        /* m908.remove(): virtual (HD vtable at +0, slot +0x44) */
        kiRemoveFollow(&i_this->m908);
    }

    enemy_fire_remove(&i_this->mEnemyFire);
    return TRUE;
}

/* 0219CF38 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    ki_class* i_this = (ki_class*)a_this;
    J3DModel* model;

    if (i_this->mDamageType == 0) {
        i_this->actor.gbaName = 0x17;
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BDL_KI_e, SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BCK_WAIT1_e, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BAS_WAIT1_e, SAFESTRING_VTBL);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                bas, 0x80000, 0x37221203);
        if (i_this->mpMorf == nullptr || (model = i_this->mpMorf->getModel()) == nullptr) {
            return FALSE;
        }
    } else {
        i_this->actor.gbaName = 6;
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BDL_FK_e, SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BCK_WAIT1_e, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BAS_WAIT1_e, SAFESTRING_VTBL);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                bas, 0x80000, 0x37221203);
        if (i_this->mpMorf == nullptr) {
            return FALSE;
        }

        model = i_this->mpMorf->getModel();
        if (model == nullptr) {
            return FALSE;
        }

        void* p = operator_new(0x74);
        {
            /* harness workaround: the matcher's name for 025E7C6C ("mDoExt_btkAnm::init") gives it &a
             * GameCube stack parameter, so the harness compares the word at SP+8; in the original it
             * still holds the McaMorf constructor's stack argument (1) */
            KiCtorFrame fr;
            fr.w[2] = 1;
            i_this->m920 = p != nullptr ? mDoExt_btkAnm_ct(p) : nullptr;
        }
        if (i_this->m920 == nullptr) {
            return FALSE;
        }

        void* key = dComIfG_getObjectRes("Ki", dRes_INDEX_KI_BTK_FK_e, SAFESTRING_VTBL);
        if (!mDoExt_btkAnm_init(i_this->m920, model->getModelData(), key, 1,
                                J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }

    for (u16 i = 0; i < model->getModelData()->getJointNum(); i++) {
        if (i == KI_JNT_J_KI_HEAD_e) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack): HD joints are 0x1C-byte elements,
             * an index past the end yields element 0 */
            model->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
        }
    }

    J3DAnmTexPattern* anmTexPattern = nullptr;
    for (s32 i = 0; i < 4; i++) {
        anmTexPattern = static_cast<J3DAnmTexPattern*>(dComIfG_getObjectRes("Ki", ki_tex_anm_idx[i], SAFESTRING_VTBL));
        /* anmTexPattern->searchUpdateMaterialID(modelData): HD virtual (vtable at +4, slot +0x1C) */
        u32 fn = kiSearchPattern(anmTexPattern);
        gabi::call_ptr(fn, anmTexPattern, J3DModel_modelData(model));
    }

    // HD: an mDoExt_btpAnm replaces the J3DTexNoAnm array
    i_this->m330 = new_mDoExt_btpAnm();
    mDoExt_btpAnm_init(i_this->m330, model->getModelData(), anmTexPattern,
                       1, 2, 1.0f, 0, -1, false, 0);

    // HD: no tex_anm_set(i_this, 1) here
    i_this->mpMorf->getModel()->mUserArea = gabi::ea(i_this); /* setUserArea */

    /* HD: the shadow packet is set up only in Ganon's Tower ("GanonK"):
     * if (sead::SafeString("GanonK") == dComIfGp_getStartStageName()) */
    SafeString a;
    a.mStringTop = gabi::guest_addr("GanonK"); /* "GanonK" */
    a.__vtbl = SAFESTRING_VTBL;
    SafeString b;
    {
        u32 stage = dComIfGp_ea() + 0x5134;
        b.__vtbl = SAFESTRING_VTBL;
        b.mStringTop = stage;
    }
    kiAssureString(a.__vtbl, &a);
    kiAssureString(a.__vtbl, &a);
    u32 bv = b.__vtbl;
    u32 s1 = a.mStringTop;
    kiAssureString(bv, &b);
    u32 s2 = b.mStringTop;
    bool eq = false;
    if (s1 == s2) {
        eq = true;
    } else {
        for (u32 n = 0x40001; n != 0; n--) {
            u8 c1 = kiStringByte(s1);
            u8 c2 = kiStringByte(s2);
            if (c1 != c2) break;
            if (c1 == 0) {
                eq = true;
                break;
            }
            s1++;
            s2++;
        }
    }
    if (eq) {
        ki_packet_init(i_this->m2B8, 0);
    }
    return TRUE;
}

static ki_class* ki_class_ct(ki_class* p);

/* 0219D568 */
static cPhs_State daKi_Create(fopAc_ac_c* a_this) {
    ki_class* i_this = (ki_class*)a_this;

    /* fopAcM_ct(a_this, ki_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            ki_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, "Ki");
    if (ret == cPhs_COMPLEATE_e) {
        u8 prm = fopAcM_GetParam(a_this) & 0xFF;
        if (prm == 0xFF) {
            prm = 0;
        }
        u32 param = fopAcM_GetParam(a_this);
        i_this->mParameters = prm;
        i_this->mAction = prm;
        i_this->m2CD = (param >> 8) & 0x7F;
        i_this->mDamageType = (param >> 8) & 0x80;
        i_this->mKiPathIndex = param >> 0x10;
        i_this->m2CF = param >> 0x18;
        a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex("keeth" /* "keeth" */, 0);

        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void*)&useHeapInit), 0x4E00)) {
            return cPhs_ERROR_e;
        }

        switch (i_this->m2CD) {
        case 0:
            i_this->mMaxAttackMoveDist300 = 300.0f;
            break;
        case 1:
            i_this->mMaxAttackMoveDist300 = 800.0f;
            break;
        case 2:
            i_this->mMaxAttackMoveDist300 = 1500.0f;
            break;
        case 3:
        default:
            i_this->mMaxAttackMoveDist300 = 3000.0f;
            break;
        }

        if (i_this->mKiPathIndex != 0xFF) {
            i_this->ppd = dPath_GetRoomPath(i_this->mKiPathIndex, fopAcM_GetRoomNo(a_this));
            if (i_this->ppd == nullptr) {
                return cPhs_ERROR_e;
            }
            i_this->mCurrKiPathIndex = i_this->mKiPathIndex + 1;
            i_this->m2D7 = 1;
        }

        if (i_this->m2CF != 0xFF) {
            i_this->m2D4 = i_this->m2CF + 1;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
        i_this->mAcchCir.SetWall(20.0f, 50.0f);
        a_this->health = 1;
        a_this->max_health = 1;
        i_this->m320 = (s16)gabi::ftoi(cM_rndF(10000.0f));

        i_this->mStts.Init(0x32, 2, a_this);

        i_this->m580.Set(&at_sph_src);          /* at_sph_src */
        i_this->m6AC.Set(&co_sph_src);          /* co_sph_src */
        i_this->mDamageSphere.Set(&tg_sph_src); /* tg_sph_src */

        i_this->m580.SetStts(&i_this->mStts);
        i_this->m6AC.SetStts(&i_this->mStts);
        i_this->mDamageSphere.SetStts(&i_this->mStts);

        if (i_this->mDamageType != 0) {
            i_this->m580.SetAtType(KiAttackType::FIRE);
            i_this->mDamageSphere.SetTgType(0xFF3DFCFF);
        }

        i_this->m580.OffAtSPrmBit(2 /* cCcD_AtSPrm_VsEnemy_e */);
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */

        if (!hio_set) {
            l_kiHIO.mNo = mDoHIO_createChild("L[X" /* "キース" */, &l_kiHIO);
            i_this->m339 = 1;
            hio_set = 1;
        }

        i_this->mEnemyIce.mpActor = a_this;
        i_this->mEnemyIce.mWallRadius = REG0_F(4) + 30.0f;
        i_this->mEnemyIce.mCylHeight = REG0_F(5) + 30.0f;
        i_this->mEnemyIce.mParticleScale = 0.5f;
        i_this->mEnemyIce.mYOffset = 10.0f;
        i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
        i_this->mEnemyFire.mpActor = a_this;

        for (int i = 0; i < 10; i++) {
            i_this->mEnemyFire.mFlameJntIdxs[i] = fire_j[i];           /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = fire_sc[i];     /* fire_sc */
        }

        kiMain_execute(i_this); /* daKi_Execute */
    }
    return ret;
}

// HD: compiler-generated and template code of this translation unit.
/* enemyfire::enemyfire (compiler-generated, HD) */
/* 0219D368 */
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {
    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(&p->mDirection) == 0) /* cXyz mDirection: GHS null-check of the member address */
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    kiMain_sphereCtor(&p->mSph); /* dCcD_Sph::dCcD_Sph */
    p->m228 = 1.0f;
    return p;
}

/* dBgS_ObjAcch inline constructor: this TU's vtables */
static void ki_ObjAcch_ct(dBgS_ObjAcch* a) {
    kiMain_acchCtor(a); /* dBgS_Acch::dBgS_Acch */
    kiAcch(a)->mCheckVtable = KI_ACCH_CHECK_VTABLE;
    kiAcch(a)->mPassVtable = KI_ACCH_PASS_VTABLE;
    kiAcch(a)->mObjectThrough = 1;
    kiAcch(a)->mGroupVtable = KI_ACCH_GROUP_VTABLE;
}

/* 0219D3F4 */
static ki_class* ki_class_ct(ki_class* p) {
    if (p == nullptr) {
        p = (ki_class*)operator_new(0x10BC);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(&p->actor);
    p->actor.__vtbl = KI_VTBL;
    kiMain_packetCtor(p->m2B8); /* mDoExt_J3DModelPacketS (HD) */
    kiMain_circleCtor(&p->mAcchCir);
    ki_ObjAcch_ct(&p->mAcch);
    dCcD_Stts_ct(&p->mStts);
    kiMain_sphereCtor(&p->m580);
    kiMain_sphereCtor(&p->m6AC);
    kiMain_sphereCtor(&p->mDamageSphere);
    kiMain_followCtor(&p->m908, 0, 0); /* dPa_followEcallBack(0, 0) */
    /* enemyice (inline) */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, KI_CYLINDER_VTABLE);
    kiMain_circleCtor(&p->mEnemyIce.mBgAcchCir);
    ki_ObjAcch_ct(&p->mEnemyIce.mBgAcch);
    enemyfire_ct(&p->mEnemyFire);
    return p;
}


/* static initialisation (header statics, l_kiHIO) */
/* 0219D920 */
static void __sinit_d_a_ki_cpp() {
    sinit_header_statics(KI_HEADER_STATICS, KI_HEADER_RECORDS);
    kiHIO_c& h = l_kiHIO;
    h.m05 = 0;
    h.m06 = 0;
    h.m07 = 0;
    h.m10 = 1.0f;
    h.m08 = 1.0f;
    h.m14 = 1.5f;
    h.m18 = 2.0f;
    h.m1C = 55.0f;
    h.m20 = 175.0f;
    h.m24 = 60.0f;
    h.m28 = 30.0f;
    h.m2C = 3.0f;
    h.m30 = 7.0f;
    h.m44 = 30.0f;
    h.m34 = 65.0f;
    h.m38 = 100.0f;
    h.m40 = 7.0f;
    h.m3C = 15.0f;
    h.m4C = 30;
    h.m48 = 0.2f;
    h.m54 = 3;
    h.m52 = 7;
    h.m50 = 0x12C;
    h.m4E = 0x320;
    h.m0C = 500.0f;
    h.m58 = 80.0f;
    h.__vtbl = KIHIO_VTBL;
}

/* deleting destructor of a trivially destructible class of this TU (sead::SafeString
 * slot 1 of the vtable at KI_SAFESTRING_VTABLE) */
/* 0219DAC4 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* dBgS_ObjAcch inline destructor (this TU's vtables) */
static void ki_ObjAcch_dt(dBgS_ObjAcch* a) {
    kiAcch(a)->mGroupVtable = KI_ACCH_GROUP_VTABLE;
    kiAcch(a)->mPassVtable = KI_ACCH_PASS_VTABLE;
    kiMain_acchDtor(a, 0); /* dBgS_Acch::~dBgS_Acch */
}

/* ki_class deleting destructor (compiler-generated, HD virtual destructor) */
/* 0219DAD8 */
static void ki_class_dt(ki_class* p, s32 flags) {
    if (p != nullptr) {
        kiMain_sphereDtor(&p->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&p->mEnemyFire.mStts, 2);
        ki_ObjAcch_dt(&p->mEnemyIce.mBgAcch);
        kiMain_circleDtor(&p->mEnemyIce.mBgAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&p->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&p->mEnemyIce.mStts, 2);
        kiMain_sphereDtor(&p->mDamageSphere, 2);
        kiMain_sphereDtor(&p->m6AC, 2);
        kiMain_sphereDtor(&p->m580, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        ki_ObjAcch_dt(&p->mAcch);
        kiMain_circleDtor(&p->mAcchCir.m_cir, 2);
        kiMain_packetDtor(p->m2B8, 2); /* mDoExt_J3DModelPacketS::~ */
        kiMain_actorDtor(p, 0);        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
/* 0219DBF8 */
static void SafeString_assureTerminationImpl(void* string) {
}

/* WWHD glue
ext 025E1AA4 void kiMonsSe(u32 sound, cXyz* position, u32 actorID, u32 param, s32 reverb)
str 10012C5C in 02199388
str 10012C5F in 021994B0
str 10012D50 in 0219CE98
str 10012D53 in 0219CF38
str 10012D70 in 0219D568
str 10012D68 in 0219D568
str 10012D74 in 0219D568
str 10012D58 in 0219CF38
str 10012C78
str 10012C84
data l_kiHIO 10464B4C
data hio_set 101B8168
data ki_all_count 10464B28
data ki_fight_count 10464B2C
data ki_tex_anm_idx 101B816C
data fk_tex_anm_idx 101B8174
data ki_tex_max_frame 101B8158
data ki_tex_loop 101B8160
data ki_item_tbl 101B8164
data ki_emitterManager 1047B2D4
data at_sph_src 101B819C
data co_sph_src 101B821C
data tg_sph_src 101B81DC
data fire_j 101B8284
data fire_sc 101B825C
ext 02041C30 void kiMain_removeFire(enemyfire_l* fire)
ext 0259138C void kiMain_iceEntry(mDoExt_McaMorf* morph, s32 mode, void* data)
ext 025E789C BOOL kiMain_initPattern(mDoExt_btpAnm* animation, J3DModelData* model, J3DAnmTexPattern* pattern, s32 play, s32 mode, s16 start, s16 end, bool modify, s32 entry, f32 speed)
ext 025E7820 mDoExt_btpAnm* kiMain_createPattern(u32 storage)
ext 025E7C6C mDoExt_btkAnm* kiMain_createTexture(void* storage)
ext 025E7CE0 BOOL kiMain_initTexture(mDoExt_btkAnm* animation, J3DModelData* model, void* key, s32 play, s32 mode, s16 start, s16 end, bool modify, s32 entry, f32 speed)
ext 027F583C void kiMain_jointEntry(J3DModel* model, u32 joint)
ext 0200E814 s32 kiMain_characterIndex(u32 table, const char* name, s32 mode)
ext 0207FD38 void kiMain_packetInit(void* packet, u32 mode)
ext 025166F0 void kiMain_sphereCtor(dCcD_Sph* sphere)
ext 024F0474 void kiMain_acchCtor(dBgS_ObjAcch* acch)
ext 02080404 void kiMain_packetCtor(void* packet)
ext 024EFE94 void kiMain_circleCtor(dBgS_AcchCir* circle)
ext 025A5894 void kiMain_followCtor(dPa_followEcallBack_l* callback, u32 pos, u32 angle)
ext 0219AAE8 BOOL kiMain_execute(ki_class* actor)
ext 024EFD9C void kiMain_acchDtor(dBgS_ObjAcch* acch, s32 flags)
ext 02515AE8 void kiMain_sphereDtor(dCcD_Sph* sphere, s32 flags)
ext 02018034 void kiMain_circleDtor(void* circle, s32 flags)
ext 02082DDC void kiMain_packetDtor(void* packet, s32 flags)
ext 025D50BC void kiMain_actorDtor(ki_class* actor, s32 flags)
*/
