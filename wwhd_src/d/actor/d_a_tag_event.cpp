/* Event trigger tag. GameCube layout and actions with HD arrival and password handling.
 * Verified against the private WWHD executable. */
#include "d/actor/d_a_tag_event.h"
#include "bindings.h"
#include "gc_api/tu.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1003EFF0,
    .safeStringAssure = 0x024A8250,
    .headerStatics = {0x1046E280, 0x101D18F8, 0x1046E28C},
};
static const u32 lWideStringVtable = 0x1003F008;
static const u32 lActorVtable = 0x1003F020;

static u32 lScalar4 = 0;
static u32 lScalar2 = 0;
static u32 lScalar5 = 0;
static u32 lScalar3 = 0;
static f32 lAngleMin = 0;
static f32 lAngleMax = 0;
namespace TagEvent {
// Keep every address-passed payload above outgoing EABI linkage (caller SP+4).
template<class T> struct Local : gabi::Local<T> {
    gabi::Local<be<u8>[16]> linkage;
};
u32 save() { return TagEvent_saveAddress(); }
u32 play() { return TagEvent_getGameInfo(); }
s32 isSwitch(daTag_Event_c* actor, s32 bit) {
    u32 info = save() + 0x20;
    s32 room = actor->current.roomNo;
    return TagEvent_isSwitch(info, bit, room);
}
void onSwitch(daTag_Event_c* actor, s32 bit) {
    u32 info = save() + 0x20;
    s32 room = actor->current.roomNo;
    TagEvent_onSwitch(info, bit, room);
}
s32 eventBit(u32 bit) { return TagEvent_isEventBit(save() + 0x644, bit); }
void onEventBit(u32 bit) { TagEvent_onEventBit(save() + 0x644, bit); }
void eventFlag() {
    u32 state = play();
    TagEvent_game(state)->mEventFlag |= 1;
}
}


namespace TagEvent {
void order(daTag_Event_c* actor, u32 number) {
    s32 index = actor->mEventIdx;
    TagEvent_orderOtherEventId(actor, index, number, 65535, 0, 1);
}
s32 selectMjEvent(daTag_Event_c* actor, u32 fallback) {
    u32 state = play();
    u32 ship = (u32)TagEvent_game(state)->mPlayerStatus & 0x10000;
    u32 number = actor->getEventNo();
    state = play();
    s32 index = TagEvent_getEventIdx(state + 0x52C4,
                              ship ? 0u : fallback, number);
    actor->mEventIdx = index;
    return index;
}
bool huntRange(daTag_Event_c* actor, u32 player, s32 bit, bool useSwitch) {
    Local<cXyz> difference;
    TagEvent_subtract(gabi::at<cXyz>(player + 0x314),
                     difference.get(), &actor->current.pos);
    f32 height = difference.get()->y;
    f32 x = difference.get()->x;
    f32 z = difference.get()->z;
    if (height < 0.0f) height = -height;
    if (useSwitch && bit != 255 && isSwitch(actor, bit)) {
        actor->mAction = daTag_Event_c::ACTION_WAIT;
        return false;
    }
    Local<cXyz> horizontal;
    horizontal.get()->y = 0.0f;
    horizontal.get()->z = z;
    horizontal.get()->x = x;
    f32 distance = PSVECSquareMag(horizontal.get());
    f32 radius = actor->scale.x;
    f32 radiusSquared = radius * radius;
    if (!(distance < radiusSquared * 10000.0f)) return false;
    f32 limit = actor->scale.y;
    if (height > limit * 100.0f) return false;
    return true;
}
}

namespace TagEvent {
void assure(TagSafeString* text) {
    u32 target = gabi::at<TagStringVtable>(text->vtable)->mAssureTermination;
    gabi::call_ptr<void>(target, text);
}
void setPartners(u32 partner) {
    u32 control = play() + 0x51D0;
    u32 pid = TagEvent_getPId(control, partner);
    gabi::at<TagEventControl>(control)->mPtItem = pid;
    control = play() + 0x51D0;
    pid = TagEvent_getPId(control, partner);
    gabi::at<TagEventControl>(control)->mPtTalk = pid;
}
}

