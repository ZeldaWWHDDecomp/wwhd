/**
 * d_a_bridge.cpp (WWHD)
 * Rope bridge (planks on two ropes, rideable, cuttable).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * daBridge_Draw is in d_a_bridge_draw.cpp, daBridge_Execute in d_a_bridge_exec.cpp.
 */
#include "d/actor/d_a_bridge.h"

/* 020DF908 */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    WWHD_FUNC(0x020DF908, void, bgw, i_ac, i_pt);
    bridge_class* i_this = (bridge_class*)i_ac;

    gabi::Local<cXyz> pos;
    cXyz_mi(&i_this->mBr[0].mPosition, pos, &i_pt->current.pos);
    s32 brIdx = gabi::ftoi(std_sqrtf(gabi::fmadds(pos->x, pos->x, pos->z * pos->z)) / 76.5f - -0.5f);
    s32 cnt = i_this->mBrCount;
    if (brIdx > cnt - 1) {
        brIdx = cnt - 1;
    } else if (brIdx < 0) {
        brIdx = 0;
    }

    br_s* pBr = &i_this->mBr[brIdx];
    f32 fVar2 = (i_this->mTypeBits & 5) != 0 ? 0.85f : 1.0f;
    cMtx_YrotS(calc_mtx(), (s16)-pBr->mRotation.y);

    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> posDiff;
    gabi::Local<cXyz> sp4C;
    cXyz_mi(&i_pt->current.pos, tmp, &pBr->mPosition);
    posDiff->copy(*tmp);
    MtxPosition(posDiff, sp4C);

    gabi::Local<cXyz> sp40;
    cXyz_mi(&i_pt->old.pos, tmp, &pBr->mPosition);
    posDiff->copy(*tmp);
    MtxPosition(posDiff, sp40);

    i_pt->speed.y = -5.0f;

    f32 fVar7;
    f32 m3F4;
    if (i_pt != nullptr && fopAcM_GetName_l(i_pt) == fpcNm_PLAYER_e_hd) {
        fVar7 = 100.0f;
        pBr->m3F4 = -31.0f;
        i_this->m033C = 5;
        /* HD: heavier (player flag 0x02000000 at +0x3B8), and a breaking hit (flag 0x20000 at +0x3C0) */
        if (gabi::load<u32>(gabi::ea(i_pt) + 0x3B8) & 0x02000000) {
            fVar7 = 150.0f;
            pBr->m3F4 = -40.0f;
        }
        if (gabi::load<u32>(gabi::ea(i_pt) + 0x3C0) & 0x00020000) {
            goto break_plank;
        }
        m3F4 = pBr->m3F4;
    } else if (i_pt != nullptr && fopAcM_GetName_l(i_pt) == fpcNm_MO2_e_hd) {
        fVar7 = 150.0f;
        pBr->m3F4 = -40.0f;
        i_pt->speed.y = -20.0f;
        m3F4 = pBr->m3F4;
    } else if (i_pt != nullptr && fopAcM_GetName_l(i_pt) == fpcNm_BK_e_hd) {
        u32 bk = gabi::ea(i_pt);
        fVar7 = 100.0f;
        i_pt->speed.y = -20.0f;
        pBr->m3F4 = -25.0f;
        gabi::store<u32>(bk + 0xC88, fopAcM_GetID(i_ac)); /* dr.m7B8 */
        gabi::store<s16>(bk + 0xC82, 8);                  /* dr.m7B2 */
        gabi::store<u16>(bk + 0xC7C, (u16)pBr->mRotation.x);   /* dr.m7AC = pBr->mRotation */
        gabi::store<u16>(bk + 0xC7E, (u16)pBr->mRotation.y);
        gabi::store<u16>(bk + 0xC80, (u16)pBr->mRotation.z);
        if (sp4C->x > 0.0f) {
            gabi::store<u32>(bk + 0xC6C, gabi::ea(&pBr->m11C[1])); /* dr.m79C */
            gabi::store<s16>(bk + 0xC7E, (s16)(gabi::load<s16>(bk + 0xC7E) - 0x4000));
            gabi::store<s16>(bk + 0xC84, -0x2000); /* dr.m7B4 */
        } else {
            gabi::store<u32>(bk + 0xC6C, gabi::ea(&pBr->m0F8[1]));
            gabi::store<s16>(bk + 0xC7E, (s16)(gabi::load<s16>(bk + 0xC7E) + 0x4000));
            gabi::store<s16>(bk + 0xC84, 0x2000);
        }
        gabi::store<u32>(bk + 0xC8C, gabi::ea(pBr)); /* m0B2C */
        m3F4 = pBr->m3F4;
    } else {
        fVar7 = 50.0f;
        if (i_pt == nullptr) {
            m3F4 = -10.0f;
        } else {
            pBr->m3F4 = -10.0f;
            if (fopAcM_GetName_l(i_pt) == fpcNm_BOMB_e_hd && daBomb_getBombRestTime(i_pt) <= 1) {
                goto break_plank;
            }
            m3F4 = pBr->m3F4;
        }
    }

    {
        m3F4 = m3F4 * fVar2;
        pBr->m3F4 = m3F4;
        fVar7 = fVar7 * fVar2;
        f32 wave = cM_ssin(i_this->m0300) * i_this->m02FC * 0.03f;
        pBr->m3F4 = gabi::fmadds(wave, fVar7, m3F4);
        pBr->m406 = 2;
        pBr->m400 = (s16)gabi::ftoi(-(sp4C->x * fVar7));

        cXyz_mi(sp4C, tmp, sp40);
        pos->copy(*tmp);
        f32 fVar3 = std_sqrtf(PSVECSquareMag(pos)) * 0.3f * fVar2;
        if (fVar3 > 20.0f) {
            fVar3 = 20.0f;
        }
        if (!(i_this->m02E0 > fVar3)) {
            i_this->m02E0 = fVar3;
        }
        f32 t = fabsf(pos->x) * fVar2;
        if (t > 50.0f) {
            t = 50.0f;
        }
        if (!(i_this->m02E4 > t)) {
            i_this->m02E4 = i_this->m02E4 + 0.5f;
        }
        return;
    }

