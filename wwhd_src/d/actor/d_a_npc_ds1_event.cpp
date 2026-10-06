/**
 * d_a_npc_ds1_event.cpp (WWHD)
 * NPC - Doc Bandam (Windfall Potion Shop shopkeeper): event cuts and event actions
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#include "d/actor/d_a_npc_ds1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline s32 dComIfGp_evmng_getMyActIdx_l(s32 staffId, u32 table, s32 num, s32 force, s32 last) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, table, num, force, last);
}
static inline u8 dComIfGp_evmng_getIsAddvance_l(s32 staffId) { return gabi::call<u8>(0x025447C8, dComIfGp_getPEvtManager(), staffId); }
static inline void fopMsgM_demoMsgFlagOn() { gabi::call(0x025DB58C); }
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 item, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, item, argFlag, bitNo, roomNo, angle, scale);
}
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
static inline void dComIfGs_onEventBit_ds1(u16 f) {
    dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
static inline void ShopItems_showItem(ShopItems_c_l* s) { gabi::call(0x025BCE04, s); }
/* daPy_py_c: player status word at +0x3B8; bit 0x08000000 hides the player (offPlayerNoDraw / on) */
static inline void daPy_offNoDraw(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
}
static inline void daPy_onNoDraw(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) | 0x08000000u);
}
/* dNpc_HeadAnm_c (at 0x850) */
static inline void dNpc_HeadAnm_swing_horizone_init(void* h, s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F514, h, a, b, c, d); }
static inline void dNpc_HeadAnm_swing_vertical_init(void* h, s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F36C, h, a, b, c, d); }
/* HD message manager (*0x101F4B5C) */
static inline u32 l_msgMng_ds1() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_getStatus(u32 mng) { return gabi::call<u32>(0x025F795C, mng); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
/* 025F74D0: sets the message status (reads only r4) */
static inline void fopMsgM_setStatus(u32 mng, u32 status) { gabi::call(0x025F74D0, mng, status); }
/* dComIfGp_setItemBeastNumCount(i, n): play+0x5B70 + 2i (s16) */
static inline void dComIfGp_addItemBeastNumCount(s32 i, s16 n) {
    u32 a = dComIfGp_ea() + 0x5B70 + 2 * i;
    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n));
}
static inline s32* evt_int(s32 staffId, u32 name) {
    return (s32*)dComIfGp_evmng_getMySubstanceP(staffId, gabi::at<const char>(name), 3);
}
static inline s32 ldi(s32* p) { return gabi::load<s32>(gabi::ea(p)); }
static inline void JPABaseEmitter_becomeInvalidEmitter_l(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}
static inline u8 gamma_u8(u32 c) { return (u8)gabi::ftoi(gabi::call<f32>(0x0222E124, c) * 255.0f); }
static inline void cLib_addCalcPos2(cXyz* v, const cXyz* t, f32 scale, f32 maxStep) { gabi::call(0x0200F164, v, t, scale, maxStep); }
static inline void cutEnd(s32 staffId) { dComIfGp_evmng_cutEnd(staffId); }

/* l_msgId (0x10466EE0): the message set by evn_talk */
#define l_msgId gabi::at<be<s32>>(0x10466EE0)

