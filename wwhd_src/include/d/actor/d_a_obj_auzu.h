/* WWHD whirlpool. */
#pragma once
#include "bindings.h"
namespace daObjAuzu {
struct Act_c : fopAc_ac_c {
 request_of_phase_process_class mPhase; gptr<J3DModel> mpModel; mDoExt_btkAnm mBtk;
 be<s32> mType; be<f32> mScaleFactor; be<u8> mToAppear,mExists,mBgmStarted; u8 _437;
 be<u32> mTagId;
 u32 is_exist(); bool create_heap(); void set_mtx(); void init_mtx(); void set_state_map();
 s32 _create(); bool _delete(); void bgm_start(); void ship_whirl(); bool _execute(); bool _draw();
};
WWHD_OFFSET(Act_c,mType,0x42C); WWHD_OFFSET(Act_c,mTagId,0x438); WWHD_SIZE(Act_c,0x43C);
struct FloatColor { be<f32> r,g,b,a; };
}
