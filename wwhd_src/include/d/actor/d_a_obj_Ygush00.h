// Spring-water gush. HD: GC actor fields 0x290 move to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"

class daObjGryw00_c : public fopAc_ac_c {
public:
    f32 get_draw_water_lv(fopAc_ac_c* actor);
};
class daObjYgush00_c : public fopAc_ac_c {
public:
    daObjYgush00_c();
    enum Param_e {PRM_TYPE_W=3, PRM_TYPE_S=0};
    int param_get_arg();
    cPhs_State _create();
    bool _delete();
    bool _execute();
    bool _draw();
    bool create_heap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ mDoExt_bckAnm mBckAnm;
    /* 0x4B8 */ be<s32> mType;
    /* 0x4BC */ gptr<daObjGryw00_c> mpGryw00;
};
WWHD_OFFSET(daObjYgush00_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjYgush00_c, mBckAnm, 0x42C);
WWHD_OFFSET(daObjYgush00_c, mType, 0x4B8);
WWHD_SIZE(daObjYgush00_c, 0x4C0);


// HD: unit-local view of the inline bck animation construction stores.
struct YgushBckLayout {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> mVtable;
    /* 0x14 */ u8 mCalcBase[0x34];
    /* 0x48 */ be<u32> mMayaVtable;
    u8 mPadding[0xC];
    /* 0x58 */ be<u32> mCalcAnm;
    u8 mPadding2[0x20];
    /* 0x7C */ be<u32> mOne[4];
};
WWHD_SIZE(YgushBckLayout, 0x8C);
WWHD_OFFSET(YgushBckLayout, mOne, 0x7C);
void YgushFrameCtrl_init(J3DFrameCtrl* controller, s32 start);
void YgushMtxCalc_construct(void* calcBase);
void YgushMtxCalc_destruct(be<u32>* vtable, s32 flags);
