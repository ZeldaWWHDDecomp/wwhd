/* Floating barrel spawner. GameCube actor fields shift by +0x11C in HD;
 * collision check layouts are unchanged. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

namespace daObjComing {
struct Act_c;
struct PrmBarrel2_c {
    cXyz mPos;
    be<s32> mType, mItem;
    be<u8> m14;
    u8 padding;
    be<s16> mAngleY;
};
struct Ctrl_c {
    enum Mode { MODE_WAIT, MODE_APPEAR, MODE_LEAVE };
    void init(const Act_c*);
    void init_timer(const Act_c*);
    void set_wait_timer(const Act_c*);
    void set_retry_timer(const Act_c*);
    void set_appear_timer(const Act_c*);
    be<s32> m00, m04;
    be<u32> m08;
};
struct Act_c : fopAc_ac_c {
    enum Ptn_e {
        PTN_00_e, PTN_01_e, PTN_02_e, PTN_03_e, PTN_04_e, PTN_05_e,
        PTN_06_e, PTN_07_e, PTN_08_e, PTN_09_e, PTN_0A_e, PTN_0B_e,
        PTN_0C_e, PTN_0D_e, PTN_0E_e, PTN_0F_e, PTN_10_e, PTN_11_e,
        PTN_12_e, PTN_13_e, PTN_14_e, PTN_15_e
    };
    enum Prm_e {
        PRM_WAIT_TIME_W = 2, PRM_WAIT_TIME_S = 24,
        PRM_RANGE_W = 1, PRM_RANGE_S = 6,
        PRM_TBL_NUM_W = 3, PRM_TBL_NUM_S = 8,
        PRM_SAVE_W = 8, PRM_SAVE_S = 16,
        PRM_ALWAYS_W = 1, PRM_ALWAYS_S = 6,
        PRM_MAKENUM_W = 2, PRM_MAKENUM_S = 28,
        PRM_TYPE_W = 4, PRM_TYPE_S = 0
    };
    struct MakeTmp_c { cXyz m00, m0C; be<f32> m18, m1C; };
    s32 _create();
    s32 _execute();
    // HD: keep the full r3 result at guest call boundaries (GC uses bool/enums).
    u32 chk_switch();
    u32 chk_ship_spd(const fopAc_ac_c*);
    u32 chk_ship_range(const fopAc_ac_c*);
    void get_make_pos(const fopAc_ac_c*, cXyz*);
    u32 chk_make_pos_gnd(const MakeTmp_c*);
    u32 chk_make_pos_wall(const MakeTmp_c*);
    u32 chk_make_pos(const MakeTmp_c*);
    u32 barrel2_get_ptn();
    u32 barrel2_get_param(PrmBarrel2_c*);
    void mode_barrel2_wait(Ctrl_c*);
    void mode_barrel2_appear(Ctrl_c*);
    void mode_barrel2_leave(Ctrl_c*);
    /* GC 0x290 -> HD 0x3AC */ be<u32> m290;
    /* GC 0x294 -> HD 0x3B0 */ Ctrl_c m294[5];
    /* GC 0x2D0 -> HD 0x3EC */ be<s8> mRoomId;
    u8 padding[3];
};
WWHD_SIZE(PrmBarrel2_c, 0x18);
WWHD_OFFSET(PrmBarrel2_c, mType, 0x0C);
WWHD_OFFSET(PrmBarrel2_c, mItem, 0x10);
WWHD_OFFSET(PrmBarrel2_c, m14, 0x14);
WWHD_OFFSET(PrmBarrel2_c, mAngleY, 0x16);
WWHD_SIZE(Ctrl_c, 0x0C);
WWHD_OFFSET(Ctrl_c, m04, 4);
WWHD_OFFSET(Ctrl_c, m08, 8);
WWHD_SIZE(Act_c::MakeTmp_c, 0x20);
WWHD_OFFSET(Act_c::MakeTmp_c, m0C, 0x0C);
WWHD_OFFSET(Act_c::MakeTmp_c, m18, 0x18);
WWHD_OFFSET(Act_c::MakeTmp_c, m1C, 0x1C);
WWHD_SIZE(Act_c, 0x3F0);
WWHD_OFFSET(Act_c, m290, 0x3AC);
WWHD_OFFSET(Act_c, m294, 0x3B0);
WWHD_OFFSET(Act_c, mRoomId, 0x3EC);
}

