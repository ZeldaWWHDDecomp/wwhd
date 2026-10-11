// Medli/Makar event and warp trigger.
#include "d/actor/d_a_tag_md_cb.h"
#include "bindings.h"
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
using namespace mdcb_detail;
static Hio l_HIO;
using GuestSigned = be<s32>;
using GuestUnsigned = be<u32>;
using GuestHalf = be<u16>;
struct VtableWord {
    be<u32> first;
};
static GuestSigned l_HIO_counter;
static GuestUnsigned l_msgId;
struct MessageSlot {
    gptr<MessageManager> mpMessage;
};
static MessageSlot l_message;
static GuestHalf cb1_status;
struct staff_name_table {
    gptr<char> values[12];
};
static const staff_name_table staff_name;
struct event_name_tbl_table {
    gptr<char> values[7];
};
static const event_name_tbl_table event_name_tbl;
struct cut_name_tbl_table {
    gptr<char> values[6];
};
static const cut_name_tbl_table cut_name_tbl;
struct event_init_tbl_table {
    MdCbAction values[6];
};
static const event_init_tbl_table event_init_tbl;
struct event_action_tbl_table {
    MdCbAction values[6];
};
static const event_action_tbl_table event_action_tbl;
static const MdCbAction message_action;
static const MdCbAction warp_action;
struct EventBits {
    GuestHalf values[2];
};
static const EventBits event_bit;
static const VtableWord hio_vtable;
static const VtableWord actor_vtable;
static const HioPrm init_data;
static WordBlock header_zero;
static FloatPair header_pi;
static HeaderObject header_first;
static HeaderObject header_second;
static Registration first_registration;
static Registration second_registration;
static Registration third_registration;

