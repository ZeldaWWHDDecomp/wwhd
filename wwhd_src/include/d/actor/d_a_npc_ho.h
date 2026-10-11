/* daNpc_Ho_c (Mrs. Marie, Windfall teacher), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_ho: daNpc_Ho_c derives from
 * fopAc_ac_c directly (not fopNpc_npc_c), so its members are +0x11C up to mpJoyPendentModel;
 * mShadowId (GameCube 0x2A0) is gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x118;
 * mDoExt_btpAnm grew from 0x14 to 0x74, so everything from mBlinkFrame on is +0x178 up to
 * mCurrActionFunc; the pointer to member function is 8 bytes (GHS) instead of 12, so the
 * members after it are +0x174. Size 0x828 (GameCube 0x6B4). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member function, measured) */

struct daNpc_Ho_c : fopAc_ac_c {
    enum HoFlags {
        HO_FLAG_00000001 = 0x00000001,
        HO_FLAG_00000004 = 0x00000004,
        HO_FLAG_00000008 = 0x00000008,
        HO_FLAG_00000010 = 0x00000010,
        HO_FLAG_00000020 = 0x00000020,
    };
    enum HoStates {
        HO_STATE_WAIT_01 = 0,
        HO_STATE_TALK_01 = 1,
        HO_STATE_TALK_02 = 2,
        HO_STATE_TALK_03 = 3,
        HO_STATE_TALK_03_CONTINUE = 4,
        HO_STATE_GIVE_01 = 5,
        HO_STATE_GIVE_02 = 6,
        HO_STATE_PREACH = 7,
    };

    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum TalkStates { TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2, TALK_FINISHED = -1 };
    u32 ChkOrder(u8 flag) {
        return (u8)mOrderFlags & flag;
    }
    void ClrOrder() {
        mOrderFlags = 0;
    }
    void SetOrder(u8 v) {
        mOrderFlags = (s8)((u8)mOrderFlags | v);
    }
    bool chkFlag(u16 flag) {
        return (mFlags & flag) == flag;
    }
    void clrFlag(u16 flag) {
        mFlags = (u16)(mFlags & ~flag);
    }
    void setFlag(u16 flag) {
        mFlags = (u16)(mFlags | flag);
    }
    bool isMorf() {
        return mpMorf->mCurMorf < 1.0f;
    } /* mDoExt_McaMorf::isMorf: mCurMorf (+0xB0) < 1.0 */

    s16 XyCheckCB(int);
    void receivePendant(int);
    BOOL initTexPatternAnm(u32); /* bool */
    void playTexPatternAnm();
    void setAnm(s8);
    void setAnmStatus();
    bool chkAttentionLocal();
    void chkAttention();
    void eventOrder();
    void checkOrder();
    u32 next_msg_sub0(u32);
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void msgPushButton();
    void msgAnm(u8);
    void talkInit();
    u16 talk();
    BOOL init();
    void setAttention(bool);
    void lookBack();
    bool wait01();
    bool talk01();
    bool talk02();
    bool talk03();
    bool give01();
    bool give02();
    bool preach();
    BOOL wait_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DModel> mpJoyPendentModel;
    /* 0x3BC */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x3C0 */ mDoExt_btpAnm mBtpAnm;                     /* mDoExt_btpAnm (HD 0x74) */
    /* 0x434 */ be<u8> mBlinkFrame;
    /* 0x435 */ u8 _435;
    /* 0x436 */ be<s16> mBlinkTimer;
    /* 0x438 */ dBgS_ObjAcch mObjAcch;
    /* 0x5FC */ dBgS_AcchCir mAcchCir;
    /* 0x63C */ dCcD_Stts mStts;
    /* 0x678 */ dCcD_Cyl mCyl;
    /* 0x7A8 */ dNpc_JntCtrl_c m_jnt;
    /* 0x7DC */ cXyz mEyePos;
    /* 0x7E8 */ cXyz mAttnBasePos;
    /* 0x7F4 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x7F6 */ be<s8> mAnmEnded;
    /* 0x7F7 */ be<u8> mAttnSetCount;
    /* 0x7F8 */ be<f32> mAnmTimer;
    /* 0x7FC */ be<u32> mCurrMsgNo;
    /* 0x800 */ be<u16> mFlags;
    /* 0x802 */ be<u8> mAttentionTimer;
    /* 0x803 */ be<u8> mMsgSelectNum;
    /* 0x804 */ be<u8> mMsgAnmIdx;
    /* 0x805 */ be<u8> mAnmLoopCount;
    /* 0x806 */ be<u8> mItemNum;
    /* 0x807 */ u8 _807;
    /* 0x808 */ be<s32> mNextMessageId;
    /* 0x80C */ be<f32> mCylCollisionRadius;
    /* 0x810 */ ProcFunc_l mCurrActionFunc;
    /* 0x818 */ be<s8> mTexPatternIdx;
    /* 0x819 */ be<s8> mCurrAnmIdx;
    /* 0x81A */ be<s8> mOrderFlags;
    /* 0x81B */ be<s8> mState;
    /* 0x81C */ be<s8> mPrevState;
    /* 0x81D */ be<u8> mType;
    /* 0x81E */ be<s8> mActionStatus;
    /* 0x81F */ be<s8> mTalkState;
    /* 0x820 */ be<u32> mtrlSndId;
    /* 0x824 */ be<s8> mReverb;
    /* 0x825 */ u8 _825[3];
};
WWHD_OFFSET(daNpc_Ho_c, mBtpAnm, 0x3C0);
WWHD_OFFSET(daNpc_Ho_c, mObjAcch, 0x438);
WWHD_OFFSET(daNpc_Ho_c, mCyl, 0x678);
WWHD_OFFSET(daNpc_Ho_c, m_jnt, 0x7A8);
WWHD_OFFSET(daNpc_Ho_c, mCurrActionFunc, 0x810);
WWHD_OFFSET(daNpc_Ho_c, mReverb, 0x824);
WWHD_SIZE(daNpc_Ho_c, 0x828);

