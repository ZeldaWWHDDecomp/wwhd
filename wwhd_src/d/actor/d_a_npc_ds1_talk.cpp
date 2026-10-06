/**
 * d_a_npc_ds1_talk.cpp (WWHD)
 * NPC - Doc Bandam: messages, talk, look-back and the wait action.
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#include "d/actor/d_a_npc_ds1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD messages: the manager (*101F4B5C); +0x922 u8 (a trigger was handled), +0x938 the current
 * message number, +0x948 a flag */
static inline u32 l_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_SearchByID(u32 mng) { return gabi::call<u32>(0x025F795C, mng); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
/* 025F74D0: sets the message status (HD, on the manager) */
static inline void fopMsgM_setStatus(u32 mng, u32 status) { gabi::call(0x025F74D0, mng, status); }
static inline void fopMsgM_demoMsgFlagOn() { gabi::call(0x025DB58C); }
/* 020078BC: the A button triggered on pad n (CPad_CHECK_TRIG_A) */
static inline BOOL CPad_CHECK_TRIG_A(s32 n) { return gabi::call<BOOL>(0x020078BC, n); }
static inline u8 dComIfGs_getBeastNum(s32 i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xBC + i); }
static inline BOOL dComIfGs_checkEmptyBottle() { return gabi::call<BOOL>(0x025B5C54, gabi::load<u32>(0x101F84DC) + 0x5C); }
static inline void dComIfGs_onEventBit_ds1(u16 f) {
    dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
static inline u8 dShop_BoughtErrorStatus(ShopItems_c_l* s, s32 a, s32 price) { return gabi::call<u8>(0x025BBA1C, s, a, price); }
static inline BOOL dShop_now_triggercheck(STControl_l* st, ShopItems_c_l* s, be<u32>* msgNo, u32 cb, u32 arg) {
    return gabi::call<BOOL>(0x025BB698, st, s, msgNo, cb, arg);
}
static inline u8 ShopItems_getSelectItemNo(ShopItems_c_l* s) { return gabi::call<u8>(0x025BD22C, s); }
static inline void ShopItems_hideSelectItem(ShopItems_c_l* s) { gabi::call(0x025BCD90, s); }
static inline void ShopItems_showItem(ShopItems_c_l* s) { gabi::call(0x025BCE04, s); }
static inline void ShopItems_Item_ZoomUp(ShopItems_c_l* s, cXyz* p) { gabi::call(0x025BCC7C, s, p); }
static inline u32 ShopItems_getSelectItemBuyMsg(ShopItems_c_l* s) { return gabi::call<u32>(0x025BD278, s); }
/* getSelectItemPos / getSelectItemBasePos return a cXyz through a hidden result pointer */
static inline void ShopItems_getSelectItemPos(ShopItems_c_l* s, cXyz* out) { gabi::call(0x025BD0EC, s, out); }
static inline void ShopItems_getSelectItemBasePos(ShopItems_c_l* s, cXyz* out) { gabi::call(0x025BCFA8, s, out); }
static inline void ShopCursor_setPos(u32 c, cXyz* p) { gabi::call(0x025BD31C, c, p); }
static inline void ShopCursor_anm_play(u32 c) { gabi::call(0x025BD290, c); }
/* ShopCursor_c (HD): show/hide flag byte at +0xB4; setScale's m48/m4C/m50 at +0xA8.., m38/m3C at +0x98.. */
static inline void ShopCursor_setShow(u32 c, u8 v) { gabi::store<u8>(c + 0xB4, v); }
static inline BOOL checkItemGet(u8 item, s32 p) { return gabi::call<BOOL>(0x0254DA50, item, p); }
static inline void execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
static inline bool cXyz_normalizeRS(cXyz* v) { return gabi::call<bool>(0x0201B47C, v); }
static inline void dComIfGp_setNextStage_l(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint,
                                           s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, dst, eye, defY, maxVel, headOnly);
}
static inline BOOL J3DFrameCtrl_checkPass(J3DFrameCtrl* f, f32 frame) { return gabi::call<BOOL>(0x027F2BF8, f, frame); }
static inline void cp_int(cXyz* dst, cXyz* src) { /* GHS struct copy: integer words */
    u32 s = gabi::ea(src), d = gabi::ea(dst);
    gabi::store<u32>(d, gabi::load<u32>(s));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
}
#define l_msgId gabi::at<be<s32>>(0x10466EE0) /* the talk's message process id */

