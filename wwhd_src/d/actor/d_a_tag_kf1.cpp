/* Kf1 pottery compensation event tag. Private, numerically verified
 * reconstruction. */
#include "d/actor/d_a_tag_kf1.h"

// HD: GameCube bodies are placeholders; verified HD call and store order is
// authoritative.
static s32 l_partnerCount;
static daTag_Kf1_HIO_c l_HIO;
static KfPartnerList l_partners;
static KfActionWords l_waitAction;
static u8 l_debugDraw;
static u32 l_redGuard;
static KfColorWord l_red;
static KfColorWord l_redLiteral;
static u32 l_saveInfo;
static KfCutNames l_cutNames;
static KfEventNames l_eventNames;
static KfHioDefaults l_hioDefaults;
static f32 l_headerNear;
static f32 l_headerFar;
static u8 l_headerByte;
static u8 l_headerFlag;
static u32 l_headerZero0;
static u32 l_headerZero1;
static u32 l_headerZero2;
static u32 l_headerZero3;
static f32 l_nearLiteral;
static f32 l_farLiteral;
static f32 l_nextStageSpeed;
static KfGlobalNode l_headerNode0;
static KfGlobalNode l_headerNode1;
static KfGlobalNode l_hioNode;

static KfPlay *play() {
    return kfGetPlay();
}
// Resolve the GHS descriptor at the point of invocation, allowing callees to
// change fields.
static void invokeAction(daTag_Kf1_c *i_this, u32 context) {
    // HD: reload the GHS descriptor for each invocation; exit handlers may mutate it.
    s32 index = i_this->mAction.i;
    s32 adjust = i_this->mAction.d;
    KfActionObject object(i_this, adjust);
    u32 target = index < 0 ? (u32)i_this->mAction.f
                           : object.virtualTarget(i_this->mAction.virtualOffset(), index);
    gabi::call_ptr(target, object.address(), context);
}
/* 024AC920 */
static daTag_Kf1_HIO_c *daTag_Kf1_HIO_c_ct(daTag_Kf1_HIO_c *object) {
    if (!object) {
        object = kfAllocate(0x14);
        if (!object)
            return 0;
    }
    object->__vtbl = daTag_Kf1_HIO_c::__vt;
    u32 word = (u32)l_hioDefaults.words[0];
    object->mAttentionDistanceBits = word;
    word = (u32)l_hioDefaults.words[1];
    object->mAttentionHeightBits = word;
    word = (u32)l_hioDefaults.words[2];
    object->mNo = -1;
    object->mDebugDrawWord = word;
    return object;
}

/* 024ABBF4 */
static u32 searchActor_Kutani(fopAc_ac_c *i_this, u32 context) {
    if (l_partnerCount < 100 && kfIsActor(i_this) && i_this &&
        (s16)i_this->mProcName == fpcNm_TSUBO_e && kfGetActorParam(i_this, 4, 24) == 14) {
        u32 count = (u32)l_partnerCount;
        l_partnerCount = (u32)(count + 1);
        l_partners.actors[count] = gabi::ea(i_this);
    }
    return 0;
}

/* 024ABDB0 */
u32 daTag_Kf1_c::createInit() {
    kfSetActorInfo(&mTagEventCut, "TagKf1", this);
    gabi::Local<KfActionWords> descriptor;
    descriptor->words[0] = l_waitAction.words[0];
    descriptor->words[1] = l_waitAction.words[1];
    set_action(reinterpret_cast<const KfAction *>(descriptor.get()), 0);
    return 1;
}

/* 024ABFB4 */
void daTag_Kf1_c::setStt(u32 state) {
    mState = (u8)(state);
}

/* 024AC61C */
u32 daTag_Kf1_c::next_msgStatus(be<u32> *message) {
    u32 number = (u32)*message;
    if (number == KfMsg_CANNOT_PAY) {
        *message = KfMsg_PAYMENT_NEXT;
        return 15;
    }
    if (number == KfMsg_PAYMENT_NEXT) {
        *message = KfMsg_PAYMENT_END;
        return 15;
    }
    if (number == KfMsg_CHECK_RUPEES) {
        KfSave *save = KfGuestMemory::save(l_saveInfo);
        u32 rupees = save->mRupees;
        mRupees = (u16)(rupees);
    }
    return 16;
}

