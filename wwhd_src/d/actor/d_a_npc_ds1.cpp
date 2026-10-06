/**
 * d_a_npc_ds1.cpp (WWHD)
 * NPC - Doc Bandam (Windfall Potion Shop shopkeeper)
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names.
 * Part files: d_a_npc_ds1_talk.cpp (messages, talk, wait actions), d_a_npc_ds1_event.cpp (event
 * cuts and actions); d_a_npc_ds1_pending.cpp holds weak guest-call stubs.
 */
#include "d/actor/d_a_npc_ds1.h"

#define DS1_VTBL 0x100199D4 /* daNpc_Ds1_c vtable (HD) */
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x100199A4, 0x100199C4, 0x100199B4};
#define AAB_VTBL 0x10019994 /* this TU's cM3dGAab vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline BOOL mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                      s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline void mDoExt_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }
static inline u32 ShopCursor_create(J3DModelData* d, void* brk, f32 scale) { return gabi::call<u32>(0x025BBD7C, d, brk, scale); }
static inline void ShopCursor_draw(u32 c) { gabi::call(0x025BD338, c); }
static inline void ShopItems_setItemSetDataList(ShopItems_c_l* s) { gabi::call(0x025BCF84, s); }
static inline void ShopItems_createItem(ShopItems_c_l* s, s32 n, s32 roomNo) { gabi::call(0x025BC810, s, n, roomNo); }
static inline void ShopItems_Item_Move(ShopItems_c_l* s) { gabi::call(0x025BCCA4, s); }
static inline void ShopCam_shop_cam_action_init(ShopCam_action_c_l* c) { gabi::call(0x025BBE98, c); }
static inline void ShopCam_ds_normal_cam_action_init(ShopCam_action_c_l* c) { gabi::call(0x025BC438, c); }
static inline void ShopCam_move(ShopCam_action_c_l* c) { gabi::call(0x025BC714, c); }
static inline void dNpc_EventCut_setActorInfo(dNpc_EventCut_c* e, const char* name, fopAc_ac_c* a) { gabi::call(0x0259F7D4, e, name, a); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline s8 dBgS_GetRoomId_l(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor_l(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline void* gnd_poly(dBgS_Acch& a) { return gabi::at<u8>(gabi::ea(&a) + 0xD4 + 0x14); }
static inline BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb) { return gabi::call<BOOL>(0x025E535C, m, pos, se, reverb); }
static inline void dNpc_setAnm_2(mDoExt_McaMorf* m, s32 loopMode, f32 morf, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    gabi::call(0x0259D79C, m, loopMode, morf, speed, anmIdx, soundIdx, arc);
}
static inline void STControl_setWaitParm(STControl_l* s, s16 a, s16 b, s16 c, s16 d, f32 e, f32 f, s16 g, s16 h) {
    gabi::call(0x025885C4, s, a, b, c, d, e, f, g, h);
}
static inline void STControl_init(STControl_l* s) { gabi::call(0x025885E8, s); }
/* dComIfGp_getSelectItem(i): play+0x5BBB+i */
static inline u8 dComIfGp_getSelectItem(s32 i) { return gabi::load<u8>(dComIfGp_ea() + 0x5BBB + i); }
/* chu jellies in the beast bag (save +0xC0: red, +0xC1: green, +0xC2: blue) */
static inline u8 dComIfGs_getBeastNum(s32 i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xBC + i); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline f32 std_powf(f32 x, f32 y) { return gabi::call<f32>(0x028F4560, x, y); }
/* emitter->becomeInvalidEmitter() (HD: flags at +0x254, max frame at +0x5C) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
/* the player's virtual at +0x114 of its HD vtable: setPlayerPosAndAngle(cXyz*, s16) */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    gabi::call_ptr(gabi::load<u32>(pl->__vtbl + 0x114), pl, pos, angle);
}
static inline void J3DModel_copyAnmMtx(J3DModel* dst, J3DModel* src, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(src) + 0x2C);
    u32 m = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    mtx_copy(gabi::at<Mtx34>(gabi::ea(dst) + 0xC8), gabi::at<Mtx34>(jnt * 0x30 + m));
}

static inline Mtx34* J3DSys_mCurrentMtx_ds1() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
/* l_se_pos (0x10466EF0): the room's sound positions (__sinit) */
#define l_se_pos(i) gabi::at<cXyz>(0x10466EF0 + 0xC * (i))
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101BE138)
#define l_btp_ix(i) gabi::load<s32>(0x10019970 + 4 * (i)) /* btp resource of each tex anm */
#define l_anm_loop(i) gabi::load<s32>(0x101BE17C + 4 * (i))
#define l_anm_morf(i) gabi::load<f32>(0x101BE1A4 + 4 * (i))
#define l_anm_speed(i) gabi::load<f32>(0x101BE1CC + 4 * (i))
#define l_anm_ix(i) gabi::load<s32>(0x10019A04 + 4 * (i))

