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

/* 022D6828 */
int daNpc_Rsh1_countShop() {
    WWHD_FUNC(0x022D6828, int);
    u8 i = 0;
    int result = 0;
    for (; i <= 11; i++) {
        if (rsh1_isGetItemReserve(i)) {
            result++;
        } else if (i == 0 && dComIfGs_checkGetItem_l(0x30 /* dItemNo_DELIVERY_BAG_e */)) {
            result++;
        }
    }
    return result;
}
VERIFY(0x022D6828, daNpc_Rsh1_countShop);

/* 022D68B8 */
BOOL daNpc_Rsh1_shopMsgCheck(u32 param_1) {
    WWHD_FUNC(0x022D68B8, BOOL, param_1);
    if ((param_1 >= 0x286B && param_1 <= 0x2882) || (param_1 >= 0x2868 && param_1 <= 0x286A) || param_1 == 0x2863 ||
        param_1 == 0x2884) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022D68B8, daNpc_Rsh1_shopMsgCheck);

/* 022D68F0 */
BOOL daNpc_Rsh1_shopStickMoveMsgCheck(u32 param_1) {
    WWHD_FUNC(0x022D68F0, BOOL, param_1);
    if ((param_1 >= 0x286B && param_1 <= 0x2882 && (param_1 % 2 != 0)) || param_1 == 0x2863) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022D68F0, daNpc_Rsh1_shopStickMoveMsgCheck);

/* 022D691C */
static BOOL nodeCallBack_Rsh(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022D691C, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model_p = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Rsh1_c* actor_p = gabi::at<daNpc_Rsh1_c>(gabi::load<u32>(gabi::ea(model_p) + 0xB8));
        J3DJoint* jnt_p = J3DNode_toJoint(i_node);
        u32 jnt_no = gabi::load<u16>(gabi::ea(jnt_p) + 4);
        if (actor_p) {
            PSMTXCopy(gabi::at<Mtx34>(rsh1_getAnmMtx(model_p, jnt_no)), calc_mtx());
            if (jnt_no == (u32)(s32)actor_p->m_head_jnt_num) {
                gabi::Local<cXyz> temp1;
                gabi::Local<cXyz> temp2;
                temp1->x = 0.0f;
                temp1->y = 0.0f;
                temp1->z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-actor_p->mJntCtrl.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-actor_p->mJntCtrl.mAngles[0][0]);
                MtxPosition(temp1.get(), temp2.get());
                f32 x = temp2->x, y = temp2->y, z = temp2->z;
                actor_p->mAttnBasePos.z = z;
                actor_p->mAttnBasePos.y = y;
                actor_p->mAttnBasePos.x = x;
                temp1->x = 28.0f;
                temp1->z = 0.0f;
                temp1->y = 20.0f;
                MtxPosition(temp1.get(), temp2.get());
                x = temp2->x;
                z = temp2->z;
                y = temp2->y;
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
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* j3dSys.mCurrentMtx */
            u32 anm = rsh1_getAnmMtx(model_p, jnt_no);         /* setAnmMtx */
            rsh1_mtx_copy(anm, gabi::ea(calc_mtx()));
        }
    }
    return TRUE;
}
VERIFY(0x022D691C, nodeCallBack_Rsh);

/* 022D6BB0 */
BOOL daNpc_Rsh1_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x022D6BB0, BOOL, this, i_modify);
    J3DModelData* morf_model_data_p = J3DModel_getModelData(mpMorf->getModel());
    s32 idx = gabi::load<s32>(0x10021640 + (s32)m958 * 4); /* l_btp_ix_tbl */
    m_head_tex_pattern = (J3DAnmTexPattern*)rsh1_getRes(idx);
    if (!m_head_tex_pattern) {
        JUT_ASSERT_fail(STR(0x10021728), 0x24C, STR(0x1002170C));
    }
    if (rsh1_btpInit(mBtpAnm, morf_model_data_p, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, i_modify, 0) == 0) {
        return FALSE;
    }
    mBtpFrame = 0;
    mTimer = 0;
    return TRUE;
}
VERIFY(0x022D6BB0, &daNpc_Rsh1_c::initTexPatternAnm);

