#include "d/actor/d_a_bo.h"
using daBo::Actor;

// Keep outgoing linkage and stack arguments below all live guest payloads.
template <class R = void, class... A> static R boCall(u32 target, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return gabi::call<R>(target, args...);
}
template <class R = void, class... A> static R boVirtualCall(u32 target, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...)                                                                    \
    if (gabi::Activation::nested())                                                                \
        return boCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                         \
    gabi::Activation wwhd_activation_

template <class T> static T *boMember(void *object, u32 offset) {
    return gabi::at<T>(gabi::ea(object) + offset);
}
static u32 boPlay() {
    return gabi::ea(boCall<void *>(0x025200D4));
}
struct BoSafeString {
    be<u32> text, vtable;
};
static void *boResource(s32 index, u32 nameAddress = 0x1000A1D8) {
    gabi::Local<BoSafeString> name;
    name->vtable = 0x1000A09C;
    name->text = nameAddress;
    u32 controller = gabi::load<u32>(0x101F4F28);
    return boCall<void *>(0x026066C4, gabi::at<void>(controller), name.get(), index);
}
static void smoke_set(Actor *actor) {
    WWHD_FUNC(0x020BB0E8, void, actor);
    boCall(0x025A5F88, boMember<void>(actor, 0x4cc));
    s32 room = (s8)actor->current.roomNo;
    u32 particle = gabi::load<u32>(boPlay() + 0x5ab0);
    boCall(0x025A847C, gabi::at<void>(particle), 2, 0x2027, boMember<cXyz>(actor, 0x4b8),
           &actor->shape_angle, 0, 0xb9, boMember<void>(actor, 0x4cc), room, 0, 0, 0);
    u32 emitter = *boMember<be<u32>>(actor, 0x4d0);
    if (emitter) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5c, 1);
        gabi::store<f32>(emitter + 0x6c, 10.f);
        gabi::store<f32>(emitter + 0x238, 3.f);
        gabi::store<f32>(emitter + 0x68, 10.f);
        gabi::store<u32>(emitter + 0x254, flags | 0x40);
        gabi::store<f32>(emitter + 0x23c, 3.f);
        gabi::store<f32>(emitter + 0x240, 3.f);
        gabi::store<f32>(emitter + 0x34, 30.f);
        *boMember<be<u32>>(actor, 0x4e2) = 0xA0A080FF;
    }
}
VERIFY(0x020BB0E8, smoke_set);
static BOOL nodeCallBack_DW(J3DNode *node, s32 stage) {
    WWHD_FUNC(0x020BB838, BOOL, node, stage);
    if (stage == 0) {
        J3DJoint *joint = boCall<J3DJoint *>(0x027F7878, node);
        u32 model = gabi::load<u32>(0x104B462C);
        u32 actor = gabi::load<u32>(model + 0xb8);
        u16 index = gabi::load<u16>(gabi::ea(joint) + 4);
        if (actor && index == 9) {
            u32 data = gabi::load<u32>(model + 0x2c);
            u16 flags = gabi::load<u16>(data + 4);
            u32 joints = gabi::load<u32>(data + 0x10);
            gabi::store<u16>(data + 4, flags | 0x10);
            u32 matrix = gabi::load<u32>(0x1018C7B0);
            boCall(0x028E90D4, gabi::at<void>(joints + 0x1b0), gabi::at<void>(matrix));
            gabi::Local<cXyz> origin;
            origin->y = 0;
            origin->z = 0;
            origin->x = 0;
            boCall(0x0200FCD8, origin.get(), gabi::at<void>(actor + 0x420));
        }
    }
    return 1;
}
VERIFY(0x020BB838, nodeCallBack_DW);
static void anm_init(Actor *actor, s32 animation, f32 blend, u8 loop, f32 speed, s32 secondary,
                     s32 lower) {
    WWHD_FUNC(0x020BBC58, void, actor, animation, blend, loop, speed, secondary, lower);
    *boMember<be<s32>>(actor, 0x3f8) = animation;
    void *first = boResource(animation);
    void *second = nullptr;
    if (secondary >= 0)
        second = boResource(secondary);
    mDoExt_McaMorf_c *morph = lower ? actor->lowerMorph.get() : actor->upperMorph.get();
    boCall(0x025E4A98, morph, first, (u32)loop, blend, speed, 0.f, -1.f, second);
}
VERIFY(0x020BBC58, anm_init);
static void angle_initial(Actor *actor) {
    WWHD_FUNC(0x020BCDA8, void, actor);
    *boMember<be<s16>>(actor, 0x45c) = 0;
    *boMember<be<s16>>(actor, 0x456) = 0;
    *boMember<be<s16>>(actor, 0x450) = 0;
    *boMember<be<s16>>(actor, 0x46e) = 0;
    *boMember<be<s16>>(actor, 0x454) = 0;
    *boMember<be<s16>>(actor, 0x470) = 0;
    *boMember<be<s16>>(actor, 0x460) = 0;
    *boMember<be<s16>>(actor, 0x458) = 0;
    *boMember<be<s16>>(actor, 0x45a) = 0;
    *boMember<be<s16>>(actor, 0x45e) = 0;
    *boMember<be<s16>>(actor, 0x472) = 0;
    *boMember<be<s16>>(actor, 0x452) = 0;
}
VERIFY(0x020BCDA8, angle_initial);
static void wait_initial(Actor *actor) {
    WWHD_FUNC(0x020BCDE0, void, actor);
    s32 animation = 22;
    if (boCall<f32>(0x02019788) < 0.5f)
        animation = 21;
    anm_init(actor, animation, 5.f, 2, 1.f, -1, 0);
    f32 random = boCall<f32>(0x020198D8, 60.f);
    u32 attackFlags = *boMember<be<u32>>(actor, 0x790);
    s16 delay = (s16)gabi::ftoi(random + 60.f);
    u32 targetFlags = *boMember<be<u32>>(actor, 0x7bc);
    *boMember<be<u8>>(actor, 0x7ff) = 0;
    *boMember<be<u32>>(actor, 0x790) = attackFlags & ~1u;
    *boMember<be<u32>>(actor, 0x7bc) = targetFlags | 1;
    *boMember<be<s16>>(actor, 0x482) = delay;
    angle_initial(actor);
    actor->action = 0;
    actor->mode = 2;
}
VERIFY(0x020BCDE0, wait_initial);
static BOOL daBO_IsDelete(Actor *actor) {
    WWHD_FUNC(0x020BF56C, BOOL, actor);
    return 1;
}
VERIFY(0x020BF56C, daBO_IsDelete);
static BOOL daBO_Delete(Actor *actor) {
    WWHD_FUNC(0x020BF574, BOOL, actor);
    u32 vtable = *boMember<be<u32>>(actor, 0x4cc);
    boVirtualCall(gabi::load<u32>(vtable + 0x44), boMember<void>(actor, 0x4cc));
    u32 emitter = *boMember<be<u32>>(actor, 0x4ec);
    if (emitter) {
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5c, 0xffffffff);
        gabi::store<u32>(emitter + 0x254, flags | 1);
        *boMember<be<u32>>(actor, 0x4ec) = 0;
    }
    emitter = *boMember<be<u32>>(actor, 0x4f0);
    if (emitter) {
        boCall(0x0281DE68, gabi::at<void>(emitter), 1);
        emitter = *boMember<be<u32>>(actor, 0x4f0);
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5c, 0xffffffff);
        gabi::store<u32>(emitter + 0x254, flags | 1);
        *boMember<be<u32>>(actor, 0x4f0) = 0;
    }
    emitter = *boMember<be<u32>>(actor, 0x4f4);
    if (emitter) {
        boCall(0x0281DE68, gabi::at<void>(emitter), 1);
        emitter = *boMember<be<u32>>(actor, 0x4f4);
        u32 flags = gabi::load<u32>(emitter + 0x254);
        gabi::store<u32>(emitter + 0x5c, 0xffffffff);
        gabi::store<u32>(emitter + 0x254, flags | 1);
        *boMember<be<u32>>(actor, 0x4f4) = 0;
    }
    boCall(0x02041C30, boMember<void>(actor, 0xed0));
    boCall(0x025204C8, boMember<void>(actor, 0x3c8), gabi::at<char>(0x1000A25C));
    return 1;
}
VERIFY(0x020BF574, daBO_Delete);
static void yodare_execute(void *callback, void *emitter, void *particle) {
    WWHD_FUNC(0x020C00C0, void, callback, emitter, particle);
    f32 x = *boMember<be<f32>>(particle, 0x28), z = *boMember<be<f32>>(particle, 0x30),
        y = *boMember<be<f32>>(particle, 0x2c);
    gabi::Local<cXyz> position;
    position->z = z;
    position->y = y + 20.f;
    position->x = x;
    *boMember<be<f32>>(callback, 0x28) = x;
    *boMember<be<f32>>(callback, 0x30) = z;
    *boMember<be<f32>>(callback, 0x2c) = y + 20.f;
    u32 world = boPlay();
    f32 ground =
        boCall<f32>(0x02008974, gabi::at<void>(world + 0x12a0), boMember<void>(callback, 4));
    position->y = ground;
    if (ground > y) {
        u32 particleControl = gabi::load<u32>(boPlay() + 0x5ab0);
        boCall(0x025A847C, gabi::at<void>(particleControl), 0, 0x8108, position.get(), 0, 0, 0xff,
               0, -1, 0, 0, 0);
    }
}
VERIFY(0x020C00C0, yodare_execute);
static u32 particle_list_count(void *list) {
    WWHD_FUNC(0x020C0A94, u32, list);
    return *boMember<be<u32>>(list, 8);
}
VERIFY(0x020C0A94, particle_list_count);
static void callback_empty0(void *p) {
    WWHD_FUNC(0x020C0A9C, void, p);
}
VERIFY(0x020C0A9C, callback_empty0);
static void callback_empty1(void *p) {
    WWHD_FUNC(0x020C0AA0, void, p);
}
VERIFY(0x020C0AA0, callback_empty1);
static void callback_empty2(void *p) {
    WWHD_FUNC(0x020C0AA4, void, p);
}
VERIFY(0x020C0AA4, callback_empty2);
static void empty_virtual(void *p) {
    WWHD_FUNC(0x020C0BF8, void, p);
}
VERIFY(0x020C0BF8, empty_virtual);
static void emitter_set_flags(void *emitter, u32 flags) {
    WWHD_FUNC(0x020C0220, void, emitter, flags);
    *boMember<be<u32>>(emitter, 0x254) = *boMember<be<u32>>(emitter, 0x254) | flags;
}
VERIFY(0x020C0220, emitter_set_flags);
static void trivial_destructor(void *object, u32 flags) {
    WWHD_FUNC(0x020C0230, void, object, flags);
    if (object && (flags & 1))
        boCall(0x0273AF40, object);
}
VERIFY(0x020C0230, trivial_destructor);
static void *identity_pointer(void *p) {
    WWHD_FUNC(0x020C0244, void *, p);
    return p;
}
VERIFY(0x020C0244, identity_pointer);