/* 0222D37C */
static BOOL daNpc_Ds1_checkCreateDrugChuchu(u8 itemNo) {
    WWHD_FUNC(0x0222D37C, BOOL, itemNo);
    if (itemNo == 0x49 /* red potion */)
        return TRUE;
    if (itemNo == 0x4A /* green potion */) {
        if (dComIfGs_isEventBit_ds1(0xD04))
            return TRUE;
        return FALSE;
    }
    if (itemNo == 0x4B /* blue potion */ && dComIfGs_isEventBit_ds1(0xD02))
        return TRUE;
    return FALSE;
}
VERIFY(0x0222D37C, daNpc_Ds1_checkCreateDrugChuchu);

/* 0222D3FC */
s16 daNpc_Ds1_c::XyEventCB(int i) {
    WWHD_FUNC(0x0222D3FC, s16, this, i);
    s16 ret = -1;
    u8 itemNo = dComIfGp_getSelectItem(i);
    if ((u32)(itemNo - 0x49) <= 2) {
        bool enough = false;
        if (!daNpc_Ds1_checkCreateDrugChuchu(itemNo)) {
            if (itemNo == 0x49) {
                if (dComIfGs_getBeastNum(4) >= 10) enough = true;
            } else if (itemNo == 0x4A) {
                if (dComIfGs_getBeastNum(5) >= 15) enough = true;
            } else if (itemNo == 0x4B) {
                if (dComIfGs_getBeastNum(6) >= 15) enough = true;
            }
        }
        if (enough) {
            mOrderType = 5;
            ret = mEventIdx[2];
            mA50 = 2;
            ds1_setAction(this, DS1_event_action, nullptr);
        } else if (!daNpc_Ds1_checkCreateDrugChuchu(itemNo)) {
            mOrderType = 6;
            ret = mEventIdx[3];
            mA50 = 3;
            ds1_setAction(this, DS1_event_action, nullptr);
        }
        mItemNo = itemNo;
    } else {
        mA50 = 4;
    }
    return ret;
}
VERIFY(0x0222D3FC, &daNpc_Ds1_c::XyEventCB);

/* 0222D6C0 */
static s16 daNpc_Ds1_XyEventCB(void* i_this, int i) {
    WWHD_FUNC(0x0222D6C0, s16, i_this, i);
    return ((daNpc_Ds1_c*)i_this)->XyEventCB(i);
}
VERIFY(0x0222D6C0, daNpc_Ds1_XyEventCB);

/* 0222D6C4 */
static BOOL nodeCallBack_Ds(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0222D6C4, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpc_Ds1_c* i_this = gabi::at<daNpc_Ds1_c>(gabi::load<u32>(model + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
        if (i_this != nullptr) {
            u32 blk = gabi::load<u32>(model + 0x2C);
            gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
            PSMTXCopy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30), calc_mtx());
            if (jntNo == (u32)(s32)i_this->mJntNo[0]) {
                gabi::Local<cXyz> offs;
                gabi::Local<cXyz> pos;
                offs->x = 0.0f;
                offs->y = 0.0f;
                offs->z = 0.0f;
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->mJntCtrl.mAngles[0][1]);
                mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->mJntCtrl.mAngles[0][0]);
                MtxPosition(offs, pos);
                i_this->mAttentionBasePos.z = pos->z;
                i_this->mAttentionBasePos.y = pos->y;
                i_this->mAttentionBasePos.x = pos->x;
                offs->x = 28.0f;
                offs->z = 0.0f;
                offs->y = 20.0f;
                MtxPosition(offs, pos);
                i_this->eyePos.x = pos->x;
                i_this->eyePos.z = pos->z;
                i_this->eyePos.y = pos->y;
                if (i_this->mAttnSetCount != 0xFF)
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
            } else if (jntNo == (u32)(s32)i_this->mJntNo[1]) {
                mDoMtx_XrotM(calc_mtx(), i_this->mJntCtrl.mAngles[1][1]);
                mDoMtx_ZrotM(calc_mtx(), i_this->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx_ds1());
            blk = gabi::load<u32>(model + 0x2C);
            gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
            mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30), calc_mtx());
        }
    }
    return TRUE;
}
VERIFY(0x0222D6C4, nodeCallBack_Ds);

/* 0222D958 */
static BOOL daNpc_Ds1_shopMsgCheck(u32 msgNo) {
    WWHD_FUNC(0x0222D958, BOOL, msgNo);
    if (msgNo - 0x1DD1 < 10 || msgNo == 0x1DC0)
        return TRUE;
    return FALSE;
}
VERIFY(0x0222D958, daNpc_Ds1_shopMsgCheck);

/* 0222D97C */
BOOL daNpc_Ds1_c::initTexPatternAnm(u8 modify) {
    WWHD_FUNC(0x0222D97C, BOOL, this, modify);
    J3DModelData* modelData = J3DModel_getModelData(mpMorf->getModel());
    s32 resIdx = l_btp_ix(mTexAnmIdx);
    mpBtp = (J3DAnmTexPattern*)ds1_getRes(resIdx);
    if (mpBtp.get() == nullptr)
        JUT_ASSERT_fail(STR(0x10019A3C), 0x1FE, STR(0x10019A4C));
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, mpBtp, TRUE, 2, 1.0f, 0, -1, modify, 0))
        return FALSE;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x0222D97C, &daNpc_Ds1_c::initTexPatternAnm);