#include "gc_api/d/d_com_inf_game.h"
#include "d/d_item.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/SSystem/c_lib.h"

// HD: these adapters retain the original ABI widths, including unnormalised bool arguments.
BOOL hoNightStop();
BOOL hoEndCheck(dEvent_manager_c*, const char*);
BOOL hoTriggerA(s32);
BOOL hoTriggerB(s32);
bool hoLockonTruth(dAttention_c*);
fopAc_ac_c* hoLockonTarget(dAttention_c*, s32);
fopAc_ac_c* hoActionTarget(dAttention_c*, s32);
void hoPlayerEyePos(cXyz*, f32);
void hoLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
s32 hoBtpInit(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32);
s8 hoNameIndex(JUTNameTab*, const char*);
s8 hoRoomId(dBgS*, void*);
u8 hoPolyColor(dBgS*, void*);
u32 hoMaterialSound(dBgS*, void*);
void hoMorfEntry(mDoExt_McaMorf*);
u16 hoMessageStatus(msg_class*);
void hoSetMessageStatus(msg_class*, u32);
u32 hoMessageSet(msg_class*, u32, cXyz*);
void hoActorCtor(fopAc_ac_c*);
void hoBtpCtor(void*);
void hoJointCtor(dNpc_JntCtrl_c*);
void hoCircleDtor(void*, s32);
void hoAcchDtor(dBgS_ObjAcch*, s32);
void hoActorDtor(fopAc_ac_c*, s32);

namespace ho_cpp {
enum { dItemNo_NONE_e = 0xFF, fopMsgStts_MSG_DISPLAYED_e = 0xE, fopMsgStts_MSG_DESTROYED_e = 0x13 };
struct CylinderSource {
    u8 _bytes[0x44];
};
WWHD_SIZE(CylinderSource, 0x44);
enum : u32 { HO_VTABLE = 0x1001A5D0, CYL_VTABLE = 0x1001A590, WAIT_ACTION_ADDRESS = 0x02240B18 };
// HD: a NULL model-data pointer still addresses the animation slot at +0x38.
struct ModelDataView {
    u8 _00[0x38];
    gptr<J3DAnmTexPattern> mpAnmTexPattern;
};
WWHD_OFFSET(ModelDataView, mpAnmTexPattern, 0x38);
WWHD_SIZE(ModelDataView, 0x3C);
inline void removeTexPattern(J3DModelData* p) {
    gabi::store<u32>(gabi::ea(p) + offsetof(ModelDataView, mpAnmTexPattern), 0);
}
struct AcchVtables {
    u8 _00[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mActorVtable;
};
WWHD_OFFSET(AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(AcchVtables, mActorVtable, 0x20);
WWHD_SIZE(AcchVtables, 0x24);
inline AcchVtables* acch(dBgS_ObjAcch* p) {
    return reinterpret_cast<AcchVtables*>(p);
}
inline void* circle(dBgS_AcchCir* p) {
    return gabi::at<void>(gabi::ea(p) + 0x14);
}
struct AnimationVtable {
    u8 _00[0x14];
    be<u32> mGetFrameMax;
};
struct AnimationView {
    u8 _00[4];
    gptr<AnimationVtable> mpVtable;
};
WWHD_OFFSET(AnimationVtable, mGetFrameMax, 0x14);
WWHD_SIZE(AnimationVtable, 0x18);
WWHD_OFFSET(AnimationView, mpVtable, 4);
WWHD_SIZE(AnimationView, 8);
inline s32 getFrameMax(J3DAnmTexPattern* p) {
    auto* v = reinterpret_cast<AnimationView*>(p);
    return gabi::call_ptr<s32>(v->mpVtable->mGetFrameMax, p);
}
// HD: index arithmetic wraps in the guest's 32-bit address space.
template <class T> inline const T& entry(const T* table, s32 index) {
    return *gabi::at<T>(gabi::ea(table) + (u32)index * sizeof(T));
}
// HD: GHS member pointers carry a signed adjustment and an 8-byte virtual slot index.
inline void callAction(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = (s16)gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
inline BOOL hoEndCheck(const char* name) {
    return ::hoEndCheck(dComIfGp_getPEvtManager(), name);
}
} // namespace ho_cpp

BOOL hoOrderChangeEvent(fopAc_ac_c*, fopAc_ac_c*, const char*, u16, u16);
