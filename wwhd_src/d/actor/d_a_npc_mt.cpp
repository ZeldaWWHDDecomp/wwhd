/* Carlov, Nintendo Gallery. Reconstructed from the WWHD game. */
#include "d/actor/d_a_npc_mt.h"

// HD: GHS emits these inline-destructor vtables for this translation unit.
const gc_tu_data gc_tu = {.safeString = 0x1001E998, .objAcchDt = {0x1001E9B0, 0x1001E9C0}};

static MtMessageTable2 l_msg_mt_3rd_talk = {{0x3668, 0x0000}};
static MtMessageTable2 l_msg_mt_4th_talk = {{0x3669, 0x0000}};
static MtMessageTable2 l_msg_mt_queue_full_HD = {{0x3654, 0x0000}};
static MtMessageTable2 l_msg_mt_making = {{0x3675, 0x0000}};
static MtMessageTable2 l_msg_mt_gallery = {{0x3651, 0x0000}};
static MtMessageTable2 l_msg_mt_room_in = {{0x3652, 0x0000}};
static MtMessageTable2 l_msg_mt_kansei_end = {{0x3653, 0x0000}};
static MtMessageTable2 l_msg_mt_no_color = {{0x3655, 0x0000}};
static MtMessageTable2 l_msg_mt_no_figure = {{0x3656, 0x0000}};
static MtMessageTable2 l_msg_mt_maked = {{0x3658, 0x0000}};
static MtMessageTable2 l_msg_mt_already_queued_HD = {{0x3677, 0x0000}};
static MtMessageTable2 l_msg_mt_face_ng = {{0x3659, 0x0000}};
static MtMessageTable2 l_msg_mt_body_ng = {{0x365A, 0x0000}};
static MtMessageTable8 l_msg_mt_1st_talk = {{0x365E, 0x365F, 0x3660, 0x3661, 0x3662, 0x3663, 0x3664, 0x0000}};
static MtMessageTable4 l_msg_mt_2nd_talk = {{0x3665, 0x3666, 0x3667, 0x0000}};
static MtMessageTable8 l_msg_mt_room = {{0x366B, 0x366B, 0x366C, 0x366D, 0x366E, 0x366F, 0x3670, 0x3671}};
static MtMessageTable4 l_msg_mt_make_1st = {{0x365B, 0x3674, 0x365C, 0x0002}};
static MtMessageTable4 l_msg_mt_make_2nd = {{0x365B, 0x3674, 0x365D, 0x0002}};
static MtMessageTable4 l_msg_mt_make_1st_mt = {{0x3672, 0x3674, 0x365C, 0x0002}};
static MtMessageTable4 l_msg_mt_make_2nd_mt = {{0x3672, 0x3674, 0x365D, 0x0002}};
static MtMessageTable4 l_msg_mt_make_1st_mn = {{0x3673, 0x3674, 0x365C, 0x0002}};
static MtMessageTable4 l_msg_mt_make_2nd_mn = {{0x3673, 0x3674, 0x365D, 0x0002}};
static MtMessageTable5 l_msg_mt_1st_kansei = {{0x364D, 0x364E, 0x3650, 0x0001, 0x0000}};
static MtMessageTable5 l_msg_mt_2nd_kansei = {{0x364D, 0x364F, 0x3650, 0x0001, 0x0000}};

static MtMessageChoices l_msg_mt_make;
static MtMessageChoices l_msg_mt_make_mt;
static MtMessageChoices l_msg_mt_make_mn;

static MtFigureRoomNames l_figure_room_name;
static MtFigureCompletionTable l_figure_comp;
static sMtAnmDat l_npc_anm_wait;
static MtNpcDat l_npc_dat;

static MtModelResourceTable l_bmd_ix_tbl;
static MtPatternResourceTable l_btp_ix_tbl;
static MtAnimationResourceTable l_bck_ix_tbl;

static MtMessageAnimationTable l_msg_anm_tbl;

static MtInitFunctionTable l_execute_init;
static MtMoveFunctionTable moveProc;

static MtPhaseTable l_method;
static char* l_arcname_tbl;
static char* l_my_name;

static MtCutActionTable l_cut_action;
static dCcD_SrcCyl l_cyl_src;
static MtPhotoSaveHD* l_photoSave;
static MtMessageManagerHD* l_messageManager;
// HD: this TU's header statics retain the original initialization order.

static MtHeaderStaticsHD l_headerStatics;
static MtHeaderPiHD l_headerPi;
static MtDestructorRecordsHD l_destructorRecords;
static MtPhotoQueueHD* photoBuffer() { return MtPhotoAlbum_getQueue(&l_photoSave->mAlbum); }

static void* objectResource(u32 resource) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::ea(l_arcname_tbl);
    name->__vtbl = gc_tu.safeString;
    return Mt_getObjectResHD(dComIfG_resControl(), name.get(), resource);
}

static void jointParameters(dNpc_JntCtrl_c* joint) {
    s16 params[9] = {l_npc_dat.mMax_head_x,     l_npc_dat.mMax_head_y,     l_npc_dat.mMax_backbone_x,
                     l_npc_dat.mMax_backbone_y, l_npc_dat.mMin_head_x,     l_npc_dat.mMin_head_y,
                     l_npc_dat.mMin_backbone_x, l_npc_dat.mMin_backbone_y, l_npc_dat.mMax_turn_step};
    joint->setParam(params[2], params[3], params[6], params[7], params[0], params[1], params[4], params[5],
                    params[8]);
}

static bool sameStage(SafeString* first, SafeString* second) {
    Mt_assureTermination(first);
    Mt_assureTermination(first);
    u32 string = first->mStringTop.get();
    Mt_assureTermination(second);
    if (string == second->mStringTop.get())
        return true;
    string = first->mStringTop.get();
    u32 other = second->mStringTop.get();
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 c = gabi::at<be<u8>>(string)[i];
        if (c != static_cast<u8>(gabi::at<be<u8>>(other)[i]))
            return false;
        if (!c)
            return true;
    }
    return false;
}

static void faceHome(daNpcMt_c* a) {
    a->m728 = 0;
    a->m740 = static_cast<s16>(a->m732);
    a->m74F = 2;
    a->m_jnt.mbTrn = 1;
}
static void playerGaze(daNpcMt_c* a, cXyz* eye) {
    u8 turn = static_cast<u8>(a->m750), look = static_cast<u8>(a->m751);
    u32 x = Mt_floatWord(eye->x), y = Mt_floatWord(eye->y), z = Mt_floatWord(eye->z);
    a->m728 = turn == 0;
    Mt_storeFloatWord(a->mLookAtPos.z, z);
    Mt_storeFloatWord(a->mLookAtPos.y, y);
    a->m74F = 1;
    Mt_storeFloatWord(a->mLookAtPos.x, x);
    if (!look)
        faceHome(a);
}

