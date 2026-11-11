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
  /* 0x00 */ be<f32> mThreshold;  /* was field_0x04: TP name (zeldaret/tp include/d/d_lib.h STControl::mThreshold); setWaitParm 025885C4 'high'; checkTrigger 0258873C takes the new direction bits while the stick value is >= it (tww the same) */
  /* 0x04 */ be<f32> mReleaseThreshold;  /* was field_0x08: setWaitParm 025885C4 'low'; checkTrigger 0258873C clears the direction bits while the stick value is below it, between it and mThreshold only drops bits (tww the same; TP leaves it unnamed) */
  /* 0x08 */ be<u8> mDirectionTrig;  /* was field_0x0c: TP name (zeldaret/tp include/d/d_lib.h STControl::mDirectionTrig); checkTrigger 0258873C sets the LEFT / RIGHT / UP / DOWN bits and returns them; the check*Trigger 02588A0C.. test them; Xinit / Yinit clear their half (tww the same) */
  /* 0x09 */ be<u8> mPrevDirectionTrig;  /* was field_0x0d: checkTrigger 0258873C copies mDirectionTrig into it first, then compares (a change resets mAngleShift) and counts the repeat timers only for bits held in both (tww the same); init 025885E8 clears */
  /* 0x0A */ be<s16> mXrepeatTimer;  /* was field_0x0e: checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C fire only while it is 0 and reload it with mXwaitTimer; checkTrigger 0258873C counts it down while left / right is held; Xinit 025886EC clears (tww the same) */
  /* 0x0C */ be<s16> mYrepeatTimer;  /* was field_0x10: checkUpTrigger 02588AEC / checkDownTrigger 02588B5C fire only while it is 0 and reload it with mYwaitTimer; checkTrigger 0258873C counts it down while up / down is held; Yinit 02588714 clears (tww the same) */
  /* 0x0E */ be<s16> mRepeatDelayY;  /* was field_0x12: TP name (zeldaret/tp include/d/d_lib.h STControl::mRepeatDelayY); setWaitParm 025885C4 'first'; init 025885E8 / Xinit 025886EC / Yinit 02588714 reset mXwaitTimer and mYwaitTimer to it (tww the same) */
  /* 0x10 */ be<s16> mRepeatDelayX;  /* was field_0x14: TP name (zeldaret/tp include/d/d_lib.h STControl::mRepeatDelayX); setWaitParm 025885C4 'minimum'; the check*Trigger 02588A0C.. never let mXwaitTimer / mYwaitTimer drop below it (tww the same) */
  /* 0x12 */ be<s16> mRepeatDelayStep;  /* was field_0x16: setWaitParm 025885C4 'step'; the check*Trigger 02588A0C.. subtract it from mXwaitTimer / mYwaitTimer per repeat once the step delay count is 0 (tww the same; TP leaves it unnamed) */
  /* 0x14 */ be<s16> mXwaitTimer;  /* was field_0x18: TP name (zeldaret/tp include/d/d_lib.h STControl::mXwaitTimer); checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C load mXrepeatTimer with it and shrink it by mRepeatDelayStep; init / Xinit 025886EC reset it to mRepeatDelayY (tww the same) */
  /* 0x16 */ be<s16> mYwaitTimer;  /* was field_0x1a: TP name (zeldaret/tp include/d/d_lib.h STControl::mYwaitTimer); checkUpTrigger 02588AEC / checkDownTrigger 02588B5C load mYrepeatTimer with it and shrink it by mRepeatDelayStep; init / Yinit 02588714 reset it to mRepeatDelayY (tww the same) */
  /* 0x18 */ be<s16> mStepDelayCount;  /* was field_0x1c: setWaitParm 025885C4 'delay'; init 025885E8 / Xinit 025886EC / Yinit 02588714 copy it into mXstepDelayCount / mYstepDelayCount (tww the same; TP leaves it unnamed) */
  /* 0x1A */ be<s16> mXstepDelayCount;  /* was field_0x1e: checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C count it down per repeat and only shrink mXwaitTimer once it is 0; init / Xinit 025886EC load mStepDelayCount (tww the same) */
  /* 0x1C */ be<s16> mYstepDelayCount;  /* was field_0x20: checkUpTrigger 02588AEC / checkDownTrigger 02588B5C count it down per repeat and only shrink mYwaitTimer once it is 0; init / Yinit 02588714 load mStepDelayCount (tww the same) */
  /* 0x1E */ be<s16> mAngleShift;  /* was field_0x22: checkTrigger 0258873C adds it to every direction sector bound; when the direction bits change it becomes +mAngleMargin or -mAngleMargin (by the half of the sector the stick is in), 0 when released; init 025885E8 clears (tww the same) */
  /* 0x20 */ be<s16> mAngleMargin;  /* was field_0x24: setWaitParm 025885C4 'margin'; checkTrigger 0258873C sets mAngleShift to +/- it when the direction changes (tww the same; TP leaves it unnamed) */
  /* 0x22 */ be<s16> mDiagonalWidth;  /* was field_0x26: setWaitParm 025885C4 'sector'; checkTrigger 0258873C: the bounds move by (0x2000 - it) >> 1, so each diagonal sector is it wide (0x2000 default) (tww the same; TP leaves it unnamed) */
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