/* 024AC498 */
void daTag_Kf1_c::eventOrder() {
    s32 order = (s8)mOrder;
    if (order == 1 || order == 2) {
        s32 current = (s8)mOrder;
        u32 flags = (u16)eventInfo.mCondition;
        eventInfo.mCondition = (u16)(flags | 1);
        if (current == 1)
            kfOrderSpeakEvent(this);
    } else if (order >= 3) {
        u32 name = (u32)l_eventNames.names[order];
        kfOrderOtherEvent(this, name, 1, 0xFFFF);
    }
}

/* 024ABF4C */
void daTag_Kf1_c::checkOrder() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        void *events = &play()->mEventManager;
        if (kfIsPresent(events, "BENSYO") && (s8)mOrder == 3)
            mOrder = (u8)(0);
    }
}

/* 024AC678 */
u32 daTag_Kf1_c::chkAttention(const cXyz *position) {
    KfPlay *state = play();
    u32 player = state->mPlayer;
    f32 distance2 = kfSquareDistance(position, &KfGuestMemory::actor(player)->current.pos);
    f32 distance = kfSqrt(distance2);
    state = play();
    player = (u32)state->mPlayer;
    f32 horizontal = (f32)l_HIO.mAttentionDistance,
        playerY = (f32)KfGuestMemory::actor(player)->current.pos.y, y = (f32)position->y;
    f32 dy = (f32)((f64)playerY - (f64)y);
    // PPC bge tests !LT, so unordered NaNs fail either attention comparison.
    return distance < horizontal && dy < (f32)l_HIO.mAttentionHeight;
}

/* 024AC71C */
u32 daTag_Kf1_c::partner_srch() {
    for (u32 i = 0; i < 8; ++i)
        mPartnerIds[i] = 0xFFFFFFFF;
    l_partnerCount = (u32)(0);
    for (u32 i = 0; i < 100; ++i)
        l_partners.actors[i] = 0;
    kfSearchAll(gabi::guest_fn((const void *)searchActor_Kutani), this);
    s32 count = l_partnerCount;
    if (count > 8 || count == 0)
        return 0;
    mPartnerCount = (s16)(0);
    count = l_partnerCount;
    for (s32 i = 0; i < count; ++i) {
        u32 partner = (u32)l_partners.actors[i];
        u32 id = partner ? (u32)KfGuestMemory::actor(partner)->mBsPcId : 0xFFFFFFFF;
        mPartnerIds[i] = id;
        s32 saved = (s16)mPartnerCount;
        mPartnerCount = (s16)(saved + 1);
        count = l_partnerCount;
    }
    return 1;
}

/* 024AC114 */
s32 daTag_Kf1_c::checkPartner() {
    s32 found = 0, count = (s16)mPartnerCount;
    gabi::Local<be<u32>> id;
    for (s32 i = 0; i < count; ++i) {
        u32 value = (u32)mPartnerIds[i];
        *id = value;
        if (value != 0xFFFFFFFF) {
            u32 result = kfSearchByJudge(kfJudgeByIDAddress, id.a);
            count = (s16)mPartnerCount;
            if (result)
                found = (s16)(found + 1);
        }
    }
    return found;
}

/* 024AC0CC */
void daTag_Kf1_c::goto_nextStage() {
    void *stage = &play()->mStartStage;
    f32 speed = l_nextStageSpeed;
    kfSetNextStage(stage, 0, -1, -1, 0, 1, 0, speed);
}

/* 024ABFBC */
void daTag_Kf1_c::event_talkInit(s32 staff) {
    void *events = &play()->mEventManager;
    u32 substance = kfGetSubstance(events, staff, "MsgNo", 3);
    mCurrMsgBsPcId = (u32)(0xFFFFFFFF);
    if (!substance)
        mCurrMsgNo = (u32)(0);
    else {
        u32 msg = KfGuestMemory::word(substance);
        mCurrMsgNo = (u32)(msg);
        if (msg == KfMsg_DEBT) {
            s32 debt = (s16)mMissingPartners * 10;
            KfPlay *state = play();
            state->mRupeeDebt = debt;
        }
    }
}

/* 024AC1D4 */
u32 daTag_Kf1_c::event_mesSet() {
    kfTalk(this, 0);
    return (u32)mCurrMsgBsPcId != 0xFFFFFFFF;
}