/* 0229FD7C */
daNpcMt_c* daNpcMt_construct(daNpcMt_c* a) {

    if (!a)
        a = static_cast<daNpcMt_c*>(operator_new(sizeof(daNpcMt_c)));
    if (!a)
        return a;
    __ct(static_cast<fopNpc_npc_c*>(a));
    a->__vtbl = MT_ACTOR_VTABLE_HD;
    mDoExt_btpAnm::ct(&a->mBtpAnm);
    a->mNpcNo = a->getPrmNpcNo();
    a->m732 = static_cast<s16>(a->home.angle.y);
    a->m71C = -1.0f;
    a->m747 = 0;
    a->m718 = 0.0f;
    a->m74A = 0;
    a->m736 = 0;
    a->m728 = 1;
    a->m72C = 0;
    a->m74F = 0;
    a->m746 = 0;
    return a;
}

/* 0229F85C */
BOOL daNpc_Mt_nodeCallBack(void* node, s32 phase) {
    if (phase)
        return TRUE;
    J3DModel* model = j3dSys.getModel();
    daNpcMt_c* actor = gabi::at<daNpcMt_c>(model->mUserArea);
    u16 joint = reinterpret_cast<J3DJoint*>(node)->getJntNo();
    MTXCopy(model->getAnmMtx(joint), calc_mtx());
    int part = -1;
    if (joint == static_cast<u32>(static_cast<s8>(actor->m_jnt.mHeadJntNum)))
        part = 0;
    else if (joint == static_cast<u32>(static_cast<s8>(actor->m_jnt.mBackboneJntNum)))
        part = 1;
    if (part >= 0) {
        mDoMtx_XrotM(calc_mtx(), actor->m_jnt.mAngles[part][1]);
        mDoMtx_ZrotM(calc_mtx(), static_cast<s16>(-actor->m_jnt.mAngles[part][0]));
    }
    // HD: this copy fetches the matrix pointer before marking the buffer dirty.
    J3DMtxBuffer_hd* matrices = model->mpMtxBuffer;
    Mtx34* source = calc_mtx();
    Mtx34* destination = gabi::at<Mtx34>(matrices->mpAnmMtx + joint * sizeof(Mtx34));
    matrices->mFlags |= 0x10;
    J3DModel::mtx_assign(destination, source);
    MTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    return TRUE;
}

/* 0229FD60 */
BOOL CheckCreateHeap(daNpcMt_c* a) { return a->createHeap(); }

/* 022A0018 */
s32 phase_1(daNpcMt_c* a) {
    u32 flags = static_cast<u32>(a->actor_condition);
    if (!(flags & 8)) {
        if (a) {
            daNpcMt_construct(a);
            flags = static_cast<u32>(a->actor_condition);
        }
        a->actor_condition = flags | 8;
    }
    if (dComIfGs_isEventBit(0x3D08)) {
        a->setFigure(0x40);
        return 3;
    }
    if (static_cast<s16>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mCurStage.mPoint) == 0) {
        BOOL complete = a->isComp();
        dSv_info_c* save = &l_photoSave->mSaveInfo;
        if (complete) {
            save->mSavedata.mEvent.onEventBit(0x3D08);
            a->setFigure(0x40);
            return 3;
        }
        Mt_offEventBitHD(&save->mTmp, 0x202);
        if (dComIfGs_isEventBit(0x4080)) {
            for (u32 event : {0x3080u, 0x2F01u, 0x4080u, 0x3F01u, 0x4040u})
                dComIfGs_offEventBit(event);
            a->clearPhotoSelection();
        }
    }
    a->m747 = 1;
    return 2;
}

/* 022A07F8 */
s32 phase_2(daNpcMt_c* a) {

    s32 phase = static_cast<s32>(dComIfG_resLoad(&a->mPhsArcname, l_arcname_tbl));
    if (phase == 4) {
        if (fopAcM_entrySolidHeap(a, reinterpret_cast<BOOL (*)(fopAc_ac_c*)>(CheckCreateHeap), 0x2900))
            return a->createInit();
        a->mpMorf.v = 0;
        return 5;
    }
    return phase;
}

/* 022A0880 */
s32 daNpcMt_c::_create() { return Mt_phaseHandlerHD(&mPhs, l_method.function, this); }

/* 0229FABC */
BOOL daNpcMt_c::createHeap() {
    J3DModelData* data = static_cast<J3DModelData*>(objectResource(l_bmd_ix_tbl.index[0]));
    J3DAnmTransform* animation =
        static_cast<J3DAnmTransform*>(objectResource(l_bck_ix_tbl.index[static_cast<u8>(m74A)]));
    mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, animation, 2, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11020022);
    for (u32 part = 0; part < 2; ++part) {
        s8 joint = static_cast<s8>(data->getJointName()->getIndex(part ? "backbone" : "head"));
        if (part)
            m_jnt.mBackboneJntNum = joint;
        else
            m_jnt.mHeadJntNum = joint;
        if (joint < 0) {
            Mt_assertHD("d_a_npc_mt.cpp", part ? 0x3B8 : 0x3B4,
                        part ? "m_jnt.getBackboneJntNum() >= 0" : "m_jnt.getHeadJntNum() >= 0");
        }
    }
    if (!initTexPatternAnm(0))
        return FALSE;
    u16 i = 0, count = data->getJointNum();
    while (i < count) {
        if (static_cast<u32>(i) == static_cast<u32>(static_cast<s8>(m_jnt.mHeadJntNum)) ||
            static_cast<u32>(i) == static_cast<u32>(static_cast<s8>(m_jnt.mBackboneJntNum))) {
            // HD: the joint pointer is loaded before the count.
            u32 entry = data->mJointNodes;
            if (i < static_cast<u32>(data->mJointNum))
                entry += i * 0x1C;
            gabi::at<J3DJoint>(entry)->setCallBack(reinterpret_cast<J3DJointCallBack>(daNpc_Mt_nodeCallBack));
        }
        ++i;
        count = data->getJointNum();
    }
    mDoExt_McaMorf* morph = mpMorf;
    if (morph->getModel())
        morph->getModel()->mUserArea = gabi::ea(this);
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}