static void bo_static_init() {
    WWHD_FUNC(0x020C018C, void);
    gabi::store<u32>(0x10462550, 0);
    gabi::store<u32>(0x10462548, 0);
    gabi::store<u32>(0x10462554, 0);
    gabi::store<u32>(0x1046254c, 0);
    boCall(0x028F026C, gabi::at<void>(0x10191EAC));
    gabi::store<f32>(0x10462534, -3.1415927410125732f);
    gabi::store<f32>(0x10462538, 3.1415927410125732f);
    boCall(0x028ED6F8, gabi::at<void>(0x10462544));
    boCall(0x028F026C, gabi::at<void>(0x10191EB8));
    boCall(0x028EAB2C, gabi::at<void>(0x10462545));
    boCall(0x028F026C, gabi::at<void>(0x10191EC4));
}
VERIFY(0x020C018C, bo_static_init);
static void *enemy_fire_constructor(void *object) {
    WWHD_FUNC(0x020BF9C0, void *, object);
    if (!object)
        object = boCall<void *>(0x0273AD10, 0x22c);
    if (object) {
        if (!gabi::ea(boMember<void>(object, 0x8c)))
            boCall<void *>(0x0273AD10, 12);
        boCall(0x0200BD2C, boMember<void>(object, 0xa0));
        boCall(0x02515DA0, boMember<void>(object, 0xbc));
        *boMember<be<u32>>(object, 0xb8) = 0x1004AE88;
        *boMember<be<u32>>(object, 0xbc) = 0x1004AEC0;
        boCall(0x025166F0, boMember<void>(object, 0xdc));
        *boMember<be<f32>>(object, 0x228) = 1.f;
    }
    return object;
}
VERIFY(0x020BF9C0, enemy_fire_constructor);
static Actor *actor_constructor(Actor *actor) {
    WWHD_FUNC(0x020BFA4C, Actor *, actor);
    if (!actor)
        actor = boCall<Actor *>(0x0273AD10, 0x1104);
    if (!actor)
        return actor;
    boCall(0x025D4ED0, actor);
    *boMember<be<u32>>(actor, 0xb4) = 0x1000A184;
    boCall(0x025A5B18, boMember<void>(actor, 0x4cc), 1);
    *boMember<be<u32>>(actor, 0x4f8) = 0x1000A290;
    boCall(0x02008E0C, boMember<void>(actor, 0x4fc));
    *boMember<be<u8>>(actor, 0x544) = 0;
    *boMember<be<u8>>(actor, 0x543) = 0;
    *boMember<be<u32>>(actor, 0x4fc) = gabi::ea(actor) + 0x53c;
    *boMember<be<u8>>(actor, 0x546) = 0;
    *boMember<be<u32>>(actor, 0x51c) = 0x1000A124;
    *boMember<be<u8>>(actor, 0x541) = 0;
    *boMember<be<u32>>(actor, 0x50c) = 0x1000A114;
    *boMember<be<u8>>(actor, 0x540) = 1;
    *boMember<be<u32>>(actor, 0x53c) = 0x1000A144;
    *boMember<be<u32>>(actor, 0x548) = 0x1000A134;
    *boMember<be<u8>>(actor, 0x545) = 0;
    *boMember<be<u8>>(actor, 0x542) = 0;
    *boMember<be<u32>>(actor, 0x54c) = 1;
    *boMember<be<u32>>(actor, 0x500) = gabi::ea(actor) + 0x548;
    boCall(0x024EFE94, boMember<void>(actor, 0x550));
    boCall(0x024F0474, boMember<void>(actor, 0x590));
    *boMember<be<u32>>(actor, 0x5b0) = 0x1000A164;
    *boMember<be<u32>>(actor, 0x5a4) = 0x1000A174;
    *boMember<be<u8>>(actor, 0x5a8) = 1;
    *boMember<be<u32>>(actor, 0x5a0) = 0x1000A154;
    boCall(0x0200BD2C, boMember<void>(actor, 0x754));
    boCall(0x02515DA0, boMember<void>(actor, 0x770));
    *boMember<be<u32>>(actor, 0x76c) = 0x1004AE88;
    *boMember<be<u32>>(actor, 0x770) = 0x1004AEC0;
    boCall(0x025166F0, boMember<void>(actor, 0x790));
    boCall(0x025166F0, boMember<void>(actor, 0x8bc));
    boCall(0x02515FB8, boMember<void>(actor, 0x9e8));
    *boMember<be<u32>>(actor, 0xafc) = 0x100015A8;
    *boMember<be<u32>>(actor, 0xaf8) = 0x1000A0B4;
    boCall(0x02018590, boMember<void>(actor, 0xb00));
    *boMember<be<u32>>(actor, 0xa24) = 0x1004B108;
    *boMember<be<u32>>(actor, 0xafc) = 0x1004B160;
    *boMember<be<u32>>(actor, 0xb14) = 0x1004B150;
    boCall(0x0200BD2C, boMember<void>(actor, 0xb48));
    boCall(0x02515DA0, boMember<void>(actor, 0xb64));
    *boMember<be<u32>>(actor, 0xb60) = 0x1004AE88;
    *boMember<be<u32>>(actor, 0xb64) = 0x1004AEC0;
    boCall(0x02515FB8, boMember<void>(actor, 0xb84));
    *boMember<be<u32>>(actor, 0xc98) = 0x100015A8;
    *boMember<be<u32>>(actor, 0xc94) = 0x1000A0B4;
    boCall(0x02018590, boMember<void>(actor, 0xc9c));
    *boMember<be<u32>>(actor, 0xbc0) = 0x1004B108;
    *boMember<be<u32>>(actor, 0xc98) = 0x1004B160;
    *boMember<be<u32>>(actor, 0xcb0) = 0x1004B150;
    boCall(0x024EFE94, boMember<void>(actor, 0xccc));
    boCall(0x024F0474, boMember<void>(actor, 0xd0c));
    *boMember<be<u32>>(actor, 0xd2c) = 0x1000A164;
    *boMember<be<u32>>(actor, 0xd1c) = 0x1000A154;
    *boMember<be<u8>>(actor, 0xd24) = 1;
    *boMember<be<u32>>(actor, 0xd20) = 0x1000A174;
    enemy_fire_constructor(boMember<void>(actor, 0xed0));
    boCall(0x025E895C, boMember<void>(actor, 0x10fc));
    return actor;
}
VERIFY(0x020BFA4C, actor_constructor);
static void actor_destructor(Actor *actor, u32 flags) {
    WWHD_FUNC(0x020C0AA8, void, actor, flags);
    if (!actor)
        return;
    boCall(0x025E89F8, boMember<void>(actor, 0x10fc), 2);
    boCall(0x02515AE8, boMember<void>(actor, 0xfac), 2);
    boCall(0x02515860, boMember<void>(actor, 0xf70), 2);
    *boMember<be<u32>>(actor, 0xd2c) = 0x1000A164;
    *boMember<be<u32>>(actor, 0xd20) = 0x1000A174;
    boCall(0x024EFD9C, boMember<void>(actor, 0xd0c), 0);
    boCall(0x02018034, boMember<void>(actor, 0xce0), 2);
    boCall(0x02515A70, boMember<void>(actor, 0xb84), 2);
    boCall(0x02515860, boMember<void>(actor, 0xb48), 2);
    boCall(0x02515A70, boMember<void>(actor, 0x9e8), 2);
    boCall(0x02515AE8, boMember<void>(actor, 0x8bc), 2);
    boCall(0x02515AE8, boMember<void>(actor, 0x790), 2);
    boCall(0x02515860, boMember<void>(actor, 0x754), 2);
    *boMember<be<u32>>(actor, 0x5b0) = 0x1000A164;
    *boMember<be<u32>>(actor, 0x5a4) = 0x1000A174;
    boCall(0x024EFD9C, boMember<void>(actor, 0x590), 0);
    boCall(0x02018034, boMember<void>(actor, 0x564), 2);
    *boMember<be<u32>>(actor, 0x51c) = 0x1000A0E4;
    *boMember<be<u32>>(actor, 0x53c) = 0x1000A104;
    *boMember<be<u32>>(actor, 0x548) = 0x1000A0C4;
    boCall(0x02008DAC, boMember<void>(actor, 0x4fc), 0);
    boCall(0x025D50BC, actor, 0);
    if (flags & 1)
        boCall(0x0273AF40, actor);
}
VERIFY(0x020C0AA8, actor_destructor);

static void copy_base_matrix(u32 model) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(0x1048D0CC + i * 4);
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(model + 0xc8 + i * 4, values[i]);
}
static void draw_SUB(Actor *actor) {
    WWHD_FUNC(0x020BB8D0, void, actor);
    u8 kind = *boMember<be<u8>>(actor, 0x3dc);
    void *matrix = gabi::at<void>(0x1048D0CC);
    if (kind == 2) {
        boMember<cXyz>(actor, 0x420)->copy(actor->current.pos);
        boCall(0x025E742C, boMember<gptr<void>>(actor, 0x3d0)->get());
    }
    if (kind == 0 || kind == 2) {
        f32 y = *boMember<be<f32>>(actor, 0x424), height = *boMember<be<f32>>(actor, 0x4b0);
        f32 x = *boMember<be<f32>>(actor, 0x420), z = *boMember<be<f32>>(actor, 0x428);
        boCall(0x028E93CC, matrix, x, y + height, z);
        boCall(0x025F1C28, matrix, (s32)*boMember<be<s16>>(actor, 0x464));
        boCall(0x025F1BF4, matrix, (s32)actor->shape_angle.x);
        boCall(0x025F1C5C, matrix, (s32)actor->shape_angle.z);
        height = *boMember<be<f32>>(actor, 0x4b0);
        boCall(0x025F24E0, 0.f, -height, 0.f);
        u32 morph = gabi::ea(actor->upperMorph.get());
        copy_base_matrix(gabi::load<u32>(morph + 0x90));
        boCall(0x025E55A0, actor->upperMorph.get());
        kind = *boMember<be<u8>>(actor, 0x3dc);
        if (kind == 0) {
            boCall(0x02041570, boMember<void>(actor, 0xed0));
            kind = *boMember<be<u8>>(actor, 0x3dc);
        }
    }
    if (kind == 0 || kind == 1) {
        f32 z = actor->current.pos.z, x = actor->current.pos.x, y = actor->current.pos.y;
        boCall(0x028E93CC, matrix, x, y, z);
        boCall(0x025F1C28, matrix, (s32)actor->current.angle.y);
        boCall(0x025F1BF4, matrix, (s32)actor->shape_angle.x);
        boCall(0x025F1C5C, matrix, (s32)actor->shape_angle.z);
        u32 morph = gabi::ea(actor->lowerMorph.get());
        copy_base_matrix(gabi::load<u32>(morph + 0x90));
        boCall(0x025E55A0, actor->lowerMorph.get());
    }
    void *environment = boCall<void *>(0x02555D0C);
    boCall(0x025626A4, environment, 0, &actor->current.pos, boMember<void>(actor, 0x110));
}
VERIFY(0x020BB8D0, draw_SUB);
static BOOL daBO_Draw(Actor *actor) {
    WWHD_FUNC(0x020BBAEC, BOOL, actor);
    if (gabi::load<s16>(0x1047BDE2) != 0)
        return 1;
    u8 kind = *boMember<be<u8>>(actor, 0x3dc);
    if (kind == 0 || kind == 2) {
        u32 model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
        void *environment = boCall<void *>(0x02555D0C);
        boCall(0x02562F5C, environment, gabi::at<void>(model), boMember<void>(actor, 0x110));
        kind = *boMember<be<u8>>(actor, 0x3dc);
    }
    if ((kind == 0 || kind == 1) && *boMember<be<s8>>(actor, 0xb1e) == 0) {
        u32 model = gabi::load<u32>(gabi::ea(actor->lowerMorph.get()) + 0x90);
        void *environment = boCall<void *>(0x02555D0C);
        boCall(0x02562F5C, environment, gabi::at<void>(model), boMember<void>(actor, 0x110));
    }
    s32 yaw = *boMember<be<s16>>(actor, 0x464);
    boCall(0x025BEBB8, 0xb2, actor, &actor->current.pos, yaw, 1.f, 1.f, 1.f);
    kind = *boMember<be<u8>>(actor, 0x3dc);
    if (kind == 0 && *boMember<be<s16>>(actor, 0xb26) > 20) {
        boCall(0x0259138C, actor->upperMorph.get(), -1, boMember<void>(actor, 0x10fc));
        kind = *boMember<be<u8>>(actor, 0x3dc);
    } else if (kind == 0 || kind == 2) {
        mDoExt_McaMorf_c *upper = actor->upperMorph.get();
        u32 model = gabi::load<u32>(gabi::ea(upper) + 0x90);
        if (kind == 2) {
            u32 colorAnm = *boMember<be<u32>>(actor, 0x3d0);
            u32 data = gabi::load<u32>(model + 0xac);
            f32 frame = gabi::load<f32>(colorAnm + 4);
            boCall(0x025E83FC, gabi::at<void>(colorAnm), gabi::at<void>(data), frame);
            upper = actor->upperMorph.get();
        }
        boCall(0x025E5590, upper);
        kind = *boMember<be<u8>>(actor, 0x3dc);
        if (kind == 2) {
            u32 data = gabi::load<u32>(model + 0xac);
            gabi::store<u32>(data + 0x48, 0);
            kind = *boMember<be<u8>>(actor, 0x3dc);
        }
    }
    if ((kind == 0 || kind == 1) && *boMember<be<s8>>(actor, 0xb1e) == 0)
        boCall(0x025E5590, actor->lowerMorph.get());
    return 1;
}
VERIFY(0x020BBAEC, daBO_Draw);
static void monster_sound(Actor *actor, u32 sound) {
    void *position = boMember<void>(actor, 0x37c);
    if (!gabi::ea(position))
        return;
    u32 id = actor ? *boMember<be<u32>>(actor, 4) : 0xffffffff;
    s32 room = (s8)actor->current.roomNo;
    s8 reverb = boCall<s8>(0x02520540, room);
    boCall(0x025E1AA4, sound, position, id, 0, (s32)reverb);
}
static void effect_sound(Actor *actor, u32 sound, u32 flags) {
    void *position = boMember<void>(actor, 0x37c);
    if (!gabi::ea(position))
        return;
    s32 room = (s8)actor->current.roomNo;
    s8 reverb = boCall<s8>(0x02520540, room);
    boCall(0x025E1A40, sound, position, flags, (s32)reverb);
}
static void nokezori_damage_rtn(Actor *actor) {
    WWHD_FUNC(0x020BBE4C, void, actor);
    s16 yaw = *boMember<be<s16>>(actor, 0x464), hitYaw = *boMember<be<s16>>(actor, 0x48c);
    s16 delta = (s16)(hitYaw - yaw);
    anm_init(actor, 7, 5.f, 0, 1.f, -1, 0);
    boCall(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), (s32)delta);
    gabi::Local<cXyz> direction, result;
    direction->y = 0;
    direction->z = 6250.f;
    direction->x = 0;
    boCall(0x0200FCD8, direction.get(), result.get());
    f32 x = result->x, z = result->z;
    *boMember<be<s16>>(actor, 0x460) = (s16)gabi::ftoi(x);
    *boMember<be<s16>>(actor, 0x45e) = (s16)gabi::ftoi(z);
    *boMember<be<s16>>(actor, 0x450) = 0;
    *boMember<be<s16>>(actor, 0x452) = 0;
    *boMember<be<s16>>(actor, 0x454) = 0;
    *boMember<be<f32>>(actor, 0x334) = 1.75f;
    *boMember<be<s16>>(actor, 0x498) = 1200;
    void *hit = boCall<void *>(0x02516300, boMember<void>(actor, 0x790));
    if (hit) {
        u32 type = *boMember<be<u32>>(hit, 0x10);
        if (type & 2) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2806, 0x20);
        } else if (type & 0x80) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2835, 0x20);
        } else if (type & 0x10000) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2855, 0x31);
        } else if (!(type & 0x40)) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2836, 0x20);
        }
    }
    u8 detached = *boMember<be<u8>>(actor, 0x3ec);
    *boMember<be<s16>>(actor, 0x48e) = 0;
    *boMember<be<s16>>(actor, 0x492) = 0;
    *boMember<be<f32>>(actor, 0x4b4) = 1.f;
    *boMember<be<s16>>(actor, 0x486) = 3;
    if (detached == 0) {
        actor->action = 0;
        actor->mode = 5;
    } else {
        actor->action = 4;
        actor->mode = 50;
    }
}
VERIFY(0x020BBE4C, nokezori_damage_rtn);

