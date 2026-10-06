#pragma once
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
struct daObjGnntakie_c : fopAc_ac_c {
    gptr<J3DModel> mModel;
    request_of_phase_process_class mPhs;
    mDoExt_btkAnm mpBtkAnm;
};
WWHD_OFFSET(daObjGnntakie_c, mModel, 0x3AC);
WWHD_OFFSET(daObjGnntakie_c, mPhs, 0x3B0);
WWHD_OFFSET(daObjGnntakie_c, mpBtkAnm, 0x3B8);
WWHD_SIZE(daObjGnntakie_c, 0x42C);
