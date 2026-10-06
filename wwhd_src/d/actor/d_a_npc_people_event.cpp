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
#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem_e(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* dEvt_control_c (play + 0x51D0): mPtTalk at +0xCC, mPtItem at +0xD0; 0253F124 getPId(actor),
 * 0253EE04 convPId(id) */
static inline void dComIfGp_event_setTalkPartner(void* actor) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    u32 id = gabi::call<u32>(0x0253F124, evt, actor);
    gabi::store<u32>(evt + 0xCC, id);
}
static inline fopAc_ac_c* dComIfGp_event_getTalkPartner() {
    u32 play = dComIfGp_ea();
    return gabi::call<fopAc_ac_c*>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x529C));
}
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName_e(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* angle) {
    gabi::call(0x0259D624, a, b, dist, angle);
}
/* camera: 024F8044 dCam_getBody(); dCamera_c::SubjectLockOn 02515434, SubjectLockOff 0251544C,
 * SetExtendedPosition 025154C4, ScopeViewMsgModeOff 025154B0; Stick/CStickUse(less) are inline
 * (flag word +0x510, bits 0x01800000) */
static inline u32 dCam_getBody() { return gabi::call<u32>(0x024F8044); }
/* dKy_get_moon_pos / orion / hokuto: cXyz through a hidden result pointer */
static inline void dKy_get_moon_pos(cXyz* out) { gabi::call(0x02560BCC, out); }
static inline void dKy_get_orion_pos(cXyz* out) { gabi::call(0x0256099C, out); }
static inline void dKy_get_hokuto_pos(cXyz* out) { gabi::call(0x02560AB4, out); }

/* ---- statics ---- */
enum : u32 {
    l_npc_staff_id = 0x101C3E7C, /* const char*[19] */
    l_get_item_no = 0x101C3998,  /* int[] */
    l_ug_no = 0x101C4C68,        /* u8[2] (initUgSearchArea's static) */
    cut_name_tbl = 0x101C4C6C,   /* const char*[16] (privateCut's static) */
    l_anm_set_sub = 0x101C40DC,  /* pointers to member functions [19] */
    l_pig_para = 0x1002014C,     /* int[3] */
    l_npc_anm_ub1_tbl = 0x101C3460,
    l_npc_anm_ub2_tbl = 0x101C3480,
};
static inline const char* staff_id(u32 i) { return gabi::at<const char>(gabi::load<u32>(l_npc_staff_id + i * 4)); }
static inline sPeopleAnmDat* anm(u32 ea) { return gabi::at<sPeopleAnmDat>(ea); }
/* m73C: sUbMsgDat** (cursor in a null-terminated list), m738 = *m73C */
static inline void set_ub_list(daNpcPeople_c* i_this, u32 list) {
    i_this->m73C = gabi::at<gptr<sUbMsgDat>>(list);
}
static inline sUbMsgDat* ub_cur(daNpcPeople_c* i_this) {
    return gabi::at<sUbMsgDat>(gabi::load<u32>(gabi::ea(i_this->m73C.get())));
}