/* 022A04A4 */
s16 daNpcMt_XyCheckCB(daNpcMt_c* a, s32 value) { return a->XyCheckCB(value); }

/* 022A0618 */
s32 daNpcMt_c::createInit() {
    this->gravity = -9.0f;
    setAnmTbl(&l_npc_anm_wait);
    s16 event = dComIfGp_evmng_getEventIdx("MT_GET_ITEM", 0xFF);
    this->mMtGetItemEventIdx = event;
    eventInfo.setXyCheckCB(reinterpret_cast<dEvt_info_c::CallbackFunc>(daNpcMt_XyCheckCB));
    mEventCut.setActorInfo2(l_my_name, this);
    mDoExt_McaMorf* morph = mpMorf;
    m742 = 0;
    m752 = 0;
    m753 = 0;
    m743 = 0;
    this->m73E = 0;
    J3DModel* model = morph->getModel();
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    this->attention_info.distances[1] = 0x6E;
    this->attention_info.distances[3] = 0x6E;
    this->attention_info.flags = 0x0100000A;
    jointParameters(&m_jnt);
    this->m750 = static_cast<u8>(l_npc_dat.field_0x4A);
    this->m751 = static_cast<u8>(l_npc_dat.field_0x4B);
    this->m720 = static_cast<f32>(l_npc_dat.field_0x20);
    this->m730 = static_cast<s16>(l_npc_dat.field_0x28);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    setMtx();
    mpMorf->getModel()->calc();
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    gabi::Local<cXyz> center;
    center->x = static_cast<f32>(this->current.pos.x);
    center->z = static_cast<f32>(this->current.pos.z);
    mCyl.mStts = &mStts;
    center->y = static_cast<f32>(this->current.pos.y);
    setCollision(&mCyl, center, static_cast<f32>(l_npc_dat.field_0x30), 150.0f);
    return 4;
}

// HD: next-stage names use SafeString virtual termination checks.
/* 022A08D8 */
BOOL daNpcMt_c::_delete() {
    dComIfG_resDelete(&mPhsArcname, l_arcname_tbl);
    if (heap.v != 0) {
        mDoExt_McaMorf* morph = mpMorf;
        if (morph)
            morph->stopZelAnime();
    }
    if (!static_cast<s8>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mNextStage.enable))
        return TRUE;
    for (u32 room = 1; room < 8; ++room) {
        gabi::Local<SafeString> desired, next;
        desired->mStringTop = gabi::ea(l_figure_room_name.name[room].get());
        desired->__vtbl = gc_tu.safeString;
        next->mStringTop = gabi::ea(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mNextStage.mName);
        next->__vtbl = gc_tu.safeString;
        if (sameStage(desired, next)) {
            if (!dComIfGs_isEventBit(0x4080))
                return TRUE;
            u8 i = 0;
            while (i < getPhotoSelection()) {
                if (dSnap_GetFigRoomIdHD(getPhoto(i)) == room)
                    dComIfGs_onEventBit(0x3F01);
                ++i;
            }
            return TRUE;
        }
    }
    dComIfGs_offTmpBit(0x202);
    if (dComIfGs_isEventBit(0x4080)) {
        for (u32 event : {0x3080u, 0x2F01u, 0x4080u, 0x3F01u, 0x4040u})
            dComIfGs_offEventBit(event);
        clearPhotoSelection();
    }
    return TRUE;
}

