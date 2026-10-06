#pragma once
#include "f_op/f_op_actor.h"
struct daTag_Island_c : fopAc_ac_c {
  be<u16> arrivalState, reserved;
  be<u8> action;
  u8 pad3B1;
  be<u16> flags;
  be<u32> message;
  be<s8> talkState;
  u8 pad3B9[3];
  be<u8> lessonStage;
  u8 pad3BD;
  be<s16> timer;
  be<s32> staffId;
  be<s16> eventId;
  u8 pad3C6[2];
  be<u32> nextMessage;
};
WWHD_OFFSET(daTag_Island_c, arrivalState, 0x3AC);
WWHD_OFFSET(daTag_Island_c, action, 0x3B0);
WWHD_OFFSET(daTag_Island_c, flags, 0x3B2);
WWHD_OFFSET(daTag_Island_c, message, 0x3B4);
WWHD_OFFSET(daTag_Island_c, talkState, 0x3B8);
WWHD_OFFSET(daTag_Island_c, lessonStage, 0x3BC);
WWHD_OFFSET(daTag_Island_c, timer, 0x3BE);
WWHD_OFFSET(daTag_Island_c, staffId, 0x3C0);
WWHD_OFFSET(daTag_Island_c, eventId, 0x3C4);
WWHD_OFFSET(daTag_Island_c, nextMessage, 0x3C8);
static_assert(sizeof(daTag_Island_c) == 0x3CC);
