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
#include "d/actor/d_a_tsubo_mode.h"

using daTsuboMode::Act_c;
using namespace daTsuboMode;
using daTsubo::cSAngle_subeq;
using daTsubo::cSAngle_addeq;
using daTsubo::cSAngle_Val;
using daTsubo::cSAngle_muleq;
using daTsubo::Acch_gnd;
using daTsubo::attn_onBit;
using daTsubo::attn_offBit;
using daTsubo::spec_is_boko;
using daTsubo::M_spec_act;
using daTsubo::M_data_spec_boko;

static ModeTable mode_proc;
static ModeTable set_tensor_proc;
static MomentTable moment_proc;
static TextureCallbacks tsuboCallbacks;
static cXyz BaseX;
static u32 barrelScaleInitialized;
static cXyz barrelEmitterScale;
static u32 stoolScaleInitialized;
static cXyz stoolParticleScale;

/* 02312968 daObj::SetCurrentRoomNo(actor, dBgS_GndChk*) */

static inline void* acch_gnd(Act_c* a) { return &a->mAcch.m_gnd; }

/* the common tail of the set_tensor_* functions: the axis BaseX rotated by m692/m68A/m688 */
static inline void tensor_axis(Act_c* a) {
    mDoMtx_YrotS(mDoMtx_stack_c::get(), a->m692);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), a->m68A);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), a->m688);
    gabi::Local<cXyz> sp;
    PSMTXMultVecSR(mDoMtx_stack_c::get(), &BaseX, sp);
    tsuboMode_set_tensor(a, sp);
}
// HD: damp the angle with the fused multiply-add used by GHS.
static inline void tensor_damp(be<s16>* ang, Act_c* a, be<f32> Act_c::Data_c::*first, be<f32> Act_c::Data_c::*second) {
    f32 r = tsuboMode_cSAngle_Radian(ang);
    Act_c::Data_c* d = data(a);
    f32 k2 = d->*second;
    f32 k1 = d->*first;
    cSAngle_subeq(ang, tsuboMode_cM_rad2s(gabi::fmadds(r, k1, (std::fabs(r) * r) * k2)));
}

/* the water/lava level of calc_drop_param / calc_afl_param: false when neither applies */
static inline bool drop_level(Act_c* a, f32* d3) {
    bool hasLava = a->m4FC != -1000000000.0f;
    bool belowWater = a->m4F8 > a->current.pos.y;
    if (!hasLava && !belowWater) {
        return false;
    }
    f32 lv;
    if (hasLava && belowWater) {
        lv = (a->m4FC - a->m4F8 >= 0.0f) ? (f32)a->m4FC : (f32)a->m4F8; /* GHS fsel preserves the unordered comparison. */
    } else if (hasLava) {
        lv = a->m4FC;
    } else {
        lv = a->m4F8;
    }
    *d3 = a->current.pos.y - lv;
    return true;
}

/* 0200ECD4 tsuboMode_cLib_addCalc(f32* v, f32 target, f32 scale, f32 maxStep, f32 minStep) */

/* 023121C4 daObj::posMoveF_grade(actor, ccMove, stream, f32 k1, f32 k2, const cXyz* gradeNormal,
 * f32 grade, f32 noGradeCos, const cXyz* moment) */

/* HARNESS WORKAROUND: funcdb parses the GameCube symbols of the daObj:: namespace functions
 * (posMoveF_grade__5daObjFP10fopAc_ac_c..., SetCurrentRoomNo__5daObjF...) as member functions, so
 * the harness compares one integer register more than they take (posMoveF_grade r8,
 * posMoveF_stream r6, SetCurrentRoomNo r5). The candidate passes what the original leaves there;
 * the unit file declares those registers output-only (a stack address on both sides matches). */

// HD: the sound inline always passes eyePos.
static inline void tsuboMode_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline u32 daPy_status(fopAc_ac_c* p) { return reinterpret_cast<Player*>(p)->mStatus; }
static inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return reinterpret_cast<Player*>(p)->getGrabActorID(); }

/* harness: r4..r8 are taken only to reproduce the original's leftover r8 (see daObj_posMoveF_grade) */

/* harness: r4/r5 are taken only to reproduce the original's leftover r5 (see daObj_SetCurrentRoomNo_r5) */

static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) { tsuboMode_setMass(&reinterpret_cast<MassPlay*>(dComIfGp_get())->mMassManager, obj, prio); }

static inline GXColor* tev_colorK0(fopAc_ac_c* a) { return &a->tevStr.mColorK0; }
/* 025A3BDC dPa_J3DmodelEmitter_c::dPa_J3DmodelEmitter_c (allocates when this == NULL; HD: an extra
 * argument, 0 here, before the tevstr) */
// HD: the emitter factory receives a null object and an extra argument before tevstr.
static inline void* new_dPa_J3DmodelEmitter(JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i) {
    return tsuboMode_modelEmitter(0, e, d, hd, tev, btp, n, i);
}

