// Link neck, hat and leg pose support. Private views preserve the WWHD layout.
// The shared Link declaration remains common to the independently verified ranges.
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct LinkPoseApi {
    static fopAc_ac_c* fopAcM_getTalkEventPartner(daPy_lk_c*);
    static BOOL startCheckOld(void*, const char*);
    static void* getCameraBody();
    static fopAc_ac_c* getForceLockOnActor(void*);
    static void* getLockonList(void*, int);
    static fopAc_ac_c* getLockonActor(void*);
    static BOOL checkShipNotNormalMode(daPy_lk_c*);
    static BOOL checkHeavyStateOn(daPy_lk_c*);
    static fopAc_ac_c* hintActor(void*, u32);
    static BOOL checkAttentionPosAngle(daPy_lk_c*, fopAc_ac_c*, be<u32>*);
    static BOOL detectAttention(void*, cXyz*);
    static fopAc_ac_c* getDemoLookActor(daPy_lk_c*);
    static f32 addCalc(be<f32>*, f32, f32, f32, f32);

    static BOOL ChkTgHit(dCcD_Cyl*);
    static void dKyw_get_AllWind_vec(const cXyz*, cXyz*, be<f32>*);
    static f32 PSVECSquareDistance(const cXyz*, const cXyz*);
    static void mDoMtx_ZrotS(Mtx34*, s32);
    static void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
    static BOOL ChkMoveBG(dBgS*, cBgS_PolyInfo*);
    static fopAc_ac_c* GetActorPointer(dBgS*, u32);
    static f64 RoofChk(dBgS*, dBgS_RoofChk*);
    static BOOL startRestartRoom(daPy_lk_c*, u32, int, f32, int);
};