/* 0222DA90 */
BOOL daNpc_Ds1_c::CreateHeap() {
    WWHD_FUNC(0x0222DA90, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)ds1_getRes(0x13);
    J3DAnmTransform* bck = (J3DAnmTransform*)ds1_getRes(0x23);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0,
                                                  0x11020203);
    if (morf == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    mpMorf = morf;
    if (morf->mpModel.get() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    mJntNo[0] = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10019A7C));
    if (mJntNo[0] < 0)
        JUT_ASSERT_fail(STR(0x10019A84), 0xA2B, STR(0x10019AAC));
    mJntNo[1] = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10019AC0));
    if (mJntNo[1] < 0)
        JUT_ASSERT_fail(STR(0x10019A84), 0xA2E, STR(0x10019A94));
    mJntNo[2] = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10019A6C));
    if (mJntNo[2] < 0)
        JUT_ASSERT_fail(STR(0x10019A84), 0xA31, STR(0x10019ACC));
    mJntNo[3] = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10019A74));
    if (mJntNo[3] < 0)
        JUT_ASSERT_fail(STR(0x10019A84), 0xA33, STR(0x10019AE4));
    if (mType == 0)
        mTexAnmIdx = 1;
    if (!initTexPatternAnm(false))
        return FALSE;

    J3DModelData* btkData = (J3DModelData*)ds1_getRes(0x14);
    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)ds1_getRes(0x1B);
    mpBtkModel = mDoExt_J3DModel__create(btkData, 0, 0x11020203);
    mBtkAnm.init(btkData, btk, true, 2, 1.0f, 0, -1, false, 0);
    mbRoomEffect = 1;

    J3DModelData* item0 = (J3DModelData*)ds1_getRes(0x17);
    J3DModelData* item1 = (J3DModelData*)ds1_getRes(0x18);
    if (item0 == nullptr || item1 == nullptr)
        return FALSE;
    mpItemModel[0] = mDoExt_J3DModel__create(item0, 0, 0x11020203);
    mpItemModel[1] = mDoExt_J3DModel__create(item1, 0, 0x11020203);
    if (mpItemModel[0].get() == nullptr || mpItemModel[1].get() == nullptr)
        return FALSE;

    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)mJntNo[0] || i == (u32)(s32)mJntNo[1]) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack_Ds) */
            u32 data = gabi::load<u32>(gabi::ea(mpMorf->getModel()) + 0xAC);
            u32 n = gabi::load<u32>(data + 4);
            u32 p = gabi::load<u32>(data + 8);
            if (i < n)
                p += i * 0x1C;
            gabi::store<u32>(p + 8, 0x0222D6C4);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);

    void* brk = ds1_getRes(0xC);
    J3DModelData* cursorData = (J3DModelData*)ds1_getRes(9);
    mpShopCursor = ShopCursor_create(cursorData, brk, l_HIO_ds1().mChild[mType].mCursorScale);
    if (mpShopCursor == 0)
        return FALSE;
    return TRUE;
}
VERIFY(0x0222DA90, &daNpc_Ds1_c::CreateHeap);

