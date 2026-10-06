/* d_cc_s: collision manager (dCcS), WWHD. 
 *
 * Translation unit 0251761C..02518B24 (d_cc_mass_s ends with its __sinit at 02517588 before it;
 * this unit's __sinit 025189E4 sits after ChkCamera and is followed by the per-TU copy of the
 * inline cCcS::ChkNoHitGAtTg, ChkNoHitGCo and MoveAfterCheck; d_cc_uty starts at 02518B28).
 * Ported from the GameCube d_cc_s.cpp; the constructor, Dt/Move/MassClear wrappers and the
 * __sinit are written from the WWHD code.
 *
 * Layout (HD): dCcS 0x29F4 as on GameCube; the cCcS part keeps its object lists inline (Co list
 * at +0x1000, Co count +0x2808, cCcD_DivideArea +0x2810) and its vtable at the end (+0x2850:
 * SetCoGCorrectProc +0x54, CalcParticleAngle +0x5C); dCcMassS_Mng at +0x2854.
 * Colliders (cCcD_Obj / dCcD_GObjInf): Co SPrm +0x2C (bit 0 set, 0x80 Sph3DCrr, 0x100 NoCrr),
 * stts +0x44, divide info +0x48, GetGObjInf vtable slot +0x1C, GetShapeAttr +0x2C (vtable +0x3C). */
#include "bindings.h"

