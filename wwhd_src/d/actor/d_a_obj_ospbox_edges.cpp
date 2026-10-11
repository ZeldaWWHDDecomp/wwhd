/* Per-TU vtable helpers omitted by the matcher range. */
#include "d/actor/d_a_obj_ospbox.h"
using daObjOspbox::Act_c;
/* 02380C5C */
static BOOL movebg_is_delete(Act_c *actor) {
    return TRUE;
}
/* 02380C70 */
static void actor_destructor(Act_c *actor, s32 flags) {
    if (!actor)
        return;
    gabi_destroy(static_cast<dBgS_GndChk &>(actor->mObjGndChk));
    dCcD_Cyl_dt(&actor->mCyl, 2);
    dCcD_Stts_dt(&actor->mStts, 2);
    __dt(static_cast<fopAc_ac_c *>(actor));
    if (flags & 1)
        operator_delete(actor);
}

/* ---- leftover functions of the translation unit ---- */

/* 02380C64 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002E0FC, after
 * the destructor 02380C48 */
/* 02380C64 */
static void ospbox_SafeString_assureTermination(void *p) {
}

/* WWHD glue */
