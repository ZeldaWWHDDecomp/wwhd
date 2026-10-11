/* daNpc_Bm1_c (generic Rito on Dragon Roost), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the WWHD code of d_a_npc_bm1 (_create 021FDF08):
 * - base fopNpc_npc_c 0x7DC (d/d_npc.h): mPhs .. mpStickModel +0x118 (mArcName 0x7E4);
 * - mShadowID (GameCube 0x6E8) is gone (HD shadows): mpHeadMorf .. m_hed_tex_pttrn +0x114;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (constructor 025E7820 at 0x808): mBlinkFrame .. the
 *   action function +0x174;
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: mLeftArmMtx .. the
 *   emitters +0x170 (mEventCut 0x90C, constructor 0259F740; m87F 0x9EF);
 * - an HD-only byte at 0xA08 (after mbInDemo 0xA07; 4-byte slot): mWingLPos .. m905 +0x174
 *   (mWingLPos 0xA0C, emitters 0xA48.., mType 0xA75, mSpecificType 0xA76).
 * Size 0xA7C (to be confirmed by the profile). The HD vtable at 0xB4 is 0x10017A18 (_create).
 * Layout entries marked [g] are derived, not yet read by a verified function. */
#pragma once
#include "d/actor/d_a_npc_ba1.h"
using Bm1ActionFunc = ProcFunc_l; /* Bm1ActionFunc (GHS pointer to member, 8 bytes) */
#include "d/d_npc.h"

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct dNpc_PathRun_bm1 {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_bm1, 8);

struct daNpc_Bm1_c : fopNpc_npc_c {
    enum {
        SPECIFIC_TYPE_Quill_0_e,
        SPECIFIC_TYPE_Quill_1_e,
        SPECIFIC_TYPE_Quill_2_e,
        SPECIFIC_TYPE_Quill_3_e,
        SPECIFIC_TYPE_Quill_4_e,
        SPECIFIC_TYPE_Akoot_e,
        SPECIFIC_TYPE_Skett_e,
        SPECIFIC_TYPE_Basht_e,
        SPECIFIC_TYPE_Bisht_e,
        SPECIFIC_TYPE_Hoskit_e,
        SPECIFIC_TYPE_Ilari_0xA_e,
        SPECIFIC_TYPE_Ilari_0xB_e,
        SPECIFIC_TYPE_Ilari_0xC_e,
        SPECIFIC_TYPE_Pashli_e,
        SPECIFIC_TYPE_Namali_e,
        SPECIFIC_TYPE_Kogoli_e,
        SPECIFIC_TYPE_Invalid_e = 0xFF
    };
    enum {
        TYPE_Uninitialized_e,
        TYPE_Quill_e,
        TYPE_Akoot_e,
        TYPE_Skett_e,
        TYPE_Basht_e,
        TYPE_Bisht_e,
        TYPE_Hoskit_e,
        TYPE_Ilari_e,
        TYPE_Pashli_e,
        TYPE_Namali_e,
        TYPE_Kogoli_e,
        TYPE_Invalid_e = 0xFF,
    };

    struct anm_prm_c {
        /* 0x00 */ be<s8> anmNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
        /* 0x10 */ be<s32> hasArms;
    };

