/**
 * d_a_mo2_move.cpp (WWHD)
 * Enemy - Moblin: Mo2_move (021D0A14) with jyunkai, fight_run, fight and yari_hit_check inlined.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mo2.h"
#include <cmath>

/* ---- HD layout around m05C0 (the header's m05C4/m05D0 names are off by 4 here) ----
 * 0x864 m05C0 (distance to the target, XZ), 0x868 HD: |dy| to the target, 0x86C m05C4 (cXyz). */
static inline be<f32>& m05C0_dy(mo2_class* i) { return *gabi::at<be<f32>>(gabi::ea(i) + 0x868); }

/* ---- other mo2 functions (verified separately): called by address ---- */
static void tex_anm_set(mo2_class* i, u16 idx) { gabi::call(0x021C6368, i, idx); }
static void anm_init(mo2_class* i, int bck, f32 morf, u8 loopMode, f32 speed, int snd) {
    gabi::call(0x021C6440, i, bck, morf, loopMode, speed, snd);
}
static void way_pos_check(mo2_class* i, cXyz* p) { gabi::call(0x021C7C30, i, p); }
static u32 ground_4_check(mo2_class* i, int n, s16 a, f32 r) { return gabi::call<u32>(0x021C7EDC, i, n, a, r); }
static s32 daMo2_wepon_view_check(mo2_class* i) { return gabi::call<s32>(0x021C872C, i); }
static s32 daMo2_bomb_view_check(mo2_class* i) { return gabi::call<s32>(0x021C89FC, i); }
static s32 daMo2_player_bg_check(mo2_class* i, cXyz* p) { return gabi::call<s32>(0x021C8A38, i, p); }
static BOOL daMo2_player_view_check(mo2_class* i, cXyz* p, s16 a, s16 b) { return gabi::call<BOOL>(0x021C8C14, i, p, a, b); }
static s32 daMo2_player_way_check(mo2_class* i) { return gabi::call<s32>(0x021C8DD4, i); }
static void wait_set(mo2_class* i) { gabi::call(0x021C8E30, i); }
static void fight_run_set(mo2_class* i) { gabi::call(0x021C8FB8, i); }
static void path_check(mo2_class* i) { gabi::call(0x021C9004, i); }
static void attack_set(mo2_class* i, u8 p) { gabi::call(0x021C9310, i, p); }
static void nage(mo2_class* i) { gabi::call(0x021CD958, i); }
static void p_lost(mo2_class* i) { gabi::call(0x021CDE6C, i); }
static void b_nige(mo2_class* i) { gabi::call(0x021CE138, i); }
static void defence(mo2_class* i) { gabi::call(0x021CE46C, i); }
static void oshi(mo2_class* i) { gabi::call(0x021CE5B8, i); }
static void hukki(mo2_class* i) { gabi::call(0x021CE6E8, i); }
static void aite_miru(mo2_class* i) { gabi::call(0x021CEC44, i); }
static void fail(mo2_class* i) { gabi::call(0x021CED78, i); }
static void yogan_fail(mo2_class* i) { gabi::call(0x021CEEB0, i); }
static void wepon_search(mo2_class* i) { gabi::call(0x021CF0F4, i); }
static void hip_damage(mo2_class* i) { gabi::call(0x021CF900, i); }
static void d_mahi(mo2_class* i) { gabi::call(0x021CFBBC, i); }
static void d_sit(mo2_class* i) { gabi::call(0x021CFFA4, i); }
static void d_dozou(mo2_class* i) { gabi::call(0x021D0064, i); }
static void carry(mo2_class* i) { gabi::call(0x021D0284, i); }
static void carry_drop(mo2_class* i) { gabi::call(0x021D0294, i); }
static void e3_demo(mo2_class* i) { gabi::call(0x021D0570, i); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200796C CPad stick X of a port (f1) */
static inline f32 CPad_GET_STICK_POS_X(u32 port) { return gabi::call<f32>(0x0200796C, port); }
/* 0255F530 dKy_Sound_get(): SND_INFLUENCE* (position first) */
static inline u8* dKy_Sound_get() { return gabi::call<u8*>(0x0255F530); }
/* 02518B28 cc_pl_cut_bit_get() */
static inline u32 cc_pl_cut_bit_get() { return gabi::call<u32>(0x02518B28); }
/* daPy_py_c::getCutType(): byte at +0x3AC (HD) */
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
/* dAttention_c at play+0x5804: Lockon() = LockonTruth() || flag 0x20000000 at +0x20; LockonTarget(0) */
static inline BOOL dAttention_LockonTruth(u32 att) { return gabi::call<BOOL>(0x024EDFCC, att); }
static inline fopAc_ac_c* dAttention_LockonTarget(u32 att, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, att, i); }
/* dCcD_Sph::StartCAt / MoveCAt (cXyz&) */
static inline void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167C0, s, p); }
static inline void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167E4, s, p); }
/* 02516C14 dCcMassS_Mng::Set(obj, u8 priority); the mass manager is at play+0x4EF8 */
static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio); }
/* dCcD_GObjInf::GetAtHitObj */
static inline void* GetAtHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x02516178, o); }
/* dCc_GetAc(obj->GetAc()) (HD inline): the hit object's stts (+0x44) actor (+0xC) */
static inline fopAc_ac_c* hit_obj_actor(void* obj) {
    if (obj == nullptr) return nullptr;
    u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
    if (stts == 0) return nullptr;
    return gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC));
}
static inline s32 mo2_attack_AP(s32 i) { return gabi::load<s32>(0x101BA6F4 + 4 * i); }
static inline f32 br_set_tm(s32 i) { return gabi::load<f32>(0x101BA70C + 4 * i); }
static inline u32 mo2_at_kind(s32 i) { return gabi::load<u32>(0x101BA694 + 4 * i); }
static inline u32 mo2_at_sm_kind(s32 i) { return gabi::load<u32>(0x101BA6AC + 4 * i); }
static inline s32 mo2_attack_go_SE(s32 i) { return gabi::load<s32>(0x101BA6DC + 4 * i); }
enum { fpcNm_MO2_e = 0xBC };
enum {
    JA_SE_CV_MO_FIND_ENEMY = 0x480F, JA_SE_CV_MO_LOSE_LANCE = 0x4810, JA_SE_CM_LANCE_HIT_FLOOR = 0x5808,
    JA_SE_LK_ROPE_HOOK_METAL = 0x281D,
};
enum { AT_TYPE_UNK8 = 0x8, AT_TYPE_UNK800 = 0x800 };