break_plank:
    pBr->m3F4 = -300.0f;
    i_this->m02E0 = 20.0f;
}
VERIFY(0x020DF908, ride_call_back);

/* 020DFE7C */
void kikuzu_set(bridge_class* i_this, cXyz* pPos) {
    WWHD_FUNC(0x020DFE7C, void, i_this, pPos);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<csXyz> shapeAngle;
    shapeAngle->x = player->shape_angle.x;
    s16 y = player->shape_angle.y;
    shapeAngle->z = player->shape_angle.z;
    shapeAngle->y = (s16)(y - -0x8000);

    const GXColor* k0 = gabi::at<GXColor>(gabi::ea(i_this) + 0x1A8); /* actor.tevStr.mColorK0 */
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x2B /* dPa_name::ID_AK_JN_ELEMENTKIKUZU00 */, pPos, shapeAngle, nullptr, 0xFF,
                                                    nullptr, -1, k0, k0, nullptr);
    if (emitter != nullptr) {
        u32 e = gabi::ea(emitter);
        gabi::store<f32>(e + 0x34, 10.0f);  /* setRate */
        gabi::store<s32>(e + 0x5C, 1);      /* setMaxFrame */
        gabi::store<f32>(e + 0x58, 0.2f);   /* setSpread */
        gabi::store<f32>(e + 0x7C, 0.15f);  /* setVolumeSweep */
        gabi::store<f32>(e + 0x238, 0.7f);  /* setGlobalParticleScale */
        gabi::store<f32>(e + 0x23C, 0.7f);
        gabi::store<f32>(e + 0x240, 0.7f);
    }
}
VERIFY(0x020DFE7C, kikuzu_set);

