/** Actor and input helpers. HD keeps wider entry arguments and moves
 * STControl's vtable to the end. Numbered fields retain tww names despite their
 * shifted HD offsets. Private game-derived source. */
#pragma once
#include "bindings.h"

struct STControlVtable {
  u8 padding0[0x14];
  be<u32> getValueStick;
  u8 padding18[4];
  be<u32> getAngleStick;
};
struct STControl {
  enum Direction {
    LEFT = 1,
    RIGHT = 2,
    UP = 4,
    DOWN = 8,
    HORIZONTAL = LEFT | RIGHT,
    VERTICAL = UP | DOWN
  };
  /* 0x00 */ be<f32> field_0x04;
  /* 0x04 */ be<f32> field_0x08;
  /* 0x08 */ be<u8> field_0x0c;
  /* 0x09 */ be<u8> field_0x0d;
  /* 0x0A */ be<s16> field_0x0e;
  /* 0x0C */ be<s16> field_0x10;
  /* 0x0E */ be<s16> field_0x12;
  /* 0x10 */ be<s16> field_0x14;
  /* 0x12 */ be<s16> field_0x16;
  /* 0x14 */ be<s16> field_0x18;
  /* 0x16 */ be<s16> field_0x1a;
  /* 0x18 */ be<s16> field_0x1c;
  /* 0x1A */ be<s16> field_0x1e;
  /* 0x1C */ be<s16> field_0x20;
  /* 0x1E */ be<s16> field_0x22;
  /* 0x20 */ be<s16> field_0x24;
  /* 0x22 */ be<s16> field_0x26;
  /* 0x24 */ gptr<STControlVtable> mpVtable;
};
struct dLib_circle_path_c {
  cXyz mTranslation, mPos;
  be<f32> mRadius, mWobbleAmplitude;
  be<s16> mAngle, mAngleSpeed;
};
struct dLib_wave_c {
  be<s16> mAngX, mAngZ, mAnimX, mAnimZ, mRotX, mRotZ;
};
struct dLib_anm_idx_c {
  be<s32> mBckIdx, mSndIdx;
};
struct dLib_anm_prm_c {
  be<s8> mAnmIdx, mNextPrmIdx;
  be<s16> field_0x02;
  be<f32> mMorf, mPlaySpeed;
  be<u32> mLoopMode;
};
namespace dLib_layout {
struct dPnt {
  be<u8> mArg0, mArg1, mArg2, mArg3;
  cXyz m_position;
};
struct dPath {
  be<u16> m_num, m_nextID;
  be<u8> mArg0, m_closed, mArg2, mArg3;
  gptr<dPnt> m_points;
};
struct dStage_dPath_c {
  be<s32> num;
  gptr<dPath> m_path;
};
struct stage_scls_info_class {
  be<char> mStage[8];
  be<u8> mStart;
  be<s8> mRoom;
  be<u8> mWipe, mPad;
};
struct stage_scls_info_dummy_class {
  be<s32> num;
  gptr<stage_scls_info_class> m_entries;
};
struct stageVtable {
  u8 padding0[0x16C];
  be<u32> getSclsInfo;
  u8 padding170[0x1C];
  be<u32> getPathInf;
};
struct stageData {
  gptr<stageVtable> mpVtable;
};
struct vectorWords {
  be<u32> x, y, z;
};
struct quaternionWords {
  be<u32> x, y, z, w;
};
struct sixVectors {
  cXyz mVectors[6];
};
struct safeString {
  be<u32> mData, mpVtable;
};
struct resourcePointer {
  be<u32> mResource;
};
struct gamePointer {
  gptr<dComIfG_inf_c> mGame;
};
struct dataAddress {
  u8 opaque;
};
struct word {
  be<u32> mValue;
};
struct systemInfo {
  be<u32> mBusClock;
};
struct headerStatics {
  be<f32> mNegativePi, mPi;
  be<u8> mFlag, mByte;
  u8 padding[2];
  be<u32> mZeroWords[4];
};
struct staticRecord {
  u8 opaque[12];
};
struct staticRecords {
  staticRecord mFirst, mSecond, mThird;
};
WWHD_SIZE(dPnt, 16);
WWHD_SIZE(dPath, 12);
WWHD_SIZE(dStage_dPath_c, 8);
WWHD_SIZE(stage_scls_info_class, 12);
WWHD_SIZE(stage_scls_info_dummy_class, 8);
WWHD_OFFSET(stageVtable, getPathInf, 0x18C);
WWHD_SIZE(stageVtable, 0x190);
WWHD_SIZE(stageData, 4);
WWHD_SIZE(vectorWords, 12);
WWHD_SIZE(quaternionWords, 16);
WWHD_SIZE(sixVectors, 72);
WWHD_SIZE(safeString, 8);
WWHD_SIZE(headerStatics, 0x1C);
WWHD_SIZE(staticRecords, 36);
} // namespace dLib_layout
WWHD_OFFSET(STControl, mpVtable, 0x24);
WWHD_SIZE(STControl, 40);
WWHD_SIZE(dLib_circle_path_c, 36);
WWHD_SIZE(dLib_wave_c, 12);
WWHD_SIZE(dLib_anm_idx_c, 8);
WWHD_SIZE(dLib_anm_prm_c, 16);
// HD: integer entry signatures preserve the verified input domain and wrapping
// offsets.
template <class T> inline T *dLib_at(u32 address) {
  return gabi::at<T>(address);
}
inline u32 dLib_stackSecondary() {
  return gabi::load<u32>(gabi::cpu->r[1] + 8);
}
inline f32 dLib_sine(s32 angle) {
  return gabi::load<f32>(0x104A44F8u + ((u32(angle) & 0xFFFFu) >> 3) * 8);
}
inline f32 dLib_cosine(s32 angle) {
  return gabi::load<f32>(0x104A44FCu + ((u32(angle) & 0xFFFFu) >> 3) * 8);
}
inline void dLib_copyVector(u32 dest, u32 source) {
  auto *src = dLib_at<dLib_layout::vectorWords>(source);
  auto *dst = dLib_at<dLib_layout::vectorWords>(dest);
  u32 x = src->x, z = src->z, y = src->y;
  dst->x = x;
  dst->y = y;
  dst->z = z;
}

