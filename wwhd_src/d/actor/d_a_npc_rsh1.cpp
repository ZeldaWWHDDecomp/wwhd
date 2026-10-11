/**
 * d_a_npc_rsh1.cpp (WWHD)
 * NPC - Zunari (Windfall travelling merchant)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Messages (next_msgStatus, getMsg, normal/shop_talk, talk, pl_shop_out_action): d_a_npc_rsh1_msg.cpp.
 * Movement, path and actions: d_a_npc_rsh1_move.cpp.
 */
#include "d/actor/d_a_npc_rsh1.h"

const gc_tu_data gc_tu = {
 .safeString = RSH1_SAFESTRING_VTBL,
 .safeStringAssure = 0x022DBA60,
 .objAcch = {0x10021674, 0x10021694, 0x10021684},
 .objAcchDt = {0x10021684, 0x10021694},
};

static const s16 y_vals[] = {(s16)0xE800, (s16)0xE400, (s16)0xE000, (s16)0xDC00};

// HD: the message object is managed globally; only its process ID is TU-wide.
RshStatics& rsh1_statics() { return *gabi::at<RshStatics>(0x104684BC); }
be<u32>& rsh1_messageId() { return rsh1_statics().mMessageId; }
daNpc_Rsh1_HIO_c& rsh1_HIO() { return rsh1_statics().mHIO; }