static BOOL daNpc_Ds1_shopMsgCheck(u32 msgNo) { return gabi::call<BOOL>(0x0222D958, msgNo); }
static BOOL daNpc_Ds1_checkCreateDrugChuchu(u8 itemNo) { return gabi::call<BOOL>(0x0222D37C, itemNo); }

/* ShopCam_action_c::getItemZoomPos(100.0f) (inline) into *out (a float copy) */
static void getItemZoomPos(daNpc_Ds1_c* t, cXyz* out) {
    gabi::Local<cXyz> dir;
    gabi::Local<cXyz> scaled;
    gabi::Local<cXyz> sum;
    cXyz_mi(&t->mShopCam.m18, dir, &t->mShopCam.m24);
    if (!cXyz_normalizeRS(dir))
        dir->set(0.0f, 0.0f, 1.0f);
    cXyz_ml(dir, scaled, 100.0f);
    cXyz_pl(&t->mShopCam.m24, sum, scaled);
    f32 x = sum->x, y = sum->y, z = sum->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

/* 0222FAC4 */
void daNpc_Ds1_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x0222FAC4, void, this);
    u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC5); /* dComIfGp_getMesgAnimeTagInfo() */
    switch (tag) {
    case 0: setAnm(0, -1.0f); setTexAnm(0); break;
    case 1: setAnm(1, -1.0f); setTexAnm(0); break;
    case 2: setAnm(2, -1.0f); setTexAnm(1); break;
    case 3: setAnm(3, -1.0f); setTexAnm(0); break;
    case 4: setAnm(4, -1.0f); setTexAnm(1); break;
    case 5: setAnm(9, -1.0f); setTexAnm(1); break;
    }
    if (mAnmIdx == 1) {
        mDoExt_McaMorf* m = mpMorf;
        if (J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mA4E + 1);
            mA4E = n;
            if (n > 1) {
                setAnm(0, -1.0f);
                mA4E = 0;
            }
        }
    }
    if (mAnmIdx == 9) {
        mDoExt_McaMorf* m = mpMorf;
        if (J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mA4E + 1);
            mA4E = n;
            if (n > 4) {
                setAnm(0, -1.0f);
                mA4E = 0;
            }
        }
    }
    if (mAnmIdx == 3) {
        mDoExt_McaMorf* m = mpMorf;
        if (J3DFrameCtrl_checkPass(&m->mFrameCtrl, (f32)(f64)(s16)m->mFrameCtrl.mEnd - 1.0f)) {
            s8 n = (s8)(mA4E + 1);
            mA4E = n;
            if (n > 1) {
                setAnm(2, -1.0f);
                mA4E = 0;
            }
        }
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF);
}
VERIFY(0x0222FAC4, &daNpc_Ds1_c::setAnmFromMsgTag);

/* 0222FDBC */
bool daNpc_Ds1_c::chkAttention(cXyz* pos, s16 angle) {
    WWHD_FUNC(0x0222FDBC, bool, this, pos, angle);
    u32 play = dComIfGp_ea();
    daNpc_Ds1_childHIO_c& hio = gabi::at<daNpc_Ds1_HIO_c>(0x10466F30)->mChild[mType];
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + PLAY_PLAYER));
    f32 maxX = hio.m2C;
    f32 maxZ = hio.m30;
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff, pos);
    gabi::Local<cXyz> axisZ;
    gabi::Local<cXyz> axisX;
    axisZ->set(0.0f, 0.0f, 1.0f);
    axisX->set(1.0f, 0.0f, 0.0f);
    f32 z = PSVECDotProduct(axisZ, diff);
    f32 x = PSVECDotProduct(axisX, diff);
    bool ret = false;
    if (maxZ > z && maxX > std::fabs(x))
        ret = true;
    return ret;
}
VERIFY(0x0222FDBC, &daNpc_Ds1_c::chkAttention);

