/**
 * d_a_npc_ji1_exec.cpp (WWHD)
 * NPC - Orca: _create (with the inlined constructor), CreateInit, _execute (with
 * daNpc_Ji1_setHairAngle inlined), _delete, the HIO constructor, __sinit and the deleting
 * destructors.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02055B64 cLib_calcTimer<s16>(s16*) */
static inline s16 cLib_calcTimer_s16(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 025E535C mDoExt_McaMorf::play(cXyz*, u32 se, s8 reverb): the reverb register is passed as
 * dComIfGp_getReverb returned it */
static inline BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb) { return gabi::call<BOOL>(0x025E535C, m, pos, se, reverb); }
/* 0200F164 cLib_addCalcPos2(cXyz*, const cXyz&, f32 scale, f32 maxStep) */
static inline void cLib_addCalcPos2(cXyz* v, const cXyz* t, f32 s, f32 m) { gabi::call(0x0200F164, v, t, s, m); }
/* 0259F67C dNpc_HeadAnm_c::move */
static inline void dNpc_HeadAnm_move(void* h) { gabi::call(0x0259F67C, h); }
/* 0257DB04 dKyw_get_wind_power (f32*), 0257DAA8 dKyw_get_wind_vec (cXyz*) */
static inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
/* 028E8DE8 PSVECSquareDistance */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* dBgS::GetRoomId / GetPolyColor */
static inline s32 dBgS_GetRoomId(dBgS* bgs, void* p) { return gabi::call<s32>(0x024EF130, bgs, p); }
static inline s32 dBgS_GetPolyColor(dBgS* bgs, void* p) { return gabi::call<s32>(0x024EEEB8, bgs, p); }
/* J3DModel::getAnmMtx (HD: marks the joint matrix block dirty): block at model+0x2C, flags +4, matrices +0x10 */
static inline u32 ji1_mtxBlock(J3DModel* m) { return gabi::load<u32>(gabi::ea(m) + 0x2C); }
static inline Mtx34* ji1_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = ji1_mtxBlock(m);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
/* JPABaseEmitter::becomeInvalidEmitter (HD: also sets +0x5C to -1) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
/* 025D5AA4 fopAcM_createChild(name, parentPcId, param, pos, roomNo, angle, scale, createFunc) (HD: no subtype) */
static inline u32 fopAcM_createChild(const char* name, u32 parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, u32 cb) {
    return gabi::call<u32>(0x025D5AA4, name, parent, param, pos, roomNo, angle, scale, cb);
}
/* 0259F7D4 dNpc_EventCut_c::setActorInfo(const char*, fopAc_ac_c*) */
static inline void dNpc_EventCut_setActorInfo(dNpc_EventCut_c* e, const char* name, fopAc_ac_c* a) { gabi::call(0x0259F7D4, e, name, a); }
/* 025164C0 dCcD_Cps::Set, 02018808 cM3dGCps::SetStartEnd(const cXyz&, const cXyz&) */
static inline void dCcD_Cps_Set(dCcD_Cps* c, u32 src) { gabi::call(0x025164C0, c, src); }
static inline void cM3dGCps_SetStartEnd(void* cps, cXyz* s, cXyz* e) { gabi::call(0x02018808, cps, s, e); }
/* dComIfGp attention (play+0x5804): flags word +0x20, bit 31 = alert off */
static inline void dComIfGp_att_offAleart() {
    u32 a = dComIfGp_ea() + 0x5824;
    gabi::store<u32>(a, gabi::load<u32>(a) | 0x80000000u);
}
static inline void dComIfGp_att_revivalAleart() {
    u32 a = dComIfGp_ea() + 0x5824;
    gabi::store<u32>(a, gabi::load<u32>(a) & 0x7FFFFFFFu);
}
/* mini games: play+0x5CEA type, +0x5CEE, flags +0x5CE8 */
static inline u8 dComIfGp_getMiniGameType() { return gabi::load<u8>(dComIfGp_ea() + 0x5CEA); }
static inline void dComIfGp_endMiniGame(u16 bit) {
    u32 p = dComIfGp_ea();
    u16 f = gabi::load<u16>(p + 0x5CE8);
    gabi::store<u8>(p + 0x5CEE, 0);
    gabi::store<u8>(p + 0x5CEA, 0);
    gabi::store<u16>(p + 0x5CE8, (u16)(f ^ bit));
}
/* virtual remove() (vtable +0x44) of a particle callback (vtable pointer at +0) */
static inline void ji1_vremove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }

/* ---- statics of this TU ---- */
static inline cXyz* l_head_front() { return gabi::at<cXyz>(0x10467294); } /* (0, 1, 0) */
static inline cXyz* l_head_top() { return gabi::at<cXyz>(0x104672A0); }   /* (1, 0, 0) */

