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

/* 02208334 */
static BOOL daNpc_Bms1_shopMsgCheck(u32 msgNo) {
    WWHD_FUNC(0x02208334, BOOL, msgNo);
    if (msgNo - 0x2788 < 9 || msgNo - 0x277A < 8 || msgNo == 0x2783 || msgNo == 0x2776) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02208334, daNpc_Bms1_shopMsgCheck);

/* 0220836C */
static BOOL daNpc_Bms1_shopStickMoveMsgCheck(u32 msgNo) {
    WWHD_FUNC(0x0220836C, BOOL, msgNo);
    if (msgNo - 0x2788 < 3 || msgNo - 0x277A < 3 || msgNo == 0x2783 || msgNo == 0x2776) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0220836C, daNpc_Bms1_shopStickMoveMsgCheck);

/* 02208BA8 */
BOOL daNpc_Bms1_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x02208BA8, BOOL, this, i_modify);
    s8 idx = mTexIdx;
    J3DModelData* modelData = gabi::at<J3DModelData>(J3DModel_modelData_l(mpHeadModel));
    /* l_btp_ix_tbl (.rodata 0x10017CF0) */
    m_head_tex_pattern = (J3DAnmTexPattern*)bms1_getRes(gabi::load<s32>(0x10017CF0 + idx * 4));
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(593, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x10017DF4), 0x251, STR(0x10017DD8));
    if (!mDoExt_btpAnm_init_l(mBtpAnm, modelData, m_head_tex_pattern, 1, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, 0)) {
        return FALSE;
    }
    mFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x02208BA8, &daNpc_Bms1_c::initTexPatternAnm);

/* 02208CB4 */
u32 daNpc_Bms1_c::setTexAnm(s8 i_idx) {
    WWHD_FUNC(0x02208CB4, u32, this, i_idx);
    s8 cur = mTexIdx;
    if (cur != i_idx || cur == -1) {
        mTexIdx = i_idx;
        return initTexPatternAnm(1);
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x02208CB4, &daNpc_Bms1_c::setTexAnm);

/* 02209434: CheckCreateHeap */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02209434, BOOL, i_this);
    return ((daNpc_Bms1_c*)i_this)->CreateHeap();
}
VERIFY(0x02209434, CheckCreateHeap);

/* 0220A0C8 */
static cPhs_State daNpc_Bms1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0220A0C8, cPhs_State, i_this);
    return ((daNpc_Bms1_c*)i_this)->_create();
}
VERIFY(0x0220A0C8, daNpc_Bms1_Create);

