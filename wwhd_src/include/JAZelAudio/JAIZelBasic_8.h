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
    gptr<VectorWords> field_0x0;
    gptr<VectorWords> field_0x4;
    gptr<void> field_0x8;
};
struct MonsterSlot {
    be<u32> field_0x00;
    gptr<Sound> field_0x04;
};
struct Random {
    be<u32> field_0x0, field_0x4, field_0x8[32];
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
            gabi::load<u32>(gabi::ea(mAudioCamera.get()) + offsetof(AudioCamera, field_0x8)));
    }
    gptr<AudioCamera> mAudioCamera;
    u8 padding4[0x6C];
    be<u8> mbLandingDemoStarted;
    u8 padding71[0x2];
    be<u8> field_0x0063;
    be<u8> field_0x0064;
    be<u8> field_0x0065;
    u8 padding76[0x2];
    gptr<Sound> mpMainBgmSound;
    gptr<Sound> mpSubBgmSound;
    gptr<Sound> mpStreamBgmSound;
    be<u32> mSubBgmNum;
    be<u32> mMainBgmNum;
    be<u32> mStreamBgmNum;
    be<f32> field_0x0080;
    be<f32> field_0x0084;
    be<f32> field_0x0088;
    be<f32> field_0x008c;
    be<f32> field_0x0090;
    be<f32> field_0x0094;
    be<f32> field_0x0098;
    be<f32> field_0x009c;
    be<f32> field_0x00a0;
    be<f32> field_0x00a4;
    be<f32> field_0x00a8;
    be<f32> field_0x00ac;
    u8 paddingC0[0x5];
    be<u8> mSomeSpecialBGMFlag;
    be<u8> mLastMinibossSubBGMType;
    be<u8> field_0x00b7;
    be<u8> field_0x00b8;
    u8 paddingC9[0x1];
    be<u8> field_0x00ba;
    be<u8> field_0x00bb;
    u8 paddingCC[0x4];
    be<u8> field_0x00c0;
    be<s8> field_0x00c1;
    u8 paddingD2[0x2];
    be<u32> field_0x00c4;
    be<u8> field_0x00c8;
    be<u8> field_0x00c9;
    be<u8> field_0x00ca;
    u8 paddingDB[0x3];
    be<u8> field_0x00ce;
    u8 paddingDF[0x1];
    gptr<VectorWords> field_0x00d0;
    u8 paddingE4[0x18D];
    be<u8> field_0x0201;
    u8 padding272[0x4];
    be<u8> field_0x0206;
    be<u8> field_0x0207;
    u8 padding278[0x1C];
    be<u32> field_0x0224;
    u8 padding298[0x5];
    be<u8> field_0x022d;
    u8 padding29E[0x1D0E];
    be<u8> field_0x1f3c;
    be<u8> mIsSailing;
    u8 padding1FAE[0x2];
    be<f32> field_0x1f40;
    gptr<Sound> field_0x1f44;
    gptr<Sound> field_0x1f48;
    MonsterSlot field_0x1f4c[30];
    u8 padding20AC[0x24];
    gptr<Sound> field_0x2060;
    be<u16> mVoiceIndexHD;
    u8 padding20D6[0x2];
    VectorWords mMessagePositionHD;
    u8 padding20E4[0x88];
    Random mVoiceRandomHD;
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, field_0x0063, 0x73);
WWHD_OFFSET(JAIZelBasic, field_0x0064, 0x74);
WWHD_OFFSET(JAIZelBasic, field_0x0065, 0x75);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mpSubBgmSound, 0x7C);
WWHD_OFFSET(JAIZelBasic, mpStreamBgmSound, 0x80);
WWHD_OFFSET(JAIZelBasic, mSubBgmNum, 0x84);
WWHD_OFFSET(JAIZelBasic, mMainBgmNum, 0x88);
WWHD_OFFSET(JAIZelBasic, mStreamBgmNum, 0x8C);
WWHD_OFFSET(JAIZelBasic, field_0x0080, 0x90);
WWHD_OFFSET(JAIZelBasic, field_0x0084, 0x94);
WWHD_OFFSET(JAIZelBasic, field_0x0088, 0x98);
WWHD_OFFSET(JAIZelBasic, field_0x008c, 0x9C);
WWHD_OFFSET(JAIZelBasic, field_0x0090, 0xA0);
WWHD_OFFSET(JAIZelBasic, field_0x0094, 0xA4);
WWHD_OFFSET(JAIZelBasic, field_0x0098, 0xA8);
WWHD_OFFSET(JAIZelBasic, field_0x009c, 0xAC);
WWHD_OFFSET(JAIZelBasic, field_0x00a0, 0xB0);
WWHD_OFFSET(JAIZelBasic, field_0x00a4, 0xB4);
WWHD_OFFSET(JAIZelBasic, field_0x00a8, 0xB8);
WWHD_OFFSET(JAIZelBasic, field_0x00ac, 0xBC);
WWHD_OFFSET(JAIZelBasic, mSomeSpecialBGMFlag, 0xC5);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, field_0x00b7, 0xC7);
WWHD_OFFSET(JAIZelBasic, field_0x00b8, 0xC8);
WWHD_OFFSET(JAIZelBasic, field_0x00ba, 0xCA);
WWHD_OFFSET(JAIZelBasic, field_0x00bb, 0xCB);
WWHD_OFFSET(JAIZelBasic, field_0x00c0, 0xD0);
WWHD_OFFSET(JAIZelBasic, field_0x00c1, 0xD1);
WWHD_OFFSET(JAIZelBasic, field_0x00c4, 0xD4);
WWHD_OFFSET(JAIZelBasic, field_0x00c8, 0xD8);
WWHD_OFFSET(JAIZelBasic, field_0x00c9, 0xD9);
WWHD_OFFSET(JAIZelBasic, field_0x00ca, 0xDA);
WWHD_OFFSET(JAIZelBasic, field_0x00ce, 0xDE);
WWHD_OFFSET(JAIZelBasic, field_0x00d0, 0xE0);
WWHD_OFFSET(JAIZelBasic, field_0x0201, 0x271);
WWHD_OFFSET(JAIZelBasic, field_0x0206, 0x276);
WWHD_OFFSET(JAIZelBasic, field_0x0207, 0x277);
WWHD_OFFSET(JAIZelBasic, field_0x0224, 0x294);
WWHD_OFFSET(JAIZelBasic, field_0x022d, 0x29D);
WWHD_OFFSET(JAIZelBasic, field_0x1f3c, 0x1FAC);
WWHD_OFFSET(JAIZelBasic, mIsSailing, 0x1FAD);
WWHD_OFFSET(JAIZelBasic, field_0x1f40, 0x1FB0);
WWHD_OFFSET(JAIZelBasic, field_0x1f44, 0x1FB4);
WWHD_OFFSET(JAIZelBasic, field_0x1f48, 0x1FB8);
WWHD_OFFSET(JAIZelBasic, field_0x1f4c, 0x1FBC);
WWHD_OFFSET(JAIZelBasic, field_0x2060, 0x20D0);
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
