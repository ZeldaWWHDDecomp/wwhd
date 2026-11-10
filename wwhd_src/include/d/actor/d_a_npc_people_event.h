/* daNpcPeople_c (Windfall townspeople), WWHD layout, plus the translation-unit statics and
 * local bindings shared by the d_a_npc_people*.cpp parts.
 *
 * GameCube -> WWHD, measured from the constructor 022BE7CC (allocates 0x920) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h). Members +0x118 up to mBtpAnm (0x7FC);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6F8) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6FC) on is +0x174. Size 0x920 (GameCube 0x7A9). */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/f_op/f_op_msg.h"


struct sUbMsgDat {
    /* 0x00 */ be<u32> field_0x00;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ be<u8> field_0x06;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(sUbMsgDat, 8);

struct sPeopleAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<u8> field_0x02;
};
WWHD_SIZE(sPeopleAnmDat, 3);

struct daNpcPeople_c__l_npc_dat {
    /* 0x00 */ be<s16> mMaxHeadX;  /* was field_0x00: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxHeadX (5th argument; d_npc.h setParam order) (tww the same) */
    /* 0x02 */ be<s16> mMaxHeadY;  /* was field_0x02: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxHeadY (6th argument) (tww the same) */
    /* 0x04 */ be<s16> mMaxBackboneX;  /* was field_0x04: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxBackboneX (1st argument) (tww the same) */
    /* 0x06 */ be<s16> mMaxBackboneY;  /* was field_0x06: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxBackboneY (2nd argument) (tww the same) */
    /* 0x08 */ be<s16> mMinHeadX;  /* was field_0x08: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minHeadX (7th argument) (tww the same) */
    /* 0x0A */ be<s16> mMinHeadY;  /* was field_0x0A: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minHeadY (8th argument) (tww the same) */
    /* 0x0C */ be<s16> mMinBackboneX;  /* was field_0x0C: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minBackboneX (3rd argument) (tww the same) */
    /* 0x0E */ be<s16> mMinBackboneY;  /* was field_0x0E: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minBackboneY (4th argument) (tww the same) */
    /* 0x10 */ be<s16> mMaxTurnStep;  /* was field_0x10: createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxTurnStep (9th argument) (tww the same) */
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<f32> mPlayerEyeOffsetY;  /* was field_0x14: chkAttention 022C0084: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mPlayerEyeOffsetY */
    /* 0x18 */ be<f32> mAttnOffsetX;  /* was field_0x18: _execute 022C305C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same) */
    /* 0x1C */ be<f32> mAttnOffsetY;  /* was field_0x1C: _execute 022C305C: the Y of the attention position offset (see mAttnOffsetX) (tww the same) */
    /* 0x20 */ be<f32> mAttnOffsetZ;  /* was field_0x20: _execute 022C305C: the Z of the attention position offset (see mAttnOffsetX) (tww the same) */
    /* 0x24 */ be<f32> mEyeYOffset;  /* was field_0x24: _execute 022C305C: eyePos = current.pos + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mEyeYOffset */
    /* 0x28 */ be<f32> mMaxAttnDistXZ;  /* was field_0x28: createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / executeLook 022C511C / executeLook2 022C5268 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxAttnDistXZ; executeWait 022C4074 reads it (tww the same) */
    /* 0x2C */ be<f32> mMaxLookDistXZ;  /* was field_0x2C: chkAttention 022C0084: outside the attention range but closer than it, the player is still looked at (mLookMode 1, m78A set) (tww the same) */
    /* 0x30 */ be<f32> mCullFarDist;  /* was field_0x30: createInit 022BF510: fopAcM_setCullSizeFar(it / the clipper's far) (tww the same) */
    /* 0x34 */ be<s16> mMaxAttnAngleY;  /* was field_0x34: createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxAttnAngleY (tww the same) */
    /* 0x36 */ be<s16> mMaxLookAngleY;  /* was field_0x36: createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / eventOrder 022C2A04 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxLookAngleY (tww the same) */
    /* 0x38 */ be<s16> mMaxHeadTurnVel;  /* was field_0x38: chkAttention 022C0084: mLookMaxVelTarget, the speed lookBack eases the look toward (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxHeadTurnVel */
    /* 0x3A */ be<s16> mWalkHeadTurnVel;  /* was field_0x3A: executeWalk 022C46D0: mLookMaxVelTarget while walking the path; executeUgWalk 022C5434 scales it by mTargetSpeed / mWalkSpeed (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkHeadTurnVel */
    /* 0x3C */ be<f32> mCylRadius;  /* was field_0x3C: createInit 022BF510 / eventCoCylRInit 022C17FC copy it into mCylR, the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius */
    /* 0x40 */ be<f32> mCylHeight;  /* was field_0x40: createInit 022BF510 / _execute 022C305C: setCollision(&mCyl, .., mCylR, it), the body cylinder's height (tww the same) */
    /* 0x44 */ be<f32> mWalkAnmRate;  /* was field_0x44: _execute 022C305C: while walking (mAnmIdx 2) the morf's play speed is speedF * it, at least 0.5 (tww the same) */
    /* 0x48 */ be<f32> mWalkSpeed;  /* was field_0x48: executeWalk 022C46D0: mTargetSpeed while walking the path; executeUgWalk 022C5434 divides by it (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkSpeed */
    /* 0x4C */ be<s16> field_0x4C;
    /* 0x4E */ be<s16> field_0x4E;
    /* 0x50 */ be<s16> mWaitTimeMin;  /* was field_0x50: executeWaitInit 022C3D88: mWaitTimer = it + rnd(mWaitTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMin */
    /* 0x52 */ be<s16> mWaitTimeMax;  /* was field_0x52: executeWaitInit 022C3D88: mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMax */
    /* 0x54 */ be<s16> mWalkTimeMin;  /* was field_0x54: executeWalkInit 022C4600 / executeTurnInit 022C48B8: mWaitTimer = it + rnd(mWalkTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMin */
    /* 0x56 */ be<s16> mWalkTimeMax;  /* was field_0x56: executeWalkInit 022C4600 / executeTurnInit 022C48B8: mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMax */
    /* 0x58 */ be<s16> mHomeTurnDelay;  /* was field_0x58: chkAttention 022C0084 copies it into daNpcPeople_c::mHomeTurnDelay when the player leaves the attention range (tww the same) */
    /* 0x5A */ be<u8> mBodyTurn;  /* was field_0x5A: createInit 022BF510 / eventOrder 022C2A04 copy it into daNpcPeople_c::mBodyTurn (tww the same) */
    /* 0x5B */ be<u8> mTalkBodyTurn;  /* was field_0x5B: checkOrder 022C045C copies it into daNpcPeople_c::mBodyTurn when the talk starts (tww the same) */
    /* 0x5C */ be<u8> mLookAtPlayer;  /* was field_0x5C: createInit 022BF510 / eventOrder 022C2A04 copy it into daNpcPeople_c::mLookAtPlayer (eventOrder also into m79F) (tww the same) */
    /* 0x5D */ be<u8> field_0x5D;
    /* 0x5E */ be<u8> field_0x5E;
    /* 0x5F */ be<u8> field_0x5F;
};
WWHD_SIZE(daNpcPeople_c__l_npc_dat, 0x60);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct dNpc_PathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_l, 8);

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6
 * (same as ProcFunc_l in d_a_npc_ba1.h) */
