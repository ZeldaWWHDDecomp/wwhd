/**
 * d_a_npc_zk1.cpp (WWHD)
 * NPC - Zuko (Rito postman)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_zk1.cpp) has only "Nonmatching" stubs here, so the functions are written
 * from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#include "d/actor/d_a_npc_zk1.h"

// HD: this TU owns each mapped object once.
const gc_tu_data gc_tu = {.safeString = 0x10023964};
static daNpc_Zk1_HIO_c& l_HIO = gabi::object_at<daNpc_Zk1_HIO_c>(0x10468ACC);
static cXyz& a_eye_pos_off = gabi::object_at<cXyz>(0x10468B04);
static be<u32>& l_eye_guard = gabi::object_at<be<u32>>(0x10468B10);
static be<u32>& l_demo_object = gabi::object_at<be<u32>>(0x101D5FFC);
static Zk1ActionFunc& l_wait_action = gabi::object_at<Zk1ActionFunc>(0x10023948);
static dCcD_SrcCyl& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static gptr<void>& l_message_manager = gabi::object_at<gptr<void>>(0x101F4B5C);
static be<u32>& l_red_guard = gabi::object_at<be<u32>>(0x101FDA50);
static be<u32>& l_blue_guard = gabi::object_at<be<u32>>(0x101FDAC0);
static be<u32>& l_red_colour = gabi::object_at<be<u32>>(0x101FEBF4);
static be<u32>& l_blue_colour = gabi::object_at<be<u32>>(0x101FEBF8);
static be<u32>& a_red_colour = gabi::object_at<be<u32>>(0x10023950);
static be<u32>& a_blue_colour = gabi::object_at<be<u32>>(0x10023954);
using Zk1PrmBytes = be<u8>[0x1C];
static Zk1PrmBytes& a_prm_tbl = gabi::object_at<Zk1PrmBytes>(0x101C7050);
static be<s32>& a_btp_resID = gabi::object_at<be<s32>>(0x100239D4);
using Zk1BckIds = be<s32>[4];
static Zk1BckIds& a_bck_resID_tbl = gabi::object_at<Zk1BckIds>(0x10023A8C);
using Zk1AnmParams = daNpc_Zk1_c::anm_prm_c[3];
using Zk1AttrParams = daNpc_Zk1_c::anm_prm_c[4];
static Zk1AnmParams& a_anm_state_tbl = gabi::object_at<Zk1AnmParams>(0x101C6FE0);
static Zk1AttrParams& a_anm_attr_tbl = gabi::object_at<Zk1AttrParams>(0x101C7010);
using Zk1HeapSizes = be<u32>[2];
static Zk1HeapSizes& a_heap_size_tbl = gabi::object_at<Zk1HeapSizes>(0x101C6FD4);
static gptr<char>& l_evn_name = gabi::object_at<gptr<char>>(0x101C6FB0);
static gptr<char>& cut_name_tbl = gabi::object_at<gptr<char>>(0x101C6FDC);
using Zk1ArchiveName = be<u8>[3];
static Zk1ArchiveName& l_arc_name = gabi::object_at<Zk1ArchiveName>(0x10023A68);
enum Zk1MessageStatus { ZK1_MSG_TYPING = 6, ZK1_MSG_DISPLAYED = 14, ZK1_MSG_DESTROYED = 19 };
enum Zk1State { ZK1_WAIT = 1, ZK1_TALK = 2 };

/* 022FF9AC */
void daNpc_Zk1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_model) {

    // HD: preserve the guarded eye offset initialization and its store order.

    if ((u32)l_eye_guard == 0) {
        l_eye_guard = 1;
        a_eye_pos_off.z = 0.0f;
        a_eye_pos_off.x = 14.0f;
        a_eye_pos_off.y = 18.0f;
    }
    u32 jntNo = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jntNo), stk);
    mHeadPos.x = stk->m[0][3];
    mHeadPos.y = stk->m[1][3];
    mHeadPos.z = stk->m[2][3];
    PSMTXMultVec(stk, &a_eye_pos_off, &mEyePos);
    mDoMtx_XrotM(stk, m_jnt.mAngles[0][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[0][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jntNo), stk);
}

/* 022FFB24 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpc_Zk1_c* i_this = static_cast<daNpc_Zk1_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->_nodeCB_Head(i_node, model);
        }
    }
    return TRUE;
}

/* 022FFB6C */
void daNpc_Zk1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_model) {

    u32 jntNo = reinterpret_cast<J3DJoint*>(i_node)->getJntNo();
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(i_model->getAnmMtx(jntNo), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, J3DSys::mCurrentMtx);
    mtx_copy(i_model->getAnmMtx(jntNo), stk);
}

