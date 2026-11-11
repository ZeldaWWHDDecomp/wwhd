// Auction controller, verified against the WWHD implementation.
#include "d/actor/d_a_auction.h"
#include "bindings.h"

static s16 l_rest_msg_time1[4] = {30000, 10000, 5000, -1000};
static s16 l_rest_msg_time2[4] = {15000, 10000, 5000, -1000};
static s16 l_after_bet_chk[4] = {20, 20, 40, 40};
static s16 l_item_dat22[2] = {42, 249};
static f32 l_npc_emitter_ofsy[8] = {120.0f, 180.0f, 180.0f, 180.0f, 180.0f, 180.0f, 210.0f, 180.0f};
static f32 l_after_bet_rate[4] = {2.0f, 1.5f, 1.2f, 1.1f};
static u32 l_after_bet_msg_no[4] = {7459, 7458, 7457, 7456};
static u32 l_link_face[8] = {0, 3, 4, 9, 14, 15, 16, 18};
static u32 l_rest_msg_no[3] = {7425, 7426, 7427};
static u32 l_rest_se_no[3] = {2279, 2280, 2281};
static f32 l_one = 1.0f;
static f32 l_timerX = 221.0f;
static f32 l_timerY = 439.0f;
static f32 l_timerWidth = 32.0f;
static f32 l_timerHeight = 419.0f;
static f32 l_zero = 0.0f;
static f32 l_lightY = 50.0f;
static f32 l_lightZ = -200.0f;
static f32 l_lightPower = 450.0f;
static f32 l_cameraRandomAngle = 8000.0f;
static f32 l_blendStep = 0.05f;
static f32 l_alphaStep = 4.0f;
static f32 l_alphaMaximum = 255.0f;
static f32 l_gaugeMaximum = 100.0f;
static f32 l_negativeOne = -1.0f;
static f32 l_gaugeIdleStep = 0.02f;
static f32 l_unsignedFloatBoundary = 2147483600.0f;
static f32 l_lateBidMultiplier = 2.0f;
static f32 l_midBidMultiplier = 1.5f;
static f32 l_framesPerSecond = 60.0f;
static f32 l_paralyzedCameraZ = -210.0f;
static f32 l_paralyzedCameraRadius = 256.0f;
static f32 l_piconCenterOffset = 30.0f;
static f32 l_cameraBlendScale = 0.4f;
static f32 l_cameraBlendMaximum = 700.0f;
static f32 l_cameraBlendMinimum = 0.1f;
static f32 l_angleMinimum = -3.1415927f;
static f32 l_angleMaximum = 3.1415927f;
static f32 l_auctionHIOBidMaximum = 50000.0f;
static f32 l_auctionHIOGaugeMaximum = 10000.0f;
static f32 l_cameraInitial0 = -265.0f;
static f32 l_cameraInitial1 = 48.0f;
static f32 l_cameraInitial2 = -631.0f;
static f32 l_cameraInitial3 = 332.0f;
static f32 l_cameraInitial4 = 232.0f;
static f32 l_cameraInitial5 = 286.0f;
static f32 l_cameraInitial6 = -50.0f;
static f32 l_cameraInitial7 = 202.0f;
static f32 l_cameraInitial8 = 137.0f;
static f32 l_cameraInitial9 = -87.0f;
static f32 l_cameraInitial10 = 239.0f;
static f32 l_cameraInitial11 = 330.0f;
static f32 l_cameraInitial12 = -216.0f;
static f32 l_cameraInitial13 = 205.0f;
static f32 l_cameraInitial14 = 5.0f;
static f32 l_cameraInitial15 = -379.0f;
static f32 l_cameraInitial16 = 238.0f;
static f32 l_cameraInitial17 = 115.0f;

// HD: a fifth auction prize is the fast sail; acquisition checks the equipped sail slot.
static AuctionItem l_item_dat[5] = {{31, 7440, 40, 3841},
                                    {228, 7441, 5, 4224},
                                    {237, 7442, 60, 4160},
                                    {7, 7443, 80, 4128},
                                    {119, 7444, 100, 0}};
static AuctionItem l_item_dat2[2] = {{150, 7444, 30, 4104}, {151, 7445, 40, 4100}};
static NpcDatStruct l_npc_dat[7] = {{2.0f, 2.1f, 0, 0, 1000}, {0.3f, 0.5f, 4, 6, 450},
                                    {0.3f, 0.6f, 3, 7, 500},  {0.4f, 0.7f, 2, 4, 250},
                                    {0.5f, 1.0f, 2, 3, 990},  {0.4f, 0.5f, 3, 4, 990},
                                    {0.7f, 1.2f, 2, 2, 150}};
static NpcMsgDatStruct l_npc_msg_dat[12] = {{13228, 7482, 7483, 7484, 13238},
                                            {13229, 7485, 7486, 7487, 13239},
                                            {13230, 7488, 7489, 7490, 13240},
                                            {13231, 7491, 7492, 7493, 13241},
                                            {13232, 7494, 7495, 7496, 13242},
                                            {13233, 7497, 7498, 7499, 13243},
                                            {13234, 13213, 13214, 13215, 13244},
                                            {13235, 13216, 13217, 13218, 13245},
                                            {13236, 13219, 13220, 13221, 13246},
                                            {13237, 13222, 13223, 13224, 13247},
                                            {7401, 0, 0, 0, 0},
                                            {7401, 0, 0, 0, 0}};
static NpcCameraDatStruct l_npc_camera_dat[7] = {{150.0f, 70.0f, -500, 0}, {200.0f, 100.0f, 0, 0},
                                                 {200.0f, 100.0f, 0, 0},   {200.0f, 100.0f, 0, 0},
                                                 {200.0f, 100.0f, 0, 0},   {200.0f, 100.0f, 0, 0},
                                                 {200.0f, 150.0f, 0, 0}};
static AuctionBetWait l_after_bet_wait[4] = {{8, 10}, {7, 10}, {6, 9}, {5, 8}};
static AuctionCameraTable l_camera_pos;
static AuctionCutNames l_cut_names;
static u32 l_resourceManager;
static u32 l_controllerManager;
static u32 l_saveInfo;
static u32 l_messageManager;

static AuctionHIOState l_HIO;
static AuctionOpaqueObject l_registerCamera;
static AuctionOpaqueObject l_registerHIO;
static AuctionOpaqueObject l_registerNpcHIO;
static AuctionOpaqueObject l_createScreenTask;
static AuctionOpaqueObject l_deleteScreenTask;
static AuctionProc moveProc[3] = {
    {&daAuction_c::executeWait}, {&daAuction_c::executeNormal}, {&daAuction_c::executeStart}};
static AuctionProc eventProc[6] = {
    {&daAuction_c::eventMainKai},       {&daAuction_c::eventMainUri},
    {&daAuction_c::eventMainMsgSet},    {&daAuction_c::eventMainMsgEnd},
    {&daAuction_c::eventMainMsgBikonC}, {&daAuction_c::eventMainMsgBikonW}};

namespace {
bool auctionEventEnded(daAuction_c* actor, be<s16>* eventField) {
    s16 event = *eventField;
    u32 play = dComIfGp_ea();
    return auction_eventEndCheck(&auctionGameInfo(play).play.mEvtManager, (s32)event) != 0;
}
void auctionEventFlag() {
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mEvtCtrl.mEventFlag =
        auctionGameInfo(play).play.mEvtCtrl.mEventFlag | 8;
}
} // namespace

// Private WWHD Auction event contribution.

namespace {
s32 eventTimeRemaining() {
    u32 play = dComIfGp_ea();
    u32 end = auctionGameInfo(play).play.mTimerInfo.mTimerLimitTimeMs;
    play = dComIfGp_ea();
    u32 start = auctionGameInfo(play).play.mTimerInfo.mTimerNowTimeMs;
    return (s32)((end - start) * 1000u) / 30;
}
void eventRestartTimer() {
    u32 play = dComIfGp_ea();
    if (auctionGameInfo(play).play.mTimerInfo.mTimerMode == 4) {
        play = dComIfGp_ea();
        u32 timer = auctionGameInfo(play).play.mTimerInfo.mTimerPtr;
        if (timer)
            auction_timerRestart(timer, 2);
    }
}
} // namespace

/* 02058EF4 */
daAuction_c* Auction_constructor(daAuction_c* actor) {
    if (!actor) {
        actor = auction_operator_new(0x954);
        if (!actor)
            return actor;
    }
    auction_fopNpc_npc_construct(actor);
    actor->__vtbl = 0x10007B98;
    auction_dNpc_EventCut_construct(&actor->mNpcEvtInfo);
    f32 light = l_one;
    actor->mLight.mIntensity = light;
    for (u32 i = 0; i < 8; ++i) {
        actor->mNpcIds[i] = 0xFFFFFFFF;
        actor->mNpcKinds[i] = i;
        actor->mAucMdlNo[i] = 0xFF;
    }
    for (u32 i = 0; i < 100; ++i) {
        u32 first = (u32)actor->getRand(6) + 1;
        u32 second = (u32)actor->getRand(6) + 1;

        u8 a = actor->mNpcKinds[second], b = actor->mNpcKinds[first];
        actor->mNpcKinds[first] = a;
        actor->mNpcKinds[second] = b;
    }
    u32 x = auctionWords(actor->current.pos).value[0];
    u32 z = auctionWords(actor->current.pos).value[2];
    actor->mPairedCostume0 = 0xFF;
    actor->mPairedCostume1 = 0xFF;
    u32 y = auctionWords(actor->current.pos).value[1];
    actor->mSetLinkFace = 0;
    auctionWords(actor->eyePos).value[1] = y;
    actor->mNextMsgNo = 0;
    actor->mBidMsgNo = 0;
    auctionWords(actor->eyePos).value[0] = x;
    auctionWords(actor->eyePos).value[2] = z;
    actor->mSelection = 0;
    actor->mActiveNpcs = 1;
    actor->mpEmitter = 0;
    actor->mMoveState = daAuction_c::MOVE_WAIT;
    actor->mEmitterFadeFlags = 0;
    return actor;
}

/* 02058E84 */
BOOL Auction_heapCallback(daAuction_c* actor) { return actor->createHeap(); }

/* 02059240 */
s32 daAuction_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            auction_daAuction_c_daAuction_c(this);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    s32 phase = auction_dComIfG_resLoad(mPhs, "Pspl");
    if (phase == cPhs_COMPLEATE_e) {
        if (!auction_fopAcM_entrySolidHeap(this, gabi::at<void>(0x02058E84), 0x2400))
            return cPhs_ERROR_e;
        return createInit();
    }
    return phase;
}

/* 02058DF0 */
BOOL daAuction_c::createHeap() {
    struct Name {
        be<u32> text, vtable;
    };
    gabi::Local<Name> archive;
    archive->vtable = 0x1000774C;
    u32 manager = l_resourceManager;
    archive->text = 0x10007784;
    u32 resource = auction_dRes_control_c_getIDRes(gabi::at<void>(manager), archive.get(), 0);
    if (!resource)
        return false;
    u32 model = auction_mDoExt_J3DModel__create(gabi::at<void>(resource), 0, 0x11020203);
    mpModel = model;
    return model != 0;
}

