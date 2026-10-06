#include "d/actor/d_a_ib.h"
#include "bindings.h"

static void* ibPtr(u32 address) { return gabi::at<u8>(address); }
static u32 ibResource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = 0x10011B38;
    name->__vtbl = 0x100118C4;
    gabi::Local<u8[16]> linkage;
    u32 resources = gabi::load<u32>(0x101F4F28);
    return gabi::call<u32>(0x026066C4, ibPtr(resources), name.get(), index);
}
static void ibAssert(s32 line, u32 expression) {
    gabi::call(0x0273AA24, STR(0x10011A88), line, STR(expression));
}

static BOOL ibHeap(daIball_c* self) {
    WWHD_FUNC(0x0217AFB8, BOOL, self);
    u32 data = ibResource(0x2E);
    if (!data) ibAssert(0x47A, 0x10011A94);
    u32 model = gabi::call<u32>(0x025E38E0, ibPtr(data), 0x80000, 0x11000222);
    self->model = gabi::at<J3DModel>(model);
    if (!model) return 0;
    u32 animation = ibResource(0xE);
    if (!animation) ibAssert(0x48B, 0x10011A64);
    if (!gabi::call<BOOL>(0x025E8508, &self->transformAnimation, ibPtr(data),
                         ibPtr(animation), 1, 0, 1.0f, 0, -1, 0)) return 0;
    animation = ibResource(0x56);
    if (!animation) ibAssert(0x497, 0x10011A70);
    if (!gabi::call<BOOL>(0x025E7CE0, &self->textureAnimation, ibPtr(data),
                         ibPtr(animation), 1, 2, 1.0f, 0, -1, 0, 0)) return 0;
    for (s32 i = 0; i < 2; ++i) {
        animation = ibResource(0x4B + i);
        if (!animation) ibAssert(0x4A9, 0x10011A7C);
        if (!gabi::call<BOOL>(0x025E8154, &self->colorAnimation[i], ibPtr(data),
                             ibPtr(animation), 1, 2, 1.0f, 0, -1, 0, 0)) return 0;
    }
    return 1;
}
static BOOL ibHeapCallback(daIball_c* self) {
    WWHD_FUNC(0x0217B1F4, BOOL, self);
    return ibHeap(self);
}

static daIball_c* ibConstructor(daIball_c* self) {
    WWHD_FUNC(0x0217B1F8, daIball_c*, self);
    if (!self) {
        self = gabi::call<daIball_c*>(0x0273AD10, 0x964);
        if (!self) return nullptr;
    }
    u32 base = gabi::ea(self);
    gabi::call(0x025D4ED0, self);
    self->__vtbl = 0x10011A14;
    gabi::call(0x024F0474, &self->acch);
    gabi::store<u32>(base + 0x3C0, 0x10011A04);
    gabi::store<u32>(base + 0x3CC, 0x100119F4);
    gabi::store<u32>(base + 0x3BC, 0x100119E4);
    gabi::store<u8>(base + 0x3C4, 1);
    gabi::call(0x024EFE94, &self->wall);
    gabi::call(0x0200BD2C, &self->collisionStatus);
    gabi::call(0x02515DA0, ibPtr(base + 0x5CC));
    gabi::store<u32>(base + 0x5C8, 0x1004AE88);
    gabi::store<u32>(base + 0x5CC, 0x1004AEC0);
    gabi::call(0x02515FB8, &self->cylinder);
    gabi::store<u32>(base + 0x700, 0x100015A8);
    gabi::store<u32>(base + 0x6FC, 0x100118DC);
    gabi::call(0x02018590, ibPtr(base + 0x704));
    gabi::store<u32>(base + 0x628, 0x1004B108);
    gabi::store<u32>(base + 0x718, 0x1004B150);
    gabi::store<u32>(base + 0x700, 0x1004B160);
    gabi::call(0x025E7C6C, &self->textureAnimation);
    gabi::call(0x028EFFD0, &self->colorAnimation[0], 2, 0x78, ibPtr(0x025E80D0));
    gabi::call(0x027F2BC0, &self->transformAnimation, 0);
    gabi::store<u32>(base + 0x894, 0x1016E54C);
    gabi::call(0x027DA984, ibPtr(base + 0x898));
    gabi::store<u32>(base + 0x8DC, 0);
    gabi::store<u32>(base + 0x904, 0);
    gabi::store<u32>(base + 0x894, 0x100118EC);
    gabi::store<u32>(base + 0x908, 0);
    gabi::store<u32>(base + 0x8CC, 0x1016D820);
    gabi::store<u32>(base + 0x900, 0);
    gabi::store<u32>(base + 0x90C, 0);
    gabi::call(0x025A9084, ibPtr(base + 0x910));
    gabi::store<f32>(base + 0x95C, 1.0f);
    return self;
}