static inline void dComIfGp_particle_addModelEmitter(void* e) {
    auto* particle = reinterpret_cast<ParticleLists*>(dComIfGp_getParticle());
    tsuboMode_addModelEmitter(particle->mModelEmitters, e);
}
static inline void make_obj_smoke(u32 type, cXyz* pos) { fopAcM_create(PROC_Obj_Eff, type, pos, -1, nullptr, nullptr, -1, 0); }

/* 024CF320 */
void daTsuboMode::Act_c::spec_mode_carry(bool arg1) {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
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

/* 024CF77C */
void daTsuboMode::Act_c::spec_mode_put_init() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0 && M_spec_act()[i] != nullptr) {
                tsuboMode_fopAcM_setRoomLayer(M_spec_act()[i], fopAcM_GetRoomNo(this));
                ptr->m0C = 0.0f;
                ptr->m10 = M_data_spec_boko(i)->m04;
                ptr->m14 = 0.0f;
            }
        }
    }
}

/* 024CE33C */
void daTsuboMode::Act_c::spec_set_room() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = M_spec_act()[i];
                if (boko != nullptr) {
                    s8 room = fopAcM_GetRoomNo(this);
                    boko->tevStr.mRoomNo = room;
                    boko->tevStr.mEnvrIdxOverride = tevStr.mEnvrIdxOverride; /* mEnvrIdxOverride */
                    boko->current.roomNo = room; /* fopAcM_SetRoomNo */
                }
            }
        }
    }
}

/* 024CEAFC */
void daTsuboMode::Act_c::mode_hide() {
    if (is_switch(this)) {
        tsuboMode_mode_appear_init(this);
    }
}

/* 024CEB50 */
void daTsuboMode::Act_c::mode_appear() {
    if (prm_get_spec(this) == 4) {
        m802 = (s16)(m802 - 1);
        if (m802 == 0x46 /* M_attrSpine.m1C */) {
            tsuboMode_seStart(this, JA_SE_OBJ_BOKKURI_RECOVER, 0);
        }
        if (m802 == 0xD /* M_attrSpine.m1E */) {
            tsuboMode_seStart(this, JA_SE_OBJ_BOKKURI_SWELL, 0);
        }
        if (m802 >= 0xE /* M_attrSpine.m1A */) {
            tsuboMode_cLib_addCalc(&scale.x, 0.5f, 0.1f, 0.02f, 0.001f);
            scale.y = scale.x;
            scale.z = scale.x;
        } else {
            f32 sx = scale.x;
            f32 sy = scale.y;
            f32 v = gabi::fnmsubs(sx - 1.0f, 0.7f, m808);
            v = gabi::fnmsubs(v, 0.55f, v);
            f32 w = gabi::fnmsubs(sy - 1.0f, 0.7f, m80C);
            w = gabi::fnmsubs(w, 0.55f, w);
            m808 = v;
            m80C = w;
            scale.x = sx + v;
            scale.y = sy + w;
            scale.z = sx + v;
        }
        if (m802 <= 0) {
            scale.set(1.0f, 1.0f, 1.0f);
            tsuboMode_mode_wait_init(this);
        }
    } else {
        m802 = (s16)(m802 - 1);
        if (m802 <= 0) {
            tsuboMode_mode_wait_init(this);
        } else {
            cLib_chaseF(&scale.x, 1.0f, 0.1f);
            scale.y = scale.x;
            scale.z = scale.x;
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
}

/* 024CEF30 */
void daTsuboMode::Act_c::mode_wait() {
    if (prm_get_stick(this)) {
        attn_onBit(this, fopAc_Attn_ACTION_CARRY_e);
    } else if (mAcch.ChkGroundHit()) {
        if (speedF > 0.1f) {
            speedF = speedF * 0.5f;
        } else {
            speedF = 0.0f;
        }
        gravity = data(this)->mGravity;
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        attn_onBit(this, fopAc_Attn_ACTION_CARRY_e);
    } else {
        gabi::Local<be<f32>> sp10, sp0C, sp08;
        calc_drop_param(sp08, sp0C, sp10);
        gravity = *sp08;
        daObj_posMoveF_stream(this, &mStts.m_cc_move, cXyz_Zero, *sp0C, *sp10);
        attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    }
}

/* 024CF094 */
void daTsuboMode::Act_c::mode_walk(u32 r4, u32 r5, u32 r6, u32 r7, u32 harness_r8) {
    gabi::Local<be<f32>> sp10, sp0C, sp08;
    calc_drop_param(sp08, sp0C, sp10);
    cXyz sp20;
    sp20.x = mStts.m_cc_move.x;
    gravity = *sp08;
    sp20.y = 0.0f;
    sp20.z = mStts.m_cc_move.z;
    if (PSVECSquareMag(&sp20) > 0.010000001f) {
        speedF = speedF * 0.9f;
    }
    void* pNorm = dBgS_GetTriPla(dComIfG_Bgsp(), Acch_gnd(&mAcch));
    PSVECAdd(&m6F0, &m814, &m6F0);
    if (pNorm != nullptr) {
        Act_c::Data_c* d = data(this);
        s16 deg = (s16)gabi::ftoi(((f32)(s16)d->m56 - 0.5f) * 182.04445f); /* cM_deg2s */
        gabi::Local<u32> r8tmp; /* harness: the original's r8 is this stack address here */
        tsuboMode_daObj_posMoveF_grade(this, &mStts.m_cc_move, &m7F4, *sp0C, *sp10, pNorm, d->m5C, cM_scos((u16)deg), &m6F0,
                             gabi::ea(r8tmp.get()));
    } else {
        tsuboMode_daObj_posMoveF_grade(this, &mStts.m_cc_move, &m7F4, *sp0C, *sp10, nullptr, 0.0f, 0.0f, &m6F0, harness_r8);
    }
    if (mAcch.ChkGroundHit()) {
        attn_onBit(this, fopAc_Attn_ACTION_CARRY_e);
        cXyz sp1C;
        sp1C.x = speed.x;
        sp1C.y = 0.0f;
        sp1C.z = speed.z;
        if (PSVECSquareMag(&sp1C) < 0.1f) {
            tsuboMode_mode_wait_init(this);
        }
    } else {
        attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    }
}

/* 024CF844 */
void daTsuboMode::Act_c::mode_carry(u32 r4, u32 harness_r5) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    const bool tmp = m685 != 0;
    if (!tmp) {
        harness_r5 = 8;
        if (prm_get_spec(this) == 6) {
            m686 = 1;
            m685 = 1;
        } else if (daPy_getGrabActorID(player) == mBsPcId /* fopAcM_GetID */) {
            harness_r5 = daPy_status(player);
            if (harness_r5 & 0x8000 /* getGrabUpStart */) {
                m686 = 1;
                m685 = 1;
            }
        } else {
            m685 = 1;
            m686 = 0;
        }
    }
    if (m685 != 0 && !tmp) {
        tsuboMode_init_rot_clean(this);
        shape_angle.z = 0;
        shape_angle.x = 0;
    }
    u32 st = daPy_status(player);
    if (!(st & 0x20))
        harness_r5 = st & 0x400;
    bool bVar2 = (st & 0x20) /* getGrabUpEnd */ || (st & 0x400) /* getRightFootOnGround */;
    if (bVar2 && mType == 2) {
        harness_r5 = 0;
        tsuboMode_eff_drop_water(this);
        tsuboMode_seStart(this, JA_SE_LK_CONVEY_WPOT, 0);
    }
    se_pickup_carry();
    if (m810 == 0) {
        speed.y = 0.0f;
    }
    if (actor_status & 0x102000) { /* fopAcM_checkCarryNow || fopAcM_checkHookCarryNow */
        spec_mode_carry(bVar2);
    } else {
        m6FC = current.pos.y;
        if (speedF > 0.0f && m684 == 0) {
            init_rot_throw();
            mode_drop_init();
            mode_drop();
        } else {
            tsuboMode_daObj_SetCurrentRoomNo_r5(this, acch_gnd(this), harness_r5);
            spec_mode_put_init();
            m683 = 2;
            tsuboMode_mode_wait_init(this);
        }
    }
}

/* 024CF5AC */
void daTsuboMode::Act_c::mode_drop_init() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    Act_c::Data_c* d = data(this);
    f32 fVar1 = gabi::fmadds(player->speedF, d->m1C, d->m18);
    mCyl.OnAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    mAcch.m_flags = (mAcch.m_flags & ~0x40Eu) | dBgS_Acch::LINE_CHECK;
    m67F = 0;
    mStts.Init(d->m10, 0xFF, this);
    attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    if (m810 == 0) {
        speed.y = data(this)->m14;
    }
    speedF = fVar1;
    m810 = 0;
    m678 = 5;
    gravity = data(this)->mGravity;
}

