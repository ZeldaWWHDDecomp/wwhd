/**
 * d_a_npc_bms1.cpp (WWHD)
 * NPC - Bomb-Master Cannon (Windfall bomb shop)
 *
 * The GameCube TU is "Nonmatching": the functions are
 * written from the WWHD code (cking.rpx) and verified against it, with the GameCube names; the
 * structure follows d_a_npc_bs1 (Beedle, decompiled) where the code is the same.
 * Part files: d_a_npc_bms1_heap.cpp (create/heap/matrices/execute/draw), d_a_npc_bms1_talk.cpp
 * (messages, actions, events); d_a_npc_bms1_pending.cpp holds weak guest-call stubs.
 */
#include "d/actor/d_a_npc_bms1.h"

static daNpc_Bms1_HIO_c l_HIOStorage;
static l_btp_ix_tblData l_btp_ix_tbl;
static morf_frame_tblData morf_frame_tbl;
static play_mode_tblData play_mode_tbl;
static play_speed_tblData play_speed_tbl;
static l_bck_ix_tblData l_bck_ix_tbl;
static u32 l_msgIdStorage;
static cXyz l_hairBase;
static cXyz l_hairTip;
static u32 l_hairBaseGuard;
static u32 l_hairTipGuard;
static bms1_cpp::QuaternionWords l_identityQuaternion;
static Bms1CutNames l_cutNames;
static dCcD_SrcCyl l_cyl_src;
static void* l_demoObject;

/* 02208334 */
static BOOL daNpc_Bms1_shopMsgCheck(u32 msgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (msgNo - 0x2788 < 9 || msgNo - 0x277A < 8 || msgNo == 0x2783 || msgNo == 0x2776) {
        return TRUE;
    }
    return FALSE;
}

/* 0220836C */
static BOOL daNpc_Bms1_shopStickMoveMsgCheck(u32 msgNo) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (msgNo - 0x2788 < 3 || msgNo - 0x277A < 3 || msgNo == 0x2783 || msgNo == 0x2776) {
        return TRUE;
    }
    return FALSE;
}

/* 02208BA8 */
BOOL daNpc_Bms1_c::initTexPatternAnm(u32 i_modify) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 idx = mTexIdx;
    J3DModelData* modelData = gabi::at<J3DModelData>(J3DModel_modelData_l(mpHeadModel));
    /* l_btp_ix_tbl (.rodata 0x10017CF0) */
    m_head_tex_pattern = (J3DAnmTexPattern*)bms1_getRes((s32)l_btp_ix_tbl.mEntries[idx]);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(593, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x251, "m_head_tex_pattern != (0)");
    if (!mDoExt_btpAnm_init_l(&mBtpAnm, modelData, m_head_tex_pattern, 1, 2 /* EMode_LOOP */, 1.0f, 0, -1,
                              i_modify, 0)) {
        return FALSE;
    }
    mFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 0220A160 */
void daNpc_Bms1_c::playTexPatternAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax = J3DAnmTexPattern_getFrameMax_l(m_head_tex_pattern);
        if ((s32)mFrame >= frameMax) {
            s32 frameMax2 = J3DAnmTexPattern_getFrameMax_l(m_head_tex_pattern);
            mFrame = (u8)(mFrame - frameMax2);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mFrame = (u8)(mFrame + 1);
        }
    }
}

/* 0220AB80 */
void daNpc_Bms1_c::setAnm(s8 i_idx, f32 i_morf) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 cur = mAnmIdx;
    if (i_morf < 0.0f) {
        i_morf = (f32)morf_frame_tbl.mEntries[i_idx]; /* morf_frame_tbl */
    }
    if (i_idx != cur || cur == -1) {
        mAnmIdx = i_idx;
        dNpc_setAnm_2(mpMorf, (s32)play_mode_tbl.mEntries[i_idx] /* play_mode_tbl */, i_morf,
                      (f32)play_speed_tbl.mEntries[i_idx] /* play_speed_tbl */,
                      (s32)l_bck_ix_tbl.mEntries[i_idx] /* l_bck_ix_tbl */, 0, BMS1_ARC);
    }
}