/* 0222DF74 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0222DF74, BOOL, i_this);
    return ((daNpc_Ds1_c*)i_this)->CreateHeap();
}
VERIFY(0x0222DF74, CheckCreateHeap);

/* 0222DF78 daNpc_Ds1_c::daNpc_Ds1_c (HD: allocates when this == NULL) */
static daNpc_Ds1_c* daNpc_Ds1_c_ct(daNpc_Ds1_c* i_this) {
    WWHD_FUNC(0x0222DF78, daNpc_Ds1_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ds1_c*)operator_new(0xA68);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = DS1_VTBL;
    mDoExt_btpAnm_ct(i_this->mBtpAnm);
    dBgS_ObjAcch_ct(&i_this->mAcch, OBJACCH_VT);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, AAB_VTBL);
    gabi::call(0x0259F740, &i_this->mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
    gabi::call(0x0259DAA0, &i_this->mJntCtrl);  /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    i_this->m852 = 0;
    i_this->m86A = 0;
    i_this->m864 = 0.0f;
    i_this->m850 = 0;
    i_this->m868 = 0;
    i_this->m860 = 0.0f;
    i_this->m854 = 0;
    i_this->m86C = 0;
    i_this->mStick.__vtbl = 0x10050788;
    STControl_setWaitParm(&i_this->mStick, 15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    STControl_init(&i_this->mStick);
    mDoExt_btkAnm::ct(&i_this->mBtkAnm);
    i_this->mLight.mHD20 = 1.0f;
    gabi::call(0x025BBDF0, &i_this->mShopCam);   /* ShopCam_action_c::ShopCam_action_c */
    gabi::call(0x025BC760, &i_this->mShopItems); /* ShopItems_c::ShopItems_c */
    return i_this;
}
VERIFY(0x0222DF78, daNpc_Ds1_c_ct);

/* 0222E124: a colour channel, gamma-corrected (HD) */
static f32 daNpc_Ds1_gamma(u32 c) {
    WWHD_FUNC(0x0222E124, f32, c);
    f32 v = std_powf((f32)(f64)c / 255.0f, 2.2f);
    if (v < 0.0f)
        return 0.0f;
    if (v > 1.0f)
        return 1.0f;
    return v;
}
VERIFY(0x0222E124, daNpc_Ds1_gamma);

static inline u8 gamma_u8(u8 c) { return (u8)gabi::ftoi(daNpc_Ds1_gamma(c) * 255.0f); }

/* 0222E1A4 */
void daNpc_Ds1_c::RoomEffectSet() {
    WWHD_FUNC(0x0222E1A4, void, this);
    gabi::Local<GXColor> col0;
    gabi::Local<GXColor> col1;
    gabi::Local<GXColor> col2;
    gabi::Local<cXyz> pos0;
    gabi::Local<cXyz> pos1;
    gabi::Local<cXyz> pos2;
    gabi::Local<cXyz> pos3;
    pos1->set(-158.0f, 160.0f, -663.0f);
    pos2->set(18.0f, 160.0f, -652.0f);
    pos3->set(-220.0f, 105.0f, -590.0f);
    pos0->x = cXyz_Zero->x;
    pos0->y = cXyz_Zero->y;
    pos0->z = cXyz_Zero->z;
    col0->r = 0; col0->g = 0; col0->b = 0; col0->a = 0xFF;
    col1->r = 0; col1->g = 0; col1->b = 0; col1->a = 0xFF;
    col2->r = 0; col2->g = 0; col2->b = 0; col2->a = 0xFF;
    col0->r = gamma_u8(0x41);
    col0->g = gamma_u8(0x4D);
    col0->b = gamma_u8(0x6F);
    col1->r = gamma_u8(0x90);
    col1->g = gamma_u8(0x8D);
    col1->b = gamma_u8(0x56);
    col2->r = gamma_u8(0x78);
    col2->g = gamma_u8(0x46);
    col2->b = gamma_u8(0x5A);
    mpRoomEmitter[0] = dComIfGp_particle_set(0x81D0, pos0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    mpRoomEmitter[1] = dComIfGp_particle_set(0x81D2, pos0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    mpRoomEmitter[2] = dComIfGp_particle_set(0x81D3, pos0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    mpRoomEmitter[3] = dComIfGp_particle_set(0x81D4, pos0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    mpRoomEmitter[4] = dComIfGp_particle_set(0x81D1, pos1, nullptr, nullptr, 0xFF, nullptr, -1, col0, nullptr, nullptr);
    JPABaseEmitter* e = dComIfGp_particle_set(0x81D1, pos2, nullptr, nullptr, 0xFF, nullptr, -1, col1, nullptr, nullptr);
    mpRoomEmitter[5] = e;
    if (e != nullptr)
        gabi::store<s16>(gabi::ea(e) + 0x60, 13);
    mpRoomEmitter[6] = dComIfGp_particle_set(0x81D1, pos3, nullptr, nullptr, 0xFF, nullptr, -1, col2, nullptr, nullptr);
}
VERIFY(0x0222E1A4, &daNpc_Ds1_c::RoomEffectSet);

/* 0222E598 */
BOOL daNpc_Ds1_c::CreateInit() {
    WWHD_FUNC(0x0222E598, BOOL, this);
    mAngle.z = current.angle.z;
    mAngle.x = current.angle.x;
    gravity = -30.0f;
    mAngle.y = current.angle.y;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 10); /* attention_info.flags */
    if (mType == 0)
        ds1_setAction(this, DS1_wait_action, nullptr);
    /* mAttentionBasePos = current.pos (integer copy) */
    gabi::store<u32>(gabi::ea(&mAttentionBasePos) + 8, gabi::load<u32>(gabi::ea(&current.pos) + 8));
    gabi::store<u32>(gabi::ea(&mAttentionBasePos), gabi::load<u32>(gabi::ea(&current.pos)));
    gabi::store<u32>(gabi::ea(&mAttentionBasePos) + 4, gabi::load<u32>(gabi::ea(&current.pos) + 4));
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(l_cyl_src);
    mA4E = 0;
    m8A8 = 0;
    mShopCam.mCamDataIdx = 0;
    mCyl.SetStts(&mStts);
    mShopCam.mCurrActionFunc.delta = 0;
    mShopCam.mCurrActionFunc.idx = 0;
    m9A8 = 0xFF;
    mShopCam.mCurrActionFunc.fn = 0;
    mA62 = 0;
    m9A4 = 0;
    mShopItems.mItemSetListGlobalIdx = 0;
    ShopItems_setItemSetDataList(&mShopItems);
    ShopItems_createItem(&mShopItems, 3, current.roomNo);
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x0222D6C0); /* eventInfo.setXyEventCB(daNpc_Ds1_XyEventCB) */
    dNpc_EventCut_setActorInfo(&mEventCut, STR(0x10019B48), this);
    mEventCut.mpJntCtrl = &mJntCtrl;
    mA50 = 4;
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(STR(0x10019B4C), 0xFF);
    mEventIdx[1] = dComIfGp_evmng_getEventIdx(STR(0x10019B68), 0xFF);
    mEventIdx[2] = dComIfGp_evmng_getEventIdx(STR(0x10019B74), 0xFF);
    mEventIdx[3] = dComIfGp_evmng_getEventIdx(STR(0x10019B58), 0xFF);
    if (mbRoomEffect) {
        J3DModel_setBaseTRMtx(mpBtkModel, gabi::at<Mtx34>(0x101F48F0));
        RoomEffectSet();
    }
    mLight.mColorR = 0;
    mLight.mColorG = 0;
    mLight.mColorB = 0;
    mLight.mPos.x = -60.0f;
    mLight.mPower = 0.0f;
    mLight.mFluctuation = 20.0f;
    mLight.mPos.z = -555.0f;
    mLight.mPos.y = 123.0f;
    dKy_plight_set(&mLight);
    return TRUE;
}
VERIFY(0x0222E598, &daNpc_Ds1_c::CreateInit);

/* 0222E8F0 */
cPhs_State daNpc_Ds1_c::_create() {
    WWHD_FUNC(0x0222E8F0, cPhs_State, this);
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (this != nullptr)
            daNpc_Ds1_c_ct(this);
        actor_condition |= fopAcCnd_INIT_e;
    }
    cPhs_State rt = dComIfG_resLoad(&mPhs, DS1_ARC);
    if (rt == cPhs_COMPLEATE_e) {
        mType = 0;
        if (!fopAcM_entrySolidHeap(this, 0x0222DF74 /* CheckCreateHeap */, 0x8340))
            return cPhs_ERROR_e;
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
        if (l_HIO_ds1().mCount < 0)
            l_HIO_ds1().mNo = mDoHIO_createChild(STR(0x10019B80), &l_HIO_ds1());
        l_HIO_ds1().mCount = l_HIO_ds1().mCount + 1;
        if (!CreateInit())
            return cPhs_ERROR_e;
    }
    return rt;
}
VERIFY(0x0222E8F0, &daNpc_Ds1_c::_create);

/* 0222EA14 */
static cPhs_State daNpc_Ds1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0222EA14, cPhs_State, i_this);
    return ((daNpc_Ds1_c*)i_this)->_create();
}
VERIFY(0x0222EA14, daNpc_Ds1_Create);

