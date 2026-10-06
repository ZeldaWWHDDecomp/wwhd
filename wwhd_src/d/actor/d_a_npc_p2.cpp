/**
 * d_a_npc_p2.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_p2.cpp) has only "Nonmatching" stubs for this actor: every function here was
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Heap, create, draw, small helpers: this file; cuts: d_a_npc_p2_cut.cpp; actions, talk:
 * d_a_npc_p2_act.cpp; _execute, createInit, setAnm, lookBack: d_a_npc_p2_exec.cpp.
 */
#include "d/actor/d_a_npc_p2.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry (stack)) */
static inline s32 p2_btpInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end,
                             u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 p2_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 p2_calcTimerS(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }

/* 022B5AEC */
static BOOL nodeCallBack(J3DNode* i_node, int i_param_2) {
    WWHD_FUNC(0x022B5AEC, BOOL, i_node, i_param_2);
    if (i_param_2 == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_P2_c* i_this = gabi::at<daNpc_P2_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea() */
        if (i_this != nullptr) {
            J3DJoint* joint = J3DNode_toJoint(i_node);
            u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
            Mtx34* stack = mDoMtx_stack_c::get();
            PSMTXCopy(gabi::at<Mtx34>(p2_getAnmMtx(model, jntNo)), stack);
            if (jntNo == 4) { /* head */
                /* function-local statics: static cXyz l_offset(0, 0, 0), l_eyeOffset(20, 10, 0) */
                if (gabi::load<u32>(0x104682E8) == 0) {
                    gabi::store<f32>(0x10468000, 0.0f);
                    gabi::store<f32>(0x10468008, 0.0f);
                    gabi::store<u32>(0x104682E8, 1);
                    gabi::store<f32>(0x10468004, 0.0f);
                }
                if (gabi::load<u32>(0x104682EC) == 0) {
                    gabi::store<u32>(0x104682EC, 1);
                    gabi::store<f32>(0x1046800C, 20.0f);
                    gabi::store<f32>(0x10468014, 0.0f);
                    gabi::store<f32>(0x10468010, 10.0f);
                }
                PSMTXMultVec(stack, gabi::at<cXyz>(0x10468000), &i_this->mHeadPos);
                mDoMtx_XrotM(stack, i_this->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stack, (s16)-i_this->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stack, gabi::at<cXyz>(0x1046800C), &i_this->mEyePos);
                if (i_this->mHeadCalcCount != 0xFF) {
                    i_this->mHeadCalcCount = (u8)(i_this->mHeadCalcCount + 1);
                }
            } else if (jntNo == 2) { /* backbone */
                mDoMtx_XrotM(stack, i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stack, i_this->m_jnt.mAngles[1][0]);
                mDoMtx_ZrotM(stack, REG_S(12, 0));
                mDoMtx_XrotM(stack, REG_S(12, 1));
                mDoMtx_YrotM(stack, REG_S(12, 2));
            }
            PSMTXCopy(stack, gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
            u32 anm = p2_getAnmMtx(model, jntNo);         /* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
            p2_mtx_copy(anm, gabi::ea(stack));
        }
    }
    return TRUE;
}
VERIFY(0x022B5AEC, nodeCallBack);

/* 022B5D8C */
BOOL daNpc_P2_c::initTexPatternAnm(u32 i_modify) { /* GameCube: bool; GHS forwards the register as is */
    WWHD_FUNC(0x022B5D8C, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData(mpHeadModel);
    /* l_btp_idx (0x1001FAF0) */
    s32 idx = gabi::load<s32>(0x1001FAF0 + 4 * (s32)(s8)mBtpNum);
    J3DAnmTexPattern* head_tex_pattern = (J3DAnmTexPattern*)p2_getRes(idx);
    if (head_tex_pattern == nullptr) {
        JUT_ASSERT_fail(STR(0x1001FBC0), 0x196, STR(0x1001FBA8) /* "head_tex_pattern != 0" */);
    }
    if (!p2_btpInit(mBtp, modelData, head_tex_pattern, 1, 2, 1.0f, 0, -1, (u32)i_modify, 0)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022B5D8C, &daNpc_P2_c::initTexPatternAnm);

/* 022B5EA0 */
BOOL daNpc_P2_c::_createHeap() {
    WWHD_FUNC(0x022B5EA0, BOOL, this);
    /* l_bmd_idx (0x1001FB8C), l_head_bmd_idx (0x1001FBD4), l_btp_num (0x1001FBD0) */
    J3DModelData* modelData = (J3DModelData*)p2_getRes(gabi::load<s32>(0x1001FB8C + 4 * (u32)mType));
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001FBFC), 0xA20, STR(0x1001FC0C) /* "modelData != 0" */);
    }
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1, 1,
                                                  nullptr, 0x80000, 0x11020203);
    if (morf == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    mpMorf = morf;
    if (morf->getModel() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    J3DModelData* headModelData = (J3DModelData*)p2_getRes(gabi::load<s32>(0x1001FBD4 + 4 * (u32)mType));
    if (headModelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001FBFC), 0xA3C, STR(0x1001FC20) /* "headModelData != 0" */);
    }
    if (mType != TYPE_P2C_e) {
        J3DModel* head = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
        mpHeadModel = head;
        if (head == nullptr) return FALSE;
        mBtpNum = (s8)gabi::load<u8>(0x1001FBD0 + (u32)mType);
        if (!initTexPatternAnm(false)) return FALSE;
    } else {
        J3DModel* head = mDoExt_J3DModel__create(headModelData, 0, 0x11020203);
        mpHeadModel = head;
        if (head == nullptr) return FALSE;
    }
    J3DModelData* daggerModelData = (J3DModelData*)p2_getRes(0x2A);
    if (daggerModelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001FBFC), 0xA59, STR(0x1001FC38) /* "daggerModelData != 0" */);
    }
    J3DModel* dagger = mDoExt_J3DModel__create(daggerModelData, 0, 0x11020203);
    mpDaggerModel = dagger;
    if (dagger == nullptr) return FALSE;
    J3DModelData* dagger2ModelData = (J3DModelData*)p2_getRes(0x27);
    /* HD: the assertion tests the first dagger's model data again */
    if (daggerModelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1001FBFC), 0xA60, STR(0x1001FC38) /* "daggerModelData != 0" */);
    }
    J3DModel* dagger2 = mDoExt_J3DModel__create(dagger2ModelData, 0, 0x11020203);
    mpDagger2Model = dagger2;
    if (dagger2 == nullptr) return FALSE;
    if (mType == TYPE_P2C_e) {
        J3DModelData* bookModelData = (J3DModelData*)p2_getRes(0x29);
        if (bookModelData == nullptr) {
            JUT_ASSERT_fail(STR(0x1001FBFC), 0xA68, STR(0x1001FC50) /* "bookModelData != 0" */);
        }
        J3DAnmTransform* bck = (J3DAnmTransform*)p2_getRes(0x23);
        mDoExt_McaMorf* book = mDoExt_McaMorf::create(nullptr, bookModelData, nullptr, nullptr, bck, 0, 1.0f, 0, -1, 1,
                                                      nullptr, 0, 0x11020203);
        mpBookMorf = book;
        if (book == nullptr || book->getModel() == nullptr) return FALSE;
    }
    if (mType == TYPE_P2A_e) {
        J3DModelData* telescopeModelData = (J3DModelData*)p2_getRes(0x28);
        if (telescopeModelData == nullptr) {
            JUT_ASSERT_fail(STR(0x1001FBFC), 0xA7B, STR(0x1001FBE0) /* "telescopeModelData != 0" */);
        }
        J3DModel* telescope = mDoExt_J3DModel__create(telescopeModelData, 0, 0x11020203);
        mpTelescopeModel = telescope;
        if (telescope == nullptr) return FALSE;
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    for (u16 i = 0; i < p2_getJointNum(modelData); i++) {
        if (i == 4 || i == 2) {
            /* getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack): HD joint nodes are
             * 0x1C-byte records at *(data + 8), the count at data + 4 (inline bound check) */
            u32 data = gabi::ea(J3DModel_getModelData(mpMorf->getModel()));
            u32 n = gabi::load<u32>(data + 4);
            u32 p = gabi::load<u32>(data + 8);
            if (i < n) p += i * 0x1C;
            gabi::store<u32>(p + 8, 0x022B5AEC);
        }
    }
    return TRUE;
}
VERIFY(0x022B5EA0, &daNpc_P2_c::_createHeap);

