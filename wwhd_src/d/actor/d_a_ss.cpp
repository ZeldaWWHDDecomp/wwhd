/**
 * d_a_ss.cpp (WWHD)
 * Enemy - ss (a core with ten tentacles drawn as 3D lines)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_ss.cpp) has only "Nonmatching" stubs for this actor: the functions here are
 * written from the WWHD code (cking.rpx), with the GameCube names and signatures, and verified
 * against it.
 */
#include "d/actor/d_a_ss.h"

const gc_tu_data gc_tu = {.safeString = 0x1003D124};

static const char* l_arcName = "Ss";
// HD: sound animations are loaded from the Bb archive.
static const char* l_soundArcName = "Bb";
static SsSphereSource tg_sph_src = {0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0xff1ffeff, 0x3,
                                    0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x5,  0xc,        0x0,
                                    0x0, 0x3, 0x0, 0.0f, 0.0f, 0.0f, 15.0f};

#define SS_SAFESTRING_VTBL 0x1003D124         /* this TU's sead::SafeString vtable */
#define SS_VTBL 0x1003D1DC                    /* ss_class vtable (HD virtual destructor) */
#define l_color gabi::at<GXColor>(0x1046DF54) /* line colour (file static) */

/* ss_s / ss_class structors (compiler-generated) */
// HD: ss_s::ss_s()
/* 0248D724 */
static ss_s* ss_s_ct(ss_s* p) {

    if (p == nullptr) {
        p = (ss_s*)operator_new(sizeof(ss_s));
        if (p == nullptr)
            return p;
    }
    construct_array(gabi::ea(p->mSph), 4, 0x12C, SS_SPHERE_CONSTRUCTOR);
    return p;
}

