/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
static u32 lookup(u32 name) {
    gabi::Local<ParticleShaderName> n;
    n->mString = name;
    n->__vtbl = 0x10051A34;
    u32 bank = gabi::ea(pa_getParticleShaderBank());
    s32 index = pa_findParticleShader(p(w(bank + 4)), n.get());
    if (index < 0)
        return 0;
    u32 count = w(bank + 8), data = w(bank + 12), entry = data;
    if (u32(index) < count)
        entry += u32(index) * 0x24;
    if (!b(entry + 0x20)) {
        u32 table = w(bank + 4), aux = 0;
        if (u32(index) < w(table + 0x1C))
            aux = w(table + 0x20) + u32(index) * 0x84;
        pa_initializeParticleShader(p(entry), p(aux), u32(0));
        count = w(bank + 8);
        data = w(bank + 12);
    }
    return u32(index) < count ? data + u32(index) * 0x24 : data;
}
/* 025A4E08 */
void* pa::dPa_simpleEcallBack::create(void* manager, u32 id, u32 group) {
    this->mGrpID = group;
    this->mpBaseEmitter = 0;
    this->mResID = id;
    void* result = pa_simpleCreateEmitter(this, manager);
    if (result) {
        if (id == 0xA06A || id == 0xA410) {
            this->mbIsSmoke = 1;
            W((u32)this->mpBaseEmitter + 0x1E8, 0x1047B2E0);
            u32 emitter = (u32)this->mpBaseEmitter;
            W(emitter + 0x26C, lookup(0x10051C80));
        } else
            this->mbIsSmoke = 0;
        if (group == dPa_control_c::dPtclGroup_Projection_e)
            pa_setEmitterDrawFlag(p((u32)this->mpBaseEmitter), u32(1));
    }
    return p((u32)this->mpBaseEmitter);
}
/* 025A5DD4 */
void pa::dPa_smokeEcallBack::setup(void* emitter, void* pos, void* angle, u32 info) {
    u32 e = gabi::ea(emitter);
    if (!w(0x1047B1DC)) {
        W(0x1047B1DC, 1);
        W(0x1047B154, 0x100522C0);
        pa_registerGlobalObject(p(0x101EA438));
    }
    pa_followSetup(this, emitter, pos, angle, info);
    this->field_0x14 = info;
    gabi::at<pa::JPABaseEmitter>(e)->mpParticleCallBack = 0x1047B2E0;
    gabi::at<pa::JPABaseEmitter>(e)->mUserData = (u8)this->mWindOff;
    gabi::Local<ParticleShaderConfig> config;
    config->mName.mString = 0x10000160;
    config->mName.__vtbl = 0x10051A34;
    f32 one = (f32)l_particleOne;
    config->mScaleZ = one;
    config->mEnableY = 1;
    config->mScaleY = one;
    config->mScaleW = one;
    config->mEnableX = 1;
    config->mEnableZ = 0;
    config->mEnableW = 0;
    config->mScaleX = one;
    pa_setParticleShaderConfig(emitter, config.get());
    W(e + 0x26C, lookup(0x10051C98));
}
/* 025A5764 */
void dPa_stripesSetup(void* callback, void* emitter) {
    u32 e = gabi::ea(emitter),
        count = (u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mLength;
    if (!(b(e + 0x18E) & 2) || (u32)gabi::at<pa::JPABaseEmitter>(e)->mActiveParticles.mLength % 5 ||
        count < 10 || count % 5)
        return;
    u32 model = w((u32)gabi::at<pa::JPABaseEmitter>(e)->mChildParticles.mHead),
        material = w(model + 0xEC), target = w(w(material + 12) + 0x2C);
    gabi::call_ptr<void>(target, p(material), p(w(e + 0x26C)));
    pa_loadShape(p(w(model + 0xF0)), p(w(e + 0x26C)));
    u32 block = w(e + 0x30C),
        current =
            block + ((u32)gabi::at<pa::ParticleStripesBuffers>(block)->mBank == 0 ? 0x254 : 0),
        n = (count * 2u) / 5;
    for (u32 i = 0; i < 5; i++) {
        if ((u32)gabi::at<pa::ParticleStripesBuffers>(block)->mDrawReady)
            pa_bindVertexBuffer(p(current + 0x158));
        if (n)
            pa_drawIndexed(p(w(0x104B4A88)), n, p(w(0x104B4A84)), w(0x104B4A8C), u32(0), u32(1));
        current += 0x4A8;
    }
    B(model + 0x120, 1);
}
/* 025A4AE0 */
void pa::dPa_simpleEcallBack::executeAfter(void* emitter) {
    u32 e = gabi::ea(emitter);
    s32 repeat = s32(w(0x104B588C));
    if (repeat <= 0) {
        this->mCount = 0;
        gabi::at<pa::JPABaseEmitter>(e)->mFlags =
            (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags | JPAEmtrStts_StopEmit;
        return;
    }
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags & ~JPAEmtrStts_StopEmit;
    if ((s16)this->mCount) {
        u32 item = gabi::ea(&this->mSimpleData[0]);
        do {
            gabi::Local<Vec> pos;
            u32 v = gabi::ea(pos.get());
            W(v, gabi::load<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(item)->mPos.x)));
            W(v + 4, gabi::load<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(item)->mPos.y)));
            W(v + 8, gabi::load<u32>(gabi::ea(&gabi::at<pa::dPa_simpleData_c>(item)->mPos.z)));
            if (!pa_J3DUClipper_clip(p(0x1048CFF0), p(0x104B45F8), pos.get(), f(0x10051C7C))) {
                f32 x = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.x,
                    z = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.z,
                    y = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.y;
                if ((u8)gabi::at<pa::JPABaseEmitter>(e)->mGroupID >=
                    dPa_control_c::dPtclGroup_2Dfore_e)
                    y = -y;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.x = x;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.y = y;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalTranslation.z = z;
                u8 blue = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mPrmColor.b,
                   red = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mPrmColor.r,
                   green = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mPrmColor.g;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.r = red;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.b = blue;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.g = green;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalPrmColor.a =
                    (u8)gabi::at<pa::dPa_simpleData_c>(item)->mPrmColor.a;
                u8 g = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mEnvColor[1],
                   bl = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mEnvColor[2],
                   r = (u8)gabi::at<pa::dPa_simpleData_c>(item)->mEnvColor[0];
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.b = bl;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.r = r;
                gabi::at<pa::JPABaseEmitter>(e)->mGlobalEnvColor.g = g;
                s32 n = repeat > 0 ? repeat : 1;
                do {
                    void* particle = pa_createParticle(emitter);
                    if (!particle)
                        break;
                    u32 q = gabi::ea(particle);
                    f32 py = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.y,
                        px = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.x,
                        pz = (f32)gabi::at<pa::dPa_simpleData_c>(item)->mPos.z;
                    gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.x = px;
                    gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.y = py;
                    gabi::at<pa::JPABaseParticle>(q)->mOffsetPosition.z = pz;
                    if ((u8)gabi::at<pa::dPa_simpleData_c>(item)->mbAffectedByWind) {
                        if (!w(0x1047B1C8)) {
                            W(0x1047B1C8, 1);
                            W(0x1047B158, 0x100522F0);
                            pa_registerGlobalObject(p(0x101EA42C));
                        }
                        gabi::at<pa::JPABaseParticle>(q)->mpCallBack2 = 0x1047B158;
                    }
                } while (--n);
            }
            this->mCount = u16((s16)this->mCount - 1);
            item += sizeof(pa::dPa_simpleData_c);
        } while ((s16)this->mCount);
    }
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags | JPAEmtrStts_StopEmit;
}
/* WWHD glue
*/
