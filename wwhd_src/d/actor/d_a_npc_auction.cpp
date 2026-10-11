/**
 * d_a_npc_auction.cpp (WWHD)
 * NPCs participating in the Windfall auction.
 * Ported from zeldaret/tww; GameCube names/order follow its nonmatching skeleton.
 * HD-specific layouts, ABI and copy ordering retain the verified reconstruction's behavior.
 */
#include "d/actor/d_a_npc_auction.h"

// HD: AutoFrame keeps the original entry frame and conditional Local allocation order.
// GHS matrices retain separate load/store loops, including their f32 quieting behavior.

using gabi::load;

struct PhaseTable {
    be<u32> mEntries[3];
};
static PhaseTable l_phaseTable;
static PhaseTable l_phaseTemplate;
struct StaticInitNode {
    be<u32> mWords[3];
};
static StaticInitNode l_staticInitNodes;
static StaticInitNode l_eyeInitNode;
static StaticInitNode l_lookInitNode;
struct ActionNames {
    be<u32> mEntries[2];
};
static ActionNames l_actionNames;
struct CylinderSource {
    u8 mData[0x68];
};
static CylinderSource l_cylinderSource;

struct arcNamesTable {
    be<u32> mEntries[12];
};
static arcNamesTable l_arcNames;
struct modelNumbersTable {
    be<u32> mEntries[12];
};
static modelNumbersTable l_modelNumbers;
struct extraModelsTable {
    be<s32> mEntries[12];
};
static extraModelsTable l_extraModels;
struct btpNumbersTable {
    be<u32> mEntries[12];
};
static btpNumbersTable l_btpNumbers;
struct eventNamesTable {
    be<u32> mEntries[8];
};
static eventNamesTable l_eventNames;
struct jointCallbacksTable {
    be<u32> mEntries[8];
};
static jointCallbacksTable l_jointCallbacks;
struct statusMasksTable {
    be<u32> mEntries[12];
};
static statusMasksTable l_statusMasks;
struct shadowTypesTable {
    be<u8> mEntries[12];
};
static shadowTypesTable l_shadowTypes;
struct AnimationNumbers {
    be<u32> mEntries[12][10];
};
struct CostumeRanges {
    be<u32> mEntries[8][2];
};
struct MessageNumbers {
    be<u32> mEntries[12][3];
};
static AnimationNumbers l_animationNumbers;
static CostumeRanges l_costumeRanges;
static MessageNumbers l_messageNumbers;
static u32 l_resControl;
static u32 l_save;
static u32 l_message;
static u32 l_calcMtx;
static u32 l_currentModel;
static u32 l_phaseInitialized;
static Mtx34 l_translationMtx;
static Mtx34 l_currentMtx;
static sAnmDat l_messageAnimation0;
static sAnmDat l_messageAnimation1;
static sAnmDat l_messageAnimation2;
static sAnmDat l_messageAnimation3;
static sAnmDat l_messageAnimation4;
static sAnmDat l_messageAnimation5;
static sAnmDat l_messageAnimation6;
static sAnmDat l_messageAnimation7;
static sAnmDat l_messageAnimation8;
static sAnmDat l_messageAnimation9;
static sAnmDat l_messageAnimation10;

// HD: these twelve costumes share the verified 0x34-byte HIO payload.
static AuctionHIOArray l_HIO;
static f32 l_zero;
static f32 l_ten;
static f32 l_defaultScale;
static f32 l_defaultOffset;
static f32 l_height4;
static f32 l_height5;
static f32 l_height6;
static f32 l_height7;
static f32 l_height8;
static f32 l_height9;
static f32 l_height10;
static f32 l_height11;
static f32 l_height12;
static f32 l_height13;
static f32 l_height14;
static f32 l_height15;
static f32 l_height16;
static f32 l_height17;
static f32 l_height18;
static f32 l_height19;
static f32 l_height20;
static u32 l_guard3;
static u32 l_guard2;
static u32 l_guard1;
static u32 l_guard0;
static f32 l_defaultSize;
static f32 l_defaultAngle;
static u8 l_eyeInit;
static u8 l_lookInit;

// HD: the two callbacks share this inlined matrix path.
namespace {
template <bool alternate> u32 auctionJointRotation(u32 jointObject, s32 phase) {
    if (phase != 0)
        return 1;
    J3DModel* model = gabi::at<J3DModel>((u32)l_currentModel);
    daNpcAuction_c* a = gabi::at<daNpcAuction_c>((u32)model->mUserArea);
    auction_cpp::Joint* jointData = gabi::at<auction_cpp::Joint>(auction_toJoint(jointObject));
    J3DMtxBuffer_hd* matrices = model->mpMtxBuffer.get();
    u16 joint = (u16)jointData->mJointNo;
    u16 flags = (u16)matrices->mFlags;
    Mtx34* base = gabi::at<Mtx34>((u32)matrices->mpAnmMtx);
    matrices->mFlags = flags | 0x10;
    auction_matrixCopy(gabi::ea(base) + joint * 0x30, (u32)l_calcMtx);
    if ((u32)joint == (u32)(s32)(s8)a->m_jnt.mHeadJntNum) {
        s16 angle = (s16)a->m_jnt.mAngles[0][1];
        (alternate ? auction_matrixXRotate : auction_matrixYRotate)((u32)l_calcMtx,
                                                                    alternate ? angle : (s16)-angle);
        angle = (s16)a->m_jnt.mAngles[0][0];
        auction_matrixZRotate((u32)l_calcMtx, (s16)-angle);
    }
    if ((u32)joint == (u32)(s32)(s8)a->m_jnt.mBackboneJntNum) {
        auction_matrixXRotate((u32)l_calcMtx, (s16)a->m_jnt.mAngles[1][1]);
        s16 angle = (s16)a->m_jnt.mAngles[1][0];
        auction_matrixZRotate((u32)l_calcMtx, alternate ? (s16)-angle : angle);
    }
    matrices = model->mpMtxBuffer.get();
    Mtx34* matrix = gabi::at<Mtx34>((u32)l_calcMtx);
    flags = (u16)matrices->mFlags;
    base = gabi::at<Mtx34>((u32)matrices->mpAnmMtx);
    matrices->mFlags = flags | 0x10;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)matrix->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        (base + joint)->m[i / 4][i % 4] = values[i];
    auction_matrixCopy((u32)l_calcMtx, gabi::ea(&l_currentMtx));
    return 1;
}
} // namespace

/* 021EB76C */
daNpcAuction_c* daNpcAuction_c_ct(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 self = gabi::ea(a);
    if (!self)
        self = auction_operatorNew(0x8B8);
    if (!self)
        return nullptr;
    a = gabi::at<daNpcAuction_c>(self);
    auction_fopNpcConstruct(self);
    a->__vtbl = AUCTION_VTABLE;
    auction_memclr(gabi::ea(&a->mPhs), 8);
    auction_cpp::bits(&a->mpExtraModel) = 0;
    auction_cpp::bits(&a->mpTexture) = 0;
    auction_btpConstruct(gabi::ea(&a->mBtp));
    a->mBlinkTimer = 0;
    auction_cpp::bits(&a->mpEmitter) = 0;
    a->mBtpNo = 0;
    auction_cpp::bits(&a->mpAnmTable) = 0;
    a->mPiconOfsY = 0.0f;
    a->mVoiceId = 0;
    a->mLookVel = 0;
    a->mMaxTalkDist = 0.0f;
    a->mUnused874 = 0;
    auction_memclr(gabi::ea(&a->mEventId), 2);
    a->mSelection = 0;
    a->mCanTalk = 1;
    auction_cpp::bits(&a->mpAnmTable) = 0;
    a->mVoiceTimer = 0;
    a->mItemChoice = 0;
    a->mActIdx = 0;
    a->mUnused874 = 0;
    a->mNpcNo = 0;
    a->mBlinkFrame = 0;
    a->mUnused8AE = 0;
    a->mEmitterEnabled = 0;
    auction_cpp::bits(&a->mpEmitter) = 0;
    a->mTalking = 0;
    a->mOrder = 0;
    a->mMdlNo = 0;
    a->mAnmRepeat = 0;
    a->mAnmNo = 0;
    a->mAttentive = 0;
    a->mGestureDone = 0;
    return a;
}

/* 021EB068 */
u32 daNpc_Auction_nodeCallBack1(u32 jointObject, s32 phase) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return auctionJointRotation<false>(jointObject, phase);
}

/* 021EB1C0 */
u32 daNpc_Auction_nodeCallBack2(u32 jointObject, s32 phase) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return auctionJointRotation<true>(jointObject, phase);
}

/* 021EB74C */
u32 CheckCreateHeap(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return a->createHeap();
}

/* 021EB8DC */
u32 phase_1(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 flags = (u32)a->actor_condition;
    if (!(flags & 8)) {
        if (a) {
            daNpcAuction_c_ct(a);
            flags = (u32)a->actor_condition;
        }
        a->actor_condition = flags | 8;
    }
    gabi::Local<be<u32>> managerOut;
    if (!auction_searchByName(AUCTION_MANAGER_PROCESS, gabi::ea(managerOut.get())))
        return 0;
    auction_cpp::Manager* manager = gabi::at<auction_cpp::Manager>((u32)*managerOut);
    if (!manager)
        return 0;
    u8 kind = (u8)manager->mNpcKinds[a->getPrmNpcNo()];
    if (kind >= 8) {
        auction_panic(gabi::guest_str("d_a_npc_auction.cpp"), 0x429,
                      gabi::guest_str("npcNo < daNpcAuction_c::Npc_No_Max"));
        return 5;
    }
    a->mNpcNo = kind;
    u32 actorId = a ? (u32)a->mBsPcId : 0xFFFFFFFF;
    be<u32>* ownerSlot = &manager->mNpcIds[kind];
    if ((u32)*ownerSlot != 0xFFFFFFFF)
        return 0;
    *ownerSlot = actorId;
    u32 low = (u32)l_costumeRanges.mEntries[kind][0];
    u32 high = (u32)l_costumeRanges.mEntries[kind][1];
    u8 costume;
    if (kind >= 2 && kind <= 5) {
        u32 pairIndex = kind <= 3 ? 0 : 1;
        u8 previous = (u8)gabi::at<auction_cpp::Manager>((u32)*managerOut)->mPairedCostumes[pairIndex];
        costume = (u8)((u32)a->getRand((s32)(high - low + 1)) + low);
        manager = gabi::at<auction_cpp::Manager>((u32)*managerOut);
        if (previous == 255) {
            manager->mPairedCostumes[pairIndex] = costume;
            manager = gabi::at<auction_cpp::Manager>((u32)*managerOut);
        } else if (costume == (u8)manager->mPairedCostumes[pairIndex]) {
            costume = (u8)(costume == (u8)high ? costume - 1 : costume + 1);
        }
    } else {
        costume = (u8)((u32)a->getRand((s32)(high - low + 1)) + low);
        manager = gabi::at<auction_cpp::Manager>((u32)*managerOut);
    }
    a->mMdlNo = costume;
    manager->mCostumes[kind] = costume;
    a->mBtpNo = (u32)l_btpNumbers.mEntries[costume];
    return 2;
}

