/* daNpc_Ds1_c (Doc Bandam, Windfall potion shop), WWHD layout.
 *
 * The GameCube TU is "Nonmatching" (its header only places ShopItems_c at 0x83C and the
 * ShopCursor_c* at 0x880; size 0x8A0). daNpc_Ds1_c derives from fopAc_ac_c directly (not
 * fopNpc_npc_c). Measured from the constructor 0222DF78 (allocates 0xA68) and the functions of
 * the TU. ShopItems_c (0x44) and the cursor pointer sit at 0xA04 / 0xA48 (GameCube + 0x1C8).
 * Names follow the GameCube member functions where the use is clear; others are mXXX. */
#pragma once
#include "bindings.h"

/* GHS pointer to member function {s16 this delta, s16 vtable index (0: NULL, < 0: not virtual),
 * u32 function (or, low half, the vtable offset)} */
struct ds1_ptmf {
    /* 0x0 */ be<s16> delta;
    /* 0x2 */ be<s16> idx;
    /* 0x4 */ be<u32> fn;
};
WWHD_SIZE(ds1_ptmf, 8);

/* ShopCam_action_c, HD 0x50 (constructor 025BBDF0; GameCube 0x58: the PTMF is 8 bytes, so the
 * GameCube members from 0x18 are at -8) */
struct ShopCam_action_c_l {
    /* 0x00 */ ds1_ptmf mCurrActionFunc;
    /* 0x08 */ u8 _08[0x10 - 0x08];
    /* 0x10 */ cXyz m18;
    /* 0x1C */ cXyz m24;
    /* 0x28 */ be<f32> m30;
    /* 0x2C */ cXyz mOrigCenter;
    /* 0x38 */ cXyz mOrigEye;
    /* 0x44 */ be<f32> mOrigFovy;
    /* 0x48 */ be<u16> m50;
    /* 0x4A */ be<u16> m52;
    /* 0x4C */ be<s16> m54;
    /* 0x4E */ be<s16> mCamDataIdx;
};
WWHD_SIZE(ShopCam_action_c_l, 0x50);

/* ShopItems_c, 0x44 (constructor 025BC760; GameCube layout) */
struct ShopItems_c_l {
    /* 0x00 */ be<s16> mSelectedItemIdx;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<u32> mItemActorProcessIds[8];
    /* 0x24 */ be<u32> mpItemSetList;
    /* 0x28 */ be<u8> mItemIsSoldOut[8];
    /* 0x30 */ cXyz m30;
    /* 0x3C */ be<s16> m3C;
    /* 0x3E */ be<s16> mbIsHide;
    /* 0x40 */ be<s16> mNumItems;
    /* 0x42 */ be<s16> mItemSetListGlobalIdx;
};
WWHD_SIZE(ShopItems_c_l, 0x44);