namespace TagEvent {
bool stageEquals(TagSafeString* name, TagSafeString* stage) {
    assure(name);
    assure(name);
    u32 left = name->text;
    assure(stage);
    u32 right = stage->text;
    if (left == right) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = TagEvent_bytes(left)[i];
        u8 b = TagEvent_bytes(right)[i];
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}
void createDispatch(daTag_Event_c* actor) {
    switch ((u8)actor->mAction) {
    case daTag_Event_c::ACTION_ARRIVAL: actor->actionArrival(); break;
    case daTag_Event_c::ACTION_HUNT: actor->actionHunt(); break;
    case daTag_Event_c::ACTION_HUNT2: actor->actionHunt2(); break;
    case daTag_Event_c::ACTION_READY: actor->actionReady(); break;
    case daTag_Event_c::ACTION_EVENT: actor->actionEvent(); break;
    case daTag_Event_c::ACTION_SPE_ARRIVAL: actor->actionSpeArrival(); break;
    case daTag_Event_c::ACTION_SPE_HUNT: actor->actionSpeHunt(); break;
    case daTag_Event_c::ACTION_SPE_READY: actor->actionSpeReady(); break;
    case daTag_Event_c::ACTION_SPE_EVENT: actor->actionSpeEvent(); break;
    case daTag_Event_c::ACTION_MJ_HUNT: actor->actionMjHunt(); break;
    case daTag_Event_c::ACTION_MJ_READY: actor->actionMjReady(); break;
    }
}
}


/* 024A65C4 */
u32 daTag_getBk(u32 type) {
    return TagEvent_searchFromName("Bk", 15, type);
}

/* 024A66EC */
u8 daTag_Event_c::getEventNo() {
    return (u32)mParameters >> 24;
}

/* 024A66E0 */
u8 daTag_Event_c::getSwbit() {
    return (u32)mParameters >> 8;
}

/* 024A65D8 */
u8 daTag_Event_c::getSwbit2() {
    return (u32)mParameters >> 16;
}

/* 024A65E4 */
u8 daTag_Event_c::getType() {
    return (u32)mParameters;
}

/* 024A65F0 */
u16 daTag_Event_c::getEventFlag() {
    return home.angle.z;
}

/* 024A688C */
s32 daTag_Event_c::cancelShutter() {
    u32 type = getType();
    return type == 4 || type == 7 || type == 12;
}

/* 024A65F8 */
s32 daTag_Event_c::arrivalTerms() {
    s32 bit = getSwbit2();
    if (getType() == 12) {
        if (bit != 255 && TagEvent::isSwitch(this, bit)) return 0;
        return 1;
    }
    u16 flag = getEventFlag();
    if (bit != 255 && !TagEvent::isSwitch(this, bit)) return 0;
    if (flag != 65535 && flag != 0 && !TagEvent::eventBit(flag)) return 0;
    return 1;
}

