/**
 * d_a_gm.cpp (WWHD)
 * Mini-boss - Mothula / enemy - Mothula larva.
 * Written from the WWHD code (the GameCube source of this unit is all "Nonmatching" stubs).
 * Verified against cking.rpx. Most of the actor tail is still addressed by raw offsets.
 */
#include "bindings.h"
#include "d/actor/d_a_gm.h"

// Typed accessors for the incompletely named GM tail; offsets come from the HD TU.
template<class T> static inline be<T>& gmField(gm_class* actor, u32 offset) {
    return *gabi::at<be<T>>(gabi::ea(actor) + offset);
}
template<class T = void> static inline T* gmPart(gm_class* actor, u32 offset) {
    return gabi::at<T>(gabi::ea(actor) + offset);
}
static inline void* gmPointer(gm_class* actor, u32 offset) {
    return gabi::at<void>((u32)gmField<u32>(actor, offset));
}

static inline void* gm_play() {
    // Real HD getter saves LR in the caller's outgoing linkage area.
    gabi::Local<cXyz> linkage;
    return gabi::call<void*>(0x025200D4);
}

static inline void gm_lineSet(dBgS_LinChk* check, cXyz* start, cXyz* end, gm_class* actor) {
    gabi::Local<cXyz> linkage;
    gabi::call(0x024F1AFC, check, start, end, actor);
}

BOOL daGM_IsDelete(gm_class* actor) {
    WWHD_FUNC(0x0214FFD0, BOOL, actor);
    return TRUE;
}
VERIFY(0x0214FFD0, daGM_IsDelete);

void BG_check(gm_class* actor) {
    WWHD_FUNC(0x02151100, void, actor);
    f32 wallHeight = gmField<f32>(actor, 0x448);
    f32 wallRadius = gmField<f32>(actor, 0x44C);
    gabi::call(0x024EFF44, gmPart(actor, 0x584), wallHeight, wallRadius);
    gabi::call(0x028E8D88, &actor->current.pos, gmPart<cXyz>(actor, 0x4D0), &actor->current.pos);
    f32 oldY = (f32)actor->old.pos.y + (f32)gmField<f32>(actor, 0x4D4);
    f32 y = actor->current.pos.y;
    f32 offset = gmField<f32>(actor, 0x444);
    actor->current.pos.y = y - offset;
    actor->old.pos.y = oldY - offset;
    void* play = gm_play();
    gabi::call(0x024F08A8, gmPart(actor, 0x5C4), gabi::at<void>(gabi::ea(play) + 0x12A0));
    y = actor->current.pos.y;
    offset = gmField<f32>(actor, 0x444);
    oldY = actor->old.pos.y;
    actor->current.pos.y = y + offset;
    actor->old.pos.y = oldY + offset;
    gabi::call(0x028E8DAC, &actor->current.pos, gmPart<cXyz>(actor, 0x4D0), &actor->current.pos);
    oldY = actor->old.pos.y;
    offset = gmField<f32>(actor, 0x4D4);
    actor->old.pos.y = oldY - offset;
}
VERIFY(0x02151100, BG_check);

void wing_ret_set(gm_class* actor) {
    WWHD_FUNC(0x021511B8, void, actor);
    for (u32 i = 0; i < 4; ++i) {
        u32 base = 0x530 + i * 6;
        gabi::call(0x0200F428, gmPart<be<s16>>(actor, base + 2), 0, 1, 0x200);
        gabi::call(0x0200F428, gmPart<be<s16>>(actor, base), 0, 1, 0x200);
        gabi::call(0x0200F428, gmPart<be<s16>>(actor, base + 4), 0, 1, 0x200);
    }
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x54A), 0, 1, 0x200);
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x548), 0, 1, 0x200);
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x54C), 0, 1, 0x200);
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x556), 0, 1, 0x200);
}
VERIFY(0x021511B8, wing_ret_set);

f32 gm_cosLookup(void* table, u16 angle) {
    WWHD_FUNC(0x02151294, f32, table, angle);
    return *gabi::at<be<f32>>(gabi::ea(table) + (angle >> 3) * 8 + 4);
}
VERIFY(0x02151294, gm_cosLookup);

void gm_staticDestructor(void* object, s32 flags) {
    WWHD_FUNC(0x021510EC, void, object, flags);
    if (gabi::ea(object) && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x021510EC, gm_staticDestructor);

void gm_emptyVirtual(gm_class* actor) {
    WWHD_FUNC(0x02154138, void, actor);
}
VERIFY(0x02154138, gm_emptyVirtual);

void anm_init(gm_class* actor, s32 animation, f32 morph, u8 mode, f32 speed, s32 auxiliary) {
    WWHD_FUNC(0x0214C1C8, void, actor, animation, morph, mode, speed, auxiliary);
    gmField<s32>(actor, 0x460) = animation;
    gabi::Local<SafeString> resourceName;
    resourceName->mStringTop = 0x1000FDF4;
    resourceName->__vtbl = 0x1000FC9C;
    void* resourceControl = gabi::at<void>(*gabi::at<be<u32>>(0x101F4F28));
    void* mainAnimation = gabi::call<void*>(0x026066C4, resourceControl, resourceName.get(), animation);
    void* secondary = gabi::at<void>(0);
    if (auxiliary >= 0) {
        gabi::Local<SafeString> secondaryName;
        secondaryName->mStringTop = 0x1000FDF4;
        secondaryName->__vtbl = 0x1000FC9C;
        resourceControl = gabi::at<void>(*gabi::at<be<u32>>(0x101F4F28));
        secondary = gabi::call<void*>(0x026066C4, resourceControl, secondaryName.get(), auxiliary);
    }
    gabi::call(0x025E4A98, gmPointer(actor, 0x3D0), mainAnimation, mode, secondary, morph, speed, 0.0f, -1.0f);
}
VERIFY(0x0214C1C8, anm_init);

void* gm_fireConstructor(void* object) {
    WWHD_FUNC(0x02150578, void*, object);
    u32 base = gabi::ea(object);
    if (!base) {
        object = gabi::call<void*>(0x0273AD10, 0x22C);
        base = gabi::ea(object);
        if (!base) return object;
    }
    if (!(base + 0x8C)) gabi::call<void*>(0x0273AD10, 12);
    gabi::call(0x0200BD2C, gabi::at<void>(base + 0xA0));
    gabi::call(0x02515DA0, gabi::at<void>(base + 0xBC));
    *gabi::at<be<u32>>(base + 0xB8) = 0x1004AE88;
    *gabi::at<be<u32>>(base + 0xBC) = 0x1004AEC0;
    gabi::call(0x025166F0, gabi::at<void>(base + 0xDC));
    *gabi::at<be<f32>>(base + 0x228) = 1.0f;
    return object;
}
VERIFY(0x02150578, gm_fireConstructor);

// Each cylinder's inline geometry constructors write this TU's vtables.
static inline void gm_cylinderConstructor(gm_class* actor, u32 offset) {
    gabi::call(0x02515FB8, gmPart(actor, offset));
    gmField<u32>(actor, offset + 0x114) = 0x100015A8;
    gmField<u32>(actor, offset + 0x110) = 0x1000FCB4;
    gabi::call(0x02018590, gmPart(actor, offset + 0x118));
    gmField<u32>(actor, offset + 0x3C) = 0x1004B108;
    gmField<u32>(actor, offset + 0x12C) = 0x1004B150;
    gmField<u32>(actor, offset + 0x114) = 0x1004B160;
}

gm_class* gm_constructor(gm_class* actor) {
    WWHD_FUNC(0x02150604, gm_class*, actor);
    if (!gabi::ea(actor)) {
        actor = gabi::call<gm_class*>(0x0273AD10, 0x1290);
        if (!gabi::ea(actor)) return actor;
    }
    gabi::call(0x025D4ED0, actor);
    actor->__vtbl = 0x1000FDD4;
    gabi::call(0x024EFE94, gmPart(actor, 0x584));
    gabi::call(0x024F0474, gmPart(actor, 0x5C4));
    gmField<u32>(actor, 0x5D4) = 0x1000FCE4;
    gmField<u32>(actor, 0x5E4) = 0x1000FCF4;
    gmField<u32>(actor, 0x5D8) = 0x1000FD04;
    gmField<u8>(actor, 0x5DC) = 1;
    gabi::call(0x0200BD2C, gmPart(actor, 0x788));
    gabi::call(0x02515DA0, gmPart(actor, 0x7A4));
    gmField<u32>(actor, 0x7A0) = 0x1004AE88;
    gmField<u32>(actor, 0x7A4) = 0x1004AEC0;
    gm_cylinderConstructor(actor, 0x7C4);
    gm_cylinderConstructor(actor, 0x8F4);
    gm_cylinderConstructor(actor, 0xA24);
    gabi::call(0x025166F0, gmPart(actor, 0xB54));
    gabi::call(0x0200BD2C, gmPart(actor, 0xCD4));
    gabi::call(0x02515DA0, gmPart(actor, 0xCF0));
    gmField<u32>(actor, 0xCEC) = 0x1004AE88;
    gmField<u32>(actor, 0xCF0) = 0x1004AEC0;
    gm_cylinderConstructor(actor, 0xD10);
    gabi::call(0x024EFE94, gmPart(actor, 0xE58));
    gabi::call(0x024F0474, gmPart(actor, 0xE98));
    gmField<u8>(actor, 0xEB0) = 1;
    gmField<u32>(actor, 0xEB8) = 0x1000FCF4;
    gmField<u32>(actor, 0xEA8) = 0x1000FCE4;
    gmField<u32>(actor, 0xEAC) = 0x1000FD04;
    gm_fireConstructor(gmPart(actor, 0x105C));
    gabi::call(0x025E895C, gmPart(actor, 0x1288));
    return actor;
}
VERIFY(0x02150604, gm_constructor);

void gm_destructor(gm_class* actor, s32 flags) {
    WWHD_FUNC(0x0215400C, void, actor, flags);
    if (!gabi::ea(actor)) return;
    gabi::call(0x025E89F8, gmPart(actor, 0x1288), 2);
    gabi::call(0x02515AE8, gmPart(actor, 0x1138), 2);
    gabi::call(0x02515860, gmPart(actor, 0x10FC), 2);
    gmField<u32>(actor, 0xEB8) = 0x1000FCF4;
    gmField<u32>(actor, 0xEAC) = 0x1000FD04;
    gabi::call(0x024EFD9C, gmPart(actor, 0xE98), 0);
    gabi::call(0x02018034, gmPart(actor, 0xE6C), 2);
    gabi::call(0x02515A70, gmPart(actor, 0xD10), 2);
    gabi::call(0x02515860, gmPart(actor, 0xCD4), 2);
    gabi::call(0x02515AE8, gmPart(actor, 0xB54), 2);
    gabi::call(0x02515A70, gmPart(actor, 0xA24), 2);
    gabi::call(0x02515A70, gmPart(actor, 0x8F4), 2);
    gabi::call(0x02515A70, gmPart(actor, 0x7C4), 2);
    gabi::call(0x02515860, gmPart(actor, 0x788), 2);
    gmField<u32>(actor, 0x5E4) = 0x1000FCF4;
    gmField<u32>(actor, 0x5D8) = 0x1000FD04;
    gabi::call(0x024EFD9C, gmPart(actor, 0x5C4), 0);
    gabi::call(0x02018034, gmPart(actor, 0x598), 2);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x0215400C, gm_destructor);

void gm_staticInit() {
    WWHD_FUNC(0x02150FF8, void);
    *gabi::at<be<u32>>(0x10464004) = 0;
    *gabi::at<be<u32>>(0x10464000) = 0;
    *gabi::at<be<u32>>(0x10463FFC) = 0;
    *gabi::at<be<u32>>(0x10463FF8) = 0;
    gabi::call(0x028F026C, gabi::at<void>(0x101B5474));
    *gabi::at<be<f32>>(0x10463FA0) = -3.1415927410125732f;
    *gabi::at<be<f32>>(0x10463FA4) = 3.1415927410125732f;
    gabi::call(0x028ED6F8, gabi::at<void>(0x10463FAC));
    gabi::call(0x028F026C, gabi::at<void>(0x101B5480));
    gabi::call(0x028EAB2C, gabi::at<void>(0x10463FAD));
    gabi::call(0x028F026C, gabi::at<void>(0x101B548C));
    gabi::call(0x0201A478, gabi::at<csXyz>(0x10463FE0), 10000, -12000, -2000);
    gabi::call(0x0201A478, gabi::at<csXyz>(0x10463FE6), -10000, 12000, -2000);
    gabi::call(0x0201A478, gabi::at<csXyz>(0x10463FEC), -2000, -12000, 2000);
    gabi::call(0x0201A478, gabi::at<csXyz>(0x10463FF2), 2000, 12000, 2000);
}
VERIFY(0x02150FF8, gm_staticInit);

// Matrix loads precede all stores, including when source and destination overlap.
static inline void gm_copyMatrix(u32 source, u32 destination) {
    f32 elements[12];
    for (u32 i = 0; i < 12; ++i) elements[i] = *gabi::at<be<f32>>(source + i * 4);
    for (u32 i = 0; i < 12; ++i) *gabi::at<be<f32>>(destination + i * 4) = elements[i];
}
static inline void gm_rotate(u32 matrix, u32 function, s16 angle) {
    gabi::call(function, gabi::at<void>(matrix), angle);
}
static inline u32 gm_currentMatrix() { return *gabi::at<be<u32>>(0x1018C7B0); }

BOOL nodeCallBack(void* node, s32 phase) {
    WWHD_FUNC(0x0214BB1C, BOOL, node, phase);
    if (phase != 0) return TRUE;
    u32 model = *gabi::at<be<u32>>(0x104B462C);
    gm_class* actor = gabi::at<gm_class>(*gabi::at<be<u32>>(model + 0xB8));
    void* joint = gabi::call<void*>(0x027F7878, node);
    u16 index = *gabi::at<be<u16>>(gabi::ea(joint) + 4);
    if (!gabi::ea(actor) || (u8)gmField<u8>(actor, 0x3E9) != 0) return TRUE;
    u32 matrixObject = *gabi::at<be<u32>>(model + 0x2C);
    u32 matrices = *gabi::at<be<u32>>(matrixObject + 0x10);
    *gabi::at<be<u16>>(matrixObject + 4) = (u16)*gabi::at<be<u16>>(matrixObject + 4) | 0x10;
    gabi::call(0x028E90D4, gabi::at<void>(matrices + index * 48), gabi::at<void>(gm_currentMatrix()));
    gabi::Local<cXyz> origin;
    if (index == 3 || index == 4 || index == 5) {
        origin->x = 0.0f; origin->y = 0.0f; origin->z = 0.0f;
        u32 position = index == 3 ? 0x518 : index == 4 ? 0x50C : 0x500;
        gabi::call(0x0200FCD8, origin.get(), gmPart<cXyz>(actor, position));
        if (index == 5) {
            origin->x = 70.0f;
            gabi::call(0x0200FCD8, origin.get(), gmPart<cXyz>(actor, 0x4C4));
        }
        if (index == 4) {
            gm_rotate(gm_currentMatrix(), 0x025F1C28, gmField<s16>(actor, 0x556));
            gm_rotate(gm_currentMatrix(), 0x025F1BF4, gmField<s16>(actor, 0x554));
            gm_rotate(gm_currentMatrix(), 0x025F1C5C, gmField<s16>(actor, 0x558));
        }
    } else if (index == 12 || index == 14 || index == 16 || index == 18) {
        u32 offset = 0x530 + ((index - 12) / 2) * 6;
        gm_rotate(gm_currentMatrix(), 0x025F1C28, gmField<s16>(actor, offset + 2));
        gm_rotate(gm_currentMatrix(), 0x025F1BF4, gmField<s16>(actor, offset));
        gm_rotate(gm_currentMatrix(), 0x025F1C5C, gmField<s16>(actor, offset + 4));
    }
    if (index != 12 && index != 14 && index != 16 && index != 18) {
        gm_rotate(gm_currentMatrix(), 0x025F1C28, gmField<s16>(actor, 0x54A));
        gm_rotate(gm_currentMatrix(), 0x025F1BF4, gmField<s16>(actor, 0x548));
        gm_rotate(gm_currentMatrix(), 0x025F1C5C, gmField<s16>(actor, 0x54C));
    }
    matrixObject = *gabi::at<be<u32>>(model + 0x2C);
    u32 source = gm_currentMatrix();
    matrices = *gabi::at<be<u32>>(matrixObject + 0x10);
    *gabi::at<be<u16>>(matrixObject + 4) = (u16)*gabi::at<be<u16>>(matrixObject + 4) | 0x10;
    gm_copyMatrix(source, matrices + index * 48);
    gabi::call(0x028E90D4, gabi::at<void>(gm_currentMatrix()), gabi::at<void>(0x104B4868));
    return TRUE;
}
VERIFY(0x0214BB1C, nodeCallBack);

void draw_SUB(gm_class* actor) {
    WWHD_FUNC(0x0214BDFC, void, actor);
    u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
    u32 model = *gabi::at<be<u32>>(morf + 0x90);
    f32 scaleX = actor->scale.x, scaleY = actor->scale.y, scaleZ = actor->scale.z;
    *gabi::at<be<f32>>(model + 0xBC) = scaleX;
    *gabi::at<be<f32>>(model + 0xC4) = scaleZ;
    *gabi::at<be<f32>>(model + 0xC0) = scaleY;
    f32 x = (f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0);
    f32 y = (f32)actor->current.pos.y + (f32)gmField<f32>(actor, 0x4D4);
    f32 z = (f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8);
    gabi::call(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
    gm_rotate(0x1048D0CC, 0x025F1BF4, gmField<s16>(actor, 0x55A));
    gm_rotate(0x1048D0CC, 0x025F1C5C, gmField<s16>(actor, 0x55E));
    gm_rotate(0x1048D0CC, 0x025F1C28, actor->shape_angle.y);
    gm_rotate(0x1048D0CC, 0x025F1BF4, actor->shape_angle.x);
    gm_rotate(0x1048D0CC, 0x025F1C28, gmField<s16>(actor, 0x43C));
    gm_rotate(0x1048D0CC, 0x025F1C5C, actor->shape_angle.z);
    gm_copyMatrix(0x1048D0CC, model + 0xC8);
    if ((u8)gmField<u8>(actor, 0x3E9) == 0) {
        gabi::call(0x025E55A0, gmPointer(actor, 0x3D0));
        gabi::call(0x02041570, gmPart(actor, 0x105C));
    }
    void* environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4, environment, 0, &actor->current.pos, &actor->tevStr);
}
VERIFY(0x0214BDFC, draw_SUB);

BOOL ks_set_rtn(gm_class* actor) {
    WWHD_FUNC(0x0214CFA4, BOOL, actor);
    s16 count = gmField<s16>(actor, 0x43A);
    if (count >= ((u8)gmField<u8>(actor, 0x3EA) == 1 ? 10 : 20)) return TRUE;
    s32 parameters = (s16)gmField<s16>(actor, 0x3F8) == 83 ? 5 : 4;
    if (gabi::ea(actor) && gabi::ea(&actor->eyePos)) {
        s8 room = actor->current.roomNo;
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1A40, 0x58A6, &actor->eyePos, 0, reverb);
    }
    gabi::Local<csXyz> angles;
    angles->x = actor->current.angle.x;
    s16 angleY = actor->current.angle.y;
    angles->y = angleY;
    angles->z = actor->current.angle.z;
    f32 randomAngle = gabi::call<f32>(0x02019918, 4096.0f);
    angles->y = (s16)gabi::ftoi((f32)angleY + randomAngle);
    u32 parent = gabi::ea(actor) ? (u32)gmField<u32>(actor, 4) : 0xFFFFFFFF;
    s8 room = actor->current.roomNo;
    s32 created = gabi::call<s32>(0x025D5A20, 0xCD, parent, parameters, gmPart<cXyz>(actor, 0x518), room, angles.get(), &actor->scale, 0, 0);
    if (created == -1) return TRUE;
    gmField<s16>(actor, 0x43A) = (s16)((s16)gmField<s16>(actor, 0x43A) + 1);
    return FALSE;
}
VERIFY(0x0214CFA4, ks_set_rtn);

