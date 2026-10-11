/* Rose (Outset): HD actor size0x97C, NPC base0x7DC. The GameCube header contains no
 * actor fields; descriptive names reflect their verified uses. Local views retain asserted
 * HD offsets. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"

// HD: private views of virtual slots and the mutable one-based talk partner list.
struct Ob1VirtualTable {
    be<u32> mWords[10];
};
struct Ob1VirtualObject {
    gptr<Ob1VirtualTable> mVtable;
};
struct Ob1AnmObject {
    u8 _00[4];
    gptr<Ob1VirtualTable> mVtable;
};
struct Ob1PtmfVirtual {
    u8 _00[6];
    be<s16> mVtableOffset;
};
struct Ob1TalkList {
    be<u32> mUnused;
    be<s32> mCount;
    gptr<fopAc_ac_c> mActors[1];
};
struct Ob1ModelEffects {
    u8 _00[0x38];
    be<u32> mTexPattern;
};
WWHD_SIZE(Ob1VirtualObject, 4);
WWHD_OFFSET(Ob1AnmObject, mVtable, 4);
WWHD_OFFSET(Ob1PtmfVirtual, mVtableOffset, 6);
WWHD_OFFSET(Ob1TalkList, mActors, 8);
WWHD_OFFSET(Ob1ModelEffects, mTexPattern, 0x38);

template <class T> inline T* Ob1_view(u32 address) { return gabi::at<T>(address); }
enum Ob1Resource { Ob1WaitBck = 3, Ob1BodyBdl = 5 };

/* GHS pointer to member function: {s16 this delta, s16 vtable index (0: NULL, < 0: not virtual),
 * u32 function (or, low half, the vtable offset)} */
struct Ob1ActionFunc {
    /* 0x0 */ be<s16> delta;
    /* 0x2 */ be<s16> idx;
    /* 0x4 */ be<u32> fn;
};
WWHD_SIZE(Ob1ActionFunc, 8);

/* ---- local bindings used by d_a_npc_ob1 and d_a_npc_yw1 ---- */

/* 027F3F94 (matcher: __nw) / 027F68FC: self-relative pointer at *data + 0xC (two copies) */
/* J3DModelData::getJointName()->getIndex(name): name table self-relative at +0x10 of the joint block */
/* getJointNodePointer(i)->setCallBack(cb): HD nodes of 0x1C bytes from +8, index checked against +4 */
static inline void J3DModelData_setJointCallBack(u32 data, u32 i, u32 cb) {
    gabi::at<J3DModelData>(data)->getJointNodePointer((u16)i)->mCallBack = cb;
}
static inline u32 J3DModel_modelData(J3DModel* m) { return gabi::ea(m->getModelData()); }
/* J3DModel::getAnmMtx (HD: joint matrices in a block at +0x2C, marked dirty) */
/* J3DAnmTexPattern::getFrameMax: HD virtual (vtable at +4, slot +0x14) */
static inline s32 J3DAnmTexPattern_getFrameMax(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(gabi::at<Ob1AnmObject>(gabi::ea(p))->mVtable->mWords[5], p);
}
BOOL mDoExt_btpAnm_init(void* a, u32 d, J3DAnmTexPattern* btp, s32 anmPlay, s32 mode, f32 rate,
                        s16 start, s16 end, s32 modify, s32 entry);
enum { dSv_evtReg_OB1_PIGS = 0xB6FF };

s16 cLib_calcTimer(be<s16>* t);

s32 cLib_getRndValue(s32 min, s32 range);
/* the ground polygon: dBgS_GndChk's cBgS_PolyInfo (+0x14) */
static inline void* gnd_poly(dBgS_ObjAcch& a) { return static_cast<cBgS_PolyInfo*>(&a.m_gnd); }
s32 dBgS_GetRoomId(dBgS* bgs, void* poly);
s32 dBgS_GetPolyColor(dBgS* bgs, void* poly);
/* getPoint returns a cXyz: hidden result pointer after `this` */
/* dNpc_playerEyePos returns a cXyz through a hidden result pointer (d_npc.h has it as f32: wrong) */
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16, s16, bool) */
void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel,
                  u8 headOnly);
void setActorInfo2(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a);

/* dDemo: the demo object pointer (101D5FFC), getActor(u8 id), getP_BtpData(arc), setDemoData */
void* dDemo_object_getActor(u32 obj, u8 id);
J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc);
BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p,
                       u32 q, s8 r);
