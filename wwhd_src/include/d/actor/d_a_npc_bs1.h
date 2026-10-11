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
#include "d/d_shop.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

/* ---- shared NPC / shop classes, WWHD layouts (SHARED-CANDIDATE; same as d_a_npc_bms1.h) ---- */

/* STControl (HD 0x28: GameCube members 0x04..0x28 at 0x00, the vtable at +0x24) */
struct Bs1StickControl {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(Bs1StickControl, 0x28);

/* ShopCam_action_c (HD 0x50, constructor 025BBDF0): the action pointer to member (8 bytes) and
 * 8 more bytes, then GameCube m18.. at -8 */
struct Bs1ShopCam : ShopCam_action_c {
    void Reset() { gabi::call(0x025BC620, this); }                             /* 025BC620 */
    void Save() { gabi::call(0x025BC5B0, this); }                              /* 025BC5B0 */
    BOOL shop_cam_action_init() { return gabi::call<BOOL>(0x025BBE98, this); } /* 025BBE98 */
    void move() { gabi::call(0x025BC714, this); }                              /* 025BC714 */
};
WWHD_SIZE(Bs1ShopCam, 0x50);

/* ShopItems_c (HD 0x44 as GameCube, constructor 025BC760) */
struct Bs1ShopItems : ShopItems_c {
    void setItemSetDataList() { gabi::call(0x025BCF84, this); }                     /* 025BCF84 */
    void createItem(s32 a, s32 room) { gabi::call(0x025BC810, this, a, room); }     /* 025BC810 */
    void Item_Move() { gabi::call(0x025BCCA4, this); }                              /* 025BCCA4 */
    BOOL Item_ZoomUp(cXyz* pos) { return gabi::call<BOOL>(0x025BCC7C, this, pos); } /* 025BCC7C (cXyz&) */
    void hideSelectItem() { gabi::call(0x025BCD90, this); }                         /* 025BCD90 */
    u8 getSelectItemNo() { return gabi::call<u8>(0x025BD22C, this); }               /* 025BD22C */
    void showItem() { gabi::call(0x025BCE04, this); }                               /* 025BCE04 */
    u32 getSelectItemBuyMsg() { return gabi::call<u32>(0x025BD278, this); }         /* 025BD278 */
    void getSelectItemPos(cXyz* out) { gabi::call(0x025BCFA8, this, out); }         /* 025BCFA8 */
    void getSelectItemBasePos(cXyz* out) { gabi::call(0x025BD0EC, this, out); }     /* 025BD0EC */
};
WWHD_SIZE(Bs1ShopItems, 0x44);

/* ShopCursor_c: only the members the NPCs touch (show/hide flag +0xB4, setScale +0x98..+0xB0) */
struct Bs1ShopCursor : ShopCursor_c {
    void setPos(cXyz* p) { gabi::call(0x025BD31C, this, p); } /* 025BD31C (cXyz&) */
    void anm_play() { gabi::call(0x025BD290, this); }         /* 025BD290 */
    void draw() { gabi::call(0x025BD338, this); }             /* 025BD338 */
};

/* GHS pointer to member function (8 bytes): {s16 this delta, s16 vtable index (0 NULL, < 0 plain),
 * u32 function} */
struct Bs1ActionFunc {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(Bs1ActionFunc, 8);

/* daNpc_Bs1_childHIO_c (HD 0x48): +0 not written, dNpc_HIO_c members at +4 (its vtable +0x28),
 * own members +0x2C.., vtable +0x44 */
struct daNpc_Bs1_childHIO_c {
    /* 0x00 */ be<u32> unk0;
    /* 0x04 */ dNpc_HIO_c mNpc;
    /* 0x2C */ be<u8> m2C;
    /* 0x2D */ u8 _2D[3];
    /* 0x30 */ be<f32> m30; /* ShopCursor scale .. */
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
    /* 0x3C8 */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74) */
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
    /* 0x858 */ Bs1StickControl mStickControl;
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
    /* 0x8E8 */ Bs1ActionFunc mCurrActionFunc;
    /* 0x8F0 */ Bs1ShopCam mShopCamAction;
    /* 0x940 */ Bs1ShopItems mShopItems;
    /* 0x984 */ gptr<Bs1ShopCursor> mpShopCursor;
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
extern daNpc_Bs1_HIO_c& l_HIO;
static inline daNpc_Bs1_HIO_c* bs1_HIO() { return &l_HIO; }
static inline daNpc_Bs1_childHIO_c* bs1_child(s32 type) { return &bs1_HIO()->mChild[type]; }

/* member function addresses (pointer to member targets, nested calls) */
#define BS1_wait_action 0x02217258u
#define BS1_getdemo_action 0x02217368u
#define BS1_event_action 0x0221709Cu

/* GHS pointer to member call (this->*pmf)(arg) */
static inline void bs1_pmf_call(daNpc_Bs1_c* self, Bs1ActionFunc* pmf, u32 arg) {
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
    Bs1ActionFunc* cur = &a->mCurrActionFunc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn)
        return;
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

// HD: typed views for direct inline memory operations.
struct Bs1VectorWords {
    be<u32> x, y, z;
};
struct Bs1MessageView {
    u8 mPadding[0x922];
    be<u8> mCancel;
    u8 mPadding923[0x15];
    be<u32> mMsgNo;
    u8 mPadding93C[0xC];
    be<u32> mSelectNum;
};
struct Bs1ModelMaterialAnimation {
    u8 mPadding[0x38];
    be<u32> mpTexturePattern;
};
WWHD_SIZE(Bs1VectorWords, 12);
WWHD_OFFSET(Bs1MessageView, mMsgNo, 0x938);
WWHD_OFFSET(Bs1MessageView, mSelectNum, 0x948);
WWHD_OFFSET(Bs1ModelMaterialAnimation, mpTexturePattern, 0x38);
inline Bs1VectorWords* bs1_words(cXyz* p) { return reinterpret_cast<Bs1VectorWords*>(p); }
inline Bs1MessageView* bs1_message(u32 a) { return gabi::at<Bs1MessageView>(a); }
inline dComIfG_play_c* bs1_play() { return gabi::at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0); }
inline dSv_player_c* bs1_savePlayer() { return gabi::at<dSv_player_c>(dComIfGs_ea() + 0x20); }

struct Bs1Int10 {
    be<s32> mEntries[10];
};

struct Bs1Float10 {
    be<f32> mEntries[10];
};

struct Bs1Int4 {
    be<s32> mEntries[4];
};

struct Bs1Byte6 {
    be<u8> mEntries[6];
};

struct Bs1Half7 {
    be<u16> mEntries[7];
};

struct Bs1Names8 {
    be<u32> mEntries[8];
};

struct Bs1Names2 {
    be<u32> mEntries[2];
};

struct Bs1Items6 {
    be<u32> mEntries[6];
};

struct Bs1Items3 {
    be<u32> mEntries[3];
};

extern daNpc_Bs1_HIO_c& l_HIO;

extern be<u32>& l_msgId;

extern Bs1Int10& l_bck_ix_tbl;

extern Bs1Int4& l_btp_ix_tbl;

extern Bs1Int10& play_mode_tbl;

extern Bs1Float10& morf_frame_tbl;

extern Bs1Float10& play_speed_tbl;

extern Bs1Byte6& l_beastIndices;

extern Bs1Half7& l_beastMessages;

extern Bs1Names8& cut_name_tbl;

extern Bs1Names2& a_name;

extern Bs1Names2& a_cut_name;

extern Bs1Items6& Item_set_data3;

extern Bs1Items3& Item_set_data4;

extern Bs1Items3& Item_set_data5;

extern Bs1Items3& Item_set_dataBs2;

extern dCcD_SrcCyl& l_cyl_src;

extern csXyz& l_angleZero;

extern be<s16>& l_tag_pay_rupee;

extern be<u8>& l_tag_buy_item_max;

extern be<u8>& l_tag_buy_item;

struct Bs1MatrixValues {
    be<f32> mValues[12];
};
struct Bs1AcchVtables {
    u8 mPadding[0x14];
    be<u32> mCheck;
    u8 mPadding18[8];
    be<u32> mActor;
};
WWHD_SIZE(Bs1MatrixValues, 0x30);
WWHD_SIZE(Bs1AcchVtables, 0x24);
inline s16 bs1_TimerSubstance(void* p) { return (s16) reinterpret_cast<be<u16>*>(p)[1]; }

// HD: private ABI bindings preserve the original register widths.
inline s16 bs1_calcTimer(be<s16>* timer) { return gabi::call<s16>(0x02055B64, timer); }
inline void bs1_playerEyePos(cXyz* eye, f32 height) { gabi::call(0x0259D54C, eye, height); }
inline BOOL bs1_normalizeRS(cXyz* direction) { return gabi::call<BOOL>(0x0201B47C, direction); }
inline void bs1_lookAtTarget(dNpc_JntCtrl_c* control, be<s16>* angle, cXyz* target, cXyz* velocity,
                             s32 rotation, s32 speed, s32 mode) {
    gabi::call(0x0259DED0, control, angle, target, velocity, rotation, speed, mode);
}
inline u32 bs1_nextMessageStatus(daNpc_Bs1_c* actor, be<u32>* message, u32 selection) {
    return gabi::call<u32>(0x02214858, actor, message, selection);
}
inline s32 bs1_getMyActIdx(dEvent_manager_c* manager, s32 staff, u32 names, s32 count, s32 mode, s32 flags) {
    return gabi::call<s32>(0x02542EDC, manager, staff, names, count, mode, flags);
}
inline u32 bs1_createPresent(cXyz* position, u32 item, s32 flags, s32 angle, s32 room, u32 callback,
                             u32 data) {
    return gabi::call<u32>(0x025D7DEC, position, item, flags, angle, room, callback, data);
}
inline Bs1ShopCursor* bs1_createCursor(J3DModelData* data, void* animation, f32 scale) {
    return gabi::call<Bs1ShopCursor*>(0x025BBD7C, data, animation, scale);
}
inline void bs1_eventCutConstruct(dNpc_EventCut_c* cut) { gabi::call(0x0259F740, cut); }
inline void bs1_jointControlConstruct(dNpc_JntCtrl_c* control) { gabi::call(0x0259DAA0, control); }
inline void bs1_stickSetWaitParm(Bs1StickControl* control, s32 a, s32 b, s32 c, s32 d, f32 rate,
                                 f32 threshold, s32 e, s32 f) {
    gabi::call(0x025885C4, control, a, b, c, d, rate, threshold, e, f);
}
inline void bs1_stickInit(Bs1StickControl* control) { gabi::call(0x025885E8, control); }
inline void bs1_shopCamConstruct(Bs1ShopCam* camera) { gabi::call(0x025BBDF0, camera); }
inline void bs1_shopItemsConstruct(Bs1ShopItems* items) { gabi::call(0x025BC760, items); }
inline s32 bs1_getRoomId(dBgS* background, u32 polygon) {
    return gabi::call<s32>(0x024EF130, background, polygon);
}
inline s32 bs1_getPolyColor(dBgS* background, u32 polygon) {
    return gabi::call<s32>(0x024EEEB8, background, polygon);
}
inline void bs1_registerFigure(s32 type, fopAc_ac_c* actor, cXyz* position, s32 angle, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, actor, position, angle, x, y, z);
}
inline void bs1_setActorInfo(dNpc_EventCut_c* cut, const char* name, fopAc_ac_c* actor) {
    gabi::call(0x0259F7D4, cut, name, actor);
}
inline void bs1_constructHIO(u32 hio) { gabi::call(0x0259DA18, hio); }
inline void bs1_constructChildren(void* children, s32 count, s32 size, u32 constructor) {
    gabi::call(0x028EFFD0, children, count, size, constructor);
}
inline void bs1_destroyStick(Bs1StickControl* control, s32 flags) { gabi::call(0x025886D8, control, flags); }
inline void bs1_destroyCircle(u32 circle, s32 flags) { gabi::call(0x02018034, circle, flags); }
inline void bs1_destroyAcch(dBgS_ObjAcch* acch, s32 flags) { gabi::call(0x024EFD9C, acch, flags); }
inline void bs1_destroyActor(fopAc_ac_c* actor, s32 flags) { gabi::call(0x025D50BC, actor, flags); }

