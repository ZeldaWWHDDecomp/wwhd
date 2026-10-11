/* daNpc_Km1_c (Mila, rich), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_km1: fopNpc_npc_c is 0x7DC (see
 * d_a_npc_ba1.h), so members are +0x118 up to m_backbone_jnt_num. mShadowId and the shadow
 * model (GameCube 0x6D0/0x6D4) are gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x110;
 * mDoExt_btpAnm grew from 0x14 to 0x74 (mBtpFrame.. +0x170) and the pointer to member
 * function is 8 bytes (GHS) instead of 12, so mEventCut and everything after it is +0x16C.
 * Size 0x944 (profile g_profile_NPC_KM1 at 0x101BF764; GameCube 0x7D8). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
// HD: GHS action descriptors are eight bytes, with a signed this adjustment/index.
struct Km1ActionFunc { be<s16> d, i; be<u32> f; };
WWHD_SIZE(Km1ActionFunc, 8);

struct daNpc_Km1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    bool createInit();
    void setMtx();
    bool anmResID(int, be<s32>*, be<s32>*);
    void BtpNum2ResID(int, be<s32>*);
    u32 setAnm_tex(s8);
    bool init_btp(u32, int);
    bool initTexPatternAnm(u32);
    void playTexPatternAnm();
    s32 setAnm_anm(anm_prm_c*);
    void setAnm();
    void chngAnmAtr(u32);
    void setAnm_ATR(int);
    void anmAtr(u16);
    void setStt(s8);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    void lookBack();
    u8 chkAttention();
    void setAttention();
    bool decideType(int);
    void event_actionInit(int);
    bool event_action();
    void privateCut();
    void endEvent();
    void event_proc();
    bool set_action(Km1ActionFunc*, void*);
    BOOL wait01();
    BOOL talk01();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_backbone_jnt_num;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId / shadow model before it */
    /* 0x7EC */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> field_0x6F1;
    /* 0x862 */ be<s16> field_0x6F2;
    /* 0x864 */ Km1ActionFunc mAction;                         /* field_0x6F4 */
    /* 0x86C */ dNpc_EventCut_c mEventCut;
    /* 0x8D8 */ csXyz field_0x76C;
    /* 0x8DE */ u8 _8DE[2];
    /* 0x8E0 */ cXyz field_0x774;
    /* 0x8EC */ cXyz mAttPos;
    /* 0x8F8 */ cXyz mEyePos;
    /* 0x904 */ cXyz field_0x798;
    /* 0x910 */ be<f32> field_0x7A4;
    /* 0x914 */ u8 _914[4];
    /* 0x918 */ be<f32> field_0x7AC;
    /* 0x91C */ be<s16> field_0x7B0;
    /* 0x91E */ be<s16> field_0x7B2;
    /* 0x920 */ be<s8> field_0x7B4;
    /* 0x921 */ be<s8> field_0x7B5;
    /* 0x922 */ be<u8> field_0x7B6;
    /* 0x923 */ be<u8> field_0x7B7;
    /* 0x924 */ be<u8> field_0x7B8;
    /* 0x925 */ u8 _925[3];
    /* 0x928 */ be<s32> field_0x7BC;
    /* 0x92C */ be<s32> field_0x7C0;
    /* 0x930 */ be<u8> field_0x7C4;
    /* 0x931 */ be<u8> field_0x7C5;
    /* 0x932 */ be<u8> mHeadOnlyFollow;
    /* 0x933 */ be<u8> field_0x7C7;
    /* 0x934 */ be<u8> field_0x7C8;
    /* 0x935 */ be<s8> field_0x7C9;
    /* 0x936 */ be<s8> field_0x7CA;
    /* 0x937 */ be<u8> field_0x7CB;
    /* 0x938 */ be<u8> field_0x7CC;
    /* 0x939 */ be<s8> field_0x7CD;
    /* 0x93A */ be<s8> field_0x7CE;
    /* 0x93B */ be<s8> field_0x7CF;
    /* 0x93C */ be<s8> field_0x7D0;
    /* 0x93D */ be<s8> field_0x7D1;
    /* 0x93E */ be<s8> field_0x7D2;
    /* 0x93F */ be<s8> field_0x7D3;
    /* 0x940 */ be<s8> field_0x7D4;
    /* 0x941 */ be<s8> field_0x7D5;
    /* 0x942 */ be<s8> field_0x7D6;
    /* 0x943 */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Km1_c, m_head_tex_pattern, 0x7E8);
