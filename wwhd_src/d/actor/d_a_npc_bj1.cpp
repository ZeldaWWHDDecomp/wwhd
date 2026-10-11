/**
 * d_a_npc_bj1.cpp (WWHD)
 * NPC - the Koroks of the Forest Haven
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_bj1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx. Names follow the GameCube symbols;
 * HD field names describe their verified use.
 */
#include "d/actor/d_a_npc_bj1.h"

const gc_tu_data gc_tu = {.safeString = 0x10016B84};

#define BJ1_ASSERT_FILE "d_a_npc_bj1.cpp"

static BOOL bj1_moveStateIsZero(daNpc_Bj1_c* actor);

static Bj1FigureColors l_figureColors;
static Bj1PrivateCutNames l_privateCutNames;
static Bj1DebugDefaults l_debugDefaults;
static Bj1DebugColors l_debugColors;
static Bj1GuestWord l_firstDebugGuard;
static Bj1GuestWord l_secondDebugGuard;
static Bj1GuestWord l_thirdDebugGuard;
static Bj1GuestWord l_fourthDebugGuard;
static Bj1ConstructorState l_constructorState;
static Bj1InitialRange l_initialRange;
static Bj1GlobalRegistrations l_globalRegistrations;
static Bj1RuntimeInitializer l_runtimeInitializerA;
static Bj1RuntimeInitializer l_runtimeInitializerB;
static Bj1HIOCollection l_hio;
static Bj1DefaultTable l_parameterDefaults;
using Bj1MessageManagerPtr = gptr<Bj1MessageManager>;
static Bj1MessageManagerPtr l_messageManager;
using Bj1DemoManagerPtr = gptr<void>;
static Bj1DemoManagerPtr l_demoManager;
using Bj1GuestFloat = be<f32>;
static Bj1GuestFloat l_cullUpperXZ;
static Bj1GuestFloat l_cullLowerXZ;
static Bj1GuestFloat l_cullUpperY;
static Bj1GuestFloat l_cullLowerY;
static Bj1GuestFloat l_propellerEndFrame;
static Bj1GuestFloat l_degreesToAngle;
static Bj1GuestFloat l_minimumWalkAnimationSpeed;
static Bj1GuestFloat l_tuneEndFrame;
static Bj1GuestFloat l_waitFacingRange;
static Bj1GuestFloat l_propellerSoundScale;
static Bj1GuestFloat l_unsignedFloatSplit;
static Bj1GuestFloat l_initialLowerValue;
static Bj1GuestFloat l_initialUpperValue;
static Bj1CylinderSource l_cylinderSource;
static Bj1CylinderTable l_cylinderDimensions;

static Bj1ResourceTable l_leftItemResources;
static Bj1ResourceTable l_rightItemResources;
static Bj1ResourceTable l_maskResources;
static Bj1AnimationTable l_animations;
static Bj1StateAnimations l_stateAnimations;
static Bj1AnimationTable l_attributeAnimations;
static Bj1AnimationResources l_animationResources;
static Bj1ScaleTable l_scales;
static Bj1EventNames l_eventNames;
static Bj1StaffNames l_staffNames;
static Bj1HeapSizes l_heapSizes;
// HD: uninitialized views retain the RPX PTMF words; there is one owner for each table.
static Bj1ActionWords l_waitAction;
static Bj1ActionWords l_flightAction;
static Bj1ActionWords l_potAction;
static Bj1ActionWords l_otherAction;

using l_zero_type = be<f32>;
static l_zero_type l_zero;
using l_one_type = be<f32>;
static l_one_type l_one;
using l_jumpHoldFrame_type = be<f32>;
static l_jumpHoldFrame_type l_jumpHoldFrame;
using l_integerBias_type = be<f64>;
static l_integerBias_type l_integerBias;
using l_gravity_type = be<f32>;
static l_gravity_type l_gravity;
using l_playerGoalDistance_type = be<f32>;
static l_playerGoalDistance_type l_playerGoalDistance;
using l_seedScale_type = be<f32>;
static l_seedScale_type l_seedScale;
using l_seedSpeed_type = be<f32>;
static l_seedSpeed_type l_seedSpeed;
using l_seedRise_type = be<f32>;
static l_seedRise_type l_seedRise;
using l_seedRandomRise_type = be<f32>;
static l_seedRandomRise_type l_seedRandomRise;
using l_seedRandomSpeedBias_type = be<f32>;
static l_seedRandomSpeedBias_type l_seedRandomSpeedBias;

using Bj1GuestInt = be<s32>;
static Bj1GuestInt l_jb_num;
static Bj1SearchActors l_jb_actor;
static Bj1GuestInt l_headScaleInitialized;
static cXyz l_headScale;

#define BJ1_ITM_ARC "Bj"

// HD: exact-width private bindings preserve the original register arguments and results.

static inline void* bj1_getIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = gc_tu.safeString;
    key->mStringTop = gabi::guest_addr(arc);
    return Bj1_getResource(dComIfG_resControl(), key.get(), id);
}

static inline void* bj1_getObjectIDRes(s32 id) { return bj1_getIDRes("Bj", id); }

static inline JUTNameTab* J3DModelData_getJointName(J3DModelData* d) { return d->getJointName(); }

static inline s8 JUTNameTab_getIndex(JUTNameTab* tab, const char* name) {
    return (s8)tab->getIndex(name);
}

static inline Mtx34* bj1_anmMtx(J3DModel* model, u32 jntNo) { return model->getAnmMtx(jntNo); }

// HD: ordered word copies quiet signaling NaNs exactly as the original lfs/stfs sequence.
static inline void bj1_mtxCopy(Mtx34* dstMtx, const Mtx34* srcMtx) {
    auto* dst = reinterpret_cast<be<u32>*>(dstMtx);
    auto* src = reinterpret_cast<const be<u32>*>(srcMtx);
    u32 t[12];
    for (int i = 0; i < 12; i++) {
        t[i] = src[i];
    }
    for (int i = 0; i < 12; i++) {
        u32 v = t[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u)) {
            v |= 0x00400000u;
        }
        dst[i] = v;
    }
}

static inline J3DModel* j3dSys_getModel() { return j3dSys.getModel(); }

static inline u32 jntNo_of(J3DNode* node) {
    // HD: the accessor returns a joint-data record, whose number precedes the callback record.
    return reinterpret_cast<Bj1JointData*>(J3DNode_toJoint(node))->mJointNo;
}

// HD: preserve the virtual deleting-destructor flag and slot through the guest vtable.
static inline void bj1_morfDelete(mDoExt_McaMorf* m) {
    auto* object = reinterpret_cast<Bj1VirtualMorf*>(m);
    gabi::call_ptr(object->mVtable->mDelete, m, 3);
}

static inline s32 bj1_itmTblA(s32 t) { return l_leftItemResources.mIDs[t]; }

static inline s32 bj1_itmTblB(s32 t) { return l_rightItemResources.mIDs[t]; }

static inline s32 bj1_itmTblC(s32 t) { return l_maskResources.mIDs[t]; }

static inline u16 J3DModelData_getJointCount(J3DModelData* d) { return d->getJointNum(); }

static inline void bj1_setJointCallBack(mDoExt_McaMorf* morf, u32 i, J3DJointCallBack cb) {
    J3DModelData* data = morf->mpModel->getModelData();
    // HD: the original bounds check uses the full index before selecting the joint record.
    u32 count = data->mJointNum;
    Bj1JointRecord* joints = gabi::at<Bj1JointRecord>(data->mJointNodes);
    if (i < count) {
        joints += i;
    }
    joints->mCallBack = gabi::guest_fn((const void*)cb);
}

static inline BOOL bj1_eventFlag(u32 flag) {
    return Bj1_isEventBit(&dComIfGs_info()->mSavedata.mEvent, flag);
}

static void bj1_storeMatrixOrdered(Mtx34* destination, const Mtx34* source, const u8* storeOrder) {
    auto* dst = reinterpret_cast<be<u32>*>(destination);
    auto* src = reinterpret_cast<const be<u32>*>(source);
    u32 words[12];
    for (int i = 0; i < 12; ++i) {
        words[i] = src[i];
    }
    // HD: quiet lfs/stfs signaling NaNs after capturing every source word.
    for (int j = 0; j < 12; ++j) {
        u32 i = storeOrder[j], word = words[i];
        if ((word & 0x7F800000u) == 0x7F800000u && (word & 0x003FFFFFu) != 0 &&
            !(word & 0x00400000u)) {
            word |= 0x00400000u;
        }
        dst[i] = word;
    }
}

static Mtx34* bj1_jointMatrix(mDoExt_McaMorf* morf, s8 joint) {
    J3DModel* model = morf->mpModel;
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    Mtx34* matrices = gabi::at<Mtx34>(block->mpAnmMtx);
    u16 flags = block->mFlags;
    block->mFlags = flags | 0x10;
    return &matrices[(u32)(s32)joint];
}

static BOOL bj1_deleteSeedBody(daNpc_Bj1_c* self) {
    for (u32 i = 0; i < 4; ++i) {
        fpc_ProcID id = self->mSeedIDs[i];
        fopAc_ac_c* found = self->searchByID(id);
        if (found != nullptr) {
            self->mSeedIDs[i] = fpcM_ERROR_PROCESS_ID_e;
            fopAcM_delete(found);
        }
    }
    return TRUE;
}

/* 021F2D78 */
static void* searchActor_Jb(void* i_actor, void*) {
    if ((s32)l_jb_num < 0x14 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == fpcNm_JB_e &&
        reinterpret_cast<Bj1Partner*>(i_actor)->mActive != 0) {
        s32 n = (s32)l_jb_num;
        l_jb_num = n + 1;
        l_jb_actor.mActors[n] = static_cast<fopAc_ac_c*>(i_actor);
    }
    return nullptr;
}

/* 021F2FD4 */
static BOOL nodeCallBack_Bj1(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Bj1_c* i_this = static_cast<daNpc_Bj1_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->nodeBj1Control(node, model);
        }
    }
    return TRUE;
}

/* 021F2E04 */
void daNpc_Bj1_c::nodeBj1Control(J3DNode* node, J3DModel* model) {
    /* an unused function-local static cXyz (26, 26, 0) */
    if (l_headScaleInitialized == 0) {
        l_headScale.x = 26.0f;
        l_headScale.z = 0.0f;
        l_headScale.y = 26.0f;
        l_headScaleInitialized = 1;
    }
    u32 jnt = jntNo_of(node);
    Mtx34* calc = mDoMtx_stack_c::get();
    PSMTXCopy(bj1_anmMtx(model, jnt), calc);
    if (jnt == (u32)(s32)mHeadJoint) {
        mDoMtx_XrotM(calc, m_jnt.mAngles[0][1]);
        mDoMtx_ZrotM(calc, (s16)-m_jnt.mAngles[0][0]);
        mDoMtx_stack_c::scaleM(mHeadScaleX, mHeadScaleY, mHeadScaleZ);
        PSMTXMultVec(calc, reinterpret_cast<cXyz*>(&mHeadOffsetX),
                     reinterpret_cast<cXyz*>(&mHeadPosX));
    }
    if (jnt == (u32)(s32)mBackboneJoint) {
        mDoMtx_XrotM(calc, m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(calc, (s16)-m_jnt.mAngles[1][0]);
        mDoMtx_stack_c::scaleM(mBackboneScaleX, mBackboneScaleY, mBackboneScaleZ);
    }
    if (jnt == (u32)(s32)mRightHandJoint) {
        PSMTXCopy(calc, &mPropellerAttachMtx);
    }
    PSMTXCopy(calc, J3DSys::mCurrentMtx);
    bj1_mtxCopy(bj1_anmMtx(model, jnt), calc);
}

/* 021F31FC */
static BOOL nodeCallBack_Prp(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys_getModel();
        daNpc_Bj1_c* i_this = static_cast<daNpc_Bj1_c*>(model->getUserArea());
        if (i_this != nullptr) {
            i_this->nodePrpControl(node, model);
        }
    }
    return TRUE;
}

/* 021F301C */
void daNpc_Bj1_c::nodePrpControl(J3DNode* node, J3DModel* model) {
    u32 jnt = jntNo_of(node);
    Mtx34* calc = mDoMtx_stack_c::get();
    if (jnt == (u32)(s32)mPropellerRootJoint) {
        PSMTXCopy(&mPropellerAttachMtx, calc);
        mDoMtx_stack_c::transM(-1.5f, 4.5f, -4.2f);
        mDoMtx_XYZrotM(calc, -0x4000, -0x5555, 0);
        PSMTXCopy(calc, J3DSys::mCurrentMtx);
        bj1_mtxCopy(bj1_anmMtx(model, jnt), calc);
    }
    if (jnt == (u32)(s32)mPropellerJoint) {
        mDoMtx_YrotS(calc, mPropellerAngle);
        J3DMtxBuffer_hd* block = model->mpMtxBuffer;
        Mtx34* matrices = gabi::at<Mtx34>(block->mpAnmMtx);
        block->mFlags |= 0x10;
        PSMTXConcat(&matrices[jnt], calc, calc);
        PSMTXCopy(calc, J3DSys::mCurrentMtx);
        bj1_mtxCopy(bj1_anmMtx(model, jnt), calc);
    }
}

/* 021F3EF0 */
BOOL daNpc_Bj1_c::init_BJ4_0() {
    if (dComIfGs_isSymbol(2)) {
        return 0;
    }
    if (!Bj1_isEventBit(dComIfGs_getEvent(), 0x1801)) {
        return 0;
    }
    u32 first = l_waitAction.mWords[0];
    u32 second = l_waitAction.mWords[1];
    gabi::Local<Bj1InitActionWords> action;
    action->first = first;
    action->second = second;
    set_action((Bj1Action*)(action.get()), (void*)(0));
    return 1;
}

/* 021F3F8C */
BOOL daNpc_Bj1_c::init_BJ6_0() {
    BOOL ready = dComIfGs_isSymbol(2) == 0;
    if (ready) {
        // Reload saved-global pointer after the first call.
        ready = Bj1_isEventBit(dComIfGs_getEvent(), 0x1801) != 0;
    }
    if (ready) {
        u32 flags = (u32)actor_status;
        actor_status = (flags & ~fopAcStts_NOCULLEXEC_e) | fopAcStts_UNK4000_e;
        u32 word0 = l_flightAction.mWords[0];
        u32 word1 = l_flightAction.mWords[1];

        gabi::Local<Bj1ActionFrame> frame;
        frame->words[0] = word0;
        frame->words[1] = word1;
        (void)set_action((Bj1Action*)(reinterpret_cast<Bj1Action*>(frame->words)),
                         (void*)((void*)nullptr));
    }
    return ready; // Normalized gate result; independent of setter return.
}

/* 021F42B0 */
BOOL daNpc_Bj1_c::init_BJ7_0() {
    BOOL ready = Bj1_isEventBit(dComIfGs_getEvent(), 0x1801) != 0;
    if (ready) {
        setPrtcl_drugPot_1(); // Before either descriptor read.
        u32 word0 = l_potAction.mWords[0];
        u32 word1 = l_potAction.mWords[1];

        gabi::Local<Bj1ActionFrame> frame;
        frame->words[0] = word0;
        frame->words[1] = word1;
        (void)set_action((Bj1Action*)(reinterpret_cast<Bj1Action*>(frame->words)),
                         (void*)((void*)nullptr));
    }
    return ready; // Normalized gate result; independent of setter return.
}

/* 021F4038 */
BOOL daNpc_Bj1_c::init_BJX_0() {
    BOOL ready = dComIfGs_isSymbol(2) == 0;
    if (ready) {
        // Reload saved-global pointer after the first call.
        ready = Bj1_isEventBit(dComIfGs_getEvent(), 0x1801) != 0;
    }
    if (ready) {
        u32 word0 = l_otherAction.mWords[0];
        u32 word1 = l_otherAction.mWords[1];

        gabi::Local<Bj1ActionFrame> frame;
        frame->words[0] = word0;
        frame->words[1] = word1;
        (void)set_action((Bj1Action*)(reinterpret_cast<Bj1Action*>(frame->words)),
                         (void*)((void*)nullptr));
    }
    return ready; // Normalized gate result; independent of setter return.
}

/* 021F40D4 */
BOOL daNpc_Bj1_c::init_BJX_1() {
    BOOL ready = dComIfGs_isSymbol(2) != 0;
    if (ready) {
        u32 word0 = l_otherAction.mWords[0];
        u32 word1 = l_otherAction.mWords[1];

        gabi::Local<Bj1ActionFrame> frame;
        frame->words[0] = word0;
        frame->words[1] = word1;
        (void)set_action((Bj1Action*)(reinterpret_cast<Bj1Action*>(frame->words)),
                         (void*)((void*)nullptr));
    }
    return ready; // Normalized gate result; independent of setter return.
}

