/* Zuko, Niko and Mako: heap, drawing and base callbacks of the split P2 unit.
 * GameCube names, measured HD layouts and call order; verified against cking.rpx. */
#include "d/actor/d_a_npc_p2_base.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1001FB04,
    .objAcch = {0x1001FB2C, 0x1001FB4C, 0x1001FB3C},
    .objAcchDt = {0x1001FB3C, 0x1001FB4C},
    .headerStatics = {0x10467FE4, 0x101C2E78, 0x10467FF0},
};
namespace {
enum P2Joint { JNT_BACKBONE = 2, JNT_HEAD = 4, JNT_DAGGER1 = 12, JNT_DAGGER2 = 14 };
enum P2Figure { FIGURE_ZUKO_e = 0x76, FIGURE_NIKO_e = 0x77, FIGURE_MAKO_e = 0x78 };
struct P2Vtables { u32 actor, aab, childHIO, hio; };
const P2Vtables l_vtables = {0x1001FB5C, 0x1001FB1C, 0x1001FB6C, 0x1001FB7C};
// HD: the base source owns TU-wide HIO storage; split sources use their existing accessors.
static daNpc_P2_HIO_c l_HIO;
static f32 l_rope_dist;
static cXyz l_offset;
static cXyz l_eyeOffset;
static u32 l_offsetInitialized;
static u32 l_eyeOffsetInitialized;
static const s32 l_btp_idx[2] = {54, 55};
static const s32 l_bmd_idx[3] = {38, 50, 51};
static const s32 l_head_bmd_idx[3] = {45, 46, 47};
static const u8 l_btp_num[3] = {0, 1, 0};
// HD: retain the verified 0x17-byte stride, including reads beyond this table into adjacent animation data.
static const s8 l_btp_tbl[48] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const u32 l_heap_size[3] = {16096, 14304, 17024};
static const char* l_evn_tbl[11] = {"SURPRISE", "OMAMORI_INIT", "OMAMORI_END", "P2B_INTRO", "P2B_TO_GOAL", "P2B_GOAL_WAIT_TALK", "P2B_GOAL", "P2B_ARRIVE_MAJYU", "P2B_INTRO_2", "P2B_GOAL_2", "P2B_BOMB_GET"};
} // namespace

// HD: GHS constructor entry allocates when this is NULL; stores keep the original order.
/* 022BDB70 */
static daNpc_P2_childHIO_c* daNpc_P2_childHIO_ct(daNpc_P2_childHIO_c* self) {
    if (self == nullptr) {
        self = (daNpc_P2_childHIO_c*)operator_new(0xD8);
        if (self == nullptr) return self;
    }
    self->__vtbl = l_vtables.childHIO;
    P2Base_ctorNpcHIO(&self->mNpc); /* dNpc_HIO_c::dNpc_HIO_c */
    self->mDemoWaitR = 80.0f;
    self->mDemoWaitPos.y = -545.0f;
    self->mDemoWaitPos.x = 5.0f;
    self->mGoalDist = 5000.0f;
    self->mMorf[1] = 14.0f;
    self->mGoalPos2.z = -3400.0f;
    self->mMorf[0] = 0.0f;
    self->mGoalTalkPos.z = -3020.0f;
    self->mGoalTalkPos.x = 0.0f;
    self->mMorf[6] = 4.0f;
    self->mMorf[12] = 20.0f;
    self->mMorf[7] = 8.0f;
    self->mGoalPos2.x = 0.0f;
    self->mGoalTalkPos.y = -550.0f;
    self->mMorf[4] = 4.0f;
    self->mGoalPos2.y = -550.0f;
    self->mMorf[16] = 4.0f;
    self->mGoalTalkR = 200.0f;
    self->mMorf[5] = 0.0f;
    self->mMorf[10] = 2.0f;
    self->mMorf[3] = 8.0f;
    self->mMorf[21] = 8.0f;
    self->mMorf[14] = 12.0f;
    self->mMorf[23] = 8.0f;
    self->m2C = 0;
    self->m6C = 0;
    self->mMorf[18] = 8.0f;
    self->mGoal2R = 300.0f;
    self->mMorf[13] = 8.0f;
    self->mMorf[19] = 8.0f;
    self->mMorf[15] = 8.0f;
    self->mMorf[11] = 20.0f;
    self->mDemoWaitH = 180.0f;
    self->mGoal2H = 300.0f;
    self->mMorf[2] = 8.0f;
    self->mDemoWaitPos.z = 30.0f;
    self->mMorf[17] = 8.0f;
    self->mMorf[22] = 15.0f;
    self->mMorf[20] = 8.0f;
    self->mMorf[8] = 6.0f;
    self->mGoalTalkH = 30.0f;
    self->mMorf[9] = 6.0f;
    return self;
}

