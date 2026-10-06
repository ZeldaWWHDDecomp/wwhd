/**
 * d_a_ki.cpp (WWHD)
 * Enemy - Keese
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"

#define SAFESTRING_VTBL 0x10012B5C /* this TU's sead::SafeString vtable */
#define KI_VTBL 0x10012C24         /* ki_class vtable (HD virtual destructor) */
#define KIHIO_VTBL 0x10012C14      /* kiHIO_c vtable */


/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02041C30 enemy_fire_remove(enemyfire*) */
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* 0259138C dMat_ice_c::entryDL(mDoExt_McaMorf*, int, ...) */
static inline void dMat_control_iceEntryDL(mDoExt_McaMorf* morf, s32 p1, void* p2) { gabi::call(0x0259138C, morf, p1, p2); }
/* 025E789C HD: mDoExt_btpAnm::init(J3DModelData*, J3DAnmTexPattern*, anmPlay, attr, rate, start, end, modify, entry) */
static inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm* a, J3DModelData* d, J3DAnmTexPattern* pat, s32 anmPlay, s32 attr,
                                      f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, pat, anmPlay, attr, start, end, modify, entry, rate);
}
/* 025E7820 mDoExt_btpAnm::mDoExt_btpAnm (allocates when this == NULL) */
static inline mDoExt_btpAnm* new_mDoExt_btpAnm() { return gabi::call<mDoExt_btpAnm*>(0x025E7820, (u32)0); }
/* 025E7C6C mDoExt_btkAnm::mDoExt_btkAnm (matcher: "mDoExt_btkAnm::init") */
static inline mDoExt_btkAnm* mDoExt_btkAnm_ct(void* p) { return gabi::call<mDoExt_btkAnm*>(0x025E7C6C, p); }
/* 025E7CE0 mDoExt_btkAnm::init(J3DModelData*, J3DAnmTextureSRTKey*, anmPlay, attr, rate, start, end, modify, entry) */
static inline BOOL mDoExt_btkAnm_init(mDoExt_btkAnm* a, J3DModelData* d, void* key, s32 anmPlay, s32 attr, f32 rate,
                                      s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E7CE0, a, d, key, anmPlay, attr, start, end, modify, entry, rate);
}
/* 027DF9B0 JUTNameTab::getIndex(const char*) */
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* 027F583C HD: J3DJoint::entryIn as (J3DModel*, J3DJoint*) */
static inline void J3DJoint_entryIn(J3DModel* model, u32 joint) { gabi::call(0x027F583C, model, joint); }
/* 027F3F94 (matcher: "__nw"): J3DModelData accessor returning the joint tree (u16 joint count at +8) */
static inline u16 J3DModelData_getJointNum(u32 md) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, md)) + 8); }
/* 0200E814 cDT_NamePTbl::GetIndex (dComIfGp_CharTbl() = play+0x50AC) */
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, name, p);
}
/* 0207FD38 HD: shadow model packet setup (mDoExt_J3DModelPacketS at +0x3D4) */
static inline void ki_packet_init(void* pkt, u32 p) { gabi::call(0x0207FD38, pkt, p); }
/* 0219DBF8: this TU's sead::SafeString::assureTerminationImpl_ (empty) */
static inline void SafeString_assureTermination(SafeString* s) { gabi::call(0x0219DBF8, s); }
/* dComIfGd_setListMaskOff / dComIfGd_setList (HD: j3dSys draw buffers from the play object) */
static inline void dComIfGd_setListMaskOff() {
    gabi::store<u32>(0x104B45C0 + 0x74, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B45C0 + 0x78, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
}
/* dComIfGd_setList: shared (bindings.h) */

/* HD J3DModel / J3DModelData accessors */
static inline u32 J3DModel_modelData(J3DModel* m) { return gabi::load<u32>(gabi::ea(m) + 0xAC); }
/* HD: getMaterialNodePointer(name): a sead::SafeString key looked up in the material name table
 * (sead::Buffer semantics: an index past the end yields element 0) */
static u32 ki_getMaterialByName(u32 md, u32 name) {
    gabi::Local<SafeString> s;
    s->mStringTop = name;
    s->__vtbl = SAFESTRING_VTBL;
    u32 holder = gabi::load<u32>(md + 0);
    SafeString_assureTermination(s);
    s32 off = gabi::load<s32>(holder + 0x18);
    u32 tab = off != 0 ? holder + 0x18 + off : 0;
    s32 idx = JUTNameTab_getIndex(tab, s->mStringTop);
    if (idx < 0)
        return 0;
    u32 arr = gabi::load<u32>(md + 0x10);
    if ((u32)idx < gabi::load<u32>(md + 0xC))
        return arr + idx * 0x39C;
    return arr;
}

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; joint matrices in a
 * block at model+0x2C (+0x4 flags, +0x10 matrices) */
static Mtx34* ki_getAnmMtx(u32 model, s32 jntNo) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}