/* 0222FECC. HD: a second message-number pointer (may be NULL) */
u16 daNpc_Ds1_c::next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    WWHD_FUNC(0x0222FECC, u16, this, pMsgNo, pMsgNo2);
    u32 mng = l_msgMng();
    u32 msgNo = *pMsgNo;
    u16 next = 0xF;
    switch (msgNo) {
    case 0x1DB1: case 0x1DB2: case 0x1DB3: case 0x1DB5: case 0x1DB6: case 0x1DB7: case 0x1DB8:
    case 0x1DBB: case 0x1DBC: case 0x1DC9: case 0x1DCE:
        *pMsgNo = msgNo + 1;
        break;
    case 0x1DCF:
        *pMsgNo = 0x1DDE;
        break;
    case 0x1DC0:
        if (CPad_CHECK_TRIG_A(0)) {
            *pMsgNo = 0x1DDC;
            if (pMsgNo2 != nullptr)
                *pMsgNo2 = 0x1DDC;
            gabi::store<u8>(mng + 0x922, 1);
            gabi::store<u8>(dComIfGp_ea() + 0x5BD2, 1);
            gabi::store<u8>(dComIfGp_ea() + 0x5BD3, 0);
        } else {
            next = 0xE;
        }
        break;
    case 0x1DD1: case 0x1DD2: case 0x1DD3:
        if (CPad_CHECK_TRIG_A(0)) {
            *pMsgNo = 0x1DDC;
            if (pMsgNo2 != nullptr)
                *pMsgNo2 = 0x1DDC;
            gabi::store<u8>(mng + 0x922, 1);
            gabi::store<u8>(dComIfGp_ea() + 0x5BD2, 1);
            gabi::store<u8>(dComIfGp_ea() + 0x5BD3, 0);
        } else {
            *pMsgNo = *pMsgNo + 3;
        }
        break;
    case 0x1DC3:
        if (!daNpc_Ds1_checkCreateDrugChuchu(mItemNo)) {
            *pMsgNo = 0x1DC8;
        } else {
            u8 item = mItemNo;
            bool enough = false;
            if (item == 0x49) enough = dComIfGs_getBeastNum(4) >= 5;
            else if (item == 0x4A) enough = dComIfGs_getBeastNum(5) >= 5;
            else if (item == 0x4B) enough = dComIfGs_getBeastNum(6) >= 5;
            *pMsgNo = enough ? 0x1DC4 : 0x1DC7;
        }
        break;
    case 0x1DC4:
        if (dComIfGs_checkEmptyBottle())
            return 0x10;
        *pMsgNo = 0x1DC5;
        break;
    case 0x1DC8: {
        u8 item = mItemNo;
        bool enough = false;
        if (item == 0x49) enough = dComIfGs_getBeastNum(4) >= 10;
        else if (item == 0x4A) enough = dComIfGs_getBeastNum(5) >= 15;
        else if (item == 0x4B) enough = dComIfGs_getBeastNum(6) >= 15;
        *pMsgNo = enough ? 0x1DC9 : 0x1DCE;
        break;
    }
    case 0x1DCB:
        if (dComIfGs_checkEmptyBottle())
            return 0x10;
        *pMsgNo = 0x1DCC;
        break;
    case 0x1DD4: case 0x1DD5: case 0x1DD6:
        if (gabi::load<u8>(dComIfGp_ea() + 0x5BD3) != 0 || gabi::load<u32>(mng + 0x948) != 0) {
            *pMsgNo = *pMsgNo - 3;
        } else {
            s16 price = gabi::load<s16>(dComIfGp_ea() + 0x5BA4);
            u8 err = dShop_BoughtErrorStatus(&mShopItems, 0, price);
            if (err & 8) {
                *pMsgNo = 0x1DD7;
            } else if (err & 0x10) {
                *pMsgNo = 0x1DD8;
            } else if (err & 0x20) {
                *pMsgNo = 0x1DD9;
            } else {
                BOOL got = checkItemGet(ShopItems_getSelectItemNo(&mShopItems), 0);
                mDoAud_seStart(0x87F, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
                ShopItems_hideSelectItem(&mShopItems);
                m9A8 = ShopItems_getSelectItemNo(&mShopItems);
                if (got == 0) {
                    u32 p = dComIfGp_ea() + 0x5B48; /* dComIfGp_setItemRupeeCount(-price) */
                    gabi::store<s32>(p, gabi::load<s32>(p) - price);
                    mA50 = 4;
                    mOrderType = 3;
                    return 0x10;
                }
                execItemGet(ShopItems_getSelectItemNo(&mShopItems));
                u32 p = dComIfGp_ea() + 0x5B48;
                gabi::store<s32>(p, gabi::load<s32>(p) - price);
                *pMsgNo = 0x1DDA;
            }
        }
        break;
    case 0x1DC1: case 0x1DD7: case 0x1DD8: case 0x1DD9: case 0x1DDA: case 0x1DDB:
        *pMsgNo = 0x1DC0;
        break;
    case 0x1DB9:
        if (gabi::load<u32>(mng + 0x948) != 0)
            *pMsgNo = 0x1DBA;
        else
            *pMsgNo = 0x1DBB;
        break;
    default:
        return 0x10;
    }
    return next;
}
VERIFY(0x0222FECC, &daNpc_Ds1_c::next_msgStatus);