static BOOL useHeapInit(Actor *actor) {
    WWHD_FUNC(0x020BF65C, BOOL, actor);
    s32 animation = *boMember<be<u8>>(actor, 0x3dc) == 2 ? 7 : 9;
    void *modelResource = boResource(26, 0x1000A25F);
    void *animationResource = boResource(animation, 0x1000A25F);
    mDoExt_McaMorf_c *upper =
        boCall<mDoExt_McaMorf_c *>(0x025E4F64, 0, modelResource, 0, 0, animationResource, 1, 0, -1,
                                   0.f, 1, 0, 0x80000, 0x37441422);
    actor->upperMorph = upper;
    if (!upper || !gabi::load<u32>(gabi::ea(upper) + 0x90))
        return 0;
    u32 model = gabi::load<u32>(gabi::ea(upper) + 0x90);
    gabi::store<u32>(model + 0xb8, gabi::ea(actor));
    model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
    void *joints = boCall<void *>(0x027F3F94, gabi::at<void>(gabi::load<u32>(model + 0xac)));
    u16 index = 0;
    while (index < *boMember<be<u16>>(joints, 8)) {
        model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
        u32 data = gabi::load<u32>(model + 0xac), count = gabi::load<u32>(data + 4),
            table = gabi::load<u32>(data + 8);
        if (index < count)
            table += index * 0x1c;
        gabi::store<u32>(table + 8, 0x020BB1E4);
        index = (u16)(index + 1);
        model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
        joints = boCall<void *>(0x027F3F94, gabi::at<void>(gabi::load<u32>(model + 0xac)));
    }
    if (*boMember<be<u8>>(actor, 0x3dc) == 2) {
        model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
        void *brk = boCall<void *>(0x0273AD10, 0x78);
        if (brk)
            brk = boCall<void *>(0x025E80D0, brk);
        *boMember<gptr<void>>(actor, 0x3d0) = brk;
        if (!brk)
            return 0;
        void *resource = boResource(29, 0x1000A25F);
        u32 data = gabi::load<u32>(model + 0xac);
        brk = boMember<gptr<void>>(actor, 0x3d0)->get();
        if (!boCall<s32>(0x025E8154, brk, gabi::at<void>(data), resource, 1, 0, 0, -1, 0, 1.f, 0))
            return 0;
    } else {
        modelResource = boResource(25, 0x1000A25F);
        animationResource = boResource(8, 0x1000A25F);
        mDoExt_McaMorf_c *lower =
            boCall<mDoExt_McaMorf_c *>(0x025E4F64, 0, modelResource, 0, 0, animationResource, 1, 0,
                                       -1, 0.f, 1, 0, 0x80000, 0x33221202);
        actor->lowerMorph = lower;
        if (!lower || !gabi::load<u32>(gabi::ea(lower) + 0x90))
            return 0;
        model = gabi::load<u32>(gabi::ea(lower) + 0x90);
        gabi::store<u32>(model + 0xb8, gabi::ea(actor));
        model = gabi::load<u32>(gabi::ea(actor->lowerMorph.get()) + 0x90);
        joints = boCall<void *>(0x027F3F94, gabi::at<void>(gabi::load<u32>(model + 0xac)));
        index = 0;
        while (index < *boMember<be<u16>>(joints, 8)) {
            model = gabi::load<u32>(gabi::ea(actor->lowerMorph.get()) + 0x90);
            u32 data = gabi::load<u32>(model + 0xac), count = gabi::load<u32>(data + 4),
                table = gabi::load<u32>(data + 8);
            if (index < count)
                table += index * 0x1c;
            index = (u16)(index + 1);
            gabi::store<u32>(table + 8, 0x020BB838);
            model = gabi::load<u32>(gabi::ea(actor->lowerMorph.get()) + 0x90);
            joints = boCall<void *>(0x027F3F94, gabi::at<void>(gabi::load<u32>(model + 0xac)));
        }
    }
    if (*boMember<be<u8>>(actor, 0x3dc) == 0) {
        model = gabi::load<u32>(gabi::ea(actor->upperMorph.get()) + 0x90);
        if (!boCall<s32>(0x025E8A48, boMember<void>(actor, 0x10fc), gabi::at<void>(model)))
            return 0;
    }
    return 1;
}
VERIFY(0x020BF65C, useHeapInit);

static s32 daBO_Create(Actor *actor) {
    WWHD_FUNC(0x020BFC50, s32, actor);
    u32 status = *boMember<be<u32>>(actor, 0x2e4);
    if (!(status & 8)) {
        if (actor) {
            actor_constructor(actor);
            status = *boMember<be<u32>>(actor, 0x2e4);
        }
        *boMember<be<u32>>(actor, 0x2e4) = status | 8;
    }
    s32 phase = boCall<s32>(0x02520460, boMember<void>(actor, 0x3c8), gabi::at<char>(0x1000A27C));
    if (phase != 4)
        return phase;
    u32 parameters = *boMember<be<u32>>(actor, 0xb0);
    u8 kind = (u8)parameters, variant = (u8)(parameters >> 8);
    *boMember<be<u8>>(actor, 0x3dc) = kind;
    *boMember<be<u8>>(actor, 0x3dd) = variant;
    if (kind == 0xff) {
        variant = *boMember<be<u8>>(actor, 0x3dd);
        *boMember<be<u8>>(actor, 0x3dc) = 0;
    }
    if (variant == 0xff)
        *boMember<be<u8>>(actor, 0x3dd) = 0;
    u32 play = boPlay();
    s32 figure =
        boCall<s32>(0x0200E814, gabi::at<void>(play + 0x50ac), gabi::at<char>(0x1000A274), 0);
    *boMember<be<s32>>(actor, 0x3a4) = figure;
    if (!boCall<s32>(0x025D63E8, actor, 0x020BF65C, 0x3100))
        return 5;
    kind = *boMember<be<u8>>(actor, 0x3dc);
    u32 morph = kind == 0 || kind == 2 ? gabi::ea(actor->upperMorph.get())
                                       : gabi::ea(actor->lowerMorph.get());
    u32 model = gabi::load<u32>(morph + 0x90);
    *boMember<be<u32>>(actor, 0x348) = model ? model + 0xc8 : 0;
    boCall(0x025D674C, actor, -150.f, 0.f, -150.f, 150.f, 330.f, 150.f);
    *boMember<be<u32>>(actor, 0x39c) = 0;
    *boMember<be<u8>>(actor, 0x3a0) = 1;
    *boMember<be<u8>>(actor, 0x3a1) = 1;
    *boMember<be<f32>>(actor, 0x4b4) = 1.f;
    void *collisionStatus = boMember<void>(actor, 0x754);
    boCall(0x02515F14, collisionStatus, 0xff, 1, actor);
    kind = *boMember<be<u8>>(actor, 0x3dc);
    actor->action = 0;
    actor->mode = 0;
    if (kind == 0) {
        boCall(0x0251677C, boMember<void>(actor, 0x790), gabi::at<void>(0x10191DB4));
        *boMember<gptr<void>>(actor, 0x7d4) = collisionStatus;
        *boMember<be<u8>>(actor, 0x3a9) = 3;
        boCall(0x02516518, boMember<void>(actor, 0x9e8), gabi::at<void>(0x10191E68));
        *boMember<gptr<mDoExt_McaMorf_c>>(actor, 0xedc) = actor->upperMorph.get();
        *boMember<gptr<void>>(actor, 0xa2c) = collisionStatus;
        u32 attack = *boMember<be<u32>>(actor, 0x790);
        *boMember<be<u8>>(actor, 0x7ff) = 0;
        *boMember<be<f32>>(actor, 0xcb8) = 15.f;
        *boMember<gptr<Actor>>(actor, 0xed0) = actor;
        *boMember<gptr<Actor>>(actor, 0xb18) = actor;
        *boMember<be<u32>>(actor, 0x790) = attack & ~1u;
        *boMember<be<f32>>(actor, 0xcb4) = 160.f;
        for (u32 i = 0; i < 10; ++i) {
            *boMember<be<u8>>(actor, 0xee0 + i) = gabi::load<u8>(0x10191E5C + i);
            *boMember<be<f32>>(actor, 0xeec + 4 * i) = gabi::load<f32>(0x10191E34 + 4 * i);
        }
        kind = *boMember<be<u8>>(actor, 0x3dc);
    }
    if (kind == 0 || kind == 1) {
        boCall(0x0251677C, boMember<void>(actor, 0x8bc), gabi::at<void>(0x10191DF4));
        kind = *boMember<be<u8>>(actor, 0x3dc);
        *boMember<gptr<void>>(actor, 0x900) = collisionStatus;
        if (kind == 1) {
            s16 timer = (s16)gabi::ftoi(gabi::load<f32>(0x1047BAC8) + 15.f);
            *boMember<be<s16>>(actor, 0x482) = timer;
            anm_init(actor, 8, 0.f, 0, 0.f, -1, 1);
            mDoExt_McaMorf_c *lower = actor->lowerMorph.get();
            actor->action = 4;
            actor->mode = 51;
            *boMember<be<u8>>(actor, 0x3ec) = 3;
            boCall(0x025E535C, lower, 0, 0, 0);
            draw_SUB(actor);
        }
    }
    play = boPlay();
    u32 player = gabi::load<u32>(play + 0x5b2c);
    s16 yaw = boCall<s16>(0x025D6894, actor, gabi::at<void>(player));
    boMember<cXyz>(actor, 0x420)->copy(actor->current.pos);
    kind = *boMember<be<u8>>(actor, 0x3dc);
    *boMember<be<s16>>(actor, 0x46a) = yaw;
    *boMember<be<s16>>(actor, 0x464) = yaw;
    if (kind == 2) {
        *boMember<be<u32>>(actor, 0x39c) = 0;
        mDoExt_McaMorf_c *upper = actor->upperMorph.get();
        *boMember<be<f32>>(actor, 0x4b0) = 100.f;
        actor->action = 3;
        actor->mode = 30;
        boCall(0x025E535C, upper, 0, 0, 0);
        draw_SUB(actor);
    }
    boCall(0x020BCEC8, actor);
    return phase;
}
VERIFY(0x020BFC50, daBO_Create);

static void upper_joint_commit(u32 model, u16 index) {
    u32 data = gabi::load<u32>(model + 0x2c);
    u16 flags = gabi::load<u16>(data + 4);
    u32 matrix = gabi::load<u32>(0x1018C7B0);
    gabi::store<u16>(data + 4, flags | 0x10);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(matrix + 4 * i);
    u32 destination = gabi::load<u32>(data + 0x10) + index * 0x30;
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(destination + 4 * i, values[i]);
    matrix = gabi::load<u32>(0x1018C7B0);
    boCall(0x028E90D4, gabi::at<void>(matrix), gabi::at<void>(0x104B4868));
}
static void upper_joint_position(Actor *actor, u32 offset) {
    gabi::Local<cXyz> origin;
    origin->x = 0;
    origin->y = 0;
    origin->z = 0;
    boCall(0x0200FCD8, origin.get(), boMember<void>(actor, offset));
}
static void upper_joint_rotate(Actor *actor, u32 target, u32 field, f32 factor) {
    s16 angle = *boMember<be<s16>>(actor, field);
    s16 scaled = (s16)gabi::ftoi((f32)angle * factor);
    u32 matrix = gabi::load<u32>(0x1018C7B0);
    boCall(target, gabi::at<void>(matrix), (s32)scaled);
}
static BOOL nodeCallBack_UP(J3DNode *node, s32 stage) {
    WWHD_FUNC(0x020BB1E4, BOOL, node, stage);
    if (stage != 0)
        return 1;
    J3DJoint *joint = boCall<J3DJoint *>(0x027F7878, node);
    u32 model = gabi::load<u32>(0x104B462C);
    Actor *actor = gabi::at<Actor>(gabi::load<u32>(model + 0xb8));
    u16 index = *boMember<be<u16>>(joint, 4);
    if (!actor)
        return 1;
    u32 play = boPlay();
    u32 data = gabi::load<u32>(model + 0x2c);
    u16 flags = gabi::load<u16>(data + 4);
    void *player = gabi::at<void>(gabi::load<u32>(play + 0x5b2c));
    u32 matrices = gabi::load<u32>(data + 0x10);
    gabi::store<u16>(data + 4, flags | 0x10);
    boCall(0x028E90D4, gabi::at<void>(matrices + index * 0x30),
           gabi::at<void>(gabi::load<u32>(0x1018C7B0)));
    bool changed = false;
    if (index <= 10) {
        f32 factor = (f32)index * 0.02500000037252903f;
        upper_joint_rotate(actor, 0x025F1C28, 0x458, factor);
        upper_joint_rotate(actor, 0x025F1BF4, 0x456, factor);
        upper_joint_rotate(actor, 0x025F1C5C, 0x45a, factor);
        changed = true;
        if (index == 1)
            upper_joint_position(actor, 0x414);
        else if (index == 10)
            upper_joint_position(actor, 0x444);
    }
    if (index != 1 && index != 10) {
        if (index == 11) {
            upper_joint_position(actor, 0x3fc);
            changed = true;
        } else if (index == 4) {
            upper_joint_position(actor, 0x42c);
            changed = true;
        } else if ((u32)index - 5 < 3) {
            f32 factor = (f32)(s16)(8 - index) * 1.75f;
            upper_joint_rotate(actor, 0x025F1C28, 0x470, factor);
            upper_joint_rotate(actor, 0x025F1C5C, 0x472, factor);
            changed = true;
        }
    }
    if (index == 8) {
        s16 yaw = *boMember<be<s16>>(actor, 0x452);
        s16 adjust = gabi::load<s16>(0x1047BD4E);
        u32 matrix = gabi::load<u32>(0x1018C7B0);
        boCall(0x025F1C28, gabi::at<void>(matrix), (s32)(s16)(yaw + adjust));
        s16 pitch = *boMember<be<s16>>(actor, 0x454);
        adjust = gabi::load<s16>(0x1047BD52);
        matrix = gabi::load<u32>(0x1018C7B0);
        boCall(0x025F1BF4, gabi::at<void>(matrix), (s32)(s16)(pitch + adjust));
        upper_joint_position(actor, 0x438);
        upper_joint_commit(model, index);
        return 1;
    }
    if (index == 12 && *boMember<be<u8>>(actor, 0x3e8) != 0) {
        gabi::Local<cXyz> origin, position;
        origin->x = 0;
        origin->y = 0;
        origin->z = 0;
        boCall(0x0200FCD8, origin.get(), position.get());
        f32 mouthX = *boMember<be<f32>>(actor, 0x438), x = position->x;
        u8 flipped = *boMember<be<u8>>(actor, 0x3e9);
        f32 mouthZ = *boMember<be<f32>>(actor, 0x440), z = position->z;
        gabi::Local<csXyz> angle;
        angle->x = flipped ? (s16)-24500 : (s16)23500;
        s16 yaw = boCall<s16>(0x020195B0, mouthX - x, mouthZ - z);
        angle->z = 0;
        angle->y = flipped ? (s16)(yaw + 0x7fff) : yaw;
        u32 vtable = *boMember<be<u32>>(player, 0xb4);
        u32 target = gabi::load<u32>(vtable + 0x11c);
        boVirtualCall(target, player, position.get(), angle.get());
        upper_joint_commit(model, index);
        return 1;
    }
    if (changed)
        upper_joint_commit(model, index);
    return 1;
}
VERIFY(0x020BB1E4, nodeCallBack_UP);

