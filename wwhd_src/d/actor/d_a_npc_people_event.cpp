/**
 * d_a_npc_people_event.cpp (WWHD)
 * NPC - Windfall townspeople: event order/cut functions and the message-tag animations.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * eventGetItem and eventCameraStop (GameCube: `return true`) have no WWHD copy: privateCut
 * inlines them (their cases end the cut directly).
 */
#include "d/actor/d_a_npc_people_event.h"

enum { dItemNo_PICTO_BOX_e = 0x23, dItemNo_DELUXE_PICTO_BOX_e = 0x26, dItemNo_MAGIC_ARMOR_e = 0x2A, dItemNo_SWIFT_SAIL_e = 0x77 };

enum PeopleCut_e {
    CUT_MES_SET_e = 0,
    CUT_MES_SET_TP_e = 1,
    CUT_MES_SET2_e = 2,
    CUT_FLAG_SET_e = 3,
    CUT_GET_ITEM_e = 4,
    CUT_TURN_TO_PLAYER_e = 5,
    CUT_UB1_TALK_e = 6,
    CUT_UB1_TALK_XY_e = 7,
    CUT_UB2_TALK_e = 8,
    CUT_UB_SET_ANM_e = 9,
    CUT_AREA_MAX_e = 10,
    CUT_CAMERA_STOP_e = 11,
    CUT_CAMERA_START_e = 12,
    CUT_CO_CYL_R_e = 13,
    CUT_LOOK_PO_e = 14,
    CUT_MES_SET_PO_e = 15,
};

static u8 l_ug_no[] = {0x11, 0x12};
static char* cut_name_tbl[] = {
        "MES_SET",
        "MES_SET_TP",
        "MES_SET2",
        "FLAG_SET",
        "GET_ITEM",
        "TURN_TO_PLAYER",
        "UB1_TALK",
        "UB1_TALK_XY",
        "UB2_TALK",
        "UB_SET_ANM",
        "AREA_MAX",
        "CAMERA_STOP",
        "CAMERA_START",
        "CO_CYL_R",
        "LOOK_PO",
        "MES_SET_PO",
    };
static PeopleAnmSetProc l_anm_set_sub[] = {{&daNpcPeople_c::setAnmFromMsgTagUo}, {&daNpcPeople_c::setAnmFromMsgTagUo}, {&daNpcPeople_c::setAnmFromMsgTagUo}, {&daNpcPeople_c::setAnmFromMsgTagUb}, {&daNpcPeople_c::setAnmFromMsgTagUb}, {&daNpcPeople_c::setAnmFromMsgTagUb}, {&daNpcPeople_c::setAnmFromMsgTagUb}, {&daNpcPeople_c::setAnmFromMsgTagUw}, {&daNpcPeople_c::setAnmFromMsgTagUw}, {&daNpcPeople_c::setAnmFromMsgTagUm}, {&daNpcPeople_c::setAnmFromMsgTagUm}, {&daNpcPeople_c::setAnmFromMsgTagUm}, {&daNpcPeople_c::setAnmFromMsgTagSa}, {&daNpcPeople_c::setAnmFromMsgTagSa}, {&daNpcPeople_c::setAnmFromMsgTagSa}, {&daNpcPeople_c::setAnmFromMsgTagSa}, {&daNpcPeople_c::setAnmFromMsgTagSa}, {&daNpcPeople_c::setAnmFromMsgTagUg}, {&daNpcPeople_c::setAnmFromMsgTagUg}};

