/* d_kankyo: g_env_light (HD: a function-local static behind dKy_getEnvlight(), 02555D0C, called
 * at every use). `g_env_light.f(args)` calls the accessor first and then evaluates the arguments,
 * as the HD code does (C++17: the object expression is sequenced before the arguments; lpalm's
 * draw loads its model after the call). */
#pragma once
#include "bindings.h"

/* a view of the guest light object: `this` is its guest address */
struct dScnKy_env_light_gc {
    dScnKy_env_light_c* self() { return (dScnKy_env_light_c*)this; }
    void settingTevStruct(int type, cXyz* pos, dKy_tevstr_c* tev) { ::settingTevStruct(self(), type, pos, tev); }
    void setLightTevColorType(J3DModel* model, dKy_tevstr_c* tev) { ::setLightTevColorType(self(), model, tev); }
};
#define g_env_light (*(dScnKy_env_light_gc*)dKy_getEnvlight())

/* 0255B9C8 [v d_kankyo_demo] */
inline void dKy_efplight_set(LIGHT_INFLUENCE* light) { gabi::call(0x0255B9C8, light); }
/* 0255BA9C [v d_kankyo_demo] */
inline void dKy_efplight_cut(LIGHT_INFLUENCE* light) { gabi::call(0x0255BA9C, light); }

/* 0255FE50 [v d_a_bmd] */
inline void dKy_custom_timeset(f32 speed) { gabi::call(0x0255FE50, speed); }
/* 0255FDA4 [v d_a_bmd]: HD forwards full integer argument registers. */
inline void dKy_custom_colset(s32 from, s32 to, f32 blend) { gabi::call(0x0255FDA4, from, to, blend); }
