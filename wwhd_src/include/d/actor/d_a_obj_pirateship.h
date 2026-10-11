/* Pirate ship: GameCube Act_c names; HD actor layout is +0x118 before its own fields.
 * HD adds the sea model/BTK before the child actors, and expands the wake callback.
 * Numeric mXXX names are the opaque fields named that way in tww. */
#pragma once
#include "f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor.h"
#include "JSystem/J3DGraph.h"
#include "m_Do/m_Do_ext.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"
// HD: these views cover the fields touched by this actor, preserving the enlarged emitter layout.
struct PirateshipShapeAttr {
    u8 bounds[0x18];
    be<u32> boundsVtable, __vtbl;
};
struct PirateshipCylinder : dCcD_GObjInf {
    PirateshipShapeAttr mShape;
    cM3dGCyl mCyl;
};
WWHD_SIZE(PirateshipCylinder, 0x130);
struct PirateshipBackground {
    u8 prefix[0xB0];
    be<u32> rideCallback;
};
struct PirateshipColorF {
    be<f32> r, g, b, a;
};
struct PirateshipColorVtable {
    u8 prefix[0x24];
    be<u32> setMatColor;
};
struct PirateshipColorBlock {
    u8 prefix[4];
    gptr<PirateshipColorVtable> __vtbl;
};
struct PirateshipMaterialColors {
    be<u32> flags;
};
struct PirateshipMaterial {
    u8 prefix[0x18];
    gptr<PirateshipColorBlock> mColorBlock;
    u8 reserved1C[0x84];
    PirateshipMaterialColors mColors;
};
struct PirateshipSafeStringVtable {
    u8 prefix[0x14];
    be<u32> assureTermination;
};
struct PirateshipActorVtable {
    u8 prefix[0x2C];
    be<u32> Draw;
};
struct PirateshipDoorParams {
    be<u32> params[2];
};
struct PirateshipStageName {
    be<u8> bytes[8];
};
struct PirateshipIndices2 {
    be<s32> values[2];
};
struct PirateshipIndices4 {
    be<s32> values[4];
};
struct PirateshipIndices5 {
    be<s32> values[5];
};
struct PirateshipIndices6 {
    be<s32> values[6];
};
struct PirateshipEmitter {
    u8 prefix[0x5C];
    be<s32> mMaxFrame;
    u8 reserved60[0x10];
    be<f32> mInitialVelDir;
    u8 reserved74[0x17C];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    GXColor mGlobalPrmColor;
    u8 reserved248[0xC];
    be<u32> mFlags;
    u8 reserved258[0xA];
    be<u8> mDrawMode;
};
WWHD_OFFSET(PirateshipEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(PirateshipEmitter, mDrawMode, 0x262);
struct dPa_trackEcallBack {
    be<u32> __vtbl;
    be<f32> mMaxWaterY, mWaterY, mWaterFlatY;
    cXyz mPos[3];
    u8 reserved34[8];
    be<f32> mIndirectOffset, mIndirectScale, mSpeed, mLimitSpeed;
    gptr<PirateshipEmitter> mpEmitter;
};
WWHD_SIZE(dPa_trackEcallBack, 0x50);
struct daObjPirateShipHIO_c {
    be<s8> mNo;
    be<u8> m05;
    be<s16> m06;
    be<f32> m08, m0C;
    be<s16> m10, m12;
    be<s8> m14;
    u8 reserved11[3];
    cXyz m18;
    be<f32> m24, m28, m2C, m30, m34, m38;
    be<u32> __vtbl;
};
WWHD_SIZE(daObjPirateShipHIO_c, 0x3C);
struct PirateshipMathStatics {
    be<f32> minimum, maximum;
    be<u8> sinInit, atanInit;
    u8 reservedA[2];
    be<u32> zero[4];
};
struct PirateshipDemoManager {
    gptr<void> pointer;
};
struct PirateshipDescriptors {
    be<u32> words[9];
};
struct PirateshipDoorOffset {
    cXyz pos;
    be<u32> initialized;
};
struct PirateshipOffset {
    gptr<const char> name;
    be<u32> params;
    cXyz pos;
    csXyz angle;
    be<u8> m1A;
    u8 reserved1B;
};
struct PirateshipOffsets {
    PirateshipOffset entries[11];
};
// HD: MoveBG tail ends at 0x3DC in this actor; sea model and texture animation are HD additions.
namespace daObjPirateship {
struct Act_c : fopAc_ac_c {
    void CreateWave();
    void DeleteWave();
    BOOL demo_move();
    void SetWave();
    BOOL Create();
    BOOL Execute(be<u32>* matrix);
    BOOL Delete();
    void pirateCreate(be<s32>* indices);
    void set_mtx();
    BOOL CreateHeap();
    BOOL Draw();
    void partsCreate();
    BOOL _delete();
    BOOL _draw();
    void sound_proc();
    BOOL _execute();
    BOOL _create();

    gptr<PirateshipBackground> mpBgW;
    u8 mBgMtxPrefix[0x2C];
    u8 reserved3DC[6];
    /* 0x3E2 */ be<s16> m2CA;
    be<u8> m2CC, m2CD, m2CE;
    u8 reserved3E7;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ request_of_phase_process_class mPhase;
    u8 reserved3F4[8];
    /* 0x3FC */ gptr<J3DModel> mSeaModel;
    /* 0x400 */ mDoExt_btkAnm mSeaBtk;
    /* 0x474 */ gptr<fopAc_ac_c> m2E4;
    /* 0x478 */ be<u32> m_sail_id;
    /* 0x47C */ gptr<fopAc_ac_c> m2EC;
    /* 0x480 */ be<u32> m2F0;
    u8 reserved484[4];
    /* 0x488 */ be<u32> m2F8;
    /* 0x48C */ gptr<fopAc_ac_c> m2FC;
    /* 0x490 */ be<u32> m300;
    /* 0x494 */ gptr<fopAc_ac_c> m304;
    /* 0x498 */ be<u32> m308;
    /* 0x49C */ cXyz m30C, m318;
    u8 reserved4B4[10];
    /* 0x4BE */ be<s16> m32E, m330;
    u8 reserved4C2[6];
    /* 0x4C8 */ dCcD_Stts m338;
    /* 0x504 */ PirateshipCylinder mCyl;
    /* 0x634 */ be<s8> m4A4;
    u8 reserved635[3];
    /* 0x638 */ gptr<dPath> m4A8;
    /* 0x63C */ dPa_trackEcallBack m4AC;
    /* 0x68C */ gptr<PirateshipEmitter> m4FC;
    /* 0x690 */ GXColor m500;
    /* 0x694 */ dBgS_ObjAcch m504;
    /* 0x858 */ cXyz m6C8;
};
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, m304, 0x494);
WWHD_OFFSET(Act_c, m4AC, 0x63C);
WWHD_OFFSET(Act_c, m6C8, 0x858);
WWHD_SIZE(Act_c, 0x864);

} // namespace daObjPirateship