/* 022C045C */
void daNpcPeople_c::checkOrder() {
    u16 cmd = eventInfo.getCommand(); /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) { /* checkCommandDemoAccrpt() */
        if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 3) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 7) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 8) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 9) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 0xB) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[1]) && mOrderEventNum == 0xC) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[0]) && mOrderEventNum == 0xD) {
            mOrderEventNum = 0;
        } else if (dComIfGp_evmng_startCheck(m766[1]) && mOrderEventNum == 0xF) {
            mOrderEventNum = 0;
        }
    } else if (cmd == dEvtCmd_INTALK_e) { /* checkCommandTalk() */
        if (mOrderEventNum == 2 || mOrderEventNum == 1) {
            if (mTalk == 0) {
                /* HD: the l_npc_dat index assertion of the inlined accessor */
                bool ok = true;
                if (mNpcNo > NPC_UG2) {
                    JUT_ASSERT_fail("d_a_npc_people.cpp", 0x19C6, "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max");
                    ok = mNpcNo <= NPC_UG2;
                }
                if (ok) {
                    m79E = 1;
                    m79D = mpNpcDat->field_0x5B;
                    m778 = m776;
                }
            }
            mTalk = 1;
        }
    } else if (mNpcNo == NPC_UM1) {
        if (!mbIsNight) {
            eventInfo.setEventId(-1); /* eventInfo.setEventId(-1) */
            m79C = 1;
        } else {
            eventInfo.setEventId(m766[1]);
            m79C = 0;
            m79D = 0;
            m79E = 0;
        }
    }
}

/* 022C077C */
void daNpcPeople_c::initUgSearchArea() {
    for (int i = 0; i < 2; i++) {
        u8 temp = l_ug_no[i];
        daNpcPeople_c* pActor = (daNpcPeople_c*)peopleSearchFromName(peopleEventData().l_npc_staff_id[temp], 0, 0);
        if (pActor) {
            pActor->executeSetMode(0);
            /* GHS order: mAnmFlag is read before the stores */
            u8 anmFlag = pActor->mAnmFlag;
            pActor->setTalk(0);
            pActor->setNoTalk(1);
            pActor->mAnmFlag = anmFlag | 2;
            pActor->setOrderEventNum(0);
            pActor->m79D = 1;
            pActor->m79E = 1;
            /* lfs/stfs without arithmetic: the recompiled code keeps a signalling NaN's bits,
             * and an overlapping store (a degenerate pActor) defeats the harness's NaN tolerance */
            peopleEventWord(pActor->m748) = (u32)peopleEventWord(mpNpcDat->field_0x28);
            pActor->m776 = mpNpcDat->field_0x34;
            pActor->m778 = mpNpcDat->field_0x36;
        }
    }
}

/* 022C0840 */
BOOL daNpcPeople_c::chkEndEvent() {
    switch (mNpcNo) {
    case NPC_UO1:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_UB1:
    case NPC_UB2:
        if (dComIfGp_evmng_endCheck(m766[0]) || dComIfGp_evmng_endCheck(m766[1]) || dComIfGp_evmng_endCheck(m766[3])) {
            m77E = m77E | 0x8000;
            m748 = mpNpcDat->field_0x28;
            m776 = mpNpcDat->field_0x34;
            m778 = mpNpcDat->field_0x36;
            return true;
        }
        break;
    case NPC_UB4:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_UW2:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_UM1:
        if (dComIfGp_evmng_endCheck(m766[1])) {
            if (m77E & 0x8) {
                m77E = m77E & ~0x8;
                mOrderEventNum = 9;
            } else {
                m77E = m77E | 0x8000;
            }
            return true;
        }
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_UM3:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            peopleCameraScopeViewMsgModeOff(peopleCameraBody()); /* peopleCameraBody()->ScopeViewMsgModeOff() */
            m77E = m77E | 0x8000;
            return true;
        }
        if (dComIfGp_evmng_endCheck(m766[1])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_SA3:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            return true;
        }
        break;
    case NPC_SA5:
        if (dComIfGp_evmng_endCheck(m766[0]) || dComIfGp_evmng_endCheck(m766[1])) {
            if (mEtcFlag & 0x100000) {
                mEtcFlag = mEtcFlag & ~0x100000;
                peopleEventGame()->play.mStartItemTimer = 1; /* dComIfGp_startItemTimer() */
            }
            m77E = m77E | 0x8000;
            m79C = 1;
            return true;
        }
        break;
    case NPC_UG1:
    case NPC_UG2:
        if (dComIfGp_evmng_endCheck(m766[0])) {
            m77E = m77E | 0x8000;
            initUgSearchArea();
            return true;
        }
        break;
    }
    return false;
}

