#pragma once
#include "gc_api/gc_api.h"
namespace daComing3 {
struct Act_c : fopAc_ac_c {
    BOOL get_water_height(be<f32> *, be<s32> *, const cXyz *);
    s32 get_challenge_id();
    f32 get_limit_dist();
    BOOL coming_start_init();
    void coming_start_main();
    BOOL coming_game_init();
    void coming_game_main();
    BOOL coming_wait_init();
    void coming_wait_main();
    // HD: the entry compares the full signed r4, before storing the short index.
    bool coming_process_init(s32);
    void eff_break_tsubo();
    void coming_process_main();
    void set_mtx();
    void collision_init();
    bool collision_main();
    bool create_heap();
    s32 _create();
    bool _delete();
    bool _execute();
    bool _draw();
    /* 0x3AC */ dCcD_Stts mStts;
    /* 0x3E8 */ dCcD_Cyl mCyl;
    /* 0x518 */ be<u32> unk_400;
    /* 0x51C */ be<s32> unk_404;
    /* 0x520 */ request_of_phase_process_class mPhase;
    /* 0x528 */ Mtx34 unk_410;
    /* 0x558 */ gptr<J3DModel> unk_440;
    /* 0x55C */ be<s16> unk_444;
    /* 0x55E */ u8 unk446[0x32];
    /* 0x590 */ be<f32> unk_478;
    /* 0x594 */ be<s16> unk_47C;
    /* 0x596 */ be<s16> unk_47E;
};
WWHD_OFFSET(Act_c, mStts, 0x3AC);
WWHD_OFFSET(Act_c, mCyl, 0x3E8);
WWHD_OFFSET(Act_c, unk_400, 0x518);
WWHD_OFFSET(Act_c, unk_440, 0x558);
WWHD_OFFSET(Act_c, unk_478, 0x590);
WWHD_SIZE(Act_c, 0x598);
struct SafetyCallback {
    gptr<cXyz> pos;
    be<s32> ret;
};
WWHD_SIZE(SafetyCallback, 8);
} // namespace daComing3
using daComing3::SafetyCallback;
// HD: partial views of the collision constructors and barrel fields used here.
struct Coming3CtorHD {
    u8 pad0[0x4F8];
    be<u32> mShapeBaseVtable, mShapeVtable;
};
WWHD_OFFSET(Coming3CtorHD, mShapeVtable, 0x4FC);
inline Coming3CtorHD *coming3Ctor(daComing3::Act_c *actor) {
    return reinterpret_cast<Coming3CtorHD *>(actor);
}
struct Coming3BarrelHD : fopAc_ac_c {
    u8 pad3AC[0x574 - 0x3AC];
    be<u32> mItemId;
    u8 pad578[0x590 - 0x578];
    be<u8> mExitReq;
};
WWHD_OFFSET(Coming3BarrelHD, mItemId, 0x574);
WWHD_OFFSET(Coming3BarrelHD, mExitReq, 0x590);
struct Coming3MethodHD {
    be<s16> adjustment, slot;
    be<u32> target;
};
WWHD_SIZE(Coming3MethodHD, 8);
struct Coming3GroundHD {
    gptr<u8> mGroup, mPoly;
    be<u32> mActorPid;
    u8 padC[4];
    be<u32> mVtable;
    u8 mPolyInfo[12];
    be<u32> mCheckVtable;
    cXyz mPos;
    u8 pad30[0x40 - 0x30];
    be<u32> mGroupVtable;
    be<u8> mGroupFlags[8];
    be<u32> mPolyVtable, mFlags;
};
WWHD_OFFSET(Coming3GroundHD, mPos, 0x24);
WWHD_OFFSET(Coming3GroundHD, mPolyVtable, 0x4C);
WWHD_SIZE(Coming3GroundHD, 0x54);
struct Coming3WaterHD {
    u8 pad0[0x20];
    be<u32> mVtable, mGroupVtable;
    u8 pad28[8];
    be<u32> mPolyVtable;
    u8 pad34[4];
    cXyz mPos;
    be<f32> mUpper, mHeight;
};
WWHD_OFFSET(Coming3WaterHD, mPos, 0x38);
WWHD_SIZE(Coming3WaterHD, 0x4C);
struct Coming3ParticleHD {
    u8 pad0[0x130];
    gptr<u8> mpModelEmitterList;
};
WWHD_OFFSET(Coming3ParticleHD, mpModelEmitterList, 0x130);
inline be<u32> &coming3Word(be<f32> *f) { return *reinterpret_cast<be<u32> *>(f); }

