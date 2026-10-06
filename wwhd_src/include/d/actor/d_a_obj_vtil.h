#pragma once
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "bindings.h"

// Tingle statue. Layout measured from HD constructor and profile 101CDCF4.
struct daObjVtil_c : fopAc_ac_c {
    gptr<J3DModel> mModel;
    request_of_phase_process_class mPhase;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCircle;
    dCcD_Stts mCollisionStatus;
    dCcD_Cyl mCylinder;
    be<u32> mStatueType;
    be<f32> mPreviousVerticalSpeed;
    be<u8> mMode;
    be<u8> mDeleteState;
    be<u8> mFirstGroundHit;
    u8 _733[5];
    LIGHT_INFLUENCE mLight;
};
WWHD_OFFSET(daObjVtil_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVtil_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjVtil_c, mAcch, 0x3B8);
WWHD_OFFSET(daObjVtil_c, mAcchCircle, 0x57C);
WWHD_OFFSET(daObjVtil_c, mCollisionStatus, 0x5BC);
WWHD_OFFSET(daObjVtil_c, mCylinder, 0x5F8);
WWHD_OFFSET(daObjVtil_c, mStatueType, 0x728);
WWHD_OFFSET(daObjVtil_c, mPreviousVerticalSpeed, 0x72C);
WWHD_OFFSET(daObjVtil_c, mMode, 0x730);
WWHD_OFFSET(daObjVtil_c, mDeleteState, 0x731);
WWHD_OFFSET(daObjVtil_c, mFirstGroundHit, 0x732);
WWHD_OFFSET(daObjVtil_c, mLight, 0x738);
WWHD_SIZE(daObjVtil_c, 0x75C);
