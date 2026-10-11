// Bombable walls. Private game-derived source; verified by d_a_wall.
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/f_op/f_op_actor_mng.h"

// HD: typed views of the unchanged 0x150-byte collision triangle.
struct WallTriangleGeometry {
    /* 0x00 */ cXyz mNormal;
    /* 0x0C */ be<f32> mD;
    /* 0x10 */ be<u32> __vtbl;
    /* 0x14 */ cXyz mA, mB, mC;
    void setPos(cXyz*, cXyz*, cXyz*);
};
struct WallShapeAttr {
    cXyz mMin, mMax;
    be<u32> aabVtable, shapeVtable;
};
struct WallTriangle : dCcD_GObjInf {
    /* 0x0F8 */ WallShapeAttr mShapeAttr;
    /* 0x118 */ WallTriangleGeometry mTri;
    void Set(const dCcD_SrcTri*);
};
WWHD_SIZE(WallTriangleGeometry, 0x38);
WWHD_OFFSET(WallTriangleGeometry, __vtbl, 0x10);
WWHD_SIZE(WallShapeAttr, 0x20);
WWHD_OFFSET(WallTriangle, mShapeAttr, 0xF8);
WWHD_OFFSET(WallTriangle, mTri, 0x118);
WWHD_SIZE(WallTriangle, 0x150);

// HD: emitter tail used by the smoke's immortal flag and alpha update.
struct WallEmitter {
    u8 mEmitterState[0x247];
    be<u8> mGlobalAlpha;
    u8 mEmitterTail[0xC];
    be<u32> mFlags;
    enum { STATUS_IMMORTAL_EMITTER = 0x40 };
};
WWHD_OFFSET(WallEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(WallEmitter, mFlags, 0x254);
WWHD_SIZE(WallEmitter, 0x258);

struct daWall_c : fopAc_ac_c {
    s32 _delete();
    s32 CreateHeap();
    void CreateInit();
    s32 _create();
    void set_mtx();
    void setMoveBGMtx();
    s32 _execute();
    void mode_wait();
    void mode_break();
    void set_tri();
    void set_effect();
    void set_se();
    s32 _draw();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ WallTriangle mTri[2];
    /* 0x694 */ gptr<dBgW> mpBgW;
    /* 0x698 */ Mtx34 mMtx;
    /* 0x6C8 */ be<u8> mState;
    u8 mStatePadding[3];
    /* 0x6CC */ dPa_smokeEcallBack mSmokeCb;
    /* 0x6EC */ gptr<WallEmitter> mpEmitter;
    /* 0x6F0 */ be<f32> mDst;
    /* 0x6F4 */ be<u8> mBreakCounter;
    u8 mCounterPadding[3];
    /* 0x6F8 */ be<s32> mSwitchNo;
    /* 0x6FC */ be<u8> mType;
    u8 mTypePadding[3];
};
WWHD_OFFSET(daWall_c, mPhs, 0x3AC);
WWHD_OFFSET(daWall_c, mpModel, 0x3B4);
WWHD_OFFSET(daWall_c, mStts, 0x3B8);
WWHD_OFFSET(daWall_c, mTri, 0x3F4);
WWHD_OFFSET(daWall_c, mpBgW, 0x694);
WWHD_OFFSET(daWall_c, mMtx, 0x698);
WWHD_OFFSET(daWall_c, mState, 0x6C8);
WWHD_OFFSET(daWall_c, mSmokeCb, 0x6CC);
WWHD_OFFSET(daWall_c, mpEmitter, 0x6EC);
WWHD_OFFSET(daWall_c, mDst, 0x6F0);
WWHD_OFFSET(daWall_c, mBreakCounter, 0x6F4);
WWHD_OFFSET(daWall_c, mSwitchNo, 0x6F8);
WWHD_OFFSET(daWall_c, mType, 0x6FC);
WWHD_SIZE(daWall_c, 0x700);

// HD: keep the linkage area below each original payload; callees may save LR there.
template<class T> struct WallStackSlot { u8 linkage[16]; T payload; };
template<class T> struct WallStackPayload {
    gabi::Local<WallStackSlot<T>> storage;
    T* get() { return &storage->payload; }
};
struct WallVertices { cXyz values[3]; };
struct WallParticleIDs { be<u16> values[3]; };
struct WallParameters { u8 upper[3]; be<u8> switchNo; };
struct WallResourceTable { be<s16> values[3]; };
struct WallArchiveTable { gptr<char> names[3]; };
struct WallVertexTable { cXyz values[3][4]; };
struct WallCullTable { cXyz values[3][2]; };
using WallModeFunc = void (daWall_c::*)();
struct WallModeTable { gabi::ptmf<WallModeFunc> entries[2]; };
struct WallSourceTriangle { u8 data[0x54]; };
struct WallHeaderStatics { be<f32> minimum, maximum; u8 first, second, padding[2]; be<u32> zero[4]; };
struct WallDestructorRecords { u8 entries[3][12]; };
WWHD_SIZE(WallVertices, 36);
WWHD_SIZE(WallParticleIDs, 6);
WWHD_SIZE(WallParameters, 4);
WWHD_SIZE(WallResourceTable, 6);
WWHD_SIZE(WallArchiveTable, 12);
WWHD_SIZE(WallVertexTable, 0x90);
WWHD_SIZE(WallCullTable, 0x48);
WWHD_SIZE(WallModeTable, 16);
WWHD_SIZE(WallSourceTriangle, 0x54);
WWHD_OFFSET(WallHeaderStatics, minimum, 0);
WWHD_OFFSET(WallHeaderStatics, maximum, 4);
WWHD_OFFSET(WallHeaderStatics, first, 8);
WWHD_OFFSET(WallHeaderStatics, second, 9);
WWHD_OFFSET(WallHeaderStatics, zero, 0xC);
WWHD_SIZE(WallHeaderStatics, 0x1C);
WWHD_SIZE(WallDestructorRecords, 0x24);

enum { WALL_WAIT, WALL_BREAK, JA_SE_OBJ_BOMB_WALL_BRK = 0x696C };
namespace WallCollision { enum { AT_TYPE_BOMB = 0x20 }; }
namespace WallParticleName {
enum { ID_IT_ST_SHIREN_WL1_HAHEN00 = 0xA16E, ID_IT_ST_SHIREN_WL1_SMOKE00,
       ID_IT_ST_SHIREN_WL2_HAHEN00, ID_IT_ST_SHIREN_WL2_SMOKE00,
       ID_IT_ST_SHIREN_WL3_HAHEN00, ID_IT_ST_SHIREN_WL3_SMOKE00 };
}
void WallTriangleArray_ct(WallTriangle*, u32, u32, u32);
void WallTriangleArray_dt(WallTriangle*, u32, u32, u32, u32, u32);
void WallSmokeCallback_ct(dPa_smokeEcallBack*, const GXColor*, void*, u32);
void WallGObjInf_ct(WallTriangle*);
void WallTriangleGeometry_ct(WallTriangleGeometry*);
s32 WallBgW_Set(dBgW*, cBgD_t*, u32, Mtx34*);
s32 Wall_getReverb(s32);
void WallFirstHeaderObject_ct(u8*);
void WallSecondHeaderObject_ct(u8*);

void* Wall_getRes(dRes_control_c*, SafeString*, s32);
