/* Salvatore (Squid-Hunt), reconstructed from WWHD. */
#include "d/actor/d_a_npc_kg1_core.h"

const gc_tu_data gc_tu = {.safeString = 0x1001BC44, .objAcchDt = {0x1001BC5C, 0x1001BC6C}};

static J3DModel* modelOf(kg1_core::daNpc_Kg1_c* a) {
    return a->mpMorf.get()->getModel();
}

static void copyFloatMatrix(Mtx34* dst, const Mtx34* src) {
    // HD: all twelve floating loads precede the first store.
    J3DModel::mtx_assign(dst, src);
}

/* inlined into kg1_core::daNpc_Kg1_c::initTexPatternAnm, kg1_core::daNpc_Kg1_c::CreateHeap */
static void* objectResource(s32 id) {

    Kg1ResourceName storage;
    SafeString* name = &storage.name;
    name->mStringTop = gabi::guest_addr("Kg");
    name->__vtbl = gc_tu.safeString;
    void* control = kg1Resources();
    return Kg1Core_getResource(control, name, id);
}

static JUTNameTab* jointNameTable(J3DModelData* data) {
    return data->getJointName();
}

static void copyVectorWords(cXyz* dest, const cXyz* src) {
    // HD: these copies retain integer floating-point payloads.
    const Kg1VectorWords* words = reinterpret_cast<const Kg1VectorWords*>(src);
    u32 x = words->mWord[0], y = words->mWord[1], z = words->mWord[2];
    kg1Words(dest)->mWord[0] = x;
    kg1Words(dest)->mWord[1] = y;
    kg1Words(dest)->mWord[2] = z;
}

static Mtx34* jointMatrix(J3DModel* model, u32 index) {
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    Mtx34* matrices = kg1Matrices(block->mpAnmMtx);
    u16 flags = block->mFlags;
    block->mFlags = flags | 0x10;
    return &matrices[index];
}

/* 022678B8 */
void* daNpc_Kg1_HIO_ctor(void* self) {

    Kg1HIO* hio = static_cast<Kg1HIO*>(self);
    if (!hio) {
        hio = static_cast<Kg1HIO*>(operator_new(sizeof(Kg1HIO)));
        if (!hio)
            return nullptr;
    }
    // HD: the derived HIO vtable follows the NPC HIO array.
    hio->__vtbl = Kg1HioVtable;
    Kg1Core_constructArray(&hio->mNpc, 1, 0x28, Kg1NpcHioCtor);
    hio->mNpc.m22 = 0;
    hio->mNpc.mMaxHeadY = 7000;
    hio->mDebug = 0;
    hio->mChild = -1;
    hio->mNpc.mMaxBackboneY = 8000;
    hio->mUsers = -1;
    hio->mNpc.mMinHeadX = -2500;
    hio->mNpc.m04 = 0.0f;
    hio->mNpc.mMaxHeadTurnVel = 2000;
    hio->mNpc.mMinBackboneY = -8000;
    hio->mNpc.mAttnYOffset = 35.0f;
    hio->mEnable = 0;
    hio->mNpc.mMaxAttnAngleY = 0x4000;
    hio->mNpc.mMaxBackboneX = 2000;
    hio->mNpc.mMaxAttnDistXZ = 400.0f;
    hio->mNpc.mMaxHeadX = 2500;
    hio->mNpc.mMaxTurnStep = 1000;
    hio->mDraw = 0;
    hio->mNpc.mMinBackboneX = -2000;
    hio->mNpc.mMinHeadY = -7000;
    return hio;
}

