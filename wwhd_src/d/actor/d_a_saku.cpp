/**
 * d_a_saku.cpp (WWHD)
 * Object - Brown wooden barricade
 *
 * Reconstructed for the WWHD layout from the
 * GameCube decompilation (zeldaret/tww src/d/actor/d_a_saku.cpp) and the HD code,
 * verified against cking.rpx.
 */
#include "d/actor/d_a_saku.h"
#include <cmath>

namespace {
template<class T> be<T>& field(void* p, u32 offset) {
    return *gabi::at<be<T>>(gabi::ea(p) + offset);
}

u32 sakuSmokeConstruct(void* callback) {
    WWHD_FUNC(0x0246334C, u32, callback);
    return gabi::call<u32>(0x025A5B18, callback, 1);
}
VERIFY(0x0246334C, sakuSmokeConstruct);

void sakuColorToFloat(void* output, void* color) {
    WWHD_FUNC(0x02463354, void, output, color);
    // Read all components before publishing the result, as in the HD body.
    f32 r = f32(u8(field<u8>(color, 0))) / 255.0f;
    f32 g = f32(u8(field<u8>(color, 1))) / 255.0f;
    f32 b = f32(u8(field<u8>(color, 2))) / 255.0f;
    f32 a = f32(u8(field<u8>(color, 3))) / 255.0f;
    field<f32>(output, 0) = r;
    field<f32>(output, 4) = g;
    field<f32>(output, 8) = b;
    field<f32>(output, 12) = a;
}
VERIFY(0x02463354, sakuColorToFloat);

s32 sakuGetDzbId(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x024638F8, s32, actor, half);
    s32 state = actor->state[half];
    if (half == 1 || actor->state[1] == 0) return state == 1 ? 0 : 1;
    if (state == 2 || state == 3) return 3;
    u32 save = *gabi::at<be<u32>>(0x101F84DC);
    s8 room = actor->home.roomNo;
    u32 sw = actor->topSwitch;
    return gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), sw, room) ? 4 : 2;
}
VERIFY(0x024638F8, sakuGetDzbId);

s32 sakuCreateHeap(daSaku_c* actor, s32 heap, s32 half) {
    WWHD_FUNC(0x02463A88, s32, actor, heap, half);
    u32 state = actor->state[half];
    if (state < 1 || state > 3) return 0;
    u8 model = *gabi::at<be<u8>>(0x1003981B + state);
    if (!gabi::call<s32>(0x0246379C, actor, model, heap, half)) return 0;
    s32 dzb = gabi::call<s32>(0x024638F8, actor, half);
    return gabi::call<s32>(0x02463998, actor, dzb, heap, half) != 0;
}
VERIFY(0x02463A88, sakuCreateHeap);

s32 sakuRegisterBackground(daSaku_c* actor, s32 heap, s32 half) {
    WWHD_FUNC(0x02463B54, s32, actor, heap, half);
    u32 game = gabi::call<u32>(0x025200D4);
    u32 background = actor->backgrounds[half][heap];
    if (gabi::call<s32>(0x024EEA6C, gabi::at<void>(game + 0x12A0),
                         gabi::at<void>(background), actor)) return 0;
    background = actor->backgrounds[half][heap];
    actor->activeBackgrounds[half] = background;
    gabi::call(0x024F43DC, gabi::at<void>(background));
    return 1;
}
VERIFY(0x02463B54, sakuRegisterBackground);

s32 sakuCreateDummyHeap(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02463C04, s32, actor, half);
    s32 model = actor->type != 0;
    if (!gabi::call<s32>(0x0246379C, actor, model, 1, half)) return 0;
    return gabi::call<s32>(0x02463998, actor, 1, 1, half) != 0;
}
VERIFY(0x02463C04, sakuCreateDummyHeap);

s32 sakuDraw(daSaku_c* actor) {
    WWHD_FUNC(0x02464658, s32, actor);
    gabi::call<s32>(0x024644DC, actor, 0);
    if (actor->state[1] != 0) gabi::call<s32>(0x024644DC, actor, 1);
    return 1;
}
VERIFY(0x02464658, sakuDraw);

s32 sakuIsDelete(daSaku_c* actor) {
    WWHD_FUNC(0x02465608, s32, actor);
    return 1;
}
VERIFY(0x02465608, sakuIsDelete);

void sakuSmokeDestruct(void* callback, s32 flags) {
    WWHD_FUNC(0x02465610, void, callback, flags);
    if (callback && (flags & 1)) gabi::call(0x0273AF40, callback);
}
VERIFY(0x02465610, sakuSmokeDestruct);

void sakuEmptyVirtual(void* actor) {
    WWHD_FUNC(0x024656E8, void, actor);
}
VERIFY(0x024656E8, sakuEmptyVirtual);

void* sakuCylinderConstruct(void* object) {
    WWHD_FUNC(0x02465568, void*, object);
    if (!object) object = gabi::call<void*>(0x0273AD10, 0x130);
    if (object) {
        gabi::call(0x02515FB8, object);
        field<u32>(object, 0x114) = 0x100015A8;
        field<u32>(object, 0x110) = 0x100397AC;
        gabi::call(0x02018590, gabi::at<void>(gabi::ea(object) + 0x118));
        field<u32>(object, 0x3C) = 0x1004B108;
        field<u32>(object, 0x12C) = 0x1004B150;
        field<u32>(object, 0x114) = 0x1004B160;
    }
    return object;
}
VERIFY(0x02465568, sakuCylinderConstruct);