    /* ---- methods: the declarations follow the GameCube header (int* -> be<s32>*, f32* ->
     * be<f32>*, u32* -> be<u32>*); a part adjusts the return types of its own functions to what
     * the WWHD code returns ---- */
    void nodeWngControl(J3DNode*, J3DModel*);
    void nodeArmControl(J3DNode*, J3DModel*);
    void nodeBm1Control(J3DNode*, J3DModel*);
    bool chk_appCnd();
    bool init_PST_0();
    bool init_PST_1();
    bool init_PST_2();
    bool init_PST_3();
    bool init_PST_4();
    bool init_BMB_0();
    u32 init_BMB_1(); /* tail call: result register passed through */
    bool init_BMB_2();
    bool init_BMC_0();
    bool init_BMC_1();
    bool init_BMC_2();
    bool init_BMC_3();
    u32 init_BMD_0(); /* tail call */
    bool init_BMD_1();
    bool init_SKT_0();
    u32 init_KKT_0(); /* tail call */
    bool createInit();
    void setMtx(u32); /* bool */
    u32 anmNum_toResID(int);
    u32 headAnmNum_toResID(int);
    u32 wingAnmNum_toResID(int);
    u32 btpNum_toResID(int);
    bool setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    void plyTexPttrnAnm();
    u32 setAnm_tex(s8);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    bool setAnm();
    void setPlaySpd(f32);
    void chg_anmTag();
    void control_anmTag();
    void chg_anmAtr(u8);
    void control_anmAtr();
    void setAnm_ATR(int);
    void anmAtr(u16);
    void eventOrder();
    void checkOrder();
    u8 chk_manzai();
    bool chk_talk();
    u8 chk_partsNotMove(); /* bool (talk_1 returns it as is) */
    void lookBack();
    u16 next_msgStatus(be<u32>*);
    s32 getBitMask(); /* s8, returned sign-extended; callers use the register as is */
    u32 getMsg_PST_1();
    u32 getMsg_PST_3();
    u32 getMsg_SKT_0();
    u32 getMsg_KKT_0();
    u32 getMsg_BMB_0();
    u32 getMsg_BMB_1();
    u32 getMsg_BMB_2();
    u32 getMsg_BMC_0();
    u32 getMsg_BMC_2();
    u32 getMsg_BMC_3();
    u32 getMsg_BMD_0();
    u32 getMsg_BMD_1();
    u32 getMsg();
    u8 chkAttention();      /* bool (stored as is) */
    void setAttention(u32); /* bool */
    fopAc_ac_c* searchByID(fpc_ProcID);
    bool partner_srch_sub(u32 /* fpcLyIt_JudgeFunc */);
    void partner_srch();
    u32 bm_movPass(u32); /* bool */
    void bm_setFlyAnm();
    void bm_clcFlySpd();
    void bm_clcMovSpd();
    bool bm_flyMove();
    void bm_nMove();
    void setPrtcl_Flyaway();
    void delPrtcl_Flyaway();
    void setPrtcl_Land0();
    void delPrtcl_Land0();
    void setPrtcl_Hane0();
    void flwPrtcl_Hane0();
    void delPrtcl_Hane0();
    void setPrtcl_Hane1();
    void flwPrtcl_Hane1();
    void delPrtcl_Hane1();
    bool decideType(int, int);
    void eInit_setLocFlag(be<s32>*);
    void eInit_setShapeAngleY(be<s32>*, s16);
    void eInit_setEvTimer(be<s32>*);
    cXyz* eInit_calcRelativPos(cXyz* /* hidden result (HD: allocated when NULL) */, cXyz*,
                               be<s32>*);
    void eInit_ATTENTION_(be<s32>*, be<s32>*, be<s32>*, cXyz*, be<s32>*, be<s32>*, be<s32>*);
    void eInit_SET_PLYER_GOL_(be<s32>*, cXyz*, be<s32>*);
    f32 eInit_prmFloat(be<f32>*, f32);
    void eInit_FLY_(be<s32>*, be<f32>*, be<f32>*, be<f32>*, be<f32>*);
    u32 eInit_DEL_ACTOR_();
    void eInit_WLK_(be<s32>*, be<f32>*, be<f32>*, cXyz*, be<s32>*, be<s32>*, be<s32>*);
    void eInit_INI_EVN_1_();
    void eInit_SET_NXT_PTH_INF_();
    void eInit_SET_ANM_(be<s32>*);
    void eInit_MOV_PTH_POINT_(be<s32>*, be<s32>*, be<s32>*, be<s32>*);
    void event_actionInit(int);
    u32 eMove_ATTENTION_(); /* bool; returns m_jnt.mbTrn ^ 1 as is */
    u8 eMove_KMA_FLY_();    /* bool; returns m88A as is */
    bool eMove_FLY_();
    bool eMove_WLK_();
    u32 event_action(); /* bool; returns the eMove_* result register as is */
    void cut_init_360_TRN(int);
    bool cut_move_360_TRN();
    void privateCut(int);
    void endEvent();
    BOOL isEventEntry();
    void event_proc(int);
    BOOL set_action(Bm1ActionFunc*, void*);
    void setStt(s8);
    BOOL d_wait();
    BOOL lookup();
    BOOL orooro();
    BOOL wait_1();
    BOOL talk_1();
    BOOL talk_2();
    BOOL manzai();
    BOOL wait_4();
    BOOL flyawy();
    BOOL wait_5();
    BOOL h_wait();
    BOOL wait_7();
    BOOL wait_3();
    BOOL wait_8();
    BOOL wait_2();
    BOOL walk_1();
    BOOL CHKwai();
    BOOL demo_action1(void*);
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    BOOL wait_action3(void*);
    BOOL wait_action4(void*);
    BOOL wait_action5(void*);
    BOOL wait_action6(void*);
    BOOL wait_action7(void*);
    BOOL wait_action8(void*);
    BOOL wait_action9(void*);
    BOOL wait_actionA(void*);
    u8 demo(); /* bool; returns the mbInDemo byte as is */
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    J3DModelData* create_Anm();
    J3DModelData* create_hed_Anm();
    J3DModelData* create_wng_Anm();
    J3DModelData* create_arm_Anm();
    bool create_itm_Mdl();
    BOOL CreateHeap();

