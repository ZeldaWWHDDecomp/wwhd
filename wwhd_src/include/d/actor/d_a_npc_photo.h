/* daNpcPhoto_c (Lenzo, Windfall photographer), WWHD layout.
 *
 * GameCube -> WWHD, measured from the constructor 022CC418 (allocates 0xB50) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm (0x7F0);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x6EC) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x6F0) on is +0x174. 0xB4E is an HD-only byte (zeroed by
 * the constructor). Size 0xB50 (GameCube 0x9DC). */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
#include "d/d_mesg_core.h"
#include "d/actor/d_a_tag_photo.h"

#define PHOTO_SAFESTRING_VTBL 0x10020C40 /* this TU's sead::SafeString vtable */
#define PHOTO_VTBL 0x10020FFC            /* daNpcPhoto_c vtable (constructor) */
#define PHOTO_AAB_VTBL 0x10020C58        /* this TU's cM3dGAab vtable */

struct sPhotoAnmDat {
    /* 0x00 */ be<u8> mAnmIdx;  /* was field_0x00: setAnmTbl 022CD0B8: the entry's animation index for setAnm (0xFF ends the table), compared with daNpcPhoto_c::mAnmIdx; playAnm 022CEE30 replays it (tww the same); same code as sRotenAnmDat::mAnmIdx */
    /* 0x01 */ be<u8> mMorf;  /* was field_0x01: setAnmTbl 022CD0B8: the entry's morf frames, passed to setAnm (tww the same); same code as sRotenAnmDat::mMorf */
    /* 0x02 */ be<u8> mLoopCount;  /* was field_0x02: setAnmTbl 022CD0B8: the entry's count, copied into mAnmLoopCount; 0 or less loops (tww the same); same code as sRotenAnmDat::mLoopCount */
};
WWHD_SIZE(sPhotoAnmDat, 3);

