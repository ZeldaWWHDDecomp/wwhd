/* daNpc_Bms1_c (Bomb-Master Cannon, Windfall bomb shop), WWHD layout.
 *
 * The GameCube TU is "Nonmatching" (its header only places dNpc_HeadAnm_c at 0x290 and the shop
 * members at 0x7F8..0x898): the layout is measured from the WWHD code (_create 02209DB4 inlines
 * the constructor; CreateHeap, CreateInit, the node callbacks). Base fopAc_ac_c (0x3AC), not
 * fopNpc_npc_c. GameCube -> WWHD: dNpc_HeadAnm_c +0x11C; the shop members +0x170
 * (ShopCam_action_c, HD 0x50: its pointer to member is 8 bytes) then +0x168 (ShopItems_c 0x44,
 * ShopCursor_c*). Unidentified canonical GameCube fields retain their tww names. The code is close
 * to d_a_npc_bs1 (Beedle, decompiled): names follow it where the use is the same. */
#pragma once
#include "bindings.h"
#include "d/d_shop.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/d/d_com_inf_game.h"

/* ---- shared NPC / shop classes, WWHD layouts (SHARED-CANDIDATE) ---- */

/* dNpc_HeadAnm_c (HD 0x20, GameCube 0x22): csXyz, the swing pointer to member (8 bytes, GHS),
 * two floats, three s16. Constructor inline (zeroes all but the pointer to member). */

/* STControl (HD 0x28: GameCube members 0x04..0x28 at 0x00, the vtable at +0x24) */
struct Bms1StickControl {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(Bms1StickControl, 0x28);

/* ShopCam_action_c (HD 0x50, constructor 025BBDF0): the action pointer to member (8 bytes) and
 * 8 more bytes, then GameCube m18.. at -8 */
struct Bms1ShopCam : ShopCam_action_c {
    void Reset() {
        gabi::call(0x025BC620, this);
    }
    void Save() {
        gabi::call(0x025BC5B0, this);
    }
    BOOL shop_cam_action_init() {
        return gabi::call<BOOL>(0x025BBE98, this);
    }
    void move() {
        gabi::call(0x025BC714, this);
    }
};
WWHD_SIZE(Bms1ShopCam, 0x50);

/* ShopItems_c (HD 0x44 as GameCube, constructor 025BC760) */
struct Bms1ShopItems : ShopItems_c {
    void setItemSetDataList() {
        gabi::call(0x025BCF84, this);
    }
    void createItem(s32 a, s32 room) {
        gabi::call(0x025BC810, this, a, room);
    }
    void Item_Move() {
        gabi::call(0x025BCCA4, this);
    }
    BOOL Item_ZoomUp(cXyz* pos) {
        return gabi::call<BOOL>(0x025BCC7C, this, pos);
    }
    void hideSelectItem() {
        gabi::call(0x025BCD90, this);
    }
    u8 getSelectItemNo() {
        return gabi::call<u8>(0x025BD22C, this);
    }
    void showItem() {
        gabi::call(0x025BCE04, this);
    }
    u32 getSelectItemBuyMsg() {
        return gabi::call<u32>(0x025BD278, this);
    }
    void getSelectItemPos(cXyz* out) {
        gabi::call(0x025BCFA8, this, out);
    }
    void getSelectItemBasePos(cXyz* out) {
        gabi::call(0x025BD0EC, this, out);
    }
};
WWHD_SIZE(Bms1ShopItems, 0x44);

/* ShopCursor_c: only the members the NPCs touch (show/hide flag +0xB4, setScale +0x98..+0xB0) */
struct Bms1ShopCursor : ShopCursor_c {
    void show() {
        m54 = 1;
    }
    void hide() {
        m54 = 0;
    }
    void setScale(f32 a, f32 b, f32 c, f32 d, f32 e) {
        m48 = a;
        m4C = b;
        m50 = c;
        m38 = d;
        m3C = e;
    }
    void setPos(cXyz* p) {
        gabi::call(0x025BD31C, this, p);
    }
    void anm_play() {
        gabi::call(0x025BD290, this);
    }
    void draw() {
        gabi::call(0x025BD338, this);
    }
};

/* daNpc_Bms1_HIO_c (HD 0x60, static l_HIO at 0x10466650): mNo +0, m8 +4, one child (0x54) at +8,
 * vtable +0x5C. daNpc_Bms1_childHIO_c: dNpc_HIO_c (HD: members of GameCube 0x04.. at 0x00) at +4,
 * own floats +0x2C.., vtable +0x50. */
struct daNpc_Bms1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m8;
    /* 0x08 */ be<u32> child_unk; /* child +0: not written by the constructors */
    /* 0x0C */ be<f32> m04;       /* dNpc_HIO_c: playerEyePos offset */
    /* 0x10 */ be<s16> mMaxHeadX;
    /* 0x12 */ be<s16> mMaxBackboneX;
    /* 0x14 */ be<s16> mMaxHeadY;
    /* 0x16 */ be<s16> mMaxBackboneY;
    /* 0x18 */ be<s16> mMinHeadX;
    /* 0x1A */ be<s16> mMinBackboneX;
    /* 0x1C */ be<s16> mMinHeadY;
    /* 0x1E */ be<s16> mMinBackboneY;
    /* 0x20 */ be<s16> mMaxTurnStep;
    /* 0x22 */ be<s16> mMaxHeadTurnVel;
    /* 0x24 */ be<f32> mAttnYOffset;
    /* 0x28 */ be<s16> mMaxAttnAngleY;
    /* 0x2A */ be<u8> m22;
    /* 0x2B */ u8 _2B;
    /* 0x2C */ be<f32> mMaxAttnDistXZ;
    /* 0x30 */ be<u32> npc_vtbl;        /* dNpc_HIO_c vtable (HD: after its members) */
    /* 0x34 */ be<f32> mCursorScale[5]; /* ShopCursor scale (Bs1 m30..m40) */
    /* 0x48 */ be<f32> mHairSpring;     /* hair: velocity gain */
    /* 0x4C */ be<f32> mHairDamp;       /* hair: velocity damping */
    /* 0x50 */ be<f32> mHairSlerp;      /* hair: quaternion slerp rate */
    /* 0x54 */ be<f32> mHairStretch;    /* hair: stretch factor */
    /* 0x58 */ be<u32> child_vtbl;
    /* 0x5C */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Bms1_HIO_c, 0x60);