void action_kabehari(gm_class* actor) {
    WWHD_FUNC(0x021512AC, void, actor);
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    gabi::Local<cXyz> target;
    target->z = 0.0f; target->y = 0.0f; target->x = 0.0f;
    s16 state = gmField<s16>(actor, 0x3F8);
    if (state == 40) {
        gmField<f32>(actor, 0x44C) = 70.0f;
        for (u32 i = 0; i < 23; ++i) gmField<s16>(actor, 0x40C + i * 2) = 0;
        gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) & ~1u;
        state = gmField<s16>(actor, 0x3F8);
        gmPart<cXyz>(actor, 0x464)->copy(player->current.pos);
        f32 y = player->current.pos.y;
        gmField<s16>(actor, 0x3F8) = (s16)(state + 1);
        gmField<s16>(actor, 0x43C) = 0;
        gmField<s16>(actor, 0x400) = 50;
        gmField<f32>(actor, 0x468) = y + 450.0f;
    }
    if (state == 40 || state == 41) {
        gabi::call(0x0200F428, &actor->shape_angle.x, -16384, 1, 0x200);
        f32 dy = (f32)actor->current.pos.y - (f32)gmField<f32>(actor, 0x468);
        f32 dx = (f32)actor->current.pos.x - (f32)target->x;
        f32 dz = (f32)actor->current.pos.z - (f32)target->z;
        f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
        if (distance < 4.0f || ((u32)gmField<u32>(actor, 0x5EC) & 0x10) || (s16)gmField<s16>(actor, 0x400) == 0) {
            if ((s32)gmField<s32>(actor, 0x460) != 29) anm_init(actor, 29, 10.0f, 2, 1.0f, -1);
            gmField<f32>(actor, 0x44C) = 10.0f;
            actor->shape_angle.x = -16384;
            gmField<s16>(actor, 0x3F8) = 42;
        }
    } else if (state == 42) {
        gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x43C), -32768, 1, 0x500);
        s16 difference = (s16)gabi::call<s32>(0x0200FAAC, (s16)gmField<s16>(actor, 0x43C), -32768);
        if (difference < 0x100) {
            gmField<s16>(actor, 0x3FE) = 10;
            gmField<s16>(actor, 0x3F8) = 43;
        }
    } else if (state == 43) {
        gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x43C), -32768, 1, 0x500);
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            if ((s16)gmField<s16>(actor, 0x43E) <= 1 && ((u8)gmField<u8>(actor, 0x3E8) & 7) != 7) {
                play = gm_play();
                void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
                s16 angle = gabi::call<s16>(0x025D6894, actor, other);
                actor->shape_angle.y = angle;
                actor->current.angle.y = angle;
                gmField<u8>(actor, 0x3EC) = 3;
                gmField<s16>(actor, 0x3F8) = 33;
            } else {
                gmField<u8>(actor, 0x3EC) = 0;
                gmField<s16>(actor, 0x3F8) = 0;
            }
        }
    }
    if ((u8)gmField<u8>(actor, 0x3EC) != 4) return;
    s16 angle = (s16)((s16)actor->current.angle.y - 32768);
    gabi::call(0x025F1884, gabi::at<void>(gm_currentMatrix()), angle);
    gabi::Local<cXyz> direction;
    direction->x = 0.0f; direction->y = 0.0f; direction->z = 60.0f;
    gabi::call(0x0200FCD8, direction.get(), target.get());
    gabi::call(0x028E8D88, target.get(), gmPart<cXyz>(actor, 0x4B8), target.get());
    f32 x = target->x;
    gabi::call(0x0200ED84, &actor->current.pos.x, x, 1.0f, 10.0f);
    f32 y = gmField<f32>(actor, 0x468);
    gabi::call(0x0200ED84, &actor->current.pos.y, y, 1.0f, 10.0f);
    f32 z = target->z;
    gabi::call(0x0200ED84, &actor->current.pos.z, z, 1.0f, 10.0f);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, gmPart<cXyz>(actor, 0x4A0), difference.get(), gmPart<cXyz>(actor, 0x4AC));
    f32 dx = difference->x, dy = difference->y, dz = difference->z;
    direction->x = dx; direction->y = dy; direction->z = dz;
    angle = (s16)((s16)gabi::call<s16>(0x020195B0, dx, dz) + 0x4000);
    actor->current.angle.y = angle;
    gabi::call(0x0200F428, &actor->shape_angle.y, angle, 1, 0x1000);
}
VERIFY(0x021512AC, action_kabehari);

static inline f32 gm_sin(s16 angle) {
    return *gabi::at<be<f32>>(0x104A44F8 + ((u16)angle >> 3) * 8);
}
static inline f32 gm_cos(s16 angle) {
    return *gabi::at<be<f32>>(0x104A44FC + ((u16)angle >> 3) * 8);
}
void action_fly_damage(gm_class* actor) {
    WWHD_FUNC(0x0215163C, void, actor);
    gm_play();
    s16 state = gmField<s16>(actor, 0x3F8);
    bool animateWings = state == 51;
    if (state == 50) {
        gmField<u32>(actor, 0x7F0) = ((u32)gmField<u32>(actor, 0x7F0) & ~0x70u) | 0x70u;
        if ((u8)gmField<u8>(actor, 0x3ED) != 4) {
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3E0)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 1;
        } else {
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 0;
        }
        anm_init(actor, 28, 1.0f, 0, 1.0f, -1);
        gmField<s16>(actor, 0x43C) = 0;
        actor->speedF = 30.0f;
    }
    if (state == 50 || state == 52) {
        gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) & ~1u;
        for (u32 i = 0; i < 23; ++i) gmField<s16>(actor, 0x40C + i * 2) = 0;
        void* play = gm_play();
        void* player = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
        s32 yaw = gabi::call<s32>(0x025D6894, actor, player);
        actor->current.angle.y = (s16)(yaw - 32768);
        f32 random = gabi::call<f32>(0x02019788);
        s16 increment = (s16)gabi::ftoi(10000.0f * (random < 0.5f ? -1.0f : 1.0f));
        gmField<s16>(actor, 0x42C) = (s16)((s16)gmField<s16>(actor, 0x42C) + increment);
        random = gabi::call<f32>(0x02019788);
        increment = (s16)gabi::ftoi(10000.0f * (random < 0.5f ? -1.0f : 1.0f));
        gmField<s16>(actor, 0x42E) = (s16)((s16)gmField<s16>(actor, 0x42E) + increment);
        random = gabi::call<f32>(0x02019788);
        increment = (s16)gabi::ftoi(2000.0f * (random < 0.5f ? -1.0f : 1.0f));
        s16 zPhase = gmField<s16>(actor, 0x430);
        actor->speed.x = 0.0f;
        gmField<s16>(actor, 0x430) = (s16)(zPhase + increment);
        gmField<s16>(actor, 0x432) = 5000;
        gmField<s16>(actor, 0x434) = 5000;
        gmField<s16>(actor, 0x436) = 7000;
        gmField<s16>(actor, 0x3F8) = 51;
        actor->speed.y = 0.0f;
        actor->speed.z = 0.0f;
        actor->gravity = 0.0f;
        animateWings = true;
    }
    if (animateWings) {
        for (u32 i = 0; i < 4; ++i) {
            u32 phase = 0x414 + i * 6;
            s16 x = (s16)((s16)gmField<s16>(actor, phase) + (s16)gmField<s16>(actor, 0x42C));
            gmField<s16>(actor, phase) = x;
            s16 y = (s16)((s16)gmField<s16>(actor, phase + 2) + (s16)gmField<s16>(actor, 0x42E));
            gmField<s16>(actor, phase + 2) = y;
            s16 z = (s16)((s16)gmField<s16>(actor, phase + 4) + (s16)gmField<s16>(actor, 0x430));
            gmField<s16>(actor, phase + 4) = z;
            gmField<s16>(actor, 0x532 + i * 6) = (s16)gabi::ftoi(gm_sin(x) * 5000.0f);
            y = gmField<s16>(actor, phase + 2);
            gmField<s16>(actor, 0x530 + i * 6) = (s16)gabi::ftoi(gm_cos(y) * 5000.0f);
            z = gmField<s16>(actor, phase + 4);
            gmField<s16>(actor, 0x534 + i * 6) = (s16)gabi::ftoi(gm_cos(z) * 3000.0f);
        }
        s16 x = (s16)((s16)gmField<s16>(actor, 0x40C) + (s16)gmField<s16>(actor, 0x432));
        s16 y = (s16)((s16)gmField<s16>(actor, 0x40E) + (s16)gmField<s16>(actor, 0x434));
        s16 z = (s16)((s16)gmField<s16>(actor, 0x410) + (s16)gmField<s16>(actor, 0x436));
        gmField<s16>(actor, 0x40C) = x;
        gmField<s16>(actor, 0x40E) = y;
        gmField<s16>(actor, 0x410) = z;
        gmField<s16>(actor, 0x54A) = (s16)gabi::ftoi(gm_sin(x) * -3500.0f);
        gmField<s16>(actor, 0x548) = (s16)gabi::ftoi(gm_cos(y) * -3500.0f);
        z = gmField<s16>(actor, 0x410);
        gmField<s16>(actor, 0x54C) = (s16)gabi::ftoi(gm_cos(z) * -2000.0f);
        for (u32 i = 0; i < 6; ++i) gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x42C + i * 2), 0, 1, 80);
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 2.0f);
        if ((f32)actor->speedF < 0.2f) {
            u32 animation = gabi::ea(gmPointer(actor, 0x3D8));
            actor->speedF = 0.0f;
            *gabi::at<be<f32>>(animation + 4) = 0.0f;
            s32 currentAnimation = gmField<s32>(actor, 0x460);
            gmField<s16>(actor, 0x440) = 0;
            if (currentAnimation == 28 && ((u8)gmField<u8>(actor, 0x3E8) & 7) != 7) {
                gmField<u8>(actor, 0x3EC) = 3;
                gmField<s16>(actor, 0x3F8) = 37;
            } else {
                gmField<u8>(actor, 0x3EC) = 0;
                gmField<s16>(actor, 0x3F8) = 0;
            }
        }
    }
    f32 targetY = (f32)gmField<f32>(actor, 0x658) + 100.0f;
    if ((f32)actor->current.pos.y < targetY) gabi::call(0x0200ED84, &actor->current.pos.y, targetY, 1.0f, 3.0f);
}
VERIFY(0x0215163C, action_fly_damage);

BOOL daGM_Delete(gm_class* actor) {
    WWHD_FUNC(0x0214FFD8, BOOL, actor);
    gabi::call(0x025204C8, gmPart(actor, 0x3C8), STR(0x1000FED8));
    if (gabi::ea((JKRSolidHeap*)actor->heap)) gabi::call(0x025E563C, gmPointer(actor, 0x3D0));
    gabi::call(0x02041C30, gmPart(actor, 0x105C));
    if ((s8)actor->health == -128) {
        u8 switchNo = gmField<u8>(actor, 0x3EB);
        if (switchNo != 255 && (u8)gmField<u8>(actor, 0x3F2) != 0) {
            u32 save = *gabi::at<be<u32>>(0x101F84DC);
            s8 room = actor->current.roomNo;
            gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), switchNo, room);
        }
        u32 save = *gabi::at<be<u32>>(0x101F84DC);
        s8 room = actor->home.roomNo;
        u16 id = actor->setID;
        gabi::call(0x025BA5D4, gabi::at<void>(save + 0x20), id, room);
    }
    for (u32 offset = 0xC80; offset < 0xC90; offset += 4) {
        u32 particle = gmField<u32>(actor, offset);
        if (particle) {
            u32 flags = *gabi::at<be<u32>>(particle + 0x254);
            *gabi::at<be<s32>>(particle + 0x5C) = -1;
            *gabi::at<be<u32>>(particle + 0x254) = flags | 1;
            gmField<u32>(actor, offset) = 0;
        }
    }
    return TRUE;
}
VERIFY(0x0214FFD8, daGM_Delete);

BOOL daGM_Draw(gm_class* actor) {
    WWHD_FUNC(0x0214BF60, BOOL, actor);
    u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
    u32 model = *gabi::at<be<u32>>(morf + 0x90);
    void* environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C, environment, gabi::at<void>(model), &actor->tevStr);
    s16 angle = actor->shape_angle.y;
    gabi::call(0x025BEBB8, 0xBE, actor, gmPart(actor, 0x390), angle, 1.0f, 1.0f, 1.0f);
    if ((u8)gmField<u8>(actor, 0x3E9) == 0 && (s16)gmField<s16>(actor, 0xCB2) > 20) {
        gabi::call(0x0259138C, gmPointer(actor, 0x3D0), -1, gmPart(actor, 0x1288));
        return TRUE;
    }
    gm_play();
    if ((u8)gmField<u8>(actor, 0x3E9) != 0) {
        gabi::call(0x025E54D8, gmPointer(actor, 0x3D0));
        return TRUE;
    }
    u32 data = *gabi::at<be<u32>>(model + 0xAC);
    void* countObject = gabi::call<void*>(0x027F3F94, gabi::at<void>(data));
    u16 count = *gabi::at<be<u16>>(gabi::ea(countObject) + 8);
    for (u16 i = 0; i < count; ++i) {
        data = *gabi::at<be<u32>>(model + 0xAC);
        u32 bound = *gabi::at<be<u32>>(data + 4);
        u32 joint = *gabi::at<be<u32>>(data + 8);
        if (i < bound) joint += i * 28;
        u32 material = *gabi::at<be<u32>>(joint + 0x10);
        while (material) {
            u32 descriptor = *gabi::at<be<u32>>(material);
            u32 first = *gabi::at<be<u32>>(0x101B5498);
            u16 name = *gabi::at<be<u16>>(descriptor + 12);
            u32 visibility = *gabi::at<be<u32>>(material + 8);
            bool hidden;
            if (name == first) hidden = ((u8)gmField<u8>(actor, 0x3E8) & 8) != 0;
            else if (name == (u32)*gabi::at<be<u32>>(0x101B549C)) hidden = ((u8)gmField<u8>(actor, 0x3E8) & 2) != 0;
            else if (name == (u32)*gabi::at<be<u32>>(0x101B54A4)) hidden = ((u8)gmField<u8>(actor, 0x3E8) & 1) != 0;
            else if (name == (u32)*gabi::at<be<u32>>(0x101B54A0)) hidden = ((u8)gmField<u8>(actor, 0x3E8) & 4) != 0;
            else hidden = false;
            *gabi::at<be<u8>>(visibility + 4) = hidden ? 0 : 1;
            material = *gabi::at<be<u32>>(material + 4);
        }
        data = *gabi::at<be<u32>>(model + 0xAC);
        countObject = gabi::call<void*>(0x027F3F94, gabi::at<void>(data));
        count = *gabi::at<be<u16>>(gabi::ea(countObject) + 8);
    }
    s16 state = gmField<s16>(actor, 0x440);
    u32 animationOffset = state == 1 ? 0x3E0 : state == 2 ? 0x3DC : state == 3 ? 0x3E4 : 0x3D8;
    void* animation = gmPointer(actor, animationOffset);
    data = *gabi::at<be<u32>>(model + 0xAC);
    f32 frame = *gabi::at<be<f32>>(gabi::ea(animation) + 4);
    gabi::call(0x025E83FC, animation, gabi::at<void>(data), frame);
    gabi::call(0x025E5590, gmPointer(actor, 0x3D0));
    data = *gabi::at<be<u32>>(model + 0xAC);
    *gabi::at<be<u32>>(data + 0x48) = 0;
    return TRUE;
}
VERIFY(0x0214BF60, daGM_Draw);