/* 024A6E94 */
void daTag_Event_c::demoInitProc() {
    // HD: UTF-16 password lookup may leave the buffer untouched and scan saved registers.
    u32 entrySP = gabi::cpu->r[1];
    u32 entryLR = gabi::cpu->lr;
    u32 saved[4] = {gabi::cpu->r[28], gabi::cpu->r[29], gabi::cpu->r[30], gabi::cpu->r[31]};
    TagEvent::Local<TagEvent::TagPasswordFrame> frame;
    frame->backchain = entrySP;
    for (u32 i = 0; i < 4; ++i) frame->saved[i] = saved[i];
    gabi::at<TagCallerLinkage>(entrySP)->mSavedLinkRegister = entryLR;
    mDemoTimer = 0;
    switch (getType()) {
    case 1: {
        u32 partner = daTag_getBk(5);
        TagEvent::setPartners(partner);
        TagEvent::onEventBit(4);
        mDoAud_bgmStart(0x80000013u);
        break;
    }
    case 2: {
        u32 bk = daTag_getBk(4);
        if (bk) gabi::at<TagBokoblin>(bk)->mDemoMode = 1;
        TagEvent::onEventBit(0x101);
        break;
    }
    case 3: TagEvent::onEventBit(0xE20); break;
    case 5: mDoAud_bgmAllMute(30); break;
    case 6: {
        u32 partner = TagEvent_searchFromName("mo2", 0, 0);
        TagEvent::setPartners(partner);
        break;
    }
    case 9: {
        u32 passwordIndex;
        if (!TagEvent::eventBit(0x3B20)) {
            TagEvent::onEventBit(0x3B20);
            f32 random = TagEvent_rndF(6.0f);
            passwordIndex = (u8)gabi::ftoi(random);
            TagEvent_setEventReg(TagEvent::save() + 0x644, 0xBA0F, passwordIndex);
        } else passwordIndex = TagEvent_getEventReg(TagEvent::save() + 0x644, 0xBA0F);
        u32 message = TagEvent_messageAddress();
        TagEvent_passwordGet(message, frame->password, passwordIndex + 0x1B37);
        u32 input = TagEvent::play() + 0x12A0;
        TagEvent::TagSafeString* text = &frame->text;
        text->text = gabi::ea(frame->password);
        text->vtable = lWideStringVtable;
        u32 destination = (u32)TagEvent_game(input - 0x12A0)->mInputPassword;
        TagEvent::assure(text);
        u32 source = text->text;
        s32 length = 0;
        while (TagEvent_chars(source)[(u32)length] != 0) {
            ++length;
            if (length > 0x40000) { length = 0; break; }
        }
        s32 capacity = (s32)TagEvent_game(input - 0x12A0)->mInputPasswordCapacity;
        if (length >= capacity) length = (s32)((u32)capacity - 1);
        TagEvent::assure(text);
        source = text->text;
        TagEvent_memcpy(destination, source, (u32)length * 2, 0);
        TagEvent_chars(destination)[(u32)length] = 0;
        break;
    }
    }
}

/* 024A721C */
void daTag_Event_c::demoEndProc() {
    switch (getType()) {
    case 5: mDoAud_subBgmStart(0x80000019u); break;
    case 8: TagEvent::onEventBit(0x2502); break;
    case 9: TagEvent::onEventBit(0x2110); break;
    case 10: TagEvent::onEventBit(0x3202); break;
    case 11: TagEvent::onEventBit(0x3040); break;
    }
}

/* 024A7318 */
void daTag_Event_c::demoProc() {
    u32 type = getType();
    if (type != 1 && type != 6 && type != 7 && type != 10) return;
    s32 timer = mDemoTimer;
    if (timer < 400) { timer = (s16)(timer + 1); mDemoTimer = timer; }
    else if (type == 7 || type == 10) return;
    if (type == 1 && timer == 53) mDoAud_seStart(0x693C);
    else if (type == 1 && timer == 173) mDoAud_seStart(0x693B);
    else if ((type == 1 && (timer == 222 || timer == 233)) || (type == 6 && timer == 227)) {
        u32 vibration = TagEvent::play() + 0x599C;
        TagEvent::Local<cXyz> direction;
        direction.get()->x = 0.0f;
        direction.get()->y = 1.0f;
        direction.get()->z = 0.0f;
        TagEvent_StartShock(vibration, 4, -33, direction.get());
    } else if (type == 7 && timer == 105) {
        u32 dragon = TagEvent_searchFromName("dragon", 0, 0);
        if (dragon) gabi::at<TagDragon>(dragon)->mQuakeInfo = 1;
    } else if (type == 10 && timer == 47) mDoAud_seStart(0x8FC);
}

/* 024A75B0 */
s32 daTag_Event_c::actionEvent() {
    s32 index = mEventIdx;
    u32 manager = TagEvent::play() + 0x52C4;
    if (TagEvent_endCheck(manager, index)) {
        mAction = daTag_Event_c::ACTION_WAIT;
        u32 state = TagEvent::play();
        TagEvent_game(state)->mEventFlag |= 8;
        demoEndProc();
    } else demoProc();
    return 1;
}

