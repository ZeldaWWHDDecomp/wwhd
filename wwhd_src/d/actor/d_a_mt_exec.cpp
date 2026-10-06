/**
 * d_a_mt_exec.cpp (WWHD)
 * Enemy - Magtail: daMt_Execute
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_mt.cpp) has only a "Nonmatching" placeholder for daMt_Execute, so it is written
 * from the WWHD code (cking.rpx, 021D87A0, 16 KB) and verified against it. WWHD inlines the
 * GameCube functions mt_eye_tex_anm, damage_check, mt_move (with body_control1), mt_fight and
 * the line-of-sight/background checks; they are static inline helpers here, named after the
 * GameCube symbols where the code matches them.
 */
#include "d/actor/d_a_mt.h"
#include <cmath>

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* d_cc_uty: CcAtInfo (0x1C) as in d_a_bb */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
static inline void at_power_check(CcAtInfo_l* i) { gabi::call(0x02518DB0, i); }
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
/* 02518CC8 (unnamed by the matcher): def_se_set(actor, cCcD_Obj* hitObj, u32 mtrlSndId) */
static inline void def_se_set(fopAc_ac_c* a, u32 obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
/* 025DAF3C fopKyM_createMpillar(const cXyz* pos, f32 scale) */
static inline void fopKyM_createMpillar(cXyz* pos, f32 scale) { gabi::call(0x025DAF3C, pos, scale); }
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
static inline void* fpcEx_Search(u32 fn, void* data) { return gabi::call<void*>(0x025DE508, fn, data); }
static inline BOOL dAttention_LockonTruth(dAttention_c* a) { return gabi::call<BOOL>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
/* daPy_py_c::checkPlayerGuard: virtual, slot +0x3C of the HD vtable (+0xB4) */
static inline BOOL daPy_checkPlayerGuard(fopAc_ac_c* p) { return gabi::call_ptr<BOOL>(gabi::load<u32>(p->__vtbl + 0x3C), p); }
/* 025E1FD8 mDoAud_onEnemyDamage(), 025E1D30 mDoAud_bgmHitSound(s32) (JAIZelBasic wrappers, unnamed) */
static inline void mDoAud_onEnemyDamage() { gabi::call(0x025E1FD8); }
static inline void mDoAud_bgmHitSound(s32 id) { gabi::call(0x025E1D30, id); }
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { gabi::call(0x0255F554, pos, p); }
/* csXyz::operator+ (0201A4DC): the 6-byte result comes back in r3:r4 */
static inline void csXyz_pl(csXyz* res, const csXyz* a, const csXyz* b) {
    gabi::call(0x0201A4DC, a, b);
    u32 r3 = gabi::cpu->r[3], r4 = gabi::cpu->r[4];
    res->x = (s16)(r3 >> 16);
    res->y = (s16)r3;
    res->z = (s16)(r4 >> 16);
}
/* this TU's out-of-line copies (d_a_mt.cpp) */
static inline f32 sinShort_ool(s16 a) { return gabi::call<f32>(0x021DD620, gabi::at<void>(0x104A44F8), a); }
static inline f32 cosShort_ool(s16 a) { return gabi::call<f32>(0x021DFDDC, gabi::at<void>(0x104A44F8), a); }
static inline void mt_fopAcM_seStart(fopAc_ac_c* a, u32 id, u32 p) { gabi::call(0x021DD518, a, id, p); }
static inline void mt_fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 p) { gabi::call(0x021DD584, a, id, p); }

#define REG6_F(i) REG_F(6, i)
#define REG6_S(i) REG_S(6, i)
#define REG10_S(i) REG_S(10, i)

/* dBgS_ObjGndChk_Yogan (lava ground check, stack object; this TU's vtables, group 4) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);
static inline void dBgS_ObjGndChk_Yogan_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    c->mPass[0] = 1;
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->__vtbl_10 = 0x1001555C;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x1001557C;
    c->__vtbl_40 = 0x1001558C;
    c->mGrp = 4;
    c->__vtbl_20 = 0x1001556C;
}
static inline void dBgS_ObjGndChk_Yogan_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x100154EC;
    c->__vtbl_40 = 0x1001550C;
    c->__vtbl_4C = 0x100154CC;
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
/* dBgS_ObjLinChk (mPass[0] = 1): its own vtables; the destructor is dBgS_LinChk's */
static inline void dBgS_ObjLinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    c->mPass[0] = 1;
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x1001560C;
    c->__vtbl_64 = 0x1001562C;
    c->__vtbl_58 = 0x1001563C;
    c->__vtbl_20 = 0x1001561C;
}
static inline cXyz* LinChk_GetCross(dBgS_LinChk_l* c) { return gabi::at<cXyz>(gabi::ea(c) + 0x30); }

struct cXyz6 { cXyz v[6]; };
struct s16x5 { be<s16> v[5]; };

/* .data / statics of the unit */
static inline f32 mt_move_chk_x(u32 i) { return gabi::load<f32>(0x101BAE74 + 4 * i); }
static inline f32 mt_move_chk_y(u32 i) { return gabi::load<f32>(0x101BAE8C + 4 * i); }
static inline f32 mt_move_chk_z(u32 i) { return gabi::load<f32>(0x101BAEA4 + 4 * i); }
static inline u8 mt_move_chk_bit(u32 i) { return gabi::load<u8>(0x101BADC8 + i); }
static inline s32 mt_trail_ofs(u32 i) { return gabi::load<s32>(0x101BAE14 + 4 * i); }
static inline s32 mt_trail_ofs_fast(u32 i) { return gabi::load<s32>(0x101BAE34 + 4 * i); }
/* mt_fight: the attack slot (static u8 in .data) */
static inline be<u8>& fight_slot() { return *gabi::at<be<u8>>(0x101BADD0); }

/* attention_info (fopAc_ac_c +0x388): position +0x08, flags +0x14 */
static inline cXyz* attention_pos(fopAc_ac_c* a) { return gabi::at<cXyz>(gabi::ea(a) + 0x390); }
static inline void attention_flags_set(fopAc_ac_c* a, u32 f) { gabi::store<u32>(gabi::ea(a) + 0x39C, f); }
static inline u8 daPy_procNo(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
static inline s32 abs16(s16 v) { return v < 0 ? -(s32)v : (s32)v; }

/* fopAcM_monsSeStart (HD inline) */
static inline void mt_monsSe(mt_class* i_this, u32 id) {
    if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
        u32 pid = fopAcM_GetID(i_this);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
        gabi::call(0x025E1AA4, id, &i_this->eyePos, pid, 0, reverb); /* mDoAud_monsSeStart */
    }
}

/* the collision push of this frame (dCcD_Stts::GetCCMoveP), applied to the position */
static inline void mt_cc_move(mt_class* i_this) {
    if (gabi::ea(&i_this->mStts) != 0) {
        i_this->current.pos.x = i_this->current.pos.x + i_this->mStts.m_cc_move.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mStts.m_cc_move.z;
    }
}

/* the knock-back: speed from the hit angle */
static inline void mt_knockback(mt_class* i_this, s16 angleY) {
    mDoMtx_YrotS(calc_mtx(), angleY);
    gabi::Local<cXyz> v;
    f32 k = l_HIO().m4C;
    v->x = 0.0f;
    v->y = 40.0f * k;
    v->z = -20.0f * k;
    MtxPosition(v, &i_this->speed);
}

