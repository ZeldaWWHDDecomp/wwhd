/* Event control and actor event information, ported from tww.
 * HD: four item buttons, trailing event-info vtable and retained register-width semantics.
 * Verified against the private WWHD reference. */
#include "d/d_event_readable.h"

// HD: bindings retain the verified argument registers where the shared API differs.
/* dEvent_manager_c (play + 0x52C4) */
static inline EventManager* evmng() {
    return reinterpret_cast<EventManager*>(dComIfGp_getPEvtManager());
}
static inline dComIfG_inf_c* play() { return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get()); }

static inline EventStageInfo* stage_getEventInfo() {
    EventStageData* stage = reinterpret_cast<EventStageData*>(&play()->play.mStageData);
    return gabi::call_ptr<EventStageInfo*>(static_cast<u32>(stage->methods->getEventInfo), stage);
}

static const u8 l_itemButton[] = {0, 1, 2};
static u32 l_demoMode = 0;

static inline void end_partners(EventControl* control) {
    u32 a1 = control->convPId(static_cast<u32>(control->mPt1));
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    if (a1 != 0)
        actor1->eventInfo.mCommand = dEvtCmd_NONE_e;
    u32 a2 = control->convPId(static_cast<u32>(control->mPt2));
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if (a2 != 0)
        actor2->eventInfo.mCommand = dEvtCmd_NONE_e;
    s16 id = static_cast<s16>(control->mEventId);
    if (id != -1) {
        evmng()->endProc((u32)(s32)id, 1);
        control->mEventId = static_cast<u16>(0xFFFF);
    }
}

// HD: value-initialises the members first
/* 0253EAB8 */
static u32 dEvt_control_c_ct(u32 i_this) {
    if (i_this == 0) {
        i_this = gabi::ea(operator_new(sizeof(EventControl)));
        if (i_this == 0)
            return 0;
    }
    EventControl* self = gabi::at<EventControl>(i_this);
    self->field_0xdc = static_cast<u8>(0);
    self->mCullFarClipRatio = static_cast<f32>(0.0f);
    self->mEventEndSound = static_cast<u8>(0);
    self->mItemNo = static_cast<u8>(0);
    self->mbEndProc = static_cast<u8>(0);
    self->field_0xdb = static_cast<u8>(0);
    self->mPtItem = static_cast<u32>(0);
    self->mEventId = static_cast<u16>(0);
    self->mMode = static_cast<u8>(dEvtMode_NONE_e);
    self->mOrderCount = static_cast<u8>(0);
    self->mTalkButton = static_cast<u8>(0);
    self->mPt1 = static_cast<u32>(0);
    self->mEventInfoIdx = static_cast<u8>(0);
    self->mDebugStb = static_cast<u8>(0);
    self->mPtTalk = static_cast<u32>(0);
    self->mHindFlag = static_cast<u16>(0);
    self->mEventFlag = static_cast<u16>(0);
    self->mbInPhoto = static_cast<u8>(0);
    self->mGetItemNo = static_cast<u8>(0);
    self->mPt2 = static_cast<u32>(0);
    self->mFirstOrderIdx = static_cast<u8>(0);
    self->field_0xde = static_cast<u8>(0);
    bzero(&self->mTactFreeMStick[0], u32(0xA));
    self->remove();
    return i_this;
}

/* 0253ED80 */
s32 EventControl::orderOld(u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                           u32 eventName) {
    u32 i_this = gabi::ea(this);
    u32 idx = evmng()->getEventIdx(gabi::at<char>(eventName), 0xFF);
    return gabi::call<s32>(0x0253EC0C, i_this, eventType, priority, flag, hindFlag, ac1, ac2, idx,
                           0xFFu); /* order */
}

