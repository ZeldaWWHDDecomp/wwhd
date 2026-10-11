/**
 * d_a_npc_bs1.cpp (WWHD)
 * NPC - Beedle (boat shopkeeper)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bs1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Messages (next_msgStatus, getMsg, normal_talk, shop_talk, talk, setAnmFromMsgTag): d_a_npc_bs1_msg.cpp.
 */
#include "d/actor/d_a_npc_bs1.h"

static daNpc_Bs1_HIO_c l_HIO;
static u32 l_msgId;
static Bs1Int10 l_bck_ix_tbl;
static Bs1Int4 l_btp_ix_tbl;
static Bs1Int10 play_mode_tbl;
static Bs1Float10 morf_frame_tbl;
static Bs1Float10 play_speed_tbl;
static Bs1Byte6 l_beastIndices;
static Bs1Half7 l_beastMessages;
static Bs1Names8 cut_name_tbl;
static Bs1Names2 a_name;
static Bs1Names2 a_cut_name;
static Bs1Items6 Item_set_data3;
static Bs1Items3 Item_set_data4;
static Bs1Items3 Item_set_data5;
static Bs1Items3 Item_set_dataBs2;
static dCcD_SrcCyl l_cyl_src;
static csXyz l_angleZero;
static s16 l_tag_pay_rupee;
static u8 l_tag_buy_item_max;
static u8 l_tag_buy_item;

/* ---- local bindings (SHARED-CANDIDATE) ---- */

/* J3DModel::getAnmMtx (HD: the joint matrix block at +0x2C, marked dirty) */

/* matrix assignment through FPRs: twelve lfs, then twelve stfs */

/* 02212258 */
static s16 daNpc_Bs1_XyEventCB(void* i_this, int value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bs1_c*)i_this)->XyEventCB(value);
}

/* CreateInit: the part shared by all types (GHS duplicated it per case) */

/* 02212008 */
s16 daNpc_Bs1_c::XyEventCB(int i_itemBtn) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 eventIdx = -1;
    u8 selectedItem = dComIfGp_getSelectItem(i_itemBtn); /* dComIfGp_getSelectItem */
    if (mType == 0) {
        if (selectedItem == 0x9D /* dItemNo_COMPLIMENTARY_ID_e */) {
            eventIdx = mEventIdxs[0];
            m82B = 0;
            bs1_setAction(this, BS1_event_action);
        } else if (selectedItem == 0x9E /* dItemNo_FILL_UP_COUPON_e */) {
            eventIdx = mEventIdxs[1];
            m82B = 1;
            bs1_setAction(this, BS1_event_action);
        }
    }
    return eventIdx;
}

/* 0221225C */
static BOOL nodeCallBack_Bs(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = bs1_currentModel();                               /* j3dSys.getModel() */
        daNpc_Bs1_c* i_this = gabi::at<daNpc_Bs1_c>((u32)model->mUserArea); /* getUserArea() */
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(gabi::at<Mtx34>(bs1_getAnmMtx(model, jntNo)), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_head_jnt_num) {
                gabi::Local<cXyz> offset;
                gabi::Local<cXyz> pos;
                offset->x = 0.0f;
                offset->y = 0.0f;
                offset->z = 0.0f;
                cMtx_XrotM(calc_mtx(), i_this->mJntCtrl.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->mJntCtrl.mAngles[0][0]);
                MtxPosition(offset.get(), pos.get());
                f32 x = pos->x, y = pos->y, z = pos->z;
                i_this->m718.y = y; /* setAttentionBasePos(pos) */
                i_this->m718.z = z;
                i_this->m718.x = x;
                offset->x = 28.0f;
                offset->z = 0.0f;
                offset->y = 20.0f;
                MtxPosition(offset.get(), pos.get());
                y = pos->y;
                x = pos->x;
                z = pos->z;
                i_this->eyePos.x = x; /* setEyePos(pos) */
                i_this->eyePos.y = y;
                i_this->eyePos.z = z;
                u8 cnt = i_this->m72F; /* incAttnSetCount() */
                if (cnt != 0xFF) {
                    i_this->m72F = (u8)(cnt + 1);
                }
            } else if (jntNo == (u32)(s32)i_this->m_backbone_jnt_num) {
                cMtx_XrotM(calc_mtx(), i_this->mJntCtrl.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
            u32 anm = bs1_getAnmMtx(model, jntNo);              /* model->setAnmMtx(jntNo, *calc_mtx) */
            bs1_mtx_copy(anm, gabi::ea(calc_mtx()));
        }
    }
    return TRUE;
}

/* 02212704 */
BOOL daNpc_Bs1_c::initTexPatternAnm(u32 i_modify) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* modelData = J3DModel_getModelData(mpMorf->getModel());
    /* l_btp_ix_tbl (0x1001846C) */
    m_head_tex_pattern = (J3DAnmTexPattern*)bs1_getRes((s32)l_btp_ix_tbl.mEntries[(s32)m828]);
    if (!m_head_tex_pattern) {
        JUT_ASSERT_fail("d_a_npc_bs1.cpp", 0x1CD, "m_head_tex_pattern != (0)");
    }
    if (!bs1_btpInit(&mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, i_modify, 0)) {
        return FALSE;
    }
    mFrame = 0;
    m2C6 = 0;
    return TRUE;
}

/* 0221391C */
void daNpc_Bs1_c::playTexPatternAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (bs1_calcTimer(&m2C6) == 0) { /* cLib_calcTimer */
        s32 max = bs1_getFrameMax(m_head_tex_pattern);
        if ((s32)mFrame >= max) {
            max = bs1_getFrameMax(m_head_tex_pattern);
            mFrame = (u8)(mFrame - max);
            f32 r = cM_rndF(100.0f);
            m2C6 = (s16)gabi::ftoi(r + 30.0f);
        } else {
            mFrame = (u8)(mFrame + 1);
        }
    }
}

/* 022142F8 */
void daNpc_Bs1_c::setAnm(s8 index) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* play_mode_tbl (0x101BD584), morf_frame_tbl (0x101BD5AC), play_speed_tbl (0x101BD5D4), l_bck_ix_tbl
     * (0x10018444) */
    if (index != m829 || m829 == -1) {
        m829 = index;
        s32 i = index;
        bs1_dNpc_setAnm_2(mpMorf, (u32)play_mode_tbl.mEntries[i], (f32)morf_frame_tbl.mEntries[i],
                          (f32)play_speed_tbl.mEntries[i], (s32)l_bck_ix_tbl.mEntries[i], -1, "Bs");
    }
}