/* 024CF6C8 */
void daTsuboMode::Act_c::mode_drop() {
    tsuboMode_daObj_SetCurrentRoomNo(this, acch_gnd(this));
    cXyz sp14;
    sp14.x = mStts.m_cc_move.x;
    sp14.y = 0.0f;
    sp14.z = mStts.m_cc_move.z;
    if (PSVECSquareMag(&sp14) > 0.010000001f) {
        speedF = speedF * 0.9f;
    }
    gabi::Local<be<f32>> sp10, sp08, sp0C;
    calc_drop_param(sp10, sp08, sp0C);
    gravity = *sp10;
    daObj_posMoveF_stream(this, &mStts.m_cc_move, &m7F4, *sp08, *sp0C);
}

/* 024CFAB8 */
void daTsuboMode::Act_c::mode_sink() {
    tsuboMode_daObj_SetCurrentRoomNo(this, acch_gnd(this));
    water_tention();
    Act_c::Data_c* d = data(this);
    daObj_posMoveF_stream(this, &mStts.m_cc_move, cXyz_Zero, d->m40, d->m44);
}

/* 024CFD3C */
void daTsuboMode::Act_c::mode_afl() {
    tsuboMode_daObj_SetCurrentRoomNo(this, acch_gnd(this));
    cXyz* pStreamSpd = tsuboMode_daObj_get_path_spd(Acch_gnd(&mAcch), 30.0f);
    cXyz sp20;
    cXyz_ml(&m7F4, &sp20, data(this)->m20);
    gabi::Local<be<f32>> sp08, sp0C, sp10;
    calc_afl_param(sp08, sp0C, sp10);
    s32 t = gabi::ftoi(cM_rndF(400.0f));
    m804 = (s16)(m804 + t + 0x514);
    gravity = gabi::fmadds(cM_ssin(m804), 0.3f, *sp08);
    water_tention();
    tsuboMode_daObj_posMoveF_grade(this, &mStts.m_cc_move, pStreamSpd, *sp0C, *sp10, nullptr, 0.0f, 0.0f, &sp20, 0x10040000 /* harness */);
    s32 t2 = gabi::ftoi(cM_rndF(400.0f));
    m806 = (s16)(m806 + t2 + 0x514);
}

