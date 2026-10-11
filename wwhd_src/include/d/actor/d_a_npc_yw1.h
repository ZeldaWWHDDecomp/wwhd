/* Sue-Belle (Outset), HD actor layout. GameCube actor fields are unavailable;
 * named fields preserve verified HD offsets. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/tu.h"

// HD: private views of virtual slots and the mutable one-based talk partner list.
struct Yw1VirtualTable {
    be<u32> mWords[10];
};
struct Yw1VirtualObject {
    gptr<Yw1VirtualTable> mVtable;
};
struct Yw1AnmObject {
    u8 _00[4];
    gptr<Yw1VirtualTable> mVtable;
};
struct Yw1PtmfVirtual {
    u8 _00[6];
    be<s16> mVtableOffset;
};
struct Yw1TalkList {
    be<u32> mUnused;
    be<s32> mCount;
    gptr<fopAc_ac_c> mActors[1];
};
struct Yw1ModelEffects {
    u8 _00[0x38];
    be<u32> mTexPattern;
};
WWHD_SIZE(Yw1VirtualObject, 4);
WWHD_OFFSET(Yw1AnmObject, mVtable, 4);
WWHD_OFFSET(Yw1PtmfVirtual, mVtableOffset, 6);
WWHD_OFFSET(Yw1TalkList, mActors, 8);
WWHD_OFFSET(Yw1ModelEffects, mTexPattern, 0x38);

template <class T> inline T* Yw1_view(u32 address) { return gabi::at<T>(address); }
enum Yw1Resource { Yw1BodyBdl = 6 };

/* GHS pointer to member function: {s16 this delta, s16 vtable index (0: NULL, < 0: not virtual),
 * u32 function (or, low half, the vtable offset)} */
struct daNpc_Yw1_c;
using Yw1ActionMethod = BOOL (daNpc_Yw1_c::*)(void*);
struct Yw1ActionFunc {
    gabi::ptmf<Yw1ActionMethod> mFunction;
};
WWHD_SIZE(Yw1ActionFunc, 8);

/* HD: unit-local ABI helpers. */

/* J3DModelData::getJointName()->getIndex(name): name table self-relative at +0x10 of the joint block */
/* getJointNodePointer(i)->setCallBack(cb): HD nodes of 0x1C bytes from +8, index checked against +4 */
static inline void Yw1_setJointCallback(u32 data, u32 i, u32 cb) {
    Yw1_view<J3DModelData>(data)->getJointNodePointer((u16)i)->mCallBack = cb;
}
static inline u32 Yw1_modelData(J3DModel* m) { return gabi::ea(m->getModelData()); }
/* J3DModel::getAnmMtx (HD: joint matrices in a block at +0x2C, marked dirty) */
/* J3DAnmTexPattern::getFrameMax: HD virtual (vtable at +4, slot +0x14) */
static inline s32 Yw1_patternFrameMax(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(Yw1_view<Yw1AnmObject>(gabi::ea(p))->mVtable->mWords[5], p);
}
BOOL mDoExt_btpAnm_init(void* a, u32 d, J3DAnmTexPattern* btp, s32 anmPlay, s32 mode, f32 rate,
                        s16 start, s16 end, s32 modify, s32 entry);

s16 cLib_calcTimer(be<s16>* t);

s32 cLib_getRndValue(s32 min, s32 range);
/* the ground polygon: dBgS_GndChk's cBgS_PolyInfo (+0x14) */
static inline void* gnd_poly(dBgS_ObjAcch& a) { return static_cast<cBgS_PolyInfo*>(&a.m_gnd); }
s32 dBgS_GetRoomId(dBgS* bgs, void* poly);
s32 dBgS_GetPolyColor(dBgS* bgs, void* poly);
/* getPoint returns a cXyz: hidden result pointer after `this` */
void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel,
                  u8 headOnly);
void setActorInfo2(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a);

void* dDemo_object_getActor(u32 obj, u8 id);
J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc);
BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p,
                       u32 q, s8 r);
/* dAttention_c (play+0x5804): LockonTruth, LockonTarget(i) (matcher: ActionTarget), ActionTarget(i)
 * (matcher: LockonTarget) */
