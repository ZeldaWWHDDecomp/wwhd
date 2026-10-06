/* WWHD recovery fairy. */
#include "d/actor/d_a_npc_fa1.h"

void Fa1_init_up1(daNpc_Fa1_c*);
void Fa1_init_bottle_baba_move2(daNpc_Fa1_c*);
void Fa1_init_baba_up(daNpc_Fa1_c*);

void Fa1_init_bottle_baba_wait(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223311C, void, actor);
    u32 a = gabi::ea(actor);
    actor->mMode = 4;
    f32 zero = gabi::load<f32>(0x10019E64);
    gabi::store<f32>(a + 0x378, zero);
    gabi::store<f32>(a + 0x344, zero);
    gabi::store<f32>(a + 0x33C, zero);
    gabi::store<f32>(a + 0x340, zero);
    gabi::store<f32>(a + 0x370, zero);
    gabi::store<f32>(a + 0x374, zero);
    actor->mTimer = gabi::load<u16>(0x1046701C);
}
VERIFY(0x0223311C, Fa1_init_bottle_baba_wait);

void Fa1_init_bottle_appear_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233220, void, actor);
    actor->mMode = 3;
    Fa1_init_up1(actor);
    if (actor->mType == 5) gabi::call(0x0254DA38, 0x16);
}
VERIFY(0x02233220, Fa1_init_bottle_appear_move);

void Fa1_init_hover_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223326C, void, actor);
    u32 a = gabi::ea(actor);
    actor->mMode = 8;
    f32 gravity = gabi::load<f32>(0x1046702C);
    gabi::store<f32>(a + 0x374, gravity);
    f32 rise = gabi::load<f32>(0x10467028);
    f32 zero = gabi::load<f32>(0x10019E64);
    gabi::store<f32>(a + 0x370, zero);
    gabi::store<f32>(a + 0x378, -rise);
}
VERIFY(0x0223326C, Fa1_init_hover_move);

void Fa1_init_areaMove(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022332A0, void, actor);
    actor->mSubMode = 2;
    gabi::store<f32>(gabi::ea(actor) + 0x374, gabi::load<f32>(0x10019E50));
    actor->mAreaAngle = 0;
    actor->mAreaTurn = 0;
}
VERIFY(0x022332A0, Fa1_init_areaMove);

void Fa1_init_straight2(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022332C4, void, actor);
    actor->mMoveTimer = 0;
    gabi::store<f32>(gabi::ea(actor) + 0x374, gabi::load<f32>(0x10019E50));
    actor->mSubMode = 0;
}
VERIFY(0x022332C4, Fa1_init_straight2);

void Fa1_bottle_baba_wait(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022347E4, void, actor);
    if (!actor->mTimer) Fa1_init_bottle_baba_move2(actor);
}
VERIFY(0x022347E4, Fa1_bottle_baba_wait);

void Fa1_baba_down(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234CA0, void, actor);
    if (gabi::load<f32>(gabi::ea(actor) + 0x318) <
        gabi::load<f32>(gabi::ea(actor->mpFlower.get()) + 0x318)) Fa1_init_baba_up(actor);
}
VERIFY(0x02234CA0, Fa1_baba_down);

void Fa1_init_down(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022351A0, void, actor);
    actor->mSubMode = 1;
    actor->mNeckAngle = gabi::load<s16>(0x10466FF8);
    actor->mNeckStep = gabi::load<s16>(0x10466FFA);
}
VERIFY(0x022351A0, Fa1_init_down);

void Fa1_up1(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022351C4, void, actor);
    if (gabi::load<f32>(gabi::ea(actor) + 0x340) < gabi::load<f32>(0x10019E64)) Fa1_init_down(actor);
}
VERIFY(0x022351C4, Fa1_up1);

void Fa1_init_up2(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022351DC, void, actor);
    u32 a = gabi::ea(actor);
    actor->mSubMode = 2;
    s8 room = gabi::load<s8>(a + 0x326);
    gabi::store<f32>(a + 0x378, gabi::load<f32>(0x10466FD8));
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, 0x6975, gabi::at<void>(a + 0x314), nullptr, reverb);
}
VERIFY(0x022351DC, Fa1_init_up2);

void Fa1_down(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02235238, void, actor);
    if (actor->mLocalPosition.y < gabi::load<f32>(0x10466FE4)) Fa1_init_up2(actor);
}
VERIFY(0x02235238, Fa1_down);

BOOL Fa1_morfCallback_execute(Fa1MorfCallback* callback, u16 joint, void* transform) {
    WWHD_FUNC(0x02233BE8, BOOL, callback, joint, transform);
    if (joint == callback->neckJoint) gabi::store<s16>(gabi::ea(transform) + 0xC, callback->neckAngle);
    return TRUE;
}
VERIFY(0x02233BE8, Fa1_morfCallback_execute);

void Fa1_init_up1(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233154, void, actor);
    actor->mSubMode = 0;
    actor->mTimer = gabi::load<u16>(0x10466FF6);
    actor->maxFallSpeed = gabi::load<f32>(0x10466FDC);
    actor->speed.y = gabi::load<f32>(0x10466FD4);
    f32 gravity = gabi::load<f32>(0x10466FE0);
    actor->mTurnAngle = 0;
    actor->gravity = gravity;
    actor->mNeckAngle = 0;
    actor->mNeckStep = 0;
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, &actor->current.pos, difference.get(), gabi::at<void>(player + 0x314));
    f32 x = difference->x;
    f32 zero = gabi::load<f32>(0x10019E64);
    f32 z = difference->z;
    f32 y = difference->y;
    gabi::Local<cXyz> horizontal;
    horizontal->y = zero;
    actor->mLocalPosition.x = x;
    actor->mLocalPosition.y = y;
    horizontal->x = x;
    horizontal->z = z;
    actor->mLocalPosition.z = z;
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    f32 radius = gabi::call<f32>(0x028F4384, square);
    z = actor->mLocalPosition.z;
    actor->mPlayerRadius = radius;
    x = actor->mLocalPosition.x;
    s32 angle = gabi::call<s32>(0x020195B0, x, z);
    actor->current.angle.y = s16(u32(angle) - 0x4000);
}
VERIFY(0x02233154, Fa1_init_up1);

u16 Fa1_calcTimer(be<u16>* timer) {
    WWHD_FUNC(0x0223556C, u16, timer);
    u16 value = *timer;
    if (value) { value = u16(value - 1); *timer = value; }
    return value;
}
VERIFY(0x0223556C, Fa1_calcTimer);

u16 Fa1_randomU16(u16 base, u16 range) {
    WWHD_FUNC(0x0223558C, u16, base, range);
    f32 random = gabi::call<f32>(0x020198D8, f32(range));
    return u16(gabi::ftoi(gabi::fadds_ppc(f32(base), random)));
}
VERIFY(0x0223558C, Fa1_randomU16);

u8 Fa1_randomU8(u8 base, u8 range) {
    WWHD_FUNC(0x02235608, u8, base, range);
    f32 random = gabi::call<f32>(0x020198D8, f32(range));
    return u8(gabi::ftoi(gabi::fadds_ppc(f32(base), random)));
}
VERIFY(0x02235608, Fa1_randomU8);