/* 021991AC */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021991AC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = gabi::load<u32>(0x104B462C);
        ki_class* i_this = gabi::at<ki_class>(gabi::load<u32>(model + 0xB8));
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(ki_getAnmMtx(model, jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m328);
            cMtx_ZrotM(calc_mtx(), i_this->m326);
            Mtx34* dst = ki_getAnmMtx(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x021991AC, nodeCallBack);

/* 021992DC */
static void* ki_a_d_sub(void* ac1, void*) {
    WWHD_FUNC(0x021992DC, void*, ac1);
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_KI_e) {
        ki_class* ki = (ki_class*)ac1;
        ki_all_count() += 1;
        if (ki->mAction == ki_class::ACT_ATTACK_MOVE_INDEX_e && ki->mBehaviorType < 10) {
            ki_fight_count() += 1;
        }
    }
    return nullptr;
}
VERIFY(0x021992DC, ki_a_d_sub);

/* 02199364 */
static u32 ki_check(ki_class* i_this) {
    WWHD_FUNC(0x02199364, u32, i_this);
    ki_all_count() = 0;
    ki_fight_count() = 0;
    return (u32)gabi::ea(fpcM_Search(0x021992DC /* ki_a_d_sub */, &i_this->actor));
}
VERIFY(0x02199364, ki_check);

/* 02199388 */
static void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    WWHD_FUNC(0x02199388, void, i_this, anmResIdx, morf, loopMode, playSpeed, soundResIdx);
    if (soundResIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012C5C), anmResIdx, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x10012C5C), soundResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012C5C), anmResIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02199388, anm_init);

/* 021994B0 */
static void tex_anm_set(ki_class* i_this, unsigned short idx) {
    WWHD_FUNC(0x021994B0, void, i_this, idx);
    i_this->m335 = 1;
    /* HD: the fire keese has its own eye patterns */
    u16 res = i_this->mDamageType != 0 ? fk_tex_anm_idx(idx) : ki_tex_anm_idx(idx);
    J3DAnmTexPattern* pat = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x10012C5F), res, SAFESTRING_VTBL);
    i_this->m336 = (u8)ki_tex_max_frame(idx);
    i_this->m337 = ki_tex_loop(idx);
    i_this->m32C = pat;
    i_this->m334 = 0;
    gabi::store<f32>(gabi::ea(pat), 0.0f); /* m32C->setFrame(0.0f); HD: frame at +0 */
    /* HD: no J3DTexNoAnm loop (the pattern is applied through an mDoExt_btpAnm in Draw) */
}
VERIFY(0x021994B0, tex_anm_set);

