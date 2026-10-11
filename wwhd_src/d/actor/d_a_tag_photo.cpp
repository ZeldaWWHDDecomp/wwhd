#include "d/actor/d_a_tag_photo.h"
#include "bindings.h"
// HD: message manager and EventCut have different APIs from GameCube.
namespace dSv_event_flag_c { enum { UNK_1601 = 0x1601 }; }
enum { PHOTO_HEAP_CALLBACK = 0x024B1CEC, fopMsgStts_MSG_DISPLAYED_e = 0x0E, fopMsgStts_MSG_DESTROYED_e = 0x13 };
static void* l_messageManager = {0};
static const u32 l_msg_talk[] = {0x2A6D, 0x2A70, 0x2A73, 0x2A76, 0x2A79, 0x2A7C, 0x2A7F};
// HD: free phase handlers are represented by checked guest addresses.
static const u32 l_method[] = {0x024B1D94, 0x024B1E9C, 0};
static const char* cut_name_tbl[] = {"MES_SET"};
static daTagPhoto_c::PhotoProcEntry moveProc[] = {
    {&daTagPhoto_c::executeWait},
    {&daTagPhoto_c::executeTalk},
};

/* 024B26B0 */
u32 PrmAbstract(daTagPhoto_c* p, u32 width, u32 shift) {

    u32 left = width&63, right = shift&63;
    return (right >= 32?0:(u32)p->mParameters>>right) & ((left >= 32?0:1u<<left)-1);
}
/* 024B1CF4 */
u8 daTagPhoto_c::getPrmTagNo() {
    daTagPhoto_c* p = this;

    return (u8)PrmAbstract(p, 8, 0);
}
// HD: the out-of-line constructor may allocate when passed NULL.
/* 024B1D20 */
daTagPhoto_c* daTagPhoto_construct(daTagPhoto_c* p) {

    if (!p) p = static_cast<daTagPhoto_c*>(operator_new(sizeof(daTagPhoto_c)));
    if (p) {
        fopAc_ac_c_ct(p);
        p->__vtbl = 0x1003fbac;
        Photo_eventCutConstruct(&p->mEventCut);
        p->mTagNo = p->getPrmTagNo();
        p->field_0x31C = 0;
        p->mMode = 0;
    }
    return p;
}
/* 024B1D94 */
s32 phase_1(daTagPhoto_c* p) {

    u32 flags = p->actor_condition;
    if (!(flags&8)) {
        if (p) {
            daTagPhoto_construct(p);
            flags = p->actor_condition;
        }
        p->actor_condition = flags|8;
    }
    return 2;
}
/* 024B1DE8 */
s32 daTagPhoto_c::createInit() {
    daTagPhoto_c* p = this;

    p->mPhotoTalkEventIdx = dComIfGp_evmng_getEventIdx("PHOTO_TALK", 0xFF);
    p->mPhotoTalk2EventIdx = dComIfGp_evmng_getEventIdx("PHOTO_TALK2", 0xFF);
    Photo_setActorInfo(&p->mEventCut, "TagPo", p);
    s16 x = p->current.angle.x, y = p->current.angle.y, z = p->current.angle.z;
    p->shape_angle.x = x;
    p->shape_angle.z = z;
    p->shape_angle.y = y;
    p->attention_info.flags = fopAc_Attn_TALKFLAG_CHECK_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e;
    p->attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x23;
    s16 event = p->mPhotoTalk2EventIdx;
    p->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x24;
    p->eventInfo.setEventId(event);
    return 4;
}
/* 024B1CEC */
s32 daTagPhoto_c::createHeap() {

    return 1;
}
/* 024B1E9C */
s32 phase_2(daTagPhoto_c* p) {

    if (!Photo_entryHeap(p, PHOTO_HEAP_CALLBACK, 0x10000))return 5;
    p->createInit();
    return 4;
}
/* 024B1F00 */
s32 daTagPhoto_c::_create() {
    daTagPhoto_c* p = this;

    return Photo_phaseHandler(&p->mPhase, l_method, p);
}
/* 024B1F14 */
s32 daTagPhotoCreate(daTagPhoto_c* p) {

    return p->_create();
}
/* 024B1F18 */
s32 daTagPhotoDelete(daTagPhoto_c* p) {

    return 1;
}
/* 024B1F20 */
void daTagPhoto_c::setMessage(u32 msg) {
    daTagPhoto_c* p = this;

    p->mMsgNo = msg;
    p->mMsgID = fpcM_ERROR_PROCESS_ID_e;
}
/* 024B1F30 */
void daTagPhoto_c::eventMesSetInit(s32 staff) {
    daTagPhoto_c* p = this;

    auto msg = Photo_getMyIntegerP(dComIfGp_getPEvtManager(), staff, "MsgNo", 3);
    p->field_0x310 = nullptr;
    u32 number = 0;
    if (msg) {
        number = *msg;
        if (!number)number = l_msg_talk[(u8)p->mTagNo];
    }
    p->setMessage(number);
}
/* 024B1FD4 */
u32 daTagPhoto_c::getMsg() {
    daTagPhoto_c* p = this;

    u8 tag = p->mTagNo;
    p->field_0x310 = nullptr;
    return l_msg_talk[tag];
}
/* 024B1FF0 */
u16 daTagPhoto_c::next_msgStatus(be<u32>* out) {
    daTagPhoto_c* p = this;

    auto seq = p->field_0x310.get();
    if (!seq)return fopMsgStts_MSG_ENDS_e;
    seq = seq+1;
    p->field_0x310 = seq;
    u32 number = *seq;
    if (!number) {
        p->field_0x310 = nullptr;
        return fopMsgStts_MSG_ENDS_e;
    }
    *out = number;
    return fopMsgStts_MSG_CONTINUES_e;
}
/* 024B2038 */
u16 daTagPhoto_c::talk(s32 mode) {
    daTagPhoto_c* p = this;

    u32 id = p->mMsgID;
    void* manager = l_messageManager;
    u16 status = 0xff;
    if (id == fpcM_ERROR_PROCESS_ID_e) {
        u32 msg;
        if (mode == 1) {
            msg = p->getMsg();
            p->mMsgNo = msg;
        }
        else msg = p->mMsgNo;
        s32 made = Photo_messageSet(manager, msg, &p->eyePos);
        p->mMsgID = made;
        if (made != -1)p->mMessageActive = 0;
    }
    else if (p->mMessageActive) {
        status = Photo_getStatus(manager);
        if (status == fopMsgStts_MSG_DISPLAYED_e) {
            u16 next = p->next_msgStatus(&p->mMsgNo);
            Photo_setStatus(manager, next);
            if (Photo_getStatusInt(manager) == fopMsgStts_MSG_CONTINUES_e) {
                u32 msg = p->mMsgNo;
                Photo_messageSet(manager, msg, nullptr);
            }
        }
        else if (status == fopMsgStts_BOX_CLOSED_e) {
            Photo_setStatus(manager, fopMsgStts_MSG_DESTROYED_e);
            p->mMsgID = fpcM_ERROR_PROCESS_ID_e;
        }
    }
    else p->mMessageActive = 1;
    return status;
}
/* 024B216C */
bool daTagPhoto_c::eventMesSet() {
    daTagPhoto_c* p = this;

    return p->talk(0) == fopMsgStts_BOX_CLOSED_e;
}
/* 024B219C */
void daTagPhoto_c::privateCut() {
    daTagPhoto_c* p = this;

    s32 staff = dComIfGp_evmng_getMyStaffId("TagPo", 0, 0);
    if (staff == -1)return;
    s8 act = Photo_getMyActIdx(dComIfGp_getPEvtManager(), staff, cut_name_tbl, 1, 1, 0);
    p->mActIdx = act;
    dEvent_manager_c* manager = dComIfGp_getPEvtManager();
    if (act != -1) {
        if (Photo_isAdvance(manager, staff)) {
            if (p->mActIdx != 0) {
                dComIfGp_evmng_cutEnd(staff);
                return;
            }
            p->eventMesSetInit(staff);
        }
        if (p->mActIdx == 0&&!p->eventMesSet())return;
        manager = dComIfGp_getPEvtManager();
    }
    Photo_cutEnd(manager, staff);
}
/* 024B228C */
void daTagPhoto_c::eventMove() {
    daTagPhoto_c* p = this;

    s16 event = p->mPhotoTalkEventIdx;
    bool done = dComIfGp_evmng_endCheck(event) != 0;
    if (!done) {
        event = p->mPhotoTalk2EventIdx;
        done = dComIfGp_evmng_endCheck(event) != 0;
    }
    if (done) {
        dComIfGp_event_reset();
        dComIfGs_onEventBit(dSv_event_flag_c::UNK_1601);
    }
    else {
        u8 flag = p->mEventCut.mAttnFlag;
        if (Photo_cutProc(&p->mEventCut)) {
            if (!p->mEventCut.mAttnFlag)p->mEventCut.mAttnFlag = flag;
        }
        else p->privateCut();
    }
}
/* 024B2374 */
s32 daTagPhoto_c::_execute() {
    daTagPhoto_c* p = this;

    if (!dComIfGp_event_runCheck()) {
        (this->*moveProc[mMode].proc)();
    }
    else p->eventMove();
    f32 z = p->current.pos.z, y = p->current.pos.y, x = p->current.pos.x;
    p->attention_info.position.z = z;
    p->attention_info.position.x = x;
    p->attention_info.position.y = y+50.0f;
    return 1;
}
/* 024B2480 */
s32 daTagPhotoExecute(daTagPhoto_c* p) {

    return p->_execute();
}
/* 024B2484 */
s32 daTagPhotoDraw(daTagPhoto_c* p) {

    return 1;
}
/* 024B248C */
void daTagPhoto_c::setMode(u32 mode) {
    daTagPhoto_c* p = this;

    if (mode == 1)p->mMsgID = fpcM_ERROR_PROCESS_ID_e;
    p->mMode = mode;
}
// Keep the ABI linkage words before both vectors: real nonleaf calls save LR at SP+4.
struct PhotoWaitFrame {
    u8 linkage[8];
    cXyz horizontal;
    cXyz difference;
};
/* 024B24A4 */
void daTagPhoto_c::executeWait() {
    daTagPhoto_c* p = this;

    gabi::Local<PhotoWaitFrame> frame;
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    cXyz_mi(&player->current.pos, &frame->difference, &p->current.pos);
    f32 x = frame->difference.x;
    frame->horizontal.x = x;
    f32 z = frame->difference.z;
    frame->horizontal.y = 0;
    frame->horizontal.z = z;
    f32 squared = PSVECSquareMag(&frame->horizontal);
    f32 distance = std_sqrtf(squared);
    if (distance<160.0f) {
        u16 flags = p->eventInfo.mCondition;
        u8 entered = p->field_0x31C;
        p->eventInfo.mCondition = flags|dEvtCnd_CANTALK_e;
        if (!entered) {
            p->field_0x31C = 1;
            auto npc = Photo_searchFromName("Po", 0xF, 0);
            if (npc&&!npc->isDate())p->eventInfo.setEventId(p->mPhotoTalkEventIdx);
        }
    }
}
/* 024B256C */
void daTagPhoto_c::executeTalk() {
    daTagPhoto_c* p = this;

    if (p->talk(1) == fopMsgStts_BOX_CLOSED_e) {
        dComIfGp_event_reset();
        p->setMode(0);
    }
}
/* 024B25C0 */
void photoStaticInit() {

    sinit_header_statics(0x1046E618, 0x101D2004);
}
/* 024B2654 */
s32 daTagPhotoIsDelete(daTagPhoto_c* p) {

    return 1;
}
/* 024B265C */
void photoDestructor(daTagPhoto_c* p, s32 flags) {

    if (p) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags&1)operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0259F740 void Photo_eventCutConstruct(PhotoEventCut*)
