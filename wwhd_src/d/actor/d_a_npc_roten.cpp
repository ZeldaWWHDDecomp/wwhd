/**
 * d_a_npc_roten.cpp (WWHD)
 * NPC - Traveling Merchants: construction, heap, draw, animation, collision, messages.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_roten.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_roten.h"

static s16 daNpcRoten_XyEventCB(void* i_this, int param_1);

const gc_tu_data gc_tu = {.safeString = 0x10021278};
static RotenStringTable l_arcname_tbl;
static RotenStringTable l_npc_staff_id;
static RotenNpcTable l_npc_dat;
// HD: main-model resource IDs differ among merchants; the other resource tables retain GC order.
static RotenIndexTable l_bmd_ix_tbl = {{2, 14, 15}};
static RotenIndexTable l_head_bmd_ix_tbl = {{23, 24, 25}};
static RotenIndexTable l_head_bck_ix_tbl = {{18, 16, 17}};
static RotenIndexTable l_btp_ix_tbl = {{3, 3, 3}};
static RotenAnimationTable l_bck_ix_tbl = {{{0, 4, 7, 5, 6, 10, 11, 8, 9, 19},
                                            {0, 4, 7, 5, 6, 10, 11, 8, 9, 19},
                                            {0, 4, 7, 5, 6, 10, 11, 8, 9, 19}}};
static RotenSaveTable l_save_dat;
static RotenItemTable l_item_dat = {{{1, 0, 7, 4, 9, 3, 7, 9, 9, 7, 9, 10},
                                     {1, 2, 1, 10, 9, 3, 2, 9, 9, 8, 9, 10},
                                     {1, 0, 6, 5, 7, 4, 2, 4, 5, 5, 11, 10}}};
static sRotenAnmDat l_npc_anm_wait;
static sRotenAnmDat l_npc_anm_walk;
static RotenWindAnmTable l_npc_anm_wind;
static dCcD_SrcSph l_sph_src;
static dCcD_SrcCyl dNpc_cyl_src;

static RotenMessageNumbers l_msg_xy_koukan_end;
static RotenMessageNumbers l_msg_xy_koukan_first;
static RotenMessageNumbers l_msg_xy_koukan_no;
static RotenMessageNumbers l_msg_xy_koukan_rupee;
static RotenForceMessages l_msg_try_force;
static RotenMapMessages l_msg_collect_map;
static RotenMessageTable l_msg_xy_exchange3;
static RotenMessageTable l_msg_xy_no_roten_item;
static RotenMessageTable l_msg_xy_invalid_item;
static RotenMessageTable l_msg_1st_talk;
static RotenMessageTable l_msg_exchange3;
static RotenMessageTable l_msg_exchange;
static RotenMessageTable l_msg_etc;
static RotenExchangeMessages l_msg_xy_koukan_item;
static RotenExchangeMessages l_msg_xy_koukan_item2;
static RotenExchangeMessages l_msg_xy_koukan_item3;
static RotenMessageTable l_msg_xy_after_get_demo;
static RotenMessageTable l_msg_xy_after_get_demo2;
static RotenMessageAnimations l_msg_anm;
static RotenItemNumber l_get_item_no;
static RotenCurrentModel l_currentModel;
static Mtx34 l_currentMtx;
static RotenMessageManagerPtr l_messageManager;

// HD: the original GHS tables select executeWait/Talk/Walk/Turn/Wind (and their Init methods).
// One mapped definition preserves member-pointer adjustment and virtual dispatch across both
// sources.
static RotenInitTable l_execute_init;
static RotenMoveTable moveProc;
static RotenCutNames cut_name_tbl;
static RotenPhaseHandlers l_method;
static constexpr u32 l_cylinderVtable = 0x10021290;
static constexpr u32 l_staticObjects = 0x104684A0;
static constexpr u32 l_staticDescriptors = 0x101C602C;
static constexpr u32 l_acchBaseVtable = 0x100212A0;
static constexpr u32 l_acchVtable = 0x100212B0;
static u32 daObj_PrmAbstract(fopAc_ac_c* actor, s32 width, s32 shift);

/* 022D30F4 */
static daNpcRoten_c* daNpcRoten_c_ct(daNpcRoten_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpcRoten_c*)operator_new(0xB40);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    roten_constructNpc(i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = l_actorVtable;
    mDoExt_btpAnm::ct(&i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    dCcD_Cyl_ct(&i_this->mCyl2, l_cylinderVtable);
    roten_constructSphere(&i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->field_0x9C8 = 1;
    i_this->field_0x9C7 = 0;
    i_this->field_0x9CA = 0;
    i_this->mShownItemBtn = daNpcRoten_c::dItemBtn_NONE_e;
    i_this->field_0x6FC = fpcM_ERROR_PROCESS_ID_e;
    i_this->field_0x98C = nullptr;
    i_this->field_0x990 = 0.0f;
    i_this->field_0x9C9 = 0;
    i_this->field_0x994 = -1.0f;
    i_this->field_0x9C6 = 0;
    i_this->field_0x9C4 = 0;
    i_this->field_0x9BB = 0;
    i_this->field_0x9BC = 0;
    i_this->field_0x9C1 = 0;
    i_this->field_0x99C = 1;
    i_this->field_0x9B2 = 0;
    i_this->field_0x9C0 = 0;
    return i_this;
}

/* 022D2A60 */
static BOOL daNpc_Roten_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = l_currentModel.model.get(); /* j3dSys.getModel() */
        daNpcRoten_c* i_this = static_cast<daNpcRoten_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        Mtx34* dst = getAnmMtx(model, jntNo);
        mtx_copy(dst, calc_mtx());            /* model->setAnmMtx(jntNo, *calc_mtx) */
        PSMTXCopy(calc_mtx(), &l_currentMtx); /* J3DSys::mCurrentMtx */
    }
    return TRUE;
}

