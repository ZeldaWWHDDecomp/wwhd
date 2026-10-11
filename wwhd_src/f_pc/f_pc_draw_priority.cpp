/** Draw priority. */
#include "f_pc/f_pc_draw_priority.h"

static DrawPriorityHeaderStatics l_headerStatics;
static const DrawPriorityPiPair l_piPair;

/* 025DE460 */
s16 fpcDwPi_Get(draw_priority_class* i_drawPrioClass) {
    return i_drawPrioClass->mPriority;
}

/* 025DE468 */
void fpcDwPi_Set(draw_priority_class* i_drawPrioClass, u32 i_priority) {
    // HD: keep the full argument register until the halfword store.
    i_drawPrioClass->mPriority = static_cast<u16>(i_priority);
}

/* 025DE470 */
void fpcDwPi_Init(draw_priority_class* i_drawPrioClass, u32 i_priority) {
    fpcDwPi_Set(i_drawPrioClass, i_priority);
}

// HD: compiler-generated and template code of this translation unit.
/* 025DE474 */
static void __sinit_f_pc_draw_priority_cpp() {
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3A70);
    f32 negativePi = l_piPair.negativePi;
    f32 positivePi = l_piPair.positivePi;
    l_headerStatics.negativePi = negativePi;
    l_headerStatics.positivePi = positivePi;
    DrawPriorityFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3A7C);
    DrawPrioritySecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3A88);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A79C
 data l_piPair 100582C8
 ext 028ED6F8 void DrawPriorityFirstHeaderObject_ct(u8*)
 ext 028EAB2C void DrawPrioritySecondHeaderObject_ct(u8*)
*/