/* the point m598 behind the head: m598 = current.pos + rotY(shape_angle.y) * (0, y, z) */
static inline void mt_set_back_pos(mt_class* i_this, s16 angleY, f32 y, f32 z) {
    mDoMtx_YrotS(calc_mtx(), angleY);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = y;
    v->z = z;
    MtxPosition(v, &i_this->m598);
    PSVECAdd(&i_this->m598, &i_this->current.pos, &i_this->m598);
}

/* mt_eye_tex_anm (inlined) */
static inline void mt_eye_tex_anm(mt_class* i_this) {
    if (i_this->m580 != 0) {
        i_this->m580 = i_this->m580 - 1;
    } else {
        i_this->m580 = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
        if (i_this->mBtpOn == 0) {
            tex_anm_set(i_this, 0);
        }
    }
    if (i_this->mBtpOn != 0) {
        if (i_this->mBtpFrame < i_this->mBtpMaxFrame) {
            i_this->mBtpFrame = i_this->mBtpFrame + 1;
        } else {
            i_this->mBtpOn = 0;
        }
    }
}

/* damage_check (inlined) */
static inline void damage_check(mt_class* i_this) {
    dComIfGp_get(); /* HD: result unused */
    gabi::Local<CcAtInfo_l> atInfo;
    int hit = 0;
    atInfo->pParticlePos = nullptr;
    i_this->mStts.Move();

    int i = i_this->mD20 == 1 ? 2 : 0;
    for (; i < MT_PART_NUM; i++) {
        if (!i_this->mSph[i].ChkTgHit() || i_this->m57C != 0) {
            continue;
        }
        atInfo->mpObj = gabi::ea(i_this->mSph[i].GetTgHitObj());
        if (gabi::load<u32>(atInfo->mpObj + 0x10) & 0x100000 /* AT_TYPE_LIGHT_ARROW */) {
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->m1DD4 = 1;
            i_this->mEnemyIce.mYOffset = REG0_F(0) + -20.0f;
            i_this->health = 0;
            return;
        }
        at_power_check(atInfo);
        u8 type = atInfo->mResultingAttackType;
        if (type == 4 || (gabi::load<u32>(atInfo->mpObj + 0x10) & 0x80000 /* AT_TYPE_ICE_ARROW */)) {
            i_this->m1A17 = 0;
            i_this->m57C = 5;
            hit = 2;
            water_damage_se_set(i_this);
            goto at_sph;
        }
        u8 state = i_this->m570;
        if (state == 2) {
            if (type == 2) {
                i_this->m1A14 = 3;
                i_this->m57C = 5;
                return;
            }
            if (type == 6) {
                i_this->m1A14 = 2;
                i_this->m57C = 5;
                return;
            }
        } else {
            if (type == 2) {
                i_this->m57C = 5;
                mt_knockback(i_this, atInfo->m0C.y);
                i_this->m570 = 1;
                i_this->m400 = 1;
                i_this->m571 = 0x14;
                return;
            }
            if (type == 6) {
                goto at_sph;
            }
        }
        if (state == 1) {
            i_this->m571 = 15;
            mt_set_back_pos(i_this, i_this->shape_angle.y, 60.0f, -120.0f);
            i_this->m57E = REG6_S(7) + 6;
            i_this->m572 = REG6_S(8) + 6;
            i_this->m57C = 5;
            def_se_set(i_this, atInfo->mpObj, 0x40);
            return;
        }
        if (state == 2) {
            i_this->m57C = REG10_S(0) + 3;
            cc_at_check(i_this, atInfo);
            if (i_this->health <= 0) {
                i_this->m1A14 = 2;
            }
            i_this->m1A18 = 12;
            dComIfGp_particle_set(0x80CF, &i_this->current.pos);
        }
        goto at_sph;
    }

at_sph:
    if (i_this->m57C == 0 && i_this->mAtSph.ChkTgHit()) {
        i_this->m57C = 5;
        atInfo->mpObj = gabi::ea(i_this->mAtSph.GetTgHitObj());
        atInfo->pParticlePos = i_this->mAtSph.GetTgHitPosP();
        if (gabi::load<u32>(atInfo->mpObj + 0x10) & 0x100000 /* AT_TYPE_LIGHT_ARROW */) {
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            return;
        }
        at_power_check(atInfo);
        i_this->m1A17 = (u8)(i_this->m1A17 - atInfo->mDamage);
        u8 type = atInfo->mResultingAttackType;
        if (!((type >= 2 && type <= 4) || type == 6) && i_this->mD20 != 1) {
            return;
        }
        atInfo->mpActor = cc_at_check(i_this, atInfo);
        if (atInfo->mResultingAttackType == 6) {
            i_this->m1A14 = 1;
            hit = 1;
            i_this->m1A17 = 0;
            mt_monsSe(i_this, 0x4803 /* JA_SE_CV_MT_DAMAGE? */);
        } else {
            if (atInfo->mResultingAttackType == 4 || (gabi::load<u32>(atInfo->mpObj + 0x10) & 0x80000)) {
                i_this->m1A17 = 0;
                hit = 2;
                water_damage_se_set(i_this);
            }
            if ((s8)i_this->m1A17 > 0) {
                i_this->m571 = 15;
                mt_set_back_pos(i_this, i_this->shape_angle.y, 60.0f, -120.0f);
                i_this->m57E = 0x19;
                i_this->m572 = 10;
                i_this->current.angle.x = -0x4000;
                if (gabi::ea(&i_this->eyePos) != 0) {
                    mt_monsSe(i_this, 0x4803);
                }
            } else {
                hit = 1;
                mt_monsSe(i_this, 0x4803);
            }
        }
        anm_init(i_this, 10, 2.0f, 2, 1.0f, 0);
        i_this->m19F0 = 0;
    }

    if (hit != 0) {
        mt_knockback(i_this, atInfo->m0C.y);
        if (hit == 2) {
            i_this->speed.y = 0.0f;
        }
        i_this->m400 = 1;
        i_this->m570 = 1;
        i_this->m571 = 0x14;
    }
}

