#include "d/actor/d_a_obj_YLzou.h"
namespace {
template <class T> T field(daObjYLzou_c *a, u32 off) { return gabi::load<T>(gabi::ea(a) + off); }
template <class T> void put(daObjYLzou_c *a, u32 off, T value) {
    gabi::store<T>(gabi::ea(a) + off, value);
}
void *ptr(u32 ea) { return gabi::at<void>(ea); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 save() { return gabi::load<u32>(0x101F84DC); }
s32 eventBit(u32 bit) { return gabi::call<s32>(0x025B8B94, ptr(save() + 0x644), bit); }
void setEvent(u32 bit) { gabi::call<void>(0x025B8B68, ptr(save() + 0x644), bit); }
void memberCall(u32 object, u32 descriptor) {
    s16 adjust = gabi::load<s16>(descriptor), slot = gabi::load<s16>(descriptor + 2);
    object += adjust;
    u32 target;
    if (slot < 0)
        target = gabi::load<u32>(descriptor + 4);
    else {
        s16 vtOff = gabi::load<s16>(descriptor + 6);
        u32 vt = gabi::load<u32>(object + vtOff);
        target = gabi::load<u32>(vt + u32(slot) * 8 + 4);
    }
    gabi::call<void>(target, ptr(object));
}
void sound(daObjYLzou_c *a, u32 id) {
    s8 room = field<s8>(a, 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, id, ptr(gabi::ea(a) + 0x314), 0, reverb);
}
void quake() {
    u32 p = play();
    gabi::Local<be<f32>[3]> axis;
    (*axis)[0] = 0;
    (*axis)[1] = 1;
    (*axis)[2] = 0;
    gabi::call<void>(0x025CB408, ptr(p + 0x599C), 6, 1, axis.get());
}
void matrixCopy(u32 src, u32 dst) {
    f32 values[12];
    for (int i = 0; i < 12; ++i)
        values[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 12; ++i)
        gabi::store<f32>(dst + 4 * i, values[i]);
}
bool sameText(u32 a, u32 b) {
    for (;; ++a, ++b) {
        u8 x = gabi::load<u8>(a), y = gabi::load<u8>(b);
        if (x != y)
            return false;
        if (x == 0)
            return true;
    }
}
} // namespace
void set_start_type(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BDADC, void, a);
    s32 action, complete = 0, demo = 0;
    bool moved = false;
    if (eventBit(0x2D04) == 0) {
        s32 sw = a->mSaveSwitch;
        if (sw != 255) {
            u32 sv = save();
            s8 room = field<s8>(a, 0x2FE);
            if (gabi::call<s32>(0x025BA0C0, ptr(sv + 0x20), sw, room) == 0) {
                action = 0;
                demo = 1;
            } else {
                action = 6;
                moved = true;
            }
        } else {
            action = 6;
            moved = true;
        }
    } else if (eventBit(0x3A04) == 0) {
        action = 6;
        moved = true;
    } else {
        s32 courtyard = eventBit(0x3804);
        u32 sv = save();
        if (courtyard == 0) {
            if (gabi::call<s32>(0x025B8B94, ptr(sv + 0x644), 0x3820) == 1) {
                action = 7;
                demo = 2;
            } else
                action = 11;
        } else if (gabi::call<s32>(0x025B8B94, ptr(sv + 0x644), 0x2D02) == 0) {
            action = 6;
            moved = true;
        } else {
            s32 n = gabi::call<s32>(0x025B7E00, ptr(save() + 0xD4));
            sv = save();
            if (n < 8) {
                if (gabi::call<s32>(0x025B8B94, ptr(sv + 0x644), 0x3820) == 1) {
                    action = 7;
                    demo = 2;
                } else
                    action = 11;
            } else {
                complete = 1;
                if (gabi::call<s32>(0x025B8B94, ptr(sv + 0x644), 0x2C01) == 0)
                    action = 12;
                else if (eventBit(0x3980) == 0) {
                    action = 13;
                    demo = 3;
                } else
                    action = 12;
            }
        }
    }
    a->mTriforceComplete = complete;
    a->mActionIdx = action;
    a->mDemoIdx = demo;
    u32 sv = save();
    gabi::call<void>(moved ? 0x025B8B68 : 0x025B8B7C, ptr(sv + 0x644), 0x3820);
}
VERIFY(0x023BDADC, set_start_type);
bool create_heap(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BDD20, bool, a);
    u32 which = a->mTriforceComplete;
    u32 idx = gabi::load<u32>(0x101CE094 + which * 4);
    gabi::Local<be<u32>[2]> request;
    (*request)[0] = 0x10033ADC;
    (*request)[1] = 0x10033AE4;
    u32 md = gabi::call<u32>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)), request.get(), idx);
    if (!md) {
        gabi::call<void>(0x0273AA24, ptr(0x10033B84), 0x194, ptr(0x10033B80));
        return false;
    }
    u32 model = gabi::call<u32>(0x025E38E0, ptr(md), 0x80000, 0x11000022);
    a->mpModel = gabi::at<J3DModel>(model);
    if (!model)
        return false;
    which = a->mTriforceComplete;
    gabi::Local<be<u32>[2]> collisionName;
    (*collisionName)[0] = 0x10033ADC;
    (*collisionName)[1] = 0x10033AE4;
    idx = gabi::load<u32>(0x101CE09C + which * 4);
    u32 dzb =
        gabi::call<u32>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)), collisionName.get(), idx);
    model = gabi::ea(a->mpModel.get());
    u32 bg = gabi::call<u32>(0x024F2478, ptr(dzb), 1, ptr(model ? model + 0xC8 : 0));
    a->mpBgW = ptr(bg);
    return gabi::ea(a->mpModel.get()) != 0 && bg != 0;
}
VERIFY(0x023BDD20, create_heap);
bool solidHeapCB(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BDE28, bool, a);
    return create_heap(a);
}
VERIFY(0x023BDE28, solidHeapCB);
void setup_action(daObjYLzou_c *a, s32 idx) {
    WWHD_FUNC(0x023BDE2C, void, a, idx);
    u32 offset = u32(idx) * 8;
    memberCall(gabi::ea(a), 0x101CE0A4 + offset);
    u32 first = gabi::load<u32>(0x101CE11C + offset);
    a->mActionMember[0] = first;
    u32 second = gabi::load<u32>(0x101CE120 + offset);
    a->mActionIdx = idx;
    a->mActionMember[1] = second;
}
VERIFY(0x023BDE2C, setup_action);
void set_mtx(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BDEE4, void, a);
    u16 angle = a->mVibAngle;
    f32 y = field<f32>(a, 0x318);
    f32 amplitude = a->mVibHeight;
    f32 sine = gabi::load<f32>(0x104A44F8 + (u32(angle) >> 3) * 8);
    y = gabi::fmadds(sine, amplitude, y);
    f32 z = field<f32>(a, 0x31C), x = field<f32>(a, 0x314);
    gabi::call<void>(0x028E93CC, ptr(0x1048D0CC), x, y, z);
    s16 ry = field<s16>(a, 0x32A), rz = field<s16>(a, 0x32C), rx = field<s16>(a, 0x328);
    gabi::call<void>(0x025F19F8, ptr(0x1048D0CC), rx, ry, rz);
    matrixCopy(0x1048D0CC, gabi::ea(a->mpModel.get()) + 0xC8);
    x = field<f32>(a, 0x330);
    z = field<f32>(a, 0x338);
    y = field<f32>(a, 0x334);
    gabi::call<void>(0x025F2518, x, y, z);
    gabi::call<void>(0x028E90D4, ptr(0x1048D0CC), ptr(gabi::ea(a) + 0x3BC));
}
VERIFY(0x023BDEE4, set_mtx);
void init_mtx(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BDFE4, void, a);
    u32 model = gabi::ea(a->mpModel.get());
    f32 y = field<f32>(a, 0x334), x = field<f32>(a, 0x330), z = field<f32>(a, 0x338);
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
    set_mtx(a);
}
VERIFY(0x023BDFE4, init_mtx);
s32 create(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE004, s32, a);
    u32 flags = field<u32>(a, 0x2E4);
    if (!(flags & 8)) {
        if (a) {
            gabi::call<void>(0x025D4ED0, a);
            put<u32>(a, 0xB4, 0x10033AFC);
            gabi::call<void>(0x028EFFD0, ptr(gabi::ea(a) + 0x408), 2, 0x34, ptr(0x023BEFC4));
            flags = field<u32>(a, 0x2E4);
        }
        put<u32>(a, 0x2E4, flags | 8);
    }
    if (field<u8>(a, 0xC) == 0) {
        a->mSaveSwitch = gabi::call<s32>(0x023BF0C8, a, 8, 0);
        set_start_type(a);
    }
    s32 phase = gabi::call<s32>(0x02520460, ptr(gabi::ea(a) + 0x3AC), ptr(0x10033ADC));
    if (phase == 4) {
        u32 idx = a->mTriforceComplete;
        u32 heapSize = gabi::load<u32>(0x101CE194 + idx * 4);
        if (gabi::call<s32>(0x025D63E8, a, ptr(0x023BDE28), heapSize)) {
            u32 p = play();
            u32 bg = gabi::ea(a->mpBgW.get());
            if (gabi::call<s32>(0x024EEA6C, ptr(p + 0x12A0), ptr(bg), a))
                phase = 5;
            else {
                u32 model = gabi::ea(a->mpModel.get());
                s32 idx = a->mActionIdx;
                put<u32>(a, 0x348, model ? model + 0xC8 : 0);
                setup_action(a, idx);
                init_mtx(a);
            }
        } else
            phase = 5;
    }
    return phase;
}
VERIFY(0x023BE004, create);
s32 actor_create(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE144, s32, a);
    return create(a);
}
VERIFY(0x023BE144, actor_create);
void remove_slip_smoke(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE148, void, a);
    for (int i = 0; i < 2; ++i) {
        u32 cb = gabi::ea(a) + 0x408 + 0x34 * i;
        u32 vt = gabi::load<u32>(cb);
        u32 target = gabi::load<u32>(vt + 0x44);
        gabi::call<void>(target, ptr(cb));
    }
}
VERIFY(0x023BE148, remove_slip_smoke);
bool delete_actor(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE198, bool, a);
    gabi::call<void>(0x025204C8, ptr(gabi::ea(a) + 0x3AC), ptr(0x10033ADC));
    remove_slip_smoke(a);
    if (field<u32>(a, 0xF4)) {
        u32 bg = gabi::ea(a->mpBgW.get());
        if (bg) {
            if (gabi::load<u32>(bg) < 0x100) {
                u32 p = play();
                bg = gabi::ea(a->mpBgW.get());
                gabi::call<void>(0x020087EC, ptr(p + 0x12A0), ptr(bg));
            }
            a->mpBgW = nullptr;
        }
    }
    return true;
}
VERIFY(0x023BE198, delete_actor);
bool actor_delete(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE218, bool, a);
    return delete_actor(a);
}
VERIFY(0x023BE218, actor_delete);
void set_slip_smoke_pos(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE21C, void, a);
    if (gabi::load<u32>(0x1046CAC8) == 0) {
        gabi::store<u32>(0x1046CAC8, 1);
        gabi::store<f32>(0x1046CAA4, 0);
        gabi::store<f32>(0x1046CAA0, 240);
        gabi::store<f32>(0x1046CA9C, -240);
        gabi::store<f32>(0x1046CA94, -240);
        gabi::store<f32>(0x1046CAA8, -240);
        gabi::store<f32>(0x1046CA98, 0);
    }
    f32 y = field<f32>(a, 0x318) - 1550.0f, z = field<f32>(a, 0x31C) - 1200.0f,
        x = field<f32>(a, 0x314);
    gabi::call<void>(0x028E93CC, ptr(0x1048D0CC), x, y, z);
    s16 ry = field<s16>(a, 0x322);
    gabi::call<void>(0x025F1C28, ptr(0x1048D0CC), ry);
    for (int i = 0; i < 2; ++i) {
        gabi::call<void>(0x025F23EC);
        u32 pos = 0x1046CA94 + i * 12;
        x = gabi::load<f32>(pos);
        y = gabi::load<f32>(pos + 4);
        z = gabi::load<f32>(pos + 8);
        gabi::call<void>(0x025F24E0, x, y, z);
        u32 cb = gabi::ea(a) + 0x408 + i * 0x34;
        gabi::store<f32>(cb + 0x20, gabi::load<f32>(0x1048D0D8));
        gabi::store<f32>(cb + 0x24, gabi::load<f32>(0x1048D0E8));
        gabi::store<f32>(cb + 0x28, gabi::load<f32>(0x1048D0F8));
        ry = field<s16>(a, 0x322);
        gabi::store<s16>(cb + 0x2C, 0);
        gabi::store<s16>(cb + 0x2E, ry);
        gabi::store<s16>(cb + 0x30, 0);
        gabi::call<void>(0x025F2468);
    }
}
VERIFY(0x023BE21C, set_slip_smoke_pos);
void start_slip_smoke(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE33C, void, a);
    if (gabi::load<u32>(0x1046CACC) == 0) {
        gabi::store<f32>(0x1046CABC, 2);
        gabi::store<f32>(0x1046CAC4, 2);
        gabi::store<u32>(0x1046CACC, 1);
        gabi::store<f32>(0x1046CAC0, 2);
    }
    set_slip_smoke_pos(a);
    for (int i = 0; i < 2; ++i) {
        s8 room = field<s8>(a, 0x326);
        u32 cb = gabi::ea(a) + 0x408 + i * 0x34;
        u32 p = play();
        u32 control = gabi::load<u32>(p + 0x5AB0);
        u32 emitter =
            gabi::call<u32>(0x025A847C, ptr(control), 2, 0x2022, ptr(cb + 0x20), ptr(cb + 0x2C),
                            ptr(0x1046CABC), 0xB9, ptr(cb), room, ptr(0), ptr(0), ptr(0));
        if (emitter) {
            gabi::store<f32>(emitter + 0x70, 15);
            gabi::store<f32>(emitter + 0x58, 0.15f);
            gabi::store<f32>(emitter + 0x34, 2);
            gabi::store<s16>(emitter + 0x60, 30);
        }
    }
}
VERIFY(0x023BE33C, start_slip_smoke);
void end_slip_smoke(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE46C, void, a);
    for (int i = 0; i < 2; ++i)
        gabi::call<void>(0x025A5F88, ptr(gabi::ea(a) + 0x408 + i * 0x34));
}
VERIFY(0x023BE46C, end_slip_smoke);
void smoke_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE4B4, void, a);
    u8 requested = a->mSmokeRequested, previous = a->mSmokePrevious;
    if (previous != requested) {
        if (requested)
            start_slip_smoke(a);
        else
            end_slip_smoke(a);
        a->mSmokePrevious = a->mSmokeRequested;
    } else if (previous)
        set_slip_smoke_pos(a);
}
VERIFY(0x023BE4B4, smoke_proc);
void vib_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE548, void, a);
    u32 previous = a->mVibPrevious, requested = a->mVibRequested;
    s16 angle = a->mVibAngle;
    if (previous != requested) {
        previous = requested;
        a->mVibPrevious = previous;
    }
    a->mVibAngle = s16(u16(angle) + 0x4000);
    f32 height = a->mVibHeight;
    if (previous == 1) {
        height += 0.05f;
        if (height > 0.8f)
            height = 0.8f;
    } else if (previous == 2) {
        height -= 0.05f;
        if (height < 0.4f)
            height = 0.4f;
    } else {
        height -= 0.1f;
        if (height < 0)
            height = 0;
    }
    a->mVibHeight = height;
}
VERIFY(0x023BE548, vib_proc);
bool execute(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE5F8, bool, a);
    gabi::call<void>(0x025D6870, a, ptr(0));
    set_mtx(a);
    u32 bg = gabi::ea(a->mpBgW.get());
    if (bg && gabi::load<u32>(bg) < 0x100)
        gabi::call<void>(0x024F43DC, ptr(bg));
    memberCall(gabi::ea(a), gabi::ea(a) + 0x3EC);
    smoke_proc(a);
    vib_proc(a);
    return true;
}
VERIFY(0x023BE5F8, execute);
bool actor_execute(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE6B4, bool, a);
    return execute(a);
}
VERIFY(0x023BE6B4, actor_execute);
bool draw(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE6B8, bool, a);
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4, ptr(env), 1, ptr(gabi::ea(a) + 0x314), ptr(gabi::ea(a) + 0x110));
    env = gabi::call<u32>(0x02555D0C);
    u32 model = gabi::ea(a->mpModel.get());
    gabi::call<void>(0x02562F5C, ptr(env), ptr(model), ptr(gabi::ea(a) + 0x110));
    model = gabi::ea(a->mpModel.get());
    gabi::call<void>(0x025E2DE0, ptr(model), 0);
    return true;
}
VERIFY(0x023BE6B8, draw);
bool actor_draw(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE714, bool, a);
    return draw(a);
}
VERIFY(0x023BE714, actor_draw);
s32 is_delete(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE718, s32, a);
    return 1;
}
VERIFY(0x023BE718, is_delete);
void start_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE720, void, a);
    a->mEventIdx = -1;
    put<f32>(a, 0x31C, 0);
    put<f32>(a, 0x2F0, 0);
    put<f32>(a, 0x318, 0);
    put<f32>(a, 0x314, 0);
    put<f32>(a, 0x2F4, 0);
    put<f32>(a, 0x2EC, 0);
}
VERIFY(0x023BE720, start_wait_init);
void regist_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE74C, void, a);
    s32 demo = a->mDemoIdx;
    if (demo != 1) {
        u32 p = play();
        gabi::call<void>(0x0254351C, ptr(p + 0x52E8));
        u32 status = field<u32>(a, 0x2E0);
        demo = a->mDemoIdx;
        put<u32>(a, 0x2E0, status | 0x4000);
    }
    u32 name = gabi::load<u32>(0x101CE200 + u32(demo) * 4);
    u32 p = play();
    s32 idx = gabi::call<s32>(0x02543F10, ptr(p + 0x52C4), ptr(name), 255);
    a->mEventIdx = idx;
    gabi::call<void>(0x025D7A58, a, idx, 255, 65535, 0, 1);
}
VERIFY(0x023BE74C, regist_wait_init);
void vib_start_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE7EC, void, a);
    setEvent(0x3820);
}
VERIFY(0x023BE7EC, vib_start_wait_init);
void vib_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE800, void, a);
    quake();
    a->mVibRequested = 1;
}
VERIFY(0x023BE800, vib_init);
void move_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE864, void, a);
    a->mSmokeRequested = 1;
    a->mVibRequested = 2;
    put<s16>(a, 0x322, -32768);
}
VERIFY(0x023BE864, move_init);
void end_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE880, void, a);
    s32 demo = a->mDemoIdx;
    a->mSmokeRequested = 0;
    put<f32>(a, 0x370, 0);
    if (demo != 3) {
        u32 p = play();
        gabi::call<void>(0x025CB610, ptr(p + 0x599C), -1);
        p = play();
        gabi::Local<be<f32>[3]> axis;
        (*axis)[2] = 0;
        (*axis)[0] = 0;
        (*axis)[1] = 1;
        gabi::call<void>(0x025CB374, ptr(p + 0x599C), 8, 1, axis.get());
    }
    a->mVibRequested = 0;
}
VERIFY(0x023BE880, end_wait_init);
void open_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE928, void, a);
    put<f32>(a, 0x2EC, 0);
    put<f32>(a, 0x31C, -680);
    put<f32>(a, 0x2F0, 0);
    put<f32>(a, 0x2F4, -680);
    put<f32>(a, 0x314, 0);
    put<f32>(a, 0x318, 0);
}
VERIFY(0x023BE928, open_wait_init);
void stairs_move_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE954, void, a);
    put<s16>(a, 0x322, 0);
    put<f32>(a, 0x2EC, 0);
    put<f32>(a, 0x314, 0);
    put<f32>(a, 0x2F0, 0);
    put<f32>(a, 0x318, 0);
    put<f32>(a, 0x2F4, 0);
    put<f32>(a, 0x31C, -680);
    quake();
    a->mVibRequested = 1;
}
VERIFY(0x023BE954, stairs_move_wait_init);
void stairs_move_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BE9F8, void, a);
    a->mSmokeRequested = 1;
    a->mVibRequested = 2;
}
VERIFY(0x023BE9F8, stairs_move_init);
void close_wait_init(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEA0C, void, a);
    put<f32>(a, 0x31C, 0);
    put<f32>(a, 0x2F0, 0);
    put<f32>(a, 0x318, 0);
    put<f32>(a, 0x314, 0);
    put<f32>(a, 0x2F4, 0);
    put<f32>(a, 0x2EC, 0);
}
VERIFY(0x023BEA0C, close_wait_init);
void start_wait_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEA30, void, a);
    s32 sw = a->mSaveSwitch;
    if (sw != 255) {
        s8 room = field<s8>(a, 0x2FE);
        u32 sv = save();
        if (gabi::call<s32>(0x025BA0C0, ptr(sv + 0x20), sw, room) == 1)
            setup_action(a, 1);
    }
}
VERIFY(0x023BEA30, start_wait_proc);
void regist_wait_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEA90, void, a);
    s16 idx = a->mEventIdx;
    if (idx != -1) {
        if (field<u16>(a, 0xF8) == 2)
            setup_action(a, u32(a->mActionIdx) + 1);
        else
            gabi::call<void>(0x025D7A58, a, idx, 255, 65535, 0, 1);
    } else {
        u32 name = gabi::load<u32>(0x101CE200 + u32(a->mDemoIdx) * 4);
        u32 p = play();
        a->mEventIdx = gabi::call<s32>(0x02543F10, ptr(p + 0x52C4), ptr(name), 255);
    }
}
VERIFY(0x023BEA90, regist_wait_proc);
void vib_start_wait_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEB5C, void, a);
    s16 event = a->mEventIdx;
    u32 p = play();
    u32 data = gabi::call<u32>(0x02544044, ptr(p + 0x52C4), event);
    if (data) {
        p = play();
        s32 staff = gabi::call<s32>(0x02542D88, ptr(p + 0x52C4), ptr(0x10033BD8), ptr(0), 0);
        if (staff != -1) {
            p = play();
            u32 text = gabi::call<u32>(0x02544830, ptr(p + 0x52C4), staff);
            if (sameText(text, 0x10033BD0))
                setup_action(a, u32(a->mActionIdx) + 1);
        }
    }
}
VERIFY(0x023BEB5C, vib_start_wait_proc);
void vib_act_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEC20, void, a);
    s16 event = a->mEventIdx;
    u32 p = play();
    u32 data = gabi::call<u32>(0x02544044, ptr(p + 0x52C4), event);
    if (data) {
        p = play();
        s32 staff = gabi::call<s32>(0x02542D88, ptr(p + 0x52C4), ptr(0x10033BE0), ptr(0), 0);
        if (staff != -1) {
            p = play();
            u32 text = gabi::call<u32>(0x02544830, ptr(p + 0x52C4), staff);
            if (sameText(text, 0x10033BE8))
                setup_action(a, u32(a->mActionIdx) + 1);
        }
    }
    sound(a, 0x6225);
}
VERIFY(0x023BEC20, vib_act_proc);
void move_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BED04, void, a);
    f32 home = field<f32>(a, 0x2F4), z = field<f32>(a, 0x31C);
    if (z < home - 680.0f) {
        sound(a, 0x6A27);
        setup_action(a, 5);
    } else {
        f32 speed = field<f32>(a, 0x370) + 0.1f;
        s8 room = field<s8>(a, 0x326);
        if (speed > 6.0f)
            speed = 6.0f;
        put<f32>(a, 0x370, speed);
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call<void>(0x025E1A40, 0x6226, ptr(gabi::ea(a) + 0x314), 0, reverb);
    }
}
VERIFY(0x023BED04, move_proc);
void stairs_move_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEDD0, void, a);
    f32 home = field<f32>(a, 0x2F4), z = field<f32>(a, 0x31C);
    if (z > home) {
        sound(a, 0x6A27);
        setup_action(a, 10);
    } else {
        f32 speed = field<f32>(a, 0x370) + 0.1f;
        s8 room = field<s8>(a, 0x326);
        if (speed > 6.0f)
            speed = 6.0f;
        put<f32>(a, 0x370, speed);
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call<void>(0x025E1A40, 0x6226, ptr(gabi::ea(a) + 0x314), 0, reverb);
    }
}
VERIFY(0x023BEDD0, stairs_move_proc);
void end_wait_proc(daObjYLzou_c *a) {
    WWHD_FUNC(0x023BEE90, void, a);
    s16 event = a->mEventIdx;
    u32 p = play();
    if (gabi::call<s32>(0x025440C8, ptr(p + 0x52C4), event)) {
        p = play();
        u16 status = gabi::load<u16>(p + 0x52B8);
        gabi::store<u16>(p + 0x52B8, status | 8);
        if (s32(a->mActionIdx) == 14)
            setEvent(0x3980);
        u32 demo = a->mDemoIdx;
        u32 flags = field<u32>(a, 0x2E0);
        put<u32>(a, 0x2E0, flags & ~0x4000u);
        s32 next = gabi::load<s32>(0x101CE19C + demo * 4);
        setup_action(a, next);
    }
}
VERIFY(0x023BEE90, end_wait_proc);
void global_init() {
    WWHD_FUNC(0x023BEF30, void);
    gabi::store<u32>(0x1046CAB4, 0);
    gabi::store<u32>(0x1046CAAC, 0);
    gabi::store<u32>(0x1046CAB8, 0);
    gabi::store<u32>(0x1046CAB0, 0);
    gabi::call<void>(0x028F026C, ptr(0x101CE1AC));
    gabi::store<f32>(0x1046CA88, -3.1415927410125732f);
    gabi::store<f32>(0x1046CA8C, 3.1415927410125732f);
    gabi::call<void>(0x028ED6F8, ptr(0x1046CA90));
    gabi::call<void>(0x028F026C, ptr(0x101CE1B8));
    gabi::call<void>(0x028EAB2C, ptr(0x1046CA91));
    gabi::call<void>(0x028F026C, ptr(0x101CE1C4));
}
VERIFY(0x023BEF30, global_init);
void *smoke_cb_ctor(void *object) {
    WWHD_FUNC(0x023BEFC4, void *, object);
    u32 ea = gabi::ea(object);
    if (!ea)
        ea = gabi::call<u32>(0x0273AD10, 0x34);
    if (ea) {
        gabi::call<void>(0x025A5B18, ptr(ea), 1);
        gabi::store<u32>(ea, 0x10033B38);
    }
    return ptr(ea);
}
VERIFY(0x023BEFC4, smoke_cb_ctor);
void callback_base_dtor(void *object, s32 flags) {
    WWHD_FUNC(0x023BF01C, void, object, flags);
    if (object && (flags & 1))
        gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x023BF01C, callback_base_dtor);
