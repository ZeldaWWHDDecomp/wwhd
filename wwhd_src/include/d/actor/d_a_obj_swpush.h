#pragma once
#include "SSystem/SComponent/c_m3d_g_tri.h"
#include "bindings.h"
namespace swpushHD {
struct Background;
}
namespace daObjSwpush {
enum AttrFlag_e {
    FLAG_OBEY_SAVE = 0x1,
    FLAG_STAY_PRESSED = 0x2,
    FLAG_IS_TOGGLE = 0x4,
    FLAG_REQ_HEAVY = 0x8,
    FLAG_ON_IS_UP = 0x10,
    FLAG_UNK20 = 0x20,
};
struct Attr_c {

    /* 0x000 */ be<u32> mHeapSize;
    /* 0x004 */ be<u32> mFlags;
    /* 0x008 */ be<f32> mScale;
    /* 0x00C */ gptr<const char> mKbotaResName;
    /* 0x010 */ gptr<const char> mHhbotResName;
    /* 0x014 */ gptr<const char> mBgArcName;
    /* 0x018 */ gptr<const char> mModelArcName;
    /* 0x01C */ gptr<const char> mBtpArcName;
    /* 0x020 */ be<s16> mBgResIndex;
    /* 0x022 */ be<s16> mModelResIndices[2];
    /* 0x026 */ be<s16> mBtpResIndex;
    /* 0x028 */ be<f32> mSpring;
    /* 0x02C */ be<f32> mSpeedDecay;
    /* 0x030 */ be<f32> mPushSpeed0;
    /* 0x034 */ be<f32> m34;
    /* 0x038 */ be<s16> m38;
    /* 0x03A */ be<s16> mMiniPushDelay1;
    /* 0x03C */ be<s16> mPushDelay;
    /* 0x03E */ be<s16> mMiniPushDelay2;
    /* 0x040 */ be<f32> m40;
    /* 0x044 */ be<f32> m44;
    /* 0x048 */ be<s16> mPauseDuration;
    /* 0x04A */ u8 padding4A[2];
};
WWHD_SIZE(Attr_c, 0x4c);
// HD: The actor base and texture animation expand the GameCube layout.
struct Act_c : fopAc_ac_c {

