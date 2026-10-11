/**
 * d_a_alldie.cpp (WWHD)
 * Sets a switch once every enemy of the room is dead.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_alldie.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_alldie.h"
#include "gc_api/f_op/f_op_actor_mng.h"

/* 02048460 */
u8 daAlldie_c::getSwbit() {
    return fopAcM_GetParam(this) >> 0x8;
}

/* 02048410 */
BOOL daAlldie_c::actionCheck() {
    if (!fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this))) {
        setActio(ACT_TIMER);
        mTimer = 65;
    }
    return TRUE;
}

/* 0204846C */
BOOL daAlldie_c::actionTimer() {
    if (fopAcM_myRoomSearchEnemy(fopAcM_GetRoomNo(this))) {
        setActio(ACT_CHECK);
    } else if (mTimer > 0) {
        mTimer = mTimer - 1;
    } else {
        setActio(ACT_WAIT);
        dComIfGs_onSwitch(getSwbit(), fopAcM_GetRoomNo(this));
    }
    return TRUE;
}

/* 02048500 */
BOOL daAlldie_c::execute() {
    switch (mAction) {
    case ACT_CHECK:
        actionCheck();
        break;
    case ACT_TIMER:
        actionTimer();
        break;
    default:
        actionWait();
        break;
    }
    return TRUE;
}

/* 020486B8 */
static BOOL daAlldie_Draw(daAlldie_c*) {
    return TRUE;
}

/* 0204854C */
static BOOL daAlldie_Execute(daAlldie_c* i_this) {
    i_this->execute();
    return TRUE;
}

/* 02048570 */
static BOOL daAlldie_IsDelete(daAlldie_c*) {
    return TRUE;
}

/* 02048578 */
static BOOL daAlldie_Delete(daAlldie_c*) {
    return TRUE;
}

/* 02048580 */
static cPhs_State daAlldie_Create(fopAc_ac_c* ac) {
    daAlldie_c* i_this = (daAlldie_c*)ac;
    /* fopAcM_ct(this, daAlldie_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = daAlldie_c::__vt;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    if (!dComIfGs_isSwitch(i_this->getSwbit(), fopAcM_GetRoomNo(i_this))) {
        i_this->setActio(ACT_CHECK);
    } else {
        i_this->setActio(ACT_WAIT);
    }

    i_this->shape_angle.z = 0;
    i_this->shape_angle.x = 0;
    i_this->current.angle.z = 0;
    i_this->current.angle.x = 0;

    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02048624 */
static void __sinit_d_a_alldie_cpp() {
    sinit_header_statics(0x104613A4, 0x1018F840);
}

/* 020486C0 */
static void daAlldie_c_dt(daAlldie_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
ext 025D98E8 u32 fopAcM_myRoomSearchEnemy(s8 roomNo)
*/