/* 0224C550 */
BOOL daNpc_Ji1_c::_delete() {
    WWHD_FUNC(0x0224C550, BOOL, this);
    if (dComIfGp_getMiniGameType() == 2) {
        dComIfGp_endMiniGame(2);
    }
    if (dComIfGp_getMiniGameType() == 6) {
        dComIfGp_endMiniGame(0x20);
    }
    dComIfG_resDelete(&mPhs, STR(0x1001B198) /* "Ji" */);
    if (heap.get() && mpOrcaMorf.get()) {
        mpOrcaMorf->stopZelAnime();
    }
    mDoAud_seDeleteObject(&field_0xB78);
    mDoAud_seDeleteObject(&field_0xB84);
    dComIfGp_att_revivalAleart();
    ji1_vremove(&mSmokeCb);
    ji1_vremove(&mSmokeCbAT);
    return true;
}
VERIFY(0x0224C550, &daNpc_Ji1_c::_delete);

/* 0224D280: _create with the constructor inlined (fopAcM_ct) */
cPhs_State daNpc_Ji1_c::_create() {
    WWHD_FUNC(0x0224D280, cPhs_State, this);
    u32 cond = actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            u32 b = gabi::ea(this);
            fopAc_ac_c_ct(this);
            __vtbl = 0x1001AE78;
            gabi::call(0x025A5B18, &mSmokeCb, 1);   /* dPa_smokeEcallBack(u8) */
            gabi::call(0x025A5B18, &mSmokeCbAT, 1);
            gabi::call(0x0259F740, &mEventCut);    /* dNpc_EventCut_c */
            gabi::call(0x0259DAA0, &m_jnt);        /* dNpc_JntCtrl_c */
            gabi::call(0x025E7820, &mBlinkAnim);   /* mDoExt_btpAnm */
            gabi::call(0x025E80D0, &mCryBrk);      /* mDoExt_brkAnm */
            gabi::call(0x025E7C6C, &mCryBtk);      /* mDoExt_btkAnm */
            static const dBgS_ObjAcch_vt acch_vt = {0x1001AE48, 0x1001AE68, 0x1001AE58};
            dBgS_ObjAcch_ct(&mAcch, acch_vt);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&field_0x638);
            dCcD_Stts_ct(&field_0x674);
            dCcD_Cyl_ct(&field_0x6B0, 0x1001AE38);
            dCcD_Cyl_ct(&field_0x7E0, 0x1001AE38);
            dCcD_Cyl_ct(&field_0x910, 0x1001AE38);
            /* dCcD_Cps: GObjInf, shape attr, cM3dGAab (per TU), cM3dGCps */
            u32 c = gabi::ea(&field_0xA40);
            gabi::call(0x02515FB8, &field_0xA40);
            gabi::store<u32>(c + 0x114, 0x100015A8);
            gabi::store<u32>(c + 0x110, 0x1001AE38);
            gabi::call(0x02018150, c + 0x118);
            gabi::store<u32>(c + 0x130, 0x1004AF60);
            field_0xA40.__vtbl_hitinf = 0x1004AF18;
            gabi::store<u32>(c + 0x114, 0x1004AF70);
            /* dNpc_HeadAnm_c */
            gabi::store<s16>(b + 0xE24, 0);
            gabi::store<s16>(b + 0xE26, 0);
            gabi::store<s16>(b + 0xE28, 0);
            gabi::store<f32>(b + 0xE34, 0.0f);
            gabi::store<f32>(b + 0xE38, 0.0f);
            gabi::store<s16>(b + 0xE3C, 0);
            gabi::store<s16>(b + 0xE3E, 0);
            gabi::store<s16>(b + 0xE40, 0);
            cond = actor_condition;
        }
        actor_condition = cond | fopAcCnd_INIT_e;
    }

    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001B1C4) /* "Ji" */);
    if (state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x0224A2B4 /* CheckCreateHeap */, 0x16840)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return state;
}
VERIFY(0x0224D280, &daNpc_Ji1_c::_create);

/* the common tail of CreateInit's branches */
static inline BOOL CreateInit_tail(daNpc_Ji1_c* i_this) {
    dComIfGp_att_offAleart();
    dNpc_EventCut_setActorInfo(&i_this->mEventCut, STR(0x1001B1B8) /* "Ji1" */, i_this);
    i_this->mEventCut.mpJntCtrl = &i_this->m_jnt;
    i_this->field_0xC84 = 0x12;
    i_this->field_0xD7B = 0;
    i_this->field_0xD7C = 0;
    i_this->field_0xD7E = 0;
    attn_flags(i_this) |= 1; /* fopAc_Attn_LOCKON_MISC_e */
    attn_distance(i_this, 1) = 0xA9;
    attn_distance(i_this, 3) = 0xA9;
    attn_distance(i_this, 2) = 0xB5;
    i_this->mCryBrkFrame = 0.0f;
    i_this->mCryBtkFrame = 0.0f;
    i_this->field_0x430 = nullptr;
    i_this->set_mtx();
    gabi::store<u8>(0x10467244, 0); /* HD: a file static cleared at creation */
    return true;
}