static inline void* gm_resource(u32 name, s32 index) {
    gabi::Local<SafeString> key;
    key->mStringTop = name;
    key->__vtbl = 0x1000FC9C;
    void* control = gabi::at<void>(*gabi::at<be<u32>>(0x101F4F28));
    return gabi::call<void*>(0x026066C4, control, key.get(), index);
}
BOOL useHeapInit(gm_class* actor) {
    WWHD_FUNC(0x02150124, BOOL, actor);
    u8 larva = gmField<u8>(actor, 0x3E9);
    if (!larva) {
        void* resource = gm_resource(0x1000FEDB, 33);
        void* morf = gabi::call<void*>(0x025E4F64, gabi::at<void>(0), resource, 0, 0, 0, 1, 0, -1, 1, 0, 0x80000, 0x37441422, 1.0f);
        gmField<u32>(actor, 0x3D0) = gabi::ea(morf);
    } else {
        u8 type = gmField<u8>(actor, 0x3E8);
        if (type >= 1 && type <= 4) {
            void* resource = gm_resource(0x1000FEDB, type <= 2 ? 34 : 35);
            void* morf = gabi::call<void*>(0x025E4F64, gabi::at<void>(0), resource, 0, 0, 0, 2, 0, -1, 0, 0, 0, 0x11020203, 1.0f);
            gmField<u32>(actor, 0x3D0) = gabi::ea(morf);
        }
    }
    u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
    if (!morf) return FALSE;
    u32 model = *gabi::at<be<u32>>(morf + 0x90);
    if (!model) return FALSE;
    if ((u8)gmField<u8>(actor, 0x3E9) == 0) {
        *gabi::at<be<u32>>(model + 0xB8) = gabi::ea(actor);
        morf = gabi::ea(gmPointer(actor, 0x3D0));
        model = *gabi::at<be<u32>>(morf + 0x90);
        u32 data = *gabi::at<be<u32>>(model + 0xAC);
        void* countObject = gabi::call<void*>(0x027F3F94, gabi::at<void>(data));
        u16 count = *gabi::at<be<u16>>(gabi::ea(countObject) + 8);
        for (u16 i = 0; i < count; ++i) {
            morf = gabi::ea(gmPointer(actor, 0x3D0));
            model = *gabi::at<be<u32>>(morf + 0x90);
            data = *gabi::at<be<u32>>(model + 0xAC);
            u32 bound = *gabi::at<be<u32>>(data + 4);
            u32 joint = *gabi::at<be<u32>>(data + 8);
            if (i < bound) joint += i * 28;
            *gabi::at<be<u32>>(joint + 8) = 0x0214BB1C;
            morf = gabi::ea(gmPointer(actor, 0x3D0));
            model = *gabi::at<be<u32>>(morf + 0x90);
            data = *gabi::at<be<u32>>(model + 0xAC);
            countObject = gabi::call<void*>(0x027F3F94, gabi::at<void>(data));
            count = *gabi::at<be<u16>>(gabi::ea(countObject) + 8);
        }
        larva = gmField<u8>(actor, 0x3E9);
        morf = gabi::ea(gmPointer(actor, 0x3D0));
    } else larva = gmField<u8>(actor, 0x3E9);
    model = *gabi::at<be<u32>>(morf + 0x90);
    if (larva) return TRUE;
    for (u32 i = 0; i < 4; ++i) {
        void* animation = gabi::call<void*>(0x0273AD10, 0x78);
        if (gabi::ea(animation)) animation = gabi::call<void*>(0x025E80D0, animation);
        gmField<u32>(actor, 0x3D8 + i * 4) = gabi::ea(animation);
        if (!gabi::ea(animation)) return FALSE;
        void* resource = gm_resource(0x1000FEDB, i == 0 ? 41 : i == 1 ? 38 : i == 2 ? 39 : 40);
        animation = gmPointer(actor, 0x3D8 + i * 4);
        u32 data = *gabi::at<be<u32>>(model + 0xAC);
        if (!gabi::call<BOOL>(0x025E8154, animation, gabi::at<void>(data), resource, 1, i < 2 ? 2 : 0, 0, -1, 0, 0, 1.0f)) return FALSE;
    }
    morf = gabi::ea(gmPointer(actor, 0x3D0));
    model = *gabi::at<be<u32>>(morf + 0x90);
    return gabi::call<BOOL>(0x025E8A48, gmPart(actor, 0x1288), gabi::at<void>(model)) ? TRUE : FALSE;
}
VERIFY(0x02150124, useHeapInit);

BOOL Line_check(gm_class* actor, cXyz* end) {
    WWHD_FUNC(0x0214CDB4, BOOL, actor, end);
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD14, 0x1000FD24, 0x1000FD44, 0x1000FD34}, false);
    s16 xAngle = gmField<s16>(actor, 0x55A);
    gm_rotate(gm_currentMatrix(), 0x025F18EC, xAngle);
    s16 zAngle = gmField<s16>(actor, 0x55E);
    gm_rotate(gm_currentMatrix(), 0x025F1C5C, zAngle);
    gabi::Local<cXyz> direction;
    direction->x = 0.0f; direction->y = 100.0f; direction->z = 0.0f;
    gabi::Local<cXyz> start;
    gabi::call(0x0200FCD8, direction.get(), start.get());
    gabi::call(0x028E8D88, start.get(), gmPart<cXyz>(actor, 0x500), start.get());
    gabi::call(0x028E8D88, start.get(), gmPart<cXyz>(actor, 0x4D0), start.get());
    f32 startY = start->y;
    f32 endY = end->y;
    start->y = startY + 100.0f;
    end->y = endY + 100.0f;
    gm_lineSet( check.get(), start.get(), end, actor);
    void* play = gm_play();
    BOOL hit = gabi::call<BOOL>(0x02008860, gabi::at<void>(gabi::ea(play) + 0x12A0), check.get());
    if (!hit) {
        gmPart<cXyz>(actor, 0x4E8)->copy(*start);
        gmPart<cXyz>(actor, 0x4F4)->copy(*end);
    }
    u32 base = gabi::ea(check.get());
    *gabi::at<be<u32>>(base + 0x58) = 0x1000FD44;
    *gabi::at<be<u32>>(base + 0x64) = 0x1000FCD4;
    *gabi::at<be<u32>>(base + 0x20) = 0x1000FCC4;
    gabi::call(0x02008B4C, check.get(), 0);
    return hit ? TRUE : FALSE;
}
VERIFY(0x0214CDB4, Line_check);

static inline void gm_safeAssure(SafeString* string) {
    u32 vtable = string->__vtbl;
    u32 target = *gabi::at<be<u32>>(vtable + 0x14);
    gabi::call_ptr(target, string);
}
static inline u32 gm_relative(u32 object, u32 offset) {
    u32 relative = *gabi::at<be<u32>>(object + offset);
    return relative ? object + offset + relative : 0;
}

s32 daGM_Create(gm_class* actor) {
    WWHD_FUNC(0x021507C4, s32, actor);
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (gabi::ea(actor)) {
            gm_constructor(actor);
            condition = actor->actor_condition;
        }
        actor->actor_condition = condition | 8;
    }
    s32 phase = gabi::call<s32>(0x02520460, gmPart(actor, 0x3C8), STR(0x1000FF04));
    if (phase != 4) return phase;
    u32 parameters = actor->mParameters;
    gmField<u8>(actor, 0x3E8) = parameters;
    u8 type = (u32)actor->mParameters >> 8;
    gmField<u8>(actor, 0x3E9) = type;
    gmField<u8>(actor, 0x3EA) = (u32)actor->mParameters >> 16;
    gmField<u8>(actor, 0x3EB) = (u32)actor->mParameters >> 24;
    if ((u8)parameters == 255) gmField<u8>(actor, 0x3E8) = 0;
    u8 special = gmField<u8>(actor, 0x3EA);
    if (type == 255) { type = 0; gmField<u8>(actor, 0x3E9) = 0; }
    if (special == 255) { type = gmField<u8>(actor, 0x3E9); gmField<u8>(actor, 0x3EA) = 0; }
    if (type == 0) {
        gmField<u8>(actor, 0x3F2) = (s16)actor->current.angle.z;
        actor->shape_angle.x = 0;
        actor->current.angle.z = 0;
        actor->current.angle.x = 0;
        actor->shape_angle.z = 0;
    }
    u8 switchNo = gmField<u8>(actor, 0x3EB);
    if (switchNo != 255) {
        u32 save = *gabi::at<be<u32>>(0x101F84DC);
        s8 room = *gabi::at<be<s8>>(0x1047E6C8);
        if (gabi::call<BOOL>(0x025BA0C0, gabi::at<void>(save + 0x20), switchNo, room)) return 5;
    }
    s16 overrideType = *gabi::at<be<s16>>(0x1047BB1A);
    if (overrideType != 0) gmField<u8>(actor, 0x3EA) = (s16)(overrideType - 1);
    if (!gabi::call<BOOL>(0x025D63E8, actor, 0x02150124, 0x7400)) return 5;
    gabi::Local<SafeString> modelName, expectedName, wing1, wing2, wing3, wing4;
    modelName->__vtbl = expectedName->__vtbl = wing1->__vtbl = wing2->__vtbl = wing3->__vtbl = wing4->__vtbl = 0x1000FC9C;
    expectedName->mStringTop = 0x1000FF18;
    wing1->mStringTop = 0x1000FF08;
    wing2->mStringTop = 0x1000FEF4;
    wing3->mStringTop = 0x1000FEFC;
    wing4->mStringTop = 0x1000FF10;
    u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
    u32 model = *gabi::at<be<u32>>(morf + 0x90);
    u32 nameObject = *gabi::at<be<u32>>(model + 0x14);
    u32 modelNameEA = gm_relative(nameObject, 4);
    modelName->mStringTop = modelNameEA;
    gm_emptyVirtual((gm_class*)expectedName.get());
    gm_safeAssure(expectedName.get());
    u32 expectedEA = expectedName->mStringTop;
    gm_safeAssure(modelName.get());
    modelNameEA = modelName->mStringTop;
    bool matching = expectedEA == modelNameEA;
    if (!matching) {
        u32 a = expectedName->mStringTop, b = modelName->mStringTop;
        matching = true;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 x = *gabi::at<be<u8>>(a + i), y = *gabi::at<be<u8>>(b + i);
            if (x != y) { matching = false; break; }
            if (!x) break;
        }
    }
    if (matching) {
        SafeString* names[4] = {wing1.get(), wing2.get(), wing3.get(), wing4.get()};
        const u32 destinations[4] = {0x101B5498, 0x101B549C, 0x101B54A0, 0x101B54A4};
        for (u32 i = 0; i < 4; ++i) {
            gm_safeAssure(names[i]);
            nameObject = *gabi::at<be<u32>>(model + 0x14);
            u32 nameTable = gm_relative(nameObject, 0x18);
            u32 name = names[i]->mStringTop;
            u32 index = gabi::call<u32>(0x027DF9B0, gabi::at<void>(nameTable), STR(name));
            *gabi::at<be<u32>>(destinations[i]) = index;
        }
    }
    actor->max_health = 12; actor->health = 12;
    morf = gabi::ea(gmPointer(actor, 0x3D0));
    model = *gabi::at<be<u32>>(morf + 0x90);
    actor->cullMtx = model ? model + 0xC8 : 0;
    gabi::call(0x025D674C, actor, -200.0f, -150.0f, -200.0f, 200.0f, 250.0f, 150.0f);
    gabi::call(0x024F06B4, gmPart(actor, 0x5C4), &actor->current.pos, &actor->old.pos, actor, 1, gmPart(actor, 0x584), &actor->speed, 0, 0);
    void* status = gmPart(actor, 0x788);
    gabi::call(0x02515F14, status, 100, 1, actor);
    gmField<f32>(actor, 0x444) = 0.0f;
    gmField<s16>(actor, 0x550) = actor->current.angle.y;
    gmPart<cXyz>(actor, 0x470)->copy(actor->current.pos);
    void* play = gm_play();
    actor->itemTableIdx = gabi::call<s32>(0x0200E814, gabi::at<void>(gabi::ea(play) + 0x50AC), STR(0x1000FF1C), 0);
    if ((u8)gmField<u8>(actor, 0x3E9) != 0) {
        gmField<u32>(actor, 0x39C) = 0;
        u8 larvaType = gmField<u8>(actor, 0x3E8);
        gmField<f32>(actor, 0x44C) = 100.0f;
        if (larvaType == 2 || larvaType == 4) actor->shape_angle.y = (s16)((s16)actor->shape_angle.y - 32768);
        gmField<u8>(actor, 0x3EC) = 1;
        gmField<s16>(actor, 0x3F8) = 10;
        return phase;
    }
    gabi::call(0x02516518, gmPart(actor, 0x7C4), gabi::at<void>(0x101B53A8));
    gmField<u32>(actor, 0x808) = gabi::ea(status);
    gabi::call(0x0251677C, gmPart(actor, 0xB54), gabi::at<void>(0x101B5334));
    gmField<u32>(actor, 0xB98) = gabi::ea(status);
    gabi::call(0x02516518, gmPart(actor, 0x8F4), gabi::at<void>(0x101B53EC));
    gmField<u32>(actor, 0x938) = gabi::ea(status);
    gabi::call(0x02516518, gmPart(actor, 0xA24), gabi::at<void>(0x101B5430));
    gmField<u32>(actor, 0xA68) = gabi::ea(status);
    gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) & ~1u;
    gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
    gmField<u32>(actor, 0x39C) = 4;
    gmField<u8>(actor, 0x38A) = 4;
    f32 bodyR = (f32)*gabi::at<be<f32>>(0x1047BAA4) + 100.0f;
    u8 specialType = gmField<u8>(actor, 0x3EA);
    f32 height = *gabi::at<be<f32>>(0x1047BAA0);
    actor->mBtBodyR = bodyR;
    actor->mBtHeight = height + (specialType == 1 ? 120.0f : 200.0f);
    f32 maxDistance = *gabi::at<be<f32>>(0x1047BAA8);
    actor->mBtStartFrame = 0.0f;
    actor->mBtEndFrame = 10.0f;
    actor->mBtMaxDis = maxDistance + (specialType == 1 ? 800.0f : 880.0f);
    actor->mBtAttackType = 1;
    actor->stealItemLeft = specialType == 1 ? 1 : 3;
    if (specialType != 1) gmField<u8>(actor, 0xBC3) = 5;
    actor->scale.x = 0.0f; actor->scale.y = 0.0f; actor->scale.z = 0.0f;
    actor->mBtNowFrame = 1000.0f;
    if (specialType == 1) gmField<u8>(actor, 0x38A) = 3;
    morf = gabi::ea(gmPointer(actor, 0x3D0));
    gmField<f32>(actor, 0xE40) = 40.0f;
    gmField<f32>(actor, 0xE44) = 40.0f;
    gmField<u32>(actor, 0xCA4) = gabi::ea(actor);
    gmField<u32>(actor, 0x105C) = gabi::ea(actor);
    gmField<u32>(actor, 0x1068) = morf;
    gmField<u8>(actor, 0xE54) = 1;
    gmField<u8>(actor, 0x3EC) = 20;
    gmField<s16>(actor, 0x3F8) = 200;
    for (u32 i = 0; i < 10; ++i) {
        gmField<u8>(actor, 0x106C + i) = *gabi::at<be<u8>>(0x101B539C + i);
        gmField<f32>(actor, 0x1078 + i * 4) = *gabi::at<be<f32>>(0x101B5374 + i * 4);
    }
    draw_SUB(actor);
    specialType = gmField<u8>(actor, 0x3EA);
    if (specialType == 0) {
        actor->actor_status = ((u32)actor->actor_status | 0x04004000u) & ~0x20u;
        morf = gabi::ea(gmPointer(actor, 0x3D0));
        model = *gabi::at<be<u32>>(morf + 0x90);
        u32 flags = (u32)*gabi::at<be<u32>>(model + 0x74) & ~1u;
        *gabi::at<be<u32>>(model + 0x74) = flags;
        gabi::call(0x027F596C, gabi::at<void>(model), flags);
    } else if (specialType == 1 || specialType == 2) {
        play = gm_play();
        actor->itemTableIdx = gabi::call<s32>(0x0200E814, gabi::at<void>(gabi::ea(play) + 0x50AC), STR(specialType == 2 ? 0x1000FF24 : 0x1000FF2C), 0);
        if (specialType == 1) {
            gmField<f32>(actor, 0x44C) = 70.0f;
            gmField<f32>(actor, 0x448) = 60.0f;
        }
        actor->scale.x = 1.0f; actor->scale.y = 1.0f; actor->scale.z = 1.0f;
        if (specialType == 2) {
            gmField<s16>(actor, 0x3F8) = 0;
            gmField<u8>(actor, 0x3EC) = 0;
        } else {
            gmField<u8>(actor, 0x3E8) = 15;
            actor->max_health = 2; actor->health = 2;
            gmField<s16>(actor, 0x3F8) = 60;
            gmField<u8>(actor, 0x3EC) = 10;
        }
    }
    return phase;
}
VERIFY(0x021507C4, daGM_Create);