/* 02230364 */
u32 daNpc_Ds1_c::getMsg() {
    WWHD_FUNC(0x02230364, u32, this);
    u32 msgNo = m9A4;
    if (msgNo != 0) {
        m9A4 = 0;
        return msgNo;
    }
    if (dComIfGp_event_chkTalkXY()) {
        if ((u32)(mItemNo - 0x49) <= 2) {
            dComIfGs_onEventBit_ds1(0x1120);
            return 0x1DC3;
        }
        if (dComIfGs_isEventBit_ds1(0x1120))
            return 0x1DC2;
        return 0x1DDD;
    }
    msgNo = 0x1DB1;
    if (dComIfGs_isEventBit_ds1(0x1120))
        msgNo = 0x1DDB;
    return msgNo;
}
VERIFY(0x02230364, &daNpc_Ds1_c::getMsg);

/* 02230420 */
u16 daNpc_Ds1_c::normal_talk() {
    WWHD_FUNC(0x02230420, u16, this);
    u32 mng = l_msgMng();
    u16 status = (u16)fopMsgM_SearchByID(mng);
    if (status == 0xE) {
        fopMsgM_setStatus(mng, gabi::call<u32>(0x0222FECC, this, &mMsgNo, nullptr)); /* next_msgStatus: r3 passed on unmasked */
        if (fopMsgM_SearchByID(mng) == 0xF)
            fopMsgM_messageSet(mng, mMsgNo, nullptr);
    } else if (status == 0x12) {
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DCA) {
            u8 item = mItemNo;
            if (item == 0x49) {
                u32 p = dComIfGp_ea() + 0x5B78; /* dComIfGp_setItemBeastNumCount(4, -10) */
                gabi::store<s16>(p, (s16)(gabi::load<s16>(p) - 10));
            } else if (item == 0x4A) {
                u32 p = dComIfGp_ea() + 0x5B7A;
                gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + -15));
            } else {
                u32 p = dComIfGp_ea() + 0x5B7C;
                gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + -15));
            }
            msgNo = mMsgNo;
        } else if (msgNo == 0x1DC4) {
            u8 item = mItemNo;
            s32 i = item == 0x49 ? 4 : (item == 0x4A ? 5 : 6);
            u32 p = dComIfGp_ea() + 0x5B70 + 2 * i;
            gabi::store<s16>(p, (s16)(gabi::load<s16>(p) - 5));
            msgNo = mMsgNo;
        }
        if (msgNo == 0x1DCA) {
            mOrderType = 4;
        } else if (msgNo == 0x1DCB || msgNo == 0x1DC4) {
            mOrderType = 3;
        } else if (msgNo == 0x1DCD || msgNo == 0x1DCC) {
            u8 item = mItemNo;
            mOrderType = 0;
            if (item != 0x49)
                dComIfGs_onEventBit_ds1(item == 0x4A ? 0xD04 : 0xD02);
            dComIfGp_setNextStage_l(STR(0x10019BD0), 1, current.roomNo, -1, 0.0f, 0, 1, 0);
        }
        fopMsgM_setStatus(mng, 0x13);
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn();
    }
    gabi::Local<cXyz> zoom;
    getItemZoomPos(this, zoom);
    ShopItems_Item_ZoomUp(&mShopItems, zoom);
    ShopCursor_setShow(mpShopCursor, 0);
    return status;
}
VERIFY(0x02230420, &daNpc_Ds1_c::normal_talk);