WWHD_OFFSET(daNpc_Km1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Km1_c, mEventCut, 0x86C);
WWHD_OFFSET(daNpc_Km1_c, mEyePos, 0x8F8);
WWHD_OFFSET(daNpc_Km1_c, field_0x7BC, 0x928);
WWHD_OFFSET(daNpc_Km1_c, field_0x7CD, 0x939);
WWHD_SIZE(daNpc_Km1_c, 0x944);

// HD: the HIO vtable moved to the end and the debug colour option is a byte.
struct daNpc_Km1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> field_0;
        /* 0x02 */ be<s16> field_2;
        /* 0x04 */ be<s16> field_4;
        /* 0x06 */ be<s16> field_6;
        /* 0x08 */ be<s16> field_8;
        /* 0x0A */ be<s16> field_A;
        /* 0x0C */ be<s16> field_C;
        /* 0x0E */ be<s16> field_E;
        /* 0x10 */ be<s16> field_10;
        /* 0x12 */ be<s16> field_12;
        /* 0x14 */ be<f32> mAttentionArrowYOffset;
        /* 0x18 */ be<u8> field_18; /* HD: a byte flag (debug colours in _draw) */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> field_0x8;
    /* 0x08 */ hio_prm_c mPrmTbl;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Km1_HIO_c, 0x28);
s16 Km1_calcTimer(be<s16>* t);
s32 Km1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
bool Km1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Km1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Km1_actionTarget(dAttention_c* a, s32 i);
void Km1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
BOOL Km1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7);
void* Km1_demoGetActor(u32 obj, u8 id);
void Km1_npcCtor(daNpc_Km1_c* p);
void Km1_btpCtor(void* p);
void Km1_eventCutCtor(dNpc_EventCut_c* p);
void Km1_circleDestroy(void* p, s32 flags);
void Km1_acchDestroy(dBgS_Acch* p, s32 flags);
void Km1_actorDestroy(daNpc_Km1_c* p, s32 flags);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Km1AnmVtable { u8 _0[0x14]; be<u32> mGetFrameMax; };
struct Km1Anm { u8 _0[4]; gptr<Km1AnmVtable> mpVtable; };
struct Km1ModelAnimation { u8 _0[0x38]; be<u32> mpBtpAnimation; };
struct Km1AcchVtables { u8 _0[0x14]; be<u32> mCheckVtable; u8 _18[8]; be<u32> mPolyVtable; };
WWHD_OFFSET(Km1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Km1Anm, mpVtable, 4);
WWHD_OFFSET(Km1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Km1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Km1AcchVtables, mPolyVtable, 0x20);
template<class T,class U> inline T* Km1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Km1_frameMax(J3DAnmTexPattern* p) { auto* a=Km1_view<Km1Anm>(p); auto* t=a->mpVtable.get(); return gabi::call_ptr<s32>(t->mGetFrameMax,p); }
inline void Km1_loadAction(Km1ActionFunc* dst, Km1ActionFunc* src) {
    auto* d=Km1_view<be<u32>>(dst); auto* s=Km1_view<be<u32>>(src);
    d[0]=s[0];d[1]=s[1];
}
struct Km1VirtualAction { u8 _0[6]; be<s16> mVtableOffset; };
inline void Km1_callAction(void* self, Km1ActionFunc* action, void* arg) {
    s16 index=action->i; u32 object=gabi::ea(self)+(s32)(s16)action->d;
    if(index<0) { gabi::call_ptr<BOOL>(action->f,object,arg); }
    else {
        s16 offset=Km1_view<Km1VirtualAction>(action)->mVtableOffset;
        auto* vtable=gabi::at<be<u32>>(object+offset);
        auto* entry=gabi::at<be<u32>>((u32)*vtable+index*8+4);
        gabi::call_ptr<BOOL>(*entry,object,arg);
    }
}
inline be<u32>* Km1_word(u32 a) { return gabi::at<be<u32>>(a); }
inline void Km1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if((u32)*guard==0) { *guard=1; memcpy_g(dst,src,4); }
}

s8 Km1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Km1_polyColour(dBgS*, cBgS_PolyInfo*);