void dLib_debugDrawFan();
void dLib_getPosFromMtx(u32 m, u32 out);
void STControl_setWaitParm(u32 p, s32 first, s32 minimum, s32 step, s32 delay,
                           s32 margin, s32 sector, f64 high, f64 low);
void STControl_init(u32 p);
void STControl_delete(u32 p, u32 flags);
void STControl_Xinit(u32 p);
void STControl_Yinit(u32 p);
s32 STControl_checkLeftTrigger(u32 p);
s32 STControl_checkRightTrigger(u32 p);
s32 STControl_checkUpTrigger(u32 p);
s32 STControl_checkDownTrigger(u32 p);
f64 STControl_getValueStick();
s32 STControl_getAngleStick();
void dLib_headerObjectDelete(u32 p, u32 flags);
void dLib_headerObjectDestroy();
u32 dLib_setFirstMsg(u32 bit, u32 first, u32 again);
s32 dLib_brkInit(u32 model, u32 animation, u32 arc, u32 number);
s32 dLib_btkInit(u32 model, u32 animation, u32 arc, u32 number);
u32 STControl_construct(u32 p, s32 first, s32 minimum, s32 step, s32 delay,
                        s32 margin, s32 sector, f64 high, f64 low);
s32 dLib_groundPosition(u32 pos);
void dLib_setCirclePath(u32 p);
f64 dLib_getWaterY(u32 pos, u32 acch);
s32 dLib_pathInfo(u32 out, s32 point);
u32 STControl_checkTrigger(u32 p);
s32 dLib_checkActorInCircle(u32 center, u32 actor, f64 radius, f64 height);
s32 dLib_checkPlayerInCircle(u32 center, f64 radius, f64 height);
void dLib_get_QuatFromTriangle(u32 out, u32 a, u32 b, u32 c);
void dLib_calc_QuatFromTriangle(u32 out, u32 a, u32 b, u32 c, f64 t);
s32 dLib_checkActorInFan(u32 center, u32 actor, s32 facing, s32 angle_limit,
                         f64 radius, f64 height);