/* 0222EA18. HD: the loop reads the seven pointers after the first emitter (mpRoomEmitter[1..6]
 * and mpBtkModel), not the seven emitters RoomEffectSet stores */
void daNpc_Ds1_c::RoomEffectDelete() {
    WWHD_FUNC(0x0222EA18, void, this);
    for (int i = 1; i <= 7; i++) {
        JPABaseEmitter* e = gabi::at<JPABaseEmitter>(gabi::load<u32>(gabi::ea(mpRoomEmitter) + 4 * i));
        if (e != nullptr)
            JPABaseEmitter_becomeInvalidEmitter(e);
    }
}
VERIFY(0x0222EA18, &daNpc_Ds1_c::RoomEffectDelete);

/* 0222EA54 */
BOOL daNpc_Ds1_c::_delete() {
    WWHD_FUNC(0x0222EA54, BOOL, this);
    dKy_plight_cut(&mLight);
    dComIfG_resDelete(&mPhs, DS1_ARC);
    if (heap.get() != nullptr && mpMorf.get() != nullptr)
        mpMorf->stopZelAnime();
    mDoAud_seDeleteObject(l_se_pos(0));
    mDoAud_seDeleteObject(l_se_pos(1));
    mDoAud_seDeleteObject(l_se_pos(2));
    mDoAud_seDeleteObject(l_se_pos(3));
    RoomEffectDelete();
    if (l_HIO_ds1().mCount >= 0) {
        l_HIO_ds1().mCount = l_HIO_ds1().mCount - 1;
        if (l_HIO_ds1().mCount < 0)
            mDoHIO_deleteChild(l_HIO_ds1().mNo);
    }
    return TRUE;
}
VERIFY(0x0222EA54, &daNpc_Ds1_c::_delete);