// HD: the GC shadow pass is absent.
/* 022A1BA0 */
BOOL daNpcMt_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    J3DModelData* data = model->getModelData();
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    settingTevStruct(environment, TEV_TYPE_ACTOR, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, model, &tevStr);
    mBtpAnm.entry(data, static_cast<u8>(mFrame));
    mpMorf->updateDL();
    reinterpret_cast<MtModelDataAnimationHD*>(data)->mpTexNoAnimator = 0;
    dSnap_RegistFig(0xA7, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 022A1914 */
BOOL daNpcMt_c::_execute() {
    chkAttention();
    checkOrder();
    if (dComIfGp_event_runCheck() && eventInfo.mCommand != dEvtCmd_INTALK_e)
        eventMove();
    else {
        (this->*moveProc.function[static_cast<u8>(m746)])();
    }
    eventOrder();
    playTexPatternAnm();
    playAnm();
    gabi::Local<cXyz> center;
    center->x = static_cast<f32>(this->current.pos.x);
    center->y = static_cast<f32>(this->current.pos.y);
    center->z = static_cast<f32>(this->current.pos.z);
    setCollision(&mCyl, center, static_cast<f32>(l_npc_dat.field_0x30), 150.0f);
    f32 x = static_cast<f32>(this->current.pos.x), y = static_cast<f32>(this->current.pos.y),
        z = static_cast<f32>(this->current.pos.z);
    f32 offset = static_cast<f32>(l_npc_dat.field_0x18);
    this->attention_info.position.z = z;
    this->attention_info.position.x = x;
    this->attention_info.position.y = y + offset;
    offset = static_cast<f32>(l_npc_dat.field_0x1C);
    this->eyePos.z = z;
    this->eyePos.x = x;
    this->eyePos.y = y + offset;
    lookBack();
    setMtx();
    return FALSE;
}

/* 022A1C4C */
u32 daNpcMt_c::executeCommon() {

    this->m744 = static_cast<u8>(this->m743) != 0;
    u8 result = static_cast<u8>(this->m742);
    if (result == 1 && static_cast<u8>(this->m746) != 1) {
        executeSetMode(1);
        result = static_cast<u8>(this->m742);
    }
    return result;
}

/* 022A0DFC */
void daNpcMt_c::executeSetMode(u8 mode) {
    m718 = 0.0f;
    m746 = (this->*l_execute_init.function[mode])();
}

/* 022A1CAC */
s32 daNpcMt_c::executeWaitInit() {

    this->speedF = 0.0f;
    setAnmTbl(&l_npc_anm_wait);
    s16 params[9] = {l_npc_dat.mMax_head_x,     l_npc_dat.mMax_head_y,     l_npc_dat.mMax_backbone_x,
                     l_npc_dat.mMax_backbone_y, l_npc_dat.mMin_head_x,     l_npc_dat.mMin_head_y,
                     l_npc_dat.mMin_backbone_x, l_npc_dat.mMin_backbone_y, l_npc_dat.mMax_turn_step};
    m_jnt.setParam(params[2], params[3], params[6], params[7], params[0], params[1], params[4], params[5],
                   params[8]);
    return 0;
}

/* 022A1D2C */
void daNpcMt_c::executeWait() {
    executeCommon();
    if (static_cast<u8>(this->m74A))
        setAnmTbl(&l_npc_anm_wait);
}

/* 022A1D74 */
s32 daNpcMt_c::executeTalkInit() { return 1; }

/* 022A1D7C */
void daNpcMt_c::executeTalk() {
    executeCommon();
    if (talk2(1) == 0x12) {
        this->m742 = 0;
        executeSetMode(0);
        dComIfGp_event_reset();
    } else
        setAnmFromMsgTag();
}

/* 022A0E84 */
void daNpcMt_c::checkOrder() {
    u16 status = eventInfo.mCommand;
    if (status == 2) {
        s16 event = static_cast<s16>(this->mMtGetItemEventIdx);
        if (dComIfGp_evmng_startCheck(event) && static_cast<u8>(this->m744) == 3)
            this->m744 = 0;
    } else if (status == 1 && (static_cast<u8>(this->m744) == 1 || static_cast<u8>(this->m744) == 2)) {
        this->m742 = 1;
        executeSetMode(1);
    }
}

/* 022A1500 */
void daNpcMt_c::eventOrder() {
    u8 mode = static_cast<u8>(this->m744);
    if (mode == 1 || mode == 2) {
        u16 flags = eventInfo.mCondition;
        mode = m744;
        eventInfo.mCondition = flags | 0x21;
        if (mode == 2)
            fopAcM_orderSpeakEvent(this);
    } else if (mode == 3) {
        fopAc_ac_c* link = dComIfGp_getPlayer(0);
        fopAcM_orderChangeEventId(link, this, mMtGetItemEventIdx, 0, 0xFFFF);
    }
}

/* 022A1478 */
void daNpcMt_c::eventMove() {

    if (chkEndEvent())
        return;
    u8 saved = static_cast<u8>(this->mEventCut.mbAttention);
    if (mEventCut.cutProc()) {
        if (!static_cast<u8>(this->mEventCut.mbAttention))
            this->mEventCut.mbAttention = saved;
    } else {
        privateCut();
        setAnmFromMsgTag();
    }
}

/* 022A12FC */
void daNpcMt_c::privateCut() {
    s32 staff = dComIfGp_evmng_getMyStaffId(l_my_name, nullptr, 0);
    if (staff == -1)
        return;
    s8 action = static_cast<s8>(dComIfGp_evmng_getMyActIdx(staff, l_cut_action.action, 2, 1, 0));
    this->m74D = action;
    dEvent_manager_c* event = dComIfGp_getPEvtManager();
    if (action == -1) {
        Mt_cutEndHD(event, staff);
        return;
    }
    if (Mt_getIsAddvanceHD(event, staff)) {
        action = static_cast<s8>(this->m74D);
        if (action == 0)
            eventMesSetInit(staff);
        else if (action == 1)
            eventGetItemInit();
        else {
            dComIfGp_evmng_cutEnd(staff);
            return;
        }
    }
    action = static_cast<s8>(this->m74D);
    if (action != 0 || eventMesSet())
        dComIfGp_evmng_cutEnd(staff);
}

/* 022A0FA8 */
void daNpcMt_c::eventMesSetInit(s32 staff) {

    be<u32>* substance = static_cast<be<u32>*>(dComIfGp_evmng_getMySubstanceP(staff, "MsgNo", 3));
    if (substance) {
        this->mpMsgTbl.v = 0;
        u32 message = *substance;
        if (message == 1)
            return;
        if (message == 0)
            message = dispatchGetMsg();
        setMessage(message);
        be<u32>* table = mpMsgTbl;
        if (table)
            setMessage(*table);
    } else {
        u32 table = static_cast<u32>(this->mpMsgTbl.v) + 4;
        this->mpMsgTbl.v = table;
        setMessage(*mpMsgTbl);
    }
}

/* 022A1270 */
BOOL daNpcMt_c::eventMesSet() {

    u32 result = talk2(0);
    if (result == 0x12) {
        u8 flags = static_cast<u8>(this->m749);
        if (flags & 1) {
            this->m749 = flags & 0xFE;
            this->mItemNo = 7;
            this->m744 = 3;
        } else if (flags & 2) {
            this->m749 = flags & 0xFD;
            this->mItemNo = 5;
            this->m744 = 3;
        }
    }
    return result == 0x12;
}

/* 022A1064 */
void daNpcMt_c::eventGetItemInit() {

    u32 item = Mt_createItemForPresentDemoHD(&current.pos, mItemNo, 0, -1, -1, nullptr, nullptr);
    if (item != 0xFFFFFFFF)
        dComIfGp_event_setItemPartnerId(item);
}

// HD: message status and creation use the expanded message manager.
/* 022A10BC */
u16 daNpcMt_c::talk2(s32 init) {
    s32 message = static_cast<s32>(this->mCurrMsgBsPcId);
    MtMessageManagerHD* actor = l_messageManager;
    u16 status = 0xFF;
    if (message == -1) {
        u32 number;
        if (init == 1) {
            number = dispatchGetMsg();
            this->mCurrMsgNo = number;
        } else
            number = static_cast<u32>(this->mCurrMsgNo);
        s32 id = actor->messageSet(number, &eyePos);
        this->mCurrMsgBsPcId = id;
        if (id != -1) {
            this->mbHasMsg = 0;
            this->m734 = 0xFFFF;
        }
    } else if (!static_cast<u8>(this->mbHasMsg))
        this->mbHasMsg = 1;
    else {
        status = actor->getStatus16();
        if (status == 0xE) {
            u32 next = dispatchNextMsgStatus();
            actor->setStatus(next);
            if (actor->getStatus() == 0xF)
                actor->messageSet(mCurrMsgNo, nullptr);
        } else if (status == 0x12) {
            actor->setStatus(0x13);
            this->mCurrMsgBsPcId = -1;
        }
        this->m734 = status;
        dispatchAnmAtr(status);
    }
    return status;
}

// HD: consume the selected album photo and describe every submitted figure room.
/* 022A1DF4 */
u16 daNpcMt_c::next_msgStatus(be<u32>* messageOut) {
    u32 table = static_cast<u32>(this->mpMsgTbl.v);
    if (!table)
        return 0x10;
    table += 4;
    this->mpMsgTbl.v = table;
    u32 message = *mpMsgTbl;
    if (message == 2) {
        u8 selected = Mt_photoResultHD()->mSelectedPhoto;
        MtPictureAlbumHD* album = MtPhotoAlbum_getPictures(&l_photoSave->mAlbum);
        MtPictureAlbum_erase(album, selected);
        dComIfG_play_c& play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        play.mItemPictureNumCount = static_cast<s16>(play.mItemPictureNumCount) - 1;
    }
    if (message == 0 || message == 2) {
        this->mpMsgTbl.v = 0;
        return 0x10;
    }
    if (message == 1) {
        if (getPhotoSelection() > 1)
            message = 0x3676;
        else {
            u32 room = dSnap_GetFigRoomIdHD(getPhoto(0));
            if (room >= 8)
                Mt_assertHD("d_a_npc_mt.cpp", 0x69E,
                            "roomID < (sizeof(l_msg_mt_room) / sizeof(l_msg_mt_room[0]))");
            message = static_cast<u32>(l_msg_mt_room.message[room]);
        }
    }
    *messageOut = message;
    return 0xF;
}

// HD: submitted photos are queued; the additional messages distinguish full and duplicate queues.
/* 022A2208 */
u32 daNpcMt_c::getMsg() {
    this->mpMsgTbl.v = 0;
    u8 showingPhoto = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mEvtCtrl.mbInPhoto;
    dComIfG_play_c* inventory = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
    auto table = [&](be<u32>* message) {
        mpMsgTbl = message;
        return message ? static_cast<u32>(*message) : 0;
    };
    auto event = [&](u32 bit) { return dComIfGs_isEventBit(bit) != 0; };
    auto on = [&](u32 bit) { dComIfGs_onEventBit(bit); };
    auto off = [&](u32 bit) { dComIfGs_offEventBit(bit); };
    if (showingPhoto) {
        u32 photo = daNpcMt_changePhotoNo(static_cast<u8>(inventory->mPictureResult)), figure = 0xFF;
        if (photo - 0x49 < 0x86)
            figure = dSnap_PhotoIndex2TableIndexHD(photo);
        if (!Mt_photoResultHD()->mColorValid)
            return table(l_msg_mt_no_color.message);
        if (figure == 0xFF)
            return table(l_msg_mt_no_figure.message);
        if (isFigureGet(figure))
            return table(l_msg_mt_maked.message);
        if (hasPhoto(figure))
            return table(l_msg_mt_already_queued_HD.message);
        if (static_cast<u8>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPictureResultDetail) & 1)
            return table(l_msg_mt_face_ng.message);
        if (static_cast<u8>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPictureResultDetail) & 2)
            return table(l_msg_mt_body_ng.message);
        if (event(0x4080)) {
            for (u32 bit : {0x3080u, 0x4080u, 0x3F01u, 0x4040u})
                off(bit);
            clearPhotoSelection();
        }
        submitPhoto(figure);
        u32 experienced = event(0x3A01) || event(0x3401);
        MtMessageChoices* choices = figure == 0x5E   ? &l_msg_mt_make_mt
                                    : figure == 0x5F ? &l_msg_mt_make_mn
                                                     : &l_msg_mt_make;
        mpMsgTbl = choices->table[experienced];
        on(0x2F01);
        on(0x3401);
    } else {
        u32 equipped = static_cast<u8>(inventory->mEvtCtrl.mTalkButton);
        if (equipped - 1 <= 3) {
            be<u32>* message = mpMsgTbl;
            return message ? static_cast<u32>(*message) : 0;
        }
        if (event(0x3080)) {
            if (!event(0x4080)) {
                mpMsgTbl = event(0x3A01) ? l_msg_mt_2nd_kansei.message : l_msg_mt_1st_kansei.message;
                makeFigures();
                on(0x4080);
                off(0x2F01);
            } else {
                if (!event(0x3F01))
                    return table(l_msg_mt_gallery.message);
                if (!event(0x4040)) {
                    on(0x4040);
                    return table(l_msg_mt_room_in.message);
                }
                mpMsgTbl = l_msg_mt_kansei_end.message;
                for (u32 bit : {0x3080u, 0x4080u, 0x3F01u, 0x4040u})
                    off(bit);
                dComIfGs_onTmpBit(0x202);
                clearPhotoSelection();
            }
        } else if (event(0x2F01))
            return table(getPhotoSelection() >= 0xC ? l_msg_mt_queue_full_HD.message
                                                    : l_msg_mt_making.message);
        else if (!event(0x2F02)) {
            on(0x2F02);
            return table(l_msg_mt_1st_talk.message);
        } else if (!event(0x3A01))
            return table(l_msg_mt_2nd_talk.message);
        else if (!dComIfGs_isTmpBit(0x202)) {
            dComIfGs_onTmpBit(0x202);
            return table(l_msg_mt_3rd_talk.message);
        } else
            return table(l_msg_mt_4th_talk.message);
    }
    be<u32>* selected = mpMsgTbl;
    return selected ? static_cast<u32>(*selected) : 0;
}

