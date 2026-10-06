/**
 * d_a_tsubo_mode.cpp (WWHD)
 * Small carriable objects (pots, skulls, barrels, ...): the mode procedures, mode_proc_call,
 * _execute, _draw, the set_tensor_* and eff_break_* functions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tsubo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include <cmath>
#include "d/actor/d_a_tsubo.h"

using namespace daTsubo;

/* ---- tsubo functions outside this file (guest calls by address) ---- */
static inline void set_tensor(Act_c* a, const cXyz* v) { gabi::call(0x024CB088, a, v); }
static inline void mode_appear_init(Act_c* a) { gabi::call(0x024CAB30, a); }
static inline void mode_wait_init(Act_c* a) { gabi::call(0x024CAC44, a); }
static inline void init_rot_clean(Act_c* a) { gabi::call(0x024CAA54, a); }
static inline bool chk_sink_water(Act_c* a) { return gabi::call<bool>(0x024CCB44, a); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02006760 cSAngle::Radian() const, 02019510 cM_rad2s(f32) */
static inline f32 cSAngle_Radian(be<s16>* a) { return gabi::call<f32>(0x02006760, a); }
static inline s16 cM_rad2s(f32 r) { return gabi::call<s16>(0x02019510, r); }
/* 0200658C cSAngle::cSAngle(s16) (returns this) */
static inline be<s16>* cSAngle_ct_s16(be<s16>* a, s16 v) { return gabi::call<be<s16>*>(0x0200658C, a, v); }
/* 02312968 daObj::SetCurrentRoomNo(actor, dBgS_GndChk*) */
static inline void daObj_SetCurrentRoomNo(fopAc_ac_c* a, void* gnd) { gabi::call(0x02312968, a, gnd); }
/* mAcch.m_gnd as passed by address (the dBgS_GndChk itself: Acch +0xD4) */
static inline void* acch_gnd(Act_c* a) { return gabi::at<u8>(gabi::ea(&a->mAcch) + 0xD4); }
#define cXyz_BaseX gabi::at<cXyz>(0x101FFBB4)

/* the common tail of the set_tensor_* functions: the axis BaseX rotated by m692/m68A/m688 */
static inline void tensor_axis(Act_c* a) {
    mDoMtx_YrotS(mDoMtx_stack_c::get(), a->m692);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), a->m68A);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), a->m688);
    gabi::Local<cXyz> sp;
    PSMTXMultVecSR(mDoMtx_stack_c::get(), cXyz_BaseX, sp);
    set_tensor(a, sp);
}
/* angle -= rad2s(r * k1 + r * |r| * k2) (HD: fmadds) */
static inline void tensor_damp(be<s16>* ang, Act_c* a, u32 off1, u32 off2) {
    f32 r = cSAngle_Radian(ang);
    Act_c::Data_c* d = data(a);
    f32 k2 = gabi::load<f32>(gabi::ea(d) + off2);
    f32 k1 = gabi::load<f32>(gabi::ea(d) + off1);
    cSAngle_subeq(ang, cM_rad2s(gabi::fmadds(r, k1, (std::fabs(r) * r) * k2)));
}

/* 024CFE88..024CFEC4: moment_small ... moment_woodS (empty) */
static void moment_small(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE88, void, a);
}
VERIFY(0x024CFE88, moment_small);
static void moment_big(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE8C, void, a);
}
VERIFY(0x024CFE8C, moment_big);
static void moment_water(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE90, void, a);
}
VERIFY(0x024CFE90, moment_water);
static void moment_barrel(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE94, void, a);
}
VERIFY(0x024CFE94, moment_barrel);
static void moment_stool(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE98, void, a);
}
VERIFY(0x024CFE98, moment_stool);
static void moment_skull(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFE9C, void, a);
}
VERIFY(0x024CFE9C, moment_skull);
static void moment_pail(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEA0, void, a);
}
VERIFY(0x024CFEA0, moment_pail);
static void moment_spine(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEA4, void, a);
}
VERIFY(0x024CFEA4, moment_spine);
static void moment_hbox2S(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEA8, void, a);
}
VERIFY(0x024CFEA8, moment_hbox2S);
static void moment_tryColSun(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEAC, void, a);
}
VERIFY(0x024CFEAC, moment_tryColSun);
static void moment_tryColMer(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEB0, void, a);
}
VERIFY(0x024CFEB0, moment_tryColMer);
static void moment_tryColJup(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEB4, void, a);
}
VERIFY(0x024CFEB4, moment_tryColJup);
static void moment_tryKeyGate(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEB8, void, a);
}
VERIFY(0x024CFEB8, moment_tryKeyGate);
static void moment_pinecone(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEBC, void, a);
}
VERIFY(0x024CFEBC, moment_pinecone);
static void moment_kutani(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEC0, void, a);
}
VERIFY(0x024CFEC0, moment_kutani);
static void moment_woodS(Act_c* a, const cXyz*) {
    WWHD_FUNC(0x024CFEC4, void, a);
}
VERIFY(0x024CFEC4, moment_woodS);

/* 024CFEC8; HD: the two (identical) prm_get_stick branches are merged */
static void set_tensor_wait(Act_c* a) {
    WWHD_FUNC(0x024CFEC8, void, a);
    tensor_axis(a);
}
VERIFY(0x024CFEC8, set_tensor_wait);

/* 024CFF40 set_tensor_hide, 024CFF44 set_tensor_appear, 024D0070 set_tensor_carry */
static void set_tensor_hide(Act_c* a) {
    WWHD_FUNC(0x024CFF40, void, a);
    set_tensor_wait(a);
}
VERIFY(0x024CFF40, set_tensor_hide);
static void set_tensor_appear(Act_c* a) {
    WWHD_FUNC(0x024CFF44, void, a);
    set_tensor_wait(a);
}
VERIFY(0x024CFF44, set_tensor_appear);
static void set_tensor_carry(Act_c* a) {
    WWHD_FUNC(0x024D0070, void, a);
    set_tensor_wait(a);
}
VERIFY(0x024D0070, set_tensor_carry);