static void ibSetMatrix(daIball_c* self) {
    WWHD_FUNC(0x0217B380, void, self);
    f32 scaleX = self->scale.x, scaleY = self->scale.y;
    u32 model = gabi::ea(self->model);
    f32 scaleZ = self->scale.z;
    gabi::store<f32>(model + 0xBC, scaleX);
    gabi::store<f32>(model + 0xC4, scaleZ);
    gabi::store<f32>(model + 0xC0, scaleY);
    f32 y = (f32)self->current.pos.y + 40.0f;
    f32 x = self->current.pos.x, z = self->current.pos.z;
    gabi::call(0x028E93CC, ibPtr(0x1048D0CC), x, y, z);
    gabi::call(0x025F19F8, ibPtr(0x1048D0CC), (s16)self->current.angle.x,
               (s16)self->current.angle.y, (s16)self->current.angle.z);
    // Load the entire matrix before writing: the original permits overlapping storage.
    constexpr u32 loadOrder[12] = {11, 9, 1, 6, 10, 5, 3, 8, 7, 4, 0, 2};
    f32 values[12];
    for (s32 i = 0; i < 11; ++i)
        values[loadOrder[i]] = gabi::load<f32>(0x1048D0CC + loadOrder[i] * 4);
    model = gabi::ea(self->model);
    values[2] = gabi::load<f32>(0x1048D0D4);
    constexpr u32 storeOrder[12] = {11, 4, 0, 10, 2, 8, 6, 9, 1, 7, 5, 3};
    for (s32 i = 0; i < 12; ++i)
        gabi::store<f32>(model + 0xC8 + storeOrder[i] * 4, values[storeOrder[i]]);
}
static void ibPointLight(daIball_c* self) {
    WWHD_FUNC(0x0217B46C, void, self);
    f32 frame = self->colorAnimation[0].frame.mFrame;
    s32 color = 2;
    if (frame < 45.0f) {
        if (!(frame < 0.0f)) color = 0;
        else if (!(frame < 90.0f)) {
            if (!(frame > 135.0f)) color = 2;
        }
    } else if (frame < 90.0f) color = 1;
    else if (!(frame > 135.0f)) color = 2;
    f32 random = gabi::call<f32>(0x020198D8, 0.2f);
    gabi::call(0x0200ED84, &self->lightFlicker, random + 1.0f, 0.5f, 0.02f);
    self->light.position.copy(self->current.pos);
    u32 rgb = 0x10011ADC + (u32)color * 6;
    self->light.red = gabi::load<s16>(rgb);
    self->light.green = gabi::load<s16>(rgb + 2);
    self->light.blue = gabi::load<s16>(rgb + 4);
    f32 power = (gabi::load<f32>(0x1047BCD0) + 80.0f) * (f32)self->lightFlicker;
    self->light.power = (f32)(s16)gabi::ftoi(power);
    self->light.fluctuation = 250.0f;
}