/* 02214360 */
u32 daNpc_Bs1_c::setTexAnm(s8 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m828 != value || m828 == -1) {
        m828 = value;
        return initTexPatternAnm(1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022147AC */
BOOL daNpc_Bs1_c::chkAttention(cXyz* param, s16 angle) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function()); /* the angle is not used */
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
    f32 maxAttnDistXZ = bs1_child(mType)->mNpc.mMaxAttnDistXZ;
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    cXyz_mi(&pPlayer->current.pos, sp14.get(), param);
    sp08->x = 0.0f;
    sp08->z = 1.0f;
    sp08->y = 0.0f;
    return maxAttnDistXZ > PSVECDotProduct(sp08.get(), sp14.get());
}

/* 02213D00 */
void daNpc_Bs1_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m82A == 3) {
        fopAcM_orderOtherEventId(this, m83A, 0xFF, 0xFFFF, 0, 1);
    } else if (m82A == 1 || m82A == 2) {
        eventInfo.mCondition =
            (u16)eventInfo.mCondition | dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e; /* CANTALK | CANTALKITEM */
        if (m82A == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 022139EC */
void daNpc_Bs1_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 cmd = (u16)eventInfo.mCommand; /* eventInfo command */
    if (cmd == 2 /* checkCommandDemoAccrpt */) {
        if (m82A == 3) {
            m82A = 0;
            bs1_setAction(this, BS1_getdemo_action);
        } else if (m82A == 4) {
            m82A = 0;
            bs1_setAction(this, BS1_event_action);
        }
    } else if (cmd == 1 /* checkCommandTalk */) {
        if (m82A == 1 || m82A == 2) {
            m82A = 0;
            m731 = 1;
            talkInit();
            u8 talkXY = (u8)bs1_play()->mEvtCtrl.mTalkButton; /* dComIfGp_event_chkTalkXY */
            if ((u32)(talkXY - 1) > 3) {
                gabi::Local<cXyz> pos;
                pos->x = 0.0f;
                pos->z = 125.0f;
                pos->y = 0.0f;
                mShopCamAction.shop_cam_action_init();
                fopAc_ac_c* player = dComIfGp_getPlayer(0);
                /* daPy_py_c::setPlayerPosAndAngle (virtual, HD slot 0x114) */
                bs1_setPlayerPosAndAngle(player, pos.get(), (s32)-0x6000);
            }
        }
    } else {
        mShopCamAction.Save();
    }
}

/* 022124F0 */
static u32 daNpc_Bs1_getBuyItemMax(int i_itemCost, int i_itemNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 item = (u32)i_itemNo;
    dSv_player_c* playerSave = bs1_savePlayer();
    s32 beastNum;
    /* HD: the beast index is read from a byte table (the GameCube switch) */
    if (item == 0x1F /* dItemNo_BOKOBABA_SEED_e */) {
        beastNum = (u8)playerSave->mBagItemRecord.mBeastNum[7];
    } else if (item >= 0x45 && item <= 0x47) {
        beastNum = (u8)playerSave->mBagItemRecord.mBeastNum[(u8)l_beastIndices.mEntries[item - 0x45]];
    } else if (item >= 0x49 && item <= 0x4B) {
        beastNum = (u8)playerSave->mBagItemRecord.mBeastNum[(u8)l_beastIndices.mEntries[item - 0x46]];
    } else {
        beastNum = (u8)playerSave->mBagItemRecord.mBeastNum[3];
    }
    u8 wallet = (u8)playerSave->mPlayerStatusA.mWalletSize;
    s32 currRupee = (u16)playerSave->mPlayerStatusA.mRupee; /* dComIfGs_getRupee() */
    s32 maxRupees;                                          /* dComIfGs_getRupeeMax() */
    if (wallet < 1) {
        maxRupees = 500;
    } else if (wallet == 1) {
        maxRupees = 1000;
    } else {
        maxRupees = 5000;
    }
    s32 r4 = maxRupees - currRupee;
    s32 r5 = (s32)ppc_divw((u32)r4, (u32)i_itemCost);
    if (r4 - r5 * i_itemCost != 0) {
        r5 += 1;
    }
    return beastNum >= r5 ? r5 : beastNum; /* cLib_maxLimit */
}

/* 02213D5C */
void daNpc_Bs1_c::setCollision() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> out;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = -16.0f;
    MtxTrans(current.pos.x, current.pos.y, current.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), m726.y);
    MtxPosition(offset.get(), out.get());
    mCyl.SetC(out.get());
    mCyl.SetR(46.0f);
    mCyl.SetH(130.0f);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 022139E0 */
void daNpc_Bs1_c::talkInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m835 = 0;
}

/* 022160EC */
BOOL daNpc_Bs1_c::shopMsgCheck(u32 msgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType == 0) {
        if ((0xF42 <= msgNo && msgNo <= 0xF54) || (0xF67 <= msgNo && msgNo <= 0xF6E) ||
            (0xF63 <= msgNo && msgNo <= 0xF66) || msgNo == 0xF3E) {
            return TRUE;
        }
    } else {
        if ((0x2F4A <= msgNo && msgNo <= 0x2F53) || (0x2F6B <= msgNo && msgNo <= 0x2F78) || msgNo == 0x2F47) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 02212700 */
static u32 daNpc_Bs1_getDefaultMsgCB(void* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bs1_c*)i_this)->getDefaultMsg();
}

/* 02212620 */
u32 daNpc_Bs1_c::getDefaultMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 msgNo;
    if (mType == 0) {
        u8 points = dComIfGs_getEventReg(0x86FF);
        if (bs1_ShopItems_isSoldOutItemAll(&mShopItems)) {
            msgNo = 0xF5E;
        } else if (points >= 60) {
            msgNo = 0xF60;
        } else if (points != 0) {
            msgNo = 0xF5F;
        } else {
            msgNo = 0xF3F;
        }
    } else if (!dComIfGs_isEventBit(0x1F08)) {
        msgNo = 0x2F48;
    } else if (!dComIfGs_isEventBit(0x2108)) {
        dComIfGs_onEventBit(0x2108);
        msgNo = 0x2F54;
    } else {
        msgNo = 0x2F55;
    }
    return msgNo;
}