void smoke_cb_dtor(void *object, s32 flags) {
    WWHD_FUNC(0x023BF038, void, object, flags);
    if (object && (flags & 1))
        gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x023BF038, smoke_cb_dtor);
void callback_execute(void *object) { WWHD_FUNC(0x023BF030, void, object); }
VERIFY(0x023BF030, callback_execute);
void callback_draw(void *object) { WWHD_FUNC(0x023BF034, void, object); }
VERIFY(0x023BF034, callback_draw);
void callback_setup(void *object) { WWHD_FUNC(0x023BF04C, void, object); }
VERIFY(0x023BF04C, callback_setup);
void wait_act_proc(void *object) { WWHD_FUNC(0x023BF0C4, void, object); }
VERIFY(0x023BF0C4, wait_act_proc);
void actor_dtor(daObjYLzou_c *a, s32 flags) {
    WWHD_FUNC(0x023BF050, void, a, flags);
    if (a) {
        gabi::call<void>(0x028F0164, ptr(gabi::ea(a) + 0x408), 2, 0x34, ptr(0x023BF038), ptr(0),
                         ptr(0));
        gabi::call<void>(0x025D50BC, a, 0);
        if (flags & 1)
            gabi::call<void>(0x0273AF40, a);
    }
}
VERIFY(0x023BF050, actor_dtor);
u32 param(daObjYLzou_c *a, u32 width, u32 shift) {
    WWHD_FUNC(0x023BF0C8, u32, a, width, shift);
    u32 bits = field<u32>(a, 0xB0);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? (1u << width) : 0u) - 1;
    return (shift < 32 ? bits >> shift : 0u) & mask;
}
VERIFY(0x023BF0C8, param);
