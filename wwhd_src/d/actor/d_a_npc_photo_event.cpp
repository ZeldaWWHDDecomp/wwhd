/**
 * d_a_npc_photo_event.cpp (WWHD)
 * NPC - Lenzo: event order/move, the event cuts and their actions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_photo.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_photo.h"

// HD bindings and helpers.
// HD: mDoAud_seStart(id) with the default position/parameters

/* dComIfGp_event_getPt2(): dEvt_control_c::convPId(play + 0x51D0, mPt2 (play + 0x5298)) */

// HD: daNpcPhoto_c's message functions are virtual (vtable at +0xB4): slot 0x1C getMsg
static inline u32 photo_v_getMsg(daNpcPhoto_c* self) { return gabi::call_ptr<u32>(gabi::object_at<PhotoMessageVtable>(self->__vtbl).getMsg.get(), self); }


/* 022CD3F8 */
void daNpcPhoto_c::checkOrder() {
    u16 command = eventInfo_getCommand(this);
    if (command == dEvtCmd_INDEMO_e) {
        if (dComIfGp_evmng_startCheck(mPhotoLinkBackEventIdx) && field_0x9BE == 3) {
            field_0x9BE = 0;
        } else if (dComIfGp_evmng_startCheck(mPhotoGetItemEventIdx) && field_0x9BE == 4) {
            field_0x9BE = 0;
        } else if (dComIfGp_evmng_startCheck(mPhotoGetItem2EventIdx) && field_0x9BE == 5) {
            field_0x9BE = 0;
        } else if (dComIfGp_evmng_startCheck(mPhotoGetPhotoEventIdx) && field_0x9BE == 6) {
            field_0x9BE = 0;
        } else if (dComIfGp_evmng_startCheck(mPhotoGalleryEventIdx) && field_0x9BE == 7) {
            field_0x9BE = 0;
        } else if (dComIfGp_evmng_startCheck(mPhotoDateUB4EventIdx) && field_0x9BE == 10) {
            field_0x9BE = 0;
        }
    } else if (command == dEvtCmd_INTALK_e && (field_0x9BE == 2 || field_0x9BE == 1)) {
        field_0x9BC = true;
        executeSetMode(1);
    }
}
/* 022CD570 */
/* (unnamed by the matcher) */
void daNpcPhoto_c::setMessage(u32 msg) {
    mCurrMsgNo = msg;
    mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
}
/* 022CD580 */
void daNpcPhoto_c::eventMesSetInit(int i_staffId) {
    be<u32>* pData = (be<u32>*)dComIfGp_evmng_getMyIntegerP(i_staffId, "MsgNo" /* "MsgNo" */);
    if (pData != nullptr) {
        field_0x980 = nullptr;
        field_0x9D0 = 0;
        u32 msgNo = *pData;
        switch (msgNo) {
        case 0:
            if (!dComIfGs_isEventBit(0x1601 /* l_save_dat.field_0x04 */)) {
                field_0x9D0 = 0;
                field_0x980 = l_msg_1st_talk_photo;
                dComIfGs_onEventBit(0x1601);
            } else {
                u32 temp = 0;
                fopAc_ac_c* ac = dComIfGp_event_getPt2(); /* daTagPhoto_c */
                if (ac != nullptr) {
                    temp = static_cast<daTagPhoto_c*>(ac)->mTagNo; /* getTagNo() */
                    if (temp >= 7) { // HD: l_msg_talk_photo bound check
                        JUT_ASSERT_fail("d_a_npc_photo.cpp", 0x896, "tagNo < sizeof(l_msg_talk_photo) / sizeof(l_msg_talk_photo[0])");
                        setMessage(*field_0x980);
                        return;
                    }
                }
                field_0x9D0 = 0;
                field_0x980 = &gabi::object_at<be<u32>>(l_msg_talk_photo[temp]);
            }
            setMessage(*field_0x980);
            return;
        case 1:
            setMessage(photo_v_getMsg(this));
            return;
        case 99:
            mDoAud_seStart_1(0x907 /* JA_SE_UTSUSHIE_TO_DX */);
            break;
        default:
            setMessage(msgNo);
            return;
        }
    }
    field_0x980 = field_0x980.get() + 1;
    if (field_0x9D0 != 0) {
        field_0x9D0 = field_0x9D0 + 1;
    }
    setMessage(*field_0x980);
}
/* 022CD718 */
void daNpcPhoto_c::eventSeSetInit(int i_staffId) {
    dComIfGp_evmng_getMyIntegerP(i_staffId, "SeNo" /* "SeNo" */);
    mDoAud_seStart_1(0x907 /* JA_SE_UTSUSHIE_TO_DX */); /* every case */
}
/* cXyz copy through FPRs (a struct assignment GHS computes with) */
static inline void cxyz_cpf(cXyz* d, const cXyz& s) {
    f32 x = s.x, y = s.y, z = s.z;
    d->x = x;
    d->y = y;
    d->z = z;
}