static void bo4_move(Actor *actor) {
    WWHD_FUNC(0x020C0248, void, actor);
    u32 emitter = 0;
    switch ((u8)actor->mode) {
    case 30: {
        u32 co = *boMember<be<u32>>(actor, 0x7a8), tg = *boMember<be<u32>>(actor, 0x7bc),
            at = *boMember<be<u32>>(actor, 0x790);
        *boMember<be<u32>>(actor, 0x7a8) = co & ~1u;
        *boMember<be<u32>>(actor, 0x7bc) = tg & ~1u;
        *boMember<be<u32>>(actor, 0x790) = at & ~1u;
        boCall(0x0251621C, boMember<void>(actor, 0x790));
        boCall(0x0251621C, boMember<void>(actor, 0x790));
        anm_init(actor, 7, 5.f, 0, 1.f, -1, 0);
        actor->speed.y = 30.f;
        *boMember<be<f32>>(actor, 0x370) = 10.f;
        *boMember<be<f32>>(actor, 0x374) = -3.f;
        u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
        s16 yaw = boCall<s16>(0x025D6894, actor, gabi::at<void>(player));
        f32 y = actor->current.pos.y;
        emitter = *boMember<be<u32>>(actor, 0x4d0);
        u8 mode = actor->mode;
        actor->current.angle.y = (s16)(yaw + 0x8000);
        *boMember<be<f32>>(actor, 0x4ac) = y - 80.f;
        actor->mode = (u8)(mode + 1);
        break;
    }
    case 31: {
        s16 pitch = actor->shape_angle.x;
        f32 speed = actor->speed.y;
        actor->shape_angle.x = (s16)(pitch - 0x1200);
        if (speed < 0.f && actor->current.pos.y < *boMember<be<f32>>(actor, 0x4ac)) {
            f32 ground = *boMember<be<f32>>(actor, 0x4ac);
            *boMember<be<f32>>(actor, 0x370) = 0;
            *boMember<be<f32>>(actor, 0x374) = -1.f;
            *boMember<be<f32>>(actor, 0x4b0) = 200.f;
            actor->current.pos.y = ground - 200.f;
            actor->shape_angle.x = (s16)0x8000;
            actor->speed.y = 10.f;
            effect_sound(actor, 0x5847, 0);
            u32 particles = gabi::load<u32>(boPlay() + 0x5ab0);
            boCall(0x025A847C, gabi::at<void>(particles), 0, 0x810a, boMember<cXyz>(actor, 0x444),
                   0, 0, 0xff, 0, -1, boMember<void>(actor, 0x1a8), boMember<void>(actor, 0x1a8),
                   0);
            *boMember<be<s16>>(actor, 0x486) = 10;
            *boMember<be<s16>>(actor, 0x492) = 180;
            boMember<cXyz>(actor, 0x4b8)->copy(*boMember<cXyz>(actor, 0x444));
            smoke_set(actor);
            anm_init(actor, 6, 0.f, 0, 1.f, -1, 0);
            emitter = *boMember<be<u32>>(actor, 0x4d0);
            actor->mode = 33;
        } else
            emitter = *boMember<be<u32>>(actor, 0x4d0);
        break;
    }
    case 33: {
        s16 yaw = *boMember<be<s16>>(actor, 0x464);
        f32 ground = *boMember<be<f32>>(actor, 0x4ac);
        actor->shape_angle.z = 0x7fff;
        s16 xAdjust = gabi::load<s16>(0x1047C0AC), yAdjust = gabi::load<s16>(0x1047C0AA);
        gabi::Local<csXyz> angle;
        gabi::Local<cXyz> position;
        angle->x = (s16)(xAdjust + 0x4000);
        angle->y = (s16)(yaw + 0xc000 + yAdjust);
        angle->z = 0;
        position->x = *boMember<be<f32>>(actor, 0x42c);
        position->z = *boMember<be<f32>>(actor, 0x434);
        f32 extra = gabi::load<f32>(0x1047C030);
        position->y = ((ground + 30.f) + extra) + 80.f;
        s32 room = (s8)actor->current.roomNo;
        boCall(0x025D5834, 463, 8, position.get(), room, angle.get(), &actor->scale, 0, 0);
        u8 mode = actor->mode;
        actor->scale.y = 0;
        actor->scale.z = 0;
        actor->mode = (u8)(mode + 1);
        *boMember<be<f32>>(actor, 0x4b4) = 0;
        actor->scale.x = 0;
        *boMember<be<s16>>(actor, 0x484) = 10;
        emitter = *boMember<be<u32>>(actor, 0x4d0);
        break;
    }
    case 34:
        if (*boMember<be<s16>>(actor, 0x484) == 0) {
            emitter = *boMember<be<u32>>(actor, 0x4d0);
            if (!emitter) {
                boCall(0x025D57E0, actor);
                emitter = *boMember<be<u32>>(actor, 0x4d0);
            }
        } else
            emitter = *boMember<be<u32>>(actor, 0x4d0);
        break;
    default:
        emitter = *boMember<be<u32>>(actor, 0x4d0);
        break;
    }
    if (emitter && *boMember<be<s16>>(actor, 0x486) == 0) {
        u8 alpha = *boMember<be<u8>>(actor, 0x493);
        gabi::store<u8>(emitter + 0x247, alpha);
        s16 life = (s16)(*boMember<be<s16>>(actor, 0x492) - 4);
        *boMember<be<s16>>(actor, 0x492) = life;
        if (life < 0)
            boCall(0x025A5F88, boMember<void>(actor, 0x4cc));
    }
}
VERIFY(0x020C0248, bo4_move);
static void bo5_move(Actor *actor) {
    WWHD_FUNC(0x020C0624, void, actor);
    switch ((u8)actor->mode) {
    case 40: {
        u32 tg = *boMember<be<u32>>(actor, 0x7bc), co = *boMember<be<u32>>(actor, 0x7a8),
            body = *boMember<be<u32>>(actor, 0xa00);
        *boMember<be<u32>>(actor, 0x7bc) = tg & ~1u;
        *boMember<be<s16>>(actor, 0x492) = 3;
        *boMember<be<u32>>(actor, 0x7a8) = co & ~1u;
        *boMember<be<u32>>(actor, 0xa00) = body & ~1u;
        boCall(0x0251621C, boMember<void>(actor, 0x9e8));
        boCall(0x0251621C, boMember<void>(actor, 0x790));
        gabi::store<u8>(0x101EACB7, 2);
        anm_init(actor, 15, 5.f, 2, 1.f, -1, 0);
        *boMember<be<u32>>(actor, 0x39c) = *boMember<be<u32>>(actor, 0x39c) & ~4u;
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        actor->mode = (u8)(actor->mode + 1);
        return;
    }
    case 41: {
        mDoExt_McaMorf_c *upper = actor->upperMorph.get();
        if (!boCall<s32>(0x027F2BF8, boMember<void>(upper, 0x98), 15.f))
            return;
        s16 count = (s16)(*boMember<be<s16>>(actor, 0x492) - 1);
        *boMember<be<s16>>(actor, 0x492) = count;
        if (count > 0)
            return;
        *boMember<be<u32>>(actor, 0x39c) = *boMember<be<u32>>(actor, 0x39c) & ~4u;
        anm_init(actor, 9, 0.f, 0, 0.f, -1, 0);
        u32 particles = gabi::load<u32>(boPlay() + 0x5ab0);
        boCall(0x025A847C, gabi::at<void>(particles), 0, 0x810a, boMember<cXyz>(actor, 0x444), 0, 0,
               0xff, 0, -1, boMember<void>(actor, 0x1a8), boMember<void>(actor, 0x1a8), 0);
        *boMember<be<s16>>(actor, 0x486) = 10;
        boMember<cXyz>(actor, 0x4b8)->copy(*boMember<cXyz>(actor, 0x444));
        *boMember<be<s16>>(actor, 0x492) = 180;
        smoke_set(actor);
        effect_sound(actor, 0x5801, 0);
        *boMember<be<u8>>(actor, 0x3ea) = 1;
        actor->action = 2;
        actor->mode = 20;
        return;
    }
    case 50: {
        u32 body = *boMember<be<u32>>(actor, 0xa00), co = *boMember<be<u32>>(actor, 0x7a8),
            tg = *boMember<be<u32>>(actor, 0x7bc);
        *boMember<be<u32>>(actor, 0xa00) = body & ~1u;
        *boMember<be<u32>>(actor, 0x7a8) = co & ~1u;
        *boMember<be<u32>>(actor, 0x7bc) = tg & ~1u;
        boCall(0x0251621C, boMember<void>(actor, 0x9e8));
        boCall(0x0251621C, boMember<void>(actor, 0x790));
        gabi::store<u8>(0x101EACB7, 2);
        anm_init(actor, 14, 5.f, 0, 1.f, -1, 0);
        *boMember<be<u32>>(actor, 0x39c) = 0;
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        actor->mode = (u8)(actor->mode + 1);
        [[fallthrough]];
    }
    case 51: {
        if (*boMember<be<s16>>(actor, 0x482) != 0)
            return;
        u32 upper = gabi::ea(actor->upperMorph.get());
        if (!(gabi::load<u8>(upper + 0xa7) & 1) && gabi::load<f32>(upper + 0x98) != 0.f)
            return;
        *boMember<be<s16>>(actor, 0x486) = 10;
        *boMember<be<s16>>(actor, 0x492) = 180;
        *boMember<be<u8>>(actor, 0x3ea) = 1;
        actor->action = 2;
        actor->mode = 20;
        return;
    }
    default:
        return;
    }
}
VERIFY(0x020C0624, bo5_move);

