/** JAIZelBasic initialization, scenes and wave banks.
 * HD:32 SE slots,16 allocation records and retained camera state expand the GC layout. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
namespace jaizel7 {
using GuestByte = be<u8>;
struct Sound : ::JAISound {
    void stop(u32);
    void setPortData(u32, u32);
    void setTrackVolume(u32, f32, u32);
    void setSeqInterVolume(u32, f32, u32);
    void setTempoProportion(f32, u32);
};
struct SoundHead {
    be<u8> track, mState;  /* was field_0x4: JAISound::track (JSystem/JAudio/JAISound.h; byte 0 of the sound handle, GC +4) */
};
struct AudioCamera {
    gptr<cXyz> position, previousPosition;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos); was field_0x4: JAInter::Camera::previousPosition (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 points it at mCameraOldPos */
    gptr<Mtx34> matrix;  /* was field_0x8: JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera matrix */
};
struct VectorWords {
    be<u32> w[3];
};
struct MonsterSlot {
    be<u32> mActorId;  /* was field_0x00: monsSeStart 020299D8: the actor id argument (fopAcM_monsSeStart passes the actor's id) owning the slot; -1 free (monsSeInit 0202573C) */
    gptr<Sound> mpSound;  /* was field_0x04: monsSeStart 020299D8: handle of the slot's sound (stopped when the actor starts a new one) */
};
struct AllocationRecord {
    be<u32> count;
    gptr<Sound> sound;
};
struct Random {
    be<u32> mSeed, mValue, mShuffle[32];  /* was field_0x0: JMath::TRandom_enough_::mSeed (JSystem/JMath/random.h): mRandomHD is a TRandom_enough_ (createJAIZelBasic seeds it, 0x88 bytes); was field_0x4: JMath::TRandom_enough_::mValue (JSystem/JMath/random.h); was field_0x8: JMath::TRandom_enough_::mShuffle (JSystem/JMath/random.h) */
};
struct SeqUpdateData {
    u8 padding[0x48];
    gptr<Sound> mSequence;
};
struct JAIZelBasic {
    void monsSeInit();
    void initSe();
    void kuroboVoicePlay(u32 id, cXyz *pos, u32 reverb);
    void mbossBgmNearByProcess(f32 dist);
    void enemyNearBy();
    u32 getMapInfoFxline(u32 n);
    s32 checkLinkOnSea();
    s32 checkLinkOnBoardSea();
    void init(void *heap, u32 size);
    void gframeProcess();
    void resetProcess();
    void sceneChange(u32 bgm, u32 wave1, u32 wave2, s32 keep);
    void setScene(s32 sc, s32 room, s32 keep, s32 layer);
    void load1stDynamicWave();
    void bgmStreamPrepare(u32 id);
    void load2ndDynamicWave();
    void resetRecover();
    void getCameraInfo(cXyz *pos, Mtx34 *mtx, u32 idx);
    void getCameraMapInfo(u32 v);
    void setCameraGroupInfo(u32 g);
    void setCameraPolygonPos(cXyz *pos);
    void setLinkGroupInfo(u32 g);
    void setEventBit(be<u8> *p);
    s32 spotNameToId(GuestByte *name);
    void setSceneName(GuestByte *name, s32 room, s32 layer);
    s32 checkStaticWaves();
    u32 check1stDynamicWave();
    void loadStaticWaves();
    s32 checkFirstWaves();
    void setLinkHp(s32 hp, s32 max);
    void setLinkSwordType(u32 a, u32 b);
    void setLinkShieldType(u32 a, u32 b);
    void setLinkBootsType(u32 a);
    void setLinkOnBoard(u32 a);
    void heartGaugeOn();
    void initLevObjSE();
    void bgmStart(u32, u32, u32);
    void bgmStop(u32, u32);
    s32 checkBgmPlaying();
    s32 checkSeqIDDemoPlaying(u32);
    s32 checkDayTime();
    s32 checkEventBit(u32);
    u32 expandSceneBgmNum(u32);
    void sceneBgmStart();
    void menuOut();
    s32 isDemo();
    void zeldaGFrameWork();
    void hdStreamVolume(u32, f32);
    void bgmMute(gptr<Sound> *, u32, u32, u32);
    void startSoundVec(u32, gptr<Sound> *, cXyz *, u32, u32, u32);
    void setSeCategoryVolume(u32, u32);
    void stopSoundCategory(u32, cXyz *);
    void initializeAudioBase();
    void initDriver(void *, u32, u32);
    void initInterface(u32);
    void processFrameWork();
    void prepareStream(u32, gptr<Sound> *, u32, u32, u32, u32);
    // HD: retain guest-null camera accesses and unsigned32bit indexing.
    AudioCamera *cameraAt(u32 index) {
        return gabi::at<AudioCamera>(gabi::ea(mAudioCamera.get()) +
                                     index * u32(sizeof(AudioCamera)));
    }
    gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x28];
    gptr<void> __vtbl;
    be<u8> mIsMenuIn;  /* was field_0x0020: JAIZelBasic::mIsMenuIn (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSceneBgmStartedOnce;  /* was field_0x0021: JAIZelBasic::mSceneBgmStartedOnce (JAZelAudio/JAIZelBasic.h) */
    u8 padding32[0x2];
    gptr<be<u8>> mpEventBits;  /* was field_0x0024: JAIZelBasic::mpEventBits (JAZelAudio/JAIZelBasic.h) */
    be<u32> mSceneFrameCount;  /* was field_0x0028: JAIZelBasic::mSceneFrameCount (JAZelAudio/JAIZelBasic.h) */
    be<u8> mHour;
    be<u8> mMinute;
    be<u8> mWeekday;
    be<u8> field_0x002f;
    be<u16> mPrevTime;  /* was field_0x0030: JAIZelBasic::mPrevTime (JAZelAudio/JAIZelBasic.h) */
    be<u8> mMorningBgmPlaying;  /* was field_0x0032: JAIZelBasic::mMorningBgmPlaying (JAZelAudio/JAIZelBasic.h) */
    be<u8> mDayNightSwitched;  /* was field_0x0033: JAIZelBasic::mDayNightSwitched (JAZelAudio/JAIZelBasic.h) */
    be<s32> mLinkHp;  /* was field_0x0034: JAIZelBasic::mLinkHp (JAZelAudio/JAIZelBasic.h) */
    be<s32> mLinkMaxHp;  /* was field_0x0038: JAIZelBasic::mLinkMaxHp (JAZelAudio/JAIZelBasic.h) */
    be<f32> mLinkHpRatio;  /* was field_0x003c: JAIZelBasic::mLinkHpRatio (JAZelAudio/JAIZelBasic.h) */
    be<u8> mHeartGaugeTimer;  /* was field_0x0040: JAIZelBasic::mHeartGaugeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkSwordType;  /* was field_0x0041: JAIZelBasic::mLinkSwordType (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkSwordPlace;  /* was field_0x0042: JAIZelBasic::mLinkSwordPlace (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkShieldType;  /* was field_0x0043: JAIZelBasic::mLinkShieldType (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkShieldPlace;  /* was field_0x0044: JAIZelBasic::mLinkShieldPlace (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkSwordShieldOnBack;  /* was field_0x0045: JAIZelBasic::mLinkSwordShieldOnBack (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkBootsType;  /* was field_0x0046: JAIZelBasic::mLinkBootsType (JAZelAudio/JAIZelBasic.h) */
    be<u8> mLinkOnBoard;  /* was field_0x0047: JAIZelBasic::mLinkOnBoard (JAZelAudio/JAIZelBasic.h) */
    cXyz mCameraOldPos;  /* was field_0x0048: JAIZelBasic::mCameraOldPos (JAZelAudio/JAIZelBasic.h) */
    cXyz mCameraPolygonPos;  /* was field_0x0054: JAIZelBasic::mCameraPolygonPos (JAZelAudio/JAIZelBasic.h) */
    be<u8> mbLandingDemoStarted;
    be<u8> mEventTimer;  /* was field_0x0061: JAIZelBasic::mEventTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mKamomeFlag;  /* was field_0x0062: JAIZelBasic::mKamomeFlag (JAZelAudio/JAIZelBasic.h) */
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
    be<u32> mSubBgmStopTimer;  /* was field_0x00b0: JAIZelBasic::mSubBgmStopTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> field_0x00b4;
    be<u8> mSomeSpecialBGMFlag;
    be<u8> mLastMinibossSubBGMType;
    be<u8> mSkyCloistersTimer;  /* was field_0x00b7: JAIZelBasic::mSkyCloistersTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mStreamBgmStarted;  /* was field_0x00b8: JAIZelBasic::mStreamBgmStarted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mDemoFanfareMuted;  /* was field_0x00b9: JAIZelBasic::mDemoFanfareMuted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTetraFlyStreamStarted;  /* was field_0x00ba: JAIZelBasic::mTetraFlyStreamStarted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mFanfareResumePending;  /* was field_0x00bb: JAIZelBasic::mFanfareResumePending (JAZelAudio/JAIZelBasic.h) */
    be<u8> mPendingBgmStart;  /* was field_0x00bc: JAIZelBasic::mPendingBgmStart (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSceneBgmLocked;  /* was field_0x00bd: JAIZelBasic::mSceneBgmLocked (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTalkSeBlocked;  /* was field_0x00be: JAIZelBasic::mTalkSeBlocked (JAZelAudio/JAIZelBasic.h) */
    be<u8> mCameraGroupLocked;  /* was field_0x00bf: JAIZelBasic::mCameraGroupLocked (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBattleBgmFadeDelay;  /* was field_0x00c0: JAIZelBasic::mBattleBgmFadeDelay (JAZelAudio/JAIZelBasic.h) */
    be<s8> mEnemyNearbyOn;  /* was field_0x00c1: JAIZelBasic::mEnemyNearbyOn (JAZelAudio/JAIZelBasic.h) */
    u8 paddingD2[0x2];
    be<u32> mEnemyNearbyFadeTimer;  /* was field_0x00c4: JAIZelBasic::mEnemyNearbyFadeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mKaitengiriTimer;  /* was field_0x00c8: JAIZelBasic::mKaitengiriTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSwordUsing;  /* was field_0x00c9: JAIZelBasic::mSwordUsing (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBattleBgmDisabled;  /* was field_0x00ca: JAIZelBasic::mBattleBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    be<s8> mEnemyNearbyDelay;  /* was field_0x00cb: JAIZelBasic::mEnemyNearbyDelay (JAZelAudio/JAIZelBasic.h) */
    be<u8> mDkMaskBroken;  /* was field_0x00cc: JAIZelBasic::mDkMaskBroken (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSkipSceneStream;  /* was field_0x00cd: JAIZelBasic::mSkipSceneStream (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBattleBgmHoldTimer;  /* was field_0x00ce: JAIZelBasic::mBattleBgmHoldTimer (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDF[0x1];
    gptr<cXyz> mpCbPracticePos;  /* was field_0x00d0: JAIZelBasic::mpCbPracticePos (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpSeSound[32];
    be<u32> mSeNum[32];
    be<u32> mSePosAddr[32];  /* was field_0x0194: JAIZelBasic::mSePosAddr (JAZelAudio/JAIZelBasic.h) */
    be<u32> mSeSlotIndex;  /* was field_0x01f4: JAIZelBasic::mSeSlotIndex (JAZelAudio/JAIZelBasic.h) */
    be<u8> mNoCameraTimer;  /* was field_0x01f8: JAIZelBasic::mNoCameraTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mAddGrassSePlayed;  /* was field_0x01f9: JAIZelBasic::mAddGrassSePlayed (JAZelAudio/JAIZelBasic.h) */
    be<u8> mCutGrassSeTimer;  /* was field_0x01fa: JAIZelBasic::mCutGrassSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBrkWrailKSePlayed;  /* was field_0x01fb: JAIZelBasic::mBrkWrailKSePlayed (JAZelAudio/JAIZelBasic.h) */
    be<u8> mColBrkWrailingSePlayed;  /* was field_0x01fc: JAIZelBasic::mColBrkWrailingSePlayed (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBgnStringRippleSeTimer;  /* was field_0x01fd: JAIZelBasic::mBgnStringRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBgnBodyRippleSeTimer;  /* was field_0x01fe: JAIZelBasic::mBgnBodyRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBgnHeadRippleSeTimer;  /* was field_0x01ff: JAIZelBasic::mBgnHeadRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBgnTailRippleSeTimer;  /* was field_0x0200: JAIZelBasic::mBgnTailRippleSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mAllSeMuted;  /* was field_0x0201: JAIZelBasic::mAllSeMuted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSwingSeTimers[2];  /* was field_0x0202: JAIZelBasic::mSwingSeTimers (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSeaEnvSeOff;  /* was field_0x0204: JAIZelBasic::mSeaEnvSeOff (JAZelAudio/JAIZelBasic.h) */
    be<u8> mForceBack;  /* was field_0x0205: JAIZelBasic::mForceBack (JAZelAudio/JAIZelBasic.h) */
    be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTaktSeTimer;  /* was field_0x0207: JAIZelBasic::mTaktSeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mPirateSailSePlayed;  /* was field_0x0208: JAIZelBasic::mPirateSailSePlayed (JAZelAudio/JAIZelBasic.h) */
    u8 padding279[0x3];
    be<u32> field_0x020c;
    be<u32> field_0x0210;
    be<u32> mpTelescopeSound;  /* was field_0x0214: JAIZelBasic::mpTelescopeSound (JAZelAudio/JAIZelBasic.h) */
    be<u32> mCameraMapInfo;  /* was field_0x0218: JAIZelBasic::mCameraMapInfo (JAZelAudio/JAIZelBasic.h) */
    be<u8> mCameraSeaFloorGroupInfo;
    be<u8> mLinkSeaFloorGroupInfo;
    be<u8> field_0x021e;
    u8 padding28F[0x1];
    be<s32> mNextSceneNo;  /* was field_0x0220: JAIZelBasic::mNextSceneNo (JAZelAudio/JAIZelBasic.h) */
    be<s32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    be<u32> mNextSceneBgmId;
    be<u8> mSceneSetPending;  /* was field_0x022c: JAIZelBasic::mSceneSetPending (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSceneActive;  /* was field_0x022d: JAIZelBasic::mSceneActive (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSceneBgmChanged;  /* was field_0x022e: JAIZelBasic::mSceneBgmChanged (JAZelAudio/JAIZelBasic.h) */
    be<u8> mWaveToReload;  /* was field_0x022f: JAIZelBasic::mWaveToReload (JAZelAudio/JAIZelBasic.h) */
    be<u8> mWaveBankToErase;  /* was field_0x0230: JAIZelBasic::mWaveBankToErase (JAZelAudio/JAIZelBasic.h) */
    be<u8> mWaveBankReload;  /* was field_0x0231: JAIZelBasic::mWaveBankReload (JAZelAudio/JAIZelBasic.h) */
    be<u8> mFirstDynamicSceneWaveIndex;
    be<u8> mPrevFirstDynamicSceneWaveIndex;  /* was field_0x0233: JAIZelBasic::mPrevFirstDynamicSceneWaveIndex (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSetNum;
    be<u8> mSecondDynamicSceneWaveIndex;
    be<u8> mPrevSecondDynamicSceneWaveIndex;  /* was field_0x0236: JAIZelBasic::mPrevSecondDynamicSceneWaveIndex (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSecondDynamicWaveChanged;  /* was field_0x0237: JAIZelBasic::mSecondDynamicWaveChanged (JAZelAudio/JAIZelBasic.h) */
    be<u8> mIslandRoomNo;
    be<u8> mLayerNo;  /* was field_0x0239: JAIZelBasic::mLayerNo (JAZelAudio/JAIZelBasic.h) */
    be<u8> mInitialized;  /* was field_0x023a: JAIZelBasic::mInitialized (JAZelAudio/JAIZelBasic.h) */
    u8 padding2AB[0x1];
    be<u32> mBgmWaveCountHD;
    gptr<be<u8>> mBgmWaveInfoHD;
    u8 padding2B4[0x1338];
    be<u32> mLevelObjectSoundCountHD;
    u8 padding15F0[0x600];
    be<u32> mSeaEnvPosCount;  /* was field_0x1b80: JAIZelBasic::mSeaEnvPosCount (JAZelAudio/JAIZelBasic.h) */
    cXyz mSeaPositionHD;
    u8 padding1C00[0x240];
    be<u32> mRiverPosCount;  /* was field_0x1dd0: JAIZelBasic::mRiverPosCount (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBstHoriSeOn;  /* was field_0x1dd4: JAIZelBasic::mBstHoriSeOn (JAZelAudio/JAIZelBasic.h) */
    u8 padding1E45[0xC3];
    be<u32> mWaterfallPosCount;  /* was field_0x1e98: JAIZelBasic::mWaterfallPosCount (JAZelAudio/JAIZelBasic.h) */
    u8 padding1F0C[0x24];
    be<u32> mWindowPosCount;  /* was field_0x1ec0: JAIZelBasic::mWindowPosCount (JAZelAudio/JAIZelBasic.h) */
    be<u32> mpSeaEnvSoundL;  /* was field_0x1ec4: JAIZelBasic::mpSeaEnvSoundL (JAZelAudio/JAIZelBasic.h) */
    be<u32> mpSeaEnvSoundR;  /* was field_0x1ec8: JAIZelBasic::mpSeaEnvSoundR (JAZelAudio/JAIZelBasic.h) */
    be<u32> mpSeaEnvSoundS;  /* was field_0x1ecc: JAIZelBasic::mpSeaEnvSoundS (JAZelAudio/JAIZelBasic.h) */
    be<u32> mpSeaShoreSound;  /* was field_0x1ed0: JAIZelBasic::mpSeaShoreSound (JAZelAudio/JAIZelBasic.h) */
    u8 padding1F44[0x60];
    be<u32> mFireblastPosCount;  /* was field_0x1f34: JAIZelBasic::mFireblastPosCount (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpWindSound;  /* was field_0x1f38: JAIZelBasic::mpWindSound (JAZelAudio/JAIZelBasic.h) */
    be<u8> mIslandLinkBgm;  /* was field_0x1f3c: JAIZelBasic::mIslandLinkBgm (JAZelAudio/JAIZelBasic.h) */
    be<u8> mIsSailing;
    u8 padding1FAE[0x2];
    be<f32> mShipSpeed;  /* was field_0x1f40: JAIZelBasic::mShipSpeed (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpShipCruiseSound;  /* was field_0x1f44: JAIZelBasic::mpShipCruiseSound (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpShipWaveSound;  /* was field_0x1f48: JAIZelBasic::mpShipWaveSound (JAZelAudio/JAIZelBasic.h) */
    MonsterSlot mMonsSe[30];  /* was field_0x1f4c: JAIZelBasic::mMonsSe (JAZelAudio/JAIZelBasic.h) */
    be<u8> mKuroboMotionCount;  /* was field_0x203c: JAIZelBasic::mKuroboMotionCount (JAZelAudio/JAIZelBasic.h) */
    be<u8> mKuroboVoiceCount;  /* was field_0x203d: JAIZelBasic::mKuroboVoiceCount (JAZelAudio/JAIZelBasic.h) */
    u8 padding20AE[0x2];
    gptr<Sound> mpKuroboMotionSound[4];  /* was field_0x2040: JAIZelBasic::mpKuroboMotionSound (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpKuroboVoiceSound[4];  /* was field_0x2050: JAIZelBasic::mpKuroboVoiceSound (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpCharVoiceSound;  /* was field_0x2060: JAIZelBasic::mpCharVoiceSound (JAZelAudio/JAIZelBasic.h) */
    be<u16> mVoiceIndexHD;
    u8 padding20D6[0xE];
    be<u32> mSystemSoundHD;  /* was field_0x2074: same slot (+0x20E4) as JAIZelBasic::mSystemSoundHD in JAZelAudio/JAIZelBasic.h; initSe 02025760 clears it */
    AllocationRecord mAllocationRecordsHD[16];
    be<u32> mAllocationRecordIndexHD;
    Random mRandomHD;
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, __vtbl, 0x2C);
WWHD_OFFSET(JAIZelBasic, mIsMenuIn, 0x30);
WWHD_OFFSET(JAIZelBasic, mSceneBgmStartedOnce, 0x31);
WWHD_OFFSET(JAIZelBasic, mpEventBits, 0x34);
WWHD_OFFSET(JAIZelBasic, mSceneFrameCount, 0x38);
WWHD_OFFSET(JAIZelBasic, mHour, 0x3C);
WWHD_OFFSET(JAIZelBasic, mMinute, 0x3D);
WWHD_OFFSET(JAIZelBasic, mWeekday, 0x3E);
WWHD_OFFSET(JAIZelBasic, field_0x002f, 0x3F);
WWHD_OFFSET(JAIZelBasic, mPrevTime, 0x40);
WWHD_OFFSET(JAIZelBasic, mMorningBgmPlaying, 0x42);
WWHD_OFFSET(JAIZelBasic, mDayNightSwitched, 0x43);
WWHD_OFFSET(JAIZelBasic, mLinkHp, 0x44);
WWHD_OFFSET(JAIZelBasic, mLinkMaxHp, 0x48);
WWHD_OFFSET(JAIZelBasic, mLinkHpRatio, 0x4C);
WWHD_OFFSET(JAIZelBasic, mHeartGaugeTimer, 0x50);
WWHD_OFFSET(JAIZelBasic, mLinkSwordType, 0x51);
WWHD_OFFSET(JAIZelBasic, mLinkSwordPlace, 0x52);
WWHD_OFFSET(JAIZelBasic, mLinkShieldType, 0x53);
WWHD_OFFSET(JAIZelBasic, mLinkShieldPlace, 0x54);
WWHD_OFFSET(JAIZelBasic, mLinkSwordShieldOnBack, 0x55);
WWHD_OFFSET(JAIZelBasic, mLinkBootsType, 0x56);
WWHD_OFFSET(JAIZelBasic, mLinkOnBoard, 0x57);
WWHD_OFFSET(JAIZelBasic, mCameraOldPos, 0x58);
WWHD_OFFSET(JAIZelBasic, mCameraPolygonPos, 0x64);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, mEventTimer, 0x71);
WWHD_OFFSET(JAIZelBasic, mKamomeFlag, 0x72);
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
WWHD_OFFSET(JAIZelBasic, mSubBgmStopTimer, 0xC0);
WWHD_OFFSET(JAIZelBasic, field_0x00b4, 0xC4);
WWHD_OFFSET(JAIZelBasic, mSomeSpecialBGMFlag, 0xC5);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, mSkyCloistersTimer, 0xC7);
WWHD_OFFSET(JAIZelBasic, mStreamBgmStarted, 0xC8);
WWHD_OFFSET(JAIZelBasic, mDemoFanfareMuted, 0xC9);
WWHD_OFFSET(JAIZelBasic, mTetraFlyStreamStarted, 0xCA);
WWHD_OFFSET(JAIZelBasic, mFanfareResumePending, 0xCB);
WWHD_OFFSET(JAIZelBasic, mPendingBgmStart, 0xCC);
WWHD_OFFSET(JAIZelBasic, mSceneBgmLocked, 0xCD);
WWHD_OFFSET(JAIZelBasic, mTalkSeBlocked, 0xCE);
WWHD_OFFSET(JAIZelBasic, mCameraGroupLocked, 0xCF);
WWHD_OFFSET(JAIZelBasic, mBattleBgmFadeDelay, 0xD0);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyOn, 0xD1);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyFadeTimer, 0xD4);
WWHD_OFFSET(JAIZelBasic, mKaitengiriTimer, 0xD8);
WWHD_OFFSET(JAIZelBasic, mSwordUsing, 0xD9);
WWHD_OFFSET(JAIZelBasic, mBattleBgmDisabled, 0xDA);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyDelay, 0xDB);
WWHD_OFFSET(JAIZelBasic, mDkMaskBroken, 0xDC);
WWHD_OFFSET(JAIZelBasic, mSkipSceneStream, 0xDD);
WWHD_OFFSET(JAIZelBasic, mBattleBgmHoldTimer, 0xDE);
WWHD_OFFSET(JAIZelBasic, mpCbPracticePos, 0xE0);
WWHD_OFFSET(JAIZelBasic, mpSeSound, 0xE4);
WWHD_OFFSET(JAIZelBasic, mSeNum, 0x164);
WWHD_OFFSET(JAIZelBasic, mSePosAddr, 0x1E4);
WWHD_OFFSET(JAIZelBasic, mSeSlotIndex, 0x264);
WWHD_OFFSET(JAIZelBasic, mNoCameraTimer, 0x268);
WWHD_OFFSET(JAIZelBasic, mAddGrassSePlayed, 0x269);
WWHD_OFFSET(JAIZelBasic, mCutGrassSeTimer, 0x26A);
WWHD_OFFSET(JAIZelBasic, mBrkWrailKSePlayed, 0x26B);
WWHD_OFFSET(JAIZelBasic, mColBrkWrailingSePlayed, 0x26C);
WWHD_OFFSET(JAIZelBasic, mBgnStringRippleSeTimer, 0x26D);
WWHD_OFFSET(JAIZelBasic, mBgnBodyRippleSeTimer, 0x26E);
WWHD_OFFSET(JAIZelBasic, mBgnHeadRippleSeTimer, 0x26F);
WWHD_OFFSET(JAIZelBasic, mBgnTailRippleSeTimer, 0x270);
WWHD_OFFSET(JAIZelBasic, mAllSeMuted, 0x271);
WWHD_OFFSET(JAIZelBasic, mSwingSeTimers, 0x272);
WWHD_OFFSET(JAIZelBasic, mSeaEnvSeOff, 0x274);
WWHD_OFFSET(JAIZelBasic, mForceBack, 0x275);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mTaktSeTimer, 0x277);
WWHD_OFFSET(JAIZelBasic, mPirateSailSePlayed, 0x278);
WWHD_OFFSET(JAIZelBasic, field_0x020c, 0x27C);
WWHD_OFFSET(JAIZelBasic, field_0x0210, 0x280);
WWHD_OFFSET(JAIZelBasic, mpTelescopeSound, 0x284);
WWHD_OFFSET(JAIZelBasic, mCameraMapInfo, 0x288);
WWHD_OFFSET(JAIZelBasic, mCameraSeaFloorGroupInfo, 0x28C);
WWHD_OFFSET(JAIZelBasic, mLinkSeaFloorGroupInfo, 0x28D);
WWHD_OFFSET(JAIZelBasic, field_0x021e, 0x28E);
WWHD_OFFSET(JAIZelBasic, mNextSceneNo, 0x290);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mNextSceneBgmId, 0x298);
WWHD_OFFSET(JAIZelBasic, mSceneSetPending, 0x29C);
WWHD_OFFSET(JAIZelBasic, mSceneActive, 0x29D);
WWHD_OFFSET(JAIZelBasic, mSceneBgmChanged, 0x29E);
WWHD_OFFSET(JAIZelBasic, mWaveToReload, 0x29F);
WWHD_OFFSET(JAIZelBasic, mWaveBankToErase, 0x2A0);
WWHD_OFFSET(JAIZelBasic, mWaveBankReload, 0x2A1);
WWHD_OFFSET(JAIZelBasic, mFirstDynamicSceneWaveIndex, 0x2A2);
WWHD_OFFSET(JAIZelBasic, mPrevFirstDynamicSceneWaveIndex, 0x2A3);
WWHD_OFFSET(JAIZelBasic, mSetNum, 0x2A4);
WWHD_OFFSET(JAIZelBasic, mSecondDynamicSceneWaveIndex, 0x2A5);
WWHD_OFFSET(JAIZelBasic, mPrevSecondDynamicSceneWaveIndex, 0x2A6);
WWHD_OFFSET(JAIZelBasic, mSecondDynamicWaveChanged, 0x2A7);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_OFFSET(JAIZelBasic, mLayerNo, 0x2A9);
WWHD_OFFSET(JAIZelBasic, mInitialized, 0x2AA);
WWHD_OFFSET(JAIZelBasic, mBgmWaveCountHD, 0x2AC);
WWHD_OFFSET(JAIZelBasic, mBgmWaveInfoHD, 0x2B0);
WWHD_OFFSET(JAIZelBasic, mLevelObjectSoundCountHD, 0x15EC);
WWHD_OFFSET(JAIZelBasic, mSeaEnvPosCount, 0x1BF0);
WWHD_OFFSET(JAIZelBasic, mSeaPositionHD, 0x1BF4);
WWHD_OFFSET(JAIZelBasic, mRiverPosCount, 0x1E40);
WWHD_OFFSET(JAIZelBasic, mBstHoriSeOn, 0x1E44);
WWHD_OFFSET(JAIZelBasic, mWaterfallPosCount, 0x1F08);
WWHD_OFFSET(JAIZelBasic, mWindowPosCount, 0x1F30);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundL, 0x1F34);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundR, 0x1F38);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundS, 0x1F3C);
WWHD_OFFSET(JAIZelBasic, mpSeaShoreSound, 0x1F40);
WWHD_OFFSET(JAIZelBasic, mFireblastPosCount, 0x1FA4);
WWHD_OFFSET(JAIZelBasic, mpWindSound, 0x1FA8);
WWHD_OFFSET(JAIZelBasic, mIslandLinkBgm, 0x1FAC);
WWHD_OFFSET(JAIZelBasic, mIsSailing, 0x1FAD);
WWHD_OFFSET(JAIZelBasic, mShipSpeed, 0x1FB0);
WWHD_OFFSET(JAIZelBasic, mpShipCruiseSound, 0x1FB4);
WWHD_OFFSET(JAIZelBasic, mpShipWaveSound, 0x1FB8);
WWHD_OFFSET(JAIZelBasic, mMonsSe, 0x1FBC);
WWHD_OFFSET(JAIZelBasic, mKuroboMotionCount, 0x20AC);
WWHD_OFFSET(JAIZelBasic, mKuroboVoiceCount, 0x20AD);
WWHD_OFFSET(JAIZelBasic, mpKuroboMotionSound, 0x20B0);
WWHD_OFFSET(JAIZelBasic, mpKuroboVoiceSound, 0x20C0);
WWHD_OFFSET(JAIZelBasic, mpCharVoiceSound, 0x20D0);
WWHD_OFFSET(JAIZelBasic, mVoiceIndexHD, 0x20D4);
WWHD_OFFSET(JAIZelBasic, mSystemSoundHD, 0x20E4);
WWHD_OFFSET(JAIZelBasic, mAllocationRecordsHD, 0x20E8);
WWHD_OFFSET(JAIZelBasic, mAllocationRecordIndexHD, 0x2168);
WWHD_OFFSET(JAIZelBasic, mRandomHD, 0x216C);
WWHD_SIZE(JAIZelBasic, 0x21F4);
WWHD_SIZE(AudioCamera, 12);
WWHD_SIZE(VectorWords, 12);
WWHD_SIZE(MonsterSlot, 8);
WWHD_SIZE(AllocationRecord, 8);
WWHD_SIZE(Random, 136);
using GuestByte = be<u8>;
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestHalf = be<u16>;
using SoundPtr = gptr<Sound>;
using ManagerPtr = gptr<void>;
using BasicPtr = gptr<JAIZelBasic>;
struct scene_info_s {
    GuestHalf bgmNum;
    GuestByte dyWave1st, dyWave2nd;  /* was field_0x2: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_1st index (mFirstDynamicSceneWaveIndex); was field_0x3: setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_2nd index (mSecondDynamicSceneWaveIndex) */
};
struct SceneInfoTable {
    scene_info_s entries[256];
};
struct WavePair {
    GuestByte banks[2];
};
struct WaveTable {
    WavePair entries[256];
};
struct CategoryVolumes {
    GuestByte entries[8];
};
struct StaticWaves {
    GuestWord banks[20];
};
struct SpotNames {
    gptr<GuestByte> entries[120];
};
struct AudioSafeStringVtable {
    u8 padding[0x14];
    gabi::gfn assureTermination;
};
struct AudioSafeString {
    gptr<GuestByte> mTextPtr;
    gptr<AudioSafeStringVtable> __vtbl;
};
struct BasicVtable {
    u8 slots[0x10];
};
} // namespace jaizel7

