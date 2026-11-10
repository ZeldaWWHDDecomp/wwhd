/* Hyrule Castle's Link statue. GameCube fields keep their tww names.
 * HD: actor base +0x11C; the GHS member pointer is eight bytes instead of twelve,
 * so members after mCurrentActProc are at GameCube +0x118. */
#pragma once
#include "bindings.h"
#include "gc_api/SSystem/c_phase.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_particle.h"

namespace ylzou {
struct EffSmokeCB : dPa_smokeEcallBack {
    /* 0x20 */ cXyz mPos;  /* was field_0x20: eff_set_slip_smoke_pos 023BE21C: the smoke position from the matrix stack (tww multVecZero(&mSmokeCbs[i].it)); eff_smoke_slip_start 023BE33C passes it as the ID_AK_JT_ELEMENTSMOKE00 particle position (tww) */
    /* 0x2C */ csXyz mAngle;  /* was field_0x2C: eff_set_slip_smoke_pos 023BE21C: (0, current.angle.y, 0) (tww mSmokeCbs[i].it.set(0, current.angle.y, 0)); eff_smoke_slip_start 023BE33C passes it as the particle angle (tww) */
    /* 0x32 */ u8 padding[2];
};
WWHD_OFFSET(EffSmokeCB, mPos, 0x20);
WWHD_OFFSET(EffSmokeCB, mAngle, 0x2C);
WWHD_SIZE(EffSmokeCB, 0x34);
} // namespace ylzou

struct daObjYLzou_c : fopAc_ac_c {
    enum Param_e { PARAM_SWSAVE_W = 8, PARAM_SWSAVE_S = 0 };
    typedef void (daObjYLzou_c::*ProcFunc)();
    union ProcHD {
        gabi::ptmf<ProcFunc> fn;
        be<u32> words[2];
    };
    /* 0x3AC */ request_of_phase_process_class mPhs;  /* was field_0x290: _create 023BE004 / _delete 023BE198: the phase of the "YLzou" resource load / delete (tww dComIfG_resLoad(&it, l_arcname)); after tww daObjDoguu_c::mPhs and the other actors' request_of_phase_process_class mPhs */
    /* 0x3B4 */ gptr<J3DModel> mpModel;  /* was field_0x298: create_heap 023BDD20: the statue model from bdl_table[mTriforceComplete] (tww it = model); set_mtx 023BDEE4 / init_mtx 023BDFE4 place it; _create 023BE004 hands its base matrix to the actor; _draw 023BE6B8 draws it (tww) */
    /* 0x3B8 */ gptr<dBgW> mpBgW;  /* was field_0x29C: create_heap 023BDD20: the dBgW from dzb_table[mTriforceComplete] on mpModel's base matrix (tww it = dBgW_NewSet(..)); _create 023BE004 registers it, _execute 023BE5F8 moves it, _delete 023BE198 releases it (tww) */
    /* 0x3BC */ Mtx34 mMtx;  /* was field_0x2A0: set_mtx 023BDEE4: copy of the matrix stack after placing the statue (tww cMtx_copy(mDoMtx_stack_c::get(), it)) */
    /* 0x3EC */ ProcHD mCurrentActProc;
    /* 0x3F4 */ be<s32> mActionIdx;
    /* 0x3F8 */ be<s32> mSwSaveNo;  /* was field_0x2E0: _create 023BE004: param_get_swSave() (tww; the PARAM_SWSAVE bits); set_start_type 023BDADC / move_ylzou_demo_start_wait_act_proc test that switch when it is not 0xFF (tww fopAcM_isSwitch(this, it)) */
    /* 0x3FC */ be<s16> mEventIdx;  /* was field_0x2E4: demo_regist_wait_act_proc 023BEA90: the event index of l_demo_name[mDemoIdx] (tww it = dComIfGp_evmng_getEventIdx(..)); the demo act procs order it and test existence / end (tww); -1 when none */
    /* 0x3FE */ u8 paddingEvent[2];
    /* 0x400 */ be<s32> mTriforceComplete;  /* was field_0x2E8: set_start_type 023BDADC: our local triforce_complete (tww it = triforce_complete); create_heap 023BDD20 / _create 023BE004 index bdl_table / dzb_table / solidHeapSize_table with it (tww) */
    /* 0x404 */ be<s32> mDemoIdx;
    /* 0x408 */ ylzou::EffSmokeCB mSmokeCbs[2];
    /* 0x470 */ be<s16> mVibAngle;  /* was field_0x358: vib_proc 023BE548 adds 0x4000 each frame (tww it += l_HIO.m14 / 0x4000); set_mtx 023BDEE4 lifts the statue by mVibAmplitude * sin(it) (tww current.pos.y + field_0x35C * cM_ssin(it)); our local angle */
    /* 0x472 */ u8 paddingAngle[2];
    /* 0x474 */ be<f32> mVibAmplitude;  /* was field_0x35C: vib_proc 023BE548: grows by 0.05 to 0.8 in vibration state 1, shrinks to 0.4 / 0 in the others (tww); set_mtx 023BDEE4 the shake height factor (our locals amplitude / height) */
    /* 0x478 */ be<u8> mSmokeState;  /* was field_0x360: eff_smoke_proc 023BE4B4: the smoke state in effect; when mSmokeReq differs the slip smoke starts (1) or ends, and it takes mSmokeReq; while 1 the smoke positions follow (tww) */
    /* 0x479 */ be<u8> mSmokeReq;  /* was field_0x361: the requested slip smoke state: the demo act init procs (023BE864, 023BE880, ..) set 1 / 0 (tww); eff_smoke_proc 023BE4B4 applies it (our local requested) */
    /* 0x47A */ u8 paddingSmoke[2];
    /* 0x47C */ be<s32> mVibState;  /* was field_0x364: vib_proc 023BE548: the vibration state in effect, taken from mVibReq when they differ; selects the mVibAmplitude rule (tww switch (it)) */
    /* 0x480 */ be<s32> mVibReq;  /* was field_0x368: the requested vibration state: move_ylzou_demo_vib_act_init_proc 023BE800 1, move_ylzou_demo_move_act_init_proc 023BE864 2, later 0 (tww); vib_proc 023BE548 applies it (our local requested) */