/* 022FFC90 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_calcTiming) {

    if (i_calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpc_Zk1_c* i_this = static_cast<daNpc_Zk1_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->_nodeCB_BackBone(i_node, model);
        }
    }
    return TRUE;
}

/* 022FFCD8 */
s32 daNpc_Zk1_c::btpResID(int) {

    // HD: the resource table has one entry; the incoming index is ignored.
    return (s32)a_btp_resID;
}

/* 022FFCE4 */
bool daNpc_Zk1_c::setBtp(s32 i_btpNum, u32 i_modify) {

    J3DModel* model = mpMorf->getModel();
    if (i_btpNum < 0) {
        return false;
    }
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mArcName, btpResID(i_btpNum));
    if (btp == nullptr) /* JUT_ASSERT(402, a_btp != NULL) */
        JUT_ASSERT_fail("d_a_npc_zk1.cpp", 0x192, "a_btp != (0)");
    mBtpNum = (s8)i_btpNum;
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return Zk1_btpInit(mBtpAnm, model->getModelData(), btp, 1, 0 /* EMode_NONE */, 1.0f, 0, -1,
                       i_modify, 0) != 0;
}

/* 022FFDD0 */
u32 daNpc_Zk1_c::init_texPttrnAnm(s32 i_btpNum, u32 i_modify) {

    // HD: the tail call preserves the complete return register, rather than narrowing to bool.
    return Zk1_setBtpResult(this, i_btpNum, i_modify);
}

