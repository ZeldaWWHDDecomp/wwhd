/* daNpc_Ds1_c (Doc Bandam, Windfall potion shop), WWHD layout.
 *
 * The GameCube TU is "Nonmatching" (its header only places ShopItems_c at 0x83C and the
 * ShopCursor_c* at 0x880; size 0x8A0). daNpc_Ds1_c derives from fopAc_ac_c directly (not
 * fopNpc_npc_c). Measured from the constructor 0222DF78 (allocates 0xA68) and the functions of
 * the TU. ShopItems_c (0x44) and the cursor pointer sit at 0xA04 / 0xA48 (GameCube + 0x1C8).
 * Names follow the GameCube member functions where the use is clear; others are mXXX. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/d_shop.h"
#include "d/d_lib.h"
#include "d/actor/d_a_player.h"

// HD: these shop bottle codes precede the inventory potion item codes.
enum Ds1PotionCode {
    DS1_RED_POTION = 0x49,
    DS1_GREEN_POTION = 0x4A,
    DS1_BLUE_POTION = 0x4B,
};
enum Ds1EventFlag {
    DS1_GREEN_POTION_UNLOCKED = 0xD04,
    DS1_BLUE_POTION_UNLOCKED = 0xD02,
};

struct Ds1TuData {
    u32 mActorVtable, mChildHioVtable, mHioVtable, mStickVtable, mAabVtable;
    u32 mWaitAction, mGetdemoAction, mDummyAction, mEventAction;
};
extern const Ds1TuData ds1_tu;

/* GHS pointer to member function {s16 this delta, s16 vtable index (0: NULL, < 0: not virtual),
 * u32 function (or, low half, the vtable offset)} */
struct ds1_ptmf {
    /* 0x0 */ be<s16> delta;
    /* 0x2 */ be<s16> idx;
    /* 0x4 */ be<u32> fn;
};
WWHD_SIZE(ds1_ptmf, 8);




struct daNpc_Ds1_c : fopAc_ac_c {
    /* action functions (GHS PTMF targets) */
    BOOL wait_action(void*);
    BOOL getdemo_action(void*);
    BOOL dummy_action(void*);
    BOOL event_action(void*);