static const int l_btp_ix_tbl[] = {dRes_INDEX_RSH_BTP_RS_MABA01_e, dRes_INDEX_RSH_BTP_RS_MABA01_e};
static const int l_bck_ix_tbl[] = {
 dRes_INDEX_RSH_BCK_RS_WAIT01_e, dRes_INDEX_RSH_BCK_RS_WAIT02_e,
 dRes_INDEX_RSH_BCK_RS_TALK01_e, dRes_INDEX_RSH_BCK_RS_TALK02_e,
 dRes_INDEX_RSH_BCK_RS_TALK02_e, dRes_INDEX_RSH_BCK_RS_WALK_e, dRes_INDEX_RSH_BCK_RS_PUSH_e
};
static const int l_bas_ix_tbl[] = {
 dRes_INDEX_RSH_BAS_RS_WAIT01_e, dRes_INDEX_RSH_BAS_RS_WAIT02_e,
 dRes_INDEX_RSH_BAS_RS_TALK01_e, dRes_INDEX_RSH_BAS_RS_TALK02_e,
 dRes_INDEX_RSH_BAS_RS_TALK02_e, dRes_INDEX_RSH_BAS_RS_WALK_e, dRes_INDEX_RSH_BAS_RS_PUSH_e
};
static const int play_mode_tbl[] = {2, 2, 2, 2, 2, 2, 2};
static const f32 morf_frame_tbl[] = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
static const f32 play_speed_tbl[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const cXyz shop_buyer_pos[] = {
 {1314.0f, 672.0f, -204426.0f}, {1544.0f, 672.0f, -204214.0f},
 {1775.0f, 672.0f, -204029.0f}, {1985.0f, 672.0f, -203786.0f}
};

static const char* cut_name_tbl[] = {"TALKMSG", "CONTINUE_TALK", "SETANM", "TURN"};
u32 rsh1_cutNames() { return gabi::ea(&cut_name_tbl[0]); }

/* daNpc_Rsh1_checkRotenBaseTalkArea (inlined into eventOrder in WWHD): base_talk_area_tbl is a
 * function-local static, initialised on first use (guard 0x104684C8, table 0x10468530) */
/* inlined into eventOrder */
static inline BOOL daNpc_Rsh1_checkRotenBaseTalkArea() {
    cXyz* tbl = rsh1_statics().mTalkArea;
    if (rsh1_statics().mTalkAreaGuard == 0) {
        rsh1_statics().mTalkAreaGuard = 1;
        tbl[2].x = 930.0f;
        tbl[0].y = 671.0f;
        tbl[3].y = 671.0f;
        tbl[1].y = 671.0f;
        tbl[2].y = 671.0f;
        tbl[0].z = -204393.0f;
        tbl[3].x = 971.0f;
        tbl[1].z = -204321.0f;
        tbl[2].z = -204114.0f;
        tbl[3].z = -204189.0f;
        tbl[0].x = 580.0f;
        tbl[1].x = 539.0f;
    }
    fopAc_ac_c* link_p = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    cXyz link_pos;
    cXyz temp2;
    cXyz temp3;
    cXyz cross;
    cXyz base_y;
    link_pos.x = link_p->current.pos.x;
    f32 bx = rsh1_baseY().x; /* cXyz::BaseY */
    link_pos.y = link_p->current.pos.y;
    f32 by = rsh1_baseY().y;
    link_pos.z = link_p->current.pos.z;
    f32 bz = rsh1_baseY().z;
    int j = 0, k = 1, l = 0;
    while (j < 4) {
        cXyz_mi(&tbl[j], &temp2, &link_pos);
        cXyz_mi(&tbl[k], &temp3, &link_pos);
        rsh1_outprod(&temp2, &cross, &temp3); /* cXyz::outprod */
        base_y.x = bx;
        base_y.y = by;
        base_y.z = bz;
        if (PSVECDotProduct(&base_y, &cross) > 0.0f) {
            l++;
        }
        k++;
        j++;
        if (k > 3) k = 0;
    }
    return l == 4 || l == 0;
}

/* 022D737C */
BOOL daNpc_Rsh1_c::checkCreateInShopPlayer() {
    fopAc_ac_c* link_p = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
    s16 ay = current.angle.y;
    cXyz dir;
    cXyz link_pos;
    cXyz diff;
    cXyz temp2;
    cXyz temp3;
    cXyz cross;
    cXyz base_y;
    dir.z = cM_ssin(ay);
    dir.y = 0.0f;
    dir.x = -cM_scos(ay);
    link_pos.x = link_p->current.pos.x;
    link_pos.y = link_p->current.pos.y;
    link_pos.z = link_p->current.pos.z;
    cXyz_mi(&link_pos, &diff, &current.pos);

    for (int i = 0; i < 2; i++) {
        f32 bx = rsh1_baseY().x; /* cXyz::BaseY */
        f32 by = rsh1_baseY().y;
        f32 bz = rsh1_baseY().z;
        cXyz* tbl = i == 0 ? rsh1_statics().mShopArea1 : rsh1_statics().mShopArea2;
        int j = 0, k = 1, l = 0;
        while (j < 4) {
            cXyz_mi(&tbl[j], &temp2, &link_pos);
            cXyz_mi(&tbl[k], &temp3, &link_pos);
            rsh1_outprod(&temp2, &cross, &temp3); /* cXyz::outprod */
            base_y.x = bx;
            base_y.y = by;
            base_y.z = bz;
            if (PSVECDotProduct(&base_y, &cross) > 0.0f) {
                l++;
            }
            k++;
            j++;
            if (k > 3) k = 0;
        }
        if ((l == 4 || l == 0) && PSVECDotProduct(&diff, &dir) > 0.0f && std::fabs((f32)diff.y) < 25.0f) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 022D6828 */
int daNpc_Rsh1_countShop() {
    u8 i = 0;
    int result = 0;
    for (; i <= 11; i++) {
        if (rsh1_isGetItemReserve(i)) {
            result++;
        } else if (i == 0 && dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
            result++;
        }
    }
    return result;
}

/* 022D68B8 */
BOOL daNpc_Rsh1_shopMsgCheck(u32 param_1) {
    if ((param_1 >= 0x286B && param_1 <= 0x2882) || (param_1 >= 0x2868 && param_1 <= 0x286A) || param_1 == 0x2863 ||
        param_1 == 0x2884) {
        return TRUE;
    }
    return FALSE;
}

/* 022D68F0 */
BOOL daNpc_Rsh1_shopStickMoveMsgCheck(u32 param_1) {
    if ((param_1 >= 0x286B && param_1 <= 0x2882 && (param_1 % 2 != 0)) || param_1 == 0x2863) {
        return TRUE;
    }
    return FALSE;
}

/* 022D691C */
static BOOL nodeCallBack_Rsh(J3DNode* i_node, int i_calcTiming) {
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model_p = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Rsh1_c* actor_p = static_cast<daNpc_Rsh1_c*>(model_p->getUserArea());
        J3DJoint* jnt_p = J3DNode_toJoint(i_node);
        u32 jnt_no = rsh1_jointNumber(jnt_p);
        if (actor_p) {
            PSMTXCopy(model_p->getAnmMtx(jnt_no), calc_mtx());
            if (jnt_no == (u32)(s32)actor_p->m_head_jnt_num) {
                cXyz temp1;
                cXyz temp2;
                temp1.x = 0.0f;
                temp1.y = 0.0f;
                temp1.z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-actor_p->mJntCtrl.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-actor_p->mJntCtrl.mAngles[0][0]);
                MtxPosition(&temp1, &temp2);
                f32 x = temp2.x, y = temp2.y, z = temp2.z;
                actor_p->mAttnBasePos.z = z;
                actor_p->mAttnBasePos.y = y;
                actor_p->mAttnBasePos.x = x;
                temp1.x = 28.0f;
                temp1.z = 0.0f;
                temp1.y = 20.0f;
                MtxPosition(&temp1, &temp2);
                x = temp2.x;
                z = temp2.z;
                y = temp2.y;
                actor_p->eyePos.x = x;
                actor_p->eyePos.z = z;
                actor_p->eyePos.y = y;
                if (actor_p->m76F != 0xFF) {
                    actor_p->m76F = (u8)(actor_p->m76F + 1);
                }
            } else if (jnt_no == (u32)(s32)actor_p->m_backbone_jnt_num) {
                cMtx_XrotM(calc_mtx(), actor_p->mJntCtrl.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), actor_p->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx); /* j3dSys.mCurrentMtx */
            Mtx34* anm = model_p->getAnmMtx(jnt_no);         /* setAnmMtx */
            J3DModel::mtx_assign(anm, calc_mtx());
        }
    }
    return TRUE;
}

/* 022D6BB0 */
BOOL daNpc_Rsh1_c::initTexPatternAnm(u32 i_modify) {
    J3DModelData* morf_model_data_p = J3DModel_getModelData(mpMorf->getModel());
    s32 idx = l_btp_ix_tbl[(s32)m958]; /* l_btp_ix_tbl */
    m_head_tex_pattern = (J3DAnmTexPattern*)rsh1_getRes(idx);
    if (!m_head_tex_pattern) {
        JUT_ASSERT_fail("d_a_npc_rsh1.cpp", 0x24C, "m_head_tex_pattern != (0)");
    }
    if (rsh1_btpInit(mBtpAnm, morf_model_data_p, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, i_modify, 0) == 0) {
        return FALSE;
    }
    mBtpFrame = 0;
    mTimer = 0;
    return TRUE;
}

/* 022D7C7C */
void daNpc_Rsh1_c::playTexPatternAnm() {
    if (cLib_calcTimer(&mTimer) == 0) { /* cLib_calcTimer */
        s32 max = rsh1_getFrameMax(m_head_tex_pattern);
        if ((s32)(u8)mBtpFrame >= max) {
            max = rsh1_getFrameMax(m_head_tex_pattern);
            mBtpFrame = (u8)(mBtpFrame - max);
            mTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBtpFrame = (u8)(mBtpFrame + 1);
        }
    }
}

/* 022D8B20 */
void daNpc_Rsh1_c::setAnm(s8 i_index) {
    if (i_index != m959 || m959 == -1) {
        m959 = i_index;
        s32 i = i_index;
        rsh1_dNpc_setAnm_2(mpMorf, play_mode_tbl[i] /* play_mode_tbl */,
                           morf_frame_tbl[i] /* morf_frame_tbl */,
                           play_speed_tbl[i] /* play_speed_tbl */,
                           l_bck_ix_tbl[i] /* l_bck_ix_tbl */,
                           l_bas_ix_tbl[i] /* l_bas_ix_tbl */, STR(RSH1_ARC));
    }
}

/* 022D8380 */
u32 daNpc_Rsh1_c::setTexAnm(s8 param_1) {
    if (m958 != param_1 || m958 == -1) {
        m958 = param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 022D8B7C */
void daNpc_Rsh1_c::setAnmFromMsgTag() {
    switch (dComIfGp_getMesgAnimeAttrInfo()) { /* dComIfGp_getMesgAnimeAttrInfo() */
    case 0: setAnm(0); break;
    case 1: setAnm(1); break;
    case 2: setAnm(2); break;
    case 3: setAnm(3); break;
    case 4:
        setAnm(4);
        m95A = 2;
        break;
    case 5: setAnm(5); break;
    case 6: setAnm(6); break;
    }
    if (m959 == 4) {
        mDoExt_McaMorf* m = mpMorf;
        f32 end = (f32)(s32)m->mFrameCtrl.mEnd;
        if (m->mFrameCtrl.checkPass(end - 1.0f)) {
            s8 c = (s8)(m95A - 1);
            m95A = c;
            if (c <= 0) {
                setAnm(0);
                m95A = 0;
            }
        }
    }
    dComIfGp_clearMesgAnimeAttrInfo(); /* dComIfGp_clearMesgAnimeAttrInfo() */
}

/* 022D8D44 */
bool daNpc_Rsh1_c::chkAttention(cXyz* i_pos, s16 i_angleAdjustment) {
    fopAc_ac_c* link_p = dComIfGp_getLinkPlayer();
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    f32 max_attn_dist_xz = hio.mNpcHIO.mMaxAttnDistXZ;
    int max_attn_angle_y = hio.mNpcHIO.mMaxAttnAngleY;
    cXyz pos_diff;
    cXyz xz;
    cXyz_mi(&link_p->current.pos, &pos_diff, i_pos);
    f32 dx = pos_diff.x;
    f32 dz = pos_diff.z;
    xz.x = dx;
    xz.z = dz;
    xz.y = 0.0f;
    f32 temp_abs_xz = std_sqrtf(PSVECSquareMag(&xz));
    s16 angle = cM_atan2s(dx, dz);
    if (mbAttention) {
        max_attn_dist_xz = max_attn_dist_xz + 40.0f;
        max_attn_angle_y += 0x71C;
    }
    angle = (s16)(angle - i_angleAdjustment);
    int a = angle < 0 ? -angle : angle;
    return max_attn_angle_y > a && max_attn_dist_xz > temp_abs_xz;
}

/* 022D808C */
void daNpc_Rsh1_c::eventOrder() {
    s8 m = m95B;
    if (m == 5) {
        rsh1_orderOtherEventId(this, (s32)mShopOutEventIdx, 0xFF, 0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
        return;
    }
    if (m == 4) {
        rsh1_orderOtherEvent(this, "RSH_GET_DEMO" /* "RSH_GET_DEMO" */, 1, 0xFFFF); /* fopAcM_orderOtherEvent */
        return;
    }
    if (m == 3) {
        if (mShopIdx != -1 || daNpc_Rsh1_checkRotenBaseTalkArea()) {
            eventInfo.onCondition(dEvtCnd_CANTALK_e); /* CANTALK */
        }
    } else if (m == 1 || m == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
    } else {
        return;
    }
    if (m95B == 1 || m95B == 2) {
        fopAcM_orderSpeakEvent(this);
    }
}

/* 022D7D4C */
void daNpc_Rsh1_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo command */
    if (cmd == 2 /* checkCommandDemoAccrpt */) {
        if (m95B == 5) {
            m95B = 0;
            rsh1_setAction(this, RSH1_event_action);
        } else if (m95B == 4) {
            m95B = 0;
            mShopCamAct.Reset();
            rsh1_setAction(this, RSH1_getdemo_action);
        }
    } else if (cmd == 1 /* checkCommandTalk */) {
        s8 m = m95B;
        if (m == 1 || m == 2 || m == 3) {
            fopAc_ac_c* link_p = dComIfGp_getLinkPlayer();
            s16 temp = (s16)(cLib_targetAngleY(&current.pos, &link_p->current.pos) - home.angle.y);
            if (temp > 0x1800 || temp < -0x2800) {
                m95B = 0;
                m780 = 0x2883;
                m771 = 1;
                talkInit();
                return;
            }
            if (m95B != 2) {
                mShopCamAct.shop_cam_action_init();
            }
            m95B = 0;
            m771 = 1;
            talkInit();
        }
    } else {
        mShopCamAct.Save();
    }
}

/* 022D8738 */
void daNpc_Rsh1_c::setCollision() {
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    f32 r, h;
    /* chkAction(&daNpc_Rsh1_c::pl_shop_out_action) */
    if (mCurrProc.i == -1 && mCurrProc.d == 0 && mCurrProc.f == RSH1_pl_shop_out_action) {
        r = hio.mCylR2;
        h = hio.mCylH;
        mCyl.SetC(&m794);
    } else {
        r = hio.mCylR1;
        h = hio.mCylH;
        mCyl.SetC(&current.pos);
    }
    mCyl.SetR(r);
    mCyl.SetH(h);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 022D7D40 */
void daNpc_Rsh1_c::talkInit() {
    m961 = 0;
}

/* 022D75E8 */
BOOL daNpc_Rsh1_c::CreateInit() {
    mActorAngle.x = current.angle.x;
    mActorAngle.y = current.angle.y;
    mActorAngle.z = current.angle.z;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 173; /* attention_info.distances[TYPE_BATTLE] = 173 */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 173; // HD: the GameCube out-of-range distances[8] write lands here
    gravity = -30.0f;
    if (m95E == 0) {
        rsh1_setAction(this, RSH1_wait_action);
    }
    rsh1_copy12(gabi::ea(&mAttnBasePos), gabi::ea(&current.pos));
    rsh1_statusInit(&mStts, 0xFF, 0xFF, this); /* mStts.Init */
    mCyl.Set(rsh1_cylinderSource());         /* mCyl.Set(l_cyl_src) */
    mCyl.SetStts(&mStts); /* SetStts */
    m962 = 0;
    m749 = 0;
    m780 = 0;
    mShopSelectItemNo = 0xFF;
    mShopCamAct.mCamDataIdx = 8;                 /* setCamDataIdx(8) */
    mShopCamAct.mCurrActionFunc.mThisDelta = 0; /* setCamAction(NULL) */
    mShopCamAct.mCurrActionFunc.mVtableIndex = 0;
    mShopCamAct.mCurrActionFunc.mFunction = 0;
    createShopList();
    s32 idx = mShopIdx;
    if (idx != -1) {
        RshShopItems* it = &mShopItemsArr[idx]; /* init() */
        it->mSelectedItemIdx = -1;
        it->mbIsHide = 0;
        it->m3C = 0;
    }
    mpShopItems = nullptr;
    mShopIdx = -1;
    pathGet();
    cullSizeFar = 0.25f; /* fopAcM_setCullSizeFar(this, 0.25f) */
    set_mtx();
    mpMorf->setMorf(0.0f);
    u32 zx = rsh1_words(cXyz::Zero).x, zy = rsh1_words(cXyz::Zero).y, zz = rsh1_words(cXyz::Zero).z; /* cXyz::Zero */
    rsh1_words(m794).x = zx;
    rsh1_words(m794).y = zy;
    rsh1_words(m794).z = zz;
    m793 = 0;
    rsh1_words(m7A0).y = zy;
    rsh1_words(m7A0).z = zz;
    rsh1_words(m7A0).x = zx;
    rsh1_setActorInfo(&mEventCut, "Rsh1" /* "Rsh1" */, this); /* setActorInfo */
    mEventCut.setJntCtrlPtr(&mJntCtrl);     /* setJntCtrlPtr */
    if (checkCreateInShopPlayer()) {
        m95B = 5;
        actor_status = actor_status | 0x4000;
        mShopOutEventIdx = rsh1_getEventIdx("RSH_SHOP_OUT" /* "RSH_SHOP_OUT" */);
        rsh1_setAction(this, RSH1_dummy_action);
    } else {
        m95B = 0;
    }
    return TRUE;
}

/* 022D6FDC */
void daNpc_Rsh1_c::createShopList() {
    csXyz temp;
    csXyz_ct(&temp, 0, home.angle.y, 0);

    int i, k;
    s32 n;
    for (k = 0, i = 0;; i++) {
        int j = 0;
        RshShopItems* items = &mShopItemsArr[i];
        items->mItemSetListGlobalIdx = 8; /* setItemDataIdx(8) */
        temp.y = y_vals[i];
        while (j < 3 && k < 12) {
            int idx = i * 3 + j;
            mShopItemDataPtrs[k] = 0;
            BOOL got;
            if (k != 0) {
                got = rsh1_isGetItemReserve((u8)k);
            } else {
                got = dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e) ? TRUE : FALSE;
            }
            if (got) {
                __shop_items_set_data* data = rsh1_shopItemData()[k]; /* Item_setData_rshop[k] */
                u32 itemNo = rsh1_itemData(data)->mItemNo;
                s8 room = fopAcM_GetRoomNo(this);
                u32 id = rsh1_createShopItem(gabi::ea(&rsh1_shopItemPositions()[idx]) /* Item_set_pos_data_rshop_0 */, itemNo, &temp,
                                         (s32)room, 0, 0); /* fopAcM_createShopItem */
                items->mItemActorProcessIds[j] = id;
                mShopItemDataPtrs[idx] = gabi::ea(rsh1_shopItemData()[k].get());
                j++;
            }
            k++;
        }
        items->mNumItems = (s16)j; /* setItemSum(j) */
        if (j < 3) {
            n = j == 0 ? i : i + 1;
            break;
        } else if (i == 3) {
            n = i + 1;
            break;
        }
    }
    if (0 < n) {
        m78C = n;
        for (i = 0; i < m78C; i++) {
            rsh1_setItemDataList(&mShopItemsArr[i], &mShopItemDataPtrs[i * 3]); /* setItemSetDataList */
        }
        n = m78C;
    }
    m78C = n > 1 ? n : 1;
}

/* 022D86EC */
void daNpc_Rsh1_c::setAttention() {
    f32 z = mAttnBasePos.z;
    f32 x = mAttnBasePos.x;
    f32 ofs = rsh1_HIO().mNpcHIO.mAttnYOffset;
    attention_info.position.x = x; /* attention_info.position */
    f32 y = mAttnBasePos.y;
    attention_info.position.z = z;
    f32 ey = current.pos.y;
    attention_info.position.y = y + ofs;
    f32 ex = current.pos.x;
    f32 ez = current.pos.z;
    eyePos.y = ey + 170.0f;
    eyePos.z = ez;
    eyePos.x = ex;
}

/* 022D83A8 */
void daNpc_Rsh1_c::lookBack() {
    cXyz player_eye_pos;
    cXyz* vec_p = nullptr;
    s16 temp2 = current.angle.y;
    u8 temp = 1;
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    switch (m95C) {
    case 1:
        if (mbAttention) {
            cXyz eye;
            rsh1_playerEyePos(&eye, (f32)hio.mNpcHIO.m04); /* dNpc_playerEyePos */
            rsh1_copy12(gabi::ea(&player_eye_pos), gabi::ea(&eye));
            vec_p = &player_eye_pos;
            setTexAnm(0);
        } else {
            vec_p = nullptr;
            setTexAnm(0);
        }
        break;
    case 2:
        if (mpShopItems) {
            s16 ci = mShopCamAct.mCurrActionFunc.mVtableIndex;
            bool match = ci == 0 || (ci == -1 && mShopCamAct.mCurrActionFunc.mThisDelta == 0 &&
                                     mShopCamAct.mCurrActionFunc.mFunction == RSH1_TALK_CAM_ACTION /* rsh_talk_cam_action */);
            if (match) {
                cXyz eye;
                rsh1_playerEyePos(&eye, (f32)hio.mNpcHIO.m04);
                rsh1_copy12(gabi::ea(&player_eye_pos), gabi::ea(&eye));
                temp = 0;
            } else if (mpShopItems->mSelectedItemIdx == -1) {
                /* mShopCamAct.getItemZoomPos(125.0f) (inline) */
                cXyz dir;
                cXyz ofs;
                cXyz res;
                cXyz_mi(&mShopCamAct.m18, &dir, &mShopCamAct.m24);
                if (!rsh1_normalizeRS(&dir)) { /* normalizeRS */
                    dir.x = 0.0f;
                    dir.z = 1.0f;
                    dir.y = 0.0f;
                }
                cXyz_ml(&dir, &ofs, 125.0f);           /* cXyz::operator* */
                cXyz_pl(&mShopCamAct.m24, &res, &ofs); /* cXyz::operator+ */
                f32 z = res.z, y = res.y, x = res.x;
                temp = 0;
                player_eye_pos.y = y;
                player_eye_pos.z = z;
                player_eye_pos.x = x;
            } else {
                cXyz out;
                cXyz base;
                mpShopItems->getSelectItemBasePos(&out);
                rsh1_copy12(gabi::ea(&base), gabi::ea(&out));
                mpShopItems->getSelectItemPos(&out);
                rsh1_copy12(gabi::ea(&player_eye_pos), gabi::ea(&out));
                mpShopCursor->setPos(&base);
                f32 a = hio.m34, d = hio.m40, c = hio.m3C, b = hio.m38, e = hio.m44;
                RshShopCursor* cur = mpShopCursor;
                cur->m4C = b;
                cur->m48 = a;
                cur->m50 = c;
                cur->m38 = d;
                cur->m3C = e;
                mpShopCursor->anm_play();
            }
            vec_p = &player_eye_pos;
        } else {
            vec_p = nullptr;
        }
        break;
    default:
        break;
    }
    cXyz eye_copy;
    s16 vel;
    if (mJntCtrl.mbTrn != 0) { /* trnChk */
        cLib_addCalcAngleS2(&mLookAtMaxVel, (s32)hio.mNpcHIO.mMaxHeadTurnVel, 4, 0x800); /* cLib_addCalcAngleS2 */
        eye_copy.x = eyePos.x;
        vel = mLookAtMaxVel;
        eye_copy.z = eyePos.z;
        eye_copy.y = eyePos.y;
    } else {
        eye_copy.x = eyePos.x;
        eye_copy.z = eyePos.z;
        eye_copy.y = eyePos.y;
        mLookAtMaxVel = 0;
        vel = 0;
    }
    rsh1_lookAtTarget(&mJntCtrl, &current.angle.y, vec_p, &eye_copy, (s32)temp2, (s32)vel, (u32)temp); /* lookAtTarget */
}

/* 022D71D4 */
bool daNpc_Rsh1_c::pathGet() {
    u32 param = (mParameters >> 0x10) & 0xFF;
    dPath* path = dPath_GetRoomPath(param, fopAcM_GetRoomNo(this));
    mpPath = path;
    if (path) {
        RshPathPoint* pnt = rsh1_path(path)->m_points; /* m_points */
        for (int i = 0; i < (s32)rsh1_path(path)->m_num; i++, pnt++) {
            u8 arg3 = pnt->mArg3;
            if (arg3 != 0xFF) {
                f32 y = pnt->m_position.y;
                f32 x = pnt->m_position.x;
                f32 z = pnt->m_position.z;
                mPathPointPos[arg3].x = x;
                mPathPointPos[arg3].y = y;
                mPathPointPos[arg3].z = z;
                m744[pnt->mArg3] = (s8)i;
                if (pnt->mArg3 == 1) {
                    m704 = (s8)i;
                }
            }
        }
    }
    mShopIdx = 0;
    return true;
}

/* 022D8E84 */
BOOL daNpc_Rsh1_c::shopPosMove() {
    s32 idx = mShopIdx;
    if (idx != -1) {
        f32 step = rsh1_HIO().m48;
        const cXyz& src = shop_buyer_pos[idx]; /* shop_buyer_pos */
        cXyz temp;
        temp.x = src.x;
        temp.y = src.y;
        temp.z = src.z;
        if (rsh1_chasePosXZ(&current.pos, &temp, step) != 0) { /* cLib_chasePosXZ */
            setAnm(0);
            return TRUE;
        }
        s16 target = cLib_targetAngleY(&current.pos, &temp);
        cLib_addCalcAngleS2(&current.angle.y, (s32)target, 8, 0x1000); /* cLib_addCalcAngleS2 */
        setAnm(5);
        return FALSE;
    }
    return TRUE;
}

/* 022D8A44 */
BOOL daNpc_Rsh1_c::_draw() {
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_data_p = J3DModel_getModelData(morf_model_p);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    rsh1_btpEntry(mBtpAnm, morf_model_data_p, (u32)(u8)mBtpFrame); /* mBtpAnm.entry */
    mpMorf->updateDL();
    rsh1_removeBtp(morf_model_data_p); /* mBtpAnm.remove (inline) */
    // HD: no blob shadow
    RshShopItems* items = mpShopItems;
    if (items && items->mSelectedItemIdx >= 0) {
        mpShopCursor->draw();
    }
    rsh1_registerFigure(DSNAP_TYPE_NPC_RSH1, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022D8854 */
BOOL daNpc_Rsh1_c::_execute() {
    // HD: no debug HIO switches
    RshCameraView* cam = rsh1_camera(); /* dComIfGp_getCamera(0) */
    cXyz center;
    cXyz lookat_diff;
    center.x = cam->mCenter.x; /* view.mLookat.mCenter */
    center.y = cam->mCenter.y;
    center.z = cam->mCenter.z;
    cXyz_mi(&center, &lookat_diff, &current.pos);
    f32 lookat_dist = std_sqrtf(PSVECSquareMag(&lookat_diff));
    if (lookat_dist > REG_F(10, 10) /* REG10_F(10) */ + 5000.0f) {
        return TRUE;
    }
    dNpc_HIO_c& n = rsh1_HIO().mNpcHIO;
    mJntCtrl.setParam(n.mMaxBackboneX, n.mMaxBackboneY, n.mMinBackboneX, n.mMinBackboneY, n.mMaxHeadX, n.mMaxHeadY,
                      n.mMinHeadX, n.mMinHeadY, n.mMaxTurnStep);
    playTexPatternAnm();
    u8 stop = (u8)mpMorf->play(&eyePos, 0, 0);
    mDoExt_McaMorf* morf = mpMorf;
    f32 prev = mMorfPrevFrame;
    mMorfIsStop = (s8)stop;
    if (morf->mFrameCtrl.getFrame() < prev) {
        morf = mpMorf;
        mMorfIsStop = 1;
    }
    mMorfPrevFrame = morf->mFrameCtrl.getFrame();
    checkOrder();
    rsh1_callProc(this);
    mShopCamAct.move();
    if (mpShopItems) {
        mpShopItems->Item_Move();
    }
    eventOrder();
    lookBack();
    setAttention();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = rsh1_groundRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)); /* GetRoomId(mAcch.m_gnd) */
    tevStr.mEnvrIdxOverride = rsh1_groundColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)); /* GetPolyColor */
    set_mtx();
    setCollision();
    return TRUE;
}

/* 022D7BF4 */
BOOL daNpc_Rsh1_c::_delete() {
    dComIfG_resDelete(&mPhs, STR(RSH1_ARC));
    if (heap && mpMorf) {
        mpMorf->stopZelAnime();
    }
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    s32 n = hio.m08;
    if (n >= 0) {
        n = n - 1;
        hio.m08 = n;
        if (n < 0) {
            mDoHIO_deleteChild(hio.mNo);
        }
    }
    return TRUE;
}

/* 022D6FD8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Rsh1_c*)i_this)->CreateHeap();
}

/* 022D7998 */
cPhs_State daNpc_Rsh1_c::_create() {
    /* fopAcM_ct(this, daNpc_Rsh1_c): the inline constructor (HD virtual destructor) */
    if (!(actor_condition & 8)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            rsh1_setActorVtable(this);
            rsh1_btpConstruct(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, RSH1_CYL_AAB_VTBL);
            rsh1_jointControlConstruct(&mJntCtrl);  /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            rsh1_eventCutConstruct(&mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
            rsh1_setStickVtable(&mSTControl);
            rsh1_stickWaitParm(&mSTControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000); /* setWaitParm */
            rsh1_stickInit(&mSTControl);                                        /* init */
            rsh1_shopCameraConstruct(&mShopCamAct); /* ShopCam_action_c::ShopCam_action_c */
            rsh1_constructItems(mShopItemsArr, 4, 0x44, 0x025BC760); /* __construct_array(ShopItems_c) */
        }
        actor_condition = actor_condition | 8;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(RSH1_ARC));
    if (state == cPhs_COMPLEATE_e) {
        m95E = 0; /* (param >> 20) & 0xF: every value maps to 0 */
        if (!fopAcM_entrySolidHeap(this, 0x022D6FD8 /* CheckCreateHeap */, 0x6100)) {
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
        daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
        if (hio.m08 < 0) {
            hio.mNo = mDoHIO_createChild("露店の社長" /* "露店の社長" */, &hio);
        }
        hio.m08 = hio.m08 + 1;
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return state;
}

/* 022D6CC4 */
BOOL daNpc_Rsh1_c::CreateHeap() {
    J3DModelData* model_p = (J3DModelData*)rsh1_getRes(dRes_INDEX_RSH_BDL_RS_e);
    void* anm = rsh1_getRes(dRes_INDEX_RSH_BCK_RS_WAIT01_e);
    void* bas = rsh1_getRes(dRes_INDEX_RSH_BAS_RS_WAIT01_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, model_p, nullptr, nullptr, (J3DAnmTransform*)anm, 2, 1.0f, 0, -1, 1, bas, 0,
                                    0x11020203);
    if (!mpMorf || !mpMorf->getModel()) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = rsh1_getJointIndex(model_p, "head" /* "head" */);
    if (m_head_jnt_num < 0) {
        JUT_ASSERT_fail("d_a_npc_rsh1.cpp", 0x9DB, "m_head_jnt_num >= 0");
    }
    m_backbone_jnt_num = rsh1_getJointIndex(model_p, "backbone" /* "backbone" */);
    if (m_backbone_jnt_num < 0) {
        JUT_ASSERT_fail("d_a_npc_rsh1.cpp", 0x9DE, "m_backbone_jnt_num >= 0");
    }
    if (m95E == 0) {
        m958 = 0;
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < rsh1_getJointNum(model_p); i = (u16)(i + 1)) {
        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
            u32 data = gabi::ea(J3DModel_getModelData(mpMorf->getModel()));
            rsh1_setJointCallBack(data, i, 0x022D691C /* nodeCallBack_Rsh */);
        }
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    void* cur_brk = rsh1_getRes(dRes_INDEX_RSH_BRK_SHOP_CURSOR01_e); /* GHS evaluates the second argument first */
    void* cur_mdl = rsh1_getRes(dRes_INDEX_RSH_BMD_SHOP_CURSOR01_e);
    mpShopCursor = rsh1_ShopCursor_create(cur_mdl, cur_brk, rsh1_HIO().m34);
    return mpShopCursor != nullptr ? TRUE : FALSE;
}

/* 022D729C */
void daNpc_Rsh1_c::set_mtx() {
    J3DModel* morf_model_p = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x + m7A0.x, current.pos.y + m7A0.y, current.pos.z + m7A0.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel::mtx_assign(&morf_model_p->mBaseTransformMtx, mDoMtx_stack_c::get());
}

/* 022D7BF0 */
static cPhs_State daNpc_Rsh1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Rsh1_c*)i_this)->_create();
}

