/* Command Melody monuments. */
#include "d/actor/d_a_obj_hsehi1.h"
const gc_tu_data gc_tu = {.safeString = 0x1002ADF4};
// HD: private emitter/message/virtual views cover fields absent from the shared API.
struct HshEmitter : JPABaseEmitter {
    u8 _70[0x254 - 0x70];
    be<u32> mStatus;
};
enum HshMessageStatus {
    HshMessage_Typing = 6,
    HshMessage_Displayed = 14,
    HshMessage_Destroyed = 19,
    HshMessage_Input = 21
};
struct HshMessage {
    u8 _00[0x921];
    be<u8> mSend;
};
struct MessagePointer {
    gptr<HshMessage> pointer;
};
struct MessageId {
    be<u32> value;
};
static MessagePointer messageControl;
static MessageId l_msgId;
static daObj_hsh_HIO_c l_HIO;
struct HioWords {
    be<u32> words[4];
};
static HioWords hio_init_data;
struct HioLabel {
    be<u8> bytes[64];
};
static HioLabel tact_hio_label;
static HioLabel message_hio_label;
static HshAction wait_action;
static HshAction off_action;
static HshAction talk_action;
static HshAction delete_action;
using EventAction = gabi::ptmf<s32 (daObj_hsh_c::*)(s32)>;
struct EventActions {
    EventAction entries[8];
};
static EventActions event_init_tbl;
static EventActions event_action_tbl;
struct EventNames {
    gptr<char> names[2];
};
static EventNames event_name_tbl;
struct CutNames {
    gptr<char> names[8];
};
static CutNames cut_name_tbl;
struct HshPlayerVtable {
    u8 _00[0x2C];
    be<u32> getTactMusic;
};
struct HshPlayerView {
    u8 _00[0xB4];
    gptr<HshPlayerVtable> vtable;
};
static u32 getTactMusic(fopAc_ac_c *player) {
    auto *view = reinterpret_cast<HshPlayerView *>(player);
    return gabi::call_ptr<u32>(view->vtable->getTactMusic, player);
}
static void action_from(daObj_hsh_c *actor, const HshAction &source, void *arg = nullptr) {
    gabi::Local<HshAction> next;
    next->d = (s16)source.d;
    next->i = (s16)source.i;
    next->f = (u32)source.f;
    actor->setAction(next.get(), arg);
}
static HshMessage *message_manager() { return messageControl.pointer; }
extern void seStartLevel(u32, const cXyz *, u32, s32);
static void sound(daObj_hsh_c *actor, u32 id, bool current = false) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    seStartLevel(id, current ? &actor->current.pos : &actor->eyePos, 0, reverb);
}
extern s16 Hsh_XyEvent(daObj_hsh_c *, s32);
extern s32 Hsh_chkAttention(daObj_hsh_c *, cXyz *, s16);
extern void modelUpdate(J3DModel *);
extern u32 messageSet(HshMessage *, u32, const cXyz *);
extern u32 messageStatus(HshMessage *);
extern void setMessageStatus(HshMessage *, u32);
extern void setEventName(dEvt_info_c *, const char *);
extern u8 calcTimer(be<u8> *);
extern f32 sqrtf_guest(f32);
extern s32 SetBackground(dBgW *, cBgD_t *, u32, Mtx34 *);
extern s32 GetRoomId(dBgS *, cBgS_PolyInfo *);
extern u8 GetPolyColor(dBgS *, cBgS_PolyInfo *);
extern s8 createHioChild(const char *, daObj_hsh_HIO_c *);
extern void deleteHioChild(s8);
extern void destroyCircle(cM3dGCir_l *, s32);
extern void destroyActor(fopAc_ac_c *, s32);
extern void destroyObjAcch(dBgS_ObjAcch *, s32);
extern void constructHeader0(void *);
extern void constructHeader1(void *);
extern void registerHeader(void *);
extern void *copyString(void *, const void *);
static s16 daObj_hsh_XyCheckCB(void *, s32);
static s16 daObj_hsh_XyEventCB(void *, s32);
static s32 checkCreateHeap(fopAc_ac_c *);
// HD: string comparison stays inline; literals are translated to their registered guest bytes.
static bool hsh_streq(const be<u8> *a, const char *text) {
    const auto *b = gabi::at<be<u8>>(gabi::guest_addr(text));
    u8 left, right;
    do {
        left = *a++;
        right = *b++;
    } while (left == right && left != 0);
    return left == right;
}
extern void cutEnd(void *, s32);
extern s32 getIsAdvance(void *, s32);
struct StaticFloat {
    be<f32> value;
};
struct StaticFlag {
    be<u8> value;
};
struct StaticRecord {
    be<u32> words[3];
};
static HioWords staticZeros;
static StaticFloat staticMinimum;
static StaticFloat staticMaximum;
static StaticFlag staticFlag0;
static StaticFlag staticFlag1;
static StaticRecord staticRecord0;
static StaticRecord staticRecord1;
static StaticRecord staticRecord2;
/* 0235A374 */
s16 daObj_hsh_c::XyCheckCB(s32 button) { return dComIfGp_getSelectItem(button) == 0x22; }
/* 0235A3B4 */
static s16 daObj_hsh_XyCheckCB(void *actor, s32 button) { return static_cast<daObj_hsh_c *>(actor)->XyCheckCB(button); }
/* 0235A3B8 */
s16 daObj_hsh_c::XyEventCB(s32 button) {
    sound(this, 0x8A7);
    u32 flags = mFlags;
    s16 event = mEventId[0];
    mEventSelector = 0;
    mFlags = flags | 1;
    return event;
}
/* 0235A418 */
static s16 daObj_hsh_XyEventCB(void *actor, s32 button) {
    return Hsh_XyEvent(static_cast<daObj_hsh_c *>(actor), button);
}
/* 0235A5C0 */
s32 daObj_hsh_c::draw() {
    // HD: the verified draw path omits the GC shadow submission.
    if (!((u32)mFlags & 8)) {
        settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
        modelUpdate(mpModel);
    }
    return TRUE;
}
/* 0235A624 */
static s32 draw_actor(daObj_hsh_c *a) { return a->draw(); }
/* 0235A628 */
s32 daObj_hsh_c::checkCommandTalk() {
    if (eventInfo.checkCommandTalk()) {
        u32 mode = dComIfGp_event_getTalkXYBtn();
        if (mode - 1 > 3)
            return TRUE;
        if (field_0x514 == 5)
            field_0x514 = -1;
    }
    return FALSE;
}
/* 0235A6AC */
void daObj_hsh_c::eventEnd() {
    dComIfGp_event_reset();
    u32 flags = mFlags;
    mEventSelector = -1;
    mFlags = flags & ~1u;
}
/* 0235AAA0 */
void daObj_hsh_c::checkOrder() {
    if (eventInfo.checkCommandTalk() && (u32)((s32)field_0x514 - 3) <= 2) {
        field_0x514 = -1;
        u32 mode = dComIfGp_event_getTalkXYBtn();
        if (mode - 1 > 3)
            action_from(this, talk_action);
    }
}
/* 0235B2B4 */
void daObj_hsh_c::emitterDelete(gptr<HshEmitter> *emitter) {
    HshEmitter *e = *emitter;
    if (e) {
        e->mStatus = (u32)e->mStatus & ~0x40u;
        e = *emitter;
        u32 flags = e->mStatus;
        e->mMaxFrame = -1;
        e->mStatus = flags | 1;
        *emitter = nullptr;
    }
}
/* 0235AEA8 */
void daObj_hsh_c::onOffDraw() {
    u32 flags = mFlags;
    dBgW *bg = mpBgW;
    mFlags = flags | 8;
    if (bg) {
        dBgS *manager = dComIfG_Bgsp();
        manager->Release(mpBgW);
    }
}
/* 0235B3FC */
void daObj_hsh_c::offOffDraw() {
    u32 flags = mFlags;
    dBgW *bg = mpBgW;
    mFlags = flags & ~8u;
    if (bg) {
        dBgS *manager = dComIfG_Bgsp();
        manager->Regist(mpBgW, this);
    }
}
/* 0235B450 */
void daObj_hsh_c::drawStart() { offOffDraw(); }
/* 0235B454 */
void daObj_hsh_c::drawStop() {
    onOffDraw();
    emitterDelete(&mpEmitter);
}
/* 0235B3F0 */
void daObj_hsh_c::initialDefault(s32 staff) {}
/* 0235B3F4 */
s32 daObj_hsh_c::actionDefault(s32 staff) { return TRUE; }
/* 0235B87C */
s32 daObj_hsh_c::talk_init() {
    // HD: a non-sentinel message ID succeeds without the GC SearchByID check.
    u32 id = l_msgId.value;
    HshMessage *manager = message_manager();
    if (id == 0xFFFFFFFF) {
        u32 new_id = messageSet(manager, mMsgNo, &eyePos);
        l_msgId.value = new_id;
        return FALSE;
    }
    return TRUE;
}
/* 0235B8E8 */
s32 daObj_hsh_c::actionMsgSetEvent(s32 staff) { return talk_init(); }
/* 0235B8EC */
u32 daObj_hsh_c::next_msgStatus(be<u32> *message) {
    u32 n = *message;
    if (n == 0 || n == 0xEF3 || n == (u32)mPrmMsgNo || n == 0x1901)
        return fopMsgStts_MSG_ENDS_e;
    return fopMsgStts_MSG_CONTINUES_e;
}
/* 0235BAD0 */
s32 daObj_hsh_c::actionMessageEvent(s32 staff) {
    auto *p = dComIfGp_evmng_getMyIntegerP(staff, "prm0");
    return talk(p ? (s32)*p : 0);
}
/* 0235BB38 */
s32 daObj_hsh_c::actionTactEvent(s32 staff) {
    auto *p = dComIfGp_evmng_getMyIntegerP(staff, "prm0");
    u32 mode = p ? (u32)(s32)*p : 0;
    u32 music = getTactMusic(dComIfGp_getPlayer(0));
    if (music == mode)
        mFlags = (u32)mFlags | 4;
    return talk(1);
}
/* 0235BBD0 */
void daObj_hsh_c::initialJudgeEvent(s32 staff) {
    u32 flags = mFlags;
    if (flags & 4)
        mFlags = flags & ~4u;
    else if (flags & 2) {
        drawStart();
        mFlags = (u32)mFlags & ~2u;
        eventEnd();
    }
}
/* 0235BC44 */
void daObj_hsh_c::particle_set(u16 id) {
    dComIfGp_getParticle()->set(0, id, &current.pos, &current.angle, nullptr, 255, nullptr, -1, nullptr, nullptr,
                                nullptr);
}
/* 0235BCB4 */
// HD: distinct private spelling keeps both particle_set overloads addressable by the verifier.
void daObj_hsh_c::particle_setEmitter(gptr<HshEmitter> *emitter, u16 id) {
    if (!*emitter) {
        auto *e = reinterpret_cast<HshEmitter *>(dComIfGp_getParticle()->set(
            0, id, &current.pos, &current.angle, nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr));
        *emitter = e;
        if (e)
            e->mStatus = (u32)e->mStatus | 0x40;
    }
}
/* 0235BF68 */
void daObj_hsh_c::setAttention(bool set) {
    if (!set)
        return;
    f32 y = current.pos.y, extra_eye = (f32)l_HIO.prm.field_0x10, extra_attn = (f32)l_HIO.prm.field_0x0c;
    f32 eye = y + (argument == 0 ? 90.0f : 80.0f), attn = y + (argument == 0 ? 180.0f : 120.0f);
    f32 z = current.pos.z, x = current.pos.x;
    eyePos.x = x;
    eyePos.y = eye + extra_eye;
    eyePos.z = z;
    attention_info.position.x = x;
    attention_info.position.y = attn + extra_attn;
    attention_info.position.z = z;
}
/* 0235C260 */
u32 daObj_hsh_c::getMsg() { return argument == 0 ? 0x1901 : (u32)mPrmMsgNo; }
/* 0235C3AC */
s32 daObj_hsh_c::offAction(void *arg) {
    if (mActionMode == 0)
        mActionMode = 1;
    return TRUE;
}
/* 0235C3C8 */
s32 daObj_hsh_c::deleteAction(void *arg) {
    if (mActionMode == 0)
        mActionMode = 1;
    else if (mActionMode != -1)
        fopAcM_delete(this);
    return TRUE;
}
/* 0235A960 */
s32 daObj_hsh_c::setAction(HshAction *next, void *arg) {
    s16 new_index = next->i, old_index = mAction.i;
    s16 adjust;
    u32 target;
    if (old_index == new_index) {
        if (old_index == 0)
            return TRUE;
        adjust = next->d;
        target = next->f;
        if ((s16)mAction.d == adjust && (u32)mAction.f == target)
            return TRUE;
    } else {
        target = next->f;
        adjust = next->d;
    }
    if (old_index != 0) {
        mActionMode = -1;
        (this->*mAction)(arg);
    }
    mAction.f = target;
    mAction.i = new_index;
    mAction.d = adjust;
    field_0x522 = 0;
    field_0x520 = 0;
    mActionMode = 0;
    field_0x51c = 0;
    field_0x51e = 0;
    field_0x528 = 0;
    (this->*mAction)(arg);
    return TRUE;
}
/* 0235AB28 */
void daObj_hsh_c::action(void *arg) {
    s16 index = mAction.i;
    if (index == 0) {
        speedF = 0;
        action_from(this, wait_action);
    }
    (this->*mAction)(arg);
}
/* 0235ABE0 */
void daObj_hsh_c::eventOrder() {
    if ((u32)mFlags & 1)
        return;
    s32 order = field_0x514;
    if (order == 4 || order == 3) {
        s32 now = field_0x514;
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (now == 4)
            fopAcM_orderSpeakEvent(this);
    } else if (order == 5) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e);
        if (argument == 0)
            setEventName(&eventInfo, "hsehi1_talk");
    } else if (order != -1 && order < 2) {
        mEventSelector = order;
        s16 event = mEventId[(u32)order];
        fopAcM_orderOtherEventId(this, event, 255, 65535, 0, 1);
    }
}
/* 0235A6F8 */
s32 daObj_hsh_c::eventProc() {
    if (eventInfo.checkCommandDemoAccrpt() && field_0x514 != -1) {
        field_0x514 = -1;
        mFlags = (u32)mFlags | 1;
    }
    s32 staff = dComIfGp_evmng_getMyStaffId("Hsh", nullptr, 0);
    if (dComIfGp_event_runCheck() && checkCommandTalk() == 0) {
        if (staff != -1) {
            s32 act = dComIfGp_evmng_getMyActIdx(staff, cut_name_tbl.names, 8, 1, 0);
            void *manager = dComIfGp_getPEvtManager();
            if (act == -1)
                cutEnd(manager, staff);
            else {
                s32 advanced = getIsAdvance(manager, staff);
                if (advanced)
                    (this->*event_init_tbl.entries[act])(staff);
                if ((this->*event_action_tbl.entries[act])(staff) != 0)
                    dComIfGp_evmng_cutEnd(staff);
            }
        }
        if ((u32)mFlags & 1) {
            s32 selector = mEventSelector;
            s16 event = mEventId[(u32)selector];
            if (dComIfGp_evmng_endCheck(event))
                eventEnd();
            return TRUE;
        }
        if (staff != -1)
            return TRUE;
    }
    return FALSE;
}
/* 0235B924 */
s32 daObj_hsh_c::talk(s32 mode) {
    // HD: TactCorrect takes precedence over MesgCancelButton, unlike the GC order.
    HshMessage *manager = message_manager();
    u16 status = messageStatus(manager);
    dComIfGp_get();
    if (status == HshMessage_Displayed) {
        if (mode == 1) {
            if ((u32)mFlags & 4) {
                setMessageStatus(manager, fopMsgStts_MSG_ENDS_e);
                manager->mSend = 1;
                if ((u32)mMsgNo == 0x5B3) {
                    s32 room = current.roomNo;
                    dComIfGs_onSwitch(mSwitchNo, room);
                }
            } else {
                if (dComIfGp_checkMesgCancelButton() != 0) {
                    setMessageStatus(manager, fopMsgStts_MSG_ENDS_e);
                    manager->mSend = 1;
                    mFlags = (u32)mFlags | 2;
                }
            }
        } else {
            u32 next = next_msgStatus(&mMsgNo);
            setMessageStatus(manager, next);
            if (messageStatus(manager) == fopMsgStts_MSG_CONTINUES_e)
                messageSet(manager, mMsgNo, nullptr);
        }
    } else if (status == HshMessage_Input) {
        if (mode == 2)
            return TRUE;
    } else if (status != HshMessage_Typing && status == fopMsgStts_BOX_CLOSED_e) {
        setMessageStatus(manager, HshMessage_Destroyed);
        return TRUE;
    }
    return FALSE;
}
/* 0235B7F4 */
void daObj_hsh_c::initialMsgSetEvent(s32 staff) {
    l_msgId.value = 0xFFFFFFFF;
    mMsgNo = 0;
    auto *p = dComIfGp_evmng_getMyIntegerP(staff, "MsgNo");
    if (p) {
        u32 n = (u32)(s32)*p;
        mMsgNo = n;
        if (n == 0x5B3)
            dComIfGp_setMelodyNum(2);
    }
}
/* 0235BD54 */
void daObj_hsh_c::initialAppearEvent(s32 staff) {
    dComIfGs_onEventBit(0x2B10);
    particle_set(0x8270);
    particle_setEmitter(&mpEmitter, 0x8271);
    sound(this, 0x6A05, true);
    mAppearDeleteTimer = 30;
    action_from(this, wait_action);
}
/* 0235BE08 */
s32 daObj_hsh_c::actionAppearEvent(s32 staff) {
    if (calcTimer(&mAppearDeleteTimer) == 0) {
        offOffDraw();
        emitterDelete(&mpEmitter);
        return TRUE;
    }
    return FALSE;
}
/* 0235BE70 */
void daObj_hsh_c::initialDeleteEvent(s32 staff) {
    particle_set(0x8270);
    particle_setEmitter(&mpEmitter, 0x8271);
    sound(this, 0x6A05, true);
    mAppearDeleteTimer = 60;
    action_from(this, delete_action);
}
/* 0235BF0C */
s32 daObj_hsh_c::actionDeleteEvent(s32 staff) {
    if (calcTimer(&mAppearDeleteTimer) == 0) {
        drawStop();
        return TRUE;
    }
    return FALSE;
}
/* 0235C018 */
bool daObj_hsh_c::chkAttention(cXyz *pos, s16 angle) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    f32 z = (f32)player->current.pos.z - (f32)pos->z, x = (f32)player->current.pos.x - (f32)pos->x;
    f32 dist = l_HIO.prm.mAttnDist;
    s32 range = l_HIO.prm.mAttnAngle;
    // HD: fused multiply-add and f32 square-root result are both verified.
    f32 d = sqrtf_guest(gabi::fmadds(x, x, z * z));
    s16 target = cM_atan2s(x, z);
    if (mAttentionLatch) {
        dist += 40.0f;
        range += 0x71C;
    }
    s16 diff = (s16)((s32)target - angle);
    s32 magnitude = diff < 0 ? -(s32)diff : (s32)diff;
    return range > magnitude && dist > d;
}
/* 0235C144 */
s32 daObj_hsh_c::waitAction(void *arg) {
    s32 mode = mActionMode;
    if (mode == 0)
        mActionMode = 1;
    else if (mode != -1) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        s32 attentive = Hsh_chkAttention(this, pos.get(), (s16)shape_angle.y);
        u32 flags = attention_info.flags;
        mAttentionLatch = attentive;
        if (attentive) {
            s8 order = field_0x514;
            u32 type = argument;
            attention_info.flags = flags | 0x20000008;
            if (order == -1)
                field_0x514 = type == 0 ? 5 : 3;
        } else
            attention_info.flags = flags & ~0x20000008u;
        setAttention(true);
    }
    return TRUE;
}
/* 0235C27C */
s32 daObj_hsh_c::talkAction(void *arg) {
    s32 mode = mActionMode;
    if (mode == 0) {
        l_msgId.value = 0xFFFFFFFF;
        u32 msg = getMsg();
        u8 next = (u8)((u8)mActionMode + 1);
        s8 type = argument;
        mMsgNo = msg;
        mActionMode = next;
        if (type == 0) {
            auto *player = static_cast<daPy_py_c *>(dComIfGp_getLinkPlayer());
            player->onNoResetFlg0(0x08000000);
        }
    } else if (mode != -1) {
        if (mode == 1) {
            if (talk_init())
                mActionMode = (u8)((u8)mActionMode + 1);
        } else if (talk(0)) {
            action_from(this, wait_action);
            dComIfGp_event_reset();
            if (argument == 0) {
                auto *player = static_cast<daPy_py_c *>(dComIfGp_getLinkPlayer());
                player->offNoResetFlg0(0x08000000);
            }
        }
        setAttention(true);
    }
    return TRUE;
}
/* 0235AC8C */
void daObj_hsh_c::setBaseMtx() {
    J3DModel *model = mpModel;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), shape_angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