/* body_control1 (inlined into mt_move): the trail, the segment models and colliders */
static inline void body_control1(mt_class* i_this) {
    cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 2, 0x800);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x800);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 4, 0x400);

    i_this->m810[i_this->mD10].copy(i_this->current.pos);
    csXyz* tr = &i_this->mB10[i_this->mD10];
    gabi::store<u16>(gabi::ea(&tr->x), gabi::load<u16>(gabi::ea(&i_this->shape_angle.x)));
    gabi::store<u16>(gabi::ea(&tr->y), gabi::load<u16>(gabi::ea(&i_this->shape_angle.y)));
    gabi::store<u16>(gabi::ea(&tr->z), gabi::load<u16>(gabi::ea(&i_this->shape_angle.z)));
    i_this->mC90[i_this->mD10] = i_this->m584;

    for (int i = 0; i < MT_PART_NUM; i++) {
        s32 ofs = i_this->mD1C != 0 ? mt_trail_ofs_fast(i) : mt_trail_ofs(i);
        u32 k = (u32)(i_this->mD10 + ofs) & 0x3F;
        J3DModel* model = i_this->mpMorf[i]->getModel();
        /* J3DModel::setBaseScale(scale) */
        u32 m = gabi::ea(model);
        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        gabi::store<f32>(m + 0xC0, sy);
        gabi::store<f32>(m + 0xBC, sx);
        gabi::store<f32>(m + 0xC4, sz);
        cXyz* pos = &i_this->m810[k];
        PSMTXTrans(mDoMtx_stack_c::get(), pos->x, pos->y, pos->z);
        csXyz* ang = &i_this->mB10[k];
        mDoMtx_YrotM(mDoMtx_stack_c::get(), ang->y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), ang->x);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), ang->z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), i_this->mC90[k]);
        if (i == 0) {
            f32 s = l_HIO().m1C;
            mDoMtx_stack_c::scaleM(s, s, s);
        } else {
            f32 s = i_this->m71C[i];
            mDoMtx_stack_c::scaleM(s, s * i_this->m73C[i], 1.0f);
        }
        mDoMtx_stack_c::transM(0.0f, 0.0f, i_this->m58C);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        if (i == 0) {
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 0.0f;
            v->z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mDoMtx_stack_c::get(), v, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            i_this->mAtSph.SetR(l_HIO().m44);
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            gabi::Local<cXyz> v2;
            v2->x = 0.0f;
            v2->y = 0.0f;
            v2->z = REG6_F(9) + 100.0f;
            gabi::Local<cXyz> headPos;
            PSMTXMultVec(mDoMtx_stack_c::get(), v2, headPos);
            i_this->mSph[0].SetC(headPos);
            i_this->mSph[0].SetR(50.0f);
            dComIfG_Ccsp_Set(&i_this->mSph[0]);
        } else {
            i_this->mSph[i].SetC(pos);
            if (i_this->m57C != 0) {
                i_this->mSph[i].SetR(-200.0f);
            } else {
                i_this->mSph[i].SetR(l_HIO().m48);
            }
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
        if (i_this->mD1D != 0) {
            i_this->m5BC[i].copy(*pos);
            csXyz* dst = &i_this->m67C[i];
            gabi::store<u16>(gabi::ea(&dst->x), gabi::load<u16>(gabi::ea(&ang->x)));
            gabi::store<u16>(gabi::ea(&dst->y), gabi::load<u16>(gabi::ea(&ang->y)));
            gabi::store<u16>(gabi::ea(&dst->z), gabi::load<u16>(gabi::ea(&ang->z)));
        }
    }
    if (i_this->m5AA == 0) {
        i_this->mD10 = (i_this->mD10 + 1) & 0x3F;
    } else {
        i_this->mD10 = i_this->mD10 & 0x3F;
    }
}