using namespace jaizel7;
using StringPtr = gptr<GuestByte>;
void setParamInterfaceHeapSize(u32);
void setParamSeRegistMax(u32);
void setParamSoundSceneMax(u32);
void setParamSeqControlBufferMax(u32);
void setParamStreamControlBufferMax(u32);
void setParamStreamDecodedBufferBlocks(u32);
void setParamAutoHeapMax(u32);
void setParamStayHeapMax(u32);
void setParamAutoHeapRoomSize(u32);
void setParamStayHeapSize(u32);
void setParamSeqPlayTrackMax(u32);
void setParamSystemTrackMax(u32);
void setParamDistanceMax(f32);
void setParamMaxVolumeDistance(f32);
void setParamMinDistanceVolume(f32);
void setParamSeDolbyCenterValue(u32);
void setParamSeDolbyFrontDistanceMax(f32);
void setParamSeDolbyBehindDistanceMax(f32);
void setParamSeDistanceFxParameter(u32);
void setParamInputGainDown(f32);
void setParamOutputGainUp(f32);
void setParamAudioResPath(u32);
void setParamInitDataFileName(jaizel7::GuestByte *);
void setParamWavePath(jaizel7::GuestByte *);
void setParamSequenceArchivesPath(jaizel7::GuestByte *);
void setParamStreamPath(jaizel7::GuestByte *);
void setParamSequenceArchivesFileName(jaizel7::GuestByte *);
void setParamAudioCameraMax(u32);
void setParamDummyObjectMax(u32);
void setParamDummyObjectLifeTime(u32);
void setParamStreamInsideBufferCut(u32);
void setParamSoundOutputMode(u32);
jaizel7::AudioSafeString *getInitDataPath();
jaizel7::AudioSafeString *getWavePath();
jaizel7::AudioSafeString *getSequencePath();
void hd_releaseSceneStreams();
void hd_selectSceneStream(u32);
void hd_loadSceneStream();
void hd_resetSceneStream();
s32 hd_sceneStreamReady(u32);
void eraseWaveBank(u32, u32);
void eraseWaveHeap(u32, u32);
void resetWaveBank(u32, u32);

u32 getParamSeqPlayTrackMax();
jaizel7::SeqUpdateData *getPlayTrackInfo(u32);
void *hd_streamPlayer(void *);
void hd_stopAllStreams(void *, u32);
s32 isDungeonItem(void *, u32);
s32 getWaveLoadStatus(u32);
void loadWaveBank(u32, u32);
void *allocateAudioObject(u32);
void zeroBasicVector(cXyz *, u32);
void constructBasicRecords(jaizel7::AllocationRecord *, u32, u32, u32);
void seedBasicRandom(jaizel7::Random *, u32);

WWHD_SIZE(jaizel7::SeqUpdateData, 0x4C);
WWHD_SIZE(jaizel7::SoundHead, 2);
WWHD_SIZE(jaizel7::scene_info_s, 4);
WWHD_SIZE(jaizel7::WavePair, 2);
WWHD_SIZE(jaizel7::AudioSafeString, 8);
WWHD_SIZE(jaizel7::AudioSafeStringVtable, 0x18);
WWHD_SIZE(jaizel7::CategoryVolumes, 8);
WWHD_SIZE(jaizel7::StaticWaves, 80);
WWHD_SIZE(jaizel7::SpotNames, 480);
