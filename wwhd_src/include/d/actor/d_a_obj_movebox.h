/* Pushable box, WWHD layout. */
#pragma once
#include "bindings.h"
namespace daObjMovebox {
struct Act_c;
struct Attr_c {
    be<s16> m00,m02,m04,m06,m08,m0A;
    be<f32> m0C,m10,m14,m18,m1C,m20,mLandSmokeScale,m28,m2C,m30,m34;
    be<s16> m38; u8 _3A[2];
    be<f32> m3C,m40,m44,m48,m4C,m50,m54;
    be<s32> mModelFileIndex,mDZBFileIndex,mDZBHeapSize;
    be<f32> mScaleY,m68,m6C,mScaleXZ,m74;
    be<u32> mMoveSE,mCantMoveSE,mNormalFallSE,mWaterFallSE,mMagmaFallSE;
    be<s16> mCullMinX,mCullMinY,mCullMinZ,mCullMaxX,mCullMaxY,mCullMaxZ;
    be<u8> mbUseBGTevType,mbCastsShadow,m9A; u8 _9B;
};
WWHD_SIZE(Attr_c,0x9C);
struct BgcSrc_c { be<f32> m00,m04,m08,m0C; };
struct Bgc_c {
    be<f32> mGroundY[23]; be<s32> mMaxGroundIdx; be<f32> mWaterY;
    cXyz mWallPos[23]; be<s32> mWallIdx; be<f32> mNearestWallDist; be<u32> mStateFlags;
    void gnd_pos(const Act_c*,const BgcSrc_c*,s32,f32);
    void wrt_pos(const cXyz*);
    void wall_pos(const Act_c*,const BgcSrc_c*,s32,s16,f32);
    void proc_vertical(Act_c*);
    bool chk_wall_pre(const Act_c*,const BgcSrc_c*,s32,s16);
    bool chk_wall_touch(const Act_c*,const BgcSrc_c*,s16);
    bool chk_wall_touch2(const Act_c*,const BgcSrc_c*,s32,s16);
};
WWHD_SIZE(Bgc_c,0x184);
struct EffSmokeCB {
    u8 _00[0x20]; cXyz field_0x20; csXyz field_0x2C; u8 _32[2];
};
WWHD_SIZE(EffSmokeCB,0x34);
struct Act_c : dBgS_MoveBgActor {
    request_of_phase_process_class mPhs; Mtx34 mMtx; gptr<J3DModel> mpModel; be<s32> mMode;
    dCcD_Stts mStts; dCcD_Cyl mCyl; Bgc_c mBgc; be<s32> mType; be<u16> mPrmZ,mPrmX;
    be<u32> mpPath; be<s16> m604; u8 _71E[2];
    be<f32> m608,m60C,m610,m614,m618,m61C,m620,m624;
    be<s32> m628,m62C; be<f32> m630; be<s32> m634; be<u32> mPPLabel;
    be<s16> mMomentCnt[4],m644,m646,m648;
    be<u8> m64A; be<s8> mReverb; be<u8> mbShouldAppear,mbPrmZInitialized,mbPrmXInitialized,m64F;
    EffSmokeCB mSmokeCbs[2]; be<u32> mChildPID; be<s32> mbRollCrash;
    s32 prm(s32 width,s32 shift) const { return gabi::call<s32>(0x02333CD8,this,width,shift); }
    s32 prm_get_swSave1();
    u8 pathId() const { return mType==11 ? 255 : (mPrmZ&255); }
    u8 swSave2() const { return mType==11 ? 255 : (mPrmZ>>8); }
    const char* arc() const { return STR(gabi::load<u32>(0x1002D724+4*(s32)mType)); }
    const Attr_c* attr(u32 file,u32 msg=0) const {
        if((u32)mType>=13) JUT_ASSERT_fail(STR(file),0x5FF,STR(msg ? msg : file+0x14));
        return gabi::at<Attr_c>(0x101CB424+0x9C*(s32)mType);
    }
    void prmX_init(); void prmZ_init(); bool chk_appear(); void path_init(); void path_save();
    BOOL CreateHeap(); void clr_moment_cnt(); void set_mtx(); void init_mtx(); void mode_wait_init(); BOOL Create();
    static void RideCallBack(dBgW*,fopAc_ac_c*,fopAc_ac_c*);
    static fopAc_ac_c* PPCallBack(fopAc_ac_c*,fopAc_ac_c*,s16,u32);
    void make_item(); void eff_break(); void sound_break(); void mode_afl_init();
    void sound_land(); void vib_land(); void eff_land_smoke(); BOOL Execute(gptr<Mtx34>*); BOOL Draw();
    void eff_smoke_slip_remove(); BOOL Delete(); s32 check_to_walk(); void eff_set_slip_smoke_pos();
    void eff_smoke_slip_start(); void mode_walk_init(); void mode_wait(); void sound_slip(); void sound_limit();
    void eff_smoke_slip_end(); void mode_walk(); void afl_sway(); void mode_afl();
};
WWHD_OFFSET(Act_c,mMtx,0x3E8);
WWHD_OFFSET(Act_c,mBgc,0x58C);
WWHD_OFFSET(Act_c,mType,0x710);
WWHD_OFFSET(Act_c,mSmokeCbs,0x768);
WWHD_SIZE(Act_c,0x7D8);
}