/* 0224C650 */
BOOL daNpc_Ji1_c::CreateInit() {
    WWHD_FUNC(0x0224C650, BOOL, this);
    gabi::Local<cXyz> temp;
    temp->x = 190.0f;
    temp->y = 230.0f;
    temp->z = -1100.0f;
    gabi::Local<csXyz> temp2;
    csXyz_ct(temp2, 0, 0, 0);
    fopAcM_createChild(STR(0x1001B1BC) /* "Kmon" */, gabi::load<u32>(gabi::ea(this) + 4), 0, temp, fopAcM_GetRoomNo(this), temp2,
                       nullptr, 0);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpOrcaMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    gravity = -30.0f;
    attn_flags(this) = 0xB; /* LOCKON_MISC | LOCKON_TALK | ACTION_SPEAK */

    field_0x638.Init(0xFF, 0xFF, this);
    field_0x674.Init(0xFF, 0xFF, this);
    field_0x6B0.Set(gabi::at<dCcD_SrcCyl>(0x101BEA8C) /* l_cyl_src */);
    field_0x6B0.SetStts(&field_0x638);
    field_0x7E0.Set(gabi::at<dCcD_SrcCyl>(0x101BEAD0) /* l_cyl2_src */);
    field_0x7E0.SetStts(&field_0x674);
    field_0x7E0.mGObjTg.mHitCallback = 0x02248CF0; /* daJi1_TgHitCallback */
    field_0x7E0.mGObjCo.mHitCallback = 0x02248CA4; /* daJi1_CoHitCallback */
    field_0x6B0.mGObjTg.mSPrm &= ~1u;              /* OffTgShield */
    field_0x7E0.mGObjTg.mSPrm |= 1u;               /* OnTgShield */
    field_0x910.Set(gabi::at<dCcD_SrcCyl>(0x101BEB14) /* l_cylAt_src */);
    field_0x910.SetStts(&field_0x638);
    dCcD_Cps_Set(&field_0xA40, 0x101BEB58 /* l_cpsAt_src */);
    field_0xA40.SetStts(&field_0x638);
    cM3dGCps_SetStartEnd(field_0xA40.mCps, &field_0xB84, &field_0xB78);
    gabi::store<f32>(gabi::ea(&field_0xA40) + 0x134, 50.0f); /* SetR */
    field_0xA40.mGObjAt.mHitCallback = 0x02248EA0;          /* daJi1_AtHitCallback */

    mBtHeight = 240.0f; /* initBt */
    mBtBodyR = 100.0f;
    mAnimation = -1;
    field_0xD68 = 0;
    field_0xD6C = 0;
    field_0xD70 = 0;
    field_0xD74 = 0;
    field_0xD78 = 0;
    field_0xD79 = 0;
    field_0xC94 = 0;
    field_0xC90 = 0;
    field_0xC98 = l_HIO().field_0x54[field_0xD70];
    field_0xC28 = 0;
    field_0xC2C = 0;
    field_0xC4C = 0.0f;
    field_0xC50 = 0.0f;
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);

    if (dComIfGs_isEventBit(0x0520)) {
        if (!dComIfGs_isEventBit(0x0002)) {
            field_0xD84 = 0;
            field_0xD50.set(-13.0f, -4.0f, 10.0f);
            field_0xD5C.x = 0x4000;
            field_0xD5C.y = 0;
            field_0xD5C.z = 0x7FFF;
            current.pos.x = 0.0f;
            current.pos.y = 0.0f;
            current.pos.z = -850.0f;
            current.angle.y = -0x8000;
            ji1_setAction(this, ACT_kaitenExpAction, nullptr);
        } else {
            field_0xD84 = 1;
            field_0xD50.set(-13.0f, -4.0f, 10.0f);
            field_0xD5C.x = 0x4000;
            field_0xD5C.y = 0;
            field_0xD5C.z = 0x7FFF;
            ji1_setAction(this, ACT_normalAction, nullptr);
        }
        field_0xD28.copy(home.pos);
    } else {
        if (dComIfGs_isEventBit(0x0501) || dComIfGs_isEventBit(0x0001)) {
            field_0xD84 = 1;
            field_0xD50.set(-13.0f, -4.0f, 10.0f);
            field_0xD5C.x = 0x4000;
            field_0xD5C.y = 0;
            field_0xD5C.z = 0x7FFF;
            field_0xD28.copy(home.pos);
            ji1_setAction(this, ACT_normalAction, nullptr);
        } else {
            field_0xD84 = 0;
            if (!dComIfGs_isEventBit(0x0640) || l_HIO().field_0x30) {
                field_0xD68 = 0;
                ji1_setAction(this, ACT_kaitenExpAction, nullptr);
            } else {
                ji1_setAction(this, ACT_kaitenwaitAction, nullptr);
            }
            current.pos.x = 0.0f;
            current.pos.y = 0.0f;
            current.pos.z = -850.0f;
            current.angle.y = -0x8000;
            field_0xD28.copy(current.pos);
            home.angle.y = -0x8000;
        }
    }
    return CreateInit_tail(this);
}
VERIFY(0x0224C650, &daNpc_Ji1_c::CreateInit);

