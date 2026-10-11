/* Auction NPC, WWHD layout.
 * HD: fopNpc_npc_c ends at 0x7DC. The GHS action pointer has two s16 words and a function word;
 * the embedded texture animation grew to 0x74, placing the GameCube mPiconOfsY at HD 0x894.
 */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "JSystem/J3DGraph.h"
#include "d/d_npc.h"
struct sAnmDat {
    be<u8> mAnmNo;
    be<u8> mMorf;
    be<s8> mRepeat;
};
WWHD_SIZE(sAnmDat, 3);
namespace auction_cpp {

// HD: partial views of objects owned by other actors; padding records the verified layout.
struct Manager {
    u8 _00[0x37C];
    cXyz eyePos;
    u8 _388[0x850 - 0x388];
    be<u32> mNpcIds[8];
    u8 _870[0x924 - 0x870];
    be<u8> mNpcKinds[8];
    be<u8> mCostumes[8];
    be<u8> mPairedCostumes[2];
    u8 _936[2];
    be<u8> mActiveNpcs;
    be<u8> mSelection;
    u8 _93A;
    be<u8> mItemChoice;
    be<u8> mCurrentNpc;
    u8 _93D[2];
    be<u8> mResult;
    u8 _940;
    be<u8> mSpeakingNpc;
    u8 _942[10];
    be<u8> mGestureFlags;
};
WWHD_OFFSET(Manager, mNpcKinds, 0x924);
WWHD_OFFSET(Manager, mGestureFlags, 0x94C);
struct Particle {
    u8 _00[0xCC];
    be<u32> mStatus;
};
struct ParticleNode {
    gptr<Particle> mpParticle;
    u8 _04[8];
    gptr<ParticleNode> mpNext;
};
struct Emitter {
    u8 _00[0x5C];
    be<u32> mId;
    u8 _60[0x1AC - 0x60];
    gptr<ParticleNode> mpParticles;
    u8 _1B0[0x1F0 - 0x1B0];
    u8 mGlobalRotation[0x3C];
    cXyz mGlobalTranslation;
    u8 _238[0x254 - 0x238];
    be<u32> mStatus;
};
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mStatus, 0x254);
struct Message {
    be<u32> mMessageNo;
    u8 _04[0x948 - 4];
    be<u32> mChoice;
};
struct JointTree {
    u8 _00[8];
    be<u16> mJointCount;
};
struct JointData {
    u8 _00[4];
    be<u32> mExtent;
    be<u32> mEntries;
};
struct JointName {
    u8 _00[0x10];
    be<u32> mNameOffset;
};
struct Material {
    u8 _00[0x38];
    be<u32> mAnimation;
};
struct Joint {
    u8 _00[4];
    be<u16> mJointNo;
    u8 _06[2];
    be<u32> mCallback;
};
struct MatrixBlock {
    u8 _00[4];
    be<u16> mFlags;
    u8 _06[10];
    gptr<Mtx34> mpMatrices;
};
struct Play {
    u8 _00[0x5292];
    be<u8> mEventRunning;
    u8 _5293[0x52B0 - 0x5293];
    be<u8> mTalkXY;
    be<u8> mPreItem;
    u8 _52B2[6];
    be<u16> mEventFlags;
    u8 _52BA[10];
    u8 mEventManager[0x5AB0 - 0x52C4];
    be<u32> mParticle;
    u8 _5AB4[0x5B34 - 0x5AB4];
    be<u32> mPlayer;
    u8 _5B38[0x5BBB - 0x5B38];
    be<u8> mXYItems[10];
    be<u8> mMsgTag;
};
WWHD_OFFSET(Play, mMsgTag, 0x5BC5);

