/* d_particle: dPa_control_c and the emitter callbacks, WWHD layouts with GameCube names. More
 * GameCube API (dComIfGp_particle_setToon, dPa_name) in gc_api/d/d_particle.h. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

/* dPa_control_c (play object's particle manager, dComIfGp_getParticle()); layout not needed */
struct dPa_control_c {
    u8 _opaque;
    /* 025A847C set(u8 grp, u16 id, pos, angle, scale, u8 alpha, cb, s8 setupInfo, prmColor, envColor,
     * scale2D) [v lamp: grp 0, setup -1 on the stack] */
    JPABaseEmitter* set(u8 grp, u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                        dPa_levelEcallBack* cb, s8 setup, const GXColor* prm, const GXColor* env, const cXyz* scale2D) {
        return gabi::call<JPABaseEmitter*>(0x025A847C, this, grp, id, pos, angle, scale, alpha, cb, setup, prm, env, scale2D);
    }
    /* 025A8D40 setSimple(u16, const cXyz*, u8, const GXColor&, const GXColor&, int) [v lamp] */
    BOOL setSimple(u16 id, const cXyz* pos, u8 alpha, const GXColor& prm, const GXColor& env, int p) {
        return gabi::call<BOOL>(0x025A8D40, this, id, pos, alpha, &prm, &env, p);
    }
};

struct dPa_smokeEcallBack; /* layout below dPa_followEcallBack */
/* dPa_rippleEcallBack (HD 0x14); layout not needed here (see d_a_kb.h) */
struct dPa_rippleEcallBack {
    u8 _opaque;
    void end() { gabi::call(0x025A9270, this); } /* 025A9270 [g] */
};
/* dPa_followEcallBack (0x14, HD: vtable at +0; constructor 025A5894(this, u8, u8)) [v lamp] */
struct dPa_followEcallBack {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ be<u32> m08;
    /* 0x0C */ be<u32> m0C;
    /* 0x10 */ be<u8> m10;
    /* 0x11 */ be<u8> m11;
    /* 0x12 */ be<u8> m12;
    /* 0x13 */ be<u8> m13;
    JPABaseEmitter* getEmitter() { return mpEmitter; }
    /* remove(): virtual (vtable +0x44) */
    void remove() { gabi::call_ptr(gabi::load<u32>(__vtbl + 0x44), this); }
    /* 025A5AC8 dPa_followEcallBack::end (called directly) [g] */
    void end() { gabi::call(0x025A5AC8, this); }
};
WWHD_SIZE(dPa_followEcallBack, 0x14);
/* dPa_smokeEcallBack (0x20, unchanged from GameCube apart from the HD vtable at +0) [v nz] */
struct dPa_smokeEcallBack : dPa_followEcallBack {
    /* 0x14 */ be<s8> field_0x14;
    /* 0x15 */ be<u8> mWindOff;
    /* 0x16 */ GXColor mColor;
    /* 0x1A */ u8 _1A[2];
    /* 0x1C */ be<u32> mTevstr;
    /* 025A5F88 dPa_smokeEcallBack::end (called directly) */
    void end() { gabi::call(0x025A5F88, this); }
};
WWHD_SIZE(dPa_smokeEcallBack, 0x20);