/* 0226559C */
BOOL daNpc_Kg1_nodeCallBack(J3DNode* node, s32 stage) {

    if (stage != 0)
        return TRUE;
    J3DModel* model = j3dSys.getModel();
    kg1_core::daNpc_Kg1_c* actor = static_cast<kg1_core::daNpc_Kg1_c*>(model->getUserArea());
    Kg1JointResource* joint = Kg1Core_jointResource(node);
    u32 index = joint->mIndex;
    Mtx34* matrix = kg1Matrix();
    PSMTXCopy(jointMatrix(model, index), matrix);
    if (index == (u32) static_cast<s8>(actor->m_jnt.mHeadJntNum)) {
        mDoMtx_XrotM(matrix, static_cast<s16>(actor->m_jnt.mAngles[0][1]));
        mDoMtx_ZrotM(matrix, (s16) - static_cast<s16>(actor->m_jnt.mAngles[0][0]));
        if (!static_cast<u32>(kg1HeadOffsets().mHeadInitialized)) {
            kg1HeadOffsets().mHead.z = 0.0f;
            kg1HeadOffsets().mHeadInitialized = 1;
            kg1HeadOffsets().mHead.y = 5.0f;
            kg1HeadOffsets().mHead.x = 24.0f;
        }
        if (!static_cast<u32>(kg1HeadOffsets().mEyeInitialized)) {
            kg1HeadOffsets().mEye.z = 0.0f;
            kg1HeadOffsets().mEyeInitialized = 1;
            kg1HeadOffsets().mEye.y = -16.0f;
            kg1HeadOffsets().mEye.x = 24.0f;
        }
        PSMTXMultVec(matrix, &kg1HeadOffsets().mHead, &actor->mHeadPos);
        mDoMtx_XrotM(matrix, static_cast<s16>(actor->m_jnt.mAngles[0][1]));
        mDoMtx_ZrotM(matrix, (s16) - static_cast<s16>(actor->m_jnt.mAngles[0][0]));
        PSMTXMultVec(matrix, &kg1HeadOffsets().mEye, &actor->mEyePos);
        PSMTXMultVec(matrix, &kg1HeadOffsets().mHead, &actor->attention_info.position);
        actor->attention_info.position.y = static_cast<f32>(actor->attention_info.position.y) +
                                           static_cast<f32>(kg1HIO().mNpc.mAttnYOffset);
    }
    if (index == (u32) static_cast<s8>(actor->m_jnt.mBackboneJntNum)) {
        mDoMtx_XrotM(matrix, static_cast<s16>(actor->m_jnt.mAngles[1][1]));
        mDoMtx_ZrotM(matrix, (s16) - static_cast<s16>(actor->m_jnt.mAngles[1][0]));
    }
    copyFloatMatrix(jointMatrix(model, index), matrix);
    PSMTXCopy(matrix, J3DSys::mCurrentMtx);
    if (index == 8) {
        mDoMtx_stack_c::transM(23.46f, -22.26f, -47.05f);
        Kg1Core_XYZrotM(matrix, 0x1F4B, (s16)0xB100, 0x1F4B);
        J3DModel* prop = actor->m6C4;
        copyFloatMatrix(&prop->mBaseTransformMtx, matrix);
    }
    return TRUE;
}

/* 0226676C */
void kg1_core::daNpc_Kg1_c::lookBack() {
    auto* a = this;

    fopAc_ac_c* player = kg1Play()->play.mPlayerInfo[0].mpPlayer.get();
    cXyz difference;
    cXyz horizontal;
    cXyz target;
    cXyz eye;
    cXyz playerEye;
    cXyz_mi(&player->current.pos, &difference, &a->current.pos);
    horizontal.x = difference.x;
    horizontal.y = 0.0f;
    horizontal.z = difference.z;
    f32 distance = PSVECSquareMag(&horizontal);
    distance = kg1Sqrt(distance);
    cXyz* targetAddress = nullptr;
    bool immediate = false;
    if (static_cast<u8>(a->mEventCut.mbAttention)) {
        f32 x = static_cast<f32>(a->mEventCut.mPos.x), y = static_cast<f32>(a->mEventCut.mPos.y);
        a->m_jnt.mbTrn = 1;
        a->m731 = 1;
        target.x = x;
        target.y = y;
        target.z = static_cast<f32>(a->mEventCut.mPos.z);
        targetAddress = &target;
    } else {
        if (a->m74D == 8) {
            copyVectorWords(&target, &kg1CameraEye());
            targetAddress = &target;
        } else if (distance < static_cast<f32>(kg1HIO().mNpc.mMaxAttnDistXZ)) {
            dNpc_playerEyePos(&playerEye, static_cast<f32>(kg1HIO().mNpc.m04));
            copyVectorWords(&target, &playerEye);
            targetAddress = &target;
            BOOL attention = a->chkAttention();
            a->m731 = (u8)attention;
        } else
            a->m731 = 0;
        immediate = static_cast<u8>(a->m_jnt.mbTrn) == 0;
    }
    if (immediate) {
        eye.x = static_cast<f32>(a->eyePos.x);
        eye.y = static_cast<f32>(a->eyePos.y);
        eye.z = static_cast<f32>(a->eyePos.z);
        s16 facing = static_cast<s16>(a->current.angle.y);
        a->m72E = 0;
        kg1LookAt(&a->m_jnt, &a->current.angle.y, targetAddress, &eye, facing, 0, 1);
        return;
    }
    s16 speed = static_cast<s16>(a->mEventCut.mTurnSpeed),
        defaultSpeed = static_cast<s16>(kg1HIO().mNpc.mMaxHeadTurnVel);
    if (speed == 0)
        speed = defaultSpeed;
    cLib_addCalcAngleS2(&a->m72E, speed, 4, 0x800);
    eye.x = static_cast<f32>(a->eyePos.x);
    eye.y = static_cast<f32>(a->eyePos.y);
    eye.z = static_cast<f32>(a->eyePos.z);
    s16 facing = static_cast<s16>(a->current.angle.y), tracking = static_cast<s16>(a->m72E);
    kg1LookAt(&a->m_jnt, &a->current.angle.y, targetAddress, &eye, facing, tracking, 1);
}