/* ---- more local bindings (SHARED-CANDIDATE) ---- */


/* dEvent_manager_c (play + 0x52C4): the running event's name (*(this + 0x53C), or an
 * empty default string) */
static inline u32 dComIfGp_evmng_getRunEventName() { return photo_runEventName(dComIfGp_getPEvtManager()); }
/* sead::SafeString equality (HD form of strcmp(a, b) == 0): both strings are terminated through
 * their vtable (slot +0x14; the left one twice), then compared by pointer, then byte by byte (at
 * most 0x40001 bytes) (the same as in d_a_npc_people.cpp) */
static inline bool photo_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::object_at<PhotoSafeStringVtable>(a->__vtbl).assureTermination.get(), a);
    gabi::call_ptr(gabi::object_at<PhotoSafeStringVtable>(a->__vtbl).assureTermination.get(), a);
    u32 pa = a->mStringTop;
    gabi::call_ptr(gabi::object_at<PhotoSafeStringVtable>(b->__vtbl).assureTermination.get(), b);
    u32 pb = b->mStringTop;
    if (pa == pb) {
        return true;
    }
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::object_at<be<u8>>(pa + n);
        if (ca != gabi::object_at<be<u8>>(pb + n)) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* strcmp(name, dComIfGp_getRunEventName()) == 0 with SafeString temporaries */
static inline bool photo_isRunEvent(const char* name) {
    gabi::Local<SafeString> l;
    l->__vtbl = PHOTO_SAFESTRING_VTBL;
    l->mStringTop = gabi::guest_addr(name);
    u32 run = dComIfGp_evmng_getRunEventName();
    gabi::Local<SafeString> r;
    r->__vtbl = PHOTO_SAFESTRING_VTBL;
    r->mStringTop = run;
    return photo_SafeString_eq(l.get(), r.get());
}
/* dBgS_GndChk (this TU's vtables) */
static inline void photo_GndChk_dt(dBgS_GndChk* check) {
    gabi_destroy(*check);
}


/* cameras: 024F8044 dCam_getBody(), dCamera_c Stop/Start/Set */


/* HD picture album (dSv picture data, 02720144(save + 0x12C0)) */
static inline u32 photo_v_next_msgStatus(daNpcPhoto_c* self, be<u32>* msgNo) { return gabi::call_ptr<u32>(gabi::object_at<PhotoMessageVtable>(self->__vtbl).nextMsgStatus.get(), self, msgNo); }
static inline void photo_v_anmAtr(daNpcPhoto_c* self, u16 status) { gabi::call_ptr(gabi::object_at<PhotoMessageVtable>(self->__vtbl).setAnmAtr.get(), self, status); }

