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
    gptr<cXyz> field_0x0;
};
struct scene_info_s {
    be<u16> bgmNum;
    be<u8> field_0x2, field_0x3;
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
    /* 0x0038 */ be<u32> field_0x0028;
    u8 padding3C[0x34];
    /* 0x0070 */ be<u8> mbLandingDemoStarted;
    /* 0x0071 */ be<u8> field_0x0061;
    u8 padding72[0x3];
    /* 0x0075 */ be<u8> field_0x0065;
    u8 padding76[0x2];
    /* 0x0078 */ gptr<Sound> mpMainBgmSound;
    u8 padding7C[0x8];
    /* 0x0084 */ be<u32> mSubBgmNum;
    /* 0x0088 */ be<u32> mMainBgmNum;
    u8 padding8C[0x4];
    /* 0x0090 */ be<f32> field_0x0080;
    /* 0x0094 */ be<f32> field_0x0084;
    /* 0x0098 */ be<f32> field_0x0088;
    /* 0x009C */ be<f32> field_0x008c;
    /* 0x00A0 */ be<f32> field_0x0090;
    /* 0x00A4 */ be<f32> field_0x0094;
    /* 0x00A8 */ be<f32> field_0x0098;
    /* 0x00AC */ be<f32> field_0x009c;
    u8 paddingB0[0x4];
    /* 0x00B4 */ be<f32> field_0x00a4;
    u8 paddingB8[0x4];
    /* 0x00BC */ be<f32> field_0x00ac;
    /* 0x00C0 */ be<s32> field_0x00b0;
    u8 paddingC4[0x2];
    /* 0x00C6 */ be<u8> mLastMinibossSubBGMType;
    u8 paddingC7[0x4];
    /* 0x00CB */ be<u8> field_0x00bb;
    /* 0x00CC */ be<u8> field_0x00bc;
    u8 paddingCD[0x11];
    /* 0x00DE */ be<u8> field_0x00ce;
    u8 paddingDF[0x18A];
    /* 0x0269 */ be<u8> field_0x01f9;
    /* 0x026A */ be<u8> field_0x01fa;
    /* 0x026B */ be<u8> field_0x01fb;
    /* 0x026C */ be<u8> field_0x01fc;
    /* 0x026D */ be<u8> field_0x01fd;
    /* 0x026E */ be<u8> field_0x01fe;
    /* 0x026F */ be<u8> field_0x01ff;
    /* 0x0270 */ be<u8> field_0x0200;
    u8 padding271[0x1];
    /* 0x0272 */ be<u8> field_0x0202[2];
    u8 padding274[0x1];
    /* 0x0275 */ be<u8> field_0x0205;
    /* 0x0276 */ be<u8> field_0x0206;
    /* 0x0277 */ be<u8> field_0x0207;
    u8 padding278[0x16];
    /* 0x028E */ be<u8> field_0x021e;
    u8 padding28F[0x5];
    /* 0x0294 */ be<s32> field_0x0224;
    u8 padding298[0x5];
    /* 0x029D */ be<u8> field_0x022d;
    u8 padding29E[0xA];
    /* 0x02A8 */ be<u8> mIslandRoomNo;
    /* 0x02A9 */ be<u8> field_0x0239;
    u8 padding2AA[0x1C5E];
    /* 0x1F08 */ be<u32> field_0x1e98;
    u8 padding1F0C[0x98];
    /* 0x1FA4 */ be<u32> field_0x1f34;
    /* 0x1FA8 */ gptr<Sound> field_0x1f38;
    u8 padding1FAC[0x100];
    /* 0x20AC */ be<u8> field_0x203c;
    /* 0x20AD */ be<u8> field_0x203d;
    u8 padding20AE[0x22];
    /* 0x20D0 */ gptr<Sound> field_0x2060;
    /* 0x20D4 */ be<u16> mResetIndexHD;
    u8 padding20D6[2];
};
WWHD_OFFSET(JAIZelBasic, mAudioCamera, 0x0);
WWHD_OFFSET(JAIZelBasic, field_0x0028, 0x38);
WWHD_OFFSET(JAIZelBasic, mbLandingDemoStarted, 0x70);
WWHD_OFFSET(JAIZelBasic, field_0x0061, 0x71);
WWHD_OFFSET(JAIZelBasic, field_0x0065, 0x75);
WWHD_OFFSET(JAIZelBasic, mpMainBgmSound, 0x78);
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
WWHD_OFFSET(JAIZelBasic, field_0x00a4, 0xB4);
WWHD_OFFSET(JAIZelBasic, field_0x00ac, 0xBC);
WWHD_OFFSET(JAIZelBasic, field_0x00b0, 0xC0);
WWHD_OFFSET(JAIZelBasic, mLastMinibossSubBGMType, 0xC6);
WWHD_OFFSET(JAIZelBasic, field_0x00bb, 0xCB);
WWHD_OFFSET(JAIZelBasic, field_0x00bc, 0xCC);
WWHD_OFFSET(JAIZelBasic, field_0x00ce, 0xDE);
WWHD_OFFSET(JAIZelBasic, field_0x01f9, 0x269);
WWHD_OFFSET(JAIZelBasic, field_0x01fa, 0x26A);
WWHD_OFFSET(JAIZelBasic, field_0x01fb, 0x26B);
WWHD_OFFSET(JAIZelBasic, field_0x01fc, 0x26C);
WWHD_OFFSET(JAIZelBasic, field_0x01fd, 0x26D);
WWHD_OFFSET(JAIZelBasic, field_0x01fe, 0x26E);
WWHD_OFFSET(JAIZelBasic, field_0x01ff, 0x26F);
WWHD_OFFSET(JAIZelBasic, field_0x0200, 0x270);
WWHD_OFFSET(JAIZelBasic, field_0x0202, 0x272);
WWHD_OFFSET(JAIZelBasic, field_0x0205, 0x275);
WWHD_OFFSET(JAIZelBasic, field_0x0206, 0x276);
WWHD_OFFSET(JAIZelBasic, field_0x0207, 0x277);
WWHD_OFFSET(JAIZelBasic, field_0x021e, 0x28E);
WWHD_OFFSET(JAIZelBasic, field_0x0224, 0x294);
WWHD_OFFSET(JAIZelBasic, field_0x022d, 0x29D);
WWHD_OFFSET(JAIZelBasic, mIslandRoomNo, 0x2A8);
WWHD_OFFSET(JAIZelBasic, field_0x0239, 0x2A9);
WWHD_OFFSET(JAIZelBasic, field_0x1e98, 0x1F08);
WWHD_OFFSET(JAIZelBasic, field_0x1f34, 0x1FA4);
WWHD_OFFSET(JAIZelBasic, field_0x1f38, 0x1FA8);
WWHD_OFFSET(JAIZelBasic, field_0x203c, 0x20AC);
WWHD_OFFSET(JAIZelBasic, field_0x203d, 0x20AD);
WWHD_OFFSET(JAIZelBasic, field_0x2060, 0x20D0);
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