/* 021EBF80 */
u32 phase_2(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 self = gabi::ea(a);
    u8 costume = (u8)a->mMdlNo;
    u32 result = auction_loadArchive(gabi::ea(&a->mPhs), (u32)l_arcNames.mEntries[costume]);
    if (result == 4) {
        if (auction_entrySolidHeap(self, gabi::guest_fn((const void*)&CheckCreateHeap), 0x10000))
            return a->createInit();
        auction_cpp::bits(&a->mpMorf) = 0;
        return 5;
    }
    return result;
}

/* 021EB450 */
u32 daNpcAuction_c::createHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 self = gabi::ea(this);
    gabi::Local<SafeString> modelName, animationName, extraName;
    u8 costume = (u8)mMdlNo;
    u32 resource = (u32)l_resControl;
    u32 modelNumber = (u32)l_modelNumbers.mEntries[costume];
    modelName->mStringTop = (u32)l_arcNames.mEntries[costume];
    modelName->__vtbl = SAFE_STRING_VTABLE;
    auction_cpp::JointData* modelData = gabi::at<auction_cpp::JointData>(
        auction_getResource(resource, gabi::ea(modelName.get()), modelNumber));
    costume = (u8)mMdlNo;
    u8 animationIndex = (u8)mAnmNo;
    resource = (u32)l_resControl;
    u32 animationNumber = (u32)l_animationNumbers.mEntries[costume][animationIndex];
    animationName->mStringTop = (u32)l_arcNames.mEntries[costume];
    animationName->__vtbl = SAFE_STRING_VTABLE;
    u32 animation = auction_getResource(resource, gabi::ea(animationName.get()), animationNumber);
    mDoExt_McaMorf* morf = gabi::at<mDoExt_McaMorf>(auction_createMorf(
        0, gabi::ea(modelData), 0, 0, animation, 2, 0, -1, 1, 0, 0x80000, 0x11020203, 1.0f));
    auction_cpp::bits(&mpMorf) = gabi::ea(morf);
    if (!morf || !auction_cpp::bits(&morf->mpModel))
        return 0;
    costume = (u8)mMdlNo;
    s32 extraModel = (s32)l_extraModels.mEntries[costume];
    if (extraModel >= 0) {
        extraName->mStringTop = (u32)l_arcNames.mEntries[costume];
        extraName->__vtbl = SAFE_STRING_VTABLE;
        resource = (u32)l_resControl;
        u32 extraData = auction_getResource(resource, gabi::ea(extraName.get()), extraModel);
        J3DModel* extra = gabi::at<J3DModel>(auction_createModel(extraData, 0x80000, 0x15020022));
        auction_cpp::bits(&mpExtraModel) = gabi::ea(extra);
        if (!extra)
            return 0;
    }
    auction_cpp::JointName* names = gabi::at<auction_cpp::JointName>(auction_jointNames(gabi::ea(modelData)));
    u32 delta = (u32)names->mNameOffset;
    u32 strings = delta ? gabi::ea(names) + 0x10 + delta : 0;
    s8 head = (s8)auction_findJointName(strings, gabi::guest_str("head"));
    m_jnt.mHeadJntNum = head;
    if (head < 0)
        auction_panic(gabi::guest_str("d_a_npc_auction.cpp"), 0x4A9,
                      gabi::guest_str("m_jnt.getHeadJntNum() >= 0"));
    names = gabi::at<auction_cpp::JointName>(auction_jointNames(gabi::ea(modelData)));
    delta = (u32)names->mNameOffset;
    strings = delta ? gabi::ea(names) + 0x10 + delta : 0;
    s8 backbone = (s8)auction_findJointName(strings, gabi::guest_str("backbone"));
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0)
        auction_panic(gabi::guest_str("d_a_npc_auction.cpp"), 0x4AD,
                      gabi::guest_str("m_jnt.getBackboneJntNum() >= 0"));
    if (!initTexPatternAnm(0))
        return 0;
    auction_cpp::JointTree* tree = gabi::at<auction_cpp::JointTree>(auction_jointTree(gabi::ea(modelData)));
    u16 count = (u16)tree->mJointCount;
    for (u16 joint = 0; joint < count; ++joint) {
        s8 headIndex = (s8)m_jnt.mHeadJntNum;
        if ((u32)joint == (u32)(s32)headIndex || (u32)joint == (u32)(s32)(s8)m_jnt.mBackboneJntNum) {
            u32 extent = (u32)modelData->mExtent;
            auction_cpp::Joint* entry = gabi::at<auction_cpp::Joint>((u32)modelData->mEntries);
            if (joint < extent)
                entry = gabi::at<auction_cpp::Joint>(gabi::ea(entry) + (u32)joint * 0x1C);
            u8 npc = (u8)mNpcNo;
            entry->mCallback = l_jointCallbacks.mEntries[npc];
        }
        tree = gabi::at<auction_cpp::JointTree>(auction_jointTree(gabi::ea(modelData)));
        count = (u16)tree->mJointCount;
    }
    morf = mpMorf.get();
    J3DModel* model = morf->mpModel.get();
    model->mUserArea = self;
    auction_setWall(gabi::ea(&mAcchCir), 30.0f, 30.0f);
    auction_acchSet(gabi::ea(&mObjAcch), gabi::ea(&current.pos), gabi::ea(&old.pos), self, 1,
                    gabi::ea(&mAcchCir), gabi::ea(&speed), gabi::ea(&current.angle), gabi::ea(&shape_angle));
    return 1;
}

/* 021EBBB8 */
u32 daNpcAuction_XyCheckCB(daNpcAuction_c* a, s32 event) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return a->XyCheckCB(event);
}

/* 021EBC78 */
s32 daNpcAuction_XyEventCB(daNpcAuction_c* a, s32 event) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return a->XyEventCB(event);
}

/* 021EBD5C */
u32 daNpcAuction_c::createInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 self = gabi::ea(this);
    u8 npc = (u8)mNpcNo;
    gravity = -9.0f;
    auction_eventCutSet(gabi::ea(&mEventCut), (u32)l_eventNames.mEntries[npc], self);
    mDoExt_McaMorf* morf = mpMorf.get();
    mTalking = 0;
    mLookVel = 0;
    mAttentive = 0;
    J3DModel* model = morf->mpModel.get();
    cullMtx = model ? gabi::ea(model) + 0xC8 : 0;
    wait_action_init();
    u8 attention = (u8)mNpcNo == 0 ? 0xAD : 0xA9;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = attention;
    attention_info.flags = 10;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = attention;
    auction_cpp::Play* play = auction_getPlay();
    s32 event =
        auction_getEventIndex(gabi::ea(&play->mEventManager), gabi::guest_str("AUCTION_TALK_XY"), 255);
    mEventId = (s16)event;
    auction_setArchiveName(self + 0xF8, gabi::guest_str("AUCTION_TALK"));
    u8 costume = (u8)mMdlNo;
    eventInfo.mpEventCB = gabi::guest_fn((const void*)&daNpcAuction_XyEventCB);
    eventInfo.mpCheckCB = gabi::guest_fn((const void*)&daNpcAuction_XyCheckCB);
    mMaxTalkDist = (f32)l_HIO.mCostumes[costume].mTalkDistance;
    setMtx();
    model = mpMorf.get()->mpModel.get();
    auction_modelCalc(gabi::ea(model));
    auction_statusInit(gabi::ea(&mStts), 255, 255, self);
    auction_cylinderSet(gabi::ea(&mCyl), gabi::ea(&l_cylinderSource));
    auction_cpp::bits(&mCyl.mStts) = gabi::ea(&mStts);
    auction_setAttention(self, 60.0f, 150.0f);
    return 4;
}

/* 021EBB90 */
void daNpcAuction_c::wait_action_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mActionIndex = -1;
    mActionFunction = WAIT_ACTION;
    mActionDelta = 0;
}

/* 021ED81C */
void daNpcAuction_c::wait_action() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mOrder = (u8)mAttentive != 0;
    if ((u8)mTalking == 1) {
        if (auction_talk(gabi::ea(this), 1) == 0x12) {
            mTalking = 0;
            setAnm2(7, 2, 8.0f);
            if (!(u8)mSelection) {
                auction_cpp::Play* play = auction_getPlay();
                play->mEventFlags = (u16)play->mEventFlags | 8;
            } else if (!(u8)mNpcNo) {
                gabi::Local<be<u16>> process;
                *process = AUCTION_MANAGER_PROCESS;
                auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
                if (manager) {
                    u8 selection = (u8)mSelection;
                    if (selection == 1)
                        manager->mSelection = 1;
                    else if (selection == 2) {
                        manager->mSelection = 2;
                        manager->mItemChoice = (u8)mItemChoice;
                    }
                }
            }
        }
        setAnmFromMsgTag();
    }
    if (!(u8)mAnmNo)
        setAnm2(7, 2, 8.0f);
}

