/**
 * d_a_kanban.cpp (WWHD)
 * Object - Cuttable sign
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kanban.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * daKanban_Execute (with mother_move, parts_move & co. inlined): d_a_kanban_exec.cpp.
 */
#define KANBAN_STORAGE_OWNER
#include "d/actor/d_a_kanban.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/JSystem/JUTAssert.h"

// HD: this translation unit's vtables and common-header static storage.
const gc_tu_data gc_tu = {
    .safeString = 0x10012504,
    .headerStatics = {0x10464A70, 0x101B7E98, 0x10464AB4},
};

#define SAFESTRING_VTBL 0x10012504 /* this TU's sead::SafeString vtable */
#define KANBAN_VTBL 0x1001256C     /* kanban_class vtable (HD virtual destructor) */
#define HIO_VTBL 0x1001255C
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x1001252C, 0x1001254C, 0x1001253C};
#define CYL_AAB_VTBL 0x1001251C    /* cM3dGAab vtable (per TU) */

enum { fpcNm_Bomb2_e = 0x127, fpcNm_KANBAN_e = 0xB6 };
enum { JA_SE_OBJ_FALL_WATER_M = 0x6919, JA_SE_OBJ_BREAK_BOARD = 0x6937 };
enum { dPa_name_ID_AK_JN_OK = 0xD, dPa_name_ID_AK_JN_HAMON00 = 0x33 };

/* ---- file statics ---- */
struct daKanban_HIO_c {
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<s16> m08;
    /* 0x0A */ be<s16> m0A;
    /* 0x0C */ be<f32> m0C;
};
WWHD_SIZE(daKanban_HIO_c, 0x10);
static daKanban_HIO_c l_HIO;
static s32 target_info_count;
struct KanbanTarget { gptr<fopAc_ac_c> actor; };
static KanbanTarget target_info[10] = {};
static u32 l_msgId = 0xFFFFFFFF;
static const u32 cut_parts_arg_data[40] = {
    33554432, 67108864, 8388608, 16777216, 2145386496, 2145386496, 2145386496, 2145386496,
    67108864, 8388608, 16777216, 268435456, 1073741824, 2145386496, 2145386496, 2145386496,
    4194304, 2097152, 33554432, 67108864, 2145386496, 2145386496, 2145386496, 2145386496,
    2097152, 33554432, 67108864, 8388608, 2145386496, 2145386496, 2145386496, 2145386496,
    134217728, 4194304, 2097152, 33554432, 67108864, 8388608, 16777216, 268435456,
};
static const s32 kanban_bdl[11] = {
    3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13,
};
static const u32 kut_size_dt[11] = {
    0, 1472, 1472, 1472, 1472, 1472, 1472, 1472,
    1472, 1472, 1472,
};
static const s16 pl_cut_real_no_dt[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
};
static const s16 pl_cut_no_dt[32] = {
    255, 0, 4, 3, 3, 1, 0, 4,
    4, 4, 1, 3, 1, 3, 1, 1,
    1, 3, 1, 1, 0, 1, 0, 1,
    0, 1, 1, 4, 0, 1, 1, 1,
};
struct KanbanCylinderSource {
    be<u32> mFlags, mAttackType, mAttackPower, mAttackPrm;
    be<u32> mTargetType, mTargetPrm, mCollisionPrm;
    be<u32> mAttackInfo, mAttackFlags, mTargetInfo, mTargetFlags, mCollisionFlags;
    be<f32> mCenter[3];
    be<f32> mRadius, mHeight;
};
WWHD_SIZE(KanbanCylinderSource, 0x44);
static const KanbanCylinderSource cyl_src = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xFF1DFEFF, 0x00000003, 0x00000019, 0x00000000,
    0x00000000, 0x00FF0000, 0x00000000, 0x00000000,
    {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
};
#define REG8_F(i) REG_F(8, i)
#define REG8_S(i) REG_S(8, i)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* kanban_particle_setShipTail: dPa_control_c::set with group 5 */
static inline JPABaseEmitter* kanban_particle_setShipTail(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                            dPa_levelEcallBack* cb) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 5, id, pos, angle, scale, alpha, cb, -1, nullptr, nullptr, nullptr);
}
/* fopAcM_SetStatusMap: the low 6 bits of actor_status */
static inline void kanban_setStatusMap(fopAc_ac_c* a, u32 map) { a->actor_status = (a->actor_status & ~0x3Fu) | map; }
/* dBgS_Acch: MaskWaterIn (flags & WATER_IN), m_wtr.GetHeight() (dBgS_WtrChk +0x48) */
static inline u32 Acch_MaskWaterIn(dBgS_Acch* a) { return a->m_flags & dBgS_Acch::WATER_IN; }
static inline f32 Acch_WtrHeight(dBgS_Acch* a) { return a->m_wtr.GetHeight(); }
/* player (daPy_lk_c, HD): checkHammerQuake() status bit 0x20000 at +0x3C0; getSwordTopPos() +0x3E4 */
static inline bool daPy_checkHammerQuake(fopAc_ac_c* pl) { return (kanban_player(pl)->mQuakeFlags & 0x20000) != 0; }
static inline cXyz* daPy_getSwordTopPos(fopAc_ac_c* pl) { return &kanban_player(pl)->mSwordTopPos; }

