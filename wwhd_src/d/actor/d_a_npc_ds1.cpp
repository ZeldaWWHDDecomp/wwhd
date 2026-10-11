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

static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    auto* animation = reinterpret_cast<Ds1Animation*>(p);
    return gabi::call_ptr<s32>(animation->mVtable->mFrameMax.get(), p);
}

static inline void* gnd_poly(dBgS_Acch& a) { return static_cast<cBgS_PolyInfo*>(&a.m_gnd); }

/* dComIfGp_getSelectItem(i): play+0x5BBB+i */
/* chu jellies in the beast bag (save +0xC0: red, +0xC1: green, +0xC2: blue) */

/* emitter->becomeInvalidEmitter() (HD: flags at +0x254, max frame at +0x5C) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    auto* emitter = reinterpret_cast<Ds1Emitter*>(e);
    u32 flags = emitter->mStatus;
    emitter->mMaxFrame = -1;
    emitter->mStatus = flags | 1;
}
/* the player's virtual at +0x114 of its HD vtable: setPlayerPosAndAngle(cXyz*, s16) */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    reinterpret_cast<daPy_py_c*>(pl)->setPlayerPosAndAngle(pos, angle);
}
static inline void J3DModel_copyAnmMtx(J3DModel* dst, J3DModel* src, s32 jnt) {
    J3DMtxBuffer_hd* buffer = src->mpMtxBuffer;
    u32 matrices = buffer->mpAnmMtx;
    buffer->mFlags |= 0x10;
    mtx_copy(&dst->mBaseTransformMtx, gabi::at<Mtx34>(matrices + jnt * sizeof(Mtx34)));
}

static inline Mtx34* J3DSys_mCurrentMtx_ds1() { return &l_currentMatrix; }

/* l_se_pos (0x10466EF0): the room's sound positions (__sinit) */

/* 0222D37C */
static BOOL daNpc_Ds1_checkCreateDrugChuchu(u8 itemNo) {
    if (itemNo == DS1_RED_POTION /* red potion */)
        return TRUE;
    if (itemNo == DS1_GREEN_POTION /* green potion */) {
        if (dComIfGs_isEventBit(DS1_GREEN_POTION_UNLOCKED))
            return TRUE;
        return FALSE;
    }
    if (itemNo == DS1_BLUE_POTION /* blue potion */ && dComIfGs_isEventBit(DS1_BLUE_POTION_UNLOCKED))
        return TRUE;
    return FALSE;
}

/* 0222D3FC */
s16 daNpc_Ds1_c::XyEventCB(int i) {
    s16 ret = -1;
    u8 itemNo = dComIfGp_getSelectItem(i);
    if ((u32)(itemNo - DS1_RED_POTION) <= 2) {
        bool enough = false;
        if (!daNpc_Ds1_checkCreateDrugChuchu(itemNo)) {
            if (itemNo == DS1_RED_POTION) {
                if (dComIfGs_getBeastNum(4) >= 10) enough = true;
            } else if (itemNo == DS1_GREEN_POTION) {
                if (dComIfGs_getBeastNum(5) >= 15) enough = true;
            } else if (itemNo == DS1_BLUE_POTION) {
                if (dComIfGs_getBeastNum(6) >= 15) enough = true;
            }
        }
        if (enough) {
            mOrderType = 5;
            ret = mEventIdx[2];
            mEventActionIdx = 2;
            ds1_setAction(this, ds1_tu.mEventAction, nullptr);
        } else if (!daNpc_Ds1_checkCreateDrugChuchu(itemNo)) {
            mOrderType = 6;
            ret = mEventIdx[3];
            mEventActionIdx = 3;
            ds1_setAction(this, ds1_tu.mEventAction, nullptr);
        }
        mItemNo = itemNo;
    } else {
        mEventActionIdx = 4;
    }
    return ret;
}

/* 0222D6C0 */
static s16 daNpc_Ds1_XyEventCB(void* i_this, int i) {
    return ((daNpc_Ds1_c*)i_this)->XyEventCB(i);
}