/* 024CFF48 */
static void set_tensor_walk(Act_c* a) {
    WWHD_FUNC(0x024CFF48, void, a);
    gabi::Local<cXyz> sp28;
    cXyz_mi(&a->current.pos, sp28, &a->old.pos);
    f32 len = std_sqrtf(PSVECSquareMag(sp28));
    f32 abs = (len + len) / (f32)(u8)data(a)->mAcchRoofHeight;
    f32 r = cSAngle_Radian(&a->m690);
    cSAngle_Val(&a->m690, cM_rad2s(gabi::fmadds(abs - r, 0.7f, r)));
    cSAngle_addeq(&a->m68E, &a->m690);
    gabi::Local<be<s16>> sp08;
    be<s16>* p = cSAngle_ct_s16(sp08, a->current.angle.y);
    a->m692 = *p;
    tensor_axis(a);
}
VERIFY(0x024CFF48, set_tensor_walk);

/* 024D0074 set_tensor_drop (matcher: unnamed) */
static void set_tensor_drop(Act_c* a) {
    WWHD_FUNC(0x024D0074, void, a);
    tensor_damp(&a->m68C, a, 0x30, 0x34);
    cSAngle_addeq(&a->m68A, &a->m68C);
    tensor_damp(&a->m690, a, 0x30, 0x34);
    cSAngle_addeq(&a->m68E, &a->m690);
    tensor_axis(a);
}
VERIFY(0x024D0074, set_tensor_drop);

/* 024D0184 set_tensor_sink (matcher: unnamed) */
static void set_tensor_sink(Act_c* a) {
    WWHD_FUNC(0x024D0184, void, a);
    tensor_damp(&a->m68C, a, 0x48, 0x4C);
    cSAngle_addeq(&a->m68A, &a->m68C);
    tensor_damp(&a->m690, a, 0x48, 0x4C);
    cSAngle_addeq(&a->m68E, &a->m690);
    tensor_axis(a);
}
VERIFY(0x024D0184, set_tensor_sink);

/* 024D0294 */
static void set_tensor_afl(Act_c* a) {
    WWHD_FUNC(0x024D0294, void, a);
    tensor_damp(&a->m68C, a, 0x48, 0x4C);
    cSAngle_addeq(&a->m68A, &a->m68C);
    f32 r = cSAngle_Radian(&a->m68E);
    cSAngle_addeq(&a->m690, cM_rad2s(r * -0.0005f));
    cSAngle_addeq(&a->m690, (s16)gabi::ftoi(cM_ssin(a->m806) * 4.0f));
    tensor_damp(&a->m690, a, 0x48, 0x4C);
    cSAngle_addeq(&a->m68E, &a->m690);
    tensor_axis(a);
}
VERIFY(0x024D0294, set_tensor_afl);