/* PowerPC slw / sraw: the shift count is the low 6 bits; 32..63 give 0 / the sign */
static inline u32 ppc_slw(u32 v, s32 n) { n &= 63; return n >= 32 ? 0 : v << n; }
static inline s32 ppc_sraw(s32 v, s32 n) { n &= 63; return n >= 32 ? (v >> 31) : (v >> n); }

/* 0218A548 */
static BOOL daKanban_Draw(kanban_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->actor.current.pos, &i_this->actor.tevStr);
    /* HD: no shadow (sp0C and m570 are gone) */
    f32 s = l_HIO.m0C;
    i_this->actor.scale.set(s, s, s);

    u32 uVar5 = i_this->m294;
    for (s32 i = 0; i < 11; i++, uVar5 >>= 1) {
        if (!(uVar5 & 1)) {
            continue;
        }
        J3DModel* model = i_this->m544[i];
        J3DModel_setBaseScale(model, &i_this->actor.scale);
        MtxTrans(i_this->actor.current.pos.x + i_this->m2E4, i_this->actor.current.pos.y + i_this->m2E8,
                 i_this->actor.current.pos.z + i_this->m2EC, false);
        cMtx_XrotM(calc_mtx(), i_this->m2AC.x);
        cMtx_ZrotM(calc_mtx(), i_this->m2AC.z);
        cMtx_YrotM(calc_mtx(), i_this->actor.shape_angle.y);
        cMtx_XrotM(calc_mtx(), i_this->actor.shape_angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->actor.shape_angle.z);
        MtxTrans(i_this->m2CC.x - i_this->m2E4, i_this->m2CC.y - i_this->m2E8, i_this->m2CC.z - i_this->m2EC, true);
        J3DModel_setBaseTRMtx(model, calc_mtx());
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->actor.tevStr);
        mDoExt_modelUpdateDL(model);
    }
    return TRUE;
}

/* shibuki_set (inlined into sea_water_check) */
static inline void shibuki_set(kanban_class* i_this, cXyz* pos, f32 scaleXZ) {
    if (daSea_ChkArea(i_this->actor.current.pos.x, i_this->actor.current.pos.z)) {
        pos->y = daSea_calcWave(i_this->actor.current.pos.x, i_this->actor.current.pos.z);
    } else if (Acch_MaskWaterIn(&i_this->m350)) {
        pos->y = Acch_WtrHeight(&i_this->m350);
    }
    fopKyM_createWpillar(pos, scaleXZ, 0.5f, 0);
    fopAcM_seStart_nn(&i_this->actor, JA_SE_OBJ_FALL_WATER_M, 0);
}

