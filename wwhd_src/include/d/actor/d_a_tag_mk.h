#pragma once
#include "bindings.h"

// HD profile 101D1EF4 allocates 0x3D8 bytes. Actor-owned fields retain the
// GameCube ordering, displaced by the larger HD actor base (+0x11C).
struct daTag_Mk_c : fopAc_ac_c {
    be<u32> message;
    be<s8> talkState;
    u8 pad3B1;
    be<s16> musicState;
    u8 pad3B4[4];
    cXyz direction;
    be<u8> action;
    u8 pad3C5;
    be<s16> eventIndex;
    be<s32> staffIndex;
    be<s16> cutEndTimer;
    u8 pad3CE[2];
    be<u32> itemId;
    gptr<const char> staffName;
};
static_assert(sizeof(daTag_Mk_c) == 0x3D8);
static_assert(offsetof(daTag_Mk_c, message) == 0x3AC);
static_assert(offsetof(daTag_Mk_c, talkState) == 0x3B0);
static_assert(offsetof(daTag_Mk_c, musicState) == 0x3B2);
static_assert(offsetof(daTag_Mk_c, direction) == 0x3B8);
static_assert(offsetof(daTag_Mk_c, action) == 0x3C4);
static_assert(offsetof(daTag_Mk_c, eventIndex) == 0x3C6);
static_assert(offsetof(daTag_Mk_c, staffIndex) == 0x3C8);
static_assert(offsetof(daTag_Mk_c, cutEndTimer) == 0x3CC);
static_assert(offsetof(daTag_Mk_c, itemId) == 0x3D0);
static_assert(offsetof(daTag_Mk_c, staffName) == 0x3D4);
