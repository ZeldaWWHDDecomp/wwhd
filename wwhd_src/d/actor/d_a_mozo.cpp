/**
 * d_a_mozo.cpp (WWHD)
 * Enemy - Moblin Statue (Mozo, daMozo_c): the statue that fires a beam (type 0/2) or fire (type 1).
 *
 * Verified against cking.rpx.
 * Written mostly from the WWHD code (most GameCube functions are Nonmatching stubs); raw style.
 * GameCube names: mozoHeap = daMozo_c::CreateHeap, mozoDraw = _draw, mozoEventMove = event_move,
 * mozoSetMatrix = set_mtx, mozoExecute = _execute, mozoDelete = _delete, mozoSetAnimation = setAnm,
 * mozoWaitInit = wait_proc_init, mozoCreateInit = CreateInit, mozoCreate = _create,
 * mozoAnimation = anime_proc, mozoCheckRange = checkRange, mozoGetBeamActor = getBeamActor,
 * mozoBeamInit/mozoBeam = search_beam_proc_init/search_beam_proc, mozoFireInit/mozoFire =
 * search_fire_proc_init/search_fire_proc, mozoToWaitInit/mozoToWait = towait_proc_init/towait_proc,
 * mozoHioConstructor = daMozo_HIO_c::daMozo_HIO_c, mozoJointCallback = the joint node callback,
 * the *Callback functions = the daMozo_* profile entries.
 */
#include "d/actor/d_a_mozo.h"
#include "bindings.h"