// HD: singleton access remains a call at each original access.
static dComIfG_inf_c *play() {
    return reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
}
static EventManager *events() {
    return reinterpret_cast<EventManager *>(&play()->play.mEvtManager);
}
static BOOL eventBit(u16 bit) {
    return dComIfGs_isEventBit(bit);
}
static void onEventBit(u16 bit) {
    dComIfGs_onEventBit(bit);
}
static BOOL switchBit(daTag_MdCb_c *actor, u32 bit, bool roomArgument = true) {
    s8 room = actor->current.roomNo;
    return dComIfGs_isSwitch(bit, room);
}
static fopAc_ac_c *linkPlayer() {
    return daPy_getPlayerLinkActorClass();
}
static s32 invokeAction(const MdCbAction *entry, daTag_MdCb_c *self, u32 argument) {
    return (self->*entry->fn)(argument);
}
static void copyAction(MdCbAction *dst, const MdCbAction *src) {
    u32 a = src->words[0], b = src->words[1];
    dst->words[0] = a;
    dst->words[1] = b;
}
/* 024AE6E8 */
BOOL daTag_MdCb_c::checkCommandTalk() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return eventInfo.mCommand == 1;
}
/* 024AE694 */
s32 daTag_MdCb_c::getMyStaffId() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const char *name = staff_name.values[(s32)(s8)argument];
    return events()->getMyStaffId(name, 0, 0);
}
/* 024AE6FC */
void daTag_MdCb_c::eventEnd() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto *p = play();
    p->play.mEvtCtrl.mEventFlag = (u16)p->play.mEvtCtrl.mEventFlag | 8;
    u32 flags = m2AC;
    m2B5 = -1;
    m2AC = flags & ~1u;
}
/* 024AE968 */
BOOL daTag_MdCb_c::checkAreaIn(fopAc_ac_c *other) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!other)
        return FALSE;
    f32 distance = fopAcM_searchActorDistanceXZ2(this, other);
    f32 radius = scale.x;
    f32 otherY = other->current.pos.y, ownY = current.pos.y;
    f32 square = gabi::fmuls_ppc(radius, radius), difference = gabi::fsubs_ppc(otherY, ownY);
    if (!(distance < square) || difference < -10.0f || !(difference < (f32)scale.y))
        return FALSE;
    m2B9 = 1;
    return TRUE;
}
/* 024AE9FC */
BOOL daTag_MdCb_c::checkTimer() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return cLib_calcTimer(&m2B2) == 0;
}
/* 024AF18C */
BOOL daTag_MdCb_c::checkEventFinish() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch ((s8)argument) {
    case 2:
        return eventBit(0x3310);
    case 3:
        return eventBit(0x4180);
    case 4:
        return eventBit(0x3308);
    case 5:
        return eventBit(0x3304);
    case 6:
        return eventBit(0x3302);
    case 7:
        return eventBit(0x3301);
    case 8:
        return eventBit(0x3480);
    case 9:
        return eventBit(0x3440);
    case 10:
        return eventBit(0x3420);
    case 11:
        return eventBit(0x3410);
    default:
        return FALSE;
    }
}
/* 024AEFA0 */
BOOL daTag_MdCb_c::setAction(MdCbAction *next, void *argument) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 index = next->index, oldIndex = m290.index;
    if (oldIndex == index && oldIndex == 0)
        return TRUE;
    s16 delta = next->delta;
    u32 target = next->target;
    if (oldIndex == index && (s16)m290.delta == delta && (u32)m290.target == target)
        return TRUE;
    if (oldIndex != 0) {
        m2B6 = -1;
        invokeAction(&m290, this, gabi::ea(argument));
    }
    m290.target = target;
    m290.index = index;
    m290.delta = delta;
    m2D0 = 0;
    m2CE = 0;
    m2B6 = 0;
    m2CA = 0;
    m2CC = 0;
    m2D8 = 0.0f;
    invokeAction(&m290, this, gabi::ea(argument));
    return TRUE;
}
/* 024AF0E0 */
void daTag_MdCb_c::action(void *argument) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((s16)m290.index == 0) {
        gabi::Local<MdCbAction> initial;
        copyAction(initial, &warp_action);
        setAction(initial, nullptr);
    }
    invokeAction(&m290, this, gabi::ea(argument));
}
/* 024AFD10 */
BOOL daTag_MdCb_c::warpAction(void *argument) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m2B6 == 0)
        m2B6 = 1;
    else if (m2B6 != -1 && this->argument <= 1) {
        u16 bit = event_bit.values[(s32)(s8)this->argument];
        onEventBit(bit);
    }
    return TRUE;
}
/* 024AF2CC */
void daTag_MdCb_c::eventOrder() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (checkEventFinish()) {
        m2B4 = -1;
        return;
    }
    s8 pending = m2B4;
    if (pending != -1 && pending < 7) {
        m2B5 = pending;
        s16 id = m2BC[(s32)pending];
        fopAcM_orderOtherEventId(this, id, 0xFF, 0xFFFF, 0, 4);
    }
}
/* 024AF468 */
BOOL daTag_MdCb_c::init() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 parameter = mParameters;
    u8 lowX = static_cast<u8>((u16)current.angle.x);
    m29C = parameter & 0xFFFF;
    m2A4 = parameter >> 24;
    m2A0 = (parameter >> 16) & 0xFF;
    m2B0 = lowX;
    for (u32 i = 0; i < 7; i++) {
        const char *name = event_name_tbl.values[i];
        s32 id = events()->getEventIdx(name, 0xFF);
        m2BC[i] = id;
    }
    gabi::Local<MdCbAction> initial;
    copyAction(initial, argument > 1 ? &message_action : &warp_action);
    setAction(initial, nullptr);
    m2B4 = -1;
    s8 kind = argument;
    m2B7 = 0;
    m2BA = 0;
    if (kind == 8 || kind == 11) {
        kind = argument;
        m2B2 = 0;
    }
    if (kind == 3) {
        m2B2 = 60;
        scale.x = 2250.0f;
    } else
        m2B2 = l_HIO.mPrm.m0A;
    return TRUE;
}
/* 024AF5D0 */
static s32 daTag_MdCb_create(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = actor->actor_condition;
    if (!(flags & 8)) {
        if (actor) {
            fopAc_ac_c_ct(actor);
            flags = actor->actor_condition;
            actor->__vtbl = gabi::ea(&actor_vtable);
        }
        actor->actor_condition = flags | 8;
    }
    s8 kind = actor->argument;
    if (kind <= 1) {
        u16 bit = event_bit.values[(s32)kind];
        if (eventBit(bit))
            return cPhs_ERROR_e;
    } else {
        if (kind == 11)
            dComIfGs_offEventBit(0x3410);
        if (actor->checkEventFinish())
            return cPhs_ERROR_e;
    }
    if (l_HIO.mNo < 0) {
        s32 index = mDoHIO_createChild("Medli/Makar tag", &l_HIO);
        l_HIO.mNo = index;
        l_HIO.mpActor = actor;
        l_HIO_counter = 1;
    } else
        l_HIO_counter = (u32)l_HIO_counter + 1;
    if (actor->argument > 1)
        PSVECScale(&actor->scale, &actor->scale, 50.0f);
    return actor->init() ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}