/* 024A7630 */
s32 daTag_Event_c::actionReady() {
    s32 bit = getSwbit();
    if (eventInfo.checkCommandDemoAccrpt()) {
        demoInitProc();
        mAction = daTag_Event_c::ACTION_EVENT;
        actionEvent();
        if (bit != 255) TagEvent::onSwitch(this, bit);
    } else if (bit != 255 && TagEvent::isSwitch(this, bit)) mAction = daTag_Event_c::ACTION_WAIT;
    else {
        u32 number = getEventNo();
        TagEvent::order(this, number);
        if (cancelShutter()) TagEvent::eventFlag();
    }
    return 1;
}

/* 024A68DC */
s32 daTag_Event_c::actionHunt() {
    // HD: the Link pointer may be absent during arrival.
    u32 player = TagEvent_game(TagEvent::play())->mLinkPlayer;
    if (!player || huntBlocked()) return 1;
    s32 bit = getSwbit();
    if (TagEvent::huntRange(this, player, bit, true)) {
        mAction = daTag_Event_c::ACTION_READY;
        u32 number = getEventNo();
        TagEvent::order(this, number);
        if (cancelShutter()) TagEvent::eventFlag();
    }
    return 1;
}

/* 024A6CF4 */
s32 daTag_Event_c::actionArrival() {
    if (!arrivalTerms()) return 1;
    switch (getType()) {
    case 2:
        mBkType = TagEvent::eventBit(4) ? 7 : 5;
        mAction = daTag_Event_c::ACTION_HUNT2;
        actionHunt2();
        break;
    case 3:
        if (TagEvent::eventBit(0xE20) || !TagEvent::eventBit(0x101)) mAction = daTag_Event_c::ACTION_WAIT;
        else { mAction = daTag_Event_c::ACTION_HUNT; actionHunt(); }
        break;
    case 5:
        if (TagEvent::eventBit(0x1101)) mAction = daTag_Event_c::ACTION_WAIT;
        else { mAction = daTag_Event_c::ACTION_HUNT; actionHunt(); }
        break;
    case 9:
        if (TagEvent::eventBit(0xA02)) { mAction = daTag_Event_c::ACTION_HUNT; actionHunt(); }
        else mAction = daTag_Event_c::ACTION_WAIT;
        break;
    case 10:
        if (TagEvent::eventBit(0xE20)) { mAction = daTag_Event_c::ACTION_HUNT; actionHunt(); }
        else mAction = daTag_Event_c::ACTION_WAIT;
        break;
    case 11: mAction = daTag_Event_c::ACTION_MJ_HUNT; actionMjHunt(); break;
    default: mAction = daTag_Event_c::ACTION_HUNT; actionHunt(); break;
    }
    return 1;
}

/* 024A7B24 */
s32 daTag_Event_c::actionSpeEvent() {
    return actionEvent();
}

/* 024A7B28 */
s32 daTag_Event_c::actionSpeReady() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        demoInitProc();
        mAction = daTag_Event_c::ACTION_SPE_EVENT;
        actionSpeEvent();
        TagEvent::onEventBit(0x2740);
    } else TagEvent::order(this, 255);
    return 1;
}

/* 024A78E8 */
s32 daTag_Event_c::actionSpeHunt() {
    // HD: the Link pointer may be absent during arrival.
    u32 player = TagEvent_game(TagEvent::play())->mLinkPlayer;
    if (!player || huntBlocked()) return 1;
    if (TagEvent::huntRange(this, player, 255, false)) {
        s32 index = mEventIdx;
        mAction = daTag_Event_c::ACTION_SPE_READY;
        TagEvent_orderOtherEventId(this, index, 255, 65535, 0, 1);
    }
    return 1;
}