/* 022A0FA0 */
void daNpcMt_c::setMessage(u32 message) { this->mCurrMsgNo = message; }

/* 022A141C */
void daNpcMt_c::setAnmFromMsgTag() {

    u32 tag = dComIfGp_getMesgAnimeAttrInfo();
    if (tag <= 6)
        setAnmTbl(l_msg_anm_tbl.animation[tag]);
    dComIfGp_setMesgAnimeAttrInfo(0xFF);
}

/* 0229FD64 */
u8 daNpcMt_c::getPrmNpcNo() { return 0; }

/* 022A04A8 */
void daNpcMt_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    // HD: capture all three scale values before writing the model.
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    model = mpMorf->getModel();
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 022A0B20 */
void daNpcMt_c::chkAttention() {
    u8 event = static_cast<u8>(this->mEventCut.mbAttention);
    this->m752 = 0;
    bool close = false;
    if (event) {
        f32 z = static_cast<f32>(this->mEventCut.mPos.z), y = static_cast<f32>(this->mEventCut.mPos.y);
        u8 turn = static_cast<u8>(this->m750);
        this->mLookAtPos.z = z;
        this->m74F = 1;
        this->mLookAtPos.y = y;
        this->mLookAtPos.x = static_cast<f32>(this->mEventCut.mPos.x);
        if (turn) {
            this->m728 = 0;
            this->m_jnt.mbTrn = 1;
        } else
            this->m728 = 1;
        close = true;
    } else {
        fopAc_ac_c* player = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.getPlayerPtr(0);
        gabi::Local<cXyz> origin, target;
        gabi::Local<be<f32>> distance;
        gabi::Local<be<s16>> angle;
        origin->x = static_cast<f32>(this->current.pos.x);
        origin->y = static_cast<f32>(this->current.pos.y);
        origin->z = static_cast<f32>(this->current.pos.z);
        target->x = static_cast<f32>(player->current.pos.x);
        target->y = static_cast<f32>(player->current.pos.y);
        target->z = static_cast<f32>(player->current.pos.z);
        f32 radius = static_cast<f32>(this->m720);
        s32 cone = static_cast<s16>(this->m730);
        dNpc_calc_DisXZ_AngY(origin, target, distance, angle);
        u8 wasNear = static_cast<u8>(this->m743);
        s16 difference = (s16)(angle.get()->get() - static_cast<s16>(this->shape_angle.y));
        if (wasNear) {
            radius += 40.0f;
            cone += 0x71C;
        }
        *angle = difference;
        f32 separation = distance.get()->get();
        s32 absAngle = difference < 0 ? -(s32)difference : difference;
        if (radius > separation && cone > absAngle) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos(eye, l_npc_dat.field_0x14);
            playerGaze(this, eye);
            close = true;
        } else {
            if (wasNear == 1) {
                this->m743 = 0;
                this->m72E = static_cast<s16>(l_npc_dat.field_0x48);
            }
            if (static_cast<f32>(l_npc_dat.field_0x24) > separation || static_cast<u8>(this->m753)) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos(eye, l_npc_dat.field_0x14);
                playerGaze(this, eye);
                this->m752 = 1;
            } else {
                u32 message = static_cast<u32>(this->m6F0);
                this->m74F = 0;
                if (!message) {
                    s16 timer = static_cast<s16>(this->m72E);
                    if (timer)
                        this->m72E = (u16)(timer - 1);
                    else
                        faceHome(this);
                }
            }
        }
    }
    if (close && !static_cast<u8>(this->m743))
        this->m743 = 1;
    this->m73C = static_cast<s16>(l_npc_dat.field_0x2A);
}

