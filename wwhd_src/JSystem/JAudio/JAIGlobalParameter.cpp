/* Audio configuration parameters and accessors. Verified WWHD behavior. */
#include "JSystem/JAudio/JAIGlobalParameter.h"

namespace JAIGlobalParameter {
// HD: path addresses retain their full u32 API signatures; float APIs use f64 registers
// around the same f32 storage. Byte/halfword getter results retain u32 width.
static u32 dummyObjectMax;
static u32 interfaceHeapSize;
static u32 soundSceneMax;
static u32 seRegistMax;
static u32 seqTrackMax;
static u32 seTrackMax;
static u32 seqControlBufferMax;
static u32 streamControlBufferMax;
static u32 autoHeapMax;
static u32 stayHeapMax;
static u32 seqPlayTrackMax;
static u32 autoHeapRoomSize;
static u32 stayHeapSize;
static u32 initDataFileName;
static u32 wavePath;
static u32 sequenceArchivesPath;
static u32 streamPath;
static u32 sequenceArchivesFileName;
static u32 audioResPath;
static f32 inputGainDown;
static f32 outputGainUp;
static f32 audioParamDistanceMax;
static f32 audioParamMinDistanceVolume;
static f32 audioParamMaxVolumeDistance;
static f32 audioParamSeDolbyCenterValue;
static f32 audioParamSeDolbyFrontDistanceMax;
static f32 audioParamSeDolbyBehindDistanceMax;
static u32 audioParamDopplarMoveTime;
static u32 dummyObjectLifeTime;
static u32 seqMuteMoveSpeedSePlay;
static u32 audioParamAudioCameraMax;
static u32 systemTrackMax;
static u16 audioParamSeDistanceFxParameter;
static u16 seDistanceWaitMax;
static u8 seqMuteVolumeSePlay;
static u8 audioSeqParameterLines;
static u8 distanceParameterMoveTime;
static f32 audioParamPanDistanceMax;
static f32 audioParamPanDistance2Max;
static f32 audioParamPanAngleParameter;
static f32 audioParamPanAngleParameter2;
static f32 audioParamDopplarParameter;
static f32 audioParamSeDistancePitchMax;
static u16 audioParamSeDefaultFx;

static JAIBasicView* msBasic;
} // namespace JAIGlobalParameter
using namespace JAIGlobalParameter;
static AudioHeaderStatics headerStatics;
static AudioDestructorRecord destructorRecord;