/* 022D7C78 */
static BOOL daNpc_Rsh1_Delete(daNpc_Rsh1_c* i_this) {
    return i_this->_delete();
}

/* 022D8A40 */
static BOOL daNpc_Rsh1_Execute(daNpc_Rsh1_c* i_this) {
    return i_this->_execute();
}

/* 022D8B14 */
static BOOL daNpc_Rsh1_Draw(daNpc_Rsh1_c* i_this) {
    return i_this->_draw();
}

/* 022D8B18 */
static BOOL daNpc_Rsh1_IsDelete(daNpc_Rsh1_c*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 022DB690 */
static daNpc_Rsh1_HIO_c* daNpc_Rsh1_HIO_ct(daNpc_Rsh1_HIO_c* self) {
    if (self == nullptr) {
        self = (daNpc_Rsh1_HIO_c*)operator_new(0x6C);
        if (self == nullptr) return self;
    }
    self->__vtbl = RSH1_HIO_VTBL;
    rsh1_npcHioConstruct(&self->mNpcHIO); /* dNpc_HIO_c::dNpc_HIO_c */
    dNpc_HIO_c& n = self->mNpcHIO;
    n.m04 = -20.0f;
    n.mMaxHeadX = 0x200;
    n.mMaxHeadY = 0x200;
    n.mMaxBackboneX = 0x1388;
    n.mMaxBackboneY = 0x1770;
    n.mMinHeadX = -0x200;
    n.mMinHeadY = -0x200;
    n.mMinBackboneX = -5000;
    n.mMinBackboneY = -6000;
    n.mMaxTurnStep = 0x1000;
    n.mMaxHeadTurnVel = 0x800;
    n.mAttnYOffset = 80.0f;
    n.mMaxAttnAngleY = 0x4000;
    n.m22 = 0;
    n.mMaxAttnDistXZ = 400.0f;
    self->m34 = 1.0f;
    self->m38 = 0.9f;
    self->m3C = 0.5f;
    self->m40 = 40.0f;
    self->m44 = 30.0f;
    self->m48 = 7.5f;
    self->m4C = 1300.0f;
    self->mCylR1 = 100.0f;
    self->mCylR2 = 180.0f;
    self->mCylH = 190.0f;
    self->m68 = 0;
    self->m69 = 0;
    self->m6A = 0;
    self->m6B = 0;
    for (int i = 0; i < 12; i++) self->m5C[i] = 0;
    self->mNo = -1;
    self->m08 = -1;
    return self;
}

/* WWHD glue
data cut_name_tbl 101C6168
str 100218AC
str 100218B4
str 100218C4
str 100218CC
str 100217A8 in 022D75E8
data l_btp_ix_tbl 10021640
data l_bck_ix_tbl 100216C4
data l_bas_ix_tbl 100216E0
data play_mode_tbl 101C60E4
data morf_frame_tbl 101C6100
data play_speed_tbl 101C611C
data shop_buyer_pos 101C6138
str 10021728 in 022D6BB0
str 1002170C in 022D6BB0
str 10021740 in 022D6CC4
str 10021774 in 022D6CC4
str 10021760 in 022D6CC4
str 10021788 in 022D6CC4
str 10021748 in 022D6CC4
str 100217C0 in 022D7998
str 100217A0 in 022D75E8
str 100217F4 in 022D808C
*/