// HD: checked ABI bindings used by this translation unit.
// Scalar return adapters preserve full GHS registers. Resource calls retain the legacy
// two-word SafeString and three-word resource-name temporaries; shared helpers build a
// different temporary. Teardown adapters preserve the original interior-object arguments.
// Compiler constructors operate on the explicit guest layout and are not C++ host constructors.
BOOL cLib_chasePos(cXyz*, const cXyz*, f32);
BOOL cXyz_normalizeRS(cXyz*);
void cBgS_GndChk__ct(void*);
void cBgS_GndChk__dt(void*, s32);
void cCcD_Stts__ct(void*);
void cM3dGCyl__ct(void*);
void csXyz__ct(csXyz*, s32, s32, s32);
void pirateshipLineDestruct(void*, s32);
void pirateshipAcchConstruct(void*);
void dBgS_MoveBgActor__ct(daObjPirateship::Act_c*);
s32 pirateshipMoveBGCreate(daObjPirateship::Act_c*, const char*, s32, u32, u32);
s32 pirateshipMoveBGExecute(daObjPirateship::Act_c*);
s32 pirateshipMoveBGDelete(daObjPirateship::Act_c*);
void pirateshipStatusDestruct(void*, s32);
void pirateshipAcchDestruct(void*, s32);
void dCcD_GStts__ct(void*);
void dCcD_GObjInf__ct(void*);
void dComIfG_deleteObjectRes(const char*);
u32 pirateshipDemoActor(void*, u32);
void dDemo_setDemoData(daObjPirateship::Act_c*, s32, u32, const char*, u32, u32, u32, u32);
void pirateshipDoorSetPos(void*, cXyz*, s32);
void dKy_get_seacolor(GXColor*, GXColor*);
void dLib_pathMove(cXyz*, be<s8>*, void*, f32, u32, daObjPirateship::Act_c*);
void dPa_trackEcallBack_remove(dPa_trackEcallBack*);
void pirateshipActorDestruct(void*, s32);
u32 fopAcM_createChild(s32, u32, u32, cXyz*, s32, csXyz*, void*, s32, void*);
u32 fopAcM_createChild(const char*, u32, u32, cXyz*, s32, csXyz*, void*, void*);
void fopAcM_createChildFromOffset(const char*, u32, u32, cXyz*, s32, csXyz*, void*, void*);
void mDoAud_seStart(u32, cXyz*);
void mDoAud_seDeleteObject(cXyz*);
void* pirateshipObjectResource(void*, void*, s32);
void* pirateshipNamedResource(void*, void*, void*);
void pirateshipAssert(const char*, s32, const char*);
void* operator_new(u32);
void pirateshipConvertSeaColor(void*, const void*, f32);
PirateshipColorF* pirateshipMaterialColor(void*, s32);
void JPAGetXYZRotateMtx(s32, s32, s32, Mtx34*);
void pirateshipInitAtanTable(be<u8>*);
void pirateshipInitSinTable(be<u8>*);
void __construct_array(void*, s32, u32, u32);
void pirateshipInitDescriptor(void*);
s32 pirateshipGroundRoom(dBgS*, cBgS_PolyInfo*);