/* 0218A6F8 */
BOOL sea_water_check(kanban_class* i_this) {
    u8 iVar3 = 0;

    if (REG8_S(1) != 0) {
        return FALSE;
    }

    i_this->m528.copy(i_this->actor.current.pos);
    i_this->actor.gravity = -3.0f;

    if (daSea_ChkArea(i_this->actor.current.pos.x, i_this->actor.current.pos.z)) {
        f32 f1 = daSea_calcWave(i_this->actor.current.pos.x, i_this->actor.current.pos.z);
        i_this->m528.y = f1;
        if (i_this->m2C4 == 1) {
            i_this->m2A0 = 7.0f;
        }
        if (i_this->actor.current.pos.y < (f1 - 120.0f) + REG8_F(6)) {
            i_this->actor.gravity = 0.0f;
            i_this->actor.speedF = 0.0f;
            i_this->actor.speed.set(0.0f, 0.0f, 0.0f);
            i_this->m2BC = (s16)(i_this->m2BC + (s16)gabi::ftoi(REG8_F(7) + 1000.0f));
            f32 sn = cM_ssin(i_this->m2BC);
            f32 f0 = gabi::fmadds(sn, REG8_F(9) + 2.0f, REG8_F(8) + 147.0f);
            cLib_addCalc2(&i_this->actor.current.pos.y, f1 - f0, 1.0f, 30.0f);
            iVar3 = 1;
        }
    } else if (Acch_MaskWaterIn(&i_this->m350)) {
        iVar3 = 2;
        f32 f4 = Acch_WtrHeight(&i_this->m350);
        i_this->m528.y = f4;
        if (i_this->m2C4 == 1) {
            i_this->m2A0 = 7.0f;
        }
        i_this->actor.gravity = -3.0f;
        if (i_this->actor.current.pos.y < f4) {
            i_this->actor.gravity = 0.0f;
            i_this->actor.speedF = 0.0f;
            i_this->actor.speed.set(0.0f, 0.0f, 0.0f);
            i_this->m2BC = (s16)(i_this->m2BC + (s16)gabi::ftoi(REG8_F(7) + 1000.0f));
            f32 sn = cM_ssin(i_this->m2BC);
            f32 f0 = gabi::fmadds(sn, REG8_F(9) + 2.0f, REG8_F(8) + 147.0f);
            cLib_addCalc2(&i_this->actor.current.pos.y, f4 - f0, 1.0f, 30.0f);
            iVar3 = 1;
        }
    }

    if (iVar3 != 0) {
        if (iVar3 == 1 && i_this->m29C == 0) {
            gabi::Local<cXyz> sp30;
            sp30->set(0.5f, 0.5f, 0.5f);
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514); /* HD: remove() is end() */
            kanban_particle_setShipTail(dPa_name_ID_AK_JN_HAMON00, &i_this->m528, nullptr, sp30, 0xFF,
                                          (dPa_levelEcallBack*)&i_this->m514);
            gabi::Local<cXyz> sp24;
            sp24->x = i_this->actor.current.pos.x;
            sp24->y = i_this->actor.current.pos.y;
            sp24->z = i_this->actor.current.pos.z;
            i_this->m2BC = 0;
            i_this->m514.mRate = 0.0f;
            i_this->m29C = 1;
            shibuki_set(i_this, sp24, 0.5f);
        }
        return TRUE;
    }

    i_this->m2A0 = 0.0f;
    return FALSE;
}

/* 0218AAF4 */
void* bom_search_sub(void* ac, void*) {
    if (fopAc_IsActor(ac) && ac != nullptr) { /* HD: null check */
        bool bVar2 = false;
        s16 proc = fpcM_GetName(ac);
        if (proc == fpcNm_BOMB_e) {
            if (kanban_bombState(ac, 0 /* daBomb_c::STATE_0 */)) {
                bVar2 = true;
            }
        } else if (proc == fpcNm_Bomb2_e) {
            if (kanban_bombExploding(ac)) {
                bVar2 = true;
            }
        }
        if (bVar2 && target_info_count < 10) {
            s32 n = target_info_count;
            target_info_count = n + 1;
            target_info[n].actor = (fopAc_ac_c*)ac;
        }
    }
    return nullptr;
}

