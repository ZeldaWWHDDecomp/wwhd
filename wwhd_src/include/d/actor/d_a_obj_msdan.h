/* Msdan staircase spawner, WWHD layout. GameCube source methods are placeholders;
 * names follow its Act_c interface. HD actor base is 0x3AC; size 0x3BC. */
#pragma once
#include "gc_api/gc_api.h"

enum { fpcNm_Obj_MsdanSub_e = 0x4F, JA_SE_READ_RIDDLE_1 = 0x806 };

namespace daObjMsdan {
class Act_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1002D7F0 }; // HD: virtual destructor
    enum Mode_e { MODE_WAIT_e, MODE_ORDER_e, MODE_EVENT_e, MODE_END_e };
    Act_c();
    enum Prm_e {
        PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0,
        PRM_SIZE_W = 1, PRM_SIZE_S = 0x10,
        PRM_SOUND_W = 1, PRM_SOUND_S = 0x12,
        PRM_EVID_W = 8, PRM_EVID_S = 0x18,
    };
    u32 prm_get_swSave();
    u32 prm_get_size();
    u32 prm_get_sound();
    u32 prm_get_evId();
    cPhs_State Mthd_Create();
    BOOL Mthd_Execute();
    BOOL Mthd_Delete();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<u32> mMode;
};
WWHD_OFFSET(Act_c, mPhs, 0x3AC);
WWHD_OFFSET(Act_c, mEventIdx, 0x3B4);
WWHD_OFFSET(Act_c, mMode, 0x3B8);
WWHD_SIZE(Act_c, 0x3BC);
}  // namespace daObjMsdan

/* HD: positionless sound call, bound by this unit's external glue. */
void mDoAud_seStart(u32 id);
