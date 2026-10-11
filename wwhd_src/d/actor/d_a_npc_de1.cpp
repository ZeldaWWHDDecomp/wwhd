/**
 * d_a_npc_de1.cpp (WWHD)
 * NPC - Great Deku Tree
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_de1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx. Names follow the GameCube symbols.
 */
#include "d/actor/d_a_npc_de1.h"
static BOOL CheckCreateHeap(fopAc_ac_c*);

const gc_tu_data gc_tu = {.safeString = 0x10019664};
static auto& l_HIO = gabi::object_at<daNpc_De1_HIO_c>(0x10466D18);
static auto& l_lift_num = gabi::object_at<be<s32>>(0x10466D08);
static auto& l_lift_actor = gabi::object_at<gptr<fopAc_ac_c>[100]>(0x10466D50);
static auto& a_lift_jnt_names = gabi::object_at<gptr<char>[10]>(0x101BDF24);
static auto& l_heapSize = gabi::object_at<be<u32>>(0x101BDF4C);
static auto& a_anm_tbl = gabi::object_at<De1AnmRes[7]>(0x10019800);
static auto& a_stt_anm_tbl = gabi::object_at<daNpc_De1_c::anm_prm_c[8]>(0x101BDF50);
static auto& a_anm_num_tbl = gabi::object_at<daNpc_De1_c::anm_prm_c[7]>(0x101BDFD0);
static auto& a_atr_anm_tbl = gabi::object_at<daNpc_De1_c::anm_prm_c[4]>(0x101BE050);
static auto& a_cut_tbl = gabi::object_at<gptr<char>[1]>(0x101BE040);
static auto& a_event_tbl = gabi::object_at<be<u32>[6]>(0x101BE038);
static auto& a_lift_prm_tbl = gabi::object_at<be<u32>[10]>(0x100198E0);
static auto& a_hio_prm = gabi::object_at<daNpc_De1_HIO_c::hio_prm_c>(0x101BE090);
static auto& l_wait_action1 = gabi::object_at<De1ActionFunc>(0x10019658);
static auto& l_wait_action2 = gabi::object_at<De1ActionFunc>(0x10019650);
static auto& l_demoObject = gabi::object_at<be<u32>>(0x101D5FFC);
static auto& l_message = gabi::object_at<gptr<void>>(0x101F4B5C);
/* 0222A8FC */
static void* searchActor_leafLift(void* i_actor, void*) {
    if ((s32)l_lift_num < 100 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == DE1_LEAF_SEARCH_NAME /* PROC_Obj_Llift */) {
        s32 n = (s32)l_lift_num;
        l_lift_num = n + 1;
        l_lift_actor[n] = (fopAc_ac_c*)i_actor;
    }
    return nullptr;
}

/* 0222B300 */
BOOL daNpc_De1_c::createInit() {
    mEventCut2.setActorInfo2("De1" /* "De1" */, this);
    s8 type = mType;
    attention_info.flags = 0xA;                               /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0x16; /* attention_info.distances[3] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0x15;  /* attention_info.distances[1] */
    De1_copyBits(&mHomePos.x, &current.pos.x);
    De1_copyBits(&mHomePos.y, &current.pos.y);
    De1_copyBits(&mHomePos.z, &current.pos.z);
    gravity = 0.0f;
    if ((u32)(s32)type < 1) {
        if (!dComIfGs_isEventBit(0x1801)) {
            De1ActionFunc pmf;
            De1_loadAction(&pmf, &l_wait_action1);
            set_action(&pmf, nullptr);
        } else {
            De1ActionFunc pmf;
            De1_loadAction(&pmf, &l_wait_action2);
            set_action(&pmf, nullptr);
        }
    } else if ((u32)(s32)type == 1) {
        De1ActionFunc pmf;
        De1_loadAction(&pmf, &l_wait_action2);
        set_action(&pmf, nullptr);
    }
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    mpMorf->setMorf(0.0f);
    mEyePosEnabled = 1;
    setMtx();
    return TRUE;
}

/* 0222B0CC */
void daNpc_De1_c::setMtx() {
    if (mInDemo == 0) {
        mAnmEnd = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mAnmFrame) {
            mAnmEnd = 1;
        }
        De1_view<be<u32>>(&mAnmFrame)[0] =
            De1_view<De1MorfFrame>(mpMorf.get())->mFrameBits; /* frame */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    if (De1_calcTimer(&mSeTimer) == 0 && mAnmEnd != 0 && mAnmNum == 3) {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x48B9, &current.pos, 0, reverb);
    }
    auto* gnd = static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd); /* ground polygon info */
    tevStr.mRoomNo = De1_roomId(dComIfG_Bgsp(), gnd);
    tevStr.mEnvrIdxOverride = De1_polyColour(dComIfG_Bgsp(), gnd);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    f32 s = l_HIO.mPrm.mScale;
    mDoMtx_stack_c::scaleM(s, s, s);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    De1_copyBackVertices(mpBgW);
    mpMorf->calc();
    De1_deformCalc(&mDeformRes, mpMorf->getModel());
    dBgW* bgw = mpBgW;
    u32 old = De1_view<De1BgW>(bgw)->mpVertices;
    De1_view<De1BgW>(bgw)->mpVertices = mDeformVtx;
    if (old == 0) {
        De1_copyBackVertices(bgw);
    }
    dBgW_Move(bgw);
    followPa_happa();
    setDemoStartCenter();
    if (mType == 0 && !dComIfGs_isEventBit(0x1801)) {
        cc_set();
    }
    setAttention();
}