/* 020E0FAC */
void himo_cut_control1(cXyz* pPos) {
    WWHD_FUNC(0x020E0FAC, void, pPos);
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> transformedPos;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = gabi::load<f32>(gabi::load<u32>(BRIDGE_WP)) * 7.0f;
    cMtx_YrotS(calc_mtx(), gabi::load<s16>(BRIDGE_WY));
    MtxPosition(pos, transformedPos);
    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 23.0f;

    pPos++;
    for (int i = 1; i < 5; i++, pPos++) {
        f32 x = (pPos[0].x - pPos[-1].x) + transformedPos->x;
        f32 y = (pPos[0].y - pPos[-1].y) - 10.0f;
        f32 z = (pPos[0].z - pPos[-1].z) + transformedPos->z;

        s32 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        cMtx_YrotS(calc_mtx(), (s16)atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(pos, sp18);
        pPos[0].y = pPos[-1].y + sp18->y;
        pPos[0].x = pPos[-1].x + sp18->x;
        pPos[0].z = pPos[-1].z + sp18->z;
    }
}
VERIFY(0x020E0FAC, himo_cut_control1);

/* 020E115C */
void* s_a_b_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x020E115C, void*, ac1, ac2);
    /* HD: ac1 is checked for NULL */
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fopAcM_GetName_l(ac1) == fpcNm_BRIDGE_e_hd && ac1 != ac2) {
        bridge_class* bridge = (bridge_class*)ac1;
        if ((bridge->mTypeBits & 0x82) == 2) {
            return ac1;
        }
    }
    return nullptr;
}
VERIFY(0x020E115C, s_a_b_sub);

/* 020E37C4 */
static BOOL daBridge_IsDelete(bridge_class* i_this) {
    WWHD_FUNC(0x020E37C4, BOOL, i_this);
    br_s* pBr = &i_this->mBr[0];
    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        mDoAud_seDeleteObject(&pBr->m3CC);
    }
    return TRUE;
}
VERIFY(0x020E37C4, daBridge_IsDelete);

/* 020E3838 */
static BOOL daBridge_Delete(bridge_class* i_this) {
    WWHD_FUNC(0x020E3838, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000B664) /* "Bridge" */);
    if (i_this->mpBgW != nullptr) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->mpBgW);
    }
    return TRUE;
}
VERIFY(0x020E3838, daBridge_Delete);

/* 020E3890 */
static BOOL CallbackCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020E3890, BOOL, a_this);
    bridge_class* i_this = (bridge_class*)a_this;