/* 020590FC */
s32 daAuction_c::createInit() {
    const char* names[5] = {"AUCTION_START", "AUCTION_GET_ITEM", "AUCTION_NO_ITEM",
                            "AUCTION_START2", "AUCTION_END2"};
    be<s16>* destinations[5] = {&mEvtStartIdx, &mEvtGetItemIdx, &mEvtNoItemIdx, &mEvtStart2Idx,
                                &mEvtEnd2Idx};
    for (u32 i = 0; i < 5; ++i) {
        u32 play = dComIfGp_ea();
        s32 event = auction_dEvent_manager_c_getEventIdx(&auctionGameInfo(play).play.mEvtManager,
                                                         names[i], 0xFF);
        *destinations[i] = event;
    }
    auction_dNpc_EventCut_c_setActorInfo(&mNpcEvtInfo, "Auction", this);
    u8 kind = mAucMdlNo[0];
    mCurrentNpc = 0;
    mTopBidder = 0xFF;
    s32 message = kind < 12 ? (s16)l_npc_msg_dat[(u32)kind].mNpcNameMsgID : 0;
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mNpcNameMessageID = message;
    mCurrAuctionItemIndex = 0;
    s16 startingBid = l_item_dat[0].mStartingBid;
    s16 name = l_item_dat[0].mNameMsgID;
    mCurrBid = startingBid;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mItemNameMessageID = (s32)name;
    s16 bid = mCurrBid;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mMsgCountNumber = bid;
    setMtx();
    return cPhs_COMPLEATE_e;
}

/* 020592EC */
BOOL daAuction_c::_delete() {
    if (auction_fopAcM_isCreating(this)) {
        u32 global = l_controllerManager;
        u32 controller = auctionControllers(global)->mpController.v;
        auction_enqueueScreenTask(&gabi::at<AuctionScreenController>(controller)->mQueue,
                                  &l_deleteScreenTask);
    }
    auction_dComIfG_resDelete(mPhs, "Pspl");
    if (heap.v) {
        u32 emitter = mpEmitter;
        if (emitter) {
            u32 flags = auctionEmitter(emitter)->mStatus;
            auctionEmitter(emitter)->mMaxFrame = 0xFFFFFFFF;
            auctionEmitter(emitter)->mStatus = flags | 1;
        }
    }
    return true;
}

/* 0205ACD8 */
BOOL daAuction_c::_draw() {
    u32 env = auction_dKy_getEnvlight();
    auction_dScnKy_env_light_c_settingTevStruct(gabi::at<void>(env), 3, &current.pos, &tevStr);
    env = auction_dKy_getEnvlight();
    u32 model = mpModel;
    auction_dScnKy_env_light_setLightTevColorType(gabi::at<void>(env), gabi::at<void>(model),
                                                  &tevStr);
    if (mFlags & 4) {
        model = mpModel;
        auction_mDoExt_modelUpdateDL(gabi::at<void>(model), 0);
    }
    return true;
}

/* 0205AAC8 */
BOOL daAuction_c::_execute() {
    checkOrder();
    u32 play = dComIfGp_ea();
    if (!auctionGameInfo(play).play.mEvtCtrl.mMode || eventInfo.mCommand == 1)
        (this->*moveProc[(u8)mMoveState].fn)();
    else
        eventMove();
    eventOrder();
    u32 emitter = mpEmitter;
    if (emitter) {
        u8 flags = mEmitterFadeFlags;
        f32 step = l_alphaStep;
        if (flags & 1) {
            s16 delay = mEmitterFadeDelay;
            if (delay)
                mEmitterFadeDelay = (s16)((u32)(s32)delay - 1);
            else {
                f32 alpha = mAlpha;
                f32 next = gabi::fadds_ppc(alpha, step);
                f32 limit = l_alphaMaximum;
                emitter = mpEmitter;
                if (next > limit) {
                    mAlpha = limit;
                    mEmitterFadeFlags = mEmitterFadeFlags & 0xFE;
                    next = limit;
                } else
                    mAlpha = next;
                auctionEmitter(emitter)->mGlobalAlpha = (u8)gabi::ftoi(next);
            }
        } else if (flags & 2) {
            f32 next = gabi::fsubs_ppc((f32)mAlpha, step);
            f32 zero = l_zero;
            mAlpha = next;
            emitter = mpEmitter;
            if (next < zero) {
                u32 status = auctionEmitter(emitter)->mStatus;
                auctionEmitter(emitter)->mMaxFrame = 0xFFFFFFFF;
                auctionEmitter(emitter)->mStatus = status | 1;
                mpEmitter = 0;
            } else
                auctionEmitter(emitter)->mGlobalAlpha = (u8)gabi::ftoi(next);
        }
    }
    setMtx();
    return true;
}

/* 0205AD44 */
void daAuction_c::executeWait() {
    for (u32 i = 0; i < 8; ++i)
        if (mNpcIds[i] == 0xFFFFFFFF)
            return;
    mMoveState = daAuction_c::MOVE_NORMAL;
}

/* 0205CFD0 */
void daAuction_c::executeNormal(void* context) {}

/* 0205CFD4 */
void daAuction_c::executeStart(void* context) {}

/* 02059380 */
void daAuction_c::checkOrder() {
    if (eventInfo.mCommand != 2)
        return;
    be<s16>* events[5] = {&mEvtStartIdx, &mEvtGetItemIdx, &mEvtNoItemIdx, &mEvtStart2Idx,
                          &mEvtEnd2Idx};
    for (u32 i = 0; i < 3; ++i) {
        s16 event = *events[i];
        u32 play = dComIfGp_ea();
        if (auction_dEvent_manager_c_startCheck(&auctionGameInfo(play).play.mEvtManager,
                                                (s32)event) &&
            mOrderEvent == (s8)(i + 3)) {
            mOrderEvent = 0;
            break;
        }
    }
    for (u32 i = 3; i < 5; ++i) {
        s16 event = *events[i];
        u32 play = dComIfGp_ea();
        if (auction_dEvent_manager_c_startCheck(&auctionGameInfo(play).play.mEvtManager,
                                                (s32)event) &&
            mOrderEvent == (s8)(i + 3)) {
            mOrderEvent = 0;
            break;
        }
    }
}

/* 0205A724 */
void daAuction_c::eventOrder() {
    if (mSelection == 1) {
        mOrderEvent = 3;
        mMoveState = daAuction_c::MOVE_START;
        u8 request = mSelection;
        mSelection = 0;
        mAuctionType = request;
    } else if (mSelection == 2) {
        u8 request = mSelection;
        mMoveState = daAuction_c::MOVE_START;
        mSelection = 0;
        mOrderEvent = 6;
        mAuctionType = request;
    }
    s8 order = mOrderEvent;
    if (order == 1 || order == 2) {
        s8 current = mOrderEvent;
        eventInfo.mCondition = eventInfo.mCondition | 1;
        if (current == 1)
            auction_fopAcM_orderSpeakEvent(this);
        return;
    }
    if (order == 3) {
        u32 play = l_saveInfo;
        if (auction_dSv_event_c_isEventBit_abi2(&auctionSave(play)->mSavedata.mEvent, 0x4008)) {
            play = l_saveInfo;
            u32 data = auction_dSv_info_getEvent(gabi::at<void>(play + 0x12C0));
            s16 bid = mCurrBid;
            u8 index = auctionSavedItem(data)->mAuctionItem;
            mCurrBid = bid + 10;
            mCurrAuctionItemIndex = index;
            s32 message = l_item_dat[index].mNameMsgID;
            play = dComIfGp_ea();
            auctionGameInfo(play).play.mItemNameMessageID = message;
        } else {
            u32 index = auction_daAuction_c_getItemNo(this);
            mCurrAuctionItemIndex = index;
            s32 message = l_item_dat[index].mNameMsgID;
            play = dComIfGp_ea();
            auctionGameInfo(play).play.mItemNameMessageID = message;
        }
        s16 bid = l_item_dat[(u8)mCurrAuctionItemIndex].mStartingBid;
        mCurrBid = bid;
        play = dComIfGp_ea();
        auctionGameInfo(play).play.mMsgCountNumber = bid;
        play = dComIfGp_ea();
        auction_fopAcM_orderChangeEventId(
            gabi::at<void>(auctionGameInfo(play).play.mPlayerInfo[0].mpPlayer.v), this,
            (s32)(s16)mEvtStartIdx, 0, 0xFF7F);
        play = dComIfGp_ea();
        u16 flags = auctionGameInfo(play).play.mMinigameFlags;
        auctionGameInfo(play).play.mMiniGameType = 5;
        auctionGameInfo(play).play.mMinigameFlags = flags | 0x10;
        return;
    }
    if (order == 4 || order == 5 || order == 7) {
        be<s16>* requestedEvent = order == 4   ? &mEvtGetItemIdx
                                  : order == 5 ? &mEvtNoItemIdx
                                               : &mEvtEnd2Idx;
        s32 event = *requestedEvent;
        auction_fopAcM_orderOtherEventId(this, event, 0xFF, 0xFF7F, 0, 1);
        return;
    }
    if (order == 6) {
        u8 index = (u8)mCurrAuctionItemIndex & 1;
        mCurrAuctionItemIndex = index;
        s16 bid = l_item_dat2[index].mStartingBid;
        s32 message = l_item_dat2[index].mNameMsgID;
        mCurrBid = bid;
        u32 play = dComIfGp_ea();
        auctionGameInfo(play).play.mItemNameMessageID = message;
        bid = mCurrBid;
        play = dComIfGp_ea();
        auctionGameInfo(play).play.mMsgCountNumber = bid;
        play = dComIfGp_ea();
        auction_fopAcM_orderChangeEventId(
            gabi::at<void>(auctionGameInfo(play).play.mPlayerInfo[0].mpPlayer.v), this,
            (s32)(s16)mEvtStart2Idx, 0, 0xFF7F);
        play = dComIfGp_ea();
        u16 flags = auctionGameInfo(play).play.mMinigameFlags;
        auctionGameInfo(play).play.mMiniGameType = 5;
        auctionGameInfo(play).play.mMinigameFlags = flags | 0x10;
    }
}

/* 0205A428 */
void daAuction_c::eventMove() {
    if (auctionEventEnded(this, &mEvtStartIdx)) {
        auctionEventFlag();
        mOrderEvent = mCurrentNpc ? 5 : 4;
        return;
    }
    bool ended = auctionEventEnded(this, &mEvtGetItemIdx);
    if (!ended)
        ended = auctionEventEnded(this, &mEvtNoItemIdx);
    if (!ended)
        ended = auctionEventEnded(this, &mEvtEnd2Idx);
    if (ended) {
        auctionEventFlag();
        u32 play = dComIfGp_ea();
        u16 flags = auctionGameInfo(play).play.mMinigameFlags;
        auctionGameInfo(play).play.mMinigameResult = 0;
        auctionGameInfo(play).play.mMiniGameType = 0;
        auctionGameInfo(play).play.mMinigameFlags = flags ^ 0x10;
        auction_fopAcM_delete((u32)mpTimer);
        u32 global = l_controllerManager, controller = auctionControllers(global)->mpController.v;
        auction_enqueueScreenTask(&gabi::at<AuctionScreenController>(controller)->mQueue,
                                  &l_deleteScreenTask);
        return;
    }
    if (auctionEventEnded(this, &mEvtStart2Idx)) {
        auctionEventFlag();
        mOrderEvent = 7;
        return;
    }
    u8 attention = mNpcEvtInfo.mbAttention;
    if (auction_dNpc_EventCut_c_cutProc(&mNpcEvtInfo)) {
        if (!mNpcEvtInfo.mbAttention)
            mNpcEvtInfo.mbAttention = attention;
    } else
        privateCut();
}