// HD: unit-local adapters for the original guest ABI.
inline u8 bs1_ShopItems_getItemNo(Bs1ShopItems* items, s32 idx) {
    return gabi::call<u8>(0x025BD248, items, idx);
}
inline BOOL bs1_isBomb(u8 item) { return gabi::call<BOOL>(0x02550FC4, item); }
inline BOOL bs1_ShopItems_isSoldOutItemAll(Bs1ShopItems* items) {
    return gabi::call<BOOL>(0x025BCF44, items);
}
inline void* bs1_getRes(s32 idx) { return dComIfG_getObjectRes("Bs" /* "Bs" */, idx, BS1_SAFESTRING_VTBL); }
inline s32 bs1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                       s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
inline s32 bs1_getFrameMax(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 4) + 0x14), p);
}
inline void bs1_dNpc_setAnm_2(mDoExt_McaMorf* m, u32 loop, f32 morf, f32 speed, s32 anm, s32 snd,
                              const char* arc) {
    gabi::call(0x0259D79C, m, loop, morf, speed, anm, snd, arc);
}
inline u32 bs1_getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBuffer_hd* blk = model->mpMtxBuffer;
    blk->mFlags = (u16)blk->mFlags | 0x10;
    return (u32)blk->mpAnmMtx + jntNo * 0x30;
}
inline void bs1_mtx_copy(u32 dst, u32 src) {
    f32 t[12];
    for (int i = 0; i < 12; i++)
        t[i] = (f32)gabi::at<Bs1MatrixValues>(src)->mValues[i];
    for (int i = 0; i < 12; i++)
        gabi::at<Bs1MatrixValues>(dst)->mValues[i] = t[i];
}
inline u32 bs1_msgMgr() { return gabi::load<u32>(0x101F4B5C); }
inline u32 bs1_msgGetStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
inline void bs1_msgSetStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
inline u32 bs1_msgSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
inline void bs1_demoMsgFlagOn() { gabi::call(0x025DB58C); }
inline void bs1_offPlayerNoDraw(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
}
inline bool bs1_isMorf(mDoExt_McaMorf* m) { return gabi::load<f32>(gabi::ea(m) + 0xB0) < 1.0f; }
inline s8 bs1_getJointIndex(J3DModelData* d, const char* name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return gabi::call<s8>(0x027DF9B0, tab, name); /* JUTNameTab::getIndex */
}
inline u16 bs1_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
inline void bs1_setJointCallBack(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n)
        p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
