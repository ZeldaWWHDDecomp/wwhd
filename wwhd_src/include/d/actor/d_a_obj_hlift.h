// Pirate ship rope minigame platforms.
#pragma once
#include "gc_api/gc_api.h"
namespace daObjHlift {
struct Act_c : dBgS_MoveBgActor {
    struct size_data {
        be<s16> field_0x00;
        be<s16> field_0x01;
        be<u32> field_0x02;
    };
    enum Mode_e { Mode_LOWER, Mode_L_U, Mode_UPPER, Mode_U_L, Mode_DEMOREQ, Mode_UNK };
    enum Prm_e {
        PRM_DIST_W = 3,
        PRM_DIST_S = 0,
        PRM_EVENTID_W = 8,
        PRM_EVENTID_S = 16,
        PRM_SIZE_W = 1,
        PRM_SIZE_S = 4,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 8
    };
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void mode_lower_init();
    void mode_lower();
    void mode_l_u_init();
    void mode_l_u();
    void mode_upper_init();
    void mode_upper();
    void mode_u_l_init();
    void mode_u_l();
    void mode_demoreq_init(Mode_e);
    void mode_demoreq();
    void set_mtx();
    void init_mtx();
    void rot_set();
    void vib_set();
    void vib_proc();
    u8 chk_demo_end(); // HD: the original byte XOR is not boolean-normalized.
    void se_whole();
    BOOL Execute(gptr<Mtx34>*);
    BOOL Draw();
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mModel1;
    /* 0x3EC */ gptr<J3DModel> mModel2;
    /* 0x3F0 */ be<s32> field_0x2D8;
    /* 0x3F4 */ be<s32> field_0x2DC;
    /* 0x3F8 */ be<s32> field_0x2E0;
    /* 0x3FC */ be<s32> field_0x2E4;
    /* 0x400 */ be<s16> field_0x2E8;
    /* 0x402 */ be<s16> field_0x2EA;
    /* 0x404 */ be<f32> field_0x2EC;
    /* 0x408 */ be<f32> field_0x2F0;
    /* 0x40C */ be<s16> field_0x2F4;
    /* 0x40E */ be<u8> field_0x2F6;
    /* 0x40F */ u8 field_0x2F7;
    /* 0x410 */ be<s16> mEventId;
    /* 0x412 */ u8 field_0x2FA[2];
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, field_0x2E0, 0x3F8);
WWHD_OFFSET(Act_c, mEventId, 0x410);
WWHD_SIZE(Act_c, 0x414);
} // namespace daObjHlift