static void boParticle(u32 effect, cXyz *position, csXyz *angle = nullptr, cXyz *scale = nullptr) {
    u32 control = gabi::load<u32>(boPlay() + 0x5ab0);
    boCall(0x025A847C, gabi::at<void>(control), 0, effect, position, angle, scale, 0xff, 0, -1, 0,
           0, 0);
}
static void boDamageFinish(Actor *actor) {
    actor->action = 0;
    *boMember<be<s16>>(actor, 0x48e) = 0;
    actor->mode = 4;
    *boMember<be<s16>>(actor, 0x490) = 0;
    *boMember<be<u8>>(actor, 0x3e4) = 0;
    boParticle(0x8109, boMember<cXyz>(actor, 0x444));
}
static BOOL damage_check(Actor *actor) {
    WWHD_FUNC(0x020BC15C, BOOL, actor);
    void *player = gabi::at<void>(gabi::load<u32>(boPlay() + 0x5b2c));
    boCall(0x02515E50, boMember<void>(actor, 0x770));
    if (*boMember<be<s16>>(actor, 0x48a) != 0)
        return 0;
    void *shockPlayer = gabi::at<void>(gabi::load<u32>(boPlay() + 0x5b2c));
    if (*boMember<be<u32>>(shockPlayer, 0x3c0) & 0x20000) {
        f32 playerZ = *boMember<be<f32>>(shockPlayer, 0x3ec), z = actor->current.pos.z;
        f32 playerX = *boMember<be<f32>>(shockPlayer, 0x3e4), x = actor->current.pos.x;
        f32 dz = playerZ - z, dx = playerX - x;
        f32 distance = boCall<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        if (distance < 1000.f) {
            effect_sound(actor, 0x2855, 0x31);
            *boMember<be<u8>>(actor, 0x3e6) = 1;
            goto apply_reaction;
        }
    }
    if (!boCall<s32>(0x025162A4, boMember<void>(actor, 0x790)))
        return 0;
    {
        void *hit = boCall<void *>(0x02516300, boMember<void>(actor, 0x790));
        if (!hit)
            return 0;
        boMember<cXyz>(actor, 0x474)->copy(*boMember<cXyz>(actor, 0x85c));
        s16 invincible = (s16)gabi::ftoi(gabi::load<f32>(0x1047BAB8) + 8.f);
        f32 x = *boMember<be<f32>>(actor, 0x3fc), z = *boMember<be<f32>>(actor, 0x404);
        *boMember<be<s16>>(actor, 0x48a) = invincible;
        f32 playerX = *boMember<be<f32>>(player, 0x314),
            playerZ = *boMember<be<f32>>(player, 0x31c);
        s16 angle = boCall<s16>(0x020195B0, playerX - x, playerZ - z);
        *boMember<be<s16>>(actor, 0x48c) = angle;
        *boMember<be<s16>>(actor, 0x480) = 0;
        *boMember<be<u8>>(actor, 0x3e6) = 1;
        boCall(0x025E1FD8);
        u32 type = *boMember<be<u32>>(hit, 0x10);
        switch (type) {
        case 2: {
            u8 attack = *boMember<be<u8>>(player, 0x3ac);
            for (u32 index = 0; index < 32; ++index) {
                if (attack == gabi::load<u16>(0x10191D34 + index * 2)) {
                    u8 reaction = gabi::load<u8>(0x10191D74 + index);
                    *boMember<be<u8>>(actor, 0x3e6) = reaction;
                    goto apply_reaction;
                }
            }
            return 0;
        }
        case 0x40: {
            *boMember<be<u8>>(actor, 0x3e6) = 1;
            u8 timer = (u8)(gabi::load<s16>(0x1047BB12) + 5);
            *boMember<be<u8>>(actor, 0x7ff) = 0;
            u32 at = *boMember<be<u32>>(actor, 0x790);
            *boMember<be<s16>>(actor, 0x480) = timer;
            u32 status = *boMember<be<u32>>(actor, 0x39c);
            *boMember<be<f32>>(actor, 0x4b4) = 1.f;
            *boMember<be<u32>>(actor, 0x790) = at & ~1u;
            *boMember<be<u32>>(actor, 0x39c) = status & ~4u;
            boParticle(0x10, boMember<cXyz>(actor, 0x474));
            gabi::Local<cXyz> scale;
            scale->x = 2.f;
            scale->z = 2.f;
            scale->y = 2.f;
            boParticle(0xf, boMember<cXyz>(actor, 0x474), boMember<csXyz>(player, 0x328),
                       scale.get());
            monster_sound(actor, 0x4850);
            effect_sound(actor, 0x2828, 0x20);
            break;
        }
        case 0x200:
        case 0x40000: {
            f32 timer = gabi::load<f32>(0x1047BAC0) + 34.f;
            *boMember<be<u8>>(actor, 0x3ec) = 1;
            *boMember<be<s16>>(actor, 0xed4) = (s16)gabi::ftoi(timer);
            *boMember<be<u32>>(actor, 0x39c) = 0;
            break;
        }
        case 0x80000: {
            f32 timer = gabi::load<f32>(0x1047BAC4) + 80.f;
            *boMember<be<u8>>(actor, 0x3ec) = 2;
            *boMember<be<u8>>(actor, 0xb24) = 1;
            *boMember<be<s16>>(actor, 0xb1c) = (s16)gabi::ftoi(timer);
            *boMember<be<u8>>(actor, 0x3a1) = 30;
            *boMember<be<u32>>(actor, 0x39c) = 0;
            break;
        }
        case 0x100000: {
            *boMember<be<f32>>(actor, 0xcc4) = 1.f;
            *boMember<be<u8>>(actor, 0xb1e) = 1;
            f32 timer = gabi::load<f32>(0x1047BAC4) + 80.f;
            *boMember<be<u32>>(actor, 0x39c) = 0;
            s32 room = (s8)actor->current.roomNo;
            u8 variant = *boMember<be<u8>>(actor, 0x3dd);
            *boMember<be<f32>>(actor, 0xb20) = timer;
            *boMember<be<u8>>(actor, 0x3ec) = 3;
            boCall(0x025D5834, 214, ((u32)variant << 8) | 1, &actor->current.pos, room,
                   &actor->current.angle, &actor->scale, 0, 0);
            break;
        }
        case 0x10000:
            if (*boMember<be<u8>>(player, 0x3ac) == 17) {
                *boMember<be<u8>>(actor, 0x3e6) = 1;
                break;
            }
            *boMember<be<u8>>(actor, 0x3e6) = 2;
            goto apply_reaction;
        case 0x8000000: {
            if (*boMember<be<s8>>(actor, 0x3a9) > 0) {
                u8 health = *boMember<be<u8>>(actor, 0x3a1);
                *boMember<be<u8>>(actor, 0x3a1) = 10;
                void *attack = boCall<void *>(0x02516300, boMember<void>(actor, 0x790));
                gabi::Local<be<u32>[8]> hitInfo;
                (*hitInfo)[0] = gabi::ea(attack);
                (*hitInfo)[5] = 0;
                boCall(0x025192A8, actor, hitInfo.get());
                *boMember<be<u8>>(actor, 0x3a1) = health;
            }
            boParticle(0x27b, boMember<cXyz>(actor, 0x390));
            *boMember<be<u8>>(actor, 0x3e6) = 0;
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2836, 0x20);
            break;
        }
        default:
            *boMember<be<u8>>(actor, 0x3e6) = 1;
            break;
        }
    }
apply_reaction: {
    s16 yaw = *boMember<be<s16>>(actor, 0x464), hitYaw = *boMember<be<s16>>(actor, 0x48c);
    s16 delta = (s16)(hitYaw - yaw);
    u8 reaction = *boMember<be<u8>>(actor, 0x3e6);
    if (reaction == 2) {
        void *currentPlayer = gabi::at<void>(gabi::load<u32>(boPlay() + 0x5b2c));
        gabi::Local<cXyz> position;
        position->x = *boMember<be<f32>>(actor, 0x85c);
        position->y = *boMember<be<f32>>(actor, 0x860);
        position->z = *boMember<be<f32>>(actor, 0x864);
        boParticle(0x10, position.get());
        gabi::Local<cXyz> scale;
        scale->x = 2.f;
        scale->z = 2.f;
        scale->y = 2.f;
        boParticle(0xf, position.get(), boMember<csXyz>(currentPlayer, 0x328), scale.get());
        actor->action = 4;
        actor->mode = 40;
        if (actor) {
            monster_sound(actor, 0x4850);
            effect_sound(actor, 0x2828, 0x20);
        }
        return 1;
    }
    u32 attackFlags = *boMember<be<u32>>(actor, 0x790);
    reaction = *boMember<be<u8>>(actor, 0x3e6);
    *boMember<be<u32>>(actor, 0x790) = attackFlags & ~1u;
    *boMember<be<u8>>(actor, 0x7ff) = 0;
    if (reaction == 1) {
        nokezori_damage_rtn(actor);
        *boMember<be<s16>>(actor, 0x48e) = 0;
        *boMember<be<s16>>(actor, 0x490) = 0;
        *boMember<be<u8>>(actor, 0x3e4) = 0;
        boParticle(0x8109, boMember<cXyz>(actor, 0x444));
        return 1;
    }
    if (gabi::load<f32>(0x1047BABC) != 0.f && actor->mode == 4)
        return 0;
    if (actor)
        monster_sound(actor, 0x484f);
    gabi::Local<cXyz> direction, result;
    direction->x = 0;
    direction->y = 0;
    direction->z = 2250.f;
    boCall(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), (s32)delta);
    boCall(0x0200FCD8, direction.get(), result.get());
    anm_init(actor, 18, 2.f, 0, 1.f, -1, 0);
    f32 x = result->x, z = result->z;
    yaw = *boMember<be<s16>>(actor, 0x464);
    hitYaw = *boMember<be<s16>>(actor, 0x48c);
    *boMember<be<s16>>(actor, 0x472) = (s16)gabi::ftoi(x);
    *boMember<be<s16>>(actor, 0x470) = (s16)gabi::ftoi(z);
    *boMember<be<s16>>(actor, 0x49a) = 0;
    *boMember<be<s16>>(actor, 0x49c) = 7250;
    *boMember<be<f32>>(actor, 0x4a4) = -10000.f;
    *boMember<be<s16>>(actor, 0x49e) = (s16)(hitYaw - yaw);
    void *currentPlayer = gabi::at<void>(gabi::load<u32>(boPlay() + 0x5b2c));
    u8 attack = *boMember<be<u8>>(currentPlayer, 0x3ac);
    if (attack == 3)
        *boMember<be<s16>>(actor, 0x49e) = (s16)(*boMember<be<s16>>(actor, 0x49e) - 0x4000);
    else if (attack == 4)
        *boMember<be<s16>>(actor, 0x49e) = (s16)(*boMember<be<s16>>(actor, 0x49e) + 0x4000);
    *boMember<be<s16>>(actor, 0x498) = 0x800;
    void *hit = boCall<void *>(0x02516300, boMember<void>(actor, 0x790));
    if (hit && actor && gabi::ea(boMember<void>(actor, 0x37c))) {
        u32 type = *boMember<be<u32>>(hit, 0x10);
        if (type & 2)
            effect_sound(actor, 0x2803, 0x31);
        else
            effect_sound(actor, type & 0x80 ? 0x2833 : 0x2834, 0x31);
    }
    boDamageFinish(actor);
    return 1;
}
}
VERIFY(0x020BC15C, damage_check);

// The two fading particle callbacks share the upper body's ninth joint.
static void boEmitterMatrix(Actor *actor, u32 emitter, bool identityCall) {
    u32 morph = gabi::ea(actor->upperMorph.get());
    u32 model = gabi::load<u32>(morph + 0x90);
    u32 data = gabi::load<u32>(model + 0x2c);
    u16 flags = gabi::load<u16>(data + 4);
    u32 matrices = gabi::load<u32>(data + 0x10);
    gabi::store<u16>(data + 4, flags | 0x10);
    void *rotation = gabi::at<void>(emitter + 0x1f0);
    if (identityCall)
        rotation = identity_pointer(rotation);
    boCall(0x02824890, gabi::at<void>(matrices + 0x1b0), rotation, gabi::at<void>(emitter + 0x220),
           gabi::at<void>(emitter + 0x22c));
}
static u32 boSpawnParticleReturn(Actor *actor, u32 effect) {
    u32 control = gabi::load<u32>(boPlay() + 0x5ab0);
    return gabi::ea(boCall<void *>(0x025A847C, gabi::at<void>(control), 0, effect,
                                   &actor->current.pos, 0, 0, 0xff, 0, -1, 0, 0, 0));
}
static void boFinishEmitter(Actor *actor, u32 emitterOffset, u32 stateOffset, bool skip) {
    u32 emitter = *boMember<be<u32>>(actor, emitterOffset);
    if (skip || !emitter)
        return;
    boEmitterMatrix(actor, emitter, true);
    u8 state = *boMember<be<u8>>(actor, stateOffset);
    if (state == 0) {
        emitter_set_flags(gabi::at<void>(*boMember<be<u32>>(actor, emitterOffset)), 1);
        *boMember<be<u8>>(actor, stateOffset) = (u8)(*boMember<be<u8>>(actor, stateOffset) + 1);
    } else if (state == 1) {
        emitter = *boMember<be<u32>>(actor, emitterOffset);
        u32 first = particle_list_count(gabi::at<void>(emitter + 0x1ac));
        u32 second = particle_list_count(gabi::at<void>(emitter + 0x1b8));
        if (first + second == 0) {
            gabi::store<u32>(emitter + 0x1e8, 0);
            u32 current = *boMember<be<u32>>(actor, emitterOffset);
            gabi::store<u32>(current + 0x5c, 0xffffffff);
            emitter_set_flags(gabi::at<void>(current), 1);
            *boMember<be<u32>>(actor, emitterOffset) = 0;
            *boMember<be<u8>>(actor, stateOffset) = 0;
        }
    }
}
static void boRegisterBody(Actor *actor) {
    boCall(0x02018D40, boMember<void>(actor, 0x8a8), boMember<cXyz>(actor, 0x3fc));
    boCall(0x02018C8C, boMember<void>(actor, 0x8a8), 60.f);
    boCall(0x0200E240, gabi::at<void>(boPlay() + 0x26a4), boMember<void>(actor, 0x790));
}
static void boExecuteFinish(Actor *actor) {
    cXyz &attentionPosition = *boMember<cXyz>(actor, 0x390);
    u8 state = *boMember<be<u8>>(actor, 0x3e2);
    s32 animation = *boMember<be<s32>>(actor, 0x3f8);
    boFinishEmitter(actor, 0x4f0, 0x3e2, state == 0 && (animation == 21 || animation == 22));
    state = *boMember<be<u8>>(actor, 0x3e3);
    animation = *boMember<be<s32>>(actor, 0x3f8);
    boFinishEmitter(actor, 0x4f4, 0x3e3, state == 0 && animation == 13);
    f32 target = *boMember<be<f32>>(actor, 0x4b4);
    boCall(0x0200ED84, &actor->scale.y, target, 1.f, 0.0874999985f);
    *boMember<be<u8>>(actor, 0x768) = 0xff;
    boCall(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018c7b0)), (s16)actor->current.angle.y);
    boCall(0x025F1BF4, gabi::at<void>(gabi::load<u32>(0x1018c7b0)), (s16)actor->current.angle.x);
    gabi::Local<cXyz> forward, velocity;
    forward->y = 0;
    forward->x = 0;
    forward->z = actor->speedF;
    boCall(0x0200FCD8, forward.get(), velocity.get());
    f32 vertical = actor->speed.y + actor->gravity;
    actor->speed.x = velocity->x;
    actor->speed.y = vertical;
    actor->speed.z = velocity->z;
    boCall(0x025D6800, actor, boMember<void>(actor, 0x754));
    u8 kind = *boMember<be<u8>>(actor, 0x3dc);
    if (kind == 0 || kind == 2)
        boCall(0x025E535C, actor->upperMorph.get(), 0, 0, 0);
    kind = *boMember<be<u8>>(actor, 0x3dc);
    if (kind == 0 || kind == 1) {
        boCall(0x025E535C, actor->lowerMorph.get(), 0, 0, 0);
        if (*boMember<be<u8>>(actor, 0x3dc) == 0) {
            if (actor->mode != 5) {
                attentionPosition = *boMember<cXyz>(actor, 0x3fc);
                attentionPosition.y = attentionPosition.y + 60.f;
                *boMember<cXyz>(actor, 0x37c) = *boMember<cXyz>(actor, 0x444);
            } else {
                cXyz *root = boMember<cXyz>(actor, 0x414);
                *boMember<be<f32>>(actor, 0x37c) = root->x;
                *boMember<be<f32>>(actor, 0x380) = root->y;
                f32 raised = root->y + 60.f;
                attentionPosition.x = root->x;
                *boMember<be<f32>>(actor, 0x380) = raised;
                *boMember<be<f32>>(actor, 0x384) = root->z;
                attentionPosition.y = raised;
                attentionPosition.z = root->z;
            }
        }
    }
    if (*boMember<be<u8>>(actor, 0x3e8) || actor->mode == 12) {
        attentionPosition.x = actor->current.pos.x;
        attentionPosition.z = actor->current.pos.z;
        attentionPosition.y = actor->current.pos.y + 60.f;
        actor->shape_angle.y = *boMember<be<s16>>(actor, 0x464);
    }
    boRegisterBody(actor);
    if (actor->action == 0) {
        boCall(0x020182E0, boMember<void>(actor, 0xb00), boMember<cXyz>(actor, 0x414));
        boCall(0x02018428, boMember<void>(actor, 0xb00), 160.f);
        boCall(0x020184DC, boMember<void>(actor, 0xb00), 15.f);
        boCall(0x0200E240, gabi::at<void>(boPlay() + 0x26a4), boMember<void>(actor, 0x9e8));
    }
    kind = *boMember<be<u8>>(actor, 0x3dc);
    if (kind == 0 || kind == 1) {
        gabi::Local<cXyz> center;
        center->x = actor->current.pos.x;
        center->y = actor->current.pos.y;
        center->z = actor->current.pos.z;
        center->y = center->y + 10.f;
        boCall(0x02018D40, boMember<void>(actor, 0x9d4), center.get());
        boCall(0x02018C8C, boMember<void>(actor, 0x9d4), 45.f);
        boCall(0x0200E240, gabi::at<void>(boPlay() + 0x26a4), boMember<void>(actor, 0x8bc));
    }
    draw_SUB(actor);
}