struct GmAttackInfo {
    gptr<void> object;
    u8 _4[0x10];
    be<u32> particlePosition;
    u8 _18[4];
};
WWHD_SIZE(GmAttackInfo, 0x1C);
static inline void gm_hitSound(gm_class* actor, u32 sound, bool actorSound = false) {
    if (!gabi::ea(actor) || !gabi::ea(&actor->eyePos)) return;
    s8 room = actor->current.roomNo;
    if (actorSound) {
        u32 id = gmField<u32>(actor, 4);
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1AA4, sound, &actor->eyePos, id, 0, reverb);
    } else {
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call(0x025E1A40, sound, &actor->eyePos, 32, reverb);
    }
}
static inline void gm_hitParticle(u32 id, cXyz* position, csXyz* angles = nullptr, cXyz* scale = nullptr) {
    void* play = gm_play();
    void* controller = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5AB0));
    gabi::call(0x025A847C, controller, 0, id, position, angles ? angles : gabi::at<csXyz>(0), scale ? scale : gabi::at<cXyz>(0), 255, 0, -1, 0, 0, 0);
}
BOOL body_atari_check(gm_class* actor) {
    WWHD_FUNC(0x0214C2F4, BOOL, actor);
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    gabi::call(0x02515E50, gmPart(actor, 0x7A4));
    s16 invulnerability = gmField<s16>(actor, 0x404);
    if (invulnerability != 0) return FALSE;
    s16 state = gmField<s16>(actor, 0x3F8);
    if (state == 90 || state == 91) return FALSE;
    if (!gabi::call<BOOL>(0x025162A4, gmPart(actor, 0x7C4))) {
        gmField<u8>(actor, 0x3EE) = 0;
        return FALSE;
    }
    if ((u8)gmField<u8>(actor, 0x3EE) != 0) return FALSE;
    gmField<u8>(actor, 0x3ED) = 0;
    void* hit = gabi::call<void*>(0x02516300, gmPart(actor, 0x7C4));
    gabi::Local<cXyz> hitPosition;
    f32 y = gmField<f32>(actor, 0x894), z = gmField<f32>(actor, 0x898);
    gmField<u8>(actor, 0x3EE) = 1;
    hitPosition->z = z;
    f32 x = gmField<f32>(actor, 0x890);
    hitPosition->y = y; hitPosition->x = x;
    if (!gabi::ea(hit)) return FALSE;
    gabi::Local<GmAttackInfo> info;
    info->particlePosition = 0;
    gmField<s16>(actor, 0x404) = 8;
    u32 attackType = *gabi::at<be<u32>>(gabi::ea(hit) + 0x10);
    bool skipDamage = false, damageChecked = false;
    switch (attackType) {
    case 0x08000000:
        if ((s8)actor->stealItemLeft > 0) {
            u8 health = actor->health;
            actor->health = 10;
            info->object = gabi::call<void*>(0x02516300, gmPart(actor, 0x7C4));
            gabi::call(0x025192A8, actor, info.get());
            gmField<u8>(actor, 0x3F3) = (u8)((u8)gmField<u8>(actor, 0x3F3) + 1);
            actor->health = (s8)health;
        }
        gm_hitParticle(0x27B, gmPart<cXyz>(actor, 0x390));
        gmField<u8>(actor, 0x3ED) = 3;
        skipDamage = true;
        break;
    case 2: {
        gm_hitSound(actor, 0x2806);
        gmField<u8>(actor, 0x3ED) = 0;
        u8 cut = *gabi::at<be<u8>>(gabi::ea(player) + 0x3AC);
        bool strong = (cut >= 5 && cut <= 10) || cut == 12 || (cut >= 14 && cut <= 16) || cut == 21 || cut == 23 || (cut >= 25 && cut <= 27) || cut == 30 || cut == 31;
        if (strong) gmField<u8>(actor, 0x3ED) = 1;
        else if (cut >= 17 && cut <= 20) {
            info->object = gabi::call<void*>(0x02516300, gmPart(actor, 0x7C4));
            gabi::call(0x025192A8, actor, info.get());
            damageChecked = true;
        }
        break;
    }
    case 0x200000:
        gmField<u8>(actor, 0x3ED) = 3;
        if ((u8)gmField<u8>(actor, 0x3EC) != 2 && ((u8)gmField<u8>(actor, 0x3E8) & 15) != 15) {
            if ((s16)gmField<s16>(actor, 0x3F8) == 34) gmField<u8>(actor, 0x3ED) = 9;
            gmField<s16>(actor, 0x3F8) = 20;
            gmField<u8>(actor, 0x3EC) = 2;
            gmField<f32>(actor, 0x44C) = 250.0f;
        }
        skipDamage = true;
        break;
    case 0x40:
        gmField<u8>(actor, 0x3ED) = 4;
        skipDamage = true;
        gm_hitParticle(0x27B, gmPart<cXyz>(actor, 0x390));
        gm_hitSound(actor, 0x2835);
        break;
    case 0x80:
        gm_hitSound(actor, 0x2835);
        break;
    case 0x10000:
        gm_hitSound(actor, 0x2855);
        gmField<u8>(actor, 0x3ED) = 7;
        if ((u8)*gabi::at<be<u8>>(gabi::ea(player) + 0x3AC) == 17) gmField<u8>(actor, 0x3ED) = 8;
        break;
    case 0x20: gmField<u8>(actor, 0x3ED) = 6; break;
    case 0x8000:
        gmField<u8>(actor, 0x3ED) = 10;
        skipDamage = true;
        gm_hitSound(actor, 0x2836);
        break;
    case 0x200: case 0x40000:
        gmField<s16>(actor, 0x1060) = 100;
        actor->health = 0;
        if (gabi::ea(&actor->eyePos)) gm_hitSound(actor, 0x48AF, true);
        gmField<u8>(actor, 0x3ED) = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case 0x100000:
        gmField<u8>(actor, 0xCAA) = 1;
        gmField<f32>(actor, 0xCAC) = 80.0f;
        gmField<f32>(actor, 0xE50) = 1.0f;
        actor->health = 0;
        gmField<u32>(actor, 0x39C) = 0;
        if (gabi::ea(&actor->eyePos)) gm_hitSound(actor, 0x48AF, true);
        gmField<u8>(actor, 0x3ED) = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case 0x80000:
        gmField<s16>(actor, 0xCA8) = 200;
        gabi::call(0x02041C30, gmPart(actor, 0x105C));
        gm_hitSound(actor, 0x48AF, true);
        skipDamage = true;
        actor->health = 0;
        gmField<u8>(actor, 0x3ED) = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case 0x4000:
        gmField<u8>(actor, 0x3ED) = 5;
        gm_hitSound(actor, 0x2836);
        break;
    default:
        gmField<u8>(actor, 0x3ED) = 0;
        gm_hitSound(actor, 0x2836);
        break;
    }
    if (!skipDamage) {
        if (!damageChecked) {
            info->object = gabi::call<void*>(0x02516300, gmPart(actor, 0x7C4));
            gabi::call(0x025192A8, actor, info.get());
        }
        u8 damageType = gmField<u8>(actor, 0x3ED);
        if (damageType == 1 || damageType == 7 || damageType == 8 || (s8)actor->health <= 0) {
            gm_hitParticle(16, hitPosition.get());
            gabi::Local<cXyz> scale;
            scale->x = 2.0f; scale->y = 2.0f; scale->z = 2.0f;
            gm_hitParticle(15, hitPosition.get(), &player->shape_angle, scale.get());
        } else gm_hitParticle(13, hitPosition.get(), &player->shape_angle);
    }
    attackType = *gabi::at<be<u32>>(gabi::ea(hit) + 0x10);
    if (attackType == 0x100000 || attackType == 0x80000) return TRUE;
    u8 damageType = gmField<u8>(actor, 0x3ED);
    if (damageType == 3 || damageType == 9) return TRUE;
    if (((u8)gmField<u8>(actor, 0x3E8) & 15) == 15 || (u8)gmField<u8>(actor, 0x3E9) != 0) return TRUE;
    gmField<f32>(actor, 0x44C) = 250.0f;
    play = gm_play();
    player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    if ((u32)*gabi::at<be<u32>>(0x10463FA8) == 0) {
        *gabi::at<be<f32>>(0x10463FCC) = 72.0f;
        *gabi::at<be<u32>>(0x10463FA8) = 1;
        *gabi::at<be<f32>>(0x10463FB4) = -72.0f;
        *gabi::at<be<f32>>(0x10463FB8) = -30.0f;
        *gabi::at<be<f32>>(0x10463FD8) = 72.0f;
        *gabi::at<be<f32>>(0x10463FC8) = 144.0f;
        *gabi::at<be<f32>>(0x10463FDC) = -30.0f;
        *gabi::at<be<f32>>(0x10463FD4) = -144.0f;
        *gabi::at<be<f32>>(0x10463FD0) = -30.0f;
        *gabi::at<be<f32>>(0x10463FBC) = -94.0f;
        *gabi::at<be<f32>>(0x10463FB0) = 94.0f;
        *gabi::at<be<f32>>(0x10463FC4) = -30.0f;
        *gabi::at<be<f32>>(0x10463FC0) = -72.0f;
    }
    u8 lastFlags = gmField<u8>(actor, 0x3E8);
    for (u32 i = 0; i < 4; ++i) {
        u32 bit = 1u << i;
        if (!((u8)gmField<u8>(actor, 0x3E8) & bit)) {
            u32 parameters = (i + 1) | ((u8)gmField<u8>(actor, 0x3EC) == 4 ? 0x200 : 0x100);
            s16 angle = actor->shape_angle.y;
            gm_rotate(gm_currentMatrix(), 0x025F1884, angle);
            gabi::Local<cXyz> direction;
            direction->x = *gabi::at<be<f32>>(0x10463FB0 + i * 12);
            direction->y = *gabi::at<be<f32>>(0x10463FB4 + i * 12);
            direction->z = *gabi::at<be<f32>>(0x10463FB8 + i * 12);
            gabi::Local<cXyz> spawnPosition, basePosition;
            gabi::call(0x0200FCD8, direction.get(), spawnPosition.get());
            gabi::call(0x0201AD78, &actor->current.pos, basePosition.get(), gmPart<cXyz>(actor, 0x4D0));
            gabi::call(0x028E8D88, spawnPosition.get(), basePosition.get(), spawnPosition.get());
            gabi::Local<csXyz> angles;
            angles->x = actor->shape_angle.x;
            angle = actor->shape_angle.y;
            angles->y = (s16)(angle - 32768);
            angles->z = actor->shape_angle.z;
            s8 room = actor->current.roomNo;
            gabi::call(0x025D5834, 0xCC, parameters, spawnPosition.get(), room, angles.get(), &actor->scale, 0, 0);
            gm_hitParticle(13, spawnPosition.get(), &player->shape_angle);
            lastFlags = (u8)gmField<u8>(actor, 0x3E8) | bit;
            s8 health = actor->health;
            gmField<u8>(actor, 0x3E8) = lastFlags;
            if (health > 0 && (u8)gmField<u8>(actor, 0x3ED) != 10) break;
        }
        if (i == 3) {
            s8 health = actor->health;
            if (health > 0 && (u8)gmField<u8>(actor, 0x3ED) != 10) return TRUE;
        }
    }
    if ((lastFlags & 15) != 15) {
        gm_hitSound(actor, 0x48AC, true);
        actor->mBtNowFrame = 1000.0f;
        gmField<s16>(actor, 0x3F8) = (u8)gmField<u8>(actor, 0x3EC) == 4 ? 52 : 50;
        gmField<u8>(actor, 0x3EC) = 5;
    } else if ((u8)gmField<u8>(actor, 0x3EC) != 10) {
        gm_hitSound(actor, 0x48AD, true);
        actor->mBtNowFrame = 1000.0f;
        gmField<s16>(actor, 0x3F8) = (u8)gmField<u8>(actor, 0x3EC) == 4 ? 61 : 60;
        gmField<f32>(actor, 0x448) = 60.0f;
        gmField<f32>(actor, 0x44C) = 150.0f;
        f32 random = gabi::call<f32>(0x020198D8, 600.0f);
        gmField<u8>(actor, 0x3EC) = 10;
        gmField<s16>(actor, 0x402) = (s16)gabi::ftoi(random + 600.0f);
    }
    return TRUE;
}
VERIFY(0x0214C2F4, body_atari_check);

