/* Beamos: verified WWHD layout with zeldaret/tww member names.
 * HD: actor base grows to 0x3AC; GHS eye procedure occupies eight bytes; virtual dispatch reads
 * the vtable offset from the low half of the target word, as in the shared ptmf adapter. brk animations grow to 0x78; HIO vtable moves to the tail. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_beam.h"
#include "d/d_path.h"
struct daBemos_c : fopAc_ac_c {
    s32 CreateHeap1();
    s32 CreateHeap2();
    s32 CreateHeap3();
    s32 CreateHeap();
    s32 blue_eye_wait_init();
    s32 red_eye_wait_init();
    s32 yellow_range_check(s32 range, void* angle);
    s32 blue_eye_search_init();
    s32 red_eye_search_init();
    void* getBeamActor();
    s32 blue_eye_charge_init();
    s32 red_eye_charge_init();
    s32 eye_dummy();
    void set_mtx();
    s32 CreateInit1();
    s32 CreateInit2();
    s32 CreateInit3();
    s32 CreateInit();
    s32 _delete();
    s32 _create();
    s32 _execute();
    s32 _draw();
    void event_move();
    s32 yellow_eye_wait_init();
    s32 blue_eye_wait();
    s32 yellow_eye_search_init();
    s32 red_eye_wait();
    s32 eye_break_init();
    void guard_proc();
    s32 blue_range_check(s32 active, csXyz* angle);
    s32 red_range_check(s32 active, csXyz* angle);
    s32 yellow_eye_wait();
    s32 yellow_eye_search();
    s32 blue_eye_search();
    s32 red_eye_search();
    s32 blue_eye_charge();
    s32 red_eye_charge();
    s32 eye_break();

    /* 0x3AC */ gabi::ptmf<void (daBemos_c::*)()> m290;
    /* 0x3B4 */ csXyz m29C;
    u8 _pad3BA[0x2];
    /* 0x3BC */ cXyz m2A4;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<J3DModel> m2B8;
    /* 0x3D4 */ gptr<J3DModel> m2BC;
    /* 0x3D8 */ gptr<J3DModel> m2C0;
    u8 _pad3DC[0x4];
    /* 0x3E0 */ mDoExt_brkAnm mBrkAnm1;
    /* 0x458 */ gptr<J3DAnmTevRegKey> m_sch_brk;
    /* 0x45C */ mDoExt_brkAnm mBrkAnm2;
    /* 0x4D4 */ gptr<JPABaseEmitter> m2FC;
    /* 0x4D8 */ gptr<JPABaseEmitter> m300;
    /* 0x4DC */ gptr<JPABaseEmitter> m304;
    /* 0x4E0 */ gptr<JPABaseEmitter> m308;
    /* 0x4E4 */ gptr<JPABaseEmitter> m30C;
    /* 0x4E8 */ be<u8> m310;
    u8 _pad4E9[0x3];
    /* 0x4EC */ cXyz m314;
    /* 0x4F8 */ be<f32> m320;
    u8 _pad4FC[0x30];
    /* 0x52C */ cXyz m354;
    /* 0x538 */ cXyz m360;
    /* 0x544 */ cXyz m36C;
    /* 0x550 */ dCcD_Stts mSttsCyl;
    /* 0x58C */ dCcD_Cyl mCyl;
    /* 0x6BC */ dCcD_Stts mSttsSph;
    /* 0x6F8 */ dCcD_Sph mSph;
    /* 0x824 */ dBgS_ObjGndChk mGndChk;
    /* 0x878 */ be<f32> m6A0;
    /* 0x87C */ be<s32> m6A4;
    /* 0x880 */ be<s32> m6A8;
    /* 0x884 */ be<s16> m6AC;
    /* 0x886 */ be<s16> m6AE;
    /* 0x888 */ be<s16> m6B0;
    /* 0x88A */ be<s16> m6B2;
    u8 _pad88C[0x2];
    /* 0x88E */ be<s16> m6B6;
    /* 0x890 */ be<u8> m6B8;
    /* 0x891 */ be<u8> m6B9;
    /* 0x892 */ be<u8> m6BA;
    /* 0x893 */ be<u8> m6BB;
    /* 0x894 */ be<u8> m6BC;
    /* 0x895 */ be<u8> m6BD;
    /* 0x896 */ be<u8> m6BE;
    /* 0x897 */ be<u8> m6BF;
    /* 0x898 */ be<u8> m6C0;
    u8 _tail[3];
};
WWHD_OFFSET(daBemos_c, m290, 0x3AC);
WWHD_OFFSET(daBemos_c, m29C, 0x3B4);
WWHD_OFFSET(daBemos_c, m2A4, 0x3BC);
WWHD_OFFSET(daBemos_c, mPhase, 0x3C8);
WWHD_OFFSET(daBemos_c, m2B8, 0x3D0);
WWHD_OFFSET(daBemos_c, m2BC, 0x3D4);
WWHD_OFFSET(daBemos_c, m2C0, 0x3D8);
WWHD_OFFSET(daBemos_c, mBrkAnm1, 0x3E0);
WWHD_OFFSET(daBemos_c, m_sch_brk, 0x458);
WWHD_OFFSET(daBemos_c, mBrkAnm2, 0x45C);
WWHD_OFFSET(daBemos_c, m2FC, 0x4D4);
WWHD_OFFSET(daBemos_c, m300, 0x4D8);
WWHD_OFFSET(daBemos_c, m304, 0x4DC);
WWHD_OFFSET(daBemos_c, m308, 0x4E0);
WWHD_OFFSET(daBemos_c, m30C, 0x4E4);
WWHD_OFFSET(daBemos_c, m310, 0x4E8);
WWHD_OFFSET(daBemos_c, m314, 0x4EC);
WWHD_OFFSET(daBemos_c, m320, 0x4F8);
WWHD_OFFSET(daBemos_c, m354, 0x52C);
WWHD_OFFSET(daBemos_c, m360, 0x538);
WWHD_OFFSET(daBemos_c, m36C, 0x544);
WWHD_OFFSET(daBemos_c, mSttsCyl, 0x550);
WWHD_OFFSET(daBemos_c, mCyl, 0x58C);
WWHD_OFFSET(daBemos_c, mSttsSph, 0x6BC);
WWHD_OFFSET(daBemos_c, mSph, 0x6F8);
WWHD_OFFSET(daBemos_c, mGndChk, 0x824);
WWHD_OFFSET(daBemos_c, m6A0, 0x878);
WWHD_OFFSET(daBemos_c, m6A4, 0x87C);
WWHD_OFFSET(daBemos_c, m6A8, 0x880);
WWHD_OFFSET(daBemos_c, m6AC, 0x884);
WWHD_OFFSET(daBemos_c, m6AE, 0x886);
WWHD_OFFSET(daBemos_c, m6B0, 0x888);
WWHD_OFFSET(daBemos_c, m6B2, 0x88A);
WWHD_OFFSET(daBemos_c, m6B6, 0x88E);
WWHD_OFFSET(daBemos_c, m6B8, 0x890);
WWHD_OFFSET(daBemos_c, m6B9, 0x891);
WWHD_OFFSET(daBemos_c, m6BA, 0x892);
WWHD_OFFSET(daBemos_c, m6BB, 0x893);
WWHD_OFFSET(daBemos_c, m6BC, 0x894);
WWHD_OFFSET(daBemos_c, m6BD, 0x895);
WWHD_OFFSET(daBemos_c, m6BE, 0x896);
WWHD_OFFSET(daBemos_c, m6BF, 0x897);
WWHD_OFFSET(daBemos_c, m6C0, 0x898);
WWHD_SIZE(daBemos_c, 0x89C);
// HD: preserve integer word copies of floating-point fields without quieting NaNs.
template <class T> inline be<u32>& bemos_bits(be<T>& field) { return *gabi::at<be<u32>>(gabi::ea(&field)); }