    s16 XyEventCB(int);
    BOOL initTexPatternAnm(u8);
    BOOL CreateHeap();
    void RoomEffectSet();
    BOOL CreateInit();
    cPhs_State _create();
    void RoomEffectDelete();
    BOOL _delete();
    void playTexPatternAnm();
    void talkInit();
    void checkOrder();
    void eventOrder();
    void setCollision();
    BOOL _execute();
    BOOL _draw();
    void setAnm(s8, f32);
    u32 setTexAnm(s8);
    void setAnmFromMsgTag();
    bool chkAttention(cXyz*, s16);
    u16 next_msgStatus(be<u32>*, be<u32>*);
    u32 getMsg();
    u16 normal_talk();
    u16 shop_talk();
    u16 talk();
    void setAttention(u32);
    void lookBack();
    BOOL wait01();
    BOOL talk01();
    BOOL evn_talk_init(int);
    BOOL evn_continue_talk_init(int);
    BOOL evn_ItemModel_init(int);
    BOOL evn_head_swing_init(int);
    BOOL evn_setAnm_init(int);
    BOOL evn_move_pos_init(int);
    BOOL evn_init_pos_init(int);
    BOOL evn_jnt_lock_init(int);
    BOOL evn_player_hide_init(int);
    BOOL evn_talk();
    BOOL evn_Anm();
    BOOL evn_move_pos();
    BOOL privateCut();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DAnmTexPattern> mpBtp;
    /* 0x3BC */ mDoExt_btpAnm mBtpAnm;              /* mDoExt_btpAnm (HD 0x74) */
    /* 0x430 */ be<u8> mBtpFrame;
    /* 0x431 */ u8 _431;
    /* 0x432 */ be<s16> mBlinkTimer;
    /* 0x434 */ gptr<J3DModel> mpItemModel[2];  /* hand-held models (resources 0x17, 0x18) */
    /* 0x43C */ dBgS_ObjAcch mAcch;
    /* 0x600 */ dBgS_AcchCir mAcchCir;
    /* 0x640 */ dCcD_Stts mStts;
    /* 0x67C */ dCcD_Cyl mCyl;
    /* 0x7AC */ be<s8> mJntNo[4];             /* head, backbone, and the two item joints */
    /* 0x7B0 */ dNpc_EventCut_c mEventCut;
    /* 0x81C */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x850 */ dNpc_HeadAnm_c mHeadAnm;
    /* 0x870 */ cXyz mMovePos;
    /* 0x87C */ be<s16> mMoveAngle;
    /* 0x87E */ u8 _87E[2];
    /* 0x880 */ STControl mStick;
    /* 0x8A8 */ be<u8> mShopInputHandled;
    /* 0x8A9 */ u8 _8A9[0x8B8 - 0x8A9];
    /* 0x8B8 */ cXyz mAttentionBasePos;
    /* 0x8C4 */ be<s16> mHeadAngleY;
    /* 0x8C6 */ csXyz mAngle;
    /* 0x8CC */ u8 _8CC[2];
    /* 0x8CE */ be<u8> mAnmEnd;
    /* 0x8CF */ be<u8> mAttnSetCount;
    /* 0x8D0 */ be<u8> mPlayerInRange;
    /* 0x8D1 */ be<u8> mIsTalking;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<f32> mPrevFrame;
    /* 0x8D8 */ be<u32> mMixEmitter;                   /* JPABaseEmitter* (event cuts) */
    /* 0x8DC */ be<u32> mMixChildEmitter;                   /* JPABaseEmitter* (event cuts) */
    /* 0x8E0 */ gptr<JPABaseEmitter> mpRoomEmitter[7];
    /* 0x8FC */ gptr<J3DModel> mpBtkModel;
    /* 0x900 */ mDoExt_btkAnm mBtkAnm;
    /* 0x974 */ LIGHT_INFLUENCE mLight;
    /* 0x998 */ be<u32> mMsgNo;
    /* 0x99C */ be<u32> mNextMsgNo;
    /* 0x9A0 */ be<u32> mEndMsgNo;                   /* the end message number */
    /* 0x9A4 */ be<u32> mPendingMsgNo;
    /* 0x9A8 */ be<u8> mBoughtItemNo;
    /* 0x9A9 */ be<u8> mItemModelFlags;          /* bit 0: model 0 drawn in hand, bit 1: model 1 */
    /* 0x9AA */ be<u8> mbRoomEffect;
    /* 0x9AB */ be<u8> mSeTimer;
    /* 0x9AC */ ds1_ptmf mAction;
    /* 0x9B4 */ ShopCam_action_c mShopCam;
    /* 0xA04 */ ShopItems_c mShopItems;
    /* 0xA48 */ be<u32> mpShopCursor;
    /* 0xA4C */ be<s8> mTexAnmIdx;
    /* 0xA4D */ be<s8> mAnmIdx;
    /* 0xA4E */ be<u8> mAnmLoopCount;                     /* s8 animation loop count */
    /* 0xA4F */ be<s8> mOrderType;
    /* 0xA50 */ be<s32> mEventActionIdx;
    /* 0xA54 */ be<s16> mEventIdx[4];
    /* 0xA5C */ be<u8> mWaitState;
    /* 0xA5D */ be<u8> mPreviousWaitState;
    /* 0xA5E */ be<s8> mType;
    /* 0xA5F */ u8 _A5F;
    /* 0xA60 */ be<s8> mActionStatus;
    /* 0xA61 */ be<u8> mTalkState;
    /* 0xA62 */ be<u8> mShopState;
    /* 0xA63 */ be<u8> mItemNo;
    /* 0xA64 */ u8 _A64[4];
};
WWHD_OFFSET(daNpc_Ds1_c, mAcch, 0x43C);
WWHD_OFFSET(daNpc_Ds1_c, mCyl, 0x67C);
WWHD_OFFSET(daNpc_Ds1_c, mEventCut, 0x7B0);
WWHD_OFFSET(daNpc_Ds1_c, mJntCtrl, 0x81C);
WWHD_OFFSET(daNpc_Ds1_c, mStick, 0x880);
WWHD_OFFSET(daNpc_Ds1_c, mBtkAnm, 0x900);
WWHD_OFFSET(daNpc_Ds1_c, mLight, 0x974);
WWHD_OFFSET(daNpc_Ds1_c, mAction, 0x9AC);
WWHD_OFFSET(daNpc_Ds1_c, mShopItems, 0xA04);
WWHD_OFFSET(daNpc_Ds1_c, mItemNo, 0xA63);
WWHD_SIZE(daNpc_Ds1_c, 0xA68);