// HD: typed guest views also support the separately verified translation-unit parts.
// Storage is defined once in d_a_npc_bms1.cpp.
inline daNpc_Bms1_HIO_c& bms1HIO() {
    return *gabi::at<daNpc_Bms1_HIO_c>(0x10466650);
}
/* static fpc_ProcID l_msgId */
inline be<u32>& bms1MessageId() {
    return *gabi::at<be<u32>>(0x10466618);
}

enum : u32 {
    BMS1_VTBL = 0x10017D54,
    BMS1_SAFESTRING_VTBL = 0x10017CFC,
    PMF_wait_action = 0x0220BDCC,
    PMF_getdemo_action = 0x0220BF90,
    PMF_event_action = 0x0220C7CC,
};
#define BMS1_ARC "Bms" /* "Bms" (shared by _create, _delete, CreateHeap, setAnm) */

struct daNpc_Bms1_c : fopAc_ac_c {
    /* GHS pointer to member (8 bytes) */
    struct ActionFunc {
        /* 0x0 */ be<s16> d;
        /* 0x2 */ be<s16> i;
        /* 0x4 */ be<u32> f;
    };

    BOOL initTexPatternAnm(u32 i_modify); /* bool, passed as the full register */
    u32 setTexAnm(s8 i_idx);
    BOOL CreateHeap();
    void set_mtx();
    BOOL CreateInit();
    cPhs_State _create();
    BOOL _delete();
    void playTexPatternAnm();
    void demo_end_init();
    BOOL demo_move();
    void talkInit();
    void checkOrder();
    void eventOrder();
    void setCollision();
    BOOL _execute();
    BOOL _draw();
    void setAnm(s8 i_idx, f32 i_morf);
    void setAnmFromMsgTag();
    BOOL chkAttention(cXyz* i_pos, s16 i_angle);
    u32 next_msgStatus(be<u32>* pMsgNo, be<u32>* pMsgNo2); /* u16 status, returned in the full register */
    u32 getMsg();
    u16 normal_talk();
    u16 shop_talk();
    u16 talk();
    void setAttention(bool i_force);
    void lookBack();
    BOOL checkPlayerLanding();
    BOOL wait01();
    BOOL talk01();
    BOOL wait_action(void*);
    BOOL getdemo_action(void*);
    BOOL evn_talk_init(int i_staffIdx);
    BOOL evn_continue_talk_init(int i_staffIdx);
    BOOL evn_viblation_init(int i_staffIdx);
    BOOL evn_head_swing_init(int i_staffIdx);
    BOOL evn_talk();
    BOOL privateCut();
    BOOL event_action(void*);

