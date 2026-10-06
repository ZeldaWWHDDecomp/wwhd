/**
 * d_a_tsubo_dmg.cpp (WWHD)
 * Small carriable objects (pots, skulls, ...): damage, sound/splash, water/lava and bound
 * functions (part B of d_a_tsubo).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tsubo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tsubo.h"

using namespace daTsubo;

/* ---- functions of the other part files (guest calls by address) ---- */
static inline void init_rot_clean(Act_c* a) { gabi::call(0x024CAA54, a); }
static inline void mode_wait_init(Act_c* a) { gabi::call(0x024CAC44, a); }
static inline void set_senv(Act_c* a, s32 p1, s32 p2) { gabi::call(0x024CBDE8, a, p1, p2); }
/* harness workaround: 024CBDE8 is named daObj::PrmAbstract by the matcher and its facts compare r6;
 * where the original sets r6 explicitly (the data row), pass it */
static inline void set_senv_r6(Act_c* a, s32 p1, s32 p2, u32 r6) { gabi::call(0x024CBDE8, a, p1, p2, r6); }
/* eff_break_* through the PTMF table eff_break_proc (0x10040CE0, 16 entries) */
#define EFF_BREAK_PROC 0x10040CE0

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD: the tsubo TU's fopAcM_seStart inline has no null checks at all */
static inline void fopAcM_seStart_raw(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 024EECAC dBgS::GetMtrlSndId(const cBgS_PolyInfo&) */
static inline u32 GetMtrlSndId(cBgS_PolyInfo* p) { return dBgS_GetMtrlSndId(dComIfG_Bgsp(), p); }
/* cBgS_PolyInfo: poly index u16 +0, bg index u16 +2 */
static inline u16 PolyInfo_GetBgIndex(cBgS_PolyInfo* p) { return gabi::load<u16>(gabi::ea(p) + 2); }
static inline u16 PolyInfo_GetPolyIndex(cBgS_PolyInfo* p) { return gabi::load<u16>(gabi::ea(p) + 0); }
/* 025DADA4 fopKyM_create(s16 name, s32 param, cXyz* pos, cXyz* scale, void* createFunc) */
static inline u32 fopKyM_create(s16 name, s32 param, const cXyz* pos, cXyz* scale, u32 fn) {
    return gabi::call<u32>(0x025DADA4, name, param, pos, scale, fn);
}
/* 025DAF3C fopKyM_createMpillar(const cXyz* pos, f32 scale) */
static inline void fopKyM_createMpillar(const cXyz* pos, f32 s) { gabi::call(0x025DAF3C, pos, s); }
/* 023129C4 daObj::HitSeStart(const cXyz*, int roomNo, const dCcD_GObjInf*, u32 se) */
static inline void daObj_HitSeStart(cXyz* pos, s32 room, dCcD_GObjInf* o, u32 se, u32 r7) { gabi::call(0x023129C4, pos, room, o, se, r7); }
/* 02312C8C daObj::HitEff_kikuzu(const fopAc_ac_c*, const dCcD_Cyl*), 02312E54 HitEff_hibana */
static inline void daObj_HitEff_kikuzu(fopAc_ac_c* a, dCcD_Cyl* c) { gabi::call(0x02312C8C, a, c); }
static inline void daObj_HitEff_hibana(fopAc_ac_c* a, dCcD_Cyl* c) { gabi::call(0x02312E54, a, c); }
/* 02311CD8 daObj::make_land_effect(fopAc_ac_c*, dBgS_GndChk*, f32) */
static inline void daObj_make_land_effect(fopAc_ac_c* a, void* gnd, f32 s) { gabi::call(0x02311CD8, a, gnd, s); }
/* 025052BC dCamera_c::ForceLockOff(fpc_ProcID): the camera of dComIfGp_getCamera(0)
 * (camera_class* at play+0x5AF8; its dCamera_c at +0x248) */
static inline void dCamera_ForceLockOff(fopAc_ac_c* a) {
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
    gabi::call(0x025052BC, cam + 0x248, gabi::load<u32>(gabi::ea(a) + 4) /* base.base.mBsPcId */);
}
/* 024EEB2C dBgS::ChkMoveBG_NoDABg, 024EEABC dBgS::ChkMoveBG (const cBgS_PolyInfo&) */
static inline BOOL dBgS_ChkMoveBG_NoDABg_l(cBgS_PolyInfo* p) { return gabi::call<BOOL>(0x024EEB2C, dComIfG_Bgsp(), p); }
static inline BOOL dBgS_ChkMoveBG_l(cBgS_PolyInfo* p) { return gabi::call<BOOL>(0x024EEABC, dComIfG_Bgsp(), p); }
/* 028E9F74 C_VECReflect(const Vec* src, const Vec* normal, Vec* dst) */
static inline void C_VECReflect(const cXyz* s, const void* n, cXyz* d) { gabi::call(0x028E9F74, s, n, d); }
/* the hit object's At type (cCcD_ObjAt::mType, +0x10) and its virtuals: GetShapeAttr() is the
 * HD vtable (+0x3C) slot 0x2C; cCcD_ShapeAttr::GetNVec(const cXyz&, cXyz*) the vtable (+0x1C)
 * slot 0x9C */
static inline u32 obj_GetAtType(void* o) { return gabi::load<u32>(gabi::ea(o) + 0x10); }
static inline u32 obj_GetShapeAttr(void* o) { return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(o) + 0x3C) + 0x2C), o); }
static inline BOOL shapeAttr_GetNVec(u32 sa, cXyz* pos, cXyz* out) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(sa + 0x1C) + 0x9C), sa, pos, out);
}
/* fopAcM_GetProfName (inline, null-checked): base_process_class::mProcName at +0xE */
static inline s16 fopAcM_GetProfName(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 0xE); }
/* dPa_followEcallBack::isEnd(): bit 0 of +0x10 */
static inline bool followEcallBack_isEnd(dPa_followEcallBack* f) { return (f->m10 & 1) != 0; }

enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_UNK8 = 0x8, AT_TYPE_BOMB = 0x20, AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_WIND = 0x200000,
    /* SWORD | BOKO_STICK | MACHETE | UNK800 | SPIKE | UNK2000 | DARKNUT_SWORD | MOBLIN_SPEAR */
    AT_TYPE_CUT_MASK = 0x14003C82,
};
enum { fpcNm_PLAYER_e = 0xA8 };
enum {
    JA_SE_OBJ_WPOT_LIFTUP = 0x6943, JA_SE_OBJ_BOKKURI_PICK_UP = 0x69F1, JA_SE_OBJ_BOKKURI_BOUND = 0x69F2,
};
enum { dPa_ID_IT_SN_WPOT_PITYA = 0x8085, dPa_ID_IT_SN_WPOT_PITYA_C = 0x8092 };
#define G_CM3D_F_INF_NEG (-1000000000.0f)

/* the material sound of the first poly info hit (bg index below 0x100) */
static inline u32 mtrl_snd(cBgS_PolyInfo* p0, cBgS_PolyInfo* p1, u32 def, bool nullchk) {
    cBgS_PolyInfo* arr[2] = {p0, p1};
    for (s32 i = 0; i < 2; i++) {
        if ((!nullchk || arr[i] != nullptr) && PolyInfo_GetBgIndex(arr[i]) < 0x100) {
            return GetMtrlSndId(arr[i]);
        }
    }
    return def;
}

static inline cBgS_PolyInfo* Acch_wtr(Act_c* a) { return gabi::at<cBgS_PolyInfo>(gabi::ea(&a->mAcch) + 0x174); }
static inline cBgS_PolyInfo* Acch_roof(Act_c* a) { return gabi::at<cBgS_PolyInfo>(gabi::ea(&a->mAcch) + 0x128); }
static inline cBgS_PolyInfo* Yogan_poly(Act_c* a) { return gabi::at<cBgS_PolyInfo>(gabi::ea(&a->mGndChkYogan) + 0x14); }

/* 024CBE2C */
void se_break(Act_c* a, cBgS_PolyInfo* p) {
    WWHD_FUNC(0x024CBE2C, void, a, p);
    u32 snd = mtrl_snd(p, Acch_gnd(&a->mAcch), 0, true);
    fopAcM_seStart_raw(a, data(a)->mSoundID_Break, snd);
    Act_c::Data_c* d = data(a);
    set_senv_r6(a, d->m60, d->m61, gabi::ea(d));
}
VERIFY(0x024CBE2C, se_break);

/* 024CBF64 */
void spec_kill(Act_c* a) {
    WWHD_FUNC(0x024CBF64, void, a);
    s32 spec = prm_get_spec(a);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &a->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = M_spec_act()[i];
                if (boko != nullptr) {
                    s8 room = fopAcM_GetRoomNo(a);
                    ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
                    boko->current.roomNo = room; /* fopAcM_SetRoomNo */
                    fopAcM_cancelCarryNow(boko);
                    fopAcM_delete(boko);
                }
            }
        }
    }
}
VERIFY(0x024CBF64, spec_kill);

/* 024CC004; HD: the item action is clamped to 0..15, kankyo process 0x17 (GameCube 0x19), and the
 * eff_break_proc index is asserted */
void damaged(Act_c* a, s32 arg1, cBgS_PolyInfo* arg2, bool arg3, const cXyz* arg4) {
    WWHD_FUNC(0x024CC004, void, a, arg1, arg2, arg3, arg4);
    s32 action = arg1 < 0 ? 0 : (arg1 >= 0x10 ? 0xF : arg1);
    s32 itemNo = prm_get_itemNo(a);
    s32 itemBitNo = prm_get_itemSave(a);
    gabi::Local<csXyz> sp08;
    csXyz_ct(sp08, 0, a->home.angle.y, 0);
    fopAcM_createItemFromTable(&a->current.pos, itemNo, itemBitNo, a->home.roomNo, 0, sp08, action, nullptr);
    fopAcM_cancelCarryNow(a);
    a->actor_status = a->actor_status & ~0x100000u; /* fopAcM_cancelHookCarryNow */

    if (arg3) {
        if (a->mType == 2 && arg4 != nullptr) {
            fopKyM_create(0x17, a->tevStr.mRoomNo, arg4, nullptr, 0);
        }
        if ((u32)a->mType >= 0x10) /* JUT_ASSERT(3426, mType < ARRAY_SIZE(eff_break_proc)) */
            JUT_ASSERT_fail(STR(0x10040CAC), 0xD62, STR(0x10040CBC));
        ptmf_call(EFF_BREAK_PROC + 8 * a->mType, a);
        se_break(a, arg2);
    } else {
        spec_kill(a);
    }
}
VERIFY(0x024CC004, (void (*)(Act_c*, s32, cBgS_PolyInfo*, bool, const cXyz*))damaged);