/* 024CE454 */
bool daTsuboMode::Act_c::mode_proc_call() {
    if ((actor_status & 0x102000) && m678 != 4) { /* fopAcM_checkCarryNow || fopAcM_checkHookCarryNow */
        tsuboMode_mode_carry_init(this);
    }
    s32 mode = m678;
    cXyz sp08;
    sp08.x = current.pos.x;
    sp08.y = current.pos.y;
    sp08.z = current.pos.z;
    (this->*mode_proc.mProc[mode])();
    bool isMoveBg = prm_get_moveBg(this);
    f32 verticalSpeed = speed.y;
    tsuboMode_crr_pos(this, &sp08);
    if (tsuboMode_damage_bg_proc_directly(this)) {
        return false;
    }
    if (!isMoveBg) {
        tsuboMode_bound(this, verticalSpeed);
    }
    moment_proc_call();
    (this->*set_tensor_proc.mProc[m678])();
    if (m678 != 4) {
        tevStr.mRoomNo = fopAcM_GetRoomNo(this);
        tevStr.mEnvrIdxOverride = tsuboMode_dBgS_GetPolyColor(dComIfG_Bgsp(), Acch_gnd(&mAcch));
    }
    PSVECScale(&m7F4, &m7F4, 0.95f);
    if (PSVECSquareMag(&m7F4) < 0.1f) {
        m7F4.set(0.0f, 0.0f, 0.0f);
    }
    PSVECScale(&m814, &m814, 0.6f);
    if (PSVECSquareMag(&m814) < 0.1f) {
        m814.set(0.0f, 0.0f, 0.0f);
    }
    spec_set_room();
    if (mType == 14) {
        eff_kutani_set();
    }
    return true;
}

/* 024CFA48 */
void daTsuboMode::Act_c::water_tention() {
    if (tsuboMode_chk_sink_water(this)) {
        if (m4FC != -1000000000.0f && m500 != -1000000000.0f) {
            f32 d = m4FC - m500;
            f32 k = d >= 0.0f ? 0.2f : 0.8f;
            current.pos.y = gabi::fmadds(d, k, current.pos.y);
        }
    }
}

/* 024CFE88 */
void daTsuboMode::Act_c::moment_small(const cXyz* i_axis) {
}

/* 024CFE8C */
void daTsuboMode::Act_c::moment_big(const cXyz* i_axis) {
}

/* 024CFE90 */
void daTsuboMode::Act_c::moment_water(const cXyz* i_axis) {
}

/* 024CFE94 */
void daTsuboMode::Act_c::moment_barrel(const cXyz* i_axis) {
}

/* 024CFE98 */
void daTsuboMode::Act_c::moment_stool(const cXyz* i_axis) {
}

/* 024CFE9C */
void daTsuboMode::Act_c::moment_skull(const cXyz* i_axis) {
}

/* 024CFEA0 */
void daTsuboMode::Act_c::moment_pail(const cXyz* i_axis) {
}

/* 024CFEA4 */
void daTsuboMode::Act_c::moment_spine(const cXyz* i_axis) {
}

/* 024CFEA8 */
void daTsuboMode::Act_c::moment_hbox2S(const cXyz* i_axis) {
}

/* 024CFEAC */
void daTsuboMode::Act_c::moment_tryColSun(const cXyz* i_axis) {
}

/* 024CFEB0 */
void daTsuboMode::Act_c::moment_tryColMer(const cXyz* i_axis) {
}

/* 024CFEB4 */
void daTsuboMode::Act_c::moment_tryColJup(const cXyz* i_axis) {
}

/* 024CFEB8 */
void daTsuboMode::Act_c::moment_tryKeyGate(const cXyz* i_axis) {
}

/* 024CFEBC */
void daTsuboMode::Act_c::moment_pinecone(const cXyz* i_axis) {
}

/* 024CFEC0 */
void daTsuboMode::Act_c::moment_kutani(const cXyz* i_axis) {
}

/* 024CFEC4 */
void daTsuboMode::Act_c::moment_woodS(const cXyz* i_axis) {
}