static inline bool has_weapon(mo2_class* i) { return i->mbHasInnateWeapon != 0 || i->m2943 != 0; }

static inline void to_jyunkai(mo2_class* i, s16 mode) {
    i->mDamageReaction.mAction = ACTION_JYUNKAI;
    path_check(i);
    wait_set(i);
    i->mDamageReaction.mMode = mode;
}

static inline void set_m05A4_rnd(mo2_class* i, int k, f32 r, f32 b) { i->m05A4[k] = (s16)gabi::ftoi(cM_rndF(r) + b); }

/* path_check2 (inlined) */
static inline void path_check2(mo2_class* i_this) {
    gabi::Local<dBgS_LinChk_l> chk;
    dBgS_LinChk_ct(chk, LINCHK_VTBLS);
    gabi::Local<cXyz> local_9c;
    gabi::Local<cXyz> local_a8;
    *local_9c = i_this->current.pos.get();
    local_9c->y = i_this->current.pos.y + (REG_F(13, 2) + 20.0f);
    *local_a8 = i_this->m05C4.get();
    local_a8->y = i_this->m05C4.y + (REG_F(13, 3) + 10.0f);
    dBgS_LinChk_Set(chk, local_9c, local_a8, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
        i_this->m2968 = 0;
    }
    dBgS_LinChk_dt(chk, LINCHK_VTBLS);
}

/* walk_set (inlined) */
static inline void walk_set(mo2_class* i_this) {
    if (i_this->mbHasInnateWeapon != 0) {
        if (i_this->mMode == 1) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_KWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KWALK_e);
            return;
        }
        anm_init(i_this, dRes_INDEX_MO2_BCK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_WALK_e);
        return;
    }
    if (i_this->m2943 == 0) {
        anm_init(i_this, dRes_INDEX_MO2_BCK_NWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NWALK_e);
    } else {
        anm_init(i_this, dRes_INDEX_MO2_BCK_NYWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NYWALK_e);
    }
}