/* 024CC238 */
void damaged(Act_c* a, s32 arg1, cBgS_PolyInfo* arg2) {
    WWHD_FUNC(0x024CC238, void, a, arg1, arg2);
    damaged(a, arg1, arg2, true, &a->current.pos);
}
VERIFY(0x024CC238, (void (*)(Act_c*, s32, cBgS_PolyInfo*))damaged);

/* 024CC244 */
void set_wind_vec(Act_c* a) {
    WWHD_FUNC(0x024CC244, void, a);
    void* obj = a->mCyl.GetTgHitObj();
    if (obj != nullptr && (obj_GetAtType(obj) & AT_TYPE_WIND)) {
        gabi::Local<cXyz> sp48;
        cXyz* rv = &a->mCyl.mGObjTg.mRVec;
        sp48->x = rv->x;
        sp48->y = rv->y;
        sp48->z = rv->z;
        f32 fVar7 = PSVECSquareMag(sp48);
        if (fVar7 > 31684.0f) {
            PSVECScale(sp48, sp48, 178.0f / std_sqrtf(fVar7));
        }
        u32 sa = obj_GetShapeAttr(obj);
        gabi::Local<cXyz> sp3C;
        sp3C->x = cXyz_Zero->x;
        sp3C->y = cXyz_Zero->y;
        sp3C->z = cXyz_Zero->z;
        f32 fVar1 = 1.0f;
        if (shapeAttr_GetNVec(sa, &a->current.pos, sp3C)) {
            PSVECScale(sp3C, sp3C, 45.0f);
            PSVECSquareMag(&a->m7F4);
            fopAc_ac_c* hit = a->mCyl.GetTgHitAc();
            if (hit != nullptr && hit != nullptr && fopAcM_GetProfName(hit) == fpcNm_PLAYER_e) {
                s16 ang = cM_atan2s(sp3C->x, sp3C->z);
                f32 cos = cM_scos((s16)(hit->shape_angle.y - ang));
                if (cos > 0.866f) {
                    fVar1 = cos + cos + 1.0f;
                } else {
                    fVar1 = 0.0f;
                }
            }
        }
        /* HD: fsel on 0.01 - fVar7 */
        f32 m1, m2;
        if (0.01f - fVar7 >= 0.0f) {
            m1 = 0.0f;
            m2 = 1.0f;
        } else {
            m1 = 1.0f;
            m2 = 0.05f;
        }
        gabi::Local<cXyz> t1, t2, t3, res;
        cXyz_ml(sp48, t1, m1);
        cXyz_ml(sp3C, t2, m2);
        cXyz_ml(t2, t3, fVar1);
        cXyz_pl(t1, res, t3);
        a->m7F4.copy(*res);
    }
}
VERIFY(0x024CC244, set_wind_vec);

/* 024CC4BC */
void mode_walk_init(Act_c* a) {
    WWHD_FUNC(0x024CC4BC, void, a);
    a->mCyl.OffAtSPrmBit(1);
    a->mCyl.OnTgSPrmBit(1);
    a->mCyl.OnCoSPrmBit(1);
    Acch_ClrRoofNone(&a->mAcch);
    Acch_ClrWallNone(&a->mAcch);
    a->mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&a->mAcch);
    a->mAcch.OffLineCheck();
    a->m67F = 0;
    init_rot_clean(a);
    a->m678 = 3;
}
VERIFY(0x024CC4BC, mode_walk_init);

/* 024CC52C */
bool damage_tg_acc(Act_c* a) {
    WWHD_FUNC(0x024CC52C, bool, a);
    void* obj = a->mCyl.GetTgHitObj();
    bool iVar9 = false;
    if (obj != nullptr) {
        f32 fVar1 = 1.5f;
        f32 fVar2 = 0.0f;
        if (obj_GetAtType(obj) & AT_TYPE_CUT_MASK) {
            fVar1 = 2.0f;
            fVar2 = 9.0f;
        }
        /* HD: fVar1 > 0 folded */
        f32 temp1 = std_sqrtf(PSVECSquareMag(&a->m814));
        gabi::Local<cXyz> sp34;
        cXyz* rv = &a->mCyl.mGObjTg.mRVec;
        sp34->z = rv->z;
        sp34->y = rv->y;
        sp34->x = rv->x;
        f32 temp2 = std_sqrtf(PSVECSquareMag(sp34));
        if (fVar1 > temp1 && temp2 > temp1) {
            if (temp2 < fVar1) {
                a->m814.x = sp34->x;
                a->m814.z = sp34->z;
                a->m814.y = sp34->y + fVar2;
            } else {
                gabi::Local<cXyz> r;
                cXyz_ml(sp34, r, fVar1 / temp2);
                a->m814.x = r->x;
                a->m814.y = r->y + fVar2;
                a->m814.z = r->z;
            }
            return true;
        }

        if (obj_GetAtType(obj) & AT_TYPE_WIND) {
            u32 sa = obj_GetShapeAttr(obj);
            gabi::Local<cXyz> sp28;
            sp28->x = cXyz_Zero->x;
            sp28->y = cXyz_Zero->y;
            sp28->z = cXyz_Zero->z;
            if (shapeAttr_GetNVec(sa, &a->current.pos, sp28)) {
                fopAc_ac_c* hit = a->mCyl.GetTgHitAc();
                if (hit != nullptr && hit != nullptr && fopAcM_GetProfName(hit) == fpcNm_PLAYER_e) {
                    s16 ang = cM_atan2s(sp28->x, sp28->z);
                    if (cM_scos((s16)(hit->shape_angle.y - ang)) > 0.866f) {
                        gabi::Local<cXyz> r;
                        cXyz_ml(sp28, r, 1.5f);
                        a->m814.x = r->x;
                        a->m814.z = r->z;
                        a->m814.y = r->y;
                        iVar9 = true;
                    }
                }
            }
        }
    }
    return iVar9;
}
VERIFY(0x024CC52C, damage_tg_acc);