BOOL dAttention_LockonTruth(dAttention_c* a);
fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i);
BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out);
// HD: message state is reached through the shared NPC manager pointer.
static inline u32 l_msgMng() { return gabi::ea(fopNpc_npc_c::mpCurrMsg.get()); }
u32 fopMsgM_SearchByID(u32 mng);
u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos);
/* play+0x5C30: {?, s32 n, fopAc_ac_c* list[] (1-based)}: the current talk partner */
static inline u32 Yw1_talkActor(u32 play) {
    Yw1TalkList* list = Yw1_view<Yw1TalkList>(play + 0x5C30);
    s32 count = list->mCount;
    return gabi::ea(list->mActors[count - 1].get());
}
void Yw1_copyBytes(u32 dst, u32 src, u32 n); /* memcpy (import) */
/* GHS pointer-to-member call with one argument */
static inline s32 Yw1_callAction(Yw1ActionFunc* f, void* self, void* arg) {
    s16 idx = f->mFunction.i;
    void* p = Yw1_view<void>(gabi::ea(self) + (s16)f->mFunction.d);
    if (idx < 0)
        return gabi::call_ptr<s32>(f->mFunction.f, p, arg);
    s16 offset = Yw1_view<Yw1PtmfVirtual>(gabi::ea(f))->mVtableOffset;
    Yw1VirtualTable* vt = *Yw1_view<gptr<Yw1VirtualTable>>(gabi::ea(p) + offset);
    return gabi::call_ptr<s32>(vt->mWords[idx * 2 + 1], p, arg);
}
/* virtuals (actor vtable at +0xB4: getMsg +0x1C, anmAtr +0x24) */
static inline u32 Yw1_virtualGetMsg(fopNpc_npc_c* a) {
    return gabi::call_ptr<u32>(Yw1_view<Yw1VirtualTable>(a->__vtbl)->mWords[7], a);
}
static inline void Yw1_virtualAnmAtr(fopNpc_npc_c* a, u16 st) {
    gabi::call_ptr(Yw1_view<Yw1VirtualTable>(a->__vtbl)->mWords[9], a, st);
}