/* 0222D6C4 */
static BOOL nodeCallBack_Ds(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(l_currentModel.mValue); /* j3dSys.getModel() */
        daNpc_Ds1_c* i_this = gabi::at<daNpc_Ds1_c>(model->mUserArea);
        u32 jntNo = reinterpret_cast<Ds1JointNumber*>(J3DNode_toJoint(node))->mJointNo;
        if (i_this != nullptr) {
            J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
            buffer->mFlags |= 0x10;
            PSMTXCopy(gabi::at<Mtx34>(buffer->mpAnmMtx + jntNo * 0x30), calc_mtx());
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
            buffer = model->mpMtxBuffer;
            buffer->mFlags |= 0x10;
            mtx_copy(gabi::at<Mtx34>(buffer->mpAnmMtx + jntNo * 0x30), calc_mtx());
        }
    }
    return TRUE;
}

/* 0222D958 */
static BOOL daNpc_Ds1_shopMsgCheck(u32 msgNo) {
    if (msgNo - 0x1DD1 < 10 || msgNo == 0x1DC0)
        return TRUE;
    return FALSE;
}

/* 0222D97C */
BOOL daNpc_Ds1_c::initTexPatternAnm(u8 modify) {
    J3DModelData* modelData = J3DModel_getModelData(mpMorf->getModel());
    s32 resIdx = l_btpResources.mResources[mTexAnmIdx];
    mpBtp = (J3DAnmTexPattern*)ds1_getRes(resIdx);
    if (mpBtp.get() == nullptr)
        JUT_ASSERT_fail("d_a_npc_ds1.cpp", 0x1FE, "m_head_tex_pattern != (0)");
    if (!ds1Guest_mDoExt_btpAnm_init(&mBtpAnm, modelData, mpBtp, TRUE, 2, 1.0f, 0, -1, modify, 0))
        return FALSE;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 0222DA90 */
BOOL daNpc_Ds1_c::CreateHeap() {
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
    mJntNo[0] = ds1Guest_JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "head");
    if (mJntNo[0] < 0)
        JUT_ASSERT_fail("d_a_npc_ds1.cpp", 0xA2B, "m_head_jnt_num >= 0");
    mJntNo[1] = ds1Guest_JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "backbone");
    if (mJntNo[1] < 0)
        JUT_ASSERT_fail("d_a_npc_ds1.cpp", 0xA2E, "m_backbone_jnt_num >= 0");
    mJntNo[2] = ds1Guest_JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "handL");
    if (mJntNo[2] < 0)
        JUT_ASSERT_fail("d_a_npc_ds1.cpp", 0xA31, "m_handL_jnt_num >= 0");
    mJntNo[3] = ds1Guest_JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "handR");
    if (mJntNo[3] < 0)
        JUT_ASSERT_fail("d_a_npc_ds1.cpp", 0xA33, "m_handR_jnt_num >= 0");
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

    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)mJntNo[0] || i == (u32)(s32)mJntNo[1]) {
            /* getJointNodePointer(i)->setCallBack(nodeCallBack_Ds) */
            J3DModelData* data = mpMorf->getModel()->mModelData;
            data->getJointNodePointer(i)->setCallBack(nodeCallBack_Ds);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);

    void* brk = ds1_getRes(0xC);
    J3DModelData* cursorData = (J3DModelData*)ds1_getRes(9);
    mpShopCursor = ds1Guest_ShopCursor_create(cursorData, brk, l_HIO.mChild[mType].mCursorScale);
    if (mpShopCursor == 0)
        return FALSE;
    return TRUE;
}