/* 0205A104 */
void daAuction_c::privateCut() {
    enum {
        ACT_MES_SET,
        ACT_MES_END,
        ACT_START,
        ACT_MAIN,
        ACT_GET_ITEM,
        ACT_CAMERA_OFF,
        ACT_GET_ITEM_NPC,
        ACT_GET_ITEM_MES,
        ACT_CAMERA_OFF_NPC,
        ACT_END,
        ACT_CAMERA_TEST
    };
    u32 play = dComIfGp_ea();
    s32 staff = auction_dEvent_manager_c_getMyStaffId(&auctionGameInfo(play).play.mEvtManager,
                                                      "Auction", 0, 0);
    if (staff == -1)
        return;
    play = dComIfGp_ea();
    s8 action = (s8)auction_dEvent_manager_c_getMyActIdx(&auctionGameInfo(play).play.mEvtManager,
                                                         staff, &l_cut_names, 11, 1, 0);
    mAction = action;
    play = dComIfGp_ea();
    if (action == -1) {
        auction_dEvent_manager_c_cutEnd(&auctionGameInfo(play).play.mEvtManager, staff);
        return;
    }
    if (auction_dEvent_manager_c_getIsAddvance(&auctionGameInfo(play).play.mEvtManager, staff)) {
        action = mAction;
        bool initialized = true;
        switch (action) {
        case ACT_MES_SET:
            eventTalkInit(staff);
            break;
        case ACT_START:
            eventStartInit();
            break;
        case ACT_MAIN:
            eventMainInit();
            break;
        case ACT_GET_ITEM:
            eventGetItemInit();
            break;
        case ACT_CAMERA_OFF:
            auction_offCamera(this);
            break;
        case ACT_GET_ITEM_NPC:
            eventGetItemNpcInit(staff);
            break;
        case ACT_GET_ITEM_MES:
            eventGetItemMesInit();
            break;
        case ACT_END:
            eventEndInit();
            break;
        default:
            initialized = false;
            break;
        }
        if (initialized)
            action = mAction;
    } else
        action = mAction;
    bool done = true;
    switch (action) {
    case ACT_MES_SET:
    case ACT_GET_ITEM_MES:
        done = eventMesSet();
        break;
    case ACT_MES_END:
        done = eventMesEnd();
        break;
    case ACT_START:
        done = eventStart();
        break;
    case ACT_MAIN:
        done = auction_daAuction_c_eventMain(this);
        break;
    case ACT_CAMERA_OFF_NPC:
        done = eventCameraOffNpc();
        break;
    case ACT_END:
        done = eventEnd();
        break;
    }
    if (done) {
        play = dComIfGp_ea();
        auction_dEvent_manager_c_cutEnd(&auctionGameInfo(play).play.mEvtManager, staff);
    }
    u32 camera = auction_dCam_getBody();
    u8 flags = mCameraFlags;
    if (flags & 1) {
        auction_dCamera_c_Stop(gabi::at<void>(camera));
        flags = ((u8)mCameraFlags & 0xFE) | 4;
        mCameraFlags = flags;
    }
    if (flags & 2) {
        auction_dCamera_c_Start(gabi::at<void>(camera));
        flags = (u8)mCameraFlags & 0xF1;
        mCameraFlags = flags;
    }
    if ((flags & 4) && (flags & 8)) {
        gabi::Local<cXyz> eye, center;
        center->z = mCameraEye.z;
        eye->x = mCameraCenter.x;
        eye->y = mCameraCenter.y;
        center->x = mCameraEye.x;
        eye->z = mCameraCenter.z;
        center->y = mCameraEye.y;
        auction_dCamera_c_Set(gabi::at<void>(camera), eye.get(), center.get());
    }
}

/* 020594BC */
void daAuction_c::eventTalkInit(s32 staff) {
    u32 play = dComIfGp_ea();
    u32 data = auction_dEvent_manager_c_getMySubstanceP(&auctionGameInfo(play).play.mEvtManager,
                                                        staff, "MsgNo", 3);
    u32 message = 0;
    if (data) {
        message = *auctionInteger(data);
        if (message == 0x1CF4) {
            auction_mDoAud_seStart(0x8DC);
            message = *auctionInteger(data);
        }
    }
    auction_daAuction_c_setMessage_abi2(this, message);
}

/* 02059C64 */
BOOL daAuction_c::eventMesSet() {
    auction_fopNpc_npc_c_talk(this, 0);
    return mCurrMsgBsPcId != 0xFFFFFFFF;
}

/* 02059CA8 */
BOOL daAuction_c::eventMesEnd() { return auction_fopNpc_npc_c_talk_abi2(this, 0) == 0x12; }

/* 02059544 */
void daAuction_c::eventStartInit() {
    u8 type = mAuctionType;
    f32 y = l_timerY, x = l_timerX, w = l_timerWidth;
    mActiveNpcs = 0xFE;
    f32 h = l_timerHeight;
    s32 timer = auction_dTimer_createTimer(4, type == 2 ? 0x1E : 0x3C, 1, 0, x, y, w, h);
    mTimerID = timer;
    if (timer == -1)
        auction_JUTAssertion_showAssert("d_a_auction.cpp", 0x87B,
                                        "mTimerID != fpcM_ERROR_PROCESS_ID_e");
    u32 global = l_controllerManager, controller = auctionControllers(global)->mpController.v;
    auction_enqueueScreenTask(&gabi::at<AuctionScreenController>(controller)->mQueue,
                              &l_createScreenTask);
    // HD: the auction screen is created through a queued task instead of a gauge process.
    mpTimer = 0;
    mTauntShown = 1;
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mMiniGameRupee = 0;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mAuctionRupee = 0;
    auction_dAuction_screen_slotHide();
    auction_dAuction_screen_gaugeHide_abi2();
    auction_dAuction_screen_talkEnd();
    u8 index = mCurrAuctionItemIndex;
    s16 item = l_item_dat[(u32)index].mItemID;
    u32 itemID = auction_fopAcM_create(0x103, (s32)item, &current.pos, -1, 0, 0, -1, 0);
    f32 one = l_one;
    mCurrAuctionItemPID = itemID;
    auction_dKy_custom_colset(0, 4, one);
    f32 z = l_lightZ, power = l_lightPower;
    mLight.mPos.z = z;
    mLight.mPower = power;
    f32 lightY = l_lightY, zero = l_zero;
    mLight.mPos.y = lightY;
    mLight.mPos.x = zero;
    mLight.mColor.b = 0x3C;
    mLight.mFluctuation = zero;
    mLight.mColor.r = 0x96;
    mLight.mColor.g = 0x64;
    auction_dKy_efplight_set(&mLight);
    mFlags = mFlags | 4;
    auction_dAuction_screen_talkEnd();
}

/* 02059CD8 */
BOOL daAuction_c::eventStart() {
    if (!mpTimer) {
        u32 gauge = auction_fopMsgM_SearchByID((u32)mTimerID);
        mpTimer = gauge;
        if (gauge)
            auction_dTimer_setShowType(gabi::at<void>(gauge), 0);
    }
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mDoStatusForce = 0x3E;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mAStatusForce = 0x3E;
    return mpTimer != 0;
}

/* 020596F0 */
void daAuction_c::eventMainInit() {
    f32 zero = l_zero;
    m804 = 0xE10;
    for (u32 i = 0; i < 7; ++i)
        mBidGauge[i] = zero;
    u8 type = mAuctionType;
    mRestMsgIdx = 0;
    mAuctionEnded = 0;
    mGestureFlags = 0;
    mFirstBidMade = 0;
    mMainState = type == 2;
    f32 one = l_one;
    mCameraFlags = 0;
    m7E0 = zero;
    m7E4 = one;
    u32 play = dComIfGp_ea();
    if (auctionGameInfo(play).play.mTimerInfo.mTimerMode == 4) {
        play = dComIfGp_ea();
        u32 timer = auctionGameInfo(play).play.mTimerInfo.mTimerPtr;
        if (timer)
            auction_dTimer_start(gabi::at<void>(timer));
    }
    if (mAuctionType == 1)
        auction_dAuction_screen_gaugeShow();
    mEmitterFadeFlags = mEmitterFadeFlags | 1;
    play = dComIfGp_ea();
    u32 link = auctionGameInfo(play).play.mpPlayerPtr[0].v;
    auctionPlayer(link)->mDemo.mDemoType = 3;
    auctionPlayer(link)->mDemo.mParam0 = 0;
    mCurLinkAnm = 1;
    play = dComIfGp_ea();
    auto* eventManager = &auctionGameInfo(play).play.mEvtCtrl;
    u32 index = auction_dEvt_control_c_getPId(eventManager, this);
    auctionGameInfo(play).play.mEvtCtrl.mPtTalk = index;
    mCameraPosIdx = 0;
    for (u32 i = 0; i < 6; ++i)
        auctionCameraWords(mCameraCenter).value[i] =
            auctionCameraWords(l_camera_pos.positions[0][0]).value[i];
    auction_dAuction_screen_talkEnd();
}

/* 02059DB4 */
bool daAuction_c::eventMain() {
    u32 play = dComIfGp_ea();
    u32 link = auctionGameInfo(play).play.mpPlayerPtr[0].v;
    u32 vt = auctionPlayer(link)->__vtbl;
    f32 rate =
        gabi::call_ptr<f32>(auctionPlayerVtable(vt)->getBaseAnimeFrameRate, auctionPlayer(link));
    if (rate == l_zero) {
        u8 animation = mCurLinkAnm;
        if (animation != 1 && animation != 29 && animation != 37)
            setLinkAnm(1);
    }
    mFlags = (u8)mFlags & 4;
    play = dComIfGp_ea();
    if (auctionGameInfo(play).play.mTimerInfo.mTimerMode == 4) {
        play = dComIfGp_ea();
        u32 timer = auctionGameInfo(play).play.mTimerInfo.mTimerPtr;
        if (timer)
            auction_dTimer_stop(gabi::at<void>(timer), 2);
    }
    (this->*eventProc[(u8)mMainState].fn)();
    if (mSetLinkFace)
        auctionPlayer(link)->mFace = (u8)mFace;
    if (mAuctionType == 1) {
        s16 wait = (s16)gabi::ftoi(mBidGauge[0]);
        play = dComIfGp_ea();
        auctionGameInfo(play).play.mAuctionGauge = wait;
    }
    if (mMainState > MAIN_URI || !mAuctionEnded)
        return false;
    u8 kind = mAucMdlNo[(u8)mCurrentNpc];
    if (kind >= 12) {
        auction_JUTAssertion_showAssert_abi2(gabi::at<void>(0x100078D8), 0x95A,
                                             gabi::at<void>(0x100078E8));
        kind = 0;
    }
    s32 name = l_npc_msg_dat[kind].mNpcNameMsgID;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mNpcNameMessageID = name;
    play = dComIfGp_ea();
    if (auctionGameInfo(play).play.mTimerInfo.mTimerMode == 4) {
        play = dComIfGp_ea();
        u32 timer = auctionGameInfo(play).play.mTimerInfo.mTimerPtr;
        if (timer)
            auction_dTimer_stop(gabi::at<void>(timer), 2);
    }
    auction_dAuction_screen_slotHide();
    auction_dAuction_screen_gaugeHide_abi2();
    u8 flags = mEmitterFadeFlags;
    mAuctionEnded = 1;
    mGestureFlags = 0;
    mEmitterFadeFlags = flags | 2;
    auctionPlayer(link)->mDemo.mDemoType = 2;
    auctionPlayer(link)->mDemo.mDemoMode = 1;
    u8 camera = mCameraFlags;
    if (camera & 4)
        mCameraFlags = (camera | 2) & 0xFE;
    auction_mDoAud_seStart(0x8DE);
    s16 bid = mCurrBid;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mMsgCountNumber = bid;
    return true;
}

