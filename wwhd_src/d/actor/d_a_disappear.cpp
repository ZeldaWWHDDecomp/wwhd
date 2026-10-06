/**
 * d_a_disappear.cpp (WWHD)
 * Enemy death puff: plays the death effects and drops the item after a delay.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_disappear.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define DISAPPEAR_VTBL 0x1000DCAC /* disappear_class vtable (HD virtual destructor) */

enum {
    daDisItem_NONE1_e = 1,
    daDisItem_HEART_CONTAINER_e = 2,
    daDisItem_NONE3_e = 3,
    daDisItem_HEART_e = 10,
    daDisItem_NONE13_e = 13,
};
enum { JA_SE_CM_MONS_EXPLODE = 0x5801 };
enum {
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUPOFU = 0x14,
    ID_AK_JN_SIBOUSPIRIT = 0x15,
    ID_AK_JN_SIBOUFLASH = 0x16,
    ID_IT_JN_PUCHI_SHIBOUA = 0x43C,
    ID_IT_JN_PUCHI_SHIBOUB = 0x43D,
    ID_IT_JN_PUCHI_SHIBOUC = 0x43E,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D8A5C fopAcM_createItemForBoss(pos, param, roomNo, angle, scale, flag) */
static inline void fopAcM_createItemForBoss(cXyz* pos, s32 param, s32 roomNo, csXyz* angle, cXyz* scale, s32 flag) {
    gabi::call(0x025D8A5C, pos, param, roomNo, angle, scale, flag);
}
/* 025D8870 fopAcM_createItem(pos, itemNo, itemBitNo, roomNo, type, angle, action, scale) */
static inline fpc_ProcID fopAcM_createItem(cXyz* pos, s32 itemNo, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action,
                                           cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D8870, pos, itemNo, bitNo, roomNo, type, angle, action, scale);
}
/* 025D9154 fopAcM_createIball(pos, itemTableIdx, roomNo, angle, itemBitNo) */
static inline fpc_ProcID fopAcM_createIball(cXyz* pos, s32 tbl, s32 roomNo, csXyz* angle, s32 bitNo) {
    return gabi::call<fpc_ProcID>(0x025D9154, pos, tbl, roomNo, angle, bitNo);
}
/* 025A86F4 dPa_control_c::setNormalStripes(u16 id, pos, angle, scale, u8 alpha, u16 rate) */
static inline void dComIfGp_particle_setStripes(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha, u16 rate) {
    dPa_control_c* pa = dComIfGp_getParticle();
    gabi::call(0x025A86F4, pa, id, pos, angle, scale, alpha, rate);
}
static inline s16 REG8_S(int i) { return REG_S(8, i); }

struct disappear_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<s32> mItemBitNo;
    /* 0x3B8 */ be<s16> mTimer;
    /* 0x3BA */ u8 _3BA[2];
};
WWHD_SIZE(disappear_class, 0x3BC);

/* static u32 ki_item_d[] = {dItemNo_HEART_e, dItemNo_LARGE_MAGIC_e, dItemNo_ARROW_10_e}; (.data) */
static inline u32 ki_item_d(s32 i) { return gabi::load<u32>(0x101B44B0 + 4 * i); }