struct PeoplePmf_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(PeoplePmf_l, 8);

struct daNpcPeople_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
        PRM_ARG0_W = 0x8,
        PRM_ARG0_S = 0x0,
    };

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhase; }
    void setEtcFlag(u32 flags) { mEtcFlag = mEtcFlag | flags; }
    void setResFlag(u8 flag) { mResFlag = flag; }
    void setAnmFlag(u8 flags) { mAnmFlag = mAnmFlag | flags; }
    void setTalk(u8 value) { mTalk = value; }
    void setNoTalk(u8 value) { mNoTalk = value; }
    void setOrderEventNum(u8 value) { mOrderEventNum = value; }

    /* the declarations follow the GameCube header; a part may adjust the return type of its
     * own functions to what the WWHD code returns */
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    bool executeCommon();
    void executeSetMode(u32); /* u8, not re-extended */
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    s32 executeBikkuriInit();
    void executeBikkuri();
    s32 executeFurueInit();
    void executeFurue();
    s32 executeKyoroInit();
    void executeKyoro();
    s32 executeLetterInit();
    void executeLetter();
    s32 executeLookInit();
    void executeLook();
    s32 executeLook2Init();
    void executeLook2();
    s32 executeUgWalkInit();
    void executeUgWalk();
    s32 executeUgTurnInit();
    void executeUgTurn();
    s32 executeUgLookInit();
    void executeUgLook();
    s32 executeUgLook2Init();
    void executeUgLook2();
    s32 executeUgSitInit();
    void executeUgSit();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetTpInit(int);
    void eventMesSetInit(int);
    bool eventMesSet();
    bool eventMesSet2();
    void eventFlagSetInit(int);
    void eventGetItemInit(int);
    bool eventGetItem();
    void eventTurnToPlayerInit();
    bool eventTurnToPlayer();
    void eventUb1TalkInit(int);
    bool eventUb1Talk();
    void eventUb1TalkXyInit(int);
    bool eventUb1TalkXy();
    bool eventUb2Talk();
    bool eventUbSetAnm();
    void eventAreaMaxInit();
    void eventCameraStopInit();
    bool eventCameraStop();
    void eventCameraStartInit();
    void eventCoCylRInit(int);
    bool eventLookPo();
    void eventMesSetPoInit(int);
    bool eventMesSetPo();
    u16 talk2(int, fopAc_ac_c*);
    u16 talk3(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u32 getMsg3();
    void chkMsg();
    void setMessage(u32);
    void setMessageUb(sUbMsgDat*);
    void setAnmFromMsgTag();
    u32 setAnmFromMsgTagUo(int);
    u32 setAnmFromMsgTagUb(int);
    u32 setAnmFromMsgTagUw(int);
    u32 setAnmFromMsgTagUm(int);
    u32 setAnmFromMsgTagSa(int);
    u32 setAnmFromMsgTagUg(int);
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    u8 getPrmArg0();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    int getBck(int); /* HD: the u8 index is not re-extended (full register) */
    int getHeadBck(int);
    void setAnm(u32, int, f32, f32); /* u8 index, full register */
    bool setAnmTbl(sPeopleAnmDat*, int);
    void setWaitAnm();
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    s16 photoCB(int);
    int getRand(int max);
    BOOL isPhoto(u8);
    BOOL isColor();
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    BOOL chkSurprise();
    BOOL chkEndEvent();
    BOOL is1GetMap20();
    BOOL is1DayGetMap20();
    int getWindDir();
    BOOL isUo1FdaiAll();
    BOOL isUo1FdaiOne();
    s32 chkDaiza();
    BOOL checkPig();
    BOOL isPigOk();
    s16 getPigTimer();
    void resetPig();
    void initUgSearchArea();
    void getDirDistToPos(cXyz* result, s16, f32); /* cXyz returned through a hidden pointer */
    void warp();

    /* 0x7DC */ request_of_phase_process_class mPhase;
    /* 0x7E4 */ request_of_phase_process_class mPhase2;
    /* 0x7EC */ gptr<J3DModel> mpHeadModel;
    /* 0x7F0 */ gptr<J3DModel> mpEtcModel;
    /* 0x7F4 */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x7F8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7FC */ u8 mBtpAnm[0x74];                  /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x870 */ dNpc_PathRun_l mPathRun;           /* HD: no mShadowId before it */
    /* 0x878 */ u8 m704[0x884 - 0x878];
    /* 0x884 */ cXyz mLookAtPos;
    /* 0x890 */ cXyz m71C;
    /* 0x89C */ gptr<sPeopleAnmDat> mpAnmTbl;  /* was m728: setAnmTbl 022BED70 stores the table (NULL to stop); playAnm 022C2D6C steps to the next entry and plays its animation */
    /* 0x8A0 */ gptr<daNpcPeople_c__l_npc_dat> mpNpcDat;
    /* 0x8A4 */ gptr<daNpcPeople_c> mpTalkPartner;  /* was m730: eventUb1TalkInit 022C1228 sets this / NULL; eventUb1TalkXyInit 022C1384 sets peopleSearchFromName("Ub2"); the Ub talk events talk through it */
    /* 0x8A8 */ gptr<be<u32>> mpMsgList;  /* was m734: getMsg3 022C1F20 points it at a message number list (l_msg_look_full_moon, ...); eventMesSetInit 022C0BF8 / next_msgStatus 022C6108 step through it */
    /* 0x8AC */ gptr<sUbMsgDat> mpUbMsg;  /* was m738: eventUb1TalkInit 022C1228 sets *mpUbMsgList and passes it to setMessageUb; next_msgStatus 022C6108 steps it */
    /* 0x8B0 */ gptr<gptr<sUbMsgDat>> mpUbMsgList;  /* was m73C: eventUb1TalkInit 022C1228 / eventUb1TalkXyInit 022C1384 select a l_msg_ub1_* / l_msg_xy_ub1_* list of sUbMsgDat pointers */
    /* 0x8B4 */ be<f32> mTargetSpeed;  /* was m740: _execute 022C305C chases speedF to it (cLib_chaseF); the walk / warp / event code sets it (0 to stop) */
    /* 0x8B8 */ be<f32> mMorf;  /* was m744: setAnm 022BEB7C uses it as the morf frames when not negative; setAnmTbl 022BED70 sets 16 / 13 */
    /* 0x8BC */ be<f32> mMaxAttnDistXZ;  /* was m748: chkAttention 022C0084: the player is in attention while closer than it (+40 while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 / executeLook 022C511C copy npc dat 0x28, setMessageUb 022C1168 / eventAreaMaxInit 022C167C / eventUb2Talk 022C2380 set 1000 (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnDistXZ */
    /* 0x8C0 */ be<f32> mCylR;  /* was m74C: createInit 022BF510 sets npc dat 0x3C and passes it to setCollision as the radius; eventCoCylRInit 022C17FC sets the event's radius */
    /* 0x8C4 */ be<f32> mAnmSpeed;  /* was m750: setAnmTbl 022BED70 sets 1 / -1; playAnm 022C2D6C passes it as setAnm's play speed */
    /* 0x8C8 */ u8 m754[0x8CC - 0x8C8];
    /* 0x8CC */ be<u32> mEtcFlag;
    /* 0x8D0 */ be<s32> mGetItemNo;  /* was m75C: next_msgStatus 022C6108 sets l_item_id_sa3[...] or 4; eventGetItemInit 022C0FC8 gives that item */
    /* 0x8D4 */ be<s32> mAnmPlayMode;  /* was m760: setAnm 022BEB7C stores the play mode it passes to mpMorf->setAnm (setAnmTbl 022BED70: 0 for counted entries, 2 for looping ones); setAnmTbl restarts an unchanged animation only while it is 0 (tww the same); same code as daNpcRoten_c::mAnmPlayMode */
    /* 0x8D8 */ be<u8> mLookHeadOnly;  /* was m764: lookBack 022C2E50 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022C0084 sets it (0 = turn the body), the walk / turn / furue / Ug modes set or clear it (tww the same); same code as daNpcRoten_c::mLookHeadOnly */
    /* 0x8D9 */ be<u8> m765;
    /* 0x8DA */ be<s16> mEventIdx[4];  /* was m766: createInit 022BF510 fills it with people_getEventIdx("UO1_GET_ITEM", ...) or -1; XyEventCB 022BF220 / eventOrder 022C2A04 order [i] */
    /* 0x8E2 */ be<s16> mWaitTimer;  /* was m76E: executeWaitInit 022C3D88 sets npc dat 0x50 + random; executeWait 022C4074 counts it down before walking the path */
    /* 0x8E4 */ be<s16> mHomeTurnDelay;  /* was m770: chkAttention 022C0084 sets npc dat 0x58 when the player leaves the attention range and, without a path, counts it down before turning back to mTargetAngleY (tww the same); same code as daNpcRoten_c::mHomeTurnDelay */
    /* 0x8E6 */ be<s16> mReactTimer;  /* was m772: executeBikkuriInit 022C4B30 sets 0x15, executeFurueInit 022C4BF4 0x96; executeBikkuri / executeFurue count it down */
    /* 0x8E8 */ be<s16> mFurueTimer;  /* was m774: executeFurueInit 022C4BF4 sets 10 + rnd(20); executeFurue 022C4CA8 counts it down */
    /* 0x8EA */ be<s16> mMaxAttnAngleY;  /* was m776: chkAttention 022C0084: the angle of the outer check, which only sets mCanTalk (+0x71C while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 copy npc dat 0x34, the Ub talk events 0x7FFF; checkOrder 022C045C copies it into mMaxLookAngleY when the talk starts (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnAngleY */
    /* 0x8EC */ be<s16> mMaxLookAngleY;  /* was m778: chkAttention 022C0084: the player is looked at (mInAttention, mLookMode 1) while the angle to them is below it (+0x71C while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 / eventOrder 022C2A04 copy npc dat 0x36, checkOrder 022C045C mMaxAttnAngleY at the talk start, the Ub talk events 0x7FFF (tww the same); daNpcRoten_c uses 0x4000 / 0x7FFF while talking in its place */
    /* 0x8EE */ be<s16> mTargetAngleY;  /* was m77A: eventTurnToPlayerInit 022C1094 sets the XZ angle to the player; eventTurnToPlayer 022C21D8 ends when current.angle.y equals it */
    /* 0x8F0 */ be<u16> mMsgStatus;  /* was m77C: talk3 022C1FD8 keeps the message status; talk2 022C1BBC resets 0xFFFF and tests MSG_CONTINUES / BOX_OPENING */
    /* 0x8F2 */ be<u16> m77E;
    /* 0x8F4 */ be<s16> mBlinkTimer;  /* was m780: initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 counts it down and restarts the head texture pattern with 0x78 */
    /* 0x8F6 */ be<s16> mLookMaxVelTarget;  /* was m782: chkAttention 022C0084 / executeWalk 022C46D0 set npc dat 0x38 / 0x3A; lookBack 022C2E50 eases mLookMaxVel toward it (the turn speed limit) */
    /* 0x8F8 */ be<s16> mLookMaxVel;  /* was m784: lookBack 022C2E50 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel; createInit 022BF510 clears it */
    /* 0x8FA */ be<s16> mLookAngleY;  /* was m786: lookBack 022C2E50: the target angle in look mode 2; chkAttention 022C0084 sets mTargetAngleY, executeWalk 022C46D0 / executeTurn 022C4A04 / executeUgWalk 022C5434 / executeUgTurn 022C5D4C the path angle, executeFurue 022C4CA8 +-0x4000 (tww the same); same code as daNpcRoten_c::mLookAngleY */
    /* 0x8FC */ be<u8> mTalk;
    /* 0x8FD */ be<u8> mInAttention;  /* was m789: chkAttention 022C0084: 1 once the player is in the look range (distance +40 and angles +0x71C while set), 0 when they leave; executeWait 022C4074 / executeWalk 022C46D0 / executeKyoro 022C4E98 / checkPig 022C3E78 / setWaitAnm 022BEEDC test it (tww the same); same code as daNpcRoten_c::mInAttention */
    /* 0x8FE */ be<u8> m78A;
    /* 0x8FF */ be<u8> mCanTalk;  /* was m78B: chkAttention 022C0084: 1 while the player is in the talk range (or the event attention is on), else 0; executeCommon 022C3AC4 sets mOrderEventNum 1 (can talk) while it is set (tww the same); same code as daNpcRoten_c::mCanTalk */
    /* 0x900 */ be<u8> mNoTalk;
    /* 0x901 */ be<u8> mOrderEventNum;
    /* 0x902 */ be<u8> mBtpFrame;  /* was m78E: initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 advances and wraps it; _draw 022C36D4 passes it to mDoExt_btpAnm_entry */
    /* 0x903 */ be<u8> mExecuteMode;  /* was m78F: _execute 022C305C calls moveProc[it]; executeCommon 022C3AC4 / chkSurprise 022C39F0 test it */
    /* 0x904 */ be<u8> mResFlag;
    /* 0x905 */ be<u8> mNpcNo;
    /* 0x906 */ be<u8> m792;
    /* 0x907 */ be<u8> mAnmIdx;  /* was m793: setAnm 022BEB7C stores the animation index; createHeap 022BE334 loads the bck by it; setAnmTbl 022BED70 / setWaitAnm 022BEEDC / _execute 022C305C / getMsg 022C6C68 test it (tww the same); same code as daNpcRoten_c::mAnmIdx */
    /* 0x908 */ be<u8> mLastRandNo;  /* was m794: _execute 022C305C draws a random number again when it equals this one, then keeps it here */
    /* 0x909 */ be<u8> mAnmFlag;
    /* 0x90A */ be<s8> mAnmLoopCount;  /* was m796: setAnmTbl 022BED70 sets the table entry's count (sPeopleAnmDat +0x02); playAnm 022C2D6C counts it down before the next entry */
    /* 0x90B */ be<s8> mCutNo;  /* was m797: privateCut 022C25E8 stores the event cut index and switches on it */
    /* 0x90C */ be<u8> m798;
    /* 0x90D */ be<s8> mLookMode;  /* was m799: lookBack 022C2E50 switches on it: 1 looks at mLookAtPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022C0084 and the walk / turn / look / Ug modes set it (tww the same); same code as daNpcRoten_c::mLookMode */
    /* 0x90E */ be<u8> m79A;
    /* 0x90F */ be<u8> mbIsNight;
    /* 0x910 */ be<u8> m79C;
    /* 0x911 */ be<u8> mBodyTurn;  /* was m79D: checkOrder 022C045C (talk start) copies npc dat 0x5B, createInit 022BF510 / eventOrder 022C2A04 0x5A, the Ug look modes set 1; while set, chkAttention 022C0084 and lookBack 022C2E50 (when talking) turn the body (mLookHeadOnly 0) (tww the same); same code as daNpcRoten_c::mBodyTurn */
    /* 0x912 */ be<u8> mLookAtPlayer;  /* was m79E: createInit 022BF510 / eventOrder 022C2A04 copy npc dat 0x5C, checkOrder 022C045C sets 1 at the talk start; chkAttention 022C0084: while 0 the player in range only turns him to mTargetAngleY instead of being looked at (tww the same); same role as daNpcPhoto_c::mLookAtPlayer */
    /* 0x913 */ be<u8> m79F;
    /* 0x914 */ be<u8> m7A0;
    /* 0x915 */ be<u8> m7A1;
    /* 0x916 */ be<u8> m7A2;
    /* 0x917 */ be<s8> mHandRJntNo;  /* was m7A3: createHeap 022BE334: the "handR" joint index of the body model; _draw 022C36D4 puts mpEtcModel on that joint */
    /* 0x918 */ be<u8> m7A4;
    /* 0x919 */ be<u8> m7A5;
    /* 0x91A */ be<u8> mPsoNo;  /* was m7A6: _draw 022C36D4: l_pso_tbl()[it] unless 0xFF; _execute 022C305C sets 0xFF, executeFurue / executeLetter / executeLook set it */
    /* 0x91B */ be<u8> m7A7;
    /* 0x91C */ be<u8> m7A8;
    /* 0x91D */ u8 _91D[3];
};
WWHD_OFFSET(daNpcPeople_c, mPhase, 0x7DC);
WWHD_OFFSET(daNpcPeople_c, mBtpAnm, 0x7FC);
WWHD_OFFSET(daNpcPeople_c, mPathRun, 0x870);
WWHD_OFFSET(daNpcPeople_c, mpNpcDat, 0x8A0);
WWHD_OFFSET(daNpcPeople_c, mEtcFlag, 0x8CC);
WWHD_OFFSET(daNpcPeople_c, mTargetAngleY, 0x8EE);
WWHD_OFFSET(daNpcPeople_c, mNpcNo, 0x905);
WWHD_OFFSET(daNpcPeople_c, mbIsNight, 0x90F);
WWHD_OFFSET(daNpcPeople_c, m7A8, 0x91C);
WWHD_SIZE(daNpcPeople_c, 0x920);