    /* ---- layout ---- */
    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ char mArcName[4];
    /* 0x7E8 */ be<s8> m_hed_jnt_num;
    /* 0x7E9 */ be<s8> m_bbone_jnt_num;
    /* 0x7EA */ be<s8> m_nec_jnt_num;
    /* 0x7EB */ be<s8> m_arm_L_jnt_num;
    /* 0x7EC */ be<s8> m_arm_R_jnt_num;
    /* 0x7ED */ u8 _7ED[3];
    /* 0x7F0 */ gptr<J3DModel> mpBinderModel;
    /* 0x7F4 */ gptr<J3DModel> mpBagModel;
    /* 0x7F8 */ gptr<J3DModel> mpKnifeModel;
    /* 0x7FC */ gptr<J3DModel> mpStickModel;
    /* 0x800 */ gptr<mDoExt_McaMorf> mpHeadMorf; /* HD: no mShadowID before it */
    /* 0x804 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x808 */ u8 mHeadBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x87C */ be<u8> mBlinkFrame;
    /* 0x87D */ u8 _87D;
    /* 0x87E */ be<s16> mBlinkTimer;
    /* 0x880 */ be<s16> pad70C;
    /* 0x882 */ u8 _882[2];
    /* 0x884 */ gptr<mDoExt_McaMorf> mpWingMorf;
    /* 0x888 */ be<s8> m_wngL1_jnt_num;
    /* 0x889 */ be<s8> m_wngR1_jnt_num;
    /* 0x88A */ be<s8> m_wngL3_jnt_num;
    /* 0x88B */ be<s8> m_wngR3_jnt_num;
    /* 0x88C */ u8 pad718[4];
    /* 0x890 */ gptr<mDoExt_McaMorf> mpArmMorf;
    /* 0x894 */ be<s8> m_armL1_jnt_num;
    /* 0x895 */ be<s8> m_armR1_jnt_num;
    /* 0x896 */ be<s8> m_armL2_jnt_num;
    /* 0x897 */ be<s8> m_armR2_jnt_num;
    /* 0x898 */ be<s8> m_hnd_R_jnt_num;
    /* 0x899 */ u8 _899[3];
    /* 0x89C */ Bm1ActionFunc mCurrActionFunc; /* HD: 8 bytes (GameCube 12) */
    /* 0x8A4 */ Mtx34 mLeftArmMtx;
    /* 0x8D4 */ Mtx34 mRightArmMtx;
    /* 0x904 */ dNpc_PathRun_bm1 mPathRun;
    /* 0x90C */ dNpc_EventCut_c mEventCut; /* hides fopNpc_npc_c::mEventCut (as on GameCube) */
    /* 0x978 */ be<u32> mPartnerProcID;
    /* 0x97C */ cXyz mInitialPos;
    /* 0x988 */ csXyz m818;
    /* 0x98E */ u8 _98E[2];
    /* 0x990 */ cXyz mEyePos;
    /* 0x99C */ cXyz m82C;
    /* 0x9A8 */ cXyz mTargetPos;
    /* 0x9B4 */ be<f32> mFrame;
    /* 0x9B8 */ be<f32> mTargetFlySpeed;
    /* 0x9BC */ be<f32> mFlySpeedY;
    /* 0x9C0 */ be<f32> mTargetFlyStep;
    /* 0x9C4 */ be<f32> mFlyAccelY;
    /* 0x9C8 */ be<f32> m858;
    /* 0x9CC */ be<s16> m85C;
    /* 0x9CE */ be<s16> m85E;
    /* 0x9D0 */ be<s16> m860;
    /* 0x9D2 */ be<s16> mEventIdTable[4];
    /* 0x9DA */ be<s16> mEventIdx;
    /* 0x9DC */ be<s16> m86C;
    /* 0x9DE */ be<s16> m86E;
    /* 0x9E0 */ be<s16> m870;
    /* 0x9E2 */ be<s16> m872;
    /* 0x9E4 */ be<s16> mHeadLookAtMaxVel;
    /* 0x9E6 */ be<s16> m876;
    /* 0x9E8 */ be<u16> mOldMsgStat;
    /* 0x9EA */ be<s8> mbMorfAnimStopped;
    /* 0x9EB */ be<s8> m87B;
    /* 0x9EC */ be<s8> m87C;
    /* 0x9ED */ be<u8> m87D;
    /* 0x9EE */ be<u8> m87E;
    /* 0x9EF */ be<u8> m87F;
    /* 0x9F0 */ be<u8> m880;
    /* 0x9F1 */ be<u8> m881;
    /* 0x9F2 */ be<u8> m882;
    /* 0x9F3 */ be<u8> mbManzai;
    /* 0x9F4 */ be<u8> mbInitPostman0;
    /* 0x9F5 */ be<u8> mbHasArms;
    /* 0x9F6 */ be<u8> mbSetShapeAngle;
    /* 0x9F7 */ be<u8> m887;
    /* 0x9F8 */ be<u8> m888;
    /* 0x9F9 */ be<u8> m889;
    /* 0x9FA */ be<u8> m88A;
    /* 0x9FB */ be<u8> m88B;
    /* 0x9FC */ be<u8> mbRanExecute;
    /* 0x9FD */ be<u8> m88D;
    /* 0x9FE */ be<u8> m88E;
    /* 0x9FF */ be<u8> m88F;
    /* 0xA00 */ be<s32> mbSetEyePos;
    /* 0xA04 */ be<u8> mbAttention;
    /* 0xA05 */ be<u8> m895;
    /* 0xA06 */ be<u8> m896;
    /* 0xA07 */ be<u8> mbInDemo;
    /* 0xA08 */ union {
        be<u8> mbKeepOldPosition;
        be<u8> mHD_A08;
    }; // HD: compatibility name for the separately claimed movement part. /* HD-only byte */
    /* 0xA09 */ u8 _A09[3];
    /* 0xA0C */ cXyz mWingLPos;
    /* 0xA18 */ cXyz mWingRPos;
    /* 0xA24 */ cXyz mArmLPos;
    /* 0xA30 */ cXyz mArmRPos;
    /* 0xA3C */ be<u8> m8C8;
    /* 0xA3D */ u8 m8C9[3];
    /* 0xA40 */ be<u8> m8CC;
    /* 0xA41 */ u8 m8CD[7];
    /* 0xA48 */ gptr<JPABaseEmitter> mpFlyawayEmitterL;
    /* 0xA4C */ gptr<JPABaseEmitter> mpFlyawayEmitterR;
    /* 0xA50 */ gptr<JPABaseEmitter> mpLandEmitterL;
    /* 0xA54 */ gptr<JPABaseEmitter> mpLandEmitterR;
    /* 0xA58 */ gptr<JPABaseEmitter> mpFeatherEmitterL;
    /* 0xA5C */ gptr<JPABaseEmitter> mpFeatherEmitterR;
    /* 0xA60 */ gptr<JPABaseEmitter> mpFeather1EmitterL;
    /* 0xA64 */ gptr<JPABaseEmitter> mpFeather1EmitterR;
    /* 0xA68 */ be<s8> m8F4;
    /* 0xA69 */ be<s8> mActionIndex;
    /* 0xA6A */ be<s8> mActNo;
    /* 0xA6B */ be<u8> m8F7;
    /* 0xA6C */ be<u8> m8F8;
    /* 0xA6D */ be<s8> mBtpNum;
    /* 0xA6E */ be<s8> mAnmNum;
    /* 0xA6F */ u8 m8FB[2];
    /* 0xA71 */ be<s8> m8FD;
    /* 0xA72 */ be<s8> mStatus;
    /* 0xA73 */ be<s8> m8FF;
    /* 0xA74 */ be<s8> mLookBackState;
    /* 0xA75 */ be<s8> mType;
    /* 0xA76 */ be<s8> mSpecificType;
    /* 0xA77 */ be<s8> mSpawnCondition;
    /* 0xA78 */ be<s8> m904;
    /* 0xA79 */ be<s8> m905;
    /* 0xA7A */ u8 _A7A[2];
};
WWHD_OFFSET(daNpc_Bm1_c, mArcName, 0x7E4);
WWHD_OFFSET(daNpc_Bm1_c, mHeadBtpAnm, 0x808);
WWHD_OFFSET(daNpc_Bm1_c, mBlinkFrame, 0x87C);
WWHD_OFFSET(daNpc_Bm1_c, mCurrActionFunc, 0x89C);
WWHD_OFFSET(daNpc_Bm1_c, mEventCut, 0x90C);
WWHD_OFFSET(daNpc_Bm1_c, m87F, 0x9EF);
WWHD_OFFSET(daNpc_Bm1_c, mWingLPos, 0xA0C);
WWHD_OFFSET(daNpc_Bm1_c, mpFeather1EmitterR, 0xA64);
WWHD_OFFSET(daNpc_Bm1_c, mType, 0xA75);
WWHD_OFFSET(daNpc_Bm1_c, mSpecificType, 0xA76);
WWHD_SIZE(daNpc_Bm1_c, 0xA7C);