/* 0222DF74 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpc_Ds1_c*)i_this)->CreateHeap();
}

/* 0222DF78 */
static daNpc_Ds1_c* daNpc_Ds1_c_ct(daNpc_Ds1_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ds1_c*)operator_new(0xA68);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = ds1_tu.mActorVtable;
    ds1Guest_mDoExt_btpAnm_ct(&i_this->mBtpAnm);
    dBgS_ObjAcch_ct(&i_this->mAcch, gc_tu.objAcch);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Cyl_ct(&i_this->mCyl, ds1_tu.mAabVtable);
    ds1Guest_EventCut_construct(&i_this->mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
    ds1Guest_JntCtrl_construct(&i_this->mJntCtrl);  /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    i_this->mHeadAnm.field_0x00.y = 0;
    i_this->mHeadAnm.field_0x1E = 0;
    i_this->mHeadAnm.field_0x18 = 0.0f;
    i_this->mHeadAnm.field_0x00.x = 0;
    i_this->mHeadAnm.field_0x1C = 0;
    i_this->mHeadAnm.field_0x14 = 0.0f;
    i_this->mHeadAnm.field_0x00.z = 0;
    i_this->mHeadAnm.field_0x20 = 0;
    i_this->mStick.mpVtable = gabi::at<STControlVtable>(ds1_tu.mStickVtable);
    ds1Guest_STControl_setWaitParm(&i_this->mStick, 15, 15, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    ds1Guest_STControl_init(&i_this->mStick);
    mDoExt_btkAnm::ct(&i_this->mBtkAnm);
    i_this->mLight.mHD20 = 1.0f;
    ds1Guest_ShopCam_construct(&i_this->mShopCam);   /* ShopCam_action_c::ShopCam_action_c */
    ds1Guest_ShopItems_construct(&i_this->mShopItems); /* ShopItems_c::ShopItems_c */
    return i_this;
}

/* 0222E124 */
static f32 daNpc_Ds1_gamma(u32 c) {
    f32 v = ds1Guest_std_powf((f32)(f64)c / 255.0f, 2.2f);
    if (v < 0.0f)
        return 0.0f;
    if (v > 1.0f)
        return 1.0f;
    return v;
}

static inline u8 gamma_u8(u8 c) { return (u8)gabi::ftoi(daNpc_Ds1_gamma(c) * 255.0f); }

/* 0222E1A4 */
void daNpc_Ds1_c::RoomEffectSet() {
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
        reinterpret_cast<Ds1Emitter*>(e)->mLifeTime = 13;
    mpRoomEmitter[6] = dComIfGp_particle_set(0x81D1, pos3, nullptr, nullptr, 0xFF, nullptr, -1, col2, nullptr, nullptr);
}

/* 0222E598 */
BOOL daNpc_Ds1_c::CreateInit() {
    mAngle.z = current.angle.z;
    mAngle.x = current.angle.x;
    gravity = -30.0f;
    mAngle.y = current.angle.y;
    attention_info.flags = 10; /* attention_info.flags */
    if (mType == 0)
        ds1_setAction(this, ds1_tu.mWaitAction, nullptr);
    /* mAttentionBasePos = current.pos (integer copy) */
    ds1VectorWords(&mAttentionBasePos).z = ds1VectorWords(&current.pos).z;
    ds1VectorWords(&mAttentionBasePos).x = ds1VectorWords(&current.pos).x;
    ds1VectorWords(&mAttentionBasePos).y = ds1VectorWords(&current.pos).y;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set((&l_cylinderSource));
    mAnmLoopCount = 0;
    mShopInputHandled = 0;
    mShopCam.mCamDataIdx = 0;
    mCyl.SetStts(&mStts);
    mShopCam.mCurrActionFunc.mThisDelta = 0;
    mShopCam.mCurrActionFunc.mVtableIndex = 0;
    mBoughtItemNo = 0xFF;
    mShopCam.mCurrActionFunc.mFunction = 0;
    mShopState = 0;
    mPendingMsgNo = 0;
    mShopItems.mItemSetListGlobalIdx = 0;
    ds1Guest_ShopItems_setItemSetDataList(&mShopItems);
    ds1Guest_ShopItems_createItem(&mShopItems, 3, current.roomNo);
    eventInfo.setXyEventCB(daNpc_Ds1_XyEventCB); /* eventInfo.setXyEventCB(daNpc_Ds1_XyEventCB) */
    ds1Guest_dNpc_EventCut_setActorInfo(&mEventCut, "Ds1", this);
    mEventCut.mpJntCtrl = &mJntCtrl;
    mEventActionIdx = 4;
    mEventIdx[0] = dComIfGp_evmng_getEventIdx("CREATE_DRUG", 0xFF);
    mEventIdx[1] = dComIfGp_evmng_getEventIdx("GET_DRUG", 0xFF);
    mEventIdx[2] = dComIfGp_evmng_getEventIdx("PUT_ITEM", 0xFF);
    mEventIdx[3] = dComIfGp_evmng_getEventIdx("PUT_ITEM_FAIL", 0xFF);
    if (mbRoomEffect) {
        J3DModel_setBaseTRMtx(mpBtkModel, &l_identityMatrix);
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

/* 0222E8F0 */
cPhs_State daNpc_Ds1_c::_create() {
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0)
            daNpc_Ds1_c_ct(this);
        actor_condition |= fopAcCnd_INIT_e;
    }
    cPhs_State rt = dComIfG_resLoad(&mPhs, DS1_ARC);
    if (rt == cPhs_COMPLEATE_e) {
        mType = 0;
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)&CheckCreateHeap), 0x8340))
            return cPhs_ERROR_e;
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
        if (l_HIO.mCount < 0)
            l_HIO.mNo = mDoHIO_createChild("薬屋店員", &l_HIO);
        l_HIO.mCount = l_HIO.mCount + 1;
        if (!CreateInit())
            return cPhs_ERROR_e;
    }
    return rt;
}