enum NpcNo_e {
    NPC_UO1, NPC_UO2, NPC_UO3, NPC_UB1, NPC_UB2, NPC_UB3, NPC_UB4, NPC_UW1, NPC_UW2, NPC_UM1,
    NPC_UM2, NPC_UM3, NPC_SA1, NPC_SA2, NPC_SA3, NPC_SA4, NPC_SA5, NPC_UG1, NPC_UG2,
};


// Views of common TU data; no second definitions of message/animation objects.
struct PeopleEventData {
    gptr<sUbMsgDat> l_msg_ub1_collect_map20[2];
    gptr<sUbMsgDat> l_msg_xy_ub1_no_photo[2];
    gptr<sUbMsgDat> l_msg_xy_ub1_have_map20[2];
    gptr<sUbMsgDat> l_msg_xy_ub1_get_map20_1[2];
    gptr<sUbMsgDat> l_msg_xy_ub1_get_map20_3[2];
    u8 _0028[0x1C8];
    sPeopleAnmDat l_npc_anm_bikkuri[2];
    u8 _01F6[0x2];
    sPeopleAnmDat l_npc_anm_miburui[2];
    u8 _01FE[0x12];
    sPeopleAnmDat l_npc_anm_kyoro2[2];
    u8 _0216[0x2];
    sPeopleAnmDat l_ub_anm_yada[2];
    u8 _021E[0x3A];
    sPeopleAnmDat l_npc_anm_talk4[2];
    u8 _025E[0x2];
    sPeopleAnmDat l_npc_anm_talkH[2];
    u8 _0266[0x2A];
    sPeopleAnmDat l_npc_anm_wait[1];
    sPeopleAnmDat l_npc_anm_talk[1];
    u8 _0296[0x3];
    sPeopleAnmDat l_npc_anm_furue[1];
    sPeopleAnmDat l_ub_anm_wait2[1];
    sPeopleAnmDat l_ub_anm_kuyasi[1];
    sPeopleAnmDat l_npc_anm_talk3[1];
    sPeopleAnmDat l_npc_anm_Mojimoji[1];
    sPeopleAnmDat l_npc_anm_happy[1];
    sPeopleAnmDat l_npc_anm_shobon_um[1];
    sPeopleAnmDat l_npc_anm_happy_um[1];
    sPeopleAnmDat l_npc_anm_happy2_um[1];
    sPeopleAnmDat l_npc_anm_um3_wait3[1];
    sPeopleAnmDat l_npc_anm_um3_talk2[1];
    sPeopleAnmDat l_npc_anm_um3_talk3[1];
    sPeopleAnmDat l_npc_anm_talk_sa[1];
    sPeopleAnmDat l_npc_anm_talk2_sa[1];
    sPeopleAnmDat l_npc_anm_talk3_sa[1];
    sPeopleAnmDat l_npc_anm_kiai_sa[1];
    sPeopleAnmDat l_npc_anm_talk2_ug[1];
    u8 _02CC[0x270];
    gptr<sPeopleAnmDat> l_npc_anm_ub1_tbl[8];
    gptr<sPeopleAnmDat> l_npc_anm_ub2_tbl[8];
    u8 _057C[0x158];
    gptr<sUbMsgDat> l_msg_ub1_collect_map20_1day[4];
    u8 _06E4[0x70];
    gptr<sUbMsgDat> l_msg_xy_ub1_get_map20_2[4];
    u8 _0764[0x310];
    be<s32> l_get_item_no[7];
    u8 _0A90[0x84];
    gptr<sUbMsgDat> l_msg_ub1_1st_talk[5];
    gptr<sUbMsgDat> l_msg_ub1_no_camera[3];
    gptr<sUbMsgDat> l_msg_ub1_no_collect_map20[5];
    u8 _0B48[0x410];
    gptr<char> l_npc_staff_id[19];
};
WWHD_OFFSET(PeopleEventData, l_msg_ub1_collect_map20, 0x0);
WWHD_OFFSET(PeopleEventData, l_msg_xy_ub1_no_photo, 0x8);
WWHD_OFFSET(PeopleEventData, l_msg_xy_ub1_have_map20, 0x10);
WWHD_OFFSET(PeopleEventData, l_msg_xy_ub1_get_map20_1, 0x18);
WWHD_OFFSET(PeopleEventData, l_msg_xy_ub1_get_map20_3, 0x20);
WWHD_OFFSET(PeopleEventData, l_npc_anm_bikkuri, 0x1F0);
WWHD_OFFSET(PeopleEventData, l_npc_anm_miburui, 0x1F8);
WWHD_OFFSET(PeopleEventData, l_npc_anm_kyoro2, 0x210);
WWHD_OFFSET(PeopleEventData, l_ub_anm_yada, 0x218);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk4, 0x258);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talkH, 0x260);
WWHD_OFFSET(PeopleEventData, l_npc_anm_wait, 0x290);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk, 0x293);
WWHD_OFFSET(PeopleEventData, l_npc_anm_furue, 0x299);
WWHD_OFFSET(PeopleEventData, l_ub_anm_wait2, 0x29C);
WWHD_OFFSET(PeopleEventData, l_ub_anm_kuyasi, 0x29F);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk3, 0x2A2);
WWHD_OFFSET(PeopleEventData, l_npc_anm_Mojimoji, 0x2A5);
WWHD_OFFSET(PeopleEventData, l_npc_anm_happy, 0x2A8);
WWHD_OFFSET(PeopleEventData, l_npc_anm_shobon_um, 0x2AB);
WWHD_OFFSET(PeopleEventData, l_npc_anm_happy_um, 0x2AE);
WWHD_OFFSET(PeopleEventData, l_npc_anm_happy2_um, 0x2B1);
WWHD_OFFSET(PeopleEventData, l_npc_anm_um3_wait3, 0x2B4);
WWHD_OFFSET(PeopleEventData, l_npc_anm_um3_talk2, 0x2B7);
WWHD_OFFSET(PeopleEventData, l_npc_anm_um3_talk3, 0x2BA);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk_sa, 0x2BD);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk2_sa, 0x2C0);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk3_sa, 0x2C3);
WWHD_OFFSET(PeopleEventData, l_npc_anm_kiai_sa, 0x2C6);
WWHD_OFFSET(PeopleEventData, l_npc_anm_talk2_ug, 0x2C9);
WWHD_OFFSET(PeopleEventData, l_npc_anm_ub1_tbl, 0x53C);
WWHD_OFFSET(PeopleEventData, l_npc_anm_ub2_tbl, 0x55C);
WWHD_OFFSET(PeopleEventData, l_msg_ub1_collect_map20_1day, 0x6D4);
WWHD_OFFSET(PeopleEventData, l_msg_xy_ub1_get_map20_2, 0x754);
WWHD_OFFSET(PeopleEventData, l_get_item_no, 0xA74);
WWHD_OFFSET(PeopleEventData, l_msg_ub1_1st_talk, 0xB14);
WWHD_OFFSET(PeopleEventData, l_msg_ub1_no_camera, 0xB28);
WWHD_OFFSET(PeopleEventData, l_msg_ub1_no_collect_map20, 0xB34);
WWHD_OFFSET(PeopleEventData, l_npc_staff_id, 0xF58);
inline PeopleEventData& peopleEventData() { return gabi::object_at<PeopleEventData>(0x101C2F24); }
struct PeopleEventPig : fopAc_ac_c { u8 _3AC[0x750 - 0x3AC]; be<u8> mTauraPosSet; /* was m408: view of kb_class::mTauraPosSet (d_a_kb.h) */ };
WWHD_OFFSET(PeopleEventPig, mTauraPosSet, 0x750);
struct PeopleEventCamera { u8 _000[0x510]; be<u32> mFlags; };
WWHD_OFFSET(PeopleEventCamera, mFlags, 0x510);
inline dComIfG_inf_c* peopleEventGame() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
inline be<u32>& peopleEventWord(be<f32>& value) { return *reinterpret_cast<be<u32>*>(&value); }
inline be<s32> (&peoplePigParams())[3] { return gabi::object_at<be<s32>[3]>(0x1002014C); }
struct PeopleEventVtable { u8 _00[0x1C]; gfn getMsg; };
inline u32 peopleEventGetMsg(daNpcPeople_c* self) { return gabi::call_ptr<u32>(gabi::at<PeopleEventVtable>(self->__vtbl)->getMsg.get(), self); }
struct PeopleAnmSetProc { gabi::ptmf<u32 (daNpcPeople_c::*)(int)> fn; };
fopAc_ac_c* peopleSearchFromName(const char*, u32, u32);
u32 peopleCreateItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void peopleCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*);
PeopleEventCamera* peopleCameraBody();
void peopleCameraScopeViewMsgModeOff(PeopleEventCamera*);
void peopleCameraSubjectLockOn(PeopleEventCamera*, u32);
void peopleCameraSetExtendedPosition(PeopleEventCamera*, cXyz*);
void peopleCameraSubjectLockOff(PeopleEventCamera*);
void dKy_get_moon_pos(cXyz*);
void dKy_get_orion_pos(cXyz*);
void dKy_get_hokuto_pos(cXyz*);