/* 0222B990 */
BOOL daNpc_De1_c::anmResID(u32 i_anmNum, be<s32>* o_bckIdx, be<s32>* o_soundIdx) {
    if (i_anmNum >= 7) /* JUT_ASSERT(317, 0 <= i_anmNum && i_anmNum < 7) */
        JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x13D, "0 <= i_num && i_num < ANM_END");
    if (o_bckIdx == nullptr || o_soundIdx == nullptr)
        JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x13E, "o_bck_num && o_bas_num");

    *o_bckIdx = a_anm_tbl[i_anmNum].mBck;
    *o_soundIdx = a_anm_tbl[i_anmNum].mSound;
    return TRUE;
}

/* 0222BAC8 */
BOOL daNpc_De1_c::setAnm_anm(anm_prm_c* i_anmPrm) {
    s8 num = i_anmPrm->mAnmNum;
    BOOL ret = FALSE;
    if (num < 0 || mAnmNum == num) {
        return FALSE;
    }
    mAnmNum = num;
    if (mpMorf.get() != nullptr) {
        De1Int bckIdx;
        De1Int soundIdx;
        anmResID((u32)(s32)num, &bckIdx, &soundIdx);
        if (bckIdx >= 0) {
            dNpc_setAnmIDRes(mpMorf, i_anmPrm->mLoopMode, i_anmPrm->mMorf, i_anmPrm->mSpeed, bckIdx,
                             soundIdx, "De" /* "De" */);
        }
        if (mAnmNum == 3) {
            set_pa_happa();
        } else {
            del_pa_happa();
        }
        ret = TRUE;
    }
    mLoopCnt = 0;
    mAnmFrame = 0.0f;
    mAnmEnd = 0;
    return ret;
}

/* 0222BC18 */
BOOL daNpc_De1_c::setAnm_NUM(int i_num) { return setAnm_anm(&a_anm_num_tbl[i_num]); }

/* 0222BBA8 */
BOOL daNpc_De1_c::setAnm() { return setAnm_anm(&a_stt_anm_tbl[mStt]); }

/* 0222C448 */
void daNpc_De1_c::chngAnmAtr(u32 i_atr) {
    if (i_atr >= 4 || i_atr == mAnmAtr) {
        return;
    }
    mAnmAtr = (u8)i_atr;
    setAnm_ATR();
}

/* 0222C464 */
void daNpc_De1_c::ctrlAnmAtr() {
    if (mAnmAtr != 3 || mAnmEnd == 0) {
        return;
    }
    mAnmAtr = 0;
    setAnm_ATR();
}

/* 0222C430 */
BOOL daNpc_De1_c::setAnm_ATR() { return setAnm_anm(&a_atr_anm_tbl[mAnmAtr]); }

/* 0222C488 */
void daNpc_De1_c::anmAtr(u32 i_msgStatus) {
    if (i_msgStatus == 6) {
        if (mMsgAtrInit == 0) {
            mMsgAnmAtr = 0xFF;
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mMsgAtrInit = (s8)(mMsgAtrInit + 1);
        }
        u8 atr = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgCameraAttrInfo (probably) */
        if (atr != 0xFF && atr != mMsgAnmAtr) {
            dComIfGp_clearMesgAnimeTagInfo();
            mMsgAnmAtr = atr;
        }
    } else if (i_msgStatus == 0xE) {
        mMsgAtrInit = 0;
    }
    ctrlAnmAtr();
}

/* 0222BBC4 */
void daNpc_De1_c::setStt(u32 i_stt) {
    s8 old = mStt;
    mStt = (s8)i_stt;
    switch (i_stt) {
    case 2:
    case 4:
        mPrevStt = old;
        mAnmAtr = 0xFF;
        return;
    case 7:
        attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x22; /* attention_info.distances[2] */
        attention_info.flags |= 0x4000000;
        setAnm();
        return;
    default:
        setAnm();
        return;
    }
}

