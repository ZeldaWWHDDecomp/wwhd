/** JAIZelBasic scene/sea BGM work, part5.
 * HD: JAIBasic loses its leading vtable, ordinary game fields are GC+0x10, and
 * the expanded 32-entry SE arrays move scene fields to GC+0x70.
 * Numeric members retain the exact tww names; only fields used in this unit are exposed. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
namespace jaizel5 {
struct Sound : ::JAISound {
    void stop(u32);
    void setVolume(f32, u32, u32);
    void setTrackVolume(u32, f32, u32);
};
struct AudioCamera {
    gptr<cXyz> position;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos) */
};
struct isle_area_s {
    be<f32> centerX, field_0x4, centerZ, radius;  /* was field_0x0: checkOnOuterSea 02023670: x of the island area centre in its 100000-unit sea room (compared with the camera x; > 50000 means none); was field_0x8: checkOnOuterSea 02023670: z of the island area centre in its sea room (compared with the camera z); was field_0xC: checkOnOuterSea 02023670 / changeSeaBgm 02023A94: area radius (+2000 / 3000 / 4000 rings fade the sea BGM) */
};
struct scene_info_s {
    be<u16> bgmNum;
    be<u8> dyWave1st, dyWave2nd;  /* was field_0x2: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_1st index (mFirstDynamicSceneWaveIndex); was field_0x3: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_2nd index (mSecondDynamicSceneWaveIndex) */
};
// HD: indexing keeps the full u8 room number, without clamping to the sea's 49 rooms.
struct IsleAreas {
    isle_area_s entries[256];
};
struct IsleInfo {
    scene_info_s entries[256];
};
struct JAIZelBasic {
    s32 checkPlayingStreamBgmFlag(u32 id);
    u32 checkSeaBgmID();
    s32 checkDayTime();
    u32 checkPlayingMainBgmFlag();
    void sceneBgmStart();
    u32 expandSceneBgmNum(u32 n);
    void mainBgmStopOnly(u32 fade);
    s32 checkOnOuterSea(be<f32> *out);
    void startIsleBgm();
    void processMorningToNormal();
    void processTime();
    void changeSeaBgm();
    void bgmBattleGFrame();
    void mbossBgmMuteProcess();
    void enemyNearByGFrame();
    void stSkyCloistersProcess();
    void processHeartGaugeSound();
    s32 checkEventBit(u32);
    s32 isDemo();
    void stopBattleBgm();
    void bgmStart(u32, u32, u32);
    u32 seStart(u32, u32, u32, s32, f32, f32, f32, f32, u32);
    f32 calcMainBgmVolWithSeaFactor(f32);
    // HD: nullable camera objects still read guest address zero rather than host nullptr.
    u32 cameraPositionEA() {
        return gabi::load<u32>(gabi::ea(mAudioCamera.get()));
    }
    /* 0x000 */ gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x2D];
    /* 0x031 */ be<u8> mSceneBgmStartedOnce;  /* was field_0x0021: JAIZelBasic::mSceneBgmStartedOnce (JAZelAudio/JAIZelBasic.h) */
    u8 padding32[0xA];
    /* 0x03C */ be<u8> mHour;
    /* 0x03D */ be<u8> mMinute;
    u8 padding3E[0x2];
    /* 0x040 */ be<u16> mPrevTime;  /* was field_0x0030: JAIZelBasic::mPrevTime (JAZelAudio/JAIZelBasic.h) */
    /* 0x042 */ be<u8> mMorningBgmPlaying;  /* was field_0x0032: JAIZelBasic::mMorningBgmPlaying (JAZelAudio/JAIZelBasic.h) */
    /* 0x043 */ be<u8> mDayNightSwitched;  /* was field_0x0033: JAIZelBasic::mDayNightSwitched (JAZelAudio/JAIZelBasic.h) */
    /* 0x044 */ be<s32> mLinkHp;  /* was field_0x0034: JAIZelBasic::mLinkHp (JAZelAudio/JAIZelBasic.h) */
    u8 padding48[0x8];
    /* 0x050 */ be<u8> mHeartGaugeTimer;  /* was field_0x0040: JAIZelBasic::mHeartGaugeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding51[0x1F];
    /* 0x070 */ be<u8> mbLandingDemoStarted;
    u8 padding71[0x1];
    /* 0x072 */ be<u8> mKamomeFlag;  /* was field_0x0062: JAIZelBasic::mKamomeFlag (JAZelAudio/JAIZelBasic.h) */
    u8 padding73[0x5];
    /* 0x078 */ gptr<Sound> mpMainBgmSound;
    /* 0x07C */ gptr<Sound> mpSubBgmSound;
    u8 padding80[0x4];
    /* 0x084 */ be<u32> mSubBgmNum;
    /* 0x088 */ be<u32> mMainBgmNum;
    u8 padding8C[0x4];
    /* 0x090 */ be<f32> mMainBgmMuteVolume;  /* was field_0x0080: JAIZelBasic::mMainBgmMuteVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x094 */ be<f32> mMainBgmTalkVolume;  /* was field_0x0084: JAIZelBasic::mMainBgmTalkVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x098 */ be<f32> mMainBgmMenuVolume;  /* was field_0x0088: JAIZelBasic::mMainBgmMenuVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x09C */ be<f32> mMainBgmSubBgmVolume;  /* was field_0x008c: JAIZelBasic::mMainBgmSubBgmVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0A0 */ be<f32> mMainBgmSceneVolume;  /* was field_0x0090: JAIZelBasic::mMainBgmSceneVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0A4 */ be<f32> mMainBgmSeaVolume;  /* was field_0x0094: JAIZelBasic::mMainBgmSeaVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0A8 */ be<f32> mMainBgmTaktVolume;  /* was field_0x0098: JAIZelBasic::mMainBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    /* 0x0AC */ be<f32> mMainBgmCbPracticeVolume;  /* was field_0x009c: JAIZelBasic::mMainBgmCbPracticeVolume (JAZelAudio/JAIZelBasic.h) */
    u8 paddingB0[0xC];
    /* 0x0BC */ be<f32> mMainBgmHeightVolume;  /* was field_0x00ac: JAIZelBasic::mMainBgmHeightVolume (JAZelAudio/JAIZelBasic.h) */
    u8 paddingC0[0x7];
    /* 0x0C7 */ be<u8> mSkyCloistersTimer;  /* was field_0x00b7: JAIZelBasic::mSkyCloistersTimer (JAZelAudio/JAIZelBasic.h) */
    u8 paddingC8[0x5];
    /* 0x0CD */ be<u8> mSceneBgmLocked;  /* was field_0x00bd: JAIZelBasic::mSceneBgmLocked (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCE[0x2];
    /* 0x0D0 */ be<u8> mBattleBgmFadeDelay;  /* was field_0x00c0: JAIZelBasic::mBattleBgmFadeDelay (JAZelAudio/JAIZelBasic.h) */
    /* 0x0D1 */ be<u8> mEnemyNearbyOn;  /* was field_0x00c1: JAIZelBasic::mEnemyNearbyOn (JAZelAudio/JAIZelBasic.h) */
    u8 paddingD2[0x2];
    /* 0x0D4 */ be<u32> mEnemyNearbyFadeTimer;  /* was field_0x00c4: JAIZelBasic::mEnemyNearbyFadeTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x0D8 */ be<u8> mKaitengiriTimer;  /* was field_0x00c8: JAIZelBasic::mKaitengiriTimer (JAZelAudio/JAIZelBasic.h) */
    /* 0x0D9 */ be<u8> mSwordUsing;  /* was field_0x00c9: JAIZelBasic::mSwordUsing (JAZelAudio/JAIZelBasic.h) */
    /* 0x0DA */ be<u8> mBattleBgmDisabled;  /* was field_0x00ca: JAIZelBasic::mBattleBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    /* 0x0DB */ be<u8> mEnemyNearbyDelay;  /* was field_0x00cb: JAIZelBasic::mEnemyNearbyDelay (JAZelAudio/JAIZelBasic.h) */
    /* 0x0DC */ be<u8> mDkMaskBroken;  /* was field_0x00cc: JAIZelBasic::mDkMaskBroken (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDD[0x18B];
    /* 0x268 */ be<u8> mNoCameraTimer;  /* was field_0x01f8: JAIZelBasic::mNoCameraTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding269[0xC];
    /* 0x275 */ be<u8> mForceBack;  /* was field_0x0205: JAIZelBasic::mForceBack (JAZelAudio/JAIZelBasic.h) */
    /* 0x276 */ be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    u8 padding277[0x1D];
    /* 0x294 */ be<u32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    /* 0x298 */ be<u32> mNextSceneBgmId;
    u8 padding29C[0x1];
    /* 0x29D */ be<u8> mSceneActive;  /* was field_0x022d: JAIZelBasic::mSceneActive (JAZelAudio/JAIZelBasic.h) */
    /* 0x29E */ be<u8> mSceneBgmChanged;  /* was field_0x022e: JAIZelBasic::mSceneBgmChanged (JAZelAudio/JAIZelBasic.h) */
    u8 padding29F[0x9];
    /* 0x2A8 */ be<u8> mIslandRoomNo;
    u8 padding2A9[3];
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, mSceneBgmStartedOnce, 0x31);
WWHD_OFFSET(JAIZelBasic, mHour, 0x3C);
WWHD_OFFSET(JAIZelBasic, mMinute, 0x3D);
WWHD_OFFSET(JAIZelBasic, mPrevTime, 0x40);
WWHD_OFFSET(JAIZelBasic, mMorningBgmPlaying, 0x42);
WWHD_OFFSET(JAIZelBasic, mDayNightSwitched, 0x43);
WWHD_OFFSET(JAIZelBasic, mLinkHp, 0x44);
WWHD_OFFSET(JAIZelBasic, mHeartGaugeTimer, 0x50);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, mKamomeFlag, 0x72);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mpSubBgmSound, 0x7C);
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
WWHD_OFFSET(JAIZelBasic, mMainBgmHeightVolume, 0xBC);
WWHD_OFFSET(JAIZelBasic, mSkyCloistersTimer, 0xC7);
WWHD_OFFSET(JAIZelBasic, mSceneBgmLocked, 0xCD);
WWHD_OFFSET(JAIZelBasic, mBattleBgmFadeDelay, 0xD0);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyOn, 0xD1);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyFadeTimer, 0xD4);
WWHD_OFFSET(JAIZelBasic, mKaitengiriTimer, 0xD8);
WWHD_OFFSET(JAIZelBasic, mSwordUsing, 0xD9);
WWHD_OFFSET(JAIZelBasic, mBattleBgmDisabled, 0xDA);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyDelay, 0xDB);
WWHD_OFFSET(JAIZelBasic, mDkMaskBroken, 0xDC);
WWHD_OFFSET(JAIZelBasic, mNoCameraTimer, 0x268);
WWHD_OFFSET(JAIZelBasic, mForceBack, 0x275);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mNextSceneBgmId, 0x298);
WWHD_OFFSET(JAIZelBasic, mSceneActive, 0x29D);
WWHD_OFFSET(JAIZelBasic, mSceneBgmChanged, 0x29E);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_SIZE(JAIZelBasic, 0x2AC);
WWHD_SIZE(AudioCamera, 4);
WWHD_SIZE(isle_area_s, 16);
WWHD_SIZE(scene_info_s, 4);
// HD: preserve reads even when the position pointer is zero.
inline f32 cameraX(u32 position) {
    return gabi::load<f32>(position);
}
inline f32 cameraZ(u32 position) {
    return gabi::load<f32>(position + offsetof(cXyz, z));
}
struct SaveOptionsView {
    u8 padding[0x12C0];
    u8 mOptions;
};
inline void *optionsAddress(SaveOptionsView *save) {
    return gabi::at<void>(gabi::ea(save) + offsetof(SaveOptionsView, mOptions));
}
} // namespace jaizel5