/* 021995BC */
static BOOL ki_player_bg_check(ki_class* i_this) {
    WWHD_FUNC(0x021995BC, BOOL, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    /* player->checkGrabWear(): daPy_py_c field (GameCube 0x2B0) < 0 */
    if (std::fabs((f32)player->speedF) < 0.1f && gabi::load<f32>(gabi::ea(player) + 0x3CC) < 0.0f) {
        return TRUE;
    }

    gabi::Local<dBgS_LinChk_l> linChk;
    ki_LinChk_ct(linChk);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    sp08->copy(player->eyePos);
    sp14->copy(a_this->eyePos);
    dBgS_LinChk_Set(linChk, sp14, sp08, a_this);
    BOOL ret = cBgS_LineCross(dComIfG_Bgsp(), linChk);
    ki_LinChk_dt(linChk);
    return ret;
}
VERIFY(0x021995BC, ki_player_bg_check);

/* 02199748 */
static BOOL daKi_Draw(ki_class* i_this) {
    WWHD_FUNC(0x02199748, BOOL, i_this);
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
    /* HD: the eye pattern goes through an mDoExt_btpAnm (GameCube: setTexNoAnimator) */
    gabi::store<f32>(gabi::ea(i_this->m32C.get()), (f32)(u8)i_this->m334);
    mDoExt_btpAnm_init(i_this->m330, gabi::at<J3DModelData>(J3DModel_modelData(pModel)), i_this->m32C, 1, 2, 1.0f, 0, -1,
                       true, 0);
    mDoExt_btpAnm_entry(i_this->m330, gabi::at<J3DModelData>(J3DModel_modelData(pModel)), i_this->m334);

    u32 md = J3DModel_modelData(pModel);
    if (i_this->mDamageType == 0) {
        /* HD: joint and materials looked up by name */
        u32 joint = gabi::load<u32>(md + 8); /* getJointNodePointer(KI_JNT_KI_ALLROOT_e) */
        u32 material = ki_getMaterialByName(md, 0x10012C78 /* "MX_ki_eye_l" */);
        u32 shape = gabi::load<u32>(material + 8);
        gabi::store<u8>(shape + 4, 0); /* shape->hide() */
        i_this->mpMorf->entryDL();
        dComIfGd_setListMaskOff();
        gabi::store<u8>(shape + 4, 1); /* shape->show() */
        /* HD: no getMatPacket(0)->unlock() */

        material = ki_getMaterialByName(md, 0x10012C84 /* "m_ki_main" */);
        gabi::store<u8>(gabi::load<u32>(material + 8) + 4, 0);
        J3DJoint_entryIn(pModel, joint);
        gabi::store<u8>(gabi::load<u32>(material + 8) + 4, 1);

        dComIfGd_setList();
    } else {
        mDoExt_btkAnm_entry(i_this->m920, gabi::at<J3DModelData>(md), gabi::load<f32>(gabi::ea(i_this->m920.get()) + 4));

        dComIfGd_setListMaskOff();
        i_this->mpMorf->entryDL();
        dComIfGd_setList();
    }

    /* HD: no blob shadow here */
    return TRUE;
}
VERIFY(0x02199748, daKi_Draw);

/* 02199A64 */
static void ki_pos_move(ki_class* i_this, s8 arg1) {
    WWHD_FUNC(0x02199A64, void, i_this, arg1);
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
    gabi::Local<cXyz> sp0C;
    sp0C->x = 0.0f;
    sp0C->y = 0.0f;
    sp0C->z = i_this->actor.speedF;
    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->actor.current.angle.x);
    MtxPosition(sp0C, &i_this->actor.speed);
    i_this->actor.current.pos.x += i_this->actor.speed.x;
    i_this->actor.current.pos.y += i_this->actor.speed.y;
    i_this->actor.current.pos.z += i_this->actor.speed.z;
}
VERIFY(0x02199A64, ki_pos_move);

/* 0219CE90 */
static BOOL daKi_IsDelete(ki_class*) {
    WWHD_FUNC(0x0219CE90, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0219CE90, daKi_IsDelete);

/* 0219CE98 */
static BOOL daKi_Delete(ki_class* i_this) {
    WWHD_FUNC(0x0219CE98, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10012D50));

    if (i_this->m339 != 0) {
        hio_set() = 0;
        mDoHIO_deleteChild(l_kiHIO().mNo);
    }

    if (i_this->actor.heap != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }

    if (i_this->mDamageType != 0) {
        /* m908.remove(): virtual (HD vtable at +0, slot +0x44) */
        u32 vt = i_this->m908.__vtbl;
        gabi::call_ptr(gabi::load<u32>(vt + 0x44), &i_this->m908);
    }

    enemy_fire_remove(&i_this->mEnemyFire);
    return TRUE;
}
VERIFY(0x0219CE98, daKi_Delete);

