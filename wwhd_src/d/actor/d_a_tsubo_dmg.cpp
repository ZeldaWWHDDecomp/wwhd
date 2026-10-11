/**
 * d_a_tsubo_dmg.cpp (WWHD)
 * Small carriable objects (pots, skulls, ...): damage, sound/splash, water/lava and bound
 * functions (part B of d_a_tsubo).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tsubo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tsubo_dmg.h"

using daTsuboDamage::Act_c;
using daTsuboDamage::HitObject;
using daTsuboDamage::ShapeAttr;
using daTsuboDamage::Camera;
using daTsuboDamage::CameraPlay;
using daTsuboDamage::EffectTable;

static EffectTable eff_break_proc;

// HD: the tsubo TU's fopAcM_seStart inline has no null checks at all
static inline void fopAcM_seStart_raw(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline u32 GetMtrlSndId(cBgS_PolyInfo* p) { return dBgS_GetMtrlSndId(dComIfG_Bgsp(), p); }
/* cBgS_PolyInfo: poly index u16 +0, bg index u16 +2 */
static inline u16 PolyInfo_GetBgIndex(cBgS_PolyInfo* p) { return p->mBgIndex; }
static inline u16 PolyInfo_GetPolyIndex(cBgS_PolyInfo* p) { return p->mPolyIndex; }
// HD: this camera accessor keeps the original load before reading the actor ID.
static inline void dCamera_ForceLockOff(fopAc_ac_c* a) {
    Camera* camera = reinterpret_cast<CameraPlay*>(dComIfGp_get())->mpCamera;
    tsuboDamage_cameraForceLockOff(&camera->mCamera, a->mBsPcId);
}
/* dPa_followEcallBack::isEnd(): bit 0 of +0x10 */
static inline bool followEcallBack_isEnd(dPa_followEcallBack* f) { return (f->m10 & 1) != 0; }

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

static inline cBgS_PolyInfo* Acch_wtr(Act_c* a) { return reinterpret_cast<cBgS_PolyInfo*>(&a->mAcch.m_wtr); }
static inline cBgS_PolyInfo* Acch_roof(Act_c* a) { return &a->mAcch.m_roof; }
static inline cBgS_PolyInfo* Yogan_poly(Act_c* a) { return static_cast<cBgS_PolyInfo*>(reinterpret_cast<dBgS_GndChk*>(&a->mGndChkYogan)); }

static inline void limit_speed(Act_c* a) {
    f32 sp = std_sqrtf(gabi::fmadds(a->speed.y, a->speed.y, a->speedF * a->speedF));
    if (sp > data(a)->m50) {
        f32 fVar4 = data(a)->m50 / sp;
        PSVECScale(&a->speed, &a->speed, fVar4);
        a->speedF = a->speedF * fVar4;
    }
}

/* 024CD41C */
void daTsuboDamage::Act_c::spec_mode_carry_init() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3F && daTsubo::spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0 && daTsubo::M_spec_act()[i] != nullptr) {
                fopAcM_setStageLayer(daTsubo::M_spec_act()[i]);
            }
        }
    }
}

/* 024CBF64 */
void daTsuboDamage::Act_c::spec_kill() {
    s32 spec = prm_get_spec(this);
    if (spec != 0x3F && daTsubo::spec_is_boko(spec)) {
        Act_c::M7A0* ptr = &m7A0[0];
        for (s32 i = 0; i < 3; i++, ptr++) {
            if (ptr->m04 != 0) {
                fopAc_ac_c* boko = daTsubo::M_spec_act()[i];
                if (boko != nullptr) {
                    s8 room = fopAcM_GetRoomNo(this);
                    ptr->m00 = fpcM_ERROR_PROCESS_ID_e;
                    boko->current.roomNo = room; /* fopAcM_SetRoomNo */
                    fopAcM_cancelCarryNow(boko);
                    fopAcM_delete(boko);
                }
            }
        }
    }
}

/* 024CC4BC */
void daTsuboDamage::Act_c::mode_walk_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    daTsubo::Acch_ClrRoofNone(&mAcch);
    daTsubo::Acch_ClrWallNone(&mAcch);
    mAcch.ClrGrndNone();
    daTsubo::Acch_ClrWaterNone(&mAcch);
    mAcch.OffLineCheck();
    m67F = 0;
    tsuboDamage_init_rot_clean(this);
    m678 = 3;
}

