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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8 bs1_ShopItems_getItemNo(ShopItems_c_l* items, s32 idx) { return gabi::call<u8>(0x025BD248, items, idx); }
static inline BOOL bs1_isBomb(u8 item) { return gabi::call<BOOL>(0x02550FC4, item); }
static inline BOOL bs1_ShopItems_isSoldOutItemAll(ShopItems_c_l* items) { return gabi::call<BOOL>(0x025BCF44, items); }
static inline u8 bs1_getEventReg(u16 reg) {
    return gabi::call<u8>(0x025B8BB0, gabi::load<u32>(0x101F84DC) + 0x644, (u32)reg);
}
static inline BOOL bs1_isEventBit(u16 f) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
static inline void bs1_onEventBit(u16 f) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
static inline void* bs1_getRes(s32 idx) { return dComIfG_getObjectRes(STR(0x10018494) /* "Bs" */, idx, BS1_SAFESTRING_VTBL); }
static inline s32 bs1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end,
                              u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 bs1_getFrameMax(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 4) + 0x14), p);
}
static inline void bs1_dNpc_setAnm_2(mDoExt_McaMorf* m, u32 loop, f32 morf, f32 speed, s32 anm, s32 snd, u32 arc) {
    gabi::call(0x0259D79C, m, loop, morf, speed, anm, snd, arc);
}
/* J3DModel::getAnmMtx (HD: the joint matrix block at +0x2C, marked dirty) */
static inline u32 bs1_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* matrix assignment through FPRs: twelve lfs, then twelve stfs */
static inline void bs1_mtx_copy(u32 dst, u32 src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(dst + 4 * i, t[i]);
}

/* 02212008: daNpc_Bs1_c::XyEventCB (unnamed by the matcher) */
s16 daNpc_Bs1_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x02212008, s16, this, i_itemBtn);
    s16 eventIdx = -1;
    u8 selectedItem = gabi::load<u8>(dComIfGp_ea() + 0x5BBB + i_itemBtn); /* dComIfGp_getSelectItem */
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
VERIFY(0x02212008, &daNpc_Bs1_c::XyEventCB);

/* 0221225C */
static BOOL nodeCallBack_Bs(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0221225C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Bs1_c* i_this = gabi::at<daNpc_Bs1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
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
            u32 anm = bs1_getAnmMtx(model, jntNo); /* model->setAnmMtx(jntNo, *calc_mtx) */
            bs1_mtx_copy(anm, gabi::ea(calc_mtx()));
        }
    }
    return TRUE;
}
VERIFY(0x0221225C, nodeCallBack_Bs);

/* 022124F0 */
static u32 daNpc_Bs1_getBuyItemMax(int i_itemCost, int i_itemNo) {
    WWHD_FUNC(0x022124F0, u32, i_itemCost, i_itemNo);
    u32 item = (u32)i_itemNo;
    u32 save = gabi::load<u32>(0x101F84DC);
    s32 beastNum;
    /* HD: the beast index is read from a byte table (the GameCube switch) */
    if (item == 0x1F /* dItemNo_BOKOBABA_SEED_e */) {
        beastNum = gabi::load<u8>(save + 0xBC + 7);
    } else if (item >= 0x45 && item <= 0x47) {
        beastNum = gabi::load<u8>(save + 0xBC + gabi::load<u8>(0x10018443 + item));
    } else if (item >= 0x49 && item <= 0x4B) {
        beastNum = gabi::load<u8>(save + 0xBC + gabi::load<u8>(0x10018442 + item));
    } else {
        beastNum = gabi::load<u8>(save + 0xBC + 3);
    }
    u8 wallet = gabi::load<u8>(save + 0x32);
    s32 currRupee = gabi::load<u16>(save + 0x24); /* dComIfGs_getRupee() */
    s32 maxRupees; /* dComIfGs_getRupeeMax() */
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
VERIFY(0x022124F0, daNpc_Bs1_getBuyItemMax);

/* 02212620 */
u32 daNpc_Bs1_c::getDefaultMsg() {
    WWHD_FUNC(0x02212620, u32, this);
    u32 msgNo;
    if (mType == 0) {
        u8 points = bs1_getEventReg(0x86FF);
        if (bs1_ShopItems_isSoldOutItemAll(&mShopItems)) {
            msgNo = 0xF5E;
        } else if (points >= 60) {
            msgNo = 0xF60;
        } else if (points != 0) {
            msgNo = 0xF5F;
        } else {
            msgNo = 0xF3F;
        }
    } else if (!bs1_isEventBit(0x1F08)) {
        msgNo = 0x2F48;
    } else if (!bs1_isEventBit(0x2108)) {
        bs1_onEventBit(0x2108);
        msgNo = 0x2F54;
    } else {
        msgNo = 0x2F55;
    }
    return msgNo;
}
VERIFY(0x02212620, &daNpc_Bs1_c::getDefaultMsg);

/* 02212700 (unnamed by the matcher) */
static u32 daNpc_Bs1_getDefaultMsgCB(void* i_this) {
    WWHD_FUNC(0x02212700, u32, i_this);
    return ((daNpc_Bs1_c*)i_this)->getDefaultMsg();
}
VERIFY(0x02212700, daNpc_Bs1_getDefaultMsgCB);

/* 02212704 */
BOOL daNpc_Bs1_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x02212704, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData(mpMorf->getModel());
    /* l_btp_ix_tbl (0x1001846C) */
    m_head_tex_pattern = (J3DAnmTexPattern*)bs1_getRes(gabi::load<s32>(0x1001846C + 4 * (s32)m828));
    if (!m_head_tex_pattern) {
        JUT_ASSERT_fail(STR(0x10018498), 0x1CD, STR(0x100184A8));
    }
    if (!bs1_btpInit(mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, i_modify, 0)) {
        return FALSE;
    }
    mFrame = 0;
    m2C6 = 0;
    return TRUE;
}
VERIFY(0x02212704, &daNpc_Bs1_c::initTexPatternAnm);

/* 02212B9C (unnamed by the matcher) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02212B9C, BOOL, i_this);
    return ((daNpc_Bs1_c*)i_this)->CreateHeap();
}
VERIFY(0x02212B9C, CheckCreateHeap);

/* 02213890 */
static cPhs_State daNpc_Bs1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02213890, cPhs_State, i_this);
    return ((daNpc_Bs1_c*)i_this)->_create();
}
VERIFY(0x02213890, daNpc_Bs1_Create);