/* (s16)(f32 * 0.2f) of an s16 difference */
static inline s16 tenth2(s32 d) { return (s16)gabi::ftoi((f32)(s16)d * 0.2f); }
/* cLib_minMaxLimit<s16> on int-extended limits */
static inline s16 minMax(s32 v, s32 lo, s32 hi) {
    if (v < lo) return (s16)lo;
    if (hi < v) return (s16)hi;
    return (s16)v;
}

/* daNpc_Ji1_setHairAngle (inlined in _execute) */
static inline void daNpc_Ji1_setHairAngle(daNpc_Ji1_c* i_this) {
    u32 wp1 = dKyw_get_wind_power();
    u32 wp2 = dKyw_get_wind_power();
    f32 w2 = gabi::load<f32>(wp2);
    f32 w1 = gabi::load<f32>(wp1);
    f32 wind = w1 * w2 * 25.0f;
    cXyz* windVec = dKyw_get_wind_vec();

    J3DModel* pModel = i_this->mpOrcaMorf->getModel();
    {
        u32 blk = ji1_mtxBlock(pModel);
        gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    }
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;
    {
        s8 head = i_this->m_jnt.mHeadJntNum;
        Mtx34* m = ji1_getAnmMtx(i_this->mpOrcaMorf->getModel(), head);
        PSMTXMultVecSR(m, l_head_front(), temp2);
    }
    {
        s8 head = i_this->m_jnt.mHeadJntNum;
        Mtx34* m = ji1_getAnmMtx(i_this->mpOrcaMorf->getModel(), head);
        PSMTXMultVecSR(m, l_head_top(), temp);
    }
    s16 temp3 = i_this->field_0xBA6;
    s16 temp4 = i_this->field_0xBA8;
    gabi::Local<cXyz> xz;
    xz->x = (f32)temp2->x;
    xz->y = 0.0f;
    xz->z = (f32)temp2->z;
    if (temp->y < 0.0f) {
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        i_this->field_0xBA6 = cM_atan2s(temp2->y, -d);
    } else {
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        i_this->field_0xBA6 = cM_atan2s(temp2->y, d);
    }
    s16 ba8 = cM_atan2s(temp2->x, temp2->z);
    i_this->field_0xBA8 = ba8;
    s16 temp3_2 = (s16)(i_this->field_0xBA6 - temp3) >> 1;
    s16 temp4_2 = (s16)(ba8 - temp4) >> 1;

    Mtx34* pMtx = ji1_getAnmMtx(pModel, i_this->hair1JointNo);
    gabi::Local<cXyz> sp48;
    sp48->x = (f32)pMtx->m[0][3];
    sp48->y = (f32)pMtx->m[1][3];
    sp48->z = (f32)pMtx->m[2][3];
    gabi::Local<cXyz> sp3C;
    sp3C->x = i_this->field_0xBC4.x - sp48->x;
    sp3C->y = i_this->field_0xBC4.y - sp48->y - 8.5f;
    sp3C->z = i_this->field_0xBC4.z - sp48->z;
    gabi::Local<cXyz> wv;
    cXyz_ml(windVec, wv, wind);
    PSVECAdd(sp3C, wv, sp3C);

    f32 x = sp3C->x;
    if (std::fabs(x) < 0.01f) {
        x = 0.0f;
        sp3C->x = x;
    }
    f32 z = sp3C->z;
    if (std::fabs(z) < 0.01f) {
        z = 0.0f;
        sp3C->z = z;
    }

    s16 r4 = i_this->current.angle.y + i_this->m_jnt.mAngles[0][1];
    f32 temp10 = gabi::fmadds(z, cM_scos(r4), x * cM_ssin(r4));
    f32 y = sp3C->y;
    s16 r25 = i_this->field_0xBAC;
    s16 r26 = i_this->field_0xBAA;
    s16 temp9 = cM_atan2s(-temp10, -y);
    if (temp9 < 0 && temp9 > -0x6000) {
        temp9 = 0;
    } else if (temp9 > 0x7C00 || temp9 <= -0x6000) {
        temp9 = 0x7C00;
    }
    temp9 += i_this->field_0xBA6;
    cLib_addCalcAngleS2(&i_this->field_0xBAA, temp9, 5, 0x400);
    {
        s32 add = i_this->field_0xBB6 + temp3_2;
        i_this->field_0xBAA = (s16)(i_this->field_0xBAA + add + add);
    }
    i_this->field_0xBAA = minMax(i_this->field_0xBAA, l_HIO().field_0xF4, l_HIO().field_0xF6);

    {
        s16 a = i_this->current.angle.y + i_this->m_jnt.mAngles[0][1];
        f32 yy = sp3C->y;
        f32 f1 = gabi::fmadds(temp10, temp10, yy * yy);
        f32 f5 = cM_ssin(a);
        f32 f6 = cM_scos(a);
        f32 sq = std_sqrtf(f1);
        f32 v = gabi::fmsubs((f32)sp3C->z, f5, (f32)sp3C->x * f6);
        cLib_addCalcAngleS2(&i_this->field_0xBAC, cM_atan2s(v, sq), 5, 0x400);
    }
    i_this->field_0xBAC = (s16)(i_this->field_0xBAC + (s16)(i_this->field_0xBB8 - temp4_2));
    s16 bac = minMax(i_this->field_0xBAC, l_HIO().field_0xE8, l_HIO().field_0xEA);
    s16 baa = i_this->field_0xBAA;
    i_this->field_0xBAC = bac;

    s32 d26 = baa - r26;
    s32 d25 = bac - r25;
    i_this->field_0xBB6 = tenth2(d26);
    i_this->field_0xBB8 = tenth2(d25);
    i_this->field_0xBAE = (s16)(i_this->field_0xBAE - d26);
    i_this->field_0xBB0 = (s16)(i_this->field_0xBB0 - d25);
    r26 = i_this->field_0xBAE;
    r25 = i_this->field_0xBB0;
    cLib_addCalcAngleS2(&i_this->field_0xBAE, 0, 5, 0x400);
    cLib_addCalcAngleS2(&i_this->field_0xBB0, 0, 5, 0x400);

    i_this->field_0xBAE = (s16)(i_this->field_0xBAE + (s16)(i_this->field_0xBBA + temp3_2));
    i_this->field_0xBAE = minMax(i_this->field_0xBAE, (s16)(l_HIO().field_0xF8 - i_this->field_0xBAA),
                                 (s16)(l_HIO().field_0xFA - i_this->field_0xBAA));
    i_this->field_0xBB0 = (s16)(i_this->field_0xBB0 + (s16)(i_this->field_0xBBC - temp4_2));
    i_this->field_0xBB0 = minMax(i_this->field_0xBB0, (s16)(l_HIO().field_0xEC - i_this->field_0xBAC),
                                 (s16)(l_HIO().field_0xEE - i_this->field_0xBAC));

    d26 = i_this->field_0xBAE - r26;
    d25 = i_this->field_0xBB0 - r25;
    i_this->field_0xBBA = tenth2(d26);
    i_this->field_0xBBC = tenth2(d25);
    i_this->field_0xBB2 = (s16)(i_this->field_0xBB2 - d26);
    i_this->field_0xBB4 = (s16)(i_this->field_0xBB4 - d25);
    r26 = i_this->field_0xBB2;
    r25 = i_this->field_0xBB4;
    cLib_addCalcAngleS2(&i_this->field_0xBB2, 0, 5, 0x400);
    cLib_addCalcAngleS2(&i_this->field_0xBB4, 0, 5, 0x400);

    i_this->field_0xBB2 = (s16)(i_this->field_0xBB2 + (s16)(i_this->field_0xBBE + temp3_2));
    i_this->field_0xBB2 = minMax(i_this->field_0xBB2, (s16)(l_HIO().field_0xFC - i_this->field_0xBAA - i_this->field_0xBAE),
                                 (s16)(l_HIO().field_0xFE - i_this->field_0xBAA - i_this->field_0xBAE));
    i_this->field_0xBB4 = (s16)(i_this->field_0xBB4 + (s16)(i_this->field_0xBC0 - temp4_2));
    i_this->field_0xBB4 = minMax(i_this->field_0xBB4, (s16)(l_HIO().field_0xF0 - i_this->field_0xBB0 - i_this->field_0xBAC),
                                 (s16)(l_HIO().field_0xF2 - i_this->field_0xBB0 - i_this->field_0xBAC));
    i_this->field_0xBBE = tenth2(i_this->field_0xBB2 - r26);
    i_this->field_0xBC0 = tenth2(i_this->field_0xBB4 - r25);

    wind = gabi::fmadds(std_sqrtf(PSVECSquareDistance(&i_this->field_0xBC4, sp48)), 0.65f, wind) / 30.0f;
    if (wind > 1.0f) {
        wind = 1.0f;
    }
    s16 temp14 = (s16)gabi::ftoi(gabi::fmadds(wind, 4600.0f, 1500.0f));
    s16 bd0 = (s16)(i_this->field_0xBD0 + temp14);
    i_this->field_0xBD0 = bd0;
    i_this->field_0xBD2 = (s16)gabi::ftoi(wind * 2280.0f * cM_scos(bd0));
    f32 f5 = (f32)temp14;
    f32 f8 = (f32)bd0;
    i_this->field_0xBD4 = (s16)gabi::ftoi(wind * 3908.0f * cM_scos(gabi::ftoi(gabi::fnmsubs(3.0f, f5, f8))));
    i_this->field_0xBD6 = (s16)gabi::ftoi(wind * 7568.0f * cM_scos(gabi::ftoi(gabi::fnmsubs(6.0f, f5, f8))));
    i_this->field_0xBC4.copy(*sp48.get());
}