/* 02216064 */
BOOL daNpc_Bs1_c::shopStickMoveMsgCheck(u32 msgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mType == 0) {
        if ((0xF42 <= msgNo && msgNo <= 0xF44) || ((0xF67 <= msgNo && msgNo <= 0xF6E) && (msgNo & 1)) ||
            (0xF63 <= msgNo && msgNo <= 0xF66) || msgNo == 0xF3E) {
            return TRUE;
        }
    } else {
        if ((0x2F4A <= msgNo && msgNo <= 0x2F4C) || ((0x2F72 <= msgNo && msgNo <= 0x2F76) && !(msgNo & 1)) ||
            msgNo == 0x2F78 || msgNo == 0x2F6B || msgNo == 0x2F47) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 02212BA0 */
void daNpc_Bs1_c::createShopList() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* Item_set_data3 (0x101BD548), Item_set_data4 (0x101BD560), Item_set_data5 (0x101BD56C),
     * Item_set_dataBs2 (0x101BD578); shopItems_setData_Bomb30Bs2 0x101EB8C8, arrow30Bs2 0x101EB8DC,
     * red_bottleBs2 0x101EB8F0 */
    gabi::Local<csXyz> angle; /* csXyz::Zero (0x101FFB14) */
    u16 ax = (u16)l_angleZero.x, az = (u16)l_angleZero.z, ay = (u16)l_angleZero.y;
    angle->z = (s16)az;
    angle->x = (s16)ax;
    angle->y = (s16)ay;
    if (mType == 0) {
        be<u32>* pDataSet;
        switch ((u32)(s32)mShopIndex) {
        case 4:
            pDataSet = Item_set_data4.mEntries;
            break;
        case 5:
            pDataSet = Item_set_data5.mEntries;
            break;
        default:
            pDataSet = Item_set_data3.mEntries;
            break;
        }
        for (int i = 0; i < 3; i++) {
            u8 itemNo = bs1_setDataItemNo((u32)pDataSet[i]);
            int idx = i;
            if ((itemNo == 0x2C /* dItemNo_BAIT_BAG_e */ && bs1_checkGetItem(0x2C)) ||
                (itemNo == 0x83 /* dItemNo_HYOI_PEAR_e */ &&
                 bs1_checkGetItem(0x31 /* dItemNo_BOMB_BAG_e */))) {
                itemNo = bs1_setDataItemNo((u32)pDataSet[(i + 3)]);
                idx += 3;
            }
            mShopItems.mItemActorProcessIds[i] =
                bs1_createShopItem(bs1_posTbl(this) + 12 * i, itemNo, angle.get(), current.roomNo);
            mpItemSetList[i] = (u32)pDataSet[idx];
        }
    } else {
        gabi::Local<be<u32>[5]> dataSet; /* GameCube dataSet[4]; the fifth slot takes the (mock-only)
                                            overflow, as the frame padding does */
        be<u32>* ds = *dataSet.get();
        int index = 0;
        if (bs1_checkGetItem(0x31)) {
            ds[index] = 0x101EB8C8;
            index = 1;
        }
        if ((u8)bs1_savePlayer()->mPlayerItem.mItems[12] != 0xFF) { /* dComIfGs_getItem(dInvSlot_BOW_e) */
            ds[index] = 0x101EB8DC;
            index++;
        }
        ds[index] = 0x101EB8F0;
        index++;
        if (!bs1_checkGetItem(0x31)) {
            ds[index] = 0x101EB8C8;
            index++;
        }
        if ((u8)bs1_savePlayer()->mPlayerItem.mItems[12] == 0xFF) {
            ds[index] = 0x101EB8DC;
            index++;
        }
        /* HD: an assertion that at most four entries were written */
        if (index > 3) {
            JUT_ASSERT_fail("d_a_npc_bs1.cpp", 0x749, "change_count <= 3");
        }
        int dataIdx = 0;
        for (int i = 0; i < 3; i++) {
            u8 itemNo = bs1_setDataItemNo((u32)Item_set_dataBs2.mEntries[i]);
            if ((i == 0 && dComIfGs_isEventBit(0x2020)) || (i == 1 && dComIfGs_isEventBit(0x2010)) ||
                (i == 2 && dComIfGs_isEventBit(0x2008))) {
                u32 d = (u32)ds[dataIdx];
                itemNo = bs1_setDataItemNo(d);
                mpItemSetList[i] = d;
                dataIdx++;
            } else {
                mpItemSetList[i] = (u32)Item_set_dataBs2.mEntries[i];
            }
            mShopItems.mItemActorProcessIds[i] =
                bs1_createShopItem(bs1_posTbl(this) + 12 * i, itemNo, angle.get(), current.roomNo);
        }
    }
    mShopItems.mNumItems = 3; /* setItemSum(3) */
    bs1_ShopItems_setItemSetDataList(&mShopItems, mpItemSetList);
    for (int i = 0; i < 3; i++) {
        mShopItems.mSelectedItemIdx = (s16)i;
        if ((!bs1_checkGetItem(0x31) && bs1_isBomb(mShopItems.getSelectItemNo())) ||
            ((u8)bs1_savePlayer()->mPlayerItem.mItems[12] == 0xFF &&
             bs1_isArrow(mShopItems.getSelectItemNo()))) {
            bs1_ShopItems_SoldOutItem(&mShopItems, i);
            m76C[i] = 1;
        } else {
            m76C[i] = 0;
        }
        cXyz* src = gabi::at<cXyz>(bs1_posTbl(this) + 12 * i);
        f32 x = (f32)src->x;
        mItemPosOffsets[i].x = x;
        f32 y = (f32)src->y;
        mItemPosOffsets[i].y = y;
        f32 z = (f32)src->z;
        mItemPosOffsets[i].z = z;
    }
    mShopItems.mSelectedItemIdx = -1;
}

