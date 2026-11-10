/* Unit-private ambient sound layout. Unknown GC fields retain tww names.
 * HD: arrays reconstructed from verified accesses; no shared declarations changed. */
#pragma once
#include "bindings.h"
struct JAISound;
struct AtmosCamera {
    gptr<cXyz> position;  /* was field_0x0: JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); seaEnvSePlay 0201C524 places the four sea points around it */
    be<u32> mUnaccessed;
    gptr<void> matrix;  /* was field_0x8: JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); seaEnvSePlay 0201C524 / waterfallSePlay 0201CF54 transform by it */
};
WWHD_OFFSET(AtmosCamera, matrix, 8);
struct AtmosSafeString;
struct AtmosSafeStringVtable {
    u8 mUnaccessed[0x14];
    gfn cstr;
};
struct AtmosSafeString {
    gptr<be<u8>> mTextPtr;
    gptr<AtmosSafeStringVtable> __vtbl;
};
class JAIZelBasic {
  public:
    gptr<AtmosCamera> mAudioCamera;
    u8 mUnaccessed4[0x2C];
    be<u8> mIsMenuIn;  /* was field_0x0020: JAIZelBasic::mIsMenuIn (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed31[0x33];
    cXyz mCameraPolygonPos;  /* was field_0x0054: JAIZelBasic::mCameraPolygonPos (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed70[0x38];
    be<f32> mMainBgmTaktVolume;  /* was field_0x0098: JAIZelBasic::mMainBgmTaktVolume (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessedAC[0x1BC];
    be<u8> mNoCameraTimer;  /* was field_0x01f8: JAIZelBasic::mNoCameraTimer (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed269[0x8];
    be<u8> mAllSeMuted;  /* was field_0x0201: JAIZelBasic::mAllSeMuted (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed272[0x2];
    be<u8> mSeaEnvSeOff;  /* was field_0x0204: JAIZelBasic::mSeaEnvSeOff (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed275[0x1];
    be<u8> mFanfareActive;  /* was field_0x0206: JAIZelBasic::mFanfareActive (JAZelAudio/JAIZelBasic.h) */
    be<u8> mTaktSeTimer;  /* was field_0x0207: JAIZelBasic::mTaktSeTimer (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed278[0x10];
    // HD: the sea-group byte aliases the low byte of the GC field_0x0218 word.
    union {
 be<u32> mCameraMapInfo;  /* was field_0x0218: JAIZelBasic::mCameraMapInfo (JAZelAudio/JAIZelBasic.h) */
 struct {u8 mUnaccessedGroup[3]; be<u8> mSeaEnvGroupHD;} mGroupBytes;
 };
    u8 mUnaccessed28C[0x8];
    be<s32> mSceneNo;  /* was field_0x0224: JAIZelBasic::mSceneNo (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed298[0x5];
    be<u8> mSceneActive;  /* was field_0x022d: JAIZelBasic::mSceneActive (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed29E[0x1352];
    cXyz mSeaEnvPos[64];
    cXyz mSeaEnvCameraPos[64];
    be<s32> mSeaEnvPosCount;  /* was field_0x1b80: JAIZelBasic::mSeaEnvPosCount (JAZelAudio/JAIZelBasic.h) */
    cXyz mSeaShorePos;
    // HD: 48 stored entries; registration still compares against 64 before resetting to 47.
    cXyz mRiverPos[48];
    be<s32> mRiverPosCount;  /* was field_0x1dd0: JAIZelBasic::mRiverPosCount (JAZelAudio/JAIZelBasic.h) */
    be<u8> mBstHoriSeOn;  /* was field_0x1dd4: JAIZelBasic::mBstHoriSeOn (JAZelAudio/JAIZelBasic.h) */
    u8 mUnaccessed1E45[0x3];
    cXyz mWaterfallPos[16];
    be<s32> mWaterfallPosCount;  /* was field_0x1e98: JAIZelBasic::mWaterfallPosCount (JAZelAudio/JAIZelBasic.h) */
    cXyz mWindowPos[3];
    be<s32> mWindowPosCount;  /* was field_0x1ec0: JAIZelBasic::mWindowPosCount (JAZelAudio/JAIZelBasic.h) */
    gptr<JAISound> mpSeaEnvSoundL;  /* was field_0x1ec4: JAIZelBasic::mpSeaEnvSoundL (JAZelAudio/JAIZelBasic.h) */
    gptr<JAISound> mpSeaEnvSoundR;  /* was field_0x1ec8: JAIZelBasic::mpSeaEnvSoundR (JAZelAudio/JAIZelBasic.h) */
    gptr<JAISound> mpSeaEnvSoundS;  /* was field_0x1ecc: JAIZelBasic::mpSeaEnvSoundS (JAZelAudio/JAIZelBasic.h) */
    gptr<JAISound> mpSeaShoreSound;  /* was field_0x1ed0: JAIZelBasic::mpSeaShoreSound (JAZelAudio/JAIZelBasic.h) */
    f32 calcPosPanLR(cXyz* pos);
    f32 calcPosPanSR(cXyz* pos, f32 scale);
    f32 calcPosVolume(cXyz* pos, f32 scale);
    void initSeaEnvPos();
    // HD: the GC declarations say void; these retain the verified return registers.
    s32 registSeaEnvPos(cXyz* pos);
    void seaEnvSePlay(u32 type, u32 reverb);
    void seaShoreSE(u32 type, cXyz* pos, u32 keep, u32 reverb);
    void initRiverPos();
    s32 registRiverPos(cXyz* pos);
    void riverSePlay(u32 type, u32 reverb);
    void waterfallSePlay(u32 type, cXyz* pos, u32 reverb);
    void initWindowPos();
    s32 registWindowPos(cXyz* pos);
    void rainPlay(s32 type);
};
WWHD_OFFSET(JAIZelBasic, mIsMenuIn, 0x30);
WWHD_OFFSET(JAIZelBasic, mCameraPolygonPos, 0x64);
WWHD_OFFSET(JAIZelBasic, mMainBgmTaktVolume, 0xA8);
WWHD_OFFSET(JAIZelBasic, mNoCameraTimer, 0x268);
WWHD_OFFSET(JAIZelBasic, mAllSeMuted, 0x271);
WWHD_OFFSET(JAIZelBasic, mSeaEnvSeOff, 0x274);
WWHD_OFFSET(JAIZelBasic, mFanfareActive, 0x276);
WWHD_OFFSET(JAIZelBasic, mTaktSeTimer, 0x277);
WWHD_OFFSET(JAIZelBasic, mCameraMapInfo, 0x288);
WWHD_OFFSET(JAIZelBasic, mGroupBytes.mSeaEnvGroupHD, 0x28B);
WWHD_OFFSET(JAIZelBasic, mSceneNo, 0x294);
WWHD_OFFSET(JAIZelBasic, mSceneActive, 0x29D);
WWHD_OFFSET(JAIZelBasic, mSeaEnvPos, 0x15F0);
WWHD_OFFSET(JAIZelBasic, mSeaEnvCameraPos, 0x18F0);
WWHD_OFFSET(JAIZelBasic, mSeaEnvPosCount, 0x1BF0);
WWHD_OFFSET(JAIZelBasic, mSeaShorePos, 0x1BF4);
WWHD_OFFSET(JAIZelBasic, mRiverPos, 0x1C00);
WWHD_OFFSET(JAIZelBasic, mRiverPosCount, 0x1E40);
WWHD_OFFSET(JAIZelBasic, mBstHoriSeOn, 0x1E44);
WWHD_OFFSET(JAIZelBasic, mWaterfallPos, 0x1E48);
WWHD_OFFSET(JAIZelBasic, mWaterfallPosCount, 0x1F08);
WWHD_OFFSET(JAIZelBasic, mWindowPos, 0x1F0C);
WWHD_OFFSET(JAIZelBasic, mWindowPosCount, 0x1F30);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundL, 0x1F34);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundR, 0x1F38);
WWHD_OFFSET(JAIZelBasic, mpSeaEnvSoundS, 0x1F3C);
WWHD_OFFSET(JAIZelBasic, mpSeaShoreSound, 0x1F40);
WWHD_SIZE(JAIZelBasic, 0x1F44);
enum AtmosSoundId : u32 {
    JA_SE_ATM_RAIN_1 = 0x105B,
    JA_SE_ATM_WATERFALL_M = 0x1066,
    JA_SE_ATM_WATERFALL_S = 0x1067,
    JA_SE_ATM_WATERFALL_M_RIV = 0x107C,
    JA_SE_ATM_RAIN_ROOM = 0x1084,
    JA_SE_ATM_RAIN_HARD = 0x1085,
    JA_SE_ATM_RAIN_ROOM_HARD = 0x1086,
    JA_SE_ATM_WATERFALL_M_LPF = 0x10A7,
    JA_SE_ATM_RAIN_LPF = 0x10A8,
    JA_SE_ATM_RIVER_M = 0x3033,
    JA_SE_ATM_RIVER_S = 0x3034,
    JA_SE_ATM_RIVER_S_ISLE = 0x303A,
    JA_SE_SEA_ALL_NORM_L = 0x7000,
    JA_SE_SEA_ALL_NORM_R = 0x7001,
    JA_SE_SEA_ALL_NORM_SR = 0x7002,
    JA_SE_SEASHORE_NORM = 0x7003,
    JA_SE_SEASHORE_QUAY = 0x7004,
    JA_SE_SEASHORE_CLIFF = 0x7005,
    JA_SE_MAGMA_L = 0x7006,
    JA_SE_MAGMA_R = 0x7007,
    JA_SE_MAGMA_SR = 0x7008,
    JA_SE_CM_BST_HORI = 0x7035,
    JA_STRM_DEMO_33_01 = 0xC0000023,
    JA_STRM_DEMO_31_01 = 0xC000003C,
};