/* 022D30F0 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return ((daNpcRoten_c*)i_this)->createHeap(); }

/* 022D3234 */
static cPhs_State phase_1(daNpcRoten_c* i_this) {
    /* fopAcM_ct(i_this, daNpcRoten_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcRoten_c_ct(i_this);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->mNpcNo = i_this->getPrmNpcNo();
    if (!dComIfGs_isEventBit(0x1108)) {
        return cPhs_STOP_e;
    }
    i_this->field_0x9BC = 1; /* setResFlag(1) */
    return cPhs_NEXT_e;
}

/* 022D3A44 */
static cPhs_State phase_2(daNpcRoten_c* i_this) {
    u8 npcNo = i_this->getPrmNpcNo();
    // HD: l_arcname_tbl is indexed through a range-checked accessor
    if (npcNo >= 3) {
        JUT_ASSERT_fail(
            "d_a_npc_roten.cpp", 0x5E7,
            "(npc_no >= 0) && (npc_no < (sizeof(l_arcname_tbl) / sizeof(l_arcname_tbl[0])))");
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    cPhs_State result = dComIfG_resLoad(&i_this->mPhs, arcname(npcNo));
    if (result == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x022D30F0 /* CheckCreateHeap */, 0x4620)) {
            result = i_this->createInit();
        } else {
            i_this->mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return result;
}

/* 022D3B1C */
cPhs_State daNpcRoten_c::_create() {

    return roten_phaseHandler(&mPhs2, &l_method, this); /* dComLbG_PhaseHandler */
}

/* 022D2CCC */
BOOL daNpcRoten_c::createHeap() {
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectIDRes(arcname(mNpcNo), l_bmd_ix_tbl.entries[mNpcNo]);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), l_bck_ix_tbl.entries[mNpcNo][field_0x9C0]);
    // HD: the same differed display list flags for all three merchants (GameCube l_diff_flag_tbl)
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    J3DModelData* headModelData =
        (J3DModelData*)dComIfG_getObjectIDRes(arcname(mNpcNo), l_head_bmd_ix_tbl.entries[mNpcNo]);
    J3DAnmTransform* headBck = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), l_head_bck_ix_tbl.entries[mNpcNo]);
    field_0x6D8 = mDoExt_McaMorf::create(nullptr, headModelData, nullptr, nullptr, headBck, 2, 1.0f,
                                         0, -1, 1, nullptr, 0x80000, 0x37441422);
    if (field_0x6D8.get() == nullptr || field_0x6D8->getModel() == nullptr) {
        return FALSE;
    }
    field_0x6D4 = field_0x6D8->getModel();

    m_jnt.mHeadJntNum =
        roten_getJointIndex(gabi::ea(modelData->getJointName()), "head" /* "head" */);
    if (m_jnt.mHeadJntNum < 0) { /* JUT_ASSERT(1605, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail("d_a_npc_roten.cpp", 0x645, "m_jnt.getHeadJntNum() >= 0");
    }
    m_jnt.mBackboneJntNum =
        roten_getJointIndex(gabi::ea(modelData->getJointName()), "backbone1" /* "backbone1" */);
    if (m_jnt.mBackboneJntNum < 0) {
        JUT_ASSERT_fail("d_a_npc_roten.cpp", 0x64A, "m_jnt.getBackboneJntNum() >= 0");
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    m_hand_L_jnt_num =
        roten_getJointIndex(gabi::ea(modelData->getJointName()), "handL" /* "handL" */);
    if (m_hand_L_jnt_num < 0) {
        JUT_ASSERT_fail("d_a_npc_roten.cpp", 0x65C, "m_hand_L_jnt_num >= 0");
    }
    m_bag_jnt_num = roten_getJointIndex(gabi::ea(modelData->getJointName()), "Bag1" /* "Bag1" */);
    if (m_bag_jnt_num < 0) {
        JUT_ASSERT_fail("d_a_npc_roten.cpp", 0x660, "m_bag_jnt_num >= 0");
    }

    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* modelData->getJointNodePointer(i)->setCallBack(daNpc_Roten_nodeCallBack) */
            modelData->getJointNodePointer(i)->setCallBack(
                reinterpret_cast<J3DJointCallBack>(daNpc_Roten_nodeCallBack));
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}