/* 0224B268 HD: decrements the AtHit window (mHD_E64); daNpc_Ji1_setHairAngle inlined */
BOOL daNpc_Ji1_c::_execute() {
    WWHD_FUNC(0x0224B268, BOOL, this);
    dComIfGp_get(); /* daPy_getPlayerActorClass(), unused */
    u8 e64 = mHD_E64;
    u8 d84 = field_0xD84;
    if (e64 != 0) {
        mHD_E64 = (u8)(e64 - 1);
    }
    if (d84 == 2) {
        harpoonMove();
    }

    if (cLib_calcTimer_s16(&mBlinkTimer) == 0) {
        u8 f = (u8)(mBlinkFrame + 1);
        mBlinkFrame = f;
        /* headTexPattern->getFrameMax(): virtual (vtable at +4, slot 0x14) */
        u32 anm = gabi::ea(headTexPattern.get());
        s32 max = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm);
        if ((s32)f >= max) {
            u32 anm2 = gabi::ea(headTexPattern.get());
            s32 max2 = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm2 + 4) + 0x14), anm2);
            mBlinkFrame = (u8)(mBlinkFrame - max2);
            mBlinkTimer = (s16)((s16)gabi::ftoi(cM_rndF(100.0f)) + 30);
        }
    }

    u32 sound;
    if (mAcch.ChkGroundHit()) {
        sound = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(gabi::ea(&mAcch) + 0xD4 + 0x14));
    } else {
        sound = 0;
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    McaMorf_play(mpOrcaMorf, &current.pos, sound, reverb);
    mpOrcaMorf->calc();

    if (isItemWaitAnim()) {
        /* static cXyz aim_offset(-13.0f, -4.0f, 10.0f); static csXyz aim_angle(0x4000, 0, 0x7FFF) */
        cXyz* aim_offset = gabi::at<cXyz>(0x1046727C);
        if (gabi::load<u32>(0x104673B0) == 0) {
            gabi::store<u32>(0x104673B0, 1);
            aim_offset->x = -13.0f;
            aim_offset->y = -4.0f;
            aim_offset->z = 10.0f;
        }
        csXyz* aim_angle = gabi::at<csXyz>(0x10467234);
        if (gabi::load<u32>(0x104673B4) == 0) {
            gabi::store<u32>(0x104673B4, 1);
            csXyz_ct(aim_angle, 0x4000, 0, 0x7FFF);
        }
        field_0xD5C.x = (s16)aim_angle->x;
        field_0xD5C.y = (s16)aim_angle->y;
        field_0xD5C.z = (s16)aim_angle->z;
        cLib_addCalcPos2(&field_0xD50, aim_offset, 0.25f, 5.0f);
    } else {
        /* static cXyz aim_offset(-13.06f, -4.0f, 44.35f); static csXyz aim_angle(0x3E94, 0, 0x7E93) */
        cXyz* aim_offset = gabi::at<cXyz>(0x10467288);
        if (gabi::load<u32>(0x104673B8) == 0) {
            gabi::store<u32>(0x104673B8, 1);
            aim_offset->x = -13.06f;
            aim_offset->y = -4.0f;
            aim_offset->z = 44.35f;
        }
        csXyz* aim_angle = gabi::at<csXyz>(0x1046723C);
        if (gabi::load<u32>(0x104673BC) == 0) {
            gabi::store<u32>(0x104673BC, 1);
            csXyz_ct(aim_angle, 0x3E94, 0, 0x7E93);
        }
        field_0xD5C.x = (s16)aim_angle->x;
        field_0xD5C.y = (s16)aim_angle->y;
        field_0xD5C.z = (s16)aim_angle->z;
        cLib_addCalcPos2(&field_0xD50, aim_offset, 0.25f, 5.0f);
    }

    if (field_0xD84 == 1 && (mAnimation == 0x13 || mAnimation == 0x14)) {
        mpSpearMorf->setFrame(mpOrcaMorf->getFrame());
    } else {
        mpSpearMorf->mFrameCtrl.mFrame = 0.0f;
        mpSpearMorf->setPlaySpeed(0.0f);
    }
    mpSpearMorf->calc();
    dNpc_HeadAnm_move(mHeadAnm);

    if (mAnimation == 0x17) {
        f32 v = mCryBtkFrame + 1.0f;
        if (!(v < 60.0f)) { /* NaN: 30 */
            mCryBtkFrame = 30.0f;
        } else {
            mCryBtkFrame = v;
        }
    } else {
        if (mAnimation != 0x18 && field_0x430.get() != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(field_0x430);
            field_0x430 = nullptr;
        }
        if (mCryBtkFrame > 0.0f) {
            f32 v = mCryBrkFrame + 1.0f;
            mCryBrkFrame = v;
            if (!(v < 30.0f)) {
                mCryBrkFrame = 0.0f;
                mCryBtkFrame = 0.0f;
            }
        }
    }

    ptmf_invoke(mAction, this, (u32)0);
    lookBack();
    daNpc_Ji1_setHairAngle(this);

    f32 px = current.pos.x;
    f32 py = current.pos.y;
    f32 pz = current.pos.z;
    gabi::store<f32>(gabi::ea(this) + 0x390, px); /* attention_info.position */
    gabi::store<f32>(gabi::ea(this) + 0x394, py + 190.0f);
    gabi::store<f32>(gabi::ea(this) + 0x398, pz);
    shape_angle.x = (s16)current.angle.x;
    shape_angle.y = (s16)current.angle.y;
    shape_angle.z = (s16)current.angle.z;
    eyePos.x = px;
    eyePos.y = py + 150.0f;
    eyePos.z = pz;
    fopAcM_posMoveF(this, &field_0x638.m_cc_move);
    gabi::Local<cXyz> temp;
    temp->x = (f32)current.pos.x;
    temp->y = (f32)current.pos.y;
    temp->z = (f32)current.pos.z;
    mAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, temp);
    field_0xC40.copy(*diff.get());
    tevStr.mRoomNo = (s8)dBgS_GetRoomId(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(&mAcch) + 0xE8));
    s32 col = dBgS_GetPolyColor(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(&mAcch) + 0xE8));
    gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)col); /* tevStr.mEnvrIdxOverride */
    set_mtx();
    field_0x6B0.SetC(&current.pos);
    dComIfG_Ccsp_Set(&field_0x6B0);
    field_0x7E0.SetC(&current.pos);
    dComIfG_Ccsp_Set(&field_0x7E0);
    field_0xC28 = 0;
    return true;
}
VERIFY(0x0224B268, &daNpc_Ji1_c::_execute);