/* 0219CF38 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0219CF38, BOOL, a_this);
    ki_class* i_this = (ki_class*)a_this;
    J3DModel* model;

    if (i_this->mDamageType == 0) {
        i_this->actor.gbaName = 0x17;
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BDL_KI_e, SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BCK_WAIT1_e, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BAS_WAIT1_e, SAFESTRING_VTBL);
        i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                bas, 0x80000, 0x37221203);
        if (i_this->mpMorf == nullptr || (model = i_this->mpMorf->getModel()) == nullptr) {
            return FALSE;
        }
    } else {
        i_this->actor.gbaName = 6;
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BDL_FK_e, SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BCK_WAIT1_e, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BAS_WAIT1_e, SAFESTRING_VTBL);
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
            /* harness workaround: the matcher's name for 025E7C6C ("mDoExt_btkAnm::init") gives it a
             * GameCube stack parameter, so the harness compares the word at SP+8; in the original it
             * still holds the McaMorf constructor's stack argument (1) */
            struct Frame { be<u32> w[4]; };
            gabi::Local<Frame> fr;
            fr->w[2] = 1;
            i_this->m920 = p != nullptr ? mDoExt_btkAnm_ct(p) : nullptr;
        }
        if (i_this->m920 == nullptr) {
            return FALSE;
        }

        void* key = dComIfG_getObjectRes(STR(0x10012D53), dRes_INDEX_KI_BTK_FK_e, SAFESTRING_VTBL);
        if (!mDoExt_btkAnm_init(i_this->m920, gabi::at<J3DModelData>(J3DModel_modelData(model)), key, 1,
                                J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }

    for (u16 i = 0; i < J3DModelData_getJointNum(J3DModel_modelData(model)); i++) {
        if (i == 0xE /* KI_JNT_J_KI_HEAD_e */) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack): HD joints are 0x1C-byte elements,
             * an index past the end yields element 0 */
            u32 md = J3DModel_modelData(model);
            u32 n = gabi::load<u32>(md + 4);
            u32 p = gabi::load<u32>(md + 8);
            if (n > 0xE) p += 0xE * 0x1C;
            gabi::store<u32>(p + 8, 0x021991AC /* nodeCallBack */);
        }
    }

    u32 anmTexPattern = 0;
    for (s32 i = 0; i < 4; i++) {
        anmTexPattern = gabi::ea(dComIfG_getObjectRes(STR(0x10012D53), ki_tex_anm_idx(i), SAFESTRING_VTBL));
        /* anmTexPattern->searchUpdateMaterialID(modelData): HD virtual (vtable at +4, slot +0x1C) */
        u32 fn = gabi::load<u32>(gabi::load<u32>(anmTexPattern + 4) + 0x1C);
        gabi::call_ptr(fn, anmTexPattern, J3DModel_modelData(model));
    }

    /* HD: an mDoExt_btpAnm replaces the J3DTexNoAnm array */
    i_this->m330 = new_mDoExt_btpAnm();
    mDoExt_btpAnm_init(i_this->m330, gabi::at<J3DModelData>(J3DModel_modelData(model)), gabi::at<J3DAnmTexPattern>(anmTexPattern),
                       1, 2, 1.0f, 0, -1, false, 0);

    /* HD: no tex_anm_set(i_this, 1) here */
    gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    /* HD: the shadow packet is set up only in Ganon's Tower ("GanonK"):
     * if (sead::SafeString("GanonK") == dComIfGp_getStartStageName()) */
    gabi::Local<SafeString> a;
    a->mStringTop = 0x10012D58; /* "GanonK" */
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    {
        u32 stage = dComIfGp_ea() + 0x5134;
        b->__vtbl = SAFESTRING_VTBL;
        b->mStringTop = stage;
    }
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 bv = b->__vtbl;
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(bv + 0x14), b.get());
    u32 s2 = b->mStringTop;
    bool eq = false;
    if (s1 == s2) {
        eq = true;
    } else {
        for (u32 n = 0x40001; n != 0; n--) {
            u8 c1 = gabi::load<u8>(s1);
            u8 c2 = gabi::load<u8>(s2);
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
VERIFY(0x0219CF38, useHeapInit);

/* 0219D368: enemyfire::enemyfire (compiler-generated, HD) */
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {
    WWHD_FUNC(0x0219D368, enemyfire_l*, p);
    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(p) + 0x8C == 0) /* cXyz mDirection: GHS null-check of the member address */
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    gabi::call(0x025166F0, &p->mSph); /* dCcD_Sph::dCcD_Sph */
    p->m228 = 1.0f;
    return p;
}
VERIFY(0x0219D368, enemyfire_ct);

/* dBgS_ObjAcch inline constructor: this TU's vtables */
static void ki_ObjAcch_ct(dBgS_ObjAcch* a) {
    gabi::call(0x024F0474, a); /* dBgS_Acch::dBgS_Acch */
    gabi::store<u32>(gabi::ea(a) + 0x10, 0x10012BA4);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x10012BC4);
    gabi::store<u8>(gabi::ea(a) + 0x18, 1);
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x10012BB4);
}

/* 0219D3F4 */
static ki_class* ki_class_ct(ki_class* p) {
    WWHD_FUNC(0x0219D3F4, ki_class*, p);
    if (p == nullptr) {
        p = (ki_class*)operator_new(0x10BC);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(&p->actor);
    p->actor.__vtbl = KI_VTBL;
    gabi::call(0x02080404, p->m2B8); /* mDoExt_J3DModelPacketS (HD) */
    gabi::call(0x024EFE94, &p->mAcchCir);
    ki_ObjAcch_ct(&p->mAcch);
    dCcD_Stts_ct(&p->mStts);
    gabi::call(0x025166F0, &p->m580);
    gabi::call(0x025166F0, &p->m6AC);
    gabi::call(0x025166F0, &p->mDamageSphere);
    gabi::call(0x025A5894, &p->m908, 0, 0); /* dPa_followEcallBack(0, 0) */
    /* enemyice (inline) */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, 0x10012B74);
    gabi::call(0x024EFE94, &p->mEnemyIce.mBgAcchCir);
    ki_ObjAcch_ct(&p->mEnemyIce.mBgAcch);
    enemyfire_ct(&p->mEnemyFire);
    return p;
}
VERIFY(0x0219D3F4, ki_class_ct);