static inline void dNpc_PathRun_setInf(dNpc_PathRun_c* p, u8 path, s8 room, u8 dir) {
    p->setInf(path, room, dir);
}
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_c* p, cXyz* out, u8 idx) {
    p->getPoint(out, idx);
}
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_c* p, cXyz* pos, bool dir) {
    return p->chkPointPass(pos, dir);
}
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_c* p) { return p->nextIdxAuto(); }
static inline u8 dNpc_PathRun_maxPoint(dNpc_PathRun_c* p) { return p->maxPoint(); }
struct Yw1ActionWords {
    be<u32> mWords[2];
};
struct daNpc_Yw1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Hair(J3DNode* node, J3DModel* model);
    void _nodeCB_Head(J3DNode* node, J3DModel* model);
    void _nodeCB_BackBone(J3DNode* node, J3DModel* model);
    BOOL bodyCreateHeap();
    s32 btpResID(int num);
    BOOL init_texPttrnAnm(s8 num, s32 modify);
    BOOL headCreateHeap();
    BOOL CreateHeap();
    u8 decideType(int type);
    BOOL set_action(Yw1ActionFunc* action, void* arg);
    void set_pthPoint(u8 idx);
    BOOL init_YW1_0();
    BOOL init_YW1_1();
    BOOL init_YW1_2();
    BOOL init_YW1_3();
    void play_texPttrnAnm();
    void play_animation();
    void setHairAngle();
    fopAc_ac_c* searchByID(fpc_ProcID id, be<s32>* pErr);
    u8 upLift();
    void setAttention(s32 force);
    void setMtx(s32 force);
    BOOL createInit();
    cPhs_State _create();
    BOOL _delete();
    BOOL partner_search_sub(u32 searchFn);
    void partner_search();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void endEvent();
    void privateCut(int staffIdx);
    void lookBack();
    void event_proc(int staffIdx);
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 bckResID(int num);
    void setAnm_anm(anm_prm_c* prm);
    void setAnm_NUM(int num, int withTex);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u8 atr);
    void anmAtr(u16 msgStatus);
    u16 next_msgStatus(u32* pMsgNo);
    u32 getMsg_YW1_0();
    u32 getMsg_YW1_1();
    u32 getMsg_YW1_2();
    u32 getMsg_YW1_3();
    u32 getMsg();
    BOOL chk_talk();
    u8 chk_parts_notMov();
    BOOL chkAttention();
    void chngTsuboAnm();
    void setStt(s8 stt);
    BOOL chk_areaIN(f32 r, f32 h, s16 ang, cXyz* pos);
    u8 chk_brkTsubo();
    u8 chk_bm1Odoroki();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL walk_1();
    BOOL turn_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hair1_jnt_num;
    /* 0x7E7 */ be<s8> m_hair2_jnt_num;
    /* 0x7E8 */ be<s8> m_hair3_jnt_num;
    /* 0x7E9 */ u8 _7E9[7];
    /* 0x7F0 */ gptr<J3DModel> mpHedModel;
    /* 0x7F4 */ mDoExt_btpAnm mBtpAnm; /* its J3DAnmTexPattern* at +0x10 (0x804) */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ Yw1ActionFunc mAction;
    /* 0x874 */ be<u32> mBm1Id;   /* the Rito partner (Bm1) */
    /* 0x878 */ be<u32> mTsuboId; /* the pot she carries */
    /* 0x87C */ be<u32> mLookActorId;
    /* 0x880 */ dNpc_PathRun_c mPathRun;
    /* 0x888 */ cXyz mHomePos;
    /* 0x894 */ csXyz mHomeAngle;
    /* 0x89A */ csXyz mAngle;
    /* 0x8A0 */ cXyz mEyePos;
    /* 0x8AC */ cXyz mLookPos;
    /* 0x8B8 */ cXyz mAttPos;
    /* 0x8C4 */ be<f32> mPrevFrame;
    /* 0x8C8 */ u8 _8C8[4];
    /* 0x8CC */ be<s16> mSaveAngleY;
    /* 0x8CE */ be<s16> mSaveHeadY;
    /* 0x8D0 */ be<s16> mSaveBboneY;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<s32> mActRet;
    /* 0x8D8 */ u8 _8D8[2];
    /* 0x8DA */ be<s16> mWaitTimer;
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ be<s16> mHeadTurnSpd;
    /* 0x8E2 */ be<s16> mTargetAngY;
    /* 0x8E4 */ be<s8> mAnmEnd;
    /* 0x8E5 */ be<s8> mLoopCnt;
    /* 0x8E6 */ be<u8> mPreItemNo;
    /* 0x8E7 */ be<u8> mbTsubo;
    /* 0x8E8 */ be<u8> mbPotPenaltyPaid;
    /* 0x8E9 */ be<u8> mbTsuboLost;
    /* 0x8EA */ be<u8> mbPathEnd;
    /* 0x8EB */ be<u8> mbAwaitRescue;
    /* 0x8EC */ be<u8> mbKeepShapeAngle;
    /* 0x8ED */ be<u8> mbHidden;
    /* 0x8EE */ be<u8> mbInit;
    /* 0x8EF */ be<u8> mbAttention;
    /* 0x8F0 */ be<u8> mbTalk;
    /* 0x8F1 */ be<u8> mbHeadOnly;
    /* 0x8F2 */ be<u8> mbDemo;
    /* 0x8F3 */ u8 _8F3;
    /* 0x8F4 */ be<s16> mHeadPitch; /* hair (setHairAngle / _nodeCB_Hair) */
    /* 0x8F6 */ be<s16> mHeadYaw;
    /* 0x8F8 */ be<s16> mHair1Pitch;
    /* 0x8FA */ be<s16> mHair1Yaw;
    /* 0x8FC */ u8 _8FC[0x908 - 0x8FC];
    /* 0x908 */ cXyz mHairPrevPos;
    /* 0x914 */ be<s16> mHair1PitchVelocity;
    /* 0x916 */ be<s16> mHair1YawVelocity;
    /* 0x918 */ be<s16> mHair2Pitch;
    /* 0x91A */ be<s16> mHair2Yaw;
    /* 0x91C */ be<s16> mHair2PitchVelocity;
    /* 0x91E */ be<s16> mHair2YawVelocity;
    /* 0x920 */ be<s16> mHair3Pitch;
    /* 0x922 */ be<s16> mHair3Yaw;
    /* 0x924 */ be<s16> mHair3PitchVelocity;
    /* 0x926 */ be<s16> mHair3YawVelocity;
    /* 0x928 */ be<s16> mHairWave;
    /* 0x92A */ be<s16> mHair1Wave;
    /* 0x92C */ be<s16> mHair2Wave;
    /* 0x92E */ be<s16> mHair3Wave;
    /* 0x930 */ be<s8> mActIdx;
    /* 0x931 */ be<u8> mAnmAtr;
    /* 0x932 */ be<u8> mAnmTag;
    /* 0x933 */ be<s8> mTexNo;
    /* 0x934 */ be<s8> mAnmNo;
    /* 0x935 */ be<s8> mOrderType;
    /* 0x936 */ be<s8> mStt;
    /* 0x937 */ be<s8> mPrevStt;
    /* 0x938 */ be<s8> mLookMode;
    /* 0x939 */ be<s8> mHioNo;
    /* 0x93A */ be<s8> mType;
    /* 0x93B */ be<s8> mActStep;
    /* 0x93C */ be<s8> mAtrCnt;
    /* 0x93D */ u8 _93D[3];
};
WWHD_OFFSET(daNpc_Yw1_c, mBtpFrame, 0x868);
WWHD_OFFSET(daNpc_Yw1_c, mPathRun, 0x880);
WWHD_OFFSET(daNpc_Yw1_c, mAttPos, 0x8B8);
WWHD_OFFSET(daNpc_Yw1_c, mActRet, 0x8D4);
WWHD_OFFSET(daNpc_Yw1_c, mHeadPitch, 0x8F4);
WWHD_OFFSET(daNpc_Yw1_c, mHair1PitchVelocity, 0x914);
WWHD_OFFSET(daNpc_Yw1_c, mActIdx, 0x930);
WWHD_SIZE(daNpc_Yw1_c, 0x940);