/* 02213918 */
static BOOL daNpc_Bs1_Delete(daNpc_Bs1_c* i_this) {
    WWHD_FUNC(0x02213918, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02213918, daNpc_Bs1_Delete);

/* 0221391C */
void daNpc_Bs1_c::playTexPatternAnm() {
    WWHD_FUNC(0x0221391C, void, this);
    if (gabi::call<s16>(0x02055B64, &m2C6) == 0) { /* cLib_calcTimer */
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
VERIFY(0x0221391C, &daNpc_Bs1_c::playTexPatternAnm);

/* 022139E0 (unnamed by the matcher) */
void daNpc_Bs1_c::talkInit() {
    WWHD_FUNC(0x022139E0, void, this);
    m835 = 0;
}
VERIFY(0x022139E0, &daNpc_Bs1_c::talkInit);

/* 022139EC */
void daNpc_Bs1_c::checkOrder() {
    WWHD_FUNC(0x022139EC, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo command */
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
            u8 talkXY = gabi::load<u8>(dComIfGp_ea() + 0x52B0); /* dComIfGp_event_chkTalkXY */
            if ((u32)(talkXY - 1) > 3) {
                gabi::Local<cXyz> pos;
                pos->x = 0.0f;
                pos->z = 125.0f;
                pos->y = 0.0f;
                mShopCamAction.shop_cam_action_init();
                fopAc_ac_c* player = dComIfGp_getPlayer(0);
                /* daPy_py_c::setPlayerPosAndAngle (virtual, HD slot 0x114) */
                gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(player) + 0xB4) + 0x114), player, pos.get(), (s32)-0x6000);
            }
        }
    } else {
        mShopCamAction.Save();
    }
}
VERIFY(0x022139EC, &daNpc_Bs1_c::checkOrder);

/* 02213D00 (unnamed by the matcher) */
void daNpc_Bs1_c::eventOrder() {
    WWHD_FUNC(0x02213D00, void, this);
    if (m82A == 3) {
        fopAcM_orderOtherEventId(this, m83A, 0xFF, 0xFFFF, 0, 1);
    } else if (m82A == 1 || m82A == 2) {
        u32 p = gabi::ea(this) + 0xFA;
        gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | 0x21)); /* CANTALK | CANTALKITEM */
        if (m82A == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x02213D00, &daNpc_Bs1_c::eventOrder);