/* 022312F8 */
BOOL daNpc_Ds1_c::getdemo_action(void* arg) {
    WWHD_FUNC(0x022312F8, BOOL, this, arg);
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(0x10019BD8), nullptr, 0);
    s8 status = mActionStatus;
    if (status == 0) {
        s32 item = m9A8;
        if (item == 0xFF) {
            u8 itemNo = mItemNo;
            if (itemNo == 0x49)
                item = 0x51;
            else if (itemNo == 0x4A)
                item = 0x52;
            else
                item = 0x53;
        }
        daPy_offNoDraw(dComIfGp_getPlayer(0));
        mShopCam.mCurrActionFunc.fn = 0;
        mShopCam.mCurrActionFunc.idx = 0;
        mShopCam.mCurrActionFunc.delta = 0;
        mA5C = mA5D;
        u32 id = fopAcM_createItemForPresentDemo(&current.pos, item, 0, -1, current.roomNo, nullptr, nullptr);
        if (id != 0xFFFFFFFF)
            dComIfGp_event_setItemPartnerId(id);
        cutEnd(staffId);
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (status != -1) {
        fopMsgM_demoMsgFlagOn();
        cutEnd(staffId);
        if (dComIfGp_evmng_endCheck(mEventIdx[1])) {
            mOrderType = 1;
            if (m9A8 != 0xFF) {
                m9A4 = 0x1DDA;
                ShopItems_showItem(&mShopItems);
                m9A8 = 0xFF;
                mShopItems.mSelectedItemIdx = -1;
                dComIfGp_event_reset();
            } else {
                u32 msg = 0x1DCD;
                if (gabi::call<BOOL>(0x0222D37C, (u32)mItemNo)) /* daNpc_Ds1_checkCreateDrugChuchu */
                    msg = 0x1DC6;
                m9A4 = msg;
                dComIfGp_event_reset();
            }
            u8 itemNo = mItemNo;
            if (itemNo == 0x4A)
                dComIfGs_onEventBit_ds1(0xD04);
            else if (itemNo == 0x4B)
                dComIfGs_onEventBit_ds1(0xD02);
            ds1_setAction(this, DS1_wait_action, nullptr);
        }
    }
    return TRUE;
}
VERIFY(0x022312F8, &daNpc_Ds1_c::getdemo_action);

/* 0223164C */
BOOL daNpc_Ds1_c::dummy_action(void* arg) {
    WWHD_FUNC(0x0223164C, BOOL, this, arg);
    if (mActionStatus == 0)
        mActionStatus = 1;
    return TRUE;
}
VERIFY(0x0223164C, &daNpc_Ds1_c::dummy_action);

/* 02231668 */
BOOL daNpc_Ds1_c::evn_talk_init(int staffId) {
    WWHD_FUNC(0x02231668, BOOL, this, staffId);
    s32* msgNo = evt_int(staffId, 0x10019BE0);
    s32* endMsgNo = evt_int(staffId, 0x10019BE8);
    *l_msgId = -1;
    mMsgNo = msgNo != nullptr ? ldi(msgNo) : 0;
    m9A0 = endMsgNo != nullptr ? ldi(endMsgNo) : 0;
    return TRUE;
}
VERIFY(0x02231668, &daNpc_Ds1_c::evn_talk_init);

/* 02231710 */
BOOL daNpc_Ds1_c::evn_continue_talk_init(int staffId) {
    WWHD_FUNC(0x02231710, BOOL, this, staffId);
    s32* endMsgNo = evt_int(staffId, 0x10019BF4);
    m9A0 = endMsgNo != nullptr ? ldi(endMsgNo) : 0;
    return TRUE;
}
VERIFY(0x02231710, &daNpc_Ds1_c::evn_continue_talk_init);

/* 02231774 */
BOOL daNpc_Ds1_c::evn_ItemModel_init(int staffId) {
    WWHD_FUNC(0x02231774, BOOL, this, staffId);
    s32* p = evt_int(staffId, 0x10019C00);
    mItemModelFlags = (u8)(p != nullptr ? ldi(p) : 0);
    return TRUE;
}
VERIFY(0x02231774, &daNpc_Ds1_c::evn_ItemModel_init);

/* 022317DC */
BOOL daNpc_Ds1_c::evn_head_swing_init(int staffId) {
    WWHD_FUNC(0x022317DC, BOOL, this, staffId);
    s32* p = evt_int(staffId, 0x10019C04);
    u32 type = p != nullptr ? ldi(p) : 0;
    if (type < 1)
        dNpc_HeadAnm_swing_horizone_init(&m850, 2, 0x1000, 0x1000, 1);
    else if (type == 1)
        dNpc_HeadAnm_swing_vertical_init(&m850, 2, 0x1000, 0x800, 1);
    return TRUE;
}
VERIFY(0x022317DC, &daNpc_Ds1_c::evn_head_swing_init);

