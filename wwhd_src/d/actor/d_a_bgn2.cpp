/**
 * d_a_bgn2.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 2) / G (King crab)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn2.cpp) to the WWHD layout and verified against cking.rpx.
 * daBgn2_Execute (with move() and its action functions inlined) is in d_a_bgn2_exec.cpp.
 */
#include "d/actor/d_a_bgn2.h"

enum {
    dRes_INDEX_BGN_BCK_BGN_HEAD1_e = 0x6,
    dRes_INDEX_BGN_BCK_WAIT1_e = 0xF,
    dRes_INDEX_BGN_BDL_BGN_HEAD1_e = 0x15,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENA2_e = 0x17,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENB2_e = 0x1A,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENC2_e = 0x1D,
    dRes_INDEX_BGN_BDL_BGN_KUMO1_e = 0x1F,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENB2_e = 0x26,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENC2_e = 0x29,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 0x2E,
};

/* 0208F7C8 */
static daBgn2_HIO_c* daBgn2_HIO_c_ct(daBgn2_HIO_c* p) {
    WWHD_FUNC(0x0208F7C8, daBgn2_HIO_c*, p);
    if (p == nullptr) {
        p = (daBgn2_HIO_c*)operator_new(0x34);
        if (p == nullptr)
            return p;
    }
    p->mNo = -1;
    p->m05 = 0;
    p->m06 = 0;
    p->m08 = 280.0f;
    p->m0C = 3000.0f;
    p->m10 = 15.0f;
    p->m14 = -2.0f;
    p->m18 = 0x5dc;
    p->m1A = 0xa28;
    p->m1C = 0xed8;
    p->m1E = 0x3c;
    p->m20 = 0x32;
    p->m22 = 0x28;
    p->m24 = 100;
    p->m26 = 100;
    p->m28 = 100;
    p->m2A = 600;
    p->m2C = 100;
    p->m2E = 3;
    p->m30 = 10;
    p->__vtbl = BGN2_HIO_VTBL;
    return p;
}
VERIFY(0x0208F7C8, daBgn2_HIO_c_ct);

/* 0208AEF8 */
void anm_init(bgn2_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x0208AEF8, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10008EAC) /* "Bgn" */, bckFileIdx, BGN2_SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR(0x10008EAC), soundFileIdx, BGN2_SAFESTRING_VTBL);
        i_this->mpBodyMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10008EAC), bckFileIdx, BGN2_SAFESTRING_VTBL);
        i_this->mpBodyMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x0208AEF8, anm_init);

/* 0208B020 (the matcher calls it bgn3_s_sub: it finds fpcNm_BGN_e, 0xF3 in HD) */
static void* bgn_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0208B020, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xF3 /* fpcNm_BGN_e */) {
        return param_1;
    } else {
        return nullptr;
    }
}
VERIFY(0x0208B020, bgn_s_sub);

/* 0208B070 (the matcher calls it bgn_s_sub: it finds fpcNm_BGN3_e, 0xF5 in HD) */
static void* bgn3_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0208B070, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xF5 /* fpcNm_BGN3_e */) {
        return param_1;
    } else {
        return nullptr;
    }
}
VERIFY(0x0208B070, bgn3_s_sub);

/* 0208FAE0 */
static BOOL daBgn2_Draw(bgn2_class*) {
    WWHD_FUNC(0x0208FAE0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0208FAE0, daBgn2_Draw);

/* 0208B0C0 */
int gr_check(bgn2_class* i_this, cXyz* param_2) {
    WWHD_FUNC(0x0208B0C0, int, i_this, param_2);
    fopAc_ac_c* actor = i_this;
    gabi::Local<dBgS_LinChk_l> linChk;
    bgn2_LinChk_ct(linChk);

    gabi::Local<cXyz> local_ac;
    gabi::Local<cXyz> local_b8;
    f32 x = param_2->x;
    f32 y = param_2->y;
    f32 z = param_2->z;
    local_ac->x = x;
    local_ac->y = y + 200.0f;
    local_ac->z = z;
    local_b8->x = x;
    local_b8->y = y - 1000.0f;
    local_b8->z = z;
    dBgS_LinChk_Set(linChk, local_ac, local_b8, actor);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        param_2->copy(linChk->mCross);
        param_2->y = REG0_F(8) + -2.0f;
        if (dBgS_GetAttributeCode(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(linChk.get()) + 0x14)) == 0x13 /* dBgS_Attr_WATER_e */) {
            bgn2_LinChk_dt(linChk);
            return FALSE;
        }
    }
    bgn2_LinChk_dt(linChk);
    return TRUE;
}
VERIFY(0x0208B0C0, gr_check);

