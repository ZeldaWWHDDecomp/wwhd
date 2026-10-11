/** Process creation entry points. */
#include "f_pc/f_pc_creator.h"

static CreatorHeaderStatics l_headerStatics;
static const CreatorPiPair l_piPair;

/* 025DDCE4 */
BOOL fpcCt_Abort(base_process_class* i_proc) {
    // The shared process layout retains this guest pointer as a u32 field.
    return fpcCtRq_Cancel(gabi::at<create_request>(i_proc->mpCtRq));
}

/* 025DDCEC */
BOOL fpcCt_Handler() {
    return fpcCtRq_Handler();
}

// HD: compiler-generated and template code of this translation unit.
/* 025DDCF0 */
static void __sinit_f_pc_creator_cpp() {
    // Preserve the constructor's nonsequential zeroing and pi load/store order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F39B0);
    f32 first = l_piPair.negativePi;
    f32 second = l_piPair.positivePi;
    l_headerStatics.negativePi = first;
    l_headerStatics.positivePi = second;
    CreatorFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F39BC);
    CreatorSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F39C8);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A710
 data l_piPair 10057EAC
 ext 025DD934 BOOL fpcCtRq_Cancel(create_request*)
 ext 025DDAC4 BOOL fpcCtRq_Handler()
 ext 028ED6F8 void CreatorFirstHeaderObject_ct(u8*)
 ext 028EAB2C void CreatorSecondHeaderObject_ct(u8*)
*/