namespace JAIGlobalParameter {
/* 02804454 */
void setParamInterfaceHeapSize(u32 value) {
    interfaceHeapSize = value;
}
/* 02804460 */
void setParamSoundSceneMax(u32 value) {
    soundSceneMax = value;
}
/* 0280446C */
void setParamSeRegistMax(u32 value) {
    seRegistMax = value;
}
/* 02804478 */
void setParamSeTrackMax(u32 value) {
    seTrackMax = value;
}
/* 02804484 */
void setParamSeqControlBufferMax(u32 value) {
    seqControlBufferMax = value;
}
/* 02804490 */
void setParamStreamControlBufferMax(u32 value) {
    streamControlBufferMax = value;
}
/* 0280449C */
void setParamAutoHeapMax(u32 value) {
    autoHeapMax = value;
}
/* 028044A8 */
void setParamStayHeapMax(u32 value) {
    stayHeapMax = value;
}
/* 028044B4 */
void setParamSeqPlayTrackMax(u32 value) {
    seqPlayTrackMax = value;
    seqControlBufferMax = value << 1;
}
/* 028044CC */
void setParamInputGainDown(f64 value) {
    inputGainDown = value;
}
/* 028044D8 */
void setParamOutputGainUp(f64 value) {
    outputGainUp = value;
}
/* 028044E4 */
void setParamDistanceMax(f64 value) {
    audioParamDistanceMax = value;
}
/* 028044F0 */
void setParamMaxVolumeDistance(f64 value) {
    audioParamMaxVolumeDistance = value;
}
/* 028044FC */
void setParamMinDistanceVolume(f64 value) {
    audioParamMinDistanceVolume = value;
}
/* 02804508 */
// HD: store the original low byte, but clamp only the driver argument; omit the GC stream call.
void setParamSoundOutputMode(u32 value) {
    u32 mode = value;
    if (value > 2) {
        audioParameterAssert("JAIGlobalParameter.cpp", 347, "(0)");
        mode = 1;
    }
    msBasic->mOutputMode = value;
    audioDriverSetOutputMode(mode);
}
/* 02804588 */
void setParamSeDistanceFxParameter(u32 value) {
    audioParamSeDistanceFxParameter = value;
}
/* 02804594 */
// HD: this setter is a no-op.
void setParamStreamDecodedBufferBlocks(u32 value) {
}
/* 02804598 */
void setParamStreamInsideBufferCut(u32 value) {
    JAIBasicView* basic = msBasic;
    basic->mFlags = (basic->mFlags & 0xF7FF) | ((value & 1) << 11);
}
/* 028045B0 */
void setParamAutoHeapRoomSize(u32 value) {
    autoHeapRoomSize = value;
}
/* 028045BC */
void setParamStayHeapSize(u32 value) {
    stayHeapSize = value;
}
/* 028045C8 */
void setParamSeDolbyCenterValue(u32 value) {
    audioParamSeDolbyCenterValue = (f32)value;
}
/* 028045FC */
void setParamSeDolbyFrontDistanceMax(f64 value) {
    audioParamSeDolbyFrontDistanceMax = value;
}
/* 02804608 */
void setParamSeDolbyBehindDistanceMax(f64 value) {
    audioParamSeDolbyBehindDistanceMax = value;
}
/* 02804614 */
void setParamInitDataFileName(u32 value) {
    initDataFileName = value;
}
/* 02804620 */
void setParamWavePath(u32 value) {
    wavePath = value;
}
/* 0280462C */
void setParamSequenceArchivesPath(u32 value) {
    sequenceArchivesPath = value;
}
/* 02804638 */
void setParamStreamPath(u32 value) {
    streamPath = value;
}
/* 02804644 */
void setParamAudioResPath(u32 value) {
    audioResPath = value;
}
/* 02804650 */
void setParamSequenceArchivesFileName(u32 value) {
    sequenceArchivesFileName = value;
}
/* 0280465C */
void setParamDummyObjectLifeTime(u32 value) {
    dummyObjectLifeTime = value;
}
/* 02804668 */
void setParamDummyObjectMax(u32 value) {
    dummyObjectMax = value;
}
/* 02804674 */
void setParamAudioCameraMax(u32 value) {
    audioParamAudioCameraMax = value;
}
/* 02804680 */
void setParamSystemTrackMax(u32 value) {
    systemTrackMax = value;
}
/* 0280468C */
u32 getParamSeCategoryMax() {
    return audioSoundTableGetCategoryMax();
}
} // namespace JAIGlobalParameter
using namespace JAIGlobalParameter;
namespace JAIGlobalParameter {
/* 028046B8 */
u32 getParamSoundSceneMax() {
    return soundSceneMax;
}
/* 028046C4 */
u32 getParamSeRegistMax() {
    return seRegistMax;
}
/* 028046D0 */
u32 getParamSeTrackMax() {
    return seTrackMax;
}
/* 028046DC */
u32 getParamSeqTrackMax() {
    return seqTrackMax;
}
/* 028046E8 */
u32 getParamSeqControlBufferMax() {
    return seqControlBufferMax;
}
/* 028046F4 */
u32 getParamAutoHeapMax() {
    return autoHeapMax;
}
/* 02804700 */
u32 getParamStayHeapMax() {
    return stayHeapMax;
}
/* 0280470C */
u32 getParamSeqPlayTrackMax() {
    return seqPlayTrackMax;
}
/* 02804718 */
f64 getParamDistanceMax() {
    return audioParamDistanceMax;
}
/* 02804724 */
f64 getParamMaxVolumeDistance() {
    return audioParamMaxVolumeDistance;
}
/* 02804730 */
f64 getParamMinDistanceVolume() {
    return audioParamMinDistanceVolume;
}
/* 0280473C */
u32 getParamAutoHeapRoomSize() {
    return autoHeapRoomSize;
}
/* 02804748 */
u32 getParamStayHeapSize() {
    return stayHeapSize;
}
/* 02804754 */
f64 getParamSeDolbyCenterValue() {
    return audioParamSeDolbyCenterValue;
}
/* 02804760 */
f64 getParamSeDolbyFrontDistanceMax() {
    return audioParamSeDolbyFrontDistanceMax;
}
/* 0280476C */
f64 getParamSeDolbyBehindDistanceMax() {
    return audioParamSeDolbyBehindDistanceMax;
}
/* 02804778 */
u32 getParamInitDataFileName() {
    return initDataFileName;
}
/* 02804784 */
u32 getParamWavePath() {
    return wavePath;
}
/* 02804790 */
u32 getParamSequenceArchivesPath() {
    return sequenceArchivesPath;
}
/* 0280479C */
u32 getParamAudioResPath() {
    return audioResPath;
}
/* 028047A8 */
u32 getParamSequenceArchivesFileName() {
    return sequenceArchivesFileName;
}
/* 028047B4 */
u32 getParamDopplarMoveTime() {
    return audioParamDopplarMoveTime;
}
/* 028047C0 */
u32 getParamDistanceParameterMoveTime() {
    return distanceParameterMoveTime;
}
/* 028047CC */
u32 getParamDummyObjectMax() {
    return dummyObjectMax;
}
/* 028047D8 */
u32 getParamSeqMuteVolumeSePlay() {
    return seqMuteVolumeSePlay;
}
/* 028047E4 */
u32 getParamSeqMuteMoveSpeedSePlay() {
    return seqMuteMoveSpeedSePlay;
}
/* 028047F0 */
u32 getParamAudioCameraMax() {
    return audioParamAudioCameraMax;
}
/* 028047FC */
u32 getParamSeqParameterLines() {
    return audioSeqParameterLines;
}
/* 02804808 */
u32 getParamSeDistanceWaitMax() {
    return seDistanceWaitMax;
}

} // namespace JAIGlobalParameter
using namespace JAIGlobalParameter;

