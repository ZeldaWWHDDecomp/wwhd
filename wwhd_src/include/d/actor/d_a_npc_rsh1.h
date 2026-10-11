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
#include "gc_api/gc_api.h"
#include "d/d_shop.h"

/* Zunari ABI adapters over the shared NPC and shop layouts. */

// HD: STControl storage prefix. The shared d_lib layout conflicts with gc_api/d_lib.
struct RshStickControl { u8 _padding[0x24]; be<u32> mVtable; };
WWHD_OFFSET(RshStickControl, mVtable, 0x24);
WWHD_SIZE(RshStickControl, 0x28);

/* ShopCam_action_c (HD 0x50, constructor 025BBDF0): the action pointer to member (8 bytes) and
 * 8 more bytes, then GameCube m18.. at -8 */
struct RshShopCamera : ShopCam_action_c {
    void Reset() { gabi::call(0x025BC620, this); }                       /* 025BC620 */
    void Save() { gabi::call(0x025BC5B0, this); }                        /* 025BC5B0 */
    BOOL shop_cam_action_init() { return gabi::call<BOOL>(0x025BBE98, this); } /* 025BBE98 */
    void move() { gabi::call(0x025BC714, this); }                        /* 025BC714 */
};
WWHD_SIZE(RshShopCamera, 0x50);

/* ShopItems_c (HD 0x44 as GameCube, constructor 025BC760) */
struct RshShopItems : ShopItems_c {
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
WWHD_SIZE(RshShopItems, 0x44);
WWHD_OFFSET(RshShopItems, mNumItems, 0x40);

/* ShopCursor_c: only the members the NPCs touch (show/hide flag +0xB4, setScale +0x98..+0xB0) */
struct RshShopCursor : ShopCursor_c {
    void show() { m54 = 1; }
    void hide() { m54 = 0; }
    void setPos(cXyz* p) { gabi::call(0x025BD31C, this, p); }   /* 025BD31C (cXyz&) */
    void anm_play() { gabi::call(0x025BD290, this); }           /* 025BD290 */
    void draw() { gabi::call(0x025BD338, this); }               /* 025BD338 */
};

WWHD_SIZE(RshShopCursor, 0xB8);
WWHD_OFFSET(RshShopCursor, m54, 0xB4);

/* daNpc_Rsh1_HIO_c (HD 0x6C, l_HIO at 0x104685A0; vtable at the end) */
struct daNpc_Rsh1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m08;
    /* 0x08 */ dNpc_HIO_c mNpcHIO;
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
daNpc_Rsh1_HIO_c& rsh1_HIO();

struct daNpc_Rsh1_c : fopAc_ac_c {
    /* GHS pointer to member (8 bytes) */
    typedef BOOL (daNpc_Rsh1_c::*ProcFunc)(void*);
    using ProcStorage = gabi::ptmf<ProcFunc>;

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
    /* 0x84C */ RshStickControl mSTControl;
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
    /* 0x920 */ ProcStorage mCurrProc;
    /* 0x928 */ RshShopCamera mShopCamAct;
    /* 0x978 */ gptr<RshShopItems> mpShopItems;
    /* 0x97C */ RshShopItems mShopItemsArr[4];
    /* 0xA8C */ be<u32> mShopItemDataPtrs[12];
    /* 0xABC */ gptr<RshShopCursor> mpShopCursor;
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


/* Unit-local ABI bindings and layout adapters. */
#define RSH1_SAFESTRING_VTBL 0x1002164Cu /* this TU's sead::SafeString vtable */
static inline void* rsh1_getRes(s32 idx) { return dComIfG_getObjectRes(STR(RSH1_ARC), idx, RSH1_SAFESTRING_VTBL); }
/* dComIfGs_isGetItemReserve(i): dSv_player_get_bag_item_c (save + 0xB0)::isReserve (025B7840) */
static inline BOOL rsh1_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, gabi::load<u32>(0x101F84DC) + 0xB0, (u32)i); }
/* 02520C0C dComIfGs_checkGetItem(u8) */

/* J3DModel::getAnmMtx (HD: joint matrix block at +0x2C, dirty flag 0x10 at +4, matrices at +0x10) */