/* 022B62D8 (GameCube CreateHeap_CB; unnamed by the matcher) */
static BOOL CreateHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022B62D8, BOOL, i_this);
    return ((daNpc_P2_c*)i_this)->_createHeap();
}
VERIFY(0x022B62D8, CreateHeap_CB);

/* 022B62DC */
void daNpc_P2_c::getArg() {
    WWHD_FUNC(0x022B62DC, void, this);
    u32 param = mParameters;
    mSwitchNo = (u8)(param >> 10);
    u8 sub = (u8)(param >> 2);
    mSubType = sub;
    u8 type = (u8)(param & 3);
    mType = type;
    if (type == 3) mType = 0;
    if (sub == 0xFF) mSubType = 0;
}
VERIFY(0x022B62DC, &daNpc_P2_c::getArg);

/* 022B6318 */
u32 daNpc_P2_c::setTexAnm() {
    WWHD_FUNC(0x022B6318, u32, this);
    /* l_btp_tbl (0x101C2C78): per type 0x17 btp numbers by texture animation */
    u8 type = mType;
    s32 btp = (s8)gabi::load<u8>(0x101C2C78 + type * 0x17 + (s32)(s8)mTexAnm);
    if ((u32)btp == (u32)(s32)(s8)mBtpNum) return (u32)gabi::ea(this); /* original: r3 still this */
    if (btp == -1) return (u32)gabi::ea(this); /* original: r3 still this */
    if (type == TYPE_P2C_e) return (u32)gabi::ea(this); /* original: r3 still this */
    mBtpNum = (s8)btp;
    return initTexPatternAnm(true);
}
VERIFY(0x022B6318, &daNpc_P2_c::setTexAnm);

