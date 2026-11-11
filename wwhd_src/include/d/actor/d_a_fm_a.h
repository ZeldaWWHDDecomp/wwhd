/* Floormaster part A: private HD views and exact call adapters. */
#pragma once
#include "d/actor/d_a_fm.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
namespace fm_a {
inline J3DModel* actorModel(fopAc_ac_c* actor) { return gabi::at<J3DModel>(actor->model); }
struct HIO {
    u8 mUnaccessed0[0x4];
    be<u8> mDebugTargets;  /* was field_0x004: as fm_main::HIO::mDebugTargets */
    be<u8> mDebugHeadTop;  /* was field_0x005: as fm_main::HIO::mDebugHeadTop */
    be<u8> mDebugGrabAxis;  /* was field_0x006: as fm_main::HIO::mDebugGrabAxis */
    be<u8> mDebugHandAxis;  /* was field_0x007: as fm_main::HIO::mDebugHandAxis */
    u8 mUnaccessed8[0x1];
    be<u8> mDebugAngles;  /* was field_0x009: as fm_main::HIO::mDebugAngles */
    be<u8> mDebugGoalLine;  /* was field_0x00A: as fm_main::HIO::mDebugGoalLine */
    be<u8> mDebugJoints;  /* was field_0x00B: as fm_main::HIO::mDebugJoints */
    be<u8> mDebugArea;  /* was field_0x00C: as fm_main::HIO::mDebugArea */
    u8 mUnaccessedD[0x2B];
    cXyz mHandOffset;  /* was field_0x038: as fm_main::HIO::mHandOffset */
    u8 mUnaccessed44[0x22];
    be<s16> mUnderFootTime;  /* was field_0x066: as fm_main::HIO::mUnderFootTime */
    u8 mUnaccessed68[0x1C];
    be<s16> mHideTimeMin;  /* was field_0x084: as fm_main::HIO::mHideTimeMin */
    be<s16> mHideTimeMax;  /* was field_0x086: as fm_main::HIO::mHideTimeMax */
    u8 mUnaccessed88[0x2];
    be<s16> field_0x08A;
    u8 mUnaccessed8C[0x14];
    be<s16> mRndMoveTime;  /* was field_0x0A0: as fm_main::HIO::mRndMoveTime */
    u8 mUnaccessedA2[0x6];
    be<f32> mHeightRange;  /* was field_0x0A8: as fm_main::HIO::mHeightRange */
    be<f32> mRndBackDist;  /* was field_0x0AC: as fm_main::HIO::mRndBackDist */
    be<f32> mRndMoveStep;  /* was field_0x0B0: as fm_main::HIO::mRndMoveStep */
    u8 mUnaccessedB4[0xC];
    be<f32> mUnderFootStep;  /* was field_0x0C0: as fm_main::HIO::mUnderFootStep */
    u8 mUnaccessedC4[0x4];
    be<f32> mSphRadius;  /* was field_0x0C8: as fm_main::HIO::mSphRadius */
    be<f32> mChaseSpeed;  /* was field_0x0CC: as fm_main::HIO::mChaseSpeed */
    be<f32> mPathSpeed;  /* was field_0x0D0: as fm_main::HIO::mPathSpeed */
    u8 mUnaccessedD4[0x8];
    be<f32> mFootGrabDist;  /* was field_0x0DC: as fm_main::HIO::mFootGrabDist */
    be<f32> mNearRange;  /* was field_0x0E0: as fm_main::HIO::mNearRange */
    be<f32> mAttackRange;  /* was field_0x0E4: as fm_main::HIO::mAttackRange */
    be<f32> mNoticeRange;  /* was field_0x0E8: as fm_main::HIO::mNoticeRange */
    u8 mUnaccessedEC[0x34];
    be<f32> mHoleScaleClosed;  /* was field_0x120: as fm_main::HIO::mHoleScaleClosed */
    be<f32> mHoleScaleOpen;  /* was field_0x124: as fm_main::HIO::mHoleScaleOpen */
    be<f32> mHoleScaleStep;  /* was field_0x128: as fm_main::HIO::mHoleScaleStep */
    be<f32> mHoleScaleStepUnderFoot;  /* was field_0x12C: as fm_main::HIO::mHoleScaleStepUnderFoot */
    u8 mUnaccessed130[0x15C - 0x130];
};
WWHD_OFFSET(HIO, mDebugTargets, 0x4);
WWHD_OFFSET(HIO, mDebugHeadTop, 0x5);
WWHD_OFFSET(HIO, mDebugGrabAxis, 0x6);
WWHD_OFFSET(HIO, mDebugHandAxis, 0x7);
WWHD_OFFSET(HIO, mDebugAngles, 0x9);
WWHD_OFFSET(HIO, mDebugGoalLine, 0xA);
WWHD_OFFSET(HIO, mDebugJoints, 0xB);
WWHD_OFFSET(HIO, mDebugArea, 0xC);
WWHD_OFFSET(HIO, mHandOffset, 0x38);
WWHD_OFFSET(HIO, mUnderFootTime, 0x66);
WWHD_OFFSET(HIO, mHideTimeMin, 0x84);
WWHD_OFFSET(HIO, mHideTimeMax, 0x86);
WWHD_OFFSET(HIO, field_0x08A, 0x8A);
WWHD_OFFSET(HIO, mRndMoveTime, 0xA0);
WWHD_OFFSET(HIO, mHeightRange, 0xA8);
WWHD_OFFSET(HIO, mRndBackDist, 0xAC);
WWHD_OFFSET(HIO, mRndMoveStep, 0xB0);
WWHD_OFFSET(HIO, mUnderFootStep, 0xC0);
WWHD_OFFSET(HIO, mSphRadius, 0xC8);
WWHD_OFFSET(HIO, mChaseSpeed, 0xCC);
WWHD_OFFSET(HIO, mPathSpeed, 0xD0);
WWHD_OFFSET(HIO, mFootGrabDist, 0xDC);
WWHD_OFFSET(HIO, mNearRange, 0xE0);
WWHD_OFFSET(HIO, mAttackRange, 0xE4);
WWHD_OFFSET(HIO, mNoticeRange, 0xE8);
WWHD_OFFSET(HIO, mHoleScaleClosed, 0x120);
WWHD_OFFSET(HIO, mHoleScaleOpen, 0x124);
WWHD_OFFSET(HIO, mHoleScaleStep, 0x128);
WWHD_OFFSET(HIO, mHoleScaleStepUnderFoot, 0x12C);
WWHD_SIZE(HIO, 0x15C);
inline HIO& hio() { return *gabi::at<HIO>(0x10463E0C); }
inline be<u32>& counter() { return *gabi::at<be<u32>>(0x101FF558); }
inline u32 zeroQuatAddress() { return 0x101E9C38; }
struct PlayerVtable {
    u8 mUnaccessed[0x4C]; be<u32> checkPlayerFly;
    u8 mUnaccessed50[0xC]; be<u32> checkFrontRoll;
    u8 mUnaccessed60[0xBC - 0x60]; be<u32> getGrabActorID;
    u8 mUnaccessedC0[0x124 - 0xC0]; be<u32> setPlayerPosAndAngle;
};
WWHD_OFFSET(PlayerVtable, checkPlayerFly, 0x4C);
WWHD_OFFSET(PlayerVtable, checkFrontRoll, 0x5C);
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(PlayerVtable, setPlayerPosAndAngle, 0x124);
inline PlayerVtable* vtable(fopAc_ac_c* player) { return gabi::at<PlayerVtable>(player->__vtbl); }
struct Player : fopAc_ac_c { u8 mUnaccessed[0x3D8 - 0x3AC]; cXyz mHeadTopPos; };
WWHD_OFFSET(Player, mHeadTopPos, 0x3D8);
struct ModelMatrixBlock {
    u8 mUnaccessed[4]; be<u16> mFlags;
    u8 mUnaccessed6[0xA]; gptr<Mtx34> mAnmMtx;
};
WWHD_OFFSET(ModelMatrixBlock, mFlags, 4);
WWHD_OFFSET(ModelMatrixBlock, mAnmMtx, 0x10);
struct Model { u8 mUnaccessed[0x2C]; gptr<ModelMatrixBlock> mMatrixBlock; };
WWHD_OFFSET(Model, mMatrixBlock, 0x2C);
struct ModelData { u8 mUnaccessed[0x44]; be<u32> mTextureSRTAnimation; };
WWHD_OFFSET(ModelData, mTextureSRTAnimation, 0x44);
struct VectorWords { be<u32> x, y, z; };
WWHD_SIZE(VectorWords, 12);
struct MatrixWords { be<u32> m[3][4]; };
WWHD_SIZE(MatrixWords, 0x30);
inline Mtx34* anmMtx(J3DModel* model, u32 joint) {
    auto* block = reinterpret_cast<Model*>(model)->mMatrixBlock.get();
    Mtx34* matrices = block->mAnmMtx;
    block->mFlags |= 0x10;
    return matrices + joint;
}
// GC names/order from mode_tbl. EXEC's unused mode argument retains value21.
enum Mode { MODE_HIDE=1, MODE_UNDERFOOT=2, MODE_PATH_MOVE=3, MODE_GOAL_KEEPER=4,
            MODE_APPEAR=5, MODE_DISAPPEAR=6, MODE_WAIT=7, MODE_ATTACK=8,
            MODE_GRAB=13, MODE_GRAB_FOOT_DEMO=15, MODE_UNUSED_EXEC=21 };
}
inline s32 daPy_checkPlayerFly(fopAc_ac_c* player) { return gabi::call_ptr<s32>(fm_a::vtable(player)->checkPlayerFly, player); }
inline s32 daPy_checkFrontRoll(fopAc_ac_c* player) { return gabi::call_ptr<s32>(fm_a::vtable(player)->checkFrontRoll, player); }
inline s32 daPy_getGrabActorID(fopAc_ac_c* player) { return gabi::call_ptr<s32>(fm_a::vtable(player)->getGrabActorID, player); }
inline void daPy_setPlayerPosAndAngle_mtx(fopAc_ac_c* player, Mtx34* mtx) { gabi::call_ptr(fm_a::vtable(player)->setPlayerPosAndAngle, player, mtx); }
constexpr u32 pathMove_CB = 0x02140CA8;
enum { DSNAP_TYPE_FM=0xB5, FM_JNT_TE_e=5, fpcNm_NPC_CB1_e=0x14E, fpcNm_NPC_MD_e=0x16F };
void fm_setAnmFull(daFm_c* a, s32 idx, u32 force);
bool fm_setHoleScale(daFm_c* a, f32 x, f32 y, f32 z);
void fm_modeProc(daFm_c* a, s32 proc, s32 mode);
void fm_holeExecute(daFm_c* a);
void fm_iceProc(daFm_c* a);
void fm_setBaseTarget(daFm_c* a);
void fm_bodySetMtx(daFm_c* a);
void fm_holeSetMtx(daFm_c* a);
u8 fm_areaCheck(daFm_c* a);
u32 fm_lineCheck(daFm_c* a, cXyz* p1, cXyz* p2);
u32 fm_isLink(daFm_c* a, fopAc_ac_c* t);
void fm_setAttention(daFm_c* a);
void fm_spAttackVJump(daFm_c* a);
void fm_spAttackJump(daFm_c* a);
u32 daBomb_chk_state(fopAc_ac_c* b, u32 st);
void morf_play(mDoExt_McaMorf* m, cXyz* pos, u32 a, s32 b);
void C_QUATSlerp(Quaternion_fm* p, u32 q, Quaternion_fm* r, f32 t);
void morf_calc(mDoExt_McaMorf* m);
u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh);
void cM3dGSph_SetC_xyz(cM3dGSph* s, f32 x, f32 y, f32 z);
void dLib_pathMove(cXyz* pos, be<s8>* pnt, dPath* path, f32 spd, u32 cb, void* data);
void dSnap_RegistFig_pos(u32 type, fopAc_ac_c* actor, cXyz* pos, s16 angle, f32 x, f32 y, f32 z);
s32 fm_calcTimer(be<s32>* timer);
void fm_addCalcPosXZ2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep);
void mDoMtx_inverseTranspose(Mtx34* src, Mtx34* dst);
void mDoMtx_MtxToRot(Mtx34* src, csXyz* dst);
void dMat_control_iceEntryDL(mDoExt_McaMorf* morf, s32 param, void* invisible);
bool fopAcM_SearchByID_out(u32 id, gptr<fopAc_ac_c>* out);
BOOL enemy_ice(enemyice_l* ice);
void dLib_debugDrawAxis(Mtx34* matrix, f32 scale);
BOOL fm_startCheckOld(void* manager, const char* name);
BOOL fm_endCheckOld(void* manager, const char* name);
void fopAcM_orderOtherEvent2(fopAc_ac_c* actor, const char* name, u16 flag, u16 hind);
void fm_copyColor(GXColor* dst, const GXColor* src, u32 size);
inline void fm_setAnm(daFm_c* actor, s8 index, bool force) { fm_setAnmFull(actor, s32(index), u32(force)); }

using FmColorGuard = be<u32>;
