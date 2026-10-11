/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
struct ShaderState {
    u8 bytes[0x11C];
};
static void fillConfig(ParticleShaderConfig* config, u32 name, bool mode = false) {
    config->mName.mString = name;
    config->mName.__vtbl = 0x10051A34;
    f32 one = (f32)l_particleOne;
    config->mScaleZ = one;
    config->mScaleY = one;
    config->mScaleW = one;
    config->mScaleX = one;
    config->mEnableX = 1;
    config->mEnableY = mode;
    config->mEnableZ = 0;
    config->mEnableW = 0;
}

static void removeEmitter(void* callback, u32 off) {
    u32 a = gabi::ea(callback), e = w(a + off);
    if (!e)
        return;
    gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0;
    e = w(a + off);
    u32 flags = (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags;
    gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = ~u32(0);
    gabi::at<pa::JPABaseEmitter>(e)->mFlags = flags | JPAEmtrStts_StopEmit;
    W(a + off, 0);
}
static void disableFog() {
    if (!((u8)l_particleStatus & 1))
        return;
    gabi::Local<ShaderState> state;
    u32 a = gabi::ea(state.get());
    pa_captureParticleShaderState(state.get());
    u32 flags = w(a + 12);
    W(a + 4, 0);
    B(a + 1, 0);
    W(a + 12, flags | 2);
    B(a, 0);
    pa_applyParticleShaderState(state.get());
}
/* 025A7220 */
void dPa_singleRippleDraw(void* callback, void* emitter) { disableFog(); }
/* 025A7278 */
void dPa_singleRippleSetup(void* callback, void* emitter, void* pos, void* angle, u32 info) {
    gabi::Local<ParticleShaderConfig> config;
    W(gabi::ea(emitter) + 0x1E8, 0x1047B2E8);
    fillConfig(config.get(), 0x10051CB8);
    pa_setParticleShaderConfig(emitter, config.get());
}
/* 025A9084 */
void* dPa_rippleCtor(void* obj) {
    obj = allocate(obj, 20);
    if (obj) {
        pa::dPa_rippleEcallBack* self = gabi::at<pa::dPa_rippleEcallBack>(gabi::ea(obj));
        f32 zero = (f32)l_particleZero;
        self->__vtbl = gabi::ea(l_particleRippleVtable);
        self->mPos = 0;
        self->mFlags = 0;
        self->mRate = zero;
        self->mpBaseEmitter = 0;
    }
    return obj;
}
/* 025A90E0 */
void pa::dPa_rippleEcallBack::execute(void* emitter) {
    u32 e = gabi::ea(emitter), pos = (u32)this->mPos;
    u8 group = (u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID;
    f32 y = (f32)gabi::at<pa::Vec>(pos)->y, z = (f32)gabi::at<pa::Vec>(pos)->z,
        x = (f32)gabi::at<pa::Vec>(pos)->x;
    if (group >= dPa_control_c::dPtclGroup_2Dfore_e)
        y = -y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
    f32 scale = (f32)this->mRate, slope = f(0x10051F18), bias = f(0x10051F1C);
    gabi::at<pa::JPABaseEmitter>(e)->mRate = gabi::fmadds(slope, scale, bias);
    if ((u32)this->mFlags & JPAEmtrStts_StopEmit) {
        gabi::Local<Color> sea, foam;
        pa_dKy_get_seacolor(sea.get(), foam.get());
        u32 s = gabi::ea(sea.get());
        u8 blue = b(s + 2), green = b(s + 1);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = blue;
        u8 red = b(s);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
    }
}
/* 025A9180 */
void pa::dPa_rippleEcallBack::draw(void* emitter) { disableFog(); }
/* 025A91D8 */
void pa::dPa_rippleEcallBack::setup(void* emitter, void* pos, void* angle, u32 info) {
    u32 e = gabi::ea(emitter);
    gabi::at<pa::JPABaseEmitter>(e)->mpParticleCallBack = 0x1047B2E8;
    u32 flags = (u32)this->mFlags;
    this->mpBaseEmitter = e;
    this->mRate = (f32)l_particleOne;
    this->mPos = gabi::ea(pos);
    this->mFlags = flags | JPAEmtrStts_StopEmit;
    gabi::Local<ParticleShaderConfig> config;
    fillConfig(config.get(), 0x10051F20);
    pa_setParticleShaderConfig(emitter, config.get());
}
/* 025A9270 */
void pa::dPa_rippleEcallBack::end() {
    u32 e = (u32)this->mpBaseEmitter;
    if (!e)
        return;
    u32 flags = (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags;
    gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = ~u32(0);
    gabi::at<pa::JPABaseEmitter>(e)->mFlags = flags | JPAEmtrStts_StopEmit;
    e = (u32)this->mpBaseEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & ~u32(0x40);
    e = (u32)this->mpBaseEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0;
    u32 cbflags = (u32)this->mFlags;
    this->mpBaseEmitter = 0;
    this->mFlags = cbflags | JPAEmtrStts_StopCalc;
}
/* 025A92C0 */
void pa::dPa_waveEcallBack::remove() { removeEmitter(this, 0x60); }
/* 025A9904 */
void pa::dPa_waveEcallBack::setup(void* emitter, void* pos, void* angle, u32 info) {
    this->mpBaseEmitter = gabi::ea(emitter);
    this->mFadeTimer = 0;
    this->mVel = (f32)l_particleZero;
    this->mpPos = gabi::ea(pos);
    this->mState = 0;
    f32 one = (f32)l_particleOne;
    this->mVelFade2 = one;
    this->mMaxParticleVelocity = f(0x10051F34);
    this->mpRot = gabi::ea(angle);
    gabi::Local<ParticleShaderConfig> config;
    fillConfig(config.get(), 0x10051F38);
    pa_setParticleShaderConfig(emitter, config.get());
    pa_releaseTrackEmitterRendering(p((u32)this->mpBaseEmitter));
}
/* 025A99B8 */
void pa::dPa_splashEcallBack::remove() { removeEmitter(this, 24); }
/* 025A9BA0 */
void pa::dPa_splashEcallBack::setup(void* emitter, void* pos, void* angle, u32 info) {
    this->mpPos = gabi::ea(pos);
    this->mpBaseEmitter = gabi::ea(emitter);
    this->mpRot = gabi::ea(angle);
    this->mState = 0;
    gabi::Local<ParticleShaderConfig> config;
    fillConfig(config.get(), 0x10051F50);
    pa_setParticleShaderConfig(emitter, config.get());
}
/* 025A9C20 */
void pa::dPa_cutTurnEcallBack_c::setup(void* emitter, void* pos, void* angle, u32 info) {
    this->mpBaseEmitter = gabi::ea(emitter);
    this->field_0x6 = 0;
    this->mAlpha = 255;
    this->field_0x5 = 0;
}
/* 025A9D64 */
void pa::dPa_cutTurnEcallBack_c::end() {
    if (!(u32)this->mpBaseEmitter)
        return;
    removeEmitter(this, 12);
    this->field_0x5 = 1;
}
/* 025A9DA0 */
void pa::dPa_trackEcallBack::getMaxWaterY(void* pos) {
    u32 v = gabi::ea(pos);
    f32 x = (f32)gabi::at<pa::Vec>(v)->x, z = (f32)gabi::at<pa::Vec>(v)->z, height;
    if (pa_daSea_ChkArea(x, z)) {
        x = (f32)gabi::at<pa::Vec>(v)->x;
        z = (f32)gabi::at<pa::Vec>(v)->z;
        height = pa_daSea_calcWave(x, z);
        gabi::at<pa::Vec>(v)->y = height;
        f32 bound = (f32)this->mMinY;
        if (bound > height)
            height = bound;
    } else {
        f32 bound = (f32)this->mMinY;
        height = bound == f(0x10051EC8) ? (f32)this->mBaseY : bound;
    }
    gabi::at<pa::Vec>(v)->y = gabi::fadds_ppc(height, f(0x10051CAC));
}
/* 025A9E38 */
void pa::dPa_trackEcallBack::remove() { removeEmitter(this, 0x4C); }
/* 025AA08C */
void dPa_copyMatrix(void* out, void* matrix) {
    u32 o = gabi::ea(out), m = gabi::ea(matrix);
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = (f32)gabi::at<ParticleMatrix>(m)->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; i++)
        gabi::at<ParticleMatrix>(o)->m[i / 4][i % 4] = values[i];
}
/* 025AA770 */
void pa::dPa_trackEcallBack::setup(void* emitter, void* pos, void* angle, u32 info) {
    this->mpRot = gabi::ea(angle);
    this->mpPos = gabi::ea(pos);
    this->mState = 0;
    this->mpBaseEmitter = gabi::ea(emitter);
    pa_prepareWaveEmitterRendering(emitter);
}
/* 025A97A8 */
void pa::dPa_waveEcallBack::drawBefore(void* emitter) {
    u32 e = gabi::ea(emitter),
        count = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength;
    if (count < 2)
        return;
    u32 model = w((u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead);
    gabi::Local<Color> sea, foam;
    gabi::Local<ColorF> first, second;
    pa_dKy_get_seacolor(sea.get(), foam.get());
    B(gabi::ea(sea.get()) + 3, 255);
    B(gabi::ea(foam.get()) + 3, 255);
    pa_colorToFloat(second.get(), sea.get());
    u32 light = gabi::ea(pa_getEnvlight());
    pa_blendParticleColors(first.get(), second.get(), f(light + 0x10B8));
    pa_setMaterialColor(p(w(model + 0xEC)), first.get(), u32(0));
    pa_colorToFloat(first.get(), foam.get());
    light = gabi::ea(pa_getEnvlight());
    pa_blendParticleColors(second.get(), first.get(), f(light + 0x10BC));
    pa_setMaterialColor(p(w(model + 0xEC)), second.get(), u32(1));
    pa_loadMaterial(p(w(model + 0xEC)));
    u32 material = w(model + 0xEC), target = w(w(material + 12) + 0x2C),
        resource = w((u32)this->mpBaseEmitter + 0x26C);
    gabi::call_ptr<void>(target, p(material), p(resource));
    pa_loadShape(p(w(model + 0xF0)), p(w((u32)this->mpBaseEmitter + 0x26C)));
    for (u32 i = 0; i < 2; i++) {
        u32 block = w((u32)this->mpBaseEmitter + 0x308),
            bank = i * 2 + ((u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank == 0 ? 1 : 0);
        pa_bindVertexBuffer(p(block + bank * 0x254 + 0x158));
        u32 buffers = w(model + 0x100), vertex = w(buffers), index = w(buffers + 4),
            indices = w(buffers + 8);
        pa_drawIndexed(p(index), count, p(vertex), indices, u32(0), u32(1));
    }
    B(model + 0x120, 1);
}
/* 025A99EC */
void pa::dPa_splashEcallBack::execute(void* emitter) {
    u32 e = gabi::ea(emitter);
    gabi::Local<Color> sea, foam;
    pa_dKy_get_seacolor(sea.get(), foam.get());
    u32 c = gabi::ea(sea.get());
    u8 blue = b(c + 2), red = b(c), green = b(c + 1);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = blue;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
    f32 factor = f(0x10051F4C), scale;
    if ((s16)this->mState) {
        gabi::Local<Vec> v;
        u32 t = gabi::ea(v.get());
        f32 y = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y,
            z = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z,
            x = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x;
        F(t + 8, z);
        F(t, x);
        F(t + 4, y);
        if (pa_cLib_chaseF(v.get(), (f32)l_particleZero, f(0x10051D34))) {
            pa_splashRemove(this);
            return;
        }
        scale = f(t);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.y = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.z = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.x = scale;
        gabi::at<pa::JPABaseEmitter>(e)->mInitialVelDir = factor * scale;
        return;
    }
    u32 pos = (u32)this->mpPos;
    f32 x = (f32)gabi::at<pa::Vec>(pos)->x, y = (f32)gabi::at<pa::Vec>(pos)->y,
        z = (f32)gabi::at<pa::Vec>(pos)->z;
    if ((u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID >= 7)
        y = -y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
    scale = (f32)this->mScaleTimer / (f32)this->mMaxScaleTimer;
    f32 one = (f32)l_particleOne;
    s32 angle = sh((u32)this->mpRot + 2);
    if (scale > one)
        scale = one;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.y = scale;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.x = scale;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.y = scale;
    gabi::at<pa::JPABaseEmitter>(e)->mInitialVelDir = factor * scale;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalParticleScale.z = scale;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.z = scale;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalDynamicsScale.x = scale;
    pa_JPAGetXYZRotateMtx(u32(0), angle, u32(0), p(e + 0x1F0));
}
/* 025A9E6C */
void pa::dPa_trackEcallBack::execute(void* emitter) {
    u32 e = gabi::ea(emitter);
    gabi::Local<Color> sea, foam;
    pa_dKy_get_seacolor(sea.get(), foam.get());
    u32 s = gabi::ea(sea.get()), t = gabi::ea(foam.get());
    u8 red = b(s), blue = b(t + 2);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
    u8 green = b(s + 1), white = b(t);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.b = blue;
    u8 sb = b(s + 2), fg = b(t + 1);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.r = white;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.g = fg;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = sb;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
    if ((s16)this->mState) {
        gabi::Local<Vec> pos;
        u32 v = gabi::ea(pos.get());
        gabi::at<pa::Vec>(v)->x = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x;
        gabi::at<pa::Vec>(v)->y = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y;
        gabi::at<pa::Vec>(v)->z = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z;
        pa_trackGetMaxWaterY(this, pos.get());
        gabi::Local<s16> alpha;
        u32 c = gabi::ea(alpha.get());
        H(c, (u8)gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a);
        pa_cLib_chaseS(alpha.get(), s32(0), s32(10));
        s16 value = sh(c);
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = value;
        if (!value) {
            pa_trackRemove(this);
            return;
        }
    } else {
        u32 pos = (u32)this->mpPos;
        f32 x = (f32)gabi::at<pa::Vec>(pos)->x, y = (f32)gabi::at<pa::Vec>(pos)->y,
            z = (f32)gabi::at<pa::Vec>(pos)->z;
        if ((u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID >= 7)
            y = -y;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
        s32 angle = sh((u32)this->mpRot + 2);
        if ((f32)this->mVel < (f32)l_particleZero)
            angle = s16(angle + 0x8000);
        pa_JPAGetXYZRotateMtx(u32(0), angle, u32(0), p(e + 0x1F0));
        f32 speed = std::fabs((f32)this->mVel), limit = (f32)this->mMinVel;
        gabi::Local<s16> alpha;
        u32 c = gabi::ea(alpha.get());
        H(c, (u8)gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a);
        pa_cLib_chaseS(alpha.get(), s32(speed > limit ? 255 : 0), s32(5));
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = b(c + 1);
    }
    u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
    while (node) {
        u32 particle = w(node);
        node = w(node + 12);
        gabi::Local<Vec> pos;
        u32 v = gabi::ea(pos.get());
        gabi::at<pa::Vec>(v)->x = f(particle + 16);
        gabi::at<pa::Vec>(v)->y = f(particle + 20);
        gabi::at<pa::Vec>(v)->z = f(particle + 24);
        pa_trackGetMaxWaterY(this, pos.get());
        F(particle + 16, (f32)gabi::at<pa::Vec>(v)->x);
        F(particle + 20, (f32)gabi::at<pa::Vec>(v)->y);
        F(particle + 24, (f32)gabi::at<pa::Vec>(v)->z);
    }
}
/* 025A92F4 */
void pa::dPa_waveEcallBack::executeAfter(void* emitter) {
    u32 e = gabi::ea(emitter);
    f32 zero = (f32)l_particleZero;
    s16 stop = (s16)this->mState;
    f32 m0 = f(0x104B5778), m1 = f(0x104B5768), m2 = f(0x104B5788);
    this->mRotMtx[0].x = m1;
    this->mRotMtx[0].y = m0;
    this->mRotMtx[0].z = m2;
    f32 n2 = f(0x104B578C), n0 = f(0x104B576C), n1 = f(0x104B577C);
    this->mRotMtx[1].x = n0;
    this->mRotMtx[1].z = n2;
    this->mRotMtx[1].y = n1;
    f32 z0 = f(0x104B5770), z1 = f(0x104B5780), z2 = f(0x104B5790);
    this->mRotMtx[2].y = z1;
    this->mRotMtx[2].x = z0;
    this->mRotMtx[2].z = z2;
    if (stop) {
        gabi::at<pa::JPABaseEmitter>(e)->mInitialVelDir = zero;
        s16 delay = (s16)this->mFadeTimer;
        if (delay > 0)
            this->mFadeTimer = delay - 1;
        else
            pa_waveRemove(this);
        return;
    }
    u32 pos = (u32)this->mpPos;
    f32 oldx = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x,
        x = (f32)gabi::at<pa::Vec>(pos)->x,
        oldy = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y,
        y = (f32)gabi::at<pa::Vec>(pos)->y,
        oldz = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z,
        z = (f32)gabi::at<pa::Vec>(pos)->z;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
    if ((u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID >= 7)
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y =
            -(f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y;
    f32 dy = y - oldy, dx = x - oldx, dz = z - oldz;
    f32 distance = f32(pa_sqrtHD(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy))));
    f32 cap = (f32)this->mMaxParticleVelocity, scale = (f32)this->mVelFade1;
    if (!(distance < cap))
        distance = cap;
    f32 speed = (scale * distance) * (f32)this->mVelFade2;
    pa_JPAGetXYZRotateMtx(u32(0), s32(sh((u32)this->mpRot + 2)), u32(0), p(e + 0x1F0));
    f32 last = (f32)this->mVel, delta = speed - last, step = (f32)this->mVelSpeed;
    if (std::fabs(delta) > step)
        speed = delta > zero ? last + step : last - step;
    gabi::at<pa::JPABaseEmitter>(e)->mInitialVelDir = speed;
    this->mVel = speed;
}
/* 025AA574 */
void pa::dPa_trackEcallBack::drawBefore(void* emitter) {
    u32 e = gabi::ea(emitter);
    if ((u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength < 6)
        return;
    u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead, count = 0;
    if (!node)
        return;
    for (u32 n = node; n; n = w(n + 12))
        if (!b(w(n) + 0x10C))
            ++count;
    if (count < 6)
        return;
    disableFog();
    node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
    f32 groups = f32(count) * f(0x10051F64), split = f(0x10051F68);
    u32 n;
    if (groups >= split)
        n = u32(gabi::ftoi(groups - split)) + 0x80000000;
    else
        n = u32(gabi::ftoi(groups));
    u32 model = w(node), mat = w(model + 0xEC), target = w(w(mat + 12) + 0x2C);
    gabi::call_ptr<void>(target, p(mat), p(w((u32)this->mpBaseEmitter + 0x26C)));
    pa_loadShape(p(w(model + 0xF0)), p(w((u32)this->mpBaseEmitter + 0x26C)));
    u32 block = w((u32)this->mpBaseEmitter + 0x310), bank = w(block + 0x2BA70) == 0 ? 1 : 0,
        remaining = n - 1;
    if (remaining) {
        u32 current = block + bank * 0x254;
        do {
            if (w(block + 0x2BA80))
                pa_bindVertexBuffer(p(current + 0x158));
            pa_drawIndexed(p(w(0x104B4A88)), u32(6), p(w(0x104B4A84)), w(0x104B4A8C), u32(0),
                           u32(1));
            current += 0x4A8;
        } while (--remaining);
    }
    B(model + 0x120, 1);
}
static void zeroBlocks(u32 dest, u32 length) {
    u32 end = dest + length;
    while (dest < end) {
        u32 aligned = dest & ~31u;
        for (u32 i = 0; i < 32; i++)
            B(aligned + i, 0);
        dest += 32;
    }
}
static f32 waveFusedAdd(f32 a, f32 c, f32 b) {
    if (a != a)
        return gabi::ppc_qnan(a);
    if (b != b)
        return gabi::ppc_qnan(b);
    if (c != c)
        return gabi::ppc_qnan(c);
    return gabi::fmadds(a, c, b);
}
static f32 waveInnerAdd(f32 a, f32 c, f32 b) {
    if (b != b)
        return gabi::ppc_qnan(b);
    if (a != a)
        return gabi::ppc_qnan(a);
    if (c != c)
        return gabi::ppc_qnan(c);
    return gabi::fmadds(a, c, b);
}
/* 025A9508 */
void pa::dPa_waveEcallBack::draw(void* emitter) {
    u32 a = gabi::ea(this), e = gabi::ea(emitter);
    f32 ex = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x,
        ez = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z,
        ey = (f32)gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y, one = (f32)l_particleOne;
    u32 count = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength;
    f32 stride = one / f32(count - 1);
    if (count < 2)
        return;
    u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
    pa_drawParticleModel(p(w(node)));
    f32 zero = (f32)l_particleZero, half = (f32)l_particleHalf;
    for (u32 side = 0; side < 2; side++) {
        u32 v = a + 0x1C + side * 12;
        f32 x = (f32)gabi::at<pa::Vec>(v)->x, y = (f32)gabi::at<pa::Vec>(v)->y,
            z = (f32)gabi::at<pa::Vec>(v)->z;
        f32 dx =
            waveFusedAdd((f32)this->mRotMtx[2].x, z,
                         waveInnerAdd((f32)this->mRotMtx[0].x, x, (f32)this->mRotMtx[1].x * y));
        f32 dy =
            waveFusedAdd((f32)this->mRotMtx[2].y, z,
                         waveInnerAdd((f32)this->mRotMtx[1].y, y, (f32)this->mRotMtx[0].y * x));
        f32 dz =
            waveFusedAdd((f32)this->mRotMtx[2].z, z,
                         waveInnerAdd((f32)this->mRotMtx[0].z, x, (f32)this->mRotMtx[1].z * y));
        u32 block = w((u32)this->mpBaseEmitter + 0x308),
            bank = side * 2 + (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank,
            dest = (u32)gabi::at<pa::ParticleRenderBuffer>(block + bank * 0x254)->mData;
        zeroBlocks(dest, 0x3A80);
        bank = side * 2 + (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank;
        dest = (u32)gabi::at<pa::ParticleRenderBuffer>(block + bank * 0x254)->mData;
        F(dest + 16, zero);
        F(dest + 4, gabi::fadds_ppc(ey, dy));
        F(dest + 8, gabi::fadds_ppc(ez, dz));
        F(dest, gabi::fadds_ppc(ex, dx));
        F(dest + 12, half);
        f32 t = zero;
        node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
        while (node) {
            u32 particle = w(node);
            f32 px = f(particle + 0x28), py = f(particle + 0x2C), pz = f(particle + 0x30);
            dest += 20;
            F(dest, px);
            F(dest + 12, t);
            F(dest + 4, py);
            F(dest + 8, pz);
            F(dest + 16, one);
            node = w(node + 12);
            t += stride;
        }
    }
    u32 block = w((u32)this->mpBaseEmitter + 0x308),
        cur = block + (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank * 0x254 + 4;
    for (u32 i = 0; i < 2; i++) {
        pa_flushVertexBuffer(p(cur), u32(0), w(cur + 0x14C));
        cur += 0x4A8;
    }
    gabi::at<pa::ParticleRippleBuffers>(block)->mBank =
        (u32)gabi::at<pa::ParticleRippleBuffers>(block)->mBank == 0 ? 1 : 0;
}
/* 025AA12C */
void pa::dPa_trackEcallBack::draw(void* emitter) {
    u32 a = gabi::ea(this), e = gabi::ea(emitter);
    if ((u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength < 6)
        return;
    u32 node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead, count = 0;
    if (!node)
        return;
    for (u32 n = node; n; n = w(n + 12))
        if (!b(w(n) + 0x10C))
            ++count;
    if (count < 6)
        return;
    f32 groups = f32(count) * f(0x10051F64), split = f(0x10051F68);
    u32 n =
        groups >= split ? u32(gabi::ftoi(groups - split)) + 0x80000000 : u32(gabi::ftoi(groups));
    f32 stride = (f32)l_particleOne / f32(n - 1), half = (f32)l_particleHalf;
    struct Matrix {
        u8 bytes[48];
    };
    gabi::Local<Matrix> input, output;
    u32 in = gabi::ea(input.get());
    pa_PSMTXIdentity(input.get());
    F(in + 0x1C, f(e + 0x194) * (f32)this->mExTransY);
    F(in + 0x14, (f32)this->mExScaleY);
    u32 model = w((u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead);
    pa_copyMatrix(output.get(), input.get());
    pa_setParticleMaterialMatrix(p(w(model + 0xEC)), output.get(), u32(1));
    pa_drawParticleModel(p(model), p((u32)this->mpBaseEmitter));
    node = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mHead;
    f32 zero = (f32)l_particleZero, t = zero;
    for (u32 group = 0; group < n; group++) {
        if (group == 0) {
            for (u32 i = 0; i < 3; i++) {
                u32 q = w(node), v = a + 16 + i * 12;
                gabi::at<pa::Vec>(v)->x = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x;
                gabi::at<pa::Vec>(v)->y = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y;
                gabi::at<pa::Vec>(v)->z = (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z;
                node = w(node + 12);
            }
        } else {
            u32 block = w((u32)this->mpBaseEmitter + 0x310),
                bank = (group * 2 - 2) + w(block + 0x2BA70), current = block + bank * 0x254;
            zeroBlocks((u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData, 0x60);
            bank = (group * 2 - 2) + w(block + 0x2BA70);
            current = block + bank * 0x254;
            f32 previous = t - stride, u = zero;
            u32 i = 0;
            while (i < 3) {
                u32 q = w(node);
                if (!(u8)gabi::at<pa::JPABaseParticle>(q)->hdHidden) {
                    gabi::Local<Vec> pos;
                    u32 v = gabi::ea(pos.get());
                    gabi::at<pa::Vec>(v)->x =
                        (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.x;
                    gabi::at<pa::Vec>(v)->y =
                        (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.y;
                    gabi::at<pa::Vec>(v)->z =
                        (f32)gabi::at<pa::JPABaseParticle>(q)->mGlobalPosition.z;
                    pa_trackGetMaxWaterY(this, pos.get());
                    u32 dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + i * 40;
                    F(dest, (f32)gabi::at<pa::Vec>(v)->x);
                    F(dest + 4, (f32)gabi::at<pa::Vec>(v)->y);
                    F(dest + 8, (f32)gabi::at<pa::Vec>(v)->z);
                    dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + i * 40;
                    F(dest + 12, u);
                    F(dest + 16, t);
                    u32 old = a + 16 + i * 12;
                    f32 x = f(old), z = f(old + 8), y = f(old + 4);
                    dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + i * 40;
                    F(dest + 20, x);
                    F(dest + 24, y);
                    F(dest + 28, z);
                    dest = (u32)gabi::at<pa::ParticleRenderBuffer>(current)->mData + i * 40;
                    F(dest + 36, previous);
                    F(dest + 32, u);
                    F(old, (f32)gabi::at<pa::Vec>(v)->x);
                    F(old + 4, (f32)gabi::at<pa::Vec>(v)->y);
                    F(old + 8, (f32)gabi::at<pa::Vec>(v)->z);
                    u += half;
                    ++i;
                }
                node = w(node + 12);
            }
        }
        t += stride;
    }
    u32 block = w((u32)this->mpBaseEmitter + 0x310), bank = w(block + 0x2BA70), remain = n - 1;
    if (remain) {
        u32 current = block + bank * 0x254 + 4;
        do {
            pa_flushVertexBuffer(p(current), u32(0),
                                 (u32)gabi::at<pa::ParticleNativeVertexBuffer>(current)->mSize);
            current += 0x4A8;
        } while (--remain);
        bank = w(block + 0x2BA70);
    }
    W(block + 0x2BA70, bank == 0 ? 1 : 0);
    pa_loadMaterial(p(w(model + 0xEC)));
}
/* WWHD glue
*/
