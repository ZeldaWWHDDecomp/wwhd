/* JAIGlobalParameter API and private HD audio object fields.
 * HD: verified signatures retain f64 values and full u32 pointer/byte/halfword registers. */
#pragma once
#include "bindings.h"
struct JAIBasicView {
    u8 _00[0x11];
    /* 0x11 */ be<u8> field_0xd;  // GameCube name; HD output mode.
    /* 0x12 */ be<u16> field_0xe; // GameCube flags; stream buffer cut is bit11.
};
struct AudioHeaderStatics {
    be<u32> mWords[4];
};
struct AudioDestructorRecord {
    be<u32> mWords[3];
};
WWHD_OFFSET(JAIBasicView, field_0xd, 0x11);
WWHD_OFFSET(JAIBasicView, field_0xe, 0x12);
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