/* 0222EA14 */
static cPhs_State daNpc_Ds1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Ds1_c*)i_this)->_create();
}

/* 0222EA18 */
void daNpc_Ds1_c::RoomEffectDelete() {
    for (int i = 1; i <= 7; i++) {
        // HD: index seven intentionally reaches the following model pointer.
        JPABaseEmitter* e = i < 7 ? mpRoomEmitter[i].get() : reinterpret_cast<JPABaseEmitter*>(mpBtkModel.get());
        if (e != nullptr)
            JPABaseEmitter_becomeInvalidEmitter(e);
    }
}

/* 0222EA54 */
BOOL daNpc_Ds1_c::_delete() {
    dKy_plight_cut(&mLight);
    dComIfG_resDelete(&mPhs, DS1_ARC);
    if (heap.get() != nullptr && mpMorf.get() != nullptr)
        mpMorf->stopZelAnime();
    mDoAud_seDeleteObject((&l_soundPositions.mPositions[0]));
    mDoAud_seDeleteObject((&l_soundPositions.mPositions[1]));
    mDoAud_seDeleteObject((&l_soundPositions.mPositions[2]));
    mDoAud_seDeleteObject((&l_soundPositions.mPositions[3]));
    RoomEffectDelete();
    if (l_HIO.mCount >= 0) {
        l_HIO.mCount = l_HIO.mCount - 1;
        if (l_HIO.mCount < 0)
            mDoHIO_deleteChild(l_HIO.mNo);
    }
    return TRUE;
}

/* 0222EB18 */
static BOOL daNpc_Ds1_Delete(daNpc_Ds1_c* i_this) {
    return i_this->_delete();
}