/* 024CC798 */
void se_pickup(Act_c* a) {
    WWHD_FUNC(0x024CC798, void, a);
    if (a->mType == 7) {
        fopAcM_seStart_raw(a, JA_SE_OBJ_BOKKURI_PICK_UP, 0);
    }
}
VERIFY(0x024CC798, se_pickup);

/* 024CC7EC */
bool damage_cc_proc(Act_c* a) {
    WWHD_FUNC(0x024CC7EC, bool, a);
    bool bVar1 = false;
    if (a->mCyl.ChkAtHit()) {
        if (a->mType != 7) {
            damaged(a, 3, nullptr);
            bVar1 = true;
        }
        a->mCyl.ClrAtHit();
    } else if (a->mCyl.ChkTgHit()) {
        void* obj = a->mCyl.GetTgHitObj();
        if (obj != nullptr && (!(data(a)->mFlag & Act_c::DATA_FLAG_10_e) || !(obj_GetAtType(obj) & AT_TYPE_HOOKSHOT))) {
            if ((obj_GetAtType(obj) & AT_TYPE_WIND) && !prm_get_stick(a)) {
                set_wind_vec(a);
                if (a->m678 == 2) {
                    mode_walk_init(a);
                }
            } else {
                if (damage_tg_acc(a) && a->m678 == 2 && a->mType == 7) {
                    if (prm_get_stick(a)) {
                        a->m800 = 0x4B0; /* M_attrSpine.m30 */
                        if (prm_get_stick(a)) {
                            se_pickup(a);
                        }
                    }
                    prm_off_stick(a);
                    mode_walk_init(a);
                }

                s32 type = a->mType;
                u32 at = obj_GetAtType(obj);
                if (type == 3 || type == 4) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(a, 7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_UNK8) {
                        damaged(a, 3, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SKULL_HAMMER) {
                        damaged(a, 1, nullptr);
                        bVar1 = true;
                    }
                } else if (type == 0 || type == 1 || type == 2 || type == 5 || type == 6 || type == 8 || type == 13 ||
                           type == 14 || type == 15) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(a, 7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SWORD) {
                        damaged(a, 1, nullptr);
                        bVar1 = true;
                    } else {
                        damaged(a, 9, nullptr);
                        bVar1 = true;
                    }
                } else if (type == 7) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(a, 7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SKULL_HAMMER) {
                        damaged(a, 1, nullptr);
                        bVar1 = true;
                    }
                } else if (at & AT_TYPE_BOMB) {
                    damaged(a, 7, nullptr);
                    bVar1 = true;
                }
            }
        }

        if (!bVar1) {
            /* harness workaround: the facts say HitSeStart reads r7; the original holds the data row there */
            Act_c::Data_c* d = data(a);
            daObj_HitSeStart(&a->eyePos, a->current.roomNo, &a->mCyl, d->mSoundID_Hit, gabi::ea(d));
            set_senv(a, data(a)->m62, data(a)->m63);
            s32 type = a->mType;
            if (type == 3 || type == 4 || type == 7) {
                daObj_HitEff_kikuzu(a, &a->mCyl);
            } else if (type == 9 || type == 10 || type == 11 || type == 12) {
                daObj_HitEff_hibana(a, &a->mCyl);
            }
        }
        a->mCyl.ClrTgHit();
    }
    return bVar1;
}
VERIFY(0x024CC7EC, damage_cc_proc);

/* 024CCB44: HD: m504 (set by crr_pos_water) */
u8 chk_sink_water(Act_c* a) {
    WWHD_FUNC(0x024CCB44, u8, a);
    return a->m504;
}
VERIFY(0x024CCB44, chk_sink_water);

/* 024CCB4C */
bool chk_sink_lava(Act_c* a) {
    WWHD_FUNC(0x024CCB4C, bool, a);
    if (a->m67F != 0) {
        return false;
    }
    f32 m04 = data(a)->m04;
    f32 fVar1 = (20.0f - m04 >= 0.0f) ? m04 : 20.0f; /* fsel */
    return a->m4F8 > a->current.pos.y + fVar1;
}
VERIFY(0x024CCB4C, chk_sink_lava);

