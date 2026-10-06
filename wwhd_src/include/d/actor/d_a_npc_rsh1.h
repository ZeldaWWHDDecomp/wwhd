/* daNpc_Rsh1_c (Zunari, the Windfall travelling merchant), WWHD layout.
 *
 * Measured from the WWHD code (_create 022D7998 inlines the constructor, initTexPatternAnm,
 * playTexPatternAnm, the HIO constructor). Base fopAc_ac_c (0x3AC). GameCube -> WWHD:
 * mPhs/mpMorf +0x11C; HD has no shadow id: m_head_tex_pattern sits at 0x3B8 and mDoExt_btpAnm (HD 0x74)
 * at 0x3BC; from mBtpFrame (GameCube 0x2BC) to the end of mCyl +0x174; the joint numbers,
 * dNpc_JntCtrl_c, dNpc_EventCut_c, STControl (HD 0x28) and the members up to mCurrProc +0x174;
 * mCurrProc is an 8-byte GHS pointer to member, ShopCam_action_c HD 0x50 (GameCube 0x58), so the
 * members from mpShopItems on are +0x168. Size 0xAD0 (GameCube 0x968). */
#pragma once
#include "bindings.h"

/* ---- shared NPC / shop classes, WWHD layouts (SHARED-CANDIDATE; as d_a_npc_bms1.h) ---- */

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
    /* 0x24 */ be<u32> m24;
    /* 0x28 */ be<u8> mItemIsSoldOut[8];
    /* 0x30 */ u8 _30[0x3C - 0x30];
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
    void show() { mbShow = 1; }
    void hide() { mbShow = 0; }
    void setPos(cXyz* p) { gabi::call(0x025BD31C, this, p); }   /* 025BD31C (cXyz&) */
    void anm_play() { gabi::call(0x025BD290, this); }           /* 025BD290 */
    void draw() { gabi::call(0x025BD338, this); }               /* 025BD338 */
};

/* dNpc_HIO_c, HD 0x28 (GameCube members 0x04.. at 0x00, the vtable at +0x24; constructor 0259DA18) */
struct dNpc_HIO_c_l {
    /* 0x00 */ be<f32> m04;
    /* 0x04 */ be<s16> mMaxHeadX;
    /* 0x06 */ be<s16> mMaxBackboneX;
    /* 0x08 */ be<s16> mMaxHeadY;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinHeadX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMinHeadY;
    /* 0x12 */ be<s16> mMinBackboneY;
    /* 0x14 */ be<s16> mMaxTurnStep;
    /* 0x16 */ be<s16> mMaxHeadTurnVel;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<s16> mMaxAttnAngleY;
    /* 0x1E */ be<u8> m22;
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> mMaxAttnDistXZ;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_c_l, 0x28);

