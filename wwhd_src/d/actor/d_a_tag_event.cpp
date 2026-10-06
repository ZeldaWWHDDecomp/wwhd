#include "d/actor/d_a_tag_event.h"
#include "bindings.h"

namespace TagEvent {
// Keep every address-passed payload above outgoing EABI linkage (caller SP+4).
template<class T> struct Local : gabi::Local<T> {
    gabi::Local<be<u8>[16]> linkage;
};
template<class T> T read(u32 address) { return *gabi::at<be<T>>(address); }
template<class T> void write(u32 address, T value) { *gabi::at<be<T>>(address) = value; }
u32 save() { return read<u32>(0x101F84DC); }
u32 play() { return gabi::call<u32>(0x025200D4); }
s32 isSwitch(daTag_Event_c* actor, s32 bit) {
    u32 info = save() + 0x20;
    s32 room = actor->current.roomNo;
    return gabi::call<s32>(0x025BA0C0, info, bit, room);
}
void onSwitch(daTag_Event_c* actor, s32 bit) {
    u32 info = save() + 0x20;
    s32 room = actor->current.roomNo;
    gabi::call<void>(0x025B9E38, info, bit, room);
}
s32 eventBit(u32 bit) { return gabi::call<s32>(0x025B8B94, save() + 0x644, bit); }
void onEventBit(u32 bit) { gabi::call<void>(0x025B8B68, save() + 0x644, bit); }
void eventFlag() {
    u32 state = play();
    write<u16>(state + 0x52B8, read<u16>(state + 0x52B8) | 1);
}
}

u8 Tag_getSwbit(daTag_Event_c* actor);
u8 Tag_getEventNo(daTag_Event_c* actor);
u8 Tag_getType(daTag_Event_c* actor);
u16 Tag_getEventFlag(daTag_Event_c* actor);
s32 Tag_arrivalTerms(daTag_Event_c* actor);
s32 Tag_huntBlocked(daTag_Event_c* actor);
s32 Tag_cancelShutter(daTag_Event_c* actor);
s32 Tag_actionHunt(daTag_Event_c* actor);
s32 Tag_actionHunt2(daTag_Event_c* actor);
s32 Tag_actionMjHunt(daTag_Event_c* actor);
s32 Tag_actionArrival(daTag_Event_c* actor);
s32 Tag_actionEvent(daTag_Event_c* actor);
s32 Tag_actionReady(daTag_Event_c* actor);
s32 Tag_actionMjReady(daTag_Event_c* actor);
s32 Tag_actionSpeHunt(daTag_Event_c* actor);
s32 Tag_actionSpeArrival(daTag_Event_c* actor);
s32 Tag_actionSpeReady(daTag_Event_c* actor);
s32 Tag_actionSpeEvent(daTag_Event_c* actor);
void Tag_demoInitProc(daTag_Event_c* actor);
void Tag_demoEndProc(daTag_Event_c* actor);
void Tag_demoProc(daTag_Event_c* actor);

namespace TagEvent {
void order(daTag_Event_c* actor, u32 number) {
    s32 index = actor->mEventIdx;
    gabi::call<void>(0x025D7A58, actor, index, number, 65535, 0, 1);
}
s32 selectMjEvent(daTag_Event_c* actor, u32 fallback) {
    u32 state = play();
    u32 ship = read<u32>(state + 0x5CD8) & 0x10000;
    u32 number = Tag_getEventNo(actor);
    state = play();
    s32 index = gabi::call<s32>(0x02543F10, state + 0x52C4,
                              ship ? 0u : fallback, number);
    actor->mEventIdx = index;
    return index;
}
bool huntRange(daTag_Event_c* actor, u32 player, s32 bit, bool useSwitch) {
    Local<cXyz> difference;
    gabi::call<void>(0x0201ADE0, gabi::at<cXyz>(player + 0x314),
                     difference.get(), &actor->current.pos);
    f32 height = difference.get()->y;
    f32 x = difference.get()->x;
    f32 z = difference.get()->z;
    if (height < 0.0f) height = -height;
    if (useSwitch && bit != 255 && isSwitch(actor, bit)) {
        actor->mAction = 0;
        return false;
    }
    Local<cXyz> horizontal;
    horizontal.get()->y = 0.0f;
    horizontal.get()->z = z;
    horizontal.get()->x = x;
    f32 distance = gabi::call<f32>(0x028E8DD0, horizontal.get());
    f32 radius = actor->scale.x;
    f32 radiusSquared = radius * radius;
    if (!(distance < radiusSquared * 10000.0f)) return false;
    f32 limit = actor->scale.y;
    if (height > limit * 100.0f) return false;
    return true;
}
}