/* matrix assignment through FPRs (lfs/stfs: the recompiled lfs quiets a signalling NaN) */

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
static inline RshShopCursor* rsh1_ShopCursor_create(void* mdl, void* brk, f32 scale) {
    return gabi::call<RshShopCursor*>(0x025BBD7C, mdl, brk, scale);
}
static inline void rsh1_dNpc_setAnm_2(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 anm, s32 bas, const char* arc) {
    gabi::call(0x0259D79C, m, loop, morf, speed, anm, bas, arc);
}
/* (this->*mCurrProc)(NULL): GHS pointer to member call (i < 0: plain; i >= 0: virtual) */
static inline void rsh1_callProc(daNpc_Rsh1_c* a) {
    (a->*a->mCurrProc)(nullptr);
}
/* setAction(&daNpc_Rsh1_c::fn, NULL) (inline): GHS compares the pointer to member with {0, -1, fn} */
static inline void rsh1_setAction(daNpc_Rsh1_c* a, u32 fn) {
    daNpc_Rsh1_c::ProcStorage* cur = &a->mCurrProc;
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
static inline s16 rsh1_getEventIdx(const char* name) { return gabi::call<s16>(0x02543F10, dComIfGp_getPEvtManager(), name, (u32)0xFF); }
/* cXyz assignment as GHS does it for a plain copy (three word loads, three word stores) */
static inline void rsh1_copy12(u32 dst, u32 src) {
    u32 a = gabi::load<u32>(src), b = gabi::load<u32>(src + 4), c = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, a);
    gabi::store<u32>(dst + 4, b);
    gabi::store<u32>(dst + 8, c);
}

// HD: one TU-wide message ID, shared by the split sources.
be<u32>& rsh1_messageId();

/* HD message and save ABI bindings. */
struct RshMessageManager;
static inline u16 rsh1m_msgGetStatus(RshMessageManager* mng) { return gabi::call<u16>(0x025F795C, mng); }
static inline void rsh1m_msgSetStatus(RshMessageManager* mng, u32 st) { gabi::call(0x025F74D0, mng, st); }
static inline u32 rsh1m_msgSet(RshMessageManager* mng, u32 msgNo, void* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
static inline void rsh1m_demoMsgFlagOn() { gabi::call(0x025DB58C); }
static inline BOOL rsh1m_CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline u8 rsh1m_play_u8(u32 off) { return gabi::load<u8>(dComIfGp_ea() + off); }
static inline void rsh1m_play_set_u8(u32 off, u8 v) { gabi::store<u8>(dComIfGp_ea() + off, v); }
static inline u32 rsh1m_save() { return gabi::load<u32>(0x101F84DC); }
static inline BOOL rsh1m_isEventBit(u16 f) { return gabi::call<BOOL>(0x025B8B94, rsh1m_save() + 0x644, f); }
static inline void rsh1m_onEventBit(u16 f) { gabi::call(0x025B8B68, rsh1m_save() + 0x644, f); }
static inline BOOL rsh1m_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline BOOL rsh1m_checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
static inline void rsh1m_execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
static inline s32 rsh1m_BoughtErrorStatus(u32 items, s32 a, s32 price) { return gabi::call<s32>(0x025BBA1C, items, a, price); }
static inline BOOL rsh1m_now_triggercheck(void* stick, u32 items, be<u32>* msg, u32 cb, u32 cbArg) {
    return gabi::call<BOOL>(0x025BB698, stick, items, msg, cb, cbArg);
}
static inline void rsh1m_seStart(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }
static inline BOOL rsh1m_checkReserveItem(u8 item) { return gabi::call<BOOL>(0x025B7570, rsh1m_save() + 0x96, item); }
static inline BOOL rsh1m_normalizeRS(cXyz* v) { return gabi::call<BOOL>(0x0201B47C, v); }
static inline f32 rsh1m_addCalcPos(cXyz* pos, cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200EE00, pos, target, scale, maxStep, minStep);
}
/* HD-only save flags: *(save + 0x12C0) + 0x8C (02720118), a 32-bit word; 0271F954 tests, 0271F968 sets */
static inline u32 rsh1m_hdFlags() { return gabi::call<u32>(0x02720118, rsh1m_save() + 0x12C0); }
static inline BOOL rsh1m_hdFlagCheck(u32 p, u32 bit) { return gabi::call<BOOL>(0x0271F954, p, bit); }
static inline void rsh1m_hdFlagOn(u32 p, u32 bit) { gabi::call(0x0271F968, p, bit); }
/* ShopCam_action_c::rsh_talk_cam_action_init(actor, cXyz ctr, cXyz eye, f32 fovy) (cXyz by value) */
static inline void rsh1m_talkCamInit(RshShopCamera* cam, fopAc_ac_c* a, cXyz* ctr, cXyz* eye, f32 fovy) {
    gabi::call(0x025BC1B0, cam, a, ctr, eye, fovy);
}