/* 022D34DC */
static s16 daNpcRoten_XyEventCB(void* i_this, int param_1) {
    return ((daNpcRoten_c*)i_this)->XyEventCB(param_1);
}

/* 022D3710 */
cPhs_State daNpcRoten_c::createInit() {
    s32 weight = 0xFF;
    u8 railId = getPrmRailID();
    if (railId != 0xFF) {
        roten_dNpc_PathRun_setInf(&mPathRun, railId, current.roomNo, 1);
        if (!mPathRun.isPath()) {
            return cPhs_ERROR_e;
        }
        actor_status = actor_status & ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        gabi::Local<cXyz> point;
        roten_dNpc_PathRun_getPoint(&mPathRun, point, mPathRun.mIdx);
        old.pos.copy(*point);
        current.pos.copy(old.pos);
        roten_dNpc_PathRun_incIdxLoop(&mPathRun);
        roten_dNpc_PathRun_getPoint(&mPathRun, point, mPathRun.mIdx);
        gabi::Local<cXyz> a, b;
        b->x = point->x;
        b->y = point->y;
        b->z = point->z;
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        dNpc_calc_DisXZ_AngY(a, b, nullptr, &current.angle.y);
        field_0x9A6 = 1;
        weight = 0xFE;
    }
    gravity = -9.0f;
    field_0x99E = dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_1ST", 0xFF); /* "ROTEN_EXCHANGE_1ST" */
    field_0x9A0 = dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_2ND", 0xFF); /* "ROTEN_EXCHANGE_2ND" */
    field_0x9A2 = dComIfGp_evmng_getEventIdx("ROTEN_CHANGE_ITEM", 0xFF);  /* "ROTEN_CHANGE_ITEM" */
    field_0x9A4 = dComIfGp_evmng_getEventIdx("ROTEN_GET_MAP", 0xFF);      /* "ROTEN_GET_MAP" */
    mEventCut.setActorInfo2(l_npc_staff_id.entries[mNpcNo].get(), this);
    field_0x9AC = 0;
    field_0x9AE = 0;
    field_0x9B4 = 0;
    field_0x9B5 = 0;
    eventInfo.setXyEventCB(daNpcRoten_XyEventCB); /* eventInfo.setXyEventCB(daNpcRoten_XyEventCB) */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;  /* attention_info.distances[TALK] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA; /* attention_info.distances[SPEAK] */
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;      /* LOCKON_TALK | ACTION_SPEAK */
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -200.0f, 0.0f, -200.0f, 200.0f, 300.0f, 200.0f);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 groundH = mObjAcch.GetGroundH();
    if (!(groundH == -1000000000.0f)) { /* -G_CM3D_F_INF */
        current.pos.y = groundH;
        home.pos.y = groundH;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(weight, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl2.Set(&dNpc_cyl_src);
    mCyl2.SetStts(&mStts);
    setCollision(npc_dat(mNpcNo).field_0x2C, 200.0f);
    setCollisionB();
    mSph.Set(&l_sph_src);
    mSph.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}

/* 022D3B34 */
bool daNpcRoten_c::_delete() {
    // HD: dComIfG_resDelete (GameCube resDeleteDemo)
    dComIfG_resDelete(&mPhs, arcname(mNpcNo));
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}

/* 022D5790 */
bool daNpcRoten_c::_draw() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = roten_modelData(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, field_0x6D4, &tevStr);
    mBtpAnm.entry(modelData, field_0x9B8);
    // HD: no l_bmt_ix_tbl material table
    mpMorf->updateDL();
    if (field_0x9C8 != 0) {
        Mtx34* headMtx = getAnmMtx(pModel, m_jnt.mHeadJntNum);
        J3DModel_setBaseTRMtx(field_0x6D4, headMtx);
        field_0x6D8->updateDL();
    }
    static_cast<RotenModelData*>(modelData)->mpTexNoAnm = 0; /* mBtpAnm.remove(modelData) */
    // HD: no shadow
    dSnap_RegistFig(0x7E /* DSNAP_TYPE_NPC_ROTEN */, this, 1.0f, 1.0f, 1.0f);
    return true;
}