/* 021EC1A8 */
void daNpcAuction_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 state = (u16)eventInfo.mCommand;
    if (state == AUCTION_EVENT_DEMO) {
        mOrder = 0;
    } else if (state == AUCTION_EVENT_TALK) {
        u8 order = (u8)mOrder;
        if ((order == 1 || order == 2) && !(u8)mTalking && !(u8)mSelection)
            mTalking = 1;
    }
}

/* 021ECD6C */
void daNpcAuction_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 order = (u8)mOrder;
    if (order != 1 && order != 2)
        return;
    eventInfo.mCondition = (u16)eventInfo.mCondition | 1;
    u32 save = (u32)l_save;
    u32 event = auction_isEventBit(save + 0x644, 0x1404);
    order = (u8)mOrder;
    if (event)
        eventInfo.mCondition = (u16)eventInfo.mCondition | 0x20;
    if (order == 2)
        auction_orderSpeak(gabi::ea(this));
}

/* 021ECB68 */
void daNpcAuction_c::eventMove() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto ended = [](u32 name) {
        auction_cpp::Play* play = auction_getPlay();
        return auction_eventEndCheck(gabi::ea(&play->mEventManager), name) != 0;
    };
    if (ended(gabi::guest_str("AUCTION_GET_ITEM")) || ended(gabi::guest_str("AUCTION_NO_ITEM")) ||
        ended(gabi::guest_str("AUCTION_END2"))) {
        mMaxTalkDist = 200.0f;
        return;
    }
    if (ended(gabi::guest_str("AUCTION_TALK_XY"))) {
        if (!(u8)mSelection) {
            auction_cpp::Play* play = auction_getPlay();
            play->mEventFlags = (u16)play->mEventFlags | 8;
            mTalking = 0;
            mCanTalk = 1;
            return;
        }
        if (!(u8)mNpcNo) {
            gabi::Local<be<u16>> process;
            *process = AUCTION_MANAGER_PROCESS;
            auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
            if (manager) {
                u8 selection = (u8)mSelection;
                if (selection == 1) {
                    manager->mSelection = 1;
                    mCanTalk = 1;
                    mTalking = 0;
                    return;
                }
                if (selection == 2) {
                    manager->mSelection = 2;
                    manager->mItemChoice = (u8)mItemChoice;
                }
            }
        }
        mCanTalk = 1;
        mTalking = 0;
        return;
    }
    u8 previousCut = (u8)mEventCut.mbAttention;
    if (auction_eventCutMove(gabi::ea(&mEventCut))) {
        if (!(u8)mEventCut.mbAttention)
            mEventCut.mbAttention = previousCut;
    } else {
        privateCut();
        if ((u8)mNpcNo == 7)
            setAnmFromMsgTag();
    }
}

/* 021EC724 */
void daNpcAuction_c::privateCut() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 name = (u32)l_eventNames.mEntries[(u8)mNpcNo];
    auction_cpp::Play* play = auction_getPlay();
    s32 staff = auction_getStaffId(gabi::ea(&play->mEventManager), name, 0, 0);
    if (staff == -1)
        return;
    play = auction_getPlay();
    s8 action =
        (s8)auction_getActionIndex(gabi::ea(&play->mEventManager), staff, gabi::ea(&l_actionNames), 2, 1, 0);
    mActIdx = action;
    play = auction_getPlay();
    if (action == -1) {
        auction_cutEnd(gabi::ea(&play->mEventManager), staff);
        return;
    }
    if (auction_isAdvance(gabi::ea(&play->mEventManager), staff)) {
        action = (s8)mActIdx;
        if (action == 0)
            eventMainInit();
        else if (action == 1)
            eventMesSetInit(staff);
        else {
            play = auction_getPlay();
            auction_cutEnd(gabi::ea(&play->mEventManager), staff);
            return;
        }
    }
    action = (s8)mActIdx;
    if (action == 0) {
        if (!eventMain())
            return;
    } else if (action == 1) {
        if (!eventMesSet())
            return;
    }
    play = auction_getPlay();
    auction_cutEnd(gabi::ea(&play->mEventManager), staff);
}

/* 021EC200 */
void daNpcAuction_c::eventMainInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mEmitterEnabled = 1;
}

/* 021EC400 */
u32 daNpcAuction_c::eventMain() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<be<u16>> process;
    *process = AUCTION_MANAGER_PROCESS;
    auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
    if (!manager)
        return 0;
    u8 flags = (u8)manager->mGestureFlags;
    bool particle = false;
    auto animate = [&](s32 index, u8 bit) {
        setAnm2(index, 2, 8.0f);
        mGestureDone = (u8)mGestureDone | bit;
    };
    if (flags & 4) {
        if (!((u8)mGestureDone & 8)) {
            animate(3, 8);
            flags = (u8)manager->mGestureFlags;
            particle = (flags & 4) != 0;
        } else
            particle = true;
    } else if ((u8)manager->mSpeakingNpc == (u8)mNpcNo) {
        if (flags & 1) {
            if ((u8)mGestureDone & 4) {
                clrEmitter();
                return 0;
            }
            animate(2, 4);
        } else if (flags & 8) {
            if ((u8)mGestureDone & 0x10) {
                clrEmitter();
                return 0;
            }
            animate(4, 0x10);
        } else if (flags & 0x20) {
            if ((u8)mGestureDone & 0x40) {
                clrEmitter();
                return 0;
            }
            animate(6, 0x40);
        } else if (flags & 0x10) {
            if (!((u8)mGestureDone & 0x20))
                animate(5, 0x20);
            mEmitterEnabled = 0;
        } else {
            u8 animation = (u8)mAnmNo;
            mGestureDone = 0;
            if (animation)
                setAnm2(0, 2, 8.0f);
        }
        flags = (u8)manager->mGestureFlags;
        particle = (flags & 4) != 0;
    } else {
        u8 animation = (u8)mAnmNo;
        mGestureDone = 0;
        if (animation)
            setAnm2(0, 2, 8.0f);
        flags = (u8)manager->mGestureFlags;
        particle = (flags & 4) != 0;
    }
    if (!particle) {
        clrEmitter();
        return 0;
    }
    if (!(u32)auction_cpp::bits(&mpEmitter)) {
        s32 room = (s8)current.roomNo;
        auction_cpp::Play* play = auction_getPlay();
        auction_cpp::Emitter* emitter =
            auction_particleSet((u32)play->mParticle, 0, 0x819D, gabi::ea(&current.pos), 0, 0, 255, 0, room,
                                gabi::ea(&tevStr.mColorK0), gabi::ea(&tevStr.mColorK0), 0);
        mpEmitter = emitter;
        if (!emitter)
            return 0;
        emitter->mStatus = (u32)emitter->mStatus | 0x40;
        if (!(u32)auction_cpp::bits(&mpEmitter))
            return 0;
    }
    {
        J3DModel* model = mpMorf.get()->mpModel.get();
        s32 joint = (s8)m_jnt.mHeadJntNum;
        J3DMtxBuffer_hd* matrices = model->mpMtxBuffer.get();
        u16 flags = (u16)matrices->mFlags;
        Mtx34* base = gabi::at<Mtx34>((u32)matrices->mpAnmMtx);
        matrices->mFlags = flags | 0x10;
        auction_matrixCopy(gabi::ea(base) + (u32)joint * 0x30, (u32)l_calcMtx);
        auction_matrixZRotate((u32)l_calcMtx, -0x4000);
        u8 costume = (u8)mMdlNo;
        auction_matrixTranslate(gabi::ea(&l_translationMtx), 0.0f,
                                (f32)l_HIO.mCostumes[costume].mParticleHeight, 0.0f);
        Mtx34* matrix = gabi::at<Mtx34>((u32)l_calcMtx);
        auction_matrixConcat(gabi::ea(matrix), gabi::ea(&l_translationMtx), gabi::ea(matrix));
        auction_cpp::Emitter* emitter = mpEmitter;
        auction_emitterSetGlobalMatrix((u32)l_calcMtx, gabi::ea(&emitter->mGlobalRotation),
                                       gabi::ea(&emitter->mGlobalTranslation));
        return 0;
    }
}

/* 021EC20C */
void daNpcAuction_c::eventMesSetInit(s32 staff) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auction_cpp::Play* play = auction_getPlay();
    u32 substance = auction_substance(gabi::ea(&play->mEventManager), staff, gabi::guest_str("MsgNo"), 3);
    u32 message = substance ? (u32)*gabi::at<be<u32>>(substance)
                            : gabi::call_ptr<u32>(load<u32>((u32)__vtbl + 0x1C), this);
    mCurrMsgNo = message;
}

/* 021EC6F4 */
u32 daNpcAuction_c::eventMesSet() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return auction_talk(gabi::ea(this), 0) == 0x12;
}

/* 021EBBB0 */
u32 daNpcAuction_c::XyCheckCB(s32 event) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 021EBBBC */
s32 daNpcAuction_c::XyEventCB(s32 event) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auction_cpp::Play* play = auction_getPlay();
    s32 index =
        auction_getEventIndex(gabi::ea(&play->mEventManager), gabi::guest_str("DEFAULT_TALK_XY"), 255);
    play = auction_getPlay();
    u8 item = (u8)play->mXYItems[(u32)event];
    bool missing = false;
    if (item == AUCTION_ITEM_TRIFORCE_CHART || item == AUCTION_ITEM_TREASURE_CHART) {
        u32 save = (u32)l_save;
        missing = !auction_isEventBit(save + 0x644, item == AUCTION_ITEM_TRIFORCE_CHART ? 0x1008 : 0x1004);
    }
    if (missing) {
        index = (s16)mEventId;
        mCanTalk = 0;
    }
    return index;
}

