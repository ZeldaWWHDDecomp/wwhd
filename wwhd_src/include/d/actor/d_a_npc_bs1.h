/* daNpc_Bs1_c (Beedle, boat shop), WWHD layout. 
 *
 * GameCube -> WWHD, measured from _create (02213610, inline constructor), CreateHeap and the HIO
 * constructors: daNpc_Bs1_c derives from fopAc_ac_c directly (+0x11C). GameCube m29C is gone, so
 * the three sold-sign models start at 0x3B8 (+0x118); mDoExt_btpAnm grew from 0x14 to 0x74, so
 * mFrame.. is +0x178; dBgS_ObjAcch 0x1C4, dBgS_AcchCir 0x40, dCcD_Stts 0x3C, dCcD_Cyl 0x130 as
 * GameCube, so everything up to the action pointer to member is +0x178; the pointer to member is
 * 8 bytes (GameCube 12), so the members from ShopCam_action_c on are +0x174; ShopCam_action_c is
 * 0x50 (GameCube 0x58), so the members after it are +0x16C. Size 0x9B0 (GameCube 0x844). */
#pragma once
#include "bindings.h"

/* ---- shared NPC / shop classes, WWHD layouts (SHARED-CANDIDATE; same as d_a_npc_bms1.h) ---- */

/* STControl (HD 0x28: GameCube members 0x04..0x28 at 0x00, the vtable at +0x24) */
struct STControl_l {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(STControl_l, 0x28);

/* ShopCam_action_c (HD 0x50, constructor 025BBDF0): the action pointer to member (8 bytes) and
 * 8 more bytes, then GameCube m18.. at -8 */
struct ShopCam_action_c_l {
    /* 0x00 */ u8 mAction[8];
    /* 0x08 */ u8 _08[8];
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
    void Reset() { gabi::call(0x025BC620, this); }                       /* 025BC620 */
    void Save() { gabi::call(0x025BC5B0, this); }                        /* 025BC5B0 */
    BOOL shop_cam_action_init() { return gabi::call<BOOL>(0x025BBE98, this); } /* 025BBE98 */
    void move() { gabi::call(0x025BC714, this); }                        /* 025BC714 */
};
WWHD_SIZE(ShopCam_action_c_l, 0x50);

/* ShopItems_c (HD 0x44 as GameCube, constructor 025BC760) */
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
    void setItemSetDataList() { gabi::call(0x025BCF84, this); }              /* 025BCF84 */
    void createItem(s32 a, s32 room) { gabi::call(0x025BC810, this, a, room); } /* 025BC810 */
    void Item_Move() { gabi::call(0x025BCCA4, this); }                       /* 025BCCA4 */
    BOOL Item_ZoomUp(cXyz* pos) { return gabi::call<BOOL>(0x025BCC7C, this, pos); } /* 025BCC7C (cXyz&) */
    void hideSelectItem() { gabi::call(0x025BCD90, this); }                  /* 025BCD90 */
    u8 getSelectItemNo() { return gabi::call<u8>(0x025BD22C, this); }        /* 025BD22C */
    void showItem() { gabi::call(0x025BCE04, this); }                        /* 025BCE04 */
    u32 getSelectItemBuyMsg() { return gabi::call<u32>(0x025BD278, this); }  /* 025BD278 */
    void getSelectItemPos(cXyz* out) { gabi::call(0x025BCFA8, this, out); }     /* 025BCFA8 */
    void getSelectItemBasePos(cXyz* out) { gabi::call(0x025BD0EC, this, out); } /* 025BD0EC */
};
WWHD_SIZE(ShopItems_c_l, 0x44);

/* ShopCursor_c: only the members the NPCs touch (show/hide flag +0xB4, setScale +0x98..+0xB0) */
struct ShopCursor_c_l {
    /* 0x00 */ u8 _00[0x98];
    /* 0x98 */ be<f32> m98;
    /* 0x9C */ be<f32> m9C;
    /* 0xA0 */ u8 _A0[8];
    /* 0xA8 */ be<f32> mA8;
    /* 0xAC */ be<f32> mAC;
    /* 0xB0 */ be<f32> mB0;
    /* 0xB4 */ be<u8> mbShow;
    void setPos(cXyz* p) { gabi::call(0x025BD31C, this, p); }   /* 025BD31C (cXyz&) */
    void anm_play() { gabi::call(0x025BD290, this); }           /* 025BD290 */
    void draw() { gabi::call(0x025BD338, this); }               /* 025BD338 */
};

/* GHS pointer to member function (8 bytes): {s16 this delta, s16 vtable index (0 NULL, < 0 plain),
 * u32 function} */
struct Bs1ActionFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(Bs1ActionFunc_l, 8);

/* daNpc_Bs1_childHIO_c (HD 0x48): +0 not written, dNpc_HIO_c members at +4 (its vtable +0x28),
 * own members +0x2C.., vtable +0x44 */