/* 0208B268
 * HD: the legs are m2B98[0..29] (GameCube i + 4, past the end of the array for the last four),
 * and the ripple frame test comes before the ground check */
void asi_hamon_set(bgn2_class* i_this) {
    WWHD_FUNC(0x0208B268, void, i_this);
    gabi::Local<cXyz> local_1c;
    gabi::Local<cXyz> local_28;

    local_28->x = 0.0f;
    local_28->z = 0.0f;
    local_28->y = 0.0f;
    settingTevStruct(dKy_getEnvlight(), 3 /* TEV_TYPE_BG2 */, local_28, bg_tevstr());
    for (int i = 0; i < 30; i++) {
        local_1c->copy(i_this->m2B98[i]);
        if (!((i_this->m0310 + i) & 7) && !gr_check(i_this, local_1c)) {
            dComIfGp_particle_setSimple(0x8407 /* ID_AK_SN_O_KGTCOMMONHAMON00 */, local_1c, 0xFF);
        }
    }
}
VERIFY(0x0208B268, asi_hamon_set);

/* 0208B340 */
int checkGround(bgn2_class* i_this) {
    WWHD_FUNC(0x0208B340, int, i_this);
    fopAc_ac_c* actor = i_this;
    f32 h = l_HIO().m08;
    if (actor->current.pos.y > h) {
        return FALSE;
    }
    actor->current.pos.y = h;
    actor->speed.y = 0.0f;
    return TRUE;
}
VERIFY(0x0208B340, checkGround);

/* 0208B378 */
void move_se_set(bgn2_class* i_this) {
    WWHD_FUNC(0x0208B378, void, i_this);
    fopAc_ac_c* actor = i_this;
    u32 uVar2;
    gabi::Local<cXyz> local_28;

    cXyz_mi(&actor->current.pos, local_28, &actor->old.pos);
    local_28->y = 0.0f;
    uVar2 = f2u(std_sqrtf(PSVECSquareMag(local_28)) * 3.5f);
    if (uVar2 > 100) {
        uVar2 = 100;
    }
    fopAcM_seStart(actor, 0x705A /* JA_SE_CM_BGN_MECHA_ROTATE */, uVar2);
    gabi::Local<cXyz> tmp;
    cXyz_mi(&actor->current.pos, tmp, &actor->old.pos);
    local_28->copy(*tmp);
    local_28->z = 0.0f;
    local_28->x = 0.0f;
    uVar2 = f2u(std_sqrtf(PSVECSquareMag(local_28)) * 2.0f);
    if (uVar2 > 100) {
        uVar2 = 100;
    }
    fopAcM_seStart(actor, 0x705B /* JA_SE_CM_BGN_MECHA_ROPE */, uVar2);
}
VERIFY(0x0208B378, move_se_set);

