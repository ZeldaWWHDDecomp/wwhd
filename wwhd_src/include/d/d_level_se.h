/* Level sound emitter, GameCube names and WWHD layout.
 * HD: environment data is four bytes later; lifecycle calls the process base ctor/dtor. */
#pragma once
#include "bindings.h"

class dLevelSe_c : public base_process_class {
public:
    enum Flags { CUSTOM_PARAM = 1, CUSTOM_REVERB = 4, STOPPED = 8 };
    /* 0xB8 */ u8 _B8[0xE0 - 0xB8];
    /* 0xE0 */ cXyz mPos;
    /* 0xEC */ u8 _EC[0xF8 - 0xEC];
    /* 0xF8 */ be<u32> mParam;
    /* 0xFC */ be<u32> mSoundParam; // HD: tww field_0xf8.
    /* 0x100 */ be<s8> mReverb;
    /* 0x101 */ be<u8> mFlag;
    /* 0x102 */ u8 _102[2];
};
WWHD_OFFSET(dLevelSe_c, mPos, 0xE0);
WWHD_OFFSET(dLevelSe_c, mParam, 0xF8);
WWHD_OFFSET(dLevelSe_c, mSoundParam, 0xFC);
WWHD_OFFSET(dLevelSe_c, mReverb, 0x100);
WWHD_OFFSET(dLevelSe_c, mFlag, 0x101);
WWHD_SIZE(dLevelSe_c, 0x104);

void mDoAud_seStart(u32 soundId, cXyz* pos);
void mDoAud_seStart(u32 soundId, cXyz* pos, u32 param);
void base_process_class__ct(base_process_class* i_this);
void base_process_class__dt(base_process_class* i_this, u32 flags);