/* 024CCBA0 */
void se_fall_water(Act_c* a) {
    WWHD_FUNC(0x024CCBA0, void, a);
    cBgS_PolyInfo* p0 = a->m505 ? nullptr : Acch_wtr(a);
    u32 snd = mtrl_snd(p0, Acch_gnd(&a->mAcch), 0x13, true);
    fopAcM_seStart_raw(a, data(a)->mSoundID_FallWater, snd);
    set_senv(a, data(a)->m64, data(a)->m65);
}
VERIFY(0x024CCBA0, se_fall_water);

/* 024CCCEC */
void eff_hit_water_splash(Act_c* a) {
    WWHD_FUNC(0x024CCCEC, void, a);
    gabi::Local<cXyz> sp08;
    sp08->set(a->current.pos.x, a->m4FC, a->current.pos.z);
    fopKyM_createWpillar(sp08, data(a)->mBC, data(a)->mC0, 0);
}
VERIFY(0x024CCCEC, eff_hit_water_splash);

static inline void limit_speed(Act_c* a) {
    f32 sp = std_sqrtf(gabi::fmadds(a->speed.y, a->speed.y, a->speedF * a->speedF));
    if (sp > data(a)->m50) {
        f32 fVar4 = data(a)->m50 / sp;
        PSVECScale(&a->speed, &a->speed, fVar4);
        a->speedF = a->speedF * fVar4;
    }
}

/* 024CCD44 */
void mode_afl_init(Act_c* a) {
    WWHD_FUNC(0x024CCD44, void, a);
    a->mCyl.OffAtSPrmBit(1);
    a->mCyl.OnTgSPrmBit(1);
    a->mCyl.OnCoSPrmBit(1);
    Acch_ClrRoofNone(&a->mAcch);
    Acch_ClrWallNone(&a->mAcch);
    a->mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&a->mAcch);
    a->mAcch.OffLineCheck();
    a->m67F = 0;
    limit_speed(a);
    attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    a->m804 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    s16 v = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    a->m678 = 7;
    a->m806 = v;
}
VERIFY(0x024CCD44, mode_afl_init);

/* 024CCE64 (not named by the matcher): mode_sink_init */
void mode_sink_init(Act_c* a) {
    WWHD_FUNC(0x024CCE64, void, a);
    a->mCyl.OffAtSPrmBit(1);
    a->mCyl.OnTgSPrmBit(1);
    a->mCyl.OnCoSPrmBit(1);
    a->mStts.Init(0xFF, 0xFF, a);
    a->mAcch.SetRoofNone();
    a->mAcch.SetWallNone();
    a->mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&a->mAcch);
    a->mAcch.OffLineCheck();
    a->m67F = 0;
    a->gravity = data(a)->mGravity + data(a)->m3C;
    limit_speed(a);
    attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    a->m678 = 6;
}
VERIFY(0x024CCE64, mode_sink_init);

/* 024CCF84 */
void se_fall_lava(Act_c* a) {
    WWHD_FUNC(0x024CCF84, void, a);
    u32 snd = mtrl_snd(Yogan_poly(a), Acch_gnd(&a->mAcch), 0x17, false);
    fopAcM_seStart_raw(a, data(a)->mSoundID_FallLava, snd);
    set_senv(a, data(a)->m64, data(a)->m65);
}
VERIFY(0x024CCF84, se_fall_lava);

/* 024CD0B8 */
void eff_hit_lava_splash(Act_c* a) {
    WWHD_FUNC(0x024CD0B8, void, a);
    gabi::Local<cXyz> sp08;
    sp08->set(a->current.pos.x, a->m4F8, a->current.pos.z);
    fopKyM_createMpillar(sp08, data(a)->mC4);
}
VERIFY(0x024CD0B8, eff_hit_lava_splash);

/* 024CD108 */
void damaged_lava(Act_c* a) {
    WWHD_FUNC(0x024CD108, void, a);
    gabi::Local<cXyz> sp08;
    f32 h = a->m4F8 - 80.0f;
    sp08->set(a->current.pos.x, h, a->current.pos.z);
    cXyz* p;
    if (a->current.pos.y < h) {
        p = sp08;
    } else {
        p = &a->current.pos;
    }
    damaged(a, 2, Yogan_poly(a), true, p);
}
VERIFY(0x024CD108, damaged_lava);

/* 024CD170 */
bool chk_sinkdown_water(Act_c* a) {
    WWHD_FUNC(0x024CD170, bool, a);
    return a->m4FC != G_CM3D_F_INF_NEG && a->m4FC > a->current.pos.y + (f32)data(a)->mAcchRoofHeight + 50.0f;
}
VERIFY(0x024CD170, chk_sinkdown_water);

