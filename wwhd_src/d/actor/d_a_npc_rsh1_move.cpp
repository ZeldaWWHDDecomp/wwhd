/**
 * d_a_npc_rsh1_move.cpp (WWHD): Zunari's path movement, wait/talk sub-modes, actions and event cuts.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_rsh1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_rsh1.h"

static inline fopAc_ac_c* rsh1_link() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); }
/* mpMorf->isMorf(): mCurMorf (+0xB0) < 1.0f */
static inline bool rsh1_isMorf(daNpc_Rsh1_c* a) { return gabi::load<f32>(gabi::ea(a->mpMorf.get()) + 0xB0) < 1.0f; }
/* daPy_py_c::offNoResetFlg0(daPyFlg0_NO_DRAW): status word +0x3B8 */
static inline void rsh1_offNoDraw(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
}
static inline void rsh1_event_reset() {
    u32 a = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
}
/* ShopItems_c::init() (inline) */
static inline void rsh1_items_init(ShopItems_c_l* it) {
    it->mSelectedItemIdx = -1;
    it->mbIsHide = 0;
    it->m3C = 0;
}

/* 022D9E04 */
int daNpc_Rsh1_c::getAimShopPosIdx() {
    WWHD_FUNC(0x022D9E04, int, this);
    fopAc_ac_c* link_p = rsh1_link();
    int temp_78C = m78C;
    int result_index = 1;
    f32 fcond = rsh1_HIO().m4C;
    int condition = temp_78C < 1 ? 1 : temp_78C;
    f32 lx = link_p->current.pos.x;
    f32 ly = link_p->current.pos.y;
    f32 lz = link_p->current.pos.z;
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> xz1;
    gabi::Local<cXyz> xz2;
    for (int i = 0; i <= condition; i++) {
        temp->x = lx;
        temp->y = ly;
        temp->z = lz;
        gabi::call(0x028E8DAC, temp.get(), &mPathPointPos[i], temp.get()); /* PSVECSubtract (temp -= pos) */
        f32 tz = temp->z;
        f32 tx = temp->x;
        xz1->z = tz;
        xz1->y = 0.0f;
        xz1->x = tx;
        f32 mag = std_sqrtf(PSVECSquareMag(xz1.get()));
        if (mag < fcond) {
            f32 x2 = temp->x;
            f32 z2 = temp->z;
            xz2->x = x2;
            xz2->y = 0.0f;
            xz2->z = z2;
            fcond = std_sqrtf(PSVECSquareMag(xz2.get()));
            result_index = i;
        }
    }
    return result_index;
}
VERIFY(0x022D9E04, &daNpc_Rsh1_c::getAimShopPosIdx);

/* 022D9F74 */
BOOL daNpc_Rsh1_c::pathMove(be<s32>* param_1) {
    WWHD_FUNC(0x022D9F74, BOOL, this, param_1);
    s32 temp4;
    if (param_1) {
        temp4 = *param_1;
    } else {
        temp4 = getAimShopPosIdx();
    }
    s8 temp = m704;
    s8 aim = m744[temp4];
    if (temp == aim) {
    } else if (temp < aim) {
        temp = (s8)(temp + 1);
    } else {
        temp = (s8)(temp - 1);
    }
    u32 point = gabi::load<u32>(gabi::ea(mpPath.get()) + 8) + temp * 0x10;
    f32 step = rsh1_HIO().m48;
    gabi::Local<cXyz> target_pos;
    f32 py = gabi::load<f32>(point + 8);
    f32 px = gabi::load<f32>(point + 4);
    f32 pz = gabi::load<f32>(point + 0xC);
    target_pos->x = px;
    target_pos->y = py;
    target_pos->z = pz;
    if (gabi::call<BOOL>(0x0200F764, &current.pos, target_pos.get(), step) != 0) { /* cLib_chasePosXZ */
        m704 = temp;
        u8 arg3 = gabi::load<u8>(point + 3);
        if (arg3 != 0xFF) {
            s32 temp2 = arg3 - 1;
            s32 temp3 = arg3 + 8;
            if (temp2 != -1 && mShopIdx != temp2) {
                rsh1_items_init(&mShopItemsArr[temp2]);
            }
            s32 cur = mShopIdx;
            if (cur != -1 && cur != temp2) {
                rsh1_items_init(&mShopItemsArr[cur]);
            }
            mShopIdx = temp2;
            if (temp2 != -1) {
                mShopCamAct.mCamDataIdx = (s16)temp3; /* setCamDataIdx(mShopIdx + 8), then setCamDataIdx(temp3) */
                mpShopItems = &mShopItemsArr[temp2];
            } else {
                mpShopItems = nullptr;
                mShopCamAct.mCamDataIdx = (s16)temp3;
            }
            if ((u32)temp4 == (u32)(temp2 + 1)) {
                gabi::call(0x0200F428, &current.angle.y, (s32)home.angle.y, 8, 0x1000); /* cLib_addCalcAngleS2 */
                return TRUE;
            }
        }
    }
    s16 target = cLib_targetAngleY(&current.pos, target_pos.get());
    gabi::call(0x0200F428, &current.angle.y, (s32)target, 8, 0x1000);
    return FALSE;
}
VERIFY(0x022D9F74, &daNpc_Rsh1_c::pathMove);