struct daNpc_Bs1_childHIO_c {
    /* 0x00 */ be<u32> unk0;
    /* 0x04 */ be<f32> m04;                /* dNpc_HIO_c: playerEyePos offset */
    /* 0x08 */ be<s16> mMaxHeadX;
    /* 0x0A */ be<s16> mMaxBackboneX;
    /* 0x0C */ be<s16> mMaxHeadY;
    /* 0x0E */ be<s16> mMaxBackboneY;
    /* 0x10 */ be<s16> mMinHeadX;
    /* 0x12 */ be<s16> mMinBackboneX;
    /* 0x14 */ be<s16> mMinHeadY;
    /* 0x16 */ be<s16> mMinBackboneY;
    /* 0x18 */ be<s16> mMaxTurnStep;
    /* 0x1A */ be<s16> mMaxHeadTurnVel;
    /* 0x1C */ be<f32> mAttnYOffset;
    /* 0x20 */ be<s16> mMaxAttnAngleY;
    /* 0x22 */ be<u8> m22;
    /* 0x23 */ u8 _23;
    /* 0x24 */ be<f32> mMaxAttnDistXZ;
    /* 0x28 */ be<u32> npc_vtbl;
    /* 0x2C */ be<u8> m2C;
    /* 0x2D */ u8 _2D[3];
    /* 0x30 */ be<f32> m30;                /* ShopCursor scale .. */
    /* 0x34 */ be<f32> m34;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Bs1_childHIO_c, 0x48);

/* daNpc_Bs1_HIO_c (HD 0x9C): mNo +0, m8 +4, two children +8, vtable +0x98. l_HIO at 0x10466878 */
struct daNpc_Bs1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m8;
    /* 0x08 */ daNpc_Bs1_childHIO_c mChild[2];
    /* 0x98 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Bs1_HIO_c, 0x9C);

struct daNpc_Bs1_c : fopAc_ac_c {
    enum { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };

    s16 XyEventCB(int);
    BOOL initTexPatternAnm(u32 i_modify); /* bool, passed as the full register */
    void playTexPatternAnm();
    void setAnm(s8);
    u32 setTexAnm(s8);
    void setAnmFromMsgTag();
    BOOL chkAttention(cXyz* i_pos, s16 i_angle); /* cXyz by value: GHS passes a pointer to a copy */
    void eventOrder();
    void checkOrder();
    u16 next_msgStatus(be<u32>*, be<u32>*); /* HD: a second, nullable msgNo pointer in r5 */
    u32 getMsg();
    void setCollision();
    void talkInit();
    BOOL shopMsgCheck(u32);
    u32 getDefaultMsg();
    BOOL shopStickMoveMsgCheck(u32);
    BOOL checkBeastItemSellMsg(u32);
    u16 normal_talk();
    u16 shop_talk();
    u16 talk();
    void createShopList();
    BOOL isSellBomb();
    BOOL CreateInit();
    void setAttention(bool);
    void lookBack();
    bool wait01();
    bool talk01();
    BOOL wait_action(void*);
    BOOL getdemo_action(void*);
    BOOL evn_talk_init(int);
    BOOL evn_continue_talk_init(int);
    BOOL evn_talk();
    BOOL evn_jnt_lock_init(int);
    BOOL evn_wait_init(int);
    BOOL evn_wait();
    BOOL evn_set_anm_init(int);
    BOOL evn_praise_init();
    BOOL evn_mantan_init();
    BOOL privateCut();
    BOOL event_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DModel> mpSoldSignModels[3]; /* HD: GameCube m29C is gone */
    /* 0x3C4 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x3C8 */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74) */
    /* 0x43C */ be<u8> mFrame;
    /* 0x43D */ be<u8> m2C5;
    /* 0x43E */ be<s16> m2C6;
    /* 0x440 */ gptr<J3DModel> mpHelmetModel;
    /* 0x444 */ dBgS_ObjAcch mAcch;
    /* 0x608 */ dBgS_AcchCir mAcchCir;
    /* 0x648 */ dCcD_Stts mStts;
    /* 0x684 */ dCcD_Cyl mCyl;
    /* 0x7B4 */ be<s8> m_head_jnt_num;
    /* 0x7B5 */ be<s8> m_backbone_jnt_num;
    /* 0x7B6 */ be<s16> m63E;
    /* 0x7B8 */ dNpc_EventCut_c mEventCut;
    /* 0x824 */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x858 */ STControl_l mStickControl;
    /* 0x880 */ be<s8> m708;
    /* 0x881 */ u8 m70C[0x890 - 0x881];
    /* 0x890 */ cXyz m718;
    /* 0x89C */ be<s16> m724;
    /* 0x89E */ csXyz m726;
    /* 0x8A4 */ be<u16> m72C;
    /* 0x8A6 */ be<u8> m72E;
    /* 0x8A7 */ be<u8> m72F;
    /* 0x8A8 */ be<u8> m730;
    /* 0x8A9 */ be<u8> m731;
    /* 0x8AA */ be<u8> m732;
    /* 0x8AB */ be<u8> m733;
    /* 0x8AC */ be<f32> m734;
    /* 0x8B0 */ be<u32> m738;
    /* 0x8B4 */ be<u32> m73C;
    /* 0x8B8 */ be<u32> m740;
    /* 0x8BC */ be<u32> m744;
    /* 0x8C0 */ cXyz mItemPosOffsets[3];
    /* 0x8E4 */ be<u8> m76C[3];
    /* 0x8E7 */ u8 m76F;
    /* 0x8E8 */ Bs1ActionFunc_l mCurrActionFunc;
    /* 0x8F0 */ ShopCam_action_c_l mShopCamAction;
    /* 0x940 */ ShopItems_c_l mShopItems;
    /* 0x984 */ gptr<ShopCursor_c_l> mpShopCursor;
    /* 0x988 */ be<u32> mpItemSetList[3];
    /* 0x994 */ be<s8> m828;
    /* 0x995 */ be<s8> m829;
    /* 0x996 */ be<s8> m82A;
    /* 0x997 */ be<s8> m82B;
    /* 0x998 */ be<s16> mEventIdxs[2];
    /* 0x99C */ be<s8> m830;
    /* 0x99D */ be<u8> m831;
    /* 0x99E */ be<s8> mType;
    /* 0x99F */ be<s8> mShopIndex;
    /* 0x9A0 */ be<s8> mActionStatus;
    /* 0x9A1 */ be<s8> m835;
    /* 0x9A2 */ be<s8> m836;
    /* 0x9A3 */ be<s8> m837;
    /* 0x9A4 */ be<s8> m838;
    /* 0x9A5 */ be<u8> m839;
    /* 0x9A6 */ be<s16> m83A;
    /* 0x9A8 */ be<u32> m83C;
    /* 0x9AC */ be<u32> m840;
};
WWHD_OFFSET(daNpc_Bs1_c, mPhase, 0x3AC);
WWHD_OFFSET(daNpc_Bs1_c, mBtpAnm, 0x3C8);
WWHD_OFFSET(daNpc_Bs1_c, mAcch, 0x444);
WWHD_OFFSET(daNpc_Bs1_c, mAcchCir, 0x608);
WWHD_OFFSET(daNpc_Bs1_c, mStts, 0x648);
WWHD_OFFSET(daNpc_Bs1_c, mCyl, 0x684);
WWHD_OFFSET(daNpc_Bs1_c, mEventCut, 0x7B8);
WWHD_OFFSET(daNpc_Bs1_c, mJntCtrl, 0x824);
WWHD_OFFSET(daNpc_Bs1_c, mStickControl, 0x858);
WWHD_OFFSET(daNpc_Bs1_c, mCurrActionFunc, 0x8E8);
WWHD_OFFSET(daNpc_Bs1_c, mShopCamAction, 0x8F0);
WWHD_OFFSET(daNpc_Bs1_c, mShopItems, 0x940);
WWHD_OFFSET(daNpc_Bs1_c, mpShopCursor, 0x984);
WWHD_OFFSET(daNpc_Bs1_c, mType, 0x99E);
WWHD_SIZE(daNpc_Bs1_c, 0x9B0);

