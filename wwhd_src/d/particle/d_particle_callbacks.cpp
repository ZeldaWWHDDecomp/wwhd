/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#define WWHD_PARTICLE_DATA_OWNER
#include "d/particle/d_particle_local.h"
using namespace pa;
// HD: callback tables are image-checked, with one owner across the split sources.
static const u32 l_particleSimpleVtable[14] = {
    0x00000000, 0x00000000, 0x00000000, 0x028F036C, 0x00000000, 0x028F036C, 0x00000000,
    0x025AAB5C, 0x00000000, 0x025A4AE0, 0x00000000, 0x025A4CB4, 0x00000000, 0x025AAB68};
static const u32 l_particleFollowVtable[18] = {
    0x00000000, 0x00000000, 0x00000000, 0x028F036C, 0x00000000, 0x028F036C,
    0x00000000, 0x025A590C, 0x00000000, 0x025AAB60, 0x00000000, 0x025A5A00,
    0x00000000, 0x025AAB68, 0x00000000, 0x025A5A04, 0x00000000, 0x025A5AC8};
static const u32 l_particleSmokeVtable[18] = {
    0x00000000, 0x00000000, 0x00000000, 0x025AAC48, 0x00000000, 0x028F036C,
    0x00000000, 0x025A590C, 0x00000000, 0x025AAB60, 0x00000000, 0x025A5D90,
    0x00000000, 0x025AAB68, 0x00000000, 0x025A5DD4, 0x00000000, 0x025A5F88};
static const u32 l_particleRippleVtable[18] = {
    0x00000000, 0x00000000, 0x00000000, 0x028F036C, 0x00000000, 0x028F036C,
    0x00000000, 0x025A90E0, 0x00000000, 0x025AAB60, 0x00000000, 0x025AAB64,
    0x00000000, 0x025A9180, 0x00000000, 0x025A91D8, 0x00000000, 0x00000000};
static const u32 l_particleModelEmitterVtable[6] = {0x00000000, 0x00000000, 0x00000000,
                                                    0x025A4158, 0x00000000, 0x025A4204};

static u32 l_particleEmitterManager;
static u32 l_particleModelPool;
static u8 l_particleStatus;

// Translation-unit constants have one owner across the split HD sources.
static const f32 l_particleColorRange = 255.0f;
static const f32 l_particleOne = 1.0f;
static const f32 l_particleZero = 0.0f;
static const f32 l_particleNormalizeEpsilon = 0.000003814697265625f;
static const f32 l_particleDegreesToRadians = 0.01745329238474369f;
static const f32 l_particleAngleToDegrees = 0.0054931640625f;
static const f32 l_particleHalf = 0.5f;

