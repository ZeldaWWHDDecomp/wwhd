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
    gptr<cXyz> field_0x0;
};
struct isle_area_s {
    be<f32> field_0x0, field_0x4, field_0x8, field_0xC;
};
struct scene_info_s {
    be<u16> bgmNum;
    be<u8> field_0x2, field_0x3;
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
    /* 0x031 */ be<u8> field_0x0021;
    u8 padding32[0xA];
    /* 0x03C */ be<u8> mHour;
    /* 0x03D */ be<u8> mMinute;
    u8 padding3E[0x2];
    /* 0x040 */ be<u16> field_0x0030;
    /* 0x042 */ be<u8> field_0x0032;
    /* 0x043 */ be<u8> field_0x0033;
    /* 0x044 */ be<s32> field_0x0034;
    u8 padding48[0x8];
    /* 0x050 */ be<u8> field_0x0040;
    u8 padding51[0x1F];
    /* 0x070 */ be<u8> mbLandingDemoStarted;
    u8 padding71[0x1];
    /* 0x072 */ be<u8> field_0x0062;
    u8 padding73[0x5];
    /* 0x078 */ gptr<Sound> mpMainBgmSound;
    /* 0x07C */ gptr<Sound> mpSubBgmSound;
    u8 padding80[0x4];
    /* 0x084 */ be<u32> mSubBgmNum;
    /* 0x088 */ be<u32> mMainBgmNum;
    u8 padding8C[0x4];
    /* 0x090 */ be<f32> field_0x0080;
    /* 0x094 */ be<f32> field_0x0084;
    /* 0x098 */ be<f32> field_0x0088;
    /* 0x09C */ be<f32> field_0x008c;
    /* 0x0A0 */ be<f32> field_0x0090;
    /* 0x0A4 */ be<f32> field_0x0094;
    /* 0x0A8 */ be<f32> field_0x0098;
    /* 0x0AC */ be<f32> field_0x009c;
    u8 paddingB0[0xC];
    /* 0x0BC */ be<f32> field_0x00ac;
    u8 paddingC0[0x7];
    /* 0x0C7 */ be<u8> field_0x00b7;
    u8 paddingC8[0x5];
    /* 0x0CD */ be<u8> field_0x00bd;
    u8 paddingCE[0x2];
    /* 0x0D0 */ be<u8> field_0x00c0;
    /* 0x0D1 */ be<u8> field_0x00c1;
    u8 paddingD2[0x2];
    /* 0x0D4 */ be<u32> field_0x00c4;
    /* 0x0D8 */ be<u8> field_0x00c8;
    /* 0x0D9 */ be<u8> field_0x00c9;
    /* 0x0DA */ be<u8> field_0x00ca;
    /* 0x0DB */ be<u8> field_0x00cb;
    /* 0x0DC */ be<u8> field_0x00cc;
    u8 paddingDD[0x18B];
    /* 0x268 */ be<u8> field_0x01f8;
    u8 padding269[0xC];
    /* 0x275 */ be<u8> field_0x0205;
    /* 0x276 */ be<u8> field_0x0206;
    u8 padding277[0x1D];
    /* 0x294 */ be<u32> field_0x0224;
    /* 0x298 */ be<u32> mNextSceneBgmId;
    u8 padding29C[0x1];
    /* 0x29D */ be<u8> field_0x022d;
    /* 0x29E */ be<u8> field_0x022e;
    u8 padding29F[0x9];
    /* 0x2A8 */ be<u8> mIslandRoomNo;
    u8 padding2A9[3];
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, field_0x0021, 0x31);
WWHD_OFFSET(JAIZelBasic, mHour, 0x3C);
WWHD_OFFSET(JAIZelBasic, mMinute, 0x3D);
WWHD_OFFSET(JAIZelBasic, field_0x0030, 0x40);
WWHD_OFFSET(JAIZelBasic, field_0x0032, 0x42);
WWHD_OFFSET(JAIZelBasic, field_0x0033, 0x43);
WWHD_OFFSET(JAIZelBasic, field_0x0034, 0x44);
WWHD_OFFSET(JAIZelBasic, field_0x0040, 0x50);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, field_0x0062, 0x72);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
WWHD_OFFSET(JAIZelBasic, mpSubBgmSound, 0x7C);
WWHD_OFFSET(JAIZelBasic, mSubBgmNum, 0x84);
WWHD_OFFSET(JAIZelBasic, mMainBgmNum, 0x88);
WWHD_OFFSET(JAIZelBasic, field_0x0080, 0x90);
WWHD_OFFSET(JAIZelBasic, field_0x0084, 0x94);
WWHD_OFFSET(JAIZelBasic, field_0x0088, 0x98);
WWHD_OFFSET(JAIZelBasic, field_0x008c, 0x9C);
WWHD_OFFSET(JAIZelBasic, field_0x0090, 0xA0);
WWHD_OFFSET(JAIZelBasic, field_0x0094, 0xA4);
WWHD_OFFSET(JAIZelBasic, field_0x0098, 0xA8);
WWHD_OFFSET(JAIZelBasic, field_0x009c, 0xAC);
WWHD_OFFSET(JAIZelBasic, field_0x00ac, 0xBC);
WWHD_OFFSET(JAIZelBasic, field_0x00b7, 0xC7);
WWHD_OFFSET(JAIZelBasic, field_0x00bd, 0xCD);
WWHD_OFFSET(JAIZelBasic, field_0x00c0, 0xD0);
WWHD_OFFSET(JAIZelBasic, field_0x00c1, 0xD1);
WWHD_OFFSET(JAIZelBasic, field_0x00c4, 0xD4);
WWHD_OFFSET(JAIZelBasic, field_0x00c8, 0xD8);
WWHD_OFFSET(JAIZelBasic, field_0x00c9, 0xD9);
WWHD_OFFSET(JAIZelBasic, field_0x00ca, 0xDA);
WWHD_OFFSET(JAIZelBasic, field_0x00cb, 0xDB);
WWHD_OFFSET(JAIZelBasic, field_0x00cc, 0xDC);
WWHD_OFFSET(JAIZelBasic, field_0x01f8, 0x268);
WWHD_OFFSET(JAIZelBasic, field_0x0205, 0x275);
WWHD_OFFSET(JAIZelBasic, field_0x0206, 0x276);
WWHD_OFFSET(JAIZelBasic, field_0x0224, 0x294);
WWHD_OFFSET(JAIZelBasic, mNextSceneBgmId, 0x298);
WWHD_OFFSET(JAIZelBasic, field_0x022d, 0x29D);
WWHD_OFFSET(JAIZelBasic, field_0x022e, 0x29E);
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
