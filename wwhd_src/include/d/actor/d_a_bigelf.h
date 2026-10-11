/* Great Fairy layout.
 * HD: the base actor and animation wrappers grow; action PTMFs shrink to eight bytes.
 * The GameCube fairy pointer is a process ID, and the point light gains an intensity word. */
#pragma once
#include "gc_api/gc_api.h"
struct daBigelf_c : fopAc_ac_c {
  public:
    using ActionFunc = BOOL (daBigelf_c::*)(void*);
    enum StateFlag {
        FLAG_CUT_PENDING = 0x1,
        FLAG_HIDDEN = 0x2,
        FLAG_TALK_CONTROL = 0x4,
        FLAG_FLOWER_VISIBLE = 0x8,
        FLAG_DISAPPEARING = 0x10,
        FLAG_FACE_FIXED = 0x20,
        FLAG_OCT_EXIT_PENDING = 0x40,
        FLAG_LIGHT_CREATED = 0x80,
        FLAG_APPEAR_SOUND_PENDING = 0x100,
        FLAG_DIM_SOUND_PENDING = 0x200,
        FLAG_CHIME_SOUND_PENDING = 0x400
    };
    enum State { STATE_HUNT, STATE_READY, STATE_EVENT, STATE_DONE, STATE_SEARCH_OCT, STATE_OCT };
    enum DemoAction {
        ACT_WAIT,
        ACT_FA1,
        ACT_APPEAR,
        ACT_TALK,
        ACT_EXIT,
        ACT_FL_DEMO,
        ACT_FL_LINK,
        ACT_FL_DELETE,
        ACT_FL_DM_BF,
        ACT_FL_DM_MD,
        ACT_FL_DM_AF
    };
    u32 getType();
    u32 getEventFlag();
    void makeFa1S();
    void makeFa1();
    u32 getSwbit();
    u32 getSwbit2();
    u32 getMsg();
    void talkInit();
    void lightProc();
    void demoInitFlDelete();
    void darkInit();
    void lightEnd();
    void lightInit(cXyz* position);
    void demoInitFlDmMd();
    void darkEnd();
    void darkProc();
    void demoProcCom();
    void setAttention(BOOL force);
    void lookBack();
    s32 getNowEventAction();
    void demoInitWait();
    u32 demoProcWait();
    u32 demoProcFlLink();
    void demoInitFa1();
    u32 demoProcFa1();
    u32 demoProcTalk();
    u32 hunt();
    BOOL _delete();
    BOOL oct_search();
    void setAnm(s32 animation);
    void setAnmStatus();
    u32 next_msgStatus(be<u32>* message);
    u32 talk();
    void demoInitCom();
    void demoInitAppear();
    void demoInitTalk();
    void demoInitExit();
    void demoInitFlLink();
    void demoInitFlDmAf();
    u32 demoProcAppear();
    u32 demoProcExit();
    u32 demoProcFlDmBf();
    u32 demoProcFlDmAf();
    u32 demoProcFlDemo();
    u32 demoProcFlDelete();
    u32 demoProcFlDmMd();
    void demoProc();
    u32 ready0();
    u32 event0();
    u32 oct();
    void oct_delete();
    BOOL wait_action(void* argument);
    BOOL nodeCallBack(void* node);
    BOOL CreateHeap();
    BOOL init();
    s32 _create();
    BOOL _execute();
    BOOL _draw();