/* 0221572C */
BOOL daNpc_Bs1_c::isSellBomb() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (int index = 0; index < 3; index++) {
        if (bs1_isBomb(bs1_ShopItems_getItemNo(&mShopItems, index)) &&
            mShopItems.mItemIsSoldOut[(s16)index] != 1) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 02213060 */
BOOL daNpc_Bs1_c::CreateInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    m726.x = current.angle.x;
    m726.y = current.angle.y;
    m726.z = current.angle.z;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e |
                           fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA; /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    switch ((u32)(s32)mType) {
    case 0:
        bs1_setAction(this, BS1_wait_action);
        m83A = dComIfGp_evmng_getEventIdx("BS1_GETDEMO" /* "BS1_GETDEMO" */, 0xFF);
        bs1_setActorInfo(&mEventCut, "Bs1" /* "Bs1" */, this); /* setActorInfo */
        mEventCut.mpJntCtrl = &mJntCtrl;                       /* setJntCtrlPtr */
        break;
    case 1:
        bs1_setAction(this, BS1_wait_action);
        m83A = dComIfGp_evmng_getEventIdx("BS2_GETDEMO" /* "BS2_GETDEMO" */, 0xFF);
        bs1_setActorInfo(&mEventCut, "Bs2" /* "Bs2" */, this);
        mEventCut.mpJntCtrl = &mJntCtrl;
        break;
    }
    bs1_CreateInit_common(this);
    s32 idx;
    if (mShopIndex != -1) {
        if (mType == 0) {
            s32 v = mShopIndex + 2; /* cLib_minMaxLimit<int>(mShopIndex + 2, 3, 5) */
            if (v < 3) {
                v = 3;
            } else if (v > 5) {
                v = 5;
            }
            idx = (s8)v;
        } else {
            idx = 6;
        }
        /* cLib_minMaxLimit<s8>(mShopIndex, 0, 7) */
        if (idx < 0) {
            idx = 0;
        } else if (idx > 7) {
            idx = 7;
        }
        mShopIndex = (s8)idx;
    } else {
        idx = 2;
    }
    /* mShopCamAction.setCamAction(NULL); setCamDataIdx; mShopItems.setItemDataIdx */
    mShopCamAction.mCurrActionFunc.mFunction = 0;
    mShopCamAction.mCurrActionFunc.mThisDelta = 0;
    mShopCamAction.mCamDataIdx = (s16)idx;
    mShopCamAction.mCurrActionFunc.mVtableIndex = 0;
    mShopItems.mItemSetListGlobalIdx = (s16)idx;
    createShopList();
    m82B = 2;
    mEventIdxs[0] = dComIfGp_evmng_getEventIdx("PUT_PRAICE_TICKET" /* "PUT_PRAICE_TICKET" */, 0xFF);
    mEventIdxs[1] = dComIfGp_evmng_getEventIdx("PUT_FULL_TICKET" /* "PUT_FULL_TICKET" */, 0xFF);
    return TRUE;
}

/* 02216368 */
void daNpc_Bs1_c::setAttention(bool shouldSet) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!shouldSet && m72F >= 2) {
        return;
    }
    f32 y = m718.y;
    f32 z = m718.z;
    f32 x = m718.x;
    f32 off = bs1_child(mType)->mNpc.mAttnYOffset;
    attention_info.position.z = z; /* attention_info.position */
    attention_info.position.x = x;
    attention_info.position.y = y + off;
}

/* ---- local bindings for the event / talk code (SHARED-CANDIDATE) ---- */
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0 messageSet */

/* 022163B0 */
void daNpc_Bs1_c::lookBack() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> sp74;
    f32 x = 0.0f, y = 0.0f, z = 0.0f; /* sp68 */
    cXyz* target = nullptr;
    s16 desiredYRot = current.angle.y;
    switch ((u32)(s32)m830) {
    case 1:
        if (m730 != 0) {
            gabi::Local<cXyz> eye;
            bs1_playerEyePos(eye.get(), (f32)bs1_child(mType)->mNpc.m04); /* dNpc_playerEyePos */
            z = current.pos.z;
            u32 ey = (u32)bs1_words(eye.get())->y;
            u32 ex = (u32)bs1_words(eye.get())->x;
            f32 eyeY = eyePos.y;
            bs1_words(sp74.get())->y = ey;
            bs1_words(sp74.get())->x = ex;
            target = sp74.get();
            u32 ez = (u32)bs1_words(eye.get())->z;
            x = current.pos.x;
            y = eyeY - 80.0f;
            bs1_words(sp74.get())->z = ez;
            setTexAnm(2);
        } else {
            setTexAnm(1);
            target = nullptr;
        }
        break;
    case 2:
        if ((s16)mShopCamAction.mCurrActionFunc.mVtableIndex == 0) { /* mShopCamAction.checkCamAction(NULL) */
            gabi::Local<cXyz> eye;
            bs1_playerEyePos(eye.get(), (f32)bs1_child(mType)->mNpc.m04); /* dNpc_playerEyePos */
            y = eyePos.y;
            u32 ez = (u32)bs1_words(eye.get())->z;
            u32 ex = (u32)bs1_words(eye.get())->x;
            bs1_words(sp74.get())->z = ez;
            bs1_words(sp74.get())->x = ex;
            z = current.pos.z;
            u32 ey = (u32)bs1_words(eye.get())->y;
            x = current.pos.x;
            target = sp74.get();
            bs1_words(sp74.get())->y = ey;
        } else if (mShopItems.mSelectedItemIdx == -1) {
            /* mShopCamAction.getItemZoomPos(100.0f) */
            gabi::Local<cXyz> dir;
            gabi::Local<cXyz> scaled;
            gabi::Local<cXyz> res;
            cXyz_mi(&mShopCamAction.m18, dir.get(), &mShopCamAction.m24);
            if (!bs1_normalizeRS(dir.get())) { /* cXyz::normalizeRS */
                dir->x = 0.0f;
                dir->y = 0.0f;
                dir->z = 1.0f;
            }
            cXyz_ml(dir.get(), scaled.get(), 100.0f);
            cXyz_pl(&mShopCamAction.m24, res.get(), scaled.get());
            f32 ry = res->y;
            x = current.pos.x;
            sp74->y = ry;
            f32 rx = res->x;
            f32 rz = res->z;
            sp74->x = rx;
            sp74->z = rz;
            z = current.pos.z;
            target = sp74.get();
            y = eyePos.y;
        } else {
            gabi::Local<cXyz> basePos;
            gabi::Local<cXyz> itemPos;
            mShopItems.getSelectItemBasePos(basePos.get());
            mShopItems.getSelectItemPos(itemPos.get());
            u32 iz = (u32)bs1_words(itemPos.get())->z;
            u32 iy = (u32)bs1_words(itemPos.get())->y;
            bs1_words(sp74.get())->z = iz;
            bs1_words(sp74.get())->y = iy;
            u32 ix = (u32)bs1_words(itemPos.get())->x;
            bs1_words(sp74.get())->x = ix;
            mpShopCursor->setPos(basePos.get());
            daNpc_Bs1_childHIO_c* c = bs1_child(mType);
            Bs1ShopCursor* cur = mpShopCursor;
            f32 s3c = c->m3C, s34 = c->m34, s38 = c->m38, s30 = c->m30, s40 = c->m40;
            cur->m48 = s30; /* setScale */
            cur->m4C = s34;
            cur->m50 = s38;
            cur->m38 = s3c;
            cur->m3C = s40;
            mpShopCursor->anm_play();
            x = current.pos.x;
            y = eyePos.y;
            z = current.pos.z;
            target = sp74.get();
        }
        break;
    }
    s16 vel;
    if (mJntCtrl.mbTrn != 0) { /* trnChk() */
        cLib_addCalcAngleS2(&m724, bs1_child(mType)->mNpc.mMaxHeadTurnVel, 4, 0x800);
        vel = m724;
    } else {
        m724 = 0;
        vel = 0;
    }
    gabi::Local<cXyz> sp68;
    sp68->x = x;
    sp68->y = y;
    sp68->z = z;
    bs1_lookAtTarget(&mJntCtrl, &current.angle.y, target, sp68.get(), (s32)desiredYRot, (s32)vel,
                     1); /* lookAtTarget */
}