/* ======================= helpers shared by the d_a_npc_bs1 source files ======================= */
#define BS1_HIO_EA 0x10466878u
static inline daNpc_Bs1_HIO_c* bs1_HIO() { return gabi::at<daNpc_Bs1_HIO_c>(BS1_HIO_EA); }
static inline daNpc_Bs1_childHIO_c* bs1_child(s32 type) { return &bs1_HIO()->mChild[type]; }

/* member function addresses (pointer to member targets, nested calls) */
#define BS1_wait_action 0x02217258u
#define BS1_getdemo_action 0x02217368u
#define BS1_event_action 0x0221709Cu

/* GHS pointer to member call (this->*pmf)(arg) */
static inline void bs1_pmf_call(daNpc_Bs1_c* self, Bs1ActionFunc_l* pmf, u32 arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
/* setAction(&daNpc_Bs1_c::fn, arg) (inline): GHS compares with {0, -1, fn} */
static inline void bs1_setAction(daNpc_Bs1_c* a, u32 fn, u32 arg = 0) {
    Bs1ActionFunc_l* cur = &a->mCurrActionFunc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn) return;
    if (i != 0) {
        a->mActionStatus = -1; /* ACTION_ENDING */
        bs1_pmf_call(a, cur, 0);
    }
    a->mActionStatus = 0; /* ACTION_STARTING */
    cur->d = 0;
    cur->i = -1;
    cur->f = fn;
    gabi::call_ptr<BOOL>(cur->f, a, arg);
}
/* checkAction(&daNpc_Bs1_c::fn) */
static inline bool bs1_checkAction(daNpc_Bs1_c* a, u32 fn) {
    return a->mCurrActionFunc.i == -1 && a->mCurrActionFunc.d == 0 && a->mCurrActionFunc.f == fn;
}
#define BS1_SAFESTRING_VTBL 0x100183BCu /* this TU's sead::SafeString vtable */
#define BS1_VTBL 0x10018414u            /* daNpc_Bs1_c vtable (HD virtual destructor), written by _create */
/* HD: the matcher swapped these two names (see the definitions) */
#define BS1_shopStickMoveMsgCheck 0x02216064u
#define BS1_shopMsgCheck 0x022160ECu
/* HEADER READY */