/* 0205AF48 */
void daAuction_c::eventMainKai() {
    eventRestartTimer();
    if (eventTimeRemaining() <= 0)
        mAuctionEnded = 1;
    else {
        s32 cancel = auction_checkTriggerB(0);
        f32 zero = l_zero;
        f32 limit = l_gaugeMaximum;
        if (cancel) {
            setMessage2(0x1CFC);
            auction_mDoAud_seStart(0x8E6);
        } else {
            f32 speed = m7E0;
            f32 increment = m7E4;
            f32 next = speed + increment;
            f32 one = l_one;
            f32 negative = l_negativeOne;
            if (!(next < one)) {
                m7E0 = one;
                m7E4 = negative;
            } else if (next > negative)
                m7E0 = next;
            else {
                m7E0 = negative;
                m7E4 = one;
            }
            f32 gauge = mBidGauge[0];
            if (gauge > zero && gauge < limit)
                mBidGauge[0] = gauge + l_gaugeIdleStep;
            if (auction_checkTriggerA(0)) {
                f32 low = l_npc_dat[0].mGaugeIncMin;
                f32 high = l_npc_dat[0].mGaugeIncMax;
                f32 random = cM_rndF(high - low);
                low = l_npc_dat[0].mGaugeIncMin;
                gauge = mBidGauge[0];
                gauge = gauge + (random + low);
                f32 unsignedBoundary = l_unsignedFloatBoundary;
                mBidGauge[0] = gauge;
                u32 soundParameter =
                    gauge < unsignedBoundary
                        ? (u32)gabi::ftoi(gauge)
                        : (u32)gabi::ftoi(gauge - l_unsignedFloatBoundary) + 0x80000000;
                auction_mDoAud_seStartWithParam(0x8E4, 0, soundParameter);
            }
            if (mBidGauge[0] > limit)
                mBidGauge[0] = limit;
            s16 wait = mBidWaitTimer;
            if (wait) {
                s16 nextWait = (s16)((u16)wait - 1);
                mBidWaitTimer = nextWait;
                if (!nextWait) {
                    u8 flags = mGestureFlags;
                    if (flags & 2) {
                        u32 camera = getRand(3) & 0xFF;
                        mCameraPosIdx = camera;
                        auto& source = auctionCameraWords(l_camera_pos.positions[camera][0]);
                        for (u32 i = 0; i < 3; ++i)
                            auctionCameraWords(mCameraCenter).value[i] = source.value[i];
                        flags = mGestureFlags;
                        for (u32 i = 3; i < 6; ++i)
                            auctionCameraWords(mCameraCenter).value[i] = source.value[i];
                    }
                    mGestureFlags = flags & 0xF9;
                }
            } else
                for (u32 i = 1; i < 7; ++i) {
                    auto* row = &l_npc_dat[i];
                    f32 low = row->mGaugeIncMin;
                    f32 high = row->mGaugeIncMax;
                    f32 random = cM_rndF(high - low);
                    low = row->mGaugeIncMin;
                    f32 addition = random + low;
                    mBidGauge[i] = (f32)mBidGauge[i] + addition;
                }
            u32 end = (mGestureFlags & 2) ? 1 : 7;
            u32 bidder = 0;
            for (; bidder < end; ++bidder)
                if (!(mBidGauge[bidder] < limit) && (s16)mCurrBid < l_npc_dat[bidder].mSpendingLimit)
                    break;
            if (bidder < end) {
                s16 previous = mCurrBid;
                mNewBidder = bidder;
                u32 message;
                if (bidder) {
                    f32 multiplier = one;
                    if (eventTimeRemaining() < 30000)
                        multiplier = l_lateBidMultiplier;
                    else if (eventTimeRemaining() < 60000)
                        multiplier = l_midBidMultiplier;
                    auto* row = &l_npc_dat[bidder];
                    s16 low = row->mBidIncMin;
                    s16 high = row->mBidIncMax;
                    f32 random = cM_rndF((f32)((s32)high - low));
                    low = row->mBidIncMin;
                    s16 amount = (s16)gabi::ftoi(random + (f32)low);
                    s16 increase = (s16)gabi::ftoi((f32)amount * multiplier);
                    s16 current = mCurrBid;
                    mAfterBetIdx = 4;
                    s16 total = (s16)((u16)current + (u16)increase);
                    mCurrBid = total;
                    mDisplayBid = total;
                    mBidGauge[bidder] = zero;
                    nextBet();
                    message = 0x1CF9;
                } else {
                    s16 current = mCurrBid;
                    mDisplayBid = (s16)((u16)previous + 1);
                    u32 play = dComIfGp_ea();
                    auctionGameInfo(play).play.mMsgSetNumber = (s16)((u16)current + 1);
                    setLinkAnm(72);
                    auction_dTimer_setShowType_abi2((u32)mpTimer, 1);
                    auction_dAuction_screen_gaugeUp();
                    play = dComIfGp_ea();
                    gabi::Local<cXyz> direction;
                    direction->x = zero;
                    direction->y = one;
                    direction->z = zero;
                    auction_dVibration_c_StartShock(play + 0x599C, 5, 1, direction.get());
                    play = dComIfGp_ea();
                    direction->x = zero;
                    direction->y = one;
                    direction->z = zero;
                    auction_dVibration_c_StartShock(play + 0x599C, 4, 0x3E, direction.get());
                    message = 0x1CFA;
                }
                f32 random = cM_rndF(zero);
                f32 frames = (random + one) * l_framesPerSecond;
                u8 shown = mTauntShown;
                mMainState = daAuction_c::MAIN_BIKON_C;
                mBidMsgNo = message;
                mBidWaitTimer = (s16)gabi::ftoi(frames);
                if (shown)
                    mTauntShown = 0;
                else if (mTopBidder != 0xFF && bidder && eventTimeRemaining() > l_rest_msg_time1[0] &&
                         !getRand(3)) {
                    s16 bid = mCurrBid;
                    message = bid <= 100   ? 0x1CFD
                              : bid <= 150 ? 0x1CFE
                              : bid <= 200 ? 0x1CFF
                                           : 0x1D00;
                    u32 play = dComIfGp_ea();
                    auctionGameInfo(play).play.mMsgCountNumber = previous;
                    u32 kind = mAucMdlNo[(u8)mCurrentNpc];
                    if (kind >= 12) {
                        auction_JUTAssertion_showAssert(
                            "d_a_auction.cpp", 0x9FA,
                            "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                        kind = 0;
                    }
                    s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
                    play = dComIfGp_ea();
                    auctionGameInfo(play).play.mNpcNameMessageID = name;
                    setMessage2(message);
                    setCameraNpc((u32)mCurrentNpc, 0);
                    if (!mCurrentNpc && mCurLinkAnm != 29)
                        setLinkAnm(20);
                    mGestureFlags = mGestureFlags | 0x20;
                    mSpeakingNpc = (u8)mResult;
                    mTauntShown = 1;
                }
                if (mMainState == daAuction_c::MAIN_BIKON_C) {
                    u8 selected = mNewBidder;
                    s16 bid = mDisplayBid;
                    mSpeakingNpc = selected;
                    mResult = selected;
                    u32 play = dComIfGp_ea();
                    auctionGameInfo(play).play.mMsgCountNumber = bid;
                    u32 kind = mAucMdlNo[(u8)mResult];
                    if (kind >= 12) {
                        auction_JUTAssertion_showAssert(
                            "d_a_auction.cpp", 0xA13,
                            "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                        kind = 0;
                    }
                    s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
                    play = dComIfGp_ea();
                    auctionGameInfo(play).play.mNpcNameMessageID = name;
                }
                mCameraFlags = (mCameraFlags | 9) & 0xFD;
            } else {
                s32 remaining = eventTimeRemaining();
                u8 next = mRestMsgIdx;
                if (remaining < l_rest_msg_time1[next]) {
                    setMessage2((u32)l_rest_msg_no[next]);
                    next = mRestMsgIdx;
                    auction_mDoAud_seStart_abi2((u32)l_rest_se_no[next]);
                    mRestMsgIdx = (u8)mRestMsgIdx + 1;
                }
            }
        }
        if (mGestureFlags & 2) {
            mCameraEye.x = zero;
            mCameraEye.y = l_lightY;
            mCameraEye.z = l_paralyzedCameraZ;
            mGestureFlags = mGestureFlags | 4;
            u32 npc = auction_daAuction_c_getNpcActorP(this, 0);
            for (u32 i = 0; i < 3; ++i)
                auctionCameraWords(mCameraCenter).value[i] =
                    auctionWords(auctionActor(npc)->current.pos).value[i];
            // Native sine-table indexing and remaining effect reviewed before baseline.
            s16 angle = mCameraSwayAngle;
            f32 sine = cM_ssin(angle);
            mCameraCenter.x = gabi::fmadds(sine, l_paralyzedCameraRadius, (f32)mCameraCenter.x);
            mCameraCenter.y = (f32)mCameraCenter.y + limit;
            mCameraSwayAngle = (s16)((u16)mCameraSwayAngle + 200);
            auction_mDoAud_seStart(0x5120);
        }
    }
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mDoStatusForce = 0x25;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mAStatusForce = 0x27;
}

/* 0205B9C0 */
void daAuction_c::eventMainUri() {
    eventRestartTimer();
    if (eventTimeRemaining() <= 0)
        mAuctionEnded = 1;
    else if (mFirstBidMade && auction_checkTriggerA(0))
        setMessage2(0x1D1A);
    else {
        f32 doubleRate = l_lateBidMultiplier;
        s16 wait = mBidWaitTimer;
        if (wait)
            mBidWaitTimer = (s16)((u16)wait - 1);
        else
            for (u32 i = 1; i < 7; ++i) {
                auto* row = &l_npc_dat[i];
                f32 low = row->mGaugeIncMin;
                f32 high = row->mGaugeIncMax;
                f32 random = cM_rndF(high - low);
                low = row->mGaugeIncMin;
                f32 speed = random + low;
                f32 value = mBidGauge[i];
                mBidGauge[i] = value + (speed + speed);
            }
        f32 limit = l_gaugeMaximum;
        f32 zero = l_zero;
        u32 bidder = 1;
        for (; bidder < 7; ++bidder) {
            if (!(mBidGauge[bidder] < limit)) {
                mBidGauge[bidder] = zero;
                if ((s16)mCurrBid < l_npc_dat[bidder].mSpendingLimit)
                    break;
            }
        }
        if (bidder < 7) {
            mNewBidder = bidder;
            f32 unit = l_one;
            f32 multiplier = unit;
            if (eventTimeRemaining() < 30000)
                multiplier = doubleRate;
            else if (eventTimeRemaining() < 60000)
                multiplier = l_midBidMultiplier;
            auto* row = &l_npc_dat[bidder];
            s16 low = row->mBidIncMin;
            s16 high = row->mBidIncMax;
            f32 random = cM_rndF((f32)((s32)high - low));
            low = row->mBidIncMin;
            s16 base = (s16)gabi::ftoi(random + (f32)low);
            f32 amount = (f32)((s32)base * 2) * multiplier;
            mAfterBetIdx = 4;
            mFirstBidMade = 1;
            mBidMsgNo = 0x1CF9;
            s16 old = mCurrBid;
            s16 increment = (s16)gabi::ftoi(amount);
            s16 total = (s16)((u16)old + (u16)increment);
            mCurrBid = total;
            mDisplayBid = total;
            random = cM_rndF(unit);
            f32 duration = random * l_framesPerSecond;
            mMainState = daAuction_c::MAIN_BIKON_C;
            u8 shown = mTauntShown;
            mBidWaitTimer = (s16)gabi::ftoi(duration);
            if (shown)
                mTauntShown = 0;
            else if (eventTimeRemaining() > l_rest_msg_time1[0] && !getRand(3)) {
                s16 bid = mCurrBid;
                u32 message = bid <= 100   ? 0x1CFD
                              : bid <= 150 ? 0x1CFE
                              : bid <= 200 ? 0x1CFF
                                           : 0x1D00;
                u32 kind = mAucMdlNo[(u8)mCurrentNpc];
                if (kind >= 12) {
                    auction_JUTAssertion_showAssert(
                        "d_a_auction.cpp", 0xAB3,
                        "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                    kind = 0;
                }
                s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
                u32 play = dComIfGp_ea();
                auctionGameInfo(play).play.mNpcNameMessageID = name;
                setMessage2(message);
                setCameraNpc((u32)mCurrentNpc, 0);
                if (!mCurrentNpc)
                    setLinkAnm(20);
                u8 flags = mGestureFlags;
                u8 camera = mResult;
                mGestureFlags = flags | 0x20;
                mSpeakingNpc = camera;
                mTauntShown = 1;
            }
            if (mMainState == daAuction_c::MAIN_BIKON_C) {
                u8 selected = mNewBidder;
                s16 bid = mDisplayBid;
                mSpeakingNpc = selected;
                mResult = selected;
                u32 play = dComIfGp_ea();
                auctionGameInfo(play).play.mMsgCountNumber = bid;
                u32 kind = mAucMdlNo[(u8)mResult];
                if (kind >= 12) {
                    auction_JUTAssertion_showAssert(
                        "d_a_auction.cpp", 0xACC,
                        "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                    kind = 0;
                }
                s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
                play = dComIfGp_ea();
                auctionGameInfo(play).play.mNpcNameMessageID = name;
            }
            mCameraFlags = (mCameraFlags | 9) & 0xFD;
        } else {
            s32 remaining = eventTimeRemaining();
            u8 next = mRestMsgIdx;
            if (remaining < l_rest_msg_time2[next]) {
                setMessage2((u32)l_rest_msg_no[next]);
                next = mRestMsgIdx;
                auction_mDoAud_seStart_abi2((u32)l_rest_se_no[next]);
                mRestMsgIdx = (u8)mRestMsgIdx + 1;
            }
        }
    }
    if (mFirstBidMade) {
        u32 play = dComIfGp_ea();
        auctionGameInfo(play).play.mDoStatusForce = 0x25;
    }
}

/* 0205BFD8 */
void daAuction_c::eventMainMsgSet() {
    if (eventMesSet())
        mMainState = daAuction_c::MAIN_MSG_END;
}

/* 0205C014 */
void daAuction_c::eventMainMsgEnd() {
    if (eventMesEnd()) {
        u8 animation = mCurLinkAnm;
        if (animation != 1 && animation != 29)
            setLinkAnm(1);
        u8 flags = mGestureFlags;
        if (flags & 0x20) {
            mMainState = daAuction_c::MAIN_BIKON_C;
            m7E0 = l_zero;
        } else {
            // Integer loads preserve vector bits, including signaling NaNs.
            u32 y = auctionWords(current.pos).value[1];
            u8 mode = mAuctionType;
            auctionWords(eyePos).value[1] = y;
            u32 x = auctionWords(current.pos).value[0];
            flags = mGestureFlags;
            auctionWords(eyePos).value[0] = x;
            u32 z = auctionWords(current.pos).value[2];
            mMainState = mode == 2;
            auctionWords(eyePos).value[2] = z;
            mSetLinkFace = 0;
            m7E0 = l_zero;
        }
        if (!(flags & 2)) {
            u32 camera = getRand(3) & 0xFF;
            mCameraPosIdx = camera;
            auto& source = auctionCameraWords(l_camera_pos.positions[camera][0]);
            for (u32 i = 0; i < 6; ++i)
                auctionCameraWords(mCameraCenter).value[i] = source.value[i];
        }
        auction_dTimer_setShowType_abi2((u32)mpTimer, 0);
        auction_dAuction_screen_gaugeDown();
        mGestureFlags = mGestureFlags & 0xD6;
        auction_dAuction_screen_talkEnd();
        auction_dAuction_screen_gaugeShow();
        return;
    }
    u32 play = dComIfGp_ea();
    if (auctionGameInfo(play).play.mMesgSendButton) {
        u32 message = mNextMsgNo;
        if (message - 0x1D3Au < 0x12 || message - 0x339Du < 0xC) {
            setCameraNpc((u32)mReplyNpc, 0);
            u8 flags = mGestureFlags;
            u8 camera = mReplyNpc;
            mGestureFlags = flags | 8;
            mSpeakingNpc = camera;
        }
    }
}

/* 0205C248 */
void daAuction_c::eventMainMsgBikonC() {
    u8 bidder = mNewBidder;
    s16 bid = mDisplayBid;
    mResult = bidder;
    mSpeakingNpc = bidder;
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mMsgCountNumber = bid;
    u32 kind = mAucMdlNo[(u8)mResult];
    if (kind >= 12) {
        auction_JUTAssertion_showAssert(
            "d_a_auction.cpp", 0xB5D,
            "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
        kind = 0;
    }
    s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mNpcNameMessageID = name;
    u32 npc = auction_daAuction_c_getNpcActorP_abi2(this, (u32)mResult);
    gabi::Local<cXyz> position;
    position->x = auctionActor(npc)->current.pos.x;
    f32 y = auctionActor(npc)->current.pos.y;
    position->y = y;
    u8 index = mResult;
    position->z = auctionActor(npc)->current.pos.z;
    f32 height = getPiconDispOfs((u32)index);
    f32 offset = height + l_piconCenterOffset;
    f32 scale = l_cameraBlendScale;
    f32 maximum = l_cameraBlendMaximum;
    f32 minimum = l_cameraBlendMinimum;
    position->y = y + offset;
    f32 distance = auction_cLib_addCalcPos(&mCameraCenter, position.get(), scale, maximum, minimum);
    if (distance == l_zero) {
        mMainState = daAuction_c::MAIN_BIKON_W;
        npc = auction_daAuction_c_getNpcActorP_abi2(this, (u32)mResult);
        for (u32 i = 0; i < 3; ++i)
            auctionWords(mBikonPos).value[i] = auctionWords(auctionActor(npc)->current.pos).value[i];
        height = getPiconDispOfs((u32)mResult);
        f32 itemY = mBikonPos.y;
        s8 room = current.roomNo;
        mBikonPos.y = itemY + height;
        play = dComIfGp_ea();
        u32 particles = auctionGameInfo(play).play.mParticle.v;
        auction_dPa_control_c_set(particles, 0, 0x8153, &mBikonPos, 0, 0, 0xFF, 0, (s32)room, 0, 0, 0);
        mTimer = 30;
        if (!mResult)
            auction_mDoAud_seStart(0x8EB);
        else {
            auction_mDoAud_seStart(0x591F);
            mSetLinkFace = 1;
            u32 face = getRand(8);
            u8 npcIndex = mResult;
            mFace = l_link_face[face];
            npc = auction_daAuction_c_getNpcActorP_abi2(this, (u32)npcIndex);
            for (u32 i = 0; i < 3; ++i)
                auctionWords(eyePos).value[i] = auctionWords(auctionActor(npc)->eyePos).value[i];
            play = dComIfGp_ea();
            auctionGameInfo(play).play.mDoStatusForce = 0;
            play = dComIfGp_ea();
            auctionGameInfo(play).play.mAStatusForce = 0x3E;
            return;
        }
        npc = auction_daAuction_c_getNpcActorP_abi2(this, (u32)mResult);
        for (u32 i = 0; i < 3; ++i)
            auctionWords(eyePos).value[i] = auctionWords(auctionActor(npc)->eyePos).value[i];
    }
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mDoStatusForce = 0;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mAStatusForce = 0x3E;
}

/* 0205C4B4 */
void daAuction_c::eventMainMsgBikonW() {
    s16 timer = mTimer;
    if (timer)
        mTimer = (s16)((u16)timer - 1);
    else {
        setMessage2((s32)mBidMsgNo);
        setCameraNpc((u32)mResult, 0);
        auction_dAuction_screen_talkStart();
        if (mResult)
            mGestureFlags = mGestureFlags | 1;
    }
    u32 play = dComIfGp_ea();
    auctionGameInfo(play).play.mDoStatusForce = 0;
    play = dComIfGp_ea();
    auctionGameInfo(play).play.mAStatusForce = 0x3E;
}

/* 02059A44 */
void daAuction_c::eventGetItemInit() {
    u8 type = mAuctionType, index = mCurrAuctionItemIndex;
    u32 itemAddress = type == 2 ? gabi::ea(l_item_dat22) + (u32)index * 2 : gabi::ea(l_item_dat) + (u32)index * 8;
    u8 item = auctionItemLowByte(itemAddress);
    s32 id = auction_fopAcM_createItemForPresentDemo(&current.pos, item, 0, -1, -1, 0, 0);
    if (id != -1) {
        u32 play = dComIfGp_ea();
        auctionGameInfo(play).play.mEvtCtrl.mPtItem = id;
    }
    index = mCurrAuctionItemIndex;
    itemAddress = (type == 2 ? gabi::ea(l_item_dat2) : gabi::ea(l_item_dat)) + (u32)index * 8;
    auction_obtainItem(gabi::at<AuctionItem>(itemAddress));
    setCameraNpc(0, 0);
}

/* 02059B40 */
void daAuction_c::offCamera() { mCameraFlags = (mCameraFlags | 2) & 0xFE; }

/* 02059B54 */
void daAuction_c::eventGetItemNpcInit(s32 staff) {
    u8 npc = mCurrentNpc;
    setCameraNpc(npc, 0);
    u32 play = dComIfGp_ea();
    u32 data = auction_dEvent_manager_c_getMySubstanceP(&auctionGameInfo(play).play.mEvtManager,
                                                        staff, "Timer", 3);
    s16 timer = 0;
    npc = mCurrentNpc;
    if (data)
        timer = auctionIntegerWords(data)->low;
    mSpeakingNpc = npc;
    mTimer = timer;
    mGestureFlags = 0x10;
}

/* 02059BD4 */
void daAuction_c::eventGetItemMesInit() {
    u8 npc = mCurrentNpc;
    u32 kind = mAucMdlNo[npc];
    if (kind >= 12) {
        auction_JUTAssertion_showAssert(
            "d_a_auction.cpp", 0xBF6,
            "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
        kind = 0;
    }
    s16 message = l_npc_msg_dat[kind].mWinMsgNo;
    auction_daAuction_c_setMessage_abi3(this, (s32)message);
}

/* 0205A010 */
BOOL daAuction_c::eventCameraOffNpc() {
    if (!eventMesEnd())
        return false;
    u8 flags = mCameraFlags;
    mGestureFlags = 0;
    mCameraFlags = (flags | 2) & 0xFE;
    return true;
}

/* 02059C44 */
void daAuction_c::eventEndInit() {
    u8 flags = mFlags;
    f32 blend = l_zero;
    mFlags = flags & 0xFB;
    mBlend = blend;
    auction_dKy_efplight_cut(&mLight);
}

/* 0205A078 */
BOOL daAuction_c::eventEnd() {
    f32 blend = mBlend;
    f32 step = l_blendStep;
    f32 next = gabi::fadds_ppc(blend, step);
    f32 limit = l_one;
    if (next > limit)
        next = limit;
    mBlend = next;
    auction_dKy_custom_colset(4, 0, next);
    return mBlend == limit;
}

/* 0205C55C */
u16 daAuction_c::next_msgStatus(be<u32>* message) {
    f32 zero = l_zero;
    u32 number = *message;
    // HD: loads the message singleton before dispatch, even on unrelated cases.
    u32 manager = l_messageManager;
    u16 status = 15;
    switch (number) {
    case 0x1CF2:
        *message = 0x1CF3;
        break;
    case 0x1CF4:
        if (mAuctionType == 2)
            *message = 0x1D1B;
        else {
            u32 save = l_saveInfo;
            if (auction_dSv_event_c_isEventBit_abi3(save + 0x644, 0x4008)) {
                save = l_saveInfo;
                u32 kind = auction_dSv_event_c_getEventReg(save + 0x644, 0x790F) & 0xFF;
                if (kind >= 12) {
                    auction_JUTAssertion_showAssert(
                        "d_a_auction.cpp", 0xC7E,
                        "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                    kind = 0;
                }
                s16 name = l_npc_msg_dat[kind].mNpcNameMsgID;
                u32 play = dComIfGp_ea();
                auctionGameInfo(play).play.mNpcNameMessageID = name;
                *message = 0x1CF6;
            } else
                *message = 0x1CF5;
        }
        break;
    case 0x1CF7:
    case 0x1CF9: {
        s16 bid = mCurrBid;
        u32 play = dComIfGp_ea();
        auctionGameInfo(play).play.mMiniGameRupee = bid;
        bid = mCurrBid;
        play = dComIfGp_ea();
        auctionGameInfo(play).play.mAuctionRupee = bid;
        if (number == 0x1CF7) {
            auction_dAuction_screen_slotShow();
            *message = 0x1CF8;
            break;
        }
        u32 after = mAfterBetIdx;
        if (after < 4)
            *message = l_after_bet_msg_no[after];
        else {
            u32 kind = mAucMdlNo[(u8)mCurrentNpc];
            if (kind >= 12) {
                auction_JUTAssertion_showAssert(
                    "d_a_auction.cpp", 0xC9B,
                    "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
                kind = 0;
            }
            if (!mResult && l_npc_msg_dat[kind].mOutbidMsgNo) {
                auto* row = &l_npc_msg_dat[kind];
                // HD: threshold is 10000ms, unlike the GC source's 60000ms.
                u32 offset = eventTimeRemaining() < 10000 ? 4 : 2;
                *message = (s32)(offset == 4 ? row->mOutbidLateMsgNo : row->mOutbidMsgNo);
            } else
                status = 16;
        }
        u8 previous = mCurrentNpc;
        u8 selected = mResult;
        mReplyNpc = previous;
        mTopBidder = selected;
        mCurrentNpc = selected;
        if (selected)
            setLinkAnm(1);
        break;
    }
    case 0x1CFA: {
        u32 play = dComIfGp_ea();
        u32 save = l_saveInfo;
        s16 bid = auctionGameInfo(play).play.mMsgSetNumber;
        u16 rupees = auctionSave(save)->mSavedata.mPlayer.mPlayerStatusA.mRupee;
        f32 half = l_lightY;
        if ((s32)rupees < bid || bid <= (s16)mCurrBid) {
            bool insufficient = (s32)rupees < bid;
            auction_mDoAud_seStart(0x8E5);
            setLinkAnm(74);
            *message = insufficient ? 0x1CFB : 0x1D1E;
            mBidGauge[0] = half;
        } else {
            auction_mDoAud_seStart(0x8E3);
            mBidGauge[0] = zero;
            if (bid == 999) {
                *message = 0x1D24;
                u8 previous = mCurrentNpc;
                u8 selected = mResult;
                mReplyNpc = previous;
                mTopBidder = selected;
                mCurrBid = bid;
                mCurrentNpc = selected;
            } else {
                f32 frames = l_framesPerSecond;
                *message = 0x1CF9;
                mAfterBetIdx = 0;
                for (u32 i = 0; i < 4; ++i) {
                    f32 old = (f32)(s16)mCurrBid;
                    f32 rate = l_after_bet_rate[i];
                    s16 minimum = (s16)gabi::ftoi(old * rate);
                    if (bid >= minimum && (s16)mCurrBid >= l_after_bet_chk[i]) {
                        auto* row = &l_after_bet_wait[i];
                        s16 low = row->minimum;
                        s16 high = row->maximum;
                        f32 random = cM_rndF((f32)((s32)high - low));
                        u32 selected = mAfterBetIdx;
                        low = l_after_bet_wait[selected].minimum;
                        mBidWaitTimer = (s16)gabi::ftoi((random + (f32)low) * frames);
                        break;
                    }
                    mAfterBetIdx = i + 1;
                }
                mCurrBid = bid;
            }
            play = dComIfGp_ea();
            auctionGameInfo(play).play.mMsgCountNumber = bid;
        }
        break;
    }
    case 0x1CFC:
        if (!auctionMessage(manager)->mSelectNum)
            *message = 0x1D1F;
        else
            status = 16;
        break;
    case 0x1D1A:
        if (!auctionMessage(manager)->mSelectNum)
            mAuctionEnded = 1;
        status = 16;
        break;
    case 0x1D1F:
        auction_dComIfGp_setNextStage("sea", 3, 11, -1, 0, 1, 0, zero);
        [[fallthrough]];
    case 0x1D24: {
        s16 bid = mCurrBid;
        u32 play = dComIfGp_ea();
        auctionGameInfo(play).play.mMiniGameRupee = bid;
        bid = mCurrBid;
        play = dComIfGp_ea();
        auctionGameInfo(play).play.mAuctionRupee = bid;
        mAuctionEnded = 1;
        status = 16;
        break;
    }
    case 0x1D20:
    case 0x1D21:
    case 0x1D22:
    case 0x1D23: {
        setLinkAnm(20);
        u32 selected = getRand(6) + 1;
        u32 kind = mAucMdlNo[(selected & 0xFF)];
        if (kind >= 12) {
            auction_JUTAssertion_showAssert(
                "d_a_auction.cpp", 0xCFF,
                "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])");
            kind = 0;
        }
        *message = (s32)l_npc_msg_dat[kind].mAfterBetReplyMsgNo;
        mReplyNpc = selected;
        break;
    }
    case 0x1D48:
    case 0x33A2:
    case 0x33A8:
    case 0x33A5:
    case 0x339F:
    case 0x1D4B:
    case 0x1D3F:
    case 0x1D45:
    case 0x1D42:
    case 0x1D3C:
        mCameraSwayAngle = 0;
        mGestureFlags = mGestureFlags | 2;
        status = 16;
        break;
    case 0x1D05:
        if (mAuctionType == 2)
            *message = 0x1D1C;
        else {
            u32 save = l_saveInfo;
            if (mCurrentNpc) {
                auction_dSv_event_c_onEventBit_abi2(save + 0x644, 0x4008);
                u8 selected = mCurrentNpc;
                save = l_saveInfo;
                u8 kind = mAucMdlNo[selected];
                auction_dSv_event_c_setEventReg(save + 0x644, 0x790F, (u32)kind);
                save = l_saveInfo;
                u32 item = auction_dSv_info_getEvent_abi2(save + 0x12C0);
                auctionSavedItem(item)->mAuctionItem = mCurrAuctionItemIndex;
            } else
                auction_dSv_event_c_offEventBit(save + 0x644, 0x4008);
            auction_fopAcM_deleteByID((u32)mCurrAuctionItemPID);
            status = 16;
        }
        break;
    case 0x1D1C:
    case 0x1D07: {
        s16 bid = mCurrBid;
        u32 play = dComIfGp_ea();
        u32 old = auctionGameInfo(play).play.mItemRupeeCount;
        auctionGameInfo(play).play.mItemRupeeCount =
            number == 0x1D1C ? old + (s32)bid : old - (s32)bid;
        if (number == 0x1D1C)
            status = 16;
        else
            *message = 0x1D08;
        break;
    }
    case 0x1D1D:
    case 0x1D08:
        *message = 0x1D09;
        break;
    default:
        status = 16;
        break;
    }
    mNextMsgNo = status == 15 ? (u32)*message : 0;
    return status;
}

/* 020594AC */
void daAuction_c::setMessage(u32 message) {
    mCurrMsgNo = message;
    mCurrMsgBsPcId = 0xFFFFFFFF;
}

/* 0205AD6C */
void daAuction_c::setMessage2(u32 message) {
    // HD: setMessage leaves r3 (this) unchanged; the original keeps using it.
    auction_daAuction_c_setMessage_abi2(this, message);
    u32 receiver = gabi::ea(this);
    auctionActorController(receiver)->mMainState = daAuction_c::MAIN_MSG_SET;
    auction_dAuction_screen_gaugeHide(gabi::at<void>(receiver));
}

/* 02059030 */
void daAuction_c::setMtx() {
    f32 sx = scale.x, sy = scale.y;
    u32 model = mpModel;
    f32 sz = scale.z;
    auctionModel(model)->mBaseScale.x = sx;
    auctionModel(model)->mBaseScale.y = sy;
    auctionModel(model)->mBaseScale.z = sz;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    auction_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    model = mpModel;
    for (u32 i = 0; i < 12; ++i)
        auctionModel(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}

/* 02059838 */
void Auction_obtainItem(AuctionItem* item) {
    u16 event = item->mObtainedEventBit;
    if (event) {
        u32 save = l_saveInfo;
        auction_dSv_event_c_onEventBit(&auctionSave(save)->mSavedata.mEvent, event);
    }
}

/* 0205A608 */
BOOL Auction_itemObtained(AuctionItem* item) {
    u16 event = item->mObtainedEventBit;
    if (event) {
        u32 save = l_saveInfo;
        return auction_dSv_event_c_isEventBit(&auctionSave(save)->mSavedata.mEvent, event) != 0;
    }
    if (item->mItemID == 0x77) {
        u32 save = l_saveInfo;
        return auctionSave(save)->mSavedata.mPlayer.mPlayerItem.mItems[1] == 0x77;
    }
    return false;
}

/* 0205CD90 */
BOOL Auction_hasAvailableItem() {
    for (u32 i = 0; i < 5; ++i)
        if (!auction_itemObtained(&l_item_dat[i]))
            return true;
    return false;
}

/* 0205A67C */
u8 daAuction_c::getItemNo() {
    u32 available = 0;
    for (u32 i = 0; i < 5; ++i)
        if (!auction_itemObtained(&l_item_dat[i]))
            ++available;
    u32 choice = getRand(available);
    u32 result = 0;
    for (u32 i = 0; i < 4; ++i) {
        if (!auction_itemObtained(&l_item_dat[i])) {
            if (!choice)
                break;
            --choice;
        }
        ++result;
    }
    return result;
}

/* 02059854 */
fopAc_ac_c* daAuction_c::getNpcActorP(s32 index) {
    if (!index) {
        u32 play = dComIfGp_ea();
        return auctionGameInfo(play).play.mPlayerInfo[0].mpPlayer.get();
    }
    u32 id = mNpcIds[(u32)index];
    gabi::Local<be<u32>> query;
    query.get()->set(id);
    if (id == 0xFFFFFFFF)
        return nullptr;
    return auction_fopAcIt_Judge(gabi::at<void>(0x025E1234), query.get());
}

/* 020598BC */
void daAuction_c::setCameraNpc(s32 index, s16 angle) {
    u32 npc = auction_daAuction_c_getNpcActorP(this, index);
    if (!npc) {
        auction_JUTAssertion_showAssert("d_a_auction.cpp", 0xDA1, "actor");
        return;
    }
    f32 x = auctionActor(npc)->current.pos.x;
    mCameraCenter.x = x;
    f32 y = auctionActor(npc)->current.pos.y;
    mCameraCenter.y = y;
    f32 z = auctionActor(npc)->current.pos.z;
    auto* data = &l_npc_camera_dat[(u32)index];
    mCameraCenter.z = z;
    f32 height = (f32)data->mCenterOffsetY;
    mCameraCenter.y = gabi::fadds_ppc(y, height);
    u16 facing = auctionActor(npc)->current.angle.y;
    f32 radius = (f32)data->mEyeDistance;
    f32 sine = cM_ssin(facing);
    f32 offsetX = gabi::fmuls_ppc(sine, radius);
    f32 zero = l_zero;
    mCameraEye.x = offsetX;
    mCameraEye.y = zero;
    facing = auctionActor(npc)->current.angle.y;
    f32 cosine = cM_scos(facing);
    radius = (f32)data->mEyeDistance;
    mCameraEye.z = gabi::fmuls_ppc(cosine, radius);
    s16 baseAngle = data->mEyeAngleY;
    s32 extra = angle;
    if (!extra)
        extra = (s16)gabi::ftoi(cM_rndFX((f32)l_cameraRandomAngle));
    auction_mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)((u32)(s32)baseAngle + (u32)extra));
    s16 tilt = data->mEyeAngleX;
    auction_mDoMtx_XrotM(mDoMtx_stack_c::get(), tilt);
    auction_PSMTXMultVec(mDoMtx_stack_c::get(), &mCameraEye, &mCameraEye);
    auction_PSVECAdd(&mCameraEye, &mCameraCenter, &mCameraEye);
    mCameraFlags = (mCameraFlags | 9) & 0xFD;
}

/* 02059D50 */
void daAuction_c::setLinkAnm(u8 animation) {
    u32 selected = animation;
    if (selected == 1 && !mTopBidder)
        selected = 0x1D;
    u32 play = dComIfGp_ea();
    u32 link = auctionGameInfo(play).play.mpPlayerPtr[0].v;
    auctionPlayer(link)->mDemo.mDemoMode = selected;
    auctionPlayer(link)->mDemo.mParam0 = 3;
    mCurLinkAnm = selected;
}

/* 0205C1CC */
f32 daAuction_c::getPiconDispOfs(s32 index) {
    if (!index)
        return l_npc_emitter_ofsy[0];
    u32 npc = auction_daAuction_c_getNpcActorP(this, index);
    if (npc)
        return auctionNpc(npc)->mPiconOfsY;
    auction_JUTAssertion_showAssert("d_a_auction.cpp", 0xDDE, "act");
    return l_zero;
}

/* 0205AD98 */
void daAuction_c::nextBet() {
    u8 delay = mNextBetDelay;
    if (delay) {
        mNextBetDelay = (u8)(delay - 1);
        return;
    }
    f32 threshold = l_gaugeMaximum, maximum = l_lightY, scale = l_one;
    for (u32 i = 1; i <= 6; ++i) {
        u32 play = dComIfGp_ea();
        u32 end = auctionGameInfo(play).play.mTimerInfo.mTimerLimitTimeMs;
        play = dComIfGp_ea();
        u32 start = auctionGameInfo(play).play.mTimerInfo.mTimerNowTimeMs;
        s32 milliseconds = (s32)((end - start) * 1000u);
        s32 seconds = milliseconds / 30 / 1000;
        s32 random = getRand((s32)(60u - (u32)seconds));
        f32 addition = gabi::fmuls_ppc((f32)random, scale);
        f32 wait = mBidGauge[i];
        if (addition > maximum)
            addition = maximum;
        f32 next = gabi::fadds_ppc(wait, addition);
        mBidGauge[i] = next;
        if (!(next < threshold))
            mNextBetDelay = (u8)((u8)mNextBetDelay + 1);
    }
    mNextBetDelay = (u8)((u8)mNextBetDelay + 2);
}

/* 02058E88 */
s32 daAuction_c::getRand(s32 range) {
    f32 random = cM_rndF((f32)range);
    s32 result = gabi::ftoi(random);
    return (u32)result == (u32)range ? 0 : result;
}

/* 020592E8 */
s32 Auction_createWrapper(daAuction_c* actor) { return actor->_create(); }

/* 0205937C */
BOOL Auction_deleteWrapper(daAuction_c* actor) { return actor->_delete(); }

/* 0205ACD4 */
BOOL Auction_executeWrapper(daAuction_c* actor) { return actor->_execute(); }

/* 0205AD40 */
BOOL Auction_drawWrapper(daAuction_c* actor) { return actor->_draw(); }

/* 0205CFC8 */
BOOL Auction_isDelete(void* actor) { return true; }

// HD: compiler-generated and template code of this translation unit.

/* 0205CFB4 */
void Auction_SafeString_deletingDestructor(void* object, s32 flags) {
    if (object && (flags & 1))
        auction___dl(object);
}

/* 0205D074 */
void Auction_SafeString_assureTerminated(void* object) {}

/* 0205CFD8 */
void Auction_deletingDestructor(daAuction_c* actor, s32 flags) {
    if (!actor)
        return;
    auction_dCcD_Cyl_destruct(&actor->mCyl, 2);
    auction_dCcD_Stts_destruct(&actor->mStts, 2);
    auction_dBgS_AcchCir_destruct(&actor->mAcchCir.m_cir, 2);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = 0x10007764;
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = 0x10007774;
    auction_dBgS_ObjAcch_destruct(&actor->mObjAcch, 0);
    auction_fopAc_ac_c_destruct_fopAc_ac_c(actor, 0);
    if (flags & 1)
        auction___dl_abi2(actor);
}

/* 0205CE14 */
void Auction_staticInit() {
    l_HIO.mRegistrationWords[3] = 0;
    l_HIO.mRegistrationWords[2] = 0;
    l_HIO.mRegistrationWords[1] = 0;
    l_HIO.mRegistrationWords[0] = 0;
    auction_registerGlobalObject(&l_registerCamera);
    f32 light = l_angleMinimum, blend = l_angleMaximum;
    l_HIO.mAngleMin = light;
    l_HIO.mAngleMax = blend;
    auction_auctionHIO_destruct(&l_HIO.mRoot);
    auction_registerGlobalObject(&l_registerHIO);
    auction_auctionNpcHIO_destruct(&l_HIO.mNpc);
    auction_registerGlobalObject(&l_registerNpcHIO);
    f32 speed = l_auctionHIOBidMaximum, first = l_cameraInitial0, gauge = l_auctionHIOGaugeMaximum;
    f32 second = l_cameraInitial1, third = l_cameraInitial2;
    l_HIO.mBidMax[0] = speed;
    l_HIO.mGaugeMax[1] = gauge;
    l_camera_pos.positions[0][0].x = first;
    l_camera_pos.positions[0][0].y = second;
    f32 fourth = l_cameraInitial3;
    l_camera_pos.positions[0][0].z = third;
    f32 fifth = l_cameraInitial4;
    l_camera_pos.positions[0][1].x = fourth;
    l_camera_pos.positions[0][1].y = fifth;
    f32 sixth = l_cameraInitial5, seventh = l_cameraInitial6;
    l_camera_pos.positions[0][1].z = sixth;
    f32 eighth = l_cameraInitial7;
    l_camera_pos.positions[1][0].x = seventh;
    f32 ninth = l_cameraInitial8;
    l_camera_pos.positions[1][0].y = eighth;
    f32 tenth = l_cameraInitial9, eleventh = l_cameraInitial10, thirteenth = l_cameraInitial12;
    f32 twelfth = l_cameraInitial11;
    l_camera_pos.positions[1][0].z = ninth;
    l_camera_pos.positions[1][1].x = tenth;
    f32 fourteenth = l_cameraInitial13;
    l_camera_pos.positions[1][1].y = eleventh;
    f32 fifteenth = l_cameraInitial14;
    l_camera_pos.positions[1][1].z = twelfth;
    f32 sixteenth = l_cameraInitial15, seventeenth = l_cameraInitial16;
    l_camera_pos.positions[2][0].x = thirteenth;
    l_HIO.mGaugeMax[0] = gauge;
    f32 eighteenth = l_cameraInitial17;
    l_camera_pos.positions[2][0].y = fourteenth;
    l_HIO.mBidMax[1] = speed;
    l_camera_pos.positions[2][0].z = fifteenth;
    l_camera_pos.positions[2][1].x = sixteenth;
    l_camera_pos.positions[2][1].y = seventeenth;
    l_camera_pos.positions[2][1].z = eighteenth;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit
data l_cut_names 101903D0
data l_rest_msg_time1 10190150
data l_rest_msg_time2 10190158
data l_after_bet_chk 10190160
data l_item_dat22 10190168
data l_npc_emitter_ofsy 101901A4
data l_after_bet_rate 101901C4
data l_after_bet_msg_no 101901D4
data l_link_face 101901F4
data l_rest_msg_no 1019027C
data l_rest_se_no 10190288
data l_one 10007798
data l_timerX 10007810
data l_timerY 10007814
data l_timerWidth 10007818
data l_timerHeight 1000781C
data l_zero 10007820
data l_lightY 10007824
data l_lightZ 10007828
data l_lightPower 1000782C
data l_cameraRandomAngle 10007864
data l_blendStep 10007928
data l_alphaStep 100079AC
data l_alphaMaximum 100079B0
data l_gaugeMaximum 100079B4
data l_negativeOne 100079B8
data l_gaugeIdleStep 100079BC
data l_unsignedFloatBoundary 100079C0
data l_lateBidMultiplier 100079C4
data l_midBidMultiplier 100079C8
data l_framesPerSecond 100079CC
data l_paralyzedCameraZ 100079D0
data l_paralyzedCameraRadius 100079D4
data l_piconCenterOffset 10007A8C
data l_cameraBlendScale 10007A90
data l_cameraBlendMaximum 10007A94
data l_cameraBlendMinimum 10007A98
data l_angleMinimum 10007B40
data l_angleMaximum 10007B44
data l_auctionHIOBidMaximum 10007B48
data l_auctionHIOGaugeMaximum 10007B4C
data l_cameraInitial0 10007B50
data l_cameraInitial1 10007B54
data l_cameraInitial2 10007B58
data l_cameraInitial3 10007B5C
data l_cameraInitial4 10007B60
data l_cameraInitial5 10007B64
data l_cameraInitial6 10007B68
data l_cameraInitial7 10007B6C
data l_cameraInitial8 10007B70
data l_cameraInitial9 10007B74
data l_cameraInitial10 10007B78
data l_cameraInitial11 10007B7C
data l_cameraInitial12 10007B80
data l_cameraInitial13 10007B84
data l_cameraInitial14 10007B88
data l_cameraInitial15 10007B8C
data l_cameraInitial16 10007B90
data l_cameraInitial17 10007B94
str 100077F8 "Pspl" in 02059240
str 10007800 "Pspl" in 020592EC
str 1000779C "Auction" in 020590FC
str 10007808 "MsgNo" in 020594BC
str 10007880 "Timer" in 02059B54
str 10007888 "d_a_auction.cpp" in 02059BD4
str 10007898 "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])" in 02059BD4
str 10007830 "d_a_auction.cpp" in 02059544
str 10007840 "mTimerID != fpcM_ERROR_PROCESS_ID_e" in 02059544
str 10007870 "d_a_auction.cpp" in 020598BC
str 10007868 "actor" in 020598BC
str 1000792C "Auction" in 0205A104
str 10007A7C "d_a_auction.cpp" in 0205C1CC
str 10007A78 "act" in 0205C1CC
str 10007A9C "d_a_auction.cpp" in 0205C248
str 10007AAC "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])" in 0205C248
str 10007A28 "d_a_auction.cpp" in 0205B9C0
str 10007A38 "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])" in 0205B9C0
str 10007AF0 "d_a_auction.cpp" in 0205C55C
str 10007B00 "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])" in 0205C55C
str 10007AEC "sea" in 0205C55C
str 100079D8 "d_a_auction.cpp" in 0205AF48
str 100079E8 "a_auc_mdl_no < sizeof(l_npc_msg_dat) / sizeof(l_npc_msg_dat[0])" in 0205AF48
ext 025B8B68 void auction_dSv_event_c_onEventBit(void * a0, u16 a1)
ext 025B8B94 s32 auction_dSv_event_c_isEventBit(void * a0, u16 a1)
ext 0205A608 s32 auction_itemObtained(AuctionItem * a0)
ext 0255BA9C void auction_dKy_efplight_cut(AuctionLight * a0)
ext 025A11EC void auction_fopNpc_npc_c_talk(daAuction_c * a0, s32 a1)
ext 025A11EC u32 auction_fopNpc_npc_c_talk_abi2(daAuction_c * a0, s32 a1)
ext 025DE50C u32 auction_fopMsgM_SearchByID(u32 a0)
ext 025C5E54 void auction_dTimer_setShowType(void * a0, s32 a1)
ext 020594AC u32 auction_daAuction_c_setMessage(daAuction_c * a0, u32 a1)
ext 0261BC80 void auction_dAuction_screen_gaugeHide(void * a0)
ext 0273AF40 void auction___dl(void * a0)
ext 02515A70 void auction_dCcD_Cyl_destruct(dCcD_Cyl * a0, s32 a1)
ext 02515860 void auction_dCcD_Stts_destruct(dCcD_Stts * a0, s32 a1)
ext 02018034 void auction_dBgS_AcchCir_destruct(void * a0, s32 a1)
ext 024EFD9C void auction_dBgS_ObjAcch_destruct(dBgS_ObjAcch * a0, s32 a1)
ext 025D50BC void auction_fopAc_ac_c_destruct_fopAc_ac_c(daAuction_c * a0, s32 a1)
ext 0273AF40 void auction___dl_abi2(daAuction_c * a0)
ext 026067F4 u32 auction_dRes_control_c_getIDRes(void * a0, void * a1, s32 a2)
ext 025E38E0 u32 auction_mDoExt_J3DModel__create(void * a0, s32 a1, s32 a2)
ext 020198D8 f32 cM_rndF(f32 a0)
ext 0273AD10 daAuction_c * auction_operator_new(s32 a0)
ext 025A1458 void auction_fopNpc_npc_construct(daAuction_c * a0)
ext 0259F740 void auction_dNpc_EventCut_construct(dNpc_EventCut_c * a0)
ext 02058EF4 void auction_daAuction_c_daAuction_c(daAuction_c * a0)
ext 02520460 s32 auction_dComIfG_resLoad(u8 * a0, const char * a1)
ext 025D63E8 s32 auction_fopAcM_entrySolidHeap(daAuction_c * a0, void * a1, s32 a2)
ext 025986B0 s32 auction_fopAcM_isCreating(daAuction_c * a0)
ext 020063C0 void auction_enqueueScreenTask(void * a0, void * a1)
ext 025204C8 void auction_dComIfG_resDelete(u8 * a0, const char * a1)
ext 02555D0C u32 auction_dKy_getEnvlight()
ext 025626A4 void auction_dScnKy_env_light_c_settingTevStruct(void * a0, s32 a1, void * a2, void * a3)
ext 02562F5C void auction_dScnKy_env_light_setLightTevColorType(void * a0, void * a1, void * a2)
ext 025E2DE0 void auction_mDoExt_modelUpdateDL(void * a0, s32 a1)
ext 028E93CC void auction_PSMTXTrans(void * a0, f32 a1, f32 a2, f32 a3)
ext 025D5218 fopAc_ac_c * auction_fopAcIt_Judge(void * a0, gabi::be<u32> * a1)
ext 0255FDA4 void auction_dKy_custom_colset(s32 a0, s32 a1, f32 a2)
ext 02543F10 s32 auction_dEvent_manager_c_getEventIdx(void * a0, const char * a1, s32 a2)
ext 0259F7D4 void auction_dNpc_EventCut_c_setActorInfo(dNpc_EventCut_c * a0, const char * a1, daAuction_c * a2)
ext 028F026C void auction_registerGlobalObject(void * a0)
ext 028ED6F8 void auction_auctionHIO_destruct(void * a0)
ext 028EAB2C void auction_auctionNpcHIO_destruct(void * a0)
ext 0254487C u32 auction_dEvent_manager_c_getMySubstanceP(void * a0, s32 a1, const char * a2, s32 a3)
ext 025E1988 void auction_mDoAud_seStart(s32 a0)
ext 020594AC void auction_daAuction_c_setMessage_abi2(daAuction_c * a0, u32 a1)
ext 0273AA24 void auction_JUTAssertion_showAssert(const char * a0, s32 a1, const char * a2)
ext 020594AC void auction_daAuction_c_setMessage_abi3(daAuction_c * a0, s32 a1)
ext 025C5864 void auction_dTimer_start(void * a0)
ext 0261BC68 void auction_dAuction_screen_gaugeShow()
ext 0253F124 u32 auction_dEvt_control_c_getPId(void * a0, daAuction_c * a1)
ext 0261BC20 void auction_dAuction_screen_talkEnd()
ext 025D7DEC s32 auction_fopAcM_createItemForPresentDemo(void * a0, u8 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6)
ext 02059838 void auction_obtainItem(AuctionItem * a0)
ext 0254407C s32 auction_dEvent_manager_c_startCheck(void * a0, s32 a1)
ext 025C60F4 s32 auction_dTimer_createTimer(s32 a0, s32 a1, s32 a2, s32 a3, f32 a4, f32 a5, f32 a6, f32 a7)
ext 0261BC50 void auction_dAuction_screen_slotHide()
ext 0261BC80 void auction_dAuction_screen_gaugeHide_abi2()
ext 025D5834 u32 auction_fopAcM_create(s32 a0, s32 a1, void * a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7)
ext 0255B9C8 void auction_dKy_efplight_set(AuctionLight * a0)
ext 02059854 u32 auction_daAuction_c_getNpcActorP(daAuction_c * a0, s32 a1)
ext 02019918 f32 cM_rndFX(f32 a0)
ext 025F1884 void auction_mDoMtx_YrotS(void * a0, s16 a1)
ext 025F1BF4 void auction_mDoMtx_XrotM(void * a0, s16 a1)
ext 028E8F64 void auction_PSMTXMultVec(void * a0, cXyz * a1, cXyz * a2)
ext 028E8D88 void auction_PSVECAdd(cXyz * a0, cXyz * a1, cXyz * a2)
ext 025D57E0 void auction_fopAcM_delete(u32 a0)
ext 0259F858 s32 auction_dNpc_EventCut_c_cutProc(dNpc_EventCut_c * a0)
ext 02542D88 s32 auction_dEvent_manager_c_getMyStaffId(void * a0, const char * a1, s32 a2, s32 a3)
ext 02542EDC s32 auction_dEvent_manager_c_getMyActIdx(void * a0, s32 a1, void * a2, s32 a3, s32 a4, s32 a5)
ext 02543280 void auction_dEvent_manager_c_cutEnd(void * a0, s32 a1)
ext 025447C8 s32 auction_dEvent_manager_c_getIsAddvance(void * a0, s32 a1)
ext 02059B40 void auction_offCamera(daAuction_c * a0)
ext 02059DB4 s32 auction_daAuction_c_eventMain(daAuction_c * a0)
ext 024F8044 u32 auction_dCam_getBody()
ext 02514F2C void auction_dCamera_c_Stop(void * a0)
ext 02514F38 void auction_dCamera_c_Start(void * a0)
ext 02514F50 void auction_dCamera_c_Set(void * a0, cXyz * a1, cXyz * a2)
ext 025C5794 void auction_dTimer_stop(void * a0, s32 a1)
ext 0273AA24 void auction_JUTAssertion_showAssert_abi2(void * a0, s32 a1, void * a2)
ext 025D76A8 void auction_fopAcM_orderSpeakEvent(daAuction_c * a0)
ext 025B8B94 s32 auction_dSv_event_c_isEventBit_abi2(void * a0, s32 a1)
ext 02720118 u32 auction_dSv_info_getEvent(void * a0)
ext 0205A67C u32 auction_daAuction_c_getItemNo(daAuction_c * a0)
ext 025D7970 void auction_fopAcM_orderChangeEventId(void * a0, daAuction_c * a1, s32 a2, s32 a3, s32 a4)
ext 025D7A58 void auction_fopAcM_orderOtherEventId(daAuction_c * a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
ext 0261BC08 void auction_dAuction_screen_talkStart()
ext 025C5E54 void auction_dTimer_setShowType_abi2(u32 a0, s32 a1)
ext 0261CF64 void auction_dAuction_screen_gaugeDown()
ext 02059854 u32 auction_daAuction_c_getNpcActorP_abi2(daAuction_c * a0, u32 a1)
ext 0200EE00 f32 auction_cLib_addCalcPos(cXyz * a0, cXyz * a1, f32 a2, f32 a3, f32 a4)
ext 025A847C void auction_dPa_control_c_set(u32 a0, s32 a1, s32 a2, cXyz * a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 02007898 s32 auction_checkTriggerA(s32 a0)
ext 025E1988 void auction_mDoAud_seStart_abi2(u32 a0)
ext 025B8B94 s32 auction_dSv_event_c_isEventBit_abi3(u32 a0, s32 a1)
ext 025B8BB0 u32 auction_dSv_event_c_getEventReg(u32 a0, s32 a1)
ext 0261BC38 void auction_dAuction_screen_slotShow()
ext 0252012C void auction_dComIfGp_setNextStage(const char * a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, f32 a7)
ext 025B8B68 void auction_dSv_event_c_onEventBit_abi2(u32 a0, s32 a1)
ext 025B8AF4 void auction_dSv_event_c_setEventReg(u32 a0, s32 a1, u32 a2)
ext 02720118 u32 auction_dSv_info_getEvent_abi2(u32 a0)
ext 025B8B7C void auction_dSv_event_c_offEventBit(u32 a0, s32 a1)
ext 025D57E4 void auction_fopAcM_deleteByID(u32 a0)
ext 020078BC s32 auction_checkTriggerB(s32 a0)
ext 025E1A04 void auction_mDoAud_seStartWithParam(s32 a0, s32 a1, u32 a2)
ext 0261CF60 void auction_dAuction_screen_gaugeUp()
ext 025CB374 void auction_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, cXyz * a3)
ext 025440C8 s32 auction_eventEndCheck(void* manager,s32 event)
ext 025C57D4 void auction_timerRestart(u32 timer,s32 type)
data l_item_dat 1019016C
data l_item_dat2 10190194
data l_npc_dat 10190294
data l_npc_msg_dat 10190304
data l_npc_camera_dat 1019037C
data l_after_bet_wait 101901E4
data l_camera_pos 10461544
data l_resourceManager 101F4F28
data l_controllerManager 101F8344
data l_saveInfo 101F84DC
data l_messageManager 101F4B5C
data l_HIO 10461518
data l_registerCamera 101903FC
data l_registerHIO 10190408
data l_registerNpcHIO 10190414
data l_createScreenTask 1048E0AC
data l_deleteScreenTask 1048E0DC
data moveProc 10190214
data eventProc 1019022C
str 100077B4 "AUCTION_START" in 020590FC
str 100077C4 "AUCTION_GET_ITEM" in 020590FC
str 100077A4 "AUCTION_NO_ITEM" in 020590FC
str 100077D8 "AUCTION_START2" in 020590FC
str 100077E8 "AUCTION_END2" in 020590FC
*/
// clang-format on
