#ifndef WWHD_D_A_KYTAG02_H
#define WWHD_D_A_KYTAG02_H

#include "f_op/f_op_actor.h"

struct kytag02_class : fopAc_ac_c {
    /* 0x3AC */ gptr<void> mpPath;
    /* 0x3B0 */ cXyz mWindVec;
};
WWHD_OFFSET(kytag02_class, mpPath, 0x3AC);
WWHD_OFFSET(kytag02_class, mWindVec, 0x3B0);
WWHD_SIZE(kytag02_class, 0x3BC);

#endif