/* 021F4D48 */
BOOL daNpc_Bj1_c::createInit() {
    for (u32 i = 0; i < 3; ++i) {
        u32 name = (u32)l_staffNames.mNames[i];
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        u32 result = Bj1_getEventIndex((dEvent_manager_c*)(&scene->play.mEvtManager), name, 0xFF);
        mEventIDs[i] = (u16)result;
    }
    s8 type = (s8)mType;
    attention_info.flags = 10;
    if ((u32)(s32)type > 8) {
        return 0;
    }
    u32 path;
    switch (type) {
    case 0:
    case 1:
    case 7: {
        s8 currentType = (s8)mType;
        u32 params = (u32)mParameters;
        attention_info.distances[1] = 0xAB;
        attention_info.distances[3] = 0xAB;
        Bj1ScaleRecord* row = &l_scales.mRecords[currentType];
        mHeadScaleX = (f32)row->mHeadScale.x;
        mHeadScaleY = (f32)row->mHeadScale.y;
        mHeadScaleZ = (f32)row->mHeadScale.z;
        f32 row3 = (f32)row->mBackboneScale.x;
        mBackboneScaleX = row3;
        mBackboneScaleY = (f32)row->mBackboneScale.y;
        mBackboneScaleZ = (f32)row->mBackboneScale.z;
        f32 row6 = (f32)row->mHeadOffset.x;
        u32 actorX = Bj1_floatBits(current.pos.x);
        mHeadOffsetX = row6;
        f32 row7 = (f32)row->mHeadOffset.y;
        f32 speed = (f32)l_gravity;
        mHeadOffsetY = row7;
        f32 row8 = (f32)row->mHeadOffset.z;
        mLookTargetXBits = actorX;
        mHeadOffsetZ = row8;
        path = (params >> 16) & 0xFF;
        u32 actorZ = Bj1_floatBits(current.pos.z);
        u32 actorY = Bj1_floatBits(current.pos.y);
        mLookTargetZBits = actorZ;
        mPathIndex = (u8)path;
        Bj1_setFloatBits(mLookTargetY, actorY);
        gravity = speed;
        break;
    }
    case 2:
    case 3:
    case 5:
    case 8: {
        s8 currentType = (s8)mType;
        u32 params = (u32)mParameters;
        attention_info.distances[1] = 0xAA;
        attention_info.distances[3] = 0xAA;
        Bj1ScaleRecord* row = &l_scales.mRecords[currentType];
        mHeadScaleX = (f32)row->mHeadScale.x;
        mHeadScaleY = (f32)row->mHeadScale.y;
        mHeadScaleZ = (f32)row->mHeadScale.z;
        f32 row3 = (f32)row->mBackboneScale.x;
        mBackboneScaleX = row3;
        mBackboneScaleY = (f32)row->mBackboneScale.y;
        mBackboneScaleZ = (f32)row->mBackboneScale.z;
        f32 row6 = (f32)row->mHeadOffset.x;
        u32 actorX = Bj1_floatBits(current.pos.x);
        mHeadOffsetX = row6;
        f32 row7 = (f32)row->mHeadOffset.y;
        f32 speed = (f32)l_gravity;
        mHeadOffsetY = row7;
        f32 row8 = (f32)row->mHeadOffset.z;
        mLookTargetXBits = actorX;
        mHeadOffsetZ = row8;
        path = (params >> 16) & 0xFF;
        u32 actorZ = Bj1_floatBits(current.pos.z);
        u32 actorY = Bj1_floatBits(current.pos.y);
        mLookTargetZBits = actorZ;
        mPathIndex = (u8)path;
        Bj1_setFloatBits(mLookTargetY, actorY);
        gravity = speed;
        break;
    }
    case 4: {
        s8 currentType = (s8)mType;
        attention_info.distances[1] = 0xA7;
        attention_info.distances[3] = 0xAB;
        Bj1ScaleRecord* row = &l_scales.mRecords[currentType];
        mHeadScaleX = (f32)row->mHeadScale.x;
        mHeadScaleY = (f32)row->mHeadScale.y;
        mHeadScaleZ = (f32)row->mHeadScale.z;
        f32 row3 = (f32)row->mBackboneScale.x;
        u32 actorZ = Bj1_floatBits(current.pos.z);
        mBackboneScaleX = row3;
        mBackboneScaleY = (f32)row->mBackboneScale.y;
        mBackboneScaleZ = (f32)row->mBackboneScale.z;
        f32 row6 = (f32)row->mHeadOffset.x;
        u32 actorX = Bj1_floatBits(current.pos.x);
        mHeadOffsetX = row6;
        f32 row7 = (f32)row->mHeadOffset.y;
        f32 speed = (f32)l_gravity;
        mHeadOffsetY = row7;
        f32 row8 = (f32)row->mHeadOffset.z;
        mLookTargetXBits = actorX;
        mHeadOffsetZ = row8;
        gravity = speed;
        u32 params = (u32)mParameters;
        u32 actorY = Bj1_floatBits(current.pos.y);
        mLookTargetZBits = actorZ;
        path = (params >> 16) & 0xFF;
        Bj1_setFloatBits(mLookTargetY, actorY);
        mPathIndex = (u8)path;
        break;
    }
    case 6: {
        s8 currentType = (s8)mType;
        u32 params = (u32)mParameters;
        attention_info.distances[1] = 0x5A;
        attention_info.distances[3] = 0x5A;
        u32 actorX = Bj1_floatBits(current.pos.x);
        Bj1ScaleRecord* row = &l_scales.mRecords[currentType];
        mHeadScaleX = (f32)row->mHeadScale.x;
        mHeadScaleY = (f32)row->mHeadScale.y;
        mHeadScaleZ = (f32)row->mHeadScale.z;
        f32 row3 = (f32)row->mBackboneScale.x;
        mBackboneScaleX = row3;
        mBackboneScaleY = (f32)row->mBackboneScale.y;
        mBackboneScaleZ = (f32)row->mBackboneScale.z;
        f32 row6 = (f32)row->mHeadOffset.x;
        u32 actorZ = Bj1_floatBits(current.pos.z);
        mHeadOffsetX = row6;
        f32 row7 = (f32)row->mHeadOffset.y;
        f32 speed = (f32)l_gravity;
        mHeadOffsetY = row7;
        f32 row8 = (f32)row->mHeadOffset.z;
        gravity = speed;
        mHeadOffsetZ = row8;
        mLookTargetXBits = actorX;
        u32 actorY = Bj1_floatBits(current.pos.y);
        mLookTargetZBits = actorZ;
        path = (params >> 16) & 0xFF;
        Bj1_setFloatBits(mLookTargetY, actorY);
        mPathIndex = (u8)path;
        break;
    }
    default:
        return 0;
    }
    if (path != 255) {
        s8 room = (s8)current.roomNo;
        Bj1_setPathInfo((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)), path, room, 1);
        if (gabi::ea(mPath.mPath.get()) == 0) {
            return 0;
        }
        u32 flags = (u32)actor_status;
        actor_status = flags & ~0x80u;
    }
    s8 eventType = (s8)mMessageType;
    u32 eventName = (u32)l_eventNames.mNames[eventType];
    Bj1_setEventActor((dNpc_EventCut_c*)(reinterpret_cast<void*>(&mEventCut2)), eventName,
                      (daNpc_Bj1_c*)(this));
    eventType = (s8)mMessageType;
    mAnimation = 11;
    if ((u32)(s32)eventType > 16) {
        return 0;
    }
    BOOL initialized;
    switch (eventType) {
    case 6:
        initialized = init_BJ4_0();
        break;
    case 10:
        initialized = init_BJ6_0();
        break;
    case 0:
    case 2:
    case 4:
    case 8:
    case 13:
    case 15:
        initialized = init_BJX_0();
        break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 14:
    case 16:
        initialized = init_BJX_1();
        break;
    case 12:
        initialized = init_BJ7_0();
        break;
    default:
        return 0;
    }
    if (!initialized) {
        return 0;
    }
    u16 zAngle = (u16)current.angle.z;
    u16 yAngle = (u16)current.angle.y;
    u16 xAngle = (u16)current.angle.x;
    shape_angle.z = zAngle;
    shape_angle.y = yAngle;
    shape_angle.x = xAngle;
    Bj1_initCollisionStatus((dCcD_Stts*)(reinterpret_cast<void*>(&mStts)), 0xFF, 0xFF,
                            (daNpc_Bj1_c*)(this));
    mCyl.mStts = &mStts;
    Bj1_setCollisionCylinder((dCcD_Cyl*)(reinterpret_cast<void*>(&mCyl)),
                             (const void*)(&l_cylinderSource));
    mDoExt_McaMorf* morf = mpMorf;
    f32 zero = (f32)l_zero;
    ((mDoExt_McaMorf*)(morf))->setMorf(zero);
    setMtx(1);
    return 1;
}

/* 021F4628 */
void daNpc_Bj1_c::setMtx_anmProc() {
    s8 animation = (s8)mAnimation;
    if (animation == 4) {
        if ((s8)mJumpState == 0) {
            return;
        }
        f32 direction = (f32)speed.y, zero = (f32)l_zero;
        mDoExt_McaMorf* morph = mpMorf;
        bool negative = direction < zero;
        f32 boundary = (f32)l_jumpHoldFrame, frame = (f32)morph->mFrameCtrl.mFrame;
        if (negative) {
            if (!(frame < boundary)) {
                return;
            }
            s32 end = (s16)morph->mFrameCtrl.mEnd;
            f64 bias = (f64)l_integerBias;
            f32 converted =
                (f32)(u64_as_f64(0x4330000000000000ull | ((u32)end ^ 0x80000000u)) - bias);
            s16 truncated = (s16)gabi::ftoi(converted);
            f32 newFrame =
                (f32)(u64_as_f64(0x4330000000000000ull | ((u32)(s32)truncated ^ 0x80000000u)) -
                      bias);
            morph->mFrameCtrl.mFrame = newFrame;
        } else if (!(frame < boundary)) {
            morph->mFrameCtrl.mFrame = boundary;
        }
    } else if (animation == 7) {
        if ((s8)mAnmEnded == 0 || (u32)mPotPourEmitter != 0) {
            return;
        }
        if (Bj1_calcTimer((be<s16>*)(reinterpret_cast<void*>(&mPotPourTimer)))) {
            return;
        }
        s32 room = (s8)current.roomNo, reverb = dComIfGp_getReverb(room);
        Bj1_startSound(0x69F8, (cXyz*)(reinterpret_cast<void*>(&current.pos.x)), 0, reverb);
        setAnm_NUM(5);
        setPrtcl_drugPot_2();
    }
}

/* 021F4974 */
void daNpc_Bj1_c::setMtx(int initializing) {
    if ((u8)mDemoActive == 0) {
        u32 mode = 0;
        if ((u32)mObjAcch.m_flags & 0x20u) {
            dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            mode = Bj1_getMaterialSound(
                (dBgS*)(&scene->play.mBgS),
                (cBgS_PolyInfo*)(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)));
        }
        s32 room = (s8)current.roomNo, reverb = dComIfGp_getReverb(room);
        mDoExt_McaMorf* morph = mpMorf;
        u32 result = Bj1_playMorf((mDoExt_McaMorf*)(morph),
                                  (cXyz*)(reinterpret_cast<void*>(&eyePos.x)), mode, reverb);
        morph = mpMorf;
        f32 previous = (f32)mPreviousAnmFrame;
        mAnmEnded = (u8)result;
        f32 frame = (f32)morph->mFrameCtrl.mFrame;
        if (frame < previous) {
            morph = mpMorf;
            mAnmEnded = 1;
        }
        mPreviousAnmFrame = (f32)morph->mFrameCtrl.mFrame;
        setMtx_anmProc();
        Bj1_playMorf((mDoExt_McaMorf*)(mpPrpMorf.get()),
                     (cXyz*)(reinterpret_cast<void*>(&eyePos.x)), 0, 0);
        if ((u8)mPropellerOpening != 0) {
            setPrtcl_peraProOpen();
            mPropellerOpening = 0;
        }
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        Bj1_correctPosition((dBgS_ObjAcch*)(reinterpret_cast<void*>(&mObjAcch)),
                            (dBgS*)(&scene->play.mBgS));
    }
    dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    u32 first = Bj1_getRoomId((dBgS*)(&scene->play.mBgS),
                              (cBgS_PolyInfo*)(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)));
    tevStr.mRoomNo = (u8)first;
    scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    u32 second = Bj1_getPolyColor((dBgS*)(&scene->play.mBgS),
                                  (cBgS_PolyInfo*)(static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd)));
    f32 x = (f32)current.pos.x, y = (f32)current.pos.y, z = (f32)current.pos.z;
    tevStr.mEnvrIdxOverride = (u8)second;
    PSMTXTrans((Mtx34*)(mDoMtx_stack_c::get()), x, y, z);
    s32 yaw = (s16)current.angle.y;
    mDoMtx_YrotM((Mtx34*)(mDoMtx_stack_c::get()), yaw);
    mDoExt_McaMorf* morph = mpMorf.get();
    J3DModel* model = morph->mpModel.get();
    const u8 bodyOrder[12] = {0, 4, 7, 9, 2, 5, 3, 8, 1, 10, 6, 11};
    bj1_storeMatrixOrdered(&model->mBaseTransformMtx, mDoMtx_stack_c::get(), bodyOrder);
    ((mDoExt_McaMorf*)(mpMorf.get()))->calc();
    ((mDoExt_McaMorf*)(mpPrpMorf.get()))->calc();
    const u8 attachmentOrder[12] = {2, 7, 5, 9, 4, 10, 6, 0, 3, 1, 8, 11};
    if (gabi::ea(mpLeftHandModel.get()) != 0) {
        Mtx34* matrix = bj1_jointMatrix(mpMorf, mLeftHandJoint);
        PSMTXCopy(matrix, (Mtx34*)(mDoMtx_stack_c::get()));
        u32 destination = gabi::ea(mpLeftHandModel.get());
        bj1_storeMatrixOrdered(&gabi::at<J3DModel>(destination)->mBaseTransformMtx,
                               mDoMtx_stack_c::get(), attachmentOrder);
        ((J3DModel*)(gabi::at<void>(gabi::ea(mpLeftHandModel.get()))))->calc();
    }
    if (gabi::ea(mpRightHandModel.get()) != 0) {
        Mtx34* matrix = bj1_jointMatrix(mpMorf, mRightHandJoint);
        PSMTXCopy(matrix, (Mtx34*)(mDoMtx_stack_c::get()));
        u32 destination = gabi::ea(mpRightHandModel.get());
        bj1_storeMatrixOrdered(&gabi::at<J3DModel>(destination)->mBaseTransformMtx,
                               mDoMtx_stack_c::get(), attachmentOrder);
        ((J3DModel*)(gabi::at<void>(gabi::ea(mpRightHandModel.get()))))->calc();
    }
    Mtx34* matrix = bj1_jointMatrix(mpMorf, mHeadJoint);
    PSMTXCopy(matrix, (Mtx34*)(mDoMtx_stack_c::get()));
    u32 destination = gabi::ea(mpMaskModel.get());
    bj1_storeMatrixOrdered(&gabi::at<J3DModel>(destination)->mBaseTransformMtx,
                           mDoMtx_stack_c::get(), attachmentOrder);
    ((J3DModel*)(gabi::at<void>(gabi::ea(mpMaskModel.get()))))->calc();
    flwPrtcl_danceLR();
    Bj1_setAttentionRegister(this, initializing);
}

/* 021F4330 */
u32 daNpc_Bj1_c::anmNum_toResID(int index) { return l_animationResources.mIDs[index]; }

/* 021F4468 */
BOOL daNpc_Bj1_c::setAnm_anm(anm_prm_c* prm) {
    s8 current = (s8)(u8)mAnimation;
    s8 animation = prm->mAnmNum;
    if (current == animation) {
        return 1;
    }
    mAnimation = (u8)animation;
    s32 resource = anmNum_toResID((s32)animation);
    s32 loop = prm->mLoopMode;
    void* morph = mpMorf.get();
    f32 speed = prm->mSpeed;
    f32 blend = prm->mMorf;
    dNpc_setAnmIDRes((mDoExt_McaMorf*)(morph), loop, blend, speed, resource, -1, "Bj");
    if ((s8)(u8)mAnimation == 7) {
        setPrtcl_danceLR();
        mPotPourEmitter = 0;
        mPotPourTimer = 30;
        f32 zero = (f32)l_zero;
        mAnmEnded = 0;
        mAnmTagIndex = 0;
        mPreviousAnmFrame = zero;
    } else {
        delPrtcl_danceLR();
        f32 zero = (f32)l_zero;
        mAnmTagIndex = 0;
        mPreviousAnmFrame = zero;
        mAnmEnded = 0;
    }
    return 1;
}

/* 021F4544 */
void daNpc_Bj1_c::setAnm_NUM(int index) { setAnm_anm(&l_animations.mRecords[(u32)index]); }

/* 021F7EEC */
BOOL daNpc_Bj1_c::setAnm() {
    s32 tag = (s8)(u8)mState;
    anm_prm_c *record = &l_stateAnimations.mRecords[tag];
    setAnm_anm(record);
    return 1;
}
/* 021F70C0 */
void daNpc_Bj1_c::setAnm_prp(s8 mode) {
    f32 zero = (f32)l_zero;
    f32 end = (f32)l_propellerEndFrame;
    if (mode != 1 && mode != 2) {
        return;
    }
    gabi::Local<SafeString> key;
    key->__vtbl = gc_tu.safeString;
    dRes_control_c *manager = dComIfG_resControl();
    key->mStringTop = gabi::guest_str("Bj");
    void *resource = Bj1_getResource(manager, key.get(), 40);
    if (mode == 1) {
        f32 speed = (f32)l_one;
        mDoExt_McaMorf *morph = mpPrpMorf;
        morph->setAnm(static_cast<J3DAnmTransform *>(resource), J3DFrameCtrl::EMode_NONE, zero,
                      speed, zero, end, nullptr);
    } else {
        mDoExt_McaMorf *morph = mpPrpMorf;
        morph->setAnm(static_cast<J3DAnmTransform *>(resource), J3DFrameCtrl::EMode_LOOP, zero, end,
                      zero, end, nullptr);
        f32 frame = (f32)l_jumpHoldFrame;
        mDoExt_McaMorf* reloadedMorph = mpPrpMorf;
        reloadedMorph->mFrameCtrl.mFrame = frame;
    }
    s32 room = (s8)current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    Bj1_startSound(mode == 1 ? 0x58C0 : 0x58C1, &current.pos, 0, reverb);
    if (mode == 1) {
        mPropellerOpening = 1;
    }
}
/* 021F7F40 */
void daNpc_Bj1_c::chg_anmAtr(u8 attribute) {
    if (attribute < 11 && attribute != mAnmAttribute) {
        mAnmAttribute = attribute;
        setAnm_ATR();
    }
}
/* 021F7F5C */
void daNpc_Bj1_c::control_anmAtr() {
    u8 attribute = mAnmAttribute;
    if ((attribute == 6 || attribute == 7) && mAnmEnded != 0) {
        mAnmAttribute = 0;
        setAnm_NUM(0);
    } else if (attribute == 10 && mAnmEnded != 0) {
        mAnmAttribute = 2;
        setAnm_NUM(3);
    }
}
/* 021F7F28 */
void daNpc_Bj1_c::setAnm_ATR() {
    anm_prm_c *record = &l_attributeAnimations.mRecords[(u8)mAnmAttribute];
    setAnm_anm(record);
}
/* 021F7FB0 */
void daNpc_Bj1_c::anmAtr(u16 attribute) {
    if (attribute == 6) {
        if ((s8)(u8)mDemoAnmPhase == 0) {
            mAnmAttribute = 255;
            dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
            u8 value = (u8)scene->play.mMesgAnime;
            chg_anmAtr(value);
            u8 phase = (u8)mDemoAnmPhase;
            mDemoAnmPhase = (u8)(phase + 1);
        }
        dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        u8 value = (u8)scene->play.mMesgAnimeTagInfo;
        scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        scene->play.mMesgAnimeTagInfo = 255;
        if (value != 255 && (u8)mPendingAnmAttribute != value) {
            mPendingAnmAttribute = value;
        }
    } else if (attribute == 14) {
        mDemoAnmPhase = 0;
    }
    control_anmAtr();
}
/* 021F7828 */
void daNpc_Bj1_c::eventOrder() {
    s8 order = (s8)(u8)mEventOrder;
    if (order == 1 || order == 2) {
        s8 kind = (s8)(u8)mMessageType;
        s8 capturedOrder = (s8)(u8)mEventOrder;
        u16 flags = (u16)eventInfo.mCondition | dEvtCnd_CANTALK_e;
        eventInfo.mCondition = flags;
        if (kind == 12) {
            eventInfo.mCondition = (u16)(flags | dEvtCnd_CANTALKITEM_e);
        }
        if (capturedOrder == 1) {
            fopAcM_orderSpeakEvent(this);
        }
        return;
    }
    if (order < 3) {
        return;
    }
    s16 index = (s16)(order - 3);
    mEventIndex = index;
    s32 event = (s16)mEventIDs[index];
    fopAcM_orderOtherEventId(this, event, 255, 65535, 0, 1);
}
/* 021F5568 */
void daNpc_Bj1_c::checkOrder() {
    u16 state = (u16)eventInfo.mCommand;
    if (state == 2) {
        s16 index = mEventIndex;
        s32 event = (s16)mEventIDs[index];
        u32 player = dComIfGp_ea();
        if (Bj1_startCheck((dEvent_manager_c*)(gabi::at<void>(player + 0x52C4u)), event)) {
            if ((s16)mEventIndex == 2) {
                actor_status = (u32)actor_status & 0xFFFFBFFFu;
            }
            mEventOrder = 0;
        }
    } else if (state == 1) {
        s8 order = (s8)(u8)mEventOrder;
        if (order == 1 || order == 2) {
            mEventOrder = 0;
            mTalkRequested = 1;
        }
    }
}