/* 0208B530 */
int pos_move(bgn2_class* i_this) {
    WWHD_FUNC(0x0208B530, int, i_this);
    fopAc_ac_c* actor = i_this;
    f32 fVar1;
    gabi::Local<cXyz> local_18;
    gabi::Local<cXyz> local_24;

    local_18->x = 0.0f;
    local_18->y = 0.0f;
    local_18->z = actor->speedF;
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    MtxPosition(local_18, local_24);
    actor->speed.x = local_24->x;
    actor->speed.z = local_24->z;
    i_this->m034C.copy(i_this->m0340);
    i_this->m0340.x = i_this->m0340.x + actor->speed.x;
    i_this->m0340.z = i_this->m0340.z + actor->speed.z;
    f32 x = i_this->m0340.x;
    f32 z = i_this->m0340.z;
    fVar1 = std_sqrtf(gabi::fmadds(x, x, z * z));
    if (fVar1 > REG0_F(3) + 1500.0f) {
        actor->speedF = 0.0f;
        i_this->m0340.z = i_this->m034C.z;
        i_this->m0340.x = i_this->m034C.x;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0208B530, pos_move);

/* 0208B648 */
static void* ki_c_sub(void* param_1, void*) {
    WWHD_FUNC(0x0208B648, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD7 /* fpcNm_KI_e */) {
        ki_all_count() = ki_all_count() + 1;
    }
    return nullptr;
}
VERIFY(0x0208B648, ki_c_sub);

/* 0208EE38 */
static BOOL daBgn2_IsDelete(bgn2_class*) {
    WWHD_FUNC(0x0208EE38, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0208EE38, daBgn2_IsDelete);

/* 0208EE40 */
static BOOL daBgn2_Delete(bgn2_class* i_this) {
    WWHD_FUNC(0x0208EE40, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10008F48) /* "Bgn" */);
    if (i_this->m30D8 != 0) {
        s8 no = l_HIO().mNo;
        hio_set() = 0;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}
VERIFY(0x0208EE40, daBgn2_Delete);

/* 025EBA58 mDoExt_3DlineMat1_c::init(u16 numLines, u16 numSegs, ResTIMG*, BOOL hasSize) [as in d_a_bmdhand] */
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_l* l, u16 lines, u16 segs, void* tex, s32 hasSize) {
    return gabi::call<BOOL>(0x025EBA58, l, lines, segs, tex, hasSize);
}
/* 0207FD38 HD: model packet setup (mDoExt_J3DModelPacketS) [as in d_a_ki] */
static inline void bgn2_packet_init(void* pkt, u32 p) { gabi::call(0x0207FD38, pkt, p); }

/* 0208EEA4
 * HD: the model packets (and a fourth one at m3304) are set up after the models */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0208EEA4, BOOL, a_this);
    J3DModelData* modelData;
    J3DAnmTevRegKey* pBrk;
    bgn2_class* i_this = (bgn2_class*)a_this;

    {
        J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008F4C) /* "Bgn" */, dRes_INDEX_BGN_BDL_BGN_HEAD1_e, BGN2_SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BCK_BGN_HEAD1_e, BGN2_SAFESTRING_VTBL);
        i_this->mpHeadMorf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                    nullptr, 0, 0x11020203);
    }
    if (i_this->mpHeadMorf == nullptr || i_this->mpHeadMorf->getModel() == nullptr) {
        return FALSE;
    }
    bgn2_packet_init(i_this->m02B8, 0);
    {
        J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BDL_BGN_KUMO1_e, BGN2_SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BCK_WAIT1_e, BGN2_SAFESTRING_VTBL);
        i_this->mpBodyMorf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                    nullptr, 0, 0x11020203);
    }
    if (i_this->mpBodyMorf == nullptr || i_this->mpBodyMorf->getModel() == nullptr) {
        return FALSE;
    }
    bgn2_packet_init(i_this->m02D0, 0);
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BDL_BGN_JYAKUTENA2_e, BGN2_SAFESTRING_VTBL);
    i_this->mpJyakutenModel[2] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpJyakutenModel[2] == nullptr) {
        return FALSE;
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BDL_BGN_JYAKUTENB2_e, BGN2_SAFESTRING_VTBL);
    i_this->mpJyakutenModel[1] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpJyakutenModel[1] == nullptr) {
        return FALSE;
    }
    {
        mDoExt_brkAnm* brk = (mDoExt_brkAnm*)operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mJyakutenBBrkAnm = brk;
    }
    if (i_this->mJyakutenBBrkAnm == nullptr) {
        return FALSE;
    }
    pBrk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BRK_BGN_JYAKUTENB2_e, BGN2_SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mJyakutenBBrkAnm, modelData, pBrk, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BDL_BGN_JYAKUTENC2_e, BGN2_SAFESTRING_VTBL);
    i_this->mpJyakutenModel[0] = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpJyakutenModel[0] == nullptr) {
        return FALSE;
    }
    {
        mDoExt_brkAnm* brk = (mDoExt_brkAnm*)operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mJyakutenCBrkAnm = brk;
    }
    if (i_this->mJyakutenCBrkAnm == nullptr) {
        return FALSE;
    }
    pBrk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BRK_BGN_JYAKUTENC2_e, BGN2_SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mJyakutenCBrkAnm, modelData, pBrk, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    bgn2_packet_init(i_this->m02E4, 0);
    void* pBti = dComIfG_getObjectRes(STR(0x10008F4C), dRes_INDEX_BGN_BTI_NOT_CUT1_e, BGN2_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->mRedRopeMat, 1, 0x3C, pBti, 1)) {
        return FALSE;
    }
    bgn2_packet_init(i_this->m3304, 0);
    return TRUE;
}
VERIFY(0x0208EEA4, useHeapInit);