/* 02208CB4 */
u32 daNpc_Bms1_c::setTexAnm(s8 i_idx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 cur = mTexIdx;
    if (cur != i_idx || cur == -1) {
        mTexIdx = i_idx;
        return initTexPatternAnm(1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 0220A680 */
void daNpc_Bms1_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 order = mOrderEvt;
    if (order == BMS1_ORDER_LANDING) {
        fopAcM_orderOtherEvent2_l(this, "BMS_LAND_DEMO", 1, 0xFFFF);
    } else if (order == BMS1_ORDER_GET_ITEM) {
        fopAcM_orderOtherEvent2_l(this, "BMS_GET_DEMO", 1, 0xFFFF);
    } else if (order == BMS1_ORDER_TALK || order == BMS1_ORDER_SHOP) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderEvt == BMS1_ORDER_TALK) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}

/* 0220A6F0 */
void daNpc_Bms1_c::setCollision() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = -16.0f;
    MtxTrans(current.pos.x, current.pos.y, current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), mInitialAngle.y);
    gabi::Local<cXyz> out;
    MtxPosition(offset.get(), out.get());
    mCyl.SetC(out.get());
    mCyl.SetR(46.0f);
    mCyl.SetH(130.0f);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 0220A360 */
void daNpc_Bms1_c::talkInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mTalkStep = 0;
}

/* 0220A230 */
BOOL daNpc_Bms1_c::demo_move() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 demoId = demoActorID;
    if (demoId != 0 && demoId <= 0x20) {
        u32 demoObj = l_demoObject.v.get(); /* dComIfGp_demo_get() */
        if (demoObj == 0) {
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            demoObj = l_demoObject.v.get();
        }
        void* demoActor = dDemo_object_getActor_l(demoObj, demoId);
        if (demoActor != nullptr) {
            mbDemo = 1;
            J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData_l(demoActor, "Bms" /* "Bms" */);
            if (btp != nullptr) {
                J3DModelData* modelData = gabi::at<J3DModelData>(J3DModel_modelData_l(mpHeadModel));
                mDoExt_btpAnm_init_l(&mBtpAnm, modelData, btp, 1, 2, 1.0f, 0, -1, 1, 0);
            }
            dDemo_setDemoData_l(this, 0x6A, mpMorf, "Bms", 0, nullptr, 0, 0);
            return TRUE;
        }
    }
    if (mbDemo == 1) {
        demo_end_init();
    }
    return FALSE;
}

/* 0220A224 */
void daNpc_Bms1_c::demo_end_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mbDemo = 0;
}

/* 0220A0CC */
BOOL daNpc_Bms1_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (mbCreateErr != 1) {
        dComIfG_resDelete(&mPhs, BMS1_ARC);
        if (heap.get() != nullptr && mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        daNpc_Bms1_HIO_c& hio = bms1HIO();
        if (hio.m8 >= 0) {
            s32 n = hio.m8 - 1;
            hio.m8 = n;
            if (n < 0) {
                mDoHIO_deleteChild(hio.mNo);
            }
        }
    }
    return TRUE;
}

/* 02209434 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bms1_c*)i_this)->CreateHeap();
}

/* 0220A0C8 */
static cPhs_State daNpc_Bms1_Create(fopAc_ac_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ((daNpc_Bms1_c*)i_this)->_create();
}

/* 0220A15C */
static BOOL daNpc_Bms1_Delete(daNpc_Bms1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_delete();
}

/* 0220A950 */
static BOOL daNpc_Bms1_Execute(daNpc_Bms1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_execute();
}

/* 0220AB74 */
static BOOL daNpc_Bms1_Draw(daNpc_Bms1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return i_this->_draw();
}

/* 0220AB78 */
static BOOL daNpc_Bms1_IsDelete(daNpc_Bms1_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 0220C994 */
static void* daNpc_Bms1_childHIO_c_ct(void* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = operator_new(0x54);
        if (i_this == nullptr)
            return i_this;
    }
    bms1_cpp::ChildHIO* child = static_cast<bms1_cpp::ChildHIO*>(i_this);
    child->__vtbl = 0x10017D64;
    bms1_HIOConstruct(&child->mNpc); /* dNpc_HIO_c::dNpc_HIO_c */
    for (u32 i = 0; i < 9; ++i)
        child->mSettings[i] = 0.0f;
    return i_this;
}