/* STControl, HD 0x28 (vtable at +0x24) */
struct STControl_l {
    u8 _[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(STControl_l, 0x28);

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
    /* 0x3BC */ u8 mBtpAnm[0x74];              /* mDoExt_btpAnm (HD 0x74) */
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
    /* 0x850 */ be<s16> m850;                   /* 0x850..0x870: dNpc_HeadAnm_c (swing_*_init 0259F514/0259F36C, this = +0x850) */
    /* 0x852 */ be<s16> m852;
    /* 0x854 */ be<s16> m854;
    /* 0x856 */ u8 _856[0x860 - 0x856];
    /* 0x860 */ be<f32> m860;
    /* 0x864 */ be<f32> m864;
    /* 0x868 */ be<s16> m868;
    /* 0x86A */ be<s16> m86A;
    /* 0x86C */ be<s16> m86C;
    /* 0x86E */ u8 _86E[2];
    /* 0x870 */ cXyz mMovePos;
    /* 0x87C */ be<s16> mMoveAngle;
    /* 0x87E */ u8 _87E[2];
    /* 0x880 */ STControl_l mStick;
    /* 0x8A8 */ be<u8> m8A8;
    /* 0x8A9 */ u8 _8A9[0x8B8 - 0x8A9];
    /* 0x8B8 */ cXyz mAttentionBasePos;
    /* 0x8C4 */ be<s16> mHeadAngleY;
    /* 0x8C6 */ csXyz mAngle;
    /* 0x8CC */ u8 _8CC[2];
    /* 0x8CE */ be<u8> mAnmEnd;
    /* 0x8CF */ be<u8> mAttnSetCount;
    /* 0x8D0 */ be<u8> m8D0;
    /* 0x8D1 */ be<u8> m8D1;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<f32> mPrevFrame;
    /* 0x8D8 */ be<u32> m8D8;                   /* JPABaseEmitter* (event cuts) */
    /* 0x8DC */ be<u32> m8DC;                   /* JPABaseEmitter* (event cuts) */
    /* 0x8E0 */ gptr<JPABaseEmitter> mpRoomEmitter[7];
    /* 0x8FC */ gptr<J3DModel> mpBtkModel;
    /* 0x900 */ mDoExt_btkAnm mBtkAnm;
    /* 0x974 */ LIGHT_INFLUENCE mLight;
    /* 0x998 */ be<u32> mMsgNo;
    /* 0x99C */ be<u32> m99C;
    /* 0x9A0 */ be<u32> m9A0;                   /* the end message number */
    /* 0x9A4 */ be<u32> m9A4;
    /* 0x9A8 */ be<u8> m9A8;
    /* 0x9A9 */ be<u8> mItemModelFlags;          /* bit 0: model 0 drawn in hand, bit 1: model 1 */
    /* 0x9AA */ be<u8> mbRoomEffect;
    /* 0x9AB */ be<u8> mSeTimer;
    /* 0x9AC */ ds1_ptmf mAction;
    /* 0x9B4 */ ShopCam_action_c_l mShopCam;
    /* 0xA04 */ ShopItems_c_l mShopItems;
    /* 0xA48 */ be<u32> mpShopCursor;
    /* 0xA4C */ be<s8> mTexAnmIdx;
    /* 0xA4D */ be<s8> mAnmIdx;
    /* 0xA4E */ be<u8> mA4E;                     /* s8 animation loop count */
    /* 0xA4F */ be<s8> mOrderType;
    /* 0xA50 */ be<s32> mA50;
    /* 0xA54 */ be<s16> mEventIdx[4];
    /* 0xA5C */ be<u8> mA5C;
    /* 0xA5D */ be<u8> mA5D;
    /* 0xA5E */ be<s8> mType;
    /* 0xA5F */ u8 _A5F;
    /* 0xA60 */ be<s8> mActionStatus;
    /* 0xA61 */ be<u8> mA61;
    /* 0xA62 */ be<u8> mA62;
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
struct dNpc_HIO_l {
    /* 0x00 */ be<f32> m00;
    /* 0x04 */ be<s16> mJntPrm[9];               /* dNpc_JntCtrl_c::setParam arguments */
    /* 0x16 */ be<s16> m16;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<u8> m1E;
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_l, 0x28);

/* daNpc_Ds1_childHIO_c (0x4C, constructor 02232B24) */
struct daNpc_Ds1_childHIO_c {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ dNpc_HIO_l mNpc;
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
inline daNpc_Ds1_HIO_c& l_HIO_ds1() { return *gabi::at<daNpc_Ds1_HIO_c>(0x10466F30); }

/* ---- addresses of this TU ---- */
#define DS1_SAFESTRING_VTBL 0x1001997C /* this TU's sead::SafeString vtable */
#define DS1_ARC STR(0x101BE240)        /* "Ds" */
enum : u32 {
    DS1_wait_action = 0x022311F4,
    DS1_getdemo_action = 0x022312F8,
    DS1_dummy_action = 0x0223164C,
    DS1_event_action = 0x022328E4,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
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
static inline BOOL dComIfGs_isEventBit_ds1(u16 f) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