/* 024CE204 */
void daTsuboMode::Act_c::moment_proc_call() {
    m6F0.x = cXyz_Zero->x;
    m6F0.y = cXyz_Zero->y;
    m6F0.z = cXyz_Zero->z;
    if (m678 == 3) {
        void* pla = nullptr;
        if (mAcch.ChkGroundHit()) {
            pla = dBgS_GetTriPla(dComIfG_Bgsp(), Acch_gnd(&mAcch));
        }
        if (pla != nullptr) {
            (this->*moment_proc.mProc[mType])((const cXyz*)pla);
        } else {
            cLib_addCalc0(&m6EC, 0.4f, (f32)(u8)data(this)->mAcchRoofHeight * 0.125f);
        }
    }
}

/* 024CF4E0 */
void daTsuboMode::Act_c::init_rot_throw() {
    cSAngle_Val(&m688, (s16)data(this)->m28);
    cSAngle_muleq(&m688, cM_rnd());
    cSAngle_Val(&m68A, (s16)gabi::ftoi(cM_rndFX(32768.0f)));
    cSAngle_Val(&m68C, (s16)data(this)->m2A);
    cSAngle_Val(&m68E, cSAngle__0);
    cSAngle_Val(&m690, (s16)data(this)->m2C);
    cSAngle_Val(&m692, (s16)current.angle.y);
}

/* 024CFF40 */
void daTsuboMode::Act_c::set_tensor_hide() {
    set_tensor_wait();
}

/* 024CFF44 */
void daTsuboMode::Act_c::set_tensor_appear() {
    set_tensor_wait();
}

/* 024CFEC8 */
void daTsuboMode::Act_c::set_tensor_wait() {
    tensor_axis(this);
}

/* 024CFF48 */
void daTsuboMode::Act_c::set_tensor_walk() {
    cXyz sp28;
    cXyz_mi(&current.pos, &sp28, &old.pos);
    f32 len = std_sqrtf(PSVECSquareMag(&sp28));
    f32 abs = (len + len) / (f32)(u8)data(this)->mAcchRoofHeight;
    f32 r = tsuboMode_cSAngle_Radian(&m690);
    cSAngle_Val(&m690, tsuboMode_cM_rad2s(gabi::fmadds(abs - r, 0.7f, r)));
    cSAngle_addeq(&m68E, &m690);
    gabi::Local<be<s16>> sp08;
    be<s16>* p = tsuboMode_cSAngle_ct_s16(sp08, current.angle.y);
    m692 = *p;
    tensor_axis(this);
}

/* 024D0070 */
void daTsuboMode::Act_c::set_tensor_carry() {
    set_tensor_wait();
}

/* 024D0074 */
void daTsuboMode::Act_c::set_tensor_drop() {
    tensor_damp(&m68C, this, &Act_c::Data_c::m30, &Act_c::Data_c::m34);
    cSAngle_addeq(&m68A, &m68C);
    tensor_damp(&m690, this, &Act_c::Data_c::m30, &Act_c::Data_c::m34);
    cSAngle_addeq(&m68E, &m690);
    tensor_axis(this);
}

/* 024D0184 */
void daTsuboMode::Act_c::set_tensor_sink() {
    tensor_damp(&m68C, this, &Act_c::Data_c::m48, &Act_c::Data_c::m4C);
    cSAngle_addeq(&m68A, &m68C);
    tensor_damp(&m690, this, &Act_c::Data_c::m48, &Act_c::Data_c::m4C);
    cSAngle_addeq(&m68E, &m690);
    tensor_axis(this);
}

/* 024D0294 */
void daTsuboMode::Act_c::set_tensor_afl() {
    tensor_damp(&m68C, this, &Act_c::Data_c::m48, &Act_c::Data_c::m4C);
    cSAngle_addeq(&m68A, &m68C);
    f32 r = tsuboMode_cSAngle_Radian(&m68E);
    cSAngle_addeq(&m690, tsuboMode_cM_rad2s(r * -0.0005f));
    cSAngle_addeq(&m690, (s16)gabi::ftoi(cM_ssin(m806) * 4.0f));
    tensor_damp(&m690, this, &Act_c::Data_c::m48, &Act_c::Data_c::m4C);
    cSAngle_addeq(&m68E, &m690);
    tensor_axis(this);
}

/* 024D0404 */
void daTsuboMode::Act_c::eff_break_barrel() {
    cXyz sp18;
    sp18.x = current.pos.x;
    sp18.y = gabi::fmadds((f32)(u8)data(this)->mAcchRoofHeight, 0.5f, current.pos.y);
    sp18.z = current.pos.z;
    JPABaseEmitter* e = dComIfGp_particle_set(ID_IT_JN_TR_HAHEN_A, &sp18, nullptr, nullptr, 0xFF, nullptr, -1,
                                              tev_colorK0(this), tev_colorK0(this));
    if (e != nullptr) {
        // HD: store the initialization guard before setting the emitter scale.
        auto& guard = barrelScaleInitialized;
        cXyz* em_scl = &barrelEmitterScale;
        if (guard == 0) {
            guard = 1;
            em_scl->set(1.0f, 0.8f, 1.0f);
        }
        f32 sx = em_scl->x, sy = em_scl->y, sz = em_scl->z;
        auto* emitter = reinterpret_cast<Emitter*>(e);
        emitter->mEmitterScale.x = sx;
        emitter->mEmitterScale.y = sy;
        emitter->mEmitterScale.z = sz;
    }
    make_obj_smoke(0 /* barrel */, &sp18);
}