#define ARC_BRIDGE STR(0x1000B67C) /* "Bridge" */
#define ARC_ALWAYS STR(0x1000B684) /* "Always" */
    J3DModelData* modelData2 = nullptr;

    int modelNum = i_this->mTypeBits & 1;
    if (i_this->mTypeBits & 4) {
        modelNum = 1;
    }
    /* static const int bridge_bmd[] = { OBM_BRIDGE, OBM_BRIDGE2 } (0x1000B674) */
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(ARC_BRIDGE, gabi::load<s32>(0x1000B674 + modelNum * 4), BRIDGE_SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(2455, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000B6A0), 0x997, STR(0x1000B6B0));

    if (modelNum != 0) {
        modelData2 = (J3DModelData*)dComIfG_getObjectRes(ARC_BRIDGE, 6 /* OBM_CHAIN1 */, BRIDGE_SAFESTRING_VTBL);
        if (modelData2 == nullptr) /* JUT_ASSERT(2461, modelData2 != NULL) */
            JUT_ASSERT_fail(STR(0x1000B6A0), 0x99D, STR(0x1000B68C));
    }

    br_s* pBr = i_this->mBr;
    int iVar8 = 2;
    if (i_this->mTypeBits & 1) {
        iVar8 = 0;
    }

    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        pBr->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
        if (pBr->mpModel == nullptr) {
            return FALSE;
        }

        if ((i_this->mTypeBits & 4) == 0) {
            /* HD: the first plank only sets up the side ropes (GameCube: also the plank ropes when
             * (i + iVar8) & 3 == 0) */
            if (i == 0) {
                void* tex;
                if (i_this->mTypeBits & 8) {
                    tex = dComIfG_getObjectRes(ARC_ALWAYS, 0x8D /* TXM_ROPE1 */, BRIDGE_SAFESTRING_VTBL);
                } else {
                    tex = dComIfG_getObjectRes(ARC_ALWAYS, 0x7E /* ROPE */, BRIDGE_SAFESTRING_VTBL);
                }
                if (!mDoExt_3DlineMat1_init(&i_this->mLineMat, 2, 14, tex, 0)) {
                    return FALSE;
                }
            } else if (((i + iVar8) & 3) == 0) {
                pBr->m408 = 7;
                if ((i_this->mTypeBits & 1) == 1) {
                    pBr->m418 = 0x32;
                    /* HD: asserted again */
                    if (modelData2 == nullptr) /* JUT_ASSERT(2508, modelData2 != NULL) */
                        JUT_ASSERT_fail(STR(0x1000B6A0), 0x9CC, STR(0x1000B68C));
                    pBr->mpModelRope0 = mDoExt_J3DModel__create(modelData2, 0x80000, 0x11020002);
                    pBr->mpModelRope1 = mDoExt_J3DModel__create(modelData2, 0x80000, 0x11020002);
                    if (pBr->mpModelRope0 == nullptr || pBr->mpModelRope1 == nullptr) {
                        return FALSE;
                    }
                } else {
                    pBr->m418 = -1;
                    void* tex;
                    if (i_this->mTypeBits & 8) {
                        tex = dComIfG_getObjectRes(ARC_ALWAYS, 0x8D, BRIDGE_SAFESTRING_VTBL);
                    } else {
                        tex = dComIfG_getObjectRes(ARC_ALWAYS, 0x7E, BRIDGE_SAFESTRING_VTBL);
                    }
                    if (!mDoExt_3DlineMat1_init(&pBr->mLineMat1, 4, 5, tex, 1)) {
                        return FALSE;
                    }
                }
            }
        }

        if ((i_this->mTypeBits & 1) == 0) {
            pBr->mScale.y = cM_rndF(0.3f) + 1.0f;
            /* HD: (i + iVar8) & iVar8 (GameCube: & 3) */
            if (((i + iVar8) & iVar8) == 0) {
                pBr->mScale.z = 1.5f;
                pBr->mScale.x = 1.05f;
            } else {
                pBr->mScale.x = cM_rndF(0.1f) + 1.0f;
                pBr->mScale.z = 1.5f;
            }
        } else {
            pBr->mScale.z = 1.5f;
            pBr->mScale.x = 1.0f;
            pBr->mScale.y = 1.0f;
        }
        J3DModel_setBaseScale(pBr->mpModel, &pBr->mScale);

        if (cM_rndF(1.0f) < 0.5f) {
            pBr->mRotationYExtra = -0x8000;
        }
    }

    i_this->mpBgW = dBgWSv_new();
    if (i_this->mpBgW == nullptr) {
        return FALSE;
    }

    void* cBgD;
    if ((i_this->mTypeBits & 1) == 1) {
        cBgD = dComIfG_getObjectRes(ARC_BRIDGE, 0xA /* MBRDG2 */, BRIDGE_SAFESTRING_VTBL);
    } else {
        cBgD = dComIfG_getObjectRes(ARC_BRIDGE, 9 /* MBRDG */, BRIDGE_SAFESTRING_VTBL);
    }
    if (dBgWSv_Set(i_this->mpBgW, cBgD, 0)) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(i_this->mpBgW.get()) + 0xB0, 0x020DF908 /* ride_call_back */); /* SetRideCallback */
    dBgWSv_CopyBackVtx(i_this->mpBgW);
    u32 vtxTbl = gabi::load<u32>(gabi::ea(i_this->mpBgW.get()) + 0x90);
    for (int i = 0; i < gabi::load<s32>(gabi::load<u32>(gabi::ea(i_this->mpBgW.get()) + 0x94)); i++) {
        u32 v = vtxTbl + i * 0xC;
        gabi::store<u32>(v + 0, gabi::load<u32>(gabi::ea(i_this) + 0x314));
        gabi::store<u32>(v + 4, gabi::load<u32>(gabi::ea(i_this) + 0x318));
        gabi::store<u32>(v + 8, gabi::load<u32>(gabi::ea(i_this) + 0x31C));
    }
    dBgW_Move(i_this->mpBgW);
    return TRUE;
}
VERIFY(0x020E3890, CallbackCreateHeap);