void sakuCylinderDestruct(void* object, s32 flags) {
    WWHD_FUNC(0x024655F4, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x024655F4, sakuCylinderDestruct);

void* sakuConstruct(daSaku_c* actor) {
    WWHD_FUNC(0x02463610, void*, actor);
    if (!actor) actor = gabi::call<daSaku_c*>(0x0273AD10, 0x1024);
    if (actor) {
        gabi::call(0x025D4ED0, actor);
        actor->__vtbl = 0x100397BC;
        gabi::call(0x028EFFD0, &actor->smoke[0], 2, 0x20, 0x0246334C);
        gabi::call(0x0200BD2C, &actor->collisionStatus[0]);
        gabi::call(0x02515DA0, gabi::at<void>(gabi::ea(actor) + 0x408));
        field<u32>(actor, 0x404) = 0x1004AE88;
        field<u32>(actor, 0x408) = 0x1004AEC0;
        gabi::call(0x028EFFD0, &actor->targetCylinders[0][0], 6, 0x130, 0x02465568);
        gabi::call(0x028EFFD0, &actor->fireCylinders[0], 3, 0x130, 0x02465568);
    }
    return actor;
}
VERIFY(0x02463610, sakuConstruct);

void sakuDestruct(daSaku_c* actor, s32 flags) {
    WWHD_FUNC(0x02465624, void, actor, flags);
    if (!actor) return;
    gabi::call(0x028F0164, &actor->fireCylinders[0], 3, 0x130, 0x02515A70, 0, 0);
    gabi::call(0x028F0164, &actor->targetCylinders[0][0], 6, 0x130, 0x02515A70, 0, 0);
    gabi::call(0x02515860, &actor->collisionStatus[0], 2);
    gabi::call(0x028F0164, &actor->smoke[0], 2, 0x20, 0x02465610, 0, 0);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1) gabi::call(0x0273AF40, actor);
}
VERIFY(0x02465624, sakuDestruct);

s32 sakuChangeCollision(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02465138, s32, actor, half);
    if (actor->state[half] == 0) return 0;
    s32 timer = actor->collisionTimers[half];
    if (timer >= 0) {
        if (timer == 0) {
            u32 game = gabi::call<u32>(0x025200D4);
            u32 background = actor->activeBackgrounds[half];
            gabi::call(0x020087EC, gabi::at<void>(game + 0x12A0), gabi::at<void>(background));
            gabi::call<s32>(0x02463B54, actor, 1, half);
            timer = actor->collisionTimers[half];
        }
        actor->collisionTimers[half] = s32(u32(timer) - 1);
    }
    return 1;
}
VERIFY(0x02465138, sakuChangeCollision);

s32 sakuExecute(daSaku_c* actor) {
    WWHD_FUNC(0x02465310, s32, actor);
    for (s32 half = 0; half < 2; ++half) {
        s32 timer = actor->particleTimers[half];
        if (timer != 0 && timer < 2000) actor->particleTimers[half] = s32(u32(timer) + 1);
    }
    s32 fire = actor->fireTimer;
    u32 age = actor->collisionTimers[2];
    if (fire != 0) actor->fireTimer = s32(u32(fire) - 1);
    actor->collisionTimers[2] = s32(age + 1);
    for (s32 half = 0; half < 2; ++half) {
        switch (s32(actor->state[half])) {
        case 1: gabi::call<s32>(0x02464D30, actor, half); break;
        case 2: gabi::call<s32>(0x0246505C, actor, half); break;
        case 3: gabi::call<s32>(0x02464F0C, actor, half); break;
        }
    }
    for (s32 half = 0; half < 2; ++half) gabi::call<s32>(0x02465138, actor, half);
    gabi::call(0x02463E40, actor);
    gabi::call(0x024651FC, actor);
    return 1;
}
VERIFY(0x02465310, sakuExecute);

s32 sakuRecreateHeap(daSaku_c* actor, s32 heapId, s32 half) {
    WWHD_FUNC(0x024646A4, s32, actor, heapId, half);
    u32 heap = actor->heaps[half][heapId];
    if (!heap) {
        gabi::call(0x0273AA24, gabi::at<void>(0x10039854), 0x38B, gabi::at<void>(0x10039834));
        heap = actor->heaps[half][heapId];
    }
    u32 table = field<u32>(gabi::at<void>(heap), 0xC);
    u32 freeAll = *gabi::at<be<u32>>(table + 0x54);
    gabi::call(freeAll, gabi::at<void>(heap));
    heap = actor->heaps[half][heapId];
    u32 previous = gabi::call<u32>(0x025E3570, gabi::at<void>(heap));
    gabi::call<s32>(0x02463A88, actor, 1, half);
    gabi::call(0x025E3570, gabi::at<void>(previous));
    return 1;
}
VERIFY(0x024646A4, sakuRecreateHeap);

s32 sakuFireFade(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x0246505C, s32, actor, half);
    if (actor->particleTimers[0] > 10) {
        u8 speed = *gabi::at<be<u8>>(0x1046D7C9);
        gabi::call<s32>(0x0200F4FC, &actor->modelAlpha[half][1], 255, speed);
        speed = *gabi::at<be<u8>>(0x1046D7C9);
        if (gabi::call<s32>(0x0200F4FC, &actor->modelAlpha[half][0], 0, speed)) {
            if (actor->heaps[half][0] != 0) {
                u8 delay = actor->heapReleaseDelay[half];
                if (delay != 0) {
                    delay = u8(delay - 1);
                    actor->heapReleaseDelay[half] = delay;
                    if (delay == 0) {
                        u32 heap = actor->heaps[half][0];
                        gabi::call(0x025E3868, gabi::at<void>(heap));
                        actor->heaps[half][0] = 0;
                        actor->models[half][0] = 0;
                    }
                }
            }
        }
    }
    return 1;
}
VERIFY(0x0246505C, sakuFireFade);