/* 02230784 */
u16 daNpc_Ds1_c::shop_talk() {
    WWHD_FUNC(0x02230784, u16, this);
    u32 mng = l_msgMng();
    ShopCursor_setShow(mpShopCursor, 1);
    if (dShop_now_triggercheck(&mStick, &mShopItems, &mMsgNo, 0, 0)) {
        m99C = 0;
        m8A8 = 1;
    }
    u16 status = (u16)fopMsgM_SearchByID(mng);
    if (status == 0xE || status == 0xF) {
        if ((s8)m8A8 != 0 || gabi::load<u8>(mng + 0x922) != 0) {
            m8A8 = 0;
        } else {
            m99C = mMsgNo;
            fopMsgM_setStatus(mng, gabi::call<u32>(0x0222FECC, this, &m99C, &mMsgNo)); /* next_msgStatus */
            if (fopMsgM_SearchByID(mng) == 0xF)
                fopMsgM_messageSet(mng, m99C, nullptr);
        }
    } else if (status == 0x12) {
        fopMsgM_setStatus(mng, 0x13);
        u32 msgNo = mMsgNo;
        if (msgNo == 0x1DC4 || (msgNo >= 0x1DD4 && msgNo <= 0x1DD6))
            mOrderType = 3;
        mShopItems.mSelectedItemIdx = -1;
    } else if (status == 1) {
        fopMsgM_demoMsgFlagOn();
    }
    return status;
}
VERIFY(0x02230784, &daNpc_Ds1_c::shop_talk);

/* 022308E0 */
u16 daNpc_Ds1_c::talk() {
    WWHD_FUNC(0x022308E0, u16, this);
    u32 mng = l_msgMng();
    u16 ret = 0xFF;
    s8 st = mA61;
    if (st == 0) {
        *l_msgId = -1;
        mMsgNo = getMsg();
        mA61 = 1;
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        m99C = 0;
        m9A8 = 0xFF;
        return ret;
    }
    if (st == -1) {
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        return ret;
    }
    if (*l_msgId == -1) {
        if (dComIfGp_event_chkTalkXY() && !dComIfGp_evmng_ChkPresentEnd())
            return ret;
        s32 id = (s32)fopMsgM_messageSet(mng, mMsgNo, &eyePos);
        *l_msgId = id;
        if (id != -1) {
            if (mMsgNo != 0x1DC0)
                mA61 = 2;
            else
                mA61 = 3;
        }
        mShopCam.m54 = mShopItems.mSelectedItemIdx;
        return ret;
    }
    setAnmFromMsgTag();
    s8 st2 = mA61;
    if ((u32)(s32)st2 == 2)
        ret = normal_talk();
    else if ((u32)(s32)st2 == 3)
        ret = shop_talk();
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BD2) != 0) {
        u32 msgNo = gabi::load<u32>(mng + 0x938);
        mMsgNo = msgNo;
        if (msgNo - 0x1DD1 < 3 || msgNo == 0x1DC0) {
            if (msgNo == 0x1DC0) {
                s16 hide = mShopItems.mbIsHide;
                mShopItems.mSelectedItemIdx = -1;
                if (hide != 0)
                    ShopItems_showItem(&mShopItems);
            }
            mA61 = 3;
        } else {
            if (!daNpc_Ds1_shopMsgCheck(msgNo)) {
                s16 hide = mShopItems.mbIsHide;
                mShopItems.mSelectedItemIdx = -1;
                if (hide != 0)
                    ShopItems_showItem(&mShopItems);
            }
            mA61 = 2;
        }
    }
    mShopCam.m54 = mShopItems.mSelectedItemIdx;
    return ret;
}
VERIFY(0x022308E0, &daNpc_Ds1_c::talk);