    enum Mode_e { MODE_UPPER, MODE_U_L, MODE_LOWER, MODE_L_U };
    enum DemoMode_e {
        DEMO_MODE_NON,
        DEMO_MODE_REQ_PAUSE,
        DEMO_MODE_RUN_PAUSE,
        DEMO_MODE_REQ_SW,
        DEMO_MODE_RUN_SW
    };
    enum Prm_e {
        PRM_VERSION_W = 1,
        PRM_VERSION_S = 30,
        PRM_TYPE_W = 3,
        PRM_TYPE_S = 24,
        PRM_MDL_W = 1,
        PRM_MDL_S = 16,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 8,
        PRM_EVID_W = 8,
        PRM_EVID_S = 0,
    };
    /* 0x3AC */ request_of_phase_process_class mKbotaPhs;
    /* 0x3B4 */ request_of_phase_process_class mHhbotPhs;
    /* 0x3BC */ gptr<swpushHD::Background> mpBgW;
    /* 0x3C0 */ Mtx34 mMtx;
    /* 0x3F0 */ be<f32> m2D4;
    /* 0x3F4 */ gptr<J3DModel> mpModel;
    /* 0x3F8 */ mDoExt_btpAnm mBtpAnm;
    /* 0x46C */ be<s32> mType;
    /* 0x470 */ be<s32> mMode;
    /* 0x474 */ be<s32> mDemoMode;
    /* 0x478 */ be<s16> mPauseTimer;
    /* 0x47A */ be<s16> mEventID;
    /* 0x47C */ be<u16> mPrmZ;
    /* 0x47E */ be<u8> mPrmZInit;
    /* 0x47F */ be<u8> mVibTimer;
    /* 0x480 */ be<u8> mRidingMode;
    /* 0x481 */ be<u8> mPrevRiding;
    /* 0x482 */ be<s16> mMiniPushTimer;
    /* 0x484 */ be<u8> mMiniPushFlg;
    /* 0x485 */ be<u8> mHeavyRiding;
    /* 0x486 */ be<u8> mPrevHeavyRiding;
    /* 0x487 */ u8 _487;
    /* 0x488 */ be<s16> mPushTimer;
    /* 0x48A */ be<u8> mPushFlg;
    /* 0x48B */ be<u8> mChangingState;
    /* 0x48C */ be<f32> mTargetHFrac;
    /* 0x490 */ be<f32> mCurHFrac;
    /* 0x494 */ be<f32> mSpeed;
    /* 0x498 */ be<f32> m31C;
    /* 0x49C */ be<f32> m320;
    /* 0x4A0 */ be<s16> m324;
    /* 0x4A2 */ u8 _4a2[2];
    /* 0x4A4 */ be<f32> m328;
    /* 0x4A8 */ be<f32> m32C;
    /* 0x4AC */ be<f32> mTopPos;
    /* 0x4B0 */ be<s16> mDebounceTimer;
    /* 0x4B2 */ u8 _4b2[2];
    void prmZ_init();
    bool is_switch2();
    s32 create_res_load();
    bool create_heap();
    void set_mtx();
    void init_mtx();
    void set_btp_frame();
    void calc_top_pos();
    void set_push_flag();
    void top_bg_aim_req(f32, s16);
    void mode_upper_init();
    void mode_lower_init();
    void mode_u_l_init();
    void mode_l_u_init();
    void mode_upper();
    void mode_lower();
    void mode_u_l();
    void mode_l_u();
    void demo_non_init();
    void demo_non();
    void demo_reqPause_init();
    void demo_reqPause();
    void demo_runPause_init();
    void demo_runPause();
    void demo_stop_puase();
    void demo_reqSw_init();
    void demo_reqSw();
    void demo_runSw_init();
    void demo_runSw();
    s32 Mthd_Create();
    s32 Mthd_Delete();
    s32 Mthd_Execute();
    s32 Mthd_Draw();
    Attr_c& attr(const char* file, const char* msg);
};
WWHD_SIZE(Act_c, 0x4b4);
WWHD_OFFSET(Attr_c, mHeapSize, 0x0);
WWHD_OFFSET(Attr_c, mFlags, 0x4);
WWHD_OFFSET(Attr_c, mScale, 0x8);
WWHD_OFFSET(Attr_c, mKbotaResName, 0xC);
WWHD_OFFSET(Attr_c, mHhbotResName, 0x10);
WWHD_OFFSET(Attr_c, mBgArcName, 0x14);
WWHD_OFFSET(Attr_c, mModelArcName, 0x18);
WWHD_OFFSET(Attr_c, mBtpArcName, 0x1C);
WWHD_OFFSET(Attr_c, mBgResIndex, 0x20);
WWHD_OFFSET(Attr_c, mModelResIndices, 0x22);
WWHD_OFFSET(Attr_c, mBtpResIndex, 0x26);
WWHD_OFFSET(Attr_c, mSpring, 0x28);
WWHD_OFFSET(Attr_c, mSpeedDecay, 0x2C);
WWHD_OFFSET(Attr_c, mPushSpeed0, 0x30);
WWHD_OFFSET(Attr_c, m34, 0x34);
WWHD_OFFSET(Attr_c, m38, 0x38);
WWHD_OFFSET(Attr_c, mMiniPushDelay1, 0x3A);
WWHD_OFFSET(Attr_c, mPushDelay, 0x3C);
WWHD_OFFSET(Attr_c, mMiniPushDelay2, 0x3E);
WWHD_OFFSET(Attr_c, m40, 0x40);
WWHD_OFFSET(Attr_c, m44, 0x44);
WWHD_OFFSET(Attr_c, mPauseDuration, 0x48);
WWHD_OFFSET(Attr_c, padding4A, 0x4A);
WWHD_OFFSET(Act_c, mKbotaPhs, 0x3AC);
WWHD_OFFSET(Act_c, mHhbotPhs, 0x3B4);
WWHD_OFFSET(Act_c, mpBgW, 0x3BC);
WWHD_OFFSET(Act_c, mMtx, 0x3C0);
WWHD_OFFSET(Act_c, m2D4, 0x3F0);
WWHD_OFFSET(Act_c, mpModel, 0x3F4);
WWHD_OFFSET(Act_c, mBtpAnm, 0x3F8);
WWHD_OFFSET(Act_c, mType, 0x46C);
WWHD_OFFSET(Act_c, mMode, 0x470);
WWHD_OFFSET(Act_c, mDemoMode, 0x474);
WWHD_OFFSET(Act_c, mPauseTimer, 0x478);
WWHD_OFFSET(Act_c, mEventID, 0x47A);
WWHD_OFFSET(Act_c, mPrmZ, 0x47C);
WWHD_OFFSET(Act_c, mPrmZInit, 0x47E);
WWHD_OFFSET(Act_c, mVibTimer, 0x47F);
WWHD_OFFSET(Act_c, mRidingMode, 0x480);
WWHD_OFFSET(Act_c, mPrevRiding, 0x481);
WWHD_OFFSET(Act_c, mMiniPushTimer, 0x482);
WWHD_OFFSET(Act_c, mMiniPushFlg, 0x484);
WWHD_OFFSET(Act_c, mHeavyRiding, 0x485);
WWHD_OFFSET(Act_c, mPrevHeavyRiding, 0x486);
WWHD_OFFSET(Act_c, mPushTimer, 0x488);
WWHD_OFFSET(Act_c, mPushFlg, 0x48A);
WWHD_OFFSET(Act_c, mChangingState, 0x48B);
WWHD_OFFSET(Act_c, mTargetHFrac, 0x48C);
WWHD_OFFSET(Act_c, mCurHFrac, 0x490);
WWHD_OFFSET(Act_c, mSpeed, 0x494);
WWHD_OFFSET(Act_c, m31C, 0x498);
WWHD_OFFSET(Act_c, m320, 0x49C);
WWHD_OFFSET(Act_c, m324, 0x4A0);
WWHD_OFFSET(Act_c, m328, 0x4A4);
WWHD_OFFSET(Act_c, m32C, 0x4A8);
WWHD_OFFSET(Act_c, mTopPos, 0x4AC);
WWHD_OFFSET(Act_c, mDebounceTimer, 0x4B0);
} // namespace daObjSwpush