// HD: non-priority-1 orders require item life; a priority-1 head prevents insertion
// after the new slot is written. The GameCube priority-0 assertion is absent.
/* 0253EC0C */
s32 EventControl::order(u32 eventType, u32 priority, u32 flag, u32 hindFlag, u32 ac1, u32 ac2,
                        u32 eventIdx) {
    u32 i_this = gabi::ea(this);
    u8 info =
        gabi::load<u8>(gabi::cpu->r[1] + 8 + 3); /* 9th argument (u8 eventInfoIdx), on the stack */
    if (priority != 1) {
        if (static_cast<u16>(play()->play.mItemNowLife) == 0)
            return 0;
    }
    s32 cnt = static_cast<s8>(mOrderCount);
    if (cnt >= 8)
        return 0;
    u32 o = i_this + cnt * 0x18;
    EventOrder* entry = gabi::at<EventOrder>(o);
    entry->mEventType = static_cast<u16>((u16)eventType);
    entry->mPriority = static_cast<u16>((u16)priority);
    entry->mFlag = static_cast<u16>((u16)flag);
    entry->mActor1 = static_cast<u32>(ac1);
    entry->mActor2 = static_cast<u32>(ac2);
    entry->mEventId = static_cast<u16>((u16)eventIdx);
    entry->mHindFlag = static_cast<u16>((u16)hindFlag);
    entry->mEventInfoIdx = static_cast<u8>(info);
    s32 c = static_cast<s8>(mOrderCount);
    if (c == 0) {
        mFirstOrderIdx = static_cast<u8>(0);
        entry->mNextOrderIdx = static_cast<u8>(0xFF);
    } else {
        s32 qi = static_cast<s8>(mFirstOrderIdx);
        u32 q = i_this + qi * 0x18;
        u16 qp = static_cast<u16>(gabi::at<EventOrder>(q)->mPriority);
        if (qp == 1)
            return 0;
        u16 np = static_cast<u16>(entry->mPriority);
        if (np < qp) {
            mFirstOrderIdx = static_cast<u8>((u8)c);
            entry->mNextOrderIdx = static_cast<u8>((u8)qi);
        } else {
            s32 nx;
            while ((nx = static_cast<s8>(gabi::at<EventOrder>(q)->mNextOrderIdx)) >= 0) {
                if (np < static_cast<u16>(gabi::at<EventOrder>(i_this + nx * 0x18)->mPriority))
                    break;
                q = i_this + nx * 0x18;
            }
            entry->mNextOrderIdx = static_cast<u8>((u8)nx);
            gabi::at<EventOrder>(q)->mNextOrderIdx = static_cast<u8>(static_cast<u8>(mOrderCount));
        }
    }
    mOrderCount = static_cast<u8>((u8)(static_cast<u8>(mOrderCount) + 1));
    return 1;
}

/* 0253F138 */
void EventControl::setParam(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    EventStageInfo* info = stage_getEventInfo();
    mPt1 = static_cast<u32>(getPId(static_cast<u32>(eventOrder->mActor1)));
    mPt2 = static_cast<u32>(getPId(static_cast<u32>(eventOrder->mActor2)));
    mEventId = static_cast<u16>((u16) static_cast<s16>(eventOrder->mEventId));
    mHindFlag = static_cast<u16>(static_cast<u16>(eventOrder->mHindFlag));
    dComIfG_inf_c* p = play();
    u32 a1 = static_cast<u32>(eventOrder->mActor1);
    bool useActor1 = gabi::ea(p->play.mPlayerInfo[0].mpPlayer.get()) != a1;
    mPtTalk = static_cast<u32>(getPId((useActor1 ? static_cast<u32>(eventOrder->mActor1)
                                                 : static_cast<u32>(eventOrder->mActor2))));
    mPtItem = static_cast<u32>(getPId((useActor1 ? static_cast<u32>(eventOrder->mActor1)
                                                 : static_cast<u32>(eventOrder->mActor2))));
    mEventInfoIdx = static_cast<u8>(static_cast<u8>(eventOrder->mEventInfoIdx));
    u8 idx = static_cast<u8>(eventOrder->mEventInfoIdx);
    if (idx == 0xFF || info == 0 || !((s32)idx < (s32) static_cast<u32>(info->num))) {
        field_0xde = static_cast<u8>(0xFF);
        mEventFlag = static_cast<u16>(0);
        mCullFarClipRatio = static_cast<f32>(1.0f);
    } else {
        u8 v = static_cast<u8>(
            gabi::at<EventStageEntry>(static_cast<u32>(info->events) + idx * 0x18)->field_0x10);
        mEventFlag = static_cast<u16>(0);
        field_0xde = static_cast<u8>(v);
        mCullFarClipRatio = static_cast<f32>(1.0f);
    }
}

/* 0253F0F8 */
BOOL EventControl::beforeFlagProc(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u16 f = static_cast<u16>(eventOrder->mFlag);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if ((f & 4) && !(static_cast<u16>(actor2->eventInfo.mCondition) & dEvtCnd_CANTALK_e))
        return FALSE;
    return TRUE;
}