s32 sakuDebrisFade(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02464F0C, s32, actor, half);
    if (actor->heaps[half][0] != 0 && actor->heaps[half][1] != 0) {
        u8 delay = actor->heapReleaseDelay[half];
        if (delay != 0) {
            delay = u8(delay - 1);
            actor->heapReleaseDelay[half] = delay;
            if (delay == 0) {
                u32 heap = actor->heaps[half][0];
                gabi::call(0x025E3868, gabi::at<void>(heap));
                actor->heaps[half][0] = 0;
                actor->models[half][0] = 0;
            }
        }
    }
    void* callback = &actor->smoke[half];
    if (actor->particleTimers[half] >= 10 && field<u32>(callback, 4) != 0) {
        u8 fade = *gabi::at<be<u8>>(0x1046D7CA);
        f32 step = f32(fade) / 10200.0f;
        gabi::call<s32>(0x0200F5C8, &actor->smokeAlpha[half], 0.0f, step);
        f32 alpha = std::fabs(f32(actor->smokeAlpha[half]));
        actor->smokeAlpha[half] = alpha;
        u8 byteAlpha = u8(gabi::ftoi(alpha * 255.0f));
        u32 emitter = field<u32>(callback, 4);
        field<u8>(gabi::at<void>(emitter), 0x247) = byteAlpha;
        if (byteAlpha == 0) {
            u32 table = field<u32>(callback, 0);
            u32 end = *gabi::at<be<u32>>(table + 0x44);
            gabi::call(end, callback);
            actor->emitters[half] = 0;
        }
    }
    return 1;
}
VERIFY(0x02464F0C, sakuDebrisFade);

void sakuSetBackgroundMatrices(daSaku_c* actor) {
    WWHD_FUNC(0x024636D8, void, actor);
    void* matrix = gabi::at<void>(0x1048D0CC);
    f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
    gabi::call(0x028E93CC, matrix, x, y, z);
    s16 yaw = actor->shape_angle.y;
    gabi::call(0x025F1C28, matrix, yaw);
    f32 sx = actor->scale.x, sy = actor->scale.y, sz = actor->scale.z;
    gabi::call(0x025F2518, sx, sy, sz);
    gabi::call(0x028E90D4, matrix, &actor->backgroundMatrices[0][0]);
    if (actor->state[1] != 0) {
        y = f32(actor->current.pos.y) + 200.0f;
        x = actor->current.pos.x; z = actor->current.pos.z;
        gabi::call(0x028E93CC, matrix, x, y, z);
        yaw = actor->shape_angle.y;
        gabi::call(0x025F1C28, matrix, yaw);
        sx = actor->scale.x; sy = actor->scale.y; sz = actor->scale.z;
        gabi::call(0x025F2518, sx, sy, sz);
        gabi::call(0x028E90D4, matrix, &actor->backgroundMatrices[1][0]);
    }
}
VERIFY(0x024636D8, sakuSetBackgroundMatrices);

void sakuSetModelMatrices(daSaku_c* actor) {
    WWHD_FUNC(0x02463E40, void, actor);
    void* matrix = gabi::at<void>(0x1048D0CC);
    for (s32 half = 0; half < 2; ++half) {
        if (half == 1 && actor->state[1] == 0) break;
        for (s32 modelId = 0; modelId < 2; ++modelId) {
            u32 modelAddress = actor->models[half][modelId];
            if (!modelAddress) continue;
            void* model = gabi::at<void>(modelAddress);
            f32 sx = actor->scale.x, sy = actor->scale.y, sz = actor->scale.z;
            field<f32>(model, 0xBC) = sx;
            if (half == 0) {
                field<f32>(model, 0xC4) = sz; field<f32>(model, 0xC0) = sy;
            } else {
                field<f32>(model, 0xC0) = sy; field<f32>(model, 0xC4) = sz;
            }
            f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
            if (half == 1) y += 200.0f;
            gabi::call(0x028E93CC, matrix, x, y, z);
            s16 rx = actor->shape_angle.x, ry = actor->shape_angle.y, rz = actor->shape_angle.z;
            gabi::call(0x025F1B48, matrix, rx, ry, rz);
            f32 values[12];
            for (u32 i = 0; i < 12; ++i) values[i] = field<f32>(matrix, i * 4);
            // Each HD copy loads the entire matrix before its first store.
            const u8 lowerOrder[12] = {10,6,11,3,4,9,2,0,7,8,1,5};
            const u8 upperOrder[12] = {2,6,4,3,10,9,5,0,1,8,11,7};
            const u8* order = half == 0 ? lowerOrder : upperOrder;
            for (u32 i = 0; i < 12; ++i) {
                u32 component = order[i];
                field<f32>(model, 0xC8 + component * 4) = values[component];
            }
        }
    }
}
VERIFY(0x02463E40, sakuSetModelMatrices);