    /* 0x3AC */ request_of_phase_process_class mPhaseProcReq;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpBckAnimator;
    /* 0x3B8 */ mDoExt_brkAnm mBrkAnimator;
    /* 0x430 */ mDoExt_btkAnm mBtkAnimator;
    /* 0x4A4 */ gptr<J3DModel> mpFlowerModel;
    /* 0x4A8 */ mDoExt_brkAnm mFlowerBrkAnimator;
    /* 0x520 */ be<s8> mHandRBJointIndex;
    /* 0x521 */ u8 _pad521[3];
    /* 0x524 */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x558 */ cXyz mUnkPos, mCurrentPos;
    /* 0x570 */ be<u16> _pad570;
    /* 0x572 */ be<s8> mAnmFinished;
    /* 0x573 */ be<u8> mAttnSetCount;
    /* 0x574 */ be<f32> mPrevFrame;
    /* 0x578 */ be<u32> mCurrentMsgNo;
    /* 0x57C */ be<u16> mStateBits, _pad57E;
    /* 0x580 */ be<u8> mMsgState, _pad581, mGivenItem, _pad583;
    /* 0x584 */ be<u32> mCurrentMessageId, mpFairyActor;
    /* 0x58C */ be<s16> mHeadTurn;
    /* 0x58E */ be<u8> mIsLightShining, _pad58F;
    /* 0x590 */ LIGHT_INFLUENCE mLightInfluence;
    /* 0x5B4 */ cXyz mLightInfluencePos;
    /* 0x5C0 */ GXColorS10 mLightInfluenceColor;
    /* 0x5C8 */ be<f32> mLightInfluencePower, mLightInfluenceFluctuation, _pad5D0,
        mLightInfluenceTargetIntensity, _pad5D8, _pad5DC;
    /* 0x5E0 */ be<u8> mDarkActive;
    /* 0x5E1 */ u8 _pad5E1[3];
    /* 0x5E4 */ be<f32> m3A0, m3A4, m3A8, m3AC;
    /* 0x5F4 */ be<s16> mOctTimer, _pad5F6;
    /* 0x5F8 */ gabi::ptmf<ActionFunc> mCurrentStateFunc;
    /* 0x600 */ be<s8> mAnmID;
    /* 0x601 */ be<u8> mState, _pad602, _pad603;
    /* 0x604 */ be<s16> mDemoCounter, _pad606;
    /* 0x608 */ be<s32> mStaffId;
    /* 0x60C */ be<s16> eventID, mAttentionDelay;
    /* 0x610 */ be<u32> mpAppearEmitter;
    /* 0x614 */ cXyz mHandPos;
    /* 0x620 */ be<u8> mLightState;
    /* 0x621 */ u8 _pad621[3];
    /* 0x624 */ be<u32> mpChangeEmitter, mpLinkEmitter;
    /* 0x62C */ be<f32> mHeightOffset, mDisappearScale, mDisappearTarget, mLightRadius;
    /* 0x63C */ be<s8> mInitialized, mColorType, mActionPhase, mTalkPhase;
    /* 0x640 */ be<u32> mOctID;
};
WWHD_OFFSET(daBigelf_c, mpBckAnimator, 0x3B4);
WWHD_OFFSET(daBigelf_c, mBrkAnimator, 0x3B8);
WWHD_OFFSET(daBigelf_c, mBtkAnimator, 0x430);
WWHD_OFFSET(daBigelf_c, mpFlowerModel, 0x4A4);
WWHD_OFFSET(daBigelf_c, mJntCtrl, 0x524);
WWHD_OFFSET(daBigelf_c, mUnkPos, 0x558);
WWHD_OFFSET(daBigelf_c, mStateBits, 0x57C);
WWHD_OFFSET(daBigelf_c, mCurrentMessageId, 0x584);
WWHD_OFFSET(daBigelf_c, mLightInfluence, 0x590);
WWHD_OFFSET(daBigelf_c, mDarkActive, 0x5E0);
WWHD_OFFSET(daBigelf_c, mCurrentStateFunc, 0x5F8);
WWHD_OFFSET(daBigelf_c, mDemoCounter, 0x604);
WWHD_OFFSET(daBigelf_c, mHandPos, 0x614);
WWHD_OFFSET(daBigelf_c, mLightState, 0x620);
WWHD_OFFSET(daBigelf_c, mHeightOffset, 0x62C);
WWHD_OFFSET(daBigelf_c, mTalkPhase, 0x63F);
WWHD_SIZE(daBigelf_c, 0x644);