/* 02266668 */
BOOL kg1_core::daNpc_Kg1_c::chkAttention() {
    auto* a = this;

    fopAc_ac_c* player = kg1Play()->play.mPlayerInfo[0].mpPlayer.get();
    cXyz difference;
    cXyz horizontal;
    cXyz_mi(&player->current.pos, &difference, &a->current.pos);
    horizontal.x = difference.x;
    horizontal.y = 0.0f;
    horizontal.z = difference.z;
    f32 distance = PSVECSquareMag(&horizontal);
    distance = kg1Sqrt(distance);
    f32 radius = static_cast<f32>(kg1HIO().mNpc.mMaxAttnDistXZ);
    s16 angleLimit = static_cast<s16>(kg1HIO().mNpc.mMaxAttnAngleY);
    if (!(distance < radius))
        return FALSE;
    f32 x = static_cast<f32>(player->current.pos.x) - static_cast<f32>(a->current.pos.x);
    f32 z = static_cast<f32>(player->current.pos.z) - static_cast<f32>(a->current.pos.z);
    s32 angle = kg1Atan2(x, z);
    s32 facing = static_cast<s16>(a->current.angle.y) + static_cast<s16>(a->m_jnt.mAngles[0][1]) +
                 static_cast<s16>(a->m_jnt.mAngles[1][1]);
    s16 delta = (s16)(angle - facing);
    s16 magnitude = (s16)(delta < 0 ? -(s32)delta : delta);
    return magnitude < angleLimit;
}

/* 0226586C */
BOOL kg1_core::daNpc_Kg1_c::initTexPatternAnm(s32 index, u32 modify) {
    auto* a = this;

    J3DModel* model = modelOf(a);
    J3DModelData* data = model->getModelData();
    s32 resource = static_cast<s32>(kg1BtpIndices().mIndex[index]);
    J3DAnmTexPattern* pattern = reinterpret_cast<J3DAnmTexPattern*>(objectResource(resource));
    a->m_eye_tex_pattern = pattern;
    if (!pattern) {
        Kg1Core_assert("d_a_npc_kg1.cpp", 0x1CF, "m_eye_tex_pattern != (0)");
        pattern = a->m_eye_tex_pattern.get();
    }
    if (!Kg1Core_initBtp(&a->m6F8, data, pattern, 1, 2, 1.0f, 0, -1, (u32)modify, 0))
        return FALSE;
    a->m720 = 0;
    a->m72C = 0;
    return TRUE;
}

/* 022665C8 */
void kg1_core::daNpc_Kg1_c::playTexPatternAnm() {
    auto* a = this;

    if (Kg1Core_calcTimer(&a->m72C) != 0)
        return;
    J3DAnmTexPattern* anm = a->m_eye_tex_pattern;
    Kg1PatternVtable* vt = kg1Pattern(anm)->mpVtable;
    s32 max = gabi::call_ptr<s32>(vt->mGetFrameMax.get(), anm);
    if ((s32)a->m720 >= max) {
        anm = a->m_eye_tex_pattern;
        vt = kg1Pattern(anm)->mpVtable;
        max = gabi::call_ptr<s32>(vt->mGetFrameMax.get(), anm);
        u8 frame = a->m720;
        a->m72C = 120;
        a->m720 = (u8)(frame - max);
    } else
        a->m720 = (u8)(a->m720 + 1);
}

/* 02265CCC */
void kg1_core::daNpc_Kg1_c::set_mtx() {
    auto* a = this;

    J3DModel* model = modelOf(a);
    f32 x = static_cast<f32>(a->scale.x), y = static_cast<f32>(a->scale.y),
        z = static_cast<f32>(a->scale.z);
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = static_cast<f32>(a->current.pos.x);
    y = static_cast<f32>(a->current.pos.y);
    z = static_cast<f32>(a->current.pos.z);
    PSMTXTrans(kg1Matrix(), x, y, z);
    mDoMtx_YrotM(kg1Matrix(), static_cast<s16>(a->current.angle.y));
    model = modelOf(a);
    copyFloatMatrix(&model->mBaseTransformMtx, kg1Matrix());
    model = a->m6C4.get();
    x = static_cast<f32>(a->scale.x);
    y = static_cast<f32>(a->scale.y);
    z = static_cast<f32>(a->scale.z);
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
}

/* 02265CC8 */
BOOL CheckCreateHeap(kg1_core::daNpc_Kg1_c* a) {
    return a->CreateHeap();
}