void Fa1_setPointLightParam(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233494, void, actor);
    u32 z = gabi::load<u32>(gabi::ea(actor) + 0x398);
    u32 x = gabi::load<u32>(gabi::ea(actor) + 0x390);
    actor->mLightRed = 200;
    f32 power = gabi::load<f32>(0x10019E70);
    gabi::store<u32>(gabi::ea(actor) + 0x85C, x);
    u32 y = gabi::load<u32>(gabi::ea(actor) + 0x394);
    actor->mLightPower = power;
    actor->mLightBlue = 200;
    f32 zero = gabi::load<f32>(0x10019E64);
    actor->mLightGreen = 200;
    actor->mLightFluctuation = zero;
    gabi::store<u32>(gabi::ea(actor) + 0x864, z);
    f32 range = gabi::load<f32>(0x10019E74);
    gabi::store<u32>(gabi::ea(actor) + 0x860, y);
    actor->mLightFluctuation = gabi::call<f32>(0x0255F360, range);
}
VERIFY(0x02233494, Fa1_setPointLightParam);

Fa1MorfCallback* Fa1_morfCallback_constructor(Fa1MorfCallback* callback) {
    WWHD_FUNC(0x02232FE4, Fa1MorfCallback*, callback);
    if (!callback) callback = gabi::call<Fa1MorfCallback*>(0x0273AD10, 8);
    if (callback) {
        callback->neckJoint = 0;
        callback->vtable = 0x10019F40;
        callback->neckAngle = 0;
    }
    return callback;
}
VERIFY(0x02232FE4, Fa1_morfCallback_constructor);

void Fa1_init_normal_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022332E0, void, actor);
    f32 fall = gabi::load<f32>(0x10019E6C);
    f32 gravity = gabi::load<f32>(0x10019E68);
    actor->mMode = 0;
    f32 speed = gabi::load<f32>(0x10466FB4);
    actor->maxFallSpeed = fall;
    actor->speedF = speed;
    actor->gravity = gravity;
    u16 timer = Fa1_randomU16(gabi::load<u16>(0x10466FF0), 60);
    s8 type = actor->mType;
    actor->mTimer = timer;
    if (type == 4) Fa1_init_areaMove(actor);
    else Fa1_init_straight2(actor);
}
VERIFY(0x022332E0, Fa1_init_normal_move);

void Fa1_init_escape_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223404C, void, actor);
    actor->mMode = 2;
    actor->gravity = gabi::load<f32>(0x10466FC4);
    actor->maxFallSpeed = gabi::load<f32>(0x10466FC0);
    actor->mTimer = gabi::load<u16>(0x10466FF2);
}
VERIFY(0x0223404C, Fa1_init_escape_move);

void Fa1_init_areaOutMove(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234F80, void, actor);
    actor->mSubMode = 3;
    actor->gravity = gabi::load<f32>(0x10019E68);
    actor->mAreaAngle = 0;
    actor->mAreaTurn = 0;
}
VERIFY(0x02234F80, Fa1_init_areaOutMove);

BOOL Fa1_IsDelete(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233BE0, BOOL, actor);
    return TRUE;
}
VERIFY(0x02233BE0, Fa1_IsDelete);

void Fa1_SafeString_assureTerminated(SafeString* string) {
    WWHD_FUNC(0x02235568, void, string);
}
VERIFY(0x02235568, Fa1_SafeString_assureTerminated);

void Fa1_up2(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02235498, void, actor);
}
VERIFY(0x02235498, Fa1_up2);

void Fa1_deleteString(void* string, s32 flags) {
    WWHD_FUNC(0x02235484, void, string, flags);
    if (string && (flags & 1)) gabi::call(0x0273AF40, string);
}
VERIFY(0x02235484, Fa1_deleteString);

daNpc_Fa1_c* Fa1_constructor(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233030, daNpc_Fa1_c*, actor);
    if (!actor) actor = gabi::call<daNpc_Fa1_c*>(0x0273AD10, 0x8C0);
    if (!actor) return nullptr;
    u32 a = gabi::ea(actor);
    gabi::call(0x025A1458, actor);
    gabi::store<u32>(a + 0xB4, 0x10019DF8);
    Fa1_morfCallback_constructor(&actor->mMorfCallback);
    gabi::call(0x025A5894, gabi::at<void>(a + 0x7F0), 0, 0);
    gabi::call(0x02008E0C, gabi::at<void>(a + 0x808));
    gabi::store<u32>(a + 0x854, 0x10019DB8);
    gabi::store<u32>(a + 0x848, 0x10019DC8);
    gabi::store<u32>(a + 0x828, 0x10019DA8);
    gabi::store<u32>(a + 0x818, 0x10019D98);
    gabi::store<u32>(a + 0x808, a + 0x848);
    gabi::store<u32>(a + 0x80C, a + 0x854);
    gabi::store<u8>(a + 0x84C, 1);
    gabi::store<u8>(a + 0x84D, 0);
    gabi::store<u8>(a + 0x84E, 0);
    gabi::store<u8>(a + 0x84F, 0);
    gabi::store<u8>(a + 0x850, 0);
    gabi::store<u8>(a + 0x851, 0);
    gabi::store<u8>(a + 0x852, 0);
    gabi::store<u32>(a + 0x858, 0xF);
    actor->mLightMultiplier = gabi::load<f32>(0x10019E50);
    return actor;
}
VERIFY(0x02233030, Fa1_constructor);

void Fa1_destructor(daNpc_Fa1_c* actor, s32 flags) {
    WWHD_FUNC(0x0223549C, void, actor, flags);
    if (!actor) return;
    u32 a = gabi::ea(actor);
    gabi::store<u32>(a + 0x828, 0x10019D28);
    gabi::store<u32>(a + 0x848, 0x10019D48);
    gabi::store<u32>(a + 0x854, 0x10019D08);
    gabi::call(0x02008DAC, gabi::at<void>(a + 0x808), 0);
    gabi::call(0x02515A70, gabi::at<void>(a + 0x690), 2);
    gabi::call(0x02515860, gabi::at<void>(a + 0x654), 2);
    gabi::call(0x02018034, gabi::at<void>(a + 0x628), 2);
    gabi::store<u32>(a + 0x470, 0x10019DD8);
    gabi::store<u32>(a + 0x464, 0x10019DE8);
    gabi::call(0x024EFD9C, gabi::at<void>(a + 0x450), 0);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x0223549C, Fa1_destructor);

void* Fa1_hoverHio_constructor(void* hio) {
    WWHD_FUNC(0x02235250, void*, hio);
    if (!hio) hio = gabi::call<void*>(0x0273AD10, 0x1C);
    if (hio) {
        u32 a = gabi::ea(hio);
        gabi::store<s8>(a, -1);
        gabi::store<u32>(a + 0x18, 0x10019E20);
        for (u32 offset = 0; offset != 20; offset += 4)
            gabi::store<u32>(a + 4 + offset, gabi::load<u32>(0x10019EBC + offset));
    }
    return hio;
}
VERIFY(0x02235250, Fa1_hoverHio_constructor);