/* 024CD1E8 */
bool damage_bg_proc(Act_c* a) {
    WWHD_FUNC(0x024CD1E8, bool, a);
    bool uVar6 = a->mAcch.ChkGroundHit();
    bool cVar2 = chk_sink_water(a);
    bool cVar3 = chk_sink_lava(a);
    bool bVar7 = false;

    if (a->m678 == 2 || a->m678 == 3) {
        if (cVar2) {
            se_fall_water(a);
            eff_hit_water_splash(a);
            init_rot_clean(a);
            /* HD: NaN goes to mode_afl_init (bge) */
            if (data(a)->mGravity + data(a)->m3C < 0.0f) {
                mode_sink_init(a);
            } else {
                mode_afl_init(a);
            }
        } else if (cVar3) {
            se_fall_lava(a);
            eff_hit_lava_splash(a);
            damaged_lava(a);
            bVar7 = true;
        }
    } else if (a->m678 == 5) {
        if (cVar2) {
            se_fall_water(a);
            eff_hit_water_splash(a);
            if (data(a)->mGravity + data(a)->m3C < 0.0f) {
                mode_sink_init(a);
            } else {
                mode_afl_init(a);
            }
        } else if (cVar3) {
            se_fall_lava(a);
            eff_hit_lava_splash(a);
            damaged_lava(a);
            bVar7 = true;
        } else if (a->mType == 7 && uVar6) {
            mode_walk_init(a);
        }
    } else if (a->m678 == 6) {
        if (chk_sinkdown_water(a)) {
            damaged(a, 2, a->m505 ? nullptr : Acch_wtr(a), false, &a->current.pos);
            bVar7 = true;
        }
    } else if (a->m678 == 7 && uVar6 && !cVar2) {
        mode_wait_init(a);
    }
    return bVar7;
}
VERIFY(0x024CD1E8, damage_bg_proc);

/* 024CDA98 (not named by the matcher): eff_land_smoke */
void eff_land_smoke(Act_c* a) {
    WWHD_FUNC(0x024CDA98, void, a);
    daObj_make_land_effect(a, gabi::at<u8>(gabi::ea(&a->mAcch) + 0xD4), data(a)->mC8);
}
VERIFY(0x024CDA98, eff_land_smoke);

/* 024CD920 */
void cam_lockoff(Act_c* a) {
    WWHD_FUNC(0x024CD920, void, a);
    dCamera_ForceLockOff(a);
}
VERIFY(0x024CD920, cam_lockoff);

/* 024CD95C */
void eff_drop_water(Act_c* a) {
    WWHD_FUNC(0x024CD95C, void, a);
    for (s32 i = 0; i < 3; i++) {
        if (followEcallBack_isEnd(&a->m70C[i])) {
            dComIfGp_particle_set(dPa_ID_IT_SN_WPOT_PITYA, &a->m700, &a->shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&a->m70C[i], -1, nullptr, nullptr, nullptr);
            break;
        }
    }
    for (s32 i = 0; i < 3; i++) {
        if (followEcallBack_isEnd(&a->m748[i])) {
            dComIfGp_particle_set(dPa_ID_IT_SN_WPOT_PITYA_C, &a->m700, &a->shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&a->m748[i], -1, nullptr, nullptr, nullptr);
            break;
        }
    }
}
VERIFY(0x024CD95C, eff_drop_water);

/* 024CDAB4 */
bool damage_bg_proc_directly(Act_c* a) {
    WWHD_FUNC(0x024CDAB4, bool, a);
    u32 flags = a->mAcch.m_flags;
    bool uVar11 = (flags & dBgS_Acch::GROUND_HIT) != 0;
    bool uVar8 = (flags & dBgS_Acch::GROUND_LANDING) != 0;
    bool iVar10 = false;

    if (a->m678 == 2 || a->m678 == 3) {
        bool uVar7 = (flags & dBgS_Acch::ROOF_HIT) != 0;
        if (uVar8 && a->m6FC - a->current.pos.y > data(a)->m38) {
            damaged(a, 2, nullptr);
            iVar10 = true;
        } else if (uVar11 && uVar7) {
            f32 roofHeight = a->mAcch.GetRoofHeight();
            if (roofHeight < a->current.pos.y + (f32)data(a)->mAcchRoofHeight - 10.0f && roofHeight > a->mAcch.GetGroundH()) {
                damaged(a, 3, nullptr);
                iVar10 = true;
            }
        }
        if (uVar11) {
            a->m6FC = a->current.pos.y;
        }
    } else if (a->m678 == 5) {
        bool uVar7 = (flags & dBgS_Acch::ROOF_HIT) != 0;
        bool uVar9 = (flags & dBgS_Acch::WALL_HIT) != 0;
        bool cVar5 = chk_sink_water(a);
        bool cVar3 = chk_sink_lava(a);
        if (a->mType == 7) {
            if (uVar8 && a->m6FC - a->current.pos.y > data(a)->m38) {
                damaged(a, 2, nullptr);
                iVar10 = true;
            }
            if (uVar11) {
                a->m6FC = a->current.pos.y;
            }
        } else if (uVar11) {
            damaged(a, 2, nullptr);
            iVar10 = true;
        } else if (uVar9) {
            damaged(a, 3, (cBgS_PolyInfo*)&a->mAcchCir);
            iVar10 = true;
        } else if (uVar7) {
            damaged(a, 3, Acch_roof(a));
            iVar10 = true;
        }
        if (uVar11 || uVar9 || uVar7 || cVar5 || cVar3) {
            cam_lockoff(a);
        }
    } else if (a->m678 == 7) {
        a->m6FC = a->current.pos.y;
    }

    if (a->m681 > 0) {
        a->m681 = a->m681 - 1;
    } else if (uVar11) {
        if (a->m680 == 0) {
            u32 st = a->m678;
            if ((st >= 1 && st <= 3) || st == 5) {
                if (!iVar10) {
                    if (a->mType == 2) {
                        eff_drop_water(a);
                    }
                    if (a->mType == 7) {
                        fopAcM_seStart_raw(a, data(a)->m88, 0x2D);
                    } else {
                        u32 snd = GetMtrlSndId(Acch_gnd(&a->mAcch));
                        fopAcM_seStart_raw(a, data(a)->m88, snd);
                    }
                    if (!chk_sink_water(a)) {
                        eff_land_smoke(a);
                    }
                }
                a->m680 = 1;
            }
        }
    } else {
        a->m680 = 0;
    }
    return iVar10;
}
VERIFY(0x024CDAB4, damage_bg_proc_directly);