/* 021F8078 */
BOOL daNpc_Bj1_c::chk_talk() {
    mTalkItem = 0xFF;
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u32 eventState = (u8)scene->play.mEvtCtrl.mTalkButton;
    if (eventState - 1u > 3u) {
        return 1;
    }
    scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    if (Bj1_checkPresentEnd(&scene->play.mEvtManager) == 0) {
        return 0;
    }
    scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u8 talkState = (u8)scene->play.mEvtCtrl.mItemNo;
    mTalkItem = talkState;
    return 1;
}
/* 021F80F8 */
BOOL daNpc_Bj1_c::chk_drct(f32 range) {
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    fopAc_ac_c *player = scene->play.mPlayerInfo[0].mpPlayer;
    s32 direction = Bj1_targetAngleY(&current.pos, &player->current.pos);
    f32 scale = (f32)l_degreesToAngle;
    s32 facing = (s16)current.angle.y;
    // Preserve fmuls frA/frC order and fctiwz saturation/invalid behavior.
    f32 scaled = gabi::fmuls_ppc(range, scale);
    s32 delta = (s16)(direction - facing);
    s32 converted = (s32)(u32)ppc_fctiwz((f64)scaled);
    s32 threshold = (s16)(u16)converted;
    s32 magnitude = delta < 0 ? -delta : delta;
    return magnitude < threshold;
}
/* 021F8190 */
BOOL daNpc_Bj1_c::chk_partsNotMove() {
    if ((s16)mPreviousHeadRoll != (s16)m_jnt.mAngles[1][1]) {
        return 1;
    }
    return (s16)mPreviousHeadPitch != (s16)m_jnt.mAngles[0][1] ? 1 : 0;
}
/* 021F6A54 */
void daNpc_Bj1_c::lookBack() {
    u32 mode = (u32)(s32)(s8)(u8)mLookMode;
    s32 heading = (s16)current.angle.y;
    f32 zero = (f32)l_zero;
    mPreviousHeading = (s16)heading;
    s32 roll = (s16)m_jnt.mAngles[1][1];
    gabi::Local<Bj1LookFrame> frame;
    frame->mTarget.z = zero;
    mPreviousHeadRoll = (s16)roll;
    s32 pitch = (s16)m_jnt.mAngles[0][1];
    frame->mTarget.x = zero;
    f32 y = (f32)eyePos.y;
    frame->mTarget.y = zero;
    f32 x = (f32)current.pos.x;
    u32 flag = (u8)mLookAtPlayer;
    f32 z = (f32)current.pos.z;
    mPreviousHeadPitch = (s16)pitch;
    cXyz *targetArg = nullptr;
    if (mode == BJ1_LOOK_PLAYER) {
        f32 distance = (f32)l_cullLowerY;
        Bj1_playerEyePos(&frame->mScratch, distance);
        s32 type = (s8)mType;
        u32 tz = Bj1_floatBits(frame->mScratch.z);
        x = (f32)current.pos.x;
        u32 ty = Bj1_floatBits(frame->mScratch.y);
        Bj1_setFloatBits(frame->mTarget.z, tz);
        z = (f32)current.pos.z;
        y = (f32)eyePos.y;
        Bj1_setFloatBits(frame->mTarget.y, ty);
        u32 tx = Bj1_floatBits(frame->mScratch.x);
        s32 limit = (s16)l_hio.mRecords[type].mLookAngle;
        Bj1_setFloatBits(frame->mTarget.x, tx);
        targetArg = &frame->mTarget;
        Bj1_addCalcAngle2(&mLookLimit, limit, 4, 0x800);
    } else if (mode == BJ1_LOOK_TARGET) {
        u32 tz = (u32)mLookTargetZBits;
        x = (f32)current.pos.x;
        Bj1_setFloatBits(frame->mTarget.z, tz);
        s32 type = (s8)mType;
        u32 tx = (u32)mLookTargetXBits, ty = Bj1_floatBits(mLookTargetY);
        Bj1_setFloatBits(frame->mTarget.y, ty);
        Bj1_setFloatBits(frame->mTarget.x, tx);
        s32 limit = (s16)l_hio.mRecords[type].mLookAngle;
        z = (f32)current.pos.z;
        targetArg = &frame->mTarget;
        Bj1_addCalcAngle2(&mLookLimit, limit, 4, 0x800);
    } else {
        if (mode == BJ1_LOOK_INITIAL_HEADING) {
            heading = (s16)mLookHeading;
        }
        s32 type = (s8)mType;
        s32 limit = (s16)l_hio.mRecords[type].mLookAngle;
        Bj1_addCalcAngle2(&mLookLimit, limit, 4, 0x800);
    }
    u32 enabled = (u8)m_jnt.mbTrn;
    s32 limit;
    if (enabled == 0) {
        mLookLimit = 0;
        limit = 0;
    } else {
        limit = (s16)mLookLimit;
    }
    frame->mOrigin.x = x;
    frame->mOrigin.y = y;
    frame->mOrigin.z = z;
    Bj1_lookAtTarget(&m_jnt, &current.angle.y, targetArg, &frame->mOrigin, heading, limit, flag);
}
/* 021F81C0 */
BOOL daNpc_Bj1_c::getMaskInf(u8 *output) {
    u32 destination = gabi::ea(output);
    if (destination == 0) {
        return 0;
    }
    u8 mask;
    switch ((s8)(u8)mMessageType) {
    case 1:
        mask = 0x01;
        break;
    case 3:
        mask = 0x02;
        break;
    case 5:
        mask = 0x04;
        break;
    case 7:
        mask = 0x08;
        break;
    case 9:
        mask = 0x10;
        break;
    case 11:
        mask = 0x20;
        break;
    case 14:
        mask = 0x40;
        break;
    case 16:
        mask = 0x80;
        break;
    default:
        return 0;
    }
    *reinterpret_cast<be<u8> *>(output) = mask;
    return 1;
}
/* 021F82C0 */
BOOL daNpc_Bj1_c::chkReg(u16 reg) {
    gabi::Local<Bj1MaskFrame> frame;
    if (getMaskInf(reinterpret_cast<u8 *>(&frame->mask)) == 0) {
        return 0;
    }
    dSv_info_c *saved = dComIfGs_info();
    u32 bits = Bj1_getEventReg(&saved->mSavedata.mEvent, reg);
    u8 mask = (u8)frame->mask; // After the getter call.
    return (bits & mask) != 0;
}
/* 021F8314 */
void daNpc_Bj1_c::setReg(u16 reg) {
    gabi::Local<Bj1MaskFrame> frame;
    if (getMaskInf(reinterpret_cast<u8 *>(&frame->mask)) == 0) {
        return;
    }
    dSv_info_c *saved = dComIfGs_info();
    u32 bits = Bj1_getEventReg(&saved->mSavedata.mEvent, reg);
    saved = dComIfGs_info(); // Reload before reading private mask.
    u8 mask = (u8)frame->mask;
    Bj1_setEventReg(&saved->mSavedata.mEvent, reg, bits | mask);
}
/* 021F8384 */
u32 daNpc_Bj1_c::next_msgStatus(be<u32> *message) {
    u32 msg = (u32)*message;
    switch (msg) {
    case 0x13ED:
        *message = 0x13EE;
        return 15;
    case 0x13EE:
        *message = 0x13EF;
        return 15;
    case 0x13F0:
        *message = 0x13F1;
        return 15;
    case 0x13F2:
        *message = 0x13F3;
        return 15;
    case 0x13F5:
        *message = 0x13F6;
        return 15;
    case 0x13F8:
        *message = 0x13F9;
        return 15;
    case 0x13FB:
        *message = 0x13FC;
        return 15;
    case 0x13FD:
        *message = 0x13FE;
        return 15;
    case 0x13FF:
        *message = 0x1400;
        return 15;
    case 0x1400:
        *message = 0x1401;
        return 15;
    case 0x1402:
        *message = 0x1403;
        return 15;
    case 0x1403:
        *message = 0x1404;
        return 15;
    case 0x1406:
        *message = 0x1407;
        return 15;
    case 0x1407:
        *message = 0x1408;
        return 15;
    case 0x140B:
        *message = 0x140C;
        return 15;
    case 0x140E:
        *message = 0x140F;
        return 15;
    case 0x1412:
        *message = 0x1413;
        return 15;
    case 0x1414:
        *message = 0x1415;
        return 15;
    case 0x1416:
        *message = 0x1417;
        return 15;
    case 0x1417:
        *message = 0x1418;
        return 15;
    case 0x1418:
        *message = 0x1419;
        return 15;
    case 0x1419:
        *message = 0x141A;
        return 15;
    case 0x141A:
        *message = 0x141B;
        return 15;
    case 0x141B:
        *message = 0x141C;
        return 15;
    case 0x141C:
        *message = 0x141D;
        return 15;
    case 0x141F:
        *message = 0x1420;
        return 15;
    case 0x1422:
        *message = 0x1423;
        return 15;
    case 0x1425:
        *message = 0x1426;
        return 15;
    case 0x1428:
        *message = 0x1429;
        return 15;
    case 0x142C:
        *message = 0x142D;
        return 15;
    case 0x142E:
        *message = 0x142F;
        return 15;
    case 0x1431:
        *message = 0x1432;
        return 15;
    case 0x1433:
        *message = 0x1417;
        return 15;
    case 0x1482:
        *message = 0x1483;
        return 15;
    case 0x1487:
        *message = 0x1488;
        return 15;
    case 0x1488:
        *message = 0x1489;
        return 15;
    case 0x148A:
        *message = 0x148B;
        return 15;
    case 0x148B:
        *message = 0x148C;
        return 15;
    case 0x148C:
        *message = 0x148D;
        return 15;
    case 0x148D:
        *message = 0x148E;
        return 15;
    case 0x148E:
        *message = 0x148F;
        return 15;
    case 0x148F:
        *message = 0x1490;
        return 15;
    case 0x1490:
        *message = 0x1491;
        return 15;
    case 0x1491:
        *message = 0x1492;
        return 15;
    case 0x1492:
        *message = 0x1493;
        return 15;
    case 0x1494:
        *message = 0x1495;
        return 15;
    case 0x1495:
        *message = 0x1496;
        return 15;
    case 0x1497:
        *message = 0x1498;
        return 15;
    case 0x1498:
        *message = 0x1499;
        return 15;
    case 0x1499:
        *message = 0x149A;
        return 15;
    case 0x149A:
        *message = 0x149B;
        return 15;
    case 0x149B:
        *message = 0x149C;
        return 15;
    case 0x149C:
        *message = 0x149D;
        return 15;
    case 0x1423: {
        dSv_info_c *saved = dComIfGs_info();
        Bj1_onEventBit(&saved->mSavedata.mEvent, 0x1B80);
        // Fall through to shared decision; saved-global is reloaded there.
        break;
    }
    case 0x1424:
        break;
    default:
        return 16;
    }
    dSv_info_c *saved = dComIfGs_info();
    u8 count = (u8)saved->mSavedata.mPlayer.mBagItemRecord.mBeastNum[1];
    u32 next;
    if (count < 4) {
        u8 flag = (u8)mCollectedFirstGift;
        next = flag == 0 ? 0x1425 : 0x1427;
    } else {
        if (Bj1_checkEmptyBottle(&saved->mSavedata.mPlayer.mPlayerItem) != 0) {
            next = 0x1428;
        } else {
            u8 flag = (u8)mCollectedSecondGift;
            next = flag == 0 ? 0x142E : 0x1430;
        }
    }
    *message = next;
    return 15;
}
/* 021F88A4 */
u32 daNpc_Bj1_c::getMsg_BJ1_0() {
    if (!bj1_eventFlag(0x604)) {
        return 0x140E;
    }
    s32 stage = Bj1_windDirection();
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    if (stage == 1) {
        return Bj1_isEventBit(save, 0xC80) ? 0x1414 : 0x1412;
    }
    return Bj1_isEventBit(save, 0x904) ? 0x1411 : 0x1410;
}
/* 021F8958 */
u32 daNpc_Bj1_c::getMsg_BJ2_0() {
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0x920);
    return enabled ? 0x13F4u : 0x13F2u;
}
/* 021F89A4 */
u32 daNpc_Bj1_c::getMsg_BJ3_0() {
    if ((u8)mPartnerTalkAlternate != 0) {
        return 0x13FBu;
    }
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0xC20);
    return enabled ? 0x13FAu : 0x13F8u;
}
/* 021F8A14 */
u32 daNpc_Bj1_c::getMsg_BJ4_0() {
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0xC08);
    return enabled ? 0x13FFu : 0x13FDu;
}
/* 021F8A60 */
u32 daNpc_Bj1_c::getMsg_BJ5_0() {
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0xC04);
    return enabled ? 0x13F7u : 0x13F5u;
}
/* 021F8AAC */
u32 daNpc_Bj1_c::getMsg_BJ6_0() {
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0xC02);
    return enabled ? 0x1405u : 0x1402u;
}
/* 021F8AF8 */
u32 daNpc_Bj1_c::getMsg_BJ7_0() {
    if ((u8)mFirstEventEnded != 0) {
        mFirstEventEnded = 0;
        return 0x142B;
    }
    if ((u8)mSecondEventEnded != 0) {
        mSecondEventEnded = 0;
        return 0x142C;
    }
    u8 state = mTalkItem;
    if (state == 70) {
        return bj1_eventFlag(0x1B80) ? 0x1424 : 0x1422;
    }
    if (state != 255) {
        return 0x1421;
    }
    dSv_player_collect_c *inventory = &dComIfGs_info()->mSavedata.mPlayer.mCollect;
    BOOL hasItem = Bj1_isSymbol(inventory, dSymbol_FARORE_e);
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL progressed = Bj1_isEventBit(save, 0xD08);
    if (hasItem) {
        if (!progressed) {
            return 0x1433;
        }
        if (!bj1_eventFlag(0x1C80)) {
            return 0x1431;
        }
    } else if (!progressed) {
        return 0x1416;
    }
    return bj1_eventFlag(0x1B80) ? 0x141E : 0x141F;
}
/* 021F8C94 */
u32 daNpc_Bj1_c::getMsg_BJ8_0() {
    if (!bj1_eventFlag(0xD40)) {
        return 0x1406;
    }
    s32 stage = Bj1_windDirection();
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    if (stage == 7) {
        return Bj1_isEventBit(save, 0xD10) ? 0x140D : 0x140B;
    }
    return Bj1_isEventBit(save, 0xD20) ? 0x140A : 0x1409;
}
/* 021F8D48 */
u32 daNpc_Bj1_c::getMsg_BJ9_0() {
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    BOOL enabled = Bj1_isEventBit(save, 0xC10);
    return enabled ? 0x13F0u : 0x13EDu;
}
/* 021F8D94 */
u32 daNpc_Bj1_c::getMsg_Corog() {
    BOOL first = chkReg(0x9AFF);
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    if (first) {
        if (Bj1_isEventBit(save, 0x102)) {
            return chkReg(0x99FF) ? 0x1486 : 0x1485;
        }
        if (!chkReg(0x9EFF)) {
            return chkReg(0x98FF) ? 0x149E : 0x1487;
        }
        if (chkReg(0x96FF)) {
            return 0x1484;
        }
        save = &dComIfGs_info()->mSavedata.mEvent;
        u32 bits = Bj1_getEventReg(save, 0x9EFF);
        u16 count = 0;
        for (int i = 0; i < 8; ++i) {
            count += (bits & 1u) != 0;
            bits = ((bits >> 1) | (bits << 31)) & 0xFFu;
        }
        dComIfG_inf_c *player = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        player->play.mMsgCountNumber = (s16)(u16)(8 - count);
        return 0x1482;
    }
    if (Bj1_isEventBit(save, 0x2E10)) {
        return chkReg(0x97FF) ? 0x1494 : 0x1497;
    }
    return chkReg(0x97FF) ? 0x1494 : 0x148A;
}
/* 021F8F8C */
u32 daNpc_Bj1_c::getMsg() {
    switch ((s8)(u8)mMessageType) {
    case 0:
        return getMsg_BJ1_0();
    case 2:
        return getMsg_BJ2_0();
    case 4:
        return getMsg_BJ3_0();
    case 6:
        return getMsg_BJ4_0();
    case 8:
        return getMsg_BJ5_0();
    case 10:
        return getMsg_BJ6_0();
    case 12:
        return getMsg_BJ7_0();
    case 13:
        return getMsg_BJ8_0();
    case 15:
        return getMsg_BJ9_0();
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 14:
    case 16:
        return getMsg_Corog();
    default:
        return 0;
    }
}
/* 021F908C */
BOOL daNpc_Bj1_c::chkAttention() {
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    dAttention_c *attention = &scene->play.mAttention;
    fopAc_ac_c *selected;
    if (Bj1_lockonTruth(attention) != 0) {
        selected = Bj1_actionTarget(attention, 0);
    } else {
        selected = Bj1_lockonTarget(attention, 0);
    }
    return selected == this;
}
/* 021F4914 */
void daNpc_Bj1_c::setAttention(bool force) {
    s8 type = (s8)mType;
    u32 flag = (u32)mAttentionFromHead;
    f32 z = (f32)current.pos.z, y = (f32)current.pos.y, x = (f32)current.pos.x;
    f32 height = (f32)l_hio.mRecords[type].mAttentionHeight;
    attention_info.position.z = z;
    f32 lifted = (f32)((f64)y + (f64)height);
    attention_info.position.x = x;
    attention_info.position.y = lifted;
    if (flag == 0 && !force) {
        return;
    }
    f32 bz = (f32)mHeadPosZ, by = (f32)mHeadPosY;
    eyePos.z = bz;
    f32 bx = (f32)mHeadPosX;
    eyePos.y = by;
    eyePos.x = bx;
}

/* 021F58D8 */
fopAc_ac_c* daNpc_Bj1_c::searchByID(fpc_ProcID id) {
    gabi::Local<be<u32>> result;
    *result = 0;
    Bj1_searchById(id, (be<u32>*)(result.get()));
    return gabi::at<fopAc_ac_c>(*result);
}

/* 021F5450 */
BOOL daNpc_Bj1_c::partner_srch_sub(u32 callback) {
    mPartnerID = 0xFFFFFFFFu;
    l_jb_num = 0;
    for (u32 i = 0; i < 20; ++i) {
        l_jb_actor.mActors[i] = nullptr;
    }
    Bj1_searchActors((void*)(gabi::at<void>(callback)), (daNpc_Bj1_c*)(this));
    if ((u32)(s32)l_jb_num == 0) {
        return 0;
    }
    u32 actor = gabi::ea(l_jb_actor.mActors[0].get());
    u32 id = actor ? (u32)gabi::at<fopAc_ac_c>(actor)->mBsPcId : 0xFFFFFFFFu;
    mPartnerID = id;
    return 1;
}

/* 021F54FC */
void daNpc_Bj1_c::partner_srch() {
    if ((s8)mActionPhase == 1 && (s8)mMessageType == 4) {
        // Retain an observed callee boundary; do not inline callback enumeration.
        u32 result = partner_srch_sub(0x021F2D78u);
        if (result != 0) {
            u8 phase = (u8)mActionPhase;
            mActionPhase = (u8)(phase + 1);
        }
    }
}