/* 02216748 */
bool daNpc_Bs1_c::wait01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (m731 != 0) {
        s8 old = m830;
        m830 = 2;
        m831 = (u8)old;
    } else if (m730 != 0) {
        m82A = 2;
    } else if (m82A == 2) {
        m82A = 0;
    }
    return bs1_isMorf(mpMorf);
}

/* 022167F0 */
bool daNpc_Bs1_c::talk01() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 status = talk();
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        m830 = (s8)m831;
        dComIfGp_event_reset();
        m731 = 0;
        setAnm(0);
        bs1_offPlayerNoDraw(player);
        mShopCamAction.Reset();
    } else if (status == 8 && shopMsgCheck(m738)) {
        /* HD: the result is not used; the GameCube checkBeastItemSellMsg branch is gone */
        mShopItems.getSelectItemBuyMsg();
    }
    /* HD: the CHOOSE / CANCEL button prompts follow the cursor's visibility */
    if (mpShopCursor->m54 != 0) {
        dComIfGp_setDoStatusForce(0x17); /* dComIfGp_setDoStatusForce(dActStts_CHOOSE_e) */
        dComIfGp_setAStatusForce(0x27);  /* dComIfGp_setAStatusForce(dActStts_CANCEL_e) */
    }
    return bs1_isMorf(mpMorf);
}

/* 02217258 */
BOOL daNpc_Bs1_c::wait_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == ACTION_STARTING) {
        m830 = 1;
        m731 = 0;
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        s16 val = (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]);
        gabi::Local<cXyz> pos;
        f32 px = current.pos.x, py = current.pos.y, pz = current.pos.z;
        pos->x = px;
        pos->y = py;
        pos->z = pz;
        m730 = (u8)chkAttention(pos.get(), val);
        bool setAttn;
        switch ((u32)(s32)m830) {
        case 1:
            setAttn = wait01();
            break;
        case 2:
            setAttn = talk01();
            break;
        default:
            setAttn = false;
            break;
        }
        lookBack();
        setAttention(setAttn);
    }
    return TRUE;
}

/* 02217368 */
BOOL daNpc_Bs1_c::getdemo_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* a_name (0x101BD61C): "Bs1", "Bs2"; a_cut_name (0x101BD624): "dummy1", "dummy2" */
    u32 name = (u32)a_name.mEntries[(s32)mType];
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(name), nullptr, 0);
    bs1_getMyActIdx(dComIfGp_getPEvtManager(), staffIdx, gabi::ea(&a_cut_name), 2, 0,
                    0); /* getMyActIdx (unused) */
    if (mActionStatus == ACTION_STARTING) {
        bs1_offPlayerNoDraw(dComIfGp_getPlayer(0));
        m830 = (s8)m831;
        mShopCamAction.Reset();
        u8 itemNo = mShopItems.getSelectItemNo();
        u32 itemPID = bs1_createPresent(&current.pos, (u32)itemNo, 0, -1, (s32)(s8)current.roomNo, 0,
                                        0); /* fopAcM_createItemForPresentDemo */
        if (itemPID != 0xFFFFFFFF) {
            dComIfGp_event_setItemPartnerId(itemPID); /* dComIfGp_event_setItemPartnerId */
        }
        dComIfGp_evmng_cutEnd(staffIdx);
        mShopItems.mSelectedItemIdx = -1;
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        bs1_demoMsgFlagOn();
        dComIfGp_evmng_cutEnd(staffIdx);
        if (dComIfGp_evmng_endCheck(m83A)) {
            m82A = 1;
            if (mType == 0) {
                if (dComIfGs_getEventReg(0x86FF) != 0) {
                    m740 = 0xF4C;
                } else {
                    m740 = 0xF4E;
                }
            } else {
                m740 = 0x2F53;
            }
            dComIfGp_event_reset();
            bs1_setAction(this, BS1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}

/* ---- joint-table helpers (SHARED-CANDIDATE, same as d_a_npc_bms1.h) ---- */

/* 022168EC */
BOOL daNpc_Bs1_c::evn_talk_init(int actorId) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, "MsgNo" /* "MsgNo" */, 3);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, "EndMsgNo" /* "EndMsgNo" */, 3);
    l_msgId = 0xFFFFFFFF; /* HD: no l_msg */
    m738 = pMsgNo != nullptr ? (u32)*pMsgNo : 0;
    m744 = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}

/* 02216994 */
BOOL daNpc_Bs1_c::evn_continue_talk_init(int actorId) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, "EndMsgNo" /* "EndMsgNo" */, 3);
    m744 = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}

/* 022169F8 */
BOOL daNpc_Bs1_c::evn_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 mgr = bs1_msgMgr(); /* HD: the message manager replaces l_msg */
    if (l_msgId == 0xFFFFFFFF) {
        u32 id = bs1_msgSet(mgr, m738, &eyePos);
        l_msgId = id;
        if (id != 0xFFFFFFFF) {
            bs1_demoMsgFlagOn();
        }
    } else {
        setAnmFromMsgTag();
        if (bs1_msgGetStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
            /* next_msgStatus's result is passed on in the full register */
            bs1_msgSetStatus(mgr, bs1_nextMessageStatus(this, &m738, (u32)0));
            if (bs1_msgGetStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                bs1_msgSet(mgr, m738, nullptr);
            }
        } else if (bs1_msgGetStatus(mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            bs1_msgSetStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            l_msgId = 0xFFFFFFFF;
            return TRUE;
        } else if ((bs1_msgGetStatus(mgr) == 2 || bs1_msgGetStatus(mgr) == 6) && m738 == m744) {
            m744 = 0;
            return TRUE;
        }
    }
    return FALSE;
}

