/** JAIZelBasic per-frame audio work, part6.
 * HD: early members are GC+0x10; expanded SE arrays move later members to GC+0x70.
 * The GameCube body is nonmatching; numeric fields below retain its header's exact names. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
namespace jaizel6 {
struct Sound : ::JAISound {
    void setVolume(f32, u32, u32);
};
struct AudioCamera {
    gptr<cXyz> position;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos) */
};
struct scene_info_s {
    be<u16> bgmNum;
    be<u8> dyWave1st, dyWave2nd;  /* was field_0x2: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_1st index (mFirstDynamicSceneWaveIndex); was field_0x3: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_2nd index (mSecondDynamicSceneWaveIndex) */
};
// HD: room indexing retains the full u8 register, without clamping to the 49 sea rooms.
struct IsleInfo {
    scene_info_s entries[256];
};
struct JAIZelBasic {
    void zeldaGFrameWork();
    void sceneSE(u32, f32);
    f32 calcMainBgmVol();
    void processLevObjSE();
    void cbPracticeProcess();
    void processDemoFanfareMute();
    s32 checkBgmPlaying();
    void bgmStop(u32, u32);
    void bgmStart(u32, u32, u32);
    s32 checkSubBgmPlaying();
    void subBgmStart(u32);
    void subBgmStopInner();
    s32 isDemo();
    s32 checkSeqIDDemoPlaying(u32);
    s32 checkPlayingStreamBgmFlag(u32);
    s32 checkEventBit(u32);
    u32 checkSeaBgmID();
    void sceneBgmStart();
    u32 expandSceneBgmNum(u32);
    void processHeartGaugeSound();
    void processTime();
    void changeSeaBgm();
    void bgmBattleGFrame();
    void mbossBgmMuteProcess();
    void enemyNearByGFrame();
    void stSkyCloistersProcess();
    s32 checkStreamPlaying(u32);
    void startSoundVec(u32, gptr<Sound> *, u32, u32, u32, u32);
    u32 seStart(u32, u32, u32, s32, f32, f32, f32, f32, u32);
    // HD: a nullable camera object still reads guest address zero.
    u32 cameraPositionEA() {
        return gabi::load<u32>(gabi::ea(mAudioCamera.get()));
    }
    /* 0x0000 */ gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x34];
    /* 0x0038 */ be<u32> mSceneFrameCount;  /* was field_0x0028: JAIZelBasic::mSceneFrameCount (JAZelAudio/JAIZelBasic.h) */
    u8 padding3C[0x34];
    /* 0x0070 */ be<u8> mbLandingDemoStarted;
    /* 0x0071 */ be<u8> mEventTimer;  /* was field_0x0061: JAIZelBasic::mEventTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding72[0x3];
    /* 0x0075 */ be<u8> mMtDragonMuteRequest;  /* was field_0x0065: JAIZelBasic::mMtDragonMuteRequest (JAZelAudio/JAIZelBasic.h) */
    u8 padding76[0x2];
    /* 0x0078 */ gptr<Sound> mpMainBgmSound;
    u8 padding7C[0x8];
    /* 0x0084 */ be<u32> mSubBgmNum;
    /* 0x0088 */ be<u32> mMainBgmNum;
    u8 padding8C[0x4];
    /* 0x0090 */ be<f32> mMainBgmMuteVolume;  /* was field_0x0080: JAIZelBasic::mMainBgmMuteVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0094 */ be<f32> mMainBgmTalkVolume;  /* was field_0x0084: JAIZelBasic::mMainBgmTalkVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0098 */ be<f32> mMainBgmMenuVolume;  /* was field_0x0088: JAIZelBasic::mMainBgmMenuVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x009C */ be<f32> mMainBgmSubBgmVolume;  /* was field_0x008c: JAIZelBasic::mMainBgmSubBgmVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x00A0 */ be<f32> mMainBgmSceneVolume;  /* was field_0x0090: JAIZelBasic::mMainBgmSceneVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x00A4 */ be<f32> mMainBgmSeaVolume;  /* was field_0x0094: JAIZelBasic::mMainBgmSeaVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x00A8 */ be<f32> mMainBgmTaktVolume;  /* was field_0x0098: JAIZelBasic::mMainBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x00AC */ be<f32> mMainBgmCbPracticeVolume;  /* was field_0x009c: JAIZelBasic::mMainBgmCbPracticeVolume (JAZelAudio/JAIZelBasic.h) */
    u8 paddingB0[0x4];
    /* 0x00B4 */ be<f32> mSubBgmTaktVolume;  /* was field_0x00a4: JAIZelBasic::mSubBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    u8 paddingB8[0x4];
    /* 0x00BC */ be<f32> mMainBgmHeightVolume;  /* was field_0x00ac: JAIZelBasic::mMainBgmHeightVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x00C0 */ be<s32> mSubBgmStopTimer;  /* was field_0x00b0: JAIZelBasic::mSubBgmStopTimer (JAZelAudio/JAIZelBasic.h) */
    u8 paddingC4[0x2];
    /* 0x00C6 */ be<u8> mLastMinibossSubBGMType;
    u8 paddingC7[0x4];
    /* 0x00CB */ be<u8> mFanfareResumePending;  /* was field_0x00bb: JAIZelBasic::mFanfareResumePending (JAZelAudio/JAIZelBasic.h) */
    /* 0x00CC */ be<u8> mPendingBgmStart;  /* was field_0x00bc: JAIZelBasic::mPendingBgmStart (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCD[0x11];
    /* 0x00DE */ be<u8> mBattleBgmHoldTimer;  /* was field_0x00ce: JAIZelBasic::mBattleBgmHoldTimer (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDF[0x18A];
    /* 0x0269 */ be<u8> mAddGrassSePlayed;  /* was field_0x01f9: JAIZelBasic::mAddGrassSePlayed (JAZelAudio/JAIZelBasic.h) */
    /* 0x026A */ be<u8> mCutGrassSeTimer;  /* was field_0x01fa: JAIZelBasic::mCutGrassSeTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x026B */ be<u8> mBrkWrailKSePlayed;  /* was field_0x01fb: JAIZelBasic::mBrkWrailKSePlayed (JAZelAudio/JAIZelBasic.h) */
    /* 0x026C */ be<u8> mColBrkWrailingSePlayed;  /* was field_0x01fc: JAIZelBasic::mColBrkWrailingSePlayed (JAZelAudio/JAIZelBasic.h) */
    /* 0x026D */ be<u8> mBgnStringRippleSeTimer;  /* was field_0x01fd: JAIZelBasic::mBgnStringRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x026E */ be<u8> mBgnBodyRippleSeTimer;  /* was field_0x01fe: JAIZelBasic::mBgnBodyRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x026F */ be<u8> mBgnHeadRippleSeTimer;  /* was field_0x01ff: JAIZelBasic::mBgnHeadRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x0270 */ be<u8> mBgnTailRippleSeTimer;  /* was field_0x0200: JAIZelBasic::mBgnTailRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding271[0x1];
    /* 0x0272 */ be<u8> mSwingSeTimers[2];  /* was field_0x0202: JAIZelBasic::mSwingSeTimers (JAZelAudio/JAIZelBasic.h) */
    u8 padding274[0x1];
    /* 0x0275 */ be<u8> mForceBack;  /* was field_0x0205: JAIZelBasic::mForceBack (JAZelAudio/JAIZelBasic.h) */
    /* 0x0276 */ be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    /* 0x0277 */ be<u8> mTaktSeTimer;  /* was field_0x0207: JAIZelBasic::mTaktSeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding278[0x16];
    /* 0x028E */ be<u8> field_0x021e;
    u8 padding28F[0x5];
    /* 0x0294 */ be<s32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    u8 padding298[0x5];
    /* 0x029D */ be<u8> mSceneActive;  /* was field_0x022d: JAIZelBasic::mSceneActive (JAZelAudio/JAIZelBasic.h) */
    u8 padding29E[0xA];
    /* 0x02A8 */ be<u8> mIslandRoomNo;
    /* 0x02A9 */ be<u8> mLayerNo;  /* was field_0x0239: JAIZelBasic::mLayerNo (JAZelAudio/JAIZelBasic.h) */
    u8 padding2AA[0x1C5E];
    /* 0x1F08 */ be<u32> mWaterfallPosCount;  /* was field_0x1e98: JAIZelBasic::mWaterfallPosCount (JAZelAudio/JAIZelBasic.h) */
    u8 padding1F0C[0x98];
    /* 0x1FA4 */ be<u32> mFireblastPosCount;  /* was field_0x1f34: JAIZelBasic::mFireblastPosCount (JAZelAudio/JAIZelBasic.h) */
    /* 0x1FA8 */ gptr<Sound> mpWindSound;  /* was field_0x1f38: JAIZelBasic::mpWindSound (JAZelAudio/JAIZelBasic.h) */
    u8 padding1FAC[0x100];
    /* 0x20AC */ be<u8> mKuroboMotionCount;  /* was field_0x203c: JAIZelBasic::mKuroboMotionCount (JAZelAudio/JAIZelBasic.h) */
    /* 0x20AD */ be<u8> mKuroboVoiceCount;  /* was field_0x203d: JAIZelBasic::mKuroboVoiceCount (JAZelAudio/JAIZelBasic.h) */
    u8 padding20AE[0x22];
    /* 0x20D0 */ gptr<Sound> mpCharVoiceSound;  /* was field_0x2060: JAIZelBasic::mpCharVoiceSound (JAZelAudio/JAIZelBasic.h) */
    /* 0x20D4 */ be<u16> mResetIndexHD;
    u8 padding20D6[2];
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, mSceneFrameCount, 0x38);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, mEventTimer, 0x71);
WWHD_OFFSET(JAIZelBasic, mMtDragonMuteRequest, 0x75);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mSubBgmNum, 0x84);
WWHD_OFFSET(JAIZelBasic, mMainBgmNum, 0x88);
WWHD_OFFSET(JAIZelBasic, mMainBgmMuteVolume, 0x90);
WWHD_OFFSET(JAIZelBasic, mMainBgmTalkVolume, 0x94);
WWHD_OFFSET(JAIZelBasic, mMainBgmMenuVolume, 0x98);
WWHD_OFFSET(JAIZelBasic, mMainBgmSubBgmVolume, 0x9C);
WWHD_OFFSET(JAIZelBasic, mMainBgmSceneVolume, 0xA0);
WWHD_OFFSET(JAIZelBasic, mMainBgmSeaVolume, 0xA4);
WWHD_OFFSET(JAIZelBasic, mMainBgmTaktVolume, 0xA8);
WWHD_OFFSET(JAIZelBasic, mMainBgmCbPracticeVolume, 0xAC);
WWHD_OFFSET(JAIZelBasic, mSubBgmTaktVolume, 0xB4);
WWHD_OFFSET(JAIZelBasic, mMainBgmHeightVolume, 0xBC);
WWHD_OFFSET(JAIZelBasic, mSubBgmStopTimer, 0xC0);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, mFanfareResumePending, 0xCB);
WWHD_OFFSET(JAIZelBasic, mPendingBgmStart, 0xCC);
WWHD_OFFSET(JAIZelBasic, mBattleBgmHoldTimer, 0xDE);
WWHD_OFFSET(JAIZelBasic, mAddGrassSePlayed, 0x269);
WWHD_OFFSET(JAIZelBasic, mCutGrassSeTimer, 0x26A);
WWHD_OFFSET(JAIZelBasic, mBrkWrailKSePlayed, 0x26B);
WWHD_OFFSET(JAIZelBasic, mColBrkWrailingSePlayed, 0x26C);
WWHD_OFFSET(JAIZelBasic, mBgnStringRippleSeTimer, 0x26D);
WWHD_OFFSET(JAIZelBasic, mBgnBodyRippleSeTimer, 0x26E);
WWHD_OFFSET(JAIZelBasic, mBgnHeadRippleSeTimer, 0x26F);
WWHD_OFFSET(JAIZelBasic, mBgnTailRippleSeTimer, 0x270);
WWHD_OFFSET(JAIZelBasic, mSwingSeTimers, 0x272);
WWHD_OFFSET(JAIZelBasic, mForceBack, 0x275);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mTaktSeTimer, 0x277);
WWHD_OFFSET(JAIZelBasic, field_0x021e, 0x28E);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mSceneActive, 0x29D);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_OFFSET(JAIZelBasic, mLayerNo, 0x2A9);
WWHD_OFFSET(JAIZelBasic, mWaterfallPosCount, 0x1F08);
WWHD_OFFSET(JAIZelBasic, mFireblastPosCount, 0x1FA4);
WWHD_OFFSET(JAIZelBasic, mpWindSound, 0x1FA8);
WWHD_OFFSET(JAIZelBasic, mKuroboMotionCount, 0x20AC);
WWHD_OFFSET(JAIZelBasic, mKuroboVoiceCount, 0x20AD);
WWHD_OFFSET(JAIZelBasic, mpCharVoiceSound, 0x20D0);
WWHD_OFFSET(JAIZelBasic, mResetIndexHD, 0x20D4);
WWHD_SIZE(JAIZelBasic, 0x20D8);
WWHD_SIZE(AudioCamera, 4);
WWHD_SIZE(scene_info_s, 4);
// HD: terminal reset touches only the first halfword of GameCube field_0x2064.
// Camera position reads below preserve guest-zero reads when its pointer is NULL.
inline f32 cameraX(u32 p) {
    return gabi::load<f32>(p);
}
inline f32 cameraY(u32 p) {
    return gabi::load<f32>(p + offsetof(cXyz, y));
}
inline f32 cameraZ(u32 p) {
    return gabi::load<f32>(p + offsetof(cXyz, z));
}
} // namespace jaizel6