void* Fa1_areaHio_constructor(void* hio) {
    WWHD_FUNC(0x022352C4, void*, hio);
    if (!hio) hio = gabi::call<void*>(0x0273AD10, 0x28);
    if (hio) {
        u32 a = gabi::ea(hio);
        gabi::store<s8>(a, -1);
        gabi::store<u32>(a + 0x24, 0x10019E30);
        for (u32 offset = 0; offset != 32; offset += 4)
            gabi::store<u32>(a + 4 + offset, gabi::load<u32>(0x10019ED0 + offset));
    }
    return hio;
}
VERIFY(0x022352C4, Fa1_areaHio_constructor);

void* Fa1_hio_constructor(void* hio) {
    WWHD_FUNC(0x02235350, void*, hio);
    if (!hio) hio = gabi::call<void*>(0x0273AD10, 0x94);
    if (hio) {
        u32 a = gabi::ea(hio);
        gabi::store<u32>(a + 0x90, 0x10019E40);
        Fa1_areaHio_constructor(gabi::at<void>(a + 0x4C));
        Fa1_hoverHio_constructor(gabi::at<void>(a + 0x74));
        gabi::store<s8>(a, -1);
        for (u32 offset = 4; offset <= 0x48; offset += 4)
            gabi::store<u32>(a + offset, gabi::load<u32>(0x10019EEC + offset));
    }
    return hio;
}
VERIFY(0x02235350, Fa1_hio_constructor);

void Fa1_staticInitialize() {
    WWHD_FUNC(0x022353E4, void);
    gabi::store<u32>(0x10466FA8, 0);
    gabi::store<u32>(0x10466FA0, 0);
    gabi::store<u32>(0x10466FAC, 0);
    gabi::store<u32>(0x10466FA4, 0);
    gabi::call(0x028F026C, gabi::at<void>(0x101BE368));
    f32 timer = gabi::load<f32>(0x10019F38);
    f32 unused = gabi::load<f32>(0x10019F3C);
    gabi::store<f32>(0x10466F94, timer);
    gabi::store<f32>(0x10466F98, unused);
    gabi::call(0x028ED6F8, gabi::at<void>(0x10466F9C));
    gabi::call(0x028F026C, gabi::at<void>(0x101BE374));
    gabi::call(0x028EAB2C, gabi::at<void>(0x10466F9D));
    gabi::call(0x028F026C, gabi::at<void>(0x101BE380));
    Fa1_hio_constructor(gabi::at<void>(0x10466FB0));
}
VERIFY(0x022353E4, Fa1_staticInitialize);

void Fa1_init_straight(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234DC8, void, actor);
    actor->mSubMode = 0;
    u8 timer = Fa1_randomU8(60, 60);
    f32 gravity = gabi::load<f32>(0x10019E68);
    actor->mMoveTimer = timer;
    actor->gravity = gravity;
}
VERIFY(0x02234DC8, Fa1_init_straight);

void Fa1_init_turn(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234E14, void, actor);
    actor->mSubMode = 1;
    u8 timer = Fa1_randomU8(60, 60);
    f32 gravity = gabi::load<f32>(0x10019E68);
    actor->mMoveTimer = timer;
    actor->gravity = gravity;
}
VERIFY(0x02234E14, Fa1_init_turn);

void Fa1_init_baba_up(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234C3C, void, actor);
    u8 mode = actor->mMode;
    f32 rise = actor->mAreaHeight;
    actor->mSubMode = 1;
    actor->maxFallSpeed = rise;
    if (mode == 6) {
        s32 reverb = gabi::call<s32>(0x02520540, s8(actor->current.roomNo));
        gabi::call(0x025E1A40, 0x6A2D, &actor->current.pos, nullptr, reverb);
    }
}
VERIFY(0x02234C3C, Fa1_init_baba_up);

void Fa1_init_baba_down(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022346FC, void, actor);
    f32 fall = actor->mAreaRadius;
    actor->mSubMode = 0;
    f32 zero = gabi::load<f32>(0x10019E64);
    u8 mode = actor->mMode;
    actor->speed.y = zero;
    actor->maxFallSpeed = fall;
    f32 gravity = gabi::load<f32>(0x10467014);
    actor->gravity = gravity;
    if (mode == 5) {
        s32 reverb = gabi::call<s32>(0x02520540, s8(actor->current.roomNo));
        gabi::call(0x025E1A40, 0x6A2E, &actor->current.pos, nullptr, reverb);
    }
}
VERIFY(0x022346FC, Fa1_init_baba_down);

void Fa1_init_bottle_baba_move2(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234778, void, actor);
    actor->mMode = 6;
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    actor->mpFlower = gabi::at<fopAc_ac_c>(player);
    f32 speed = gabi::load<f32>(0x10467000);
    actor->mAreaAngle = 0;
    actor->speedF = speed;
    actor->mAreaRadius = gabi::load<f32>(0x10467010);
    actor->mAreaHeight = gabi::load<f32>(0x1046700C);
    Fa1_init_baba_down(actor);
}
VERIFY(0x02234778, Fa1_init_bottle_baba_move2);

void Fa1_init_bottle_baba_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022347F4, void, actor);
    actor->mMode = 5;
    gabi::call(0x025D5578, 0x14F, &actor->mpFlower);
    u32 flower = gabi::ea(actor->mpFlower.get());
    actor->mTimer = gabi::load<u16>(0x1046701A);
    f32 speed = gabi::load<f32>(0x10467000);
    actor->mAreaAngle = 1;
    actor->speedF = speed;
    actor->mAreaRadius = gabi::load<f32>(0x10467008);
    actor->mAreaHeight = gabi::load<f32>(0x10467004);
    if (flower) {
        f32 x = gabi::load<f32>(flower + 0x314);
        f32 offset = gabi::load<f32>(0x10019EB0);
        x = gabi::fadds_ppc(x, offset);
        flower = gabi::ea(actor->mpFlower.get());
        actor->current.pos.x = x;
        actor->current.pos.z = gabi::load<f32>(flower + 0x31C);
    }
    Fa1_init_baba_down(actor);
}
VERIFY(0x022347F4, Fa1_init_bottle_baba_move);

void Fa1_straight(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234E60, void, actor);
    u32 a = gabi::ea(actor);
    gabi::store<f32>(a + 0x330, gabi::load<f32>(0x10466FBC));
    if (!gabi::call<u8>(0x0207A9A0, &actor->mMoveTimer)) {
        gabi::Local<cXyz> difference;
        gabi::call(0x0201ADE0, &actor->current.pos, difference.get(), gabi::at<void>(a + 0x2EC));
        gabi::Local<cXyz> horizontal;
        horizontal->x = f32(difference->x);
        horizontal->y = gabi::load<f32>(0x10019E64);
        horizontal->z = f32(difference->z);
        f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
        f32 radius = gabi::call<f32>(0x028F4384, square);
        if (radius > gabi::load<f32>(a + 0x330)) Fa1_init_turn(actor);
    }
}
VERIFY(0x02234E60, Fa1_straight);