// HD: event integers are narrowed through their low guest-endian byte or halfword.
union Bm1EventParam {
    be<s32> mValue;
    struct {
        u8 mPadding[2];
        be<s16> mHalf;
    };
    struct {
        u8 mBytePadding[3];
        be<u8> mByte;
    };
};
static inline Bm1EventParam* Bm1_eventParam(be<s32>* value) {
    return reinterpret_cast<Bm1EventParam*>(value);
}

s8 JUTNameTab_getIndex(u32 tab, const char* name);

s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr,
                       f32 rate, s32 start, s32 end, u32 modify, s32 entry);

char* strcpy_g(char* d, const char* s);

s16 cLib_calcTimer_g(be<s16>* t);

s8 dBgS_GetRoomId(dBgS* bgs, void* poly);

u8 dBgS_GetPolyColor(dBgS* bgs, void* poly);

void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z);

void dNpc_PathRun_setInf(dNpc_PathRun_bm1* p, u32 idx, s8 room, u8 fwd);

dPath* dNpc_PathRun_nextPath(dNpc_PathRun_bm1* p, s8 room);

void dNpc_PathRun_setInfDrct(dNpc_PathRun_bm1* p, dPath* path);

BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n,
                       void* ids, u32 p6, s8 p7);

BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, u32 name, s32 snd,
                         const char* arc);