/* 022D60DC */
u16 daNpcRoten_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 status = fopMsgStts_MSG_CONTINUES_e;
    RotenMessageManager* msg =
        l_messageManager.manager; // HD: the message manager (GameCube mpCurrMsg)
    switch ((u32)*pMsgNo) {
    case 0x293D:
    case 0x29A1:
    case 0x2A05:
        status = fopMsgStts_MSG_ENDS_e;
        break;
    default:
        if (field_0x98C.get() != nullptr) {
            field_0x98C = field_0x98C.get() + 1;
            u32 v = *field_0x98C;
            switch (v) {
            case 0:
                field_0x98C = nullptr;
                status = 0x10;
                break;
            case 1:
                if ((u32)msg->mSelectNum != 0) { /* mSelectNum */
                    field_0x9B2 = field_0x9B2 | 8;
                }
                field_0x98C = nullptr;
                status = 0x10;
                break;
            case 2:
                if ((u32)msg->mSelectNum == 0) {
                    dComIfG_inf_c* play = gabi::at<dComIfG_inf_c>(dComIfGp_ea());
                    if ((s32)dComIfGs_getRupee() <
                        (s32)(s16)play->play.mMessageRupee /* getMessageRupee */) {
                        field_0x9B2 = field_0x9B2 | 0x20;
                        field_0x98C = nullptr;
                        status = 0x10;
                        break;
                    }
                    s16 rupee = dComIfGp_getMessageRupee();
                    dComIfG_inf_c* play2 = gabi::at<dComIfG_inf_c>(
                        dComIfGp_ea()); /* dComIfGp_setItemRupeeCount(-rupee) */
                    play2->play.mItemRupeeCount = (s32)play2->play.mItemRupeeCount - rupee;
                    u8 temp = item_dat(mNpcNo, field_0x9BE);
                    u8 reg = dComIfGs_getEventReg(save_dat(mNpcNo, 2));
                    dComIfGs_setEventReg(save_dat(mNpcNo, 2), (u8)(reg + 1));
                    if (dComIfGs_isGetItemReserve(temp)) {
                        *pMsgNo = l_msg_xy_koukan_end.entries[mNpcNo];
                        field_0x9B2 = field_0x9B2 | 0x40;
                    } else {
                        *pMsgNo = l_msg_xy_koukan_first.entries[mNpcNo];
                    }
                    dComIfGs_setReserveItemChange(mShownItemBtn,
                                                  (u8)(temp + dItemNo_TOWN_FLOWER_e));
                    field_0x98C = nullptr;
                } else {
                    field_0x9B2 = field_0x9B2 | 0x10;
                    field_0x98C = nullptr;
                    status = 0x10;
                }
                break;
            case 3:
                status = 0x10;
                break;
            default:
                *pMsgNo = v;
                break;
            }
        } else {
            status = 0x10;
        }
        break;
    }
    return status;
}

