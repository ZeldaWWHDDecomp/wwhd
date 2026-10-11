/**
 * d_a_att.cpp (WWHD)
 * Attention target for the Puppet Ganon (bgn) strings and body: forwards hits to the boss.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_att.cpp, USA version) to the WWHD layout and code, verified
 * against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_att.h"

enum { JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_CM_BGN_D_STRING_PLINK = 0x5970 };
static bgn_att_view_c* boss;
static cXyz non_pos;

// HD: source collision records stay packed words because shared source types are opaque.
static const u32 sita_sph_src[] = {
    0, 0, 0, 5, 32768, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1121058816
};
static const u32 bm_sph_src[] = {
    0, 0, 0, 5, 4280155903, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1128792064
};
static const u32 cc_cyl_src[] = {
    0, 0, 0, 0, 4280155903, 3, 117, 0, 0, 0, 0, 0, 0, 0, 0, 1128792064, 1157234688
};

// HD: placement construction uses this unit's cylinder AAB vtable.
att_class::att_class() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x10007704;
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, 0x100076F4);
    dCcD_Sph_construct(&mSph);
}

/* 0205875C */
static BOOL daAtt_Draw(att_class* i_this) {
    return TRUE;
}

/* 02058764 */
static void* boss_s_sub(void* search, void* param) {
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BGN_e) {
        return search;
    }
    return nullptr;
}

/* 020587B4 */
static BOOL daAtt_Execute(att_class* i_this) {
    if (i_this->m2B5 == 101) {
        i_this->eyePos.copy(i_this->current.pos);
        i_this->attention_info.position.copy(i_this->current.pos);
        /* fopAcM_OffStatus(i_this, 0): no effect */
        i_this->attention_info.flags = 0;
        i_this->mSph.SetC(&i_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
    }

    if (i_this->m2B5 == 100) {
        i_this->eyePos.copy(i_this->current.pos);
        i_this->attention_info.position.copy(i_this->current.pos);
        return TRUE;
    }

    boss = (bgn_att_view_c*)fpcM_Search(boss_s_sub, i_this);
    if (boss == nullptr) {
        return TRUE;
    }
    int r30 = i_this->m2B5;

    bool hitCheck = true;
    if (i_this->m550 != 0) {
        i_this->m550 = (s8)(i_this->m550 - 1);
        if (i_this->m550 != 0)
            hitCheck = false;
    }

    if (hitCheck && (i_this->mCyl.ChkTgHit() || i_this->mSph.ChkTgHit())) {
        i_this->m550 = 10;
        bgn_att_move_s* p = &boss->mAAA8[r30];
        p->m308 = (s8)(p->m308 - 1);
        p = &boss->mAAA8[r30];
        if (p->m308 <= 0) {
            p->m2D0 = 1;
            boss->mAAA8[r30].m304 = 6000.0f;
            mDoAud_seStart(JA_SE_LK_W_WEP_HIT, nullptr, 0x21, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            if (r30 <= 1) {
                if (boss->mAAA8[1 - r30].m2D0 != 0) {
                    boss->mC7AC[2] = 600;
                }
            }
        } else {
            p->m300 = 15;
            mDoAud_seStart(JA_SE_CM_BGN_D_STRING_PLINK, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        }
    }

    i_this->current.pos.copy(boss->mC33C[r30]);
    gabi::Local<cXyz> sp08;
    sp08->x = i_this->current.pos.x;
    sp08->y = i_this->current.pos.y;
    sp08->z = i_this->current.pos.z;

    bgn_att_view_c* b = boss;
    if (b->m02B5 == 0 &&
        (b->mC748 != 0 || b->mC74C != 0 || r30 != 7) &&
        b->mAAA8[r30].m2D0 == 0 &&
        b->mAAA8[r30].m2EC < 1.0f) {
        i_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        i_this->mCyl.SetR(200.0f);
        i_this->mCyl.SetC(sp08);
        sp08->y = sp08->y + 1000.0f;
        i_this->mSph.SetC(sp08);
        i_this->eyePos.copy(*sp08);
        i_this->attention_info.position.copy(*sp08);
    } else {
        /* fopAcM_OffStatus(i_this, 0): no effect */
        i_this->attention_info.flags = 0;
        i_this->mCyl.SetC(&non_pos);
        i_this->mSph.SetC(&non_pos);
    }

    dComIfG_Ccsp_Set(&i_this->mCyl);
    dComIfG_Ccsp_Set(&i_this->mSph);
    return TRUE;
}

/* 02058B28 */
static BOOL daAtt_IsDelete(att_class* i_this) {
    return TRUE;
}

/* 02058B30 */
static BOOL daAtt_Delete(att_class* i_this) {
    return TRUE;
}

/* 02058B38 */
static cPhs_State daAtt_Create(fopAc_ac_c* i_this) {
    att_class* a_this = (att_class*)i_this;
    fopAcM_ct(i_this, att_class);
    dComIfGp_get(); /* HD: a leftover accessor call (result unused) */

    a_this->m2B5 = fopAcM_GetParam(a_this) & 0xFF;
    a_this->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 4;

    if (a_this->m2B5 == 101) {
        a_this->actor_status |= fopAcStts_UNK10000000_e;
        a_this->mStts.Init(0xFF, 0xFF, a_this);
        a_this->mSph.Set((const dCcD_SrcSph*)sita_sph_src);
        a_this->mSph.SetStts(&a_this->mStts);
    }

    if (a_this->m2B5 < 10) {
        boss = nullptr;
        a_this->mStts.Init(0xFF, 0xFF, a_this);
        a_this->mCyl.Set((const dCcD_SrcCyl*)cc_cyl_src);
        a_this->mCyl.SetStts(&a_this->mStts);
        a_this->mSph.Set((const dCcD_SrcSph*)bm_sph_src);
        a_this->mSph.SetStts(&a_this->mStts);
    }

    return cPhs_COMPLEATE_e;
}

// HD: static initialisation (header statics, then non_pos)
/* 02058CCC */
static void __sinit_d_a_att_cpp() {
    sinit_header_statics(0x104614F0, 0x101900FC);
    non_pos.x = -30000.0f;
    non_pos.y = -30000.0f;
    non_pos.z = -30000.0f;
}

// HD: att_class deleting destructor (compiler-generated, HD virtual destructor)
/* 02058D78 */
static void att_class_dt(att_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Sph_destruct(&i_this->mSph, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data sita_sph_src 10190038
data bm_sph_src 10190078
data cc_cyl_src 101900B8
data boss 104614EC
data non_pos 1046150C
ext 025166F0 void dCcD_Sph_construct(dCcD_Sph*)
ext 02515AE8 void dCcD_Sph_destruct(dCcD_Sph*, s32)
*/