/* l_HIO (0x10468A24, 0x44): vtable, s8 mNo, s32, daNpc_Yw1_childHIO_c mChild[1] (0x38 each; the
 * prm table 101C6F2C is copied to +4 of the child) */
struct daNpc_Yw1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s16> mPrm[9]; /* jnt setParam */
    /* 0x16 */ be<s16> mHeadTurnSpd;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<u8> mDebugDraw;
    /* 0x1D */ u8 _1D[3];
    /* 0x20 */ be<f32> mAreaRadius;
    /* 0x24 */ be<s16> mTurnScale;
    /* 0x26 */ be<s16> mTurnStep;
    /* 0x28 */ be<f32> mWalkAnmRate;
    /* 0x2C */ be<f32> mWalkSpd;
    /* 0x30 */ be<f32> mWalkAccel;
    /* 0x34 */ be<u32> mChildIndex;
};
WWHD_SIZE(daNpc_Yw1_childHIO_c, 0x38);
struct daNpc_Yw1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mSelectedChild;
    /* 0x0C */ daNpc_Yw1_childHIO_c mChild[1];
};
WWHD_SIZE(daNpc_Yw1_HIO_c, 0x44);

struct Yw1SearchResult {
    be<u32> mActor;
};
struct Yw1SearchError {
    be<s32> mValue;
};
struct Yw1WindPower {
    be<f32> mValue;
};
struct Yw1RitoView {
    u8 _00[0x9F0];
    be<u8> mbStartled;
};
WWHD_OFFSET(Yw1RitoView, mbStartled, 0x9F0);
extern cXyz& l_hairRoot;
extern cXyz& l_hairTip;

u32 Yw1_groundSound(dBgS* bgs, void* poly);

BOOL McaMorf_play(mDoExt_McaMorf* m, cXyz* pos, u32 se, s32 reverb);

void Yw1_setCarryNow(fopAc_ac_c* a, BOOL stageLayer);

void Yw1_constructArray(u32 p, u32 n, u32 size, u32 ctor);

void Yw1_npcCtor(fopNpc_npc_c* p);

void Yw1_headerByteCtor0(be<u8>* p);

void Yw1_headerByteCtor1(be<u8>* p);

struct Yw1HioParameters {
    be<s16> mJointParams[9];
    be<s16> mHeadTurnSpeed;
    be<f32> mAttentionOffsetY;
    be<u8> mbDebugDraw;
    u8 _pad[3];
    be<f32> mAreaRadius;
    be<s16> mTurnScale;
    be<s16> mTurnStep;
    be<f32> mWalkAnimationRate;
    be<f32> mWalkSpeed;
    be<f32> mWalkAcceleration;
};
WWHD_SIZE(Yw1HioParameters, 0x30);

WWHD_SIZE(Yw1VirtualTable, 0x28);
WWHD_SIZE(Yw1AnmObject, 8);
WWHD_SIZE(Yw1TalkList, 0xC);
WWHD_SIZE(Yw1ModelEffects, 0x3C);
WWHD_SIZE(Yw1ActionWords, 8);
WWHD_SIZE(Yw1SearchResult, 4);
WWHD_SIZE(Yw1SearchError, 4);
WWHD_SIZE(Yw1WindPower, 4);