/* 022C0BF0 */
void daNpcPeople_c::setMessage(u32 msgNo) {
    mCurrMsgNo = msgNo;
}

/* 022C0BF8 */
void daNpcPeople_c::eventMesSetInit(int staffIdx) {
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "MsgNo" /* "MsgNo" */);
    if (pMsgNo) {
        m734 = nullptr;
        u32 no = *pMsgNo;
        if (no == 0x1) {
            // HD: getMsg is virtual, at slot 0x1C.
            u32 msg = peopleEventGetMsg(this);
            setMessage(msg);
        } else if (no == 0x358C) {
            if (mCurrMsgNo != 0x358D) {
                mEtcFlag = mEtcFlag | 0x800;
            }
        } else if (no == 0x3594) {
            mEtcFlag = mEtcFlag | 0x800;
            dComIfGs_onTmpBit(0x0208);
            // HD: reorder the checks and use the Swift Sail.
            if (!dComIfGs_isEventBit(0x0B80)) {
                setMessage(0x3595);
            } else if (!dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
                setMessage(0x3597);
            } else if (!dComIfGs_checkGetItem(dItemNo_SWIFT_SAIL_e)) {
                setMessage(0x3594);
            } else if (!dComIfGs_isEventBit(0x1C08)) {
                setMessage(0x3598);
            } else if (!dComIfGs_checkGetItem(dItemNo_MAGIC_ARMOR_e)) {
                setMessage(0x3596);
            } else {
                mEtcFlag = mEtcFlag & ~0x800;
                dComIfGs_onEventBit(0x2B04);
                setMessage(0x3599);
            }
        } else if (no == 0x359A) {
            mEtcFlag = mEtcFlag | 0x800;
            setMessage(*pMsgNo);
        } else {
            setMessage(no);
        }
    } else {
        be<u32>* p = m734.get() + 1;
        m734 = p;
        setMessage(*p);
    }
}

/* 022C0EC0 */
void daNpcPeople_c::eventMesSetTpInit(int param_1) {
    dComIfGp_event_setTalkPartner(this);
    eventMesSetInit(param_1);
}

/* 022C0F20 */
void daNpcPeople_c::eventFlagSetInit(int staffIdx) {
    be<s32>* pTurn = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Turn" /* "Turn" */);
    be<s32>* pLook = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Look" /* "Look" */);
    // HD: check both optional event arguments for null.
    if (pTurn && *pTurn != 0) {
        m79D = *pTurn;
    }
    if (pLook && *pLook != 0) {
        m79E = *pLook;
    }
}

/* 022C0FC8 */
void daNpcPeople_c::eventGetItemInit(int staffIdx) {
    int itemNo;
    be<s32>* pItemIdx = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "ItemNo" /* "ItemNo" */);
    s8 roomNo = current.roomNo;
    if (pItemIdx) {
        itemNo = peopleEventData().l_get_item_no[*pItemIdx];
    } else {
        itemNo = m75C;
    }
    fpc_ProcID itemPID = peopleCreateItemForPresentDemo(&current.pos, itemNo, 0, -1, roomNo, nullptr, nullptr);
    if (itemPID != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(itemPID);
    }
}

/* 022C1094 */
void daNpcPeople_c::eventTurnToPlayerInit() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> a, b;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = link->current.pos.x;
    b->y = link->current.pos.y;
    b->z = link->current.pos.z;
    peopleCalcDisXZAngY(a.get(), b.get(), nullptr, &m77A);
}

/* 022C1148 */
u32 daNpcPeople_c::setAnmFromMsgTagUb(int param_1) {
    switch (param_1) {
    case 0: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 1: return setAnmTbl(peopleEventData().l_ub_anm_wait2, 1);
    case 2: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 3: return setAnmTbl(peopleEventData().l_ub_anm_yada, 1);
    case 4: return setAnmTbl(peopleEventData().l_ub_anm_kuyasi, 1);
    }
    // HD: unmatched tags leave the this register as the return value.
    return (u32)gabi::ea(this);
}