static void bo3_move(Actor *actor) {
    u8 mode = actor->mode;
    if (mode == 20) {
        s16 delay = *boMember<be<s16>>(actor, 0x480);
        if (delay) {
            delay = (s16)(delay - 1);
            *boMember<be<s16>>(actor, 0x480) = delay;
        }
        if (!delay) {
            u32 cylinder = *boMember<be<u32>>(actor, 0xa00);
            u32 target = *boMember<be<u32>>(actor, 0x7bc);
            u32 collision = *boMember<be<u32>>(actor, 0x7a8);
            *boMember<be<u32>>(actor, 0xa00) = cylinder & ~1u;
            *boMember<be<u32>>(actor, 0x7a8) = collision & ~1u;
            *boMember<be<u32>>(actor, 0x7bc) = target & ~1u;
            boCall(0x0251621C, boMember<void>(actor, 0x9e8));
            boCall(0x0251621C, boMember<void>(actor, 0x790));
            gabi::Local<csXyz> angles;
            angles->x = actor->current.angle.x;
            angles->y = *boMember<be<s16>>(actor, 0x464);
            angles->z = actor->current.angle.z;
            if (*boMember<be<u8>>(actor, 0x3ea) == 0)
                boCall(0x025D5834, 0xd6, 2, boMember<cXyz>(actor, 0x420),
                       (s32)(s8)actor->current.roomNo, angles.get(), &actor->scale, 0, 0);
            *boMember<be<u8>>(actor, 0x3ea) = 0;
            *boMember<be<u8>>(actor, 0x3dc) = 1;
            anm_init(actor, 19, 5.f, 0, 1.f, -1, 1);
            gabi::store<u8>(0x101eacb7, 4);
            actor->mode = (u8)(actor->mode + 1);
        }
    } else if (mode == 21) {
        u32 lower = gabi::ea(actor->lowerMorph.get());
        if (boCall<BOOL>(0x027F2BF8, gabi::at<void>(lower + 0x98), 43.f)) {
            s32 disappearType = 0;
            if (*boMember<be<u8>>(actor, 0x3dd)) {
                *boMember<be<s16>>(actor, 0x484) = 50;
                disappearType = 3;
            }
            if (*boMember<be<u8>>(actor, 0x3ec) != 3)
                boCall(0x025D99E8, actor, &actor->current.pos, 5, disappearType,
                       (u32)*boMember<be<u8>>(actor, 0x3a8));
        }
        lower = gabi::ea(actor->lowerMorph.get());
        if ((gabi::load<u8>(lower + 0xa7) & 1) || gabi::load<f32>(lower + 0x98) == 0.f) {
            if (*boMember<be<u8>>(actor, 0x3dd))
                actor->mode = (u8)(actor->mode + 1);
            else {
                u32 save = gabi::load<u32>(0x101f84dc);
                u16 number = *boMember<be<u16>>(actor, 0x2d8);
                s32 room = (s8)*boMember<be<u8>>(actor, 0x2fe);
                boCall(0x025BA5D4, gabi::at<void>(save + 0x20), (u32)number, room);
                boCall(0x025D57E0, actor);
            }
        }
    } else if (mode == 22 && *boMember<be<s16>>(actor, 0x484) == 0) {
        boCall(0x025D5834, 0xd5, 1, &actor->current.pos, (s32)(s8)actor->current.roomNo,
               &actor->current.angle, &actor->scale, 0, 0);
        u32 save = gabi::load<u32>(0x101f84dc);
        u16 number = *boMember<be<u16>>(actor, 0x2d8);
        s32 room = (s8)*boMember<be<u8>>(actor, 0x2fe);
        boCall(0x025BA5D4, gabi::at<void>(save + 0x20), (u32)number, room);
        boCall(0x025D57E0, actor);
    }
    u32 emitter = *boMember<be<u32>>(actor, 0x4d0);
    if (emitter && *boMember<be<s16>>(actor, 0x486) == 0) {
        u8 alpha = *boMember<be<u8>>(actor, 0x493);
        gabi::store<u8>(emitter + 0x247, alpha);
        s16 next = (s16)(*boMember<be<s16>>(actor, 0x492) - 4);
        *boMember<be<s16>>(actor, 0x492) = next;
        if (next < 0)
            boCall(0x025A5F88, boMember<void>(actor, 0x4cc));
    }
}

static void boApproachAngles(Actor *actor) {
    boCall(0x0200F428, boMember<be<s16>>(actor, 0x464), (s16)*boMember<be<s16>>(actor, 0x46a), 1,
           0x800);
    boCall(0x0200F428, boMember<be<s16>>(actor, 0x458), (s16)*boMember<be<s16>>(actor, 0x45e), 1,
           (s16)*boMember<be<s16>>(actor, 0x498));
    boCall(0x0200F428, boMember<be<s16>>(actor, 0x45a), (s16)*boMember<be<s16>>(actor, 0x460), 1,
           (s16)*boMember<be<s16>>(actor, 0x498));
}
static bool boAnimationEnded(Actor *actor) {
    u32 morph = gabi::ea(actor->upperMorph.get());
    return (gabi::load<u8>(morph + 0xa7) & 1) || gabi::load<f32>(morph + 0x98) == 0.f;
}
static void boStopAttack(Actor *actor) {
    u32 flags = *boMember<be<u32>>(actor, 0x790);
    *boMember<be<u8>>(actor, 0x7ff) = 0;
    *boMember<be<u32>>(actor, 0x790) = flags & ~1u;
}
static void boRecoilMove(Actor *actor, u32 player) {
    u8 attack = gabi::load<u8>(player + 0x3ac);
    s16 turn = *boMember<be<s16>>(actor, 0x49a);
    u32 matrix = gabi::load<u32>(0x1018c7b0);
    if (attack != 3)
        turn = (s16)-turn;
    boCall(0x025F1884, gabi::at<void>(matrix), turn);
    gabi::Local<cXyz> forward, displacement;
    forward->x = 0;
    forward->z = *boMember<be<f32>>(actor, 0x4a4);
    forward->y = 0;
    boCall(0x0200FCD8, forward.get(), displacement.get());
    s16 z = (s16)gabi::ftoi(displacement->z);
    s16 x = (s16)gabi::ftoi(displacement->x);
    s16 angle = *boMember<be<s16>>(actor, 0x49a);
    s16 angularVelocity = *boMember<be<s16>>(actor, 0x49c);
    *boMember<be<s16>>(actor, 0x452) = z;
    *boMember<be<s16>>(actor, 0x454) = x;
    *boMember<be<s16>>(actor, 0x49a) = (s16)(angle + angularVelocity);
    boCall(0x0200EDC8, boMember<be<f32>>(actor, 0x4a4), 1.f, 250.f);
    boCall(0x0200F428, boMember<be<s16>>(actor, 0x470), 0, 1, 0x87);
    boCall(0x0200F428, boMember<be<s16>>(actor, 0x472), 0, 1, 0x87);
    if (__builtin_fabsf(*boMember<be<f32>>(actor, 0x4a4)) < 1.f) {
        *boMember<be<s16>>(actor, 0x46e) = 0;
        *boMember<be<s16>>(actor, 0x470) = 0;
        *boMember<be<s16>>(actor, 0x472) = 0;
        *boMember<be<s16>>(actor, 0x450) = 0;
        *boMember<be<s16>>(actor, 0x452) = 0;
        *boMember<be<s16>>(actor, 0x454) = 0;
        *boMember<be<s16>>(actor, 0x482) = 0;
        s16 delay = *boMember<be<s16>>(actor, 0x480);
        actor->mode = 2;
        if (delay)
            *boMember<be<s16>>(actor, 0x488) = 45;
    }
}
static void boDisableCollision(Actor *actor) {
    u32 cylinder = *boMember<be<u32>>(actor, 0xa00);
    u32 target = *boMember<be<u32>>(actor, 0x7bc);
    u32 collision = *boMember<be<u32>>(actor, 0x7a8);
    *boMember<be<u32>>(actor, 0xa00) = cylinder & ~1u;
    *boMember<be<u32>>(actor, 0x7a8) = collision & ~1u;
    *boMember<be<u32>>(actor, 0x7bc) = target & ~1u;
    boCall(0x0251621C, boMember<void>(actor, 0x9e8));
    boCall(0x0251621C, boMember<void>(actor, 0x790));
}
static void boUnhide(Actor *actor, u32 sound, bool resetAngles) {
    anm_init(actor, 17, 5.f, 0, 1.f, -1, 0);
    anm_init(actor, 16, 5.f, 0, 1.f, -1, 1);
    *boMember<be<u32>>(actor, 0x39c) = *boMember<be<u32>>(actor, 0x39c) & ~4u;
    effect_sound(actor, sound, 0);
    boDisableCollision(actor);
    if (resetAngles)
        angle_initial(actor);
}
static void boWake(Actor *actor) {
    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    if (!(boCall<f32>(0x025D68EC, actor, gabi::at<void>(player)) < 600.f))
        return;
    u32 cylinder = *boMember<be<u32>>(actor, 0xa00);
    u32 target = *boMember<be<u32>>(actor, 0x7bc);
    u32 collision = *boMember<be<u32>>(actor, 0x7a8);
    *boMember<be<u32>>(actor, 0xa00) = cylinder | 1;
    *boMember<be<u32>>(actor, 0x7a8) = collision | 1;
    *boMember<be<u32>>(actor, 0x7bc) = target | 1;
    effect_sound(actor, 0x5844, 0);
    monster_sound(actor, 0x484d);
    anm_init(actor, 9, 5.f, 0, 1.f, -1, 0);
    anm_init(actor, 8, 5.f, 0, 1.f, -1, 1);
    u32 attack = *boMember<be<u32>>(actor, 0x790);
    u32 status = *boMember<be<u32>>(actor, 0x39c);
    *boMember<be<u32>>(actor, 0x794) = 1;
    *boMember<be<u32>>(actor, 0x790) = (attack | 1) & ~0x10u;
    *boMember<be<u8>>(actor, 0x7ff) = 1;
    actor->mode = 1;
    *boMember<be<u32>>(actor, 0x39c) = status | 4;
}
static void boEmerging(Actor *actor) {
    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    *boMember<be<s16>>(actor, 0x46a) = boCall<s16>(0x025D6894, actor, gabi::at<void>(player));
    u32 emitter = *boMember<be<u32>>(actor, 0x4ec);
    if (!emitter) {
        emitter = boSpawnParticleReturn(actor, 0x8106);
        *boMember<be<u32>>(actor, 0x4ec) = emitter;
    } else {
        u32 morph = gabi::ea(actor->upperMorph.get());
        u32 model = gabi::load<u32>(morph + 0x90);
        u32 data = gabi::load<u32>(model + 0x2c);
        u16 flags = gabi::load<u16>(data + 4);
        u32 matrices = gabi::load<u32>(data + 0x10);
        gabi::store<u16>(data + 4, flags | 0x10);
        boCall(0x028249B0, gabi::at<void>(matrices + 0x1b0), gabi::at<void>(emitter + 0x1f0),
               gabi::at<void>(emitter + 0x22c));
    }
    u32 morph = gabi::ea(actor->upperMorph.get());
    f32 frame = gabi::load<f32>(morph + 0x9c);
    if (frame > 39.f)
        boStopAttack(actor);
    if (boAnimationEnded(actor)) {
        emitter = *boMember<be<u32>>(actor, 0x4ec);
        if (emitter) {
            u32 flags = gabi::load<u32>(emitter + 0x254);
            gabi::store<u32>(emitter + 0x5c, 0xffffffff);
            gabi::store<u32>(emitter + 0x254, flags | 1);
            *boMember<be<u32>>(actor, 0x4ec) = 0;
        }
        wait_initial(actor);
    }
}
static void boWaiting(Actor *actor) {
    if (*boMember<be<s16>>(actor, 0x488))
        return;
    u32 emitter = *boMember<be<u32>>(actor, 0x4f0);
    if (!emitter) {
        emitter = boSpawnParticleReturn(actor, 0x8107);
        *boMember<be<u32>>(actor, 0x4f0) = emitter;
        if (emitter)
            gabi::store<u32>(emitter + 0x1e8, gabi::ea(actor) + 0x4f8);
        *boMember<be<u8>>(actor, 0x3e2) = 0;
    } else
        boEmitterMatrix(actor, emitter, false);
    if (*boMember<be<s16>>(actor, 0x482) == 0) {
        u32 morph = gabi::ea(actor->upperMorph.get());
        f32 frame = gabi::load<f32>(morph + 0x9c);
        s32 next = *boMember<be<s32>>(actor, 0x3f8) == 22 ? 21 : 22;
        anm_init(actor, next, 20.f, 2, 1.f, -1, 0);
        gabi::store<f32>(gabi::ea(actor->upperMorph.get()) + 0x9c, (f32)(s16)gabi::ftoi(frame));
        f32 random = boCall<f32>(0x020198D8, 60.f);
        *boMember<be<s16>>(actor, 0x482) = (s16)gabi::ftoi(random + 60.f);
    }
    bool attacking = false;
    if (*boMember<be<s16>>(actor, 0x484) == 0) {
        u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
        attacking = boCall<f32>(0x025D68EC, actor, gabi::at<void>(player)) < 380.f;
    }
    if (attacking) {
        anm_init(actor, 5, 5.f, 0, 1.f, -1, 0);
        if (boCall<f32>(0x02019788) < 0.5f) {
            *boMember<be<u8>>(actor, 0x7a4) = 0;
            actor->action = 1;
            actor->mode = 10;
        } else {
            u32 flags = *boMember<be<u32>>(actor, 0x790);
            u32 target = *boMember<be<u32>>(actor, 0x7bc);
            *boMember<be<u8>>(actor, 0x7a4) = 1;
            *boMember<be<u32>>(actor, 0x790) = flags & ~0x10u;
            *boMember<be<u32>>(actor, 0x7bc) = target | 1;
            monster_sound(actor, 0x484e);
            actor->mode = 3;
        }
    } else {
        u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
        if (boCall<f32>(0x025D68EC, actor, gabi::at<void>(player)) > 900.f) {
            boUnhide(actor, 0x5848, true);
            actor->mode = 0;
        }
    }
    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    *boMember<be<s16>>(actor, 0x46a) = boCall<s16>(0x025D6894, actor, gabi::at<void>(player));
}
static void boStriking(Actor *actor, u32 companion, bool active) {
    if (active && *boMember<be<s32>>(actor, 0x3f8) == 5) {
        u32 morph = gabi::ea(actor->upperMorph.get());
        f32 frame = gabi::load<f32>(morph + 0x9c);
        if (!(frame < 27.f) && gabi::load<f32>(morph + 0x9c) < 55.f) {
            if (gabi::load<f32>(morph + 0x9c) < 38.f) {
                if (gabi::load<f32>(morph + 0x9c) == 27.f) {
                    *boMember<be<u32>>(actor, 0x790) = *boMember<be<u32>>(actor, 0x790) | 1;
                    *boMember<be<u32>>(actor, 0x794) = 1;
                    effect_sound(actor, 0x5845, 0);
                } else if (*boMember<be<u32>>(actor, 0x7e4) & 1) {
                    *boMember<be<u8>>(actor, 0x7ff) = 0;
                    actor->mode = 6;
                    *boMember<be<u32>>(actor, 0x790) = *boMember<be<u32>>(actor, 0x790) & ~1u;
                    anm_init(actor, 12, 0.f, 0, 1.f, -1, 0);
                } else if (boCall<BOOL>(0x025160DC, boMember<void>(actor, 0x790))) {
                    void *hit = boCall<void *>(0x02515BBC, boMember<void>(actor, 0x7e0));
                    if (hit && gabi::ea(hit) == companion) {
                        boStopAttack(actor);
                        anm_init(actor, 12, 0.f, 0, 1.f, -1, 0);
                    }
                }
            } else if (boCall<BOOL>(0x027F2BF8, gabi::at<void>(morph + 0x98), 38.f))
                boStopAttack(actor);
        } else {
            u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
            *boMember<be<s16>>(actor, 0x46a) =
                boCall<s16>(0x025D6894, actor, gabi::at<void>(player));
        }
    }
    if (boAnimationEnded(actor)) {
        f32 random = boCall<f32>(0x020198D8, 30.f);
        *boMember<be<s16>>(actor, 0x484) = (s16)gabi::ftoi(random + 30.f);
        wait_initial(actor);
    }
}