/* 0253F290 */
void EventControl::afterFlagProc(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u16 f = static_cast<u16>(eventOrder->mFlag);
    if (f & 1) {
        mPt2 = static_cast<u32>(getPId(0));
        f = static_cast<u16>(eventOrder->mFlag);
    }
    if (f & 2) {
        evmng()->issueStaff("ALL");
        f = static_cast<u16>(eventOrder->mFlag);
    }
    if (f & 8)
        mEventFlag = static_cast<u16>(static_cast<u16>(mEventFlag) | 0x20);
}

/* 0253F4D4 */
BOOL EventControl::commonCheck(u32 order, u32 cond, u32 cmd) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a1 = static_cast<u32>(eventOrder->mActor1);
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if (a1 != 0 && (static_cast<u16>(actor1->eventInfo.mCondition) & cond) == cond && a2 != 0 &&
        (static_cast<u16>(actor2->eventInfo.mCondition) & cond) == cond) {
        actor1->eventInfo.mCommand = static_cast<u16>((u16)cmd);
        actor2->eventInfo.mCommand = static_cast<u16>((u16)cmd);
        setParam(order);
        return TRUE;
    }
    return FALSE;
}

/* 0253F54C */
BOOL EventControl::talkCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if (!commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e))
        return FALSE;
    u32 id = (u32)(s32) static_cast<s16>(mEventId);
    mMode = static_cast<u8>(dEvtMode_TALK_e);
    if ((s32)id == -1) {
        if (a2 != 0 && reinterpret_cast<EventInfo*>(&actor2->eventInfo)->getEventName() != 0) {
            id = (u32)(s32) static_cast<s16>(actor2->eventInfo.mEventId);
        } else {
            id = evmng()->getEventIdx("DEFAULT_TALK", 0xFF);
        }
        mEventId = static_cast<u16>((u16)id);
    }
    if (!evmng()->order(id))
        JUT_ASSERT_fail("d_event.cpp", 0x170, "0");
    return TRUE;
}

// HD: a fourth item button (type 9, talk button 4)
/* 0253F8FC */
BOOL EventControl::talkXyCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u16 t = static_cast<u16>(eventOrder->mEventType);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    s32 itemBtn;
    switch (t) {
    case dEvtType_SHOWITEM_X_e:
        itemBtn = 0;
        break;
    case dEvtType_SHOWITEM_Y_e:
        itemBtn = 1;
        break;
    case dEvtType_SHOWITEM_HD_e:
        itemBtn = 3;
        break;
    default:
        itemBtn = 2;
        break;
    }
    if (static_cast<u8>(play()->play.mSelectItem[itemBtn]) == 0xFF)
        return FALSE;
    if (a2 == 0 || !(static_cast<u16>(actor2->eventInfo.mCondition) & dEvtCnd_CANTALKITEM_e))
        return FALSE;
    if (!commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e))
        return FALSE;
    mMode = static_cast<u8>(dEvtMode_TALK_e);
    mItemNo = static_cast<u8>(static_cast<u8>(play()->play.mSelectItem[itemBtn]));
    switch (static_cast<u16>(eventOrder->mEventType)) {
    case dEvtType_SHOWITEM_X_e:
        mTalkButton = static_cast<u8>(1);
        break;
    case dEvtType_SHOWITEM_Y_e:
        mTalkButton = static_cast<u8>(2);
        break;
    case dEvtType_SHOWITEM_HD_e:
        mTalkButton = static_cast<u8>(4);
        break;
    default:
        mTalkButton = static_cast<u8>(3);
        break;
    }
    u32 id;
    u32 cb = static_cast<u32>(actor2->eventInfo.mpEventCB);
    if (cb == 0 || (s32)(id = gabi::call_ptr<u32>(cb, a2, itemBtn)) == -1)
        id = evmng()->getEventIdx("DEFAULT_TALK_XY", 0xFF);
    mEventId = static_cast<u16>((u16)id);
    if (!evmng()->order(id))
        JUT_ASSERT_fail("d_event.cpp", 0x1C8, "0");
    return TRUE;
}

/* 0253FAE8 */
BOOL EventControl::photoCheckOrder(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    mbInPhoto = static_cast<u8>(0);
    if (static_cast<u8>(play()->play.mPictureStatus) != 1) {
        play(); /* the status is read again for the == 0 test (same result) */
        return FALSE;
    }
    if (a2 == 0)
        JUT_ASSERT_fail("d_event.cpp", 0x1DC, "actor2");
    if (!commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e))
        return FALSE;
    u32 id;
    u32 cb = static_cast<u32>(actor2->eventInfo.mpPhotoCB);
    if (cb == 0 || (s32)(id = gabi::call_ptr<u32>(cb, a2, 0)) == -1)
        id = evmng()->getEventIdx("DEFAULT_TALK", 0xFF);
    mEventId = static_cast<u16>((u16)id);
    if (!evmng()->order(id))
        JUT_ASSERT_fail("d_event.cpp", 0x1EC, "0");
    mMode = static_cast<u8>(dEvtMode_TALK_e);
    mbInPhoto = static_cast<u8>(1);
    return TRUE;
}