namespace TagEvent {
struct SafeString_l { be<u32> text; be<u32> vtable; };
// Failed password lookup leaves this buffer untouched. The HD length scan can
// therefore reach saved registers; retain their defined guest-frame bytes.
struct PasswordFrame_l {
    be<u32> backchain;
    u8 _04[4];
    SafeString_l text;
    u8 _10[8];
    be<u16> password[20];
    be<u32> saved[4];
};
WWHD_OFFSET(PasswordFrame_l, text, 0x08);
WWHD_OFFSET(PasswordFrame_l, password, 0x18);
WWHD_OFFSET(PasswordFrame_l, saved, 0x40);
WWHD_SIZE(PasswordFrame_l, 0x50);
void assure(SafeString_l* text) {
    u32 target = read<u32>((u32)text->vtable + 0x14);
    gabi::call<void>(target, text);
}
void setPartners(u32 partner) {
    u32 control = play() + 0x51D0;
    u32 pid = gabi::call<u32>(0x0253F124, control, partner);
    write<u32>(control + 0xD0, pid);
    control = play() + 0x51D0;
    pid = gabi::call<u32>(0x0253F124, control, partner);
    write<u32>(control + 0xCC, pid);
}
}

namespace TagEvent {
bool stageEquals(SafeString_l* name, SafeString_l* stage) {
    assure(name);
    assure(name);
    u32 left = name->text;
    assure(stage);
    u32 right = stage->text;
    if (left == right) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = read<u8>(left + i);
        u8 b = read<u8>(right + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}
void createDispatch(daTag_Event_c* actor) {
    switch ((u8)actor->mAction) {
    case 1: Tag_actionArrival(actor); break;
    case 2: Tag_actionHunt(actor); break;
    case 3: Tag_actionHunt2(actor); break;
    case 4: Tag_actionReady(actor); break;
    case 5: Tag_actionEvent(actor); break;
    case 6: Tag_actionSpeArrival(actor); break;
    case 7: Tag_actionSpeHunt(actor); break;
    case 8: Tag_actionSpeReady(actor); break;
    case 9: Tag_actionSpeEvent(actor); break;
    case 10: Tag_actionMjHunt(actor); break;
    case 11: Tag_actionMjReady(actor); break;
    }
}
}

u32 Tag_getBk(u32 type) {
    WWHD_FUNC(0x024A65C4, u32, type);
    return gabi::call<u32>(0x025D9F38, STR(0x1003EFE8), 15, type);
}
VERIFY(0x024A65C4, Tag_getBk);

u8 Tag_getSwbit2(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A65D8, u8, actor);
    return (u32)actor->mParameters >> 16;
}
VERIFY(0x024A65D8, Tag_getSwbit2);

u8 Tag_getType(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A65E4, u8, actor);
    return TagEvent::read<u8>(gabi::ea(actor) + 0xB3);
}
VERIFY(0x024A65E4, Tag_getType);

u16 Tag_getEventFlag(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A65F0, u16, actor);
    return actor->home.angle.z;
}
VERIFY(0x024A65F0, Tag_getEventFlag);