/* 0218AB94 */
BOOL shock_damage_check(kanban_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    target_info_count = 0;
    for (s32 i = 0; i < 10; i++) {
        target_info[i].actor = nullptr;
    }

    fpcM_Search(gabi::guest_fn(reinterpret_cast<const void*>(bom_search_sub)), &i_this->actor);

    if (target_info_count != 0) {
        for (s32 i = 0; i < target_info_count; i++) {
            fopAc_ac_c* t = target_info[i].actor;
            f32 x = t->current.pos.x - i_this->actor.current.pos.x;
            f32 z = t->current.pos.z - i_this->actor.current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 1000.0f) {
                i_this->m2B2[4] = 100;
                i_this->m2B2[4] = (s16)(100 + REG8_S(4));
                return TRUE;
            }
        }
    }

    if (daPy_checkHammerQuake(player)) {
        cXyz* top = daPy_getSwordTopPos(player);
        f32 x = top->x - i_this->actor.current.pos.x;
        f32 z = top->z - i_this->actor.current.pos.z;
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 1000.0f) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 0218AD44 */
void cut_point_check(kanban_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->m2C4 == 0) {
        s16 pa = fopAcM_searchPlayerAngleY(a_this);
        s16 y = a_this->shape_angle.y - pa;
        s16 sVar3 = (s16)(y < 0 ? -y : y);
        if (sVar3 > 0x6000) {
            i_this->m2C4 = 2;
        } else if (sVar3 > 0x2000 && sVar3 < 0x6000) {
            if ((s32)fopAcM_searchPlayerAngleY(a_this) & 0x80000000) {
                i_this->m2C4 = 2;
            }
        }
    }

    s32 uVar5 = i_this->m2C4 * 8;
    u32 parameters = 0;
    if ((ppc_sraw(i_this->m2C2, i_this->m2C4) & 1) || (i_this->m2C2 & 2)) {
        return;
    }

    i_this->m2C2 = (s16)(i_this->m2C2 | ppc_slw(1, i_this->m2C4));
    if (i_this->m294 & 1) {
        i_this->m294 = 0x7FE;
        i_this->m29B = 0;
    }

    u32 uVar8 = i_this->m294 << 0x14;
    for (s32 i = 0; i < 8; i++) {
        u32 arg = cut_parts_arg_data[uVar5 + i];
        if (arg != 0x7FE00000 && (uVar8 & arg)) {
            uVar8 ^= arg;
            parameters |= arg;
        }
    }

    if (parameters != 0) {
        gabi::Local<cXyz> sp18;
        sp18->x = i_this->m5B0.GetTgHitPosP()->x;
        sp18->y = i_this->m5B0.GetTgHitPosP()->y;
        sp18->z = i_this->m5B0.GetTgHitPosP()->z;
        dComIfGp_particle_set(dPa_name_ID_AK_JN_OK, sp18, &player->shape_angle);
        kanban_setPauseTimer(1);
        a_this->current.angle.z = i_this->m2C4;

        fpc_ProcID fVar6 = kanban_createChild(fpcNm_KANBAN_e, fopAcM_GetID(a_this), parameters, &a_this->current.pos,
                                              fopAcM_GetRoomNo(a_this), &a_this->current.angle, &a_this->scale, 0, 0);
        if (fVar6 != fpcM_ERROR_PROCESS_ID_e) {
            fopAcM_seStart_nn(a_this, JA_SE_OBJ_BREAK_BOARD, 0);
            i_this->m294 = uVar8 >> 0x14;
            if (i_this->m2C4 == 1) {
                i_this->m298 = 0;
                if (i_this->m2C2 & 0x10) {
                    i_this->m298 = 1;
                }
                a_this->current.pos.y = i_this->m2F8.y - 70.0f;
                i_this->m2CC.y = 70.0f;
                i_this->m2E8 = 140.0f;
                i_this->m2A4 = 0.0f;
                i_this->m2A8 = 0.0f;
                i_this->m2C0 = 0xb;
            }
            if ((i_this->m2C2 & 5) == 5 || i_this->m2C4 == 4) {
                i_this->m2A4 = 25.0f;
                i_this->m2A8 = 10.0f;
            }
        }
        a_this->current.angle.z = 0;
    }
}

/* 0218B00C */
BOOL ret_keisan_move(kanban_class* i_this) {
    dComIfGp_get(); /* HD: unused fetch */
    cLib_addCalc0(&i_this->m2A0, 1.0f, 2.0f);
    f32 x = i_this->m2F8.x - i_this->actor.current.pos.x;
    f32 y = i_this->m2F8.y - i_this->actor.current.pos.y;
    f32 z = i_this->m2F8.z - i_this->actor.current.pos.z;
    cLib_addCalcAngleS2(&i_this->actor.current.angle.y, cM_atan2s(x, z), 1, 0x1000);
    i_this->actor.shape_angle.x = i_this->actor.current.angle.x;
    i_this->actor.shape_angle.y = i_this->actor.current.angle.y;
    i_this->actor.shape_angle.z = i_this->actor.current.angle.z;

    if (std_sqrtf(y * y) > 2.0f) {
        cLib_addCalc2(&i_this->actor.current.pos.y, i_this->m2F8.y, 1.0f, 10.0f);
        return FALSE;
    }

    if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 2.0f) {
        i_this->actor.speedF = 0.0f;
        i_this->actor.current.pos.copy(i_this->m2F8);
        return TRUE;
    }

    cLib_addCalc2(&i_this->actor.current.pos.x, i_this->m2F8.x, 1.0f, 10.0f);
    cLib_addCalc2(&i_this->actor.current.pos.z, i_this->m2F8.z, 1.0f, 10.0f);
    return FALSE;
}