/* jyunkai (inlined). Returns false where the GameCube function returns early. */
static inline void jyunkai(mo2_class* i_this) {
    dComIfGp_get();
    fopAcM_SearchByID(i_this->mWeaponPcId); /* result unused */
    i_this->mCoCyl.SetR(REG0_F(4) + 40.0f);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case (u16)-10:
        i_this->m05A4[1] = 0x3c;
        i_this->mDamageReaction.mMode = -9;
        /* fallthrough */
    case (u16)-9:
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 2;
        }
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] > 0x1e) {
            return;
        }
        break;
    case 0:
        walk_set(i_this);
        /* fallthrough */
    case (u16)-1:
        i_this->mDamageReaction.mMode = 1;
        if (has_weapon(i_this)) {
            if (i_this->m2968 != 0) {
                s8 n = (s8)(i_this->m2969 + i_this->mHasPath);
                i_this->m2969 = n;
                if (n >= (s8)ppd(i_this)->m_num) {
                    if (ppd(i_this)->m_closed & 1) { /* dPath_ChkClose */
                        i_this->m2969 = 0;
                    } else {
                        i_this->mHasPath = -1;
                        i_this->m2969 = (s8)(ppd(i_this)->m_num - 2);
                    }
                    u16 nextID = ppd(i_this)->m_nextID;
                    if (nextID != 0xffff) {
                        i_this->ppd = dPath_GetRoomPath(nextID, fopAcM_GetRoomNo(i_this));
                        if (i_this->ppd == nullptr) JUT_ASSERT_fail(STR(0x10014EAC), 0xAFB, STR(0x10014ECC));
                    }
                } else if (n < 0) {
                    i_this->mHasPath = 1;
                    i_this->m2969 = 1;
                }
                dPnt_l* point = &ppd(i_this)->m_points[i_this->m2969];
                i_this->m05C4.x = point->m_position.x;
                i_this->m05C4.y = point->m_position.y;
                i_this->m05C4.z = point->m_position.z;
            } else {
                way_pos_check(i_this, &i_this->m05C4);
            }
        } else {
            way_pos_check(i_this, &i_this->m05C4);
            set_m05A4_rnd(i_this, 1, 25.0f, 30.0f);
        }
        i_this->m05A4[2] = 0x1e;
        /* fallthrough */
    case 1: {
        s16 maxSpeed;
        f32 dVar12;
        if (has_weapon(i_this)) {
            maxSpeed = 0x400;
            dVar12 = l_mo2HIO().m050;
        } else {
            maxSpeed = 0x1000;
            dVar12 = l_mo2HIO().m054;
        }
        f32 x = i_this->m05C4.x - i_this->current.pos.x;
        f32 z = i_this->m05C4.z - i_this->current.pos.z;
        i_this->mDamageReaction.m4D0 = cM_atan2s(x, z);
        if (i_this->m2968 != 0 && has_weapon(i_this)) {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            if (d < (dVar12 * 0.25f) * 15.0f) {
                dPnt_l* point = &ppd(i_this)->m_points[i_this->m2969];
                if (point->mArg3 == 3 || point->mArg3 == 7 || point->mArg3 == 8) {
                    wait_set(i_this);
                    if (point->mArg3 >= 7) {
                        set_m05A4_rnd(i_this, 1, 80.0f, 70.0f);
                    }
                    i_this->mDamageReaction.mMode = 2;
                } else {
                    i_this->mDamageReaction.mMode = -1;
                }
            }
            path_check2(i_this);
            if ((i_this->mCoCyl.ChkCoHit() || i_this->mDamageReaction.mAcch.ChkWallHit()) && i_this->m05A4[2] == 0) {
                wait_set(i_this);
                set_m05A4_rnd(i_this, 1, 80.0f, 70.0f);
                i_this->mHasPath = -i_this->mHasPath;
                i_this->mDamageReaction.mMode = 2;
            }
        } else {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            if (d < (dVar12 * 0.25f) + (dVar12 * 0.25f) ||
                (i_this->m05A4[2] == 0 &&
                 (i_this->mDamageReaction.mAcch.ChkWallHit() || ground_4_check(i_this, 1, i_this->current.angle.y, 200.0f)))) {
                wait_set(i_this);
                i_this->mDamageReaction.mMode = 2;
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, maxSpeed);
        cLib_addCalc2(&i_this->speedF, dVar12, 1.0f, 5.0f);
        break;
    }
    case 2:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] == 0) {
            if (i_this->m2968 != 0 && has_weapon(i_this)) {
                dPnt_l* point = &ppd(i_this)->m_points[i_this->m2969];
                if (point->mArg3 == 7 || point->mArg3 == 8) {
                    i_this->mDamageReaction.mMode = 4;
                    set_m05A4_rnd(i_this, 1, 100.0f, 100.0f);
                    anm_init(i_this, dRes_INDEX_MO2_BCK_KKEIKAI_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KKEIKAI_e);
                    if (point->mArg3 == 7) {
                        i_this->mDamageReaction.m4D0 += -0x4000;
                    } else {
                        i_this->mDamageReaction.m4D0 += 0x4000;
                    }
                    break;
                }
            }
            i_this->mDamageReaction.mMode = 0;
            if (i_this->mPathIndex != 0xFF && i_this->m2968 == 0) {
                path_check(i_this);
            }
            if (i_this->mbHasInnateWeapon == 0 && i_this->m2943 == 0 && i_this->m05AE == 0) {
                i_this->m2943 = 1;
            }
        }
        break;
    case 4:
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x1000);
        /* fallthrough */
    case 3: {
        i_this->speedF = 0.0f;
        s32 frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == 3 || frame == 0x35) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_MO_SEARCH, 0);
        }
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 0;
            if (i_this->mPathIndex != 0xFF && i_this->m2968 == 0) {
                path_check(i_this);
            }
        }
        break;
    }
    }
    s32 iVar6 = gabi::call<s32>(0x025D9DA0, i_this, 1000.0f) /* fopAcM_otoCheck */;
    iVar6 += search_sp();
    if (has_weapon(i_this)) {
        if (iVar6 != 0 ||
            (i_this->m05C0 < l_mo2HIO().m02C &&
             daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038))) {
            if (i_this->mMode == 1) {
                i_this->mDamageReaction.mAction = ACTION_NAGE;
                if (rouya_mode() != 0) {
                    i_this->mDamageReaction.mMode = -10;
                } else {
                    i_this->mDamageReaction.mMode = 0;
                }
                if (iVar6 != 0) {
                    u8* sound = dKy_Sound_get();
                    i_this->m2A10.copy(*gabi::at<cXyz>(gabi::ea(sound)));
                    i_this->m2A0C = 0x1e;
                }
            } else {
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->m05A4[1] = REG_S(18, 0); /* HD: GameCube 0 */
            }
            if (i_this != nullptr) {
                fopAcM_monsSeStart(i_this, JA_SE_CV_MO_FIND_ENEMY, 0);
            }
        }
    } else if (i_this->m2943 == 0 && i_this->mDamageReaction.mMode == 2) {
        s32 frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if ((frame == 0xb || frame == 0x19) && cM_rndF(1.0f) < 0.5f) {
            if (i_this != nullptr) {
                fopAcM_monsSeStart(i_this, JA_SE_CV_MO_LOSE_LANCE, 0);
            }
        }
    }
    if (i_this->mbHasInnateWeapon == 0 && daMo2_wepon_view_check(i_this)) {
        i_this->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
        i_this->mDamageReaction.mMode = -1;
    }
    if (daMo2_bomb_view_check(i_this)) {
        i_this->mDamageReaction.mAction = ACTION_B_NIGE;
        i_this->mDamageReaction.mMode = 0;
    }
}

/* HD: in Hyrule, keep the Moblin out of the wall behind it (as in d_mahi) */
static inline void back_wall_check(mo2_class* i_this) {
    if (!SafeString_eq(STR(0x10014D84) /* "Hyrule" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL)) {
        return;
    }
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
    gabi::Local<cXyz> sp34;
    *sp34 = i_this->current.pos.get();
    sp34->y += 20.0f;
    cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
    gabi::Local<cXyz> sp54;
    sp54->x = 0.0f;
    sp54->y = 20.0f;
    sp54->z = REG_F(18, 7) + -50.0f;
    gabi::Local<cXyz> sp48;
    MtxPosition(sp54, sp48);
    gabi::Local<cXyz> sp94;
    cXyz_pl(&i_this->current.pos, sp94, sp48);
    gabi::Local<cXyz> spA0;
    spA0->copy(*sp94);
    dBgS_LinChk_Set(linChk, sp34, spA0, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        i_this->current.pos.x = gabi::fnmsubs(sp48->x, REG_F(18, 9) + 0.2f, i_this->current.pos.x);
        i_this->current.pos.z = gabi::fnmsubs(sp48->z, REG_F(18, 9) + 0.2f, i_this->current.pos.z);
        hd_debug_print(0x1E, 0x190, STR(0x10014EE0) /* "MO2 BACK KABE HIT!" */);
    }
    dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
}