/* 02265978 */
BOOL kg1_core::daNpc_Kg1_c::CreateHeap() {
    auto* a = this;

    J3DModelData* data = static_cast<J3DModelData*>(objectResource(5));
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(objectResource(17));
    mDoExt_McaMorf* morf =
        Kg1Core_newMorf(0, data, 0, 0, animation, 2, 0, -1, 1.0f, 1, 0, 0x80000, 0x11020022);
    a->mpMorf = morf;
    if (!morf || !morf->mpModel.get())
        return FALSE;
    JUTNameTab* names = jointNameTable(data);
    s8 head = Kg1Core_jointIndex(names, "head");
    a->m_jnt.mHeadJntNum = head;
    if (head < 0)
        Kg1Core_assert("d_a_npc_kg1.cpp", 0x227, "m_jnt.getHeadJntNum() >= 0");
    names = jointNameTable(data);
    s8 spine = Kg1Core_jointIndex(names, "backbone2");
    a->m_jnt.mBackboneJntNum = spine;
    if (spine < 0)
        Kg1Core_assert("d_a_npc_kg1.cpp", 0x22C, "m_jnt.getBackboneJntNum() >= 0");
    if (!a->initTexPatternAnm(3, 0))
        return FALSE;
    a->initTexPatternAnm(0, 1);
    J3DModel* prop =
        mDoExt_J3DModel__create(static_cast<J3DModelData*>(objectResource(6)), 0x80000, 0x11020022);
    a->m6C4 = prop;
    if (!prop)
        return FALSE;
    J3DModelData* propData = static_cast<J3DModelData*>(objectResource(6));
    J3DAnmTexPattern* propPattern = static_cast<J3DAnmTexPattern*>(objectResource(10));
    if (!Kg1Core_initBtp(&a->mBtpAnm, propData, propPattern, 1, 2, 1.0f, 0, -1, 0, 0))
        return FALSE;
    u16 index = 0;
    u16 jointCount = data->getJointNum();
    while (index < jointCount) {
        if ((u32)index == (u32) static_cast<s8>(a->m_jnt.mHeadJntNum) ||
            (u32)index == (u32) static_cast<s8>(a->m_jnt.mBackboneJntNum) || index == 8) {
            J3DJoint* joint = data->getJointNodePointer(index);
            joint->mCallBack = Kg1NodeCallback;
        }
        index = (u16)(index + 1);
        jointCount = data->getJointNum();
    }
    J3DModel* model = modelOf(a);
    model->mUserArea = gabi::ea(a);
    a->mAcchCir.SetWall(30.0f, 30.0f);
    a->mObjAcch.Set(&a->current.pos, &a->old.pos, a, 1, &a->mAcchCir, &a->speed, &a->current.angle,
                    &a->shape_angle);
    return TRUE;
}

/* 022660C4 */
s32 kg1_core::daNpc_Kg1_c::CreateInit() {
    auto* a = this;

    a->mStts.Init(0xFF, 0xFF, a);
    a->mCyl.Set(kg1CylinderSource());
    a->mCyl.mStts = &a->mStts;
    a->setCollision(60.0f, 150.0f);
    a->gravity = -9.0f;
    a->mEventCut.setActorInfo2("Auc0", a);
    a->set_mtx();
    a->m730 = 0;
    a->m72E = 0;
    a->m731 = 0;
    J3DModel* model = modelOf(a);
    a->cullMtx = gabi::ea(model ? &model->mBaseTransformMtx : nullptr);
    a->wait_action_init();
    s32 users = static_cast<s32>(kg1HIO().mUsers);
    if (users < 0) {
        // HD: ASCII source alias for the Japanese debug label; guest bytes unchanged.
        s32 child = Kg1Core_createChild("Squid-Hunt", &kg1HIO());
        users = static_cast<s32>(kg1HIO().mUsers);
        kg1HIO().mChild = (s8)child;
    }
    kg1HIO().mUsers = users + 1;
    a->attention_info.flags = 10;
    auto* play = kg1Play();
    a->mEventIds[0] = Kg1Core_eventIndex(&play->play.mEvtManager, "MINIGAME_START", 0xFF);
    play = kg1Play();
    a->mEventIds[1] = Kg1Core_eventIndex(&play->play.mEvtManager, "MINIGAME_END", 0xFF);
    play = kg1Play();
    s16 finalId = Kg1Core_eventIndex(&play->play.mEvtManager, "KAISEN_GETITEM", 0xFF);
    a->mPreviousAnimation = 0;
    a->mEventIds[2] = finalId;
    a->m74D = 2;
    Kg1Core_setAnm(a);
    u32 y = static_cast<u32>(kg1Words(&a->current.pos)->mWord[1]),
        x = static_cast<u32>(kg1Words(&a->current.pos)->mWord[0]);
    a->m_jnt.mbBackBoneLock = 1;
    kg1Words(&a->mEyePos)->mWord[0] = x;
    a->m750 = 1;
    kg1Words(&a->mEyePos)->mWord[1] = y;
    u32 z = static_cast<u32>(kg1Words(&a->current.pos)->mWord[2]);
    a->m_jnt.mbHeadLock = 1;
    kg1Words(&a->mEyePos)->mWord[2] = z;
    return cPhs_COMPLEATE_e;
}