/* 024CD4C8 */
void daTsuboDamage::Act_c::mode_carry_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OffCoSPrmBit(1);
    daTsubo::Acch_ClrRoofNone(&mAcch);
    daTsubo::Acch_ClrWallNone(&mAcch);
    mAcch.ClrGrndNone();
    daTsubo::Acch_ClrWaterNone(&mAcch);
    mAcch.OffLineCheck();
    m67F = 0;
    daTsubo::attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    speedF = 0.0f;
    spec_mode_carry_init();
    if (mType == 2) {
        fopAcM_seStart_raw(this, JA_SE_OBJ_WPOT_LIFTUP, 0);
    }
    m802 = 6;
    m684 = (actor_status & fopAcStts_HOOK_CARRY_e) != 0; /* fopAcM_checkHookCarryNow */
    if (mType == 7 && prm_get_stick(this)) {
        m800 = 0x4B0; /* M_attrSpine.m30 */
        se_pickup_carry_init();
    }
    prm_off_stick(this);
    m685 = 0;
    m686 = 0;
    m678 = 4;
}

/* 024CCE64 */
void daTsuboDamage::Act_c::mode_sink_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    mStts.Init(0xFF, 0xFF, this);
    mAcch.SetRoofNone();
    mAcch.SetWallNone();
    mAcch.ClrGrndNone();
    daTsubo::Acch_ClrWaterNone(&mAcch);
    mAcch.OffLineCheck();
    m67F = 0;
    gravity = data(this)->mGravity + data(this)->m3C;
    limit_speed(this);
    daTsubo::attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    m678 = 6;
}

/* 024CCD44 */
void daTsuboDamage::Act_c::mode_afl_init() {
    mCyl.OffAtSPrmBit(1);
    mCyl.OnTgSPrmBit(1);
    mCyl.OnCoSPrmBit(1);
    daTsubo::Acch_ClrRoofNone(&mAcch);
    daTsubo::Acch_ClrWallNone(&mAcch);
    mAcch.ClrGrndNone();
    daTsubo::Acch_ClrWaterNone(&mAcch);
    mAcch.OffLineCheck();
    m67F = 0;
    limit_speed(this);
    daTsubo::attn_offBit(this, fopAc_Attn_ACTION_CARRY_e);
    m804 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    s16 v = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    m678 = 7;
    m806 = v;
}

// HD: item actions are clamped, the effect process differs, and the table index is asserted.
/* 024CC004: explicit entry for the overloaded damaged method */
void daTsuboDamage::Act_c::damaged(s32 arg1, cBgS_PolyInfo* arg2, bool arg3, const cXyz* arg4) {
    WWHD_FUNC(0x024CC004, void, this, arg1, arg2, arg3, arg4);
    s32 action = arg1 < 0 ? 0 : (arg1 >= 0x10 ? 0xF : arg1);
    s32 itemNo = prm_get_itemNo(this);
    s32 itemBitNo = prm_get_itemSave(this);
    gabi::Local<csXyz> sp08;
    csXyz_ct(sp08, 0, home.angle.y, 0);
    fopAcM_createItemFromTable(&current.pos, itemNo, itemBitNo, home.roomNo, 0, sp08, action, nullptr);
    fopAcM_cancelCarryNow(this);
    actor_status = actor_status & ~fopAcStts_HOOK_CARRY_e; /* fopAcM_cancelHookCarryNow */

    if (arg3) {
        if (mType == 2 && arg4 != nullptr) {
            tsuboDamage_fopKyM_create(0x17, tevStr.mRoomNo, arg4, nullptr, 0);
        }
        // HD: the original assert text calls these M_type and Type_Max.
        if ((u32)mType >= 0x10)
            JUT_ASSERT_fail("d_a_tsubo.cpp", 0xD62, "mType < ARRAY_SIZE(eff_break_proc)");
        (this->*eff_break_proc.mProc[mType])();
        se_break(arg2);
    } else {
        spec_kill();
    }
}

VERIFY(0x024CC004, (void (daTsuboDamage::Act_c::*)(s32, cBgS_PolyInfo*, bool, const cXyz*))&daTsuboDamage::Act_c::damaged);