/* 024A79F4 */
s32 daTag_Event_c::actionSpeArrival() {
    if (TagEvent::eventBit(0x1001)) { mAction = daTag_Event_c::ACTION_WAIT; return 1; }
    s32 first = TagEvent::eventBit(0x1820);
    s32 second = TagEvent::eventBit(0x2740);
    if (!first && second) { mAction = daTag_Event_c::ACTION_WAIT; return 1; }
    u32 manager = TagEvent::play() + 0x52C4;
    u32 name = !first ? gabi::guest_addr("SUPERELF") : second ? gabi::guest_addr("fairy_flag_on_Jmp") : gabi::guest_addr("fairy_Jmp");
    mEventIdx = TagEvent_getEventIdx(manager, name, 255);
    mAction = daTag_Event_c::ACTION_SPE_HUNT;
    return actionSpeHunt();
}

/* 024A7744 */
s32 daTag_Event_c::actionMjReady() {
    s32 bit = getSwbit();
    if (eventInfo.checkCommandDemoAccrpt()) {
        demoInitProc();
        mAction = daTag_Event_c::ACTION_EVENT;
        actionEvent();
        if (bit != 255) TagEvent::onSwitch(this, bit);
    } else if (bit != 255 && TagEvent::isSwitch(this, bit)) mAction = daTag_Event_c::ACTION_WAIT;
    else {
        s32 index = TagEvent::selectMjEvent(this, gabi::guest_addr("BEAST_GATE2"));
        u32 number = getEventNo();
        TagEvent_orderOtherEventId(this, index, number, 65535, 0, 1);
        if (cancelShutter()) TagEvent::eventFlag();
    }
    return 1;
}

/* 024A6A9C */
s32 daTag_Event_c::actionMjHunt() {
    // HD: the Link pointer may be absent during arrival.
    u32 player = TagEvent_game(TagEvent::play())->mLinkPlayer;
    if (!player || huntBlocked()) return 1;
    s32 bit = getSwbit();
    if (TagEvent::huntRange(this, player, bit, true)) {
        TagEvent::selectMjEvent(this, gabi::guest_addr("BEAST_GATE2"));
        mAction = daTag_Event_c::ACTION_MJ_READY;
        u32 number = getEventNo();
        TagEvent::order(this, number);
        if (cancelShutter()) TagEvent::eventFlag();
    }
    return 1;
}

/* 024A66F8 */
s32 daTag_Event_c::actionHunt2() {
    s32 bit = getSwbit();
    if (bit != 255 && TagEvent::isSwitch(this, bit)) {
        mAction = daTag_Event_c::ACTION_WAIT;
    } else if (daTag_getBk(mBkType) == 0) {
        s32 timer = mHuntTimer;
        if (timer > 0) mHuntTimer = timer - 1;
        else {
            mAction = daTag_Event_c::ACTION_READY;
            u32 number = getEventNo();
            s32 index = mEventIdx;
            TagEvent_orderOtherEventId(this, index, number, 65535, 0, 1);
        }
    } else mHuntTimer = 65;
    return 1;
}

/* 024A67D0 */
s32 daTag_Event_c::huntBlocked() {
    // HD: delay proximity triggers while the arrival counter is inactive.
    u32 delay = mArrivalDelay;
    if (delay != 0) { mArrivalDelay = delay + 1; return 1; }
    u32 state = TagEvent::play();
    s32 change = gabi::ftoi((f32)TagEvent_game(state)->mArrivalChange);
    state = TagEvent::play();
    u32 count = (u16)TagEvent_game(state)->mArrivalCount;
    if ((s32)(count + (u32)change) > 0) return 0;
    if (change < 0) mArrivalDelay = (u32)mArrivalDelay + (u32)change;
    return 1;
}

/* 024A81E0 */
s32 daTag_Event_Draw(daTag_Event_c* actor) {
    return 1;
}