    void set_start_type();
    void set_mtx();
    void init_mtx();
    bool create_heap();
    void eff_set_slip_smoke_pos();
    void eff_smoke_slip_start();
    void eff_smoke_slip_end();
    void eff_smoke_slip_remove();
    void eff_smoke_proc();
    void vib_proc();
    s32 _create();
    bool _delete();
    void move_ylzou_demo_start_wait_act_proc();
    void demo_regist_wait_act_proc();
    void demo_vib_start_wait_act_proc();
    void demo_vib_act_proc();
    void move_ylzou_demo_move_act_proc();
    void go_up_stairs_demo_move_act_proc();
    void demo_end_wait_act_proc();
    void move_ylzou_demo_start_wait_act_init_proc();
    void demo_regist_wait_act_init_proc();
    void move_ylzou_demo_vib_start_wait_act_init_proc();
    void move_ylzou_demo_vib_act_init_proc();
    void move_ylzou_demo_move_act_init_proc();
    void demo_end_wait_act_init_proc();
    void open_wait_act_init_proc();
    void close_wait_act_init_proc();
    void go_up_stairs_demo_move_start_wait_act_init_proc();
    void go_up_stairs_demo_move_act_init_proc();
    void setup_action(s32);
    bool _execute();
    bool _draw();
    void wait_act_proc();
};
WWHD_SIZE(daObjYLzou_c::ProcHD, 8);
WWHD_OFFSET(daObjYLzou_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjYLzou_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjYLzou_c, mpBgW, 0x3B8);
WWHD_OFFSET(daObjYLzou_c, mMtx, 0x3BC);
WWHD_OFFSET(daObjYLzou_c, mCurrentActProc, 0x3EC);
WWHD_OFFSET(daObjYLzou_c, mActionIdx, 0x3F4);
WWHD_OFFSET(daObjYLzou_c, mSwSaveNo, 0x3F8);
WWHD_OFFSET(daObjYLzou_c, mEventIdx, 0x3FC);
WWHD_OFFSET(daObjYLzou_c, mTriforceComplete, 0x400);
WWHD_OFFSET(daObjYLzou_c, mDemoIdx, 0x404);
WWHD_OFFSET(daObjYLzou_c, mSmokeCbs, 0x408);
WWHD_OFFSET(daObjYLzou_c, mVibAngle, 0x470);
WWHD_OFFSET(daObjYLzou_c, mVibAmplitude, 0x474);
WWHD_OFFSET(daObjYLzou_c, mSmokeState, 0x478);
WWHD_OFFSET(daObjYLzou_c, mVibState, 0x47C);
WWHD_OFFSET(daObjYLzou_c, mSmokeReq, 0x479);
WWHD_OFFSET(daObjYLzou_c, mVibReq, 0x480);
WWHD_SIZE(daObjYLzou_c, 0x484);