/* 024CC238: explicit entry for the overloaded damaged method */
void daTsuboDamage::Act_c::damaged(s32 arg1, cBgS_PolyInfo* arg2) {
    WWHD_FUNC(0x024CC238, void, this, arg1, arg2);
    damaged(arg1, arg2, true, &current.pos);
}

VERIFY(0x024CC238, (void (daTsuboDamage::Act_c::*)(s32, cBgS_PolyInfo*))&daTsuboDamage::Act_c::damaged);

/* 024CD108 */
void daTsuboDamage::Act_c::damaged_lava() {
    cXyz sp08;
    f32 h = m4F8 - 80.0f;
    sp08.set(current.pos.x, h, current.pos.z);
    cXyz* p;
    if (current.pos.y < h) {
        p = &sp08;
    } else {
        p = &current.pos;
    }
    damaged(2, Yogan_poly(this), true, p);
}

/* 024CC52C */
bool daTsuboDamage::Act_c::damage_tg_acc() {
    HitObject* obj = reinterpret_cast<HitObject*>(mCyl.GetTgHitObj());
    bool iVar9 = false;
    if (obj != nullptr) {
        f32 fVar1 = 1.5f;
        f32 fVar2 = 0.0f;
        if (obj->GetAtType() & AT_TYPE_CUT_MASK) {
            fVar1 = 2.0f;
            fVar2 = 9.0f;
        }
        // HD: fVar1 > 0 folded
        f32 temp1 = std_sqrtf(PSVECSquareMag(&m814));
        cXyz sp34;
        cXyz* rv = &mCyl.mGObjTg.mRVec;
        sp34.z = rv->z;
        sp34.y = rv->y;
        sp34.x = rv->x;
        f32 temp2 = std_sqrtf(PSVECSquareMag(&sp34));
        if (fVar1 > temp1 && temp2 > temp1) {
            if (temp2 < fVar1) {
                m814.x = sp34.x;
                m814.z = sp34.z;
                m814.y = sp34.y + fVar2;
            } else {
                cXyz r;
                cXyz_ml(&sp34, &r, fVar1 / temp2);
                m814.x = r.x;
                m814.y = r.y + fVar2;
                m814.z = r.z;
            }
            return true;
        }

        if (obj->GetAtType() & AT_TYPE_WIND) {
            ShapeAttr* shapeAttr = obj->GetShapeAttr();
            cXyz sp28;
            sp28.x = cXyz_Zero->x;
            sp28.y = cXyz_Zero->y;
            sp28.z = cXyz_Zero->z;
            if (shapeAttr->GetNVec( &current.pos, &sp28)) {
                fopAc_ac_c* hit = mCyl.GetTgHitAc();
                if (hit != nullptr && hit != nullptr && hit->mProfName == fpcNm_PLAYER_e) {
                    s16 ang = cM_atan2s(sp28.x, sp28.z);
                    if (cM_scos((s16)(hit->shape_angle.y - ang)) > 0.866f) {
                        cXyz r;
                        cXyz_ml(&sp28, &r, 1.5f);
                        m814.x = r.x;
                        m814.z = r.z;
                        m814.y = r.y;
                        iVar9 = true;
                    }
                }
            }
        }
    }
    return iVar9;
}