/* mt_move (inlined): crawl along walls/ceilings, steered by eight line checks */
static inline void mt_move(mt_class* i_this) {
    dComIfGp_get(); /* HD: result unused */
    u32 hit = 0;
    {
        gabi::Local<dBgS_LinChk_l> linChk;
        dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mDoMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        gabi::Local<cXyz> offs;
        offs->x = 0.0f;
        offs->z = 0.0f;
        offs->y = 50.0f;
        gabi::Local<cXyz> start;
        MtxPosition(offs, start);
        gabi::Local<cXyz> sum;
        cXyz_pl(start, sum, &i_this->current.pos);
        start->copy(*sum);

        int n = abs16(i_this->current.angle.x) < 0x1000 ? 6 : 4;
        gabi::Local<cXyz6> cross;
        gabi::Local<cXyz6> end;
        for (int i = 0; i < n; i++) {
            offs->z = mt_move_chk_z(i);
            offs->y = mt_move_chk_y(i);
            offs->x = mt_move_chk_x(i);
            MtxPosition(offs, &end->v[i]);
            PSVECAdd(&end->v[i], &i_this->current.pos, &end->v[i]);
            dBgS_LinChk_Set(linChk, start, &end->v[i], i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                cross->v[i].copy(*LinChk_GetCross(linChk));
                hit |= mt_move_chk_bit(i);
            }
        }

        if ((hit & 3) == 3) {
            /* on a wall: follow it */
            gabi::Local<cXyz> d;
            cXyz_mi(&cross->v[0], d, &cross->v[1]);
            offs->y = l_HIO().m18;
            gabi::Local<cXyz> dd;
            dd->copy(*d);
            offs->z = 0.0f;
            offs->x = 0.0f;
            gabi::Local<cXyz> up;
            MtxPosition(offs, up);
            if (i_this->m5AA == 0) {
                cLib_addCalc2(&i_this->current.pos.x, gabi::fmadds(dd->x, 0.5f, cross->v[1].x) + up->x, 1.0f, 1.0f);
                cLib_addCalc2(&i_this->current.pos.y, gabi::fmadds(dd->y, 0.5f, cross->v[1].y) + up->y, 1.0f, 1.0f);
                cLib_addCalc2(&i_this->current.pos.z, gabi::fmadds(dd->z, 0.5f, cross->v[1].z) + up->z, 1.0f, 1.0f);
            }
            f32 dz = dd->z;
            f32 dx = dd->x;
            i_this->current.angle.x = (s16)-cM_atan2s(dd->y, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
            dx = dd->x;
            dz = dd->z;
            if (fabsf(dx) > 0.1f || fabsf(dz) > 0.1f) {
                i_this->m5A8 = cM_atan2s(dx, dz);
            }
            s16 diff = (s16)(i_this->m5A8 - i_this->current.angle.y);
            if (diff < 0) diff = (s16)-diff;
            if ((u16)diff > 0x4000) {
                i_this->current.angle.x = (s16)(-0x8000 - i_this->current.angle.x);
            }
            i_this->m5AE = 0x17;
        } else if (hit & 1) {
            /* keep the pitch */
        } else if (i_this->m5AE != 0) {
            i_this->m5AE = i_this->m5AE - 1;
        } else {
            i_this->current.angle.x = (s16)(i_this->current.angle.x + (REG0_S(2) + 0x800));
        }

        if (abs16(i_this->current.angle.x) < 0x1000) {
            /* on the floor */
            if ((hit & 0x30) == 0x30) {
                gabi::Local<cXyz> d;
                cXyz_mi(&cross->v[4], d, &cross->v[5]);
                f32 dx = d->x, dz = d->z;
                gabi::Local<cXyz> dd;
                dd->copy(*d);
                i_this->current.angle.y = (s16)(cM_atan2s(dx, dz) + 0x4000);
            } else if (i_this->m3D0 >= 10) {
                i_this->current.angle.y = (s16)(i_this->current.angle.y + i_this->m5A4);
            } else {
                s16 target;
                bool turn = true;
                if ((s8)i_this->m3D8 != 0) {
                    /* follow the path */
                    gabi::Local<cXyz> d;
                    cXyz_mi(&i_this->m598, d, &i_this->current.pos);
                    f32 dx = d->x, dz = d->z;
                    gabi::Local<cXyz> dd;
                    dd->copy(*d);
                    target = cM_atan2s(dx, dz);
                    dz = dd->z;
                    dx = dd->x;
                    f32 dzz = dz * dz;
                    i_this->m5A4 = 0x800;
                    if (std_sqrtf(gabi::fmadds(dx, dx, dzz)) < 100.0f) {
                        u32 ppd = gabi::ea(i_this->ppd.get());
                        s8 no = (s8)(i_this->m3D9 + i_this->m3DA);
                        i_this->m3D9 = no;
                        if (no >= (s8)gabi::load<u8>(ppd + 1)) {
                            if (gabi::load<u8>(ppd + 5) & 1) {
                                no = 0;
                                i_this->m3D9 = no;
                            } else {
                                i_this->m3DA = -1;
                                no = (s8)(gabi::load<u16>(ppd) - 2);
                                i_this->m3D9 = no;
                            }
                        } else if (no < 0) {
                            i_this->m3DA = 1;
                            no = 1;
                            i_this->m3D9 = no;
                        }
                        cXyz* pnt = gabi::at<cXyz>(gabi::load<u32>(ppd + 8) + no * 0x10 + 4);
                        i_this->m598.x = pnt->x;
                        i_this->m598.y = pnt->y;
                        i_this->m598.z = pnt->z;
                    }
                } else {
                    target = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                    cLib_addCalcAngleS2(&i_this->m5A4, (s16)(REG0_S(4) + 0x400), 1, 0x10);
                }
                if (i_this->m3D1 == 1) {
                    i_this->current.angle.y = target;
                } else {
                    cLib_addCalcAngleS2(&i_this->current.angle.y, target, 0x10, i_this->m5A4);
                }
                (void)turn;
                if (i_this->m3D0 < 10 && i_this->m1A15 == 0 &&
                    fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0)) < l_HIO().m24) {
                    /* the player is near: drop to the floor once the body is flat */
                    int k;
                    for (k = 0; k < 8; k++) {
                        if (abs16(i_this->mB10[k * 8].x) > 0x1000) break;
                    }
                    if (k == 8) {
                        i_this->mD1D = 1;
                    }
                }
            }
            cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x400);
        } else {
            /* on a wall or the ceiling */
            if ((hit & 0xC) == 0xC) {
                gabi::Local<cXyz> d;
                cXyz_mi(&cross->v[2], d, &cross->v[3]);
                gabi::Local<cXyz> dd;
                dd->copy(*d);
                mDoMtx_XrotS(calc_mtx(), (s16)-i_this->current.angle.x);
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->current.angle.y);
                gabi::Local<cXyz> t;
                MtxPosition(dd, t);
                f32 tz = t->z, tx = t->x;
                i_this->current.angle.z = cM_atan2s(t->y, std_sqrtf(gabi::fmadds(tx, tx, tz * tz)));
            }
            if (i_this->m3D0 < 10) {
                i_this->m5A4 = 0;
            }
        }

        if (i_this->m5AA == 0) {
            s16 ph = i_this->m586;
            s16 reg = REG0_S(0);
            f32 amp = gabi::fmadds(cM_ssin(ph * 100), 1000.0f, 3500.0f);
            cLib_addCalcAngleS2(&i_this->m584, (s16)gabi::ftoi(cM_ssin(ph * (reg + 2000)) * amp), 4, 0x400);
            if (i_this->mD1C != 0) {
                i_this->speedF = 10.0f;
            } else {
                i_this->speedF = 5.0f;
            }
        } else {
            i_this->speedF = 0.0f;
            cLib_addCalc0(&i_this->speedF, 1.0f, 5.0f);
        }
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mDoMtx_YrotM(calc_mtx(), i_this->m584);
        offs->x = 0.0f;
        offs->y = 0.0f;
        offs->z = i_this->speedF;
        MtxPosition(offs, &i_this->speed);
        fopAcM_posMove(i_this, nullptr);

        if ((hit & 0xF) == 0) {
            s16 c = (s16)(i_this->m588 + 1);
            i_this->m588 = c;
            if (c >= 10) {
                /* lost the wall: fall and fight */
                i_this->m570 = 1;
                i_this->m571 = 0x11;
            }
        } else {
            i_this->m588 = 0;
        }
        dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
    }

    body_control1(i_this);

    if (i_this->mD1D != 0) {
        i_this->mD1D = 0;
        i_this->m570 = 1;
        i_this->m571 = 0;
        i_this->m572 = l_HIO().m10;
        i_this->m5AA = 0;
        anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
    }
    cLib_addCalc2(&i_this->m58C, -10.0f, 1.0f, 1.0f);

    if (gabi::ea(&i_this->mStts) != 0) {
        i_this->current.pos.x = i_this->current.pos.x + i_this->mStts.m_cc_move.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mStts.m_cc_move.z;
        for (int i = 0; i < MT_JOINT_NUM; i++) {
            i_this->m810[i].x = i_this->m810[i].x + i_this->mStts.m_cc_move.x;
            i_this->m810[i].z = i_this->m810[i].z + i_this->mStts.m_cc_move.z;
        }
    }

    if (i_this->m574 == 0) {
        i_this->m574 = (s16)gabi::ftoi(cM_rndF(100.0f) + 150.0f);
        if (l_HIO().m05 == 0 && abs16(i_this->current.angle.x) < 0x1000) {
            i_this->m5AA = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f + REG0_F(8));
        }
    }
}

/* the fall of mt_fight: integrate the speed */
static inline void mt_fall(mt_class* i_this) {
    i_this->current.pos.x = i_this->current.pos.x + i_this->speed.x;
    i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
    i_this->current.pos.z = i_this->current.pos.z + i_this->speed.z;
    i_this->m57C = 5;
    f32 vy = i_this->speed.y + i_this->gravity;
    if (vy < -100.0f) {
        i_this->speed.y = -100.0f;
    } else {
        i_this->speed.y = vy;
    }
}