/* 021ED964 */
u32 daNpcAuction_c::next_msgStatus(u32 message) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 player = (u32)l_message;
    gabi::Local<be<u16>> process;
    *process = AUCTION_MANAGER_PROCESS;
    auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
    u32 current = (u32)*gabi::at<be<u32>>(message);
    u32 next = 0;
    u32 status = 15;
    switch (current) {
    case 0x27DA:
        next = 0x27DC;
        break;
    case 0x27DC:
        next = 0x27DD;
        break;
    case 0x27DD:
        next = 0x27DE;
        break;
    case 0x27DB:
    case 0x27DE:
        next = 0x27DF;
        break;
    case 0x27DF:
        if (!(u32)gabi::at<auction_cpp::Message>(player)->mChoice) {
            *gabi::at<be<u32>>(message) = 0x27E1;
            auction_onEventBit((u32)l_save + 0x644, 0x1408);
            return status;
        }
        next = 0x27E0;
        break;
    case 0x27E1:
        next = 0x27E2;
        break;
    case 0x27E2:
        next = (u32)gabi::at<auction_cpp::Message>(player)->mChoice ? 0x27E4 : 0x27E3;
        break;
    case 0x27E5:
        next = 0x27E6;
        break;
    case 0x27E6:
        next = 0x27E7;
        break;
    case 0x27E7:
        next = (u32)gabi::at<auction_cpp::Message>(player)->mChoice ? 0x27E8 : 0x27E9;
        break;
    case 0x27E3:
        next = 0x27E9;
        break;
    case 0x27E4:
    case 0x27E8:
        next = 0x27E5;
        break;
    case 0x27E9:
        mSelection = 1;
        status = 16;
        break;
    case 0x27EC:
        if (!(u32)gabi::at<auction_cpp::Message>(player)->mChoice) {
            *gabi::at<be<u32>>(message) = 0x27EE;
            auction_onAuctionWin((u32)l_save + 0x96);
            return status;
        }
        next = 0x27ED;
        break;
    case 0x27EE:
        next = 0x27EF;
        break;
    case 0x27EF:
        mSelection = 2;
        status = 16;
        break;
    case 0x1D35:
        next = 0x1D36;
        break;
    case 0x1D36:
        next = 0x1D37;
        break;
    case 0x1D37:
        next = manager && (u8)manager->mResult ? 0x1D39 : 0x1D38;
        break;
    default:
        status = 16;
        break;
    }
    if (next)
        *gabi::at<be<u32>>(message) = next;
    return status;
}

/* 021EDBF8 */
u32 daNpcAuction_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 kind = (u8)mNpcNo;
    auto eventBit = [](u32 flag) { return auction_isEventBit((u32)l_save + 0x644, flag) != 0; };
    if (!kind) {
        auction_cpp::Play* play = auction_getPlay();
        u32 selection = (u8)play->mTalkXY;
        if (selection - 1 <= 3) {
            play = auction_getPlay();
            u8 item = (u8)play->mPreItem;
            if (item == AUCTION_ITEM_TRIFORCE_CHART || item == AUCTION_ITEM_TREASURE_CHART) {
                if (eventBit(item == AUCTION_ITEM_TRIFORCE_CHART ? 0x1008 : 0x1004))
                    return 0x27F7;
                mItemChoice = item == AUCTION_ITEM_TREASURE_CHART;
                return 0x27EB;
            }
            return 0x27EA;
        }
        if (eventBit(0xA02) && !auction_isTriforce(0x69))
            return 0x27F8;
        if (eventBit(0x1002))
            return auction_isSecondQuest() ? 0x27DB : 0x27F0;
        auction_onEventBit((u32)l_save + 0x644, 0x1002);
        return 0x27DA;
    }
    if (kind == 6)
        return 0x1D35;
    gabi::Local<be<u16>> process;
    *process = AUCTION_MANAGER_PROCESS;
    auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
    if (!manager)
        auction_panic(gabi::guest_str("d_a_npc_auction.cpp"), 0x80A, gabi::guest_str("actAuc != (0)"));
    u8 costume = (u8)mMdlNo;
    u8 current = (u8)manager->mCurrentNpc;
    be<u32>* table = l_messageNumbers.mEntries[costume];
    if (!current)
        return (u32)table[0];
    return (u32)table[current == (u8)mNpcNo ? 1 : 2];
}

/* 021EC93C */
void daNpcAuction_c::setAnmFromMsgTag() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auction_cpp::Play* play = auction_getPlay();
    u8 kind = (u8)mNpcNo;
    u8 tag = (u8)play->mMsgTag;
    u32 table = 0;
    if (tag <= 7) {
        if (kind == 0 || kind == 7) {
            const u32 tables[8] = {gabi::ea(&l_messageAnimation2),  gabi::ea(&l_messageAnimation2),
                                   gabi::ea(&l_messageAnimation3),  gabi::ea(&l_messageAnimation4),
                                   gabi::ea(&l_messageAnimation0),  gabi::ea(&l_messageAnimation5),
                                   gabi::ea(&l_messageAnimation10), gabi::ea(&l_messageAnimation6)};
            table = tables[tag];
        } else {
            const u32 tables[8] = {gabi::ea(&l_messageAnimation2), gabi::ea(&l_messageAnimation7),
                                   gabi::ea(&l_messageAnimation8), gabi::ea(&l_messageAnimation9),
                                   gabi::ea(&l_messageAnimation1), gabi::ea(&l_messageAnimation9),
                                   gabi::ea(&l_messageAnimation1), gabi::ea(&l_messageAnimation1)};
            table = tables[tag];
        }
        setAnmTbl(table);
        if ((kind == 0 || kind == 7) && tag == 6) {
            mVoiceId = 0x8DD;
            mVoiceTimer = 100;
        }
    }
    play = auction_getPlay();
    play->mMsgTag = 255;
}

/* 021EB750 */
u8 daNpcAuction_c::getPrmNpcNo() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 index = (s8)argument;
    return (u32)(s32)index < 8 ? (u8)index : 0;
}

/* 021EBC7C */
void daNpcAuction_c::setMtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* model = mpMorf.get()->mpModel.get();
    f32 z = (f32)scale.z, x = (f32)scale.x, y = (f32)scale.y;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = (f32)current.pos.x;
    y = (f32)current.pos.y;
    z = (f32)current.pos.z;
    auction_matrixTranslate(gabi::ea(&l_translationMtx), x, y, z);
    auction_matrixYRotate(gabi::ea(&l_translationMtx), (s16)current.angle.y);
    model = mpMorf.get()->mpModel.get();
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = (f32)l_translationMtx.m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}

/* 021ECF4C */
void daNpcAuction_c::lookBack() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 costume = (u8)mMdlNo;
    AuctionHIO* parameters = &l_HIO.mCostumes[costume];
    u8 emitterEnabled = (u8)mEmitterEnabled;
    s16 facing = (s16)current.angle.y;
    f32 eyeHeight = (f32)parameters->mEyeHeight;
    s16 turn = (s16)parameters->mTurnSpeed;
    u32 track = 1;
    gabi::Local<cXyz> target;
    u32 targetAddress = 0;
    if (emitterEnabled) {
        gabi::Local<be<u16>> process;
        *process = AUCTION_MANAGER_PROCESS;
        auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
        if (manager) {
            f32 ownX = (f32)eyePos.x;
            f32 x = (f32)manager->eyePos.x;
            f32 y = (f32)manager->eyePos.y;
            f32 z = (f32)manager->eyePos.z;
            if (ownX != x || (f32)eyePos.y != y || (f32)eyePos.z != z) {
                target->set(x, y, z);
                targetAddress = gabi::ea(target.get());
            }
        }
    } else if ((u8)mEventCut.mbAttention) {
        f32 x = (f32)mEventCut.mPos.x, y = (f32)mEventCut.mPos.y;
        m_jnt.mbTrn = 1;
        f32 z = (f32)mEventCut.mPos.z;
        target->set(x, y, z);
        targetAddress = gabi::ea(target.get());
        track = 0;
        if (!(u8)mAttentive)
            mAttentive = 1;
    } else {
        auction_cpp::Play* play = auction_getPlay();
        u32 player = (u32)play->mPlayer;
        gabi::Local<cXyz> actorPosition;
        actorPosition->set((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
        costume = (u8)mMdlNo;
        gabi::Local<cXyz> playerPosition;
        f32 px = (f32)gabi::at<fopAc_ac_c>(player)->current.pos.x,
            py = (f32)gabi::at<fopAc_ac_c>(player)->current.pos.y;
        f32 distanceLimit = (f32)mMaxTalkDist;
        parameters = &l_HIO.mCostumes[costume];
        f32 pz = (f32)gabi::at<fopAc_ac_c>(player)->current.pos.z;
        playerPosition->set(px, py, pz);
        s32 angleLimit = (s16)parameters->mAngleLimit;
        s32 frontalLimit = 0x4000;
        gabi::Local<be<f32>> distance;
        gabi::Local<be<s16>> angle;
        auction_distanceAngle(gabi::ea(actorPosition.get()), gabi::ea(playerPosition.get()),
                              gabi::ea(distance.get()), gabi::ea(angle.get()));
        u8 attentive = (u8)mAttentive;
        s16 currentFacing = (s16)current.angle.y;
        s16 relative = (s16)((s32)(s16)*angle - currentFacing);
        f32 actualDistance = *distance;
        if (attentive) {
            distanceLimit += 40.0f;
            angleLimit += 0x71C;
            frontalLimit = 0x471C;
        }
        *angle = relative;
        // PPC ble branches whenever the greater-than bit is clear, including
        // unordered.
        bool far = !(distanceLimit > actualDistance);
        if (!far) {
            s32 absolute = relative < 0 ? -(s32)relative : relative;
            if (frontalLimit > absolute) {
                gabi::Local<cXyz> eye;
                auction_playerEyePos(gabi::ea(eye.get()), eyeHeight);
                target->copy(*eye);
                targetAddress = gabi::ea(target.get());
                track = 1;
                if (!(u8)mAttentive)
                    mAttentive = 1;
            } else if (angleLimit > absolute) {
                u8 talking = (u8)mTalking;
                if (talking) {
                    gabi::Local<cXyz> eye;
                    auction_playerEyePos(gabi::ea(eye.get()), eyeHeight);
                    target->copy(*eye);
                    targetAddress = gabi::ea(target.get());
                    m_jnt.mbTrn = 1;
                    track = 0;
                    attentive = (u8)mAttentive;
                }
                if (!attentive)
                    mAttentive = 1;
            } else
                far = true;
        }
        if (far) {
            u8 kind = (u8)mNpcNo;
            mAttentive = 0;
            if (!kind) {
                facing = (s16)home.angle.y;
                track = 0;
                m_jnt.mbTrn = 1;
            }
        }
    }
    if ((u8)mTalking) {
        track = 0;
        m_jnt.mbTrn = 1;
    }
    s16 smoothing = 0;
    if ((u8)m_jnt.mbTrn) {
        s16 overrideTurn = (s16)mEventCut.mTurnSpeed;
        if (overrideTurn)
            turn = overrideTurn;
        auction_approachAngle(gabi::ea(&mLookVel), turn, 4, 0x800);
        smoothing = (s16)mLookVel;
    } else
        mLookVel = 0;
    gabi::Local<cXyz> origin;
    origin->set((f32)eyePos.x, (f32)eyePos.y, (f32)eyePos.z);
    auction_lookAtTarget(gabi::ea(&m_jnt), gabi::ea(&current.angle.y), targetAddress, gabi::ea(origin.get()),
                         facing, smoothing, track);
    u16 x = (u16)current.angle.x, y = (u16)current.angle.y, z = (u16)current.angle.z;
    shape_angle.x = x;
    shape_angle.y = y;
    shape_angle.z = z;
}

/* 021EB318 */
u32 daNpcAuction_c::initTexPatternAnm(u32 restart) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* model = gabi::at<J3DModel>((u32)auction_cpp::bits(&mpExtraModel));
    if (!model)
        model = mpMorf.get()->mpModel.get();
    u32 material = (u32)auction_cpp::bits(&model->mModelData);
    u8 costume = (u8)mMdlNo;
    u32 animation = (u32)mBtpNo;
    u32 name = (u32)l_arcNames.mEntries[costume];
    gabi::Local<SafeString> resource;
    resource->mStringTop = name;
    resource->__vtbl = SAFE_STRING_VTABLE;
    u32 resources = (u32)l_resControl;
    u32 texture = auction_getResource(resources, gabi::ea(resource.get()), animation);
    auction_cpp::bits(&mpTexture) = texture;
    if (!texture) {
        auction_panic(gabi::guest_str("d_a_npc_auction.cpp"), 0x902,
                      gabi::guest_str("m_head_tex_pattern != (0)"));
        texture = (u32)auction_cpp::bits(&mpTexture);
    }
    u32 ok = auction_btpInit(gabi::ea(&mBtp), material, texture, 1, 2, 0, -1, restart, 0, 1.0f);
    if (!ok)
        return 0;
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return 1;
}