static inline void gm_actionSound(gm_class* actor, u32 sound) {
    if (!gabi::ea(&actor->eyePos)) return;
    s8 room = actor->current.roomNo;
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, sound, &actor->eyePos, 0, reverb);
}
static inline bool gm_animationStopped(gm_class* actor) {
    u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
    if ((u8)*gabi::at<be<u8>>(morf + 0xA7) & 1) return true;
    return (f32)*gabi::at<be<f32>>(morf + 0x98) == 0.0f;
}
static inline void gm_zeroWingPhases(gm_class* actor) {
    for (u32 i = 0; i < 23; ++i) gmField<s16>(actor, 0x40C + i * 2) = 0;
}
static inline bool gm_groundProbe(gm_class* actor, cXyz* base, bool yaw) {
    if (yaw) gm_rotate(gm_currentMatrix(), 0x025F1884, actor->current.angle.y);
    gm_rotate(gm_currentMatrix(), yaw ? 0x025F1BF4 : 0x025F18EC, gmField<s16>(actor, 0x55A));
    gm_rotate(gm_currentMatrix(), 0x025F1C5C, gmField<s16>(actor, 0x55E));
    gabi::Local<cXyz> offset, target, copy;
    offset->x = 0.0f; offset->y = 100.0f; offset->z = yaw ? 200.0f : 0.0f;
    gabi::call(0x0200FCD8, offset.get(), target.get());
    gabi::call(0x028E8D88, target.get(), base, target.get());
    f32 x = target->x, y = target->y, z = target->z;
    copy->x = x; copy->y = y; copy->z = z;
    return Line_check(actor, copy.get()) != FALSE;
}
void action_ground_attack(gm_class* actor) {
    WWHD_FUNC(0x02151B98, void, actor);
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    f32 dx = (f32)player->current.pos.x - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
    f32 dz = (f32)player->current.pos.z - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
    s32 animation;
    u8 action;
    s16 state = gmField<s16>(actor, 0x3F8);
    switch (state) {
    case 60:
        gmField<u8>(actor, 0xBC3) = 0;
        gmField<u32>(actor, 0x7F0) = ((u32)gmField<u32>(actor, 0x7F0) & ~0x70u) | 0x70u;
        actor->current.angle.y = actor->shape_angle.y;
        gmField<s16>(actor, 0x552) = 0;
        actor->speedF = 10.0f;
        for (u32 i = 0; i < 6; ++i) gmField<f32>(actor, 0x4D0 + i * 4) = 0.0f;
        [[fallthrough]];
    case 61: {
        *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
        gmField<s16>(actor, 0x440) = 0;
        gm_zeroWingPhases(actor);
        gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) & ~1u;
        actor->mBtHeight = (f32)*gabi::at<be<f32>>(0x1047BAA0) + 120.0f;
        actor->mBtBodyR = (f32)*gabi::at<be<f32>>(0x1047BAA4) + 100.0f;
        actor->mBtAttackType = 1;
        actor->mBtStartFrame = 0.0f;
        actor->mBtEndFrame = 10.0f;
        actor->mBtMaxDis = (f32)*gabi::at<be<f32>>(0x1047BAA8) + 800.0f;
        actor->mBtNowFrame = 1000.0f;
        gmField<f32>(actor, 0x44C) = 250.0f;
        if ((s8)actor->health <= 0) {
            if ((u8)gmField<u8>(actor, 0x3EA) == 0) goto ground_demo;
            anm_init(actor, 19, 1.0f, 0, 1.0f, -1);
            gmField<s16>(actor, 0x3F8) = 100;
            goto ground_tail;
        }
        actor->speed.x = 0.0f; actor->speed.z = 0.0f;
        actor->speed.y = 10.0f; actor->gravity = -3.0f;
        gmField<s16>(actor, 0x3F8) = 62;
        [[fallthrough]];
    }
    case 62: {
        actor->shape_angle.x = (s16)((s16)actor->shape_angle.x + 0x100);
        f32 ground = (f32)gmField<f32>(actor, 0x658) + 80.0f;
        if ((f32)actor->current.pos.y < ground) {
            s8 health = actor->health;
            anm_init(actor, health > 0 ? 17 : 20, health > 0 ? 10.0f : 1.0f, 0, 1.0f, -1);
            gmField<f32>(actor, 0x454) = -20.0f;
            gmField<f32>(actor, 0x458) = 60.0f;
            gmField<f32>(actor, 0x45C) = 100.0f;
            actor->speedF = 0.0f;
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        }
        break;
    }
    case 63:
        if ((u32)gmField<u32>(actor, 0x5EC) & 0x20) {
            gm_actionSound(actor, 0x58AA);
            gmField<s16>(actor, 0x3F8) = 70;
        }
        break;
    case 70:
        gm_zeroWingPhases(actor);
        anm_init(actor, 25, 2.0f, 2, 1.0f, -1);
        actor->speedF = 0.0f;
        gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        [[fallthrough]];
    case 71: {
        void* frame = gabi::at<void>(gabi::ea(gmPointer(actor, 0x3D0)) + 0x98);
        if (gabi::call<BOOL>(0x027F2BF8, frame, 23.0f)) {
            s16 counter = (s16)((s16)gmField<s16>(actor, 0x412) + 1);
            gmField<s16>(actor, 0x412) = counter;
            s16 limit = (s16)((s16)*gabi::at<be<s16>>(0x1047BB12) + 1);
            if (counter > limit) goto ground_next;
        }
        break;
    }
    case 72: {
        u32 id = gabi::ea(actor) ? (u32)gmField<u32>(actor, 4) : 0xFFFFFFFF;
        f32 random = gabi::call<f32>(0x020198D8, (f32)(id & 7) * 2.0f);
        gmField<s16>(actor, 0x41A) = (s16)gabi::ftoi(random + 20.0f);
        gmField<s16>(actor, 0x410) = 0;
        if (!gm_groundProbe(actor, &player->current.pos, false)) {
            actor->current.angle.y = gabi::call<s16>(0x020195B0, dx, dz);
        } else if ((u8)gmField<u8>(actor, 0x3EA) != 1) {
            if ((s32)gmField<s32>(actor, 0x460) != 25) anm_init(actor, 25, 2.0f, 2, 1.0f, -1);
            break;
        } else {
            actor->speedF = 30.0f;
            gmField<s16>(actor, 0x442) = 0;
            gmField<s16>(actor, 0x442) = (s16)((s16)*gabi::at<be<s16>>(0x1047BB14) + 0x4000);
            random = gabi::call<f32>(0x02019788);
            if (random < 0.5f) gmField<s16>(actor, 0x442) = (s16)-((s16)*gabi::at<be<s16>>(0x1047BB14) + 0x4000);
            s16 yaw = gabi::call<s16>(0x020195B0, dx, dz);
            actor->current.angle.y = yaw;
            actor->current.angle.y = (s16)(yaw + (s16)gmField<s16>(actor, 0x442));
        }
        if ((u8)gmField<u8>(actor, 0x3EA) == 1) {
            gmField<f32>(actor, 0x448) = 60.0f;
            gmField<f32>(actor, 0x44C) = 70.0f;
        }
        anm_init(actor, 26, 2.0f, 2, 1.0f, -1);
        gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        [[fallthrough]];
    }
    case 73: {
        gm_actionSound(actor, 0x7030);
        s16 yaw = actor->current.angle.y, shapeYaw = actor->shape_angle.y;
        s16 difference = (s16)gabi::call<s32>(0x0200FAAC, yaw, shapeYaw);
        if (difference < 0x100) {
            actor->speedF = 30.0f;
            s16 counter = (s16)((s16)gmField<s16>(actor, 0x410) + 1);
            s16 limit = gmField<s16>(actor, 0x41A);
            gmField<s16>(actor, 0x410) = counter;
            if (counter > limit) goto ground_restart;
            if ((u8)gmField<u8>(actor, 0x3EA) == 1) actor->current.angle.y = gabi::call<s16>(0x020195B0, dx, dz);
            else if (gm_groundProbe(actor, gmPart<cXyz>(actor, 0x500), true)) goto ground_restart;
        }
        f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        if (!(distance > 400.0f)) {
            if ((u8)gmField<u8>(actor, 0x3EA) == 1 && gm_groundProbe(actor, gmPart<cXyz>(actor, 0x500), true)) goto ground_restart;
            gmField<s16>(actor, 0x3F6) = 0;
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
            anm_init(actor, 14, 2.0f, 0, 1.0f, -1);
            goto ground_attackStart;
        }
        break;
    }
    case 74:
        anm_init(actor, 14, 2.0f, 0, 1.0f, -1);
        goto ground_attackStart;
    case 75: {
        gm_actionSound(actor, 0x7030);
        dx = (f32)player->current.pos.x - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
        dz = (f32)player->current.pos.z - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
        actor->current.angle.y = gabi::call<s16>(0x020195B0, dx, dz);
        if (gm_animationStopped(actor)) {
            anm_init(actor, 15, 0.0f, 0, 1.0f, -1);
            gmField<s16>(actor, 0x440) = 0;
            gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) | 1;
            gmField<u32>(actor, 0xB58) = 1;
            actor->speedF = 35.0f;
            actor->gravity = -3.0f;
            actor->speed.y = 20.0f;
            goto ground_next;
        }
        break;
    }
    case 76:
        if ((s16)gmField<s16>(actor, 0x414) == 0 && ((u32)gmField<u32>(actor, 0xBA8) & 1)) {
            gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
            actor->speedF = -35.0f;
            gmField<s16>(actor, 0x414) = 1;
        }
        if (gm_animationStopped(actor)) {
            anm_init(actor, 16, 0.0f, 0, 1.0f, 7);
            u32 flags = gmField<u32>(actor, 0xB54);
            s16 oldState = gmField<s16>(actor, 0x3F8);
            gmField<s16>(actor, 0x414) = 0;
            gmField<u32>(actor, 0xB54) = flags & ~1u;
            gmField<s16>(actor, 0x3F8) = (s16)(oldState + 1);
        }
        break;
    case 77:
        if ((u32)gmField<u32>(actor, 0x5EC) & 0x20) {
            actor->speedF = 0.0f;
            gmField<s16>(actor, 0x3F8) = 70;
        }
        break;
    case 80: {
        gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        if ((u8)gmField<u8>(actor, 0x3ED) != 4) {
            gm_hitSound(actor, 0x48AC, true);
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3E0)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 1;
            gmField<s16>(actor, 0x408) = 0;
            anm_init(actor, 18, 0.0f, 0, 1.0f, -1);
            actor->speedF = 35.0f;
        } else {
            gmField<s16>(actor, 0x408) = 45;
            actor->speedF = 0.0f;
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 0;
        }
        play = gm_play();
        void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
        s32 yaw = gabi::call<s32>(0x025D6894, actor, other);
        actor->current.angle.y = (s16)(yaw + 32768);
        if ((u8)gmField<u8>(actor, 0x3ED) == 1) {
            actor->gravity = -3.0f;
            actor->speed.y = 20.0f;
        }
        goto ground_next;
    }
    case 81: {
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 1.5f);
        s16 target = (s16)((s16)*gabi::at<be<s16>>(0x1047BB16) + 44);
        if ((s16)gmField<s16>(actor, 0x408) == target) {
            gabi::call(0x02041CAC, actor);
            gm_actionSound(actor, 0x50BC);
        }
        bool finished = (u8)gmField<u8>(actor, 0x3ED) == 4 ? (s16)gmField<s16>(actor, 0x408) == 0 : gm_animationStopped(actor);
        if (finished) {
            anm_init(actor, 26, 2.0f, 2, 2.0f, -1);
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 0;
            goto ground_next;
        }
        break;
    }
    case 82: {
        gm_actionSound(actor, 0x7030);
        s16 yaw = (s16)((s16)gabi::call<s16>(0x020195B0, dx, dz) + 32768);
        s16 shapeYaw = actor->shape_angle.y;
        actor->current.angle.y = yaw;
        s16 difference = (s16)gabi::call<s32>(0x0200FAAC, yaw, shapeYaw);
        if (difference < 0x100) {
            anm_init(actor, 23, 2.0f, 2, 1.0f, -1);
            gmField<s16>(actor, 0x3FE) = 4;
            yaw = (s16)((s16)gabi::call<s16>(0x020195B0, dx, dz) + 32768);
            gmField<s16>(actor, 0x416) = yaw;
            gmField<s16>(actor, 0x418) = 0x4000;
            goto ground_next;
        }
        break;
    }
    case 83:
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            s16 offset = gmField<s16>(actor, 0x418), yaw = gmField<s16>(actor, 0x416);
            actor->current.angle.y = (s16)(yaw + offset);
            offset = gmField<s16>(actor, 0x418);
            gmField<s16>(actor, 0x3FE) = 4;
            gmField<s16>(actor, 0x418) = (s16)(offset ^ 0x4000);
        }
        if (ks_set_rtn(actor)) goto ground_restart;
        break;
    case 84: {
        anm_init(actor, 24, 1.0f, 0, 1.0f, -1);
        gm_zeroWingPhases(actor);
        play = gm_play();
        void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
        s32 yaw = gabi::call<s32>(0x025D6894, actor, other);
        actor->current.angle.y = (s16)(yaw + 32768);
        actor->speedF = 20.0f;
        gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        gm_actionSound(actor, 0x58A8);
        gm_hitSound(actor, 0x48B0, true);
        goto ground_next;
    }
    case 85:
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 3.0f);
        if (gm_animationStopped(actor)) gmField<s16>(actor, 0x3F8) = 82;
        break;
    case 90:
        gmField<u8>(actor, 0x3E8) = 0;
        anm_init(actor, 22, 0.0f, 0, 1.0f, 8);
        actor->speedF = 0.0f;
        goto ground_next;
    case 91:
        if (gm_animationStopped(actor)) {
            actor->speed.y = 0.0f;
            actor->gravity = 0.0f;
            gmField<s16>(actor, 0x55A) = 0;
            gmField<s16>(actor, 0x55C) = 0;
            gmField<s16>(actor, 0x55E) = 0;
            gmField<u8>(actor, 0xBC3) = 5;
            f32 height = *gabi::at<be<f32>>(0x1047BAA0), radius = *gabi::at<be<f32>>(0x1047BAA4);
            actor->mBtHeight = height + 200.0f;
            actor->mBtBodyR = radius + 100.0f;
            f32 distance = *gabi::at<be<f32>>(0x1047BAA8);
            actor->mBtStartFrame = 0.0f;
            actor->mBtAttackType = 1;
            actor->mBtEndFrame = 10.0f;
            actor->mBtMaxDis = distance + 880.0f;
            gmField<s16>(actor, 0x3F8) = 37;
            gmField<u8>(actor, 0x3EC) = 3;
        }
        break;
    case 100: {
        gmField<s16>(actor, 0x408) = 0;
        actor->gravity = -3.0f;
        *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
        gmField<s16>(actor, 0x440) = 0;
        gmField<u8>(actor, 0x79C) = 255;
        gm_hitSound(actor, 0x48AF, true);
        gm_zeroWingPhases(actor);
        if ((u8)gmField<u8>(actor, 0x3ED) == 7) {
            actor->speedF = 0.0f;
            anm_init(actor, 21, 0.0f, 0, 1.0f, -1);
        } else {
            play = gm_play();
            void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
            s32 yaw = gabi::call<s32>(0x025D6894, actor, other);
            actor->current.angle.y = (s16)(yaw + 32768);
            actor->speedF = 45.0f;
            actor->gravity = -3.0f;
            actor->speed.y = 45.0f;
            gmField<s16>(actor, 0x3FE) = 200;
            anm_init(actor, 19, 1.0f, 0, 1.0f, -1);
        }
        goto ground_next;
    }
    case 101: {
        if ((u8)gmField<u8>(actor, 0x3ED) == 7) {
            if (gm_animationStopped(actor)) {
                u8 stealBit = actor->stealItemBitNo;
                gabi::call(0x025D99E8, actor, &actor->current.pos, 10, 0, stealBit);
                gabi::call(0x025D57E0, actor);
                u32 save = *gabi::at<be<u32>>(0x101F84DC);
                s8 room = actor->home.roomNo;
                u16 id = actor->setID;
                gabi::call(0x025BA5D4, gabi::at<void>(save + 0x20), id, room);
                u8 switchNo = gmField<u8>(actor, 0x3EB);
                if (switchNo != 255 && (u8)gmField<u8>(actor, 0x3F2) != 0) {
                    save = *gabi::at<be<u32>>(0x101F84DC);
                    room = actor->current.roomNo;
                    gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), switchNo, room);
                }
            }
            break;
        }
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 1.5f);
        s16 substate = gmField<s16>(actor, 0x40C);
        if (substate == 0) {
            if ((s16)gmField<s16>(actor, 0x3FE) == 0 || ((u32)gmField<u32>(actor, 0x5EC) & 0x20)) {
                gm_actionSound(actor, 0x58A9);
                anm_init(actor, 20, 1.0f, 0, 1.0f, -1);
                gmField<s16>(actor, 0x40C) = 1;
                actor->speedF = 0.0f;
            }
        } else if (substate == 1) {
            if (gm_animationStopped(actor)) {
                void* animation = gmPointer(actor, 0x3E4);
                gmField<s16>(actor, 0x40C) = (s16)(substate + 1);
                *gabi::at<be<f32>>(gabi::ea(animation) + 4) = 0.0f;
                gmField<s16>(actor, 0x440) = 3;
            }
        } else if (substate == 2) {
            u32 animation = gabi::ea(gmPointer(actor, 0x3E4));
            bool stopped = ((u8)*gabi::at<be<u8>>(animation + 15) & 1) != 0;
            if (!stopped) stopped = (f32)*gabi::at<be<f32>>(animation) == 0.0f;
            if (stopped) gmField<s16>(actor, 0x40C) = (s16)(substate + 1);
        } else if (substate == 3) {
            if ((s16)gmField<s16>(actor, 0x3FE) == 0 || ((u32)gmField<u32>(actor, 0x5EC) & 0x20)) {
                s16 timer = (s16)((s16)gmField<s16>(actor, 0x40E) + 1);
                gmField<s16>(actor, 0x40E) = timer;
                if (timer > 15) {
                    u8 stealBit = actor->stealItemBitNo;
                    gabi::call(0x025D99E8, actor, &actor->current.pos, 10, 0, stealBit);
                    u8 switchNo = gmField<u8>(actor, 0x3EB);
                    if (switchNo != 255 && (u8)gmField<u8>(actor, 0x3F2) != 0) {
                        u32 save = *gabi::at<be<u32>>(0x101F84DC);
                        s8 room = actor->current.roomNo;
                        gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), switchNo, room);
                    }
                    gabi::call(0x025D57E0, actor);
                    u32 save = *gabi::at<be<u32>>(0x101F84DC);
                    s8 room = actor->home.roomNo;
                    u16 id = actor->setID;
                    gabi::call(0x025BA5D4, gabi::at<void>(save + 0x20), id, room);
                    gmField<s16>(actor, 0x40C) = (s16)((s16)gmField<s16>(actor, 0x40C) + 1);
                }
            }
        }
        break;
    }
    default: break;
    }
    goto ground_tail;
ground_attackStart:
    gm_hitSound(actor, 0x48AE, true);
    gmField<s16>(actor, 0x440) = 2;
    actor->speedF = 0.0f;
    goto ground_next;
ground_restart:
    gmField<s16>(actor, 0x3F8) = 70;
    goto ground_tail;
ground_next:
    gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
ground_tail:
    actor->mBtNowFrame = 1000.0f;
    state = gmField<s16>(actor, 0x3F8);
    if ((u32)((s32)state - 74) < 4) {
        s16 counter = (s16)((s16)gmField<s16>(actor, 0x3F6) + 1);
        gmField<s16>(actor, 0x3F6) = counter;
        s16 first = (s16)gabi::ftoi(*gabi::at<be<f32>>(0x1047BAAC));
        if (counter > first) {
            s16 last = (s16)gabi::ftoi((f32)*gabi::at<be<f32>>(0x1047BAB0) + 17.0f);
            if (counter < last) actor->mBtNowFrame = 5.0f;
        }
    }
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x43C), 0, 1, 0x800);
    gabi::call(0x0200F428, &actor->shape_angle.x, 0, 1, 0x800);
    state = gmField<s16>(actor, 0x3F8);
    if (state != 81 && state != 101 && state != 85) {
        s16 yaw = actor->current.angle.y;
        gabi::call(0x0200F428, &actor->shape_angle.y, yaw, 1, 0x800);
    }
    animation = gmField<s32>(actor, 0x460);
    if (animation != 19 && animation != 20 && body_atari_check(actor)) {
        u8 damageType = gmField<u8>(actor, 0x3ED);
        gmField<u8>(actor, 0x3EC) = 10;
        if (damageType == 7) {
            actor->health = 0;
            if ((u8)gmField<u8>(actor, 0x3EA) == 0) goto ground_demo;
            gmField<s16>(actor, 0x3F8) = 100;
        } else if ((s8)actor->health <= 0) {
            if ((u8)gmField<u8>(actor, 0x3EA) == 0) goto ground_demo;
            gmField<s16>(actor, 0x3F8) = 100;
        } else {
            damageType = gmField<u8>(actor, 0x3ED);
            gmField<s16>(actor, 0x3F8) = damageType == 3 ? 84 : 80;
        }
        return;
    }
    state = gmField<s16>(actor, 0x3F8);
    action = gmField<u8>(actor, 0x3EC);
    if ((u32)((s32)state - 71) <= 2 && (u8)gmField<u8>(actor, 0x3EA) != 1 && (s16)gmField<s16>(actor, 0x402) == 0) gmField<s16>(actor, 0x3F8) = 90;
    if (action == 10 && ((u32)gmField<u32>(actor, 0x5EC) & 0x20)) gabi::call(0x025D9A70, actor, gmPart<csXyz>(actor, 0x55A));
    return;
ground_demo:
    gmField<u8>(actor, 0x3EC) = 20;
    gmField<s16>(actor, 0x3F8) = 200;
}
VERIFY(0x02151B98, action_ground_attack);