/* dKy_tevstr_c (HD) assignment as the HD operator= copies it: three 0x44-byte light blocks
 * (+0x00, +0xC0, +0x144: six floats, four colour bytes, four shorts, eight floats) and the words,
 * shorts and bytes at +0x84..+0xBC; the rest is not copied */
static inline void tev_blk_copy(u32 d, u32 s) {
    for (u32 o = 0x00; o < 0x18; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(s + o));
    for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d + o, gabi::load<u8>(s + o));
    for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(s + o));
}
static inline void tevstr_assign(u32 d, u32 s) {
    tev_blk_copy(d, s);
    for (u32 o = 0x84; o < 0x90; o += 4) gabi::store<u32>(d + o, gabi::load<u32>(s + o));
    for (u32 o = 0x90; o < 0x98; o += 2) gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0x98; o < 0xA0; o += 4) gabi::store<u32>(d + o, gabi::load<u32>(s + o));
    for (u32 o = 0xA0; o < 0xA8; o += 2) gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0xA8; o < 0xB4; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(s + o));
    for (u32 o = 0xB4; o < 0xBD; o++) gabi::store<u8>(d + o, gabi::load<u8>(s + o));
    tev_blk_copy(d + 0xC0, s + 0xC0);
    tev_blk_copy(d + 0x144, s + 0x144);
}

/* 0208F218 */
static cPhs_State daBgn2_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0208F218, cPhs_State, a_this);
    bgn2_class* i_this = (bgn2_class*)a_this;
    /* fopAcM_ct(a_this, bgn2_class): placement new with the inline member constructors */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = BGN2_VTBL;
            gabi::call(0x02080404, i_this->m02B8); /* mDoExt_J3DModelPacketS */
            gabi::call(0x02080404, i_this->m02D0);
            gabi::call(0x02080404, i_this->m02E4);
            dCcD_Stts_ct(&i_this->mStts);
            gabi::call(0x025166F0, &i_this->m039C); /* dCcD_Sph::dCcD_Sph */
            gabi::call(0x028EFFD0, i_this->m04C8, 2, 0x12C, 0x025166F0);  /* __construct_array */
            gabi::call(0x028EFFD0, i_this->m0720, 30, 0x12C, 0x025166F0);
            gabi::call(0x025166F0, &i_this->m2A48);
            gabi::call(0x025EB82C, &i_this->mRedRopeMat); /* mDoExt_3DlineMat1_c */
            gabi::call(0x02080404, i_this->m3304);
            dBgS_AcchCir_ct(&i_this->mAcchCir);
            dBgS_ObjAcch_ct(&i_this->mAcch, dBgS_ObjAcch_vt{0x10008E14, 0x10008E34, 0x10008E24});
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, STR(0x10008F50) /* "Bgn" */);

    if (res == cPhs_COMPLEATE_e) {
        bgn_g() = 0;
        if (!fopAcM_entrySolidHeap(a_this, 0x0208EEA4 /* useHeapInit */, 0x96000)) {
            return cPhs_ERROR_e;
        }
        if (hio_set() == 0) {
            i_this->m30D8 = 1;
            hio_set() = 1;
            l_HIO().mNo = mDoHIO_createChild(STR(0x10008F54) /* "G (King crab)" */, &l_HIO());
        }
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        i_this->m039C.Set(gabi::at<dCcD_SrcSph>(0x101910C8) /* cc_sph_src */);
        i_this->m039C.SetStts(&i_this->mStts);
        i_this->m039C.mObjAt.mSPrm &= ~1u; /* OffAtSetBit */
        for (int i = 0; i < 2; i++) {
            i_this->m04C8[i].Set(gabi::at<dCcD_SrcSph>(0x101910C8));
            i_this->m04C8[i].SetStts(&i_this->mStts);
            i_this->m04C8[i].mObjAt.mSPrm &= ~1u;
        }
        for (int i = 0; i < 30; i++) {
            i_this->m0720[i].Set(gabi::at<dCcD_SrcSph>(0x101910C8));
            i_this->m0720[i].SetStts(&i_this->mStts);
        }
        i_this->m2A48.Set(gabi::at<dCcD_SrcSph>(0x10191108) /* core_sph_src */);
        i_this->m2A48.SetStts(&i_this->mStts);
        i_this->m0340.copy(a_this->current.pos);
        a_this->health = 3;
        a_this->max_health = 3;
        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
        i_this->mAcchCir.SetWall(50.0f, 50.0f);
        tevstr_assign(gabi::ea(bg_tevstr()), gabi::ea(&a_this->tevStr));
        daBgn2_Execute(i_this);
    }
    return res;
}
VERIFY(0x0208F218, daBgn2_Create);