/* 021ECDF0 */
void daNpcAuction_c::playTexPatternAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (auction_decrementTimer(gabi::ea(&mBlinkTimer)))
        return;
    u32 texture = (u32)auction_cpp::bits(&mpTexture);
    s32 frames = gabi::call_ptr<s32>(load<u32>(load<u32>(texture + 4) + 0x14), texture);
    if ((s32)(u8)mBlinkFrame >= frames) {
        texture = (u32)auction_cpp::bits(&mpTexture);
        frames = gabi::call_ptr<s32>(load<u32>(load<u32>(texture + 4) + 0x14), texture);
        mBlinkFrame = (u8)((u32)(u8)mBlinkFrame - (u32)frames);
        mBlinkTimer = (u16)((u32)getRand(60) + 60);
    } else {
        mBlinkFrame = (u8)((u8)mBlinkFrame + 1);
    }
}

/* 021ECE9C */
void daNpcAuction_c::playAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!auction_morfPlay((u32)auction_cpp::bits(&mpMorf), 0, 0, 0))
        return;
    if (!(u32)auction_cpp::bits(&mpAnmTable))
        return;
    s8 repeats = (s8)mAnmRepeat;
    if (repeats <= 0)
        return;
    repeats = (s8)(repeats - 1);
    mAnmRepeat = repeats;
    u32 table = (u32)auction_cpp::bits(&mpAnmTable);
    if (!repeats) {
        table += 3;
        auction_cpp::bits(&mpAnmTable) = table;
        setAnmTbl(table);
    } else {
        setAnm((u8)gabi::at<sAnmDat>(table)->mAnmNo, 0, 0.0f);
    }
}

/* 021EC288 */
void daNpcAuction_c::setAnm(s32 animation, s32 mode, f32 blend) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 costume = (u8)mMdlNo;
    u32 resources = (u32)l_resControl;
    u32 archive = (u32)l_arcNames.mEntries[costume];
    u32 resourceId = (u32)l_animationNumbers.mEntries[costume][(u32)animation];
    gabi::Local<SafeString> name;
    name->mStringTop = archive;
    name->__vtbl = SAFE_STRING_VTABLE;
    u32 resource = auction_getResource(resources, gabi::ea(name.get()), resourceId);
    auction_morfSetAnimation((u32)auction_cpp::bits(&mpMorf), resource, mode, 0, blend, 1.0f, 0.0f, -1.0f);
    mAnmNo = (u8)animation;
}

/* 021EC360 */
void daNpcAuction_c::setAnm2(s32 animation, s32 mode, f32 blend) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((u32)(u8)mAnmNo != (u32)animation || mode == 0) {
        setAnm(animation, mode, blend);
        auction_cpp::bits(&mpAnmTable) = 0;
    }
}

/* 021EC878 */
void daNpcAuction_c::setAnmTbl(u32 table) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 animation = (u8)gabi::at<sAnmDat>(table)->mAnmNo;
    if (animation == 255) {
        auction_cpp::bits(&mpAnmTable) = 0;
        return;
    }
    auction_cpp::bits(&mpAnmTable) = table;
    s8 repeats = (s8)gabi::at<sAnmDat>(table)->mRepeat;
    mAnmRepeat = repeats;
    animation = (u8)gabi::at<sAnmDat>(table)->mAnmNo;
    if (repeats > 0 || (u8)mAnmNo != animation)
        setAnm(animation, repeats > 0 ? 0 : 2, (f32)(u8)gabi::at<sAnmDat>(table)->mMorf);
}

/* 021EC0E8 */
u32 daNpcAuction_c::isExecute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 costume = (u8)mMdlNo;
    u32 status = (u32)actor_status;
    u32 mask = (u32)l_statusMasks.mEntries[costume];
    gabi::Local<be<u16>> process;
    *process = (u16)0x17E;
    actor_status = status & ~mask;
    auction_cpp::Manager* manager = auction_judge(AUCTION_MANAGER_JUDGE, gabi::ea(process.get()));
    if (!manager)
        return 0;
    u32 shift = (u8)mNpcNo & 63;
    u8 active = (u8)manager->mActiveNpcs;
    if (!(active & (shift < 32 ? 1u << shift : 0)))
        return 0;
    costume = (u8)mMdlNo;
    status = (u32)actor_status;
    mask = (u32)l_statusMasks.mEntries[costume];
    actor_status = status | mask;
    return 1;
}

/* 021EB870 */
s32 daNpcAuction_c::getRand(s32 limit) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s32 result = gabi::ftoi(auction_cM_rndF((f32)limit));
    return (u32)result == (u32)limit ? 0 : result;
}

/* 021EC3AC */
void daNpcAuction_c::clrEmitter() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auction_cpp::Emitter* emitter = mpEmitter;
    if (!emitter)
        return;
    u32 flags = (u32)emitter->mStatus;
    emitter->mId = 0xFFFFFFFF;
    auction_cpp::ParticleNode* node = emitter->mpParticles.get();
    emitter->mStatus = flags | 1;
    while (node) {
        auction_cpp::Particle* particle = node->mpParticle.get();
        particle->mStatus = (u32)particle->mStatus | 2;
        node = node->mpNext.get();
    }
    auction_cpp::bits(&mpEmitter) = 0;
}

/* 021EC014 */
u32 daNpc_AuctionCreate(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!(u32)l_phaseInitialized) {
        l_phaseInitialized = 1;
        auction_phaseInit(gabi::ea(&l_phaseTable), gabi::ea(&l_phaseTemplate), 12);
    }
    return auction_phaseExecute(gabi::ea(&a->mPhase), gabi::ea(&l_phaseTable), gabi::ea(a));
}

/* 021EC084 */
u32 daNpc_AuctionDelete(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 costume = (u8)a->mMdlNo;
    auction_deleteArchive(gabi::ea(&a->mPhs), (u32)l_arcNames.mEntries[costume]);
    if ((u32)auction_cpp::bits(&a->heap)) {
        mDoExt_McaMorf* morf = a->mpMorf.get();
        if (morf)
            auction_stopMorf(gabi::ea(morf));
    }
    return 1;
}

/* 021ED628 */
u32 daNpc_AuctionDraw(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!a->isExecute())
        return 1;
    J3DModel* model = a->mpMorf.get()->mpModel.get();
    J3DModel* extra = gabi::at<J3DModel>((u32)auction_cpp::bits(&a->mpExtraModel));
    u32 material = (u32)auction_cpp::bits(&model->mModelData);
    if (extra)
        material = (u32)auction_cpp::bits(&extra->mModelData);
    u32 lighting = auction_getLighting();
    auction_setLight(lighting, 0, gabi::ea(&a->current.pos), gabi::ea(&a->tevStr));
    mDoExt_McaMorf* morf = a->mpMorf.get();
    lighting = auction_getLighting();
    auction_setModelLight(lighting, auction_cpp::bits(&morf->mpModel), gabi::ea(&a->tevStr));
    if (extra) {
        lighting = auction_getLighting();
        auction_setModelLight(lighting, (u32)auction_cpp::bits(&a->mpExtraModel), gabi::ea(&a->tevStr));
    }
    auction_btpEntry(gabi::ea(&a->mBtp), material, (u8)a->mBlinkFrame);
    auction_morfEntry((u32)auction_cpp::bits(&a->mpMorf));
    if (extra) {
        s32 joint = (s8)a->m_jnt.mHeadJntNum;
        J3DMtxBuffer_hd* matrices = model->mpMtxBuffer.get();
        u16 flags = (u16)matrices->mFlags;
        Mtx34* matrix = gabi::at<Mtx34>((u32)matrices->mpAnmMtx + (u32)joint * 0x30);
        extra = gabi::at<J3DModel>((u32)auction_cpp::bits(&a->mpExtraModel));
        matrices->mFlags = flags | 0x10;
        f32 values[12];
        for (u32 i = 0; i < 12; ++i)
            values[i] = (f32)matrix->m[i / 4][i % 4];
        for (u32 i = 0; i < 12; ++i)
            extra->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
        auction_modelUpdate((u32)auction_cpp::bits(&a->mpExtraModel), 0);
    }
    gabi::at<auction_cpp::Material>(material)->mAnimation = 0;
    u8 costume = (u8)a->mMdlNo;
    auction_drawShadow((u8)l_shadowTypes.mEntries[costume], gabi::ea(a), 1.0f, 1.0f, 1.0f);
    return 1;
}