/* 0223188C */
BOOL daNpc_Ds1_c::evn_setAnm_init(int staffId) {
    WWHD_FUNC(0x0223188C, BOOL, this, staffId);
    s32* anmNo = evt_int(staffId, 0x10019C08);
    s32* loopCnt = evt_int(staffId, 0x10019C10);
    f32* morf = (f32*)dComIfGp_evmng_getMySubstanceP(staffId, STR(0x10019C18), 0);
    if (anmNo != nullptr) {
        s8 idx = (s8)ldi(anmNo);
        f32 m = -1.0f;
        if (morf != nullptr)
            m = gabi::load<f32>(gabi::ea(morf));
        setAnm(idx, m);
        mA4E = loopCnt != nullptr ? (u8)(s8)ldi(loopCnt) : 0;
    }
    if (mAnmIdx == 6) {
        /* static GXColor prm[3], env[3] (guards 0x10466F88 / 0x10466F8C) */
        u32 prm = 0x101BE120;
        if (gabi::load<u32>(0x10466F88) == 0) {
            gabi::store<u32>(0x10466F88, 1);
            gabi::store<u8>(prm + 0, gamma_u8(0xBC));
            gabi::store<u8>(prm + 1, gamma_u8(0x27));
            gabi::store<u8>(prm + 2, gamma_u8(0x40));
            gabi::store<u8>(prm + 4, gamma_u8(0x27));
            gabi::store<u8>(prm + 5, gamma_u8(0xBC));
            gabi::store<u8>(prm + 6, gamma_u8(0x40));
            gabi::store<u8>(prm + 8, gamma_u8(0x27));
            gabi::store<u8>(prm + 9, gamma_u8(0x63));
            gabi::store<u8>(prm + 10, gamma_u8(0xBC));
        }
        u32 env = 0x101BE12C;
        if (gabi::load<u32>(0x10466F8C) == 0) {
            gabi::store<u32>(0x10466F8C, 1);
            gabi::store<u8>(env + 0, gamma_u8(0xDE));
            gabi::store<u8>(env + 1, gamma_u8(0x80));
            gabi::store<u8>(env + 2, gamma_u8(0x38));
            gabi::store<u8>(env + 4, gamma_u8(0x34));
            gabi::store<u8>(env + 5, gamma_u8(0xDE));
            gabi::store<u8>(env + 6, gamma_u8(0x38));
            gabi::store<u8>(env + 8, gamma_u8(0x38));
            gabi::store<u8>(env + 9, gamma_u8(0xDE));
            gabi::store<u8>(env + 10, gamma_u8(0x80));
        }
        s32 i = 0;
        u8 itemNo = mItemNo;
        if (itemNo != 0x49)
            i = itemNo == 0x4A ? 1 : 2;
        if (m8D8 == 0) {
            GXColor* p = gabi::at<GXColor>(prm + 4 * i);
            GXColor* e = gabi::at<GXColor>(env + 4 * i);
            m8D8 = gabi::ea(dComIfGp_particle_set(0x81BD, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, p, e, nullptr));
            mLight.mColorR = (s16)(p->r << 2);
            mLight.mColorG = (s16)(p->g << 2);
            mLight.mColorB = (s16)(p->b << 2);
        }
        if (m8DC == 0) {
            GXColor* p = gabi::at<GXColor>(prm + 4 * i);
            GXColor* e = gabi::at<GXColor>(env + 4 * i);
            m8DC = gabi::ea(dComIfGp_particle_set(0x81BE, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1, p, e, nullptr));
        }
    }
    return TRUE;
}
VERIFY(0x0223188C, &daNpc_Ds1_c::evn_setAnm_init);

/* 02231D0C */
BOOL daNpc_Ds1_c::evn_move_pos_init(int staffId) {
    WWHD_FUNC(0x02231D0C, BOOL, this, staffId);
    u32 pos = gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, STR(0x10019C20), 1));
    evt_int(staffId, 0x10019C24);
    u32 dst = gabi::ea(&mMovePos);
    u32 src = pos != 0 ? pos : gabi::ea(&home.pos);
    gabi::store<u32>(dst, gabi::load<u32>(src));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
    return TRUE;
}
VERIFY(0x02231D0C, &daNpc_Ds1_c::evn_move_pos_init);