void sakuInitialize(daSaku_c* actor) {
    WWHD_FUNC(0x02464018, void, actor);
    for (s32 half = 0; half < 2; ++half) {
        actor->particleTimers[half] = 0;
        actor->emitters[half] = 0;
        actor->modelAlpha[half][0] = 255;
        actor->modelAlpha[half][1] = 0;
        actor->collisionTimers[half] = -1;
        actor->heapReleaseDelay[half] = 2;
    }
    u32 model = actor->models[0][0];
    actor->burning = 0;
    actor->fireTimer = 0;
    actor->cullMtx = model ? model + 0xC8 : 0;
    gabi::call(0x02515F14, &actor->collisionStatus[0], 255, 255, actor);
    gabi::call(0x02463C94, actor);
    gabi::call(0x02463E40, actor);
    for (s32 half = 0; half < 2; ++half) {
        for (u32 i = 0; i < 4; ++i)
            field<u8>(&actor->smoke[half], 0x16 + i) = *gabi::at<be<u8>>(0x101CFE50 + i);
        field<u8>(&actor->smoke[half], 0x11) = 1;
    }
}
VERIFY(0x02464018, sakuInitialize);

s32 sakuBroken(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02464C34, s32, actor, half);
    gabi::call<s32>(0x02464980, actor, half);
    actor->collisionTimers[half] = 0;
    actor->state[half] = 3;
    u32 save = *gabi::at<be<u32>>(0x101F84DC);
    s8 room = actor->home.roomNo;
    u32 sw = half == 0 ? u32(actor->bottomSwitch) : u32(actor->topSwitch);
    gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), sw, room);
    gabi::call<s32>(0x024646A4, actor, 1, half);
    if (half == 0) {
        u32 model = actor->models[0][1];
        actor->cullMtx = model ? model + 0xC8 : 0;
    }
    actor->modelAlpha[half][0] = 0;
    actor->modelAlpha[half][1] = 255;
    return 1;
}
VERIFY(0x02464C34, sakuBroken);

s32 sakuBurn(daSaku_c* actor) {
    WWHD_FUNC(0x02464850, s32, actor);
    if (actor->burning == 0) {
        for (s32 half = 0; half < 2; ++half) {
            if (actor->state[half] == 1) {
                actor->state[half] = 2;
                gabi::call<s32>(0x024646A4, actor, 1, half);
                actor->collisionTimers[half] = 50;
            }
        }
        u32 model = actor->models[0][1];
        if (!model) model = actor->models[1][1];
        if (model) actor->cullMtx = model + 0xC8;
        gabi::call<s32>(0x02464758, actor, 0);
        s8 room = actor->home.roomNo;
        actor->fireTimer = 90;
        u32 save = *gabi::at<be<u32>>(0x101F84DC);
        u32 sw = actor->bottomSwitch;
        gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), sw, room);
        if (actor->state[1] != 0) {
            room = actor->home.roomNo;
            save = *gabi::at<be<u32>>(0x101F84DC);
            sw = actor->topSwitch;
            gabi::call(0x025B9E38, gabi::at<void>(save + 0x20), sw, room);
        }
        actor->burning = 1;
    }
    return 1;
}
VERIFY(0x02464850, sakuBurn);

void sakuCheckCollision(daSaku_c* actor) {
    WWHD_FUNC(0x024651FC, void, actor);
    if (actor->fireTimer != 0) {
        for (s32 i = 0; i < 3; ++i) {
            void* cylinder = &actor->fireCylinders[i];
            gabi::call(0x020182E0, gabi::at<void>(gabi::ea(cylinder) + 0x118), &actor->collisionPositions[0][i]);
            u32 game = gabi::call<u32>(0x025200D4);
            gabi::call(0x0200E240, gabi::at<void>(game + 0x26A4), cylinder);
        }
    }
    for (s32 half = 0; half < 2; ++half) {
        if (actor->state[half] != 1) continue;
        for (s32 i = 0; i < 3; ++i) {
            void* cylinder = &actor->targetCylinders[half][i];
            gabi::call(0x020182E0, gabi::at<void>(gabi::ea(cylinder) + 0x118), &actor->collisionPositions[half][i]);
            u32 game = gabi::call<u32>(0x025200D4);
            gabi::call(0x0200E240, gabi::at<void>(game + 0x26A4), cylinder);
        }
    }
}
VERIFY(0x024651FC, sakuCheckCollision);

s32 sakuLoadModel(daSaku_c* actor, s32 index, s32 heap, s32 half) {
    WWHD_FUNC(0x0246379C, s32, actor, index, heap, half);
    const s32 brown[6] = {3,7,5,4,6,8};
    const s32 pale[6] = {3,5,4,6,8,7};
    if (half == 1) index = s32(u32(index) + 3);
    bool alternate = actor->type != 0;
    u32 resourceManager = *gabi::at<be<u32>>(0x101F4F28);
    u32 archive = *gabi::at<be<u32>>(alternate ? 0x101CFE8C : 0x101CFE88);
    s32 resource = alternate ? pale[index] : brown[index];
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = archive; (*name)[1] = 0x10039794;
    void* data = gabi::call<void*>(0x026066C4, gabi::at<void>(resourceManager), name.get(), resource);
    if (!data) gabi::call(0x0273AA24, gabi::at<void>(0x100397F8), 0x466, gabi::at<void>(0x10039808));
    u32 model = gabi::call<u32>(0x025E38E0, data, 0, 0x11020203);
    actor->models[half][heap] = model;
    return model != 0;
}
VERIFY(0x0246379C, sakuLoadModel);

s32 sakuLoadBackground(daSaku_c* actor, s32 index, s32 heap, s32 half) {
    WWHD_FUNC(0x02463998, s32, actor, index, heap, half);
    const s32 resources[5] = {3,4,5,6,3};
    u32 background = gabi::call<u32>(0x024F23F4, 0);
    actor->backgrounds[half][heap] = background;
    if (!background) return 0;
    s32 resource = resources[index];
    u32 manager = *gabi::at<be<u32>>(0x101F4F28);
    u32 archive = *gabi::at<be<u32>>(0x101CFE84);
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = archive; (*name)[1] = 0x10039794;
    void* data = gabi::call<void*>(0x026066C4, gabi::at<void>(manager), name.get(), resource);
    background = actor->backgrounds[half][heap];
    return gabi::call<s32>(0x0200A030, gabi::at<void>(background), data, 1,
                             &actor->backgroundMatrices[half][0]) == 0;
}
VERIFY(0x02463998, sakuLoadBackground);