namespace d_cc_s_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void PSVECSubtract_l(u32 a, u32 b, u32 out) { gabi::call(0x028E8DAC, a, b, out); }
static inline f32 PSVECMag_l(u32 v) { return gabi::call<f32>(0x028E8E10, v); }
static inline void PSVECNormalize_l(u32 in, u32 out) { gabi::call(0x028E8EF0, in, out); }
static inline void PSVECAdd_l(u32 a, u32 b, u32 out) { gabi::call(0x028E8D88, a, b, out); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline s16 cM_atan2s_l(f32 y, f32 x) { return gabi::call<s16>(0x020195B0, y, x); }
static inline s32 cLib_distanceAngleS_l(s16 a, s16 b) { return gabi::call<s32>(0x0200FAAC, a, b); }
static inline void cM3d_CalcVecZAngle_l(u32 v, u32 out) { gabi::call(0x02017300, v, out); }
static inline void dKy_SordFlush_set_l(u32 pos, s32 type) { gabi::call(0x0255F554, pos, type); }
static inline void cCcD_Stts_PlusCcMove_l(u32 stts, f32 x, f32 y, f32 z) { gabi::call(0x0200BE28, stts, x, y, z); }
static inline void cCcS_ct_l(u32 p) { gabi::call(0x0200E138, p); }
static inline void cCcS_Ct_l(u32 p) { gabi::call(0x0200E1C4, p); }
static inline void cCcS_Dt_l(u32 p) { gabi::call(0x0200E23C, p); }
static inline void cCcS_Move_l(u32 p) { gabi::call(0x0200E558, p); }
static inline void cCcS_DrawClear_l(u32 p) { gabi::call(0x0200E5BC, p); }
static inline void dCcMassS_Mng_ct_l(u32 p) { gabi::call(0x02516AB0, p); }
static inline void dCcMassS_Mng_Ct_l(u32 p) { gabi::call(0x02516A78, p); }
static inline void dCcMassS_Mng_Clear_l(u32 p) { gabi::call(0x025169A8, p); }
static inline void cM3dGCps_ct_l(u32 p) { gabi::call(0x02018150, p); }
static inline void cM3dGCps_Set_l(u32 p, u32 start, u32 end, f32 r) { gabi::call(0x020181B0, p, start, end, r); }
static inline void cCcD_CpsAttr_CalcAabBox_l(u32 p) { gabi::call(0x0200C2D8, p); }
static inline void cCcD_CpsAttr_dt_l(u32 p, s32 flags) { gabi::call(0x0200B600, p, flags); }
static inline BOOL cCcD_CpsAttr_CrossCo_l(u32 p, u32 shape, u32 out) { return gabi::call<BOOL>(0x0200C20C, p, shape, out); }
static inline void cCcD_DivideInfo_ct_l(u32 p) { gabi::call(0x0200B6C0, p); }
static inline void cCcD_DivideInfo_dt_l(u32 p, s32 flags) { gabi::call(0x0200B708, p, flags); }
static inline BOOL cCcD_DivideInfo_Chk_l(u32 p, u32 other) { return gabi::call<BOOL>(0x0200B71C, p, other); }
static inline void cCcD_DivideArea_CalcDivideInfoOverArea_l(u32 p, u32 info, u32 aab) { gabi::call(0x0200BAC0, p, info, aab); }
/* d_cc_d */
static inline u32 dCcD_GObjInf_ChkAtNoGuard_l(u32 inf) { return gabi::call<u32>(0x02516204, inf); }
static inline void dCcD_GAtTgCoCommonBase_SetEffCounterTimer_l(u32 b) { gabi::call(0x02515C40, b); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr f32 kZeroEps = 3.814697265625e-06f; /* cM3d_IsZero (0x100030B8) */

/* obj->GetGObjInf() (virtual, cCcD_Obj vtable at +0x3C) */
static inline u32 GetGObjInf(u32 obj) { return gabi::call_ptr<u32>(ld(ld(obj + 0x3C) + 0x1C), obj); }
/* dCcD_GObjInf::GetAc: the actor of its stts (stts +0x44, actor +0xC), or null */
static inline u32 GetAc(u32 stts) { return stts != 0 ? ld(stts + 0xC) : 0; }

/* 0251761C */
static s32 dCcS_GetRank(u32 self, u32 weight) {
    WWHD_FUNC(0x0251761C, s32, self, weight);
    if ((s32)weight == 0xFF) return 10;
    if ((s32)weight == 0xFE) return 9;
    if (weight >= 0xD9) return 8;
    if (weight >= 0xB5) return 7;
    if (weight >= 0x91) return 6;
    if (weight >= 0x6D) return 5;
    if (weight >= 0x49) return 4;
    if (weight >= 0x25) return 3;
    if (weight >= 0x02) return 2;
    return weight == 1;
}
VERIFY(0x0251761C, dCcS_GetRank);

/* 025176B8: the 9th argument (co2 GStts) is on the stack */
static void dCcS_SetCoGObjInf(u32 self, u32 co1Set, u32 co2Set, u32 co1Obj, u32 co2Obj, u32 co1Stts, u32 co2Stts,
                              u32 co1GStts) {
    WWHD_FUNC(0x025176B8, void, self, co1Set, co2Set, co1Obj, co2Obj, co1Stts, co2Stts, co1GStts);
    u32 co2GStts = ld(gabi::cpu->r[1] + 8);
    if (co1Set) {
        st(co1Obj + 0xEC, 0);                  /* SetCoHitApid: actor cache cleared */
        st(co1Obj + 0xE8, ld(co2Stts + 0x10));
        if (ld(co2GStts + 0x1C) & 1) st(co1Obj + 0xE0, ld(co1Obj + 0xE0) | 1); /* OnCoHitNoActor */
    }
    if (co2Set) {
        st(co2Obj + 0xEC, 0);
        st(co2Obj + 0xE8, ld(co1Stts + 0x10));
        if (ld(co1GStts + 0x1C) & 1) st(co2Obj + 0xE0, ld(co2Obj + 0xE0) | 1);
    }
    if (co1Set) {
        u32 cb = ld(co1Obj + 0xE4);
        if (cb != 0) {
            u32 s1 = ld(co1Obj + 0x44), s2 = ld(co2Obj + 0x44);
            u32 ac1 = GetAc(s1);
            u32 ac2 = GetAc(s2);
            gabi::call_ptr(cb, ac1, co1Obj, ac2, co2Obj);
        }
    }
    if (co2Set) {
        u32 cb = ld(co2Obj + 0xE4);
        if (cb != 0) {
            u32 s2 = ld(co2Obj + 0x44), s1 = ld(co1Obj + 0x44);
            u32 ac2 = GetAc(s2);
            u32 ac1 = GetAc(s1);
            gabi::call_ptr(cb, ac2, co2Obj, ac1, co1Obj);
        }
    }
}
VERIFY(0x025176B8, dCcS_SetCoGObjInf);

/* 02517828 */
static void dCcS_SetPosCorrect(u32 self, u32 obj1, u32 ppos1, u32 obj2, u32 ppos2, f32 cross_len) {
    WWHD_FUNC(0x02517828, void, self, obj1, ppos1, obj2, ppos2, cross_len);
    if (ld(obj1 + 0x2C) & 0x100) return; /* ChkCoNoCrr */
    if (ld(obj2 + 0x2C) & 0x100) return;
    {
        u32 s1 = ld(obj1 + 0x44);
        if (s1 == 0) return;
        u32 s2 = ld(obj2 + 0x44);
        if (s2 == 0) return;
        if (ld(s1 + 0xC) != 0 && ld(s1 + 0xC) == ld(s2 + 0xC)) return;
    }
    f32 abs_cross = std::fabs(cross_len);
    if (abs_cross < kZeroEps) return;

    gabi::call_ptr(ld(ld(self + 0x2850) + 0x54), self, obj1, obj2); /* SetCoGCorrectProc */

    bool correctY = false;
    if ((ld(obj1 + 0x2C) & 0x80) && (ld(obj2 + 0x2C) & 0x80)) correctY = true; /* ChkCoSph3DCrr */

    u32 s1 = ld(obj1 + 0x44);
    u32 s2 = ld(obj2 + 0x44);
    if (ld8(s1 + 0x14) == 0 && ld8(s2 + 0x14) == 0) return;
    if (ld8(s1 + 0x14) == 0xFF && ld8(s2 + 0x14) == 0xFF) return;
    s32 rank1 = dCcS_GetRank(self, ld8(s1 + 0x14));
    s32 rank2 = dCcS_GetRank(self, ld8(s2 + 0x14));

    f32 dz = ldf(ppos2 + 8) - ldf(ppos1 + 8);
    u8 rank = ld8(0x101D5760 + rank1 * 11 + rank2); /* rank_tbl */
    f32 dx = ldf(ppos2 + 0) - ldf(ppos1 + 0);
    u8 invRank = (u8)(100 - rank);
    f32 obj2Weight = (f32)rank * 0.01f;
    f32 obj1Weight = (f32)invRank * 0.01f;

    gabi::Local<cXyz> vec2; /* sp+0x08 */
    gabi::Local<cXyz> vec1; /* sp+0x14 */
    f32 dy, len;
    bool main_path;
    if (correctY) {
        dy = ldf(ppos2 + 4) - ldf(ppos1 + 4);
        len = sqrtf_l(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
        main_path = !(std::fabs(len) < kZeroEps); /* bge: taken on NaN */
    } else {
        f32 zz = dz * dz;
        dy = 0.0f;
        len = sqrtf_l(gabi::fmadds(dx, dx, zz));
        main_path = !(std::fabs(len) < kZeroEps); /* blt: not taken on NaN */
    }
    u32 v1 = gabi::ea(vec1.get()), v2 = gabi::ea(vec2.get());
    if (main_path) {
        f32 push = cross_len / len;
        dx = dx * push;
        dz = dz * push;
        f32 x1 = -(dx * obj2Weight);
        stf(v2 + 0, dx * obj1Weight);
        f32 z1 = -(dz * obj2Weight);
        stf(v1 + 0, x1);
        stf(v1 + 8, z1);
        stf(v2 + 8, dz * obj1Weight);
        f32 y1;
        if (correctY) {
            dy = dy * push;
            y1 = -(dy * obj2Weight);
            stf(v1 + 4, y1);
            stf(v2 + 4, dy * obj1Weight);
        } else {
            y1 = 0.0f;
            stf(v2 + 4, 0.0f);
            stf(v1 + 4, y1);
        }
        cCcD_Stts_PlusCcMove_l(ld(obj1 + 0x44), x1, y1, z1);
    } else {
        stf(v2 + 8, 0.0f);
        stf(v2 + 4, 0.0f);
        stf(v1 + 4, 0.0f);
        stf(v1 + 8, 0.0f);
        f32 x1;
        if (!(abs_cross < kZeroEps)) {
            x1 = -(cross_len * obj2Weight);
            stf(v1 + 0, x1);
            stf(v2 + 0, cross_len * obj1Weight);
        } else {
            x1 = -obj2Weight;
            stf(v2 + 0, obj1Weight);
            stf(v1 + 0, x1);
        }
        cCcD_Stts_PlusCcMove_l(ld(obj1 + 0x44), x1, 0.0f, 0.0f);
    }
    {
        f32 x2 = ldf(v2 + 0), y2 = ldf(v2 + 4), z2 = ldf(v2 + 8);
        cCcD_Stts_PlusCcMove_l(ld(obj2 + 0x44), x2, y2, z2);
    }
    PSVECAdd_l(ppos1, v1, ppos1);
    PSVECAdd_l(ppos2, v2, ppos2);
}
VERIFY(0x02517828, dCcS_SetPosCorrect);

/* 02517C70 */
static u32 dCcS_ChkShieldFrontRange(u32 self, u32 obj1, u32 obj2) {
    WWHD_FUNC(0x02517C70, u32, self, obj1, obj2);
    u32 inf1 = GetGObjInf(obj1);
    if (inf1 == 0) return 0;
    u32 s1 = ld(inf1 + 0x44);
    if (s1 == 0) return 0;
    u32 ac1 = ld(s1 + 0xC);
    if (ac1 == 0) return 0;
    u32 inf2 = GetGObjInf(obj2);
    if (inf2 == 0) return 0;
    u32 s2 = ld(inf2 + 0x44);
    if (s2 == 0) return 0;
    u32 ac2 = ld(s2 + 0xC);
    if (ac2 == 0) return 0;

    gabi::Local<cXyz> delta;
    u32 d = gabi::ea(delta.get());
    PSVECSubtract_l(ac1 + 0x37C, ac2 + 0x37C, d); /* eyePos */
    f32 dist = PSVECMag_l(d);
    if (std::fabs(dist) < kZeroEps) return 0;
    PSVECNormalize_l(d, d);
    s16 deltaAngle = cM_atan2s_l(ldf(d + 0), ldf(d + 8));
    u32 shield = ld(inf2 + 0xD8); /* mpShieldFrontRangeYAngle */
    s16 shieldAngle = shield != 0 ? gabi::load<s16>(shield) : gabi::load<s16>(ac2 + 0x32A); /* shape_angle.y */
    if (cLib_distanceAngleS_l(deltaAngle, shieldAngle) > 0x4000) return 0;
    return 1;
}
VERIFY(0x02517C70, dCcS_ChkShieldFrontRange);

/* 02517DEC */
static u32 dCcS_ChkShield(u32 self, u32 obj1, u32 obj2, u32 inf1, u32 inf2) {
    WWHD_FUNC(0x02517DEC, u32, self, obj1, obj2, inf1, inf2);
    if (dCcD_GObjInf_ChkAtNoGuard_l(inf1)) return 0;
    u32 tgSPrm = ld(inf2 + 0x94);
    if (!(tgSPrm & 1)) return 0;   /* ChkTgShield */
    if (!(tgSPrm & 8)) return 1;   /* ChkTgShieldFrontRange */
    return dCcS_ChkShieldFrontRange(self, obj1, obj2);
}
VERIFY(0x02517DEC, dCcS_ChkShield);

/* 02517E94: arguments 9..12 (tg stts, at/tg GStts, pos) are on the stack */
static void dCcS_ProcAtTgHitmark(u32 self, u32 b1, u32 b2, u32 atObj, u32 tgObj, u32 atInf, u32 tgInf, u32 atStts) {
    WWHD_FUNC(0x02517E94, void, self, b1, b2, atObj, tgObj, atInf, tgInf, atStts);
    u32 sp = gabi::cpu->r[1];
    u32 tgStts = ld(sp + 8);
    u32 tgGStts = ld(sp + 0x10);
    u32 pos = ld(sp + 0x14);
    cXyz* ppos = gabi::at<cXyz>(pos);

    if (ld(atInf + 0x50) & 2) return;     /* ChkAtNoHitMark */
    if (ld(tgInf + 0x94) & 4) return;     /* ChkTgNoHitMark */
    if (ld8(tgInf + 0xB2) == 0xFF) return; /* GetTgHitMark */
    if (tgGStts == 0) return;             /* HD: null check */
    if (gabi::load<u16>(tgGStts + 8) != 0xFFFF) return; /* ChkNoneActorPerfTblId */

    if (!dCcS_ChkShield(self, atObj, tgObj, atInf, tgInf)) {
        u8 mark = ld8(atInf + 0x6E); /* GetAtHitMark */
        if (mark == 0) return;
        if (mark == 1 && ld8(tgInf + 0xB2) == 1) {
            dComIfGp_particle_set(0x27B, ppos); /* ID_IT_JN_PIYOHIT00 */
            return;
        }
        gabi::Local<csXyz> angle; /* sp+0x3C */
        gabi::call_ptr(ld(ld(self + 0x2850) + 0x5C), self, atInf, atStts, tgStts, angle.get()); /* CalcParticleAngle */
        mark = ld8(atInf + 0x6E);
        gabi::Local<cXyz> flush; /* sp+0x18 */
        if (mark == 0xF) {
            dComIfGp_particle_set(0x10, ppos); /* ID_AK_JN_CRITICALHITFLASH */
            gabi::Local<cXyz> scale;
            scale->x = 2.0f;
            scale->z = 2.0f;
            scale->y = 2.0f;
            dComIfGp_particle_set(0xD, ppos, angle.get(), scale.get()); /* ID_AK_JN_OK */
        } else if (mark == 1) {
            dComIfGp_particle_set(0xD, ppos, angle.get());
        } else {
            dComIfGp_particle_set(mark, ppos, angle.get());
        }
        flush->x = ppos->x;
        flush->y = ppos->y;
        flush->z = ppos->z;
        dKy_SordFlush_set_l(gabi::ea(flush.get()), 1);
    } else {
        if (ld8(tgInf + 0xB2) == 0) return;
        {
            gabi::Local<cXyz> flush; /* sp+0x30 */
            flush->x = ppos->x;
            flush->y = ppos->y;
            flush->z = ppos->z;
            dKy_SordFlush_set_l(gabi::ea(flush.get()), 0);
        }
        gabi::Local<csXyz> angle; /* sp+0x44 */
        gabi::call_ptr(ld(ld(self + 0x2850) + 0x5C), self, atInf, atStts, tgStts, angle.get());
        u8 mark = ld8(tgInf + 0xB2);
        dComIfGp_particle_set(mark, ppos, angle.get());
    }
}
VERIFY(0x02517E94, dCcS_ProcAtTgHitmark);

/* 0251821C */
static void dCcS_CalcParticleAngle(u32 self, u32 atInf, u32 atStts, u32 tgStts, u32 o_angle) {
    WWHD_FUNC(0x0251821C, void, self, atInf, atStts, tgStts, o_angle);
    gabi::Local<cXyz> vec;
    u32 v = gabi::ea(vec.get());
    stf(v + 0, ldf(atInf + 0x7C)); /* *GetAtVecP() */
    stf(v + 4, ldf(atInf + 0x80));
    stf(v + 8, ldf(atInf + 0x84));
    bool normalize = !(std::fabs(PSVECMag_l(v)) < kZeroEps); /* bge: taken on NaN */
    if (!normalize) {
        u32 atActor = ld(atStts + 0xC);
        u32 tgActor = ld(tgStts + 0xC);
        if (atActor != 0 && tgActor != 0) {
            PSVECSubtract_l(tgActor + 0x314, atActor + 0x314, v); /* current.pos */
            normalize = !(std::fabs(PSVECMag_l(v)) < kZeroEps);
        }
    }
    if (normalize) {
        PSVECNormalize_l(v, v);
    } else {
        stf(v + 8, 0.0f);
        stf(v + 4, -1.0f);
        stf(v + 0, 0.0f);
    }
    cM3d_CalcVecZAngle_l(v, o_angle);
}
VERIFY(0x0251821C, dCcS_CalcParticleAngle);

/* 02518328: arguments 9..12 (tg stts, at/tg GStts, hit pos) are on the stack */
static void dCcS_SetAtTgGObjInf(u32 self, u32 setAt, u32 setTg, u32 atObj, u32 tgObj, u32 atInf, u32 tgInf,
                                u32 atStts) {
    WWHD_FUNC(0x02518328, void, self, setAt, setTg, atObj, tgObj, atInf, tgInf, atStts);
    u32 sp = gabi::cpu->r[1];
    u32 tgStts = ld(sp + 8);
    u32 atGStts = ld(sp + 0xC);
    u32 tgGStts = ld(sp + 0x10);
    u32 hitPos = ld(sp + 0x14);

    u32 shield = dCcS_ChkShield(self, atObj, tgObj, atInf, tgInf);
    if (setAt) {
        for (u32 i = 0; i < 12; i += 4) st(atInf + 0x70 + i, ld(hitPos + i));     /* SetAtHitPos */
        for (u32 i = 0; i < 12; i += 4) st(atInf + 0x88 + i, ld(tgInf + 0xB4 + i)); /* SetAtRVec(*GetTgVecP()) */
        if (atGStts != 0 && ld8(atGStts + 5) == 0) gabi::store<u8>(atGStts + 5, ld8(tgInf + 0xB3)); /* TgSpl */
        u32 apid = ld(tgStts + 0x10);
        st(atInf + 0x60, 0); /* SetAtHitApid */
        st(atInf + 0x5C, apid);
        if (shield) st(atInf + 0x54, ld(atInf + 0x54) | 1); /* OnAtShieldHit */
        if (tgGStts != 0 && (ld(tgGStts + 0x1C) & 1)) st(atInf + 0x54, ld(atInf + 0x54) | 2); /* OnAtHitNoActor */
    }
    if (setTg) {
        for (u32 i = 0; i < 12; i += 4) st(tgInf + 0xCC + i, ld(hitPos + i));     /* SetTgHitPos */
        for (u32 i = 0; i < 12; i += 4) st(tgInf + 0xC0 + i, ld(atInf + 0x7C + i)); /* SetTgRVec(*GetAtVecP()) */
        if (atGStts != 0) { /* HD: the rest needs the at GStts */
            if (tgGStts != 0 && ld8(atGStts + 4) == 0) gabi::store<u8>(tgGStts + 4, ld8(atInf + 0x6F)); /* AtSpl */
            u32 apid = ld(atStts + 0x10);
            st(tgInf + 0xA4, 0); /* SetTgHitApid */
            st(tgInf + 0xA0, apid);
            if (shield) st(tgInf + 0x98, ld(tgInf + 0x98) | 2); /* OnTgShieldHit */
            if (ld(atGStts + 0x1C) & 1) st(tgInf + 0x98, ld(tgInf + 0x98) | 1); /* OnTgHitNoActor */
        }
    }
    if (setAt) {
        u32 cb = ld(atInf + 0x58);
        if (cb != 0) {
            u32 sa = ld(atInf + 0x44), sb = ld(tgInf + 0x44);
            u32 acA = GetAc(sa);
            u32 acB = GetAc(sb);
            gabi::call_ptr(cb, acA, atInf, acB, tgInf);
        }
    }
    if (setTg) {
        u32 cb = ld(tgInf + 0x9C);
        if (cb != 0) {
            u32 sb = ld(tgInf + 0x44), sa = ld(atInf + 0x44);
            u32 acB = GetAc(sb);
            u32 acA = GetAc(sa);
            gabi::call_ptr(cb, acB, tgInf, acA, atInf);
        }
    }
    if (setAt && setTg && (!(gabi::load<s8>(atInf + 0x64) > 0) || !(gabi::load<s8>(tgInf + 0xA8) > 0))) {
        dCcD_GAtTgCoCommonBase_SetEffCounterTimer_l(atInf + 0x50);
        dCcD_GAtTgCoCommonBase_SetEffCounterTimer_l(tgInf + 0x94);
        gabi::call(0x02517E94, self, setAt, setTg, atObj, tgObj, atInf, tgInf, atStts, tgStts, atGStts, tgGStts,
                   hitPos); /* ProcAtTgHitmark */
    }
}
VERIFY(0x02518328, dCcS_SetAtTgGObjInf);

/* 025185F8: the 9th argument (tg GStts) is on the stack */
static bool dCcS_ChkAtTgHitAfterCross(u32 self, u32 setAt, u32 setTg, u32 atInf, u32 tgInf, u32 atStts, u32 tgStts,
                                      u32 atGStts) {
    WWHD_FUNC(0x025185F8, bool, self, setAt, setTg, atInf, tgInf, atStts, tgStts, atGStts);
    u32 tgGStts = ld(gabi::cpu->r[1] + 8);
    u32 tgApid = ld(tgStts + 0x10);
    u32 atApid = ld(atStts + 0x10);
    if (setAt) {
        st(atGStts + 0xC, tgApid); /* SetAtApid */
        if ((ld(atInf + 0x50) & 1) && ld(atGStts + 0x10) == ld(tgStts + 0x10)) return true; /* ChkAtNoConHit */
    }
    if (setTg) {
        st(tgGStts + 0x14, atApid); /* SetTgApid */
        if ((ld(tgInf + 0x94) & 2) && !(ld(atInf + 0x50) & 4) && ld(tgGStts + 0x18) == ld(atStts + 0x10))
            return true; /* ChkTgNoConHit, ChkAtStopNoConHit */
    }
    return false;
}
VERIFY(0x025185F8, dCcS_ChkAtTgHitAfterCross);

/* 02518670 */
static void dCcS_CalcTgPlusDmg(u32 self, u32 obj1, u32 obj2, u32 stts1, u32 stts2) {
    WWHD_FUNC(0x02518670, void, self, obj1, obj2, stts1, stts2);
    u32 inf1 = GetGObjInf(obj1);
    u32 inf2 = GetGObjInf(obj2);
    if (!dCcS_ChkShield(self, obj1, obj2, inf1, inf2)) {
        u8 atp = ld8(obj1 + 0x14); /* GetAtAtp */
        if ((s32)ld8(stts2 + 0x16) < (s32)atp) gabi::store<u8>(stts2 + 0x16, atp); /* PlusDmg */
    }
}
VERIFY(0x02518670, dCcS_CalcTgPlusDmg);

/* 02518704: dCcS::dCcS (0x29F4) */
static u32 dCcS_ct(u32 p) {
    WWHD_FUNC(0x02518704, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x29F4));
        if (p == 0) return p;
    }
    cCcS_ct_l(p);
    st(p + 0x2850, 0x1004B3E8);
    dCcMassS_Mng_ct_l(p + 0x2854);
    return p;
}
VERIFY(0x02518704, dCcS_ct);

/* 02518760 */
static void dCcS_Ct(u32 self) {
    WWHD_FUNC(0x02518760, void, self);
    cCcS_Ct_l(self);
    dCcMassS_Mng_Ct_l(self + 0x2854);
}
VERIFY(0x02518760, dCcS_Ct);

/* 02518794 */
static void dCcS_Dt(u32 self) {
    WWHD_FUNC(0x02518794, void, self);
    cCcS_Dt_l(self);
}
VERIFY(0x02518794, dCcS_Dt);

/* 02518798 */
static void dCcS_Move(u32 self) {
    WWHD_FUNC(0x02518798, void, self);
    cCcS_Move_l(self);
}
VERIFY(0x02518798, dCcS_Move);

/* 0251879C (DrawAfter is empty and inlined) */
static void dCcS_Draw(u32 self) {
    WWHD_FUNC(0x0251879C, void, self);
    cCcS_DrawClear_l(self);
    dCcMassS_Mng_Clear_l(self + 0x2854);
}
VERIFY(0x0251879C, dCcS_Draw);

/* 025187D0 */
static void dCcS_MassClear(u32 self) {
    WWHD_FUNC(0x025187D0, void, self);
    dCcMassS_Mng_Clear_l(self + 0x2854);
}
VERIFY(0x025187D0, dCcS_MassClear);

/* 025187D8 */
static bool dCcS_ChkCamera(u32 self, u32 start, u32 end, f32 radius, u32 ac1, u32 ac2) {
    WWHD_FUNC(0x025187D8, bool, self, start, end, radius, ac1, ac2);
    s32 count = (s32)ld(self + 0x2808); /* mObjCoCount */
    if (count <= 0) return false;
    u32 it = self + 0x1000;             /* Co list */
    u32 end_it = it + (u32)count * 4;
    /* frame: cCcD_DivideInfo +0x0 (8), cross_len +0x8, cCcD_CpsAttr +0xC (0x40: aab, vtables +0x18/+0x1C,
     * cM3dGCps +0x20 with its vtable at +0x38) */
    gabi::Local<u8[0x4C]> frame;
    u32 f = gabi::ea(frame.get());
    u32 div = f, crossLen = f + 0x8, cps = f + 0xC;
    st(cps + 0x1C, 0x100015A8); /* cCcD_ShapeAttr */
    st(cps + 0x18, 0x1004B3AC); /* cM3dGAab (this unit's copy) */
    cM3dGCps_ct_l(cps + 0x20);
    st(cps + 0x1C, 0x10001678); /* cCcD_CpsAttr */
    st(cps + 0x38, 0x10001718); /* cM3dGCps */
    cM3dGCps_Set_l(cps + 0x20, start, end, radius);
    cCcD_CpsAttr_CalcAabBox_l(cps);
    cCcD_DivideInfo_ct_l(div);
    cCcD_DivideArea_CalcDivideInfoOverArea_l(self + 0x2810, div, cps);
    for (; it < end_it; it += 4) {
        u32 obj = ld(it);
        if (!(ld(obj + 0x2C) & 1)) continue; /* ChkCoSet */
        if (GetAc(ld(obj + 0x44)) == ac1) continue;
        if (GetAc(ld(obj + 0x44)) == ac2) continue;
        if (!cCcD_DivideInfo_Chk_l(obj + 0x48, div)) continue;
        obj = ld(it);
        u32 shape = gabi::call_ptr<u32>(ld(ld(obj + 0x3C) + 0x2C), obj); /* GetShapeAttr */
        if (shape == 0) continue;
        if (cCcD_CpsAttr_CrossCo_l(cps, shape, crossLen)) {
            cCcD_DivideInfo_dt_l(div, 2);
            cCcD_CpsAttr_dt_l(cps, 2);
            return true;
        }
    }
    cCcD_DivideInfo_dt_l(div, 2);
    cCcD_CpsAttr_dt_l(cps, 2);
    return false;
}
VERIFY(0x025187D8, dCcS_ChkCamera);

/* 025189E4: static initialisers (header statics) */
static void __sinit_d_cc_s_cpp() {
    WWHD_FUNC(0x025189E4, void, (u32)0);
    sinit_header_statics(0x1046F000, 0x101D57DC);
}
VERIFY(0x025189E4, __sinit_d_cc_s_cpp);

/* 02518A78: this unit's copy of the inline cCcS::ChkNoHitGAtTg */
static bool cCcS_ChkNoHitGAtTg(u32 self, u32 atInf, u32 tgInf, u32 atGStts, u32 tgGStts) {
    WWHD_FUNC(0x02518A78, bool, self, atInf, tgInf, atGStts, tgGStts);
    return false;
}
VERIFY(0x02518A78, cCcS_ChkNoHitGAtTg);

/* 02518A80 */
static bool dCcS_ChkNoHitGCo(u32 self, u32 obj1, u32 obj2) {
    WWHD_FUNC(0x02518A80, bool, self, obj1, obj2);
    u32 inf1 = GetGObjInf(obj1);
    u32 inf2 = GetGObjInf(obj2);
    u32 co1 = ld(inf1 + 0xDC), co2 = ld(inf2 + 0xDC); /* dCcD_GObjCo SPrm: 1 AtLasso, 2 TgLasso */
    if ((co1 & 1) && !(co2 & 2)) return true;
    if ((co2 & 1) && !(co1 & 2)) return true;
    return false;
}
VERIFY(0x02518A80, dCcS_ChkNoHitGCo);

/* 02518B24 */
static void dCcS_MoveAfterCheck(u32 self) {
    WWHD_FUNC(0x02518B24, void, self);
}
VERIFY(0x02518B24, dCcS_MoveAfterCheck);

}  // namespace d_cc_s_cpp