/* 022664A4 */
void kg1_core::daNpc_Kg1_c::eventOrder() {
    auto* a = this;

    u8 order = a->m732;
    if (order == 1 || order == 2) {
        u16 flags = static_cast<u16>(a->eventInfo.mCondition);
        order = a->m732;
        a->eventInfo.mCondition = flags | dEvtCnd_CANTALK_e;
        if (order == 2)
            fopAcM_orderSpeakEvent(a);
    } else if (order >= 3 && order <= 5) {
        s16 id = a->mEventIds[order - 3];
        fopAcM_orderOtherEventId(a, id, 0xFF, 0xFFFF, 0, 1);
        a->eventInfo.mCondition = static_cast<u16>(a->eventInfo.mCondition) | dEvtCnd_UNK2_e;
    }
}

/* 022663A4 */
void kg1_core::daNpc_Kg1_c::checkOrder() {
    auto* a = this;

    u16 condition = static_cast<u16>(a->eventInfo.mCommand);
    if (condition == 2) {
        s16 event = a->mEventIds[0];
        auto* play = kg1Play();
        if (Kg1Core_startCheck(&play->play.mEvtManager, event) && a->m732 == 3)
            a->m732 = 0;
        event = a->mEventIds[0];
        play = kg1Play();
        if (Kg1Core_endCheck(&play->play.mEvtManager, event))
            a->m732 = 0;
        event = a->mEventIds[2];
        play = kg1Play();
        if (Kg1Core_startCheck(&play->play.mEvtManager, event) && a->m732 == 5)
            a->m732 = 0;
    } else if (condition == 1 && (a->m732 == 2 || a->m732 == 1))
        a->m730 = 1;
}

/* 02266C90 */
void kg1_core::daNpc_Kg1_c::kg1_talk_camera() {
    auto* a = this;

    s8 cameraIndex = kg1Play()->play.mPlayerInfo[0].mCameraID;
    auto* play = kg1Play();
    u8 enabled = static_cast<u8>(a->m751);
    camera_class* camera = kg1CameraProcess(play->play.mCameraInfo[cameraIndex].mpCamera);
    if (!enabled || !camera)
        return;
    Kg1Core_cameraStart(kg1CameraBody(camera));
    cXyz center;
    cXyz eye;
    center.x = static_cast<f32>(kg1CameraCenter().x);
    center.y = static_cast<f32>(kg1CameraCenter().y);
    center.z = static_cast<f32>(kg1CameraCenter().z);
    eye.x = static_cast<f32>(kg1CameraEye().x);
    eye.y = static_cast<f32>(kg1CameraEye().y);
    eye.z = static_cast<f32>(kg1CameraEye().z);
    Kg1Core_cameraSet(kg1CameraBody(camera), &center, &eye, 0, 40.0f);
    Kg1Core_cameraReset(kg1CameraBody(camera));
    Kg1Core_cameraTrim(kg1CameraBody(camera), 1);
}

/* 02265DE8 */
void kg1_core::daNpc_Kg1_c::wait_action_init() {
    auto* a = this;

    a->clr_seq_flag();
    a->mAction.i = -1;
    a->mAction.f = Kg1WaitAction;
    a->mAction.d = 0;
}

/* 02265DC8 */
void kg1_core::daNpc_Kg1_c::clr_seq_flag() {
    auto* a = this;

    a->mSequenceFlags[5] = 0;
    a->mSequenceFlags[1] = 0;
    a->mSequenceFlags[4] = 0;
    a->mSequenceFlags[3] = 0;
    a->mSequenceFlags[2] = 0;
    a->mSequenceFlags[0] = 0;
}

/* 02267498 */
u32 kg1_core::daNpc_Kg1_c::getMsg() {
    auto* a = this;

    if (a->mSequenceFlags[2]) {
        if (!a->mSequenceFlags[1])
            return 0x1D5D;
        if (!a->mSequenceFlags[3])
            return 0x1D5F;
        if (!a->mSequenceFlags[4]) {
            u32 score = kg1GetEventReg(kg1SaveEvent(), 0xBEFF);
            if (score > static_cast<u8>(a->mGameBoardScore)) {
                Kg1Core_onEventBit(kg1SaveEvent(), 0xE04);
                kg1SetEventReg(kg1SaveEvent(), 0xBEFF, static_cast<u8>(a->mGameBoardScore));
                u32 wins = kg1GetEventReg(kg1SaveEvent(), 0xFF07);
                if (wins < 3)
                    kg1SetEventReg(kg1SaveEvent(), 0xFF07, (u8)(wins + 1));
                a->mSequenceFlags[5] = 1;
                return 0x1D63;
            }
        }
        return 0x1D64;
    }
    return static_cast<u8>(a->mbDidClearGame) ? 0x1D4E : 0x1D4D;
}

