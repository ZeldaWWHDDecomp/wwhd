/* Per-TU vtable helpers omitted by the matcher range. */
#include "d/actor/d_a_obj_ospbox.h"
namespace daObjOspbox {
static BOOL movebg_is_delete(Act_c* actor) {
 WWHD_FUNC(0x02380C5C,BOOL,actor);
 return TRUE;
}
VERIFY(0x02380C5C,movebg_is_delete);
static void actor_destructor(Act_c* actor,s32 flags) {
 WWHD_FUNC(0x02380C70,void,actor,flags);
 if(!actor) return;
 u32 ea=gabi::ea(actor);
 gabi::store<u32>(ea+0x578,0x1002E130);
 gabi::store<u32>(ea+0x598,0x1002E150);
 gabi::store<u32>(ea+0x5A4,0x1002E110);
 gabi::call(0x02008DAC,gabi::at<u8>(ea+0x558),0);
 gabi::call(0x02515A70,gabi::at<u8>(ea+0x428),2);
 gabi::call(0x02515860,gabi::at<u8>(ea+0x3EC),2);
 gabi::call(0x025D50BC,actor,0);
 if(flags&1) gabi::call(0x0273AF40,actor);
}
VERIFY(0x02380C70,actor_destructor);
}

/* ---- leftover functions of the translation unit ---- */

/* 02380C64 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002E0FC, after the destructor 02380C48 */
static void ospbox_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02380C64, void, p);
}
VERIFY(0x02380C64, ospbox_SafeString_assureTermination);