void coming3Panic(const char *, s32, const char *);
J3DModel *coming3CreateModel(void *, u32, u32);
void coming3SttsInit(void *, s32, s32, fopAc_ac_c *);
void coming3CylSet(void *, const void *);
void coming3MatrixZXYRotate(void *, s32, s32, s32);
void coming3MatrixCopy(const void *, void *);
void coming3ModelCalc(J3DModel *);
void coming3ActorConstruct(fopAc_ac_c *);
void coming3SttsConstruct(void *);
void coming3GSttsConstruct(void *);
void coming3CollisionConstruct(void *);
void coming3CylinderConstruct(void *);
s32 coming3ResourceLoad(void *, const char *);
s32 coming3SolidHeap(fopAc_ac_c *, u32, u32);
void coming3ResourceDelete(void *, const char *);
void coming3SettingTevStruct(void *, u32, const cXyz *, void *);
void coming3SetLightTevColor(void *, J3DModel *, void *);
void coming3ModelUpdate(J3DModel *, u32);
s32 coming3SearchByID(u32, void *);
void coming3ActorDelete(fopAc_ac_c *);
void coming3GroundDestroy(void *, s32);
void coming3Delete(void *);
void coming3CheckDestroy(void *, s32);
void coming3CollisionDestroy(void *, s32);
void coming3SttsDestroy(void *, s32);
void coming3ActorDestroy(fopAc_ac_c *, s32);
s32 coming3CheckTargetHit(void *);
void coming3ClearTargetHit(void *);
void coming3MatrixVector(const void *, const cXyz *, cXyz *);
void coming3CylinderRadius(void *, f32);
void coming3CylinderHeight(void *, f32);
void coming3CylinderCenter(void *, const cXyz *);
void coming3CollisionSet(void *, void *);
s32 coming3IsActor(fopAc_ac_c *);
void coming3GroundConstruct(void *);
void coming3RegisterStatic(void *);
void coming3WaterConstruct(void *);
s32 coming3SeaCheckArea(f32, f32);
f32 coming3SeaWave(f32, f32);
s32 coming3WaterCheck(void *, void *);
s32 coming3RoomReadId(void *, void *);
f32 coming3GroundCross(void *, void *);
s32 coming3MaterialSoundId(void *, void *);
s32 coming3Reverb(s32);
void coming3SoundStart(u32, const cXyz *, u32, s32);
void *coming3ParticleSet(void *, u32, u32, const cXyz *, const csXyz *, const cXyz *, u32, void *,
                         s32, const void *, const void *, u32);
void *coming3ModelEmitterConstruct(void *, void *, void *, u32, void *, void *, u32, u32);
void coming3AppendEmitter(void *, void *);
f32 coming3SquareDistance(const cXyz *, const cXyz *);
f32 coming3SquareRoot(f32);
void coming3ActorSearch(u32, void *);
void coming3AngleConstruct(csXyz *, s32, s32, s32);
u32 coming3ActorCreate(s32, u32, const cXyz *, s32, const csXyz *, void *, s32, u32);

u8 coming3CreateHeapResult(daComing3::Act_c *);

// HD: retain this TU's guest vtables and callback targets, rather than host
// function/member pointers; eight-byte GHS dispatch is not yet supported.
enum : u32 {
    COMING3_HEAP_CALLBACK = 0x02114B28,
    COMING3_SAFETY_CALLBACK = 0x021156E4,
    COMING3_ACTOR_VTABLE = 0x1000CAB8,
    COMING3_STTS_VTABLE = 0x1004AE88,
    COMING3_GSTTS_VTABLE = 0x1004AEC0,
    COMING3_SHAPE_BASE_VTABLE = 0x100015A8,
    COMING3_SHAPE_VTABLE = 0x1000C9B8,
    COMING3_CYL_HIT_VTABLE = 0x1004B108,
    COMING3_SHAPE_FINAL_VTABLE = 0x1004B160,
    COMING3_CYL_VTABLE = 0x1004B150,
    COMING3_OBJ_GROUND_VTABLE = 0x1000CA18,
    COMING3_GROUND_POLY_VTABLE = 0x1000CA38,
    COMING3_GROUND_CHECK_VTABLE = 0x1000CA28,
    COMING3_GROUND_GROUP_VTABLE = 0x1000CA48,
    COMING3_GROUND_CHECK_BASE_VTABLE = 0x1000C9E8,
    COMING3_GROUND_GROUP_BASE_VTABLE = 0x1000CA08,
    COMING3_POLY_BASE_VTABLE = 0x1000C9C8,
    COMING3_WATER_CHECK_BASE_VTABLE = 0x1000CA58,
    COMING3_WATER_GROUP_BASE_VTABLE = 0x1000CA78,
};

WWHD_OFFSET(Coming3WaterHD, mVtable, 0x20);
WWHD_OFFSET(Coming3WaterHD, mHeight, 0x48);
WWHD_OFFSET(Coming3GroundHD, mFlags, 0x50);
WWHD_OFFSET(Coming3GroundHD, mCheckVtable, 0x20);