/* 02231DC8 */
BOOL daNpc_Ds1_c::evn_init_pos_init(int staffId) {
    WWHD_FUNC(0x02231DC8, BOOL, this, staffId);
    u32 pos = gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, STR(0x10019C2C), 1));
    s32* angle = evt_int(staffId, 0x10019C30);
    u32 src = pos != 0 ? pos : gabi::ea(&home.pos);
    u32 x = gabi::load<u32>(src), y = gabi::load<u32>(src + 4), z = gabi::load<u32>(src + 8);
    gabi::store<u32>(gabi::ea(&old.pos), x);
    gabi::store<u32>(gabi::ea(&old.pos) + 4, y);
    gabi::store<u32>(gabi::ea(&old.pos) + 8, z);
    gabi::store<u32>(gabi::ea(&current.pos), x);
    gabi::store<u32>(gabi::ea(&current.pos) + 4, y);
    gabi::store<u32>(gabi::ea(&current.pos) + 8, z);
    if (angle != nullptr)
        current.angle.y = gabi::load<s16>(gabi::ea(angle) + 2);
    else
        current.angle.y = home.angle.y;
    return TRUE;
}
VERIFY(0x02231DC8, &daNpc_Ds1_c::evn_init_pos_init);

/* 02231EBC */
BOOL daNpc_Ds1_c::evn_jnt_lock_init(int staffId) {
    WWHD_FUNC(0x02231EBC, BOOL, this, staffId);
    s32* p = evt_int(staffId, 0x10019C38);
    u32 type = p != nullptr ? ldi(p) : 0;
    if (type < 2) {
        mJntCtrl.mbHeadLock = (u8)type;
        mJntCtrl.mbBackBoneLock = 0;
    } else if (type == 2) {
        mJntCtrl.mbHeadLock = 0;
        mJntCtrl.mbBackBoneLock = 1;
    } else if (type == 3) {
        mJntCtrl.mbHeadLock = 1;
        mJntCtrl.mbBackBoneLock = 1;
    }
    return TRUE;
}
VERIFY(0x02231EBC, &daNpc_Ds1_c::evn_jnt_lock_init);

/* 02231F6C */
BOOL daNpc_Ds1_c::evn_player_hide_init(int staffId) {
    WWHD_FUNC(0x02231F6C, BOOL, this, staffId);
    s32* p = evt_int(staffId, 0x10019C3C);
    u32 type = p != nullptr ? ldi(p) : 0;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (type < 1)
        daPy_offNoDraw(player);
    else if (type == 1)
        daPy_onNoDraw(player);
    return TRUE;
}
VERIFY(0x02231F6C, &daNpc_Ds1_c::evn_player_hide_init);

