#pragma once
#include "bindings.h"
struct YboilBrk : mDoExt_baseAnm { u8 padding[0x78-0x10]; };
class daObjYboil_c : public fopAc_ac_c {
public:
 request_of_phase_process_class mPhase;
 gptr<J3DModel> mModels[50];
 mDoExt_bckAnm mBck[50];
 mDoExt_btkAnm mBtk[50];
 YboilBrk mBrk[50];
 be<u32> mTimers[50];
 cXyz mPositions[50],mScales[50];
 be<f32> mScaleMax,mScaleMin;
 BOOL CreateHeap(); void pos_reset(s32); void set_mtx(); void CreateInit();
};
WWHD_OFFSET(daObjYboil_c,mModels,0x3B4);
WWHD_OFFSET(daObjYboil_c,mBck,0x47C);
WWHD_OFFSET(daObjYboil_c,mBtk,0x1FD4);
WWHD_OFFSET(daObjYboil_c,mBrk,0x367C);
WWHD_OFFSET(daObjYboil_c,mTimers,0x4DEC);
WWHD_OFFSET(daObjYboil_c,mPositions,0x4EB4);
WWHD_OFFSET(daObjYboil_c,mScales,0x510C);
WWHD_OFFSET(daObjYboil_c,mScaleMax,0x5364);
WWHD_SIZE(daObjYboil_c,0x536C);