/* 0253FC2C */
BOOL EventControl::catchCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a1 = static_cast<u32>(eventOrder->mActor1);
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if (a1 == 0)
        return FALSE;
    if (a2 != 0 && !(static_cast<u16>(actor2->eventInfo.mCondition) & dEvtCnd_CANCATCH_e))
        return FALSE; /* CANCATCH */
    actor1->eventInfo.mCommand = dEvtCmd_INCATCH_e;
    if (a2 != 0)
        actor2->eventInfo.mCommand = dEvtCmd_INCATCH_e;
    setParam(order);
    u8 item = static_cast<u8>(reinterpret_cast<EventAttention*>(&play()->play.mAttention)
                                  ->mCatchChgItem); /* dComIfGp_att_getCatchChgItem */
    s16 id = static_cast<s16>(mEventId);
    mItemNo = static_cast<u8>(item);
    mMode = static_cast<u8>(dEvtMode_DEMO_e);
    if (id != -1 && !evmng()->order((u32)(s32)id))
        JUT_ASSERT_fail("d_event.cpp", 0x21A, "0");
    mEventFlag = static_cast<u16>(static_cast<u16>(mEventFlag) | 0x80);
    return TRUE;
}

/* 0253EE40 */
BOOL EventControl::talkEnd() {
    end_partners(this);
    u32 p = eventItemPartner(0);
    if (p != 0 && (static_cast<s16>(gabi::at<fopAc_ac_c>(p)->mProcName) == 0xFF ||
                   static_cast<s16>(gabi::at<fopAc_ac_c>(p)->mProcName) == 0x101))
        eventDeleteItem(p);
    return TRUE;
}

/* 0253F318 */
BOOL EventControl::demoCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    u32 a1 = static_cast<u32>(eventOrder->mActor1);
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    s16 eventId = static_cast<s16>(eventOrder->mEventId);
    if (a2 == 0) {
        JUT_ASSERT_fail("d_event.cpp", 0x256, "0");
        return FALSE;
    }
    if (!beforeFlagProc(order))
        return FALSE;
    if (!evmng()->order((u32)(s32)eventId))
        return FALSE;
    if (a1 != 0)
        actor1->eventInfo.mCommand = dEvtCmd_INDEMO_e;
    actor2->eventInfo.mCommand = dEvtCmd_INDEMO_e;
    mMode = static_cast<u8>(dEvtMode_DEMO_e);
    setParam(order);
    afterFlagProc(order);
    u32 snd = evmng()->getEventEndSound((u32)(s32)eventId);
    if (snd != 0 && static_cast<u8>(mEventEndSound) == 0) {
        mEventEndSound = static_cast<u8>((u8)snd);
        eventStartSound(0x806);
    }
    return TRUE;
}

/* 0253EF08 */
BOOL EventControl::demoEnd() {
    end_partners(this);
    evmng()->cancelStaff("ALL");
    return TRUE;
}

/* 0253F784 */
BOOL EventControl::potentialCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    u32 a1 = static_cast<u32>(eventOrder->mActor1);
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    u32 a2 = static_cast<u32>(eventOrder->mActor2);
    if (a1 == 0 || a2 == 0)
        JUT_ASSERT_fail("d_event.cpp", 0x2B7, "0");
    if (!beforeFlagProc(order))
        return FALSE;
    actor1->eventInfo.mCommand = dEvtCmd_INDEMO_e;
    mMode = static_cast<u8>(dEvtMode_DEMO_e);
    setParam(order);
    afterFlagProc(order);
    return TRUE;
}

