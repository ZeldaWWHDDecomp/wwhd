// HD: Auction controller omits the GC gauge PID and adds four bytes to LIGHT_INFLUENCE.
// The phase/event fields shift +0x118, timer/light +0x114, and later fields +0x118.
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/gc_api.h"
struct AuctionLight {
    cXyz mPos;
    GXColorS10 mColor;
    be<f32> mPower;
    be<f32> mFluctuation;
    u8 _1C[4];
    be<f32> mIntensity; // HD: extra intensity after the GameCube light fields.
};
WWHD_SIZE(AuctionLight, 0x24);
WWHD_OFFSET(AuctionLight, mPower, 0x14);
WWHD_OFFSET(AuctionLight, mIntensity, 0x20);
struct daAuction_c : fopNpc_npc_c {
    enum MoveState { MOVE_WAIT, MOVE_NORMAL, MOVE_START };
    enum MainState { MAIN_KAI, MAIN_URI, MAIN_MSG_SET, MAIN_MSG_END, MAIN_BIKON_C, MAIN_BIKON_W };
    using ProcFunc_t = void (daAuction_c::*)();
    void setMessage(u32 message);
    void offCamera();
    void eventEndInit();
    BOOL eventMesSet();
    BOOL eventMesEnd();
    BOOL eventStart();
    void executeWait();
    void executeStart(void* context);
    void executeNormal(void* context);
    void setMessage2(u32 message);
    void eventMainMsgSet();
    BOOL createHeap();
    s32 getRand(s32 range);
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    void setMtx();
    fopAc_ac_c* getNpcActorP(s32 index);
    void setLinkAnm(u8 animation);
    BOOL eventCameraOffNpc();
    BOOL eventEnd();
    u8 getItemNo();
    s32 createInit();
    void eventTalkInit(s32 staff);
    void eventGetItemNpcInit(s32 staff);
    void eventGetItemMesInit();
    void eventMainInit();
    void eventGetItemInit();
    void checkOrder();
    void eventStartInit();
    void setCameraNpc(s32 index, s16 angle);
    void eventMove();
    BOOL _execute();
    void nextBet();
    void privateCut();
    bool eventMain();
    void eventOrder();
    f32 getPiconDispOfs(s32 index);
    void eventMainMsgBikonW();
    void eventMainMsgEnd();
    void eventMainMsgBikonC();
    void eventMainUri();
    u16 next_msgStatus(be<u32>* message);
    void eventMainKai();
    /* 7DC */ u8 mPhs[8];
    /* 7E4 */ dNpc_EventCut_c mNpcEvtInfo;
    /* 850 */ be<u32> mNpcIds[8];  /* was m738: constructor 02058EF4 sets all 8 to 0xFFFFFFFF; daNpcAuction_c phase_1 021EB8DC stores its process id in [its mNpcNo]; executeWait 0205AD44 goes to MOVE_NORMAL once all 8 are set; getNpcActorP 02059854 finds the NPC by it (tww the same; tww setNpcID()); same name as the d_a_npc_auction.h Manager view mNpcIds */
    /* 870 */ be<u32> mCurrAuctionItemPID;
    /* 874 */ be<u32> mTimerID;
    /* 878 */ be<u32> mpTimer;
    /* 87C */ AuctionLight mLight;
    /* 8A0 */ be<u32> mpModel;
    /* 8A4 */ cXyz mCameraCenter;  /* was m78C: privateCut 0205A104 passes it as the center of dCamera_c::Set(center, eye) while mCameraFlags 0x4 and 0x8 are set; setCameraNpc 020598BC sets it to the NPC's pos + (0, mCenterOffsetY, 0); eventMainInit 020596F0 / eventMainKai 0205AF48 copy l_camera_pos[i][0] (tww the same) */
    /* 8B0 */ cXyz mCameraEye;  /* was m798: privateCut 0205A104 passes it as the eye of dCamera_c::Set(center, eye); setCameraNpc 020598BC sets it to mCameraCenter + the rotated mEyeDistance offset; eventMainInit 020596F0 / eventMainMsgEnd 0205C014 copy l_camera_pos[i][1] (tww the same) */
    /* 8BC */ be<u32> mpEmitter;
    /* 8C0 */ cXyz mBikonPos;  /* was m7A8: eventMainMsgBikonC 0205C248: the bidder's pos + getPiconDispOfs, where it sets particle 0x8153 (tww dPa_name::ID_IT_SN_PF_BIKON01) once the camera arrived (tww the same) */
    /* 8CC */ u8 _8CC[8];
    /* 8D4 */ be<f32> mAlpha;
    /* 8D8 */ be<f32> mBlend;
    /* 8DC */ be<f32> mBidGauge[7];  /* was m7C4: eventMainKai 0205AF48 / eventMainUri 0205B9C0 / nextBet 0205AD98: one gauge per bidder ([0] Link, raised on A; [1..6] the NPCs, by NpcDatStruct mGaugeIncMin/Max); a bidder bids once it reaches 100 (l_gaugeMaximum) and is reset; eventMain 02059DB4 shows [0] as mAuctionGauge (tww the same) */
    /* 8F8 */ be<f32> m7E0;
    /* 8FC */ be<f32> m7E4;
    /* 900 */ u8 _900[4];
    /* 904 */ be<s32> mNextMsgNo;  /* was m7EC: next_msgStatus 0205C55C stores the next message number when it continues (status 15), else 0; eventMainMsgEnd 0205C014 turns the camera to mReplyNpc when the button is pressed on an NPC reply (0x1D3A-0x1D4B, 0x339D-0x33A8) (tww the same) */
    /* 908 */ be<s32> mBidMsgNo;  /* was m7F0: eventMainKai 0205AF48 sets 0x1CF9 (NPC bid) / 0x1CFA (Link's bid), eventMainUri 0205B9C0 0x1CF9; eventMainMsgBikonW 0205C4B4 shows it with setMessage2 when mTimer runs out; constructor 02058EF4 0 (tww the same) */
    /* 90C */ be<s16> mEvtStartIdx;
    /* 90E */ be<s16> mEvtGetItemIdx;
    /* 910 */ be<s16> mEvtNoItemIdx;
    /* 912 */ be<s16> mEvtStart2Idx;
    /* 914 */ be<s16> mEvtEnd2Idx;
    /* 916 */ be<s16> mCurrBid;
    /* 918 */ be<s16> mDisplayBid;  /* was m800: eventMainKai 0205AF48 / eventMainUri 0205B9C0: the new total after an NPC bid, mCurrBid + 1 for Link's bid; eventMainKai / eventMainMsgBikonC 0205C248 copy it into mMsgCountNumber, the number the bid message shows (tww the same) */
    /* 91A */ be<s16> mTimer;
    /* 91C */ be<u16> m804;
    /* 91E */ be<s16> mBidWaitTimer;  /* was m806: eventMainKai 0205AF48 / eventMainUri 0205B9C0 count it down each frame and keep the NPC gauges still while it runs; set after a bid to (rnd + 1) * 60 frames and by next_msgStatus 0205C55C (0x1CFA) to l_after_bet_wait * 60 (tww the same) */
    /* 920 */ be<s16> mCameraSwayAngle;  /* was m808: eventMainKai 0205AF48: while mGestureFlags 0x2 is set, mCameraCenter.x = Link's x + sin(it) * 256 (l_paralyzedCameraRadius) and it grows by 200 a frame; next_msgStatus 0205C55C sets 0 with that bit (tww the same) */
    /* 922 */ be<s16> mEmitterFadeDelay;  /* was m80A: _execute 0205AAC8: while it is nonzero the emitter fade-in (mEmitterFadeFlags 0x1) waits and counts it down; never set (only counted down) in WWHD or tww */
    /* 924 */ be<u8> mNpcKinds[8];  /* was m80C: constructor 02058EF4: 0..7, then entries 1-6 swapped 100 times by getRand; daNpcAuction_c phase_1 021EB8DC takes [its prm npc no] as its mNpcNo (tww the same shuffle); same name as the d_a_npc_auction.h Manager view mNpcKinds */
    /* 92C */ be<u8> mAucMdlNo[8];
    /* 934 */ be<u8> mPairedCostume0;  /* was m81C: constructor 02058EF4 0xFF; daNpcAuction_c phase_1 021EB8DC stores the costume of the first NPC of kinds 2-3 and moves the second one off it if it rolls the same; the d_a_npc_auction.h Manager view mPairedCostumes[0] */
    /* 935 */ be<u8> mPairedCostume1;  /* was m81D: constructor 02058EF4 0xFF; daNpcAuction_c phase_1 021EB8DC: the same for NPC kinds 4-5; the d_a_npc_auction.h Manager view mPairedCostumes[1] */
    /* 936 */ be<u8> mMoveState;
    /* 937 */ be<u8> mMainState;  /* was m81F: eventMain 02059DB4 runs eventProc[it] (enum MainState MAIN_KAI .. MAIN_BIKON_W); eventMainInit 020596F0 sets MAIN_URI for mAuctionType 2, else MAIN_KAI; setMessage2 0205AD6C MAIN_MSG_SET; the Kai / Uri / MsgSet / MsgEnd / BikonC states move it on (tww the same) */
    /* 938 */ be<u8> mActiveNpcs;  /* was m820: constructor 02058EF4 1, eventStartInit 02059544 0xFE; daNpcAuction_c isExecute 021EC0E8 runs an NPC only while bit mNpcNo is set (tww the same writes); same name as the d_a_npc_auction.h Manager view mActiveNpcs */
    /* 939 */ be<u8> mSelection;  /* was m821: daNpcAuction_c wait_action 021ED81C / eventMove 021ECB68 set 1 (auction) or 2 (second auction, with mItemChoice) after the talk; eventOrder 0205A724 orders AUCTION_START / AUCTION_START2 for it, copies it to mAuctionType and clears it (tww the same); Manager view mSelection */
    /* 93A */ be<u8> mAuctionType;  /* was m822: eventOrder 0205A724 copies mSelection; for 2 eventStartInit 02059544 uses a 30 s timer (else 60), eventMainInit 020596F0 starts in MAIN_URI, eventGetItemInit 02059A44 l_item_dat2, next_msgStatus 0205C55C 0x1D1B / 0x1D1C; for 1 the gauge is shown (tww the same) */
    /* 93B */ be<u8> mCurrAuctionItemIndex;
    /* 93C */ be<u8> mCurrentNpc;  /* was m824: the top bidder (0 = Link): createInit 020590FC 0; next_msgStatus 0205C55C sets it to mResult when the bid message runs; eventMove 0205A428 orders NO_ITEM while nonzero, else GET_ITEM; daNpcAuction_c getMsg 021EDBF8 picks its line by it (tww the same); Manager view mCurrentNpc */
    /* 93D */ be<u8> mReplyNpc;  /* was m825: next_msgStatus 0205C55C: the outbid top bidder when Link bids, or the random NPC 1-6 answering the after-bet line (0x1D20-0x1D23); eventMainMsgEnd 0205C014 turns the camera to it and sets mSpeakingNpc on the reply messages (tww the same) */
    /* 93E */ be<u8> mTopBidder;  /* was m826: createInit 020590FC 0xFF (no bid yet); next_msgStatus 0205C55C sets it with mCurrentNpc from mResult; eventMainKai 0205AF48 taunts only once it is not 0xFF; setLinkAnm 02059D50 uses animation 0x1D for 1 while it is 0 (Link leads) (tww the same) */
    /* 93F */ be<u8> mResult;  /* was m827: the bidder of the bid in progress (0 = Link): eventMainMsgBikonC 0205C248 copies mNewBidder, the effect / camera / bid message use it; next_msgStatus 0205C55C copies it to mCurrentNpc / mTopBidder; daNpcAuction_c next_msgStatus 021ED964 shows 0x1D39 while nonzero (tww the same); Manager view mResult */
    /* 940 */ be<u8> mNewBidder;  /* was m828: eventMainKai 0205AF48 / eventMainUri 0205B9C0: the first bidder whose mBidGauge reached 100 below its mSpendingLimit; eventMainMsgBikonC 0205C248 copies it into mResult and mSpeakingNpc (tww the same) */
    /* 941 */ be<u8> mSpeakingNpc;  /* was m829: the NPC whose line runs: eventMainMsgBikonC 0205C248 the bidder, eventMainMsgEnd 0205C014 mReplyNpc, eventGetItemNpcInit 02059B54 mCurrentNpc; daNpcAuction_c eventMain 021EC400 plays the mGestureFlags gestures for the NPC with this number (tww the same); Manager view mSpeakingNpc */
    /* 942 */ be<u8> mRestMsgIdx;  /* was m82A: eventMainKai 0205AF48 / eventMainUri 0205B9C0: once the time left drops below l_rest_msg_time1/2[it], show l_rest_msg_no[it] with sound l_rest_se_no[it] and add 1; eventMainInit 020596F0 0 (tww the same) */
    /* 943 */ be<u8> mAuctionEnded;  /* was m82B: set 1 when the time runs out (eventMainKai 0205AF48 / eventMainUri 0205B9C0) and by the closing messages (next_msgStatus 0205C55C 0x1D1A / 0x1D1F / 0x1D24); eventMain 02059DB4 then ends the main cut (screens hidden, Link demo 2) (tww the same) */
    /* 944 */ be<u8> mAfterBetIdx;  /* was m82C: next_msgStatus 0205C55C (0x1CFA): the l_after_bet_rate / _chk / _wait tier Link's bid met (4 = none); on 0x1CF9 a value below 4 shows l_after_bet_msg_no[it], else the NPC reply; NPC bids set 4 (eventMainKai 0205AF48 / eventMainUri 0205B9C0) (tww the same) */
    /* 945 */ be<u8> mCurLinkAnm;
    /* 946 */ be<u8> mFirstBidMade;  /* was m82E: eventMainUri 0205B9C0 sets 1 at the first NPC bid; while it is set A shows 0x1D1A (stop the auction) and the Do status is 0x25; eventMainInit 020596F0 0 (tww the same) */
    /* 947 */ be<u8> mCameraPosIdx;  /* was m82F: eventMainKai 0205AF48 / eventMainMsgEnd 0205C014 set getRand(3) and copy l_camera_pos[it] into mCameraCenter / mCameraEye; eventMainInit 020596F0 0 (WWHD uses the register copy; tww reads the field back) */
    /* 948 */ be<u8> mTauntShown;  /* was m830: eventMainKai 0205AF48 / eventMainUri 0205B9C0: after a bid it is only cleared while set; else a taunt (0x1CFD-0x1D00, 1 in 3) may run and sets it; eventStartInit 02059544 1 (tww the same) */
    /* 949 */ be<u8> mNextBetDelay;  /* was m831: nextBet 0205AD98: while nonzero it is counted down and nothing else happens; else every NPC gauge gets a random push and it becomes the number of NPCs at 100 plus 2 (tww the same) */
    /* 94A */ be<u8> mSetLinkFace;  /* was m832: eventMain 02059DB4 sets Link's mFace to mFace while it is set; eventMainMsgBikonC 0205C248 sets 1 with a random l_link_face when an NPC bids; constructor 02058EF4 / eventMainMsgEnd 0205C014 0 (tww the same) */
    /* 94B */ be<u8> mFace;
    /* 94C */ be<u8> mGestureFlags;  /* was m834: bit field: 0x1 eventMainMsgBikonW 0205C4B4 (NPC bid line), 0x2 / 0x4 the sway camera (next_msgStatus 0205C55C, eventMainKai 0205AF48), 0x8 eventMainMsgEnd 0205C014 reply, 0x10 eventGetItemNpcInit 02059B54, 0x20 taunt; daNpcAuction_c eventMain 021EC400 gestures by them (tww the same); Manager view mGestureFlags */
    /* 94D */ be<u8> mCameraFlags;  /* was m835: bit field read by privateCut 0205A104: 0x1 stop the camera (then 0x4), 0x2 restart it (clears 0x4 / 0x8), 0x4 + 0x8 set mCameraCenter / mCameraEye; setCameraNpc 020598BC sets 0x9, offCamera 02059B40 / eventCameraOffNpc 0205A010 0x2 (tww the same; tww offCamera()) */
    /* 94E */ be<u8> mEmitterFadeFlags;  /* was m836: _execute 0205AAC8: 0x1 fades the emitter's alpha in to the maximum (then clears it), 0x2 fades it out and deletes the emitter at 0; eventMainInit 020596F0 sets 0x1, eventMain 02059DB4 0x2 when the auction ends (tww the same) */
    /* 94F */ be<u8> mFlags;
    /* 950 */ be<s8> mOrderEvent;  /* was m838: eventOrder 0205A724: 1 / 2 the talk (1 orders the speak event), 3 AUCTION_START, 4 GET_ITEM, 5 NO_ITEM, 6 START2, 7 END2; checkOrder 02059380 clears it once that event starts; eventOrder / eventMove 0205A428 set it (tww the same); same role as daNpc_Hi1_c::mOrderEvent */
    /* 951 */ be<s8> mAction;
    /* 952 */ u8 _952[2];
};
WWHD_SIZE(daAuction_c, 0x954);
WWHD_OFFSET(daAuction_c, mNpcEvtInfo, 0x7E4);
WWHD_OFFSET(daAuction_c, mLight, 0x87C);
WWHD_OFFSET(daAuction_c, mpModel, 0x8A0);
WWHD_OFFSET(daAuction_c, mCameraFlags, 0x94D);