/* 022CD764 */
void daNpcPhoto_c::eventPosSetInit() {
    fopAc_ac_c* ac = dComIfGp_event_getPt2(); /* daTagPhoto_c */
    gabi::Local<be<s16>> angle;
    if (ac != nullptr) {
        f32 tz = ac->current.pos.z;
        f32 ty = ac->current.pos.y;
        f32 tx = ac->current.pos.x;
        dPath* path = dPath_GetNextRoomPath(mPathRun.mPath, fopAcM_GetRoomNo(this));
        if (path != nullptr) {
            u32 pnt = dPath_GetPnt(path, static_cast<daTagPhoto_c*>(ac)->mTagNo /* getTagNo() */);
            if (pnt != 0) {
                f32 x = gabi::object_at<path_layout::dPnt>(pnt).m_position.x;
                old.pos.x = x;
                f32 y = gabi::object_at<path_layout::dPnt>(pnt).m_position.y;
                old.pos.y = y;
                f32 z = gabi::object_at<path_layout::dPnt>(pnt).m_position.z;
                current.pos.y = y;
                old.pos.z = z;
                current.pos.x = x;
                current.pos.z = z;
                gabi::Local<dBgS_GndChk> gndChk;
                dBgS_GndChk_ct(gndChk.get(), gc_tu.gndChk, false);
                cXyz* p = &gndChk->m_pos;
                f32 gy = current.pos.y + 50.0f;
                p->z = current.pos.z;
                p->x = current.pos.x;
                p->y = gy;
                f32 floor_y = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
                if (floor_y != -1000000000.0f /* -G_CM3D_F_INF */) {
                    old.pos.y = floor_y;
                    current.pos.y = floor_y;
                }
                photo_GndChk_dt(gndChk.get());
            }
        }
        gabi::Local<cXyz> a;
        a->y = current.pos.y;
        a->x = current.pos.x;
        a->z = current.pos.z;
        gabi::Local<cXyz> b;
        b->x = tx;
        b->y = ty;
        b->z = tz;
        dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    } else {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        // HD: in PHOTO_GALLERY Lenzo stands at a second position
        cXyz* src = photo_isRunEvent("PHOTO_GALLERY") ? &l_gallery_pos[1] : &l_gallery_pos[0];
        old.pos.copy(*src);
        current.pos.copy(old.pos);
        gabi::Local<cXyz> a;
        a->x = current.pos.x;
        a->z = current.pos.z;
        a->y = current.pos.y;
        gabi::Local<cXyz> b;
        b->x = link->current.pos.x;
        b->y = link->current.pos.y;
        b->z = link->current.pos.z;
        dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    }
    s16 ang = *angle;
    current.angle.y = ang;
    shape_angle.y = ang;
    field_0x9AE = ang;
    dComIfGp_event_setTalkPartner(this);
    mPathRun.mIdx = (u8)(gabi::object_at<path_layout::dPath>(gabi::ea(mPathRun.mPath.get())).m_num /* m_num */ - 2);
    mPathRun.mbDir = 0;
    executeSetMode(0);
    field_0x9C1 = 1;
    int temp = gabi::ftoi(cM_rndF(3.0f));
    if (temp == 3) {
        temp = 0;
    }
    mMsgNno = l_msg_2F[temp]; /* l_msg_2F */
}
/* 022CDBD8 */
void daNpcPhoto_c::eventGetItemInit() {
    u32 itemID = fopAcM_createItemForPresentDemo(&current.pos, mItemNo, 0, -1, fopAcM_GetRoomNo(this), nullptr, nullptr);
    if (itemID != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(itemID);
    }
}
/* 022CDC34 */
void daNpcPhoto_c::eventSetAngleInit() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> pos;
    cXyz_mi(&current.pos, pos.get(), &link->current.pos);
    s16 ang = cM_atan2s(pos->x, pos->z);
    gabi::object_at<PhotoPlayerDemoAngle>(gabi::ea(link)).mDemoMoveAngle = ang; /* link->changeDemoMoveAngle() */
}
/* 022CDC8C */
void daNpcPhoto_c::eventSetEyeInit() {
    field_0x9B0 = 0;
    dComIfGp_event_setTalkPartner(this);
}
/* 022CDCDC */
void daNpcPhoto_c::eventTurnToPlayerInit() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> a;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    gabi::Local<cXyz> b;
    b->x = link->current.pos.x;
    b->y = link->current.pos.y;
    b->z = link->current.pos.z;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, &field_0x9AE);
}
/* 022CDD50 */
void daNpcPhoto_c::eventClrHanmeInit() {
    initTexPatternAnm(true, -1);
    field_0x9C9 = field_0x9C9 & 0x7F;
}
/* 022CDD90 */
void daNpcPhoto_c::eventGetPhotoInit() {
    /* HD: dComIfGp_setPictureStatusGetOn(dKy_get_dayofweek()) is the album: a free slot gets the
     * day's picture */
    u32 slot = photo_getFreePictureSlot(photo_album());
    if (slot < 0xC) {
        photo_markPictureSlot(photo_album(), slot);
        u8 day = dKy_get_dayofweek();
        u32 pic = photo_getPicture(photo_album(), slot);
        photo_setPictureDay(pic, day);
        u32 play = dComIfGp_ea();
        gabi::object_at<dComIfG_inf_c>(play).play.mItemPictureNumCount += 1;
    }
    field_0x9B0 = 10;
}
/* 022CDE38 */
void daNpcPhoto_c::eventMesSetUbInit(int i_staffId) {
    eventMesSetInit(i_staffId);
}
/* 022CDE3C */
void daNpcPhoto_c::setMsgCamera() {
    if (field_0x9D0 == 0) {
        return;
    }
    u32 pCam = dCam_getBody();
    if (*field_0x9D0 >= 0) {
        dCamera_Stop(pCam);
        be<s8>* p = field_0x9D0;
        gabi::Local<cXyz> temp;
        cxyz_cpf(temp.get(), l_msg_camera[*p * 2]);
        gabi::Local<cXyz> temp2;
        cxyz_cpf(temp2.get(), l_msg_camera[*p * 2 + 1]);
        mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), temp.get(), temp.get());
        PSMTXMultVec(mDoMtx_stack_c::get(), temp2.get(), temp2.get());
        cXyz* attnPos = &attention_info.position; /* attention_info.position */
        PSVECAdd(temp.get(), attnPos, temp.get());
        PSVECAdd(temp2.get(), attnPos, temp2.get());
        gabi::Local<cXyz> center;
        cxyz_cpf(center.get(), *temp);
        gabi::Local<cXyz> eye;
        cxyz_cpf(eye.get(), *temp2);
        dCamera_Set(pCam, center.get(), eye.get(), 60.0f, 0);
        field_0x9D4 = true;
    } else {
        dCamera_Start(pCam);
        field_0x9D4 = false;
    }
}
/* 022CDFAC */
u16 daNpcPhoto_c::talk2(int i_param) {
    u16 status = 0xFF;
    // HD: the message (GameCube mpCurrMsg) is the message manager
    u32 mgr = photo_msgManager();
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (i_param == 1) {
            mCurrMsgNo = photo_v_getMsg(this);
        }
        // HD: two messages are replaced on the date (mReplaceDateMessages, set by getMsg)
        if (mReplaceDateMessages) {
            if (mCurrMsgNo == 0x2A65) {
                mCurrMsgNo = 0x2A85;
            } else if (mCurrMsgNo == 0x2A66) {
                mCurrMsgNo = 0x2A5B;
            }
        }
        mCurrMsgBsPcId = msgMng_messageSet(mgr, mCurrMsgNo, &eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) { // HD: only when the message was set
            mbHasMsg = 0;
            field_0x9D5 = 1;
            setMsgCamera();
        }
    } else if (mbHasMsg) {
        status = (u16)msgMng_getStatus(mgr);
        switch (status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            msgMng_setStatus(mgr, photo_v_next_msgStatus(this, &mCurrMsgNo));
            if (msgMng_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                msgMng_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        case 6: /* fopMsgStts_MSG_TYPING_e */
            if (field_0x9D5 != 0) {
                field_0x9D5 = 0;
                setMsgCamera();
            }
            break;
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            if (field_0x9D4) {
                dCamera_Start(dCam_getBody());
            }
            if (field_0x9C6 & 0x20) {
                field_0x9C6 = (field_0x9C6 & ~0x20) | 0x40;
            }
            msgMng_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        if (status != 6) {
            field_0x9D5 = 1;
        }
        photo_v_anmAtr(this, status);
    } else {
        // HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag
        mbHasMsg = 1;
    }
    return status;
}
/* 022CE1E0 */
bool daNpcPhoto_c::eventMesSet() {
    // HD: in PHOTO_GALLERY Lenzo walks to l_gallery_pos while he talks (but for message 0x2A68)
    if (mCurrMsgNo != 0x2A68 && photo_isRunEvent("PHOTO_GALLERY")) {
        cLib_addCalcPosXZ(&current.pos, &l_gallery_pos[0], 0.2f, 40.0f, 4.0f);
    }
    return talk2(0) == fopMsgStts_BOX_CLOSED_e;
}
/* 022CE33C */
bool daNpcPhoto_c::eventSetEye() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    u16 t = field_0x9B0;
    s16 temp = (s16)gabi::ftoi(gabi::fmadds(cM_ssin(t), 12288.0f, (f32)(s32)link->shape_angle.y));
    field_0x9B0 = t + 0x400;
    mEyePos.x = 0.0f;
    mEyePos.y = 0.0f;
    mEyePos.z = 100.0f;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), temp);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), -0x1000);
    PSMTXMultVec(mDoMtx_stack_c::get(), &mEyePos, &mEyePos);
    PSVECAdd(&mEyePos, &link->eyePos, &mEyePos);
    return TRUE;
}
/* 022CE434 */
bool daNpcPhoto_c::eventTurnToPlayer() {
    return current.angle.y == field_0x9AE;
}
/* 022CE44C */
bool daNpcPhoto_c::eventGetPhoto() {
    field_0x9B0 = field_0x9B0 - 1;
    return field_0x9B0 == 0;
}
/* 022CE468 */
bool daNpcPhoto_c::eventLookUb() {
    fopAc_ac_c* pActor = fopAcM_searchFromName("Ub4" /* "Ub4" */, 0, 0);
    if (pActor != nullptr) {
        mLookAtPos.copy(pActor->eyePos);
        field_0x9D6 = 1;
        field_0x994 = false;
    }
    return true;
}
/* 022CE4D8 */
u32 daNpcPhoto_c::eventMesSetUb() {
    eventLookUb();
    return photo_eventMesSetRegister(this); /* eventMesSet(): its r3 as is */
}
/* 022CE50C */
void daNpcPhoto_c::privateCut() {
    /* cut_name_tbl (.data 0x101C52C0): MES_SET, SE_SET, POS_SET, GET_ITEM, SET_ANGLE, SET_EYE,
     * TURN_TO_PLAYER, CLR_HANME, GET_PHOTO, MES_SET_UB, LOOK_UB */
    int staffIdx = dComIfGp_evmng_getMyStaffId(l_npc_staff_id, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    mActIdx = photo_getMyActIdx(staffIdx, 0x101C52C0, 11, TRUE, 0);
    if (mActIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch ((s32)mActIdx) {
        case 0: eventMesSetInit(staffIdx); break;
        case 1: eventSeSetInit(staffIdx); break;
        case 2: eventPosSetInit(); break;
        case 3: eventGetItemInit(); break;
        case 4: eventSetAngleInit(); break;
        case 5: eventSetEyeInit(); break;
        case 6: eventTurnToPlayerInit(); break;
        case 7: eventClrHanmeInit(); break;
        case 8: eventGetPhotoInit(); break;
        case 9: eventMesSetUbInit(staffIdx); break;
        }
    }
    bool evtRes;
    switch ((s32)mActIdx) {
    case 0: evtRes = eventMesSet(); break;
    /* case 3: eventGetItem() (HD: inlined, true) */
    case 5: evtRes = eventSetEye(); break;
    case 6: evtRes = eventTurnToPlayer(); break;
    case 8: evtRes = eventGetPhoto(); break;
    case 9: evtRes = eventMesSetUb(); break;
    case 10: evtRes = eventLookUb(); break;
    default: evtRes = true; break;
    }
    if (evtRes) {
        dComIfGp_evmng_cutEnd(staffIdx);
    }
}
/* 022CE7B0 */
void daNpcPhoto_c::setAnmFromMsgTag() {
    sPhotoAnmDat* wait = l_npc_anm_wait; /* l_npc_anm_wait */
    sPhotoAnmDat* talk = l_npc_anm_talk; /* l_npc_anm_talk */
    sPhotoAnmDat* talk2 = l_npc_anm_talk2; /* l_npc_anm_talk2 */
    switch (dComIfGp_getMesgAnimeAttrInfo()) {
    case 0:
        setAnmTbl(wait);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    case 1:
        setAnmTbl(talk);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    case 2:
    case 4:
        setAnmTbl(talk2);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    case 3:
        setAnmTbl(wait);
        initTexPatternAnm(true, 0);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    case 5:
        setAnmTbl(wait);
        initTexPatternAnm(true, 1);
        mFrame = 1;
        field_0x9C9 = field_0x9C9 | 0x80;
        break;
    case 6:
        setAnmTbl(talk);
        initTexPatternAnm(true, 0);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    case 9:
        setAnmTbl(l_npc_anm_spit /* l_npc_anm_spit */);
        field_0x9C9 = field_0x9C9 & 0x7F;
        break;
    }
    dComIfGp_setMesgAnimeAttrInfo(0xFF); /* dComIfGp_setMesgAnimeAttrInfo(0xFF) */
}
/* 022CE97C */
void daNpcPhoto_c::eventMove() {
    if (dComIfGp_evmng_endCheck(mPhotoLinkBackEventIdx)) {
        field_0x9C6 = field_0x9C6 | 0x40;
    } else if (dComIfGp_evmng_endCheck(field_0x9A6)) {
        field_0x9C7 = true;
        field_0x9BE = 0;
        field_0x9BC = false;
        eventInfo_setEventId(this, -1);
        field_0x9C6 = field_0x9C6 | 0x40;
        executeSetMode(0);
    } else if (dComIfGp_evmng_endCheck(mPhotoGetItemEventIdx) || dComIfGp_evmng_endCheck(mPhotoGetItem2EventIdx) ||
               dComIfGp_evmng_endCheck(mPhotoGetPhotoEventIdx) || dComIfGp_evmng_endCheck(mPhotoGalleryEventIdx)) {
        field_0x9C6 = field_0x9C6 | 0x40;
    } else if (dComIfGp_evmng_endCheck(mPhotoDateUB4EventIdx)) {
        field_0x9D8 = true;
        field_0x9D7 = true;
        field_0x9C6 = field_0x9C6 | 0x50;
    } else {
        u8 temp = mEventCut.mbAttention; /* getAttnFlag() */
        if (mEventCut.cutProc()) {
            if (!mEventCut.mbAttention) {
                mEventCut.mbAttention = temp; /* setAttnFlag() */
            }
        } else {
            privateCut();
            setAnmFromMsgTag();
        }
    }
}
/* 022CEB58 */
void daNpcPhoto_c::eventOrder() {
    if (field_0x9C6 & 0x40) {
        field_0x9C6 = field_0x9C6 & ~0x40;
        dComIfGp_event_reset();
        initTexPatternAnm(true, -1);
        field_0x9C7 = true;
        field_0x9BC = false;
        field_0x9C9 = field_0x9C9 & 0x7F;
        executeSetMode(0);
    }
    u8 temp = field_0x9BE;
    if (temp == 2 || temp == 1) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (field_0x9BE == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (temp == 3) {
        fopAcM_orderChangeEventId(dComIfGp_getLinkPlayer(), this, mPhotoLinkBackEventIdx, 0, 0xFFFF);
    } else if (temp == 4) {
        fopAcM_orderChangeEventId(dComIfGp_getLinkPlayer(), this, mPhotoGetItemEventIdx, 0, 0xFFFF);
    } else if (temp == 5) {
        fopAcM_orderChangeEventId(dComIfGp_getLinkPlayer(), this, mPhotoGetItem2EventIdx, 0, 0xFFFF);
    } else if (temp == 6) {
        fopAcM_orderChangeEventId(dComIfGp_getLinkPlayer(), this, mPhotoGetPhotoEventIdx, 0, 0xFFFF);
    } else if (temp == 7) {
        fopAcM_orderOtherEventId(this, mPhotoGalleryEventIdx, 0xFF, 0xFFFF, 0, 1);
    } else if (temp == 10) {
        fopAcM_orderOtherEventId(this, mPhotoDateUB4EventIdx, 0xFF, 0xFFFF, 0, 1);
    }
}

/* WWHD glue (tools/verify/autoglue.py): data owned by d_a_npc_photo.cpp
str 10020E50 "MsgNo" in 022CD580
str 10020E58 "d_a_npc_photo.cpp" in 022CD580
str 10020E6C "tagNo < sizeof(l_msg_talk_photo) / sizeof(l_msg_talk_photo[0])" in 022CD580
str 10020EAC "SeNo" in 022CD718
str 10020EEC "Ub4" in 022CE468
str 10020EBC "PHOTO_GALLERY" in 022CD764
str 10020ED4 "PHOTO_GALLERY" in 022CE1E0
*/