/* 022B76BC */
cPhs_State daNpc_P2_c::_create() {
    WWHD_FUNC(0x022B76BC, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_P2_c): the inline constructor (HD virtual destructor) */
    u32 cond = actor_condition;
    if (!(cond & 8)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            gabi::store<u32>(gabi::ea(this) + 0xB4, P2_VTBL);
            gabi::call(0x025E7820, mBtp);       /* mDoExt_btpAnm */
            gabi::call(0x0259DAA0, &m_jnt);     /* dNpc_JntCtrl_c */
            gabi::call(0x0259F740, &mEventCut); /* dNpc_EventCut_c */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x1001FB2C, 0x1001FB4C, 0x1001FB3C});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, P2_AAB_VTBL);
            gabi::call(0x025A5B18, mSmokeCB, 1); /* dPa_smokeEcallBack(1) */
            cond = actor_condition;
        }
        actor_condition = cond | 8;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(P2_ARC));
    if (state == cPhs_COMPLEATE_e) {
        getArg();
        /* l_heap_size (0x100200A4) */
        if (!fopAcM_entrySolidHeap(this, 0x022B62D8 /* CreateHeap_CB */, gabi::load<u32>(0x100200A4 + 4 * (u32)mType))) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return state;
}
VERIFY(0x022B76BC, &daNpc_P2_c::_create);

/* 022B7864 */
static cPhs_State daNpc_P2Create(void* i_this) {
    WWHD_FUNC(0x022B7864, cPhs_State, i_this);
    return ((daNpc_P2_c*)i_this)->_create();
}
VERIFY(0x022B7864, daNpc_P2Create);