/* 0222EB1C */
void daNpc_Ds1_c::playTexPatternAnm() {
    if (ds1Guest_cLib_calcTimer(&mBlinkTimer) == 0) {
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

/* 0222EBE0 */
void daNpc_Ds1_c::talkInit() {
    mTalkState = 0;
}

/* 0222EBEC */
void daNpc_Ds1_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        if (mOrderType == 3) {
            mShopCam.mCurrActionFunc.mFunction = 0;
            mShopCam.mCurrActionFunc.mVtableIndex = 0;
            mOrderType = 0;
            mShopCam.mCurrActionFunc.mThisDelta = 0;
            ds1_setAction(this, ds1_tu.mGetdemoAction, nullptr);
        } else if (mOrderType == 4) {
            mShopCam.mCurrActionFunc.mFunction = 0;
            mShopCam.mCurrActionFunc.mVtableIndex = 0;
            mOrderType = 0;
            mShopCam.mCurrActionFunc.mThisDelta = 0;
            ds1_setAction(this, ds1_tu.mEventAction, nullptr);
        } else if (mOrderType == 5) {
            mOrderType = 0;
        }
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrderType == 1 || mOrderType == 2) {
            if (dComIfGp_event_chkTalkXY()) {
                s32 a50 = mEventActionIdx;
                mOrderType = 0;
                if (a50 != 2) {
                    mIsTalking = 1;
                    talkInit();
                }
                mItemNo = dComIfGp_event_getPreItemNo();
            } else if (dComIfGs_isEventBit(0x1120)) {
                ds1Guest_ShopCam_shop_cam_action_init(&mShopCam);
                gabi::Local<cXyz> pos;
                pos->y = 0.0f;
                pos->z = 100.0f;
                pos->x = 10.0f;
                daPy_setPlayerPosAndAngle(dComIfGp_getPlayer(0), pos, -0x6000);
                mOrderType = 0;
                mIsTalking = 1;
                talkInit();
            } else {
                ds1Guest_ShopCam_ds_normal_cam_action_init(&mShopCam);
                gabi::Local<cXyz> pos;
                pos->y = 0.0f;
                pos->z = 100.0f;
                pos->x = -115.0f;
                daPy_setPlayerPosAndAngle(dComIfGp_getPlayer(0), pos, -0x6000);
                mOrderType = 0;
                mIsTalking = 1;
                talkInit();
            }
        }
    }
}

/* 0222F050 */
void daNpc_Ds1_c::eventOrder() {
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

/* 0222F100 */
void daNpc_Ds1_c::setCollision() {
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

/* 0222F1BC */
BOOL daNpc_Ds1_c::_execute() {
    bool inEvent = mAction.idx == -1 && mAction.delta == 0 && mAction.fn == ds1_tu.mEventAction;
    daNpc_Ds1_childHIO_c& hio = l_HIO.mChild[mType];
    if (!inEvent) {
        dNpc_HIO_c& prm = hio.mNpc;
        mJntCtrl.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    } else {
        dNpc_HIO_c& prm = hio.mNpc;
        mJntCtrl.setParam(prm.mMaxBackboneX, 0x1C70, prm.mMinBackboneX, -0x38E0, prm.mMaxHeadX, 0x38E0, prm.mMinHeadX, -0x1C70, prm.mMaxTurnStep);
    }
    playTexPatternAnm();
    if (mAcch.ChkGroundHit()) {
        u32 sndId = ds1Guest_dBgS_GetMtrlSndId(dComIfG_Bgsp(), gnd_poly(mAcch));
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mAnmEnd = ds1Guest_McaMorf_play(mpMorf, &eyePos, sndId, reverb);
    } else {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mAnmEnd = ds1Guest_McaMorf_play(mpMorf, &eyePos, 0, reverb);
    }
    if (mpMorf->getFrame() < mPrevFrame)
        mAnmEnd = 1;
    mPrevFrame = mpMorf->getFrame();
    checkOrder();
    ds1_ptmf_call(&mAction, this, nullptr);
    ds1Guest_ShopCam_move(&mShopCam);
    ds1Guest_ShopItems_Item_Move(&mShopItems);
    eventOrder();
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts)));
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = ds1Guest_dBgS_GetRoomId(dComIfG_Bgsp(), gnd_poly(mAcch));
    u8 polyColor = ds1Guest_dBgS_GetPolyColor(dComIfG_Bgsp(), gnd_poly(mAcch));
    J3DModel* model = mpMorf->getModel();
    tevStr.mEnvrIdxOverride = polyColor; /* tevStr.mEnvrIdxOverride */
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    setCollision();
    if (mbRoomEffect) {
        mDoAud_seStart(0x703C, (&l_soundPositions.mPositions[0]), 0, dComIfGp_getReverb(current.roomNo));
        mDoAud_seStart(0x703D, (&l_soundPositions.mPositions[1]), 0, dComIfGp_getReverb(current.roomNo));
        mDoAud_seStart(0x703E, (&l_soundPositions.mPositions[2]), 0, dComIfGp_getReverb(current.roomNo));
        u8 t = mSeTimer;
        mSeTimer = (u8)(t + 1);
        if (t >= 50) {
            mDoAud_seStart(0x69A1, (&l_soundPositions.mPositions[3]), 0, dComIfGp_getReverb(current.roomNo));
            mSeTimer = 0;
        }
        mBtkAnm.play();
    }
    cLib_addCalc0(&mLight.mPower, 0.25f, 20.0f);
    return TRUE;
}

