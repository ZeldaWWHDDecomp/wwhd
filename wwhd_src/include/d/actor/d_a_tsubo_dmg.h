// Pot damage and environmental modes. Non-owning view of the shared pot layout.
// HD: actor members retain the GameCube names at offsets shifted by 0x11C.
#pragma once
#include "d/actor/d_a_tsubo.h"

namespace daTsuboDamage {
struct Act_c : daTsubo::Act_c {
    void se_break(cBgS_PolyInfo* p);
    void spec_kill();
    void damaged(s32 arg1, cBgS_PolyInfo* arg2, bool arg3, const cXyz* arg4);
    void damaged(s32 arg1, cBgS_PolyInfo* arg2);
    void set_wind_vec();
    void mode_walk_init();
    bool damage_tg_acc();
    void se_pickup();
    bool damage_cc_proc();
    u8 chk_sink_water();
    bool chk_sink_lava();
    void se_fall_water();
    void eff_hit_water_splash();
    void mode_afl_init();
    void mode_sink_init();
    void se_fall_lava();
    void eff_hit_lava_splash();
    void damaged_lava();
    bool chk_sinkdown_water();
    bool damage_bg_proc();
    void eff_land_smoke();
    void cam_lockoff();
    void eff_drop_water();
    bool damage_bg_proc_directly();
    bool damage_kill_proc();
    void spec_mode_carry_init();
    void se_pickup_carry_init();
    void mode_carry_init();
    void crr_pos_water();
    void crr_pos_lava();
    void crr_pos(const cXyz* arg1);
    void bound(f32 arg1);
};
WWHD_SIZE(Act_c, 0x93C);
WWHD_OFFSET(Act_c, mAcch, 0x3BC);
WWHD_OFFSET(Act_c, mCyl, 0x624);
WWHD_OFFSET(Act_c, mType, 0x790);
WWHD_OFFSET(Act_c, m814, 0x930);

struct EffectTable { gabi::ptmf<void (Act_c::*)()> mProc[16]; };
WWHD_SIZE(EffectTable, 0x80);
struct ShapeVtable {
    u8 mPadding[0x9C];
    gfn GetNVec;
};
WWHD_OFFSET(ShapeVtable, GetNVec, 0x9C);
WWHD_SIZE(ShapeVtable, 0xA0);
struct ShapeAttr {
    u8 mPadding[0x1C];
    gptr<ShapeVtable> mpVtable;
    BOOL GetNVec(cXyz* position, cXyz* normal) {
        return gabi::call_ptr<BOOL>(mpVtable->GetNVec.get(), this, position, normal);
    }
};
WWHD_OFFSET(ShapeAttr, mpVtable, 0x1C);
WWHD_SIZE(ShapeAttr, 0x20);
struct HitVtable {
    u8 mPadding[0x2C];
    gfn GetShapeAttr;
};
WWHD_OFFSET(HitVtable, GetShapeAttr, 0x2C);
WWHD_SIZE(HitVtable, 0x30);
struct HitObject {
    u8 mPrefix[0x10];
    be<u32> mAtType;
    u8 mPadding[0x28];
    gptr<HitVtable> mpVtable;
    u32 GetAtType() const { return mAtType; }
    ShapeAttr* GetShapeAttr() {
        return gabi::call_ptr<ShapeAttr*>(mpVtable->GetShapeAttr.get(), this);
    }
};
WWHD_OFFSET(HitObject, mAtType, 0x10);
WWHD_OFFSET(HitObject, mpVtable, 0x3C);
WWHD_SIZE(HitObject, 0x40);
struct Camera {
    u8 mPadding[0x248];
    u8 mCamera;
};
WWHD_OFFSET(Camera, mCamera, 0x248);
WWHD_SIZE(Camera, 0x249);
struct CameraPlay {
    u8 mPadding[0x5AF8];
    gptr<Camera> mpCamera;
};
WWHD_OFFSET(CameraPlay, mpCamera, 0x5AF8);
WWHD_SIZE(CameraPlay, 0x5AFC);

}

void tsuboDamage_init_rot_clean(daTsuboDamage::Act_c* a);
void tsuboDamage_mode_wait_init(daTsuboDamage::Act_c* a);
void tsuboDamage_set_senv(daTsuboDamage::Act_c* a, s32 p1, s32 p2);
void tsuboDamage_set_senv_r6(daTsuboDamage::Act_c* a, s32 p1, s32 p2, u32 dataRow);
u32 tsuboDamage_fopKyM_create(s16 name, s32 param, const cXyz* pos, cXyz* scale, u32 fn);
void tsuboDamage_fopKyM_createMpillar(const cXyz* pos, f32 scale);
void tsuboDamage_daObj_HitSeStart(cXyz* pos, s32 room, dCcD_GObjInf* obj, u32 se, u32 dataRow);
void tsuboDamage_daObj_HitEff_kikuzu(fopAc_ac_c* a, dCcD_Cyl* cyl);
void tsuboDamage_daObj_HitEff_hibana(fopAc_ac_c* a, dCcD_Cyl* cyl);
void tsuboDamage_daObj_make_land_effect(fopAc_ac_c* a, dBgS_GndChk* ground, f32 scale);
void tsuboDamage_cameraForceLockOff(void* camera, u32 actorId);
BOOL tsuboDamage_ChkMoveBG_NoDABg(dBgS* bgs, cBgS_PolyInfo* poly);
BOOL tsuboDamage_ChkMoveBG(dBgS* bgs, cBgS_PolyInfo* poly);
void tsuboDamage_C_VECReflect(const cXyz* src, const void* normal, cXyz* dst);

enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_UNK8 = 0x8, AT_TYPE_BOMB = 0x20, AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_WIND = 0x200000,
    /* SWORD | BOKO_STICK | MACHETE | UNK800 | SPIKE | UNK2000 | DARKNUT_SWORD | MOBLIN_SPEAR */
    AT_TYPE_CUT_MASK = 0x14003C82,
};
enum { fpcNm_PLAYER_e = 0xA8 };
enum {
    JA_SE_OBJ_WPOT_LIFTUP = 0x6943, JA_SE_OBJ_BOKKURI_PICK_UP = 0x69F1, JA_SE_OBJ_BOKKURI_BOUND = 0x69F2,
};
enum { dPa_ID_IT_SN_WPOT_PITYA = 0x8085, dPa_ID_IT_SN_WPOT_PITYA_C = 0x8092 };
constexpr f32 G_CM3D_F_INF_NEG = -1000000000.0f;