/* 0222EB18 */
static BOOL daNpc_Ds1_Delete(daNpc_Ds1_c* i_this) {
    WWHD_FUNC(0x0222EB18, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0222EB18, daNpc_Ds1_Delete);

/* 0222EB1C */
void daNpc_Ds1_c::playTexPatternAnm() {
    WWHD_FUNC(0x0222EB1C, void, this);
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = J3DAnm_getFrameMax(mpBtp);
        if ((s32)mBtpFrame >= frameMax0) {
            s32 frameMax = J3DAnm_getFrameMax(mpBtp);
            mBtpFrame = (u8)(mBtpFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBtpFrame = mBtpFrame + 1;
        }
    }
}
VERIFY(0x0222EB1C, &daNpc_Ds1_c::playTexPatternAnm);

/* 0222EBE0 */
void daNpc_Ds1_c::talkInit() {
    WWHD_FUNC(0x0222EBE0, void, this);
    mA61 = 0;
}
VERIFY(0x0222EBE0, &daNpc_Ds1_c::talkInit);

/* 0222EBEC */
void daNpc_Ds1_c::checkOrder() {
    WWHD_FUNC(0x0222EBEC, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        if (mOrderType == 3) {
            mShopCam.mCurrActionFunc.fn = 0;
            mShopCam.mCurrActionFunc.idx = 0;
            mOrderType = 0;
            mShopCam.mCurrActionFunc.delta = 0;
            ds1_setAction(this, DS1_getdemo_action, nullptr);
        } else if (mOrderType == 4) {
            mShopCam.mCurrActionFunc.fn = 0;
            mShopCam.mCurrActionFunc.idx = 0;
            mOrderType = 0;
            mShopCam.mCurrActionFunc.delta = 0;
            ds1_setAction(this, DS1_event_action, nullptr);
        } else if (mOrderType == 5) {
            mOrderType = 0;
        }
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrderType == 1 || mOrderType == 2) {
            if (dComIfGp_event_chkTalkXY()) {
                s32 a50 = mA50;
                mOrderType = 0;
                if (a50 != 2) {
                    m8D1 = 1;
                    talkInit();
                }
                mItemNo = dComIfGp_event_getPreItemNo();
            } else if (dComIfGs_isEventBit_ds1(0x1120)) {
                ShopCam_shop_cam_action_init(&mShopCam);
                gabi::Local<cXyz> pos;
                pos->y = 0.0f;
                pos->z = 100.0f;
                pos->x = 10.0f;
                daPy_setPlayerPosAndAngle(dComIfGp_getPlayer(0), pos, -0x6000);
                mOrderType = 0;
                m8D1 = 1;
                talkInit();
            } else {
                ShopCam_ds_normal_cam_action_init(&mShopCam);
                gabi::Local<cXyz> pos;
                pos->y = 0.0f;
                pos->z = 100.0f;
                pos->x = -115.0f;
                daPy_setPlayerPosAndAngle(dComIfGp_getPlayer(0), pos, -0x6000);
                mOrderType = 0;
                m8D1 = 1;
                talkInit();
            }
        }
    }
}
VERIFY(0x0222EBEC, &daNpc_Ds1_c::checkOrder);

/* 0222F050 */
void daNpc_Ds1_c::eventOrder() {
    WWHD_FUNC(0x0222F050, void, this);
    s8 order = mOrderType;
    if (order == 3) {
        fopAcM_orderOtherEventId(this, mEventIdx[1], 0xFF, 0xFFFF, 0, 1);
    } else if (order == 4) {
        fopAcM_orderOtherEventId(this, mEventIdx[0], 0xFF, 0xFFFF, 0, 1);
    } else if (order == 5) {
        fopAcM_orderOtherEventId(this, mEventIdx[2], 0xFF, 0xFFFF, 0, 1);
    } else if (order == 6) {
        fopAcM_orderOtherEventId(this, mEventIdx[3], 0xFF, 0xFFFF, 0, 1);
    } else if (order == 1 || order == 2) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mOrderType == 1)
            fopAcM_orderSpeakEvent(this);
    }
}
VERIFY(0x0222F050, &daNpc_Ds1_c::eventOrder);

/* 0222F100 */
void daNpc_Ds1_c::setCollision() {
    WWHD_FUNC(0x0222F100, void, this);
    gabi::Local<cXyz> offs;
    gabi::Local<cXyz> pos;
    offs->x = 0.0f;
    offs->y = 0.0f;
    offs->z = -16.0f;
    MtxTrans(current.pos.x, current.pos.y, current.pos.z, 0);
    mDoMtx_YrotM(calc_mtx(), mAngle.y);
    MtxPosition(offs, pos);
    mCyl.SetC(pos);
    mCyl.SetR(46.0f);
    mCyl.SetH(130.0f);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x0222F100, &daNpc_Ds1_c::setCollision);

