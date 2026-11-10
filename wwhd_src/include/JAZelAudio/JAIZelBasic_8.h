/** JAIZelBasic part8 private layout.
 * HD: leading-vtable removal and expanded SE tables shift fields by GC+0x10/+0x70.
 * Numeric field names retain tww's names; HD additions are named by their use. */
#pragma once
#include "JAZelAudio/JAIZelSound.h"
#include "gc_api/d/d_com_inf_game.h"
namespace jaizel8 {
struct Track {
    void writePortApp(u32, u32);
    void readPortApp(u32, be<u16> *);
};
struct Sequence {
    u8 padding[0x1360];
    Track mRootTrack;
};
struct Sound : ::JAISound {
    void start(u32);
    void stop(u32);
    void setVolume(f32, u32, u32);
    void setSeqInterVolume(u32, f32, u32);
    void setPan(f32, u32, u32);
    void setDolby(f32, u32, u32);
    void setPortData(u32, u32);
    void setTrackVolume(u32, f32, u32);
    Sequence *getSeqParameter();
};
struct VectorWords {
    be<u32> w[3];
};
struct AudioCamera {
    gptr<VectorWords> position;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos) */
    gptr<VectorWords> previousPosition;  /* was field_0x4: JAInter::Camera::previousPosition (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 points it at mCameraOldPos */
    gptr<void> matrix;  /* was field_0x8: JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera matrix */
};
struct MonsterSlot {
    be<u32> mActorId;  /* was field_0x00: monsSeStart 020299D8: the actor id argument (fopAcM_monsSeStart passes the actor's id) owning the slot; -1 free (monsSeInit 0202573C) */
    gptr<Sound> mpSound;  /* was field_0x04: monsSeStart 020299D8: handle of the slot's sound (stopped when the actor starts a new one) */
};
struct Random {
    be<u32> mSeed, mValue, mShuffle[32];  /* was field_0x0: JMath::TRandom_enough_::mSeed (JSystem/JMath/random.h): mRandomHD is a TRandom_enough_ (createJAIZelBasic seeds it, 0x88 bytes); was field_0x4: JMath::TRandom_enough_::mValue (JSystem/JMath/random.h); was field_0x8: JMath::TRandom_enough_::mShuffle (JSystem/JMath/random.h) */
    f32 get();
};
struct JAIZelBasic {
    void talkIn();
    void bgmStreamPlay();
    void changeBgmStatus(u32 v);
    void changeSubBgmStatus(u32 v);
    void cbPracticePlay(VectorWords *pos);
    void cbPracticeStop();
    void bgmMuteMtDragon();
    void stSkyCloisters();
    void bgmAllMute(u32 fade);
    void stWaterLevelUp();
    void stWaterLevelDown();
    void taktModeMute();
    void taktModeMuteOff();
    void bgmNowBattle(f32 dist);
    void bgmNowKaitengiri();
    u32 getRandomU32(u32 n);
    void bgmHitSound(u32 kind);
    void bgmSetSwordUsing(u32 v);
    void prepareLandingDemo(u32 v);
    void startLandingDemo();
    void endLandingDemo();
    void seStopActor(VectorWords *pos, u32 id);
    void seDeleteObject(VectorWords *pos);
    void charVoicePlay(u32 a, u32 b, VectorWords *pos, u32 reverb);
    void messageSePlay(u32 msg, VectorWords *pos, u32 reverb);
    u32 getLinkVoiceVowel(u32 n);
    void linkVoiceStart(u32 n, VectorWords *pos, u32 vowel, u32 reverb);
    void shipCruiseSePlay(VectorWords *pos, f32 speed);
    void setShipSailState(u32 v);
    void monsSeStart(u32 id, VectorWords *pos, u32 obj, u32 info, u32 reverb);
    f32 getMapInfoFxParameter(u32 n);
    u32 getMapInfoGround(u32 n);
    s32 isDemo();
    void bgmStart(u32, u32, u32);
    void bgmStop(u32, u32);
    void bgmMute(gptr<Sound> *, u32, u32, u32);
    void hdStreamVolume(u32, f32);
    void enemyNearBy();
    void mbossBgmNearByProcess(f32);
    s32 checkLinkOnSea();
    s32 checkLinkOnBoardSea();
    void kuroboVoicePlay(u32, VectorWords *, u32);
    u32 seStart(u32, VectorWords *, u32, u32, f32, f32, f32, f32, u32);
    void startSoundVec(u32, gptr<Sound> *, VectorWords *, u32, u32, u32);
    void setSeCategoryVolume(u32, u32);
    void stopAllSound(VectorWords *);
    void stopAllSoundID(u32);
    void stopActorSound(u32, VectorWords *);
    void deleteObject(VectorWords *);
    f32 calcPosVolume(VectorWords *, f32);
    f32 calcPosPanLR(VectorWords *);
    f32 calcPosPanSR(VectorWords *, f32);
    u32 randomIndex();
    // HD: preserve guest-null camera reads.
    VectorWords *cameraPosition() {
        return gabi::at<VectorWords>(gabi::load<u32>(gabi::ea(mAudioCamera.get())));
    }
    void *cameraMatrix() {
        return gabi::at<void>(
            gabi::load<u32>(gabi::ea(mAudioCamera.get()) + offsetof(AudioCamera, matrix)));
    }
    gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x6C];
    be<u8> mbLandingDemoStarted;
    u8 padding71[0x2];
    be<u8> mBgmDisabled;  /* was field_0x0063: JAIZelBasic::mBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTalkInBlocked;  /* was field_0x0064: JAIZelBasic::mTalkInBlocked (JAZelAudio/JAIZelBasic.h) */
    be<u8> mMtDragonMuteRequest;  /* was field_0x0065: JAIZelBasic::mMtDragonMuteRequest (JAZelAudio/JAIZelBasic.h) */
    u8 padding76[0x2];
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
    be<u8> mSkyCloistersTimer;  /* was field_0x00b7: JAIZelBasic::mSkyCloistersTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mStreamBgmStarted;  /* was field_0x00b8: JAIZelBasic::mStreamBgmStarted (JAZelAudio/JAIZelBasic.h) */
    u8 paddingC9[0x1];
    be<u8> mTetraFlyStreamStarted;  /* was field_0x00ba: JAIZelBasic::mTetraFlyStreamStarted (JAZelAudio/JAIZelBasic.h) */
    be<u8> mFanfareResumePending;  /* was field_0x00bb: JAIZelBasic::mFanfareResumePending (JAZelAudio/JAIZelBasic.h) */
    u8 paddingCC[0x4];
    be<u8> mBattleBgmFadeDelay;  /* was field_0x00c0: JAIZelBasic::mBattleBgmFadeDelay (JAZelAudio/JAIZelBasic.h) */
    be<s8> mEnemyNearbyOn;  /* was field_0x00c1: JAIZelBasic::mEnemyNearbyOn (JAZelAudio/JAIZelBasic.h) */
    u8 paddingD2[0x2];
    be<u32> mEnemyNearbyFadeTimer;  /* was field_0x00c4: JAIZelBasic::mEnemyNearbyFadeTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mKaitengiriTimer;  /* was field_0x00c8: JAIZelBasic::mKaitengiriTimer (JAZelAudio/JAIZelBasic.h) */
    be<u8> mSwordUsing;  /* was field_0x00c9: JAIZelBasic::mSwordUsing (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBattleBgmDisabled;  /* was field_0x00ca: JAIZelBasic::mBattleBgmDisabled (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDB[0x3];
    be<u8> mBattleBgmHoldTimer;  /* was field_0x00ce: JAIZelBasic::mBattleBgmHoldTimer (JAZelAudio/JAIZelBasic.h) */
    u8 paddingDF[0x1];
    gptr<VectorWords> mpCbPracticePos;  /* was field_0x00d0: JAIZelBasic::mpCbPracticePos (JAZelAudio/JAIZelBasic.h) */
    u8 paddingE4[0x18D];
    be<u8> mAllSeMuted;  /* was field_0x0201: JAIZelBasic::mAllSeMuted (JAZelAudio/JAIZelBasic.h) */
    u8 padding272[0x4];
    be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTaktSeTimer;  /* was field_0x0207: JAIZelBasic::mTaktSeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 padding278[0x1C];
    be<u32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    u8 padding298[0x5];
    be<u8> mSceneActive;  /* was field_0x022d: JAIZelBasic::mSceneActive (JAZelAudio/JAIZelBasic.h) */
    u8 padding29E[0x1D0E];
    be<u8> mIslandLinkBgm;  /* was field_0x1f3c: JAIZelBasic::mIslandLinkBgm (JAZelAudio/JAIZelBasic.h) */
    be<u8> mIsSailing;
    u8 padding1FAE[0x2];
    be<f32> mShipSpeed;  /* was field_0x1f40: JAIZelBasic::mShipSpeed (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpShipCruiseSound;  /* was field_0x1f44: JAIZelBasic::mpShipCruiseSound (JAZelAudio/JAIZelBasic.h) */
    gptr<Sound> mpShipWaveSound;  /* was field_0x1f48: JAIZelBasic::mpShipWaveSound (JAZelAudio/JAIZelBasic.h) */
    MonsterSlot mMonsSe[30];  /* was field_0x1f4c: JAIZelBasic::mMonsSe (JAZelAudio/JAIZelBasic.h) */
    u8 padding20AC[0x24];
    gptr<Sound> mpCharVoiceSound;  /* was field_0x2060: JAIZelBasic::mpCharVoiceSound (JAZelAudio/JAIZelBasic.h) */
    be<u16> mVoiceIndexHD;
    u8 padding20D6[0x2];
    VectorWords mMessagePositionHD;
    u8 padding20E4[0x88];
    Random mVoiceRandomHD;
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, mBgmDisabled, 0x73);
WWHD_OFFSET(JAIZelBasic, mTalkInBlocked, 0x74);
WWHD_OFFSET(JAIZelBasic, mMtDragonMuteRequest, 0x75);
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
WWHD_OFFSET(JAIZelBasic, mSkyCloistersTimer, 0xC7);
WWHD_OFFSET(JAIZelBasic, mStreamBgmStarted, 0xC8);
WWHD_OFFSET(JAIZelBasic, mTetraFlyStreamStarted, 0xCA);
WWHD_OFFSET(JAIZelBasic, mFanfareResumePending, 0xCB);
WWHD_OFFSET(JAIZelBasic, mBattleBgmFadeDelay, 0xD0);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyOn, 0xD1);
WWHD_OFFSET(JAIZelBasic, mEnemyNearbyFadeTimer, 0xD4);
WWHD_OFFSET(JAIZelBasic, mKaitengiriTimer, 0xD8);
WWHD_OFFSET(JAIZelBasic, mSwordUsing, 0xD9);
WWHD_OFFSET(JAIZelBasic, mBattleBgmDisabled, 0xDA);
WWHD_OFFSET(JAIZelBasic, mBattleBgmHoldTimer, 0xDE);
WWHD_OFFSET(JAIZelBasic, mpCbPracticePos, 0xE0);
WWHD_OFFSET(JAIZelBasic, mAllSeMuted, 0x271);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mTaktSeTimer, 0x277);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mSceneActive, 0x29D);
WWHD_OFFSET(JAIZelBasic, mIslandLinkBgm, 0x1FAC);
WWHD_OFFSET(JAIZelBasic, mIsSailing, 0x1FAD);
WWHD_OFFSET(JAIZelBasic, mShipSpeed, 0x1FB0);
WWHD_OFFSET(JAIZelBasic, mpShipCruiseSound, 0x1FB4);
WWHD_OFFSET(JAIZelBasic, mpShipWaveSound, 0x1FB8);
WWHD_OFFSET(JAIZelBasic, mMonsSe, 0x1FBC);
WWHD_OFFSET(JAIZelBasic, mpCharVoiceSound, 0x20D0);
WWHD_OFFSET(JAIZelBasic, mVoiceIndexHD, 0x20D4);
WWHD_OFFSET(JAIZelBasic, mMessagePositionHD, 0x20D8);
WWHD_OFFSET(JAIZelBasic, mVoiceRandomHD, 0x216C);
WWHD_SIZE(JAIZelBasic, 0x21F4);
WWHD_SIZE(VectorWords, 12);
WWHD_SIZE(AudioCamera, 12);
WWHD_SIZE(MonsterSlot, 8);
WWHD_SIZE(Random, 136);
WWHD_OFFSET(Sequence, mRootTrack, 0x1360);
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
using GuestByte = be<u8>;
struct MessageVoice {
    be<u16> character, sound;
};
struct MessageVoiceTable {
    MessageVoice entries[0x118];
};
struct VoiceEntry {
    GuestByte sound, vowel;
};
struct LinkVoice {
    VoiceEntry entries[4];
};
// HD: full unsigned input indexes are not clamped to the known table count.
struct LinkVoiceTable {
    LinkVoice entries[1];
};
// HD: table offset arithmetic wraps in32bits for every incoming unsigned index.
inline LinkVoice *voiceAt(LinkVoiceTable *table, u32 index) {
    return gabi::at<LinkVoice>(gabi::ea(table) + index * u32(sizeof(LinkVoice)));
}
struct SequenceTrack {
    u8 padding[0x1360];
    Track mRootTrack;
};
inline Track *rootTrack(Sequence *sequence) {
    return gabi::at<Track>(gabi::ea(sequence) + offsetof(Sequence, mRootTrack));
}
inline bool audioPlayerPresent() {
    return gabi::load<u32>(dComIfGp_ea() + PLAY_OFF(mpPlayerPtr[0])) != 0;
}
struct AllocationRecord {
    GuestWord count;
    gptr<Sound> sound;
};
struct HeaderStatics {
    GuestFloat low, high;
    GuestByte byte0, byte1;
    u8 padding[2];
    GuestWord words[4];
};
struct DestructionRecord {
    GuestWord words[3];
};
struct FloatRange {
    GuestFloat low, high;
};
} // namespace jaizel8
void jaizelTransform(void *, jaizel8::VectorWords *, jaizel8::VectorWords *);
f32 getParamDistanceMax();
f32 std_sqrtf(f32);
void *allocateBasicRecord(u32);
void deleteBasicObject(void *);
u32 checkBasicPointer(void *);
void registerBasicGlobal(jaizel8::DestructionRecord *);
void constructBasicByte0(jaizel8::GuestByte *);
void constructBasicByte1(jaizel8::GuestByte *);

WWHD_SIZE(jaizel8::AllocationRecord, 8);
WWHD_SIZE(jaizel8::HeaderStatics, 28);
WWHD_SIZE(jaizel8::DestructionRecord, 12);
WWHD_SIZE(jaizel8::FloatRange, 8);