namespace swpushHD {
struct BackgroundData {
    be<s32> m_v_num;
};
struct Background {
    /* 0x00 */ u8 padding[8];
    /* 0x08 */ gptr<Mtx34> pm_base;
    /* 0x0C */ u8 padding2[0x60];
    /* 0x6C */ be<u8> m_flag;
    /* 0x6D */ u8 padding3[0x23];
    /* 0x90 */ gptr<cXyz> pm_vtx_tbl;
    /* 0x94 */ gptr<BackgroundData> pm_bgd;
    /* 0x98 */ u8 padding4[0x18];
    /* 0xB0 */ be<u32> mRideCallback;
    /* 0xB4 */ u8 padding5[0x10];
};
struct Joint {
    u8 padding[8];
    be<u32> mCallBack;
    u8 padding2[0x10];
};
struct JointData {
    u8 padding[4];
    be<u16> mJntNo;
};
struct ModelData {
    be<u32> mpResHeader;
    be<u32> mJointNum;
    gptr<Joint> mJointNodes;
};
struct MatrixBuffer {
    u8 padding[4];
    be<u16> mFlags;
    u8 padding2[0xA];
    gptr<Mtx34> mpAnmMtx;
};
struct PlayerStatus {
    u8 padding[0x3B8];
    be<u32> mPlayerStatus;
};
struct SaveBlock {
    u8 padding[0x20];
    dSv_info_c mSaveInfo;
};
struct RideVectors {
    cXyz point[4];
};
struct HeaderZeros {
    be<u32> words[4];
};
struct DestructorRecord {
    u8 data[0xC];
};
WWHD_OFFSET(Background, pm_base, 8);
WWHD_OFFSET(Background, m_flag, 0x6C);
WWHD_OFFSET(Background, pm_vtx_tbl, 0x90);
WWHD_OFFSET(Background, pm_bgd, 0x94);
WWHD_OFFSET(Background, mRideCallback, 0xB0);
WWHD_SIZE(Background, 0xC4);
WWHD_SIZE(Joint, 0x1C);
WWHD_OFFSET(Joint, mCallBack, 8);
WWHD_SIZE(JointData, 6);
WWHD_OFFSET(JointData, mJntNo, 4);
WWHD_SIZE(ModelData, 0xC);
WWHD_OFFSET(ModelData, mJointNodes, 8);
WWHD_SIZE(MatrixBuffer, 0x14);
WWHD_OFFSET(MatrixBuffer, mpAnmMtx, 0x10);
WWHD_OFFSET(PlayerStatus, mPlayerStatus, 0x3B8);
WWHD_OFFSET(SaveBlock, mSaveInfo, 0x20);
WWHD_SIZE(RideVectors, 0x30);
WWHD_SIZE(HeaderZeros, 0x10);
WWHD_SIZE(DestructorRecord, 0xC);
} // namespace swpushHD
using SwpushProc = void (daObjSwpush::Act_c::*)();
struct SwpushProcEntry {
    gabi::ptmf<SwpushProc> run;
};
WWHD_SIZE(SwpushProcEntry, 8);