/* 02216B68 */
BOOL daNpc_Bs1_c::evn_jnt_lock_init(int actorIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* substance = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorIdx, "prm" /* "prm" */, 3);
    u32 jnt_to_lock = substance != nullptr ? (u32)*substance : 0;
    switch (jnt_to_lock) {
    case 0:
        mJntCtrl.mbHeadLock = 0;
        mJntCtrl.mbBackBoneLock = 0;
        break;
    case 1:
        mJntCtrl.mbHeadLock = 1;
        mJntCtrl.mbBackBoneLock = 0;
        break;
    case 2:
        mJntCtrl.mbHeadLock = 0;
        mJntCtrl.mbBackBoneLock = 1;
        break;
    case 3:
        mJntCtrl.mbHeadLock = 1;
        mJntCtrl.mbBackBoneLock = 1;
        break;
    }
    return TRUE;
}

/* 02216C18 */
BOOL daNpc_Bs1_c::evn_wait_init(int actorIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    void* pTimer = dComIfGp_evmng_getMySubstanceP(actorIdx, "Timer" /* "Timer" */, 3);
    m63E = pTimer != nullptr ? bs1_TimerSubstance(pTimer) : (s16)0;
    return TRUE;
}

/* 02216C80 */
BOOL daNpc_Bs1_c::evn_wait() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return bs1_calcTimer(&m63E) == 0 ? TRUE : FALSE; /* cLib_calcTimer */
}

/* 02216CAC */
BOOL daNpc_Bs1_c::evn_set_anm_init(int actorIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    be<u32>* pAnmNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorIdx, "AnmNo" /* "AnmNo" */, 3);
    u32 anmNo = pAnmNo != nullptr ? (u32)*pAnmNo : 0;
    setAnm((s8)anmNo);
    return TRUE;
}

/* 02216D1C */
BOOL daNpc_Bs1_c::evn_praise_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dSv_player_c* playerSave = bs1_savePlayer();
    s16 life = (s16)((u16)playerSave->mPlayerStatusA.mMaxLife -
                     (u16)playerSave->mPlayerStatusA.mLife); /* getMaxLife - getLife */
    f32 f = (f32)(s32)life;
    /* HD: dComIfGp_setItemLifeCount adds to a float */
    dComIfGp_setItemLifeCount(f);
    return TRUE;
}

/* 02216DA0 */
BOOL daNpc_Bs1_c::evn_mantan_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dSv_player_c* playerSave = bs1_savePlayer();
    s16 life = (s16)((u16)playerSave->mPlayerStatusA.mMaxLife - (u16)playerSave->mPlayerStatusA.mLife);
    u32 magic = (u8)playerSave->mPlayerStatusA.mMaxMagic -
                (u8)playerSave->mPlayerStatusA.mMagic; /* getMaxMagic - getMagic */
    u32 bombs = (u8)playerSave->mItemMax.mItemMax2.mBombNum -
                (u8)playerSave->mItemRecord.mItemRecord2.mBombNum; /* getBombMax - getBombNum */
    u32 arrows = (u8)playerSave->mItemMax.mItemMax2.mArrowNum -
                 (u8)playerSave->mItemRecord.mItemRecord2.mArrowNum; /* getArrowMax - getArrowNum */
    f32 f = (f32)(s32)life;
    dComIfGp_setItemLifeCount(f);
    dComIfGp_setItemMagicCount((s16)magic);
    dComIfGp_setItemBombNumCount((s16)bombs);
    dComIfGp_setItemArrowNumCount((s16)arrows);
    return TRUE;
}

/* 02216E90 */
BOOL daNpc_Bs1_c::privateCut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* cut_name_tbl (0x101BD5FC): TALKMSG, CONTINUE_TALK, JNTLOCK, WAIT, SETANM, PRAISE, MANTAN, GETTICKET */
    s32 staffId = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
    if (staffId == -1) {
        return FALSE;
    }
    s32 actIdx = bs1_getMyActIdx(dComIfGp_getPEvtManager(), staffId, gabi::ea(&cut_name_tbl), 8, 1, 0);
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffId);
    } else {
        if (dComIfGp_evmng_getIsAddvance(staffId)) {
            switch (actIdx) {
            case 0:
                evn_talk_init(staffId);
                break;
            case 1:
                evn_continue_talk_init(staffId);
                break;
            case 2:
                evn_jnt_lock_init(staffId);
                break;
            case 3:
                evn_wait_init(staffId);
                break;
            case 4:
                evn_set_anm_init(staffId);
                break;
            case 5:
                evn_praise_init();
                break;
            case 6:
                evn_mantan_init();
                break;
            case 7: /* dComIfGs_setReserveItemEmpty() */
                dComIfGs_setReserveItemEmpty();
                break;
            }
        }
        BOOL end;
        switch ((u32)actIdx) {
        case 0:
        case 1:
            end = evn_talk();
            break;
        case 3:
            end = evn_wait();
            break;
        default:
            end = TRUE;
            break;
        }
        if (end) {
            dComIfGp_evmng_cutEnd(staffId);
        }
    }
    return TRUE;
}