// HD: GHS constructor entry allocates when this is NULL.
/* 022BDD10 */
static daNpc_P2_HIO_c* daNpc_P2_HIO_ct(daNpc_P2_HIO_c* self) {
    if (self == nullptr) {
        self = (daNpc_P2_HIO_c*)operator_new(0x294);
        if (self == nullptr) return self;
    }
    self->__vtbl = l_vtables.hio;
    P2Base_constructArray(self->children, 3, 0xD8, gabi::guest_fn((const void*)&daNpc_P2_childHIO_ct)); // HD: __construct_array
    // Zuko
    self->children[0].mNpc.m04 = -20.0f;
    self->children[0].mNpc.mMaxHeadX = 0x834;
    self->children[0].mNpc.mMaxBackboneX = 0;
    self->children[0].mNpc.mMaxHeadY = 0x28A0;
    self->children[0].mNpc.mMaxBackboneY = 0x1130;
    self->children[0].mNpc.mMinHeadX = -0x1FFE;
    self->children[0].mNpc.mMinBackboneX = 0;
    self->children[0].mNpc.mMinHeadY = -0x28A0;
    self->children[0].mNpc.mMinBackboneY = -0x1130;
    self->children[0].mNpc.mMaxTurnStep = 0x258;
    self->children[0].mNpc.mMaxHeadTurnVel = 0;
    self->children[0].mNpc.mAttnYOffset = 50.0f;
    self->children[0].mNpc.mMaxAttnAngleY = 0x4000;
    self->children[0].mNpc.m22 = 0;
    self->children[0].mNpc.mMaxAttnDistXZ = 200.0f;
    // Niko
    self->children[1].mNpc.m04 = -26.0f;
    self->children[1].mNpc.mMaxHeadX = 0x1FFE;
    self->children[1].mNpc.mMaxBackboneX = 0;
    self->children[1].mNpc.mMaxHeadY = 0x4268;
    self->children[1].mNpc.mMaxBackboneY = 0xFA0;
    self->children[1].mNpc.mMinHeadX = -0x1FFE;
    self->children[1].mNpc.mMinBackboneX = 0;
    self->children[1].mNpc.mMinHeadY = -0x3A98;
    self->children[1].mNpc.mMinBackboneY = -0xFA0;
    self->children[1].mNpc.mMaxTurnStep = 0x1000;
    self->children[1].mNpc.mMaxHeadTurnVel = 0x800;
    self->children[1].mNpc.mAttnYOffset = 50.0f;
    self->children[1].mNpc.mMaxAttnAngleY = 0x4000;
    self->children[1].mNpc.m22 = 0;
    self->children[1].mNpc.mMaxAttnDistXZ = 200.0f;
    // Mako
    self->children[2].mNpc.m04 = -20.0f;
    self->children[2].mNpc.mMaxHeadX = 0x1FFE;
    self->children[2].mNpc.mMaxBackboneX = 0;
    self->children[2].mNpc.mMaxHeadY = 0x2134;
    self->children[2].mNpc.mMaxBackboneY = 0x8E8;
    self->children[2].mNpc.mMinHeadX = -0x1FFE;
    self->children[2].mNpc.mMinBackboneX = 0;
    self->children[2].mNpc.mMinHeadY = -0x2134;
    self->children[2].mNpc.mMinBackboneY = -0x8E8;
    self->children[2].mNpc.mMaxTurnStep = 0x960;
    self->children[2].mNpc.mMaxHeadTurnVel = 0x7D0;
    self->children[2].mNpc.mAttnYOffset = 50.0f;
    self->children[2].mNpc.mMaxAttnAngleY = 0x4000;
    self->children[2].mNpc.m22 = 0;
    self->children[2].mNpc.mMaxAttnDistXZ = 200.0f;
    self->mRunSpeedScale = 3.0f;
    self->mMinRunSpeed = 0.9f;
    return self;
}