/* 022B7868 */
BOOL daNpc_P2_c::_delete() {
    WWHD_FUNC(0x022B7868, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(P2_ARC));
    /* mSmokeCB.remove() (virtual, vtable +0x44) */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(mSmokeCB)) + 0x44), mSmokeCB);
    mDoAud_seDeleteObject(&mRopePos);
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022B7868, &daNpc_P2_c::_delete);

/* 022B78D4 */
static BOOL daNpc_P2Delete(void* i_this) {
    WWHD_FUNC(0x022B78D4, BOOL, i_this);
    return ((daNpc_P2_c*)i_this)->_delete();
}
VERIFY(0x022B78D4, daNpc_P2Delete);

/* 022B78D8 */
void daNpc_P2_c::playTexPatternAnm() {
    WWHD_FUNC(0x022B78D8, void, this);
    if (!p2_calcTimerS(&mBlinkTimer)) {
        u32 tex = gabi::load<u32>(gabi::ea(this) + 0x3D0); /* mBtp.getBtpAnm() */
        s32 max = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(tex + 4) + 0x14), tex); /* getFrameMax() */
        if ((s32)(u32)mBlinkFrame >= max) {
            tex = gabi::load<u32>(gabi::ea(this) + 0x3D0);
            max = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(tex + 4) + 0x14), tex);
            mBlinkFrame = (u8)(mBlinkFrame - max);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = (u8)(mBlinkFrame + 1);
        }
    }
}
VERIFY(0x022B78D8, &daNpc_P2_c::playTexPatternAnm);

/* 022B799C */
void daNpc_P2_c::talkInit() {
    WWHD_FUNC(0x022B799C, void, this);
    mTalkState = 0;
}
VERIFY(0x022B799C, &daNpc_P2_c::talkInit);

/* 022B79A8 */
void daNpc_P2_c::checkOrder() {
    WWHD_FUNC(0x022B79A8, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        mOrder = 0;
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrder == 1 || mOrder == 2) {
            mOrder = 0;
            mbTalk = 1;
            talkInit();
        }
    }
}
VERIFY(0x022B79A8, &daNpc_P2_c::checkOrder);

/* 022BB044 */
void daNpc_P2_c::eventOrder() {
    WWHD_FUNC(0x022BB044, void, this);
    s8 order = mOrder;
    if (order == 1 || order == 2) {
        u32 p = gabi::ea(this) + 0xFA; /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | 1));
        if (mOrder == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        /* l_evn_tbl (0x101C2E4C) */
        gabi::call(0x025D77DC, this, gabi::load<u32>(0x101C2E4C + 4 * order), 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
    }
}
VERIFY(0x022BB044, &daNpc_P2_c::eventOrder);

/* 022BB0A4 */
void daNpc_P2_c::setMtx() {
    WWHD_FUNC(0x022BB0A4, void, this);
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    p2_mtx_copy(gabi::ea(model) + 0xC8, gabi::ea(mDoMtx_stack_c::get()));
}
VERIFY(0x022BB0A4, &daNpc_P2_c::setMtx);

/* 022BB174 */
void daNpc_P2_c::setCollision() {
    WWHD_FUNC(0x022BB174, void, this);
    u8 big = mbBigCyl;
    mCyl.SetC(&current.pos);
    if (big) {
        mCyl.SetR(90.0f);
        mCyl.SetH(120.0f);
    } else {
        mCyl.SetR(30.0f);
        mCyl.SetH(120.0f);
    }
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x022BB174, &daNpc_P2_c::setCollision);

/* 022BB650 */
static BOOL daNpc_P2Execute(void* i_this) {
    WWHD_FUNC(0x022BB650, BOOL, i_this);
    return ((daNpc_P2_c*)i_this)->_execute();
}
VERIFY(0x022BB650, daNpc_P2Execute);