/* l_npc_dat (.data 0x101C521C) */
struct NpcDatStruct {
    /* 0x00 */ be<f32> mPlayerEyeOffsetY;  /* was field_0x00: chkAttention 022CCD4C: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mPlayerEyeOffsetY */
    /* 0x04 */ be<s16> mMaxHeadX;  /* was field_0x04: _execute 022CF17C: m_jnt.setParam's maxHeadX (5th argument; d_npc.h setParam order) (tww the same) */
    /* 0x06 */ be<s16> mMaxHeadY;  /* was field_0x06: _execute 022CF17C: m_jnt.setParam's maxHeadY (6th argument) (tww the same) */
    /* 0x08 */ be<s16> mMaxBackboneX;  /* was field_0x08: _execute 022CF17C: m_jnt.setParam's maxBackboneX (1st argument) (tww the same) */
    /* 0x0A */ be<s16> mMaxBackboneY;  /* was field_0x0A: _execute 022CF17C: m_jnt.setParam's maxBackboneY (2nd argument) (tww the same) */
    /* 0x0C */ be<s16> mMinHeadX;  /* was field_0x0C: _execute 022CF17C: m_jnt.setParam's minHeadX (7th argument) (tww the same) */
    /* 0x0E */ be<s16> mMinHeadY;  /* was field_0x0E: _execute 022CF17C: m_jnt.setParam's minHeadY (8th argument) (tww the same) */
    /* 0x10 */ be<s16> mMinBackboneX;  /* was field_0x10: _execute 022CF17C: m_jnt.setParam's minBackboneX (3rd argument) (tww the same) */
    /* 0x12 */ be<s16> mMinBackboneY;  /* was field_0x12: _execute 022CF17C: m_jnt.setParam's minBackboneY (4th argument) (tww the same) */
    /* 0x14 */ be<s16> mMaxTurnStep;  /* was field_0x14: _execute 022CF17C: m_jnt.setParam's maxTurnStep (9th argument) (tww the same) */
    /* 0x16 */ be<s16> mMaxHeadTurnVel;  /* was field_0x16: chkAttention 022CCD4C: mLookMaxVelTarget, the speed lookBack eases the look toward (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxHeadTurnVel */
    /* 0x18 */ be<s16> mWalkHeadTurnVel;  /* was field_0x18: executeWalk 022CFB4C: mLookMaxVelTarget while walking the path (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkHeadTurnVel */
    /* 0x1A */ u8 mPadding1A[2];
    /* 0x1C */ be<f32> mAttnOffsetX;  /* was field_0x1C: _execute 022CF17C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same) */
    /* 0x20 */ be<f32> mAttnOffsetY;  /* was field_0x20: _execute 022CF17C: the Y of the attention position offset (see mAttnOffsetX) (tww the same) */
    /* 0x24 */ be<f32> mAttnOffsetZ;  /* was field_0x24: _execute 022CF17C: the Z of the attention position offset (see mAttnOffsetX) (tww the same) */
    /* 0x28 */ be<s16> mMaxAttnAngleY;  /* was field_0x28: createInit 022CC8D0 copies it into daNpcPhoto_c::mMaxAttnAngleY (tww the same) */
    /* 0x2A */ u8 mPadding2A[2];
    /* 0x2C */ be<f32> mMaxAttnDistXZ;  /* was field_0x2C: chkAttention 022CCD4C: the player is in attention while closer than it (+40 while mInAttention) (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnDistXZ */
    /* 0x30 */ be<f32> mEyeYOffset;  /* was field_0x30: _execute 022CF17C: mEyePos = current.pos + (0, it, 0), then eyePos (tww the same); same role as daNpcRoten_c NpcDatStruct::mEyeYOffset */
    /* 0x34 */ be<f32> mCylOffsetZ;  /* was field_0x34: executeWait 022CF6C4: mCylOffset.z (with the 150 radius) until event bit 0x1701 is set (tww the same) */
    /* 0x38 */ be<f32> mWalkAnmRate;  /* was field_0x38: _execute 022CF17C: while walking (mAnmIdx 3) the morf's play speed is speedF * it, at least 0.5 (tww the same); same role as daNpcPeople_c__l_npc_dat::mWalkAnmRate */
    /* 0x3C */ be<f32> mWalkSpeed;  /* was field_0x3C: executeWalk 022CFB4C: mTargetSpeed while walking the path (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkSpeed */
    /* 0x40 */ be<f32> mClimbSpeedRate;  /* was field_0x40: executeWalk 022CFB4C: mTargetSpeed = mWalkSpeed * it while the next path point is more than 400 higher (tww the same) */
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ be<s16> field_0x46;
    /* 0x48 */ be<s16> field_0x48;
    /* 0x4A */ be<s16> mWaitTimeMin;  /* was field_0x4A: executeSetMode 022CD190 (mode 0): mWaitTimer = it + rnd(mWaitTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMin */
    /* 0x4C */ be<s16> mWaitTimeMax;  /* was field_0x4C: executeSetMode 022CD190 (mode 0): mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMax */
    /* 0x4E */ be<s16> mWalkTimeMin;  /* was field_0x4E: executeSetMode 022CD190 (modes 2 / 3): mWaitTimer = it + rnd(mWalkTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMin */
    /* 0x50 */ be<s16> mWalkTimeMax;  /* was field_0x50: executeSetMode 022CD190 (modes 2 / 3): mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMax */
    /* 0x52 */ be<u8> mBodyTurn;  /* was field_0x52: createInit 022CC8D0 copies it into daNpcPhoto_c::mBodyTurn (tww the same) */
    /* 0x53 */ be<u8> mLookAtPlayer;  /* was field_0x53: createInit 022CC8D0 copies it into daNpcPhoto_c::mLookAtPlayer (tww the same) */
};
WWHD_SIZE(NpcDatStruct, 0x54);