static inline f32 gm_tune(u32 offset) { return *gabi::at<be<f32>>(0x1047B608 + offset); }
static inline s16 gm_tuneAngle(u32 offset) { return *gabi::at<be<s16>>(0x1047B608 + offset); }
static inline void gm_cameraVectors(gm_class* actor, cXyz* center, cXyz* eye) {
    f32 eyeX = gmField<f32>(actor, 0x564), centerY = gmField<f32>(actor, 0x574);
    eye->x = eyeX; center->y = centerY;
    f32 eyeZ = gmField<f32>(actor, 0x56C), centerX = gmField<f32>(actor, 0x570);
    eye->z = eyeZ; center->x = centerX;
    f32 eyeY = gmField<f32>(actor, 0x568), centerZ = gmField<f32>(actor, 0x578);
    eye->y = eyeY; center->z = centerZ;
}
void action_demo(gm_class* actor) {
    WWHD_FUNC(0x02153350, void, actor);
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    play = gm_play();
    s8 cameraIndex = *gabi::at<be<s8>>(gabi::ea(play) + 0x5B30);
    play = gm_play();
    u32 camera = *gabi::at<be<u32>>(gabi::ea(play) + cameraIndex * 0x34 + 0x5AF8);
    void* cameraBody = gabi::at<void>(camera + 0x248);
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD94, 0x1000FDA4, 0x1000FDC4, 0x1000FDB4}, false);
    u32 checkEA = gabi::ea(check.get());
    *gabi::at<be<u8>>(checkEA + 0x5D) = 1;
    *gabi::at<be<u32>>(checkEA + 0x68) = 3;
    s16 state = gmField<s16>(actor, 0x3F8);
    switch (state) {
    case 200:
        gm_zeroWingPhases(actor);
        gmField<u32>(actor, 0x39C) = 0;
        if ((s8)actor->health > 0) {
            actor->current.angle.y = 0;
            actor->shape_angle.y = 0;
            actor->shape_angle.x = -16384;
            gmField<s16>(actor, 0x560) = 1;
            actor->current.pos.x = -100.0f;
            actor->current.pos.y = 570.0f;
            actor->current.pos.z = -1350.0f;
            anm_init(actor, 29, 10.0f, 2, 1.0f, -1);
            gmField<s16>(actor, 0x43C) = -32768;
        }
        gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        [[fallthrough]];
    case 201:
        if ((u16)gmField<u16>(actor, 0xF8) != 2) {
            play = gm_play();
            *gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) = (u16)*gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) | 1;
            gabi::call(0x025D7B24, actor, 2, 65535, 0);
            gmField<u16>(actor, 0xFA) = (u16)gmField<u16>(actor, 0xFA) | 2;
            break;
        }
        *gabi::at<be<u32>>(gabi::ea(player) + 0x428) = 0;
        *gabi::at<be<s16>>(gabi::ea(player) + 0x420) = 3;
        *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 1;
        gabi::call(0x02514F2C, cameraBody);
        gabi::call(0x02515280, cameraBody, 2);
        gmField<f32>(actor, 0x580) = 50.0f;
        if ((s8)actor->health > 0) {
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
            actor->scale.x = 1.0f; actor->scale.y = 1.0f; actor->scale.z = 1.0f;
        } else {
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3D8)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 0;
            play = gm_play();
            void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
            s32 yaw = gabi::call<s32>(0x025D6894, actor, other);
            actor->current.angle.y = (s16)(yaw + 32768);
            actor->speedF = 20.0f; actor->gravity = -3.0f; actor->speed.y = 45.0f;
            anm_init(actor, 19, 1.0f, 0, 1.0f, -1);
            gmField<s16>(actor, 0x3F8) = 220;
            goto demo_destroy;
        }
        break;
    case 202: {
        gabi::Local<cXyz> position;
        position->x = gm_tune(0x6DC) + 73.0f;
        position->y = player->current.pos.y;
        position->z = gm_tune(0x6E0) + 1284.0f;
        u32 vtable = player->__vtbl;
        u32 function = *gabi::at<be<u32>>(vtable + 0x114);
        gabi::call_ptr(function, player, position.get(), -32768);
        gmField<f32>(actor, 0x570) = gm_tune(0x6E4) + 74.0f;
        gmField<f32>(actor, 0x574) = gm_tune(0x6E8) + 107.0f;
        gmField<f32>(actor, 0x578) = gm_tune(0x6EC) + 1095.0f;
        gmField<f32>(actor, 0x564) = gm_tune(0x6F0) + 71.0f;
        gmField<f32>(actor, 0x568) = gm_tune(0x6F4) + 124.0f;
        s16 next = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        gmField<s16>(actor, 0x3FE) = 10;
        gmField<s16>(actor, 0x3F8) = next;
        gmField<f32>(actor, 0x56C) = gm_tune(0x6F8) + 800.0f;
        [[fallthrough]];
    }
    case 203:
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 2;
            gmField<s16>(actor, 0x560) = 2;
            *gabi::at<be<s16>>(gabi::ea(player) + 0x422) = -32768;
            actor->current.pos.x = 100.0f;
            actor->current.pos.y = gm_tune(0x704) + 450.0f;
            actor->current.pos.z = 1150.0f;
            gabi::call(0x025E1960, 30);
            gmField<s16>(actor, 0x3FE) = (s16)(gm_tuneAngle(0x620) + 50);
            gmField<s16>(actor, 0x3F8) = 205;
            goto demo_cameraSet;
        }
        break;
    case 205:
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 26;
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
            gmField<s16>(actor, 0x3FE) = 40;
        }
        break;
    case 206:
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            s16 next = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
            gmField<s16>(actor, 0x3FE) = (s16)(gm_tuneAngle(0x622) + 120);
            gmField<s16>(actor, 0x3F8) = next;
            actor->current.pos.x = 100.0f; actor->current.pos.y = 570.0f; actor->current.pos.z = 1350.0f;
            gmField<f32>(actor, 0x57C) = 0.0f;
        }
        break;
    case 207: {
        f32 targetY = gm_tune(0x5B0) + 355.0f;
        f32 step = (f32)gmField<f32>(actor, 0x57C) * 10.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x574), targetY, 0.05f, step);
        step = (f32)gmField<f32>(actor, 0x57C) * 10.0f;
        targetY = gm_tune(0x5BC) + 121.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x568), targetY, 0.05f, step);
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x57C), 1.0f, 1.0f, 0.05f);
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            anm_init(actor, 27, 0.0f, 2, 1.0f, -1);
            f32 dx = (f32)gmField<f32>(actor, 0x470) - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
            f32 dz = (f32)gmField<f32>(actor, 0x478) - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
            actor->current.angle.y = gabi::call<s16>(0x020195B0, dx, dz);
            actor->shape_angle.x = 0x4000;
            s16 yaw = actor->shape_angle.y;
            actor->speedF = 22.0f;
            gmField<s16>(actor, 0x43C) = 0;
            actor->gravity = 1.0f; actor->speed.y = -65.0f;
            gmField<s16>(actor, 0x3FE) = 47;
            actor->shape_angle.y = (s16)(yaw - 32768);
            *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 0;
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        }
        break;
    }
    case 208: {
        if ((s16)gmField<s16>(actor, 0x3FE) == 37) *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 24;
        if (gabi::ea(actor)) gm_actionSound(actor, 0x7031);
        f32 height = (f32)player->current.pos.y + 220.0f;
        if ((f32)actor->current.pos.y < height) {
            actor->gravity = 0.0f; actor->speed.y = 0.0f;
            height = (f32)player->current.pos.y + 220.0f;
            gabi::call(0x0200ED84, &actor->current.pos.y, height, 1.0f, 50.0f);
        }
        s16 yaw = actor->current.angle.y;
        gabi::call(0x0200F428, &actor->shape_angle.y, yaw, 1, 0x1000);
        s16 speed = (s16)gabi::ftoi(gm_tune(0x708) + 1280.0f);
        gabi::call(0x0200F428, &actor->shape_angle.x, 0, 1, speed);
        f32 step = gm_tune(0x70C) + 50.0f;
        f32 target = actor->current.pos.x;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x570), target, 1.0f, step);
        target = (f32)actor->current.pos.y + gm_tune(0x6F0);
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x574), target, 1.0f, step);
        target = actor->current.pos.z;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x578), target, 1.0f, step);
        target = gm_tune(0x6E4) + 67.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x564), target, 1.0f, step);
        target = gm_tune(0x6E8) + 95.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x568), target, 1.0f, step);
        target = gm_tune(0x6EC) + 1.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x56C), target, 1.0f, step);
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            play = gm_play();
            gabi::call(0x025CB610, gabi::at<void>(gabi::ea(play) + 0x599C), 32);
            gabi::Local<cXyz> center, eye;
            gm_cameraVectors(actor, center.get(), eye.get());
            gabi::call(0x0251510C, cameraBody, center.get(), eye.get());
            gabi::call(0x02514F38, cameraBody);
            gabi::call(0x02515280, cameraBody, 0);
            play = gm_play();
            *gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) = (u16)*gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) | 8;
            gmField<u8>(actor, 0x38A) = 4;
            actor->current.angle.x = 0;
            gmField<s16>(actor, 0x560) = 0;
            gmField<u32>(actor, 0x39C) = 4;
            gabi::call(0x025E1918, 0x80000019u);
            actor->actor_status = ((u32)actor->actor_status | 0x20u) & ~0x4000u;
            gmField<s16>(actor, 0x3F8) = 0;
            gmField<u8>(actor, 0x3EC) = 0;
            u32 morf = gabi::ea(gmPointer(actor, 0x3D0));
            u32 model = *gabi::at<be<u32>>(morf + 0x90);
            u32 flags = (u32)*gabi::at<be<u32>>(model + 0x74) | 1;
            *gabi::at<be<u32>>(model + 0x74) = flags;
            gabi::call(0x027F596C, gabi::at<void>(model), flags);
        }
        break;
    }
    case 220:
        gabi::call(0x025E1928);
        gmField<s16>(actor, 0x3FE) = 200;
        gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        [[fallthrough]];
    case 221: {
        if ((f32)actor->speedF != 0.0f && ((s16)gmField<s16>(actor, 0x3FE) == 0 || ((u32)gmField<u32>(actor, 0x5EC) & 0x20))) {
            if ((s32)gmField<s32>(actor, 0x460) != 20) anm_init(actor, 20, 1.0f, 0, 1.0f, -1);
            actor->speedF = 0.0f;
        }
        u32 playerVtable = player->__vtbl;
        play = gm_play();
        void* other = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
        s32 yaw = gabi::call<s32>(0x025D6894, actor, other);
        u32 function = *gabi::at<be<u32>>(playerVtable + 0x114);
        gabi::call_ptr(function, player, &player->current.pos, (s16)(yaw + 32768));
        if (gm_tuneAngle(0x510) == 0 && (f32)actor->speedF == 0.0f && (s32)gmField<s32>(actor, 0x460) == 20 && gm_animationStopped(actor)) {
            *gabi::at<be<f32>>(gabi::ea(gmPointer(actor, 0x3E4)) + 4) = 0.0f;
            gmField<s16>(actor, 0x440) = 3;
            gmField<s16>(actor, 0x3F8) = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
        }
        break;
    }
    case 222: {
        u32 animation = gabi::ea(gmPointer(actor, 0x3E4));
        bool stopped = ((u8)*gabi::at<be<u8>>(animation + 15) & 1) != 0;
        if (!stopped) stopped = (f32)*gabi::at<be<f32>>(animation) == 0.0f;
        if (stopped) {
            u8 stealBit = actor->stealItemBitNo;
            gabi::call(0x025D99E8, actor, &actor->current.pos, 10, 0, stealBit);
            s16 next = (s16)((s16)gmField<s16>(actor, 0x3F8) + 1);
            actor->scale.x = 0.0f; actor->scale.y = 0.0f; actor->scale.z = 0.0f;
            gmField<s16>(actor, 0x3FE) = 100;
            gmField<s16>(actor, 0x3F8) = next;
        }
        break;
    }
    case 223:
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            play = gm_play();
            gabi::call(0x025CB610, gabi::at<void>(gabi::ea(play) + 0x599C), 32);
            gabi::Local<cXyz> center, eye;
            gm_cameraVectors(actor, center.get(), eye.get());
            gabi::call(0x0251510C, cameraBody, center.get(), eye.get());
            gabi::call(0x02514F38, cameraBody);
            gabi::call(0x02515280, cameraBody, 0);
            *gabi::at<be<s16>>(gabi::ea(player) + 0x420) = 2;
            *gabi::at<be<u32>>(gabi::ea(player) + 0x430) = 1;
            play = gm_play();
            *gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) = (u16)*gabi::at<be<u16>>(gabi::ea(play) + 0x52B8) | 8;
            u8 switchNo = gmField<u8>(actor, 0x3EB);
            if (switchNo != 255 && (u8)gmField<u8>(actor, 0x3F2) != 0) {
                u32 save = *gabi::at<be<u32>>(0x101F84DC);
                s8 room = actor->current.roomNo;
                gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), switchNo, room);
            }
            gabi::call(0x025D57E0, actor);
            u32 save = *gabi::at<be<u32>>(0x101F84DC);
            s8 room = actor->home.roomNo;
            u16 id = actor->setID;
            gabi::call(0x025BA5D4, gabi::at<void>(save + 0x20), id, room);
        }
        break;
    default: break;
    }
    state = gmField<s16>(actor, 0x3F8);
    if (state >= 221) {
        f32 y = (f32)actor->current.pos.y + 50.0f;
        f32 x = actor->current.pos.x;
        gmField<f32>(actor, 0x570) = x;
        y += gm_tune(0x5C8);
        f32 z = actor->current.pos.z;
        gmField<f32>(actor, 0x574) = y;
        gmField<f32>(actor, 0x578) = z;
        u32 matrix = gm_currentMatrix();
        s32 yaw = gabi::call<s32>(0x025D6894, actor, player);
        gm_rotate(matrix, 0x025F1884, (s16)(yaw + gm_tuneAngle(0x620)));
        f32 height = gm_tune(0x5CC), distance = gm_tune(0x5D0);
        gabi::Local<cXyz> direction;
        direction->x = 0.0f; direction->z = distance + 400.0f; direction->y = height + 200.0f;
        gabi::call(0x0200FCD8, direction.get(), gmPart<cXyz>(actor, 0x564));
        gabi::call(0x028E8D88, gmPart<cXyz>(actor, 0x564), &actor->current.pos, gmPart<cXyz>(actor, 0x564));
        state = gmField<s16>(actor, 0x3F8);
    }
    if (state >= 203 && state != 220) {
demo_cameraSet:
        gabi::Local<cXyz> center, eye;
        gm_cameraVectors(actor, center.get(), eye.get());
        f32 fov = gmField<f32>(actor, 0x580);
        gabi::call(0x02514F88, cameraBody, center.get(), eye.get(), 0, fov);
    }