/* 022D6304 */
u32 daNpcRoten_c::getMsg() {
    u32 msgNo = 0;
    field_0x98C = nullptr;
    u16 flags = field_0x9B2;
    if (flags & 8) {
        field_0x9B2 = flags & ~8;
        msgNo = l_msg_xy_koukan_no.entries[mNpcNo];
    } else if (flags & 0x10) {
        field_0x9B2 = flags & ~0x10;
        msgNo = l_msg_xy_koukan_no.entries[mNpcNo];
    } else if (flags & 0x20) {
        field_0x9B2 = flags & ~0x20;
        msgNo = l_msg_xy_koukan_rupee.entries[mNpcNo];
    } else if (dComIfGp_event_chkTalkXY()) {
        u8 itemNo = dComIfGp_event_getPreItemNo();
        if (isGetMap(itemNo)) {
            field_0x98C = l_msg_try_force.entries;
            dComIfGs_setReserveItemEmpty();
            dComIfGs_onEventBit(0x3E04);
        } else if (dComIfGs_getEventReg(save_dat(mNpcNo, 2)) >= 3) {
            field_0x98C = l_msg_xy_exchange3.entries[mNpcNo].get();
        } else if (!isHaitatuItem(itemNo)) {
            field_0x98C = l_msg_xy_no_roten_item.entries[mNpcNo].get();
        } else if (!isKoukanItem(itemNo)) {
            field_0x98C = l_msg_xy_invalid_item.entries[mNpcNo].get();
        } else {
            u8 npcNo = mNpcNo;
            u8 idx = (u8)(itemNo - dItemNo_TOWN_FLOWER_e);
            field_0x9BE = idx;
            field_0x98C = l_msg_xy_koukan_item.entries[npcNo][idx].get();
            dComIfGs_onEventBit(save_dat(npcNo, 6));
        }
    } else {
        if (!dComIfGs_isEventBit(save_dat(mNpcNo, 0))) {
            field_0x98C = l_msg_1st_talk.entries[mNpcNo].get();
            dComIfGs_onEventBit(save_dat(mNpcNo, 0));
        } else if (dComIfGs_getEventReg(save_dat(mNpcNo, 2)) >= 3) {
            field_0x98C = l_msg_exchange3.entries[mNpcNo].get();
        } else if (mNpcNo == 1 && dComIfGs_isEventBit(0x3E04)) {
            field_0x98C = l_msg_collect_map.entries;
        } else if (dComIfGs_isEventBit(save_dat(mNpcNo, 6))) {
            field_0x98C = l_msg_exchange.entries[mNpcNo].get();
        } else {
            field_0x98C = l_msg_etc.entries[mNpcNo].get();
        }
    }
    if (field_0x98C.get() != nullptr) {
        msgNo = *field_0x98C;
    }
    return msgNo;
}

/* 022D4094 */
void daNpcRoten_c::setMessage(u32 msgNo) {
    mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
    mCurrMsgNo = msgNo;
}

/* 022D4BE4 */
void daNpcRoten_c::setAnmFromMsgTag() {
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    // HD: a table of the nine animation lists (.data 0x100215D0)
    if (attr <= 8) {
        setAnmTbl(l_msg_anm.entries[attr].get());
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}

/* 022D3208 */
u8 daNpcRoten_c::getPrmNpcNo() { return (u8)daObj_PrmAbstract(this, PRM_NPC_NO_W, PRM_NPC_NO_S); }

/* 022D32D0 */
u8 daNpcRoten_c::getPrmRailID() {
    return (u8)daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}

/* 022D34E0 */
void daNpcRoten_c::setMtx() {
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 022D2BB8 */
BOOL daNpcRoten_c::initTexPatternAnm(u8 modify) {
    J3DModelData* modelData = roten_modelData(mpMorf->getModel());
    m_head_tex_pattern =
        (J3DAnmTexPattern*)dComIfG_getObjectIDRes(arcname(mNpcNo), l_btp_ix_tbl.entries[mNpcNo]);
    if (m_head_tex_pattern.get() == nullptr) { /* JUT_ASSERT(3117, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_roten.cpp", 0xC2D, "m_head_tex_pattern != (0)");
    }
    if (roten_initBtp(&mBtpAnm, modelData, m_head_tex_pattern.get(), 1, J3DFrameCtrl::EMode_LOOP, 0,
                      -1, (u32)modify, 0, 1.0f) == 0) { /* mBtpAnm.init */
        return FALSE;
    }
    field_0x9B8 = 0;
    field_0x9AA = 0;
    return TRUE;
}

/* 022D50D4 */
void daNpcRoten_c::playTexPatternAnm() {
    if (cLib_calcTimer(&field_0x9AA) == 0) {
        s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)field_0x9B8 >= frameMax) {
            frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            u8 f = field_0x9B8;
            field_0x9AA = 0x78;
            field_0x9B8 = (u8)(f - frameMax);
        } else {
            field_0x9B8 = (u8)(field_0x9B8 + 1);
        }
    }
}

/* 022D5174 */
void daNpcRoten_c::playAnm() {
    if (mpMorf->play(nullptr, 0, 0) && field_0x988.get() != nullptr && field_0x9C2 > 0) {
        s8 n = (s8)(field_0x9C2 - 1);
        field_0x9C2 = n;
        if (n == 0) {
            field_0x988 = field_0x988.get() + 1;
            if (setAnmTbl(field_0x988)) {
                field_0x9C1 = field_0x9C1 | 1;
            }
        } else {
            setAnm(field_0x988->field_0x00, 0, 0.0f);
        }
    }
}