/* 022675BC */
u16 kg1_core::daNpc_Kg1_c::next_msgStatus(be<u32>* message) {
    auto* a = this;

    msg_class* msgObject = fopNpc_npc_c::mpCurrMsg;
    auto* play = kg1Play();
    u32 current = *message;
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    switch (current) {
    case 0x1D4D:
        if (!Kg1Core_isEventBit(kg1SaveEvent(), 0xE04))
            *message = 0x1D52;
        else if (Kg1Core_getDays() < 4)
            *message = 0x1D4F;
        else if (Kg1Core_isEventBit(kg1SaveEvent(true), 0x101))
            *message = 0x1D50;
        else {
            Kg1Core_onEventBit(kg1SaveEvent(true), 0x101);
            *message = 0x1D51;
        }
        a->mbDidClearGame = 1;
        break;
    case 0x1D4E:
    case 0x1D4F:
    case 0x1D50:
    case 0x1D51:
        *message = 0x1D52;
        break;
    case 0x1D52:
    case 0x1D5E: {
        a->clr_seq_flag();
        u32 choice = static_cast<u32>(msgObject->mSelectNum);
        if (choice == 0) {
            u16 rupees = dComIfGs_getRupee();
            if (rupees < 10)
                *message = 0x1D54;
            else {
                a->m74D = 1;
                play = kg1Play();
                u32 balance = static_cast<u32>(play->play.mItemRupeeCount);
                play->play.mItemRupeeCount = balance - 10;
                cXyz position;
                position.x = 0.0f;
                position.y = 0.0f;
                position.z = 250.0f;
                a->m_jnt.mbHeadLock = 1;
                a->m751 = 1;
                Kg1PlayerVtable* vt = kg1PlayerVtable(player);
                gabi::call_ptr(vt->mSetPlayerPosAndAngle.get(), player, &position, 0x2000);
                *message = 0x1D55;
            }
        } else if (choice == 1)
            *message = 0x1D53;
        break;
    }
    case 0x1D55:
        *message = static_cast<u8>(a->mbGameEnd) ? 0x1D5C : 0x1D56;
        a->mSequenceFlags[0] = 1;
        break;
    case 0x1D56:
    case 0x1D57:
    case 0x1D58:
    case 0x1D59:
    case 0x1D5D:
        *message = current + 1;
        break;
    case 0x1D5A: {
        u32 choice = static_cast<u32>(msgObject->mSelectNum);
        if (choice == 0)
            *message = 0x1D5C;
        else if (choice == 1)
            *message = 0x1D5B;
        break;
    }
    case 0x1D5B:
        *message = 0x1D57;
        break;
    case 0x1D5F: {
        u32 wins = kg1GetEventReg(kg1SaveEvent(), 0xFE07);
        *message = wins == 0 ? 0x1D60 : wins == 1 ? 0x1D61 : 0x1D62;
        dSv_event_c* event = kg1SaveEvent();
        if (wins < 3)
            wins = (u8)(wins + 1);
        kg1SetEventReg(event, 0xFE07, wins);
        break;
    }
    case 0x1D5C:
        a->mbGameEnd = 1;
        return 0x10;
    default:
        return 0x10;
    }
    return 0xF;
}

/* 02266C34 */
void kg1_core::daNpc_Kg1_c::anmAtr(u16 attr) {
    auto* a = this;

    if (attr == 6) {
        u8 requested = static_cast<u8>(kg1Play()->play.mMesgAnime);
        if (requested < 9) {
            a->m74D = static_cast<s8>(kg1RequestedAnimations().mAnimation[requested]);
            kg1Play()->play.mMesgAnime = 0xFF;
        }
    }
}