struct daNpcPhoto_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
        PRM_ARG0_W = 0x8,
        PRM_ARG0_S = 0x0,
    };

    request_of_phase_process_class* getPhaseP() { return &mPhs1; }

    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* bool, returned as loaded */
    void executeSetMode(u32); /* u8 */
    void executeWait();
    void executeTalk();
    void executeWalk();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventSeSetInit(int);
    void eventPosSetInit();
    void eventGetItemInit();
    bool eventGetItem();
    void eventSetAngleInit();
    void eventSetEyeInit();
    bool eventSetEye();
    void eventTurnToPlayerInit();
    bool eventTurnToPlayer();
    void eventClrHanmeInit();
    void eventGetPhotoInit();
    bool eventGetPhoto();
    void eventMesSetUbInit(int);
    u32 eventMesSetUb(); /* bool; returns eventMesSet's register as is */
    bool eventLookUb();
    u16 talk2(int);
    void setMsgCamera();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmRailID();
    u8 getPrmArg0();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32, int); /* bool */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 */
    bool setAnmTbl(sPhotoAnmDat*);
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL isPhotoOk();
    BOOL isPhotoDxOk();
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */

    /* 0x7DC */ request_of_phase_process_class mPhs1;
    /* 0x7E4 */ request_of_phase_process_class mPhs2;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x7F0 */ mDoExt_btpAnm mBtpAnm;          /* mDoExt_btpAnm (HD 0x74) */
    /* 0x864 */ dNpc_PathRun_c mPathRun;   // HD: no mShadowId before it
    /* 0x86C */ dCcD_Cyl mCounterCyl[2];  /* was field_0x6F8: _execute 022CF17C sets the two cylinders at l_counter_pos (radius 110, height 150) while mPlaceState is 2; executeWait 022CF6C4 sets mCounterHit when either touches the player; createInit 022CC8D0 Set(l_cyl_src2) (tww the same) */
    /* 0xACC */ cXyz mCylOffset;  /* was field_0x958: _execute 022CF17C / createInit 022CC8D0: the collision cylinder center is current.pos + it; executeWait 022CF6C4 sets (0, 0, npc dat 0x34) or clears it */
    /* 0xAD8 */ cXyz mLookAtPos;
    /* 0xAE4 */ cXyz mEyePos;
    /* 0xAF0 */ gptr<sPhotoAnmDat> mpAnmDat;
    /* 0xAF4 */ gptr<be<u32>> mpMsgList;  /* was field_0x980: eventMesSetInit 022CD580 points it at a message number list (l_msg_1st_talk_photo, ...) and sets *it; next_msgStatus 022CFE34 / getMsg 022D031C step through it */
    /* 0xAF8 */ be<f32> mTargetSpeed;  /* was field_0x984: _execute 022CF17C chases speedF to it (cLib_chaseF 0.1 / 0.5); executeWalk 022CFB4C sets npc dat 0x3C (* 0x40), executeSetMode 022CD190 clears it */
    /* 0xAFC */ be<f32> mCylR;  /* was field_0x988: the constructor 022CC418 sets 60; createInit 022CC8D0 passes it to setCollision as the radius */
    /* 0xB00 */ be<f32> mMorf;  /* was field_0x98C: setAnm 022CCFAC uses it as the morf frames once when not negative, then sets -1; the constructor 022CC418 sets -1 */
    /* 0xB04 */ be<s32> mMsgNno;
    /* 0xB08 */ be<u8> mLookHeadOnly;  /* was field_0x994: lookBack 022CEF70 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022CCD4C sets it (0 = turn the body), eventLookUb 022CE468 clears it (tww the same); same role as daNpcRoten_c::mLookHeadOnly */
    /* 0xB09 */ u8 _B09;
    /* 0xB0A */ be<s16> mPhotoLinkBackEventIdx;
    /* 0xB0C */ be<s16> mPhotoGetItemEventIdx;
    /* 0xB0E */ be<s16> mPhotoGetItem2EventIdx;
    /* 0xB10 */ be<s16> mPhotoGetPhotoEventIdx;
    /* 0xB12 */ be<s16> mPhotoGalleryEventIdx;
    /* 0xB14 */ be<s16> mPhotoCounterTalk0EventIdx;
    /* 0xB16 */ be<s16> mPhotoCounterTalk1EventIdx;
    /* 0xB18 */ be<s16> mPhotoDateUB4EventIdx;
    /* 0xB1A */ be<s16> mCounterTalkEventIdx;  /* was field_0x9A6: createInit 022CC8D0 copies mPhotoCounterTalk0EventIdx; eventMove 022CE97C waits for that event's end and then resets the talk (tww the same) */
    /* 0xB1C */ be<s16> mWaitTimer;  /* was field_0x9A8: executeSetMode 022CD190 sets npc dat 0x4A / 0x4E + random; executeWait 022CF6C4 counts it down before executeSetMode(3); createInit 022CC8D0 sets 1 (tww the same); same role as daNpcRoten_c::mWaitTimer */
    /* 0xB1E */ u8 _B1E[2];
    /* 0xB20 */ be<s16> mHomeTurnDelay;  /* was field_0x9AC: chkAttention 022CCD4C sets 30 when the player leaves the attention range and counts it down before turning back to mTargetAngleY (tww the same); same role as daNpcRoten_c::mHomeTurnDelay */
    /* 0xB22 */ be<s16> mTargetAngleY;  /* was field_0x9AE: the constructor 022CC418 sets home.angle.y; eventPosSetInit 022CD764 / eventTurnToPlayerInit 022CDCDC set it; eventTurnToPlayer 022CE434 ends when current.angle.y equals it */
    /* 0xB24 */ be<s16> mCutCounter;  /* was field_0x9B0: the counter of two event cuts: eventSetEyeInit 022CDC8C clears it, eventSetEye 022CE33C steps it by 0x400 as the sine phase of the eye sweep; eventGetPhotoInit 022CDD90 sets 10, eventGetPhoto 022CE44C counts it down and ends the cut at 0 (tww the same) */
    /* 0xB26 */ be<s16> mMaxAttnAngleY;  /* was field_0x9B2: createInit 022CC8D0 copies npc dat 0x28; chkAttention 022CCD4C: the player is in attention while the angle to them is below it (+0x71C while mInAttention) (tww the same) */
    /* 0xB28 */ be<s16> mTimer;
    /* 0xB2A */ be<s16> mLookMaxVelTarget;  /* was field_0x9B6: chkAttention 022CCD4C / executeWalk 022CFB4C set npc dat 0x16 / 0x18; lookBack 022CEF70 eases mLookMaxVel toward it */
    /* 0xB2C */ be<s16> mLookMaxVel;  /* was field_0x9B8: lookBack 022CEF70 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel */
    /* 0xB2E */ be<s16> mLookAngleY;  /* was field_0x9BA: lookBack 022CEF70: the target angle in look mode 2; chkAttention 022CCD4C sets mTargetAngleY, executeWalk 022CFB4C / executeTurn 022CFD7C the path angle (tww the same); same role as daNpcRoten_c::mLookAngleY */
    /* 0xB30 */ be<u8> mTalking;  /* was field_0x9BC: checkOrder 022CD3F8 sets it when the talk starts; executeCommon 022CF670 switches to the talk mode while 1 and returns it; executeTalk 022CFA00 / eventMove 022CE97C / eventOrder 022CEB58 clear it (tww the same); same role as daNpcRoten_c::mTalking */
    /* 0xB31 */ be<u8> mInAttention;  /* was field_0x9BD: chkAttention 022CCD4C: 1 once the player is in the attention range (distance +40 and angle +0x71C while set), 0 when they leave (tww the same); executeWait 022CF6C4 tests it; same pattern as daNpcRoten_c::mInAttention */
    /* 0xB32 */ be<u8> mOrderEvent;  /* was field_0x9BE: eventOrder 022CEB58 orders by it; checkOrder 022CD3F8 clears 3-6 once PHOTO_LINK_BACK / GET_ITEM / GET_ITEM2 / GET_PHOTO starts; executeWait 022CF6C4 / eventMove 022CE97C set it (tww the same); after daNpc_Cb1_c::mOrderEvent */
    /* 0xB33 */ be<u8> mFrame;
    /* 0xB34 */ be<u8> mExecuteMode;  /* was field_0x9C0: executeSetMode 022CD190 stores the mode; _execute 022CF17C calls moveProc[it]; chkAttention 022CCD4C tests 0 (tww the same); same role as daNpcRoten_c::mExecuteMode */
    /* 0xB35 */ be<u8> mPlaceState;  /* was field_0x9C1: Lenzo's situation: the constructor 022CC418 0; phase_1 022CC4F8 4 (date, tww isDate() == 4: _draw 022CF534 registers the snap object only then); eventPosSetInit 022CD764 1; executeWait 022CF6C4 3 (player went down); executeWalk 022CFB4C 2 (at the counter, mCounterCyl on); getMsg 022D031C picks messages by it (tww the same) */
    /* 0xB36 */ be<u8> field_0x9C2;
    /* 0xB37 */ be<u8> field_0x9C3;
    /* 0xB38 */ be<u8> field_0x9C4;
    /* 0xB39 */ be<u8> mItemNo;
    /* 0xB3A */ be<u8> mEventFlags;  /* was field_0x9C6: bit field: next_msgStatus 022CFE34 sets 0x1 / 0x2 / 0x20 and getMsg 022D031C 0x4; when the box closes executeTalk 022CFA00 turns 0x1 / 0x2 into the item event (mOrderEvent 4) and 0x4 into the spit event (3), else sets 0x40, and talk2 022CDFAC turns 0x20 into 0x40; eventMove 022CE97C sets 0x40 / 0x50; eventOrder 022CEB58 clears 0x40 and resets the event; executeWait 022CF6C4 tests 0x10 (tww the same); same role as daNpcRoten_c::mEventFlags / daNpcPeople_c::mEventFlags */
    /* 0xB3B */ be<u8> mModeInTalk;  /* was field_0x9C7: _execute 022CF17C runs the mode (not eventMove) during a talk only while set; the constructor 022CC418, executeWait 022CF6C4, eventMove 022CE97C, eventOrder 022CEB58 set it, XyEventCB 022CC690 clears it (tww the same) */
    /* 0xB3C */ be<u8> mAnmIdx;  /* was field_0x9C8: setAnm 022CCFAC stores the animation index; createHeap 022CC160 loads the bck by it; setAnmTbl 022CD0B8 / _execute 022CF17C test it (tww the same); same role as daNpcRoten_c::mAnmIdx */
    /* 0xB3D */ be<u8> mAnmFlags;  /* was field_0x9C9: bit field: 0x1 playAnm 022CEE30 sets it when the animation table ends (cleared each frame); 0x4 setAnmTbl 022CD0B8 keeps the table while set, executeTalk 022CFA00 sets it with the spit table, playAnm clears it when the animation ends; 0x80 setAnmFromMsgTag 022CE7B0 sets it and playTexPatternAnm 022CED84 stops the blink while set, eventClrHanmeInit 022CDD50 / eventOrder 022CEB58 clear it (tww the same) */
    /* 0xB3E */ be<s8> mAnmLoopCount;  /* was field_0x9CA: setAnmTbl 022CD0B8 sets the table entry's count (+0x02); playAnm 022CEE30 counts it down before the next entry */
    /* 0xB3F */ be<s8> mActIdx;
    /* 0xB40 */ be<u8> mPhotoAccepted;  /* was field_0x9CC: next_msgStatus 022CFE34 sets it at message-list marker 6, where the shown picture is erased (dPb_erasePicture) and mEventFlags 0x1 orders the item event; never read in WWHD (HD has no l_msg_3rd_order_end); tww getMsg gives l_msg_3rd_order_end instead of l_msg_not_color while it is set */
    /* 0xB41 */ be<u8> mCounterHit;  /* was field_0x9CD: executeWait 022CF6C4 clears it, then sets it when an mCounterCyl touches the player; chkAttention 022CCD4C treats the player as in attention while set (tww the same) */
    /* 0xB42 */ u8 _B42[2];
    /* 0xB44 */ gptr<be<s8>> mpMsgCameraList;  /* was field_0x9D0: eventMesSetInit 022CD580 points it at the message's camera index list (or 0) and steps it; setMsgCamera 022CDE3C reads the camera from it (tww the same); u8* (camera index list) */
    /* 0xB48 */ be<u8> mMsgCameraSet;  /* was field_0x9D4: setMsgCamera 022CDE3C: 1 when it set the message camera (dCamera_Set), 0 when it restarted the normal camera; talk2 022CDFAC restarts the camera at the box close while set (tww the same) */
    /* 0xB49 */ be<u8> mMsgCameraPending;  /* was field_0x9D5: talk2 022CDFAC sets 1 when a message is set and on every status but typing; on MSG_TYPING it clears it and calls setMsgCamera once (tww the same) */
    /* 0xB4A */ be<s8> mLookMode;  /* was field_0x9D6: lookBack 022CEF70 switches on it: 1 looks at mLookAtPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022CCD4C / eventLookUb 022CE468 set it (tww the same); same role as daNpcRoten_c::mLookMode */
    /* 0xB4B */ be<u8> mBodyTurn;  /* was field_0x9D7: createInit 022CC8D0 copies npc dat 0x52, eventMove 022CE97C sets it after PHOTO_DATE_UB4; while set chkAttention 022CCD4C and lookBack 022CEF70 (when talking) turn the body (mLookHeadOnly 0) (tww the same); same role as daNpcRoten_c::mBodyTurn */
    /* 0xB4C */ be<u8> mLookAtPlayer;  /* was field_0x9D8: createInit 022CC8D0 copies npc dat 0x53, eventMove 022CE97C sets it after PHOTO_DATE_UB4; chkAttention 022CCD4C: while 0 the player in range only turns him to mTargetAngleY instead of being looked at (tww the same) */
    /* 0xB4D */ u8 _B4D;
    /* 0xB4E */ be<u8> mReplaceDateMessages;            /* HD only: zeroed by the constructor */
    /* 0xB4F */ u8 _B4F;
};
WWHD_OFFSET(daNpcPhoto_c, mPhs1, 0x7DC);
WWHD_OFFSET(daNpcPhoto_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpcPhoto_c, mPathRun, 0x864);
WWHD_OFFSET(daNpcPhoto_c, mCounterCyl, 0x86C);
WWHD_OFFSET(daNpcPhoto_c, mCylOffset, 0xACC);
WWHD_OFFSET(daNpcPhoto_c, mMsgNno, 0xB04);
WWHD_OFFSET(daNpcPhoto_c, mTargetAngleY, 0xB22);
WWHD_OFFSET(daNpcPhoto_c, mFrame, 0xB33);
WWHD_OFFSET(daNpcPhoto_c, mpMsgCameraList, 0xB44);
WWHD_OFFSET(daNpcPhoto_c, mReplaceDateMessages, 0xB4E);
WWHD_SIZE(daNpcPhoto_c, 0xB50);