// HD: TREASURE is type 0xB
/* 0253F648 */
BOOL EventControl::doorCheck(u32 order) {
    EventOrder* eventOrder = gabi::at<EventOrder>(order);
    if (!commonCheck(order, dEvtCnd_CANDOOR_e, dEvtCmd_INDOOR_e))
        return FALSE;
    mMode = static_cast<u8>(dEvtMode_DEMO_e);
    u32 a2 = convPId(static_cast<u32>(mPt2));
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    s32 id = static_cast<s16>(mEventId);
    bool ok = true;
    if (id == -1) {
        if (a2 == 0)
            ok = false;
        else {
            id = static_cast<s16>(actor2->eventInfo.mEventId);
            if (id == -1)
                ok = false;
            else
                mEventId = static_cast<u16>((u16)id);
        }
    }
    if (ok) {
        if (evmng()->getEventData((u32)id) == 0)
            ok = false;
    }
    if (ok) {
        s16 id2 = static_cast<s16>(mEventId);
        if (!evmng()->order((u32)(s32)id2))
            JUT_ASSERT_fail("d_event.cpp", 0x2F7, "0");
    } else {
        u16 f = static_cast<u16>(mEventFlag);
        mEventId = static_cast<u16>(0xFFFF);
        mEventFlag = static_cast<u16>(f | 8);
    }
    if (static_cast<u16>(eventOrder->mEventType) == dEvtType_TREASURE_e) {
        dComIfG_inf_c* p = play();
        p->play.mEvtCtrl.mEventFlag =
            static_cast<u16>(p->play.mEvtCtrl.mEventFlag) | 4; /* dComIfGp_event_onEventFlag(4) */
    }
    return TRUE;
}

/* 0253F84C */
BOOL EventControl::itemCheck(u32 order) {
    if (!commonCheck(order, dEvtCnd_CANGETITEM_e, dEvtCmd_INGETITEM_e))
        return FALSE;
    mMode = static_cast<u8>(dEvtMode_DEMO_e);
    u32 id = evmng()->getEventIdx("DEFAULT_GETITEM", 0xFF);
    mEventId = static_cast<u16>((u16)id);
    if (!evmng()->order(id))
        JUT_ASSERT_fail("d_event.cpp", 0x321, "0");
    return TRUE;
}

/* 0253EFA8 */
BOOL EventControl::endProc() {
    switch (static_cast<u8>(mMode)) {
    case dEvtMode_TALK_e:
        talkEnd();
        break;
    case dEvtMode_DEMO_e:
        demoEnd();
        break;
    case dEvtMode_COMPULSORY_e:
        break;
    default:
        JUT_ASSERT_fail("d_event.cpp", 0x34C, "0");
        break;
    }
    mMode = static_cast<u8>(dEvtMode_NONE_e);
    field_0xde = static_cast<u8>(0xFF);
    mEventInfoIdx = static_cast<u8>(0xFF);
    mTalkButton = static_cast<u8>(0);
    mbInPhoto = static_cast<u8>(0);
    mItemNo = static_cast<u8>(0xFF);
    mEventFlag = static_cast<u16>(0);
    return TRUE;
}

/* 0253F0B0 */
BOOL EventControl::checkChange() {
    u32 i_this = gabi::ea(this);
    if (static_cast<s8>(mOrderCount) == 0)
        return FALSE;
    s32 next = static_cast<s8>(mFirstOrderIdx);
    do {
        u32 o = i_this + next * 0x18;
        EventOrder* entry = gabi::at<EventOrder>(o);
        next = static_cast<s8>(entry->mNextOrderIdx);
        if (static_cast<u16>(entry->mEventType) == dEvtType_CHANGE_e)
            return TRUE;
    } while (next >= 0);
    return FALSE;
}

/* 0253F434 */
BOOL EventControl::changeProc() {
    u32 i_this = gabi::ea(this);
    if (static_cast<s8>(mOrderCount) == 0)
        return FALSE;
    s32 next = static_cast<s8>(mFirstOrderIdx);
    mOrderCount = static_cast<u8>(0);
    do {
        u32 o = i_this + next * 0x18;
        EventOrder* entry = gabi::at<EventOrder>(o);
        u16 t = static_cast<u16>(entry->mEventType);
        next = static_cast<s8>(entry->mNextOrderIdx);
        if (t == dEvtType_CHANGE_e && demoCheck(o))
            return TRUE;
    } while (next >= 0);
    return FALSE;
}