/* 024CF4E0 */
static void init_rot_throw(Act_c* a) {
    WWHD_FUNC(0x024CF4E0, void, a);
    cSAngle_Val(&a->m688, (s16)data(a)->m28);
    cSAngle_muleq(&a->m688, cM_rnd());
    cSAngle_Val(&a->m68A, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
    cSAngle_Val(&a->m68C, (s16)data(a)->m2A);
    cSAngle_Val(&a->m68E, cSAngle__0);
    cSAngle_Val(&a->m690, (s16)data(a)->m2C);
    cSAngle_Val(&a->m692, (s16)a->current.angle.y);
}
VERIFY(0x024CF4E0, init_rot_throw);

/* 024CEAFC */
static void mode_hide(Act_c* a) {
    WWHD_FUNC(0x024CEAFC, void, a);
    if (is_switch(a)) {
        mode_appear_init(a);
    }
}
VERIFY(0x024CEAFC, mode_hide);

/* 024CFA48; HD: the sign test is an fsel (a NaN difference takes 0.8) */
static void water_tention(Act_c* a) {
    WWHD_FUNC(0x024CFA48, void, a);
    if (chk_sink_water(a)) {
        if (a->m4FC != -1000000000.0f && a->m500 != -1000000000.0f) {
            f32 d = a->m4FC - a->m500;
            f32 k = d >= 0.0f ? 0.2f : 0.8f;
            a->current.pos.y = gabi::fmadds(d, k, a->current.pos.y);
        }
    }
}
VERIFY(0x024CFA48, water_tention);

/* 024CFAB8 */
static void mode_sink(Act_c* a) {
    WWHD_FUNC(0x024CFAB8, void, a);
    daObj_SetCurrentRoomNo(a, acch_gnd(a));
    water_tention(a);
    Act_c::Data_c* d = data(a);
    daObj_posMoveF_stream(a, &a->mStts.m_cc_move, cXyz_Zero, d->m40, d->m44);
}
VERIFY(0x024CFAB8, mode_sink);

/* the water/lava level of calc_drop_param / calc_afl_param: false when neither applies */
static inline bool drop_level(Act_c* a, f32* d3) {
    bool bVar5 = a->m4FC != -1000000000.0f;
    bool fVar5 = a->m4F8 > a->current.pos.y;
    if (!bVar5 && !fVar5) {
        return false;
    }
    f32 lv;
    if (bVar5 && fVar5) {
        lv = (a->m4FC - a->m4F8 >= 0.0f) ? (f32)a->m4FC : (f32)a->m4F8; /* HD: fsel */
    } else if (bVar5) {
        lv = a->m4FC;
    } else {
        lv = a->m4F8;
    }
    *d3 = a->current.pos.y - lv;
    return true;
}

/* 024CED64 */
static void calc_drop_param(Act_c* a, be<f32>* o1, be<f32>* o2, be<f32>* o3) {
    WWHD_FUNC(0x024CED64, void, a, o1, o2, o3);
    f32 f3;
    if (drop_level(a, &f3)) {
        Act_c::Data_c* d = data(a);
        f32 f0;
        if (!(f3 < 0.0f)) {
            f0 = 0.0f;
        } else if (!(f3 > (f32)(-(s32)(u8)d->mAcchRoofHeight))) {
            f0 = 0.5f;
        } else {
            f0 = -(f3 * (0.5f / (f32)(u8)d->mAcchRoofHeight));
        }
        f32 f6 = 1.0f - f0;
        *o2 = gabi::fmadds(f0, d->m40, f6 * d->m20);
        d = data(a);
        *o3 = gabi::fmadds(f0, d->m44, f6 * d->m24);
        d = data(a);
        f32 gmax = d->mGravity * 0.1f;
        f32 g = gabi::fmadds(f0, d->m3C, d->mGravity);
        *o1 = (gmax - g >= 0.0f) ? g : gmax; /* HD: fsel */
    } else {
        *o2 = data(a)->m20;
        *o3 = data(a)->m24;
        *o1 = data(a)->mGravity;
    }
}
VERIFY(0x024CED64, calc_drop_param);

/* 024CFB20; HD: -fVar3 * (1 / h) became -(fVar3 / h) */
static void calc_afl_param(Act_c* a, be<f32>* o1, be<f32>* o2, be<f32>* o3) {
    WWHD_FUNC(0x024CFB20, void, a, o1, o2, o3);
    f32 f3;
    if (drop_level(a, &f3)) {
        Act_c::Data_c* d = data(a);
        f32 f0;
        if (!(f3 < 0.0f)) {
            f0 = 0.0f;
        } else if (!(f3 > (f32)(-(s32)(u8)d->mAcchRoofHeight))) {
            f0 = 1.0f;
        } else {
            f0 = -(f3 / (f32)(u8)d->mAcchRoofHeight);
        }
        f32 f6 = 1.0f - f0;
        *o2 = gabi::fmadds(f0, d->m40, f6 * d->m20);
        d = data(a);
        *o3 = gabi::fmadds(f0, d->m44, f6 * d->m24);
        d = data(a);
        *o1 = gabi::fmadds(f0, d->m3C, d->mGravity);
    } else {
        *o2 = data(a)->m20;
        *o3 = data(a)->m24;
        *o1 = data(a)->mGravity;
    }
}
VERIFY(0x024CFB20, calc_afl_param);

/* ---- more local bindings (SHARED-CANDIDATE) ---- */
/* 0200ECD4 cLib_addCalc(f32* v, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline void cLib_addCalc(be<f32>* v, f32 t, f32 s, f32 mx, f32 mn) { gabi::call(0x0200ECD4, v, t, s, mx, mn); }
/* 023121C4 daObj::posMoveF_grade(actor, ccMove, stream, f32 k1, f32 k2, const cXyz* gradeNormal,
 * f32 grade, f32 noGradeCos, const cXyz* moment) */
static inline void daObj_posMoveF_grade(fopAc_ac_c* a, const cXyz* cc, const cXyz* stream, f32 k1, f32 k2, const void* nrm, f32 g,
                                        f32 cosg, const cXyz* mom, u32 harness_r8) {
    gabi::call(0x023121C4, a, cc, stream, nrm, mom, harness_r8, k1, k2, g, cosg);
}
/* HARNESS WORKAROUND: funcdb parses the GameCube symbols of the daObj:: namespace functions
 * (posMoveF_grade__5daObjFP10fopAc_ac_c..., SetCurrentRoomNo__5daObjF...) as member functions, so
 * the harness compares one integer register more than they take (posMoveF_grade r8,
 * posMoveF_stream r6, SetCurrentRoomNo r5). The candidate passes what the original leaves there;
 * the unit file declares those registers output-only (a stack address on both sides matches). */
static inline void daObj_SetCurrentRoomNo_r5(fopAc_ac_c* a, void* gnd, u32 harness_r5) { gabi::call(0x02312968, a, gnd, harness_r5); }
/* 023120B4 daObj::get_path_spd(const cBgS_PolyInfo&, f32) -> const cXyz& */
static inline cXyz* daObj_get_path_spd(cBgS_PolyInfo* p, f32 v) { return gabi::call<cXyz*>(0x023120B4, p, v); }
/* 025D5418 fopAcM_setRoomLayer(actor, s8 roomNo) */
static inline void fopAcM_setRoomLayer(fopAc_ac_c* a, s32 room) { gabi::call(0x025D5418, a, room); }
/* HD fopAcM_seStart inline where GHS dropped even the &eyePos check */
static inline void fopAcM_seStart_raw(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* daPy_py_c (HD): the status word at +0x3C0 (getGrabUpStart 0x8000, getGrabUpEnd 0x20,
 * getRightFootOnGround 0x400); getGrabActorID() is the virtual at vtable (+0xB4) slot 0xBC */
static inline u32 daPy_status(fopAc_ac_c* p) { return gabi::load<u32>(gabi::ea(p) + 0x3C0); }
static inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(gabi::load<u32>(p->__vtbl + 0xBC), p); }
static inline void eff_drop_water(Act_c* a) { gabi::call(0x024CD95C, a); }
static inline void se_pickup(Act_c* a) { gabi::call(0x024CC798, a); }

/* 024CEB50; the M_attrSpine constants are immediates in HD */
static void mode_appear(Act_c* a) {
    WWHD_FUNC(0x024CEB50, void, a);
    if (prm_get_spec(a) == 4) {
        a->m802 = (s16)(a->m802 - 1);
        if (a->m802 == 0x46 /* M_attrSpine.m1C */) {
            fopAcM_seStart_raw(a, 0x69F4 /* JA_SE_OBJ_BOKKURI_RECOVER */, 0);
        }
        if (a->m802 == 0xD /* M_attrSpine.m1E */) {
            fopAcM_seStart_raw(a, 0x69F5 /* JA_SE_OBJ_BOKKURI_SWELL */, 0);
        }
        if (a->m802 >= 0xE /* M_attrSpine.m1A */) {
            cLib_addCalc(&a->scale.x, 0.5f, 0.1f, 0.02f, 0.001f);
            a->scale.y = a->scale.x;
            a->scale.z = a->scale.x;
        } else {
            f32 sx = a->scale.x;
            f32 sy = a->scale.y;
            f32 v = gabi::fnmsubs(sx - 1.0f, 0.7f, a->m808);
            v = gabi::fnmsubs(v, 0.55f, v);
            f32 w = gabi::fnmsubs(sy - 1.0f, 0.7f, a->m80C);
            w = gabi::fnmsubs(w, 0.55f, w);
            a->m808 = v;
            a->m80C = w;
            a->scale.x = sx + v;
            a->scale.y = sy + w;
            a->scale.z = sx + v;
        }
        if (a->m802 <= 0) {
            a->scale.set(1.0f, 1.0f, 1.0f);
            mode_wait_init(a);
        }
    } else {
        a->m802 = (s16)(a->m802 - 1);
        if (a->m802 <= 0) {
            mode_wait_init(a);
        } else {
            cLib_chaseF(&a->scale.x, 1.0f, 0.1f);
            a->scale.y = a->scale.x;
            a->scale.z = a->scale.x;
        }
        fopAcM_posMoveF(a, &a->mStts.m_cc_move);
    }
}
VERIFY(0x024CEB50, mode_appear);

/* 024CEF30 */
static void mode_wait(Act_c* a) {
    WWHD_FUNC(0x024CEF30, void, a);
    if (prm_get_stick(a)) {
        attn_onBit(a, fopAc_Attn_ACTION_CARRY_e);
    } else if (a->mAcch.ChkGroundHit()) {
        if (a->speedF > 0.1f) {
            a->speedF = a->speedF * 0.5f;
        } else {
            a->speedF = 0.0f;
        }
        a->gravity = data(a)->mGravity;
        fopAcM_posMoveF(a, &a->mStts.m_cc_move);
        attn_onBit(a, fopAc_Attn_ACTION_CARRY_e);
    } else {
        gabi::Local<be<f32>> sp10, sp0C, sp08;
        calc_drop_param(a, sp08, sp0C, sp10);
        a->gravity = *sp08;
        daObj_posMoveF_stream(a, &a->mStts.m_cc_move, cXyz_Zero, *sp0C, *sp10);
        attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    }
}
VERIFY(0x024CEF30, mode_wait);

/* 024CF094 */
/* harness: r4..r8 are taken only to reproduce the original's leftover r8 (see daObj_posMoveF_grade) */
static void mode_walk(Act_c* a, u32 r4, u32 r5, u32 r6, u32 r7, u32 harness_r8) {
    WWHD_FUNC(0x024CF094, void, a, r4, r5, r6, r7, harness_r8);
    gabi::Local<be<f32>> sp10, sp0C, sp08;
    calc_drop_param(a, sp08, sp0C, sp10);
    gabi::Local<cXyz> sp20;
    sp20->x = a->mStts.m_cc_move.x;
    a->gravity = *sp08;
    sp20->y = 0.0f;
    sp20->z = a->mStts.m_cc_move.z;
    if (PSVECSquareMag(sp20) > 0.010000001f) {
        a->speedF = a->speedF * 0.9f;
    }
    void* pNorm = dBgS_GetTriPla(dComIfG_Bgsp(), Acch_gnd(&a->mAcch));
    PSVECAdd(&a->m6F0, &a->m814, &a->m6F0);
    if (pNorm != nullptr) {
        Act_c::Data_c* d = data(a);
        s16 deg = (s16)gabi::ftoi(((f32)(s16)d->m56 - 0.5f) * 182.04445f); /* cM_deg2s */
        gabi::Local<u32> r8tmp; /* harness: the original's r8 is a stack address here */
        daObj_posMoveF_grade(a, &a->mStts.m_cc_move, &a->m7F4, *sp0C, *sp10, pNorm, d->m5C, cM_scos((u16)deg), &a->m6F0,
                             gabi::ea(r8tmp.get()));
    } else {
        daObj_posMoveF_grade(a, &a->mStts.m_cc_move, &a->m7F4, *sp0C, *sp10, nullptr, 0.0f, 0.0f, &a->m6F0, harness_r8);
    }
    if (a->mAcch.ChkGroundHit()) {
        attn_onBit(a, fopAc_Attn_ACTION_CARRY_e);
        gabi::Local<cXyz> sp1C;
        sp1C->x = a->speed.x;
        sp1C->y = 0.0f;
        sp1C->z = a->speed.z;
        if (PSVECSquareMag(sp1C) < 0.1f) {
            mode_wait_init(a);
        }
    } else {
        attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    }
}
VERIFY(0x024CF094, mode_walk);

/* 024CF5AC */
static void mode_drop_init(Act_c* a) {
    WWHD_FUNC(0x024CF5AC, void, a);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    Act_c::Data_c* d = data(a);
    f32 fVar1 = gabi::fmadds(player->speedF, d->m1C, d->m18);
    a->mCyl.OnAtSPrmBit(1);
    a->mCyl.OnTgSPrmBit(1);
    a->mCyl.OnCoSPrmBit(1);
    a->mAcch.m_flags = (a->mAcch.m_flags & ~0x40Eu) | dBgS_Acch::LINE_CHECK;
    a->m67F = 0;
    a->mStts.Init(d->m10, 0xFF, a);
    attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    if (a->m810 == 0) {
        a->speed.y = data(a)->m14;
    }
    a->speedF = fVar1;
    a->m810 = 0;
    a->m678 = 5;
    a->gravity = data(a)->mGravity;
}
VERIFY(0x024CF5AC, mode_drop_init);

/* 024CF6C8 */
static void mode_drop(Act_c* a) {
    WWHD_FUNC(0x024CF6C8, void, a);
    daObj_SetCurrentRoomNo(a, acch_gnd(a));
    gabi::Local<cXyz> sp14;
    sp14->x = a->mStts.m_cc_move.x;
    sp14->y = 0.0f;
    sp14->z = a->mStts.m_cc_move.z;
    if (PSVECSquareMag(sp14) > 0.010000001f) {
        a->speedF = a->speedF * 0.9f;
    }
    gabi::Local<be<f32>> sp10, sp08, sp0C;
    calc_drop_param(a, sp10, sp08, sp0C);
    a->gravity = *sp10;
    daObj_posMoveF_stream(a, &a->mStts.m_cc_move, &a->m7F4, *sp08, *sp0C);
}
VERIFY(0x024CF6C8, mode_drop);

/* 024CF2BC */
static void se_pickup_carry(Act_c* a) {
    WWHD_FUNC(0x024CF2BC, void, a);
    if (a->mType == 7) {
        if (a->m811 != 0 && (daPy_status(dComIfGp_getPlayer(0)) & 0x8000) /* getGrabUpStart */) {
            a->m811 = 0;
            se_pickup(a);
        }
    }
}
VERIFY(0x024CF2BC, se_pickup_carry);

/* 024CF320 */
static void spec_mode_carry(Act_c* a, bool arg1) {
    WWHD_FUNC(0x024CF320, void, a, arg1);
    s32 spec = prm_get_spec(a);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &a->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0 && M_spec_act()[i] != nullptr) {
                f32 fVar1 = M_data_spec_boko(i)->m04;
                f32 m10;
                if (arg1) {
                    f32 r = cM_rndF(3.0f);
                    m10 = ptr->m10 + r;
                    ptr->m18 = 2;
                    ptr->m10 = m10;
                    ptr->m14 = r;
                } else {
                    f32 v = ptr->m14 - 0.8f;
                    m10 = ptr->m10 + v;
                    ptr->m10 = m10;
                    ptr->m14 = v;
                }
                if (m10 < fVar1) {
                    ptr->m18 = (s8)(ptr->m18 - 1);
                    if (ptr->m18 <= 0) {
                        ptr->m10 = fVar1;
                        ptr->m14 = 0.0f;
                    } else {
                        ptr->m10 = gabi::fmadds(ptr->m10 - fVar1, -0.5f, fVar1);
                        ptr->m14 = ptr->m14 * -0.5f;
                    }
                }
            }
        }
    }
}
VERIFY(0x024CF320, spec_mode_carry);

