/**
 * d_a_npc_btsw2.cpp (WWHD)
 * NPC - traveling merchant Btsw2.
 *
 * Written from the WWHD code, verified against cking.rpx.
 */
#include "d/actor/d_a_npc_btsw2.h"

const gc_tu_data gc_tu = {.safeString = 0x10018A00};
const char daNpc_Btsw2_c::m_arc_name[] = "Btsw";

static daNpc_Btsw2_HIO_c l_HIO;
static Btsw2HeaderZero l_headerZero;
static Btsw2DestructorRecords l_headerRecords;
static u8 l_headerByte;
static u8 l_headerOtherByte;
static f32 l_minAngle;
static f32 l_maxAngle;
static cXyz a_att_pos_offst;
static cXyz a_eye_pos_offst;
static u32 a_att_pos_guard;
static u32 a_eye_pos_guard;
static Btsw2SinCosTable l_sinCosTable;
static const f32 l_zero = 0.0f;
static const f32 l_eyeOffsetX = 10.0f;
static const f32 l_eyeOffsetY = 20.0f;
static const f32 l_one = 1.0f;
static const f32 l_gravity = -30.0f;
static const f32 l_attentionHeight = 40.0f;
static const f32 l_attentionDistance = 200.0f;
static const f32 l_moveSpeed = 0.8f;
static const f32 l_moveAcceleration = 0.95f;
static const f32 l_minAngleInitial = -3.1415927410125732f;
static const f32 l_maxAngleInitial = 3.1415927410125732f;
static const f32 a_morf_frame_tbl[11] = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
static const s32 l_bck_ix_tbl[11] = {15, 16, 13, 14, 9, 12, 12, 17, 8, 10, 11};
static const f32 a_play_speed_tbl[11] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const s32 a_play_mode_tbl[11] = {2, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0};
static const u32 l_btp_ix_tbl[1] = {36};

static Btsw2CylinderSource l_cyl_src;
static Btsw2ArchiveName l_itemArc;
static BOOL nodeCallBack(void*, int);
static BOOL CallbackCreateHeap(daNpc_Btsw2_c*);

static u32 address(daNpc_Btsw2_c* actor) {
    return gabi::ea(actor);
}

static void* btsw2_resource(const char* archive, u32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(archive);
    key->__vtbl = gc_tu.safeString;
    u32 manager = gabi::ea(dComIfG_resControl());
    return Btsw2Resource(gabi::at<void>(manager), key.get(), id);
}
static s32 btsw2_joint(void* modelData, const char* name) {
    void* jointNames = Btsw2JointNames(modelData);
    u32 names = gabi::ea(jointNames);
    u32 displacement = gabi::at<Btsw2NameBlock>(names)->mTableOffset;
    return Btsw2NameIndex(gabi::at<void>(displacement ? names + 0x10 + displacement : 0), name);
}
static void btsw2_setCallback(u32 data, s8 joint) {
    u32 index = u16(s16(joint));
    u32 count = gabi::at<J3DModelData>(data)->mJointNum;
    u32 array = gabi::at<J3DModelData>(data)->mJointNodes;
    u32 selected = index < count ? array + index * 0x1C : array;
    gabi::at<J3DJoint>(selected)->mCallBack = gabi::guest_fn((const void*)nodeCallBack);
}

void* daNpc_Btsw2_HIO_construct(void* object);

/* 0221C39C */
BOOL nodeCallBack(void* node, int phase) {
    if (phase != 0)
        return TRUE;
    u32 model = gabi::ea(j3dSys.getModel());
    u32 actor = gabi::at<J3DModel>(model)->mUserArea;
    if (!actor)
        return TRUE;
    u32 initialized = a_att_pos_guard;
    f32 zero = l_zero;
    if (!initialized) {
        a_att_pos_offst.x = zero;
        a_att_pos_offst.z = zero;
        a_att_pos_guard = 1;
        a_att_pos_offst.y = zero;
    }
    if (!a_eye_pos_guard) {
        f32 x = l_eyeOffsetX;
        a_eye_pos_guard = 1;
        f32 y = l_eyeOffsetY;
        a_eye_pos_offst.x = x;
        a_eye_pos_offst.y = y;
        a_eye_pos_offst.z = zero;
    }
    void* jointObject = Btsw2JointObject();
    u16 joint = static_cast<Btsw2JointData*>(jointObject)->mJointNo;
    u32 matrices = gabi::at<J3DModel>(model)->mpMtxBuffer.v.get();
    u16 flags = gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags;
    u32 array = gabi::at<J3DMtxBuffer_hd>(matrices)->mpAnmMtx;
    gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    PSMTXCopy(gabi::at<Mtx34>(array + u32(joint) * 48), mDoMtx_stack_c::get());
    s8 head = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mHeadJntNum;
    if (u32(joint) == u32(s32(head))) {
        PSMTXMultVec(mDoMtx_stack_c::get(), &a_att_pos_offst, &gabi::at<daNpc_Btsw2_c>(actor)->mAttPos);
        gabi::Local<Mtx34> rotation;
        PSMTXCopy(mDoMtx_stack_c::get(), rotation.get());
        f32 x = rotation->m[0][3];
        f32 z = rotation->m[2][3];
        rotation->m[2][3] = zero;
        f32 y = rotation->m[1][3];
        rotation->m[1][3] = zero;
        rotation->m[0][3] = zero;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
        s16 neck = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mAngles[0][1];
        s16 facing = gabi::at<daNpc_Btsw2_c>(actor)->current.angle.y;
        mDoMtx_YrotM(mDoMtx_stack_c::get(), s16(u16(neck) + u16(facing)));
        s16 pitch = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mAngles[0][0];
        mDoMtx_XrotM(mDoMtx_stack_c::get(), s16(-s32(pitch)));
        facing = gabi::at<daNpc_Btsw2_c>(actor)->current.angle.y;
        mDoMtx_YrotM(mDoMtx_stack_c::get(), s16(-s32(facing)));
        PSMTXConcat(mDoMtx_stack_c::get(), rotation.get(), mDoMtx_stack_c::get());
        PSMTXMultVec(mDoMtx_stack_c::get(), &a_eye_pos_offst, &gabi::at<daNpc_Btsw2_c>(actor)->eyePos);
    } else {
        s8 back = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mBackboneJntNum;
        if (u32(joint) == u32(s32(back))) {
            s16 pitch = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mAngles[1][1];
            mDoMtx_XrotM(mDoMtx_stack_c::get(), pitch);
            s16 roll = gabi::at<daNpc_Btsw2_c>(actor)->m_jnt.mAngles[1][0];
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), s16(-s32(roll)));
        }
    }
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    matrices = gabi::at<J3DModel>(model)->mpMtxBuffer.v.get();
    flags = gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags;
    array = gabi::at<J3DMtxBuffer_hd>(matrices)->mpAnmMtx;
    gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    mtx_copy(gabi::at<Mtx34>(array + u32(joint) * 48), mDoMtx_stack_c::get());
    return TRUE;
}