struct AcchVtables {
    u8 _00[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mActorVtable;
};
WWHD_SIZE(AcchVtables, 0x24);
WWHD_OFFSET(AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(AcchVtables, mActorVtable, 0x20);
template <class T> inline be<u32>& bits(gptr<T>* p) {
    return *reinterpret_cast<be<u32>*>(p);
}
} // namespace auction_cpp
struct daNpcAuction_c : fopNpc_npc_c {
    u8 getPrmNpcNo();
    s32 getRand(s32 limit);
    void wait_action_init();
    u32 XyCheckCB(s32 event);
    s32 XyEventCB(s32 event);
    void eventMainInit();
    u32 initTexPatternAnm(u32 restart);
    u32 createHeap();
    void setMtx();
    u32 createInit();
    u32 isExecute();
    void checkOrder();
    void eventMesSetInit(s32 staff);
    void setAnm(s32 animation, s32 mode, f32 blend);
    void setAnm2(s32 animation, s32 mode, f32 blend);
    void clrEmitter();
    u32 eventMesSet();
    void setAnmTbl(u32 table);
    void eventOrder();
    void playTexPatternAnm();
    void playAnm();
    void privateCut();
    u32 eventMain();
    void setAnmFromMsgTag();
    void eventMove();
    void wait_action();
    u32 getMsg();
    u32 next_msgStatus(u32 message);
    void lookBack();
    be<s16>& mActionVtableOffset() {
        return *reinterpret_cast<be<s16>*>(reinterpret_cast<u8*>(&mActionFunction) + 2);
    }
    auction_cpp::AcchVtables& mObjAcchView() {
        return *reinterpret_cast<auction_cpp::AcchVtables*>(&mObjAcch);
    }
    /* 0x7DC */ be<s16> mActionDelta;
    /* 0x7DE */ be<s16> mActionIndex;
    /* 0x7E0 */ be<u32> mActionFunction;
    /* 0x7E4 */ request_of_phase_process_class mPhs;
    /* 0x7EC */ request_of_phase_process_class mPhase;
    /* 0x7F4 */ gptr<J3DModel> mpExtraModel;
    /* 0x7F8 */ gptr<J3DAnmTexPattern> mpTexture;
    /* 0x7FC */ mDoExt_btpAnm mBtp;
    /* 0x870 */ gptr<auction_cpp::Emitter> mpEmitter;
    /* 0x874 */ be<u32> mUnused874;
    /* 0x878 */ u8 _878[0x14];
    /* 0x88C */ gptr<sAnmDat> mpAnmTable;
    /* 0x890 */ be<f32> mMaxTalkDist;
    /* 0x894 */ be<f32> mPiconOfsY;
    /* 0x898 */ be<u32> mVoiceId;
    /* 0x89C */ be<u32> mBtpNo;
    /* 0x8A0 */ be<u16> mBlinkTimer;
    /* 0x8A2 */ be<s16> mLookVel;
    /* 0x8A4 */ be<s16> mEventId;
    /* 0x8A6 */ be<s16> mVoiceTimer;
    /* 0x8A8 */ be<u8> mTalking;
    /* 0x8A9 */ be<u8> mAttentive;
    /* 0x8AA */ be<u8> mOrder;
    /* 0x8AB */ be<u8> mBlinkFrame;
    /* 0x8AC */ be<u8> mNpcNo;
    /* 0x8AD */ be<u8> mMdlNo;
    /* 0x8AE */ be<u8> mUnused8AE;
    /* 0x8AF */ be<u8> mItemChoice;
    /* 0x8B0 */ be<u8> mCanTalk;
    /* 0x8B1 */ be<u8> mSelection;
    /* 0x8B2 */ be<u8> mAnmNo;
    /* 0x8B3 */ be<u8> mGestureDone;
    /* 0x8B4 */ be<s8> mAnmRepeat;
    /* 0x8B5 */ be<u8> mEmitterEnabled;
    /* 0x8B6 */ be<s8> mActIdx;
    /* 0x8B7 */ u8 _8B7;
};
WWHD_SIZE(daNpcAuction_c, 0x8B8);
WWHD_OFFSET(daNpcAuction_c, mActionDelta, 0x7DC);
WWHD_OFFSET(daNpcAuction_c, mActionIndex, 0x7DE);
WWHD_OFFSET(daNpcAuction_c, mActionFunction, 0x7E0);
WWHD_OFFSET(daNpcAuction_c, mPhs, 0x7E4);
WWHD_OFFSET(daNpcAuction_c, mPhase, 0x7EC);
WWHD_OFFSET(daNpcAuction_c, mpExtraModel, 0x7F4);
WWHD_OFFSET(daNpcAuction_c, mpTexture, 0x7F8);
WWHD_OFFSET(daNpcAuction_c, mBtp, 0x7FC);
WWHD_OFFSET(daNpcAuction_c, mpEmitter, 0x870);
WWHD_OFFSET(daNpcAuction_c, mUnused874, 0x874);
WWHD_OFFSET(daNpcAuction_c, _878, 0x878);
WWHD_OFFSET(daNpcAuction_c, mpAnmTable, 0x88C);
WWHD_OFFSET(daNpcAuction_c, mMaxTalkDist, 0x890);
WWHD_OFFSET(daNpcAuction_c, mPiconOfsY, 0x894);
WWHD_OFFSET(daNpcAuction_c, mVoiceId, 0x898);
WWHD_OFFSET(daNpcAuction_c, mBtpNo, 0x89C);
WWHD_OFFSET(daNpcAuction_c, mBlinkTimer, 0x8A0);
WWHD_OFFSET(daNpcAuction_c, mLookVel, 0x8A2);
WWHD_OFFSET(daNpcAuction_c, mEventId, 0x8A4);
WWHD_OFFSET(daNpcAuction_c, mVoiceTimer, 0x8A6);
WWHD_OFFSET(daNpcAuction_c, mTalking, 0x8A8);
WWHD_OFFSET(daNpcAuction_c, mAttentive, 0x8A9);
WWHD_OFFSET(daNpcAuction_c, mOrder, 0x8AA);
WWHD_OFFSET(daNpcAuction_c, mBlinkFrame, 0x8AB);
WWHD_OFFSET(daNpcAuction_c, mNpcNo, 0x8AC);
WWHD_OFFSET(daNpcAuction_c, mMdlNo, 0x8AD);
WWHD_OFFSET(daNpcAuction_c, mUnused8AE, 0x8AE);
WWHD_OFFSET(daNpcAuction_c, mItemChoice, 0x8AF);
WWHD_OFFSET(daNpcAuction_c, mCanTalk, 0x8B0);
WWHD_OFFSET(daNpcAuction_c, mSelection, 0x8B1);
WWHD_OFFSET(daNpcAuction_c, mAnmNo, 0x8B2);
WWHD_OFFSET(daNpcAuction_c, mGestureDone, 0x8B3);
WWHD_OFFSET(daNpcAuction_c, mAnmRepeat, 0x8B4);
WWHD_OFFSET(daNpcAuction_c, mEmitterEnabled, 0x8B5);
WWHD_OFFSET(daNpcAuction_c, mActIdx, 0x8B6);
WWHD_OFFSET(daNpcAuction_c, _8B7, 0x8B7);

// HD: boundary signatures retain GHS register widths (including full-word status results).
void auction_approachAngle(u32, u32, u32, u32);
void auction_circleDestroy(u32, u32);
f32 auction_cM_rndF(f32);
u32 auction_decrementTimer(u32);
u32 auction_isSecondQuest();
void auction_acchDestroy(u32, u32);
void auction_setWall(u32, f32, f32);
void auction_acchSet(u32, u32, u32, u32, u32, u32, u32, u32, u32);
void auction_correctBackground(u32, u32);
void auction_statusDestroy(u32, u32);
void auction_cylinderDestroy(u32, u32);
void auction_statusInit(u32, u32, u32, u32);
void auction_cylinderSet(u32, u32);
auction_cpp::Play* auction_getPlay();
u32 auction_loadArchive(u32, u32);
void auction_deleteArchive(u32, u32);
u32 auction_isTriforce(u32);
u32 auction_phaseExecute(u32, u32, u32);
void auction_setArchiveName(u32, u32);
s32 auction_getStaffId(u32, u32, u32, u32);
u32 auction_getActionIndex(u32, u32, u32, u32, u32, u32);
void auction_cutEnd(u32, u32);
s32 auction_getEventIndex(u32, u32, u32);
u32 auction_eventEndCheck(u32, u32);
u32 auction_isAdvance(u32, u32);
u32 auction_substance(u32, u32, u32, u32);
u32 auction_getLighting();
void auction_setLight(u32, u32, u32, u32);
void auction_setModelLight(u32, u32, u32);
void auction_playerEyePos(u32, f32);
void auction_distanceAngle(u32, u32, u32, u32);
void auction_hioConstruct(u32);
void auction_lookAtTarget(u32, u32, u32, u32, u32, u32, u32);
void auction_setJointLimits(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void auction_eventCutSet(u32, u32, u32);
u32 auction_eventCutMove(u32);
u32 auction_talk(u32, u32);
void auction_fopNpcConstruct(u32);
void auction_setAttention(u32, f32, f32);
auction_cpp::Emitter* auction_particleSet(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void auction_onAuctionWin(u32);
void auction_onEventBit(u32, u32);
u32 auction_isEventBit(u32, u32);
void auction_drawShadow(u32, u32, f32, f32, f32);
void auction_actorDestroy(u32, u32);
auction_cpp::Manager* auction_judge(u32, u32);
u32 auction_searchByName(u32, u32);
u32 auction_entrySolidHeap(u32, u32, u32);
void auction_orderSpeak(u32);
void auction_startVoice(u32);
void auction_modelUpdate(u32, u32);
u32 auction_createModel(u32, u32, u32);
void auction_morfSetAnimation(u32, u32, u32, u32, f32, f32, f32, f32);
u32 auction_createMorf(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, f32);
u32 auction_morfPlay(u32, u32, u32, u32);
void auction_morfEntry(u32);
void auction_stopMorf(u32);
void auction_btpConstruct(u32);
u32 auction_btpInit(u32, u32, u32, u32, u32, u32, u32, u32, u32, f32);
void auction_btpEntry(u32, u32, u32);
void auction_matrixXRotate(u32, u32);
void auction_matrixYRotate(u32, u32);
void auction_matrixZRotate(u32, u32);
u32 auction_getResource(u32, u32, u32);
void auction_panic(u32, u32, u32);
u32 auction_operatorNew(u32);
void auction_operatorDelete(u32);
s32 auction_findJointName(u32, u32);
u32 auction_jointTree(u32);
void auction_modelCalc(u32);
u32 auction_jointNames(u32);
u32 auction_toJoint(u32);
void auction_emitterSetGlobalMatrix(u32, u32, u32);
void auction_matrixCopy(u32, u32);
void auction_matrixConcat(u32, u32, u32);
void auction_matrixTranslate(u32, f32, f32, f32);
void auction_initLookState(u32);
void auction_initEyeState(u32);
void auction_staticInit(u32);
void auction_memclr(u32, u32);
void auction_phaseInit(u32, u32, u32);

// HD: metadata retained as image addresses, rather than host pointers.
enum AuctionMetadata : u32 {
    AUCTION_VTABLE = 0x10016804,
    ACCH_ACTOR_VTABLE = 0x1001630C,
    ACCH_CHECK_VTABLE = 0x1001631C,
    SAFE_STRING_VTABLE = 0x100162F4,
    HIO_VTABLE = 0x1001632C,
    WAIT_ACTION = 0x021ED81C,
    AUCTION_MANAGER_JUDGE = 0x025E121C,
};

u32 CheckCreateHeap(daNpcAuction_c*);
u32 daNpcAuction_XyCheckCB(daNpcAuction_c*, s32);
s32 daNpcAuction_XyEventCB(daNpcAuction_c*, s32);

enum AuctionState : u32 {
    AUCTION_MANAGER_PROCESS = 0x17E,
    AUCTION_ITEM_TRIFORCE_CHART = 150,
    AUCTION_ITEM_TREASURE_CHART = 151,
    AUCTION_EVENT_TALK = 1,
    AUCTION_EVENT_DEMO = 2,
};

struct AuctionHIO {
    be<f32> mEyeHeight;
    be<u16> mAngles[9];
    be<u16> mTurnSpeed;
    be<f32> mAttentionHeight;
    be<u16> mAngleLimit;
    be<u8> mDebug;
    u8 _1F;
    be<f32> mTalkDistance;
    be<u32> __vtbl;
    be<f32> mLookHeight;
    be<f32> mParticleHeight;
    be<f32> mPiconHeight;
};
WWHD_SIZE(AuctionHIO, 0x34);
struct AuctionHIOArray {
    AuctionHIO mCostumes[12];
};

WWHD_OFFSET(AuctionHIO, mEyeHeight, 0);
WWHD_OFFSET(AuctionHIO, mAngles, 4);
WWHD_OFFSET(AuctionHIO, mTurnSpeed, 0x16);
WWHD_OFFSET(AuctionHIO, mAttentionHeight, 0x18);
WWHD_OFFSET(AuctionHIO, mAngleLimit, 0x1C);
WWHD_OFFSET(AuctionHIO, mTalkDistance, 0x20);
WWHD_OFFSET(AuctionHIO, __vtbl, 0x24);
WWHD_OFFSET(AuctionHIO, mLookHeight, 0x28);
WWHD_OFFSET(AuctionHIO, mParticleHeight, 0x2C);
WWHD_OFFSET(AuctionHIO, mPiconHeight, 0x30);
WWHD_SIZE(auction_cpp::Manager, 0x94D);
WWHD_SIZE(auction_cpp::Emitter, 0x258);
WWHD_SIZE(auction_cpp::Particle, 0xD0);
WWHD_SIZE(auction_cpp::ParticleNode, 0x10);
WWHD_OFFSET(auction_cpp::ParticleNode, mpNext, 0xC);
WWHD_OFFSET(auction_cpp::Message, mChoice, 0x948);
WWHD_OFFSET(auction_cpp::Play, mEventManager, 0x52C4);
WWHD_OFFSET(auction_cpp::Play, mPlayer, 0x5B34);
WWHD_SIZE(auction_cpp::MatrixBlock, 0x14);
WWHD_OFFSET(auction_cpp::MatrixBlock, mpMatrices, 0x10);

daNpcAuction_c* daNpcAuction_c_ct(daNpcAuction_c* a);
u32 daNpc_Auction_nodeCallBack1(u32 jointObject, s32 phase);
u32 daNpc_Auction_nodeCallBack2(u32 jointObject, s32 phase);
u32 CheckCreateHeap(daNpcAuction_c* a);
u32 phase_1(daNpcAuction_c* a);
u32 phase_2(daNpcAuction_c* a);
u32 daNpcAuction_XyCheckCB(daNpcAuction_c* a, s32 event);
s32 daNpcAuction_XyEventCB(daNpcAuction_c* a, s32 event);
u32 daNpc_AuctionCreate(daNpcAuction_c* a);
u32 daNpc_AuctionDelete(daNpcAuction_c* a);
u32 daNpc_AuctionDraw(daNpcAuction_c* a);
u32 daNpc_AuctionExecute(daNpcAuction_c* a);
u32 daNpc_AuctionIsDelete(daNpcAuction_c* a);