/* 022C1168 */
void daNpcPeople_c::setMessageUb(sUbMsgDat* param_1) {
    setAnmFromMsgTagUb(param_1->field_0x04);
    m7A2 = param_1->field_0x05;
    m79D = param_1->field_0x06;
    m79E = param_1->field_0x06;
    if (param_1->field_0x06 == 1) {
        m748 = 1000.0f;
        m776 = 0x7FFF;
        m778 = 0x7FFF;
    } else {
        m748 = mpNpcDat->field_0x28;
        m776 = mpNpcDat->field_0x34;
        m778 = mpNpcDat->field_0x36;
    }
    m7A0 = param_1->field_0x06;
    setMessage(param_1->field_0x00);
}

/* 022C1228 */
void daNpcPeople_c::eventUb1TalkInit(int staffIdx) {
    if (!dComIfGs_isEventBit(0x0A40)) {
        m73C = peopleEventData().l_msg_ub1_1st_talk;
        dComIfGs_onEventBit(0x0A40);
    } else if (!dComIfGs_checkGetItem(dItemNo_PICTO_BOX_e) && !dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
        m73C = peopleEventData().l_msg_ub1_no_camera;
    } else if (!is1GetMap20()) {
        m73C = peopleEventData().l_msg_ub1_no_collect_map20;
        dComIfGs_onEventBit(0x2102);
    } else if (!is1DayGetMap20()) {
        m73C = peopleEventData().l_msg_ub1_collect_map20;
    } else {
        m73C = peopleEventData().l_msg_ub1_collect_map20_1day;
    }
    m738 = *m73C;
    setMessageUb(m738);
    m730 = this;
    dComIfGp_event_setTalkPartner(this);
    m730 = nullptr;
}

/* 022C1384 */
void daNpcPeople_c::eventUb1TalkXyInit(int staffIdx) {
    be<s32>* pMsgNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "MsgNo" /* "MsgNo" */);
    BOOL temp = false;
    if (pMsgNo) {
        switch ((u32)*pMsgNo) {
        case 0:
            m73C = peopleEventData().l_msg_xy_ub1_get_map20_2;
            temp = true;
            break;
        case 1:
            m73C = peopleEventData().l_msg_xy_ub1_get_map20_3;
            temp = true;
            break;
        }
    } else {
        // HD: dComIfGp_getPictureResult reads the play accessor twice.
        dComIfGp_get();
        if (peopleEventGame()->play.mPictureResult != 5) {
            m73C = peopleEventData().l_msg_xy_ub1_no_photo;
        } else if (is1GetMap20()) {
            m73C = peopleEventData().l_msg_xy_ub1_have_map20;
            temp = true;
        } else {
            m73C = peopleEventData().l_msg_xy_ub1_get_map20_1;
            dComIfGs_setEventReg(0xC103, 1);
        }
    }
    m738 = *m73C;
    setMessageUb(m738);
    if (temp) {
        m730 = (daNpcPeople_c*)peopleSearchFromName("Ub2" /* "Ub2" */, 0, 0);
    } else {
        m730 = this;
    }
    dComIfGp_event_setTalkPartner(m730);
    m730 = nullptr;
}

/* 022C167C */
void daNpcPeople_c::eventAreaMaxInit() {
    m748 = 1000.0f;
    m776 = 0x7FFF;
    m778 = 0x7FFF;
    m740 = 0.0f;
}

/* 022C16A4 */
void daNpcPeople_c::eventCameraStopInit() {
    PeopleEventCamera* camera = peopleCameraBody();
    gabi::Local<cXyz> pos;
    pos->x = m71C.x;
    pos->y = m71C.y;
    pos->z = m71C.z;
    gabi::Local<cXyz> tmp;
    switch (m7A7) {
    case 0:
        dKy_get_moon_pos(tmp.get());
        pos->copy(*tmp);
        break;
    case 1:
        dKy_get_orion_pos(tmp.get());
        pos->copy(*tmp);
        break;
    case 2:
        dKy_get_hokuto_pos(tmp.get());
        pos->copy(*tmp);
        break;
    }
    peopleCameraSubjectLockOn(camera, 0);         /* SubjectLockOn(NULL) */
    peopleCameraSetExtendedPosition(camera, pos.get()); /* SetExtendedPosition(&pos) */
    camera->mFlags = camera->mFlags | 0x01800000; /* StickUseless(), CStickUseless() */
}