/* 024AC218 */
u32 daTag_Kf1_c::event_mesEnd() {
    return kfTalk(this, 0) == 0x12;
}

/* 024AC05C */
void daTag_Kf1_c::bensyoInit() {
    u32 debt = (u32)((s16)mMissingPartners * 10);
    KfPlay *state = play();
    u32 balance = (u32)state->mRupeeBalance;
    state->mRupeeBalance = balance - debt;
    s32 cost = (s16)mMissingPartners * 10;
    u32 rupees = (u16)mRupees;
    mCurrMsgBsPcId = (u32)(0xFFFFFFFF);
    mCurrMsgNo = (u32)((s32)rupees > cost ? KfMsg_CAN_PAY : KfMsg_CANNOT_PAY);
}

/* 024AC248 */
u32 daTag_Kf1_c::event_bensyo() {
    return event_mesSet();
}

/* 024AC19C */
void daTag_Kf1_c::event_cntTsubo() {
    s32 found = checkPartner();
    s32 count = (s16)mPartnerCount;
    mMissingPartners = (s16)(count - found);
}

/* 024AC24C */
void daTag_Kf1_c::privateCut() {
    void *events = &play()->mEventManager;
    s32 staff = kfGetMyStaffId(events, "TagKf1", 0, 0);
    if (staff == -1)
        return;
    events = &play()->mEventManager;
    s32 cut = (s8)kfGetMyActIdx(events, staff, gabi::ea(&l_cutNames), 5, 1, 0);
    mCutIndex = (s8)(cut);
    events = &play()->mEventManager;
    if (cut == -1) {
        kfCutEnd(events, staff);
        return;
    }
    if (kfIsAdvance(events, staff)) {
        cut = (s8)mCutIndex;
        if (cut == 0)
            event_talkInit(staff);
        else if (cut == 2)
            bensyoInit();
        else if (cut == 3)
            goto_nextStage();
        else if (cut == 4)
            event_cntTsubo();
        // Original reloads only after one of the four initialization callbacks.
        if (cut == 0 || cut == 2 || cut == 3 || cut == 4)
            cut = (s8)mCutIndex;
    } else
        cut = (s8)mCutIndex;
    bool finish;
    if ((u32)cut < 1)
        finish = event_mesSet() != 0;
    else if (cut == 1)
        finish = event_mesEnd() != 0;
    else if (cut == 2)
        finish = event_bensyo() != 0;
    else
        finish = true;
    if (finish) {
        events = &play()->mEventManager;
        kfCutEnd(events, staff);
    }
}

/* 024AC3E4 */
void daTag_Kf1_c::event_proc() {
    void *events = &play()->mEventManager;
    if (kfIsEnd(events, "BENSYO")) {
        setStt(2);
        return;
    }
    u32 saved = (u8)mTagEventCut.mbAttention;
    u32 result = kfCutProc(&mTagEventCut);
    if (result) {
        if (!(u8)mTagEventCut.mbAttention)
            mTagEventCut.mbAttention = (u8)(saved);
    } else
        privateCut();
}

/* 024ABC8C */
u32 daTag_Kf1_c::set_action(const KfAction *descriptor, u32 context) {
    s32 index = (s16)descriptor->i, oldIndex = (s16)mAction.i, adjust;
    u32 target;
    if (oldIndex == index) {
        if (!oldIndex)
            return 1;
        adjust = (s16)descriptor->d;
        s32 oldAdjust = (s16)mAction.d;
        target = (u32)descriptor->f;
        if (oldAdjust == adjust && (u32)mAction.f == target)
            return 1;
    } else {
        target = (u32)descriptor->f;
        adjust = (s16)descriptor->d;
    }
    if (oldIndex) {
        mActionPhase = (u8)(0xFF);
        invokeAction(this, context);
    }
    mAction.f = (u32)(target);
    mAction.d = (s16)(adjust);
    mAction.i = (s16)(index);
    mActionPhase = (u8)(0);
    invokeAction(this, context);
    return 1;
}

/* 024AC80C */
u32 daTag_Kf1_c::wait01() {
    u32 attention = (u8)mbAttention;
    mOrder = (u8)(0);
    if (attention) {
        u32 found = checkPartner();
        u32 count = (u32)(s16)mPartnerCount;
        if (count != found)
            mOrder = (u8)(3);
    }
    return 1;
}