// HD: the shared declaration stores these animation and collision blocks opaquely.
struct LinkPoseAnmHeap {
    /* 0x00 */ be<u16> mIdx;
    /* 0x02 */ be<u16> field_0x2;
    /* 0x04 */ be<u16> field_0x4;
    /* 0x06 */ be<u16> field_0x6;
    /* 0x08 */ gptr<void> m_buffer;
    /* 0x0C */ gptr<JKRSolidHeap> mpAnimeHeap;
};
WWHD_SIZE(LinkPoseAnmHeap, 0x10);
WWHD_OFFSET(LinkPoseAnmHeap, mIdx, 0);
struct LinkPoseTransformInfo {
    /* 0x00 */ cXyz mScale;
    /* 0x0C */ csXyz mRotation;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ cXyz mTranslate;
};
WWHD_SIZE(LinkPoseTransformInfo, 0x20);
WWHD_OFFSET(LinkPoseTransformInfo, mTranslate, 0x14);
struct LinkPoseOldFrame {
    /* 0x00 */ be<u8> mOldFrameFlg;
    /* 0x01 */ be<u8> field_0x1;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> mOldFrameMorfCounter;
    /* 0x08 */ be<f32> field_0x8;
    /* 0x0C */ be<f32> mOldFrameRate;
    /* 0x10 */ be<f32> field_0x10;
    /* 0x14 */ be<f32> field_0x14;
    /* 0x18 */ be<u16> mOldFrameStartJoint;
    /* 0x1A */ be<u16> mOldFrameEndJoint;
    /* 0x1C */ gptr<LinkPoseTransformInfo> mOldFrameTransInfo;
    /* 0x20 */ gptr<Quaternion> mOldFrameQuaternion;
};
WWHD_SIZE(LinkPoseOldFrame, 0x24);
WWHD_OFFSET(LinkPoseOldFrame, mOldFrameTransInfo, 0x1C);
WWHD_OFFSET(LinkPoseOldFrame, mOldFrameQuaternion, 0x20);
struct LinkPoseFootData {
    /* 0x000 */ be<u8> field_0x000;
    /* 0x001 */ be<u8> field_0x001;
    /* 0x002 */ be<s16> field_0x002;
    /* 0x004 */ be<s16> field_0x004;
    /* 0x006 */ be<s16> field_0x006;
    /* 0x008 */ be<s16> field_0x008;
    /* 0x00A */ be<s16> field_0x00A;
    /* 0x00C */ cXyz field_0x00C;
    /* 0x018 */ cXyz field_0x018;
    /* 0x024 */ cXyz field_0x024;
    /* 0x030 */ be<f32> field_0x030;
    /* 0x034 */ u8 field_0x034[0x54];
    /* 0x088 */ Mtx34 field_0x088[3];
};
WWHD_SIZE(LinkPoseFootData, 0x118);
WWHD_OFFSET(LinkPoseFootData, field_0x008, 8);
WWHD_OFFSET(LinkPoseFootData, field_0x00A, 0xA);
struct LinkPoseMatAnm {
    /* 0x00 */ u8 _00[0x74];
    /* 0x74 */ be<f32> mNowOffsetX;
    /* 0x78 */ be<f32> mNowOffsetY;
};
WWHD_SIZE(LinkPoseMatAnm, 0x7C);
WWHD_OFFSET(LinkPoseMatAnm, mNowOffsetX, 0x74);
WWHD_OFFSET(LinkPoseMatAnm, mNowOffsetY, 0x78);
struct LinkPoseLayout {
    /* 0x0000 */ u8 _0000[0x604];
    /* 0x0604 */ gptr<LinkPoseMatAnm> m_tex_eye_scroll[2];
    /* 0x060C */ u8 _060C[0xB68 - 0x60C];
    /* 0x0B68 */ dBgS_RoofChk mRoofChk;
    /* 0x0BB4 */ u8 _0BB4[0x5848 - 0xBB4];
    /* 0x5848 */ LinkPoseAnmHeap m_anm_heap_under[2];
    /* 0x5868 */ LinkPoseAnmHeap m_anm_heap_upper[3];
    /* 0x5898 */ u8 _5898[0x65CC - 0x5898];
    /* 0x65CC */ gptr<LinkPoseOldFrame> m_old_fdata;
    /* 0x65D0 */ LinkPoseAnmHeap m_tex_anm_heap;
    /* 0x65E0 */ u8 _65E0[0x73F0 - 0x65E0];
    /* 0x73F0 */ LinkPoseFootData mFootData[2];
};
WWHD_SIZE(LinkPoseLayout, 0x7620);
WWHD_OFFSET(LinkPoseLayout, mRoofChk, 0xB68);
WWHD_OFFSET(LinkPoseLayout, m_anm_heap_upper, 0x5868);
WWHD_OFFSET(LinkPoseLayout, m_old_fdata, 0x65CC);
WWHD_OFFSET(LinkPoseLayout, m_tex_anm_heap, 0x65D0);
WWHD_OFFSET(LinkPoseLayout, mFootData, 0x73F0);
inline LinkPoseLayout& LinkPose(daPy_lk_c* player) {
    return gabi::object_at<LinkPoseLayout>(gabi::ea(player));
}
struct LinkPoseStageInfo {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> mStageTypeAndSchbit;
};
WWHD_SIZE(LinkPoseStageInfo, 0x10);
WWHD_OFFSET(LinkPoseStageInfo, mStageTypeAndSchbit, 0xC);
struct LinkPoseCamera {
    /* 0x000 */ u8 _000[0x264];
    /* 0x264 */ cXyz mEye;
    /* 0x270 */ u8 _270[0x7C0 - 0x270];
    // HD: selfie look-target offset.
    /* 0x7C0 */ cXyz mSelfieOffset;
};
WWHD_SIZE(LinkPoseCamera, 0x7CC);
WWHD_OFFSET(LinkPoseCamera, mEye, 0x264);
WWHD_OFFSET(LinkPoseCamera, mSelfieOffset, 0x7C0);
struct LinkPoseShip {
    /* 0x000 */ u8 _000[0x71C];
    /* 0x71C */ gptr<cXyz> mpCraneTop;
};
WWHD_SIZE(LinkPoseShip, 0x720);
WWHD_OFFSET(LinkPoseShip, mpCraneTop, 0x71C);
struct LinkPoseConstants {
    static constexpr u32 dStageType_BOSS_e = 3;
    static constexpr u32 CL_JNT_LLEGA_JNT_e = 0x20;
    static constexpr u32 CL_JNT_LLEGB_JNT_e = 0x21;
    static constexpr u32 CL_JNT_LFOOT_JNT_e = 0x22;
    static constexpr u32 CL_JNT_LTOE_JNT_e = 0x23;
    static constexpr u32 CL_JNT_RLEGA_JNT_e = 0x25;
    static constexpr u32 CL_JNT_RLEGB_JNT_e = 0x26;
    static constexpr u32 CL_JNT_RFOOT_JNT_e = 0x27;
    static constexpr u32 CL_JNT_RTOE_JNT_e = 0x28;
    static constexpr u32 CL_JNT_HEAD_JNT_e = 0xF;
    static constexpr u32 CL_JNT_HATA_JNT_e = 0x1A;
    static constexpr u32 CL_JNT_CENTER_e = 0x1;
    static constexpr u32 CL_JNT_CL_LHANDA_e = 0x8;
    static constexpr u32 dRes_INDEX_LKANM_BCK_DAMDASH_e = 0x64;
    static constexpr u32 dRes_INDEX_LKANM_BCK_DASHKAZE_e = 0x73;
    static constexpr u32 dRes_INDEX_LKANM_BTP_TDASHKAZE_e = 0x208;
    static constexpr u32 daPyStts0_UNK10_e = 0x10;
    static constexpr u32 daPyStts0_SHIP_RIDE_e = 0x10000;
    static constexpr u32 daPyStts1_WIND_WAKER_CONDUCT_e = 0x1;
    static constexpr u32 JA_SE_LK_HAT_SWING = 0x2056;