/* daNpc_Rsh1_HIO_c (HD 0x6C, l_HIO at 0x104685A0; vtable at the end) */
struct daNpc_Rsh1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m08;
    /* 0x08 */ dNpc_HIO_c_l mNpcHIO;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<f32> m3C;
    /* 0x3C */ be<f32> m40;
    /* 0x40 */ be<f32> m44;
    /* 0x44 */ be<f32> m48;
    /* 0x48 */ be<f32> m4C;
    /* 0x4C */ be<f32> mCylR1;
    /* 0x50 */ be<f32> mCylR2;
    /* 0x54 */ be<f32> mCylH;
    /* 0x58 */ be<u8> m5C[12];
    /* 0x64 */ be<u8> m68;
    /* 0x65 */ be<u8> m69;
    /* 0x66 */ be<u8> m6A;
    /* 0x67 */ be<u8> m6B;
    /* 0x68 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Rsh1_HIO_c, 0x6C);
#define RSH1_HIO_ADDR 0x104685A0u
static inline daNpc_Rsh1_HIO_c& rsh1_HIO() { return *gabi::at<daNpc_Rsh1_HIO_c>(RSH1_HIO_ADDR); }

struct daNpc_Rsh1_c : fopAc_ac_c {
    /* GHS pointer to member (8 bytes) */
    struct ProcFunc_l {
        /* 0x0 */ be<s16> d;
        /* 0x2 */ be<s16> i;
        /* 0x4 */ be<u32> f;
    };

    BOOL checkCreateInShopPlayer();
    BOOL initTexPatternAnm(u32 i_modify); /* bool, passed on as the full register */
    void playTexPatternAnm();
    void setAnm(s8);
    u32 setTexAnm(s8);
    void setAnmFromMsgTag();
    bool chkAttention(cXyz*, s16); /* cXyz by value: passed by address */
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*, be<u32>*); /* HD: a second message number pointer (r5); full 32-bit status */
    u32 getMsg();
    void setCollision();
    void talkInit();
    u16 normal_talk();
    u16 shop_talk();
    u16 talk();
    BOOL CreateInit();
    void createShopList();
    void setAttention();
    void lookBack();
    bool pathGet();
    int getAimShopPosIdx();
    BOOL shopPosMove();
    BOOL pathMove(be<s32>*);
    bool wait01();
    bool talk01();
    BOOL getdemo_action(void*);
    BOOL wait_action(void*);
    BOOL pl_shop_out_action(void*);
    bool evn_setAnm_init(int);
    bool evn_talk_init(int);
    bool evn_continue_talk_init(int);
    BOOL evn_talk();
    bool evn_turn_init(int);
    BOOL evn_turn();
    bool privateCut();
    BOOL event_action(void*);
    BOOL dummy_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no shadow id */
    /* 0x3BC */ u8 mBtpAnm[0x74];                          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x430 */ be<u8> mBtpFrame;
    /* 0x431 */ u8 _431;
    /* 0x432 */ be<s16> mTimer;
    /* 0x434 */ u8 _434[4];
    /* 0x438 */ dBgS_ObjAcch mAcch;
    /* 0x5FC */ dBgS_AcchCir mAcchCir;
    /* 0x63C */ dCcD_Stts mStts;
    /* 0x678 */ dCcD_Cyl mCyl;
    /* 0x7A8 */ be<s8> m_head_jnt_num;
    /* 0x7A9 */ be<s8> m_backbone_jnt_num;
    /* 0x7AA */ u8 _7AA[2];
    /* 0x7AC */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x7E0 */ dNpc_EventCut_c mEventCut;
    /* 0x84C */ STControl_l mSTControl;
    /* 0x874 */ gptr<dPath> mpPath;
    /* 0x878 */ be<s8> m704;
    /* 0x879 */ u8 _879[3];
    /* 0x87C */ cXyz mPathPointPos[5];
    /* 0x8B8 */ be<s8> m744[5];
    /* 0x8BD */ be<s8> m749;
    /* 0x8BE */ u8 _8BE[0x8CC - 0x8BE];
    /* 0x8CC */ cXyz mAttnBasePos;
    /* 0x8D8 */ be<s16> mLookAtMaxVel;
    /* 0x8DA */ csXyz mActorAngle;
    /* 0x8E0 */ u8 _8E0[2];
    /* 0x8E2 */ be<s8> mMorfIsStop;
    /* 0x8E3 */ be<u8> m76F;
    /* 0x8E4 */ be<u8> mbAttention;
    /* 0x8E5 */ be<u8> m771;
    /* 0x8E6 */ u8 _8E6[2];
    /* 0x8E8 */ be<f32> mMorfPrevFrame;
    /* 0x8EC */ be<u32> m778;
    /* 0x8F0 */ be<u32> m77C;
    /* 0x8F4 */ be<u32> m780;
    /* 0x8F8 */ be<u32> m784;
    /* 0x8FC */ be<s32> mShopIdx;
    /* 0x900 */ be<s32> m78C;
    /* 0x904 */ be<s16> mShopOutEventIdx;
    /* 0x906 */ be<u8> mShopSelectItemNo;
    /* 0x907 */ be<u8> m793;
    /* 0x908 */ cXyz m794;
    /* 0x914 */ cXyz m7A0;
    /* 0x920 */ ProcFunc_l mCurrProc;
    /* 0x928 */ ShopCam_action_c_l mShopCamAct;
    /* 0x978 */ gptr<ShopItems_c_l> mpShopItems;
    /* 0x97C */ ShopItems_c_l mShopItemsArr[4];
    /* 0xA8C */ be<u32> mShopItemDataPtrs[12];
    /* 0xABC */ gptr<ShopCursor_c_l> mpShopCursor;
    /* 0xAC0 */ be<s8> m958;
    /* 0xAC1 */ be<s8> m959;
    /* 0xAC2 */ be<s8> m95A;
    /* 0xAC3 */ be<s8> m95B;
    /* 0xAC4 */ be<s8> m95C;
    /* 0xAC5 */ be<u8> m95D;
    /* 0xAC6 */ be<s8> m95E;
    /* 0xAC7 */ u8 _AC7;
    /* 0xAC8 */ be<s8> mActionStatus;
    /* 0xAC9 */ be<s8> m961;
    /* 0xACA */ be<s8> m962;
    /* 0xACB */ be<u8> mItemNo;
    /* 0xACC */ u8 _ACC[4];
};
WWHD_OFFSET(daNpc_Rsh1_c, mpMorf, 0x3B4);
WWHD_OFFSET(daNpc_Rsh1_c, mBtpFrame, 0x430);
WWHD_OFFSET(daNpc_Rsh1_c, mAcch, 0x438);
WWHD_OFFSET(daNpc_Rsh1_c, mAcchCir, 0x5FC);
WWHD_OFFSET(daNpc_Rsh1_c, mStts, 0x63C);
WWHD_OFFSET(daNpc_Rsh1_c, mCyl, 0x678);
WWHD_OFFSET(daNpc_Rsh1_c, mJntCtrl, 0x7AC);
WWHD_OFFSET(daNpc_Rsh1_c, mEventCut, 0x7E0);
WWHD_OFFSET(daNpc_Rsh1_c, mSTControl, 0x84C);
WWHD_OFFSET(daNpc_Rsh1_c, mpPath, 0x874);
WWHD_OFFSET(daNpc_Rsh1_c, mCurrProc, 0x920);
WWHD_OFFSET(daNpc_Rsh1_c, mShopCamAct, 0x928);
WWHD_OFFSET(daNpc_Rsh1_c, mpShopItems, 0x978);
WWHD_OFFSET(daNpc_Rsh1_c, mShopItemsArr, 0x97C);
WWHD_OFFSET(daNpc_Rsh1_c, mpShopCursor, 0xABC);
WWHD_OFFSET(daNpc_Rsh1_c, mActionStatus, 0xAC8);
WWHD_SIZE(daNpc_Rsh1_c, 0xAD0);