/* 024CF77C */
static void spec_mode_put_init(Act_c* a) {
    WWHD_FUNC(0x024CF77C, void, a);
    s32 spec = prm_get_spec(a);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &a->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0 && M_spec_act()[i] != nullptr) {
                fopAcM_setRoomLayer(M_spec_act()[i], fopAcM_GetRoomNo(a));
                ptr->m0C = 0.0f;
                ptr->m10 = M_data_spec_boko(i)->m04;
                ptr->m14 = 0.0f;
            }
        }
    }
}
VERIFY(0x024CF77C, spec_mode_put_init);

/* 024CF844 */
/* harness: r4/r5 are taken only to reproduce the original's leftover r5 (see daObj_SetCurrentRoomNo_r5) */
static void mode_carry(Act_c* a, u32 r4, u32 harness_r5) {
    WWHD_FUNC(0x024CF844, void, a, r4, harness_r5);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    const bool tmp = a->m685 != 0;
    if (!tmp) {
        harness_r5 = 8;
        if (prm_get_spec(a) == 6) {
            a->m686 = 1;
            a->m685 = 1;
        } else if (daPy_getGrabActorID(player) == gabi::load<u32>(gabi::ea(a) + 4) /* fopAcM_GetID */) {
            harness_r5 = daPy_status(player);
            if (harness_r5 & 0x8000 /* getGrabUpStart */) {
                a->m686 = 1;
                a->m685 = 1;
            }
        } else {
            a->m685 = 1;
            a->m686 = 0;
        }
    }
    if (a->m685 != 0 && !tmp) {
        init_rot_clean(a);
        a->shape_angle.z = 0;
        a->shape_angle.x = 0;
    }
    u32 st = daPy_status(player);
    if (!(st & 0x20))
        harness_r5 = st & 0x400;
    bool bVar2 = (st & 0x20) /* getGrabUpEnd */ || (st & 0x400) /* getRightFootOnGround */;
    if (bVar2 && a->mType == 2) {
        harness_r5 = 0;
        eff_drop_water(a);
        fopAcM_seStart_raw(a, 0x283F /* JA_SE_LK_CONVEY_WPOT */, 0);
    }
    se_pickup_carry(a);
    if (a->m810 == 0) {
        a->speed.y = 0.0f;
    }
    if (a->actor_status & 0x102000) { /* fopAcM_checkCarryNow || fopAcM_checkHookCarryNow */
        spec_mode_carry(a, bVar2);
    } else {
        a->m6FC = a->current.pos.y;
        if (a->speedF > 0.0f && a->m684 == 0) {
            init_rot_throw(a);
            mode_drop_init(a);
            mode_drop(a);
        } else {
            daObj_SetCurrentRoomNo_r5(a, acch_gnd(a), harness_r5);
            spec_mode_put_init(a);
            a->m683 = 2;
            mode_wait_init(a);
        }
    }
}
VERIFY(0x024CF844, mode_carry);