// HD: per-TU GHS vtables and engine callback tokens.
enum : u32 {
    VT_SAFE_STRING = 0x1002E818,
    VT_HIO = 0x1002E8C0,
    VT_GROUND_CHECK = 0x1002E850,
    VT_GROUND_POLY_INFO = 0x1002E860,
    VT_GROUND_POLY_PASS = 0x1002E880,
    VT_GROUND_GROUP_PASS = 0x1002E870,
    VT_GROUP_PASS_BASE = 0x1002E840,
    VT_ACTOR = 0x1002EABC,
    VT_STATUS = 0x1004AE88,
    VT_GLOBAL_STATUS = 0x1004AEC0,
    VT_SHAPE_BASE = 0x100015A8,
    VT_CYLINDER_BOUNDS = 0x1002E830,
    VT_CYLINDER_GEOMETRY = 0x1004B150,
    VT_CYLINDER_HIT = 0x1004B108,
    VT_CYLINDER_SHAPE = 0x1004B160,
    VT_TRACK = 0x10052268,
    VT_ACCH_GROUP_PASS = 0x1002E8A0,
    VT_ACCH_CHECK = 0x1002E890,
    VT_ACCH_POLY_PASS = 0x1002E8B0,
    FN_SEARCH_BY_ID = 0x025E1234,
    FN_MOVE_BG_TYPICAL_ROT_Y = 0x024EE708,
};
enum {
    PIRATESHIP_FIRST_ISLAND = 0,
    PIRATESHIP_UMIKZ = 1,
    PIRATESHIP_ARRIVE_MJ = 2,
    PIRATESHIP_ONLY = 3,
    PIRATESHIP_YUUKAIGO = 4,
    KAIZOKUSEN_DZB_ROOM1A = 5,
    KAIZOKUSEN_DZB_ROOM1B = 6,
    KAIZOKUSEN_BDL_SHIP = 14,
    PIRATESHIP_TAIL = 0x8315,
    PIRATESHIP_SPLASH = 0x833D,
    fpcNm_PIRATE_FLAG_e = 0xAD,
    fpcNm_Obj_Tousekiki_e = 0x3A,
    fpcNm_Kaji_e = 0x3C,
    fpcNm_KNOB00_e = 0x131,
    JA_SE_OBJ_PIRATE_CRUISE = 0x7034,
    JA_SE_OBJ_PIRATE_SAIL = 0x50BE,
};
void ride_call_back(void*, void*, fopAc_ac_c*);
BOOL path_move_call_back(cXyz*, cXyz*, cXyz*, daObjPirateship::Act_c*);
void pirateship_stringNoop(void*);
void* pirateship_vectorCtor(void*);

WWHD_OFFSET(daObjPirateship::Act_c, mpBgW, 0x3AC);
WWHD_OFFSET(daObjPirateship::Act_c, m2CA, 0x3E2);
WWHD_OFFSET(daObjPirateship::Act_c, mPhase, 0x3EC);
WWHD_OFFSET(daObjPirateship::Act_c, mSeaModel, 0x3FC);
WWHD_OFFSET(daObjPirateship::Act_c, mSeaBtk, 0x400);
WWHD_OFFSET(daObjPirateship::Act_c, m_sail_id, 0x478);
WWHD_OFFSET(daObjPirateship::Act_c, m338, 0x4C8);
WWHD_OFFSET(daObjPirateship::Act_c, mCyl, 0x504);
WWHD_OFFSET(daObjPirateship::Act_c, m4A4, 0x634);
WWHD_OFFSET(daObjPirateship::Act_c, m4A8, 0x638);
WWHD_OFFSET(daObjPirateship::Act_c, m4FC, 0x68C);
WWHD_OFFSET(daObjPirateship::Act_c, m500, 0x690);
WWHD_OFFSET(daObjPirateship::Act_c, m504, 0x694);