// HD: ss_s::~ss_s() (deleting)
/* 0248D77C */
static void ss_s_dt(ss_s* p, s32 flags) {

    if (p != nullptr) {
        destroy_arr(gabi::ea(p->mSph), 4, 0x12C, SS_SPHERE_DESTRUCTOR);
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: ss_class::ss_class()
/* 0248D390 */
static ss_class* ss_class_ct(ss_class* p) {

    if (p == nullptr) {
        p = (ss_class*)operator_new(sizeof(ss_class));
        if (p == nullptr)
            return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = SS_VTBL;
    construct_array(gabi::ea(p->mHand), 10, sizeof(ss_s),
                    gabi::guest_fn(reinterpret_cast<const void*>(ss_s_ct)));
    lineMat0_ct(&p->mLineMat);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Sph_ct(&p->mSph);
    return p;
}

// HD: ss_class::~ss_class() (deleting)
/* 0248D7E4 */
static void ss_class_dt(ss_class* p, s32 flags) {

    if (p != nullptr) {
        dCcD_Sph_dt(&p->mSph, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        lineMat0_dt(&p->mLineMat, 2);
        destroy_arr(gabi::ea(p->mHand), 10, sizeof(ss_s),
                    gabi::guest_fn(reinterpret_cast<const void*>(ss_s_dt)));
        ssActorDestroy(p);
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: deleting destructor of a class with a trivial destructor (this TU's vtable 1003D128)
/* 0248D6E4 */
static void trivial_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: empty virtual (same vtable)
/* 0248D87C */
static void empty_virtual(void* p) {}

/* 0248D6F8 / 0248D70C: JMath::TSinCosTable::sinShort / cosShort (per-TU copies) */
/* 0248D6F8 */
static f32 sinShort(u32 table, s16 a) { return gabi::at<SsSinCosEntry>(table)[(u16)a >> 3].mSin; }
/* 0248D70C */
static f32 cosShort(u32 table, s16 a) { return gabi::at<SsSinCosEntry>(table)[(u16)a >> 3].mCos; }

/* 0248AAC0 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        ss_class* i_this = static_cast<ss_class*>(model->getUserArea());
        s32 jntNo = reinterpret_cast<SsJoint*>(joint)->mJntNo;
        if (i_this != nullptr && jntNo == 1) {
            /* model->getAnmMtx(jntNo) (HD: marks the joint matrices dirty) */
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (i_this->mType != 0)
                cMtx_XrotM(calc_mtx(), 0x4000);
            cMtx_YrotM(calc_mtx(), i_this->mHeadRotY);
            cMtx_XrotM(calc_mtx(), i_this->mHeadRotX);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

// HD: hand_draw is inlined.
/* 0248AC00 */
static BOOL daSs_Draw(ss_class* i_this) {

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
    if (i_this->mMode < SS_DEAD) {
        J3DModel* model = i_this->mpMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        i_this->mpMorf->entryDL();
    }
    /* hand_draw */
    lineMat0_update(&i_this->mLineMat, 10, l_color, &i_this->tevStr);
    dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c*>(&i_this->mLineMat));
    return TRUE;
}

/* 0248ACB8 */
static void anm_init(ss_class* i_this, int anmResIdx, float morf, unsigned char loopMode,
                     float speed, int soundAnmResIdx) {

    if (soundAnmResIdx >= 0) {
        void* anm = dComIfG_getObjectRes(l_arcName, anmResIdx, SS_SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes(l_soundArcName, soundAnmResIdx, SS_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)anm, loopMode, morf, speed, 0.0f, -1.0f, sound);
    } else {
        void* anm = dComIfG_getObjectRes(l_arcName, anmResIdx, SS_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm((J3DAnmTransform*)anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 0248D1B8 */
static BOOL daSs_IsDelete(ss_class*) { return TRUE; }

/* 0248D1C0 */
static BOOL daSs_Delete(ss_class* i_this) {

    dComIfG_resDelete(&i_this->mPhs, l_arcName);
    return TRUE;
}

/* 0248D1F0 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {

    ss_class* i_this = (ss_class*)a_this;
    if (!lineMat0_init(&i_this->mLineMat, 10, 10, 1))
        return FALSE;

    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(l_arcName, 0xC, SS_SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(l_arcName, 8, SS_SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2, 1.0f, 0,
                                            -1, 1, nullptr, 0x80000, 0x11000022);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr)
        return FALSE;

    J3DModel* model = i_this->mpMorf->getModel();
    for (u16 i = 0; i < model->getModelData()->getJointNum(); i++) {
        if (i == 1) {
            /* modelData->getJointNodePointer(1)->setCallBack(nodeCallBack) */
            J3DModelData* data = model->getModelData();
            u32 joint = data->mJointNodes;
            if (data->mJointNum > 1)
                joint += 0x1C;
            gabi::at<J3DJoint>(joint)->setCallBack(nodeCallBack);
        }
    }
    model->mUserArea = gabi::ea(i_this);
    return TRUE;
}

/* 0248D434 */
static cPhs_State daSs_Create(fopAc_ac_c* a_this) {

    ss_class* i_this = (ss_class*)a_this;
    /* fopAcM_ct(i_this, ss_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            ss_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhs, l_arcName);
    if (res == cPhs_ERROR_e)
        return cPhs_ERROR_e;
    if (res != cPhs_COMPLEATE_e)
        return res;

    u32 prm = fopAcM_GetParam(a_this);
    i_this->mType = prm & 0xFF;
    i_this->mKind = (prm >> 8) & 0xFF;
    u8 sw = prm >> 24;
    if (sw == 0xFF) {
        i_this->mSwNo = 0;
    } else {
        i_this->mSwNo = sw;
        if (sw != 0 && dComIfGs_isSwitch(sw, fopAcM_GetRoomNo(a_this)))
            return cPhs_ERROR_e;
    }

    if (i_this->mKind == 1) {
        i_this->mMode = SS_SHIELDED_INIT;
    } else if (i_this->mKind == 2 || i_this->mKind == 3) {
        i_this->mMode = SS_OPENING_INIT;
    } else {
        i_this->mMode = SS_OPEN_INIT;
    }
    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)),
                               0x7240))
        return cPhs_ERROR_e;

    a_this->health = 2;
    s16 cnt = (s16)gabi::ftoi(cM_rndF(10000.0f));
    a_this->health = 2;
    i_this->mCounter = cnt;
    a_this->max_health = 2;
    i_this->mStts.Init(0x32, 0, a_this);
    i_this->mSph.Set(reinterpret_cast<dCcD_SrcSph*>(&tg_sph_src));
    i_this->mSph.SetStts(&i_this->mStts);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            dCcD_Sph* sph = &i_this->mHand[i].mSph[j];
            sph->Set(reinterpret_cast<dCcD_SrcSph*>(&tg_sph_src));
            sph->SetStts(&i_this->mStts);
            sph->mGObjTg.mSPrm = sph->mGObjTg.mSPrm & ~1u; /* OffTgShield */
        }
    }
    return cPhs_COMPLEATE_e;
}

// HD: static initialisation of the translation unit (header statics only)
/* 0248D650 */
static void __sinit_d_a_ss_cpp() { sinit_header_statics(0x1046DF58, 0x101D0BEC); }

/* ====================================================================================
 * daSs_Execute (0248ADE8, 9 KB): core_move, hand_move and the hand_1_* functions are inlined.
 * ==================================================================================== */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define non_pos gabi::at<cXyz>(0x1046DF74) /* static cXyz (0, -10000, 0) */
#define SIN_TABLE 0x104A44F8u
#define REGF(c, i) REG_F(c, i)
#define REGS(c, i) REG_S(c, i)

/* pointing the current matrix along a vector */
static inline void ss_rot_to(cXyz* d) {
    Mtx34* m = calc_mtx();
    s16 ay = cM_atan2s(d->x, d->z);
    mDoMtx_YrotS(m, ay);
    f32 zz = d->z * d->z;
    f32 xz2 = gabi::fmadds(d->x, d->x, zz);
    m = calc_mtx();
    f32 xz = std_sqrtf(xz2);
    s16 ax = cM_atan2s(d->y, xz);
    mDoMtx_XrotM(m, (s16)-ax);
}

/* hand_1_set: tentacle of a ceiling type ss (mType == 1) */
static inline void hand_1_set(ss_class* i_this, ss_s* hand) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    gabi::Local<dBgS_LinChk> lc;
    dBgS_LinChk* linChk = lc.get();
    dBgS_LinChk_ct(lc.get(), ss_linchk_vt, false);
    gabi::Local<cXyz> sp70, spA0, spAC, sp7C;

    mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
    sp70->x = 0.0f;
    sp70->y = 0.0f;
    sp70->z = REGF(12, 6) + -250.0f;
    MtxPosition(sp70, spA0);
    PSVECAdd(spA0, &hand->mPos, spA0);
    mDoMtx_ZrotM(calc_mtx(), hand->mAngleZ);
    sp70->x = 0.0f;
    sp70->z = 0.0f;
    sp70->y = REGF(12, 7) + 20.0f;
    s16 ang = 0;
    for (int j = 0; j < 10; j++) {
        ss_seg* seg = &hand->mSeg[j];
        seg->mSize = 0;
        MtxPush();
        mDoMtx_ZrotM(calc_mtx(), ang);
        f32 r = cM_rndFX(gabi::fmadds((f32)j, 800.0f, 2000.0f));
        ang = (s16)(ang + (s16)gabi::ftoi(r));
        MtxPosition(sp70, spAC);
        MtxPull();
        PSVECAdd(spA0, spAC, spA0);
        sp70->y = sp70->y + (REGF(12, 9) + 3.0f);
        dBgS_LinChk_Set(lc.get(), &hand->mPos, spA0, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), lc.get())) {
            seg->mPos.copy(*linChk_cross(linChk));
            cXyz_mi(spA0, sp7C, &seg->mPos);
            MtxPush();
            ss_rot_to(sp7C);
            f32 z = REGF(12, 8) + -5.0f;
            sp7C->x = 0.0f;
            sp7C->y = 0.0f;
            sp7C->z = z;
            MtxPosition(sp7C, spAC);
            PSVECAdd(&seg->mPos, spAC, &seg->mPos);
            MtxPull();
        }
    }
    ss_LinChk_dt(linChk);
}

/* hand_1_set_2: tentacle of a floor type ss */
static inline void hand_1_set_2(ss_class* i_this, ss_s* hand) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    gabi::Local<dBgS_LinChk> lc;
    dBgS_LinChk* linChk = lc.get();
    dBgS_LinChk_ct(lc.get(), ss_linchk_vt, false);
    gabi::Local<cXyz> sp70, spA0, spAC, spC4, sp88, sp13C;

    mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
    spC4->copy(hand->mPos);
    sp70->x = 0.0f;
    sp70->y = 0.0f;
    sp70->z = REGF(10, 5) + 20.0f;
    spA0->x = hand->mPos.x;
    f32 py = hand->mPos.y;
    spA0->z = hand->mPos.z;
    spA0->y = py - (REGF(10, 6) + 300.0f);
    s16 ang = 0;
    for (int j = 0; j < 10; j++) {
        ss_seg* seg = &hand->mSeg[j];
        seg->mSize = 0;
        MtxPush();
        mDoMtx_YrotM(calc_mtx(), ang);
        f32 r = cM_rndFX(gabi::fmadds((f32)j, 800.0f, 2000.0f));
        ang = (s16)(ang + (s16)gabi::ftoi(r));
        MtxPosition(sp70, spAC);
        MtxPull();
        PSVECAdd(spC4, spAC, spC4);
        cXyz_ml(spAC, sp13C, REGF(10, 3) + 0.3f);
        PSVECAdd(spA0, sp13C, spA0);
        sp70->z = sp70->z + (REGF(10, 7) + 3.0f);
        dBgS_LinChk_Set(lc.get(), spC4, spA0, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), lc.get())) {
            seg->mPos.copy(*linChk_cross(linChk));
            cXyz_mi(spC4, sp88, &seg->mPos);
            MtxPush();
            ss_rot_to(sp88);
            f32 z = REGF(8, 8) + 5.0f;
            sp88->x = 0.0f;
            sp88->y = 0.0f;
            sp88->z = z;
            MtxPosition(sp88, spAC);
            PSVECAdd(&seg->mPos, spAC, &seg->mPos);
            MtxPull();
        }
    }
    ss_LinChk_dt(linChk);
}