// HD fields beyond the shared declarations, measured from the original accesses.
struct PhotoSelectedPicture {
    u8 mReserved[0x5BEB];
    be<u8> mSlot;
};
struct PhotoPlayerDemoAngle {
    u8 mReserved[0x422];
    be<s16> mDemoMoveAngle;
};
struct PhotoModelMaterialState {
    u8 mReserved[0x38];
    be<u32> mTexturePattern;
};
struct PhotoPictureSave {
    u8 mReserved[0x12C0];
    u8 mPictureData;
};
struct PhotoPictureAlbum {
    u8 mReserved[0x3C030C];
    be<u8> mPictureNum;
};
u32 photo_getPictureSave(u32);
u32 photo_getFreePictureSlot(u32);
void photo_markPictureSlot(u32, u32);
u32 photo_getPicture(u32, u32);
void photo_setPictureDay(u32, s32);
void photo_erasePicture(u32, u8);
u32 photo_eventMesSetRegister(daNpcPhoto_c*);
void photo_actorConstruct(fopNpc_npc_c*);
void photo_destroyArray(void*, u32, u32, u32, u32, u32);
void photo_actorDestroy(fopAc_ac_c*, s32);
s8 photo_eventGetActIdx(dEvent_manager_c*, s32, u32, s32, BOOL, s32);
u32 photo_runEventName(dEvent_manager_c*);
inline u32 photo_album() {
    return photo_getPictureSave(gabi::ea(&gabi::object_at<PhotoPictureSave>(dComIfGs_ea()).mPictureData));
}

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h) ---- */
/* save info: event flags at *(0x101F84DC) + 0x644, temporary flags at + 0x1178 */