/* 024CFD3C */
static void mode_afl(Act_c* a) {
    WWHD_FUNC(0x024CFD3C, void, a);
    daObj_SetCurrentRoomNo(a, acch_gnd(a));
    cXyz* pStreamSpd = daObj_get_path_spd(Acch_gnd(&a->mAcch), 30.0f);
    gabi::Local<cXyz> sp20;
    cXyz_ml(&a->m7F4, sp20, data(a)->m20);
    gabi::Local<be<f32>> sp08, sp0C, sp10;
    calc_afl_param(a, sp08, sp0C, sp10);
    s32 t = gabi::ftoi(cM_rndF(400.0f));
    a->m804 = (s16)(a->m804 + t + 0x514);
    a->gravity = gabi::fmadds(cM_ssin(a->m804), 0.3f, *sp08);
    water_tention(a);
    daObj_posMoveF_grade(a, &a->mStts.m_cc_move, pStreamSpd, *sp0C, *sp10, nullptr, 0.0f, 0.0f, sp20, 0x10040000 /* harness */);
    s32 t2 = gabi::ftoi(cM_rndF(400.0f));
    a->m806 = (s16)(a->m806 + t2 + 0x514);
}
VERIFY(0x024CFD3C, mode_afl);

/* ---- part C, batch 3 ---- */
static inline void mode_carry_init(Act_c* a) { gabi::call(0x024CD4C8, a); }
static inline void crr_pos(Act_c* a, cXyz* p) { gabi::call(0x024CD7AC, a, p); }
static inline bool damage_bg_proc_directly(Act_c* a) { return gabi::call<bool>(0x024CDAB4, a); }
static inline void bound(Act_c* a, f32 v) { gabi::call(0x024CDF3C, a, v); }
static inline bool damage_cc_proc(Act_c* a) { return gabi::call<bool>(0x024CC7EC, a); }
static inline bool damage_bg_proc(Act_c* a) { return gabi::call<bool>(0x024CD1E8, a); }
static inline bool damage_kill_proc(Act_c* a) { return gabi::call<bool>(0x024CD3CC, a); }
static inline void set_mtx(Act_c* a) { gabi::call(0x024CB280, a); }
static inline void spec_set_actor(Act_c* a) { gabi::call(0x024CADD8, a); }
static inline void spec_carry_spec(Act_c* a) { gabi::call(0x024CAE78, a); }
static inline void spec_clr_actor(Act_c* a) { gabi::call(0x024CAF20, a); }
static inline void cull_set_draw(Act_c* a) { gabi::call(0x024CA5F0, a); }
static inline void cull_set_move(Act_c* a) { gabi::call(0x024CBD48, a); }

