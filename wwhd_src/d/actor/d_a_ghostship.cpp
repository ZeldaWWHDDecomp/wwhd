#include "d/actor/d_a_ghostship.h"
#include <cmath>

static u8 *ghostPtr(u32 address) { return gabi::at<u8>(address); }
static void ghostCopyWord(be<f32> *destination, const be<f32> *source) {
    gabi::store<u32>(gabi::ea(destination), gabi::load<u32>(gabi::ea(source)));
}
static BOOL ghostSailFactor(u8 *cloth, s32 row, s32 column) {
    WWHD_FUNC(0x02149ED4, BOOL, cloth, row, column);
    return column == 0;
}
static BOOL ghostHeap(daGhostship_c *self) {
    WWHD_FUNC(0x02149EE0, BOOL, self);
    u32 data = gabi::ea(dComIfG_getObjectRes(STR(0x1000FC84), 5, 0x1000FAE0));
    if (!data)
        gabi::call(0x0273AA24, STR(0x1000FB84), 0x58, STR(0x1000FB98));
    self->model = mDoExt_J3DModel__create((J3DModelData *)ghostPtr(data), 0, 0x11020203);
    if (!self->model)
        return 0;
    u32 animation = gabi::ea(dComIfG_getObjectRes(STR(0x1000FC84), 8, 0x1000FAE0));
    if (!animation)
        gabi::call(0x0273AA24, STR(0x1000FB84), 0x5F, STR(0x1000FBAC));
    if (!gabi::call<BOOL>(0x025E7CE0, &self->btk, ghostPtr(data), ghostPtr(animation), true, 2,
                          1.0f, 0, (s16)-1, false, 0))
        return 0;
    u32 texture1 = gabi::ea(dComIfG_getObjectRes(STR(0x1000FC84), 0xB, 0x1000FAE0));
    u32 texture2 = gabi::ea(dComIfG_getObjectRes(STR(0x1000FC84), 0xC, 0x1000FAE0));
    u32 toon = gabi::ea(dComIfG_getObjectRes(STR(0x1000FC8C), 3, 0x1000FAE0));
    self->cloth = gabi::call<u8 *>(0x0251BE14, ghostPtr(texture1), ghostPtr(toon), 5, 5, 700.0f,
                                   350.0f, ghostPtr(gabi::ea(self) + 0x110), 0);
    u8 *second = gabi::call<u8 *>(0x0251BE14, ghostPtr(texture2), ghostPtr(toon), 6, 6, 1800.0f,
                                  1000.0f, ghostPtr(gabi::ea(self) + 0x110), 0);
    bool firstPresent = self->cloth != nullptr;
    self->cloth2 = second;
    if (!firstPresent || !second)
        return 0;
    gabi::store<u32>(gabi::ea(second) + 0xAC, 0x02149ED4);
    return 1;
}
static BOOL ghostHeapCB(daGhostship_c *self) {
    WWHD_FUNC(0x0214A0D8, BOOL, self);
    return ghostHeap(self);
}
static BOOL ghostPathStep(daGhostship_c *self, cXyz *position, cXyz *current, cXyz *next) {
    WWHD_FUNC(0x0214A0DC, BOOL, self, position, current, next);
    ghostCopyWord(&self->currentPoint.x, &current->x);
    ghostCopyWord(&self->currentPoint.y, &current->y);
    f32 water = self->current.pos.y;
    // HD reads source Z before replacing Y, preserving overlapping point inputs.
    u32 currentZ = gabi::load<u32>(gabi::ea(&current->z));
    self->currentPoint.y = water;
    gabi::store<u32>(gabi::ea(&self->currentPoint.z), currentZ);
    ghostCopyWord(&self->nextPoint.x, &next->x);
    ghostCopyWord(&self->nextPoint.y, &next->y);
    u32 nextZ = gabi::load<u32>(gabi::ea(&next->z));
    self->nextPoint.y = water;
    gabi::store<u32>(gabi::ea(&self->nextPoint.z), nextZ);
    gabi::Local<cXyz> delta, difference, secondDifference, xz, secondXZ;
    // Reserve the caller linkage area below live guest-stack temporaries.
    gabi::Local<u8[16]> linkage;
    cXyz_mi(&self->nextPoint, delta.get(), &self->currentPoint);
    if (!gabi::call<BOOL>(0x0201B47C, delta.get()))
        return 1;
    f32 x = delta->x, z = delta->z;
    s16 target = gabi::call<s16>(0x020195B0, x, z);
    s16 remaining = gabi::call<s16>(0x0200F378, &self->current.angle.y, target, 8, 0x200, 8);
    f32 cosine = cM_scos(remaining);
    f32 step = (f32)self->speedF * std::fabs(cosine);
    gabi::call(0x0200F764, position, &self->nextPoint, step);
    cXyz_mi(position, difference.get(), &self->nextPoint);
    xz->x = difference->x;
    xz->y = 0;
    xz->z = difference->z;
    f32 distance = std_sqrtf(PSVECSquareMag(xz.get()));
    f32 factor = gabi::load<f32>(0x1047BCE4) + 1.0f;
    if (distance < step * factor)
        return 1;
    cXyz_mi(position, secondDifference.get(), &self->nextPoint);
    secondXZ->x = secondDifference->x;
    secondXZ->y = 0;
    secondXZ->z = secondDifference->z;
    return std_sqrtf(PSVECSquareMag(secondXZ.get())) == 0.0f;
}
static BOOL ghostPathCB(cXyz *p, cXyz *current, cXyz *next, daGhostship_c *self) {
    WWHD_FUNC(0x0214A2B4, BOOL, p, current, next, self);
    return ghostPathStep(self, p, current, next);
}
static void ghostGetArg(daGhostship_c *self) {
    WWHD_FUNC(0x0214A2CC, void, self);
    u32 parameter = self->mParameters;
    self->moonPhase = (u8)parameter;
    self->pathNo = (parameter >> 16) & 255;
}
static void ghostPathInit(daGhostship_c *self) {
    WWHD_FUNC(0x0214A2E0, void, self);
    self->mode = 2;
}
static void ghostWaitInit(daGhostship_c *self) {
    WWHD_FUNC(0x0214A2EC, void, self);
    self->mode = 0;
}