/* 024A7BAC */
s32 daTag_Event_Execute(daTag_Event_c* actor) {
    switch ((u8)actor->mAction) {
    case daTag_Event_c::ACTION_ARRIVAL: actor->actionArrival(); break;
    case daTag_Event_c::ACTION_HUNT: actor->actionHunt(); break;
    case daTag_Event_c::ACTION_HUNT2: actor->actionHunt2(); break;
    case daTag_Event_c::ACTION_READY: actor->actionReady(); break;
    case daTag_Event_c::ACTION_EVENT: actor->actionEvent(); break;
    case daTag_Event_c::ACTION_SPE_ARRIVAL: actor->actionSpeArrival(); break;
    case daTag_Event_c::ACTION_SPE_HUNT: actor->actionSpeHunt(); break;
    case daTag_Event_c::ACTION_SPE_READY: actor->actionSpeReady(); break;
    case daTag_Event_c::ACTION_SPE_EVENT: actor->actionSpeEvent(); break;
    case daTag_Event_c::ACTION_MJ_HUNT: actor->actionMjHunt(); break;
    case daTag_Event_c::ACTION_MJ_READY: actor->actionMjReady(); break;
    }
    return 1;
}

/* 024A7C9C */
s32 daTag_Event_IsDelete(daTag_Event_c* actor) {
    return 1;
}

/* 024A7CA4 */
s32 daTag_Event_Delete(daTag_Event_c* actor) {
    return 1;
}

/* 024A7CAC */
s32 daTag_Event_Create(daTag_Event_c* actor) {
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            TagEvent_actorCtor(actor);
            condition = actor->actor_condition;
            actor->__vtbl = lActorVtable;
        }
        actor->actor_condition = condition | 8;
    }
    s32 bit = actor->getSwbit();
    u32 number = actor->getEventNo();
    u32 manager = TagEvent::play() + 0x52C4;
    s32 index = TagEvent_getEventIdx(manager, 0, number);
    actor->mEventIdx = index;
    actor->eventInfo.mEventId = index;
    number = actor->getEventNo();
    actor->eventInfo.mMapToolId = number;
    if (actor->getType() == 13) actor->mAction = daTag_Event_c::ACTION_SPE_ARRIVAL;
    else if ((s16)actor->mEventIdx != -1 && (bit == 255 || !TagEvent::isSwitch(actor, bit)))
        actor->mAction = daTag_Event_c::ACTION_ARRIVAL;
    else actor->mAction = daTag_Event_c::ACTION_WAIT;
    actor->shape_angle.x = 0;
    actor->shape_angle.z = 0;
    actor->current.angle.x = 0;
    actor->current.angle.z = 0;
    // HD: dispatch immediately in the two designated stages.
    TagEvent::Local<TagEvent::TagSafeString> name;
    TagEvent::Local<TagEvent::TagSafeString> stage;
    name.get()->text = gabi::guest_addr("Atorizk");
    name.get()->vtable = gc_tu.safeString;
    u32 state = TagEvent::play();
    stage.get()->vtable = gc_tu.safeString;
    stage.get()->text = state + 0x5134;
    bool dispatch = TagEvent::stageEquals(name.get(), stage.get());
    if (!dispatch) {
        TagEvent::Local<TagEvent::TagSafeString> secondName;
        TagEvent::Local<TagEvent::TagSafeString> secondStage;
        secondName.get()->vtable = gc_tu.safeString;
        secondName.get()->text = gabi::guest_addr("kenroom");
        state = TagEvent::play();
        secondStage.get()->vtable = gc_tu.safeString;
        secondStage.get()->text = state + 0x5134;
        dispatch = TagEvent::stageEquals(secondName.get(), secondStage.get());
    }
    if (dispatch) TagEvent::createDispatch(actor);
    return 4;
}

// HD: compiler-generated and template code of this translation unit.

/* 024A814C */
void __sinit_d_a_tag_event_cpp() {
    lScalar4 = 0;
    lScalar2 = 0;
    lScalar5 = 0;
    lScalar3 = 0;
    TagEvent_registerGlobalObject(gc_tu.headerStatics[1]);
    lAngleMin = -3.1415927410125732f;
    lAngleMax = 3.1415927410125732f;
    TagEvent_scalarCtor(gc_tu.headerStatics[0] + 8);
    TagEvent_registerGlobalObject(gc_tu.headerStatics[1] + 12);
    TagEvent_scalarCtor2(gc_tu.headerStatics[0] + 9);
    TagEvent_registerGlobalObject(gc_tu.headerStatics[1] + 24);
}