void Fa1_turn(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234EF0, void, actor);
    if (gabi::call<u8>(0x0207A9A0, &actor->mMoveTimer)) {
        gabi::Local<cXyz> difference;
        gabi::call(0x0201ADE0, &actor->current.pos, difference.get(), gabi::at<void>(gabi::ea(actor) + 0x2EC));
        f32 x = -f32(difference->x);
        f32 z = -f32(difference->z);
        s32 angle = gabi::call<s32>(0x020195B0, x, z);
        gabi::call(0x0200F378, &actor->current.angle.y, angle, 0x20, 0x800, 1);
    } else Fa1_init_straight(actor);
}
VERIFY(0x02234EF0, Fa1_turn);

void Fa1_position_move(daNpc_Fa1_c* actor, f32 height, f32 verticalSpeed) {
    WWHD_FUNC(0x02233C04, void, actor, height, verticalSpeed);
    u32 a = gabi::ea(actor);
    f32 difference = gabi::fsubs_ppc(f32(actor->current.pos.y), gabi::load<f32>(a + 0x2F0));
    if (difference > height) { verticalSpeed = -verticalSpeed; actor->maxFallSpeed = verticalSpeed; }
    else if (difference < -height) actor->maxFallSpeed = verticalSpeed;
    else verticalSpeed = actor->maxFallSpeed;
    f32 zero = gabi::load<f32>(0x10019E64);
    f32 gravity = actor->gravity;
    u16 angle = gabi::load<u16>(a + 0x322);
    f32 speed = actor->speed.y;
    if (verticalSpeed < zero) {
        speed = gabi::fsubs_ppc(speed, gravity);
        f32 limit = actor->maxFallSpeed;
        actor->speed.y = speed;
        if (speed < limit) actor->speed.y = limit;
    } else {
        speed = gabi::fadds_ppc(speed, gravity);
        f32 limit = actor->maxFallSpeed;
        actor->speed.y = speed;
        if (speed > limit) actor->speed.y = limit;
    }
    u32 table = 0x104A44F8 + ((u32(angle) >> 3) << 3);
    f32 forward = actor->speedF;
    actor->speed.x = gabi::fmuls_ppc(forward, gabi::load<f32>(table));
    actor->speed.z = gabi::fmuls_ppc(forward, gabi::load<f32>(table + 4));
    gabi::call(0x025D6800, actor, nullptr);
}
VERIFY(0x02233C04, Fa1_position_move);

BOOL Fa1_delete(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233984, BOOL, actor);
    gabi::call(0x025A5AC8, gabi::at<void>(gabi::ea(actor) + 0x7F0));
    if (actor->mType == 3) gabi::call(0x0255BA9C, &actor->mLightPosition);
    u32 count = gabi::load<u32>(0x10466F90);
    if (count) {
        count -= 1;
        gabi::store<u32>(0x10466F90, count);
        if (s32(count) > 0) return TRUE;
    }
    s8 hio = gabi::load<s8>(0x10466FB0);
    if (hio >= 0) {
        gabi::call(0x025F0A18, hio);
        gabi::store<s8>(0x10466FB0, -1);
    }
    return TRUE;
}
VERIFY(0x02233984, Fa1_delete);

BOOL Fa1_Delete(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233A08, BOOL, actor);
    return Fa1_delete(actor);
}
VERIFY(0x02233A08, Fa1_Delete);

BOOL Fa1_draw(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233B58, BOOL, actor);
    u32 morf = gabi::ea(actor->mpFairyMorf.get());
    u32 model = gabi::load<u32>(morf + 0x90);
    void* environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4, environment, 0, &actor->current.pos, gabi::at<void>(gabi::ea(actor) + 0x110));
    environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C, environment, gabi::at<void>(model), gabi::at<void>(gabi::ea(actor) + 0x110));
    gabi::call(0x025E5590, actor->mpFairyMorf.get());
    f32 scale = gabi::load<f32>(0x10019E50);
    gabi::call(0x025BED80, 0x84, actor, scale, scale, scale);
    return TRUE;
}
VERIFY(0x02233B58, Fa1_draw);

BOOL Fa1_Draw(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233BDC, BOOL, actor);
    return Fa1_draw(actor);
}
VERIFY(0x02233BDC, Fa1_Draw);

void Fa1_BGCheck(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233CD8, void, actor);
    u32 a = gabi::ea(actor);
    u32 z = gabi::load<u32>(a + 0x31C);
    u32 y = gabi::load<u32>(a + 0x318);
    gabi::store<u32>(a + 0x834, z);
    u32 x = gabi::load<u32>(a + 0x314);
    gabi::store<u32>(a + 0x830, y);
    gabi::store<u32>(a + 0x82C, x);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    f32 ground = gabi::call<f32>(0x02008974, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x808));
    f32 invalid = gabi::load<f32>(0x10019E80);
    actor->mGroundY = ground;
    if (ground == invalid) {
        f32 zero = gabi::load<f32>(0x10019E64);
        f32 one = gabi::load<f32>(0x10019E50);
        actor->mMoveTarget.x = zero;
        actor->mMoveTarget.y = one;
        actor->mMoveTarget.z = zero;
        return;
    }
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    u16 index = gabi::load<u16>(a + 0x81C);
    u16 background = gabi::load<u16>(a + 0x81E);
    u32 normal = gabi::ea(gabi::call<void*>(0x020084C8, gabi::at<void>(play + 0x12A0), background, index));
    gabi::store<u32>(a + 0x890, gabi::load<u32>(normal));
    gabi::store<u32>(a + 0x894, gabi::load<u32>(normal + 4));
    gabi::store<u32>(a + 0x898, gabi::load<u32>(normal + 8));
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    u8 room = gabi::call<u8>(0x024EF130, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x81C));
    gabi::store<u8>(a + 0x1C9, room);
    actor->current.roomNo = s8(room);
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    u8 color = gabi::call<u8>(0x024EEEB8, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x81C));
    gabi::store<u8>(a + 0x1CA, color);
}
VERIFY(0x02233CD8, Fa1_BGCheck);

void Fa1_findPlayer(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233DC4, void, actor);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, gabi::at<void>(player + 0x314), difference.get(), &actor->current.pos);
    s32 direction = gabi::call<s32>(0x020195B0, f32(difference->x), f32(difference->z));
    s16 target = s16(u32(direction) - u32(s32(s16(actor->current.angle.y))));
    s16 neck = actor->mMorfCallback.neckAngle;
    if (target < 0) {
        if (target < -0x4000) target = -0x4000;
        s16 step = s16(s16(target - neck) / 4);
        if (step < -0x800) step = -0x800;
        neck = s16(neck + step);
        if (neck < target) neck = target;
    } else {
        if (target > 0x4000) target = 0x4000;
        s16 step = s16(s16(target - neck) / 4);
        if (step > 0x800) step = 0x800;
        neck = s16(neck + step);
        if (neck > target) neck = target;
    }
    actor->mMorfCallback.neckAngle = neck;
}
VERIFY(0x02233DC4, Fa1_findPlayer);