/* 0222F1BC */
BOOL daNpc_Ds1_c::_execute() {
    WWHD_FUNC(0x0222F1BC, BOOL, this);
    bool inEvent = mAction.idx == -1 && mAction.delta == 0 && mAction.fn == DS1_event_action;
    daNpc_Ds1_childHIO_c& hio = l_HIO_ds1().mChild[mType];
    if (!inEvent) {
        be<s16>* p = hio.mNpc.mJntPrm;
        mJntCtrl.setParam(p[1], p[3], p[5], p[7], p[0], p[2], p[4], p[6], p[8]);
    } else {
        be<s16>* p = hio.mNpc.mJntPrm;
        mJntCtrl.setParam(p[1], 0x1C70, p[5], -0x38E0, p[0], 0x38E0, p[4], -0x1C70, p[8]);
    }
    playTexPatternAnm();
    if (mAcch.ChkGroundHit()) {
        u32 sndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd_poly(mAcch));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mAnmEnd = McaMorf_play(mpMorf, &eyePos, sndId, reverb);
    } else {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mAnmEnd = McaMorf_play(mpMorf, &eyePos, 0, reverb);
    }
    if (mpMorf->getFrame() < mPrevFrame)
        mAnmEnd = 1;
    mPrevFrame = mpMorf->getFrame();
    checkOrder();
    ds1_ptmf_call(&mAction, this, nullptr);
    ShopCam_move(&mShopCam);
    ShopItems_Item_Move(&mShopItems);
    eventOrder();
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts)));
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = dBgS_GetRoomId_l(dComIfG_Bgsp(), gnd_poly(mAcch));
    u8 polyColor = dBgS_GetPolyColor_l(dComIfG_Bgsp(), gnd_poly(mAcch));
    J3DModel* model = mpMorf->getModel();
    gabi::store<u8>(gabi::ea(this) + 0x1CA, polyColor); /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    setCollision();
    if (mbRoomEffect) {
        mDoAud_seStart(0x703C, l_se_pos(0), 0, dComIfGp_getReverb(current.roomNo));
        mDoAud_seStart(0x703D, l_se_pos(1), 0, dComIfGp_getReverb(current.roomNo));
        mDoAud_seStart(0x703E, l_se_pos(2), 0, dComIfGp_getReverb(current.roomNo));
        u8 t = mSeTimer;
        mSeTimer = (u8)(t + 1);
        if (t >= 50) {
            mDoAud_seStart(0x69A1, l_se_pos(3), 0, dComIfGp_getReverb(current.roomNo));
            mSeTimer = 0;
        }
        mBtkAnm.play();
    }
    cLib_addCalc0(&mLight.mPower, 0.25f, 20.0f);
    return TRUE;
}
VERIFY(0x0222F1BC, &daNpc_Ds1_c::_execute);

/* 0222F5CC */
static BOOL daNpc_Ds1_Execute(daNpc_Ds1_c* i_this) {
    WWHD_FUNC(0x0222F5CC, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0222F5CC, daNpc_Ds1_Execute);

/* 0222F5D0 */
BOOL daNpc_Ds1_c::_draw() {
    WWHD_FUNC(0x0222F5D0, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData(model);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)mBtpAnm, modelData, mBtpFrame);
    mpMorf->updateDL();
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* mBtpAnm.remove(modelData) */

    u8 inHand0 = mItemModelFlags & 1;
    setLightTevColorType(dKy_getEnvlight(), mpItemModel[0], &tevStr);
    if (inHand0) {
        J3DModel_copyAnmMtx(mpItemModel[0], model, mJntNo[2]);
    } else {
        mDoMtx_stack_c::transS(-90.0f, 95.0f, -580.0f);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), 0x76C, 0, 0);
        J3DModel_setBaseTRMtx(mpItemModel[0], mDoMtx_stack_c::get());
    }
    mDoExt_modelUpdateDL(mpItemModel[0], 0);

    u8 inHand1 = mItemModelFlags & 2;
    setLightTevColorType(dKy_getEnvlight(), mpItemModel[1], &tevStr);
    if (inHand1) {
        J3DModel_copyAnmMtx(mpItemModel[1], model, mJntNo[3]);
    } else {
        mDoMtx_stack_c::transS(-11.0f, 101.0f, -545.0f);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), -0x8000, 0x3C8C, 0);
        J3DModel_setBaseTRMtx(mpItemModel[1], mDoMtx_stack_c::get());
    }
    mDoExt_modelUpdateDL(mpItemModel[1], 0);

    if (mShopItems.mSelectedItemIdx >= 0)
        ShopCursor_draw(mpShopCursor);
    if (mbRoomEffect) {
        mDoExt_btkAnm_entry(&mBtkAnm, J3DModel_getModelData(mpBtkModel), mBtkAnm.getFrame());
        mDoExt_modelUpdateDL(mpBtkModel, 0);
    }
    gabi::call(0x025BEBB8, 0x5D, this, &current.pos, (s16)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
    return TRUE;
}
VERIFY(0x0222F5D0, &daNpc_Ds1_c::_draw);