/* dNpc_HIO_c? (0x28, constructor 0259DA18): a float, 10 s16, a float, s16, u8, a float, vtable */

/* daNpc_Ds1_childHIO_c (0x4C, constructor 02232B24) */
struct daNpc_Ds1_childHIO_c {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<f32> mCursorScale;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<f32> m44;
    /* 0x48 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Ds1_childHIO_c, 0x4C);

/* daNpc_Ds1_HIO_c (0x58, constructor 02232B9C; l_HIO at 0x10466F30). HD: +4 counts the actors
 * using it (the child is created by the first and deleted by the last) */
struct daNpc_Ds1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> mCount;
    /* 0x08 */ daNpc_Ds1_childHIO_c mChild[1];
    /* 0x54 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Ds1_HIO_c, 0x58);

BOOL ds1Guest_wait_action(daNpc_Ds1_c*, void*);
BOOL ds1Guest_getdemo_action(daNpc_Ds1_c*, void*);
BOOL ds1Guest_dummy_action(daNpc_Ds1_c*, void*);
BOOL ds1Guest_event_action(daNpc_Ds1_c*, void*);
#define DS1_SAFESTRING_VTBL gc_tu.safeString
#define DS1_ARC "Ds"

/* GHS pointer-to-member call with one argument (the action functions take a void*) */
static inline BOOL ds1_ptmf_call(ds1_ptmf* f, void* self, void* arg) {
    s16 idx = f->idx;
    u32 p = gabi::ea(self) + (s32)(s16)f->delta;
    if (idx < 0) return gabi::call_ptr<BOOL>(f->fn, p, arg);
    u32 vt = gabi::load<u32>(p + gabi::load<s16>(gabi::ea(f) + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + idx * 8 + 4), p, arg);
}
/* daNpc_Ds1_c::setAction (inline): if the action changes, the old one runs once with status -1,
 * then the new one runs with status 0 (called directly: a non-virtual member of this class) */
static inline void ds1_setAction(daNpc_Ds1_c* t, u32 fn, void* arg) {
    ds1_ptmf* a = &t->mAction;
    s16 idx = a->idx;
    if (idx == -1) {
        if (a->delta == 0 && a->fn == fn) return;
    } else if (idx == 0) {
        t->mActionStatus = 0;
        a->fn = fn;
        a->idx = -1;
        a->delta = 0;
        gabi::call_ptr<BOOL>(a->fn, t, arg);
        return;
    }
    t->mActionStatus = -1;
    ds1_ptmf_call(a, t, arg);
    t->mActionStatus = 0;
    a->delta = 0;
    a->idx = -1;
    a->fn = fn;
    gabi::call_ptr<BOOL>(a->fn, t, arg);
}
static inline void* ds1_getRes(s32 idx) { return dComIfG_getObjectRes(DS1_ARC, idx, DS1_SAFESTRING_VTBL); }

// HD: unit-private signatures preserve the verified callers' register widths.
u32 ds1Guest_fopMsgM_SearchByID(u32);
u32 ds1Guest_fopMsgM_messageSet(u32, u32, cXyz*);
void ds1Guest_fopMsgM_setStatus(u32, u32);
void ds1Guest_fopMsgM_demoMsgFlagOn();
BOOL ds1Guest_CPad_CHECK_TRIG_A(s32);
u8 ds1Guest_dShop_BoughtErrorStatus(ShopItems_c*, s32, s32);
BOOL ds1Guest_dShop_now_triggercheck(STControl*, ShopItems_c*, be<u32>*, u32, u32);
u8 ds1Guest_ShopItems_getSelectItemNo(ShopItems_c*);
void ds1Guest_ShopItems_hideSelectItem(ShopItems_c*);
void ds1Guest_ShopItems_showItem(ShopItems_c*);
void ds1Guest_ShopItems_Item_ZoomUp(ShopItems_c*, cXyz*);
u32 ds1Guest_ShopItems_getSelectItemBuyMsg(ShopItems_c*);
void ds1Guest_ShopItems_getSelectItemPos(ShopItems_c*, cXyz*);
void ds1Guest_ShopItems_getSelectItemBasePos(ShopItems_c*, cXyz*);
void ds1Guest_ShopCursor_setPos(u32, cXyz*);
void ds1Guest_ShopCursor_anm_play(u32);
BOOL ds1Guest_checkItemGet(u8, s32);
void ds1Guest_execItemGet(u8);
bool ds1Guest_cXyz_normalizeRS(cXyz*);
void ds1Guest_dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
void ds1Guest_dNpc_playerEyePos(cXyz*, f32);
void ds1Guest_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
BOOL ds1Guest_J3DFrameCtrl_checkPass(J3DFrameCtrl*, f32);
BOOL ds1Guest_daNpc_Ds1_shopMsgCheck(u32);
BOOL ds1Guest_daNpc_Ds1_checkCreateDrugChuchu(u8);
u32 ds1Guest_fopAcM_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void ds1Guest_dNpc_HeadAnm_swing_horizone_init(void*, s16, s16, s16, s32);
void ds1Guest_dNpc_HeadAnm_swing_vertical_init(void*, s16, s16, s16, s32);
void ds1Guest_cLib_addCalcPos2(cXyz*, const cXyz*, f32, f32);
s8 ds1Guest_JUTNameTab_getIndex(u32, const char*);
s16 ds1Guest_cLib_calcTimer(be<s16>*);
BOOL ds1Guest_mDoExt_btpAnm_init(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32);
void ds1Guest_mDoExt_btpAnm_ct(void*);
u32 ds1Guest_ShopCursor_create(J3DModelData*, void*, f32);
void ds1Guest_ShopCursor_draw(u32);
void ds1Guest_ShopItems_setItemSetDataList(ShopItems_c*);
void ds1Guest_ShopItems_createItem(ShopItems_c*, s32, s32);
void ds1Guest_ShopItems_Item_Move(ShopItems_c*);
void ds1Guest_ShopCam_shop_cam_action_init(ShopCam_action_c*);
void ds1Guest_ShopCam_ds_normal_cam_action_init(ShopCam_action_c*);
void ds1Guest_ShopCam_move(ShopCam_action_c*);
void ds1Guest_dNpc_EventCut_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*);
u32 ds1Guest_dBgS_GetMtrlSndId(dBgS*, void*);
s8 ds1Guest_dBgS_GetRoomId(dBgS*, void*);
u8 ds1Guest_dBgS_GetPolyColor(dBgS*, void*);
BOOL ds1Guest_McaMorf_play(mDoExt_McaMorf*, cXyz*, u32, s32);
void ds1Guest_dNpc_setAnm_2(mDoExt_McaMorf*, s32, f32, f32, s32, s32, const char*);
void ds1Guest_STControl_setWaitParm(STControl*, s16, s16, s16, s16, f32, f32, s16, s16);
void ds1Guest_STControl_init(STControl*);
f32 ds1Guest_std_powf(f32, f32);
s16 ds1Guest_XyEventCB(daNpc_Ds1_c*, int);
BOOL ds1Guest_initTexPatternAnm(daNpc_Ds1_c*, u8);
BOOL ds1Guest_CreateHeap(daNpc_Ds1_c*);
void ds1Guest_RoomEffectSet(daNpc_Ds1_c*);
BOOL ds1Guest_CreateInit(daNpc_Ds1_c*);
cPhs_State ds1Guest__create(daNpc_Ds1_c*);
void ds1Guest_RoomEffectDelete(daNpc_Ds1_c*);
BOOL ds1Guest__delete(daNpc_Ds1_c*);
void ds1Guest_playTexPatternAnm(daNpc_Ds1_c*);
void ds1Guest_talkInit(daNpc_Ds1_c*);
void ds1Guest_checkOrder(daNpc_Ds1_c*);
void ds1Guest_eventOrder(daNpc_Ds1_c*);
void ds1Guest_setCollision(daNpc_Ds1_c*);
BOOL ds1Guest__execute(daNpc_Ds1_c*);
BOOL ds1Guest__draw(daNpc_Ds1_c*);
void ds1Guest_setAnm(daNpc_Ds1_c*, s8, f32);
u32 ds1Guest_setTexAnm(daNpc_Ds1_c*, s8);
void ds1Guest_setAnmFromMsgTag(daNpc_Ds1_c*);
bool ds1Guest_chkAttention(daNpc_Ds1_c*, cXyz*, s16);
u16 ds1Guest_next_msgStatus(daNpc_Ds1_c*, be<u32>*, be<u32>*);
u32 ds1Guest_getMsg(daNpc_Ds1_c*);
u16 ds1Guest_normal_talk(daNpc_Ds1_c*);
u16 ds1Guest_shop_talk(daNpc_Ds1_c*);
u16 ds1Guest_talk(daNpc_Ds1_c*);
void ds1Guest_setAttention(daNpc_Ds1_c*, u32);
void ds1Guest_lookBack(daNpc_Ds1_c*);
BOOL ds1Guest_wait01(daNpc_Ds1_c*);
BOOL ds1Guest_talk01(daNpc_Ds1_c*);
BOOL ds1Guest_evn_talk_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_continue_talk_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_ItemModel_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_head_swing_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_setAnm_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_move_pos_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_init_pos_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_jnt_lock_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_player_hide_init(daNpc_Ds1_c*, int);
BOOL ds1Guest_evn_talk(daNpc_Ds1_c*);
BOOL ds1Guest_evn_Anm(daNpc_Ds1_c*);
BOOL ds1Guest_evn_move_pos(daNpc_Ds1_c*);
BOOL ds1Guest_privateCut(daNpc_Ds1_c*);