/* 022A1708 */
void daNpcMt_c::lookBack() {

    s8 mode = static_cast<s8>(this->m74F);
    f32 x = static_cast<f32>(this->eyePos.x), y = static_cast<f32>(this->eyePos.y),
        z = static_cast<f32>(this->eyePos.z);
    s16 yaw = static_cast<s16>(this->current.angle.y), targetAngle = static_cast<s16>(this->m73C);
    u32 enabled = static_cast<u8>(this->m728);
    gabi::Local<cXyz> target, eye;
    cXyz* targetPtr = nullptr;
    if (mode == 1) {
        Mt_copyFloatWordsHD(target, &mLookAtPos);
        targetPtr = target;
    } else if (mode == 2)
        yaw = static_cast<s16>(this->m740);
    bool forced = static_cast<u8>(this->m742) && static_cast<u8>(this->m750);
    if (forced) {
        this->m_jnt.mbTrn = 1;
        enabled = 0;
    }
    s16 turn = 0;
    if (forced || static_cast<u8>(this->m_jnt.mbTrn)) {
        s16 eventAngle = static_cast<s16>(this->mEventCut.mTurnSpeed);
        if (eventAngle)
            targetAngle = eventAngle;
        cLib_addCalcAngleS2(&m73E, targetAngle, 4, 0x800);
        turn = static_cast<s16>(this->m73E);
    } else
        this->m73E = 0;
    eye->x = x;
    eye->y = y;
    eye->z = z;
    Mt_lookAtTargetHD(&m_jnt, &current.angle.y, targetPtr, eye, yaw, turn, enabled);
    u16 ax = static_cast<u16>(this->current.angle.x), ay = static_cast<u16>(this->current.angle.y),
        az = static_cast<u16>(this->current.angle.z);
    this->shape_angle.x = ax;
    this->shape_angle.z = az;
    this->shape_angle.y = ay;
}

/* 0229F9B8 */
BOOL daNpcMt_c::initTexPatternAnm(u32 modify) {

    J3DModelData* data = mpMorf->getModel()->getModelData();
    J3DAnmTexPattern* pattern = static_cast<J3DAnmTexPattern*>(objectResource(l_btp_ix_tbl.index[0]));
    m_head_tex_pattern = pattern;
    if (!pattern) {
        Mt_assertHD("d_a_npc_mt.cpp", 0x85E, "m_head_tex_pattern != (0)");
        pattern = m_head_tex_pattern;
    }
    if (!Mt_initBtpHD(&mBtpAnm, data, pattern, 1, 2, 1.0f, 0, -1, modify))
        return FALSE;
    this->mFrame = 0;
    this->mTimer = 0;
    return TRUE;
}

/* 022A1598 */
void daNpcMt_c::playTexPatternAnm() {

    if (cLib_calcTimer(&mTimer))
        return;
    J3DAnmTexPattern* animation = m_head_tex_pattern;
    s32 max = Mt_patternFrameMax(animation);
    if ((s32) static_cast<u8>(this->mFrame) >= max) {
        animation = m_head_tex_pattern;
        max = Mt_patternFrameMax(animation);
        u8 frame = static_cast<u8>(this->mFrame);
        this->mTimer = 120;
        this->mFrame = frame - (u32)max;
    } else
        this->mFrame = static_cast<u8>(this->mFrame) + 1;
}

/* 022A1638 */
void daNpcMt_c::playAnm() {

    u8 flags = static_cast<u8>(this->mAnmFlag);
    mDoExt_McaMorf* morph = mpMorf;
    this->mAnmFlag = flags & 0xFE;
    if (!morph->play(nullptr, 0, 0) || !static_cast<u32>(this->mpAnmDat.v))
        return;
    s8 count = static_cast<s8>(this->m74C);
    if (count <= 0)
        return;
    --count;
    this->m74C = count;
    if (count == 0) {
        u32 table = static_cast<u32>(this->mpAnmDat.v) + 3;
        this->mpAnmDat.v = table;
        if (setAnmTbl(mpAnmDat))
            this->mAnmFlag = static_cast<u8>(this->mAnmFlag) | 1;
    } else
        setAnm(mpAnmDat->field_0x00, 0, 0.0f);
}