/* 0225CC44 */
static daNpc_Ji1_HIO_c* daNpc_Ji1_HIO_ct(daNpc_Ji1_HIO_c* p) {
    WWHD_FUNC(0x0225CC44, daNpc_Ji1_HIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Ji1_HIO_c*)operator_new(0x100);
        if (p == nullptr) return p;
    }
    p->__vtbl = 0x1001AE88;
    p->mNo = -1;
    p->field_0x08 = 100.0f;
    p->field_0x0C = 7000;
    p->field_0x0E = 4000;
    p->field_0x10 = 9000;
    p->field_0x12 = 7000;
    p->field_0x14 = 5000;
    p->field_0x16 = 1;
    p->field_0x18 = 2000;
    p->field_0x1C = 0.0f;
    p->field_0x20 = 20.0f;
    p->field_0x24 = 20.0f;
    p->field_0x28 = 0;
    p->field_0x2C = 200.0f;
    p->field_0x30 = 0;
    p->field_0x34 = 150.0f;
    p->field_0x38 = 20.0f;
    p->field_0x3C = 90.0f;
    p->field_0x40 = 10.0f;
    p->field_0x44 = 2.0f;
    p->field_0x48 = 2.0f;
    p->field_0x4C = 1.25f;
    p->field_0x50 = 1.5f;
    p->field_0x54[0] = 9;
    p->field_0x54[1] = 9;
    p->field_0x54[2] = 9;
    p->field_0x54[3] = 2;
    p->field_0x54[4] = 2;
    p->field_0x54[5] = 2;
    p->field_0x60[0] = 100;
    p->field_0x60[1] = 300;
    p->field_0x60[2] = 500;
    p->field_0x60[3] = 1000;
    p->field_0x68 = 0;
    p->field_0x6C = 0.7f;
    p->field_0x70 = 1.5f;
    p->field_0x78 = 1.4f;
    p->field_0x74 = 1.4f;
    p->field_0x80 = 1.4f;
    p->field_0x7C = 1.2f;
    p->field_0x84 = 0.1f;
    p->field_0x88 = 0xB;
    p->field_0x8A = 0x32;
    p->field_0x8C = 0x28;
    p->field_0x8E = 0x1E;
    p->field_0x90 = 0x14;
    p->field_0x92 = 0xF;
    p->field_0x94 = 10;
    p->field_0x96 = 0xB4;
    p->field_0x98 = 0x8C;
    p->field_0x9A = 100;
    p->field_0x9C = 0x50;
    p->field_0x9E = 0x3C;
    p->field_0xA0 = 0x32;
    p->field_0xA4 = 6.0f;
    p->field_0xA8 = 16.0f;
    p->field_0xAC = 70.0f;
    p->field_0xB0 = 10.0f;
    p->field_0xB4 = 1.0f;
    p->field_0xB8 = 0;
    p->field_0xBC = 0.25f;
    p->field_0xC0 = -5.0f;
    p->field_0xC4[0].set(90.0f, 5.0f, 30.0f);
    p->field_0xC4[1].set(90.0f, 5.0f, -30.0f);
    p->field_0xC4[2].set(-20.0f, -20.0f, 0.0f);
    p->field_0xE8 = (s16)0xC800;
    p->field_0xEA = 0x3800;
    p->field_0xEC = (s16)0xC600;
    p->field_0xEE = 0x3A00;
    p->field_0xF0 = (s16)0xC600;
    p->field_0xF2 = 0x3A00;
    p->field_0xF4 = (s16)0xFB00;
    p->field_0xF6 = 0x3800;
    p->field_0xF8 = (s16)0xF600;
    p->field_0xFA = 0x3A00;
    p->field_0xFC = (s16)0xF100;
    p->field_0xFE = 0x3A00;
    return p;
}
VERIFY(0x0225CC44, daNpc_Ji1_HIO_ct);