/* 02124514 */
static BOOL daDisappear_Draw(disappear_class*) {
    WWHD_FUNC(0x02124514, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02124514, daDisappear_Draw);

/* 0212451C */
static BOOL daDisappear_Execute(disappear_class* i_this) {
    WWHD_FUNC(0x0212451C, BOOL, i_this);
    if (i_this->mTimer != 0) {
        i_this->mTimer = i_this->mTimer - 1;

        if (i_this->mTimer == 0) {
            s8 dropType = i_this->health;

            if (dropType != daDisItem_NONE1_e && dropType != daDisItem_NONE3_e) {
                if (dropType == daDisItem_HEART_CONTAINER_e) {
                    fopAcM_createItemForBoss(&i_this->current.pos, 0, fopAcM_GetRoomNo(i_this), &i_this->current.angle, nullptr, 0);
                } else if (dropType >= daDisItem_HEART_e && dropType <= daDisItem_NONE13_e) {
                    if (dropType < daDisItem_HEART_e + 3) {
                        fopAcM_createItem(&i_this->current.pos, ki_item_d(dropType - daDisItem_HEART_e), -1, -1, 0, nullptr, 4,
                                          nullptr);
                    }
                } else {
                    fopAcM_createIball(&i_this->current.pos, i_this->itemTableIdx, fopAcM_GetRoomNo(i_this), &i_this->current.angle,
                                       i_this->mItemBitNo);
                }
            }
        }
    } else {
        fopAcM_delete(i_this);
    }

    return TRUE;
}
VERIFY(0x0212451C, daDisappear_Execute);

/* 0212461C */
static BOOL daDisappear_IsDelete(disappear_class*) {
    WWHD_FUNC(0x0212461C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0212461C, daDisappear_IsDelete);

/* 02124624 */
static BOOL daDisappear_Delete(disappear_class*) {
    WWHD_FUNC(0x02124624, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02124624, daDisappear_Delete);

/* set_disappear (inlined into Create) */
static inline void set_disappear(disappear_class* i_this, f32 scale) {
    fopAcM_seStart(i_this, JA_SE_CM_MONS_EXPLODE, 0);

    gabi::Local<cXyz> particleScale;
    s16 t = REG8_S(0);
    particleScale->x = scale;
    particleScale->y = scale;
    particleScale->z = scale;
    i_this->mTimer = 58 + t;

    switch (i_this->health) {
    case 0x0:
    case 0x2:
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
        dComIfGp_particle_set(ID_AK_JN_SIBOUPOFU, &i_this->current.pos, nullptr, particleScale);
        // Fall-through
    case 0x3:
        dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, &i_this->current.pos, nullptr, particleScale);
        dComIfGp_particle_setStripes(ID_AK_JN_SIBOUSPIRIT, &i_this->current.pos, nullptr, particleScale, 0xFF, 0x96);
        dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, &i_this->current.pos, nullptr, particleScale);
        break;
    case 0x1:
        dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, &i_this->current.pos, nullptr, particleScale);
        dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, &i_this->current.pos, nullptr, particleScale);
        break;
    case 0x4:
        dComIfGp_particle_set(ID_IT_JN_PUCHI_SHIBOUA, &i_this->current.pos);
        dComIfGp_particle_set(ID_IT_JN_PUCHI_SHIBOUB, &i_this->current.pos);
        dComIfGp_particle_set(ID_IT_JN_PUCHI_SHIBOUC, &i_this->current.pos);
        break;
    }
}

/* 0212462C */
static cPhs_State daDisappear_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x0212462C, cPhs_State, i_ac);
    disappear_class* dis = static_cast<disappear_class*>(i_ac);

    /* fopAcM_ct(dis, disappear_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = DISAPPEAR_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }

    u32 prm = fopAcM_GetParam(dis);
    dis->health = prm & 0xFF; /* drop type */
    f32 scaleMag = (f32)((prm >> 8) & 0xFF) * 0.1f;

    s32 bitNo = (prm >> 0x10) & 0xFF;
    if (bitNo == 0xFF) {
        bitNo = -1;
    }
    dis->mItemBitNo = bitNo;

    set_disappear(dis, scaleMag);

    return cPhs_COMPLEATE_e;
}
VERIFY(0x0212462C, daDisappear_Create);

/* 02124A84: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_disappear_cpp() {
    WWHD_FUNC(0x02124A84, void, (u32)0);
    sinit_header_statics(0x10463B78, 0x101B44DC);
}
VERIFY(0x02124A84, __sinit_d_a_disappear_cpp);

/* 02124B18: disappear_class deleting destructor (compiler-generated, HD virtual destructor) */
static void disappear_class_dt(disappear_class* i_this, s32 flags) {
    WWHD_FUNC(0x02124B18, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02124B18, disappear_class_dt);