/* 0222C544 */
u16 daNpc_De1_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 status = fopMsgStts_MSG_CONTINUES_e;
    switch (*pMsgNo) {
    case 0x139D:
        *pMsgNo = 0x139E;
        break;
    case 0x139F:
        *pMsgNo = 0x13A0;
        break;
    case 0x13A1:
        *pMsgNo = 0x13A2;
        break;
    case 0x13A3:
        *pMsgNo = 0x13A4;
        break;
    case 0x13A4:
        *pMsgNo = 0x13A5;
        break;
    case 0x13A5:
        *pMsgNo = 0x13A6;
        break;
    case 0x13A8:
    case 0x13AC:
        *pMsgNo = 0x13A9;
        break;
    case 0x13A9:
    case 0x13C5:
        if (dComIfGs_isEventBit(0x1D40)) {
            *pMsgNo = 0x13AA;
        } else {
            *pMsgNo = 0x13AB;
        }
        break;
    case 0x13AA: {
        u32 sel = De1_view<De1Message>(l_message.get())->mSelectNum;
        if (sel < 1) {
            *pMsgNo = 0x13AD;
        } else if (sel == 1) {
            if (dComIfGs_isEventBit(0x102)) {
                *pMsgNo = 0x13D2;
            } else {
                *pMsgNo = 0x13C7;
            }
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AB: {
        u32 sel = De1_view<De1Message>(l_message.get())->mSelectNum;
        if (sel < 1) {
            *pMsgNo = 0x13AD;
        } else if (sel == 1) {
            *pMsgNo = 0x13CC;
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AD: {
        u32 sel = De1_view<De1Message>(l_message.get())->mSelectNum;
        if (sel < 1) {
            if (dComIfGs_isEventBit(0x102)) {
                *pMsgNo = 0x13B0;
            } else {
                *pMsgNo = 0x13AE;
            }
        } else if (sel == 1) {
            *pMsgNo = 0x13B6;
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AE:
    case 0x13AF:
    case 0x13B0:
    case 0x13C1:
    case 0x13C3:
    case 0x13C4:
        *pMsgNo = 0x13C5;
        break;
    case 0x13B6:
        if (dComIfGs_isEventBit(0x1820)) {
            if (dComIfGs_isStageBossEnemy(6)) {
                *pMsgNo = 0x13C2;
            } else {
                *pMsgNo = 0x13C4;
            }
        } else {
            *pMsgNo = 0x13C0;
        }
        break;
    case 0x13C0:
        *pMsgNo = 0x13C1;
        break;
    case 0x13C2:
        *pMsgNo = 0x13C3;
        break;
    case 0x13C7:
        *pMsgNo = 0x13C8;
        break;
    case 0x13C8:
        *pMsgNo = 0x13C9;
        break;
    case 0x13C9:
        *pMsgNo = 0x13CA;
        break;
    case 0x13CA:
        *pMsgNo = 0x13CB;
        dComIfGs_onEventBit(0x3940);
        break;
    case 0x13CC:
        *pMsgNo = 0x13CD;
        break;
    case 0x13CD:
        *pMsgNo = 0x13CE;
        break;
    case 0x13CE:
        *pMsgNo = 0x13CF;
        break;
    case 0x13CF:
        *pMsgNo = 0x13D0;
        break;
    case 0x13D0:
        *pMsgNo = 0x13D1;
        break;
    default:
        status = fopMsgStts_MSG_ENDS_e; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}

/* 0222C998 */
u32 daNpc_De1_c::getMsg() {
    switch ((u32)(s32)mType) {
    case 0:
        if (dComIfGs_checkGetItem(0x20)) {
            return De1_firstMessage(0xE40, 0x139F, 0x13A1);
        }
        return De1_firstMessage(0xE80, 0x139C, 0x139D);
    case 1:
        if (dComIfGs_isEventBit(0x1C40)) {
            if (mbSecondTalk != 0) {
                return 0x13AC;
            }
            mbSecondTalk = 1;
            return 0x13A8;
        }
        return De1_firstMessage(0x1C20, 0x13A3, 0x13A7);
    default:
        return 0;
    }
}

/* 0222C1AC */
void daNpc_De1_c::eventOrder() {
    s8 order = mOrder;
    if (order == 1 || order == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e);
        if (mOrder == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {

        u32 name = a_event_tbl[order];
        if (order == 3) {
            De1_orderNamedEvent(this, name, 1, 0xF);
        } else {
            De1_orderNamedEvent(this, name, 1, 0xFFFF);
        }
    }
}

/* 0222B6E8 */
void daNpc_De1_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        if (De1_startNamedEvent(dComIfGp_getPEvtManager(), "LIFT_UP" /* "LIFT_UP" */) &&
            mOrder == 3) {
            mOrder = 0;
            return;
        }
        if (De1_startNamedEvent(dComIfGp_getPEvtManager(), "DE_CHUCHU" /* "DE_CHUCHU" */) &&
            mOrder == 4) {
            De1_audioRequest(0x80000045);
            mOrder = 0;
            return;
        }
        if (De1_startNamedEvent(dComIfGp_getPEvtManager(), "contact" /* "contact" */) &&
            mOrder == 5) {
            dComIfGs_onEventBit(0x1801);
            De1_nextStage("Omori" /* "Omori" */, 0xD5, 0, 8, 0.0f, 0, 1, 0);
            mOrder = 0;
        }
    } else if (cmd == dEvtCmd_INTALK_e) {
        if (mOrder == 1 || mOrder == 2) {
            mOrder = 0;
            mTalkReq = 1;
        }
    }
}

/* 0222CA64 */
u8 daNpc_De1_c::chkAttention() {
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift == nullptr) {
        return FALSE;
    }
    f32 dist = l_HIO.mPrm.mAttDist;
    if (mAttention != 0) {
        dist = dist + 50.0f;
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz diff;
    cXyz_mi(&player->current.pos, &diff, &lift->current.pos);
    cXyz xz;
    De1_copyBits(&xz.x, &diff.x);
    xz.y = 0.0f;
    De1_copyBits(&xz.z, &diff.z);
    f32 len = std_sqrtf(PSVECSquareMag(&xz));
    return !(dist < len);
}

/* 0222B008 */
void daNpc_De1_c::setAttention() {
    cXyz offset;
    offset.x = 0.0f;
    offset.z = 700.0f;
    offset.y = 1100.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &mAttPos);
    De1_copyBits(&mEyePos.x, &mAttPos.x);
    De1_copyBits(&mEyePos.y, &mAttPos.y);
    De1_copyBits(&mEyePos.z, &mAttPos.z);
    De1_copyBits(&eyePos.x, &mAttPos.x);
    De1_copyBits(&eyePos.y, &mAttPos.y);
    De1_copyBits(&eyePos.z, &mAttPos.z);
    cXyz* att = &attention_info.position; /* attention_info.position */
    De1_copyBits(&att->x, &mAttPos.x);
    att->y = gabi::fadds_ppc(mAttPos.y, l_HIO.mPrm.mAttOffsetY);
    De1_copyBits(&att->z, &mAttPos.z);
}

/* 0222AF38 */
fopAc_ac_c* daNpc_De1_c::searchByID(u32 i_id) {
    De1Word actor;
    actor = 0;
    De1_searchById(i_id, &actor);
    return De1_actor(actor);
}

/* 0222AEB4 */
void daNpc_De1_c::setDemoStartCenter() {
    cXyz offset; /* l_HIO demo start offset */
    De1_copyBits(&offset.x, &l_HIO.mPrm.mDemoStartOffset.x);
    De1_copyBits(&offset.y, &l_HIO.mPrm.mDemoStartOffset.y);
    De1_copyBits(&offset.z, &l_HIO.mPrm.mDemoStartOffset.z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset, &mDemoStartCenter);
}

/* 0222CB30 */
BOOL daNpc_De1_c::partner_srch() {
    mPartnerId = 0xFFFFFFFF;
    l_lift_num = 0;
    for (int i = 0; i < 100; i++) {
        l_lift_actor[i] = nullptr;
    }
    fpcM_Search(gabi::guest_fn((const void*)searchActor_leafLift), this);
    if ((s32)l_lift_num != 0) {
        mPartnerId = fopAcM_GetID(l_lift_actor[0].get());
        return TRUE;
    }
    return FALSE;
}

/* 0222CBE0 */
void daNpc_De1_c::ccCreate() {

    u32 prm[10];
    for (int i = 0; i < 10; i++) {
        prm[i] = a_lift_prm_tbl[i];
    }
    for (int i = 0; i < 10; i++) {
        u32 id = fopAcM_create(DE1_LEAF_CREATE_NAME /* PROC_Obj_Llift? */, prm[i], &current.pos,
                               fopAcM_GetRoomNo(this), nullptr, nullptr, -1, 0);
        mLiftId[i] = id;
        if (id == 0xFFFFFFFF)
            JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x39F, "m_cc_ID[ i] != fpcM_ERROR_PROCESS_ID_e");
    }
}

/* 0222AF6C */
void daNpc_De1_c::cc_set() {
    for (int i = 0; i < 10; i++) {
        fopAc_ac_c* lift = searchByID(mLiftId[i]);
        if (lift != nullptr) {
            PSMTXCopy(mpMorf->getModel()->getAnmMtx(mLiftJnt[i]), mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), &De1_view<De1LeafLift>(lift)->mFollowMtx);
        }
    }
}

/* 0222BA40 */
void daNpc_De1_c::set_pa_happa() {
    mPaHappa.remove();
    s8 room = fopAcM_GetRoomNo(this);
    dPa_control_c* pa = dComIfGp_getParticle();
    dPa_control_set(pa, 0, 0x81BA, &mPaHappaPos, &current.angle, nullptr, 0xFF,
                    (dPa_levelEcallBack*)&mPaHappa, room, nullptr, nullptr, nullptr);
}

/* 0222B638 */
void daNpc_De1_c::del_pa_happa() { mPaHappa.remove(); }

/* 0222AE24 */
void daNpc_De1_c::followPa_happa() {
    if (mPaHappa.getEmitter() != nullptr) {
        PSMTXCopy(mpMorf->getModel()->getAnmMtx(mBranchJnt), mDoMtx_stack_c::get());
        Mtx34* m = mDoMtx_stack_c::get();
        De1_copyBits(&mPaHappaPos.x, &m->m[0][3]);
        De1_copyBits(&mPaHappaPos.y, &m->m[1][3]);
        De1_copyBits(&mPaHappaPos.z, &m->m[2][3]);
    }
}

/* 0222AC84 */
BOOL daNpc_De1_c::decideType(int i_prm) {
    mSubtype = -1;
    if (fpcM_GetName(this) == fpcNm_NPC_DE1_e /* PROC_NPC_DE1 */) {
        mSubtype = 0;
        mType = dComIfGs_isSymbol(2) != 0;
        return TRUE;
    }
    return FALSE;
}

/* 0222BC2C */
void daNpc_De1_c::event_actionInit(int i_staffIdx) {
    be<s32>* actNo = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "ActNo" /* "ActNo" */, 3);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "Timer" /* "Timer" */, 3);
    be<s32>* num = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, "Num" /* "Num" */, 3);
    if (actNo == nullptr) {
        return;
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 act = De1_lowByte(actNo);
    mEvtActNo = act;
    switch ((u32)(s32)act) {
    case 1: /* the selected leaf lift becomes the item partner */
        if (num != nullptr) {
            fopAc_ac_c* lift = searchByID(mLiftId[(s32)*num]);
            if (lift != nullptr) {
                dEvt_control_c* evt = dComIfGp_getEvent();
                De1_view<De1EventPartner>(evt)->mItemPartner = De1_eventPid(evt, lift);
            }
        }
        break;
    case 2: {
        s16 t = 0;
        if (timer != nullptr)
            t = De1_lowHalf(timer);
        mEvtTimer = t;
        s16 n = 0;
        if (num != nullptr)
            n = De1_lowHalf(num);
        mEvtLiftNo = n;
        break;
    }
    case 3:
        if (num != nullptr)
            setAnm_NUM(*num);
        break;
    case 4: {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x48B8, &current.pos, 0, reverb);
        mSeTimer = 0x54;
        break;
    }
    case 5: { /* player demo mode (daPy_lk_c fields; virtual at vtable +0xE4) */
        auto* p = De1_view<De1Player>(player);
        p->mDemoParam1 = 0x18;
        p->mDemoMode = 3;
        p->mDemoParam0 = 0;
        De1_playerDemo(player, 0x1D);
        break;
    }
    }
}