/* 022BB654 */
void daNpc_P2_c::draw_item(J3DModel* i_model, s8 i_jnt) {
    WWHD_FUNC(0x022BB654, void, this, i_model, i_jnt);
    setLightTevColorType(dKy_getEnvlight(), i_model, &tevStr);
    u32 anm = p2_getAnmMtx(mpMorf->getModel(), i_jnt);
    PSMTXCopy(gabi::at<Mtx34>(anm), mDoMtx_stack_c::get());
    p2_mtx_copy(gabi::ea(i_model) + 0xC8, gabi::ea(mDoMtx_stack_c::get())); /* setBaseTRMtx */
    mDoExt_modelUpdateDL(i_model, 0);
}
VERIFY(0x022BB654, &daNpc_P2_c::draw_item);

/* 022BB744 */
void daNpc_P2_c::drawDagger() {
    WWHD_FUNC(0x022BB744, void, this);
    if (mbDagger) {
        draw_item(mpDaggerModel, 0xC);
    } else {
        draw_item(mpDagger2Model, 0xE);
    }
}
VERIFY(0x022BB744, &daNpc_P2_c::drawDagger);

/* 022BB768 */
void daNpc_P2_c::drawHead() {
    WWHD_FUNC(0x022BB768, void, this);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    J3DModel* head = mpHeadModel;
    if (mType != TYPE_P2C_e) {
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)mBtp, J3DModel_getModelData(head), (s16)(u16)mBlinkFrame);
        u32 anm = p2_getAnmMtx(mpMorf->getModel(), 4);
        p2_mtx_copy(gabi::ea(mpHeadModel.get()) + 0xC8, anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
        gabi::store<u32>(gabi::ea(J3DModel_getModelData(mpHeadModel)) + 0x38, 0); /* HD: clear the texture pattern entry */
    } else {
        u32 anm = p2_getAnmMtx(mpMorf->getModel(), 4);
        p2_mtx_copy(gabi::ea(head) + 0xC8, anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
    }
}
VERIFY(0x022BB768, &daNpc_P2_c::drawHead);