/* fight_run (inlined). Returns true where the GameCube function returns early. */
static inline bool fight_run(mo2_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dVar9 = CPad_GET_STICK_POS_X(0);
    i_this->mDamageReaction.m4D0 = i_this->m05D6;
    if (i_this->m05B0 == 0 && i_this->mDamageReaction.mMode != 0) {
        s16 maxSpeed = 0x400;
        if (i_this->mDamageReaction.mMode == 1) {
            maxSpeed = 0x800;
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, maxSpeed);
    }
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        if (i_this->m05A4[1] == 0) {
            fight_run_set(i_this);
            i_this->mDamageReaction.mMode = 1;
            i_this->m2964 = 0;
        } else {
            i_this->speedF = 0.0f;
            break;
        }
        /* fallthrough */
    case 1: {
        f32 target;
        f32 fVar10 = 20.0f;
        if (i_this->m2964 != 0) {
            target = l_mo2HIO().m06C;
            i_this->m05F0 = l_mo2HIO().m024 + 4;
            i_this->m05F2 = 4;
        } else {
            if (has_weapon(i_this)) {
                target = l_mo2HIO().m058;
            } else {
                target = l_mo2HIO().m05C;
            }
            fVar10 = 5.0f;
        }
        cLib_addCalc2(&i_this->speedF, target, 1.0f, fVar10);
        if (daMo2_player_way_check(i_this)) {
            if (i_this->m2964 > 0x14) {
                i_this->m2964 = 0;
                anm_init(i_this, dRes_INDEX_MO2_BCK_GAKEDEMO_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
                i_this->m05B0 = (s16)gabi::ftoi(cM_rndF(10.0f) + 15.0f);
            }
            if (i_this->m05B0 == 1) {
                i_this->m05B0 = 0;
                fight_run_set(i_this);
            }
            if (i_this->m05B0 == 0 && i_this->m05C0 < l_mo2HIO().m030) {
                i_this->mDamageReaction.mMode = 2;
                i_this->m05A4[4] = 0x32;
            }
        } else {
            if (i_this->m2964 == 0 && i_this->mbHasInnateWeapon != 0) {
                i_this->m2964 = 1;
                anm_init(i_this, dRes_INDEX_MO2_BCK_DASH_e, 5.0f, J3DFrameCtrl::EMode_LOOP, l_mo2HIO().m070, dRes_INDEX_MO2_BAS_DASH_e);
            }
            if (i_this->m2964 != 0) {
                i_this->m2964 = i_this->m2964 + 1;
            }
            /* HD: also the height difference */
            if (i_this->m05B0 == 0 && i_this->m05C0 < l_mo2HIO().m034 && m05C0_dy(i_this) < REG_F(10, 3) + 250.0f) {
                i_this->mDamageReaction.mAction = ACTION_FIGHT;
                i_this->mDamageReaction.mMode = 0;
                return true;
            }
        }
        break;
    }
    case 2:
        i_this->m2964 = 0;
        if (cM_rndF(1.0f) < 0.3f && i_this->mbHasInnateWeapon == 0) {
            i_this->mDamageReaction.mMode = 8;
            wait_set(i_this);
            set_m05A4_rnd(i_this, 1, 20.0f, 20.0f);
        } else {
            if (std::fabs(dVar9) > 0.1f) {
                if (has_weapon(i_this)) {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKLR_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_BWALKLR_e);
                } else {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKLR_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWALKLR_e);
                }
                if (dVar9 > 0.0f) {
                    i_this->mDamageReaction.mMode = 5;
                } else {
                    i_this->mDamageReaction.mMode = 6;
                }
            } else if (i_this->m05C0 < l_mo2HIO().m034) {
                if (has_weapon(i_this)) {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, -1.0f, dRes_INDEX_MO2_BAS_BWALKFB_e);
                } else {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, -1.0f, dRes_INDEX_MO2_BAS_NBWALKFB_e);
                }
                i_this->mDamageReaction.mMode = 4;
            } else {
                if (has_weapon(i_this)) {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_BWALKFB_e);
                } else {
                    anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWALKFB_e);
                }
                i_this->mDamageReaction.mMode = 3;
            }
            set_m05A4_rnd(i_this, 1, 20.0f, 20.0f);
        }
        break;
    case 3:
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m064, 1.0f, 20.0f);
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 2;
        }
        break;
    case 4:
        if (i_this->m05BF != 2) {
            cLib_addCalc2(&i_this->speedF, -l_mo2HIO().m064, 1.0f, 20.0f);
            if (i_this->m05A4[1] == 0) {
                i_this->mDamageReaction.mMode = 2;
            }
        } else {
            i_this->mDamageReaction.mMode = 3;
            if (has_weapon(i_this)) {
                anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_BWALKFB_e);
            } else {
                anm_init(i_this, dRes_INDEX_MO2_BCK_NBWALKFB_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWALKFB_e);
            }
        }
        break;
    case 5:
        if (i_this->m05BF != 4) {
            i_this->m05D8 = 0x4000;
        } else {
            i_this->mDamageReaction.mMode = 6;
            i_this->m05D8 = -0x4000;
        }
        goto temp_59C;
    case 6:
        if (i_this->m05BF != 8) {
            i_this->m05D8 = -0x4000;
        } else {
            i_this->mDamageReaction.mMode = 5;
            i_this->m05D8 = 0x4000;
        }
    temp_59C:
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m068, 1.0f, 30.0f);
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 2;
        }
        break;
    case 8:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 2;
        }
    }
    if (i_this->mDamageReaction.mMode >= 3 && i_this->m05BA <= 2) {
        if (i_this->m05C0 > l_mo2HIO().m030 + 75.0f) {
            to_jyunkai(i_this, 0);
        }
        if (i_this->m05C0 < l_mo2HIO().m034 + 62.5f && i_this->m05C0 > l_mo2HIO().m034 - 62.5f && i_this->m05A4[4] == 0) {
            i_this->m05A4[4] = l_mo2HIO().m074;
            if (cM_rndF(100.0f) < l_mo2HIO().m078 && m05C0_dy(i_this) < REG_F(10, 3) + 250.0f) { /* HD: height */
                i_this->mDamageReaction.mAction = ACTION_FIGHT;
                i_this->mDamageReaction.mMode = 0;
            }
        }
        if (i_this->m05B6 == 0) {
            u32 attention = dComIfGp_ea() + PLAY_ATTENTION;
            if (i_this->mbHasInnateWeapon != 0 && daPy_getCutType(player) != 0 && (cc_pl_cut_bit_get() & i_this->m2960) != 0) {
                if ((dAttention_LockonTruth(attention) || (gabi::load<u32>(attention + 0x20) & 0x20000000) != 0) &&
                    i_this == dAttention_LockonTarget(attention, 0)) {
                    i_this->mDamageReaction.mAction = ACTION_DEFENCE;
                    i_this->mDamageReaction.mMode = 0;
                }
            }
        }
    }
    if (i_this->mbHasInnateWeapon != 0 && i_this->m05C0 < l_mo2HIO().m034 - 62.5f &&
        m05C0_dy(i_this) < REG_F(10, 3) + 250.0f && /* HD */
        daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
        i_this->m05A0++;
        if (i_this->m05A0 >= 0xF && cM_rndF(1.0f) < 0.5f) {
            i_this->m05A0 = 0;
            i_this->mDamageReaction.mAction = ACTION_OSHI;
            i_this->mDamageReaction.mMode = 0;
        }
    } else {
        i_this->m05A0 = 0;
    }
    if (daMo2_player_bg_check(i_this, &i_this->mDamageReaction.m714->current.pos)) {
        to_jyunkai(i_this, -10);
    }
    if (i_this->mbHasInnateWeapon == 0 && daMo2_wepon_view_check(i_this)) {
        i_this->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
        i_this->mDamageReaction.mMode = -1;
    }
    if (daMo2_bomb_view_check(i_this)) {
        i_this->mDamageReaction.mAction = ACTION_B_NIGE;
        i_this->mDamageReaction.mMode = 0;
    }
    f32 r = REG_F(6, 7) + 90.0f;
    i_this->m05BF = (u8)ground_4_check(i_this, 4, i_this->current.angle.y, r);
    if (i_this->m05BA != 0) {
        if (std::fabs(i_this->speedF) < 30.0f) {
            if (i_this->m05BE == 0) {
                i_this->mDamageReaction.m710 = 3;
            } else if (i_this->m05BE == 1) {
                i_this->mDamageReaction.m710 = 4;
            } else if ((i_this->m059C & 0x10) != 0) {
                i_this->mDamageReaction.m710 = 3;
            } else {
                i_this->mDamageReaction.m710 = 4;
            }
            cLib_addCalcAngleS2(&i_this->m2952, 12000, 2, 0x1800);
        } else {
            i_this->mDamageReaction.m710 = 1;
        }
    } else {
        i_this->mDamageReaction.m710 = 1;
        if (i_this->m05BC == 0) {
            i_this->m05BC = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
            if (i_this->m05BF == 4) {
                i_this->m05BE = 0;
                i_this->m05BA = 0x10;
            } else if (i_this->m05BF == 8) {
                i_this->m05BE = 1;
                i_this->m05BA = 0x10;
            } else if (i_this->m05BF == 2) {
                i_this->m05BE = 2;
                i_this->m05BA = 0x20;
            }
        }
    }
    back_wall_check(i_this);
    return false;
}