/* 0222BEB0 */
BOOL daNpc_De1_c::event_action() {
    switch ((u32)(s32)mEvtActNo) {
    case 0: {
        fopAc_ac_c* lift = searchByID(mPartnerId);
        if (lift != nullptr && De1_moveUpLift(lift)) {
            return TRUE;
        }
        return FALSE;
    }
    case 1:
    case 3:
    case 4:
    case 5:
        return TRUE;
    case 2:
        if (De1_calcTimer(&mEvtTimer) != 0) {
            return FALSE;
        }
        if ((u32)(s32)mEvtLiftNo < 10) {
            fopAc_ac_c* lift = searchByID(mLiftId[mEvtLiftNo]);
            if (lift != nullptr) {
                De1_view<De1LeafLift>(lift)->mMoveRequested = 1;
            }
        }
        return TRUE;
    default:
        return FALSE;
    }
}

/* 0222BF7C */
void daNpc_De1_c::privateCut() {
    s32 staffId = dComIfGp_evmng_getMyStaffId("De1" /* "De1" */, nullptr, 0);
    if (staffId == -1) {
        return;
    }
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffId, a_cut_tbl, 1, TRUE, 0);
    mActIdx = actIdx;
    dEvent_manager_c* evm = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        De1_cutEnd(evm, staffId); /* cutEnd */
        return;
    }
    if (De1_cutAdvance(evm, staffId) /* getIsAddvance */) {
        if (mActIdx == 0) {
            event_actionInit(staffId);
        }
    }
    if (mActIdx == 0 && !event_action()) {
        return;
    }
    dComIfGp_evmng_cutEnd(staffId);
}