/* PTMF call with one argument (see ptmf_call in bindings.h) */
static inline void ptmf_call1(u32 entry, void* self, const void* arg) {
    s16 delta = gabi::load<s16>(entry);
    s16 idx = gabi::load<s16>(entry + 2);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0) {
        gabi::call_ptr(gabi::load<u32>(entry + 4), p, arg);
    } else {
        u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
        gabi::call_ptr(gabi::load<u32>(vt + idx * 8 + 4), p, arg);
    }
}
/* 024EEEB8 dBgS::GetPolyColor(const cBgS_PolyInfo&) */
static inline u8 dBgS_GetPolyColor(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<u8>(0x024EEEB8, bgs, p); }
/* 025165A4 dCcD_Cyl::MoveCAtTg(cXyz&), 02516C14 dCcMassS_Mng::Set(obj, u8 prio) (play+0x4EF8) */
static inline void dCcD_Cyl_MoveCAtTg(dCcD_Cyl* c, cXyz* p) { gabi::call(0x025165A4, c, p); }
static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio); }
/* 025D6CE8 fopAcM_cullingCheck; HD tests actor_condition bit 4 inline first */
static inline BOOL fopAcM_cullingCheck(fopAc_ac_c* a) { return gabi::call<BOOL>(0x025D6CE8, a); }
/* tevStr: mRoomNo +0xB9, mEnvrIdxOverride +0xBA, mColorC0 (s16 x4) +0x90, mColorK0 +0x98 */
static inline GXColor* tev_colorK0(fopAc_ac_c* a) { return gabi::at<GXColor>(gabi::ea(&a->tevStr) + 0x98); }
/* 025A3BDC dPa_J3DmodelEmitter_c::dPa_J3DmodelEmitter_c (allocates when this == NULL; HD: an extra
 * argument, 0 here, before the tevstr) */
static inline void* new_dPa_J3DmodelEmitter(JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i) {
    return gabi::call<void*>(0x025A3BDC, (u32)0, e, d, hd, tev, btp, n, i);
}
/* 0200FE78 cLs_Addition(list, node): dComIfGp_particle_addModelEmitter (the list at particle+0x130) */
static inline void dComIfGp_particle_addModelEmitter(void* e) {
    u32 pa = gabi::ea(dComIfGp_getParticle());
    gabi::call(0x0200FE78, gabi::load<u32>(pa + 0x130), e);
}
/* daObjEff::Act_c::make_*_smoke(pos): fopAcM_create(PROC_Obj_Eff 0x1D3, type, pos, -1) */
static inline void make_obj_smoke(u32 type, cXyz* pos) { fopAcM_create(0x1D3, type, pos, -1, nullptr, nullptr, -1, 0); }

/* 024CE204; the moment_proc PTMF table is at 0x10040DA8 */
static void moment_proc_call(Act_c* a) {
    WWHD_FUNC(0x024CE204, void, a);
    a->m6F0.x = cXyz_Zero->x;
    a->m6F0.y = cXyz_Zero->y;
    a->m6F0.z = cXyz_Zero->z;
    if (a->m678 == 3) {
        void* pla = nullptr;
        if (a->mAcch.ChkGroundHit()) {
            pla = dBgS_GetTriPla(dComIfG_Bgsp(), Acch_gnd(&a->mAcch));
        }
        if (pla != nullptr) {
            ptmf_call1(0x10040DA8 + 8 * a->mType, a, pla /* GetNP() */);
        } else {
            cLib_addCalc0(&a->m6EC, 0.4f, (f32)(u8)data(a)->mAcchRoofHeight * 0.125f);
        }
    }
}
VERIFY(0x024CE204, moment_proc_call);

/* 024CE33C spec_set_room (matcher: unnamed) */
static void spec_set_room(Act_c* a) {
    WWHD_FUNC(0x024CE33C, void, a);
    s32 spec = prm_get_spec(a);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &a->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = M_spec_act()[i];
                if (boko != nullptr) {
                    s8 room = fopAcM_GetRoomNo(a);
                    boko->tevStr.mRoomNo = room;
                    gabi::store<u8>(gabi::ea(boko) + 0x1CA, gabi::load<u8>(gabi::ea(a) + 0x1CA)); /* mEnvrIdxOverride */
                    boko->current.roomNo = room; /* fopAcM_SetRoomNo */
                }
            }
        }
    }
}
VERIFY(0x024CE33C, spec_set_room);

/* 024CE3D0 */
static void eff_kutani_set(Act_c* a) {
    WWHD_FUNC(0x024CE3D0, void, a);
    if (a->m798 != 0) {
        a->m798 = (u8)(a->m798 - 1);
        if (a->m798 == 0) {
            dComIfGp_particle_set(0x814B /* ID_AK_SN_KUTANIVESSEL */, &a->current.pos, nullptr, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&a->m784);
        }
    }
}
VERIFY(0x024CE3D0, eff_kutani_set);

