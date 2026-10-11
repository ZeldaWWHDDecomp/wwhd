/* Great Sea enemy cannon.
 * HD: actor fields move by 0x11C; the collision sphere/status sizes become 0x12C/0x3C.
 * Unknown member names follow zeldaret/tww exactly. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"

struct daObj_Canon_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e { MODE_WAIT, MODE_ATTACK, MODE_DELETE, MODE_SW_WAIT, MODE_APPEAR, MODE_COUNT };
    void _nodeControl(J3DNode* node, J3DModel* model);
    s32 _createHeap();
    void getArg();
    void modeProc(s32 proc, s32 mode);
    void setMtx();
    void createInit();
    s32 _create();
    s32 _delete();
    void setAttention();
    void setCollision();
    s32 _execute();
    void debugDraw();
    s32 _draw();
    void modeWaitInit();
    s32 checkTgHit();
    void modeWait();
    void modeAttackInit();
    void lockonCannon();
    void attackCannon();
    void modeAttack();
    void setEffect(u16 id);
    void modeDeleteInit();
    void modeSwWaitInit();
    void modeSwWait();
    void modeAppearInit();
    void modeAppear();
    void modeDelete();
    /* 0x3AC */ be<s32> mCurMode;
    /* 0x3B0 */ be<u8> field_0x294;
    /* 0x3B1 */ be<u8> field_0x295;
    /* 0x3B2 */ be<u8> field_0x296;
    /* 0x3B3 */ be<u8> field_0x297;
    /* 0x3B4 */ dNpc_PathRun_c field_0x298;
    /* 0x3BC */ u8 field_0x2A0[0x10];
    /* 0x3CC */ gptr<dPath> field_0x2B0;
    /* 0x3D0 */ u8 field_0x2B4[4];
    /* 0x3D4 */ request_of_phase_process_class mPhs;
    /* 0x3DC */ gptr<J3DModel> mpModel;
    /* 0x3E0 */ be<s16> field_0x2C4;
    /* 0x3E2 */ be<s16> field_0x2C6;
    /* 0x3E4 */ be<s16> field_0x2C8;
    /* 0x3E6 */ be<s16> field_0x2CA;
    /* 0x3E8 */ cXyz field_0x2CC;
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ dCcD_Stts mStts;
    /* 0x55C */ be<s32> field_0x440;
    /* 0x560 */ be<s32> field_0x444;
    /* 0x564 */ u8 field_0x448[8];
    /* 0x56C */ cXyz field_0x450;
    /* 0x578 */ cXyz field_0x45C;
    /* 0x584 */ be<s16> field_0x468;
    /* 0x586 */ be<s16> field_0x46A;
    /* 0x588 */ be<f32> field_0x46C;
    /* 0x58C */ dPa_followEcallBack field_0x470;
    /* 0x5A0 */ be<s32> field_0x484;
    /* 0x5A4 */ be<s32> field_0x488;
};
WWHD_OFFSET(daObj_Canon_c, mCurMode, 0x3AC);
WWHD_OFFSET(daObj_Canon_c, field_0x294, 0x3B0);
WWHD_OFFSET(daObj_Canon_c, field_0x298, 0x3B4);
WWHD_OFFSET(daObj_Canon_c, field_0x2B0, 0x3CC);
WWHD_OFFSET(daObj_Canon_c, mPhs, 0x3D4);
WWHD_OFFSET(daObj_Canon_c, mpModel, 0x3DC);
WWHD_OFFSET(daObj_Canon_c, field_0x2C4, 0x3E0);
WWHD_OFFSET(daObj_Canon_c, field_0x2CC, 0x3E8);
WWHD_OFFSET(daObj_Canon_c, mSph, 0x3F4);
WWHD_OFFSET(daObj_Canon_c, mStts, 0x520);
WWHD_OFFSET(daObj_Canon_c, field_0x440, 0x55C);
WWHD_OFFSET(daObj_Canon_c, field_0x450, 0x56C);
WWHD_OFFSET(daObj_Canon_c, field_0x45C, 0x578);
WWHD_OFFSET(daObj_Canon_c, field_0x46A, 0x586);
WWHD_OFFSET(daObj_Canon_c, field_0x46C, 0x588);
WWHD_OFFSET(daObj_Canon_c, field_0x470, 0x58C);
WWHD_OFFSET(daObj_Canon_c, field_0x488, 0x5A4);
WWHD_SIZE(daObj_Canon_c, 0x5A8);