// HD: the manager replaces GameCube's msg_class pointer; these are its verified fields.
struct RshMessageManager {
    u8 _padding[0x922];
    be<u8> mCancel;
    u8 _padding923[0x938 - 0x923];
    be<u32> mMsgNo;
    u8 _padding93C[0x948 - 0x93C];
    be<u32> mSelectNum;
};
WWHD_OFFSET(RshMessageManager, mCancel, 0x922);
WWHD_OFFSET(RshMessageManager, mMsgNo, 0x938);
WWHD_OFFSET(RshMessageManager, mSelectNum, 0x948);
WWHD_SIZE(RshMessageManager, 0x94C);
struct RshCameraView {
    u8 _padding[0xE8];
    cXyz mCenter;
};
WWHD_OFFSET(RshCameraView, mCenter, 0xE8);
WWHD_SIZE(RshCameraView, 0xF4);
struct RshPlayerState : fopAc_ac_c {
    u8 _padding3AC[0x3B8 - 0x3AC];
    be<u32> mNoResetFlg0;
};
WWHD_OFFSET(RshPlayerState, mNoResetFlg0, 0x3B8);
WWHD_SIZE(RshPlayerState, 0x3BC);
struct RshPathPoint { be<u8> mArg0, mArg1, mArg2, mArg3; cXyz m_position; };
struct RshPath { be<u16> m_num, m_nextID; be<u8> mArg0, m_closed, mArg2, mArg3; gptr<RshPathPoint> m_points; };
WWHD_OFFSET(RshPath, m_points, 8);
WWHD_SIZE(RshPath, 0xC);
WWHD_OFFSET(RshPathPoint, m_position, 4);
WWHD_SIZE(RshPathPoint, 0x10);
inline RshPlayerState* rsh1_playerState(fopAc_ac_c* p) { return reinterpret_cast<RshPlayerState*>(p); }
// 025E7820: HD ABI used by Zunari.
inline void rsh1_btpConstruct(void* a) { return gabi::call<void>(0x025E7820, a); }
// 0259DAA0: HD ABI used by Zunari.
inline void rsh1_jointControlConstruct(dNpc_JntCtrl_c* a) { return gabi::call<void>(0x0259DAA0, a); }
// 0259F740: HD ABI used by Zunari.
inline void rsh1_eventCutConstruct(dNpc_EventCut_c* a) { return gabi::call<void>(0x0259F740, a); }
// 025885C4: HD ABI used by Zunari.
inline void rsh1_stickWaitParm(RshStickControl* a, s32 l, s32 r, s32 u, s32 d, f32 value, f32 threshold, s32 repeat, s32 angle) { return gabi::call<void>(0x025885C4, a, l, r, u, d, value, threshold, repeat, angle); }
// 025885E8: HD ABI used by Zunari.
inline void rsh1_stickInit(RshStickControl* a) { return gabi::call<void>(0x025885E8, a); }
// 025BBDF0: HD ABI used by Zunari.
inline void rsh1_shopCameraConstruct(RshShopCamera* a) { return gabi::call<void>(0x025BBDF0, a); }
// 028EFFD0: HD ABI used by Zunari.
inline void rsh1_constructItems(RshShopItems* a, s32 count, s32 size, u32 ctor) { return gabi::call<void>(0x028EFFD0, a, count, size, ctor); }
// 025E7B3C: HD ABI used by Zunari.
inline void rsh1_btpEntry(void* a, J3DModelData* d, u32 frame) { return gabi::call<void>(0x025E7B3C, a, d, frame); }
// 025BEBB8: HD ABI used by Zunari.
inline void rsh1_registerFigure(s32 type, daNpc_Rsh1_c* a, cXyz* p, s32 angle, f32 sx, f32 sy, f32 sz) { return gabi::call<void>(0x025BEBB8, type, a, p, angle, sx, sy, sz); }
// 0259DA18: HD ABI used by Zunari.
inline void rsh1_npcHioConstruct(dNpc_HIO_c* a) { return gabi::call<void>(0x0259DA18, a); }
// 025D87E4: HD ABI used by Zunari.
inline u32 rsh1_createShopItem(u32 p, u32 item, csXyz* angle, s32 room, s32 arg4, s32 arg5) { return gabi::call<u32>(0x025D87E4, p, item, angle, room, arg4, arg5); }
// 025BCFA0: HD ABI used by Zunari.
inline void rsh1_setItemDataList(RshShopItems* a, be<u32>* list) { return gabi::call<void>(0x025BCFA0, a, list); }
// 02515F14: HD ABI used by Zunari.
inline void rsh1_statusInit(dCcD_Stts* a, s32 weight, s32 flags, daNpc_Rsh1_c* actor) { return gabi::call<void>(0x02515F14, a, weight, flags, actor); }
// 02516518: HD ABI used by Zunari.