/* 024AF360 */
BOOL daTag_MdCb_c::execute() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 kind = argument;
    m2B8 = (u8)m2B9;
    m2B9 = 0;
    if (kind <= 1) {
        scale.x = l_HIO.mPrm.m04;
        scale.y = l_HIO.mPrm.m00;
    }
    if (!eventProc()) {
        for (u32 i = 0; i < 7; i++) {
            const char *name = event_name_tbl.values[i];
            s32 id = events()->getEventIdx(name, 0xFF);
            m2BC[i] = id;
        }
        if (checkCondition())
            action(nullptr);
        eventOrder();
    }
    return TRUE;
}
/* 024AE748 */
BOOL daTag_MdCb_c::eventProc() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (eventInfo.mCommand != 2)
        return FALSE;
    if (m2B4 != -1) {
        u32 flags = m2AC;
        m2B4 = -1;
        m2AC = flags | 1;
    }
    s32 staff = getMyStaffId();
    if (!play()->play.mEvtCtrl.mMode || checkCommandTalk())
        return FALSE;
    if (staff != -1) {
        s32 actionIndex = events()->getMyActIdx(staff, cut_name_tbl.values, 6, 1, 0);
        auto *manager = events();
        if (actionIndex == -1)
            manager->cutEnd(staff);
        else {
            BOOL advance = manager->getIsAddvance(staff);

            if (advance)
                invokeAction(&event_init_tbl.values[actionIndex], this, staff);
            if (invokeAction(&event_action_tbl.values[actionIndex], this, staff))
                events()->cutEnd(staff);
        }
    }
    s16 eventId = m2BC[(s32)(s8)m2B5];
    if (events()->endCheck(eventId))
        eventEnd();
    return TRUE;
}
/* 024AF974 */
BOOL daTag_MdCb_c::talk_init() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 id = l_msgId;
    MessageManager *manager = l_message.mpMessage;
    if (id == fpcM_ERROR_PROCESS_ID_e) {
        u32 message = m2A8;
        l_msgId = manager->messageSet(message, &eyePos);
        return FALSE;
    }
    return TRUE;
}
/* 024AF9E4 */
u16 daTag_MdCb_c::next_msgStatus(be<u32> *message) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 number = *message;
    switch (number) {
    case 0x19F0:
        onEventBit(0x3320);
        m2B2 = l_HIO.mPrm.m08;
        break;
    case 0x19F1:
        onEventBit(0x3310);
        onEventBit(0x4001);
        m2B2 = l_HIO.mPrm.m08;
        break;
    case 0x19F2:
    case 0x19F4:
        m2B2 = l_HIO.mPrm.m08;
        break;
    case 0x19F3:
        onEventBit(0x3304);
        break;
    case 0x19F5:
        onEventBit(0x3301);
        break;
    case 0x1530:
        onEventBit(0x3480);
        break;
    case 0x1531:
        onEventBit(0x3440);
        break;
    case 0x1533:
        *message = 0x1535;
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1534:
        onEventBit(0x3420);
        break;
    case 0x1535:
        onEventBit(0x3410);
        break;
    }
    return fopMsgStts_MSG_ENDS_e;
}
/* 024AFB68 */
BOOL daTag_MdCb_c::talk() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    MessageManager *manager = l_message.mpMessage;
    u16 status = manager->getStatus();
    play();
    if (status == fopMsgStts_MSG_DISPLAYED_e) {
        u16 next = next_msgStatus(&m2A8);
        manager->setStatus(next);
        if (manager->getStatus() == fopMsgStts_MSG_CONTINUES_e) {
            u32 message = m2A8;
            manager->messageSet(message, 0);
        }
    } else if (status != fopMsgStts_INPUT_e) {
        if (status == fopMsgStts_MSG_TYPING_e) {
            s8 kind = m2B5;
            if (kind == 0 || kind == 2 || kind == 3 || kind == 5 || kind == 6) {
                fopAc_ac_c *actor = play()->play.mpPlayerPtr[1];
                if (actor) {
                    auto *p = play();
                    auto *table = gabi::at<NpcVtable>(actor->__vtbl);
                    u32 target = table->setMessageAnimation;
                    u8 animation = p->play.mMesgAnime;
                    gabi::call_ptr(target, actor, animation);
                }
            }
        } else if (status == fopMsgStts_BOX_CLOSED_e) {
            manager->setStatus(fopMsgStts_MSG_DESTROYED_e);
            return TRUE;
        }
    }
    return FALSE;
}
/* 024AF7F8 */
void daTag_MdCb_c::initialInitEvent(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c *actor = play()->play.mpPlayerPtr[1];
    dComIfGp_event_setTalkPartner(actor);
}
/* 024AF844 */
void daTag_MdCb_c::initialMsgSetEvent(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
    be<u32> *substance = events()->getMySubstanceP(staff, "msg_number", 3);
    u32 number = substance ? (u32)*substance : (u32)m29C;
    m2A8 = number;
    if (number == 0x19F3 || number == 0x1531) {
        fopAc_ac_c *actor = play()->play.mpPlayerPtr[1];
        if (actor) {
            u32 sound = m2A8 == 0x19F3 ? 0x4993 : 0x4992;
            auto *p = play();
            gabi::Local<cXyz> direction;
            direction->x = 0;
            direction->y = 1;
            direction->z = 0;
            p->play.mVibration.StartShock( 4, -0x21, direction.get());
            s8 room = actor->current.roomNo;
            s32 reverb = dComIfGp_getReverb(room);
            if (actor && gabi::ea(&actor->current.pos)) {
                u32 id = actor->mBsPcId;
                mDoAud_monsSeStart(sound, &actor->current.pos, id, reverb);
            }
        }
    }
}
/* 024AFCB0 */
void daTag_MdCb_c::initialPlayerOffDrow(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    daPy_py_c *actor = static_cast<daPy_py_c *>((fopAc_ac_c *)play()->play.mpPlayerPtr[0]);
    actor->mNoResetFlg0 = (u32)actor->mNoResetFlg0 | daPy_py_c::daPyFlg0_NO_DRAW;
}
/* 024AFCE0 */
void daTag_MdCb_c::initialPlayerOnDrow(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    daPy_py_c *actor = static_cast<daPy_py_c *>((fopAc_ac_c *)play()->play.mpPlayerPtr[0]);
    actor->mNoResetFlg0 = (u32)actor->mNoResetFlg0 & ~u32(daPy_py_c::daPyFlg0_NO_DRAW);
}
/* 024AFD80 */
BOOL daTag_MdCb_c::messageAction(void *parameter) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m2B6 == 0) {
        m2B6 = 1;
        return TRUE;
    }
    if (m2B6 == -1 || argument <= 1 || checkEventFinish())
        return TRUE;
    fopAc_ac_c *actor = play()->play.mpPlayerPtr[1];
    if (!actor)
        return TRUE;
    auto *table = gabi::at<NpcVtable>(actor->__vtbl);
    u32 target = table->isTagCheckOK;
    if (!gabi::call_ptr<BOOL>(target, actor))
        return TRUE;
    s16 name = actor->mProcName;
    if (name == fpcNm_NPC_MD_e) {
        if (argument == 5)
            m2B4 = 2;
        else if ((u32)actor->actor_status & fopAcStts_CARRY_e) {
            if (!((u32) reinterpret_cast<MedliView *>(actor)->m30F0 & daMdStts_CARRY_ACTION))
                m2B4 = 1;
        } else
            m2B4 = 0;
    } else if (actor->mProcName == fpcNm_NPC_CB1_e) {
        s8 kind = argument;
        if (kind == 9)
            m2B4 = 5;
        else if (kind == 11)
            m2B4 = 6;
        else if ((u32)actor->actor_status & fopAcStts_CARRY_e) {
            if (!((u16)cb1_status & daCbStts_NO_CARRY_ACTION))
                m2B4 = 4;
        } else
            m2B4 = 3;
    }
    return TRUE;
}
/* 024AEA28 */
BOOL daTag_MdCb_c::checkCondition() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c *other = (fopAc_ac_c *)play()->play.mpPlayerPtr[1];
    if (!other)
        return FALSE;
    s8 room = other->current.roomNo;
    if (room == -1 || room != (s8)current.roomNo)
        return FALSE;
    s8 kind = argument;
    if (kind <= 1)
        return checkAreaIn(other) ? TRUE : FALSE;
    if (kind == 2) {
        if (eventBit(0x3310) || !switchBit(this, m2B0))
            return FALSE;
        auto *p = play();
        fopAc_ac_c *player = p->play.mPlayerInfo[0].mpPlayer;
        f32 distance = fopAcM_searchActorDistanceXZ2(other, player);
        return !(distance < 250000.0f) ? FALSE : checkTimer();
    }
    if (kind == 3) {
        if (switchBit(this, m2B0, false)) {
            onEventBit(0x4180);
            onEventBit(0x3310);
        }
        if (eventBit(0x4180) || !eventBit(0x3310))
            return FALSE;
        if (eventBit(0x4001))
            return checkTimer();
        if (checkAreaIn(linkPlayer()) || checkAreaIn(other))
            return checkTimer();
    } else if (kind == 4) {
        if (switchBit(this, m2B0, false))
            onEventBit(0x3308);
        if (switchBit(this, m2A0) && !eventBit(0x3308) && checkAreaIn(other))
            return checkTimer();
    } else if (kind == 5) {
        if (eventBit(0x3404) && checkAreaIn(linkPlayer()))
            return TRUE;
    } else if (kind == 6) {
        if (!dComIfGs_isCollect(1, 1))
            return FALSE;
        if (switchBit(this, m2B0))
            onEventBit(0x3302);
        if (eventBit(0x3302))
            return FALSE;
        u32 count = m2A4, index = m2A0, i = 0;
        while (i < count) {
            if (switchBit(this, index)) {
                m2BA = 1;
                break;
            }
            count = m2A4;
            ++i;
            ++index;
        }
        if (checkAreaIn(other) && m2BA)
            return checkTimer();
    } else if (kind == 7) {
        if (switchBit(this, m2B0, false))
            onEventBit(0x3301);
        u32 count = m2A4, index = m2A0, i = 0;
        while (i < count) {
            if (!switchBit(this, index))
                break;
            count = m2A4;
            ++i;
            ++index;
        }
        if (!eventBit(0x3301) && i == (u32)m2A4 && checkAreaIn(other))
            return checkTimer();
    } else if (kind == 8) {
        if (switchBit(this, m2B0, false))
            onEventBit(0x3480);
        if (!eventBit(0x3480) && checkAreaIn(other))
            return checkTimer();
    } else if (kind == 9) {
        if (eventBit(0x3408) && checkAreaIn(linkPlayer()))
            return TRUE;
    } else if (kind == 10) {
        if (!dComIfGs_checkGetItem(dItemNo_HOOKSHOT_e))
            return FALSE;
        u32 count = m2A4, index = m2A0, i = 0;
        while (i < count) {
            if (switchBit(this, index))
                return checkTimer();
            count = m2A4;
            ++i;
            ++index;
        }
    } else if (kind == 11) {
        if (switchBit(this, m2B0, false))
            onEventBit(0x3410);
        if (!eventBit(0x3410) && eventBit(0x3440) && checkAreaIn(linkPlayer()))
            return TRUE;
    }
    return FALSE;
}
/* 024AE68C */
static BOOL daTag_MdCb_Draw(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}
/* 024AF434 */
static BOOL daTag_MdCb_Execute(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    actor->execute();
    return TRUE;
}
/* 024AF458 */
static BOOL daTag_MdCb_IsDelete(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}
// HD: profile Delete is empty. Virtual destruction still releases HIO ownership.
/* 024AF460 */
static BOOL daTag_MdCb_Delete(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}
/* 024AF748 */
static s32 daTag_MdCb_Create(daTag_MdCb_c *actor) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return daTag_MdCb_create(actor);
}
/* 024AF7EC */
void daTag_MdCb_c::initialDefault(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}
/* 024AF7F0 */
BOOL daTag_MdCb_c::actionDefault(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}
/* 024AF9E0 */
BOOL daTag_MdCb_c::actionMsgSetEvent(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return talk_init();
}
/* 024AFCAC */
BOOL daTag_MdCb_c::actionMessageEvent(s32 staff) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return talk();
}
/* 024AF74C */
static void daTag_MdCb_destructor(daTag_MdCb_c *actor, s32 flags) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!actor)
        return;
    actor->__vtbl = gabi::ea(&actor_vtable);
    s32 count = l_HIO_counter;
    bool release = true;
    if (count) {
        count = (s32)((u32)count - 1);
        l_HIO_counter = count;
        release = count <= 0;
    }
    if (release) {
        s8 child = l_HIO.mNo;
        if (child >= 0) {
            mDoHIO_deleteChild(child);
            l_HIO.mNo = -1;
        }
    }
    mdcb_deleteActorBase(actor, 0);
    if (flags & 1)
        operator_delete(actor);
}
/* 024AFF08 */
static Hio *daTag_MdCb_HIO_ctor(Hio *object) {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!object)
        object = static_cast<Hio *>(operator_new(0x20));
    if (object) {
        object->mNo = -1;
        object->mVtable = gabi::ea(&hio_vtable);
        for (u32 i = 0; i < 5; i++)
            object->mPrm.words[i] = init_data.words[i];
    }
    return object;
}
/* 024AFF7C */
static void __sinit_d_a_tag_md_cb_cpp() {
    // HD: preserve the verified GHS prologue and stack placement.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    header_zero.word[2] = 0;
    header_zero.word[0] = 0;
    header_zero.word[3] = 0;
    header_zero.word[1] = 0;
    __register_global_object(gabi::ea(&first_registration));
    header_pi.negative = -3.1415927410125732f;
    header_pi.positive = 3.1415927410125732f;
    mdcb_constructFirst(&header_first);
    __register_global_object(gabi::ea(&second_registration));
    mdcb_constructSecond(&header_second);
    __register_global_object(gabi::ea(&third_registration));
    daTag_MdCb_HIO_ctor(&l_HIO);
}