// Private integration proposal; not registered source.
// Requires boSpawnParticleReturn(Actor*,u32) and boEmitterMatrix(Actor*,u32,bool).
static void bo2_move(Actor *actor) {
    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    u32 companion = gabi::load<u32>(boPlay() + 0x5b2c);
    f32 stickX = boCall<f32>(0x0200796C, 0);
    f32 stickY = boCall<f32>(0x02007990, 0);
    u8 mode = actor->mode;
    switch (mode) {
    case 10: {
        anm_init(actor, 20, 5.f, 0, 1.f, -1, 0);
        effect_sound(actor, 0x5849, 0);
        u32 attack = *boMember<be<u32>>(actor, 0x790);
        u8 next = actor->mode;
        *boMember<be<u8>>(actor, 0x3e7) = 0;
        u32 target = *boMember<be<u32>>(actor, 0x7bc);
        next = (u8)(next + 1);
        *boMember<be<u32>>(actor, 0x790) = attack | 0x10;
        *boMember<be<u8>>(actor, 0x3e8) = 0;
        actor->mode = next;
        *boMember<be<u32>>(actor, 0x7bc) = target & ~1u;
        break;
    }
    case 11: {
        u32 upper = gabi::ea(actor->upperMorph.get());
        f32 frame = gabi::load<f32>(upper + 0x9c);
        if (!(frame < 28.f)) {
            frame = gabi::load<f32>(upper + 0x9c);
            if (!(frame > 33.f)) {
                if (boCall<BOOL>(0x025160DC, boMember<void>(actor, 0x790))) {
                    companion = gabi::ea(boCall<void *>(0x02515BBC, boMember<void>(actor, 0x7e0)));
                    if (companion && companion == player) {
                        u32 actualPlayer = gabi::load<u32>(boPlay() + 0x5b34);
                        if (companion == actualPlayer)
                            *boMember<be<u8>>(actor, 0x3e7) = 1;
                    }
                }
                upper = gabi::ea(actor->upperMorph.get());
            }
        }
        if (boCall<BOOL>(0x027F2BF8, gabi::at<void>(upper + 0x98), 27.f)) {
            u32 attack = *boMember<be<u32>>(actor, 0x790);
            *boMember<be<u32>>(actor, 0x790) = attack | 1;
            *boMember<be<u32>>(actor, 0x794) = 1;
            // HD uses r26 as the sound's retained actor ID here.
            if (actor && gabi::ea(actor) + 0x37c)
                companion = *boMember<be<u32>>(actor, 4);
            monster_sound(actor, 0x4851);
        }
        upper = gabi::ea(actor->upperMorph.get());
        BOOL passed = boCall<BOOL>(0x027F2BF8, gabi::at<void>(upper + 0x98), 33.f);
        upper = gabi::ea(actor->upperMorph.get());
        if (passed) {
            u32 attack = *boMember<be<u32>>(actor, 0x790);
            *boMember<be<u8>>(actor, 0x7ff) = 0;
            *boMember<be<u32>>(actor, 0x790) = attack & ~1u;
        }
        if ((gabi::load<u8>(upper + 0xa7) & 1) || gabi::load<f32>(upper + 0x98) == 0.f) {
            wait_initial(actor);
            f32 random = boCall<f32>(0x020198D8, 30.f);
            *boMember<be<s16>>(actor, 0x484) = (s16)gabi::ftoi((f32)(random + 30.f));
        }
        break;
    }
    case 12: {
        u32 upper;
        if (*boMember<be<u8>>(actor, 0x3e8) == 0) {
            upper = gabi::ea(actor->upperMorph.get());
            BOOL passed = boCall<BOOL>(0x027F2BF8, gabi::at<void>(upper + 0x98), 33.f);
            bool setup = passed != 0;
            if (!passed) {
                upper = gabi::ea(actor->upperMorph.get());
                setup = !(gabi::load<f32>(upper + 0x9c) < 33.f);
            }
            if (setup) {
                gabi::store<u32>(companion + 0x428, 0);
                gabi::store<s16>(companion + 0x420, 3);
                gabi::store<u32>(companion + 0x430, 30);
                s16 target = boCall<s16>(0x025D6894, gabi::at<void>(player), actor);
                s16 facing = gabi::load<s16>(player + 0x32a);
                s16 difference = boCall<s16>(0x0200FAAC, (s32)facing, (s32)target);
                *boMember<be<u8>>(actor, 0x3e9) = difference > 0x4000 ? 1 : 0;
                *boMember<be<u8>>(actor, 0x3e8) = 1;
                upper = gabi::ea(actor->upperMorph.get());
            }
        } else
            upper = gabi::ea(actor->upperMorph.get());
        if (!(gabi::load<u8>(upper + 0xa7) & 1) && gabi::load<f32>(upper + 0x98) != 0.f)
            break;
        anm_init(actor, 13, 5.f, 2, 1.f, -1, 0);
        u32 emitter = boSpawnParticleReturn(actor, 0x810c);
        if (emitter)
            boEmitterMatrix(actor, emitter, false);
        *boMember<be<s16>>(actor, 0x492) = 0;
        *boMember<be<s16>>(actor, 0x484) = 0;
        *boMember<be<s32>>(actor, 0x3f0) = 1;
        *boMember<be<s32>>(actor, 0x3f4) = 1;
        monster_sound(actor, 0x4852);
        actor->mode = (u8)(actor->mode + 1);
        break;
    }
    case 13: {
        u32 emitter = *boMember<be<u32>>(actor, 0x4f4);
        if (!emitter) {
            emitter = boSpawnParticleReturn(actor, 0x8107);
            *boMember<be<u32>>(actor, 0x4f4) = emitter;
            if (emitter) {
                gabi::store<u32>(emitter + 0x1e8, gabi::ea(actor) + 0x4f8);
                u32 current = *boMember<be<u32>>(actor, 0x4f4);
                gabi::store<f32>(current + 0x34, 0.07f);
            }
            *boMember<be<u8>>(actor, 0x3e3) = 0;
        } else
            boEmitterMatrix(actor, emitter, false);
        if (*boMember<be<s16>>(actor, 0x484) == 0) {
            u32 actualPlayer = gabi::load<u32>(boPlay() + 0x5b34);
            bool alternate = (gabi::load<u32>(actualPlayer + 0x3bc) & 1) != 0;
            if (!alternate)
                alternate = gabi::load<s16>(actualPlayer + 0x699e) != 0;
            actualPlayer = gabi::load<u32>(boPlay() + 0x5b34);
            boCall(alternate ? 0x023F4CB0 : 0x023F51D0, gabi::at<void>(actualPlayer), -1.f);
            *boMember<be<s16>>(actor, 0x484) = 30;
        }
        s16 count = (s16)(*boMember<be<s16>>(actor, 0x492) + 1);
        if (count > 120) {
            *boMember<be<s16>>(actor, 0x492) = 0;
            anm_init(actor, 11, 5.f, 0, 1.f, -1, 0);
            monster_sound(actor, 0x5846);
            u32 camera = gabi::load<u32>(boPlay() + 0x5af8);
            s32 id = *boMember<be<s32>>(actor, 4);
            boCall(0x0253E860, gabi::at<void>(camera + 0x248), id);
            actor->mode = (u8)(actor->mode + 1);
            break;
        }
        u32 upper = gabi::ea(actor->upperMorph.get());
        *boMember<be<s16>>(actor, 0x492) = count;
        if (boCall<BOOL>(0x027F2BF8, gabi::at<void>(upper + 0x98), 0.f)) {
            monster_sound(actor, 0x4852);
            emitter = boSpawnParticleReturn(actor, 0x810c);
            if (emitter)
                boEmitterMatrix(actor, emitter, false);
        }
        s32 direction = *boMember<be<s32>>(actor, 0x3f0);
        if ((direction > 0 && stickX < 0.f) || (direction < 0 && stickX > 0.f)) {
            s16 current = *boMember<be<s16>>(actor, 0x492);
            *boMember<be<s32>>(actor, 0x3f0) = direction > 0 ? -1 : 1;
            *boMember<be<s16>>(actor, 0x492) = (s16)(current + 2);
        }
        direction = *boMember<be<s32>>(actor, 0x3f4);
        if ((direction > 0 && stickY < 0.f) || (direction < 0 && stickY > 0.f)) {
            s16 current = *boMember<be<s16>>(actor, 0x492);
            *boMember<be<s32>>(actor, 0x3f4) = direction > 0 ? -1 : 1;
            *boMember<be<s16>>(actor, 0x492) = (s16)(current + 2);
        }
        if (boCall<BOOL>(0x02007898, 0)) {
            s16 current = *boMember<be<s16>>(actor, 0x492);
            *boMember<be<s16>>(actor, 0x492) = (s16)(current + 2);
        }
        break;
    }
    case 14: {
        u32 upper = gabi::ea(actor->upperMorph.get());
        if (boCall<BOOL>(0x027F2BF8, gabi::at<void>(upper + 0x98), 9.f)) {
            s16 angle = *boMember<be<s16>>(actor, 0x464);
            gabi::store<s16>(player + 0x322, angle);
            gabi::store<u32>(companion + 0x430, 9);
            u8 away = *boMember<be<u8>>(actor, 0x3e9);
            *boMember<be<u8>>(actor, 0x3e8) = 0;
            gabi::store<u32>(companion + 0x428, away == 0 ? 1 : 0);
        }
        upper = gabi::ea(actor->upperMorph.get());
        if ((gabi::load<u8>(upper + 0xa7) & 1) || gabi::load<f32>(upper + 0x98) == 0.f) {
            u32 world = boPlay();
            u16 flags = gabi::load<u16>(world + 0x52b8);
            gabi::store<u16>(world + 0x52b8, flags | 8);
            wait_initial(actor);
            f32 random = boCall<f32>(0x020198D8, 30.f);
            s16 delay = (s16)gabi::ftoi((f32)(random + 30.f));
            *boMember<be<s16>>(actor, 0x484) = (s16)((s32)delay * 2);
        }
        break;
    }
    default:
        break;
    }
    if (actor->mode < 12 && *boMember<be<u8>>(actor, 0x3e7) == 0)
        damage_check(actor);
    if (*boMember<be<u8>>(actor, 0x3e7) == 0)
        return;
    if (*boMember<be<u16>>(actor, 0xf8) != 2) {
        boCall(0x025D7B24, actor, 2, 0xff6f, 0);
        u16 conditions = *boMember<be<u16>>(actor, 0xfa);
        *boMember<be<u16>>(actor, 0xfa) = conditions | 2;
        return;
    }
    if (actor->action != 1) {
        u32 world = boPlay();
        u16 flags = gabi::load<u16>(world + 0x52b8);
        gabi::store<u16>(world + 0x52b8, flags | 8);
        *boMember<be<u8>>(actor, 0x3e7) = 0;
        return;
    }
    actor->mode = 12;
    u32 secondInitialized;
    if (gabi::load<u32>(0x1046253c))
        secondInitialized = gabi::load<u32>(0x10462540);
    else {
        gabi::store<u32>(0x1046253c, 1);
        gabi::store<f32>(0x10462558, 40.f);
        secondInitialized = gabi::load<u32>(0x10462540);
        gabi::store<f32>(0x1046255c, 10.f);
        gabi::store<f32>(0x10462560, 280.f);
    }
    if (!secondInitialized) {
        gabi::store<u32>(0x10462540, 1);
        gabi::store<f32>(0x10462564, 0.f);
        gabi::store<f32>(0x10462568, 50.f);
        gabi::store<f32>(0x1046256c, 50.f);
    }
    u32 world = boPlay();
    u32 camera = gabi::load<u32>(world + 0x5af8);
    s32 id = *boMember<be<s32>>(actor, 4);
    boCall(0x0253E70C, gabi::at<void>(camera + 0x248), 5, id, gabi::at<void>(0x1000A194),
           gabi::at<void>(0x1000A1A0), gabi::at<void>(0x1000A1AC), gabi::at<void>(0x1000A084),
           gabi::at<void>(0x1000A07C), gabi::at<void>(0x10191D30), gabi::at<void>(0x1000A08C),
           gabi::at<void>(0x10462564), gabi::at<void>(0x1000A078), gabi::at<void>(0x10462558),
           gabi::at<void>(0x1000A094), gabi::at<void>(0x10191D2C), 0);
    *boMember<be<u8>>(actor, 0x3e7) = 0;
}