// 0259F7D4: HD ABI used by Zunari.
inline void rsh1_setActorInfo(dNpc_EventCut_c* a, const char* name, daNpc_Rsh1_c* actor) { return gabi::call<void>(0x0259F7D4, a, name, actor); }
// 025D7A58: HD ABI used by Zunari.
inline void rsh1_orderOtherEventId(daNpc_Rsh1_c* a, s32 event, s32 map, s32 arg3, s32 arg4, s32 arg5) { return gabi::call<void>(0x025D7A58, a, event, map, arg3, arg4, arg5); }
// 025D77DC: HD ABI used by Zunari.
inline void rsh1_orderOtherEvent(daNpc_Rsh1_c* a, const char* event, s32 priority, s32 flags) { return gabi::call<void>(0x025D77DC, a, event, priority, flags); }
// 0259D54C: HD ABI used by Zunari.
inline void rsh1_playerEyePos(cXyz* out, f32 offset) { return gabi::call<void>(0x0259D54C, out, offset); }
// 0259DED0: HD ABI used by Zunari.
inline void rsh1_lookAtTarget(dNpc_JntCtrl_c* a, be<s16>* angle, cXyz* target, cXyz* eye, s32 base, s32 velocity, u32 headOnly) { return gabi::call<void>(0x0259DED0, a, angle, target, eye, base, velocity, headOnly); }
// 024EF130: HD ABI used by Zunari.
inline s8 rsh1_groundRoomId(dBgS* bg, cBgS_PolyInfo* poly) { return gabi::call<s8>(0x024EF130, bg, poly); }
// 024EEEB8: HD ABI used by Zunari.
inline u8 rsh1_groundColor(dBgS* bg, cBgS_PolyInfo* poly) { return gabi::call<u8>(0x024EEEB8, bg, poly); }
// 022D8D44: HD ABI used by Zunari.
inline u8 rsh1_attentionByte(daNpc_Rsh1_c* a, cXyz* p, s32 angle) { return gabi::call<u8>(0x022D8D44, a, p, angle); }
// 025D7DEC: HD ABI used by Zunari.
inline u32 rsh1_createPresent(cXyz* p, u32 item, s32 arg2, s32 arg3, s32 room, s32 arg5, s32 arg6) { return gabi::call<u32>(0x025D7DEC, p, item, arg2, arg3, room, arg5, arg6); }
// 0254457C: HD ABI used by Zunari.
inline BOOL rsh1_endCheckOld(dEvent_manager_c* a, const char* name) { return gabi::call<BOOL>(0x0254457C, a, name); }
// 02542EDC: HD ABI used by Zunari.
inline s32 rsh1_getActIdx(dEvent_manager_c* a, s32 staff, u32 names, s32 count, s32 force, s32 arg5) { return gabi::call<s32>(0x02542EDC, a, staff, names, count, force, arg5); }
// 028F026C: HD ABI used by Zunari.
inline void rsh1_registerGlobal(u32 record) { return gabi::call<void>(0x028F026C, record); }
// 028ED6F8: HD ABI used by Zunari.
inline void rsh1_initializeHeaderByte(u32 value) { return gabi::call<void>(0x028ED6F8, value); }
// 028EAB2C: HD ABI used by Zunari.
inline void rsh1_initializeHeaderFlag(u32 value) { return gabi::call<void>(0x028EAB2C, value); }
// 022DB690: HD ABI used by Zunari.
inline void rsh1_constructHio(u32 hio) { return gabi::call<void>(0x022DB690, hio); }
// 0273AF40: HD ABI used by Zunari.
inline void rsh1_operatorDelete(void* value) { return gabi::call<void>(0x0273AF40, value); }
// 025886D8: HD ABI used by Zunari.
inline void rsh1_stickDestroy(RshStickControl* a, s32 flags) { return gabi::call<void>(0x025886D8, a, flags); }
// 02018034: HD ABI used by Zunari.
inline void rsh1_circleDestroy(u32 a, s32 flags) { return gabi::call<void>(0x02018034, a, flags); }
// 024EFD9C: HD ABI used by Zunari.
inline void rsh1_acchDestroy(u32 a, s32 flags) { return gabi::call<void>(0x024EFD9C, a, flags); }
// 025D50BC: HD ABI used by Zunari.
inline void rsh1_actorDestroy(daNpc_Rsh1_c* a, s32 flags) { return gabi::call<void>(0x025D50BC, a, flags); }

