// Pirate ship rope minigame platforms.
#pragma once
#include "gc_api/gc_api.h"
namespace daObjHlift {
struct Act_c : dBgS_MoveBgActor {
    struct size_data {
        be<s16> mBdlResIdx;  /* was field_0x00: CreateHeap 023561E4 loads the platform model Hlift[it] for the size index mSizeIdx (tww CreateHeap the same) */
        be<s16> mDzbResIdx;  /* was field_0x01: Mthd_Create 02356090 passes it to MoveBGCreate as the collision (dzb) resource of the size (tww Create the same) */
        be<u32> mHeapSize;  /* was field_0x02: Mthd_Create 02356090 passes it to MoveBGCreate as the heap size of the size (tww Create the same) */
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
    /* 0x3F0 */ be<s32> mDistIdx;  /* was field_0x2D8: Mthd_Create 02356090: prm PRM_DIST; distance() (inlined into mode_l_u 02356BD0 / mode_u_l 02356EE0) reads M_up_dist[it], the lift's travel height */
    /* 0x3F4 */ be<s32> mSizeIdx;  /* was field_0x2DC: Mthd_Create 02356090: prm PRM_SIZE; CreateHeap 023561E4 picks the model resource from M_data_size[it] */
    /* 0x3F8 */ be<s32> mMode;  /* was field_0x2E0: mode_upper_init 023562EC / mode_lower_init 02356344 / mode_l_u_init 02356B2C / mode_u_l_init 02356E40 / mode_demoreq_init 02356A14 set Mode_*; Execute 02356828 calls mode_proc[it] */
    /* 0x3FC */ be<s32> mNextMode;  /* was field_0x2E4: mode_demoreq_init 02356A14 stores the mode to start after the demo request; mode_demoreq 02357040 starts mode_u_l_init when it is Mode_U_L, else mode_l_u_init; Create 023564F4 sets Mode_UNK */
    /* 0x400 */ be<s16> mVibTimer;  /* was field_0x2E8: vib_set 02356A00 sets 10; vib_proc 02356728 counts it down while the lift shakes */
    /* 0x402 */ be<s16> mVibAngle;  /* was field_0x2EA: vib_set 02356A00 sets 0x4000; vib_proc 02356728 adds 0x4000 per frame, the phase of the shake sine */
    /* 0x404 */ be<f32> mVibOffsetY;  /* was field_0x2EC: vib_proc 02356728: sine * 2.5 * factor, 0 at the end; set_mtx 02356358 adds it to the model's height; Create 023564F4 clears it */
    /* 0x408 */ be<f32> mMoveSpeed;  /* was field_0x2F0: mode_l_u 02356BD0 / mode_u_l 02356EE0: speed of the cLib_chaseF of current.pos.y (eased to 10, then 2 near the end); mode_l_u_init 02356B2C / mode_u_l_init 02356E40 clear it */
    /* 0x40C */ be<s16> mTimeLag;  /* was field_0x2F4: lagSet (inlined into mode_l_u_init 02356B2C / mode_u_l_init 02356E40): L_time_lag[L_time_lag_num]; mode_l_u 02356BD0 / mode_u_l 02356EE0 wait while it counts down */
    /* 0x40E */ be<u8> mDemoRunning;  /* was field_0x2F6: mode_demoreq 02357040 sets 1 when the demo event is accepted; chk_demo_end 02356664 clears it at the event's end and returns !it; Create 023564F4 / mode_demoreq_init 02356A14 clear it */
    /* 0x40F */ u8 field_0x2F7;
    /* 0x410 */ be<s16> mEventId;
    /* 0x412 */ u8 field_0x2FA[2];
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mMode, 0x3F8);
WWHD_OFFSET(Act_c, mEventId, 0x410);
WWHD_SIZE(Act_c, 0x414);
} // namespace daObjHlift
