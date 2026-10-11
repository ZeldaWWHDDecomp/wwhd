/* Background line query setup: GameCube method with HD layout.
 * A unit-local view avoids redefining the shared caller-facing inline method. */
#pragma once
#include "f_op/f_op_actor.h"
class dBgS_LinChk {
  public:
    void Set(cXyz *pi_start, cXyz *pi_end, fopAc_ac_c *pi_actor);
    u8 checkStorage[0x6C];
};
WWHD_SIZE(dBgS_LinChk, 0x6C);
void SetLineCheck(dBgS_LinChk *, cXyz *, cXyz *, u32);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