/* 0218C81C */
static BOOL daKanban_IsDelete(kanban_class*) {
    return TRUE;
}

/* 0218C824 */
static BOOL daKanban_Delete(kanban_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Kanban");
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514); /* HD: remove() is end() */
    l_msgId = 0xFFFFFFFF;
    return 1;
}

/* 0218C874 */
// HD: HD: setTex (the texture patch of the sign's text) is gone
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    kanban_class* i_this = (kanban_class*)a_this;

    mDoMtx_stack_c::transS(i_this->actor.current.pos.x, i_this->actor.current.pos.y, i_this->actor.current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->actor.shape_angle.y);

    u32 uVar5 = i_this->m294;
    for (s32 i = 0; i < 11; i++, uVar5 >>= 1) {
        if (!i_this->m290 || (uVar5 & 1)) {
            J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Kanban", kanban_bdl[i], SAFESTRING_VTBL);
            if (modelData == nullptr) /* JUT_ASSERT(1926, modelData != NULL) */
                JUT_ASSERT_fail("d_a_kanban.cpp", 0x786, "modelData != NULL");

            J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
            if (model == nullptr) {
                return FALSE;
            }
            i_this->m544[i] = model;
            J3DModel_setBaseScale(model, &i_this->actor.scale);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

            if (i == 9 || i_this->m290) {
                i_this->actor.cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
            }
        }
    }
    return TRUE;
}

/* 0218CA20 */
static cPhs_State daKanban_Create(fopAc_ac_c* a_this) {
    u32 maxHeapSize = 0;
    kanban_class* i_this = (kanban_class*)a_this;

    /* fopAcM_ct(&i_this->actor, kanban_class): inline member constructors */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = KANBAN_VTBL;
            dBgS_AcchCir_ct(&i_this->m310);
            dBgS_ObjAcch_ct(&i_this->m350, OBJACCH_VT);
            kanban_rippleCtor(&i_this->m514);
            dCcD_Stts_ct(&i_this->m574);
            dCcD_Cyl_ct(&i_this->m5B0, CYL_AAB_VTBL);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, "Kanban");
    if (ret == cPhs_COMPLEATE_e) {
        i_this->m294 = fopAcM_GetParam(a_this) >> 0x14;
        a_this->shape_angle.x = a_this->current.angle.x;
        a_this->shape_angle.y = a_this->current.angle.y;
        a_this->shape_angle.z = a_this->current.angle.z;
        i_this->m2D8.copy(a_this->current.pos);
        i_this->m2F0.x = a_this->current.angle.x;
        i_this->m2F0.y = a_this->current.angle.y;
        i_this->m2F0.z = a_this->current.angle.z;
        i_this->m2F8.copy(a_this->current.pos);
        i_this->m290 = 1;

        if (i_this->m294 == 0) {
            i_this->m290 = 0;
            kanban_setStatusMap(a_this, 0x38);
            i_this->m2A4 = 105.0f;
            i_this->m2A8 = 50.0f;
            i_this->m294 = 1;
            maxHeapSize = 0x3F40;
        } else {
            i_this->m2C4 = a_this->current.angle.z;
            a_this->current.angle.z = 0;
            a_this->shape_angle.z = 0;
            u32 idx = i_this->m294;
            for (s32 i = 0; i < 11; i++) {
                if (idx & 1) {
                    maxHeapSize += kut_size_dt[i];
                }
                idx >>= 1;
            }
        }

        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), maxHeapSize)) {
            return cPhs_ERROR_e;
        }

        a_this->eyePos.set(a_this->current.pos.x, a_this->current.pos.y + 40.0f, a_this->current.pos.z);
        a_this->attention_info.position.x = (f32)a_this->eyePos.x;
        a_this->attention_info.position.y = a_this->eyePos.y + 60.0f;
        a_this->attention_info.position.z = (f32)a_this->eyePos.z;
        a_this->attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 5;
        a_this->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 6;
        a_this->attention_info.flags = fopAc_Attn_TALKFLAG_READ_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e;

        i_this->m350.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->m310, &a_this->speed,
                         &a_this->current.angle, &a_this->shape_angle);
        i_this->m310.SetWall(20.0f, 30.0f);
        i_this->m350.SetWaterCheckOffset(300.0f);
        i_this->m574.Init(0xFF, 0, a_this);
        i_this->m5B0.Set(reinterpret_cast<const dCcD_SrcCyl*>(&cyl_src));
        i_this->m5B0.SetStts(&i_this->m574);

        if (i_this->m290) {
            u32 id = a_this->parentActorID; /* fopAcM_GetLinkId */
            i_this->m2C8 = id;
            if (id == 0xFFFFFFFF) {
                return cPhs_ERROR_e;
            }
            kanban_class* ac = (kanban_class*)fopAcM_SearchByID(id);
            if (ac != nullptr) {
                i_this->m2F8.copy(ac->m2F8);
                i_this->m2F0.x = ac->m2F0.x;
                i_this->m2F0.y = ac->m2F0.y;
                i_this->m2F0.z = ac->m2F0.z;
                a_this->current.pos.copy(i_this->m2F8);
                a_this->current.pos.y = i_this->m2F8.y - 70.0f;
                i_this->m2CC.y = 70.0f;
                i_this->m2E8 = 140.0f;
                i_this->m304.copy(i_this->m2CC);
                i_this->m2BE = KANBAN_PARTS;
                i_this->m2C0 = 0x6e;
            } else {
                return cPhs_ERROR_e;
            }
        } else {
            i_this->m2BE = KANBAN_MOTHER;
            i_this->m2C0 = 10;
        }
    }
    return ret;
}

