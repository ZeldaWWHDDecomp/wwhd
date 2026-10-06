/**
 * d_a_boss_item.cpp (WWHD)
 * Boss item: drops the heart container of a defeated boss that was not collected yet.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_boss_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define BOSSITEM_VTBL 0x1000B04C /* bossitem_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
static inline BOOL dComIfGs_isStageLife(s32 no) { return gabi::call<BOOL>(0x02520B88, no); }
/* 025D8A5C fopAcM_createItemForBoss(pos, param, roomNo, angle, scale, flag) (HD: param r4 not read) */
static inline void fopAcM_createItemForBoss(cXyz* pos, s32 param, s32 roomNo, csXyz* angle, cXyz* scale, s32 flag) {
    gabi::call(0x025D8A5C, pos, param, roomNo, angle, scale, flag);
}

struct bossitem_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
};
WWHD_SIZE(bossitem_class, 0x3B4);

static inline int daBossItem_prm_getStage(bossitem_class* a) { return fopAcM_GetParam(a) & 0xFF; }

/* 020D0A60 */
static BOOL daBossItem_IsDelete(bossitem_class* i_this) {
    WWHD_FUNC(0x020D0A60, BOOL, i_this);
    return TRUE;
}
VERIFY(0x020D0A60, daBossItem_IsDelete);

/* 020D0A68 (fopAcM_RegisterDeleteID: debug only, empty) */
static BOOL daBossItem_Delete(bossitem_class* i_this) {
    WWHD_FUNC(0x020D0A68, BOOL, i_this);
    return TRUE;
}
VERIFY(0x020D0A68, daBossItem_Delete);

/* 020D0A70 */
static cPhs_State daBossItem_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x020D0A70, cPhs_State, i_this);
    bossitem_class* a_this = (bossitem_class*)i_this;
    /* fopAcM_ct(i_this, bossitem_class): HD vtable */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = BOSSITEM_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    int stageNo = daBossItem_prm_getStage(a_this);
    BOOL isStageBossDead = dComIfGs_isStageBossEnemy(stageNo);

    if (isStageBossDead && !dComIfGs_isStageLife(stageNo)) {
        fopAcM_createItemForBoss(&i_this->current.pos, 1, fopAcM_GetRoomNo(i_this), &i_this->current.angle, nullptr, 1);
    }

    return cPhs_ERROR_e;
}
VERIFY(0x020D0A70, daBossItem_Create);

/* 020D0B1C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_boss_item_cpp() {
    WWHD_FUNC(0x020D0B1C, void, (u32)0);
    sinit_header_statics(0x10462740, 0x101924E0);
}
VERIFY(0x020D0B1C, __sinit_d_a_boss_item_cpp);

/* 020D0BB0: bossitem_class deleting destructor (compiler-generated, HD virtual destructor) */
static void bossitem_class_dt(bossitem_class* i_this, s32 flags) {
    WWHD_FUNC(0x020D0BB0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020D0BB0, bossitem_class_dt);
