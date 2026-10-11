/* daNpc_De1_c (the Great Deku Tree), WWHD layout. 
 *
 * The GameCube header has no members (all functions are "Nonmatching" stubs), so the layout is
 * measured from the WWHD code (the inline constructor in _create and the field accesses of all
 * the unit's functions). Fields without a known meaning are named after their offset.
 * Base: fopNpc_npc_c (0x7DC in HD). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct De1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(De1ActionFunc, 8);

struct daNpc_De1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ u8 _01[3];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    BOOL CreateHeap();
    BOOL decideType(int);
    BOOL set_action(De1ActionFunc*, void*);
    void followPa_happa();
    void setDemoStartCenter();
    fopAc_ac_c* searchByID(u32);
    void cc_set();
    void setAttention();
    void setMtx();
    BOOL createInit();
    cPhs_State _create();
    void del_pa_happa();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    void endEvent();
    BOOL anmResID(u32, be<s32>*, be<s32>*);
    void set_pa_happa();
    BOOL setAnm_anm(anm_prm_c*);
    BOOL setAnm();
    void setStt(u32); /* s8, compared unnormalised */
    BOOL setAnm_NUM(int);
    void event_actionInit(int);
    BOOL event_action();
    void privateCut();
    void event_proc();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    BOOL setAnm_ATR();
    void chngAnmAtr(u32); /* u8, compared unnormalised */
    void ctrlAnmAtr();
    void anmAtr(u32); /* u16, compared unnormalised */
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u8 chkAttention();
    BOOL partner_srch();
    void ccCreate();
    BOOL wait01();
    BOOL wait02();
    BOOL wait03();
    BOOL wait04();
    BOOL wait05();
    BOOL talk01();
    BOOL talk02();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<dBgW> mpBgW;   /* dBgWDeform (0xC4, HD vtable at +4) */
    /* 0x7E8 */ be<u32> mDeformRes; /* skinned collision vertices ("de.cvtx"): resource */
    /* 0x7EC */ be<u32> mDeformVtx; /*   and the vertex buffer the BG reads (+0x90) */
    /* 0x7F0 */ De1ActionFunc mAction;
    /* 0x7F8 */ dNpc_EventCut_c
        mEventCut2;               /* the actor's own dNpc_EventCut_c (constructor 0259F740) */
    /* 0x864 */ csXyz mInitAngle; /* _execute: current.angle on the first frame */
    /* 0x86A */ u8 _86A[2];
    /* 0x86C */ cXyz mInitPos; /* _execute: current.pos on the first frame */
    /* 0x878 */ cXyz mEyePos;  /* setAttention: copy of mAttPos */
    /* 0x884 */ cXyz mAttPos;
    /* 0x890 */ cXyz mHomePos; /* createInit */
    /* 0x89C */ cXyz mDemoStartCenter;
    /* 0x8A8 */ be<f32> mAnmFrame;
    /* 0x8AC */ u8 _8AC[4];
    /* 0x8B0 */ be<s16> mSeTimer;
    /* 0x8B2 */ be<s16> mEvtTimer;
    /* 0x8B4 */ u8 _8B4[2];
    /* 0x8B6 */ be<s16> mEvtLiftNo;
    /* 0x8B8 */ u8 _8B8[4];
    /* 0x8BC */ be<s8> mAnmEnd;
    /* 0x8BD */ be<u8> mLoopCnt;
    /* 0x8BE */ be<u8> mMesgAnimeTag;
    /* 0x8BF */ be<u8> mbSecondTalk; /* getMsg: second talk */
    /* 0x8C0 */ be<u8> mInitDone;
    /* 0x8C1 */ u8 _8C1[3];
    /* 0x8C4 */ be<s32> mEyePosEnabled;
    /* 0x8C8 */ u8 _8C8[4];
    /* 0x8CC */ be<u8> mAttention; /* chkAttention result */
    /* 0x8CD */ be<u8> mTalkReq;
    /* 0x8CE */ u8 _8CE;
    /* 0x8CF */ be<u8> mInDemo;
    /* 0x8D0 */ be<u8> mTalkReset;
    /* 0x8D1 */ u8 _8D1[3];
    /* 0x8D4 */ dPa_followEcallBack mPaHappa; /* leaf particle (0x81BA) on joint branchL */
    /* 0x8E8 */ cXyz mPaHappaPos;
    /* 0x8F4 */ be<u32> mLiftId[10]; /* leaf lifts created by ccCreate */
    /* 0x91C */ be<s8> mLiftJnt[10]; /* joints the leaf lifts follow */
    /* 0x926 */ u8 _926[2];
    /* 0x928 */ be<u32> mPartnerId; /* partner_srch: leaf lift (proc 0x78) */
    /* 0x92C */ be<s8> mActIdx;
    /* 0x92D */ be<s8> mEvtActNo;
    /* 0x92E */ be<u8> mAnmAtr;
    /* 0x92F */ be<u8> mMsgAnmAtr;
    /* 0x930 */ be<s8> mAnmNum;
    /* 0x931 */ be<s8> mOrder; /* eventOrder: 1/2 talk, 3.. other events */
    /* 0x932 */ be<s8> mStt;
    /* 0x933 */ be<s8> mPrevStt;
    /* 0x934 */ be<s8> mSubtype;
    /* 0x935 */ be<s8> mType; /* decideType: Farore's... symbol 2 owned */
    /* 0x936 */ be<s8> mActPhase;
    /* 0x937 */ be<s8> mMsgAtrInit;
    /* 0x938 */ be<s8> mHeadJnt;
    /* 0x939 */ be<s8> mBranchJnt;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpc_De1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_De1_c, mAction, 0x7F0);