/* 022BB8F8 */
void daNpc_P2_c::drawP2a() {
    WWHD_FUNC(0x022BB8F8, void, this);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    if (!p2_isEventBit(0x808)) {
        draw_item(mpTelescopeModel, 0xC);
    }
    gabi::call(0x025BEBB8, 0x76, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}
VERIFY(0x022BB8F8, &daNpc_P2_c::drawP2a);

/* 022BB9A4 */
void daNpc_P2_c::drawP2b() {
    WWHD_FUNC(0x022BB9A4, void, this);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    gabi::call(0x025BEBB8, 0x77, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}
VERIFY(0x022BB9A4, &daNpc_P2_c::drawP2b);

/* 022BBA24 */
void daNpc_P2_c::drawP2c() {
    WWHD_FUNC(0x022BBA24, void, this);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    mDoExt_McaMorf* book = mpBookMorf;
    env = dKy_getEnvlight();
    setLightTevColorType(env, book->getModel(), &tevStr);
    /* mpBookMorf->getModel()->setBaseTRMtx(mpMorf->getModel()->getAnmMtx(2)): the book model is read
     * before the joint block's dirty flag is written */
    u32 blk = gabi::load<u32>(gabi::ea(mpMorf->getModel()) + 0x2C);
    book = mpBookMorf;
    u16 fl = gabi::load<u16>(blk + 4);
    u32 anm = gabi::load<u32>(blk + 0x10) + 2 * 0x30;
    u32 bookModel = gabi::ea(book->getModel());
    gabi::store<u16>(blk + 4, (u16)(fl | 0x10));
    p2_mtx_copy(bookModel + 0xC8, anm); /* the book follows the backbone */
    mpBookMorf->updateDL();
    gabi::call(0x025BEBB8, 0x78, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}
VERIFY(0x022BBA24, &daNpc_P2_c::drawP2c);

/* 022BBB44 */
BOOL daNpc_P2_c::_draw() {
    WWHD_FUNC(0x022BBB44, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    switch ((u8)mType) {
    case TYPE_P2A_e: drawP2a(); break;
    case TYPE_P2B_e: drawP2b(); break;
    case TYPE_P2C_e: drawP2c(); break;
    }
    return TRUE;
}
VERIFY(0x022BBB44, &daNpc_P2_c::_draw);

/* 022BBBCC */
static BOOL daNpc_P2Draw(void* i_this) {
    WWHD_FUNC(0x022BBBCC, BOOL, i_this);
    return ((daNpc_P2_c*)i_this)->_draw();
}
VERIFY(0x022BBBCC, daNpc_P2Draw);

/* 022BDB70: daNpc_P2_childHIO_c::daNpc_P2_childHIO_c (GHS: allocates when this == NULL) */
static daNpc_P2_childHIO_c* daNpc_P2_childHIO_ct(daNpc_P2_childHIO_c* self) {
    WWHD_FUNC(0x022BDB70, daNpc_P2_childHIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_P2_childHIO_c*)operator_new(0xD8);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x1001FB6C;
    gabi::call(0x0259DA18, &self->mNpc); /* dNpc_HIO_c::dNpc_HIO_c */
    u32 p = gabi::ea(self);
    auto F = [p](u32 off, f32 v) { gabi::store<f32>(p + off, v); };
    F(0x54, 80.0f);
    F(0x4C, -545.0f);
    F(0x48, 5.0f);
    F(0x70, 5000.0f);
    F(0x78, 14.0f);
    F(0x44, -3400.0f);
    F(0x74, 0.0f);
    F(0x38, -3020.0f);
    F(0x30, 0.0f);
    F(0x8C, 4.0f);
    F(0xA4, 20.0f);
    F(0x90, 8.0f);
    F(0x3C, 0.0f);
    F(0x34, -550.0f);
    F(0x84, 4.0f);
    F(0x40, -550.0f);
    F(0xB4, 4.0f);
    F(0x5C, 200.0f);
    F(0x88, 0.0f);
    F(0x9C, 2.0f);
    F(0x80, 8.0f);
    F(0xC8, 8.0f);
    F(0xAC, 12.0f);
    F(0xD0, 8.0f);
    gabi::store<u8>(p + 0x2C, 0);
    gabi::store<u8>(p + 0x6C, 0);
    F(0xBC, 8.0f);
    F(0x60, 300.0f);
    F(0xA8, 8.0f);
    F(0xC0, 8.0f);
    F(0xB0, 8.0f);
    F(0xA0, 20.0f);
    F(0x58, 180.0f);
    F(0x68, 300.0f);
    F(0x7C, 8.0f);
    F(0x50, 30.0f);
    F(0xB8, 8.0f);
    F(0xCC, 15.0f);
    F(0xC4, 8.0f);
    F(0x94, 6.0f);
    F(0x64, 30.0f);
    F(0x98, 6.0f);
    return self;
}
VERIFY(0x022BDB70, daNpc_P2_childHIO_ct);

/* 022BDD10: daNpc_P2_HIO_c::daNpc_P2_HIO_c */
static daNpc_P2_HIO_c* daNpc_P2_HIO_ct(daNpc_P2_HIO_c* self) {
    WWHD_FUNC(0x022BDD10, daNpc_P2_HIO_c*, self);
    if (self == nullptr) {
        self = (daNpc_P2_HIO_c*)operator_new(0x294);
        if (self == nullptr) return self;
    }
    self->__vtbl = 0x1001FB7C;
    gabi::call(0x028EFFD0, self->children, 3, 0xD8, 0x022BDB70); /* __construct_array */
    u32 p = gabi::ea(self);
    auto H = [p](u32 off, s16 v) { gabi::store<s16>(p + off, v); };
    auto F = [p](u32 off, f32 v) { gabi::store<f32>(p + off, v); };
    auto B = [p](u32 off, u8 v) { gabi::store<u8>(p + off, v); };
    /* children[0] (+0x04) */
    F(0x08, -20.0f);   /* m04 */
    H(0x0C, 0x834);    /* mMaxHeadX */
    H(0x0E, 0);        /* mMaxBackboneX */
    H(0x10, 0x28A0);   /* mMaxHeadY */
    H(0x12, 0x1130);   /* mMaxBackboneY */
    H(0x14, -0x1FFE);  /* mMinHeadX */
    H(0x16, 0);        /* mMinBackboneX */
    H(0x18, -0x28A0);  /* mMinHeadY */
    H(0x1A, -0x1130);  /* mMinBackboneY */
    H(0x1C, 0x258);    /* mMaxTurnStep */
    H(0x1E, 0);        /* mMaxHeadTurnVel */
    F(0x20, 50.0f);    /* mAttnYOffset */
    H(0x24, 0x4000);   /* mMaxAttnAngleY */
    B(0x26, 0);
    F(0x28, 200.0f);   /* mMaxAttnDistXZ */
    /* children[1] (+0xDC) */
    F(0xE0, -26.0f);
    H(0xE4, 0x1FFE);
    H(0xE6, 0);
    H(0xE8, 0x4268);
    H(0xEA, 0xFA0);
    H(0xEC, -0x1FFE);
    H(0xEE, 0);
    H(0xF0, -0x3A98);
    H(0xF2, -0xFA0);
    H(0xF4, 0x1000);
    H(0xF6, 0x800);
    F(0xF8, 50.0f);
    H(0xFC, 0x4000);
    B(0xFE, 0);
    F(0x100, 200.0f);
    /* children[2] (+0x1B4) */
    F(0x1B8, -20.0f);
    H(0x1BC, 0x1FFE);
    H(0x1BE, 0);
    H(0x1C0, 0x2134);
    H(0x1C2, 0x8E8);
    H(0x1C4, -0x1FFE);
    H(0x1C6, 0);
    H(0x1C8, -0x2134);
    H(0x1CA, -0x8E8);
    H(0x1CC, 0x960);
    H(0x1CE, 0x7D0);
    F(0x1D0, 50.0f);
    H(0x1D4, 0x4000);
    B(0x1D6, 0);
    F(0x1D8, 200.0f);
    self->mRunSpeedScale = 3.0f;
    self->mMinRunSpeed = 0.9f;
    return self;
}
VERIFY(0x022BDD10, daNpc_P2_HIO_ct);

/* 022BDEC8: __sinit_d_a_npc_p2_cpp (compiler-generated) */
static void __sinit_d_a_npc_p2_cpp() {
    WWHD_FUNC(0x022BDEC8, void);
    sinit_header_statics(0x10467FE4, 0x101C2E78);
    daNpc_P2_HIO_ct(p2_HIO()); /* l_HIO */
    gabi::store<f32>(0x10467FE0, 80.0f); /* l_rope_dist */
}
VERIFY(0x022BDEC8, __sinit_d_a_npc_p2_cpp);

/* 022BDF78: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dtor(SafeString* self, u32 flags) {
    WWHD_FUNC(0x022BDF78, void, self, flags);
    if (self != nullptr && (flags & 1)) operator_delete(self);
}
VERIFY(0x022BDF78, SafeString_dtor);

/* 022BDF8C */
static BOOL daNpc_P2IsDelete(void*) {
    WWHD_FUNC(0x022BDF8C, BOOL, (void*)nullptr);
    return TRUE;
}
VERIFY(0x022BDF8C, daNpc_P2IsDelete);

/* 022BDF94: daNpc_P2_c deleting destructor (HD virtual destructor) */
static void daNpc_P2_dtor(daNpc_P2_c* self, u32 flags) {
    WWHD_FUNC(0x022BDF94, void, self, flags);
    if (self == nullptr) return;
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    u32 p = gabi::ea(self);
    gabi::store<u32>(p + 0x514, 0x1001FB3C); /* dBgS_ObjAcch vtables (this TU) */
    gabi::store<u32>(p + 0x508, 0x1001FB4C);
    gabi::call(0x024EFD9C, &self->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x025D50BC, self, 0);            /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x022BDF94, daNpc_P2_dtor);

/* 022BE030: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(SafeString* self) {
    WWHD_FUNC(0x022BE030, void, self);
}
VERIFY(0x022BE030, SafeString_assureTerminationImpl);