s32 Tag_arrivalTerms(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A65F8, s32, actor);
    s32 bit = Tag_getSwbit2(actor);
    if (Tag_getType(actor) == 12) {
        if (bit != 255 && TagEvent::isSwitch(actor, bit)) return 0;
        return 1;
    }
    u16 flag = Tag_getEventFlag(actor);
    if (bit != 255 && !TagEvent::isSwitch(actor, bit)) return 0;
    if (flag != 65535 && flag != 0 && !TagEvent::eventBit(flag)) return 0;
    return 1;
}
VERIFY(0x024A65F8, Tag_arrivalTerms);

u8 Tag_getSwbit(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A66E0, u8, actor);
    return (u32)actor->mParameters >> 8;
}
VERIFY(0x024A66E0, Tag_getSwbit);

u8 Tag_getEventNo(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A66EC, u8, actor);
    return (u32)actor->mParameters >> 24;
}
VERIFY(0x024A66EC, Tag_getEventNo);

s32 Tag_actionHunt2(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A66F8, s32, actor);
    s32 bit = Tag_getSwbit(actor);
    if (bit != 255 && TagEvent::isSwitch(actor, bit)) {
        actor->mAction = 0;
    } else if (Tag_getBk(actor->mBkType) == 0) {
        s32 timer = actor->mHuntTimer;
        if (timer > 0) actor->mHuntTimer = timer - 1;
        else {
            actor->mAction = 4;
            u32 number = Tag_getEventNo(actor);
            s32 index = actor->mEventIdx;
            gabi::call<void>(0x025D7A58, actor, index, number, 65535, 0, 1);
        }
    } else actor->mHuntTimer = 65;
    return 1;
}
VERIFY(0x024A66F8, Tag_actionHunt2);

s32 Tag_huntBlocked(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A67D0, s32, actor);
    u32 delay = actor->mArrivalDelay;
    if (delay != 0) { actor->mArrivalDelay = delay + 1; return 1; }
    u32 state = TagEvent::play();
    s32 change = gabi::ftoi(TagEvent::read<f32>(state + 0x5B44));
    state = TagEvent::play();
    u32 count = TagEvent::read<u16>(state + 0x5BAC);
    if ((s32)(count + (u32)change) > 0) return 0;
    if (change < 0) actor->mArrivalDelay = (u32)actor->mArrivalDelay + (u32)change;
    return 1;
}
VERIFY(0x024A67D0, Tag_huntBlocked);

s32 Tag_cancelShutter(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A688C, s32, actor);
    u32 type = Tag_getType(actor);
    return type == 4 || type == 7 || type == 12;
}
VERIFY(0x024A688C, Tag_cancelShutter);

s32 Tag_actionSpeEvent(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7B24, s32, actor);
    return Tag_actionEvent(actor);
}
VERIFY(0x024A7B24, Tag_actionSpeEvent);

s32 Tag_Execute(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7BAC, s32, actor);
    switch ((u8)actor->mAction) {
    case 1: Tag_actionArrival(actor); break;
    case 2: Tag_actionHunt(actor); break;
    case 3: Tag_actionHunt2(actor); break;
    case 4: Tag_actionReady(actor); break;
    case 5: Tag_actionEvent(actor); break;
    case 6: Tag_actionSpeArrival(actor); break;
    case 7: Tag_actionSpeHunt(actor); break;
    case 8: Tag_actionSpeReady(actor); break;
    case 9: Tag_actionSpeEvent(actor); break;
    case 10: Tag_actionMjHunt(actor); break;
    case 11: Tag_actionMjReady(actor); break;
    }
    return 1;
}
VERIFY(0x024A7BAC, Tag_Execute);

s32 Tag_IsDelete(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7C9C, s32, actor);
    return 1;
}
VERIFY(0x024A7C9C, Tag_IsDelete);

s32 Tag_Delete(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7CA4, s32, actor);
    return 1;
}
VERIFY(0x024A7CA4, Tag_Delete);

s32 Tag_Draw(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A81E0, s32, actor);
    return 1;
}
VERIFY(0x024A81E0, Tag_Draw);