/* 022FFDD4 */
BOOL daNpc_Zk1_c::bodyCreateHeap() {

    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(mArcName, 4 /* bdl */);
    if (a_mdl_dat == nullptr) /* JUT_ASSERT(1477, a_mdl_dat != NULL) */
        JUT_ASSERT_fail("d_a_npc_zk1.cpp", 0x5C5, "a_mdl_dat != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                    1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        // HD: deletion uses the virtual deleting destructor.
        mDoExt_McaMorf* morf = mpMorf;
        if (morf != nullptr) {
            Zk1_destroyMorf(morf);
        }
        mpMorf = nullptr;
        return FALSE;
    }
    if (!init_texPttrnAnm(0, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_hed_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("head" /* "head" */);
    if (m_hed_jnt_num < 0) /* JUT_ASSERT(1499, m_hed_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_zk1.cpp", 0x5DB, "m_hed_jnt_num >= 0");
    m_bbone_jnt_num = (s8)a_mdl_dat->getJointName()->getIndex("backbone" /* "backbone" */);
    if (m_bbone_jnt_num < 0) /* JUT_ASSERT(1501, m_bbone_jnt_num >= 0) */
        JUT_ASSERT_fail("d_a_npc_zk1.cpp", 0x5DD, "m_bbone_jnt_num >= 0");
    /* getJointNodePointer(jnt)->setCallBack(...): a joint index out of range uses the table base */
    {
        J3DModelData* md = mpMorf->getModel()->getModelData();
        md->getJointNodePointer((u16)(s32)m_hed_jnt_num)->setCallBack(nodeCB_Head);
    }
    {
        J3DModelData* md = mpMorf->getModel()->getModelData();
        md->getJointNodePointer((u16)(s32)m_bbone_jnt_num)->setCallBack(nodeCB_BackBone);
    }
    mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
    return TRUE;
}

/* 02300094 */
BOOL daNpc_Zk1_c::CreateHeap() {

    if (!bodyCreateHeap()) {
        return FALSE;
    }
    mAcchCir.SetWall(30.0f, 80.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 02300124 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return ((daNpc_Zk1_c*)i_this)->CreateHeap(); }

/* 02300128 */
bool daNpc_Zk1_c::decideType(int i_param) {

    if (mType > 0) {
        return true;
    }
    mType = 1;
    mSpecificType = 0;
    // HD: copy the archive name into the actor without changing byte load/store order.
    u8 c0 = l_arc_name[0], c1 = l_arc_name[1], c2 = l_arc_name[2];
    Zk1_bytes(mArcName)[0] = c0;
    Zk1_bytes(mArcName)[1] = c1;
    Zk1_bytes(mArcName)[2] = c2;
    bool ret = false;
    if (mType != -1 && mSpecificType != -1) {
        ret = true;
    }
    return ret;
}

/* 0230018C */
BOOL daNpc_Zk1_c::set_action(Zk1ActionFunc* i_action, void* i_arg) {

    Zk1ActionFunc* cur = &mCurrProcFunc;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_action->f;
        newD = i_action->d;
    }
    if (cur->i != 0) {
        mActState = 9;
        Zk1_callAction(this, cur, i_arg);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActState = 0;
    Zk1_callAction(this, cur, i_arg);
    return TRUE;
}

/* 023002B8 */
bool daNpc_Zk1_c::init_ZK1_0() {

    if (!dComIfGs_isSymbol(1)) {
        return false;
    }
    gabi::Local<Zk1ActionFunc> pmf;
    Zk1_loadAction(pmf, &l_wait_action);
    set_action(pmf, nullptr);
    mFirstConversationDone = dComIfGs_isEventBit(0x1802) == 1;
    mSecondConversationDone = dComIfGs_isEventBit(0x1C01) == 1;
    return true;
}

/* 02300388 */
void daNpc_Zk1_c::play_btp_anm() {

    J3DAnmTexPattern* btp = Zk1_view<Zk1BtpAnm>(mBtpAnm)->mpAnimation.get();
    u8 maxFrame = (u8)Zk1_frameMax(btp);
    if (mBtpNum != 0 || Zk1_calcTimer(&mBlinkTimer) == 0) {
        u8 frame = (u8)(mBlinkFrame + 1);
        mBlinkFrame = frame;
        if (frame >= maxFrame) {
            if (mBtpNum == 0) {
                mBlinkTimer = (s16)Zk1_getRndValue(60, 90);
                maxFrame = 0;
            }
            mBlinkFrame = maxFrame;
        }
    }
}

/* 02300428 */
void daNpc_Zk1_c::play_animation() {

    u32 mtrlSndId = 0;
    play_btp_anm();
    if (mObjAcch.ChkGroundHit() /* mObjAcch.ChkGroundHit() */) {
        mtrlSndId = Zk1_materialSound(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mbMorfAnimStopped = (s8)Zk1_playMorf(mpMorf, &eyePos, mtrlSndId, reverb); /* mpMorf->play() */
    if (mpMorf->getFrame() < mPrevMorfFrame) {
        mbMorfAnimStopped = 1;
    }
    mPrevMorfFrame = mpMorf->getFrame();
}

/* 023004CC */
void daNpc_Zk1_c::setAttention(u32 i_setEyePos) {

    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO.mPrmTbl.mAttPosOffsetY;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (mbSetEyePos == 0 && i_setEyePos == 0) {
        return;
    }
    eyePos.z = mEyePos.z;
    eyePos.y = mEyePos.y;
    eyePos.x = mEyePos.x;
}

/* 02300520 */
void daNpc_Zk1_c::setMtx(u32 i_param) {

    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mAngle.x, mAngle.y, mAngle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    mpMorf->calc();
    setAttention(i_param);
}

/* 02300628 */
bool daNpc_Zk1_c::createInit() {

    // There is one named event.
    const char* name = l_evn_name.get();
    mEventIdTable[0] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    mEventCut.setActorInfo2("Zk1" /* "Zk1" */, (fopNpc_npc_c*)(void*)this);
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9; /* attention_info.distances[SPEAK] */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;  /* attention_info.distances[TALK] */
    mAnmNum = 4;
    gravity = 0.0f;
    bool ok;
    switch (mSpecificType) {
    case 0:
        ok = init_ZK1_0();
        break;
    default:
        ok = false;
        break;
    }
    if (!ok) {
        return false;
    }
    mAngle.x = current.angle.x;
    mAngle.y = current.angle.y;
    mAngle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = Zk1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Zk1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(1);
    return true;
}

/* 023007BC */
cPhs_State daNpc_Zk1_c::_create() {

    /* fopAcM_ct(this, daNpc_Zk1_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            Zk1_npcCtor(
                this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            __vtbl = 0x10023AB0;
            Zk1_btpCtor(mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!decideType(fopAcM_GetParam(this) & 0xFF)) {
        return cPhs_ERROR_e;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, mArcName);
    mbResLoaded = state == cPhs_COMPLEATE_e;
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    // Heap size follows the actor type.
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)a_heap_size_tbl[mType])) {
        return cPhs_ERROR_e;
    }
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, -20.0f, -70.0f, 70.0f, 240.0f, 70.0f);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}

/* 02300908 */
static cPhs_State daNpc_Zk1_Create(fopAc_ac_c* i_this) { return ((daNpc_Zk1_c*)i_this)->_create(); }

/* 0230090C */
BOOL daNpc_Zk1_c::_delete() {

    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}

/* 02300960 */
static BOOL daNpc_Zk1_Delete(daNpc_Zk1_c* i_this) { return i_this->_delete(); }

/* 02300964 */
void daNpc_Zk1_c::checkOrder() {

    u16 command = (u16)eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == dEvtCmd_INDEMO_e) {
        if (dComIfGp_evmng_startCheck(mEventIdTable[mEventIdx]) && mEvtCond >= 3) {
            mEvtCond = 0;
            mMesgAnimeTagInfo = 0xFF;
            mAnmAtr = 0xFF;
        }
    } else if (command == dEvtCmd_INTALK_e) {
        if (mEvtCond == 1 || mEvtCond == 2) {
            mEvtCond = 0;
            mTalkOrdered = 1;
        }
    }
}

/* 02300A3C */
u8 daNpc_Zk1_c::demo() {

    u8 id = demoActorID;
    if (id == 0) {
        if (mbInDemo != 0) {
            mbInDemo = 0;
        }
        return mbInDemo;
    }
    if (mbInDemo == 0) {
        m_jnt.mAngles[0][1] = 0;
        m_jnt.mAngles[1][0] = 0;
        mbInDemo = 1;
        mKeepShapeAngle = 0;
        m_jnt.mAngles[0][0] = 0;
        id = demoActorID;
        m_jnt.mAngles[1][1] = 0;
    }
    void* actor = nullptr;
    if (id != 0 && id <= 0x20) {
        /* dComIfGp_demo_getActor(id) */
        u32 obj = (u32)l_demo_object;
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            obj = (u32)l_demo_object;
        }
        actor = Zk1_demoGetActor(obj, id);
    }
    J3DAnmTexPattern* cur = Zk1_view<Zk1BtpAnm>(mBtpAnm)->mpAnimation.get();
    if (cur != nullptr) {
        u8 maxFrame = (u8)Zk1_frameMax(cur);
        u8 frame = (u8)(mBlinkFrame + 1);
        if (frame < maxFrame) {
            mBlinkFrame = frame;
        } else {
            mBlinkFrame = maxFrame;
        }
    }
    if (actor != nullptr) {
        J3DAnmTexPattern* btp = Zk1_demoBtp(actor, mArcName);
        if (btp != nullptr) {
            Zk1_btpInit(mBtpAnm, mpMorf->getModel()->getModelData(), btp, 1, 0, 1.0f, 0, -1, 1, 0);
            mBlinkFrame = 0;
            mBtpNum = 1;
        }
    }
    Zk1_demoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, 0, 0);
    return mbInDemo;
}

/* 02300BF8 */
s32 daNpc_Zk1_c::isEventEntry() {

    const char* name = Zk1_staffName(&mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 02300C38 */
void daNpc_Zk1_c::endEvent() {

    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
    mMesgAnimeTagInfo = 0xFF;
}

/* 02300C7C */
void daNpc_Zk1_c::privateCut(int i_staffIdx) {

    if (i_staffIdx == -1) {
        return;
    }
    // HD: the single private cut has no action initialization or action body.
    s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(i_staffIdx, &cut_name_tbl, 1, TRUE, 0);
    mActIdx = actIdx;
    if (actIdx != -1) {
        dComIfGp_evmng_getIsAddvance(i_staffIdx);
    }
    dComIfGp_evmng_cutEnd(i_staffIdx);
}

/* 02300D14 */
void daNpc_Zk1_c::event_proc(int i_staffIdx) {

    s16 ev = mEventIdTable[mEventIdx];
    if (dComIfGp_evmng_endCheck(ev)) {
        endEvent();
        return;
    }
    if (!mEventCut.cutProc()) {
        privateCut(i_staffIdx);
    }
}

/* 02300DB8 */
fopAc_ac_c* daNpc_Zk1_c::searchByID(fpc_ProcID i_id, be<s32>* o_res) {

    gabi::Local<be<u32>> actor;
    *actor = 0;
    *o_res = 0;
    if (!Zk1_searchById(i_id, actor)) {
        *o_res = 1;
    }
    return Zk1_actorFromIdResult(*actor);
}

/* 02300E0C */
void daNpc_Zk1_c::lookBack() {

    gabi::Local<cXyz> vec1;
    vec1->x = 0.0f;
    vec1->y = 0.0f;
    vec1->z = 0.0f;
    mJointHeadY = m_jnt.mAngles[0][1];
    f32 srcX = current.pos.x;
    s16 targetY = current.angle.y;
    mActorAngleY = targetY;
    u8 headOnly = mHeadOnlyFollow;
    f32 srcZ = current.pos.z;
    f32 srcY = eyePos.y;
    mJointBackboneY = m_jnt.mAngles[1][1];
    cXyz* dstPos = nullptr;
    switch ((u32)(s32)mLookBackState) {
    case 0:
        break;
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, -20.0f);
        Zk1_copyWords(&mLookTarget, eye.get());
        Zk1_copyWords(vec1.get(), eye.get());
        dstPos = vec1;
        break;
    }
    case 2:
        Zk1_copyWords(vec1.get(), &mLookTarget);
        dstPos = vec1;
        break;
    case 3:
        targetY = mTargetYRot;
        break;
    case 4: {
        gabi::Local<be<s32>> res;
        fopAc_ac_c* partner = searchByID(mPartnerID, res);
        if (partner != nullptr && *res == 0) {
            Zk1_copyWords(&mLookTarget, &partner->current.pos);
            f32 x = mLookTarget.x;
            f32 ey = partner->eyePos.y;
            vec1->x = x;
            f32 z = mLookTarget.z;
            vec1->y = ey;
            vec1->z = z;
            dstPos = vec1;
            mLookTarget.y = ey;
        }
        break;
    }
    default:
        break;
    }
    gabi::Local<cXyz> vec2; /* passed by value: a copy */
    vec2->x = srcX;
    vec2->z = srcZ;
    vec2->y = srcY;
    Zk1_lookAtTarget(&m_jnt, &current.angle.y, dstPos, vec2, targetY, l_HIO.mPrmTbl.mMaxTurnStep,
                     headOnly);
}

/* 023010A0 */
void daNpc_Zk1_c::eventOrder() {

    s8 cond = mEvtCond;
    if (cond == 1 || cond == 2) {
        s8 c = mEvtCond;
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
        if (c == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (cond >= 3) {
        s16 idx = (s16)(cond - 3);
        mEventIdx = idx;
        fopAcM_orderOtherEventId(this, mEventIdTable[idx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* the common tail of _execute's three paths */
static inline void zk1_setAngle(daNpc_Zk1_c* i_this) {
    s16 z = i_this->current.angle.z, y = i_this->current.angle.y, x = i_this->current.angle.x;
    i_this->mAngle.x = x;
    i_this->mAngle.y = y;
    i_this->mAngle.z = z;
    if (i_this->mKeepShapeAngle == 0) {
        i_this->shape_angle.z = i_this->current.angle.z;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.x = i_this->current.angle.x;
    }
}

/* 02301110 */
BOOL daNpc_Zk1_c::_execute() {

    if (mbRanExecute == 0) {
        mInitAngle.y = current.angle.y;
        mInitAngle.z = current.angle.z;
        Zk1_copyWords(&m72C, &current.pos);
        mInitAngle.x = current.angle.x;
        mbRanExecute = 1;
    }
    daNpc_Zk1_HIO_c::hio_prm_c& prm = l_HIO.mPrmTbl;
    m_jnt.setParam(prm.mJnt[4], prm.mJnt[5], prm.mJnt[6], prm.mJnt[7], prm.mJnt[0], prm.mJnt[1],
                   prm.mJnt[2], prm.mJnt[3], prm.mJnt[8]);
    if (mHidden != 0 && demoActorID == 0) {
        return TRUE;
    }
    checkOrder();
    if (demo() == 0) {
        s32 staffIdx = -1;
        bool evt = false;
        if (dComIfGp_event_runCheck() &&
            (u16)eventInfo.mCommand != 1 /* !eventInfo.checkCommandTalk() */) {
            staffIdx = isEventEntry();
            if (staffIdx >= 0)
                evt = true;
        }
        if (evt || mRunEvent != 0) {
            event_proc(staffIdx);
        } else {
            Zk1_callAction(this, &mCurrProcFunc, nullptr); /* (this->*mCurrProcFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
        eventOrder();
        zk1_setAngle(this);
    } else {
        mHidden = 0;
        eventOrder();
        zk1_setAngle(this);
    }
    tevStr.mRoomNo = Zk1_roomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride =
        Zk1_polyColour(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    setMtx(0);
    if (mbInDemo == 0) {
        setCollision(80.0f, 220.0f);
    }
    return TRUE;
}

/* 023013E0 */
static BOOL daNpc_Zk1_Execute(daNpc_Zk1_c* i_this) { return i_this->_execute(); }

/* 023013E4 */
BOOL daNpc_Zk1_c::_draw() {

    J3DModel* model = mpMorf->getModel();
    J3DModelData* model_data = model->getModelData();
    if (mHidden != 0 || mDrawDisabled != 0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, model_data, mBlinkFrame);
    mpMorf->entryDL();
    Zk1_view<Zk1ModelAnimation>(model_data)->mpBtpAnimation = 0; /* mBtpAnm.remove(model_data) */
    // HD: no shadowDraw()
    dSnap_RegistFig(0x8C, this, 1.0f, 1.0f, 1.0f);
    /* debug leftovers: function-local static colours initialised on first use */
    if (l_HIO.mPrmTbl.mDebug != 0) {
        Zk1_initDebugColour(&l_red_guard, &l_red_colour, &a_red_colour);
        Zk1_initDebugColour(&l_blue_guard, &l_blue_colour, &a_blue_colour);
    }
    return TRUE;
}

/* 02301518 */
static BOOL daNpc_Zk1_Draw(daNpc_Zk1_c* i_this) { return i_this->_draw(); }

/* 0230151C */
static BOOL daNpc_Zk1_IsDelete(daNpc_Zk1_c*) { return TRUE; }

/* 02301524 */
s32 daNpc_Zk1_c::bckResID(int i_idx) {

    // Resource lookup follows the animation index.
    return (s32)a_bck_resID_tbl[i_idx];
}

/* 02301538 */
void daNpc_Zk1_c::setAnm_anm(anm_prm_c* i_prm) {

    s8 num = i_prm->mAnmNum;
    if (num < 0 || mAnmNum == num) {
        return;
    }
    s32 resID = bckResID(num);
    s32 loopMode = i_prm->mLoopMode;
    f32 morf = i_prm->mMorf;
    f32 speed = i_prm->mSpeed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    mbMorfAnimStopped = 0;
    mAnmNum = i_prm->mAnmNum;
    mPrevMorfFrame = 0.0f;
    mAnimationLoopCount = 0;
}

/* 023015D0 */
void daNpc_Zk1_c::setAnm() {

    anm_prm_c* a_anm_prm_tbl = a_anm_state_tbl; /* [3] */
    init_texPttrnAnm(a_anm_prm_tbl[mStatus].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[mStatus]);
}

/* 02301640 */
void daNpc_Zk1_c::setAnm_ATR() {

    anm_prm_c* a_anm_prm_tbl = a_anm_attr_tbl; /* [4] */
    init_texPttrnAnm(a_anm_prm_tbl[mAnmAtr].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[mAnmAtr]);
}

/* 023016A8 */
void daNpc_Zk1_c::chngAnmAtr(u32 i_atr) {

    if (i_atr == mAnmAtr || i_atr >= 4) {
        return;
    }
    mAnmAtr = (u8)i_atr;
    setAnm_ATR();
}

/* 023016C4 */
void daNpc_Zk1_c::anmAtr(u32 i_msgStatus) {

    if (i_msgStatus == ZK1_MSG_TYPING) {
        if (mMessageAnimationStarted == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mMessageAnimationStarted = mMessageAnimationStarted + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        if (tag != 0xFF && tag != mMesgAnimeTagInfo) {
            dComIfGp_clearMesgAnimeTagInfo(); /* dComIfGp_clearMesgAnimeTagInfo() */
            mMesgAnimeTagInfo = tag;
            /* chngAnmTag(): empty */
        }
    } else if (i_msgStatus == ZK1_MSG_DISPLAYED) {
        mMessageAnimationStarted = 0;
    }
    /* ctrlAnmAtr(), ctrlAnmTag(): empty */
}

/* 02301794 */
u16 daNpc_Zk1_c::next_msgStatus(be<u32>* pMsgNo) {

    u16 ret = fopMsgStts_MSG_CONTINUES_e;
    switch ((u32)*pMsgNo) {
    case 0x17A2:
        *pMsgNo = 0x17A3;
        break;
    case 0x17A3:
        *pMsgNo = 0x17A4;
        break;
    case 0x17A4:
        *pMsgNo = 0x17A5;
        break;
    case 0x17A5:
        *pMsgNo = 0x17A6;
        break;
    case 0x17A8:
        *pMsgNo = 0x17A9;
        break;
    case 0x17A9:
        *pMsgNo = 0x17AA;
        break;
    case 0x17AA:
        *pMsgNo = 0x17AB;
        break;
    case 0x17AD:
        if (dKy_daynight_check()) {
            *pMsgNo = 0x17AF;
        } else {
            *pMsgNo = 0x17AE;
        }
        break;
    default:
        ret = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return ret;
}

/* 023018C0 */
u32 daNpc_Zk1_c::getMsg_ZK1_0() {

    if (!dComIfGs_isEventBit(0x1A80)) {
        return dComIfGs_isEventBit(0x1802) ? 0x17A7 : 0x17A2;
    }
    if (mFirstConversationDone == 0) {
        return dComIfGs_isEventBit(0x1802) ? 0x17AC : 0x17A8;
    }
    if (mSecondConversationDone == 0) {
        return dComIfGs_isEventBit(0x1C01) ? 0x17B0 : 0x17AD;
    }
    return dComIfGs_isEventBit(0x1D80) ? 0x17B2 : 0x17B1;
}

/* 023019D8 */
u32 daNpc_Zk1_c::getMsg() {

    u32 ret = 0;
    if (mSpecificType == 0) {
        ret = getMsg_ZK1_0();
    }
    return ret;
}

/* 02301A10 */
bool daNpc_Zk1_c::chk_talk() {

    if (dComIfGp_event_chkTalkXY()) {
        if (!dComIfGp_evmng_ChkPresentEnd()) {
            return false;
        }
        mItemNo = dComIfGp_event_getPreItemNo();
        return true;
    }
    mItemNo = 0xFF;
    return true;
}

/* 02301AA8 */
u8 daNpc_Zk1_c::chk_parts_notMov() {

    // HD: true when a part moved
    return !(mJointHeadY == m_jnt.mAngles[0][1] && mJointBackboneY == m_jnt.mAngles[1][1] &&
             mActorAngleY == current.angle.y);
}

/* 02301AE8 */
u8 daNpc_Zk1_c::chkAttention() {

    dAttention_c* attention = dComIfGp_getAttention();
    if (Zk1_lockonTruth(attention)) {
        return this == (void*)Zk1_lockonTarget(attention, 0);
    }
    return this == (void*)Zk1_actionTarget(attention, 0);
}

/* 02301B70 */
void daNpc_Zk1_c::setStt(u32 i_status) {

    s8 old = mStatus;
    mStatus = (s8)i_status;
    switch (i_status) {
    case 1:
        mEvtCond = 0;
        break;
    case 2:
        mEvtCond = 0;
        mMesgAnimeTagInfo = 0xFF;
        mAnmAtr = 0xFF;
        mPrevStatus = old;
        mMessageAnimationStarted = 0;
        break;
    }
    setAnm();
}

/* 02301BBC */
BOOL daNpc_Zk1_c::wait_1() {

    s16 diff = 0;
    if (mReturnToInitialAngle != 0) {
        cLib_addCalcAngleS(&current.angle.y, mInitAngle.y, 4, 0x800, 0x80);
        diff = (s16)(mInitAngle.y - current.angle.y);
    }
    if (mTalkOrdered != 0) {
        if (chk_talk()) {
            setStt(ZK1_TALK);
            mHeadOnlyFollow = 0;
            mLookBackState = 1;
            mReturnToInitialAngle = 0;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    } else {
        mLookBackState = 0;
        mEvtCond = 2;
        mHeadOnlyFollow = 1;
        if (diff == 0) {
            if (mbAttention != 0) {
                mTimer = (s16)Zk1_getRndValue(15, 30);
            }
            if (Zk1_calcTimer(&mTimer) != 0) {
                mLookBackState = 1;
            }
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            gabi::Local<cXyz> pos; /* passed by value: a copy */
            pos->x = player->current.pos.x;
            pos->y = player->current.pos.y;
            pos->z = player->current.pos.z;
            if (!Zk1_checkAttention(this, pos, 200.0f, 50.0f, 76.0f, mLookBackState == 1)) {
                mLookBackState = 0;
                mReturnToInitialAngle = 1;
            }
        }
    }
    return TRUE;
}

/* 02301D34 */
u32 daNpc_Zk1_c::talk_1() {

    u32 ret = chk_parts_notMov();
    talk(1);
    // HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus)
    if (mbHasMsg != 0 && Zk1_messageStatus(l_message_manager) == ZK1_MSG_DESTROYED) {
        u32 msgNo = mCurrMsgNo;
        if (msgNo == 0x17A6 || msgNo == 0x17AB) {
            dComIfGs_onEventBit(0x1802);
        } else if (msgNo < 0x17AE) {
        } else if (msgNo <= 0x17AF) {
            dComIfGs_onEventBit(0x1C01);
        } else if (msgNo == 0x17B1) {
            dComIfGs_onEventBit(0x1D80);
        }
        mItemNo = 0xFF;
        mTalkOrdered = 0;
        setStt(mPrevStatus);
        mTimer = (s16)Zk1_getRndValue(15, 30);
        endEvent();
    }
    return ret;
}

/* 02301E80 */
BOOL daNpc_Zk1_c::wait_action1(void*) {

    s8 state = mActState;
    if (state == 0) {
        setStt(ZK1_WAIT);
        mReturnToInitialAngle = 1;
        mActState = mActState + 1;
        return TRUE;
    }
    if ((u32)(s32)state > 3) {
        return TRUE;
    }
    mbAttention = chkAttention();
    switch ((u32)(s32)mStatus) {
    case 1:
        mbSetEyePos = wait_1();
        break;
    case 2:
        mbSetEyePos = talk_1();
        break;
    }
    return TRUE;
}

/* 02301F38 */
static daNpc_Zk1_HIO_c* daNpc_Zk1_HIO_c_ct(daNpc_Zk1_HIO_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpc_Zk1_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1002399C;
    memcpy_g(&i_this->mPrmTbl, &a_prm_tbl, 0x1C); /* a_prm_tbl; 028FEAC0 memcpy */
    i_this->mNo = -1;
    i_this->field_0x8 = -1;
    return i_this;
}

// HD: compiler-generated initialization and deleting destructors.
/* 02301FA4 */
static void __sinit_d_a_npc_zk1_cpp() {

    sinit_header_statics_z(0x10468AC0, 0x101C706C, 0x10468AF4);
    daNpc_Zk1_HIO_c_ct(&l_HIO); /* static daNpc_Zk1_HIO_c l_HIO */
}

/* 02302044 */
static void SafeString_dt(SafeString* i_this, s32 flags) {

    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 02302058 */
static void daNpc_Zk1_c_dt(daNpc_Zk1_c* i_this, s32 flags) {

    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Zk1_circleDestroy(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        Zk1_view<Zk1AcchVtables>(&i_this->mObjAcch)->mPolyVtable = 0x1002397C;
        Zk1_view<Zk1AcchVtables>(&i_this->mObjAcch)->mCheckVtable = 0x1002398C;
        Zk1_acchDestroy(&i_this->mObjAcch, 0);
        Zk1_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 023020F4 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue
str 100239DC "d_a_npc_zk1.cpp" in 022FFCE4
str 100239EC "a_btp != (0)" in 022FFCE4
str 10023A04 "d_a_npc_zk1.cpp" in 022FFDD4
str 10023A14 "a_mdl_dat != (0)" in 022FFDD4
str 100239FC "head" in 022FFDD4
str 10023A28 "m_hed_jnt_num >= 0" in 022FFDD4
str 10023A3C "backbone" in 022FFDD4
str 10023A48 "m_bbone_jnt_num >= 0" in 022FFDD4
str 10023A6C "Zk1" in 02300628
str 100239BC "d_demo.h" in 02300A3C
str 100239AC "m_object != (0)" in 02300A3C
ext 024EF130 s8 Zk1_roomId(dBgS*, cBgS_PolyInfo*)
ext 024EEEB8 u8 Zk1_polyColour(dBgS*, cBgS_PolyInfo*)
ext 02055B64 s16 Zk1_calcTimer(be<s16>* t)
ext 025E789C s32 Zk1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 024EDFCC bool Zk1_lockonTruth(dAttention_c* a)
ext 024EC8D0 fopAc_ac_c* Zk1_lockonTarget(dAttention_c* a, s32 i)
ext 024EE464 fopAc_ac_c* Zk1_actionTarget(dAttention_c* a, s32 i)
ext 0259E2D4 void Zk1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 02527028 BOOL Zk1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7)
ext 02526E70 void* Zk1_demoGetActor(u32 obj, u8 id)
ext 025A1458 void Zk1_npcCtor(daNpc_Zk1_c* p)
ext 025E7820 void Zk1_btpCtor(void* p)
ext 02018034 void Zk1_circleDestroy(void* p, s32 flags)
ext 024EFD9C void Zk1_acchDestroy(dBgS_Acch* p, s32 flags)
ext 025D50BC void Zk1_actorDestroy(daNpc_Zk1_c* p, s32 flags)
ext 02527828 J3DAnmTexPattern* Zk1_demoBtp(void*,const char*)
ext 021E1E78 s32 Zk1_getRndValue(s32,s32)
ext 0259D8BC BOOL Zk1_checkAttention(fopAc_ac_c*,cXyz*,f32,f32,f32,u32)
ext 024EECAC u32 Zk1_materialSound(dBgS*,cBgS_PolyInfo*)
ext 025D54C4 BOOL Zk1_searchById(u32,be<u32>*)
ext 022FFCE4 u32 Zk1_setBtpResult(daNpc_Zk1_c*,s32,u32)
ext 025E535C BOOL Zk1_playMorf(mDoExt_McaMorf*,cXyz*,u32,s32)
ext 025F795C u32 Zk1_messageStatus(void*)
*/