/* 0208F8A8: static initialisation (header statics, zero, l_HIO, bg_tevstr) */
static void __sinit_d_a_bgn2_cpp() {
    WWHD_FUNC(0x0208F8A8, void, (u32)0);
    sinit_header_statics(0x1046202C, 0x10191148);
    zero_l()->x = 0.0f;
    zero_l()->y = 0.0f;
    zero_l()->z = 0.0f;
    daBgn2_HIO_c_ct(&l_HIO());
    /* dKy_tevstr_c bg_tevstr: HD default constructor, the default light block (.data 0x1016E414)
     * in each of its three slots */
    u32 d = gabi::ea(bg_tevstr());
    tev_blk_copy(d, 0x1016E414);
    tev_blk_copy(d + 0xC0, 0x1016E414);
    tev_blk_copy(d + 0x144, 0x1016E414);
}
VERIFY(0x0208F8A8, __sinit_d_a_bgn2_cpp);

/* 0208FACC: deleting destructor of a trivially destructible class of this TU (sead::SafeString,
 * slot 1 of the vtable at 0x10008DDC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0208FACC, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0208FACC, SafeString_dt);

/* 0208FC10: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0208FC10, void, (u32)0);
}
VERIFY(0x0208FC10, SafeString_assureTermination);

/* 0208FAE8: bgn2_class deleting destructor (compiler-generated, HD virtual destructor) */
static void bgn2_class_dt(bgn2_class* p, s32 flags) {
    WWHD_FUNC(0x0208FAE8, void, p, flags);
    if (p != nullptr) {
        /* dBgS_ObjAcch inline destructor (this TU's vtables) */
        gabi::store<u32>(gabi::ea(&p->mAcch) + 0x20, 0x10008E24);
        gabi::store<u32>(gabi::ea(&p->mAcch) + 0x14, 0x10008E34);
        gabi::call(0x024EFD9C, &p->mAcch, 0);                                             /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2);           /* cM3dGCir::~cM3dGCir */
        gabi::call(0x02082DDC, p->m3304, 2);                                              /* mDoExt_J3DModelPacketS::~ */
        gabi::call(0x025EB8B8, &p->mRedRopeMat, 2);                                       /* ~mDoExt_3DlineMat1_c (matcher: draw) */
        gabi::call(0x02515AE8, &p->m2A48, 2);                                             /* dCcD_Sph::~dCcD_Sph */
        gabi::call(0x028F0164, p->m0720, 30, 0x12C, 0x02515AE8, 0, 0);                    /* __destroy_arr */
        gabi::call(0x028F0164, p->m04C8, 2, 0x12C, 0x02515AE8, 0, 0);
        gabi::call(0x02515AE8, &p->m039C, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02082DDC, p->m02E4, 2);
        gabi::call(0x02082DDC, p->m02D0, 2);
        gabi::call(0x02082DDC, p->m02B8, 2);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0208FAE8, bgn2_class_dt);
