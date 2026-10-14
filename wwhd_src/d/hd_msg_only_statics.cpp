/**
 * hd_msg_only_statics.cpp
 * HD - the message-only window's (MsgOnly_00) translation unit statics: the always-enabled state check, the
 * state-number template copy and the shared headers' static initialiser
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified against the recompiled original
 * (tools/verify, unit hd_msg_msg_window_main).
 * All names are ours.
 */

#include "d/hd_parts.h"
static u32 sOnlyStaticsHeaderStatics;
static u32 sOnlyStaticsHeaderDescriptors;
static u32 sOnlyStaticsHeaderZero;
static u32 sOnlyStaticsPiConstants;

/* 026AE9A4 */
static u32 hdMsgOnly_isStateEnabled() {
    return TRUE;
}

/* 026AE9AC */
static s32 hdMsgOnly_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* 026AEAAC */
static void __sinit_hd_msg_only_statics_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sOnlyStaticsHeaderStatics), gabi::ea(&sOnlyStaticsHeaderDescriptors), gabi::ea(&sOnlyStaticsHeaderZero),
                                gabi::ea(&sOnlyStaticsPiConstants));
}

/* WWHD glue
data sOnlyStaticsHeaderStatics 10497E68 nocheck
data sOnlyStaticsHeaderDescriptors 101F6534 nocheck
data sOnlyStaticsHeaderZero 10497E74 nocheck
data sOnlyStaticsPiConstants 100FCF08 nocheck
*/