/* 0221C68C */
BOOL daNpc_Btsw2_c::initTexPatternAnm(u8 modify) {
    u32 morf = gabi::ea(mpMorf.get());
    u32 model = gabi::at<mDoExt_McaMorf>(morf)->mpModel.v.get();
    // HD: this lookup always uses the first texture-pattern resource.
    u32 resource = l_btp_ix_tbl[0];
    u32 modelData = gabi::at<J3DModel>(model)->mModelData.v.get();
    u32 manager = gabi::ea(dComIfG_resControl());
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_str(m_arc_name);
    key->__vtbl = gc_tu.safeString;
    void* pattern = Btsw2Resource(gabi::at<void>(manager), key.get(), resource);
    m_btp = pattern;
    if (!pattern) {
        JUT_ASSERT_fail("d_a_npc_btsw2.cpp", 0x11A, "m_btp != (0)");
        pattern = m_btp.get();
    }
    if (!Btsw2BtpInit(&mBtpAnm, gabi::at<void>(modelData), pattern, 1, 2, 0, -1, modify, 1.0f))
        return FALSE;
    mBtpFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}

/* 0221CEF8 */
void daNpc_Btsw2_c::playTexPatternAnm() {
    if (cLib_calcTimer(&mBlinkTimer))
        return;
    u32 pattern = gabi::ea(m_btp.get());
    u32 vtable = gabi::at<Btsw2TexturePattern>(pattern)->__vtbl;
    u32 target = gabi::at<Btsw2TextureVtable>(vtable)->mpGetFrameMax;
    s32 frames = gabi::call<s32>(target, gabi::at<void>(pattern));
    u8 frame = mBtpFrame;
    if (s32(frame) < frames) {
        frame = mBtpFrame;
        mBtpFrame = u8(frame + 1);
        return;
    }
    pattern = gabi::ea(m_btp.get());
    vtable = gabi::at<Btsw2TexturePattern>(pattern)->__vtbl;
    target = gabi::at<Btsw2TextureVtable>(vtable)->mpGetFrameMax;
    frames = gabi::call<s32>(target, gabi::at<void>(pattern));
    frame = mBtpFrame;
    mBtpFrame = u8(u32(frame) - u32(frames));
    f32 random = cM_rndF(100.0f);
    mBlinkTimer = s16(gabi::ftoi(gabi::fadds_ppc(random, 30.0f)));
}

/* 0221D410 */
void daNpc_Btsw2_c::setAnm(s8 animation) {
    s8 current = m745;
    if (animation == current || current == -1)
        return;
    s8 mode = m747;
    m745 = animation;
    u32 animationOffset = u32(s32(animation)) * 4;
    u32 modeOffset = u32(s32(mode)) * 4;
    u32 morf = gabi::ea(mpMorf.get());
    f32 morph = a_morf_frame_tbl[(s32)modeOffset / 4];
    s32 resource = l_bck_ix_tbl[(s32)animationOffset / 4];
    f32 speed = a_play_speed_tbl[(s32)modeOffset / 4];
    s32 loop = a_play_mode_tbl[(s32)modeOffset / 4];
    dNpc_setAnm(gabi::at<mDoExt_McaMorf>(morf), loop, morph, speed, resource, -1, "Btsw");
    current = m745;
    m_jnt.mbHeadLock = current == 7 || current == 8;
}

/* 0221D4D0 */
bool daNpc_Btsw2_c::chkAttention(cXyz* position, s16 facing) {
    // HD: preserve the verified pointer argument for the by-value GameCube position.
    u32 play = dComIfGp_ea();
    u32 player = gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
    f32 z = position->z;
    f32 playerZ = gabi::at<fopAc_ac_c>(player)->current.pos.z;
    f32 playerX = gabi::at<fopAc_ac_c>(player)->current.pos.x;
    f32 dz = gabi::fsubs_ppc(playerZ, z);
    f32 x = position->x;
    f32 range = l_HIO.mNpc.mMaxAttnDistXZ;
    f32 dx = gabi::fsubs_ppc(playerX, x);
    f32 squareZ = gabi::fmuls_ppc(dz, dz);
    f32 square = gabi::fmadds(dx, dx, squareZ);
    s32 angleRange = l_HIO.mNpc.mMaxAttnAngleY;
    f32 distance = std_sqrtf(square);
    s16 angle = cM_atan2s(dx, dz);
    if (mHasAttention) {
        range = gabi::fadds_ppc(range, 40.0f);
        angleRange += 0x71C;
    }
    s32 delta = s16(u16(angle) - u16(facing));
    s32 absolute = delta < 0 ? -delta : delta;
    return angleRange > absolute && range > distance;
}