/* hand_1_move: a grown tentacle (collision spheres, cut check). `info` shares its stack slot
 * with hand_1_cut's ground check (as in the original frame). */
static inline void hand_1_move(ss_class* i_this, ss_s* hand, CcAtInfo_ss* info) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    s8 n = hand->mLen;
    if ((i_this->mCounter & 3) == 0 && i_this->mMode < SS_DEAD && n < 10)
        hand->mLen = n + 1;

    int cnt = 0;
    for (int k = 0; k < 10; k++) {
        ss_seg* seg = &hand->mSeg[k];
        if (k >= n - 1)
            seg->mSize = 0;
        else if (k == n - 2)
            seg->mSize = 1;
        else if (k == n - 3)
            seg->mSize = 2;
        else if (k == n - 4)
            seg->mSize = 3;
        else
            seg->mSize = 4;
        if (i_this->mMode < SS_DEAD) {
            s32 c = i_this->mCounter & 3;
            if ((k == c + 2 || k == c + 7 || k == c + 12 || k == c + 17) && cnt < 4) {
                dCcD_Sph* sph = &hand->mSph[cnt];
                sph->mSph.SetC(&seg->mPos);
                dComIfG_Ccsp_Set(sph);
                cnt++;
            }
        }
    }

    s32 cut = 0;
    for (int j = 0; j < 4; j++) {
        if (hand->mSph[j].ChkTgHit()) {
            info->mpObj = gabi::ea(hand->mSph[j].GetTgHitObj());
            if (info->mpObj != 0) {
                info->mpActor = at_power_check(info);
                if (info->mResultingAttackType == 5)
                    cut = 2;
                else if (info->mpObj != 0 && hitObjIsWind(info->mpObj))
                    cut = 0;
                else
                    cut = 1;
            }
            break;
        }
    }
    if (cut == 0 && i_this->mMode < SS_DEAD)
        return;

    dComIfGs_onEventBit(0x2B20);
    hand->mMode = SS_HAND_CUT;
    hand->mPos.copy(hand->mSeg[9].mPos);
    hand->mGround = 0.0f;
    if (cut != 0)
        def_se_set(i_this, info->mpObj, 0x21);
    hand->mHeight = REGF(8, 3) + 20.0f;
    hand->mSwing = 0.0f;
    hand->mSwingPhase = (s16)gabi::ftoi(cM_rndF(65536.0f));
    hand->mSwingStep = (s16)gabi::ftoi(cM_rndF(3000.0f) + 3000.0f);
    hand->mRotSpeed = (s16)gabi::ftoi(cM_rndFX(2000.0f));
    gabi::Local<cXyz> sp70;
    if (cut == 2) {
        hand->mBurn = 1;
        sp70->z = 5.0f;
        sp70->y = 5.0f;
        hand->mHoldTimer = 2;
        sp70->x = 0.0f;
        mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
        MtxPosition(sp70, &hand->mSpeed);
    } else {
        f32 a = REGF(8, 12) + 25.0f;
        f32 r1 = cM_rndF(7.0f);
        sp70->y = a + r1;
        f32 b = REGF(8, 13) + 7.0f;
        f32 r2 = cM_rndF(5.0f);
        sp70->z = b + r2;
        hand->mHoldTimer = REGS(8, 7) + 8;
        sp70->x = 0.0f;
        mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
        MtxPosition(sp70, &hand->mSpeed);
    }
}

