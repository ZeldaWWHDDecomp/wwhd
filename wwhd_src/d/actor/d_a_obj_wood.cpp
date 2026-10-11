/**
 * d_a_obj_wood.cpp (WWHD)
 * Object - Wood (registers a tree unit with the dWood packet and deletes itself).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_wood.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_wood.h"

using daObjWood::Act_c;

/* inlined into _create (fopAcM_ct) */
daObjWood::Act_c::Act_c() {
    // HD: GHS constructs the base and collision members explicitly.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, AAB_VT);
}

/* inlined into Create */
cPhs_State daObjWood::Act_c::_create() {
    fopAcM_ct(this, Act_c);
    if (dComIfGp_createWood() != NULL) {
        dComIfGp_getWood()->put_unit(current.pos, fopAcM_GetRoomNo(this));
    }
    return cPhs_ERROR_e;
}

/* inlined into Execute */
bool daObjWood::Act_c::_execute() {
    mCyl.SetC(&current.pos);
    return true;
}

/* 023BBCE4 */
static cPhs_State daObjWood_Method_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}

/* 023BBDE4 */
static BOOL daObjWood_Method_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}

/* 023BBDEC */
static BOOL daObjWood_Method_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}

/* 023BBE18 */
static BOOL daObjWood_Method_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}

/* 023BBF20 */
static BOOL daObjWood_Method_IsDelete(void*) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 023BBE20 */
static void __sinit_d_a_obj_wood_cpp() {
    sinit_header_statics(0x1046CA18, 0x101CDE3C);
}

/* 023BBEB4 */
// HD: deleting destructor; collision members then the actor base.
static void daObjWood_Act_c_dt(daObjWood::Act_c* i_this, s32 flags) {
    if (i_this != NULL) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