/* 0221CFFC */
void daNpc_Btsw2_c::eventOrder() {
    s8 state = m746;
    if (state == 1 || state == 2) {
        s8 orderedState = m746;
        u16 condition = eventInfo.mCondition;
        eventInfo.mCondition = condition | dEvtCmd_INTALK_e;
        if (orderedState == 1)
            fopAcM_orderSpeakEvent(this);
    }
}

/* 0221CFBC */
void daNpc_Btsw2_c::checkOrder() {
    // HD: only the talk command is tested here.
    if (eventInfo.mCommand != dEvtCmd_INTALK_e)
        return;
    s8 state = m746;
    if (state == 1 || state == 2) {
        m746 = 0;
        m71E = 1;
    }
}

/* 0221D5FC */
void daNpc_Btsw2_c::anmAtr(u16 messageStatus) {
    u32 play = dComIfGp_ea();
    u8 requested = gabi::at<dComIfG_inf_c>(play)->play.mMesgAnime;
    if (requested <= 6)
        setAnm(requested);
    u32 morf = gabi::ea(mpMorf.get());
    s16 end = gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mEnd;
    f32 frame = f32(end) - 1.0f;
    if (gabi::at<mDoExt_McaMorf>(morf)->checkFrame(frame)) {
        s8 animation = m745;
        if (animation == 4 || animation == 6)
            setAnm(1);
    }
    play = dComIfGp_ea();
    gabi::at<dComIfG_inf_c>(play)->play.mMesgAnime = 255;
}

/* 0221D6B4 */
u32 daNpc_Btsw2_c::getMsg() {
    if (!dComIfGs_isEventBit(0x3102)) {
        dComIfGs_onEventBit(0x3102);
        return 0x1AB0;
    }
    return dKy_daynight_check() ? 0x1AB2 : 0x1AB1;
}

/* 0221D724 */
u16 daNpc_Btsw2_c::next_msgStatus(u32* message) {
    return fopMsgStts_MSG_ENDS_e;
}

/* 0221DAB0 */
void daNpc_Btsw2_c::setAttention() {
    f32 z = mAttPos.z;
    f32 y = mAttPos.y;
    f32 height = l_HIO.mNpc.mAttnYOffset;
    f32 x = mAttPos.x;
    attention_info.position.z = z;
    attention_info.position.x = x;
    attention_info.position.y = gabi::fadds_ppc(y, height);
}

/* 0221DAD8 */
void daNpc_Btsw2_c::lookBack() {
    f32 zero = l_zero;
    f32 x = zero, y = zero, z = zero;
    int headOnly = 1;
    s8 mode = m747;
    s16 facing = current.angle.y;
    gabi::Local<cXyz> eye;
    cXyz* target = nullptr;
    if (mode == 1 || mode == 2) {
        bool attention;
        if (mode == 2) {
            headOnly = 0;
            m_jnt.mbTrn = 1;
            attention = mHasAttention;
            if (!attention) {
                gabi::Local<cXyz> playerEye;
                f32 offset = l_HIO.mNpc.m04;
                Btsw2PlayerEye(playerEye.get(), offset);
                eye->copy(*playerEye.get());
                s16 desired = cLib_targetAngleY(&current.pos, eye.get());
                cLib_addCalcAngleS2(&current.angle.y, desired, 4, 0x1800);
                attention = mHasAttention;
            }
        } else
            attention = mHasAttention;
        if (attention) {
            gabi::Local<cXyz> playerEye;
            f32 offset = l_HIO.mNpc.m04;
            Btsw2PlayerEye(playerEye.get(), offset);
            y = eyePos.y;
            eye->copy(*playerEye.get());
            x = current.pos.x;
            z = current.pos.z;
            target = eye.get();
        }
    }
    gabi::Local<cXyz> selfEye;
    selfEye->x = x;
    selfEye->y = y;
    selfEye->z = z;
    s16 speed = l_HIO.mNpc.mMaxHeadTurnVel;
    Btsw2LookAt(&m_jnt, &current.angle.y, target, selfEye.get(), facing, speed, headOnly);
}

/* 0221CAE8 */
BOOL CallbackCreateHeap(daNpc_Btsw2_c* actor) {
    return Btsw2CallCreateHeap(actor);
}