/* 0253FD1C */
BOOL EventControl::checkStart() {
    u32 i_this = gabi::ea(this);
    if (static_cast<s8>(mOrderCount) == 0)
        return FALSE;
    s32 idx = static_cast<s8>(mFirstOrderIdx);
    mOrderCount = static_cast<u8>(0);
    do {
        u32 o = i_this + idx * 0x18;
        EventOrder* entry = gabi::at<EventOrder>(o);
        u16 t = static_cast<u16>(entry->mEventType);
        idx = static_cast<s8>(entry->mNextOrderIdx);
        switch (t) {
        case dEvtType_TALK_e:
            if (talkCheck(o))
                return TRUE;
            break;
        case dEvtType_OTHER_e:
            if (demoCheck(o))
                return TRUE;
            break;
        case dEvtType_DOOR_e:
        case dEvtType_TREASURE_e:
            if (doorCheck(o))
                return TRUE;
            break;
        case dEvtType_COMPULSORY_e:
            mMode = static_cast<u8>(dEvtMode_COMPULSORY_e);
            setParam(o);
            return TRUE;
        case dEvtType_POTENTIAL_e:
            return potentialCheck(o);
        case dEvtType_ITEM_e:
            return itemCheck(o);
        case dEvtType_SHOWITEM_X_e:
        case dEvtType_SHOWITEM_Y_e:
        case dEvtType_SHOWITEM_Z_e:
        case dEvtType_SHOWITEM_HD_e:
            if (talkXyCheck(o))
                return TRUE;
            break;
        case dEvtType_PHOTO_e:
            if (photoCheckOrder(o))
                return TRUE;
            break;
        case dEvtType_CATCH_e:
            if (catchCheck(o))
                return TRUE;
            break;
        case dEvtType_CHANGE_e:
            break;
        default:
            JUT_ASSERT_fail("d_event.cpp", 0x3D4, "0");
            break;
        }
    } while (idx >= 0);
    return FALSE;
}

/* 0253FF0C */
BOOL EventControl::soundProc() {
    if (static_cast<u8>(mEventEndSound) != 0 && static_cast<u8>(mMode) != dEvtMode_DEMO_e)
        mEventEndSound = static_cast<u8>(0);
    return TRUE;
}

/* 0253FF34 */
BOOL EventControl::check() {
    u16 f = static_cast<u16>(mEventFlag);
    u8 cnt = static_cast<u8>(mOrderCount);
    u8 mode = static_cast<u8>(mMode);
    field_0xdb = static_cast<u8>(0);
    field_0xdc = static_cast<u8>(cnt);
    if (f & 8) {
        mbEndProc = static_cast<u8>(1);
        mEventFlag = static_cast<u16>(static_cast<u16>(mEventFlag) & 0xFFF7);
    }
    if (mode != dEvtMode_NONE_e && static_cast<u8>(mbEndProc) == 1) {
        mbEndProc = static_cast<u8>(0);
        endProc();
        mode = static_cast<u8>(mMode);
    }
    if (checkChange()) {
        if (mode != dEvtMode_NONE_e) {
            mbEndProc = static_cast<u8>(0);
            endProc();
        }
        changeProc();
        mode = static_cast<u8>(mMode);
    }
    if (mode == dEvtMode_NONE_e && checkStart())
        eventStopQuake(gabi::ea(&play()->play.mVibration), -1);
    soundProc();
    mOrderCount = static_cast<u8>(0);
    return FALSE;
}

// HD: the item-button table is indexed by the order type (6..8)
/* 02540024 */
BOOL EventControl::photoCheck() {
    u32 i_this = gabi::ea(this);
    s32 cnt = static_cast<s8>(mOrderCount);
    s32 first = static_cast<s8>(mFirstOrderIdx);
    if (cnt == 0)
        return FALSE;
    u32 o = i_this + first * 0x18;
    EventOrder* entry = gabi::at<EventOrder>(o);
    u16 t = static_cast<u16>(entry->mEventType);
    if (t < 6 || t > 8)
        return FALSE;
    u8 btn = l_itemButton[t - dEvtType_SHOWITEM_X_e];
    if (static_cast<u8>(play()->play.mSelectItem[btn]) != 0x23) {
        if (static_cast<u8>(play()->play.mSelectItem[btn]) != 0x26)
            return FALSE;
    }
    EventSaveContext* save = gabi::at<EventSaveContext>(dComIfGs_ea());
    EventPictureArchive* pic = save->pictures.getPictureInfo();
    if (static_cast<u8>(pic->mPictureNum) == 0)
        return FALSE; /* dComIfGs_getPictureNum */
    u32 a2 = static_cast<u32>(entry->mActor2);
    fopAc_ac_c* actor2 = gabi::at<fopAc_ac_c>(a2);
    if (a2 == 0)
        return FALSE;
    if (!(static_cast<u32>(actor2->attention_info.flags) & 0x01000000))
        return FALSE;
    if (static_cast<u8>(play()->play.mPictureStatus) == 2)
        return FALSE;
    u32 a1 = static_cast<u32>(entry->mActor1);
    fopAc_ac_c* actor1 = gabi::at<fopAc_ac_c>(a1);
    if (a1 == 0)
        return FALSE;
    if (!(static_cast<u16>(actor1->eventInfo.mCondition) & dEvtCnd_CANTALK_e))
        return FALSE;
    if (!(static_cast<u16>(actor2->eventInfo.mCondition) & dEvtCnd_CANTALK_e))
        return FALSE;
    entry->mEventType = static_cast<u16>(0xC);
    play()->play.mPictureStatus = 2; /* dComIfGp_setPictureStatusOn */
    return TRUE;
}