/* 022C045C */
void daNpcPeople_c::checkOrder() {
    WWHD_FUNC(0x022C045C, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2) { /* checkCommandDemoAccrpt() */
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
    } else if (cmd == 1) { /* checkCommandTalk() */
        if (mOrderEventNum == 2 || mOrderEventNum == 1) {
            if (mTalk == 0) {
                /* HD: the l_npc_dat index assertion of the inlined accessor */
                bool ok = true;
                if (mNpcNo >= 0x13) {
                    JUT_ASSERT_fail(STR(0x1002063C), 0x19C6, STR(0x10020608));
                    ok = mNpcNo < 0x13;
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
            gabi::store<s16>(gabi::ea(this) + 0xFC, -1); /* eventInfo.setEventId(-1) */
            m79C = 1;
        } else {
            gabi::store<s16>(gabi::ea(this) + 0xFC, m766[1]);
            m79C = 0;
            m79D = 0;
            m79E = 0;
        }
    }
}
VERIFY(0x022C045C, &daNpcPeople_c::checkOrder);

/* 022C077C */
void daNpcPeople_c::initUgSearchArea() {
    WWHD_FUNC(0x022C077C, void, this);
    for (int i = 0; i < 2; i++) {
        u8 temp = gabi::load<u8>(l_ug_no + i);
        daNpcPeople_c* pActor = (daNpcPeople_c*)fopAcM_searchFromName_e(staff_id(temp), 0, 0);
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
            gabi::store<u32>(gabi::ea(&pActor->m748), gabi::load<u32>(gabi::ea(&mpNpcDat->field_0x28)));
            pActor->m776 = mpNpcDat->field_0x34;
            pActor->m778 = mpNpcDat->field_0x36;
        }
    }
}
VERIFY(0x022C077C, &daNpcPeople_c::initUgSearchArea);

/* 022C0840 */
BOOL daNpcPeople_c::chkEndEvent() {
    WWHD_FUNC(0x022C0840, BOOL, this);
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
            gabi::call(0x025154B0, dCam_getBody()); /* dCam_getBody()->ScopeViewMsgModeOff() */
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
                gabi::store<u8>(dComIfGp_ea() + 0x5BDF, 1); /* dComIfGp_startItemTimer() */
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
VERIFY(0x022C0840, &daNpcPeople_c::chkEndEvent);

/* 022C0BF0 */
void daNpcPeople_c::setMessage(u32 msgNo) {
    WWHD_FUNC(0x022C0BF0, void, this, msgNo);
    mCurrMsgNo = msgNo;
}
VERIFY(0x022C0BF0, &daNpcPeople_c::setMessage);

/* 022C0BF8 */
void daNpcPeople_c::eventMesSetInit(int staffIdx) {
    WWHD_FUNC(0x022C0BF8, void, this, staffIdx);
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10020650) /* "MsgNo" */);
    if (pMsgNo) {
        m734 = nullptr;
        u32 no = *pMsgNo;
        if (no == 0x1) {
            /* HD: getMsg is virtual (vtable slot +0x1C) */
            u32 msg = gabi::call_ptr<u32>(gabi::load<u32>(__vtbl + 0x1C), this);
            setMessage(msg);
        } else if (no == 0x358C) {
            if (mCurrMsgNo != 0x358D) {
                mEtcFlag = mEtcFlag | 0x800;
            }
        } else if (no == 0x3594) {
            mEtcFlag = mEtcFlag | 0x800;
            dComIfGs_onTmpBit(0x0208);
            /* HD: the checks are in a different order */
            if (!dComIfGs_isEventBit(0x0B80)) {
                setMessage(0x3595);
            } else if (!dComIfGs_checkGetItem_e(0x26 /* DELUXE_PICTO_BOX */)) {
                setMessage(0x3597);
            } else if (!dComIfGs_checkGetItem_e(0x77 /* SAIL */)) {
                setMessage(0x3594);
            } else if (!dComIfGs_isEventBit(0x1C08)) {
                setMessage(0x3598);
            } else if (!dComIfGs_checkGetItem_e(0x2A /* MAGIC_ARMOR */)) {
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
        be<u32>* p = gabi::at<be<u32>>(gabi::ea(m734.get()) + 4);
        m734 = p;
        setMessage(*p);
    }
}
VERIFY(0x022C0BF8, &daNpcPeople_c::eventMesSetInit);

/* 022C0EC0 */
void daNpcPeople_c::eventMesSetTpInit(int param_1) {
    WWHD_FUNC(0x022C0EC0, void, this, param_1);
    dComIfGp_event_setTalkPartner(this);
    eventMesSetInit(param_1);
}
VERIFY(0x022C0EC0, &daNpcPeople_c::eventMesSetTpInit);

/* 022C0F20 */
void daNpcPeople_c::eventFlagSetInit(int staffIdx) {
    WWHD_FUNC(0x022C0F20, void, this, staffIdx);
    be<s32>* pTurn = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10020658) /* "Turn" */);
    be<s32>* pLook = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10020660) /* "Look" */);
    /* HD: null checks */
    if (pTurn && *pTurn != 0) {
        m79D = *pTurn;
    }
    if (pLook && *pLook != 0) {
        m79E = *pLook;
    }
}
VERIFY(0x022C0F20, &daNpcPeople_c::eventFlagSetInit);