static void ibCreateInit(daIball_c* self) {
    WWHD_FUNC(0x0217B5CC, void, self);
    u32 model = gabi::ea(self->model);
    gabi::store<u32>(gabi::ea(self) + 0x348, model ? model + 0xC8 : 0);
    gabi::call(0x02515F14, &self->collisionStatus, 255, 255, self);
    gabi::call(0x02516518, &self->cylinder, ibPtr(0x10011B40));
    gabi::store<u32>(gabi::ea(self) + 0x630, gabi::ea(&self->collisionStatus));
    gabi::call(0x024EFF44, &self->wall, 30.0f, 30.0f);
    gabi::call(0x024F06B4, &self->acch, &self->current.pos, &self->old.pos, self,
               1, &self->wall, &self->speed, 0, 0);
    f32 y = self->current.pos.y;
    u32 flags = gabi::load<u32>(gabi::ea(self) + 0x3D4);
    self->playedSound = 0;
    f32 frame = self->transformAnimation.mFrame;
    model = gabi::ea(self->model);
    gabi::store<u32>(gabi::ea(self) + 0x3D4, flags | 0x60000);
    self->mode = 0;
    self->current.pos.y = y - 40.0f;
    self->gravity = -7.0f;
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E86B8, &self->transformAnimation, ibPtr(data), frame);
    ibSetMatrix(self);
    gabi::call(0x027F4D5C, self->model.get());
    gabi::call(0x025564B4, &self->light);
    ibPointLight(self);
    self->lightFlicker = 1.0f;
    gabi::call(0x02526144, self);
}
static cPhs_State ibCreate(daIball_c* self) {
    WWHD_FUNC(0x0217B6F8, cPhs_State, self);
    u32 flags = self->actor_condition;
    if (!(flags & 8)) {
        if (self) {
            ibConstructor(self);
            flags = self->actor_condition;
        }
        self->actor_condition = flags | 8;
    }
    if (!gabi::call<BOOL>(0x025D63E8, self, ibPtr(0x0217B1F4), 0x3500)) return 5;
    ibCreateInit(self);
    return 4;
}
static cPhs_State ibCreateCallback(daIball_c* self) {
    WWHD_FUNC(0x0217B788, cPhs_State, self);
    return ibCreate(self);
}

static BOOL ibDraw(daIball_c* self) {
    WWHD_FUNC(0x0217B78C, BOOL, self);
    u32 environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, ibPtr(environment), 0, &self->current.pos, &self->tevStr);
    environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, ibPtr(environment), self->model.get(), &self->tevStr);
    for (s32 i = 0; i < 2; ++i) {
        f32 frame = self->colorAnimation[i].frame.mFrame;
        gabi::call(0x025E8480, &self->colorAnimation[i], self->model.get(), frame);
    }
    u32 model = gabi::ea(self->model);
    f32 frame = self->textureAnimation.mFrameCtrl.mFrame;
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E7FC4, &self->textureAnimation, ibPtr(data), frame);
    model = gabi::ea(self->model);
    frame = self->transformAnimation.mFrame;
    data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E86B8, &self->transformAnimation, ibPtr(data), frame);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D88));
    gabi::call(0x025E2DE0, self->model.get(), 0);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    return 1;
}
static BOOL ibDrawCallback(daIball_c* self) {
    WWHD_FUNC(0x0217B878, BOOL, self);
    return ibDraw(self);
}
static BOOL ibDelete(daIball_c* self) {
    WWHD_FUNC(0x0217B87C, BOOL, self);
    gabi::call(0x025A9270, ibPtr(gabi::ea(self) + 0x910));
    gabi::call(0x0255A374, &self->light);
    gabi::call(0x02526180, self);
    return 1;
}
static BOOL ibDeleteCallback(daIball_c* self) {
    WWHD_FUNC(0x0217B8C0, BOOL, self);
    return ibDelete(self);
}