u32 JAIZel_seStart(JAIZelBasic* basic, u32 id, cXyz* pos, u32 info, s32 reverb, u32 flag,
                   f32 volume, f32 pitch, f32 pan, f32 fx);
s32 JAIZel_checkStreamPlaying(JAIZelBasic* basic, u32 id);
s32 JAIZel_checkPlayingStreamBgmFlag(JAIZelBasic* basic, u32 id);
void JAIBasic_startSoundVec(JAIZelBasic* basic, u32 id, gptr<JAISound>* handle, cXyz* pos, u32 a,
                            u32 b, u32 c);
void JAISound_setPan(JAISound* sound, u32 time, u32 kind, f32 pan);
void JAISound_setDolby(JAISound* sound, u32 time, u32 kind, f32 dolby);
void JAISound_stop(JAISound* sound, u32 time);
void JAISound_setVolume(JAISound* sound, u32 time, u32 kind, f32 volume);
void JAISound_setPortData(JAISound* sound, u32 port, u32 value);
void JAIZel_multVec(void* matrix, cXyz* in, cXyz* out);
f32 JAIZel_sqrt(f32 value);
void JAIZel_delete(void* pointer);
f32 JAIGlobalParameter_getParamDistanceMax();
f32 JAIGlobalParameter_getParamMaxVolumeDistance();
f32 JAIGlobalParameter_getParamMinDistanceVolume();
f32 JAIGlobalParameter_getParamSeDolbyCenterValue();
f32 JAIGlobalParameter_getParamSeDolbyFrontDistanceMax();
f32 JAIGlobalParameter_getParamSeDolbyBehindDistanceMax();
