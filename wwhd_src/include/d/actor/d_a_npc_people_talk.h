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
#include "gc_api/f_op/f_op_msg.h"


struct sUbMsgDat {
    /* 0x00 */ be<u32> mMsgNo;  /* was field_0x00: setMessageUb 022C1168 passes it to setMessage; next_msgStatus 022C6108 steps mpUbMsg to the next entry and ends the list at 0 (tww the same) */
    /* 0x04 */ be<u8> mAnmTag;  /* was field_0x04: setMessageUb 022C1168: the speaker's animation tag, passed to setAnmFromMsgTagUb (tww the same) */
    /* 0x05 */ be<u8> mPartnerAnmTag;  /* was field_0x05: setMessageUb 022C1168 copies it into daNpcPeople_c::mPartnerAnmTag, which the partner Ub plays in eventUb2Talk 022C2380 (tww the same) */
    /* 0x06 */ be<u8> mFacePlayer;  /* was field_0x06: setMessageUb 022C1168 copies it into daNpcPeople_c::mBodyTurn and mLookAtPlayer and, when 1, opens the attention to 1000 / 0x7FFF (else the npc dat ranges); keeps it in mPartnerFacePlayer for the partner Ub (tww the same) */
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(sUbMsgDat, 8);

struct sPeopleAnmDat {
    /* 0x00 */ be<u8> mAnmIdx;  /* was field_0x00: setAnmTbl 022BED70: the entry's animation index for setAnm (0xFF ends the table), compared with daNpcPeople_c::mAnmIdx; playAnm 022C2D6C replays it (tww the same); same code as sRotenAnmDat::mAnmIdx */
    /* 0x01 */ be<u8> mMorf;  /* was field_0x01: setAnmTbl 022BED70: the entry's morf frames, passed to setAnm (tww the same); same code as sRotenAnmDat::mMorf */
    /* 0x02 */ be<u8> mLoopCount;  /* was field_0x02: setAnmTbl 022BED70: the entry's count, copied into mAnmLoopCount; 0 loops, negative plays backwards (tww the same); same code as sRotenAnmDat::mLoopCount */
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
    /* 0x2C */ be<f32> mMaxLookDistXZ;  /* was field_0x2C: chkAttention 022C0084: outside the attention range but closer than it, the player is still looked at (mLookMode 1, mInLookDist set) (tww the same) */
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
    /* 0x890 */ cXyz mUgTargetPos;  /* was m71C: executeUgWalk 022C5434 / executeUgTurnInit 022C5AC4 set it (home.pos or current.pos + a direction times a distance, or a 50-unit side step); executeUgWalk walks toward it, executeUgTurnInit / executeUgTurn 022C5D4C turn to its angle; eventCameraStopInit 022C16A4 takes it as the camera's position unless UM3 looks at a star (tww the same) */
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
    /* 0x8F2 */ be<u16> mEventFlags;  /* was m77E: bit field: next_msgStatus 022C6108 sets 0x1..0x40 for message-list codes 1-5 / 7 / 9-0xA; when the box closes executeTalk 022C4404 orders the matching event (mOrderEventNum 3 / 7 / 8 / 9 / 0xC / 0xD / 0xF) and clears the bit, else sets 0x8000; chkEndEvent 022C0840 sets 0x8000 when the NPC's event ends; eventOrder 022C2A04 clears 0x8000 and resets the npc-data look state and the event (tww the same) */
    /* 0x8F4 */ be<s16> mBlinkTimer;  /* was m780: initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 counts it down and restarts the head texture pattern with 0x78 */
    /* 0x8F6 */ be<s16> mLookMaxVelTarget;  /* was m782: chkAttention 022C0084 / executeWalk 022C46D0 set npc dat 0x38 / 0x3A; lookBack 022C2E50 eases mLookMaxVel toward it (the turn speed limit) */
    /* 0x8F8 */ be<s16> mLookMaxVel;  /* was m784: lookBack 022C2E50 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel; createInit 022BF510 clears it */
    /* 0x8FA */ be<s16> mLookAngleY;  /* was m786: lookBack 022C2E50: the target angle in look mode 2; chkAttention 022C0084 sets mTargetAngleY, executeWalk 022C46D0 / executeTurn 022C4A04 / executeUgWalk 022C5434 / executeUgTurn 022C5D4C the path angle, executeFurue 022C4CA8 +-0x4000 (tww the same); same code as daNpcRoten_c::mLookAngleY */
    /* 0x8FC */ be<u8> mTalk;
    /* 0x8FD */ be<u8> mInAttention;  /* was m789: chkAttention 022C0084: 1 once the player is in the look range (distance +40 and angles +0x71C while set), 0 when they leave; executeWait 022C4074 / executeWalk 022C46D0 / executeKyoro 022C4E98 / checkPig 022C3E78 / setWaitAnm 022BEEDC test it (tww the same); same code as daNpcRoten_c::mInAttention */
    /* 0x8FE */ be<u8> mInLookDist;  /* was m78A: chkAttention 022C0084: cleared each frame, 1 while the player is outside the attention range but closer than npc dat mMaxLookDistXZ (he still looks at them); executeWait 022C4074 / executeWalk 022C46D0 / executeKyoro 022C4E98 / checkPig 022C3E78 treat it like mInAttention (tww the same) */
    /* 0x8FF */ be<u8> mCanTalk;  /* was m78B: chkAttention 022C0084: 1 while the player is in the talk range (or the event attention is on), else 0; executeCommon 022C3AC4 sets mOrderEventNum 1 (can talk) while it is set (tww the same); same code as daNpcRoten_c::mCanTalk */
    /* 0x900 */ be<u8> mNoTalk;
    /* 0x901 */ be<u8> mOrderEventNum;
    /* 0x902 */ be<u8> mBtpFrame;  /* was m78E: initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 advances and wraps it; _draw 022C36D4 passes it to mDoExt_btpAnm_entry */
    /* 0x903 */ be<u8> mExecuteMode;  /* was m78F: _execute 022C305C calls moveProc[it]; executeCommon 022C3AC4 / chkSurprise 022C39F0 test it */
    /* 0x904 */ be<u8> mResFlag;
    /* 0x905 */ be<u8> mNpcNo;
    /* 0x906 */ be<u8> mSa3ItemIdx;  /* was m792: next_msgStatus 022C6108: message-list code 6 stores the index that follows; it picks the event bit (l_item_chk_sa3[it]) and the item (l_item_id_sa3[it]) given (tww the same) */
    /* 0x907 */ be<u8> mAnmIdx;  /* was m793: setAnm 022BEB7C stores the animation index; createHeap 022BE334 loads the bck by it; setAnmTbl 022BED70 / setWaitAnm 022BEEDC / _execute 022C305C / getMsg 022C6C68 test it (tww the same); same code as daNpcRoten_c::mAnmIdx */
    /* 0x908 */ be<u8> mLastRandNo;  /* was m794: _execute 022C305C draws a random number again when it equals this one, then keeps it here */
    /* 0x909 */ be<u8> mAnmFlag;
    /* 0x90A */ be<s8> mAnmLoopCount;  /* was m796: setAnmTbl 022BED70 sets the table entry's count (sPeopleAnmDat +0x02); playAnm 022C2D6C counts it down before the next entry */
    /* 0x90B */ be<s8> mCutNo;  /* was m797: privateCut 022C25E8 stores the event cut index and switches on it */
    /* 0x90C */ be<u8> m798;
    /* 0x90D */ be<s8> mLookMode;  /* was m799: lookBack 022C2E50 switches on it: 1 looks at mLookAtPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022C0084 and the walk / turn / look / Ug modes set it (tww the same); same code as daNpcRoten_c::mLookMode */
    /* 0x90E */ be<u8> mSurpriseFlags;  /* was m79A: bit 0x1: executeBikkuriInit 022C4B30 sets it, executeFurue 022C4CA8 clears it when the shake ends, getMsg 022C6C68 clears it and picks l_msg_uo2_surprise for UO2; bit 0x2: executeFurue toggles it at random times and turns the look +0x4000 / -0x4000 by it (tww the same) */
    /* 0x90F */ be<u8> mbIsNight;
    /* 0x910 */ be<u8> mModeInTalk;  /* was m79C: _execute 022C305C runs the mode (not eventMove) during a talk only while set; executeCommon 022C3AC4 switches to the talk mode and returns 1 while talking with it set; executeTalkInit 022C43A8 tests it; XyEventCB 022BF220 / checkOrder 022C045C / executeCommon / executeTalk 022C4404 / createInit 022BF510 set or clear it, the constructor 022BE7CC sets 1 (tww the same); same role as daNpcPhoto_c::mModeInTalk */
    /* 0x911 */ be<u8> mBodyTurn;  /* was m79D: checkOrder 022C045C (talk start) copies npc dat 0x5B, createInit 022BF510 / eventOrder 022C2A04 0x5A, the Ug look modes set 1; while set, chkAttention 022C0084 and lookBack 022C2E50 (when talking) turn the body (mLookHeadOnly 0) (tww the same); same code as daNpcRoten_c::mBodyTurn */
    /* 0x912 */ be<u8> mLookAtPlayer;  /* was m79E: createInit 022BF510 / eventOrder 022C2A04 copy npc dat 0x5C, checkOrder 022C045C sets 1 at the talk start; chkAttention 022C0084: while 0 the player in range only turns him to mTargetAngleY instead of being looked at (tww the same); same role as daNpcPhoto_c::mLookAtPlayer */
    /* 0x913 */ be<u8> m79F;
    /* 0x914 */ be<u8> mPartnerFacePlayer;  /* was m7A0: setMessageUb 022C1168 stores the Ub message's sUbMsgDat::mFacePlayer there; eventUb2Talk 022C2380: the partner Ub copies it into its mBodyTurn and mLookAtPlayer and opens its attention to 1000 / 0x7FFF while it is 1 (tww the same); like mPartnerAnmTag */
    /* 0x915 */ be<u8> mCanMove;  /* was m7A1: createInit 022BF510 sets 1 for the path walkers and UG1; _execute 022C305C chases the walk speed and moves the actor (posMoveF, CrrPos) only while set; the constructor 022BE7CC clears it (tww the same) */
    /* 0x916 */ be<u8> mPartnerAnmTag;  /* was m7A2: setMessageUb 022C1168 stores the Ub message's partner animation tag (sUbMsgDat +0x05); eventUb2Talk 022C2380: the partner Ub plays it (setAnmFromMsgTagUb) once, then sets 0xFF (tww the same) */
    /* 0x917 */ be<s8> mHandRJntNo;  /* was m7A3: createHeap 022BE334: the "handR" joint index of the body model; _draw 022C36D4 puts mpEtcModel on that joint */
    /* 0x918 */ be<u8> m7A4;
    /* 0x919 */ be<u8> m7A5;
    /* 0x91A */ be<u8> mPsoNo;  /* was m7A6: _draw 022C36D4: l_pso_tbl()[it] unless 0xFF; _execute 022C305C sets 0xFF, executeFurue / executeLetter / executeLook set it */
    /* 0x91B */ be<u8> mSkyLookNo;  /* was m7A7: executeWait 022C4074: the sky object UM3's telescope look found, 0 the moon, 1 Orion, 2 the Big Dipper (dKy_moon / orion / hokuto_look_chk); getMsg3 022C1F20 picks the message list and eventCameraStopInit 022C16A4 the camera position by it (tww the same) */
    /* 0x91C */ be<u8> mMsgChecked;  /* was m7A8: talk2 022C1BBC: 1 once chkMsg has run for the shown message (typing after a continue or box open, or close wait); MSG_DISPLAYED and CLOSE_WAIT call chkMsg only while 0; a new message and MSG_DISPLAYED reset it (tww the same) */
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
WWHD_OFFSET(daNpcPeople_c, mMsgChecked, 0x91C);
WWHD_SIZE(daNpcPeople_c, 0x920);

enum NpcNo_e {
    NPC_UO1, NPC_UO2, NPC_UO3, NPC_UB1, NPC_UB2, NPC_UB3, NPC_UB4, NPC_UW1, NPC_UW2, NPC_UM1,
    NPC_UM2, NPC_UM3, NPC_SA1, NPC_SA2, NPC_SA3, NPC_SA4, NPC_SA5, NPC_UG1, NPC_UG2,
};


// Read-only views of common TU data, owned by the message/execution parts.
struct PeopleTalkData {
    be<u32> l_msg_uw1_talk_next[2];
    be<u32> l_msg_uw1_done_gp1_arasoi[2];
    be<u32> l_msg_uw2_request_yes[2];
    be<u32> l_msg_uw2_request_no[2];
    u8 _020[0x48];
    be<u32> l_msg_um3_no_nazo_talk2[2];
    u8 _070[0x60];
    be<u16> l_item_chk_sa3[4];
    u8 _0D8[0x18];
    be<u32> l_msg_sa4_night_yes[2];
    u8 _0F8[0x18];
    be<u32> l_msg_sa5_no[2];
    be<u32> l_msg_sa5_yes[2];
    be<u32> l_msg_sa5_wait[2];
    u8 _128[0x20];
    be<u32> l_msg_xy_sa5_wait[2];
    u8 _150[0xD8];
    sPeopleAnmDat l_npc_anm_wait[1];
    u8 _22B[0x501];
    be<u32> l_msg_uw1_talked_night[4];
    u8 _73C[0x30];
    be<u32> l_msg_um1_wind[8];
    u8 _78C[0x88];
    be<u32> l_msg_um3_nazo_talk[4];
    u8 _824[0xC0];
    be<s32> l_item_id_sa3[4];
    u8 _8F4[0xC0];
    be<u32> l_msg_sa4_night_no[4];
    u8 _9C4[0x38];
    be<u32> l_msg_xy_sa5_yes[4];
    u8 _A0C[0x104];
    be<u32> l_msg_uw1_get_pearl1[3];
    be<u32> l_msg_uw1_not_talked_night[3];
    u8 _B28[0x1BC];
    be<u32> l_msg_sa5_explain1[5];
    be<u32> l_msg_sa5_explain2[5];
    be<u32> l_msg_sa5_explain3[5];
    gptr<be<u32>> l_msg_sa5_explain[3];
    be<u32> l_msg_sa5_ok[3];
    u8 _D38[0x48];
    be<u32> l_msg_xy_sa5_no[3];
    be<u32> l_msg_xy_sa5_yes_ng[3];
    be<u32> l_msg_xy_sa5_explain[3];
    be<u32> l_msg_xy_sa5_ok[3];
};
WWHD_OFFSET(PeopleTalkData, l_msg_uw1_talk_next, 0x0);
WWHD_OFFSET(PeopleTalkData, l_msg_uw1_done_gp1_arasoi, 0x8);
WWHD_OFFSET(PeopleTalkData, l_msg_uw2_request_yes, 0x10);
WWHD_OFFSET(PeopleTalkData, l_msg_uw2_request_no, 0x18);
WWHD_OFFSET(PeopleTalkData, l_msg_um3_no_nazo_talk2, 0x68);
WWHD_OFFSET(PeopleTalkData, l_item_chk_sa3, 0xD0);
WWHD_OFFSET(PeopleTalkData, l_msg_sa4_night_yes, 0xF0);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_no, 0x110);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_yes, 0x118);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_wait, 0x120);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_wait, 0x148);
WWHD_OFFSET(PeopleTalkData, l_npc_anm_wait, 0x228);
WWHD_OFFSET(PeopleTalkData, l_msg_uw1_talked_night, 0x72C);
WWHD_OFFSET(PeopleTalkData, l_msg_um1_wind, 0x76C);
WWHD_OFFSET(PeopleTalkData, l_msg_um3_nazo_talk, 0x814);
WWHD_OFFSET(PeopleTalkData, l_item_id_sa3, 0x8E4);
WWHD_OFFSET(PeopleTalkData, l_msg_sa4_night_no, 0x9B4);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_yes, 0x9FC);
WWHD_OFFSET(PeopleTalkData, l_msg_uw1_get_pearl1, 0xB10);
WWHD_OFFSET(PeopleTalkData, l_msg_uw1_not_talked_night, 0xB1C);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_explain1, 0xCE4);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_explain2, 0xCF8);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_explain3, 0xD0C);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_explain, 0xD20);
WWHD_OFFSET(PeopleTalkData, l_msg_sa5_ok, 0xD2C);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_no, 0xD80);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_yes_ng, 0xD8C);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_explain, 0xD98);
WWHD_OFFSET(PeopleTalkData, l_msg_xy_sa5_ok, 0xDA4);
inline PeopleTalkData& peopleTalkData() { return gabi::object_at<PeopleTalkData>(0x101C2F8C); }
struct PeopleTalkVtable {
    u8 _00[0x14];
    gfn next_msgStatus;
    u8 _18[4];
    gfn getMsg;
    u8 _20[4];
    gfn anmAtr;
};
inline PeopleTalkVtable* peopleTalkVtable(daNpcPeople_c* self) { return gabi::at<PeopleTalkVtable>(self->__vtbl); }
// HD: next_msgStatus returns the full virtual-call register here.
inline u32 peopleNextStatus(daNpcPeople_c* self, be<u32>* no) { return gabi::call_ptr<u32>(peopleTalkVtable(self)->next_msgStatus.get(), self, no); }
inline u32 peopleGetMsg(daNpcPeople_c* self) { return gabi::call_ptr<u32>(peopleTalkVtable(self)->getMsg.get(), self); }
inline void peopleAnmAtr(daNpcPeople_c* self, u16 status) { gabi::call_ptr(peopleTalkVtable(self)->anmAtr.get(), self, status); }
struct PeopleTalkMessageManager {
    u8 _000[0x948];
    be<u32> mSelectNum;
};
WWHD_OFFSET(PeopleTalkMessageManager, mSelectNum, 0x948);
inline PeopleTalkMessageManager* peopleMessageManager() { return gabi::object_at<gptr<PeopleTalkMessageManager>>(0x101F4B5C); }
inline dComIfG_inf_c* peopleTalkGame() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
inline u8 peopleMessageLowByte(be<u32>* word) { return *gabi::at<be<u8>>(gabi::ea(word) + 3); }
inline const char* peopleUg1StaffId() { return gabi::object_at<gptr<const char>>(0x101C3EC0); }
struct PeopleInitProc { gabi::ptmf<s32 (daNpcPeople_c::*)()> fn; };
u32 peopleMsgGetStatus(PeopleTalkMessageManager*);
void peopleMsgSetStatus(PeopleTalkMessageManager*, u32);
u32 peopleMsgMessageSet(PeopleTalkMessageManager*, u32, cXyz*);
u32 peopleMsgScopeMessageSet(PeopleTalkMessageManager*, u32);
void peopleCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*);
u32 peopleCheckGetItemNum(u8);
fopAc_ac_c* peopleSearchFromName(const char*, u32, u32);
s16 peopleCalcTimer(be<s16>*);
void peopleLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void peoplePlayerEyePos(cXyz*, f32);
struct PeopleAnmVtable { u8 _00[0x14]; gfn getFrameMax; };
struct PeopleAnmView { u8 _00[4]; gptr<PeopleAnmVtable> __vtbl; };
inline s32 peopleFrameMax(J3DAnmTexPattern* anm) { PeopleAnmView* view = reinterpret_cast<PeopleAnmView*>(anm); return gabi::call_ptr<s32>(view->__vtbl->getFrameMax.get(), anm); }