    /* setAction (inline): GHS compares the pointer to member with {0, -1, fn} */
    void setAction(u32 fn, void* arg) {
        if (mAction.i == -1 && mAction.d == 0 && mAction.f == fn)
            return;
        if (mAction.i != 0) {
            void* p = gabi::at<void>(gabi::ea(this) + (s16)mAction.d);
            s16 idx = mAction.i;
            mActionStatus = -1;
            if (idx < 0) {
                gabi::call_ptr(mAction.f, p, (u32)0);
            } else {
                u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(gabi::ea(&mAction) + 6));
                gabi::call_ptr(gabi::load<u32>(vt + idx * 8 + 4), p, (u32)0);
            }
        }
        mActionStatus = 0;
        mAction.d = 0;
        mAction.i = -1;
        mAction.f = fn;
        gabi::call_ptr(mAction.f, this, arg);
    }
    /* (this->*mAction)(NULL) */
    void callAction() {
        s16 idx = mAction.i;
        void* p = gabi::at<void>(gabi::ea(this) + (s16)mAction.d);
        if (idx < 0) {
            gabi::call_ptr(mAction.f, p, (u32)0);
        } else {
            u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(gabi::ea(&mAction) + 6));
            gabi::call_ptr(gabi::load<u32>(vt + idx * 8 + 4), p, (u32)0);
        }
    }

    /* 0x3AC */ dNpc_HeadAnm_c mHeadAnm;
    /* 0x3CC */ cXyz mHairScaleL; /* hairL joint scale */
    /* 0x3D8 */ cXyz mHairScaleR;
    /* 0x3E4 */ be<f32> mHairStretchL; /* dynamic scale factor of the hair joints */
    /* 0x3E8 */ be<f32> mHairStretchR;
    /* 0x3EC */ cXyz mHairPosL; /* hair tip positions (spring) */
    /* 0x3F8 */ cXyz mHairPosR;
    /* 0x404 */ cXyz mHairVelL;
    /* 0x410 */ cXyz mHairVelR;
    /* 0x41C */ be<f32> mHairQuatL[4];
    /* 0x42C */ be<f32> mHairQuatR[4];
    /* 0x43C */ request_of_phase_process_class mPhs;
    /* 0x444 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x448 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x44C */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x4C0 */ be<u8> mFrame;
    /* 0x4C1 */ u8 _4C1;
    /* 0x4C2 */ be<s16> mBlinkTimer;
    /* 0x4C4 */ gptr<J3DModel> mpHeadModel;      /* res 0x23: the head (hair joints, btp) */
    /* 0x4C8 */ gptr<J3DModel> mpHeadAccessory;  /* res 0x1B */
    /* 0x4CC */ gptr<J3DModel> mpHeadAccessory2; /* res 0x1C */
    /* 0x4D0 */ gptr<J3DModel> mpBodyAccessory;  /* res 0x1E / 0x1F (type 0) */
    /* 0x4D4 */ gptr<J3DModel> mpShopModel;      /* res 0x20 (type 0) */
    /* 0x4D8 */ dBgS_ObjAcch mAcch;
    /* 0x69C */ dBgS_AcchCir mAcchCir;
    /* 0x6DC */ dCcD_Stts mStts;
    /* 0x718 */ dCcD_Cyl mCyl;
    /* 0x848 */ be<s8> m_head_jnt_num;
    /* 0x849 */ be<s8> m_backbone_jnt_num;
    /* 0x84A */ be<s8> m_hairL_jnt_num; /* joints of the head model */
    /* 0x84B */ be<s8> m_hairR_jnt_num;
    /* 0x84C */ be<s8> mAccessoryJoint; /* body joint the model at 0x4D0 follows */
    /* 0x84D */ u8 _84D[3];
    /* 0x850 */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x884 */ dNpc_EventCut_c mEventCut;
    /* 0x8F0 */ Bms1StickControl mStickControl;
    /* 0x918 */ be<s8> mTriggerHandled; /* shop_talk: trigger just handled */
    /* 0x919 */ u8 _919[0x928 - 0x919];
    /* 0x928 */ cXyz mAttnBasePos;
    /* 0x934 */ be<s16> mHeadTurnVel;
    /* 0x936 */ csXyz mInitialAngle; /* the angle at creation */
    /* 0x93C */ u8 _93C[2];
    /* 0x93E */ be<u8> mAnimationLooped; /* mpMorf->play() result */
    /* 0x93F */ be<u8> mAttentionMode;   /* setAttention */
    /* 0x940 */ be<u8> mPlayerInRange;   /* player in attention range */
    /* 0x941 */ be<u8> mTalkRequested;   /* talk requested */
    /* 0x942 */ u8 _942[2];
    /* 0x944 */ be<f32> mPrevFrame;
    /* 0x948 */ be<f32> mPlayerHeight; /* player height (landing check) */
    /* 0x94C */ be<u32> mMsgNo;
    /* 0x950 */ be<u32> mMsgNo2;
    /* 0x954 */ be<u32> mEndMsgNo;
    /* 0x958 */ be<u32> mNextMsgNo; /* getMsg: preset message */
    /* 0x95C */ be<u8> mBoughtItemNo;
    /* 0x95D */ be<u8> mbDemo;
    /* 0x95E */ u8 _95E[2];
    /* 0x960 */ ActionFunc mAction;
    /* 0x968 */ Bms1ShopCam mShopCam;
    /* 0x9B8 */ Bms1ShopItems mShopItems;
    /* 0x9FC */ gptr<Bms1ShopCursor> mpShopCursor;
    /* 0xA00 */ be<s8> mTexIdx;
    /* 0xA01 */ be<s8> mAnmIdx;
    /* 0xA02 */ be<s8> mAnimationRepeats; /* anm repeat count / sound timer */
    /* 0xA03 */ be<s8> mOrderEvt;         /* 1/2 talk, 3 getdemo, 4 event */
    /* 0xA04 */ be<s8> mMode;             /* 1 wait, 2 talk */
    /* 0xA05 */ be<s8> mPrevMode;
    /* 0xA06 */ be<s8> mType;
    /* 0xA07 */ u8 _A07;
    /* 0xA08 */ be<s8> mActionStatus;
    /* 0xA09 */ be<s8> mTalkStep;
    /* 0xA0A */ be<u8> mShopInitialized;
    /* 0xA0B */ be<u8> mBoughtBombItem; /* evn_talk: bought bomb item */
    /* 0xA0C */ u8 _A0C;
    /* 0xA0D */ be<u8> mbCreateErr;
    /* 0xA0E */ u8 _A0E[2];
};
WWHD_OFFSET(daNpc_Bms1_c, mPhs, 0x43C);
WWHD_OFFSET(daNpc_Bms1_c, mAcch, 0x4D8);
WWHD_OFFSET(daNpc_Bms1_c, mCyl, 0x718);
WWHD_OFFSET(daNpc_Bms1_c, mJntCtrl, 0x850);
WWHD_OFFSET(daNpc_Bms1_c, mStickControl, 0x8F0);
WWHD_OFFSET(daNpc_Bms1_c, mAttnBasePos, 0x928);
WWHD_OFFSET(daNpc_Bms1_c, mAction, 0x960);
WWHD_OFFSET(daNpc_Bms1_c, mShopCam, 0x968);
WWHD_OFFSET(daNpc_Bms1_c, mShopItems, 0x9B8);
WWHD_OFFSET(daNpc_Bms1_c, mpShopCursor, 0x9FC);
WWHD_OFFSET(daNpc_Bms1_c, mbCreateErr, 0xA0D);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* bms1_getRes(s32 idx) {
    return dComIfG_getObjectRes(BMS1_ARC, idx, BMS1_SAFESTRING_VTBL);
}
static inline void* bms1_getRes(const char* arc, s32 idx) {
    return dComIfG_getObjectRes(arc, idx, BMS1_SAFESTRING_VTBL);
}