/* 02232000 */
BOOL daNpc_Ds1_c::evn_talk() {
    WWHD_FUNC(0x02232000, BOOL, this);
    u32 mng = l_msgMng_ds1();
    if (*l_msgId == -1) {
        u32 id = fopMsgM_messageSet(mng, mMsgNo, &eyePos);
        *l_msgId = id;
        if (id != 0xFFFFFFFF)
            fopMsgM_demoMsgFlagOn();
        return FALSE;
    }
    setAnmFromMsgTag();
    if (fopMsgM_getStatus(mng) == 0xE) {
        u32 st = gabi::call<u32>(0x0222FECC, this, &mMsgNo, 0); /* next_msgStatus(&mMsgNo, NULL): the whole r3 is passed on */
        fopMsgM_setStatus(mng, st);
        if (fopMsgM_getStatus(mng) == 0xF)
            fopMsgM_messageSet(mng, mMsgNo, nullptr);
        return FALSE;
    }
    if (fopMsgM_getStatus(mng) == 0x12) {
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DCA) {
            u8 itemNo = mItemNo;
            if (itemNo == 0x49)
                dComIfGp_addItemBeastNumCount(4, -10);
            else if (itemNo == 0x4A)
                dComIfGp_addItemBeastNumCount(5, -15);
            else
                dComIfGp_addItemBeastNumCount(6, -15);
        } else if (msgNo == 0x1DC4) {
            u8 itemNo = mItemNo;
            if (itemNo == 0x49)
                dComIfGp_addItemBeastNumCount(4, -5);
            else
                dComIfGp_addItemBeastNumCount(itemNo == 0x4A ? 5 : 6, -5);
        }
        fopMsgM_setStatus(mng, 0x13);
        *l_msgId = -1;
        return TRUE;
    }
    if (fopMsgM_getStatus(mng) == 2 || fopMsgM_getStatus(mng) == 6) {
        if (mMsgNo == m9A0) {
            m9A0 = 0;
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02232000, &daNpc_Ds1_c::evn_talk);

static inline f32 endFrameM1(mDoExt_McaMorf* m) { return (f32)(s32)m->mFrameCtrl.mEnd - 1.0f; }

/* 022322D0 */
BOOL daNpc_Ds1_c::evn_Anm() {
    WWHD_FUNC(0x022322D0, BOOL, this);
    BOOL pass = mpMorf->mFrameCtrl.checkPass(endFrameM1(mpMorf));
    s8 cnt = mA4E;
    if (pass) {
        cnt = (s8)(cnt - 1);
        mA4E = cnt;
    }
    if (cnt <= 0)
        return TRUE;
    s8 anm = mAnmIdx;
    if (anm == 6) {
        J3DModel* model = mpMorf->getModel();
        s32 jnt = mJntNo[2];
        u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
        u32 mtx = gabi::load<u32>(blk + 0x10);
        gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
        Mtx34* stk = mDoMtx_stack_c::get();
        PSMTXCopy(gabi::at<Mtx34>(jnt * 0x30 + mtx), stk);
        u32 e0 = m8D8;
        f32 x = stk->m[0][3], z = stk->m[2][3], y = stk->m[1][3];
        if (e0 != 0) {
            f32 yy = y;
            if (gabi::load<u8>(e0 + 0x262) >= 7)
                yy = -yy;
            gabi::store<f32>(e0 + 0x22C, x);
            gabi::store<f32>(e0 + 0x230, yy);
            gabi::store<f32>(e0 + 0x234, z);
        }
        u32 e1 = m8DC;
        if (e1 != 0) {
            f32 yy = y;
            if (gabi::load<u8>(e1 + 0x262) >= 7)
                yy = -yy;
            gabi::store<f32>(e1 + 0x22C, x);
            gabi::store<f32>(e1 + 0x230, yy);
            gabi::store<f32>(e1 + 0x234, z);
        }
        if (mpMorf->mFrameCtrl.checkPass(endFrameM1(mpMorf))) {
            if (m8D8 != 0) {
                JPABaseEmitter_becomeInvalidEmitter_l(m8D8);
                m8D8 = 0;
            }
            if (m8DC != 0) {
                JPABaseEmitter_becomeInvalidEmitter_l(m8DC);
                m8DC = 0;
            }
        }
        if (mpMorf->mFrameCtrl.checkPass(135.0f)) {
            gabi::Local<cXyz> dir;
            dir->y = 1.0f;
            dir->x = 0.0f;
            dir->z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x11, dir);
            mDoAud_seStart(0x69A0, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
            mLight.mPower = 100.0f;
            return FALSE;
        }
        if (mpMorf->mFrameCtrl.checkPass(10.0f))
            mItemModelFlags = mItemModelFlags | 2;
    } else if (anm == 8) {
        if (mpMorf->mFrameCtrl.checkPass(128.0f))
            mItemModelFlags = mItemModelFlags | 1;
    }
    return FALSE;
}
VERIFY(0x022322D0, &daNpc_Ds1_c::evn_Anm);

/* 022325D0 */
BOOL daNpc_Ds1_c::evn_move_pos() {
    WWHD_FUNC(0x022325D0, BOOL, this);
    cLib_addCalcPos2(&current.pos, &mMovePos, 0.25f, 5.0f);
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> v;
    cXyz_mi(&mMovePos, d, &current.pos);
    u32 x = gabi::load<u32>(d.a), y = gabi::load<u32>(d.a + 4), z = gabi::load<u32>(d.a + 8);
    gabi::store<u32>(v.a + 4, y);
    gabi::store<u32>(v.a + 8, z);
    gabi::store<u32>(v.a, x);
    if (std_sqrtf(PSVECSquareMag(v)) < 1.0f)
        return TRUE;
    s16 angle = cLib_targetAngleY(&current.pos, &mMovePos);
    mMoveAngle = angle;
    cLib_addCalcAngleS2(&current.angle.y, angle, 4, 0x1000);
    return FALSE;
}
VERIFY(0x022325D0, &daNpc_Ds1_c::evn_move_pos);

/* 02232698 */
BOOL daNpc_Ds1_c::privateCut() {
    WWHD_FUNC(0x02232698, BOOL, this);
    s32 staffId = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
    if (staffId == -1)
        return FALSE;
    s32 actIdx = dComIfGp_evmng_getMyActIdx_l(staffId, 0x101BE1F4 /* cut names */, 10, 1, 0);
    if (actIdx == -1) {
        cutEnd(staffId);
        return TRUE;
    }
    if (dComIfGp_evmng_getIsAddvance_l(staffId)) {
        switch (actIdx) {
        case 0: evn_talk_init(staffId); break;
        case 1: evn_continue_talk_init(staffId); break;
        case 2: evn_ItemModel_init(staffId); break;
        case 3: evn_head_swing_init(staffId); break;
        case 5: evn_setAnm_init(staffId); break;
        case 6: evn_move_pos_init(staffId); break;
        case 7: evn_init_pos_init(staffId); break;
        case 8: evn_jnt_lock_init(staffId); break;
        case 9: evn_player_hide_init(staffId); break;
        }
    }
    BOOL end;
    if ((u32)actIdx <= 1)
        end = evn_talk();
    else if (actIdx == 5)
        end = evn_Anm();
    else if (actIdx == 6)
        end = evn_move_pos();
    else
        end = TRUE;
    if (end)
        cutEnd(staffId);
    return TRUE;
}
VERIFY(0x02232698, &daNpc_Ds1_c::privateCut);

/* 022328E4 */
BOOL daNpc_Ds1_c::event_action(void* arg) {
    WWHD_FUNC(0x022328E4, BOOL, this, arg);
    s8 status = mActionStatus;
    if (status == 0) {
        dComIfGp_evmng_getMyStaffId(STR(0x10019CB0), nullptr, 0);
        daPy_offNoDraw(dComIfGp_getPlayer(0));
        s8 st = mActionStatus;
        mShopCam.mCurrActionFunc.idx = 0;
        mA5C = mA5D;
        mShopCam.mCurrActionFunc.delta = 0;
        mShopCam.mCurrActionFunc.fn = 0;
        mOrderType = 0;
        mActionStatus = (s8)(st + 1);
        return TRUE;
    }
    if (status == -1)
        return TRUE;
    privateCut();
    if (dComIfGp_evmng_endCheck(mEventIdx[mA50])) {
        s32 a50 = mA50;
        if (a50 == 2) {
            mOrderType = 4;
            mA50 = 0;
        } else if (a50 == 3) {
            if (mMsgNo == 0x1DC4) {
                mOrderType = 3;
                mA50 = 4;
            }
        } else if (a50 == 0) {
            mOrderType = 1;
            m9A4 = 0x1DCB;
            mA50 = 4;
        }
        dComIfGp_event_reset();
        ds1_setAction(this, DS1_wait_action, nullptr);
    }
    lookBack();
    return TRUE;
}
VERIFY(0x022328E4, &daNpc_Ds1_c::event_action);