static void ibCheckGeo(daIball_c* self) {
    WWHD_FUNC(0x0217B8C4, void, self);
    gabi::Local<u8[0x54]> checker;
    gabi::Local<cXyz> scale;
    gabi::Local<cXyz> position;
    gabi::Local<u8[16]> linkage;
    s32 timer = self->timer;
    self->previousSpeedY = self->speed.y;
    if (timer > 32) gabi::call(0x025D6870, self, &self->collisionStatus);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8, &self->acch, ibPtr(play + 0x12A0));
    u32 table = 0x10011B04 + (u32)(u8)self->mode * 8;
    s16 slot = gabi::load<s16>(table + 2);
    u32 adjusted = gabi::ea(self) + gabi::load<s16>(table);
    u32 target;
    if (slot < 0) target = gabi::load<u32>(table + 4);
    else {
        s16 offset = gabi::load<s16>(table + 6);
        u32 vtable = gabi::load<u32>(adjusted + offset);
        target = gabi::load<u32>(vtable + (u32)(s32)slot * 8 + 4);
    }
    gabi::call(target, ibPtr(adjusted));
    u32 groundCheck = gabi::ea(checker.get());
    gabi::call(0x02008E0C, checker.get());
    gabi::store<u8>(groundCheck + 0x48, 0);
    gabi::store<u8>(groundCheck + 0x45, 0);
    f32 z = self->current.pos.z, oldY = self->old.pos.y;
    gabi::store<u32>(groundCheck + 0x40, 0x100119D4);
    gabi::store<u32>(groundCheck + 0x20, 0x100119B4);
    gabi::store<u32>(groundCheck, groundCheck + 0x40);
    f32 y = oldY + 30.0f;
    gabi::store<u8>(groundCheck + 0x44, 1);
    f32 x = self->current.pos.x;
    gabi::store<u32>(groundCheck + 0x50, 4);
    gabi::store<u32>(groundCheck + 0x10, 0x100119A4);
    gabi::store<f32>(groundCheck + 0x2C, z);
    gabi::store<u8>(groundCheck + 0x46, 0);
    gabi::store<u8>(groundCheck + 0x4A, 0);
    gabi::store<u8>(groundCheck + 0x49, 0);
    gabi::store<u32>(groundCheck + 0x4C, 0x100119C4);
    y = y + 40.0f;
    gabi::store<u32>(groundCheck + 4, groundCheck + 0x4C);
    gabi::store<f32>(groundCheck + 0x24, x);
    gabi::store<u8>(groundCheck + 0x47, 0);
    gabi::store<f32>(groundCheck + 0x28, y);
    play = gabi::call<u32>(0x025200D4);
    f32 ground = gabi::call<f32>(0x02008974, ibPtr(play + 0x12A0), checker.get());
    f32 groundHeight = gabi::load<f32>(gabi::ea(self) + 0x440);
    if (ground != -1000000000.0f) {
        f32 difference = ground - groundHeight;
        f32 actorY = self->current.pos.y;
        bool disappear = difference < 20.0f ? ground > actorY : ground > actorY + 20.0f;
        if (disappear) {
            s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
            gabi::call(0x025E1A40, 0x691B, &self->current.pos, 0, reverb);
            position->x = self->current.pos.x;
            position->y = ground;
            position->z = self->current.pos.z;
            scale->x = 0.25f; scale->y = 0.25f; scale->z = 0.25f;
            play = gabi::call<u32>(0x025200D4);
            u32 particles = gabi::load<u32>(play + 0x5AB0);
            gabi::call(0x025A847C, ibPtr(particles), 0, 0x80D5, position.get(), 0,
                       scale.get(), 255, 0, -1, 0, 0, 0);
            gabi::call(0x025D57E0, self);
        }
    }
    gabi::store<u32>(groundCheck + 0x20, 0x10011934);
    gabi::store<u32>(groundCheck + 0x40, 0x10011954);
    gabi::store<u32>(groundCheck + 0x4C, 0x10011914);
    gabi::call(0x02008DAC, checker.get(), 0);
}
static void ibAnimation(daIball_c* self) {
    WWHD_FUNC(0x0217BB38, void, self);
    u32 play = gabi::call<u32>(0x025200D4);
    bool event = gabi::load<u8>(play + 0x5292) != 0;
    s32 index = self->playSpeedIndex;
    self->colorAnimation[0].frame.mRate = gabi::load<f32>(0x10011A28 + (u32)index * 4);
    gabi::call(0x025E742C, &self->colorAnimation[0]);
    bool looped = ((u8)self->colorAnimation[0].frame.mState & 2) != 0;
    self->colorAnimation[1].frame.mRate = 1.0f;
    gabi::call(0x025E742C, &self->colorAnimation[1]);
    self->textureAnimation.mFrameCtrl.mRate = 1.0f;
    gabi::call(0x025E742C, &self->textureAnimation);
    gabi::call(0x025E742C, &self->transformAnimation);
    if (!event) {
        f32 frame = self->colorAnimation[0].frame.mFrame;
        f32 previous = frame - (f32)self->colorAnimation[0].frame.mRate;
        if (looped || (!(frame < 45.0f) && previous < 45.0f) ||
            (!(frame < 90.0f) && previous < 90.0f)) {
            s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
            gabi::call(0x025E1A40, 0x580E, &self->current.pos, 0, reverb);
        }
    }
}
static void ibParticle(s32 id, cXyz* position, cXyz* scale, u32 callback) {
    u32 play = gabi::call<u32>(0x025200D4);
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    gabi::call(0x025A847C, ibPtr(particles), 0, id, position, 0, scale,
               255, ibPtr(callback), -1, 0, 0, 0);
}
static void ibDisappear(daIball_c* self, s32 type, s32 color) {
    WWHD_FUNC(0x0217BC58, void, self, type, color);
    gabi::Local<cXyz> scale;
    gabi::Local<cXyz> position;
    gabi::Local<u8[16]> linkage;
    scale->x = 1.0f; scale->y = 1.0f; scale->z = 1.0f;
    position->x = self->current.pos.x;
    position->z = self->current.pos.z;
    position->y = (f32)self->current.pos.y + 40.0f;
    if ((u32)type > 1) return;
    if (type == 0) {
        u32 callback = 0x1047B26C + (u32)color * 8;
        ibParticle(0x1C, position.get(), scale.get(), callback);
        ibParticle(0x1D, position.get(), scale.get(), 0);
        if (color == 2) ibParticle(0x47, position.get(), nullptr, 0);
    } else {
        ibParticle(0x19, position.get(), scale.get(), 0);
        ibParticle(0x1A, position.get(), scale.get(), 0);
        ibParticle(0x1B, position.get(), scale.get(), 0);
    }
    s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
    gabi::call(0x025E1A40, type == 0 ? 0x5816 : 0x580D,
               &self->current.pos, 0, reverb);
}
static BOOL ibDead(daIball_c* self) {
    WWHD_FUNC(0x0217BE98, BOOL, self);
    f32 frame = self->colorAnimation[0].frame.mFrame;
    s32 color = 2;
    if (frame < 45.0f) {
        if (!(frame < 0.0f)) color = 0;
        else if (!(frame < 90.0f)) {
            if (!(frame > 135.0f)) color = 2;
        }
    } else if (frame < 90.0f) color = 1;
    else if (!(frame > 135.0f)) color = 2;
    gabi::call(0x0217BC58, self, 0, color);
    gabi::call(0x025D57E0, self);
    return 1;
}
static BOOL ibCreateItem(daIball_c* self) {
    WWHD_FUNC(0x0217BF6C, BOOL, self);
    gabi::Local<be<u32>[8]> items;
    gabi::Local<csXyz> angles;
    gabi::Local<cXyz> scale;
    gabi::Local<u8[16]> linkage;
    u16 id = gabi::load<u16>(gabi::ea(self) + 0xB2);
    for (s32 i = 0; i < 8; ++i) {
        u32 play = gabi::call<u32>(0x025200D4);
        u32 table = play + 0x50A0;
        play = gabi::call<u32>(0x025200D4);
        u32 entry = gabi::load<u32>(play + 0x5110 + i * 4);
        (*items)[i] = gabi::call<u32>(0x0200EA74, ibPtr(table), entry, id);
    }
    f32 scaleX = gabi::load<f32>(0x101FFBA8);
    f32 scaleY = gabi::load<f32>(0x101FFBAC);
    f32 scaleZ = gabi::load<f32>(0x101FFBB0);
    for (s32 i = 0; i < 8; ++i) {
        s32 bit = -1;
        if (gabi::call<BOOL>(0x0255106C, (u8)(u32)(*items)[i])) {
            bit = (s8)(gabi::load<u32>(gabi::ea(self) + 0xB0) >> 16);
            bool replace = bit == 31 || bit == 255 || bit == -1;
            if (!replace) {
                s32 room = (s8)self->current.roomNo;
                u8 item = (u8)(u32)(*items)[i];
                u32 save = gabi::load<u32>(0x101F84DC);
                replace = item == 0x4B
                    ? gabi::call<BOOL>(0x025B8DC8, ibPtr(save + 0x598), bit, room)
                    : gabi::call<BOOL>(0x025BA494, ibPtr(save + 0x20), bit, room);
            }
            if (replace) { (*items)[i] = 3; bit = -1; }
        } else if (gabi::call<BOOL>(0x02551080, (u8)(u32)(*items)[i])) {
            if ((s8)(gabi::load<u32>(gabi::ea(self) + 0xB0) >> 16) != 0)
                (*items)[i] = 3;
            bit = -1;
        }
        angles->y = gabi::load<s16>(0x101FFB16);
        angles->x = gabi::load<s16>(0x101FFB14);
        angles->z = gabi::load<s16>(0x101FFB18);
        scale->x = scaleX; scale->y = scaleY; scale->z = scaleZ;
        f32 direction = gabi::call<f32>(0x02019918, 1.0f);
        f32 sign = direction < 0.0f ? -1.0f : 1.0f;
        f32 jitter = gabi::call<f32>(0x02019918, 4.0f);
        f32 horizontal = gabi::fmadds(sign, 10.0f, jitter);
        angles->y = (s16)gabi::ftoi(gabi::call<f32>(0x020198D8, 32767.0f));
        u32 item = gabi::call<u32>(0x02551108, (u8)(u32)(*items)[i]);
        (*items)[i] = item;
        f32 random = gabi::call<f32>(0x020198D8, 10.0f);
        f32 vertical = 53.0f + random;
        gabi::call(0x025D8AB0, &self->current.pos, item, (s8)self->current.roomNo,
                   angles.get(), scale.get(), bit, ibPtr(0x0217C7A4),
                   horizontal, vertical, -6.0f);
    }
    ibDead(self);
    return 1;
}
static void ibDamage(daIball_c* self) {
    WWHD_FUNC(0x0217C3C0, void, self);
    if (!gabi::call<BOOL>(0x025162A4, &self->cylinder)) return;
    u32 hit = gabi::call<u32>(0x02516300, &self->cylinder);
    if (hit && (gabi::load<u32>(hit + 0x10) &
                (0x101C4040u | 0x07008000u | 0x00010482u)))
        gabi::call<BOOL>(0x0217BF6C, self);
}
static BOOL ibExecute(daIball_c* self) {
    WWHD_FUNC(0x0217C444, BOOL, self);
    s32 timer = (s32)((u32)(s32)self->timer + 1);
    f32 eyeY = (f32)self->current.pos.y + 45.0f;
    u32 x = gabi::load<u32>(gabi::ea(self) + 0x314);
    u32 z = gabi::load<u32>(gabi::ea(self) + 0x31C);
    gabi::store<u32>(gabi::ea(self) + 0x37C, x);
    self->timer = timer;
    gabi::store<f32>(gabi::ea(self) + 0x380, eyeY);
    gabi::store<u32>(gabi::ea(self) + 0x384, z);
    ibPointLight(self);
    if (!self->playedSound) {
        s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
        gabi::call(0x025E1A40, 0x5802, &self->current.pos, 0, reverb);
        self->playedSound = 1;
    }
    gabi::call(0x0217B8C4, self);
    ibAnimation(self);
    ibDamage(self);
    ibSetMatrix(self);
    gabi::call(0x020182E0, ibPtr(gabi::ea(self) + 0x704), &self->current.pos);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, ibPtr(play + 0x26A4), &self->cylinder);
    return 1;
}
static BOOL ibExecuteCallback(daIball_c* self) {
    WWHD_FUNC(0x0217C518, BOOL, self);
    return ibExecute(self);
}
static void ibWaitInit(daIball_c* self) {
    WWHD_FUNC(0x0217C51C, void, self);
    gabi::call(0x025A9270, ibPtr(gabi::ea(self) + 0x910));
    self->mode = 0;
}
static void ibWaterInit(daIball_c* self) {
    WWHD_FUNC(0x0217C554, void, self);
    u32 play = gabi::call<u32>(0x025200D4);
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    gabi::call(0x025A847C, ibPtr(particles), 5, 0x33, &self->current.pos, 0,
               &self->scale, 255, ibPtr(gabi::ea(self) + 0x910), -1, 0, 0, 0);
    self->mode = 1;
    gabi::store<f32>(gabi::ea(self) + 0x920, 0.0f);
}
static void ibWait(daIball_c* self) {
    WWHD_FUNC(0x0217C5CC, void, self);
    if (gabi::load<u32>(gabi::ea(self) + 0x3D4) & 0x80) {
        f32 bounce = (f32)self->previousSpeedY * 0.5f;
        f32 limit = (f32)self->gravity - 0.5f;
        self->previousSpeedY = bounce;
        if (bounce > limit) self->speedF = 0.0f;
        else {
            self->speed.x = 0.0f;
            self->speed.z = 0.0f;
            self->speed.y = -bounce;
        }
    }
    f32 water = gabi::load<f32>(gabi::ea(self) + 0x47C);
    if (water > (f32)self->current.pos.y && water != -1000000000.0f) {
        ibWaterInit(self);
        self->current.pos.y = water;
    }
}
static void ibWater(daIball_c* self) {
    WWHD_FUNC(0x0217C688, void, self);
    f32 water = gabi::load<f32>(gabi::ea(self) + 0x47C);
    if (water == -1000000000.0f || water < (f32)self->current.pos.y)
        ibWaitInit(self);
    if (water != -1000000000.0f) self->current.pos.y = water;
}