struct Ds1Emitter {
    u8 paddingMaxFrame[0x5C];
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 paddingTranslation[0x22C - 0x62];
    cXyz mGlobalTranslation;
    u8 paddingFlags[0x254 - 0x238];
    be<u32> mStatus;
    u8 paddingCoordinate[0x262 - 0x258];
    be<u8> mCoordinateSystem;
};
WWHD_OFFSET(Ds1Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Ds1Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Ds1Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Ds1Emitter, mStatus, 0x254);
WWHD_OFFSET(Ds1Emitter, mCoordinateSystem, 0x262);
struct Ds1MessageManager {
    u8 paddingTrigger[0x922];
    be<u8> mTriggerHandled;
    u8 paddingMessage[0x938 - 0x923];
    be<u32> mMessageNo;
    u8 paddingSelection[0x948 - 0x93C];
    be<u32> mSelectionFlag;
};
WWHD_OFFSET(Ds1MessageManager, mTriggerHandled, 0x922);
WWHD_OFFSET(Ds1MessageManager, mMessageNo, 0x938);
WWHD_OFFSET(Ds1MessageManager, mSelectionFlag, 0x948);
struct Ds1Word { be<u32> mValue; };
struct Ds1MessageID { be<s32> mValue; };
struct Ds1SoundPositions { cXyz mPositions[4]; };
struct Ds1BtpTable { be<s32> mResources[2]; };
struct Ds1AnmIntTable { be<s32> mValues[10]; };
struct Ds1AnmFloatTable { be<f32> mValues[10]; };
struct Ds1CutNames { be<u32> mNames[10]; };
struct Ds1Colors { GXColor mColors[3]; };
WWHD_SIZE(Ds1Word, 4);
WWHD_SIZE(Ds1MessageID, 4);
WWHD_SIZE(Ds1SoundPositions, 48);
WWHD_SIZE(Ds1BtpTable, 8);
WWHD_SIZE(Ds1AnmIntTable, 40);
WWHD_SIZE(Ds1AnmFloatTable, 40);
WWHD_SIZE(Ds1CutNames, 40);
WWHD_SIZE(Ds1Colors, 12);
extern daNpc_Ds1_HIO_c& l_HIO;
extern Ds1Word& l_messageManager;
extern Ds1MessageID& l_messageID;
extern Ds1SoundPositions& l_soundPositions;
extern dCcD_SrcCyl& l_cylinderSource;
extern Ds1BtpTable& l_btpResources;
extern Ds1AnmIntTable& l_anmLoop;
extern Ds1AnmIntTable& l_anmResources;
extern Ds1AnmFloatTable& l_anmMorph;
extern Ds1AnmFloatTable& l_anmSpeed;
extern Ds1CutNames& l_cutNames;
extern Ds1Colors& l_particlePrmColors;
extern Ds1Colors& l_particleEnvColors;
extern Ds1Word& l_prmColorGuard;
extern Ds1Word& l_envColorGuard;
extern Mtx34& l_currentMatrix;
extern Mtx34& l_identityMatrix;
extern Ds1Word& l_currentModel;