/* 02266268 */
s32 kg1_core::daNpc_Kg1_c::_create() {
    auto* a = this;

    u32 flags = static_cast<u32>(a->actor_condition);
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(a)) {
            Kg1Core_constructNpc(a);
            a->__vtbl = Kg1ActorVtable;
            mDoExt_btpAnm::ct(&a->mBtpAnm);
            mDoExt_btpAnm::ct(&a->m6F8);
            mDoExt_btpAnm::ct(&a->m70C);
            flags = static_cast<u32>(a->actor_condition);
        }
        a->actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&a->mPhs, "Kg");
    if (phase == cPhs_COMPLEATE_e) {
        if (Kg1Core_entryHeap(a, Kg1HeapCallback, 0x10000))
            return a->CreateInit();
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 02266320 */
BOOL daNpc_Kg1Delete(kg1_core::daNpc_Kg1_c* a) {

    dComIfG_resDelete(&a->mPhs, "Kg");
    if (gabi::ea(a->heap.get())) {
        mDoExt_McaMorf* morf = a->mpMorf;
        if (morf)
            morf->stopZelAnime();
    }
    s32 count = static_cast<s32>(kg1HIO().mUsers);
    if (count >= 0) {
        --count;
        kg1HIO().mUsers = count;
        if (count < 0)
            mDoHIO_deleteChild(static_cast<s8>(kg1HIO().mChild));
    }
    return TRUE;
}

/* 02266990 */
BOOL daNpc_Kg1Execute(kg1_core::daNpc_Kg1_c* a) {

    s16 params[9];
    for (int i = 0; i < 9; ++i)
        params[i] = static_cast<s16>(kg1JointParams()[i]);
    a->m_jnt.setParam(params[1], params[3], params[5], params[7], params[0], params[2], params[4],
                      params[6], params[8]);
    a->checkOrder();
    // HD: the GHS action member pointer has eight bytes.
    (a->*a->mAction)();
    a->eventOrder();
    a->playTexPatternAnm();
    Kg1Core_setAnm(a);
    if (static_cast<u8>(a->m750))
        a->mpMorf.get()->play(nullptr, 0, 0);
    auto* play = kg1Play();
    a->mObjAcch.CrrPos(&play->play.mBgS);
    a->setCollision(60.0f, 150.0f);
    f32 y = static_cast<f32>(a->current.pos.y), z = static_cast<f32>(a->current.pos.z),
        x = static_cast<f32>(a->current.pos.x);
    a->attention_info.position.x = x;
    a->eyePos.z = z;
    a->attention_info.position.y = y + 190.0f;
    a->attention_info.position.z = z;
    a->eyePos.y = y + 150.0f;
    a->eyePos.x = x;
    a->lookBack();
    a->set_mtx();
    return FALSE;
}

/* 02266B24 */
BOOL daNpc_Kg1Draw(kg1_core::daNpc_Kg1_c* a) {

    dSnap_RegistFig(0x81, a, 1.0f, 1.0f, 1.0f);
    J3DModel* model = modelOf(a);
    J3DModelData* data = model->getModelData();
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    settingTevStruct(environment, 0, &a->current.pos, &a->tevStr);
    mDoExt_McaMorf* morf = a->mpMorf;
    environment = dKy_getEnvlight();
    model = morf->mpModel.get();
    setLightTevColorType(environment, model, &a->tevStr);
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, a->m6C4.get(), &a->tevStr);
    Kg1Core_entryBtp(&a->m6F8, data, (s32)a->m720);
    a->mpMorf.get()->updateDL();
    reinterpret_cast<Kg1ModelDataView*>(data)->mTextureAnimation = 0;
    if (a->mSpecialAnimation) {
        s16 frame = (s16)gabi::ftoi(static_cast<f32>(a->mBtpAnm.mFrameCtrl.mFrame));
        J3DModel* prop = a->m6C4;
        Kg1Core_entryBtp(&a->mBtpAnm, prop->getModelData(), frame);
        mDoExt_modelUpdateDL(a->m6C4.get(), 0);
    }
    if (static_cast<u8>(a->m779))
        kg1Play()->play.mDoStatusForce = 25;
    return TRUE;
}

/* 02267AB8 */
BOOL daNpc_Kg1IsDelete(kg1_core::daNpc_Kg1_c* a) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit
/* 02267AA4 */
void daNpc_Kg1_hio_delete(void* self, u32 flags) {

    if (self && (flags & 1))
        operator_delete(self);
}

/* 02267AC0 */
void kg1_core::daNpc_Kg1_c::__dt(u32 flags) {
    auto* a = this;

    if (gabi::ea(a) == 0)
        return;
    Kg1Core_destroyCylinder(&a->mCyl, 2);
    Kg1Core_destroyStts(&a->mStts, 2);
    Kg1Core_destroyCircle(&a->mAcchCir.m_cir, 2);
    // HD: preserve this unit's destructor vtables and destruction order.
    a->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
    a->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    Kg1Core_destroyAcch(&a->mObjAcch, 0);
    Kg1Core_destroyActor(a, 0);
    if (flags & 1)
        operator_delete(a);
}

/* 02267B5C */
void daNpc_Kg1_texVirtualEmpty(void* a) {
}

/* 022679B4 */