/* 024CC7EC */
bool daTsuboDamage::Act_c::damage_cc_proc() {
    bool bVar1 = false;
    if (mCyl.ChkAtHit()) {
        if (mType != 7) {
            damaged(3, nullptr);
            bVar1 = true;
        }
        mCyl.ClrAtHit();
    } else if (mCyl.ChkTgHit()) {
        HitObject* obj = reinterpret_cast<HitObject*>(mCyl.GetTgHitObj());
        if (obj != nullptr && (!(data(this)->mFlag & Act_c::DATA_FLAG_10_e) || !(obj->GetAtType() & AT_TYPE_HOOKSHOT))) {
            if ((obj->GetAtType() & AT_TYPE_WIND) && !prm_get_stick(this)) {
                set_wind_vec();
                if (m678 == 2) {
                    mode_walk_init();
                }
            } else {
                if (damage_tg_acc() && m678 == 2 && mType == 7) {
                    if (prm_get_stick(this)) {
                        m800 = 0x4B0; /* M_attrSpine.m30 */
                        if (prm_get_stick(this)) {
                            se_pickup();
                        }
                    }
                    prm_off_stick(this);
                    mode_walk_init();
                }

                s32 type = mType;
                u32 at = obj->GetAtType();
                if (type == 3 || type == 4) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_UNK8) {
                        damaged(3, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SKULL_HAMMER) {
                        damaged(1, nullptr);
                        bVar1 = true;
                    }
                } else if (type == 0 || type == 1 || type == 2 || type == 5 || type == 6 || type == 8 || type == 13 ||
                           type == 14 || type == 15) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SWORD) {
                        damaged(1, nullptr);
                        bVar1 = true;
                    } else {
                        damaged(9, nullptr);
                        bVar1 = true;
                    }
                } else if (type == 7) {
                    if (at & AT_TYPE_BOMB) {
                        damaged(7, nullptr);
                        bVar1 = true;
                    } else if (at & AT_TYPE_SKULL_HAMMER) {
                        damaged(1, nullptr);
                        bVar1 = true;
                    }
                } else if (at & AT_TYPE_BOMB) {
                    damaged(7, nullptr);
                    bVar1 = true;
                }
            }
        }

        if (!bVar1) {
            /* harness workaround: the facts say HitSeStart reads r7; the original holds the data row there */
            Act_c::Data_c* d = data(this);
            tsuboDamage_daObj_HitSeStart(&eyePos, current.roomNo, &mCyl, d->mSoundID_Hit, gabi::ea(d));
            tsuboDamage_set_senv(this, data(this)->m62, data(this)->m63);
            s32 type = mType;
            if (type == 3 || type == 4 || type == 7) {
                tsuboDamage_daObj_HitEff_kikuzu(this, &mCyl);
            } else if (type == 9 || type == 10 || type == 11 || type == 12) {
                tsuboDamage_daObj_HitEff_hibana(this, &mCyl);
            }
        }
        mCyl.ClrTgHit();
    }
    return bVar1;
}

/* 024CD1E8 */
bool daTsuboDamage::Act_c::damage_bg_proc() {
    bool uVar6 = mAcch.ChkGroundHit();
    bool cVar2 = chk_sink_water();
    bool cVar3 = chk_sink_lava();
    bool bVar7 = false;

    if (m678 == 2 || m678 == 3) {
        if (cVar2) {
            se_fall_water();
            eff_hit_water_splash();
            tsuboDamage_init_rot_clean(this);
            // HD: NaN goes to mode_afl_init (bge)
            if (data(this)->mGravity + data(this)->m3C < 0.0f) {
                mode_sink_init();
            } else {
                mode_afl_init();
            }
        } else if (cVar3) {
            se_fall_lava();
            eff_hit_lava_splash();
            damaged_lava();
            bVar7 = true;
        }
    } else if (m678 == 5) {
        if (cVar2) {
            se_fall_water();
            eff_hit_water_splash();
            if (data(this)->mGravity + data(this)->m3C < 0.0f) {
                mode_sink_init();
            } else {
                mode_afl_init();
            }
        } else if (cVar3) {
            se_fall_lava();
            eff_hit_lava_splash();
            damaged_lava();
            bVar7 = true;
        } else if (mType == 7 && uVar6) {
            mode_walk_init();
        }
    } else if (m678 == 6) {
        if (chk_sinkdown_water()) {
            damaged(2, m505 ? nullptr : Acch_wtr(this), false, &current.pos);
            bVar7 = true;
        }
    } else if (m678 == 7 && uVar6 && !cVar2) {
        tsuboDamage_mode_wait_init(this);
    }
    return bVar7;
}