/* 022C17C0 */
void daNpcPeople_c::eventCameraStartInit() {
    PeopleEventCamera* camera = peopleCameraBody();
    peopleCameraSubjectLockOff(camera); /* SubjectLockOff() */
    camera->mFlags = camera->mFlags & ~0x01800000; /* StickUse(), CStickUse() */
}

/* 022C17FC */
void daNpcPeople_c::eventCoCylRInit(int staffIdx) {
    be<s32>* pRad = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "CoCylR" /* "CoCylR" */);
    if (pRad) {
        m74C = (f32)(s32)*pRad;
    } else {
        m74C = mpNpcDat->field_0x3C;
    }
}

/* 022C1894 */
void daNpcPeople_c::resetPig() {
    for (int i = 0; i < 3; i++) {
        PeopleEventPig* pig = static_cast<PeopleEventPig*>(peopleSearchFromName("Pig" /* "Pig" */, 0xF00, peoplePigParams()[i]));
        if (pig != nullptr) {
            pig->current.pos.copy(pig->home.pos);
            pig->m408 = 0; /* kb_class::m408 */
        }
    }
    dComIfGs_setTmpReg(0xFD07, 0);
}

/* 022C1B24 */
void daNpcPeople_c::setAnmFromMsgTag() {
    int temp = dComIfGp_getMesgAnimeAttrInfo();
    if (temp != 0x32) {
        /* (this->*l_anm_set_sub[mNpcNo])(temp) */
        (this->*l_anm_set_sub[mNpcNo].fn)(temp);
    }
    dComIfGp_setMesgAnimeAttrInfo(0x32); /* dComIfGp_setMesgAnimeAttrInfo(0x32) */
}

/* 022C1EEC */
bool daNpcPeople_c::eventMesSet() {
    return talk2(0, this) == fopMsgStts_BOX_CLOSED_e;
}

/* 022C2170 */
bool daNpcPeople_c::eventMesSet2() {
    // HD: keep the PAL scope check.
    if (peopleEventGame()->play.mScopeType == 0) { /* dComIfGp_getScopeType() */
        return true;
    }
    return talk3(1) == fopMsgStts_BOX_CLOSED_e;
}

/* 022C21D8 */
bool daNpcPeople_c::eventTurnToPlayer() {
    return current.angle.y == m77A ? TRUE : FALSE;
}

static inline bool ub1_talk_step(daNpcPeople_c* i_this, const char* ub2_name) {
    u16 status = i_this->talk2(0, dComIfGp_event_getTalkPartner());
    if (status == fopMsgStts_BOX_CLOSED_e) {
        i_this->m73C = i_this->m73C.get() + 1;
        i_this->m738 = *i_this->m73C;
        if (i_this->m738.get() == nullptr) {
            return true;
        }
        i_this->setMessageUb(i_this->m738);
        if (dComIfGp_event_getTalkPartner() == i_this) {
            i_this->m730 = (daNpcPeople_c*)peopleSearchFromName(ub2_name, 0, 0);
        } else {
            i_this->m730 = i_this;
        }
    }
    return false;
}

/* 022C21F0 */
bool daNpcPeople_c::eventUb1Talk() {
    return ub1_talk_step(this, "Ub2");
}

/* 022C22B8 */
bool daNpcPeople_c::eventUb1TalkXy() {
    return ub1_talk_step(this, "Ub2");
}

