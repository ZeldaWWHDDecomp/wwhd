/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
/* 025A47E0 */
void* dPa_modelControlCtor(void* obj, void* firstModel, void* secondModel, void* heap) {
    obj = allocate(obj, 16);
    if (!obj)
        return obj;
    u32 a = gabi::ea(obj);
    W(a + 12, gabi::ea(heap));
    u32 base = w(0x101FCD08);
    u32 offset = pa_allocateParticleModelPool(p(base + 0x3C0), heap, u32(4));
    u32 models = 0;
    if (offset) {
        base = w(0x101FCD08);
        models = gabi::ea(
            pa_constructModelArray(p(offset + base), u32(80), u32(12), p(0x025A479C), u32(0)));
    }
    l_particleModelPool = models;
    if (!models) {
        pa_JUTAssertion_showAssert(p(0x10051C54), u32(0x144), p(0x10051C44));
        models = (u32)l_particleModelPool;
    }
    for (u32 i = 0; i < 80; i++, models += 12) {
        void* mdl = pa_mDoExt_J3DModel_create(firstModel, u32(0x80000), u32(0x37221223));
        gabi::at<pa::dPa_J3Dmodel_c>(models)->mModel = gabi::ea(mdl);
        mdl = pa_mDoExt_J3DModel_create(secondModel, u32(0x80000), u32(0x37221223));
        u32 first = (u32)gabi::at<pa::dPa_J3Dmodel_c>(models)->mModel;
        gabi::at<pa::dPa_J3Dmodel_c>(models)->hdModel = gabi::ea(mdl);
        if (!first)
            pa_JUTAssertion_showAssert(p(0x10051C54), u32(0x169), p(0x10051C64));
    }
    pa_cLs_Create(obj);
    return obj;
}
/* 025A4984 */
void pa::dPa_control_c::modelControlDraw() {
    u32 a = gabi::ea(this), models = (u32)l_particleModelPool;
    for (u32 i = 0; i < 80; i++)
        gabi::at<pa::dPa_J3Dmodel_c>(models + i * sizeof(pa::dPa_J3Dmodel_c))->mInit = 0;
    pa_prepareParticleModelLighting();
    u32 node = w(a);
    while (node) {
        u32 emitter = w(node + 12), flags = w(emitter + 0x254), next = w(node + 8);
        if ((flags & 8) && w(emitter + 0x1B4) + w(emitter + 0x1C0) == 0) {
            W(emitter + 0x254, w(emitter + 0x254) & ~u32(0x40));
            pa_cLs_SingleCut(p(node));
            u32 fn = w(w(node + 16) + 12);
            gabi::call_ptr<void>(fn, p(node), u32(3));
        } else {
            u32 fn = w(w(node + 16) + 20);
            gabi::call_ptr<void>(fn, p(node));
        }
        node = next;
    }
}
/* 025A60B0 */
void dPa_smokeParticleExecute(void* callback, void* emitter, void* particle) {
    if (!w(gabi::ea(emitter) + 0x258))
        pa_setWindPower(particle);
}
/* 025A5FE0 */
void dPa_smokeParticleSetup(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle), e = gabi::ea(emitter);
    u32 oldMode = 1;
    void* mode = pa_getParticleCallbackContext(callback);
    if (mode) {
        u32 renderer = w(0x101F8B4C);
        oldMode = pa_exchangeParticleContext(p(renderer), mode);
    }
    u32 model = (u32)gabi::at<pa::JPABaseParticle>(q)->hdModelData;
    pa_setParticleTexture(p(e + 0x9C), u32(1), u32(1), p(model));
    model = (u32)gabi::at<pa::JPABaseParticle>(q)->hdModelData;
    u32 mat = gabi::ea(pa_getParticleMaterial(p(model), u32(1)));
    u8 flags = b(mat + 0x190);
    W(mat + 0x15C, 2);
    B(mat + 0x190, flags | 2);
    model = (u32)gabi::at<pa::JPABaseParticle>(q)->hdModelData;
    mat = gabi::ea(pa_getParticleMaterial(p(model), u32(1)));
    flags = b(mat + 0x190);
    W(mat + 0x160, 2);
    B(mat + 0x190, flags | 2);
    pa_initializeSmokeParticleModel(particle);
    if (oldMode != 1)
        pa_restoreParticleContext(p(w(0x101F8B4C)), oldMode);
}
/* 025A6AA8 */
void dPa_smokeParticleDrawAfter(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle), e = gabi::ea(emitter);
    if ((u8)gabi::at<pa::JPABaseParticle>(q)->hdHidden)
        return;
    u32 mat = (u32)gabi::at<pa::JPABaseParticle>(q)->hdModel, fn = w(w(mat + 12) + 0x2C),
        resource = w(e + 0x26C);
    gabi::call_ptr<void>(fn, p(mat), p(resource));
    pa_loadShape(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModelData), p(resource));
    u32 block = (u32)gabi::at<pa::JPABaseParticle>(q)->hdMaterial,
        bank = (u32)gabi::at<pa::ParticleSmokeBuffers>(block)->mBank == 0 ? 1 : 0;
    pa_bindVertexBuffer(p(block + bank * 0x254 + 0x158));
    u32 buffers = (u32)gabi::at<pa::JPABaseParticle>(q)->hdVertexBuffer, vertex = w(buffers),
        count = w(buffers + 8), index = w(buffers + 4);
    pa_drawIndexed(p(index), u32(6), p(vertex), count, u32(0), u32(1));
    gabi::at<pa::JPABaseParticle>(q)->mStatus = (u32)gabi::at<pa::JPABaseParticle>(q)->mStatus | 8;
}
/* 025A6B58 */
void dPa_rippleParticleExecute(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle);
    gabi::Local<Vec> pos;
    gabi::Local<f32> height;
    u32 v = gabi::ea(pos.get());
    gabi::at<pa::Vec>(v)->x = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x;
    gabi::at<pa::Vec>(v)->y = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y;
    gabi::at<pa::Vec>(v)->z = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z;
    if (pa_fopAcM_getWaterY(pos.get(), height.get()))
        gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.y = f(gabi::ea(height.get()));
}
/* 025A6BB4 */
void dPa_rippleParticleBeforeDraw(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle), e = gabi::ea(emitter);
    if ((u8)gabi::at<pa::JPABaseParticle>(q)->hdRenderMode)
        return;
    f32 width = (f32)gabi::at<pa::JPABaseParticle>(q)->mScaleX,
        twiceWidth = gabi::fadds_ppc(width, width),
        height = (f32)gabi::at<pa::JPABaseParticle>(q)->mScaleY,
        twiceHeight = gabi::fadds_ppc(height, height);
    u32 scale = w(e + 0x150);
    f32 half = (f32)l_particleHalf,
        x = gabi::fmuls_ppc(gabi::fmuls_ppc(twiceWidth, f(scale + 4)), half),
        y = gabi::fmuls_ppc(gabi::fmuls_ppc(twiceHeight, f(scale + 8)), half);
    s32 index = s32((u32)gabi::at<pa::JPABaseParticle>(q)->hdDrawIndex);
    if (index >= 0) {
        scale = w(e + 0x154 + u32(index) * 4);
        x = gabi::fmuls_ppc(gabi::fmuls_ppc(twiceWidth, f(scale + 4)), half);
        y = gabi::fmuls_ppc(gabi::fmuls_ppc(twiceHeight, f(scale + 8)), half);
    }
    u32 flags = (u32)gabi::at<pa::JPABaseParticle>(q)->mStatus;
    gabi::at<pa::JPABaseParticle>(q)->hdDrawWidth = x;
    gabi::at<pa::JPABaseParticle>(q)->hdDrawHeight = y;
    gabi::at<pa::JPABaseParticle>(q)->mStatus = flags | 8;
}
/* 025AAA88 */
void dPa_destroySelectTextureArray() {
    pa_destroyArray(p(0x1047B24C), u32(4), u32(8), p(0x025AABA4), u32(0), u32(0));
}
/* 025AAAAC */
void dPa_destroyColorArray() {
    pa_destroyArray(p(0x1047B26C), u32(3), u32(8), p(0x025AAC08), u32(0), u32(0));
}
/* WWHD glue
*/