inline dComIfG_inf_c& auctionGameInfo(u32 address) { return *gabi::at<dComIfG_inf_c>(address); }

struct AuctionItem {
    be<s16> mItemID, mNameMsgID, mStartingBid;
    be<u16> mObtainedEventBit;
};
WWHD_SIZE(AuctionItem, 8);

// HD: full-width call-site ABI bindings used by this translation unit.
void auction_dSv_event_c_onEventBit(void* a0, u16 a1);
s32 auction_dSv_event_c_isEventBit(void* a0, u16 a1);
s32 auction_itemObtained(AuctionItem* a0);
void auction_dKy_efplight_cut(AuctionLight* a0);
void auction_fopNpc_npc_c_talk(daAuction_c* a0, s32 a1);
u32 auction_fopNpc_npc_c_talk_abi2(daAuction_c* a0, s32 a1);
u32 auction_fopMsgM_SearchByID(u32 a0);
void auction_dTimer_setShowType(void* a0, s32 a1);
u32 auction_daAuction_c_setMessage(daAuction_c* a0, u32 a1);
void auction_dAuction_screen_gaugeHide(void* a0);
void auction___dl(void* a0);
void auction_dCcD_Cyl_destruct(dCcD_Cyl* a0, s32 a1);
void auction_dCcD_Stts_destruct(dCcD_Stts* a0, s32 a1);
void auction_dBgS_AcchCir_destruct(void* a0, s32 a1);
void auction_dBgS_ObjAcch_destruct(dBgS_ObjAcch* a0, s32 a1);
void auction_fopAc_ac_c_destruct_fopAc_ac_c(daAuction_c* a0, s32 a1);
void auction___dl_abi2(daAuction_c* a0);
u32 auction_dRes_control_c_getIDRes(void* a0, void* a1, s32 a2);
u32 auction_mDoExt_J3DModel__create(void* a0, s32 a1, s32 a2);
f32 auction_cM_rndF(f32 a0);
daAuction_c* auction_operator_new(s32 a0);
void auction_fopNpc_npc_construct(daAuction_c* a0);
void auction_dNpc_EventCut_construct(dNpc_EventCut_c* a0);
void auction_daAuction_c_daAuction_c(daAuction_c* a0);
s32 auction_dComIfG_resLoad(u8* a0, const char* a1);
s32 auction_fopAcM_entrySolidHeap(daAuction_c* a0, void* a1, s32 a2);
s32 auction_fopAcM_isCreating(daAuction_c* a0);
void auction_enqueueScreenTask(void* a0, void* a1);
void auction_dComIfG_resDelete(u8* a0, const char* a1);
u32 auction_dKy_getEnvlight();
void auction_dScnKy_env_light_c_settingTevStruct(void* a0, s32 a1, void* a2, void* a3);
void auction_dScnKy_env_light_setLightTevColorType(void* a0, void* a1, void* a2);
void auction_mDoExt_modelUpdateDL(void* a0, s32 a1);
void auction_PSMTXTrans(void* a0, f32 a1, f32 a2, f32 a3);
fopAc_ac_c* auction_fopAcIt_Judge(void* a0, gabi::be<u32>* a1);
void auction_dKy_custom_colset(s32 a0, s32 a1, f32 a2);
s32 auction_dEvent_manager_c_getEventIdx(void* a0, const char* a1, s32 a2);
void auction_dNpc_EventCut_c_setActorInfo(dNpc_EventCut_c* a0, const char* a1, daAuction_c* a2);
void auction_registerGlobalObject(void* a0);
void auction_auctionHIO_destruct(void* a0);
void auction_auctionNpcHIO_destruct(void* a0);
u32 auction_dEvent_manager_c_getMySubstanceP(void* a0, s32 a1, const char* a2, s32 a3);
void auction_mDoAud_seStart(s32 a0);
void auction_daAuction_c_setMessage_abi2(daAuction_c* a0, u32 a1);
void auction_JUTAssertion_showAssert(const char* a0, s32 a1, const char* a2);
void auction_daAuction_c_setMessage_abi3(daAuction_c* a0, s32 a1);
void auction_dTimer_start(void* a0);
void auction_dAuction_screen_gaugeShow();
u32 auction_dEvt_control_c_getPId(void* a0, daAuction_c* a1);
void auction_dAuction_screen_talkEnd();
s32 auction_fopAcM_createItemForPresentDemo(void* a0, u8 a1, s32 a2, s32 a3, s32 a4, s32 a5,
                                            s32 a6);