/* 0222B950 */
void daNpc_De1_c::endEvent() {
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}

/* 0222C06C */
void daNpc_De1_c::event_proc() {
    if (De1_endNamedEvent(dComIfGp_getPEvtManager(), "LIFT_UP" /* "LIFT_UP" */)) {
        endEvent();
        mOrder = 1;
        setStt(3);
        return;
    }
    if (De1_endNamedEvent(dComIfGp_getPEvtManager(), "DE_CHUCHU" /* "DE_CHUCHU" */)) {
        dComIfGs_onTmpBit(0x308);
        endEvent();
        setStt(7);
        return;
    }
    if (De1_endNamedEvent(dComIfGp_getPEvtManager(), "contact" /* "contact" */)) {
        endEvent();
        De1ActionFunc pmf;
        De1_loadAction(&pmf, &l_wait_action2);
        set_action(&pmf, nullptr);
        return;
    }
    if (!mEventCut2.cutProc()) {
        privateCut();
    }
}

/* 0222AD00 */
BOOL daNpc_De1_c::set_action(De1ActionFunc* i_newProcFunc, void* i_argsP) {
    De1ActionFunc* cur = &mAction;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if ((u16)cur->d == (u16)newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
    }
    if (cur->i != 0) {
        mActPhase = -1;
        De1_callAction(this, cur, i_argsP);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActPhase = 0;
    De1_callAction(this, cur, i_argsP);
    return TRUE;
}

/* 0222CC98 */
BOOL daNpc_De1_c::wait01() {
    if (mTalkReq != 0) {
        setStt(2);
        return TRUE;
    }
    mOrder = mAttention != 0 ? 2 : 0;
    return TRUE;
}

/* 0222CCEC */
BOOL daNpc_De1_c::wait02() {
    if (mTalkReq != 0) {
        setStt(4);
        return TRUE;
    }
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift != nullptr && lift->current.pos.y < lift->home.pos.y + 560.0f) {
        setStt(5);
        return TRUE;
    }
    mOrder = mAttention != 0 ? 2 : 0;
    return TRUE;
}

