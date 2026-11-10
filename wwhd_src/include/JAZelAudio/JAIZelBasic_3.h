/** JAIZelBasic parts 3/4.
 * HD: ordinary fields are GC+0x10, expanded SE arrays move later fields to GC+0x70.
 * Numeric names retain the GameCube names. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
namespace jaizel34 {
struct SoundHeader {
    be<u8> track;  /* was field_0x4: JAISound::track (JSystem/JAudio/JAISound.h; byte 0 of the sound handle, GC +4) */
};
struct Sound : ::JAISound {
    u8 sequenceKind() {
        return reinterpret_cast<SoundHeader *>(this)->track;
    }
    void stop(u32);
    void setVolume(f32, u32, u32);
    void setPan(f32, u32, u32);
    void setDolby(f32, u32, u32);
    void setTempoProportion(f32, u32);
};
struct VectorWords {
    be<u32> w[3];
};
struct AudioCamera {
    gptr<VectorWords> position;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos) */
    gptr<VectorWords> previousPosition;  /* was field_0x4: JAInter::Camera::previousPosition (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 points it at mCameraOldPos */
    gptr<void> matrix;  /* was field_0x8: JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera matrix */
};
struct SeqUpdateData {
    u8 padding[0x48];
    gptr<Sound> mSequence;
};
struct LevelObjectInstance {
    be<f32> volume, pan, surround;
    be<s8> reverb;
    u8 padding[3];
};
struct LevelObjectSound {
    be<u32> soundID, instanceCount;
    LevelObjectInstance instances[20];
};
struct SafeString;
struct SafeStringVtable {
    u8 padding[0x14];
    gabi::gfn assureTermination;
};
struct SafeString {
    gptr<be<u8>> mTextPtr;
    gptr<SafeStringVtable> __vtbl;
    void assureTermination();
};
using ManagerPtr = gptr<void>;
using GuestByte = be<u8>;
extern ManagerPtr &l_soundManager;
using GuestFloat = be<f32>;
extern GuestFloat &l_towerTempo;
struct JAIZelBasic {
    void initLevObjSE();
    void processLevObjSE();
    s32 checkSubBgmPlaying();
    u32 checkPlayingSubBgmFlag();
    s32 checkCbPracticePlay();
    void cbPracticeProcess();
    s32 checkSeqIDDemoPlaying(u32 id);
    u32 checkDemoFanfarePlaying();
    void hdStreamVolume(u32 fade, f32 v);
    void muteMainBgmAll();
    void unmuteMainBgmAll();
    void processDemoFanfareMute();
    s32 checkBgmPlaying();
    void demoBgmStop(u32 fade);
    void bgmStop(u32 fade, s32 keepSub);
    void bgmStart(u32 id, u32 fade, u32 keepSub);
    u32 seStart(u32, u32, u32, s32, f32, f32, f32, f32, u32);
    f32 calcPosVolume(VectorWords *, f32);
    f32 calcPosPanLR(VectorWords *);
    f32 calcPosPanSR(VectorWords *, f32);
    void bgmMute(gptr<Sound> *, u32, u32, u32);
    void startSoundVec(u32, gptr<Sound> *, u32, u32, u32, u32);
    f32 calcMainBgmVol();
    f32 calcMainBgmVolWithoutFanfare();
    // HD: camera reads retain guest-null behavior instead of dereferencing host nullptr.
    VectorWords *cameraPosition() {
        return gabi::at<VectorWords>(gabi::load<u32>(gabi::ea(mAudioCamera.get())));
    }
    void *cameraMatrix() {
        return gabi::at<void>(
            gabi::load<u32>(gabi::ea(mAudioCamera.get()) + offsetof(AudioCamera, matrix)));
    }
    gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x3E];
    be<u8> mMorningBgmPlaying;  /* was field_0x0032: JAIZelBasic::mMorningBgmPlaying (JAZelAudio/JAIZelBasic.h) */
    u8 padding43[0x30];
    be<u8> mBgmDisabled;  /* was field_0x0063: JAIZelBasic::mBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTalkInBlocked;  /* was field_0x0064: JAIZelBasic::mTalkInBlocked (JAZelAudio/JAIZelBasic.h) */
    be<u8> mMtDragonMuteRequest;  /* was field_0x0065: JAIZelBasic::mMtDragonMuteRequest (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSelectBgmOnly;  /* was field_0x0066: JAIZelBasic::mSelectBgmOnly (JAZelAudio/JAIZelBasic.h) */
    u8 padding77[0x1];
    gptr<Sound> mpMainBgmSound;
    gptr<Sound> mpSubBgmSound;
    gptr<Sound> mpStreamBgmSound;
    be<u32> mSubBgmNum;
    be<u32> mMainBgmNum;
    be<u32> mStreamBgmNum;
    be<f32> mMainBgmMuteVolume;  /* was field_0x0080: JAIZelBasic::mMainBgmMuteVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmTalkVolume;  /* was field_0x0084: JAIZelBasic::mMainBgmTalkVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmMenuVolume;  /* was field_0x0088: JAIZelBasic::mMainBgmMenuVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmSubBgmVolume;  /* was field_0x008c: JAIZelBasic::mMainBgmSubBgmVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmSceneVolume;  /* was field_0x0090: JAIZelBasic::mMainBgmSceneVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmSeaVolume;  /* was field_0x0094: JAIZelBasic::mMainBgmSeaVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmTaktVolume;  /* was field_0x0098: JAIZelBasic::mMainBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmCbPracticeVolume;  /* was field_0x009c: JAIZelBasic::mMainBgmCbPracticeVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mSubBgmTalkMenuVolume;  /* was field_0x00a0: JAIZelBasic::mSubBgmTalkMenuVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mSubBgmTaktVolume;  /* was field_0x00a4: JAIZelBasic::mSubBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mSubBgmCbPracticeVolume;  /* was field_0x00a8: JAIZelBasic::mSubBgmCbPracticeVolume (JAZelAudio/JAIZelBasic.h) */
    be<f32> mMainBgmHeightVolume;  /* was field_0x00ac: JAIZelBasic::mMainBgmHeightVolume (JAZelAudio/JAIZelBasic.h) */
    u8 paddingC0[0x5];
    be<u8> mSomeSpecialBGMFlag;
    be<u8> mLastMinibossSubBGMType;
    u8 paddingC7[0x1];
    be<u8> mStreamBgmStarted;  /* was field_0x00b8: JAIZelBasic::mStreamBgmStarted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mDemoFanfareMuted;  /* was field_0x00b9: JAIZelBasic::mDemoFanfareMuted (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCA[0x1];
    be<u8> mFanfareResumePending;  /* was field_0x00bb: JAIZelBasic::mFanfareResumePending (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCC[0x2];
    be<u8> mTalkSeBlocked;  /* was field_0x00be: JAIZelBasic::mTalkSeBlocked (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCF[0xB];
    be<u8> mBattleBgmDisabled;  /* was field_0x00ca: JAIZelBasic::mBattleBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    be<u8> mEnemyNearbyDelay;  /* was field_0x00cb: JAIZelBasic::mEnemyNearbyDelay (JAZelAudio/JAIZelBasic.h) */
    be<u8> mDkMaskBroken;  /* was field_0x00cc: JAIZelBasic::mDkMaskBroken (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDD[0x3];
    gptr<VectorWords> mpCbPracticePos;  /* was field_0x00d0: JAIZelBasic::mpCbPracticePos (JAZelAudio/JAIZelBasic.h) */
    u8 paddingE4[0x18D];
    be<u8> mAllSeMuted;  /* was field_0x0201: JAIZelBasic::mAllSeMuted (JAZelAudio/JAIZelBasic.h) */
    u8 padding272[0x4];
    be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTaktSeTimer;  /* was field_0x0207: JAIZelBasic::mTaktSeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding278[0x1C];
    be<s32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    be<u32> mNextSceneBgmId;
    u8 padding29C[0xC];
    be<u8> mIslandRoomNo;
    u8 padding2A9[0xB];
    LevelObjectSound mLevelObjectSounds[15];
    be<u32> mLevelObjectSoundCount;
    u8 padding15F0[0x9BC];
    be<u8> mIslandLinkBgm;  /* was field_0x1f3c: JAIZelBasic::mIslandLinkBgm (JAZelAudio/JAIZelBasic.h) */
    be<u8> mIsSailing;
    u8 padding1FAE[0x2];
    be<f32> mShipSpeed;  /* was field_0x1f40: JAIZelBasic::mShipSpeed (JAZelAudio/JAIZelBasic.h) */
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, mMorningBgmPlaying, 0x42);
WWHD_OFFSET(JAIZelBasic, mBgmDisabled, 0x73);
WWHD_OFFSET(JAIZelBasic, mTalkInBlocked, 0x74);
WWHD_OFFSET(JAIZelBasic, mMtDragonMuteRequest, 0x75);
WWHD_OFFSET(JAIZelBasic, mSelectBgmOnly, 0x76);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mpSubBgmSound, 0x7C);
WWHD_OFFSET(JAIZelBasic, mpStreamBgmSound, 0x80);
WWHD_OFFSET(JAIZelBasic, mSubBgmNum, 0x84);
WWHD_OFFSET(JAIZelBasic, mMainBgmNum, 0x88);
WWHD_OFFSET(JAIZelBasic, mStreamBgmNum, 0x8C);
WWHD_OFFSET(JAIZelBasic, mMainBgmMuteVolume, 0x90);
WWHD_OFFSET(JAIZelBasic, mMainBgmTalkVolume, 0x94);
WWHD_OFFSET(JAIZelBasic, mMainBgmMenuVolume, 0x98);
WWHD_OFFSET(JAIZelBasic, mMainBgmSubBgmVolume, 0x9C);
WWHD_OFFSET(JAIZelBasic, mMainBgmSceneVolume, 0xA0);
WWHD_OFFSET(JAIZelBasic, mMainBgmSeaVolume, 0xA4);
WWHD_OFFSET(JAIZelBasic, mMainBgmTaktVolume, 0xA8);
WWHD_OFFSET(JAIZelBasic, mMainBgmCbPracticeVolume, 0xAC);
WWHD_OFFSET(JAIZelBasic, mSubBgmTalkMenuVolume, 0xB0);
WWHD_OFFSET(JAIZelBasic, mSubBgmTaktVolume, 0xB4);
WWHD_OFFSET(JAIZelBasic, mSubBgmCbPracticeVolume, 0xB8);
WWHD_OFFSET(JAIZelBasic, mMainBgmHeightVolume, 0xBC);
WWHD_OFFSET(JAIZelBasic, mSomeSpecialBGMFlag, 0xC5);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, mStreamBgmStarted, 0xC8);
WWHD_OFFSET(JAIZelBasic, mDemoFanfareMuted, 0xC9);
WWHD_OFFSET(JAIZelBasic, mFanfareResumePending, 0xCB);
WWHD_OFFSET(JAIZelBasic, mTalkSeBlocked, 0xCE);
WWHD_OFFSET(JAIZelBasic, mBattleBgmDisabled, 0xDA);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyDelay, 0xDB);
WWHD_OFFSET(JAIZelBasic, mDkMaskBroken, 0xDC);
WWHD_OFFSET(JAIZelBasic, mpCbPracticePos, 0xE0);
WWHD_OFFSET(JAIZelBasic, mAllSeMuted, 0x271);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mTaktSeTimer, 0x277);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mNextSceneBgmId, 0x298);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_OFFSET(JAIZelBasic, mLevelObjectSounds, 0x2B4);
WWHD_OFFSET(JAIZelBasic, mLevelObjectSoundCount, 0x15EC);
WWHD_OFFSET(JAIZelBasic, mIslandLinkBgm, 0x1FAC);
WWHD_OFFSET(JAIZelBasic, mIsSailing, 0x1FAD);
WWHD_OFFSET(JAIZelBasic, mShipSpeed, 0x1FB0);
WWHD_SIZE(LevelObjectInstance, 16);
WWHD_SIZE(LevelObjectSound, 0x148);
WWHD_SIZE(SafeString, 8);
WWHD_SIZE(VectorWords, 12);
} // namespace jaizel34

u32 getParamSeqPlayTrackMax();
jaizel34::SeqUpdateData *getPlayTrackInfo(u32);
void *hd_streamPlayer(void *);
s32 hd_checkStream(void *, u32, u32, jaizel34::Sound *, jaizel34::Sound *);
void hd_stopStream(void *, u32, jaizel34::Sound *, jaizel34::Sound *);
void hd_setStreamVolume(void *, u32, f32);
void hd_setStreamMixVolume(void *, u32, f32);
void jaizelTransform(void *, jaizel34::VectorWords *, jaizel34::VectorWords *);
f32 std_sqrtf(f32);

WWHD_SIZE(jaizel34::JAIZelBasic, 0x1FB4);
WWHD_SIZE(jaizel34::AudioCamera, 12);
WWHD_SIZE(jaizel34::SeqUpdateData, 0x4C);
WWHD_SIZE(jaizel34::SafeStringVtable, 0x18);
WWHD_SIZE(jaizel34::SoundHeader, 1);