/* 02213D5C */
void daNpc_Bs1_c::setCollision() {
    WWHD_FUNC(0x02213D5C, void, this);
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
VERIFY(0x02213D5C, &daNpc_Bs1_c::setCollision);

/* 02214100 */
static BOOL daNpc_Bs1_Execute(daNpc_Bs1_c* i_this) {
    WWHD_FUNC(0x02214100, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02214100, daNpc_Bs1_Execute);

/* 022142EC */
static BOOL daNpc_Bs1_Draw(daNpc_Bs1_c* i_this) {
    WWHD_FUNC(0x022142EC, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022142EC, daNpc_Bs1_Draw);

/* 022142F0 */
static BOOL daNpc_Bs1_IsDelete(daNpc_Bs1_c*) {
    WWHD_FUNC(0x022142F0, BOOL, (daNpc_Bs1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022142F0, daNpc_Bs1_IsDelete);

/* 022142F8 */
void daNpc_Bs1_c::setAnm(s8 index) {
    WWHD_FUNC(0x022142F8, void, this, index);
    /* play_mode_tbl (0x101BD584), morf_frame_tbl (0x101BD5AC), play_speed_tbl (0x101BD5D4), l_bck_ix_tbl (0x10018444) */
    if (index != m829 || m829 == -1) {
        m829 = index;
        s32 i = index;
        bs1_dNpc_setAnm_2(mpMorf, gabi::load<u32>(0x101BD584 + 4 * i), gabi::load<f32>(0x101BD5AC + 4 * i),
                          gabi::load<f32>(0x101BD5D4 + 4 * i), gabi::load<s32>(0x10018444 + 4 * i), -1, 0x100185C4 /* "Bs" */);
    }
}
VERIFY(0x022142F8, &daNpc_Bs1_c::setAnm);

/* 02214360 */
u32 daNpc_Bs1_c::setTexAnm(s8 value) {
    WWHD_FUNC(0x02214360, u32, this, value);
    if (m828 != value || m828 == -1) {
        m828 = value;
        return initTexPatternAnm(1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x02214360, &daNpc_Bs1_c::setTexAnm);

/* 022147AC */
BOOL daNpc_Bs1_c::chkAttention(cXyz* param, s16 angle) {
    WWHD_FUNC(0x022147AC, BOOL, this, param, angle); /* the angle is not used */
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
    f32 maxAttnDistXZ = bs1_child(mType)->mMaxAttnDistXZ;
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    cXyz_mi(&pPlayer->current.pos, sp14.get(), param);
    sp08->x = 0.0f;
    sp08->z = 1.0f;
    sp08->y = 0.0f;
    return maxAttnDistXZ > PSVECDotProduct(sp08.get(), sp14.get());
}
VERIFY(0x022147AC, &daNpc_Bs1_c::chkAttention);

/* 0221572C */
BOOL daNpc_Bs1_c::isSellBomb() {
    WWHD_FUNC(0x0221572C, BOOL, this);
    for (int index = 0; index < 3; index++) {
        if (bs1_isBomb(bs1_ShopItems_getItemNo(&mShopItems, index)) && mShopItems.mItemIsSoldOut[(s16)index] != 1) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0221572C, &daNpc_Bs1_c::isSellBomb);

/* 02216064: the matcher names it shopMsgCheck; it is the GameCube shopStickMoveMsgCheck */
BOOL daNpc_Bs1_c::shopStickMoveMsgCheck(u32 msgNo) {
    WWHD_FUNC(0x02216064, BOOL, this, msgNo);
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
VERIFY(0x02216064, &daNpc_Bs1_c::shopStickMoveMsgCheck);

/* 022160EC: the matcher names it shopStickMoveMsgCheck; it is the GameCube shopMsgCheck */
BOOL daNpc_Bs1_c::shopMsgCheck(u32 msgNo) {
    WWHD_FUNC(0x022160EC, BOOL, this, msgNo);
    if (mType == 0) {
        if ((0xF42 <= msgNo && msgNo <= 0xF54) || (0xF67 <= msgNo && msgNo <= 0xF6E) || (0xF63 <= msgNo && msgNo <= 0xF66) ||
            msgNo == 0xF3E) {
            return TRUE;
        }
    } else {
        if ((0x2F4A <= msgNo && msgNo <= 0x2F53) || (0x2F6B <= msgNo && msgNo <= 0x2F78) || msgNo == 0x2F47) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022160EC, &daNpc_Bs1_c::shopMsgCheck);

/* 02216368 */
void daNpc_Bs1_c::setAttention(bool shouldSet) {
    WWHD_FUNC(0x02216368, void, this, shouldSet);
    if (!shouldSet && m72F >= 2) {
        return;
    }
    f32 y = m718.y;
    f32 z = m718.z;
    f32 x = m718.x;
    f32 off = bs1_child(mType)->mAttnYOffset;
    gabi::store<f32>(gabi::ea(this) + 0x398, z); /* attention_info.position */
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    gabi::store<f32>(gabi::ea(this) + 0x394, y + off);
}
VERIFY(0x02216368, &daNpc_Bs1_c::setAttention);

/* ---- local bindings for the event / talk code (SHARED-CANDIDATE) ---- */
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0 messageSet */
static inline u32 bs1_msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 bs1_msgGetStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
static inline void bs1_msgSetStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 bs1_msgSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
static inline void bs1_demoMsgFlagOn() { gabi::call(0x025DB58C); }
#define bs1_l_msgId (*gabi::at<be<u32>>(0x10466858))
static inline void bs1_event_reset() {
    u32 a = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
}
static inline void bs1_offPlayerNoDraw(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
}
static inline bool bs1_isMorf(mDoExt_McaMorf* m) { return gabi::load<f32>(gabi::ea(m) + 0xB0) < 1.0f; }

/* 022163B0 */
void daNpc_Bs1_c::lookBack() {
    WWHD_FUNC(0x022163B0, void, this);
    gabi::Local<cXyz> sp74;
    f32 x = 0.0f, y = 0.0f, z = 0.0f; /* sp68 */
    cXyz* target = nullptr;
    s16 desiredYRot = current.angle.y;
    switch ((u32)(s32)m830) {
    case 1:
        if (m730 != 0) {
            gabi::Local<cXyz> eye;
            gabi::call(0x0259D54C, eye.get(), (f32)bs1_child(mType)->m04); /* dNpc_playerEyePos */
            z = current.pos.z;
            u32 ey = gabi::load<u32>(gabi::ea(eye.get()) + 4);
            u32 ex = gabi::load<u32>(gabi::ea(eye.get()) + 0);
            f32 eyeY = eyePos.y;
            gabi::store<u32>(gabi::ea(sp74.get()) + 4, ey);
            gabi::store<u32>(gabi::ea(sp74.get()) + 0, ex);
            target = sp74.get();
            u32 ez = gabi::load<u32>(gabi::ea(eye.get()) + 8);
            x = current.pos.x;
            y = eyeY - 80.0f;
            gabi::store<u32>(gabi::ea(sp74.get()) + 8, ez);
            setTexAnm(2);
        } else {
            setTexAnm(1);
            target = nullptr;
        }
        break;
    case 2:
        if (gabi::load<s16>(gabi::ea(&mShopCamAction) + 2) == 0) { /* mShopCamAction.checkCamAction(NULL) */
            gabi::Local<cXyz> eye;
            gabi::call(0x0259D54C, eye.get(), (f32)bs1_child(mType)->m04); /* dNpc_playerEyePos */
            y = eyePos.y;
            u32 ez = gabi::load<u32>(gabi::ea(eye.get()) + 8);
            u32 ex = gabi::load<u32>(gabi::ea(eye.get()) + 0);
            gabi::store<u32>(gabi::ea(sp74.get()) + 8, ez);
            gabi::store<u32>(gabi::ea(sp74.get()) + 0, ex);
            z = current.pos.z;
            u32 ey = gabi::load<u32>(gabi::ea(eye.get()) + 4);
            x = current.pos.x;
            target = sp74.get();
            gabi::store<u32>(gabi::ea(sp74.get()) + 4, ey);
        } else if (mShopItems.mSelectedItemIdx == -1) {
            /* mShopCamAction.getItemZoomPos(100.0f) */
            gabi::Local<cXyz> dir;
            gabi::Local<cXyz> scaled;
            gabi::Local<cXyz> res;
            cXyz_mi(&mShopCamAction.m18, dir.get(), &mShopCamAction.m24);
            if (!gabi::call<BOOL>(0x0201B47C, dir.get())) { /* cXyz::normalizeRS */
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
            u32 iz = gabi::load<u32>(gabi::ea(itemPos.get()) + 8);
            u32 iy = gabi::load<u32>(gabi::ea(itemPos.get()) + 4);
            gabi::store<u32>(gabi::ea(sp74.get()) + 8, iz);
            gabi::store<u32>(gabi::ea(sp74.get()) + 4, iy);
            u32 ix = gabi::load<u32>(gabi::ea(itemPos.get()) + 0);
            gabi::store<u32>(gabi::ea(sp74.get()) + 0, ix);
            mpShopCursor->setPos(basePos.get());
            daNpc_Bs1_childHIO_c* c = bs1_child(mType);
            ShopCursor_c_l* cur = mpShopCursor;
            f32 s3c = c->m3C, s34 = c->m34, s38 = c->m38, s30 = c->m30, s40 = c->m40;
            cur->mA8 = s30; /* setScale */
            cur->mAC = s34;
            cur->mB0 = s38;
            cur->m98 = s3c;
            cur->m9C = s40;
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
        cLib_addCalcAngleS2(&m724, bs1_child(mType)->mMaxHeadTurnVel, 4, 0x800);
        vel = m724;
    } else {
        m724 = 0;
        vel = 0;
    }
    gabi::Local<cXyz> sp68;
    sp68->x = x;
    sp68->y = y;
    sp68->z = z;
    gabi::call(0x0259DED0, &mJntCtrl, &current.angle.y, target, sp68.get(), (s32)desiredYRot, (s32)vel, 1); /* lookAtTarget */
}
VERIFY(0x022163B0, &daNpc_Bs1_c::lookBack);

/* 02216748 */
bool daNpc_Bs1_c::wait01() {
    WWHD_FUNC(0x02216748, bool, this);
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
VERIFY(0x02216748, &daNpc_Bs1_c::wait01);

/* 022167F0 */
bool daNpc_Bs1_c::talk01() {
    WWHD_FUNC(0x022167F0, bool, this);
    u16 status = talk();
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        m830 = (s8)m831;
        bs1_event_reset();
        m731 = 0;
        setAnm(0);
        bs1_offPlayerNoDraw(player);
        mShopCamAction.Reset();
    } else if (status == 8 && shopMsgCheck(m738)) {
        /* HD: the result is not used; the GameCube checkBeastItemSellMsg branch is gone */
        mShopItems.getSelectItemBuyMsg();
    }
    /* HD: the CHOOSE / CANCEL button prompts follow the cursor's visibility */
    if (mpShopCursor->mbShow != 0) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BBA, 0x17); /* dComIfGp_setDoStatusForce(dActStts_CHOOSE_e) */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB9, 0x27); /* dComIfGp_setAStatusForce(dActStts_CANCEL_e) */
    }
    return bs1_isMorf(mpMorf);
}
VERIFY(0x022167F0, &daNpc_Bs1_c::talk01);

/* 022168EC */
BOOL daNpc_Bs1_c::evn_talk_init(int actorId) {
    WWHD_FUNC(0x022168EC, BOOL, this, actorId);
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, STR(0x100185F8) /* "MsgNo" */, 3);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, STR(0x10018600) /* "EndMsgNo" */, 3);
    bs1_l_msgId = 0xFFFFFFFF; /* HD: no l_msg */
    m738 = pMsgNo != nullptr ? (u32)*pMsgNo : 0;
    m744 = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}
VERIFY(0x022168EC, &daNpc_Bs1_c::evn_talk_init);

/* 02216994 */
BOOL daNpc_Bs1_c::evn_continue_talk_init(int actorId) {
    WWHD_FUNC(0x02216994, BOOL, this, actorId);
    be<u32>* pEndMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorId, STR(0x1001860C) /* "EndMsgNo" */, 3);
    m744 = pEndMsgNo != nullptr ? (u32)*pEndMsgNo : 0;
    return TRUE;
}
VERIFY(0x02216994, &daNpc_Bs1_c::evn_continue_talk_init);

/* 022169F8 */
BOOL daNpc_Bs1_c::evn_talk() {
    WWHD_FUNC(0x022169F8, BOOL, this);
    u32 mgr = bs1_msgMgr(); /* HD: the message manager replaces l_msg */
    if (bs1_l_msgId == 0xFFFFFFFF) {
        u32 id = bs1_msgSet(mgr, m738, &eyePos);
        bs1_l_msgId = id;
        if (id != 0xFFFFFFFF) {
            bs1_demoMsgFlagOn();
        }
    } else {
        setAnmFromMsgTag();
        if (bs1_msgGetStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
            /* next_msgStatus's result is passed on in the full register */
            bs1_msgSetStatus(mgr, gabi::call<u32>(0x02214858, this, &m738, (u32)0));
            if (bs1_msgGetStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                bs1_msgSet(mgr, m738, nullptr);
            }
        } else if (bs1_msgGetStatus(mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            bs1_msgSetStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            bs1_l_msgId = 0xFFFFFFFF;
            return TRUE;
        } else if ((bs1_msgGetStatus(mgr) == 2 || bs1_msgGetStatus(mgr) == 6) && m738 == m744) {
            m744 = 0;
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022169F8, &daNpc_Bs1_c::evn_talk);

/* 02216B68 */
BOOL daNpc_Bs1_c::evn_jnt_lock_init(int actorIdx) {
    WWHD_FUNC(0x02216B68, BOOL, this, actorIdx);
    be<u32>* substance = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorIdx, STR(0x10018618) /* "prm" */, 3);
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
VERIFY(0x02216B68, &daNpc_Bs1_c::evn_jnt_lock_init);

/* 02216C18 */
BOOL daNpc_Bs1_c::evn_wait_init(int actorIdx) {
    WWHD_FUNC(0x02216C18, BOOL, this, actorIdx);
    void* pTimer = dComIfGp_evmng_getMySubstanceP(actorIdx, STR(0x1001861C) /* "Timer" */, 3);
    m63E = pTimer != nullptr ? gabi::load<s16>(gabi::ea(pTimer) + 2) : (s16)0;
    return TRUE;
}
VERIFY(0x02216C18, &daNpc_Bs1_c::evn_wait_init);

/* 02216C80 */
BOOL daNpc_Bs1_c::evn_wait() {
    WWHD_FUNC(0x02216C80, BOOL, this);
    return gabi::call<s16>(0x02055B64, &m63E) == 0 ? TRUE : FALSE; /* cLib_calcTimer */
}
VERIFY(0x02216C80, &daNpc_Bs1_c::evn_wait);

/* 02216CAC */
BOOL daNpc_Bs1_c::evn_set_anm_init(int actorIdx) {
    WWHD_FUNC(0x02216CAC, BOOL, this, actorIdx);
    be<u32>* pAnmNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(actorIdx, STR(0x10018624) /* "AnmNo" */, 3);
    u32 anmNo = pAnmNo != nullptr ? (u32)*pAnmNo : 0;
    setAnm((s8)anmNo);
    return TRUE;
}
VERIFY(0x02216CAC, &daNpc_Bs1_c::evn_set_anm_init);

/* 02216D1C (unnamed by the matcher) */
BOOL daNpc_Bs1_c::evn_praise_init() {
    WWHD_FUNC(0x02216D1C, BOOL, this);
    u32 save = gabi::load<u32>(0x101F84DC);
    s16 life = (s16)(gabi::load<u16>(save + 0x20) - gabi::load<u16>(save + 0x22)); /* getMaxLife - getLife */
    f32 f = (f32)(s32)life;
    /* HD: dComIfGp_setItemLifeCount adds to a float */
    u32 a = dComIfGp_ea() + 0x5B44;
    gabi::store<f32>(a, gabi::fadds_ppc(gabi::load<f32>(a), f));
    return TRUE;
}
VERIFY(0x02216D1C, &daNpc_Bs1_c::evn_praise_init);

/* 02216DA0 (unnamed by the matcher) */
BOOL daNpc_Bs1_c::evn_mantan_init() {
    WWHD_FUNC(0x02216DA0, BOOL, this);
    u32 save = gabi::load<u32>(0x101F84DC);
    s16 life = (s16)(gabi::load<u16>(save + 0x20) - gabi::load<u16>(save + 0x22));
    u32 magic = gabi::load<u8>(save + 0x33) - gabi::load<u8>(save + 0x34);  /* getMaxMagic - getMagic */
    u32 bombs = gabi::load<u8>(save + 0x90) - gabi::load<u8>(save + 0x8A);  /* getBombMax - getBombNum */
    u32 arrows = gabi::load<u8>(save + 0x8F) - gabi::load<u8>(save + 0x89); /* getArrowMax - getArrowNum */
    f32 f = (f32)(s32)life;
    u32 a = dComIfGp_ea() + 0x5B44;
    gabi::store<f32>(a, gabi::fadds_ppc(gabi::load<f32>(a), f));
    a = dComIfGp_ea() + 0x5B60;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) + magic));
    a = dComIfGp_ea() + 0x5B6C;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) + bombs));
    a = dComIfGp_ea() + 0x5B68;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) + arrows));
    return TRUE;
}
VERIFY(0x02216DA0, &daNpc_Bs1_c::evn_mantan_init);