/* 022B5AEC */
static BOOL nodeCallBack(J3DNode* i_node, int i_param_2) {
    if (i_param_2 == J3DNodeCBCalcTiming_In) {
        J3DModel* model = j3dSys.getModel();
        daNpc_P2_c* i_this = static_cast<daNpc_P2_c*>(model->getUserArea());
        if (i_this != nullptr) {
            J3DJoint* joint = J3DNode_toJoint(i_node);
            u32 jntNo = reinterpret_cast<P2BaseJointData*>(joint)->mJntNo;
            Mtx34* stack = mDoMtx_stack_c::get();
            PSMTXCopy(model->getAnmMtx(jntNo), stack);
            if (jntNo == JNT_HEAD) { /* head */
                /* function-local statics: static cXyz l_offset(0, 0, 0), l_eyeOffset(20, 10, 0) */
                if (l_offsetInitialized == 0) {
                    l_offset.x = 0.0f;
                    l_offset.z = 0.0f;
                    l_offsetInitialized = 1;
                    l_offset.y = 0.0f;
                }
                if (l_eyeOffsetInitialized == 0) {
                    l_eyeOffsetInitialized = 1;
                    l_eyeOffset.x = 20.0f;
                    l_eyeOffset.z = 0.0f;
                    l_eyeOffset.y = 10.0f;
                }
                PSMTXMultVec(stack, &l_offset, &i_this->mHeadPos);
                mDoMtx_XrotM(stack, i_this->m_jnt.mAngles[0][1]);
                mDoMtx_ZrotM(stack, (s16)-i_this->m_jnt.mAngles[0][0]);
                PSMTXMultVec(stack, &l_eyeOffset, &i_this->mEyePos);
                if (i_this->mHeadCalcCount != 0xFF) {
                    i_this->mHeadCalcCount = (u8)(i_this->mHeadCalcCount + 1);
                }
            } else if (jntNo == JNT_BACKBONE) { /* backbone */
                mDoMtx_XrotM(stack, i_this->m_jnt.mAngles[1][1]);
                mDoMtx_ZrotM(stack, i_this->m_jnt.mAngles[1][0]);
                mDoMtx_ZrotM(stack, REG_S(12, 0));
                mDoMtx_XrotM(stack, REG_S(12, 1));
                mDoMtx_YrotM(stack, REG_S(12, 2));
            }
            PSMTXCopy(stack, J3DSys::mCurrentMtx); /* J3DSys::mCurrentMtx */
            Mtx34* anm = model->getAnmMtx(jntNo);         /* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
            J3DModel::mtx_assign(anm, stack);
        }
    }
    return TRUE;
}

/* 022B5D8C */
BOOL daNpc_P2_c::initTexPatternAnm(u32 i_modify) { // HD: forwards the complete register, rather than a bool.
    J3DModelData* modelData = J3DModel_getModelData(mpHeadModel);
    s32 idx = l_btp_idx[(s32)(s8)mBtpNum];
    J3DAnmTexPattern* head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes("P2", idx);
    if (head_tex_pattern == nullptr) {
        JUTAssertion::showAssert("d_a_npc_p2.cpp", 0x196, "head_tex_pattern != (0)" /* "head_tex_pattern != 0" */);
    }
    if (!P2Base_initBtp(mBtp, modelData, head_tex_pattern, 1, 2, 1.0f, 0, -1, (u32)i_modify, 0)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 022B78D8 */
void daNpc_P2_c::playTexPatternAnm() {
    if (!P2Base_calcTimer(&mBlinkTimer)) {
        J3DAnmTexPattern* tex = P2Base_btp(this)->mpAnm;
        s32 max = gabi::call_ptr<s32>(P2Base_pattern(tex)->vtable->getFrameMax, tex); /* getFrameMax() */
        if ((s32)(u32)mBlinkFrame >= max) {
            tex = P2Base_btp(this)->mpAnm;
            max = gabi::call_ptr<s32>(P2Base_pattern(tex)->vtable->getFrameMax, tex);
            mBlinkFrame = (u8)(mBlinkFrame - max);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = (u8)(mBlinkFrame + 1);
        }
    }
}

/* 022B6318 */
u32 daNpc_P2_c::setTexAnm() {
    u8 type = mType;
    s32 btp = (s8)l_btp_tbl[type * 0x17 + (s32)(s8)mTexAnm];
    if ((u32)btp == (u32)(s32)(s8)mBtpNum) return (u32)gabi::ea(this); // HD: unchanged animation leaves this in r3.
    if (btp == -1) return (u32)gabi::ea(this); // HD: unchanged animation leaves this in r3.
    if (type == TYPE_P2C_e) return (u32)gabi::ea(this); // HD: unchanged animation leaves this in r3.
    mBtpNum = (s8)btp;
    return initTexPatternAnm(true);
}

/* 022B799C */
void daNpc_P2_c::talkInit() {
    mTalkState = 0;
}

/* 022BB044 */
void daNpc_P2_c::eventOrder() {
    s8 order = mOrder;
    if (order == 1 || order == 2) {
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (mOrder == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        P2Base_orderOther(this, l_evn_tbl[order], 1, 0xFFFF); /* fopAcM_orderOtherEvent2 */
    }
}

/* 022B79A8 */
void daNpc_P2_c::checkOrder() {
    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
    if (cmd == dEvtCmd_INDEMO_e) {
        mOrder = 0;
    } else if (cmd == dEvtCmd_INTALK_e) {
        if (mOrder == 1 || mOrder == 2) {
            mOrder = 0;
            mbTalk = 1;
            talkInit();
        }
    }
}

/* 022BB0A4 */
void daNpc_P2_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 022BB174 */
void daNpc_P2_c::setCollision() {
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

/* 022BB654 */
void daNpc_P2_c::draw_item(J3DModel* i_model, s8 i_jnt) {
    g_env_light.setLightTevColorType(i_model, &tevStr);
    Mtx34* anm = mpMorf->getModel()->getAnmMtx(i_jnt);
    PSMTXCopy(anm, mDoMtx_stack_c::get());
    i_model->setBaseTRMtx(mDoMtx_stack_c::get()); /* setBaseTRMtx */
    mDoExt_modelUpdateDL(i_model, 0);
}

/* 022BB744 */
void daNpc_P2_c::drawDagger() {
    if (mbDagger) {
        draw_item(mpDaggerModel, JNT_DAGGER1);
    } else {
        draw_item(mpDagger2Model, JNT_DAGGER2);
    }
}

/* 022BB768 */
void daNpc_P2_c::drawHead() {
    g_env_light.setLightTevColorType(mpHeadModel, &tevStr);
    J3DModel* head = mpHeadModel;
    if (mType != TYPE_P2C_e) {
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)mBtp, J3DModel_getModelData(head), (s16)(u16)mBlinkFrame);
        Mtx34* anm = mpMorf->getModel()->getAnmMtx(JNT_HEAD);
        mpHeadModel->setBaseTRMtx(anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
        P2Base_patternData(mpHeadModel->getModelData())->mTexPatternEntry = 0; /* HD: clear the texture pattern entry */
    } else {
        Mtx34* anm = mpMorf->getModel()->getAnmMtx(JNT_HEAD);
        head->setBaseTRMtx(anm);
        mDoExt_modelUpdateDL(mpHeadModel, 0);
    }
}

/* 022BB8F8 */
void daNpc_P2_c::drawP2a() {
    mDoExt_McaMorf* morf = mpMorf;
    g_env_light.setLightTevColorType(morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    if (!dComIfGs_isEventBit(0x808)) {
        draw_item(mpTelescopeModel, JNT_DAGGER1);
    }
    P2Base_registerFigure(FIGURE_ZUKO_e, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}

/* 022BB9A4 */
void daNpc_P2_c::drawP2b() {
    mDoExt_McaMorf* morf = mpMorf;
    g_env_light.setLightTevColorType(morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    P2Base_registerFigure(FIGURE_NIKO_e, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}

/* 022BBA24 */
void daNpc_P2_c::drawP2c() {
    mDoExt_McaMorf* morf = mpMorf;
    g_env_light.setLightTevColorType(morf->getModel(), &tevStr);
    mpMorf->entryDL();
    drawDagger();
    drawHead();
    mDoExt_McaMorf* book = mpBookMorf;
    g_env_light.setLightTevColorType(book->getModel(), &tevStr);
    /* mpBookMorf->getModel()->setBaseTRMtx(mpMorf->getModel()->getAnmMtx(2)): the book model is read
     * before the joint block's dirty flag is written */
    J3DMtxBuffer_hd* blk = mpMorf->getModel()->mpMtxBuffer;
    book = mpBookMorf;
    u16 fl = blk->mFlags;
    Mtx34* anm = P2Base_jointMatrix(blk, JNT_BACKBONE);
    J3DModel* bookModel = book->getModel();
    blk->mFlags = fl | 0x10;
    bookModel->setBaseTRMtx(anm); /* the book follows the backbone */
    mpBookMorf->updateDL();
    P2Base_registerFigure(FIGURE_MAKO_e, this, &current.pos, (s32)current.angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
}

/* 022BBB44 */
BOOL daNpc_P2_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    switch ((u8)mType) {
    case TYPE_P2A_e: drawP2a(); break;
    case TYPE_P2B_e: drawP2b(); break;
    case TYPE_P2C_e: drawP2c(); break;
    }
    return TRUE;
}

/* 022B62D8 */
static BOOL CreateHeap_CB(fopAc_ac_c* i_this) {
    return ((daNpc_P2_c*)i_this)->_createHeap();
}

/* 022B62DC */
void daNpc_P2_c::getArg() {
    u32 param = mParameters;
    mSwitchNo = (u8)(param >> 10);
    u8 sub = (u8)(param >> 2);
    mSubType = sub;
    u8 type = (u8)(param & 3);
    mType = type;
    if (type == 3) mType = 0;
    if (sub == 0xFF) mSubType = 0;
}

/* 022B5EA0 */
BOOL daNpc_P2_c::_createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("P2", l_bmd_idx[(u32)mType]);
    if (modelData == nullptr) {
        JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA20, "modelData != (0)" /* "modelData != 0" */);
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
    J3DModelData* headModelData = (J3DModelData*)dComIfG_getObjectRes("P2", l_head_bmd_idx[(u32)mType]);
    if (headModelData == nullptr) {
        JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA3C, "headModelData != (0)" /* "headModelData != 0" */);
    }
    if (mType != TYPE_P2C_e) {
        J3DModel* head = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
        mpHeadModel = head;
        if (head == nullptr) return FALSE;
        mBtpNum = (s8)l_btp_num[(u32)mType];
        if (!initTexPatternAnm(false)) return FALSE;
    } else {
        J3DModel* head = mDoExt_J3DModel__create(headModelData, 0, 0x11020203);
        mpHeadModel = head;
        if (head == nullptr) return FALSE;
    }
    J3DModelData* daggerModelData = (J3DModelData*)dComIfG_getObjectRes("P2", 0x2A);
    if (daggerModelData == nullptr) {
        JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA59, "daggerModelData != (0)" /* "daggerModelData != 0" */);
    }
    J3DModel* dagger = mDoExt_J3DModel__create(daggerModelData, 0, 0x11020203);
    mpDaggerModel = dagger;
    if (dagger == nullptr) return FALSE;
    J3DModelData* dagger2ModelData = (J3DModelData*)dComIfG_getObjectRes("P2", 0x27);
    /* HD: the assertion tests the first dagger's model data again */
    if (daggerModelData == nullptr) {
        JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA60, "daggerModelData != (0)" /* "daggerModelData != 0" */);
    }
    J3DModel* dagger2 = mDoExt_J3DModel__create(dagger2ModelData, 0, 0x11020203);
    mpDagger2Model = dagger2;
    if (dagger2 == nullptr) return FALSE;
    if (mType == TYPE_P2C_e) {
        J3DModelData* bookModelData = (J3DModelData*)dComIfG_getObjectRes("P2", 0x29);
        if (bookModelData == nullptr) {
            JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA68, "bookModelData != (0)" /* "bookModelData != 0" */);
        }
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes("P2", 0x23);
        mDoExt_McaMorf* book = mDoExt_McaMorf::create(nullptr, bookModelData, nullptr, nullptr, bck, 0, 1.0f, 0, -1, 1,
                                                      nullptr, 0, 0x11020203);
        mpBookMorf = book;
        if (book == nullptr || book->getModel() == nullptr) return FALSE;
    }
    if (mType == TYPE_P2A_e) {
        J3DModelData* telescopeModelData = (J3DModelData*)dComIfG_getObjectRes("P2", 0x28);
        if (telescopeModelData == nullptr) {
            JUTAssertion::showAssert("d_a_npc_p2.cpp", 0xA7B, "telescopeModelData != (0)" /* "telescopeModelData != 0" */);
        }
        J3DModel* telescope = mDoExt_J3DModel__create(telescopeModelData, 0, 0x11020203);
        mpTelescopeModel = telescope;
        if (telescope == nullptr) return FALSE;
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this);
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == JNT_HEAD || i == JNT_BACKBONE) {
            /* getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack): HD joint nodes are
             * 0x1C-byte records at *(data + 8), the count at data + 4 (inline bound check) */
            mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
        }
    }
    return TRUE;
}

/* 022B76BC */
cPhs_State daNpc_P2_c::_create() {
    /* fopAcM_ct(this, daNpc_P2_c): the inline constructor (HD virtual destructor) */
    u32 cond = actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = l_vtables.actor;
            P2Base_ctorBtp(mBtp);       /* mDoExt_btpAnm */
            P2Base_ctorJoint(&m_jnt);     /* dNpc_JntCtrl_c */
            P2Base_ctorEventCut(&mEventCut); /* dNpc_EventCut_c */
            dBgS_ObjAcch_ct(&mObjAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, l_vtables.aab);
            P2Base_ctorSmoke(mSmokeCB, 1); /* dPa_smokeEcallBack(1) */
            cond = actor_condition;
        }
        actor_condition = cond | fopAcCnd_INIT_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "P2");
    if (state == cPhs_COMPLEATE_e) {
        getArg();
        if (!fopAcM_entrySolidHeap(this, CreateHeap_CB, l_heap_size[(u32)mType])) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return state;
}

/* 022B7868 */
BOOL daNpc_P2_c::_delete() {
    dComIfG_resDelete(&mPhs, "P2");
    /* mSmokeCB.remove() (virtual, vtable +0x44) */
    gabi::call_ptr(P2Base_smoke(this)->vtable->remove, mSmokeCB);
    mDoAud_seDeleteObject(&mRopePos);
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 022B7864 */
static cPhs_State daNpc_P2Create(void* i_this) {
    return ((daNpc_P2_c*)i_this)->_create();
}

/* 022B78D4 */
static BOOL daNpc_P2Delete(void* i_this) {
    return ((daNpc_P2_c*)i_this)->_delete();
}

/* 022BB650 */
static BOOL daNpc_P2Execute(void* i_this) {
    return ((daNpc_P2_c*)i_this)->_execute();
}

/* 022BBBCC */
static BOOL daNpc_P2Draw(void* i_this) {
    return ((daNpc_P2_c*)i_this)->_draw();
}

/* 022BDF8C */
static BOOL daNpc_P2IsDelete(void*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 022BDEC8 */
static void __sinit_d_a_npc_p2_cpp() {
    __sinit_header_statics();
    daNpc_P2_HIO_ct(&l_HIO);
    l_rope_dist = 80.0f;
}

/* 022BDF78 */
static void SafeString_dtor(SafeString* self, u32 flags) {
    if (self != nullptr && (flags & 1)) operator_delete(self);
}

/* 022BDF94 */
static void daNpc_P2_dtor(daNpc_P2_c* self, u32 flags) {
    if (self == nullptr) return;
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    P2Base_dtorCircle(&self->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
    self->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0]; /* dBgS_ObjAcch vtables (this TU) */
    self->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    P2Base_dtorAcch(&self->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    P2Base_dtorActor(self, 0);            /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}

/* 022BE030 */
static void SafeString_assureTerminationImpl(SafeString* self) {
}

/* WWHD glue
data l_btp_idx 1001FAF0
data l_bmd_idx 1001FB8C
data l_head_bmd_idx 1001FBD4
data l_btp_num 1001FBD0
data l_btp_tbl 101C2C78
data l_heap_size 100200A4
data l_evn_tbl 101C2E4C
data l_HIO 10468054 nocheck
data l_rope_dist 10467FE0 nocheck
data l_offset 10468000 nocheck
data l_eyeOffset 1046800C nocheck
data l_offsetInitialized 104682E8 nocheck
data l_eyeOffsetInitialized 104682EC nocheck
str 100200A0 "P2"
str 1001FBA8
str 1001FBC0 in 022B5D8C
str 1001FBE0
str 1001FBFC in 022B5EA0
str 1001FC0C
str 1001FC20
str 1001FC38
str 1001FC50
str 1001FF00
str 1001FF0C
str 1001FF1C
str 1001FF28
str 1001FF34
str 1001FF40
str 1001FF54
str 1001FF60
str 1001FF74
str 1001FF80
str 1001FF8C
ext 025E789C s32 P2Base_initBtp(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32)
ext 02055B64 s16 P2Base_calcTimer(be<s16>*)
ext 025E7820 void P2Base_ctorBtp(void*)
ext 0259DAA0 void P2Base_ctorJoint(dNpc_JntCtrl_c*)
ext 0259F740 void P2Base_ctorEventCut(dNpc_EventCut_c*)
ext 025A5B18 void P2Base_ctorSmoke(void*, s32)
ext 025D77DC void P2Base_orderOther(fopAc_ac_c*, const char*, s32, u32)
ext 025BEBB8 void P2Base_registerFigure(s32, fopAc_ac_c*, cXyz*, s32, f32, f32, f32)
ext 0259DA18 void P2Base_ctorNpcHIO(dNpc_HIO_p2*)
ext 028EFFD0 void P2Base_constructArray(void*, u32, u32, u32)
ext 02018034 void P2Base_dtorCircle(void*, s32)
ext 024EFD9C void P2Base_dtorAcch(dBgS_ObjAcch*, s32)
ext 025D50BC void P2Base_dtorActor(fopAc_ac_c*, s32)
*/