/* 024D0550 */
void daTsuboMode::Act_c::eff_break_tsubo() {
    u16 textureIndex = (u16)data(this)->m80;
    J3DModelData* mdl = (J3DModelData*)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_MPM_TUBO_e, TSUBO_SAFESTRING_VTBL);
    void* btp = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTP_MPM_TUBO_e, TSUBO_SAFESTRING_VTBL);
    JPABaseEmitter* e = dComIfGp_particle_set(ID_AK_JN_M_TUBOHAHEN, &current.pos, nullptr, &data(this)->m74);
    if (e != nullptr) {
        void* me = new_dPa_J3DmodelEmitter(e, mdl, 0, &tevStr, btp, textureIndex, 0);
        if (me != nullptr) {
            dComIfGp_particle_addModelEmitter(me);
        }
    }
    GXColor color;
    color.r = (u8)tevStr.mColorC0.r; /* mColorC0 */
    color.g = (u8)tevStr.mColorC0.g;
    color.b = (u8)tevStr.mColorC0.b;
    color.a = (u8)tevStr.mColorC0.a;
    Act_c::Data_c* d = data(this);
    auto* cb = reinterpret_cast<dPa_levelEcallBack*>(&tsuboCallbacks.mTsubo[d->m80]); /* dPa_control_c::getTsuboSelectTexEcallBack(m80) */
    dComIfGp_particle_set(ID_AK_JN_TUBOKONAGONA, &current.pos, nullptr, &d->m74, 0xFF, cb, -1,
                          tev_colorK0(this), &color); // HD: particle primary/environment colors are reversed from GameCube.
}

/* 024D06F4 */
void daTsuboMode::Act_c::eff_break_stool() {
    cXyz sp18;
    sp18.x = current.pos.x;
    sp18.y = current.pos.y + 35.0f;
    sp18.z = current.pos.z;
    // HD: store the initialization guard before setting the particle scale.
    auto& guard = stoolScaleInitialized;
    cXyz* particle_scl = &stoolParticleScale;
    if (guard == 0) {
        guard = 1;
        particle_scl->set(1.0f, 0.8f, 1.0f);
    }
    JPABaseEmitter* e = dComIfGp_particle_set(ID_IT_JN_TR_HAHEN_C, &sp18, nullptr, nullptr, 0xFF, nullptr, -1,
                                              tev_colorK0(this), tev_colorK0(this), particle_scl);
    if (e != nullptr) {
        reinterpret_cast<Emitter*>(e)->mInitialVelDir = 25.0f; /* setDirectionalSpeed */
    }
    make_obj_smoke(1 /* stool */, &sp18);
}

/* 024D07FC */
void daTsuboMode::Act_c::eff_break_skull() {
    cXyz sp18;
    sp18.x = current.pos.x;
    sp18.y = current.pos.y + 20.0f;
    sp18.z = current.pos.z;
    dComIfGp_particle_set(ID_IT_JN_DOKURO00, &sp18, nullptr, nullptr, 0xFF, nullptr, -1, tev_colorK0(this), tev_colorK0(this));
    make_obj_smoke(2 /* skull */, &sp18);
}

/* 024D08B4 */
void daTsuboMode::Act_c::eff_break_pail() {
    eff_break_stool();
}

/* 024D08B8 */
void daTsuboMode::Act_c::eff_break_pinecone() {
    cXyz sp18;
    sp18.x = current.pos.x;
    sp18.y = gabi::fmadds((f32)(u8)data(this)->mAcchRoofHeight, 0.5f, current.pos.y);
    sp18.z = current.pos.z;
    dComIfGp_particle_set(ID_IT_SN_BOKKURI_MATSU00, &sp18, nullptr, nullptr, 0xFF, nullptr, -1, tev_colorK0(this), tev_colorK0(this));
    make_obj_smoke(4 /* pinecone */, &sp18);
}

/* 024CE3D0 */
void daTsuboMode::Act_c::eff_kutani_set() {
    if (m798 != 0) {
        m798 = (u8)(m798 - 1);
        if (m798 == 0) {
            dComIfGp_particle_set(ID_AK_SN_KUTANIVESSEL, &current.pos, nullptr, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&m784);
        }
    }
}

/* 024CED64 */
void daTsuboMode::Act_c::calc_drop_param(be<f32>* outArg1, be<f32>* outArg2, be<f32>* outArg3) {
    f32 f3;
    if (drop_level(this, &f3)) {
        Act_c::Data_c* d = data(this);
        f32 f0;
        if (!(f3 < 0.0f)) {
            f0 = 0.0f;
        } else if (!(f3 > (f32)(-(s32)(u8)d->mAcchRoofHeight))) {
            f0 = 0.5f;
        } else {
            f0 = -(f3 * (0.5f / (f32)(u8)d->mAcchRoofHeight));
        }
        f32 f6 = 1.0f - f0;
        *outArg2 = gabi::fmadds(f0, d->m40, f6 * d->m20);
        d = data(this);
        *outArg3 = gabi::fmadds(f0, d->m44, f6 * d->m24);
        d = data(this);
        f32 gmax = d->mGravity * 0.1f;
        f32 g = gabi::fmadds(f0, d->m3C, d->mGravity);
        *outArg1 = (gmax - g >= 0.0f) ? g : gmax; /* GHS fsel preserves the unordered comparison. */
    } else {
        *outArg2 = data(this)->m20;
        *outArg3 = data(this)->m24;
        *outArg1 = data(this)->mGravity;
    }
}

