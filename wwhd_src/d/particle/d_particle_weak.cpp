/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
// HD: shared no-op callback entries retain their independent addresses and harness signatures.
// Names follow the image-checked emitter and particle callback slots.
/* 025A5A00 */
void dPa_followDrawNoOp() {}
/* 025AAB5C */
void dPa_emitterExecuteNoOp() {}
/* 025AAB60 */
void dPa_emitterExecuteAfterNoOp() {}
/* 025AAB64 */
void dPa_emitterDrawNoOp() {}
/* 025AAB68 */
void dPa_emitterDrawBeforeNoOp() {}
/* 025AAB6C */
void dPa_particleInitNoOp() {}
/* 025AAB70 */
void dPa_particleDrawNoOp() {}
/* 025AAB74 */
void dPa_particleDrawAfterNoOp() {}
/* 025AABA0 */
void dPa_selectTextureSetupNoOp() {}
/* 025AAC04 */
void dPa_setColorSetupNoOp() {}
/* 025AAC30 */
void dPa_kageroDrawNoOp() {}
/* 025AACC4 */
void dPa_bombSmokeSetupNoOp() {}
/* 025AAE04 */
void dPa_shaderNameNoOp() {}
/* 025AAB48 */
void dPa_shaderNameDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAB78 */
void dPa_modelEmitterBaseDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAB8C */
void dPa_windParticleDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AABA4 */
void dPa_selectTextureDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC08 */
void dPa_setColorDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC1C */
void dPa_stripesDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC34 */
void dPa_kageroDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC48 */
void dPa_smokeDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC88 */
void dPa_smokeParticleDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AAC9C */
void dPa_rippleParticleDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AACB0 */
void dPa_singleRippleDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AACC8 */
void dPa_bombSmokeDtor(void* obj, u32 flag) { destroy(obj, flag); }
/* 025AABB8 */
void dPa_setColorExecute(void* callback, void* emitter) {
    gabi::Local<ColorF> color;
    pa_colorToFloat(color.get(), p(gabi::ea(callback) + 4));
    u32 a = gabi::ea(color.get()), e = gabi::ea(emitter);
    u32 x = w(a), y = w(a + 4), z = w(a + 8), alpha = w(a + 12);
    W(e + 0x2A0, x);
    W(e + 0x2A4, y);
    W(e + 0x2A8, z);
    W(e + 0x2AC, alpha);
}
/* 025AAAD0 */
void dPa_groundCheckDtor(void* obj, u32 flag) {
    if (obj) {
        u32 a = gabi::ea(obj);
        W(a + 0x20, 0x10051B1C);
        W(a + 0x24, 0x10051B3C);
        W(a + 0x30, 0x10051A8C);
        pa_cBgS_GndChk_baseDtor(p(a + 16), u32(0));
        destroy(obj, flag);
    }
}
/* 025AAC5C */
void* dPa_callbackPairCtor(void* obj) { return allocate(obj, 8); }
/* 025AACDC */
void* dPa_colorScaleCtor(void* obj) {
    obj = allocate(obj, 24);
    if (obj) {
        u32 a = gabi::ea(obj);
        W(a + 4, 0x10051A34);
        W(a, 0x10051A18);
        f32 one = (f32)l_particleOne;
        F(a + 20, one);
        F(a + 8, one);
        F(a + 16, one);
        F(a + 12, one);
    }
    return obj;
}
/* 025AAD64 */
void* dPa_referenceCountCtor(void* obj) {
    obj = allocate(obj, 4);
    if (obj)
        W(gabi::ea(obj), 0);
    return obj;
}
/* 025AADA0 */
void dPa_modelEmitterDrawThunk(void* obj, u32 flag) {
    u32 a = gabi::ea(obj) + 16, fn = w(w(a) + 12);
    gabi::call_ptr<void>(fn, p(a), flag);
}
/* 025AADB0 */
void dPa_boundMethodInvoke(void* obj, u32 arg) {
    u32 a = gabi::ea(obj), targetObj = w(a + 4);
    if (!targetObj)
        return;
    s16 slot = sh(a + 10);
    if (!slot)
        return;
    targetObj += s32(sh(a + 8));
    u32 fn;
    if (slot < 0)
        fn = w(a + 12);
    else
        fn = w(w(targetObj + s32(sh(a + 14))) + u32(slot) * 8 + 4);
    gabi::call_ptr<void>(fn, p(targetObj), arg);
}
/* 025AAE08 */
u32 dPa_referenceCountDecrement(void* obj) {
    u32 a = gabi::ea(obj), count = w(a);
    if (count)
        W(a, --count);
    return count;
}
/* 025AAE24 */
void dPa_mathHeaderInit() {
    W(0x1047B304, 0);
    W(0x1047B2FC, 0);
    W(0x1047B308, 0);
    W(0x1047B300, 0);
    pa_registerGlobalObject(p(0x101EA4E8));
    f32 lo = f(0x10052458), hi = f(0x1005245C);
    F(0x1047B2F0, lo);
    F(0x1047B2F4, hi);
    pa_initializeMathHeaderPrimary(p(0x1047B2F8));
    pa_registerGlobalObject(p(0x101EA4F4));
    pa_initializeMathHeaderSecondary(p(0x1047B2F9));
    pa_registerGlobalObject(p(0x101EA500));
}
/* WWHD glue
*/