/* hand_1_cut: a cut tentacle falls and burns */
static inline void hand_1_cut(ss_class* i_this, ss_s* hand, dBgS_GndChk* gndChk) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    gabi::Local<cXyz> spAC, sp70, sp94, spA0, spC4, spDC;
    spAC->y = 0.0f;
    spAC->x = 0.0f;
    spAC->z = REGF(8, 11) + 15.0f;
    cXyz* gndPos = &gndChk->m_pos;

    if (hand->mHoldTimer != 0) {
        hand->mHoldTimer = hand->mHoldTimer - 1;
        hand->mSeg[0].mPos.copy(i_this->home.pos);
        for (int k = 1; k < 10; k++) {
            cXyz_mi(&hand->mSeg[k].mPos, sp94, &hand->mSeg[k - 1].mPos);
            sp70->y = sp94->y;
            sp70->x = sp94->x;
            sp70->z = sp94->z;
            ss_rot_to(sp70);
            MtxPosition(spAC, sp70);
            cXyz_pl(&hand->mSeg[k - 1].mPos, sp94, sp70);
            hand->mSeg[k].mPos.copy(*sp94);
        }
        hand->mSeg[9].mPos.copy(hand->mPos);
        dBgS_GndChk_ct(gndChk, ss_gndchk_vt, false);
        cLib_addCalc2(&hand->mHeight, REGF(8, 2) + -20.0f, 1.0f, REGF(8, 4) + 1.0f);
    } else {
        cLib_addCalc2(&hand->mSwing, REGF(8, 15) + 10.0f, 1.0f, REGF(8, 16) + 1.0f);
        hand->mSeg[9].mPos.copy(hand->mPos);
        hand->mSwingPhase = hand->mSwingPhase + 100;
        dBgS_GndChk_ct(gndChk, ss_gndchk_vt, false);
        cLib_addCalc2(&hand->mHeight, REGF(8, 2) + -20.0f, 1.0f, REGF(8, 4) + 1.0f);
    }

    spA0->y = 0.0f;
    for (int k = 8; k >= 0; k--) {
        ss_seg* seg = &hand->mSeg[k];
        gndPos->x = seg->mPos.x;
        gndPos->z = seg->mPos.z;
        gndPos->y = seg->mPos.y + 50.0f;
        f32 gnd = cBgS_GroundCross(dComIfG_Bgsp(), gndChk) + 5.0f;
        f32 y = seg->mPos.y + hand->mHeight;
        f32 floor = gnd + hand->mGround;
        f32 zoff = REGF(8, 17);
        if (y < floor)
            y = floor;
        spA0->z = zoff + -2.0f;
        spA0->x = cM_ssin((s16)(hand->mSwingPhase + k * hand->mSwingStep)) * hand->mSwing;
        mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
        MtxPosition(spA0, spC4);
        sp70->x = (seg->mPos.x - hand->mSeg[k + 1].mPos.x) + spC4->x;
        sp70->y = y - hand->mSeg[k + 1].mPos.y;
        sp70->z = (seg->mPos.z - hand->mSeg[k + 1].mPos.z) + spC4->z;
        ss_rot_to(sp70);
        MtxPosition(spAC, sp70);
        if (hand->mHoldTimer != 0 && k == 0) {
            seg->mPos.copy(i_this->home.pos);
            if (hand->mBurn != 0)
                dComIfGp_particle_setSimple(1, &seg->mPos);
        } else {
            cXyz_pl(&hand->mSeg[k + 1].mPos, spDC, sp70);
            seg->mPos.copy(*spDC);
            if ((k & 3) == 0 && hand->mBurn != 0)
                dComIfGp_particle_setSimple(1, &seg->mPos);
        }
    }

    PSVECAdd(&hand->mPos, &hand->mSpeed, &hand->mPos);
    f32 grav = REGF(8, 14) + 3.0f;
    f32 py = hand->mPos.y;
    hand->mSpeed.y = hand->mSpeed.y - grav;
    gndPos->x = hand->mPos.x;
    gndPos->y = py + 50.0f;
    gndPos->z = hand->mPos.z;
    f32 gnd = cBgS_GroundCross(dComIfG_Bgsp(), gndChk) + 5.0f;
    if (hand->mLandTimer == 0) {
        if (hand->mSpeed.y < -300.0f) {
            hand->mLen = 0;
            hand->mMode = SS_HAND_SET;
        }
        if (!(hand->mPos.y > gnd + hand->mGround))
            hand->mLandTimer = (s16)gabi::ftoi(cM_rndF(20.0f) + 40.0f);
    } else {
        cLib_addCalc0(&hand->mSpeed.x, 1.0f, REGF(8, 1) + 0.4f);
        cLib_addCalc0(&hand->mSpeed.z, 1.0f, REGF(8, 1) + 0.4f);
        hand->mAngleY = hand->mAngleY + hand->mRotSpeed;
        cLib_addCalcAngleS2(&hand->mRotSpeed, 0, 1, 0x1E);
        hand->mLandTimer = hand->mLandTimer - 1;
        if (hand->mLandTimer == 0) {
            hand->mBurn = 0;
            hand->mLen = 0;
            hand->mMode = SS_HAND_SET;
            fopAcM_seStart(i_this, 0x5868, 0);
        }
    }
    f32 floor = gnd + hand->mGround;
    if (hand->mPos.y < floor) {
        hand->mPos.y = floor;
        hand->mSpeed.y = -5.0f;
        cLib_addCalc2(&hand->mGround, REGF(0, 18) + -10.0f, 0.05f, REGF(0, 19) + 0.2f);
    }
    ss_GndChk_dt(gndChk);
}