/* 02230ADC */
void daNpc_Ds1_c::setAttention(u32 force) {
    WWHD_FUNC(0x02230ADC, void, this, force);
    if (force == 0 && mAttnSetCount >= 2)
        return;
    daNpc_Ds1_childHIO_c& hio = l_HIO_ds1().mChild[mType];
    u32 p = gabi::ea(this) + 0x390; /* attention_info.position */
    f32 y = mAttentionBasePos.y + hio.mNpc.m18;
    gabi::store<f32>(p + 8, mAttentionBasePos.z);
    gabi::store<f32>(p, mAttentionBasePos.x);
    gabi::store<f32>(p + 4, y);
}
VERIFY(0x02230ADC, &daNpc_Ds1_c::setAttention);

/* 02230B24 */
void daNpc_Ds1_c::lookBack() {
    WWHD_FUNC(0x02230B24, void, this);
    f32 eyeX = 0.0f, eyeY = 0.0f, eyeZ = 0.0f;
    cXyz* target = nullptr;
    s16 angleY = current.angle.y;
    gabi::Local<cXyz> dst;
    s8 st = mA5C;
    if ((u32)(s32)st == 1) {
        if (m8D0 != 0) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye, l_HIO_ds1().mChild[mType].mNpc.m00);
            cp_int(dst, eye);
            target = dst;
            eyeZ = current.pos.z;
            eyeX = current.pos.x;
            eyeY = eyePos.y;
            setTexAnm(0);
        } else {
            target = nullptr;
            setTexAnm(0);
        }
    } else if ((u32)(s32)st == 2) {
        if (mShopCam.mCurrActionFunc.idx == 0) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye, l_HIO_ds1().mChild[mType].mNpc.m00);
            eyeY = eyePos.y;
            cp_int(dst, eye);
            eyeZ = current.pos.z;
            eyeX = current.pos.x;
            target = dst;
        } else if (mShopItems.mSelectedItemIdx == -1) {
            getItemZoomPos(this, dst);
            eyeX = current.pos.x;
            eyeZ = current.pos.z;
            target = dst;
            eyeY = eyePos.y;
        } else {
            gabi::Local<cXyz> itemPos;
            gabi::Local<cXyz> basePos;
            ShopItems_getSelectItemPos(&mShopItems, itemPos);
            ShopItems_getSelectItemBasePos(&mShopItems, basePos);
            cp_int(dst, basePos);
            ShopCursor_setPos(mpShopCursor, itemPos);
            daNpc_Ds1_childHIO_c& hio = l_HIO_ds1().mChild[mType];
            u32 c = mpShopCursor;
            f32 s4 = hio.m40, s2 = hio.m38, s3 = hio.m3C, s1 = hio.mCursorScale, s5 = hio.m44;
            gabi::store<f32>(c + 0xA8, s1);
            gabi::store<f32>(c + 0xAC, s2);
            gabi::store<f32>(c + 0xB0, s3);
            gabi::store<f32>(c + 0x98, s4);
            gabi::store<f32>(c + 0x9C, s5);
            ShopCursor_anm_play(mpShopCursor);
            eyeX = current.pos.x;
            eyeY = eyePos.y;
            eyeZ = current.pos.z;
            target = dst;
        }
    }
    if (mAction.idx == -1 && mAction.delta == 0 && mAction.fn == DS1_event_action) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, l_HIO_ds1().mChild[mType].mNpc.m00);
        eyeX = current.pos.x;
        cp_int(dst, eye);
        eyeY = eyePos.y;
        target = dst;
        eyeZ = current.pos.z;
    }
    gabi::Local<cXyz> eyeCopy;
    if (mJntCtrl.mbTrn) {
        cLib_addCalcAngleS2(&mHeadAngleY, l_HIO_ds1().mChild[mType].mNpc.m16, 4, 0x800);
        eyeCopy->x = eyeX;
        eyeCopy->y = eyeY;
        eyeCopy->z = eyeZ;
        lookAtTarget(&mJntCtrl, &current.angle.y, target, eyeCopy, angleY, mHeadAngleY, 1);
    } else {
        eyeCopy->x = eyeX;
        mHeadAngleY = 0;
        eyeCopy->y = eyeY;
        eyeCopy->z = eyeZ;
        lookAtTarget(&mJntCtrl, &current.angle.y, target, eyeCopy, angleY, 0, 1);
    }
}
VERIFY(0x02230B24, &daNpc_Ds1_c::lookBack);