/* 021F78B0 */
void daNpc_Bj1_c::setCollision_SP_() {
    mCyl.SetC(&current.pos);
    s32 type = (s8)mType;

    f32 radius = (f32)l_cylinderDimensions.mRecords[type].mRadius;
    mCyl.SetR(radius);
    type = (s8)mType; // native reload after first setter
    f32 height = (f32)l_cylinderDimensions.mRecords[type].mHeight;
    mCyl.SetH(height);
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    cCcS_Set(&scene->play.mCcS, &mCyl);
}
/* 021F9114 */
void daNpc_Bj1_c::set_pthPoint(u8 requested) {
    if (mPath.mPath.v.get() == 0) {
        return;
    }
    u32 count = Bj1_maxPoint(&mPath);
    u32 selected = requested;
    if (count < selected) {
        selected = count;
    }
    gabi::Local<cXyz> position;
    u32 byteIndex = selected & 255u;
    mPath.mIdx = (u8)byteIndex;
    Bj1_getPathPoint(&mPath, position.get(), byteIndex);
    u32 y = Bj1_floatBits((*position.get()).y);
    u32 x = Bj1_floatBits((*position.get()).x);
    u32 z = Bj1_floatBits((*position.get()).z);
    Bj1_setFloatBits(current.pos.x, x);
    Bj1_setFloatBits(current.pos.z, z);
    Bj1_setFloatBits(current.pos.y, y);
    mPath.nextIdxAuto();
}
/* 021F6E20 */
void daNpc_Bj1_c::bj_clcFlySpd() {
    s32 state = (s8)(u8)mFlightState;
    if (state == 1) {
        s32 direction = Bj1_targetAngleY(&current.pos, reinterpret_cast<cXyz *>(&mMoveTargetXBits));
        s32 type = (s8)mType;
        Bj1HIORecord *prm = &l_hio.mRecords[type];
        s32 maximum = (s16)prm->mFlightTurnMax;
        s32 scale = (s16)prm->mFlightTurnScale;
        Bj1_addCalcAngle(&current.angle.y, direction, scale, maximum, 0);
        type = (s8)mType;
        f32 currentY = (f32)current.pos.y;
        f32 targetY = (f32)mMoveTargetY;
        prm = &l_hio.mRecords[type];
        f32 delta = gabi::fsubs_ppc(targetY, currentY);
        f32 limit = (f32)prm->mFlightHeightThreshold;
        bool rising = delta > limit; // PPC GT bit: unordered yields false
        mPathRising = (u8)rising;
        f32 target = (f32)prm->mFlightVerticalSpeed;
        f32 step = (f32)prm->mFlightVerticalStep;
        if (!rising) {
            target = -target;
        }
        Bj1_chaseF(&speed.y, target, step);
    } else if (state == 2) {
        s32 type = (s8)mType;
        Bj1HIORecord *fallParameters = &l_hio.mRecords[type];
        f32 target = (f32)fallParameters->mFlightVerticalSpeed;
        f32 step = (f32)fallParameters->mFlightVerticalStep;
        Bj1_chaseF(&speed.y, -target, -step);
    }
    f32 step = (f32)mSpeedStep;
    f32 target = (f32)mTargetSpeed;
    Bj1_chaseF(&speedF, target, step);
}
/* 021F6F40 */
u32 daNpc_Bj1_c::bj_movPass(int updateHeight) {
    u32 oldIndex = 0, result = 0;
    gabi::Local<Bj1PassFrame> frame;
    Bj1PathFlags* path = reinterpret_cast<Bj1PathFlags*>(mPath.mPath.get());
    bool loop = false;
    if (path != 0) {
        u8 flags = (u8)path->m_closed;
        oldIndex = (u8)mPath.mIdx;
        loop = (flags & 1) != 0;
    }
    if (loop) {
        f32 x = (f32)current.pos.x;
        u32 flag = (u8)mPath.mbDir;
        frame->mPosition.x = x;
        f32 y = (f32)current.pos.y, z = (f32)current.pos.z;
        frame->mPosition.y = y;
        frame->mPosition.z = z;
        u32 crossed = mPath.chkPointPass(&frame->mPosition, flag != 0);
        if (crossed == 0) {
            return 0;
        }
        mPath.nextIdxAuto();
        result = 1;
        if (updateHeight == 0) {
            return result;
        }
    } else {
        Bj1_subtractVector(reinterpret_cast<cXyz *>(&mMoveTargetXBits), &frame->mVector,
                           &current.pos);
        f32 x = (f32)frame->mVector.x, zero = (f32)l_zero;
        frame->mHorizontal.x = x;
        f32 z = (f32)frame->mVector.z;
        frame->mHorizontal.y = zero;
        frame->mHorizontal.z = z;
        f64 squared = Bj1_squareMagnitude(&frame->mHorizontal);
        f64 distance = Bj1_squareRoot(squared);
        f32 threshold = (f32)mPointPassDistance;
        if (distance > (f64)threshold) {
            return 0; // unordered continues
        }
        path = reinterpret_cast<Bj1PathFlags*>(mPath.mPath.get());
        result = 1;
        if (path == 0) {
            return result;
        }
        if (mPath.nextIdxAuto() == 0) {
            result = 2;
        }
        if (updateHeight == 0) {
            return result;
        }
    }
    u32 currentIndex = (u8)mPath.mIdx;
    Bj1_getPathPoint(&mPath, &frame->mVector, currentIndex);
    f32 firstY = (f32)frame->mVector.y;
    Bj1_getPathPoint(&mPath, &frame->mVector, oldIndex);
    f32 secondY = (f32)frame->mVector.y;
    mPathRising = (u8) !(secondY > firstY);
    return result;
}
/* 021F7270 */
BOOL daNpc_Bj1_c::bj_flyMove() {
    s32 state = (s8)(u8)mFlightState;
    if (state == 0) {
        return 0;
    }
    if ((u8)mFlightComplete != 0) {
        mSkipCollisionMove = 1;
        return 1;
    }
    gabi::Local<Bj1FlyMoveFrame> frame;
    if (mPath.mPath.v.get() != 0) {
        u32 index = (u8)mPath.mIdx;
        Bj1_getPathPoint(&mPath, &frame->mPoint, index);
        u32 x = Bj1_floatBits(frame->mPoint.x), z = Bj1_floatBits(frame->mPoint.z);
        mMoveTargetXBits = x;
        u32 y = Bj1_floatBits(frame->mPoint.y);
        state = (s8)(u8)mFlightState;
        Bj1_setFloatBits(mMoveTargetY, y);
        mMoveTargetZBits = z;
    }
    if (state == 1 || state == 2 || state == 7) {
        s32 type = (s8)mType, angle = (s16)mPropellerSpeed;
        Bj1HIORecord *prm = &l_hio.mRecords[type];
        s32 step = (s16)prm->mPropellerSpeedStep, updated = (s16)(angle + step);
        s32 maximum = (s16)prm->mPropellerSpeedMax;
        if (maximum < updated) {
            updated = maximum;
        }
        state = (s8)(u8)mFlightState;
        mPropellerSpeed = (s16)updated;
    }
    f32 zero = (f32)l_zero;
    switch ((u32)state) {
    case 1: {
        bj_clcFlySpd();
        u32 result = bj_movPass(1);
        if (result >= 1 && result <= 2) {
            mFlightComplete = 1;
        }
        return 1;
    }
    case 2: {
        bj_clcFlySpd();
        s32 type = (s8)mType;
        f32 ground = (f32)mObjAcch.m_ground_h, height = (f32)current.pos.y;
        f32 delta = gabi::fsubs_ppc(height, ground),
            threshold = (f32)l_hio.mRecords[type].mLandingHeight;
        bool landed = delta < threshold;
        mFlightComplete = (u8)landed;
        if (landed) {
            setAnm_NUM(1);
            speed.y = zero;
            mFlightState = 3;
        }
        return 1;
    }
    case 3: {
        s32 type = (s8)mType, angle = (s16)mPropellerSpeed;
        s32 step = (s16)l_hio.mRecords[type].mPropellerSpeedStep;
        s32 updated = (s16)(angle - step);
        if (updated < 0) {
            updated = 0;
        }
        bool done = updated == 0;
        mPropellerSpeed = (s16)updated;
        mFlightComplete = (u8)done;
        if (done) {
            setAnm_prp(2);
            mFlightState = 4;
        }
        return 1;
    }
    case 4: {
        mDoExt_McaMorf* morph = mpPrpMorf;
        bool done = morph->mFrameCtrl.checkPass(zero) != 0;
        mFlightComplete = (u8)done;
        if (done) {
            mPropellerVisible = 0;
            f32 falling = (f32)l_gravity;
            mFlightState = 5;
            gravity = falling;
        }
        return 1;
    }
    case 5: {
        bool done = ((u32)mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) != 0;
        mFlightComplete = (u8)done;
        if (done) {
            mFlightState = 0;
        }
        return 1;
    }
    case 6: {
        mDoExt_McaMorf* morph = mpPrpMorf;
        u8 flags = (u8)morph->mFrameCtrl.mState;
        bool done = (flags & J3DFrameCtrl::STATE_STOP_E) != 0;
        if (!done) {
            done = (f32)morph->mFrameCtrl.mRate == zero;
        }
        mFlightComplete = (u8)done;
        if (done) {
            mFlightState = 7;
        }
        return 1;
    }
    case 7: {
        s32 type = (s8)mType, angle = (s16)mPropellerSpeed;
        s32 limit = (s16)l_hio.mRecords[type].mFlightStartPropellerSpeed;
        bool ready = angle > limit;
        mFlightComplete = (u8)ready;
        if (ready) {
            setAnm_NUM(10);
            type = (s8)mType;
            s32 timer = (s16)l_hio.mRecords[type].mFlightDuration;
            mFlightState = 1;
            mFlightTimer = (s16)timer;
        }
        return 1;
    }
    default:
        return 1;
    }
}
/* 021F7604 */
void daNpc_Bj1_c::bj_clcMovSpd() {
    if ((s8)(u8)mJumpState == 0) {
        s32 direction = Bj1_targetAngleY(&current.pos, reinterpret_cast<cXyz *>(&mMoveTargetXBits));
        s32 type = (s8)mType;
        Bj1HIORecord *prm = &l_hio.mRecords[type];
        s32 maximum = (s16)prm->mWalkTurnMax;
        s32 scale = (s16)prm->mWalkTurnScale;
        Bj1_addCalcAngle(&current.angle.y, direction, scale, maximum, 0);
    }
    // Match PPC load order: +924 precedes +91C; FPR order is target then step.
    f32 step = (f32)mSpeedStep;
    f32 target = (f32)mTargetSpeed;
    Bj1_chaseF(&speedF, target, step);
}
/* 021F7684 */
void daNpc_Bj1_c::bj_nMove() {
    if ((u8)mWalkComplete != 0) {
        mSkipCollisionMove = 1;
        return;
    }
    s32 state = (s8)(u8)mJumpState;
    if (state == 1) {
        mJumpState = 2;
        return;
    }
    if (state == 2) {
        if (((u32)mObjAcch.m_flags & 0x20) == 0) {
            return;
        }
        setAnm_NUM(8);
        mJumpState = 0;
        f32 speed = (f32)l_zero, fall = (f32)l_gravity;
        speedF = speed;
        gravity = fall;
        return;
    }
    if ((s8)(u8)mWalkState != 1 || state != 0) {
        return;
    }
    bj_clcMovSpd();
    s32 type = (s8)mType;
    f32 speed = (f32)speedF;
    f32 scale = (f32)l_hio.mRecords[type].mWalkAnimationScale;
    f32 animationSpeed = gabi::fmuls_ppc(speed, scale);
    f32 minimum = (f32)l_minimumWalkAnimationSpeed;
    mDoExt_McaMorf* morph = mpMorf;
    if (animationSpeed < minimum) {
        animationSpeed = minimum;
    }
    morph->mFrameCtrl.mRate = animationSpeed;
    u32 result = bj_movPass(0);
    if (result == 1) {
        mWalkComplete = 1;
        return;
    }
    if (result == 2) {
        mWalkState = 0;
        mWalkComplete = 1;
    }
}
/* 021F414C */
void daNpc_Bj1_c::setPrtcl_drugPot_1() {

    gabi::Local<Bj1ParticleFrame> frame;
    s8 room = (s8)current.roomNo;
    if (Bj1_getMapTrans(room, (be<f32>*)(reinterpret_cast<f32*>(&frame->x)),
                        (be<f32>*)(reinterpret_cast<f32*>(&frame->z)),
                        (be<s16>*)(reinterpret_cast<s16*>(&frame->angle))) == 0) {
        return;
    }
    f32 z = (f32)frame->z;
    s16 angle = (s16)frame->angle;
    mPotPosZ = z;
    f32 x = (f32)frame->x;
    mPotAngleY = angle;
    mPotPosX = x;
    f32 zero = (f32)l_zero;
    mPotAngleX = 0;
    mPotAngleZ = 0;
    room = (s8)current.roomNo;
    mPotPosY = zero;
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter = Bj1_createParticle(
            (dPa_control_c*)(control), 0, 0x8176, (const cXyz*)(reinterpret_cast<cXyz*>(&mPotPosX)),
            (const csXyz*)(reinterpret_cast<csXyz*>(&mPotAngleX)), (const cXyz*)((void*)nullptr),
            0xFF, (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
            (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        room = (s8)current.roomNo; // Before storing preceding emitter.
        mPotEmitterA = emitter;
    }
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter = Bj1_createParticle(
            (dPa_control_c*)(control), 0, 0x8177, (const cXyz*)(reinterpret_cast<cXyz*>(&mPotPosX)),
            (const csXyz*)(reinterpret_cast<csXyz*>(&mPotAngleX)), (const cXyz*)((void*)nullptr),
            0xFF, (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
            (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        room = (s8)current.roomNo; // Before storing preceding emitter.
        mPotEmitterB = emitter;
    }
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter = Bj1_createParticle(
            (dPa_control_c*)(control), 0, 0x8178, (const cXyz*)(reinterpret_cast<cXyz*>(&mPotPosX)),
            (const csXyz*)(reinterpret_cast<csXyz*>(&mPotAngleX)), (const cXyz*)((void*)nullptr),
            0xFF, (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
            (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        mPotEmitterC = emitter;
    }
}

/* 021F4558 */
void daNpc_Bj1_c::setPrtcl_drugPot_2() {

    gabi::Local<Bj1ParticleFrame> frame;
    s8 room = (s8)current.roomNo;
    if (Bj1_getMapTrans(room, (be<f32>*)(reinterpret_cast<f32*>(&frame->x)),
                        (be<f32>*)(reinterpret_cast<f32*>(&frame->z)),
                        (be<s16>*)(reinterpret_cast<s16*>(&frame->angle))) == 0) {
        return;
    }
    f32 z = (f32)frame->z;
    s16 angle = (s16)frame->angle;
    mPotPosZ = z;
    f32 x = (f32)frame->x;
    mPotAngleY = angle;
    mPotPosX = x;
    f32 zero = (f32)l_zero;
    mPotAngleX = 0;
    mPotAngleZ = 0;
    room = (s8)current.roomNo;
    mPotPosY = zero;
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter = Bj1_createParticle(
            (dPa_control_c*)(control), 0, 0x822D, (const cXyz*)(reinterpret_cast<cXyz*>(&mPotPosX)),
            (const csXyz*)(reinterpret_cast<csXyz*>(&mPotAngleX)), (const cXyz*)((void*)nullptr),
            0xFF, (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
            (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        mPotPourEmitter = emitter;
    }
}

/* 021F4394 */
void daNpc_Bj1_c::setPrtcl_danceLR() {
    delPrtcl_danceLR();
    s8 room = (s8)current.roomNo;
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter =
            Bj1_createParticle((dPa_control_c*)(control), 0, 0x8223,
                               (const cXyz*)(reinterpret_cast<cXyz*>(&current.pos.x)),
                               (const csXyz*)((void*)nullptr), (const cXyz*)((void*)nullptr), 0xFF,
                               (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
                               (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        room = (s8)current.roomNo; // Before storing first emitter.
        mLeftDanceEmitter = emitter;
    }
    {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        dPa_control_c* control = scene->play.mParticle.get();
        u32 emitter =
            Bj1_createParticle((dPa_control_c*)(control), 0, 0x8224,
                               (const cXyz*)(reinterpret_cast<cXyz*>(&current.pos.x)),
                               (const csXyz*)((void*)nullptr), (const cXyz*)((void*)nullptr), 0xFF,
                               (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
                               (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
        mRightDanceEmitter = emitter;
    }
}

/* 021F4864 */
void daNpc_Bj1_c::flwPrtcl_danceLR() {
    for (u32 i = 0; i < 2; ++i) {
        Bj1Emitter* emitter =
            gabi::at<Bj1Emitter>(i == 0 ? (u32)mLeftDanceEmitter : (u32)mRightDanceEmitter);
        if (emitter != nullptr) {
            mDoExt_McaMorf* morf = mpMorf;
            s8 joint = i == 0 ? (s8)mLeftHandJoint : (s8)mRightHandJoint;
            J3DModel* model = morf->mpModel;
            J3DMtxBuffer_hd* block = model->mpMtxBuffer;
            Mtx34* matrices = gabi::at<Mtx34>(block->mpAnmMtx);
            u16 flags = block->mFlags;
            Mtx34* matrix = &matrices[(u32)(s32)joint];
            block->mFlags = flags | 0x10;
            Bj1_setEmitterMatrix(matrix, &emitter->mRotationMtx, &emitter->mPosition);
        }
    }
}

/* 021F4344 */
void daNpc_Bj1_c::delPrtcl_danceLR() {
    Bj1Emitter* emitter = gabi::at<Bj1Emitter>(mLeftDanceEmitter);
    if (emitter != nullptr) {
        u32 flags = emitter->mStatus;
        emitter->mMaxFrame = -1;
        emitter->mStatus = flags | 1;
        mLeftDanceEmitter = 0;
    }
    emitter = gabi::at<Bj1Emitter>(mRightDanceEmitter);
    if (emitter != nullptr) {
        u32 flags = emitter->mStatus;
        emitter->mMaxFrame = -1;
        emitter->mStatus = flags | 1;
        mRightDanceEmitter = 0;
    }
}

/* 021F4798 */
void daNpc_Bj1_c::setPrtcl_peraProOpen() {
    Mtx34* matrix = bj1_jointMatrix(mpPrpMorf, mPropellerJoint);
    PSMTXCopy(matrix, (Mtx34*)(mDoMtx_stack_c::get()));
    mPropellerPosX = (f32)mDoMtx_stack_c::get()->m[0][3];
    mPropellerPosY = (f32)mDoMtx_stack_c::get()->m[1][3];
    f32 z = (f32)mDoMtx_stack_c::get()->m[2][3];
    s32 room = (s8)current.roomNo;
    mPropellerPosZ = z;
    dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dPa_control_c* control = scene->play.mParticle.get();
    u32 emitter = Bj1_createParticle(
        (dPa_control_c*)(control), 0, 0x830B,
        (const cXyz*)(reinterpret_cast<void*>(&mPropellerPosX)),
        (const csXyz*)(reinterpret_cast<void*>(&current.angle.x)), (const cXyz*)((void*)nullptr),
        255, (void*)((void*)nullptr), room, (const GXColor*)((void*)nullptr),
        (const GXColor*)((void*)nullptr), (const cXyz*)((void*)nullptr));
    mPropellerEmitter = emitter;
}

/* 021F65B8 */
BOOL daNpc_Bj1_c::createSeed() {
    // HD: angles/scale/position, four choices and four actor results share this exact frame.
    gabi::Local<Bj1SeedFrame> frame;
    for (u32 i = 0; i < 4; ++i) {
        frame->mResults[i] = nullptr;
        mSeedIDs[i] = fpcM_ERROR_PROCESS_ID_e;
    }
    mDoExt_McaMorf* morph = mpMorf;
    s32 joint = mRightHandJoint;
    J3DModel* model = morph->mpModel;
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    Mtx34* matrices = gabi::at<Mtx34>(block->mpAnmMtx);
    u16 flags = block->mFlags;
    block->mFlags = flags | 0x10;
    PSMTXCopy(&matrices[(u32)joint], mDoMtx_stack_c::get());
    f32 px = mDoMtx_stack_c::get()->m[0][3];
    f32 addY = l_seedRise;
    f32 randomX = l_one;
    f32 pz = mDoMtx_stack_c::get()->m[2][3];
    f32 randomY = l_seedRandomRise;
    frame->mPosition.x = px;
    f32 speed = l_seedSpeed;
    f32 uniform = l_seedScale;
    f32 py = mDoMtx_stack_c::get()->m[1][3];
    frame->mPosition.z = pz;
    f32 addX = l_seedRandomSpeedBias;
    frame->mPosition.y = py;
    u32 made = 0;
    for (u32 i = 0; i < 4; ++i) {
        frame->mChoices[0] = -0x1000;
        frame->mChoices[3] = 0x1000;
        frame->mChoices[1] = -0xA00;
        frame->mChoices[2] = 0xA00;
        Bj1_initAngles(&frame->mAngles, 0, 0, 0);
        s32 selected = frame->mChoices[i], facing = current.angle.y;
        frame->mScale.y = uniform;
        frame->mScale.x = uniform;
        frame->mScale.z = uniform;
        frame->mAngles.y = (s16)(facing + selected);
        f64 rx = Bj1_rndFX(randomX);
        f64 ry = Bj1_rndFX(randomY);
        f32 vy = (f32)(ry + (f64)addY), vx = (f32)(rx + (f64)addX);
        s32 room = current.roomNo;
        fopAc_ac_c* actor = gabi::at<fopAc_ac_c>(
            Bj1_createItemForKP2(&frame->mPosition, dItemNo_BOKOBABA_SEED_e, room, &frame->mAngles,
                                 &frame->mScale, 1, vx, vy, speed));
        frame->mResults[i] = actor;
        if (actor == nullptr) {
            break;
        }
        u32 actorFlags = actor->actor_status;
        actor->actor_status = actorFlags | fopAcStts_UNK4000_e;
        actor = frame->mResults[i];
        fpc_ProcID id = actor != nullptr ? (fpc_ProcID)actor->mBsPcId : fpcM_ERROR_PROCESS_ID_e;
        ++made;
        mSeedIDs[i] = id;
    }
    return made == 4;
}

/* 021F6040 */
BOOL daNpc_Bj1_c::deleteSeed() { return bj1_deleteSeedBody(this); }

/* 021F3C28 */
BOOL daNpc_Bj1_c::charDecide(int prm) {
    u32 n = (u32)(s32)(fpcM_GetName(this) - fpcNm_NPC_BJ1_e); /* PROC_NPC_BJ1 .. PROC_NPC_BJ9 */
    mType = -1;
    mMessageType = 0xFF;
    if (n > 8) {
        return FALSE;
    }
    if (n == 0) {
        mType = 0;
        mMessageType = (prm == 1);
        return TRUE;
    }
    if (n == 6) {
        mType = 6;
        mMessageType = 0xC;
        return TRUE;
    }
    mType = (s8)n;
    /* types 1..5, 7, 8: the message set is 2n (or 2n+1 with parameter 1); BJ8/BJ9 shifted by one */
    u8 base = n < 6 ? (u8)(2 * n) : (u8)(2 * n - 1);
    mMessageType = prm == 1 ? (u8)(base + 1) : base;
    return TRUE;
}

/* 021F590C */
void daNpc_Bj1_c::eInit_setLocFlag(int* value) {
    u32 v = gabi::ea(value);
    mLookAtPlayer = 0;
    if (v == 0) {
        return;
    }
    u32 flag = *reinterpret_cast<be<u32>*>(value);
    if (flag == 1) {
        mLookAtPlayer = 1;
    } else if (flag == 2) {
        m_jnt.mbTrn = 1;
    }
}

/* 021F594C */
void daNpc_Bj1_c::eInit_setShapeAngleY(int* input, s16 angle) {
    mKeepShapeAngle = 0;
    if (!input) {
        return;
    }
    u32 value = *reinterpret_cast<be<u32>*>(input);
    u8 enabled = (value == 1);
    mKeepShapeAngle = enabled;
    if (enabled) {
        shape_angle.y = angle;
    }
}

/* 021F597C */
void daNpc_Bj1_c::eInit_setEvTimer(int* value) {
    mEventTimer = 0;
    if (value) {
        mEventTimer = (s16) * reinterpret_cast<be<s32>*>(value);
    }
}

/* 021F5794 */
u32 daNpc_Bj1_c::eInit_calcRelativPos(cXyz* destination, cXyz* relative, void* angles) {
    s32 yaw;
    if (angles != nullptr) {
        s32 extra = reinterpret_cast<csXyz*>(angles)->y;
        s32 base = shape_angle.y;
        yaw = (s16)(base + extra);
    } else {
        yaw = shape_angle.y;
    }
    // HD: contiguous hidden-result buffers retain the native call frame.
    gabi::Local<Bj1RelativeFrame> frame;
    f32 z = current.pos.z, x, y;
    if (relative != nullptr) {
        f32 rx = relative->x;
        x = current.pos.x;
        f32 ry = relative->y;
        frame->mInput.x = rx;
        frame->mInput.y = ry;
        f32 rz = relative->z;
        y = current.pos.y;
        frame->mInput.z = rz;
    } else {
        y = current.pos.y;
        f32 zero = l_zero;
        x = current.pos.x;
        frame->mInput.x = zero;
        frame->mInput.z = zero;
        frame->mInput.y = zero;
    }
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), yaw);
    PSMTXMultVec(mDoMtx_stack_c::get(), &frame->mInput, &frame->mOutput);
    if (destination == nullptr) {
        destination = gabi::at<cXyz>(Bj1_allocate(sizeof(cXyz)));
        if (destination == nullptr) {
            return 0;
        }
    }
    auto* src = reinterpret_cast<be<f32>*>(&frame->mOutput);
    auto* dst = reinterpret_cast<be<f32>*>(destination);
    for (u32 i = 0; i < 3; ++i) {
        f32 value = src[i];
        dst[i] = value;
    }
    return gabi::ea(destination);
}

/* 021F5D68 */
f32 daNpc_Bj1_c::eInit_prmFloat(f32* value, f32 fallback) {
    return value ? *reinterpret_cast<be<f32>*>(value) : fallback;
}

/* 021F5998 */
void daNpc_Bj1_c::eInit_ATTENTION_(int* modePtr, int* loc, int* shape, cXyz* relative, int* angles,
                                   int* point, int* timer) {
    if (!modePtr) {
        return;
    }
    u32 mode = *reinterpret_cast<be<u32>*>(modePtr);
    s32 baseYaw = (s16)shape_angle.y;
    gabi::Local<Bj1AttentionFrame> frame;
    cXyz* tmp = reinterpret_cast<cXyz*>(&frame->mPoint);
    auto directionToPoint = [&]() -> s32 {
        return Bj1_targetAngleY((const cXyz*)(reinterpret_cast<void*>(&current.pos.x)),
                                (const cXyz*)(reinterpret_cast<void*>(&mLookTargetXBits)));
    };
    // Chain the actual returned r3, rather than silently restoring this between callees.
    auto finish = [&](u32 carryAngle, bool explicitBase) {
        u32 carry = Bj1_setLocFlagCarry(this, loc, carryAngle);
        u32 angle = explicitBase ? (u32)baseYaw : gabi::cpu->r[5];
        carry = Bj1_setShapeAngleCarry(gabi::at<void>(carry), shape, angle);
        Bj1_setEventTimerCarry(gabi::at<void>(carry), timer);
    };
    switch (mode) {
    case 0:
        finish(gabi::ea(relative), true);
        return;
    case 1: {
        mLookMode = 1;
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        fopAc_ac_c* player = scene->play.mPlayerInfo[0].mpPlayer.get();
        s32 direction = Bj1_targetAngleY((const cXyz*)(reinterpret_cast<void*>(&current.pos.x)),
                                         (const cXyz*)(&player->current.pos));
        finish((u32)direction, false);
        return;
    }
    case 2: {
        if (!relative) {
            return;
        }
        mLookMode = 2;
        s32 direction = Bj1_targetAngleY((const cXyz*)(reinterpret_cast<void*>(&current.pos.x)),
                                         (const cXyz*)(relative));
        finish((u32)direction, false);
        return;
    }
    case 3: {
        mLookMode = 2;
        eInit_calcRelativPos((cXyz*)(tmp), (cXyz*)(relative), (void*)(angles));
        u32 z = (u32)frame->mPoint.z, y = (u32)frame->mPoint.y, x = (u32)frame->mPoint.x;
        Bj1_setFloatBits(mLookTargetY, y);
        mLookTargetXBits = x;
        mLookTargetZBits = z;
        finish((u32)directionToPoint(), false);
        return;
    }
    case 4: {
        if (!angles) {
            return;
        }
        mLookMode = 3;
        s32 direction = (s16) reinterpret_cast<csXyz*>(angles)->y;
        finish((u32)direction, false);
        return;
    }
    case 5: {
        if (gabi::ea(mPath.mPath.get()) == 0) {
            return;
        }
        u32 index = (u8)mPath.mIdx;
        if (point) {
            index = (u8) reinterpret_cast<Bj1EventInteger*>(point)->mLowByte;
        }
        Bj1_getPathPoint((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)), (cXyz*)(tmp), index);
        u32 carryAngle = gabi::cpu->r[5];
        u32 z = (u32)frame->mPoint.z, x = (u32)frame->mPoint.x;
        mLookMode = 2;
        mLookTargetXBits = x;
        u32 y = (u32)frame->mPoint.y;
        mLookTargetZBits = z;
        Bj1_setFloatBits(mLookTargetY, y);
        finish(carryAngle, true);
        return;
    }
    case 6: {
        u32 id = (u32)mPartnerID;
        u32 partner = gabi::ea(searchByID(id));
        if (partner == 0) {
            return;
        }
        mLookMode = 2;
        u32 x = Bj1_floatBits(gabi::at<fopAc_ac_c>(partner)->eyePos.x);
        mLookTargetXBits = x;
        u32 y = Bj1_floatBits(gabi::at<fopAc_ac_c>(partner)->eyePos.y);
        Bj1_setFloatBits(mLookTargetY, y);
        u32 z = Bj1_floatBits(gabi::at<fopAc_ac_c>(partner)->eyePos.z);
        mLookTargetZBits = z;
        finish((u32)directionToPoint(), false);
        return;
    }
    case 7: {
        f32 y = (f32)eyePos.y;
        u32 x = Bj1_floatBits(mPotPosX), z = Bj1_floatBits(mPotPosZ);
        mLookTargetY = y;
        mLookTargetZBits = z;
        mLookMode = 2;
        mLookTargetXBits = x;
        finish((u32)directionToPoint(), false);
        return;
    }
    default:
        mLookMode = 0;
        finish(gabi::ea(relative), true);
        return;
    }
}

/* 021F5C78 */
void daNpc_Bj1_c::eInit_PLYER_MOV_1_() {
    dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = scene->play.mPlayerInfo[0].mpPlayer.get();
    s16 angle = Bj1_targetAngleY((const cXyz*)(reinterpret_cast<cXyz*>(&mInitialPosXBits)),
                                 (const cXyz*)(&player->current.pos));
    s16 delta = (s16)((s32)angle - (s32)(s16)mInitialAngleY);
    s32 absolute = delta < 0 ? -(s32)delta : (s32)delta;

    gabi::Local<Bj1PlayerMoveFrame> local;
    cXyz* output = &local->output;
    cXyz* input = &local->input;
    if (absolute > 4096) {
        f32 x = (f32)current.pos.x;
        f32 y = (f32)current.pos.y;
        f32 z = (f32)current.pos.z;
        PSMTXTrans((Mtx34*)(mDoMtx_stack_c::get()), x, y, z);
        s16 yaw = (s16)mInitialAngleY;
        mDoMtx_YrotM((Mtx34*)(mDoMtx_stack_c::get()), yaw);
        f32 zero = (f32)l_zero;
        f32 distance = (f32)l_playerGoalDistance;
        input->y = zero;
        input->z = distance;
        input->x = zero;
        PSMTXMultVec((const Mtx34*)(mDoMtx_stack_c::get()), (const cXyz*)(input), (cXyz*)(output));
    } else {
        f32 y = (f32)player->current.pos.y;
        f32 z = (f32)player->current.pos.z;
        f32 x = (f32)player->current.pos.x;
        output->z = z;
        output->y = y;
        output->x = x;
    }
    scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    Bj1_setGoal((dEvent_manager_c*)(&scene->play.mEvtManager), (cXyz*)(output));
}

/* 021F5D78 */
void daNpc_Bj1_c::eInit_MOV_(f32* first, f32* second, f32* third, int* timer) {
    u8 pointIndex = (u8)mPath.mIdx;
    setAnm_NUM(8);
    f32 fallback = (f32)l_zero;
    mLookMode = 0;
    f32 value = eInit_prmFloat((f32*)(first), fallback);
    mPointPassDistance = value;
    value = eInit_prmFloat((f32*)(second), fallback);
    mTargetSpeed = value;
    value = eInit_prmFloat((f32*)(third), fallback);
    mSpeedStep = value;

    gabi::Local<Bj1MovePointFrame> result;
    cXyz* out = &result->point;
    Bj1_getPathPoint((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)), (cXyz*)(out), pointIndex);
    u32 z = Bj1_floatBits(out->z);
    u32 x = Bj1_floatBits(out->x);
    mMoveTargetZBits = z;
    u32 y = Bj1_floatBits(out->y);
    mMoveTargetXBits = x;
    Bj1_setFloatBits(mMoveTargetY, y);
    Bj1_setEventTimerCarry(this, timer);
    mWalkComplete = 0;
    mWalkState = 1;
}

/* 021F5E48 */
void daNpc_Bj1_c::eInit_JMP_(f32* speed, f32* gravity) {
    f32 fallback = (f32)l_zero;
    mJumpState = 1;
    f32 value = eInit_prmFloat((f32*)(speed), fallback);
    this->speed.y = value;
    value = eInit_prmFloat((f32*)(gravity), fallback);
    this->gravity = value;
    setAnm_NUM(4);
}

/* 021F5EA4 */
void daNpc_Bj1_c::eInit_CHG_PTH_(int* pathNumber, int* pointNumber) {
    Bj1_setPathInfo((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)), (u8)mPathIndex,
                    (s8)current.roomNo, 1);
    if (gabi::ea(mPath.mPath.get()) && pathNumber) {
        s32 number = *reinterpret_cast<be<s32>*>(pathNumber);
        if (number > 0) {
            u32 path = Bj1_nextPath((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)),
                                    (s8)current.roomNo);
            s32 wanted = number - 1;
            bool found = path != 0;
            for (s32 i = 0; found && i < wanted; ++i) {
                path = Bj1_nextRoomPath((dPath*)(gabi::at<void>(path)), (s8)current.roomNo);
                found = path != 0;
            }
            if (found) {
                Bj1_setPathDirect((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)),
                                  (dPath*)(gabi::at<void>(path)));
            }
        }
    }
    if (pointNumber) {
        u8 point = reinterpret_cast<Bj1EventInteger*>(pointNumber)->mLowByte;
        u32 count = Bj1_maxPoint((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath)));
        if ((u32)point >= count) {
            point = (u8)(Bj1_maxPoint((dNpc_PathRun_c*)(reinterpret_cast<void*>(&mPath))) - 1u);
        }
        mPath.mIdx = point;
    }
}

/* 021F5FBC */
void daNpc_Bj1_c::eInit_END_MOV_() {
    setAnm_NUM(0);
    f32 value = (f32)l_zero;
    mWalkState = 0;
    mTargetSpeed = value;
    speedF = value;
    mSpeedStep = value;
}

/* 021F6008 */
void daNpc_Bj1_c::eInit_SET_TNE_() {
    setAnm_NUM(9);
    mLookMode = 0;
}

/* 021F60AC */
void daNpc_Bj1_c::eInit_SET_ANM_(int* animation, f32* speed) {
    if (!animation) {
        return;
    }
    s32 number = (s8) reinterpret_cast<Bj1EventInteger*>(animation)->mLowByte;
    if ((u32)number >= 11u) {
        return;
    }
    setAnm_NUM(number);
    if (speed) {
        mDoExt_McaMorf* morf = mpMorf;
        f32 value = *reinterpret_cast<be<f32>*>(speed);
        ((mDoExt_McaMorf*)(morf))->setMorf(value);
    }
}

/* 021F6114 */
void daNpc_Bj1_c::event_actionInit(int event) {
    auto get = [&](const char* name, int type) -> u32 {
        dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        return Bj1_getSubstance((dEvent_manager_c*)(&scene->play.mEvtManager), event,
                                (const char*)(name), type);
    };
    u32 selector = get("ActNo", 3);
    if (selector == 0) {
        return;
    }
    u32 value = *gabi::at<be<u32>>(selector);
    s32 action = (s8)value;
    mAttentionFromHead = 0;
    mEventAction = (u8)value;
    switch ((u32)action) {
    case 0: {
        u32 p0 = get("prm_0", 3), p1 = get("prm_1", 3), p2 = get("prm_2", 3);
        u32 p3 = get("Offst", 1), p4 = get("Angle", 3), p5 = get("Index", 3), p6 = get("Timer", 3);
        mAttentionFromHead = 1;
        eInit_ATTENTION_((int*)(gabi::at<void>(p0)), (int*)(gabi::at<void>(p1)),
                         (int*)(gabi::at<void>(p2)), (cXyz*)(gabi::at<void>(p3)),
                         (int*)(gabi::at<void>(p4)), (int*)(gabi::at<void>(p5)),
                         (int*)(gabi::at<void>(p6)));
        return;
    }
    case 1:
        eInit_PLYER_MOV_1_();
        return;
    case 2:
    case 8: {
        u32 p0 = get("GoalR", 0), p1 = get("Speed", 0), p2 = get("Accel", 0), p3 = get("Timer", 3);
        mAttentionFromHead = 1;
        eInit_MOV_((f32*)(gabi::at<void>(p0)), (f32*)(gabi::at<void>(p1)),
                   (f32*)(gabi::at<void>(p2)), (int*)(gabi::at<void>(p3)));
        return;
    }
    case 3: {
        u32 p0 = get("Speed", 0), p1 = get("Grvty", 0);
        mAttentionFromHead = 1;
        eInit_JMP_((f32*)(gabi::at<void>(p0)), (f32*)(gabi::at<void>(p1)));
        return;
    }
    case 4: {
        u32 p0 = get("PthNo", 3), p1 = get("Index", 3);
        eInit_CHG_PTH_((int*)(gabi::at<void>(p0)), (int*)(gabi::at<void>(p1)));
        return;
    }
    case 5:
        eInit_END_MOV_();
        return;
    case 6:
        eInit_SET_TNE_();
        return;
    case 7:
        Bj1_deleteSeedTail(this);
        return;
    case 9: {
        u32 p0 = get("AnmNo", 3), p1 = get("Morfc", 0);
        eInit_SET_ANM_((int*)(gabi::at<void>(p0)), (f32*)(gabi::at<void>(p1)));
        return;
    }
    default:
        return;
    }
}

/* 021F6504 */
u32 daNpc_Bj1_c::eMove_MOV_() {
    if ((s16)mEventTimer >= 0) {
        return Bj1_calcTimer((be<s16>*)(reinterpret_cast<void*>(&mEventTimer))) == 0 ? 1u : 0u;
    }
    return (u32)((u8)m_jnt.mbTrn ^ 1u);
}

/* 021F6558 */
u32 daNpc_Bj1_c::eMove_JMP_() {
    if ((s16)mEventTimer >= 0) {
        return Bj1_calcTimer((be<s16>*)(reinterpret_cast<void*>(&mEventTimer))) == 0 ? 1u : 0u;
    }
    return (u8)mWalkComplete;
}

/* 021F6818 */
BOOL daNpc_Bj1_c::eMove_SET_TNE_() {
    f32 frame = (f32)l_tuneEndFrame;
    mDoExt_McaMorf* morf = mpMorf;
    if (morf->mFrameCtrl.checkPass(frame) != 0) {
        createSeed();
    }
    return (s8)(u8)mAnmEnded != 0;
}
/* 021F6874 */
BOOL daNpc_Bj1_c::eMove_PTH_MOV_() {
    BOOL done = (s8)(u8)mWalkState == 0;
    if (!done) {
        if ((u8)mWalkComplete != 0) {
            u8 point = (u8)mPath.mIdx;
            gabi::Local<Bj1PointFrame> local;
            Bj1_getPathPoint(&mPath, &local->output, point);
            u32 x = Bj1_floatBits(local->output.x);
            u32 z = Bj1_floatBits(local->output.z);
            mMoveTargetXBits = x;
            u32 y = Bj1_floatBits(local->output.y);
            mMoveTargetZBits = z;
            Bj1_setFloatBits(mMoveTargetY, y);
        }
        mWalkComplete = 0;
    }
    return done;
}
/* 021F68F8 */
u32 daNpc_Bj1_c::event_action() {
    switch ((s8)(u8)mEventAction) {
    case 0:
        return eMove_MOV_();
    case 2:
        return eMove_JMP_();
    case 3:
        return bj1_moveStateIsZero(this);
    case 6:
        return eMove_SET_TNE_();
    case 8:
        return eMove_PTH_MOV_();
    default:
        return 1;
    }
}
/* 021F6980 */
void daNpc_Bj1_c::privateCut(int event) {
    if (event == -1) {
        return;
    }
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    s8 cut =
        (s8)Bj1_getActIndex(&scene->play.mEvtManager, event, l_privateCutNames.mNames, 1, 1, 0);
    mPrivateCut = cut;
    scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    if (cut == -1) {
        Bj1_cutEnd(&scene->play.mEvtManager, event);
        return;
    }
    if (Bj1_getIsAddvance(&scene->play.mEvtManager, event) != 0) {
        if ((s8)(u8)mPrivateCut != 0) {
            scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
            Bj1_cutEnd(&scene->play.mEvtManager, event);
            return;
        }
        event_actionInit(event);
    }
    if ((s8)(u8)mPrivateCut == 0) {
        if (event_action() == 0) {
            return;
        }
    }
    scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    Bj1_cutEnd(&scene->play.mEvtManager, event);
}
/* 021F5754 */
void daNpc_Bj1_c::endEvent() {
    dComIfG_inf_c* scene = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    u16 flags = (u16)scene->play.mEvtCtrl.mEventFlag;
    scene->play.mEvtCtrl.mEventFlag = flags | 8;
    mAnmAttribute = 255;
}

/* 021F5714 */
s32 daNpc_Bj1_c::isEventEntry() {
    u32 name = (u32)mEventCut2.mpEvtStaffName;
    u32 player = dComIfGp_ea();
    return Bj1_getMyStaffId((dEvent_manager_c*)(gabi::at<void>(player + 0x52C4u)), name, 0, 0);
}

/* 021F6CC4 */
void daNpc_Bj1_c::event_proc(int argument) {
    s32 index = (s16)mEventIndex;
    s32 event = (s16)mEventIDs[index];
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u32 ended = Bj1_endCheck(&scene->play.mEvtManager, event);
    if (ended != 0) {
        u32 current = (u32)(s32)(s16)mEventIndex;
        if (current == 0) {
            mEventOrder = 1;
            mFirstEventEnded = 1;
        } else if (current == 1) {
            mEventOrder = 1;
            mSecondEventEnded = 1;
        } else if (current == 2) {
            dSv_info_c *save = dComIfGs_info();
            Bj1_onEventBit(&save->mSavedata.mEvent, 0x2902);
        }
        endEvent();
        return;
    }
    u32 processed = mEventCut2.cutProc();
    if (processed == 0) {
        privateCut(argument);
    }
    lookBack();
}
/* 021F3DC4 */
BOOL daNpc_Bj1_c::set_action(Bj1Action* incoming, void* argument) {
    s16 oldIndex = mAction.i;
    s16 newIndex = incoming->i;
    s16 newAdjustment;
    u32 newFunction;
    if (oldIndex == newIndex) {
        if (oldIndex == 0) {
            return TRUE;
        }
        newAdjustment = incoming->d;
        s16 oldAdjustment = mAction.d;
        newFunction = incoming->f;
        if (oldAdjustment == newAdjustment && (u32)mAction.f == newFunction) {
            return TRUE;
        }
    } else {
        newFunction = incoming->f;
        newAdjustment = incoming->d;
    }
    if (oldIndex != 0) {
        mActionPhase = 9;
        (this->*mAction)(argument);
    }
    // HD: capture the incoming descriptor across the old action's callback.
    mAction.f = newFunction;
    mAction.d = newAdjustment;
    mAction.i = newIndex;
    mActionPhase = 0;
    (this->*mAction)(argument);
    return TRUE;
}

/* 021F919C */
void daNpc_Bj1_c::setStt(s8 state) {
    s8 previous = (s8)(u8)mState;
    f32 zero = (f32)l_zero;
    mWaitTimer = 0;
    mState = (u8)state;
    switch ((u32)(s32)state) {
    case BJ1_STATE_TALK:
        if ((s8)(u8)mMessageType == 4) {
            Bj1PartnerState* target = reinterpret_cast<Bj1PartnerState*>(searchByID((u32)mPartnerID));
            if (target != 0) {
                mPartnerTalkAlternate = (u8)target->mTalkAlternate != 0;
            }
        }
        mPreviousState = (u8)previous;
        mAnmAttribute = 255;
        m_jnt.mbTrn = 1;
        mLookMode = BJ1_LOOK_PLAYER;
        return;
    case BJ1_STATE_FLY: {
        Bj1HIORecord *table = &l_hio.mRecords[(s8)mType];
        mLookMode = BJ1_LOOK_NONE;
        mTargetSpeed = (f32)table->mFlightSpeed;
        f32 next = (f32)table->mFlightSpeedStep;
        mFlightComplete = 0;
        mSpeedStep = next;
        mFlightState = 6;
        gravity = zero;
        setAnm_prp(1);
        mPropellerVisible = 1;
        break;
    }
    case BJ1_STATE_FALL:
        mTargetSpeed = zero;
        mLookMode = BJ1_LOOK_PLAYER;
        mLookAtPlayer = 1;
        break;
    case BJ1_STATE_WAIT_2:
        mFlightWaitTimer = 90;
        break;
    case BJ1_STATE_WALK: {
        Bj1HIORecord *table = &l_hio.mRecords[(s8)mType];
        mTargetSpeed = (f32)table->mWalkSpeed;
        mSpeedStep = (f32)table->mWalkSpeedStep;
        f32 next = (f32)table->mWalkPassDistance;
        mWalkState = 1;
        mPointPassDistance = next;
        break;
    }
    default:
        break;
    }
    setAnm();
}
/* 021F9358 */
BOOL daNpc_Bj1_c::wait_1() {
    s8 order = (s8)(u8)mEventOrder;
    if (order == 1 || order >= 3) {
        return 1;
    }
    if ((u8)mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(BJ1_STATE_TALK);
        }
        return 1;
    }
    u8 reset = mAttentionSelected;
    mEventOrder = 2;
    if (reset != 0) {
        mWaitTimer = 60;
    }
    BOOL timer = Bj1_calcTimer(&mWaitTimer);
    if (timer) {
        mLookMode = BJ1_LOOK_PLAYER;
        mLookAtPlayer = 0;
        return 1;
    }
    s16 angle = (s16)mInitialAngleY;
    mLookMode = BJ1_LOOK_INITIAL_HEADING;
    mLookHeading = angle;
    m_jnt.mbTrn = 1;
    return 1;
}
/* 021F9424 */
BOOL daNpc_Bj1_c::wait_2() {
    s8 order = (s8)(u8)mEventOrder;
    if (order == 1 || order >= 3) {
        return 1;
    }
    if ((u8)mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(BJ1_STATE_TALK);
        }
        return 1;
    }
    u8 reset = mAttentionSelected;
    mEventOrder = 2;
    if (reset != 0) {
        mWaitTimer = 60;
        mFlightWaitTimer = 90;
    }
    BOOL timer = Bj1_calcTimer(&mWaitTimer);
    if (timer) {
        mLookMode = BJ1_LOOK_PLAYER;
        mLookAtPlayer = 0;
        return 1;
    }
    if (!Bj1_calcTimer(&mFlightWaitTimer)) {
        setStt(BJ1_STATE_FLY);
    }
    return 1;
}
/* 021F94FC */
BOOL daNpc_Bj1_c::wait_3() {
    Bj1HIORecord *table = &l_hio.mRecords[(s8)mType];
    s32 target = (s16)mInitialAngleY;
    s32 step = (s16)table->mWalkTurnScale;
    s32 maximum = (s16)table->mWalkTurnMax;
    Bj1_addCalcAngle(&current.angle.y, target, step, maximum, 0);
    s8 order = (s8)(u8)mEventOrder;
    if (order == 1 || order >= 3) {
        return 1;
    }
    if ((u8)mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(BJ1_STATE_TALK);
        }
        return 1;
    }
    u8 reset = mAttentionSelected;
    mEventOrder = 2;
    if (reset != 0) {
        mWaitTimer = 60;
    }
    BOOL timer = Bj1_calcTimer(&mWaitTimer);
    if (timer) {
        f32 distance = (f32)l_waitFacingRange;
        if (chk_drct(distance)) {
            mLookMode = BJ1_LOOK_PLAYER;
            mLookAtPlayer = 0;
            return 1;
        }
    }
    s16 angle = (s16)mInitialAngleY;
    mLookMode = BJ1_LOOK_INITIAL_HEADING;
    mLookHeading = angle;
    m_jnt.mbTrn = 1;
    return 1;
}
/* 021F960C */
BOOL daNpc_Bj1_c::wait_4() {
    s8 order = (s8)(u8)mEventOrder;
    if (order == 1 || order >= 3) {
        return 1;
    }
    if ((u8)mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(BJ1_STATE_TALK);
        }
        return 1;
    }
    dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
    if (!Bj1_isEventBit(save, 0x2902)) {
        dSv_player_get_item_c *inventory = &dComIfGs_info()->mSavedata.mPlayer.mGetItem;
        if (Bj1_isItem(inventory, 6, 0)) {
            mEventOrder = 5;
            return 1;
        }
    }
    u8 reset = mAttentionSelected;
    mEventOrder = 2;
    if (reset != 0) {
        mWaitTimer = 60;
    }
    BOOL timer = Bj1_calcTimer(&mWaitTimer);
    if (timer) {
        mLookMode = BJ1_LOOK_PLAYER;
        mLookAtPlayer = 0;
        return 1;
    }
    s16 angle = (s16)mInitialAngleY;
    mLookMode = BJ1_LOOK_INITIAL_HEADING;
    mLookHeading = angle;
    m_jnt.mbTrn = 1;
    return 1;
}
/* 021F9728 */
BOOL daNpc_Bj1_c::flyMov() {
    s32 state = (s8)(u8)mFlightState;
    mEventOrder = 0;
    mFlightComplete = 0;
    if (state != 1) {
        return 1;
    }
    if (Bj1_calcTimer(&mFlightTimer) != 0) {
        return 1;
    }
    gabi::Local<Bj1FlyFrame> frame;
    dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    fopAc_ac_c *player = scene->play.mPlayerInfo[0].mpPlayer;
    Bj1_subtractVector(&current.pos, &frame->mDifference, &player->current.pos);
    f32 x = (f32)frame->mDifference.x, zero = (f32)l_zero;
    frame->mHorizontal.x = x;
    f32 z = (f32)frame->mDifference.z;
    frame->mHorizontal.y = zero;
    frame->mHorizontal.z = z;
    f64 squared = Bj1_squareMagnitude(&frame->mHorizontal);
    f64 distance = Bj1_squareRoot(squared);
    scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    player = scene->play.mPlayerInfo[0].mpPlayer;
    s32 heading = Bj1_targetAngleY(&current.pos, &player->current.pos);
    s32 type = (s8)mType, facing = (s16)current.angle.y;
    s32 delta = (s16)(heading - facing);
    Bj1HIORecord *prm = &l_hio.mRecords[type];
    f32 radius = (f32)prm->mTalkDistance;
    s32 magnitude = (s16)(delta < 0 ? -delta : delta);
    if (distance < (f64)radius) {
        s32 limit = (s16)prm->mTalkAngle;
        if (magnitude < limit) {
            setStt(BJ1_STATE_FALL);
        }
    }
    return 1;
}
/* 021F9844 */
BOOL daNpc_Bj1_c::fall01() {
    s8 phase = (s8)(u8)mFlightState;
    mEventOrder = 0;
    mFlightComplete = 0;
    if (phase == 0) {
        setStt(BJ1_STATE_WAIT_2);
    } else if (phase == 1 && gabi::ftoi((f32)speedF) == 0) {
        f32 zero = (f32)l_zero;
        mFlightState = 2;
        speedF = zero;
    }
    return 1;
}
/* 021F98C4 */
BOOL daNpc_Bj1_c::talk_1() {
    BOOL result = chk_partsNotMove();
    Bj1_talk(this, 1);
    if ((u8)mbHasMsg == 0) {
        return result;
    }
    Bj1MessageManager *message = l_messageManager;
    if (Bj1_messageStatus(message) != 19) {
        return result;
    }
    u32 id = (u32)mCurrMsgNo;
    u32 flag = 0;
    s8 previous;
    bool captured = false;
    switch (id) {
    case 0x140F:
        flag = 0x604;
        break;
    case 0x1410:
        flag = 0x904;
        break;
    case 0x1413:
        flag = 0xC80;
        break;
    case 0x13F3:
        flag = 0x920;
        break;
    case 0x13F9:
        flag = 0xC20;
        break;
    case 0x13FE:
        flag = 0xC08;
        break;
    case 0x13F6:
        flag = 0xC04;
        break;
    case 0x1404:
        flag = 0xC02;
        break;
    case 0x1408:
        flag = 0xD40;
        break;
    case 0x141D:
        flag = 0xD08;
        break;
    case 0x1432:
        flag = 0x1C80;
        break;
    case 0x140C:
        flag = 0xD10;
        break;
    case 0x1409:
        flag = 0xD20;
        break;
    case 0x13EF:
        flag = 0xC10;
        break;
    case 0x1426:
        previous = (s8)(u8)mPreviousState;
        captured = true;
        mCollectedFirstGift = 1;
        break;
    case 0x1429: {
        dComIfG_inf_c *player = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        s16 value = (s16)player->play.mItemBeastNumCounts[1];
        player->play.mItemBeastNumCounts[1] = (s16)(u16)((s32)value - 4);
        previous = (s8)(u8)mPreviousState;
        mEventOrder = 3;
        mTalkRequested = 0;
        mTalkItem = 255;
        setStt((s32)previous);
        mWaitTimer = 60;
        endEvent();
        return result;
    }
    case 0x142B:
        previous = (s8)(u8)mPreviousState;
        captured = true;
        mTalkItem = 255;
        mEventOrder = 4;
        mTalkRequested = 0;
        setStt((s32)previous);
        mWaitTimer = 60;
        endEvent();
        return result;
    case 0x142F:
        previous = (s8)(u8)mPreviousState;
        captured = true;
        mCollectedSecondGift = 1;
        break;
    case 0x1493:
    case 0x149D: {
        dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
        Bj1_onEventBit(save, 0x2E10);
        save = &dComIfGs_info()->mSavedata.mEvent;
        Bj1_onEventBit(save, 0x1D40);
        setReg(0x97FF);
        break;
    }
    case 0x1483:
        setReg(0x96FF);
        break;
    case 0x1485:
        setReg(0x99FF);
        break;
    case 0x1489:
        setReg(0x98FF);
        break;
    default:
        break;
    }
    if (flag != 0) {
        dSv_event_c *save = &dComIfGs_info()->mSavedata.mEvent;
        Bj1_onEventBit(save, flag);
    }
    if (!captured) {
        previous = (s8)(u8)mPreviousState;
    }
    mTalkItem = 255;
    mTalkRequested = 0;
    setStt((s32)previous);
    mWaitTimer = 60;
    endEvent();
    return result;
}
/* 021FA138 */
BOOL daNpc_Bj1_c::walk_1() {
    s8 phase = (s8)(u8)mWalkState;
    mWalkComplete = 0;
    if (phase == 0) {
        u32 y = Bj1_floatBits(current.pos.y);
        u32 x = Bj1_floatBits(current.pos.x);
        f32 zero = (f32)l_zero;
        u16 az = (u16)(s16)current.angle.z;
        mTargetSpeed = zero;
        mInitialAngleZ = (s16)(u16)(az);
        speedF = zero;
        mSpeedStep = zero;
        u16 ax = (u16)(s16)current.angle.x;
        u16 ay = (u16)(s16)current.angle.y;
        mInitialAngleX = (s16)(u16)(ax);
        mInitialAngleY = (s16)(u16)(ay);
        mInitialPosXBits = x;
        u32 z = Bj1_floatBits(current.pos.z);
        mInitialPosYBits = y;
        mInitialPosZBits = z;
        setStt(BJ1_STATE_WAIT_3);
    } else {
        u32 point = (u8)mPath.mIdx;
        gabi::Local<cXyz> position;
        Bj1_getPathPoint(&mPath, position.get(), point);
        u32 z = Bj1_floatBits((*position.get()).z);
        u32 x = Bj1_floatBits((*position.get()).x);
        mMoveTargetZBits = z;
        u32 y = Bj1_floatBits((*position.get()).y);
        mMoveTargetXBits = x;
        Bj1_setFloatBits(mMoveTargetY, y);
        mLookAtPlayer = 1;
    }
    return 1;
}
/* 021FA1FC */
BOOL daNpc_Bj1_c::wait_action1(void *unused) {
    s8 phase = (s8)(u8)mActionPhase;
    if (phase == 0) {
        setStt(BJ1_STATE_WAIT_1);
        u8 current = (u8)mActionPhase;
        mActionPhase = (u8)(current + 1);
        return 1;
    }
    if ((u32)(s32)phase > 3) {
        return 1;
    }
    u32 attention = chkAttention();
    s8 state = (s8)(u8)mState;
    mAttentionSelected = (u8)attention;
    switch (state) {
    case BJ1_STATE_WAIT_1: {
        u32 result = wait_1();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_TALK: {
        u32 result = talk_1();
        mAttentionFromHead = result;
        break;
    }
    default:
        break;
    }
    lookBack();
    return 1;
}
/* 021FA2B8 */
BOOL daNpc_Bj1_c::wait_action2(void *unused) {
    s8 phase = (s8)(u8)mActionPhase;
    if (phase == 0) {
        setStt(BJ1_STATE_WAIT_2);
        u8 current = (u8)mActionPhase;
        mActionPhase = (u8)(current + 1);
        return 1;
    }
    if ((u32)(s32)phase > 3) {
        return 1;
    }
    u32 attention = chkAttention();
    s8 state = (s8)(u8)mState;
    mAttentionSelected = (u8)attention;
    switch (state) {
    case BJ1_STATE_TALK: {
        u32 result = talk_1();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_FLY: {
        u32 result = flyMov();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_FALL: {
        u32 result = fall01();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_WAIT_2: {
        u32 result = wait_2();
        mAttentionFromHead = result;
        break;
    }
    default:
        break;
    }
    lookBack();
    return 1;
}
/* 021FA398 */
BOOL daNpc_Bj1_c::wait_action3(void *unused) {
    s8 phase = (s8)(u8)mActionPhase;
    if (phase == 0) {
        set_pthPoint(0);
        setStt(BJ1_STATE_WALK);
        u8 current = (u8)mActionPhase;
        mActionPhase = (u8)(current + 1);
        return 1;
    }
    if ((u32)(s32)phase > 3) {
        return 1;
    }
    u32 attention = chkAttention();
    s8 state = (s8)(u8)mState;
    mAttentionSelected = (u8)attention;
    switch (state) {
    case BJ1_STATE_TALK: {
        u32 result = talk_1();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_WALK: {
        u32 result = walk_1();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_WAIT_3: {
        u32 result = wait_3();
        mAttentionFromHead = result;
        break;
    }
    default:
        break;
    }
    lookBack();
    return 1;
}
/* 021FA478 */
BOOL daNpc_Bj1_c::wait_action4(void *unused) {
    s8 phase = (s8)(u8)mActionPhase;
    if (phase == 0) {
        set_pthPoint(0);
        setStt(BJ1_STATE_WAIT_4);
        u8 current = (u8)mActionPhase;
        mActionPhase = (u8)(current + 1);
        return 1;
    }
    if ((u32)(s32)phase > 3) {
        return 1;
    }
    u32 attention = chkAttention();
    s8 state = (s8)(u8)mState;
    mAttentionSelected = (u8)attention;
    switch (state) {
    case BJ1_STATE_TALK: {
        u32 result = talk_1();
        mAttentionFromHead = result;
        break;
    }
    case BJ1_STATE_WAIT_4: {
        u32 result = wait_4();
        mAttentionFromHead = result;
        break;
    }
    default:
        break;
    }
    lookBack();
    return 1;
}
/* 021F563C */
BOOL daNpc_Bj1_c::demo() {
    if ((u8)demoActorID == 0) {
        if ((u8)mDemoActive != 0) {
            mDemoActive = 0;
        }
        return 0;
    }
    u32 demoId = (u8)demoActorID;
    mDemoActive = 1;
    if (demoId != 0 && demoId <= 32) {
        u32 manager = gabi::ea(l_demoManager.get());
        if (manager == 0) {
            Bj1_assert((const char*)("d_demo.h"), 0x23A, (const char*)("m_object != (0)"));
            manager = gabi::ea(l_demoManager.get());
        }
        Bj1_getDemoActor((void*)(gabi::at<void>(manager)), demoId);
    }
    mDoExt_McaMorf* morph = mpMorf;
    Bj1_setDemoData((daNpc_Bj1_c*)(this), 0x6A, (mDoExt_McaMorf*)(morph), (const char*)("Bj"), 0, 0,
                    0, 0);
    return (u8)mDemoActive;
}

/* 021F7C74 */
BOOL daNpc_Bj1_c::_draw() {
    u8 hidden = (u8)mHidden;
    mDoExt_McaMorf* morph = mpMorf;
    J3DModel *model =
        morph->mpModel; // This read occurs even when hidden.
    if (hidden != 0 || (u8)mDrawDisabled != 0) {
        return 1;
    }
    dScnKy_env_light_c *lighting = dKy_getEnvlight();
    settingTevStruct(lighting, TEV_TYPE_ACTOR, &current.pos, &tevStr);
    lighting = dKy_getEnvlight();
    setLightTevColorType(lighting, model, &tevStr);
    dComIfGp_ea();
    mpMorf->entryDL();
    if (mpLeftHandModel.v.get() != 0) {
        lighting = dKy_getEnvlight();
        setLightTevColorType(lighting, mpLeftHandModel.get(), &tevStr);
        mDoExt_modelEntryDL(mpLeftHandModel.get());
    }
    if (mpRightHandModel.v.get() != 0) {
        lighting = dKy_getEnvlight();
        setLightTevColorType(lighting, mpRightHandModel.get(), &tevStr);
        mDoExt_modelEntryDL(mpRightHandModel.get());
    }
    lighting = dKy_getEnvlight();
    setLightTevColorType(lighting, mpMaskModel.get(), &tevStr);
    mDoExt_modelEntryDL(mpMaskModel.get());
    if ((u8)mPropellerVisible != 0) {
        mDoExt_McaMorf* prop = mpPrpMorf;
        lighting = dKy_getEnvlight();
        J3DModel *propModel = prop->mpModel;
        setLightTevColorType(lighting, propModel, &tevStr);
        mpPrpMorf->entryDL();
    }
    s8 type = (s8)mType;
    if ((u32)(s32)type <= 8) {
        f32 scale = (f32)l_one;
        u32 color = (u8)l_figureColors.mColors[type];
        Bj1_registerFigure(color, this, scale, scale, scale);
        type = (s8)mType;
    }
    if ((u8)l_hio.mRecords[type].mDrawDebug == 0) {
        return 1;
    }
    if ((u32)l_firstDebugGuard == 0) {
        l_firstDebugGuard = 1;
        memcpy_g(&l_debugColors.mFirst, &l_debugDefaults.mColors[0], sizeof(Bj1GuestWord));
    }
    if ((u32)l_secondDebugGuard == 0) {
        l_secondDebugGuard = 1;
        memcpy_g(&l_debugColors.mSecond, &l_debugDefaults.mColors[1], sizeof(Bj1GuestWord));
    }
    if ((u32)l_thirdDebugGuard == 0) {
        l_thirdDebugGuard = 1;
        memcpy_g(&l_debugColors.mThird, &l_debugDefaults.mColors[2], sizeof(Bj1GuestWord));
    }
    if ((s8)(u8)mMessageType == 6 && (u32)l_thirdDebugGuard == 0) {
        l_thirdDebugGuard = 1;
        memcpy_g(&l_debugColors.mThird, &l_debugDefaults.mColors[2], sizeof(Bj1GuestWord));
    }
    if ((u32)l_fourthDebugGuard == 0) {
        l_fourthDebugGuard = 1;
        memcpy_g(&l_debugColors.mFourth, &l_debugDefaults.mColors[3], sizeof(Bj1GuestWord));
    }
    return 1;
}
/* 021F7944 */
BOOL daNpc_Bj1_c::_execute() {
    if ((u8)mInitialTransformSaved == 0) {
        u16 ax = (u16)(s16)current.angle.x, ay = (u16)(s16)current.angle.y;
        mInitialAngleX = (s16)(u16)(ax);
        u32 x = Bj1_floatBits(current.pos.x), y = Bj1_floatBits(current.pos.y);
        mInitialPosXBits = x;
        u16 az = (u16)(s16)current.angle.z;
        mInitialAngleY = (s16)(u16)(ay);
        mInitialAngleZ = (s16)(u16)(az);
        u32 z = Bj1_floatBits(current.pos.z);
        mInitialPosYBits = y;
        mInitialPosZBits = z;
        mInitialTransformSaved = 1;
    }
    Bj1HIORecord *table = &l_hio.mRecords[(s8)mType];
    s32 p5 = (s16)table->mMaxBackboneY, p6 = (s16)table->mMinBackboneX,
        p7 = (s16)table->mMinBackboneY;
    s32 p9 = (s16)table->mMaxHeadY, p8 = (s16)table->mMaxHeadX, p4 = (s16)table->mMaxBackboneX;
    s32 p11 = (s16)table->mMinHeadY, p10 = (s16)table->mMinHeadX, p12 = (s16)table->mMaxTurnStep;
    m_jnt.setParam(p4, p5, p6, p7, p8, p9, p10, p11, p12);
    if ((u8)mHidden != 0 && (u8)demoActorID == 0) {
        return 1;
    }
    mSkipCollisionMove = 0;
    mHidden = 0;
    partner_srch();
    checkOrder();
    if (!demo()) {
        dComIfG_inf_c *scene = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
        bool eventPath = false;
        if ((u8)scene->play.mEvtCtrl.mMode != 0 && (u16)eventInfo.mCommand != dEvtCmd_INTALK_e) {
            s32 staff = isEventEntry();
            if (staff >= 0) {
                event_proc(staff);
                eventPath = true;
            }
        }
        if (!eventPath) {
            (this->*mAction)(nullptr); // HD: retain the native unconditional PTMF dispatch.
        }
        if (!bj_flyMove()) {
            bj_nMove();
        }
        if ((u8)mSkipCollisionMove == 0) {
            Bj1_moveActor(this, &mStts);
        }
        if ((u8)mKeepShapeAngle == 0) {
            u16 x = (u16)(s16)current.angle.x, y = (u16)(s16)current.angle.y;
            shape_angle.x = (s16)(u16)(x);
            u16 z = (u16)(s16)current.angle.z;
            shape_angle.y = (s16)(u16)(y);
            shape_angle.z = (s16)(u16)(z);
        }
    }
    eventOrder();
    if ((u8)mPropellerVisible != 0) {
        s8 type = (s8)mType;
        s32 angle = (s16)mPropellerAngle, delta = (s16)mPropellerSpeed;
        f32 zero = (f32)l_zero;
        table = &l_hio.mRecords[type];
        mPropellerAngle = (s16)(u16)((u16)(angle + delta));
        s32 denominator = (s16)table->mPropellerSpeedMax;
        f64 value = zero;
        if (denominator != 0) {
            delta = (s16)mPropellerSpeed;
            f64 bias = (f64)l_integerBias;
            f32 numerator =
                (f32)(u64_as_f64(0x4330000000000000ull | ((u32)delta ^ 0x80000000u)) - bias);
            f32 divisor =
                (f32)(u64_as_f64(0x4330000000000000ull | ((u32)denominator ^ 0x80000000u)) - bias);
            f32 ratio = (f32)((f64)numerator / (f64)divisor);
            f32 scale = (f32)l_propellerSoundScale;
            f32 scaled = (f32)((f64)ratio * round25((f64)scale));
            f32 comparison = (f32)((f64)scaled - (f64)scale);
            value = ppc_fsel(comparison, scaled, scale);
            value = ppc_fsel(value, zero, value);
        }
        f32 split = (f32)l_unsignedFloatSplit;
        u32 soundValue;
        if (value < (f64)split) {
            soundValue = (u32)gabi::ftoi(value);
        } else {
            soundValue = (u32)gabi::ftoi((f32)(value - (f64)split)) + 0x80000000u;
        }
        s32 room = (s8)current.roomNo;
        s32 reverb = dComIfGp_getReverb(room);
        Bj1_startSound(0x50BF, &current.pos, soundValue, reverb);
    }
    setMtx(0);
    if ((u8)mDemoActive == 0) {
        setCollision_SP_();
    }
    return 1;
}
/* 021F53D8 */
BOOL daNpc_Bj1_c::_delete() {
    dComIfG_resDelete(&mPhs, "Bj");
    if (gabi::ea(heap.get()) != 0) {
        mDoExt_McaMorf* morph = mpMorf;
        if (morph) {
            ((mDoExt_McaMorf*)(morph))->stopZelAnime();
        }
        morph = mpPrpMorf;
        if (morph) {
            ((mDoExt_McaMorf*)(morph))->stopZelAnime();
        }
    }
    Bj1_deleteDrugPot((daNpc_Bj1_c*)(this)); // Immediate leaf preserves incoming r3.
    delPrtcl_danceLR();
    return 1;
}

/* 021F3C24 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Bj1_c*>(i_this)->CreateHeap();
}

/* 021F5220 */
cPhs_State daNpc_Bj1_c::_create() {
    u32 actor = gabi::ea(this);
    u32 status = (u32)actor_condition;
    if ((status & fopAcCnd_INIT_e) == 0) {
        if (actor != 0) {
            __ct(static_cast<fopNpc_npc_c*>(this));
            __vtbl = BJ1_VTBL;
            Bj1_initEventCut((dNpc_EventCut_c*)(reinterpret_cast<void*>(&mEventCut2)));
            status = (u32)actor_condition;
        }
        actor_condition = status | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Bj");
    if (phase != cPhs_COMPLEATE_e) {
        return (cPhs_State)phase;
    }
    u32 subtype = reinterpret_cast<Bj1EventInteger*>(&mParameters)->mLowByte;
    if (!charDecide(subtype)) {
        return cPhs_ERROR_e;
    }
    s32 type = (s8)mType;
    u32 heapSize = (u32)l_heapSizes.mSizes[type];
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, heapSize)) {
        return cPhs_ERROR_e;
    }
    mDoExt_McaMorf* morph = mpMorf;
    u32 model = gabi::ea(morph->mpModel.get());
    u32 cullMatrix = model != 0 ? model + 0xC8u : 0;
    f32 upperXZ = (f32)l_cullUpperXZ;
    f32 lowerXZ = (f32)l_cullLowerXZ;
    f32 upperY = (f32)l_cullUpperY;
    f32 lowerY = (f32)l_cullLowerY;
    cullMtx = cullMatrix;
    fopAcM_setCullSizeBox(this, lowerXZ, lowerY, lowerXZ, upperXZ, upperY, upperXZ);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return (cPhs_State)phase;
}

/* 021F3244 */
J3DModelData* daNpc_Bj1_c::create_Anm() {
    u32 t = (u32)(s32)mType;
    J3DModelData* modelData;
    if (t < 3) {
        if (t == 1) {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
        } else if (t == 2) {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x18);
        } else {
            modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
        }
    } else if (t == 3) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
    } else if (t < 6) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
    } else if (t < 8) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x18);
    } else if (t == 8) {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x17);
    } else {
        modelData = (J3DModelData*)bj1_getObjectIDRes(0x26);
    }
    if (modelData == nullptr) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1119, "a_mdl_dat != (0)");
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bj1_getObjectIDRes(0xA);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020203);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->mpModel.get() == nullptr) {
        if (morf != nullptr) {
            bj1_morfDelete(morf);
        }
        mpMorf = nullptr;
        return nullptr;
    }
    mHeadJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "head");
    if (mHeadJoint < 0) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1133, "m_hed_jnt_num >= 0");
    }
    mBackboneJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "backbone");
    if (mBackboneJoint < 0) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1136, "m_bbone_jnt_num >= 0");
    }
    mLeftHandJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "armL2");
    if (mLeftHandJoint < 0) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x1139, "m_armL2_jnt_num >= 0");
    }
    mRightHandJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "armR2");
    if (mRightHandJoint < 0) {
        JUT_ASSERT_fail(BJ1_ASSERT_FILE, 0x113C, "m_armR2_jnt_num >= 0");
    }
    return modelData;
}