/* 0222CD8C */
BOOL daNpc_De1_c::wait03() {
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift != nullptr && De1_isLiftDown(lift)) {
        setStt(1);
    }
    return TRUE;
}

/* 0222CDE0 */
BOOL daNpc_De1_c::wait04() {
    mOrder = 0;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz diff;
    cXyz_mi(&player->current.pos, &diff, &mDemoStartCenter);
    cXyz xz;
    De1_copyBits(&xz.x, &diff.x);
    xz.y = 0.0f;
    De1_copyBits(&xz.z, &diff.z);
    f32 len = std_sqrtf(PSVECSquareMag(&xz));
    if (len < l_HIO.mPrm.mCallDist) {
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        mOrder = 4;
        De1_view<De1Player>(pl)->mDemoAngle = cLib_targetAngleY(&pl->current.pos, &current.pos);
    }
    return TRUE;
}

/* 0222CE8C */
BOOL daNpc_De1_c::wait05() {
    if (mOrder == 5) {
        return TRUE;
    }
    mOrder = 0;
    for (int i = 0; i < 10; i++) {
        if (searchByID(mLiftId[i]) != nullptr) {
            return TRUE;
        }
    }
    mOrder = 5;
    attention_info.flags &= ~0x4000000u;
    return TRUE;
}

/* 0222CF18 */
BOOL daNpc_De1_c::talk01() {
    if (mOrder != 3) {
        mMesgAnimeTag = 0xFF;
        mTalkReset = 0;
        mTalkReq = 0;
        endEvent();
        mOrder = 3;
    }
    return TRUE;
}

/* 0222CF78 */
BOOL daNpc_De1_c::talk02() {
    talk(1);
    if (mbHasMsg != 0 && De1_messageStatus(l_message.get()) == 0x13 /* fopMsgStts_BOX_CLOSED_e */) {
        mMesgAnimeTag = 0xFF;
        setStt((u32)(s32)mPrevStt);
        mTalkReset = 0;
        mTalkReq = 0;
        endEvent();
    }
    return TRUE;
}

/* 0222CFF8 */
BOOL daNpc_De1_c::wait_action1(void*) {
    if (mActPhase == 0) {
        setStt(6);
        ccCreate();
        mActPhase = (s8)(mActPhase + 1);
        return TRUE;
    }
    if (mActPhase != -1) {
        mAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 6:
            wait04();
            break;
        case 7:
            wait05();
            break;
        }
    }
    return TRUE;
}

/* 0222D0A4 */
BOOL daNpc_De1_c::wait_action2(void*) {
    if (mActPhase == 0) {
        setStt(1);
        mActPhase = (s8)(mActPhase + 1);
        return TRUE;
    }
    if (mActPhase == -1) {
        return TRUE;
    }
    if (mActPhase == 1) {
        partner_srch();
        mActPhase = (s8)(mActPhase + 1);
    }
    mAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case 1:
        wait01();
        break;
    case 2:
        talk01();
        break;
    case 3:
        wait02();
        break;
    case 4:
        talk02();
        break;
    case 5:
        wait03();
        break;
    }
    return TRUE;
}

/* ---- compiler-generated tail ---- */
/* 0222B878 */
u8 daNpc_De1_c::demo() {
    if (demoActorID == 0) {
        if (mInDemo != 0) {
            mInDemo = 0;
            return 0;
        }
        return mInDemo;
    }
    u8 id = demoActorID;
    mInDemo = 1;

    if (id != 0 && id <= 0x20) {
        u32 obj = (u32)l_demoObject;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demoObject;
        }
        De1_demoGetActor(obj, id); /* dDemo_object_c::getActor */
    }
    De1_demoData(this, 0x6A, mpMorf.get(), "De" /* "De" */, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mInDemo;
}