void bigelf_setActColorRatio(f32 value);
void bigelf_setBackgroundColorRatio(f32 value);
void bigelf_setVrboxColorRatio(f32 value);
void bigelf_setPriorityLight(LIGHT_INFLUENCE* light);
void bigelf_constructJointController(dNpc_JntCtrl_c* controller);

// HD: private call bindings preserve the verified register widths.
u32 bigelf_createActor(s32 name, u32 parameters, cXyz* position, s32 room, csXyz* angle, u32 scale,
                       s32 parent, u32 append);
daBigelf_c* bigelf_allocateActor(u32 size);
void bigelf_constructActor(fopAc_ac_c* actor);
void bigelf_deleteObject(void* object);
void bigelf_destroyActor(fopAc_ac_c* actor, u32 flags);
u32 bigelf_getGameInfo();
s32 bigelf_getActionIndex(void* manager, s32 staff, void* names, s32 count, s32 mode, s32 flag);
u32 bigelf_getSubstance(void* manager, s32 staff, const char* name, s32 type);
void bigelf_endCut(void* manager, s32 staff);
u32 bigelf_judgeActor(void* judge, be<u32>* argument);
void bigelf_changeFairy(void* fairy);
s32 bigelf_getReverb(s32 room);
void bigelf_startSound(u32 sound, cXyz* position, u32 argument, s32 reverb);
u32 bigelf_getEventIndex(void* manager, const char* name, s32 mapToolId);
void bigelf_orderOtherEvent(fopAc_ac_c* actor, s32 event, u32 mapToolId, u32 flags, u32 priority,
                            u32 mode);
void bigelf_deleteResource(request_of_phase_process_class* phase, const char* archive);
void bigelf_stopAnimation(void* animator);
u32 bigelf_searchActorName(const char* name, u32 argument, u32 flags);
u32 bigelf_getResource(void* manager, void* name, s32 index);
void bigelf_setMorfAnimation(void* animator, void* animation, s32 mode, u32 argument, f32 morph,
                             f32 speed, f32 start, f32 end);
u32 bigelf_setMessage(void* manager, u32 message, cXyz* position);
u32 bigelf_getMessageStatus(void* manager);
void bigelf_setMessageStatus(void* manager, u32 status);
u32 bigelf_setParticle(void* manager, u32 group, u16 effect, cXyz* position, csXyz* angle,
                       cXyz* scale, u32 alpha, u32 callback, s32 room, u32 primary, u32 environment,
                       u32 extra);
