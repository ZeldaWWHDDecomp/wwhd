#pragma once
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"

namespace daObj {
struct ObjQuaternion {
    be<f32> x, y, z, w;
};
static_assert(sizeof(ObjQuaternion) == 16);
void make_land_effect(fopAc_ac_c*, dBgS_GndChk*, f32);
// HD returns the guest address of static vector storage in r3.
cXyz* get_wind_spd(fopAc_ac_c*, f32);
cXyz* get_path_spd(cBgS_PolyInfo*, f32);
void posMoveF_stream(fopAc_ac_c*, const cXyz*, const cXyz*, f32, f32);
void posMoveF_grade(fopAc_ac_c*, const cXyz*, const cXyz*, f32, f32, const cXyz*, f32, f32,
                    const cXyz*);
void quat_rotBaseY(ObjQuaternion*, const cXyz*);
void quat_rotBaseY2(ObjQuaternion*, const cXyz*);
void quat_rotBaseZ(ObjQuaternion*, const cXyz*);
void quat_rotVec(ObjQuaternion*, const cXyz*, const cXyz*);
void SetCurrentRoomNo(fopAc_ac_c*, dBgS_GndChk*);
void HitSeStart(const cXyz*, s32, const dCcD_GObjInf*, u32);
void HitEff_sub_kikuzu(const cXyz*, const cXyz*, const dKy_tevstr_c*);
void HitEff_kikuzu(const fopAc_ac_c*, const dCcD_Cyl*);
void HitEff_hibana(const cXyz*, const cXyz*);
void HitEff_hibana(const fopAc_ac_c*, const dCcD_Cyl*);
} // namespace daObj

// HD: private views for this utility's particle and collision dispatch paths.
namespace objHD {
enum {
    dBgS_Attr_GRASS_e = 4, dBgS_Attr_GIANT_FLOWER_e = 5, dBgS_Attr_CARPET_e = 10,
    dBgS_Attr_ICE_e = 15, dBgS_Attr_WATER_e = 19, dBgS_Attr_UNK1B_e = 27,
    ID_AK_JN_ELEMENTSHIBUKI00 = 0x23, ID_AK_JN_ELEMENTKUSA00 = 0x24,
    ID_AK_JN_ELEMENTKIKUZU00 = 0x2B, ID_AK_JN_ELEMENTHIBANA00 = 0x2C,
    OBJECT_EFFECT_PROCESS = 0x1D3, LAND_SMOKE = 3,
    AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_ARROWS = 0x1C4000, AT_TYPE_HOOKSHOT = 0x8000,
    fpcNm_PLAYER_e = 0xA8, dCcG_SE_WOOD = 4, dCcG_SE_METAL = 5,
    dItemNo_MASTER_SWORD_1_e = 0x39, dItemNo_MASTER_SWORD_2_e = 0x3A, dItemNo_MASTER_SWORD_3_e = 0x3E,
    JA_SE_LK_HAMMER_HIT = 0x2855, JA_SE_LK_ARROW_REBOUND = 0x287A, JA_SE_LK_HS_REBOUND = 0x287B,
    JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_SW_HIT_M = 0x2805, JA_SE_LK_SW_HIT_S = 0x2803,
};
struct Linkage {
    u32 words[8];
};
struct QuaternionWords {
    be<u32> mWord[4];
};
struct SinCos {
    be<f32> mSin, mCos;
};
struct SinCosTable { SinCos mValues[8192]; };
struct ShapeVtable {
    u8 padding0[0x8C];
    gfn getCoCP;
};
struct CylinderShape {
    u8 padding0[0x1C];
    gptr<ShapeVtable> __vtbl;
};
struct Emitter {
    u8 padding0[0x14];
    cXyz mTranslation;
    u8 padding20[0x34 - 0x20];
    be<f32> mRate;
    u8 padding38[0x58 - 0x38];
    be<f32> mSpread;
    be<u32> mMaxFrame;
    be<u16> mLifeTime;
    u8 padding62[0x6C - 0x62];
    be<f32> mInitialVelAxis, mInitialVelDir;
    u8 padding74[8];
    be<f32> mVolumeSweep;
};
struct HeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    u8 mAngleTag, mByteTag;
    u8 paddingA[2];
    be<u32> mZero[4];
};
struct HeaderVector {
    u8 mStorage[12];
};
WWHD_SIZE(Linkage, 0x20);
WWHD_SIZE(QuaternionWords, 0x10);
WWHD_SIZE(SinCos, 8);
WWHD_OFFSET(ShapeVtable, getCoCP, 0x8C);
WWHD_OFFSET(CylinderShape, __vtbl, 0x1C);
WWHD_OFFSET(Emitter, mTranslation, 0x14);
WWHD_OFFSET(Emitter, mRate, 0x34);
WWHD_OFFSET(Emitter, mSpread, 0x58);
WWHD_OFFSET(Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mInitialVelAxis, 0x6C);
WWHD_OFFSET(Emitter, mInitialVelDir, 0x70);
WWHD_OFFSET(Emitter, mVolumeSweep, 0x7C);
WWHD_OFFSET(HeaderStatics, mAngleTag, 8);
WWHD_OFFSET(HeaderStatics, mByteTag, 9);
WWHD_OFFSET(HeaderStatics, mZero, 0xC);
WWHD_SIZE(HeaderVector, 12);
} // namespace objHD
