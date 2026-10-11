/**
 * d_a_boss_item.cpp (WWHD)
 * Boss item: drops the heart container of a defeated boss that was not collected yet.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_boss_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_boss_item.h"

/* inlined into daBossItem_Create (fopAcM_ct) */
bossitem_class::bossitem_class() {
    // HD: GHS constructs the base and stores the virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 020D0A60 */
static BOOL daBossItem_IsDelete(bossitem_class* i_this) {
    return TRUE;
}

/* 020D0A68 */
// HD: the debug delete registration is absent.
static BOOL daBossItem_Delete(bossitem_class* i_this) {
    return TRUE;
}

/* 020D0A70 */
static cPhs_State daBossItem_Create(fopAc_ac_c* i_this) {
    bossitem_class* a_this = (bossitem_class*)i_this;
    // HD: no debug create registration.
    fopAcM_ct(a_this, bossitem_class);

    int stageNo = daBossItem_prm::getStage(a_this);
    BOOL isStageBossDead = dComIfGs_isStageBossEnemy(stageNo);

    if (isStageBossDead && !dComIfGs_isStageLife(stageNo)) {
        fopAcM_createItemForBoss(&i_this->current.pos, 1, fopAcM_GetRoomNo(i_this), &i_this->current.angle, nullptr, 1);
    }

    return cPhs_ERROR_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 020D0B1C */
// HD: initialisation of the common header statics.
static void __sinit_d_a_boss_item_cpp() {
    sinit_header_statics(0x10462740, 0x101924E0);
}

/* 020D0BB0 */
// HD: compiler-generated deleting destructor.
static void bossitem_class_dt(bossitem_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