void Tag_SafeStringDtor(void* object, s32 flags) {
    WWHD_FUNC(0x024A81E8, void, object, flags);
    if (object && (flags & 1)) gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x024A81E8, Tag_SafeStringDtor);

void Tag_ActorDtor(daTag_Event_c* actor, s32 flags) {
    WWHD_FUNC(0x024A81FC, void, actor, flags);
    if (actor) {
        gabi::call<void>(0x025D50BC, actor, 0);
        if (flags & 1) gabi::call<void>(0x0273AF40, actor);
    }
}
VERIFY(0x024A81FC, Tag_ActorDtor);

void Tag_SafeStringEmpty(void* object) {
    WWHD_FUNC(0x024A8250, void, object);
}
VERIFY(0x024A8250, Tag_SafeStringEmpty);

void Tag_SafeStringEmpty2(void* object) {
    WWHD_FUNC(0x024A8254, void, object);
}
VERIFY(0x024A8254, Tag_SafeStringEmpty2);



s32 Tag_actionHunt(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A68DC, s32, actor);
    u32 player = TagEvent::read<u32>(TagEvent::play() + 0x5B34);
    if (!player || Tag_huntBlocked(actor)) return 1;
    s32 bit = Tag_getSwbit(actor);
    if (TagEvent::huntRange(actor, player, bit, true)) {
        actor->mAction = 4;
        u32 number = Tag_getEventNo(actor);
        TagEvent::order(actor, number);
        if (Tag_cancelShutter(actor)) TagEvent::eventFlag();
    }
    return 1;
}
VERIFY(0x024A68DC, Tag_actionHunt);

s32 Tag_actionMjHunt(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A6A9C, s32, actor);
    u32 player = TagEvent::read<u32>(TagEvent::play() + 0x5B34);
    if (!player || Tag_huntBlocked(actor)) return 1;
    s32 bit = Tag_getSwbit(actor);
    if (TagEvent::huntRange(actor, player, bit, true)) {
        TagEvent::selectMjEvent(actor, 0x1003F03C);
        actor->mAction = 11;
        u32 number = Tag_getEventNo(actor);
        TagEvent::order(actor, number);
        if (Tag_cancelShutter(actor)) TagEvent::eventFlag();
    }
    return 1;
}
VERIFY(0x024A6A9C, Tag_actionMjHunt);

s32 Tag_actionArrival(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A6CF4, s32, actor);
    if (!Tag_arrivalTerms(actor)) return 1;
    switch (Tag_getType(actor)) {
    case 2:
        actor->mBkType = TagEvent::eventBit(4) ? 7 : 5;
        actor->mAction = 3;
        Tag_actionHunt2(actor);
        break;
    case 3:
        if (TagEvent::eventBit(0xE20) || !TagEvent::eventBit(0x101)) actor->mAction = 0;
        else { actor->mAction = 2; Tag_actionHunt(actor); }
        break;
    case 5:
        if (TagEvent::eventBit(0x1101)) actor->mAction = 0;
        else { actor->mAction = 2; Tag_actionHunt(actor); }
        break;
    case 9:
        if (TagEvent::eventBit(0xA02)) { actor->mAction = 2; Tag_actionHunt(actor); }
        else actor->mAction = 0;
        break;
    case 10:
        if (TagEvent::eventBit(0xE20)) { actor->mAction = 2; Tag_actionHunt(actor); }
        else actor->mAction = 0;
        break;
    case 11: actor->mAction = 10; Tag_actionMjHunt(actor); break;
    default: actor->mAction = 2; Tag_actionHunt(actor); break;
    }
    return 1;
}
VERIFY(0x024A6CF4, Tag_actionArrival);

s32 Tag_actionEvent(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A75B0, s32, actor);
    s32 index = actor->mEventIdx;
    u32 manager = TagEvent::play() + 0x52C4;
    if (gabi::call<s32>(0x025440C8, manager, index)) {
        actor->mAction = 0;
        u32 state = TagEvent::play();
        TagEvent::write<u16>(state + 0x52B8, TagEvent::read<u16>(state + 0x52B8) | 8);
        Tag_demoEndProc(actor);
    } else Tag_demoProc(actor);
    return 1;
}
VERIFY(0x024A75B0, Tag_actionEvent);