static inline BOOL checkItemGet(u8 item, s32 flag) {
    return gabi::call<BOOL>(0x0254DA50, item, flag);
}
static inline void execItemGet(u8 item) {
    gabi::call(0x0254DA38, item);
}
static inline s32 mDoExt_btpAnm_init_l(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr,
                                       f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 J3DAnmTexPattern_getFrameMax_l(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 4) + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) {
    return gabi::call<s16>(0x02055B64, t);
}
static inline u32 J3DModel_modelData_l(J3DModel* m) {
    return gabi::ea(m->mModelData.get());
}
/* J3DModel::getAnmMtx (HD: the joint matrix block at +0x2C, marked dirty) */
static inline Mtx34* J3DModel_getAnmMtx_l(J3DModel* m, s32 jnt) {
    J3DMtxBuffer_hd* blk = m->mpMtxBuffer;
    blk->mFlags = (u16)blk->mFlags | 0x10;
    return gabi::at<Mtx34>((u32)blk->mpAnmMtx + jnt * 0x30);
}
static inline u32 j3dSys_getModel_l() {
    return gabi::load<u32>(0x104B462C);
}
static inline Mtx34* j3dSys_mCurrentMtx_l() {
    return gabi::at<Mtx34>(0x104B4868);
}
/* the self-relative joint name table of J3DModelData */
static inline s8 J3DModelData_getJointIndex_l(J3DModelData* d, const char* name) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    u32 tab = off != 0 ? h + 0x10 + off : 0;
    return gabi::call<s8>(0x027DF9B0, tab, name); /* JUTNameTab::getIndex */
}
static inline u16 J3DModelData_getJointNum_l(u32 d) {
    return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8);
}
static inline void J3DModelData_setJointCallBack_l(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n)
        p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline void dNpc_setAnm_2(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 anm, s32 snd,
                                 const char* arc) {
    gabi::call(0x0259D79C, m, loop, morf, speed, anm, snd, arc);
}
/* HD messages: the manager (*101F4B5C); 025F795C returns the status, 025F74D0 sets it */
static inline u32 l_msgMng_l() {
    return gabi::load<u32>(0x101F4B5C);
}
static inline u16 fopMsgM_getStatus_l(u32 mng) {
    return gabi::call<u16>(0x025F795C, mng);
}
static inline void fopMsgM_setStatus_l(u32 mng, u32 st) {
    gabi::call(0x025F74D0, mng, st);
}
static inline u32 fopMsgM_messageSet_l(u32 mng, u32 msgNo, cXyz* pos) {
    return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos);
}
static inline void fopMsgM_demoMsgFlagOn_l() {
    gabi::call(0x025DB58C);
}
/* 020078BC: pad trigger check (port 0 only) */
static inline BOOL CPad_CHECK_TRIG_l(s32 port) {
    return gabi::call<BOOL>(0x020078BC, port);
}
static inline s32 dShop_BoughtErrorStatus_l(Bms1ShopItems* items, s32 a, s32 price) {
    return gabi::call<s32>(0x025BBA1C, items, a, price);
}
static inline BOOL dShop_now_triggercheck_l(Bms1StickControl* stick, Bms1ShopItems* items, be<u32>* msg,
                                            u32 cb, void* cbArg) {
    return gabi::call<BOOL>(0x025BB698, stick, items, msg, cb, cbArg);
}
static inline Bms1ShopCursor* ShopCursor_create_l(void* mdl, void* brk, f32 scale) {
    return gabi::call<Bms1ShopCursor*>(0x025BBD7C, mdl, brk, scale);
}

