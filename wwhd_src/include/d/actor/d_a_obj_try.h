/* Tower of the Gods glowing statue/pillar. GameCube names with verified HD layout.
 * HD: the actor base and animation wrapper expand the original members; correction
 * aliases keep the placement-correction actor's existing interface byte-identical. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjTry {
enum Type_e { UNK_2 = 2, UNK_3, UNK_4, UNK_5, UNK_6, UNK_7, UNK_8, UNK_9, UNK_10, UNK_11, UNK_12, Type_Max };
class Act_c : public fopAc_ac_c {
  public:
    enum Mode { MODE_RESTART, MODE_WAIT, MODE_CARRY, MODE_DROP, MODE_SINK };
    void set_mtx();
    s32 _delete();
    void eff_hit_water_splash();
    void damaged();
    void init_cc();
    s32 check_circle();
    void make_vib();
    s32 create_heap();
    void mode_drop();
    void mode_carry();
    void mode_restart_init();
    void mode_sink_init();
    f64 get_water_h();
    s32 _draw();
    s32 _execute();
    s32 damage_cc_proc();
    void se_fall_water();
    s32 _create();
    s32 chk_appear();
    u8 chk_sink_water();
    u8 chk_sinkdown_water();
    void mode_carry_init();
    void cull_set_draw();
    void cull_set_move();
    void init_mtx();
    void mode_wait_init();
    void set_senv(s32 sound, s32 context);
    void eff_clr_bingo();
    void cam_lockoff();
    void eff_land_smoke();
    void prm_set_swSave(s32 bit);
    s32 damage_bg_proc();
    s32 bound();
    void mode_restart();
    void damage_bg_proc_directly();
    void mode_sink();
    void mode_drop_init();
    void calc_drop_param(f32* gravity, f32* drag, f32* stream);
    void mode_wait();
    s32 mode_proc_call();
    void eff_set_bingo(s32 sound, s32 atHome);
    /* 0x3AC */ request_of_phase_process_class phase;
    /* 0x3B4 */ be<u32> model;
    /* 0x3B8 */ mDoExt_brkAnm mBrk;
    /* 0x430 */ dBgS_ObjAcch mAcch;
    /* 0x5F4 */ dBgS_AcchCir mAcchCir;
    /* 0x634 */ dCcD_Stts mStts;
    /* 0x670 */ dCcD_Cyl mCyl;
    /* 0x7A0 */ be<u32> type, mode;
    /* 0x7A8 */ be<u32> mPreviousMode;
    /* 0x7AC */ be<s16> mTimer, mLandTimer;
    /* 0x7B0 */ be<u8> mLandEffectDone, mMovingGround, mCollisionDelay;
    /* 0x7B3 */ u8 paddingSpeed;
    /* 0x7B4 */ be<f32> mPreviousSpeedY;
    /* 0x7B8 */ union {
        cXyz field_0x63C;
        cXyz correctPosition;
    };
    /* 0x7C4 */ union {
        be<s16> field_0x648;
        be<s16> correctYaw;
    };
    /* 0x7C6 */ union {
        be<s16> mCorrectionYaw;
        be<s16> restartDelay;
    };
    /* 0x7C8 */ union {
        be<u8> field_0x64C;
        be<u8> correctEnabled;
    };
    /* 0x7C9 */ union {
        be<u8> field_0x64D;
        be<u8> correctImmediate;
    };
    /* 0x7CA */ be<u8> mCorrecting, mLeftHomeRoom;
    /* 0x7CC */ be<u8> appears, bingoActive, mUnused, mWaterSplashDone;
    /* 0x7D0 */ dPa_followEcallBack mFollowCallback;
    /* 0x7E4 */ be<u32> emitter;
};
} // namespace daObjTry
using daObjTry_c = daObjTry::Act_c;
WWHD_OFFSET(daObjTry::Act_c, phase, 0x3AC);
WWHD_OFFSET(daObjTry::Act_c, model, 0x3B4);
WWHD_OFFSET(daObjTry::Act_c, mBrk, 0x3B8);
WWHD_OFFSET(daObjTry::Act_c, mAcch, 0x430);
WWHD_OFFSET(daObjTry::Act_c, mAcchCir, 0x5F4);
WWHD_OFFSET(daObjTry::Act_c, mStts, 0x634);
WWHD_OFFSET(daObjTry::Act_c, mCyl, 0x670);
WWHD_OFFSET(daObjTry::Act_c, type, 0x7A0);
WWHD_OFFSET(daObjTry::Act_c, mPreviousMode, 0x7A8);
WWHD_OFFSET(daObjTry::Act_c, mTimer, 0x7AC);
WWHD_OFFSET(daObjTry::Act_c, mLandTimer, 0x7AE);
WWHD_OFFSET(daObjTry::Act_c, mPreviousSpeedY, 0x7B4);
WWHD_OFFSET(daObjTry::Act_c, correctPosition, 0x7B8);
WWHD_OFFSET(daObjTry::Act_c, correctYaw, 0x7C4);
WWHD_OFFSET(daObjTry::Act_c, correctEnabled, 0x7C8);
WWHD_OFFSET(daObjTry::Act_c, appears, 0x7CC);
WWHD_OFFSET(daObjTry::Act_c, mFollowCallback, 0x7D0);
WWHD_OFFSET(daObjTry::Act_c, emitter, 0x7E4);
WWHD_SIZE(daObjTry::Act_c, 0x7E8);
