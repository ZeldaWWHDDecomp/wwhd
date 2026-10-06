#pragma once
#include "bindings.h"
struct daObjShmrgrd_c : fopAc_ac_c {
 gptr<daObjShmrgrd_c> mpNext; gptr<J3DModel> mpModel;
 request_of_phase_process_class mPhase;
 u8 collision[0x96C-0x3BC];
 be<s32> mMode; be<s16> mCrushTimer; be<u8> mCrushState; u8 pad;
 be<f32> mUnused,mScaleY,mAngleZ,mAngleX,mAngleSpeedZ,mAngleSpeedX,mTargetHFrac,mCurHFrac,mVSpeed,mTopPos;
 u8 smoke[0x20]; Mtx34 mMtx; gptr<dBgW> mpBgW;
 void mode_lower_init(); void mode_upper_init(); void mode_u_l_init(); void mode_upper(); void mode_u_l();
 void vib_proc(); void crush_proc(); void calc_top_pos(); void vib_start(s16,f32); void crush_start();
 void register_list(); void leave_list(); s32 check_player_angle(fopAc_ac_c*);
 void init_mtx(); void set_mtx();
};
WWHD_OFFSET(daObjShmrgrd_c,mMode,0x96C);
WWHD_OFFSET(daObjShmrgrd_c,mMtx,0x9BC);
WWHD_SIZE(daObjShmrgrd_c,0x9F0);