/* 0225CF20 __sinit_d_a_npc_ji1_cpp: this TU's header statics, l_HIO, l_head_front, l_head_top */
static void __sinit_d_a_npc_ji1_cpp() {
    /* WWHD_FUNC(0x0225CF20, void) without arguments (the macro needs at least one) */
    if (gabi::Activation::nested()) return gabi::call(0x0225CF20);
    gabi::Activation wwhd_activation_;
    for (int i = 3; i >= 0; i--) gabi::store<u32>(0x10467248 + 4 * i, 0);
    __register_global_object(0x101BEBF4);
    gabi::store<f32>(0x1046722C, -3.1415927f);
    gabi::store<f32>(0x10467230, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10467245);
    __register_global_object(0x101BEC00);
    gabi::call(0x028EAB2C, 0x10467246);
    __register_global_object(0x101BEC0C);
    daNpc_Ji1_HIO_ct(gabi::at<daNpc_Ji1_HIO_c>(0x104672AC));
    l_head_front()->set(0.0f, 1.0f, 0.0f);
    l_head_top()->set(1.0f, 0.0f, 0.0f);
}
VERIFY(0x0225CF20, __sinit_d_a_npc_ji1_cpp);

/* 0225CFF0 daNpc_Ji1_HIO_c deleting destructor (new) */
static void daNpc_Ji1_HIO_dt(daNpc_Ji1_HIO_c* p, s32 flags) {
    WWHD_FUNC(0x0225CFF0, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0225CFF0, daNpc_Ji1_HIO_dt);

/* 0225D004 daNpc_Ji1_c deleting destructor (new): the members' destructors, then fopAc_ac_c's */
static void daNpc_Ji1_dt(daNpc_Ji1_c* p, s32 flags) {
    WWHD_FUNC(0x0225D004, void, p, flags);
    if (p != nullptr) {
        u32 b = gabi::ea(p);
        gabi::call(0x02515980, &p->field_0xA40, 2); /* dCcD_GObjInf::~dCcD_GObjInf (dCcD_Cps) */
        dCcD_Cyl_dt(&p->field_0x910, 2);
        dCcD_Cyl_dt(&p->field_0x7E0, 2);
        dCcD_Cyl_dt(&p->field_0x6B0, 2);
        dCcD_Stts_dt(&p->field_0x674, 2);
        dCcD_Stts_dt(&p->field_0x638, 2);
        gabi::call(0x02018034, b + 0x834, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(b + 0x67C, 0x1001AE58); /* dBgS_ObjAcch -> dBgS_Acch vtables */
        gabi::store<u32>(b + 0x670, 0x1001AE68);
        gabi::call(0x024EFD9C, &p->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, p, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
VERIFY(0x0225D004, daNpc_Ji1_dt);