/* mt_fight (inlined): circle the player, then lunge */
static inline void mt_fight(mt_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 pitchOfs = 0;

    switch ((u8)i_this->m571) {
    case 0: {
        mt_all_count() = 0;
        mt_fight_count() = 0;
        fpcEx_Search(0x021D7480 /* mt_a_d_sub */, i_this);
        gabi::Local<s16x5> slotAngle;
        s32 cnt = mt_fight_count();
        if (cnt <= 1) {
            for (int k = 0; k < 5; k++) slotAngle->v[k] = 0;
        } else if (cnt == 2) {
            slotAngle->v[0] = 0x2000;
            slotAngle->v[1] = 0x2000;
            slotAngle->v[2] = 0;
            slotAngle->v[3] = 0;
            slotAngle->v[4] = 0;
        } else if (cnt == 3) {
            slotAngle->v[0] = 0;
            slotAngle->v[1] = -0x3000;
            slotAngle->v[2] = 0x3000;
            slotAngle->v[3] = 0;
            slotAngle->v[4] = 0;
        } else if (cnt == 4) {
            slotAngle->v[0] = 0x2000;
            slotAngle->v[1] = -0x2000;
            slotAngle->v[2] = 0x5000;
            slotAngle->v[3] = -0x5000;
            slotAngle->v[4] = 0;
        } else {
            slotAngle->v[0] = 0;
            slotAngle->v[1] = 0x3000;
            slotAngle->v[2] = -0x3000;
            slotAngle->v[3] = 0x6000;
            slotAngle->v[4] = -0x6000;
        }
        dAttention_c* att = dComIfGp_getAttention();
        if ((dAttention_LockonTruth(att) || (gabi::load<u32>(gabi::ea(att) + 0x20) & 0x20000000)) &&
            dAttention_ActionTarget(att, 0) == i_this) {
            fight_slot() = 0;
        } else {
            u8 s = (u8)(fight_slot() + 1);
            if (s < 5) {
                fight_slot() = s;
            } else {
                fight_slot() = (u8)(s - 5);
            }
        }

        gabi::Local<cXyz> d;
        cXyz_mi(&player->current.pos, d, &i_this->current.pos);
        gabi::Local<cXyz> v;
        f32 dx = d->x;
        v->x = dx;
        v->z = d->z;
        v->y = d->y + (REG6_F(5) + 90.0f);
        i_this->m5B2 = cM_atan2s(dx, d->z);
        f32 vz = v->z, vx = v->x;
        i_this->m5B0 = (s16)-cM_atan2s(v->y, std_sqrtf(gabi::fmadds(vx, vx, vz * vz)));

        s16 ph = i_this->m586;
        gabi::Local<cXyz> wob;
        wob->x = cM_ssin(ph * 500) * (REG0_F(10) + 30.0f);
        f32 s1400 = cM_ssin(ph * 1400);
        wob->z = gabi::fmadds(cM_ssin(ph * 600), REG0_F(12) + 30.0f, l_HIO().m28);
        wob->y = gabi::fmadds(s1400, REG0_F(11) + 20.0f, 60.0f);
        pitchOfs = (s16)gabi::ftoi(s1400 * (REG0_F(16) + -2000.0f));
        mDoMtx_YrotS(calc_mtx(), (s16)(player->shape_angle.y + slotAngle->v[fight_slot()]));
        MtxPosition(wob, v);
        cLib_addCalc2(&i_this->current.pos.x, player->current.pos.x + v->x, 0.1f, REG0_F(10) + 4.0f);
        cLib_addCalc2(&i_this->current.pos.z, player->current.pos.z + v->z, 0.1f, REG0_F(10) + 4.0f);

        f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
        if ((dist > l_HIO().m24 + 100.0f || i_this->m1A15 != 0) && i_this->m582 == 0) {
            /* out of reach: back to the floor, then crawl again */
            cLib_addCalc2(&i_this->current.pos.y, i_this->mAcch.m_ground_h + 40.0f, 0.5f, 3.0f);
            f32 y = i_this->current.pos.y - (i_this->mAcch.m_ground_h + 40.0f);
            i_this->m5B0 = 0;
            pitchOfs = 0;
            if (fabsf(y) < 2.0f) {
                i_this->mD1D = 1;
            }
        } else {
            cLib_addCalc2(&i_this->current.pos.y, i_this->mAcch.m_ground_h + l_HIO().m18 + v->y, 0.1f, 5.0f);
            if (i_this->m572 == 0) {
                i_this->m572 = l_HIO().m10;
                if (cM_rndF(1.0f) < l_HIO().m14 && i_this->m582 == 0) {
                    i_this->mD1E = 0;
                    i_this->m571 = 1;
                    mt_set_back_pos(i_this, i_this->shape_angle.y, 30.0f, -30.0f);
                    anm_init(i_this, 8, 5.0f, 2, 1.0f, 0);
                    if (gabi::ea(&i_this->eyePos) != 0) {
                        mt_monsSe(i_this, 0x4802);
                    }
                }
            }
        }
        break;
    }
    case 1: {
        /* the lunge */
        if (daPy_procNo(player) == 0x10) {
            dAttention_c* att = dComIfGp_getAttention();
            if ((dAttention_LockonTruth(att) || (gabi::load<u32>(gabi::ea(att) + 0x20) & 0x20000000)) &&
                dAttention_ActionTarget(att, 0) == i_this) {
                /* grabbed by the player */
                i_this->m570 = 6;
                i_this->m572 = 0;
                anm_init(i_this, 7, 2.0f, 0, 1.0f, 0);
                break;
            }
        }
        i_this->mD1E = i_this->mD1E + 1;
        if (i_this->mD1E >= l_HIO().m30 && i_this->mD1E <= l_HIO().m32) {
            i_this->mBtStartFrame = 0.0f;
            i_this->mBtEndFrame = 20.0f;
            i_this->mBtMaxDis = 10000.0f;
            i_this->mBtAttackType = 3;
            i_this->mBtMaxDis = l_HIO().m34;
            i_this->mBtNowFrame = 10.0f;
        }
        if (i_this->mD1E == l_HIO().m3C) {
            anm_init(i_this, 9, 2.0f, 0, 1.0f, 0);
        }
        if (i_this->mD1E == l_HIO().m38) {
            mDoMtx_YrotS(calc_mtx(), i_this->m5B2);
            mDoMtx_XrotM(calc_mtx(), i_this->m5B0);
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = REG6_F(6) + -100.0f;
            v->z = REG6_F(7) + 200.0f;
            MtxPosition(v, &i_this->m598);
            PSVECAdd(&i_this->m598, &i_this->current.pos, &i_this->m598);
        }
        f32 rate, step;
        if (i_this->mD1E >= l_HIO().m38) {
            step = l_HIO().m2C;
            rate = 0.5f;
            if (i_this->mD1E == l_HIO().m3E) {
                anm_init(i_this, 7, 2.0f, 0, 1.0f, 0);
            }
        } else {
            step = REG0_F(11) + 4.0f;
            rate = 0.1f;
        }
        if (i_this->mD1E == (s16)(l_HIO().m3E + REG0_S(3))) {
            mt_monsSe(i_this, 0x5810);
        }
        if (i_this->mD1E < (s16)(l_HIO().m3E + REG6_S(7) + 3)) {
            i_this->mD20 = 1;
        }
        if (i_this->mD1E == (s16)(l_HIO().m3E + 2)) {
            i_this->m19F0 = 1;
        }
        if (i_this->mD1E >= (s16)(l_HIO().m3E + 2) && i_this->mD1E <= (s16)(l_HIO().m3E + 15)) {
            i_this->mD20 = 2;
        }
        cLib_addCalc2(&i_this->current.pos.x, i_this->m598.x, rate, step);
        cLib_addCalc2(&i_this->current.pos.y, i_this->m598.y + 20.0f, rate, step);
        cLib_addCalc2(&i_this->current.pos.z, i_this->m598.z, rate, step);
        i_this->speed.x = i_this->m598.x - i_this->current.pos.x;
        i_this->speed.y = -1.0f;
        i_this->speed.z = i_this->m598.z - i_this->current.pos.z;
        if (daPy_checkPlayerGuard(player) && i_this->mSph[0].ChkAtHit()) {
            /* bounced off the shield */
            i_this->m571 = 15;
            mt_set_back_pos(i_this, i_this->shape_angle.y, 80.0f, -120.0f);
            i_this->m57E = 10;
            i_this->m572 = 10;
            anm_init(i_this, 10, 2.0f, 2, 1.0f, 0);
            i_this->m19F0 = 0;
        }
        if (i_this->mD1E == l_HIO().m3A) {
            i_this->m571 = 0;
            i_this->m572 = l_HIO().m10;
            anm_init(i_this, 10, 5.0f, 2, 1.0f, 0);
        }
        break;
    }
    case 10: {
        f32 vy = i_this->speed.y;
        f32 ny = vy + i_this->gravity;
        i_this->current.pos.y = i_this->current.pos.y + vy;
        if (ny < -100.0f) {
            ny = -100.0f;
        }
        i_this->speed.y = ny;
        if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
            i_this->m571 = 0;
        }
        break;
    }
    case 15:
        cLib_addCalc2(&i_this->current.pos.x, i_this->m598.x, 0.5f, 20.0f);
        cLib_addCalc2(&i_this->current.pos.y, i_this->m598.y + 20.0f, 0.5f, 20.0f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->m598.z, 0.5f, 20.0f);
        i_this->speed.x = i_this->m598.x - i_this->current.pos.x;
        i_this->speed.y = -1.0f;
        i_this->speed.z = i_this->m598.z - i_this->current.pos.z;
        if (i_this->m572 == 0) {
            i_this->m571 = 0;
            i_this->m572 = l_HIO().m10;
            anm_init(i_this, 10, 5.0f, 2, 1.0f, 0);
        }
        break;
    case 0x11:
        if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
            i_this->m571 = 0x17;
            s16 t = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            i_this->m572 = t;
            i_this->m5AA = t;
        }
        mt_fall(i_this);
        break;
    case 0x14:
        /* knocked back */
        i_this->current.angle.x = (s16)(i_this->current.angle.x - 0x500);
        cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(cM_atan2s(i_this->speed.x, i_this->speed.z) + 0x8000), 2, 0x400);
        cLib_addCalc2(&i_this->m1A10, 0.9f, 1.0f, 0.1f);
        if (i_this->speed.y > 1.0f) {
            mt_fall(i_this);
        } else if (i_this->m1A14 != 0) {
            if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
                i_this->m1A14 = 2;
            }
            mt_fall(i_this);
        } else {
            /* roll up */
            i_this->mSph[0].mGObjTg.mSPrm &= ~1u;
            tex_anm_set(i_this, 1);
            f32 sy = i_this->speed.y;
            f32 sz = i_this->speed.z;
            f32 sx = i_this->speed.x;
            i_this->m570 = 2;
            f32 t = gabi::fmadds(sx, sx, sy * sy);
            i_this->m582 = l_HIO().m54;
            i_this->m594 = 0.0f;
            t = gabi::fmadds(sz, sz, t);
            i_this->m5A6 = 0;
            i_this->m44C = 0.0f;
            i_this->speedF = -std_sqrtf(t);
            i_this->m571 = 0;
            fopAcM_SetMin(i_this, -100.0f, -100.0f, -100.0f);
            fopAcM_SetMax(i_this, 100.0f, 100.0f, 100.0f);
            mt_fall(i_this);
        }
        break;
    case 0x17:
        i_this->speed.y = 0.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 1, 0x400);
        i_this->speed.x = i_this->speed.x * 0.2f;
        i_this->speed.z = i_this->speed.z * 0.2f;
        if (i_this->m572 == 0) {
            i_this->m571 = 10;
        }
        mt_fall(i_this);
        break;
    }

    if (i_this->m570 < 2) {
        if (i_this->m571 < 10) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m5B2, 2, 0x400);
            f32 h = i_this->current.pos.y - i_this->mAcch.m_ground_h;
            s16 pitch = i_this->m5B0;
            if (h > 250.0f) {
                i_this->m571 = 10;
            }
            cLib_addCalcAngleS2(&i_this->current.angle.x, (s16)(pitch + pitchOfs), 4, 0x800);
            cLib_addCalcAngleS2(&i_this->m584, 0, 1, 0x100);
        } else {
            cLib_addCalcAngleS2(&i_this->current.angle.x, 0x3800, 4, 0x800);
            cLib_addCalcAngleS2(&i_this->m584, 0, 1, 0x100);
        }
    }

    /* shake */
    f32 amp = (f32)(s32)i_this->m57E * (REG0_F(14) + 500.0f);
    i_this->m5B6.y = (s16)gabi::ftoi(sinShort_ool((s16)(i_this->m586 * 0x2100)) * amp);
    i_this->m5B6.x = (s16)gabi::ftoi(cosShort_ool((s16)(i_this->m586 * 0x2300)) * amp);
    csXyz_pl(&i_this->shape_angle, &i_this->current.angle, &i_this->m5B6);
    body_control2(i_this);
    body_wall_check(i_this);
    mt_bg_check(i_this);
    mt_cc_move(i_this);
}

