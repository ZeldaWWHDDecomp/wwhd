#include "d/actor/d_a_tag_mk.h"

template <class R = void, class... A> static R mkCall(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    gabi::store<u32>(linkage.a, callerSp);
    return gabi::call<R>(target, args...);
}
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...)                                                                    \
    if (gabi::Activation::nested())                                                                \
        return mkCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                         \
    gabi::Activation wwhd_activation_

static u32 mkPlay() {
    return mkCall<u32>(0x025200D4);
}
static u32 mkSave() {
    return gabi::load<u32>(0x101F84DC);
}
static void mkResetEvent() {
    u32 p = mkPlay();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
}
static void mkOrder(daTag_Mk_c *a, s32 event) {
    mkCall(0x025D7A58, a, event, 0xFF, 0xFFFF, 0, 1);
}
static u32 mkFindItem(daTag_Mk_c *a) {
    gabi::Local<be<u32>> id;
    *id.get() = u32(a->itemId);
    return u32(a->itemId) == 0xFFFFFFFF ? 0 : mkCall<u32>(0x025D5218, 0x025E1234, id.get());
}
void mkDemoProc(daTag_Mk_c *);
void mkTalkInit(daTag_Mk_c *);
BOOL mkCheckArea(daTag_Mk_c *, f32, f32, f32);
u16 mkTalk(daTag_Mk_c *);
u8 mkGetSwbit(daTag_Mk_c *);
u8 mkGetSwbit2(daTag_Mk_c *);
void mkSetTagWpEvId(daTag_Mk_c *);