/* 02216E90 */
BOOL daNpc_Bs1_c::privateCut() {
    WWHD_FUNC(0x02216E90, BOOL, this);
    /* cut_name_tbl (0x101BD5FC): TALKMSG, CONTINUE_TALK, JNTLOCK, WAIT, SETANM, PRAISE, MANTAN, GETTICKET */
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(mEventCut.mpEvtStaffName), nullptr, 0);
    if (staffId == -1) {
        return FALSE;
    }
    s32 actIdx = gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, 0x101BD5FC, 8, 1, 0);
    if (actIdx == -1) {
        dComIfGp_evmng_cutEnd(staffId);
    } else {
        if (dComIfGp_evmng_getIsAddvance(staffId)) {
            switch (actIdx) {
            case 0: evn_talk_init(staffId); break;
            case 1: evn_continue_talk_init(staffId); break;
            case 2: evn_jnt_lock_init(staffId); break;
            case 3: evn_wait_init(staffId); break;
            case 4: evn_set_anm_init(staffId); break;
            case 5: evn_praise_init(); break;
            case 6: evn_mantan_init(); break;
            case 7: /* dComIfGs_setReserveItemEmpty() */
                gabi::call(0x025B7270, gabi::load<u32>(0x101F84DC) + 0x96);
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
VERIFY(0x02216E90, &daNpc_Bs1_c::privateCut);

/* 0221709C */
BOOL daNpc_Bs1_c::event_action(void*) {
    WWHD_FUNC(0x0221709C, BOOL, this, (u32)0);
    if (mActionStatus == ACTION_STARTING) {
        dComIfGp_evmng_getMyStaffId(STR(0x10018678) /* "Bs1" */, nullptr, 0);
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != ACTION_ENDING) {
        privateCut();
        if (dComIfGp_evmng_endCheck(mEventIdxs[m82B])) {
            m82A = 0;
            m82B = 2;
            bs1_event_reset();
            bs1_setAction(this, BS1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}
VERIFY(0x0221709C, &daNpc_Bs1_c::event_action);

/* 02217258 */
BOOL daNpc_Bs1_c::wait_action(void*) {
    WWHD_FUNC(0x02217258, BOOL, this, (u32)0);
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
VERIFY(0x02217258, &daNpc_Bs1_c::wait_action);

/* 02217368 */
BOOL daNpc_Bs1_c::getdemo_action(void*) {
    WWHD_FUNC(0x02217368, BOOL, this, (u32)0);
    /* a_name (0x101BD61C): "Bs1", "Bs2"; a_cut_name (0x101BD624): "dummy1", "dummy2" */
    u32 name = gabi::load<u32>(0x101BD61C + 4 * (s32)mType);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(name), nullptr, 0);
    gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffIdx, 0x101BD624, 2, 0, 0); /* getMyActIdx (unused) */
    if (mActionStatus == ACTION_STARTING) {
        bs1_offPlayerNoDraw(dComIfGp_getPlayer(0));
        m830 = (s8)m831;
        mShopCamAction.Reset();
        u8 itemNo = mShopItems.getSelectItemNo();
        u32 itemPID = gabi::call<u32>(0x025D7DEC, &current.pos, (u32)itemNo, 0, -1, (s32)(s8)current.roomNo, 0, 0); /* fopAcM_createItemForPresentDemo */
        if (itemPID != 0xFFFFFFFF) {
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, itemPID); /* dComIfGp_event_setItemPartnerId */
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
                if (bs1_getEventReg(0x86FF) != 0) {
                    m740 = 0xF4C;
                } else {
                    m740 = 0xF4E;
                }
            } else {
                m740 = 0x2F53;
            }
            bs1_event_reset();
            bs1_setAction(this, BS1_wait_action);
        }
        lookBack();
    }
    return TRUE;
}
VERIFY(0x02217368, &daNpc_Bs1_c::getdemo_action);

/* ---- joint-table helpers (SHARED-CANDIDATE, same as d_a_npc_bms1.h) ---- */
static inline s8 bs1_getJointIndex(J3DModelData* d, u32 name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return gabi::call<s8>(0x027DF9B0, tab, name); /* JUTNameTab::getIndex */
}
static inline u16 bs1_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void bs1_setJointCallBack(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline void* bs1_getRes2(s32 idx) { return dComIfG_getObjectRes(STR(0x100184C8) /* "Bs" */, idx, BS1_SAFESTRING_VTBL); }

/* 02212818 */
BOOL daNpc_Bs1_c::CreateHeap() {
    WWHD_FUNC(0x02212818, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)bs1_getRes2(0x13);
    J3DAnmTransform* bck = (J3DAnmTransform*)bs1_getRes2(0xC);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (morf != nullptr) {
        mpMorf = morf;
        if (morf->getModel() != nullptr) goto ok;
    }
    mpMorf = nullptr;
    return FALSE;
ok:
    m_head_jnt_num = bs1_getJointIndex(modelData, 0x100184CC /* "head" */);
    if (m_head_jnt_num < 0) JUT_ASSERT_fail(STR(0x100184D4), 0xAD0, STR(0x100184FC));
    m_backbone_jnt_num = bs1_getJointIndex(modelData, 0x10018510 /* "backbone1" */);
    if (m_backbone_jnt_num < 0) JUT_ASSERT_fail(STR(0x100184D4), 0xAD2, STR(0x100184E4));
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
            bs1_setJointCallBack(gabi::load<u32>(gabi::ea(mpMorf->getModel()) + 0xAC), jntNo, 0x0221225C /* nodeCallBack_Bs */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    void* brk = bs1_getRes2(0x1A);
    J3DModelData* cursorData = (J3DModelData*)bs1_getRes2(0x17);
    mpShopCursor = gabi::call<ShopCursor_c_l*>(0x025BBD7C, cursorData, brk, (f32)bs1_child(mType)->m30); /* ShopCursor_create */
    if (mpShopCursor.get() == nullptr) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02212818, &daNpc_Bs1_c::CreateHeap);

/* 02213610 (the matcher leaves it unnamed) */
cPhs_State daNpc_Bs1_c::_create() {
    WWHD_FUNC(0x02213610, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Bs1_c): the HD constructor, inline */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (gabi::ea(this) != 0) { /* skipped for NULL (GHS constructor form) */
            fopAc_ac_c_ct(this);
            __vtbl = BS1_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, {0x100183E4, 0x10018404, 0x100183F4});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x100183D4);
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
            gabi::call(0x0259DAA0, &mJntCtrl);  /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            mStickControl.__vtbl = 0x10050788;
            gabi::call(0x025885C4, &mStickControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000); /* STControl::setWaitParm */
            gabi::call(0x025885E8, &mStickControl);                                         /* STControl::init */
            gabi::call(0x025BBDF0, &mShopCamAction); /* ShopCam_action_c::ShopCam_action_c */
            gabi::call(0x025BC760, &mShopItems);     /* ShopItems_c::ShopItems_c */
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, STR(0x10018590) /* "Bs" */);
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
            hio->mNo = mDoHIO_createChild(STR(0x10018594), hio);
        }
        hio->m8 = hio->m8 + 1;
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return phase_state;
}
VERIFY(0x02213610, &daNpc_Bs1_c::_create);

/* 02213894 */
BOOL daNpc_Bs1_c::_delete() {
    WWHD_FUNC(0x02213894, BOOL, this);
    dComIfG_resDelete(&mPhase, STR(0x100185A8) /* "Bs" */);
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
VERIFY(0x02213894, &daNpc_Bs1_c::_delete);

/* 02213E18 */
BOOL daNpc_Bs1_c::_execute() {
    WWHD_FUNC(0x02213E18, BOOL, this);
    daNpc_Bs1_childHIO_c* c = bs1_child(mType);
    mJntCtrl.setParam(c->mMaxBackboneX, c->mMaxBackboneY, c->mMinBackboneX, c->mMinBackboneY, c->mMaxHeadX, c->mMaxHeadY,
                      c->mMinHeadX, c->mMinHeadY, c->mMaxTurnStep);
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
    tevStr.mRoomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8); /* dBgS::GetRoomId */
    u8 color = (u8)gabi::call<s32>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8);     /* dBgS::GetPolyColor */
    J3DModel* model = mpMorf->getModel();
    gabi::store<u8>(gabi::ea(this) + 0x1CA, color); /* tevStr.mEnvrIdxOverride */
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
VERIFY(0x02213E18, &daNpc_Bs1_c::_execute);

/* 02214104 */
BOOL daNpc_Bs1_c::_draw() {
    WWHD_FUNC(0x02214104, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pModelData, mFrame);
    J3DModel* pHelmetModel = mpHelmetModel;
    setLightTevColorType(dKy_getEnvlight(), pHelmetModel, &tevStr);
    mpMorf->updateDL();
    if (bs1_child(mType)->m2C != 0) {
        PSMTXCopy(gabi::at<Mtx34>(bs1_getAnmMtx(pModel, m_head_jnt_num)), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(4.0f, 0.0f, 0.0f);
        J3DModel_setBaseTRMtx(pHelmetModel, mDoMtx_stack_c::get());
        mDoExt_modelUpdateDL(pHelmetModel, 0);
    }
    gabi::store<u32>(gabi::ea(pModelData) + 0x38, 0); /* mBtpAnm.remove(pModelData) */
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
    gabi::call(0x025BEBB8, 0x80 /* DSNAP_TYPE_NPC_BS1 */, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
    return TRUE;
}
VERIFY(0x02214104, &daNpc_Bs1_c::_draw);

/* ---- shop bindings (SHARED-CANDIDATE) ---- */
static inline BOOL bs1_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, (u32)item); } /* dComIfGs_checkGetItem */
static inline BOOL bs1_isArrow(u8 item) { return gabi::call<BOOL>(0x02550FDC, (u32)item); }
static inline u32 bs1_createShopItem(u32 pos, u8 itemNo, void* angle, s8 roomNo) {
    return gabi::call<u32>(0x025D87E4, pos, (u32)itemNo, angle, (s32)roomNo, 0, 0); /* fopAcM_createShopItem */
}
static inline void bs1_ShopItems_setItemSetDataList(ShopItems_c_l* items, be<u32>* list) { gabi::call(0x025BCFA0, items, list); }
static inline void bs1_ShopItems_SoldOutItem(ShopItems_c_l* items, s32 i) { gabi::call(0x025BCECC, items, i); }
/* __shop_items_set_data::mpItemData->mItemNo */
static inline u8 bs1_setDataItemNo(u32 setData) { return gabi::load<u8>(gabi::load<u32>(setData) + 3); }
/* Item_set_pos_data_tbl[mShopIndex] (pointer table 0x101EB974) */
static inline u32 bs1_posTbl(daNpc_Bs1_c* a) { return gabi::load<u32>(0x101EB974 + 4 * (s32)a->mShopIndex); }

/* 02212BA0 */
void daNpc_Bs1_c::createShopList() {
    WWHD_FUNC(0x02212BA0, void, this);
    /* Item_set_data3 (0x101BD548), Item_set_data4 (0x101BD560), Item_set_data5 (0x101BD56C),
     * Item_set_dataBs2 (0x101BD578); shopItems_setData_Bomb30Bs2 0x101EB8C8, arrow30Bs2 0x101EB8DC,
     * red_bottleBs2 0x101EB8F0 */
    gabi::Local<csXyz> angle; /* csXyz::Zero (0x101FFB14) */
    u16 ax = gabi::load<u16>(0x101FFB14), az = gabi::load<u16>(0x101FFB18), ay = gabi::load<u16>(0x101FFB16);
    angle->z = (s16)az;
    angle->x = (s16)ax;
    angle->y = (s16)ay;
    if (mType == 0) {
        u32 pDataSet;
        switch ((u32)(s32)mShopIndex) {
        case 4: pDataSet = 0x101BD560; break;
        case 5: pDataSet = 0x101BD56C; break;
        default: pDataSet = 0x101BD548; break;
        }
        for (int i = 0; i < 3; i++) {
            u8 itemNo = bs1_setDataItemNo(gabi::load<u32>(pDataSet + 4 * i));
            int idx = i;
            if ((itemNo == 0x2C /* dItemNo_BAIT_BAG_e */ && bs1_checkGetItem(0x2C)) ||
                (itemNo == 0x83 /* dItemNo_HYOI_PEAR_e */ && bs1_checkGetItem(0x31 /* dItemNo_BOMB_BAG_e */))) {
                itemNo = bs1_setDataItemNo(gabi::load<u32>(pDataSet + 4 * (i + 3)));
                idx += 3;
            }
            mShopItems.mItemActorProcessIds[i] = bs1_createShopItem(bs1_posTbl(this) + 12 * i, itemNo, angle.get(), current.roomNo);
            mpItemSetList[i] = gabi::load<u32>(pDataSet + 4 * idx);
        }
    } else {
        gabi::Local<be<u32>[5]> dataSet; /* GameCube dataSet[4]; the fifth slot takes the (mock-only) overflow, as the frame padding does */
        u32 ds = gabi::ea(dataSet.get());
        int index = 0;
        if (bs1_checkGetItem(0x31)) {
            gabi::store<u32>(ds, 0x101EB8C8);
            index = 1;
        }
        if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x68) != 0xFF) { /* dComIfGs_getItem(dInvSlot_BOW_e) */
            gabi::store<u32>(ds + 4 * index, 0x101EB8DC);
            index++;
        }
        gabi::store<u32>(ds + 4 * index, 0x101EB8F0);
        index++;
        if (!bs1_checkGetItem(0x31)) {
            gabi::store<u32>(ds + 4 * index, 0x101EB8C8);
            index++;
        }
        if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x68) == 0xFF) {
            gabi::store<u32>(ds + 4 * index, 0x101EB8DC);
            index++;
        }
        /* HD: an assertion that at most four entries were written */
        if (index > 3) {
            JUT_ASSERT_fail(STR(0x1001851C), 0x749, STR(0x1001852C));
        }
        int dataIdx = 0;
        for (int i = 0; i < 3; i++) {
            u8 itemNo = bs1_setDataItemNo(gabi::load<u32>(0x101BD578 + 4 * i));
            if ((i == 0 && bs1_isEventBit(0x2020)) || (i == 1 && bs1_isEventBit(0x2010)) || (i == 2 && bs1_isEventBit(0x2008))) {
                u32 d = gabi::load<u32>(ds + 4 * dataIdx);
                itemNo = bs1_setDataItemNo(d);
                mpItemSetList[i] = d;
                dataIdx++;
            } else {
                mpItemSetList[i] = gabi::load<u32>(0x101BD578 + 4 * i);
            }
            mShopItems.mItemActorProcessIds[i] = bs1_createShopItem(bs1_posTbl(this) + 12 * i, itemNo, angle.get(), current.roomNo);
        }
    }
    mShopItems.mNumItems = 3; /* setItemSum(3) */
    bs1_ShopItems_setItemSetDataList(&mShopItems, mpItemSetList);
    for (int i = 0; i < 3; i++) {
        mShopItems.mSelectedItemIdx = (s16)i;
        if ((!bs1_checkGetItem(0x31) && bs1_isBomb(mShopItems.getSelectItemNo())) ||
            (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x68) == 0xFF && bs1_isArrow(mShopItems.getSelectItemNo()))) {
            bs1_ShopItems_SoldOutItem(&mShopItems, i);
            m76C[i] = 1;
        } else {
            m76C[i] = 0;
        }
        u32 src = bs1_posTbl(this) + 12 * i;
        f32 x = gabi::load<f32>(src);
        mItemPosOffsets[i].x = x;
        f32 y = gabi::load<f32>(src + 4);
        mItemPosOffsets[i].y = y;
        f32 z = gabi::load<f32>(src + 8);
        mItemPosOffsets[i].z = z;
    }
    mShopItems.mSelectedItemIdx = -1;
}
VERIFY(0x02212BA0, &daNpc_Bs1_c::createShopList);

