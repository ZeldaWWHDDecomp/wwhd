#include "d/actor/d_a_bst.h"
#include <cmath>

// Gohdan's two hands approach Link, clap together, then withdraw.
static void paa_attack(bst_class* actor) {
    WWHD_FUNC(0x020E888C, void, actor);
    const u32 address = gabi::ea(actor);
    constexpr u32 tuning = 0x1047B608;
    auto half = [address](u32 offset) { return gabi::at<be<s16>>(address + offset); };
    auto scalar = [address](u32 offset) { return gabi::at<be<f32>>(address + offset); };
    u32 play = gabi::call<u32>(0x025200D4);
    u32 player = gabi::load<u32>(play + 0x5B2C);
    bool finish = false;
    bool move = true;
    cLib_addCalcAngleS2(half(0x328), 0, 10, 0x200);
    cLib_addCalcAngleS2(half(0x32C), 0, 10, 0x400);
    f32 scale = gabi::load<f32>(0x1000B860);
    f32 ten = gabi::load<f32>(0x1000B8BC);
    cLib_addCalc2(scalar(0x134C), gabi::load<f32>(0x1000B920), scale, ten);
    s16 state = gabi::load<s16>(address + 0x130E);
    f32 forty = gabi::load<f32>(0x1000B88C);
    gabi::Local<cXyz> vector, intermediate, transformed;
    auto target = gabi::at<cXyz>(address + 0x1310);
    auto position = gabi::at<cXyz>(address + 0x314);
    auto distanceToTarget = [&]() {
        gabi::call(0x0201ADE0, target, intermediate.get(), position);
        vector->copy(*intermediate);
        f32 square = PSVECSquareMag(vector.get());
        return std_sqrtf(square);
    };
    auto rotation = [&]() {
        s16 angle = gabi::load<s16>(address + 0x1350);
        u32 matrix = gabi::load<u32>(0x1018C7B0);
        gabi::call(0x025F1884, gabi::at<void>(matrix), angle);
    };
    switch ((u32)(s32)state) {
    case 0: {
        f32 zero = gabi::load<f32>(0x1000B854);
        gabi::store<s16>(address + 0x130E, (s16)(state + 1));
        gabi::store<f32>(address + 0x1324, zero);
        s16 playerFacing = gabi::load<s16>(player + 0x32A);
        u8 part = actor->mPartType;
        gabi::store<s16>(address + 0x1350, playerFacing);
        u16 animation = gabi::load<u16>(0x101928D0 + (u32)part * 2);
        gabi::call(0x020E47D0, actor, animation, ten, 2, scale, -1);
        s16 duration = gabi::load<s16>(tuning + 0x82);
        gabi::store<s16>(address + 0x1330, (s16)(duration + 80));
        [[fallthrough]];
    }
    case 1: {
        f32 distance = distanceToTarget();
        f32 range = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x38),
                                  gabi::load<f32>(0x1000B874));
        f32 acceleration = gabi::load<f32>(tuning + 0x3C);
        f32 increment = gabi::fadds_ppc(acceleration, gabi::load<f32>(0x1000B924));
        if (distance > range)
            cLib_addCalc2(scalar(0x370), forty, scale, increment);
        else
            cLib_addCalc0(scalar(0x370), scale, increment);
        f32 radius = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x1C),
                                   gabi::load<f32>(0x1000B928));
        gabi::store<f32>(address + 0x1320, radius);
        rotation();
        bool left = (u8)actor->mPartType == 1;
        f32 offsetX = gabi::load<f32>(tuning + 0x10);
        f32 lateral = gabi::fadds_ppc(offsetX, gabi::load<f32>(0x1000B870));
        if (left) {
            vector->y = gabi::load<f32>(tuning + 0x14);
            vector->z = gabi::load<f32>(tuning + 0x18);
            vector->x = -lateral;
        } else {
            vector->z = gabi::load<f32>(tuning + 0x18);
            vector->y = gabi::load<f32>(tuning + 0x14);
            vector->x = lateral;
        }
        gabi::call(0x0200FCD8, vector.get(), transformed.get());
        gabi::call(0x0201AD78, gabi::at<cXyz>(player + 0x314), intermediate.get(), transformed.get());
        s16 savedFacing = gabi::load<s16>(address + 0x1350);
        target->copy(*intermediate);
        cLib_addCalcAngleS2(half(0x32A), (s16)(savedFacing + (left ? 0x4000 : -0x4000)), 10, 0x800);
        if (gabi::load<s16>(address + 0x1330) == 0) {
            s16 current = gabi::load<s16>(address + 0x130E);
            gabi::store<s16>(address + 0x130E, (s16)(current + 1));
            for (u32 i = 0; i < 3; ++i)
                gabi::store<u32>(address + 0x1310 + i * 4, gabi::load<u32>(player + 0x314 + i * 4));
            u32 currentPlay = gabi::call<u32>(0x025200D4);
            u32 currentPlayer = gabi::load<u32>(currentPlay + 0x5B2C);
            s16 facing = gabi::call<s16>(0x025D6894, actor, gabi::at<void>(currentPlayer));
            gabi::store<s16>(address + 0x322, facing);
        }
        break;
    }
    case 2: {
        f32 speed = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x40),
                                  gabi::load<f32>(0x1000B8E0));
        f32 step = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x44), ten);
        cLib_addCalc2(scalar(0x370), speed, scale, step);
        f32 radius = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x1C),
                                   gabi::load<f32>(0x1000B8D8));
        gabi::store<f32>(address + 0x1320, radius);
        f32 distance = distanceToTarget();
        f32 range = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x48),
                                  gabi::load<f32>(0x1000B874));
        if (distance < range) {
            s16 current = gabi::load<s16>(address + 0x130E);
            gabi::store<s16>(address + 0x130E, (s16)(current + 1));
            s16 duration = gabi::load<s16>(tuning + 0x8E);
            gabi::store<s16>(address + 0x1330, (s16)(duration + 30));
        }
        break;
    }
    case 3: {
        move = false;
        s16 timer = gabi::load<s16>(address + 0x1330);
        if (timer >= 28) {
            if (timer == 28) {
                if ((u8)actor->mPartType == 1) {
                    if (address && address + 0x314) {
                        s8 room = gabi::load<s8>(address + 0x326);
                        s8 reverb = gabi::call<s8>(0x02520540, room);
                        gabi::call(0x025E1A40, 0x6994, position, 0, reverb);
                    }
                    gabi::Local<csXyz> angles;
                    angles->x = gabi::load<s16>(address + 0x328);
                    s16 yaw = gabi::load<s16>(address + 0x32A);
                    angles->y = yaw;
                    angles->y = (s16)(yaw + 0x4000);
                    angles->z = gabi::load<s16>(address + 0x32C);
                    u32 currentPlay = gabi::call<u32>(0x025200D4);
                    u32 particles = gabi::load<u32>(currentPlay + 0x5AB0);
                    gabi::call(0x025A847C, gabi::at<void>(particles), 4, 0xC1D7, target,
                               angles.get(), nullptr, 255, nullptr, -1, 0, nullptr, nullptr);
                    s8 room = gabi::load<s8>(address + 0x326);
                    currentPlay = gabi::call<u32>(0x025200D4);
                    particles = gabi::load<u32>(currentPlay + 0x5AB0);
                    gabi::call(0x025A847C, gabi::at<void>(particles), 2, 0xA1DA, target,
                               angles.get(), nullptr, 0xB9, gabi::at<void>(address + 0x3134),
                               room, 0, nullptr, nullptr);
                }
                s16 duration = gabi::load<s16>(tuning + 0x90);
                gabi::store<s16>(address + 0x135E, (s16)(duration + 5));
            }
            gabi::call(0x020E474C, actor, 1);
        }
        bool left = (u8)actor->mPartType == 1;
        s16 savedFacing = gabi::load<s16>(address + 0x1350);
        cLib_addCalcAngleS2(half(0x32A), (s16)(savedFacing + (left ? 0x4000 : -0x4000)), 1, 0x1000);
        f32 maxX = std::fabs(gabi::load<f32>(address + 0x33C));
        f32 targetX = target->x;
        cLib_addCalc2(scalar(0x314), targetX, scale, maxX);
        f32 maxZ = std::fabs(gabi::load<f32>(address + 0x344));
        f32 targetZ = target->z;
        cLib_addCalc2(scalar(0x31C), targetZ, scale, maxZ);
        if (gabi::load<s16>(address + 0x1330) == 0) {
            s16 current = gabi::load<s16>(address + 0x130E);
            f32 zero = gabi::load<f32>(0x1000B854);
            gabi::store<s16>(address + 0x130E, (s16)(current + 1));
            gabi::store<f32>(address + 0x370, zero);
            s16 duration = gabi::load<s16>(tuning + 0x86);
            s16 facing = gabi::load<s16>(address + 0x322);
            s16 saved = gabi::load<s16>(address + 0x1350);
            gabi::store<s16>(address + 0x1330, (s16)(duration + 30));
            gabi::store<s16>(address + 0x322, (s16)(facing - 0x8000));
            u32 matrix = gabi::load<u32>(0x1018C7B0);
            gabi::call(0x025F1884, gabi::at<void>(matrix), saved);
            bool nowLeft = (u8)actor->mPartType == 1;
            f32 offsetX = gabi::load<f32>(tuning + 0x10);
            f32 lateral = gabi::fadds_ppc(offsetX, gabi::load<f32>(0x1000B92C));
            if (nowLeft) {
                vector->y = gabi::load<f32>(tuning + 0x14);
                vector->z = gabi::load<f32>(tuning + 0x18);
                vector->x = -lateral;
            } else {
                vector->z = gabi::load<f32>(tuning + 0x18);
                vector->y = gabi::load<f32>(tuning + 0x14);
                vector->x = lateral;
            }
            gabi::call(0x0200FCD8, vector.get(), transformed.get());
            gabi::call(0x028E8D88, target, transformed.get(), target);
        }
        break;
    }
    case 4: {
        f32 acceleration = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x3C), scale);
        cLib_addCalc2(scalar(0x370), forty, scale, acceleration);
        f32 radius = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x1C),
                                   gabi::load<f32>(0x1000B930));
        s16 timer = gabi::load<s16>(address + 0x1330);
        gabi::store<f32>(address + 0x1320, radius);
        finish = timer == 0;
        break;
    }
    default:
        break;
    }
    if (move) gabi::call(0x020E4CD0, actor, 0);
    f32 floor = gabi::fadds_ppc(gabi::load<f32>(tuning + 0x18),
                              gabi::load<f32>(0x1000B888));
    if (!(gabi::load<f32>(address + 0x318) > floor))
        gabi::store<f32>(address + 0x318, floor);
    if (!finish && !(gabi::load<u32>(address + 0x13CC) & 0x10)) return;
    f32 wait = gabi::load<f32>(0x1000B8B4);
    gabi::store<s16>(address + 0x130A, 1);
    gabi::store<s16>(address + 0x130E, 0);
    f32 random = cM_rndF(wait);
    gabi::store<s16>(address + 0x133E, (s16)gabi::ftoi(gabi::fadds_ppc(random, wait)));
    if (finish) return;
    u8 part = actor->mPartType;
    u32 otherIndex = 2u - part;
    if (otherIndex >= 2) return;
    u32 slot = 0x10462978 + otherIndex * 4;
    u32 other = gabi::load<u32>(slot);
    if (gabi::load<s16>(other + 0x130A) != 11) return;
    gabi::store<s16>(other + 0x130A, 1);
    other = gabi::load<u32>(slot);
    gabi::store<s16>(other + 0x130E, 0);
    other = gabi::load<u32>(slot);
    random = cM_rndF(wait);
    gabi::store<s16>(other + 0x133E, (s16)gabi::ftoi(gabi::fadds_ppc(random, wait)));
}
VERIFY(0x020E888C, paa_attack);