/* 0222F9B4 */
static BOOL daNpc_Ds1_Draw(daNpc_Ds1_c* i_this) {
    WWHD_FUNC(0x0222F9B4, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0222F9B4, daNpc_Ds1_Draw);

/* 0222F9B8 */
static BOOL daNpc_Ds1_IsDelete(daNpc_Ds1_c*) {
    WWHD_FUNC(0x0222F9B8, BOOL, (daNpc_Ds1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0222F9B8, daNpc_Ds1_IsDelete);

/* 0222F9C0 */
void daNpc_Ds1_c::setAnm(s8 anmIdx, f32 morf) {
    WWHD_FUNC(0x0222F9C0, void, this, anmIdx, morf);
    if (anmIdx != mAnmIdx || mAnmIdx == -1) {
        mAnmIdx = anmIdx;
        if (morf < 0.0f)
            morf = l_anm_morf(anmIdx);
        dNpc_setAnm_2(mpMorf, l_anm_loop(anmIdx), morf, l_anm_speed(anmIdx), l_anm_ix(anmIdx), -1, DS1_ARC);
        u32 u = (u32)(s32)anmIdx;
        if (u == 4 || (u >= 6 && u <= 9)) {
            mJntCtrl.mbBackBoneLock = 1;
            mJntCtrl.mbHeadLock = 1;
        } else {
            mJntCtrl.mbHeadLock = 0;
            mJntCtrl.mbBackBoneLock = 0;
        }
    }
}
VERIFY(0x0222F9C0, &daNpc_Ds1_c::setAnm);

/* 0222FA9C */
u32 daNpc_Ds1_c::setTexAnm(s8 texIdx) {
    WWHD_FUNC(0x0222FA9C, u32, this, texIdx);
    if (texIdx != mTexAnmIdx || mTexAnmIdx == -1) {
        mTexAnmIdx = texIdx;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x0222FA9C, &daNpc_Ds1_c::setTexAnm);

/* 02232B24 daNpc_Ds1_childHIO_c::daNpc_Ds1_childHIO_c */
static daNpc_Ds1_childHIO_c* daNpc_Ds1_childHIO_c_ct(daNpc_Ds1_childHIO_c* p) {
    WWHD_FUNC(0x02232B24, daNpc_Ds1_childHIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Ds1_childHIO_c*)operator_new(0x4C);
        if (p == nullptr)
            return p;
    }
    p->__vtbl = 0x100199E4;
    gabi::call(0x0259DA18, &p->mNpc); /* dNpc_HIO_c */
    p->m38 = 0.0f;
    p->m2C = 0.0f;
    p->m3C = 0.0f;
    p->mCursorScale = 0.0f;
    p->m30 = 0.0f;
    p->m44 = 0.0f;
    p->m40 = 0.0f;
    return p;
}
VERIFY(0x02232B24, daNpc_Ds1_childHIO_c_ct);

/* 02232B9C daNpc_Ds1_HIO_c::daNpc_Ds1_HIO_c */
static daNpc_Ds1_HIO_c* daNpc_Ds1_HIO_c_ct(daNpc_Ds1_HIO_c* p) {
    WWHD_FUNC(0x02232B9C, daNpc_Ds1_HIO_c*, p);
    if (p == nullptr) {
        p = (daNpc_Ds1_HIO_c*)operator_new(0x58);
        if (p == nullptr)
            return p;
    }
    p->__vtbl = 0x100199F4;
    gabi::call(0x028EFFD0, p->mChild, 1, 0x4C, 0x02232B24); /* __construct_array */
    daNpc_Ds1_childHIO_c& c = p->mChild[0];
    c.mCursorScale = 0.65f;
    c.mNpc.m00 = -80.0f;
    c.m40 = 27.0f;
    c.mNpc.mJntPrm[0] = 0x1FFE;
    c.mNpc.mJntPrm[1] = 0;
    c.m44 = 20.0f;
    c.m38 = 0.9f;
    c.mNpc.mJntPrm[4] = -0x1FFE;
    c.mNpc.m20 = 300.0f;
    c.m30 = 300.0f;
    c.mNpc.mJntPrm[7] = 0;
    c.m2C = 300.0f;
    c.mNpc.mJntPrm[3] = 0x1C70;
    c.mNpc.m18 = 60.0f;
    c.mNpc.mJntPrm[8] = 0x1000;
    c.mNpc.mJntPrm[5] = 0;
    c.mNpc.mJntPrm[2] = 0x38E0;
    p->mCount = -1;
    c.mNpc.m1E = 0;
    c.mNpc.m16 = 0x800;
    c.mNpc.mJntPrm[6] = 0;
    c.m3C = 0.5f;
    p->mNo = -1;
    return p;
}
VERIFY(0x02232B9C, daNpc_Ds1_HIO_c_ct);

/* 02232CBC */
static void __sinit_d_a_npc_ds1_cpp() {
    WWHD_FUNC(0x02232CBC, void);
    sinit_header_statics_z(0x10466EE4, 0x101BE21C, 0x10466F20);
    daNpc_Ds1_HIO_c_ct(&l_HIO_ds1());
    l_se_pos(0)->set(-158.0f, 160.0f, -663.0f);
    l_se_pos(1)->set(18.0f, 160.0f, -652.0f);
    l_se_pos(2)->set(-220.0f, 105.0f, -590.0f);
    l_se_pos(3)->set(-225.0f, 260.0f, -590.0f);
}
VERIFY(0x02232CBC, __sinit_d_a_npc_ds1_cpp);

/* 02232DE0: a trivial deleting destructor (the HIO classes) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02232DE0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02232DE0, trivial_dt);

/* 02232DF4: daNpc_Ds1_c deleting destructor (inline member destructors) */
static void daNpc_Ds1_c_dt(daNpc_Ds1_c* i_this, s32 flags) {
    WWHD_FUNC(0x02232DF4, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        gabi::call(0x025886D8, &i_this->mStick, 2);                 /* STControl::~STControl */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(b + 0x614), 2);         /* mAcchCir's cM3dGCir */
        gabi::store<u32>(b + 0x45C, OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x450, OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);                  /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);                          /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02232DF4, daNpc_Ds1_c_dt);

/* 02232E9C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02232E9C, void, p);
}
VERIFY(0x02232E9C, empty_virtual);