static void* mozoPtr(u32 address) { return gabi::at<u8>(address); }
static u32 mozoGetBeamActor(daMozo_c* self, s32 actorId);
static u32 mozoResource(s32 index, u32 archive) {
    gabi::Local<SafeString> name;
    name->mStringTop = archive;
    name->__vtbl = 0x10015254;
    gabi::Local<u8[16]> linkage;
    return gabi::call<u32>(0x026066C4, mozoPtr(gabi::load<u32>(0x101F4F28)), name.get(), index);
}
static BOOL mozoJointCallback(void* node, s32 timing) {
    WWHD_FUNC(0x021D3DF0, BOOL, node, timing);
    gabi::Local<be<f32>[12]> rotation;
    gabi::Local<u8[16]> linkage;
    u32 model = gabi::load<u32>(0x104B462C);
    u32 actor = gabi::load<u32>(model + 0xB8);
    u8 type = gabi::load<u8>(actor + 0x54E);
    if (type > 1) return 1;
    u32 joint = gabi::call<u32>(0x027F7878, node);
    u32 jointOffset = gabi::load<u16>(joint + 4) * 48;
    if (timing != 0) return 1;
    u32 origin, target;
    if (type == 0) {
        origin = 0x1046579C;
        target = 0x104657A8;
        if (!gabi::load<u32>(0x10465770)) {
            gabi::store<u32>(0x10465770, 1);
            gabi::store<f32>(origin, 60.0f);
            gabi::store<f32>(origin + 8, 0.0f);
            gabi::store<f32>(origin + 4, -20.0f);
        }
        if (!gabi::load<u32>(0x10465774)) {
            gabi::store<u32>(0x10465774, 1);
            gabi::store<f32>(target, 1250.0f);
            gabi::store<f32>(target + 4, -250.0f);
            gabi::store<f32>(target + 8, 0.0f);
        }
        if (!gabi::load<u32>(0x10465778)) {
            gabi::store<f32>(0x104657B8, -20.0f);
            gabi::store<f32>(0x104657BC, 12.5f);
            gabi::store<u32>(0x10465778, 1);
            gabi::store<f32>(0x104657B4, 60.0f);
        }
        if (!gabi::load<u32>(0x1046577C)) {
            gabi::store<f32>(0x104657C4, -20.0f);
            gabi::store<f32>(0x104657C8, -12.5f);
            gabi::store<u32>(0x1046577C, 1);
            gabi::store<f32>(0x104657C0, 60.0f);
        }
    } else {
        origin = 0x104657CC;
        target = 0x104657D8;
        if (!gabi::load<u32>(0x10465780)) {
            gabi::store<f32>(origin, 0.0f);
            gabi::store<f32>(origin + 8, 0.0f);
            gabi::store<u32>(0x10465780, 1);
            gabi::store<f32>(origin + 4, 0.0f);
        }
        if (!gabi::load<u32>(0x10465784)) {
            gabi::store<f32>(target + 4, 0.0f);
            gabi::store<u32>(0x10465784, 1);
            gabi::store<f32>(target, 5000.0f);
            gabi::store<f32>(target + 8, 0.0f);
        }
    }
    constexpr u32 matrix = 0x1048D0CC;
    u32 matrices = gabi::load<u32>(model + 0x2C);
    u32 data = gabi::load<u32>(matrices + 0x10);
    gabi::store<u16>(matrices + 4, gabi::load<u16>(matrices + 4) | 0x10);
    gabi::call(0x028E90D4, mozoPtr(data + jointOffset), mozoPtr(matrix));
    gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(origin), mozoPtr(actor + 0x4E4));
    gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(target), mozoPtr(actor + 0x4F0));
    gabi::call(0x028E90D4, mozoPtr(matrix), rotation.get());
    u32 local = gabi::ea(rotation.get());
    f32 x = gabi::load<f32>(local + 0xC), z = gabi::load<f32>(local + 0x2C);
    gabi::store<f32>(local + 0x2C, 0.0f);
    f32 y = gabi::load<f32>(local + 0x1C);
    gabi::store<f32>(local + 0x1C, 0.0f);
    gabi::store<f32>(local + 0xC, 0.0f);
    gabi::call(0x028E93CC, mozoPtr(matrix), x, y, z);
    gabi::call(0x025F25CC, mozoPtr(actor + 0x534));
    gabi::call(0x028E9108, mozoPtr(matrix), rotation.get(), mozoPtr(matrix));
    if (type == 0) {
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(origin), mozoPtr(actor + 0x4B4));
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(target), mozoPtr(actor + 0x4C0));
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(0x104657B4), mozoPtr(actor + 0x4CC));
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(0x104657C0), mozoPtr(actor + 0x4D8));
    } else {
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(origin), mozoPtr(actor + 0x508));
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(target), mozoPtr(actor + 0x514));
    }
    matrices = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(matrices + 4);
    if (type == 0) data = gabi::load<u32>(matrices + 0x10);
    gabi::store<u16>(matrices + 4, flags | 0x10);
    constexpr u32 beamLoads[12] = {6, 3, 8, 10, 11, 5, 1, 4, 7, 0, 9, 2};
    constexpr u32 beamStores[12] = {11, 9, 10, 1, 0, 7, 4, 8, 6, 3, 2, 5};
    constexpr u32 fireLoads[12] = {1, 7, 4, 11, 10, 2, 0, 9, 3, 6, 8, 5};
    constexpr u32 fireStores[12] = {8, 11, 9, 4, 2, 3, 0, 6, 10, 5, 1, 7};
    f32 values[12];
    const u32* loads = type == 0 ? beamLoads : fireLoads;
    const u32* stores = type == 0 ? beamStores : fireStores;
    for (s32 i = 0; i < 12; ++i) {
        values[loads[i]] = gabi::load<f32>(matrix + loads[i] * 4);
        if (type == 1 && i == 1) data = gabi::load<u32>(matrices + 0x10);
    }
    for (s32 i = 0; i < 12; ++i)
        gabi::store<f32>(data + jointOffset + stores[i] * 4, values[stores[i]]);
    gabi::call(0x028E90D4, mozoPtr(matrix), mozoPtr(0x104B4868));
    return 1;
}
VERIFY(0x021D3DF0, mozoJointCallback);
static BOOL mozoHeap(daMozo_c* self) {
    WWHD_FUNC(0x021D42B4, BOOL, self);
    u32 data = mozoResource(9, 0x100152E0);
    u32 animation = mozoResource(6, 0x100152E0);
    u32 morf = gabi::call<u32>(0x025E4F64, 0, mozoPtr(data), 0, 0, mozoPtr(animation),
                              2, 1.0f, 0, -1, 0, 0, 0, 0x11020203);
    self->morf = gabi::at<mDoExt_McaMorf>(morf);
    u32 color = mozoResource(0xC, 0x100152E0);
    self->colorResource = color;
    if (!color) gabi::call(0x0273AA24, STR(0x100152E8), 0x16A, STR(0x100152F8));
    u32 texture = mozoResource(0xF, 0x100152E0);
    self->textureResource = texture;
    if (!texture) gabi::call(0x0273AA24, STR(0x100152E8), 0x16D, STR(0x10015308));
    BOOL colorOk = gabi::call<BOOL>(0x025E8154, &self->colorAnimation, mozoPtr(data),
                       mozoPtr((u32)self->colorResource), 1, 0, 1.0f, 0, -1, 0, 0);
    BOOL textureOk = gabi::call<BOOL>(0x025E7CE0, &self->textureAnimation, mozoPtr(data),
                       mozoPtr((u32)self->textureResource), 1, 0, 1.0f, 0, -1, 0, 0);
    if (!data) return 0;
    morf = gabi::ea(self->morf.get());
    if (!morf || !gabi::load<u32>(morf + 0x90) || !colorOk || !textureOk) return 0;
    return 1;
}
VERIFY(0x021D42B4, mozoHeap);
static BOOL mozoHeapCallback(daMozo_c* self) {
    WWHD_FUNC(0x021D4494, BOOL, self);
    return mozoHeap(self);
}
VERIFY(0x021D4494, mozoHeapCallback);
static void* mozoHioConstructor(void* self) {
    WWHD_FUNC(0x021D6070, void*, self);
    if (!self) {
        self = gabi::call<void*>(0x0273AD10, 0x48);
        if (!self) return nullptr;
    }
    u32 base = gabi::ea(self);
    gabi::store<s8>(base, -1);
    gabi::store<s32>(base + 4, -1);
    gabi::store<u32>(base + 8, base + 0x10);
    gabi::store<u32>(base + 0xC, base + 0x24);
    gabi::store<f32>(base + 0x10, 1000.0f);
    gabi::store<f32>(base + 0x14, 1200.0f);
    gabi::store<s16>(base + 0x18, 0x2000);
    gabi::store<s16>(base + 0x1A, 0x2800);
    gabi::store<u8>(base + 0x1C, 0);
    gabi::store<u32>(base + 0x20, 0x1001528C);
    gabi::store<f32>(base + 0x24, 1000.0f);
    gabi::store<f32>(base + 0x28, 1200.0f);
    gabi::store<s16>(base + 0x2C, 0x2000);
    gabi::store<s16>(base + 0x2E, 0x2800);
    gabi::store<u8>(base + 0x30, 0);
    gabi::store<u32>(base + 0x34, 0x1001529C);
    gabi::store<f32>(base + 0x38, 0.0f);
    gabi::store<f32>(base + 0x3C, -300.0f);
    gabi::store<f32>(base + 0x40, 600.0f);
    gabi::store<u32>(base + 0x44, 0x100152AC);
    return self;
}
VERIFY(0x021D6070, mozoHioConstructor);
static void mozoStaticInit() {
    WWHD_FUNC(0x021D614C, void);
    gabi::store<u32>(0x10465794, 0);
    gabi::store<u32>(0x1046578C, 0);
    gabi::store<u32>(0x10465798, 0);
    gabi::store<u32>(0x10465790, 0);
    gabi::call(0x028F026C, mozoPtr(0x101BAC90));
    gabi::store<f32>(0x10465768, gabi::load<f32>(0x100153C8));
    gabi::store<f32>(0x1046576C, gabi::load<f32>(0x100153CC));
    gabi::call(0x028ED6F8, mozoPtr(0x10465788));
    gabi::call(0x028F026C, mozoPtr(0x101BAC9C));
    gabi::call(0x028EAB2C, mozoPtr(0x10465789));
    gabi::call(0x028F026C, mozoPtr(0x101BACA8));
    mozoHioConstructor(mozoPtr(0x104657E4));
}
VERIFY(0x021D614C, mozoStaticInit);
static void mozoSetAnimation(daMozo_c* self, u32 animation, f32 blend) {
    WWHD_FUNC(0x021D483C, void, self, animation, blend);
    self->animation = animation;
    if (animation > 4) return;
    u32 resource = mozoResource(6, 0x10015344);
    f32 speed = 0.0f, start = 0.0f, end = -1.0f;
    switch (animation) {
    case 1: speed = 1.0f; break;
    case 2: speed = -0.25f; start = 24.0f; end = 36.0f; break;
    case 3: speed = 1.0f; start = 25.0f; break;
    case 4: speed = -0.25f; start = 32.0f; end = 36.0f; break;
    default: break;
    }
    gabi::call(0x025E4A98, self->morf.get(), mozoPtr(resource), 0,
               blend, speed, start, end, 0);
}
VERIFY(0x021D483C, mozoSetAnimation);
static void mozoWaitInit(daMozo_c* self) {
    WWHD_FUNC(0x021D4AC0, void, self);
    mozoSetAnimation(self, 0, 0.0f);
    self->procedure.virtualIndex = -1;
    self->procedure.target = 0x021D53D8;
    self->procedure.adjustment = 0;
}
VERIFY(0x021D4AC0, mozoWaitInit);
static void mozoBeamInit(daMozo_c* self) {
    WWHD_FUNC(0x021D52C4, void, self);
    mozoSetAnimation(self, 1, 0.0f);
    self->textureAnimation.mFrameCtrl.mFrame = 0.0f;
    self->procedure.virtualIndex = -1;
    self->procedure.adjustment = 0;
    gabi::store<f32>(gabi::ea(self) + 0x43C, 1.0f);
    gabi::store<f32>(gabi::ea(self) + 0x3C0, 1.0f);
    gabi::store<f32>(gabi::ea(self) + 0x3C4, 0.0f);
    self->procedure.target = 0x021D551C;
}
VERIFY(0x021D52C4, mozoBeamInit);
static void mozoFireInit(daMozo_c* self) {
    WWHD_FUNC(0x021D534C, void, self);
    mozoSetAnimation(self, 1, 0.0f);
    gabi::store<f32>(gabi::ea(self) + 0x440, 0.0f);
    gabi::store<f32>(gabi::ea(self) + 0x3C4, 0.0f);
    self->procedure.virtualIndex = -1;
    gabi::store<f32>(gabi::ea(self) + 0x3C0, 1.0f);
    self->procedure.adjustment = 0;
    self->procedure.target = 0x021D593C;
    self->fireTimer = 0;
    gabi::store<f32>(gabi::ea(self) + 0x43C, 1.0f);
}
VERIFY(0x021D534C, mozoFireInit);
static void mozoToWaitInit(daMozo_c* self) {
    WWHD_FUNC(0x021D54A8, void, self);
    mozoSetAnimation(self, 2, 0.0f);
    gabi::store<f32>(gabi::ea(self->morf.get()) + 0x98, -0.5f);
    gabi::store<f32>(gabi::ea(self) + 0x3C0, -1.0f);
    self->procedure.adjustment = 0;
    self->procedure.target = 0x021D5E08;
    self->procedure.virtualIndex = -1;
    gabi::store<f32>(gabi::ea(self) + 0x43C, -1.0f);
}
VERIFY(0x021D54A8, mozoToWaitInit);
static void mozoSetMatrix(daMozo_c* self) {
    WWHD_FUNC(0x021D4654, void, self);
    u32 model = gabi::load<u32>(gabi::ea(self->morf.get()) + 0x90);
    f32 z = self->scale.z, x = self->scale.x, y = self->scale.y;
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
    gabi::call(0x028E93CC, mozoPtr(0x1048D0CC), (f32)self->current.pos.x,
               (f32)self->current.pos.y, (f32)self->current.pos.z);
    gabi::call(0x025F1C28, mozoPtr(0x1048D0CC), (s16)self->current.angle.y);
    constexpr u32 loads[12] = {8, 1, 0, 3, 6, 7, 4, 2, 11, 9, 5, 10};
    constexpr u32 stores[12] = {5, 11, 0, 4, 1, 2, 3, 6, 7, 9, 8, 10};
    f32 values[12];
    for (s32 i = 0; i < 12; ++i) values[loads[i]] = gabi::load<f32>(0x1048D0CC + loads[i] * 4);
    for (s32 i = 0; i < 12; ++i) gabi::store<f32>(model + 0xC8 + stores[i] * 4, values[stores[i]]);
}
VERIFY(0x021D4654, mozoSetMatrix);
static BOOL mozoDraw(daMozo_c* self) {
    WWHD_FUNC(0x021D4498, BOOL, self);
    gabi::call(0x025200D4);
    u32 model = gabi::load<u32>(gabi::ea(self->morf.get()) + 0x90);
    u32 data = gabi::load<u32>(model + 0xAC);
    u32 light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, mozoPtr(light), 1, &self->current.pos, &self->tevStr);
    u32 morf = gabi::ea(self->morf.get());
    light = gabi::call<u32>(0x02555D0C);
    model = gabi::load<u32>(morf + 0x90);
    gabi::call(0x02562F5C, mozoPtr(light), mozoPtr(model), &self->tevStr);
    gabi::call(0x025E83FC, &self->colorAnimation, mozoPtr(data), (f32)self->colorAnimation.frame.mFrame);
    gabi::call(0x025E7FC4, &self->textureAnimation, mozoPtr(data), (f32)self->textureAnimation.mFrameCtrl.mFrame);
    gabi::call(0x025E5590, self->morf.get());
    return 1;
}
VERIFY(0x021D4498, mozoDraw);
static BOOL mozoDrawCallback(daMozo_c* self) {
    WWHD_FUNC(0x021D4534, BOOL, self);
    return mozoDraw(self);
}
VERIFY(0x021D4534, mozoDrawCallback);
static void mozoAnimation(daMozo_c* self) {
    WWHD_FUNC(0x021D4F14, void, self);
    gabi::call(0x025E535C, self->morf.get(), 0, 0, 0);
    gabi::call(0x025E742C, &self->colorAnimation);
    gabi::call(0x025E742C, &self->textureAnimation);
    u32 morf = gabi::ea(self->morf.get());
    if (gabi::load<f32>(morf + 0x9C) < 24.0f) {
        s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
        gabi::call(0x025E1A40, 0x6179, mozoPtr(gabi::ea(self) + 0x37C), 0, reverb);
        morf = gabi::ea(self->morf.get());
    }
    if (gabi::call<BOOL>(0x027F2BF8, mozoPtr(morf + 0x98), 35.0f)) {
        u32 soundPosition = gabi::ea(self) + 0x37C;
        if (gabi::load<f32>(gabi::ea(self->morf.get()) + 0x98) > 0.0f) {
            s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
            gabi::call(0x025E1A40, 0x697A, mozoPtr(soundPosition), 0, reverb);
        } else {
            gabi::call(0x025E1B0C, mozoPtr(soundPosition), 0x697A);
            s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
            gabi::call(0x025E1A40, 0x697B, mozoPtr(soundPosition), 0, reverb);
        }
    }
}
VERIFY(0x021D4F14, mozoAnimation);
static void mozoEventMove(daMozo_c* self) {
    WWHD_FUNC(0x021D4538, void, self);
    u8 state = gabi::load<u8>(0x101BACB4);
    if (state == 2) return;
    if (gabi::load<u16>(gabi::ea(self) + 0xF8) == 2) {
        self->requestEvent = 0;
        gabi::store<u8>(0x101BACB4, 1);
        u32 common = gabi::call<u32>(0x025200D4);
        if (gabi::call<BOOL>(0x0254457C, mozoPtr(common + 0x52C4), STR(0x10015318))) {
            common = gabi::call<u32>(0x025200D4);
            gabi::store<u16>(common + 0x52B8, gabi::load<u16>(common + 0x52B8) | 8);
            gabi::store<u8>(0x101BACB4, 2);
            return;
        }
        state = gabi::load<u8>(0x101BACB4);
    } else if (state == 1) {
        u32 common = gabi::call<u32>(0x025200D4);
        if (gabi::call<BOOL>(0x0254457C, mozoPtr(common + 0x52C4), STR(0x10015318))) {
            common = gabi::call<u32>(0x025200D4);
            gabi::store<u16>(common + 0x52B8, gabi::load<u16>(common + 0x52B8) | 8);
            gabi::store<u8>(0x101BACB4, 2);
            return;
        }
        state = gabi::load<u8>(0x101BACB4);
    }
    if (state == 0 && (u8)self->requestEvent == 1) {
        gabi::call(0x025D77DC, self, STR(0x10015318), 1, 0xFFFF);
        u32 base = gabi::ea(self);
        gabi::store<u16>(base + 0xFA, gabi::load<u16>(base + 0xFA) | 2);
    }
}
VERIFY(0x021D4538, mozoEventMove);
static BOOL mozoExecute(daMozo_c* self) {
    WWHD_FUNC(0x021D4734, BOOL, self);
    gabi::call(0x025E55A0, self->morf.get());
    s32 index = self->procedure.virtualIndex;
    s32 adjustment = (s16)self->procedure.adjustment;
    u32 object = gabi::ea(self) + adjustment;
    u32 target;
    if (index < 0) target = self->procedure.target;
    else {
        s32 tableOffset = gabi::load<s16>(gabi::ea(self) + 0x3B2);
        u32 entry = gabi::load<u32>(object + tableOffset) + index * 8;
        target = gabi::load<u32>(entry + 4);
        gabi::cpu->r[7] = entry;       /* registers the original holds at the bctrl */
        gabi::cpu->r[9] = tableOffset;
    }
    gabi::cpu->r[8] = adjustment;
    gabi::cpu->r[10] = target;
    gabi::call_ptr(target, mozoPtr(object));
    mozoEventMove(self);
    mozoSetMatrix(self);
    return 1;
}
VERIFY(0x021D4734, mozoExecute);
static BOOL mozoExecuteCallback(daMozo_c* self) {
    WWHD_FUNC(0x021D47CC, BOOL, self);
    return mozoExecute(self);
}
VERIFY(0x021D47CC, mozoExecuteCallback);
static BOOL mozoCheckRange(daMozo_c* self, s32 expanded) {
    WWHD_FUNC(0x021D5030, BOOL, self, expanded);
    gabi::Local<cXyz> difference, horizontal, facing;
    gabi::Local<u8[16]> linkage;
    u32 common = gabi::call<u32>(0x025200D4);
    u32 player = gabi::load<u32>(common + 0x5B2C);
    if (expanded == 0 && gabi::load<f32>(player + 0x3CC) < 0.0f) return 0;
    gabi::call(0x0201ADE0, mozoPtr(player + 0x314), difference.get(), &self->current.pos);
    horizontal->x = difference->x;
    horizontal->y = 0.0f;
    horizontal->z = difference->z;
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    f32 distance = gabi::call<f32>(0x028F4384, square);
    if ((f32)difference->y > -280.0f) return 0;
    u32 hio = gabi::load<u32>(0x104657EC + (u8)self->type * 4);
    u32 angle = gabi::load<u16>(gabi::ea(self) + 0x322);
    f32 maximum = gabi::load<f32>(hio + (expanded ? 4 : 0));
    s32 angleMaximum = gabi::load<s16>(hio + (expanded ? 0xA : 8));
    u32 trig = 0x104A44F8 + (angle >> 3) * 8;
    facing->y = 0.0f;
    facing->x = gabi::load<f32>(trig);
    facing->z = gabi::load<f32>(trig + 4);
    f32 forward = gabi::call<f32>(0x028E8F44, difference.get(), facing.get());
    common = gabi::call<u32>(0x025200D4);
    u32 other = gabi::load<u32>(common + 0x5B2C);
    s32 toward = gabi::call<s32>(0x025D6894, self, mozoPtr(other));
    s32 separation = gabi::call<s32>(0x0200FAAC, toward, (s16)self->current.angle.y);
    return separation < angleMaximum && distance < maximum && forward > 200.0f;
}
VERIFY(0x021D5030, mozoCheckRange);
static void mozoWait(daMozo_c* self) {
    WWHD_FUNC(0x021D53D8, void, self);
    u32 common = gabi::call<u32>(0x025200D4);
    u32 player = gabi::load<u32>(common + 0x5B2C);
    if (!mozoCheckRange(self, 0)) return;
    u32 table = gabi::load<u32>(player + 0xB4);
    u32 target = gabi::load<u32>(table + 0x4C);
    if (gabi::call_ptr<BOOL>(target, mozoPtr(player))) return;
    u8 type = self->type;
    if (type == 0 || type == 2) mozoBeamInit(self);
    else if (type == 1) mozoFireInit(self);
    if (gabi::load<u8>(0x101BACB4) == 0) self->requestEvent = 1;
}
VERIFY(0x021D53D8, mozoWait);
static s32 mozoCreateInit(daMozo_c* self) {
    WWHD_FUNC(0x021D4B14, s32, self);
    gabi::Local<cXyz> childScale;
    gabi::Local<u8[16]> linkage;
    u32 base = gabi::ea(self);
    u32 model = gabi::load<u32>(gabi::ea(self->morf.get()) + 0x90);
    u8 parameter = gabi::load<u8>(base + 0xB3);
    u32 data = gabi::load<u32>(model + 0xAC);
    if (parameter == 0xFF) parameter = 0;
    if (parameter > 2) parameter = 2;
    u32 morf = gabi::ea(self->morf.get());
    self->type = parameter;
    model = gabi::load<u32>(morf + 0x90);
    gabi::store<u32>(model + 0xB8, base);
    u16 index = 0;
    u32 joints = gabi::call<u32>(0x027F3F94, mozoPtr(data));
    if (gabi::load<u16>(joints + 8) > 0) {
        do {
            index = (u16)(index + 1);
            joints = gabi::call<u32>(0x027F3F94, mozoPtr(data));
            if (index >= gabi::load<u16>(joints + 8)) break;
            if (index == 2) {
                u32 count = gabi::load<u32>(data + 4);
                u32 joint = gabi::load<u32>(data + 8);
                if (count > 2) joint += 0x38;
                gabi::store<u32>(joint + 8, 0x021D3DF0);
            }
        } while (true);
    }
    for (s32 i = 0; i < 4; ++i)
        gabi::store<u32>(base + 0x534 + i * 4, gabi::load<u32>(0x101E9C38 + i * 4));
    gabi::call(0x025D672C, self, -1000.0f, -1000.0f, -1000.0f);
    gabi::call(0x025D673C, self, 1000.0f, 1000.0f, 1000.0f);
    model = gabi::load<u32>(gabi::ea(self->morf.get()) + 0x90);
    u32 y = gabi::load<u32>(base + 0x318), x = gabi::load<u32>(base + 0x314);
    gabi::store<u32>(base + 0x4D0, y);
    gabi::store<u32>(base + 0x4D8, x);
    gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
    u32 z = gabi::load<u32>(base + 0x31C);
    u8 type = self->type;
    gabi::store<u32>(base + 0x4E0, z);
    gabi::store<u32>(base + 0x4D4, z);
    gabi::store<u32>(base + 0x4DC, y);
    gabi::store<u32>(base + 0x4CC, x);
    if (type == 0) {
        s32 room = gabi::load<s8>(base + 0x1C9);
        u32 actorId = gabi::load<u32>(base + 4);
        childScale->y = 1.5f;
        childScale->z = 20.0f;
        childScale->x = 1.5f;
        u32 child = gabi::call<u32>(0x025D5A20, 0xE8, actorId, 0, &self->leftOrigin,
                                   room, 0, childScale.get(), -1, 0);
        room = gabi::load<s8>(base + 0x1C9);
        self->leftChild = child;
        actorId = gabi::load<u32>(base + 4);
        self->rightChild = gabi::call<u32>(0x025D5A20, 0xE8, actorId, 0x30000000,
                                          &self->rightOrigin, room, 0, childScale.get(), -1, 0);
    } else {
        gabi::call(0x02515F14, &self->collisionStatus, 0xFF, 0xFF, self);
        gabi::call(0x025164C0, &self->capsule, mozoPtr(0x101BAC44));
        gabi::store<u32>(base + 0x5D8, base + 0x558);
        gabi::call(0x02018808, mozoPtr(base + 0x6AC), &self->current.pos, &self->current.pos);
        gabi::store<f32>(base + 0x6C8, 25.0f);
    }
    mozoSetMatrix(self);
    gabi::call(0x025E55A0, self->morf.get());
    mozoWaitInit(self);
    if (gabi::load<s8>(0x104657E4) < 0)
        gabi::store<u8>(0x104657E4, gabi::call<u32>(0x025F0A10, STR(0x1001535C), mozoPtr(0x104657E4)));
    return 4;
}
VERIFY(0x021D4B14, mozoCreateInit);
static s32 mozoCreate(daMozo_c* self) {
    WWHD_FUNC(0x021D4DB8, s32, self);
    u32 base = gabi::ea(self);
    u32 flags = gabi::load<u32>(base + 0x2E4);
    if (!(flags & 8)) {
        if (self) {
            gabi::call(0x025D4ED0, self);
            self->__vtbl = 0x1001527C;
            gabi::call(0x025E80D0, &self->colorAnimation);
            gabi::call(0x025E7C6C, &self->textureAnimation);
            gabi::call(0x0200BD2C, &self->collisionStatus);
            gabi::call(0x02515DA0, mozoPtr(base + 0x574));
            gabi::store<u32>(base + 0x570, 0x1004AE88);
            gabi::store<u32>(base + 0x574, 0x1004AEC0);
            gabi::call(0x02515FB8, &self->capsule);
            gabi::store<u32>(base + 0x6A8, 0x100015A8);
            gabi::store<u32>(base + 0x6A4, 0x1001526C);
            gabi::call(0x02018150, mozoPtr(base + 0x6AC));
            flags = gabi::load<u32>(base + 0x2E4);
            gabi::store<u32>(base + 0x6A8, 0x1004AF70);
            gabi::store<u32>(base + 0x5D0, 0x1004AF18);
            gabi::store<u32>(base + 0x6C4, 0x1004AF60);
        }
        gabi::store<u32>(base + 0x2E4, flags | 8);
    }
    s32 phase = gabi::call<s32>(0x02520460, &self->phase, STR(0x10015364));
    if (phase == 4) {
        if (!gabi::call<BOOL>(0x025D63E8, self, mozoPtr(0x021D4494), 0x1AA0)) return 5;
        phase = mozoCreateInit(self);
        mozoExecute(self);
    }
    return phase;
}
VERIFY(0x021D4DB8, mozoCreate);
static s32 mozoCreateCallback(daMozo_c* self) {
    WWHD_FUNC(0x021D4F10, s32, self);
    return mozoCreate(self);
}
VERIFY(0x021D4F10, mozoCreateCallback);
static u32 mozoParticle(u32 position, u32 id, s32 layer) {
    u32 common = gabi::call<u32>(0x025200D4);
    u32 particles = gabi::load<u32>(common + 0x5AB0);
    return gabi::call<u32>(0x025A847C, mozoPtr(particles), layer, id,
                           mozoPtr(position), 0, 0, 0xFF, 0, -1, 0, 0, 0);
}
static void mozoBeam(daMozo_c* self) {
    WWHD_FUNC(0x021D551C, void, self);
    gabi::Local<cXyz> towardPlayer, forward, beam, horizontal;
    gabi::Local<be<f32>[4]> rotation;
    gabi::Local<be<s16>[3]> angles;
    gabi::Local<u8[16]> linkage;
    u32 base = gabi::ea(self);
    u32 common = gabi::call<u32>(0x025200D4);
    f32 frame = gabi::load<f32>(gabi::ea(self->morf.get()) + 0x9C);
    u32 player = gabi::load<u32>(common + 0x5B2C);
    if (frame > 30.0f) {
        gabi::call(0x0201ADE0, mozoPtr(player + 0x314), towardPlayer.get(), &self->unrotatedOrigin);
        gabi::call(0x0201ADE0, &self->unrotatedTarget, forward.get(), &self->unrotatedOrigin);
        gabi::call(0x023127F8, rotation.get(), forward.get(), towardPlayer.get());
        gabi::call(0x028E9BC0, &self->quaternion, rotation.get(), &self->quaternion, 0.2f);
    }
    mozoAnimation(self);
    u32 morf = gabi::ea(self->morf.get());
    f32 end = (f32)gabi::load<s16>(morf + 0xA2);
    f32 finalFrame = gabi::fsubs_ppc(end, 1.0f);
    if (gabi::load<f32>(morf + 0x9C) > finalFrame) {
        u32 left = mozoGetBeamActor(self, self->leftChild);
        u32 right = mozoGetBeamActor(self, self->rightChild);
        if (left && right) {
            gabi::call(0x0201ADE0, &self->beamTarget, beam.get(), &self->beamOrigin);
            u32 localAngles = gabi::ea(angles.get());
            for (s32 i = 0; i < 3; ++i)
                gabi::store<u16>(localAngles + i * 2, gabi::load<u16>(0x101FFB14 + i * 2));
            s32 yaw = gabi::call<s32>(0x020195B0, (f32)beam->x, (f32)beam->z);
            horizontal->x = beam->x;
            horizontal->z = beam->z;
            horizontal->y = 0.0f;
            f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
            f32 length = gabi::call<f32>(0x028F4384, square);
            f32 down = -(f32)beam->y;
            s32 pitch = gabi::call<s32>(0x020195B0, down, length);
            if (!gabi::load<u32>(left + 0x8E0))
                gabi::store<u32>(left + 0x8E0, mozoParticle(left + 0x314, 0x8121, 0));
            s16 active = gabi::load<s16>(left + 0x844);
            f32 maximum = 5.0f;
            gabi::store<f32>(left + 0x798, 0.0f);
            if (active == 0) {
                f32 scale = gabi::load<f32>(left + 0x718);
                if (scale < maximum) gabi::store<f32>(left + 0x718, gabi::fadds_ppc(scale, 1.0f));
                else {
                    gabi::store<f32>(left + 0x798, 0.0f);
                    gabi::store<s16>(left + 0x844, 1);
                    gabi::store<f32>(left + 0x718, maximum);
                }
            } else gabi::store<f32>(left + 0x718, maximum);
            u32 x = gabi::load<u32>(base + 0x4CC);
            gabi::store<u32>(left + 0x314, x);
            u32 y = gabi::load<u32>(base + 0x4D0);
            gabi::store<u32>(left + 0x318, y);
            u32 z = gabi::load<u32>(base + 0x4D4);
            gabi::store<s16>(left + 0x320, pitch);
            gabi::store<u32>(left + 0x31C, z);
            gabi::store<s16>(left + 0x322, yaw);
            s16 roll = gabi::load<s16>(localAngles + 4);
            gabi::store<f32>(left + 0x8E4, 15.0f);
            gabi::store<s16>(left + 0x324, roll);
            if (!gabi::load<u32>(right + 0x8E0))
                gabi::store<u32>(right + 0x8E0, mozoParticle(right + 0x314, 0x8121, 0));
            active = gabi::load<s16>(right + 0x844);
            gabi::store<f32>(right + 0x798, 0.0f);
            if (active == 0) {
                f32 scale = gabi::load<f32>(right + 0x718);
                if (scale < maximum) {
                    maximum = gabi::fadds_ppc(scale, 1.0f);
                    gabi::store<f32>(right + 0x718, maximum);
                } else {
                    gabi::store<f32>(right + 0x798, 0.0f);
                    gabi::store<s16>(right + 0x844, 1);
                    gabi::store<f32>(right + 0x718, maximum);
                }
            } else gabi::store<f32>(right + 0x718, maximum);
            x = gabi::load<u32>(base + 0x4D8);
            gabi::store<u32>(right + 0x314, x);
            y = gabi::load<u32>(base + 0x4DC);
            gabi::store<u32>(right + 0x318, y);
            z = gabi::load<u32>(base + 0x4E0);
            gabi::store<s16>(right + 0x320, pitch);
            gabi::store<u32>(right + 0x31C, z);
            gabi::store<s16>(right + 0x322, yaw);
            roll = gabi::load<s16>(localAngles + 4);
            gabi::store<f32>(right + 0x8E4, 15.0f);
            gabi::store<s16>(right + 0x324, roll);
        }
    }
    if (!mozoCheckRange(self, 1)) mozoToWaitInit(self);
}
VERIFY(0x021D551C, mozoBeam);
static void mozoStopEmitter(u32 emitter) {
    gabi::store<s32>(emitter + 0x5C, -1);
    gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) | 1);
}
static void mozoToWait(daMozo_c* self) {
    WWHD_FUNC(0x021D5E08, void, self);
    u32 left = mozoGetBeamActor(self, self->leftChild);
    u32 right = mozoGetBeamActor(self, self->rightChild);
    gabi::call(0x028E9BC0, &self->quaternion, mozoPtr(0x101E9C38), &self->quaternion, 0.05f);
    mozoAnimation(self);
    u8 type = self->type;
    bool finished = false;
    if (type == 0) {
        if (!left || !right) return;
        u32 emitter = gabi::load<u32>(left + 0x8E0);
        if (emitter) {
            mozoStopEmitter(emitter);
            gabi::store<u32>(left + 0x8E0, 0);
        }
        s16 state = gabi::load<s16>(left + 0x844);
        if (state == 1) {
            f32 scale = gabi::load<f32>(left + 0x718);
            if (scale < 5.0f) gabi::store<f32>(left + 0x718, gabi::fadds_ppc(scale, 1.0f));
            f32 timer = gabi::load<f32>(left + 0x798);
            if (timer < 4.0f) gabi::store<f32>(left + 0x798, gabi::fadds_ppc(timer, 1.0f));
            else {
                gabi::store<f32>(left + 0x718, 0.0f);
                gabi::store<s16>(left + 0x844, 0);
                gabi::store<f32>(left + 0x798, 0.0f);
                finished = true;
            }
        } else {
            gabi::store<f32>(left + 0x718, 0.0f);
            gabi::store<f32>(left + 0x798, 0.0f);
            finished = true;
        }
        emitter = gabi::load<u32>(right + 0x8E0);
        if (emitter) {
            mozoStopEmitter(emitter);
            gabi::store<u32>(right + 0x8E0, 0);
        }
        state = gabi::load<s16>(right + 0x844);
        if (state == 1) {
            f32 scale = gabi::load<f32>(right + 0x718);
            if (scale < 5.0f) gabi::store<f32>(right + 0x718, gabi::fadds_ppc(scale, 1.0f));
            f32 timer = gabi::load<f32>(right + 0x798);
            if (timer < 4.0f) {
                gabi::store<f32>(right + 0x798, gabi::fadds_ppc(timer, 1.0f));
                return;
            }
            gabi::store<f32>(right + 0x798, 0.0f);
            gabi::store<f32>(right + 0x718, 0.0f);
            gabi::store<s16>(right + 0x844, 0);
        } else {
            gabi::store<f32>(right + 0x718, 0.0f);
            gabi::store<f32>(right + 0x798, 0.0f);
        }
        if (!finished) return;
    } else if (type != 1) return;
    if (!(gabi::load<f32>(gabi::ea(self) + 0x540) > 0.99f)) return;
    if (!(gabi::load<f32>(gabi::ea(self->morf.get()) + 0x9C) < 25.0f)) return;
    if (!(gabi::load<f32>(gabi::ea(self) + 0x3C4) < 1.0f)) return;
    if (!(gabi::load<f32>(gabi::ea(self) + 0x440) < 1.0f)) return;
    mozoWaitInit(self);
}
VERIFY(0x021D5E08, mozoToWait);
static void mozoFire(daMozo_c* self) {
    WWHD_FUNC(0x021D593C, void, self);
    gabi::Local<cXyz> toward, forward, direction, endpoint;
    gabi::Local<be<f32>[4]> rotation;
    gabi::Local<u8[16]> linkage;
    u32 base = gabi::ea(self);
    constexpr u32 matrix = 0x1048D0CC;
    gabi::call(0x025200D4);
    f32 frame = gabi::load<f32>(gabi::ea(self->morf.get()) + 0x9C);
    bool rotate = frame > 30.0f;
    if (!rotate) {
        s32 animation = self->animation;
        rotate = animation == 4 || animation == 3;
    }
    if (rotate) {
        gabi::call(0x025F1884, mozoPtr(matrix), (s16)self->current.angle.y);
        gabi::call(0x028E8F64, mozoPtr(matrix), mozoPtr(0x1046581C), toward.get());
        gabi::call(0x028E8D88, toward.get(), &self->current.pos, toward.get());
        gabi::call(0x028E8DAC, toward.get(), &self->unrotatedOrigin, toward.get());
        gabi::call(0x0201ADE0, &self->unrotatedTarget, forward.get(), &self->unrotatedOrigin);
        gabi::call(0x023127F8, rotation.get(), forward.get(), toward.get());
        gabi::call(0x028E9BC0, &self->quaternion, rotation.get(), &self->quaternion, 0.2f);
    }
    mozoAnimation(self);
    s32 animation = self->animation;
    if (animation == 1 || animation == 3) {
        u32 morf = gabi::ea(self->morf.get());
        f32 threshold = gabi::fsubs_ppc((f32)gabi::load<s16>(morf + 0xA2), 50.0f);
        if (!(gabi::load<f32>(morf + 0x9C) > threshold)) return;
        if (!(u32)self->fireEmitter) self->fireEmitter = mozoParticle(base + 0x314, 0x81A7, 0);
        if (!(u32)self->smokeEmitter) self->smokeEmitter = mozoParticle(base + 0x314, 0x81A8, 2);
        u32 model = gabi::load<u32>(gabi::ea(self->morf.get()) + 0x90);
        u32 matrices = gabi::load<u32>(model + 0x2C);
        u16 flags = gabi::load<u16>(matrices + 4);
        u32 data = gabi::load<u32>(matrices + 0x10);
        gabi::store<u16>(matrices + 4, flags | 0x10);
        gabi::call(0x028E90D4, mozoPtr(data + 0x60), mozoPtr(matrix));
        gabi::call(0x025F19F8, mozoPtr(matrix), 0x640, 0x4000, 0);
        gabi::call(0x025F24E0, 0.0f, 50.0f, 52.0f);
        u32 emitter = self->fireEmitter;
        if (emitter) gabi::call(0x028249B0, mozoPtr(matrix), mozoPtr(emitter + 0x1F0), mozoPtr(emitter + 0x22C));
        emitter = self->smokeEmitter;
        if (emitter) gabi::call(0x028249B0, mozoPtr(matrix), mozoPtr(emitter + 0x1F0), mozoPtr(emitter + 0x22C));
        gabi::call(0x0201ADE0, &self->fireTarget, direction.get(), &self->fireOrigin);
        f32 x, y, z;
        if (gabi::call<BOOL>(0x0201B47C, direction.get())) {
            y = direction->y; z = direction->z; x = direction->x;
        } else {
            x = gabi::load<f32>(0x101FFBA8);
            y = gabi::load<f32>(0x101FFBAC);
            direction->x = x;
            z = gabi::load<f32>(0x101FFBB0);
            direction->y = y;
            direction->z = z;
        }
        f32 time = (f32)(s16)self->fireTimer;
        f32 length = gabi::fmuls_ppc(time, 45.0f);
        f32 lengthDifference = gabi::fsubs_ppc(length, 600.0f);
        f32 radius = gabi::fmuls_ppc(time, 3.0f);
        f32 radiusDifference = gabi::fsubs_ppc(radius, 80.0f);
        endpoint->x = x;
        if (lengthDifference >= 0.0f) length = 600.0f;
        endpoint->y = y;
        if (radiusDifference >= 0.0f) radius = 80.0f;
        endpoint->z = z;
        gabi::call(0x028E8E64, endpoint.get(), endpoint.get(), length);
        gabi::call(0x028E8D88, endpoint.get(), &self->fireOrigin, endpoint.get());
        gabi::call(0x02018808, mozoPtr(base + 0x6AC), &self->fireOrigin, endpoint.get());
        u32 dx = gabi::load<u32>(gabi::ea(direction.get()));
        u32 dy = gabi::load<u32>(gabi::ea(direction.get()) + 4);
        gabi::store<u32>(base + 0x610, dx);
        u32 dz = gabi::load<u32>(gabi::ea(direction.get()) + 8);
        gabi::store<f32>(base + 0x6C8, radius);
        gabi::store<u32>(base + 0x614, dy);
        gabi::store<u32>(base + 0x618, dz);
        u32 common = gabi::call<u32>(0x025200D4);
        gabi::call(0x0200E240, mozoPtr(common + 0x26A4), &self->capsule);
        u32 ex = gabi::load<u32>(gabi::ea(endpoint.get()));
        u32 ey = gabi::load<u32>(gabi::ea(endpoint.get()) + 4);
        gabi::store<u32>(base + 0x520, ex);
        u32 ez = gabi::load<u32>(gabi::ea(endpoint.get()) + 8);
        gabi::store<u32>(base + 0x524, ey);
        gabi::store<u32>(base + 0x528, ez);
        gabi::call(0x025E19CC, 0x7033, &self->soundPosition);
        s16 timer = self->fireTimer;
        self->fireTimer = (s16)(timer + 1);
        if (timer > 60) {
            mozoSetAnimation(self, 4, 0.0f);
            emitter = self->fireEmitter;
            if (emitter) {
                gabi::store<f32>(emitter + 0x34, 0.0f);
                emitter = self->fireEmitter;
                mozoStopEmitter(emitter);
                self->fireEmitter = 0;
            }
            emitter = self->smokeEmitter;
            if (emitter) {
                gabi::store<f32>(emitter + 0x34, 0.0f);
                emitter = self->smokeEmitter;
                mozoStopEmitter(emitter);
                self->smokeEmitter = 0;
            }
            mozoToWaitInit(self);
        }
    } else if (animation == 4 && gabi::load<f32>(gabi::ea(self->morf.get()) + 0x9C) < 30.0f) {
        mozoSetAnimation(self, 3, 8.0f);
        self->fireTimer = 0;
    }
}
VERIFY(0x021D593C, mozoFire);
static u32 mozoGetBeamActor(daMozo_c* self, s32 actorId) {
    WWHD_FUNC(0x021D524C, u32, self, actorId);
    gabi::Local<s32> id;
    gabi::store<s32>(gabi::ea(id.get()), actorId);
    gabi::Local<u8[16]> linkage;
    u32 actor = 0;
    if (actorId != -1) actor = gabi::call<u32>(0x025D5218, mozoPtr(0x025E1234), id.get());
    if (actor && gabi::call<BOOL>(0x025D4604, mozoPtr(actor)) &&
        gabi::load<s16>(actor + 0xE) == 0xE8) return actor;
    return 0;
}
VERIFY(0x021D524C, mozoGetBeamActor);
static BOOL mozoDelete(daMozo_c* self) {
    WWHD_FUNC(0x021D47D8, BOOL, self);
    gabi::call(0x025204C8, &self->phase, STR(0x10015324));
    gabi::call(0x025E1B34, &self->soundPosition);
    s32 child = gabi::load<s8>(0x104657E4);
    if (child >= 0) {
        gabi::call(0x025F0A18, child);
        gabi::store<s8>(0x104657E4, -1);
    }
    return 1;
}
VERIFY(0x021D47D8, mozoDelete);
static BOOL mozoDeleteCallback(daMozo_c* self) {
    WWHD_FUNC(0x021D4838, BOOL, self);
    return mozoDelete(self);
}
VERIFY(0x021D4838, mozoDeleteCallback);
static BOOL mozoIsDelete(daMozo_c* self) { WWHD_FUNC(0x021D47D0, BOOL, self); return 1; }
VERIFY(0x021D47D0, mozoIsDelete);
static void mozoStringDestructor(void* self, s32 flags) {
    WWHD_FUNC(0x021D61EC, void, self, flags);
    if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
VERIFY(0x021D61EC, mozoStringDestructor);
static void mozoStringVirtual(void* self) { WWHD_FUNC(0x021D626C, void, self); }
VERIFY(0x021D626C, mozoStringVirtual);
static void mozoDestructor(daMozo_c* self, s32 flags) {
    WWHD_FUNC(0x021D6200, void, self, flags);
    if (!self) return;
    gabi::call(0x02515980, &self->capsule, 2);
    gabi::call(0x02515860, &self->collisionStatus, 2);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1) gabi::call(0x0273AF40, self);
}
VERIFY(0x021D6200, mozoDestructor);