/* 0221C790 */
BOOL daNpc_Btsw2_c::CreateHeap() {
    u32 a = address(this);
    void* modelData = btsw2_resource("Btsw", 0x1A);
    if (!modelData)
        JUT_ASSERT_fail("d_a_npc_btsw2.cpp", 0x269, "modelData != (0)");
    void* animation = btsw2_resource("Btsw", 0xF);
    // HD: retain the constructor ABI with play speed in the final floating argument.
    void* morf = Btsw2MorfCreate(nullptr, modelData, nullptr, nullptr, animation, 2, 0, -1, 1, 0, 0x80000,
                                 0x15020022, 1.0f);
    mpMorf = static_cast<mDoExt_McaMorf*>(morf);
    if (!morf || !static_cast<mDoExt_McaMorf*>(morf)->mpModel.v.get())
        return FALSE;
    s8 head = s8(btsw2_joint(modelData, "head"));
    m_jnt.mHeadJntNum = head;
    if (head < 0)
        JUT_ASSERT_fail("d_a_npc_btsw2.cpp", 0x27C, "m_jnt.getHeadJntNum() >= 0");
    s8 back = s8(btsw2_joint(modelData, "backbone"));
    m_jnt.mBackboneJntNum = back;
    if (back < 0)
        JUT_ASSERT_fail("d_a_npc_btsw2.cpp", 0x281, "m_jnt.getBackboneJntNum() >= 0");
    m_handL_jnt_num = s8(btsw2_joint(modelData, "handL"));
    m_handR_jnt_num = s8(btsw2_joint(modelData, "handR"));
    // HD: the item models use a second guest copy of the same archive name.
    void* bagData = btsw2_resource(l_itemArc.mName, 0x14);
    void* bag = Btsw2ModelCreate(bagData, 0, 0x11020203);
    mpKabanModel = static_cast<J3DModel*>(bag);
    if (!bag)
        return FALSE;
    void* flyerData = btsw2_resource(l_itemArc.mName, 0x16);
    void* flyer = Btsw2ModelCreate(flyerData, 0, 0x11020203);
    mpTirasiModel = static_cast<J3DModel*>(flyer);
    if (!flyer)
        return FALSE;
    m744 = 0;
    if (!initTexPatternAnm(false))
        return FALSE;
    u32 currentMorf = gabi::ea(mpMorf.get());
    u32 model = gabi::at<mDoExt_McaMorf>(currentMorf)->mpModel.v.get();
    s8 headJoint = m_jnt.mHeadJntNum;
    u32 data = gabi::at<J3DModel>(model)->mModelData.v.get();
    btsw2_setCallback(data, headJoint);
    s8 backJoint = m_jnt.mBackboneJntNum;
    btsw2_setCallback(data, backJoint);
    currentMorf = gabi::ea(mpMorf.get());
    model = gabi::at<mDoExt_McaMorf>(currentMorf)->mpModel.v.get();
    gabi::at<J3DModel>(model)->mUserArea = a;
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, 0, 0);
    return TRUE;
}

/* 0221CAEC */
BOOL daNpc_Btsw2_c::CreateInit() {
    u32 a = address(this);
    u16 z = current.angle.z;
    u16 x = current.angle.x;
    f32 gravity = l_gravity;
    u16 y = current.angle.y;
    m714.x = s16(x);
    m714.y = s16(y);
    s16 index = mCurrActionFunc.i;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    this->gravity = gravity;
    m714.z = s16(z);
    bool same = index == -1 && mCurrActionFunc.d == 0 && mCurrActionFunc.f == btsw2_tu::waitAction;
    if (!same) {
        if (index != 0) {
            s16 adjustment = mCurrActionFunc.d;
            index = mCurrActionFunc.i;
            u32 receiver = a + s32(adjustment);
            mActionStatus = ACTION_ENDING;
            u32 target;
            if (index < 0)
                target = mCurrActionFunc.f;
            else {
                s16 offset = gabi::at<Btsw2PtmfView>(gabi::ea(&mCurrActionFunc))->mVtableOffset;
                u32 vtable = *gabi::at<be<u32>>(receiver + s32(offset));
                target = gabi::at<Btsw2DispatchVtable>(vtable)->mEntries[u32(s32(index))].mpFunction;
            }
            gabi::call<BOOL>(target, gabi::at<void>(receiver), 0);
        }
        mCurrActionFunc.d = 0;
        s16 adjustment = mCurrActionFunc.d;
        mCurrActionFunc.f = btsw2_tu::waitAction;
        mActionStatus = ACTION_STARTING;
        mCurrActionFunc.i = -1;
        u32 target = mCurrActionFunc.f;
        gabi::call<BOOL>(target, gabi::at<void>(a + s32(adjustment)), 0);
    }
    u32 px = reinterpret_cast<Btsw2Words*>(&current.pos)->x;
    u32 py = reinterpret_cast<Btsw2Words*>(&current.pos)->y;
    reinterpret_cast<Btsw2Words*>(&mAttPos)->x = px;
    u32 pz = reinterpret_cast<Btsw2Words*>(&current.pos)->z;
    reinterpret_cast<Btsw2Words*>(&mAttPos)->y = py;
    reinterpret_cast<Btsw2Words*>(&mAttPos)->z = pz;
    mStts.Init(255, 255, this);
    Btsw2CylinderSet(&mCyl, &l_cyl_src);
    mCyl.mStts = &mStts;
    setCollision(60.0f, 150.0f);
    u32 parameters = mParameters;
    s8 room = current.roomNo;
    m724 = 0;
    s8 pathNumber = s8(parameters >> 16);
    mPathNo = pathNumber;
    void* path = Btsw2RoomPath(pathNumber, room);
    mpPath = path;
    if (!path) {
        // HD: assertion text names this field m_move_path.
        JUT_ASSERT_fail("d_a_npc_btsw2.cpp", 0x2DE, "mpPath != NULL");
        path = mpPath.get();
    }
    u16 count = static_cast<Btsw2Path*>(path)->m_num;
    mFinalPathPntIdx = s16(count - 1);
    f32 random = cM_rndF(3.0f);
    m73E = s16(gabi::ftoi(gabi::fadds_ppc(random, 1.0f)));
    random = cM_rndF(300.0f);
    f32 timer = gabi::fadds_ppc(random, 90.0f);
    m735 = 0;
    m736 = 0;
    m742 = s16(gabi::ftoi(timer));
    mPathPntIdx = 0;
    mEventCut.setActorInfo2("Btsw2", this);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB;
    return TRUE;
}