/* 020E450C: dCcD_Cyl::dCcD_Cyl (this TU's copy; allocates when this == NULL) */
static dCcD_Cyl* dCcD_Cyl_ctor(dCcD_Cyl* c) {
    WWHD_FUNC(0x020E450C, dCcD_Cyl*, c);
    if (c == nullptr) {
        c = (dCcD_Cyl*)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, BRIDGE_AAB_VTBL);
    return c;
}
VERIFY(0x020E450C, dCcD_Cyl_ctor);

/* 020E3E24 br_s::br_s */
static br_s* br_s_ctor(br_s* p) {
    WWHD_FUNC(0x020E3E24, br_s*, p);
    if (p == nullptr) {
        p = (br_s*)operator_new(0x680);
        if (p == nullptr)
            return p;
    }
    /* dKy_tevstr_c mTevStr (HD, inline constructor): three 0x44-byte light blocks at +0, +0xC0
     * and +0x144 initialised from the template at 0x1016E414 (floats, then bytes and shorts) */
    static const u32 blk[3] = {0x004, 0x0C4, 0x148};
    const u32 T = 0x1016E414;
    for (int b = 0; b < 3; b++) {
        u32 d = gabi::ea(p) + blk[b];
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
        for (u32 o = 0x18; o < 0x1C; o++) gabi::store<u8>(d + o, gabi::load<u8>(T + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) gabi::store<s16>(d + o, gabi::load<s16>(T + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(d + o, gabi::load<f32>(T + o));
    }
    mDoExt_3DlineMat1_ct(&p->mLineMat1);
    __construct_array(p->mCyl, 2, 0x130, 0x020E450C /* dCcD_Cyl::dCcD_Cyl */);
    return p;
}
VERIFY(0x020E3E24, br_s_ctor);

/* 020E3FE8 bridge_class::bridge_class */
static bridge_class* bridge_class_ctor(bridge_class* i_this) {
    WWHD_FUNC(0x020E3FE8, bridge_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bridge_class*)operator_new(0x14AE4);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BRIDGE_VTBL;
    mDoExt_3DlineMat1_ct(&i_this->mLineMat);
    __construct_array(i_this->mBr, 50, 0x680, 0x020E3E24 /* br_s::br_s */);
    dCcD_Stts_ct(&i_this->mStts);
    return i_this;
}
VERIFY(0x020E3FE8, bridge_class_ctor);

/* 020E4098 */
static cPhs_State daBridge_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020E4098, cPhs_State, a_this);
    bridge_class* i_this = (bridge_class*)a_this;

    dComIfGp_get(); /* HD: an unused call */
    /* fopAcM_ct(&i_this->actor, bridge_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            bridge_class_ctor(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x1000B6E0) /* "Bridge" */);
    if (ret == cPhs_COMPLEATE_e) {
        u8 type = gabi::load<u8>(gabi::ea(a_this) + 0xB3); /* fopAcM_GetParam(a_this) & 0xFF */
        if (type == 0xFF) {
            i_this->mTypeBits = 0;
        } else {
            i_this->mTypeBits = type;
        }
        i_this->m02D9 = (fopAcM_GetParam(a_this) >> 8) & 0xFF;
        u8 pathId = (fopAcM_GetParam(a_this) >> 0x10) & 0xFF;
        i_this->mPathId = pathId;
        if (pathId == 0xFF) {
            return cPhs_ERROR_e;
        }

        dPath* path = dPath_GetRoomPath(pathId, fopAcM_GetRoomNo(a_this));
        if (path == nullptr) {
            return cPhs_ERROR_e;
        }
        u32 point = gabi::load<u32>(gabi::ea(path) + 8); /* m_points: {u8 ..., cXyz m_position (+4)}, 0x10 bytes */
        a_this->home.pos.x = gabi::load<f32>(point + 4);
        a_this->home.pos.y = gabi::load<f32>(point + 8);
        a_this->home.pos.z = gabi::load<f32>(point + 0xC);
        i_this->mEndPos.x = gabi::load<f32>(point + 0x14);
        i_this->mEndPos.y = gabi::load<f32>(point + 0x18);
        i_this->mEndPos.z = gabi::load<f32>(point + 0x1C);

        gabi::Local<cXyz> delta;
        cXyz_mi(&i_this->mEndPos, delta, &a_this->home.pos);
        a_this->home.angle.y = cM_atan2s(delta->x, delta->z);
        a_this->home.angle.x = (s16)-cM_atan2s(delta->y, std_sqrtf(gabi::fmadds(delta->x, delta->x, delta->z * delta->z)));

        f32 fVar1 = 0.0f;
        if (std_sqrtf(PSVECSquareMag(delta)) > 1300.0f) {
            fVar1 = 3.0f;
        }
        f32 len = std_sqrtf(PSVECSquareMag(delta));
        i_this->mBrCount = (s8)gabi::ftoi(len / ((fVar1 + 47.0f) * 1.5f));
        i_this->mPathIdP = i_this->mPathId + 1;

        if (i_this->mBrCount >= 50) {
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(a_this, 0x020E3890 /* CallbackCreateHeap */, 0x2FB60)) {
            return cPhs_ERROR_e;
        }

        /* CreateInit(a_this) */
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        br_s* pBr = i_this->mBr;
        for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
            for (int j = 0; j < 2; j++) {
                pBr->mCyl[j].Set(gabi::at<dCcD_SrcCyl>(BRIDGE_CYL_SRC));
                pBr->mCyl[j].SetStts(&i_this->mStts);
                if ((i_this->mTypeBits & 1) == 0) {
                    pBr->mCyl[j].SetH(200.0f);
                    /* OffTgShield */
                    u32 w = gabi::ea(&pBr->mCyl[j]) + 0x94;
                    gabi::store<u32>(w, gabi::load<u32>(w) & ~1u);
                }
            }
        }

        if (i_this->mpBgW != nullptr) {
            dBgS* bgs = dComIfG_Bgsp();
            if (dBgS_Regist(bgs, i_this->mpBgW, a_this)) {
                return cPhs_ERROR_e;
            }
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mBr[0].mpModel)); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(a_this, -120.0f, -30.0f, -60.0f, 120.0f, 30.0f, 60.0f);
        a_this->cullSizeFar = 10.0f;

        if ((i_this->mTypeBits & 2) != 0) {
            if (i_this->mBrCount >= 16) {
                i_this->m02DD = 15;
            } else if (i_this->mBrCount >= 12) {
                i_this->m02DD = 11;
            } else {
                i_this->m02DD = 7;
            }
        } else {
            i_this->m02DD = i_this->mBrCount;
        }
    }
    return ret;
}
VERIFY(0x020E4098, daBridge_Create);

