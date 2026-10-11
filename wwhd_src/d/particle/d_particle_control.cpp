/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
/* 025A7674 */
s32 pa::dPa_control_c::newSimple(u32 id, u32 group) {
    u32 a = gabi::ea(this);
    u8 n = (u8)this->mNumSimple;
    if (n >= 25) {
        pa_OSReport(p(0x10051D00));
        return 0;
    }
    u32 manager = (u32)l_particleEmitterManager;
    void* result = pa_simpleCreate(p(a + 0x148 + u32(n) * 0x290), p(manager), id, group);
    if (!result)
        return 0;
    this->mNumSimple = (u8)this->mNumSimple + 1;
    return 1;
}
/* 025A7D8C */
s32 pa::dPa_control_c::readScene(u32 room, void* request) {
    if (room == 0xFF)
        return 0;

    u8 count = (u8)this->mCount;
    this->mCount = count + 1;
    if (count) {
        if (room == (u8)this->mSceneNo)
            pa_JUTAssertion_showAssert(p(0x10051EA0), u32(0x7B9), p(0x10051EB0));
    } else
        this->mSceneNo = room;
    return 1;
}
/* 025A7E18 */
void pa::dPa_control_c::createScene(void* data) {
    u32 a = gabi::ea(this);
    s32 i = 0;
    u32 item = a + 0x148;
    while (i < (u8)this->mNumSimpleCommon) {
        u32 manager = (u32)l_particleEmitterManager;
        pa_simpleCreateEmitter(p(item), p(manager));
        ++i;
        item += 0x290;
    }
    if (data)
        pa_createRoomScene(this, data);
    else
        this->mSceneHeap = 0;
}
/* 025A8148 */
void pa::dPa_control_c::calc2D() {
    for (u32 group = dPtclGroup_2Dfore_e; group < dPtclGroup_2DmenuFore_e; group++) {
        u32 manager = (u32)l_particleEmitterManager;
        pa_calcEmitterGroup(p(manager), group);
    }
}
/* 025A81A0 */
void pa::dPa_control_c::calc3D() {
    for (u32 group = dPtclGroup_Normal_e; group < dPtclGroup_2Dfore_e; group++) {
        u32 manager = (u32)l_particleEmitterManager;
        pa_calcEmitterGroup(p(manager), group);
    }
}
/* 025A81F8 */
void pa::dPa_control_c::calcMenu() {
    for (u32 group = dPtclGroup_2DmenuFore_e; group < dPtclGroup_Count_e; group++) {
        u32 manager = (u32)l_particleEmitterManager;
        pa_calcEmitterGroup(p(manager), group);
    }
}
/* 025A8250 */
void pa::dPa_control_c::runSubstep(void* info) {
    if ((u32)l_particleEmitterManager)
        for (u32 group = dPtclGroup_Normal_e; group < dPtclGroup_2Dfore_e; group++) {
            u32 manager = (u32)l_particleEmitterManager;
            pa_JPAEmitterManager_substep(p(manager), info, u8(group));
        }
}
/* 025A82C4 */
void pa::dPa_control_c::runSubstepAfter(void* info) {
    if ((u32)l_particleEmitterManager)
        for (u32 group = dPtclGroup_Normal_e; group < dPtclGroup_2Dfore_e; group++) {
            u32 manager = (u32)l_particleEmitterManager;
            pa_JPAEmitterManager_substepAfter(p(manager), info, u8(group));
        }
}
/* 025A8338 */
void pa::dPa_control_c::runDrawSubstep(void* info) {
    if ((u32)l_particleEmitterManager)
        for (u32 group = dPtclGroup_Normal_e; group < dPtclGroup_2Dfore_e; group++) {
            u32 manager = (u32)l_particleEmitterManager;
            pa_JPAEmitterManager_drawSubstep(p(manager), info, u8(group));
        }
}
/* 025A83AC */
void pa::dPa_control_c::draw(void* info, u32 group) {
    u32 manager;
    if (group == dPtclGroup_2DmenuBack_e) {
        if ((u8)this->mMenuDrawEnabled)
            pa_JPAEmitterManager_prepareMenuDraw(p((u32)l_particleEmitterManager));
        u32 effect = w(0x1018F450);
        if (effect) {
            pa_prepareParticleMenuEffect(p(effect));
            manager = (u32)l_particleEmitterManager;
        } else
            manager = (u32)l_particleEmitterManager;
    } else {
        if (group == dPtclGroup_Normal_e) {
            u32 effect = w(0x1018F450);
            if (effect) {
                pa_prepareParticleWorldEffect(p(effect));
                pa_applyParticleWorldEffect(p(w(0x1018F450)));
            }
        }
        manager = (u32)l_particleEmitterManager;
    }
    if (manager) {
        if (group < dPtclGroup_2Dfore_e) {
            pa_dKy_setLight_again();
            manager = (u32)l_particleEmitterManager;
        }
        pa_JPAEmitterManager_draw(p(manager), info, group);
    }
}
/* 025A8770 */
s32 pa::dPa_control_c::checkAtrCodeEffect(s32 code) {
    if (code == 19)
        return 35;
    if (code == 4)
        return 36;
    if (code == 27 || code == 15 || code == 5 || code == 10)
        return -1;
    return 0x2022;
}
/* 025A8D10 */
void* pa::dPa_control_c::getSimple(u32 id) {
    u32 a = gabi::ea(this), n = (u8)this->mNumSimple, item = a + 0x148;
    for (u32 i = 0; i < n; i++, item += 0x290)
        if (id == h(item + 8))
            return p(item);
    return nullptr;
}
/* 025A8D40 */
s32 pa::dPa_control_c::setSimple(u32 id, void* pos, u32 alpha, void* primary, void* environment,
                                 s32 flags) {
    void* callback = pa_getSimple(this, id);
    if (!callback)
        return 0;
    return pa_simpleSet(callback, pos, alpha, primary, environment, flags);
}
/* 025A866C */
void* pa::dPa_control_c::setBombSmoke(u32 id, void* pos, void* rot, void* scale, u32 alpha) {
    void* e = pa_setEmitter(this, u32(3), id, pos, rot, scale, alpha, p(0x1047B2EC), s32(-1),
                            nullptr, nullptr, nullptr);
    if (e)
        W(gabi::ea(e) + 0x258, id == 0x2009 ? 0x00460501 : 0x00280478);
    return e;
}
/* 025A86F4 */
void* pa::dPa_control_c::setNormalStripes(u32 id, void* pos, void* rot, void* scale, u32 alpha,
                                          u32 work) {
    void* e = pa_setEmitter(this, u32(0), id, pos, rot, scale, alpha, p(0x1047B2D8), s32(-1),
                            nullptr, nullptr, nullptr);
    if (e)
        W(gabi::ea(e) + 0x258, work);
    return e;
}
/* 025A847C */
void* pa::dPa_control_c::set(u32 group, u32 id, void* pos, void* angle, void* scale, u32 alpha,
                             void* callback) {
    u32 sp = gabi::cpu->r[1];
    s32 info = s8(b(sp + 11));
    void* primary = p(w(sp + 12));
    void* environment = p(w(sp + 16));
    void* globalScale = p(w(sp + 20));
    u32 v = gabi::ea(pos);
    gabi::Local<Vec> localPos;
    u32 lp = gabi::ea(localPos.get());
    f32 y = (f32)gabi::at<pa::Vec>(v)->y, x = (f32)gabi::at<pa::Vec>(v)->x,
        z = (f32)gabi::at<pa::Vec>(v)->z;
    F(lp + 4, y);
    F(lp, x);
    F(lp + 8, z);
    u32 resource = pa_getRM_ID(id);
    u32 manager = (u32)l_particleEmitterManager;
    void* result = pa_JPAEmitterManager_createSimpleEmitterID(p(manager), localPos.get(), id, group,
                                                              resource, u32(0), u32(0));
    if (!result)
        return result;
    u32 e = gabi::ea(result);
    if (angle) {
        u32 a = gabi::ea(angle);
        s16 ry = sh(a + 2), rx = sh(a), rz = sh(a + 4);
        pa_JPAGetXYZRotateMtx(rx, ry, rz, p(e + 0x1F0));
    }
    if (scale) {
        u32 s = gabi::ea(scale);
        f32 sz = f(s + 8), sx = f(s), sy = f(s + 4);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.x = sx;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z = sz;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.y = sy;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.z = sz;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y = sy;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x = sx;
    }
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = alpha;
    if (callback) {
        u32 cb = gabi::ea(callback);
        gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = cb;
        u32 fn = w(w(cb) + 0x3C);
        gabi::call_ptr<void>(fn, callback, result, pos, angle, s32(s8(info)));
    } else if (id & 0x4000) {
        pa_setEmitterDrawFlag(result, u32(1));
        gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0x1047B2DC;
    }
    if (primary) {
        u32 c = gabi::ea(primary);
        u8 green = b(c + 1), blue = b(c + 2), red = b(c);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = blue;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
    }
    if (environment) {
        u32 c = gabi::ea(environment);
        u8 green = b(c + 1), blue = b(c + 2), red = b(c);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.b = blue;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.g = green;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.r = red;
    }
    if (pa_effectNeedsSpecialWind(id)) {
        gabi::Local<Color> sea, foam;
        pa_dKy_get_seacolor(sea.get(), foam.get());
        u32 c = gabi::ea(sea.get());
        u8 green = b(c + 1), red = b(c), blue = b(c + 2);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = blue;
        red = b(c);
        green = b(c + 1);
        blue = b(c + 2);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.g = green;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.r = red;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.b = blue;
    }
    if (globalScale) {
        u32 s = gabi::ea(globalScale);
        f32 sx = f(s), sz = f(s + 8), sy = f(s + 4);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z = sz;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x = sx;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y = sy;
    }
    return result;
}
/* 025A8BFC */
void* pa::dPa_control_c::setSimpleLandPoly(void* poly, void* pos, void* angle, void* tev,
                                           void* returnId, u32 flags, f32 sx, f32 sy, f32 sz) {
    u32 play = gabi::ea(pa_getGameInfo());
    if (!pa_cBgS_ChkPolySafe(p(play + 0x12A0), poly))
        return nullptr;
    play = gabi::ea(pa_getGameInfo());
    s32 code = pa_dBgS_GetAttributeCode(p(play + 0x12A0), poly);
    return pa_setSimpleLand(this, code, pos, angle, tev, returnId, flags, sx, sy, sz);
}
struct DrawInfo {
    u8 bytes[80];
};
static void cameraSubstep(u32 address) {
    u32 sys = w(0x101F95D0), count = w(sys + 0x1020), table = w(sys + 0x1024);
    if (count > 1)
        table += 4;
    u32 view = w(table), flags = w(view + 0x50), matrix;
    if ((flags & 0x40) && (flags & 0x80))
        matrix = view + 0x84;
    else {
        matrix = w(view + 0x48);
        if (!matrix)
            matrix = 0x104A2098;
    }
    u32 camera;
    if (flags & 0x1000)
        camera = w(view + 0x164);
    else {
        camera = w(view + 0x4C);
        if (!camera)
            camera = 0x104A20FC;
    }
    if (!matrix || !camera)
        return;
    if (!w(0x101FD9F0)) {
        W(0x101FD9F0, 1);
        W(0x101FDCD4, 0x10051BEC);
    }
    u32 fn = w(w(camera + 0x90) + 12);
    if (!gabi::call_ptr<s32>(fn, p(camera), p(0x101FDCD4)))
        return;
    gabi::Local<DrawInfo> info;
    u32 a = gabi::ea(info.get());
    W(a + 76, 0);
    F(a + 68, f(0x10051F10));
    F(a + 72, f(0x10051F14));
    W(a, matrix);
    F(a + 68, f(camera + 0xC0));
    F(a + 72, f(camera + 0xAC));
    void* source = pa_getParticleCameraMatrix(p(camera));
    pa_MTXCopy(source, p(a + 4));
    u32 play = gabi::ea(pa_getGameInfo());
    if (w(play + 0x5AB0)) {
        play = gabi::ea(pa_getGameInfo());
        gabi::call<void>(address, p(w(play + 0x5AB0)), info.get());
    }
}
/* 025A8D8C */
void dPa_cameraSubstepAfter() { cameraSubstep(0x025A82C4); }
/* 025A8F08 */
void dPa_cameraDrawSubstep() { cameraSubstep(0x025A8338); }
/* 025A87C0 */
void* pa::dPa_control_c::setSimpleLand(s32 code, void* pos, void* angle, void* tev, void* out,
                                       u32 flags, f32 landScale, f32 waterScale, f32 foamScale) {
    u32 o = gabi::ea(out), t = gabi::ea(tev);
    if (!tev || !out)
        pa_JUTAssertion_showAssert(p(0x10051F00), u32(0x9BE), p(0x10051ED4));
    s32 effect = pa_checkAtrCodeEffect(this, code);
    W(o, effect);
    if (effect == -1)
        return nullptr;
    if (code == -1) {
        struct Ground {
            u8 bytes[84];
        };
        gabi::Local<Ground> ground;
        u32 g = gabi::ea(ground.get()), v = gabi::ea(pos);
        pa_cBgS_GndChk_ctor(ground.get());
        f32 z = (f32)gabi::at<pa::Vec>(v)->z, x = (f32)gabi::at<pa::Vec>(v)->x,
            y = (f32)gabi::at<pa::Vec>(v)->y;
        W(g, g + 64);
        W(g + 4, g + 76);
        W(g + 64, 0x10051B0C);
        B(g + 70, 0);
        B(g + 69, 0);
        W(g + 16, 0x10051ADC);
        B(g + 72, 0);
        W(g + 76, 0x10051AFC);
        W(g + 32, 0x10051AEC);
        B(g + 73, 0);
        W(g + 80, 1);
        F(g + 44, z);
        F(g + 40, y + f(0x10051EC4));
        B(g + 68, 1);
        B(g + 74, 0);
        F(g + 36, x);
        B(g + 71, 0);
        u32 bg = gabi::ea(pa_getGameInfo());
        f32 height = pa_cBgS_GroundCross(p(bg + 0x12A0), ground.get());
        if (height != f(0x10051EC8)) {
            bg = gabi::ea(pa_getGameInfo());
            pa_dBgS_GetAttributeCode(p(bg + 0x12A0), p(g + 20));
        }
        W(g + 32, 0x10051AAC);
        W(g + 64, 0x10051ACC);
        W(g + 76, 0x10051A8C);
        pa_cBgS_Chk_dtor(ground.get(), u32(0));
        effect = s32(w(o));
    }
    gabi::Local<Vec> scale;
    u32 s = gabi::ea(scale.get());
    void* emitter = nullptr;
    u32 group = dPtclGroup_Normal_e, id = u32(effect) & 0xFFFF, alpha = 255, callback = 0,
        primary = 0, env = 0;
    gabi::Local<Color> c1, c2;
    bool spawn = false;
    if (effect == 0x23) {
        if (flags & 4) {
            F(s, foamScale);
            F(s + 4, foamScale);
            F(s + 8, foamScale);
            pa_dKy_get_seacolor(c1.get(), c2.get());
            id = h(o + 2);
            primary = gabi::ea(c1.get());
            group = (flags & 16) ? dPtclGroup_NormalP1_e : dPtclGroup_Normal_e;
            spawn = true;
        }
    } else if (effect == 0x24) {
        if (flags & 2) {
            u32 c = gabi::ea(c1.get());
            B(c + 2, sh(t + 0x94));
            B(c + 3, sh(t + 0x96));
            B(c + 1, sh(t + 0x92));
            B(c, sh(t + 0x90));
            F(s, waterScale);
            F(s + 4, waterScale);
            F(s + 8, waterScale);
            id = 0x24;
            primary = c;
            env = t + 0x98;
            group = (flags & 16) ? dPtclGroup_NormalP1_e : dPtclGroup_Normal_e;
            spawn = true;
        }
    } else if (effect == 0x2022) {
        if (flags & 8) {
            effect = 0x2027;
            W(o, effect);
        }
        if (flags & 9) {
            F(s, landScale);
            F(s + 4, landScale);
            F(s + 8, landScale);
            id = u32(effect) & 0xFFFF;
            group = (flags & 16) ? dPtclGroup_ToonP1_e : dPtclGroup_Toon_e;
            alpha = 128;
            callback = 0x1047B284;
            spawn = true;
        }
    }
    if (spawn)
        emitter = pa_setEmitter(this, group, id, pos, angle, scale.get(), alpha, p(callback),
                                s32(-1), p(primary), p(env), p(0));
    if (emitter) {
        u32 e = gabi::ea(emitter);
        gabi::at<pa::JPABaseEmitter>(e)->mRate = f(0x10051ECC);
        gabi::at<pa::JPABaseEmitter>(e)->mSpread = f(0x10051ED0);
        gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = 1;
    }
    return emitter;
}
/* WWHD glue
*/