BOOL mkActionArrival(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B001C, BOOL, a);
    if (mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x1E04) && !mkCall<s32>(0x02556D14))
        a->action = 2;
    return TRUE;
}
VERIFY(0x024B001C, mkActionArrival);
BOOL mkActionHunt(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0078, BOOL, a);
    u32 player = gabi::load<u32>(mkPlay() + 0x5B34);
    if ((gabi::load<u32>(player + 0x3C0) & 0x2000) &&
        mkCall<f32>(0x025D6958, a, player) < gabi::load<f32>(0x1003FA9C)) {
        s32 event = mkCall<s32>(0x02543F10, mkPlay() + 0x52C4, STR(0x1003FAA0), 0xFF);
        a->eventIndex = s16(event);
        mkOrder(a, event);
        a->action = 3;
    }
    return TRUE;
}
VERIFY(0x024B0078, mkActionHunt);
s32 mkGetNowEventAction(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0114, s32, a);
    s32 staff = a->staffIndex;
    return mkCall<s32>(0x02542EDC, mkPlay() + 0x52C4, staff, 0x101D1EC0, 4, 0, 1);
}
VERIFY(0x024B0114, mkGetNowEventAction);
void mkDemoInitWait(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0160, void, a);
    s32 staff = a->staffIndex;
    u32 timer = mkCall<u32>(0x0254487C, mkPlay() + 0x52C4, staff, STR(0x1003FACC), 3);
    a->cutEndTimer = timer ? gabi::load<s16>(timer + 2) : 0;
}
VERIFY(0x024B0160, mkDemoInitWait);
void mkDemoInitMake(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B01C4, void, a);
    gabi::Local<cXyz> pos;
    pos.get()->x = gabi::load<f32>(0x1003FAD4);
    pos.get()->y = gabi::load<f32>(0x1003FAD8);
    pos.get()->z = gabi::load<f32>(0x1003FADC);
    gabi::Local<csXyz> angle;
    angle.get()->x = 0;
    angle.get()->y = 0x4000;
    angle.get()->z = 0;
    s32 room = gabi::load<s8>(gabi::ea(a) + 0x326);
    u32 item =
        mkCall<u32>(0x025D8E6C, pos.get(), 0x1F, room, angle.get(), 0, 10.0f, 10.0f, -2.1f, 0);
    if (!item)
        mkCall(0x0273AA24, STR(0x1003FAF0), 0x167, STR(0x1003FAE8));
    a->itemId = item ? gabi::load<u32>(item + 4) : 0xFFFFFFFF;
    u32 control = mkPlay() + 0x51D0;
    u32 id = mkCall<u32>(0x0253F124, control, item);
    gabi::store<u32>(control + 0xCC, id);
}
VERIFY(0x024B01C4, mkDemoInitMake);
void mkDemoInitDelete(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B02D0, void, a);
    u32 item = mkFindItem(a);
    if (item)
        mkCall(0x025D57E0, item);
}
VERIFY(0x024B02D0, mkDemoInitDelete);
void mkDemoInitSetgoal(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B031C, void, a);
    u32 item = mkFindItem(a);
    if (item)
        mkCall(0x02543714, mkPlay() + 0x52C4, item + 0x314);
}
VERIFY(0x024B031C, mkDemoInitSetgoal);
BOOL mkDemoProcWait(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0380, BOOL, a);
    s16 timer = a->cutEndTimer;
    if (timer > 0)
        a->cutEndTimer = s16(timer - 1);
    else {
        s32 staff = a->staffIndex;
        mkCall(0x02543280, mkPlay() + 0x52C4, staff);
    }
    return FALSE;
}
VERIFY(0x024B0380, mkDemoProcWait);
BOOL mkDemoProcMake(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B03D8, BOOL, a);
    s32 staff = a->staffIndex;
    mkCall(0x02543280, mkPlay() + 0x52C4, staff);
    return FALSE;
}
VERIFY(0x024B03D8, mkDemoProcMake);
void mkDemoProc(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0414, void, a);
    s32 action = mkGetNowEventAction(a), staff = a->staffIndex;
    if (mkCall<s32>(0x025447C8, mkPlay() + 0x52C4, staff)) {
        switch (action) {
        case 0:
            mkDemoInitWait(a);
            break;
        case 1:
            mkDemoInitMake(a);
            break;
        case 2:
            mkDemoInitDelete(a);
            break;
        case 3:
            mkDemoInitSetgoal(a);
            break;
        }
    }
    switch (action) {
    case 0:
        mkDemoProcWait(a);
        break;
    case 1:
        mkDemoProcMake(a);
        break;
    default:
        staff = a->staffIndex;
        mkCall(0x02543280, mkPlay() + 0x52C4, staff);
        break;
    }
}
VERIFY(0x024B0414, mkDemoProc);
BOOL mkActionReady(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0538, BOOL, a);
    if (gabi::load<u16>(gabi::ea(a) + 0xF8) == 2) {
        a->action = 4;
        mkCall(0x025B8B68, mkSave() + 0x644, 0x1E02);
        u32 name = a->staffName.v.get();
        a->staffIndex = mkCall<s32>(0x02542D88, mkPlay() + 0x52C4, name, 0, 0);
        mkDemoProc(a);
    } else
        mkOrder(a, s16(a->eventIndex));
    return TRUE;
}
VERIFY(0x024B0538, mkActionReady);
BOOL mkActionEvent(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B05E4, BOOL, a);
    s32 event = a->eventIndex;
    if (mkCall<s32>(0x025440C8, mkPlay() + 0x52C4, event)) {
        a->action = 0;
        mkResetEvent();
    } else
        mkDemoProc(a);
    return TRUE;
}
VERIFY(0x024B05E4, mkActionEvent);
void mkTalkInit(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B065C, void, a);
    a->talkState = 0;
}
VERIFY(0x024B065C, mkTalkInit);
BOOL mkCheckArea(daTag_Mk_c *a, f32 width, f32 depth, f32 radius) {
    WWHD_FUNC(0x024B0668, BOOL, a, width, depth, radius);
    u32 player = gabi::load<u32>(mkPlay() + 0x5B34);
    gabi::Local<cXyz> relative, xz, normalized;
    mkCall(0x0201ADE0, player + 0x314, relative.get(), gabi::ea(a) + 0x314);
    xz.get()->x = relative.get()->x;
    xz.get()->y = gabi::load<f32>(0x1003FB00);
    xz.get()->z = relative.get()->z;
    f32 distance = mkCall<f32>(0x028E8DD0, xz.get());
    if (distance > radius)
        return FALSE;
    mkCall(0x0201B31C, relative.get(), normalized.get());
    f32 dot = gabi::fmadds(f32(relative.get()->x), f32(a->direction.x),
                           f32(f32(relative.get()->z) * f32(a->direction.z)));
    f32 extent = f32(f32(distance * dot) * dot);
    if (extent > depth || f32(distance - extent) > width)
        return FALSE;
    s16 delta =
        s16(s32(gabi::load<s16>(gabi::ea(a) + 0x322)) - s32(gabi::load<s16>(player + 0x322)));
    s32 magnitude = delta < 0 ? -s32(delta) : s32(delta);
    return magnitude >= 0x5000;
}
VERIFY(0x024B0668, mkCheckArea);
BOOL mkActionVilla(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B07E8, BOOL, a);
    if (gabi::load<u16>(gabi::ea(a) + 0xF8) == 1) {
        a->action = 6;
        mkTalkInit(a);
    } else if (mkCheckArea(a, gabi::load<f32>(0x1003FB04), gabi::load<f32>(0x1003FB08),
                           gabi::load<f32>(0x1003FB0C)))
        gabi::store<u16>(gabi::ea(a) + 0xFA, gabi::load<u16>(gabi::ea(a) + 0xFA) | 0x21);
    return TRUE;
}
VERIFY(0x024B07E8, mkActionVilla);
u32 mkGetMsg(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0864, u32, a);
    if (gabi::load<u8>(mkPlay() + 0x52B1) == 0x9C) {
        mkCall(0x025B8B68, mkSave() + 0x644, 0x2D80);
        return 0x1BC0;
    }
    return 0x1BBF;
}
VERIFY(0x024B0864, mkGetMsg);
u16 mkNextMsgStatus(daTag_Mk_c *a, be<u32> *message) {
    WWHD_FUNC(0x024B08BC, u16, a, message);
    u32 id = *message;
    if (id >= 0x1BC0 && id <= 0x1BC2) {
        *message = id + 1;
        return 0xF;
    }
    return 0x10;
}
VERIFY(0x024B08BC, mkNextMsgStatus);
u16 mkTalk(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B08E8, u16, a);
    u32 manager = gabi::load<u32>(0x101F4B5C);
    s8 state = a->talkState;
    u16 status = 0xFF;
    if (state == 0) {
        gabi::store<u32>(0x1046E5D8, 0xFFFFFFFF);
        a->message = mkGetMsg(a);
        a->talkState = 1;
    } else if (state != -1) {
        if (gabi::load<u32>(0x1046E5D8) == 0xFFFFFFFF) {
            u32 id = mkCall<u32>(0x025F7DB0, manager, u32(a->message), gabi::ea(a) + 0x37C);
            gabi::store<u32>(0x1046E5D8, id);
        } else if (state == 1)
            a->talkState = 2;
        else if (state == 2) {
            status = u16(mkCall<u32>(0x025F795C, manager));
            if (status == 0xE) {
                u16 next = mkNextMsgStatus(a, &a->message);
                mkCall(0x025F74D0, manager, next);
                if (mkCall<u32>(0x025F795C, manager) == 0xF)
                    mkCall(0x025F7DB0, manager, u32(a->message), 0);
            } else if (status == 0x12) {
                mkCall(0x025F74D0, manager, 0x13);
                a->talkState = -1;
            }
        }
    }
    return status;
}
VERIFY(0x024B08E8, mkTalk);
BOOL mkActionVillaTalk(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0A20, BOOL, a);
    if (!mkCall<s32>(0x02544950, mkPlay() + 0x52C4))
        return FALSE;
    if (mkTalk(a) == 0x12) {
        a->action = mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x2D80) ? 0 : 5;
        mkResetEvent();
    }
    return TRUE;
}
VERIFY(0x024B0A20, mkActionVillaTalk);
BOOL mkActionTagWp(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0ACC, BOOL, a);
    u32 play = mkPlay();
    u16 command = gabi::load<u16>(gabi::ea(a) + 0xF8);
    u32 player = gabi::load<u32>(play + 0x5B34);
    if (command == 2)
        a->action = 8;
    else if (f64(mkCall<f32>(0x025D6958, a, player)) < gabi::load<f64>(0x1003FB10))
        mkOrder(a, s16(a->eventIndex));
    return TRUE;
}
VERIFY(0x024B0ACC, mkActionTagWp);
BOOL mkActionTagWp2(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0B54, BOOL, a);
    s32 event = a->eventIndex;
    if (mkCall<s32>(0x025440C8, mkPlay() + 0x52C4, event)) {
        a->action = 0;
        mkResetEvent();
    }
    return TRUE;
}
VERIFY(0x024B0B54, mkActionTagWp2);
u8 mkGetSwbit(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0BBC, u8, a);
    return u32(a->mParameters) >> 8;
}
VERIFY(0x024B0BBC, mkGetSwbit);
u8 mkGetSwbit2(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0BC8, u8, a);
    return u32(a->mParameters) >> 16;
}
VERIFY(0x024B0BC8, mkGetSwbit2);
BOOL mkActionDaichi(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0BD4, BOOL, a);
    s16 state = a->musicState;
    switch (state) {
    case 0:
    case 10:
        if (mkCall<s32>(0x025B8B94, mkSave() + 0x1178, 0x440)) {
            a->musicState = s16(s16(a->musicState) + 1);
            mkCall(0x025E1960, 30);
        }
        break;
    case 1: {
        u8 sw = mkGetSwbit(a);
        s32 room = gabi::load<s8>(gabi::ea(a) + 0x326);
        if (mkCall<s32>(0x025BA0C0, mkSave() + 0x20, sw, room))
            a->musicState = s16(s16(a->musicState) + 1);
        break;
    }
    case 2:
    case 11:
        if (s16(a->cutEndTimer) > 0)
            a->cutEndTimer = s16(s16(a->cutEndTimer) - 1);
        else {
            a->musicState = s16(state + 1);
            if (mkCall<s32>(0x025B8B94, mkSave() + 0x1178, 0x440)) {
                mkCall(0x025E1918, 0x80000019);
                mkCall(0x025B8B68, mkSave() + 0x1178, 0x420);
            }
        }
        break;
    case 3:
    case 12: {
        u8 sw = mkGetSwbit2(a);
        s32 room = gabi::load<s8>(gabi::ea(a) + 0x326);
        if (mkCall<s32>(0x025BA0C0, mkSave() + 0x20, sw, room)) {
            a->musicState = s16(s16(a->musicState) + 1);
            if (mkCall<s32>(0x025B8B94, mkSave() + 0x1178, 0x420)) {
                mkCall(0x025E1928);
                mkCall(0x025B8B68, mkSave() + 0x1178, 0x410);
            }
        }
        break;
    }
    }
    return TRUE;
}
VERIFY(0x024B0BD4, mkActionDaichi);
BOOL mkExecute(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0D90, BOOL, a);
    switch (u8(a->action)) {
    case 1:
        mkActionArrival(a);
        break;
    case 2:
        mkActionHunt(a);
        break;
    case 3:
        mkActionReady(a);
        break;
    case 4:
        mkActionEvent(a);
        break;
    case 5:
        mkActionVilla(a);
        break;
    case 6:
        mkActionVillaTalk(a);
        break;
    case 7:
        mkActionTagWp(a);
        break;
    case 8:
        mkActionTagWp2(a);
        break;
    case 9:
        mkActionDaichi(a);
        break;
    }
    return TRUE;
}
VERIFY(0x024B0D90, mkExecute);
BOOL mkMthdExecute(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0E60, BOOL, a);
    mkExecute(a);
    return TRUE;
}
VERIFY(0x024B0E60, mkMthdExecute);
BOOL mkMthdIsDelete(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0E84, BOOL, a);
    return TRUE;
}
VERIFY(0x024B0E84, mkMthdIsDelete);
BOOL mkMthdDelete(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0E8C, BOOL, a);
    return TRUE;
}
VERIFY(0x024B0E8C, mkMthdDelete);
u8 mkGetType(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0E94, u8, a);
    return u32(a->mParameters);
}
VERIFY(0x024B0E94, mkGetType);
void mkSetTagWpEvId(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0EA0, void, a);
    mkPlay();
    u32 save = mkSave();
    if (gabi::load<u8>(save + 0x2E) != 0x3E)
        a->eventIndex = s16(mkCall<s32>(0x02543F10, mkPlay() + 0x52C4, STR(0x1003FB20), 0xFF));
    else {
        s32 count = mkCall<s32>(0x025B7E00, save + 0xD4);
        u32 events = mkPlay() + 0x52C4;
        a->eventIndex =
            s16(mkCall<s32>(0x02543F10, events, STR(count < 8 ? 0x1003FB18 : 0x1003FB28), 0xFF));
    }
}
VERIFY(0x024B0EA0, mkSetTagWpEvId);
s32 mkMthdCreate(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B0F4C, s32, a);
    u32 actor = gabi::ea(a), flags = gabi::load<u32>(actor + 0x2E4);
    if (!(flags & 8)) {
        if (actor) {
            mkCall(0x025D4ED0, a);
            flags = gabi::load<u32>(actor + 0x2E4);
            gabi::store<u32>(actor + 0xB4, 0x1003FA8C);
        }
        gabi::store<u32>(actor + 0x2E4, flags | 8);
    }
    u8 type = mkGetType(a);
    if (type == 3) {
        if (mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x2D80))
            a->action = 0;
        else {
            gabi::store<u8>(actor + 0x38B, 0xAB);
            u32 heading = gabi::load<u16>(actor + 0x322), table = 0x104A44F8 + (heading >> 3) * 8;
            f32 attention = gabi::load<f32>(actor + 0x394), eye = gabi::load<f32>(actor + 0x380),
                height = gabi::load<f32>(0x1003FB30);
            a->action = 5;
            gabi::store<u32>(actor + 0x39C, 0x02000008);
            a->direction.y = gabi::load<f32>(0x1003FB00);
            a->direction.x = gabi::load<f32>(table);
            a->direction.z = gabi::load<f32>(table + 4);
            gabi::store<f32>(actor + 0x394, f32(attention + height));
            gabi::store<f32>(actor + 0x380, f32(eye + height));
        }
    } else if (type == 2) {
        if (mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x3380) &&
            !mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x2D08) &&
            mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x2D02)) {
            a->action = 7;
            mkSetTagWpEvId(a);
        } else
            a->action = 0;
    } else if (type == 4) {
        a->action = 9;
        mkCall(0x025B8B7C, mkSave() + 0x1178, 0x420);
        mkCall(0x025B8B7C, mkSave() + 0x1178, 0x410);
        u8 second = mkGetSwbit2(a);
        s32 room = gabi::load<s8>(actor + 0x326);
        if (mkCall<s32>(0x025BA0C0, mkSave() + 0x20, second, room))
            a->musicState = 20;
        else {
            u8 first = mkGetSwbit(a);
            room = gabi::load<s8>(actor + 0x326);
            if (mkCall<s32>(0x025BA0C0, mkSave() + 0x20, first, room)) {
                a->cutEndTimer = 60;
                a->musicState = 10;
            } else {
                a->musicState = 0;
                a->cutEndTimer = 180;
            }
        }
    } else
        a->action = mkCall<s32>(0x025B8B94, mkSave() + 0x644, 0x1E02) ? 0 : 1;
    gabi::store<s16>(actor + 0x324, 0);
    gabi::store<s16>(actor + 0x320, 0);
    a->staffName = STR(0x1003FA84);
    gabi::store<s16>(actor + 0x328, 0);
    gabi::store<s16>(actor + 0x32C, 0);
    return 4;
}
VERIFY(0x024B0F4C, mkMthdCreate);
void mkStaticInit() {
    WWHD_FUNC(0x024B1290, void);
    gabi::store<u32>(0x1046E5F0, 0);
    gabi::store<u32>(0x1046E5E8, 0);
    gabi::store<u32>(0x1046E5F4, 0);
    gabi::store<u32>(0x1046E5EC, 0);
    mkCall(0x028F026C, 0x101D1ED0);
    gabi::store<f32>(0x1046E5DC, gabi::load<f32>(0x1003FB3C));
    gabi::store<f32>(0x1046E5E0, gabi::load<f32>(0x1003FB40));
    mkCall(0x028ED6F8, 0x1046E5E4);
    mkCall(0x028F026C, 0x101D1EDC);
    mkCall(0x028EAB2C, 0x1046E5E5);
    mkCall(0x028F026C, 0x101D1EE8);
}
VERIFY(0x024B1290, mkStaticInit);
BOOL mkMthdDraw(daTag_Mk_c *a) {
    WWHD_FUNC(0x024B1324, BOOL, a);
    return TRUE;
}
VERIFY(0x024B1324, mkMthdDraw);
void mkDestructor(daTag_Mk_c *a, s32 flags) {
    WWHD_FUNC(0x024B132C, void, a, flags);
    if (a) {
        mkCall(0x025D50BC, a, 0);
        if (flags & 1)
            mkCall(0x0273AF40, a);
    }
}
VERIFY(0x024B132C, mkDestructor);