enum YLzouAction {
    ACTION_MOVE_YLZOU_DEMO_START_WAIT,
    ACTION_MOVE_YLZOU_DEMO_REGIST_WAIT,
    ACTION_MOVE_YLZOU_DEMO_VIB_START_WAIT,
    ACTION_MOVE_YLZOU_DEMO_VIB,
    ACTION_MOVE_YLZOU_DEMO_MOVE,
    ACTION_MOVE_YLZOU_DEMO_END_WAIT,
    ACTION_OPEN_WAIT,
    ACTION_GO_UP_STAIRS_DEMO_REGIST_WAIT,
    ACTION_GO_UP_STAIRS_DEMO_MOVE_START_WAIT,
    ACTION_GO_UP_STAIRS_DEMO_MOVE,
    ACTION_GO_UP_STAIRS_DEMO_END_WAIT,
    ACTION_CLOSE_WAIT_1,
    ACTION_CLOSE_WAIT_2,
    ACTION_GO_UP_STAIRS2_DEMO_REGIST_WAIT,
    ACTION_GO_UP_STAIRS2_DEMO_END_WAIT,
};
enum YLzouDemoNameIdx { DEMO_NONE, DEMO_MOVE_YLZOU, DEMO_GO_UP_STAIRS, DEMO_GO_UP_STAIRS2 };
enum dRes_INDEX_YLZOU {
    dRes_INDEX_YLZOU_BDL_YLZOU_e = 4,
    dRes_INDEX_YLZOU_BDL_YLZOU2_e = 5,
    dRes_INDEX_YLZOU_DZB_YLZOU_e = 8,
    dRes_INDEX_YLZOU_DZB_YLZOU2_e = 9,
};

namespace ylzou {
enum SoundID {
    JA_SE_OBJ_H_STATUE_VIB = 0x6225,
    JA_SE_OBJ_H_STATUE_MOVE = 0x6226,
    JA_SE_OBJ_H_STATUE_STOP = 0x6A27,
};
struct EventFlagsHD {
    enum {
        MASTER_SWORD_CUTSCENE = 0x2D04,
        MASTER_SWORD_SWINGING_CUTSCENE = 0x3A04,
        HYRULE_COURTYARD_CUTSCENE = 0x3804,
        MOVED_HYRULE_STATUE = 0x3820,
        ZELDA_AWAKENED = 0x2D02,
        UNK_2C01 = 0x2C01,
        UNK_3980 = 0x3980,
    };
};
/* Private prefix view; the shared event manager is opaque. */
struct EventManagerPrefixHD {
    /* 0x00 */ u8 prefix[0x24];
    /* 0x24 */ u8 mException[12];
};
WWHD_OFFSET(EventManagerPrefixHD, mException, 0x24);
WWHD_SIZE(EventManagerPrefixHD, 0x30);
struct ActionTableHD {
    daObjYLzou_c::ProcHD entries[15];
};
struct DemoNamesHD {
    gptr<const char> entries[4];
};
struct SaveRootHD {
    /* 0x00 */ u8 prefix[0x20];
    /* 0x20 */ dSv_info_c info;
};
WWHD_OFFSET(SaveRootHD, info, 0x20);
WWHD_SIZE(SaveRootHD, 0x12C0);
using SavePointerHD = gptr<SaveRootHD>;
using ResourcePointerHD = gptr<void>;
using WordHD = be<u32>;
struct SmokePositionsHD {
    cXyz entries[2];
};
WWHD_SIZE(SmokePositionsHD, 24);
struct HeaderStaticsHD {
    /* 0x00 */ be<f32> negativePi, positivePi;
    /* 0x08 */ u8 angleZero, destructorZero;
    /* 0x0A */ u8 padding[2];
};
WWHD_OFFSET(HeaderStaticsHD, angleZero, 8);
WWHD_OFFSET(HeaderStaticsHD, destructorZero, 9);
WWHD_SIZE(HeaderStaticsHD, 12);
struct ZeroStaticsHD {
    be<u32> words[4];
};
WWHD_SIZE(ZeroStaticsHD, 16);
struct DestructorRecordHD {
    u8 data[12];
};
WWHD_SIZE(DestructorRecordHD, 12);
/* HD: only these emitter fields are touched here; the shared view does not include them. */
struct EmitterHD {
    /* 0x00 */ u8 prefix[0x34];
    /* 0x34 */ be<f32> mRate;
    /* 0x38 */ u8 paddingSpread[0x58 - 0x38];
    /* 0x58 */ be<f32> mSpread;
    /* 0x5C */ u8 paddingLife[4];
    /* 0x60 */ be<s16> mLifeTime;
    /* 0x62 */ u8 paddingSpeed[0x70 - 0x62];
    /* 0x70 */ be<f32> mDirectionalSpeed;
};
WWHD_OFFSET(EmitterHD, mRate, 0x34);
WWHD_OFFSET(EmitterHD, mSpread, 0x58);
WWHD_OFFSET(EmitterHD, mLifeTime, 0x60);
WWHD_OFFSET(EmitterHD, mDirectionalSpeed, 0x70);
WWHD_SIZE(EmitterHD, 0x74);
} // namespace ylzou

using YLzouSmokeConstructorHD = void *(*)(void *);
using YLzouSmokeDestructorHD = void (*)(void *, s32);