WWHD_OFFSET(daNpc_De1_c, mEventCut2, 0x7F8);
WWHD_OFFSET(daNpc_De1_c, mInitAngle, 0x864);
WWHD_OFFSET(daNpc_De1_c, mPaHappa, 0x8D4);
WWHD_OFFSET(daNpc_De1_c, mLiftId, 0x8F4);
WWHD_OFFSET(daNpc_De1_c, mLiftJnt, 0x91C);
WWHD_OFFSET(daNpc_De1_c, mPartnerId, 0x928);
WWHD_OFFSET(daNpc_De1_c, mBranchJnt, 0x939);
WWHD_SIZE(daNpc_De1_c,
          0x93C); /* profile g_profile_NPC_DE1 (0x101BE0D0): size 0x93C, proc name 0x74 */

// HD: virtual frame count, animation-removal and destructor layouts.
struct De1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct De1Anm {
    u8 _0[4];
    gptr<De1AnmVtable> mpVtable;
};
struct De1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct De1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(De1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(De1Anm, mpVtable, 4);
WWHD_OFFSET(De1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(De1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(De1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* De1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 De1_frameMax(J3DAnmTexPattern* p) {
    auto* a = De1_view<De1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void De1_loadAction(De1ActionFunc* dst, De1ActionFunc* src) {
    auto* d = De1_view<be<u32>>(dst);
    auto* s = De1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct De1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void De1_callAction(void* self, De1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = De1_view<De1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void De1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

// HD: collision deformation, leaf-lift attachments and player demo fields used by this actor.
struct daNpc_De1_HIO_c {
    struct hio_prm_c {
        be<f32> mAttOffsetY, mAttDist, mScale, mCallDist;
        cXyz mDemoStartOffset;
    };
    be<s8> mNo;
    u8 _1[3];
    be<s32> mCount;
    hio_prm_c mPrm;
    be<u32> __vtbl;
};
struct De1BgW {
    u8 _0[4];
    be<u32> mVtable;
    u8 _8[0x90 - 8];
    be<u32> mpVertices;
};
struct De1LeafLift {
    u8 _0[0x403];
    be<u8> mMoveRequested;
    u8 _404[0x8F4 - 0x404];
    Mtx34 mFollowMtx;
};
struct De1Player {
    u8 _0[0xB4];
    gptr<be<u32>> mpVtable;
    u8 _B8[0x420 - 0xB8];
    be<u16> mDemoMode;
    be<s16> mDemoAngle;
    u8 _424[4];
    be<s32> mDemoParam0;
    u8 _42C[4];
    be<s32> mDemoParam1;
};
struct De1EventPartner {
    u8 _0[0xD0];
    be<u32> mItemPartner;
};
struct De1Message {
    u8 _0[0x948];
    be<u32> mSelectNum;
};
struct De1AnmRes {
    be<s32> mBck, mSound;
};
struct De1MorfFrame {
    u8 _0[0x9C];
    be<u32> mFrameBits;
};
typedef be<u32> De1Word;
typedef be<s32> De1Int;
inline void De1_copyBits(void* dst, const void* src) {
    *De1_view<be<u32>>(dst) = *De1_view<be<u32>>(src);
}
inline fopAc_ac_c* De1_actor(u32 a) { return gabi::at<fopAc_ac_c>(a); }
inline s8 De1_lowByte(be<s32>* p) { return De1_view<be<s8>>(p)[3]; }
inline s16 De1_lowHalf(be<s32>* p) { return De1_view<be<s16>>(p)[1]; }
void* De1_namedResource(dRes_control_c*, SafeString*, SafeString*);
inline void* De1_objectResource(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->__vtbl = gc_tu.safeString;
    n->__vtbl = gc_tu.safeString;
    n->mStringTop = gabi::guest_addr(name);
    a->mStringTop = gabi::guest_addr(arc);
    return De1_namedResource(dComIfG_resControl(), a.get(), n.get());
}
WWHD_OFFSET(daNpc_De1_HIO_c, mNo, 0);
WWHD_OFFSET(daNpc_De1_HIO_c, mCount, 4);
WWHD_OFFSET(daNpc_De1_HIO_c, mPrm, 8);
WWHD_OFFSET(daNpc_De1_HIO_c, __vtbl, 0x24);
WWHD_SIZE(daNpc_De1_HIO_c, 0x28);
WWHD_SIZE(daNpc_De1_HIO_c::hio_prm_c, 0x1C);
WWHD_OFFSET(De1BgW, mVtable, 4);
WWHD_OFFSET(De1BgW, mpVertices, 0x90);
WWHD_SIZE(De1BgW, 0x94);
WWHD_OFFSET(De1LeafLift, mMoveRequested, 0x403);
WWHD_OFFSET(De1LeafLift, mFollowMtx, 0x8F4);
WWHD_SIZE(De1LeafLift, 0x924);
WWHD_OFFSET(De1Player, mpVtable, 0xB4);
WWHD_OFFSET(De1Player, mDemoMode, 0x420);
WWHD_OFFSET(De1Player, mDemoAngle, 0x422);
WWHD_OFFSET(De1Player, mDemoParam0, 0x428);
WWHD_OFFSET(De1Player, mDemoParam1, 0x430);
WWHD_SIZE(De1Player, 0x434);
WWHD_OFFSET(De1EventPartner, mItemPartner, 0xD0);
WWHD_SIZE(De1EventPartner, 0xD4);
WWHD_OFFSET(De1Message, mSelectNum, 0x948);
WWHD_SIZE(De1Message, 0x94C);
WWHD_SIZE(De1AnmRes, 8);
WWHD_OFFSET(De1AnmRes, mSound, 4);
WWHD_OFFSET(De1MorfFrame, mFrameBits, 0x9C);
WWHD_SIZE(De1MorfFrame, 0xA0);
WWHD_SIZE(De1AcchVtables, 0x24);
WWHD_OFFSET(De1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(De1VirtualAction, 8);
WWHD_OFFSET(De1ActionFunc, d, 0);
WWHD_OFFSET(De1ActionFunc, i, 2);
WWHD_OFFSET(De1ActionFunc, f, 4);

void De1_bgwCtor(void*);
bool De1_bgwSet(dBgW*, void*, u32);
void De1_copyBackVertices(dBgW*);
BOOL De1_deformInit(void*, void*);
void De1_deformCalc(void*, J3DModel*);
void De1_searchById(u32, be<u32>*);
s8 De1_roomId(dBgS*, cBgS_PolyInfo*);
u8 De1_polyColour(dBgS*, cBgS_PolyInfo*);
u32 De1_firstMessage(u16, u32, u32);
BOOL De1_startNamedEvent(dEvent_manager_c*, const char*);
BOOL De1_endNamedEvent(dEvent_manager_c*, const char*);
u32 De1_eventPid(dEvt_control_c*, fopAc_ac_c*);
u32 De1_messageStatus(void*);
void De1_nextStage(const char*, s16, s8, s8, f32, u32, BOOL, s8);
BOOL De1_orderNamedEvent(fopAc_ac_c*, u32, u16, u16);
void De1_audioRequest(u32);
BOOL De1_moveUpLift(fopAc_ac_c*);
BOOL De1_isLiftDown(fopAc_ac_c*);
s16 De1_calcTimer(be<s16>*);
void De1_npcCtor(daNpc_De1_c*);
void De1_eventCutCtor(dNpc_EventCut_c*);
void* De1_demoGetActor(u32, u8);
BOOL De1_demoData(daNpc_De1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void De1_cutEnd(dEvent_manager_c*, s32);
BOOL De1_cutAdvance(dEvent_manager_c*, s32);
void De1_circleDestroy(void*, s32);
void De1_acchDestroy(dBgS_Acch*, s32);
void De1_actorDestroy(daNpc_De1_c*, s32);
void* De1_namedResource(dRes_control_c*, SafeString*, SafeString*);
enum De1HdConstants { DE1_LEAF_SEARCH_NAME = 0x78, DE1_LEAF_CREATE_NAME = 0xCE };

inline void De1_playerDemo(fopAc_ac_c* player, s32 mode) {
    auto* vt = De1_view<De1Player>(player)->mpVtable.get();
    gabi::call_ptr(vt[0xE4 / 4], player, mode);
}
WWHD_OFFSET(daNpc_De1_c::anm_prm_c, mAnmNum, 0);
WWHD_OFFSET(daNpc_De1_c::anm_prm_c, mMorf, 4);
WWHD_OFFSET(daNpc_De1_c::anm_prm_c, mSpeed, 8);
WWHD_OFFSET(daNpc_De1_c::anm_prm_c, mLoopMode, 0xC);
WWHD_SIZE(daNpc_De1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_De1_c, mEyePos, 0x878);
WWHD_OFFSET(daNpc_De1_c, mEyePosEnabled, 0x8C4);
WWHD_OFFSET(daNpc_De1_c, mTalkReset, 0x8D0);
WWHD_OFFSET(daNpc_De1_c, mSubtype, 0x934);

enum { fpcNm_NPC_DE1_e=0x74 };
