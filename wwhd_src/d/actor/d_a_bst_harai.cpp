#include "d/actor/d_a_bst.h"

// Gohdan's sweeping hand attack. Fields still awaiting the complete actor layout
// are accessed through their independently checked HD offsets.
static void harai_attack(bst_class* actor) {
    WWHD_FUNC(0x020E9AA0, void, actor);
    const u32 address = gabi::ea(actor);
    constexpr u32 tuning = 0x1047B608;
    auto half = [address](u32 offset) { return gabi::at<be<s16>>(address + offset); };
    auto scalar = [address](u32 offset) { return gabi::at<be<f32>>(address + offset); };

    gabi::call<u32>(0x025200D4);
    u8 moveMode = 0;
    gabi::call(0x020E4790, actor, 1);
    cLib_addCalcAngleS2(half(0x328), 0, 10, 0x200);
    s16 facing = gabi::load<s16>(address + 0x322);
    cLib_addCalcAngleS2(half(0x32A), facing, 4, 0x1000);

    const bool rightHand = (u8)actor->mPartType == 2;
    f32 fifty = gabi::load<f32>(0x1000B890);
    s16 roll = rightHand ? 0x4000 : -0x4000;
    gabi::store<s16>(address + 0x30A4, roll);
    f32 tilt = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x34), fifty);
    if (rightHand) tilt = -tilt;
    cLib_addCalcAngleS2(half(0x32C), roll, 8, 0x400);
    f32 scale = gabi::load<f32>(0x1000B860);
    f32 maxStep = gabi::load<f32>(0x1000B8BC);
    cLib_addCalc2(scalar(0x1344), tilt, scale, maxStep);

    s16 state = gabi::load<s16>(address + 0x130E);
    f32 speed = gabi::load<f32>(0x1000B8D4);
    f32 zero = gabi::load<f32>(0x1000B854);
    switch ((u32)(s32)state) {
    case 0: {
        gabi::store<f32>(address + 0x1324, zero);
        gabi::store<f32>(address + 0x370, zero);
        gabi::store<s16>(address + 0x130E, (s16)(state + 1));
        s16 timer = gabi::load<s16>(tuning + 0x8A);
        gabi::store<s16>(address + 0x1330, (s16)(timer + 20));
        u32 play = gabi::call<u32>(0x025200D4);
        u32 player = gabi::load<u32>(play + 0x5B2C);
        s16 playerAngle = gabi::call<s16>(0x025D6894, actor, gabi::at<void>(player));
        u8 part = actor->mPartType;
        gabi::store<s16>(address + 0x322, playerAngle);
        u16 animation = gabi::load<u16>(0x10192878 + (u32)part * 2);
        gabi::call(0x020E47D0, actor, animation, maxStep, 2, scale, -1);
        [[fallthrough]];
    }
    case 1: {
        f32 acceleration = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x3C),
                                         gabi::load<f32>(0x1000B924));
        moveMode = 1;
        cLib_addCalc2(scalar(0x370), speed, scale, acceleration);
        f32 radius = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x1C),
                                   gabi::load<f32>(0x1000B930));
        s16 timer = gabi::load<s16>(address + 0x1330);
        gabi::store<f32>(address + 0x1320, radius);
        if (timer == 0) {
            s16 currentState = gabi::load<s16>(address + 0x130E);
            gabi::store<s16>(address + 0x130E, (s16)(currentState + 1));
            s16 duration = gabi::load<s16>(tuning + 0x8C);
            gabi::store<s16>(address + 0x320, 0);
            gabi::store<s16>(address + 0x1330, (s16)(duration + 60));
        }
        break;
    }
    case 2: {
        s16 turn = (s16)(gabi::load<s16>(tuning + 0x88) + 350);
        u8 part = actor->mPartType;
        moveMode = 1;
        s16 currentAngle = gabi::load<s16>(address + 0x322);
        u32 x = gabi::load<u32>(address + 0x314);
        gabi::store<s16>(address + 0x322,
                        (s16)(part == 2 ? currentAngle + turn : currentAngle - turn));
        gabi::Local<cXyz> horizontal;
        gabi::store<u32>(gabi::ea(horizontal.get()), x);
        u32 y = gabi::load<u32>(address + 0x318);
        s16 timer = gabi::load<s16>(address + 0x1330);
        gabi::store<u32>(gabi::ea(horizontal.get()) + 4, y);
        u32 z = gabi::load<u32>(address + 0x31C);
        gabi::store<f32>(gabi::ea(horizontal.get()) + 4, zero);
        gabi::store<u32>(gabi::ea(horizontal.get()) + 8, z);
        bool finish = timer == 0;
        if (!finish) {
            f32 square = PSVECSquareMag(horizontal.get());
            f32 radius = std_sqrtf(square);
            f32 boundary = gabi::load<f32>(0x1000B944);
            finish = radius > boundary;
        }
        if (finish) {
            s16 currentState = gabi::load<s16>(address + 0x130E);
            gabi::store<s16>(address + 0x320, 0);
            gabi::store<s16>(address + 0x1330, 30);
            gabi::store<s16>(address + 0x130E, (s16)(currentState + 1));
        }
        break;
    }
    case 3: {
        f32 deceleration = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x3C),
                                         gabi::load<f32>(0x1000B948));
        moveMode = 1;
        cLib_addCalc0(scalar(0x370), scale, deceleration);
        if (gabi::load<s16>(address + 0x1330) == 0) {
            gabi::store<s16>(address + 0x130A, 1);
            gabi::store<s16>(address + 0x130E, 0);
            f32 random = cM_rndF(speed);
            gabi::store<s16>(address + 0x133E,
                            (s16)gabi::ftoi(gabi::fadds_ppc(random, speed)));
        }
        break;
    }
    default:
        break;
    }
    gabi::store<s16>(address + 0x320, 0);
    f32 height = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x24),
                               gabi::load<f32>(0x1000B8CC));
    f32 heightScale = gabi::load<f32>(0x1000B87C);
    cLib_addCalc2(scalar(0x318), height, heightScale, maxStep);
    gabi::call(0x020E4CD0, actor, moveMode);
}
VERIFY(0x020E9AA0, harai_attack);