/* 022C2380 */
bool daNpcPeople_c::eventUb2Talk() {
    daNpcPeople_c* pActor = (daNpcPeople_c*)peopleSearchFromName(peopleEventData().l_npc_staff_id[3], 0, 0);
    if (pActor) {
        m79D = pActor->m7A0;
        m79E = pActor->m7A0;
        if (pActor->m7A0 == 1) {
            m748 = 1000.0f;
            m776 = 0x7FFF;
            m778 = 0x7FFF;
        } else {
            m748 = mpNpcDat->field_0x28;
            m776 = mpNpcDat->field_0x34;
            m778 = mpNpcDat->field_0x36;
        }
        if (pActor->m7A2 != 0xFF) {
            setAnmFromMsgTagUb(pActor->m7A2);
            pActor->m7A2 = 0xFF;
        }
    }
    mEtcFlag = mEtcFlag | 0x200000;
    return 0;
}

/* 022C2458 */
bool daNpcPeople_c::eventUbSetAnm() {
    switch (mNpcNo) {
    case NPC_UB1:
        if (mAnmFlag & 3) {
            mAnmFlag = mAnmFlag & ~0x3;
            setAnmTbl(peopleEventData().l_npc_anm_ub1_tbl[getRand(8)], 1);
        }
        break;
    case NPC_UB2:
        if (mAnmFlag & 3) {
            mAnmFlag = mAnmFlag & ~0x3;
            setAnmTbl(peopleEventData().l_npc_anm_ub2_tbl[getRand(8)], 1);
        }
        break;
    }
    return 0;
}

/* 022C2518 */
bool daNpcPeople_c::eventLookPo() {
    fopAc_ac_c* pActor = peopleSearchFromName("Po" /* "Po" */, 0, 0);
    if (pActor) {
        mLookAtPos.copy(pActor->eyePos);
        m799 = 1;
        m764 = false;
    }
    return true;
}

/* 022C2588 */
bool daNpcPeople_c::eventMesSetPo() {
    eventLookPo();
    if (eventMesSet()) {
        setWaitAnm();
        return true;
    }
    return false;
}

/* 022C25E8 */
void daNpcPeople_c::privateCut() {
    if (mNpcNo > NPC_UG2) {
        JUT_ASSERT_fail("d_a_npc_people.cpp", 0x1A84, "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max"); /* HD: l_npc_staff_id index */
    }
    int staffIdx = dComIfGp_evmng_getMyStaffId(peopleEventData().l_npc_staff_id[mNpcNo], nullptr, 0);
    if (staffIdx != -1) {
        s8 idx = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl, 16, TRUE, 0);
        m797 = idx;
        if (idx == -1) {
            dComIfGp_evmng_cutEnd(staffIdx);
        } else {
            if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                switch (m797) {
                case CUT_MES_SET_e: eventMesSetInit(staffIdx); break;
                case CUT_MES_SET_TP_e: eventMesSetTpInit(staffIdx); break;
                case CUT_FLAG_SET_e: eventFlagSetInit(staffIdx); break;
                case CUT_GET_ITEM_e: eventGetItemInit(staffIdx); break;
                case CUT_TURN_TO_PLAYER_e: eventTurnToPlayerInit(); break;
                case CUT_UB1_TALK_e: eventUb1TalkInit(staffIdx); break;
                case CUT_UB1_TALK_XY_e: eventUb1TalkXyInit(staffIdx); break;
                case CUT_AREA_MAX_e: eventAreaMaxInit(); break;
                case CUT_CAMERA_STOP_e: eventCameraStopInit(); break;
                case CUT_CAMERA_START_e: eventCameraStartInit(); break;
                case CUT_CO_CYL_R_e: eventCoCylRInit(staffIdx); break;
                case CUT_MES_SET_PO_e: eventMesSetPoInit(staffIdx); break;
                }
            }
            bool end;
            switch (m797) {
            case CUT_MES_SET_e: end = eventMesSet(); break;
            case CUT_MES_SET_TP_e: end = eventMesSet(); break;
            case CUT_MES_SET2_e: end = eventMesSet2(); break;
            case CUT_TURN_TO_PLAYER_e: end = eventTurnToPlayer(); break;
            case CUT_UB1_TALK_e: end = eventUb1Talk(); break;
            case CUT_UB1_TALK_XY_e: end = eventUb1TalkXy(); break;
            case CUT_UB2_TALK_e: end = eventUb2Talk(); break;
            case CUT_UB_SET_ANM_e: end = eventUbSetAnm(); break;
            case CUT_LOOK_PO_e: end = eventLookPo(); break;
            case CUT_MES_SET_PO_e: end = eventMesSetPo(); break;
            case CUT_GET_ITEM_e:   /* eventGetItem(): inlined, true */
            case CUT_CAMERA_STOP_e: /* eventCameraStop(): inlined, true */
            case CUT_FLAG_SET_e:
            default: end = true; break;
            }
            if (end) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        }
    }
}

