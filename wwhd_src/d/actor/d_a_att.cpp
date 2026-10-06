/**
 * d_a_att.cpp (WWHD)
 * Attention target for the Puppet Ganon (bgn) strings and body: forwards hits to the boss.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_att.cpp, USA version) to the WWHD layout and code, verified
 * against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ATT_VTBL 0x10007704      /* att_class vtable (HD virtual destructor) */
#define ATT_AAB_VTBL 0x100076F4  /* this TU's cM3dGAab vtable */
#define sita_sph_src gabi::at<dCcD_SrcSph>(0x10190038)
#define bm_sph_src gabi::at<dCcD_SrcSph>(0x10190078)
#define cc_cyl_src gabi::at<dCcD_SrcCyl>(0x101900B8)

enum { fpcNm_BGN_e = 0xF3 };
enum { JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_CM_BGN_D_STRING_PLINK = 0x5970 };
enum { fopAc_Attn_LOCKON_BATTLE_e = 4 };

/* bgn_class (Puppet Ganon): only the fields att uses. HD offsets: m02B5 0x3D5,
 * mAAA8[] at 0x12C20 (0x30C each), mC33C[] at 0x144B4, mC748/mC74C 0x14EB4/0x14EB8,
 * mC7AC[2] 0x14F1C. */
enum : u32 {
    BGN_M02B5 = 0x3D5,
    BGN_PART = 0x12C20, BGN_PART_SIZE = 0x30C,
    PART_M2D0 = 0x2D0, PART_M2EC = 0x2EC, PART_M300 = 0x300, PART_M304 = 0x304, PART_M308 = 0x308,
    BGN_MC33C = 0x12C20 + 0x1894,
    BGN_MC748 = 0x12C20 + 0x2294, BGN_MC74C = 0x12C20 + 0x2298,
    BGN_MC7AC_2 = 0x12C20 + 0x22FC,
};

struct att_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ u8 m298[0x3D1 - 0x3B4];
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ dCcD_Stts mStts;
    /* 0x410 */ dCcD_Cyl mCyl;
    /* 0x540 */ dCcD_Sph mSph;
    /* 0x66C */ be<s8> m550;
    /* 0x66D */ u8 _66D[3];
};
WWHD_OFFSET(att_class, mStts, 0x3D4);
WWHD_OFFSET(att_class, mCyl, 0x410);
WWHD_OFFSET(att_class, mSph, 0x540);
WWHD_OFFSET(att_class, m550, 0x66C);
WWHD_SIZE(att_class, 0x670);

/* static bgn_class* boss; static cXyz non_pos(-30000, -30000, -30000); */
static inline be<u32>& boss() { return *gabi::at<be<u32>>(0x104614EC); }
#define non_pos gabi::at<cXyz>(0x1046150C)

/* attention_info (fopAc_ac_c +0x388): distances[] +0x388, position +0x390, flags +0x39C */
static inline cXyz* attention_position(fopAc_ac_c* a) { return gabi::at<cXyz>(gabi::ea(a) + 0x390); }
static inline void attention_flags_set(fopAc_ac_c* a, u32 f) { gabi::store<u32>(gabi::ea(a) + 0x39C, f); }

/* 0205875C */
static BOOL daAtt_Draw(att_class* i_this) {
    WWHD_FUNC(0x0205875C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0205875C, daAtt_Draw);

/* 02058764 */
static void* boss_s_sub(void* search, void* param) {
    WWHD_FUNC(0x02058764, void*, search, param);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BGN_e) {
        return search;
    }
    return nullptr;
}
VERIFY(0x02058764, boss_s_sub);