/* 022C0FC8 */
void daNpcPeople_c::eventGetItemInit(int staffIdx) {
    WWHD_FUNC(0x022C0FC8, void, this, staffIdx);
    int itemNo;
    be<s32>* pItemIdx = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10020668) /* "ItemNo" */);
    s8 roomNo = current.roomNo;
    if (pItemIdx) {
        itemNo = gabi::load<s32>(l_get_item_no + *pItemIdx * 4);
    } else {
        itemNo = m75C;
    }
    fpc_ProcID itemPID = fopAcM_createItemForPresentDemo(&current.pos, itemNo, 0, -1, roomNo, nullptr, nullptr);
    if (itemPID != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(itemPID);
    }
}
VERIFY(0x022C0FC8, &daNpcPeople_c::eventGetItemInit);

/* 022C1094 */
void daNpcPeople_c::eventTurnToPlayerInit() {
    WWHD_FUNC(0x022C1094, void, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> a, b;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = link->current.pos.x;
    b->y = link->current.pos.y;
    b->z = link->current.pos.z;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, &m77A);
}
VERIFY(0x022C1094, &daNpcPeople_c::eventTurnToPlayerInit);

/* 022C1148 */
u32 daNpcPeople_c::setAnmFromMsgTagUb(int param_1) {
    WWHD_FUNC(0x022C1148, u32, this, param_1);
    /* switch -> table: wait, ub_wait2, talk, ub_yada, ub_kuyasi */
    if ((u32)param_1 <= 4) {
        return setAnmTbl(anm(gabi::load<u32>(0x10020670 + param_1 * 4)), 1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C1148, &daNpcPeople_c::setAnmFromMsgTagUb);

/* 022C1168 */
void daNpcPeople_c::setMessageUb(sUbMsgDat* param_1) {
    WWHD_FUNC(0x022C1168, void, this, param_1);
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
VERIFY(0x022C1168, &daNpcPeople_c::setMessageUb);

/* 022C1228 */
void daNpcPeople_c::eventUb1TalkInit(int staffIdx) {
    WWHD_FUNC(0x022C1228, void, this, staffIdx);
    if (!dComIfGs_isEventBit(0x0A40)) {
        set_ub_list(this, 0x101C3A38); /* l_msg_ub1_1st_talk */
        dComIfGs_onEventBit(0x0A40);
    } else if (!dComIfGs_checkGetItem_e(0x23 /* PICTO_BOX */) && !dComIfGs_checkGetItem_e(0x26 /* DELUXE_PICTO_BOX */)) {
        set_ub_list(this, 0x101C3A4C); /* l_msg_ub1_no_camera */
    } else if (!is1GetMap20()) {
        set_ub_list(this, 0x101C3A58); /* l_msg_ub1_no_collect_map20 */
        dComIfGs_onEventBit(0x2102);
    } else if (!is1DayGetMap20()) {
        set_ub_list(this, 0x101C2F24); /* l_msg_ub1_collect_map20 */
    } else {
        set_ub_list(this, 0x101C35F8); /* l_msg_ub1_collect_map20_1day */
    }
    m738 = ub_cur(this);
    setMessageUb(m738);
    m730 = this;
    dComIfGp_event_setTalkPartner(this);
    m730 = nullptr;
}
VERIFY(0x022C1228, &daNpcPeople_c::eventUb1TalkInit);

/* 022C1384 */
void daNpcPeople_c::eventUb1TalkXyInit(int staffIdx) {
    WWHD_FUNC(0x022C1384, void, this, staffIdx);
    be<s32>* pMsgNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1002068C) /* "MsgNo" */);
    BOOL temp = false;
    if (pMsgNo) {
        switch ((u32)*pMsgNo) {
        case 0:
            set_ub_list(this, 0x101C3678); /* l_msg_xy_ub1_get_map20_2 */
            temp = true;
            break;
        case 1:
            set_ub_list(this, 0x101C2F44); /* l_msg_xy_ub1_get_map20_3 */
            temp = true;
            break;
        }
    } else {
        dComIfGp_get(); /* HD: dComIfGp_getPictureResult() calls the accessor twice */
        if (gabi::load<u8>(dComIfGp_ea() + 0x5BE6) != 5) {
            set_ub_list(this, 0x101C2F2C); /* l_msg_xy_ub1_no_photo */
        } else if (is1GetMap20()) {
            set_ub_list(this, 0x101C2F34); /* l_msg_xy_ub1_have_map20 */
            temp = true;
        } else {
            set_ub_list(this, 0x101C2F3C); /* l_msg_xy_ub1_get_map20_1 */
            dComIfGs_setEventReg(0xC103, 1);
        }
    }
    m738 = ub_cur(this);
    setMessageUb(m738);
    if (temp) {
        m730 = (daNpcPeople_c*)fopAcM_searchFromName_e(STR(0x10020688) /* "Ub2" */, 0, 0);
    } else {
        m730 = this;
    }
    dComIfGp_event_setTalkPartner(m730);
    m730 = nullptr;
}
VERIFY(0x022C1384, &daNpcPeople_c::eventUb1TalkXyInit);

/* 022C167C */
void daNpcPeople_c::eventAreaMaxInit() {
    WWHD_FUNC(0x022C167C, void, this);
    m748 = 1000.0f;
    m776 = 0x7FFF;
    m778 = 0x7FFF;
    m740 = 0.0f;
}
VERIFY(0x022C167C, &daNpcPeople_c::eventAreaMaxInit);

/* 022C16A4 */
void daNpcPeople_c::eventCameraStopInit() {
    WWHD_FUNC(0x022C16A4, void, this);
    u32 camera = dCam_getBody();
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
    gabi::call(0x02515434, camera, 0);         /* SubjectLockOn(NULL) */
    gabi::call(0x025154C4, camera, pos.get()); /* SetExtendedPosition(&pos) */
    gabi::store<u32>(camera + 0x510, gabi::load<u32>(camera + 0x510) | 0x01800000); /* StickUseless(), CStickUseless() */
}
VERIFY(0x022C16A4, &daNpcPeople_c::eventCameraStopInit);

/* 022C17C0 */
void daNpcPeople_c::eventCameraStartInit() {
    WWHD_FUNC(0x022C17C0, void, this);
    u32 camera = dCam_getBody();
    gabi::call(0x0251544C, camera); /* SubjectLockOff() */
    gabi::store<u32>(camera + 0x510, gabi::load<u32>(camera + 0x510) & ~0x01800000); /* StickUse(), CStickUse() */
}
VERIFY(0x022C17C0, &daNpcPeople_c::eventCameraStartInit);

/* 022C17FC */
void daNpcPeople_c::eventCoCylRInit(int staffIdx) {
    WWHD_FUNC(0x022C17FC, void, this, staffIdx);
    be<s32>* pRad = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10020694) /* "CoCylR" */);
    if (pRad) {
        m74C = (f32)(s32)*pRad;
    } else {
        m74C = mpNpcDat->field_0x3C;
    }
}
VERIFY(0x022C17FC, &daNpcPeople_c::eventCoCylRInit);