void daNpc_Kg1_sinit() {

    // HD: shared header statics are initialized in the original order.
    kg1HeaderStatics().mZero[3] = 0;
    kg1HeaderStatics().mZero[2] = 0;
    kg1HeaderStatics().mZero[1] = 0;
    kg1HeaderStatics().mZero[0] = 0;
    Kg1Core_registerGlobal(kg1DestructorRecords(0));
    kg1HeaderStatics().mFirst = static_cast<f32>(kg1InitialValues().mFirst);
    kg1HeaderStatics().mSecond = static_cast<f32>(kg1InitialValues().mSecond);
    Kg1Core_initAngle(&kg1HeaderStatics().mAngle);
    Kg1Core_registerGlobal(kg1DestructorRecords(1));
    Kg1Core_initVector(&kg1HeaderStatics().mVector);
    Kg1Core_registerGlobal(kg1DestructorRecords(2));
    daNpc_Kg1_HIO_ctor(&kg1HIO());
    f32 cx = static_cast<f32>(kg1InitialValues().mCenter.x),
        ez = static_cast<f32>(kg1InitialValues().mEye.z),
        ex = static_cast<f32>(kg1InitialValues().mEye.x);
    f32 cy = static_cast<f32>(kg1InitialValues().mCenter.y);
    kg1CameraEye().x = ex;
    kg1CameraCenter().x = cx;
    kg1CameraCenter().y = cy;
    f32 ey = static_cast<f32>(kg1InitialValues().mEye.y),
        cz = static_cast<f32>(kg1InitialValues().mCenter.z);
    kg1CameraEye().y = ey;
    kg1CameraEye().z = ez;
    kg1CameraCenter().z = cz;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02055B64 s16 Kg1Core_calcTimer(be<s16>*)
ext 02515A70 void Kg1Core_destroyCylinder(dCcD_Cyl*,s32)
ext 02515860 void Kg1Core_destroyStts(dCcD_Stts*,s32)
ext 02018034 void Kg1Core_destroyCircle(cM3dGCir_l*,s32)
ext 024EFD9C void Kg1Core_destroyAcch(dBgS_ObjAcch*,s32)
ext 025D50BC void Kg1Core_destroyActor(fopAc_ac_c*,s32)
ext 0254407C s32 Kg1Core_startCheck(dEvent_manager_c*,s16)
ext 025440C8 s32 Kg1Core_endCheck(dEvent_manager_c*,s16)
ext 026066C4 void* Kg1Core_getResource(void*,SafeString*,s32)
ext 0273AA24 void Kg1Core_assert(const char*,u32,const char*)
ext 025E789C s32 Kg1Core_initBtp(mDoExt_btpAnm*,J3DModelData*,J3DAnmTexPattern*,u32,s32,f32,s32,s32,u32,s32)
ext 02514F44 void Kg1Core_cameraStart(dCamera_c*)
ext 02514F88 void Kg1Core_cameraSet(dCamera_c*,cXyz*,cXyz*,s32,f32)
ext 02515048 void Kg1Core_cameraReset(dCamera_c*)
ext 02515280 void Kg1Core_cameraTrim(dCamera_c*,s32)
ext 025E7B3C void Kg1Core_entryBtp(mDoExt_btpAnm*,J3DModelData*,s32)
ext 025E4F64 mDoExt_McaMorf* Kg1Core_newMorf(void*,J3DModelData*,void*,void*,J3DAnmTransform*,s32,s32,s32,f32,u32,u32,u32,u32)
ext 027DF9B0 s8 Kg1Core_jointIndex(JUTNameTab*,const char*)
ext 028EFFD0 void Kg1Core_constructArray(void*,u32,u32,u32)
ext 028F026C void Kg1Core_registerGlobal(void*)
ext 028ED6F8 void Kg1Core_initAngle(be<u8>*)
ext 028EAB2C void Kg1Core_initVector(be<u8>*)
ext 025F0A10 s32 Kg1Core_createChild(const char*,Kg1HIO*)
ext 02543F10 s16 Kg1Core_eventIndex(dEvent_manager_c*,const char*,u32)
ext 02265E24 void Kg1Core_setAnm(kg1_core::daNpc_Kg1_c*)
ext 025A1458 void Kg1Core_constructNpc(kg1_core::daNpc_Kg1_c*)
ext 025D63E8 s32 Kg1Core_entryHeap(kg1_core::daNpc_Kg1_c*,u32,u32)
ext 027F7878 Kg1JointResource* Kg1Core_jointResource(J3DNode*)
ext 025F19F8 void Kg1Core_XYZrotM(Mtx34*,s16,s16,s16)
ext 025B8B68 void Kg1Core_onEventBit(dSv_event_c*,u32)
ext 025B8B94 u32 Kg1Core_isEventBit(dSv_event_c*,u32)
ext 0258839C s32 Kg1Core_getDays()
str 1001BF4C "Kg"
str 1001BCBC "d_a_npc_kg1.cpp" in 0226586C
str 1001BCCC "m_eye_tex_pattern != (0)"
str 1001BCEC "head"
str 1001BCF4 "d_a_npc_kg1.cpp" in 02265978
str 1001BD10 "m_jnt.getHeadJntNum() >= 0"
str 1001BD04 "backbone2"
str 1001BD2C "m_jnt.getBackboneJntNum() >= 0"
str 1001BE6C "Auc0"
str 1001BE74 "Squid-Hunt" renamed
str 1001BE84 "MINIGAME_START"
str 1001BE94 "MINIGAME_END"
str 1001BEA4 "KAISEN_GETITEM"
*/