/* 024CDAB4 */
bool daTsuboDamage::Act_c::damage_bg_proc_directly() {
    u32 flags = mAcch.m_flags;
    bool uVar11 = (flags & dBgS_Acch::GROUND_HIT) != 0;
    bool uVar8 = (flags & dBgS_Acch::GROUND_LANDING) != 0;
    bool iVar10 = false;

    if (m678 == 2 || m678 == 3) {
        bool uVar7 = (flags & dBgS_Acch::ROOF_HIT) != 0;
        if (uVar8 && m6FC - current.pos.y > data(this)->m38) {
            damaged(2, nullptr);
            iVar10 = true;
        } else if (uVar11 && uVar7) {
            f32 roofHeight = mAcch.GetRoofHeight();
            if (roofHeight < current.pos.y + (f32)data(this)->mAcchRoofHeight - 10.0f && roofHeight > mAcch.GetGroundH()) {
                damaged(3, nullptr);
                iVar10 = true;
            }
        }
        if (uVar11) {
            m6FC = current.pos.y;
        }
    } else if (m678 == 5) {
        bool uVar7 = (flags & dBgS_Acch::ROOF_HIT) != 0;
        bool uVar9 = (flags & dBgS_Acch::WALL_HIT) != 0;
        bool cVar5 = chk_sink_water();
        bool cVar3 = chk_sink_lava();
        if (mType == 7) {
            if (uVar8 && m6FC - current.pos.y > data(this)->m38) {
                damaged(2, nullptr);
                iVar10 = true;
            }
            if (uVar11) {
                m6FC = current.pos.y;
            }
        } else if (uVar11) {
            damaged(2, nullptr);
            iVar10 = true;
        } else if (uVar9) {
            damaged(3, (cBgS_PolyInfo*)&mAcchCir);
            iVar10 = true;
        } else if (uVar7) {
            damaged(3, Acch_roof(this));
            iVar10 = true;
        }
        if (uVar11 || uVar9 || uVar7 || cVar5 || cVar3) {
            cam_lockoff();
        }
    } else if (m678 == 7) {
        m6FC = current.pos.y;
    }

    if (m681 > 0) {
        m681 = m681 - 1;
    } else if (uVar11) {
        if (m680 == 0) {
            u32 st = m678;
            if ((st >= 1 && st <= 3) || st == 5) {
                if (!iVar10) {
                    if (mType == 2) {
                        eff_drop_water();
                    }
                    if (mType == 7) {
                        fopAcM_seStart_raw(this, data(this)->m88, 0x2D);
                    } else {
                        u32 snd = GetMtrlSndId(daTsubo::Acch_gnd(&mAcch));
                        fopAcM_seStart_raw(this, data(this)->m88, snd);
                    }
                    if (!chk_sink_water()) {
                        eff_land_smoke();
                    }
                }
                m680 = 1;
            }
        }
    } else {
        m680 = 0;
    }
    return iVar10;
}

/* 024CD3CC */
bool daTsuboDamage::Act_c::damage_kill_proc() {
    bool bVar1 = false;
    if (m800 > 0) {
        m800 = m800 - 1;
        if (m800 == 0) {
            damaged(1, nullptr);
            bVar1 = true;
        }
    }
    return bVar1;
}

/* 024CD7AC */
void daTsuboDamage::Act_c::crr_pos(const cXyz* arg1) {
    f32 sx = arg1->x, sy = arg1->y, sz = arg1->z;
    bool bVar1 = prm_get_moveBg(this);
    if (m678 == 0 || m678 == 4 || m678 == 6 || prm_get_stick(this)) {
        sx = current.pos.x;
        sy = current.pos.y;
        sz = current.pos.z;
        bVar1 = true;
    }
    mAcch.CrrPos(dComIfG_Bgsp());
    crr_pos_water();
    crr_pos_lava();
    if (tsuboDamage_ChkMoveBG_NoDABg(dComIfG_Bgsp(), daTsubo::Acch_gnd(&mAcch))) {
        if (tsuboDamage_ChkMoveBG(dComIfG_Bgsp(), daTsubo::Acch_gnd(&mAcch))) {
            m682 = 1;
        }
        if (prm_get_moveBg(this)) {
            mParameters = mParameters | 0xC000;
            sy = mAcch.GetGroundH();
            m6FC = sy;
        }
    }
    if (bVar1) {
        current.pos.y = sy;
        current.pos.x = sx;
        current.pos.z = sz;
    }
}

