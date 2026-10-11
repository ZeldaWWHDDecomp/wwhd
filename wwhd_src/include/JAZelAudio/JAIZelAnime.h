/* Animation sound hooks. HD retains the JAIAnimeSound prefix and adds a wing-distance flag.
 * Accessed JAIZelBasic and library prefixes only. */
#pragma once
#include "bindings.h"
struct JAISound {
    u8 mUnaccessed[0x14];
    be<u32> field_0x18; // HD: the GC field is four bytes later.
    be<u32> field_0x1c;
};
WWHD_OFFSET(JAISound, field_0x18, 0x14);
WWHD_OFFSET(JAISound, field_0x1c, 0x18);
WWHD_SIZE(JAISound, 0x1C);
namespace JAInter {
struct Actor {
    be<u32> mUnaccessed;
    gptr<cXyz> field_0x4;
    be<u32> mUnaccessed8;
    be<u32> field_0xc;
};
} // namespace JAInter
WWHD_OFFSET(JAInter::Actor, field_0x4, 4);
WWHD_OFFSET(JAInter::Actor, field_0xc, 0xC);
WWHD_SIZE(JAInter::Actor, 0x10);
struct JAIZelCameraView {
    u8 mUnaccessed[8];
    gptr<void> field_0x8;
};
WWHD_OFFSET(JAIZelCameraView, field_0x8, 8);
WWHD_SIZE(JAIZelCameraView, 0xC);
struct JAIZelBasic {
    gptr<JAIZelCameraView> mCameras;
    u8 mUnaccessed4[0x38 - 4];
    be<u32> field_0x0028;
    u8 mUnaccessed3c[0x55 - 0x3C];
    be<u8> field_0x0045;
    be<u8> field_0x0046;
    u8 mUnaccessed57[0x271 - 0x57];
    be<u8> field_0x0201;
    u8 mUnaccessed272[4];
    be<u8> field_0x0206;
    be<u8> field_0x0207;
    u8 mUnaccessed278[0x294 - 0x278];
    be<s32> field_0x0224;
    u8 mUnaccessed298[0x2A8 - 0x298];
    be<u8> mIslandRoomNo;
    be<u8> field_0x0239;
};
WWHD_OFFSET(JAIZelBasic, mCameras, 0x0);
WWHD_OFFSET(JAIZelBasic, field_0x0028, 0x38);
WWHD_OFFSET(JAIZelBasic, field_0x0045, 0x55);
WWHD_OFFSET(JAIZelBasic, field_0x0046, 0x56);
WWHD_OFFSET(JAIZelBasic, field_0x0201, 0x271);
WWHD_OFFSET(JAIZelBasic, field_0x0206, 0x276);
WWHD_OFFSET(JAIZelBasic, field_0x0207, 0x277);
WWHD_OFFSET(JAIZelBasic, field_0x0224, 0x294);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2a8);
WWHD_OFFSET(JAIZelBasic, field_0x0239, 0x2a9);
WWHD_SIZE(JAIZelBasic, 0x2AA);
struct JAIAnimeFrameSoundData {
    be<u32> mSoundID;
    be<f32> mStartFrame, mEndFrame, mPitch;
    be<u32> mPlayFlags;
    be<u8> mVolume;
    be<s8> mPitchFactor;
    be<u8> mLoopCount, mPanning;
    be<s8> mVolumeFactor;
    u8 mPadding[7];
};
WWHD_OFFSET(JAIAnimeFrameSoundData, mSoundID, 0);
WWHD_OFFSET(JAIAnimeFrameSoundData, mStartFrame, 4);
WWHD_OFFSET(JAIAnimeFrameSoundData, mEndFrame, 8);
WWHD_OFFSET(JAIAnimeFrameSoundData, mPitch, 12);
WWHD_OFFSET(JAIAnimeFrameSoundData, mPlayFlags, 16);
WWHD_OFFSET(JAIAnimeFrameSoundData, mVolume, 20);
WWHD_OFFSET(JAIAnimeFrameSoundData, mPitchFactor, 21);
WWHD_OFFSET(JAIAnimeFrameSoundData, mVolumeFactor, 24);
WWHD_SIZE(JAIAnimeFrameSoundData, 0x20);
struct JAIAnimeSoundData {
    be<u16> datas;
    u8 mUnaccessed[6];
    JAIAnimeFrameSoundData mAfsData[1];
};
WWHD_OFFSET(JAIAnimeSoundData, datas, 0);
WWHD_OFFSET(JAIAnimeSoundData, mAfsData, 8);
WWHD_SIZE(JAIAnimeSoundData, 0x28);
class JAIZelAnime {
  public:
    u8 mUnaccessed[0x80];
    be<u32> dataCounter;
    be<u32> mLoopCount;
    be<f32> mCurrentTime;
    u8 mUnaccessed8c[4];
    gptr<JAIAnimeSoundData> mpAsData;
    be<u32> mVtable;
    be<u32> mUseExtendedWingDistance; // HD-only.
    void startAnimSound(JAIZelBasic*, u32, gptr<JAISound>*, JAInter::Actor*, u32);
    void setSpeedModifySound(JAISound*, JAIAnimeFrameSoundData*, f32);
    void setAnimSound(cXyz*, f32, f32, u32, s32);
    void setPlayPosition(f32);
    void initActorAnimSound(void*, u32, f32);
};
WWHD_OFFSET(JAIZelAnime, dataCounter, 0x80);
WWHD_OFFSET(JAIZelAnime, mCurrentTime, 0x88);
WWHD_OFFSET(JAIZelAnime, mpAsData, 0x90);
WWHD_OFFSET(JAIZelAnime, mUseExtendedWingDistance, 0x98);
WWHD_SIZE(JAIZelAnime, 0x9C);
u32 JAIZel_seStart(JAIZelBasic* basic, u32 id, cXyz* pos, u32 info, s32 reverb, u32 flag,
                   f32 volume, f32 pitch, f32 pan, f32 fx);