BOOL Fa1_CreateHeap(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02232EA0, BOOL, actor);
    gabi::Local<SafeString> modelKey;
    modelKey->mStringTop = 0x10019E54;
    u32 manager = gabi::load<u32>(0x101F4F28);
    modelKey->__vtbl = 0x10019CF0;
    void* data = gabi::call<void*>(0x026066C4, gabi::at<void>(manager), modelKey.get(), 0x1F);
    manager = gabi::load<u32>(0x101F4F28);
    gabi::Local<SafeString> animationKey;
    animationKey->mStringTop = 0x10019E54;
    animationKey->__vtbl = 0x10019CF0;
    void* animation = gabi::call<void*>(0x026066C4, gabi::at<void>(manager), animationKey.get(), 0xC);
    f32 rate = gabi::load<f32>(0x10019E50);
    void* morf = gabi::call<void*>(0x025E4F64, nullptr, data, &actor->mMorfCallback, nullptr,
                                 animation, 2, 0, -1, 0, 0, 0, 0x11020203, rate);
    actor->mpFairyMorf = morf;
    if (!morf) return FALSE;
    if (!gabi::load<u32>(gabi::ea(morf) + 0x90)) {
        actor->mpFairyMorf = nullptr;
        return FALSE;
    }
    u32 names = gabi::ea(gabi::call<void*>(0x027F68FC, data));
    u32 displacement = gabi::load<u32>(names + 0x10);
    void* table = gabi::at<void>(displacement ? names + 0x10 + displacement : 0);
    s32 joint = gabi::call<s32>(0x027DF9B0, table, STR(0x10019E5C));
    actor->mMorfCallback.neckJoint = u16(joint);
    actor->mMorfCallback.neckAngle = 0;
    return TRUE;
}
VERIFY(0x02232EA0, Fa1_CreateHeap);

BOOL Fa1_heapCallback(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02232FE0, BOOL, actor);
    return Fa1_CreateHeap(actor);
}
VERIFY(0x02232FE0, Fa1_heapCallback);

void Fa1_setMtx(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233378, void, actor);
    u32 a = gabi::ea(actor);
    u32 morf = gabi::ea(actor->mpFairyMorf.get());
    f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
    u32 model = gabi::load<u32>(morf + 0x90);
    gabi::call(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
    gabi::call(0x025F1C28, gabi::at<void>(0x1048D0CC), gabi::load<s16>(a + 0x32A));
    // Capture the complete source matrix before writing the model destination.
    f32 matrix[12];
    for (u32 i = 0; i != 12; ++i) matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
    const u8 order[12] = {1,9,10,8,7,5,2,11,3,0,6,4};
    for (u8 i : order) gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
    gabi::call(0x025E55A0, actor->mpFairyMorf.get());
    morf = gabi::ea(actor->mpFairyMorf.get());
    u16 joint = actor->mMorfCallback.neckJoint;
    model = gabi::load<u32>(morf + 0x90);
    u32 matrices = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(matrices + 4);
    u32 table = gabi::load<u32>(matrices + 0x10);
    u32 selected = table + u32(joint) * 48;
    gabi::store<u16>(matrices + 4, flags | 0x10);
    x = gabi::load<f32>(selected + 0xC);
    y = gabi::load<f32>(selected + 0x1C);
    z = gabi::load<f32>(selected + 0x2C);
    gabi::store<f32>(a + 0x394, y);
    gabi::store<f32>(a + 0x384, z);
    gabi::store<f32>(a + 0x390, x);
    gabi::store<f32>(a + 0x398, z);
    gabi::store<f32>(a + 0x37C, x);
    gabi::store<f32>(a + 0x380, y);
}
VERIFY(0x02233378, Fa1_setMtx);

void Fa1_init_get_player_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233EA8, void, actor);
    actor->mMode = 1;
    u16 timer = gabi::load<u16>(0x10466FEE);
    f32 radius = gabi::load<f32>(0x10019E7C);
    actor->mTimer = timer;
    s16 turn = gabi::load<s16>(0x10466FE8);
    actor->mMorfCallback.neckAngle = 0;
    actor->mTurnAngle = turn;
    actor->mPlayerRadius = radius;
    actor->current.angle.y = 0;
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    s8 room = actor->current.roomNo;
    actor->current.pos.y = gabi::load<f32>(player + 0x318);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, 0x695D, &actor->current.pos, nullptr, reverb);
}
VERIFY(0x02233EA8, Fa1_init_get_player_move);

void Fa1_hover_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223447C, void, actor);
    f32 speed = gabi::load<f32>(0x10467028);
    f32 height = gabi::load<f32>(0x10467030);
    Fa1_position_move(actor, height, speed);
    Fa1_BGCheck(actor);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    s32 direction = gabi::call<s32>(0x025D6894, actor, gabi::at<void>(player));
    gabi::call(0x0200F378, &actor->current.angle.y, direction, 8, 0x2000, 0x400);
}
VERIFY(0x0223447C, Fa1_hover_move);

void Fa1_init_bigelf_change(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234CB8, void, actor);
    u32 a = gabi::ea(actor);
    u32 flags = gabi::load<u32>(a + 0x2E0);
    u16 angle = actor->current.angle.y;
    actor->mMode = 7;
    gabi::store<u32>(a + 0x2E0, flags | 0x4000);
    f32 speed = gabi::load<f32>(0x10467034);
    actor->speedF = speed;
    u32 table = 0x104A44F8 + ((u32(angle) >> 3) << 3);
    actor->speed.x = gabi::fmuls_ppc(speed, gabi::load<f32>(table));
    actor->speed.z = gabi::fmuls_ppc(speed, gabi::load<f32>(table + 4));
    gabi::call(0x028E8D88, &actor->current.pos, &actor->speed, &actor->current.pos);
}
VERIFY(0x02234CB8, Fa1_init_bigelf_change);

void Fa1_bigelf_change(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234D1C, void, actor);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, gabi::at<void>(gabi::ea(actor) + 0x2EC), difference.get(), &actor->current.pos);
    s32 direction = gabi::call<s32>(0x020195B0, f32(difference->x), f32(difference->z));
    s16 turn = gabi::load<s16>(0x10467038);
    u16 angle = u16(u32(direction) + u32(s32(turn)));
    f32 speed = actor->speedF;
    actor->current.angle.y = s16(angle);
    u32 table = 0x104A44F8 + ((u32(angle) >> 3) << 3);
    actor->speed.x = gabi::fmuls_ppc(speed, gabi::load<f32>(table));
    actor->speed.z = gabi::fmuls_ppc(speed, gabi::load<f32>(table + 4));
    gabi::call(0x028E8D88, &actor->current.pos, &actor->speed, &actor->current.pos);
    Fa1_BGCheck(actor);
}
VERIFY(0x02234D1C, Fa1_bigelf_change);

