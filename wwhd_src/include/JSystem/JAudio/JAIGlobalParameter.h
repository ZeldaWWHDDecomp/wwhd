/* JAIGlobalParameter API and private HD audio object fields.
 * HD: verified signatures retain f64 values and full u32 pointer/byte/halfword registers. */
#pragma once
#include "bindings.h"
struct JAIBasicView {
    u8 _00[0x11];
    /* 0x11 */ be<u8> mOutputMode;  /* was field_0xd (GameCube name): JAIBasic::mOutputMode (JSystem/JAudio/JAIBasic.h, +0x11); setParamSoundOutputMode 02804508 stores it */
    /* 0x12 */ be<u16> mFlags;  /* was field_0xe (GameCube name; flags, the stream buffer cut is bit 11): JAIBasic::mFlags (JSystem/JAudio/JAIBasic.h, +0x12); setParamStreamInsideBufferCut 02804598 sets bit 11 */
};
struct AudioHeaderStatics {
    be<u32> mWords[4];
};
struct AudioDestructorRecord {
    be<u32> mWords[3];
};
WWHD_OFFSET(JAIBasicView, mOutputMode, 0x11);
WWHD_OFFSET(JAIBasicView, mFlags, 0x12);
WWHD_SIZE(JAIBasicView, 0x14);
WWHD_SIZE(AudioHeaderStatics, 0x10);
WWHD_SIZE(AudioDestructorRecord, 0xC);
namespace JAIGlobalParameter {
void setParamInterfaceHeapSize(u32 value);
void setParamSoundSceneMax(u32 value);
void setParamSeRegistMax(u32 value);
void setParamSeTrackMax(u32 value);
void setParamSeqControlBufferMax(u32 value);
void setParamStreamControlBufferMax(u32 value);
void setParamAutoHeapMax(u32 value);
void setParamStayHeapMax(u32 value);
void setParamSeqPlayTrackMax(u32 value);
void setParamInputGainDown(f64 value);
void setParamOutputGainUp(f64 value);
void setParamDistanceMax(f64 value);
void setParamMaxVolumeDistance(f64 value);
void setParamMinDistanceVolume(f64 value);
void setParamSoundOutputMode(u32 value);
void setParamSeDistanceFxParameter(u32 value);
void setParamStreamDecodedBufferBlocks(u32 value);
void setParamStreamInsideBufferCut(u32 value);
void setParamAutoHeapRoomSize(u32 value);
void setParamStayHeapSize(u32 value);
void setParamSeDolbyCenterValue(u32 value);
void setParamSeDolbyFrontDistanceMax(f64 value);
void setParamSeDolbyBehindDistanceMax(f64 value);
void setParamInitDataFileName(u32 value);
void setParamWavePath(u32 value);
void setParamSequenceArchivesPath(u32 value);
void setParamStreamPath(u32 value);
void setParamAudioResPath(u32 value);
void setParamSequenceArchivesFileName(u32 value);
void setParamDummyObjectLifeTime(u32 value);
void setParamDummyObjectMax(u32 value);
void setParamAudioCameraMax(u32 value);
void setParamSystemTrackMax(u32 value);
u32 getParamSeCategoryMax();
u32 getParamSoundSceneMax();
u32 getParamSeRegistMax();
u32 getParamSeTrackMax();
u32 getParamSeqTrackMax();
u32 getParamSeqControlBufferMax();
u32 getParamAutoHeapMax();
u32 getParamStayHeapMax();
u32 getParamSeqPlayTrackMax();
f64 getParamDistanceMax();
f64 getParamMaxVolumeDistance();
f64 getParamMinDistanceVolume();
u32 getParamAutoHeapRoomSize();
u32 getParamStayHeapSize();
f64 getParamSeDolbyCenterValue();
f64 getParamSeDolbyFrontDistanceMax();
f64 getParamSeDolbyBehindDistanceMax();
u32 getParamInitDataFileName();
u32 getParamWavePath();
u32 getParamSequenceArchivesPath();
u32 getParamAudioResPath();
u32 getParamSequenceArchivesFileName();
u32 getParamDopplarMoveTime();
u32 getParamDistanceParameterMoveTime();
u32 getParamDummyObjectMax();
u32 getParamSeqMuteVolumeSePlay();
u32 getParamSeqMuteMoveSpeedSePlay();
u32 getParamAudioCameraMax();
u32 getParamSeqParameterLines();
u32 getParamSeDistanceWaitMax();
} // namespace JAIGlobalParameter
void audioParameterAssert(const char*, s32, const char*);
void audioDriverSetOutputMode(u32);
u32 audioSoundTableGetCategoryMax();
void registerAudioParameterStatics(AudioDestructorRecord*);

// Canonical direct scalar reads shared with Sound; owners retain the sole raw static bindings.
namespace JAIGlobalParameter {
extern be<f32>& audioParamDistanceMax;
extern be<f32>& audioParamMinDistanceVolume;
extern be<f32>& audioParamMaxVolumeDistance;
extern be<f32>& audioParamSeDolbyCenterValue;
extern be<f32>& audioParamSeDolbyFrontDistanceMax;
extern be<f32>& audioParamSeDolbyBehindDistanceMax;
extern be<u32>& audioParamDopplarMoveTime;
extern be<u32>& audioParamAudioCameraMax;
extern be<u16>& audioParamSeDistanceFxParameter;
extern be<f32>& audioParamPanDistanceMax;
extern be<f32>& audioParamPanDistance2Max;
extern be<f32>& audioParamPanAngleParameter;
extern be<f32>& audioParamPanAngleParameter2;
extern be<f32>& audioParamDopplarParameter;
extern be<f32>& audioParamSeDistancePitchMax;
extern be<u16>& audioParamSeDefaultFx;
}
// Audio1 owner export: sequence parameter lines (101F9C59), read inline by the Sound SeqInter bounds checks.
namespace JAIGlobalParameter {
extern be<u8>& audioSeqParameterLines;
}
