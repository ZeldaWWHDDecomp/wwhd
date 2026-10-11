/** Fan actor: GameCube names with HD offsets.
 * HD: the moving-background base ends at 0x3E0; animation wrappers embed frame/calculator storage. */
#pragma once
#include "gc_api/gc_api.h"
// HD: private capsule views avoid mixing the geometry-unit and caller collision headers.
struct FanCapsuleShape {
    cXyz mStart;
    cXyz mEnd;
    be<f32> mRadius;
};
struct FanCapsuleGeometry {
    cXyz mStart;
    cXyz mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(FanCapsuleGeometry, __vtbl, 0x18);
WWHD_OFFSET(FanCapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(FanCapsuleGeometry, 0x20);
WWHD_SIZE(FanCapsuleShape, 0x1C);
struct daFan_c : dBgS_MoveBgActor {
    cPhs_State _create();
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    void set_mtx();
    void set_wind_length(f32);
    void set_cps(f32);
    BOOL Execute(Mtx34 **mtxP);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs; /* GameCube 0x2C8 */
    /* 0x3E8 */ request_of_phase_process_class mWindPhs;
    /* 0x3F0 */ gptr<J3DModel> mModel;
    /* 0x3F4 */ dCcD_Stts mStts;
    /* 0x430 */ dCcD_Cps mCps; /* FanCapsuleGeometry at 0x548 */
    /* 0x568 */ u8 field_0x450[0x34];
    /* 0x59C */ mDoExt_btkAnm mBtkAnm;
    /* 0x610 */ be<u32> mSwitchNo;
    /* 0x614 */ be<u8> mType;
    /* 0x615 */ u8 _615[3];
    /* 0x618 */ FanCapsuleShape mCpsS;
    /* 0x634 */ be<s16> mFanAngle;
    /* 0x636 */ be<s16> mFanSpeed;
    /* 0x638 */ be<u32> field_0x4c0;
    /* 0x63C */ gptr<J3DModel> mWindModel;
    /* 0x640 */ mDoExt_btkAnm mWindBtkAnm0;
    /* 0x6B4 */ mDoExt_btkAnm mWindBtkAnm1;
    /* 0x728 */ mDoExt_bckAnm mWindBckAnm;
    /* 0x7B4 */ cXyz mWindScale;
    /* 0x7C0 */ be<u32> mWindSePId;

    FanCapsuleGeometry *cps_shape() { return reinterpret_cast<FanCapsuleGeometry *>(mCps.mCps); }
};
WWHD_OFFSET(daFan_c, mModel, 0x3F0);
WWHD_OFFSET(daFan_c, mStts, 0x3F4);
WWHD_OFFSET(daFan_c, mCps, 0x430);
WWHD_OFFSET(daFan_c, mBtkAnm, 0x59C);
WWHD_OFFSET(daFan_c, mType, 0x614);
WWHD_OFFSET(daFan_c, mCpsS, 0x618);
WWHD_OFFSET(daFan_c, mWindModel, 0x63C);
WWHD_OFFSET(daFan_c, mWindBckAnm, 0x728);
WWHD_OFFSET(daFan_c, mWindSePId, 0x7C0);
WWHD_SIZE(daFan_c, 0x7C4);

namespace daFan_prm {
/* HD: the type is clamped to 0 when it is 3 */
inline u8 getType(daFan_c *ac) {
    u32 t = (fopAcM_GetParam(ac) >> 8) & 0x03;
    if (t >= 3)
        t = 0;
    return t;
}
inline u8 getSwitchNo(daFan_c *ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
}; // namespace daFan_prm

// HD: native table indexing uses the full u8 type, including reads beyond the three normal entries.
struct FanArcTable {
    gptr<const char> entries[256];
};
struct FanShortTable {
    be<s16> entries[256];
};
struct FanFloatTable {
    be<f32> entries[256];
};
struct FanWordTable {
    be<u32> entries[256];
};
struct FanVectorTable {
    cXyz entries[256];
};
struct FanLevelSe {
    u8 prefix[0xFC];
    be<u32> field_0xf8;
    be<s8> mReverb;
    be<u8> mFlag;
};
WWHD_OFFSET(FanLevelSe, field_0xf8, 0xFC);
WWHD_OFFSET(FanLevelSe, mReverb, 0x100);
WWHD_OFFSET(FanLevelSe, mFlag, 0x101);
// HD: GHS expands these embedded constructors with translation-unit vtables.
struct FanCpsStorage : dCcD_GObjInf {
    u8 mAabb[0x18];
    be<u32> __vtblAabb;
    be<u32> __vtblShape;
    FanCapsuleGeometry mCps;
};
WWHD_OFFSET(FanCpsStorage, __vtblAabb, 0x110);
WWHD_OFFSET(FanCpsStorage, __vtblShape, 0x114);
WWHD_OFFSET(FanCpsStorage, mCps, 0x118);
WWHD_SIZE(FanCpsStorage, 0x138);
struct FanBckStorage {
    J3DFrameCtrl mFrameCtrl;
    be<u32> __vtbl;
    u8 mTransform[0x34];
    be<u32> __vtblMtxCalc;
    u8 padding_4C[0xC];
    be<u32> mpAnmTransform;
    u8 padding_5C[0x20];
    be<u32> mpTransformInfo;
    be<u32> mpQuat;
    be<u32> mpCallback1;
    be<u32> mpCallback2;
};
WWHD_OFFSET(FanBckStorage, __vtbl, 0x10);
WWHD_OFFSET(FanBckStorage, mTransform, 0x14);
WWHD_OFFSET(FanBckStorage, __vtblMtxCalc, 0x48);
WWHD_OFFSET(FanBckStorage, mpAnmTransform, 0x58);
WWHD_OFFSET(FanBckStorage, mpTransformInfo, 0x7C);
WWHD_OFFSET(FanBckStorage, mpQuat, 0x80);
WWHD_OFFSET(FanBckStorage, mpCallback1, 0x84);
WWHD_OFFSET(FanBckStorage, mpCallback2, 0x88);
WWHD_SIZE(FanBckStorage, 0x8C);
enum {
    dRes_INDEX_YAFLW00_BCK_YAFLW00_e = 5,
    dRes_INDEX_YAFLW00_BDL_YAFLW00_e = 8,
    dRes_INDEX_YAFLW00_BTK_YAFLW00_01_e = 0xB,
    dRes_INDEX_YAFLW00_BTK_YAFLW00_02_e = 0xC,
    JA_SE_OBJ_WIND_TAG = 0x701D,
    fpcNm_LEVEL_SE_e = 0x18
};
enum : u32 {
    FAN_ACTOR_VTABLE = 0x1000EA70,
    FAN_AABB_VTABLE = 0x1000E91C,
    FAN_SHAPE_BASE_VTABLE = 0x100015A8,
    FAN_CPS_OBJ_VTABLE = 0x1004AF18,
    FAN_CPS_SHAPE_VTABLE = 0x1004AF70,
    FAN_CPS_GEOMETRY_VTABLE = 0x1004AF60,
    FAN_TRANSFORM_BASE_VTABLE = 0x1016E54C,
    FAN_TRANSFORM_VTABLE = 0x1000E92C,
    FAN_MTX_CALC_VTABLE = 0x1016D820,
    FAN_MOVE_BG_ROT_Y = 0x024EE708
};
FanLevelSe *fopKyM_SearchByID(u32);
BOOL fopKyM_Delete(void *);
u32 fopKyM_create(s16, s32, cXyz *, cXyz *, u32);
bool cLib_chaseS(be<s16> *, s16, s16);
void Fan_setCps(dCcD_Cps *, const dCcD_SrcCps *);
void Fan_setCpsShape(FanCapsuleGeometry *, const FanCapsuleShape *);
void Fan_subtract(const cXyz *, const cXyz *, cXyz *);
void Fan_entryBtkModel(mDoExt_btkAnm *, J3DModel *, f32);
void Fan_collisionCtor(dCcD_Cps *);
void Fan_capsuleCtor(FanCapsuleGeometry *);
void Fan_transformCtor(void *);
void Fan_transformDtor(void *, s32);
void Fan_collisionDtor(dCcD_Cps *, s32);
void Fan_actorDtor(fopAc_ac_c *, s32);

// HD: fan's callback reads the joint number directly; the shared getter is out of line.
struct FanJointView {
    u8 prefix[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(FanJointView, mJntNo, 4);