ext 0259F7D4 void Photo_setActorInfo(PhotoEventCut*, const char*, daTagPhoto_c*)
ext 0259F858 BOOL Photo_cutProc(PhotoEventCut*)
ext 025D63E8 s32 Photo_entryHeap(daTagPhoto_c*, u32, u32)
ext 02525FE4 s32 Photo_phaseHandler(request_of_phase_process_class*, const void*, daTagPhoto_c*)
ext 0254487C be<u32>* Photo_getMyIntegerP(dEvent_manager_c*, s32, const char*, s32)
ext 025F7DB0 s32 Photo_messageSet(void*, u32, cXyz*)
ext 025F795C u16 Photo_getStatus(void*)
ext 025F795C s32 Photo_getStatusInt(void*)
ext 025F74D0 void Photo_setStatus(void*, u16)
ext 02542EDC s32 Photo_getMyActIdx(dEvent_manager_c*, s32, const void*, s32, s32, s32)
ext 025447C8 BOOL Photo_isAdvance(dEvent_manager_c*, s32)
ext 02543280 void Photo_cutEnd(dEvent_manager_c*, s32)
ext 025D9F38 PhotoLenzo* Photo_searchFromName(const char*, u32, u32)
data l_messageManager 101F4B5C
data l_msg_talk 101D1FD8
data l_method 101D1FF4
data cut_name_tbl 101D2000
data moveProc 101D1FA8
str 1003FBD0 "PHOTO_TALK"
str 1003FBC4 "PHOTO_TALK2"
str 1003FBBC "TagPo" in 024B1DE8
str 1003FBE4 "TagPo" in 024B219C
str 1003FBDC "MsgNo"
str 1003FBEC "MES_SET"
str 1003FC00 "Po"
*/