/* hand_move */
static inline void hand_move(ss_class* i_this) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    gabi::Local<dBgS_LinChk> lc;
    dBgS_LinChk* linChk = lc.get();
    dBgS_LinChk_ct(lc.get(), ss_linchk_vt, false);
    non_pos->x = 0.0f;
    non_pos->y = -10000.0f;
    non_pos->z = 0.0f;
    gabi::Local<u8[0x54]>
        scratch; /* hand_1_move's CcAtInfo / hand_1_cut's dBgS_GndChk (one slot) */
    gabi::Local<cXyz> spD0, spB8;

    s16 acc = 0;
    for (int i = 0; i < 10; i++, acc = (s16)(acc + 0x1999)) {
        ss_s* hand = &i_this->mHand[i];
        for (int j = 0; j < 4; j++) {
            hand->mSph[j].mSph.SetC(non_pos);
            dComIfG_Ccsp_Set(&hand->mSph[j]);
        }
        switch (hand->mMode) {
        case SS_HAND_SET:
            if (i_this->mType == 1) {
                hand->mAngleY = (s16)(i_this->current.angle.y + 0x8000);
                mDoMtx_YrotS(calc_mtx(), hand->mAngleY);
                spD0->x = 0.0f;
                spD0->y = 0.0f;
                spD0->z = -1000.0f;
                MtxPosition(spD0, spB8);
                PSVECAdd(spB8, &i_this->current.pos, spB8);
                dBgS_LinChk_Set(lc.get(), &i_this->current.pos, spB8, i_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), lc.get())) {
                    cXyz* cross = linChk_cross(linChk);
                    f32 cx = cross->x, cy = cross->y, cz = cross->z;
                    hand->mPos.x = cx;
                    hand->mPos.y = cy;
                    hand->mPos.z = cz;
                    i_this->home.pos.y = cy;
                    i_this->home.pos.x = hand->mPos.x;
                    i_this->home.pos.z = hand->mPos.z;
                    spD0->z = REGF(12, 12) + 200.0f;
                    MtxPosition(spD0, spB8);
                    PSVECAdd(&hand->mPos, spB8, &hand->mPos);
                }
                hand->mAngleZ = acc;
                hand_1_set(i_this, hand);
            } else {
                hand->mPos.copy(i_this->current.pos);
                spB8->x = i_this->current.pos.x;
                f32 y = i_this->current.pos.y;
                spB8->y = y;
                spB8->z = i_this->current.pos.z;
                spB8->y = y - 1000.0f;
                dBgS_LinChk_Set(lc.get(), &hand->mPos, spB8, i_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), lc.get())) {
                    cXyz* cross = linChk_cross(linChk);
                    f32 cx = cross->x, cy = cross->y, cz = cross->z;
                    hand->mPos.x = cx;
                    hand->mPos.y = cy;
                    hand->mPos.z = cz;
                    i_this->home.pos.y = cy;
                    i_this->home.pos.x = hand->mPos.x;
                    i_this->home.pos.z = hand->mPos.z;
                    hand->mPos.y = hand->mPos.y + 50.0f;
                }
                hand->mAngleY = acc;
                hand_1_set_2(i_this, hand);
            }
            hand->mMode = hand->mMode + 1;
            break;
        case SS_HAND_MOVE:
            hand_1_move(i_this, hand, (CcAtInfo_ss*)scratch.get());
            break;
        case SS_HAND_CUT:
            hand_1_cut(i_this, hand, reinterpret_cast<dBgS_GndChk*>(scratch.get()));
            break;
        }
    }
    ss_LinChk_dt(linChk);

    /* the tentacles' segments into the line material */
    for (int i = 0; i < 10; i++) {
        SsLine* line = &i_this->mLineMat.mpLines[i];
        be<u8>* size = line->mpSize;
        cXyz* pos = line->mpSegments;
        ss_s* hand = &i_this->mHand[i];
        for (int k = 0; k < 10; k++) {
            pos[k].copy(hand->mSeg[k].mPos);
            size[k] = hand->mSeg[k].mSize;
        }
    }
}