/* 0221DC90 */
void daNpc_Btsw2_c::wait01() {
    if (m71E)
        m747 = 2;
    else
        m746 = 2;
}

/* 0221DCB4 */
void daNpc_Btsw2_c::talk01() {
    if (talk(1) != fopMsgStts_BOX_CLOSED_e)
        return;
    m747 = 1;
    u32 play = dComIfGp_ea();
    u16 flags = gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag;
    gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag = flags | 8;
    s16 waitTimer = m742;
    m71E = 0;
    if (waitTimer == 0)
        m736 = 1;
    setAnm(0);
}

/* 0221D72C */
void daNpc_Btsw2_c::pathMove() {
    s16 timer = m742;
    f32 one = l_one;
    if (timer > 0) {
        timer = s16(timer - 1);
        m742 = timer;
        if (timer == 0) {
            setAnm(7);
            f32 random = cM_rndF(3.0f);
            m73E = s16(gabi::ftoi(gabi::fadds_ppc(random, one)));
        }
        if (s8(m745) == 8) {
            u32 morf = gabi::ea(mpMorf.get());
            s16 end = gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mEnd;
            if (gabi::at<mDoExt_McaMorf>(morf)->checkFrame(gabi::fsubs_ppc(f32(end), one)))
                setAnm(0);
        }
        return;
    }
    s16 finalPoint = mFinalPathPntIdx;
    s16 currentPoint = mPathPntIdx;
    u32 path = gabi::ea(mpPath.get());
    u32 points = gabi::at<Btsw2Path>(path)->m_points;
    s16 nextPoint = currentPoint < finalPoint ? s16(currentPoint + 1) : 0;
    u32 point = points + u32(s32(nextPoint)) * 16;
    f32 y = gabi::at<Btsw2PathPoint>(point)->m_position.y;
    f32 x = gabi::at<Btsw2PathPoint>(point)->m_position.x;
    f32 z = gabi::at<Btsw2PathPoint>(point)->m_position.z;
    gabi::Local<cXyz> destination;
    destination->x = x;
    destination->y = y;
    destination->z = z;
    s16 desired = cLib_targetAngleY(&current.pos, destination.get());
    s16 maxStep = l_HIO.m30;
    s16 divisor = l_HIO.m32;
    cLib_addCalcAngleS2(&current.angle.y, desired, divisor, maxStep);
    gabi::Local<cXyz> direction;
    cXyz_mi(destination.get(), direction.get(), &current.pos);
    f32 baseSpeed = l_HIO.m34;
    speedF = baseSpeed;
    BOOL normalized = Btsw2NormalizeRS(direction.get());
    f32 zero = l_zero;
    if (normalized) {
        u16 facing = current.angle.y;
        Btsw2SinCos* sinCos = &l_sinCosTable.mEntries[u32(facing) >> 3];
        gabi::Local<cXyz> forward;
        forward->y = zero;
        f32 sine = sinCos->mSin;
        f32 cosine = sinCos->mCos;
        forward->x = sine;
        forward->z = cosine;
        f32 dot = PSVECDotProduct(direction.get(), forward.get());
        f32 acceleration = l_HIO.m38;
        f32 speed = speedF;
        speedF = gabi::fmadds(acceleration, dot, speed);
    }
    if (m736) {
        s16 facing = current.angle.y;
        speedF = zero;
        s16 delta = Btsw2DistanceAngle(facing, desired);
        cLib_addCalcAngleS2(&current.angle.y, desired, 4, 0x1800);
        if (delta < 0x10) {
            m736 = 0;
            setAnm(7);
        }
    }
    // HD: this call passes the status object directly.
    Btsw2PosMove(this, &mStts);
    gabi::Local<cXyz> remaining;
    cXyz_mi(destination.get(), remaining.get(), &current.pos);
    f32 dx = remaining->x;
    f32 dz = remaining->z;
    gabi::Local<cXyz> horizontal;
    horizontal->x = dx;
    horizontal->y = zero;
    horizontal->z = dz;
    f32 square = PSVECSquareMag(horizontal.get());
    f32 distance = std_sqrtf(square);
    if (!(distance < 10.0f))
        return;
    timer = m73E;
    mPathPntIdx = nextPoint;
    if (timer > 0) {
        m73E = s16(timer - 1);
        return;
    }
    f32 random = cM_rndF(one);
    s8 animation = random < 0.5f ? 0 : 8;
    setAnm(animation);
    s16 randomRange = l_HIO.m3E;
    random = cM_rndF(f32(randomRange));
    s16 randomBase = l_HIO.m3C;
    m742 = s16(gabi::ftoi(gabi::fadds_ppc(random, f32(randomBase))));
}