/* yari_hit_check (inlined) */
static inline fopAc_ac_c* yari_hit_check(mo2_class* i_this) {
    dComIfGp_get();
    i_this->m291C.copy(i_this->m2904);
    i_this->m2940 = 0;
    i_this->mWeapon2Sph.SetR(0.0f);
    if (i_this->m207E < 0) {
        return nullptr;
    }
    if (i_this->m2060 == 5) {
        i_this->m2904.copy(i_this->m28F8);
    } else if (i_this->m2060 == 3 || i_this->m2060 == 4) {
        i_this->m2904.copy(i_this->m28EC);
    } else {
        i_this->m2904.copy(i_this->m28D4);
        i_this->m2910.copy(i_this->m28E0);
        i_this->mWeapon2Sph.SetR(62.5f);
    }
    if (i_this->m207C != 0) {
        i_this->m207C--;
        return nullptr;
    }
    if (i_this->m2068 < i_this->m206C || i_this->m2068 > i_this->m2070) {
        return nullptr;
    }
    i_this->m2940 = (u8)(i_this->m2940 << 1);
    i_this->mWeaponSph.SetAtSpl((u8)mo2_at_kind(i_this->m2060));
    i_this->mWeapon2Sph.SetAtSpl((u8)mo2_at_kind(i_this->m2060));
    i_this->mWeaponSph.SetAtSe((u8)mo2_at_sm_kind(i_this->m2060));
    i_this->mWeapon2Sph.SetAtSe((u8)mo2_at_sm_kind(i_this->m2060));
    if (i_this->m2060 == 2 || i_this->m2060 == 1 || i_this->m2060 == 5) {
        i_this->mWeaponSph.SetAtType(AT_TYPE_UNK8);
        i_this->mWeapon2Sph.SetAtType(AT_TYPE_UNK8);
    } else {
        i_this->mWeaponSph.SetAtType(AT_TYPE_UNK800);
        i_this->mWeapon2Sph.SetAtType(AT_TYPE_UNK800);
    }
    if ((u32)gabi::ftoi(i_this->m2068) == (u32)gabi::ftoi(i_this->m206C)) {
        s32 se = mo2_attack_go_SE(i_this->m2060);
        if (i_this != nullptr) {
            fopAcM_monsSeStart(i_this, se, 0);
        }
    }
    if (i_this->m2941 == 0) {
        i_this->m2941 = 1;
        dCcD_Sph_StartCAt(&i_this->mWeaponSph, &i_this->m2904);
        dCcD_Sph_StartCAt(&i_this->mWeapon2Sph, &i_this->m2910);
    } else {
        dCcD_Sph_MoveCAt(&i_this->mWeaponSph, &i_this->m2904);
        dCcD_Sph_MoveCAt(&i_this->mWeapon2Sph, &i_this->m2910);
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
        dComIfG_Ccsp_Set(&i_this->mWeapon2Sph);
        if (i_this->m2060 == 2 || i_this->m2060 == 1) {
            dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
            dComIfG_Ccsp_SetMass(&i_this->mWeapon2Sph, 3);
        }
        if (i_this->mWeaponSph.ChkAtHit() || i_this->mWeapon2Sph.ChkAtHit()) {
            void* obj;
            if (i_this->mWeaponSph.ChkAtHit()) {
                obj = GetAtHitObj(&i_this->mWeaponSph);
            } else {
                obj = GetAtHitObj(&i_this->mWeapon2Sph);
            }
            return hit_obj_actor(obj);
        }
    }
    return nullptr;
}

