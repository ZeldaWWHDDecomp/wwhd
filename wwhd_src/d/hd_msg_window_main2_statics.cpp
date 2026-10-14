/**
 * hd_msg_window_main2_statics.cpp
 * HD - two translation units of the second main message window that hold only the statics of the shared
 * headers (their static initialisers) and a state-number template copy
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified against the recompiled original
 * (tools/verify, unit hd_msg_msg_window_main2). All names are ours.
 */

#include "d/hd_parts.h"
static u32 sMain2Tu1HeaderStatics;
static u32 sMain2Tu1HeaderDescriptors;
static u32 sMain2Tu1HeaderZero;
static u32 sMain2Tu1PiConstants;
static u32 sMain2Tu7HeaderStatics;
static u32 sMain2Tu7HeaderDescriptors;
static u32 sMain2Tu7HeaderZero;
static u32 sMain2Tu7PiConstants;

/* 026B5AA0 */
static s32 hdMsgWindowMain2_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* 026B5BA0 */
static void __sinit_hd_msg_window_main2_tu1_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sMain2Tu1HeaderStatics), gabi::ea(&sMain2Tu1HeaderDescriptors), gabi::ea(&sMain2Tu1HeaderZero),
                                gabi::ea(&sMain2Tu1PiConstants));
}

/* 026B76E8 */
static void __sinit_hd_msg_window_main2_tu7_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sMain2Tu7HeaderStatics), gabi::ea(&sMain2Tu7HeaderDescriptors), gabi::ea(&sMain2Tu7HeaderZero),
                                gabi::ea(&sMain2Tu7PiConstants));
}

/* WWHD glue
 data sMain2Tu1HeaderStatics 10498698 nocheck
 data sMain2Tu1HeaderDescriptors 101F6618 nocheck
 data sMain2Tu1HeaderZero 104986A4 nocheck
 data sMain2Tu1PiConstants 100FE648 nocheck
 data sMain2Tu7HeaderStatics 104987C0 nocheck
 data sMain2Tu7HeaderDescriptors 101F6690 nocheck
 data sMain2Tu7HeaderZero 104987CC nocheck
 data sMain2Tu7PiConstants 100FED18 nocheck
*/
