/* WWHD NPC path/runaway utility. Derived source. Layout unchanged from
 * GC. */
#pragma once
#include "bindings.h"
// HD: private dNpc_PathRun_c view; keep the existing type identity for Mk/Uk consumers.
struct MkPath_l {
    union { gptr<void> mPath; gptr<void> path; };
    union { be<u8> field_0x04; be<u8> _04; };
    union { be<u8> mIdx; be<u8> index; };
    union { be<u8> mbDir; be<u8> _06; };
    union { be<u8> field_0x07; be<u8> _07; };
};
using MkPathRun = MkPath_l;
WWHD_SIZE(MkPathRun, 8);
WWHD_OFFSET(MkPathRun, mPath, 0);
WWHD_OFFSET(MkPathRun, mIdx, 5);
struct daNpc_Mk_Static_c {
    union { be<u8> m0; be<u8> state; };
    u8 _01[3];
    union { be<f32> m4; be<f32> speedScale; };
    union { be<u16> m8; be<u16> timer; };
    union { be<u16> ma; be<u16> timerMax; };
    union { be<u8> mPointIndex1; be<u8> pointIndex; };
    union { be<u8> md; be<u8> accelerating; };
    union { be<u8> me; be<u8> cooldown; };
    union { be<u8> mPointIndex2; be<u8> oldPointIndex; };
    u32 turnPath(fopAc_ac_c *, MkPathRun *, u8);
    BOOL chkPath(fopAc_ac_c *, MkPathRun *, u8);
    BOOL walkPath(fopAc_ac_c *, MkPathRun *, u8);
    void aroundWalk(fopAc_ac_c *, fopAc_ac_c *, u8);
    f32 getSpeedF(f32, f32);
    void init(u8, u16);
    void runaway_com2(MkPathRun *, u8);
    u8 goFarLink_2(fopAc_ac_c *, MkPathRun *);
    u8 goFarLink_3(fopAc_ac_c *, MkPathRun *);
    u8 runAwayProc(fopAc_ac_c *, MkPathRun *, void *, be<s16> *);
    BOOL chkGameSet();
    void setRndPathPos(fopAc_ac_c *, MkPathRun *);
    BOOL chkPointPass(cXyz *, cXyz *, cXyz *);
};
WWHD_SIZE(daNpc_Mk_Static_c, 0x10);
WWHD_OFFSET(daNpc_Mk_Static_c, speedScale, 4);
WWHD_OFFSET(daNpc_Mk_Static_c, timer, 8);
WWHD_OFFSET(daNpc_Mk_Static_c, pointIndex, 0xC);
WWHD_OFFSET(daNpc_Mk_Static_c, m0, 0);
WWHD_OFFSET(daNpc_Mk_Static_c, ma, 0xA);
WWHD_OFFSET(daNpc_Mk_Static_c, md, 0xD);
WWHD_OFFSET(daNpc_Mk_Static_c, me, 0xE);
WWHD_OFFSET(daNpc_Mk_Static_c, mPointIndex2, 0xF);

struct MkHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mSdkByteA, mSdkByteB, padding[2];
    be<u32> mZeroVector[4];
};
WWHD_OFFSET(MkHeaderStatics, mZeroVector, 0xC);
WWHD_SIZE(MkHeaderStatics, 0x1C);
s16 Mk_targetAngle(cXyz*, cXyz*);
void Mk_subtract(cXyz*, cXyz*, cXyz*);
f32 Mk_squareMagnitude(cXyz*);
f32 Mk_sqrt(f32);
s32 Mk_segmentDistance(f32, f32, f32, f32, f32, f32, be<f32>*, be<f32>*, be<f32>*);
void Mk_addAngle(be<s16>*, s16, s16, s16, s16);
s32 Mk_pathMax(MkPathRun*);
f32 Mk_random(f32);
void Mk_groundConstruct(dBgS_GndChk*);
void Mk_groundDestruct(dBgS_GndChk*, s32);
void Mk_sdkByteA(u8*);
void Mk_sdkByteB(u8*);