/* 022C1894 */
void daNpcPeople_c::resetPig() {
    WWHD_FUNC(0x022C1894, void, this);
    for (int i = 0; i < 3; i++) {
        fopAc_ac_c* pig = fopAcM_searchFromName_e(STR(0x1002069C) /* "Pig" */, 0xF00, gabi::load<s32>(l_pig_para + i * 4));
        if (pig != nullptr) {
            pig->current.pos.copy(pig->home.pos);
            gabi::store<u8>(gabi::ea(pig) + 0x750, 0); /* kb_class::m408 */
        }
    }
    dComIfGs_setTmpReg(0xFD07, 0);
}
VERIFY(0x022C1894, &daNpcPeople_c::resetPig);

/* 022C1B24 */
void daNpcPeople_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x022C1B24, void, this);
    int temp = dComIfGp_getMesgAnimeAttrInfo();
    if (temp != 0x32) {
        /* (this->*l_anm_set_sub[mNpcNo])(temp) */
        people_pmf_call(this, gabi::at<PeoplePmf_l>(l_anm_set_sub + mNpcNo * 8), temp);
    }
    gabi::store<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925, 0x32); /* dComIfGp_setMesgAnimeAttrInfo(0x32) */
}
VERIFY(0x022C1B24, &daNpcPeople_c::setAnmFromMsgTag);