/* the line of sight to the player, then the action of the state */
static inline void mt_action(mt_class* i_this, fopAc_ac_c* player) {
    i_this->mD20 = 0;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
    gabi::Local<cXyz> plPos;
    plPos->x = player->current.pos.x;
    plPos->y = player->current.pos.y + 20.0f;
    plPos->z = player->current.pos.z;
    gabi::Local<cXyz> myPos;
    myPos->x = i_this->current.pos.x;
    myPos->y = i_this->current.pos.y + 30.0f;
    myPos->z = i_this->current.pos.z;
    dBgS_LinChk_Set(linChk, myPos, plPos, i_this);
    i_this->m1A15 = (u8)cBgS_LineCross(dComIfG_Bgsp(), linChk);

    switch ((u8)i_this->m570) {
    case 0:
        mt_move(i_this);
        break;
    case 1:
        mt_fight(i_this);
        break;
    case 2:
        /* rolled up */
        mt_move_maru(i_this);
        body_control3(i_this);
        if (i_this->m582 > 100) {
            if (i_this->m1A16 != 0) {
                i_this->m582 = 0x47;
            }
        } else {
            body_wall_check(i_this);
        }
        mt_cc_move(i_this);
        if (i_this->m582 == 0x32) {
            /* unroll */
            i_this->m400 = 0;
            i_this->m1A17 = 2;
            i_this->m570 = 1;
            i_this->m571 = 0;
            i_this->health = 8;
            i_this->mSph[0].mGObjTg.mSPrm |= 1;
            i_this->m572 = l_HIO().m10;
            i_this->m5AA = 0;
            anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
            fopAcM_SetMin(i_this, -200.0f, -200.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 200.0f, 200.0f);
            i_this->mSph[0].mObjAt.mSPrm = (i_this->mSph[0].mObjAt.mSPrm & ~0xAu) | 4;
        }
        break;
    case 3:
        /* destroyed */
        i_this->m57C = 5;
        attention_flags_set(i_this, 0);
        body_control4(i_this);
        if (i_this->m576 == 1) {
            i_this->m1DD4 = 1;
            fopAcM_delete(i_this);
            dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        }
        break;
    case 6:
        /* carried by the player */
        if (daPy_procNo(player) == 0x10) {
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 0.0f;
            v->z = REG0_F(14) + -50.0f;
            gabi::Local<cXyz> t;
            MtxPosition(v, t);
            cLib_addCalc2(&i_this->current.pos.x, player->current.pos.x + t->x, 0.5f, 50.0f);
            cLib_addCalc2(&i_this->current.pos.z, player->current.pos.z + t->z, 0.5f, 50.0f);
        }
        i_this->m400 = 1;
        i_this->m580 = 3;
        i_this->m57C = 5;
        body_control5(i_this);
        {
            f32 vy = i_this->speed.y;
            i_this->current.pos.y = i_this->current.pos.y + vy;
            i_this->speed.y = vy + i_this->gravity;
        }
        mt_bg_check(i_this);
        if (i_this->m572 != 0) {
            i_this->m58A = 0;
            cLib_addCalc0(&i_this->m590, 0.05f, REG0_F(12) + 0.02f);
            if (i_this->m572 == 1) {
                bakuha(i_this);
            }
        } else if (daPy_procNo(player) != 0x10) {
            /* released */
            i_this->m400 = 0;
            i_this->m570 = 1;
            i_this->m571 = 0;
            i_this->mSph[0].mGObjTg.mSPrm |= 1;
            i_this->m572 = l_HIO().m10;
            i_this->m5AA = 0;
            anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
            i_this->mSph[0].mObjAt.mSPrm = (i_this->mSph[0].mObjAt.mSPrm & ~0xAu) | 4;
        } else {
            i_this->m590 = REG0_F(13) + 0.2f;
            if (i_this->mAtSph.ChkTgHit()) {
                i_this->health = 0;
                i_this->m572 = REG0_S(3) + 0x28;
                i_this->m590 = REG0_F(13) + 1.5f;
                mt_fopAcM_seStart(i_this, 0x2828, 0);
                mt_fopAcM_monsSeStart(i_this, 0x4803, 0);
                gabi::Local<CcAtInfo_l> atInfo;
                atInfo->mHitSoundId = 0;
                atInfo->mpObj = gabi::ea(i_this->mAtSph.GetTgHitObj());
                at_power_check(atInfo);
                mDoAud_onEnemyDamage();
                mDoAud_bgmHitSound(atInfo->mHitSoundId);
                gabi::store<u8>(0x101EACB7, (u8)(REG0_S(7) + 6));
                dComIfGp_particle_set(0x10, &i_this->current.pos);
                gabi::Local<csXyz> ang;
                gabi::Local<cXyz> scale;
                ang->x = 0;
                ang->z = 0;
                scale->z = 2.0f;
                scale->x = 2.0f;
                scale->y = 2.0f;
                ang->y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                dComIfGp_particle_set(0xD, &i_this->current.pos, ang, scale);
                gabi::Local<cXyz> pos;
                pos->x = i_this->current.pos.x;
                pos->y = i_this->current.pos.y;
                pos->z = i_this->current.pos.z;
                dKy_SordFlush_set(pos, 1);
                anm_init(i_this, 8, 5.0f, 0, 1.0f, 0);
            }
        }
        break;
    }

    if (i_this->m582 > 60) {
        cLib_addCalc0(&i_this->mHeadScale, 1.0f, 0.05f);
    } else {
        cLib_addCalc2(&i_this->mHeadScale, 1.0f, 1.0f, 0.1f);
    }
    dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
}