/* 022D43B0 */
void daNpcRoten_c::setAnm(u8 param_1, int param_2, f32 param_3) {
    f32 f = field_0x994;
    if (!(f < 0.0f)) {
        param_3 = f;
        field_0x994 = -1.0f;
    }
    J3DAnmTransform* pAnmRes = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), l_bck_ix_tbl.entries[mNpcNo][param_1]);
    mpMorf->setAnm(pAnmRes, param_2, param_3, 1.0f, 0.0f, -1.0f, nullptr);
    field_0x9C0 = param_1;
    field_0x998 = param_2;
}

/* 022D44D0 */
bool daNpcRoten_c::setAnmTbl(sRotenAnmDat* param_1) {
    field_0x9C1 = field_0x9C1 & ~1;
    if (param_1->field_0x00 == 0xFF) {
        field_0x988 = nullptr;
        return true;
    }
    field_0x988 = param_1;
    s8 cnt = (s8)param_1->field_0x02;
    field_0x9C2 = cnt;
    u8 anm = param_1->field_0x00;
    if (cnt > 0) {
        setAnm(anm, 0, (f32)param_1->field_0x01);
    } else if (field_0x9C0 != anm || field_0x998 == 0) {
        setAnm(anm, 2, (f32)param_1->field_0x01);
    }
    return false;
}

/* 022D62EC */
BOOL daNpcRoten_c::isHaitatuItem(u8 itemNo) {
    return dItemNo_TOWN_FLOWER_e <= itemNo && itemNo <= dItemNo_XXX_039_e;
}

/* 022D32FC */
BOOL daNpcRoten_c::isKoukanItem(u8 itemNo) {
    return dItemNo_TOWN_FLOWER_e <= itemNo && itemNo <= dItemNo_SHOP_GURU_STATUE_e;
}

/* 022D3314 */
BOOL daNpcRoten_c::isGetMap(u8 itemNo) {
    if (mNpcNo == 1 && !dComIfGs_isEventBit(0x3E04) && itemNo == dItemNo_SHOP_GURU_STATUE_e) {
        return TRUE;
    }
    return FALSE;
}

/* 022D3388 */
s16 daNpcRoten_c::XyEventCB(int i_itemBtn) {
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    field_0x9BE = (u8)(itemNo - dItemNo_TOWN_FLOWER_e);
    if (isKoukanItem(itemNo) && dComIfGs_getEventReg(save_dat(mNpcNo, 2)) < 3 &&
        !isGetMap(itemNo)) {
        u8 btn = dComIfGp_event_getTalkXYBtn();
        if (btn == dTalkBtn_X_e) {
            mShownItemBtn = dItemBtn_X_e;
        } else if (btn == dTalkBtn_Y_e) {
            mShownItemBtn = dItemBtn_Y_e;
        } else if (btn == dTalkBtn_Z_e) {
            mShownItemBtn = dItemBtn_Z_e;
        } else {
            mShownItemBtn = dItemBtn_NONE_e;
        }
        field_0x9B2 = field_0x9B2 | 0xC000;
        return field_0x99E;
    }
    if (isGetMap(itemNo)) {
        field_0x9B2 = field_0x9B2 | 0xC000;
        return field_0x9A4;
    }
    return dComIfGp_evmng_getEventIdx("DEFAULT_TALK_XY", 0xFF); /* "DEFAULT_TALK_XY" */
}

/* 022D35C0 */
void daNpcRoten_c::setCollisionB() {
    Mtx34* pMtx = getAnmMtx(mpMorf->getModel(), m_bag_jnt_num);
    gabi::Local<cXyz> temp;
    temp->x = pMtx->m[0][3]; /* mDoMtx_multVecZero */
    temp->y = pMtx->m[1][3];
    temp->z = pMtx->m[2][3];
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> angle;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = temp->x;
    b->y = temp->y;
    b->z = temp->z;
    dNpc_calc_DisXZ_AngY(a, b, nullptr, angle);
    s16 ang = *angle;
    NpcDatStruct& dat = npc_dat(mNpcNo);
    temp->x = gabi::fmadds(cM_ssin(ang), dat.field_0x34, temp->x);
    temp->y = current.pos.y;
    temp->z = gabi::fmadds(cM_scos(ang), dat.field_0x34, temp->z);
    mCyl2.SetC(temp);
    mCyl2.SetR(npc_dat(mNpcNo).field_0x30);
    mCyl2.SetH(200.0f);
    dComIfG_Ccsp_Set(&mCyl2);
}