/* 0221709C */
BOOL daNpc_Bs1_c::event_action(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mActionStatus == ACTION_STARTING) {
        dComIfGp_evmng_getMyStaffId("Bs1" /* "Bs1" */, nullptr, 0);
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        privateCut();
        if (dComIfGp_evmng_endCheck(mEventIdxs[m82B])) {
            m82A = 0;
            m82B = 2;
            dComIfGp_event_reset();
            bs1_setAction(this, BS1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}

/* 02214104 */
BOOL daNpc_Bs1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, pModelData, mFrame);
    J3DModel* pHelmetModel = mpHelmetModel;
    setLightTevColorType(dKy_getEnvlight(), pHelmetModel, &tevStr);
    mpMorf->updateDL();
    if (bs1_child(mType)->m2C != 0) {
        PSMTXCopy(gabi::at<Mtx34>(bs1_getAnmMtx(pModel, m_head_jnt_num)), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(4.0f, 0.0f, 0.0f);
        J3DModel_setBaseTRMtx(pHelmetModel, mDoMtx_stack_c::get());
        mDoExt_modelUpdateDL(pHelmetModel, 0);
    }
    reinterpret_cast<Bs1ModelMaterialAnimation*>(pModelData)->mpTexturePattern =
        0; /* mBtpAnm.remove(pModelData) */
    /* HD: no blob shadow */
    if (mShopItems.mSelectedItemIdx >= 0) {
        mpShopCursor->draw();
    }
    for (int i = 0; i < 3; i++) {
        if (m76C[i] != 0) {
            setLightTevColorType(dKy_getEnvlight(), mpSoldSignModels[i], &tevStr);
            mDoExt_modelUpdateDL(mpSoldSignModels[i], 0);
        }
    }
    bs1_registerFigure(0x80 /* DSNAP_TYPE_NPC_BS1 */, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f,
                       1.0f); /* dSnap_RegistFig */
    return TRUE;
}

/* ---- shop bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs_checkGetItem */

/* __shop_items_set_data::mpItemData->mItemNo */

/* Item_set_pos_data_tbl[mShopIndex] (pointer table 0x101EB974) */

/* 02213E18 */
BOOL daNpc_Bs1_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    daNpc_Bs1_childHIO_c* c = bs1_child(mType);
    mJntCtrl.setParam(c->mNpc.mMaxBackboneX, c->mNpc.mMaxBackboneY, c->mNpc.mMinBackboneX,
                      c->mNpc.mMinBackboneY, c->mNpc.mMaxHeadX, c->mNpc.mMaxHeadY, c->mNpc.mMinHeadX,
                      c->mNpc.mMinHeadY, c->mNpc.mMaxTurnStep);
    playTexPatternAnm();
    m72E = (u8)mpMorf->play(&eyePos, 0, 0);
    f32 prev = m734;
    if (mpMorf->getFrame() < prev) {
        m72E = 1;
    }
    m734 = mpMorf->getFrame();
    checkOrder();
    bs1_pmf_call(this, &mCurrActionFunc, 0); /* (this->*mCurrActionFunc)(NULL) */
    mShopCamAction.move();
    mShopItems.Item_Move();
    eventOrder();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = (s8)bs1_getRoomId(dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8); /* dBgS::GetRoomId */
    u8 color = (u8)bs1_getPolyColor(dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8);    /* dBgS::GetPolyColor */
    J3DModel* model = mpMorf->getModel();
    tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    for (int i = 0; i < 3; i++) {
        if (m76C[i] != 0) {
            mDoMtx_stack_c::transS(mItemPosOffsets[i].x, mItemPosOffsets[i].y, mItemPosOffsets[i].z);
            J3DModel_setBaseTRMtx(mpSoldSignModels[i], mDoMtx_stack_c::get());
        }
    }
    setCollision();
    return TRUE;
}

/* 02213894 */
BOOL daNpc_Bs1_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_resDelete(&mPhase, "Bs" /* "Bs" */);
    if (heap && mpMorf) {
        mpMorf->stopZelAnime();
    }
    daNpc_Bs1_HIO_c* hio = bs1_HIO();
    s32 n = hio->m8;
    if (n >= 0) {
        n = n - 1;
        hio->m8 = n;
        if (n < 0) {
            mDoHIO_deleteChild(hio->mNo);
        }
    }
    return TRUE;
}

/* 02212B9C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bs1_c*)i_this)->CreateHeap();
}

/* 02213610 */
cPhs_State daNpc_Bs1_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* fopAcM_ct(this, daNpc_Bs1_c): the HD constructor, inline */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (gabi::ea(this) != 0) { /* skipped for NULL (GHS constructor form) */
            fopAc_ac_c_ct(this);
            __vtbl = BS1_VTBL;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, {0x100183E4, 0x10018404, 0x100183F4});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x100183D4);
            bs1_eventCutConstruct(&mEventCut);    /* dNpc_EventCut_c::dNpc_EventCut_c */
            bs1_jointControlConstruct(&mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            mStickControl.__vtbl = 0x10050788;
            bs1_stickSetWaitParm(&mStickControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0,
                                 0x2000);          /* STControl::setWaitParm */
            bs1_stickInit(&mStickControl);         /* STControl::init */
            bs1_shopCamConstruct(&mShopCamAction); /* ShopCam_action_c::ShopCam_action_c */
            bs1_shopItemsConstruct(&mShopItems);   /* ShopItems_c::ShopItems_c */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, "Bs" /* "Bs" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        s8 type = (s8)((mParameters >> 20) & 0xF);
        if (type == 1) {
            mType = 1;
        } else {
            mType = 0;
        }
        if (!fopAcM_entrySolidHeap(this, 0x02212B9C /* CheckCreateHeap */, 0)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        daNpc_Bs1_HIO_c* hio = bs1_HIO();
        if (hio->m8 < 0) {
            hio->mNo = mDoHIO_createChild("\203{\201|\203g\203V\203\207\203b\203v\223X\210\365", hio);
        }
        hio->m8 = hio->m8 + 1;
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return phase_state;
}

