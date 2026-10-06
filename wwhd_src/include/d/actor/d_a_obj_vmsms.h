#pragma once
#include "f_op/f_op_actor.h"
struct daObjVmsms_c : fopAc_ac_c {
    gptr<J3DModel> mModel;
    request_of_phase_process_class mPhs;
};
WWHD_OFFSET(daObjVmsms_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVmsms_c, mPhs, 0x3B0);
WWHD_SIZE(daObjVmsms_c, 0x3B8);