void sakuSetCollision(daSaku_c* actor) {
    WWHD_FUNC(0x02463C94, void, actor);
    void* matrix = gabi::at<void>(0x1048D0CC);
    for (s32 half = 0; half < 2; ++half) {
        if (half == 1 && actor->state[1] == 0) break;
        f32 height = half == 0 ? 20.0f : 220.0f;
        actor->collisionPositions[half][0].x = 0.0f;
        actor->collisionPositions[half][0].y = height;
        actor->collisionPositions[half][0].z = 0.0f;
        actor->collisionPositions[half][1].x = -100.0f;
        actor->collisionPositions[half][1].y = height;
        actor->collisionPositions[half][1].z = 0.0f;
        actor->collisionPositions[half][2].x = 100.0f;
        actor->collisionPositions[half][2].y = height;
        actor->collisionPositions[half][2].z = 0.0f;
        f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
        gabi::call(0x028E93CC, matrix, x, y, z);
        s16 rx = actor->shape_angle.x, ry = actor->shape_angle.y, rz = actor->shape_angle.z;
        gabi::call(0x025F1B48, matrix, rx, ry, rz);
        for (s32 i = 0; i < 3; ++i) {
            cXyz* position = &actor->collisionPositions[half][i];
            gabi::call(0x028E8F64, matrix, position, position);
            void* cylinder = &actor->targetCylinders[half][i];
            gabi::call(0x02516518, cylinder, gabi::at<void>(0x10039888));
            field<u32>(cylinder, 0x44) = gabi::ea(&actor->collisionStatus[0]);
        }
    }
}
VERIFY(0x02463C94, sakuSetCollision);

s32 sakuDelete(daSaku_c* actor) {
    WWHD_FUNC(0x024643B0, s32, actor);
    s8 child = *gabi::at<be<s8>>(0x1046D7BC);
    if (child >= 0) {
        gabi::call(0x025F0A18, child);
        *gabi::at<be<s8>>(0x1046D7BC) = -1;
    }
    for (s32 half = 0; half < 2; ++half) {
        void* callback = &actor->smoke[half];
        u32 table = field<u32>(callback, 0);
        u32 end = *gabi::at<be<u32>>(table + 0x44);
        gabi::call(end, callback);
    }
    for (s32 half = 0; half < 2; ++half) {
        if (actor->state[half] != 0) {
            u32 game = gabi::call<u32>(0x025200D4);
            u32 background = actor->activeBackgrounds[half];
            gabi::call(0x020087EC, gabi::at<void>(game + 0x12A0), gabi::at<void>(background));
        }
    }
    for (s32 half = 0; half < 2; ++half) {
        for (s32 heapId = 0; heapId < 2; ++heapId) {
            u32 heap = actor->heaps[half][heapId];
            if (heap != 0) {
                gabi::call(0x025E3868, gabi::at<void>(heap));
                actor->heaps[half][heapId] = 0;
                actor->models[half][heapId] = 0;
            }
        }
    }
    u32 archive = *gabi::at<be<u32>>(0x101CFE84);
    gabi::call(0x025204C8, &actor->collisionPhase, gabi::at<void>(archive));
    bool alternate = actor->type != 0;
    archive = *gabi::at<be<u32>>(alternate ? 0x101CFE8C : 0x101CFE88);
    gabi::call(0x025204C8, &actor->modelPhase, gabi::at<void>(archive));
    return 1;
}
VERIFY(0x024643B0, sakuDelete);

s32 sakuDrawHalf(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x024644DC, s32, actor, half);
    u8 threshold = *gabi::at<be<u8>>(0x1046D7CE);
    u8 alpha = actor->modelAlpha[half][0];
    s32 depthWrite = alpha >= threshold;
    if (actor->heaps[half][0] != 0 && actor->models[half][0] != 0 && alpha != 0) {
        void* lighting = gabi::call<void*>(0x02555D0C);
        gabi::call(0x025626A4, lighting, 0, &actor->current.pos, &actor->tevStr);
        lighting = gabi::call<void*>(0x02555D0C);
        u32 model = actor->models[half][0];
        gabi::call(0x02562F5C, lighting, gabi::at<void>(model), &actor->tevStr);
        alpha = actor->modelAlpha[half][0]; model = actor->models[half][0];
        gabi::call<s32>(0x02463408, gabi::at<void>(model), alpha, depthWrite);
        u32 game = gabi::call<u32>(0x025200D4);
        u32 list = field<u32>(gabi::at<void>(game), 0x5D70);
        *gabi::at<be<u32>>(0x104B4634) = list;
        game = gabi::call<u32>(0x025200D4);
        list = field<u32>(gabi::at<void>(game), 0x5D74);
        *gabi::at<be<u32>>(0x104B4638) = list;
        model = actor->models[half][0];
        gabi::call(0x025E2DE0, gabi::at<void>(model), 0);
        game = gabi::call<u32>(0x025200D4);
        list = field<u32>(gabi::at<void>(game), 0x5D78);
        *gabi::at<be<u32>>(0x104B4634) = list;
        game = gabi::call<u32>(0x025200D4);
        list = field<u32>(gabi::at<void>(game), 0x5D7C);
        *gabi::at<be<u32>>(0x104B4638) = list;
        model = actor->models[half][0];
        gabi::call<s32>(0x02463408, gabi::at<void>(model), 255, 1);
    }
    if (actor->heaps[half][1] != 0 && actor->models[half][1] != 0 && actor->modelAlpha[half][1] != 0) {
        void* lighting = gabi::call<void*>(0x02555D0C);
        gabi::call(0x025626A4, lighting, 0, &actor->current.pos, &actor->tevStr);
        lighting = gabi::call<void*>(0x02555D0C);
        u32 model = actor->models[half][1];
        gabi::call(0x02562F5C, lighting, gabi::at<void>(model), &actor->tevStr);
        model = actor->models[half][1];
        gabi::call<s32>(0x02463408, gabi::at<void>(model), 255, depthWrite ^ 1);
        model = actor->models[half][1];
        gabi::call(0x025E2DE0, gabi::at<void>(model), 0);
        model = actor->models[half][1];
        gabi::call<s32>(0x02463408, gabi::at<void>(model), 255, 1);
    }
    return 1;
}
VERIFY(0x024644DC, sakuDrawHalf);