/* 022C1EEC */
bool daNpcPeople_c::eventMesSet() {
    WWHD_FUNC(0x022C1EEC, bool, this);
    return talk2(0, this) == 0x12 /* fopMsgStts_BOX_CLOSED_e */;
}
VERIFY(0x022C1EEC, &daNpcPeople_c::eventMesSet);

/* 022C2170 */
bool daNpcPeople_c::eventMesSet2() {
    WWHD_FUNC(0x022C2170, bool, this);
    /* the PAL version's scope check */
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BCF) == 0) { /* dComIfGp_getScopeType() */
        return true;
    }
    return talk3(1) == 0x12;
}
VERIFY(0x022C2170, &daNpcPeople_c::eventMesSet2);

/* 022C21D8 */
bool daNpcPeople_c::eventTurnToPlayer() {
    WWHD_FUNC(0x022C21D8, bool, this);
    return current.angle.y == m77A ? TRUE : FALSE;
}
VERIFY(0x022C21D8, &daNpcPeople_c::eventTurnToPlayer);

static inline bool ub1_talk_step(daNpcPeople_c* i_this, u32 ub2_name) {
    u16 status = i_this->talk2(0, dComIfGp_event_getTalkPartner());
    if (status == 0x12) {
        i_this->m73C = gabi::at<gptr<sUbMsgDat>>(gabi::ea(i_this->m73C.get()) + 4);
        i_this->m738 = ub_cur(i_this);
        if (i_this->m738.get() == nullptr) {
            return true;
        }
        i_this->setMessageUb(i_this->m738);
        if (dComIfGp_event_getTalkPartner() == i_this) {
            i_this->m730 = (daNpcPeople_c*)fopAcM_searchFromName_e(STR(ub2_name), 0, 0);
        } else {
            i_this->m730 = i_this;
        }
    }
    return false;
}

/* 022C21F0 */
bool daNpcPeople_c::eventUb1Talk() {
    WWHD_FUNC(0x022C21F0, bool, this);
    return ub1_talk_step(this, 0x100206A4 /* "Ub2" */);
}
VERIFY(0x022C21F0, &daNpcPeople_c::eventUb1Talk);

/* 022C22B8 */
bool daNpcPeople_c::eventUb1TalkXy() {
    WWHD_FUNC(0x022C22B8, bool, this);
    return ub1_talk_step(this, 0x100206A8 /* "Ub2" */);
}
VERIFY(0x022C22B8, &daNpcPeople_c::eventUb1TalkXy);

