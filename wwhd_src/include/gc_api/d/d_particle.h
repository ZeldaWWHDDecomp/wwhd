/* d_particle (GameCube names). Layouts: dPa_control_c / dPa_followEcallBack / dPa_smokeEcallBack in d/d_particle.h,
 * JPABaseEmitter in gc_api/JSystem/JPAEmitter.h. */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JPAEmitter.h"

namespace dPa_name {
enum {
    ID_IT_JN_LK_GEPPU00 = 0x0057,
    ID_AK_JT_ELEMENTSMOKE00 = 0x2022,
};
}
/* HD: dPa_control_c::set with group 2, the room number as setup info [v nz, mo2] */
inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                 dPa_smokeEcallBack* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return pa->set(2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
/* compiler-generated construction (HD) */
inline void __ct(dPa_smokeEcallBack* cb, u8 p) { gabi::call(0x025A5B18, cb, p); }

namespace dPa_name {
enum {
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUPOFU = 0x14,
    ID_AK_JN_SIBOUSPIRIT = 0x15,
    ID_AK_JN_SIBOUFLASH = 0x16,
    ID_IT_JN_PUCHI_SHIBOUA = 0x43C,
    ID_IT_JN_PUCHI_SHIBOUB = 0x43D,
    ID_IT_JN_PUCHI_SHIBOUC = 0x43E,
}; // [v d_a_disappear]
}
/* 025A86F4 dPa_control_c::setNormalStripes [v d_a_disappear]. */
inline void dComIfGp_particle_setStripes(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha, u16 rate) {
    dPa_control_c* pa = dComIfGp_getParticle();
    gabi::call(0x025A86F4, pa, id, pos, angle, scale, alpha, rate);
}

/* 025A5B18 [v d_a_bmd]: the array-constructor adapter forwards the returned pointer. */
inline void* dPa_smokeEcallBack_construct(void* cb, s32 flag) { return gabi::call<void*>(0x025A5B18, cb, flag); }