/* dAttention_c (play+0x5804): LockonTruth, LockonTarget(i) (matcher: ActionTarget), ActionTarget(i)
 * (matcher: LockonTarget) */
BOOL dAttention_LockonTruth(dAttention_c* a);
fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i);
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out);
/* HD messages: the manager (*101F4B5C) */
static inline u32 l_msgMng() { return gabi::ea(fopNpc_npc_c::mpCurrMsg.get()); }
u32 fopMsgM_SearchByID(u32 mng);
u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos);
/* play+0x5C30: {?, s32 n, fopAc_ac_c* list[] (1-based)}: the current talk partner */
static inline u32 dComIfGp_talkActor(u32 play) {
    Ob1TalkList* list = gabi::at<Ob1TalkList>(play + 0x5C30);
    s32 count = list->mCount;
    return gabi::ea(list->mActors[count - 1].get());
}
void Ob1_copyBytes(u32 dst, u32 src, u32 n); /* memcpy (import) */
/* GHS pointer-to-member call with one argument */
static inline s32 ptmf_call1(Ob1ActionFunc* f, void* self, void* arg) {
    s16 idx = f->idx;
    void* p = gabi::at<void>(gabi::ea(self) + (s16)f->delta);
    if (idx < 0)
        return gabi::call_ptr<s32>(f->fn, p, arg);
    s16 offset = gabi::at<Ob1PtmfVirtual>(gabi::ea(f))->mVtableOffset;
    Ob1VirtualTable* vt = *gabi::at<gptr<Ob1VirtualTable>>(gabi::ea(p) + offset);
    return gabi::call_ptr<s32>(vt->mWords[idx * 2 + 1], p, arg);
}
/* virtuals (actor vtable at +0xB4: getMsg +0x1C, anmAtr +0x24) */
static inline u32 vcall_getMsg(fopNpc_npc_c* a) {
    return gabi::call_ptr<u32>(gabi::at<Ob1VirtualTable>(a->__vtbl)->mWords[7], a);
}
static inline void vcall_anmAtr(fopNpc_npc_c* a, u16 st) {
    gabi::call_ptr(gabi::at<Ob1VirtualTable>(a->__vtbl)->mWords[9], a, st);
}

/* mDoExt_btpAnm, HD 0x74 (constructor 025E7820) */
struct mDoExt_btpAnm_ob1 {
    u8 _[0x74];
};