/* 025A36E0 */
void dPa_colorS10ToFloat(void* out, void* color) {
    u32 c = gabi::ea(color), o = gabi::ea(out);
    f32 x = f32(sh(c)) / (f32)l_particleColorRange, y = f32(sh(c + 2)) / (f32)l_particleColorRange,
        z = f32(sh(c + 4)) / (f32)l_particleColorRange,
        a = f32(sh(c + 6)) / (f32)l_particleColorRange;
    F(o, x);
    F(o + 4, y);
    F(o + 8, z);
    F(o + 12, a);
}
/* 025A37A4 */
void dPa_colorToFloat(void* out, void* color) {
    u32 c = gabi::ea(color), o = gabi::ea(out);
    f32 x = f32(b(c)) / (f32)l_particleColorRange, y = f32(b(c + 1)) / (f32)l_particleColorRange,
        z = f32(b(c + 2)) / (f32)l_particleColorRange,
        a = f32(b(c + 3)) / (f32)l_particleColorRange;
    F(o, x);
    F(o + 4, y);
    F(o + 8, z);
    F(o + 12, a);
}
/* 025A39FC */
void dPa_setWindPower(void* particle) {
    u32 a = gabi::ea(particle);
    gabi::Local<Vec> pos, wind, scaled;
    gabi::Local<f32> power;
    F(gabi::ea(pos.get()), f(a + 16));
    F(gabi::ea(pos.get()) + 4, f(a + 20));
    F(gabi::ea(pos.get()) + 8, f(a + 24));
    pa_dKyw_get_AllWind_vec(pos.get(), wind.get(), power.get());
    f32 amount = gabi::fmuls_ppc(f(gabi::ea(power.get())), f(0x10051A08));
    pa_PSVECScale(wind.get(), scaled.get(), amount);
    u32 x = gabi::ea(pos.get()), s = gabi::ea(scaled.get());
    f32 vx = f(x), vy = f(x + 4), sx = f(s), sy = f(s + 4);
    f32 ox = gabi::fadds_ppc(vx, sx), oy = gabi::fadds_ppc(vy, sy), sz = f(s + 8), vz = f(x + 8);
    F(a + 16, ox);
    F(a + 20, oy);
    F(a + 24, gabi::fadds_ppc(vz, sz));
}
/* 025A3A98 */
s32 dPa_effectNeedsSpecialWind(u32 id) {

    return (id >= 0x38 && id <= 0x39) || (id >= 0x3C && id <= 0x40) || id == 0x279 || id == 0x27C ||
           (id >= 0x467 && id <= 0x469) || id == 0x2041 || (id >= 0x80CB && id <= 0x80CC) ||
           id == 0x80CE || (id >= 0x80DC && id <= 0x80DD) || (id >= 0x82AA && id <= 0x82AC) ||
           (id >= 0x82D7 && id <= 0x82D8) || (id >= 0x8407 && id <= 0x840F) ||
           (id >= 0x8419 && id <= 0x841C) || id == 0x8443;
}
/* 025A3B80 */
void* dPa_modelEmitterCtor(void* obj) {
    obj = allocate(obj, 20);
    if (obj) {
        u32 a = gabi::ea(obj);
        W(a + 12, 0);
        W(a + 16, 0x10051B7C);
        pa_cNd_ForcedClear(obj);
    }
    return obj;
}
/* 025A4158 */
void dPa_modelEmitterDtor(void* obj, u32 flag) {
    if (obj) {
        u32 a = gabi::ea(obj), heap = w(a + 20);
        W(a + 16, gabi::ea(l_particleModelEmitterVtable));
        if (heap)
            pa_mDoExt_destroySolidHeap(p(heap));
        destroy(obj, flag);
    }
}
/* 025A41BC */
void* dPa_newModel(u32 mode) {
    u32 a = (u32)l_particleModelPool;
    for (u32 i = 0; i < 80; i++, a += 12)
        if (!b(a)) {
            B(a, 1);
            u32 model = w(a + 4);
            if (mode == 1)
                model = w(a + 8);
            return p(model);
        }
    return nullptr;
}
/* 025A479C */
void* dPa_J3DModelCtor(void* obj) {
    obj = allocate(obj, 12);
    if (obj) {
        u32 a = gabi::ea(obj);
        W(a + 4, 0);
        B(a, 0);
        W(a + 8, 0);
    }
    return obj;
}
/* 025A4920 */
void dPa_modelControlDtor(void* obj) {
    u32 node = w(gabi::ea(obj));
    while (node) {
        u32 next = w(node + 8);
        pa_cLs_SingleCut(p(node));
        u32 fn = w(w(node + 16) + 12);
        gabi::call_ptr<void>(fn, p(node), u32(3));
        node = next;
    }
}
/* 025A4A60 */
void* dPa_simpleCallbackCtor(void* obj) {
    obj = allocate(obj, 0x290);
    if (obj) {
        pa::dPa_simpleEcallBack* self = gabi::at<pa::dPa_simpleEcallBack>(gabi::ea(obj));
        u32 a = gabi::ea(obj);
        self->mpBaseEmitter = 0;
        self->__vtbl = gabi::ea(l_particleSimpleVtable);
        self->mbIsSmoke = 0;
        self->mResID = 0;
        self->mGrpID = 0;
        self->mCount = 0;
        pa_bzero(p(a + 16), u32(0x280));
        self->mpBaseEmitter = 0;
        self->mCount = 0;
    }
    return obj;
}
/* 025A4D40 */
u32 dPa_getResourceManagerId(u32 id) { return (id >> 15) & 1; }
/* 025A5098 */
void dPa_windParticleExecute(void* callback, void* emitter, void* particle) {
    pa_setWindPower(particle);
}
/* 025A50A0 */
void dPa_selectTextureDraw(void* callback, void* emitter) {
    u8 index = b(gabi::ea(callback) + 4);
    pa_setParticleTexture(p(gabi::ea(emitter) + 0x9C), index, u32(0), u32(0));
}
/* 025A575C */
void dPa_stripesSetupEnd(void* callback, void* emitter) {
    pa_prepareStripesEmitterRendering(emitter);
}
/* 025A5894 */
void* dPa_followCtor(void* obj, u8 rate, u8 fixed) {
    obj = allocate(obj, 20);
    if (obj) {
        pa::dPa_followEcallBack* self = gabi::at<pa::dPa_followEcallBack>(gabi::ea(obj));
        self->field_0x12 = fixed;
        self->mRateOff = rate;
        self->mpEmitter = 0;
        self->__vtbl = gabi::ea(l_particleFollowVtable);
        self->mAngle = 0;
        self->mFlag = 0;
        self->field_0x13 = 0;
        self->mPos = 0;
    }
    return obj;
}
/* 025A590C */
void pa::dPa_followEcallBack::execute(void* emitter) {
    u32 e = gabi::ea(emitter);
    if (!(u8)this->field_0x12 && !((u8)this->mFlag & 2)) {
        u32 pos = (u32)this->mPos;
        u8 group = (u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID;
        f32 x = (f32)gabi::at<pa::Vec>(pos)->x, y = (f32)gabi::at<pa::Vec>(pos)->y,
            z = (f32)gabi::at<pa::Vec>(pos)->z;
        if (group >= dPa_control_c::dPtclGroup_2Dfore_e)
            y = -y;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
        u32 angle = (u32)this->mAngle;
        if (angle) {
            s16 ay = sh(angle + 2), ax = sh(angle), az = sh(angle + 4);
            pa_JPAGetXYZRotateMtx(ax, ay, az, p(e + 0x1F0));
        }
    }
    if ((u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & JPAEmtrStts_EnableDeleteEmitter) {
        u32 n = (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength,
            m = (u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mLength;
        if (n + m == 0) {
            u32 fn = w((u32)this->__vtbl + 0x44);
            gabi::call_ptr<void>(fn, this);
        }
    }
    if ((u8)this->mFlag & 1) {
        gabi::Local<s16> alpha;
        H(gabi::ea(alpha.get()), (u8)gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a);
        pa_cLib_chaseS(alpha.get(), s16(1), s16(4));
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = b(gabi::ea(alpha.get()) + 1);
    }
}
/* 025A5A04 */
void pa::dPa_followEcallBack::setup(void* emitter, void* pos, void* angle, s8 info) {
    u32 e = gabi::ea(emitter);
    u32 fn = w((u32)this->__vtbl + 0x44);
    gabi::call_ptr<void>(fn, this);
    if (!(u8)this->field_0x13) {
        this->mpEmitter = e;
        gabi::at<pa::JPABaseEmitter>(e)->mFlags =
            (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags | JPAEmtrStts_Immortal;
    }
    if (!(u8)this->field_0x12) {
        if ((u8)this->mRateOff)
            gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = 0;
        this->mAngle = gabi::ea(angle);
        this->mPos = gabi::ea(pos);
        this->mFlag = 0;
    }
    if (e &&
        (u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID == dPa_control_c::dPtclGroup_Projection_e)
        pa_setEmitterDrawFlag(emitter, u32(1));
}
/* 025A5AC8 */
void pa::dPa_followEcallBack::end() {
    u32 e = (u32)this->mpEmitter;
    if (!e)
        return;
    u32 flags = (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags;
    gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = ~u32(0);
    gabi::at<pa::JPABaseEmitter>(e)->mFlags = flags | JPAEmtrStts_StopEmit;
    e = (u32)this->mpEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & ~u32(0x40);
    e = (u32)this->mpEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0;
    u8 bflags = (u8)this->mFlag;
    this->mpEmitter = 0;
    this->mFlag = bflags | 1;
}
static void smokeBase(void* obj) {
    u32 a = gabi::ea(obj);
    pa_followCtor(obj, u8(0), u8(0));
    B(a + 20, 0);
    W(a, gabi::ea(l_particleSmokeVtable));
    B(a + 21, 0);
    pa_bzero(p(a + 22), u32(4));
    W(a + 28, 0);
}
static void smokeColor(void* obj) {
    u32 a = gabi::ea(obj);
    if (!w(0x101FDD10)) {
        W(0x101FDD10, 1);
        pa_copyParticleBytes(p(0x101FEC25), p(0x10051A0C), u32(4));
    }
    copy(0x101FEC25, a + 22, 4);
}
/* 025A5B18 */
void* dPa_smokeCtorRate(void* obj, u8 rate) {
    obj = allocate(obj, 32);
    if (obj) {
        pa::dPa_smokeEcallBack* self = gabi::at<pa::dPa_smokeEcallBack>(gabi::ea(obj));
        smokeBase(obj);
        smokeColor(obj);
        self->mWindOff = 0;
        self->mRateOff = rate;
        self->field_0x14 = 0xFF;
        self->field_0x12 = 0;
        self->mTevstr = 0;
    }
    return obj;
}
/* 025A5C04 */
void* dPa_smokeCtorFlags(void* obj, u8 rate, u8 fixed, u8 flag, u8 temporary) {
    obj = allocate(obj, 32);
    if (obj) {
        pa::dPa_smokeEcallBack* self = gabi::at<pa::dPa_smokeEcallBack>(gabi::ea(obj));
        smokeBase(obj);
        smokeColor(obj);
        self->mFlag = flag;
        self->field_0x14 = 0xFF;
        self->mRateOff = rate;
        self->mWindOff = 0;
        self->field_0x12 = fixed;
        self->field_0x13 = temporary;
        self->mTevstr = 0;
    }
    return obj;
}
/* 025A5CEC */
void* dPa_smokeCtorColor(void* obj, void* color, void* tev, u8 rate) {
    obj = allocate(obj, 32);
    if (obj) {
        pa::dPa_smokeEcallBack* self = gabi::at<pa::dPa_smokeEcallBack>(gabi::ea(obj));
        u32 a = gabi::ea(obj);
        smokeBase(obj);
        copy(gabi::ea(color), a + 22, 4);
        self->mRateOff = rate;
        self->field_0x14 = 0xFF;
        self->mTevstr = gabi::ea(tev);
        self->field_0x12 = 0;
    }
    return obj;
}
/* 025A5D90 */
void pa::dPa_smokeEcallBack::execute(void* emitter) {
    u32 a = gabi::ea(this);
    gabi::Local<Color> color;
    copy(a + 22, gabi::ea(color.get()), 4);
    s8 info = s8((u8)this->field_0x14);
    u32 tev = (u32)this->mTevstr;
    pa_applyLighting(emitter, p(tev), info, color.get());
}
/* 025A5F88 */
void pa::dPa_smokeEcallBack::end() {
    u32 e = (u32)this->mpEmitter;
    if (!e)
        return;
    u32 flags = (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags;
    gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = ~u32(0);
    gabi::at<pa::JPABaseEmitter>(e)->mFlags = flags | JPAEmtrStts_StopEmit;
    e = (u32)this->mpEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & ~u32(0x40);
    e = (u32)this->mpEmitter;
    gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0x1047B284;
    u8 bflags = (u8)this->mFlag;
    this->mpEmitter = 0;
    this->mFlag = bflags | 1;
}
/* 025A4CB4 */
void pa::dPa_simpleEcallBack::draw(void* emitter) {
    if ((u8)this->mbIsSmoke) {
        if (!w(0x101FDD10)) {
            W(0x101FDD10, 1);
            pa_copyParticleBytes(p(0x101FEC25), p(0x10051A0C), u32(4));
        }
        gabi::Local<Color> color;
        copy(0x101FEC25, gabi::ea(color.get()), 4);
        pa_applyLighting(emitter, nullptr, s32(-1), color.get());
    }
}
/* 025A4D48 */
void* pa::dPa_simpleEcallBack::createEmitter(void* manager) {
    u32 a = gabi::ea(this), emitter = (u32)this->mpBaseEmitter;
    if (emitter)
        return p(emitter);
    if (!w(0x1047B1CC)) {
        W(0x1047B1CC, 1);
        f32 zero = (f32)l_particleZero;
        F(0x1047B1D0, zero);
        F(0x1047B1D8, zero);
        F(0x1047B1D4, zero);
    }
    u16 id = (u16)this->mResID;
    u32 resource = pa_getRM_ID(u32(id));
    u8 group = (u8)this->mGrpID;
    void* result = pa_JPAEmitterManager_createSimpleEmitterID(manager, p(0x1047B1D0), id, group,
                                                              resource, u32(0), u32(0));
    this->mpBaseEmitter = gabi::ea(result);
    if (!result)
        return result;
    W(gabi::ea(result) + 0x1E4, a);
    emitter = (u32)this->mpBaseEmitter;
    W(emitter + 0x5C, 0);
    emitter = (u32)this->mpBaseEmitter;
    W(emitter + 0x254, w(emitter + 0x254) | 1);
    return p((u32)this->mpBaseEmitter);
}
/* 025A4F7C */
s32 pa::dPa_simpleEcallBack::set(void* pos, u8 alpha, void* primary, void* environment, s32 flags) {
    u32 a = gabi::ea(this);
    s16 count = (s16)this->mCount;
    if (count >= 32)
        return 0;
    u32 out = a + 16 + u32(s32(count) * 20), v = gabi::ea(pos), c = gabi::ea(primary),
        env = gabi::ea(environment);
    gabi::store<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(out)->mPos.x), w(v));
    gabi::store<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(out)->mPos.y), w(v + 4));
    gabi::store<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(out)->mPos.z), w(v + 8));
    gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.r = b(c);
    gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.g = b(c + 1);
    u8 cb = b(c + 2);
    gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.a = alpha;
    gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.b = cb;
    gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[0] = b(env);
    gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[1] = b(env + 1);
    gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[2] = b(env + 2);
    u16 id = (u16)this->mResID;
    if (pa_effectNeedsSpecialWind(u32(id))) {
        gabi::Local<Color> sea, foam;
        pa_dKy_get_seacolor(sea.get(), foam.get());
        u32 s = gabi::ea(sea.get());
        gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.r = b(s);
        gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.g = b(s + 1);
        gabi::at<pa::dPa_simpleData_c>(out)->mPrmColor.b = b(s);
        gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[0] = b(s);
        gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[1] = b(s + 1);
        gabi::at<pa::dPa_simpleData_c>(out)->mEnvColor[2] = b(s + 2);
    }
    gabi::at<pa::dPa_simpleData_c>(out)->mbAffectedByWind = flags;
    this->mCount = u16((s16)this->mCount + 1);
    return 1;
}
/* 025A72F4 */
void dPa_bombSmokeExecute(void* callback, void* emitter) {
    u32 e = gabi::ea(emitter), work = (u32)gabi::at<pa::JPABaseEmitter>(e)->mUserData,
        target = work & 255, step = (work >> 8) & 255;
    gabi::Local<u32> timer;
    gabi::Local<s16> alpha;
    W(gabi::ea(timer.get()), work >> 16);
    if (!pa_referenceCountDecrement(timer.get())) {
        H(gabi::ea(alpha.get()), (u8)gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a);
        s32 reached = pa_cLib_chaseS(alpha.get(), s32(target), s32(step));
        u8 value = b(gabi::ea(alpha.get()) + 1);
        if (reached)
            gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0;
        gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = value;
    }
    gabi::at<pa::JPABaseEmitter>(e)->mUserData =
        (w(gabi::ea(timer.get())) << 16) | (step << 8) | target;
}
/* 025A71D4 */
void dPa_singleRippleExecute(void* callback, void* emitter) {
    gabi::Local<Color> sea, foam;
    pa_dKy_get_seacolor(sea.get(), foam.get());
    u32 s = gabi::ea(sea.get()), e = gabi::ea(emitter);
    u8 z = b(s + 2), y = b(s + 1);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = z;
    u8 x = b(s);
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = x;
}
/* 025A3858 */
void dPa_applyLighting(void* emitter, void* tev, s32 info, void* color) {
    u32 e = gabi::ea(emitter), t = gabi::ea(tev);
    gabi::Local<Color> inputColor;
    W(gabi::ea(inputColor.get()), w(gabi::ea(color)));
    if (b(e + 0x18E) & 1)
        return;
    gabi::Local<ColorF> floats;
    gabi::Local<Color> ambient;
    gabi::Local<Color16> diffuse;
    if (!t) {
        u32 env = gabi::ea(pa_getEnvlight());
        u32 d = gabi::ea(diffuse.get()), a = gabi::ea(ambient.get());
        H(d, b(env + 0xB64));
        H(d + 2, b(env + 0xB65));
        H(d + 6, 255);
        H(d + 4, b(env + 0xB66));
        B(a, b(env + 0xB68));
        B(a + 1, b(env + 0xB69));
        B(a + 3, 255);
        B(a + 2, b(env + 0xB6A));
        pa_colorS10ToFloat(floats.get(), diffuse.get());
        u32 v = gabi::ea(floats.get());
        W(e + 0x29C, w(v + 12));
        W(e + 0x290, w(v));
        W(e + 0x298, w(v + 8));
        W(e + 0x294, w(v + 4));
        pa_colorToFloat(floats.get(), ambient.get());
        W(e + 0x2B0, w(v));
        W(e + 0x2BC, w(v + 12));
        W(e + 0x2B8, w(v + 8));
        W(e + 0x2B4, w(v + 4));
    } else {
        pa_colorS10ToFloat(floats.get(), p(t + 0x90));
        u32 v = gabi::ea(floats.get());
        W(e + 0x298, w(v + 8));
        W(e + 0x29C, w(v + 12));
        W(e + 0x290, w(v));
        W(e + 0x294, w(v + 4));
        pa_colorToFloat(floats.get(), p(t + 0x98));
        W(e + 0x2B0, w(v));
        W(e + 0x2B4, w(v + 4));
        W(e + 0x2BC, w(v + 12));
        W(e + 0x2B8, w(v + 8));
    }
    pa_colorToFloat(floats.get(), inputColor.get());
    u32 v = gabi::ea(floats.get());
    u32 red = w(v), alpha = w(v + 12);
    W(e + 0x2C0, red);
    W(e + 0x2CC, alpha);
    W(e + 0x2C4, w(v + 4));
    W(e + 0x2C8, w(v + 8));
}
/* 025A9C3C */
void pa::dPa_cutTurnEcallBack_c::executeAfter(void* emitter) {
    u32 e = gabi::ea(emitter);
    if ((u8)this->field_0x5) {
        u32 flags = (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags;
        gabi::at<pa::JPABaseEmitter>(e)->mpEmitterCallBack = 0;
        gabi::at<pa::JPABaseEmitter>(e)->mMaxFrame = ~u32(0);
        gabi::at<pa::JPABaseEmitter>(e)->mFlags = flags | JPAEmtrStts_StopEmit;
        this->mpBaseEmitter = 0;
        return;
    }
    s16 count = (s16)this->field_0x6;
    if (count <= 0)
        return;
    u32 positions = (u32)this->mPosArray;
    if (!positions)
        return;
    u32 pos = positions + u32(s32(count) * 12) - 12;
    s32 remaining = s32(count) - 1;
    while (remaining > 0) {
        void* particle = pa_createParticle(emitter);
        if (particle) {
            u32 q = gabi::ea(particle);
            f32 y = (f32)gabi::at<pa::Vec>(pos)->y, x = (f32)gabi::at<pa::Vec>(pos)->x,
                z = (f32)gabi::at<pa::Vec>(pos)->z;
            gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.x = x;
            u32 mdl = (u32)gabi::at<pa::JPABaseParticle>(q)->hdModelData;
            gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.z = z;
            gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.y = y;
            void* material = pa_getParticleMaterial(p(mdl), u32(0));
            u32 m = gabi::ea(material);
            u8 flags = b(m + 0x190);
            W(m + 0x158, 0);
            W(m + 0x154, 0);
            B(m + 0x190, flags | 4);
            W(m + 0x150, 0);
        }
        --remaining;
        pos -= 12;
    }
    u8 group = (u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID;
    f32 x = (f32)gabi::at<pa::Vec>(pos)->x, z = (f32)gabi::at<pa::Vec>(pos)->z,
        y = (f32)gabi::at<pa::Vec>(pos)->y;
    if (group >= dPa_control_c::dPtclGroup_2Dfore_e)
        y = -y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
    gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a = (u8)this->mAlpha;
    this->field_0x6 = 0;
}
/* WWHD glue
data l_particleSimpleVtable 10052070
data l_particleFollowVtable 100523C8
data l_particleSmokeVtable 10052410
data l_particleRippleVtable 10052168
data l_particleModelEmitterVtable 100522A8
data l_particleEmitterManager 1047B2D4
data l_particleModelPool 101EA4E0
data l_particleStatus 101EA4E4
data l_particleColorRange 100519F0
data l_particleOne 10051C24
data l_particleZero 10051C28
data l_particleNormalizeEpsilon 10051C2C
data l_particleDegreesToRadians 10051C30
data l_particleAngleToDegrees 10051C34
data l_particleHalf 10051CA8
ext 025260E8 void pa_notifyParticleRoomRelease(s32 room)
ext 028ED6F8 void pa_initializeMathHeaderPrimary(void* state)
ext 028EAB2C void pa_initializeMathHeaderSecondary(void* state)
ext 0203E8F4 void* pa_getParticleSystemHeap()
ext 02753004 void* pa_createParticleHeap(u32 size, void* name, void* parent, u32 direction, u32 flags)
ext 027EC1FC u32 pa_getParticleHeapSize(void* heap)
ext C0009EE8 void pa_OSReport(void* format)
ext C0009EE8 void pa_OSReport(void* format, u32 size)
ext 02756170 u32 pa_exchangeParticleContext(void* manager, void* context)
ext 02756170 void pa_restoreParticleContext(void* manager, u32 context)
ext 026124B0 void* pa_loadParticleTextureBank(void* resources, void* archive, void* resource, u32 flags)
ext 027E2DC0 void* pa_getParticleTextureBank()
ext 028010F8 void pa_initializeParticleRenderControl(void* control, void* texture, void* heap)
ext 027DFA24 void* pa_findParticleTexture(void* table, void* name)
ext 02822304 void pa_registerParticleTexture(void* manager, void* texture, u16 id)
ext 0273B0D4 u32 pa_allocateParticleModelPool(void* allocator, void* heap, u32 alignment)
ext 0255F84C void pa_prepareParticleModelLighting()
ext 025EE048 void* pa_getParticleCallbackContext(void* callback)
ext 0282ED90 void pa_setParticleTexture(void* resource, u32 index, u32 mode, void* model)
ext 0282ED90 void pa_setParticleTexture(void* resource, u32 index, u32 mode, u32 model)
ext 02825824 void pa_initializeSmokeParticleModel(void* particle)
ext 02750250 void pa_captureParticleShaderState(void* state)
ext 02750534 void pa_applyParticleShaderState(void* state)
ext 0281E7B0 void pa_releaseTrackEmitterRendering(void* emitter)
ext 0281E76C void pa_prepareWaveEmitterRendering(void* emitter)
ext 028255F8 void pa_drawParticleModel(void* particle)
ext 028255F8 void pa_drawParticleModel(void* particle, void* emitter)
ext 027FFCBC void* pa_getParticleShaderBank()
ext 027B90AC s32 pa_findParticleShader(void* bank, void* name)
ext 02800B0C void pa_initializeParticleShader(void* entry, void* data, u32 flags)
ext 020385CC void pa_prepareParticleMenuEffect(void* effect)
ext 02038158 void pa_prepareParticleWorldEffect(void* effect)
ext 020385D0 void pa_applyParticleWorldEffect(void* effect)
ext 0274D83C void* pa_getParticleCameraMatrix(void* camera)
ext 0281E048 u32 pa_getParticleWidthCount(void* emitter)
ext 0281E094 u32 pa_getParticleHeightCount(void* emitter)
ext 0281E728 void pa_prepareStripesEmitterRendering(void* emitter)
ext C000A848 void pa_copyParticleBytes(void* destination, void* source, u32 count)
ext 02825158 void pa_selectParticleDrawMode(void* particle, u32 mode)
ext 02825284 void pa_prepareRippleParticleDraw(void* particle, u32 first, u32 size, u32 flags)
ext 02010008 void pa_cLs_Create(void* list)
ext 0200FDF4 void pa_cLs_SingleCut(void* node)
ext 02019C94 void pa_cNd_ForcedClear(void* node)
ext 028E8970 void pa_MTXCopy(void* source, void* destination)
ext 02562F5C void pa_setLightTevColorType(void* environment, void* model, void* tev)
ext 0255F160 void pa_dKy_setLight_again()
ext 02821C28 void pa_JPAEmitterManager_draw(void* manager, void* drawInfo, u32 group)
ext 02821448 void* pa_JPAEmitterManager_createSimpleEmitterID(void* manager, void* position, u32 id, u32 group, u32 resource, u32 emitterCallback, u32 particleCallback)
ext 02820DE4 void* pa_JPAEmitterManager_ctor(void* manager, void* resource, u32 maxParticles, u32 maxEmitters, u32 maxGroups, void* heap, void* renderControl)
ext 02821868 void pa_JPAEmitterManager_substep(void* manager, void* info, u8 group)
ext 028219A8 void pa_JPAEmitterManager_substepAfter(void* manager, void* info, u8 group)
ext 02821AE8 void pa_JPAEmitterManager_drawSubstep(void* manager, void* info, u8 group)
ext 02821D28 void pa_JPAEmitterManager_prepareMenuDraw(void* manager)
ext 02827000 void* pa_JPAResourceManager_ctor(void* manager, void* data, void* heap, void* textureBank)
ext 02826FC4 void* pa_JPAResourceManager_getResource(void* manager, u32 id)
ext 028F0164 void pa_destroyArray(void* array, u32 count, u32 stride, void* destructor, u32 flags, u32 reserved)
ext 028EFF8C void* pa_constructModelArray(void* array, u32 count, u32 stride, void* constructor, u32 flags)
ext 02008DAC void pa_cBgS_Chk_dtor(void* check, u32 flags)
ext 02008B4C void pa_cBgS_GndChk_baseDtor(void* check, u32 flags)
ext 0274D458 void pa_blendParticleColors(void* output, void* input, f32 blend)
ext 0281E5A8 void pa_setParticleShaderConfig(void* emitter, void* config)
ext 027FE640 void* pa_getParticleMaterial(void* modelData, u32 index)
ext 027FC80C void pa_setParticleMaterialMatrix(void* material, void* matrix, u32 index)
ext 025E7820 void* pa_mDoExt_btpAnm_ctor(void* animation)
ext 025E789C void pa_mDoExt_btpAnm_init(void* animation, void* modelData, void* pattern, u32 play, u32 attribute, u32 start, s32 end, u32 modify, u32 reserved, f32 speed)
ext 0273AA24 void pa_JUTAssertion_showAssert(void* file, u32 line, void* message)
ext 028F521C void pa_bzero(void* destination, u32 size)
ext 025A7EAC s32 pa_removeRoomScene(void* control, u32 force, u32 room)
ext 025A4920 void pa_modelControlDtor(void* control)
ext 025A7674 s32 pa_newSimple(void* control, u32 id, u32 group)
ext 025A5C04 void* pa_smokeCtorFlags(void* callback, u32 rate, u32 fixed, u32 flag, u32 temporary)
ext 025A47E0 void* pa_modelControlCtor(void* control, void* first, void* second, void* heap)
ext 025A7724 void pa_applyDefaultParameters(void* control)
ext 025A37A4 void pa_colorToFloat(void* output, void* color)
ext 025AA08C void pa_copyMatrix(void* output, void* input)
ext 025A5A04 void pa_followSetup(void* callback, void* emitter, void* position, void* angle, u32 info)
ext 025A3B80 void* pa_modelEmitterCtor(void* emitter)
ext 025A4E08 void* pa_simpleCreate(void* callback, void* manager, u32 id, u32 group)
ext 025A7C30 void pa_createRoomScene(void* control, void* data)
ext 025A8D10 void* pa_getSimple(void* control, u32 id)
ext 025A4F7C s32 pa_simpleSet(void* callback, void* position, u32 alpha, void* primary, void* environment, s32 flags)
ext 025A87C0 void* pa_setSimpleLand(void* control, s32 code, void* position, void* angle, void* tev, void* output, u32 flags, f32 landScale, f32 waterScale, f32 foamScale)
ext 025A8770 s32 pa_checkAtrCodeEffect(void* control, s32 code)
ext 025A5894 void* pa_followCtor(void* callback, u8 rate, u8 fixed)
ext 025A3858 void pa_applyLighting(void* emitter, void* tev, s32 info, void* color)
ext 025AAE08 u32 pa_referenceCountDecrement(void* counter)
ext 025A36E0 void pa_colorS10ToFloat(void* output, void* color)
ext 025A847C void* pa_setEmitter(void* control, u32 group, u32 id, void* position, void* angle, void* scale, u32 alpha, void* callback, s32 info, void* primary, void* environment, void* scale2D)
ext 02008254 s32 pa_cBgS_ChkPolySafe(void* background, void* polygon)
ext 02008974 f32 pa_cBgS_GroundCross(void* background, void* check)
ext 02008E0C void pa_cBgS_GndChk_ctor(void* check)
ext 024EF0F4 s32 pa_dBgS_GetAttributeCode(void* background, void* polygon)
ext 024EF7C0 s32 pa_dBgS_SplGrpChk(void* background, void* check)
ext 024F22DC void pa_dBgS_WtrChk_ctor(void* check)
ext 025E3528 void* pa_mDoExt_createSolidHeapFromGame(u32 size, void* parent)
ext 025E35BC void* pa_mDoExt_createSolidHeapToCurrent(u32 size, void* parent, u32 alignment)
ext 025E38E0 void* pa_mDoExt_J3DModel_create(void* data, u32 flags, u32 modelFlags)
ext 025E7B3C void pa_mDoExt_btpAnm_entry(void* animation, void* modelData, s32 frame)
ext 026066C4 void* pa_dRes_control_getRes(void* resources, void* name, u32 index)
ext 02838148 s32 pa_J3DUClipper_clip(void* clipper, void* matrix, void* position, f32 radius)
ext 025A4D48 void* pa_simpleCreateEmitter(void* callback, void* manager)
ext 025A4D40 u32 pa_getRM_ID(u32 id)
ext 025A3A98 s32 pa_effectNeedsSpecialWind(u32 id)
ext 025A92C0 void pa_waveRemove(void* callback)
ext 025A99B8 void pa_splashRemove(void* callback)
ext 025A9E38 void pa_trackRemove(void* callback)
ext 025A9DA0 void pa_trackGetMaxWaterY(void* callback, void* position)
ext 025A41BC void* pa_newModel(void* modelKind)
ext 028E98C0 void pa_PSMTXRotRad(void* matrix, u32 axis, f32 angle)
ext 028E9838 void pa_PSMTXRotAxisRad(void* matrix, void* axis, f32 angle)
ext 025E2DE0 void pa_mDoExt_modelUpdateDL(void* model, u32 flag)
ext 0200F5C8 s32 pa_cLib_chaseF(void* value, f32 target, f32 step)
ext 0281DCB8 void* pa_createParticle(void* emitter)
ext 025A39FC void pa_setWindPower(void* particle)
ext 028F026C void pa_registerGlobalObject(void* descriptor)
ext 025602F0 void pa_dKy_get_seacolor(void* sea, void* foam)
ext 028245AC void pa_JPAGetXYZRotateMtx(s32 x, s32 y, s32 z, void* matrix)
ext 028F37F0 f64 pa_sqrtHD(f32 value)
ext 028EFFD0 void pa_constructArray(void* array, u32 count, u32 stride, void* constructor)
ext 028E9098 void pa_PSMTXIdentity(void* matrix)
ext 028E9108 void pa_PSMTXConcat(void* left, void* right, void* out)
ext 028E8F64 void pa_PSMTXMultVec(void* matrix, void* vector, void* out)
ext 028E91EC s32 pa_PSMTXInverse(void* matrix, void* out)
ext 025E3868 void pa_mDoExt_destroySolidHeap(void* heap)
ext 025E3678 void pa_mDoExt_adjustSolidHeap(void* heap)
ext 025E37D8 void pa_mDoExt_restoreCurrentHeap()
ext 025D9F70 s32 pa_fopAcM_getWaterY(void* position, void* height)
ext 0200F564 s32 pa_cLib_chaseS(void* value, s32 target, s32 step)
ext 0273AF40 void pa_operatorDelete(void* object)
ext 0273AD10 void* pa_operatorNew(u32 size)
ext 0273B050 void* pa_operatorNewHeap(u32 size, void* heap, u32 alignment)
ext 027EC0B0 void pa_heapFree(void* object)
ext 0281E440 void pa_setEmitterDrawFlag(void* emitter, u32 flag)
ext 0282167C void pa_calcEmitterGroup(void* manager, u32 group)
ext 028221EC void pa_clearResourceManager(void* manager, u32 resource)
ext 0282228C void pa_forceDeleteAllEmitter(void* manager)
ext 027BFE5C void pa_bindVertexBuffer(void* buffer)
ext 027B5E94 void pa_flushVertexBuffer(void* buffer, u32 offset, u32 size)
ext C0006178 void pa_drawIndexed(void* indices, u32 count, void* vertices, u32 vertexCount, u32 first, u32 instances)
ext 027FC500 void pa_setMaterialColor(void* material, void* color, u32 slot)
ext 027FC650 void pa_setMaterialKonstColor(void* material, void* color, u32 slot)
ext 027FC420 void pa_setMaterialAmbientColor(void* material, void* color, u32 slot)
ext 027FB678 void pa_loadMaterial(void* material)
ext 027FE5B8 void pa_loadShape(void* shape, void* resource)
ext 02555D0C void* pa_getEnvlight()
ext 025200D4 void* pa_getGameInfo()
ext 0257E1B8 void pa_dKyw_get_AllWind_vec(void* position, void* wind, void* power)
ext 0201AE48 void pa_PSVECScale(void* source, void* out, f32 scale)
ext 0246B6A4 s32 pa_daSea_ChkArea(f32 x, f32 z)
ext 0246BA0C f32 pa_daSea_calcWave(f32 x, f32 z)
*/