/* 0221DD2C */
BOOL daNpc_Btsw2_c::wait_action(void* argument) {
    s8 status = mActionStatus;
    if (status == ACTION_STARTING) {
        u8 current = u8(mActionStatus);
        m747 = 1;
        mActionStatus = s8(u8(current + 1));
        return TRUE;
    }
    if (status == ACTION_ENDING)
        return TRUE;
    s16 facing = current.angle.y;
    s16 head = m_jnt.mAngles[1][1];
    f32 x = current.pos.x;
    f32 y = current.pos.y;
    s16 neck = m_jnt.mAngles[0][1];
    f32 z = current.pos.z;
    gabi::Local<cXyz> position;
    position->x = x;
    position->y = y;
    position->z = z;
    // HD: retain the guest call and its full verified u8 return width.
    u8 attention = Btsw2CallChkAttention(this, position.get(), s16(u16(facing) + u16(neck) + u16(head)));
    s8 mode = m747;
    mHasAttention = attention;
    m746 = 0;
    if (mode == 1) {
        wait01();
        // HD: GHS keeps r3 across the void wait01 leaf.
        Btsw2CallPathMove(this);
    } else if (mode == 2)
        talk01();
    lookBack();
    setAttention();
    return TRUE;
}

/* 0221CD78 */
BOOL daNpc_Btsw2_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Btsw2NpcConstruct(this);
            __vtbl = btsw2_tu::actorVtable;
            mDoExt_btpAnm::ct(&mBtpAnm);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    if (dComIfGs_getEventReg(0xC203) == 3)
        return cPhs_ERROR_e;
    if (!checkItemGet(0x6A, 1))
        return cPhs_ERROR_e;
    s32 phase = dComIfG_resLoad(&mPhs, "Btsw");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CallbackCreateHeap), 0x29E0))
        return cPhs_ERROR_e;
    u32 morf = mpMorf.v;
    u32 model = gabi::at<mDoExt_McaMorf>(morf)->mpModel.v.get();
    cullMtx = model ? model + 0xC8 : 0;
    if (!Btsw2CallCreateInit(this))
        return cPhs_ERROR_e;
    return phase;
}

/* 0221CE9C */
BOOL daNpc_Btsw2_c::_delete() {
    dComIfG_resDelete(&mPhs, "Btsw");
    if (heap.v) {
        u32 morf = mpMorf.v;
        if (morf)
            gabi::at<mDoExt_McaMorf>(morf)->stopZelAnime();
    }
    return TRUE;
}

