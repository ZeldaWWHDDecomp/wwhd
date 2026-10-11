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
    /* 850 */ be<u32> m738[8];
    /* 870 */ be<u32> mCurrAuctionItemPID;
    /* 874 */ be<u32> mTimerID;
    /* 878 */ be<u32> mpTimer;
    /* 87C */ AuctionLight mLight;
    /* 8A0 */ be<u32> mpModel;
    /* 8A4 */ cXyz m78C;
    /* 8B0 */ cXyz m798;
    /* 8BC */ be<u32> mpEmitter;
    /* 8C0 */ cXyz m7A8;
    /* 8CC */ u8 _8CC[8];
    /* 8D4 */ be<f32> mAlpha;
    /* 8D8 */ be<f32> mBlend;
    /* 8DC */ be<f32> m7C4[7];
    /* 8F8 */ be<f32> m7E0;
    /* 8FC */ be<f32> m7E4;
    /* 900 */ u8 _900[4];
    /* 904 */ be<s32> m7EC;
    /* 908 */ be<s32> m7F0;
    /* 90C */ be<s16> mEvtStartIdx;
    /* 90E */ be<s16> mEvtGetItemIdx;
    /* 910 */ be<s16> mEvtNoItemIdx;
    /* 912 */ be<s16> mEvtStart2Idx;
    /* 914 */ be<s16> mEvtEnd2Idx;
    /* 916 */ be<s16> mCurrBid;
    /* 918 */ be<s16> m800;
    /* 91A */ be<s16> mTimer;
    /* 91C */ be<u16> m804;
    /* 91E */ be<s16> m806;
    /* 920 */ be<s16> m808;
    /* 922 */ be<s16> m80A;
    /* 924 */ be<u8> m80C[8];
    /* 92C */ be<u8> mAucMdlNo[8];
    /* 934 */ be<u8> m81C;
    /* 935 */ be<u8> m81D;
    /* 936 */ be<u8> mMoveState;
    /* 937 */ be<u8> m81F;
    /* 938 */ be<u8> m820;
    /* 939 */ be<u8> m821;
    /* 93A */ be<u8> m822;
    /* 93B */ be<u8> mCurrAuctionItemIndex;
    /* 93C */ be<u8> m824;
    /* 93D */ be<u8> m825;
    /* 93E */ be<u8> m826;
    /* 93F */ be<u8> m827;
    /* 940 */ be<u8> m828;
    /* 941 */ be<u8> m829;
    /* 942 */ be<u8> m82A;
    /* 943 */ be<u8> m82B;
    /* 944 */ be<u8> m82C;
    /* 945 */ be<u8> mCurLinkAnm;
    /* 946 */ be<u8> m82E;
    /* 947 */ be<u8> m82F;
    /* 948 */ be<u8> m830;
    /* 949 */ be<u8> m831;
    /* 94A */ be<u8> m832;
    /* 94B */ be<u8> mFace;
    /* 94C */ be<u8> m834;
    /* 94D */ be<u8> m835;
    /* 94E */ be<u8> m836;
    /* 94F */ be<u8> mFlags;
    /* 950 */ be<s8> m838;
    /* 951 */ be<s8> mAction;
    /* 952 */ u8 _952[2];
};
WWHD_SIZE(daAuction_c, 0x954);
WWHD_OFFSET(daAuction_c, mNpcEvtInfo, 0x7E4);
WWHD_OFFSET(daAuction_c, mLight, 0x87C);
WWHD_OFFSET(daAuction_c, mpModel, 0x8A0);
WWHD_OFFSET(daAuction_c, m835, 0x94D);

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
    be<f32> field_0x00, field_0x04;
    be<s16> mBidIncMin, mBidIncMax, mSpendingLimit;
    u8 _pad[2];
};
WWHD_SIZE(NpcDatStruct, 0x10);
WWHD_OFFSET(NpcDatStruct, mSpendingLimit, 0xC);
struct NpcMsgDatStruct {
    be<s16> field_0x00, field_0x02, field_0x04, field_0x06, field_0x08;
};
WWHD_SIZE(NpcMsgDatStruct, 0xA);
struct NpcCameraDatStruct {
    be<f32> field_0x00, field_0x04;
    be<s16> field_0x08, field_0x0A;
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