/* 0222F5CC */
static BOOL daNpc_Ds1_Execute(daNpc_Ds1_c* i_this) {
    return i_this->_execute();
}

/* 0222F5D0 */
BOOL daNpc_Ds1_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData(model);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, modelData, mBtpFrame);
    mpMorf->updateDL();
    reinterpret_cast<Ds1ModelAnimation*>(modelData)->mBtpAnimation = 0; /* mBtpAnm.remove(modelData) */

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
        ds1Guest_ShopCursor_draw(mpShopCursor);
    if (mbRoomEffect) {
        mDoExt_btkAnm_entry(&mBtkAnm, J3DModel_getModelData(mpBtkModel), mBtkAnm.getFrame());
        mDoExt_modelUpdateDL(mpBtkModel, 0);
    }
    ds1Guest_registerFigure(0x5D, this, &current.pos, (s16)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
    return TRUE;
}

/* 0222F9B4 */
static BOOL daNpc_Ds1_Draw(daNpc_Ds1_c* i_this) {
    return i_this->_draw();
}

/* 0222F9B8 */
static BOOL daNpc_Ds1_IsDelete(daNpc_Ds1_c*) {
    return TRUE;
}

/* 0222F9C0 */
void daNpc_Ds1_c::setAnm(s8 anmIdx, f32 morf) {
    if (anmIdx != mAnmIdx || mAnmIdx == -1) {
        mAnmIdx = anmIdx;
        if (morf < 0.0f)
            morf = l_anmMorph.mValues[anmIdx];
        ds1Guest_dNpc_setAnm_2(mpMorf, l_anmLoop.mValues[anmIdx], morf, l_anmSpeed.mValues[anmIdx], l_anmResources.mValues[anmIdx], -1, DS1_ARC);
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

/* 0222FA9C */
u32 daNpc_Ds1_c::setTexAnm(s8 texIdx) {
    if (texIdx != mTexAnmIdx || mTexAnmIdx == -1) {
        mTexAnmIdx = texIdx;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 02232B24 */
static daNpc_Ds1_childHIO_c* daNpc_Ds1_childHIO_c_ct(daNpc_Ds1_childHIO_c* p) {
    if (p == nullptr) {
        p = (daNpc_Ds1_childHIO_c*)operator_new(0x4C);
        if (p == nullptr)
            return p;
    }
    p->__vtbl = ds1_tu.mChildHioVtable;
    ds1Guest_NpcHio_construct(&p->mNpc); /* dNpc_HIO_c */
    p->m38 = 0.0f;
    p->m2C = 0.0f;
    p->m3C = 0.0f;
    p->mCursorScale = 0.0f;
    p->m30 = 0.0f;
    p->m44 = 0.0f;
    p->m40 = 0.0f;
    return p;
}

/* 02232B9C */
static daNpc_Ds1_HIO_c* daNpc_Ds1_HIO_c_ct(daNpc_Ds1_HIO_c* p) {
    if (p == nullptr) {
        p = (daNpc_Ds1_HIO_c*)operator_new(0x58);
        if (p == nullptr)
            return p;
    }
    p->__vtbl = ds1_tu.mHioVtable;
    ds1Guest_constructArray(p->mChild, 1, sizeof(daNpc_Ds1_childHIO_c), gabi::guest_fn((const void*)&daNpc_Ds1_childHIO_c_ct)); /* __construct_array */
    daNpc_Ds1_childHIO_c& c = p->mChild[0];
    c.mCursorScale = 0.65f;
    c.mNpc.m04 = -80.0f;
    c.m40 = 27.0f;
    c.mNpc.mMaxHeadX = 0x1FFE;
    c.mNpc.mMaxBackboneX = 0;
    c.m44 = 20.0f;
    c.m38 = 0.9f;
    c.mNpc.mMinHeadX = -0x1FFE;
    c.mNpc.mMaxAttnDistXZ = 300.0f;
    c.m30 = 300.0f;
    c.mNpc.mMinBackboneY = 0;
    c.m2C = 300.0f;
    c.mNpc.mMaxBackboneY = 0x1C70;
    c.mNpc.mAttnYOffset = 60.0f;
    c.mNpc.mMaxTurnStep = 0x1000;
    c.mNpc.mMinBackboneX = 0;
    c.mNpc.mMaxHeadY = 0x38E0;
    p->mCount = -1;
    c.mNpc.m22 = 0;
    c.mNpc.mMaxHeadTurnVel = 0x800;
    c.mNpc.mMinHeadY = 0;
    c.m3C = 0.5f;
    p->mNo = -1;
    return p;
}

/* 02232CBC */
static void __sinit_d_a_npc_ds1_cpp() {
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
    daNpc_Ds1_HIO_c_ct(&l_HIO);
    (&l_soundPositions.mPositions[0])->set(-158.0f, 160.0f, -663.0f);
    (&l_soundPositions.mPositions[1])->set(18.0f, 160.0f, -652.0f);
    (&l_soundPositions.mPositions[2])->set(-220.0f, 105.0f, -590.0f);
    (&l_soundPositions.mPositions[3])->set(-225.0f, 260.0f, -590.0f);
}

/* 02232DE0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02232DF4 */
static void daNpc_Ds1_c_dt(daNpc_Ds1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        ds1Guest_Stick_destruct(&i_this->mStick, 2);                 /* STControl::~STControl */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        ds1Guest_Circle_destruct(&i_this->mAcchCir.m_cir, 2);         /* mAcchCir's cM3dGCir */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
        ds1Guest_Acch_destruct(&i_this->mAcch, 0);                  /* dBgS_Acch::~dBgS_Acch */
        ds1Guest_Actor_destruct(i_this, 0);                          /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02232E9C */
static void empty_virtual(void* p) {
}

/* WWHD glue
str 10019A3C "d_a_npc_ds1.cpp" in 0222D97C
str 10019A4C "m_head_tex_pattern != (0)" in 0222D97C
str 10019A7C "head" in 0222DA90
str 10019A84 "d_a_npc_ds1.cpp" in 0222DA90
str 10019AAC "m_head_jnt_num >= 0" in 0222DA90
str 10019AC0 "backbone" in 0222DA90
str 10019A94 "m_backbone_jnt_num >= 0" in 0222DA90
str 10019A6C "handL" in 0222DA90
str 10019ACC "m_handL_jnt_num >= 0" in 0222DA90
str 10019A74 "handR" in 0222DA90
str 10019AE4 "m_handR_jnt_num >= 0" in 0222DA90
str 10019B48 "Ds1" in 0222E598
str 10019B4C "CREATE_DRUG" in 0222E598
str 10019B68 "GET_DRUG" in 0222E598
str 10019B74 "PUT_ITEM" in 0222E598
str 10019B58 "PUT_ITEM_FAIL" in 0222E598
str 10019B80 "薬屋店員" in 0222E8F0
str 101BE240 "Ds"
*/