/* 024AC868 */
u32 daTag_Kf1_c::wait_action1(u32 context) {
    s32 phase = (s8)mActionPhase;
    if (!phase) {
        setStt(1);
        u32 value = (u8)mActionPhase;
        mActionPhase = (u8)(value + 1);
        return 1;
    }
    if (phase != -1) {
        if (phase == 1) {
            partner_srch();
            mActionPhase = (u8)(2);
        }
        gabi::Local<cXyz> position;
        position->x = (f32)current.pos.x;
        position->y = (f32)current.pos.y;
        position->z = (f32)current.pos.z;
        u32 attention = chkAttention(position.get());
        s32 state = (s8)mState;
        mbAttention = (u8)(attention);
        if (state == 1)
            wait01();
    }
    return 1;
}

/* 024AC5B0 */
u32 daTag_Kf1_c::_draw() {
    if (l_debugDraw && !l_redGuard) {
        l_redGuard = (u32)(1);
        kfCopyBytes(gabi::ea(&l_red), gabi::ea(&l_redLiteral), 4);
    }
    return 1;
}

/* 024AC4F8 */
u32 daTag_Kf1_c::_execute() {
    checkOrder();
    KfPlay *state = play();
    if ((u8)state->mEventState && (u16)eventInfo.mCommand != dEvtCmd_INTALK_e)
        event_proc();
    else
        invokeAction(this, 0);
    eventOrder();
    return 1;
}

/* 024ABF04 */
u32 daTag_Kf1_c::_delete() {
    s32 id = (s8)l_HIO.mNo;
    if (id >= 0) {
        kfHioDeleteChild(id);
        l_HIO.mNo = (u8)(0xFF);
    }
    return 1;
}

/* 024ABE14 */
u32 daTag_Kf1_c::_create() {
    u32 flags = (u32)actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            kfNpcConstruct(this);
            __vtbl = (u32)(daTag_Kf1_c::__vt);
            kfEventCutConstruct(&mTagEventCut);
            flags = (u32)actor_condition;
        }
        actor_condition = (u32)(flags | fopAcCnd_INIT_e);
    }
    if ((s16)mProcName != fpcNm_TAG_KF1_e)
        return cPhs_ERROR_e;
    mbInitialized = (u8)(0);
    if ((s8)l_HIO.mNo < 0) {
        u32 id = kfHioCreateChild(
            "\u0083N\u0083^\u0083j\u008f\u00c4\u0082\u00ab\u008a\u00c4\u008e\u008b\u0083^\u0083O",
            gabi::ea(&l_HIO));
        l_HIO.mNo = (u8)(id);
    }
    return createInit() ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}

/* 024ABF00 */
static u32 daTag_Kf1_Create(daTag_Kf1_c *i_this) {
    return i_this->_create();
}

/* 024ABF48 */
static u32 daTag_Kf1_Delete(daTag_Kf1_c *i_this) {
    return i_this->_delete();
}

/* 024AC5AC */
static u32 daTag_Kf1_Execute(daTag_Kf1_c *i_this) {
    return i_this->_execute();
}

/* 024AC610 */
static u32 daTag_Kf1_Draw(daTag_Kf1_c *i_this) {
    return i_this->_draw();
}

/* 024AC614 */
static u32 daTag_Kf1_IsDelete(daTag_Kf1_c *i_this) {
    return 1;
}

// HD: compiler-generated and template code of this translation unit

/* 024AC984 */
static void __sinit_d_a_tag_kf1_cpp() {
    l_headerZero2 = (u32)(0);
    l_headerZero0 = (u32)(0);
    l_headerZero3 = (u32)(0);
    l_headerZero1 = (u32)(0);
    kfRegisterGlobal(gabi::ea(&l_headerNode0));
    l_headerNear = (f32)(l_nearLiteral);
    l_headerFar = (f32)(l_farLiteral);
    kfHeaderByteConstruct(gabi::ea(&l_headerByte));
    kfRegisterGlobal(gabi::ea(&l_headerNode1));
    kfHeaderFlagConstruct(gabi::ea(&l_headerFlag));
    kfRegisterGlobal(gabi::ea(&l_hioNode));
    daTag_Kf1_HIO_c_ct(&l_HIO);
}