/* 0222C36C */
BOOL daNpc_De1_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGd_setListBG();
    mpMorf->entryDL();
    dComIfGd_setList();
    // HD: no shadow.
    dSnap_RegistFig(0xA6 /* DSNAP_TYPE_NPC_DE1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 0222C218 */
BOOL daNpc_De1_c::_execute() {
    if (mInitDone == 0) {
        mInitAngle.y = current.angle.y;
        mInitAngle.z = current.angle.z;
        De1_copyBits(&mInitPos.y, &current.pos.y);
        De1_copyBits(&mInitPos.x, &current.pos.x);
        De1_copyBits(&mInitPos.z, &current.pos.z);
        mInitDone = 1;
        mInitAngle.x = current.angle.x;
    }
    checkOrder();
    if (!demo()) {
        if (dComIfGp_event_runCheck() != 0 && eventInfo.mCommand != dEvtCmd_INTALK_e) {
            event_proc();
        } else {
            De1_callAction(this, &mAction, nullptr);
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    eventOrder();
    setMtx();
    return TRUE;
}

/* 0222B648 */
BOOL daNpc_De1_c::_delete() {
    dComIfG_resDelete(&mPhs, "De" /* "De" */);
    dBgS* bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, mpBgW);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    del_pa_happa();
    /* l_HIO.removeHIO() */
    s32 n = (s32)l_HIO.mCount;
    if (n >= 0) {
        l_HIO.mCount = n - 1;
        if (n - 1 < 0) {
            mDoHIO_deleteChild((s8)l_HIO.mNo);
        }
    }
    return TRUE;
}

/* 0222AC80 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_De1_c*>(i_this)->CreateHeap();
}

/* 0222B4B8 */
cPhs_State daNpc_De1_c::_create() {
    /* fopAcM_ct(this, daNpc_De1_c): inline constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            De1_npcCtor(
                this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            mDeformRes = 0;
            mDeformVtx = 0;
            __vtbl = 0x10019948;
            De1_eventCutCtor(&mEventCut2); /* dNpc_EventCut_c::dNpc_EventCut_c */
            dPa_followEcallBack_ct(&mPaHappa, 0, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "De" /* "De" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!decideType((u8)fopAcM_GetParam(this) /* fopAcM_GetParam(this) & 0xFF */)) {
        return cPhs_ERROR_e;
    }
    /* l_HIO.entryHIO("デクの木") */
    if ((s32)l_HIO.mCount < 0) {
        s8 no = mDoHIO_createChild("fNÌØ", &l_HIO);
        l_HIO.mNo = no;
    }
    l_HIO.mCount = l_HIO.mCount + 1;
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, l_heapSize)) {
        return cPhs_ERROR_e;
    }
    J3DModel* model = mpMorf->getModel();
    cullMtx = model != nullptr ? gabi::ea(J3DModel_getBaseTRMtx(model)) : 0; /* fopAcM_SetMtx */
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 0222A97C */
BOOL daNpc_De1_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes("De" /* "De" */, 7);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes("De", 5);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* LOOP */, 1.0f,
                                    0, -1, 1, nullptr, 0x80000, 0x11000002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    mBranchJnt = (s8)modelData->getJointName()->getIndex("branchL" /* "branchL" */);
    if (mBranchJnt < 0)
        JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x704, "m_branchL_jnt_num >= 0");
    mHeadJnt = (s8)modelData->getJointName()->getIndex("head" /* "head" */);
    if (mHeadJnt < 0)
        JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x706, "m_head_jnt_num >= 0");

    for (int i = 0; i < 10; i++) {
        const char* name = a_lift_jnt_names[i].get();
        mLiftJnt[i] = (s8)modelData->getJointName()->getIndex(name);
        if (mLiftJnt[i] < 0)
            JUT_ASSERT_fail("d_a_npc_de1.cpp", 0x709, "m_c0_jnt_num[ i] >= 0");
    }
    mpMorf->getModel()->setUserArea(0); /* setUserArea(0) */
    /* mpBgW = new dBgWDeform() */
    u8* bgw = (u8*)operator_new(0xC4);
    if (bgw != nullptr) {
        De1_bgwCtor(bgw);
        De1_view<De1BgW>(bgw)->mVtable = 0x100196C8;
    }
    mpBgW = (dBgW*)(void*)bgw;
    if (bgw == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    void* bgData = dComIfG_getObjectIDRes("De", 8);
    if (De1_bgwSet(mpBgW, bgData, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    void* vtx = De1_objectResource("De", "de.cvtx" /* "de.cvtx" */);
    if (!De1_deformInit(&mDeformRes, vtx)) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(0.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.m_flags |= 0x40C;
    return TRUE;
}

/* 0222B634 */
static cPhs_State daNpc_De1_Create(fopAc_ac_c* i_this) {
    return static_cast<daNpc_De1_c*>(i_this)->_create();
}

/* 0222B6E4 */
static BOOL daNpc_De1_Delete(daNpc_De1_c* i_this) { return i_this->_delete(); }

/* 0222C368 */
static BOOL daNpc_De1_Execute(daNpc_De1_c* i_this) { return i_this->_execute(); }

/* 0222C424 */
static BOOL daNpc_De1_Draw(daNpc_De1_c* i_this) { return i_this->_draw(); }

/* 0222C428 */
static BOOL daNpc_De1_IsDelete(daNpc_De1_c*) { return TRUE; }

// HD: compiler-generated and template code; HIO allocation preserves the u8* signature.
/* 0222D1B8 */
static u8* daNpc_De1_HIO_c_ct(u8* i_this) {
    if (i_this == nullptr) {
        i_this = (u8*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    De1_view<daNpc_De1_HIO_c>(i_this)->__vtbl = 0x1001969C;               /* vtable */
    memcpy_g(&De1_view<daNpc_De1_HIO_c>(i_this)->mPrm, &a_hio_prm, 0x1C); /* parameters */
    De1_view<daNpc_De1_HIO_c>(i_this)->mNo = -1;                          /* mNo */
    De1_view<daNpc_De1_HIO_c>(i_this)->mCount = -1;                       /* mCount */
    return i_this;
}

/* 0222D224 */
static void __sinit_d_a_npc_de1_cpp() {
    sinit_header_statics_z(0x10466D0C, 0x101BE0AC, 0x10466D40);
    daNpc_De1_HIO_c_ct((u8*)&l_HIO); /* static daNpc_De1_HIO_c l_HIO */
}

/* 0222D2C4 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0222D2D8 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* 0222D2DC */
static void daNpc_De1_c_dt(daNpc_De1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        De1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        De1_view<De1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1001967C;
        De1_view<De1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1001968C;
        De1_acchDestroy(&i_this->mObjAcch, 0);
        De1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0222D378 */
static void de1_emptyVirtual(void*) {}

/* WWHD glue
str 10019738 "De" in 0222A97C
str 10019728 "branchL" in 0222A97C
str 10019744 "d_a_npc_de1.cpp" in 0222A97C
str 10019780 "m_branchL_jnt_num >= 0" in 0222A97C
str 1001973C "head" in 0222A97C
str 10019754 "m_head_jnt_num >= 0" in 0222A97C
str 10019768 "m_c0_jnt_num[ i] >= 0" in 0222A97C
str 10019730 "de.cvtx" in 0222A97C
str 100197C0 "De1" in 0222B300
str 100197C4 "De" in 0222B4B8
str 100197C8 "fNÌØ" in 0222B4B8
str 100197D4 "De" in 0222B648
str 100197D8 "LIFT_UP" in 0222B6E8
str 100197F0 "DE_CHUCHU" in 0222B6E8
str 100197E0 "contact" in 0222B6E8
str 100197E8 "Omori" in 0222B6E8
str 100196BC "d_demo.h" in 0222B878
str 100196AC "m_object != (0)" in 0222B878
str 100197FC "De" in 0222B878
str 10019838 "d_a_npc_de1.cpp" in 0222B990
str 10019848 "0 <= i_num && i_num < ANM_END" in 0222B990
str 10019868 "o_bck_num && o_bas_num" in 0222B990
str 10019880 "De" in 0222BAC8
str 10019888 "ActNo" in 0222BC2C
str 10019890 "Timer" in 0222BC2C
str 10019884 "Num" in 0222BC2C
str 10019898 "De1" in 0222BF7C
str 100198A4 "LIFT_UP" in 0222C06C
str 100198B4 "DE_CHUCHU" in 0222C06C
str 100198AC "contact" in 0222C06C
str 10019908 "d_a_npc_de1.cpp" in 0222CBE0
str 10019918 "m_cc_ID[ i] != fpcM_ERROR_PROCESS_ID_e" in 0222CBE0
ext 024F5A18 void De1_bgwCtor(void*)
ext 024F49E8 bool De1_bgwSet(dBgW*,void*,u32)
ext 024F5B08 void De1_copyBackVertices(dBgW*)
ext 025EDD64 BOOL De1_deformInit(void*,void*)
ext 025EDE0C void De1_deformCalc(void*,J3DModel*)
ext 025D54C4 void De1_searchById(u32,be<u32>*)
ext 024EF130 s8 De1_roomId(dBgS*,cBgS_PolyInfo*)
ext 024EEEB8 u8 De1_polyColour(dBgS*,cBgS_PolyInfo*)
ext 02588034 u32 De1_firstMessage(u16,u32,u32)
ext 025445B8 BOOL De1_startNamedEvent(dEvent_manager_c*,const char*)
ext 0254457C BOOL De1_endNamedEvent(dEvent_manager_c*,const char*)
ext 0253F124 u32 De1_eventPid(dEvt_control_c*,fopAc_ac_c*)
ext 025F795C u32 De1_messageStatus(void*)
ext 0252012C void De1_nextStage(const char*,s16,s8,s8,f32,u32,BOOL,s8)
ext 025D77DC BOOL De1_orderNamedEvent(fopAc_ac_c*,u32,u16,u16)
ext 025E18EC void De1_audioRequest(u32)
ext 021B4090 BOOL De1_moveUpLift(fopAc_ac_c*)
ext 021B430C BOOL De1_isLiftDown(fopAc_ac_c*)
ext 02055B64 s16 De1_calcTimer(be<s16>*)
ext 025A1458 void De1_npcCtor(daNpc_De1_c*)
ext 0259F740 void De1_eventCutCtor(dNpc_EventCut_c*)
ext 02526E70 void* De1_demoGetActor(u32,u8)
ext 02527028 BOOL De1_demoData(daNpc_De1_c*,u32,mDoExt_McaMorf*,const char*,s32,u32,u32,s32)
ext 02543280 void De1_cutEnd(dEvent_manager_c*,s32)
ext 025447C8 BOOL De1_cutAdvance(dEvent_manager_c*,s32)
ext 02018034 void De1_circleDestroy(void*,s32)
ext 024EFD9C void De1_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void De1_actorDestroy(daNpc_De1_c*,s32)
ext 02606900 void* De1_namedResource(dRes_control_c*,SafeString*,SafeString*)
*/