void ds1Guest_Circle_destruct(void*, s32);
BOOL ds1Guest_createDrugRegister(u32);
f32 ds1Guest_gammaCorrect(u32);
u32 ds1Guest_attentionRegister(daNpc_Ds1_c*, cXyz*, s16);
u32 ds1Guest_nextMessageStatusRegister(daNpc_Ds1_c*, be<u32>*, be<u32>*);
void ds1Guest_Acch_destruct(dBgS_Acch*, s32);
s32 ds1Guest_eventActionIndex(void*, s32, u32, s32, s32, s32);
u8 ds1Guest_eventAdvanced(void*, s32);
void ds1Guest_Stick_destruct(STControl*, s32);
void ds1Guest_NpcHio_construct(dNpc_HIO_c*);
void ds1Guest_JntCtrl_construct(dNpc_JntCtrl_c*);
void ds1Guest_EventCut_construct(dNpc_EventCut_c*);
void ds1Guest_ShopCam_construct(ShopCam_action_c*);
void ds1Guest_ShopItems_construct(ShopItems_c*);
void ds1Guest_registerFigure(u32, daNpc_Ds1_c*, cXyz*, s16, f32, f32, f32);
void ds1Guest_Actor_destruct(daNpc_Ds1_c*, s32);
void ds1Guest_constructArray(daNpc_Ds1_childHIO_c*, s32, u32, u32);

// HD: integer-channel copies preserve the native bit pattern, including signaling NaNs.
struct Ds1VectorWords { be<u32> x, y, z; };
WWHD_SIZE(Ds1VectorWords, 12);
inline Ds1VectorWords& ds1VectorWords(cXyz* value) {
    return *reinterpret_cast<Ds1VectorWords*>(value);
}
struct Ds1JointNumber { u8 padding[4]; be<u16> mJointNo; };
WWHD_OFFSET(Ds1JointNumber, mJointNo, 4);
struct Ds1AnimationVtable { u8 padding[0x14]; gfn mFrameMax; };
struct Ds1Animation { be<f32> mFrame; gptr<Ds1AnimationVtable> mVtable; };

struct Ds1ModelAnimation { u8 padding[0x38]; be<u32> mBtpAnimation; };
WWHD_OFFSET(Ds1ModelAnimation, mBtpAnimation, 0x38);