/* 02230F2C */
BOOL daNpc_Ds1_c::wait01() {
    WWHD_FUNC(0x02230F2C, BOOL, this);
    if (m8D1 != 0) {
        u8 prev = mA5C;
        mA5C = 2;
        mA5D = prev;
        return TRUE;
    }
    if (m8D0 != 0) {
        mOrderType = 2;
        return TRUE;
    }
    if (mOrderType == 2)
        mOrderType = 0;
    return TRUE;
}
VERIFY(0x02230F2C, &daNpc_Ds1_c::wait01);

/* 02230F8C */
BOOL daNpc_Ds1_c::talk01() {
    WWHD_FUNC(0x02230F8C, BOOL, this);
    u16 status = talk();
    if (status == 0x12) {
        u32 play = dComIfGp_ea();
        u32 msgNo = mMsgNo;
        u8 prev = mA5D;
        fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + PLAY_PLAYER));
        mA5C = prev;
        if (msgNo == 0x1DCD || msgNo == 0x1DCC) {
            ds1_setAction(this, DS1_dummy_action, nullptr);
            m8D1 = 0;
            setAnm(0, -1.0f);
        } else {
            dComIfGp_event_reset();
            if (mOrderType != 3) {
                mShopCam.mCurrActionFunc.idx = 0;
                mShopCam.mCurrActionFunc.delta = 0;
                mShopCam.mCurrActionFunc.fn = 0;
            }
            u32 f = gabi::ea(player) + 0x3B8;
            gabi::store<u32>(f, gabi::load<u32>(f) & ~0x08000000u);
            m8D1 = 0;
            setAnm(0, -1.0f);
        }
        if (mMsgNo == 0x1DB4)
            dComIfGs_onEventBit_ds1(0x1120);
    } else if (status == 8) {
        if (daNpc_Ds1_shopMsgCheck(mMsgNo))
            ShopItems_getSelectItemBuyMsg(&mShopItems);
    }
    if (gabi::load<u8>(mpShopCursor + 0xB4) != 0) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BBA, 0x17);
        gabi::store<u8>(dComIfGp_ea() + 0x5BB9, 0x27);
    }
    return gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0) < 1.0f;
}
VERIFY(0x02230F8C, &daNpc_Ds1_c::talk01);

/* 022311F4 */
BOOL daNpc_Ds1_c::wait_action(void*) {
    WWHD_FUNC(0x022311F4, BOOL, this, (void*)nullptr);
    if (mActionStatus == 0) {
        mA5C = 1;
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != -1) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        s16 a = (s16)(current.angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]);
        pos->z = current.pos.z;
        pos->y = current.pos.y;
        m8D0 = (u8)gabi::call<u32>(0x0222FDBC, this, pos.get(), a); /* chkAttention: the result byte stored as is */
        s8 st = mA5C;
        BOOL r;
        if ((u32)(s32)st == 1) {
            r = wait01();
        } else if ((u32)(s32)st == 2) {
            r = talk01();
        } else {
            r = 0;
        }
        lookBack();
        setAttention(r);
    }
    return TRUE;
}
VERIFY(0x022311F4, &daNpc_Ds1_c::wait_action);