/* 02520C0C dComIfGs_checkGetItem(u8) */

// HD: dComIfGs_getPictureNum() is a byte of the picture save data (02720144 returns it)
static inline u8 photo_getPictureNum() { return gabi::object_at<PhotoPictureAlbum>(photo_album()).mPictureNum; }


static inline s8 photo_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return photo_eventGetActIdx(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* resources by id (HD: sead::SafeString key, per-TU vtable) */

struct PhotoAnimationVtable {
    u8 mReserved[0x14];
    gfn getFrameMax;
};
struct PhotoAnimation {
    be<f32> mFrame;
    gptr<PhotoAnimationVtable> __vtbl;
};

static inline s32 J3DAnm_getFrameMax(void* p) {
    auto& animation = gabi::object_at<PhotoAnimation>(gabi::ea(p));
    return gabi::call_ptr<s32>(animation.__vtbl->getFrameMax.get(), p);
}

s32 mDoExt_btpAnm_init(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);


void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */

/* dNpc_PathRun_c */


/* HD message manager (*(0x101F4B5C)) */
static inline u32 photo_msgManager() { return gabi::object_at<be<u32>>(0x101F4B5C); }
u32 msgMng_getStatus(u32);
void msgMng_setStatus(u32, u32);
u32 msgMng_messageSet(u32, u32, cXyz*);
/* J3DModelData (HD) */

s8 JUTNameTab_getIndex(u32, const char*);


/* eventInfo (fopAc_ac_c + 0xF8): mCommand u16 at 0xF8, mEventId s16 at 0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return a->eventInfo.mCommand; }
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) { a->eventInfo.mEventId = id; }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
fopAc_ac_c* fopAcM_searchFromName(const char*, u32, u32);
/* 02554288 dKy_get_dayofweek */
s32 dKy_get_dayofweek();
/* dEvt_control_c (play + 0x51D0): mPtTalk at +0xCC; 0253F124 getPId */


// HD: one TU owns these guest tables; all four source parts share their references.
extern be<s32> (&l_bck_ix_tbl)[5];
extern be<s32> (&l_btp_ix_tbl)[2];
extern sPhotoAnmDat (&l_npc_anm_wait)[1];
extern sPhotoAnmDat (&l_npc_anm_talk)[1];
extern sPhotoAnmDat (&l_npc_anm_walk)[1];
extern sPhotoAnmDat (&l_npc_anm_spit)[2];
extern sPhotoAnmDat (&l_npc_anm_talk2)[2];

extern be<u32> (&l_msg_1st_talk)[4];
extern be<u32> (&l_msg_2nd_talk)[2];
extern be<u32> (&l_msg_1st_photo)[3];
extern be<u32> (&l_msg_2nd_photo)[3];
extern be<u32> (&l_msg_1st_order)[11];
extern be<s16> (&l_msg_1st_order_c)[5];
extern be<u32> (&l_msg_1st_order_not_end)[3];
extern be<u16> (&l_msg_1st_order_not_end_c)[1];
extern be<u32> (&l_msg_2nd_order)[4];
extern be<u8> (&l_msg_2nd_order_c)[3];
extern be<u32> (&l_msg_2nd_order_not_end)[3];
extern be<u16> (&l_msg_2nd_order_not_end_c)[1];
extern be<u32> (&l_msg_3rd_order)[5];
extern be<u32> (&l_msg_3rd_order_c)[1];
extern be<u32> (&l_msg_3rd_order_not_end)[3];
extern be<u16> (&l_msg_3rd_order_not_end_c)[1];
extern be<u32> (&l_msg_color)[3];
extern be<u16> (&l_msg_color_c)[1];
extern be<u32> (&l_msg_week_1st)[6];
extern be<u32> (&l_msg_week)[7];
extern be<u32> (&l_msg_buy_photo)[2];
extern be<u32> (&l_msg_get_photo)[2];
extern be<u32> (&l_msg_down)[2];
extern be<u32> (&l_msg_ub4)[2];
extern be<u32> (&l_msg_1st_order_xy)[4];
extern be<u32> (&l_msg_2nd_order_xy)[4];
extern be<u32> (&l_msg_3rd_order_xy)[3];
extern be<u32> (&l_msg_color_xy)[4];
extern be<u32> (&l_msg_xy_buy_photo)[9];
extern cXyz (&l_counter_pos)[2];
extern cXyz (&l_gallery_pos)[2];
extern cXyz (&l_msg_camera)[6];

// Component access keeps sinit's original scalar store order.
inline be<f32>& photo_positionComponent(cXyz& position, int axis) {
    if (axis == 0) return position.x;
    if (axis == 1) return position.y;
    return position.z;
}

enum dRes_ID_PO {
    dRes_ID_PO_BCK_WAIT01_e = 0,
    dRes_ID_PO_BCK_SPIT_e = 4,
    dRes_ID_PO_BCK_TALK01_e = 5,
    dRes_ID_PO_BCK_TALK02_e = 6,
    dRes_ID_PO_BCK_WALK01_e = 7,
    dRes_ID_PO_BDL_PO_e = 1,
    dRes_ID_PO_BTP_MABA01_e = 2,
    dRes_ID_PO_BTP_MABA02_e = 3,
};
extern NpcDatStruct& l_npc_dat;
extern const gptr<char> (&l_arcname_tbl)[1];
extern const gptr<char>& l_npc_staff_id;

// HD out-of-line bindings retain the verified widths of this unit.
struct dSnap_Obj;
void dSnap_Obj_ct(dSnap_Obj*);
void dSnap_Obj_SetInf(dSnap_Obj*, u8, fopAc_ac_c*, u8, u8, s16);
void dSnap_Obj_SetGeo(dSnap_Obj*, cXyz*, f32, f32, s16);
void dSnap_RegistSnapObj(dSnap_Obj*);
cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class*, u32, void*);
void __construct_array(void*, u32, u32, u32);
void __destroy_arr(void*, u32, u32, u32, u32);
void mDoAud_seStart_1(u32);
dPath* dPath_GetNextRoomPath(dPath*, s32);
u32 dPath_GetPnt(dPath*, s32);
u32 fopAcM_createItemForPresentDemo(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
void cLib_addCalcPosXZ(cXyz*, const cXyz*, f32, f32, f32);
u32 dCam_getBody();
void dCamera_Stop(u32);
void dCamera_Start(u32);
void dCamera_Set(u32, cXyz*, cXyz*, f32, s16);

// HD: the joint returned by J3DNode_toJoint exposes its number at this offset.
struct PhotoJointNumber {
    be<u32> mData;
    be<u16> mNumber;
};
struct PhotoSafeStringVtable {
    u8 mReserved[0x14];
    gfn assureTermination;
};


struct PhotoMessageVtable {
    u8 mReserved[0x14];
    gfn nextMsgStatus;
    u8 mReserved18[4];
    gfn getMsg;
    u8 mReserved20[4];
    gfn setAnmAtr;
};

extern be<u32> (&l_msg_1st_talk_photo)[5];
extern be<u32> (&l_msg_talk_photo)[7];
extern be<u32> (&l_msg_2F)[3];

struct PsoData {
    /* 0x00 */ be<f32> mOffsetX;  /* was field_0x00: _draw 022CF534: the snap object's center = current.pos + (mOffsetX, mOffsetY, mOffsetZ) (tww the same); same role as PsoData_l::mOffsetX (people) */
    /* 0x04 */ be<f32> mOffsetY;  /* was field_0x04: _draw 022CF534: the Y of the snap object's offset (see mOffsetX) (tww the same) */
    /* 0x08 */ be<f32> mOffsetZ;  /* was field_0x08: _draw 022CF534: the Z of the snap object's offset (see mOffsetX) (tww the same) */
    /* 0x0C */ be<f32> mRadius;  /* was field_0x0C: _draw 022CF534: dSnap_Obj_SetGeo's radius (tww d_snap.h SetGeo(center, radius, height, ..)) */
    /* 0x10 */ be<f32> mHeight;  /* was field_0x10: _draw 022CF534: dSnap_Obj_SetGeo's height (tww d_snap.h SetGeo) */
    /* 0x14 */ be<s16> mAngleY;  /* was field_0x14: _draw 022CF534: + current.angle.y, dSnap_Obj_SetGeo's angle (tww the same) */
    /* 0x16 */ be<u8> field_0x16;
    /* 0x17 */ be<u8> mCylDivNum;  /* was field_0x17: _draw 022CF534 passes it to dSnap_Obj_SetInf as the second u8; tww dSnap_Obj::SetInf stores it in field_0x19, the cylinder segment count CalcArea2D / draw loop over (for i < it, cylvtx[i]) */
};
WWHD_SIZE(PsoData, 0x18);
extern PsoData& l_pso_photo;
struct PhotoCylinderSource { be<u32> mWords[17]; };
extern PhotoCylinderSource& l_cyl_src2;
struct PhotoPhaseHandler { gfn handler; };
extern PhotoPhaseHandler (&l_method)[3];
void photo_cylElementDestroy(dCcD_Cyl*, s32);

// GameCube enum entries used here but not yet present in the shared subsets.
enum {
    fpcNm_PLAYER_e = 0xA8,
    dItemNo_PICTO_BOX_e = 0x23,
    dItemNo_DELUXE_PICTO_BOX_e = 0x26,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_COLLECT_MAP_20_e = 0xEB,
    fopMsgStts_MSG_TYPING_e = 6,
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
    DSNAP_TYPE_UNK05 = 5,
    DSNAP_TYPE_NPC_PHOTO = 0x5E,
    PHOTO_HD_ATTENTION = 0x01000000,
};