// HD: fields of dBgS_SplGrpChk absent from the shared opaque water-check view.
struct ComingWaterChk {
    cBgS_Chk mCheck;
    u8 padding14[0x0C];
    be<u32> mPolyVtable, mPassVtable;
    u8 padding28[8];
    be<u32> mGroupVtable;
    u8 padding34[4];
    cXyz mPos;
    be<f32> mUpperY, mHeight;
    u8 padding4C[4];
};
WWHD_SIZE(ComingWaterChk, 0x50);
WWHD_OFFSET(ComingWaterChk, mCheck, 0);
WWHD_OFFSET(ComingWaterChk, mPolyVtable, 0x20);
WWHD_OFFSET(ComingWaterChk, mPassVtable, 0x24);
WWHD_OFFSET(ComingWaterChk, mGroupVtable, 0x30);
WWHD_OFFSET(ComingWaterChk, mPos, 0x38);
WWHD_OFFSET(ComingWaterChk, mUpperY, 0x44);
WWHD_OFFSET(ComingWaterChk, mHeight, 0x48);
struct ComingBarrel2 : fopAc_ac_c {
    u8 padding[0x590 - sizeof(fopAc_ac_c)];
    be<u8> mExit;
};
WWHD_OFFSET(ComingBarrel2, mExit, 0x590);
WWHD_SIZE(ComingBarrel2, 0x591);

// HD: named guest-call boundaries preserve callee stubs and full register widths.
u32 Coming_PrmAbstract(const daObjComing::Act_c*, u32, u32);
void Coming_assert(const char*, s32, const char*);
f32 Coming_rndF(f32);
f32 Coming_rnd();
f32 Coming_rndFX(f32);
void Coming_initTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*);
void Coming_init(daObjComing::Ctrl_c*, const daObjComing::Act_c*);
void Coming_waitTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*);
void Coming_retryTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*);
void Coming_appearTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*);
void Coming_actorCtor(fopAc_ac_c*);
void Coming_disableDraw(create_tag_class*);
u32 Coming_getRoomId(dBgS*, cBgS_PolyInfo*);
void Coming_deleteActor(fopAc_ac_c*);
u32 Coming_isSwitch(void*, u32, s32);
u32 Coming_seaCheck(f32, f32);
f32 Coming_waveY(f32, f32);
void Coming_waterCtor(ComingWaterChk*);
void Coming_registerDestructor(void*);
u32 Coming_waterCheck(dBgS*, ComingWaterChk*);
f32 Coming_squareDistance(const cXyz*, const cXyz*);
f32 Coming_sqrt(f32);
void Coming_mtxTrans(Mtx34*, f32, f32, f32);
void Coming_mtxYrot(Mtx34*, s16);
void Coming_transM(f32, f32, f32);
void Coming_multVec(const Mtx34*, const cXyz*, cXyz*);
f32 Coming_waterY(const cXyz*);
void Coming_setLine(dBgS_ObjLinChk*, const cXyz*, const cXyz*, u32);
u32 Coming_lineCross(dBgS*, dBgS_ObjLinChk*);
u32 Coming_isActor(void*);
u32 Coming_checkGround(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*);
u32 Coming_checkWall(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*);
u32 Coming_searchActor(u32, void*);
u32 Coming_switch(daObjComing::Act_c*);
u32 Coming_shipSpeed(const daObjComing::Act_c*, const fopAc_ac_c*);
u32 Coming_shipRange(const daObjComing::Act_c*, const fopAc_ac_c*);
void Coming_makePosition(const daObjComing::Act_c*, const fopAc_ac_c*, cXyz*);
u32 Coming_checkPosition(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*);
u32 Coming_pattern(const daObjComing::Act_c*);
u32 Coming_type(u32);
u32 Coming_item(u32);
u32 Coming_buoy(u32);
s32 Coming_targetAngle(const cXyz*, const cXyz*);
u32 Coming_parameters(daObjComing::Act_c*, daObjComing::PrmBarrel2_c*);
void Coming_angleCtor(csXyz*, s32, s32, s32);
u32 Coming_createBarrel(s32, u32, const cXyz*, s32, const csXyz*, u32, s32, u32);
u32 Coming_searchById(u32, be<u32>*);
void Coming_headerByteCtor(be<u8>*);
void Coming_headerFlagCtor(be<u8>*);
void Coming_groundCtor(cBgS_GndChk*);
void Coming_lineCtor(cBgS_LinChk*);
void Coming_checkDtor(cBgS_Chk*, u32);
void Coming_polyDtor(void*, u32);
void Coming_delete(void*);
void Coming_actorDtor(fopAc_ac_c*, u32);

