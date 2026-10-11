/**
 * d_a_disappear.cpp (WWHD)
 * Enemy death puff: plays the death effects and drops the item after a delay.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_disappear.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_disappear.h"

// HD: this translation unit's common-header statics.
const gc_tu_data gc_tu = {
    .headerStatics = {0x10463B78, 0x101B44DC, 0},
};

/* 02124514 */
static BOOL daDisappear_Draw(disappear_class* i_this) {
    return TRUE;
}

/* 0212451C */
static BOOL daDisappear_Execute(disappear_class* i_this) {
    if (i_this->mTimer != 0) {
        i_this->mTimer--;

        if (i_this->mTimer == 0) {
            s8 dropType = i_this->health;

            if (dropType != daDisItem_NONE1_e && dropType != daDisItem_NONE3_e) {
                if (dropType == daDisItem_HEART_CONTAINER_e) {
                    fopAcM_createItemForBoss(&i_this->current.pos, 0, fopAcM_GetRoomNo(i_this), &i_this->current.angle, nullptr, 0);
                } else if (dropType >= daDisItem_HEART_e && dropType <= daDisItem_NONE13_e) {
                    // dItemNo_HEART_e, dItemNo_LARGE_MAGIC_e, dItemNo_ARROW_10_e.
                    // HD: numeric initializers let autoglue check the original table bytes.
                    static u32 ki_item_d[] = {
                        0x00,
                        0x0A,
                        0x10,
                    };
                    if (dropType < daDisItem_HEART_e + ARRAY_SIZE(ki_item_d)) {
                        fopAcM_createItem(&i_this->current.pos, ki_item_d[dropType - daDisItem_HEART_e], -1, -1, 0, nullptr, 4,
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

/* 0212461C */
static BOOL daDisappear_IsDelete(disappear_class* i_this) {
    return TRUE;
}

/* 02124624 */
static BOOL daDisappear_Delete(disappear_class* i_this) {
    return TRUE;
}

/* inlined into daDisappear_Create */
static void set_disappear(disappear_class* i_this, f32 scale) {
    fopAcM_seStart(i_this, JA_SE_CM_MONS_EXPLODE, 0);

    cXyz particleScale;
    s16 t = REG_S(8, 0);
    particleScale.x = scale;
    particleScale.y = scale;
    particleScale.z = scale;
    i_this->mTimer = 58 + t;

    switch (i_this->health) {
    case daDisItem_IBALL_e:
    case daDisItem_HEART_CONTAINER_e:
    case daDisItem_HEART_e:
    case daDisItem_MAGIC_e:
    case daDisItem_ARROW_e:
    case daDisItem_NONE13_e:
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUPOFU, &i_this->current.pos, nullptr, &particleScale);
        // Fall-through
    case daDisItem_NONE3_e:
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUBAKUEN, &i_this->current.pos, nullptr, &particleScale);
        dComIfGp_particle_setStripes(dPa_name::ID_AK_JN_SIBOUSPIRIT, &i_this->current.pos, nullptr, &particleScale, 0xFF, 0x96);
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUFLASH, &i_this->current.pos, nullptr, &particleScale);
        break;
    case daDisItem_NONE1_e:
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUBAKUEN, &i_this->current.pos, nullptr, &particleScale);
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUFLASH, &i_this->current.pos, nullptr, &particleScale);
        break;
    case daDisItem_UNK4_e:
        dComIfGp_particle_set(dPa_name::ID_IT_JN_PUCHI_SHIBOUA, &i_this->current.pos);
        dComIfGp_particle_set(dPa_name::ID_IT_JN_PUCHI_SHIBOUB, &i_this->current.pos);
        dComIfGp_particle_set(dPa_name::ID_IT_JN_PUCHI_SHIBOUC, &i_this->current.pos);
        break;
    }
}

/* inlined into daDisappear_Create (fopAcM_ct) */
disappear_class::disappear_class() {
    // HD: GHS explicitly constructs the actor base and stores the derived vtable.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 0212462C */
static cPhs_State daDisappear_Create(fopAc_ac_c* i_ac) {
    disappear_class* dis = static_cast<disappear_class*>(i_ac);

    fopAcM_ct(dis, disappear_class);

    // HD: the three parameters are extracted from one read of mParameters.
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

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02124A84 */
// HD: static initialisation of the common-header statics.
static void __sinit_d_a_disappear_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 02124B18 */
// HD: compiler-generated deleting destructor.
static void disappear_class_dt(disappear_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data ki_item_d 101B44B0
*/