/* 024CD5E0 */
void daTsuboDamage::Act_c::crr_pos_water() {
    f32 fVar1 = mAcch.m_wtr.GetHeight(); /* m_wtr.GetHeight() */
    bool bVar5 = daSea_ChkArea(current.pos.x, current.pos.z);
    f32 fVar7 = daSea_calcWave(current.pos.x, current.pos.z);
    f32 fVar2 = current.pos.y + data(this)->m04;
    bool bVar3 = mAcch.ChkWaterIn() && fVar2 < fVar1;
    bool bVar4 = bVar5 && fVar2 < fVar7;

    m500 = m4FC;
    if (bVar3 && bVar4) {
        if (fVar1 > fVar7) {
            bVar4 = false;
        } else {
            bVar3 = false;
        }
    }
    if (bVar3) {
        m4FC = fVar1;
        m505 = 0;
        m504 = 1;
    } else if (bVar4) {
        m4FC = fVar7;
        m504 = 1;
        m505 = 1;
    } else {
        m504 = 0;
        m505 = 0;
        m4FC = G_CM3D_F_INF_NEG;
    }
}

/* 024CD70C */
void daTsuboDamage::Act_c::crr_pos_lava() {
    if (m67F != 0) {
        m4F8 = G_CM3D_F_INF_NEG;
    } else {
        f32 y = old.pos.y + data(this)->m04 + 1.0f;
        cXyz* p = &reinterpret_cast<dBgS_GndChk*>(&mGndChkYogan)->m_pos; /* SetPos */
        p->z = current.pos.z;
        p->x = current.pos.x;
        p->y = y;
        m4F8 = cBgS_GroundCross(dComIfG_Bgsp(), &mGndChkYogan);
    }
}

/* 024CDE40 */
static f32 reflect(cXyz* pos, cBgS_PolyInfo* tri, f32 rebound) {
    void* pla = cBgS_GetTriPla(dComIfG_Bgsp(), PolyInfo_GetBgIndex(tri), PolyInfo_GetPolyIndex(tri));
    if (pla != nullptr) {
        cXyz sp18;
        tsuboDamage_C_VECReflect(pos, pla, &sp18);
        f32 dot = PSVECDotProduct(&sp18, (cXyz*)pla);
        f32 dVar4 = gabi::fnmsubs(1.0f - rebound, dot, 1.0f);
        f32 tmp = std_sqrtf(PSVECSquareMag(pos)) * dVar4;
        cXyz r;
        cXyz_ml(&sp18, &r, tmp);
        pos->copy(r);
        return dVar4;
    }
    return 0.0f;
}


