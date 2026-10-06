/* WWHD NPC path/runaway utility. Derived source. Layout unchanged from
 * GC. */
#pragma once
#include "bindings.h"
struct MkPath_l {
  gptr<void> path;
  be<u8> _04, index, _06, _07;
};
WWHD_SIZE(MkPath_l, 8);
struct daNpc_Mk_Static_c {
  be<u8> state;
  u8 _01[3];
  be<f32> speedScale;
  be<u16> timer, timerMax;
  be<u8> pointIndex, accelerating, cooldown, oldPointIndex;
  u32 turnPath(fopAc_ac_c *, MkPath_l *, u8);
  BOOL chkPath(fopAc_ac_c *, MkPath_l *, u8);
  BOOL walkPath(fopAc_ac_c *, MkPath_l *, u8);
  void aroundWalk(fopAc_ac_c *, fopAc_ac_c *, u8);
  f32 getSpeedF(f32, f32);
  void init(u8, u16);
  void runaway_com2(MkPath_l *, u8);
  u8 goFarLink_2(fopAc_ac_c *, MkPath_l *);
  u8 goFarLink_3(fopAc_ac_c *, MkPath_l *);
  u8 runAwayProc(fopAc_ac_c *, MkPath_l *, void *, be<s16> *);
  BOOL chkGameSet();
  void setRndPathPos(fopAc_ac_c *, MkPath_l *);
  BOOL chkPointPass(cXyz *, cXyz *, cXyz *);
};
WWHD_SIZE(daNpc_Mk_Static_c, 0x10);
WWHD_OFFSET(daNpc_Mk_Static_c, speedScale, 4);
WWHD_OFFSET(daNpc_Mk_Static_c, timer, 8);
WWHD_OFFSET(daNpc_Mk_Static_c, pointIndex, 0xC);