s32 Tag_actionReady(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7630, s32, actor);
    s32 bit = Tag_getSwbit(actor);
    if (TagEvent::read<u16>(gabi::ea(actor) + 0xF8) == 2) {
        Tag_demoInitProc(actor);
        actor->mAction = 5;
        Tag_actionEvent(actor);
        if (bit != 255) TagEvent::onSwitch(actor, bit);
    } else if (bit != 255 && TagEvent::isSwitch(actor, bit)) actor->mAction = 0;
    else {
        u32 number = Tag_getEventNo(actor);
        TagEvent::order(actor, number);
        if (Tag_cancelShutter(actor)) TagEvent::eventFlag();
    }
    return 1;
}
VERIFY(0x024A7630, Tag_actionReady);

s32 Tag_actionMjReady(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7744, s32, actor);
    s32 bit = Tag_getSwbit(actor);
    if (TagEvent::read<u16>(gabi::ea(actor) + 0xF8) == 2) {
        Tag_demoInitProc(actor);
        actor->mAction = 5;
        Tag_actionEvent(actor);
        if (bit != 255) TagEvent::onSwitch(actor, bit);
    } else if (bit != 255 && TagEvent::isSwitch(actor, bit)) actor->mAction = 0;
    else {
        s32 index = TagEvent::selectMjEvent(actor, 0x1003F05C);
        u32 number = Tag_getEventNo(actor);
        gabi::call<void>(0x025D7A58, actor, index, number, 65535, 0, 1);
        if (Tag_cancelShutter(actor)) TagEvent::eventFlag();
    }
    return 1;
}
VERIFY(0x024A7744, Tag_actionMjReady);

s32 Tag_actionSpeHunt(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A78E8, s32, actor);
    u32 player = TagEvent::read<u32>(TagEvent::play() + 0x5B34);
    if (!player || Tag_huntBlocked(actor)) return 1;
    if (TagEvent::huntRange(actor, player, 255, false)) {
        s32 index = actor->mEventIdx;
        actor->mAction = 8;
        gabi::call<void>(0x025D7A58, actor, index, 255, 65535, 0, 1);
    }
    return 1;
}
VERIFY(0x024A78E8, Tag_actionSpeHunt);

s32 Tag_actionSpeArrival(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A79F4, s32, actor);
    if (TagEvent::eventBit(0x1001)) { actor->mAction = 0; return 1; }
    s32 first = TagEvent::eventBit(0x1820);
    s32 second = TagEvent::eventBit(0x2740);
    if (!first && second) { actor->mAction = 0; return 1; }
    u32 manager = TagEvent::play() + 0x52C4;
    u32 name = !first ? 0x1003F088u : second ? 0x1003F068u : 0x1003F07Cu;
    actor->mEventIdx = gabi::call<s32>(0x02543F10, manager, name, 255);
    actor->mAction = 7;
    return Tag_actionSpeHunt(actor);
}
VERIFY(0x024A79F4, Tag_actionSpeArrival);

s32 Tag_actionSpeReady(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7B28, s32, actor);
    if (TagEvent::read<u16>(gabi::ea(actor) + 0xF8) == 2) {
        Tag_demoInitProc(actor);
        actor->mAction = 9;
        Tag_actionSpeEvent(actor);
        TagEvent::onEventBit(0x2740);
    } else TagEvent::order(actor, 255);
    return 1;
}
VERIFY(0x024A7B28, Tag_actionSpeReady);

void Tag_demoEndProc(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A721C, void, actor);
    switch (Tag_getType(actor)) {
    case 5: gabi::call<void>(0x025E1918, 0x80000019u); break;
    case 8: TagEvent::onEventBit(0x2502); break;
    case 9: TagEvent::onEventBit(0x2110); break;
    case 10: TagEvent::onEventBit(0x3202); break;
    case 11: TagEvent::onEventBit(0x3040); break;
    }
}
VERIFY(0x024A721C, Tag_demoEndProc);