BOOL Fa1_checkBinCatch(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233F40, BOOL, actor);
    u32 a = gabi::ea(actor);
    if (gabi::load<u16>(a + 0xF8) == 6) {
        gabi::call(0x025D57E0, actor);
        return TRUE;
    }
    s16 timer = gabi::load<s16>(0x10466FF4);
    f32 radius = gabi::load<f32>(0x10466FC8);
    f32 lower = gabi::load<f32>(0x10466FD0);
    f32 upper = gabi::load<f32>(0x10466FCC);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    gabi::call(0x024EBB70, gabi::at<void>(play + 0x5934), actor, 0x57, timer, 1, radius, upper, lower);
    u16 condition = gabi::load<u16>(a + 0xFA);
    gabi::store<u16>(a + 0xFA, condition | 0x40);
    return FALSE;
}
VERIFY(0x02233F40, Fa1_checkBinCatch);

static f32 Fa1_horizontalRadius(const cXyz* vector) {
    gabi::Local<cXyz> horizontal;
    horizontal->x = f32(vector->x);
    horizontal->y = gabi::load<f32>(0x10019E64);
    horizontal->z = f32(vector->z);
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    return gabi::call<f32>(0x028F4384, square);
}

static void Fa1_areaTurn(daNpc_Fa1_c* actor, s32 angle) {
    if (!gabi::call<s16>(0x02055B64, &actor->mAreaAngle)) {
        actor->mAreaTurn = s16(u16(actor->mAreaTurn) ^ 1);
        u32 timer = gabi::call<u32>(0x021E1E78, 15, 20);
        actor->mAreaAngle = s16(u8(timer));
    }
    s32 turn = actor->mAreaTurn == 0 ? 0x2000 : -0x2000;
    gabi::call(0x0200F378, &actor->current.angle.y, s16(u32(angle) + u32(turn)), 0x20, 0x800, 1);
}

void Fa1_areaMove(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234FA4, void, actor);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, &actor->current.pos, difference.get(), gabi::at<void>(gabi::ea(actor) + 0x2EC));
    f32 radius = Fa1_horizontalRadius(difference.get());
    if (radius > gabi::load<f32>(gabi::ea(actor) + 0x330)) { Fa1_init_areaOutMove(actor); return; }
    s16 direction = actor->current.angle.y;
    Fa1_areaTurn(actor, direction);
}
VERIFY(0x02234FA4, Fa1_areaMove);

void Fa1_areaOutMove(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223509C, void, actor);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, &actor->current.pos, difference.get(), gabi::at<void>(gabi::ea(actor) + 0x2EC));
    f32 radius = Fa1_horizontalRadius(difference.get());
    if (radius < gabi::load<f32>(gabi::ea(actor) + 0x330)) { Fa1_init_areaMove(actor); return; }
    s32 direction = gabi::call<s32>(0x0200F93C, &actor->current.pos, gabi::at<void>(gabi::ea(actor) + 0x2EC));
    Fa1_areaTurn(actor, direction);
}
VERIFY(0x0223509C, Fa1_areaOutMove);

static void Fa1_dispatch(daNpc_Fa1_c* actor, u32 descriptor) {
    s16 index = gabi::load<s16>(descriptor + 2);
    s16 adjustment = gabi::load<s16>(descriptor);
    u32 receiver = gabi::ea(actor) + u32(s32(adjustment));
    u32 target;
    if (index < 0) target = gabi::load<u32>(descriptor + 4);
    else {
        s16 offset = gabi::load<s16>(descriptor + 6);
        u32 table = gabi::load<u32>(receiver + u32(s32(offset)));
        target = gabi::load<u32>(table + u32(s32(index)) * 8 + 4);
    }
    gabi::call(target, gabi::at<void>(receiver));
}

BOOL Fa1_createInit(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223350C, BOOL, actor);
    u32 a = gabi::ea(actor);
    f32 zero = gabi::load<f32>(0x10019E64);
    u32 parameters = gabi::load<u32>(a + 0xB0);
    actor->mLocalPosition.y = zero;
    actor->mLocalPosition.x = zero;
    s8 type = s8(parameters);
    actor->mLocalPosition.z = zero;
    actor->mStatus = 0;
    actor->mpFlower = nullptr;
    actor->mType = type;
    bool bottle = type == 3 || type == 2 || type == 5;
    bool hover = type == 6;
    if (type == 3) Fa1_init_bottle_baba_wait(actor);
    else if (type == 2 || type == 5) Fa1_init_bottle_appear_move(actor);
    else if (type == 6) Fa1_init_hover_move(actor);
    else {
        if (type == 4) {
            u32 flags = gabi::load<u32>(a + 0x2E0);
            s8 reloadedType = actor->mType;
            f32 radius = gabi::load<f32>(0x10019E78);
            gabi::store<u32>(a + 0x2E0, flags & ~0x80u);
            gabi::store<f32>(a + 0x330, radius);
            type = reloadedType;
        } else {
            f32 radius = gabi::load<f32>(0x10466FBC);
            type = actor->mType;
            gabi::store<f32>(a + 0x330, radius);
        }
        if (type == 1) {
            u32 flags = gabi::load<u32>(a + 0x2E0);
            actor->mStatus = 1;
            gabi::store<u32>(a + 0x2E0, flags & ~0x80u);
            u16 random = Fa1_randomU16(0, 255);
            s16 angle = actor->current.angle.y;
            actor->current.angle.y = s16(u32(s32(angle)) + (u32(random) << 8));
        } else actor->mType = 0;
        Fa1_init_normal_move(actor);
    }
    if (bottle || hover) {
        u32 flags = gabi::load<u32>(a + 0x2E0);
        gabi::store<u32>(a + 0x2E0, (flags & ~0x80u) | (hover ? 0 : 0x4000));
    }
    f32 z = actor->current.pos.z;
    f32 y = actor->current.pos.y;
    f32 height = gabi::load<f32>(0x10019E7C);
    f32 x = actor->current.pos.x;
    y = gabi::fadds_ppc(y, height);
    gabi::store<f32>(a + 0x834, z);
    gabi::store<f32>(a + 0x82C, x);
    gabi::store<f32>(a + 0x830, y);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    f32 ground = gabi::call<f32>(0x02008974, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x808));
    type = actor->mType;
    actor->mGroundY = ground;
    if (type != 6 && !(ground == gabi::load<f32>(0x10019E80)))
        gabi::store<f32>(a + 0x2F0, gabi::fadds_ppc(ground, gabi::load<f32>(0x10019E84)));
    gabi::call(0x02515F14, gabi::at<void>(a + 0x654), 255, 255, actor);
    gabi::store<u32>(a + 0x6D4, a + 0x654);
    gabi::call(0x02516518, gabi::at<void>(a + 0x690), gabi::at<void>(0x101BE2DC));
    Fa1_setMtx(actor);
    if (actor->mType == 3) {
        Fa1_setPointLightParam(actor);
        gabi::call(0x0255B9C8, &actor->mLightPosition);
    }
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    actor->mpEmitter = gabi::call<void*>(0x025A847C, gabi::at<void>(particles), 0, 0x52,
          &actor->current.pos, 0, 0, 255, gabi::at<void>(a + 0x7F0), -1, 0, 0, 0);
    return TRUE;
}
VERIFY(0x0223350C, Fa1_createInit);