/* 02212258: daNpc_Bs1_XyEventCB (unnamed by the matcher) */
static s16 daNpc_Bs1_XyEventCB(void* i_this, int value) {
    WWHD_FUNC(0x02212258, s16, i_this, value);
    return ((daNpc_Bs1_c*)i_this)->XyEventCB(value);
}
VERIFY(0x02212258, daNpc_Bs1_XyEventCB);

/* CreateInit: the part shared by all types (GHS duplicated it per case) */
static inline void bs1_CreateInit_common(daNpc_Bs1_c* a) {
    u32 py = gabi::load<u32>(gabi::ea(a) + 0x318);
    u32 px = gabi::load<u32>(gabi::ea(a) + 0x314);
    u32 pz = gabi::load<u32>(gabi::ea(a) + 0x31C);
    gabi::store<u32>(gabi::ea(&a->m718) + 0, px); /* m718 = current.pos */
    gabi::store<u32>(gabi::ea(&a->m718) + 4, py);
    gabi::store<u32>(gabi::ea(&a->m718) + 8, pz);
    gabi::store<u32>(gabi::ea(a) + 0x100, 0x02212258); /* eventInfo.setXyEventCB(daNpc_Bs1_XyEventCB) */
    a->mStts.Init(0xFF, 0xFF, a);
    a->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BD504) /* l_cyl_src */);
    a->m708 = 0;
    a->m740 = 0;
    a->mShopIndex = (s8)gabi::load<u8>(gabi::ea(a) + 0xB3); /* fopAcM_GetParam(this) & 0xFF */
    a->m837 = 0;
    gabi::store<u32>(gabi::ea(&a->mCyl) + 0x44, gabi::ea(&a->mStts)); /* mCyl.SetStts(&mStts) */
    a->m836 = 0;
}