static void ibStaticInit() {
    WWHD_FUNC(0x0217C710, void);
    gabi::store<u32>(0x104647AC, 0);
    gabi::store<u32>(0x104647A4, 0);
    gabi::store<u32>(0x104647B0, 0);
    gabi::store<u32>(0x104647A8, 0);
    gabi::call(0x028F026C, ibPtr(0x101B78A0));
    gabi::store<f32>(0x10464798, gabi::load<f32>(0x10011B30));
    gabi::store<f32>(0x1046479C, gabi::load<f32>(0x10011B34));
    gabi::call(0x028ED6F8, ibPtr(0x104647A0));
    gabi::call(0x028F026C, ibPtr(0x101B78AC));
    gabi::call(0x028EAB2C, ibPtr(0x104647A1));
    gabi::call(0x028F026C, ibPtr(0x101B78B8));
}
static BOOL ibInternalTrue() { WWHD_FUNC(0x0217C7A4, BOOL); return 1; }
static BOOL ibIsDelete(daIball_c* self) { WWHD_FUNC(0x0217C7C0, BOOL, self); return 1; }
static void ibStringDestructor(void* self, s32 flags) {
    WWHD_FUNC(0x0217C7AC, void, self, flags);
    if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
static void ibStringVirtual(void* self) { WWHD_FUNC(0x0217C870, void, self); }
static void ibDestructor(daIball_c* self, s32 flags) {
    WWHD_FUNC(0x0217C7C8, void, self, flags);
    if (!self) return;
    u32 base = gabi::ea(self);
    gabi::call(0x027F3628, ibPtr(base + 0x894), 0);
    gabi::call(0x02515A70, &self->cylinder, 2);
    gabi::call(0x02515860, &self->collisionStatus, 2);
    gabi::call(0x02018034, ibPtr(base + 0x584), 2);
    gabi::store<u32>(base + 0x3CC, 0x100119F4);
    gabi::store<u32>(base + 0x3C0, 0x10011A04);
    gabi::call(0x024EFD9C, &self->acch, 0);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1) gabi::call(0x0273AF40, self);
}