demo_destroy:
    *gabi::at<be<u32>>(checkEA + 0x58) = 0x1000FD44;
    *gabi::at<be<u32>>(checkEA + 0x64) = 0x1000FCD4;
    *gabi::at<be<u32>>(checkEA + 0x20) = 0x1000FCC4;
    gabi::call(0x02008B4C, check.get(), 0);
}
VERIFY(0x02153350, action_demo);
// Inline particle state machine emitted in daGM_Execute.
static inline void gm_removeEmitter(gm_class* actor, u32 slot, u32 state) {
    u32 emitter = gmField<u32>(actor, slot);
    if (!emitter) return;
    auto* flags = gabi::at<be<u32>>(emitter + 0x254);
    u32 oldFlags = *flags;
    *gabi::at<be<s32>>(emitter + 0x5C) = -1;
    *flags = oldFlags | 1;
    gmField<u8>(actor, state) = 0;
    gmField<u32>(actor, slot) = 0;
}
static inline u32 gm_spawnEmitter(gm_class* actor, u32 id) {
    void* play = gm_play();
    void* controller = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5AB0));
    return gabi::ea(gabi::call<void*>(0x025A847C, controller, 0, id, &actor->current.pos,
        &actor->shape_angle, gabi::at<cXyz>(0), 255, 0, -1, 0, 0, 0));
}
static inline void gm_attachEmitter(gm_class* actor, u32 slot, u32 matrixOffset) {
    u32 emitter = gmField<u32>(actor, slot);
    if (!emitter) return;
    u32 morf = gmField<u32>(actor, 0x3D0);
    u32 model = *gabi::at<be<u32>>(morf + 0x90);
    u32 matrixObject = *gabi::at<be<u32>>(model + 0x2C);
    auto* flags = gabi::at<be<u16>>(matrixObject + 4);
    u16 oldFlags = *flags;
    u32 matrices = *gabi::at<be<u32>>(matrixObject + 0x10);
    *flags = oldFlags | 0x10;
    gabi::call(0x028249B0, gabi::at<void>(matrices + matrixOffset),
        gabi::at<void>(emitter + 0x1F0), gabi::at<void>(emitter + 0x22C));
}
static inline bool gm_wingEmitter(gm_class* actor, u32 slot, u32 state, u32 timer, u32 matrixOffset, bool* attached = nullptr) {
    bool reloadFlags = true;
    switch ((u8)gmField<u8>(actor, state)) {
    case 0: {
        u32 emitter = gm_spawnEmitter(actor, 0x81A3);
        gmField<u32>(actor, slot) = emitter;
        if (emitter) gmField<u8>(actor, state) = (u8)gmField<u8>(actor, state) + 1;
        break;
    }
    case 1:
        if (!(u32)gmField<u32>(actor, slot)) { reloadFlags = false; break; }
        if ((u8)gmField<u8>(actor, 0x3EC) == 5) {
            gmField<u8>(actor, state) = (u8)gmField<u8>(actor, state) + 1;
            gmField<s16>(actor, timer) = 0;
        }
        break;
    case 2: {
        u32 emitter = gmField<u32>(actor, slot);
        if (!emitter) { reloadFlags = false; break; }
        *gabi::at<be<f32>>(emitter + 0x34) = 80.0f;
        emitter = gmField<u32>(actor, slot);
        *gabi::at<be<f32>>(emitter + 0x74) = 80.0f;
        emitter = gmField<u32>(actor, slot);
        *gabi::at<be<u8>>(emitter + 0x244) = 255;
        *gabi::at<be<u8>>(emitter + 0x245) = 0;
        *gabi::at<be<u8>>(emitter + 0x246) = 0;
        gmField<u8>(actor, state) = (u8)gmField<u8>(actor, state) + 1;
        break;
    }
    case 3: {
        u32 emitter = gmField<u32>(actor, slot);
        if (!emitter) { reloadFlags = false; break; }
        *gabi::at<be<f32>>(emitter + 0x34) = 3.0f;
        emitter = gmField<u32>(actor, slot);
        *gabi::at<be<f32>>(emitter + 0x74) = 0.0f;
        gmField<u8>(actor, state) = (u8)gmField<u8>(actor, state) + 1;
        break;
    }
    case 4: {
        u32 emitter = gmField<u32>(actor, slot);
        if (!emitter) { reloadFlags = false; break; }
        s16 next = (s16)((s16)gmField<s16>(actor, timer) + 17);
        gmField<s16>(actor, timer) = next;
        if (next > 255) {
            gmField<s16>(actor, timer) = 255;
            gmField<u8>(actor, state) = 1;
            next = 255;
        }
        *gabi::at<be<u8>>(emitter + 0x244) = 255;
        *gabi::at<be<u8>>(emitter + 0x245) = next;
        *gabi::at<be<u8>>(emitter + 0x246) = next;
        break;
    }
    }
    if (!(u32)gmField<u32>(actor, slot)) return reloadFlags;
    if (attached) *attached = true;
    gm_attachEmitter(actor, slot, matrixOffset);
    return true;
}
static inline void gm_dropAction(gm_class* actor) {
    gm_play();
    u32 state = (u32)(s32)(s16)gmField<s16>(actor, 0x3F8);
    if (state == 10) {
        gm_zeroWingPhases(actor);
        actor->gravity = -1.0f;
        actor->speed.y = 7.0f;
        if ((u8)gmField<u8>(actor, 0x3E9) != 2) {
            actor->speedF = 40.0f;
            u8 type = gmField<u8>(actor, 0x3E8);
            f32 random = gabi::call<f32>(0x02019918, 4096.0f) + 16384.0f;
            s16 delta = (s16)gabi::ftoi(random);
            if (type == 1 || type == 3) {
                actor->current.angle.y = (s16)actor->current.angle.y + delta;
                gmField<s16>(actor, 0x40C) = 0x1000;
            } else {
                actor->current.angle.y = (s16)actor->current.angle.y - delta;
                gmField<s16>(actor, 0x40C) = -0x1000;
            }
            gmField<s16>(actor, 0x40E) = 0x300;
        }
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        state = 11;
    }
    if (state == 11 && (((u32)gmField<u32>(actor, 0x5EC) & 0x20) ||
        (f32)gmField<f32>(actor, 0x474) - 1000.0f > (f32)actor->current.pos.y)) {
        actor->shape_angle.x = 0;
        gmField<s16>(actor, 0x40C) = 0;
        gmField<s16>(actor, 0x40E) = 0;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
    } else if (state == 12 && (f32)actor->scale.x < 0.1f) {
        gabi::call(0x025D57E0, actor);
    }
    gabi::call(0x0200EDC8, &actor->scale.x, 1.0f, 0.02f);
    f32 scale = actor->scale.x;
    actor->scale.y = scale; actor->scale.z = scale;
    gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 5.0f);
    if ((f32)actor->speed.y < 0.0f) {
        actor->shape_angle.y = (s16)actor->shape_angle.y + (s16)gmField<s16>(actor, 0x40C);
        actor->shape_angle.x = (s16)actor->shape_angle.x + (s16)gmField<s16>(actor, 0x40E);
    }
}
static inline void gm_uchiwaOscillation(gm_class* actor) {
    gmField<s16>(actor, 0x40C) = (s16)gmField<s16>(actor, 0x40C) + 400;
    gmField<s16>(actor, 0x40E) = (s16)gmField<s16>(actor, 0x40E) + 400;
    gmField<s16>(actor, 0x410) = (s16)gmField<s16>(actor, 0x410) + 400;
    gmField<s16>(actor, 0x54A) = (s16)gabi::ftoi(gm_sin(gmField<s16>(actor, 0x40C)) * 4000.0f);
    gmField<s16>(actor, 0x548) = (s16)gabi::ftoi(gm_cos(gmField<s16>(actor, 0x40E)) * 4000.0f);
    gmField<s16>(actor, 0x54C) = (s16)gabi::ftoi(gm_cos(gmField<s16>(actor, 0x410)) * 4000.0f);
}
static inline void gm_uchiwaAction(gm_class* actor) {
    gm_play();
    u32 state = (u32)(s32)(s16)gmField<s16>(actor, 0x3F8);
    if (state == 20) {
        gabi::call(0x0214C1C8, actor, 28, 1.0f, 0, 1.0f, -1);
        gm_zeroWingPhases(actor);
        void* play = gm_play();
        void* target = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
        s32 yaw = gabi::call<s32>(0x025D6894, actor, target);
        actor->current.angle.y = yaw + 0x8000;
        if (gabi::ea(actor) + 0x37C) {
            gm_actionSound(actor, 0x58A8);
            gm_hitSound(actor, 0x48B0, true);
        }
        gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) & ~1u;
        gmField<f32>(actor, 0x44C) = 250.0f;
        actor->speedF = 60.0f; actor->speed.y = 24.0f;
        if ((u8)gmField<u8>(actor, 0x3ED) == 3) { actor->speedF = 20.0f; actor->speed.y = 22.0f; }
        gmField<s16>(actor, 0x412) = 5000;
        u32 morf = gmField<u32>(actor, 0x3D0);
        *gabi::at<be<f32>>(morf + 0x98) = 0.0f;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        state = 21;
    }
    if (state == 21) {
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 3.0f);
        gabi::call(0x0200EDC8, &actor->speed.y, 1.0f, 2.0f);
        for (u32 i = 0; i < 4; ++i) {
            u32 source = 0x10463FE0 + i * 6;
            gmField<s16>(actor, 0x532 + i * 6) = *gabi::at<be<s16>>(source + 2);
            gmField<s16>(actor, 0x530 + i * 6) = *gabi::at<be<s16>>(source);
            gmField<s16>(actor, 0x534 + i * 6) = *gabi::at<be<s16>>(source + 4);
        }
        gm_uchiwaOscillation(actor);
        if ((u8)gmField<u8>(actor, 0x3ED) != 3) {
            actor->shape_angle.z = (s16)actor->shape_angle.z + (s16)gmField<s16>(actor, 0x412);
            gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x412), 0, 1, 0x100);
        }
        if ((f32)actor->speed.y < 0.1f && (f32)actor->speedF < 0.1f) {
            actor->speedF = 0.0f;
            gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
            gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x556), 0x2000, 1, 0x1000);
            return;
        }
    } else if (state == 22) {
        actor->gravity = -0.08f;
        gm_uchiwaOscillation(actor);
        if ((f32)actor->current.pos.y < (f32)gmField<f32>(actor, 0x658) + 150.0f) {
            actor->speed.y = 0.0f; actor->gravity = 0.0f;
            gmField<s16>(actor, 0x3F8) = 0; gmField<u8>(actor, 0x3EC) = 0;
        }
    }
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x556), 0x2000, 1, 0x1000);
}
static inline void gm_reduceOffset(gm_class* actor) {
    for (u32 offset = 0x4D0; offset < 0x4DC; offset += 4)
        gabi::call(0x0200EDC8, gmPart<be<f32>>(actor, offset), 1.0f, 5.0f);
}
static inline void* gm_playerTarget() {
    void* play = gm_play();
    return gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
}
static inline void gm_diveAction(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(*gabi::at<be<u32>>(gabi::ea(play) + 0x5B2C));
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD14, 0x1000FD24, 0x1000FD44, 0x1000FD34}, false);
    gabi::Local<cXyz> direction, start;
    start->x = 0.0f; start->y = 0.0f; start->z = 0.0f;
    s16 state = gmField<s16>(actor, 0x3F8);
    switch (state) {
    case 30:
        gm_zeroWingPhases(actor);
        for (u32 offset = 0x47C; offset < 0x4A0; offset += 4) gmField<f32>(actor, offset) = 0.0f;
        *gabi::at<be<f32>>((u32)gmField<u32>(actor, 0x3DC) + 4) = 0.0f;
        gmField<s16>(actor, 0x440) = 2; gmField<s16>(actor, 0x43E) = 0;
        for (u32 offset = 0x47C; offset < 0x4A0; offset += 4) gmField<f32>(actor, offset) = 0.0f;
        if ((s32)gmField<s32>(actor, 0x460) != 13) gabi::call(0x0214C1C8, actor, 13, 1.0f, 2, 1.0f, -1);
        actor->current.angle.y = actor->shape_angle.y;
        actor->speedF = 0.0f;
        gmField<f32>(actor, 0x4DC) = 0.0f; gmField<f32>(actor, 0x4E0) = 0.0f; gmField<f32>(actor, 0x4E4) = 0.0f;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        [[fallthrough]];
    case 31: {
        gabi::call(0x0200F428, &actor->shape_angle.x, 0, 1, 0x200);
        s32 yaw = gabi::call<s32>(0x025D6894, actor, gm_playerTarget());
        gabi::call(0x0200F428, &actor->shape_angle.y, yaw, 1, 0x1000);
        f32 targetY = (f32)player->current.pos.y + 400.0f;
        gabi::call(0x0200ED84, &actor->current.pos.y, targetY, 1.0f, 20.0f);
        f32 dy = (f32)actor->current.pos.y - targetY;
        actor->current.angle.y = actor->shape_angle.y;
        if (__builtin_fabsf(dy) < 2.0f) {
            *gabi::at<be<f32>>((u32)gmField<u32>(actor, 0x3D0) + 0x98) = 2.0f;
            gmField<s16>(actor, 0x3FE) = 45;
            gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        }
        if (!gabi::ea(&actor->eyePos)) break;
        gm_actionSound(actor, 0x702F);
        break;
    }
    case 32: {
        if (gabi::ea(actor)) gm_actionSound(actor, 0x702F);
        if ((s16)gmField<s16>(actor, 0x3FE) > 15) actor->current.angle.y = gabi::call<s32>(0x025D6894, actor, gm_playerTarget());
        f32 distance = gabi::call<f32>(0x025D68EC, actor, gm_playerTarget());
        if (distance < 500.0f) {
            u32 matrix = gm_currentMatrix();
            s32 yaw = gabi::call<s32>(0x025D6894, actor, gm_playerTarget());
            gm_rotate(matrix, 0x025F1884, (s16)(yaw + 0x8000));
            direction->x = 0.0f; direction->y = 0.0f; direction->z = 500.0f;
            gabi::Local<cXyz> target;
            gabi::call(0x0200FCD8, direction.get(), target.get());
            gabi::call(0x028E8D88, target.get(), &player->current.pos, target.get());
            f32 x = target->x;
            if (__builtin_fabsf((f32)actor->current.pos.x - x) > 20.0f) gabi::call(0x0200ED84, &actor->current.pos.x, x, 1.0f, 10.0f);
            f32 z = target->z;
            if (__builtin_fabsf((f32)actor->current.pos.z - z) > 20.0f) gabi::call(0x0200ED84, &actor->current.pos.z, z, 1.0f, 10.0f);
        }
        gabi::call(0x0200F428, &actor->shape_angle.y, (s16)actor->current.angle.y, 1, 0x1000);
        if ((s16)gmField<s16>(actor, 0x3FE) != 0) break;
        gabi::Local<cXyz> target;
        target->x = (f32)player->current.pos.x; target->y = (f32)player->current.pos.y; target->z = (f32)player->current.pos.z;
        if (gabi::call<BOOL>(0x0214CDB4, actor, target.get())) break;
        if ((s16)gabi::call<s32>(0x0200FAAC, (s16)actor->shape_angle.y, (s16)actor->current.angle.y) > 0x500) break;
        actor->shape_angle.y = actor->current.angle.y;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        [[fallthrough]];
    }
    case 33: {
        gmField<s16>(actor, 0x43E) = (s16)gmField<s16>(actor, 0x43E) + 1;
        *gabi::at<be<f32>>((u32)gmField<u32>(actor, 0x3D0) + 0x98) = 1.0f;
        gabi::call(0x0214C1C8, actor, 27, 5.0f, 2, 1.0f, -1);
        gmField<u32>(actor, 0x8F4) = (u32)gmField<u32>(actor, 0x8F4) | 1;
        gmField<u32>(actor, 0x8F8) = 1;
        gmField<s16>(actor, 0x43C) = 0;
        gmField<u32>(actor, 0x7F0) = ((u32)gmField<u32>(actor, 0x7F0) & ~0x70u) | 0x20;
        actor->shape_angle.x = 0;
        actor->speedF = gm_tune(0x4B0) + 30.0f;
        actor->gravity = gm_tune(0x4B4) + 1.0f;
        actor->speed.y = gm_tune(0x4B8) + -85.0f;
        gmField<s16>(actor, 0x3F6) = 0;
        gmPart<cXyz>(actor, 0x464)->copy(player->current.pos);
        f32 y = player->current.pos.y;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        gmField<f32>(actor, 0x468) = y + 350.0f;
        [[fallthrough]];
    }
    case 34: {
        if (gabi::ea(&actor->eyePos)) gm_actionSound(actor, 0x7031);
        actor->mBtNowFrame = 1000.0f;
        s16 counter = (s16)gmField<s16>(actor, 0x3F6) + 1;
        gmField<s16>(actor, 0x3F6) = counter;
        if (counter > (s16)gabi::ftoi(gm_tune(0x4A4)) && counter < (s16)gabi::ftoi(gm_tune(0x4A8) + 62.0f)) actor->mBtNowFrame = 5.0f;
        if ((f32)actor->speed.y > 0.0f && (f32)actor->current.pos.y + (f32)gmField<f32>(actor, 0x4D4) > (f32)gmField<f32>(actor, 0x468)) {
            gmField<s16>(actor, 0x412) = 1;
            gabi::call(0x0200EDC8, &actor->gravity, 1.0f, 5.0f);
            gabi::call(0x0200EDC8, &actor->speed.y, 1.0f, 5.0f);
        } else if ((s16)gmField<s16>(actor, 0x412) != 0) {
            gabi::call(0x0200EDC8, &actor->gravity, 1.0f, 5.0f);
            gabi::call(0x0200EDC8, &actor->speed.y, 1.0f, 5.0f);
        }
        f32 y = actor->current.pos.y;
        if (y + (f32)gmField<f32>(actor, 0x4D4) < (f32)start->y) { y = start->y; actor->current.pos.y = y; }
        f32 ground = (f32)gmField<f32>(actor, 0x658) + 100.0f;
        if (y < ground) actor->current.pos.y = ground;
        for (u32 i = 0; i < 3; ++i) {
            gmPart<cXyz>(actor, 0x4A0 + i * 12)->x = 0.0f;
            gmPart<cXyz>(actor, 0x4A0 + i * 12)->y = 0.0f;
            gmPart<cXyz>(actor, 0x4A0 + i * 12)->z = 0.0f;
            gm_rotate(gm_currentMatrix(), 0x025F1884, actor->current.angle.y);
            direction->x = *gabi::at<be<f32>>(0x101B5310 + i * 4);
            direction->y = *gabi::at<be<f32>>(0x101B531C + i * 4);
            direction->z = *gabi::at<be<f32>>(0x101B5328 + i * 4);
            cXyz* target = gmPart<cXyz>(actor, 0x47C + i * 12);
            gabi::call(0x0200FCD8, direction.get(), target);
            gabi::call(0x028E8D88, target, &actor->current.pos, target);
            gabi::call(0x028E8D88, target, gmPart<cXyz>(actor, 0x4D0), target);
            gabi::Local<cXyz> combined;
            gabi::call(0x0201AD78, &actor->current.pos, combined.get(), gmPart<cXyz>(actor, 0x4D0));
            start->copy(*combined);
            gm_lineSet( check.get(), start.get(), target, actor);
            void* scene = gm_play();
            if (gabi::call<BOOL>(0x02008860, gabi::at<void>(gabi::ea(scene) + 0x12A0), check.get()))
                gmPart<cXyz>(actor, 0x4A0 + i * 12)->copy(*gabi::at<cXyz>(gabi::ea(check.get()) + 0x30));
        }
        if ((u32)gmField<u32>(actor, 0x5EC) & 0x10) {
            actor->mBtNowFrame = 1000.0f;
            gmField<s16>(actor, 0x3F8) = 37;
            gmField<u32>(actor, 0x7F0) = ((u32)gmField<u32>(actor, 0x7F0) & ~0x70u) | 0x70;
            goto diveDone;
        }
        cXyz* first = gmPart<cXyz>(actor, 0x4A0);
        cXyz* second = gmPart<cXyz>(actor, 0x4AC);
        bool hitFirst = (f32)first->x != 0.0f || (f32)first->z != 0.0f;
        bool hitSecond = (f32)second->x != 0.0f || (f32)second->z != 0.0f;
        if (!hitFirst && !hitSecond) break;
        if (hitFirst && hitSecond) {
            actor->speedF = 0.0f; actor->gravity = 0.0f; actor->speed.y = 0.0f;
            actor->mBtNowFrame = 1000.0f;
            gmField<s16>(actor, 0x3F8) = 40; gmField<u8>(actor, 0x3EC) = 4;
            gmField<u32>(actor, 0x7F0) = ((u32)gmField<u32>(actor, 0x7F0) & ~0x70u) | 0x70;
        } else {
            gabi::Local<cXyz> difference;
            gabi::call(0x0201ADE0, second, difference.get(), &actor->current.pos);
            direction->x = (f32)difference->x; direction->z = (f32)difference->z; direction->y = (f32)difference->y;
            if ((f32)first->x != 0.0f || (f32)first->z != 0.0f) {
                gabi::call(0x0201ADE0, first, difference.get(), &actor->current.pos);
                direction->x = (f32)difference->x; direction->y = (f32)difference->y; direction->z = (f32)difference->z;
            }
            s32 yaw = gabi::call<s32>(0x020195B0, (f32)direction->x, (f32)direction->z);
            actor->shape_angle.y = yaw + 0x4000;
            gabi::call(0x0200F428, &actor->shape_angle.y, (s16)actor->current.angle.y, 1, 0x1000);
        }
        break;
    }
    case 37:
        for (u32 offset = 0x47C; offset < 0x4A0; offset += 4) gmField<f32>(actor, offset) = 0.0f;
        actor->gravity = 0.0f; actor->speed.y = 0.0f;
        gmField<s16>(actor, 0x40A) = 0;
        gabi::call(0x0214C1C8, actor, 30, 5.0f, 2, 1.0f, -1);
        actor->speedF = 20.0f;
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        [[fallthrough]];
    case 38: {
        if (gabi::ea(actor)) gm_actionSound(actor, 0x702F);
        gabi::call(0x0200ED84, &actor->current.pos.y, (f32)player->current.pos.y + 500.0f, 1.0f, 20.0f);
        f32 dx = (f32)gmField<f32>(actor, 0x470) - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
        f32 dz = (f32)gmField<f32>(actor, 0x478) - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
        actor->current.angle.y = gabi::call<s32>(0x020195B0, dx, dz);
        s32 yaw = gabi::call<s32>(0x025D6894, actor, gm_playerTarget());
        gabi::call(0x0200F428, &actor->shape_angle.y, yaw, 1, 0x1000);
        bool passed = false;
        for (f32 frame : {4.0f, 8.0f, 12.0f, 16.0f}) {
            u32 morf = gmField<u32>(actor, 0x3D0);
            if (gabi::call<BOOL>(0x027F2BF8, gabi::at<void>(morf + 0x98), frame)) { passed = true; break; }
        }
        if (passed) gabi::call(0x0214CFA4, actor);
        if (gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz)) < 40.0f) gmField<s16>(actor, 0x3F8) = 30;
        break;
    }
    }
    gm_reduceOffset(actor);
 diveDone:
    u32 checkEA = gabi::ea(check.get());
    *gabi::at<be<u32>>(checkEA + 0x58) = 0x1000FD44;
    *gabi::at<be<u32>>(checkEA + 0x64) = 0x1000FCD4;
    *gabi::at<be<u32>>(checkEA + 0x20) = 0x1000FCC4;
    gabi::call(0x02008B4C, check.get(), 0);
}
static inline void gm_flightAction(gm_class* actor) {
    fopAc_ac_c* player = static_cast<fopAc_ac_c*>(gm_playerTarget());
    gm_play();
    f32 dx = (f32)player->current.pos.x - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
    f32 dz = (f32)player->current.pos.z - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
    s16 state = gmField<s16>(actor, 0x3F8);
    if (state == 0) {
        gm_zeroWingPhases(actor);
        for (u32 offset = 0x47C; offset < 0x4A0; offset += 4) gmField<f32>(actor, offset) = 0.0f;
        *gabi::at<be<f32>>((u32)gmField<u32>(actor, 0x3D8) + 4) = 0.0f;
        gmField<f32>(actor, 0x448) = 0.0f; gmField<f32>(actor, 0x454) = -120.0f;
        gmField<f32>(actor, 0x458) = 220.0f; gmField<f32>(actor, 0x45C) = 60.0f;
        gmField<s16>(actor, 0x440) = 0; gmField<f32>(actor, 0x44C) = 250.0f;
        f32 random = gabi::call<f32>(0x020198D8, 30.0f) + 30.0f;
        gmField<s16>(actor, 0x3FE) = 0; gmField<s16>(actor, 0x400) = (s16)gabi::ftoi(random);
        if ((s32)gmField<s32>(actor, 0x460) != 13) gabi::call(0x0214C1C8, actor, 13, 10.0f, 2, 1.0f, -1);
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        state = 1;
    }
    if (state == 1) {
        gmField<f32>(actor, 0x468) = (f32)player->current.pos.y + 250.0f;
        fopAc_ac_c* targetPlayer = static_cast<fopAc_ac_c*>(gm_playerTarget());
        if ((s16)gmField<s16>(actor, 0x3FE) == 0) {
            s16 delta = 0x2000;
            if (gabi::call<f32>(0x02019788) < 0.5f) delta = -0x2000;
            void* scene = gm_play();
            void* camera = gabi::at<void>(*gabi::at<be<u32>>(gabi::ea(scene) + 0x5AF8));
            s32 cameraAngle = gabi::call<s32>(0x024F8000, camera);
            for (u32 i = 0; i < 2; ++i) {
                gm_rotate(gm_currentMatrix(), 0x025F1884, (s16)(cameraAngle + delta));
                gabi::Local<cXyz> direction, target, copy;
                direction->x = 0.0f; direction->y = 0.0f; direction->z = 300.0f;
                gabi::call(0x0200FCD8, direction.get(), target.get());
                gabi::call(0x028E8D88, target.get(), &targetPlayer->current.pos, target.get());
                f32 y = (f32)actor->current.pos.y + (f32)gmField<f32>(actor, 0x4D4);
                copy->x = (f32)target->x; copy->y = y; copy->z = (f32)target->z; target->y = y;
                if (!gabi::call<BOOL>(0x0214CDB4, actor, copy.get())) {
                    f32 targetDx = (f32)target->x - ((f32)actor->current.pos.x + (f32)gmField<f32>(actor, 0x4D0));
                    f32 targetDz = (f32)target->z - ((f32)actor->current.pos.z + (f32)gmField<f32>(actor, 0x4D8));
                    actor->speedF = 10.0f; gmField<s16>(actor, 0x3FE) = 20;
                    s16 random = (s16)gabi::ftoi(gabi::call<f32>(0x020198D8, 20.0f));
                    gmField<s16>(actor, 0x3FE) = (s16)gmField<s16>(actor, 0x3FE) + random;
                    gmField<f32>(actor, 0x450) = 0.0f;
                    gmField<s16>(actor, 0x550) = gabi::call<s32>(0x020195B0, targetDx, targetDz);
                    goto flightVertical;
                }
                delta = (s16)(delta - 0x8000);
            }
        }
        if (!(__builtin_fabsf((f32)actor->current.pos.y - (f32)gmField<f32>(actor, 0x468)) > 40.0f) &&
            !(gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz)) > 300.0f) &&
            (s16)gmField<s16>(actor, 0x400) == 0) {
            f32 chance = 0.1f;
            u8 flags = gmField<u8>(actor, 0x3E8);
            for (u32 i = 0; i < 4; ++i) {
                if (flags & 1) { chance += 0.1f; if (chance > 0.3f) chance = 0.3f; flags >>= 1; }
            }
            gmField<s16>(actor, 0x3F8) = 2;
            if (gabi::call<f32>(0x02019788) < chance && ((u8)gmField<u8>(actor, 0x3E8) & 7) != 7) {
                actor->speedF = 0.0f; gmField<u8>(actor, 0x3EC) = 3; gmField<s16>(actor, 0x3F8) = 30;
            }
        }
    } else if (state == 2) {
        gabi::call(0x0214C1C8, actor, 11, 10.0f, 2, 1.0f, -1);
        if (gabi::ea(&actor->eyePos)) gm_hitSound(actor, 0x48AE, true);
        *gabi::at<be<f32>>((u32)gmField<u32>(actor, 0x3DC) + 4) = 0.0f;
        gmField<s16>(actor, 0x440) = 2; actor->speedF = 0.0f;
        f32 random = gabi::call<f32>(0x020198D8, 20.0f) + 20.0f;
        gmField<s16>(actor, 0x400) = (s16)gabi::ftoi(random);
        gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        gmField<f32>(actor, 0x468) = (f32)player->current.pos.y + 100.0f;
        goto flightAttackCheck;
    } else if (state == 3) {
        gmField<f32>(actor, 0x468) = (f32)player->current.pos.y + 100.0f;
 flightAttackCheck:
        if (!(__builtin_fabsf((f32)actor->current.pos.y - (f32)gmField<f32>(actor, 0x468)) > 40.0f) && (s16)gmField<s16>(actor, 0x400) == 0) {
            s16 yaw = actor->shape_angle.y;
            gmField<s16>(actor, 0x550) = yaw; actor->current.angle.y = yaw;
            actor->speedF = 50.0f;
            gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) | 1;
            gmField<u32>(actor, 0xB58) = 1;
            gabi::call(0x0214C1C8, actor, 12, 2.0f, 0, 1.0f, 6);
            gmField<s16>(actor, 0x3F8) = (s16)gmField<s16>(actor, 0x3F8) + 1;
        }
    } else if (state == 4) {
        if ((s16)gmField<s16>(actor, 0x412) == 0) {
            if ((u32)gmField<u32>(actor, 0xBA8) & 1) {
                gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
                actor->speedF = -25.0f; gmField<s16>(actor, 0x412) = 1;
            } else if ((s16)*gabi::at<be<s16>>(gabi::ea(player) + 0x3B0) != 0) { actor->speedF = -50.0f; gmField<s16>(actor, 0x412) = 1; }
        }
        gabi::call(0x0200EDC8, &actor->speedF, 1.0f, 5.0f);
        if (gm_animationStopped(actor)) {
            f32 random = gabi::call<f32>(0x020198D8, 20.0f) + 40.0f;
            actor->speedF = 0.0f;
            gmField<s16>(actor, 0x400) = (s16)gabi::ftoi(random);
            gabi::call(0x0214C1C8, actor, 11, 2.0f, 2, 1.0f, -1);
            gmField<s16>(actor, 0x3F8) = 0;
            gmField<u32>(actor, 0xB54) = (u32)gmField<u32>(actor, 0xB54) & ~1u;
        }
    }
 flightVertical:
    player = static_cast<fopAc_ac_c*>(gm_playerTarget());
    if (!((f32)actor->current.pos.y < (f32)player->current.pos.y + 150.0f)) {
        s16 phase = (s16)gmField<s16>(actor, 0x40C) + 1000;
        gmField<s16>(actor, 0x40C) = phase;
        f32 offset = gm_sin(phase) * 100.0f;
        gmField<f32>(actor, 0x4E0) = offset;
        s16 currentState = gmField<s16>(actor, 0x3F8);
        if (currentState == 3 || currentState == 4) {
            gmField<f32>(actor, 0x4DC) = 0.0f; gmField<f32>(actor, 0x4E0) = 0.0f; gmField<f32>(actor, 0x4E4) = 0.0f; offset = 0.0f;
        }
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x4D4), offset, 1.0f, 10.0f);
    }
    if (((u8)gmField<u8>(actor, 0x3E8) & 7) == 7 && (s16)gabi::call<s32>(0x0200FAAC, (s16)actor->shape_angle.z, 0) < 0x100) gmField<s16>(actor, 0x552) = -0x4000;
    if ((s16)gmField<s16>(actor, 0x3F8) != 4) {
        s16 step = (s16)gabi::ftoi(gmField<f32>(actor, 0x450));
        gabi::call(0x0200F428, &actor->current.angle.y, (s16)gmField<s16>(actor, 0x550), 1, step);
        gmField<f32>(actor, 0x450) = 1700.0f;
        gabi::call(0x0200ED84, gmPart<be<f32>>(actor, 0x450), 1700.0f, 1.0f, 3.0f);
        s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
        gabi::call(0x0200F428, &actor->shape_angle.y, yaw, 1, 0x800);
    }
    gabi::call(0x0200ED84, &actor->current.pos.y, (f32)gmField<f32>(actor, 0x468), 1.0f, 3.0f);
    gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x43C), 0, 1, 0x200);
    gabi::call(0x0200F428, &actor->shape_angle.x, 0, 1, 0x200);
    gm_actionSound(actor, 0x702F);
}
BOOL daGM_Execute(gm_class* actor) {
    WWHD_FUNC(0x0214D128, BOOL, actor);
    if ((u8)gmField<u8>(actor, 0x3E9) == 0) {
        gabi::call(0x025DA088, actor, 45, 9, 38);
        if ((s16)gmField<s16>(actor, 0x560) != 1) {
            u8 flags = gmField<u8>(actor, 0x3E8);
            if (flags & 4) {
                gm_removeEmitter(actor, 0xC80, 0x3EF);
                flags = gmField<u8>(actor, 0x3E8);
            } else if (gm_wingEmitter(actor, 0xC80, 0x3EF, 0x3FA, 0x330)) flags = gmField<u8>(actor, 0x3E8);
            if (flags & 8) gm_removeEmitter(actor, 0xC84, 0x3F0);
            else {
                bool attached = false;
                gm_wingEmitter(actor, 0xC84, 0x3F0, 0x3FC, 0x390, &attached);
                u32 emitter = gmField<u32>(actor, 0xC84);
                if (attached) {
                    *gabi::at<be<s16>>(emitter + 0x20) = 0x471C;
                    *gabi::at<be<s16>>(emitter + 0x22) = -0x1555;
                    *gabi::at<be<s16>>(emitter + 0x24) = 0;
                    emitter = gmField<u32>(actor, 0xC84);
                    *gabi::at<be<f32>>(emitter + 0x14) = 40.0f;
                    *gabi::at<be<f32>>(emitter + 0x18) = 0.0f;
                    *gabi::at<be<f32>>(emitter + 0x1C) = -30.0f;
                }
            }
        }
        s16 state = gmField<s16>(actor, 0x3F8);
        if ((s16)gmField<s16>(actor, 0x40A) != 0 || state == 34 || state == 208) {
            u8 particleState = gmField<u8>(actor, 0x3F1);
            if (particleState == 0 || particleState == 1) {
                u32 slot = particleState == 0 ? 0xC88 : 0xC8C;
                u32 emitter = gm_spawnEmitter(actor, particleState == 0 ? 0x81A1 : 0x81A2);
                gmField<u32>(actor, slot) = emitter;
                if (emitter) gmField<u8>(actor, 0x3F1) = (u8)gmField<u8>(actor, 0x3F1) + 1;
            }
            if ((s16)gmField<s16>(actor, 0x3F8) == 34) gmField<s16>(actor, 0x40A) = 45;
            gm_attachEmitter(actor, 0xC88, 0x90);
            gm_attachEmitter(actor, 0xC8C, 0x90);
        } else {
            gm_removeEmitter(actor, 0xC88, 0x3F1);
            gm_removeEmitter(actor, 0xC8C, 0x3F1);
        }
        if ((u8)gmField<u8>(actor, 0x3E9) == 0 && gabi::call<BOOL>(0x020402C8, gmPart(actor, 0xCA4))) {
            u32 model = *gabi::at<be<u32>>((u32)gmField<u32>(actor, 0x3D0) + 0x90);
            gm_copyMatrix(0x1048D0CC, model + 0xC8);
            gabi::call(0x025E55A0, gmPointer(actor, 0x3D0));
            gmField<f32>(actor, 0x390) = (f32)gmField<f32>(actor, 0x500);
            actor->eyePos.copy(*gmPart<cXyz>(actor, 0x500));
            gmField<f32>(actor, 0x394) = (f32)gmField<f32>(actor, 0x504) + 100.0f;
            gmField<f32>(actor, 0x398) = (f32)gmField<f32>(actor, 0x508);
            return TRUE;
        }
    }
    for (u32 i = 0; i < 7; ++i) {
        s16 timer = gmField<s16>(actor, 0x3FE + i * 2);
        if (timer != 0) gmField<s16>(actor, 0x3FE + i * 2) = timer - 1;
    }
    switch ((u8)gmField<u8>(actor, 0x3EC)) {
    case 0: gm_flightAction(actor); break;
    case 1: gm_dropAction(actor); break;
    case 2: gm_uchiwaAction(actor); break;
    case 3: gm_diveAction(actor); break;
    case 4: gabi::call(0x021512AC, actor); break;
    case 5: gabi::call(0x0215163C, actor); break;
    case 10: gabi::call(0x02151B98, actor); break;
    case 20: gabi::call(0x02153350, actor); break;
    }
    gm_rotate(gm_currentMatrix(), 0x025F1884, actor->current.angle.y);
    gm_rotate(gm_currentMatrix(), 0x025F1BF4, actor->current.angle.x);
    gabi::Local<cXyz> direction, velocity, center;
    direction->x = 0.0f; direction->y = 0.0f; direction->z = (f32)actor->speedF;
    gabi::call(0x0200FCD8, direction.get(), velocity.get());
    f32 vertical = (f32)actor->speed.y + (f32)actor->gravity;
    actor->speed.x = (f32)velocity->x;
    actor->speed.z = (f32)velocity->z;
    actor->speed.y = vertical;
    u8 larva = gmField<u8>(actor, 0x3E9);
    if (vertical < -100.0f) actor->speed.y = -100.0f;
    if (!larva) {
        u8 action = gmField<u8>(actor, 0x3EC);
        if (action != 20) {
            if (action != 10) {
                gabi::call(0x0214C2F4, actor);
                action = gmField<u8>(actor, 0x3EC);
                if (action == 5) goto twistWing;
                if (action == 2) goto playAnimation;
            }
            gabi::call(0x021511B8, actor);
            if ((u8)gmField<u8>(actor, 0x3EC) == 2) goto playAnimation;
 twistWing:
            gabi::call(0x0200F428, &actor->shape_angle.z, (s16)gmField<s16>(actor, 0x552), 1, 0x100);
            gabi::call(0x0200F428, gmPart<be<s16>>(actor, 0x552), 0, 1, 0x100);
        }
 playAnimation:
        if ((s16)gmField<s16>(actor, 0x408) == 0) {
            s32 material = 0;
            if ((u32)gmField<u32>(actor, 0x5EC) & 0x20) {
                void* play = gm_play();
                material = gabi::call<s32>(0x024EECAC, gabi::at<void>(gabi::ea(play) + 0x12A0), gmPart(actor, 0x6AC));
            }
            s32 reverb = gabi::call<s32>(0x02520540, (s8)actor->current.roomNo);
            gabi::call(0x025E535C, gmPointer(actor, 0x3D0), &actor->eyePos, material, reverb);
        }
        s16 colour = gmField<s16>(actor, 0x440);
        u32 brkOffset = colour == 1 ? 0x3E0 : colour == 2 ? 0x3DC : colour == 3 ? 0x3E4 : 0x3D8;
        gabi::call(0x025E742C, gmPointer(actor, brkOffset));
        gabi::call(0x0201AD78, &actor->current.pos, center.get(), gmPart<cXyz>(actor, 0x4D0));
        action = gmField<u8>(actor, 0x3EC);
        if (action != 20) {
            if (action == 10) {
                actor->eyePos.copy(*gmPart<cXyz>(actor, 0x500));
                gmField<f32>(actor, 0x390) = (f32)gmField<f32>(actor, 0x500);
                gmField<f32>(actor, 0x394) = (f32)gmField<f32>(actor, 0x504) + 50.0f;
                gmField<f32>(actor, 0x398) = (f32)gmField<f32>(actor, 0x508);
                center->x = (f32)gmField<f32>(actor, 0x50C);
                center->z = (f32)gmField<f32>(actor, 0x514);
                center->y = (f32)gmField<f32>(actor, 0x510) + (f32)gmField<f32>(actor, 0x454);
            } else {
                gmField<f32>(actor, 0x390) = (f32)center->x;
                gmField<f32>(actor, 0x394) = (f32)center->y;
                gmField<f32>(actor, 0x398) = (f32)center->z;
                f32 y = (f32)center->y + 100.0f;
                gmField<f32>(actor, 0x394) = y;
                actor->eyePos.copy(*center);
                center->y = (f32)center->y + (f32)gmField<f32>(actor, 0x454);
            }
            if ((s16)gmField<s16>(actor, 0x3F8) == 34) {
                gabi::Local<cXyz> combined;
                gabi::call(0x0201AD78, &actor->current.pos, combined.get(), gmPart<cXyz>(actor, 0x4D0));
                center->x = (f32)combined->x; center->z = (f32)combined->z; center->y = (f32)combined->y - 10.0f;
                gabi::call(0x020182E0, gmPart(actor, 0x8DC), center.get());
                gabi::call(0x02018428, gmPart(actor, 0x8DC), 60.0f);
                gabi::call(0x020184DC, gmPart(actor, 0x8DC), 375.0f);
            } else {
                gabi::call(0x020182E0, gmPart(actor, 0x8DC), center.get());
                gabi::call(0x02018428, gmPart(actor, 0x8DC), (f32)gmField<f32>(actor, 0x458));
                gabi::call(0x020184DC, gmPart(actor, 0x8DC), (f32)gmField<f32>(actor, 0x45C));
            }
            void* play = gm_play();
            gabi::call(0x0200E240, gabi::at<void>(gabi::ea(play) + 0x26A4), gmPart(actor, 0x7C4));
            gabi::Local<cXyz> combined;
            gabi::call(0x0201AD78, &actor->current.pos, combined.get(), gmPart<cXyz>(actor, 0x4D0));
            center->x = (f32)combined->x; center->z = (f32)combined->z; center->y = (f32)combined->y - 40.0f;
            gabi::call(0x020182E0, gmPart(actor, 0xA0C), center.get());
            gabi::call(0x02018428, gmPart(actor, 0xA0C), 60.0f);
            gabi::call(0x020184DC, gmPart(actor, 0xA0C), 185.0f);
            play = gm_play();
            gabi::call(0x0200E240, gabi::at<void>(gabi::ea(play) + 0x26A4), gmPart(actor, 0x8F4));
            play = gm_play();
            gabi::call(0x02516C14, gabi::at<void>(gabi::ea(play) + 0x4EF8), gmPart(actor, 0x8F4), 3);
            gabi::call(0x02018D40, gmPart(actor, 0xC6C), gmPart<cXyz>(actor, 0x4C4));
            gabi::call(0x02018C8C, gmPart(actor, 0xC6C), 55.0f);
            play = gm_play();
            gabi::call(0x0200E240, gabi::at<void>(gabi::ea(play) + 0x26A4), gmPart(actor, 0xB54));
        }
        if ((s16)gmField<s16>(actor, 0x560) == 0 && ((u8)gmField<u8>(actor, 0x3E8) & 15) != 15) {
            if ((s16)gmField<s16>(actor, 0x3F8) == 34) goto moveActor;
            gabi::Local<cXyz> combined;
            gabi::call(0x0201AD78, &actor->current.pos, combined.get(), gmPart<cXyz>(actor, 0x4D0));
            s16 phase = (s16)gmField<s16>(actor, 0x438) + 1000;
            gmField<s16>(actor, 0x438) = phase;
            f32 x = combined->x; f32 z = combined->z;
            center->y = (f32)combined->y; center->z = z; center->x = x;
            gmField<f32>(actor, 0x524) = gabi::call<f32>(0x02151294, gabi::at<void>(0x104A44F8), phase) * 300.0f;
            gmField<f32>(actor, 0x52C) = gabi::call<f32>(0x02151294, gabi::at<void>(0x104A44F8), (s16)gmField<s16>(actor, 0x438)) * 300.0f;
            center->x = x + (f32)gmField<f32>(actor, 0x524);
            center->z = z + (f32)gmField<f32>(actor, 0x52C);
            center->y = (f32)gmField<f32>(actor, 0x658);
            gabi::call(0x020182E0, gmPart(actor, 0xB3C), center.get());
            gabi::call(0x02018428, gmPart(actor, 0xB3C), 60.0f);
            gabi::call(0x020184DC, gmPart(actor, 0xB3C), 200.0f);
            void* play = gm_play();
            gabi::call(0x0200E240, gabi::at<void>(gabi::ea(play) + 0x26A4), gmPart(actor, 0xA24));
            u8 actionNow = gmField<u8>(actor, 0x3EC);
            if (actionNow != 5 && actionNow != 2) {
                play = gm_play();
                gabi::call(0x02516C14, gabi::at<void>(gabi::ea(play) + 0x4EF8), gmPart(actor, 0xA24), 3);
            }
        }
    }
 moveActor:
    bool noStatus = (s16)gmField<s16>(actor, 0x3F8) == 34 || (u8)gmField<u8>(actor, 0x3EC) == 20;
    gabi::call(0x025D6800, actor, noStatus ? gabi::at<void>(0) : gmPart(actor, 0x788));
    gabi::call(0x02151100, actor);
    gabi::call(0x0214BDFC, actor);
    return TRUE;
}
VERIFY(0x0214D128, daGM_Execute);