s32 Fa1_create(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233854, s32, actor);
    u32 a = gabi::ea(actor);
    u32 condition = gabi::load<u32>(a + 0x2E4);
    if (!(condition & 8)) {
        if (actor) { Fa1_constructor(actor); condition = gabi::load<u32>(a + 0x2E4); }
        gabi::store<u32>(a + 0x2E4, condition | 8);
    }
    if (!gabi::call<BOOL>(0x025D63E8, actor, 0x02232FE0, 0x1100)) return 5;
    u32 morf = gabi::ea(actor->mpFairyMorf.get());
    u32 model = gabi::load<u32>(morf + 0x90);
    gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
    if (gabi::load<s8>(0x10466FB0) < 0) {
        s32 id = gabi::call<s32>(0x025F0A10, STR(0x10019E88), gabi::at<void>(0x10466FB0));
        gabi::store<s8>(0x10466FB0, s8(id));
        gabi::store<u32>(0x10466F90, 1);
    } else gabi::store<u32>(0x10466F90, gabi::load<u32>(0x10466F90) + 1);
    return Fa1_createInit(actor) ? 4 : 5;
}
VERIFY(0x02233854, Fa1_create);

s32 Fa1_Create(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233980, s32, actor);
    return Fa1_create(actor);
}
VERIFY(0x02233980, Fa1_Create);

BOOL Fa1_execute(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233A0C, BOOL, actor);
    Fa1_calcTimer(&actor->mTimer);
    u8 mode = actor->mMode;
    Fa1_dispatch(actor, 0x101BE320 + u32(mode) * 8);
    s16 angle = actor->current.angle.y;
    void* morf = actor->mpFairyMorf.get();
    gabi::store<s16>(gabi::ea(actor) + 0x32A, angle);
    gabi::call(0x025E535C, morf, 0, 0, 0);
    Fa1_setMtx(actor);
    if (actor->mType == 3) Fa1_setPointLightParam(actor);
    if (actor->mMode == 1) {
        u32 emitter = gabi::ea(actor->mpEmitter.get());
        if (emitter) gabi::store<f32>(emitter + 0x34, gabi::load<f32>(0x10019E50));
    }
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    f32 radius = gabi::load<f32>(0x10019E94);
    f32 upper = gabi::load<f32>(0x10019E98);
    f32 lower = gabi::load<f32>(0x10019E9C);
    gabi::call(0x024EBD80, gabi::at<void>(play + 0x594C), actor, 0x6000, 1, radius, upper, lower);
    return TRUE;
}
VERIFY(0x02233A0C, Fa1_execute);

BOOL Fa1_Execute(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02233B54, BOOL, actor);
    return Fa1_execute(actor);
}
VERIFY(0x02233B54, Fa1_Execute);

static BOOL Fa1_checkCollision(daNpc_Fa1_c* actor) {
    u32 a = gabi::ea(actor);
    gabi::call(0x020182E0, gabi::at<void>(a + 0x7A8), &actor->current.pos);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), gabi::at<void>(a + 0x690));
    return gabi::call<BOOL>(0x02516464, gabi::at<void>(a + 0x690));
}

void Fa1_normal_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234078, void, actor);
    u8 submode = actor->mSubMode;
    actor->speedF = gabi::load<f32>(0x10466FB4);
    Fa1_dispatch(actor, 0x101BE274 + u32(submode) * 8);
    f32 height = gabi::load<f32>(0x10019EA0);
    f32 speed = gabi::load<f32>(0x10019E50);
    Fa1_position_move(actor, height, speed);
    Fa1_BGCheck(actor);
    Fa1_findPlayer(actor);
    if (Fa1_checkCollision(actor)) { Fa1_init_get_player_move(actor); return; }
    if (!Fa1_checkBinCatch(actor) && (u8(actor->mStatus) & 1) && !actor->mTimer) Fa1_init_escape_move(actor);
}
VERIFY(0x02234078, Fa1_normal_move);

void Fa1_escape_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x0223434C, void, actor);
    f32 gravity = actor->gravity;
    f32 speed = gabi::fadds_ppc(f32(actor->speed.y), gravity);
    f32 limit = actor->maxFallSpeed;
    actor->speed.y = speed;
    u16 angle = actor->current.angle.y;
    if (speed > limit) actor->speed.y = limit;
    u32 table = 0x104A44F8 + ((u32(angle) >> 3) << 3);
    f32 forward = actor->speedF;
    actor->speed.x = gabi::fmuls_ppc(forward, gabi::load<f32>(table));
    actor->speed.z = gabi::fmuls_ppc(forward, gabi::load<f32>(table + 4));
    gabi::call(0x025D6800, actor, nullptr);
    Fa1_BGCheck(actor);
    s16 neck = actor->mMorfCallback.neckAngle;
    if (neck > 0) { neck = s16(neck - 0x800); if (neck < 0) neck = 0; }
    else if (neck < 0) { neck = s16(neck + 0x800); if (neck > 0) neck = 0; }
    actor->mMorfCallback.neckAngle = neck;
    if (Fa1_checkCollision(actor)) Fa1_init_get_player_move(actor);
    else if (!actor->mTimer) gabi::call(0x025D57E0, actor);
}
VERIFY(0x0223434C, Fa1_escape_move);

static void Fa1_speedAnimation(daNpc_Fa1_c* actor) {
    u32 morf = gabi::ea(actor->mpFairyMorf.get());
    f32 increment = gabi::load<f32>(0x10019EA4);
    f32 rate = gabi::fadds_ppc(gabi::load<f32>(morf + 0x98), increment);
    f32 maximum = gabi::load<f32>(0x10019EA8);
    if (rate > maximum) rate = maximum;
    gabi::store<f32>(morf + 0x98, rate);
}

static void Fa1_copyPosition(daNpc_Fa1_c* actor, const cXyz* position) {
    u32 p = gabi::ea(position);
    u32 y = gabi::load<u32>(p + 4);
    u32 x = gabi::load<u32>(p);
    u32 z = gabi::load<u32>(p + 8);
    u32 a = gabi::ea(actor);
    gabi::store<u32>(a + 0x314, x);
    gabi::store<u32>(a + 0x31C, z);
    gabi::store<u32>(a + 0x318, y);
}

