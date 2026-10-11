/* Base background collision check: GameCube names and HD size 0x14.
 * ChkSameActorPid omits GameCube const for verifier member-pointer support. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
class cBgS_Chk {
  public:
    u8 ChkSameActorPid(u32 pid);
    /* 0x00 */ gptr<void> mPolyPassChk;
    /* 0x04 */ gptr<void> mGrpPassChk;
    /* 0x08 */ be<u32> mActorPid;
    /* 0x0C */ be<u8> mSameActorChk;
    u8 padding[3];
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_OFFSET(cBgS_Chk, mActorPid, 8);
WWHD_OFFSET(cBgS_Chk, mSameActorChk, 0xC);
WWHD_OFFSET(cBgS_Chk, __vtbl, 0x10);
WWHD_SIZE(cBgS_Chk, 0x14);
void *operator_new(u32);
void operator_delete(void *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
void constructBaseCheck(cBgS_Chk *);
void destroyBaseCheck(cBgS_Chk *, s32);