/* 02213060 */
BOOL daNpc_Bs1_c::CreateInit() {
    WWHD_FUNC(0x02213060, BOOL, this);
    m726.x = current.angle.x;
    m726.y = current.angle.y;
    m726.z = current.angle.z;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAA);  /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAA);  /* attention_info.distances[SPEAK] */
    gravity = -30.0f;
    switch ((u32)(s32)mType) {
    case 0:
        bs1_setAction(this, BS1_wait_action);
        m83A = dComIfGp_evmng_getEventIdx(STR(0x1001855C) /* "BS1_GETDEMO" */, 0xFF);
        gabi::call(0x0259F7D4, &mEventCut, STR(0x10018544) /* "Bs1" */, this); /* setActorInfo */
        mEventCut.mpJntCtrl = &mJntCtrl;                                        /* setJntCtrlPtr */
        break;
    case 1:
        bs1_setAction(this, BS1_wait_action);
        m83A = dComIfGp_evmng_getEventIdx(STR(0x10018568) /* "BS2_GETDEMO" */, 0xFF);
        gabi::call(0x0259F7D4, &mEventCut, STR(0x10018548) /* "Bs2" */, this);
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
    gabi::store<u32>(gabi::ea(&mShopCamAction) + 4, 0);
    gabi::store<u16>(gabi::ea(&mShopCamAction) + 0, 0);
    mShopCamAction.mCamDataIdx = (s16)idx;
    gabi::store<u16>(gabi::ea(&mShopCamAction) + 2, 0);
    mShopItems.mItemSetListGlobalIdx = (s16)idx;
    createShopList();
    m82B = 2;
    mEventIdxs[0] = dComIfGp_evmng_getEventIdx(STR(0x10018574) /* "PUT_PRAICE_TICKET" */, 0xFF);
    mEventIdxs[1] = dComIfGp_evmng_getEventIdx(STR(0x1001854C) /* "PUT_FULL_TICKET" */, 0xFF);
    return TRUE;
}
VERIFY(0x02213060, &daNpc_Bs1_c::CreateInit);