struct daNpc_Ob1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ be<s32> mFlags;
    };

    void nodeOb1Control(J3DNode* node, J3DModel* model);
    J3DModelData* create_Anm();
    J3DModelData* create_hed_Mdl();
    s32 btpNum_toResID(int num);
    BOOL setBtp(s32 modify, int num);
    BOOL iniTexPttrnAnm(s32 modify);
    BOOL CreateHeap();
    BOOL charDecide(int type);
    BOOL set_action(Ob1ActionFunc* action, void* arg);
    BOOL init_OB1_0();
    BOOL init_OB1_1();
    BOOL init_OB1_2();
    void plyTexPttrnAnm();
    void setAttention(s32 force);
    void setMtx(s32 force);
    BOOL createInit();
    cPhs_State _create();
    BOOL _delete();
    void partner_srch();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void endEvent();
    void event_actionInit(int staffIdx);
    BOOL event_action();
    void privateCut(int staffIdx);
    void lookBack();
    void event_proc(int staffIdx);
    void ob_clcMovSpd();
    s32 ob_movPass();
    void ob_nMove();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 anmNum_toResID(int num);
    u32 setAnm_tex(s8 tex);
    BOOL setAnm_anm(anm_prm_c* prm);
    void setAnm_NUM(int num, int withTex);
    BOOL setAnm();
    void setAnm_ATR(int withTex);
    void chg_anmAtr(u8 atr);
    void control_anmAtr();
    void anmAtr(u16 msgStatus);
    BOOL chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(u32* pMsgNo);
    u32 getMsg_OB1_0();
    u32 getMsg_OB1_1();
    u32 getMsg_OB1_2();
    u32 getMsg();
    BOOL chkAttention();
    fopAc_ac_c* searchByID(fpc_ProcID id);
    s8 bitCount(u8 bits);
    void set_pigCnt();
    void ob_setPthPos();
    void get_attPos(cXyz* out);
    void clrSpd();
    void setStt(s8 stt);
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL walk_1();
    BOOL talk_1();
    BOOL manzai();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    s16 eventIdx(s32 i) { return mEventIdx[i]; }

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ gptr<J3DModel> mpHedModel;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x7F0 */ mDoExt_btpAnm_ob1 mBtpAnm;
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ u8 _865;
    /* 0x866 */ be<s16> mBlinkTimer;
    /* 0x868 */ Ob1ActionFunc mAction;
    /* 0x870 */ dNpc_PathRun_c mPathRun;
    /* 0x878 */ dNpc_EventCut_c mEventCut2; /* the TU's own event cut ("Ob1") */
    /* 0x8E4 */ be<u32> mSearchProcId;      /* a process id (searched in setStt, result unused) */
    /* 0x8E8 */ cXyz mHomePos;
    /* 0x8F4 */ csXyz mHomeAngle;
    /* 0x8FA */ csXyz mAngle;
    /* 0x900 */ cXyz mEyePos;
    /* 0x90C */ cXyz mLookPos;
    /* 0x918 */ cXyz mTargetPos;
    /* 0x924 */ be<f32> mPrevFrame;
    /* 0x928 */ be<f32> mSpdTarget;
    /* 0x92C */ be<f32> mSpdStep;
    /* 0x930 */ be<f32> mPassDist;
    /* 0x934 */ be<s16> mSaveHeadY;
    /* 0x936 */ be<s16> mSaveBboneY;
    /* 0x938 */ be<s16> mSaveAngleY;
    /* 0x93A */ u8 _93A[2];
    /* 0x93C */ be<u32> mMsgNo;
    /* 0x940 */ be<s16> mEventIdx[1];
    /* 0x942 */ be<s16> mEvtNo;
    /* 0x944 */ u8 _944[2];
    /* 0x946 */ be<s16> mTimer;
    /* 0x948 */ u8 _948[2];
    /* 0x94A */ be<s16> mWaitTimer;
    /* 0x94C */ u8 _94C[2];
    /* 0x94E */ be<s16> mHeadTurnSpd;
    /* 0x950 */ be<s16> mTargetAngY;
    /* 0x952 */ be<u16> mMsgStatus;
    /* 0x954 */ be<s8> mAnmEnd;
    /* 0x955 */ be<s8> mLoopCnt;
    /* 0x956 */ u8 _956;
    /* 0x957 */ be<s8> mPigCntGot;  /* pigs already caught (event register bits) */
    /* 0x958 */ be<s8> mPigCntHere; /* pigs on the map */
    /* 0x959 */ be<s8> mPigCntNew;  /* pigs on the map not yet caught */
    /* 0x95A */ be<s8> mTalkStep;
    /* 0x95B */ be<u8> mPreItemNo;
    /* 0x95C */ be<u8> mbFirstPigTalk;
    /* 0x95D */ be<u8> mbPartnerMessage;
    /* 0x95E */ be<u8> mbHidden;
    /* 0x95F */ be<u8> mbKeepAngle;
    /* 0x960 */ be<u8> mbSkipMove;
    /* 0x961 */ be<u8> mbHideBody;
    /* 0x962 */ be<u8> mbPointPassed;
    /* 0x963 */ be<u8> mbInit;
    /* 0x964 */ be<s32> mActRet;
    /* 0x968 */ be<u8> mbAttention;
    /* 0x969 */ be<u8> mbTalk;
    /* 0x96A */ be<u8> mbHeadOnly;
    /* 0x96B */ be<u8> mbDemo;
    /* 0x96C */ be<s8> mMoveMode;
    /* 0x96D */ u8 _96D;
    /* 0x96E */ be<s8> mActIdx;
    /* 0x96F */ be<s8> mEvtActNo;
    /* 0x970 */ be<u8> mAnmAtr;
    /* 0x971 */ be<u8> mAnmTag;
    /* 0x972 */ be<s8> mTexNo;
    /* 0x973 */ be<s8> mAnmNo;
    /* 0x974 */ be<s8> mOrderType;
    /* 0x975 */ be<s8> mStt;
    /* 0x976 */ be<s8> mPrevStt;
    /* 0x977 */ be<s8> mLookMode;
    /* 0x978 */ be<u8> mCharacterFlags;
    /* 0x979 */ be<s8> mType;
    /* 0x97A */ be<s8> mActStep;
    /* 0x97B */ be<s8> mAtrCnt;
};
WWHD_OFFSET(daNpc_Ob1_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Ob1_c, mEventCut2, 0x878);
WWHD_OFFSET(daNpc_Ob1_c, mHomePos, 0x8E8);
WWHD_OFFSET(daNpc_Ob1_c, mMsgNo, 0x93C);
WWHD_OFFSET(daNpc_Ob1_c, mActRet, 0x964);
WWHD_SIZE(daNpc_Ob1_c, 0x97C);