struct daObj_Canon_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> debug_draw;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ u8 padding06[2];
    /* 0x08 */ be<f32> field_0x08;
    /* 0x0C */ be<f32> field_0x0C;
    /* 0x10 */ be<f32> field_0x10;
    /* 0x14 */ be<f32> field_0x14;
    /* 0x18 */ be<f32> field_0x18;
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<s16> field_0x28;
    /* 0x2A */ u8 padding2A[2];
    /* 0x2C */ be<f32> field_0x2C;
    /* 0x30 */ be<s16> field_0x30;
    /* 0x32 */ be<s16> field_0x32;
    /* 0x34 */ be<s16> field_0x34;
    /* 0x36 */ be<s16> field_0x36;
    /* 0x38 */ be<s16> field_0x38;
    /* 0x3A */ be<s16> field_0x3A;
    /* 0x3C */ be<s16> field_0x3C;
    /* 0x3E */ be<s16> field_0x3E;
    /* 0x40 */ be<f32> field_0x40;
    /* 0x44 */ be<f32> field_0x44;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ u8 padding4A[2];
};
WWHD_SIZE(daObj_Canon_HIO_c, 0x4C);
WWHD_OFFSET(daObj_Canon_HIO_c, field_0x28, 0x28);
WWHD_OFFSET(daObj_Canon_HIO_c, field_0x3E, 0x3E);

namespace canonHD {
enum {
    dRes_INDEX_WALLBOM_BDL_WALLBOM_e = 3,
    WALLBOM_JNT_SHOT_e = 3,
    AT_TYPE_BOMB = 0x20,
    AT_TYPE_BOOMERANG = 0x40,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_LK_SHIP_CANNON_FIRE = 0x2852,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    ID_IT_SN_MJTAIHOU_HAHEN00 = 0x82E4,
    ID_IT_SN_MJTAIHOU_SMOKE00 = 0x82E5,
    ID_IT_SN_MJTAIHOU_SENKO00 = 0x82E6,
    ID_IT_JN_MJTAIHOU_SMOKE01 = 0x3E1,
    daPyStts0_UNK1000000_e = 0x1000000,
    daPyStts0_SWIM_e = 0x100000,
    SHIP_SHOOT_CANNON = 0x20000,
    MODEL_ANM_MTX_DIRTY = 0x10,
    MAP_ICON_MASK = 0x1F,
    MAP_ICON_CANNON = 0x15,
};

struct JointData {
    /* 0x00 */ u8 padding[4];
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_OFFSET(JointData, mJntNo, 4);
struct Joint {
    /* 0x00 */ u8 padding[8];
    /* 0x08 */ be<u32> mCallBack;
    /* 0x0C */ u8 padding2[4];
    /* 0x10 */ gptr<J3DMaterial> mMesh;
    /* 0x14 */ u8 padding3[8];
};
WWHD_OFFSET(Joint, mCallBack, 8);
WWHD_OFFSET(Joint, mMesh, 0x10);
WWHD_SIZE(Joint, 0x1C);
struct ModelData {
    /* 0x00 */ be<u32> mpResHeader;
    /* 0x04 */ be<u32> mJointNum;
    /* 0x08 */ gptr<Joint> mJointNodes;
};
WWHD_SIZE(ModelData, 0xC);
struct MatrixBuffer {
    /* 0x00 */ u8 padding[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 padding2[0xA];
    /* 0x10 */ gptr<Mtx34> mpAnmMtx;
};
WWHD_OFFSET(MatrixBuffer, mpAnmMtx, 0x10);
WWHD_SIZE(MatrixBuffer, 0x14);
struct Ship {
    /* 0x000 */ u8 padding[0x644];
    /* 0x644 */ be<u32> mState;
};
WWHD_OFFSET(Ship, mState, 0x644);
struct SaveBlock {
    /* 0x00 */ u8 padding[0x20];
    /* 0x20 */ dSv_info_c mSaveInfo;
};
WWHD_OFFSET(SaveBlock, mSaveInfo, 0x20);
struct HeaderZeros {
    be<u32> words[4];
};
struct DestructorRecord {
    u8 data[0xC];
};
struct DebugObject {
    u8 data[4];
};
} // namespace canonHD

using CanonModeProc = void (daObj_Canon_c::*)();
struct CanonModeEntry {
    /* 0x00 */ gabi::ptmf<CanonModeProc> init;
    /* 0x08 */ gabi::ptmf<CanonModeProc> run;
    /* 0x10 */ gptr<const char> name;
};
WWHD_OFFSET(CanonModeEntry, run, 8);
WWHD_OFFSET(CanonModeEntry, name, 0x10);
WWHD_SIZE(CanonModeEntry, 0x14);

WWHD_SIZE(canonHD::JointData, 6);
WWHD_SIZE(canonHD::Ship, 0x648);
WWHD_SIZE(canonHD::SaveBlock, sizeof(dSv_info_c) + 0x20);
WWHD_SIZE(canonHD::HeaderZeros, 0x10);
WWHD_SIZE(canonHD::DestructorRecord, 0xC);
WWHD_SIZE(canonHD::DebugObject, 4);