/* 0221763C: daNpc_Bs1_childHIO_c::daNpc_Bs1_childHIO_c (GHS: allocates when this == NULL) */
static daNpc_Bs1_childHIO_c* daNpc_Bs1_childHIO_ct(daNpc_Bs1_childHIO_c* self) {
    WWHD_FUNC(0x0221763C, daNpc_Bs1_childHIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_Bs1_childHIO_c*)operator_new(0x48);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x10018424;
    gabi::call(0x0259DA18, gabi::ea(self) + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    /* HD: the members are zeroed */
    self->m2C = 0;
    self->m40 = 0.0f;
    self->m30 = 0.0f;
    self->m3C = 0.0f;
    self->m34 = 0.0f;
    self->m38 = 0.0f;
    return self;
}
VERIFY(0x0221763C, daNpc_Bs1_childHIO_ct);

/* 022176B4: daNpc_Bs1_HIO_c::daNpc_Bs1_HIO_c */
static daNpc_Bs1_HIO_c* daNpc_Bs1_HIO_ct(daNpc_Bs1_HIO_c* self) {
    WWHD_FUNC(0x022176B4, daNpc_Bs1_HIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_Bs1_HIO_c*)operator_new(0x9C);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x10018434;
    gabi::call(0x028EFFD0, self->mChild, 2, 0x48, 0x0221763C); /* __construct_array */
    for (int i = 0; i < 2; i++) {
        daNpc_Bs1_childHIO_c* c = &self->mChild[i];
        c->m04 = -64.0f;
        c->mMaxHeadX = 0x1FFE;
        c->mMaxHeadY = 0x38E0;
        c->mMaxBackboneX = 0;
        c->mMaxBackboneY = 0x1C70;
        c->mMinHeadX = -0x1FFE;
        c->mMinHeadY = -0x38E0;
        c->mMinBackboneX = 0;
        c->mMinBackboneY = -0x1C70;
        c->mMaxTurnStep = 0x1000;
        c->mMaxHeadTurnVel = 0x800;
        c->mAttnYOffset = 55.0f;
        c->m22 = 0;
        c->mMaxAttnDistXZ = 300.0f;
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
VERIFY(0x022176B4, daNpc_Bs1_HIO_ct);

/* 0221782C: __sinit_d_a_npc_bs1_cpp (compiler-generated) */
static void __sinit_d_a_npc_bs1_cpp() {
    WWHD_FUNC(0x0221782C, void);
    sinit_header_statics(0x1046685C, 0x101BD62C);
    daNpc_Bs1_HIO_ct(bs1_HIO()); /* l_HIO */
}
VERIFY(0x0221782C, __sinit_d_a_npc_bs1_cpp);

/* 022178CC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dtor(SafeString* self, u32 flags) {
    WWHD_FUNC(0x022178CC, void, self, flags);
    if (self != nullptr && (flags & 1)) operator_delete(self);
}
VERIFY(0x022178CC, SafeString_dtor);

/* 02217988: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's SafeString vtable
   slot +0x14, next to the deleting destructor 022178CC at +0x0C) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x02217988, void, (u32)0);
}
VERIFY(0x02217988, SafeString_assureTermination);

/* 022178E0: daNpc_Bs1_c deleting destructor (HD virtual destructor) */
static void daNpc_Bs1_dtor(daNpc_Bs1_c* self, u32 flags) {
    WWHD_FUNC(0x022178E0, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x025886D8, &self->mStickControl, 2); /* STControl::~STControl */
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    u32 p = gabi::ea(self);
    gabi::store<u32>(p + 0x464, 0x100183F4); /* dBgS_ObjAcch vtables (this TU) */
    gabi::store<u32>(p + 0x458, 0x10018404);
    gabi::call(0x024EFD9C, &self->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x025D50BC, self, 0);         /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x022178E0, daNpc_Bs1_dtor);