/* 022C2958 */
void daNpcPeople_c::eventMove() {
    if (chkEndEvent()) {
        setWaitAnm();
    } else {
        u8 oldFlag = mEventCut.mbAttention;
        if (mEventCut.cutProc()) {
            if (!mEventCut.mbAttention) {
                mEventCut.mbAttention = oldFlag;
            }
        } else {
            mEtcFlag = mEtcFlag & ~0x00200000;
            privateCut();
        }
    }
}

/* 022C2A04 */
void daNpcPeople_c::eventOrder() {
    if (m77E & 0x8000) {
        m77E = m77E & ~0x8000;
        m79D = mpNpcDat->field_0x5A;
        m79E = mpNpcDat->field_0x5C;
        m79F = mpNpcDat->field_0x5C;
        m778 = mpNpcDat->field_0x36;
        mTalk = 0;
        mAnmFlag = mAnmFlag | 2;
        dComIfGp_event_reset();
        if (mNpcNo == NPC_UW2) {
            m77A = home.angle.y;
        }
    }
    if (mOrderEventNum == 2 || mOrderEventNum == 1) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mOrderEventNum == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (mOrderEventNum == 3) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 0, 0xFFFF);
    } else if (mOrderEventNum == 7) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 0, 0xFFFF);
    } else if (mOrderEventNum == 8) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 0, 0xFFFF);
    } else if (mOrderEventNum == 9) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 0, 0xFFFF);
    } else if (mOrderEventNum == 0xB) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 8 /* dEvtFlag_UNK8_e */, 0xFFFF);
    } else if (mOrderEventNum == 0xC) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[1], 0, 0xFFFF);
    } else if (mOrderEventNum == 0xD) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[0], 0, 0xFFFF);
    } else if (mOrderEventNum == 0xF) {
        fopAcM_orderChangeEventId(dComIfGp_getPlayer(0), this, m766[1], 0, 0xFFFF);
    }
}

/* 022C8550 */
u32 daNpcPeople_c::setAnmFromMsgTagUo(int param_1) {
    switch (param_1) {
    case 0: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 1: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 2: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 3: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 4: return setAnmTbl(peopleEventData().l_npc_anm_bikkuri, 1);
    case 5: return setAnmTbl(peopleEventData().l_npc_anm_furue, 1);
    case 6: return setAnmTbl(peopleEventData().l_npc_anm_miburui, 1);
    case 7: return setAnmTbl(peopleEventData().l_npc_anm_kyoro2, 1);
    }
    // HD: unmatched tags leave the this register as the return value.
    return (u32)gabi::ea(this);
}

/* 022C8570 */
u32 daNpcPeople_c::setAnmFromMsgTagUw(int param_1) {
    m77A = home.angle.y;
    m79E = 1;
    switch (param_1) {
    case 0: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 1: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 2: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 3: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 4: return setAnmTbl(peopleEventData().l_npc_anm_talk3, 1);
    case 5: return setAnmTbl(peopleEventData().l_npc_anm_talk4, 1);
    case 6: return setAnmTbl(peopleEventData().l_npc_anm_talkH, 1);
    case 7:
        m77A = current.angle.y;
        m79E = 0;
        return setAnmTbl(peopleEventData().l_npc_anm_Mojimoji, 1);
    case 8: return setAnmTbl(peopleEventData().l_npc_anm_happy, 1);
    }
    // HD: unmatched tags leave the this register as the return value.
    return (u32)gabi::ea(this);
}

