/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
static f32 norm2(f32 x, f32 y, f32 z) { return gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)); }
static void normalize(f32& x, f32& y, f32& z, f32 one) {
    f32 inv = one / f32(pa_sqrtHD(norm2(x, y, z)));
    x *= inv;
    z *= inv;
    y *= inv;
}
/* 025A4204 */
void dPa_modelEmitterDraw(void* emitter) {
    u32 a = gabi::ea(emitter), e = (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpBaseEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & ~JPAEmtrStts_StopDraw;
    if (e + 0x1AC) {
        gabi::Local<ParticleMatrix> matrix, rotation;
        pa_PSMTXIdentity(matrix.get());
        pa_PSMTXIdentity(rotation.get());
        u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
        f32 one = (f32)l_particleOne, eps = (f32)l_particleNormalizeEpsilon,
            radians = (f32)l_particleDegreesToRadians, angleScale = (f32)l_particleAngleToDegrees,
            zero = (f32)l_particleZero;
        while (node) {
            void* model = pa_newModel(p((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->hdModelData));
            if (!model)
                return;
            u32 out = gabi::ea(model), q = w(node), v = q + 0x8C;
            f32 y = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.y,
                x = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.x,
                z = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.z;
            if (!(norm2(x, y, z) > eps)) {
                x = zero;
                z = zero;
                y = one;
            } else if (norm2(x, y, z) > eps)
                normalize(x, y, z, one);
            f32 vx = (f32)gabi::at<pa::Vec>(v)->x, vy = (f32)gabi::at<pa::Vec>(v)->y,
                vz = (f32)gabi::at<pa::Vec>(v)->z;
            f32 cx = gabi::fmsubs(vy, z, vz * y), cy = gabi::fmsubs(vz, x, vx * z),
                cz = gabi::fmsubs(vx, y, vy * x);
            if (!(norm2(cx, cy, cz) > eps)) {
                cy = zero;
                cz = one;
                cx = zero;
            } else if (norm2(cx, cy, cz) > eps)
                normalize(cx, cy, cz, one);
            f32 nx = gabi::fmsubs(y, cz, z * cy), ny = gabi::fmsubs(z, cx, x * cz),
                nz = gabi::fmsubs(x, cy, y * cx);
            gabi::at<pa::Vec>(v)->x = nx;
            gabi::at<pa::Vec>(v)->y = ny;
            gabi::at<pa::Vec>(v)->z = nz;
            nx = (f32)gabi::at<pa::Vec>(v)->x;
            ny = (f32)gabi::at<pa::Vec>(v)->y;
            if (norm2(nx, ny, nz) > eps) {
                f32 inv = one / f32(pa_sqrtHD(norm2(nx, ny, nz)));
                nx = (f32)gabi::at<pa::Vec>(v)->x * inv;
                ny = (f32)gabi::at<pa::Vec>(v)->y * inv;
                gabi::at<pa::Vec>(v)->x = nx;
                nz = (f32)gabi::at<pa::Vec>(v)->z * inv;
                gabi::at<pa::Vec>(v)->y = ny;
                gabi::at<pa::Vec>(v)->z = nz;
            }
            matrix.get()->m[0][0] = (f32)gabi::at<pa::Vec>(v)->x;
            matrix.get()->m[0][1] = x;
            matrix.get()->m[0][2] = cx;
            matrix.get()->m[1][0] = (f32)gabi::at<pa::Vec>(v)->y;
            matrix.get()->m[1][1] = y;
            matrix.get()->m[1][2] = cy;
            matrix.get()->m[2][0] = (f32)gabi::at<pa::Vec>(v)->z;
            matrix.get()->m[2][1] = z;
            matrix.get()->m[2][2] = cz;
            f32 angle = f32((u16)gabi::at<JPABaseParticle>(q)->mRotateAngle) * angleScale;
            if (angle != zero) {
                u8 axis = (u8)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x26;
                if (axis < 3) {
                    pa_PSMTXRotRad(rotation.get(),
                                   u32(axis == 0   ? 'y'
                                       : axis == 1 ? 'x'
                                                   : 'z'),
                                   angle * radians);
                } else if (axis == 3) {
                    gabi::Local<Vec> av;
                    u32 avp = gabi::ea(av.get());
                    W(avp, w(0x10051C38));
                    W(avp + 4, w(0x10051C3C));
                    W(avp + 8, w(0x10051C40));
                    pa_PSMTXRotAxisRad(rotation.get(), av.get(), angle * radians);
                }
                pa_PSMTXConcat(matrix.get(), rotation.get(), matrix.get());
            }
            pa_PSMTXConcat(matrix.get(), rotation.get(), matrix.get());
            f32 py = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y,
                pz = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z,
                px = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x;
            matrix.get()->m[0][3] = px;
            matrix.get()->m[1][3] = py;
            matrix.get()->m[2][3] = pz;
            e = (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpBaseEmitter;
            f32 sx = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x *
                     (f32)gabi::at<JPABaseParticle>(q)->mScaleX,
                sy = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y *
                     (f32)gabi::at<JPABaseParticle>(q)->mScaleY,
                sz = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z *
                     (f32)gabi::at<JPABaseParticle>(q)->mScaleX;
            void* light = pa_getEnvlight();
            pa_setLightTevColorType(light, model, p(a + 0x2C));
            if ((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x1c &&
                (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x20) {
                F((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x1c,
                  f32((u16)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x24));
                pa_mDoExt_btpAnm_entry(p((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x20),
                                       p((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->modelData),
                                       s32(sh(a + 0x28)));
            }
            F(out + 0xC0, sy);
            F(out + 0xC4, sz);
            F(out + 0xBC, sx);
            f32 values[12];
            for (u32 i = 0; i < 12; i++)
                values[i] = (f32)matrix.get()->m[i / 4][i % 4];
            for (u32 i = 0; i < 12; i++)
                F(out + 0xC8 + i * 4, values[i]);
            pa_mDoExt_modelUpdateDL(model, u32(0));
            if ((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x1c)
                W((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->modelData + 0x38, 0);
            node = w(node + 12);
        }
    }
    e = (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpBaseEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags | JPAEmtrStts_StopDraw;
}
static void clearBuffer(u32 dest, u32 size) {
    u32 end = dest + size;
    while (dest < end) {
        for (u32 i = 0; i < 32; i++)
            B((dest & ~31u) + i, 0);
        dest += 32;
    }
}
/* 025A50B8 */
void dPa_stripesDraw(void* callback, void* emitter) {
    u32 e = gabi::ea(emitter);
    if (!(b(e + 0x18E) & 2))
        return;
    f32 percent = f32((u32)gabi::at<pa::JPABaseEmitter>(e)->mUserData) * f(0x10051C90),
        one = (f32)l_particleOne,
        step = one / f32((u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mLength / 5 - 1);
    u32 count = (u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mLength;
    if ((u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength % 5 || count < 10 ||
        count % 5)
        return;
    u32 model = w((u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mHead);
    pa_drawParticleModel(p(model), emitter);
    f32 eps = (f32)l_particleNormalizeEpsilon, zero = (f32)l_particleZero, size = f(0x10051C94);
    for (u32 strand = 0; strand < 5; strand++) {
        u32 block = w(e + 0x30C),
            bank = strand * 2 + (u32)gabi::at<pa::ParticleStripesBuffers>(block)->mBank,
            current = block + bank * 0x254;
        clearBuffer((u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData, 0xC80);
        bank = strand * 2 + (u32)gabi::at<pa::ParticleStripesBuffers>(block)->mBank;
        current = block + bank * 0x254;
        u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mHead, ordinal = 0,
            offset = 0;
        f32 t = zero;
        while (node) {
            if (ordinal % 5 == strand) {
                u32 q = w(node), v = q + 0x8C;
                f32 width = ((size * (f32)gabi::at<pa::JPABaseParticle>(q)->mScaleX) *
                             (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x) *
                            percent;
                u32 trig =
                    0x104A44F8 + ((u16)gabi::at<pa::JPABaseParticle>(q)->mRotateAngle >> 3) * 8;
                f32 sine = f(trig), cosine = f(trig + 4), neg = -width, leftz = neg * sine,
                    leftx = neg * cosine, rightx = width * cosine, rightz = width * sine;
                f32 y = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.y,
                    x = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.x,
                    z = (f32)gabi::at<pa::JPABaseParticle>(q)->mVelocity.z,
                    py = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y,
                    px = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x,
                    pz = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z;
                if (!(norm2(x, y, z) > eps)) {
                    x = zero;
                    z = zero;
                    y = one;
                } else if (norm2(x, y, z) > eps)
                    normalize(x, y, z, one);
                f32 vx = (f32)gabi::at<pa::Vec>(v)->x, vy = (f32)gabi::at<pa::Vec>(v)->y,
                    vz = (f32)gabi::at<pa::Vec>(v)->z, cx = gabi::fmsubs(vy, z, vz * y),
                    cy = gabi::fmsubs(vz, x, vx * z), cz = gabi::fmsubs(vx, y, vy * x);
                if (!(norm2(cx, cy, cz) > eps)) {
                    cz = zero;
                    cx = one;
                    cy = zero;
                } else if (norm2(cx, cy, cz) > eps)
                    normalize(cx, cy, cz, one);
                f32 nx = gabi::fmsubs(y, cz, z * cy), ny = gabi::fmsubs(z, cx, x * cz),
                    nz = gabi::fmsubs(x, cy, y * cx);
                gabi::at<pa::Vec>(v)->y = ny;
                gabi::at<pa::Vec>(v)->z = nz;
                gabi::at<pa::Vec>(v)->x = nx;
                if (norm2(nx, ny, nz) > eps) {
                    f32 inv = one / f32(pa_sqrtHD(norm2(nx, ny, nz)));
                    nx = (f32)gabi::at<pa::Vec>(v)->x * inv;
                    ny = (f32)gabi::at<pa::Vec>(v)->y * inv;
                    gabi::at<pa::Vec>(v)->x = nx;
                    nz = (f32)gabi::at<pa::Vec>(v)->z * inv;
                    gabi::at<pa::Vec>(v)->y = ny;
                    gabi::at<pa::Vec>(v)->z = nz;
                }
                nx = (f32)gabi::at<pa::Vec>(v)->x;
                ny = (f32)gabi::at<pa::Vec>(v)->y;
                nz = (f32)gabi::at<pa::Vec>(v)->z;
                f32 ly = gabi::fmadds(leftz, ny, gabi::fmadds(leftx, cy, zero * y)),
                    lx = gabi::fmadds(leftz, nx, gabi::fmadds(leftx, cx, zero * x)),
                    lz = gabi::fmadds(leftz, nz, gabi::fmadds(leftx, cz, zero * z));
                u32 dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + offset;
                F(dest + 4, ly + py);
                F(dest, lx + px);
                F(dest + 8, lz + pz);
                dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + offset;
                F(dest + 12, zero);
                F(dest + 16, t);
                f32 rx = gabi::fmadds(rightz, nx, gabi::fmadds(rightx, cx, zero * x)),
                    ry = gabi::fmadds(rightz, ny, gabi::fmadds(rightx, cy, zero * y)),
                    rz = gabi::fmadds(rightz, nz, gabi::fmadds(rightx, cz, zero * z));
                dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + offset;
                F(dest + 20, rx + px);
                F(dest + 24, ry + py);
                F(dest + 28, rz + pz);
                dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + offset;
                offset += 40;
                F(dest + 36, t);
                t += step;
                F(dest + 32, one);
            }
            node = w(node + 12);
            ordinal = u16(ordinal + 1);
        }
    }
    u32 block = w(e + 0x30C),
        current = block + (u32)gabi::at<pa::ParticleStripesBuffers>(block)->mBank * 0x254 + 4;
    for (u32 i = 0; i < 5; i++) {
        pa_flushVertexBuffer(p(current), u32(0),
                             (u32)gabi::at<pa::ParticleNativeVertexBuffer>(current)->mSize);
        current += 0x4A8;
    }
    gabi::at<pa::ParticleStripesBuffers>(block)->mBank =
        (u32)gabi::at<pa::ParticleStripesBuffers>(block)->mBank == 0 ? 1 : 0;
    pa_loadMaterial(p(w(model + 0xEC)));
}
// Preserve the reference expression's NaN operand order for the rotated corners.
static f32 rippleFusedAdd(f32 a, f32 c, f32 b) {
    if (b != b)
        return gabi::ppc_qnan(b);
    if (a != a)
        return gabi::ppc_qnan(a);
    if (c != c)
        return gabi::ppc_qnan(c);
    return gabi::fmadds(a, c, b);
}
static f32 rippleFusedSub(f32 a, f32 c, f32 b) {
    if (a != a)
        return gabi::ppc_qnan(a);
    return gabi::fmsubs(a, c, b);
}
/* 025A6C3C */
void dPa_rippleParticleDraw(void* callback, void* emitter, void* particle) {
    u32 q = gabi::ea(particle);
    f32 x = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x,
        z = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z,
        y = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y,
        width = (f32)gabi::at<pa::JPABaseParticle>(q)->hdDrawWidth,
        height = (f32)gabi::at<pa::JPABaseParticle>(q)->hdDrawHeight;
    u32 trig = 0x104A44F8 + ((u16)gabi::at<pa::JPABaseParticle>(q)->mRotateAngle >> 3) * 8;
    f32 sine = f(trig), cosine = f(trig + 4), xs[4], zs[4];
    xs[0] = gabi::fmsubs(cosine, -width, gabi::fmuls_ppc(sine, height));
    zs[0] = rippleFusedAdd(cosine, height, gabi::fmuls_ppc(sine, -width));
    xs[1] = gabi::fmsubs(cosine, width, gabi::fmuls_ppc(sine, height));
    zs[1] = rippleFusedAdd(cosine, height, gabi::fmuls_ppc(sine, width));
    xs[2] = rippleFusedSub(cosine, width, gabi::fmuls_ppc(sine, -height));
    zs[2] = rippleFusedAdd(cosine, -height, gabi::fmuls_ppc(sine, width));
    xs[3] = rippleFusedSub(cosine, -width, gabi::fmuls_ppc(sine, -height));
    zs[3] = rippleFusedAdd(cosine, -height, gabi::fmuls_ppc(sine, -width));
    u32 check = 0x1047B178;
    if (!w(0x1047B1E0)) {
        W(0x1047B1E0, 1);
        pa_dBgS_WtrChk_ctor(p(check));
        pa_registerGlobalObject(p(0x101EA444));
    }
    f32 floor = y - f(0x10051CB0), offset = f(0x10051CAC), zero = (f32)l_particleZero;
    F(check + 0x3C, floor);
    F(check + 0x38, x);
    F(check + 0x40, z);
    F(check + 0x44, floor + f(0x10051CB4));
    bool each = false;
    if ((u8)l_particleStatus & 1) {
        if (pa_daSea_ChkArea(x, z)) {
            u32 bg = gabi::ea(pa_getGameInfo());
            s32 hit = pa_dBgS_SplGrpChk(p(bg + 0x12A0), p(check));
            if (!hit || !(f(check + 0x48) > zero))
                each = true;
        }
    }
    gabi::Local<Vec> pos;
    gabi::Local<f32> water;
    u32 v = gabi::ea(pos.get());
    f32 heights[4];
    if (each) {
        for (u32 i = 0; i < 4; i++) {
            gabi::at<pa::Vec>(v)->y = y;
            gabi::at<pa::Vec>(v)->x = xs[i] + x;
            gabi::at<pa::Vec>(v)->z = zs[i] + z;
            s32 found = pa_fopAcM_getWaterY(pos.get(), water.get());
            heights[i] = (found ? f(gabi::ea(water.get())) : y) + offset;
        }
    } else {
        gabi::at<pa::Vec>(v)->x = x;
        gabi::at<pa::Vec>(v)->y = y;
        gabi::at<pa::Vec>(v)->z = z;
        if (pa_fopAcM_getWaterY(pos.get(), water.get()))
            y = f(gabi::ea(water.get()));
        for (u32 i = 0; i < 4; i++)
            heights[i] = y + offset;
    }
    u32 block = (u32)gabi::at<pa::JPABaseParticle>(q)->hdRippleBuffer,
        bank = (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank;
    clearBuffer((u32)gabi::at<pa::ParticleRenderBuffer>(block + bank * 0x254)->mData, 0x40);
    bank = (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank;
    u32 dest = (u32)gabi::at<pa::ParticleRenderBuffer>(block + bank * 0x254)->mData;
    f32 one = (f32)l_particleOne;
    for (u32 i = 0; i < 4; i++) {
        F(dest + i * 20, i == 1 ? gabi::fadds_ppc(x, xs[i]) : gabi::fadds_ppc(xs[i], x));
        F(dest + i * 20 + 4, heights[i]);
        F(dest + i * 20 + 8, zs[i] + z);
        F(dest + i * 20 + 12, (i == 1 || i == 2) ? one : zero);
        F(dest + i * 20 + 16, i >= 2 ? one : zero);
    }
    pa_selectParticleDrawMode(particle, u32(0));
    pa_prepareRippleParticleDraw(particle, u32(0), u32(0x1F4), u32(1));
}
/* WWHD glue
*/
