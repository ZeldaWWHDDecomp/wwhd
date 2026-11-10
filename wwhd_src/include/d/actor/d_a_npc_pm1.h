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
    /* 0x862 */ be<s16> mBlinkTimer;  /* was field_0x6F2: init_btp 022D0F4C 0; playTexPatternAnm 022D14A4: counted down by cLib_calcTimer while btp 0 plays, then reset to 30 + rnd(60) (after our decomp's daNpc_Pf1_c mBlinkTimer, same code) */
    /* 0x864 */ Pm1Action mAction; /* field_0x6F4 */
    /* 0x86C */ dNpc_EventCut_c mEventCut;
    /* 0x8D8 */ csXyz mHomeAngle;  /* was field_0x76C: _execute 022D1F70: current.angle on the first frame (mbHomeSet); wait01 022D269C turns back to its y (after our decomp's daNpc_Pf1_c mHomeAngle, same code) */
    /* 0x8DE */ u8 _8DE[2];
    /* 0x8E0 */ cXyz mHomePos;  /* was field_0x774: _execute 022D1F70: current.pos on the first frame (mbHomeSet) (after our decomp's daNpc_Pf1_c mHomePos, same code) */
    /* 0x8EC */ cXyz mAttPos;
    /* 0x8F8 */ cXyz mEyePos;
    /* 0x904 */ cXyz mLookPos;  /* was field_0x798: createInit 022D174C: current.pos; lookBack 022D1CF4: the look-at target in LOOK_POSITION (after our decomp's daNpc_Pf1_c mLookPos) */
    /* 0x910 */ be<f32> mPrevMorfFrame;  /* was field_0x7A4: setMtx 022D15F0: the morf frame of the last frame; a smaller frame now means the animation looped; setAnm_anm 022D2324 0 (after our decomp's daNpc_Pf1_c mPrevMorfFrame, same code) */
    /* 0x914 */ u8 _914[4];
    /* 0x918 */ be<f32> field_0x7AC;
    /* 0x91C */ be<s16> mTurnSpeed;  /* was field_0x7B0: lookBack 022D1CF4: eased to HIO mTurnSpeedTarget while m_jnt turns, else 0, and passed as the look-at's maxVel (after our decomp's daNpcMt_c mTurnSpeed, the same lookAtTarget argument) */
    /* 0x91E */ be<s16> mLookAngleY;  /* was field_0x7B2: wait01 022D269C: mHomeAngle.y with LOOK_ANGLE; lookBack 022D1CF4: the target angle in LOOK_ANGLE (after our decomp's daNpc_Pf1_c mLookAngleY) */
    /* 0x920 */ be<s8> mbMorfAnimStopped;  /* was field_0x7B4: setMtx 022D15F0: mpMorf->play()'s result, or 1 when the frame went back; setAnm_anm 022D2324 0 (after our decomp's daNpc_Pf1_c mbMorfAnimStopped, same code) */
    /* 0x921 */ be<s8> field_0x7B5;
    /* 0x922 */ be<u8> field_0x7B6;
    /* 0x923 */ be<u8> mItemNo;  /* was field_0x7B7: wait01 022D269C: 0xFF, or dComIfGp_event_getPreItemNo() on a talk with an item; talk01 022D275C 0xFF (after our decomp's daNpc_Pf1_c mItemNo, same code) */
    /* 0x924 */ be<u8> mbHomeSet;  /* was field_0x7B8: _execute 022D1F70: 0 stores mHomePos / mHomeAngle once, then 1 (after our decomp's daNpc_Pf1_c mbHomeSet, same code) */
    /* 0x925 */ u8 _925[3];
    /* 0x928 */ be<s32> mbInitEyePos;  /* was field_0x7BC: createInit 022D174C 1; setAttention 022D1590: copies mEyePos to eyePos while it or mbSetEyePos is set, then clears it (one eye position right after create) */
    /* 0x92C */ be<s32> mbSetEyePos;  /* was field_0x7C0: wait_action1 022D27DC: the result of talk01 / wait01 (0 for other states); setAttention 022D1590 copies mEyePos to eyePos while set (after our decomp's daNpc_Pf1_c mbSetEyePos, same code) */
    /* 0x930 */ be<u8> mbAttention;  /* was field_0x7C4: wait_action1 022D27DC: chkAttention(); wait01 022D269C looks at the player while set, else turns home (after our decomp's daNpc_Pf1_c mbAttention, same code) */
    /* 0x931 */ be<u8> mbTalk;  /* was field_0x7C5: checkOrder 022D1A5C: 1 on the talk command while mEvtState is 1/2; wait01 022D269C enters STATE_TALK; talk01 022D275C clears it when the message closes (after our decomp's daNpc_Pf1_c mbTalk) */
    /* 0x932 */ be<u8> mHeadOnlyFollow;
    /* 0x933 */ be<u8> mbDemo;  /* was field_0x7C7: demo 022D1A9C: 1 while demoActorID is set (returned), else 0; setMtx 022D15F0 skips animation and ground while set (after our decomp's daNpc_Pf1_c mbDemo, same code) */
    /* 0x934 */ be<u8> field_0x7C8;
    /* 0x935 */ be<s8> mActionIndex;  /* was field_0x7C9: privateCut 022D1C04: dComIfGp_evmng_getMyActIdx(staff, cut_name_tbl, ..); -1 ends the cut, 0 runs event_action (after our decomp's daNpc_Pf1_c mActionIndex, same code) */
    /* 0x936 */ be<s8> field_0x7CA;
    /* 0x937 */ be<u8> mAnmAtr;  /* was field_0x7CB: chngAnmAtr 022D24B0: the message animation attribute; setAnm_ATR 022D244C plays a_atr_prm_tbl[it]; setStt 022D2594 0xFF (after our decomp's daNpc_Pf1_c mAnmAtr) */
    /* 0x938 */ be<u8> mMesgAnimeTag;  /* was field_0x7CC: anmAtr 022D24D4: the last dComIfGp_getMesgAnimeTagInfo() taken (after our decomp's daNpc_Pf1_c mMesgAnimeTag, same code) */
    /* 0x939 */ be<s8> mBtpNum;  /* was field_0x7CD: setAnm_tex 022D2300: the btp (texture pattern) number; initTexPatternAnm 022D1060 / playTexPatternAnm 022D14A4 use it; CreateHeap 022D10A4 a_tex_pattern_num_tbl[0] (after our decomp's daNpc_Pf1_c mBtpNum) */
    /* 0x93A */ be<s8> mBckNum;  /* was field_0x7CE: setAnm_anm 022D2324: the bck number of the animation set (after our decomp's daNpc_Pf1_c mBckNum, same code) */
    /* 0x93B */ be<s8> mEvtState;  /* was field_0x7CF: wait01 022D269C 2; checkOrder 022D1A5C starts the talk at 1/2 and clears it; eventOrder 022D1F38 orders the speak event at 1, can-talk at 1/2 (after our decomp's daNpc_Pf1_c mEvtState) */
    /* 0x93C */ be<s8> mStt;  /* was field_0x7D0: setStt 022D2594: the state; wait_action1 022D27DC runs talk01 / wait01 by it; setAnm 022D23E0 plays a_anm_prm_tbl[it] (after our decomp's daNpc_Pf1_c mStt, same code) */
    /* 0x93D */ be<s8> mPrevStt;  /* was field_0x7D1: setStt 022D2594: the state before; talk01 022D275C goes back to it (after our decomp's daNpc_Pf1_c mPrevStt) */
    /* 0x93E */ be<s8> mLookMode;  /* was field_0x7D2: lookBack 022D1CF4: LOOK_PLAYER / LOOK_POSITION / LOOK_ANGLE; wait01 022D269C / setStt 022D2594 set it (after our decomp's daNpc_Pf1_c mLookMode) */
    /* 0x93F */ be<s8> field_0x7D3;
    /* 0x940 */ be<s8> field_0x7D4;
    /* 0x941 */ be<s8> mActState;  /* was field_0x7D5: set_action 022D1378: -1 when the old action ends, 0 for the new one; wait_action1 022D27DC sets up at 0 and steps to 1 (after our decomp's daNpc_Pf1_c mActState, same code; pf1 ends with 9) */
    /* 0x942 */ be<s8> mAtrSet;  /* was field_0x7D6: anmAtr 022D24D4: 0 takes the message's animation attribute once and steps to 1, MSG_DISPLAYED resets it (after our decomp's daNpc_Pf1_c mAtrSet, same code) */
    /* 0x943 */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Pm1_c, m_head_tex_pattern, 0x7E8);