// Private proposal. Returns true for the direct BE2B4 shape-angle join.
// False requires BE110: the three angle-addCalc calls, optional mode-4
// facing transition, conditional damage check, then shape-angle copy.
static bool boMoveMode5(Actor *actor) {
    s16 deathDelay = *boMember<be<s16>>(actor, 0x480);
    if (deathDelay) {
        deathDelay = (s16)(deathDelay - 1);
        *boMember<be<s16>>(actor, 0x480) = deathDelay;
        if (deathDelay == 1) {
            actor->action = 2;
            actor->mode = 20;
            return true;
        }
    }

    if (*boMember<be<s16>>(actor, 0x48e) < 16 && !(*boMember<be<u8>>(actor, 0x3e4) & 1)) {
        s16 current = *boMember<be<s16>>(actor, 0x458);
        s16 target = *boMember<be<s16>>(actor, 0x45e);
        s16 difference = boCall<s16>(0x0200FAAC, (s32)current, (s32)target);
        if (difference < 0x100) {
            target = *boMember<be<s16>>(actor, 0x45e);
            s16 adjusted = (s16)((u16)target ^ 0xff00);
            s16 count = *boMember<be<s16>>(actor, 0x48e);
            count = (s16)(count + 1);
            *boMember<be<s16>>(actor, 0x45e) = (s16)((s32)adjusted / 10);
            *boMember<be<s16>>(actor, 0x48e) = count;
            if (count >= 16) {
                u8 flags = *boMember<be<u8>>(actor, 0x3e4);
                *boMember<be<s16>>(actor, 0x45e) = 0;
                *boMember<be<s16>>(actor, 0x458) = 0;
                *boMember<be<u8>>(actor, 0x3e4) = flags | 1;
            }
        }
    }
    if (*boMember<be<s16>>(actor, 0x490) < 16 && !(*boMember<be<u8>>(actor, 0x3e4) & 2)) {
        s16 current = *boMember<be<s16>>(actor, 0x45a);
        s16 target = *boMember<be<s16>>(actor, 0x460);
        s16 difference = boCall<s16>(0x0200FAAC, (s32)current, (s32)target);
        if (difference < 0x100) {
            target = *boMember<be<s16>>(actor, 0x460);
            s16 adjusted = (s16)((u16)target ^ 0xff00);
            s16 count = *boMember<be<s16>>(actor, 0x490);
            count = (s16)(count + 1);
            *boMember<be<s16>>(actor, 0x460) = (s16)((s32)adjusted / 10);
            *boMember<be<s16>>(actor, 0x490) = count;
            if (count >= 16) {
                u8 flags = *boMember<be<u8>>(actor, 0x3e4);
                *boMember<be<s16>>(actor, 0x460) = 0;
                *boMember<be<s16>>(actor, 0x45a) = 0;
                *boMember<be<u8>>(actor, 0x3e4) = flags | 2;
            }
        }
    }
    bool atRest = *boMember<be<s16>>(actor, 0x45e) == 0;
    if (atRest)
        atRest = *boMember<be<s16>>(actor, 0x460) == 0;
    s16 count = *boMember<be<s16>>(actor, 0x492);
    if (atRest) {
        count = (s16)(count + 1);
        *boMember<be<s16>>(actor, 0x492) = count;
    }
    if (count > 30) {
        wait_initial(actor);
        *boMember<be<f32>>(actor, 0x4b4) = 1.f;
        *boMember<be<s16>>(actor, 0x492) = 0;
    }
    if (*boMember<be<u8>>(actor, 0x3ec) != 0)
        return false;
    if (*boMember<be<s16>>(actor, 0x486) != 0)
        return false;
    if (!boCall<BOOL>(0x025162A4, boMember<void>(actor, 0x9e8)))
        return false;
    void *hit = boCall<void *>(0x02516300, boMember<void>(actor, 0x9e8));
    if (!hit)
        return false;
    u32 type = *boMember<be<u32>>(hit, 0x10);

    if (type == 0x200 || type == 0x40000) {
        *boMember<be<s16>>(actor, 0xed4) = 80;
        *boMember<be<u8>>(actor, 0x3ec) = 1;
        nokezori_damage_rtn(actor);
        *boMember<be<s16>>(actor, 0x48e) = 0;
        *boMember<be<u8>>(actor, 0x3e4) = 0;
        *boMember<be<s16>>(actor, 0x490) = 0;
        boParticle(0x8109, boMember<cXyz>(actor, 0x444));
        return false;
    }
    if (type == 0x80000) {
        f32 duration = gabi::load<f32>(0x1047BAC4);
        *boMember<be<u8>>(actor, 0xb24) = 1;
        *boMember<be<u8>>(actor, 0x3ec) = 2;
        *boMember<be<s16>>(actor, 0xb1c) = (s16)gabi::ftoi((f32)(duration + 80.f));
        *boMember<be<u8>>(actor, 0x3a1) = 30;
        *boMember<be<u32>>(actor, 0x39c) = 0;
        nokezori_damage_rtn(actor);
        *boMember<be<s16>>(actor, 0x48e) = 0;
        *boMember<be<s16>>(actor, 0x490) = 0;
        *boMember<be<u8>>(actor, 0x3e4) = 0;
        boParticle(0x8109, boMember<cXyz>(actor, 0x444));
        return false;
    }
    if (type == 0x100000) {
        *boMember<be<f32>>(actor, 0xcc4) = 1.f;
        *boMember<be<u8>>(actor, 0xb1e) = 1;
        f32 duration = gabi::load<f32>(0x1047BAC4);
        *boMember<be<f32>>(actor, 0xb20) = (f32)(duration + 80.f);
        *boMember<be<u32>>(actor, 0x39c) = 0;
        u8 kind = *boMember<be<u8>>(actor, 0x3dd);
        *boMember<be<u8>>(actor, 0x3ec) = 3;
        boCall(0x025D5834, 0xd6, ((u32)kind << 8) | 1, &actor->current.pos,
               (s32)(s8)actor->current.roomNo, &actor->current.angle, &actor->scale, 0, 0);
        nokezori_damage_rtn(actor);
        *boMember<be<s16>>(actor, 0x48e) = 0;
        *boMember<be<s16>>(actor, 0x490) = 0;
        *boMember<be<u8>>(actor, 0x3e4) = 0;
        boParticle(0x8109, boMember<cXyz>(actor, 0x444));
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        return false;
    }

    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    u32 attack = *boMember<be<u32>>(actor, 0x790);
    f32 z = *boMember<be<f32>>(actor, 0xabc);
    *boMember<be<u32>>(actor, 0x790) = attack & ~1u;
    gabi::Local<cXyz> point;
    point->z = z;
    f32 y = *boMember<be<f32>>(actor, 0xab8);
    f32 x = *boMember<be<f32>>(actor, 0xab4);
    point->y = y;
    point->x = x;
    *boMember<be<u8>>(actor, 0x7ff) = 0;
    u32 status = *boMember<be<u32>>(actor, 0x39c);
    *boMember<be<u32>>(actor, 0x39c) = status & ~4u;
    *boMember<be<f32>>(actor, 0x4b4) = 1.f;
    boParticle(0x10, point.get());
    gabi::Local<cXyz> scale;
    scale->x = 2.f;
    scale->y = 2.f;
    scale->z = 2.f;
    boParticle(0xf, point.get(), gabi::at<csXyz>(player + 0x328), scale.get());
    s16 delay = gabi::load<s16>(0x1047BB12);
    actor->action = 2;
    actor->mode = 20;
    *boMember<be<s16>>(actor, 0x480) = (u8)((s32)delay + 5);
    monster_sound(actor, 0x4850);
    effect_sound(actor, 0x2828, 0x20);
    return false;
}

// BE110 common angle interpolation, preserving per-call reloads:
// boCall(0200F428, actor+464, s16 actor+46a, 1, 0x800);
// boCall(0200F428, actor+458, s16 actor+45e, 1, s16 actor+498);
// boCall(0200F428, actor+45a, s16 actor+460, 1, s16 actor+498);
// Reload mode afterward. If mode == 4, enter BE1A8 facing branch.
// Otherwise, if mode != 0 && mode != 5, call damage_check(actor).
// BE2B4 copies current angle x -> shape x, snapshots current y,
// then snapshots current z before writing shape y and shape z.

static void bo_move(Actor *actor) {
    u32 player = gabi::load<u32>(boPlay() + 0x5b2c);
    u32 companion = gabi::load<u32>(boPlay() + 0x5b2c);
    bool interpolate = true;
    switch ((u8)actor->mode) {
    case 0:
        boWake(actor);
        break;
    case 1:
        boEmerging(actor);
        break;
    case 2:
        boWaiting(actor);
        break;
    case 3:
        boStriking(actor, companion, true);
        break;
    case 4:
        boRecoilMove(actor, player);
        break;
    case 5:
        interpolate = !boMoveMode5(actor);
        break;
    case 6:
        boStriking(actor, companion, false);
        break;
    case 7:
        if (boAnimationEnded(actor))
            actor->mode = 0;
        break;
    default:
        break;
    }
    if (interpolate) {
        boApproachAngles(actor);
        bool skipDamage = false;
        if (actor->mode == 4) {
            u32 currentPlayer = gabi::load<u32>(boPlay() + 0x5b2c);
            s16 target = boCall<s16>(0x025D6894, actor, gabi::at<void>(currentPlayer));
            s16 angle = *boMember<be<s16>>(actor, 0x464);
            s16 difference = boCall<s16>(0x0200FAAC, (s32)target, (s32)angle);
            if (difference > 0x337f) {
                wait_initial(actor);
                boUnhide(actor, 0x5848, false);
                actor->mode = 7;
                skipDamage = true;
            }
        }
        if (!skipDamage && actor->mode != 0 && actor->mode != 5)
            damage_check(actor);
    }
    u16 x = actor->current.angle.x;
    u16 y = actor->current.angle.y;
    actor->shape_angle.x = x;
    u16 z = actor->current.angle.z;
    actor->shape_angle.y = y;
    actor->shape_angle.z = z;
}
static BOOL daBO_Execute(Actor *actor) {
    WWHD_FUNC(0x020BCEC8, BOOL, actor);
    if (*boMember<be<u8>>(actor, 0x3dc) == 0 &&
        boCall<BOOL>(0x020402C8, boMember<void>(actor, 0xb18))) {
        if ((s8)*boMember<be<u8>>(actor, 0xb1e)) {
            u32 morph = gabi::ea(actor->upperMorph.get());
            copy_base_matrix(gabi::load<u32>(morph + 0x90));
            boCall(0x025E55A0, actor->upperMorph.get());
        } else
            boRegisterBody(actor);
        return 1;
    }
    for (u32 index = 0; index < 5; ++index) {
        be<s16> *timer = boMember<be<s16>>(actor, 0x482 + 2 * index);
        s16 remaining = *timer;
        if (remaining)
            *timer = (s16)(remaining - 1);
    }
    switch ((u8)actor->action) {
    case 0:
        bo_move(actor);
        break;
    case 1:
        bo2_move(actor);
        break;
    case 2:
        bo3_move(actor);
        break;
    case 3:
        bo4_move(actor);
        break;
    case 4:
        bo5_move(actor);
        break;
    default:
        break;
    }
    boExecuteFinish(actor);
    return 1;
}
VERIFY(0x020BCEC8, daBO_Execute);