/* l_HIO (0x10467D50, 0x3C): vtable, s8 mNo, s32 mEntryID, hio_prm_c (0x30, copied from
 * a_prm_tbl 101C27D0) */
struct daNpc_Ob1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mEntryID;
    /* 0x0C */ be<s16> mPrm
        [9]; /* jnt setParam: max bbone x/y, min bbone x/y, max head x/y, min head x/y, turn step */
    /* 0x1E */ be<s16> mHeadTurnSpd; /* lookBack */
    /* 0x20 */ be<f32> mAttnYOffset;
    /* 0x24 */ be<u8> mDebugDraw;
    /* 0x25 */ u8 _25[3];
    /* 0x28 */ be<s16> mWalkTurnStep;
    /* 0x2A */ u8 _2A[2];
    /* 0x2C */ be<f32> mWalkAnmRate;
    /* 0x30 */ be<f32> mWalkSpd;
    /* 0x34 */ be<f32> mWalkAccel;
    /* 0x38 */ be<f32> mPassDist;
};
WWHD_SIZE(daNpc_Ob1_HIO_c, 0x3C);

struct Ob1ActionWords {
    be<u32> mWords[2];
};
WWHD_SIZE(Ob1ActionWords, 8);

struct Ob1Joint {
    u8 _00[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(Ob1Joint, mJointNo, 4);

struct Ob1Pig {
    u8 _00[0x74D];
    be<u8> mPigBit;
};
WWHD_OFFSET(Ob1Pig, mPigBit, 0x74D);

struct Ob1PathPoint {
    u8 _00[4];
    cXyz mPosition;
};
WWHD_SIZE(Ob1PathPoint, 0x10);
struct Ob1Path {
    u8 _00[5];
    be<u8> mClosed;
    u8 _06[2];
    gptr<Ob1PathPoint> mPoints;
};
WWHD_OFFSET(Ob1Path, mClosed, 5);

void Ob1_npcCtor(fopNpc_npc_c* p);

void Ob1_btpCtor(mDoExt_btpAnm_ob1* p);

void Ob1_eventCutCtor(dNpc_EventCut_c* p);

void Ob1_headerByteCtor0(be<u8>* p);

void Ob1_headerByteCtor1(be<u8>* p);

WWHD_SIZE(daNpc_Ob1_c::anm_prm_c, 0x14);
WWHD_SIZE(Ob1Path, 0xC);

WWHD_OFFSET(daNpc_Ob1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ob1_c, m_hed_jnt_num, 0x7E4);
WWHD_OFFSET(daNpc_Ob1_c, m_bbone_jnt_num, 0x7E5);
WWHD_OFFSET(daNpc_Ob1_c, mpHedModel, 0x7E8);
WWHD_OFFSET(daNpc_Ob1_c, m_hed_tex_pttrn, 0x7EC);
WWHD_OFFSET(daNpc_Ob1_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpc_Ob1_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Ob1_c, mBlinkTimer, 0x866);
WWHD_OFFSET(daNpc_Ob1_c, mAction, 0x868);
WWHD_OFFSET(daNpc_Ob1_c, mPathRun, 0x870);
WWHD_OFFSET(daNpc_Ob1_c, mEventCut2, 0x878);
WWHD_OFFSET(daNpc_Ob1_c, mSearchProcId, 0x8E4);
WWHD_OFFSET(daNpc_Ob1_c, mHomePos, 0x8E8);
WWHD_OFFSET(daNpc_Ob1_c, mHomeAngle, 0x8F4);
WWHD_OFFSET(daNpc_Ob1_c, mAngle, 0x8FA);
WWHD_OFFSET(daNpc_Ob1_c, mEyePos, 0x900);
WWHD_OFFSET(daNpc_Ob1_c, mLookPos, 0x90C);
WWHD_OFFSET(daNpc_Ob1_c, mTargetPos, 0x918);
WWHD_OFFSET(daNpc_Ob1_c, mPrevFrame, 0x924);
WWHD_OFFSET(daNpc_Ob1_c, mSpdTarget, 0x928);
WWHD_OFFSET(daNpc_Ob1_c, mSpdStep, 0x92C);
WWHD_OFFSET(daNpc_Ob1_c, mPassDist, 0x930);
WWHD_OFFSET(daNpc_Ob1_c, mSaveHeadY, 0x934);
WWHD_OFFSET(daNpc_Ob1_c, mSaveBboneY, 0x936);
WWHD_OFFSET(daNpc_Ob1_c, mSaveAngleY, 0x938);
WWHD_OFFSET(daNpc_Ob1_c, mMsgNo, 0x93C);
WWHD_OFFSET(daNpc_Ob1_c, mEventIdx, 0x940);
WWHD_OFFSET(daNpc_Ob1_c, mEvtNo, 0x942);
WWHD_OFFSET(daNpc_Ob1_c, mTimer, 0x946);
WWHD_OFFSET(daNpc_Ob1_c, mWaitTimer, 0x94A);
WWHD_OFFSET(daNpc_Ob1_c, mHeadTurnSpd, 0x94E);
WWHD_OFFSET(daNpc_Ob1_c, mTargetAngY, 0x950);
WWHD_OFFSET(daNpc_Ob1_c, mMsgStatus, 0x952);
WWHD_OFFSET(daNpc_Ob1_c, mAnmEnd, 0x954);
WWHD_OFFSET(daNpc_Ob1_c, mLoopCnt, 0x955);
WWHD_OFFSET(daNpc_Ob1_c, mPigCntGot, 0x957);
WWHD_OFFSET(daNpc_Ob1_c, mPigCntHere, 0x958);
WWHD_OFFSET(daNpc_Ob1_c, mPigCntNew, 0x959);
WWHD_OFFSET(daNpc_Ob1_c, mTalkStep, 0x95A);
WWHD_OFFSET(daNpc_Ob1_c, mPreItemNo, 0x95B);
WWHD_OFFSET(daNpc_Ob1_c, mbFirstPigTalk, 0x95C);
WWHD_OFFSET(daNpc_Ob1_c, mbPartnerMessage, 0x95D);
WWHD_OFFSET(daNpc_Ob1_c, mbHidden, 0x95E);
WWHD_OFFSET(daNpc_Ob1_c, mbKeepAngle, 0x95F);
WWHD_OFFSET(daNpc_Ob1_c, mbSkipMove, 0x960);
WWHD_OFFSET(daNpc_Ob1_c, mbHideBody, 0x961);
WWHD_OFFSET(daNpc_Ob1_c, mbPointPassed, 0x962);
WWHD_OFFSET(daNpc_Ob1_c, mbInit, 0x963);
WWHD_OFFSET(daNpc_Ob1_c, mActRet, 0x964);
WWHD_OFFSET(daNpc_Ob1_c, mbAttention, 0x968);
WWHD_OFFSET(daNpc_Ob1_c, mbTalk, 0x969);
WWHD_OFFSET(daNpc_Ob1_c, mbHeadOnly, 0x96A);
WWHD_OFFSET(daNpc_Ob1_c, mbDemo, 0x96B);
WWHD_OFFSET(daNpc_Ob1_c, mMoveMode, 0x96C);
WWHD_OFFSET(daNpc_Ob1_c, mActIdx, 0x96E);
WWHD_OFFSET(daNpc_Ob1_c, mEvtActNo, 0x96F);
WWHD_OFFSET(daNpc_Ob1_c, mAnmAtr, 0x970);
WWHD_OFFSET(daNpc_Ob1_c, mAnmTag, 0x971);
WWHD_OFFSET(daNpc_Ob1_c, mTexNo, 0x972);
WWHD_OFFSET(daNpc_Ob1_c, mAnmNo, 0x973);
WWHD_OFFSET(daNpc_Ob1_c, mOrderType, 0x974);
WWHD_OFFSET(daNpc_Ob1_c, mStt, 0x975);
WWHD_OFFSET(daNpc_Ob1_c, mPrevStt, 0x976);
WWHD_OFFSET(daNpc_Ob1_c, mLookMode, 0x977);
WWHD_OFFSET(daNpc_Ob1_c, mCharacterFlags, 0x978);
WWHD_OFFSET(daNpc_Ob1_c, mType, 0x979);
WWHD_OFFSET(daNpc_Ob1_c, mActStep, 0x97A);
WWHD_OFFSET(daNpc_Ob1_c, mAtrCnt, 0x97B);