/* core_move: the core's states, then its damage check */
static inline void core_move(ss_class* i_this) {
    (void)dComIfGp_get(); /* fopAc_ac_c* player = dComIfGp_getPlayer(0) (unused) */
    fopAc_ac_c* a_this = i_this;
    f32 dist = fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer(0));
    f32 range = 350.0f;
    if (i_this->mKind == 3)
        range = 1500.0f;

    switch (i_this->mMode) {
    case SS_OPEN_INIT:
        anm_init(i_this, 9, 1.0f, 2, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        /* fallthrough */
    case SS_OPEN:
        i_this->mShielded = 0;
        break;
    case SS_SHIELDED_INIT:
        anm_init(i_this, 8, 1.0f, 2, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        /* fallthrough */
    case SS_SHIELDED:
        i_this->mShielded = 1;
        break;
    case SS_OPEN_RETURN:
        anm_init(i_this, 9, 5.0f, 2, 1.0f, -1);
        i_this->mMode = SS_WAIT_OPEN;
        break;
    case SS_OPENING_INIT:
        anm_init(i_this, 7, 1.0f, 0, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        break;
    case SS_OPENING:
        if (morf_isStop(i_this->mpMorf)) {
            anm_init(i_this, 9, 1.0f, 2, 1.0f, -1);
            i_this->mMode = i_this->mMode + 1;
        }
        /* fallthrough */
    case SS_WAIT_OPEN:
        i_this->mShielded = 0;
        if (dist < range) {
            i_this->mMode = SS_CLOSING_INIT;
            fopAcM_seStart(a_this, 0x58F6, 0);
        }
        break;
    case SS_SHIELDED_RETURN:
        i_this->mMode = SS_WAIT_SHIELDED;
        anm_init(i_this, 8, 5.0f, 2, 1.0f, -1);
        break;
    case SS_CLOSING_INIT:
        anm_init(i_this, 4, 1.0f, 0, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        break;
    case SS_CLOSING:
        if (morf_isStop(i_this->mpMorf)) {
            anm_init(i_this, 8, 1.0f, 2, 1.0f, -1);
            i_this->mMode = i_this->mMode + 1;
        }
        /* fallthrough */
    case SS_WAIT_SHIELDED: {
        f32 r = range + 20.0f;
        i_this->mShielded = 1;
        if (dist > r) {
            i_this->mMode = SS_OPENING_INIT;
            fopAcM_seStart(a_this, 0x58F5, 0);
        }
        break;
    }
    case SS_DAMAGE_INIT:
        if (i_this->mShielded != 0) {
            anm_init(i_this, 6, 1.0f, 0, 1.0f, -1);
            i_this->mMode = i_this->mMode + 1;
        } else {
            anm_init(i_this, 5, 1.0f, 0, 1.0f, -1);
            i_this->mHeadSwingTimer = 0x14;
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    case SS_DAMAGE:
        if (morf_isStop(i_this->mpMorf)) {
            if (i_this->mPrevMode > 30)
                i_this->mMode = SS_SHIELDED_RETURN;
            else if (i_this->mPrevMode > 20)
                i_this->mMode = SS_OPENING_INIT;
            else if (i_this->mPrevMode > 10)
                i_this->mMode = SS_SHIELDED_INIT;
            else
                i_this->mMode = SS_OPEN_INIT;
        }
        break;
    case SS_DEAD:
        if (i_this->mTimer[0] == 0x45) {
            gabi::Local<cXyz> scale;
            scale->x = 0.5f;
            scale->z = 0.5f;
            scale->y = 0.5f;
            cXyz* eye = &a_this->eyePos;
            dComIfGp_particle_set(0x13, eye, nullptr, scale);
            dComIfGp_particle_set(0x16, eye, nullptr, scale);
            fopAcM_seStart(a_this, 0x586A, 0);
        }
        if (i_this->mTimer[1] != 0) {
            dComIfGp_particle_setSimple(1, &a_this->eyePos);
            for (int i = 0; i < 10; i++) {
                cXyz* p = i_this->mLineMat.mpLines[i].mpSegments;
                for (int j = 0; j < 10;) {
                    f32 r = cM_rndF(1.0f);
                    if (r < REGF(0, 15) + 0.2f)
                        dComIfGp_particle_setSimple(1, p);
                    s16 step = REGS(0, 7);
                    j += step + 2;
                    p += step + 2;
                }
                ss_s* hand = &i_this->mHand[i];
                hand->mMode = SS_HAND_MOVE;
                if ((i_this->mCounter & 1) == 0 && hand->mLen != 0)
                    hand->mLen = hand->mLen - 1;
            }
        }
        if (i_this->mTimer[0] == 0) {
            if (i_this->mSwNo != 0)
                dComIfGs_onSwitch(i_this->mSwNo, a_this->current.roomNo);
            fopAcM_delete(a_this);
        }
        break;
    }

    /* damage */
    cXyz* eye = &a_this->eyePos;
    if (i_this->mSph.ChkTgHit() && i_this->mDamageTimer == 0) {
        gabi::Local<CcAtInfo_ss> info;
        i_this->mDamageTimer = 10;
        info->mpObj = gabi::ea(i_this->mSph.GetTgHitObj());
        info->mpActor = at_power_check(info);
        if (info->mpObj != 0 && hitObjIsWind(info->mpObj))
            return;
        i_this->mPrevMode = i_this->mMode;
        i_this->mMode = SS_DAMAGE_INIT;
        if (i_this->mShielded == 0 || info->mResultingAttackType == 5) {
            info->mpObj = gabi::ea(i_this->mSph.GetTgHitObj());
            info->pParticlePos = gabi::ea(i_this->mSph.GetTgHitPosP());
            at_power_check(info);
            if (info->mResultingAttackType == 10 || info->mResultingAttackType == 3)
                a_this->health = 0;
            if (info->mResultingAttackType == 5) {
                a_this->health = 0;
                i_this->mTimer[1] = 50;
                fopAcM_seStart(a_this, 0x5869, 0);
            } else {
                cc_at_check(a_this, info);
            }
            if (a_this->health <= 0) {
                i_this->mDamageTimer = 100;
                i_this->mMode = SS_DEAD;
                i_this->mTimer[0] = 70;
            }
        } else {
            fopAcM_seStart(a_this, 0x69AF, 0);
        }
    }
    if (i_this->mMode >= SS_DEAD) {
        eye->y = 10000.0f;
        eye->z = 10000.0f;
        eye->x = 10000.0f;
    }
    i_this->mSph.mSph.SetC(eye);
    i_this->mSph.mSph.SetR(REGF(0, 0) + 35.0f);
    if (i_this->mShielded != 0)
        i_this->mSph.mGObjTg.mSPrm = i_this->mSph.mGObjTg.mSPrm | 1; /* OnTgShield */
    else
        i_this->mSph.mGObjTg.mSPrm = i_this->mSph.mGObjTg.mSPrm & ~1u; /* OffTgShield */
    dComIfG_Ccsp_Set(&i_this->mSph);
}

/* 0248ADE8 */
static BOOL daSs_Execute(ss_class* i_this) {

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    l_color->r = (u8)REGS(8, 3);
    l_color->g = (u8)(REGS(8, 4) + 0xB4);
    l_color->b = (u8)(REGS(8, 5) + 0x32);
    l_color->a = 0xFF;
    i_this->mCounter = i_this->mCounter + 1;
    for (int i = 0; i < 4; i++) {
        if (i_this->mTimer[i] != 0)
            i_this->mTimer[i] = i_this->mTimer[i] - 1;
    }
    if (i_this->mDamageTimer != 0)
        i_this->mDamageTimer = i_this->mDamageTimer - 1;

    core_move(i_this);
    hand_move(i_this);

    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
    MtxTrans(i_this->home.pos.x, i_this->home.pos.y, i_this->home.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    if (i_this->mType != 0)
        mDoMtx_XrotM(calc_mtx(), (s16)(REGS(8, 5) - 0x4000));
    J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), calc_mtx());

    gabi::Local<cXyz> sp148, sp254;
    sp148->x = 0.0f;
    sp148->z = 0.0f;
    sp148->y = 45.0f;
    MtxPosition(sp148, &i_this->eyePos);
    cXyz_mi(&player->eyePos, sp254, &i_this->eyePos);
    f32 dx = sp254->x, dz = sp254->z;
    sp148->x = dx;
    sp148->y = sp254->y;
    sp148->z = dz;

    f32 s = sinShort(SIN_TABLE, (s16)(i_this->mCounter * 0x2F00));
    f32 amp = (f32)((s8)i_this->mHeadSwingTimer * 200);
    s16 wy = (s16)gabi::ftoi(s * amp);
    f32 c = cosShort(SIN_TABLE, (s16)(i_this->mCounter * 0x2C00));
    s16 wx = (s16)gabi::ftoi(c * amp);
    s16 ay = cM_atan2s(dx, dz);
    i_this->mHeadRotY = (s16)(ay * (REGS(0, 0) + 1) - i_this->current.angle.y + wy);
    f32 zz = sp148->z * sp148->z;
    f32 xz = std_sqrtf(gabi::fmadds(sp148->x, sp148->x, zz));
    s16 ax = cM_atan2s(sp148->y, xz);
    i_this->mHeadRotX = (s16) - (ax * (REGS(0, 2) + 1) + wx);
    if ((s8)i_this->mHeadSwingTimer != 0)
        i_this->mHeadSwingTimer = i_this->mHeadSwingTimer - 1;
    i_this->mpMorf->calc();
    return TRUE;
}

// clang-format off
/* WWHD glue
str 1003D1F4 "Ss" in 0248ACB8
str 1003D1F7 "Bb" in 0248ACB8
str 1003D290 "Ss" in 0248D1C0
str 1003D293 "Ss" in 0248D1F0
str 1003D296 "Ss" in 0248D434
data tg_sph_src 101D0BAC
*/
// clang-format on