/* 0220A0CC */
BOOL daNpc_Bms1_c::_delete() {
    WWHD_FUNC(0x0220A0CC, BOOL, this);
    if (mbCreateErr != 1) {
        dComIfG_resDelete(&mPhs, BMS1_ARC);
        if (heap.get() != nullptr && mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        daNpc_Bms1_HIO_c& hio = l_HIO();
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
VERIFY(0x0220A0CC, &daNpc_Bms1_c::_delete);

/* 0220A15C */
static BOOL daNpc_Bms1_Delete(daNpc_Bms1_c* i_this) {
    WWHD_FUNC(0x0220A15C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0220A15C, daNpc_Bms1_Delete);

/* 0220A160 */
void daNpc_Bms1_c::playTexPatternAnm() {
    WWHD_FUNC(0x0220A160, void, this);
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
VERIFY(0x0220A160, &daNpc_Bms1_c::playTexPatternAnm);

/* 0220A224 (GameCube demo_end_init) */
void daNpc_Bms1_c::demo_end_init() {
    WWHD_FUNC(0x0220A224, void, this);
    mbDemo = 0;
}
VERIFY(0x0220A224, &daNpc_Bms1_c::demo_end_init);

/* 0220A230 */
BOOL daNpc_Bms1_c::demo_move() {
    WWHD_FUNC(0x0220A230, BOOL, this);
    u8 demoId = demoActorID;
    if (demoId != 0 && demoId <= 0x20) {
        u32 demoObj = gabi::load<u32>(0x101D5FFC); /* dComIfGp_demo_get() */
        if (demoObj == 0) {
            JUT_ASSERT_fail(STR(0x10017DB0), 0x23A, STR(0x10017D84));
            demoObj = gabi::load<u32>(0x101D5FFC);
        }
        void* demoActor = dDemo_object_getActor_l(demoObj, demoId);
        if (demoActor != nullptr) {
            mbDemo = 1;
            J3DAnmTexPattern* btp = dDemo_actor_getP_BtpData_l(demoActor, STR(0x10017EF0) /* "Bms" */);
            if (btp != nullptr) {
                J3DModelData* modelData = gabi::at<J3DModelData>(J3DModel_modelData_l(mpHeadModel));
                mDoExt_btpAnm_init_l(mBtpAnm, modelData, btp, 1, 2, 1.0f, 0, -1, 1, 0);
            }
            dDemo_setDemoData_l(this, 0x6A, mpMorf, STR(0x10017EF0), 0, nullptr, 0, 0);
            return TRUE;
        }
    }
    if (mbDemo == 1) {
        demo_end_init();
    }
    return FALSE;
}
VERIFY(0x0220A230, &daNpc_Bms1_c::demo_move);

/* 0220A360 (GameCube talkInit) */
void daNpc_Bms1_c::talkInit() {
    WWHD_FUNC(0x0220A360, void, this);
    mTalkStep = 0;
}
VERIFY(0x0220A360, &daNpc_Bms1_c::talkInit);

/* 0220A680 */
void daNpc_Bms1_c::eventOrder() {
    WWHD_FUNC(0x0220A680, void, this);
    s8 order = mOrderEvt;
    if (order == 4) {
        fopAcM_orderOtherEvent2_l(this, STR(0x10017EFC), 1, 0xFFFF);
    } else if (order == 3) {
        fopAcM_orderOtherEvent2_l(this, STR(0x10017F0C), 1, 0xFFFF);
    } else if (order == 1 || order == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrderEvt == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x0220A680, &daNpc_Bms1_c::eventOrder);

/* 0220A6F0 */
void daNpc_Bms1_c::setCollision() {
    WWHD_FUNC(0x0220A6F0, void, this);
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = -16.0f;
    MtxTrans(current.pos.x, current.pos.y, current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), m936.y);
    gabi::Local<cXyz> out;
    MtxPosition(offset.get(), out.get());
    mCyl.SetC(out.get());
    mCyl.SetR(46.0f);
    mCyl.SetH(130.0f);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x0220A6F0, &daNpc_Bms1_c::setCollision);

/* 0220A950 */
static BOOL daNpc_Bms1_Execute(daNpc_Bms1_c* i_this) {
    WWHD_FUNC(0x0220A950, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0220A950, daNpc_Bms1_Execute);

/* 0220AB74 */
static BOOL daNpc_Bms1_Draw(daNpc_Bms1_c* i_this) {
    WWHD_FUNC(0x0220AB74, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0220AB74, daNpc_Bms1_Draw);

/* 0220AB78 */
static BOOL daNpc_Bms1_IsDelete(daNpc_Bms1_c*) {
    WWHD_FUNC(0x0220AB78, BOOL, (daNpc_Bms1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0220AB78, daNpc_Bms1_IsDelete);

/* 0220AB80 */
void daNpc_Bms1_c::setAnm(s8 i_idx, f32 i_morf) {
    WWHD_FUNC(0x0220AB80, void, this, i_idx, i_morf);
    s8 cur = mAnmIdx;
    if (i_morf < 0.0f) {
        i_morf = gabi::load<f32>(0x101BD334 + i_idx * 4); /* morf_frame_tbl */
    }
    if (i_idx != cur || cur == -1) {
        mAnmIdx = i_idx;
        dNpc_setAnm_2(mpMorf, gabi::load<s32>(0x101BD318 + i_idx * 4) /* play_mode_tbl */, i_morf,
                      gabi::load<f32>(0x101BD350 + i_idx * 4) /* play_speed_tbl */,
                      gabi::load<s32>(0x10017D94 + i_idx * 4) /* l_bck_ix_tbl */, 0, BMS1_ARC);
    }
}
VERIFY(0x0220AB80, &daNpc_Bms1_c::setAnm);

/* 0220C994: daNpc_Bms1_childHIO_c::daNpc_Bms1_childHIO_c (HD: allocates when this == NULL) */
static void* daNpc_Bms1_childHIO_c_ct(void* i_this) {
    WWHD_FUNC(0x0220C994, void*, i_this);
    if (i_this == nullptr) {
        i_this = operator_new(0x54);
        if (i_this == nullptr)
            return i_this;
    }
    u32 p = gabi::ea(i_this);
    gabi::store<u32>(p + 0x50, 0x10017D64);
    gabi::call(0x0259DA18, p + 4); /* dNpc_HIO_c::dNpc_HIO_c */
    for (u32 o = 0x2C; o <= 0x4C; o += 4) gabi::store<f32>(p + o, 0.0f);
    return i_this;
}
VERIFY(0x0220C994, daNpc_Bms1_childHIO_c_ct);

/* 0220CA14: daNpc_Bms1_HIO_c::daNpc_Bms1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Bms1_HIO_c* daNpc_Bms1_HIO_c_ct(daNpc_Bms1_HIO_c* i_this) {
    WWHD_FUNC(0x0220CA14, daNpc_Bms1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Bms1_HIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017D74;
    gabi::call(0x028EFFD0, gabi::ea(i_this) + 8, 1, 0x54, 0x0220C994); /* __construct_array(mChild, 1, size, ct) */
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
VERIFY(0x0220CA14, daNpc_Bms1_HIO_c_ct);

/* 0220CB54: static initialisation of the translation unit */
static void __sinit_d_a_npc_bms1_cpp() {
    WWHD_FUNC(0x0220CB54, void);
    sinit_header_statics(0x1046661C, 0x101BD37C);
    daNpc_Bms1_HIO_c_ct(&l_HIO()); /* static daNpc_Bms1_HIO_c l_HIO */
}
VERIFY(0x0220CB54, __sinit_d_a_npc_bms1_cpp);

/* 0220CBF4: trivial deleting destructor (this TU's copy: sead::SafeString / HIO) */
static void trivial_dt(void* i_this, s32 flags) {
    WWHD_FUNC(0x0220CBF4, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0220CBF4, trivial_dt);

/* 0220CC08: daNpc_Bms1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Bms1_c_dt(daNpc_Bms1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0220CC08, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025886D8, &i_this->mStickControl, 2); /* STControl::~STControl */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x20, 0x10017D34);
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x14, 0x10017D44);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0220CC08, daNpc_Bms1_c_dt);

/* 0220CCB0: empty virtual */
static void empty_virtual(void*) {
    WWHD_FUNC(0x0220CCB0, void, (void*)nullptr);
}
VERIFY(0x0220CCB0, empty_virtual);

/* 0220CCB4: cLib_calcTimer<s8> (this TU's copy) */
static s8 cLib_calcTimer_s8(be<s8>* t) {
    WWHD_FUNC(0x0220CCB4, s8, t);
    s8 v = *t;
    if (v != 0) {
        v = (s8)(v - 1);
        *t = v;
    }
    return v;
}
VERIFY(0x0220CCB4, cLib_calcTimer_s8);