void bigelf_absolutePosition(void* actor, cXyz* offset, cXyz* position);
u32 bigelf_targetAngleY(cXyz* source, cXyz* target);
void bigelf_startShipPosition(void* ship, cXyz* position, s32 angle);
u32 bigelf_getEventProcessId(void* control, fopAc_ac_c* actor);
void bigelf_deleteActor(void* actor);
void bigelf_startShock(void* vibration, u32 type, s32 duration, cXyz* direction);
void bigelf_startBgm(u32 sound);
void bigelf_assertion(const char* file, u32 line, const char* condition);
void bigelf_startSystemSound(u32 sound);
void bigelf_stopBgm(u32 duration);
BOOL bigelf_isAdvance(void* manager, s32 staff);
u32 bigelf_getStaffId(void* manager, const char* name, u32 actor, u32 argument);
BOOL bigelf_eventEndCheck(void* manager, s32 event);
void bigelf_onEventBit(void* events, u32 bit);
void bigelf_onSwitch(void* memory, u32 bit, s32 room);
BOOL bigelf_isSwitch(void* memory, u32 bit, s32 room);
void bigelf_orderChangeEvent(fopAc_ac_c* actor, void* player, u32 event, u32 priority, u32 flags);
u32 bigelf_isEventBit(void* events, u32 bit);
void bigelf_registerGlobal(void* descriptor);
void bigelf_constructStaticColor(void* color);
void bigelf_constructStaticVector(void* vector);
void bigelf_copyMatrix(void* source, void* destination);
u32 bigelf_getJoint(void* node);
void bigelf_rotateZ(void* matrix, s32 angle);
void bigelf_matrixPosition(cXyz* source, cXyz* destination);
u32 bigelf_getJointNames(void* data);
u32 bigelf_getJointIndex(void* names, const char* name);
void bigelf_translateMatrix(void* matrix, f32 x, f32 y, f32 z);
void bigelf_rotateY(void* matrix, s32 angle);
void bigelf_calculateMorf(void* animator);
u32 bigelf_getJointTree(void* data);
u32 bigelf_createModel(void* data, u32 modelFlags, u32 differenceFlags);
s32 bigelf_loadResource(request_of_phase_process_class* phase, const char* archive);
using BigelfHeapCallback = BOOL (*)(daBigelf_c*);
BOOL bigelf_entrySolidHeap(fopAc_ac_c* actor, BigelfHeapCallback callback, u32 size);
u32 bigelf_playMorf(void* animator, cXyz* position, u32 sound, u32 reverb);
void bigelf_translate(f32 x, f64 y, f32 z);
void bigelf_scale(f32 x, f32 y, f32 z);
void bigelf_customColorSet(s32 source, s32 destination, f32 blend);
u32 bigelf_getEnvironment();
void bigelf_settingTev(void* environment, u32 light, cXyz* position, dKy_tevstr_c* tev);
void bigelf_setLightTev(void* environment, void* model, dKy_tevstr_c* tev);
void bigelf_updateMorf(void* animator);
void bigelf_updateModel(void* model, u32 flags);

void bigelf_cLib_addCalc2(void* value, f32 target, f32 scale, f32 maximum);
s16 bigelf_fopAcM_searchActorAngleY(fopAc_ac_c* actor, void* other);
f32 bigelf_fopAcM_searchActorDistanceXZ(fopAc_ac_c* actor, void* other);
void bigelf_cLib_addCalc0(be<f32>* value, f32 scale, f32 maximum);
void bigelf_cLib_addCalcAngleS(be<s16>* value, s32 target, s32 scale, s32 maximum, s32 minimum);
u32 bigelf_mDoExt_McaMorf_construct(u32 self, void* model, u32 callback1, u32 callback2,
                                    void* animation, s32 mode, s32 start, s32 end, s32 modify,
                                    u32 sound, u32 modelFlags, u32 differenceFlags, f32 speed);
BOOL bigelf_mDoExt_brkAnm_init(mDoExt_brkAnm* animator, void* model, void* animation, s32 play,
                               s32 mode, s32 start, s32 end, s32 modify, s32 entry, f32 speed);
BOOL bigelf_mDoExt_btkAnm_init(mDoExt_btkAnm* animator, void* model, void* animation, s32 play,
                               s32 mode, s32 start, s32 end, s32 modify, s32 entry, f32 speed);
void bigelf_dNpc_JntCtrl_setParam(dNpc_JntCtrl_c* controller, u32 a, u32 b, u32 c, u32 d, s32 headX,
                                  s32 headY, s32 backboneX, s32 backboneY, s32 step);
void bigelf_mDoExt_baseAnm_play(mDoExt_baseAnm* animator);
void bigelf_cLib_addCalc(be<f32>* value, f32 target, f32 scale, f32 maximum, f32 minimum);
void bigelf_mDoExt_brkAnm_entry(mDoExt_brkAnm* animator, void* model, f32 frame);
void bigelf_mDoExt_btkAnm_entry(mDoExt_btkAnm* animator, void* model, f32 frame);

struct BigelfGlobalWord {
    be<u32> value;
};
WWHD_SIZE(BigelfGlobalWord, 4);