/* fight (inlined) */
static inline void fight(mo2_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->m02E0 = 2;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        attack_set(i_this, 0);
        tex_anm_set(i_this, 6);
        i_this->mDamageReaction.mMode = 1;
        i_this->m05A4[2] = 8;
        /* fallthrough */
    case 1: {
        i_this->mWeaponSph.SetAtAtp((u8)mo2_attack_AP(i_this->m2060));
        i_this->mWeapon2Sph.SetAtAtp((u8)mo2_attack_AP(i_this->m2060));
        attack_info_s* info = attack_info(i_this->m2060);
        f32 r3 = info[i_this->m2064].speed;
        i_this->m2068 += r3;
        cLib_addCalc2(&i_this->speedF, 0.0f, 1.0f, 20.0f);
        if (i_this->m2060 == 3 && i_this->m2064 == 0) {
            i_this->speedF = 30.0f;
        } else if (i_this->m2060 == 5 && i_this->m2064 == 1) {
            if (i_this->m207E > 0) {
                i_this->speedF = 70.0f;
            } else {
                i_this->speedF = -30.0f;
            }
            i_this->m05F0 = l_mo2HIO().m024 + 3;
            i_this->m05F2 = 4;
        }
        if (i_this->m2068 > i_this->m2074 &&
            daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
            i_this->mDamageReaction.m710 = 1;
        }
        if (i_this->m207E > 0) {
            if (i_this->m2060 == 1) {
                s32 r3 = l_mo2HIO().m0D8 + l_mo2HIO().m0DA + l_mo2HIO().m0DC + l_mo2HIO().m0DE;
                s32 t = gabi::ftoi(i_this->m2068);
                if (t >= l_mo2HIO().m0D8 && t <= r3) {
                    i_this->m0594 = 1;
                    t = gabi::ftoi(i_this->m2068);
                    if (t >= l_mo2HIO().m0D8 + l_mo2HIO().m0DA && t < l_mo2HIO().m0DC + (l_mo2HIO().m0D8 + l_mo2HIO().m0DA)) {
                        i_this->m0598 = 1;
                        t = gabi::ftoi(i_this->m2068);
                    }
                    s32 iVar6 = t - l_mo2HIO().m0D8;
                    if (iVar6 < 10) {
                        i_this->m0590 = l_mo2HIO().m0E0[iVar6];
                    }
                }
            } else if (i_this->m2060 == 2) {
                s32 r3 = l_mo2HIO().m108 + l_mo2HIO().m10A + l_mo2HIO().m10C + l_mo2HIO().m10E;
                s32 t = gabi::ftoi(i_this->m2068);
                if (t >= l_mo2HIO().m108 && t <= r3) {
                    i_this->m0594 = 1;
                    t = gabi::ftoi(i_this->m2068);
                    if (t >= l_mo2HIO().m108 + l_mo2HIO().m10A && t < l_mo2HIO().m10C + (l_mo2HIO().m108 + l_mo2HIO().m10A)) {
                        i_this->m0598 = 1;
                        t = gabi::ftoi(i_this->m2068);
                    }
                    s32 iVar6 = t - l_mo2HIO().m108;
                    if (iVar6 < 10) {
                        i_this->m0590 = l_mo2HIO().m110[iVar6];
                    }
                }
            }
        }
        if (i_this->m0594 == 0) {
            f32 br = br_set_tm(i_this->m2060);
            /* `>=` written as !(<): GHS branches on the negated comparison (NaN) */
            if (!(i_this->m2068 < br) && !(i_this->m2068 > br + 2.0f) && i_this->m2060 == 2) {
                i_this->m05F0 = l_mo2HIO().m024 + 0x10;
                i_this->m05F2 = 0;
                i_this->m05EE = i_this->current.angle.y + REG0_S(8) + 0x2000;
            }
        }
        if (i_this->m2060 == 1 && !(i_this->m2068 < 31.0f) && !(i_this->m2068 > 32.1f)) {
            i_this->m05F0 = l_mo2HIO().m024 + 8;
            i_this->m05F2 = 2;
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
            mDoAud_seStart(JA_SE_CM_LANCE_HIT_FLOOR, &i_this->m28D4, 0, reverb);
        }
        if (i_this->m2060 == 2 && i_this->m05A4[2] == 1) {
            i_this->m05F0 = l_mo2HIO().m024 + 4;
            i_this->m05F2 = 4;
        }
        if (fopAcM_searchPlayerDistance(i_this) < 500.0f) {
            if (i_this->m2068 < i_this->m2078 || i_this->m2942 != 0) {
                i_this->mDamageReaction.m4D0 = i_this->m05D6;
            }
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x800);
        }
        fopAc_ac_c* hitActor = yari_hit_check(i_this);
        if (hitActor != nullptr) {
            if (fpcM_GetName(hitActor) == fpcNm_PLAYER_e) {
                if (daPy_checkPlayerGuard(player) && i_this->m2060 != 3) {
                    i_this->mpMorf->setPlaySpeed(-1.0f);
                    if (i_this->m05F0 != 0) {
                        i_this->m05F0 = l_mo2HIO().m024 + 6;
                    }
                    i_this->m207E = -1;
                    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
                }
            } else if (fpcM_GetName(hitActor) == fpcNm_MO2_e) {
                i_this->m2954 = fopAcM_GetID(hitActor);
            }
        } else {
            i_this->mWeaponSph.ClrAtHit();
            i_this->mWeapon2Sph.ClrAtHit();
            if (i_this->m2940 != 0) {
                if (i_this->m2940 == 2) {
                    i_this->mpMorf->setPlaySpeed(-1.0f);
                    i_this->m0594 = 0;
                    if (i_this->m05F0 != 0) {
                        i_this->m05F0 = l_mo2HIO().m024 + 6;
                    }
                    i_this->m207E = -1;
                    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
                } else {
                    to_jyunkai(i_this, 2);
                }
                i_this->m05BE = 1;
                i_this->m05BA = 0x10;
                gabi::Local<cXyz> local_38;
                local_38->x = local_38->y = local_38->z = 1.0f;
                dComIfGp_particle_set(0xC /* ID_AK_JN_NG */, &i_this->m2934, nullptr, local_38);
                s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
                mDoAud_seStart(JA_SE_LK_ROPE_HOOK_METAL, &i_this->m2934, 0, reverb);
            }
        }
        if (i_this->mpMorf->isStop()) {
            if ((i_this->m2064 == 2 && i_this->m207E > 0) || (i_this->m2060 == 4 && i_this->m2064 == 0) ||
                (i_this->m207E < 0 && i_this->m2064 == 0) || (i_this->m2060 == 5 && i_this->m207E < 0 && i_this->m2064 == 1)) {
                if (i_this->m2060 == 3) {
                    attack_set(i_this, 1);
                    i_this->mDamageReaction.m4D0 = i_this->m05D6;
                } else if (i_this->m2954 != fpcM_ERROR_PROCESS_ID_e) {
                    i_this->mDamageReaction.mAction = ACTION_AITE_MIRU;
                    i_this->mDamageReaction.mMode = 0;
                } else if (i_this->m05C0 < l_mo2HIO().m030) {
                    if (daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
                        if (cM_rndF(1.0f) < 0.5f || i_this->m207E < 0) {
                            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                            i_this->mDamageReaction.mMode = 2;
                            i_this->m05A4[1] = 0;
                        } else {
                            i_this->mDamageReaction.mMode = 0;
                        }
                    } else if (has_weapon(i_this)) {
                        i_this->mDamageReaction.mAction = ACTION_P_LOST;
                        i_this->mDamageReaction.mMode = 0;
                        i_this->m05A4[1] = 0;
                    } else {
                        to_jyunkai(i_this, -10);
                    }
                } else {
                    to_jyunkai(i_this, -10);
                }
            } else {
                attack_info_s* info = attack_info(i_this->m2060);
                f32 fVar11;
                if (i_this->m207E > 0) {
                    i_this->m2064++;
                    fVar11 = info[i_this->m2064].speed;
                } else {
                    i_this->m2064--;
                    fVar11 = -info[i_this->m2064].speed;
                }
                anm_init(i_this, info[i_this->m2064].bckFileIdx, 0.0f, J3DFrameCtrl::EMode_NONE, fVar11, info[i_this->m2064].soundFileIdx);
            }
        }
    }
    }
}