/* 022D6CC4 */
BOOL daNpc_Rsh1_c::CreateHeap() {
    WWHD_FUNC(0x022D6CC4, BOOL, this);
    J3DModelData* model_p = (J3DModelData*)rsh1_getRes(0x20);
    void* anm = rsh1_getRes(0x1B);
    void* bas = rsh1_getRes(0xF);
    mpMorf = mDoExt_McaMorf::create(nullptr, model_p, nullptr, nullptr, (J3DAnmTransform*)anm, 2, 1.0f, 0, -1, 1, bas, 0,
                                    0x11020203);
    if (!mpMorf || !mpMorf->getModel()) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = rsh1_getJointIndex(model_p, STR(0x10021740) /* "head" */);
    if (m_head_jnt_num < 0) {
        JUT_ASSERT_fail(STR(0x10021774), 0x9DB, STR(0x10021760));
    }
    m_backbone_jnt_num = rsh1_getJointIndex(model_p, STR(0x10021788) /* "backbone" */);
    if (m_backbone_jnt_num < 0) {
        JUT_ASSERT_fail(STR(0x10021774), 0x9DE, STR(0x10021748));
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
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    void* cur_brk = rsh1_getRes(0x26); /* GHS evaluates the second argument first */
    void* cur_mdl = rsh1_getRes(0x23);
    mpShopCursor = rsh1_ShopCursor_create(cur_mdl, cur_brk, rsh1_HIO().m34);
    return mpShopCursor != nullptr ? TRUE : FALSE;
}
VERIFY(0x022D6CC4, &daNpc_Rsh1_c::CreateHeap);

/* 022D6FD8 (CheckCreateHeap; unnamed by the matcher) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022D6FD8, BOOL, i_this);
    return ((daNpc_Rsh1_c*)i_this)->CreateHeap();
}
VERIFY(0x022D6FD8, CheckCreateHeap);

/* 022D729C */
void daNpc_Rsh1_c::set_mtx() {
    WWHD_FUNC(0x022D729C, void, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x + m7A0.x, current.pos.y + m7A0.y, current.pos.z + m7A0.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    rsh1_mtx_copy(gabi::ea(morf_model_p) + 0xC8, gabi::ea(mDoMtx_stack_c::get()));
}
VERIFY(0x022D729C, &daNpc_Rsh1_c::set_mtx);

/* 022D7998 (_create; unnamed by the matcher) */
cPhs_State daNpc_Rsh1_c::_create() {
    WWHD_FUNC(0x022D7998, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Rsh1_c): the inline constructor (HD virtual destructor) */
    if (!(actor_condition & 8)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            gabi::store<u32>(gabi::ea(this) + 0xB4, RSH1_VTBL);
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, dBgS_ObjAcch_vt{0x10021674, 0x10021694, 0x10021684});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10021664);
            gabi::call(0x0259DAA0, &mJntCtrl);  /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c::dNpc_EventCut_c */
            mSTControl.__vtbl = 0x10050788;
            gabi::call(0x025885C4, &mSTControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000); /* setWaitParm */
            gabi::call(0x025885E8, &mSTControl);                                        /* init */
            gabi::call(0x025BBDF0, &mShopCamAct); /* ShopCam_action_c::ShopCam_action_c */
            gabi::call(0x028EFFD0, mShopItemsArr, 4, 0x44, 0x025BC760); /* __construct_array(ShopItems_c) */
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
            hio.mNo = mDoHIO_createChild(STR(0x100217C0) /* "露店の社長" */, &hio);
        }
        hio.m08 = hio.m08 + 1;
        if (!CreateInit()) {
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022D7998, &daNpc_Rsh1_c::_create);

/* 022D7BF0 */
static cPhs_State daNpc_Rsh1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022D7BF0, cPhs_State, i_this);
    return ((daNpc_Rsh1_c*)i_this)->_create();
}
VERIFY(0x022D7BF0, daNpc_Rsh1_Create);