/* free functions of the TU (static in the GameCube source; declared here so all parts can call them) */
int daNpc_Rsh1_countShop();
BOOL daNpc_Rsh1_shopMsgCheck(u32);
BOOL daNpc_Rsh1_shopStickMoveMsgCheck(u32);
BOOL daNpc_Rsh1_checkRotenBaseTalkArea();

/* action addresses (pointer-to-member targets) */
enum : u32 {
    RSH1_getdemo_action = 0x022DA818,
    RSH1_wait_action = 0x022DA748,
    RSH1_pl_shop_out_action = 0x022DAA78,
    RSH1_event_action = 0x022DB4A4,
    RSH1_dummy_action = 0x022DB674,
};
#define RSH1_VTBL 0x100216A4u /* daNpc_Rsh1_c vtable (HD virtual destructor) */
#define RSH1_ARC 0x101C619Cu  /* "Rsh" (daNpc_Rsh1_c::m_arcname) */
/* HEADER READY */

/* ---- local bindings and helpers of the TU (SHARED-CANDIDATE where noted) ---- */
#define RSH1_SAFESTRING_VTBL 0x1002164Cu /* this TU's sead::SafeString vtable */
static inline void* rsh1_getRes(s32 idx) { return dComIfG_getObjectRes(STR(RSH1_ARC), idx, RSH1_SAFESTRING_VTBL); }
/* dComIfGs_isGetItemReserve(i): dSv_player_get_bag_item_c (save + 0xB0)::isReserve (025B7840) */
static inline BOOL rsh1_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, gabi::load<u32>(0x101F84DC) + 0xB0, (u32)i); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem_l(u8 item) { return gabi::call<BOOL>(0x02520C0C, (u32)item); }
/* J3DModel::getAnmMtx (HD: joint matrix block at +0x2C, dirty flag 0x10 at +4, matrices at +0x10) */
static inline u32 rsh1_getAnmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* matrix assignment through FPRs (lfs/stfs: the recompiled lfs quiets a signalling NaN) */
static inline void rsh1_mtx_copy(u32 dst, u32 src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(dst + 4 * i, t[i]);
}
static inline s32 rsh1_btpInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end,
                               u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 rsh1_getFrameMax(J3DAnmTexPattern* p) {
    u32 e = gabi::ea(p);
    return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(e + 4) + 0x14), e);
}
static inline s8 rsh1_getJointIndex(J3DModelData* d, const char* name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return gabi::call<s8>(0x027DF9B0, tab, name); /* JUTNameTab::getIndex */
}
static inline u16 rsh1_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void rsh1_setJointCallBack(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline ShopCursor_c_l* rsh1_ShopCursor_create(void* mdl, void* brk, f32 scale) {
    return gabi::call<ShopCursor_c_l*>(0x025BBD7C, mdl, brk, scale);
}
static inline void rsh1_dNpc_setAnm_2(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 anm, s32 bas, const char* arc) {
    gabi::call(0x0259D79C, m, loop, morf, speed, anm, bas, arc);
}
/* (this->*mCurrProc)(NULL): GHS pointer to member call (i < 0: plain; i >= 0: virtual) */
static inline void rsh1_callProc(daNpc_Rsh1_c* a) {
    daNpc_Rsh1_c::ProcFunc_l* pmf = &a->mCurrProc;
    u32 thisp = gabi::ea(a) + (s32)(s16)pmf->d;
    s16 i = pmf->i;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, (u32)0);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, (u32)0);
    }
}
/* setAction(&daNpc_Rsh1_c::fn, NULL) (inline): GHS compares the pointer to member with {0, -1, fn} */
static inline void rsh1_setAction(daNpc_Rsh1_c* a, u32 fn) {
    daNpc_Rsh1_c::ProcFunc_l* cur = &a->mCurrProc;
    if (cur->i == -1 && cur->d == 0 && cur->f == fn) return;
    if (cur->i != 0) {
        a->mActionStatus = -1;
        rsh1_callProc(a);
    }
    a->mActionStatus = 0;
    cur->f = fn;
    cur->d = 0;
    cur->i = -1;
    gabi::call_ptr<BOOL>(cur->f, a, (u32)0);
}
/* dComIfGp_evmng_getEventIdx(name, 0xFF) */
static inline s16 rsh1_getEventIdx(u32 name) { return gabi::call<s16>(0x02543F10, dComIfGp_getPEvtManager(), name, (u32)0xFF); }
/* cXyz assignment as GHS does it for a plain copy (three word loads, three word stores) */
static inline void rsh1_copy12(u32 dst, u32 src) {
    u32 a = gabi::load<u32>(src), b = gabi::load<u32>(src + 4), c = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, a);
    gabi::store<u32>(dst + 4, b);
    gabi::store<u32>(dst + 8, c);
}