/* 02540158 */
s32 EventControl::moveApproval(u32 ac) {
    fopAc_ac_c* actor = gabi::at<fopAc_ac_c>(ac);
    u8 mode = static_cast<u8>(mMode);
    if (mode == dEvtMode_NONE_e)
        return dEvtMove_MOVE_e;
    switch (mode) {
    case dEvtMode_TALK_e:
        if (convPId(static_cast<u32>(mPt1)) == ac)
            return dEvtMove_FORCE_e;
        if (convPId(static_cast<u32>(mPt2)) == ac)
            return dEvtMove_FORCE_e;
        break;
    case dEvtMode_DEMO_e:
        if (convPId(static_cast<u32>(mPt1)) == ac)
            return dEvtMove_FORCE_e;
        if (convPId(static_cast<u32>(mPt2)) == ac)
            return dEvtMove_FORCE_e;
        if (static_cast<u8>(actor->demoActorID) != 0)
            return dEvtMove_FORCE_e;
        break;
    case dEvtMode_COMPULSORY_e:
        if (convPId(static_cast<u32>(mPt1)) == ac)
            return dEvtMove_FORCE_e;
        break;
    }
    u32 status = static_cast<u32>(actor->actor_status);
    if (status & fopAcStts_FORCEMOVE_e)
        return dEvtMove_FORCE_e;
    if (static_cast<u8>(mMode) == dEvtMode_TALK_e && (status & fopAcStts_UNK40_e))
        return dEvtMove_MOVE_e;
    if (l_demoMode == 1)
        return dEvtMove_MOVE_e; /* dComIfGp_demo_mode */
    if (status & fopAcStts_UNK800_e)
        return dEvtMove_MOVE_e;
    bool bossStop;
    if (static_cast<u8>(play()->play.mEvtCtrl.mMode) == dEvtMode_COMPULSORY_e ||
        static_cast<u8>(play()->play.mEvtCtrl.mMode) == dEvtMode_TALK_e) {
        status = static_cast<u32>(actor->actor_status);
        if (status & fopAcStts_BOSS_e)
            return dEvtMove_NOMOVE_e;
        bossStop = (static_cast<u16>(mEventFlag) & 0x80) != 0;
    } else {
        bossStop = (static_cast<u16>(mEventFlag) & 0x80) != 0;
        status = static_cast<u32>(actor->actor_status);
    }
    if (bossStop && (status & fopAcStts_BOSS_e))
        return dEvtMove_NOMOVE_e;
    if (status & fopAcStts_UNK4000_e)
        return dEvtMove_MOVE_e;
    if (status & fopAcStts_CARRY_e)
        return dEvtMove_MOVE_e;
    return dEvtMove_NOMOVE_e;
}

/* 02540310 */
s32 EventControl::compulsory(u32 actor, u32 eventName, u32 p3) {
    if (static_cast<u8>(mMode) != dEvtMode_NONE_e)
        return 0;
    return orderOld(dEvtType_COMPULSORY_e, 1, 0, p3, actor, 0, eventName);
}

/* 0253EA70 */
void EventControl::remove() {
    mTalkButton = static_cast<u8>(0);
    mCullFarClipRatio = static_cast<f32>(0.0f);
    field_0xdb = static_cast<u8>(0);
    field_0xde = static_cast<u8>(0xFF);
    mbEndProc = static_cast<u8>(0);
    mEventInfoIdx = static_cast<u8>(0xFF);
    mMode = static_cast<u8>(dEvtMode_NONE_e);
    mItemNo = static_cast<u8>(0xFF);
    mDebugStb = static_cast<u8>(0);
    mOrderCount = static_cast<u8>(0);
    mEventEndSound = static_cast<u8>(0);
    mbInPhoto = static_cast<u8>(0);
    mEventFlag = static_cast<u16>(0);
}

/* 02540348 */
u32 EventControl::getStageEventDt() {
    EventStageInfo* info = stage_getEventInfo();
    if (static_cast<u8>(mMode) == dEvtMode_NONE_e)
        return 0;
    if (info == 0)
        return 0;
    u8 idx = static_cast<u8>(mEventInfoIdx);
    if (idx == 0xFF)
        return 0;
    if (!((s32)idx < (s32) static_cast<u32>(info->num)))
        return 0;
    return static_cast<u32>(info->events) + idx * 0x18;
}