// Shared by two inlined HD paths; this is not a guest function call.
static void ghostMatrices(daGhostship_c *self) {
    gabi::call(0x025872F4, &self->current.pos, 0.0f, &self->wave);
    self->shape_angle.x = self->wave.rotX;
    self->shape_angle.z = self->wave.rotZ;
    f32 sx = self->scale.x, sy = self->scale.y, sz = self->scale.z;
    u32 model = gabi::ea(self->model);
    gabi::store<f32>(model + 0xBC, sx);
    gabi::store<f32>(model + 0xC0, sy);
    gabi::store<f32>(model + 0xC4, sz);
    u8 *matrix = ghostPtr(0x1048D0CC);
    f32 px = self->current.pos.x, py = self->current.pos.y, pz = self->current.pos.z;
    gabi::call(0x028E93CC, matrix, px, py, pz);
    s16 ax = self->shape_angle.x, az = self->shape_angle.z;
    gabi::call(0x025F19F8, matrix, ax, 0, az);
    gabi::call(0x025F1C28, matrix, (s16)self->shape_angle.y);
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = gabi::load<f32>(0x1048D0CC + i * 4);
    model = gabi::ea(self->model);
    for (u32 i = 0; i < 12; i++)
        gabi::store<f32>(model + 0xC8 + i * 4, values[i]);
    model = gabi::ea(self->model);
    gabi::call(0x028E90D4, ghostPtr(model ? model + 0xC8 : 0), matrix);
    gabi::call(0x025F23EC);
    gabi::call(0x025F24E0, 0.0f, 3200.0f, 75.0f);
    gabi::call(0x0251B638, (u8 *)self->cloth, matrix);
    gabi::call(0x025F2468);
    gabi::call(0x025F24E0, -900.0f, 2080.0f, 85.0f);
    gabi::call(0x025F1C28, matrix, 0x4000);
    gabi::call(0x0251B638, (u8 *)self->cloth2, matrix);
}
static void ghostCreateInit(daGhostship_c *self) {
    WWHD_FUNC(0x0214A2F8, void, self);
    ghostCopyWord(&self->pathPos.x, &self->current.pos.x);
    ghostCopyWord(&self->pathPos.y, &self->current.pos.y);
    ghostCopyWord(&self->pathPos.z, &self->current.pos.z);
    for (s32 i = 0; i < 12; i++) {
        auto &circle = self->circles[i];
        s16 angle = (s16)(i * 0x2000), oldSpeed = circle.angleSpeed;
        circle.angle = angle;
        f32 vertical = 600.0f * cM_scos((s16)(angle + oldSpeed));
        s32 direction = 1;
        if (i & 1) {
            vertical = 600.0f * cM_ssin((s16)(angle + oldSpeed));
            direction = -1;
        }
        circle.radius = (f32)i * 100.0f;
        circle.wobbleAmplitude = gabi::load<f32>(0x1047BCD4) + 300.0f;
        s32 base = 0x100 + gabi::load<s16>(0x1047BD48) + 16 * i;
        f32 distance = gabi::load<f32>(0x1047BCF8) + 500.0f;
        f32 random = cM_rndF(100.0f);
        circle.angleSpeed = (s16)gabi::ftoi(((f32)base + random) * (f32)direction);
        circle.translation.x =
            gabi::fmadds(distance, cM_ssin(self->shape_angle.y), (f32)self->current.pos.x);
        f32 y = ((f32)self->current.pos.y + 600.0f) + vertical;
        random = cM_rndF(100.0f);
        circle.translation.y = y + random;
        circle.translation.z =
            gabi::fmadds(distance, cM_scos(self->shape_angle.y), (f32)self->current.pos.z);
        gabi::call(0x02587128, &circle);
    }
    if (self->pathNo != 255) {
        self->path = gabi::call<u8 *>(0x025AAF88, (s32)self->pathNo, (s8)self->current.roomNo);
        ghostPathInit(self);
    } else
        ghostWaitInit(self);
    gabi::call(0x024EFF44, &self->cir, 30.0f, 30.0f);
    gabi::call(0x024F06B4, &self->acch, &self->current.pos, &self->old.pos, self, 1, &self->cir,
               &self->speed, 0, 0);
    gabi::store<u32>(gabi::ea(self) + 0x4A4, gabi::load<u32>(gabi::ea(self) + 0x4A4) | 0xC);
    ghostMatrices(self);
}
static BOOL ghostCreate(daGhostship_c *self) {
    WWHD_FUNC(0x0214A758, BOOL, self);
    if (!((u32)self->actor_condition & 8)) {
        if (self) {
            gabi::call(0x025D4ED0, self);
            gabi::store<u32>(gabi::ea(self) + 0xB4, 0x1000FB28);
            gabi::call(0x025E7C6C, &self->btk);
            gabi::call(0x024F0474, &self->acch);
            gabi::store<u32>(gabi::ea(self) + 0x48C, 0x1000FAF8);
            gabi::store<u32>(gabi::ea(self) + 0x49C, 0x1000FB08);
            gabi::store<u32>(gabi::ea(self) + 0x490, 0x1000FB18);
            gabi::store<u8>(gabi::ea(self) + 0x494, 1);
            gabi::call(0x024EFE94, &self->cir);
        }
        self->actor_condition = (u32)self->actor_condition | 8;
    }
    s32 phase = gabi::call<s32>(0x02520460, &self->phase, STR(0x1000FC84));
    if (phase != 4)
        return phase;
    phase = gabi::call<s32>(0x02520460, &self->clothPhase, STR(0x1000FC8C));
    if (phase != 4)
        return phase;
    ghostGetArg(self);
    u32 save = gabi::load<u32>(0x101F84DC);
    if (gabi::call<u8>(0x025B8BB0, ghostPtr(save + 0x644), 0x8803) == 3)
        return 5;
    if (!gabi::call<BOOL>(0x025D63E8, self, 0x0214A0D8, 0x1EA0))
        return 5;
    ghostCreateInit(self);
    return 4;
}
static BOOL ghostDelete(daGhostship_c *self) {
    WWHD_FUNC(0x0214A8B8, BOOL, self);
    gabi::call(0x025204C8, &self->phase, STR(0x1000FC84));
    gabi::call(0x025204C8, &self->clothPhase, STR(0x1000FC8C));
    u32 cloth = gabi::ea(self->cloth);
    if (cloth) {
        u32 target = gabi::load<u32>(gabi::load<u32>(cloth + 0xC) + 0xC);
        gabi::call_ptr<void>(target, ghostPtr(cloth), 3);
    }
    cloth = gabi::ea(self->cloth2);
    self->cloth = nullptr;
    if (cloth) {
        u32 target = gabi::load<u32>(gabi::load<u32>(cloth + 0xC) + 0xC);
        gabi::call_ptr<void>(target, ghostPtr(cloth), 3);
    }
    self->cloth2 = nullptr;
    return 1;
}
static void ghostModeProc(daGhostship_c *self) {
    WWHD_FUNC(0x0214A958, void, self);
    u32 entry = 0x1000FBF0 + (u32)self->mode * 8;
    s16 virtualSlot = gabi::load<s16>(entry + 2), adjust = gabi::load<s16>(entry);
    u32 adjusted = gabi::ea(self) + (s32)adjust, target;
    if (virtualSlot < 0)
        target = gabi::load<u32>(entry + 4);
    else {
        s16 vtableOffset = gabi::load<s16>(entry + 6);
        u32 vtable = gabi::load<u32>(adjusted + (s32)vtableOffset);
        target = gabi::load<u32>(vtable + (u32)virtualSlot * 8 + 4);
    }
    gabi::call_ptr<void>(target, ghostPtr(adjusted));
}
static void ghostClothParameters(u32 cloth, f32 vertical) {
    gabi::store<u16>(cloth + 0x1B6, 0x100);
    gabi::store<u16>(cloth + 0x1BA, 0);
    gabi::store<u16>(cloth + 0x1BC, 900);
    gabi::store<f32>(cloth + 0x1A0, 0.45f);
    gabi::store<f32>(cloth + 0x1A4, vertical);
    gabi::store<f32>(cloth + 0x1A8, 0.875f);
    gabi::store<f32>(cloth + 0x1AC, 1.0f);
    gabi::store<u16>(cloth + 0x1BE, (u16)-800);
    gabi::store<f32>(cloth + 0xE4, 7.0f);
    gabi::store<f32>(cloth + 0xE8, 6.0f);
    gabi::store<f32>(cloth + 0x1B0, 1.0f);
}
static void ghostClothMove(u32 cloth) {
    u32 target = gabi::load<u32>(gabi::load<u32>(cloth + 0xC) + 0x3C);
    gabi::call_ptr<void>(target, ghostPtr(cloth));
}
static BOOL ghostExecute(daGhostship_c *self) {
    WWHD_FUNC(0x0214A9A4, BOOL, self);
    u32 save = gabi::load<u32>(0x101F84DC);
    f32 time = gabi::load<f32>(save + 0x44);
    u32 play = gabi::call<u32>(0x025200D4);
    u32 player = gabi::load<u32>(play + 0x5B2C);
    f32 distance = gabi::call<f32>(0x025D6958, self, ghostPtr(player));
    self->canEnterShip = 0;
    u32 moon = gabi::call<u32>(0x02560828);
    // Unordered time takes the HD ble branch and remains in the active path.
    if ((u32)self->moonPhase != moon || (time > 90.0f && time < 285.0f)) {
        gabi::call(0x02560C5C);
        self->alpha = 0;
        self->actor_status = (u32)self->actor_status & ~0x23u;
    } else {
        gabi::call(0x02560C34);
        f32 hideDistance = gabi::load<f32>(0x10463F88);
        save = gabi::load<u32>(0x101F84DC);
        f32 target = distance < hideDistance ? 0.0f : gabi::load<f32>(0x10463F80);
        if (gabi::call<BOOL>(0x025B80C8, ghostPtr(save + 0xE4), 0x23)) {
            save = gabi::load<u32>(0x101F84DC);
            if (gabi::call<BOOL>(0x025B7F68, ghostPtr(save + 0xE4), 0x23)) {
                self->canEnterShip = 1;
                target = gabi::load<f32>(0x10463F80);
            }
        }
        gabi::call(0x0200F5C8, &self->alpha, target, 0.02f);
        if (self->alpha == 0.0f)
            self->actor_status = (u32)self->actor_status & ~0x23u;
        else {
            s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
            gabi::call(0x025E1A40, 0x107A, &self->current.pos, 0, reverb);
            self->actor_status = (u32)self->actor_status | 0x23;
        }
    }
    for (s32 i = 0; i < 12; i++) {
        auto &circle = self->circles[i];
        f32 radius = circle.radius, target;
        // HD bge takes unordered inputs; only an ordered radius below 900 selects this target.
        if (radius < 900.0f) {
            target = gabi::fmadds((f32)i, 100.0f, 1500.0f);
            self->radiusTargets[i] = target;
        } else if (radius > gabi::fmadds((f32)i, 100.0f, 1400.0f)) {
            target = 800.0f;
            self->radiusTargets[i] = target;
        } else
            target = self->radiusTargets[i];
        s32 phase = (s32)(s16)circle.angle + (s32)(s16)circle.angleSpeed;
        s16 doubled = (s16)((phase * 2) & 0xFFFE);
        f32 vertical = 600.0f * cM_scos(doubled);
        s32 direction = 1;
        if (i & 1) {
            vertical = 600.0f * cM_ssin(doubled);
            direction = -1;
        }
        gabi::call(0x0200ED84, &circle.radius, target, 0.1f, 10.0f);
        circle.wobbleAmplitude = gabi::load<f32>(0x1047BCD4) + 300.0f;
        s32 base = gabi::load<s16>(0x1047BD48) + 2 * i + 5;
        f32 offset = gabi::load<f32>(0x1047BCF8) + 500.0f;
        f32 random = cM_rndF(20.0f);
        circle.angleSpeed = (s16)gabi::ftoi(((f32)base + random) * (f32)direction);
        circle.translation.x =
            gabi::fmadds(offset, cM_ssin(self->shape_angle.y), (f32)self->current.pos.x);
        circle.translation.y = ((f32)self->current.pos.y + 600.0f) + vertical;
        circle.translation.z =
            gabi::fmadds(offset, cM_scos(self->shape_angle.y), (f32)self->current.pos.z);
        gabi::call(0x02587128, &circle);
        if (self->alpha != 0.0f) {
            play = gabi::call<u32>(0x025200D4);
            u32 particles = gabi::load<u32>(play + 0x5AB0);
            gabi::call(0x025A8D40, ghostPtr(particles), 0x8306, &circle.position, 255,
                       ghostPtr(0x101D5E98), ghostPtr(0x101D5E98), 0);
        }
    }
    if (self->alpha == gabi::load<f32>(0x10463F80) && distance < gabi::load<f32>(0x10463F8C)) {
        save = gabi::load<u32>(0x101F84DC);
        u32 event = gabi::call<u32>(0x025B8BB0, ghostPtr(save + 0x644), 0x8803);
        if (event < 3 && !self->enteredShip) {
            gabi::call(0x025E1A40, 0x2889, 0, 0, 0);
            u32 scene = gabi::call<u32>(0x02521678, &self->current.pos);
            if (!scene)
                gabi::call(0x0273AA24, STR(0x1000FB48), 0x1D3, STR(0x1000FB5C));
            save = gabi::load<u32>(0x101F84DC);
            u8 room = gabi::load<u8>(scene + 9), start = gabi::load<u8>(scene + 8);
            gabi::call(0x025B8AF4, ghostPtr(save + 0x644), 0xC3FF, room);
            save = gabi::load<u32>(0x101F84DC);
            gabi::call(0x025B8AF4, ghostPtr(save + 0x644), 0x85FF, start);
            gabi::call(0x0252012C, STR(0x1000FAD8), 0, 2, -1, 0, 1, 0, 0.0f);
            self->enteredShip = 1;
        }
    }
    ghostModeProc(self);
    gabi::call(0x025E742C, &self->btk);
    ghostMatrices(self);
    u32 cloth = gabi::ea(self->cloth);
    ghostClothParameters(cloth, -1.5f);
    u32 wind = gabi::call<u32>(0x0257DAA8);
    gabi::call(0x0251E7A8, (u8 *)self->cloth, ghostPtr(wind));
    ghostClothMove(gabi::ea(self->cloth));
    cloth = gabi::ea(self->cloth2);
    ghostClothParameters(cloth, -0.25f);
    wind = gabi::call<u32>(0x0257DAA8);
    gabi::Local<cXyz> secondWind;
    gabi::Local<u8[16]> linkage;
    secondWind->x = gabi::load<f32>(wind);
    secondWind->y = gabi::load<f32>(wind + 4);
    secondWind->z = gabi::load<f32>(wind + 8);
    PSVECScale(secondWind.get(), secondWind.get(), 0.548f);
    secondWind->y = -0.83f;
    gabi::call(0x0251E7A8, (u8 *)self->cloth2, secondWind.get());
    ghostClothMove(gabi::ea(self->cloth2));
    return 0;
}
struct GhostshipColor_l {
    be<f32> r, g, b, a;
};
static void ghostNormalize(GhostshipColor_l *out, u8 *color) {
    WWHD_FUNC(0x0214B3F4, void, out, color);
    u32 a = gabi::ea(color);
    f32 r = (f32)gabi::load<u8>(a) / 255.0f, g = (f32)gabi::load<u8>(a + 1) / 255.0f,
        b = (f32)gabi::load<u8>(a + 2) / 255.0f, alpha = (f32)gabi::load<u8>(a + 3) / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = alpha;
}
static void ghostCopyGuard(u32 guard, u32 destination, u32 source) {
    if (!gabi::load<u32>(guard)) {
        gabi::store<u32>(guard, 1);
        gabi::call(0xC000A848, ghostPtr(destination), ghostPtr(source), 4);
    }
}
static u32 ghostKColor(u32 material) {
    u32 block = gabi::load<u32>(material + 0x18),
        target = gabi::load<u32>(gabi::load<u32>(block + 4) + 0x4C);
    return gabi::call_ptr<u32>(target, ghostPtr(block), 3);
}
static BOOL ghostDraw(daGhostship_c *self) {
    WWHD_FUNC(0x0214B4A8, BOOL, self);
    if (gabi::load<u8>(0x10463F7C)) {
        ghostCopyGuard(0x101FDAC0, 0x101FEBF8, 0x1000FAC8);
        ghostCopyGuard(0x101FDA50, 0x101FEBF4, 0x1000FACC);
        ghostCopyGuard(0x101FDA50, 0x101FEBF4, 0x1000FACC);
        ghostCopyGuard(0x101FDA50, 0x101FEBF4, 0x1000FACC);
        ghostCopyGuard(0x101FDA48, 0x101FEBEC, 0x1000FAD0);
        ghostCopyGuard(0x101FDAC0, 0x101FEBF8, 0x1000FAC8);
        ghostCopyGuard(0x101FDAC0, 0x101FEBF8, 0x1000FAC8);
        ghostCopyGuard(0x101FDAC0, 0x101FEBF8, 0x1000FAC8);
    }
    if (self->alpha == 0.0f || gabi::load<u8>(0x10463F7D))
        return 1;
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, ghostPtr(env), 0, &self->current.pos, ghostPtr(gabi::ea(self) + 0x110));
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, ghostPtr(env), (J3DModel *)self->model,
               ghostPtr(gabi::ea(self) + 0x110));
    u8 alpha = (u8)gabi::ftoi(255.5f * (f32)self->alpha);
    u32 data = gabi::load<u32>(gabi::ea(self->model) + 0xAC);
    u32 table = gabi::call<u32>(0x027F3F8C, ghostPtr(data));
    for (u8 i = 0; i < gabi::load<u16>(table + 0x24); i = (u8)(i + 1)) {
        u32 count = gabi::load<u32>(data + 0xC), material = gabi::load<u32>(data + 0x10);
        if ((u32)i < count)
            material += (u32)i * 0x39C;
        u32 color = ghostKColor(material);
        gabi::store<u8>(color + 3, alpha);
        color = ghostKColor(material);
        u32 block = gabi::load<u32>(material + 0x18),
            target = gabi::load<u32>(gabi::load<u32>(block + 4) + 0x3C);
        gabi::call_ptr<void>(target, ghostPtr(block), 3, ghostPtr(color));
        gabi::Local<GhostshipColor_l> input, converted;
        gabi::Local<u8[16]> linkage;
        ghostNormalize(input.get(), ghostPtr(color));
        gabi::call(0x0274D458, converted.get(), input.get(), 1.0f);
        gabi::store<u32>(material + 0xA0, gabi::load<u32>(material + 0xA0) | 0x400);
        u32 output = gabi::call<u32>(0x027F9F0C, ghostPtr(material + 0xA0), 10);
        f32 savedAlpha = (f32)gabi::load<u8>(color + 3) / 255.0f;
        f32 r = converted->r, g = converted->g, b = converted->b;
        gabi::store<f32>(output + 4, g);
        gabi::store<f32>(output + 8, b);
        gabi::store<f32>(output, r);
        gabi::store<f32>(output + 12, savedAlpha);
        table = gabi::call<u32>(0x027F3F8C, ghostPtr(data));
    }
    gabi::call(0x025E7FC4, &self->btk, ghostPtr(data), (f32)self->btk.mFrameCtrl.mFrame);
    gabi::call(0x025E2DE0, (J3DModel *)self->model, 0);
    gabi::store<u32>(data + 0x44, 0);
    u32 cloth = gabi::ea(self->cloth);
    gabi::store<u16>(gabi::ea(self) + 0x1A6, alpha);
    u32 target = gabi::load<u32>(gabi::load<u32>(cloth + 0xC) + 0x44);
    gabi::call_ptr<void>(target, ghostPtr(cloth));
    cloth = gabi::ea(self->cloth2);
    gabi::store<u16>(gabi::ea(self) + 0x1A6, alpha);
    target = gabi::load<u32>(gabi::load<u32>(cloth + 0xC) + 0x44);
    gabi::call_ptr<void>(target, ghostPtr(cloth));
    return 1;
}
static void ghostPathMove(daGhostship_c *self) {
    WWHD_FUNC(0x0214B834, void, self);
    gabi::call(0x0200ED84, &self->speedF, (f32)self->pathSpeed, 0.1f, 2.0f);
    gabi::call(0x02587D24, &self->pathPos, &self->pointNo, (u8 *)self->path, (f32)self->speedF,
               0x0214A2B4, self);
    f32 factor = gabi::load<f32>(0x1047BCD0) + 0.01f;
    gabi::call(0x0200F268, &self->current.pos, &self->pathPos, factor, (f32)self->speedF);
    if (self->speedF != 0.0f && self->pathSpeed != 0.0f) {
        s16 angle = gabi::call<s16>(0x0200F93C, &self->current.pos, &self->pathPos);
        gabi::call(0x0200F428, &self->shape_angle.y, angle, 8, 0x100);
    }
}
static void ghostPathMode(daGhostship_c *self) {
    WWHD_FUNC(0x0214B8FC, void, self);
    if (self->pathNo != 255) {
        self->pathSpeed = 10.0f;
        ghostPathMove(self);
    }
    self->current.pos.y = gabi::call<f32>(0x025871F8, &self->current.pos, &self->acch);
}
static u8 *ghostHioCtor(u8 *self) {
    WWHD_FUNC(0x0214B954, u8 *, self);
    if (!self) {
        self = gabi::call<u8 *>(0x0273AD10, 0x18);
        if (!self)
            return nullptr;
    }
    u32 a = gabi::ea(self);
    gabi::store<f32>(a + 8, 0.51f);
    gabi::store<f32>(a + 0x10, 3000.0f);
    gabi::store<u8>(a + 4, 0);
    gabi::store<u32>(a, 0x1000FB38);
    gabi::store<f32>(a + 0xC, 5.0f);
    gabi::store<u8>(a + 5, 0);
    gabi::store<f32>(a + 0x14, 1500.0f);
    return self;
}
static void ghostStaticInit() {
    WWHD_FUNC(0x0214B9D0, void);
    gabi::store<u32>(0x10463F98, 0);
    gabi::store<u32>(0x10463F90, 0);
    gabi::store<u32>(0x10463F9C, 0);
    gabi::store<u32>(0x10463F94, 0);
    gabi::call(0x028F026C, ghostPtr(0x101B529C));
    gabi::store<f32>(0x10463F6C, -3.1415927410125732f);
    gabi::store<f32>(0x10463F70, 3.1415927410125732f);
    gabi::call(0x028ED6F8, ghostPtr(0x10463F74));
    gabi::call(0x028F026C, ghostPtr(0x101B52A8));
    gabi::call(0x028EAB2C, ghostPtr(0x10463F75));
    gabi::call(0x028F026C, ghostPtr(0x101B52B4));
    ghostHioCtor(ghostPtr(0x10463F78));
}
static void ghostStringDtor(u8 *self, s32 flags) {
    WWHD_FUNC(0x0214BA70, void, self, flags);
    if (self && (flags & 1))
        gabi::call(0x0273AF40, self);
}
static BOOL ghostIsDelete(daGhostship_c *self) {
    WWHD_FUNC(0x0214BA84, BOOL, self);
    return 1;
}
static void ghostWait(daGhostship_c *self) { WWHD_FUNC(0x0214BA8C, void, self); }
static void ghostRealize(daGhostship_c *self) { WWHD_FUNC(0x0214BA90, void, self); }
static void ghostActorDtor(daGhostship_c *self, s32 flags) {
    WWHD_FUNC(0x0214BA94, void, self, flags);
    if (self) {
        gabi::call(0x02018034, ghostPtr(gabi::ea(self) + 0x654), 2);
        gabi::store<u32>(gabi::ea(self) + 0x49C, 0x1000FB08);
        gabi::store<u32>(gabi::ea(self) + 0x490, 0x1000FB18);
        gabi::call(0x024EFD9C, &self->acch, 0);
        gabi::call(0x025D50BC, self, 0);
        if (flags & 1)
            gabi::call(0x0273AF40, self);
    }
}
static void ghostStringVirtual() { WWHD_FUNC(0x0214BB18, void); }