/* 021F3564 */
J3DModelData* daNpc_Bj1_c::create_prp_Anm() {
    J3DModelData* modelData = (J3DModelData*)bj1_getIDRes("Bj", 0x29);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x1159, "a_mdl_dat != (0)");
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bj1_getIDRes("Bj", 0x28);
    mpPrpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2, 1.0f, 0, -1, 1,
                                       nullptr, 0x80000, 0x11000022);
    mDoExt_McaMorf* morf = mpPrpMorf;
    if (morf == nullptr) {
        return nullptr;
    }
    if (morf->mpModel.get() == nullptr) {
        if (morf != nullptr) {
            bj1_morfDelete(morf);
        }
        mpPrpMorf = nullptr;
        return nullptr;
    }
    mPropellerJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "center");
    if (mPropellerJoint < 0) {
        JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x116F, "m_cen_jnt_num >= 0");
    }
    mPropellerRootJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), "stalk");
    if (mPropellerRootJoint < 0) {
        JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x1172, "m_stalk_jnt_num >= 0");
    }
    return modelData;
}

/* 021F374C */
BOOL daNpc_Bj1_c::create_itm_Mdl() {
    mpRightHandModel = nullptr;
    mpMaskModel = nullptr;
    mpLeftHandModel = nullptr;
    s32 id = bj1_itmTblA(mType);
    if (id >= 0) {
        J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, id);
        if (data == nullptr) {
            JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x11B2, "a_mdl_dat != (0)");
        }
        mpLeftHandModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
        if (mpLeftHandModel.get() == nullptr) {
            return FALSE;
        }
        if (bj1_itmTblA(mType) == 0x19) {
            mLeftItemJoint = JUTNameTab_getIndex(J3DModelData_getJointName(data), "plant_al");
            if (mLeftItemJoint < 0) {
                JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x11C0, "m_plant_l_jnt_num >= 0");
            }
        }
    }
    id = bj1_itmTblB(mType);
    if (id >= 0) {
        J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, id);
        if (data == nullptr) {
            JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x11C7, "a_mdl_dat != (0)");
        }
        mpRightHandModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
        if (mpRightHandModel.get() == nullptr) {
            return FALSE;
        }
        if (bj1_itmTblB(mType) == 0x1A) {
            mRightItemJoint = JUTNameTab_getIndex(J3DModelData_getJointName(data), "plant_ar");
            if (mRightItemJoint < 0) {
                JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x11D5, "m_plant_r_jnt_num >= 0");
            }
        }
    }
    J3DModelData* data = (J3DModelData*)bj1_getIDRes(BJ1_ITM_ARC, bj1_itmTblC(mType));
    if (data == nullptr) {
        JUT_ASSERT_fail("d_a_npc_bj1.cpp", 0x11DB, "a_mdl_dat != (0)");
    }
    mpMaskModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    return mpMaskModel.get() != nullptr;
}