/* 022A01A4 */
void daNpcMt_c::setAnm(u8 animation, s32 attr, f32 blend) {

    f32 override = static_cast<f32>(this->m71C);
    if (!(override < 0.0f)) {
        blend = override;
        this->m71C = -1.0f;
    }
    J3DAnmTransform* resource = static_cast<J3DAnmTransform*>(objectResource(l_bck_ix_tbl.index[animation]));
    mpMorf->setAnm(resource, attr, blend, 1.0f, 0.0f, -1.0f, nullptr);
    this->m74A = animation;
}

/* 022A02B0 */
BOOL daNpcMt_c::setAnmTbl(sMtAnmDat* table) {
    if (table->field_0x00 == 0xFF) {
        mpAnmDat.v = 0;
        return TRUE;
    }
    mpAnmDat = table;
    s8 count = table->field_0x02;
    m74C = count;
    u8 animation = table->field_0x00;
    if (count > 0 || static_cast<u8>(m74A) != animation) {
        setAnm(animation, count > 0 ? 0 : 2, static_cast<f32>(static_cast<u8>(table->field_0x01)));
    }
    return FALSE;
}

/* 022A03AC */
s16 daNpcMt_c::XyCheckCB(s32 index) {

    if (!dComIfGs_isEventBit(0x2F02))
        return 0;
    if (dComIfGs_isEventBit(0x2F01)) {
        if (dComIfGs_isEventBit(0x3080))
            return 0;
        if (getPhotoSelection() >= 0xC)
            return 0;
    }
    u8 item = dComIfGp_getSelectItem(index);
    if (item != 0x23 && item != 0x26)
        return 0;
    MtPictureAlbumHD* photos = MtPhotoAlbum_getPictures(&l_photoSave->mAlbum);
    return static_cast<u8>(photos->mPictureCount) != 0;
}

/* 022A0588 */
void daNpcMt_c::setCollision(dCcD_Cyl* cylinder, cXyz* center, f32 radius, f32 height) {
    cylinder->SetC(center);
    cylinder->SetR(radius);
    cylinder->SetH(height);
    dComIfG_Ccsp_Set(cylinder);
}

/* 022A0F34 */
BOOL daNpcMt_c::chkEndEvent() {

    s16 event = static_cast<s16>(this->mMtGetItemEventIdx);
    if (!dComIfGp_evmng_endCheck(event))
        return FALSE;
    dComIfGp_event_reset();
    return TRUE;
}

/* 0229FE28 */
u8 daNpcMt_c::isFigureGet(u8 figure) {

    if (figure >= 0x86)
        return 0;
    u16 event = static_cast<u16>(l_figure_comp.event[figure >> 3]);
    u32 value = dComIfGs_getEventRegHD(event);
    return value & (1u << (figure & 7));
}

/* 0229FF88 */
void daNpcMt_c::setFigure(u8 figure) {

    if (figure >= 0x86)
        return;
    const be<u16>* event = &l_figure_comp.event[figure >> 3];
    u32 value = dComIfGs_getEventRegHD(*event);
    // Reload after the event accessor, as the HD code does.
    dComIfGs_setEventRegHD(*event, (u8)(value | (1u << (figure & 7))));
    dComIfGs_onEventBit(0x3A01);
}

/* 0229FEAC */
s32 daNpcMt_c::getFigureMakeNum() {

    s32 count = 0;
    for (u32 i = 0; i < 0x86; ++i)
        if (isFigureGet(i))
            ++count;
    return count;
}

// HD: completion requires 133 of the 134 figure slots.
/* 0229FF24 */
BOOL daNpcMt_c::isComp() { return getFigureMakeNum() >= 0x85; }

// HD: Carlov remaps four photo IDs before selecting a figure.
/* 022A1F1C */
u32 daNpcMt_changePhotoNo(u32 photo) {

    switch (photo) {
    case 1:
        return 0x6A;
    case 2:
        return 0x69;
    case 4:
        return 0x67;
    case 6:
        return 0x6D;
    default:
        return photo;
    }
}

/* 022A0898 */
u32 daNpcMt_c::getPhoto(u32 index) { return photoBuffer()->get(index); }

/* 022A037C */
u8 daNpcMt_c::getPhotoSelection() { return static_cast<u8>(photoBuffer()->mNumPhotos); }

/* 0229FF54 */
void daNpcMt_c::clearPhotoSelection() { photoBuffer()->mNumPhotos = 0; }

/* 022A1F64 */
BOOL daNpcMt_c::hasPhoto(u32 figure) {
    u32 count = getPhotoSelection();
    for (u32 i = 0; i < count; ++i)
        if (getPhoto((u8)i) == figure)
            return TRUE;
    return FALSE;
}

/* 022A2008 */
void daNpcMt_c::submitPhoto(u32 photo) {

    u32 selection = getPhotoSelection();
    photoBuffer()->set(selection, photo);
    photoBuffer()->setActive(1);
}

/* 022A2078 */
void daNpcMt_c::makeFigures() {
    u32 count = getPhotoSelection();
    for (u32 i = 0; i < count; ++i) {
        u32 figure = getPhoto((u8)i);
        setFigure(figure);
        if (figure == dSnap_PhotoIndex2TableIndexHD(0x4A))
            setFigure(dSnap_PhotoIndex2TableIndexHD(0x49));
        else if (figure == dSnap_PhotoIndex2TableIndexHD(0x72)) {
            for (u32 photo = 0x73; photo <= 0x78; ++photo)
                setFigure(dSnap_PhotoIndex2TableIndexHD(photo));
        } else if (figure == dSnap_PhotoIndex2TableIndexHD(0x8C))
            setFigure(dSnap_PhotoIndex2TableIndexHD(0x8D));
        else if (figure == dSnap_PhotoIndex2TableIndexHD(0x88))
            setFigure(dSnap_PhotoIndex2TableIndexHD(0x87));
    }
}

/* 022A0894 */
s32 daNpc_MtCreate(daNpcMt_c* a) { return a->_create(); }

/* 022A0B1C */
BOOL daNpc_MtDelete(daNpcMt_c* a) { return a->_delete(); }

/* 022A1B9C */
BOOL daNpc_MtExecute(daNpcMt_c* a) { return a->_execute(); }

/* 022A1C48 */
BOOL daNpc_MtDraw(daNpcMt_c* a) { return a->_draw(); }