/* 024A81E8 */
void TagSafeString_dt(void* object, s32 flags) {
    if (object && (flags & 1)) TagEvent_delete(object);
}

/* 024A81FC */
void daTag_Event_c_dt(daTag_Event_c* actor, s32 flags) {
    if (actor) {
        TagEvent_actorDtor(actor, 0);
        if (flags & 1) TagEvent_delete(actor);
    }
}

/* 024A8250 */
void TagSafeString_assureTermination(void* object) {
}

/* 024A8254 */
void TagSafeString16_assureTermination(void* object) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1003F03C "BEAST_GATE2" in 024A6A9C
str 1003F05C "BEAST_GATE2" in 024A7744
str 1003F088 "SUPERELF"
str 1003F068 "fairy_flag_on_Jmp"
str 1003F07C "fairy_Jmp"
str 1003F094 "Atorizk"
str 1003F09C "kenroom"
str 1003F054 "dragon"
str 1003F04C "mo2"
str 1003EFE8 "Bk"
data lAngleMax 1046E284 nocheck # HD BSS, initialized by __sinit
data lAngleMin 1046E280 nocheck # HD BSS, initialized by __sinit
data lScalar3 1046E290 nocheck # HD BSS, initialized by __sinit
data lScalar5 1046E298 nocheck # HD BSS, initialized by __sinit
data lScalar2 1046E28C nocheck # HD BSS, initialized by __sinit
data lScalar4 1046E294 nocheck # HD BSS, initialized by __sinit
ext 025200D4 u32 TagEvent_getGameInfo()
ext 025BA0C0 s32 TagEvent_isSwitch(u32, s32, s32)
ext 025B9E38 void TagEvent_onSwitch(u32, s32, s32)
ext 025B8B94 s32 TagEvent_isEventBit(u32, u32)
ext 025B8B68 void TagEvent_onEventBit(u32, u32)
ext 025D7A58 void TagEvent_orderOtherEventId(daTag_Event_c*, s32, u32, u32, s32, s32)
ext 02543F10 s32 TagEvent_getEventIdx(u32, u32, u32)
ext 0201ADE0 void TagEvent_subtract(const cXyz*, cXyz*, const cXyz*)
ext 028E8DD0 f32 PSVECSquareMag(const cXyz*)
ext 0253F124 u32 TagEvent_getPId(u32, u32)
ext 025D9F38 u32 TagEvent_searchFromName(const char*, u32, u32)
ext 025440C8 s32 TagEvent_endCheck(u32, s32)
ext 0273AF40 void TagEvent_delete(void*)
ext 025D50BC void TagEvent_actorDtor(daTag_Event_c*, s32)
ext 025E1918 void mDoAud_subBgmStart(u32)
ext 025E1988 void mDoAud_seStart(u32)
ext 025CB374 void TagEvent_StartShock(u32, s32, s32, cXyz*)
ext 025E18EC void mDoAud_bgmStart(u32)
ext 025E1960 void mDoAud_bgmAllMute(s32)
ext 020198D8 f32 TagEvent_rndF(f32)
ext 025B8AF4 void TagEvent_setEventReg(u32, u32, u32)
ext 025B8BB0 u8 TagEvent_getEventReg(u32, u32)
ext 025F8A08 void TagEvent_passwordGet(u32, be<u16>*, u32)
ext C0009988 void TagEvent_memcpy(u32, u32, u32, u32)
ext 025D4ED0 void TagEvent_actorCtor(daTag_Event_c*)
ext 028F026C void TagEvent_registerGlobalObject(u32)
ext 028ED6F8 void TagEvent_scalarCtor(u32)
ext 028EAB2C void TagEvent_scalarCtor2(u32)
*/