/* 022C2380 */
bool daNpcPeople_c::eventUb2Talk() {
    WWHD_FUNC(0x022C2380, bool, this);
    daNpcPeople_c* pActor = (daNpcPeople_c*)fopAcM_searchFromName_e(staff_id(3), 0, 0);
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
VERIFY(0x022C2380, &daNpcPeople_c::eventUb2Talk);

/* 022C2458 */
bool daNpcPeople_c::eventUbSetAnm() {
    WWHD_FUNC(0x022C2458, bool, this);
    switch (mNpcNo) {
    case NPC_UB1:
        if (mAnmFlag & 3) {
            mAnmFlag = mAnmFlag & ~0x3;
            setAnmTbl(anm(gabi::load<u32>(l_npc_anm_ub1_tbl + getRand(8) * 4)), 1);
        }
        break;
    case NPC_UB2:
        if (mAnmFlag & 3) {
            mAnmFlag = mAnmFlag & ~0x3;
            setAnmTbl(anm(gabi::load<u32>(l_npc_anm_ub2_tbl + getRand(8) * 4)), 1);
        }
        break;
    }
    return 0;
}
VERIFY(0x022C2458, &daNpcPeople_c::eventUbSetAnm);

/* 022C2518 */
bool daNpcPeople_c::eventLookPo() {
    WWHD_FUNC(0x022C2518, bool, this);
    fopAc_ac_c* pActor = fopAcM_searchFromName_e(STR(0x100206AC) /* "Po" */, 0, 0);
    if (pActor) {
        mLookAtPos.copy(pActor->eyePos);
        m799 = 1;
        m764 = false;
    }
    return true;
}
VERIFY(0x022C2518, &daNpcPeople_c::eventLookPo);

/* 022C2588 */
bool daNpcPeople_c::eventMesSetPo() {
    WWHD_FUNC(0x022C2588, bool, this);
    eventLookPo();
    if (eventMesSet()) {
        setWaitAnm();
        return true;
    }
    return false;
}
VERIFY(0x022C2588, &daNpcPeople_c::eventMesSetPo);

/* 022C25E8 */
void daNpcPeople_c::privateCut() {
    WWHD_FUNC(0x022C25E8, void, this);
    if (mNpcNo >= 0x13) {
        JUT_ASSERT_fail(STR(0x100206E4), 0x1A84, STR(0x100206B0)); /* HD: l_npc_staff_id index */
    }
    int staffIdx = dComIfGp_evmng_getMyStaffId(staff_id(mNpcNo), nullptr, 0);
    if (staffIdx != -1) {
        s8 idx = dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl, 16, TRUE, 0);
        m797 = idx;
        if (idx == -1) {
            dComIfGp_evmng_cutEnd(staffIdx);
        } else {
            if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                switch (m797) {
                case 0: eventMesSetInit(staffIdx); break;
                case 1: eventMesSetTpInit(staffIdx); break;
                case 3: eventFlagSetInit(staffIdx); break;
                case 4: eventGetItemInit(staffIdx); break;
                case 5: eventTurnToPlayerInit(); break;
                case 6: eventUb1TalkInit(staffIdx); break;
                case 7: eventUb1TalkXyInit(staffIdx); break;
                case 0xA: eventAreaMaxInit(); break;
                case 0xB: eventCameraStopInit(); break;
                case 0xC: eventCameraStartInit(); break;
                case 0xD: eventCoCylRInit(staffIdx); break;
                case 0xF: eventMesSetPoInit(staffIdx); break;
                }
            }
            bool end;
            switch (m797) {
            case 0: end = eventMesSet(); break;
            case 1: end = eventMesSet(); break;
            case 2: end = eventMesSet2(); break;
            case 5: end = eventTurnToPlayer(); break;
            case 6: end = eventUb1Talk(); break;
            case 7: end = eventUb1TalkXy(); break;
            case 8: end = eventUb2Talk(); break;
            case 9: end = eventUbSetAnm(); break;
            case 0xE: end = eventLookPo(); break;
            case 0xF: end = eventMesSetPo(); break;
            case 4:   /* eventGetItem(): inlined, true */
            case 0xB: /* eventCameraStop(): inlined, true */
            case 3:
            default: end = true; break;
            }
            if (end) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        }
    }
}
VERIFY(0x022C25E8, &daNpcPeople_c::privateCut);