/* 021ED31C */
u32 daNpc_AuctionExecute(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!a->isExecute())
        return 0;
    AuctionHIO* parameters = &l_HIO.mCostumes[(u8)a->mMdlNo];
    s16 angles[9];
    for (u32 i = 0; i < 9; ++i)
        angles[i] = (s16)(u16)parameters->mAngles[i];
    auction_setJointLimits(gabi::ea(&a->m_jnt), angles[1], angles[3], angles[5], angles[7], angles[0],
                           angles[2], angles[4], angles[6], angles[8]);
    a->checkOrder();
    auction_cpp::Play* play = auction_getPlay();
    bool action = !(u8)play->mEventRunning;
    if (!action)
        action = (u16)a->eventInfo.mCommand == 1 && (u8)a->mCanTalk != 0;
    if (action) {
        s32 offset = (s16)a->mActionDelta;
        s32 virtualIndex = (s16)a->mActionIndex;
        u32 receiver = gabi::ea(a) + (u32)offset;
        a->mEmitterEnabled = 0;
        u32 target;
        if (virtualIndex < 0)
            target = (u32)a->mActionFunction;
        else {
            s32 tableOffset = (s16)a->mActionVtableOffset();
            target = load<u32>(load<u32>(receiver + (u32)tableOffset) + (u32)virtualIndex * 8 + 4);
        }
        gabi::call_ptr(target, receiver);
    } else
        a->eventMove();
    a->eventOrder();
    a->playTexPatternAnm();
    a->playAnm();
    play = auction_getPlay();
    auction_correctBackground(gabi::ea(&a->mObjAcch), gabi::ea(play) + 0x12A0);
    auction_setAttention(gabi::ea(a), 60.0f, 150.0f);
    parameters = &l_HIO.mCostumes[(u8)a->mMdlNo];
    f32 x = (f32)a->current.pos.x, z = (f32)a->current.pos.z, y = (f32)a->current.pos.y;
    f32 height = (f32)parameters->mAttentionHeight;
    a->attention_info.position.z = z;
    a->attention_info.position.x = x;
    a->attention_info.position.y = y + height;
    height = (f32)parameters->mLookHeight;
    a->eyePos.z = z;
    a->eyePos.x = x;
    a->eyePos.y = y + height;
    a->lookBack();
    a->setMtx();
    s16 timer = (s16)a->mVoiceTimer;
    if (timer) {
        timer = (s16)(timer - 1);
        a->mVoiceTimer = timer;
        if (!timer)
            auction_startVoice((u32)a->mVoiceId);
    }
    parameters = &l_HIO.mCostumes[(u8)a->mMdlNo];
    a->mPiconOfsY = (f32)parameters->mPiconHeight;
    return 0;
}

/* 021EE404 */
u32 daNpc_AuctionIsDelete(daNpcAuction_c* a) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 021EE3F0 */
void SafeString_dtor(u32 object, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (object && (flags & 1))
        auction_operatorDelete(object);
}

/* 021EE4A8 */
void SafeString_assureTerminationImpl(u32 object) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 021EE40C */
void daNpcAuction_c_dt(daNpcAuction_c* a, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 self = gabi::ea(a);
    if (!self)
        return;
    auction_cylinderDestroy(gabi::ea(&a->mCyl), 2);
    auction_statusDestroy(gabi::ea(&a->mStts), 2);
    auction_circleDestroy(gabi::ea(&a->mAcchCir.m_cir), 2);
    a->mObjAcchView().mActorVtable = ACCH_ACTOR_VTABLE;
    a->mObjAcchView().mCheckVtable = ACCH_CHECK_VTABLE;
    auction_acchDestroy(gabi::ea(&a->mObjAcch), 0);
    auction_actorDestroy(self, 0);
    if (flags & 1)
        auction_operatorDelete(self);
}