/* 024CFB20 */
void daTsuboMode::Act_c::calc_afl_param(be<f32>* outArg1, be<f32>* outArg2, be<f32>* outArg3) {
    f32 f3;
    if (drop_level(this, &f3)) {
        Act_c::Data_c* d = data(this);
        f32 f0;
        if (!(f3 < 0.0f)) {
            f0 = 0.0f;
        } else if (!(f3 > (f32)(-(s32)(u8)d->mAcchRoofHeight))) {
            f0 = 1.0f;
        } else {
            f0 = -(f3 / (f32)(u8)d->mAcchRoofHeight);
        }
        f32 f6 = 1.0f - f0;
        *outArg2 = gabi::fmadds(f0, d->m40, f6 * d->m20);
        d = data(this);
        *outArg3 = gabi::fmadds(f0, d->m44, f6 * d->m24);
        d = data(this);
        *outArg1 = gabi::fmadds(f0, d->m3C, d->mGravity);
    } else {
        *outArg2 = data(this)->m20;
        *outArg3 = data(this)->m24;
        *outArg1 = data(this)->mGravity;
    }
}

/* 024CF2BC */
void daTsuboMode::Act_c::se_pickup_carry() {
    if (mType == 7) {
        if (m811 != 0 && (daPy_status(dComIfGp_getPlayer(0)) & 0x8000) /* getGrabUpStart */) {
            m811 = 0;
            tsuboMode_se_pickup(this);
        }
    }
}

/* 024CE71C */
bool daTsuboMode::Act_c::_execute() {
    tsuboMode_cull_set_move(this);
    bool hiddenBySwitch;
    if (prm_get_spec(this) == 5) {
        hiddenBySwitch = !is_switch(this);
    } else {
        hiddenBySwitch = false;
    }
    if (!hiddenBySwitch) {
        if (m682 != 0 || m678 != 2 || !mAcch.ChkGroundHit() || mAcch.ChkGroundLanding() || prm_get_cull(this) == 0 ||
            !(actor_condition & 4) || !tsuboMode_fopAcM_cullingCheck(this) || m800 != 0 || prm_get_moveBg(this)) {
            m682 = 0;
            BOOL shouldDelete = TRUE;
            tsuboMode_spec_set_actor(this);
            if (!tsuboMode_damage_cc_proc(this)) {
                if (!tsuboMode_damage_bg_proc(this) && !tsuboMode_damage_kill_proc(this)) {
                    if (m683 != 0) {
                        m683 = (u8)(m683 - 1);
                    }
                    if (mode_proc_call()) {
                        shouldDelete = FALSE;
                        tsuboMode_set_mtx(this);
                        tsuboMode_spec_carry_spec(this);
                        if (m678 != 0) {
                            mStts.mRoomId = current.roomNo;
                            tsuboMode_dCcD_Cyl_MoveCAtTg(&mCyl, &current.pos);
                            dComIfG_Ccsp_Set(&mCyl);
                        }
                        if (m678 == 5 || m678 == 6 || m678 == 3 || m683 != 0) {
                            dComIfG_Ccsp_SetMass(&mCyl, 3);
                        }
                        f32 x = current.pos.x;
                        f32 y = current.pos.y + data(this)->mAttnY;
                        f32 z = current.pos.z;
                        attention_info.position.x = x;
                        attention_info.position.y = y;
                        attention_info.position.z = z;
                        eyePos.set(x, y, z);
                    }
                }
            }
            tsuboMode_spec_clr_actor(this);
            model = (m678 == 4 && m686) ? (u32)mpModel.v : 0u; /* fopAcM_SetModel */
            if (shouldDelete) {
                fopAcM_delete(this);
            }
        }
    }
    tsuboMode_cull_set_draw(this);
    return true;
}

