/* WWHD Timer. Derived from zeldaret/tww and audited against HD code.
 * Reconstruction; */
#include "d/actor/d_a_obj_timer.h"
namespace daObjTimer {
/* 023A257C */
u32 prmAbstract(Act_c* actor, u32 width, u32 shift) {
    u32 parameters = fopAcM_GetParam(actor);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? parameters >> shift : 0u) & mask;
}

/* 023A2154 */
void Act_c::mode_count_init() {
    u32 time = prm_get_time() * 15u;
    mMode = Mode_COUNT_e;
    mTimer = (s32)time;
}

/* 023A2198 */
void Act_c::mode_wait_init() {
    mTimer = 0;
    mMode = Mode_WAIT_e;
}

/* 023A21A8 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);
    fopAcM_offDraw(this);
    // HD: switch checks use the home room, rather than the current room.
    u32 sw = prm_get_swSave();
    s32 room = home.roomNo;
    if (dComIfGs_isSwitch(sw, room))
        mode_count_init();
    else
        mode_wait_init();
    return cPhs_COMPLEATE_e;
}

/* 023A2258 */
bool Act_c::_execute() {
    // HD: GHS stores each pointer to member in eight bytes.
    static ModeEntry mode_proc[] = {
        {&Act_c::mode_wait},
        {&Act_c::mode_count},
    };
    (this->*mode_proc[mMode].proc)();
    return true;
}

/* 023A22CC */
void Act_c::mode_wait() {
    u32 sw = prm_get_swSave();
    s32 room = home.roomNo;
    if (dComIfGs_isSwitch(sw, room))
        mode_count_init();
}

/* 023A232C */
void Act_c::mode_count() {
    if (mIsStop)
        return;
    // HD: preserve the timer decrement wrapping at INT_MIN.
    s32 timer = (s32)((u32)mTimer - 1u);
    mTimer = timer;
    if (timer % 30 == 0) {
        s32 seconds = timer / 30;
        if (seconds <= 20) {
            u32 sound;
            if (seconds > 10)
                sound = JA_SE_SYS_EV_TIMER_20;
            else if (seconds > 5)
                sound = JA_SE_SYS_EV_TIMER_10;
            else if (seconds > 0)
                sound = JA_SE_SYS_EV_TIMER_5;
            else
                sound = JA_SE_SYS_EV_TIMER_0;
            mDoAud_seStart(sound);
            timer = mTimer;
        }
    }
    if (timer > 0) {
        u32 sw = prm_get_swSave();
        s32 room = home.roomNo;
        if (dComIfGs_isSwitch(sw, room))
            return;
    }
    u32 sw = prm_get_swSave();
    s32 room = home.roomNo;
    dComIfGs_offSwitch(sw, room);
    mode_wait_init();
}

/* 023A2474 */
s32 Mthd_Create(Act_c* actor) {
    return actor->_create();
}

/* 023A2478 */
BOOL Mthd_Delete(Act_c* actor) {
    return TRUE;
}

/* 023A2480 */
BOOL Mthd_Execute(Act_c* actor) {
    return actor->_execute();
}

/* 023A2484 */
// HD: translation-unit static initialisation.
void __sinit_d_a_obj_timer_cpp() {
    sinit_header_statics(0x1046C4EC, 0x101CD2B0);
}

/* 023A2518 */
// HD: actor deleting destructor.
void destruct(Act_c* actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}

/* 023A256C */
BOOL Mthd_Draw(Act_c* actor) {
    return TRUE;
}

/* 023A2574 */
BOOL Mthd_IsDelete(Act_c* actor) {
    return TRUE;
}

s32 Act_c::prm_get_time() const {
    return prmAbstract(const_cast<Act_c*>(this), PRM_TIME_W, PRM_TIME_S);
}

s32 Act_c::prm_get_swSave() const {
    return prmAbstract(const_cast<Act_c*>(this), PRM_SWSAVE_W, PRM_SWSAVE_S);
}

Act_c::Act_c() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
} // namespace daObjTimer

// HD: expose the unit namespace to generated verification registrations.
using namespace daObjTimer;

/* WWHD glue (tools/verify/autoglue.py): private bindings
data mode_proc 10031038
ext 025E1988 void mDoAud_seStart(u32)
*/