struct daBemos_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 _pad1[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<s16> m1C;
    /* 0x1A */ be<s16> m1E;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<f32> m24;
    /* 0x24 */ be<s16> m28;
    /* 0x26 */ be<s16> m2A;
    /* 0x28 */ be<f32> m2C;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<f32> m3C;
    /* 0x3C */ be<s16> m40;
    /* 0x3E */ be<s16> m42;
    /* 0x40 */ be<f32> m44;
    /* 0x44 */ be<f32> m48;
    /* 0x48 */ be<u8> m4C;
    u8 _pad49[3];
    /* 0x4C */ be<u32> __vtbl;
};
WWHD_SIZE(daBemos_HIO_c, 0x50);
WWHD_OFFSET(daBemos_HIO_c, mNo, 0x0);
WWHD_OFFSET(daBemos_HIO_c, m08, 0x4);
WWHD_OFFSET(daBemos_HIO_c, m0C, 0x8);
WWHD_OFFSET(daBemos_HIO_c, m10, 0xC);
WWHD_OFFSET(daBemos_HIO_c, m14, 0x10);
WWHD_OFFSET(daBemos_HIO_c, m18, 0x14);
WWHD_OFFSET(daBemos_HIO_c, m1C, 0x18);
WWHD_OFFSET(daBemos_HIO_c, m1E, 0x1A);
WWHD_OFFSET(daBemos_HIO_c, m20, 0x1C);
WWHD_OFFSET(daBemos_HIO_c, m24, 0x20);
WWHD_OFFSET(daBemos_HIO_c, m28, 0x24);
WWHD_OFFSET(daBemos_HIO_c, m2A, 0x26);
WWHD_OFFSET(daBemos_HIO_c, m2C, 0x28);
WWHD_OFFSET(daBemos_HIO_c, m30, 0x2C);
WWHD_OFFSET(daBemos_HIO_c, m34, 0x30);
WWHD_OFFSET(daBemos_HIO_c, m38, 0x34);
WWHD_OFFSET(daBemos_HIO_c, m3C, 0x38);
WWHD_OFFSET(daBemos_HIO_c, m40, 0x3C);
WWHD_OFFSET(daBemos_HIO_c, m42, 0x3E);
WWHD_OFFSET(daBemos_HIO_c, m44, 0x40);
WWHD_OFFSET(daBemos_HIO_c, m48, 0x44);
WWHD_OFFSET(daBemos_HIO_c, m4C, 0x48);
WWHD_OFFSET(daBemos_HIO_c, __vtbl, 0x4C);
// HD: HIO is a guest singleton; it must stay shared with the original constructor's writes.
inline daBemos_HIO_c& bemos_hio() { return *gabi::at<daBemos_HIO_c>(0x104692C0); }