s32 sakuMaterialAlpha(void* model, u32 alpha, s32 depthWrite) {
    WWHD_FUNC(0x02463408, s32, model, alpha, depthWrite);
    if (!model) gabi::call(0x0273AA24, gabi::at<void>(0x10039754), 0x5FC, gabi::at<void>(0x10039764));
    gabi::call(0x027F58E0, model, alpha < 255 ? 1 : 0);
    u32 data = field<u32>(model, 0xAC);
    void* resource = gabi::call<void*>(0x027F3F8C, gabi::at<void>(data));
    u16 count = field<u16>(resource, 0x24);
    for (u16 i = 0; i < count; i = u16(i + 1)) {
        data = field<u32>(model, 0xAC);
        void* modelData = gabi::at<void>(data);
        u32 materialCount = field<u32>(modelData, 0xC);
        u32 material = field<u32>(modelData, 0x10);
        if (i < materialCount) material += u32(i) * 0x39C;
        void* node = gabi::at<void>(material);
        u32 nativeData = field<u32>(node, 0);
        s32 relative = field<s32>(gabi::at<void>(nativeData), 0x20);
        u32 pixelEngine = relative ? nativeData + 0x20 + u32(relative) : 0;
        void* pe = gabi::at<void>(pixelEngine);
        gabi::call(0x027E212C, pe, 3);
        gabi::call(0x027E19B4, gabi::at<void>(pixelEngine + 0x18), 4);
        gabi::call(0x027E19E4, gabi::at<void>(pixelEngine + 0x18), 4);
        gabi::call(0x027E19C4, gabi::at<void>(pixelEngine + 0x18), 5);
        gabi::call(0x027E19F4, gabi::at<void>(pixelEngine + 0x18), 5);
        gabi::call(0x027E18F8, gabi::at<void>(pixelEngine + 8), depthWrite != 0);
        u32 tev = field<u32>(node, 0x18);
        u32 table = field<u32>(gabi::at<void>(tev), 4);
        u32 getColor = *gabi::at<be<u32>>(table + 0x4C);
        void* color = gabi::call<void*>(getColor, gabi::at<void>(tev), 3);
        field<u8>(color, 3) = alpha;
        tev = field<u32>(node, 0x18);
        table = field<u32>(gabi::at<void>(tev), 4);
        u32 setColor = *gabi::at<be<u32>>(table + 0x3C);
        gabi::call(setColor, gabi::at<void>(tev), 3, color);
        gabi::Local<be<f32>[4]> normalized;
        gabi::call(0x02463354, normalized.get(), color);
        gabi::Local<be<f32>[4]> converted;
        gabi::call(0x0274D458, converted.get(), normalized.get(), 1.0f);
        u32 flag = field<u32>(node, 0xA0);
        field<u32>(node, 0xA0) = flag | 0x400;
        void* shaderColor = gabi::call<void*>(0x027F9F0C, gabi::at<void>(material + 0xA0), 10);
        u8 outputAlpha = field<u8>(color, 3);
        f32 red = (*converted)[0], green = (*converted)[1], blue = (*converted)[2];
        field<f32>(shaderColor, 4) = green;
        field<f32>(shaderColor, 8) = blue;
        field<f32>(shaderColor, 0) = red;
        field<f32>(shaderColor, 12) = f32(outputAlpha) / 255.0f;
        data = field<u32>(model, 0xAC);
        resource = gabi::call<void*>(0x027F3F8C, gabi::at<void>(data));
        count = field<u16>(resource, 0x24);
    }
    return 1;
}
VERIFY(0x02463408, sakuMaterialAlpha);

s32 sakuCheckHit(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02464D30, s32, actor, half);
    u32 broke = 0;
    for (s32 i = 0; i < 3; ++i) {
        void* cylinder = &actor->targetCylinders[half][i];
        if (!gabi::call<s32>(0x025162A4, cylinder)) continue;
        void* hit = gabi::call<void*>(0x02516300, cylinder);
        if (!hit) continue;
        bool alternate = actor->type != 0;
        u32 attack = field<u32>(hit, 0x10);
        u32 mask = alternate ? 0x04000C20 : 0x14010C2A;
        broke |= (attack & mask) != 0;
        if (broke) {
            u32 game = gabi::call<u32>(0x025200D4);
            gabi::Local<cXyz> direction;
            direction->x = 0.0f; direction->y = 1.0f; direction->z = 0.0f;
            gabi::call(0x025CB374, gabi::at<void>(game + 0x599C), 4, -0x21, direction.get());
            attack = field<u32>(hit, 0x10);
        }
        if (attack & 0x00060200) return gabi::call<s32>(0x02464850, actor);
    }
    if (broke) {
        if (half == 1 && actor->state[0] == 1) gabi::call<s32>(0x02464C34, actor, 0);
        return gabi::call<s32>(0x02464C34, actor, half);
    }
    return 1;
}
VERIFY(0x02464D30, sakuCheckHit);