// clang-format off
/* WWHD glue:
data l_HIO 1046E5A8
data l_HIO_counter 101D1D94
data l_msgId 1046E598
data l_message 101F4B5C
data cb1_status 101BDC2A
data staff_name 101D1D64
data event_name_tbl 101D1E30
data cut_name_tbl 101D1DF8
data event_init_tbl 101D1D98
data event_action_tbl 101D1DC8
data message_action 1003F920
data warp_action 1003F928
data event_bit 1003F940
data hio_vtable 1003F954
data actor_vtable 1003FA70
data init_data 1003FA50
data header_zero 1046E5C8
data header_pi 1046E59C
data header_first 1046E5A4
data header_second 1046E5A5
data first_registration 101D1E4C
data second_registration 101D1E58
data third_registration 101D1E64
ext 02542D88 s32 EventManager::getMyStaffId(const char*,u32,u32)
ext 02543F10 s32 EventManager::getEventIdx(const char*,u32)
ext 02542EDC s32 EventManager::getMyActIdx(s32,const gptr<char>*,u32,u32,u32)
ext 02543280 void EventManager::cutEnd(s32)
ext 025447C8 BOOL EventManager::getIsAddvance(s32)
ext 025440C8 BOOL EventManager::endCheck(s16)
ext 0254487C be<u32>* EventManager::getMySubstanceP(s32,const char*,u32)
ext 025F795C u32 MessageManager::getStatus()
ext 025F74D0 void MessageManager::setStatus(u32)
ext 025F7DB0 u32 MessageManager::messageSet(u32,cXyz*)
ext 028E8E64 void PSVECScale(const cXyz*,cXyz*,f32)
ext 025E1A7C void mDoAud_monsSeStart(u32,cXyz*,u32,s32)
ext 025D50BC void mdcb_deleteActorBase(fopAc_ac_c*,s32)
ext 028ED6F8 void mdcb_constructFirst(HeaderObject*)
ext 028EAB2C void mdcb_constructSecond(HeaderObject*)
str 1003FA2C "Medli/Makar tag" renamed
str 1003FA44 "msg_number"
*/