VERIFY(0x0217AFB8, ibHeap);
VERIFY(0x0217B1F4, ibHeapCallback);
VERIFY(0x0217B1F8, ibConstructor);
VERIFY(0x0217B380, ibSetMatrix);
VERIFY(0x0217B46C, ibPointLight);
VERIFY(0x0217B5CC, ibCreateInit);
VERIFY(0x0217B6F8, ibCreate);
VERIFY(0x0217B788, ibCreateCallback);
VERIFY(0x0217B78C, ibDraw);
VERIFY(0x0217B878, ibDrawCallback);
VERIFY(0x0217B87C, ibDelete);
VERIFY(0x0217B8C0, ibDeleteCallback);
VERIFY(0x0217B8C4, ibCheckGeo);
VERIFY(0x0217BB38, ibAnimation);
VERIFY(0x0217BC58, ibDisappear);
VERIFY(0x0217BE98, ibDead);
VERIFY(0x0217BF6C, ibCreateItem);
VERIFY(0x0217C3C0, ibDamage);
VERIFY(0x0217C444, ibExecute);
VERIFY(0x0217C518, ibExecuteCallback);
VERIFY(0x0217C51C, ibWaitInit);
VERIFY(0x0217C554, ibWaterInit);
VERIFY(0x0217C5CC, ibWait);
VERIFY(0x0217C688, ibWater);
VERIFY(0x0217C710, ibStaticInit);
VERIFY(0x0217C7A4, ibInternalTrue);
VERIFY(0x0217C7AC, ibStringDestructor);
VERIFY(0x0217C7C0, ibIsDelete);
VERIFY(0x0217C7C8, ibDestructor);
VERIFY(0x0217C870, ibStringVirtual);
