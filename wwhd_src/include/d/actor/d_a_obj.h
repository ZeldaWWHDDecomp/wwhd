#pragma once
#include "f_op/f_op_actor.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"

namespace daObj {
struct ObjQuaternion { be<f32> x, y, z, w; };
static_assert(sizeof(ObjQuaternion) == 16);
void make_land_effect(fopAc_ac_c*, dBgS_GndChk*, f32);
// HD returns the guest address of static vector storage in r3.
cXyz* get_wind_spd(fopAc_ac_c*, f32);
cXyz* get_path_spd(cBgS_PolyInfo*, f32);
void posMoveF_stream(fopAc_ac_c*, const cXyz*, const cXyz*, f32, f32);
void posMoveF_grade(fopAc_ac_c*, const cXyz*, const cXyz*, f32, f32,
                    const cXyz*, f32, f32, const cXyz*);
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
}
