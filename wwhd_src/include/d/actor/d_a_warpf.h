/* Boss warp flower. Private HD layout. */
#pragma once
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_item.h"
#include "d/d_level_se.h"
#include "gc_api/JSystem/JPAEmitter.h"
template<class T> inline T* Warpf_view(u32 address) {return gabi::at<T>(address);}
struct WarpfStageInfo {
    u8 mPadding0[9];
    be<u8> mSaveTable;
};
WWHD_OFFSET(WarpfStageInfo, mSaveTable, 9);
struct WarpfEmitter : JPABaseEmitter {
    u8 mPadding70[0x1D7];
    be<u8> mGlobalAlpha;
    u8 mPadding248[12];
    be<u32> mFlags;
};
WWHD_OFFSET(WarpfEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(WarpfEmitter, mFlags, 0x254);
WWHD_SIZE(WarpfEmitter, 0x258);
struct WarpfBckAnimation {
    u8 mFrameCtrl[0x10];
    be<u32> mVtable;
    u8 mMatrix[0x34];
    be<u32> mMatrixVtable;
    u8 mPadding4C[12];
    be<u32> mMatrixAnm;
    u8 mPadding5C[0x20];
    be<u32> mFrameData, mBlend, mJoint, mModel;
};
WWHD_OFFSET(WarpfBckAnimation, mVtable, 0x10);
WWHD_OFFSET(WarpfBckAnimation, mMatrix, 0x14);
WWHD_OFFSET(WarpfBckAnimation, mMatrixVtable, 0x48);
WWHD_OFFSET(WarpfBckAnimation, mMatrixAnm, 0x58);
WWHD_OFFSET(WarpfBckAnimation, mFrameData, 0x7C);
WWHD_OFFSET(WarpfBckAnimation, mBlend, 0x80);
WWHD_OFFSET(WarpfBckAnimation, mJoint, 0x84);
WWHD_OFFSET(WarpfBckAnimation, mModel, 0x88);
WWHD_SIZE(WarpfBckAnimation, 0x8C);
struct WarpfPlayerVtable {
    u8 mPadding0[0x4C];
    be<u32> mCheckFly;
};
WWHD_OFFSET(WarpfPlayerVtable, mCheckFly, 0x4C);
struct daWarpf_c : fopAc_ac_c {
    using InitFunc = void(daWarpf_c::*)(int);
    using ActionFunc = BOOL(daWarpf_c::*)(int);
    BOOL CreateHeap();
    bool _delete();
    BOOL checkEndDemo();
    void onEndDemo() {}
    cPhs_State CreateInit();
    cPhs_State _create();
    bool _execute();
    void eventOrder();
    void checkOrder();
    void demo_proc();
    void initWait(int);
    BOOL actWait(int);
    void initWarpStart(int);
    BOOL actWarpStart(int);
    void initWarpMode_1(int);
    BOOL actWarpMode_1(int);
    void initWarpMode_2(int);
    BOOL actWarpMode_2(int);
    void initWarpMode_3(int);
    BOOL actWarpMode_3(int);
    void initEndWait(int);
    BOOL actEndWait(int);
    BOOL check_warp_event();
    f32 get_distance();
    f32 get_earth_pos();
    void set_effect();
    void set_effect_wind00();
    u32 get_angle_wind01();
    void anim_play();
    void setEndAnim();
    void set_se();
    void set_mtx();
    bool _draw();

    const char* arcname();

    /* 0x3AC */ be<u32> m290;  /* JPABaseEmitter* */
    /* 0x3B0 */ be<u32> m294;
    /* 0x3B4 */ be<u32> m298;
    /* 0x3B8 */ be<u32> m29C;
    /* 0x3BC */ request_of_phase_process_class mPhase;
    /* 0x3C4 */ gptr<J3DModel> m2A8;
    /* 0x3C8 */ be<u32> m2AC;  /* mDoExt_brkAnm* */
    /* 0x3CC */ be<u32> m2B0;  /* mDoExt_bckAnm* */
    /* 0x3D0 */ be<u32> m2B4;  /* mDoExt_btkAnm* */
    /* 0x3D4 */ be<u32> m2B8;  /* mDoExt_brkAnm* */
    /* 0x3D8 */ be<u32> m2BC;  /* mDoExt_brkAnm* */
    /* 0x3DC */ gptr<J3DModel> m2C0;
    /* 0x3E0 */ be<s32> mStaffID;
    /* 0x3E4 */ be<s32> mWarpTimer;
    /* 0x3E8 */ u8 m2CC[4];
    /* 0x3EC */ be<s32> m2D0;
    /* 0x3F0 */ u8 m2D4[2];
    /* 0x3F2 */ be<s16> m2D6;
    /* 0x3F4 */ be<s32> m2D8;
    /* 0x3F8 */ be<s32> mStageNo;
    /* 0x3FC */ be<s32> m2E0;
    /* 0x400 */ be<s32> m2E4;
    /* 0x404 */ be<u32> m2E8;  /* dLevelSe_c* */
    /* 0x408 */ cXyz m2EC;
};
WWHD_OFFSET(daWarpf_c, mPhase, 0x3BC);
WWHD_OFFSET(daWarpf_c, mStaffID, 0x3E0);
WWHD_OFFSET(daWarpf_c, m2D6, 0x3F2);
WWHD_OFFSET(daWarpf_c, mStageNo, 0x3F8);
WWHD_OFFSET(daWarpf_c, m2EC, 0x408);
WWHD_SIZE(daWarpf_c, 0x414);


u32 Warpf_fastCreate(s16, u32, cXyz*, cXyz*, void*);
u32 Warpf_brkConstruct(u32);
BOOL Warpf_brkInit(u32, J3DModelData*, void*, bool, s32, f32, s16, s16, bool, s32);
void Warpf_brkEntryModel(u32, J3DModel*, f32);
u32 Warpf_btkConstruct(u32);
void Warpf_frameConstruct(u32, s32);
void Warpf_matrixConstruct(void*);
s32 Warpf_calcTimer(be<s32>*);
f32 Warpf_groundCheck(cXyz*);
s16 Warpf_cameraAngleX(u32);
s16 Warpf_cameraAngleY(u32);
void Warpf_actorDestruct(daWarpf_c*, s32);

struct WarpfInitEntry {
    gabi::ptmf<daWarpf_c::InitFunc> mFunction;
};
struct WarpfActionEntry {
    gabi::ptmf<daWarpf_c::ActionFunc> mFunction;
};
WWHD_SIZE(WarpfInitEntry, 8);
WWHD_SIZE(WarpfActionEntry, 8);
struct WarpfTU {
    u32 mBckVtable;
    u32 mActorVtable;
    u32 mBaseAnmVtable;
    u32 mMatrixVtable;
    u32 mHeapCallback;
    u32 mHeaderStorage;
    u32 mHeaderRecords;
};

namespace daWarpf_prm {
inline u32 getSetType(daWarpf_c* actor) {return fopAcM_GetParam(actor) >> 0x1C;}
}

WWHD_OFFSET(daWarpf_c, m290, 0x3AC);
WWHD_OFFSET(daWarpf_c, m294, 0x3B0);
WWHD_OFFSET(daWarpf_c, m298, 0x3B4);
WWHD_OFFSET(daWarpf_c, m29C, 0x3B8);
WWHD_OFFSET(daWarpf_c, m2A8, 0x3C4);
WWHD_OFFSET(daWarpf_c, m2AC, 0x3C8);
WWHD_OFFSET(daWarpf_c, m2B0, 0x3CC);
WWHD_OFFSET(daWarpf_c, m2B4, 0x3D0);
WWHD_OFFSET(daWarpf_c, m2B8, 0x3D4);
WWHD_OFFSET(daWarpf_c, m2BC, 0x3D8);
WWHD_OFFSET(daWarpf_c, m2C0, 0x3DC);
WWHD_OFFSET(daWarpf_c, mWarpTimer, 0x3E4);
WWHD_OFFSET(daWarpf_c, m2D0, 0x3EC);
WWHD_OFFSET(daWarpf_c, m2D8, 0x3F4);
WWHD_OFFSET(daWarpf_c, m2E0, 0x3FC);
WWHD_OFFSET(daWarpf_c, m2E4, 0x400);
WWHD_OFFSET(daWarpf_c, m2E8, 0x404);