/* 024CD3CC */
bool damage_kill_proc(Act_c* a) {
    WWHD_FUNC(0x024CD3CC, bool, a);
    bool bVar1 = false;
    if (a->m800 > 0) {
        a->m800 = a->m800 - 1;
        if (a->m800 == 0) {
            damaged(a, 1, nullptr);
            bVar1 = true;
        }
    }
    return bVar1;
}
VERIFY(0x024CD3CC, damage_kill_proc);

/* 024CD41C */
void spec_mode_carry_init(Act_c* a) {
    WWHD_FUNC(0x024CD41C, void, a);
    s32 spec = prm_get_spec(a);
    if (spec != 0x3F && spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &a->m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0 && M_spec_act()[i] != nullptr) {
                fopAcM_setStageLayer(M_spec_act()[i]);
            }
        }
    }
}
VERIFY(0x024CD41C, spec_mode_carry_init);

/* 024CD4B0 */
void se_pickup_carry_init(Act_c* a) {
    WWHD_FUNC(0x024CD4B0, void, a);
    if (a->mType == 7) {
        a->m811 = 1;
    }
}
VERIFY(0x024CD4B0, se_pickup_carry_init);

/* 024CD4C8 */
void mode_carry_init(Act_c* a) {
    WWHD_FUNC(0x024CD4C8, void, a);
    a->mCyl.OffAtSPrmBit(1);
    a->mCyl.OnTgSPrmBit(1);
    a->mCyl.OffCoSPrmBit(1);
    Acch_ClrRoofNone(&a->mAcch);
    Acch_ClrWallNone(&a->mAcch);
    a->mAcch.ClrGrndNone();
    Acch_ClrWaterNone(&a->mAcch);
    a->mAcch.OffLineCheck();
    a->m67F = 0;
    attn_offBit(a, fopAc_Attn_ACTION_CARRY_e);
    a->speedF = 0.0f;
    spec_mode_carry_init(a);
    if (a->mType == 2) {
        fopAcM_seStart_raw(a, JA_SE_OBJ_WPOT_LIFTUP, 0);
    }
    a->m802 = 6;
    a->m684 = (a->actor_status >> 20) & 1; /* fopAcM_checkHookCarryNow */
    if (a->mType == 7 && prm_get_stick(a)) {
        a->m800 = 0x4B0; /* M_attrSpine.m30 */
        se_pickup_carry_init(a);
    }
    prm_off_stick(a);
    a->m685 = 0;
    a->m686 = 0;
    a->m678 = 4;
}
VERIFY(0x024CD4C8, mode_carry_init);

/* 024CD5E0 */
void crr_pos_water(Act_c* a) {
    WWHD_FUNC(0x024CD5E0, void, a);
    f32 fVar1 = gabi::load<f32>(gabi::ea(&a->mAcch) + 0x174 + 0x48); /* m_wtr.GetHeight() */
    bool bVar5 = daSea_ChkArea(a->current.pos.x, a->current.pos.z);
    f32 fVar7 = daSea_calcWave(a->current.pos.x, a->current.pos.z);
    f32 fVar2 = a->current.pos.y + data(a)->m04;
    bool bVar3 = a->mAcch.ChkWaterIn() && fVar2 < fVar1;
    bool bVar4 = bVar5 && fVar2 < fVar7;

    a->m500 = a->m4FC;
    if (bVar3 && bVar4) {
        if (fVar1 > fVar7) {
            bVar4 = false;
        } else {
            bVar3 = false;
        }
    }
    if (bVar3) {
        a->m4FC = fVar1;
        a->m505 = 0;
        a->m504 = 1;
    } else if (bVar4) {
        a->m4FC = fVar7;
        a->m504 = 1;
        a->m505 = 1;
    } else {
        a->m504 = 0;
        a->m505 = 0;
        a->m4FC = G_CM3D_F_INF_NEG;
    }
}
VERIFY(0x024CD5E0, crr_pos_water);