/* 0235AD5C */
s32 daObj_hsh_c::execute() {
    if (argument == 0) {
        bool on = dComIfGs_isEventBit(0x2B10);
        u32 status = actor_status;
        actor_status = on ? ((status & ~63u) | 0x24) : (status & ~0x20u);
    }
    mObjAcch.CrrPos(dComIfG_Bgsp());
    if (mObjAcch.GetGroundH() != -1000000000.0f) {
        auto *ground = static_cast<cBgS_PolyInfo *>(&mObjAcch.m_gnd);
        s32 room = GetRoomId(dComIfG_Bgsp(), ground);
        tevStr.mRoomNo = room;
        current.roomNo = room;
        u8 color = GetPolyColor(dComIfG_Bgsp(), ground);
        tevStr.mEnvrIdxOverride = color;
        u32 actor = mObjAcch.m_gnd.mActorId, background = mObjAcch.m_gnd.mpBgW;
        mPolyInfo.mActorId = actor;
        mPolyInfo.mpBgW = background;
        u16 polygon = mObjAcch.m_gnd.mPolyIndex, bg = mObjAcch.m_gnd.mBgIndex;
        mPolyInfo.mPolyIndex = polygon;
        mPolyInfo.mBgIndex = bg;
    }
    if (eventProc() == 0) {
        checkOrder();
        action(nullptr);
    }
    eventOrder();
    setBaseMtx();
    return TRUE;
}
/* 0235AE94 */
static s32 execute_actor(daObj_hsh_c *a) { return a->execute(); }
/* 0235AE98 */
static s32 isdelete_actor(daObj_hsh_c *a) { return TRUE; }
/* 0235AEA0 */
static s32 delete_actor(daObj_hsh_c *a) {
    // HD: this wrapper returns success without invoking the separately verified destructor.
    return TRUE;
}
/* 0235A41C */
s32 daObj_hsh_c::createHeap() {
    bool second = argument != 0;
    const char *arc = second ? "Hsehi2" : "Hsehi1";
    auto *data = (J3DModelData *)dComIfG_getObjectRes(arc, 4, 0x1002ADF4);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_hsehi1.cpp", second ? 0x211 : 0x1FB, "modelData != (0)");
    J3DModel *model = mDoExt_J3DModel__create(data, 0, 0x11020203);
    mpModel = model;
    if (!model)
        return FALSE;
    dBgW *bg = new_dBgW();
    mpBgW = bg;
    if (!bg)
        return FALSE;
    void *dzb = dComIfG_getObjectRes(arc, 7, 0x1002ADF4);
    return SetBackground(mpBgW, static_cast<cBgD_t *>(dzb), dBgW::MOVE_BG_e, &mMtx) == 0;
}
/* 0235A5BC */
static s32 checkCreateHeap(fopAc_ac_c *a) { return static_cast<daObj_hsh_c *>(a)->createHeap(); }
/* 0235AEF8 */
s32 daObj_hsh_c::init() {
    mEventSelector = -1;
    mAttentionLatch = 0;
    field_0x514 = -1;
    u32 prm = mParameters;
    mSwitchNo = prm & 255;
    mPrmMsgNo = (prm >> 8) & 65535;
    mAcchCir.SetWall(30, 30);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    setBaseMtx();
    dComIfG_Bgsp()->Regist(mpBgW, this);
    mpBgW->Move();
    if (argument == 0 && !dComIfGs_isEventBit(0x2B10)) {
        onOffDraw();
        action_from(this, off_action);
    } else
        action_from(this, wait_action);
    for (u32 i = 0; i < 2; i++) {
        const char *name = event_name_tbl.names[i];
        s16 event = dComIfGp_evmng_getEventIdx(name, 255);
        mEventId[i] = event;
    }
    eventInfo.setXyCheckCB(daObj_hsh_XyCheckCB);
    eventInfo.setXyEventCB(daObj_hsh_XyEventCB);
    return TRUE;
}
/* 0235B090 */
s32 daObj_hsh_c::create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1002AF70;
            dBgS_ObjAcch_ct(&mObjAcch, {0x1002AE1C, 0x1002AE3C, 0x1002AE2C});
            dBgS_AcchCir_ct(&mAcchCir);
            mPolyInfo.mpBgW = 0;
            mPolyInfo.mPolyIndex = 65535;
            mPolyInfo.__vtbl = 0x1002AE0C;
            mPolyInfo.mBgIndex = 256;
            mPolyInfo.mActorId = 0xFFFFFFFF;
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    if (argument == 0 && dComIfGs_isEventBit(0x2510))
        return cPhs_ERROR_e;
    const char *arc = argument == 0 ? "Hsehi1" : "Hsehi2";
    s32 phase = dComIfG_resLoad(&mPhase, arc);
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, checkCreateHeap, 0x4000)) {
            mpBgW = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
        if (l_HIO.mNo < 0) {
            const char *name = reinterpret_cast<const char *>(argument == 0 ? &tact_hio_label : &message_hio_label);
            s8 no = createHioChild(name, &l_HIO);
            l_HIO.mpActor = this;
            l_HIO.mNo = no;
        }
        if (init() == 0)
            phase = cPhs_ERROR_e;
    }
    return phase;
}
/* 0235B2B0 */
static s32 create_actor(daObj_hsh_c *a) { return a->create(); }
/* 0235B2F0 */
static void daObj_hsh_destruct(daObj_hsh_c *a, s32 flags) {
    if (a) {
        const char *arc = a->argument == 0 ? "Hsehi1" : "Hsehi2";
        a->__vtbl = 0x1002AF70;
        dComIfG_resDelete(&a->mPhase, arc);
        if (a->mpBgW)
            dComIfG_Bgsp()->Release(a->mpBgW);
        a->emitterDelete(&a->mpEmitter);
        s8 child = l_HIO.mNo;
        if (child >= 0) {
            deleteHioChild(child);
            l_HIO.mNo = -1;
        }
        destroyCircle(&a->mAcchCir.m_cir, 2);
        static_cast<dBgS_GrpPassChk &>(a->mObjAcch).__vtbl = 0x1002AE2C;
        static_cast<dBgS_PolyPassChk &>(a->mObjAcch).__vtbl = 0x1002AE3C;
        destroyObjAcch(&a->mObjAcch, 0);
        destroyActor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}
/* 0235C410 */
static void *hio_ct(void *p) {
    if (!p) {
        p = operator_new(sizeof(daObj_hsh_HIO_c));
        if (!p)
            return p;
    }
    auto *hio = static_cast<daObj_hsh_HIO_c *>(p);
    hio->mNo = -1;
    hio->__vtbl = 0x1002AE4C;
    auto *words = reinterpret_cast<HioWords *>(&hio->prm);
    for (u32 i = 0; i < 4; i++)
        words->words[i] = (u32)hio_init_data.words[i];
    return p;
}
/* 0235C47C */
static void sinit() {
    for (u32 i = 0; i < 4; i++)
        staticZeros.words[i] = 0;
    registerHeader(&staticRecord0);
    staticMinimum.value = -3.1415927f;
    staticMaximum.value = 3.1415927f;
    constructHeader0(&staticFlag0);
    registerHeader(&staticRecord1);
    constructHeader1(&staticFlag1);
    registerHeader(&staticRecord2);
    hio_ct(&l_HIO);
}
/* 0235C51C */
static void hio_dt(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 0235C530 */
static void hio_empty(void *p) {}
/* 0235B48C */
void daObj_hsh_c::initialLinkDispEvent(s32 staff) {
    // HD: keep the original 0x20-byte guest buffer, rather than the GC ten-byte declaration.
    struct Buffer {
        be<u8> bytes[0x20];
    };
    gabi::Local<Buffer> frame;
    auto *buf = frame->bytes;
    void *target = dComIfGp_evmng_getMySubstanceP(staff, "target", 4);
    bool player = false;
    if (target) {
        copyString(buf, target);
        player = hsh_streq(buf, "@PLAYER");
    }
    void *disp = dComIfGp_evmng_getMySubstanceP(staff, "disp", 4);
    if (!disp)
        return;
    copyString(buf, disp);
    if (player) {
        auto *link = static_cast<daPy_py_c *>(dComIfGp_getLinkPlayer());
        if (hsh_streq(buf, "on"))
            link->offNoResetFlg0(0x08000000);
        if (hsh_streq(buf, "off"))
            link->onNoResetFlg0(0x08000000);
    } else {
        if (hsh_streq(buf, "on"))
            drawStart();
        if (hsh_streq(buf, "off"))
            drawStop();
    }
}

/* WWHD glue
data messageControl 101F4B5C
data l_msgId 10469FE4
data l_HIO 1046A004
data hio_init_data 1002AF50
data tact_hio_label 1002AED8
data message_hio_label 1002AEE4
data wait_action 1002AD90
data off_action 1002AD98
data talk_action 1002ADA0
data delete_action 1002ADA8
data event_init_tbl 101CA1DC
data event_action_tbl 101CA21C
data event_name_tbl 101CA1D4
data cut_name_tbl 101CA25C
data staticZeros 10469FF4
data staticMinimum 10469FE8
data staticMaximum 10469FEC
data staticFlag0 10469FF0
data staticFlag1 10469FF1
data staticRecord0 101CA29C
data staticRecord1 101CA2A8
data staticRecord2 101CA2B4
str 1002AE74 "Hsehi1" in 0235A41C
str 1002AE7C "Hsehi2" in 0235A41C
str 1002AEC8 "Hsehi1" in 0235B090
str 1002AED0 "Hsehi2" in 0235B090
str 1002AEF4 "Hsehi1" in 0235B2F0
str 1002AEFC "Hsehi2" in 0235B2F0
str 1002AE84 "d_a_obj_hsehi1.cpp"
str 1002AE98 "modelData != (0)"
str 1002AEAC "Hsh"
str 1002AEB4 "hsehi1_talk"
str 1002AF04 "@PLAYER"
str 1002AF0C "off"
str 1002AF10 "target"
str 1002AF18 "disp"
str 1002AF20 "on"
str 1002AF24 "MsgNo"
str 1002AF2C "prm0" in 0235BAD0
str 1002AF34 "prm0" in 0235BB38
ext 025E1A40 void seStartLevel(u32,const cXyz*,u32,s32)
ext 0235A3B8 s16 Hsh_XyEvent(daObj_hsh_c*,s32)
ext 0235C018 s32 Hsh_chkAttention(daObj_hsh_c*,cXyz*,s16)
ext 025E2DA8 void modelUpdate(J3DModel*)
ext 025F7DB0 u32 messageSet(HshMessage*,u32,const cXyz*)
ext 025F795C u32 messageStatus(HshMessage*)
ext 025F74D0 void setMessageStatus(HshMessage*,u32)
ext 0253E9B0 void setEventName(dEvt_info_c*,const char*)
ext 0207A9A0 u8 calcTimer(be<u8>*)
ext 028F4384 f32 sqrtf_guest(f32)
ext 0200A030 s32 SetBackground(dBgW*,cBgD_t*,u32,Mtx34*)
ext 024EF130 s32 GetRoomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 GetPolyColor(dBgS*,cBgS_PolyInfo*)
ext 025F0A10 s8 createHioChild(const char*,daObj_hsh_HIO_c*)
ext 025F0A18 void deleteHioChild(s8)
ext 02018034 void destroyCircle(cM3dGCir_l*,s32)
ext 024EFD9C void destroyObjAcch(dBgS_ObjAcch*,s32)
ext 025D50BC void destroyActor(fopAc_ac_c*,s32)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 028F026C void registerHeader(void*)
ext 028F040C void* copyString(void*,const void*)
ext 02543280 void cutEnd(void*,s32)
ext 025447C8 s32 getIsAdvance(void*,s32)
*/