/* 024CDF3C */
void daTsuboDamage::Act_c::bound(f32 arg1) {
    if (mType == 7) {
        cXyz sp28;
        sp28.set(speed.x, arg1, speed.z);
        f32 abs = std_sqrtf(PSVECSquareMag(&sp28));
        if (abs > 8.0f) {
            u32 flags = mAcch.m_flags;
            bool uVar8 = (flags & dBgS_Acch::WALL_HIT) != 0;
            bool uVar7 = (flags & dBgS_Acch::GROUND_LANDING) != 0;
            bool uVar5 = (flags & dBgS_Acch::ROOF_HIT) != 0;
            cXyz sp1C;
            sp1C.set(sp28.x, sp28.y, sp28.z);
            f32 dVar9 = 0.0f;
            if (uVar8) {
                f32 v = reflect(&sp1C, (cBgS_PolyInfo*)&mAcchCir, 0.5f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar7) {
                f32 v = reflect(&sp1C, daTsubo::Acch_gnd(&mAcch), 0.6f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar5) {
                f32 v = reflect(&sp1C, Acch_roof(this), 0.4f);
                if (v > dVar9) dVar9 = v;
            }
            if (uVar8 || uVar7 || uVar5) {
                f32 x = sp1C.x, y = sp1C.y, z = sp1C.z;
                speed.x = x;
                speed.y = y;
                speed.z = z;
                cXyz xz;
                xz.set(x, 0.0f, z);
                speedF = std_sqrtf(PSVECSquareMag(&xz)); /* absXZ */
                current.angle.y = cM_atan2s(sp1C.x, sp1C.z);
                if (abs > 45.0f) {
                    fopAcM_seStart_raw(this, JA_SE_OBJ_BOKKURI_BOUND, 100);
                } else {
                    s32 v = gabi::ftoi(abs * dVar9 * 2.2222223f);
                    if (v != 0) {
                        fopAcM_seStart_raw(this, JA_SE_OBJ_BOKKURI_BOUND, v);
                    }
                }
            }
        }
    } else if (mAcch.ChkWallHit()) {
        speedF = speedF * 0.9f;
    }
}

/* 024CC244 */
void daTsuboDamage::Act_c::set_wind_vec() {
    HitObject* obj = reinterpret_cast<HitObject*>(mCyl.GetTgHitObj());
    if (obj != nullptr && (obj->GetAtType() & AT_TYPE_WIND)) {
        cXyz sp48;
        cXyz* rv = &mCyl.mGObjTg.mRVec;
        sp48.x = rv->x;
        sp48.y = rv->y;
        sp48.z = rv->z;
        f32 fVar7 = PSVECSquareMag(&sp48);
        if (fVar7 > 31684.0f) {
            PSVECScale(&sp48, &sp48, 178.0f / std_sqrtf(fVar7));
        }
        ShapeAttr* shapeAttr = obj->GetShapeAttr();
        cXyz sp3C;
        sp3C.x = cXyz_Zero->x;
        sp3C.y = cXyz_Zero->y;
        sp3C.z = cXyz_Zero->z;
        f32 fVar1 = 1.0f;
        if (shapeAttr->GetNVec( &current.pos, &sp3C)) {
            PSVECScale(&sp3C, &sp3C, 45.0f);
            PSVECSquareMag(&m7F4);
            fopAc_ac_c* hit = mCyl.GetTgHitAc();
            if (hit != nullptr && hit != nullptr && hit->mProfName == fpcNm_PLAYER_e) {
                s16 ang = cM_atan2s(sp3C.x, sp3C.z);
                f32 cos = cM_scos((s16)(hit->shape_angle.y - ang));
                if (cos > 0.866f) {
                    fVar1 = cos + cos + 1.0f;
                } else {
                    fVar1 = 0.0f;
                }
            }
        }
        // HD: fsel on 0.01 - fVar7
        f32 m1, m2;
        if (0.01f - fVar7 >= 0.0f) {
            m1 = 0.0f;
            m2 = 1.0f;
        } else {
            m1 = 1.0f;
            m2 = 0.05f;
        }
        // HD: keep the hidden-result temporaries and the original evaluation order.
        cXyz t1;
        cXyz t2;
        cXyz t3;
        cXyz res;
        cXyz_ml(&sp48, &t1, m1);
        cXyz_ml(&sp3C, &t2, m2);
        cXyz_ml(&t2, &t3, fVar1);
        cXyz_pl(&t1, &res, &t3);
        m7F4.copy(res);
    }
}

/* 024CD95C */
void daTsuboDamage::Act_c::eff_drop_water() {
    for (s32 i = 0; i < 3; i++) {
        if (followEcallBack_isEnd(&m70C[i])) {
            dComIfGp_particle_set(dPa_ID_IT_SN_WPOT_PITYA, &m700, &shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&m70C[i], -1, nullptr, nullptr, nullptr);
            break;
        }
    }
    for (s32 i = 0; i < 3; i++) {
        if (followEcallBack_isEnd(&m748[i])) {
            dComIfGp_particle_set(dPa_ID_IT_SN_WPOT_PITYA_C, &m700, &shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&m748[i], -1, nullptr, nullptr, nullptr);
            break;
        }
    }
}

/* 024CDA98 */
void daTsuboDamage::Act_c::eff_land_smoke() {
    tsuboDamage_daObj_make_land_effect(this, &mAcch.m_gnd, data(this)->mC8);
}

/* 024CCCEC */
void daTsuboDamage::Act_c::eff_hit_water_splash() {
    cXyz sp08;
    sp08.set(current.pos.x, m4FC, current.pos.z);
    fopKyM_createWpillar(&sp08, data(this)->mBC, data(this)->mC0, 0);
}

/* 024CD0B8 */
void daTsuboDamage::Act_c::eff_hit_lava_splash() {
    cXyz sp08;
    sp08.set(current.pos.x, m4F8, current.pos.z);
    tsuboDamage_fopKyM_createMpillar(&sp08, data(this)->mC4);
}

/* 024CCB4C */
bool daTsuboDamage::Act_c::chk_sink_lava() {
    if (m67F != 0) {
        return false;
    }
    f32 m04 = data(this)->m04;
    f32 fVar1 = (20.0f - m04 >= 0.0f) ? m04 : 20.0f; /* fsel */
    return m4F8 > current.pos.y + fVar1;
}

// HD: the water correction caches this result.
/* 024CCB44 */
u8 daTsuboDamage::Act_c::chk_sink_water() {
    return m504;
}

/* 024CD170 */
bool daTsuboDamage::Act_c::chk_sinkdown_water() {
    return m4FC != G_CM3D_F_INF_NEG && m4FC > current.pos.y + (f32)data(this)->mAcchRoofHeight + 50.0f;
}

/* 024CCBA0 */
void daTsuboDamage::Act_c::se_fall_water() {
    cBgS_PolyInfo* p0 = m505 ? nullptr : Acch_wtr(this);
    u32 snd = mtrl_snd(p0, daTsubo::Acch_gnd(&mAcch), 0x13, true);
    fopAcM_seStart_raw(this, data(this)->mSoundID_FallWater, snd);
    tsuboDamage_set_senv(this, data(this)->m64, data(this)->m65);
}

/* 024CCF84 */
void daTsuboDamage::Act_c::se_fall_lava() {
    u32 snd = mtrl_snd(Yogan_poly(this), daTsubo::Acch_gnd(&mAcch), 0x17, false);
    fopAcM_seStart_raw(this, data(this)->mSoundID_FallLava, snd);
    tsuboDamage_set_senv(this, data(this)->m64, data(this)->m65);
}

/* 024CBE2C */
void daTsuboDamage::Act_c::se_break(cBgS_PolyInfo* p) {
    u32 snd = mtrl_snd(p, daTsubo::Acch_gnd(&mAcch), 0, true);
    fopAcM_seStart_raw(this, data(this)->mSoundID_Break, snd);
    Act_c::Data_c* d = data(this);
    tsuboDamage_set_senv_r6(this, d->m60, d->m61, gabi::ea(d));
}

/* 024CD4B0 */
void daTsuboDamage::Act_c::se_pickup_carry_init() {
    if (mType == 7) {
        m811 = 1;
    }
}

/* 024CC798 */
void daTsuboDamage::Act_c::se_pickup() {
    if (mType == 7) {
        fopAcM_seStart_raw(this, JA_SE_OBJ_BOKKURI_PICK_UP, 0);
    }
}

/* 024CD920 */
void daTsuboDamage::Act_c::cam_lockoff() {
    dCamera_ForceLockOff(this);
}

/* WWHD glue
str 10040CAC
str 10040CBC "mType < ARRAY_SIZE(eff_break_proc)" renamed
data eff_break_proc 10040CE0
ext 024CAA54 void tsuboDamage_init_rot_clean(daTsuboDamage::Act_c* a)
ext 024CAC44 void tsuboDamage_mode_wait_init(daTsuboDamage::Act_c* a)
ext 024CBDE8 void tsuboDamage_set_senv(daTsuboDamage::Act_c* a, s32 p1, s32 p2)
ext 024CBDE8 void tsuboDamage_set_senv_r6(daTsuboDamage::Act_c* a, s32 p1, s32 p2, u32 dataRow)
ext 025DADA4 u32 tsuboDamage_fopKyM_create(s16 name, s32 param, const cXyz* pos, cXyz* scale, u32 fn)
ext 025DAF3C void tsuboDamage_fopKyM_createMpillar(const cXyz* pos, f32 scale)
ext 023129C4 void tsuboDamage_daObj_HitSeStart(cXyz* pos, s32 room, dCcD_GObjInf* obj, u32 se, u32 dataRow)
ext 02312C8C void tsuboDamage_daObj_HitEff_kikuzu(fopAc_ac_c* a, dCcD_Cyl* cyl)
ext 02312E54 void tsuboDamage_daObj_HitEff_hibana(fopAc_ac_c* a, dCcD_Cyl* cyl)
ext 02311CD8 void tsuboDamage_daObj_make_land_effect(fopAc_ac_c* a, dBgS_GndChk* ground, f32 scale)
ext 025052BC void tsuboDamage_cameraForceLockOff(void* camera, u32 actorId)
ext 024EEB2C BOOL tsuboDamage_ChkMoveBG_NoDABg(dBgS* bgs, cBgS_PolyInfo* poly)
ext 024EEABC BOOL tsuboDamage_ChkMoveBG(dBgS* bgs, cBgS_PolyInfo* poly)
ext 028E9F74 void tsuboDamage_C_VECReflect(const cXyz* src, const void* normal, cXyz* dst)
*/