void auction_obtainItem(AuctionItem* a0);
s32 auction_dEvent_manager_c_startCheck(void* a0, s32 a1);
s32 auction_dTimer_createTimer(s32 a0, s32 a1, s32 a2, s32 a3, f32 a4, f32 a5, f32 a6, f32 a7);
void auction_dAuction_screen_slotHide();
void auction_dAuction_screen_gaugeHide_abi2();
u32 auction_fopAcM_create(s32 a0, s32 a1, void* a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
void auction_dKy_efplight_set(AuctionLight* a0);
u32 auction_daAuction_c_getNpcActorP(daAuction_c* a0, s32 a1);
f32 auction_cM_rndFX(f32 a0);
void auction_mDoMtx_YrotS(void* a0, s16 a1);
void auction_mDoMtx_XrotM(void* a0, s16 a1);
void auction_PSMTXMultVec(void* a0, cXyz* a1, cXyz* a2);
void auction_PSVECAdd(cXyz* a0, cXyz* a1, cXyz* a2);
void auction_fopAcM_delete(u32 a0);
s32 auction_dNpc_EventCut_c_cutProc(dNpc_EventCut_c* a0);
s32 auction_dEvent_manager_c_getMyStaffId(void* a0, const char* a1, s32 a2, s32 a3);
s32 auction_dEvent_manager_c_getMyActIdx(void* a0, s32 a1, void* a2, s32 a3, s32 a4, s32 a5);
void auction_dEvent_manager_c_cutEnd(void* a0, s32 a1);
s32 auction_dEvent_manager_c_getIsAddvance(void* a0, s32 a1);
void auction_offCamera(daAuction_c* a0);
s32 auction_daAuction_c_eventMain(daAuction_c* a0);
u32 auction_dCam_getBody();
void auction_dCamera_c_Stop(void* a0);
void auction_dCamera_c_Start(void* a0);
void auction_dCamera_c_Set(void* a0, cXyz* a1, cXyz* a2);
void auction_dTimer_stop(void* a0, s32 a1);
void auction_JUTAssertion_showAssert_abi2(void* a0, s32 a1, void* a2);
void auction_fopAcM_orderSpeakEvent(daAuction_c* a0);
s32 auction_dSv_event_c_isEventBit_abi2(void* a0, s32 a1);
u32 auction_dSv_info_getEvent(void* a0);
u32 auction_daAuction_c_getItemNo(daAuction_c* a0);
void auction_fopAcM_orderChangeEventId(void* a0, daAuction_c* a1, s32 a2, s32 a3, s32 a4);
void auction_fopAcM_orderOtherEventId(daAuction_c* a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
void auction_dAuction_screen_talkStart();
void auction_dTimer_setShowType_abi2(u32 a0, s32 a1);
void auction_dAuction_screen_gaugeDown();
u32 auction_daAuction_c_getNpcActorP_abi2(daAuction_c* a0, u32 a1);
f32 auction_cLib_addCalcPos(cXyz* a0, cXyz* a1, f32 a2, f32 a3, f32 a4);
void auction_dPa_control_c_set(u32 a0, s32 a1, s32 a2, cXyz* a3, s32 a4, s32 a5, s32 a6, s32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
s32 auction_checkTriggerA(s32 a0);
void auction_mDoAud_seStart_abi2(u32 a0);
s32 auction_dSv_event_c_isEventBit_abi3(u32 a0, s32 a1);
u32 auction_dSv_event_c_getEventReg(u32 a0, s32 a1);
void auction_dAuction_screen_slotShow();
void auction_dComIfGp_setNextStage(const char* a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6,
                                   f32 a7);
void auction_dSv_event_c_onEventBit_abi2(u32 a0, s32 a1);
void auction_dSv_event_c_setEventReg(u32 a0, s32 a1, u32 a2);
u32 auction_dSv_info_getEvent_abi2(u32 a0);
void auction_dSv_event_c_offEventBit(u32 a0, s32 a1);
void auction_fopAcM_deleteByID(u32 a0);
s32 auction_checkTriggerB(s32 a0);
void auction_mDoAud_seStartWithParam(s32 a0, s32 a1, u32 a2);
void auction_dAuction_screen_gaugeUp();
void auction_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, cXyz* a3);

s32 auction_eventEndCheck(void* manager, s32 event);
void auction_timerRestart(u32 timer, s32 type);

struct NpcDatStruct {
    be<f32> mGaugeIncMin, mGaugeIncMax;  /* was field_0x00: eventMainKai 0205AF48 / eventMainUri 0205B9C0: mBidGauge[i] += it + rnd(mGaugeIncMax - it) (row 0 on Link's A press, rows 1-6 each frame outside mBidWaitTimer; Uri doubles it) (tww the same); was field_0x04: eventMainKai 0205AF48 / eventMainUri 0205B9C0: the top of the random mBidGauge step, mGaugeIncMin + rnd(it - mGaugeIncMin) (tww the same) */
    be<s16> mBidIncMin, mBidIncMax, mSpendingLimit;
    u8 _pad[2];
};
WWHD_SIZE(NpcDatStruct, 0x10);
WWHD_OFFSET(NpcDatStruct, mSpendingLimit, 0xC);
struct NpcMsgDatStruct {
    be<s16> mNpcNameMsgID, mOutbidMsgNo, mOutbidLateMsgNo, mAfterBetReplyMsgNo, mWinMsgNo;  /* was field_0x00: createInit 020590FC, eventMain 02059DB4, eventMainKai 0205AF48 / eventMainUri 0205B9C0, eventMainMsgBikonC 0205C248, next_msgStatus 0205C55C: the bidder kind's name message, set as mNpcNameMessageID (tww dComIfGp_setNpcNameMessageID); was field_0x02: next_msgStatus 0205C55C (0x1CF9 after Link's bid with no after-bet line): the outbid top bidder kind's reply while 10 s or more remain (tww: 60 s); 0 (kinds 10 / 11) = no reply (tww the same); was field_0x04: next_msgStatus 0205C55C (0x1CF9): the same reply as mOutbidMsgNo when under 10 s remain (tww: 60 s) (tww the same); was field_0x06: next_msgStatus 0205C55C: after the after-bet lines 0x1D20-0x1D23 (l_after_bet_msg_no) the next message is this one of a random NPC 1-6's kind (mReplyNpc) (tww the same); was field_0x08: eventGetItemMesInit 02059BD4: the message of the GET_ITEM_MES cut, of the winning NPC's kind (mCurrentNpc) (tww the same) */
};
WWHD_SIZE(NpcMsgDatStruct, 0xA);
struct NpcCameraDatStruct {
    be<f32> mEyeDistance, mCenterOffsetY;  /* was field_0x00: setCameraNpc 020598BC: mCameraEye starts at (sin, 0, cos)(the NPC's angle y) * it before the rotations, the eye's distance from mCameraCenter (tww the same); was field_0x04: setCameraNpc 020598BC: mCameraCenter = the NPC's pos + (0, it, 0) (tww the same) */
    be<s16> mEyeAngleX, mEyeAngleY;  /* was field_0x08: setCameraNpc 020598BC: the XrotM angle applied to the eye offset (-500 in Link's row) (tww the same); was field_0x0A: setCameraNpc 020598BC: the YrotS base angle, plus the passed angle or a random cM_rndFX(l_cameraRandomAngle) (tww the same) */
};
WWHD_SIZE(NpcCameraDatStruct, 0xC);
struct AuctionBetWait {
    be<s16> minimum, maximum;
};
WWHD_SIZE(AuctionBetWait, 4);
struct AuctionCameraTable {
    cXyz positions[3][2];
};
WWHD_SIZE(AuctionCameraTable, 0x48);

// HD: explicit word views keep integer vector copies and their original store order.
struct AuctionVectorWords {
    be<u32> value[3];
};
WWHD_SIZE(AuctionVectorWords, 12);
inline AuctionVectorWords& auctionWords(cXyz& v) {
    return *reinterpret_cast<AuctionVectorWords*>(&v);
}
struct AuctionCameraWords {
    be<u32> value[6];
};
WWHD_SIZE(AuctionCameraWords, 24);
inline AuctionCameraWords& auctionCameraWords(cXyz& v) {
    return *reinterpret_cast<AuctionCameraWords*>(&v);
}
inline fopAc_ac_c* auctionActor(u32 address) { return gabi::at<fopAc_ac_c>(address); }
inline daAuction_c* auctionActorController(u32 address) { return gabi::at<daAuction_c>(address); }
inline J3DModel* auctionModel(u32 address) { return gabi::at<J3DModel>(address); }
inline daPy_py_c* auctionPlayer(u32 address) { return gabi::at<daPy_py_c>(address); }
struct AuctionEmitter {
    u8 _0[0x5C];
    be<u32> mMaxFrame;
    u8 _60[0x247 - 0x60];
    be<u8> mGlobalAlpha;
    u8 _248[12];
    be<u32> mStatus;
};
WWHD_SIZE(AuctionEmitter, 0x258);
WWHD_OFFSET(AuctionEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(AuctionEmitter, mStatus, 0x254);
inline AuctionEmitter* auctionEmitter(u32 address) { return gabi::at<AuctionEmitter>(address); }
struct AuctionScreenController {
    u8 _0[0x18];
    u8 mQueue;
    u8 _19[3];
};
WWHD_SIZE(AuctionScreenController, 0x1C);
struct AuctionControllerManager {
    u8 _0[0x1F4];
    gptr<AuctionScreenController> mpController;
};
WWHD_SIZE(AuctionControllerManager, 0x1F8);
inline AuctionControllerManager* auctionControllers(u32 address) {
    return gabi::at<AuctionControllerManager>(address);
}
struct AuctionNpcPicon {
    u8 _0[0x894];
    be<f32> mPiconOfsY;
};
WWHD_SIZE(AuctionNpcPicon, 0x898);
inline AuctionNpcPicon* auctionNpc(u32 address) { return gabi::at<AuctionNpcPicon>(address); }
inline dSv_info_c* auctionSave(u32 address) { return gabi::at<dSv_info_c>(address + 0x20); }
struct AuctionSavedItem {
    u8 _0[0x11];
    be<u8> mAuctionItem;
};
WWHD_SIZE(AuctionSavedItem, 0x12);
inline AuctionSavedItem* auctionSavedItem(u32 address) {
    return gabi::at<AuctionSavedItem>(address);
}
struct AuctionMessage {
    u8 _0[0x948];
    be<u32> mSelectNum;
};
WWHD_SIZE(AuctionMessage, 0x94C);
inline AuctionMessage* auctionMessage(u32 address) { return gabi::at<AuctionMessage>(address); }

struct AuctionHIOState {
    be<f32> mAngleMin, mAngleMax, mBidMax[2], mGaugeMax[2];
    u8 mRoot, mNpc, _1A[2];
    be<u32> mRegistrationWords[4];
};
WWHD_SIZE(AuctionHIOState, 0x2C);
struct AuctionOpaqueObject {
    u8 _data;
};
WWHD_SIZE(AuctionOpaqueObject, 1);
struct AuctionProc {
    gabi::ptmf<daAuction_c::ProcFunc_t> fn;
};
WWHD_SIZE(AuctionProc, 8);
struct AuctionPlayerVtable {
    u8 _0[0x9C];
    be<u32> getBaseAnimeFrameRate;
};
WWHD_SIZE(AuctionPlayerVtable, 0xA0);
inline AuctionPlayerVtable* auctionPlayerVtable(u32 address) {
    return gabi::at<AuctionPlayerVtable>(address);
}
inline be<u32>* auctionInteger(u32 address) { return gabi::at<be<u32>>(address); }
struct AuctionIntegerWords {
    be<s16> high, low;
};
WWHD_SIZE(AuctionIntegerWords, 4);
inline AuctionIntegerWords* auctionIntegerWords(u32 address) {
    return gabi::at<AuctionIntegerWords>(address);
}
inline u8 auctionItemLowByte(u32 address) { return *gabi::at<be<u8>>(address + 1); }

struct AuctionCutNames {
    gptr<const char> names[11];
};
WWHD_SIZE(AuctionCutNames, 44);