static inline BOOL dComIfGp_evmng_endCheckOld_l(const char* name) {
    return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name);
}
static inline void* dDemo_object_getActor_l(u32 obj, u8 id) {
    return gabi::call<void*>(0x02526E70, obj, id);
}
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData_l(void* ac, const char* arc) {
    return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc);
}
static inline BOOL dDemo_setDemoData_l(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n,
                                       void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
static inline void fopAcM_orderOtherEvent2_l(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    gabi::call(0x025D77DC, a, name, flag, hind);
}
static inline u32 fopAcM_createItemForPresentDemo_l(cXyz* pos, s32 item, u8 argFlag, s32 bitNo, s32 roomNo,
                                                    csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, item, argFlag, bitNo, roomNo, angle, scale);
}
static inline s32 dComIfGp_evmng_getMyActIdx_l(dEvent_manager_c* m, s32 staffId, u32 tbl, s32 n, s32 force,
                                               s32 nameType) {
    return gabi::call<s32>(0x02542EDC, m, staffId, tbl, n, force, nameType);
}
static inline BOOL dEvent_manager_getIsAddvance_l(dEvent_manager_c* m, s32 staffId) {
    return gabi::call<BOOL>(0x025447C8, m, staffId);
}
static inline void dEvent_manager_cutEnd_l(dEvent_manager_c* m, s32 staffId) {
    gabi::call(0x02543280, m, staffId);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) {
    gabi::call(0x0259D54C, out, offs);
}
static inline void dNpc_JntCtrl_lookAtTarget_l(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye,
                                               s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* the player (daPy_py_c) through its HD vtable (+0xB4) */
static inline u32 player_vfunc(fopAc_ac_c* pl, u32 slot) {
    return gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + slot);
}
/* daPy_py_c::offPlayerNoDraw: status word +0x3B8 */
static inline void daPy_offNoDraw_l(fopAc_ac_c* pl) {
    u32 a = gabi::ea(pl) + 0x3B8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
}
static inline f32 McaMorf_curMorf_l(mDoExt_McaMorf* m) {
    return gabi::load<f32>(gabi::ea(m) + 0xB0);
}
/* the morf's J3DFrameCtrl::checkPass(getEndFrame() - 1.0f) */
static inline BOOL McaMorf_checkEnd_l(mDoExt_McaMorf* m) {
    f32 end = (f32)(s16)m->mFrameCtrl.mEnd;
    return m->mFrameCtrl.checkPass(end - 1.0f);
}
static inline void mDoAud_seStart_l(u32 id, cXyz* pos, u32 param, s32 reverb) {
    gabi::call(0x025E1A40, id, pos, param, reverb);
}