/* 022D5238 */
void daNpcRoten_c::setCollisionH() {
    Mtx34* mtx = getAnmMtx(mpMorf->getModel(), m_jnt.mHeadJntNum);
    PSMTXCopy(mtx, calc_mtx());
    mDoMtx_stack_c::transS(npc_dat(mNpcNo).field_0x3C, 0.0f, 0.0f);
    roten_PSMTXConcat(calc_mtx(), mDoMtx_stack_c::get(), calc_mtx());
    gabi::Local<cXyz> temp;
    Mtx34* m = calc_mtx();
    temp->x = m->m[0][3];
    temp->y = m->m[1][3];
    temp->z = m->m[2][3];
    mSph.SetC(temp);
    mSph.SetR(npc_dat(mNpcNo).field_0x38);
    dComIfG_Ccsp_Set(&mSph);
    if (field_0x9CA == 0 && mSph.ChkTgHit()) {
        executeSetMode(MODE_WIND);
        field_0x9CA = 1;
    }
}

/* 022D3B30 */
static cPhs_State daNpc_RotenCreate(void* i_this) { return ((daNpcRoten_c*)i_this)->_create(); }

/* 022D3B98 */
static BOOL daNpc_RotenDelete(void* i_this) { return ((daNpcRoten_c*)i_this)->_delete(); }

/* 022D578C */
static BOOL daNpc_RotenExecute(void* i_this) { return ((daNpcRoten_c*)i_this)->_execute(); }

/* 022D58F0 */
static BOOL daNpc_RotenDraw(void* i_this) { return ((daNpcRoten_c*)i_this)->_draw(); }