u32 Bm1_pathMaxPoint(dNpc_PathRun_bm1* p);

void Bm1_pathGetPoint(dNpc_PathRun_bm1* p, cXyz* out, u8 idx);

void Bm1_pathNextIdxAuto(dNpc_PathRun_bm1* p);

u32 Bm1_setBtpResult(daNpc_Bm1_c* npc, u32 modify, s32 btp);

u32 Bm1_initSkettResult(daNpc_Bm1_c* npc);

u32 Bm1_initBashtResult(daNpc_Bm1_c* npc);

u32 Bm1_initPashliResult(daNpc_Bm1_c* npc);

u32 Bm1_moveFlyResult(daNpc_Bm1_c* npc);

u32 Bm1_moveKmaFlyResult(daNpc_Bm1_c* npc);

u32 Bm1_moveAttentionResult(daNpc_Bm1_c* npc);

u32 Bm1_moveWalkResult(daNpc_Bm1_c* npc);

struct Bm1JointNumberView {
    u8 mPadding[4];
    be<u16> mJointNo;
};
static inline u16 Bm1_jointNumber(J3DJoint* joint) {
    return reinterpret_cast<Bm1JointNumberView*>(joint)->mJointNo;
}

// HD: private views retain the exact guest memory widths and virtual slots.
template <class T> static inline T* Bm1_view(u32 address) { return gabi::at<T>(address); }
struct Bm1NameHeader {
    u8 mPadding[4];
    be<u32> mNameOffset;
};
struct Bm1EmitterView {
    u8 mPadding[0x5C];
    be<s32> mMaxFrame;
    u8 mBody[0x254 - 0x60];
    be<u32> mStatus;
};
struct Bm1JointCallback {
    u8 mPadding[8];
    be<u32> mCallback;
    u8 mBody[0x1C - 12];
};
struct Bm1JointTable {
    u8 mPadding[4];
    be<u32> mCount;
    gptr<Bm1JointCallback> mJoints;
};

static inline void McaMorf_delete(mDoExt_McaMorf* morf) {
    if (morf != nullptr) {
        u32 vt = gabi::load<u32>(gabi::ea(morf));
        gabi::call_ptr(gabi::load<u32>(vt + 0xC), morf, 3);
    }
}