/* 021F39FC */
BOOL daNpc_Bj1_c::CreateHeap() {
    J3DModelData* modelData = create_Anm();
    if (modelData == nullptr) {
        return FALSE;
    }
    J3DModelData* prpData = create_prp_Anm();
    if (prpData == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    if (!create_itm_Mdl()) {
        mpMorf = nullptr;
        mpPrpMorf = nullptr;
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointCount(prpData); i++) {
        if (i == (u32)(s32)mPropellerJoint || i == (u32)(s32)mPropellerRootJoint) {
            bj1_setJointCallBack(mpPrpMorf, i, nodeCallBack_Prp);
        }
    }
    mpPrpMorf->mpModel->mUserArea = gabi::ea(this); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointCount(modelData); i++) {
        if (i == (u32)(s32)mHeadJoint || i == (u32)(s32)mBackboneJoint ||
            i == (u32)(s32)mRightHandJoint) {
            bj1_setJointCallBack(mpMorf, i, nodeCallBack_Bj1);
        }
    }
    mpMorf->mpModel->mUserArea = gabi::ea(this); /* setUserArea */
    mAcchCir.SetWall(30.0f, 40.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return TRUE;
}

/* 021F5360 */
static cPhs_State daNpc_Bj1_Create(daNpc_Bj1_c* self) { return ((daNpc_Bj1_c*)(self))->_create(); }

/* 021F544C */
static BOOL daNpc_Bj1_Delete(daNpc_Bj1_c* self) { return ((daNpc_Bj1_c*)(self))->_delete(); }

/* 021F7C70 */
static BOOL daNpc_Bj1_Execute(daNpc_Bj1_c *self) { return self->_execute(); }
/* 021F7EE0 */
static BOOL daNpc_Bj1_Draw(daNpc_Bj1_c *self) { return self->_draw(); }
/* 021F7EE4 */
static BOOL daNpc_Bj1_IsDelete(fopAc_ac_c *actor) { return 1; }

// HD: compiler-generated and template code of this translation unit.
/* 021F65A4 */
static BOOL bj1_moveStateIsZero(daNpc_Bj1_c* actor) { return (s8)actor->mJumpState == 0; }

/* 021F5364 */
static u32 bj1_drugpotCleanupEntry(u32 actor) {
    daNpc_Bj1_c* self = gabi::at<daNpc_Bj1_c>(actor);
    auto* slots = reinterpret_cast<Bj1PotEmitters*>(&self->mPotEmitterA);
    for (u32 i = 0; i < 3; ++i) {
        Bj1Emitter* pot = gabi::at<Bj1Emitter>(slots->mEmitters[i]);
        if (pot != nullptr) {
            u32 flags = pot->mStatus;
            pot->mMaxFrame = -1;
            pot->mStatus = flags | 1;
            slots->mEmitters[i] = 0;
        }
    }
    // HD: the native leaf preserves the incoming result register.
    return actor;
}

/* 021F60A8 */
static BOOL bj1_deleteSeedTailAlias(daNpc_Bj1_c* actor) {
    return ((daNpc_Bj1_c*)(actor))->deleteSeed();
}

/* 021FA534 */
static u32 bj1_parameterRecordCtor(void *storage) {
    u32 a = gabi::ea(storage);
    if (a == 0) {
        a = Bj1_allocate(sizeof(Bj1HIORecord));
        if (a == 0) {
            return 0;
        }
    }
    gabi::at<Bj1HIORecord>(a)->mVtable = BJ1_PARAMETER_VTBL;
    return a;
}
/* 021FA574 */
static u32 bj1_parameterCollectionCtor(void *storage) {
    u32 a = gabi::ea(storage);
    if (a == 0) {
        a = Bj1_allocate(sizeof(Bj1ParameterCollection));
        if (a == 0) {
            return 0;
        }
    }
    Bj1ParameterCollection *collection = gabi::at<Bj1ParameterCollection>(a);
    collection->mVtable = BJ1_COLLECTION_VTBL;
    Bj1_constructArray(collection->mRecords, 9, sizeof(Bj1HIORecord), bj1_parameterRecordCtor);
    for (u32 i = 0; i < 9; ++i) {
        Bj1HIORecord *record = &collection->mRecords[i];
        record->mIndex = i;
        memcpy_g(&record->mMaxHeadX, &l_parameterDefaults.mRecords[i],
                 sizeof(Bj1ParameterDefaults));
    }
    collection->mSelected = -1;
    collection->mHostIONode = 0xFF;
    return a;
}
/* 021FA624 */
static void bj1_sinit() {
    l_constructorState.mWords[2] = 0;
    l_constructorState.mWords[0] = 0;
    l_constructorState.mWords[3] = 0;
    l_constructorState.mWords[1] = 0;
    Bj1_registerGlobal(&l_globalRegistrations.mRecords[0]);
    f32 lower = (f32)l_initialLowerValue, upper = (f32)l_initialUpperValue;
    l_initialRange.mLower = lower;
    l_initialRange.mUpper = upper;
    Bj1_initializeRuntimeA(&l_runtimeInitializerA);
    Bj1_registerGlobal(&l_globalRegistrations.mRecords[1]);
    Bj1_initializeRuntimeB(&l_runtimeInitializerB);
    Bj1_registerGlobal(&l_globalRegistrations.mRecords[2]);
    // HD: the owner maps the nine HIO records; the collection header precedes them.
    bj1_parameterCollectionCtor(gabi::at<Bj1ParameterCollection>(
        gabi::ea(&l_hio) - offsetof(Bj1ParameterCollection, mRecords)));
}
/* 021FA6C4 */
static u32 bj1_safeStringDestructor(void *storage, u32 flags) {
    u32 a = gabi::ea(storage);
    if (a == 0 || (flags & 1) == 0) {
        return a;
    }
    return Bj1_deallocateWithFlags(storage, flags);
}
/* 021FA6D8 */
static u32 bj1_actorDestructor(void *storage, u32 flags) {
    u32 a = gabi::ea(storage);
    if (a == 0) {
        return 0;
    }
    daNpc_Bj1_c *actor = static_cast<daNpc_Bj1_c *>(storage);
    __dt(&actor->mCyl);
    __dt(&actor->mStts);
    __dt(&actor->mAcchCir);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = BJ1_GROUP_PASS_VTBL;
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = BJ1_POLY_PASS_VTBL;
    Bj1_destroyAcch(&actor->mObjAcch, 0);
    u32 result = Bj1_destroyActor(actor, 0);
    if ((flags & 1) != 0) {
        result = Bj1_deallocate(storage);
    }
    return result;
}
/* 021FA774 */
static u32 bj1_safeStringEmptyVirtual(void *storage) { return gabi::ea(storage); }
// clang-format off
/* WWHD glue
str 10016EC4 "Bj" in 021F70C0
str 10016C48 "a_mdl_dat != (0)" in 021F3244
str 10016C5C "m_hed_jnt_num >= 0" in 021F3244
str 10016C7C "m_bbone_jnt_num >= 0" in 021F3244
str 10016C94 "m_armL2_jnt_num >= 0" in 021F3244
str 10016CAC "m_armR2_jnt_num >= 0" in 021F3244
str 10016C2C "Bj" in 021F3244
str 10016C38 "d_a_npc_bj1.cpp" in 021F3244
str 10016CD8 "d_a_npc_bj1.cpp" in 021F3564
str 10016CE8 "a_mdl_dat != (0)" in 021F3564
str 10016CD8 "d_a_npc_bj1.cpp" in 021F3564
str 10016CFC "m_cen_jnt_num >= 0" in 021F3564
str 10016CD8 "d_a_npc_bj1.cpp" in 021F3564
str 10016D10 "m_stalk_jnt_num >= 0" in 021F3564
str 10016CCC "Bj" in 021F3564
str 10016D2C "d_a_npc_bj1.cpp" in 021F374C
str 10016D3C "a_mdl_dat != (0)" in 021F374C
str 10016D2C "d_a_npc_bj1.cpp" in 021F374C
str 10016D5C "m_plant_l_jnt_num >= 0" in 021F374C
str 10016D2C "d_a_npc_bj1.cpp" in 021F374C
str 10016D3C "a_mdl_dat != (0)" in 021F374C
str 10016D2C "d_a_npc_bj1.cpp" in 021F374C
str 10016D80 "m_plant_r_jnt_num >= 0" in 021F374C
str 10016D2C "d_a_npc_bj1.cpp" in 021F374C
str 10016D3C "a_mdl_dat != (0)" in 021F374C
str 10016D28 "Bj" in 021F374C
str 10016DCC "Bj" in 021F4468
str 10016E1B "Bj" in 021F53D8
str 10016E18 "Bj" in 021F5220
str 10016BF8 "d_demo.h" in 021F563C
str 10016BDC "m_object != (0)" in 021F563C
str 10016E1E "Bj" in 021F563C
ext 026067F4 void* Bj1_getResource(dRes_control_c* manager, SafeString* key, s32 id)
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34* matrix, s16 x, s16 y, s16 z)
ext 028E9108 void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* result)

data l_figureColors 10016ED4
data l_privateCutNames 101BBE9C
data l_debugDefaults 10016B70
data l_debugColors 101FEBE8
data l_firstDebugGuard 101FDA50
data l_secondDebugGuard 101FDAC0
data l_thirdDebugGuard 101FDA48
data l_fourthDebugGuard 101FDA44
data l_constructorState 10465D88
data l_initialRange 10465D7C
data l_globalRegistrations 101BC3AC
data l_runtimeInitializerA 10465D84
data l_runtimeInitializerB 10465D85
data l_hio 10465DB0
data l_parameterDefaults 101BC028
data l_messageManager 101F4B5C
data l_demoManager 101D5FFC
data l_cullUpperXZ 10016E0C
data l_cullLowerXZ 10016E08
data l_cullUpperY 10016E14
data l_cullLowerY 10016E10
data l_propellerEndFrame 10016EC0
data l_degreesToAngle 10016EE0
data l_minimumWalkAnimationSpeed 10016EC8
data l_tuneEndFrame 10016EB4
data l_waitFacingRange 10016EE4
data l_propellerSoundScale 10016ECC
data l_unsignedFloatSplit 10016ED0
data l_initialLowerValue 10016EE8
data l_initialUpperValue 10016EEC
data l_cylinderSource 101EA190
data l_cylinderDimensions 101BBEA0

ext 021F5364 void Bj1_deleteDrugPot(daNpc_Bj1_c* arg0)
ext 02515F14 void Bj1_initCollisionStatus(dCcD_Stts* arg0, u32 arg1, u32 arg2, daNpc_Bj1_c* arg3)
ext 02516518 void Bj1_setCollisionCylinder(dCcD_Cyl* arg0, const void* arg1)
ext 025A847C u32 Bj1_createParticle(dPa_control_c* arg0, u32 arg1, u32 arg2, const cXyz* arg3, const csXyz* arg4, const cXyz* arg5, u32 arg6, void* arg7, s32 arg8, const GXColor* arg9, const GXColor* arg10, const cXyz* arg11)
ext 025E535C u32 Bj1_playMorf(mDoExt_McaMorf* arg0, cXyz* arg1, u32 arg2, s32 arg3)

data l_leftItemResources 101BBBC4
data l_rightItemResources 101BBBE8
data l_maskResources 101BBC0C
data l_animations 101BBC30
data l_stateAnimations 101BBEE8
data l_attributeAnimations 101BBF78
data l_animationResources 10016DA0
data l_scales 101BBCF0
data l_eventNames 101BBE34
data l_staffNames 101BBBB8
data l_heapSizes 101BBE78
data l_waitAction 10016B40
data l_flightAction 10016B48
data l_potAction 10016B50
data l_otherAction 10016B58

data l_zero 10016C08
data l_one 10016C18
data l_jumpHoldFrame 10016DD0
data l_integerBias 10016DD8
data l_gravity 10016DE0
data l_playerGoalDistance 10016E24
data l_seedScale 10016EA0
data l_seedSpeed 10016EA4
data l_seedRise 10016EA8
data l_seedRandomRise 10016EAC
data l_seedRandomSpeedBias 10016EB0
str 10016E28 "ActNo"
str 10016E30 "prm_0"
str 10016E38 "prm_1"
str 10016E40 "prm_2"
str 10016E48 "Offst"
str 10016E50 "Angle"
str 10016E58 "Index"
str 10016E60 "Timer"
str 10016E68 "GoalR"
str 10016E70 "Speed"
str 10016E78 "Accel"
str 10016E80 "Grvty"
str 10016E88 "PthNo"
str 10016E90 "AnmNo"
str 10016E98 "Morfc"

ext 0200F93C s32 Bj1_targetAngleY(const cXyz* arg0, const cXyz* arg1)
ext 02019918 f64 Bj1_rndFX(f32 arg0)
ext 0201A478 void Bj1_initAngles(csXyz* arg0, s32 arg1, s32 arg2, s32 arg3)
ext 0200F378 void Bj1_addCalcAngle(be<s16>* angle, s32 target, s32 scale, s32 maximum, s32 minimum)
ext 0200F428 void Bj1_addCalcAngle2(be<s16>* angle, s32 target, s32 scale, s32 maximum)
ext 0200F5C8 void Bj1_chaseF(be<f32>* value, f32 target, f32 step)
ext 028EFFD0 void Bj1_constructArray(void* storage, u32 count, u32 size, Bj1RecordConstructor constructor)
ext 02543280 void Bj1_cutEnd(dEvent_manager_c* manager, s32 staff)
ext 025447C8 u32 Bj1_getIsAddvance(dEvent_manager_c* manager, s32 staff)
ext 025440C8 u32 Bj1_endCheck(dEvent_manager_c* manager, s32 event)
ext 0201ADE0 void Bj1_subtractVector(const cXyz* left, cXyz* result, const cXyz* right)
ext 028E8DD0 f64 Bj1_squareMagnitude(const cXyz* vector)
ext 028F4384 f64 Bj1_squareRoot(f64 value)
ext 0257E5D4 s32 Bj1_windDirection()
ext 0259D54C void Bj1_playerEyePos(cXyz* result, f32 distance)
ext 0259DED0 void Bj1_lookAtTarget(dNpc_JntCtrl_c* joints, be<s16>* angle, cXyz* target, cXyz* origin, s32 heading, s32 limit, u32 flag)
ext 025F795C u32 Bj1_messageStatus(Bj1MessageManager* message)
ext 025A11EC void Bj1_talk(fopNpc_npc_c* actor, s32 mode)
ext 025B5C54 u32 Bj1_checkEmptyBottle(dSv_player_item_c* items)
ext 025B5D40 BOOL Bj1_isItem(dSv_player_get_item_c* items, s32 item, u32 slot)
ext 024EDFCC u32 Bj1_lockonTruth(dAttention_c* attention)
ext 024EC8D0 fopAc_ac_c* Bj1_actionTarget(dAttention_c* attention, s32 index)
ext 024EE464 fopAc_ac_c* Bj1_lockonTarget(dAttention_c* attention, s32 index)
ext 02544950 u32 Bj1_checkPresentEnd(dEvent_manager_c* manager)
ext 025BED80 void Bj1_registerFigure(u32 color, fopAc_ac_c* actor, f32 x, f32 y, f32 z)
ext 025D6870 void Bj1_moveActor(fopAc_ac_c* actor, dCcD_Stts* status)
ext 028F026C void Bj1_registerGlobal(Bj1GlobalRegistration* record)
ext 028ED6F8 void Bj1_initializeRuntimeA(Bj1RuntimeInitializer* state)
ext 028EAB2C void Bj1_initializeRuntimeB(Bj1RuntimeInitializer* state)
ext 0273AF40 u32 Bj1_deallocate(void* storage)
ext 0273AF40 u32 Bj1_deallocateWithFlags(void* storage, u32 flags)
ext 024EFD9C void Bj1_destroyAcch(dBgS_ObjAcch* collision, u32 flags)
ext 025D50BC u32 Bj1_destroyActor(fopAc_ac_c* actor, u32 flags)
ext 02542EDC u32 Bj1_getActIndex(dEvent_manager_c* manager, s32 staff, const gptr<const char>* names, s32 count, u32 force, u32 prefix)
ext 02055B64 u32 Bj1_calcTimer(be<s16>* arg0)
ext 024EECAC u32 Bj1_getMaterialSound(dBgS* arg0, cBgS_PolyInfo* arg1)
ext 024EEEB8 u32 Bj1_getPolyColor(dBgS* arg0, cBgS_PolyInfo* arg1)
ext 024EF130 u32 Bj1_getRoomId(dBgS* arg0, cBgS_PolyInfo* arg1)
ext 024F08A8 void Bj1_correctPosition(dBgS_ObjAcch* arg0, dBgS* arg1)
ext 02520630 s32 Bj1_getMapTrans(s32 arg0, be<f32>* arg1, be<f32>* arg2, be<s16>* arg3)
ext 02526E70 u32 Bj1_getDemoActor(void* arg0, u32 arg1)
ext 02527028 void Bj1_setDemoData(daNpc_Bj1_c* arg0, u32 arg1, mDoExt_McaMorf* arg2, const char* arg3, u32 arg4, u32 arg5, u32 arg6, u32 arg7)
ext 02542D88 s32 Bj1_getMyStaffId(dEvent_manager_c* arg0, u32 arg1, u32 arg2, u32 arg3)
ext 02543714 void Bj1_setGoal(dEvent_manager_c* arg0, cXyz* arg1)
ext 02543F10 u32 Bj1_getEventIndex(dEvent_manager_c* arg0, u32 arg1, u32 arg2)
ext 0254407C BOOL Bj1_startCheck(dEvent_manager_c* arg0, s32 arg1)
ext 0254487C u32 Bj1_getSubstance(dEvent_manager_c* arg0, s32 arg1, const char* arg2, s32 arg3)
ext 0259E6D0 void Bj1_setPathInfo(dNpc_PathRun_c* arg0, u32 arg1, s32 arg2, u32 arg3)
ext 0259E730 void Bj1_setPathDirect(dNpc_PathRun_c* arg0, dPath* arg1)
ext 0259E744 u32 Bj1_nextPath(dNpc_PathRun_c* arg0, s32 arg1)
ext 0259E778 void Bj1_getPathPoint(dNpc_PathRun_c* arg0, cXyz* arg1, u32 arg2)
ext 0259EDB8 u32 Bj1_maxPoint(dNpc_PathRun_c* arg0)
ext 0259F740 void Bj1_initEventCut(dNpc_EventCut_c* arg0)
ext 0259F814 void Bj1_setEventActor(dNpc_EventCut_c* arg0, u32 arg1, daNpc_Bj1_c* arg2)
ext 025AB070 u32 Bj1_nextRoomPath(dPath* arg0, s32 arg1)
ext 025B7D90 u32 Bj1_isSymbol(dSv_player_collect_c* arg0, s32 arg1)
ext 025B8BB0 u32 Bj1_getEventReg(dSv_event_c* events, u32 reg)
ext 025B8AF4 void Bj1_setEventReg(dSv_event_c* events, u32 reg, u32 value)
ext 025B8B68 void Bj1_onEventBit(dSv_event_c* events, u32 bit)
ext 025B8B94 u32 Bj1_isEventBit(dSv_event_c* arg0, u32 arg1)
ext 025D54C4 void Bj1_searchById(u32 arg0, be<u32>* arg1)
ext 025D8E6C u32 Bj1_createItemForKP2(cXyz* arg0, u32 arg1, s32 arg2, csXyz* arg3, cXyz* arg4, s32 arg5, f32 arg6, f32 arg7, f32 arg8)
ext 025DE508 void Bj1_searchActors(void* arg0, daNpc_Bj1_c* arg1)
ext 025E1A40 void Bj1_startSound(u32 arg0, cXyz* arg1, u32 arg2, s32 arg3)
ext 0273AA24 void Bj1_assert(const char* arg0, u32 arg1, const char* arg2)
ext 0273AD10 u32 Bj1_allocate(u32 arg0)
ext 028249B0 void Bj1_setEmitterMatrix(Mtx34* arg0, Mtx34* arg1, cXyz* arg2)
ext 021F4914 void Bj1_setAttentionRegister(daNpc_Bj1_c* actor, s32 force)

ext 021F590C u32 Bj1_setLocFlagCarry(daNpc_Bj1_c* actor, int* value, u32 carry)
ext 021F594C u32 Bj1_setShapeAngleCarry(void* actor, int* value, u32 angle)
ext 021F597C void Bj1_setEventTimerCarry(void* actor, int* value)
ext 021F60A8 BOOL Bj1_deleteSeedTail(daNpc_Bj1_c* actor)

data l_jb_num 10465D78
data l_jb_actor 1046617C
data l_headScaleInitialized 104661CC
data l_headScale 10465D98
str 10016C1C "armL2"
str 10016C24 "armR2"
str 10016C2C "Bj"
str 10016C30 "head"
str 10016C38 "d_a_npc_bj1.cpp"
str 10016C48 "a_mdl_dat != (0)"
str 10016C5C "m_hed_jnt_num >= 0"
str 10016C70 "backbone"
str 10016C7C "m_bbone_jnt_num >= 0"
str 10016C94 "m_armL2_jnt_num >= 0"
str 10016CAC "m_armR2_jnt_num >= 0"
str 10016CC4 "stalk"
str 10016CD0 "center"
str 10016CFC "m_cen_jnt_num >= 0"
str 10016D10 "m_stalk_jnt_num >= 0"
str 10016D50 "plant_al"
str 10016D5C "m_plant_l_jnt_num >= 0"
str 10016D74 "plant_ar"
str 10016D80 "m_plant_r_jnt_num >= 0"
*/

// clang-format on
