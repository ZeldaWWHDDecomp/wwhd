/* Native JAIGlobalParameter setters/getters02804454..02804808, including
 * registration02804690. Prior DummyObject and following InitData excluded. */
#include "gabi.h"
using namespace gabi;
namespace jai_global_cpp {
void setParamInterfaceHeapSize(u32 value) {
  WWHD_FUNC(0x02804454, void, value);
  store<u32>(0x101F9BBC, value);
}
VERIFY(0x02804454, setParamInterfaceHeapSize);
void setParamSoundSceneMax(u32 value) {
  WWHD_FUNC(0x02804460, void, value);
  store<u32>(0x101F9BC0, value);
}
VERIFY(0x02804460, setParamSoundSceneMax);
void setParamSeRegistMax(u32 value) {
  WWHD_FUNC(0x0280446C, void, value);
  store<u32>(0x101F9BC4, value);
}
VERIFY(0x0280446C, setParamSeRegistMax);
void setParamSeTrackMax(u32 value) {
  WWHD_FUNC(0x02804478, void, value);
  store<u32>(0x101F9BCC, value);
}
VERIFY(0x02804478, setParamSeTrackMax);
void setParamSeqControlBufferMax(u32 value) {
  WWHD_FUNC(0x02804484, void, value);
  store<u32>(0x101F9BD0, value);
}
VERIFY(0x02804484, setParamSeqControlBufferMax);
void setParamStreamControlBufferMax(u32 value) {
  WWHD_FUNC(0x02804490, void, value);
  store<u32>(0x101F9BD4, value);
}
VERIFY(0x02804490, setParamStreamControlBufferMax);
void setParamAutoHeapMax(u32 value) {
  WWHD_FUNC(0x0280449C, void, value);
  store<u32>(0x101F9BD8, value);
}
VERIFY(0x0280449C, setParamAutoHeapMax);
void setParamStayHeapMax(u32 value) {
  WWHD_FUNC(0x028044A8, void, value);
  store<u32>(0x101F9BDC, value);
}
VERIFY(0x028044A8, setParamStayHeapMax);
void setParamSeqPlayTrackMax(u32 value) {
  WWHD_FUNC(0x028044B4, void, value);
  store<u32>(0x101F9BE0, value);
  store<u32>(0x101F9BD0, value << 1);
}
VERIFY(0x028044B4, setParamSeqPlayTrackMax);
void setParamInputGainDown(f64 value) {
  WWHD_FUNC(0x028044CC, void, value);
  store<f32>(0x101F9C04, value);
}
VERIFY(0x028044CC, setParamInputGainDown);
void setParamOutputGainUp(f64 value) {
  WWHD_FUNC(0x028044D8, void, value);
  store<f32>(0x101F9C08, value);
}
VERIFY(0x028044D8, setParamOutputGainUp);
void setParamDistanceMax(f64 value) {
  WWHD_FUNC(0x028044E4, void, value);
  store<f32>(0x101F9C0C, value);
}
VERIFY(0x028044E4, setParamDistanceMax);
void setParamMaxVolumeDistance(f64 value) {
  WWHD_FUNC(0x028044F0, void, value);
  store<f32>(0x101F9C14, value);
}
VERIFY(0x028044F0, setParamMaxVolumeDistance);
void setParamMinDistanceVolume(f64 value) {
  WWHD_FUNC(0x028044FC, void, value);
  store<f32>(0x101F9C10, value);
}
VERIFY(0x028044FC, setParamMinDistanceVolume);
void setParamSoundOutputMode(u32 value) {
  WWHD_FUNC(0x02804508, void, value);
  u32 mode = value;
  if (value > 2) {
    call<void>(0x0273AA24, at<void>(0x1016F654), 347, at<void>(0x1016F650));
    mode = 1;
  }
  store<u8>(load<u32>(0x104B4F14) + 0x11, value);
  call<void>(0x028157C8, mode);
}
VERIFY(0x02804508, setParamSoundOutputMode);
void setParamSeDistanceFxParameter(u32 value) {
  WWHD_FUNC(0x02804588, void, value);
  store<u16>(0x101F9C50, value);
}
VERIFY(0x02804588, setParamSeDistanceFxParameter);
void setParamStreamDecodedBufferBlocks(u32 value) {
  WWHD_FUNC(0x02804594, void, value);
}
VERIFY(0x02804594, setParamStreamDecodedBufferBlocks);
void setParamStreamInsideBufferCut(u32 value) {
  WWHD_FUNC(0x02804598, void, value);
  u32 p = load<u32>(0x104B4F14);
  store<u16>(p + 0x12, (load<u16>(p + 0x12) & 0xF7FF) | ((value & 1) << 11));
}
VERIFY(0x02804598, setParamStreamInsideBufferCut);
void setParamAutoHeapRoomSize(u32 value) {
  WWHD_FUNC(0x028045B0, void, value);
  store<u32>(0x101F9BE4, value);
}
VERIFY(0x028045B0, setParamAutoHeapRoomSize);
void setParamStayHeapSize(u32 value) {
  WWHD_FUNC(0x028045BC, void, value);
  store<u32>(0x101F9BE8, value);
}
VERIFY(0x028045BC, setParamStayHeapSize);
void setParamSeDolbyCenterValue(u32 value) {
  WWHD_FUNC(0x028045C8, void, value);
  store<f32>(0x101F9C18, (f32)value);
}
VERIFY(0x028045C8, setParamSeDolbyCenterValue);
void setParamSeDolbyFrontDistanceMax(f64 value) {
  WWHD_FUNC(0x028045FC, void, value);
  store<f32>(0x101F9C1C, value);
}
VERIFY(0x028045FC, setParamSeDolbyFrontDistanceMax);
void setParamSeDolbyBehindDistanceMax(f64 value) {
  WWHD_FUNC(0x02804608, void, value);
  store<f32>(0x101F9C20, value);
}
VERIFY(0x02804608, setParamSeDolbyBehindDistanceMax);
void setParamInitDataFileName(u32 value) {
  WWHD_FUNC(0x02804614, void, value);
  store<u32>(0x101F9BEC, value);
}
VERIFY(0x02804614, setParamInitDataFileName);
void setParamWavePath(u32 value) {
  WWHD_FUNC(0x02804620, void, value);
  store<u32>(0x101F9BF0, value);
}
VERIFY(0x02804620, setParamWavePath);
void setParamSequenceArchivesPath(u32 value) {
  WWHD_FUNC(0x0280462C, void, value);
  store<u32>(0x101F9BF4, value);
}
VERIFY(0x0280462C, setParamSequenceArchivesPath);
void setParamStreamPath(u32 value) {
  WWHD_FUNC(0x02804638, void, value);
  store<u32>(0x101F9BF8, value);
}
VERIFY(0x02804638, setParamStreamPath);
void setParamAudioResPath(u32 value) {
  WWHD_FUNC(0x02804644, void, value);
  store<u32>(0x101F9C00, value);
}
VERIFY(0x02804644, setParamAudioResPath);
void setParamSequenceArchivesFileName(u32 value) {
  WWHD_FUNC(0x02804650, void, value);
  store<u32>(0x101F9BFC, value);
}
VERIFY(0x02804650, setParamSequenceArchivesFileName);
void setParamDummyObjectLifeTime(u32 value) {
  WWHD_FUNC(0x0280465C, void, value);
  store<u32>(0x101F9C40, value);
}
VERIFY(0x0280465C, setParamDummyObjectLifeTime);
void setParamDummyObjectMax(u32 value) {
  WWHD_FUNC(0x02804668, void, value);
  store<u32>(0x101F9BB8, value);
}
VERIFY(0x02804668, setParamDummyObjectMax);
void setParamAudioCameraMax(u32 value) {
  WWHD_FUNC(0x02804674, void, value);
  store<u32>(0x101F9C48, value);
}
VERIFY(0x02804674, setParamAudioCameraMax);
void setParamSystemTrackMax(u32 value) {
  WWHD_FUNC(0x02804680, void, value);
  store<u32>(0x101F9C4C, value);
}
VERIFY(0x02804680, setParamSystemTrackMax);
u32 getParamSeCategoryMax() {
  WWHD_FUNC(0x0280468C, u32);
  return call<u32>(0x0280E03C);
}
VERIFY(0x0280468C, getParamSeCategoryMax);
void staticInit() {
  WWHD_FUNC(0x02804690, void);
  for (u32 i = 0; i < 4; ++i)
    store<u32>(0x104B4FCC + i * 4, 0);
  call<void>(0x028F026C, at<void>(0x101F9BAC));
}
VERIFY(0x02804690, staticInit);
u32 getParamSoundSceneMax() {
  WWHD_FUNC(0x028046B8, u32);
  return load<u32>(0x101F9BC0);
}
VERIFY(0x028046B8, getParamSoundSceneMax);
u32 getParamSeRegistMax() {
  WWHD_FUNC(0x028046C4, u32);
  return load<u32>(0x101F9BC4);
}
VERIFY(0x028046C4, getParamSeRegistMax);
u32 getParamSeTrackMax() {
  WWHD_FUNC(0x028046D0, u32);
  return load<u32>(0x101F9BCC);
}
VERIFY(0x028046D0, getParamSeTrackMax);
u32 getParamSeqTrackMax() {
  WWHD_FUNC(0x028046DC, u32);
  return load<u32>(0x101F9BC8);
}
VERIFY(0x028046DC, getParamSeqTrackMax);
u32 getParamSeqControlBufferMax() {
  WWHD_FUNC(0x028046E8, u32);
  return load<u32>(0x101F9BD0);
}
VERIFY(0x028046E8, getParamSeqControlBufferMax);
u32 getParamAutoHeapMax() {
  WWHD_FUNC(0x028046F4, u32);
  return load<u32>(0x101F9BD8);
}
VERIFY(0x028046F4, getParamAutoHeapMax);
u32 getParamStayHeapMax() {
  WWHD_FUNC(0x02804700, u32);
  return load<u32>(0x101F9BDC);
}
VERIFY(0x02804700, getParamStayHeapMax);
u32 getParamSeqPlayTrackMax() {
  WWHD_FUNC(0x0280470C, u32);
  return load<u32>(0x101F9BE0);
}
VERIFY(0x0280470C, getParamSeqPlayTrackMax);
f64 getParamDistanceMax() {
  WWHD_FUNC(0x02804718, f64);
  return load<f32>(0x101F9C0C);
}
VERIFY(0x02804718, getParamDistanceMax);
f64 getParamMaxVolumeDistance() {
  WWHD_FUNC(0x02804724, f64);
  return load<f32>(0x101F9C14);
}
VERIFY(0x02804724, getParamMaxVolumeDistance);
f64 getParamMinDistanceVolume() {
  WWHD_FUNC(0x02804730, f64);
  return load<f32>(0x101F9C10);
}
VERIFY(0x02804730, getParamMinDistanceVolume);
u32 getParamAutoHeapRoomSize() {
  WWHD_FUNC(0x0280473C, u32);
  return load<u32>(0x101F9BE4);
}
VERIFY(0x0280473C, getParamAutoHeapRoomSize);
u32 getParamStayHeapSize() {
  WWHD_FUNC(0x02804748, u32);
  return load<u32>(0x101F9BE8);
}
VERIFY(0x02804748, getParamStayHeapSize);
f64 getParamSeDolbyCenterValue() {
  WWHD_FUNC(0x02804754, f64);
  return load<f32>(0x101F9C18);
}
VERIFY(0x02804754, getParamSeDolbyCenterValue);
f64 getParamSeDolbyFrontDistanceMax() {
  WWHD_FUNC(0x02804760, f64);
  return load<f32>(0x101F9C1C);
}
VERIFY(0x02804760, getParamSeDolbyFrontDistanceMax);
f64 getParamSeDolbyBehindDistanceMax() {
  WWHD_FUNC(0x0280476C, f64);
  return load<f32>(0x101F9C20);
}
VERIFY(0x0280476C, getParamSeDolbyBehindDistanceMax);
u32 getParamInitDataFileName() {
  WWHD_FUNC(0x02804778, u32);
  return load<u32>(0x101F9BEC);
}
VERIFY(0x02804778, getParamInitDataFileName);
u32 getParamWavePath() {
  WWHD_FUNC(0x02804784, u32);
  return load<u32>(0x101F9BF0);
}
VERIFY(0x02804784, getParamWavePath);
u32 getParamSequenceArchivesPath() {
  WWHD_FUNC(0x02804790, u32);
  return load<u32>(0x101F9BF4);
}
VERIFY(0x02804790, getParamSequenceArchivesPath);
u32 getParamAudioResPath() {
  WWHD_FUNC(0x0280479C, u32);
  return load<u32>(0x101F9C00);
}
VERIFY(0x0280479C, getParamAudioResPath);
u32 getParamSequenceArchivesFileName() {
  WWHD_FUNC(0x028047A8, u32);
  return load<u32>(0x101F9BFC);
}
VERIFY(0x028047A8, getParamSequenceArchivesFileName);
u32 getParamDopplarMoveTime() {
  WWHD_FUNC(0x028047B4, u32);
  return load<u32>(0x101F9C3C);
}
VERIFY(0x028047B4, getParamDopplarMoveTime);
u32 getParamDistanceParameterMoveTime() {
  WWHD_FUNC(0x028047C0, u32);
  return load<u8>(0x101F9C5B);
}
VERIFY(0x028047C0, getParamDistanceParameterMoveTime);
u32 getParamDummyObjectMax() {
  WWHD_FUNC(0x028047CC, u32);
  return load<u32>(0x101F9BB8);
}
VERIFY(0x028047CC, getParamDummyObjectMax);
u32 getParamSeqMuteVolumeSePlay() {
  WWHD_FUNC(0x028047D8, u32);
  return load<u8>(0x101F9C58);
}
VERIFY(0x028047D8, getParamSeqMuteVolumeSePlay);
u32 getParamSeqMuteMoveSpeedSePlay() {
  WWHD_FUNC(0x028047E4, u32);
  return load<u32>(0x101F9C44);
}
VERIFY(0x028047E4, getParamSeqMuteMoveSpeedSePlay);
u32 getParamAudioCameraMax() {
  WWHD_FUNC(0x028047F0, u32);
  return load<u32>(0x101F9C48);
}
VERIFY(0x028047F0, getParamAudioCameraMax);
u32 getParamSeqParameterLines() {
  WWHD_FUNC(0x028047FC, u32);
  return load<u8>(0x101F9C59);
}
VERIFY(0x028047FC, getParamSeqParameterLines);
u32 getParamSeDistanceWaitMax() {
  WWHD_FUNC(0x02804808, u32);
  return load<u16>(0x101F9C54);
}
VERIFY(0x02804808, getParamSeDistanceWaitMax);
} // namespace jai_global_cpp