/* 022D6744 */
static BOOL daNpc_RotenIsDelete(void*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 022D680C */
static u32 daObj_PrmAbstract(fopAc_ac_c* actor, s32 width, s32 shift) {
    u32 prm = actor->mParameters;
    u32 bit = (width & 0x20) ? 0 : (1u << (width & 0x1F));
    u32 v = (shift & 0x20) ? 0 : (prm >> (shift & 0x1F));
    return v & (bit - 1);
}

/* 022D669C */
static void __sinit_d_a_npc_roten_cpp() {
    sinit_header_statics(l_staticObjects, l_staticDescriptors);
}

/* 022D6730 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 022D6754 */
static void daNpcRoten_c_dt(daNpcRoten_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        roten_destroySphere(&i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        roten_destroyCircle(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
        reinterpret_cast<RotenAcchVtables*>(&i_this->mObjAcch)->cBgS_Chk_vtable = l_acchBaseVtable;
        reinterpret_cast<RotenAcchVtables*>(&i_this->mObjAcch)->dBgS_Chk_vtable = l_acchVtable;
        roten_destroyAcch(&i_this->mObjAcch, 0);
        roten_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 022D6808 */
static void SafeString_assureTerminationImpl(SafeString*) {}

// clang-format off
/* WWHD glue
data l_arcname_tbl 101C5A38
data l_npc_staff_id 101C5D3C
data l_npc_dat 101C5D54
data l_bmd_ix_tbl 100212D8
data l_head_bmd_ix_tbl 100212E4
data l_head_bck_ix_tbl 100212F0
data l_btp_ix_tbl 100212FC
data l_bck_ix_tbl 1002132C
data l_save_dat 100212C0
data l_item_dat 10021308
data l_npc_anm_wait 101C5478
data l_npc_anm_walk 101C547E
data l_npc_anm_wind 101C5D48
data l_sph_src 101C5988
data dNpc_cyl_src 101EA190
data l_msg_xy_koukan_end 101C5CCC
data l_msg_xy_koukan_first 101C5CD8
data l_msg_xy_koukan_no 101C5CB4
data l_msg_xy_koukan_rupee 101C5CC0
data l_msg_try_force 101C5D20
data l_msg_collect_map 101C5464
data l_msg_xy_exchange3 101C5AE0
data l_msg_xy_no_roten_item 101C5AEC
data l_msg_xy_invalid_item 101C5AF8
data l_msg_1st_talk 101C5A44
data l_msg_exchange3 101C5A50
data l_msg_exchange 101C5A80
data l_msg_etc 101C5AB0
data l_msg_xy_koukan_item 101C5E50
data l_msg_xy_koukan_item2 101C5EE0
data l_msg_xy_koukan_item3 101C5F70
data l_msg_xy_after_get_demo 101C5D08
data l_msg_xy_after_get_demo2 101C5D14
data l_msg_anm 100215D0
data l_get_item_no 101C546C
data l_currentModel 104B462C
data l_currentMtx 104B4868
data l_messageManager 101F4B5C
ext 025D54C4 BOOL roten_fopAcM_SearchByID_o(u32 id, gptr<fopAc_ac_c>* out)
ext 025D7970 BOOL roten_fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind)
ext 0218432C void daItemBase_dead(fopAc_ac_c* a)
ext 021842C8 void daItemBase_show(fopAc_ac_c* a)
ext 0259DED0 void roten_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 0259E6D0 void roten_dNpc_PathRun_setInf(dNpc_PathRun_r* r, u8 id, s8 room, u8 b)
ext 0259E778 void roten_dNpc_PathRun_getPoint(dNpc_PathRun_r* r, cXyz* out, u8 idx)
ext 0259EB60 void roten_dNpc_PathRun_incIdxLoop(dNpc_PathRun_r* r)
ext 0259E838 BOOL roten_dNpc_PathRun_chkPointPass(dNpc_PathRun_r* r, cXyz* pos, u32 dir)
ext 0259ED58 BOOL roten_dNpc_PathRun_nextIdxAuto(dNpc_PathRun_r* r)
ext 028E9108 void roten_PSMTXConcat(Mtx34* a, Mtx34* b, Mtx34* ab)
ext 025A1458 void roten_constructNpc(daNpcRoten_c* actor)
ext 025166F0 void roten_constructSphere(dCcD_Sph* sphere)
ext 025E789C s32 roten_initBtp(mDoExt_btpAnm* animator,J3DModelData* model,J3DAnmTexPattern* animation,s32 play,s32 mode,s32 start,s32 end,u32 modify,s32 entry,f32 speed)
ext 027DF9B0 s8 roten_getJointIndex(u32 table,const char* name)
ext 02525FE4 cPhs_State roten_phaseHandler(request_of_phase_process_class* phase,void* handlers,daNpcRoten_c* actor)
ext 02515AE8 void roten_destroySphere(dCcD_Sph* sphere,s32 flags)
ext 02018034 void roten_destroyCircle(void* circle,s32 flags)
ext 024EFD9C void roten_destroyAcch(dBgS_ObjAcch* acch,s32 flags)
ext 025D50BC void roten_destroyActor(fopAc_ac_c* actor,s32 flags)
ext 025D7DEC u32 roten_createPresentItem(cXyz* position,u32 itemNo,u32 flags,s32 bitNo,s32 room,u32 angle,u32 scale)
ext 02542EDC s8 roten_getActionIndex(void* manager,s32 staff,u32 names,s32 count,BOOL force,s32 nameType)
data l_execute_init 101C59C8
data moveProc 101C59F0
data cut_name_tbl 101C600C
data l_method 101C6000
str 100213A8 "d_a_npc_roten.cpp" in 022D2BB8
str 100213BC "m_head_tex_pattern != (0)" in 022D2BB8
str 100213E4 "head" in 022D2CCC
str 100213F4 "d_a_npc_roten.cpp" in 022D2CCC
str 1002142C "m_jnt.getHeadJntNum() >= 0" in 022D2CCC
str 10021408 "backbone1" in 022D2CCC
str 10021448 "m_jnt.getBackboneJntNum() >= 0" in 022D2CCC
str 100213DC "handL" in 022D2CCC
str 10021414 "m_hand_L_jnt_num >= 0" in 022D2CCC
str 100213EC "Bag1" in 022D2CCC
str 10021468 "m_bag_jnt_num >= 0" in 022D2CCC
str 100214F4 "d_a_npc_roten.cpp" in 022D3A44
str 10021508 "(npc_no >= 0) && (npc_no < (sizeof(l_arcname_tbl) / sizeof(l_arcname_tbl[0])))" in 022D3A44
str 100214CC "ROTEN_EXCHANGE_1ST" in 022D3710
str 100214E0 "ROTEN_EXCHANGE_2ND" in 022D3710
str 100214A8 "ROTEN_CHANGE_ITEM" in 022D3710
str 100214BC "ROTEN_GET_MAP" in 022D3710
str 10021484 "DEFAULT_TALK_XY" in 022D3388
*/
// clang-format on
