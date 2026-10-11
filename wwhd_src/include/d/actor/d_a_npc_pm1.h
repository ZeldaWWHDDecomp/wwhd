/* daNpc_Pm1_c (Maggie, poor, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (no members in its header). The WWHD code is the same as
 * d_a_npc_km1's apart from constants, so the layout is Km1's (d_a_npc_km1.h), confirmed by every
 * function of the unit: size 0x944 (profile at 0x101C53DC, process name 0x162). Field names follow
 * the GameCube Km1 header. */
#pragma once
#include "gc_api/gc_api.h"

struct daNpc_Pm1_c;
using Pm1Action = gabi::ptmf<BOOL (daNpc_Pm1_c::*)(void *)>;
WWHD_SIZE(Pm1Action, 8);

// HD: getObjectIDRes uses archive IDs, rather than GameCube resource indices.
enum dRes_ID_PM {
    dRes_ID_PM_BAS_FIND_e = 0,
    dRes_ID_PM_BAS_TALK01_e = 1,
    dRes_ID_PM_BAS_WAIT01_e = 2,
    dRes_ID_PM_BCK_FIND_e = 3,
    dRes_ID_PM_BCK_TALK01_e = 4,
    dRes_ID_PM_BCK_WAIT01_e = 5,
    dRes_ID_PM_BDL_PM_e = 6,
    dRes_ID_PM_BTP_MABA01_e = 7,
};
struct daNpc_Pm1_c : fopNpc_npc_c {
    enum Animation { ANM_WAIT, ANM_TALK, ANM_END };
    enum TexPattern { TEXPATTERN_MABA, TEXPATTERN_END };
    enum State { STATE_WAIT = 1, STATE_TALK = 2 };
    enum LookMode { LOOK_PLAYER = 1, LOOK_POSITION = 2, LOOK_ANGLE = 3 };
    enum MessageStatus { MSG_TYPING = 6, MSG_DISPLAYED = 14, MSG_DESTROYED = 19 };
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
    bool anmResID(int, be<s32> *, be<s32> *);
    void BtpNum2ResID(int, be<s32> *);
    u32 setAnm_tex(s8);
    bool init_btp(u32, int);
    bool initTexPatternAnm(u32);
    void playTexPatternAnm();
    s32 setAnm_anm(anm_prm_c *);
    void setAnm();
    void chngAnmAtr(u32);
    void setAnm_ATR(int);
    void anmAtr(u16);
    void setStt(s8);
    u16 next_msgStatus(be<u32> *);
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
    bool set_action(Pm1Action *, void *);
    BOOL wait01();
    BOOL talk01();
    BOOL wait_action1(void *);
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
    /* 0x7E8 */ gptr<J3DAnmTexPattern>
        m_head_tex_pattern;            /* HD: no mShadowId / shadow model before it */
    /* 0x7EC */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> field_0x6F1;
    /* 0x862 */ be<s16> field_0x6F2;
    /* 0x864 */ Pm1Action mAction; /* field_0x6F4 */
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
WWHD_OFFSET(daNpc_Pm1_c, m_head_tex_pattern, 0x7E8);
WWHD_OFFSET(daNpc_Pm1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Pm1_c, mEventCut, 0x86C);
WWHD_OFFSET(daNpc_Pm1_c, mEyePos, 0x8F8);
WWHD_OFFSET(daNpc_Pm1_c, field_0x7BC, 0x928);
WWHD_OFFSET(daNpc_Pm1_c, field_0x7CD, 0x939);
WWHD_SIZE(daNpc_Pm1_c, 0x944);

// HD: these signatures preserve byte/word arguments instead of the GameCube bool forms.
struct Pm1PatternVtable {
    u8 padding[0x14];
    gfn getFrameMax;
};
struct Pm1Pattern {
    be<f32> frame;
    gptr<Pm1PatternVtable> vtable;
};
WWHD_OFFSET(Pm1Pattern, vtable, 4);
inline s32 pm1_J3DAnm_getFrameMax(J3DAnmTexPattern *p) {
    return gabi::call_ptr<s32>(reinterpret_cast<Pm1Pattern *>(p)->vtable->getFrameMax.get(), p);
}
s32 pm1_mDoExt_btpAnm_init(void *, J3DModelData *, J3DAnmTexPattern *, s32, s32, f32, s32, s32, u32,
                           s32);
bool pm1_dAttention_LockonTruth(dAttention_c *);
fopAc_ac_c *pm1_dAttention_LockonTarget(dAttention_c *, s32);
fopAc_ac_c *pm1_dAttention_ActionTarget(dAttention_c *, s32);
s8 pm1_getMyActIdx(dEvent_manager_c *, s32, const gptr<char> *, s32, BOOL, s32);
static inline s8 pm1_dComIfGp_evmng_getMyActIdx(s32 staffId, const gptr<char> *tbl, s32 n,
                                                BOOL force, s32 nameType) {
    return pm1_getMyActIdx(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
void pm1_dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c *, be<s16> *, cXyz *, cXyz *, s16, s16, u8);
u32 pm1_messageStatus(msg_class *);
static inline u32 pm1_fopMsgM_getStatus() {
    return pm1_messageStatus(fopNpc_npc_c::mpCurrMsg.get());
}
s8 pm1_JUTNameTab_getIndex(u32, const char *);
s8 pm1_dBgS_GetRoomId(dBgS *, void *);
u8 pm1_dBgS_GetPolyColor(dBgS *, void *);
BOOL pm1_dDemo_setDemoData(fopAc_ac_c *, u8, mDoExt_McaMorf *, const char *, s32, void *, u32, s8);
struct Pm1ActionWords {
    be<u32> words[2];
};
WWHD_SIZE(Pm1ActionWords, 8);
inline void pm1_pmf_load(Pm1Action *dst, const be<u32> *src) {
    auto *out = reinterpret_cast<Pm1ActionWords *>(dst);
    out->words[0] = src[0];
    out->words[1] = src[1];
}
static inline void pm1_pmf_call(daNpc_Pm1_c *self, Pm1Action *action, void *arg) {
    (self->*(*action))(arg);
}

void pm1_constructNpc(fopNpc_npc_c *);
void pm1_constructBtp(void *);
void pm1_constructEventCut(dNpc_EventCut_c *);
struct Pm1ModelDataView {
    u8 padding[0x38];
    be<u32> patternAnimation;
};
WWHD_OFFSET(Pm1ModelDataView, patternAnimation, 0x38);
inline void pm1_removePattern(J3DModelData *p) {
    reinterpret_cast<Pm1ModelDataView *>(p)->patternAnimation = 0;
}
void *pm1_getDemoActor(void *, u8);

struct daNpc_Pm1_HIO_c {
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
WWHD_SIZE(daNpc_Pm1_HIO_c, 0x28);

WWHD_OFFSET(Pm1PatternVtable, getFrameMax, 0x14);
WWHD_SIZE(Pm1PatternVtable, 0x18);
WWHD_SIZE(Pm1Pattern, 8);
WWHD_SIZE(Pm1ModelDataView, 0x3C);
WWHD_OFFSET(daNpc_Pm1_HIO_c, mPrmTbl, 8);
WWHD_OFFSET(daNpc_Pm1_HIO_c, __vtbl, 0x24);
WWHD_SIZE(daNpc_Pm1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Pm1_c, mBtpAnm, 0x7EC);