// HD: scalar word copies load every component before any destination store.
struct RshVectorWords { be<u32> x, y, z; };
WWHD_SIZE(RshVectorWords, 0xC);
inline RshVectorWords& rsh1_words(cXyz& v) { return *reinterpret_cast<RshVectorWords*>(&v); }
inline BOOL rsh1_chasePosXZ(cXyz* p, cXyz* target, f32 step) { return gabi::call<BOOL>(0x0200F764, p, target, step); }
inline void rsh1_outprod(cXyz* p, cXyz* out, cXyz* other) { gabi::call(0x0201B080, p, out, other); }
inline bool rsh1_normalizeRS(cXyz* p) { return gabi::call<bool>(0x0201B47C, p); }
inline cXyz& rsh1_baseY() { return *gabi::at<cXyz>(0x101FFBC0); }
inline RshCameraView* rsh1_camera() { return gabi::at<RshCameraView>(gabi::load<u32>(dComIfGp_ea() + 0x5AF8)); }
inline RshMessageManager* rsh1_messageManager() { return gabi::at<RshMessageManager>(gabi::load<u32>(0x101F4B5C)); }
inline RshPath* rsh1_path(dPath* p) { return reinterpret_cast<RshPath*>(p); }
inline void rsh1_removeBtp(J3DModelData* d) { *gabi::at<be<u32>>(gabi::ea(d) + 0x38) = 0; }

enum {
 dRes_INDEX_RSH_BCK_RS_PUSH_e = 21, dRes_INDEX_RSH_BCK_RS_TALK01_e = 23,
 dRes_INDEX_RSH_BCK_RS_TALK02_e = 24, dRes_INDEX_RSH_BCK_RS_WAIT01_e = 27,
 dRes_INDEX_RSH_BCK_RS_WAIT02_e = 28, dRes_INDEX_RSH_BCK_RS_WALK_e = 29,
 dRes_INDEX_RSH_BAS_RS_PUSH_e = 9, dRes_INDEX_RSH_BAS_RS_TALK01_e = 11,
 dRes_INDEX_RSH_BAS_RS_TALK02_e = 12, dRes_INDEX_RSH_BAS_RS_WAIT01_e = 15,
 dRes_INDEX_RSH_BAS_RS_WAIT02_e = 16, dRes_INDEX_RSH_BAS_RS_WALK_e = 17,
 dRes_INDEX_RSH_BDL_RS_e = 32, dRes_INDEX_RSH_BMD_SHOP_CURSOR01_e = 35,
 dRes_INDEX_RSH_BRK_SHOP_CURSOR01_e = 38, dRes_INDEX_RSH_BTP_RS_MABA01_e = 41,
 DSNAP_TYPE_NPC_RSH1 = 0x5F
};

inline void rsh1_subtract(cXyz* a, cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }

// HD: one owner-source view of this translation unit's writable statics.
struct RshStatics {
    /* 0x000 */ be<u32> mMessageId;
    /* 0x004 */ be<f32> mNegativePi, mPositivePi;
    /* 0x00C */ be<u32> mTalkAreaGuard;
    /* 0x010 */ be<u8> mHeaderByte, mHeaderFlag;
    u8 _padding012[2];
    /* 0x014 */ cXyz mShopArea1[4];
    /* 0x044 */ cXyz mShopArea2[4];
    /* 0x074 */ cXyz mTalkArea[4];
    /* 0x0A4 */ cXyz mTalkCenter[2];
    /* 0x0BC */ cXyz mTalkEye[2];
    /* 0x0D4 */ be<u32> mHeaderZeros[4];
    /* 0x0E4 */ daNpc_Rsh1_HIO_c mHIO;
    /* 0x150 */ be<u32> mTalkCenterGuard, mTalkEyeGuard;
};
WWHD_OFFSET(RshStatics, mShopArea1, 0x14);
WWHD_OFFSET(RshStatics, mShopArea2, 0x44);
WWHD_OFFSET(RshStatics, mTalkArea, 0x74);
WWHD_OFFSET(RshStatics, mTalkCenter, 0xA4);
WWHD_OFFSET(RshStatics, mTalkEye, 0xBC);
WWHD_OFFSET(RshStatics, mHIO, 0xE4);
WWHD_OFFSET(RshStatics, mTalkCenterGuard, 0x150);
WWHD_SIZE(RshStatics, 0x158);
RshStatics& rsh1_statics();
inline be<f32>* rsh1_components(cXyz* vectors) { return reinterpret_cast<be<f32>*>(vectors); }

inline gptr<__shop_items_set_data>* rsh1_shopItemData() { return gabi::at<gptr<__shop_items_set_data>>(0x101EB940); }
inline ShopItems_c__ItemData* rsh1_itemData(__shop_items_set_data* data) { return gabi::at<ShopItems_c__ItemData>(data->mpItemData); }
inline cXyz* rsh1_shopItemPositions() { return gabi::at<cXyz>(0x101EB998); }
inline dCcD_SrcCyl* rsh1_cylinderSource() { return gabi::at<dCcD_SrcCyl>(0x101C60A0); }
inline bool rsh1_forceWait() { return *gabi::at<be<u8>>(0x101D5F45) != 0; }
inline void rsh1_setStickVtable(RshStickControl* stick) { stick->mVtable = 0x10050788; }
inline void rsh1_setActorVtable(daNpc_Rsh1_c* a) { *gabi::at<be<u32>>(gabi::ea(a) + 0xB4) = RSH1_VTBL; }
enum : u32 { RSH1_CYL_AAB_VTBL = 0x10021664, RSH1_HIO_VTBL = 0x100216B4,
 RSH1_HEADER_RECORD_0 = 0x101C6178, RSH1_HEADER_RECORD_1 = 0x101C6184, RSH1_HEADER_RECORD_2 = 0x101C6190,
 RSH1_SHOP_CAM_ACTION = 0x025BBF4C, RSH1_TALK_CAM_ACTION = 0x025BC38C };

static inline void rsh1m_setAction(daNpc_Rsh1_c* a, u32 fn) {
    daNpc_Rsh1_c::ProcStorage* cur = &a->mCurrProc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn) {
        return;
    }
    if (i != 0) {
        u32 thisp = gabi::ea(a) + (s32)(s16)cur->d;
        s16 ci = cur->i;
        a->mActionStatus = -1;
        if (ci < 0) {
            gabi::call_ptr<BOOL>(cur->f, thisp, (u32)0);
        } else {
            u32 vt = gabi::load<u32>(thisp + (s32)gabi::load<s16>(gabi::ea(cur) + 6));
            gabi::call_ptr<BOOL>(gabi::load<u32>(vt + ci * 8 + 4), thisp, (u32)0);
        }
    }
    cur->d = 0;
    u32 thisp = gabi::ea(a) + (s32)(s16)cur->d;
    cur->i = -1;
    cur->f = fn;
    a->mActionStatus = 0;
    gabi::call_ptr<BOOL>(cur->f, thisp, (u32)0);
}

enum : u32 { RSH1_next_msgStatus = 0x022D8F58 };
static inline u32 rsh1m_next_msgStatus(daNpc_Rsh1_c* a, be<u32>* p, be<u32>* p2) {
    return gabi::call<u32>(RSH1_next_msgStatus, a, p, p2);
}

inline u16 rsh1_jointNumber(J3DJoint* j) { return *gabi::at<be<u16>>(gabi::ea(j) + 4); }

u32 rsh1_cutNames();

enum { dItemNo_DELIVERY_BAG_e = 0x30 };

enum { fpcNm_PLAYER_e = 0xA8, dItemNo_TOWN_FLOWER_e = 0x8C, dItemNo_SAIL_e = 0x78, dItemNo_MAGIC_ARMOR_e = 0x2A, dItemNo_SWIFT_SAIL_e = 0x77, dActStts_CHOOSE_e = 0x17, dActStts_CANCEL_e = 0x27 };
