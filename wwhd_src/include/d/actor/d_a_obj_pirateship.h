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
    be<u8> mNoPathMove;  /* was m05: executeShip (in daObjPirateship::Act_c::_execute 02385B70): the path move and the sway run only while it is 0 (tww _execute the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> m06;
    be<f32> mPathSpeed;  /* was m08: path_move_call_back 02384078: the speed toward the next path point is (it + 5) * |cos| of the turn left (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (2) */
    be<f32> mSwayHeight;  /* was m0C: executeShip (in daObjPirateship::Act_c::_execute 02385B70): pos.y = home.pos.y + (it + 10) * sin(mSwayAngle) (tww _execute the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (100) */
    be<s16> mSwaySpeedRising;  /* was m10: executeShip (in daObjPirateship::Act_c::_execute 02385B70): mSwaySpeed's target while mSwayAngle is within +-0x4000 (the ship rising) (tww _execute the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (250) */
    be<s16> mSwaySpeedFalling;  /* was m12: executeShip (in daObjPirateship::Act_c::_execute 02385B70): mSwaySpeed's target while mSwayAngle is outside +-0x4000 (the ship sinking) (tww _execute the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (500) */
    be<s8> m14;
    u8 reserved11[3];
    cXyz mWakeOffset;  /* was m18: daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's position in ship space (mWakePos is the ship matrix times it) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (0, 0, 0) */
    be<f32> mWakeScale;  /* was m24: daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's particle and dynamics scale (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (4) */
    be<f32> mWakeIndirectOffset;  /* was m28: daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mIndirectOffset (tww setIndirectTexData's first argument) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (-0.02) */
    be<f32> mWakeIndirectScale;  /* was m2C: daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mIndirectScale (tww setIndirectTexData's second argument) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (5) */
    be<f32> mWakeSpeed;  /* was m30: daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mSpeed (tww setSpeed) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (300) */
    be<f32> mWakeLimitSpeed;  /* was m34: daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mLimitSpeed (tww setLimitSpeed) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (3) */
    be<f32> mWakeDirectionalSpeed;  /* was m38: daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's mInitialVelDir (tww setDirectionalSpeed) (tww the same); pirateship_sinit 02385FA0 sets the default, never written elsewhere in WWHD or tww (17) */
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
    be<u8> mAppeared, mPiratesCreated, mSailSeOn;  /* mAppeared was m2CC: executeShip (inlined into _execute 02385B70 / _create 023852E0): while 0 the ship waits for the demo and event 0x0310, then creates the wake, registers the dBgW and sets 1; Draw 02384FB4 draws only when set; sound_proc 02384A78 runs only when 1; mPiratesCreated was m2CD: pirateCreate 023844DC sets 1; executeShip (_execute 02385B70) calls pirateCreate(create_idx_tbl_1stIsland_demo) once event 0x0001 is set and it is still 0; mSailSeOn was m2CE: _create 023852E0 sets (mParameters & 15) != 0; sound_proc 02384A78 starts JA_SE_OBJ_PIRATE_SAIL only when set */
    u8 reserved3E7;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ request_of_phase_process_class mPhase;
    u8 reserved3F4[8];
    /* 0x3FC */ gptr<J3DModel> mSeaModel;
    /* 0x400 */ mDoExt_btkAnm mSeaBtk;
    /* 0x474 */ gptr<fopAc_ac_c> mpSail;  /* was m2E4: executeShip (_execute 02385B70): searchPart(m_sail_id, it), the actor of the Psail child */
    /* 0x478 */ be<u32> m_sail_id;
    /* 0x47C */ gptr<fopAc_ac_c> mpFlag;  /* was m2EC: executeShip (_execute 02385B70): searchPart(mFlagId, it), the pirate flag child */
    /* 0x480 */ be<u32> mFlagId;  /* was m2F0: partsCreate 0238418C: fopAcM_createChild(fpcNm_PIRATE_FLAG_e, ...) */
    u8 reserved484[4];
    /* 0x488 */ be<u32> mTousekikiId;  /* was m2F8: partsCreate 0238418C: fopAcM_createChild(fpcNm_Obj_Tousekiki_e, ...) (the catapult) */
    /* 0x48C */ gptr<fopAc_ac_c> mpKaji;  /* was m2FC: executeShip (_execute 02385B70): searchPart(mKajiId, it), the steering wheel (Kaji) child */
    /* 0x490 */ be<u32> mKajiId;  /* was m300: partsCreate 0238418C: fopAcM_createChild(fpcNm_Kaji_e, ...); TWW accessor getKajiID() { return m300; } (zeldaret/tww d_a_obj_pirateship.h); view PirateShipView::mKajiId (d_a_npc_p1.h) */
    /* 0x494 */ gptr<fopAc_ac_c> mpDoor;  /* was m304: partsCreate 0238418C clears it; set_mtx 02384690 searches mDoorId and then places the door (pirateshipDoorSetPos) 400 up / 475 ahead; TWW type daKnob00_c* */
    /* 0x498 */ be<u32> mDoorId;  /* was m308: partsCreate 0238418C: fopAcM_createChild(fpcNm_KNOB00_e, ...); set_mtx 02384690 searches it */
    /* 0x49C */ cXyz mCruiseSePos, mSailSePos;  /* mCruiseSePos was m30C: sound_proc 02384A78: (0, 800, 2000) in ship space, position of JA_SE_OBJ_PIRATE_CRUISE; _delete 02385AEC mDoAud_seDeleteObject; mSailSePos was m318: sound_proc 02384A78: (0, 1500, 300) in ship space, position of JA_SE_OBJ_PIRATE_SAIL; _delete 02385AEC mDoAud_seDeleteObject */
    u8 reserved4B4[10];
    /* 0x4BE */ be<s16> mSwayAngle, mSwaySpeed;  /* mSwayAngle was m32E: executeShip (_execute 02385B70): adds mSwaySpeed each frame, pos.y = home.y + amplitude * sin(it); after the angle parameter of daObjPirateship::getShipOffsetY 025269A8 (d_com_static.cpp); mSwaySpeed was m330: executeShip (_execute 02385B70): cLib_addCalcAngleS2 towards the HIO speed by the sway half; after the speed parameter of daObjPirateship::getShipOffsetY 025269A8 */
    u8 reserved4C2[6];
    /* 0x4C8 */ dCcD_Stts mStts;  /* was m338: _create 023852E0 constructs the dCcD_Stts; pirateship_Act_c_dt 02386148 destroys it */
    /* 0x504 */ PirateshipCylinder mCyl;
    /* 0x634 */ be<s8> mPathPntNo;  /* was m4A4: executeShip (_execute 02385B70): dLib_pathMove's point number (s8*) argument; _create 023852E0 clears it */
    u8 reserved635[3];
    /* 0x638 */ gptr<dPath> mpPath;  /* was m4A8: _create 023852E0: dPath_GetRoomPath; executeShip (_execute 02385B70) moves along it with dLib_pathMove */
    /* 0x63C */ dPa_trackEcallBack mWakeCb;  /* was m4AC: CreateWave 02384570: the dPa_trackEcallBack of the PIRATESHIP_TAIL wake emitter; SetWave 02384C2C sets its water height/speed; DeleteWave 02384A48 removes it */
    /* 0x68C */ gptr<PirateshipEmitter> mpSplashEmitter;  /* was m4FC: CreateWave 02384570: the PIRATESHIP_SPLASH emitter; SetWave 02384C2C sets its colour/translation/rotation; DeleteWave 02384A48 stops it */
    /* 0x690 */ GXColor mSplashColor;  /* was m500: CreateWave 02384570 / SetWave 02384C2C: dKy_get_seacolor, the splash emitter's prm colour */
    /* 0x694 */ dBgS_ObjAcch mAcch;  /* was m504: _create 023852E0 constructs the dBgS_ObjAcch; SetWave 02384C2C takes its water height (m_wtr) when in water */
    /* 0x858 */ cXyz mWakePos;  /* was m6C8: CreateWave 02384570: current.pos, the wake emitter's position; SetWave 02384C2C: ship matrix times HIO offset, its y is the wake water height */
};
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mpDoor, 0x494);
WWHD_OFFSET(Act_c, mWakeCb, 0x63C);
WWHD_OFFSET(Act_c, mWakePos, 0x858);
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
void pirateshipBgAcchDestruct(void*, s32); /* 024EFD9C dBgS_Acch::~dBgS_Acch (was pirateshipLineDestruct (misnamed)) */
void pirateshipAcchConstruct(void*);
void dBgS_MoveBgActor__ct(daObjPirateship::Act_c*);
s32 pirateshipMoveBGCreate(daObjPirateship::Act_c*, const char*, s32, u32, u32);
s32 pirateshipMoveBGExecute(daObjPirateship::Act_c*);
s32 pirateshipMoveBGDelete(daObjPirateship::Act_c*);
void pirateshipStatusDestruct(void*, s32);
void pirateshipCylDestruct(void*, s32); /* 02515A70 dCcD_Cyl::~dCcD_Cyl (was pirateshipAcchDestruct (misnamed)) */
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
WWHD_OFFSET(daObjPirateship::Act_c, mStts, 0x4C8);
WWHD_OFFSET(daObjPirateship::Act_c, mCyl, 0x504);
WWHD_OFFSET(daObjPirateship::Act_c, mPathPntNo, 0x634);
WWHD_OFFSET(daObjPirateship::Act_c, mpPath, 0x638);
WWHD_OFFSET(daObjPirateship::Act_c, mpSplashEmitter, 0x68C);
WWHD_OFFSET(daObjPirateship::Act_c, mSplashColor, 0x690);
WWHD_OFFSET(daObjPirateship::Act_c, mAcch, 0x694);