/* 020587B4 */
static BOOL daAtt_Execute(att_class* i_this) {
    WWHD_FUNC(0x020587B4, BOOL, i_this);
    if (i_this->m2B5 == 101) {
        i_this->eyePos.copy(i_this->current.pos);
        attention_position(i_this)->copy(i_this->current.pos);
        /* fopAcM_OffStatus(i_this, 0): no effect */
        attention_flags_set(i_this, 0);
        i_this->mSph.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
    }

    if (i_this->m2B5 == 100) {
        i_this->eyePos.copy(i_this->current.pos);
        attention_position(i_this)->copy(i_this->current.pos);
        return TRUE;
    }

    boss() = gabi::ea(fpcM_Search(0x02058764 /* boss_s_sub */, i_this));
    if (boss() == 0) {
        return TRUE;
    }
    int r30 = i_this->m2B5;
    u32 part = BGN_PART + r30 * BGN_PART_SIZE;

    bool hitCheck = true;
    if (i_this->m550 != 0) {
        i_this->m550 = (s8)(i_this->m550 - 1);
        if (i_this->m550 != 0)
            hitCheck = false;
    }

    if (hitCheck && (i_this->mCyl.ChkTgHit() || i_this->mSph.ChkTgHit())) {
        i_this->m550 = 10;
        u32 p = boss() + part;
        gabi::store<u8>(p + PART_M308, (u8)(gabi::load<u8>(p + PART_M308) - 1));
        p = boss() + part;
        if (gabi::load<s8>(p + PART_M308) <= 0) {
            gabi::store<u8>(p + PART_M2D0, 1);
            gabi::store<f32>(boss() + part + PART_M304, 6000.0f);
            mDoAud_seStart(JA_SE_LK_W_WEP_HIT, nullptr, 0x21, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            if (r30 <= 1) {
                if (gabi::load<u8>(boss() + BGN_PART + (1 - r30) * BGN_PART_SIZE + PART_M2D0) != 0) {
                    gabi::store<s16>(boss() + BGN_MC7AC_2, 600);
                }
            }
        } else {
            gabi::store<s16>(p + PART_M300, 15);
            mDoAud_seStart(JA_SE_CM_BGN_D_STRING_PLINK, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        }
    }

    i_this->current.pos.copy(*gabi::at<cXyz>(boss() + BGN_MC33C + r30 * 0xC));
    gabi::Local<cXyz> sp08;
    sp08->x = i_this->current.pos.x;
    sp08->y = i_this->current.pos.y;
    sp08->z = i_this->current.pos.z;

    u32 b = boss();
    if (gabi::load<s8>(b + BGN_M02B5) == 0 &&
        (gabi::load<s16>(b + BGN_MC748) != 0 || gabi::load<s16>(b + BGN_MC74C) != 0 || r30 != 7) &&
        gabi::load<u8>(b + part + PART_M2D0) == 0 &&
        gabi::load<f32>(b + part + PART_M2EC) < 1.0f) {
        attention_flags_set(i_this, fopAc_Attn_LOCKON_BATTLE_e);
        i_this->mCyl.SetR(200.0f);
        i_this->mCyl.SetC(sp08);
        sp08->y = sp08->y + 1000.0f;
        i_this->mSph.SetC(sp08);
        i_this->eyePos.copy(*sp08);
        attention_position(i_this)->copy(*sp08);
    } else {
        /* fopAcM_OffStatus(i_this, 0): no effect */
        attention_flags_set(i_this, 0);
        i_this->mCyl.SetC(non_pos);
        i_this->mSph.SetC(non_pos);
    }

    dComIfG_Ccsp_Set(&i_this->mCyl);
    dComIfG_Ccsp_Set(&i_this->mSph);
    return TRUE;
}
VERIFY(0x020587B4, daAtt_Execute);

/* 02058B28 */
static BOOL daAtt_IsDelete(att_class* i_this) {
    WWHD_FUNC(0x02058B28, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02058B28, daAtt_IsDelete);

/* 02058B30 */
static BOOL daAtt_Delete(att_class* i_this) {
    WWHD_FUNC(0x02058B30, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02058B30, daAtt_Delete);

/* 02058B38 */
static cPhs_State daAtt_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02058B38, cPhs_State, i_this);
    att_class* a_this = (att_class*)i_this;
    /* fopAcM_ct(i_this, att_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = ATT_VTBL;
            dCcD_Stts_ct(&a_this->mStts);
            dCcD_Cyl_ct(&a_this->mCyl, ATT_AAB_VTBL);
            gabi::call(0x025166F0, &a_this->mSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    dComIfGp_get(); /* HD: a leftover accessor call (result unused) */

    a_this->m2B5 = fopAcM_GetParam(a_this) & 0xFF;
    gabi::store<u8>(gabi::ea(a_this) + 0x38A, 4); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */

    if (a_this->m2B5 == 101) {
        a_this->actor_status |= 0x10000000; /* fopAcStts_UNK10000000_e */
        a_this->mStts.Init(0xFF, 0xFF, a_this);
        a_this->mSph.Set(sita_sph_src);
        a_this->mSph.SetStts(&a_this->mStts);
    }

    if (a_this->m2B5 < 10) {
        boss() = 0;
        a_this->mStts.Init(0xFF, 0xFF, a_this);
        a_this->mCyl.Set(cc_cyl_src);
        a_this->mCyl.SetStts(&a_this->mStts);
        a_this->mSph.Set(bm_sph_src);
        a_this->mSph.SetStts(&a_this->mStts);
    }

    return cPhs_COMPLEATE_e;
}
VERIFY(0x02058B38, daAtt_Create);

/* 02058CCC: static initialisation (header statics, then non_pos) */
static void __sinit_d_a_att_cpp() {
    WWHD_FUNC(0x02058CCC, void, (u32)0);
    sinit_header_statics(0x104614F0, 0x101900FC);
    non_pos->x = -30000.0f;
    non_pos->y = -30000.0f;
    non_pos->z = -30000.0f;
}
VERIFY(0x02058CCC, __sinit_d_a_att_cpp);

/* 02058D78: att_class deleting destructor (compiler-generated, HD virtual destructor) */
static void att_class_dt(att_class* i_this, s32 flags) {
    WWHD_FUNC(0x02058D78, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02058D78, att_class_dt);