VERIFY(0x02149ED4, ghostSailFactor);
VERIFY(0x02149EE0, ghostHeap);
VERIFY(0x0214A0D8, ghostHeapCB);
VERIFY(0x0214A0DC, ghostPathStep);
VERIFY(0x0214A2B4, ghostPathCB);
VERIFY(0x0214A2CC, ghostGetArg);
VERIFY(0x0214A2E0, ghostPathInit);
VERIFY(0x0214A2EC, ghostWaitInit);
VERIFY(0x0214A2F8, ghostCreateInit);
VERIFY(0x0214A758, ghostCreate);
VERIFY(0x0214A8B8, ghostDelete);
VERIFY(0x0214A958, ghostModeProc);
VERIFY(0x0214A9A4, ghostExecute);
VERIFY(0x0214B3F4, ghostNormalize);
VERIFY(0x0214B4A8, ghostDraw);
VERIFY(0x0214B834, ghostPathMove);
VERIFY(0x0214B8FC, ghostPathMode);
VERIFY(0x0214B954, ghostHioCtor);
VERIFY(0x0214B9D0, ghostStaticInit);
VERIFY(0x0214BA70, ghostStringDtor);
VERIFY(0x0214BA84, ghostIsDelete);
VERIFY(0x0214BA8C, ghostWait);
VERIFY(0x0214BA90, ghostRealize);
VERIFY(0x0214BA94, ghostActorDtor);
VERIFY(0x0214BB18, ghostStringVirtual);