// HD: particle fields touched inline by GHS, including the coordinate-system selector.
struct BemosEmitterHD {
    u8 _pad0[0x5C];
    be<s32> mMaxFrame;
    u8 _pad60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    cXyz mGlobalScale, mGlobalTranslation, mGlobalPrmScale;
    GXColor mGlobalPrmColor, mGlobalEnvColor;
    u8 _pad24C[8];
    be<u32> mFlags;
    u8 _pad258[10];
    be<u8> mCoordinateSystem;
    u8 _pad263;
};
WWHD_OFFSET(BemosEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(BemosEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BemosEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(BemosEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BemosEmitterHD, mGlobalPrmScale, 0x238);
WWHD_OFFSET(BemosEmitterHD, mGlobalPrmColor, 0x244);
WWHD_OFFSET(BemosEmitterHD, mGlobalEnvColor, 0x248);
WWHD_OFFSET(BemosEmitterHD, mFlags, 0x254);
WWHD_OFFSET(BemosEmitterHD, mCoordinateSystem, 0x262);
WWHD_SIZE(BemosEmitterHD, 0x264);

// HD: out-of-line bindings preserve the verified call-site register widths.
u32 BemosHD_dRes_control_c_getRes(void*, void*, s32);
void BemosHD_JUTAssertion_showAssert(const char*, s32, const char*);
u32 BemosHD_mDoExt_J3DModel__create(void*, s32, s32);
u32 BemosHD_mDoExt_brkAnm_init(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
u32 BemosHD_dComIfGp_get();
u32 BemosHD_dPa_control_c_set(void*, s32, s32, void*, void*, s32, s32, s32, s32, void*, s32, s32);
void BemosHD_operator_delete(void*);
void BemosHD_cBgS_Chk_destruct_cBgS_Chk(void*, s32);
void BemosHD_dCcD_Sph_destruct(void*, s32);
void BemosHD_dCcD_Stts_destruct(void*, s32);
void BemosHD_dCcD_Cyl_destruct(void*, s32);
void BemosHD_fopAc_ac_c_destruct_fopAc_ac_c(void*, s32);
u32 BemosHD_fopAcIt_Judge(void*, void*);
u32 BemosHD_fopAc_IsActor(void*);
u32 BemosHD_operator_new(s32);
void BemosHD___register_global_object(void*);
void BemosHD_headerFirst_construct(void*);
void BemosHD_headerSecond_construct(void*);
void BemosHD_daBemos_HIO_c_daBemos_HIO_c(void*);
void BemosHD_PSMTXTrans(void*, f32, f32, f32);
void BemosHD_mDoMtx_ZXYrotM(void*, s32, s32, s32);
void BemosHD_PSMTXMultVec(void*, void*, void*);
void BemosHD_mDoMtx_YrotM(void*, s32);
void BemosHD_mDoMtx_stack_c_transM(f32, f32, f32);
void BemosHD_dCcD_Stts_Init(void*, s32, s32, void*);
void BemosHD_dCcD_Sph_Set(void*, void*);
void BemosHD_daBemos_c_blue_eye_wait_init(void*);
f32 BemosHD_cBgS_GroundCross(void*, void*);
u32 BemosHD_fopAcM_createChild(s32, u32, u32, void*, s32, void*, void*, s32, s32);
void BemosHD_daBemos_c_red_eye_wait_init(void*);
void BemosHD_dCcD_Cyl_Set(void*, void*);
void BemosHD_daBemos_c_CreateInit1(void*);
void BemosHD_daBemos_c_CreateInit2(void*);
void BemosHD_daBemos_c_CreateInit3(void*);
void BemosHD_daBemos_c_set_mtx(void*);
void BemosHD_dComIfG_resDelete(void*, const char*);
void BemosHD_memcpy(void*, void*, s32);
void BemosHD_fopAc_ac_c_fopAc_ac_c(void*);
void BemosHD_mDoExt_brkAnm_init(void*);
void BemosHD_cCcD_Stts_construct(void*);
void BemosHD_dCcD_GStts_dCcD_GStts(void*);
void BemosHD_dCcD_GObjInf_dCcD_GObjInf(void*);
void BemosHD_cM3dGCyl_construct(void*);
void BemosHD_dCcD_Sph_construct(void*);
void BemosHD_cBgS_GndChk_cBgS_GndChk(void*);
s32 BemosHD_dComIfG_resLoad(void*, const char*);
u32 BemosHD_fopAcM_entrySolidHeap(void*, void*, u32);
s32 BemosHD_daBemos_c_CreateInit(void*);
u32 BemosHD_dCcD_GObjInf_ChkTgHit(void*);
void BemosHD_daBemos_c_eye_break_init(void*);
void BemosHD_dCcD_GObjInf_ClrTgHit(void*);
void BemosHD_daBemos_c_event_move(void*);
void BemosHD_dCcD_GStts_Move(void*);
void BemosHD_daObj_HitEff_hibana(void*, void*);
void BemosHD_daObj_HitSeStart(void*, s32, void*, s32);
void BemosHD_cM3dGCyl_SetC(void*, void*);
void BemosHD_cCcS_Set(void*, void*);
void BemosHD_daBemos_c_guard_proc(void*);
void BemosHD_fopAcM_setGbaName(void*, s32, s32, s32);
u32 BemosHD_dKy_getEnvlight();
void BemosHD_dScnKy_env_light_c_settingTevStruct(void*, s32, void*, void*);
void BemosHD_dScnKy_env_light_setLightTevColorType(void*, void*, void*);
void BemosHD_mDoExt_modelUpdateDL(void*, s32);
void BemosHD_mDoExt_brkAnm_entry(void*, void*, f32);
u32 BemosHD_dSv_info_c_isSwitch(void*, u32, s32);
u32 BemosHD_dSv_event_c_isEventBit(void*, s32);
void BemosHD_dSv_event_c_onEventBit(void*, s32);
void BemosHD_fopAcM_orderOtherEvent2(void*, const char*, s32, s32);
s32 BemosHD_dEvent_manager_c_getMyStaffId(void*, const char*, s32, s32);
u32 BemosHD_dEvent_manager_c_endCheckOld(void*, const char*);
void BemosHD_dEvent_manager_c_cutEnd(void*, s32);
void BemosHD_mDoMtx_ZXYrotS(void*, s32, s32, s32);
void BemosHD_PSVECAdd(void*, void*, void*);
u32 BemosHD_dPath_GetRoomPath(u32, s32);
s32 BemosHD_cLib_targetAngleY(void*, void*);
s32 BemosHD_cLib_targetAngleX(void*, void*);
void BemosHD_cXyz_subtractInto(void*, void*, void*);
f32 BemosHD_PSVECSquareMag(void*);
f32 BemosHD_sqrtf(f32);
void BemosHD_cM3dGCyl_SetH(void*, f32);
void BemosHD_daBemos_c_yellow_eye_wait_init(void*);
u32 BemosHD_daBemos_c_getBeamActor(void*);
u32 BemosHD_daBemos_c_blue_range_check(void*, u32, void*);
void BemosHD_daBemos_c_blue_eye_charge_init(void*);
void BemosHD_cLib_chaseUC(void*, s32, s32);
void BemosHD_mDoExt_baseAnm_play(void*);
u32 BemosHD_daBemos_c_red_range_check(void*, u32, void*);
void BemosHD_daBemos_c_red_eye_charge_init(void*);
void BemosHD_cLib_addCalcAngleS2(void*, s32, s32, s32);
f32 BemosHD_PSVECDotProduct(void*, void*);
void BemosHD_PSVECScale(void*, void*, f32);
void BemosHD_cM3dGCyl_SetR(void*, f32);
s32 BemosHD_cM_atan2s(f32, f32);
u32 BemosHD_daBemos_c_yellow_range_check(void*, u32, s32);
void BemosHD_daBemos_c_yellow_eye_search_init(void*);
void BemosHD_cM3dGSph_SetC(void*, void*);
void BemosHD_cLib_chaseF(void*, f32, f32);
void BemosHD_daBemos_blue_eye_search_init(void*);
void BemosHD_JPAGetXYZRotateMtx(s32, s32, s32, void*);
void BemosHD_cLib_addCalcAngleS(void*, s32, s32, s32, s32);
void BemosHD_daBemos_red_eye_search_init(void*);
void BemosHD_fopAcM_delete(void*);
s32 BemosHD_dComIfGp_getReverb(s32);
void BemosHD_mDoAud_seStart(s32, void*, s32, s32);
void BemosHD_fopAcM_createItemFromTable(void*, u32, u32, s32, s32, void*, s32, s32);

struct BemosShapeHD {
    cXyz mMin, mMax;
    be<u32> mAabVtable, mShapeVtable;
};
WWHD_SIZE(BemosShapeHD, 0x20);
inline BemosShapeHD& bemos_shape(dCcD_Cyl& cylinder) {
    return *gabi::at<BemosShapeHD>(gabi::ea(cylinder.mShapeAttr));
}
inline dComIfG_play_c& bemos_play(u32 game) { return gabi::at<dComIfG_inf_c>(game)->play; }
inline dSv_info_c* bemos_save() { return gabi::at<dSv_info_c>(gabi::load<u32>(0x101F84DC) + 0x20); }
struct BemosPlayerVtableHD {
    u8 _pad0[0x24];
    be<u32> getGroundY;
};
WWHD_SIZE(BemosPlayerVtableHD, 0x28);
struct BemosSinCosHD {
    be<f32> sin, cos;
};
WWHD_SIZE(BemosSinCosHD, 8);
inline BemosSinCosHD& bemos_sinCos(u16 angle) { return gabi::at<BemosSinCosHD>(0x104A44F8)[angle >> 3]; }

enum dRes_INDEX_HMOS {
    /* BDL  */
    dRes_INDEX_HMOS_BDL_HMOS1_e = 0x5,
    dRes_INDEX_HMOS_BDL_HMOS2_e = 0x6,
    dRes_INDEX_HMOS_BDL_HMOS3_e = 0x7,
    dRes_INDEX_HMOS_BDL_HMOSUP1_e = 0x8,
    dRes_INDEX_HMOS_BDL_HMOSUP2_e = 0x9,
    /* BDLM */
    dRes_INDEX_HMOS_BDL_HMOS2T_e = 0xC,
    /* BRK  */
    dRes_INDEX_HMOS_BRK_HMOS2T_e = 0xF,
    dRes_INDEX_HMOS_BRK_HMOSUP1_e = 0x10,
    dRes_INDEX_HMOS_BRK_HMOSUP2_e = 0x11,
};

extern void BemosCallback_blue_eye_wait(void*);
extern void BemosCallback_red_eye_wait(void*);
extern void BemosCallback_blue_eye_search(void*);
extern void BemosCallback_red_eye_search(void*);
extern void BemosCallback_blue_eye_charge(void*);
extern void BemosCallback_red_eye_charge(void*);
extern void BemosCallback_yellow_eye_wait(void*);
extern void BemosCallback_yellow_eye_search(void*);
extern void BemosCallback_eye_break(void*);
extern void BemosCallback_eye_dummy(void*);
extern void BemosCallback_fpcSch_JudgeByID(void*);
namespace dPa_name {
enum {
    ID_AK_SN_BMOSWAIT00 = 0x815C,
    ID_AK_SN_BMOSEYE00 = 0x815D,
    ID_AK_SN_BMOSBALL00 = 0x815E,
    ID_AK_SN_LASERROOT00 = 0x8121,
    ID_AK_SN_BMOSBREAK00 = 0x815F,
    ID_AK_SN_BMOSBREAK01 = 0x8160,
    ID_AK_JN_OK = 0xD,
};
}

inline const csXyz& bemos_zeroAngle() { return *gabi::at<csXyz>(0x101FFB14); }
inline const cXyz& bemos_zero() { return *gabi::at<cXyz>(0x101FFBA8); }
inline const cXyz& bemos_baseX() { return *gabi::at<cXyz>(0x101FFBB4); }
inline const cXyz& bemos_baseZ() { return *gabi::at<cXyz>(0x101FFBCC); }
inline const be<f32>& bemos_heightReg() { return *gabi::at<be<f32>>(0x1047BBFC); }
inline be<s16>& bemos_anglePart(csXyz& angle, u32 i) { return gabi::at<be<s16>>(gabi::ea(&angle))[i]; }
inline const be<s16>& bemos_anglePart(const csXyz& angle, u32 i) {
    return gabi::at<be<s16>>(gabi::ea(&angle))[i];
}
inline be<u32>& bemos_positionWord(cXyz& pos, u32 i) { return gabi::at<be<u32>>(gabi::ea(&pos))[i]; }
inline const be<u32>& bemos_positionWord(const cXyz& pos, u32 i) {
    return gabi::at<be<u32>>(gabi::ea(&pos))[i];
}
inline gptr<JPABaseEmitter>& bemos_emitter(daBemos_c* actor, u32 i) {
    return gabi::at<gptr<JPABaseEmitter>>(gabi::ea(&actor->m2FC))[i];
}
enum { fpcNm_Beam_e = 0xE8, fpcNm_Bemos_e = 0xE9 };

// HD: the header initialization sequence and destructor records are TU-specific.
struct BemosHeaderStaticsHD {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mFirst, mSecond;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(BemosHeaderStaticsHD, mFirst, 8);
WWHD_OFFSET(BemosHeaderStaticsHD, mSecond, 9);
WWHD_OFFSET(BemosHeaderStaticsHD, mZero, 0xC);
WWHD_SIZE(BemosHeaderStaticsHD, 0x1C);
struct BemosDtorRecordHD {
    be<u32> mWords[3];
};
WWHD_SIZE(BemosDtorRecordHD, 0xC);
inline BemosHeaderStaticsHD& bemos_headerStatics() { return *gabi::at<BemosHeaderStaticsHD>(0x104692A4); }
inline BemosDtorRecordHD* bemos_dtorRecord(u32 i) { return &gabi::at<BemosDtorRecordHD>(0x101C80EC)[i]; }
inline s8 bemos_stayRoom() { return gabi::load<s8>(0x101D5F46); }
// Source descriptors have the unchanged GameCube layout; HD calls receive their original storage.
struct BemosSrcSph {
    /* 0x00 */ be<u32> mObjFlags, mAtType;
    /* 0x08 */ be<u8> mAtp;
    u8 _padding[3];
    /* 0x0C */ be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    /* 0x1C */ be<u8> mAtSe, mAtHitMark, mAtSpl, mAtMtrl;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u8> mTgSe, mTgHitMark, mTgSpl, mTgMtrl;
    /* 0x28 */ be<u32> mGTgSPrm, mGCoSPrm;
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
};
WWHD_OFFSET(BemosSrcSph, mObjFlags, 0x0);
WWHD_OFFSET(BemosSrcSph, mAtp, 0x8);
WWHD_OFFSET(BemosSrcSph, mAtSPrm, 0xC);
WWHD_OFFSET(BemosSrcSph, mAtSe, 0x1C);
WWHD_OFFSET(BemosSrcSph, mGAtSPrm, 0x20);
WWHD_OFFSET(BemosSrcSph, mTgSe, 0x24);
WWHD_OFFSET(BemosSrcSph, mGTgSPrm, 0x28);
WWHD_OFFSET(BemosSrcSph, mCenter, 0x30);
WWHD_OFFSET(BemosSrcSph, mRadius, 0x3C);
WWHD_SIZE(BemosSrcSph, 0x40);
struct BemosSrcCyl {
    /* 0x00 */ be<u32> mObjFlags, mAtType;
    /* 0x08 */ be<u8> mAtp;
    u8 _padding[3];
    /* 0x0C */ be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    /* 0x1C */ be<u8> mAtSe, mAtHitMark, mAtSpl, mAtMtrl;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u8> mTgSe, mTgHitMark, mTgSpl, mTgMtrl;
    /* 0x28 */ be<u32> mGTgSPrm, mGCoSPrm;
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
    /* 0x40 */ be<f32> mHeight;
};
WWHD_OFFSET(BemosSrcCyl, mObjFlags, 0x0);
WWHD_OFFSET(BemosSrcCyl, mAtp, 0x8);
WWHD_OFFSET(BemosSrcCyl, mAtSPrm, 0xC);
WWHD_OFFSET(BemosSrcCyl, mAtSe, 0x1C);
WWHD_OFFSET(BemosSrcCyl, mGAtSPrm, 0x20);
WWHD_OFFSET(BemosSrcCyl, mTgSe, 0x24);
WWHD_OFFSET(BemosSrcCyl, mGTgSPrm, 0x28);
WWHD_OFFSET(BemosSrcCyl, mCenter, 0x30);
WWHD_OFFSET(BemosSrcCyl, mRadius, 0x3C);
WWHD_SIZE(BemosSrcCyl, 0x44);
// HD: vtables emitted in this translation unit.
namespace BemosVtableHD {
enum : u32 {
    mGroundPolyDtor = 0x1002629C,
    mPolyPassDtor = 0x100262BC,
    mGroupPassDtor = 0x1002627C,
    mHio = 0x1002631C,
    mActor = 0x1002630C,
    mCcStts = 0x1004AE88,
    mGCcStts = 0x1004AEC0,
    mShapeAttr = 0x100015A8,
    mAab = 0x1002626C,
    mGObjInf = 0x1004B108,
    mCcCyl = 0x1004B160,
    mCyl = 0x1004B150,
    mGroundPoly = 0x100262DC,
    mGroundChk = 0x100262CC,
    mGroupPass = 0x100262EC,
    mPolyPass = 0x100262FC,
};
}