static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}

static inline void pmf_load(Bm1ActionFunc* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}

static inline void Bm1_callAction(void* self, Bm1ActionFunc* pmf, void* arg) {
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

u32 Bm1_modelNameHeader(J3DModelData* data);

s8 JUTNameTab_getIndex(u32 tab, const char* name);

void Bm1_setEmitterMatrix(Mtx34* matrix, u32 rotation, u32 translation);

void Bm1_constructNpc(daNpc_Bm1_c* npc);

void Bm1_constructBtp(void* animation);

void Bm1_constructEventCut(dNpc_EventCut_c* cut);

void Bm1_searchById(fpc_ProcID id, gptr<fopAc_ac_c>* result);

void* Bm1_demoGetActor(u32 object, u32 id);

J3DAnmTexPattern* Bm1_demoGetBtp(void* actor, const char* archive);

s8 Bm1_getMyActIndex(void* manager, s32 staff, u32 table, s32 count, s32 force, s32 extra);

void Bm1_cutEnd(void* manager, s32 staff);

BOOL Bm1_isAdvance(void* manager, s32 staff);

static inline void* Bm1_resource(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::guest_addr(arc);
    a->__vtbl = 0x10016F80;
    n->mStringTop = gabi::guest_addr(name);
    n->__vtbl = 0x10016F80;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}

struct Bm1ActorResult {
    gptr<fopAc_ac_c> actor;
};
WWHD_SIZE(Bm1ActorResult, 4);

struct Bm1SetupHioParam {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMinHeadX;
    /* 0x06 */ be<s16> mMinHeadY;
    /* 0x08 */ be<s16> mMaxBackboneX;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinBackboneX;
    /* 0x0E */ be<s16> mMinBackboneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ be<s16> mCalcAngleTarget;
    /* 0x14 */ be<f32> mAttPosOffsetY;
    /* 0x18 */ be<u8> m18;
    /* 0x19 */ u8 _19;
    /* 0x1A */ be<s16> mFlyScale;
    /* 0x1C */ be<s16> mFlyMaxStep;
    /* 0x1E */ u8 _1E[2];
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<s16> m34;
    /* 0x36 */ be<s16> m36;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<f32> m44;
    /* 0x48 */ be<f32> m48;
};
WWHD_SIZE(Bm1SetupHioParam, 0x4C);
struct Bm1SetupChildHio {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ Bm1SetupHioParam hio_prm;
    /* 0x50 */ be<u32> m50;
};
WWHD_SIZE(Bm1SetupChildHio, 0x54);
struct Bm1SetupHio {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<s8> m4;
    /* 0x005 */ u8 _5[3];
    /* 0x008 */ be<s32> m8;
    /* 0x00C */ Bm1SetupChildHio children[10];
};
WWHD_SIZE(Bm1SetupHio, 0x354);

struct Bm1EmitterMatrix {
    u8 mPadding[0x1F0];
    Mtx34 mGlobalRotation;
    u8 mReserved[0x22C - 0x220];
    cXyz mGlobalTranslation;
};
WWHD_OFFSET(Bm1EmitterMatrix, mGlobalTranslation, 0x22C);

WWHD_SIZE(Bm1EventParam, 4);
WWHD_OFFSET(Bm1EventParam, mByte, 3);
WWHD_OFFSET(Bm1EventParam, mHalf, 2);
WWHD_SIZE(Bm1NameHeader, 8);
WWHD_OFFSET(Bm1NameHeader, mNameOffset, 4);
WWHD_SIZE(Bm1EmitterView, 0x258);
WWHD_OFFSET(Bm1EmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(Bm1EmitterView, mStatus, 0x254);
WWHD_SIZE(Bm1JointCallback, 0x1C);
WWHD_OFFSET(Bm1JointCallback, mCallback, 8);
WWHD_SIZE(Bm1JointTable, 0xC);
WWHD_OFFSET(Bm1JointTable, mCount, 4);
WWHD_OFFSET(Bm1JointTable, mJoints, 8);
WWHD_SIZE(Bm1JointNumberView, 6);
WWHD_OFFSET(Bm1JointNumberView, mJointNo, 4);
WWHD_SIZE(Bm1EmitterMatrix, 0x238);
WWHD_OFFSET(Bm1EmitterMatrix, mGlobalRotation, 0x1F0);