/* 024ACA24 */
static void daTag_Kf1_c_dt(daTag_Kf1_c *i_this, u32 flags) {
    if (i_this) {
        kfAcchDestroy(&i_this->mCyl, 2);
        kfAcchCircleDestroy(&i_this->mStts, 2);
        kfLineCheckDestroy(&i_this->mAcchCir.m_cir, 2);
        static_cast<dBgS_GrpPassChk &>(i_this->mObjAcch).__vtbl = KfAcchVtables::ground;
        static_cast<dBgS_PolyPassChk &>(i_this->mObjAcch).__vtbl = KfAcchVtables::line;
        kfActorAcchDestroy(&i_this->mObjAcch, 0);
        kfActorDestroy(i_this, 0);
        if (flags & 1)
            kfFree(i_this);
    }
}

// clang-format off
/* WWHD glue: private HD data views
data l_partnerCount 1046E388
data l_HIO 1046E3A8
data l_partners 1046E3BC
data l_waitAction 1003F578
data l_debugDraw 1046E3B4
data l_redGuard 101FDA50
data l_red 101FEBF4
data l_redLiteral 1003F580
data l_saveInfo 101F84DC
data l_cutNames 101D1BF8
data l_eventNames 101D1C00
data l_hioDefaults 101D1C10
data l_headerNear 1046E38C
data l_headerFar 1046E390
data l_headerByte 1046E394
data l_headerFlag 1046E395
data l_headerZero0 1046E398
data l_headerZero1 1046E39C
data l_headerZero2 1046E3A0
data l_headerZero3 1046E3A4
data l_nearLiteral 1003F63C
data l_farLiteral 1003F640
data l_nextStageSpeed 1003F5E4
data l_headerNode0 101D1C1C
data l_headerNode1 101D1C28
data l_hioNode 101D1C34
str 1003F5B8 in 024ABDB0
str 1003F5C0
str 1003F5D4 in 024ABF4C
str 1003F5DC
str 1003F5E8 in 024AC24C
str 1003F624 in 024AC3E4
ext 025200D4 KfPlay* kfGetPlay()
ext 025D4604 u32 kfIsActor(void*)
ext 02048038 u32 kfGetActorParam(void*,u32,u32)
ext 0259F7D4 void kfSetActorInfo(void*,const char*,void*)
ext 025A1458 void kfNpcConstruct(void*)
ext 0259F740 void kfEventCutConstruct(void*)
ext 025F0A10 u32 kfHioCreateChild(const char*,u32)
ext 025F0A18 void kfHioDeleteChild(s32)
ext 025445B8 u32 kfIsPresent(void*,const char*)
ext 0254487C u32 kfGetSubstance(void*,s32,const char*,u32)
ext 0252012C void kfSetNextStage(void*,u32,s32,s32,u32,u32,u32,f32)
ext 025D5218 u32 kfSearchByJudge(u32,u32)
ext 025A11EC u32 kfTalk(void*,u32)
ext 02542D88 s32 kfGetMyStaffId(void*,const char*,u32,u32)
ext 02542EDC u32 kfGetMyActIdx(void*,s32,u32,u32,u32,u32)
ext 02543280 void kfCutEnd(void*,s32)
ext 025447C8 u32 kfIsAdvance(void*,s32)
ext 0254457C u32 kfIsEnd(void*,const char*)
ext 0259F858 u32 kfCutProc(void*)
ext 025D76A8 void kfOrderSpeakEvent(void*)
ext 025D77DC void kfOrderOtherEvent(void*,u32,u32,u32)
ext C000A848 void kfCopyBytes(u32,u32,u32)
ext 028E8DE8 f32 kfSquareDistance(const cXyz*,const cXyz*)
ext 028F4384 f32 kfSqrt(f32)
ext 025DE508 void kfSearchAll(u32,void*)
ext 0273AD10 daTag_Kf1_HIO_c* kfAllocate(u32)
ext 028F026C void kfRegisterGlobal(u32)
ext 028ED6F8 void kfHeaderByteConstruct(u32)
ext 028EAB2C void kfHeaderFlagConstruct(u32)
ext 02515A70 void kfAcchDestroy(void*,u32)
ext 02515860 void kfAcchCircleDestroy(void*,u32)
ext 02018034 void kfLineCheckDestroy(void*,u32)
ext 024EFD9C void kfActorAcchDestroy(void*,u32)
ext 025D50BC void kfActorDestroy(void*,u32)
ext 0273AF40 void kfFree(void*)
*/
// clang-format on