s32 JAIZel_checkStreamPlaying(JAIZelBasic* basic, u32 id);
void JAIAnime_initActorAnimSound(JAIZelAnime* self, void* data, u32 a, f32 f);
void JAIAnime_setAnimSoundVec(JAIZelAnime* self, JAIZelBasic* basic, cXyz* pos, u32 id, u32 flag,
                              f32 frame, f32 rate);
void JAIBasic_startSoundActor(JAIZelBasic* basic, u32 id, gptr<JAISound>* sound,
                              JAInter::Actor* actor, u32 a, u32 b);
f32 JAIGlobalParameter_getParamDistanceMax();
void JAISound_stop(JAISound* sound, u32 time);
void JAISound_setVolume(JAISound* sound, u32 time, u32 kind, f32 volume);
void JAISound_setPitch(JAISound* sound, u32 time, u32 kind, f32 pitch);
void JAISound_setPortData(JAISound* sound, u32 port, u32 value);
u32 JAISound_getID(JAISound* sound);
void JAIZel_multVec(void* matrix, cXyz* in, cXyz* out);
f32 JAIZel_sqrt(f32 value);
u32 JAIZel_checkPointer(cXyz* pos);
enum JAIZelAnime_soundId : u32 {
    JA_SE_LK_HOFUKU = 0x283e,
    JA_SE_LK_FT_WALK = 0x3800,
    JA_SE_LK_FT_JUMP = 0x3801,
    JA_SE_LK_HD_NORMAL = 0x3803,
    JA_SE_FT_SUSBRIDGE_C = 0x3812,
    JA_SE_FT_SUSBRIDGE_R = 0x3813,
    JA_SE_FT_LADDER_CLIMB = 0x3815,
    JA_SE_FT_LADDER_CLIMB_D = 0x3818,
    JA_SE_LK_WALK_HEAVY = 0x382f,
    JA_SE_LK_JUMP_HEAVY = 0x3830,
    JA_SE_LK_FT_SW_SHIELD = 0x3831,
    JA_SE_FT_PW_RUN_AWAY = 0x383c,
    JA_SE_CV_KAMOME = 0x4805,
    JA_SE_CV_DRG_MTOP_BIKU = 0x48d0,
    JA_SE_CV_DRG_MTOP_ABARE = 0x48d1,
    JA_SE_CV_DRG_MTOP_AKUBI = 0x48d2,
    JA_SE_CM_KAMOME_WING = 0x5800,
    JA_SE_CM_DK_WING_ENTER = 0x586c,
    JA_SE_CM_DK_WING_GO_UP = 0x586e,
    JA_SE_CM_DK_FOOTNOTE = 0x5870,
    JA_SE_CM_COMMON_WING_S = 0x588d,
    JA_SE_CM_DRG_MTOP_HAND = 0x58f3,
    JA_SE_CM_COMMON_WING_S_LPF = 0x59a3,
    JA_STRM_BOSS_CLEAR = 0xc0000003,
    JA_STRM_DEMO_04_01 = 0xc0000014,
    JA_STRM_BST_CLEAR = 0xc0000019,
    JA_STRM_DK_CLEAR = 0xc000001a,
    JA_STRM_BWD_CLEAR = 0xc000001b,
};