/* 021D0A14 Mo2_move (the matcher names it fight): jyunkai, fight_run, fight, yari_hit_check inlined */
static void Mo2_move(mo2_class* i_this) {
    WWHD_FUNC(0x021D0A14, void, i_this);
    dComIfGp_get();
    i_this->mDamageReaction.m710 = 0;
    i_this->mDamageReaction.m711 = 0;
    i_this->m05D8 = 0;
    cLib_addCalcAngleS2(&i_this->m2952, 0, 2, 0x800);
    if (i_this->mDamageReaction.mMode <= -0x64) {
        if (i_this->mDamageReaction.mAcch.ChkGroundLanding()) {
            tex_anm_set(i_this, 0);
        }
        i_this->m02E0 = 2;
        i_this->m05B4 = 5;
        if (std::fabs(i_this->mDamageReaction.m478) > 40.0f && !i_this->mDamageReaction.mAcch.ChkGroundHit()) {
            i_this->mWeaponSph.SetC(&i_this->mDamageReaction.m100[0xc]);
            i_this->mWeaponSph.SetR(60.0f);
            i_this->mWeaponSph.OffAtSPrmBit(4);  /* OffAtVsPlayerBit */
            i_this->mWeaponSph.SetAtSpl(1);
            i_this->mWeaponSph.OnCoSPrmBit(1);   /* OnCoSetBit */
            dComIfG_Ccsp_Set(&i_this->mWeaponSph);
            dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
            if (i_this->mWeaponSph.ChkAtHit() && i_this->speed.y < -50.0f) {
                i_this->speed.y = 0.0f;
                i_this->mDamageReaction.m474 = 8000.0f;
            }
            i_this->mCoCyl.OffCoSPrmBit(1);
        }
        return;
    }
    i_this->mWeaponSph.OnAtSPrmBit(4); /* OnAtVsPlayerBit */
    if (i_this->m2A1C == 0) {
        i_this->mCoCyl.OnCoSPrmBit(1);
    } else {
        i_this->mCoCyl.OffCoSPrmBit(1);
    }
    if (i_this->mDamageReaction.m48A != 0 && i_this->mDamageReaction.m488 == 0) {
        if (i_this->mDamageReaction.m48A == 1) {
            if ((i_this->mMode != 1 || rouya_mode() == 0) &&
                (i_this->mDamageReaction.mAction != ACTION_P_LOST || i_this->m05A4[1] == 0)) {
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->m05A4[1] = 0;
            }
        } else {
            i_this->mDamageReaction.m4D0 = i_this->m05D6;
            i_this->speedF = -30.0f;
            i_this->m05F0 = l_mo2HIO().m024 + 3;
            i_this->m05F2 = 4;
        }
    } else {
        gabi::Local<cXyz> local_20;
        if (i_this->m2A0C != 0) {
            i_this->m2A0C--;
            gabi::Local<cXyz> d;
            cXyz_mi(&i_this->m2A10, d, &i_this->current.pos);
            *local_20 = d->get();
        } else {
            i_this->mDamageReaction.m714 = dComIfGp_getPlayer(0); /* search_target */
            gabi::Local<cXyz> d;
            cXyz_mi(&i_this->mDamageReaction.m714->current.pos, d, &i_this->current.pos);
            *local_20 = d->get();
        }
        i_this->m05C0 = std_sqrtf(gabi::fmadds(local_20->x, local_20->x, local_20->z * local_20->z));
        m05C0_dy(i_this) = std::fabs((f32)local_20->y); /* HD */
        i_this->m05D6 = cM_atan2s(local_20->x, local_20->z);
        i_this->mCoCyl.SetR(REG0_F(3) + 90.0f);
        if (i_this->m2A0B != 0) {
            i_this->m2A0B--;
            if (i_this->m2A0B == 0) {
                i_this->mDamageReaction.m46C = 0.0f; /* HD */
                i_this->mDamageReaction.m468 = 0.0f;
                i_this->mDamageReaction.mAction = ACTION_D_MAHI;
                i_this->mDamageReaction.mMode = 0;
                i_this->current.angle.z = 0;
                i_this->current.angle.x = 0;
            }
        }
        i_this->mDamageReaction.mStts.SetWeight(200); /* HD */
        switch ((u16)i_this->mDamageReaction.mAction) {
        case ACTION_JYUNKAI:
            jyunkai(i_this);
            break;
        case ACTION_FIGHT_RUN:
            fight_run(i_this);
            break;
        case ACTION_FIGHT:
            fight(i_this);
            break;
        case ACTION_NAGE:
            nage(i_this);
            break;
        case ACTION_DEFENCE:
            defence(i_this);
            break;
        case ACTION_OSHI:
            oshi(i_this);
            break;
        case ACTION_P_LOST:
            p_lost(i_this);
            break;
        case ACTION_B_NIGE:
            b_nige(i_this);
            break;
        case ACTION_HUKKI:
            hukki(i_this);
            break;
        case ACTION_WEPON_SEARCH:
            wepon_search(i_this);
            break;
        case ACTION_HIP_DAMAGE:
            hip_damage(i_this);
            break;
        case ACTION_AITE_MIRU:
            aite_miru(i_this);
            break;
        case ACTION_FAIL:
            fail(i_this);
            break;
        case ACTION_YOGAN_FAIL:
            yogan_fail(i_this);
            break;
        case ACTION_CARRY:
            carry(i_this);
            break;
        case ACTION_CARRY_DROP:
            carry_drop(i_this);
            break;
        case ACTION_D_SIT:
            d_sit(i_this);
            break;
        case ACTION_D_MAHI:
            d_mahi(i_this);
            i_this->mDamageReaction.mStts.SetWeight(0xFF); /* HD */
            break;
        case ACTION_D_DOZOU:
            d_dozou(i_this);
            i_this->mDamageReaction.mStts.SetWeight(0xFF); /* HD */
            break;
        case ACTION_E3_DEMO:
            e3_demo(i_this);
            break;
        }
    }
    if (i_this->mDamageReaction.mAction != ACTION_CARRY) {
        gabi::Local<cXyz> local_20;
        gabi::Local<cXyz> local_2c;
        local_20->y = 0.0f;
        local_20->x = 0.0f;
        local_20->z = i_this->speedF;
        if (i_this->mDamageReaction.mAction != ACTION_HUKKI && i_this->mDamageReaction.mAction != ACTION_FAIL &&
            i_this->mDamageReaction.m48A == 0) {
            i_this->mDamageReaction.m482 = i_this->current.angle.y;
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y + i_this->m05D8);
        } else {
            cMtx_YrotS(calc_mtx(), i_this->mDamageReaction.m4D0);
        }
        MtxPosition(local_20, local_2c);
        i_this->speed.x = local_2c->x;
        i_this->speed.z = local_2c->z;
    }
}
VERIFY(0x021D0A14, Mo2_move);