/* 02804690 */
// HD: compiler-generated common-header zeroing and destructor registration.
static void __sinit_JAIGlobalParameter_cpp() {
    for (u32 i = 0; i < 4; ++i)
        headerStatics.mWords[i] = 0;
    registerAudioParameterStatics(&destructorRecord);
}

/* WWHD glue
data audioParamPanDistanceMax 101F9C24
data audioParamPanDistance2Max 101F9C28
data audioParamPanAngleParameter 101F9C2C
data audioParamPanAngleParameter2 101F9C30
data audioParamDopplarParameter 101F9C34
data audioParamSeDistancePitchMax 101F9C38
data audioParamSeDefaultFx 101F9C52

data dummyObjectMax 101F9BB8
data interfaceHeapSize 101F9BBC
data soundSceneMax 101F9BC0
data seRegistMax 101F9BC4
data seqTrackMax 101F9BC8
data seTrackMax 101F9BCC
data seqControlBufferMax 101F9BD0
data streamControlBufferMax 101F9BD4
data autoHeapMax 101F9BD8
data stayHeapMax 101F9BDC
data seqPlayTrackMax 101F9BE0
data autoHeapRoomSize 101F9BE4
data stayHeapSize 101F9BE8
data initDataFileName 101F9BEC
data wavePath 101F9BF0
data sequenceArchivesPath 101F9BF4
data streamPath 101F9BF8
data sequenceArchivesFileName 101F9BFC
data audioResPath 101F9C00
data inputGainDown 101F9C04
data outputGainUp 101F9C08
data audioParamDistanceMax 101F9C0C
data audioParamMinDistanceVolume 101F9C10
data audioParamMaxVolumeDistance 101F9C14
data audioParamSeDolbyCenterValue 101F9C18
data audioParamSeDolbyFrontDistanceMax 101F9C1C
data audioParamSeDolbyBehindDistanceMax 101F9C20
data audioParamDopplarMoveTime 101F9C3C
data dummyObjectLifeTime 101F9C40
data seqMuteMoveSpeedSePlay 101F9C44
data audioParamAudioCameraMax 101F9C48
data systemTrackMax 101F9C4C
data audioParamSeDistanceFxParameter 101F9C50
data seDistanceWaitMax 101F9C54
data seqMuteVolumeSePlay 101F9C58
data audioSeqParameterLines 101F9C59
data distanceParameterMoveTime 101F9C5B
data msBasic 104B4F14
data headerStatics 104B4FCC
data destructorRecord 101F9BAC
str 1016F654 "JAIGlobalParameter.cpp"
str 1016F650 "(0)"
ext 0273AA24 void audioParameterAssert(const char*, s32, const char*)
ext 028157C8 void audioDriverSetOutputMode(u32)
ext 0280E03C u32 audioSoundTableGetCategoryMax()
ext 028F026C void registerAudioParameterStatics(AudioDestructorRecord*)
*/