/* 0220CA14 */
static daNpc_Bms1_HIO_c* daNpc_Bms1_HIO_c_ct(daNpc_Bms1_HIO_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = (daNpc_Bms1_HIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017D74;
    bms1_constructChildren(&i_this->child_unk, 1, 0x54,
                           0x0220C994); /* __construct_array(mChild, 1, size, ct) */
    i_this->mMaxHeadX = 0x1388;
    i_this->mMaxBackboneX = 0;
    i_this->mMaxHeadY = 0x2710;
    i_this->mMaxBackboneY = 0x834;
    i_this->mMinHeadX = -0x1FFE;
    i_this->mMinBackboneX = 0;
    i_this->mMinHeadY = 0;
    i_this->mMinBackboneY = 0;
    i_this->mMaxTurnStep = 0x1000;
    i_this->mMaxHeadTurnVel = 0x800;
    i_this->m04 = -50.0f;
    i_this->mAttnYOffset = 40.0f;
    i_this->m22 = 0;
    i_this->mMaxAttnDistXZ = 300.0f;
    i_this->mCursorScale[0] = 0.65f;
    i_this->mCursorScale[1] = 0.9f;
    i_this->mCursorScale[2] = 0.5f;
    i_this->mCursorScale[3] = 27.0f;
    i_this->mCursorScale[4] = 20.0f;
    i_this->mHairSpring = 0.45f;
    i_this->mHairDamp = 0.8f;
    i_this->mHairSlerp = 0.9f;
    i_this->mHairStretch = 5e-05f;
    i_this->m8 = -1;
    i_this->mNo = -1;
    return i_this;
}

/* 0220CB54 */
static void __sinit_d_a_npc_bms1_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(0x1046661C, 0x101BD37C);
    daNpc_Bms1_HIO_c_ct(&l_HIOStorage); // HD: construct the single TU-wide HIO storage.
}

/* 0220CBF4 */
static void trivial_dt(void* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0220CC08 */
static void daNpc_Bms1_c_dt(daNpc_Bms1_c* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr) {
        bms1_STControlDestroy(&i_this->mStickControl, 2); /* STControl::~STControl */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        bms1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
        reinterpret_cast<bms1_cpp::AcchVtables*>(&i_this->mAcch)->mActorVtable = 0x10017D34;
        reinterpret_cast<bms1_cpp::AcchVtables*>(&i_this->mAcch)->mCheckVtable = 0x10017D44;
        bms1_acchDestroy(&i_this->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        bms1_actorDestroy(i_this, 0);        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0220CCB0 */
static void empty_virtual(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 0220CCB4 */
static s8 cLib_calcTimer_s8(be<s8>* t) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 v = *t;
    if (v != 0) {
        v = (s8)(v - 1);
        *t = v;
    }
    return v;
}

// clang-format off
/* WWHD glue
str 101BD3A0 "Bms"
str 10017F0C "BMS_GET_DEMO" in 0220A680
str 10017EFC "BMS_LAND_DEMO" in 0220A680
str 10017EF0 "Bms" in 0220A230
str 10017D84 "m_object != (0)" in 0220A230
str 10017DB0 "d_demo.h" in 0220A230
str 10017DD8 "m_head_tex_pattern != (0)" in 02208BA8
str 10017DF4 "d_a_npc_bms1.cpp" in 02208BA8
data l_HIOStorage 10466650
data l_msgIdStorage 10466618
data l_hairBase 10466638
data l_hairTip 10466644
data l_hairBaseGuard 104666B0
data l_hairTipGuard 104666B4
data l_identityQuaternion 101E9C38
data l_cutNames 101BD36C
data l_cyl_src 101BD2D4
data l_demoObject 101D5FFC
data l_btp_ix_tbl 10017CF0
data morf_frame_tbl 101BD334
data play_mode_tbl 101BD318
data play_speed_tbl 101BD350
data l_bck_ix_tbl 10017D94
*/
// clang-format on