void dLib_scaleAnime(u32 value, u32 table, s32 count, u32 index, f64 speed,
                     f64 maximum, f64 minimum);
void dLib_debugDrawAxis(u32 matrix, f64 length);
void dLib_waveRot(u32 pos, u32 wave, f64 sway);
void dLib_setAnm(u32 arc, u32 model, u32 animation, u32 parameter, u32 previous,
                 u32 indices, u32 parameters, u32 force);
void dLib_bcks_setAnm(u32 arc, u32 model, u32 animation, u32 parameter,
                      u32 previous, u32 indices, u32 parameters, u32 force);
void dLib_setNextStageBySclsNum(u32 index, s32 room);
void dLib_pathMove(u32 pos, u32 point, u32 path, u32 callback, u32 data,
                   f64 speed);
f64 dLib_lerp(f64 a, f64 b, f64 t);
u32 dLib_getIplDaysFromSaveTime();
f64 dLib_padStickValue(u32);
s32 dLib_padStickAngle(u32);
f64 dLib_groundCross(u32, u32);
f64 dLib_addCalc(u32, f32, f64, f64, f64);
f64 dLib_seaWave(f32, f32);
f64 dLib_vectorSquared(u32);
f64 dLib_vectorSquaredDistance(u32, u32);
f64 dLib_sqrt(f64);
s32 dLib_targetAngleY(u32, u32);
s32 dLib_distanceAngleS(s32, s32);
s32 dLib_atan2s(f32, f32);
u32 dLib_seaArea(f32, f32);
u32 dLib_getGame();
u32 STControl_setWaitParmResult(u32, s32, s32, s32, s32, s32, s32, f64, f64);
u32 STControl_initResult(u32);
u32 dLib_isEventBit(u32, u32);
u32 dLib_roomStatus(u32, s32);
u32 dLib_btkInitialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32);
u32 dLib_brkInitialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32);
u32 dLib_getResource(u32, u32, u32);
u32 dLib_alloc(u32);
u32 dLib_systemInfo();
u32 dLib_getTimeHigh();
void dLib_groundDestroy(u32, u32);
void dLib_groundConstruct(u32);
void dLib_addCalcAngleS2(u32, s32, s32, s32);
void dLib_vectorAdd(u32, u32, u32);
void dLib_vectorSubtract(u32, u32, u32);
void dLib_vectorScale(u32, u32, f64);
void dLib_vectorCross(u32, u32, u32);
void dLib_vectorNormZP(u32, u32);
void dLib_quatRotBaseY2(u32, u32);
void dLib_setNextStage(u32, u32, s32, s32, u32, u32, u32, f32);
void dLib_onEventBit(u32, u32);
void dLib_morfSetAnm(u32, u32, u32, u32, f32, f32, f32, f32);
void dLib_matrixYrotM(u32, s32);
void dLib_matrixTransM(f32, f32, f32);
void dLib_assert(u32, u32, u32);
void dLib_free(u32, u32);
void dLib_matrixMultVec(u32, u32, u32);
void dLib_matrixCopy(u32, u32);
void dLib_matrixTrans(u32, f32, f32, f32);
void dLib_quatSlerp(u32, u32, u32, f64);
void dLib_initByte(u32);
void dLib_initFlag(u32);
void dLib_registerStatic(u32);
void dLib_divideTicks(u32, u32, u32, u32);
void dLib_memcpy(u32, u32, u32);

void dLib_staticInit();

namespace dLib_layout {
// HD: this unit's verified game accesses use event offset 0x644 and IPL words 0x38/0x3c.
struct savedGame {
    u8 padding0[0x38];
    be<u32> mDateIPL[2];
    u8 padding40[0x604];
    be<u8> mEvent;
};
WWHD_OFFSET(savedGame, mDateIPL, 0x38);
WWHD_OFFSET(savedGame, mEvent, 0x644);
}
