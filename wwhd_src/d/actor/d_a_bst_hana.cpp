#include "d/actor/d_a_bst.h"

// Gohdan moves toward the room center and dispenses arrows or bombs from its nose.
static void hana_demo(bst_class* actor) {
    WWHD_FUNC(0x020EACE0, void, actor);
    const u32 address = gabi::ea(actor);
    constexpr u32 tuning = 0x1047B608;
    auto half = [address](u32 offset) { return gabi::at<be<s16>>(address + offset); };
    auto scalar = [address](u32 offset) { return gabi::at<be<f32>>(address + offset); };
    auto position = gabi::at<cXyz>(address + 0x314);
    f32 x = position->x;
    f32 z = position->z;
    gabi::store<s16>(address + 0x133C, 10);
    s16 angle = cM_atan2s(-x, -z);
    cLib_addCalcAngleS2(half(0x32A), angle, 10, 0x400);
    cLib_addCalcAngleS2(half(0x328), 0, 10, 0x400);
    cLib_addCalcAngleS2(half(0x32C), 0, 10, 0x400);
    f32 heightConstant = gabi::load<f32>(0x1000B93C);
    f32 tuningHeight = gabi::load<f32>(tuning + 0x34);
    f32 maxStep = gabi::load<f32>(0x1000B888);
    f32 height = gabi::fadds_ppc(tuningHeight, heightConstant);
    cLib_addCalc2(scalar(0x318), height, gabi::load<f32>(0x1000B864), maxStep);
    s16 state = gabi::load<s16>(address + 0x130E);
    f32 zero = gabi::load<f32>(0x1000B854);
    if (state == 0) {
        f32 animationSpeed = gabi::load<f32>(0x1000B860);
        gabi::store<f32>(address + 0x370, zero);
        f32 transition = gabi::load<f32>(0x1000B8B8);
        gabi::store<s16>(address + 0x130E, (s16)(state + 1));
        gabi::store<f32>(address + 0x340, zero);
        gabi::store<s16>(address + 0x1330, 40);
        gabi::call(0x020E47D0, actor, 20, transition, 0, animationSpeed, -1);
        gabi::store<u8>(address + 0x30CE, 1);
        if (gabi::ea(position)) {
            s8 room = gabi::load<s8>(address + 0x326);
            s8 reverb = gabi::call<s8>(0x02520540, room);
            gabi::call(0x025E1A40, 0x58E2, position, 0, reverb);
        }
    } else if (state != 1) {
        return;
    }

    gabi::Local<cXyz> horizontal, itemPosition;
    gabi::store<u32>(gabi::ea(horizontal.get()), gabi::load<u32>(address + 0x314));
    gabi::store<u32>(gabi::ea(horizontal.get()) + 4, gabi::load<u32>(address + 0x318));
    u32 positionZ = gabi::load<u32>(address + 0x31C);
    horizontal->y = zero;
    gabi::store<u32>(gabi::ea(horizontal.get()) + 8, positionZ);
    f32 square = PSVECSquareMag(horizontal.get());
    f32 radius = std_sqrtf(square);
    f32 limitConstant = gabi::load<f32>(0x1000B960);
    f32 limit = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x2C), limitConstant);
    if (radius > limit) {
        f32 ratio = limit / radius;
        f32 scale = gabi::load<f32>(0x1000B87C);
        f32 targetX = gabi::fmuls_ppc((f32)horizontal->x, ratio);
        cLib_addCalc2(scalar(0x314), targetX, scale, maxStep);
        f32 targetZ = gabi::fmuls_ppc((f32)horizontal->z, ratio);
        cLib_addCalc2(scalar(0x31C), targetZ, scale, maxStep);
    }
    u32 morph = actor->mpMorf;
    s32 frame = gabi::ftoi(gabi::load<f32>(morph + 0x9C));
    if (frame > 26) {
        u32 id = gabi::load<u32>(address + 0x30C0);
        u32 item = 0;
        gabi::Local<be<u32>> lookup;
        *lookup = id;
        if (id != 0xFFFFFFFF) {
            item = gabi::call<u32>(0x025D5218, gabi::at<void>(0x025E1234), lookup.get());
        }
        if (item) {
            u32 flags = gabi::load<u32>(item + 0x2E0);
            gabi::store<u32>(item + 0x2E0, flags | 0x4000);
        }
        morph = actor->mpMorf;
    }
    frame = gabi::ftoi(gabi::load<f32>(morph + 0x9C));
    if (frame != 26) return;

    u32 save = gabi::load<u32>(0x101F84DC);
    u32 model = gabi::load<u32>(morph + 0x90);
    u8 arrows = gabi::load<u8>(save + 0x89);
    u32 jointBlock = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(jointBlock + 4);
    u32 itemNumber;
    if (arrows == 0) {
        u32 matrices = gabi::load<u32>(jointBlock + 0x10);
        gabi::store<u16>(jointBlock + 4, flags | 0x10);
        u32 matrix = gabi::load<u32>(0x1018C7B0);
        itemNumber = 0x10;
        gabi::call(0x028E90D4, gabi::at<void>(matrices + 0x120), gabi::at<void>(matrix));
        gabi::store<u8>(address + 0x30D2, 0);
    } else {
        u32 matrices = gabi::load<u32>(jointBlock + 0x10);
        gabi::store<u16>(jointBlock + 4, flags | 0x10);
        u32 matrix = gabi::load<u32>(0x1018C7B0);
        itemNumber = 0xB;
        gabi::call(0x028E90D4, gabi::at<void>(matrices + 0x150), gabi::at<void>(matrix));
        gabi::store<u8>(address + 0x30D2, 1);
    }
    f32 offsetX = gabi::load<f32>(tuning + 0x10);
    f32 sixty = gabi::load<f32>(0x1000B8D4);
    f32 offsetY = gabi::load<f32>(tuning + 0x14);
    f32 offsetZ = gabi::load<f32>(tuning + 0x18);
    horizontal->y = offsetY;
    horizontal->z = offsetZ;
    horizontal->x = gabi::fadds_ppc(offsetX, sixty);
    auto eye = gabi::at<cXyz>(address + 0x37C);
    if (gabi::ea(eye)) {
        s8 room = gabi::load<s8>(address + 0x326);
        s8 reverb = gabi::call<s8>(0x02520540, room);
        gabi::call(0x025E1A40, 0x6901, eye, 0, reverb);
    }
    gabi::call(0x0200FCD8, horizontal.get(), itemPosition.get());
    s8 room = gabi::load<s8>(address + 0x326);
    u32 item = gabi::call<u32>(0x025D8870, itemPosition.get(), itemNumber, -1,
                              room, 0, nullptr, 0xB, nullptr);
    gabi::store<u32>(address + 0x30C0, item);
    if (gabi::ea(eye)) {
        s8 currentRoom = gabi::load<s8>(address + 0x326);
        s8 reverb = gabi::call<s8>(0x02520540, currentRoom);
        gabi::call(0x025E1A40, 0x6987, eye, 0, reverb);
    }
    morph = actor->mpMorf;
    model = gabi::load<u32>(morph + 0x90);
    jointBlock = gabi::load<u32>(model + 0x2C);
    flags = gabi::load<u16>(jointBlock + 4);
    u32 matrices = gabi::load<u32>(jointBlock + 0x10);
    gabi::store<u16>(jointBlock + 4, flags | 0x10);
    u32 matrix = gabi::load<u32>(0x1018C7B0);
    gabi::call(0x028E90D4, gabi::at<void>(matrices), gabi::at<void>(matrix));
    horizontal->x = zero;
    horizontal->y = zero;
    horizontal->z = zero;
    gabi::call(0x0200FCD8, horizontal.get(), itemPosition.get());
    s8 side = gabi::load<s8>(address + 0x30D2);
    s8 smokeRoom = gabi::load<s8>(address + 0x326);
    u16 effect = gabi::load<u16>(0x1019285C + (u32)((s32)side * 2));
    u32 play = gabi::call<u32>(0x025200D4);
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 2, effect, itemPosition.get(),
               gabi::at<csXyz>(address + 0x328), nullptr, 0xB9,
               gabi::at<void>(address + 0x3134), smokeRoom, 0, nullptr, nullptr);
}
VERIFY(0x020EACE0, hana_demo);