/* 021EDDC8 */
void __sinit_d_a_npc_auction_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_guard3 = 0x0;
    l_guard2 = 0x0;
    l_guard1 = 0x0;
    l_guard0 = 0x0;
    auction_staticInit(gabi::ea(&l_staticInitNodes));

    f32 defaultScaleValue1 = l_defaultScale;
    f32 defaultOffsetValue1 = l_defaultOffset;
    l_defaultSize = defaultScaleValue1;
    l_defaultAngle = defaultOffsetValue1;
    auction_initEyeState(gabi::ea(&l_eyeInit));

    auction_staticInit(gabi::ea(&l_eyeInitNode));

    auction_initLookState(gabi::ea(&l_lookInit));

    auction_staticInit(gabi::ea(&l_lookInitNode));

    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[0]));

    l_HIO.mCostumes[0].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[0].mAngles[1] = 0x0;
    f32 height7Value1 = l_height7;
    l_HIO.mCostumes[0].mAngles[2] = 0x1F40;
    l_HIO.mCostumes[0].mAngles[3] = 0x1F40;
    l_HIO.mCostumes[0].mAngles[4] = 0xF18C;
    f32 height4Value1 = l_height4;
    f32 zeroValue1 = l_zero;
    l_HIO.mCostumes[0].mEyeHeight = zeroValue1;
    f32 height6Value1 = l_height6;
    l_HIO.mCostumes[0].mAttentionHeight = height4Value1;
    l_HIO.mCostumes[0].mAngles[5] = 0x0;
    l_HIO.mCostumes[0].mAngles[6] = 0xE0C0;
    f32 height5Value1 = l_height5;
    l_HIO.mCostumes[0].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[0].mLookHeight = height5Value1;
    l_HIO.mCostumes[0].mParticleHeight = height6Value1;
    l_HIO.mCostumes[0].mPiconHeight = height7Value1;
    l_HIO.mCostumes[0].mDebug = 0x0;
    l_HIO.mCostumes[0].mAngles[7] = 0xE0C0;
    l_HIO.mCostumes[0].mAngles[8] = 0x5DC;
    l_HIO.mCostumes[0].mTurnSpeed = 0x6A4;
    l_HIO.mCostumes[0].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[0].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[1]));

    l_HIO.mCostumes[1].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[1].mAngles[1] = 0x0;
    l_HIO.mCostumes[1].__vtbl = HIO_VTABLE;
    f32 height9Value1 = l_height9;
    l_HIO.mCostumes[1].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[1].mAngles[2] = 0xFA0;
    l_HIO.mCostumes[1].mAngles[3] = 0x1B58;
    l_HIO.mCostumes[1].mAngles[4] = 0xF060;
    l_HIO.mCostumes[1].mDebug = 0x0;
    f32 tenValue1 = l_ten;
    l_HIO.mCostumes[1].mAngles[5] = 0x0;
    f32 height8Value1 = l_height8;
    l_HIO.mCostumes[1].mAngles[6] = 0xE4A8;
    l_HIO.mCostumes[1].mAngles[7] = 0xE0C0;
    l_HIO.mCostumes[1].mAngles[8] = 0x3E8;
    l_HIO.mCostumes[1].mTurnSpeed = 0x5DC;
    l_HIO.mCostumes[1].mAngleLimit = 0x7FFF;
    l_HIO.mCostumes[1].mAttentionHeight = height8Value1;
    l_HIO.mCostumes[1].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[1].mLookHeight = tenValue1;
    l_HIO.mCostumes[1].mParticleHeight = height9Value1;
    l_HIO.mCostumes[1].mPiconHeight = height8Value1;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[2]));

    l_HIO.mCostumes[2].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[2].mAngles[1] = 0x0;
    l_HIO.mCostumes[2].mAngles[2] = 0x1F40;
    l_HIO.mCostumes[2].mAngles[3] = 0x1F40;
    l_HIO.mCostumes[2].mAngles[4] = 0xE4A8;
    l_HIO.mCostumes[2].mAngles[5] = 0x0;
    l_HIO.mCostumes[2].mAngles[6] = 0xE0C0;
    l_HIO.mCostumes[2].mAngles[7] = 0xE0C0;
    l_HIO.mCostumes[2].mAngles[8] = 0x3E8;
    l_HIO.mCostumes[2].mTurnSpeed = 0x514;
    l_HIO.mCostumes[2].mAngleLimit = 0x7FFF;
    f32 height10Value1 = l_height10;
    l_HIO.mCostumes[2].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[2].mAttentionHeight = height10Value1;
    l_HIO.mCostumes[2].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[2].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[2].mLookHeight = tenValue1;
    l_HIO.mCostumes[2].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[2].mDebug = 0x0;
    l_HIO.mCostumes[2].mPiconHeight = height10Value1;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[3]));

    l_HIO.mCostumes[3].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[3].mAngles[1] = 0x0;
    l_HIO.mCostumes[3].mAngles[2] = 0x1388;
    l_HIO.mCostumes[3].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[3].mAngles[3] = 0x1F40;
    l_HIO.mCostumes[3].mAngles[4] = 0xF254;
    l_HIO.mCostumes[3].mAngles[5] = 0x0;
    l_HIO.mCostumes[3].mEyeHeight = zeroValue1;
    f32 height11Value1 = l_height11;
    l_HIO.mCostumes[3].mAngles[6] = 0xEC78;
    l_HIO.mCostumes[3].mAttentionHeight = height11Value1;
    l_HIO.mCostumes[3].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[3].mLookHeight = tenValue1;
    l_HIO.mCostumes[3].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[3].mPiconHeight = height11Value1;
    l_HIO.mCostumes[3].mAngles[7] = 0xE0C0;
    l_HIO.mCostumes[3].mAngles[8] = 0x3E8;
    l_HIO.mCostumes[3].mTurnSpeed = 0x5DC;
    l_HIO.mCostumes[3].mDebug = 0x0;
    l_HIO.mCostumes[3].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[4]));

    l_HIO.mCostumes[4].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[4].mAngles[1] = 0x0;
    l_HIO.mCostumes[4].mAngles[2] = 0x2328;
    l_HIO.mCostumes[4].mAngles[3] = 0x1388;
    l_HIO.mCostumes[4].mAngles[4] = 0xEC78;
    l_HIO.mCostumes[4].mDebug = 0x0;
    l_HIO.mCostumes[4].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[4].mAngles[5] = 0x0;
    l_HIO.mCostumes[4].mAngles[6] = 0xDCD8;
    l_HIO.mCostumes[4].mAttentionHeight = height10Value1;
    l_HIO.mCostumes[4].mAngles[7] = 0xEC78;
    l_HIO.mCostumes[4].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[4].mLookHeight = tenValue1;
    l_HIO.mCostumes[4].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[4].mPiconHeight = height10Value1;
    l_HIO.mCostumes[4].mAngles[8] = 0x7D0;
    l_HIO.mCostumes[4].mTurnSpeed = 0x5DC;
    l_HIO.mCostumes[4].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[4].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[5]));

    l_HIO.mCostumes[5].mDebug = 0x0;
    l_HIO.mCostumes[5].mAngles[0] = 0x9C4;
    l_HIO.mCostumes[5].mAngles[1] = 0x0;
    l_HIO.mCostumes[5].mAngles[2] = 0x1B58;
    l_HIO.mCostumes[5].mAngles[3] = 0x1B58;
    l_HIO.mCostumes[5].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[5].mAngles[4] = 0xFC18;
    l_HIO.mCostumes[5].mAngles[5] = 0x0;
    l_HIO.mCostumes[5].mAngles[6] = 0xE4A8;
    l_HIO.mCostumes[5].mAngles[7] = 0xE4A8;
    l_HIO.mCostumes[5].mAngles[8] = 0x2BC;
    l_HIO.mCostumes[5].mTurnSpeed = 0x514;
    l_HIO.mCostumes[5].mAngleLimit = 0x7FFF;
    f32 height12Value1 = l_height12;
    l_HIO.mCostumes[5].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[5].mAttentionHeight = height12Value1;
    l_HIO.mCostumes[5].mTalkDistance = zeroValue1;
    f32 height13Value1 = l_height13;
    l_HIO.mCostumes[5].mLookHeight = tenValue1;
    l_HIO.mCostumes[5].mParticleHeight = height13Value1;
    l_HIO.mCostumes[5].mPiconHeight = height12Value1;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[6]));

    l_HIO.mCostumes[6].mEyeHeight = zeroValue1;
    f32 height14Value1 = l_height14;
    l_HIO.mCostumes[6].mDebug = 0x0;
    l_HIO.mCostumes[6].mAttentionHeight = height14Value1;
    l_HIO.mCostumes[6].mAngles[0] = 0x9C4;
    l_HIO.mCostumes[6].mAngles[1] = 0x0;
    l_HIO.mCostumes[6].mAngles[2] = 0x1B58;
    l_HIO.mCostumes[6].mAngles[3] = 0x1B58;
    l_HIO.mCostumes[6].mAngles[4] = 0xFC18;
    l_HIO.mCostumes[6].mAngles[5] = 0x0;
    l_HIO.mCostumes[6].mAngles[6] = 0xE4A8;
    l_HIO.mCostumes[6].mAngles[7] = 0xE4A8;
    l_HIO.mCostumes[6].mAngles[8] = 0x2BC;
    l_HIO.mCostumes[6].mTurnSpeed = 0x514;
    l_HIO.mCostumes[6].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[6].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[6].mLookHeight = tenValue1;
    l_HIO.mCostumes[6].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[6].mPiconHeight = height14Value1;
    l_HIO.mCostumes[6].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[7]));

    l_HIO.mCostumes[7].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[7].mAngles[1] = 0x0;
    l_HIO.mCostumes[7].mDebug = 0x0;
    l_HIO.mCostumes[7].mAngles[2] = 0x2328;
    l_HIO.mCostumes[7].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[7].mAttentionHeight = height10Value1;
    l_HIO.mCostumes[7].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[7].mLookHeight = tenValue1;
    l_HIO.mCostumes[7].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[7].mPiconHeight = height10Value1;
    l_HIO.mCostumes[7].mAngles[3] = 0x1770;
    l_HIO.mCostumes[7].mAngles[4] = 0xFC18;
    l_HIO.mCostumes[7].mAngles[5] = 0x0;
    l_HIO.mCostumes[7].mAngles[6] = 0xDCD8;
    l_HIO.mCostumes[7].mAngles[7] = 0xE890;
    l_HIO.mCostumes[7].mAngles[8] = 0x7D0;
    l_HIO.mCostumes[7].mTurnSpeed = 0x5DC;
    l_HIO.mCostumes[7].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[7].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[8]));

    l_HIO.mCostumes[8].mAngles[0] = 0xFA0;
    l_HIO.mCostumes[8].mAngles[1] = 0x0;
    l_HIO.mCostumes[8].mAngles[2] = 0x1F40;
    l_HIO.mCostumes[8].mAngles[3] = 0x1388;
    l_HIO.mCostumes[8].mAngles[4] = 0xF060;
    l_HIO.mCostumes[8].mAngles[5] = 0x0;
    l_HIO.mCostumes[8].mAngles[6] = 0xE0C0;
    l_HIO.mCostumes[8].mAngles[7] = 0xEC78;
    l_HIO.mCostumes[8].mAngles[8] = 0x7D0;
    l_HIO.mCostumes[8].mTurnSpeed = 0x9C4;
    l_HIO.mCostumes[8].mEyeHeight = zeroValue1;
    l_HIO.mCostumes[8].mAttentionHeight = height8Value1;
    l_HIO.mCostumes[8].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[8].mDebug = 0x0;
    l_HIO.mCostumes[8].mAngleLimit = 0x7FFF;
    f32 height6Value2 = l_height6;
    l_HIO.mCostumes[8].mLookHeight = tenValue1;
    l_HIO.mCostumes[8].mParticleHeight = height6Value2;
    l_HIO.mCostumes[8].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[8].mPiconHeight = height8Value1;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[9]));

    l_HIO.mCostumes[9].mEyeHeight = zeroValue1;
    f32 height15Value1 = l_height15;
    l_HIO.mCostumes[9].mDebug = 0x0;
    l_HIO.mCostumes[9].mAttentionHeight = height15Value1;
    l_HIO.mCostumes[9].mAngles[0] = 0x9C4;
    l_HIO.mCostumes[9].mAngles[1] = 0x7D0;
    l_HIO.mCostumes[9].mAngles[2] = 0x1770;
    l_HIO.mCostumes[9].mAngles[3] = 0x1F40;
    l_HIO.mCostumes[9].mAngles[4] = 0xFE0C;
    l_HIO.mCostumes[9].mAngles[5] = 0xF830;
    l_HIO.mCostumes[9].mAngles[6] = 0xE890;
    l_HIO.mCostumes[9].mAngles[7] = 0xE0C0;
    l_HIO.mCostumes[9].mAngles[8] = 0x3E8;
    l_HIO.mCostumes[9].mTurnSpeed = 0x5DC;
    l_HIO.mCostumes[9].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[9].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[9].mLookHeight = height8Value1;
    l_HIO.mCostumes[9].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[9].mPiconHeight = height15Value1;
    l_HIO.mCostumes[9].mAngleLimit = 0x7FFF;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[10]));

    l_HIO.mCostumes[10].__vtbl = HIO_VTABLE;
    l_HIO.mCostumes[10].mAngles[0] = 0x200;
    l_HIO.mCostumes[10].mAngles[1] = 0x1388;
    l_HIO.mCostumes[10].mAngles[2] = 0x200;
    l_HIO.mCostumes[10].mAngles[3] = 0x1770;
    f32 height18Value1 = l_height18;
    f32 height17Value1 = l_height17;
    f32 height16Value1 = l_height16;
    f32 height14Value2 = l_height14;
    l_HIO.mCostumes[10].mEyeHeight = height16Value1;
    l_HIO.mCostumes[10].mAttentionHeight = height14Value2;
    l_HIO.mCostumes[10].mTalkDistance = height17Value1;
    l_HIO.mCostumes[10].mLookHeight = height18Value1;
    l_HIO.mCostumes[10].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[10].mAngles[4] = 0xFE00;
    l_HIO.mCostumes[10].mAngles[5] = 0xEC78;
    l_HIO.mCostumes[10].mAngles[6] = 0xFE00;
    f32 height19Value1 = l_height19;
    l_HIO.mCostumes[10].mDebug = 0x0;
    l_HIO.mCostumes[10].mAngles[7] = 0xE890;
    l_HIO.mCostumes[10].mAngles[8] = 0x1000;
    l_HIO.mCostumes[10].mTurnSpeed = 0x800;
    l_HIO.mCostumes[10].mAngleLimit = 0x7FFF;
    l_HIO.mCostumes[10].mPiconHeight = height19Value1;
    auction_hioConstruct(gabi::ea(&l_HIO.mCostumes[11]));

    l_HIO.mCostumes[11].mAngles[0] = 0x200;
    l_HIO.mCostumes[11].mAngles[1] = 0x1388;
    l_HIO.mCostumes[11].mAngles[2] = 0x200;
    l_HIO.mCostumes[11].mAngles[3] = 0x1770;
    l_HIO.mCostumes[11].mAngles[4] = 0xFE00;
    l_HIO.mCostumes[11].mAngles[5] = 0xEC78;
    l_HIO.mCostumes[11].mAngles[6] = 0xFE00;
    l_HIO.mCostumes[11].mAngles[7] = 0xE890;
    l_HIO.mCostumes[11].mAngles[8] = 0x1000;
    l_HIO.mCostumes[11].mTurnSpeed = 0x800;
    l_HIO.mCostumes[11].mAngleLimit = 0x7FFF;
    f32 height16Value2 = l_height16;
    f32 height20Value1 = l_height20;
    l_HIO.mCostumes[11].mEyeHeight = height16Value2;
    l_HIO.mCostumes[11].mAttentionHeight = height20Value1;
    f32 height18Value2 = l_height18;
    l_HIO.mCostumes[11].mTalkDistance = zeroValue1;
    l_HIO.mCostumes[11].mLookHeight = height18Value2;
    l_HIO.mCostumes[11].mParticleHeight = zeroValue1;
    l_HIO.mCostumes[11].__vtbl = HIO_VTABLE;
    f32 height19Value2 = l_height19;
    l_HIO.mCostumes[11].mDebug = 0x0;
    l_HIO.mCostumes[11].mPiconHeight = height19Value2;
}