void Tag_demoProc(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7318, void, actor);
    u32 type = Tag_getType(actor);
    if (type != 1 && type != 6 && type != 7 && type != 10) return;
    s32 timer = actor->mDemoTimer;
    if (timer < 400) { timer = (s16)(timer + 1); actor->mDemoTimer = timer; }
    else if (type == 7 || type == 10) return;
    if (type == 1 && timer == 53) gabi::call<void>(0x025E1988, 0x693C);
    else if (type == 1 && timer == 173) gabi::call<void>(0x025E1988, 0x693B);
    else if ((type == 1 && (timer == 222 || timer == 233)) || (type == 6 && timer == 227)) {
        u32 vibration = TagEvent::play() + 0x599C;
        TagEvent::Local<cXyz> direction;
        direction.get()->x = 0.0f;
        direction.get()->y = 1.0f;
        direction.get()->z = 0.0f;
        gabi::call<void>(0x025CB374, vibration, 4, -33, direction.get());
    } else if (type == 7 && timer == 105) {
        u32 dragon = gabi::call<u32>(0x025D9F38, STR(0x1003F054), 0, 0);
        if (dragon) TagEvent::write<u8>(dragon + 0x3E4, 1);
    } else if (type == 10 && timer == 47) gabi::call<void>(0x025E1988, 0x8FC);
}
VERIFY(0x024A7318, Tag_demoProc);



void Tag_demoInitProc(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A6E94, void, actor);
    u32 entrySP = gabi::cpu->r[1];
    u32 entryLR = gabi::cpu->lr;
    u32 saved[4] = {gabi::cpu->r[28], gabi::cpu->r[29], gabi::cpu->r[30], gabi::cpu->r[31]};
    TagEvent::Local<TagEvent::PasswordFrame_l> frame;
    frame->backchain = entrySP;
    for (u32 i = 0; i < 4; ++i) frame->saved[i] = saved[i];
    TagEvent::write<u32>(entrySP + 4, entryLR);
    actor->mDemoTimer = 0;
    switch (Tag_getType(actor)) {
    case 1: {
        u32 partner = Tag_getBk(5);
        TagEvent::setPartners(partner);
        TagEvent::onEventBit(4);
        gabi::call<void>(0x025E18EC, 0x80000013u);
        break;
    }
    case 2: {
        u32 bk = Tag_getBk(4);
        if (bk) TagEvent::write<u8>(bk + 0x137C, 1);
        TagEvent::onEventBit(0x101);
        break;
    }
    case 3: TagEvent::onEventBit(0xE20); break;
    case 5: gabi::call<void>(0x025E1960, 30); break;
    case 6: {
        u32 partner = gabi::call<u32>(0x025D9F38, STR(0x1003F04C), 0, 0);
        TagEvent::setPartners(partner);
        break;
    }
    case 9: {
        u32 passwordIndex;
        if (!TagEvent::eventBit(0x3B20)) {
            TagEvent::onEventBit(0x3B20);
            f32 random = gabi::call<f32>(0x020198D8, 6.0f);
            passwordIndex = (u8)gabi::ftoi(random);
            gabi::call<void>(0x025B8AF4, TagEvent::save() + 0x644, 0xBA0F, passwordIndex);
        } else passwordIndex = gabi::call<u8>(0x025B8BB0, TagEvent::save() + 0x644, 0xBA0F);
        u32 message = TagEvent::read<u32>(0x101F4B5C);
        gabi::call<void>(0x025F8A08, message, frame->password, passwordIndex + 0x1B37);
        u32 input = TagEvent::play() + 0x12A0;
        TagEvent::SafeString_l* text = &frame->text;
        text->text = gabi::ea(frame->password);
        text->vtable = 0x1003F008;
        u32 destination = TagEvent::read<u32>(input + 0x4950);
        TagEvent::assure(text);
        u32 source = text->text;
        s32 length = 0;
        while (TagEvent::read<u16>(source + (u32)length * 2) != 0) {
            ++length;
            if (length > 0x40000) { length = 0; break; }
        }
        s32 capacity = TagEvent::read<s32>(input + 0x4958);
        if (length >= capacity) length = (s32)((u32)capacity - 1);
        TagEvent::assure(text);
        source = text->text;
        gabi::call<void>(0xC0009988, destination, source, (u32)length * 2, 0);
        TagEvent::write<u16>(destination + (u32)length * 2, 0);
        break;
    }
    }
}
VERIFY(0x024A6E94, Tag_demoInitProc);