/* 0221D034 */
BOOL daNpc_Btsw2_c::_execute() {
    u32 a = address(this);
    s16 neckMin = l_HIO.mNpc.mMinHeadX;
    s16 headMax = l_HIO.mNpc.mMaxHeadX;
    s16 backMin = l_HIO.mNpc.mMaxBackboneY;
    s16 backMax = l_HIO.mNpc.mMinBackboneY;
    s16 headMin = l_HIO.mNpc.mMaxHeadY;
    s16 neckMax = l_HIO.mNpc.mMinBackboneX;
    s16 turnMin = l_HIO.mNpc.mMinHeadY;
    s16 turnMax = l_HIO.mNpc.mMaxBackboneX;
    s16 turnStep = l_HIO.mNpc.mMaxTurnStep;
    m_jnt.setParam(turnMax, backMin, neckMax, backMax, headMax, headMin, neckMin, turnMin, turnStep);
    Btsw2CallPlayTexPattern(this);
    u32 morf = gabi::ea(mpMorf.get());
    gabi::at<mDoExt_McaMorf>(morf)->play(&eyePos, 0, 0);
    checkOrder();
    if (!mEventCut.cutProc()) {
        s16 index = mCurrActionFunc.i;
        s16 adjustment = mCurrActionFunc.d;
        u32 receiver = a + s32(adjustment);
        u32 target;
        if (index < 0)
            target = mCurrActionFunc.f;
        else {
            s16 vtableOffset = gabi::at<Btsw2PtmfView>(gabi::ea(&mCurrActionFunc))->mVtableOffset;
            u32 vtable = *gabi::at<be<u32>>(receiver + s32(vtableOffset));
            target = gabi::at<Btsw2DispatchVtable>(vtable)->mEntries[u32(s32(index))].mpFunction;
        }
        gabi::call<BOOL>(target, gabi::at<void>(receiver), 0);
    }
    eventOrder();
    u32 play = dComIfGp_ea();
    Btsw2CorrectPosition(&mObjAcch, &gabi::at<dComIfG_inf_c>(play)->play.mBgS);
    play = dComIfGp_ea();
    s8 room = Btsw2GroundRoom(&gabi::at<dComIfG_inf_c>(play)->play.mBgS,
                              static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mRoomNo = room;
    play = dComIfGp_ea();
    u8 color = Btsw2GroundColor(&gabi::at<dComIfG_inf_c>(play)->play.mBgS,
                                static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    morf = gabi::ea(mpMorf.get());
    f32 x = current.pos.x;
    f32 y = current.pos.y;
    f32 z = current.pos.z;
    tevStr.mEnvrIdxOverride = color;
    u32 model = gabi::at<mDoExt_McaMorf>(morf)->mpModel.v.get();
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 rotation = current.angle.y;
    mDoMtx_YrotM(mDoMtx_stack_c::get(), rotation);
    mtx_copy(&gabi::at<J3DModel>(model)->mBaseTransformMtx, mDoMtx_stack_c::get());
    setCollision(60.0f, 150.0f);
    return TRUE;
}

/* 0221D214 */
BOOL daNpc_Btsw2_c::_draw() {
    // HD: this draw path omits the GameCube shadow submission.
    u32 morf = gabi::ea(mpMorf.get());
    u32 model = gabi::at<mDoExt_McaMorf>(morf)->mpModel.v.get();
    u32 modelData = gabi::at<J3DModel>(model)->mModelData.v.get();
    dScnKy_env_light_c* environment = dKy_getEnvlight();
    settingTevStruct(environment, 0, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    setLightTevColorType(environment, gabi::at<J3DModel>(model), &tevStr);
    environment = dKy_getEnvlight();
    u32 bag = gabi::ea(mpKabanModel.get());
    setLightTevColorType(environment, gabi::at<J3DModel>(bag), &tevStr);
    environment = dKy_getEnvlight();
    u32 flyer = gabi::ea(mpTirasiModel.get());
    setLightTevColorType(environment, gabi::at<J3DModel>(flyer), &tevStr);
    u8 frame = mBtpFrame;
    Btsw2BtpEntry(&mBtpAnm, gabi::at<void>(modelData), frame);
    morf = gabi::ea(mpMorf.get());
    gabi::at<mDoExt_McaMorf>(morf)->updateDL();
    s8 joint = m_handL_jnt_num;
    u32 matrices = gabi::at<J3DModel>(model)->mpMtxBuffer.v.get();
    u32 matrixArray = gabi::at<J3DMtxBuffer_hd>(matrices)->mpAnmMtx;
    u16 flags = gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags;
    bag = gabi::ea(mpKabanModel.get());
    gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    mtx_copy(&gabi::at<J3DModel>(bag)->mBaseTransformMtx,
             gabi::at<Mtx34>(matrixArray + u32(s32(joint) * 48)));
    joint = m_handR_jnt_num;
    matrices = gabi::at<J3DModel>(model)->mpMtxBuffer.v.get();
    matrixArray = gabi::at<J3DMtxBuffer_hd>(matrices)->mpAnmMtx;
    flags = gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags;
    flyer = gabi::ea(mpTirasiModel.get());
    gabi::at<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    mtx_copy(&gabi::at<J3DModel>(flyer)->mBaseTransformMtx,
             gabi::at<Mtx34>(matrixArray + u32(s32(joint) * 48)));
    bag = gabi::ea(mpKabanModel.get());
    Btsw2ModelUpdateDL(gabi::at<void>(bag), 0);
    flyer = gabi::ea(mpTirasiModel.get());
    Btsw2ModelUpdateDL(gabi::at<void>(flyer), 0);
    gabi::at<Btsw2PatternBinding>(modelData)->mpPattern = 0;
    s16 rotation = current.angle.y;
    dSnap_RegistFig(0x98, this, &current.pos, rotation, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 0221CE98 */
BOOL daNpc_Btsw2_Create(daNpc_Btsw2_c* actor) {
    return actor->_create();
}

/* 0221CEF4 */
BOOL daNpc_Btsw2_Delete(daNpc_Btsw2_c* actor) {
    return actor->_delete();
}

/* 0221D210 */
BOOL daNpc_Btsw2_Execute(daNpc_Btsw2_c* actor) {
    return actor->_execute();
}

/* 0221D40C */
BOOL daNpc_Btsw2_Draw(daNpc_Btsw2_c* actor) {
    return actor->_draw();
}

/* 0221DFE4 */
BOOL daNpc_Btsw2_IsDelete(daNpc_Btsw2_c* actor) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 0221DE30 */
void* daNpc_Btsw2_HIO_construct(void* object) {
    if (!object)
        object = operator_new(0x40);
    if (!object)
        return nullptr;
    u32 a = gabi::ea(object);
    gabi::at<daNpc_Btsw2_HIO_c>(a)->__vtbl = btsw2_tu::hioVtable;
    Btsw2NpcHioConstruct(&gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc);
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMinBackboneX = 0;
    f32 zero = l_zero;
    f32 damping = l_moveAcceleration;
    f32 speed = l_moveSpeed;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNo = -1;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxBackboneX = 0;
    f32 attentionHeight = l_attentionHeight;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxTurnStep = 1000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxHeadY = 8000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.m22 = 0;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMinHeadY = -8000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mAttnYOffset = attentionHeight;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m34 = speed;
    f32 attentionDistance = l_attentionDistance;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMinHeadX = -3000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m32 = 5;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m38 = damping;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.m04 = zero;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxBackboneY = 8000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxAttnDistXZ = attentionDistance;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m3E = 90;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m30 = 600;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxAttnAngleY = 0x2000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxHeadTurnVel = 1600;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->m3C = 90;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMinBackboneY = -8000;
    gabi::at<daNpc_Btsw2_HIO_c>(a)->mNpc.mMaxHeadX = 8000;
    return object;
}

/* 0221DF30 */
void daNpc_Btsw2_staticInit() {
    l_headerZero.mWords[2] = 0;
    l_headerZero.mWords[0] = 0;
    l_headerZero.mWords[3] = 0;
    l_headerZero.mWords[1] = 0;
    Btsw2DestructorChain(&l_headerRecords.mRecords[0]);
    f32 minimum = l_minAngleInitial;
    f32 maximum = l_maxAngleInitial;
    l_minAngle = minimum;
    l_maxAngle = maximum;
    Btsw2HeaderByteConstruct(&l_headerByte);
    Btsw2DestructorChain(&l_headerRecords.mRecords[1]);
    Btsw2HeaderOtherByteConstruct(&l_headerOtherByte);
    Btsw2DestructorChain(&l_headerRecords.mRecords[2]);
    daNpc_Btsw2_HIO_construct(&l_HIO);
}

/* 0221DFD0 */
void daNpc_Btsw2_SafeString_delete(void* object, int flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 0221DFEC */
void daNpc_Btsw2_destructor(daNpc_Btsw2_c* actor, int flags) {
    if (!actor)
        return;
    Btsw2CylinderDestroy(&actor->mCyl, 2);
    Btsw2StatusDestroy(&actor->mStts, 2);
    Btsw2CircleDestroy(&actor->mAcchCir.m_cir, 2);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = btsw2_tu::acchGroupVtable;
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = btsw2_tu::acchPolyVtable;
    Btsw2AcchDestroy(&actor->mObjAcch, 0);
    Btsw2ActorDestroy(actor, 0);
    if (flags & 1)
        operator_delete(actor);
}

/* 0221E088 */
void daNpc_Btsw2_SafeString_assureTerminated(SafeString* string) {}
/* WWHD glue
str 10018BB0 "Btsw"
data l_itemArc 10018AC4
str 10018A84 "d_a_npc_btsw2.cpp" in 0221C68C
str 10018ACC "d_a_npc_btsw2.cpp" in 0221C790
str 10018B5C "d_a_npc_btsw2.cpp" in 0221CAEC
str 10018A98 "m_btp != (0)"
str 10018AE0 "modelData != (0)"
str 10018AF4 "m_jnt.getHeadJntNum() >= 0"
str 10018B1C "m_jnt.getBackboneJntNum() >= 0"
str 10018B70 "mpPath != NULL" renamed
str 10018B54 "Btsw2"
str 10018ABC "head"
str 10018B10 "backbone"
str 10018AAC "handL"
str 10018AB4 "handR"
data l_HIO 10466AB8
data l_headerZero 10466AF8
data l_headerRecords 101BD8A8
data l_headerByte 10466AB4
data l_headerOtherByte 10466AB5
data l_minAngle 10466AAC
data l_maxAngle 10466AB0
data a_att_pos_offst 10466B08
data a_eye_pos_offst 10466B14
data a_att_pos_guard 10466B20
data a_eye_pos_guard 10466B24
data l_sinCosTable 104A44F8
data l_zero 10018A74
data l_eyeOffsetX 10018A78
data l_eyeOffsetY 10018A7C
data l_one 10018A80
data l_gravity 10018B3C
data l_attentionHeight 10018B88
data l_attentionDistance 10018B9C
data l_moveSpeed 10018BA0
data l_moveAcceleration 10018BA4
data l_minAngleInitial 10018BA8
data l_maxAngleInitial 10018BAC
data a_morf_frame_tbl 101BD850
data l_bck_ix_tbl 10018A48
data a_play_speed_tbl 101BD87C
data a_play_mode_tbl 101BD824
data l_btp_ix_tbl 100189FC
data l_cyl_src 101BD7E0
ext 02556D14 BOOL dKy_daynight_check()
ext 02515A70 void Btsw2CylinderDestroy(void*, s32)
ext 02515860 void Btsw2StatusDestroy(void*, s32)
ext 02018034 void Btsw2CircleDestroy(void*, s32)
ext 024EFD9C void Btsw2AcchDestroy(void*, s32)
ext 025D50BC void Btsw2ActorDestroy(void*, s32)
ext 025A1458 void Btsw2NpcConstruct(void*)
ext 0221CAEC BOOL Btsw2CallCreateInit(void*)
ext 0221C790 BOOL Btsw2CallCreateHeap(void*)
ext 0259DA18 void Btsw2NpcHioConstruct(void*)
ext 028F026C void Btsw2DestructorChain(void*)
ext 028ED6F8 void Btsw2HeaderByteConstruct(void*)
ext 028EAB2C void Btsw2HeaderOtherByteConstruct(void*)
ext 025E7B3C void Btsw2BtpEntry(void*, void*, u8)
ext 025E2DE0 void Btsw2ModelUpdateDL(void*, s32)
ext 025BEBB8 void dSnap_RegistFig(s32, void*, void*, s16, f32, f32, f32)
ext 0221CEF8 void Btsw2CallPlayTexPattern(void*)
ext 024F08A8 void Btsw2CorrectPosition(void*, void*)
ext 024EF130 s8 Btsw2GroundRoom(void*, void*)
ext 024EEEB8 u8 Btsw2GroundColor(void*, void*)
ext 028E93CC void PSMTXTrans(Mtx34*, f32, f32, f32)
ext 0259D54C void Btsw2PlayerEye(cXyz*, f32)
ext 0259DED0 void Btsw2LookAt(void*, void*, cXyz*, cXyz*, s16, s16, s32)
ext 0221D4D0 u8 Btsw2CallChkAttention(void*, cXyz*, s16)
ext 0221D72C void Btsw2CallPathMove(void*)
ext 02516518 void Btsw2CylinderSet(void*, void*)
ext 025AAF88 void* Btsw2RoomPath(s32, s32)
ext 026066C4 void* Btsw2Resource(void*, SafeString*, u32)
ext 025E789C BOOL Btsw2BtpInit(void*, void*, void*, s32, s32, s32, s32, u8, f32)
ext 027F68FC void* Btsw2JointNames(void*)
ext 027DF9B0 s32 Btsw2NameIndex(void*, const char*)
ext 025E4F64 void* Btsw2MorfCreate(void*, void*, void*, void*, void*, s32, s32, s32, s32, s32, u32, u32, f32)
ext 025E38E0 void* Btsw2ModelCreate(void*, s32, u32)
ext 027F7878 void* Btsw2JointObject()
ext 028E8F64 void PSMTXMultVec(const Mtx34*, const cXyz*, cXyz*)
ext 028E9108 void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*)
ext 028E8F44 f32 PSVECDotProduct(const cXyz*, const cXyz*)
ext 0200FAAC s16 Btsw2DistanceAngle(s16, s16)
ext 025D6870 void Btsw2PosMove(void*, void*)
ext 0201B47C BOOL Btsw2NormalizeRS(cXyz*)
*/