inline void* bs1_getRes2(s32 idx) { return dComIfG_getObjectRes("Bs" /* "Bs" */, idx, BS1_SAFESTRING_VTBL); }
inline BOOL bs1_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, (u32)item); }
inline BOOL bs1_isArrow(u8 item) { return gabi::call<BOOL>(0x02550FDC, (u32)item); }
inline u32 bs1_createShopItem(u32 pos, u8 itemNo, void* angle, s8 roomNo) {
    return gabi::call<u32>(0x025D87E4, pos, (u32)itemNo, angle, (s32)roomNo, 0,
                           0); /* fopAcM_createShopItem */
}
inline void bs1_ShopItems_setItemSetDataList(Bs1ShopItems* items, be<u32>* list) {
    gabi::call(0x025BCFA0, items, list);
}
inline void bs1_ShopItems_SoldOutItem(Bs1ShopItems* items, s32 i) { gabi::call(0x025BCECC, items, i); }
inline u8 bs1_setDataItemNo(u32 setData) {
    return gabi::at<ShopItems_c__ItemData>(gabi::at<__shop_items_set_data>(setData)->mpItemData)
        ->itemNoByte();
}
inline u32 bs1_posTbl(daNpc_Bs1_c* a) { return gabi::load<u32>(0x101EB974 + 4 * (s32)a->mShopIndex); }
inline void bs1_CreateInit_common(daNpc_Bs1_c* a) {
    u32 py = (u32)bs1_words(&a->current.pos)->y;
    u32 px = (u32)bs1_words(&a->current.pos)->x;
    u32 pz = (u32)bs1_words(&a->current.pos)->z;
    bs1_words(&a->m718)->x = px; /* m718 = current.pos */
    bs1_words(&a->m718)->y = py;
    bs1_words(&a->m718)->z = pz;
    a->eventInfo.mpEventCB = 0x02212258; /* eventInfo.setXyEventCB(daNpc_Bs1_XyEventCB) */
    a->mStts.Init(0xFF, 0xFF, a);
    a->mCyl.Set(&l_cyl_src /* l_cyl_src */);
    a->m708 = 0;
    a->m740 = 0;
    a->mShopIndex = (s8)(u8) reinterpret_cast<be<u8>*>(&a->mParameters)[3]; /* fopAcM_GetParam(this) & 0xFF */
    a->m837 = 0;
    a->mCyl.SetStts(&a->mStts); /* mCyl.SetStts(&mStts) */
    a->m836 = 0;
}