/* 022C2958 */
void daNpcPeople_c::eventMove() {
    WWHD_FUNC(0x022C2958, void, this);
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
VERIFY(0x022C2958, &daNpcPeople_c::eventMove);

/* 022C2A04 */
void daNpcPeople_c::eventOrder() {
    WWHD_FUNC(0x022C2A04, void, this);
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
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
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
VERIFY(0x022C2A04, &daNpcPeople_c::eventOrder);

/* 022C8550 */
u32 daNpcPeople_c::setAnmFromMsgTagUo(int param_1) {
    WWHD_FUNC(0x022C8550, u32, this, param_1);
    /* switch -> table: wait, wait, talk, talk, bikkuri, furue, miburui, kyoro2 */
    if ((u32)param_1 <= 7) {
        return setAnmTbl(anm(gabi::load<u32>(0x10020944 + param_1 * 4)), 1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C8550, &daNpcPeople_c::setAnmFromMsgTagUo);

/* 022C8570 */
u32 daNpcPeople_c::setAnmFromMsgTagUw(int param_1) {
    WWHD_FUNC(0x022C8570, u32, this, param_1);
    m77A = home.angle.y;
    m79E = 1;
    if ((u32)param_1 < 7) {
        /* wait, wait, talk, talk, talk3, talk4, talkH */
        return setAnmTbl(anm(gabi::load<u32>(0x10020964 + param_1 * 4)), 1);
    } else if (param_1 == 7) {
        m77A = current.angle.y;
        m79E = 0;
        return setAnmTbl(anm(0x101C31C9) /* l_npc_anm_Mojimoji */, 1);
    } else if (param_1 == 8) {
        return setAnmTbl(anm(0x101C31CC) /* l_npc_anm_happy */, 1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C8570, &daNpcPeople_c::setAnmFromMsgTagUw);

/* 022C85DC */
u32 daNpcPeople_c::setAnmFromMsgTagUm(int param_1) {
    WWHD_FUNC(0x022C85DC, u32, this, param_1);
    m79E = 1;
    switch (param_1) {
    case 0:
        return setAnmTbl(anm(0x101C31B4) /* l_npc_anm_wait */, 1);
        break;
    case 1:
    case 7:
        return setAnmTbl(anm(0x101C31B7) /* l_npc_anm_talk */, 1);
        break;
    case 2:
        return setAnmTbl(anm(0x101C31CF) /* l_npc_anm_shobon_um */, 1);
        break;
    case 3:
        return setAnmTbl(anm(0x101C31D2) /* l_npc_anm_happy_um */, 1);
        break;
    case 4:
        m79E = 0;
        return setAnmTbl(anm(0x101C31D8) /* l_npc_anm_um3_wait3 */, 1);
        break;
    case 5:
        return setAnmTbl(anm(0x101C31DB) /* l_npc_anm_um3_talk2 */, 1);
        break;
    case 6:
        return setAnmTbl(anm(0x101C31DE) /* l_npc_anm_um3_talk3 */, 1);
        break;
    case 8:
        return setAnmTbl(anm(0x101C31D5) /* l_npc_anm_happy2_um */, 1);
        break;
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C85DC, &daNpcPeople_c::setAnmFromMsgTagUm);

/* 022C86D0 */
u32 daNpcPeople_c::setAnmFromMsgTagSa(int param_1) {
    WWHD_FUNC(0x022C86D0, u32, this, param_1);
    /* switch -> table: wait, talk, talk2_sa, wait, talk3_sa, kiai_sa, talk, talk, talk_sa, talk */
    if ((u32)param_1 <= 9) {
        return setAnmTbl(anm(gabi::load<u32>(0x10020980 + param_1 * 4)), 1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C86D0, &daNpcPeople_c::setAnmFromMsgTagSa);

/* 022C86F0 */
u32 daNpcPeople_c::setAnmFromMsgTagUg(int param_1) {
    WWHD_FUNC(0x022C86F0, u32, this, param_1);
    /* switch -> table: wait, talk, talk2_ug */
    if ((u32)param_1 <= 2) {
        return setAnmTbl(anm(gabi::load<u32>(0x100209A8 + param_1 * 4)), 1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022C86F0, &daNpcPeople_c::setAnmFromMsgTagUg);