s32 sakuSetFireEffect(daSaku_c* actor, s32 unused) {
    WWHD_FUNC(0x02464758, s32, actor, unused);
    gabi::Local<cXyz> position;
    f32 x = actor->current.pos.x, y = actor->current.pos.y, z = actor->current.pos.z;
    position->x = x; position->z = z; position->y = y;
    u32 game = gabi::call<u32>(0x025200D4);
    u32 particles = field<u32>(gabi::at<void>(game), 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 0, 0x045C, position.get(),
               &actor->current.angle, 0, 255, 0, -1, 0, 0, 0);
    game = gabi::call<u32>(0x025200D4);
    particles = field<u32>(gabi::at<void>(game), 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 0, 0x245E, position.get(),
               &actor->current.angle, 0, 230, 0, -1, 0, 0, 0);
    s8 room = actor->current.roomNo;
    actor->particleTimers[0] = 1;
    actor->particleTimers[1] = 1;
    u32 reverb = gabi::call<u32>(0x02520540, room);
    gabi::call(0x025E1A40, 0x6924, &actor->eyePos, 0, reverb);
    return 1;
}
VERIFY(0x02464758, sakuSetFireEffect);

s32 sakuCreate(daSaku_c* actor) {
    WWHD_FUNC(0x02464114, s32, actor);
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            gabi::call(0x02463610, actor);
            condition = actor->actor_condition;
        }
        actor->actor_condition = condition | 8;
    }
    u32 params = actor->mParameters;
    u8 type = u8((params >> 4) & 15);
    actor->type = type;
    u32 archive = *gabi::at<be<u32>>(type == 0 ? 0x101CFE88 : 0x101CFE8C);
    s32 phase = gabi::call<s32>(0x02520460, &actor->modelPhase, gabi::at<void>(archive));
    if (phase != 4) return phase;
    archive = *gabi::at<be<u32>>(0x101CFE84);
    phase = gabi::call<s32>(0x02520460, &actor->collisionPhase, gabi::at<void>(archive));
    if (phase != 4) return phase;
    params = actor->mParameters;
    s8 room = actor->home.roomNo;
    actor->state[0] = 1;
    actor->topSwitch = (params >> 16) & 255;
    u32 bottom = (params >> 8) & 255;
    actor->bottomSwitch = bottom;
    u32 save = *gabi::at<be<u32>>(0x101F84DC);
    if (gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), bottom, room)) actor->state[0] = 3;
    actor->state[1] = 0;
    params = actor->mParameters;
    if (params & 15) {
        actor->state[1] = 1;
        room = actor->home.roomNo;
        save = *gabi::at<be<u32>>(0x101F84DC);
        u32 top = actor->topSwitch;
        if (gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), top, room)) actor->state[1] = 3;
    }
    gabi::call(0x024636D8, actor);
    for (s32 half = 0; half < 2; ++half) {
        if (actor->state[half] == 0) continue;
        u32 heap = gabi::call<u32>(0x025E3630, 0, 0x20);
        actor->heaps[half][0] = heap;
        if (!heap) return 5;
        field<u32>(gabi::at<void>(heap), 0x10) = 0x10039780;
        s32 created = gabi::call<s32>(0x02463A88, actor, 0, half);
        if (created) gabi::call<s32>(0x02463B54, actor, 0, half);
        gabi::call(0x025E37D8);
        heap = actor->heaps[half][0];
        gabi::call(0x025E3678, gabi::at<void>(heap));
        if (!created) return 5;
        actor->models[half][1] = 0;
        if (actor->state[half] != 1) continue;
        heap = gabi::call<u32>(0x025E3630, 0, 0x20);
        actor->heaps[half][1] = heap;
        if (!heap) return 5;
        field<u32>(gabi::at<void>(heap), 0x10) = 0x10039788;
        created = gabi::call<s32>(0x02463C04, actor, half);
        actor->models[half][1] = 0;
        gabi::call(0x025E37D8);
        heap = actor->heaps[half][1];
        gabi::call(0x025E3678, gabi::at<void>(heap));
        if (!created) return 5;
    }
    gabi::call(0x02464018, actor);
    s8 child = *gabi::at<be<s8>>(0x1046D7BC);
    if (child < 0) {
        s32 id = gabi::call<s32>(0x025F0A10, gabi::at<void>(0x10039790), gabi::at<void>(0x1046D7BC));
        *gabi::at<be<s8>>(0x1046D7BC) = s8(id);
    }
    return 4;
}
VERIFY(0x02464114, sakuCreate);