/* keep the head out of the background between the old and the new position */
static inline void mt_bg_line_check(mt_class* i_this) {
    f32 h = 40.0f;
    gabi::Local<dBgS_LinChk_l> chkUp;
    dBgS_ObjLinChk_ct(chkUp);
    gabi::Local<cXyz> start;
    start->copy(i_this->old.pos);
    gabi::Local<cXyz> end;
    end->x = i_this->old.pos.x;
    end->y = i_this->old.pos.y + 40.0f;
    end->z = i_this->old.pos.z;
    dBgS_LinChk_Set(chkUp, start, end, i_this);
    BOOL roof = cBgS_LineCross(dComIfG_Bgsp(), chkUp);
    f32 oy = i_this->old.pos.y;
    if (roof) {
        f32 t = LinChk_GetCross(chkUp)->y - 1.0f - oy;
        h = t >= 0.0f ? t : 0.0f;
    }
    gabi::Local<cXyz> oldPos;
    oldPos->x = i_this->old.pos.x;
    oldPos->y = oy + h;
    oldPos->z = i_this->old.pos.z;
    gabi::Local<cXyz> newPos;
    newPos->x = i_this->current.pos.x;
    newPos->y = i_this->current.pos.y + h;
    newPos->z = i_this->current.pos.z;

    gabi::Local<dBgS_LinChk_l> chkFwd;
    dBgS_ObjLinChk_ct(chkFwd);
    dBgS_LinChk_Set(chkFwd, oldPos, newPos, i_this);
    BOOL fwd = cBgS_LineCross(dComIfG_Bgsp(), chkFwd);
    gabi::Local<dBgS_LinChk_l> chkBack;
    dBgS_ObjLinChk_ct(chkBack);
    dBgS_LinChk_Set(chkBack, newPos, oldPos, i_this);
    BOOL back = cBgS_LineCross(dComIfG_Bgsp(), chkBack);
    if (fwd && !back) {
        /* went through a wall: back to the wall, pushed out along its normal */
        i_this->current.pos.copy(*LinChk_GetCross(chkFwd));
        u32 pi = gabi::ea(chkFwd.get()) + 0x14;
        cXyz* pla = (cXyz*)cBgS_GetTriPla(dComIfG_Bgsp(), gabi::load<u16>(pi + 2), gabi::load<u16>(pi));
        if (pla != nullptr) {
            i_this->current.pos.x = i_this->current.pos.x + pla->x;
            i_this->current.pos.y = i_this->current.pos.y + pla->y;
            i_this->current.pos.z = i_this->current.pos.z + pla->z;
        }
    }
    dBgS_LinChk_dt(chkBack, MT_LINCHK_VTBLS);
    dBgS_LinChk_dt(chkFwd, MT_LINCHK_VTBLS);
    dBgS_LinChk_dt(chkUp, MT_LINCHK_VTBLS);
}