// HD: unit-local adapters for the original guest ABI.
inline u32 bs1m_mgr() { return gabi::load<u32>(0x101F4B5C); }
inline u32 bs1m_getStatus(u32 m) { return gabi::call<u32>(0x025F795C, m); }
inline void bs1m_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
inline u32 bs1m_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
inline void bs1m_demoMsgFlagOn() { gabi::call(0x025DB58C); }
inline BOOL bs1m_trigB() { return gabi::call<BOOL>(0x020078BC, 0); }
inline s32 bs1m_boughtErrorStatus(Bs1ShopItems* items, s32 a, s32 price) {
    return gabi::call<s32>(0x025BBA1C, items, a, price);
}
inline BOOL bs1m_nowTriggercheck(Bs1StickControl* stick, Bs1ShopItems* items, be<u32>* msg, u32 cb,
                                 void* arg) {
    return gabi::call<BOOL>(0x025BB698, stick, items, msg, cb, arg);
}
inline BOOL bs1m_isSoldOutItemAll(Bs1ShopItems* items) { return gabi::call<BOOL>(0x025BCF44, items); }
inline void bs1m_SoldOutItem(Bs1ShopItems* items, s16 idx) { gabi::call(0x025BCECC, items, idx); }
inline u32 bs1m_getSelectItemShowMsg(Bs1ShopItems* items) { return gabi::call<u32>(0x025BD260, items); }
inline BOOL bs1m_checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
inline void bs1m_execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
inline void bs1m_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) {
    gabi::call(0x025E1A40, id, pos, param, reverb);
}
inline void bs1m_letterSend(u16 id) { gabi::call(0x02586D5C, id); }
inline BOOL bs1m_isEmono(u8 item) { return gabi::call<BOOL>(0x02550FF4, item); }
inline BOOL bs1m_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
inline s32 bs1m_getBuyItemMax(s32 rupee, s32 item) { return gabi::call<s32>(0x022124F0, rupee, item); }
inline BOOL bs1m_chkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
inline BOOL bs1m_normalizeRS(cXyz* v) { return gabi::call<BOOL>(0x0201B47C, v); }
inline dSv_event_c* bs1m_ev() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
inline u8 bs1m_play_u8(u32 off) { return gabi::load<u8>(dComIfGp_ea() + off); }
inline void bs1m_play_set_u8(u32 off, u8 v) { gabi::store<u8>(dComIfGp_ea() + off, v); }
inline bool bs1m_chkTalkXY(u8 mode) { return (u32)(mode - 1) <= 3; }
inline be<u32>& bs1m_msgId() { return l_msgId; }
inline BOOL bs1m_checkEnd(daNpc_Bs1_c* a) {
    mDoExt_McaMorf* m = a->mpMorf;
    f32 e = (f32)(s32)m->mFrameCtrl.mEnd;
    return m->mFrameCtrl.checkPass(e - 1.0f);
}
inline void bs1m_cancel(u32 mgr, be<u32>* pMsgNo, be<u32>* pMsgNo2, u32 msgNo) {
    *pMsgNo = msgNo;
    if (pMsgNo2 != nullptr) {
        *pMsgNo2 = msgNo;
    }
    bs1_message(mgr)->mCancel = 1;
    bs1_play()->mMesgSendButton = 1;
    bs1_play()->mMesgCancelButton = 0;
}
inline void bs1m_cancelLeave(daNpc_Bs1_c* a, u32 mgr, be<u32>* pMsgNo, be<u32>* pMsgNo2) {
    u8 points = dComIfGs_getEventReg(0x86FF);
    u32 msgNo;
    if (bs1m_isSoldOutItemAll(&a->mShopItems)) {
        msgNo = 0xF62;
    } else if (points >= 60) {
        msgNo = 0xF61;
    } else {
        msgNo = 0xF40;
    }
    bs1m_cancel(mgr, pMsgNo, pMsgNo2, msgNo);
}
inline void bs1m_payBeast(u32 beastIdx) {
    s16 pay = (s16)l_tag_pay_rupee;
    dComIfG_play_c* play = bs1_play();
    play->mItemRupeeCount = (s32)play->mItemRupeeCount + pay;
    u8 n = (u8)l_tag_buy_item;
    play = bs1_play();
    be<s16>& count = play->mItemBeastNumCounts[beastIdx];
    count = (s16)(count - n);
}
inline void bs1m_bought(daNpc_Bs1_c* a, s16 rupee) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    bs1m_seStart(0x87F /* JA_SE_SHOP_BOUGHT */, &a->eyePos, 0, reverb);
    a->mShopItems.hideSelectItem();
    dComIfG_play_c* play = bs1_play();
    play->mItemRupeeCount = (s32)play->mItemRupeeCount - rupee;
}
inline u32 bs1m_nextMsgStatus(daNpc_Bs1_c* a, be<u32>* p, be<u32>* p2) {
    return gabi::call<u32>(0x02214858, a, p, p2);
}
inline void bs1m_getItemZoomPos(Bs1ShopCam* cam, cXyz* out) {
    gabi::Local<cXyz> dir;
    cXyz_mi(&cam->m18, dir.get(), &cam->m24);
    if (!bs1m_normalizeRS(dir.get())) {
        dir->x = 0.0f;
        dir->z = 1.0f;
        dir->y = 0.0f;
    }
    gabi::Local<cXyz> scaled;
    cXyz_ml(dir.get(), scaled.get(), 100.0f);
    gabi::Local<cXyz> zoom;
    cXyz_pl(&cam->m24, zoom.get(), scaled.get());
    f32 x = zoom->x, y = zoom->y, z = zoom->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

// HD: render-system model slot and the player virtual ABI.
inline J3DModel* bs1_currentModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
inline void bs1_setPlayerPosAndAngle(fopAc_ac_c* player, cXyz* position, s32 angle) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(player) + 0xB4) + 0x114), player, position,
                   angle);
}