/* 024CE454; mode_proc (0x10040E30) and set_tensor_proc (0x10040E70) are PTMF tables in .data */
static bool mode_proc_call(Act_c* a) {
    WWHD_FUNC(0x024CE454, bool, a);
    if ((a->actor_status & 0x102000) && a->m678 != 4) { /* fopAcM_checkCarryNow || fopAcM_checkHookCarryNow */
        mode_carry_init(a);
    }
    s32 mode = a->m678;
    gabi::Local<cXyz> sp08;
    sp08->x = a->current.pos.x;
    sp08->y = a->current.pos.y;
    sp08->z = a->current.pos.z;
    ptmf_call(0x10040E30 + 8 * mode, a);
    bool iVar2 = prm_get_moveBg(a);
    f32 fVar7 = a->speed.y;
    crr_pos(a, sp08);
    if (damage_bg_proc_directly(a)) {
        return false;
    }
    if (!iVar2) {
        bound(a, fVar7);
    }
    moment_proc_call(a);
    ptmf_call(0x10040E70 + 8 * a->m678, a);
    if (a->m678 != 4) {
        a->tevStr.mRoomNo = fopAcM_GetRoomNo(a);
        gabi::store<u8>(gabi::ea(a) + 0x1CA, dBgS_GetPolyColor(dComIfG_Bgsp(), Acch_gnd(&a->mAcch)));
    }
    PSVECScale(&a->m7F4, &a->m7F4, 0.95f);
    if (PSVECSquareMag(&a->m7F4) < 0.1f) {
        a->m7F4.set(0.0f, 0.0f, 0.0f);
    }
    PSVECScale(&a->m814, &a->m814, 0.6f);
    if (PSVECSquareMag(&a->m814) < 0.1f) {
        a->m814.set(0.0f, 0.0f, 0.0f);
    }
    spec_set_room(a);
    if (a->mType == 14) {
        eff_kutani_set(a);
    }
    return true;
}
VERIFY(0x024CE454, mode_proc_call);

/* 024CE71C */
static bool _execute(Act_c* a) {
    WWHD_FUNC(0x024CE71C, bool, a);
    cull_set_move(a);
    bool bVar4;
    if (prm_get_spec(a) == 5) {
        bVar4 = !is_switch(a);
    } else {
        bVar4 = false;
    }
    if (!bVar4) {
        if (a->m682 != 0 || a->m678 != 2 || !a->mAcch.ChkGroundHit() || a->mAcch.ChkGroundLanding() || prm_get_cull(a) == 0 ||
            !(a->actor_condition & 4) || !fopAcM_cullingCheck(a) || a->m800 != 0 || prm_get_moveBg(a)) {
            a->m682 = 0;
            BOOL bVar6 = TRUE;
            spec_set_actor(a);
            if (!damage_cc_proc(a)) {
                if (!damage_bg_proc(a) && !damage_kill_proc(a)) {
                    if (a->m683 != 0) {
                        a->m683 = (u8)(a->m683 - 1);
                    }
                    if (mode_proc_call(a)) {
                        bVar6 = FALSE;
                        set_mtx(a);
                        spec_carry_spec(a);
                        if (a->m678 != 0) {
                            a->mStts.mRoomId = a->current.roomNo;
                            dCcD_Cyl_MoveCAtTg(&a->mCyl, &a->current.pos);
                            dComIfG_Ccsp_Set(&a->mCyl);
                        }
                        if (a->m678 == 5 || a->m678 == 6 || a->m678 == 3 || a->m683 != 0) {
                            dComIfG_Ccsp_SetMass(&a->mCyl, 3);
                        }
                        f32 x = a->current.pos.x;
                        f32 y = a->current.pos.y + data(a)->mAttnY;
                        f32 z = a->current.pos.z;
                        u32 b = gabi::ea(a);
                        gabi::store<f32>(b + 0x390, x); /* attention_info.position */
                        gabi::store<f32>(b + 0x394, y);
                        gabi::store<f32>(b + 0x398, z);
                        a->eyePos.set(x, y, z);
                    }
                }
            }
            spec_clr_actor(a);
            a->model = (a->m678 == 4 && a->m686) ? (u32)a->mpModel.v : 0u; /* fopAcM_SetModel */
            if (bVar6) {
                fopAcM_delete(a);
            }
        }
    }
    cull_set_draw(a);
    return true;
}
VERIFY(0x024CE71C, _execute);

/* 024CE994; HD: no simple shadow */
static bool _draw(Act_c* a) {
    WWHD_FUNC(0x024CE994, bool, a);
    if (a->m678 == 0 || prm_get_moveBg(a)) {
        return true;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a->current.pos, &a->tevStr);
    setLightTevColorType(dKy_getEnvlight(), a->mpModel, &a->tevStr);
    if (a->mpBrk != nullptr) {
        u32 anm = gabi::load<u32>(gabi::ea((mDoExt_brkAnm*)a->mpBrk) + 0x10); /* getBrkAnm() */
        s32 maxFrame = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); /* getFrameMax() */
        f32 maxf = (f32)maxFrame;
        f32 f = maxf * (1.0f - (f32)a->m800 / 1200.0f /* M_attrSpine.m30 */);
        if (a->m800 == 0 || f < 0.0f) {
            f = 0.0f;
        } else if (!(f < maxf)) {
            f = maxf - 0.001f;
        }
        mDoExt_brkAnm_entry(a->mpBrk, J3DModel_getModelData(a->mpModel), f);
    }
    mDoExt_modelUpdateDL(a->mpModel);
    if (a->mpBrk != nullptr) {
        gabi::store<u32>(gabi::ea(J3DModel_getModelData(a->mpModel)) + 0x48, 0); /* mDoExt_brkAnm::remove (HD inline) */
    }
    return true;
}
VERIFY(0x024CE994, _draw);

/* 024D0404 */
static void eff_break_barrel(Act_c* a) {
    WWHD_FUNC(0x024D0404, void, a);
    gabi::Local<cXyz> sp18;
    sp18->x = a->current.pos.x;
    sp18->y = gabi::fmadds((f32)(u8)data(a)->mAcchRoofHeight, 0.5f, a->current.pos.y);
    sp18->z = a->current.pos.z;
    JPABaseEmitter* e = dComIfGp_particle_set(0x3E5 /* ID_IT_JN_TR_HAHEN_A */, sp18, nullptr, nullptr, 0xFF, nullptr, -1,
                                              tev_colorK0(a), tev_colorK0(a));
    if (e != nullptr) {
        /* function-local static em_scl (guard 0x1046EA80) */
        be<u32>& guard = *gabi::at<be<u32>>(0x1046EA80);
        cXyz* em_scl = gabi::at<cXyz>(0x1046EA84);
        if (guard == 0) {
            guard = 1;
            em_scl->set(1.0f, 0.8f, 1.0f);
        }
        f32 sx = em_scl->x, sy = em_scl->y, sz = em_scl->z;
        gabi::store<f32>(gabi::ea(e) + 8, sx); /* setEmitterScale */
        gabi::store<f32>(gabi::ea(e) + 0xC, sy);
        gabi::store<f32>(gabi::ea(e) + 0x10, sz);
    }
    make_obj_smoke(0 /* barrel */, sp18);
}
VERIFY(0x024D0404, eff_break_barrel);