    static constexpr s16 fpcNm_BRIDGE_e = 0x59;
    static constexpr u32 daPyProc_CALL_e = 0x02;
    static constexpr u32 daPyProc_WAIT_e = 0x04;
    static constexpr u32 daPyProc_CRAWL_START_e = 0x0F;
    static constexpr u32 daPyProc_CRAWL_END_e = 0x12;
    static constexpr u32 daPyProc_WHIDE_READY_e = 0x13;
    static constexpr u32 daPyProc_LADDER_MOVE_e = 0x3C;
    static constexpr u32 daPyProc_CLIMB_MOVE_UP_DOWN_e = 0x3F;
    static constexpr u32 daPyProc_CLIMB_MOVE_SIDE_e = 0x40;
    static constexpr u32 daPyProc_DAMAGE_e = 0x66;
    static constexpr u32 daPyProc_ELEC_DAMAGE_e = 0x6C;
    static constexpr u32 daPyProc_GRAB_WAIT_e = 0x73;
    static constexpr u32 daPyProc_GRAB_HEAVY_WAIT_e = 0x74;
    static constexpr u32 daPyProc_SHIP_CRANE_e = 0x8F;
    static constexpr u32 daPyProc_FAN_GLIDE_e = 0x93;
    static constexpr u32 daPyProc_VOMIT_WAIT_e = 0x97;
    static constexpr u32 daPyProc_ICE_SLIP_ALMOST_FALL_e = 0xA0;
    static constexpr u32 daPyProc_BOTTLE_OPEN_e = 0xA4;
    static constexpr u32 daPyProc_DEMO_LOOK_WAIT_e = 0xBE;
    static constexpr u32 daPyProc_DEMO_DOOR_OPEN_e = 0xC1;
    static constexpr u32 ModeFlg_WHIDE = 0x00000010;
    static constexpr u32 ModeFlg_00000080 = 0x00000080;
    static constexpr u32 ModeFlg_SWIM = 0x00040000;
    static constexpr u32 ModeFlg_CRAWL = 0x01000000;
    static constexpr u32 ModeFlg_08000000 = 0x08000000;
};

struct LinkPoseLook {
    /* 0x00 */ be<u32> mRequestActorID;
    /* 0x04 */ be<s32> field_0x4;
    /* 0x08 */ be<f32> mDistance;
    /* 0x0C */ be<u32> mLookTargetID;
};
WWHD_SIZE(LinkPoseLook, 0x10);
WWHD_OFFSET(LinkPoseLook, mLookTargetID, 0xC);
struct LinkPoseAttention {
    /* 0x000 */ u8 _000[0x148];
    /* 0x148 */ LinkPoseLook mLook[2];
};
WWHD_SIZE(LinkPoseAttention, 0x168);
WWHD_OFFSET(LinkPoseAttention, mLook, 0x148);
inline fopAc_ac_c* LinkPose_getLookTarget() {
    LinkPoseAttention* att = reinterpret_cast<LinkPoseAttention*>(dComIfGp_getAttention());
    return LinkPoseApi::hintActor(&att->mLook[0], att->mLook[0].mLookTargetID);
}

inline dDetect_c* LinkPose_getDetect() {
    return gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT);
}