/* 021D87A0 */
BOOL daMt_Execute(mt_class* i_this) {
    WWHD_FUNC(0x021D87A0, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->m3D5 != 0 && i_this->current.pos.y < i_this->home.pos.y - 5000.0f) {
        /* fell out of the room: counts as defeated */
        dComIfGs_onSwitch(i_this->m3D5, i_this->current.roomNo);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        fopAcM_delete(i_this);
        return TRUE;
    }
    if (enemy_ice(&i_this->mEnemyIce)) {
        return TRUE;
    }
    if (i_this->m3D7 != 0) {
        if (!dComIfGs_isSwitch(i_this->m3D7 - 1, i_this->current.roomNo)) {
            return TRUE;
        }
        i_this->m3D7 = 0;
    }
    i_this->actor_status |= 0x20;

    /* lava */
    gabi::Local<dBgS_GndChk_l> gndChk;
    dBgS_ObjGndChk_Yogan_ct(gndChk);
    f32 y = i_this->current.pos.y;
    f32 z = i_this->current.pos.z;
    f32 x = i_this->current.pos.x;
    gndChk->m_pos.x = x;
    gndChk->m_pos.y = y + 200.0f;
    gndChk->m_pos.z = z;
    f32 lava = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    if (lava != -1000000000.0f && i_this->current.pos.y - 30.0f + REG0_F(13) < lava) {
        if (i_this->m1A16 == 0) {
            i_this->speedF = i_this->speedF * 0.1f;
            i_this->speed.y = 0.0f;
            gabi::Local<cXyz> pos;
            pos->x = x;
            pos->z = z;
            pos->y = lava;
            fopKyM_createMpillar(pos, 0.5f);
        }
        i_this->m1A16 = 1;
        i_this->gravity = -0.5f;
        if (i_this->speed.y < -5.0f) {
            i_this->speed.y = -5.0f;
        }
    } else {
        i_this->m1A16 = 0;
        i_this->gravity = -3.0f;
    }
    attention_flags_set(i_this, 4);

    mt_eye_tex_anm(i_this);

    if (l_HIO().mNo == 0) {
        if (i_this->m19F0 != 0) {
            u8 n = (u8)(i_this->m19F0 + 1);
            if (n == 11) {
                i_this->m19F0 = 0;
            } else {
                i_this->m19F0 = n;
            }
        }
        i_this->mBtStartFrame = 100.0f;
        i_this->mBtAttackType = 0;
        i_this->mBtEndFrame = 100.0f;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtNowFrame = 0.0f;
        for (int i = 0; i < 5; i++) {
            be<s16>* t = &(&i_this->m572)[i];
            if (*t != 0) {
                *t = *t - 1;
            }
        }
        if (i_this->m1A18 != 0) i_this->m1A18 = i_this->m1A18 - 1;
        if (i_this->m57C != 0) i_this->m57C = i_this->m57C - 1;
        if (i_this->m57E != 0) i_this->m57E = i_this->m57E - 1;
        if (i_this->m466 != 0) i_this->m466 = i_this->m466 - 1;
        if (i_this->m5AA != 0) {
            i_this->m5AA = i_this->m5AA - 1;
            i_this->m5AC = i_this->m5AC + 1;
        } else if (i_this->m5AC != 0) {
            s16 n = i_this->m5AC;
            if (n > 15) n = 15;
            i_this->m5AC = n - 1;
        }
        i_this->m586 = i_this->m586 + 1;
        if (i_this->m582 != 0) i_this->m582 = i_this->m582 - 1;

        if (i_this->m570 < 3) {
            damage_check(i_this);
        }
        mt_action(i_this, player);
    }

    mt_bg_line_check(i_this);
    attention_pos(i_this)->copy(i_this->eyePos);

    s32 step = 1, lo, hi, bhi;
    switch ((u8)i_this->m400) {
    default:
        JUT_ASSERT_fail(STR(0x10015740), 0x108B, STR(0x1001573C));
        /* fall through */
    case 0:
        lo = 0;
        hi = 40;
        bhi = 30;
        break;
    case 1:
        step = 2;
        lo = 40;
        hi = 100;
        bhi = 90;
        break;
    case 2:
        lo = 100;
        hi = 130;
        bhi = 120;
        break;
    }
    i_this->m404 = i_this->m404 + step;
    if (i_this->m404 > hi) {
        i_this->m404 = i_this->m400 != 0 ? hi : lo;
    }
    i_this->m408 = i_this->m408 + step;
    if (i_this->m408 > bhi) {
        i_this->m408 = i_this->m400 != 0 ? bhi : 0;
    }

    /* joint waves */
    csXyz* jnt = &i_this->m75C[0]; /* m75C[15] and m7B6[15] */
    if (i_this->m570 >= 2 && i_this->m582 > 120) {
        i_this->m450 = 0;
        for (int i = 0; i < 30; i++) {
            jnt[i].x = 0;
            cLib_addCalcAngleS2(&jnt[i].z, 10000, 10, 300);
        }
    } else {
        if (i_this->m582 == 120) {
            i_this->m5AC = 15;
        }
        i_this->mpMorf[0]->play(&i_this->current.pos, 0, 0);
        i_this->m58A = i_this->m58A + l_HIO().m08;
        for (int i = 0; i < 30; i++) {
            int j = i >= 15 ? i - 15 : i;
            bool still = i_this->m5AA != 0 ? j < i_this->m5AC : 14 - j < i_this->m5AC;
            if (still) {
                jnt[i].x = (s16)gabi::ftoi(-(cM_ssin(i * (REG6_S(2) + 0x32C8)) * (REG6_F(11) + 5000.0f)));
                f32 c = cM_scos(i * (REG6_S(3) + 0x32C8));
                s16 t = (s16)gabi::ftoi(gabi::fmadds(c, REG6_F(12) + 5000.0f, l_HIO().m0C));
                cLib_addCalcAngleS2(&jnt[i].z, t, 1, i_this->m450);
            } else {
                s16 ph = i_this->m58A;
                jnt[i].x = (s16)gabi::ftoi(-(cM_ssin(ph + i * (REG6_S(2) + 0x32C8)) * (REG6_F(11) + 5000.0f)));
                f32 c = cM_scos(ph + i * (REG6_S(3) + 0x32C8));
                s16 t = (s16)gabi::ftoi(gabi::fmadds(c, REG6_F(12) + 5000.0f, l_HIO().m0C));
                cLib_addCalcAngleS2(&jnt[i].z, t, 1, i_this->m450);
            }
        }
        cLib_addCalcAngleS2(&i_this->m450, 0x2000, 1, 0x100);
        for (int k = 0; k < MT_PART_NUM; k++) {
            f32 s = cM_ssin(i_this->m586 * (REG0_S(5) + 0x9C4) + k * (REG0_S(6) + 0x1D4C));
            i_this->m73C[k] = gabi::fmadds(s, i_this->m454, 1.0f);
        }
        cLib_addCalc2(&i_this->m454, 0.1f, 1.0f, 0.002f);
    }

    if (!(i_this->m3D0 & 1)) {
        /* lava drips and glow along the body */
        for (int i = 0; i < MT_PART_NUM; i++) {
            J3DModel* model = i_this->mpMorf[i]->getModel();
            gabi::Local<cXyz> v;
            v->y = REG0_F(6);
            v->z = REG0_F(7);
            v->x = REG0_F(5);
            cXyz* pos = &i_this->m46C[i];
            PSMTXMultVec(J3DModel_getBaseTRMtx(model), v, pos);
            u8 st = i_this->m570;
            if ((st >= 2 && (i_this->m582 > 40 || st == 6)) || l_HIO().m07 != 0) {
                pos->y = pos->y + 10000.0f;
            }
            if (i < 7) {
                dComIfGp_particle_setSimple(0x8058, pos);
            }
            if ((u32)(i - 1) < 6) {
                dComIfGp_particle_setSimple(0x8059, pos);
            }
            if (i_this->m468 == 0 && (i == 1 || i == 3 || i == 5)) {
                dPa_control_set(dComIfGp_getParticle(), 4, 0xC06C, pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)&i_this->mFollowCb[i], -1, nullptr, nullptr, nullptr);
            }
        }
        i_this->m468 = 1;
    }

    if (i_this->m570 < 2 && i_this->m1A16 == 0 && i_this->m5AA == 0) {
        if (i_this->m578 == 0) {
            i_this->m578 = (s16)gabi::ftoi(cM_rndF(45.0f) + 45.0f);
            mt_monsSe(i_this, 0x4801);
        }
        if (i_this->m57A == 0) {
            i_this->m57A = (s16)gabi::ftoi(cM_rndF(3.0f) + 6.0f);
            mt_monsSe(i_this, 0x580F);
        }
    }
    if (i_this->m1A14 >= 2) {
        bakuha(i_this);
        i_this->m1A14 = -1;
    }
    dBgS_ObjGndChk_Yogan_dt(gndChk);
    return TRUE;
}
VERIFY(0x021D87A0, daMt_Execute);
