#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_ext.h"
// HD: the ripple object grew to 0x90; its small callback subobject is at +0x7C.
struct IbRippleTail {
    u8 _00[0x10];
    be<f32> mRate;
};
struct IbRipple {
    be<u32> __vtbl;
    u8 mAnimation[0x34];
    be<u32> mAnimationVtable;
    u8 _3C[12];
    be<u32> field_0x48;
    u8 _4C[0x20];
    be<u32> field_0x6c, field_0x70, field_0x74, field_0x78;
    IbRippleTail mTail;
};
struct daIball_c : fopAc_ac_c {
    enum Mode { MODE_WAIT = 0, MODE_WATER = 1 };
    using ModeFunc = gabi::ptmf<void (daIball_c::*)()>;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    gptr<J3DModel> mpModel;
    mDoExt_btkAnm mBtkAnm;
    mDoExt_brkAnm mBrkAnm[2];
    // HD: mBckAnm is just the 16-byte frame controller here.
    J3DFrameCtrl mBckAnm;
    IbRipple mRippleCb;
    be<s32> mTimer;
    u8 m670[4];
    be<s32> mPlaySpeedIdx;
    u8 m678[4];
    be<f32> mPrevSpeedY;
    be<u8> mMode, mbPlayedSe;
    u8 m682[2];
    LIGHT_INFLUENCE mLight;
    be<f32> mLightFlickerMult;
    void setPointLight();
    void createDisappearEffect(s32, s32);
    BOOL createItem();
    BOOL dead();
    void checkGeo();
    void mode_wait_init();
    void mode_wait();
    void mode_water_init();
    void mode_water();
    void animControl();
    void damage();
    void set_mtx();
    void CreateInit();
    BOOL _daIball_draw();
    BOOL _daIball_execute();
    BOOL _daIball_delete();
    cPhs_State _daIball_create();
    BOOL CreateHeap();
};
WWHD_SIZE(IbRipple, 0x90);
WWHD_OFFSET(IbRipple, mTail, 0x7C);
WWHD_OFFSET(daIball_c, mAcch, 0x3AC);
WWHD_OFFSET(daIball_c, mAcchCir, 0x570);
WWHD_OFFSET(daIball_c, mStts, 0x5B0);
WWHD_OFFSET(daIball_c, mCyl, 0x5EC);
WWHD_OFFSET(daIball_c, mpModel, 0x71C);
WWHD_OFFSET(daIball_c, mBtkAnm, 0x720);
WWHD_OFFSET(daIball_c, mBrkAnm, 0x794);
WWHD_OFFSET(daIball_c, mBckAnm, 0x884);
WWHD_OFFSET(daIball_c, mRippleCb, 0x894);
WWHD_OFFSET(daIball_c, mTimer, 0x924);
WWHD_OFFSET(daIball_c, mLight, 0x93C);
WWHD_OFFSET(daIball_c, mLightFlickerMult, 0x960);
WWHD_SIZE(daIball_c, 0x964);