/* 024D0550 */
static void eff_break_tsubo(Act_c* a) {
    WWHD_FUNC(0x024D0550, void, a);
    u16 uVar1 = (u16)data(a)->m80;
    J3DModelData* mdl = (J3DModelData*)dComIfG_getObjectRes(STR(0x10040EE8) /* "Always" */, 0x31, TSUBO_SAFESTRING_VTBL);
    void* btp = dComIfG_getObjectRes(STR(0x10040EE8), 0x67, TSUBO_SAFESTRING_VTBL);
    JPABaseEmitter* e = dComIfGp_particle_set(0x17 /* ID_AK_JN_M_TUBOHAHEN */, &a->current.pos, nullptr, &data(a)->m74);
    if (e != nullptr) {
        void* me = new_dPa_J3DmodelEmitter(e, mdl, 0, &a->tevStr, btp, uVar1, 0);
        if (me != nullptr) {
            dComIfGp_particle_addModelEmitter(me);
        }
    }
    gabi::Local<GXColor> color;
    u32 t = gabi::ea(&a->tevStr);
    color->r = (u8)gabi::load<s16>(t + 0x90); /* mColorC0 */
    color->g = (u8)gabi::load<s16>(t + 0x92);
    color->b = (u8)gabi::load<s16>(t + 0x94);
    color->a = (u8)gabi::load<s16>(t + 0x96);
    Act_c::Data_c* d = data(a);
    u32 cb = 0x1047B24C + 8 * d->m80; /* dPa_control_c::getTsuboSelectTexEcallBack(m80) */
    dComIfGp_particle_set(0x18 /* ID_AK_JN_TUBOKONAGONA */, &a->current.pos, nullptr, &d->m74, 0xFF, gabi::at<dPa_levelEcallBack>(cb), -1,
                          tev_colorK0(a), color); /* HD: the colours swapped (GameCube: prm = color, env = K0) */
}
VERIFY(0x024D0550, eff_break_tsubo);

/* 024D06F4 */
static void eff_break_stool(Act_c* a) {
    WWHD_FUNC(0x024D06F4, void, a);
    gabi::Local<cXyz> sp18;
    sp18->x = a->current.pos.x;
    sp18->y = a->current.pos.y + 35.0f;
    sp18->z = a->current.pos.z;
    /* function-local static particle_scl (guard 0x1046EA90) */
    be<u32>& guard = *gabi::at<be<u32>>(0x1046EA90);
    cXyz* particle_scl = gabi::at<cXyz>(0x1046EA70);
    if (guard == 0) {
        guard = 1;
        particle_scl->set(1.0f, 0.8f, 1.0f);
    }
    JPABaseEmitter* e = dComIfGp_particle_set(0x3E7 /* ID_IT_JN_TR_HAHEN_C */, sp18, nullptr, nullptr, 0xFF, nullptr, -1,
                                              tev_colorK0(a), tev_colorK0(a), particle_scl);
    if (e != nullptr) {
        gabi::store<f32>(gabi::ea(e) + 0x70, 25.0f); /* setDirectionalSpeed */
    }
    make_obj_smoke(1 /* stool */, sp18);
}
VERIFY(0x024D06F4, eff_break_stool);

/* 024D07FC */
static void eff_break_skull(Act_c* a) {
    WWHD_FUNC(0x024D07FC, void, a);
    gabi::Local<cXyz> sp18;
    sp18->x = a->current.pos.x;
    sp18->y = a->current.pos.y + 20.0f;
    sp18->z = a->current.pos.z;
    dComIfGp_particle_set(0x3E8 /* ID_IT_JN_DOKURO00 */, sp18, nullptr, nullptr, 0xFF, nullptr, -1, tev_colorK0(a), tev_colorK0(a));
    make_obj_smoke(2 /* skull */, sp18);
}
VERIFY(0x024D07FC, eff_break_skull);

/* 024D08B8 */
static void eff_break_pinecone(Act_c* a) {
    WWHD_FUNC(0x024D08B8, void, a);
    gabi::Local<cXyz> sp18;
    sp18->x = a->current.pos.x;
    sp18->y = gabi::fmadds((f32)(u8)data(a)->mAcchRoofHeight, 0.5f, a->current.pos.y);
    sp18->z = a->current.pos.z;
    dComIfGp_particle_set(0x816A /* ID_IT_SN_BOKKURI_MATSU00 */, sp18, nullptr, nullptr, 0xFF, nullptr, -1, tev_colorK0(a), tev_colorK0(a));
    make_obj_smoke(4 /* pinecone */, sp18);
}
VERIFY(0x024D08B8, eff_break_pinecone);

/* 024D08B4 eff_break_pail (-> eff_break_stool) */
static void eff_break_pail(Act_c* a) {
    WWHD_FUNC(0x024D08B4, void, a);
    eff_break_stool(a);
}
VERIFY(0x024D08B4, eff_break_pail);

/* 024D0B9C: per-TU inline, a u8 colour to four floats / 255 (e.g. JUtility::TColor -> GXColorF) */
static void color_to_f4(be<f32>* out, const GXColor* c) {
    WWHD_FUNC(0x024D0B9C, void, out, c);
    f32 r = (f32)(u8)c->r / 255.0f;
    f32 g = (f32)(u8)c->g / 255.0f;
    f32 b = (f32)(u8)c->b / 255.0f;
    f32 al = (f32)(u8)c->a / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = al;
}
VERIFY(0x024D0B9C, color_to_f4);