/* 022D7BF4 */
BOOL daNpc_Rsh1_c::_delete() {
    WWHD_FUNC(0x022D7BF4, BOOL, this);
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
VERIFY(0x022D7BF4, &daNpc_Rsh1_c::_delete);

/* 022D7C78 */
static BOOL daNpc_Rsh1_Delete(daNpc_Rsh1_c* i_this) {
    WWHD_FUNC(0x022D7C78, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022D7C78, daNpc_Rsh1_Delete);

/* 022D7C7C */
void daNpc_Rsh1_c::playTexPatternAnm() {
    WWHD_FUNC(0x022D7C7C, void, this);
    if (gabi::call<s16>(0x02055B64, &mTimer) == 0) { /* cLib_calcTimer */
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
VERIFY(0x022D7C7C, &daNpc_Rsh1_c::playTexPatternAnm);

/* 022D7D40 (talkInit; unnamed by the matcher) */
void daNpc_Rsh1_c::talkInit() {
    WWHD_FUNC(0x022D7D40, void, this);
    m961 = 0;
}
VERIFY(0x022D7D40, &daNpc_Rsh1_c::talkInit);

/* 022D8380 */
u32 daNpc_Rsh1_c::setTexAnm(s8 param_1) {
    WWHD_FUNC(0x022D8380, u32, this, param_1);
    if (m958 != param_1 || m958 == -1) {
        m958 = param_1;
        return initTexPatternAnm(true);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x022D8380, &daNpc_Rsh1_c::setTexAnm);

/* 022D8A44 */
BOOL daNpc_Rsh1_c::_draw() {
    WWHD_FUNC(0x022D8A44, BOOL, this);
    J3DModel* morf_model_p = mpMorf->getModel();
    J3DModelData* morf_model_data_p = J3DModel_getModelData(morf_model_p);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    gabi::call(0x025E7B3C, mBtpAnm, morf_model_data_p, (u32)(u8)mBtpFrame); /* mBtpAnm.entry */
    mpMorf->updateDL();
    gabi::store<u32>(gabi::ea(morf_model_data_p) + 0x38, 0); /* mBtpAnm.remove (inline) */
    /* HD: no blob shadow */
    ShopItems_c_l* items = mpShopItems;
    if (items && items->mSelectedItemIdx >= 0) {
        mpShopCursor->draw();
    }
    gabi::call(0x025BEBB8, 0x5F /* DSNAP_TYPE_NPC_RSH1 */, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x022D8A44, &daNpc_Rsh1_c::_draw);

/* 022D8A40 */
static BOOL daNpc_Rsh1_Execute(daNpc_Rsh1_c* i_this) {
    WWHD_FUNC(0x022D8A40, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022D8A40, daNpc_Rsh1_Execute);

/* 022D8B14 */
static BOOL daNpc_Rsh1_Draw(daNpc_Rsh1_c* i_this) {
    WWHD_FUNC(0x022D8B14, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022D8B14, daNpc_Rsh1_Draw);

/* 022D8B18 */
static BOOL daNpc_Rsh1_IsDelete(daNpc_Rsh1_c*) {
    WWHD_FUNC(0x022D8B18, BOOL, (daNpc_Rsh1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022D8B18, daNpc_Rsh1_IsDelete);

/* 022D8B20 */
void daNpc_Rsh1_c::setAnm(s8 i_index) {
    WWHD_FUNC(0x022D8B20, void, this, i_index);
    if (i_index != m959 || m959 == -1) {
        m959 = i_index;
        s32 i = i_index;
        rsh1_dNpc_setAnm_2(mpMorf, gabi::load<s32>(0x101C60E4 + i * 4) /* play_mode_tbl */,
                           gabi::load<f32>(0x101C6100 + i * 4) /* morf_frame_tbl */,
                           gabi::load<f32>(0x101C611C + i * 4) /* play_speed_tbl */,
                           gabi::load<s32>(0x100216C4 + i * 4) /* l_bck_ix_tbl */,
                           gabi::load<s32>(0x100216E0 + i * 4) /* l_bas_ix_tbl */, STR(RSH1_ARC));
    }
}
VERIFY(0x022D8B20, &daNpc_Rsh1_c::setAnm);

/* 022DB690 */
static daNpc_Rsh1_HIO_c* daNpc_Rsh1_HIO_ct(daNpc_Rsh1_HIO_c* self) {
    WWHD_FUNC(0x022DB690, daNpc_Rsh1_HIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_Rsh1_HIO_c*)operator_new(0x6C);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x100216B4;
    gabi::call(0x0259DA18, &self->mNpcHIO); /* dNpc_HIO_c::dNpc_HIO_c */
    dNpc_HIO_c_l& n = self->mNpcHIO;
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
VERIFY(0x022DB690, daNpc_Rsh1_HIO_ct);

/* 022D6FDC */
void daNpc_Rsh1_c::createShopList() {
    WWHD_FUNC(0x022D6FDC, void, this);
    gabi::Local<csXyz> temp;
    csXyz_ct(temp.get(), 0, home.angle.y, 0);
    static const s16 y_vals[] = {(s16)0xE800, (s16)0xE400, (s16)0xE000, (s16)0xDC00};
    int i, k;
    s32 n;
    for (k = 0, i = 0;; i++) {
        int j = 0;
        ShopItems_c_l* items = &mShopItemsArr[i];
        items->mItemSetListGlobalIdx = 8; /* setItemDataIdx(8) */
        temp->y = y_vals[i];
        while (j < 3 && k < 12) {
            int idx = i * 3 + j;
            mShopItemDataPtrs[k] = 0;
            BOOL got;
            if (k != 0) {
                got = rsh1_isGetItemReserve((u8)k);
            } else {
                got = dComIfGs_checkGetItem_l(0x30 /* dItemNo_DELIVERY_BAG_e */) ? TRUE : FALSE;
            }
            if (got) {
                u32 data = gabi::load<u32>(0x101EB940 + k * 4); /* Item_setData_rshop[k] */
                u32 itemNo = gabi::load<u32>(gabi::load<u32>(data));
                s8 room = fopAcM_GetRoomNo(this);
                u32 id = gabi::call<u32>(0x025D87E4, 0x101EB998 + idx * 12 /* Item_set_pos_data_rshop_0 */, itemNo, temp.get(),
                                         (s32)room, 0, 0); /* fopAcM_createShopItem */
                items->mItemActorProcessIds[j] = id;
                mShopItemDataPtrs[idx] = gabi::load<u32>(0x101EB940 + k * 4);
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
            gabi::call(0x025BCFA0, &mShopItemsArr[i], &mShopItemDataPtrs[i * 3]); /* setItemSetDataList */
        }
        n = m78C;
    }
    m78C = n > 1 ? n : 1;
}
VERIFY(0x022D6FDC, &daNpc_Rsh1_c::createShopList);

/* 022D71D4 */
bool daNpc_Rsh1_c::pathGet() {
    WWHD_FUNC(0x022D71D4, bool, this);
    u32 param = (mParameters >> 0x10) & 0xFF;
    dPath* path = dPath_GetRoomPath(param, fopAcM_GetRoomNo(this));
    mpPath = path;
    if (path) {
        u32 pnt = gabi::load<u32>(gabi::ea(path) + 8); /* m_points */
        for (int i = 0; i < (s32)gabi::load<u16>(gabi::ea(path)); i++, pnt += 0x10) {
            u8 arg3 = gabi::load<u8>(pnt + 3);
            if (arg3 != 0xFF) {
                f32 y = gabi::load<f32>(pnt + 8);
                f32 x = gabi::load<f32>(pnt + 4);
                f32 z = gabi::load<f32>(pnt + 0xC);
                mPathPointPos[arg3].x = x;
                mPathPointPos[arg3].y = y;
                mPathPointPos[arg3].z = z;
                m744[gabi::load<u8>(pnt + 3)] = (s8)i;
                if (gabi::load<u8>(pnt + 3) == 1) {
                    m704 = (s8)i;
                }
            }
        }
    }
    mShopIdx = 0;
    return true;
}
VERIFY(0x022D71D4, &daNpc_Rsh1_c::pathGet);

/* 022D737C */
BOOL daNpc_Rsh1_c::checkCreateInShopPlayer() {
    WWHD_FUNC(0x022D737C, BOOL, this);
    fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); /* daPy_getPlayerLinkActorClass() */
    s16 ay = current.angle.y;
    gabi::Local<cXyz> dir;
    gabi::Local<cXyz> link_pos;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> cross;
    gabi::Local<cXyz> base_y;
    dir->z = cM_ssin(ay);
    dir->y = 0.0f;
    dir->x = -cM_scos(ay);
    link_pos->x = link_p->current.pos.x;
    link_pos->y = link_p->current.pos.y;
    link_pos->z = link_p->current.pos.z;
    cXyz_mi(link_pos.get(), diff.get(), &current.pos);
    static const u32 chk_pos_tbls[] = {0x104684D0 /* l_in_chk_pos1_tbl */, 0x10468500 /* l_in_chk_pos2_tbl */};
    for (int i = 0; i < 2; i++) {
        f32 bx = gabi::load<f32>(0x101FFBC0); /* cXyz::BaseY */
        f32 by = gabi::load<f32>(0x101FFBC4);
        f32 bz = gabi::load<f32>(0x101FFBC8);
        u32 tbl = chk_pos_tbls[i];
        int j = 0, k = 1, l = 0;
        while (j < 4) {
            cXyz_mi(gabi::at<cXyz>(tbl + j * 12), temp2.get(), link_pos.get());
            cXyz_mi(gabi::at<cXyz>(tbl + k * 12), temp3.get(), link_pos.get());
            gabi::call(0x0201B080, temp2.get(), cross.get(), temp3.get()); /* cXyz::outprod */
            base_y->x = bx;
            base_y->y = by;
            base_y->z = bz;
            if (PSVECDotProduct(base_y.get(), cross.get()) > 0.0f) {
                l++;
            }
            k++;
            j++;
            if (k > 3) k = 0;
        }
        if ((l == 4 || l == 0) && PSVECDotProduct(diff.get(), dir.get()) > 0.0f && std::fabs((f32)diff->y) < 25.0f) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022D737C, &daNpc_Rsh1_c::checkCreateInShopPlayer);

/* 022D75E8 */
BOOL daNpc_Rsh1_c::CreateInit() {
    WWHD_FUNC(0x022D75E8, BOOL, this);
    mActorAngle.x = current.angle.x;
    mActorAngle.y = current.angle.y;
    mActorAngle.z = current.angle.z;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAD); /* attention_info.distances[TYPE_BATTLE] = 173 */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAD); /* HD: the GameCube out-of-range distances[8] write lands here */
    gravity = -30.0f;
    if (m95E == 0) {
        rsh1_setAction(this, RSH1_wait_action);
    }
    rsh1_copy12(gabi::ea(&mAttnBasePos), gabi::ea(&current.pos));
    gabi::call(0x02515F14, &mStts, 0xFF, 0xFF, this); /* mStts.Init */
    gabi::call(0x02516518, &mCyl, 0x101C60A0);         /* mCyl.Set(l_cyl_src) */
    gabi::store<u32>(gabi::ea(&mCyl) + 0x44, gabi::ea(&mStts)); /* SetStts */
    m962 = 0;
    m749 = 0;
    m780 = 0;
    mShopSelectItemNo = 0xFF;
    mShopCamAct.mCamDataIdx = 8;                 /* setCamDataIdx(8) */
    gabi::store<s16>(gabi::ea(&mShopCamAct) + 0, 0); /* setCamAction(NULL) */
    gabi::store<s16>(gabi::ea(&mShopCamAct) + 2, 0);
    gabi::store<u32>(gabi::ea(&mShopCamAct) + 4, 0);
    createShopList();
    s32 idx = mShopIdx;
    if (idx != -1) {
        ShopItems_c_l* it = &mShopItemsArr[idx]; /* init() */
        it->mSelectedItemIdx = -1;
        it->mbIsHide = 0;
        it->m3C = 0;
    }
    mpShopItems = nullptr;
    mShopIdx = -1;
    pathGet();
    gabi::store<f32>(gabi::ea(this) + 0x364, 0.25f); /* fopAcM_setCullSizeFar(this, 0.25f) */
    set_mtx();
    mpMorf->setMorf(0.0f);
    u32 zx = gabi::load<u32>(0x101FFBA8), zy = gabi::load<u32>(0x101FFBAC), zz = gabi::load<u32>(0x101FFBB0); /* cXyz::Zero */
    gabi::store<u32>(gabi::ea(&m794) + 0, zx);
    gabi::store<u32>(gabi::ea(&m794) + 4, zy);
    gabi::store<u32>(gabi::ea(&m794) + 8, zz);
    m793 = 0;
    gabi::store<u32>(gabi::ea(&m7A0) + 4, zy);
    gabi::store<u32>(gabi::ea(&m7A0) + 8, zz);
    gabi::store<u32>(gabi::ea(&m7A0) + 0, zx);
    gabi::call(0x0259F7D4, &mEventCut, STR(0x100217A0) /* "Rsh1" */, this); /* setActorInfo */
    gabi::store<u32>(gabi::ea(&mEventCut) + 0x68, gabi::ea(&mJntCtrl));     /* setJntCtrlPtr */
    if (checkCreateInShopPlayer()) {
        m95B = 5;
        actor_status = actor_status | 0x4000;
        mShopOutEventIdx = rsh1_getEventIdx(0x100217A8 /* "RSH_SHOP_OUT" */);
        rsh1_setAction(this, RSH1_dummy_action);
    } else {
        m95B = 0;
    }
    return TRUE;
}
VERIFY(0x022D75E8, &daNpc_Rsh1_c::CreateInit);

/* 022D7D4C */
void daNpc_Rsh1_c::checkOrder() {
    WWHD_FUNC(0x022D7D4C, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo command */
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
            fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34));
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
VERIFY(0x022D7D4C, &daNpc_Rsh1_c::checkOrder);

/* daNpc_Rsh1_checkRotenBaseTalkArea (inlined into eventOrder in WWHD): base_talk_area_tbl is a
 * function-local static, initialised on first use (guard 0x104684C8, table 0x10468530) */
static inline BOOL rsh1_checkRotenBaseTalkArea_inl() {
    u32 tbl = 0x10468530;
    if (gabi::load<u32>(0x104684C8) == 0) {
        gabi::store<u32>(0x104684C8, 1);
        gabi::store<f32>(tbl + 0x18, 930.0f);
        gabi::store<f32>(tbl + 0x04, 671.0f);
        gabi::store<f32>(tbl + 0x28, 671.0f);
        gabi::store<f32>(tbl + 0x10, 671.0f);
        gabi::store<f32>(tbl + 0x1C, 671.0f);
        gabi::store<f32>(tbl + 0x08, -204393.0f);
        gabi::store<f32>(tbl + 0x24, 971.0f);
        gabi::store<f32>(tbl + 0x14, -204321.0f);
        gabi::store<f32>(tbl + 0x20, -204114.0f);
        gabi::store<f32>(tbl + 0x2C, -204189.0f);
        gabi::store<f32>(tbl + 0x00, 580.0f);
        gabi::store<f32>(tbl + 0x0C, 539.0f);
    }
    fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); /* daPy_getPlayerLinkActorClass() */
    gabi::Local<cXyz> link_pos;
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> cross;
    gabi::Local<cXyz> base_y;
    link_pos->x = link_p->current.pos.x;
    f32 bx = gabi::load<f32>(0x101FFBC0); /* cXyz::BaseY */
    link_pos->y = link_p->current.pos.y;
    f32 by = gabi::load<f32>(0x101FFBC4);
    link_pos->z = link_p->current.pos.z;
    f32 bz = gabi::load<f32>(0x101FFBC8);
    int j = 0, k = 1, l = 0;
    while (j < 4) {
        cXyz_mi(gabi::at<cXyz>(tbl + j * 12), temp2.get(), link_pos.get());
        cXyz_mi(gabi::at<cXyz>(tbl + k * 12), temp3.get(), link_pos.get());
        gabi::call(0x0201B080, temp2.get(), cross.get(), temp3.get()); /* cXyz::outprod */
        base_y->x = bx;
        base_y->y = by;
        base_y->z = bz;
        if (PSVECDotProduct(base_y.get(), cross.get()) > 0.0f) {
            l++;
        }
        k++;
        j++;
        if (k > 3) k = 0;
    }
    return l == 4 || l == 0;
}

/* 022D808C (matcher: "daNpc_Rsh1_checkRotenBaseTalkArea"; it is eventOrder with that check inlined) */
void daNpc_Rsh1_c::eventOrder() {
    WWHD_FUNC(0x022D808C, void, this);
    s8 m = m95B;
    if (m == 5) {
        gabi::call(0x025D7A58, this, (s32)mShopOutEventIdx, 0xFF, 0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
        return;
    }
    if (m == 4) {
        gabi::call(0x025D77DC, this, STR(0x100217F4) /* "RSH_GET_DEMO" */, 1, 0xFFFF); /* fopAcM_orderOtherEvent */
        return;
    }
    if (m == 3) {
        if (mShopIdx != -1 || rsh1_checkRotenBaseTalkArea_inl()) {
            gabi::store<u16>(gabi::ea(this) + 0xFA, (u16)(gabi::load<u16>(gabi::ea(this) + 0xFA) | 1)); /* CANTALK */
        }
    } else if (m == 1 || m == 2) {
        gabi::store<u16>(gabi::ea(this) + 0xFA, (u16)(gabi::load<u16>(gabi::ea(this) + 0xFA) | 1));
    } else {
        return;
    }
    if (m95B == 1 || m95B == 2) {
        fopAcM_orderSpeakEvent(this);
    }
}
VERIFY(0x022D808C, &daNpc_Rsh1_c::eventOrder);

/* 022D83A8 */
void daNpc_Rsh1_c::lookBack() {
    WWHD_FUNC(0x022D83A8, void, this);
    gabi::Local<cXyz> player_eye_pos;
    cXyz* vec_p = nullptr;
    s16 temp2 = current.angle.y;
    u8 temp = 1;
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    switch (m95C) {
    case 1:
        if (mbAttention) {
            gabi::Local<cXyz> eye;
            gabi::call(0x0259D54C, eye.get(), (f32)hio.mNpcHIO.m04); /* dNpc_playerEyePos */
            rsh1_copy12(gabi::ea(player_eye_pos.get()), gabi::ea(eye.get()));
            vec_p = player_eye_pos.get();
            setTexAnm(0);
        } else {
            vec_p = nullptr;
            setTexAnm(0);
        }
        break;
    case 2:
        if (mpShopItems) {
            s16 ci = gabi::load<s16>(gabi::ea(&mShopCamAct) + 2);
            bool match = ci == 0 || (ci == -1 && gabi::load<s16>(gabi::ea(&mShopCamAct)) == 0 &&
                                     gabi::load<u32>(gabi::ea(&mShopCamAct) + 4) == 0x025BC38C /* rsh_talk_cam_action */);
            if (match) {
                gabi::Local<cXyz> eye;
                gabi::call(0x0259D54C, eye.get(), (f32)hio.mNpcHIO.m04);
                rsh1_copy12(gabi::ea(player_eye_pos.get()), gabi::ea(eye.get()));
                temp = 0;
            } else if (mpShopItems->mSelectedItemIdx == -1) {
                /* mShopCamAct.getItemZoomPos(125.0f) (inline) */
                gabi::Local<cXyz> dir;
                gabi::Local<cXyz> ofs;
                gabi::Local<cXyz> res;
                cXyz_mi(&mShopCamAct.m18, dir.get(), &mShopCamAct.m24);
                if (!gabi::call<bool>(0x0201B47C, dir.get())) { /* normalizeRS */
                    dir->x = 0.0f;
                    dir->z = 1.0f;
                    dir->y = 0.0f;
                }
                gabi::call(0x0201AE48, dir.get(), ofs.get(), 125.0f);           /* cXyz::operator* */
                gabi::call(0x0201AD78, &mShopCamAct.m24, res.get(), ofs.get()); /* cXyz::operator+ */
                f32 z = res->z, y = res->y, x = res->x;
                temp = 0;
                player_eye_pos->y = y;
                player_eye_pos->z = z;
                player_eye_pos->x = x;
            } else {
                gabi::Local<cXyz> out;
                gabi::Local<cXyz> base;
                mpShopItems->getSelectItemBasePos(out.get());
                rsh1_copy12(gabi::ea(base.get()), gabi::ea(out.get()));
                mpShopItems->getSelectItemPos(out.get());
                rsh1_copy12(gabi::ea(player_eye_pos.get()), gabi::ea(out.get()));
                mpShopCursor->setPos(base.get());
                f32 a = hio.m34, d = hio.m40, c = hio.m3C, b = hio.m38, e = hio.m44;
                ShopCursor_c_l* cur = mpShopCursor;
                cur->mAC = b;
                cur->mA8 = a;
                cur->mB0 = c;
                cur->m98 = d;
                cur->m9C = e;
                mpShopCursor->anm_play();
            }
            vec_p = player_eye_pos.get();
        } else {
            vec_p = nullptr;
        }
        break;
    default:
        break;
    }
    gabi::Local<cXyz> eye_copy;
    s16 vel;
    if (mJntCtrl.mbTrn != 0) { /* trnChk */
        gabi::call(0x0200F428, &mLookAtMaxVel, (s32)hio.mNpcHIO.mMaxHeadTurnVel, 4, 0x800); /* cLib_addCalcAngleS2 */
        eye_copy->x = eyePos.x;
        vel = mLookAtMaxVel;
        eye_copy->z = eyePos.z;
        eye_copy->y = eyePos.y;
    } else {
        eye_copy->x = eyePos.x;
        eye_copy->z = eyePos.z;
        eye_copy->y = eyePos.y;
        mLookAtMaxVel = 0;
        vel = 0;
    }
    gabi::call(0x0259DED0, &mJntCtrl, &current.angle.y, vec_p, eye_copy.get(), (s32)temp2, (s32)vel, (u32)temp); /* lookAtTarget */
}
VERIFY(0x022D83A8, &daNpc_Rsh1_c::lookBack);

/* 022D86EC */
void daNpc_Rsh1_c::setAttention() {
    WWHD_FUNC(0x022D86EC, void, this);
    f32 z = mAttnBasePos.z;
    f32 x = mAttnBasePos.x;
    f32 ofs = rsh1_HIO().mNpcHIO.mAttnYOffset;
    gabi::store<f32>(gabi::ea(this) + 0x390, x); /* attention_info.position */
    f32 y = mAttnBasePos.y;
    gabi::store<f32>(gabi::ea(this) + 0x398, z);
    f32 ey = current.pos.y;
    gabi::store<f32>(gabi::ea(this) + 0x394, y + ofs);
    f32 ex = current.pos.x;
    f32 ez = current.pos.z;
    eyePos.y = ey + 170.0f;
    eyePos.z = ez;
    eyePos.x = ex;
}
VERIFY(0x022D86EC, &daNpc_Rsh1_c::setAttention);

/* 022D8738 */
void daNpc_Rsh1_c::setCollision() {
    WWHD_FUNC(0x022D8738, void, this);
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
VERIFY(0x022D8738, &daNpc_Rsh1_c::setCollision);

/* 022D8854 */
BOOL daNpc_Rsh1_c::_execute() {
    WWHD_FUNC(0x022D8854, BOOL, this);
    /* HD: no debug HIO switches */
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    gabi::Local<cXyz> center;
    gabi::Local<cXyz> lookat_diff;
    center->x = gabi::load<f32>(cam + 0xE8); /* view.mLookat.mCenter */
    center->y = gabi::load<f32>(cam + 0xEC);
    center->z = gabi::load<f32>(cam + 0xF0);
    cXyz_mi(center.get(), lookat_diff.get(), &current.pos);
    f32 lookat_dist = std_sqrtf(PSVECSquareMag(lookat_diff.get()));
    if (lookat_dist > gabi::load<f32>(0x1047BBD8) /* REG10_F(10) */ + 5000.0f) {
        return TRUE;
    }
    dNpc_HIO_c_l& n = rsh1_HIO().mNpcHIO;
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
    gabi::store<s8>(gabi::ea(this) + 0x1C9, gabi::call<s8>(0x024EF130, dComIfG_Bgsp(), gabi::ea(this) + 0x520)); /* GetRoomId(mAcch.m_gnd) */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, gabi::call<u8>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(this) + 0x520)); /* GetPolyColor */
    set_mtx();
    setCollision();
    return TRUE;
}
VERIFY(0x022D8854, &daNpc_Rsh1_c::_execute);

/* 022D8B7C */
void daNpc_Rsh1_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x022D8B7C, void, this);
    switch (gabi::load<u8>(dComIfGp_ea() + 0x5BC5)) { /* dComIfGp_getMesgAnimeAttrInfo() */
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
    gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); /* dComIfGp_clearMesgAnimeAttrInfo() */
}
VERIFY(0x022D8B7C, &daNpc_Rsh1_c::setAnmFromMsgTag);

/* 022D8D44 (cXyz by value: passed by address) */
bool daNpc_Rsh1_c::chkAttention(cXyz* i_pos, s16 i_angleAdjustment) {
    WWHD_FUNC(0x022D8D44, bool, this, i_pos, i_angleAdjustment);
    fopAc_ac_c* link_p = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34));
    daNpc_Rsh1_HIO_c& hio = rsh1_HIO();
    f32 max_attn_dist_xz = hio.mNpcHIO.mMaxAttnDistXZ;
    int max_attn_angle_y = hio.mNpcHIO.mMaxAttnAngleY;
    gabi::Local<cXyz> pos_diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(&link_p->current.pos, pos_diff.get(), i_pos);
    f32 dx = pos_diff->x;
    f32 dz = pos_diff->z;
    xz->x = dx;
    xz->z = dz;
    xz->y = 0.0f;
    f32 temp_abs_xz = std_sqrtf(PSVECSquareMag(xz.get()));
    s16 angle = cM_atan2s(dx, dz);
    if (mbAttention) {
        max_attn_dist_xz = max_attn_dist_xz + 40.0f;
        max_attn_angle_y += 0x71C;
    }
    angle = (s16)(angle - i_angleAdjustment);
    int a = angle < 0 ? -angle : angle;
    return max_attn_angle_y > a && max_attn_dist_xz > temp_abs_xz;
}
VERIFY(0x022D8D44, &daNpc_Rsh1_c::chkAttention);

/* 022D8E84 */
BOOL daNpc_Rsh1_c::shopPosMove() {
    WWHD_FUNC(0x022D8E84, BOOL, this);
    s32 idx = mShopIdx;
    if (idx != -1) {
        f32 step = rsh1_HIO().m48;
        u32 src = 0x101C6138 + idx * 12; /* shop_buyer_pos */
        gabi::Local<cXyz> temp;
        temp->x = gabi::load<f32>(src);
        temp->y = gabi::load<f32>(src + 4);
        temp->z = gabi::load<f32>(src + 8);
        if (gabi::call<BOOL>(0x0200F764, &current.pos, temp.get(), step) != 0) { /* cLib_chasePosXZ */
            setAnm(0);
            return TRUE;
        }
        s16 target = cLib_targetAngleY(&current.pos, temp.get());
        gabi::call(0x0200F428, &current.angle.y, (s32)target, 8, 0x1000); /* cLib_addCalcAngleS2 */
        setAnm(5);
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x022D8E84, &daNpc_Rsh1_c::shopPosMove);
