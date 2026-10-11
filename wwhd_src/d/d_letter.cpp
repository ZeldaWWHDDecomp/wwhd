/**
 * d_letter.cpp - letter delivery and reading states
 * WWHD, written in the style of zeldaret/tww and verified against the original
 * (tools/verify, unit d_letter).
 */
#include "d/d_letter.h"
#include "gc_api/d/d_com_inf_game.h"

enum dLetterStts_e {
    NOSEND_e,
    SEND_e,
    STOCK_e,
    READ_e,
};

// HD: keep full-width indices/results at these call sites rather than the GameCube u16/u8 API.
/* 02586D24 */
s32 dLetter_isNoSend(u32 idx) {
    return dComIfGs_getEventRegHD(idx) == NOSEND_e;
}

/* 02586D5C */
void dLetter_send(u32 idx) {
    dComIfGs_setEventRegHD(idx, SEND_e);
}

/* 02586D74 */
s32 dLetter_isSend(u32 idx) {
    return dComIfGs_getEventRegHD(idx) == SEND_e;
}

/* 02586DB0 */
void dLetter_stock(u32 idx) {
    dComIfGs_setEventRegHD(idx, STOCK_e);
}

/* 02586DC8 */
s32 dLetter_isStock(u32 idx) {
    return dComIfGs_getEventRegHD(idx) == STOCK_e;
}

/* 02586E04 */
void dLetter_read(u32 idx) {
    dComIfGs_setEventRegHD(idx, READ_e);
}

/* 02586E1C */
s32 dLetter_isRead(u32 idx) {
    return dComIfGs_getEventRegHD(idx) == READ_e;
}

/* 02586E58 */
void dLetter_delivery(u32 idx) {
    if (dLetter_isSend(idx)) {
        dLetter_stock(idx);
    }
}

/* 02586E94 */
void dLetter_autoStock(u32 idx) {
    if (dLetter_isNoSend(idx)) {
        dLetter_stock(idx);
    }
}

/* 02586ED0 */
u32 dLetter_isDelivery(u32 idx) {
    // HD: invert only the low byte of the original return register.
    return (dLetter_isNoSend(idx) ^ 1u) & 0xFF;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02586EF8 */
static void __sinit_d_letter_cpp() {
    sinit_header_statics_rodata(0x10477610, 0x101E9B90, 0x1047761C, 0x100505E4);
}

/* WWHD glue (tools/verify/autoglue.py): compiler scaffolding only */
