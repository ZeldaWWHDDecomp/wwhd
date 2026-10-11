#pragma once
#include "SSystem/SComponent/c_xyz.h"
namespace d_kankyo_part2 {
// HD: runtime lights append an intensity to the GC LIGHT_INFLUENCE.
struct LightInfluence {
  cXyz mPos;
  GXColorS10 mColor;
  be<f32> mPower, mFluctuation;
  be<s32> mIdx;
  be<f32> mHDIntensity;
};
WWHD_SIZE(LightInfluence, 0x24);
inline f32 signedToFloat(s32 value) { return (f32)value; }
inline u32 allocateVector() { return gabi::call<u32>(0x0273ad10, 0xc); }
inline BOOL eventFlag(u8 *event, u32 flag) {
  return gabi::call<BOOL>(0x025b8b94, event, flag);
}
// HD: stage point-light entries are 0x1c bytes; runtime influence entries are 0x24.
struct StageLight {
  cXyz position;
  be<f32> radius;
  u8 reserved[8];
  be<u8> r, g, b, fluctuation;
};
struct StageData {
  be<u32> vtable;
  u32 getPlightInfo() {
    return gabi::call_ptr<u32>(*gabi::at<be<u32>>(vtable + 0xdc), this);
  }
  s32 getPlightNumInfo() {
    return gabi::call_ptr<s32>(*gabi::at<be<u32>>(vtable + 0x12c), this);
  }
};
// HD: field locations follow the expanded light, material and rendering blocks.
struct Environment {
  u8 prefix[0x38];
  LightInfluence mLightInfluence[30];
  be<u32> mpPLights[200], mpEfLights[10];
  u8 lightState[0x80c - 0x7b8];
  be<u32> mpPlightInfo;
  u8 weather[0xa40 - 0x810];
  be<s32> mRainCount;
  u8 wind[0xaec - 0xa44];
  be<u32> mpWaveInfl[10];
  u8 colors[0xfa8 - 0xb14];
  GXColorS10 mAddColFog, mVrboxAddColSky0, mVrboxAddColKasumi;
  be<f32> mDiceWeatherChangeTime, mDiceWeatherTime;
  u8 blend[0xfd8 - 0xfc8];
  be<f32> mFogStartZ, mFogEndZ, mFogRatio;
  u8 ratios[0x1068 - 0xfe4];
  be<u32> mDiceWeatherCounter;
  u8 rain[8];
  be<s32> mPlayerPLightIdx, mPlayerEflightIdx;
  u8 schedule[0x1093 - 0x107c];
  be<u8> mDiceWeatherMode, mDiceWeatherState;
};
WWHD_OFFSET(Environment, mpPLights, 0x470);
WWHD_OFFSET(Environment, mpEfLights, 0x790);
WWHD_OFFSET(Environment, mAddColFog, 0xfa8);
WWHD_OFFSET(Environment, mDiceWeatherCounter, 0x1068);
WWHD_OFFSET(Environment, mDiceWeatherMode, 0x1093);
struct Save {
  u8 memory[0x44];
  be<f32> time;
  u8 player[0xd4 - 0x48];
  u8 inventory[0x644 - 0xd4];
  u8 event[1];
};
struct FloatColor {
  be<f32> r, g, b, a;
};
struct FloatMatrix {
  be<f32> elements[12];
};
struct Constants {
  be<f32> one;
  u8 alignment[8];
  be<f32> zero;
  u8 reserved[4];
  be<f32> farDistance, minPower, initialPower, stageRadiusScale;
  u8 spare[8];
  be<f32> tinyRatio;
  u8 warning[0xfc - 0xd4];
  be<f32> degreesPerHour, minutesPerHour, colorScale, dayCycle, ghostDayStart,
      ghostNightStart, ghostNightEnd;
};
inline Constants *constants() { return gabi::at<Constants>(0x1004f0a4); }
inline Environment *environment() {
  return gabi::call<Environment *>(0x02555d0c);
}
inline Save *save() { return gabi::at<Save>(gabi::load<u32>(0x101f84dc)); }
template <class T> inline T *view(u32 address) { return gabi::at<T>(address); }
inline StageData *stageData() {
  return view<StageData>(gabi::call<u32>(0x025200d4) + 0x5150);
}
inline s8 stayRoom() { return *view<be<s8>>(0x1047e6c8); }
inline void warnFogRatio() { gabi::call(0x025f27e8, 0x1004f0d4); }
inline BOOL hasSymbol(u8 *inventory) {
  return gabi::call<BOOL>(0x025b7d90, inventory, 0);
}
inline f32 squareDistance(u32 a, u32 b) {
  return gabi::call<f32>(0x028e8de8, a, b);
}
inline f32 squareRoot(f32 x) { return gabi::call<f32>(0x028f4384, x); }
inline void setRain(s32 count) { gabi::call(0x0257e7c0, count); }
enum GhostShipIsland {
  CrescentMoonIsland = 5,
  DiamondSteppeIsland = 0x24,
  BombIsland = 0x22,
  SpectacleIsland = 0xa,
  FiveStarIsles = 0x31,
  StarBeltArchipelago = 0x15,
  GreatfishIsle = 0x17
};
enum EventFlag { EndlessNight = 0xa02 };
} // namespace d_kankyo_part2