/* 024CD70C */
void crr_pos_lava(Act_c* a) {
    WWHD_FUNC(0x024CD70C, void, a);
    if (a->m67F != 0) {
        a->m4F8 = G_CM3D_F_INF_NEG;
    } else {
        f32 y = a->old.pos.y + data(a)->m04 + 1.0f;
        cXyz* p = gabi::at<cXyz>(gabi::ea(&a->mGndChkYogan) + 0x24); /* SetPos */
        p->z = a->current.pos.z;
        p->x = a->current.pos.x;
        p->y = y;
        a->m4F8 = cBgS_GroundCross(dComIfG_Bgsp(), &a->mGndChkYogan);
    }
}
VERIFY(0x024CD70C, crr_pos_lava);

/* 024CD7AC */
void crr_pos(Act_c* a, const cXyz* arg1) {
    WWHD_FUNC(0x024CD7AC, void, a, arg1);
    f32 sx = arg1->x, sy = arg1->y, sz = arg1->z;
    bool bVar1 = prm_get_moveBg(a);
    if (a->m678 == 0 || a->m678 == 4 || a->m678 == 6 || prm_get_stick(a)) {
        sx = a->current.pos.x;
        sy = a->current.pos.y;
        sz = a->current.pos.z;
        bVar1 = true;
    }
    a->mAcch.CrrPos(dComIfG_Bgsp());
    crr_pos_water(a);
    crr_pos_lava(a);
    if (dBgS_ChkMoveBG_NoDABg_l(Acch_gnd(&a->mAcch))) {
        if (dBgS_ChkMoveBG_l(Acch_gnd(&a->mAcch))) {
            a->m682 = 1;
        }
        if (prm_get_moveBg(a)) {
            a->mParameters = a->mParameters | 0xC000;
            sy = a->mAcch.GetGroundH();
            a->m6FC = sy;
        }
    }
    if (bVar1) {
        a->current.pos.y = sy;
        a->current.pos.x = sx;
        a->current.pos.z = sz;
    }
}
VERIFY(0x024CD7AC, crr_pos);

/* 024CDE40 (static) */
f32 reflect(cXyz* pos, cBgS_PolyInfo* tri, f32 rebound) {
    WWHD_FUNC(0x024CDE40, f32, pos, tri, rebound);
    void* pla = cBgS_GetTriPla(dComIfG_Bgsp(), PolyInfo_GetBgIndex(tri), PolyInfo_GetPolyIndex(tri));
    if (pla != nullptr) {
        gabi::Local<cXyz> sp18;
        C_VECReflect(pos, pla, sp18);
        f32 dot = PSVECDotProduct(sp18, (cXyz*)pla);
        f32 dVar4 = gabi::fnmsubs(1.0f - rebound, dot, 1.0f);
        f32 tmp = std_sqrtf(PSVECSquareMag(pos)) * dVar4;
        gabi::Local<cXyz> r;
        cXyz_ml(sp18, r, tmp);
        pos->copy(*r);
        return dVar4;
    }
    return 0.0f;
}
VERIFY(0x024CDE40, reflect);

/* 024CDF3C */
void bound(Act_c* a, f32 arg1) {
    WWHD_FUNC(0x024CDF3C, void, a, arg1);
    if (a->mType == 7) {
        gabi::Local<cXyz> sp28;
        sp28->set(a->speed.x, arg1, a->speed.z);
        f32 abs = std_sqrtf(PSVECSquareMag(sp28));
        if (abs > 8.0f) {
            u32 flags = a->mAcch.m_flags;
            bool uVar8 = (flags & dBgS_Acch::WALL_HIT) != 0;
            bool uVar7 = (flags & dBgS_Acch::GROUND_LANDING) != 0;
            bool uVar5 = (flags & dBgS_Acch::ROOF_HIT) != 0;
            gabi::Local<cXyz> sp1C;
            sp1C->set(sp28->x, sp28->y, sp28->z);
            f32 dVar9 = 0.0f;
            if (uVar8) {
                f32 v = reflect(sp1C, (cBgS_PolyInfo*)&a->mAcchCir, 0.5f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar7) {
                f32 v = reflect(sp1C, Acch_gnd(&a->mAcch), 0.6f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar5) {
                f32 v = reflect(sp1C, Acch_roof(a), 0.4f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar8 || uVar7 || uVar5) {
                f32 x = sp1C->x, y = sp1C->y, z = sp1C->z;
                a->speed.x = x;
                a->speed.y = y;
                a->speed.z = z;
                gabi::Local<cXyz> xz;
                xz->set(x, 0.0f, z);
                a->speedF = std_sqrtf(PSVECSquareMag(xz)); /* absXZ */
                a->current.angle.y = cM_atan2s(sp1C->x, sp1C->z);
                if (abs > 45.0f) {
                    fopAcM_seStart_raw(a, JA_SE_OBJ_BOKKURI_BOUND, 100);
                } else {
                    s32 v = gabi::ftoi(abs * dVar9 * 2.2222223f);
                    if (v != 0) {
                        fopAcM_seStart_raw(a, JA_SE_OBJ_BOKKURI_BOUND, v);
                    }
                }
            }
        }
    } else if (a->mAcch.ChkWallHit()) {
        a->speedF = a->speedF * 0.9f;
    }
}
VERIFY(0x024CDF3C, bound);