/* 0219D568 */
static cPhs_State daKi_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0219D568, cPhs_State, a_this);
    ki_class* i_this = (ki_class*)a_this;

    /* fopAcM_ct(a_this, ki_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            ki_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x10012D70));
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
        a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x10012D68) /* "keeth" */, 0);

        if (!fopAcM_entrySolidHeap(a_this, 0x0219CF38 /* useHeapInit */, 0x4E00)) {
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

        i_this->m580.Set(gabi::at<dCcD_SrcSph>(0x101B819C));          /* at_sph_src */
        i_this->m6AC.Set(gabi::at<dCcD_SrcSph>(0x101B821C));          /* co_sph_src */
        i_this->mDamageSphere.Set(gabi::at<dCcD_SrcSph>(0x101B81DC)); /* tg_sph_src */

        i_this->m580.SetStts(&i_this->mStts);
        i_this->m6AC.SetStts(&i_this->mStts);
        i_this->mDamageSphere.SetStts(&i_this->mStts);

        if (i_this->mDamageType != 0) {
            i_this->m580.SetAtType(0x200 /* AT_TYPE_FIRE */);
            i_this->mDamageSphere.SetTgType(0xFF3DFCFF);
        }

        i_this->m580.OffAtSPrmBit(2 /* cCcD_AtSPrm_VsEnemy_e */);
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */

        if (!hio_set()) {
            l_kiHIO().mNo = mDoHIO_createChild(STR(0x10012D74) /* "キース" */, &l_kiHIO());
            i_this->m339 = 1;
            hio_set() = 1;
        }

        i_this->mEnemyIce.mpActor = a_this;
        i_this->mEnemyIce.mWallRadius = REG0_F(4) + 30.0f;
        i_this->mEnemyIce.mCylHeight = REG0_F(5) + 30.0f;
        i_this->mEnemyIce.mParticleScale = 0.5f;
        i_this->mEnemyIce.mYOffset = 10.0f;
        i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
        i_this->mEnemyFire.mpActor = a_this;

        for (int i = 0; i < 10; i++) {
            i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101B8284 + i);           /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101B825C + 4 * i);     /* fire_sc */
        }

        gabi::call<BOOL>(0x0219AAE8, i_this); /* daKi_Execute */
    }
    return ret;
}
VERIFY(0x0219D568, daKi_Create);

/* 0219D920: static initialisation (header statics, l_kiHIO) */
static void __sinit_d_a_ki_cpp() {
    WWHD_FUNC(0x0219D920, void, (u32)0);
    sinit_header_statics(0x10464B30, 0x101B8290);
    kiHIO_c& h = l_kiHIO();
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
VERIFY(0x0219D920, __sinit_d_a_ki_cpp);

/* 0219DAC4: deleting destructor of a trivially destructible class of this TU (sead::SafeString
 * slot 1 of the vtable at 0x10012B5C) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0219DAC4, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0219DAC4, SafeString_dt);

/* dBgS_ObjAcch inline destructor (this TU's vtables) */
static void ki_ObjAcch_dt(dBgS_ObjAcch* a) {
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x10012BB4);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x10012BC4);
    gabi::call(0x024EFD9C, a, 0); /* dBgS_Acch::~dBgS_Acch */
}

/* 0219DAD8: ki_class deleting destructor (compiler-generated, HD virtual destructor) */
static void ki_class_dt(ki_class* p, s32 flags) {
    WWHD_FUNC(0x0219DAD8, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02515AE8, &p->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&p->mEnemyFire.mStts, 2);
        ki_ObjAcch_dt(&p->mEnemyIce.mBgAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mEnemyIce.mBgAcchCir) + 0x14), 2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&p->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&p->mEnemyIce.mStts, 2);
        gabi::call(0x02515AE8, &p->mDamageSphere, 2);
        gabi::call(0x02515AE8, &p->m6AC, 2);
        gabi::call(0x02515AE8, &p->m580, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        ki_ObjAcch_dt(&p->mAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2);
        gabi::call(0x02082DDC, p->m2B8, 2); /* mDoExt_J3DModelPacketS::~ */
        gabi::call(0x025D50BC, p, 0);        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0219DAD8, ki_class_dt);

/* 0219DBF8: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x0219DBF8, void, (u32)0);
}
VERIFY(0x0219DBF8, SafeString_assureTerminationImpl);