// HD: emitter color/flag fields follow the larger particle state; only this actor's fields are
// exposed.
struct BigelfEmitterView {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x244 - 0x60];
    GXColor mGlobalPrmColor;
    u8 _248[0x254 - 0x248];
    be<u32> mFlags;
};
WWHD_OFFSET(BigelfEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(BigelfEmitterView, mGlobalPrmColor, 0x244);
WWHD_OFFSET(BigelfEmitterView, mFlags, 0x254);
WWHD_SIZE(BigelfEmitterView, 0x258);
struct BigelfJointData {
    u8 _00[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(BigelfJointData, mJntNo, 4);
WWHD_SIZE(BigelfJointData, 6);
struct BigelfJointTree {
    u8 _00[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(BigelfJointTree, mJointNum, 8);
WWHD_SIZE(BigelfJointTree, 10);
struct BigelfNameTable {
    u8 _00[0x10];
    be<u32> mNamesOffset;
};
WWHD_OFFSET(BigelfNameTable, mNamesOffset, 0x10);
WWHD_SIZE(BigelfNameTable, 0x14);
struct BigelfHeaderStatics {
    be<f32> mMinimumAngle, mMaximumAngle;
    u8 mColorConstructor, mVectorConstructor, _0A[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(BigelfHeaderStatics, mZero, 0xC);
WWHD_SIZE(BigelfHeaderStatics, 0x1C);
struct BigelfDestructorRecord {
    be<u32> mWords[3];
};
WWHD_SIZE(BigelfDestructorRecord, 12);

// HD: actor process numbers precede the GameCube IDs by two in this build.
enum { fpcNm_NPC_FA1_e = 0x168, fpcNm_BIGELF_e = 0x171 };
enum {
    dRes_INDEX_BIGELF_BCK_APPEAR_e = 6,
    dRes_INDEX_BIGELF_BCK_FLOWER_e = 7,
    dRes_INDEX_BIGELF_BCK_IDLE_e = 8,
    dRes_INDEX_BIGELF_BDL_BIGELF_e = 11,
    dRes_INDEX_BIGELF_BDL_FLOWER_e = 12,
    dRes_INDEX_BIGELF_BRK_BIGELF_e = 15,
    dRes_INDEX_BIGELF_BRK_FLOWER_e = 16,
    dRes_INDEX_BIGELF_BTK_BIGELF_e = 19
};

static daBigelf_c* bigelf_actor__ct(daBigelf_c* actor);

static BOOL daBigelf_IsDelete(daBigelf_c* actor);

static void bigelf_SafeString__dt(void* object, u32 flags);

static void bigelf_actor__dt(daBigelf_c* actor, u32 flags);

static void bigelf_SafeString_assureTerminationImpl_();

static BOOL daBigelf_Delete(daBigelf_c* actor);

static void __sinit_d_a_bigelf_cpp();

static BOOL nodeCallBack_Bigelf(void* node, s32 phase);

static BOOL CheckCreateHeap(daBigelf_c* actor);

static s32 daBigelf_Create(daBigelf_c* actor);

static BOOL daBigelf_Execute(daBigelf_c* actor);

static BOOL daBigelf_Draw(daBigelf_c* actor);

// HD: joint nodes occupy 28 bytes; the shared J3DJoint exposes only their first fields.
struct BigelfJointNodeView : J3DJoint {
    u8 _0C[0x1C - 0x0C];
};
WWHD_SIZE(BigelfJointNodeView, 0x1C);

// Upgrade items used by this actor (tww d_item_data.h names).
enum {
    dItemNo_MAX_RUPEE_UP1_e = 0xAB,
    dItemNo_MAX_RUPEE_UP2_e = 0xAC,
    dItemNo_MAX_BOMB_UP1_e = 0xAD,
    dItemNo_MAX_BOMB_UP2_e = 0xAE,
    dItemNo_MAX_ARROW_UP1_e = 0xAF,
    dItemNo_MAX_ARROW_UP2_e = 0xB0,
    dItemNo_RED_RUPEE_e = 4
};