s32 sakuSetBreakEffect(daSaku_c* actor, s32 half) {
    WWHD_FUNC(0x02464980, s32, actor, half);
    gabi::Local<cXyz> position;
    f32 y = f32(actor->current.pos.y) + 100.0f;
    f32 z = actor->current.pos.z, x = actor->current.pos.x;
    position->z = z; position->x = x;
    if (half == 1) y += 200.0f;
    position->y = y;
    u8 debrisEnabled = *gabi::at<be<u8>>(0x1046D7C7);
    if (debrisEnabled) {
        u32 game = gabi::call<u32>(0x025200D4);
        u32 particles = field<u32>(gabi::at<void>(game), 0x5AB0);
        void* color = gabi::at<void>(gabi::ea(actor) + 0x1A8);
        gabi::call(0x025A847C, gabi::at<void>(particles), 0, 0x045D, position.get(),
                   &actor->current.angle, &actor->scale, 255, 0, -1, color, color, 0);
        x = position->x; z = position->z; y = position->y;
    }
    u8 alpha = *gabi::at<be<u8>>(0x1046D7CA);
    actor->smokeAlpha[half] = f32(alpha) / 255.0f;
    u8 red = *gabi::at<be<u8>>(0x1046D7CB);
    u8 blue = *gabi::at<be<u8>>(0x1046D7CD);
    u8 green = *gabi::at<be<u8>>(0x1046D7CC);
    *gabi::at<be<u8>>(0x101CFE50) = red;
    *gabi::at<be<u8>>(0x101CFE51) = green;
    *gabi::at<be<u8>>(0x101CFE52) = blue;
    actor->smokePositions[half].x = x;
    actor->smokePositions[half].y = y;
    actor->smokePositions[half].z = z;
    s8 room = actor->current.roomNo;
    void* callback = &actor->smoke[half];
    alpha = *gabi::at<be<u8>>(0x1046D7CA);
    u32 game = gabi::call<u32>(0x025200D4);
    u32 particles = field<u32>(gabi::at<void>(game), 0x5AB0);
    gabi::call(0x025A847C, gabi::at<void>(particles), 2, 0x2027, &actor->smokePositions[half],
               &actor->current.angle, 0, alpha, callback, room, 0, 0, 0);
    if (field<u32>(callback, 4) != 0) {
        f32 intensity = actor->smokeAlpha[half];
        u32 emitter = field<u32>(callback, 4);
        field<u8>(gabi::at<void>(emitter), 0x247) = u8(gabi::ftoi(intensity * 255.0f));
        emitter = field<u32>(callback, 4);
        u32 flags = field<u32>(gabi::at<void>(emitter), 0x254);
        field<u32>(gabi::at<void>(emitter), 0x254) = flags | 0x40;
        emitter = field<u32>(callback, 4);
        field<f32>(gabi::at<void>(emitter), 0x238) = 3.2f;
        field<f32>(gabi::at<void>(emitter), 0x23C) = 3.2f;
        field<f32>(gabi::at<void>(emitter), 0x240) = 1.0f;
        emitter = field<u32>(callback, 4);
        field<f32>(gabi::at<void>(emitter), 0x220) = 2.0f;
        field<f32>(gabi::at<void>(emitter), 0x224) = 2.0f;
        field<f32>(gabi::at<void>(emitter), 0x228) = 2.0f;
        emitter = field<u32>(callback, 4);
        field<f32>(gabi::at<void>(emitter), 8) = 1.0f;
        field<f32>(gabi::at<void>(emitter), 12) = 0.5f;
        field<f32>(gabi::at<void>(emitter), 16) = 0.7f;
        emitter = field<u32>(callback, 4);
        field<f32>(gabi::at<void>(emitter), 0x34) = 40.0f;
        emitter = field<u32>(callback, 4);
        field<u32>(gabi::at<void>(emitter), 0x5C) = 1;
    }
    u8 type = actor->type;
    room = actor->current.roomNo;
    u32 sound = type == 0 ? 0x6847 : 0x693F;
    u32 reverb = gabi::call<u32>(0x02520540, room);
    gabi::call(0x025E1A40, sound, &actor->eyePos, 0, reverb);
    actor->particleTimers[half] = 1;
    return 1;
}
VERIFY(0x02464980, sakuSetBreakEffect);

void sakuStaticInitialize() {
    WWHD_FUNC(0x02465460, void);
    void* defaults = gabi::at<void>(0x1046D7D4);
    field<u32>(defaults, 8) = 0;
    field<u32>(defaults, 0) = 0;
    field<u32>(defaults, 12) = 0;
    field<u32>(defaults, 4) = 0;
    gabi::call(0x028F026C, gabi::at<void>(0x101CFE2C));
    *gabi::at<be<f32>>(0x1046D7B0) = -3.1415927410125732f;
    *gabi::at<be<f32>>(0x1046D7B4) = 3.1415927410125732f;
    gabi::call(0x028ED6F8, gabi::at<void>(0x1046D7B8));
    gabi::call(0x028F026C, gabi::at<void>(0x101CFE38));
    gabi::call(0x028EAB2C, gabi::at<void>(0x1046D7B9));
    gabi::call(0x028F026C, gabi::at<void>(0x101CFE44));
    void* hio = gabi::at<void>(0x1046D7BC);
    field<s16>(hio, 2) = 70;
    field<s16>(hio, 4) = 70;
    field<s16>(hio, 6) = 65;
    field<s16>(hio, 8) = 7;
    field<u32>(hio, 0x14) = 0x100397CC;
    field<s8>(hio, 0) = -1;
    field<u8>(hio, 0xA) = 1;
    field<u8>(hio, 0xB) = 1;
    field<u8>(hio, 0xE) = 180;
    field<u8>(hio, 0xF) = 105;
    field<u8>(hio, 0x10) = 91;
    field<u8>(hio, 0x11) = 48;
    field<s16>(hio, 0xC) = 5;
    field<u8>(hio, 0x12) = 100;
}
VERIFY(0x02465460, sakuStaticInitialize);
}
