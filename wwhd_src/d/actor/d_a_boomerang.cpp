/**
 * d_a_boomerang.cpp (WWHD)
 * Item - Boomerang (flight, lock-on sight, blur trail)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_boomerang.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_boomerang.h"
using namespace gabi;

void boomerangPlaySight(daBoomerang_sightPacket_l* sight, s32 count) {
    WWHD_FUNC(0x020CC2EC, void, sight, count);
    for (s32 i = 0; i < count; ++i) {
        u8 frame = static_cast<u8>(static_cast<u8>(sight->mFrames[i]) + 1);
        sight->mFrames[i] = frame == 26 ? 0 : frame;
    }
}
VERIFY(0x020CC2EC, boomerangPlaySight);

void boomerangResetLocks(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CEAB4, void, actor);
    for (u32 i = 0; i < 5; ++i) {
        actor->mLockActorIDs[i] = 0xFFFFFFFF;
        actor->mLockActors[i] = nullptr;
    }
    actor->mLockCount = 0;
    actor->mCurrentLock = 0;
}
VERIFY(0x020CEAB4, boomerangResetLocks);

BOOL boomerangSetLock(daBoomerang_c* actor, fopAc_ac_c* target, s32 sound) {
    WWHD_FUNC(0x020CC020, BOOL, actor, target, sound);
    u8 count = actor->mLockCount;
    if (count >= 5) return 0;
    u32 id = target ? load<u32>(ea(target) + 4) : 0xFFFFFFFF;
    for (u32 i = 0; i < count; ++i)
        if (static_cast<u32>(actor->mLockActorIDs[i]) == id) return 0;
    actor->mLockActorIDs[count] = id;
    actor->mLockActors[static_cast<u8>(actor->mLockCount)] = target;
    store<u8>(ea(actor) + 0x4A4 + static_cast<u8>(actor->mLockCount), 0);
    if (sound) {
        u8 index = actor->mLockCount;
        call<void>(0x025E1988, load<u32>(0x1000AEC0 + 4 * index));
    }
    actor->mLockCount = static_cast<u8>(static_cast<u8>(actor->mLockCount) + 1);
    return 1;
}
VERIFY(0x020CC020, boomerangSetLock);

void boomerangSetAimActor(daBoomerang_c* actor, fopAc_ac_c* target) {
    WWHD_FUNC(0x020CF87C, void, actor, target);
    boomerangResetLocks(actor);
    actor->mThirdPerson = 1;
    boomerangSetLock(actor, target, 0);
}
VERIFY(0x020CF87C, boomerangSetAimActor);

BOOL boomerangIsDelete(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC458, BOOL, actor);
    return 1;
}
VERIFY(0x020CC458, boomerangIsDelete);
BOOL boomerangDelete(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC460, BOOL, actor);
    return 1;
}
VERIFY(0x020CC460, boomerangDelete);

void boomerangEmptyDestructor(void* object, s32 flags) {
    WWHD_FUNC(0x020CF98C, void, object, flags);
    if (object && (flags & 1)) call<void>(0x0273AF40, object);
}
VERIFY(0x020CF98C, boomerangEmptyDestructor);

// SafeString's no-op termination hook in this translation unit's vtable.
void boomerangStringAssureTermination(void* string) {
    WWHD_FUNC(0x020D0A5C, void, string);
}
VERIFY(0x020D0A5C, boomerangStringAssureTermination);

void* boomerangTrailPointCtor(void* object) {
    WWHD_FUNC(0x020CF9A0, void*, object);
    if (!object) object = call<void*>(0x0273AD10, 0x254);
    if (object) {
        call<void>(0x027B5BD8, at<void>(ea(object) + 4));
        call<void>(0x027BF734, at<void>(ea(object) + 0x158));
        store<u32>(ea(object) + 0x250, 0);
        store<u32>(ea(object) + 0x24C, 0);
    }
    return object;
}
VERIFY(0x020CF9A0, boomerangTrailPointCtor);

void* boomerangSmallCtor(void* object) {
    WWHD_FUNC(0x020CF9FC, void*, object);
    if (!object) object = call<void*>(0x0273AD10, 0x10);
    return object;
}
VERIFY(0x020CF9FC, boomerangSmallCtor);

void boomerangSetRoomInfo(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC220, void, actor);
    u32 a = ea(actor);
    store<u32>(a + 0x26490, load<u32>(a + 0x314));
    store<u32>(a + 0x26494, load<u32>(a + 0x318));
    store<u32>(a + 0x26498, load<u32>(a + 0x31C));
    u32 play = call<u32>(0x025200D4);
    f32 ground = call<f32>(0x02008974, at<void>(play + 0x12A0), at<void>(a + 0x2646C));
    u8 room;
    if (ground != load<f32>(0x1000AED4)) {
        play = call<u32>(0x025200D4);
        room = call<u8>(0x024EF130, at<void>(play + 0x12A0), at<void>(a + 0x26480));
        play = call<u32>(0x025200D4);
        u8 color = call<u8>(0x024EEEB8, at<void>(play + 0x12A0), at<void>(a + 0x26480));
        store<u8>(a + 0x1CA, color);
    } else room = load<u8>(0x1047E6C8);
    store<u8>(a + 0x1C9, room);
    store<u8>(a + 0x262AE, room);
    store<u8>(a + 0x326, room);
}
VERIFY(0x020CC220, boomerangSetRoomInfo);

BOOL boomerangExecute(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC328, BOOL, actor);
    call<u32>(0x025200D4);
    call<u32>(0x025200D4);
    for (u32 i = 0; i < static_cast<u8>(actor->mLockCount); ++i) {
        Local<be<u32>> id;
        *id = static_cast<u32>(actor->mLockActorIDs[i]);
        u32 found = 0;
        if (static_cast<u32>(*id) != 0xFFFFFFFF)
            found = call<u32>(0x025D5218, 0x025E1234, id.get());
        actor->mLockActors[i] = at<fopAc_ac_c>(found);
    }
    u32 a = ea(actor);
    s16 selector = load<s16>(a + 0x264C2);
    if (selector) {
        u32 receiver = a + static_cast<s16>(load<s16>(a + 0x264C0));
        u32 target;
        if (selector < 0) target = load<u32>(a + 0x264C4);
        else {
            s16 tableOffset = load<s16>(a + 0x264C6);
            u32 table = load<u32>(receiver + tableOffset);
            target = load<u32>(table + 8 * static_cast<u32>(selector) + 4);
        }
        call<void>(target, at<void>(receiver));
    }
    u32 y = load<u32>(a + 0x318), z = load<u32>(a + 0x31C);
    store<u32>(a + 0x380, y);
    u32 x = load<u32>(a + 0x314);
    store<u32>(a + 0x384, z);
    store<u32>(a + 0x390, x);
    store<u32>(a + 0x394, y);
    store<u32>(a + 0x37C, x);
    store<u32>(a + 0x398, z);
    boomerangSetRoomInfo(actor);
    boomerangPlaySight(at<daBoomerang_sightPacket_l>(a + 0x3B0), actor->mLockCount);
    return 1;
}
VERIFY(0x020CC328, boomerangExecute);

BOOL boomerangExecuteWrapper(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC454, BOOL, actor);
    return boomerangExecute(actor);
}
VERIFY(0x020CC454, boomerangExecuteWrapper);

void boomerangRockLineCallback(daBoomerang_c* actor, fopAc_ac_c* target) {
    WWHD_FUNC(0x020CC13C, void, actor, target);
    if (!load<u32>(ea(actor) + 0xB0)) {
        if (target) boomerangSetLock(actor, target, 1);
        return;
    }
    for (u32 i = 0; i < 5; ++i) {
        if (actor->mLockActors[i].get() != target) continue;
        actor->mLockActorIDs[i] = 0xFFFFFFFF;
        actor->mLockActors[i] = nullptr;
        if (i == static_cast<u8>(actor->mCurrentLock)) {
            actor->mJustHit = 1;
            actor->mCurrentLock = static_cast<u8>(static_cast<u8>(actor->mCurrentLock) + 1);
        }
    }
}
VERIFY(0x020CC13C, boomerangRockLineCallback);

void boomerangRockLineThunk(daBoomerang_c* actor, void* ignored, fopAc_ac_c* target) {
    WWHD_FUNC(0x020CC218, void, actor, ignored, target);
    boomerangRockLineCallback(actor, target);
}
VERIFY(0x020CC218, boomerangRockLineThunk);

void* boomerangBlurPointCtor(void* object) {
    WWHD_FUNC(0x020CFCEC, void*, object);
    if (!object) object = call<void*>(0x0273AD10, 0x254);
    if (object) {
        call<void>(0x027B5BD8, at<void>(ea(object) + 4));
        call<void>(0x027BF734, at<void>(ea(object) + 0x158));
        store<u32>(ea(object) + 0x250, 0);
        store<u32>(ea(object) + 0x24C, 0);
    }
    return object;
}
VERIFY(0x020CFCEC, boomerangBlurPointCtor);

void boomerangCollisionDtor(void* object, s32 flags) {
    WWHD_FUNC(0x020CFD48, void, object, flags);
    if (!object) return;
    call<void>(0x027FB528, at<void>(ea(object) + 0xB4), 0);
    call<void>(0x027FB528, at<void>(ea(object) + 0xC), 0);
    call<void>(0x027FD764, object, 2);
    if (flags & 1) call<void>(0x0273AF40, object);
}
VERIFY(0x020CFD48, boomerangCollisionDtor);

void boomerangTrailPointDtor(void* object, s32 flags) {
    WWHD_FUNC(0x020CFDB4, void, object, flags);
    if (!object) return;
    call<void>(0x027BF880, at<void>(ea(object) + 0x158), 2);
    call<void>(0x027B5CBC, at<void>(ea(object) + 4), 2);
    if (flags & 1) call<void>(0x0273AF40, object);
}
VERIFY(0x020CFDB4, boomerangTrailPointDtor);

void boomerangBlurPointDtor(void* object, s32 flags) {
    WWHD_FUNC(0x020CFE14, void, object, flags);
    if (!object) return;
    call<void>(0x027BF880, at<void>(ea(object) + 0x158), 2);
    call<void>(0x027B5CBC, at<void>(ea(object) + 4), 2);
    if (flags & 1) call<void>(0x0273AF40, object);
}
VERIFY(0x020CFE14, boomerangBlurPointDtor);

void boomerangSetKeepMatrix(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CD264, void, actor);
    u32 play = call<u32>(0x025200D4);
    u32 player = load<u32>(play + 0x5B2C);
    u32 vtable = load<u32>(player + 0xB4);
    u32 target = load<u32>(vtable + 0x14);
    u32 source = call<u32>(target, at<void>(player));
    call<void>(0x028E90D4, at<void>(source), at<void>(0x1048D0CC));
    f32 z = load<f32>(0x1000AF98), x = load<f32>(0x1000AF90), y = load<f32>(0x1000AF94);
    call<void>(0x025F24E0, x, y, z);
    call<void>(0x025F19F8, at<void>(0x1048D0CC), static_cast<s16>(-0x39F4), static_cast<s16>(-0x357), static_cast<s16>(-0x2AF3));
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = load<f32>(0x1048D0CC + 4 * i);
    u32 model = load<u32>(ea(actor) + 0x3AC);
    // All source values are captured before any destination store, including overlap.
    for (u32 i = 0; i < 12; ++i) store<f32>(model + 0xC8 + 4 * i, matrix[i]);
    x = load<f32>(0x1048D0D8); z = load<f32>(0x1048D0F8); y = load<f32>(0x1048D0E8);
    store<f32>(ea(actor) + 0x318, y);
    store<f32>(ea(actor) + 0x31C, z);
    store<f32>(ea(actor) + 0x314, x);
}
VERIFY(0x020CD264, boomerangSetKeepMatrix);

void boomerangSetAimPos(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CEB00, void, actor);
    u32 a = ea(actor);
    if (static_cast<u8>(actor->mReturning)) {
        u32 play = call<u32>(0x025200D4);
        u32 player = load<u32>(play + 0x5B34);
        f32 x = load<f32>(player + 0x72FC), y = load<f32>(player + 0x7300), z = load<f32>(player + 0x7304);
        store<f32>(a + 0x26284, y);
        store<f32>(a + 0x26280, x);
        store<f32>(a + 0x26288, z);
        return;
    }
    u8 current = actor->mCurrentLock;
    s32 index = current;
    while (index < static_cast<u8>(actor->mLockCount)) {
        u32 target = ea(actor->mLockActors[index].get());
        if (target) {
            store<u32>(a + 0x26280, load<u32>(target + 0x37C));
            store<u32>(a + 0x26284, load<u32>(target + 0x380));
            store<u32>(a + 0x26288, load<u32>(target + 0x384));
            return;
        }
        ++index;
        actor->mCurrentLock = ++current;
    }
}
VERIFY(0x020CEB00, boomerangSetAimPos);

struct BoomerangString_l { be<u32> text, vtable; };

// SafeString comparison uses the HD virtual accessors in their original order.
static bool boomerangStageEquals(u32 literal) {
    Local<BoomerangString_l> lhs, rhs;
    lhs->text = literal;
    lhs->vtable = 0x1000AD2C;
    u32 play = call<u32>(0x025200D4);
    rhs->text = play + 0x5134;
    rhs->vtable = 0x1000AD2C;
    u32 vt = lhs->vtable;
    call<void>(load<u32>(vt + 0x14), lhs.get());
    vt = lhs->vtable;
    call<void>(load<u32>(vt + 0x14), lhs.get());
    vt = rhs->vtable;
    u32 first = lhs->text;
    call<void>(load<u32>(vt + 0x14), rhs.get());
    u32 second = rhs->text;
    if (first == second) return true;
    first = lhs->text;
    second = rhs->text;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = load<u8>(first + i), b = load<u8>(second + i);
        if (a != b) return false;
        if (!a) return true;
    }
    return false;
}

f32 boomerangGetFlyMax(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CECF0, f32, actor);
    u32 play = call<u32>(0x025200D4);
    if (load<u32>(play + 0x5CD8) & 0x10000) return load<f32>(0x1000AFA4);
    return load<f32>(boomerangStageEquals(0x1000AFAC) ? 0x1000AFA4 : 0x1000AFA8);
}
VERIFY(0x020CECF0, boomerangGetFlyMax);

BOOL boomerangCreateHeap(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC468, BOOL, actor);
    Local<BoomerangString_l> resource;
    resource->vtable = 0x1000AD2C;
    resource->text = 0x1000AED8;
    u32 resourceManager = load<u32>(0x101F4F28);
    u32 data = call<u32>(0x026066C4, at<void>(resourceManager), resource.get(), 0x14);
    if (!data) call<void>(0x0273AA24, at<void>(0x1000AEE8), 0x7BE, at<void>(0x1000AEFC));
    u32 model = call<u32>(0x025E38E0, at<void>(data), 0x80000, 0x37220202);
    store<u32>(ea(actor) + 0x3AC, model);
    if (!model) return 0;
    if (boomerangStageEquals(0x1000AEE0))
        call<void>(0x0207FD38, at<void>(ea(actor) + 0x26194), 0);
    return 1;
}
VERIFY(0x020CC468, boomerangCreateHeap);

BOOL boomerangCreateHeapThunk(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC5E8, BOOL, actor);
    return boomerangCreateHeap(actor);
}
VERIFY(0x020CC5E8, boomerangCreateHeapThunk);

void boomerangCheckBgHit(daBoomerang_c* actor, cXyz* from, cXyz* to) {
    WWHD_FUNC(0x020CEBDC, void, actor, from, to);
    u32 a = ea(actor), check = a + 0x26400;
    call<void>(0x024F1AFC, at<void>(check), from, to, actor);
    u32 play = call<u32>(0x025200D4);
    if (!call<u32>(0x02008860, at<void>(play + 0x12A0), at<void>(check))) return;
    store<u32>(a + 0x314, load<u32>(check + 0x30));
    store<u32>(a + 0x318, load<u32>(check + 0x34));
    store<u32>(a + 0x31C, load<u32>(check + 0x38));
    play = call<u32>(0x025200D4);
    u32 particle = load<u32>(play + 0x5AB0);
    call<void>(0x025A847C, at<void>(particle), 1, 0xC, at<cXyz>(a + 0x314), 0, 0, 0xFF, 0, -1, 0, 0, 0);
    actor->mReturning = 1;
    s16 angle = static_cast<s16>(static_cast<u16>(load<s16>(a + 0x322)) - 0x8000u);
    store<s16>(a + 0x322, angle);
    store<s16>(a + 0x32A, angle);
    boomerangResetLocks(actor);
    play = call<u32>(0x025200D4);
    u32 material = call<u32>(0x024EECAC, at<void>(play + 0x12A0), at<void>(check + 0x14));
    s32 room = load<s8>(a + 0x326);
    s32 reverb = call<s32>(0x02520540, room);
    call<void>(0x025E1A40, 0x2833, at<cXyz>(a + 0x37C), material, reverb);
}
VERIFY(0x020CEBDC, boomerangCheckBgHit);

void boomerangInitBlur(void* blur, void* matrix, s16 angle) {
    WWHD_FUNC(0x020CE6CC, void, blur, matrix, angle);
    u32 a = ea(blur), m = ea(matrix);
    call<void>(0x028E8F64, matrix, at<void>(0x10462724), at<void>(a + 0xA8));
    call<void>(0x028E8F64, matrix, at<void>(0x10462730), at<void>(a + 0x378));
    u32 z1 = load<u32>(a + 0x380), y0 = load<u32>(a + 0xAC);
    store<u32>(a + 0x38C, z1);
    u32 x0 = load<u32>(a + 0xA8);
    store<u32>(a + 0xB8, y0);
    store<u32>(a + 0xB4, x0);
    u32 y1 = load<u32>(a + 0x37C), z0 = load<u32>(a + 0xB0);
    store<u32>(a + 0x388, y1);
    store<u32>(a + 0xBC, z0);
    u32 x1 = load<u32>(a + 0x378);
    store<u32>(a + 0x98, 0);
    store<u32>(a + 0x384, x1);
    store<f32>(a + 0x9C, load<f32>(m + 0xC));
    store<f32>(a + 0xA0, load<f32>(m + 0x1C));
    store<f32>(a + 0xA4, load<f32>(m + 0x2C));
    call<void>(0x028E90D4, matrix, at<void>(0x1048D0CC));
    call<void>(0x025F1C28, at<void>(0x1048D0CC), static_cast<s16>(-2 * static_cast<s32>(angle)));
    call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462724), at<void>(a + 0x648));
    call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462730), at<void>(a + 0x918));
    u32 x2 = load<u32>(a + 0x648), z3 = load<u32>(a + 0x920);
    store<u32>(a + 0x654, x2);
    store<u32>(a + 0x92C, z3);
    u32 y3 = load<u32>(a + 0x91C), x3 = load<u32>(a + 0x918);
    store<u32>(a + 0x928, y3);
    store<u32>(a + 0x924, x3);
    u32 z2 = load<u32>(a + 0x650), y2 = load<u32>(a + 0x64C);
    store<u32>(a + 0x65C, z2);
    store<u32>(a + 0x658, y2);
}
VERIFY(0x020CE6CC, boomerangInitBlur);

void boomerangSetSight(void* sight, cXyz* position, s32 index) {
    WWHD_FUNC(0x020CBB70, void, sight, position, index);
    u32 a = ea(sight);
    u32 bit = (static_cast<u32>(index) & 0x20) ? 0 : 1u << (static_cast<u32>(index) & 31);
    if (!position) {
        store<u32>(a + 0x100, load<u32>(a + 0x100) & ~bit);
        return;
    }
    struct Viewport_l { be<f32> x, y, width, height, nearZ, farZ; };
    Local<Viewport_l> viewport;
    Local<cXyz> screen;
    call<void>(0x025F1084, viewport.get());
    f32 viewportScale = static_cast<f32>(viewport->height) / load<f32>(0x1000AD20);
    call<void>(0x025F1018, position, screen.get(), viewport.get());
    f32 x = screen->x, y = screen->y, z = screen->z;
    call<void>(0x028E93CC, at<void>(0x1048D0CC), x, y, z);
    f32 half = load<f32>(0x1000AE84);
    u8 frame = load<u8>(a + 0xF4 + static_cast<u32>(index));
    f32 phase, rotation;
    if (frame < 13) {
        phase = load<f32>(0x1000AE88) - static_cast<f32>(frame) / half;
        f32 alpha = fmadds(load<f32>(0x1000AE98), phase, load<f32>(0x1000AE9C));
        store<u8>(a + 0xF9 + static_cast<u32>(index), static_cast<u8>(ftoi(alpha)));
        rotation = -phase;
    } else {
        phase = static_cast<f32>(static_cast<s32>(frame) - 13) / half;
        f32 alpha = fmadds(load<f32>(0x1000AE98), phase, load<f32>(0x1000AE9C));
        store<u8>(a + 0xF9 + static_cast<u32>(index), static_cast<u8>(ftoi(alpha)));
        rotation = phase;
    }
    f32 divisor = load<f32>(0x1000AEAC) / viewportScale;
    u32 image = load<u32>(a + 0x108);
    u16 height = load<u16>(image + 4), width = load<u16>(image + 2);
    f32 size = fmadds(load<f32>(0x1000AEA0), phase, load<f32>(0x1000AEA4));
    f32 scale = size * load<f32>(0x1000AEA8);
    f32 scaleX = (scale * static_cast<f32>(width)) / divisor;
    f32 scaleY = (scale * static_cast<f32>(height)) / divisor;
    call<void>(0x025F2518, scaleX, scaleY, scale);
    u16 turn = static_cast<u16>(ftoi(load<f32>(0x1000AEB8) * rotation));
    f32 sine = load<f32>(0x104A44F8 + 8 * (turn >> 3));
    s16 angle = static_cast<s16>(ftoi(load<f32>(0x1000AEBC) * sine));
    call<void>(0x025F1C5C, at<void>(0x1048D0CC), angle);
    call<void>(0x028E90D4, at<void>(0x1048D0CC), at<void>(a + 4 + 0x30 * static_cast<u32>(index)));
    store<u32>(a + 0x100, load<u32>(a + 0x100) | bit);
}
VERIFY(0x020CBB70, boomerangSetSight);

BOOL boomerangDraw(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CBE34, BOOL, actor);
    u32 a = ea(actor), play = call<u32>(0x025200D4);
    store<u32>(0x104B4634, load<u32>(play + 0x5D58));
    play = call<u32>(0x025200D4);
    store<u32>(0x104B4638, load<u32>(play + 0x5D60));
    if (!load<u8>(a + 0x26273) && load<u8>(a + 0x26271)) {
        store<u32>(a + 0x4B0, 0);
        for (u32 index = load<u8>(a + 0x26272); index < load<u8>(a + 0x26271); ++index) {
            u32 target = load<u32>(a + 0x26258 + 4 * index);
            boomerangSetSight(at<void>(a + 0x3B0), target ? at<cXyz>(target + 0x37C) : nullptr, index);
        }
        play = call<u32>(0x025200D4);
        u32 list = play + 0x5D30;
        call<void>(0x0252CDC0, at<void>(list), at<void>(list + 0x264), at<void>(list + 0x268), at<void>(a + 0x3B0));
    }
    if (load<s32>(a + 0x20EC) > 0) {
        play = call<u32>(0x025200D4);
        u32 buffer = load<u32>(play + 0x5D7C);
        call<void>(0x0252F3B0, at<void>(play + 0x5D30), at<void>(buffer), at<void>(a + 0x2054), at<cXyz>(a + 0x314));
    }
    play = call<u32>(0x025200D4);
    u32 player = load<u32>(play + 0x5B2C);
    u32 vtable = load<u32>(player + 0xB4);
    u32 hidden = call<u32>(load<u32>(vtable + 0xCC), at<void>(player));
    if (!(hidden && !load<u32>(a + 0xB0))) {
        u32 light = call<u32>(0x02555D0C);
        call<void>(0x025626A4, at<void>(light), 0, at<cXyz>(a + 0x314), at<void>(a + 0x110));
        light = call<u32>(0x02555D0C);
        u32 model = load<u32>(a + 0x3AC);
        call<void>(0x02562F5C, at<void>(light), at<void>(model), at<void>(a + 0x110));
        bool ice = false;
        if (!load<u32>(a + 0xB0)) {
            play = call<u32>(0x025200D4);
            u32 link = load<u32>(play + 0x5B34);
            ice = (load<u32>(link + 0x3BC) & 0x800) != 0;
        }
        model = load<u32>(a + 0x3AC);
        if (ice) call<void>(0x02591200, at<void>(model), -1, 0);
        else {
            call<void>(0x025E2DE0, at<void>(model), 0);
            play = call<u32>(0x025200D4);
            model = load<u32>(a + 0x3AC);
            u32 link = load<u32>(play + 0x5B34);
            call<void>(0x023D9340, at<void>(link), at<void>(a + 0x26194), at<void>(model));
        }
    }
    play = call<u32>(0x025200D4);
    store<u32>(0x104B4634, load<u32>(play + 0x5D78));
    play = call<u32>(0x025200D4);
    store<u32>(0x104B4638, load<u32>(play + 0x5D7C));
    return 1;
}
VERIFY(0x020CBE34, boomerangDraw);

BOOL boomerangDrawWrapper(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CC01C, BOOL, actor);
    return boomerangDraw(actor);
}
VERIFY(0x020CC01C, boomerangDrawWrapper);

void boomerangColorToFloat(void* destination, void* color) {
    WWHD_FUNC(0x020CDAA8, void, destination, color);
    u32 c = ea(color), d = ea(destination);
    u8 red = load<u8>(c), green = load<u8>(c + 1), blue = load<u8>(c + 2), alpha = load<u8>(c + 3);
    f32 divisor = load<f32>(0x1000AF9C);
    store<f32>(d, static_cast<f32>(red) / divisor);
    store<f32>(d + 4, static_cast<f32>(green) / divisor);
    store<f32>(d + 8, static_cast<f32>(blue) / divisor);
    store<f32>(d + 12, static_cast<f32>(alpha) / divisor);
}
VERIFY(0x020CDAA8, boomerangColorToFloat);

void boomerangCopyMatrix(void* destination, void* source) {
    WWHD_FUNC(0x020CDB5C, void, destination, source);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = load<f32>(ea(source) + 4 * i);
    for (u32 i = 0; i < 12; ++i) store<f32>(ea(destination) + 4 * i, matrix[i]);
}
VERIFY(0x020CDB5C, boomerangCopyMatrix);

void boomerangStaticInit() {
    WWHD_FUNC(0x020CF8B0, void);
    store<u32>(0x10462718, 0);
    store<u32>(0x10462720, 0);
    store<u32>(0x10462714, 0);
    store<u32>(0x1046271C, 0);
    call<void>(0x028F026C, at<void>(0x1019246C));
    f32 low = load<f32>(0x1000AFD8), high = load<f32>(0x1000AFDC);
    store<f32>(0x10462708, low);
    store<f32>(0x1046270C, high);
    call<void>(0x028ED6F8, at<void>(0x10462710));
    call<void>(0x028F026C, at<void>(0x10192478));
    call<void>(0x028EAB2C, at<void>(0x10462711));
    call<void>(0x028F026C, at<void>(0x10192484));
    f32 zero = load<f32>(0x1000AF14), top = load<f32>(0x1000AFC4), root = load<f32>(0x1000AFE0), depth = load<f32>(0x1000AFD4);
    store<f32>(0x10462724, top);
    store<f32>(0x10462730, root);
    store<f32>(0x10462734, zero);
    store<f32>(0x10462738, zero);
    store<f32>(0x10462728, zero);
    store<f32>(0x10192464, depth);
    store<f32>(0x1046272C, zero);
}
VERIFY(0x020CF8B0, boomerangStaticInit);

BOOL boomerangProcWait(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CF4CC, BOOL, actor);
    u32 a = ea(actor), play = call<u32>(0x025200D4);
    f32 zero = load<f32>(0x1000AF14);
    u32 player = load<u32>(play + 0x5B34);
    store<f32>(a + 0x370, zero);
    boomerangSetKeepMatrix(actor);
    s32 segments = load<s32>(a + 0x20EC);
    u32 mode = load<u32>(a + 0xB0);
    if (segments > 0) store<s32>(a + 0x20EC, segments - 5);
    if (mode == 1) {
        f32 speed = load<f32>(0x1000AFD0);
        store<f32>(a + 0x370, speed);
        call<void>(0x02516138, at<void>(a + 0x262C8));
        u32 flags = load<u32>(a + 0x262C8);
        actor->mReturning = 0;
        actor->mJustHit = 0;
        store<u32>(a + 0x262C8, flags & ~0x10u);
        actor->mUnused = 1;
        actor->mCurrentLock = 0;
        boomerangSetAimPos(actor);
        if (static_cast<u8>(actor->mCurrentLock) == static_cast<u8>(actor->mLockCount)) {
            boomerangResetLocks(actor);
            s16 yaw = load<s16>(player + 0x32A), aimYaw = load<s16>(player + 0x3D2);
            u16 pitch = load<u16>(player + 0x3D0);
            u16 direction = static_cast<u16>(static_cast<s32>(yaw) + aimYaw);
            f32 range = boomerangGetFlyMax(actor);
            u32 yawTable = 0x104A44F8 + 8 * (direction >> 3), pitchTable = 0x104A44F8 + 8 * (pitch >> 3);
            f32 x = load<f32>(a + 0x314);
            f32 sinYaw = load<f32>(yawTable), cosYaw = load<f32>(yawTable + 4);
            f32 horizontalX = range * sinYaw;
            f32 cosPitch = load<f32>(pitchTable + 4), sinPitch = load<f32>(pitchTable);
            f32 horizontalZ = range * cosYaw;
            f32 y = load<f32>(a + 0x318), z = load<f32>(a + 0x31C);
            store<f32>(a + 0x26280, fmadds(horizontalX, cosPitch, x));
            store<f32>(a + 0x26284, -fmadds(range, sinPitch, -y));
            store<f32>(a + 0x26288, fmadds(horizontalZ, cosPitch, z));
        }
        Local<cXyz> difference, horizontal;
        store<s16>(a + 0x2627A, 0);
        call<void>(0x0201ADE0, at<cXyz>(a + 0x26280), difference.get(), at<cXyz>(a + 0x314));
        f32 dx = difference->x, dz = difference->z;
        horizontal->x = dx; horizontal->z = dz; horizontal->y = zero;
        f32 square = call<f32>(0x028E8DD0, horizontal.get());
        f32 magnitude = call<f32>(0x028F4384, square);
        f32 dy = difference->y;
        s16 pitch = call<s16>(0x020195B0, -dy, magnitude);
        store<s16>(a + 0x320, pitch);
        u8 count = actor->mLockCount;
        dx = difference->x; dz = difference->z;
        s16 direction = call<s16>(0x020195B0, dx, dz);
        if (!count) {
            store<s16>(a + 0x322, static_cast<s16>(static_cast<s32>(direction) + 0x3000));
            store<u8>(a + 0x26276, static_cast<u8>(actor->mThirdPerson) == 0);
        } else {
            store<s16>(a + 0x322, direction);
            store<u8>(a + 0x26276, 0);
        }
        s16 yaw = load<s16>(a + 0x322), vertical = load<s16>(a + 0x320);
        store<s16>(a + 0x32A, yaw);
        store<s16>(a + 0x328, vertical);
        store<s16>(a + 0x32C, 0x2000);
        store<s16>(a + 0x264C0, 0);
        store<u32>(a + 0x264C4, 0x020CEE10);
        store<s16>(a + 0x26278, 0x2000);
        store<s16>(a + 0x264C2, -1);
        actor->mCancel = 0;
        u32 model = load<u32>(a + 0x3AC);
        s16 spin = load<s16>(a + 0x2627A);
        boomerangInitBlur(at<void>(a + 0x2054), model ? at<void>(model + 0xC8) : nullptr, spin);
        f32 playerY = load<f32>(player + 0x318), playerX = load<f32>(player + 0x314), playerZ = load<f32>(player + 0x31C);
        Local<cXyz> start;
        start->x = playerX; start->y = playerY + speed; start->z = playerZ;
        boomerangCheckBgHit(actor, start.get(), at<cXyz>(a + 0x314));
        call<void>(0x020CEE10, actor);
        return 1;
    }
    u32 camera = call<u32>(0x024F8044);
    if (load<u32>(camera + 0x13C) != 0xB) {
        boomerangResetLocks(actor);
        return 1;
    }
    u32 procedure = load<u32>(player + 0x65F0);
    if ((procedure != 0x80 && procedure != 0x8B) || !load<u8>(player + 0x58EC)) return 1;
    play = call<u32>(0x025200D4);
    s32 cameraIndex = load<s8>(play + 0x5B30);
    play = call<u32>(0x025200D4);
    f32 length = load<f32>(0x1000AFD4);
    u32 flags = load<u32>(a + 0x262C8);
    u32 cameraData = load<u32>(play + 0x5AF8 + static_cast<u32>(cameraIndex * 0x34));
    Local<cXyz> cameraPosition, targetPosition;
    store<u32>(ea(cameraPosition.get()) + 0, load<u32>(cameraData + 0xDC));
    store<u32>(ea(cameraPosition.get()) + 4, load<u32>(cameraData + 0xE0));
    store<u32>(ea(cameraPosition.get()) + 8, load<u32>(cameraData + 0xE4));
    f32 x = load<f32>(player + 0x58F0), y = load<f32>(player + 0x58F4), z = load<f32>(player + 0x58F8);
    store<u32>(a + 0x262C8, flags | 0x10);
    targetPosition->x = x; targetPosition->y = y; targetPosition->z = z;
    call<void>(0x020181B0, at<cXyz>(a + 0x263E0), cameraPosition.get(), targetPosition.get(), length);
    call<void>(0x028E8DAC, at<cXyz>(a + 0x263EC), at<cXyz>(a + 0x263E0), at<cXyz>(a + 0x26344));
    store<u32>(a + 0x26320, 0x020CC218);
    play = call<u32>(0x025200D4);
    call<void>(0x0200E240, at<void>(play + 0x26A4), at<void>(a + 0x262C8));
    return 1;
}
VERIFY(0x020CF4CC, boomerangProcWait);

void boomerangCopyBlur(void* blur, void* matrix, s16 angle) {
    WWHD_FUNC(0x020CE7E8, void, blur, matrix, angle);
    u32 a = ea(blur);
    for (s32 i = 54; i >= 0; --i) {
        for (u32 lane = 0; lane < 4; ++lane) {
            u32 source = a + 0xA8 + lane * 0x2D0 + 12 * static_cast<u32>(i);
            u32 x = load<u32>(source), y = load<u32>(source + 4), z = load<u32>(source + 8);
            store<u32>(source + 60, x);
            store<u32>(source + 64, y);
            store<u32>(source + 68, z);
        }
    }
    Local<cXyz> position, temporaryDifference, difference, scaled0, scaled1;
    f32 x = load<f32>(ea(matrix) + 0xC), y = load<f32>(ea(matrix) + 0x1C);
    f32 zero = load<f32>(0x1000AF14), z = load<f32>(ea(matrix) + 0x2C);
    position->y = y; position->x = x; position->z = z;
    call<void>(0x0201ADE0, at<cXyz>(a + 0x9C), temporaryDifference.get(), position.get());
    store<u32>(a + 0x9C, load<u32>(ea(position.get())));
    store<u32>(ea(difference.get()), load<u32>(ea(temporaryDifference.get())));
    store<u32>(ea(difference.get()) + 4, load<u32>(ea(temporaryDifference.get()) + 4));
    store<u32>(a + 0xA0, load<u32>(ea(position.get()) + 4));
    store<u32>(a + 0xA4, load<u32>(ea(position.get()) + 8));
    store<u32>(ea(difference.get()) + 8, load<u32>(ea(temporaryDifference.get()) + 8));
    call<void>(0x025F23EC);
    f32 step = load<f32>(0x1000AFA0), t = zero;
    for (u32 i = 0; i < 5; ++i) {
        u32 top = a + 0xA8 + 12 * i, root = a + 0x378 + 12 * i;
        call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462724), at<void>(top));
        call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462730), at<void>(root));
        call<void>(0x0201AE48, difference.get(), scaled0.get(), t);
        call<void>(0x028E8D88, at<cXyz>(top), scaled0.get(), at<cXyz>(top));
        call<void>(0x0201AE48, difference.get(), scaled0.get(), t);
        call<void>(0x028E8D88, at<cXyz>(root), scaled0.get(), at<cXyz>(root));
        t += step;
        call<void>(0x025F1C28, at<void>(0x1048D0CC), static_cast<s16>(0x633));
    }
    t = zero;
    call<void>(0x025F2468);
    call<void>(0x025F1C28, at<void>(0x1048D0CC), static_cast<s16>(-2 * static_cast<s32>(angle)));
    for (u32 i = 0; i < 5; ++i) {
        u32 top = a + 0x648 + 12 * i, root = a + 0x918 + 12 * i;
        call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462724), at<void>(top));
        call<void>(0x028E8F64, at<void>(0x1048D0CC), at<void>(0x10462730), at<void>(root));
        call<void>(0x0201AE48, difference.get(), scaled1.get(), t);
        call<void>(0x028E8D88, at<cXyz>(top), scaled1.get(), at<cXyz>(top));
        call<void>(0x0201AE48, difference.get(), scaled1.get(), t);
        call<void>(0x028E8D88, at<cXyz>(root), scaled1.get(), at<cXyz>(root));
        t += step;
        call<void>(0x025F1C28, at<void>(0x1048D0CC), static_cast<s16>(-0x633));
    }
    s32 count = static_cast<s32>(load<u32>(a + 0x98) + 5u);
    store<s32>(a + 0x98, count >= 59 ? 58 : count);
}
VERIFY(0x020CE7E8, boomerangCopyBlur);

static void boomerangConstructCollision(u32 a) {
    call<void>(0x027FD6F4, at<void>(a));
    call<void>(0x027FB40C, at<void>(a + 0xC));
    store<u32>(a + 0x18, 0x1016EF84);
    call<void>(0x028F521C, at<void>(a + 0x80), 0x34);
    if (a + 0x80 == 0) call<void>(0x0273AD10, 0x30);
    call<void>(0x027FB40C, at<void>(a + 0xB4));
    store<u32>(a + 0xC0, 0x1016EFB4);
    call<void>(0x028F521C, at<void>(a + 0x128), 0x2F0);
    f32 zero = load<f32>(0x10145180);
    store<f32>(a + 0x128, zero);
    store<f32>(a + 0x12C, zero);
    store<f32>(a + 0x130, zero);
    f32 one = load<f32>(0x1014517C);
    const u16 offsets[] = {0x138,0x154,0x160,0x144,0x140,0x190,0x1B0,0x180,0x1A4,0x158,0x17C,0x1AC,0x19C,0x16C,0x148,0x15C,0x164,0x14C,0x170,0x13C,0x198,0x1A0,0x1B4,0x184,0x194,0x18C,0x178,0x1A8,0x150,0x188,0x134,0x168,0x174,0x1B8,0x1BC,0x1C0,0x1C4,0x1C8,0x1CC,0x1D0,0x1D4};
    for (u16 offset : offsets) {
        bool identity = offset == 0x154 || offset == 0x144 || offset == 0x1A4 || offset == 0x164 || offset == 0x1B4 || offset == 0x184 || offset == 0x194 || offset == 0x134 || offset == 0x174 || offset == 0x1C4 || offset == 0x1D4;
        store<f32>(a + offset, identity ? one : zero);
    }
    for (u32 offset : {0x1D8u, 0x1F8u, 0x218u})
        call<void>(0x028EFFD0, at<void>(a + offset), 2, 0x10, 0x020CF9FC);
    for (u32 offset = 0x238; offset <= 0x388; offset += 0x30)
        if (a + offset == 0) call<void>(0x0273AD10, 0x30);
    for (u32 offset = 0x3B8; offset <= 0x408; offset += 0x10)
        if (a + offset == 0) call<void>(0x0273AD10, 0x10);
    store<u8>(a + 0x418, 0);
}

void* boomerangCollisionCtor(void* object) {
    WWHD_FUNC(0x020CFA28, void*, object);
    if (!object) object = call<void*>(0x0273AD10, 0x41C);
    if (object) boomerangConstructCollision(ea(object));
    return object;
}
VERIFY(0x020CFA28, boomerangCollisionCtor);

BOOL boomerangProcMove(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CEE10, BOOL, actor);
    u32 a = ea(actor), lock = a + 0x26244;
    if (!load<u32>(0x1046273C)) store<u32>(0x1046273C, 1);
    if (load<u8>(lock + 0x2B)) {
        call<void>(0x025D57E0, actor);
        return 1;
    }
    u32 play = call<u32>(0x025200D4);
    if (!load<u8>(play + 0x5292)) {
        play = call<u32>(0x025200D4);
        s16 oldSpin = load<s16>(lock + 0x36), spin = static_cast<s16>(static_cast<s32>(oldSpin) - 0x1F00);
        u32 player = load<u32>(play + 0x5B34);
        store<s16>(lock + 0x36, spin);
        if (oldSpin >= 0 && spin < 0) {
            s32 reverb = call<s32>(0x02520540, static_cast<s32>(load<s8>(a + 0x326)));
            call<void>(0x025E1A40, 0x2814, at<cXyz>(a + 0x37C), 0, reverb);
        }
        boomerangSetAimPos(actor);
        Local<cXyz> difference, normalized;
        call<void>(0x0201ADE0, at<cXyz>(lock + 0x3C), difference.get(), at<cXyz>(a + 0x314));
        f32 square = call<f32>(0x028E8DD0, difference.get());
        f32 distance = call<f32>(0x028F4384, square);
        cXyz* direction = difference.get();
        if (distance > load<f32>(0x1000AFB4)) {
            call<void>(0x0201AEAC, difference.get(), normalized.get(), distance);
            direction = normalized.get();
        }
        f32 dz = direction->z, dx = direction->x, dy = direction->y;
        s32 desiredYaw = call<s16>(0x020195B0, dx, dz);
        s16 currentYaw = load<s16>(a + 0x322);
        s16 delta = static_cast<s16>(desiredYaw - currentYaw);
        if (load<u8>(lock + 0x2A)) {
            store<u8>(lock + 0x2A, 0);
            s16 yaw = load<s16>(a + 0x322);
            s16 remaining = static_cast<s16>(desiredYaw - yaw);
            s32 adjusted = desiredYaw + (remaining > 0 ? -0x3000 : 0x3000);
            delta = static_cast<s16>(desiredYaw - adjusted);
            store<s16>(a + 0x322, static_cast<s16>(adjusted));
        }
        f32 speed = load<f32>(a + 0x370);
        if (load<u8>(lock + 0x28)) {
            if (distance < speed + speed) {
                if (call<u32>(0x02442ED8, at<void>(player))) {
                    store<u32>(a + 0xB0, 0);
                    u8 returning = load<u8>(lock + 0x28);
                    store<u8>(lock + 0x2F, 0);
                    store<u32>(lock + 0x280, 0x020CF4CC);
                    store<s16>(lock + 0x27C, 0);
                    store<u32>(lock + 0xDC, 0);
                    store<s16>(lock + 0x27E, -1);
                    (void)returning;
                } else store<u8>(lock + 0x2B, 1);
            }
        } else {
            bool advance = distance < speed;
            if (!advance && call<u32>(0x025160DC, at<void>(lock + 0x84))) {
                u32 hit = call<u32>(0x02515BBC, at<void>(lock + 0xD4));
                u8 index = load<u8>(lock + 0x2E);
                advance = hit == load<u32>(lock + 0x14 + 4 * index);
            }
            if (advance) {
                u8 index = load<u8>(lock + 0x2E);
                store<u8>(lock + 0x2A, 1);
                store<u32>(lock + 0x14 + 4 * index, 0);
                index = load<u8>(lock + 0x2E);
                store<u32>(lock + 4 * index, 0xFFFFFFFF);
                index = load<u8>(lock + 0x2E);
                store<u8>(lock + 0x2E, static_cast<u8>(index + 1));
                store<u8>(lock + 0x32, 0);
            }
        }
        if (!load<u8>(lock + 0x28)) {
            bool turnBack = load<u8>(lock + 0x30) != 0;
            if (!turnBack) {
                bool remaining = load<u8>(lock + 0x32) || load<u8>(lock + 0x2D) > load<u8>(lock + 0x2E);
                turnBack = !remaining || (load<u32>(lock + 0xD8) & 1);
            }
            if (turnBack) {
                store<u8>(lock + 0x2A, 1);
                store<u8>(lock + 0x28, 1);
                boomerangResetLocks(actor);
                store<u8>(lock + 0x30, 0);
                store<u8>(lock + 0x32, 0);
            }
        }
        f32 zero = load<f32>(0x1000AF14);
        if (load<u8>(lock + 0x2A)) store<s16>(a + 0x322, static_cast<s16>(desiredYaw));
        else {
            s32 limit;
            if (load<u8>(lock + 0x2F)) limit = 0x4000;
            else {
                f32 speedNow = load<f32>(a + 0x370);
                f32 factor = load<f32>(0x1000AFBC) - (distance + distance) / speedNow;
                if (factor < zero) factor = zero;
                else if (factor > load<f32>(0x1000AFC0))
                    factor = fmadds(load<f32>(0x1000AFC4), factor - load<f32>(0x1000AFC0), load<f32>(0x1000AFBC));
                limit = static_cast<s16>(ftoi(fmadds(factor, load<f32>(0x1000AFC8), load<f32>(0x1000AFCC))));
            }
            s16 yaw = load<s16>(a + 0x322);
            if (delta > limit) delta = static_cast<s16>(limit);
            else if (delta < -limit) delta = static_cast<s16>(-limit);
            store<s16>(a + 0x322, static_cast<s16>(static_cast<s32>(yaw) + delta));
        }
        Local<cXyz> horizontal;
        horizontal->y = zero; horizontal->z = dz; horizontal->x = dx;
        square = call<f32>(0x028E8DD0, horizontal.get());
        f32 horizontalLength = call<f32>(0x028F4384, square);
        s16 pitch = call<s16>(0x020195B0, -dy, horizontalLength);
        u16 pitchBits = static_cast<u16>(pitch), yawBits = load<u16>(a + 0x322);
        u32 pitchTable = 0x104A44F8 + 8 * (pitchBits >> 3), yawTable = 0x104A44F8 + 8 * (yawBits >> 3);
        speed = load<f32>(a + 0x370);
        store<s16>(a + 0x320, pitch);
        f32 cosPitch = load<f32>(pitchTable + 4), x = load<f32>(a + 0x314);
        f32 horizontalSpeed = speed * cosPitch, sinYaw = load<f32>(yawTable);
        f32 nextX = fmadds(horizontalSpeed, sinYaw, x);
        f32 y = load<f32>(a + 0x318);
        store<f32>(a + 0x314, nextX);
        f32 sinPitch = load<f32>(pitchTable), nextY = -fmadds(speed, sinPitch, -y);
        f32 z = load<f32>(a + 0x31C);
        store<f32>(a + 0x318, nextY);
        cosPitch = load<f32>(pitchTable + 4);
        f32 cosYaw = load<f32>(yawTable + 4);
        f32 nextZ = fmadds(speed * cosPitch, cosYaw, z);
        store<s16>(a + 0x32A, static_cast<s16>(yawBits));
        store<s16>(a + 0x328, pitch);
        store<f32>(a + 0x31C, nextZ);
        if (!load<u8>(lock + 0x28)) boomerangCheckBgHit(actor, at<cXyz>(a + 0x300), at<cXyz>(a + 0x314));
        s16 rollChange = static_cast<s16>(-2 * static_cast<s32>(delta));
        s16 roll;
        if (rollChange > 0x100) roll = 0x2000;
        else if (rollChange < -0x100) roll = -0x2000;
        else roll = load<s16>(lock + 0x34);
        store<s16>(lock + 0x34, roll);
        call<void>(0x0200F378, at<be<s16>>(a + 0x32C), roll, 0x10, 0x1000, 0x10);
    }
    f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318), z = load<f32>(a + 0x31C);
    call<void>(0x028E93CC, at<void>(0x1048D0CC), x, y, z);
    s16 rx = load<s16>(a + 0x328), ry = load<s16>(a + 0x32A), rz = load<s16>(a + 0x32C);
    call<void>(0x025F1B48, at<void>(0x1048D0CC), rx, ry, rz);
    s16 spin = load<s16>(lock + 0x36);
    call<void>(0x025F1C28, at<void>(0x1048D0CC), spin);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = load<f32>(0x1048D0CC + 4 * i);
    u32 model = load<u32>(a + 0x3AC);
    for (u32 i = 0; i < 12; ++i) store<f32>(model + 0xC8 + 4 * i, matrix[i]);
    f32 radius = load<f32>(0x1000AE98);
    call<void>(0x020181B0, at<cXyz>(lock + 0x19C), at<cXyz>(a + 0x300), at<cXyz>(a + 0x314), radius);
    call<void>(0x028E8DAC, at<cXyz>(lock + 0x1A8), at<cXyz>(lock + 0x19C), at<cXyz>(lock + 0x100));
    play = call<u32>(0x025200D4);
    if (!load<u8>(play + 0x5292)) {
        spin = load<s16>(lock + 0x36);
        boomerangCopyBlur(at<void>(a + 0x2054), at<void>(0x1048D0CC), spin);
        play = call<u32>(0x025200D4);
        call<void>(0x0200E240, at<void>(play + 0x26A4), at<void>(lock + 0x84));
        play = call<u32>(0x025200D4);
        call<void>(0x02516C14, at<void>(play + 0x4EF8), at<void>(lock + 0x84), 1);
    } else {
        boomerangCopyBlur(at<void>(a + 0x2054), at<void>(0x1048D0CC), 0);
        call<void>(0x02516138, at<void>(lock + 0x84));
    }
    u8 water = load<u8>(lock + 0x31);
    store<u8>(lock + 0x31, call<u8>(0x02443208, actor, water, 1));
    return 1;
}
VERIFY(0x020CEE10, boomerangProcMove);

static void boomerangConstructTrailArray(u32 storage, u32 count, u32 ctor) {
    u32 bytes = count * 0x254;
    if (!storage) storage = call<u32>(0x0273AD10, bytes + 0x18);
    if (!storage) return;
    call<void>(0x028EFFD0, at<void>(storage), count, 0x254, ctor);
    store<u32>(storage + bytes, 0);
    store<u8>(storage + bytes + 0x14, 0);
    store<u32>(storage + bytes + 8, 0x14);
    store<u32>(storage + bytes + 4, 0);
    store<u32>(storage + bytes + 0x10, 0);
    for (u32 i = 0; i < count; ++i) store<u32>(storage + 0x254 * i, 0);
}

BOOL boomerangCreate(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CD368, BOOL, actor);
    u32 a = ea(actor);
    if (!(load<u32>(a + 0x2E4) & 8)) {
        if (a) {
            call<void>(0x025D4ED0, actor);
            store<u32>(a + 0xB4, 0x1000AE74);
            call<void>(0x0252CCBC, at<void>(a + 0x3B0));
            store<u32>(a + 0x3B0, 0x1000AFFC);
            boomerangConstructTrailArray(a + 0x4C0, 2, 0x020CF9A0);
            call<void>(0x027B5430, at<void>(a + 0x980));
            call<void>(0x028EFFD0, at<void>(a + 0x9A0), 5, 0x41C, 0x020CFA28);
            call<void>(0x027BE6B8, at<void>(a + 0x1E2C));
            call<void>(0x027BDF7C, at<void>(a + 0x1EBC));
            call<void>(0x027F1278, at<void>(a + 0x2054));
            store<u32>(a + 0x2060, 0x1000B014);
            boomerangConstructTrailArray(a + 0x2C40, 120, 0x020CFCEC);
            boomerangConstructTrailArray(a + 0x143B8, 120, 0x020CFCEC);
            boomerangConstructCollision(a + 0x25B30);
            call<void>(0x027B5430, at<void>(a + 0x25F4C));
            call<void>(0x027BDF7C, at<void>(a + 0x25F6C));
            call<void>(0x027BE6B8, at<void>(a + 0x26104));
            call<void>(0x02080404, at<void>(a + 0x26194));
            u32 status = a + 0x2628C;
            call<void>(0x0200BD2C, at<void>(status));
            call<void>(0x02515DA0, at<void>(status + 0x1C));
            store<u32>(status + 0x18, 0x1004AE88);
            store<u32>(status + 0x1C, 0x1004AEC0);
            u32 capsule = a + 0x262C8;
            call<void>(0x02515FB8, at<void>(capsule));
            store<u32>(capsule + 0x114, 0x100015A8);
            store<u32>(capsule + 0x110, 0x1000AD44);
            call<void>(0x02018150, at<void>(capsule + 0x118));
            store<u32>(capsule + 0x130, 0x1004AF60);
            store<u32>(capsule + 0x3C, 0x1004AF18);
            store<u32>(capsule + 0x114, 0x1004AF70);
            u32 line = a + 0x26400;
            call<void>(0x02008FEC, at<void>(line));
            store<u8>(line + 0x5E, 0);
            store<u8>(line + 0x60, 0);
            store<u8>(line + 0x5C, 0);
            store<u32>(line + 0x20, 0x1000AE44);
            store<u8>(line + 0x5F, 0);
            store<u8>(line + 0x5D, 0);
            store<u32>(line + 0x68, 1);
            store<u8>(line + 0x62, 0);
            store<u32>(line, line + 0x58);
            store<u32>(line + 0x64, 0x1000AE54);
            store<u32>(line + 0x58, 0x1000AE64);
            store<u32>(line + 4, line + 0x64);
            store<u32>(line + 0x10, 0x1000AE34);
            store<u8>(line + 0x61, 1);
            u32 ground = a + 0x2646C;
            call<void>(0x02008E0C, at<void>(ground));
            store<u8>(ground + 0x47, 0);
            store<u32>(ground + 0x50, 1);
            store<u8>(ground + 0x46, 0);
            store<u8>(ground + 0x44, 1);
            store<u8>(ground + 0x48, 0);
            store<u32>(ground, ground + 0x40);
            store<u32>(ground + 4, ground + 0x4C);
            store<u32>(ground + 0x10, 0x1000ADB4);
            store<u32>(ground + 0x4C, 0x1000ADD4);
            store<u32>(ground + 0x40, 0x1000ADE4);
            store<u8>(ground + 0x4A, 0);
            store<u32>(ground + 0x20, 0x1000ADC4);
            store<u8>(ground + 0x49, 0);
            store<u8>(ground + 0x45, 0);
        }
        store<u32>(a + 0x2E4, load<u32>(a + 0x2E4) | 8);
    }
    call<void>(0x020CC5EC, at<void>(a + 0x3B0));
    call<void>(0x020CCA50, at<void>(a + 0x2054));
    if (!call<u32>(0x025D63E8, actor, 0x020CC5E8, 0xD40)) return 5;
    boomerangSetKeepMatrix(actor);
    u32 model = load<u32>(a + 0x3AC);
    store<u32>(a + 0x348, model ? model + 0xC8 : 0);
    store<s16>(a + 0x264C0, 0);
    store<s16>(a + 0x264C2, -1);
    store<u32>(a + 0x264C4, 0x020CF4CC);
    call<void>(0x02515F14, at<void>(a + 0x2628C), 0x3C, 0xFF, actor);
    call<void>(0x025164C0, at<void>(a + 0x262C8), at<void>(0x1019241C));
    store<u32>(a + 0x2630C, a + 0x2628C);
    store<u32>(a + 0x26320, 0x020CC218);
    store<u32>(a + 0x2644C, load<u32>(a + 0x2644C) & ~0x20000000u);
    for (u32 i = 0; i < 5; ++i) actor->mLockActorIDs[i] = 0xFFFFFFFF;
    boomerangSetRoomInfo(actor);
    store<u32>(a + 0x368, load<u32>(a + 0x3AC));
    return 4;
}
VERIFY(0x020CD368, boomerangCreate);

BOOL boomerangCreateWrapper(daBoomerang_c* actor) {
    WWHD_FUNC(0x020CDAA4, BOOL, actor);
    return boomerangCreate(actor);
}
VERIFY(0x020CDAA4, boomerangCreateWrapper);

static u32 boomerangFindRenderResource(u32 name) {
    Local<BoomerangString_l> key;
    key->vtable = 0x1000AD2C;
    key->text = name;
    u32 manager = call<u32>(0x027FFCBC);
    u32 archive = load<u32>(manager + 4);
    s32 index = call<s32>(0x027B90AC, at<void>(archive), key.get());
    if (index < 0) return 0;
    u32 count = load<u32>(manager + 8), records = load<u32>(manager + 0xC);
    u32 record = records + (static_cast<u32>(index) < count ? static_cast<u32>(index) * 0x24 : 0);
    if (!load<u8>(record + 0x20)) {
        archive = load<u32>(manager + 4);
        u32 available = load<u32>(archive + 0x1C);
        u32 data = static_cast<u32>(index) < available ? load<u32>(archive + 0x20) + static_cast<u32>(index) * 0x84 : 0;
        call<void>(0x02800B0C, at<void>(record), at<void>(data), 0);
        count = load<u32>(manager + 8);
        records = load<u32>(manager + 0xC);
    }
    return records + (static_cast<u32>(index) < count ? static_cast<u32>(index) * 0x24 : 0);
}

static void boomerangZeroCacheRange(u32 pointer, u32 bytes) {
    u32 end = pointer + bytes;
    while (pointer < end) {
        u32 block = pointer & ~31u;
        for (u32 i = 0; i < 32; ++i) store<u8>(block + i, 0);
        pointer += 32;
    }
}

void boomerangSetupSight(void* sight) {
    WWHD_FUNC(0x020CC5EC, void, sight);
    u32 a = ea(sight), resource = boomerangFindRenderResource(0x1000AF20);
    store<u32>(a + 0x10C, resource);
    store<u32>(a + 0x5BC, 0x11);
    store<u32>(a + 0x5C4, 0x1000AFE8);
    u32 points = a + 0x110;
    for (u32 i = 0; i < 2; ++i) {
        u32 point = points + i * 0x254;
        u32 vertices = load<u32>(point);
        if (!vertices) {
            u32 allocator = load<u32>(0x101F8B4C);
            u32 heap = call<u32>(0x02756140, at<void>(allocator));
            u32 vtable = load<u32>(heap + 0xC);
            u32 buffer = call<u32>(load<u32>(vtable + 0x34), at<void>(heap), 0x50, 0x40);
            if (buffer) {
                store<u32>(point + 0x250, buffer);
                store<u32>(point + 0x24C, 4);
            }
            vertices = load<u32>(point + 0x250);
            store<u32>(point, vertices);
        }
        call<void>(0x027FF478, at<void>(point + 4), at<void>(vertices), 4, at<void>(points + 0x4AC));
        if (resource && resource != load<u32>(points + 0x4B8))
            call<void>(0x027FF530, at<void>(resource), at<void>(point + 0x158), at<void>(point + 4), at<void>(points + 0x4AC), 0);
    }
    store<u32>(points + 0x4B8, resource);
    store<u8>(points + 0x4BC, 1);
    for (u32 i = 0; i < 5; ++i) call<void>(0x027FE084, at<void>(a + 0x5F0 + 0x41C * i), 1, 0);
    store<u16>(a + 0x5E8, 0); store<u16>(a + 0x5EA, 2);
    store<u16>(a + 0x5EC, 1); store<u16>(a + 0x5EE, 3);
    call<void>(0x027B54E0, at<void>(a + 0x5D0), at<void>(a + 0x5E8), 4, 4);
    store<u32>(a + 0x5D4, 6);
    u32 index = load<u32>(points + 0x4A8);
    u32 buffer = load<u32>(points + index * 0x254);
    boomerangZeroCacheRange(buffer, 0x40);
    index = load<u32>(a + 0x5B8);
    buffer = load<u32>(points + index * 0x254);
    f32 zero = load<f32>(0x1000AF14), one = load<f32>(0x1000AE88), minus = load<f32>(0x1000AF10);
    const u16 order[] = {0x4C,0x40,0,0x2C,0x28,0x1C,0x20,0xC,0x38,0x30,8,0x34,0x48,0x10,4,0x3C,0x14,0x18,0x44,0x24};
    for (u16 offset : order) {
        f32 value = (offset == 0x40 || offset == 0 || offset == 0x2C || offset == 0x28) ? minus :
            (offset == 0x4C || offset == 0x20 || offset == 0x38 || offset == 0x48 || offset == 4 || offset == 0x3C || offset == 0x14 || offset == 0x18) ? one : zero;
        store<f32>(buffer + offset, value);
    }
    index = load<u32>(points + 0x4A8);
    u32 point = points + index * 0x254;
    u32 storage = load<u32>(point + 0x150);
    call<void>(0x027B5E94, at<void>(point + 4), 0, at<void>(storage));
    index = load<u32>(points + 0x4A8);
    store<u32>(points + 0x4A8, index == 0);
    u32 graphics = load<u32>(0x101F8B18);
    call<void>(0x0274FBF8, at<void>(graphics));
    Local<BoomerangString_l> textureName;
    u32 manager = load<u32>(0x101F4F28);
    textureName->vtable = 0x1000AD2C;
    textureName->text = 0x1000AF18;
    u32 texture = call<u32>(0x026066C4, at<void>(manager), textureName.get(), 0x72);
    if (!texture) call<void>(0x0273AA24, at<void>(0x1000AF30), 0x2DB, at<void>(0x1000AF44));
    u32 textureData = load<u32>(texture + 0x20);
    call<void>(0x02773798, at<void>(a + 0x1A7C), at<void>(textureData));
    store<u32>(a + 0x108, texture);
    graphics = load<u32>(0x101F8B18);
    call<void>(0x0274FCCC, at<void>(graphics));
    bool equal = true;
    for (u32 offset : {4u,8u,12u,16u,20u,24u,56u,52u,28u}) {
        if (load<u32>(a + 0x1B0C + offset) != load<u32>(a + 0x1A7C + offset)) { equal = false; break; }
    }
    if (!equal) call<void>(0x027BDEB4, at<void>(a + 0x1B0C), at<void>(a + 0x1A7C));
    else {
        u32 data = load<u32>(a + 0x1AA4), size = load<u32>(a + 0x1AAC);
        store<u32>(a + 0x1B34, data);
        store<u32>(a + 0x1BE8, size);
        store<u32>(a + 0x1B3C, size);
        store<u32>(a + 0x1BE0, data);
    }
    u8 flags = load<u8>(a + 0x1C9C);
    store<u32>(a + 0x1C6C, 2); store<u32>(a + 0x1C68, 2); store<u32>(a + 0x1C70, 2);
    store<u8>(a + 0x1C9C, flags | 2);
}
VERIFY(0x020CC5EC, boomerangSetupSight);

static void boomerangBindRenderResource(u32 owner, u32 resourceOffset = 0x10C) {
    u32 state = call<u32>(0x027F29D4, at<void>(0x104B45C0));
    u32 resource = load<u32>(owner + resourceOffset);
    u32 current = load<u32>(state + 4), material = load<u32>(resource);
    if (material == current) return;
    u8 flags = load<u8>(material);
    u32 oldShader = load<u32>(state);
    if (flags & 2) {
        store<u8>(material, flags & ~2u);
        call<void>(0x027BB9E0, at<void>(material), 0);
    }
    u32 shaderOwner = load<u32>(material + 0x7C), shader = load<u32>(shaderOwner + 0x28);
    if (oldShader != shader) call<void>(0x027B9F68, at<void>(shader));
    u32 bytes = load<u32>(material + 0xC);
    if (bytes) {
        u32 data = load<u32>(material + 4);
        call<void>(0xC00060E0, at<void>(data), bytes);
        store<u32>(state, shader);
        store<u32>(state + 4, material);
    } else {
        call<void>(0x027BB7CC, at<void>(material));
        store<u32>(state + 4, material);
        store<u32>(state, shader);
    }
}

void boomerangDrawSight(void* sight) {
    WWHD_FUNC(0x020CDBFC, void, sight);
    u32 a = ea(sight);
    boomerangBindRenderResource(a);
    struct GraphicsState_l { u8 bytes[0x11C]; };
    struct Matrix_l { be<f32> values[12]; };
    struct Projection_l { u8 bytes[0x40]; };
    Local<GraphicsState_l> state;
    call<void>(0x02750250, state.get());
    u32 s = ea(state.get());
    u32 flags = load<u32>(s + 0xEC);
    store<u32>(s + 0xC, 3); store<u32>(s + 8, 2);
    store<u8>(s, 0); store<u8>(s + 1, 0); store<u8>(s + 0xE0, 0);
    store<u32>(s + 0xEC, ((flags & ~15u) + 7u & 0xFFFFFF0Fu) + 0x10);
    call<void>(0x02750370, state.get());
    Local<Matrix_l> matrix;
    boomerangCopyMatrix(matrix.get(), at<void>(0x104B45F8));
    call<void>(0x028E9098, matrix.get());
    u32 camera = call<u32>(0x0274D80C, at<void>(0x104B465C));
    Local<Projection_l> projection;
    call<void>(0x028E8970, at<void>(camera), projection.get());
    u32 render = a + 0x5F0, system = load<u32>(0x104B4708);
    call<void>(0x027FDA54, at<void>(render), 0, matrix.get(), projection.get(), at<void>(system + 0x240));
    call<void>(0x027FDFF4, at<void>(render), 0);
    u32 output = load<u32>(a + 0x5F4), resource = load<u32>(a + 0x10C);
    u32 ordinal = load<u32>(output + 0x4C), selection = output + 0x10 + ordinal * 0x1C;
    u32 slots = load<u32>(resource + 0xC) ? load<u32>(resource + 0x10) : 0;
    s16 vertexSlot = load<s16>(slots + 0xC);
    u32 stride = load<u32>(selection + 4);
    s16 pixelSlot = load<s16>(slots + 0xE);
    u32 data = load<u32>(selection + 0xC);
    s16 geometrySlot = load<s16>(slots + 0x10);
    if (vertexSlot != -1 || pixelSlot != -1 || geometrySlot != -1) {
        if (pixelSlot != -1) call<void>(0xC0006900, pixelSlot, at<void>(data), stride);
        if (vertexSlot != -1) call<void>(0xC0006A38, vertexSlot, at<void>(data), stride);
        if (geometrySlot != -1) call<void>(0xC00068A8, geometrySlot, at<void>(data), stride);
        resource = load<u32>(a + 0x10C);
    }
    u32 texture = load<u32>(resource + 0x14) ? load<u32>(resource + 0x18) : 0;
    call<void>(0x027BE53C, at<void>(a + 0x1B0C), at<void>(texture + 4), -1, 0);
    Local<be<u8>[8]> colors;
    Local<Matrix_l> localMatrix;
    for (u32 i = 0; i < 5; ++i) {
        if (!(load<u32>(a + 0x100) & (1u << i))) continue;
        u8 alpha = load<u8>(a + 0xF9 + i);
        u32 c = ea(colors.get());
        store<u8>(c, 255); store<u8>(c + 1, 255); store<u8>(c + 2, 50); store<u8>(c + 3, alpha);
        store<u8>(c + 4, 255); store<u8>(c + 5, 255); store<u8>(c + 6, 50); store<u8>(c + 7, alpha);
        u32 item = render + i * 0x41C;
        boomerangColorToFloat(at<void>(item + 0x168), at<void>(c));
        boomerangColorToFloat(at<void>(item + 0x178), at<void>(c + 4));
        for (u32 j = 0; j < 4; ++j) store<u32>(item + 0x148 + 4 * j, load<u32>(0x104A01EC + 4 * j));
        call<void>(0x027FB678, at<void>(item + 0xB4));
        u32 vtable = load<u32>(item + 0xC0);
        resource = load<u32>(a + 0x10C);
        call<void>(load<u32>(vtable + 0x2C), at<void>(item + 0xB4), at<void>(resource));
        boomerangCopyMatrix(localMatrix.get(), at<void>(a + 4 + i * 0x30));
        call<void>(0x028E90D4, localMatrix.get(), at<void>(item + 0x80));
        call<void>(0x027FB678, at<void>(item + 0xC));
        vtable = load<u32>(item + 0x18);
        resource = load<u32>(a + 0x10C);
        call<void>(load<u32>(vtable + 0x2C), at<void>(item + 0xC), at<void>(resource));
        u32 selected = load<u32>(a + 0x5B8) == 0;
        call<void>(0x027BFE5C, at<void>(a + 0x110 + selected * 0x254 + 0x158));
        u32 primitive = load<u32>(a + 0x5DC);
        if (primitive) {
            u32 vertices = load<u32>(a + 0x5D0), vertexOffset = load<u32>(a + 0x5D8), count = load<u32>(a + 0x5D4);
            call<void>(0xC0006178, count, primitive, at<void>(vertices), vertexOffset, 0, 1);
        }
    }
}
VERIFY(0x020CDBFC, boomerangDrawSight);

static void boomerangReleaseBlurPoint(u32 point) {
    call<void>(0x027BF7E8, at<void>(point + 0x158));
    u32 buffer = load<u32>(point + 0x250);
    store<u32>(point, 0);
    if (buffer) {
        u32 heap = call<u32>(0x02755FEC, at<void>(load<u32>(0x101F8B4C)), at<void>(buffer));
        u32 table = load<u32>(heap + 0xC);
        call<void>(load<u32>(table + 0x3C), at<void>(heap), at<void>(load<u32>(point + 0x250)));
        store<u32>(point + 0x24C, 0);
        store<u32>(point + 0x250, 0);
    }
}

static void boomerangReleaseBlurRing(u32 ring) {
    for (u32 i = 0; i < 60; ++i) {
        boomerangReleaseBlurPoint(ring + i * 0x4A8);
        boomerangReleaseBlurPoint(ring + i * 0x4A8 + 0x254);
    }
}

static void boomerangDestroyBlurContents(u32 a) {
    u32 first = a + 0xBEC, second = a + 0x12364;
    store<u32>(a + 0xC, 0x1000B014);
    boomerangReleaseBlurRing(first);
    store<u32>(first + 0x11770, 0);
    boomerangReleaseBlurRing(second);
    store<u32>(second + 0x11770, 0);
    u32 render = a + 0x23ADC;
    for (s32 i = 0; i < load<s32>(render); ++i) {
        u32 element = load<u32>(render + 4);
        if (static_cast<u32>(i) < load<u32>(render)) element += static_cast<u32>(i) * 0x23C;
        call<void>(0x027BEBEC, at<void>(element + 0x10));
        call<void>(0x027BEBEC, at<void>(element + 0x2C));
    }
    call<void>(0x027BEBEC, at<void>(render + 0x1C));
    call<void>(0x027BEBEC, at<void>(render + 0x38));
    call<void>(0x027BEBEC, at<void>(render + 0xC4));
    call<void>(0x027BEBEC, at<void>(render + 0xE0));
    call<void>(0x027BE2B0, at<void>(a + 0x23F18), 2);
    call<void>(0x027B54A0, at<void>(a + 0x23EF8), 2);
    call<void>(0x027FB528, at<void>(render + 0xB4), 0);
    call<void>(0x027FB528, at<void>(render + 0xC), 0);
    call<void>(0x027FD764, at<void>(render), 2);
    if (second) {
        boomerangReleaseBlurRing(second);
        store<u32>(second + 0x11770, 0);
        call<void>(0x028F0164, at<void>(second), 120, 0x254, 0x020CFE14, 0, 0);
    }
    if (first) {
        boomerangReleaseBlurRing(first);
        store<u32>(first + 0x11770, 0);
        call<void>(0x028F0164, at<void>(first), 120, 0x254, 0x020CFE14, 0, 0);
    }
    call<void>(0x027F13DC, at<void>(a), 0);
}

void boomerangBlurDestructor(void* object, s32 flags) {
    WWHD_FUNC(0x020CFE74, void, object, flags);
    if (!object) return;
    boomerangDestroyBlurContents(ea(object));
    if (flags & 1) call<void>(0x0273AF40, object);
}
VERIFY(0x020CFE74, boomerangBlurDestructor);

static void boomerangDestroyRenderBuffers(u32 render) {
    s32 count = load<s32>(render);
    for (s32 i = 0; i < count; ++i) {
        u32 element = load<u32>(render + 4);
        if (static_cast<u32>(i) < static_cast<u32>(count)) element += static_cast<u32>(i) * 0x23C;
        call<void>(0x027BEBEC, at<void>(element + 0x10));
        call<void>(0x027BEBEC, at<void>(element + 0x2C));
        count = load<s32>(render);
    }
    call<void>(0x027BEBEC, at<void>(render + 0x1C));
    call<void>(0x027BEBEC, at<void>(render + 0x38));
    call<void>(0x027BEBEC, at<void>(render + 0xC4));
    call<void>(0x027BEBEC, at<void>(render + 0xE0));
}

void boomerangDestructor(daBoomerang_c* actor, s32 flags) {
    WWHD_FUNC(0x020D02FC, void, actor, flags);
    if (!actor) return;
    u32 a = ea(actor), render = a + 0x23ADC;
    store<u32>(render + 0x29D0, 0x1000ADA4);
    store<u32>(render + 0x29DC, 0x1000AD64);
    store<u32>(render + 0x29B0, 0x1000AD84);
    call<void>(0x02008DAC, at<void>(render + 0x2990), 0);
    store<u32>(render + 0x2944, 0x1000AD54);
    store<u32>(render + 0x297C, 0x1000AE24);
    store<u32>(render + 0x2988, 0x1000AD64);
    call<void>(0x02008B4C, at<void>(render + 0x2924), 0);
    call<void>(0x02515980, at<void>(render + 0x27EC), 2);
    call<void>(0x02515860, at<void>(render + 0x27B0), 2);
    call<void>(0x02082DDC, at<void>(render + 0x26B8), 2);
    boomerangDestroyBlurContents(a + 0x2054);
    if (a + 0x3B0) {
        store<u32>(a + 0x3B0, 0x1000AFFC);
        u32 points = a + 0x4C0;
        boomerangReleaseBlurPoint(points);
        boomerangReleaseBlurPoint(points + 0x254);
        store<u32>(points + 0x4B8, 0);
        for (u32 i = 0; i < 5; ++i) boomerangDestroyRenderBuffers(a + 0x9A0 + i * 0x41C);
        call<void>(0x027BE2B0, at<void>(a + 0x1EBC), 2);
        call<void>(0x028F0164, at<void>(a + 0x9A0), 5, 0x41C, 0x020CFD48, 0, 0);
        call<void>(0x027B54A0, at<void>(a + 0x980), 2);
        if (points) {
            boomerangReleaseBlurPoint(points);
            boomerangReleaseBlurPoint(points + 0x254);
            store<u32>(points + 0x4B8, 0);
            call<void>(0x028F0164, at<void>(points), 2, 0x254, 0x020CFDB4, 0, 0);
        }
        call<void>(0x0252CCFC, at<void>(a + 0x3B0), 0);
    }
    call<void>(0x025D50BC, actor, 0);
    if (flags & 1) call<void>(0x0273AF40, actor);
}
VERIFY(0x020D02FC, boomerangDestructor);

static void boomerangDrawBlurRing(u32 a, u32 ring, u32 top, u32 root,
                                  s32 count, f32 step, f32 zero, f32 one) {
    f32 previous = zero, current = step;
    for (s32 i = count; i >= 0; --i) {
        u32 selector = load<u32>(ring + 0x11760);
        u32 buffer = load<u32>(ring + 0x254 * (selector + 2u * static_cast<u32>(i)));
        boomerangZeroCacheRange(buffer, 64);
        selector = load<u32>(ring + 0x11760);
        buffer = load<u32>(ring + 0x254 * (selector + 2u * static_cast<u32>(i)));
        u32 t = top + static_cast<u32>(i) * 12, r = root + static_cast<u32>(i) * 12;
        f32 x = load<f32>(t), y = load<f32>(t + 4), z = load<f32>(t + 8);
        store<f32>(buffer, x); store<f32>(buffer + 4, y);
        store<f32>(buffer + 0xC, current); store<f32>(buffer + 0x10, zero); store<f32>(buffer + 8, z);
        x = load<f32>(t + 12); y = load<f32>(t + 16); z = load<f32>(t + 20);
        store<f32>(buffer + 0x18, y); store<f32>(buffer + 0x14, x); store<f32>(buffer + 0x1C, z);
        store<f32>(buffer + 0x24, zero); store<f32>(buffer + 0x20, previous);
        x = load<f32>(r); y = load<f32>(r + 4); z = load<f32>(r + 8);
        store<f32>(buffer + 0x2C, y); store<f32>(buffer + 0x28, x); store<f32>(buffer + 0x30, z);
        store<f32>(buffer + 0x38, one); store<f32>(buffer + 0x34, current);
        y = load<f32>(r + 16); x = load<f32>(r + 12); z = load<f32>(r + 20);
        store<f32>(buffer + 0x3C, x); store<f32>(buffer + 0x48, previous);
        store<f32>(buffer + 0x40, y); store<f32>(buffer + 0x4C, one); store<f32>(buffer + 0x44, z);
        previous = current; current += step;
    }
    u32 selectedOffset = load<u32>(ring + 0x11760) * 0x254;
    for (u32 i = 0; i < 60; ++i) {
        u32 point = ring + selectedOffset + 4 + i * 0x4A8;
        call<void>(0x027B5E94, at<void>(point), 0, load<u32>(point + 0x14C));
    }
    store<u32>(ring + 0x11760, load<u32>(ring + 0x11760) == 0);
    count = load<s32>(a + 0x98);
    u32 selector = load<u32>(ring + 0x11760);
    for (s32 i = count; i >= 0; --i) {
        u32 pointIndex = (selector == 0) + 2u * static_cast<u32>(i);
        call<void>(0x027BFE5C, at<void>(ring + 0x254 * pointIndex + 0x158));
        u32 primitive = load<u32>(a + 0x23F04);
        if (primitive) {
            u32 vertices = load<u32>(a + 0x23EF8), offset = load<u32>(a + 0x23F00), number = load<u32>(a + 0x23EFC);
            call<void>(0xC0006178, number, primitive, at<void>(vertices), offset, 0, 1);
        }
        if (i != 0) selector = load<u32>(ring + 0x11760);
    }
}

void boomerangDrawBlur(void* blur) {
    WWHD_FUNC(0x020CDF78, void, blur);
    u32 a = ea(blur);
    boomerangBindRenderResource(a, 0xBE8);
    struct Matrix_l { be<f32> values[12]; };
    Local<Matrix_l> matrix;
    boomerangCopyMatrix(matrix.get(), at<void>(0x104B45F8));
    u32 render = a + 0x23ADC, system = load<u32>(0x104B4708);
    call<void>(0x027FDA54, at<void>(render), 0, matrix.get(), at<void>(0x104B470C), at<void>(system + 0x240));
    call<void>(0x027FDFF4, at<void>(render), 0);
    u32 output = load<u32>(render + 4), resource = load<u32>(a + 0xBE8);
    u32 ordinal = load<u32>(output + 0x4C), selection = output + 0x10 + ordinal * 0x1C;
    u32 slots = load<u32>(resource + 0xC) ? load<u32>(resource + 0x10) : 0;
    s16 vertex = load<s16>(slots + 0xC), pixel = load<s16>(slots + 0xE), geometry = load<s16>(slots + 0x10);
    u32 data = load<u32>(selection + 0xC), stride = load<u32>(selection + 4);
    if (pixel != -1) call<void>(0xC0006900, pixel, at<void>(data), stride);
    if (vertex != -1) call<void>(0xC0006A38, vertex, at<void>(data), stride);
    if (geometry != -1) call<void>(0xC00068A8, geometry, at<void>(data), stride);
    f32 divisor = load<f32>(0x1000AF9C);
    f32 red = static_cast<f32>(load<u8>(0x10192468)) / divisor;
    f32 green = static_cast<f32>(load<u8>(0x10192469)) / divisor;
    f32 blue = static_cast<f32>(load<u8>(0x1019246A)) / divisor;
    f32 alpha = static_cast<f32>(load<u8>(0x1019246B)) / divisor;
    store<f32>(render + 0x16C, green); store<f32>(render + 0x170, blue);
    store<f32>(render + 0x168, red); store<f32>(render + 0x174, alpha);
    call<void>(0x027FB678, at<void>(render + 0xB4));
    call<void>(load<u32>(load<u32>(render + 0xC0) + 0x2C), at<void>(render + 0xB4), at<void>(load<u32>(a + 0xBE8)));
    call<void>(0x027FB678, at<void>(render + 0xC));
    call<void>(load<u32>(load<u32>(render + 0x18) + 0x2C), at<void>(render + 0xC), at<void>(load<u32>(a + 0xBE8)));
    resource = load<u32>(a + 0xBE8);
    u32 texture = load<u32>(resource + 0x14) ? load<u32>(resource + 0x18) : 0;
    call<void>(0x027BE53C, at<void>(a + 0x23F18), at<void>(texture + 4), -1, 0);
    struct GraphicsState_l { u8 bytes[0x11C]; };
    Local<GraphicsState_l> state;
    call<void>(0x02750250, state.get());
    u32 s = ea(state.get()), flags = load<u32>(s + 0xEC);
    store<u8>(s, 1); store<u8>(s + 0xE0, 0); store<u8>(s + 1, 0);
    store<u32>(s + 0xC, 3); store<u32>(s + 8, 2);
    store<u32>(s + 0xEC, (((flags & ~15u) + 7u) & 0xFFFFFF0Fu) + 0x10);
    call<void>(0x02750370, state.get());
    s32 count = load<s32>(a + 0x98), denominator = (count >> 1) + 1;
    f32 step = (divisor / static_cast<f32>(denominator)) / divisor;
    f32 zero = load<f32>(0x1000AF14), one = load<f32>(0x1000AE88);
    boomerangDrawBlurRing(a, a + 0xBEC, a + 0xA8, a + 0x378, count, step, zero, one);
    boomerangDrawBlurRing(a, a + 0x12364, a + 0x648, a + 0x918, load<s32>(a + 0x98), step, zero, one);
}
VERIFY(0x020CDF78, boomerangDrawBlur);

static void boomerangSetupBlurRing(u32 ring, u32 material) {
    u32 declaration = ring + 0x11764, previous = ring + 0x11770;
    store<u32>(declaration, 0x11);
    store<u32>(declaration + 8, 0x1000AFF0);
    for (u32 side = 0; side < 2; ++side) {
        for (u32 i = 0; i < 60; ++i) {
            u32 point = ring + 0x254 * side + 0x4A8 * i;
            u32 buffer = load<u32>(point);
            if (!buffer) {
                u32 allocator = load<u32>(0x101F8B4C);
                u32 heap = call<u32>(0x02756140, at<void>(allocator));
                u32 vtable = load<u32>(heap + 0xC);
                u32 allocation = call<u32>(load<u32>(vtable + 0x34), at<void>(heap), 0x50, 0x40);
                if (allocation) {
                    store<u32>(point + 0x250, allocation);
                    store<u32>(point + 0x24C, 4);
                }
                buffer = load<u32>(point + 0x250);
                store<u32>(point, buffer);
            }
            call<void>(0x027FF478, at<void>(point + 4), at<void>(buffer), 4, at<void>(declaration));
            if (material && material != load<u32>(previous))
                call<void>(0x027FF530, at<void>(material), at<void>(point + 0x158), at<void>(point + 4), at<void>(declaration), 0);
        }
    }
    store<u32>(previous, material);
    store<u8>(ring + 0x11774, 1);
}

static void boomerangInitializeBlurRing(u32 ring, f32 zero, f32 one) {
    u32 selector = ring + 0x11760;
    const u16 order[] = {0x14,0x38,0x40,0x24,0x48,0x18,0x28,4,0x44,0x34,0x4C,0x30,0x1C,0,0xC,0x10,0x3C,0x2C,8,0x20};
    for (u32 i = 0; i < 60; ++i) {
        u32 index = load<u32>(selector) + 2 * i;
        u32 buffer = load<u32>(ring + index * 0x254);
        boomerangZeroCacheRange(buffer, 0x40);
        index = load<u32>(selector) + 2 * i;
        buffer = load<u32>(ring + index * 0x254);
        for (u16 offset : order) store<f32>(buffer + offset, offset == 0x38 || offset == 0x48 || offset == 0x4C || offset == 0x20 ? one : zero);
    }
    u32 index = load<u32>(selector), opposite = index == 0;
    for (u32 i = 0; i < 60; ++i) {
        u32 sourcePoint = ring + (index + 2 * i) * 0x254, destinationPoint = ring + opposite * 0x254 + i * 0x4A8;
        for (u32 group = 0; group < 4; ++group) {
            u32 source = load<u32>(sourcePoint), destination = load<u32>(destinationPoint);
            for (u32 j = 0; j < 5; ++j) {
                u32 offset = group * 20 + j * 4;
                store<f32>(destination + offset, load<f32>(source + offset));
            }
        }
        index = load<u32>(selector);
    }
    index *= 0x254;
    for (u32 i = 0; i < 60; ++i) {
        u32 point = ring + index + i * 0x4A8;
        u32 size = load<u32>(point + 0x150);
        call<void>(0x027B5E94, at<void>(point + 4), 0, at<void>(size));
    }
    store<u32>(selector, load<u32>(selector) == 0);
}

void boomerangSetupBlur(void* blur) {
    WWHD_FUNC(0x020CCA50, void, blur);
    u32 a = ea(blur), resource = boomerangFindRenderResource(0x1000AF70);
    store<u32>(a + 0xBE8, resource);
    boomerangSetupBlurRing(a + 0xBEC, load<u32>(a + 0xBE8));
    boomerangSetupBlurRing(a + 0x12364, load<u32>(a + 0xBE8));
    call<void>(0x027FE084, at<void>(a + 0x23ADC), 1, 0);
    for (u32 i = 0; i < 4; ++i) store<u16>(a + 0x23F10 + 2 * i, static_cast<u16>(i));
    call<void>(0x027B54E0, at<void>(a + 0x23EF8), at<void>(a + 0x23F10), 4, 4);
    f32 zero = load<f32>(0x1000AF14), one = load<f32>(0x1000AE88);
    store<u32>(a + 0x23EFC, 6);
    boomerangInitializeBlurRing(a + 0xBEC, zero, one);
    boomerangInitializeBlurRing(a + 0x12364, zero, one);
    u32 graphics = load<u32>(0x101F8B18);
    call<void>(0x0274FBF8, at<void>(graphics));
    Local<BoomerangString_l> name;
    name->vtable = 0x1000AD2C;
    u32 manager = load<u32>(0x101F4F28);
    name->text = 0x1000AF54;
    u32 texture = call<u32>(0x026066C4, at<void>(manager), name.get(), 0x70);
    if (!texture) call<void>(0x0273AA24, at<void>(0x1000AF5C), 0xFA, at<void>(0x1000AF80));
    u32 data = load<u32>(texture + 0x20);
    call<void>(0x02773798, at<void>(a + 0x240B0), at<void>(data));
    graphics = load<u32>(0x101F8B18);
    call<void>(0x0274FCCC, at<void>(graphics));
    u32 original = a + 0x240B0, copy = a + 0x23F18;
    bool equal = true;
    for (u32 offset : {4u,8u,12u,16u,20u,24u,56u,52u,28u}) {
        if (load<u32>(copy + offset) != load<u32>(original + offset)) { equal = false; break; }
    }
    if (!equal) call<void>(0x027BDEB4, at<void>(copy), at<void>(original));
    else {
        data = load<u32>(original + 0x28);
        u32 size = load<u32>(original + 0x30);
        store<u32>(copy + 0x28, data);
        store<u32>(copy + 0xDC, size);
        store<u32>(copy + 0x30, size);
        store<u32>(copy + 0xD4, data);
    }
    u8 flags = load<u8>(copy + 0x190);
    store<u32>(copy + 0x160, 2); store<u32>(copy + 0x15C, 2); store<u32>(copy + 0x164, 2);
    store<u8>(copy + 0x190, flags | 2);
}
VERIFY(0x020CCA50, boomerangSetupBlur);