// clang-format off
/* WWHD glue
data l_HIO 10465A58
data l_zero 10016694
data l_ten 100166F0
data l_defaultScale 100167B0
data l_defaultOffset 100167B4
data l_height4 100167B8
data l_height5 100167BC
data l_height6 100167C0
data l_height7 100167C4
data l_height8 100167C8
data l_height9 100167CC
data l_height10 100167D0
data l_height11 100167D4
data l_height12 100167D8
data l_height13 100167DC
data l_height14 100167E0
data l_height15 100167E4
data l_height16 100167E8
data l_height17 100167EC
data l_height18 100167F0
data l_height19 100167F4
data l_height20 100167F8
data l_guard3 10465A54
data l_guard2 10465A50
data l_guard1 10465A4C
data l_guard0 10465A48
data l_defaultSize 10465A3C
data l_defaultAngle 10465A40
data l_eyeInit 10465A44
data l_lookInit 10465A45
ext 0200F428 void auction_approachAngle(u32, u32, u32, u32)
ext 02018034 void auction_circleDestroy(u32, u32)
ext 020198D8 f32 auction_cM_rndF(f32)
ext 02055B64 u32 auction_decrementTimer(u32)
ext 0205CD90 u32 auction_isSecondQuest()
ext 024EFD9C void auction_acchDestroy(u32, u32)
ext 024EFF44 void auction_setWall(u32, f32, f32)
ext 024F06B4 void auction_acchSet(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 024F08A8 void auction_correctBackground(u32, u32)
ext 02515860 void auction_statusDestroy(u32, u32)
ext 02515A70 void auction_cylinderDestroy(u32, u32)
ext 02515F14 void auction_statusInit(u32, u32, u32, u32)
ext 02516518 void auction_cylinderSet(u32, u32)
ext 025200D4 auction_cpp::Play* auction_getPlay()
ext 02520460 u32 auction_loadArchive(u32, u32)
ext 025204C8 void auction_deleteArchive(u32, u32)
ext 02520C0C u32 auction_isTriforce(u32)
ext 02525FE4 u32 auction_phaseExecute(u32, u32, u32)
ext 0253E9B0 void auction_setArchiveName(u32, u32)
ext 02542D88 s32 auction_getStaffId(u32, u32, u32, u32)
ext 02542EDC u32 auction_getActionIndex(u32, u32, u32, u32, u32, u32)
ext 02543280 void auction_cutEnd(u32, u32)
ext 02543F10 s32 auction_getEventIndex(u32, u32, u32)
ext 0254457C u32 auction_eventEndCheck(u32, u32)
ext 025447C8 u32 auction_isAdvance(u32, u32)
ext 0254487C u32 auction_substance(u32, u32, u32, u32)
ext 02555D0C u32 auction_getLighting()
ext 025626A4 void auction_setLight(u32, u32, u32, u32)
ext 02562F5C void auction_setModelLight(u32, u32, u32)
ext 0259D54C void auction_playerEyePos(u32, f32)
ext 0259D624 void auction_distanceAngle(u32, u32, u32, u32)
ext 0259DA18 void auction_hioConstruct(u32)
ext 0259DED0 void auction_lookAtTarget(u32, u32, u32, u32, u32, u32, u32)
ext 0259E08C void auction_setJointLimits(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 0259F814 void auction_eventCutSet(u32, u32, u32)
ext 0259F858 u32 auction_eventCutMove(u32)
ext 025A11EC u32 auction_talk(u32, u32)
ext 025A1458 void auction_fopNpcConstruct(u32)
ext 025A15AC void auction_setAttention(u32, f32, f32)
ext 025A847C auction_cpp::Emitter* auction_particleSet(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 025B7270 void auction_onAuctionWin(u32)
ext 025B8B68 void auction_onEventBit(u32, u32)
ext 025B8B94 u32 auction_isEventBit(u32, u32)
ext 025BED80 void auction_drawShadow(u32, u32, f32, f32, f32)
ext 025D50BC void auction_actorDestroy(u32, u32)
ext 025D5218 auction_cpp::Manager* auction_judge(u32, u32)
ext 025D5578 u32 auction_searchByName(u32, u32)
ext 025D63E8 u32 auction_entrySolidHeap(u32, u32, u32)
ext 025D76A8 void auction_orderSpeak(u32)
ext 025E1988 void auction_startVoice(u32)
ext 025E2DE0 void auction_modelUpdate(u32, u32)
ext 025E38E0 u32 auction_createModel(u32, u32, u32)
ext 025E4A98 void auction_morfSetAnimation(u32, u32, u32, u32, f32, f32, f32, f32)
ext 025E4F64 u32 auction_createMorf(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, f32)
ext 025E535C u32 auction_morfPlay(u32, u32, u32, u32)
ext 025E54D8 void auction_morfEntry(u32)
ext 025E563C void auction_stopMorf(u32)
ext 025E7820 void auction_btpConstruct(u32)
ext 025E789C u32 auction_btpInit(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32)
ext 025E7B3C void auction_btpEntry(u32, u32, u32)
ext 025F1BF4 void auction_matrixXRotate(u32, u32)
ext 025F1C28 void auction_matrixYRotate(u32, u32)
ext 025F1C5C void auction_matrixZRotate(u32, u32)
ext 026067F4 u32 auction_getResource(u32, u32, u32)
ext 0273AA24 void auction_panic(u32, u32, u32)
ext 0273AD10 u32 auction_operatorNew(u32)
ext 0273AF40 void auction_operatorDelete(u32)
ext 027DF9B0 s32 auction_findJointName(u32, u32)
ext 027F3F94 u32 auction_jointTree(u32)
ext 027F4D5C void auction_modelCalc(u32)
ext 027F68FC u32 auction_jointNames(u32)
ext 027F7878 u32 auction_toJoint(u32)
ext 028249B0 void auction_emitterSetGlobalMatrix(u32, u32, u32)
ext 028E90D4 void auction_matrixCopy(u32, u32)
ext 028E9108 void auction_matrixConcat(u32, u32, u32)
ext 028E93CC void auction_matrixTranslate(u32, f32, f32, f32)
ext 028EAB2C void auction_initLookState(u32)
ext 028ED6F8 void auction_initEyeState(u32)
ext 028F026C void auction_staticInit(u32)
ext 028F521C void auction_memclr(u32, u32)
ext C000A848 void auction_phaseInit(u32, u32, u32)
data l_arcNames 101BB7FC
data l_modelNumbers 1001633C
data l_extraModels 1001636C
data l_btpNumbers 1001639C
data l_eventNames 101BB698
data l_jointCallbacks 101BB6B8
data l_statusMasks 101BB6D8
data l_shadowTypes 1001640C
data l_animationNumbers 10016418
data l_costumeRanges 100163CC
data l_messageNumbers 101BB740
data l_resControl 101F4F28
data l_save 101F84DC
data l_message 101F4B5C
data l_calcMtx 1018C7B0
data l_currentModel 104B462C
data l_phaseInitialized 101FDB54
data l_translationMtx 1048D0CC
data l_currentMtx 104B4868
str 100165FC "d_a_npc_auction.cpp" in 021EB318
str 10016610 "m_head_tex_pattern != (0)" in 021EB318
str 100166D8 "DEFAULT_TALK_XY" in 021EBBBC
str 10016630 "head" in 021EB450
str 10016638 "d_a_npc_auction.cpp" in 021EB450
str 1001664C "m_jnt.getHeadJntNum() >= 0" in 021EB450
str 10016668 "backbone" in 021EB450
str 10016674 "m_jnt.getBackboneJntNum() >= 0" in 021EB450
str 100166F4 "AUCTION_TALK_XY" in 021EBD5C
str 10016704 "AUCTION_TALK" in 021EBD5C
str 10016714 "MsgNo" in 021EC20C
str 1001678C "d_a_npc_auction.cpp" in 021EDBF8
str 100167A0 "actAuc != (0)" in 021EDBF8
str 100166A0 "d_a_npc_auction.cpp" in 021EB8DC
str 100166B4 "npcNo < daNpcAuction_c::Npc_No_Max" in 021EB8DC
data l_messageAnimation0 101BB670
data l_messageAnimation1 101BB678
data l_messageAnimation2 101BB680
data l_messageAnimation3 101BB683
data l_messageAnimation4 101BB686
data l_messageAnimation5 101BB689
data l_messageAnimation6 101BB68C
data l_messageAnimation7 101BB68F
data l_messageAnimation8 101BB692
data l_messageAnimation9 101BB695
data l_messageAnimation10 101BB734
str 10016764 "AUCTION_GET_ITEM" in 021ECB68
str 10016744 "AUCTION_NO_ITEM" in 021ECB68
str 10016778 "AUCTION_END2" in 021ECB68
str 10016754 "AUCTION_TALK_XY" in 021ECB68
data l_phaseTable 101FDB58
data l_phaseTemplate 101BB728
data l_staticInitNodes 101BB7D8
data l_eyeInitNode 101BB7E4
data l_lookInitNode 101BB7F0
data l_actionNames 101BB7D0
data l_cylinderSource 101EA190
*/
// clang-format on