/* 022C85DC */
u32 daNpcPeople_c::setAnmFromMsgTagUm(int param_1) {
    m79E = 1;
    switch (param_1) {
    case 0:
        return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
        break;
    case 1:
    case 7:
        return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
        break;
    case 2:
        return setAnmTbl(peopleEventData().l_npc_anm_shobon_um, 1);
        break;
    case 3:
        return setAnmTbl(peopleEventData().l_npc_anm_happy_um, 1);
        break;
    case 4:
        m79E = 0;
        return setAnmTbl(peopleEventData().l_npc_anm_um3_wait3, 1);
        break;
    case 5:
        return setAnmTbl(peopleEventData().l_npc_anm_um3_talk2, 1);
        break;
    case 6:
        return setAnmTbl(peopleEventData().l_npc_anm_um3_talk3, 1);
        break;
    case 8:
        return setAnmTbl(peopleEventData().l_npc_anm_happy2_um, 1);
        break;
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022C86D0 */
u32 daNpcPeople_c::setAnmFromMsgTagSa(int param_1) {
    switch (param_1) {
    case 0: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 1: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 2: return setAnmTbl(peopleEventData().l_npc_anm_talk2_sa, 1);
    case 3: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 4: return setAnmTbl(peopleEventData().l_npc_anm_talk3_sa, 1);
    case 5: return setAnmTbl(peopleEventData().l_npc_anm_kiai_sa, 1);
    case 6: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 7: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 8: return setAnmTbl(peopleEventData().l_npc_anm_talk_sa, 1);
    case 9: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    }
    // HD: unmatched tags leave the this register as the return value.
    return (u32)gabi::ea(this);
}

/* 022C86F0 */
u32 daNpcPeople_c::setAnmFromMsgTagUg(int param_1) {
    switch (param_1) {
    case 0: return setAnmTbl(peopleEventData().l_npc_anm_wait, 1);
    case 1: return setAnmTbl(peopleEventData().l_npc_anm_talk, 1);
    case 2: return setAnmTbl(peopleEventData().l_npc_anm_talk2_ug, 1);
    }
    // HD: unmatched tags leave the this register as the return value.
    return (u32)gabi::ea(this);
}

/* WWHD glue
data l_ug_no 101C4C68
data cut_name_tbl 101C4C6C
data l_anm_set_sub 101C40DC nocheck
ext 025D9F38 fopAc_ac_c* peopleSearchFromName(const char*, u32, u32)
ext 025D7DEC u32 peopleCreateItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*)
ext 0259D624 void peopleCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*)
ext 024F8044 PeopleEventCamera* peopleCameraBody()
ext 025154B0 void peopleCameraScopeViewMsgModeOff(PeopleEventCamera*)
ext 02515434 void peopleCameraSubjectLockOn(PeopleEventCamera*, u32)
ext 025154C4 void peopleCameraSetExtendedPosition(PeopleEventCamera*, cXyz*)
ext 0251544C void peopleCameraSubjectLockOff(PeopleEventCamera*)
ext 02560BCC void dKy_get_moon_pos(cXyz*)
ext 0256099C void dKy_get_orion_pos(cXyz*)
ext 02560AB4 void dKy_get_hokuto_pos(cXyz*)
str 10020608 "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max" in 022C045C
str 1002063C "d_a_npc_people.cpp" in 022C045C
str 10020650 "MsgNo" in 022C0BF8
str 10020658 "Turn"
str 10020660 "Look"
str 10020668 "ItemNo"
str 10020688 "Ub2" in 022C1384
str 1002068C "MsgNo" in 022C1384
str 10020694 "CoCylR"
str 1002069C "Pig"
str 100206AC "Po"
str 100206B0 "mNpcNo >= 0 && mNpcNo < daNpcPeople_c::Npc_No_Max" in 022C25E8
str 100206E4 "d_a_npc_people.cpp" in 022C25E8
str 100206A4 "Ub2" in 022C21F0
str 100206A8 "Ub2" in 022C22B8
*/