/* 024CE994 */
bool daTsuboMode::Act_c::_draw() {
    if (m678 == 0 || prm_get_moveBg(this)) {
        return true;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    if (mpBrk != nullptr) {
        auto* anm = reinterpret_cast<Animation*>(((mDoExt_brkAnm*)mpBrk)->mpAnm.get());
        s32 maxFrame = anm->getFrameMax();
        f32 maxf = (f32)maxFrame;
        f32 f = maxf * (1.0f - (f32)m800 / 1200.0f /* M_attrSpine.m30 */);
        if (m800 == 0 || f < 0.0f) {
            f = 0.0f;
        } else if (!(f < maxf)) {
            f = maxf - 0.001f;
        }
        mDoExt_brkAnm_entry(mpBrk, J3DModel_getModelData(mpModel), f);
    }
    mDoExt_modelUpdateDL(mpModel);
    if (mpBrk != nullptr) {
        reinterpret_cast<ModelAnimation*>(J3DModel_getModelData(mpModel))->mAnmTevRegKey = 0; /* mDoExt_brkAnm::remove (HD inline) */
    }
    return true;
}

// HD: compiler-generated color conversion for this translation unit.
/* 024D0B9C */
static void color_to_f4(be<f32>* out, const GXColor* c) {
    f32 r = (f32)(u8)c->r / 255.0f;
    f32 g = (f32)(u8)c->g / 255.0f;
    f32 b = (f32)(u8)c->b / 255.0f;
    f32 al = (f32)(u8)c->a / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = al;
}

/* WWHD glue
data mode_proc 10040E30
data set_tensor_proc 10040E70
data moment_proc 10040DA8
data tsuboCallbacks 1047B24C
data BaseX 101FFBB4
data barrelScaleInitialized 1046EA80
data barrelEmitterScale 1046EA84
data stoolScaleInitialized 1046EA90
data stoolParticleScale 1046EA70
str 10040EE8 "Always"
ext 025D6CE8 BOOL tsuboMode_fopAcM_cullingCheck(fopAc_ac_c* a)
ext 025165A4 void tsuboMode_dCcD_Cyl_MoveCAtTg(dCcD_Cyl* c, cXyz* p)
ext 024EEEB8 u8 tsuboMode_dBgS_GetPolyColor(dBgS* bgs, cBgS_PolyInfo* p)
ext 024CBD48 void tsuboMode_cull_set_move(daTsuboMode::Act_c* a)
ext 024CA5F0 void tsuboMode_cull_set_draw(daTsuboMode::Act_c* a)
ext 024CAF20 void tsuboMode_spec_clr_actor(daTsuboMode::Act_c* a)
ext 024CAE78 void tsuboMode_spec_carry_spec(daTsuboMode::Act_c* a)
ext 024CADD8 void tsuboMode_spec_set_actor(daTsuboMode::Act_c* a)
ext 024CB280 void tsuboMode_set_mtx(daTsuboMode::Act_c* a)
ext 024CD3CC bool tsuboMode_damage_kill_proc(daTsuboMode::Act_c* a)
ext 024CD1E8 bool tsuboMode_damage_bg_proc(daTsuboMode::Act_c* a)
ext 024CC7EC bool tsuboMode_damage_cc_proc(daTsuboMode::Act_c* a)
ext 024CDF3C void tsuboMode_bound(daTsuboMode::Act_c* a, f32 v)
ext 024CDAB4 bool tsuboMode_damage_bg_proc_directly(daTsuboMode::Act_c* a)
ext 024CD7AC void tsuboMode_crr_pos(daTsuboMode::Act_c* a, cXyz* p)
ext 024CD4C8 void tsuboMode_mode_carry_init(daTsuboMode::Act_c* a)
ext 024CC798 void tsuboMode_se_pickup(daTsuboMode::Act_c* a)
ext 024CD95C void tsuboMode_eff_drop_water(daTsuboMode::Act_c* a)
ext 025D5418 void tsuboMode_fopAcM_setRoomLayer(fopAc_ac_c* a, s32 room)
ext 023120B4 cXyz* tsuboMode_daObj_get_path_spd(cBgS_PolyInfo* p, f32 v)
ext 02312968 void tsuboMode_daObj_SetCurrentRoomNo_r5(fopAc_ac_c* a, void* gnd, u32 harness_r5)
ext 023121C4 void tsuboMode_daObj_posMoveF_grade(fopAc_ac_c* a, const cXyz* cc, const cXyz* stream, f32 k1, f32 k2, const void* nrm, f32 g, f32 cosg, const cXyz* mom, u32 harness_r8)
ext 0200ECD4 void tsuboMode_cLib_addCalc(be<f32>* v, f32 t, f32 s, f32 mx, f32 mn)
ext 02312968 void tsuboMode_daObj_SetCurrentRoomNo(fopAc_ac_c* a, void* gnd)
ext 0200658C be<s16>* tsuboMode_cSAngle_ct_s16(be<s16>* a, s16 v)
ext 02019510 s16 tsuboMode_cM_rad2s(f32 r)
ext 02006760 f32 tsuboMode_cSAngle_Radian(be<s16>* a)
ext 024CCB44 bool tsuboMode_chk_sink_water(daTsuboMode::Act_c* a)
ext 024CAA54 void tsuboMode_init_rot_clean(daTsuboMode::Act_c* a)
ext 024CAC44 void tsuboMode_mode_wait_init(daTsuboMode::Act_c* a)
ext 024CAB30 void tsuboMode_mode_appear_init(daTsuboMode::Act_c* a)
ext 024CB088 void tsuboMode_set_tensor(daTsuboMode::Act_c* a, const cXyz* v)
ext 025A3BDC void* tsuboMode_modelEmitter(u32 self, JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i)
ext 02516C14 void tsuboMode_setMass(void* manager, void* object, u8 priority)
ext 0200FE78 void tsuboMode_addModelEmitter(u32 list, void* emitter)
*/
