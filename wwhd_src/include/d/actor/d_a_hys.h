// Eye switch, private WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

struct HysBtpAnm {
    u8 mStorage[0x74];
    s32 init(J3DModelData*, J3DAnmTexPattern*, s32 play, s32 attr, f32 rate, s32 start, s32 end, s32 modify, s32 entry);
    void entry(J3DModelData*, s16 frame);
};
WWHD_SIZE(HysBtpAnm, 0x74);
void Hys_btpConstruct(HysBtpAnm*);
void Hys_sphConstruct(dCcD_Sph*);
void Hys_sphDestruct(dCcD_Sph*, s32);

class daHys_c : public dBgS_MoveBgActor {
public:
    typedef void (daHys_c::*ModeFunc)();
    struct ModeEntry { gabi::ptmf<ModeFunc> proc; };
    daHys_c();
    cPhs_State _create();
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Execute(gptr<Mtx34>* mtx);
    void mode_proc_call();
    void mode_wait();
    void mode_sw_on_init();
    void mode_sw_on();
    void mode_wait_init();
    BOOL Draw();
    /* 0x3E0 */ request_of_phase_process_class mPhs; /* GameCube 0x2C8 (+0x118) */
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ HysBtpAnm mBtpAnm;
    /* 0x460 */ dCcD_Stts mStts;                     /* GameCube 0x2E8 */
    /* 0x49C */ dCcD_Sph mSph;
    /* 0x5C8 */ be<u32> field_0x450;
    /* 0x5CC */ be<u32> mSwitchNo;
    /* 0x5D0 */ be<u8> field_0x458;
    /* 0x5D1 */ be<u8> mType;
    /* 0x5D2 */ u8 _5D2[2];
};
WWHD_OFFSET(daHys_c, mStts, 0x460);
WWHD_OFFSET(daHys_c, mSph, 0x49C);
WWHD_OFFSET(daHys_c, field_0x450, 0x5C8);
WWHD_OFFSET(daHys_c, mType, 0x5D1);
WWHD_SIZE(daHys_c, 0x5D4);