/* 020E4464 */
static void __sinit_d_a_bridge_cpp() {
    WWHD_FUNC(0x020E4464, void, (u32)0);
    /* the header statics: here {-pi, pi} at 0x1046294C, objects at +0xA/+0xB, the zeroed object at +0xC
     * (wy sits at +8) */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10462958 + 4 * i, 0);
    __register_global_object(0x101927FC);
    gabi::store<f32>(0x1046294C, -3.1415927f);
    gabi::store<f32>(0x10462950, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10462956);
    __register_global_object(0x10192808);
    gabi::call(0x028EAB2C, 0x10462957);
    __register_global_object(0x10192814);
}
VERIFY(0x020E4464, __sinit_d_a_bridge_cpp);

/* 020E44F8: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020E44F8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020E44F8, trivial_dt);

/* 020E4598 br_s::~br_s */
static void br_s_dt(br_s* p, s32 flags) {
    WWHD_FUNC(0x020E4598, void, p, flags);
    if (p != nullptr) {
        __destroy_arr(p->mCyl, 2, 0x130, 0x02515A70 /* dCcD_Cyl::~dCcD_Cyl */, 0);
        mDoExt_3DlineMat1_dt(&p->mLineMat1, 2);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x020E4598, br_s_dt);

/* 020E460C bridge_class::~bridge_class */
static void bridge_class_dt(bridge_class* i_this, s32 flags) {
    WWHD_FUNC(0x020E460C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Stts_dt(&i_this->mStts, 2);
        __destroy_arr(i_this->mBr, 50, 0x680, 0x020E4598 /* br_s::~br_s */, 0);
        mDoExt_3DlineMat1_dt(&i_this->mLineMat, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020E460C, bridge_class_dt);

/* 020E469C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x020E469C, void, p);
}
VERIFY(0x020E469C, empty_virtual);