/* 022A2790 */
BOOL daNpc_MtIsDelete(daNpcMt_c* a) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 022A26E8 */
void daNpcMt_staticInit() {
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    __register_global_object(gabi::ea(l_destructorRecords.record[0]));
    l_headerStatics.mPi[0] = l_headerPi.value[0];
    l_headerStatics.mPi[1] = l_headerPi.value[1];
    Mt_initHeaderFirstHD(&l_headerStatics.mFirstObject);
    __register_global_object(gabi::ea(l_destructorRecords.record[1]));
    Mt_initHeaderSecondHD(&l_headerStatics.mSecondObject);
    __register_global_object(gabi::ea(l_destructorRecords.record[2]));
}

/* 022A2798 */
void daNpcMt_destruct(daNpcMt_c* a, u32 flags) {

    if (!a)
        return;
    __dt(static_cast<fopNpc_npc_c*>(a));
    if (flags & 1)
        operator_delete(a);
}

/* 022A277C */
void daNpcMt_textureDtor(void* p, u32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}

/* 022A2834 */
void daNpcMt_textureEmpty(void* p) {}

// clang-format off
/* WWHD glue
data l_msg_mt_3rd_talk 101C2298
data l_msg_mt_4th_talk 101C22A0
data l_msg_mt_queue_full_HD 101C22A8
data l_msg_mt_making 101C22B0
data l_msg_mt_gallery 101C22B8
data l_msg_mt_room_in 101C22C0
data l_msg_mt_kansei_end 101C22C8
data l_msg_mt_no_color 101C22D0
data l_msg_mt_no_figure 101C22D8
data l_msg_mt_maked 101C22E0
data l_msg_mt_already_queued_HD 101C22E8
data l_msg_mt_face_ng 101C22F0
data l_msg_mt_body_ng 101C22F8
data l_msg_mt_1st_talk 101C2340
data l_msg_mt_2nd_talk 101C2360
data l_msg_mt_room 101C2370
data l_msg_mt_make_1st 101C2390
data l_msg_mt_make_2nd 101C23A0
data l_msg_mt_make_1st_mt 101C23B0
data l_msg_mt_make_2nd_mt 101C23C0
data l_msg_mt_make_1st_mn 101C23D0
data l_msg_mt_make_2nd_mn 101C23E0
data l_msg_mt_1st_kansei 101C2450
data l_msg_mt_2nd_kansei 101C2464
data l_msg_mt_make 101C2300
data l_msg_mt_make_mt 101C2308
data l_msg_mt_make_mn 101C2310
data l_figure_room_name 101C23F0
data l_bmd_ix_tbl 1001E984
data l_btp_ix_tbl 1001E988
data l_bck_ix_tbl 1001E9D0
data l_execute_init 101C2410
data moveProc 101C2420
data l_method 101C24E8
data l_msg_anm_tbl 1001EAD0
data l_my_name 101C2318
data l_headerStatics 10467CC0
data l_headerPi 1001EB38
data l_destructorRecords 101C24FC
data l_cyl_src 101EA190
data l_cut_action 101C24F4
str 1001EA08 "d_a_npc_mt.cpp" in 0229F9B8
str 1001E9EC "m_head_tex_pattern != (0)" in 0229F9B8
str 1001EB28 "d_a_npc_mt.cpp" in 022A1DF4
str 1001EAEC "roomID < (sizeof(l_msg_mt_room) / sizeof(l_msg_mt_room[0]))" in 022A1DF4
data l_arcname_tbl 101C231C
str 1001EAB4 "MsgNo" in 022A0FA8
str 1001EAA4 "MT_GET_ITEM" in 022A0618
data l_npc_dat 101C249C
data l_npc_anm_wait 101C2330
data l_figure_comp 101C2478
data l_photoSave 101F84DC
data l_messageManager 101F4B5C
ext 025F7DB0 s32 MtMessageManagerHD::messageSet(u32 number, cXyz* position)
ext 025F795C u16 MtMessageManagerHD::getStatus16()
ext 025F795C u32 MtMessageManagerHD::getStatus()
ext 025F74D0 void MtMessageManagerHD::setStatus(u32 status)
str 1001EA50 "backbone" in 0229FABC
str 1001EA1C "head" in 0229FABC
str 1001EA24 "d_a_npc_mt.cpp" in 0229FABC
str 1001EA5C "m_jnt.getBackboneJntNum() >= 0" in 0229FABC
str 1001EA34 "m_jnt.getHeadJntNum() >= 0" in 0229FABC
ext 0259DED0 void Mt_lookAtTargetHD(dNpc_JntCtrl_c* joint, be<s16>* outAngle, cXyz* target, cXyz* eye, s16 yaw, s16 turn, u32 enabled)
ext 028ED6F8 void Mt_initHeaderFirstHD(u8* object)
ext 028EAB2C void Mt_initHeaderSecondHD(u8* object)
ext 025E789C BOOL Mt_initBtpHD(mDoExt_btpAnm* animation, J3DModelData* data, J3DAnmTexPattern* pattern, u32 play, s32 mode, f32 speed, s32 start, s32 end, u32 modify)
ext 0273AA24 void Mt_assertHD(const char* file, u32 line, const char* condition)
ext 025BD600 u8 dSnap_PhotoIndex2TableIndexHD(u32 photo)
ext 025BD64C u32 dSnap_GetFigRoomIdHD(u32 figure)
ext 025D7DEC u32 Mt_createItemForPresentDemoHD(cXyz* position, u32 item, u32 flags, s32 bit, s32 room, csXyz* angle, cXyz* scale)
ext 025B8B7C void Mt_offEventBitHD(dSv_event_c* event, u16 bit)
ext 02543280 void Mt_cutEndHD(dEvent_manager_c* manager, s32 staff)
ext 025447C8 BOOL Mt_getIsAddvanceHD(dEvent_manager_c* manager, s32 staff)
ext 02525FE4 s32 Mt_phaseHandlerHD(request_of_phase_process_class* phase, be<u32>* table, daNpcMt_c* actor)
ext 026067F4 void* Mt_getObjectResHD(dRes_control_c* control, SafeString* name, u32 resource)
ext 02720118 MtPhotoQueueHD* MtPhotoAlbum_getQueue(MtPhotoAlbumHD* album)
ext 02720144 MtPictureAlbumHD* MtPhotoAlbum_getPictures(MtPhotoAlbumHD* album)
ext 0271F978 u32 MtPhotoQueueHD::get(u32 index)
ext 0271F994 void MtPhotoQueueHD::set(u32 index, u32 photo)
ext 0271F9A8 void MtPhotoQueueHD::setActive(u32 active)
ext 02725E0C void MtPictureAlbum_erase(MtPictureAlbumHD* album, u8 index)
*/
// clang-format on