/* 022DA1C4 */
bool daNpc_Rsh1_c::wait01() {
    WWHD_FUNC(0x022DA1C4, bool, this);
    if (m771) {
        m95D = (u8)m95C;
        m95C = 2;
        return rsh1_isMorf(this);
    }
    if (gabi::call<BOOL>(0x02516464, &mCyl)) { /* mCyl.ChkCoHit() */
        fopAc_ac_c* actor_p = gabi::call<fopAc_ac_c*>(0x02515BBC, gabi::ea(&mCyl) + 0xDC); /* GetCoHitAc */
        if (actor_p && fpcM_GetName(actor_p) == 0xA8 /* fpcNm_PLAYER_e */) {
            /* HD: the player is in the shop: start the shop-out event instead of talking */
            if (checkCreateInShopPlayer()) {
                m95B = 5;
                actor_status = actor_status | 0x4000;
                mShopOutEventIdx = rsh1_getEventIdx(0x10021850 /* "RSH_SHOP_OUT" */);
                rsh1_setAction(this, RSH1_dummy_action);
                return false;
            }
            m95B = 2;
            m780 = 0x2883;
            m793 = 1;
        }
        return rsh1_isMorf(this);
    }
    fopAc_ac_c* link_p = rsh1_link();
    BOOL path_move_res = pathMove(nullptr);
    if (!path_move_res) {
        setAnm(5);
        m95B = 0;
    } else {
        setAnm(0);
    }
    if (mbAttention) {
        f32 abs_diff = std::fabs((f32)(current.pos.y - link_p->current.pos.y));
        if (path_move_res && abs_diff < 100.0f) {
            m95B = 3;
        } else if (m95B == 3) {
            m95B = 0;
        }
    }
    return rsh1_isMorf(this);
}
VERIFY(0x022DA1C4, &daNpc_Rsh1_c::wait01);

/* 022DA4C4 */
bool daNpc_Rsh1_c::talk01() {
    WWHD_FUNC(0x022DA4C4, bool, this);
    u16 result = talk();
    if (result == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopAc_ac_c* link_p = rsh1_link();
        m95C = (s8)m95D;
        rsh1_event_reset();
        mShopCamAct.Reset();
        rsh1_offNoDraw(link_p);
        m771 = 0;
        setAnm(0);
        u32 m = m778;
        if (m == 0x2856) {
            m95B = 4;
            mItemNo = 0x8C; /* dItemNo_TOWN_FLOWER_e */
        } else if (m == 0x288E) {
            m95B = 4;
            mItemNo = 0x78; /* dItemNo_SAIL_e */
        } else if (m == 0x285F) {
            m95B = 4;
            mItemNo = 0x2A; /* dItemNo_MAGIC_ARMOR_e */
        } else if (m == 0x2883 && m793 == 1) {
            rsh1_setAction(this, RSH1_pl_shop_out_action);
        }
    } else if (result == 8 && daNpc_Rsh1_shopMsgCheck(m778)) {
        mpShopItems->getSelectItemBuyMsg(); /* HD: the result is not compared any more */
    }
    /* HD: the choose/cancel button prompts follow the shop cursor being shown */
    if (mpShopCursor->mbShow != 0) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BBA, 0x17); /* dComIfGp_setDoStatusForce(dActStts_CHOOSE_e) */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB9, 0x27); /* dComIfGp_setAStatusForce(dActStts_CANCEL_e) */
    }
    return rsh1_isMorf(this);
}
VERIFY(0x022DA4C4, &daNpc_Rsh1_c::talk01);

/* 022DA748 */
BOOL daNpc_Rsh1_c::wait_action(void*) {
    WWHD_FUNC(0x022DA748, BOOL, this, (u32)0);
    if (mActionStatus == 0) {
        m95C = 1;
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != -1) {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x;
        s16 sum = (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]); /* + getHead_y() + getBackbone_y() */
        pos->x = x;
        f32 z = current.pos.z;
        f32 y = current.pos.y;
        pos->z = z;
        pos->y = y;
        u8 att = gabi::call<u8>(0x022D8D44, this, pos.get(), (s32)sum); /* chkAttention: its bool stored as returned */
        s8 m = m95C;
        mbAttention = att;
        switch ((u32)(s32)m) {
        case 1:
            wait01();
            break;
        case 2:
            talk01();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x022DA748, &daNpc_Rsh1_c::wait_action);

/* 022DA818 */
BOOL daNpc_Rsh1_c::getdemo_action(void*) {
    WWHD_FUNC(0x022DA818, BOOL, this, (u32)0);
    s32 staff_idx = dComIfGp_evmng_getMyStaffId(STR(0x10021860) /* "Rsh1" */, nullptr, 0);
    if (mActionStatus == 0) {
        rsh1_offNoDraw(rsh1_link());
        m95C = (s8)m95D;
        mShopCamAct.Reset();
        u32 pid = gabi::call<u32>(0x025D7DEC, &current.pos, (u32)mItemNo, 0, -1, (s32)fopAcM_GetRoomNo(this), 0, 0); /* fopAcM_createItemForPresentDemo */
        if (pid != 0xFFFFFFFF) {
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, pid); /* dComIfGp_event_setItemPartnerId */
        }
        dComIfGp_evmng_cutEnd(staff_idx);
        mActionStatus = (s8)(mActionStatus + 1);
    } else if (mActionStatus != -1) {
        dComIfGp_evmng_cutEnd(staff_idx);
        if (gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), STR(0x10021868) /* "RSH_GET_DEMO" */)) { /* endCheckOld */
            m95B = 1;
            rsh1_event_reset();
            u8 item = mItemNo;
            if (item == 0x8C) {
                m780 = 0x2857;
            } else if (item == 0x78) {
                m780 = 0x288F;
            } else if (item == 0x2A) {
                m780 = 0x2860;
            }
            mItemNo = 0xFF;
            rsh1_setAction(this, RSH1_wait_action);
        }
    }
    return TRUE;
}
VERIFY(0x022DA818, &daNpc_Rsh1_c::getdemo_action);
