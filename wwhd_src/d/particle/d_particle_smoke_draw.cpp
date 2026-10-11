/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
struct SmokeLocals {
    u8 bytes[332];
};
static void clearSmoke(u32 dest) {
    u32 end = dest + 0x120;
    while (dest < end) {
        for (u32 i = 0; i < 32; i++)
            B((dest & ~31u) + i, 0);
        dest += 32;
    }
}
// The translated reference folds native 025A6284 width + left into width
// minus the unnegated product; retain that NaN operand sign for this target.
/* 025A60C4 */
void dPa_smokeParticleDraw(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle), e = gabi::ea(emitter);
    if ((u8)gabi::at<pa::JPABaseParticle>(q)->hdHidden)
        return;
    gabi::Local<SmokeLocals> local;
    u32 l = gabi::ea(local.get());
    auto P = [&](u32 off) { return p(l + off - 8); };
    auto L = [&](u32 off) { return f(l + off - 8); };
    auto S = [&](u32 off, f32 v) { F(l + off - 8, v); };
    f32 y = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y,
        sizeY = (f32)gabi::at<pa::JPABaseParticle>(q)->mScaleY;
    S(0x3C, y);
    u32 emitterInfo = w(e + 0x150);
    f32 x = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x,
        z = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z;
    S(0x38, x);
    sizeY += sizeY;
    S(0x40, z);
    f32 sy = f(emitterInfo + 8), sizeX = (f32)gabi::at<pa::JPABaseParticle>(q)->mScaleX,
        height = gabi::fmuls_ppc(sizeY, sy), width;
    sizeX += sizeX;
    f32 sx = f(emitterInfo + 4);
    width = gabi::fmuls_ppc(sizeX, sx);
    u32 trig = 0x104A44F8 + ((u16)gabi::at<pa::JPABaseParticle>(q)->mRotateAngle >> 3) * 8;
    f32 sine = f(trig), cosine = f(trig + 4);
    s32 bank = s32((u32)gabi::at<pa::JPABaseParticle>(q)->hdDrawIndex);
    if (bank >= 0) {
        u32 scale = w(e + 0x154 + u32(bank) * 4);
        width = gabi::fmuls_ppc(sizeX, f(scale + 4));
        height = gabi::fmuls_ppc(sizeY, f(scale + 8));
    }
    f32 half = (f32)l_particleHalf;
    u32 countX = pa_getParticleWidthCount(emitter);
    f32 left = -((gabi::fmuls_ppc(width, f32(countX))) * half);
    width = gabi::fsubs_ppc(width, -left);
    u32 countY = pa_getParticleHeightCount(emitter);
    u32 matrix = w(w(e + 0x150) + 0x34);
    f32 bottom = -((gabi::fmuls_ppc(height, f32(countY))) * half);
    u8 alpha = (u8)gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a;
    height = gabi::fadds_ppc(height, bottom);
    bool special = false;
    if ((u8)gabi::at<pa::JPABaseParticle>(q)->hdSmokeMode) {
        u32 obj = w(w(e + 0x1E0) + 4), target = w(w(obj) + 0x104);
        special = gabi::call_ptr<s32>(target, p(obj)) != 0;
    }
    if (special) {
        pa_constructArray(P(0xBC), u32(4), u32(8), p(0x025AAC5C));
        f32 cl = gabi::fmuls_ppc(cosine, left), ch = gabi::fmuls_ppc(cosine, height),
            sh = gabi::fmuls_ppc(sine, height), sl = gabi::fmuls_ppc(sine, left),
            sw = gabi::fmuls_ppc(sine, width), cb = gabi::fmuls_ppc(cosine, bottom),
            sb = gabi::fmuls_ppc(sine, bottom), cw = gabi::fmuls_ppc(cosine, width);
        S(0xC0, gabi::fadds_ppc(ch, sl));
        S(0xBC, gabi::fsubs_ppc(cl, sh));
        S(0xC8, gabi::fadds_ppc(ch, sw));
        S(0xC4, gabi::fsubs_ppc(cw, sh));
        S(0xCC, gabi::fsubs_ppc(cw, sb));
        S(0xD0, gabi::fadds_ppc(cb, sw));
        S(0xD4, gabi::fsubs_ppc(cl, sb));
        S(0xD8, gabi::fadds_ppc(cb, sl));
        pa_PSMTXIdentity(P(0x11C));
        pa_PSMTXMultVec(p(matrix), P(0x38), P(0x38));
        x = L(0x38);
        y = L(0x3C);
        z = L(0x40);
        S(0x80, gabi::fadds_ppc(L(0xBC), x));
        S(0x84, gabi::fadds_ppc(L(0xC0), y));
        S(0x88, z);
        S(0x98, gabi::fadds_ppc(L(0xC4), x));
        S(0x9C, gabi::fadds_ppc(L(0xC8), y));
        S(0xA0, z);
        S(0xA4, gabi::fadds_ppc(L(0xCC), x));
        S(0xA8, gabi::fadds_ppc(L(0xD0), y));
        S(0xAC, z);
        S(0xB0, gabi::fadds_ppc(L(0xD4), x));
        S(0xB4, gabi::fadds_ppc(L(0xD8), y));
        S(0xB8, z);
        f32 vectors[5][3] = {{f(matrix + 12) - x, f(matrix + 28) - y, f(matrix + 44) - z},
                             {gabi::fsubs_ppc(L(0x80), x), gabi::fsubs_ppc(L(0x84), y), z - z},
                             {gabi::fsubs_ppc(L(0x98), x), gabi::fsubs_ppc(L(0x9C), y), z - z},
                             {gabi::fsubs_ppc(L(0xA4), x), gabi::fsubs_ppc(L(0xA8), y), z - z},
                             {gabi::fsubs_ppc(L(0xB0), x), gabi::fsubs_ppc(L(0xB4), y), z - z}};
        f32 eps = (f32)l_particleNormalizeEpsilon, one = (f32)l_particleOne;
        for (u32 i = 0; i < 5; i++) {
            f32* v = vectors[i];
            f32 squared = gabi::fmadds(v[2], v[2], gabi::fmadds(v[0], v[0], v[1] * v[1]));
            if (squared > eps) {
                f32 inverse = one / f32(pa_sqrtHD(squared));
                v[0] *= inverse;
                v[1] *= inverse;
                v[2] *= inverse;
            }
            S(0x44 + i * 12, v[0]);
            S(0x48 + i * 12, v[1]);
            S(0x4C + i * 12, v[2]);
        }
        pa_PSMTXInverse(p(matrix), P(0xEC));
        for (u32 off : {0x80u, 0x98u, 0xA4u, 0xB0u})
            pa_PSMTXMultVec(P(0xEC), P(off), P(off));
        S(0x3C, (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y);
        S(0x38, (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x);
        S(0x40, (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z);
        u32 block = (u32)gabi::at<pa::JPABaseParticle>(q)->hdMaterial,
            current = block + (u32)gabi::at<pa::ParticleSmokeBuffers>(block)->mBank * 0x254;
        clearSmoke((u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData);
        current = block + (u32)gabi::at<pa::ParticleSmokeBuffers>(block)->mBank * 0x254;
        u32 dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData;
        f32 opacity = f32(alpha) / (f32)l_particleColorRange, zero = (f32)l_particleZero;
        u32 positions[6] = {0x38, 0x80, 0x98, 0xA4, 0xB0, 0x80}, normals[6] = {0, 1, 2, 3, 4, 1};
        for (u32 i = 0; i < 6; i++) {
            u32 d = dest + i * 48, off = positions[i], n = normals[i];
            F(d, L(off));
            F(d + 4, L(off + 4));
            F(d + 8, L(off + 8));
            F(d + 12, L(0x44 + n * 12));
            F(d + 16, L(0x48 + n * 12));
            F(d + 20, L(0x4C + n * 12));
            f32 uvx = i == 0 ? half : (i == 2 || i == 3 ? one : zero),
                uvy = i == 0 ? half : (i == 3 || i == 4 ? one : zero);
            F(d + 24, uvx);
            F(d + 28, uvy);
            F(d + 32, one);
            F(d + 36, one);
            F(d + 40, one);
            F(d + 44, opacity);
        }
    }
    if (!special) {
        pa_colorToFloat(P(0xDC), p(0x101D5E94));
        pa_setMaterialAmbientColor(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModel), P(0xDC),
                                   u32(0));
        pa_setMaterialColor(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModel), p(e + 0x290),
                            u32(0));
        pa_setMaterialKonstColor(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModel), p(e + 0x2B0),
                                 u32(0));
        pa_setMaterialKonstColor(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModel), p(e + 0x2C0),
                                 u32(1));
        pa_loadMaterial(p((u32)gabi::at<pa::JPABaseParticle>(q)->hdModel));
    } else {
        u32 block = (u32)gabi::at<pa::JPABaseParticle>(q)->hdMaterial,
            current = block + (u32)gabi::at<pa::ParticleSmokeBuffers>(block)->mBank * 0x254;
        pa_flushVertexBuffer(p(current + 4), u32(0),
                             (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mVertexBuffer.mSize);
        gabi::at<pa::ParticleSmokeBuffers>(block)->mBank =
            (u32)gabi::at<pa::ParticleSmokeBuffers>(block)->mBank == 0 ? 1 : 0;
    }
}
/* WWHD glue
*/