s32 Tag_Create(daTag_Event_c* actor) {
    WWHD_FUNC(0x024A7CAC, s32, actor);
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            gabi::call<void>(0x025D4ED0, actor);
            condition = actor->actor_condition;
            actor->__vtbl = 0x1003F020;
        }
        actor->actor_condition = condition | 8;
    }
    s32 bit = Tag_getSwbit(actor);
    u32 number = Tag_getEventNo(actor);
    u32 manager = TagEvent::play() + 0x52C4;
    s32 index = gabi::call<s32>(0x02543F10, manager, 0, number);
    actor->mEventIdx = index;
    TagEvent::write<u16>(gabi::ea(actor) + 0xFC, index);
    number = Tag_getEventNo(actor);
    TagEvent::write<u8>(gabi::ea(actor) + 0xFE, number);
    if (Tag_getType(actor) == 13) actor->mAction = 6;
    else if ((s16)actor->mEventIdx != -1 && (bit == 255 || !TagEvent::isSwitch(actor, bit)))
        actor->mAction = 1;
    else actor->mAction = 0;
    actor->shape_angle.x = 0;
    actor->shape_angle.z = 0;
    actor->current.angle.x = 0;
    actor->current.angle.z = 0;
    TagEvent::Local<TagEvent::SafeString_l> name;
    TagEvent::Local<TagEvent::SafeString_l> stage;
    name.get()->text = 0x1003F094;
    name.get()->vtable = 0x1003EFF0;
    u32 state = TagEvent::play();
    stage.get()->vtable = 0x1003EFF0;
    stage.get()->text = state + 0x5134;
    bool dispatch = TagEvent::stageEquals(name.get(), stage.get());
    if (!dispatch) {
        TagEvent::Local<TagEvent::SafeString_l> secondName;
        TagEvent::Local<TagEvent::SafeString_l> secondStage;
        secondName.get()->vtable = 0x1003EFF0;
        secondName.get()->text = 0x1003F09C;
        state = TagEvent::play();
        secondStage.get()->vtable = 0x1003EFF0;
        secondStage.get()->text = state + 0x5134;
        dispatch = TagEvent::stageEquals(secondName.get(), secondStage.get());
    }
    if (dispatch) TagEvent::createDispatch(actor);
    return 4;
}
VERIFY(0x024A7CAC, Tag_Create);

void Tag_staticInit() {
    WWHD_FUNC(0x024A814C, void);
    TagEvent::write<u32>(0x1046E294, 0);
    TagEvent::write<u32>(0x1046E28C, 0);
    TagEvent::write<u32>(0x1046E298, 0);
    TagEvent::write<u32>(0x1046E290, 0);
    gabi::call<void>(0x028F026C, 0x101D18F8u);
    TagEvent::write<f32>(0x1046E280, -3.1415927410125732f);
    TagEvent::write<f32>(0x1046E284, 3.1415927410125732f);
    gabi::call<void>(0x028ED6F8, 0x1046E288u);
    gabi::call<void>(0x028F026C, 0x101D1904u);
    gabi::call<void>(0x028EAB2C, 0x1046E289u);
    gabi::call<void>(0x028F026C, 0x101D1910u);
}
VERIFY(0x024A814C, Tag_staticInit);