WWHD_SIZE(daNpc_Bms1_c, 0xA10);

struct l_btp_ix_tblData {
    be<s32> mEntries[2];
};
extern l_btp_ix_tblData& l_btp_ix_tbl;

struct morf_frame_tblData {
    be<f32> mEntries[7];
};
extern morf_frame_tblData& morf_frame_tbl;

struct play_mode_tblData {
    be<s32> mEntries[7];
};
extern play_mode_tblData& play_mode_tbl;

struct play_speed_tblData {
    be<f32> mEntries[7];
};
extern play_speed_tblData& play_speed_tbl;

struct l_bck_ix_tblData {
    be<s32> mEntries[7];
};
extern l_bck_ix_tblData& l_bck_ix_tbl;

namespace bms1_cpp {
struct ChildHIO {
    be<u32> mUnknown;
    dNpc_HIO_c mNpc;
    be<f32> mSettings[9];
    be<u32> __vtbl;
};
WWHD_SIZE(ChildHIO, 0x54);
WWHD_OFFSET(ChildHIO, mSettings, 0x2C);
struct AcchVtables {
    u8 _00[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mActorVtable;
};
WWHD_SIZE(AcchVtables, 0x24);
} // namespace bms1_cpp
inline void bms1_HIOConstruct(dNpc_HIO_c* hio) {
    gabi::call(0x0259DA18, hio);
}
inline void bms1_constructChildren(void* p, u32 count, u32 size, u32 constructor) {
    gabi::call(0x028EFFD0, p, count, size, constructor);
}
inline void bms1_STControlDestroy(Bms1StickControl* p, s32 flags) {
    gabi::call(0x025886D8, p, flags);
}
inline void bms1_circleDestroy(void* p, s32 flags) {
    gabi::call(0x02018034, p, flags);
}
inline void bms1_acchDestroy(dBgS_ObjAcch* p, s32 flags) {
    gabi::call(0x024EFD9C, p, flags);
}
inline void bms1_actorDestroy(fopAc_ac_c* p, s32 flags) {
    gabi::call(0x025D50BC, p, flags);
}

WWHD_SIZE(Bms1ShopCursor, 0xB8);

// HD: generated constructor calls retain the original full argument registers.
inline void bms1_jointControlConstruct(dNpc_JntCtrl_c* p) {
    gabi::call(0x0259DAA0, p);
}
inline void bms1_eventCutConstruct(dNpc_EventCut_c* p) {
    gabi::call(0x0259F740, p);
}
inline void bms1_shopCamConstruct(Bms1ShopCam* p) {
    gabi::call(0x025BBDF0, p);
}
inline void bms1_shopItemsConstruct(Bms1ShopItems* p) {
    gabi::call(0x025BC760, p);
}
namespace bms1_cpp {
struct QuaternionWords {
    be<u32> mWords[4];
};
struct ModelMaterialAnimation {
    u8 mPadding[0x38];
    be<u32> mpTexturePattern;
};
WWHD_SIZE(QuaternionWords, 0x10);
WWHD_OFFSET(ModelMaterialAnimation, mpTexturePattern, 0x38);
} // namespace bms1_cpp
extern daNpc_Bms1_HIO_c& l_HIOStorage;
extern be<u32>& l_msgIdStorage;
extern cXyz& l_hairBase;
extern cXyz& l_hairTip;
extern be<u32>& l_hairBaseGuard;
extern be<u32>& l_hairTipGuard;
extern bms1_cpp::QuaternionWords& l_identityQuaternion;

// HD: ABI bindings used by the separately compiled parts.
/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void PSMTXConcat_l(Mtx34* a, Mtx34* b, Mtx34* ab) {
    gabi::call(0x028E9108, a, b, ab);
}
static inline void PSVECSubtract_l(cXyz* a, cXyz* b, cXyz* ab) {
    gabi::call(0x028E8DAC, a, b, ab);
}
static inline void mDoMtx_stack_quatM_l(be<f32>* q) {
    gabi::call(0x025F25CC, q);
}
static inline void daObj_quat_rotVec_l(void* q, cXyz* a, cXyz* b) {
    gabi::call(0x023127F8, q, a, b);
}
static inline void C_QUATSlerp_l(void* p, void* q, void* r, f32 t) {
    gabi::call(0x028E9BC0, p, q, r, t);
}
static inline bool cXyz_isZero_l(cXyz* v) {
    return gabi::call<bool>(0x0201B4E0, v);
}
/* HD dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_l(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angY, x, y, z);
}
static inline void dNpc_EventCut_setActorInfo_l(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a) {
    gabi::call(0x0259F7D4, c, name, a);
}
static inline void STControl_setWaitParm_l(Bms1StickControl* s, s16 a, s16 b, s16 c, s16 d, f32 e, f32 f,
                                           s16 g, s16 h) {
    gabi::call(0x025885C4, s, a, b, c, d, e, f, g, h);
}
static inline void STControl_init_l(Bms1StickControl* s) {
    gabi::call(0x025885E8, s);
}
static inline void mDoMtx_stack_copy_to_model(J3DModel* m) {
    J3DModel_setBaseTRMtx(m, mDoMtx_stack_c::get());
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline bool cXyz_normalizeRS_l(cXyz* v) {
    return gabi::call<bool>(0x0201B47C, v);
}
static inline BOOL bms1ShopMessageCheck(u32 msgNo) {
    return gabi::call<BOOL>(0x02208334, msgNo);
}
static inline BOOL bms1ShopStickMessageCheck(u32 msgNo) {
    return gabi::call<BOOL>(0x0220836C, msgNo);
}
static inline s8 bms1CalcTimer(be<s8>* t) {
    return gabi::call<s8>(0x0220CCB4, t);
}
// HD: shop message fields accessed directly by the original inline operations.
struct Bms1MessageView {
    u8 mPadding[0x922];
    be<u8> mSendRequested;
    be<u8> mSelectionChanged;
    u8 mPadding924[0x14];
    be<u32> mMessageNo;
    u8 mPadding93C[0xC];
    be<u32> mChoice;
};
WWHD_OFFSET(Bms1MessageView, mChoice, 0x948);
static inline Bms1MessageView* bms1Message(u32 address) {
    return gabi::at<Bms1MessageView>(address);
}
static inline dComIfG_play_c* bms1Play() {
    return gabi::at<dComIfG_play_c>(dComIfGp_ea() + 0x12A0);
}

inline void bms1SetPlayerPosAndAngle(fopAc_ac_c* player, cXyz* position, s16 angle) {
    gabi::call_ptr(player_vfunc(player, 0x114), player, position, angle);
}
inline BOOL bms1PlayerAirborne(fopAc_ac_c* player) {
    return gabi::call_ptr<BOOL>(player_vfunc(player, 0x4C), player);
}
inline s32 bms1BackgroundRoom(dBgS_ObjAcch* acch) {
    return gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gabi::ea(acch) + 0xD4 + 0x14);
}
inline s32 bms1BackgroundEnvironment(dBgS_ObjAcch* acch) {
    return gabi::call<s32>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(acch) + 0xD4 + 0x14);
}
inline cXyz* bms1HairBase() {
    return gabi::at<cXyz>(0x10466638);
}
inline cXyz* bms1HairTip() {
    return gabi::at<cXyz>(0x10466644);
}
inline be<u32>& bms1HairBaseGuard() {
    return *gabi::at<be<u32>>(0x104666B0);
}
inline be<u32>& bms1HairTipGuard() {
    return *gabi::at<be<u32>>(0x104666B4);
}
inline bms1_cpp::QuaternionWords* bms1IdentityQuaternion() {
    return gabi::at<bms1_cpp::QuaternionWords>(0x101E9C38);
}
inline dCcD_SrcCyl* bms1CylinderSource() {
    return gabi::at<dCcD_SrcCyl>(0x101BD2D4);
}
struct Bms1CutNames {
    gptr<const char> mNames[4];
};
inline Bms1CutNames* bms1CutNames() {
    return gabi::at<Bms1CutNames>(0x101BD36C);
}
extern Bms1CutNames& l_cutNames;
extern dCcD_SrcCyl& l_cyl_src;
extern gptr<void>& l_demoObject;

// HD: event orders stored in the actor's one-byte order field.
enum Bms1EventOrder {
    BMS1_ORDER_NONE = 0,
    BMS1_ORDER_TALK = 1,
    BMS1_ORDER_SHOP = 2,
    BMS1_ORDER_GET_ITEM = 3,
    BMS1_ORDER_LANDING = 4,
};
