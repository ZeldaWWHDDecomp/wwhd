/**
 * d_a_npc_btsw2.cpp (WWHD)
 * NPC - traveling merchant Btsw2.
 *
 * Written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_btsw2.h"

static u32 address(daNpc_Btsw2_c* actor) { return gabi::ea(actor); }

void daNpc_Btsw2_checkOrder(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CFBC, void, actor);
    u32 a = address(actor);
    if (gabi::load<u16>(a + 0xF8) != 1) return;
    s8 state = gabi::load<s8>(a + 0x8B6);
    if (state == 1 || state == 2) {
        gabi::store<s8>(a + 0x8B6, 0);
        gabi::store<u8>(a + 0x892, 1);
    }
}
VERIFY(0x0221CFBC, daNpc_Btsw2_checkOrder);

void daNpc_Btsw2_eventOrder(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CFFC, void, actor);
    u32 a = address(actor);
    s8 state = gabi::load<s8>(a + 0x8B6);
    if (state == 1 || state == 2) {
        s8 orderedState = gabi::load<s8>(a + 0x8B6);
        u16 condition = gabi::load<u16>(a + 0xFA);
        gabi::store<u16>(a + 0xFA, condition | 1);
        if (orderedState == 1) gabi::call(0x025D76A8, actor);
    }
}
VERIFY(0x0221CFFC, daNpc_Btsw2_eventOrder);

u32 daNpc_Btsw2_getMsg(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D6B4, u32, actor);
    u32 save = gabi::load<u32>(0x101F84DC);
    if (!gabi::call<BOOL>(0x025B8B94, gabi::at<void>(save + 0x644), 0x3102)) {
        save = gabi::load<u32>(0x101F84DC);
        gabi::call(0x025B8B68, gabi::at<void>(save + 0x644), 0x3102);
        return 0x1AB0;
    }
    return gabi::call<BOOL>(0x02556D14) ? 0x1AB2 : 0x1AB1;
}
VERIFY(0x0221D6B4, daNpc_Btsw2_getMsg);

u16 daNpc_Btsw2_nextMsgStatus(daNpc_Btsw2_c* actor, u32* message) {
    WWHD_FUNC(0x0221D724, u16, actor, message);
    return 0x10;
}
VERIFY(0x0221D724, daNpc_Btsw2_nextMsgStatus);

void daNpc_Btsw2_setAttention(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221DAB0, void, actor);
    u32 a = address(actor);
    f32 z = gabi::load<f32>(a + 0x880);
    f32 y = gabi::load<f32>(a + 0x87C);
    f32 height = gabi::load<f32>(0x10466AD4);
    f32 x = gabi::load<f32>(a + 0x878);
    gabi::store<f32>(a + 0x398, z);
    gabi::store<f32>(a + 0x390, x);
    gabi::store<f32>(a + 0x394, gabi::fadds_ppc(y, height));
}
VERIFY(0x0221DAB0, daNpc_Btsw2_setAttention);

void daNpc_Btsw2_wait01(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221DC90, void, actor);
    u32 a = address(actor);
    if (gabi::load<u8>(a + 0x892)) gabi::store<s8>(a + 0x8B7, 2);
    else gabi::store<s8>(a + 0x8B6, 2);
}
VERIFY(0x0221DC90, daNpc_Btsw2_wait01);

BOOL daNpc_Btsw2_delete(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CE9C, BOOL, actor);
    u32 a = address(actor);
    gabi::call(0x025204C8, gabi::at<void>(a + 0x7DC), STR(0x10018BB0));
    if (gabi::load<u32>(a + 0xF4)) {
        u32 morf = gabi::load<u32>(a + 0x44C);
        if (morf) gabi::call(0x025E563C, gabi::at<void>(morf));
    }
    return TRUE;
}
VERIFY(0x0221CE9C, daNpc_Btsw2_delete);

BOOL daNpc_Btsw2_Delete(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CEF4, BOOL, actor);
    return daNpc_Btsw2_delete(actor);
}
VERIFY(0x0221CEF4, daNpc_Btsw2_Delete);

BOOL daNpc_Btsw2_IsDelete(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221DFE4, BOOL, actor);
    return TRUE;
}
VERIFY(0x0221DFE4, daNpc_Btsw2_IsDelete);

void daNpc_Btsw2_SafeString_assureTerminated(SafeString* string) {
    WWHD_FUNC(0x0221E088, void, string);
}
VERIFY(0x0221E088, daNpc_Btsw2_SafeString_assureTerminated);

void daNpc_Btsw2_SafeString_delete(void* object, int flags) {
    WWHD_FUNC(0x0221DFD0, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x0221DFD0, daNpc_Btsw2_SafeString_delete);

void daNpc_Btsw2_destructor(daNpc_Btsw2_c* actor, int flags) {
    WWHD_FUNC(0x0221DFEC, void, actor, flags);
    if (!actor) return;
    u32 a = address(actor);
    gabi::call(0x02515A70, gabi::at<void>(a + 0x690), 2);
    gabi::call(0x02515860, gabi::at<void>(a + 0x654), 2);
    gabi::call(0x02018034, gabi::at<void>(a + 0x628), 2);
    gabi::store<u32>(a + 0x470, 0x10018A18);
    gabi::store<u32>(a + 0x464, 0x10018A28);
    gabi::call(0x024EFD9C, gabi::at<void>(a + 0x450), 0);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x0221DFEC, daNpc_Btsw2_destructor);

void daNpc_Btsw2_setAnm(daNpc_Btsw2_c* actor, s8 animation) {
    WWHD_FUNC(0x0221D410, void, actor, animation);
    s8 current = actor->mAnimation;
    if (animation == current || current == -1) return;
    s8 mode = actor->mMode;
    actor->mAnimation = animation;
    u32 animationOffset = u32(s32(animation)) * 4;
    u32 modeOffset = u32(s32(mode)) * 4;
    u32 morf = gabi::ea(actor->mpMorf.get());
    f32 morph = gabi::load<f32>(0x101BD850 + modeOffset);
    s32 resource = gabi::load<s32>(0x10018A48 + animationOffset);
    f32 speed = gabi::load<f32>(0x101BD87C + modeOffset);
    s32 loop = gabi::load<s32>(0x101BD824 + modeOffset);
    gabi::call<BOOL>(0x0259D454, gabi::at<void>(morf), loop, morph, speed,
                    resource, -1, STR(0x10018BB0));
    current = actor->mAnimation;
    gabi::store<u8>(address(actor) + 0x3B7, current == 7 || current == 8);
}
VERIFY(0x0221D410, daNpc_Btsw2_setAnm);

void daNpc_Btsw2_talk01(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221DCB4, void, actor);
    if (gabi::call<u16>(0x025A11EC, actor, 1) != 0x12) return;
    actor->mMode = 1;
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u16 flags = gabi::load<u16>(play + 0x52B8);
    gabi::store<u16>(play + 0x52B8, flags | 8);
    s16 waitTimer = actor->mWaitTimer;
    actor->mTalkAccepted = 0;
    if (waitTimer == 0) actor->mPathAdvance = 1;
    daNpc_Btsw2_setAnm(actor, 0);
}
VERIFY(0x0221DCB4, daNpc_Btsw2_talk01);

void daNpc_Btsw2_anmAtr(daNpc_Btsw2_c* actor, u16 messageStatus) {
    WWHD_FUNC(0x0221D5FC, void, actor, messageStatus);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u8 requested = gabi::load<u8>(play + 0x5BC5);
    if (requested <= 6) daNpc_Btsw2_setAnm(actor, requested);
    u32 morf = gabi::ea(actor->mpMorf.get());
    s16 end = gabi::load<s16>(morf + 0xA2);
    f32 frame = f32(end) - 1.0f;
    if (gabi::call<BOOL>(0x027F2BF8, gabi::at<void>(morf + 0x98), frame)) {
        s8 animation = actor->mAnimation;
        if (animation == 4 || animation == 6) daNpc_Btsw2_setAnm(actor, 1);
    }
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    gabi::store<u8>(play + 0x5BC5, 255);
}
VERIFY(0x0221D5FC, daNpc_Btsw2_anmAtr);

bool daNpc_Btsw2_chkAttention(daNpc_Btsw2_c* actor, cXyz* position, s16 facing) {
    WWHD_FUNC(0x0221D4D0, bool, actor, position, facing);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    f32 z = position->z;
    f32 playerZ = gabi::load<f32>(player + 0x31C);
    f32 playerX = gabi::load<f32>(player + 0x314);
    f32 dz = gabi::fsubs_ppc(playerZ, z);
    f32 x = position->x;
    f32 range = gabi::load<f32>(0x10466ADC);
    f32 dx = gabi::fsubs_ppc(playerX, x);
    f32 squareZ = gabi::fmuls_ppc(dz, dz);
    f32 square = gabi::fmadds(dx, dx, squareZ);
    s32 angleRange = gabi::load<s16>(0x10466AD8);
    f32 distance = gabi::call<f32>(0x028F4384, square);
    s16 angle = gabi::call<s16>(0x020195B0, dx, dz);
    if (actor->mHasAttention) {
        range = gabi::fadds_ppc(range, 40.0f);
        angleRange += 0x71C;
    }
    s32 delta = s16(u16(angle) - u16(facing));
    s32 absolute = delta < 0 ? -delta : delta;
    return angleRange > absolute && range > distance;
}
VERIFY(0x0221D4D0, daNpc_Btsw2_chkAttention);

BOOL daNpc_Btsw2_create(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CD78, BOOL, actor);
    u32 a = address(actor);
    u32 condition = gabi::load<u32>(a + 0x2E4);
    if (!(condition & 8)) {
        if (actor) {
            gabi::call(0x025A1458, actor);
            gabi::store<u32>(a + 0xB4, 0x10018BB8);
            gabi::call(0x025E7820, gabi::at<void>(a + 0x7F0));
            condition = gabi::load<u32>(a + 0x2E4);
        }
        gabi::store<u32>(a + 0x2E4, condition | 8);
    }
    u32 save = gabi::load<u32>(0x101F84DC);
    if (gabi::call<u8>(0x025B8BB0, gabi::at<void>(save + 0x644), 0xC203) == 3) return 5;
    if (!gabi::call<BOOL>(0x0254DA50, 0x6A, 1)) return 5;
    s32 phase = gabi::call<s32>(0x02520460, gabi::at<void>(a + 0x7DC), STR(0x10018BB0));
    if (phase != 4) return phase;
    if (!gabi::call<BOOL>(0x025D63E8, actor, gabi::at<void>(0x0221CAE8), 0x29E0)) return 5;
    u32 morf = gabi::load<u32>(a + 0x44C);
    u32 model = gabi::load<u32>(morf + 0x90);
    gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
    if (!gabi::call<BOOL>(0x0221CAEC, actor)) return 5;
    return phase;
}
VERIFY(0x0221CD78, daNpc_Btsw2_create);

BOOL daNpc_Btsw2_Create(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CE98, BOOL, actor);
    return daNpc_Btsw2_create(actor);
}
VERIFY(0x0221CE98, daNpc_Btsw2_Create);

BOOL daNpc_Btsw2_CreateHeapCallback(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CAE8, BOOL, actor);
    return gabi::call<BOOL>(0x0221C790, actor);
}
VERIFY(0x0221CAE8, daNpc_Btsw2_CreateHeapCallback);

void* daNpc_Btsw2_HIO_construct(void* object) {
    WWHD_FUNC(0x0221DE30, void*, object);
    if (!object) object = gabi::call<void*>(0x0273AD10, 0x40);
    if (!object) return nullptr;
    u32 a = gabi::ea(object);
    gabi::store<u32>(a + 0x3C, 0x10018A38);
    gabi::call(0x0259DA18, gabi::at<void>(a + 4));
    gabi::store<s16>(a + 0x12, 0);
    f32 zero = gabi::load<f32>(0x10018A74);
    f32 damping = gabi::load<f32>(0x10018BA4);
    f32 speed = gabi::load<f32>(0x10018BA0);
    gabi::store<s8>(a, -1);
    gabi::store<s16>(a + 0xA, 0);
    f32 attentionHeight = gabi::load<f32>(0x10018B88);
    gabi::store<s16>(a + 0x18, 1000);
    gabi::store<s16>(a + 0xC, 8000);
    gabi::store<u8>(a + 0x22, 0);
    gabi::store<s16>(a + 0x14, -8000);
    gabi::store<f32>(a + 0x1C, attentionHeight);
    gabi::store<f32>(a + 0x30, speed);
    f32 attentionDistance = gabi::load<f32>(0x10018B9C);
    gabi::store<s16>(a + 0x10, -3000);
    gabi::store<s16>(a + 0x2E, 5);
    gabi::store<f32>(a + 0x34, damping);
    gabi::store<f32>(a + 4, zero);
    gabi::store<s16>(a + 0xE, 8000);
    gabi::store<f32>(a + 0x24, attentionDistance);
    gabi::store<s16>(a + 0x3A, 90);
    gabi::store<s16>(a + 0x2C, 600);
    gabi::store<s16>(a + 0x20, 0x2000);
    gabi::store<s16>(a + 0x1A, 1600);
    gabi::store<s16>(a + 0x38, 90);
    gabi::store<s16>(a + 0x16, -8000);
    gabi::store<s16>(a + 8, 8000);
    return object;
}
VERIFY(0x0221DE30, daNpc_Btsw2_HIO_construct);

void daNpc_Btsw2_staticInit() {
    WWHD_FUNC(0x0221DF30, void);
    gabi::store<u32>(0x10466B00, 0);
    gabi::store<u32>(0x10466AF8, 0);
    gabi::store<u32>(0x10466B04, 0);
    gabi::store<u32>(0x10466AFC, 0);
    gabi::call(0x028F026C, gabi::at<void>(0x101BD8A8));
    f32 minimum = gabi::load<f32>(0x10018BA8);
    f32 maximum = gabi::load<f32>(0x10018BAC);
    gabi::store<f32>(0x10466AAC, minimum);
    gabi::store<f32>(0x10466AB0, maximum);
    gabi::call(0x028ED6F8, gabi::at<void>(0x10466AB4));
    gabi::call(0x028F026C, gabi::at<void>(0x101BD8B4));
    gabi::call(0x028EAB2C, gabi::at<void>(0x10466AB5));
    gabi::call(0x028F026C, gabi::at<void>(0x101BD8C0));
    daNpc_Btsw2_HIO_construct(gabi::at<void>(0x10466AB8));
}
VERIFY(0x0221DF30, daNpc_Btsw2_staticInit);

BOOL daNpc_Btsw2_draw(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D214, BOOL, actor);
    u32 a = address(actor);
    u32 morf = gabi::ea(actor->mpMorf.get());
    u32 model = gabi::load<u32>(morf + 0x90);
    u32 modelData = gabi::load<u32>(model + 0xAC);
    void* environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x025626A4, environment, 0, gabi::at<void>(a + 0x314), gabi::at<void>(a + 0x110));
    environment = gabi::call<void*>(0x02555D0C);
    gabi::call(0x02562F5C, environment, gabi::at<void>(model), gabi::at<void>(a + 0x110));
    environment = gabi::call<void*>(0x02555D0C);
    u32 bag = gabi::ea(actor->mpBagModel.get());
    gabi::call(0x02562F5C, environment, gabi::at<void>(bag), gabi::at<void>(a + 0x110));
    environment = gabi::call<void*>(0x02555D0C);
    u32 flyer = gabi::ea(actor->mpFlyerModel.get());
    gabi::call(0x02562F5C, environment, gabi::at<void>(flyer), gabi::at<void>(a + 0x110));
    u8 frame = actor->mBtpFrame;
    gabi::call(0x025E7B3C, gabi::at<void>(a + 0x7F0), gabi::at<void>(modelData), frame);
    morf = gabi::ea(actor->mpMorf.get());
    gabi::call(0x025E54D8, gabi::at<void>(morf));
    s8 joint = actor->mHandLeftJoint;
    u32 matrices = gabi::load<u32>(model + 0x2C);
    u32 matrixArray = gabi::load<u32>(matrices + 0x10);
    u16 flags = gabi::load<u16>(matrices + 4);
    bag = gabi::ea(actor->mpBagModel.get());
    gabi::store<u16>(matrices + 4, flags | 0x10);
    mtx_copy(gabi::at<Mtx34>(bag + 0xC8), gabi::at<Mtx34>(matrixArray + u32(s32(joint) * 48)));
    joint = actor->mHandRightJoint;
    matrices = gabi::load<u32>(model + 0x2C);
    matrixArray = gabi::load<u32>(matrices + 0x10);
    flags = gabi::load<u16>(matrices + 4);
    flyer = gabi::ea(actor->mpFlyerModel.get());
    gabi::store<u16>(matrices + 4, flags | 0x10);
    mtx_copy(gabi::at<Mtx34>(flyer + 0xC8), gabi::at<Mtx34>(matrixArray + u32(s32(joint) * 48)));
    bag = gabi::ea(actor->mpBagModel.get());
    gabi::call(0x025E2DE0, gabi::at<void>(bag), 0);
    flyer = gabi::ea(actor->mpFlyerModel.get());
    gabi::call(0x025E2DE0, gabi::at<void>(flyer), 0);
    gabi::store<u32>(modelData + 0x38, 0);
    s16 rotation = gabi::load<s16>(a + 0x322);
    gabi::call(0x025BEBB8, 0x98, actor, gabi::at<void>(a + 0x314), rotation, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0221D214, daNpc_Btsw2_draw);

BOOL daNpc_Btsw2_Draw(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D40C, BOOL, actor);
    return daNpc_Btsw2_draw(actor);
}
VERIFY(0x0221D40C, daNpc_Btsw2_Draw);

BOOL daNpc_Btsw2_execute(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D034, BOOL, actor);
    u32 a = address(actor);
    s16 neckMin = gabi::load<s16>(0x10466AC8);
    s16 headMax = gabi::load<s16>(0x10466AC0);
    s16 backMin = gabi::load<s16>(0x10466AC6);
    s16 backMax = gabi::load<s16>(0x10466ACE);
    s16 headMin = gabi::load<s16>(0x10466AC4);
    s16 neckMax = gabi::load<s16>(0x10466ACA);
    s16 turnMin = gabi::load<s16>(0x10466ACC);
    s16 turnMax = gabi::load<s16>(0x10466AC2);
    s16 turnStep = gabi::load<s16>(0x10466AD0);
    gabi::call(0x0259E08C, gabi::at<void>(a + 0x3AC), turnMax, backMin, neckMax,
               backMax, headMax, headMin, neckMin, turnMin, turnStep);
    gabi::call(0x0221CEF8, actor);
    u32 morf = gabi::ea(actor->mpMorf.get());
    gabi::call(0x025E535C, gabi::at<void>(morf), gabi::at<void>(a + 0x37C), 0, 0);
    daNpc_Btsw2_checkOrder(actor);
    if (!gabi::call<BOOL>(0x0259F858, gabi::at<void>(a + 0x3E0))) {
        s16 index = actor->mActionVtableIndex;
        s16 adjustment = actor->mActionThisAdjustment;
        u32 receiver = a + s32(adjustment);
        u32 target;
        if (index < 0) target = actor->mActionTarget;
        else {
            s16 vtableOffset = gabi::load<s16>(a + 0x8A2);
            u32 vtable = gabi::load<u32>(receiver + s32(vtableOffset));
            target = gabi::load<u32>(vtable + u32(s32(index) * 8) + 4);
        }
        gabi::call<BOOL>(target, gabi::at<void>(receiver), 0);
    }
    daNpc_Btsw2_eventOrder(actor);
    u32 play = gabi::ea(gabi::call<void*>(0x025200D4));
    gabi::call(0x024F08A8, gabi::at<void>(a + 0x450), gabi::at<void>(play + 0x12A0));
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    s8 room = gabi::call<s8>(0x024EF130, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x538));
    gabi::store<s8>(a + 0x1C9, room);
    play = gabi::ea(gabi::call<void*>(0x025200D4));
    u8 color = gabi::call<u8>(0x024EEEB8, gabi::at<void>(play + 0x12A0), gabi::at<void>(a + 0x538));
    morf = gabi::ea(actor->mpMorf.get());
    f32 x = gabi::load<f32>(a + 0x314);
    f32 y = gabi::load<f32>(a + 0x318);
    f32 z = gabi::load<f32>(a + 0x31C);
    gabi::store<u8>(a + 0x1CA, color);
    u32 model = gabi::load<u32>(morf + 0x90);
    gabi::call(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
    s16 rotation = gabi::load<s16>(a + 0x322);
    gabi::call(0x025F1C28, gabi::at<void>(0x1048D0CC), rotation);
    mtx_copy(gabi::at<Mtx34>(model + 0xC8), gabi::at<Mtx34>(0x1048D0CC));
    gabi::call(0x025A15AC, actor, 60.0f, 150.0f);
    return TRUE;
}
VERIFY(0x0221D034, daNpc_Btsw2_execute);

BOOL daNpc_Btsw2_Execute(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D210, BOOL, actor);
    return daNpc_Btsw2_execute(actor);
}
VERIFY(0x0221D210, daNpc_Btsw2_Execute);

void daNpc_Btsw2_playTexPattern(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CEF8, void, actor);
    u32 a = address(actor);
    if (gabi::call<s16>(0x02055B64, gabi::at<void>(a + 0x868))) return;
    u32 pattern = gabi::ea(actor->mpTexPattern.get());
    u32 vtable = gabi::load<u32>(pattern + 4);
    u32 target = gabi::load<u32>(vtable + 0x14);
    s32 frames = gabi::call<s32>(target, gabi::at<void>(pattern));
    u8 frame = actor->mBtpFrame;
    if (s32(frame) < frames) {
        frame = actor->mBtpFrame;
        actor->mBtpFrame = u8(frame + 1);
        return;
    }
    pattern = gabi::ea(actor->mpTexPattern.get());
    vtable = gabi::load<u32>(pattern + 4);
    target = gabi::load<u32>(vtable + 0x14);
    frames = gabi::call<s32>(target, gabi::at<void>(pattern));
    frame = actor->mBtpFrame;
    actor->mBtpFrame = u8(u32(frame) - u32(frames));
    f32 random = gabi::call<f32>(0x020198D8, 100.0f);
    actor->mBlinkTimer = s16(gabi::ftoi(gabi::fadds_ppc(random, 30.0f)));
}
VERIFY(0x0221CEF8, daNpc_Btsw2_playTexPattern);

void daNpc_Btsw2_lookBack(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221DAD8, void, actor);
    u32 a = address(actor);
    f32 zero = gabi::load<f32>(0x10018A74);
    f32 x = zero, y = zero, z = zero;
    int headOnly = 1;
    s8 mode = actor->mMode;
    s16 facing = gabi::load<s16>(a + 0x322);
    gabi::Local<cXyz> eye;
    cXyz* target = nullptr;
    if (mode == 1 || mode == 2) {
        bool attention;
        if (mode == 2) {
            headOnly = 0;
            gabi::store<u8>(a + 0x3B6, 1);
            attention = actor->mHasAttention;
            if (!attention) {
                gabi::Local<cXyz> playerEye;
                f32 offset = gabi::load<f32>(0x10466ABC);
                gabi::call(0x0259D54C, playerEye.get(), offset);
                eye->copy(*playerEye.get());
                s16 desired = gabi::call<s16>(0x0200F93C, gabi::at<cXyz>(a + 0x314), eye.get());
                gabi::call(0x0200F428, gabi::at<void>(a + 0x322), desired, 4, 0x1800);
                attention = actor->mHasAttention;
            }
        } else attention = actor->mHasAttention;
        if (attention) {
            gabi::Local<cXyz> playerEye;
            f32 offset = gabi::load<f32>(0x10466ABC);
            gabi::call(0x0259D54C, playerEye.get(), offset);
            y = gabi::load<f32>(a + 0x380);
            eye->copy(*playerEye.get());
            x = gabi::load<f32>(a + 0x314);
            z = gabi::load<f32>(a + 0x31C);
            target = eye.get();
        }
    }
    gabi::Local<cXyz> selfEye;
    selfEye->x = x; selfEye->y = y; selfEye->z = z;
    s16 speed = gabi::load<s16>(0x10466AD2);
    gabi::call(0x0259DED0, gabi::at<void>(a + 0x3AC), gabi::at<void>(a + 0x322),
               target, selfEye.get(), facing, speed, headOnly);
}
VERIFY(0x0221DAD8, daNpc_Btsw2_lookBack);

BOOL daNpc_Btsw2_waitAction(daNpc_Btsw2_c* actor, void* argument) {
    WWHD_FUNC(0x0221DD2C, BOOL, actor, argument);
    s8 status = actor->mActionStatus;
    if (status == 0) {
        u8 current = gabi::load<u8>(address(actor) + 0x8BA);
        actor->mMode = 1;
        actor->mActionStatus = s8(u8(current + 1));
        return TRUE;
    }
    if (status == -1) return TRUE;
    u32 a = address(actor);
    s16 facing = gabi::load<s16>(a + 0x322);
    s16 head = gabi::load<s16>(a + 0x3B2);
    f32 x = gabi::load<f32>(a + 0x314);
    f32 y = gabi::load<f32>(a + 0x318);
    s16 neck = gabi::load<s16>(a + 0x3AE);
    f32 z = gabi::load<f32>(a + 0x31C);
    gabi::Local<cXyz> position;
    position->x = x; position->y = y; position->z = z;
    u8 attention = gabi::call<u8>(0x0221D4D0, actor, position.get(), s16(u16(facing) + u16(neck) + u16(head)));
    s8 mode = actor->mMode;
    actor->mHasAttention = attention;
    actor->mEventOrder = 0;
    if (mode == 1) {
        daNpc_Btsw2_wait01(actor);
        // GHS keeps r3 across the void wait01 leaf.
        gabi::call(0x0221D72C, actor);
    } else if (mode == 2) daNpc_Btsw2_talk01(actor);
    daNpc_Btsw2_lookBack(actor);
    daNpc_Btsw2_setAttention(actor);
    return TRUE;
}
VERIFY(0x0221DD2C, daNpc_Btsw2_waitAction);

BOOL daNpc_Btsw2_createInit(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221CAEC, BOOL, actor);
    u32 a = address(actor);
    u16 z = gabi::load<u16>(a + 0x324);
    u16 x = gabi::load<u16>(a + 0x320);
    f32 gravity = gabi::load<f32>(0x10018B3C);
    u16 y = gabi::load<u16>(a + 0x322);
    actor->mLookRotation.x = s16(x); actor->mLookRotation.y = s16(y);
    s16 index = actor->mActionVtableIndex;
    gabi::store<u32>(a + 0x39C, 10);
    gabi::store<f32>(a + 0x374, gravity);
    actor->mLookRotation.z = s16(z);
    bool same = index == -1 && actor->mActionThisAdjustment == 0 && actor->mActionTarget == 0x0221DD2C;
    if (!same) {
        if (index != 0) {
            s16 adjustment = actor->mActionThisAdjustment;
            index = actor->mActionVtableIndex;
            u32 receiver = a + s32(adjustment);
            actor->mActionStatus = -1;
            u32 target;
            if (index < 0) target = actor->mActionTarget;
            else {
                s16 offset = gabi::load<s16>(a + 0x8A2);
                u32 vtable = gabi::load<u32>(receiver + s32(offset));
                target = gabi::load<u32>(vtable + u32(s32(index) * 8) + 4);
            }
            gabi::call<BOOL>(target, gabi::at<void>(receiver), 0);
        }
        actor->mActionThisAdjustment = 0;
        s16 adjustment = actor->mActionThisAdjustment;
        actor->mActionTarget = 0x0221DD2C;
        actor->mActionStatus = 0;
        actor->mActionVtableIndex = -1;
        u32 target = actor->mActionTarget;
        gabi::call<BOOL>(target, gabi::at<void>(a + s32(adjustment)), 0);
    }
    u32 px = gabi::load<u32>(a + 0x314);
    u32 py = gabi::load<u32>(a + 0x318);
    gabi::store<u32>(a + 0x878, px);
    u32 pz = gabi::load<u32>(a + 0x31C);
    gabi::store<u32>(a + 0x87C, py);
    gabi::store<u32>(a + 0x880, pz);
    gabi::call(0x02515F14, gabi::at<void>(a + 0x654), 255, 255, actor);
    gabi::call(0x02516518, gabi::at<void>(a + 0x690), gabi::at<void>(0x101BD7E0));
    gabi::store<u32>(a + 0x6D4, a + 0x654);
    gabi::call(0x025A15AC, actor, 60.0f, 150.0f);
    u32 parameters = gabi::load<u32>(a + 0xB0);
    s8 room = gabi::load<s8>(a + 0x326);
    gabi::store<u8>(a + 0x898, 0);
    s8 pathNumber = s8(parameters >> 16);
    actor->mPathNumber = pathNumber;
    void* path = gabi::call<void*>(0x025AAF88, pathNumber, room);
    actor->mpPath = path;
    if (!path) {
        gabi::call(0x0273AA24, STR(0x10018B5C), 0x2DE, STR(0x10018B70));
        path = actor->mpPath.get();
    }
    u16 count = gabi::load<u16>(gabi::ea(path));
    actor->mFinalPathPoint = s16(count - 1);
    f32 random = gabi::call<f32>(0x020198D8, 3.0f);
    actor->mPathTimer = s16(gabi::ftoi(gabi::fadds_ppc(random, 1.0f)));
    random = gabi::call<f32>(0x020198D8, 300.0f);
    f32 timer = gabi::fadds_ppc(random, 90.0f);
    actor->mPathFlags = 0; actor->mPathAdvance = 0;
    actor->mWaitTimer = s16(gabi::ftoi(timer));
    actor->mPathPoint = 0;
    gabi::call(0x0259F814, gabi::at<void>(a + 0x3E0), STR(0x10018B54), actor);
    gabi::store<u8>(a + 0x389, 0xAB);
    gabi::store<u8>(a + 0x38B, 0xAB);
    return TRUE;
}
VERIFY(0x0221CAEC, daNpc_Btsw2_createInit);

BOOL daNpc_Btsw2_initTexPattern(daNpc_Btsw2_c* actor, u8 modify) {
    WWHD_FUNC(0x0221C68C, BOOL, actor, modify);
    u32 a = address(actor);
    u32 morf = gabi::ea(actor->mpMorf.get());
    u32 model = gabi::load<u32>(morf + 0x90);
    u32 resource = gabi::load<u32>(0x100189FC);
    u32 modelData = gabi::load<u32>(model + 0xAC);
    u32 manager = gabi::load<u32>(0x101F4F28);
    gabi::Local<SafeString> key;
    key->mStringTop = 0x10018BB0; key->__vtbl = 0x10018A00;
    void* pattern = gabi::call<void*>(0x026066C4, gabi::at<void>(manager), key.get(), resource);
    actor->mpTexPattern = pattern;
    if (!pattern) {
        gabi::call(0x0273AA24, STR(0x10018A84), 0x11A, STR(0x10018A98));
        pattern = actor->mpTexPattern.get();
    }
    if (!gabi::call<BOOL>(0x025E789C, gabi::at<void>(a + 0x7F0), gabi::at<void>(modelData),
            pattern, 1, 2, 0, -1, modify, 1.0f)) return FALSE;
    actor->mBtpFrame = 0;
    actor->mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x0221C68C, daNpc_Btsw2_initTexPattern);

static void* btsw2_resource(u32 archive, u32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = archive; key->__vtbl = 0x10018A00;
    u32 manager = gabi::load<u32>(0x101F4F28);
    return gabi::call<void*>(0x026066C4, gabi::at<void>(manager), key.get(), id);
}
static s32 btsw2_joint(void* modelData, u32 name) {
    void* jointNames = gabi::call<void*>(0x027F68FC, modelData);
    u32 names = gabi::ea(jointNames);
    u32 displacement = gabi::load<u32>(names + 0x10);
    return gabi::call<s32>(0x027DF9B0, gabi::at<void>(displacement ? names + 0x10 + displacement : 0), STR(name));
}
static void btsw2_setCallback(u32 data, s8 joint) {
    u32 index = u16(s16(joint));
    u32 count = gabi::load<u32>(data + 4);
    u32 array = gabi::load<u32>(data + 8);
    u32 selected = index < count ? array + index * 0x1C : array;
    gabi::store<u32>(selected + 8, 0x0221C39C);
}
BOOL daNpc_Btsw2_createHeap(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221C790, BOOL, actor);
    u32 a = address(actor);
    void* modelData = btsw2_resource(0x10018BB0, 0x1A);
    if (!modelData) gabi::call(0x0273AA24, STR(0x10018ACC), 0x269, STR(0x10018AE0));
    void* animation = btsw2_resource(0x10018BB0, 0xF);
    void* morf = gabi::call<void*>(0x025E4F64, nullptr, modelData, nullptr, nullptr,
                                 animation, 2, 0, -1, 1, 0, 0x80000, 0x15020022, 1.0f);
    actor->mpMorf = static_cast<mDoExt_McaMorf*>(morf);
    if (!morf || !gabi::load<u32>(gabi::ea(morf) + 0x90)) return FALSE;
    s8 head = s8(btsw2_joint(modelData, 0x10018ABC));
    gabi::store<s8>(a + 0x3B4, head);
    if (head < 0) gabi::call(0x0273AA24, STR(0x10018ACC), 0x27C, STR(0x10018AF4));
    s8 back = s8(btsw2_joint(modelData, 0x10018B10));
    gabi::store<s8>(a + 0x3B5, back);
    if (back < 0) gabi::call(0x0273AA24, STR(0x10018ACC), 0x281, STR(0x10018B1C));
    actor->mHandLeftJoint = s8(btsw2_joint(modelData, 0x10018AAC));
    actor->mHandRightJoint = s8(btsw2_joint(modelData, 0x10018AB4));
    void* bagData = btsw2_resource(0x10018AC4, 0x14);
    void* bag = gabi::call<void*>(0x025E38E0, bagData, 0, 0x11020203);
    actor->mpBagModel = static_cast<J3DModel*>(bag);
    if (!bag) return FALSE;
    void* flyerData = btsw2_resource(0x10018AC4, 0x16);
    void* flyer = gabi::call<void*>(0x025E38E0, flyerData, 0, 0x11020203);
    actor->mpFlyerModel = static_cast<J3DModel*>(flyer);
    if (!flyer) return FALSE;
    actor->mPathState = 0;
    if (!daNpc_Btsw2_initTexPattern(actor, false)) return FALSE;
    u32 currentMorf = gabi::ea(actor->mpMorf.get());
    u32 model = gabi::load<u32>(currentMorf + 0x90);
    s8 headJoint = gabi::load<s8>(a + 0x3B4);
    u32 data = gabi::load<u32>(model + 0xAC);
    btsw2_setCallback(data, headJoint);
    s8 backJoint = gabi::load<s8>(a + 0x3B5);
    btsw2_setCallback(data, backJoint);
    currentMorf = gabi::ea(actor->mpMorf.get());
    model = gabi::load<u32>(currentMorf + 0x90);
    gabi::store<u32>(model + 0xB8, a);
    gabi::call(0x024EFF44, gabi::at<void>(a + 0x614), 30.0f, 0.0f);
    gabi::call(0x024F06B4, gabi::at<void>(a + 0x450), gabi::at<void>(a + 0x314),
        gabi::at<void>(a + 0x300), actor, 1, gabi::at<void>(a + 0x614), gabi::at<void>(a + 0x33C), 0, 0);
    return TRUE;
}
VERIFY(0x0221C790, daNpc_Btsw2_createHeap);

BOOL daNpc_Btsw2_nodeCallback(void* node, int phase) {
    WWHD_FUNC(0x0221C39C, BOOL, node, phase);
    if (phase != 0) return TRUE;
    u32 model = gabi::load<u32>(0x104B462C);
    u32 actor = gabi::load<u32>(model + 0xB8);
    if (!actor) return TRUE;
    u32 initialized = gabi::load<u32>(0x10466B20);
    f32 zero = gabi::load<f32>(0x10018A74);
    if (!initialized) {
        gabi::store<f32>(0x10466B08, zero);
        gabi::store<f32>(0x10466B10, zero);
        gabi::store<u32>(0x10466B20, 1);
        gabi::store<f32>(0x10466B0C, zero);
    }
    if (!gabi::load<u32>(0x10466B24)) {
        f32 x = gabi::load<f32>(0x10018A78);
        gabi::store<u32>(0x10466B24, 1);
        f32 y = gabi::load<f32>(0x10018A7C);
        gabi::store<f32>(0x10466B14, x);
        gabi::store<f32>(0x10466B18, y);
        gabi::store<f32>(0x10466B1C, zero);
    }
    void* jointObject = gabi::call<void*>(0x027F7878);
    u16 joint = gabi::load<u16>(gabi::ea(jointObject) + 4);
    u32 matrices = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(matrices + 4);
    u32 array = gabi::load<u32>(matrices + 0x10);
    gabi::store<u16>(matrices + 4, flags | 0x10);
    gabi::call(0x028E90D4, gabi::at<void>(array + u32(joint) * 48), gabi::at<void>(0x1048D0CC));
    s8 head = gabi::load<s8>(actor + 0x3B4);
    if (u32(joint) == u32(s32(head))) {
        gabi::call(0x028E8F64, gabi::at<void>(0x1048D0CC), gabi::at<void>(0x10466B08), gabi::at<void>(actor + 0x878));
        gabi::Local<Mtx34> rotation;
        gabi::call(0x028E90D4, gabi::at<void>(0x1048D0CC), rotation.get());
        f32 x = rotation->m[0][3];
        f32 z = rotation->m[2][3];
        rotation->m[2][3] = zero;
        f32 y = rotation->m[1][3];
        rotation->m[1][3] = zero;
        rotation->m[0][3] = zero;
        gabi::call(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
        s16 neck = gabi::load<s16>(actor + 0x3AE);
        s16 facing = gabi::load<s16>(actor + 0x322);
        gabi::call(0x025F1C28, gabi::at<void>(0x1048D0CC), s16(u16(neck) + u16(facing)));
        s16 pitch = gabi::load<s16>(actor + 0x3AC);
        gabi::call(0x025F1BF4, gabi::at<void>(0x1048D0CC), s16(-s32(pitch)));
        facing = gabi::load<s16>(actor + 0x322);
        gabi::call(0x025F1C28, gabi::at<void>(0x1048D0CC), s16(-s32(facing)));
        gabi::call(0x028E9108, gabi::at<void>(0x1048D0CC), rotation.get(), gabi::at<void>(0x1048D0CC));
        gabi::call(0x028E8F64, gabi::at<void>(0x1048D0CC), gabi::at<void>(0x10466B14), gabi::at<void>(actor + 0x37C));
    } else {
        s8 back = gabi::load<s8>(actor + 0x3B5);
        if (u32(joint) == u32(s32(back))) {
            s16 pitch = gabi::load<s16>(actor + 0x3B2);
            gabi::call(0x025F1BF4, gabi::at<void>(0x1048D0CC), pitch);
            s16 roll = gabi::load<s16>(actor + 0x3B0);
            gabi::call(0x025F1C5C, gabi::at<void>(0x1048D0CC), s16(-s32(roll)));
        }
    }
    gabi::call(0x028E90D4, gabi::at<void>(0x1048D0CC), gabi::at<void>(0x104B4868));
    matrices = gabi::load<u32>(model + 0x2C);
    flags = gabi::load<u16>(matrices + 4);
    array = gabi::load<u32>(matrices + 0x10);
    gabi::store<u16>(matrices + 4, flags | 0x10);
    mtx_copy(gabi::at<Mtx34>(array + u32(joint) * 48), gabi::at<Mtx34>(0x1048D0CC));
    return TRUE;
}
VERIFY(0x0221C39C, daNpc_Btsw2_nodeCallback);

void daNpc_Btsw2_pathMove(daNpc_Btsw2_c* actor) {
    WWHD_FUNC(0x0221D72C, void, actor);
    u32 a = address(actor);
    s16 timer = actor->mWaitTimer;
    f32 one = gabi::load<f32>(0x10018A80);
    if (timer > 0) {
        timer = s16(timer - 1);
        actor->mWaitTimer = timer;
        if (timer == 0) {
            daNpc_Btsw2_setAnm(actor, 7);
            f32 random = gabi::call<f32>(0x020198D8, 3.0f);
            actor->mPathTimer = s16(gabi::ftoi(gabi::fadds_ppc(random, one)));
        }
        if (s8(actor->mAnimation) == 8) {
            u32 morf = gabi::ea(actor->mpMorf.get());
            s16 end = gabi::load<s16>(morf + 0xA2);
            if (gabi::call<BOOL>(0x027F2BF8, gabi::at<void>(morf + 0x98), gabi::fsubs_ppc(f32(end), one)))
                daNpc_Btsw2_setAnm(actor, 0);
        }
        return;
    }
    s16 finalPoint = actor->mFinalPathPoint;
    s16 currentPoint = actor->mPathPoint;
    u32 path = gabi::ea(actor->mpPath.get());
    u32 points = gabi::load<u32>(path + 8);
    s16 nextPoint = currentPoint < finalPoint ? s16(currentPoint + 1) : 0;
    u32 point = points + u32(s32(nextPoint)) * 16;
    f32 y = gabi::load<f32>(point + 8);
    f32 x = gabi::load<f32>(point + 4);
    f32 z = gabi::load<f32>(point + 12);
    gabi::Local<cXyz> destination;
    destination->x = x; destination->y = y; destination->z = z;
    s16 desired = gabi::call<s16>(0x0200F93C, gabi::at<void>(a + 0x314), destination.get());
    s16 maxStep = gabi::load<s16>(0x10466AE4);
    s16 divisor = gabi::load<s16>(0x10466AE6);
    gabi::call(0x0200F428, gabi::at<void>(a + 0x322), desired, divisor, maxStep);
    gabi::Local<cXyz> direction;
    gabi::call(0x0201ADE0, destination.get(), direction.get(), gabi::at<void>(a + 0x314));
    f32 baseSpeed = gabi::load<f32>(0x10466AE8);
    gabi::store<f32>(a + 0x370, baseSpeed);
    BOOL normalized = gabi::call<BOOL>(0x0201B47C, direction.get());
    f32 zero = gabi::load<f32>(0x10018A74);
    if (normalized) {
        u16 facing = gabi::load<u16>(a + 0x322);
        u32 sinCos = 0x104A44F8 + (u32(facing) >> 3) * 8;
        gabi::Local<cXyz> forward;
        forward->y = zero;
        f32 sine = gabi::load<f32>(sinCos);
        f32 cosine = gabi::load<f32>(sinCos + 4);
        forward->x = sine; forward->z = cosine;
        f32 dot = gabi::call<f32>(0x028E8F44, direction.get(), forward.get());
        f32 acceleration = gabi::load<f32>(0x10466AEC);
        f32 speed = gabi::load<f32>(a + 0x370);
        gabi::store<f32>(a + 0x370, gabi::fmadds(acceleration, dot, speed));
    }
    if (actor->mPathAdvance) {
        s16 facing = gabi::load<s16>(a + 0x322);
        gabi::store<f32>(a + 0x370, zero);
        s16 delta = gabi::call<s16>(0x0200FAAC, facing, desired);
        gabi::call(0x0200F428, gabi::at<void>(a + 0x322), desired, 4, 0x1800);
        if (delta < 0x10) {
            actor->mPathAdvance = 0;
            daNpc_Btsw2_setAnm(actor, 7);
        }
    }
    gabi::call(0x025D6870, actor, gabi::at<void>(a + 0x654));
    gabi::Local<cXyz> remaining;
    gabi::call(0x0201ADE0, destination.get(), remaining.get(), gabi::at<void>(a + 0x314));
    f32 dx = remaining->x;
    f32 dz = remaining->z;
    gabi::Local<cXyz> horizontal;
    horizontal->x = dx; horizontal->y = zero; horizontal->z = dz;
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    f32 distance = gabi::call<f32>(0x028F4384, square);
    if (!(distance < 10.0f)) return;
    timer = actor->mPathTimer;
    actor->mPathPoint = nextPoint;
    if (timer > 0) { actor->mPathTimer = s16(timer - 1); return; }
    f32 random = gabi::call<f32>(0x020198D8, one);
    s8 animation = random < 0.5f ? 0 : 8;
    daNpc_Btsw2_setAnm(actor, animation);
    s16 randomRange = gabi::load<s16>(0x10466AF2);
    random = gabi::call<f32>(0x020198D8, f32(randomRange));
    s16 randomBase = gabi::load<s16>(0x10466AF0);
    actor->mWaitTimer = s16(gabi::ftoi(gabi::fadds_ppc(random, f32(randomBase))));
}
VERIFY(0x0221D72C, daNpc_Btsw2_pathMove);