void Fa1_get_player_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234204, void, actor);
    Fa1_speedAnimation(actor);
    s16 turn = actor->mTurnAngle;
    s16 angle = s16(u16(actor->current.angle.y) - u16(turn));
    actor->current.angle.y = angle;
    s16 acceleration = gabi::load<s16>(0x10466FEC);
    s16 limit = gabi::load<s16>(0x10466FEA);
    turn = s16(turn + acceleration);
    if (turn > limit) turn = limit;
    actor->mTurnAngle = turn;
    u16 orbitAngle = u16(u16(angle) + 0x4000);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 table = 0x104A44F8 + ((u32(orbitAngle) >> 3) << 3);
    f32 radius = actor->mPlayerRadius;
    f32 x = gabi::fmuls_ppc(radius, gabi::load<f32>(table));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    actor->mLocalPosition.x = x;
    f32 z = gabi::fmuls_ppc(radius, gabi::load<f32>(table + 4));
    f32 y = actor->mLocalPosition.y;
    actor->mLocalPosition.z = z;
    actor->mLocalPosition.y = gabi::fadds_ppc(y, gabi::load<f32>(0x10466FB8));
    gabi::Local<cXyz> position;
    gabi::call(0x0201AD78, gabi::at<void>(player + 0x314), position.get(), &actor->mLocalPosition);
    Fa1_copyPosition(actor, position.get());
    Fa1_BGCheck(actor);
    if (!actor->mTimer) {
        gabi::call(0x0254DA38, 0x16);
        gabi::call(0x025D57E0, actor);
    }
}
VERIFY(0x02234204, Fa1_get_player_move);

void Fa1_bottle_appear_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x022344EC, void, actor);
    Fa1_speedAnimation(actor);
    s16 angle = actor->current.angle.y;
    s16 turn = actor->mTurnAngle;
    s16 acceleration = actor->mNeckStep;
    s16 limit = actor->mNeckAngle;
    angle = s16(angle - turn);
    turn = s16(turn + acceleration);
    actor->current.angle.y = angle;
    actor->mTurnAngle = turn;
    if (turn > limit) actor->mTurnAngle = limit;
    u16 orbitAngle = u16(u16(angle) + 0x4000);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    f32 radius = actor->mPlayerRadius;
    f32 vertical = actor->speed.y;
    u32 table = 0x104A44F8 + ((u32(orbitAngle) >> 3) << 3);
    f32 x = gabi::fmuls_ppc(radius, gabi::load<f32>(table));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    f32 y = gabi::fadds_ppc(f32(actor->mLocalPosition.y), vertical);
    actor->mLocalPosition.x = x;
    f32 z = gabi::fmuls_ppc(radius, gabi::load<f32>(table + 4));
    actor->mLocalPosition.y = y;
    actor->mLocalPosition.z = z;
    gabi::Local<cXyz> position;
    gabi::call(0x0201AD78, gabi::at<void>(player + 0x314), position.get(), &actor->mLocalPosition);
    Fa1_copyPosition(actor, position.get());
    f32 zero = gabi::load<f32>(0x10019E64);
    f32 fall = actor->maxFallSpeed;
    f32 speed = actor->speed.y;
    f32 gravity = actor->gravity;
    if (fall < zero) {
        speed = gabi::fsubs_ppc(speed, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = speed;
        if (speed < fall) actor->speed.y = fall;
    } else {
        speed = gabi::fadds_ppc(speed, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = speed;
        if (speed > fall) actor->speed.y = fall;
    }
    radius = gabi::fadds_ppc(f32(actor->mPlayerRadius), gabi::load<f32>(0x10019EAC));
    f32 maximum = gabi::load<f32>(0x10019E7C);
    if (radius > maximum) radius = maximum;
    actor->mPlayerRadius = radius;
    Fa1_BGCheck(actor);
    Fa1_dispatch(actor, 0x101BE294 + u32(u8(actor->mSubMode)) * 8);
    if (!actor->mTimer) {
        if (actor->mType != 5) gabi::call(0x0254DA38, 0x16);
        gabi::call(0x025D57E0, actor);
    }
}
VERIFY(0x022344EC, Fa1_bottle_appear_move);

static void Fa1_babaPosition(daNpc_Fa1_c* actor) {
    u32 flower = gabi::ea(actor->mpFlower.get());
    f32 forward = gabi::load<f32>(0x10467000);
    actor->speedF = forward;
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, gabi::at<void>(flower + 0x314), difference.get(), &actor->current.pos);
    s32 direction = gabi::call<s32>(0x020195B0, f32(difference->x), f32(difference->z));
    s16 turn = gabi::load<s16>(0x10467018);
    forward = actor->speedF;
    u16 angle = u16(u32(direction) + u32(s32(turn)));
    actor->current.angle.y = s16(angle);
    u16 rereadAngle = actor->current.angle.y;
    u32 sine = 0x104A44F8 + ((u32(angle) >> 3) << 3);
    u32 cosine = 0x104A44F8 + ((u32(rereadAngle) >> 3) << 3);
    f32 fall = actor->maxFallSpeed;
    f32 x = gabi::fmuls_ppc(forward, gabi::load<f32>(sine));
    f32 zero = gabi::load<f32>(0x10019E64);
    actor->speed.x = x;
    f32 cos = gabi::load<f32>(cosine + 4);
    f32 vertical = actor->speed.y;
    f32 z = gabi::fmuls_ppc(forward, cos);
    f32 gravity = actor->gravity;
    actor->speed.z = z;
    if (fall < zero) {
        vertical = gabi::fsubs_ppc(vertical, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = vertical;
        if (vertical < fall) actor->speed.y = fall;
    } else {
        vertical = gabi::fadds_ppc(vertical, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = vertical;
        if (vertical > fall) actor->speed.y = fall;
    }
    gabi::call(0x028E8D88, &actor->current.pos, &actor->speed, &actor->current.pos);
    Fa1_BGCheck(actor);
}

void Fa1_bottle_baba_move(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234894, void, actor);
    if (!actor->mpFlower.get()) {
        Fa1_init_bottle_baba_move(actor);
        Fa1_BGCheck(actor);
        return;
    }
    Fa1_babaPosition(actor);
    u8 submode = actor->mSubMode;
    Fa1_dispatch(actor, 0x101BE2AC + u32(submode) * 8);
    if (submode == 1) {
        u32 flower = gabi::ea(actor->mpFlower.get());
        f32 height = gabi::load<f32>(0x10019EB4);
        f32 top = gabi::fadds_ppc(gabi::load<f32>(flower + 0x318), height);
        if (actor->current.pos.y > top) gabi::call(0x025D57E0, actor);
    }
}
VERIFY(0x02234894, Fa1_bottle_baba_move);

void Fa1_bottle_baba_move2(daNpc_Fa1_c* actor) {
    WWHD_FUNC(0x02234A60, void, actor);
    if (!actor->mpFlower.get()) {
        Fa1_init_bottle_baba_move2(actor);
        Fa1_BGCheck(actor);
        return;
    }
    Fa1_babaPosition(actor);
    Fa1_dispatch(actor, 0x101BE2AC + u32(u8(actor->mSubMode)) * 8);
    u32 flower = gabi::ea(actor->mpFlower.get());
    f32 height = gabi::load<f32>(0x10019EB8);
    f32 top = gabi::fadds_ppc(gabi::load<f32>(flower + 0x318), height);
    if (actor->current.pos.y > top) Fa1_init_bottle_baba_move(actor);
}
VERIFY(0x02234A60, Fa1_bottle_baba_move2);