/* 025403D4 */
u32 EventControl::nextStageEventDt(u32 idxp) {
    EventStageInfo* info = stage_getEventInfo();
    if (idxp == 0 || info == 0)
        return 0;
    u8 idx = gabi::load<u8>(idxp);
    if (idx == 0xFF)
        return 0;
    if (!((s32)idx < (s32) static_cast<u32>(info->num)))
        return 0;
    return static_cast<u32>(info->events) + idx * 0x18;
}

/* 0253F124 */
u32 EventControl::getPId(u32 ac) {
    if (ac == 0)
        return 0xFFFFFFFF;
    return static_cast<u32>(gabi::at<fopAc_ac_c>(ac)->mBsPcId);
}

/* 0253EE04 */
u32 EventControl::convPId(u32 pid) { return gabi::ea(fopAcM_SearchByID(pid)); }

/* 0254045C */
u8 EventControl::getTactFreeMStick(s32 which) { return static_cast<u8>(mTactFreeMStick[which]); }

/* 02540468 */
u8 EventControl::getTactFreeCStick(s32 which) { return static_cast<u8>(mTactFreeCStick[which]); }

// HD: dEvt_info_c constructor includes its trailing vtable.
/* 0253E948 */
static u32 dEvt_info_c_ct(u32 i_this) {
    if (i_this == 0) {
        i_this = gabi::ea(operator_new(sizeof(EventInfo)));
        if (i_this == 0)
            return 0;
    }
    EventInfo* self = gabi::at<EventInfo>(i_this);
    self->mpPhotoCB = static_cast<u32>(0);
    self->mEventId = static_cast<u16>(0xFFFF);
    self->mMapToolId = static_cast<u8>(0xFF);
    self->mCondition = static_cast<u16>(0);
    self->__vtbl = static_cast<u32>(0x1004D764);
    self->mpEventCB = static_cast<u32>(0);
    self->mpCheckCB = static_cast<u32>(0);
    self->mCommand = static_cast<u16>(0);
    return i_this;
}

/* 0253E9B0 */
void EventInfo::setEventName(u32 evtName) {
    if (evtName == 0)
        mEventId = static_cast<u16>(0xFFFF);
    else
        mEventId = static_cast<u16>((u16)evmng()->getEventIdx(gabi::at<char>(evtName), 0xFF));
}

// HD: returns the event data (its name is at offset 0)
/* 0253EA0C */
u32 EventInfo::getEventName() {
    if (static_cast<s16>(mEventId) == -1)
        return 0;
    EventManager* manager = evmng();
    return manager->getEventData((u32)(s32) static_cast<s16>(mEventId));
}

/* 0253EB70 */
s32 EventControl::giveItemCut(u32 item) {
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("GIVEMAN", nullptr, 0);
    if (staffIdx == -1)
        return 0;
    dComIfGp_evmng_cutEnd(staffIdx);
    mGetItemNo = static_cast<u8>((u8)item);
    return 1;
}

// HD: compiler-generated and template code of this translation unit
/* 02540474 */
static void __sinit_d_event_cpp() { sinit_header_statics(0x104758BC, 0x101D62A0); }

/* WWHD glue
str 1004D778 "GIVEMAN"
str 1004D780 in 0253EF08
str 1004D798 in 0253F290
str 1004D7BC in 0253F54C
str 1004D7F0 in 0253F84C
str 1004D810 in 0253F8FC
str 1004D844 in 0253FAE8
str 1004D7B0 in 0253F54C
str 1004D7AC in 0253F54C
str 1004D820 in 0253F8FC
str 1004D80C in 0253F8FC
str 1004D838 in 0253FAE8
str 1004D830 in 0253FAE8
str 1004D82C in 0253FAE8
str 1004D858 in 0253FC2C
str 1004D854 in 0253FC2C
str 1004D7A0 in 0253F318
str 1004D79C in 0253F318
str 1004D7E0 in 0253F784
str 1004D7DC in 0253F784
str 1004D7D0 in 0253F648
str 1004D7CC in 0253F648
str 1004D800 in 0253F84C
str 1004D7EC in 0253F84C
str 1004D788 in 0253EFA8
str 1004D784 in 0253EFA8
str 1004D868 in 0253FD1C
str 1004D864 in 0253FD1C
data l_itemButton 1004D874
data l_demoMode 101D6010
 */