/* 0218CE48 */
// HD: daKanban_HIO_c::daKanban_HIO_c (allocates when this == NULL)
static daKanban_HIO_c* daKanban_HIO_c_ct(daKanban_HIO_c* h) {
    if (h == nullptr) {
        h = (daKanban_HIO_c*)operator_new(0x10);
        if (h == nullptr)
            return h;
    }
    h->__vtbl = HIO_VTBL;
    h->m04 = 0.0f;
    h->m08 = 0;
    h->m0A = 0;
    h->m0C = 0.8f;
    return h;
}

/* 0218CEAC */
// HD: the zeroed header object follows target_info at 0x10464AB4.
static void __sinit_d_a_kanban_cpp() {
    __sinit_header_statics();
    daKanban_HIO_c_ct(&l_HIO);
}

/* 0218CF4C */
// HD: deleting destructor of a class with a trivial destructor (daKanban_HIO_c)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0218CF60 */
// HD: kanban_class deleting destructor (inline member destructors)
static void kanban_class_dt(kanban_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->m5B0, 2);
        dCcD_Stts_dt(&i_this->m574, 2);
        static_cast<dBgS_GrpPassChk*>(&i_this->m350)->__vtbl = OBJACCH_VT.v20;
        static_cast<dBgS_PolyPassChk*>(&i_this->m350)->__vtbl = OBJACCH_VT.v14;
        kanban_acchDtor(&i_this->m350, 0);
        kanban_circleDtor(&i_this->m310.m_cir, 2);
        kanban_actorDtor(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0218CFFC */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): bindings and sole-owner TU storage
ext 020CB92C BOOL kanban_bombState(void* bomb, s32 state)
ext 020CBAAC BOOL kanban_bombExploding(void* bomb)
ext 025D5A20 fpc_ProcID kanban_createChild(s16 name, u32 parent, u32 parameters, cXyz* position, s32 room, csXyz* angle, cXyz* scale, s8 subtype, u32 callback)
ext 025A9084 void kanban_rippleCtor(void* ripple)
ext 024EFD9C void kanban_acchDtor(void* acch, s32 flags)
ext 02018034 void kanban_circleDtor(void* circle, s32 flags)
ext 025D50BC void kanban_actorDtor(void* actor, s32 flags)
data l_HIO 10464A7C nocheck
data target_info_count 10464A6C nocheck
data target_info 10464A8C nocheck
data l_msgId 101B7CB8
data cyl_src 101B7E54
data cut_parts_arg_data 101B7D5C
data kanban_bdl 101B7DFC
data kut_size_dt 101B7E28
data pl_cut_real_no_dt 101B7CBC
data pl_cut_no_dt 101B7CFC
str 100125EC "Kanban" in 0218C824
str 100125F4 "Kanban" in 0218C874
str 100125FC "d_a_kanban.cpp" in 0218C874
str 1001260C "modelData != NULL" in 0218C874
str 10012628 "Kanban" in 0218CA20
*/