WWHD_OFFSET(daNpc_Pm1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Pm1_c, mEventCut, 0x86C);
WWHD_OFFSET(daNpc_Pm1_c, mEyePos, 0x8F8);
WWHD_OFFSET(daNpc_Pm1_c, mbInitEyePos, 0x928);
WWHD_OFFSET(daNpc_Pm1_c, mBtpNum, 0x939);
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
        /* 0x00 */ be<s16> mMaxHeadX;  /* was field_0: _execute 022D1F70 passes it to m_jnt.setParam as max_head_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x02 */ be<s16> mMaxHeadY;  /* was field_2: _execute 022D1F70 passes it to m_jnt.setParam as max_head_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x04 */ be<s16> mMinHeadX;  /* was field_4: _execute 022D1F70 passes it to m_jnt.setParam as min_head_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x06 */ be<s16> mMinHeadY;  /* was field_6: _execute 022D1F70 passes it to m_jnt.setParam as min_head_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x08 */ be<s16> mMaxBackboneX;  /* was field_8: _execute 022D1F70 passes it to m_jnt.setParam as max_backbone_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x0A */ be<s16> mMaxBackboneY;  /* was field_A: _execute 022D1F70 passes it to m_jnt.setParam as max_backbone_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x0C */ be<s16> mMinBackboneX;  /* was field_C: _execute 022D1F70 passes it to m_jnt.setParam as min_backbone_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x0E */ be<s16> mMinBackboneY;  /* was field_E: _execute 022D1F70 passes it to m_jnt.setParam as min_backbone_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x10 */ be<s16> mMaxTurnStep;  /* was field_10: _execute 022D1F70 passes it to m_jnt.setParam as max_turn_step (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) */
        /* 0x12 */ be<s16> mTurnSpeedTarget;  /* was field_12: lookBack 022D1CF4: mTurnSpeed eases to it while m_jnt turns (after our decomp's daNpcMt_c mTurnSpeedTarget, the same role) */
        /* 0x14 */ be<f32> mAttentionArrowYOffset;
        /* 0x18 */ be<u8> field_18; /* HD: a byte flag (debug colours in _draw) */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> mRefCount;  /* was field_0x8: daNpc_Pm1_HIO_c_ct 022D28A0 -1; _create 022D1858 creates the HIO child at < 0 and adds 1; _delete 022D19E0 subtracts 1 and deletes the child below 0 (after our decomp's daTag_Ba1_HIO_c mRefCount, same code) */
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