/* 02212818 */
BOOL daNpc_Bs1_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* modelData = (J3DModelData*)bs1_getRes2(0x13);
    J3DAnmTransform* bck = (J3DAnmTransform*)bs1_getRes2(0xC);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1,
                                                  1, nullptr, 0, 0x11020203);
    if (morf != nullptr) {
        mpMorf = morf;
    }
    if (morf == nullptr || morf->getModel() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = bs1_getJointIndex(modelData, "head");
    if (m_head_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bs1.cpp", 0xAD0, "m_head_jnt_num >= 0");
    m_backbone_jnt_num = bs1_getJointIndex(modelData, "backbone1");
    if (m_backbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bs1.cpp", 0xAD2, "m_backbone_jnt_num >= 0");
    if ((u32)(s32)mType <= 1) {
        m828 = 1;
    }
    if (!initTexPatternAnm(0)) {
        return FALSE;
    }
    mpHelmetModel = mDoExt_J3DModel__create((J3DModelData*)bs1_getRes2(0x14), 0, 0x11020203);
    if (mpHelmetModel.get() == nullptr) {
        return FALSE;
    }
    for (int i = 0; i < 3; i++) {
        mpSoldSignModels[i] = mDoExt_J3DModel__create((J3DModelData*)bs1_getRes2(0x23), 0, 0x11020203);
        if (mpSoldSignModels[i].get() == nullptr) {
            return FALSE;
        }
    }
    for (u16 jntNo = 0; jntNo < bs1_getJointNum(modelData); jntNo = (u16)(jntNo + 1)) {
        if (jntNo == (u32)(s32)m_head_jnt_num || jntNo == (u32)(s32)m_backbone_jnt_num) {
            bs1_setJointCallBack(gabi::ea(mpMorf->getModel()->mModelData.get()), jntNo,
                                 0x0221225C /* nodeCallBack_Bs */);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    void* brk = bs1_getRes2(0x1A);
    J3DModelData* cursorData = (J3DModelData*)bs1_getRes2(0x17);
    mpShopCursor = bs1_createCursor(cursorData, brk, (f32)bs1_child(mType)->m30); /* ShopCursor_create */
    if (mpShopCursor.get() == nullptr) {
        return FALSE;
    }
    return TRUE;
}

/* 02213890 */
static cPhs_State daNpc_Bs1_Create(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bs1_c*)i_this)->_create();
}

/* 02213918 */
static BOOL daNpc_Bs1_Delete(daNpc_Bs1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_delete();
}

/* 02214100 */
static BOOL daNpc_Bs1_Execute(daNpc_Bs1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_execute();
}

/* 022142EC */
static BOOL daNpc_Bs1_Draw(daNpc_Bs1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_draw();
}

/* 022142F0 */
static BOOL daNpc_Bs1_IsDelete(daNpc_Bs1_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 0221763C */
static daNpc_Bs1_childHIO_c* daNpc_Bs1_childHIO_ct(daNpc_Bs1_childHIO_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr) {
        self = (daNpc_Bs1_childHIO_c*)operator_new(0x48);
        if (self == nullptr)
            return self;
    }
    self->__vtbl = 0x10018424;
    bs1_constructHIO(gabi::ea(self) + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    /* HD: the members are zeroed */
    self->m2C = 0;
    self->m40 = 0.0f;
    self->m30 = 0.0f;
    self->m3C = 0.0f;
    self->m34 = 0.0f;
    self->m38 = 0.0f;
    return self;
}

/* 022176B4 */
static daNpc_Bs1_HIO_c* daNpc_Bs1_HIO_ct(daNpc_Bs1_HIO_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr) {
        self = (daNpc_Bs1_HIO_c*)operator_new(0x9C);
        if (self == nullptr)
            return self;
    }
    self->__vtbl = 0x10018434;
    bs1_constructChildren(self->mChild, 2, 0x48, 0x0221763C); /* __construct_array */
    for (int i = 0; i < 2; i++) {
        daNpc_Bs1_childHIO_c* c = &self->mChild[i];
        c->mNpc.m04 = -64.0f;
        c->mNpc.mMaxHeadX = 0x1FFE;
        c->mNpc.mMaxHeadY = 0x38E0;
        c->mNpc.mMaxBackboneX = 0;
        c->mNpc.mMaxBackboneY = 0x1C70;
        c->mNpc.mMinHeadX = -0x1FFE;
        c->mNpc.mMinHeadY = -0x38E0;
        c->mNpc.mMinBackboneX = 0;
        c->mNpc.mMinBackboneY = -0x1C70;
        c->mNpc.mMaxTurnStep = 0x1000;
        c->mNpc.mMaxHeadTurnVel = 0x800;
        c->mNpc.mAttnYOffset = 55.0f;
        c->mNpc.m22 = 0;
        c->mNpc.mMaxAttnDistXZ = 300.0f;
        c->m2C = (u8)i;
        c->m30 = 0.65f;
        c->m34 = 0.9f;
        c->m38 = 0.5f;
        c->m3C = 27.0f;
        c->m40 = 20.0f;
    }
    self->mNo = -1;
    self->m8 = -1;
    return self;
}

/* 0221782C */
static void __sinit_d_a_npc_bs1_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(0x1046685C, 0x101BD62C);
    daNpc_Bs1_HIO_ct(bs1_HIO()); /* l_HIO */
}

/* 022178CC */
static void SafeString_dtor(SafeString* self, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self != nullptr && (flags & 1))
        operator_delete(self);
}

/* 02217988 */
static void SafeString_assureTermination(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 022178E0 */
static void daNpc_Bs1_dtor(daNpc_Bs1_c* self, u32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self == nullptr)
        return;
    bs1_destroyStick(&self->mStickControl, 2); /* STControl::~STControl */
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    bs1_destroyCircle(gabi::ea(&self->mAcchCir) + 0x14, 2);               /* cM3dGCir::~cM3dGCir */
    reinterpret_cast<Bs1AcchVtables*>(&self->mAcch)->mActor = 0x100183F4; /* dBgS_ObjAcch vtables (this TU) */
    reinterpret_cast<Bs1AcchVtables*>(&self->mAcch)->mCheck = 0x10018404;
    bs1_destroyAcch(&self->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    bs1_destroyActor(self, 0);        /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

// clang-format off
/* WWHD glue
str 100185C4 "Bs" in 022142F8
str 100184CC "head" in 02212818
str 10018510 "backbone1" in 02212818
str 100184C8 "Bs" in 02212818
data l_HIO 10466878
data l_msgId 10466858
data l_bck_ix_tbl 10018444
data l_btp_ix_tbl 1001846C
data play_mode_tbl 101BD584
data morf_frame_tbl 101BD5AC
data play_speed_tbl 101BD5D4
data l_beastIndices 10018488
data l_beastMessages 100185D4
data cut_name_tbl 101BD5FC
data a_name 101BD61C
data a_cut_name 101BD624
data Item_set_data3 101BD548
data Item_set_data4 101BD560
data Item_set_data5 101BD56C
data Item_set_dataBs2 101BD578
data l_cyl_src 101BD504
data l_angleZero 101FFB14
data l_tag_pay_rupee 101D5F3A
data l_tag_buy_item_max 101D5F47
data l_tag_buy_item 101D5F48
str 10018494 "Bs"
str 10018498 "d_a_npc_bs1.cpp" in 02212704
str 100184A8 "m_head_tex_pattern != (0)" in 02212704
str 100185F8 "MsgNo" in 022168EC
str 10018600 "EndMsgNo" in 022168EC
str 1001860C "EndMsgNo" in 02216994
str 10018618 "prm" in 02216B68
str 1001861C "Timer" in 02216C18
str 10018624 "AnmNo" in 02216CAC
str 10018678 "Bs1" in 0221709C
str 100184C8 "Bs" in 02217368
str 100184D4 "d_a_npc_bs1.cpp" in 02212818
str 100184FC "m_head_jnt_num >= 0" in 02212818
str 100184E4 "m_backbone_jnt_num >= 0" in 02212818
str 10018590 "Bs" in 02213610
str 10018594 "\203{\201|\203g\203V\203\207\203b\203v\223X\210\365" in 02213610
str 100185A8 "Bs" in 02213894
str 1001851C "d_a_npc_bs1.cpp" in 02212BA0
str 1001852C "change_count <= 3" in 02212BA0
str 1001855C "BS1_GETDEMO" in 02213060
str 10018544 "Bs1" in 02213060
str 10018568 "BS2_GETDEMO" in 02213060
str 10018548 "Bs2" in 02213060
str 10018574 "PUT_PRAICE_TICKET" in 02213060
str 1001854C "PUT_FULL_TICKET" in 02213060
*/
// clang-format on
